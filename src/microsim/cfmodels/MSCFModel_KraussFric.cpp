/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    MSCFModel_KraussFric.cpp
/// @author  Tobias Mayer
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Mon, 04 Aug 2009
///
// Krauss car-following model, with acceleration decrease and faster start
/****************************************************************************/
#include <config.h>

#include <microsim/MSVehicle.h>
#include <microsim/MSLane.h>
#include <microsim/MSGlobals.h>
#include "MSCFModel_KraussFric.h"
#include "MSCFModel_Krauss.h"
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include <utils/common/RandHelper.h>



// ===========================================================================
// DEBUG constants
// ===========================================================================
//#define DEBUG_COND (true)
#define DEBUG_COND (veh->isSelected())
#define DEBUG_DRIVER_ERRORS


// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel_KraussFric::MSCFModel_KraussFric(const MSVehicleType* vtype) :
    MSCFModel_Krauss(vtype) {
}


MSCFModel_KraussFric::~MSCFModel_KraussFric() {}


double
MSCFModel_KraussFric::patchSpeedBeforeLC(const MSVehicle* veh, double vMin, double vMax) const {
    const double sigma = (veh->passingMinor()
                          ? veh->getVehicleType().getParameter().getJMParam(SUMO_ATTR_JM_SIGMA_MINOR, myDawdle)
                          : myDawdle);
    const double vDawdle = MAX2(vMin, dawdle2(vMax, sigma, veh->getRNG()));
    return vDawdle;
}


double
MSCFModel_KraussFric::stopSpeed(const MSVehicle* const veh, const double speed, double gap, double decel) const {
    // NOTE: This allows return of smaller values than minNextSpeed().
    // Only relevant for the ballistic update: We give the argument headway=veh->getActionStepLengthSecs(), to assure that
    // the stopping position is approached with a uniform deceleration also for tau!=veh->getActionStepLengthSecs().
    applyHeadwayPerceptionError(veh, speed, gap);
    return MIN2(maximumSafeStopSpeed(gap, decel, speed, false, veh->getActionStepLengthSecs()), maxNextSpeed(speed, veh));
}

double
MSCFModel_KraussFric::maxNextSpeed(double speed, const MSVehicle* /*const veh*/) const {

	//const double factor = 5./6. + (1./6. * veh->getLane()->getFrictionCoefficient()); //y = 1/3 friction + 2/3 --> 0,7 = 90%, 0,4 = 80%
	const double vMax = (speed + (double) ACCEL2SPEED(getMaxAccel()));
    return MIN2(vMax, myType->getMaxSpeed());
}


double
MSCFModel_KraussFric::followSpeed(const MSVehicle* const veh, double speed, double gap, double predSpeed, double predMaxDecel, const MSVehicle* const pred) const {
    //gDebugFlag1 = DEBUG_COND;
    applyHeadwayAndSpeedDifferencePerceptionErrors(veh, speed, gap, predSpeed, predMaxDecel, pred);
    //gDebugFlag1 = DEBUG_COND; // enable for DEBUG_EMERGENCYDECEL
    const double vsafe = maximumSafeFollowSpeed(gap, speed, predSpeed, predMaxDecel);
    //gDebugFlag1 = false;
    const double vmin = minNextSpeedEmergency(speed);
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
MSCFModel_KraussFric::dawdle2(double speed, double sigma, SumoRNG* rng) const {
    if (!MSGlobals::gSemiImplicitEulerUpdate) {
        // in case of the ballistic update, negative speeds indicate
        // a desired stop before the completion of the next timestep.
        // We do not allow dawdling to overwrite this indication
        if (speed < 0) {
            return speed;
        }
    }
    // generate random number out of [0,1)
    const double random = RandHelper::rand(rng);
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

double
MSCFModel_KraussFric::finalizeSpeed(MSVehicle* const veh, double vPos) const {
	// save old v for optional acceleration computation
	const double oldV = veh->getSpeed();
	// process stops (includes update of stopping state)
	const double vStop = MIN2(vPos, veh->processNextStop(vPos));
	// apply deceleration bounds
	const double vMin = minNextSpeed(oldV, veh);
	// aMax: Maximal admissible acceleration until the next action step, such that the vehicle's maximal
	// desired speed on the current lane will not be exceeded when the
	// acceleration is maintained until the next action step.

	const double fric = veh->getLane()->getFrictionCoefficient(); //change with function later
	//const double factor = 2./3. + (1./3. * fric); //y = 1/3*friction + 2/3 --> 0,7 = 90%, 0,4 = 80%
	const double factor = -0.3491 * fric * fric + 0.8922 * fric + 0.4493; //2nd degree polyfit
	//const double factor = 0.4480 * fric + 0.5720; //line polyfit

	// Adapt speed Limit of Road to "Drivers" desire to reduce Speed Limit
	double aMax = ( (veh->getLane()->getVehicleMaxSpeed(veh) * factor) - oldV) / veh->getActionStepLengthSecs();
	
	// apply planned speed constraints and acceleration constraints
	double vMax = MIN3( (oldV + ACCEL2SPEED(aMax)), maxNextSpeed(oldV, veh), vStop);
	// do not exceed max decel even if it is unsafe
#ifdef _DEBUG
	//if (vMin > vMax) {
	//    WRITE_WARNING("Maximum speed of vehicle '" + veh->getID() + "' is lower than the minimum speed (min: " + toString(vMin) + ", max: " + toString(vMax) + ").");
	//}
#endif

#ifdef DEBUG_FINALIZE_SPEED
	if DEBUG_COND{
		std::cout << "\n" << SIMTIME << " FINALIZE_SPEED\n";
	}
#endif

	vMax = MAX2(vMin, vMax);
	// apply further speed adaptations
	double vNext = patchSpeedBeforeLC(veh, vMin, vMax);
	//apply factor proir last LC patch
	//vNext = vNext*factor;
	// apply lane-changing related speed adaptations
	vNext = veh->getLaneChangeModel().patchSpeed(vMin, vNext, vMax, *this);
	assert(vNext >= vMin);
	assert(vNext <= vMax);

#ifdef DEBUG_FINALIZE_SPEED
	if DEBUG_COND{
		std::cout << "veh '" << veh->getID() << "' oldV=" << oldV
		<< " vMin=" << vMin
		<< " vMax=" << vMax
		<< " vPos" << vStop
		<< " vStop" << vStop
		<< " vNext=" << vNext
		<< "\n";
	}
#endif
	return vNext;
}


MSCFModel*
MSCFModel_KraussFric::duplicate(const MSVehicleType* vtype) const {
    return new MSCFModel_KraussFric(vtype);
}


/****************************************************************************/
