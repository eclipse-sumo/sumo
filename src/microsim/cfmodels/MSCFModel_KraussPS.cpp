/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/geom/GeomHelper.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSLane.h>
#include "MSCFModel_KraussPS.h"


// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel_KraussPS::MSCFModel_KraussPS(const MSVehicleType* vtype) :
    MSCFModel_Krauss(vtype) {
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
    return new MSCFModel_KraussPS(vtype);
}


/****************************************************************************/
