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
/// @file    MSCFModel_EIDM.cpp
/// @author  Dominik Salles
/// @date    Fri, 06 Jul 2018

/// Originalfile MSCFModel_IDM.cpp from
/// @author  Tobias Mayer
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Thu, 03 Sep 2009
///
// The Extended Intelligent Driver Model (EIDM) car-following model
//
// Publication: Salles, Dominik, S. Kaufmann and H. Reuss. “Extending the Intelligent Driver
// Model in SUMO and Verifying the Drive Off Trajectories with Aerial
// Measurements.” (2020).
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "MSCFModel_EIDM.h"
#include <microsim/MSVehicle.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLink.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOTime.h>

//#define DEBUG_V

#define EST_REAC_THRESHOLD 3. // under this threshold estimation, error and reaction time variables don't get taken into account
#define ClutchEngageSpeed 0.5 // When a vehicle is below this speed, we assume a "slow to start", that is because of clutch operation / powertrain inertia
#define EIDM_POS_ACC_EPS 0.05 // some slack number to ensure smoother position, speed and acceleration update

// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel_EIDM::MSCFModel_EIDM(const MSVehicleType* vtype) :
    MSCFModel(vtype), myDelta(vtype->getParameter().getCFParam(SUMO_ATTR_CF_IDM_DELTA, 4.)),
    myTwoSqrtAccelDecel(double(2 * sqrt(myAccel * myDecel))),
    myIterations(MAX2(1, int(TS / vtype->getParameter().getCFParam(SUMO_ATTR_CF_IDM_STEPPING, .25) + .5))),
    myTPersDrive(vtype->getParameter().getCFParam(SUMO_ATTR_CF_EIDM_T_PERSISTENCE_DRIVE, 3)),
    myTreaction(vtype->getParameter().getCFParam(SUMO_ATTR_CF_EIDM_T_REACTION, 0.5)),
    myTpreview(vtype->getParameter().getCFParam(SUMO_ATTR_CF_EIDM_T_LOOK_AHEAD, 4)),
    myTPersEstimate(vtype->getParameter().getCFParam(SUMO_ATTR_CF_EIDM_T_PERSISTENCE_ESTIMATE, 10)),
    myCcoolness(vtype->getParameter().getCFParam(SUMO_ATTR_CF_EIDM_C_COOLNESS, 0.99)),
    mySigmaleader(vtype->getParameter().getCFParam(SUMO_ATTR_CF_EIDM_SIG_LEADER, 0.02)),
    mySigmagap(vtype->getParameter().getCFParam(SUMO_ATTR_CF_EIDM_SIG_GAP, 0.1)),
    mySigmaerror(vtype->getParameter().getCFParam(SUMO_ATTR_CF_EIDM_SIG_ERROR, 0.04)),
    myJerkmax(vtype->getParameter().getCFParam(SUMO_ATTR_CF_EIDM_JERK_MAX, 3.)),
    myEpsilonacc(vtype->getParameter().getCFParam(SUMO_ATTR_CF_EIDM_EPSILON_ACC, 1.)),
    myTaccmax(vtype->getParameter().getCFParam(SUMO_ATTR_CF_EIDM_T_ACC_MAX, 1.2)),
    myMflatness(vtype->getParameter().getCFParam(SUMO_ATTR_CF_EIDM_M_FLATNESS, 2.)),
    myMbegin(vtype->getParameter().getCFParam(SUMO_ATTR_CF_EIDM_M_BEGIN, 0.7)),
    myUseVehDynamics(vtype->getParameter().getCFParam(SUMO_ATTR_CF_EIDM_USEVEHDYNAMICS, 0) == 1)
    //, myMaxVehPreview(vtype->getParameter().getCFParam(SUMO_ATTR_CF_EIDM_MAX_VEH_PREVIEW, 0))
{
    // IDM does not drive very precise and may violate minGap on occasion
    myCollisionMinGapFactor = vtype->getParameter().getCFParam(SUMO_ATTR_COLLISION_MINGAP_FACTOR, 0.1);
}

MSCFModel_EIDM::~MSCFModel_EIDM() {}

double
MSCFModel_EIDM::insertionFollowSpeed(const MSVehicle* const /*veh*/, double speed, double gap2pred, double predSpeed, double predMaxDecel, const MSVehicle* const /*pred*/) const {
    if (MSGlobals::gSemiImplicitEulerUpdate) {
        return maximumSafeFollowSpeed(gap2pred, speed, predSpeed, predMaxDecel, true);
    } else {
        // Not Done/checked yet for the ballistic update model!!!!
        // NOTE: Even for ballistic update, the current speed is irrelevant at insertion, therefore passing 0. (Leo)
//        return maximumSafeFollowSpeed(gap2pred, 0., predSpeed, predMaxDecel, true);
        return maximumSafeFollowSpeed(gap2pred, speed, predSpeed, predMaxDecel, true);
    }
}


double
MSCFModel_EIDM::insertionStopSpeed(const MSVehicle* const /*veh*/, double speed, double gap) const {
    if (MSGlobals::gSemiImplicitEulerUpdate) {
        return maximumSafeStopSpeed(gap, myDecel, speed, true, myHeadwayTime);
    } else {
        // Not Done/checked yet for the ballistic update model!!!!
//        return MIN2(maximumSafeStopSpeed(gap, myDecel, 0., true, 0.), myType->getMaxSpeed());
        return MIN2(maximumSafeStopSpeed(gap, myDecel, speed, true, myHeadwayTime), myType->getMaxSpeed());
    }
}

double
MSCFModel_EIDM::maximumSafeFollowSpeed(double gap, double egoSpeed, double predSpeed, double predMaxDecel, bool onInsertion, const CalcReason /* usage */) const {
    double x;
    if (gap >= 0 || MSGlobals::gComputeLC) {
        double a = 1.;
        double b = myHeadwayTime * myTwoSqrtAccelDecel - predSpeed;
        double c = -sqrt(1 + myDecel / (2 * myAccel)) * gap * myTwoSqrtAccelDecel;
        // with myDecel/myAccel, the intended deceleration is myDecel
        // with myDecel/(2*myAccel), the intended deceleration is myDecel/2
        // with the IIDM, if gap=s, then the acceleration is zero and if gap<s, then the term v/vMax is not present

        // double c = -sqrt(1 - pow(egoSpeed / MAX2(NUMERICAL_EPS, desSpeed), myDelta) + myDecel / (2 * myAccel)) * gap * myTwoSqrtAccelDecel; // c calculation when using the IDM!

        // myDecel is positive, but is intended as negative value here, therefore + instead of -
        // quadratic formula
        x = (-b + sqrt(b * b - 4.*a * c)) / (2.*a);
    } else {
        x = egoSpeed - ACCEL2SPEED(myEmergencyDecel);
        if (MSGlobals::gSemiImplicitEulerUpdate) {
            x = MAX2(x, 0.);
        }
    }

    if (myDecel != myEmergencyDecel && !onInsertion && !MSGlobals::gComputeLC) {
        double origSafeDecel = SPEED2ACCEL(egoSpeed - x);
        if (origSafeDecel > myDecel + NUMERICAL_EPS) {
            // Braking harder than myDecel was requested -> calculate required emergency deceleration.
            // Note that the resulting safeDecel can be smaller than the origSafeDecel, since the call to maximumSafeStopSpeed() above
            // can result in corrupted values (leading to intersecting trajectories) if, e.g. leader and follower are fast (leader still faster) and the gap is very small,
            // such that braking harder than myDecel is required.

            double safeDecel = EMERGENCY_DECEL_AMPLIFIER * calculateEmergencyDeceleration(gap, egoSpeed, predSpeed, predMaxDecel);
#ifdef DEBUG_EMERGENCYDECEL
            if (DEBUG_COND2) {
                std::cout << SIMTIME << " initial vsafe=" << x
                          << " egoSpeed=" << egoSpeed << " (origSafeDecel=" << origSafeDecel << ")"
                          << " predSpeed=" << predSpeed << " (predDecel=" << predMaxDecel << ")"
                          << " safeDecel=" << safeDecel
                          << std::endl;
            }
#endif
            // Don't be riskier than the usual method (myDecel <= safeDecel may occur, because a headway>0 is used above)
            safeDecel = MAX2(safeDecel, myDecel);
            // don't brake harder than originally planned (possible due to euler/ballistic mismatch)
            safeDecel = MIN2(safeDecel, origSafeDecel);
            x = egoSpeed - ACCEL2SPEED(safeDecel);
            if (MSGlobals::gSemiImplicitEulerUpdate) {
                x = MAX2(x, 0.);
            }

#ifdef DEBUG_EMERGENCYDECEL
            if (DEBUG_COND2) {
                std::cout << "     -> corrected emergency deceleration: " << safeDecel << std::endl;
            }
#endif

        }
    }
    assert(x >= 0 || !MSGlobals::gSemiImplicitEulerUpdate);
    assert(!ISNAN(x));
    return x;
}

