/****************************************************************************/
/// @file    MSCFModel_KraussPS.cpp
/// @author  Tobias Mayer
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Mon, 04 Aug 2009
/// @version $Id$
///
// Krauss car-following model, changing accel and speed by slope
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

#include <utils/geom/GeomHelper.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSLane.h>
#include "MSCFModel_KraussPS.h"


// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel_KraussPS::MSCFModel_KraussPS(const MSVehicleType* vtype, double accel, double decel, double emergencyDecel,
                                       double dawdle, double headwayTime) :
    MSCFModel_Krauss(vtype, accel, decel, emergencyDecel, decel, dawdle, headwayTime) {
}


MSCFModel_KraussPS::~MSCFModel_KraussPS() {}


double
MSCFModel_KraussPS::maxNextSpeed(double speed, const MSVehicle* const veh) const {
    const double gravity = 9.80665;
    const double aMax = MAX2(0., getMaxAccel() - gravity * sin(DEG2RAD(veh->getSlope())));
    // assuming drag force is proportional to the square of speed
    const double vMax = sqrt(aMax / getMaxAccel()) * myType->getMaxSpeed();
    return MIN2(speed + (double) ACCEL2SPEED(aMax), vMax);
}


MSCFModel*
MSCFModel_KraussPS::duplicate(const MSVehicleType* vtype) const {
    return new MSCFModel_KraussPS(vtype, myAccel, myDecel, myEmergencyDecel, myDawdle, myHeadwayTime);
}


/****************************************************************************/
