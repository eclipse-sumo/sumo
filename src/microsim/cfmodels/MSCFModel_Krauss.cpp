/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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



// ===========================================================================
// DEBUG constants
// ===========================================================================
//#define DEBUG_COND (true)
#define DEBUG_COND (veh->isSelected())
#define DEBUG_DRIVER_ERRORS

MSCFModel_Krauss::VehicleVariables::VehicleVariables(SUMOTime dawdleStep)
    : accelDawdle(1e6), updateOffset(SIMSTEP % dawdleStep + DELTA_T) { }

// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel_Krauss::MSCFModel_Krauss(const MSVehicleType* vtype) :
    MSCFModel_KraussOrig1(vtype),
    myDawdleStep(TIME2STEPS(vtype->getParameter().getCFParam(SUMO_ATTR_SIGMA_STEP, TS))) {
    if (myDawdleStep % DELTA_T != 0) {
        SUMOTime rem = myDawdleStep % DELTA_T;
        if (rem < DELTA_T / 2) {
            myDawdleStep += -rem;
        } else {
            myDawdleStep += DELTA_T - rem;
        }
        WRITE_WARNINGF(TL("Rounding 'sigmaStep' to % for vType '%'"), STEPS2TIME(myDawdleStep), vtype->getID());

    }
}


MSCFModel_Krauss::~MSCFModel_Krauss() {}


double
MSCFModel_Krauss::patchSpeedBeforeLC(const MSVehicle* veh, double vMin, double vMax) const {
    const double sigma = (veh->passingMinor()
                          ? veh->getVehicleType().getParameter().getJMParam(SUMO_ATTR_JM_SIGMA_MINOR, myDawdle)
                          : myDawdle);
    double vDawdle;
    if (myDawdleStep > DELTA_T) {
        VehicleVariables* vars = (VehicleVariables*)veh->getCarFollowVariables();
        if (SIMSTEP % myDawdleStep == vars->updateOffset) {
            const double vD = MAX2(vMin, dawdle2(vMax, sigma, veh->getRNG()));
            const double a1 = SPEED2ACCEL(vMax - veh->getSpeed());
            const double a2 = SPEED2ACCEL(vD - vMax);
            const double accelMax = (veh->getLane()->getVehicleMaxSpeed(veh) - veh->getSpeed()) / STEPS2TIME(myDawdleStep);
            // avoid exceeding maxSpeed before the next sigmaStep
            vars->accelDawdle = MIN2(a1, accelMax) + a2;
            vDawdle = veh->getSpeed() + ACCEL2SPEED(vars->accelDawdle);
            //std::cout << SIMTIME << " v=" << veh->getSpeed() << " updated vD=" << vD<< " a1=" << a1 << " a2=" << a2 << " aM=" << accelMax << " accelDawdle=" << vars->accelDawdle << " vDawdle=" << vDawdle << "\n";
        } else {
            const double safeAccel = SPEED2ACCEL(vMax - veh->getSpeed());
            const double accel = MIN2(safeAccel, vars->accelDawdle);
            vDawdle = MAX2(vMin, MIN2(vMax, veh->getSpeed() + ACCEL2SPEED(accel)));
            //std::cout << SIMTIME << " v=" << veh->getSpeed() << " safeAccel=" << safeAccel << " accel=" << accel << " vDawdle=" << vDawdle << "\n";
        }
    } else {
        vDawdle = MAX2(vMin, dawdle2(vMax, sigma, veh->getRNG()));
        //const double accel1 = SPEED2ACCEL(vMax - veh->getSpeed());
        //const double accel2 = SPEED2ACCEL(vDawdle - vMax);
        //std::cout << SIMTIME << " v=" << veh->getSpeed() << " updated vDawdle=" << vDawdle << " a1=" << accel1 << " a2=" << accel2 << " accelDawdle=" << SPEED2ACCEL(vDawdle - veh->getSpeed()) << "\n";
    }
    return vDawdle;
}


double
MSCFModel_Krauss::stopSpeed(const MSVehicle* const veh, const double speed, double gap, double decel, const CalcReason /*usage*/) const {
    // NOTE: This allows return of smaller values than minNextSpeed().
    // Only relevant for the ballistic update: We give the argument headway=veh->getActionStepLengthSecs(), to assure that
    // the stopping position is approached with a uniform deceleration also for tau!=veh->getActionStepLengthSecs().
    applyHeadwayPerceptionError(veh, speed, gap);
    return MIN2(maximumSafeStopSpeed(gap, decel, speed, false, veh->getActionStepLengthSecs()), maxNextSpeed(speed, veh));
}


double
MSCFModel_Krauss::followSpeed(const MSVehicle* const veh, double speed, double gap, double predSpeed, double predMaxDecel, const MSVehicle* const pred, const CalcReason /*usage*/) const {
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
