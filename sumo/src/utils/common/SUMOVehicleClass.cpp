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
// ---------------------------------------------------------------------------
// string representations of SUMOVehicleClass
// ---------------------------------------------------------------------------
std::string sSVC_PRIVATE("private");
std::string sSVC_PUBLIC_TRANSPORT("public_transport");
std::string sSVC_PUBLIC_EMERGENCY("public_emergency");
std::string sSVC_PUBLIC_AUTHORITY("public_authority");
std::string sSVC_PUBLIC_ARMY("public_army");
std::string sSVC_VIP("vip");
std::string sSVC_IGNORING("ignoring");

std::string sSVC_PASSENGER("passenger");
std::string sSVC_HOV("hov");
std::string sSVC_TAXI("taxi");
std::string sSVC_BUS("bus");
std::string sSVC_DELIVERY("delivery");
std::string sSVC_TRANSPORT("transport");
std::string sSVC_LIGHTRAIL("lightrail");
std::string sSVC_CITYRAIL("cityrail");
std::string sSVC_RAIL_SLOW("rail_slow");
std::string sSVC_RAIL_FAST("rail_fast");
std::string sSVC_MOTORCYCLE("motorcycle");
std::string sSVC_BICYCLE("bicycle");
std::string sSVC_PEDESTRIAN("pedestrian");


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
getVehicleClassName(SUMOVehicleClass id) throw() {
    std::string ret;
    if ((id&SVC_PRIVATE)!=0) {
        ret += ("|" + sSVC_PRIVATE);
    }
    if ((id&SVC_PUBLIC_TRANSPORT)!=0) {
        ret += ("|" + sSVC_PUBLIC_TRANSPORT);
    }
    if ((id&SVC_PUBLIC_EMERGENCY)!=0) {
        ret += ("|" + sSVC_PUBLIC_EMERGENCY);
    }
    if ((id&SVC_PUBLIC_AUTHORITY)!=0) {
        ret += ("|" + sSVC_PUBLIC_AUTHORITY);
    }
    if ((id&SVC_PUBLIC_ARMY)!=0) {
        ret += ("|" + sSVC_PUBLIC_ARMY);
    }
    if ((id&SVC_VIP)!=0) {
        ret += ("|" + sSVC_VIP);
    }
    if ((id&SVC_IGNORING)!=0) {
        ret += ("|" + sSVC_IGNORING);
    }

    if ((id&SVC_PASSENGER)!=0) {
        ret += ("|" + sSVC_PASSENGER);
    }
    if ((id&SVC_HOV)!=0) {
        ret += ("|" + sSVC_HOV);
    }
    if ((id&SVC_TAXI)!=0) {
        ret += ("|" + sSVC_TAXI);
    }
    if ((id&SVC_BUS)!=0) {
        ret += ("|" + sSVC_BUS);
    }
    if ((id&SVC_DELIVERY)!=0) {
        ret += ("|" + sSVC_DELIVERY);
    }
    if ((id&SVC_TRANSPORT)!=0) {
        ret += ("|" + sSVC_TRANSPORT);
    }
    if ((id&SVC_LIGHTRAIL)!=0) {
        ret += ("|" + sSVC_LIGHTRAIL);
    }
    if ((id&SVC_CITYRAIL)!=0) {
        ret += ("|" + sSVC_CITYRAIL);
    }
    if ((id&SVC_RAIL_SLOW)!=0) {
        ret += ("|" + sSVC_RAIL_SLOW);
    }
    if ((id&SVC_RAIL_FAST)!=0) {
        ret += ("|" + sSVC_RAIL_FAST);
    }
    if ((id&SVC_MOTORCYCLE)!=0) {
        ret += ("|" + sSVC_MOTORCYCLE);
    }
    if ((id&SVC_BICYCLE)!=0) {
        ret += ("|" + sSVC_BICYCLE);
    }
    if ((id&SVC_PEDESTRIAN)!=0) {
        ret += ("|" + sSVC_PEDESTRIAN);
    }

    if (ret.length()>0) {
        return ret.substr(1);
    } else {
        return ret;
    }
}


std::string 
getVehicleClassNames(const std::vector<SUMOVehicleClass> &ids) throw() {
    std::ostringstream oss;
    bool hadOne = false;
    for (std::vector<SUMOVehicleClass>::const_iterator i=ids.begin(); i!=ids.end(); ++i) {
        if (hadOne) {
            oss << ' ';
        }
        oss << getVehicleClassName(*i);
        hadOne = true;
    }
    return oss.str();
}



SUMOVehicleClass
getVehicleClassID(const std::string &name) throw() {
    SUMOVehicleClass ret = SVC_UNKNOWN;
    if (name.find(sSVC_PRIVATE)!=std::string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_PRIVATE);
    }
    if (name.find(sSVC_PUBLIC_TRANSPORT)!=std::string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_PUBLIC_TRANSPORT);
    }
    if (name.find(sSVC_PUBLIC_EMERGENCY)!=std::string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_PUBLIC_EMERGENCY);
    }
    if (name.find(sSVC_PUBLIC_AUTHORITY)!=std::string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_PUBLIC_AUTHORITY);
    }
    if (name.find(sSVC_PUBLIC_ARMY)!=std::string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_PUBLIC_ARMY);
    }
    if (name.find(sSVC_VIP)!=std::string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_VIP);
    }
    if (name.find(sSVC_IGNORING)!=std::string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_IGNORING);
    }


    if (name.find(sSVC_PASSENGER)!=std::string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_PASSENGER);
    }
    if (name.find(sSVC_HOV)!=std::string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_HOV);
    }
    if (name.find(sSVC_TAXI)!=std::string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_TAXI);
    }
    if (name.find(sSVC_BUS)!=std::string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_BUS);
    }
    if (name.find(sSVC_DELIVERY)!=std::string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_DELIVERY);
    }
    if (name.find(sSVC_TRANSPORT)!=std::string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_TRANSPORT);
    }
    if (name.find(sSVC_LIGHTRAIL)!=std::string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_LIGHTRAIL);
    }
    if (name.find(sSVC_CITYRAIL)!=std::string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_CITYRAIL);
    }
    if (name.find(sSVC_RAIL_SLOW)!=std::string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_RAIL_SLOW);
    }
    if (name.find(sSVC_RAIL_FAST)!=std::string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_RAIL_FAST);
    }
    if (name.find(sSVC_MOTORCYCLE)!=std::string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_MOTORCYCLE);
    }
    if (name.find(sSVC_BICYCLE)!=std::string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_BICYCLE);
    }
    if (name.find(sSVC_PEDESTRIAN)!=std::string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_PEDESTRIAN);
    }

    return ret;
}


void
parseVehicleClasses(const std::string &allowedS,
                    const std::string &disallowedS,
                    std::vector<SUMOVehicleClass> &allowed,
                    std::vector<SUMOVehicleClass> &disallowed) throw() {
    StringTokenizer sta(allowedS, " ");
    while (sta.hasNext()) {
        allowed.push_back(getVehicleClassID(sta.next()));
    }
    StringTokenizer std(disallowedS, " ");
    while (std.hasNext()) {
        disallowed.push_back(getVehicleClassID(std.next()));
    }
}


void
parseVehicleClasses(const std::vector<std::string> &classesS,
                    std::vector<SUMOVehicleClass> &classes) throw() {
    for (std::vector<std::string>::const_iterator i=classesS.begin(); i!=classesS.end(); ++i) {
        classes.push_back(getVehicleClassID(*i));
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

