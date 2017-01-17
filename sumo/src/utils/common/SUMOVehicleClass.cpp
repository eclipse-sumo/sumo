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
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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
#include <utils/iodevices/OutputDevice.h>


#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static members
// ===========================================================================
StringBijection<SUMOVehicleClass>::Entry sumoVehicleClassStringInitializer[] = {
    {"ignoring",          SVC_IGNORING},
    {"private",           SVC_PRIVATE},
    {"public_emergency",  SVC_EMERGENCY}, // !!! deprecated
    {"emergency",         SVC_EMERGENCY},
    {"public_authority",  SVC_AUTHORITY}, // !!! deprecated
    {"authority",         SVC_AUTHORITY},
    {"public_army",       SVC_ARMY}, // !!! deprecated
    {"army",              SVC_ARMY},
    {"vip",               SVC_VIP},
    {"passenger",         SVC_PASSENGER},
    {"hov",               SVC_HOV},
    {"taxi",              SVC_TAXI},
    {"public_transport",  SVC_BUS}, // !!! deprecated
    {"bus",               SVC_BUS},
    {"coach",             SVC_COACH},
    {"delivery",          SVC_DELIVERY},
    {"transport",         SVC_TRUCK},
    {"truck",             SVC_TRUCK},
    {"trailer",           SVC_TRAILER},
    {"lightrail",         SVC_TRAM}, // !!! deprecated
    {"tram",              SVC_TRAM},
    {"cityrail",          SVC_RAIL_URBAN}, // !!! deprecated
    {"rail_urban",        SVC_RAIL_URBAN},
    {"rail_slow",         SVC_RAIL}, // !!! deprecated
    {"rail",              SVC_RAIL},
    {"rail_fast",         SVC_RAIL_ELECTRIC}, // !!! deprecated
    {"rail_electric",     SVC_RAIL_ELECTRIC},
    {"motorcycle",        SVC_MOTORCYCLE},
    {"moped",             SVC_MOPED},
    {"bicycle",           SVC_BICYCLE},
    {"pedestrian",        SVC_PEDESTRIAN},
    {"evehicle",          SVC_E_VEHICLE},
    {"ship",              SVC_SHIP},
    {"custom1",           SVC_CUSTOM1},
    {"custom2",           SVC_CUSTOM2}
};

StringBijection<SUMOVehicleClass> SumoVehicleClassStrings(
    sumoVehicleClassStringInitializer, SVC_CUSTOM2, false);

std::set<std::string> deprecatedVehicleClassesSeen;


StringBijection<SUMOVehicleShape>::Entry sumoVehicleShapeStringInitializer[] = {
    {"pedestrian",            SVS_PEDESTRIAN},
    {"bicycle",               SVS_BICYCLE},
    {"moped",                 SVS_MOPED},
    {"motorcycle",            SVS_MOTORCYCLE},
    {"passenger",             SVS_PASSENGER},
    {"passenger/sedan",       SVS_PASSENGER_SEDAN},
    {"passenger/hatchback",   SVS_PASSENGER_HATCHBACK},
    {"passenger/wagon",       SVS_PASSENGER_WAGON},
    {"passenger/van",         SVS_PASSENGER_VAN},
    {"delivery",              SVS_DELIVERY},
    {"transport",             SVS_TRUCK}, // !!! deprecated
    {"truck",                 SVS_TRUCK},
    {"transport/semitrailer", SVS_TRUCK_SEMITRAILER}, // !!! deprecated
    {"truck/semitrailer",     SVS_TRUCK_SEMITRAILER},
    {"transport/trailer",     SVS_TRUCK_1TRAILER}, // !!! deprecated
    {"truck/trailer",         SVS_TRUCK_1TRAILER},
    {"bus/city",              SVS_BUS}, // !!! deprecated
    {"bus",                   SVS_BUS},
    {"bus/overland",          SVS_BUS_COACH}, // !!! deprecated
    {"bus/coach",             SVS_BUS_COACH},
    {"bus/flexible",          SVS_BUS_FLEXIBLE},
    {"bus/trolley",           SVS_BUS_TROLLEY},
    {"rail/slow",             SVS_RAIL}, // !!! deprecated
    {"rail/fast",             SVS_RAIL}, // !!! deprecated
    {"rail",                  SVS_RAIL},
    {"rail/light",            SVS_RAIL_CAR}, // !!! deprecated
    {"rail/city",             SVS_RAIL_CAR}, // !!! deprecated
    {"rail/railcar",          SVS_RAIL_CAR},
    {"rail/cargo",            SVS_RAIL_CARGO},
    {"evehicle",              SVS_E_VEHICLE},
    {"ant",                   SVS_ANT},
    {"ship",                  SVS_SHIP},
    {"emergency",             SVS_EMERGENCY},
    {"",                      SVS_UNKNOWN}
};


StringBijection<SUMOVehicleShape> SumoVehicleShapeStrings(
    sumoVehicleShapeStringInitializer, SVS_UNKNOWN, false);


// ===========================================================================
// additional constants
// ===========================================================================

const int SUMOVehicleClass_MAX = SVC_CUSTOM2;
const SVCPermissions SVCAll = 2 * SUMOVehicleClass_MAX - 1; // all relevant bits set to 1
const SVCPermissions SVC_UNSPECIFIED = -1;


// ===========================================================================
// method definitions
// ===========================================================================
// ------------ Conversion of SUMOVehicleClass