double
MSCFModel_EIDM::maximumSafeStopSpeed(double gap, double decel, double currentSpeed, bool onInsertion, double headway) const {
    double vsafe;
    if (MSGlobals::gSemiImplicitEulerUpdate) {
        const double g = gap - NUMERICAL_EPS;
        if (g < 0) {
            return 0;
        }
        double a = 1.;
        double b = headway * myTwoSqrtAccelDecel - 0.;
        double c = -sqrt(1 + decel / (2 * myAccel)) * g * myTwoSqrtAccelDecel;
        // with decel/myAccel, the intended deceleration is decel
        // with decel/(2*myAccel), the intended deceleration is decel/2
        // with the IIDM, if gap=s, then the acceleration is zero and if gap<s, then the term currentSpeed/vMax is not present

        // double c = -sqrt(1 - pow(currentSpeed / MAX2(NUMERICAL_EPS, desSpeed), myDelta) + decel / (2 * myAccel)) * g * myTwoSqrtAccelDecel; // c calculation when using the IDM!

        // decel is positive, but is intended as negative value here, therefore + instead of -
        // quadratic formula
        vsafe = (-b + sqrt(b * b - 4.*a * c)) / (2.*a);
    } else {
        // Not Done/checked yet for the ballistic update model!!!!
        vsafe = maximumSafeStopSpeedBallistic(gap, decel, currentSpeed, onInsertion, headway);
    }

    return vsafe;
}

double
MSCFModel_EIDM::patchSpeedBeforeLCEIDM(const MSVehicle* /*veh*/, double vMin, double vMax, const VehicleVariables* vars) const {
    // The default value of SUMO_ATTR_JM_SIGMA_MINOR is sigma, not sigmaerror (used in EIDM),
    // so we do not use SUMO_ATTR_JM_SIGMA_MINOR as parameter here, because this could confuse users...
    //const double sigma = (veh->passingMinor()
    //    ? veh->getVehicleType().getParameter().getJMParam(SUMO_ATTR_JM_SIGMA_MINOR, myDawdle)
    //    : myDawdle);

    // dawdling/drivingerror is now calculated here (in finalizeSpeed, not in stop-/follow-/freeSpeed anymore):
    // Instead of just multiplying mySigmaerror with vars->myw_error, we add a factor depending on the criticality of the situation,
    // measured with s*/gap. Because when the driver drives "freely" (nothing in front) he may dawdle more than in e.g. congested traffic!
    double s = MAX2(0., vars->myv_est * myHeadwayTime + vars->myv_est * (vars->myv_est - vars->myv_est_l) / myTwoSqrtAccelDecel);
    if (vars->myrespectMinGap) {
        s += myType->getMinGap() + EIDM_POS_ACC_EPS;
    } else {
        const double minGapStop_EPS = 0.05 + 0.20 * MAX2(0.25, myAccel);
        s += minGapStop_EPS + EIDM_POS_ACC_EPS;
    }
    const double intensity = MIN2(myAccel, MAX2(vMax - 0.5 * myAccel, 0.0));
    const double criticality = MIN2(MAX2(s / vars->mys_est - 0.5, -0.4), 0.0);

    const double drivingerror = mySigmaerror * vars->myw_error * intensity * (2.75 * 2.75 * criticality * criticality + 1.0);

    // else: the vehicle is very slow and we do not add driving error (= 0), because
    // we should not prevent vehicles from driving just due to dawdling
    // if someone is starting, he should definitely start

    //const double vDawdle = MAX2(vMin, dawdle2(vMax, sigma, veh->getRNG()));
    const double vDawdle = MAX2(vMin, vMax + ACCEL2SPEED(drivingerror));
    return vDawdle;
}

double
MSCFModel_EIDM::slowToStartTerm(MSVehicle* const veh, const double newSpeed, const double currentSpeed, const double vMax, VehicleVariables* vars) const {
    // Drive Off Activation and Term

    if (newSpeed == 0 || newSpeed <= currentSpeed) {
        return newSpeed;
    }

    double remainingDelay = 0.0;
    if (newSpeed != vMax) {
        remainingDelay = STEPS2TIME(DELTA_T - (myStartupDelay - (veh->getTimeSinceStartup() - DELTA_T)));
    }

    double v_corr = currentSpeed;
    for (int i = 0; i < myIterations; i++) {
        // @ToDo: Check if all clauses are still needed or if we need to add more for all possible drive off cases?!
        // When we reach this point, "newSpeed > currentSpeed" already holds
        // Activation of the Drive Off term, when
        if (currentSpeed < ClutchEngageSpeed && // The start speed is lower than ClutchEngageSpeed m/s
                vars->t_off + 4. - NUMERICAL_EPS < (SIMTIME - remainingDelay - TS * (myIterations - i - 1.) / myIterations) && // the last activation is at least 4 seconds ago
                vars->myap_update == 0 && // the last activation is at least 4 seconds ago AND an Action Point was reached
                veh->getAcceleration() < MIN2(myAccel / 4, 0.2)) { // && respectMinGap) { // the driver hasn't started accelerating yet (<0.2)
            vars->t_off = (SIMTIME - remainingDelay - TS * (myIterations - i - 1.) / myIterations); // activate the drive off term
        }
        // Calculation of the Drive Off term
        if (vars->t_off + myTaccmax + NUMERICAL_EPS > (SIMTIME - remainingDelay - TS * (myIterations - i - 1.) / myIterations)) {
            v_corr = v_corr + (newSpeed - currentSpeed) / myIterations * (tanh((((SIMTIME - remainingDelay - TS * (myIterations - i - 1.) / myIterations) - vars->t_off) * 2. / myTaccmax - myMbegin) * myMflatness) + 1.) / 2.;
        } else {
            v_corr = v_corr + (newSpeed - currentSpeed) / myIterations;
        }
    }
    return v_corr;
}

