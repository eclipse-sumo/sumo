/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    MSCFModel_CACC.cpp
/// @author  Kallirroi Porfyri
/// @author  Karagounis Vasilios
/// @date    Nov 2018
///
// CACC car-following model based on [1], [2].
// [1] Milanes, V., and S. E. Shladover. Handling Cut-In Vehicles in Strings
//    of Cooperative Adaptive Cruise Control Vehicles. Journal of Intelligent
//     Transportation Systems, Vol. 20, No. 2, 2015, pp. 178-191.
// [2] Xiao, L., M. Wang and B. van Arem. Realistic Car-Following Models for
//    Microscopic Simulation of Adaptive and Cooperative Adaptive Cruise
//     Control Vehicles. Transportation Research Record: Journal of the
//     Transportation Research Board, No. 2623, 2017. (DOI: 10.3141/2623-01).
/****************************************************************************/
#include <config.h>

#include <stdio.h>
#include <iostream>

#include "MSCFModel_CACC.h"
#include <microsim/MSVehicle.h>
#include <microsim/MSLane.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/StringUtils.h>
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include <math.h>
#include <microsim/MSNet.h>

// ===========================================================================
// debug flags
// ===========================================================================
#define DEBUG_CACC 0
#define DEBUG_CACC_INSERTION_FOLLOW_SPEED 0
#define DEBUG_CACC_SECURE_GAP 0
#define DEBUG_COND (veh->isSelected())
//#define DEBUG_COND (veh->getID() == "flow.0")
//#define DEBUG_COND (veh->getID() == "CVflowToC2.11")


// ===========================================================================
// defaults
// ===========================================================================
#define DEFAULT_SC_GAIN_CACC -0.4
#define DEFAULT_GCC_GAIN_GAP_CACC 0.005
#define DEFAULT_GCC_GAIN_GAP_DOT_CACC 0.05
#define DEFAULT_GC_GAIN_GAP_CACC 0.45
#define DEFAULT_GC_GAIN_GAP_DOT_CACC 0.0125
#define DEFAULT_CA_GAIN_GAP_CACC 0.45
#define DEFAULT_CA_GAIN_GAP_DOT_CACC 0.05
#define DEFAULT_HEADWAYTIME_ACC 1.0
#define DEFAULT_SC_MIN_GAP 1.66

// override followSpeed when deemed unsafe by the given margin (the value was selected to reduce the number of necessary interventions)
#define DEFAULT_EMERGENCY_OVERRIDE_THRESHOLD 2.0

std::map<std::string, MSCFModel_CACC::CommunicationsOverrideMode> MSCFModel_CACC::CommunicationsOverrideModeMap = {
    {"0", CACC_NO_OVERRIDE},
    {"1", CACC_MODE_NO_LEADER},
    {"2", CACC_MODE_LEADER_NO_CAV},
    {"3", CACC_MODE_LEADER_CAV}
};

std::map<MSCFModel_CACC::VehicleMode, std::string> MSCFModel_CACC::VehicleModeNames = {
    {CC_MODE, "CC"},
    {ACC_MODE, "ACC"},
    {CACC_GAP_CLOSING_MODE, "CACC_GAP_CL"},
    {CACC_GAP_MODE, "CACC_GAP"},
    {CACC_COLLISION_AVOIDANCE_MODE, "CACC_CA"}
};

// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel_CACC::MSCFModel_CACC(const MSVehicleType* vtype) :
    MSCFModel(vtype), acc_CFM(MSCFModel_ACC(vtype)),
    mySpeedControlGain(vtype->getParameter().getCFParam(SUMO_ATTR_SC_GAIN_CACC, DEFAULT_SC_GAIN_CACC)),
    myGapClosingControlGainGap(vtype->getParameter().getCFParam(SUMO_ATTR_GCC_GAIN_GAP_CACC, DEFAULT_GCC_GAIN_GAP_CACC)),
    myGapClosingControlGainGapDot(vtype->getParameter().getCFParam(SUMO_ATTR_GCC_GAIN_GAP_DOT_CACC, DEFAULT_GCC_GAIN_GAP_DOT_CACC)),
    myGapControlGainGap(vtype->getParameter().getCFParam(SUMO_ATTR_GC_GAIN_GAP_CACC, DEFAULT_GC_GAIN_GAP_CACC)),
    myGapControlGainGapDot(vtype->getParameter().getCFParam(SUMO_ATTR_GC_GAIN_GAP_DOT_CACC, DEFAULT_GC_GAIN_GAP_DOT_CACC)),
    myCollisionAvoidanceGainGap(vtype->getParameter().getCFParam(SUMO_ATTR_CA_GAIN_GAP_CACC, DEFAULT_CA_GAIN_GAP_CACC)),
    myCollisionAvoidanceGainGapDot(vtype->getParameter().getCFParam(SUMO_ATTR_CA_GAIN_GAP_DOT_CACC, DEFAULT_CA_GAIN_GAP_DOT_CACC)),
    myHeadwayTimeACC(vtype->getParameter().getCFParam(SUMO_ATTR_HEADWAY_TIME_CACC_TO_ACC, DEFAULT_HEADWAYTIME_ACC)),
    myApplyDriverstate(vtype->getParameter().getCFParam(SUMO_ATTR_APPLYDRIVERSTATE, 0)),
    myEmergencyThreshold(vtype->getParameter().getCFParam(SUMO_ATTR_CA_OVERRIDE, DEFAULT_EMERGENCY_OVERRIDE_THRESHOLD)),
    mySpeedControlMinGap(vtype->getParameter().getCFParam(SUMO_ATTR_SC_MIN_GAP, DEFAULT_SC_MIN_GAP)) {
    myCollisionMinGapFactor = vtype->getParameter().getCFParam(SUMO_ATTR_COLLISION_MINGAP_FACTOR, 0.1);
    acc_CFM.setHeadwayTime(myHeadwayTimeACC);
}

MSCFModel_CACC::~MSCFModel_CACC() {}

double
MSCFModel_CACC::freeSpeed(const MSVehicle* const veh, double speed, double seen, double maxSpeed, const bool onInsertion, const CalcReason usage) const {
    // set "caccVehicleMode" parameter to default value
    if (!MSGlobals::gComputeLC && usage == CalcReason::CURRENT) {
        const_cast<SUMOVehicleParameter&>(veh->getParameter()).setParameter("caccVehicleMode", VehicleModeNames[CC_MODE]);
    }
    return MSCFModel::freeSpeed(veh, speed, seen, maxSpeed, onInsertion, usage);
}

double
MSCFModel_CACC::followSpeed(const MSVehicle* const veh, double speed, double gap2pred, double predSpeed, double predMaxDecel, const MSVehicle* const pred, const CalcReason usage) const {
    if (myApplyDriverstate) {
        applyHeadwayAndSpeedDifferencePerceptionErrors(veh, speed, gap2pred, predSpeed, predMaxDecel, pred);
    }

    const double desSpeed = veh->getLane()->getVehicleMaxSpeed(veh);
    const double vCACC = _v(veh, pred, gap2pred, speed, predSpeed, desSpeed, true, usage);
    // using onInsertion=true disables emergencyOverides emergency deceleration smoothing
    const double vSafe = maximumSafeFollowSpeed(gap2pred, speed, predSpeed, predMaxDecel, true);

#if DEBUG_CACC == 1
    if (DEBUG_COND) {
        std::cout << SIMTIME << " veh=" << veh->getID() << " pred=" << Named::getIDSecure(pred)
                  << " v=" << speed << " vL=" << predSpeed << " gap=" << gap2pred
                  << " predDecel=" << predMaxDecel << " vCACC=" << vCACC << " vSafe=" << vSafe << "\n";
    }
#else
    UNUSED_PARAMETER(pred);
#endif
    const double speedOverride = MIN2(myEmergencyThreshold, gap2pred);
    if (vSafe + speedOverride < vCACC) {
#if DEBUG_CACC == 1
        if (DEBUG_COND) {
            std::cout << "Apply Safe speed, override=" << speedOverride << "\n";
        }
#endif
        return MAX2(0.0, vSafe + speedOverride);
    }
    return vCACC;
}

