
/****************************************************************************/
/// @file    ROVehicleType_IDM.cpp
/// @author  Tobias Mayer
/// @date    Jun 2009
/// @version $Id$
///
// A IDM vehicle type
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
#include <iostream>
#include "ROVehicleType.h"
#include "ROVehicleType_IDM.h"
#include <utils/iodevices/OutputDevice.h>

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
ROVehicleType_IDM::ROVehicleType_IDM(const std::string &id,
        const std::string &col, SUMOReal length, SUMOVehicleClass vclass,
        SUMOReal a, SUMOReal b, SUMOReal eps, SUMOReal maxSpeed)
        throw()
        : ROVehicleType(id, col, length, maxSpeed, vclass), myA(a), myB(b), myEps(eps)
        {
             myDelta = 4.0;
             myMinBtoBDistance = 33.0;
             myTimeHeadWay = 1.5;
        }

ROVehicleType_IDM::ROVehicleType_IDM(const std::string &id,
        const std::string &col, SUMOReal length, SUMOVehicleClass vclass,
        SUMOReal a, SUMOReal b, SUMOReal eps, SUMOReal maxSpeed,
        SUMOReal timeHeadWay, SUMOReal minBtoBDistance, SUMOReal delta) throw()
        : ROVehicleType(id, col, length, maxSpeed, vclass), myA(a), myB(b), myEps(eps),
        myTimeHeadWay(timeHeadWay), myMinBtoBDistance(minBtoBDistance),
        myDelta(delta) {}


ROVehicleType_IDM::~ROVehicleType_IDM() throw() {}


OutputDevice &
ROVehicleType_IDM::writeXMLDefinition(OutputDevice &dev) const {
    dev << "   ";
    dev << "<vtype model=\"SUMO_IDM\""
    << " id=\"" << myID << "\""
    << " accel=\"" << myA << "\""
    << " decel=\"" << myB << "\""
    << " sigma=\"" << myEps << "\""
    << " length=\"" << myLength << "\""
    << " maxspeed=\"" << myMaxSpeed << "\""
    << " minBtoBDistance=\"" << myMinBtoBDistance << "\""
    << " timeHeadWay=\"" << myTimeHeadWay << "\""
    << " delta=\"" << myDelta << "\"";
    if (myColor!="") {
        dev << " color=\"" << myColor << "\"";
    }
    if (myClass!=SVC_UNKNOWN) {
        dev << " vclass=\"" << getVehicleClassName(myClass) << "\"";
    }
    dev << "/>\n";
    return dev;
}



/****************************************************************************/