double
MSCFModel_EIDM::finalizeSpeed(MSVehicle* const veh, double vPos) const {
    // finalizeSpeed is only called once every timestep!

    VehicleVariables* vars = (VehicleVariables*)veh->getCarFollowVariables();
    // save old v for optional acceleration computation
    const double oldV = veh->getSpeed();

    // @ToDo: Maybe change whole calculation to calculate real freeSpeed/stopSpeed/followSpeed in every call and here calculate resulting speed with reaction Time and update?!
    // @ToDo: Could check which call resulted in speed update with stop-vector containing all calculated accelerations!
    // Check whether the speed update results from a stop calculation, if so, run _v-function again with the saved variables from stopSpeed
    double _vPos = vPos;
    if ((vPos <= SUMO_const_haltingSpeed && vPos <= oldV) || !(vPos > oldV + ACCEL2SPEED(vars->realacc) - NUMERICAL_EPS && vPos < oldV + ACCEL2SPEED(vars->realacc) + NUMERICAL_EPS)) {
        for (auto it = vars->stop.cbegin(); it != vars->stop.cend(); ++it) {
            if (vPos > oldV + ACCEL2SPEED(it->first) - NUMERICAL_EPS && vPos < oldV + ACCEL2SPEED(it->first) + NUMERICAL_EPS) {
                _vPos = _v(veh, it->second, oldV, 0, vars->v0_int, false, 1, CalcReason::CURRENT);
            }
        }
    }

    // process stops (includes update of stopping state)
    const double vStop = MIN2(_vPos, veh->processNextStop(_vPos));
    // apply deceleration bounds
    const double vMinEmergency = minNextSpeedEmergency(oldV, veh);
    // _vPos contains the uppper bound on safe speed. allow emergency braking here
    const double vMin = MIN2(minNextSpeed(oldV, veh), MAX2(_vPos, vMinEmergency));
    // apply planned speed constraints and acceleration constraints
    double vMax = MIN2(maxNextSpeed(oldV, veh), vStop);
    vMax = MAX2(vMin, vMax);

#ifdef DEBUG_V
    if (veh->isSelected()) {
        std::cout << SIMTIME
                  << " EIDM::finalizeSpeed "
                  << " veh=" << veh->getID()
                  << " oldV=" << oldV
                  << " vPos=" << vPos
                  << " _vPos=" << _vPos
                  << " vStop=" << vStop
                  << " vMinEmergency=" << vMinEmergency
                  << " vMin=" << vMin
                  << " vMax=" << vMax
                  << "\n";
    }
#endif

    // apply further speed adaptations
    double vNext = patchSpeedBeforeLCEIDM(veh, vMin, vMax, vars);

    if (MSGlobals::gSemiImplicitEulerUpdate) {

        // The model does not directly use vNext from patchSpeed (like the original MSCFModel::finalizeSpeed function),
        // but rather slowly adapts to vNext.
        vNext = veh->getLaneChangeModel().patchSpeed(vMin, vNext, vMax, *this);

        // Bound the positive change of the acceleration with myJerkmax
        if (vNext > oldV && oldV > ClutchEngageSpeed * 2 && vars->t_off + myTaccmax + NUMERICAL_EPS < SIMTIME) {
            // At junctions with minor priority acceleration will still jump because after finalizeSpeed "MAX2(vNext, vSafeMin)" is called, vSafeMin is higher and vNext from finalizeSpeed is then ignored!!!
            // If we put this jmax-Part into _v-function (via old calc_gap implementation), then vehicle can't drive over junction because it thinks it won't make it in time before a foe may appear!
            if (myJerkmax * TS + veh->getAcceleration() < 0.) { // If driver wants to accelerate, but is still decelerating, then we use a factor of 2!
                vNext = MAX2(oldV + MIN2(vNext - oldV, (myJerkmax * 2 * TS + veh->getAcceleration()) * TS), 0.); // change in acceleration (jerk) is bounded by myJerkmax*2
            } else {
                vNext = MAX2(oldV + MIN2(vNext - oldV, (myJerkmax * TS + veh->getAcceleration()) * TS), 0.); // change in acceleration (jerk) is bounded by myJerkmax
            }
        } else if (vNext <= oldV && vNext < vMax - NUMERICAL_EPS && oldV > ClutchEngageSpeed * 2) {
            // Slowing down the deceleration like this may be critical!!! Vehicle can also not come back from Emergency braking fast enough!
            /*if (vNext - oldV < -myJerkmax * TS + veh->getAcceleration()) { // driver wants to brake harder than before, change in acceleration is then bounded by -myJerkmax
                vNext = MAX2(oldV + (-myJerkmax * TS + veh->getAcceleration()) * TS, 0.);
            } else if (vNext - oldV > myJerkmax * TS + veh->getAcceleration()) { // driver wants to brake less harder than before, change in acceleration is then bounded by +myJerkmax
                vNext = MAX2(oldV + (myJerkmax * TS + veh->getAcceleration()) * TS, 0.);
            } else {
                vNext = vNext; // Do nothing, as the new vNext is in the range of +/- jerk!
            }*/

            // Workaround letting the vehicle not brake hard for Lane Change reasons (vNext), but only for safety Car following / stopping reasons (vMax)!
            vNext = MAX2(oldV + MIN2(vMax - oldV, MAX2(vNext - oldV, (-myJerkmax * TS + veh->getAcceleration()) * TS)), 0.);
        }

    } else {
        // Not Done/checked yet for the ballistic update model!!!!

        // for ballistic update, negative vnext must be allowed to
        // indicate a stop within the coming timestep (i.e., to attain negative values)
        vNext = veh->getLaneChangeModel().patchSpeed(vMin, vMax, vMax, *this);
        // (Leo) finalizeSpeed() is responsible for assuring that the next
        // velocity is chosen in accordance with maximal decelerations.
        // At this point vNext may also be negative indicating a stop within next step.
        // Moreover, because maximumSafeStopSpeed() does not consider deceleration bounds
        // vNext can be a large negative value at this point. We cap vNext here.
        vNext = MAX2(vNext, vMin);
    }

    // Driving off correction term: First we check for StartupDelay, then calculate Speed with SlowToStartTerm
    // Apply Startup delay (time) before driving off
    SUMOTime addTime = vars->myap_update * DELTA_T;
    if (myStartupDelay + addTime - (veh->getTimeSinceStartup() - DELTA_T) < DELTA_T) {
        addTime = (SUMOTime)0;
    }
    double vDelay = applyStartupDelay(veh, vMin, vNext, addTime);
    // Apply the slow to start term to the acceleration/speed when driving off
    vNext = slowToStartTerm(veh, vDelay, oldV, vNext, vars);
#ifdef DEBUG_V
    if (veh->isSelected()) {
        std::cout << SIMTIME
                  << " EIDM::finalizeSpeed (2) "
                  << " veh=" << veh->getID()
                  << " timeSinceStartup=" << veh->getTimeSinceStartup()
                  << " myap_update=" << vars->myap_update
                  << " addTime=" << addTime
                  << " vDelay=" << vDelay
                  << " oldV=" << oldV
                  << " vNext=" << vNext
                  << "\n";
    }
#endif

    // Update the desired speed
    internalspeedlimit(veh, oldV);

    if (vNext > EST_REAC_THRESHOLD) { // update the Wiener-Prozess variables
        vars->myw_gap = exp(-TS / myTPersEstimate) * vars->myw_gap + sqrt(2 * TS / myTPersEstimate) * RandHelper::randNorm(0, 0.5); // variance of 1 can create very high values and may be too high!
        vars->myw_speed = exp(-TS / myTPersEstimate) * vars->myw_speed + sqrt(2 * TS / myTPersEstimate) * RandHelper::randNorm(0, 0.5); // variance of 1 can create very high values and may be too high!
        vars->myw_error = exp(-TS / myTPersDrive) * vars->myw_error + sqrt(2 * TS / myTPersDrive) * RandHelper::randNorm(0, 1);
    } // else all those w_... are zero by default

    // Update the Action-point reaction time
    if (vars->myap_update == 0) { // Update all saved acceleration variables for further calculcation between two action points
        vars->lastacc = vars->minaccel;
        vars->wouldacc = vars->minaccel;
        vars->lastrealacc = vars->realacc;
        vars->lastleaderacc = vars->realleaderacc;
    }

#ifdef DEBUG_V
    if (veh->isSelected()) {
        std::cout << SIMTIME
                  << " EIDM::finalizeSpeed (3) "
                  << " veh=" << veh->getID()
                  << " vars->myw_gap=" << vars->myw_gap
                  << " vars->myw_speed=" << vars->myw_speed
                  << " vars->myw_error=" << vars->myw_error
                  << " vars->lastacc=" << vars->lastacc
                  << " vars->lastrealacc=" << vars->lastrealacc
                  << " vars->lastleaderacc=" << vars->lastleaderacc
                  << "\n";
    }
#endif

    // Set myap_update back to 0 when maximal reaction time is reached,
    // else add 1 for the next time step
    if (double(vars->myap_update) >= double(myTreaction / TS - 1 - NUMERICAL_EPS)) {
        vars->myap_update = 0;
    } else {
        vars->myap_update = vars->myap_update + 1;
    }

    // Here the acceleration the vehicle would adapt to without a reaction time is compared to the last acceleration update at the last action point.
    // If between two action points the vehicle would like to brake harder than -myEpsilonacc, then an action point is called for the next time step (myap_update = 0).
    // This update is only used when wouldacc becomes myEpsilonacc lower than lastacc! When accelerating (wouldacc > lastacc), always the maximal reaction time is used!
    // @ToDo: Check how to use a stable reaction time below EST_REAC_THRESHOLD m/s when braking without oscillating acceleration, then this boundary could be eliminated.
    // @ToDo: Use this asynchron action point update also for accelerating (like introduced by Wagner/Hoogendorn/Treiber), not only for keeping the CF-model stable!
    if ((vars->wouldacc - vars->lastacc) < -myEpsilonacc || vars->wouldacc < -getEmergencyDecel() || (oldV < EST_REAC_THRESHOLD && vNext < oldV)) {
        // When this if-clause holds, then the driver should react immediately!
        // 1. When the change in deceleration is lower than -myEpsilonacc
        // 2. When the intended deceleration is lower than emergencyDecel
        // 3. When the vehicle is slow and decelerating
        vars->myap_update = 0;
    }

    // Set the "inner" variables of the car-following model back to the default values for the next time step
    vars->minaccel = 100;
    vars->realacc = 100;
    vars->realleaderacc = 100;

    vars->stop.clear();

    return vNext;
}


double
MSCFModel_EIDM::followSpeed(const MSVehicle* const veh, double speed, double gap2pred, double predSpeed, double /*predMaxDecel*/, const MSVehicle* const /*pred*/, const CalcReason usage) const {
//    applyHeadwayAndSpeedDifferencePerceptionErrors(veh, speed, gap2pred, predSpeed, predMaxDecel, pred);
    VehicleVariables* vars = (VehicleVariables*)veh->getCarFollowVariables();

    // This update-variable is used to check what called followSpeed (LC- or CF-model), see enum CalcReason for more information
    // Here we don't directly use gComputeLC, which is also 0 and 1, because in freeSpeed we have another call (update = 2),
    // which is needed to differentiate between the different cases/calculations/needed output/saved variables
    int update = 1;
    CalcReason _vUsage = usage;
    if (MSGlobals::gComputeLC) {
        _vUsage = CalcReason::LANE_CHANGE;
    }
    if (_vUsage == CalcReason::LANE_CHANGE || _vUsage == CalcReason::FUTURE) {
        update = 0;
    }

#ifdef DEBUG_V
    if (veh->isSelected()) {
        std::cout << SIMTIME
                  << " EIDM::followSpeed "
                  << " veh=" << veh->getID()
                  << " speed=" << speed
                  << " gap2pred=" << gap2pred
                  << " predSpeed=" << predSpeed
                  << " vars->v0_int=" << vars->v0_int
                  << " update=" << update
                  << "\n";
    }
#endif

    double result = _v(veh, gap2pred, speed, predSpeed, vars->v0_int, true, update, _vUsage);
    return result;
}