double
MSCFModel_CACC::stopSpeed(const MSVehicle* const veh, const double speed, double gap, double decel, const CalcReason /*usage*/) const {
    if (myApplyDriverstate) {
        applyHeadwayPerceptionError(veh, speed, gap);
    }

    // NOTE: This allows return of smaller values than minNextSpeed().
    // Only relevant for the ballistic update: We give the argument headway=TS, to assure that
    // the stopping position is approached with a uniform deceleration also for tau!=TS.
    return MIN2(maximumSafeStopSpeed(gap, decel, speed, false, veh->getActionStepLengthSecs()), maxNextSpeed(speed, veh));
}

double
MSCFModel_CACC::getSecureGap(const MSVehicle* const veh, const MSVehicle* const pred, const double speed, const double leaderSpeed, const double leaderMaxDecel) const {
    // Accel in gap mode should vanish:
    double desSpacing;
    if (pred->getCarFollowModel().getModelID() != SUMO_TAG_CF_CACC) {
        //      0 = myGapControlGainSpeed * (leaderSpeed - speed) + myGapControlGainSpace * (g - myHeadwayTime * speed);
        // <=>  myGapControlGainSpace * g = - myGapControlGainSpeed * (leaderSpeed - speed) + myGapControlGainSpace * myHeadwayTime * speed;
        // <=>  g = - myGapControlGainSpeed * (leaderSpeed - speed) / myGapControlGainSpace + myHeadwayTime * speed;
        desSpacing = acc_CFM.myGapControlGainSpeed * (speed - leaderSpeed) / acc_CFM.myGapControlGainSpace + myHeadwayTimeACC * speed; // MSCFModel_ACC::accelGapControl
    } else {
        desSpacing = myHeadwayTime * speed; // speedGapControl
    }
    const double desSpacingDefault = MSCFModel::getSecureGap(veh, pred, speed, leaderSpeed, leaderMaxDecel);
#if DEBUG_CACC_SECURE_GAP == 1
    std::cout << SIMTIME << "MSCFModel_ACC::getSecureGap speed=" << speed << " leaderSpeed=" << leaderSpeed
              << " desSpacing=" << desSpacing << " desSpacingDefault=" << desSpacingDefault << "\n";
#endif
    return MAX2(desSpacing, desSpacingDefault);
}


double
MSCFModel_CACC::insertionFollowSpeed(const MSVehicle* const veh, double speed, double gap2pred, double predSpeed, double predMaxDecel, const MSVehicle* const pred) const {
#if DEBUG_CACC_INSERTION_FOLLOW_SPEED == 1
    if (DEBUG_COND) {
        std::cout << "MSCFModel_ACC::insertionFollowSpeed(), speed=" << speed << " gap2pred=" << gap2pred << " predSpeed=" << predSpeed << "\n";
    }
#endif
    // iterate to find a stationary value for
    //    speed = followSpeed(v, speed, gap2pred, predSpeed, predMaxDecel, nullptr, CalcReason::FUTURE)
    const int max_iter = 50;
    int n_iter = 0;
    const double tol = 0.1;
    double damping = 0.8;

    double res = speed;
    while (n_iter < max_iter) {
        // proposed acceleration
        const double vCACC = _v(veh, pred, gap2pred, res, predSpeed, speed, true);
        const double vSafe = maximumSafeFollowSpeed(gap2pred, res, predSpeed, predMaxDecel, true);
        const double a = MIN2(vCACC, vSafe) - res;
        res = res + damping * a;
#if DEBUG_CACC_INSERTION_FOLLOW_SPEED == 1
        if (DEBUG_COND) {
            std::cout << "   n_iter=" << n_iter << " vSafe=" << vSafe << " vCACC=" << vCACC << " a=" << a << " damping=" << damping << " res=" << res << std::endl;
        }
#endif
        damping *= 0.9;
        if (fabs(a) < tol) {
            break;
        } else {
            n_iter++;
        }
    }
    return res;
}




