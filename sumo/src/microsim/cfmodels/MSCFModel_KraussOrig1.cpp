/****************************************************************************/
/// @file    MSCFModel_KraussOrig1.cpp
/// @author  Tobias Mayer
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Mon, 04 Aug 2009
/// @version $Id$
///
// The original Krauss (1998) car-following model and parameter
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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

#include <microsim/MSVehicle.h>
#include <microsim/MSLane.h>
#include "MSCFModel_KraussOrig1.h"
#include <microsim/MSAbstractLaneChangeModel.h>
#include <utils/common/RandHelper.h>


// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel_KraussOrig1::MSCFModel_KraussOrig1(const MSVehicleType* vtype,  SUMOReal accel, SUMOReal decel,
        SUMOReal dawdle, SUMOReal headwayTime)
    : MSCFModel(vtype, accel, decel, headwayTime), myDawdle(dawdle), myTauDecel(decel* headwayTime) {
}


MSCFModel_KraussOrig1::~MSCFModel_KraussOrig1() {}


SUMOReal
MSCFModel_KraussOrig1::moveHelper(MSVehicle* const veh, SUMOReal vPos) const {
    const SUMOReal oldV = veh->getSpeed(); // save old v for optional acceleration computation
    const SUMOReal vSafe = MIN2(vPos, veh->processNextStop(vPos)); // process stops
    // we need the acceleration for emission computation;
    //  in this case, we neglect dawdling, nonetheless, using
    //  vSafe does not incorporate speed reduction due to interaction
    //  on lane changing
    const SUMOReal vMin = getSpeedAfterMaxDecel(oldV);
    // do not exceed max decel even if it is unsafe
    SUMOReal vMax = MAX2(vMin,
                         MIN3(veh->getLane()->getVehicleMaxSpeed(veh), maxNextSpeed(oldV, veh), vSafe));
#ifdef _DEBUG
    //if (vMin > vMax) {
    //    WRITE_WARNING("Vehicle's '" + veh->getID() + "' maximum speed is lower than the minimum speed (min: " + toString(vMin) + ", max: " + toString(vMax) + ").");
    //}
#endif
    return veh->getLaneChangeModel().patchSpeed(vMin, MAX2(vMin, dawdle(vMax)), vMax, *this);
}


SUMOReal
MSCFModel_KraussOrig1::followSpeed(const MSVehicle* const veh, SUMOReal speed, SUMOReal gap, SUMOReal predSpeed, SUMOReal predMaxDecel) const {
    return MIN2(vsafe(gap, predSpeed, predMaxDecel), maxNextSpeed(speed, veh));
}


SUMOReal
MSCFModel_KraussOrig1::insertionFollowSpeed(const MSVehicle* const veh, SUMOReal speed, SUMOReal gap2pred, SUMOReal predSpeed, SUMOReal predMaxDecel) const {
    // since the Krauss model tries to compute the maximum follow speed in
    // method followSpeed this is also used for insertionFollowSpeed
    // (due to discretization error this may not always be the same value as
    // returned by maximumSafeFollowSpeed)
    return followSpeed(veh, speed, gap2pred, predSpeed, predMaxDecel);
}


SUMOReal
MSCFModel_KraussOrig1::stopSpeed(const MSVehicle* const veh, const SUMOReal speed, SUMOReal gap) const {
    return MIN2(vsafe(gap, 0., 0.), maxNextSpeed(speed, veh));
}


SUMOReal
MSCFModel_KraussOrig1::dawdle(SUMOReal speed) const {
    return MAX2(SUMOReal(0), speed - ACCEL2SPEED(myDawdle * myAccel * RandHelper::rand()));
}


/** Returns the SK-vsafe. */
SUMOReal MSCFModel_KraussOrig1::vsafe(SUMOReal gap, SUMOReal predSpeed, SUMOReal /* predMaxDecel */) const {
    if (predSpeed == 0 && gap < 0.01) {
        return 0;
    }
    SUMOReal vsafe = (SUMOReal)(-1. * myTauDecel
                                + sqrt(
                                    myTauDecel * myTauDecel
                                    + (predSpeed * predSpeed)
                                    + (2. * myDecel * gap)
                                ));
    assert(vsafe >= 0);
    return vsafe;
}


MSCFModel*
MSCFModel_KraussOrig1::duplicate(const MSVehicleType* vtype) const {
    return new MSCFModel_KraussOrig1(vtype, myAccel, myDecel, myDawdle, myHeadwayTime);
}


/****************************************************************************/