double
MSCFModel_EIDM::stopSpeed(const MSVehicle* const veh, const double speed, double gap, double /*decel*/, const CalcReason usage) const {
//    applyHeadwayPerceptionError(veh, speed, gap);
//    if (gap < 0.01) {
//        return 0;
//    }
    VehicleVariables* vars = (VehicleVariables*)veh->getCarFollowVariables();

    // This update-variable is used to check what called stopSpeed (LC- or CF-model), see enum CalcReason for more information
    // Here we don't directly use gComputeLC, which is also 0 and 1, because in freeSpeed we have another call (update = 2),
    // which is needed to differentiate between the different cases/calculations/needed output/saved variables
    int update = 1;
    CalcReason _vUsage = usage;
    if (MSGlobals::gComputeLC) {
        _vUsage = CalcReason::LANE_CHANGE;
    }
    if (_vUsage == CalcReason::LANE_CHANGE || _vUsage == CalcReason::FUTURE || usage == CalcReason::CURRENT_WAIT) {
        update = 0;
    }

#ifdef DEBUG_V
    if (veh->isSelected()) {
        std::cout << SIMTIME
                  << " EIDM::stopSpeed "
                  << " veh=" << veh->getID()
                  << " speed=" << speed
                  << " gap=" << gap
                  << " vars->v0_int=" << vars->v0_int
                  << " update=" << update
                  << "\n";
    }
#endif

    double result = _v(veh, gap, speed, 0, vars->v0_int, false, update, _vUsage);
// From Sumo_IDM-implementation:
//    if (gap > 0 && speed < NUMERICAL_EPS && result < NUMERICAL_EPS) {
//        // ensure that stops can be reached:
//        result = maximumSafeStopSpeed(gap, decel, speed, false, veh->getActionStepLengthSecs());
//    }
    return result;
}

double
MSCFModel_EIDM::freeSpeed(const double currentSpeed, const double decel, const double dist, const double targetSpeed, const bool onInsertion) {
    // XXX: (Leo) This seems to be exclusively called with decel = myDecel (max deceleration) and is not overridden
    // by any specific CFModel. That may cause undesirable hard braking (at junctions where the vehicle
    // changes to a road with a lower speed limit).

    if (MSGlobals::gSemiImplicitEulerUpdate) {
        // adapt speed to succeeding lane, no reaction time is involved
        // when breaking for y steps the following distance g is covered
        // (drive with v in the final step)
        // g = (y^2 + y) * 0.5 * b + y * v
        // y = ((((sqrt((b + 2.0*v)*(b + 2.0*v) + 8.0*b*g)) - b)*0.5 - v)/b)
        const double v = SPEED2DIST(targetSpeed);
        if (dist < v) {
            return targetSpeed;
        }
        const double b = ACCEL2DIST(decel);
        const double y = MAX2(0.0, ((sqrt((b + 2.0 * v) * (b + 2.0 * v) + 8.0 * b * dist) - b) * 0.5 - v) / b);
        const double yFull = floor(y);
        const double exactGap = (yFull * yFull + yFull) * 0.5 * b + yFull * v + (y > yFull ? v : 0.0);
        const double fullSpeedGain = (yFull + (onInsertion ? 1. : 0.)) * ACCEL2SPEED(decel);
        return DIST2SPEED(MAX2(0.0, dist - exactGap) / (yFull + 1)) + fullSpeedGain + targetSpeed;
    } else {
        // ballistic update (Leo)
        // calculate maximum next speed vN that is adjustable to vT=targetSpeed after a distance d=dist
        // and given a maximal deceleration b=decel, denote the current speed by v0.
        // the distance covered by a trajectory that attains vN in the next timestep and decelerates afterwards
        // with b is given as
        // d = 0.5*dt*(v0+vN) + (t-dt)*vN - 0.5*b*(t-dt)^2, (1)
        // where time t of arrival at d with speed vT is
        // t = dt + (vN-vT)/b.  (2)
        // We insert (2) into (1) to obtain
        // d = 0.5*dt*(v0+vN) + vN*(vN-vT)/b - 0.5*b*((vN-vT)/b)^2
        // 0 = (dt*b*v0 - vT*vT - 2*b*d) + dt*b*vN + vN*vN
        // and solve for vN

        assert(currentSpeed >= 0);
        assert(targetSpeed >= 0);

        const double dt = onInsertion ? 0 : TS; // handles case that vehicle is inserted just now (at the end of move)
        const double v0 = currentSpeed;
        const double vT = targetSpeed;
        const double b = decel;
        const double d = dist - NUMERICAL_EPS; // prevent returning a value > targetSpeed due to rounding errors

        // Solvability for positive vN (if d is small relative to v0):
        // 1) If 0.5*(v0+vT)*dt > d, we set vN=vT.
        // (In case vT<v0, this implies that on the interpolated trajectory there are points beyond d where
        //  the interpolated velocity is larger than vT, but at least on the temporal discretization grid, vT is not exceeded)
        // 2) We ignore the (possible) constraint vN >= v0 - b*dt, which could lead to a problem if v0 - t*b > vT.
        //    (finalizeSpeed() is responsible for assuring that the next velocity is chosen in accordance with maximal decelerations)

        if (0.5 * (v0 + vT)*dt >= d) {
            return vT;    // (#)
        }

        const double q = ((dt * v0 - 2 * d) * b - vT * vT); // (q < 0 is fulfilled because of (#))
        const double p = 0.5 * b * dt;
        return -p + sqrt(p * p - q);
    }
}

double
MSCFModel_EIDM::freeSpeed(const MSVehicle* const veh, double speed, double seen, double maxSpeed, const bool onInsertion, const CalcReason usage) const {

    // @ToDo: Set new internal speed limit/desired speed <maxSpeed> here and change it over time in internalspeedlimit()!

    if (maxSpeed < 0.) {
        // can occur for ballistic update (in context of driving at red light)
        return maxSpeed;
    }

    // This update-variable is used to check what called freeSpeed (LC- or CF-model), see enum CalcReason for more information
    // Here we don't directly use gComputeLC, which is also 0 and 1, because we have another call (update = 2),
    // which is needed to differentiate between the different cases/calculations/needed output/saved variables
    int update = 1;
    CalcReason _vUsage = usage;
    if (MSGlobals::gComputeLC) {
        _vUsage = CalcReason::LANE_CHANGE;
    }
    if (_vUsage == CalcReason::LANE_CHANGE || _vUsage == CalcReason::FUTURE) {
        update = 0;
    }

#ifdef DEBUG_V
    if (veh->isSelected()) {
        std::cout << SIMTIME
                  << " EIDM::freeSpeed "
                  << " veh=" << veh->getID()
                  << " speed=" << speed
                  << " seen=" << seen
                  << " maxSpeed=" << maxSpeed
                  << " update=" << update
                  << " onInsertion=" << onInsertion
                  << "\n";
    }
#endif

    VehicleVariables* vars = (VehicleVariables*)veh->getCarFollowVariables();

    double vSafe, remaining_time, targetDecel;
    double secGap;
    if (onInsertion) {
        // Needed for the Insertion-Calculation to check, if insertion at this "speed" is possible to reach "maxSpeed" in the given distance "seen" (vehicle can max decelerate with myDecel!)!
        // @ToDo: Could maybe be changed to maximumSafeFollowSpeed instead of freeSpeed-Krauss calculation!
        vSafe = freeSpeed(speed, myDecel, seen, maxSpeed, onInsertion);
    } else {
        // driver needs to brake, because he is faster than the desired speed limit <maxSpeed> on the next lane or the next upcoming event (e.g. red light violation)
        // The adaption/braking starts when the <seen> time-distance is lower than myTpreview+myTreaction
        if (maxSpeed < speed && seen < speed * (myTpreview + myTreaction)) {

            update = update == 0 ? 0 : 2;

            remaining_time = MAX3((seen - speed * myTreaction) / speed, myTreaction / 2, TS); // The remaining time is at least a time step or the reaction time of the driver
            targetDecel = (speed - maxSpeed) / remaining_time; // The needed constant deceleration to reach maxSpeed before reaching the next lane/event

            // targetDecel is not set immediatly, if the vehicle is far enough away from the event (bounded by myJerkmax)
            if (remaining_time > myTpreview - targetDecel / myJerkmax) {
                targetDecel = (myTpreview - remaining_time) * myJerkmax;
            }

            // calculate distance which would result in the accel-value targetDecel at this <speed> and leaderspeed <0>
            if (vars->myap_update == 0 || update == 0) { // at action point update
                secGap = internalsecuregap(veh, speed, 0., targetDecel);
            } else { // between two action points
                secGap = internalsecuregap(veh, vars->myv_est + vars->lastrealacc * vars->myap_update * TS, 0., targetDecel);
            }

            vSafe = _v(veh, MAX2(seen, secGap), speed, 0., vars->v0_int, true, update, _vUsage);

            // Add this for "old implementation" when vehicle doesn't HAVE to reach maxspeed at seen-distance!
            // @ToDo: See #7644: <double v = MIN2(maxV, laneMaxV);> in MSVehicle::planMoveInternal! -> DONE!
            // But still: Is it better, if the vehicle brakes early enough to reach the next lane with its speed limit?
            // Instead of driving too fast for a while on the new lane, which can be more "human", but leads to other problems (junction model, traffic light braking...)
            /*    if (seen < speed*myTpreview || seen < veh->getLane()->getVehicleMaxSpeed(veh)*myTpreview / 2) {
                    remaining_time = speed < veh->getLane()->getVehicleMaxSpeed(veh) / 2 ? seen / (veh->getLane()->getVehicleMaxSpeed(veh) / 2) : seen / MAX2(speed, 0.01);
                    if (vars->v0_int > maxSpeed + NUMERICAL_EPS && vars->v0_old > vars->v0_int + NUMERICAL_EPS) {
                        maxSpeed = MAX2(maxSpeed, MIN2(vars->v0_int, vars->v0_old - (vars->v0_old - maxSpeed) / myTpreview * (myTpreview - remaining_time)));
                        vSafe = _v(veh, 500., speed, maxSpeed, maxSpeed, true, update, _vUsage);
                    } else if (vars->v0_int > maxSpeed + NUMERICAL_EPS) {
                        maxSpeed = MAX2(maxSpeed, vars->v0_int - (vars->v0_int - maxSpeed) / myTpreview * (myTpreview - remaining_time));
                        vSafe = _v(veh, 500., speed, maxSpeed, maxSpeed, true, update, _vUsage);
                    } else {
                        vSafe = _v(veh, 500., speed, maxSpeed, vars->v0_int, true, update, _vUsage);
                    }
            */
        } else { // when the <speed> is lower than <maxSpeed> or the next lane/event is not seen with myTpreview+myTreaction yet
            vSafe = _v(veh, 500., speed, maxSpeed, vars->v0_int, true, update, _vUsage);
        }
    }

    return vSafe;
}

