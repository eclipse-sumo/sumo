/****************************************************************************/
/// @file    SUMOVehicleClass.cpp
/// @author  Daniel Krajzewicz
/// @date    2006-01-24
/// @version $Id$
///
// Definitions of SUMO vehicle classes and helper functions
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
// definitions of string representations
// ===========================================================================
StringBijection<SUMOVehicleClass>::Entry sumoVehicleClassStringInitializer[] = {
    {"",                  SVC_UNKNOWN},
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


// ===========================================================================
// static members
// ===========================================================================
std::map<SUMOVehicleShape, std::string> gVehicleShapeID2Name;
std::map<std::string, SUMOVehicleShape> gVehicleShapeName2ID;


// ===========================================================================
// method definitions
// ===========================================================================
// ------------ Conversion of SUMOVehicleClass

std::string
getVehicleClassCompoundName(int id) throw() {
    std::string ret;
    const std::vector<std::string> names = SumoVehicleClassStrings.getStrings();
    for (std::vector<std::string>::const_iterator it = names.begin(); it != names.end(); it++) {
        if ((id & SumoVehicleClassStrings.get(*it))) {
            ret += ("|" + *it);
        }
    }
    if (ret.length()>0) {
        return ret.substr(1);
    } else {
        return ret;
    }
}


std::string 
getVehicleClassNames(const SUMOVehicleClasses &ids) throw() {
    std::ostringstream oss;
    bool hadOne = false;
    for (SUMOVehicleClasses::const_iterator i=ids.begin(); i!=ids.end(); ++i) {
        if (hadOne) {
            oss << ' ';
        }
        oss << toString(*i);
        hadOne = true;
    }
    return oss.str();
}


SUMOVehicleClass
getVehicleClassID(const std::string &name) throw() {
    if (SumoVehicleClassStrings.hasString(name)) {
        return SumoVehicleClassStrings.get(name);
    } else {
        return SVC_UNKNOWN;
    }
}


int
getVehicleClassCompoundID(const std::string &name) throw() {
    int ret = SVC_UNKNOWN;
    const std::vector<std::string> names = SumoVehicleClassStrings.getStrings();
    for (std::vector<std::string>::const_iterator it = names.begin(); it != names.end(); it++) {
        if (name.find(*it) != std::string::npos) {
            ret = ret | (int) SumoVehicleClassStrings.get(*it);
        }
    }
    return ret;
}


void 
parseVehicleClasses(
        const std::string &classNames,
        SUMOVehicleClasses &container) throw() {
    StringTokenizer sta(classNames, " ");
    while (sta.hasNext()) {
        container.insert(getVehicleClassID(sta.next()));
    }
}


void
parseVehicleClasses(const std::string &allowedS,
                    const std::string &disallowedS,
                    SUMOVehicleClasses &allowed,
                    SUMOVehicleClasses &disallowed) throw() {
    parseVehicleClasses(allowedS, allowed);
    parseVehicleClasses(disallowedS, disallowed);
}


void
parseVehicleClasses(const std::vector<std::string> &classesS,
                    SUMOVehicleClasses &classes) throw() {
    for (std::vector<std::string>::const_iterator i=classesS.begin(); i!=classesS.end(); ++i) {
        classes.insert(getVehicleClassID(*i));
    }
}





// ------------ Conversion of SUMOVehicleShape
void
addToShapeNames(SUMOVehicleShape id, const std::string &name) throw() {
    gVehicleShapeID2Name[id] = name;
    gVehicleShapeName2ID[name] = id;
}


void
initGuiShapeNames() throw() {
    addToShapeNames(SVS_PEDESTRIAN, "pedestrian");
    addToShapeNames(SVS_BICYCLE, "bicycle");
    addToShapeNames(SVS_MOTORCYCLE, "motorcycle");
    addToShapeNames(SVS_PASSENGER, "passenger");
    addToShapeNames(SVS_PASSENGER_SEDAN, "passenger/sedan");
    addToShapeNames(SVS_PASSENGER_HATCHBACK, "passenger/hatchback");
    addToShapeNames(SVS_PASSENGER_WAGON, "passenger/wagon");
    addToShapeNames(SVS_PASSENGER_VAN, "passenger/van");
    addToShapeNames(SVS_DELIVERY, "delivery");
    addToShapeNames(SVS_TRANSPORT, "transport");
    addToShapeNames(SVS_TRANSPORT_SEMITRAILER, "transport/semitrailer");
    addToShapeNames(SVS_TRANSPORT_1TRAILER, "transport/trailer");
    addToShapeNames(SVS_BUS, "bus");
    addToShapeNames(SVS_BUS_CITY, "bus/city");
    addToShapeNames(SVS_BUS_CITY_FLEXIBLE, "bus/flexible");
    addToShapeNames(SVS_BUS_OVERLAND, "bus/overland");
    addToShapeNames(SVS_RAIL, "rail");
    addToShapeNames(SVS_RAIL_LIGHT, "rail/light");
    addToShapeNames(SVS_RAIL_CITY, "rail/city");
    addToShapeNames(SVS_RAIL_SLOW, "rail/slow");
    addToShapeNames(SVS_RAIL_FAST, "rail/fast");
    addToShapeNames(SVS_RAIL_CARGO, "rail/cargo");
    addToShapeNames(SVS_E_VEHICLE, "evehicle");
}


std::string
getVehicleShapeName(SUMOVehicleShape id) throw() {
    if (id==SVS_UNKNOWN) {
        return "";
    }
    return gVehicleShapeID2Name[id];
}


SUMOVehicleShape
getVehicleShapeID(const std::string &name) throw() {
    if (name=="") {
        return SVS_UNKNOWN;
    }
    if (gVehicleShapeName2ID.find(name)!=gVehicleShapeName2ID.end()) {
        return gVehicleShapeName2ID[name];
    }
    return SVS_UNKNOWN;
    //!!!throw InvalidArgument("Unknown vehicle shape '" + name + "' occured.");
}



// ------------ Conversion of SUMOEmissionClass
SUMOEmissionClass
getVehicleEmissionTypeID(const std::string &name) throw(ProcessError) {
    try {
        if (name=="") {
            return SVE_UNKNOWN;
        } else if (name=="zero") {
            return SVE_ZERO_EMISSIONS;
        } else if (name.find("HDV_3_")==0) {
            return (SUMOEmissionClass)(SVE_HDV_3_1 - 1 + TplConvert<char>::_2int(name.substr(name.rfind("_")+1).c_str()));
        } else if (name.find("HDV_6_")==0) {
            return (SUMOEmissionClass)(SVE_HDV_6_1 - 1 + TplConvert<char>::_2int(name.substr(name.rfind("_")+1).c_str()));
        } else if (name.find("HDV_12_")==0) {
            return (SUMOEmissionClass)(SVE_HDV_12_1 - 1 + TplConvert<char>::_2int(name.substr(name.rfind("_")+1).c_str()));
        } else if (name.find("P_7_")==0) {
            return (SUMOEmissionClass)(SVE_P_LDV_7_1 - 1 + TplConvert<char>::_2int(name.substr(name.rfind("_")+1).c_str()));
        } else if (name.find("P_14_")==0) {
            return (SUMOEmissionClass)(SVE_P_LDV_14_1 - 1 + TplConvert<char>::_2int(name.substr(name.rfind("_")+1).c_str()));
        }
    } catch (NumberFormatException &) {
        throw ProcessError("Unknown emission type '" + name + "'.");
    }
    return SVE_UNKNOWN;
}


std::string
getVehicleEmissionTypeName(SUMOEmissionClass id) throw() {
    if (id==SVE_ZERO_EMISSIONS) {
        return "zero";
    }
    if (id<0) {
        return "";
    } else if (id<3) {
        return "HDV_3_" + toString(int(id));
    } else if (id<3+6) {
        return "HDV_6_" + toString(int(id-3));
    } else if (id<3+6+12) {
        return "HDV_12_" + toString(int(id-3-6));
    } else if (id<3+6+12+7) {
        return "P_7_" + toString(int(id-3-6-12));
    } else if (id<3+6+12+7+14) {
        return "P_14_" + toString(int(id-3-6-12-7));
    }
    return "";
}



/****************************************************************************/

