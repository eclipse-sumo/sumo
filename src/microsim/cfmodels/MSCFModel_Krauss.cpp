/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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
// DEBUG constants
// ===========================================================================
//#define DEBUG_COND (true)


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
MSCFModel_Krauss::patchSpeedBeforeLC(const MSVehicle* veh, double vMin, double vMax) const {
    const double sigma = (veh->passingMinor()
                          ? veh->getVehicleType().getParameter().getJMParam(SUMO_ATTR_JM_SIGMA_MINOR, myDawdle)
                          : myDawdle);
    const double vDawdle = MAX2(vMin, dawdle2(vMax, sigma));
    return vDawdle;
}


double
MSCFModel_Krauss::stopSpeed(const MSVehicle* const veh, const double speed, double gap) const {
    // NOTE: This allows return of smaller values than minNextSpeed().
    // Only relevant for the ballistic update: We give the argument headway=veh->getActionStepLengthSecs(), to assure that
    // the stopping position is approached with a uniform deceleration also for tau!=veh->getActionStepLengthSecs().
    return MIN2(maximumSafeStopSpeed(gap, speed, false, veh->getActionStepLengthSecs()), maxNextSpeed(speed, veh));
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
MSCFModel_Krauss::dawdle2(double speed, double sigma) const {
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