/// @todo update interactionGap logic to IDM
double
MSCFModel_EIDM::interactionGap(const MSVehicle* const veh, double vL) const {
    // Resolve the IDM equation to gap. Assume predecessor has
    // speed != 0 and that vsafe will be the current speed plus acceleration,
    // i.e that with this gap there will be no interaction.
    const double acc = myAccel * (1. - pow(veh->getSpeed() / veh->getLane()->getVehicleMaxSpeed(veh), myDelta));
    const double vNext = veh->getSpeed() + acc;
    const double gap = (vNext - vL) * (veh->getSpeed() + vL) / (2 * myDecel) + vL;

    // Don't allow timeHeadWay < deltaT situations.
    return MAX2(gap, SPEED2DIST(vNext));

    // Only needed for old Lane Change Model????
}

double
MSCFModel_EIDM::getSecureGap(const MSVehicle* const /*veh*/, const MSVehicle* const /*pred*/, const double speed, const double leaderSpeed, const double /*leaderMaxDecel*/) const {
    // SecureGap depends on v0 and may be higher than just looking at s* (In case of the IDM)
    //VehicleVariables* vars = (VehicleVariables*)veh->getCarFollowVariables();
    const double delta_v = speed - leaderSpeed;
    double s = MAX2(0.0, speed * myHeadwayTime + speed * delta_v / myTwoSqrtAccelDecel); // is calculated without MinGap because it is compared to a gap without MinGap!
    // For the IDM: - pow(speed / veh->getLane()->getVehicleMaxSpeed(veh), myDelta)) must be added to (myDecel / myAccel + 1)!
    // For the IIDM: Left out the case check for estSpeed > v0, assuming this is not needed here. The vehicle therefore may brake harder when newSpeed > v0 occurs!
    // The secure gap is calculated using -myDecel as secure maximal acceleration (using myDecel/myAccel)!

    double erg = sqrt((s * s) / (myDecel / myAccel + 1.0));
    return MIN2(s, erg);
}

// Only needed when vehicle has to reach laneMaxV before entering the new lane, see #7644
double
MSCFModel_EIDM::internalsecuregap(const MSVehicle* const veh, const double speed, const double leaderSpeed, const double targetDecel) const {
    // SecureGap depends on v0 and may be higher than just looking at s* (In case of the IDM)
    // internalsecuregap uses a targetDecel instead of myDecel!
    VehicleVariables* vars = (VehicleVariables*)veh->getCarFollowVariables();
    const double delta_v = speed - leaderSpeed;
    double s = MAX2(0.0, speed * myHeadwayTime + speed * delta_v / myTwoSqrtAccelDecel); // is calculated without MinGap because it is compared to a gap without MinGap!
    // For the IDM: - pow(speed / veh->getLane()->getVehicleMaxSpeed(veh), myDelta)) must be added to (myDecel / myAccel + 1)!
    // the secure gap is calculated using -myDecel as secure maximal acceleration (using myDecel/myAccel)!

    double erg;
    if (speed <= vars->v0_int) {
        erg = sqrt((s * s) / (MAX2(targetDecel / myAccel + 1.0, 1.0)));
    } else { // speed > v0
        double a_free = - myDecel * (1.0 - pow(vars->v0_int / speed, myAccel * myDelta / myDecel));
        erg = sqrt((s * s) / (MAX2(targetDecel / myAccel + 1.0 + a_free / myAccel, 1.0)));
    }

    return erg;
}

