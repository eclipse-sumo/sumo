/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2017 German Aerospace Center (DLR) and others.
/****************************************************************************/
//
//   This program and the accompanying materials
//   are made available under the terms of the Eclipse Public License v2.0
//   which accompanies this distribution, and is available at
//   http://www.eclipse.org/legal/epl-v20.html
//
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
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include <utils/common/RandHelper.h>


// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel_Krauss::MSCFModel_Krauss(const MSVehicleType* vtype, double accel, double decel,
                                   double emergencyDecel, double apparentDecel,
                                   double dawdle, double headwayTime) :
    MSCFModel_KraussOrig1(vtype, accel, decel, emergencyDecel, apparentDecel, dawdle, headwayTime) {
}


MSCFModel_Krauss::~MSCFModel_Krauss() {}


double
MSCFModel_Krauss::moveHelper(MSVehicle* const veh, double vPos) const {
    const double oldV = veh->getSpeed(); // save old v for optional acceleration computation
    const double vSafe = MIN2(vPos, veh->processNextStop(vPos)); // process stops
    // we need the acceleration for emission computation;
    //  in this case, we neglect dawdling, nonetheless, using
    //  vSafe does not incorporate speed reduction due to interaction
    //  on lane changing
    const double vMin = minNextSpeed(oldV, veh);
    // do not exceed max decel even if it is unsafe
    double vMax = MAX2(vMin,
                       MIN3(veh->getLane()->getVehicleMaxSpeed(veh), maxNextSpeed(oldV, veh), vSafe));
#ifdef _DEBUG
    //if (vMin > vMax) {
    //    WRITE_WARNING("Maximum speed of vehicle '" + veh->getID() + "' is lower than the minimum speed (min: " + toString(vMin) + ", max: " + toString(vMax) + ").");
    //}
#endif

    const double sigma = (veh->passingMinor() 
            ? veh->getVehicleType().getParameter().getJMParam(SUMO_ATTR_JM_SIGMA_MINOR, myDawdle) 
            : myDawdle);
    const double vDawdle = MAX2(vMin, dawdle(vMax, sigma));

    double vNext = veh->getLaneChangeModel().patchSpeed(vMin, vDawdle, vMax, *this);

#ifdef DEBUG_MOVE_HELPER
    if DEBUG_COND {
    std::cout << "\nMOVE_HELPER\n"
    << "veh '" << veh->getID() << "' vMin=" << vMin
        << " vMax=" << vMax << " vDawdle=" << vDawdle
        << " vSafe" << vSafe << " vNext=" << vNext << " vPos=" << vPos << " veh->getSpeed()=" << oldV
        << "\n";
    }
#endif

    // (Leo) At this point vNext may also be negative indicating a stop within next step.
    // This would have resulted from a call to maximumSafeStopSpeed(), which does not
    // consider deceleration bounds. Therefore, we cap vNext here.
    if (!MSGlobals::gSemiImplicitEulerUpdate) {
//        vNext = MAX2(vNext, veh->getSpeed() - ACCEL2SPEED(getMaxDecel()));
        vNext = MAX2(vNext, minNextSpeed(veh->getSpeed(), veh));
    }

    return vNext;
}



double
MSCFModel_Krauss::stopSpeed(const MSVehicle* const veh, const double speed, double gap) const {
    // NOTE: This allows return of smaller values than minNextSpeed().
    // Only relevant for the ballistic update: We give the argument headway=TS, to assure that
    // the stopping position is approached with a uniform deceleration also for tau!=TS.
    return MIN2(maximumSafeStopSpeed(gap, speed, false, TS), maxNextSpeed(speed, veh));
}


double
MSCFModel_Krauss::followSpeed(const MSVehicle* const veh, double speed, double gap, double predSpeed, double predMaxDecel) const {
    const double vsafe = maximumSafeFollowSpeed(gap, speed, predSpeed, predMaxDecel);
    const double vmin = minNextSpeed(speed);
    const double vmax = maxNextSpeed(speed, veh);
    if (MSGlobals::gSemiImplicitEulerUpdate) {
        return MIN2(vsafe, vmax);
    } else {
        // ballistic
        // XXX: the euler variant can break as strong as it wishes immediately! The ballistic cannot, refs. #2575.
        return MAX2(MIN2(vsafe, vmax), vmin);
    }
}


double
MSCFModel_Krauss::dawdle(double speed, double sigma) const {
    if (!MSGlobals::gSemiImplicitEulerUpdate) {
        // in case of the ballistic update, negative speeds indicate
        // a desired stop before the completion of the next timestep.
        // We do not allow dawdling to overwrite this indication
        if (speed < 0) {
            return speed;
        }
    }
    // generate random number out of [0,1)
    const double random = RandHelper::rand();
    // Dawdle.
    if (speed < myAccel) {
        // we should not prevent vehicles from driving just due to dawdling
        //  if someone is starting, he should definitely start
        // (but what about slow-to-start?)!!!
        speed -= ACCEL2SPEED(sigma * speed * random);
    } else {
        speed -= ACCEL2SPEED(sigma * myAccel * random);
    }
    return MAX2(0., speed);
}


MSCFModel*
MSCFModel_Krauss::duplicate(const MSVehicleType* vtype) const {
    return new MSCFModel_Krauss(vtype, myAccel, myDecel, myEmergencyDecel, myApparentDecel, myDawdle, myHeadwayTime);
}


/****************************************************************************/
