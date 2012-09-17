/****************************************************************************/
/// @file    SUMOVehicleClass.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Walter Bamberger
/// @date    2006-01-24
/// @version $Id$
///
// Definitions of SUMO vehicle classes and helper functions
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <map>
#include "SUMOVehicleClass.h"
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>


#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static members
// ===========================================================================
StringBijection<SUMOVehicleClass>::Entry sumoVehicleClassStringInitializer[] = {
    {"unknown",           SVC_UNKNOWN},
    {"private",           SVC_PRIVATE},
    {"public_transport",  SVC_PUBLIC_TRANSPORT},
    {"public_emergency",  SVC_PUBLIC_EMERGENCY},
    {"public_authority",  SVC_PUBLIC_AUTHORITY},
    {"public_army",       SVC_PUBLIC_ARMY},
    {"vip",               SVC_VIP},
    {"ignoring",          SVC_IGNORING},
    {"passenger",         SVC_PASSENGER},
    {"hov",               SVC_HOV},
    {"taxi",              SVC_TAXI},
    {"bus",               SVC_BUS},
    {"delivery",          SVC_DELIVERY},
    {"transport",         SVC_TRANSPORT},
    {"lightrail",         SVC_LIGHTRAIL},
    {"cityrail",          SVC_CITYRAIL},
    {"rail_slow",         SVC_RAIL_SLOW},
    {"rail_fast",         SVC_RAIL_FAST},
    {"motorcycle",        SVC_MOTORCYCLE},
    {"bicycle",           SVC_BICYCLE},
    {"pedestrian",        SVC_PEDESTRIAN}
};

StringBijection<SUMOVehicleClass> SumoVehicleClassStrings(
    sumoVehicleClassStringInitializer, SVC_PEDESTRIAN);


StringBijection<SUMOVehicleShape>::Entry sumoVehicleShapeStringInitializer[] = {
    {"pedestrian",            SVS_PEDESTRIAN},
    {"bicycle",               SVS_BICYCLE},
    {"motorcycle",            SVS_MOTORCYCLE},
    {"passenger",             SVS_PASSENGER},
    {"passenger/sedan",       SVS_PASSENGER_SEDAN},
    {"passenger/hatchback",   SVS_PASSENGER_HATCHBACK},
    {"passenger/wagon",       SVS_PASSENGER_WAGON},
    {"passenger/van",         SVS_PASSENGER_VAN},
    {"delivery",              SVS_DELIVERY},
    {"transport",             SVS_TRANSPORT},
    {"transport/semitrailer", SVS_TRANSPORT_SEMITRAILER},
    {"transport/trailer",     SVS_TRANSPORT_1TRAILER},
    {"bus",                   SVS_BUS},
    {"bus/city",              SVS_BUS_CITY},
    {"bus/flexible",          SVS_BUS_CITY_FLEXIBLE},
    {"bus/overland",          SVS_BUS_OVERLAND},
    {"bus/trolley",           SVS_BUS_TROLLEY},
    {"rail",                  SVS_RAIL},
    {"rail/light",            SVS_RAIL_LIGHT},
    {"rail/city",             SVS_RAIL_CITY},
    {"rail/slow",             SVS_RAIL_SLOW},
    {"rail/fast",             SVS_RAIL_FAST},
    {"rail/cargo",            SVS_RAIL_CARGO},
    {"evehicle",              SVS_E_VEHICLE},
    {"ant",                   SVS_ANT},
    {"",                      SVS_UNKNOWN}
};


StringBijection<SUMOVehicleShape> SumoVehicleShapeStrings(
    sumoVehicleShapeStringInitializer, SVS_UNKNOWN);

const int SUMOVehicleClass_MAX = SVC_PEDESTRIAN;
const SVCPermissions SVCFreeForAll = std::numeric_limits<SVCPermissions>::max(); // all bits set to 1

// ===========================================================================
// method definitions
// ===========================================================================
// ------------ Conversion of SUMOVehicleClass

std::string
getVehicleClassCompoundName(int id) {
    std::string ret;
    const std::vector<std::string> names = SumoVehicleClassStrings.getStrings();
    for (std::vector<std::string>::const_iterator it = names.begin(); it != names.end(); it++) {
        if ((id & SumoVehicleClassStrings.get(*it))) {
            ret += ("|" + *it);
        }
    }
    if (ret.length() > 0) {
        return ret.substr(1);
    } else {
        return ret;
    }
}


std::string 
getAllowedVehicleClassNames(SVCPermissions permissions) {
    std::ostringstream oss;
    const std::vector<std::string> classNames = getAllowedVehicleClassNamesList(permissions);
    bool hadOne = false;
    for (std::vector<std::string>::const_iterator it = classNames.begin(); it != classNames.end(); it++) {
        if (hadOne) {
            oss << ' ';
        }
        oss << *it;
        hadOne = true;
    }
    return oss.str();
}