void
MSCFModel_EIDM::internalspeedlimit(MSVehicle* const veh, const double oldV) const {

    VehicleVariables* vars = (VehicleVariables*)veh->getCarFollowVariables();

    double v_limcurr, v_limprev;
    v_limcurr = vars->v0_int; // model internal desired speed limit
    v_limprev = vars->v0_old; // previous desired speed limit for calculation reasons

    const MSLane* lane = veh->getLane();
    const std::vector<MSLane*>& bestLaneConts = veh->getBestLanesContinuation();
    int view = 1;
    std::vector<MSLink*>::const_iterator link = MSLane::succLinkSec(*veh, view, *lane, bestLaneConts);
    double seen = lane->getLength() - veh->getPositionOnLane();
    double v0 = lane->getVehicleMaxSpeed(veh); // current desired lane speed

    // @ToDo: nextTurn is only defined in sublane-model calculation?!
    // @ToDo: So cannot use it yet, but the next turn or speed recommendation for the street curvature (e.g. vmax=sqrt(a_transverse*Radius), a_transverse=3-4m/s^2)
    // @ToDo: should not come from here, but from MSVehicle/the street network
    // const std::pair<double, LinkDirection> nextTurn = veh->getNextTurn();

    // When driving on the last lane/link, the vehicle shouldn't adapt to the lane after anymore.
    // Otherwise we check the <seen> time-distance and whether is lower than myTpreview
    if (lane->isLinkEnd(link) != 1 && (seen < oldV * myTpreview || seen < v0 * myTpreview / 2)) {
        double speedlim = 200;
        while (true) { // loop over all upcoming edges/lanes/links until the <seen> time-distance is higher than myTpreview
            if (lane->isLinkEnd(link) != 1 && (seen < oldV * myTpreview || seen < v0 * myTpreview / 2)) { // @ToDo: add && (*link)->havePriority()???
                // @ToDo: When vehicles already brake because of a minor link, it may not be necessary to adapt the internal desired speed when turning...
                // @ToDo: The vehicle brakes anyway and it may happen, that is brakes too hard because of the low internal desired speed and takes too long
                // @ToDo: to accelerate again, because the internal desired speed must rise again?!
                // @ToDo: It can't be done via (*link)->havePriority() (vehicle will not brake for other vehicles, so it needs to brake for curve radius),
                // @ToDo: because then turning at traffic lights or somewhere else might be missing (traffic light links don't have priority definition?!)
                LinkDirection dir = (*link)->getDirection();
                switch (dir) {
                    case LinkDirection::NODIR:
                        break;
                    case LinkDirection::STRAIGHT:
                        break;
                    case LinkDirection::TURN:
                        speedlim = 4;
                        break;
                    case LinkDirection::TURN_LEFTHAND:
                        speedlim = 4;
                        break;
                    case LinkDirection::LEFT:
                        speedlim = 8;
                        break;
                    case LinkDirection::RIGHT:
                        speedlim = 6;
                        break;
                    case LinkDirection::PARTLEFT:
                        speedlim = 12;
                        break;
                    case LinkDirection::PARTRIGHT:
                        speedlim = 12;
                        break;
                }

                if (v0 > speedlim * veh->getChosenSpeedFactor() + NUMERICAL_EPS) {
                    v0 = speedlim * veh->getChosenSpeedFactor();
                }
            } else {
                break;
            }
            if ((*link)->getViaLane() == nullptr) {
                ++view;
            }
            lane = (*link)->getViaLaneOrLane();
            // @ToDo: Previously: (seen < oldV*myTpreview / 2 || seen < v0*myTpreview / 4)! Changed freeSpeed, so also changed v0-calculation here.
            // @ToDo: Vehicle now decelerates to new Speedlimit before reaching new edge (not /2 anymore)!
            // @ToDo: v0 for changing speed limits when seen < oldV*myTpreview, not seen < oldV*myTpreview/2 anymore!!!
            if (v0 > lane->getVehicleMaxSpeed(veh)) {
                v0 = lane->getVehicleMaxSpeed(veh);
            }
            seen += lane->getLength();
            link = MSLane::succLinkSec(*veh, view, *lane, bestLaneConts);
        }

        if (!(v_limprev < v0 + NUMERICAL_EPS && v_limprev > v0 - NUMERICAL_EPS) || // if v_limprev!=v0, then the upcoming v0 is different, than the old desired v_limprev and therefore v0_int must change slowly to the new v0
                (v_limprev < v0 + NUMERICAL_EPS && v_limprev > v0 - NUMERICAL_EPS && !(v_limprev < v_limcurr + NUMERICAL_EPS && v_limprev > v_limcurr - NUMERICAL_EPS))) { // When v_limprev==v0, but v_limprev!=v_limcurr, then we may have a special case and need to slowly change v_limcurr to v0

            if ((v_limcurr < v_limprev + NUMERICAL_EPS && v_limcurr < v0 + NUMERICAL_EPS && v_limprev > v0 - NUMERICAL_EPS) || // important when v_limcurr < v0 < v_limprev --> v_limcurr was decreasing, but needs to suddenly increase again
                    (v_limcurr > v_limprev - NUMERICAL_EPS && v_limcurr > v0 - NUMERICAL_EPS && v_limprev < v0 + NUMERICAL_EPS)) { // important when v_limcurr > v0 > v_limprev --> v_limcurr was increasing, but needs to suddenly decrease again
                vars->v0_old = v_limcurr;
            } else {
                if (v_limcurr >= v0 - NUMERICAL_EPS) { // v_limcurr is too high and needs to decrease
                    v_limcurr = MAX2(v0, v_limcurr - (v_limprev - v0) * TS / myTpreview);
                } else { // v_limcurr is too low and needs to increase
                    v_limcurr = MIN2(v0, v_limcurr - (v_limprev - v0) * TS / myTpreview);
                }
            }

            // when v_limcurr reaches v0, then update v_limprev=v0
            if (v_limcurr < v0 + NUMERICAL_EPS && v_limcurr > v0 - NUMERICAL_EPS) {
                vars->v0_old = v0;
                vars->v0_int = v0;
            } else { // else just update the internal desired speed with v_limcurr
                vars->v0_int = v_limcurr;
            }
        }

    } else if (!(v_limprev < v0 + NUMERICAL_EPS && v_limprev > v0 - NUMERICAL_EPS) || // if v_limprev!=v0, then the upcoming v0 is different, than the old desired v_limprev and therefore v0_int must change slowly to the new v0
               (v_limprev < v0 + NUMERICAL_EPS && v_limprev > v0 - NUMERICAL_EPS && !(v_limprev < v_limcurr + NUMERICAL_EPS && v_limprev > v_limcurr - NUMERICAL_EPS))) { // When v_limprev==v0, but v_limprev!=v_limcurr, then we may have a special case and need to slowly change v_limcurr to v0

        if ((v_limcurr < v_limprev + NUMERICAL_EPS && v_limcurr < v0 + NUMERICAL_EPS && v_limprev > v0 - NUMERICAL_EPS) || // important when v_limcurr < v0 < v_limprev --> v_limcurr was decreasing, but needs to suddenly increase again
                (v_limcurr > v_limprev - NUMERICAL_EPS && v_limcurr > v0 - NUMERICAL_EPS && v_limprev < v0 + NUMERICAL_EPS)) { // important when v_limcurr > v0 > v_limprev --> v_limcurr was increasing, but needs to suddenly decrease again
            vars->v0_old = v_limcurr;
        } else {
            if (v_limcurr >= v0 - NUMERICAL_EPS) { // v_limcurr is too high and needs to decrease
                v_limcurr = MAX2(v0, v_limcurr - (v_limprev - v0) * TS / myTpreview);
            } else { // v_limcurr is too low and needs to increase
                v_limcurr = MIN2(v0, v_limcurr - (v_limprev - v0) * TS / myTpreview);
            }
        }

        // when v_limcurr reaches v0, then update v_limprev=v0
        if (v_limcurr < v0 + NUMERICAL_EPS && v_limcurr > v0 - NUMERICAL_EPS) {
            vars->v0_old = v0;
            vars->v0_int = v0;
        } else { // else just update the internal desired speed with v_limcurr
            vars->v0_int = v_limcurr;
        }
    }
}