/// @todo update interactionGap logic
double
MSCFModel_CACC::interactionGap(const MSVehicle* const /* veh */, double /* vL */) const {
    /*maximum radar range is CACC is enabled*/
    return 250;
}


std::string
MSCFModel_CACC::getParameter(const MSVehicle* veh, const std::string& key) const {
    CACCVehicleVariables* vars = (CACCVehicleVariables*) veh->getCarFollowVariables();

    if (key.compare("caccCommunicationsOverrideMode") == 0) {
        return toString(vars->CACC_CommunicationsOverrideMode);
    }

    return "";
}


void
MSCFModel_CACC::setParameter(MSVehicle* veh, const std::string& key, const std::string& value) const {
    CACCVehicleVariables* vars = (CACCVehicleVariables*) veh->getCarFollowVariables();

    try {
        if (key.compare("caccCommunicationsOverrideMode") == 0) {
            vars->CACC_CommunicationsOverrideMode = CommunicationsOverrideModeMap[value];
        }
    } catch (NumberFormatException&) {
        throw InvalidArgument("Invalid value '" + value + "' for parameter '" + key + "' for vehicle '" + veh->getID() + "'");
    }
}


double
MSCFModel_CACC::speedSpeedControl(const double speed, double vErr, VehicleMode& vehMode) const {
    // Speed control law
    vehMode = CC_MODE;
    double sclAccel = mySpeedControlGain * vErr;
    double newSpeed = speed + ACCEL2SPEED(sclAccel);
    return newSpeed;
}

double
MSCFModel_CACC::speedGapControl(const MSVehicle* const veh, const double gap2pred,
                                const double speed, const double predSpeed, const double desSpeed, double vErr,
                                const MSVehicle* const pred, VehicleMode& vehMode) const {
    // Gap control law
    double newSpeed = 0.0;

    if (pred != nullptr) {
        if (pred->getCarFollowModel().getModelID() != SUMO_TAG_CF_CACC) {
            vehMode = ACC_MODE;
            newSpeed = acc_CFM._v(veh, gap2pred, speed, predSpeed, desSpeed, true);
#if DEBUG_CACC == 1
            if (DEBUG_COND) {
                std::cout << "        acc control mode" << std::endl;
            }
#endif
        } else {
#if DEBUG_CACC == 1
            if (DEBUG_COND) {
                std::cout << "        CACC control mode" << std::endl;
            }
#endif
            double desSpacing = myHeadwayTime * speed;
            double spacingErr = gap2pred - desSpacing;
            double accel = veh->getAcceleration();
            double speedErr = predSpeed - speed + myHeadwayTime * accel;

            if ((spacingErr > 0 && spacingErr < 0.2) && (vErr < 0.1)) {
                // gap mode
                //newSpeed = speed + 0.45 * spacingErr + 0.0125 *speedErr;
#if DEBUG_CACC == 1
                if (DEBUG_COND) {
                    std::cout << "        applying gap control:  err=" << spacingErr << " speedErr=" << speedErr << std::endl;
                }
#endif
                newSpeed = speed + myGapControlGainGap * spacingErr + myGapControlGainGapDot * speedErr;

                vehMode = CACC_GAP_MODE;
            } else if (spacingErr < 0) {
                // collision avoidance mode
                //newSpeed = speed + 0.45 * spacingErr + 0.05 *speedErr;
#if DEBUG_CACC == 1
                if (DEBUG_COND) {
                    std::cout << "        applying collision avoidance:  err=" << spacingErr << " speedErr=" << speedErr << "\n";
                }
#endif
                newSpeed = speed + myCollisionAvoidanceGainGap * spacingErr + myCollisionAvoidanceGainGapDot * speedErr;
                vehMode = CACC_COLLISION_AVOIDANCE_MODE;
            } else {
                // gap closing mode
#if DEBUG_CACC == 1
                if (DEBUG_COND) {
                    std::cout << "        applying gap closing err=" << spacingErr << " speedErr=" << speedErr << " predSpeed=" << predSpeed << " speed=" << speed << " accel=" << accel << "\n";
                }
#endif
                newSpeed = speed + myGapClosingControlGainGap * spacingErr + myGapClosingControlGainGapDot * speedErr;

                vehMode = CACC_GAP_CLOSING_MODE;
            }
        }
    } else {
        /* no leader */
#if DEBUG_CACC == 1
        if (DEBUG_COND) {
            std::cout << "        no leader" << std::endl;
        }
#endif
        newSpeed = speedSpeedControl(speed, vErr, vehMode);
    }

    return newSpeed;
}

