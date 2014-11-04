/****************************************************************************/
/// @file    MSCFModel.cpp
/// @author  Tobias Mayer
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Mon, 27 Jul 2009
/// @version $Id$
///
// The car-following model abstraction
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

#include <math.h>
#include <microsim/MSVehicleType.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSLane.h>
#include <microsim/MSAbstractLaneChangeModel.h>
#include "MSCFModel.h"


// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel::MSCFModel(const MSVehicleType* vtype, const SUMOReal accel,
                     const SUMOReal decel, const SUMOReal headwayTime)
    : myType(vtype), myAccel(accel), myDecel(decel), myHeadwayTime(headwayTime) {
}


MSCFModel::~MSCFModel() {}


SUMOReal
MSCFModel::moveHelper(MSVehicle* const veh, SUMOReal vPos) const {
    const SUMOReal oldV = veh->getSpeed(); // save old v for optional acceleration computation
    const SUMOReal vSafe = MIN2(vPos, veh->processNextStop(vPos)); // process stops
    // we need the acceleration for emission computation;
    //  in this case, we neglect dawdling, nonetheless, using
    //  vSafe does not incorporate speed reduction due to interaction
    //  on lane changing
    const SUMOReal vMin = getSpeedAfterMaxDecel(oldV);
    const SUMOReal vMax = MIN3(veh->getLane()->getVehicleMaxSpeed(veh), maxNextSpeed(oldV, veh), vSafe);
    assert(vMin <= vMax);
    return veh->getLaneChangeModel().patchSpeed(vMin, vMax, vMax, *this);
}


SUMOReal
MSCFModel::interactionGap(const MSVehicle* const veh, SUMOReal vL) const {
    // Resolve the vsafe equation to gap. Assume predecessor has
    // speed != 0 and that vsafe will be the current speed plus acceleration,
    // i.e that with this gap there will be no interaction.
    const SUMOReal vNext = MIN2(maxNextSpeed(veh->getSpeed(), veh), veh->getLane()->getVehicleMaxSpeed(veh));
    const SUMOReal gap = (vNext - vL) *
                         ((veh->getSpeed() + vL) / (2.*myDecel) + myHeadwayTime) +
                         vL * myHeadwayTime;

    // Don't allow timeHeadWay < deltaT situations.
    return MAX2(gap, SPEED2DIST(vNext));
}


SUMOReal
MSCFModel::maxNextSpeed(SUMOReal speed, const MSVehicle* const /*veh*/) const {
    return MIN2(speed + (SUMOReal) ACCEL2SPEED(getMaxAccel()), myType->getMaxSpeed());
}


SUMOReal
MSCFModel::freeSpeed(const MSVehicle* const /* veh */, SUMOReal /* speed */, SUMOReal seen, SUMOReal maxSpeed, const bool onInsertion) const {
    return freeSpeed(myDecel, seen, maxSpeed, onInsertion);
}


SUMOReal
MSCFModel::insertionFollowSpeed(const MSVehicle* const, SUMOReal, SUMOReal gap2pred, SUMOReal predSpeed, SUMOReal predMaxDecel) const {
    return maximumSafeFollowSpeed(gap2pred, predSpeed, predMaxDecel);
}



SUMOReal
MSCFModel::maximumSafeStopSpeed(SUMOReal gap) const {
    gap -= NUMERICAL_EPS; // lots of code relies on some slack
    if (gap <= 0) {
        return 0;
    } else if (gap <= ACCEL2SPEED(myDecel)) {
        return gap;
    }
    const SUMOReal g = gap;
    const SUMOReal b = ACCEL2SPEED(myDecel);
    const SUMOReal t = myHeadwayTime;
    const SUMOReal s = TS;
    // h = the distance that would be covered if it were possible to stop
    // exactly after gap and decelerate with b every simulation step
    // h = 0.5 * n * (n-1) * b * s + n * b * t (solve for n)
    //n = ((1.0/2.0) - ((t + (pow(((s*s) + (4.0*((s*((2.0*h/b) - t)) + (t*t)))), (1.0/2.0))*sign/2.0))/s));
    const SUMOReal n = floor((1.0 / 2.0) - ((t + (pow(((s * s) + (4.0 * ((s * ((2.0 * g / b) - t)) + (t * t)))), (1.0 / 2.0)) * -0.5)) / s));
    const SUMOReal h = 0.5 * n * (n - 1) * b * s + n * b * t;
    assert(h <= g + NUMERICAL_EPS);
    // compute the additional speed that must be used during deceleration to fix
    // the discrepancy between g and h
    const SUMOReal r = (g - h) / (n * s + t);
    const SUMOReal x = n * b + r;
    assert(x >= 0);
    return x;
}


/** Returns the SK-vsafe. */
SUMOReal
MSCFModel::maximumSafeFollowSpeed(SUMOReal gap, SUMOReal predSpeed, SUMOReal predMaxDecel) const {
    // the speed is safe if allows the ego vehicle to come to a stop behind the leader even if
    // the leaders starts braking hard until stopped
    // unfortunately it is not sufficent to compare stopping distances if the follower can brake harder than the leader
    // (the trajectories might intersect before both vehicles are stopped even if the follower has a shorter stopping distance than the leader)
    // To make things safe, we ensure that the leaders brake distance is computed with an deceleration that is at least as high as the follower's.
    // @todo: this is a conservative estimate for safe speed which could be increased
    const SUMOReal x = maximumSafeStopSpeed(gap + brakeGap(predSpeed, MAX2(myDecel, predMaxDecel), 0));
    assert(x >= 0);
    assert(!ISNAN(x));
    return x;
}


/****************************************************************************/