std::vector<std::string>
getAllowedVehicleClassNamesList(SVCPermissions permissions) {
    /// @todo cache values?
    const std::vector<std::string> classNames = SumoVehicleClassStrings.getStrings();
    std::vector<std::string> result;
    for (std::vector<std::string>::const_iterator it = classNames.begin(); it != classNames.end(); it++) {
        const int svc = (int)SumoVehicleClassStrings.get(*it);
        if ((svc & permissions) == svc && svc != SVC_UNKNOWN) {
            result.push_back(*it);
        }
    }
    return result;
}


std::pair<std::string, bool> 
getPermissionEncoding(SVCPermissions permissions) {
    // shortcut the common cases
    if (permissions == SVCFreeForAll) {
        return std::pair<std::string, bool>("", false); // nothing disallowed
    }
    // figure out whether its shorter to write allow or disallow
    size_t num_allowed = 0;
    for(int mask = 1; mask <= SUMOVehicleClass_MAX; mask = mask << 1) {
        if ((mask & permissions) == mask) {
            ++num_allowed;
        }
    }
    if (num_allowed <= (SumoVehicleClassStrings.size() - num_allowed) && num_allowed > 0) {
        return std::pair<std::string, bool>(getAllowedVehicleClassNames(permissions), true);
    } else {
        return std::pair<std::string, bool>(getAllowedVehicleClassNames(~permissions), false);
    }
}


SUMOVehicleClass
getVehicleClassID(const std::string& name) {
    if (SumoVehicleClassStrings.hasString(name)) {
        return SumoVehicleClassStrings.get(name);
    }
    throw ProcessError("Unknown vehicle class '" + name + "'.");
}


int
getVehicleClassCompoundID(const std::string& name) {
    int ret = SVC_UNKNOWN;
    const std::vector<std::string> names = SumoVehicleClassStrings.getStrings();
    for (std::vector<std::string>::const_iterator it = names.begin(); it != names.end(); it++) {
        if (name.find(*it) != std::string::npos) {
            ret = ret | (int) SumoVehicleClassStrings.get(*it);
        }
    }
    return ret;
}


SVCPermissions 
parseVehicleClasses(const std::string& allowedS) {
    SVCPermissions result = 0;
    StringTokenizer sta(allowedS, " ");
    while (sta.hasNext()) {
        result |= getVehicleClassID(sta.next());
    }
    return result;
}


bool 
canParseVehicleClasses(const std::string& classes) {
    StringTokenizer sta(classes, " ");
    while (sta.hasNext()) {
        if (!SumoVehicleClassStrings.hasString(sta.next())) {
            return false;
        }
    }
    return true;
}


extern SVCPermissions parseVehicleClasses(const std::string& allowedS, const std::string& disallowedS) {
    if (allowedS.size() == 0 && disallowedS.size() == 0) {
        return SVCFreeForAll;
    } else if (allowedS.size() > 0 && disallowedS.size() > 0) {
        WRITE_WARNING("SVCPermissions must be specified either via 'allow' or 'disallow'. Ignoring 'disallow'");
        return parseVehicleClasses(allowedS);
    } else if (allowedS.size() > 0) {
        return parseVehicleClasses(allowedS);
    } else {
        return ~parseVehicleClasses(disallowedS);
    }
}


SVCPermissions 
parseVehicleClasses(const std::vector<std::string> &allowedS) {
    SVCPermissions result = 0;
    for (std::vector<std::string>::const_iterator i = allowedS.begin(); i != allowedS.end(); ++i) {
        result |= getVehicleClassID(*i);
    }
    return result;
}


SUMOVehicleShape
getVehicleShapeID(const std::string& name) {
    if (SumoVehicleShapeStrings.hasString(name)) {
        return SumoVehicleShapeStrings.get(name);
    } else {
        throw ProcessError("Unknown vehicle shape '" + name + "'.");
    }
}


std::string
getVehicleShapeName(SUMOVehicleShape id) {
    return SumoVehicleShapeStrings.getString(id);
}