double
MSCFModel_EIDM::_v(const MSVehicle* const veh, const double gap2pred, const double egoSpeed,
                   const double predSpeed, const double desSpeed, const bool respectMinGap, const int update, const CalcReason usage) const {

    double v0 = MAX2(NUMERICAL_EPS, desSpeed);
    VehicleVariables* vars = (VehicleVariables*)veh->getCarFollowVariables();

    // @ToDo: Where to put such an insertion function/update, which only needs to be calculated once at the first step?????!
    // For the first iteration
    if (vars->v0_old == 0) {
        vars = (VehicleVariables*)veh->getCarFollowVariables();
        vars->v0_old = MAX2(NUMERICAL_EPS, veh->getLane()->getVehicleMaxSpeed(veh));
        vars->v0_int = MAX2(NUMERICAL_EPS, veh->getLane()->getVehicleMaxSpeed(veh));
        v0 = MAX2(NUMERICAL_EPS, veh->getLane()->getVehicleMaxSpeed(veh));
    }

    double wantedacc = 0., a_free;
    double wouldacc = 0., woulds, woulda_free;

    double estSpeed, estleaderSpeed, estGap;
    double current_estSpeed, current_estGap, current_estleaderSpeed;
    double current_gap;
    double acc = 0.;
    double a_leader = NUMERICAL_EPS; // Default without a leader, should not be 0!
    double newSpeed = egoSpeed;
    bool lastrespectMinGap = respectMinGap;
    const double minGapStop_EPS = 0.05 + 0.20 * MAX2(0.25, myAccel);

    // When doing the Follow-Calculation in adapttoLeader (MSVehicle.cpp) the mingap gets subtracted from the current gap (maybe this is needed for the Krauss-Model!).
    // For the IDM this Mingap is needed or else the vehicle will stop at two times mingap behind the leader!
    if (respectMinGap) {
        current_gap = MAX2(NUMERICAL_EPS, gap2pred + MAX2(NUMERICAL_EPS, myType->getMinGap() - 0.25)); // 0.25m tolerance because of reaction time and estimated variables
    } else {
        // gap2pred may go to 0 when offset is reached (e.g. 1m Offset -> gap2pred=0, when vehicle stands at 0.95m, gap2pred is still 0 and does not become -0.05m (negative)!)
        current_gap = MAX2(NUMERICAL_EPS, gap2pred + minGapStop_EPS);
    }

    double newGap = current_gap;

    for (int i = 0; i < myIterations; i++) {

        // Using Action-Point reaction time: update the variables, when myap_update is zero and update is 1
        current_estSpeed = newSpeed;
        if (respectMinGap) {
            current_estleaderSpeed = MAX2(predSpeed - newGap * mySigmaleader * vars->myw_speed, 0.0); // estimated variable with Wiener Prozess
        } else {
            // @ToDo: Use this??? driver would always know, that junctions, traffic lights, etc. have v=0!
            // @ToDo: With estimated variables predSpeed > 0 is possible! > 0 may result in oscillating
            current_estleaderSpeed = predSpeed;
        }
        if (update == 2) { // For freeSpeed
            current_estGap = newGap; // not-estimated variable
        } else {
            if (respectMinGap) {
                current_estGap = newGap * exp(mySigmagap * vars->myw_gap); // estimated variable with Wiener Prozess
            } else {
                current_estGap = newGap * exp(mySigmagap * vars->myw_gap * MIN2(current_estSpeed / EST_REAC_THRESHOLD, 1.0)); // estimated variable with Wiener Prozess
            }
        }

        if (vars->myap_update == 0 && usage == CalcReason::CURRENT) { // update variables with current observation
            estSpeed = current_estSpeed;
            estleaderSpeed = current_estleaderSpeed; // estimated variable with Wiener Prozess
            estGap = current_estGap; // estimated variable with Wiener Prozess
        } else if (usage == CalcReason::CURRENT) { // use stored variables (reaction time)
            estSpeed = MAX2(vars->myv_est + vars->lastrealacc * (vars->myap_update * TS - TS * (myIterations - i - 1.) / myIterations), 0.0);
            //        estSpeed = vars->myv_est;
            if (update == 2) { // For freeSpeed
                estGap = newGap; // not-estimated variable
                estleaderSpeed = MAX2(vars->myv_est_l + vars->lastleaderacc * (vars->myap_update * TS - TS * (myIterations - i - 1.) / myIterations) - vars->mys_est * mySigmaleader * vars->myw_speed, 0.0);
                //        estleaderSpeed = MAX2(vars->myv_est_l - vars->mys_est * mySigmaleader*vars->myw_speed, 0.0);
            } else {
                lastrespectMinGap = vars->myrespectMinGap;
                if (lastrespectMinGap) {
                    estleaderSpeed = MAX2(vars->myv_est_l + vars->lastleaderacc * (vars->myap_update * TS - TS * (myIterations - i - 1.) / myIterations) - vars->mys_est * mySigmaleader * vars->myw_speed, 0.0);
                    //        estleaderSpeed = MAX2(vars->myv_est_l - vars->mys_est * mySigmaleader*vars->myw_speed, 0.0);
                    estGap = vars->mys_est * exp(mySigmagap * vars->myw_gap) - ((vars->myv_est + estSpeed) / 2. - (vars->myv_est_l + estleaderSpeed) / 2.) * (vars->myap_update * TS - TS * (myIterations - i - 1.) / myIterations); // estimated variable with Wiener Prozess
                } else {
                    // @ToDo: Use this??? driver would always know, that junctions, traffic lights, etc. have v=0!
                    // @ToDo: With estimated variables predSpeed > 0 is possible! > 0 may result in oscillating
                    estleaderSpeed = vars->myv_est_l;
                    estGap = vars->mys_est * exp(mySigmagap * vars->myw_gap * MIN2(current_estSpeed / EST_REAC_THRESHOLD, 1.0)) - ((vars->myv_est + estSpeed) / 2. - (vars->myv_est_l + estleaderSpeed) / 2.) * (vars->myap_update * TS - TS * (myIterations - i - 1.) / myIterations); // estimated variable with Wiener Prozess
                }
            }
        } else { // use actual variables without reaction time
            estSpeed = current_estSpeed;
            estleaderSpeed = current_estleaderSpeed; // estimated variable with Wiener Prozess
            estGap = current_estGap; // estimated variable with Wiener Prozess
        }

        // ToDo: The headway can change for IDMM based on the scenario, should something like that also be integrated here???
        double headwayTime = myHeadwayTime;
        MSVehicle* leader;

        double s = MAX2(0., estSpeed * headwayTime + estSpeed * (estSpeed - estleaderSpeed) / myTwoSqrtAccelDecel);
        if (lastrespectMinGap) {
            s += myType->getMinGap() + EIDM_POS_ACC_EPS;
        } else {
            s += minGapStop_EPS + EIDM_POS_ACC_EPS;
        }

        // Because of the reaction time and estimated variables, s* can become lower than gap when the vehicle needs to brake/is braking, that results in the vehicle accelerating again...
        // Here: When the gap is very small, s* is influenced to then always be bigger than the gap. With this there are no oscillations in accel at small gaps!
        if (lastrespectMinGap) {
            // The allowed position error when coming to a stop behind a leader is higher with higher timesteps (approx. 0.5m at 1.0s timstep, 0.1m at 0.1s)
            if (estGap < myType->getMinGap() + (TS * 10 + 1) * EIDM_POS_ACC_EPS && estSpeed < EST_REAC_THRESHOLD && s < estGap * sqrt(1 + 2 * EIDM_POS_ACC_EPS / myAccel)) {
                s = estGap * sqrt(1 + 2 * EIDM_POS_ACC_EPS / myAccel);
            }
        } else {
            if (estGap < minGapStop_EPS + 2 * EIDM_POS_ACC_EPS && s < estGap * sqrt(1 + EIDM_POS_ACC_EPS / myAccel)) {
                // when the vehicle wants to stop (stopSpeed), it may take long to come to a full stop
                // To lower this stop time, we restrict the deceleration to always be higher than 0.05m/s^2 when stopping
                s = estGap * sqrt(1 + EIDM_POS_ACC_EPS / myAccel);
            }
        }

        // IDM calculation:
        // wantedacc = myAccel * (1. - pow(estSpeed / v0, myDelta) - (s * s) / (estGap * estGap));

        // IIDM calculation -NOT- from the original Treiber/Kesting publication:
        // With the saved variables from the last Action Point
        /*double wantedacc;
        double a_free = myAccel * (1. - pow(estSpeed / v0, myDelta));
        if (s >= estGap) { // This is the IIDM
            wantedacc = myAccel * (1. - (s * s) / (estGap * estGap));
        } else {
            wantedacc = a_free * (1. - pow(s / estGap, 2*myAccel / fabs(a_free)));
        }*/ // Old calculation form without the distinction between v > v0 and v <= v0!!! Published it in the EIDM with this form, but may be worse than original IIDM!

        // IIDM calculation from the original Treiber/Kesting publication:
        // With the saved variables from the last Action Point
        if (estSpeed <= v0) {
            a_free = myAccel * (1. - pow(estSpeed / v0, myDelta));
            if (s >= estGap) {
                wantedacc = myAccel * (1. - (s * s) / (estGap * estGap));
            } else {
                wantedacc = a_free * (1. - pow(s / estGap, 2 * myAccel / a_free));
            }
        } else { // estSpeed > v0
            a_free = - myDecel * (1. - pow(v0 / estSpeed, myAccel * myDelta / myDecel));
            if (s >= estGap) {
                wantedacc = a_free + myAccel * (1. - (s * s) / (estGap * estGap));
            } else {
                wantedacc = a_free;
            }
        }
        wantedacc = SPEED2ACCEL(MAX2(0.0, estSpeed + ACCEL2SPEED(wantedacc)) - estSpeed);

        // IIDM calculation from the original Treiber/Kesting publication:
        // With the current variables (what would the acceleration be without reaction time)
        if (usage == CalcReason::CURRENT) {
            woulds = MAX2(0., current_estSpeed * headwayTime + current_estSpeed * (current_estSpeed - current_estleaderSpeed) / myTwoSqrtAccelDecel); // s_soll
            if (respectMinGap) {
                woulds += myType->getMinGap() + EIDM_POS_ACC_EPS; // when behind a vehicle use MinGap and when at a junction then not????
            } else {
                woulds += minGapStop_EPS + EIDM_POS_ACC_EPS;
            }

            if (current_estSpeed <= v0) {
                woulda_free = myAccel * (1. - pow(current_estSpeed / v0, myDelta));
                if (woulds >= current_estGap) {
                    wouldacc = myAccel * (1. - (woulds * woulds) / (current_estGap * current_estGap));
                } else {
                    wouldacc = woulda_free * (1. - pow(woulds / current_estGap, 2 * myAccel / woulda_free));
                }
            } else { // current_estSpeed > v0
                woulda_free =  - myDecel * (1. - pow(v0 / current_estSpeed, myAccel * myDelta / myDecel));
                if (woulds >= current_estGap) {
                    wouldacc = woulda_free + myAccel * (1. - (woulds * woulds) / (current_estGap * current_estGap));
                } else {
                    wouldacc = woulda_free;
                }
            }
            wouldacc = SPEED2ACCEL(MAX2(0.0, current_estSpeed + ACCEL2SPEED(wouldacc)) - current_estSpeed);
        }

        // @ToDo: calc_gap is just estGap here, used to have an extra calc_gap calculation (like jmax), but doesn't work well here with the junction calculation:
        // @ToDo: The driver would slowly start accelerating when he thinks the junction is clear, but may still decelerate for a bit and not jump to acceleration.
        // @ToDo: This causes the driver not to drive over the junction because he thinks he won't make it in time before a foe may appear!

        // IIDM calculation -NOT- from the original Treiber/Kesting publication:
        // Resulting acceleration also with the correct drive off term.
        double calc_gap = estGap;
        /*a_free = myAccel * (1. - pow(estSpeed / v0, myDelta));
        if (s >= calc_gap) { // This is the IIDM
            acc = myAccel * (1. - (s * s) / (calc_gap * calc_gap));
        } else {
            acc = a_free * (1. - pow(s / calc_gap, 2*myAccel / fabs(a_free)));
        } */ // Old calculation form without the distinction between v > v0 and v <= v0!!! Published it in the EIDM with this form, but may be worse than original IIDM!

        // IDM calculation:
        // acc = myAccel * (1. - pow(estSpeed / v0, myDelta) - (s * s) / (calc_gap * calc_gap));

        // IIDM calculation from the original Treiber/Kesting publication:
        // Resulting acceleration also with the correct drive off term.
        if (estSpeed <= v0) {
            a_free = myAccel * (1. - pow(estSpeed / v0, myDelta));
            if (s >= calc_gap) {
                acc = myAccel * (1. - (s * s) / (calc_gap * calc_gap));
            } else {
                acc = a_free * (1. - pow(s / calc_gap, 2 * myAccel / a_free));
            }
        } else { // estSpeed > v0
            a_free = - myDecel * (1. - pow(v0 / estSpeed, myAccel * myDelta / myDecel));
            if (s >= calc_gap) {
                acc = a_free + myAccel * (1. - (s * s) / (calc_gap * calc_gap));
            } else {
                acc = a_free;
            }
        }

        double a_cah;
        // Coolness from Enhanced Intelligent Driver Model, when gap "jump" to a smaller gap accurs
        // @ToDo: Maybe without usage == CalcReason::CURRENT??? To let all calculations profit from Coolness??? (e.g. also lane change calculation)
        if (vars->minaccel > wantedacc - NUMERICAL_EPS && usage == CalcReason::CURRENT) {

            leader = (MSVehicle*)veh->getLeader(estGap + 25).first;
            if (leader != nullptr && lastrespectMinGap && estleaderSpeed >= SPEED_EPS) {
                a_leader = MIN2(leader->getAcceleration(), myAccel);
                // Change a_leader to lower values when far away from leader or else far away leaders influence the ego-vehicle!
                if (estGap > s * 3 / 2) { // maybe estGap > 2*s???
                    a_leader = a_leader * (s * 3 / 2) / estGap;
                }
            }

            // speed of the leader shouldnt become zero, because then problems with the calculation occur
            if (estleaderSpeed < SPEED_EPS) {
                estleaderSpeed = SPEED_EPS;
            }

            if (vars->t_off + myTaccmax + NUMERICAL_EPS < (SIMTIME - TS * (myIterations - i - 1.) / myIterations) && egoSpeed > SUMO_const_haltingSpeed) {

                // Enhanced Intelligent Driver Model
                if (estleaderSpeed * (estSpeed - estleaderSpeed) <= -2 * estGap * a_leader) {
                    a_cah = (estSpeed * estSpeed * a_leader) / (estleaderSpeed * estleaderSpeed - 2 * estGap * a_leader);
                } else {
                    if (estSpeed - estleaderSpeed >= 0) {
                        a_cah = a_leader - ((estSpeed - estleaderSpeed) * (estSpeed - estleaderSpeed)) / (2 * estGap);
                    } else {
                        a_cah = a_leader;
                    }
                }

                if (acc >= a_cah) {
                    // do nothing, meaning acc = acc_IDM;
                } else {
                    acc = (1 - myCcoolness) * acc + myCcoolness * (a_cah + myDecel * tanh((acc - a_cah) / myDecel));
                }
            }
        }

        newSpeed = MAX2(0.0, current_estSpeed + ACCEL2SPEED(acc) / myIterations);

        // Euler Update as future gap prediction, this will be the "real" gap with this timestep and speed calculation
        // Although this is the correct gap prediction, the calculation is unstable with this method
        //newGap = MAX2(NUMERICAL_EPS, current_gap - SPEED2DIST(newSpeed - predSpeed) * ((i + 1.) / myIterations));

        // Ballistic Update as future gap prediction, this will be the "real" gap with this timestep and speed calculation
        // Although this is the correct gap prediction, the calculation is unstable with this method
        //newGap = MAX2(NUMERICAL_EPS, current_gap - SPEED2DIST(MAX2(0.0, current_estSpeed + 0.5 * ACCEL2SPEED(acc) ((i + 1.) / myIterations)) - predSpeed) * ((i + 1.) / myIterations));

        // We cannot rely on sub-timesteps, because the here calculated "sub"-gap will not match the "full"-gap calculation of the Euler/Ballistic Update.
        // The "full"-gap is only calculated with the last measured newSpeed, while the "sub"-gap uses all "sub"-newSpeeds
        // Example: In the last iteration-step newSpeed becomes 0. Therefore in the Euler Update, the vehicle does not move for the whole timestep!
        // Example: But in the "sub"-gaps the vehicle may have moved. Therefore, stops can sometimes not be reached
        newGap = MAX2(NUMERICAL_EPS, newGap - MAX2(0., SPEED2DIST(newSpeed - predSpeed) / myIterations));
        // Ballistic:
        //newGap = MAX2(NUMERICAL_EPS, newGap - MAX2(0., SPEED2DIST(MAX2(0.0, current_estSpeed + 0.5 * ACCEL2SPEED(acc) / myIterations) - predSpeed) / myIterations));

        // To always reach stops in high-timestep simulations, we adapt the speed to the actual distance that is covered:
        // This may only be needed for Euler Update...
        if (myIterations > 1 && newSpeed < EST_REAC_THRESHOLD * TS && !lastrespectMinGap) {
            newSpeed = MAX2(0.0, predSpeed + DIST2SPEED(current_gap - newGap) * myIterations / (i + 1.));
        }
    }

    // The "real" acceleration after iterations
    acc = SPEED2ACCEL(MIN2(newSpeed, maxNextSpeed(egoSpeed, veh)) - veh->getSpeed());

#ifdef DEBUG_V
    if (veh->isSelected()) {
        std::cout << SIMTIME
                  << " EIDM::_v "
                  << " veh=" << veh->getID()
                  << " vars->minaccel=" << vars->minaccel
                  << " vars->myap_update=" << vars->myap_update
                  << " vars->myv_est_l=" << vars->myv_est_l
                  << " vars->myv_est=" << vars->myv_est
                  << " vars->mys_est=" << vars->mys_est
                  << " vars->wouldacc=" << vars->wouldacc
                  << " vars->realacc=" << vars->realacc
                  << "\n";
        std::cout << SIMTIME
                  << " EIDM::_v (2) "
                  << " veh=" << veh->getID()
                  << " newSpeed=" << newSpeed
                  << " newGap=" << newGap
                  << " predSpeed=" << predSpeed
                  << " estSpeed=" << estSpeed
                  << " estleaderSpeed=" << estleaderSpeed
                  << " estGap=" << estGap
                  << " wantedacc=" << wantedacc
                  << " wouldacc=" << wouldacc
                  << " acc=" << acc
                  << "\n";
    }
#endif

    // wantedacc is already calculated at this point. acc may still change (because of coolness and drive off), but the ratio should stay the same!
    // this means when vars->minaccel > wantedacc stands, so should vars->minaccel > acc!
    // When updating at an Action Point, store the observed variables for the next time steps until the next Action Point.
    if (vars->minaccel > wantedacc - NUMERICAL_EPS && vars->myap_update == 0 && usage == CalcReason::CURRENT) {
        vars->myv_est_l = predSpeed;
        vars->myv_est = egoSpeed;
        if (update == 2) { // For freeSpeed
            vars->mys_est = current_gap + myTreaction * egoSpeed;
        } else {
            vars->mys_est = current_gap;
        }
        vars->myrespectMinGap = respectMinGap;
    }

    if (usage == CalcReason::CURRENT && vars->wouldacc > wouldacc) {
        vars->wouldacc = wouldacc;
    }

    // It can happen that wantedacc ist higher than previous calculated wantedacc, BUT acc is lower than prev calculated values!
    // This often occurs because of "coolness"+Iteration and in this case "acc" is set to the previous (higher) calculated value!
    if (vars->realacc > acc && vars->minaccel <= wantedacc - NUMERICAL_EPS && usage == CalcReason::CURRENT) {
        acc = vars->realacc;
        newSpeed = MAX2(0.0, egoSpeed + ACCEL2SPEED(acc));
    }

    // Capture the relevant variables, because it was determined, that this call will result in the acceleration update (vars->minaccel > wantedacc)
    if (vars->minaccel > wantedacc - NUMERICAL_EPS && usage == CalcReason::CURRENT) {
        vars->minaccel = wantedacc;
        if (vars->realacc > acc) {
            vars->realacc = acc;
        }
        vars->realleaderacc = a_leader;
    }

    // Save the parameters for a potential update in finalizeSpeed, when _v was called in a stopSpeed-function!!!
    if (vars->minaccel > wantedacc - NUMERICAL_EPS && usage == CalcReason::CURRENT_WAIT && !respectMinGap) {
        vars->stop.push_back(std::make_pair(acc, gap2pred));
    }

    return MAX2(0., newSpeed);
}


MSCFModel*
MSCFModel_EIDM::duplicate(const MSVehicleType* vtype) const {
    return new MSCFModel_EIDM(vtype);
}