std::string
getVehicleClassNames(SVCPermissions permissions) {
    if (permissions == SVCAll) {
        return "all";
    }
    return joinToString(getVehicleClassNamesList(permissions), ' ');
}


std::vector<std::string>
getVehicleClassNamesList(SVCPermissions permissions) {
    /// @todo cache values?
    const std::vector<std::string> classNames = SumoVehicleClassStrings.getStrings();
    std::vector<std::string> result;
    for (std::vector<std::string>::const_iterator it = classNames.begin(); it != classNames.end(); it++) {
        const int svc = (int)SumoVehicleClassStrings.get(*it);
        if ((svc & permissions) == svc && svc != SVC_IGNORING) {
            result.push_back(*it);
        }
    }
    return result;
}


SUMOVehicleClass
getVehicleClassID(const std::string& name) {
    if (SumoVehicleClassStrings.hasString(name)) {
        return SumoVehicleClassStrings.get(name);
    }
    throw InvalidArgument("Unknown vehicle class '" + name + "'.");
}


int
getVehicleClassCompoundID(const std::string& name) {
    int ret = SVC_IGNORING;
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
    if (allowedS == "all") {
        return SVCAll;
    }
    SVCPermissions result = 0;
    StringTokenizer sta(allowedS, " ");
    while (sta.hasNext()) {
        const std::string s = sta.next();
        if (!SumoVehicleClassStrings.hasString(s)) {
            WRITE_ERROR("Unknown vehicle class '" + s + "' encountered. It will be ignored.");
        } else {
            const SUMOVehicleClass vc = getVehicleClassID(s);
            const std::string& realName = SumoVehicleClassStrings.getString(vc);
            if (realName != s) {
                deprecatedVehicleClassesSeen.insert(s);
            }
            result |= vc;
        }
    }
    return result;
}


bool
canParseVehicleClasses(const std::string& classes) {
    if (classes == "all") {
        return true;
    }
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
        return SVCAll;
    } else if (allowedS.size() > 0 && disallowedS.size() > 0) {
        WRITE_WARNING("SVCPermissions must be specified either via 'allow' or 'disallow'. Ignoring 'disallow'");
        return parseVehicleClasses(allowedS);
    } else if (allowedS.size() > 0) {
        return parseVehicleClasses(allowedS);
    } else {
        return SVCAll & ~parseVehicleClasses(disallowedS);
    }
}


SVCPermissions
parseVehicleClasses(const std::vector<std::string>& allowedS) {
    SVCPermissions result = 0;
    for (std::vector<std::string>::const_iterator i = allowedS.begin(); i != allowedS.end(); ++i) {
        const SUMOVehicleClass vc = getVehicleClassID(*i);
        const std::string& realName = SumoVehicleClassStrings.getString(vc);
        if (realName != *i) {
            WRITE_WARNING("The vehicle class '" + (*i) + "' is deprecated, use '" + realName + "' instead.");
        }
        result |= getVehicleClassID(*i);
    }
    return result;
}


void
writePermissions(OutputDevice& into, SVCPermissions permissions) {
    if (permissions == SVCAll) {
        return;
    } else if (permissions == 0) {
        into.writeAttr(SUMO_ATTR_DISALLOW, "all");
        return;
    } else {
        int num_allowed = 0;
        for (int mask = 1; mask <= SUMOVehicleClass_MAX; mask = mask << 1) {
            if ((mask & permissions) == mask) {
                ++num_allowed;
            }
        }
        if (num_allowed <= (SumoVehicleClassStrings.size() - num_allowed) && num_allowed > 0) {
            into.writeAttr(SUMO_ATTR_ALLOW, getVehicleClassNames(permissions));
        } else {
            into.writeAttr(SUMO_ATTR_DISALLOW, getVehicleClassNames(~permissions));
        }
    }
}


void
writePreferences(OutputDevice& into, SVCPermissions preferred) {
    if (preferred == SVCAll || preferred == 0) {
        return;
    } else {
        into.writeAttr(SUMO_ATTR_PREFER, getVehicleClassNames(preferred));
    }
}


SUMOVehicleShape
getVehicleShapeID(const std::string& name) {
    if (SumoVehicleShapeStrings.hasString(name)) {
        return SumoVehicleShapeStrings.get(name);
    } else {
        throw InvalidArgument("Unknown vehicle shape '" + name + "'.");
    }
}


std::string
getVehicleShapeName(SUMOVehicleShape id) {
    return SumoVehicleShapeStrings.getString(id);
}


bool isRailway(SVCPermissions permissions) {
    return (permissions & (SVC_RAIL_ELECTRIC | SVC_RAIL | SVC_RAIL_URBAN | SVC_TRAM)) > 0 && (permissions & SVC_PASSENGER) == 0;
}


bool isWaterway(SVCPermissions permissions) {
    return permissions == SVC_SHIP;
}


bool isForbidden(SVCPermissions permissions) {
    return (permissions & SVCAll) == 0;
}


const std::string DEFAULT_VTYPE_ID("DEFAULT_VEHTYPE");
const std::string DEFAULT_PEDTYPE_ID("DEFAULT_PEDTYPE");

const SUMOReal DEFAULT_VEH_PROB(1.);

const SUMOReal DEFAULT_PEDESTRIAN_SPEED(5. / 3.6);

const SUMOReal DEFAULT_CONTAINER_TRANSHIP_SPEED(5. / 3.6);

/****************************************************************************/

