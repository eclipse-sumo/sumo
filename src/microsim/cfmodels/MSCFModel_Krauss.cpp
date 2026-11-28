/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    MSCFModel_Krauss.cpp
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
#include "MSCFModel_Krauss.h"
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include <utils/common/RandHelper.h>
#include <utils/xml/SUMOSAXAttributes.h>



// ===========================================================================
// DEBUG constants
// ===========================================================================
//#define DEBUG_COND (true)
#define DEBUG_COND (veh->isSelected())
#define DEBUG_DRIVER_ERRORS

MSCFModel_Krauss::VehicleVariables::VehicleVariables(SUMOTime dawdleStep)
    : accelDawdle(1e6), updateOffset((SIMSTEP % dawdleStep + DELTA_T) % dawdleStep) { }

// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel_Krauss::MSCFModel_Krauss(const MSVehicleType* vtype) :
    MSCFModel_KraussOrig1(vtype),
    myDawdleStep(TIME2STEPS(vtype->getParameter().getCFParam(SUMO_ATTR_SIGMA_STEP, TS))) {
    const SUMOTime requested = myDawdleStep;
    const SUMOTime remainder = requested % DELTA_T;
    myDawdleStep = MAX2(DELTA_T, requested + (remainder < DELTA_T - remainder ? -remainder : DELTA_T - remainder));
    if (myDawdleStep != requested && vtype->getParameter().cfParameter.count(SUMO_ATTR_SIGMA_STEP) != 0) {
        WRITE_WARNINGF(TL("Rounding 'sigmaStep' to % for vType '%'"), STEPS2TIME(myDawdleStep), vtype->getID());
    }
}


MSCFModel_Krauss::~MSCFModel_Krauss() {}


void
MSCFModel_Krauss::VehicleVariables::saveState(OutputDevice& out, const MSCFModel& /*cfm*/) const {
    out.openTag(SUMO_TAG_CFM_VARIABLES);
    out.writeAttr(SUMO_ATTR_ID, "Krauss");
    std::ostringstream internals;
    internals << accelDawdle << " ";
    internals << updateOffset;
    out.writeAttr(SUMO_ATTR_STATE, internals.str());
    out.closeTag();
}


void
MSCFModel_Krauss::VehicleVariables::loadState(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    const std::string cfmID = attrs.get<std::string>(SUMO_ATTR_ID, nullptr, ok);
    if (cfmID != "Krauss") {
        throw ProcessError(TLF("incompatible carFollowModel '%' when loading state for Krauss", cfmID));
    }
    std::istringstream bis(attrs.getString(SUMO_ATTR_STATE));
    bis >> accelDawdle;
    bis >> updateOffset;
}


double
MSCFModel_Krauss::patchSpeedBeforeLC(const MSVehicle* veh, double vMin, double vMax) const {
    const double sigma = (veh->passingMinor()
                          ? veh->getVehicleType().getParameter().getJMParam(SUMO_ATTR_JM_SIGMA_MINOR, myDawdle)
                          : myDawdle);
    if (myDawdleStep <= DELTA_T) {
        return MAX2(vMin, dawdle2(vMax, sigma, veh->getRNG()));
    }
    VehicleVariables* vars = (VehicleVariables*)veh->getCarFollowVariables();
    // Update at the first action step at or after the scheduled dawdling step.
    const SUMOTime phase = (SIMSTEP % myDawdleStep - vars->updateOffset % myDawdleStep + myDawdleStep) % myDawdleStep;
    double desiredSpeed = vMax;
    if (phase < veh->getActionStepLength()) {
        desiredSpeed = MAX2(vMin, dawdle2(vMax, sigma, veh->getRNG()));
        const double speed = veh->getSpeed();
        const double maxAccel = SPEED2ACCEL(maxNextSpeed(speed, veh) - speed);
        const double limitAccel = (veh->getLane()->getVehicleMaxSpeed(veh) - speed) / STEPS2TIME(myDawdleStep);
        const double dawdleAccel = SPEED2ACCEL(desiredSpeed - vMax);
        // Hold only the free-flow acceleration and dawdling. Safety limits can change every step.
        vars->accelDawdle = MAX2(0., MIN2(maxAccel, limitAccel)) + dawdleAccel;
    }
    if (sigma == 0.) {
        return vMax;
    }
    return MAX2(vMin, MIN2(desiredSpeed, veh->getSpeed() + ACCEL2SPEED(vars->accelDawdle)));
}


double
MSCFModel_Krauss::stopSpeed(const MSVehicle* const veh, const double speed, double gap, double decel, const CalcReason usage) const {
    // NOTE: This allows return of smaller values than minNextSpeed().
    // Only relevant for the ballistic update: We give the argument headway=veh->getActionStepLengthSecs(), to assure that
    // the stopping position is approached with a uniform deceleration also for tau!=veh->getActionStepLengthSecs().
    applyHeadwayPerceptionError(veh, speed, gap);
    const bool relaxEmergency = usage != FUTURE; // do not relax insertionStopSpeed
    return MIN2(maximumSafeStopSpeed(gap, decel, speed, false, veh->getActionStepLengthSecs(), relaxEmergency), maxNextSpeed(speed, veh));
}


double
MSCFModel_Krauss::followSpeed(const MSVehicle* const veh, double speed, double gap, double predSpeed, double predMaxDecel, const MSVehicle* const pred, const CalcReason /*usage*/) const {
    //gDebugFlag1 = DEBUG_COND;
    applyHeadwayAndSpeedDifferencePerceptionErrors(veh, speed, gap, predSpeed, predMaxDecel, pred);
    //gDebugFlag1 = DEBUG_COND; // enable for DEBUG_EMERGENCYDECEL
    const double vsafe = maximumSafeFollowSpeed(gap, speed, predSpeed, predMaxDecel);
    //gDebugFlag1 = false;
    const double vmax = maxNextSpeed(speed, veh);
    if (MSGlobals::gSemiImplicitEulerUpdate) {
        return MIN2(vsafe, vmax);
    } else {
        // ballistic
        // XXX: the euler variant can break as strong as it wishes immediately! The ballistic cannot, refs. #2575.
        return MAX2(MIN2(vsafe, vmax), minNextSpeedEmergency(speed));
    }
}

double
MSCFModel_Krauss::dawdle2(double speed, double sigma, SumoRNG* rng) const {
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


MSCFModel*
MSCFModel_Krauss::duplicate(const MSVehicleType* vtype) const {
    return new MSCFModel_Krauss(vtype);
}


/****************************************************************************/
