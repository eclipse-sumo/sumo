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

#include <microsim/MSVehicle.h>
#include <microsim/MSLane.h>
#include "MSCFModel_KraussOrig1.h"
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include <utils/common/RandHelper.h>
#include <microsim/MSGlobals.h>

//#define DEBUG_EXECUTE_MOVE
#define DEBUG_COND (veh->getID()=="disabled")

// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel_KraussOrig1::MSCFModel_KraussOrig1(const MSVehicleType* vtype,  SUMOReal accel, SUMOReal decel,
        SUMOReal dawdle, SUMOReal headwayTime)
    : MSCFModel(vtype, accel, decel, headwayTime), myDawdle(dawdle), myTauDecel(decel * headwayTime) {}


MSCFModel_KraussOrig1::~MSCFModel_KraussOrig1() {}


SUMOReal
MSCFModel_KraussOrig1::moveHelper(MSVehicle* const veh, SUMOReal vPos) const {
    const SUMOReal oldV = veh->getSpeed(); // save old v for optional acceleration computation
    const SUMOReal vSafe = MIN2(vPos, veh->processNextStop(vPos)); // process stops
    // we need the acceleration for emission computation;
    //  in this case, we neglect dawdling, nonetheless, using
    //  vSafe does not incorporate speed reduction due to interaction
    //  on lane changing
    const SUMOReal vMin = minNextSpeed(oldV, veh);
    // do not exceed max decel even if it is unsafe
    SUMOReal vMax = MAX2(vMin,
                         MIN3(veh->getLane()->getVehicleMaxSpeed(veh), maxNextSpeed(oldV, veh), vSafe));
#ifdef _DEBUG
    //if (vMin > vMax) {
    //    WRITE_WARNING("Maximum speed of vehicle '" + veh->getID() + "' is lower than the minimum speed (min: " + toString(vMin) + ", max: " + toString(vMax) + ").");
    //}
#endif

    const SUMOReal vDawdle = MAX2(vMin, dawdle(vMax));

    SUMOReal vNext = veh->getLaneChangeModel().patchSpeed(vMin, vDawdle, vMax, *this);

#ifdef DEBUG_EXECUTE_MOVE
    if DEBUG_COND {
    std::cout << "\nMOVE_HELPER\n"
    << "veh '" << veh->getID() << "' vMin=" << vMin
        << " vMax=" << vMax << " vDawdle=" << vDawdle
        << " vSafe" << vSafe << " vNext=" << vNext
        << "\n";
    }
#endif

    // (Leo) At this point vNext may also be negative indicating a stop within next step.
    // This would have resulted from a call to maximumSafeStopSpeed(), which does not
    // consider deceleration bounds. Therefore, we cap vNext here.
    if (!MSGlobals::gSemiImplicitEulerUpdate) {
        vNext = MAX2(vNext, veh->getSpeed() - ACCEL2SPEED(getMaxDecel()));
    }

    return vNext;
}


SUMOReal
MSCFModel_KraussOrig1::followSpeed(const MSVehicle* const veh, SUMOReal speed, SUMOReal gap, SUMOReal predSpeed, SUMOReal predMaxDecel) const {
    if (MSGlobals::gSemiImplicitEulerUpdate) {
        return MIN2(vsafe(gap, predSpeed, predMaxDecel), maxNextSpeed(speed, veh)); // XXX: and why not cap with minNextSpeed!? (Leo)
    } else {
        return MAX2(MIN2(maximumSafeFollowSpeed(gap, speed, predSpeed, predMaxDecel), maxNextSpeed(speed, veh)), minNextSpeed(speed));
    }
}


SUMOReal
MSCFModel_KraussOrig1::insertionFollowSpeed(const MSVehicle* const veh, SUMOReal speed, SUMOReal gap2pred, SUMOReal predSpeed, SUMOReal predMaxDecel) const {
    if (MSGlobals::gSemiImplicitEulerUpdate) {
        return followSpeed(veh, speed, gap2pred, predSpeed, predMaxDecel);
    } else {
        // ballistic update
        return maximumSafeFollowSpeed(gap2pred, 0., predSpeed, predMaxDecel, true);
    }
}


SUMOReal
MSCFModel_KraussOrig1::stopSpeed(const MSVehicle* const veh, const SUMOReal speed, SUMOReal gap) const {
    if (MSGlobals::gSemiImplicitEulerUpdate) {
        return MIN2(vsafe(gap, 0., 0.), maxNextSpeed(speed, veh));
    } else {
        // XXX: using this here is probably in the spirit of Krauss, but we should consider,
        // if the original vsafe should be kept instead (Leo), refs. #2575
        return MIN2(maximumSafeStopSpeedBallistic(gap, speed), maxNextSpeed(speed, veh));
    }
}


SUMOReal
MSCFModel_KraussOrig1::dawdle(SUMOReal speed) const {
    if (!MSGlobals::gSemiImplicitEulerUpdate) {
        // in case of the ballistic update, negative speeds indicate
        // a desired stop before the completion of the next timestep.
        // We do not allow dawdling to overwrite this indication
        if (speed < 0) {
            return speed;
        }
    }
    return MAX2(SUMOReal(0), speed - ACCEL2SPEED(myDawdle * myAccel * RandHelper::rand()));
}


/** Returns the SK-vsafe. */
SUMOReal MSCFModel_KraussOrig1::vsafe(SUMOReal gap, SUMOReal predSpeed, SUMOReal /* predMaxDecel */) const {
    if (predSpeed == 0 && gap < 0.01) {
        return 0;
    } else if (predSpeed == 0 &&  gap <= ACCEL2SPEED(myDecel)) {
        // workaround for #2310
        return MIN2(ACCEL2SPEED(myDecel), DIST2SPEED(gap));
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