// ------------ Conversion of SUMOEmissionClass
SUMOEmissionClass
getVehicleEmissionTypeID(const std::string& name) {
    try {
        if (name == "") {
            return SVE_UNKNOWN;
        } else if (name == "zero") {
            return SVE_ZERO_EMISSIONS;
        } else if (name.find("HDV_3_") == 0) {
            return (SUMOEmissionClass)(SVE_HDV_3_1 - 1 + TplConvert::_2int(name.substr(name.rfind("_") + 1).c_str()));
        } else if (name.find("HDV_6_") == 0) {
            return (SUMOEmissionClass)(SVE_HDV_6_1 - 1 + TplConvert::_2int(name.substr(name.rfind("_") + 1).c_str()));
        } else if (name.find("HDV_12_") == 0) {
            return (SUMOEmissionClass)(SVE_HDV_12_1 - 1 + TplConvert::_2int(name.substr(name.rfind("_") + 1).c_str()));
        } else if (name.find("P_7_") == 0) {
            return (SUMOEmissionClass)(SVE_P_LDV_7_1 - 1 + TplConvert::_2int(name.substr(name.rfind("_") + 1).c_str()));
        } else if (name.find("P_14_") == 0) {
            return (SUMOEmissionClass)(SVE_P_LDV_14_1 - 1 + TplConvert::_2int(name.substr(name.rfind("_") + 1).c_str()));
        } else if (name.find("HDV_A0_3_") == 0) {
            return (SUMOEmissionClass)(SVE_HDV_A0_3_1 - 1 + TplConvert::_2int(name.substr(name.rfind("_") + 1).c_str()));
        } else if (name.find("HDV_A0_6_") == 0) {
            return (SUMOEmissionClass)(SVE_HDV_A0_6_1 - 1 + TplConvert::_2int(name.substr(name.rfind("_") + 1).c_str()));
        } else if (name.find("HDV_A0_12_") == 0) {
            return (SUMOEmissionClass)(SVE_HDV_A0_12_1 - 1 + TplConvert::_2int(name.substr(name.rfind("_") + 1).c_str()));
        } else if (name.find("P_A0_7_") == 0) {
            return (SUMOEmissionClass)(SVE_P_LDV_A0_7_1 - 1 + TplConvert::_2int(name.substr(name.rfind("_") + 1).c_str()));
        } else if (name.find("P_A0_14_") == 0) {
            return (SUMOEmissionClass)(SVE_P_LDV_A0_14_1 - 1 + TplConvert::_2int(name.substr(name.rfind("_") + 1).c_str()));
        }
    } catch (NumberFormatException&) {
    }
    throw ProcessError("Unknown emission type '" + name + "'.");
}


std::string
getVehicleEmissionTypeName(SUMOEmissionClass id) {
    if (id == SVE_ZERO_EMISSIONS) {
        return "zero";
    }
    if (id < 0) {
        return "";
    } else if (id < 3) {
        return "HDV_3_" + toString(int(id));
    } else if (id < 3 + 6) {
        return "HDV_6_" + toString(int(id - 3));
    } else if (id < 3 + 6 + 12) {
        return "HDV_12_" + toString(int(id - 3 - 6));
    } else if (id < 3 + 6 + 12 + 7) {
        return "P_7_" + toString(int(id - 3 - 6 - 12));
    } else if (id < 3 + 6 + 12 + 7 + 14) {
        return "P_14_" + toString(int(id - 3 - 6 - 12 - 7));
    }
    return "";
}

const std::string DEFAULT_VTYPE_ID("DEFAULT_VEHTYPE");
const SUMOReal DEFAULT_VEH_MAXSPEED(70.0);
const SUMOReal DEFAULT_VEH_ACCEL(2.6);
const SUMOReal DEFAULT_VEH_DECEL(4.5);
const SUMOReal DEFAULT_VEH_SIGMA(0.5);
const SUMOReal DEFAULT_VEH_LENGTH(5.);
const SUMOReal DEFAULT_VEH_MINGAP(2.5);
const SUMOReal DEFAULT_VEH_TAU(1.);
const SUMOVehicleClass DEFAULT_VEH_CLASS(SVC_UNKNOWN);
const SUMOReal DEFAULT_VEH_PROB(1.);
const SUMOReal DEFAULT_VEH_SPEEDFACTOR(1.);
const SUMOReal DEFAULT_VEH_SPEEDDEV(0.);
const SUMOReal DEFAULT_VEH_WIDTH(2.);
const SUMOReal DEFAULT_VEH_HEIGHT(1.5);
const SumoXMLTag DEFAULT_VEH_FOLLOW_MODEL(SUMO_TAG_CF_KRAUSS);
const std::string DEFAULT_VEH_LANE_CHANGE_MODEL("dkrajzew2008");
const SUMOVehicleShape DEFAULT_VEH_SHAPE(SVS_UNKNOWN);
const SUMOReal DEFAULT_VEH_TMP1(1.);
const SUMOReal DEFAULT_VEH_TMP2(1.);
const SUMOReal DEFAULT_VEH_TMP3(1.);
const SUMOReal DEFAULT_VEH_TMP4(1.);
const SUMOReal DEFAULT_VEH_TMP5(1.);

const SUMOReal DEFAULT_PERSON_SPEED(5./3.6);

/****************************************************************************/