double
MSCFModel_CACC::_v(const MSVehicle* const veh, const MSVehicle* const pred, const double gap2pred, const double speed,
                   const double predSpeed, const double desSpeed, const bool /* respectMinGap */, const CalcReason usage) const {
    double newSpeed = 0.0;
    VehicleMode vehMode = CC_MODE;

#if DEBUG_CACC == 1
    if (DEBUG_COND) {
        std::cout << SIMTIME << " MSCFModel_CACC::_v() for veh '" << veh->getID()
                  << " gap=" << gap2pred << " speed="  << speed << " predSpeed=" << predSpeed
                  << " desSpeed=" << desSpeed << std::endl;
    }
#endif

    /* Velocity error */
    double vErr = speed - desSpeed;
    bool setControlMode = false;
    CACCVehicleVariables* vars = (CACCVehicleVariables*)veh->getCarFollowVariables();
    if (vars->lastUpdateTime != MSNet::getInstance()->getCurrentTimeStep()) {
        vars->lastUpdateTime = MSNet::getInstance()->getCurrentTimeStep();
        setControlMode = true;
    }

    CommunicationsOverrideMode commMode = vars->CACC_CommunicationsOverrideMode;

    if (commMode == CACC_NO_OVERRIDE) { // old CACC logic (rely on time gap from predecessor)
        // @note: using (gap2pred + minGap) here increases oscillations but may
        // actually be a good idea once the acceleration-spike-problem is fixed
        double time_gap = gap2pred / MAX2(NUMERICAL_EPS, speed);
        double spacingErr = gap2pred - myHeadwayTime * speed;

        if (time_gap > 2 && spacingErr > mySpeedControlMinGap) {
#if DEBUG_CACC == 1
            if (DEBUG_COND) {
                std::cout << "        applying speedControl" << "  time_gap=" << time_gap << std::endl;
            }
#endif
            // Find acceleration - Speed control law
            newSpeed = speedSpeedControl(speed, vErr, vehMode);
            // Set cl to vehicle parameters
            if (setControlMode) {
                vars->CACC_ControlMode = 0;
            }
        } else if (time_gap < 1.5) {
            // Find acceleration - Gap control law
#if DEBUG_CACC == 1
            if (DEBUG_COND) {
                std::cout << "        speedGapControl" << "  time_gap=" << time_gap << std::endl;
            }
#endif

            newSpeed = speedGapControl(veh, gap2pred, speed, predSpeed, desSpeed, vErr, pred, vehMode);
            // Set cl to vehicle parameters
            if (setControlMode) {
                vars->CACC_ControlMode = 1;
            }
        } else {
            // Follow previous applied law
            int cm = vars->CACC_ControlMode;
            if (!cm) {
                // CACC_ControlMode = speed control

#if DEBUG_CACC == 1
                if (DEBUG_COND) {
                    std::cout << "        applying speedControl (previous)" <<  "  time_gap=" << time_gap << std::endl;
                }
#endif
                newSpeed = speedSpeedControl(speed, vErr, vehMode);
            } else {
                // CACC_ControlMode = gap control
#if DEBUG_CACC == 1
                if (DEBUG_COND) {
                    std::cout << "        previous speedGapControl (previous)" << "  time_gap=" << time_gap << std::endl;
                }
#endif
                newSpeed = speedGapControl(veh, gap2pred, speed, predSpeed, desSpeed, vErr, pred, vehMode);
            }
        }
    } else if (commMode == CACC_MODE_NO_LEADER) {
        newSpeed = speedSpeedControl(speed, vErr, vehMode);
    } else if (commMode == CACC_MODE_LEADER_NO_CAV) {
        newSpeed = acc_CFM._v(veh, gap2pred, speed, predSpeed, desSpeed, true);
        vehMode = ACC_MODE;
    } else if (commMode == CACC_MODE_LEADER_CAV) {
        double desSpacing = myHeadwayTime * speed;
        double spacingErr = gap2pred - desSpacing;
        double accel = veh->getAcceleration();
        double speedErr = predSpeed - speed + myHeadwayTime * accel;

        if ((spacingErr > 0 && spacingErr < 0.2) && (vErr < 0.1)) {
            // gap mode
            //newSpeed = speed + 0.45 * spacingErr + 0.0125 *speedErr;
            if (DEBUG_COND) {
                std::cout << "        applying CACC_GAP_MODE " << std::endl;
            }
            newSpeed = speed + myGapControlGainGap * spacingErr + myGapControlGainGapDot * speedErr;
            vehMode = CACC_GAP_MODE;
        } else if (spacingErr < 0) {
            // collision avoidance mode
            //newSpeed = speed + 0.45 * spacingErr + 0.05 *speedErr;
            if (DEBUG_COND) {
                std::cout << "        applying CACC_COLLISION_AVOIDANCE_MODE " << std::endl;
            }
            newSpeed = speed + myCollisionAvoidanceGainGap * spacingErr + myCollisionAvoidanceGainGapDot * speedErr;
            vehMode = CACC_COLLISION_AVOIDANCE_MODE;
        } else {
            // gap closing mode
            if (DEBUG_COND) {
                std::cout << "        applying CACC_GAP_CLOSING_MODE " << std::endl;
            }
            newSpeed = speed + myGapClosingControlGainGap * spacingErr + myGapClosingControlGainGapDot * speedErr;
            vehMode = CACC_GAP_CLOSING_MODE;
        }
    }

    if (setControlMode && !MSGlobals::gComputeLC && usage == CalcReason::CURRENT) {
        const_cast<SUMOVehicleParameter&>(veh->getParameter()).setParameter("caccVehicleMode", VehicleModeNames[vehMode]);
    }

    //std::cout << veh->getID() << " commMode: " << commMode << ", caccVehicleMode: " << VehicleModeNames[vehMode]
    //            << ", gap2pred: " << gap2pred << ", newSpeed: " << newSpeed << std::endl;

    // newSpeed is meant for step length 0.1 but this would cause extreme
    // accelerations at lower step length
    double newSpeedScaled = newSpeed;
    if (DELTA_T < 100) {
        const double accel01 = (newSpeed - speed) * 10;
        newSpeedScaled = speed + ACCEL2SPEED(accel01);
    }

#if DEBUG_CACC == 1
    if (DEBUG_COND) {
        std::cout << "        result: rawAccel=" << SPEED2ACCEL(newSpeed - speed) << " newSpeed=" << newSpeed << " newSpeedScaled=" << newSpeedScaled << "\n";
    }
#endif

    return MAX2(0., newSpeedScaled);
}



MSCFModel*
MSCFModel_CACC::duplicate(const MSVehicleType* vtype) const {
    return new MSCFModel_CACC(vtype);
}
