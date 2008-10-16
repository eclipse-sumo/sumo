/****************************************************************************/
/// @file    NINavTeqHelper.cpp
/// @author  Daniel Krajzewicz
/// @date    Jul 2006
/// @version $Id$
///
// Some parser methods shared around several formats containing NavTeq-Nets
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

#include "NINavTeqHelper.h"
#include <utils/common/TplConvert.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <netbuild/NBEdge.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
SUMOReal
NINavTeqHelper::getSpeed(const std::string &id, const std::string &speedClassS) throw(ProcessError)
{
    try {
        int speedClass = TplConvert<char>::_2int(speedClassS.c_str());
        switch (speedClass) {
        case -1:
            return (SUMOReal) 1.0 / (SUMOReal) 3.6;
        case 1:
            return (SUMOReal) 200 / (SUMOReal) 3.6; //> 130 KPH / > 80 MPH
        case 2:
            return (SUMOReal) 120 / (SUMOReal) 3.6; //101-130 KPH / 65-80 MPH
        case 3:
            return (SUMOReal) 100 / (SUMOReal) 3.6; // 91-100 KPH / 55-64 MPH
        case 4:
            return (SUMOReal) 80 / (SUMOReal) 3.6; // 71-90 KPH / 41-54 MPH
        case 5:
            return (SUMOReal) 70 / (SUMOReal) 3.6; // 51-70 KPH / 31-40 MPH
        case 6:
            return (SUMOReal) 50 / (SUMOReal) 3.6; // 31-50 KPH / 21-30 MPH
        case 7:
            return (SUMOReal) 30 / (SUMOReal) 3.6; // 11-30 KPH / 6-20 MPH
        case 8:
            return (SUMOReal) 5 / (SUMOReal) 3.6; //< 11 KPH / < 6 MPH
        default:
            throw ProcessError("Invalid speed code (edge '" + id + "').");
        }
    } catch (NumberFormatException &) {
        throw ProcessError("Non-numerical value for an edge's speed type occured (edge '" + id + "').");
    }
}


unsigned int
NINavTeqHelper::getLaneNumber(const std::string &id, const std::string &laneNoS, SUMOReal speed) throw(ProcessError)
{
    try {
        int nolanes = TplConvert<char>::_2int(laneNoS.c_str());
        if (nolanes<0) {
            return 1;
        } else if (nolanes/10 > 0) {
            return nolanes / 10;
        } else {
            switch (nolanes%10) {
            case 1:
                return 1;
            case 2:
                nolanes = 2;
                if (speed>78.0/3.6) {
                    nolanes = 3;
                }
                return nolanes;
            case 3:
                return 4;
            default:
                throw ProcessError("Invalid lane number (edge '" + id + "').");
            }
        }
    } catch (NumberFormatException &) {
        throw ProcessError("Non-numerical value for an edge's lane number occured (edge '" + id + "'.");
    }
}


void
NINavTeqHelper::addVehicleClasses(NBEdge &e, const std::string &oclassS) throw()
{
    string classS = "0000000000" + oclassS;
    classS = classS.substr(classS.length() - 10);
    // 0: allow all vehicle types
    if (classS[0]=='1') {
        return;
    }
    // Passenger cars -- becomes SVC_PASSENGER
    if (classS[1]=='1') {
        addVehicleClass(e, SVC_PASSENGER);
    }
    // High Occupancy Vehicle -- becomes SVC_PASSENGER|SVC_HOV
    if (classS[2]=='1') {
        addVehicleClass(e, SVC_HOV);
        addVehicleClass(e, SVC_PASSENGER);
    }
    // Emergency Vehicle -- becomes SVC_PUBLIC_EMERGENCY
    if (classS[3]=='1') {
        addVehicleClass(e, SVC_PUBLIC_EMERGENCY);
    }
    // Taxi -- becomes SVC_PASSENGER|SVC_TAXI
    if (classS[4]=='1') {
        addVehicleClass(e, SVC_TAXI);
        addVehicleClass(e, SVC_PASSENGER);
    }
    // Public Bus -- becomes SVC_BUS|SVC_PUBLIC_TRANSPORT
    if (classS[5]=='1') {
        addVehicleClass(e, SVC_PUBLIC_TRANSPORT);
        addVehicleClass(e, SVC_BUS);
    }
    // Delivery Truck -- becomes SVC_DELIVERY
    if (classS[6]=='1') {
        addVehicleClass(e, SVC_DELIVERY);
    }
    // Transport Truck -- becomes SVC_TRANSPORT
    if (classS[7]=='1') {
        addVehicleClass(e, SVC_TRANSPORT);
    }
    // Bicycle -- becomes SVC_BICYCLE
    if (classS[8]=='1') {
        addVehicleClass(e, SVC_BICYCLE);
    }
    // Pedestrian -- becomes SVC_PEDESTRIAN
    if (classS[9]=='1') {
        addVehicleClass(e, SVC_PEDESTRIAN);
    }
}


void
NINavTeqHelper::addVehicleClass(NBEdge &e, SUMOVehicleClass c) throw()
{
    for (unsigned int i=0; i<e.getNoLanes(); i++) {
        e.allowVehicleClass(i, c);
    }
}



/****************************************************************************/

