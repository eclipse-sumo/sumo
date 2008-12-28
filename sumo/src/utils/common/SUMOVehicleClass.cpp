/****************************************************************************/
/// @file    SUMOVehicleClass.cpp
/// @author  Daniel Krajzewicz
/// @date    2006-01-24
/// @version $Id$
///
// missing_desc
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


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
// staic members
// ===========================================================================
map<SUMOVehicleShape, string> gVehicleShapeID2Name;
map<string, SUMOVehicleShape> gVehicleShapeName2ID;


// ===========================================================================
// method definitions
// ===========================================================================
// ------------ Conversion of SUMOVehicleClass
std::string
getVehicleClassName(SUMOVehicleClass id) throw()
{
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


SUMOVehicleClass
getVehicleClassID(const std::string &name) throw()
{
    SUMOVehicleClass ret = SVC_UNKNOWN;
    if (name.find(sSVC_PRIVATE)!=string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_PRIVATE);
    }
    if (name.find(sSVC_PUBLIC_TRANSPORT)!=string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_PUBLIC_TRANSPORT);
    }
    if (name.find(sSVC_PUBLIC_EMERGENCY)!=string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_PUBLIC_EMERGENCY);
    }
    if (name.find(sSVC_PUBLIC_AUTHORITY)!=string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_PUBLIC_AUTHORITY);
    }
    if (name.find(sSVC_PUBLIC_ARMY)!=string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_PUBLIC_ARMY);
    }
    if (name.find(sSVC_VIP)!=string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_VIP);
    }


    if (name.find(sSVC_PASSENGER)!=string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_PASSENGER);
    }
    if (name.find(sSVC_HOV)!=string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_HOV);
    }
    if (name.find(sSVC_TAXI)!=string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_TAXI);
    }
    if (name.find(sSVC_BUS)!=string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_BUS);
    }
    if (name.find(sSVC_DELIVERY)!=string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_DELIVERY);
    }
    if (name.find(sSVC_TRANSPORT)!=string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_TRANSPORT);
    }
    if (name.find(sSVC_LIGHTRAIL)!=string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_LIGHTRAIL);
    }
    if (name.find(sSVC_CITYRAIL)!=string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_CITYRAIL);
    }
    if (name.find(sSVC_RAIL_SLOW)!=string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_RAIL_SLOW);
    }
    if (name.find(sSVC_RAIL_FAST)!=string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_RAIL_FAST);
    }
    if (name.find(sSVC_MOTORCYCLE)!=string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_MOTORCYCLE);
    }
    if (name.find(sSVC_BICYCLE)!=string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_BICYCLE);
    }
    if (name.find(sSVC_PEDESTRIAN)!=string::npos) {
        ret = (SUMOVehicleClass)((int) ret | (int) SVC_PEDESTRIAN);
    }

    return ret;
}



// ------------ Conversion of SUMOVehicleShape
void
addToShapeNames(SUMOVehicleShape id, const string &name) throw()
{
    gVehicleShapeID2Name[id] = name;
    gVehicleShapeName2ID[name] = id;
}


void
initGuiShapeNames() throw()
{
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
getVehicleShapeName(SUMOVehicleShape id) throw()
{
    if (id==SVS_UNKNOWN) {
        return "";
    }
    return gVehicleShapeID2Name[id];
}


SUMOVehicleShape
getVehicleShapeID(const std::string &name) throw()
{
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
getVehicleEmissionTypeID(const std::string &name) throw()
{
    if(name=="") {
        return SVE_UNKNOWN;
    } else if(name=="passenger_1_4__2l") {
        return SVE_PASSENGER_EURO4__1_4__2l;
    } else if(name=="bus/city") {
        return SVE_BUS_CITY;
    } else if(name=="bus/overland") {
        return SVE_BUS_OVERLAND;
    } else if(name=="transport_7_5t__euro4") {
        return SVE_HDV_7_5t__EURO4;
    } else if(name=="zero") {
        return SVE_ZERO_EMISSIONS;
    }
    return SVE_UNKNOWN;
}


std::string
getVehicleClassName(SUMOEmissionClass id) throw()
{
    switch(id) {
    case SVE_UNKNOWN:
        return "";
    case SVE_PASSENGER_EURO4__1_4__2l:
        return "passenger_1_4__2l";
    case SVE_BUS_CITY:
        return "bus/city";
    case SVE_BUS_OVERLAND:
        return "bus/overland";
    case SVE_HDV_7_5t__EURO4:
        return "transport_7_5t__euro4";
    case SVE_ZERO_EMISSIONS:
        return "zero";
    default:
        return "";
    }
}



/****************************************************************************/

