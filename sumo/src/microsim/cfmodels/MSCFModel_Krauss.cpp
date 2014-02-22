/****************************************************************************/
/// @file    MSCFModel_Krauss.cpp
/// @author  Tobias Mayer
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Mon, 04 Aug 2009
/// @version $Id$
///
// Krauss car-following model, with acceleration decrease and faster start
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
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
#include <microsim/MSGlobals.h>
#include "MSCFModel_Krauss.h"
#include <microsim/MSAbstractLaneChangeModel.h>
#include <utils/common/RandHelper.h>


// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel_Krauss::MSCFModel_Krauss(const MSVehicleType* vtype, SUMOReal accel, SUMOReal decel,
                                   SUMOReal dawdle, SUMOReal headwayTime)
    : MSCFModel_KraussOrig1(vtype, accel, decel, dawdle, headwayTime) {
}


MSCFModel_Krauss::~MSCFModel_Krauss() {}


SUMOReal
MSCFModel_Krauss::followSpeed(const MSVehicle* const veh, const SUMOReal speed, SUMOReal gap, SUMOReal predSpeed, SUMOReal predMaxDecel) const {
    return MIN2(_vsafe(gap, predSpeed, predMaxDecel), maxNextSpeed(speed, veh));
}


SUMOReal
MSCFModel_Krauss::stopSpeed(const MSVehicle* const veh, const SUMOReal speed, SUMOReal gap) const {
    return MIN2(_vstop(gap), maxNextSpeed(speed, veh));
}


SUMOReal
MSCFModel_Krauss::dawdle(SUMOReal speed) const {
    // generate random number out of [0,1]
    SUMOReal random = RandHelper::rand();
    // Dawdle.
    if (speed < myAccel) {
        // we should not prevent vehicles from driving just due to dawdling
        //  if someone is starting, he should definitely start
        // (but what about slow-to-start?)!!!
        speed -= ACCEL2SPEED(myDawdle * speed * random);
    } else {
        speed -= ACCEL2SPEED(myDawdle * myAccel * random);
    }
    return MAX2(SUMOReal(0), speed);
}


SUMOReal
MSCFModel_Krauss::_vstop(SUMOReal gap) const {
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
    const SUMOReal n = floor((1.0/2.0) - ((t + (pow(((s*s) + (4.0*((s*((2.0*g/b) - t)) + (t*t)))), (1.0/2.0))*-0.5))/s));
    const SUMOReal h = 0.5 * n * (n-1) * b * s + n * b * t;
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
MSCFModel_Krauss::_vsafe(SUMOReal gap, SUMOReal predSpeed, SUMOReal predMaxDecel) const {
    // the speed is safe if allows the ego vehicle to come to a stop behind the leader even if
    // the leaders starts braking hard until stopped
    // unfortunately it is not sufficent to compare stopping distances if the follower can brake harder than the leader
    // (the trajectories might intersect before both vehicles are stopped even if the follower has a shorter stopping distance than the leader)
    // To make things safe, we ensure that the leaders brake distance is computed with an deceleration that is at least as high as the follower's.
    // @todo: this is a conservative estimate for safe speed which could be increased
    const SUMOReal x = _vstop(gap + brakeGap(predSpeed, MAX2(myDecel, predMaxDecel), 0));
    assert(x >= 0);
    assert(!ISNAN(x));
    return x;
}


MSCFModel*
MSCFModel_Krauss::duplicate(const MSVehicleType* vtype) const {
    return new MSCFModel_Krauss(vtype, myAccel, myDecel, myDawdle, myHeadwayTime);
}


//void MSCFModel::saveState(std::ostream &os) {}

