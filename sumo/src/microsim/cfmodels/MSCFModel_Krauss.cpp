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
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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
    return MIN2(_vsafe(gap, 0, 0), maxNextSpeed(speed, veh));
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


/** Returns the SK-vsafe. */
SUMOReal
MSCFModel_Krauss::_vsafe(SUMOReal gap, SUMOReal predSpeed, SUMOReal predMaxDecel) const {
    if (predSpeed < predMaxDecel) {
        // avoid discretization error at low speeds
        predSpeed = 0;
    }
    if (predSpeed == 0) {
        if (gap < 0.01) {
            return 0;
        }
        // g = t * x + x^2 / (2 * b)
        const SUMOReal result = (SUMOReal)(-myTauDecel + sqrt(myTauDecel * myTauDecel + 2. * myDecel * gap));
        assert(result >= 0);
        return result;
    }
    if (gap < 0) {
        gap = 0;
    }
    // follow the leader
    // g=gap, t=myHeadwayTime, a=predMaxDecel, b=myDecel, v=predSpeed, x=vSafe
    // Solution approach: equal distances after leader and follower have stopped (partly discretized).
    // g + (v^2 - a*v)/(2*a) = x*t + (x^2 - b*x)/(2*b) + 0.5
    // The term (+ 0.5) gives an upper bound for the follower stopping distance to handle discretization errors.
    // Unfortunately, the solution approach is not correct when b > a since the
    // follower path may cross the leader path even with equal stopping distances.
    // As a workaround we lower the value of b to get a collision free model
    // This approach should be refined to get a higher (still safe) following speed.
    const SUMOReal egoDecel = MIN2(myDecel, predMaxDecel);
    const SUMOReal result = (SUMOReal)(0.5 * sqrt(
                                           4.0 * egoDecel * (2.0 * gap + predSpeed * predSpeed / predMaxDecel - predSpeed - 1.0)
                                           + (egoDecel * (2.0 * myHeadwayTime - 1.0))
                                           * (egoDecel * (2.0 * myHeadwayTime - 1.0)))
                                       + egoDecel * (0.5 - myHeadwayTime));
    // XXX recheck use of both branches of the quadratic forumula
    if (ISNAN(result)) {
        return 0;
    } else {
        return MAX2((SUMOReal)0, result);
    }
}


MSCFModel*
MSCFModel_Krauss::duplicate(const MSVehicleType* vtype) const {
    return new MSCFModel_Krauss(vtype, myAccel, myDecel, myDawdle, myHeadwayTime);
}


//void MSCFModel::saveState(std::ostream &os) {}

