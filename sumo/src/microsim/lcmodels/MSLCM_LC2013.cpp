/****************************************************************************/
/// @file    MSLCM_LC2013.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Friedemann Wesner
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @author  Leonhard Luecken
/// @date    Fri, 08.10.2013
/// @version $Id$
///
// A lane change model developed by J. Erdmann
// based on the model of D. Krajzewicz developed between 2004 and 2011 (MSLCM_DK2004)
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

#include <iostream>
#include <utils/common/RandHelper.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSNet.h>
#include "MSLCM_LC2013.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// variable definitions
// ===========================================================================
// 80km/h will be the threshold for dividing between long/short foresight
#define LOOK_FORWARD_SPEED_DIVIDER (SUMOReal)14.

#define LOOK_FORWARD_RIGHT (SUMOReal)10.
#define LOOK_FORWARD_LEFT  (SUMOReal)20.

#define JAM_FACTOR (SUMOReal)1.

#define LCA_RIGHT_IMPATIENCE (SUMOReal)-1.
#define CUT_IN_LEFT_SPEED_THRESHOLD (SUMOReal)27.

#define LOOK_AHEAD_MIN_SPEED (SUMOReal)0.0
#define LOOK_AHEAD_SPEED_MEMORY (SUMOReal)0.9
#define LOOK_AHEAD_SPEED_DECREMENT 6.

#define HELP_DECEL_FACTOR (SUMOReal)1.0

#define HELP_OVERTAKE  (SUMOReal)(10.0 / 3.6)
#define MIN_FALLBEHIND  (SUMOReal)(7.0 / 3.6)

#define RELGAIN_NORMALIZATION_MIN_SPEED (SUMOReal)10.0
#define URGENCY (SUMOReal)2.0

#define KEEP_RIGHT_TIME (SUMOReal)5.0 // the number of seconds after which a vehicle should move to the right lane
#define KEEP_RIGHT_ACCEPTANCE (SUMOReal)7.0 // calibration factor for determining the desire to keep right
#define ROUNDABOUT_DIST_BONUS (SUMOReal)100.0 // valence (distance) for to faked per roundabout edge in front (inducing inner lane usage in roundabouts by decreasing sense of lc-urgency)

#define ROUNDABOUT_DIST_FACTOR (SUMOReal)10.0 // Must be >=1.0, serves an alternative way of decreasing sense lc-urgency by multiplying the distance along the next roundabout
#define ROUNDABOUT_DIST_TRESH (SUMOReal)10.0  // roundabout distances below ROUNDABOUT_DIST_TRESH are not multiplied by ROUNDABOUT_DIST_FACTOR

#define KEEP_RIGHT_HEADWAY (SUMOReal)2.0
#define MAX_ONRAMP_LENGTH (SUMOReal)200.
#define TURN_LANE_DIST (SUMOReal)200.0 // the distance at which a lane leading elsewhere is considered to be a turn-lane that must be avoided

// ===========================================================================
// debug defines
// ===========================================================================
//#define DEBUG_PATCH_SPEED
//#define DEBUG_INFORMED
//#define DEBUG_INFORMER
//#define DEBUG_CONSTRUCTOR
//#define DEBUG_WANTS_CHANGE
//#define DEBUG_SLOW_DOWN
//#define DEBUG_SAVE_BLOCKER_LENGTH

#define DEBUG_COND (myVehicle.getID() == "disabled")

// ===========================================================================
// member method definitions
// ===========================================================================
MSLCM_LC2013::MSLCM_LC2013(MSVehicle& v) :
    MSAbstractLaneChangeModel(v, LCM_LC2013),
    mySpeedGainProbability(0),
    myKeepRightProbability(0),
    myLeadingBlockerLength(0),
    myLeftSpace(0),
    myLookAheadSpeed(LOOK_AHEAD_MIN_SPEED),
    myStrategicParam(v.getVehicleType().getParameter().getLCParam(SUMO_ATTR_LCA_STRATEGIC_PARAM, 1)),
    myCooperativeParam(v.getVehicleType().getParameter().getLCParam(SUMO_ATTR_LCA_COOPERATIVE_PARAM, 1)),
    mySpeedGainParam(v.getVehicleType().getParameter().getLCParam(SUMO_ATTR_LCA_SPEEDGAIN_PARAM, 1)),
    myKeepRightParam(v.getVehicleType().getParameter().getLCParam(SUMO_ATTR_LCA_KEEPRIGHT_PARAM, 1)),
    myChangeProbThresholdRight(2.0 * myKeepRightParam / MAX2(NUMERICAL_EPS, mySpeedGainParam)),
    myChangeProbThresholdLeft(0.2 / MAX2(NUMERICAL_EPS, mySpeedGainParam)) {
#ifdef DEBUG_CONSTRUCTOR
    if (DEBUG_COND) {
        std::cout << SIMTIME
                  << " create lcModel veh=" << myVehicle.getID()
                  << " lcStrategic=" << myStrategicParam
                  << " lcCooperative=" << myCooperativeParam
                  << " lcSpeedGain=" << mySpeedGainParam
                  << " lcKeepRight=" << myKeepRightParam
                  << "\n";
    }
#endif
}

MSLCM_LC2013::~MSLCM_LC2013() {
    changed();
}


bool
MSLCM_LC2013::debugVehicle() const {
    return DEBUG_COND;
}


int
MSLCM_LC2013::wantsChange(
    int laneOffset,
    MSAbstractLaneChangeModel::MSLCMessager& msgPass,
    int blocked,
    const std::pair<MSVehicle*, SUMOReal>& leader,
    const std::pair<MSVehicle*, SUMOReal>& neighLead,
    const std::pair<MSVehicle*, SUMOReal>& neighFollow,
    const MSLane& neighLane,
    const std::vector<MSVehicle::LaneQ>& preb,
    MSVehicle** lastBlocked,
    MSVehicle** firstBlocked) {

#ifdef DEBUG_WANTS_CHANGE
    if (DEBUG_COND) {
        std::cout << "\nWANTS_CHANGE\n" << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                  //<< std::setprecision(10)
                  << " veh=" << myVehicle.getID()
                  << " lane=" << myVehicle.getLane()->getID()
                  << " pos=" << myVehicle.getPositionOnLane()
                  << " posLat=" << myVehicle.getLateralPositionOnLane()
                  << " speed=" << myVehicle.getSpeed()
                  << " considerChangeTo=" << (laneOffset == -1  ? "right" : "left")
                  << "\n";
    }
#endif

    const int result = _wantsChange(laneOffset, msgPass, blocked, leader, neighLead, neighFollow, neighLane, preb, lastBlocked, firstBlocked);

#ifdef DEBUG_WANTS_CHANGE
    if (DEBUG_COND) {
        if (result & LCA_WANTS_LANECHANGE) {
            std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                      << " veh=" << myVehicle.getID()
                      << " wantsChangeTo=" << (laneOffset == -1  ? "right" : "left")
                      << ((result & LCA_URGENT) ? " (urgent)" : "")
                      << ((result & LCA_CHANGE_TO_HELP) ? " (toHelp)" : "")
                      << ((result & LCA_STRATEGIC) ? " (strat)" : "")
                      << ((result & LCA_COOPERATIVE) ? " (coop)" : "")
                      << ((result & LCA_SPEEDGAIN) ? " (speed)" : "")
                      << ((result & LCA_KEEPRIGHT) ? " (keepright)" : "")
                      << ((result & LCA_TRACI) ? " (traci)" : "")
                      << ((blocked & LCA_BLOCKED) ? " (blocked)" : "")
                      << ((blocked & LCA_OVERLAPPING) ? " (overlap)" : "")
                      << "\n\n\n";
        }
    }
#endif

    return result;
}


SUMOReal
MSLCM_LC2013::patchSpeed(const SUMOReal min, const SUMOReal wanted, const SUMOReal max, const MSCFModel& cfModel) {

#ifdef DEBUG_PATCH_SPEED
    if (DEBUG_COND) {
        std::cout << "\nPATCH_SPEED\n"
                  << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                  << " veh=" << myVehicle.getID()
                  << " lane=" << myVehicle.getLane()->getID()
                  << " pos=" << myVehicle.getPositionOnLane()
                  << " v=" << myVehicle.getSpeed()
                  << " wanted=" << wanted << "\n";
    }
#endif

    const SUMOReal newSpeed = _patchSpeed(min, wanted, max, cfModel);

#ifdef DEBUG_PATCH_SPEED
    if (DEBUG_COND) {
        const std::string patched = (wanted != newSpeed ? " patched=" + toString(newSpeed) : "");
        std::cout << patched
                  << "\n";
    }
#endif

    return newSpeed;
}


SUMOReal
MSLCM_LC2013::_patchSpeed(const SUMOReal min, const SUMOReal wanted, const SUMOReal max, const MSCFModel& cfModel) {
    int state = myOwnState;
#ifdef DEBUG_PATCH_SPEED
    if (DEBUG_COND) {
        std::cout
                << "\n" << SIMTIME << " patchSpeed state=" << state << " myVSafes=" << toString(myVSafes)
                << " \nspeed=" << myVehicle.getSpeed()
                << " min=" << min
                << " wanted=" << wanted << std::endl;
    }
#endif

    // letting vehicles merge in at the end of the lane in case of counter-lane change, step#2
    SUMOReal MAGIC_offset = 1.;
    //   if we want to change and have a blocking leader and there is enough room for him in front of us
    if (myLeadingBlockerLength != 0) {
        SUMOReal space = myLeftSpace - myLeadingBlockerLength - MAGIC_offset - myVehicle.getVehicleType().getMinGap();
#ifdef DEBUG_PATCH_SPEED
        if (DEBUG_COND) {
            std::cout << SIMTIME << " veh=" << myVehicle.getID() << " myLeadingBlockerLength=" << myLeadingBlockerLength << " space=" << space << "\n";
        }
#endif
        if (space > 0) { // XXX space > -MAGIC_offset
            // compute speed for decelerating towards a place which allows the blocking leader to merge in in front
            SUMOReal safe = cfModel.stopSpeed(&myVehicle, myVehicle.getSpeed(), space);
            // if we are approaching this place
            if (safe < wanted) {
                // return this speed as the speed to use
#ifdef DEBUG_PATCH_SPEED
                if (DEBUG_COND) {
                    std::cout << time << " veh=" << myVehicle.getID() << " slowing down for leading blocker, safe=" << safe << (safe + NUMERICAL_EPS < min ? " (not enough)" : "") << "\n";
                }
#endif
                return MAX2(min, safe);
            }
        }
    }

    SUMOReal nVSafe = wanted;
    bool gotOne = false;
    for (std::vector<SUMOReal>::const_iterator i = myVSafes.begin(); i != myVSafes.end(); ++i) {
        SUMOReal v = (*i);

        if (v >= min && v <= max && (MSGlobals::gSemiImplicitEulerUpdate
                                     // ballistic update: (negative speeds may appear, e.g. min<0, v<0), BUT:
                                     // XXX: LaneChanging returns -1 to indicate no restrictions, which leads to probs here (Leo), refs. #2577
                                     //      As a quick fix, we just dismiss cases where v=-1
                                     //      VERY rarely (whenever a requested help-acceleration is really indicated by v=-1)
                                     //      this can lead to failing lane-change attempts, though)
                                     || v != -1)) {
            nVSafe = MIN2(v * myCooperativeParam + (1 - myCooperativeParam) * wanted, nVSafe);
            gotOne = true;
#ifdef DEBUG_PATCH_SPEED
            if (DEBUG_COND) {
                std::cout << time << " veh=" << myVehicle.getID() << " got nVSafe=" << nVSafe << "\n";
            }
#endif
        } else {
            if (v < min) {
#ifdef DEBUG_PATCH_SPEED
                if (DEBUG_COND) {
                    std::cout << time << " veh=" << myVehicle.getID() << " ignoring low nVSafe=" << v << " min=" << min << "\n";
                }
#endif
            } else {
#ifdef DEBUG_PATCH_SPEED
                if (DEBUG_COND) {
                    std::cout << time << " veh=" << myVehicle.getID() << " ignoring high nVSafe=" << v << " max=" << max << "\n";
                }
#endif
            }
        }
    }

    if (gotOne && !myDontBrake) { // XXX: myDontBrake is initialized as false and seems not to be changed anywhere... What's its purpose???
#ifdef DEBUG_PATCH_SPEED
        if (DEBUG_COND) {
            std::cout << time << " veh=" << myVehicle.getID() << " got vSafe\n";
        }
#endif
        return nVSafe;
    }

    // check whether the vehicle is blocked
    if ((state & LCA_WANTS_LANECHANGE) != 0 && (state & LCA_BLOCKED) != 0) {
        if ((state & LCA_STRATEGIC) != 0) {
            // necessary decelerations are controlled via vSafe. If there are
            // none it means we should speed up
#ifdef DEBUG_PATCH_SPEED
            if (DEBUG_COND) {
                std::cout << time << " veh=" << myVehicle.getID() << " LCA_WANTS_LANECHANGE (strat, no vSafe)\n";
            }
#endif
            return (max + wanted) / (SUMOReal) 2.0;
        } else if ((state & LCA_COOPERATIVE) != 0) {
            // only minor adjustments in speed should be done
            if ((state & LCA_BLOCKED_BY_LEADER) != 0) {
#ifdef DEBUG_PATCH_SPEED
                if (DEBUG_COND) {
                    std::cout << time << " veh=" << myVehicle.getID() << " LCA_BLOCKED_BY_LEADER (coop)\n";
                }
#endif
                if (wanted >= 0.) {
                    return (MAX2((SUMOReal)0., min) + wanted) / (SUMOReal) 2.0;
                } else {
                    return wanted;
                }
            }
            if ((state & LCA_BLOCKED_BY_FOLLOWER) != 0) {
#ifdef DEBUG_PATCH_SPEED
                if (DEBUG_COND) {
                    std::cout << time << " veh=" << myVehicle.getID() << " LCA_BLOCKED_BY_FOLLOWER (coop)\n";
                }
#endif
                return (max + wanted) / (SUMOReal) 2.0;
            }
            //} else { // VARIANT_16
            //    // only accelerations should be performed
            //    if ((state & LCA_BLOCKED_BY_FOLLOWER) != 0) {
            //        if (gDebugFlag2) std::cout << time << " veh=" << myVehicle.getID() << " LCA_BLOCKED_BY_FOLLOWER\n";
            //        return (max + wanted) / (SUMOReal) 2.0;
            //    }
        }
    }

    /*
    // decelerate if being a blocking follower
    //  (and does not have to change lanes)
    if ((state & LCA_AMBLOCKINGFOLLOWER) != 0) {
        if (fabs(max - myVehicle.getCarFollowModel().maxNextSpeed(myVehicle.getSpeed(), &myVehicle)) < 0.001 && min == 0) { // !!! was standing
            if (gDebugFlag2) std::cout << time << " veh=" << myVehicle.getID() << " LCA_AMBLOCKINGFOLLOWER (standing)\n";
            return 0;
        }
        if (gDebugFlag2) std::cout << time << " veh=" << myVehicle.getID() << " LCA_AMBLOCKINGFOLLOWER\n";

        //return min; // VARIANT_3 (brakeStrong)
        return (min + wanted) / (SUMOReal) 2.0;
    }
    if ((state & LCA_AMBACKBLOCKER) != 0) {
        if (max <= myVehicle.getCarFollowModel().maxNextSpeed(myVehicle.getSpeed(), &myVehicle) && min == 0) { // !!! was standing
            if (gDebugFlag2) std::cout << time << " veh=" << myVehicle.getID() << " LCA_AMBACKBLOCKER (standing)\n";
            //return min; VARIANT_9 (backBlockVSafe)
            return nVSafe;
        }
    }
    if ((state & LCA_AMBACKBLOCKER_STANDING) != 0) {
        if (gDebugFlag2) std::cout << time << " veh=" << myVehicle.getID() << " LCA_AMBACKBLOCKER_STANDING\n";
        //return min;
        return nVSafe;
    }
    */

    // accelerate if being a blocking leader or blocking follower not able to brake
    //  (and does not have to change lanes)
    if ((state & LCA_AMBLOCKINGLEADER) != 0) {
#ifdef DEBUG_PATCH_SPEED
        if (DEBUG_COND) {
            std::cout << time << " veh=" << myVehicle.getID() << " LCA_AMBLOCKINGLEADER\n";
        }
#endif
        return (max + wanted) / (SUMOReal) 2.0;
    }

    if ((state & LCA_AMBLOCKINGFOLLOWER_DONTBRAKE) != 0) {
#ifdef DEBUG_PATCH_SPEED
        if (DEBUG_COND) {
            std::cout << time << " veh=" << myVehicle.getID() << " LCA_AMBLOCKINGFOLLOWER_DONTBRAKE\n";
        }
#endif
        /*
        // VARIANT_4 (dontbrake)
        if (max <= myVehicle.getCarFollowModel().maxNextSpeed(myVehicle.getSpeed(), &myVehicle) && min == 0) { // !!! was standing
            return wanted;
        }
        return (min + wanted) / (SUMOReal) 2.0;
        */
    }
    if (!myVehicle.getLane()->getEdge().hasLaneChanger()) {
        // remove chaning information if on a road with a single lane
        changed();
    }
    return wanted;
}


void*
MSLCM_LC2013::inform(void* info, MSVehicle* sender) {
    UNUSED_PARAMETER(sender);
    Info* pinfo = (Info*)info;
    assert(pinfo->first >= 0 || !MSGlobals::gSemiImplicitEulerUpdate);
    myVSafes.push_back(pinfo->first);
    myOwnState |= pinfo->second;
#ifdef DEBUG_INFORMED
    if (DEBUG_COND) {
        std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                  << " veh=" << myVehicle.getID()
                  << " informedBy=" << sender->getID()
                  << " info=" << pinfo->second
                  << " vSafe=" << pinfo->first
                  << "\n";
    }
#endif
    delete pinfo;
    return (void*) true;
}

SUMOReal
MSLCM_LC2013::overtakeDistance(const MSVehicle* follower, const MSVehicle* leader, const SUMOReal gap, SUMOReal followerSpeed, SUMOReal leaderSpeed) {
    followerSpeed = followerSpeed == INVALID_SPEED ? follower->getSpeed() : followerSpeed;
    leaderSpeed = leaderSpeed == INVALID_SPEED ? leader->getSpeed() : leaderSpeed;
    SUMOReal overtakeDist = (gap // drive to back of leader
                             + leader->getVehicleType().getLengthWithGap() // drive to front of leader
                             + follower->getVehicleType().getLength() // follower back reaches leader front
                             + leader->getCarFollowModel().getSecureGap( // save gap to leader
                                 leaderSpeed, followerSpeed, follower->getCarFollowModel().getMaxDecel()));
    return MAX2(overtakeDist, (SUMOReal)0.);
}


SUMOReal
MSLCM_LC2013::informLeader(MSAbstractLaneChangeModel::MSLCMessager& msgPass,
                           int blocked,
                           int dir,
                           const std::pair<MSVehicle*, SUMOReal>& neighLead,
                           SUMOReal remainingSeconds) {
    SUMOReal plannedSpeed = MIN2(myVehicle.getSpeed(),
                                 myVehicle.getCarFollowModel().stopSpeed(&myVehicle, myVehicle.getSpeed(), myLeftSpace - myLeadingBlockerLength));
    for (std::vector<SUMOReal>::const_iterator i = myVSafes.begin(); i != myVSafes.end(); ++i) {
        SUMOReal v = (*i);
        if (v >= myVehicle.getSpeed() - ACCEL2SPEED(myVehicle.getCarFollowModel().getMaxDecel())) {
            plannedSpeed = MIN2(plannedSpeed, v);
        }
    }
#ifdef DEBUG_INFORMER
    if (DEBUG_COND) {
        std::cout << "\nINFORM_LEADER"
                  << "\nspeed=" <<  myVehicle.getSpeed() << " planned=" << plannedSpeed << "\n";
    }
#endif

    if ((blocked & LCA_BLOCKED_BY_LEADER) != 0) {
        assert(neighLead.first != 0);
        MSVehicle* nv = neighLead.first;
#ifdef DEBUG_INFORMER
        if (DEBUG_COND) {
            std::cout << " blocked by leader nv=" <<  nv->getID() << " nvSpeed=" << nv->getSpeed() << " needGap="
                      << myVehicle.getCarFollowModel().getSecureGap(myVehicle.getSpeed(), nv->getSpeed(), nv->getCarFollowModel().getMaxDecel()) << "\n";
        }
#endif
        // decide whether we want to overtake the leader or follow it
        SUMOReal overtakeTime;
        const SUMOReal overtakeDist = overtakeDistance(&myVehicle, nv, neighLead.second);
        const SUMOReal dv = plannedSpeed - nv->getSpeed();

        if (dv > 0) {
            overtakeTime = overtakeDist / dv;
        } else {
            // -> set overtakeTime to something indicating impossibility of overtaking
            overtakeTime = remainingSeconds + 1;
        }

#ifdef DEBUG_INFORMER
        if (DEBUG_COND) {
            std::cout << SIMTIME << " informLeader() of " << myVehicle.getID()
                      << "\nnv = " << nv->getID()
                      << "\nplannedSpeed = " << plannedSpeed
                      << "\nleaderSpeed = " << nv->getSpeed()
                      << "\nmyLeftSpace = " << myLeftSpace
                      << "\nremainingSeconds = " << remainingSeconds
                      << "\novertakeDist = " << overtakeDist
                      << "\novertakeTime = " << overtakeTime
                      << std::endl;
        }
#endif

        if ((dv < 0
                // overtaking on the right on an uncongested highway is forbidden (noOvertakeLCLeft)
                || (dir == LCA_MLEFT && !myVehicle.congested() && !myAllowOvertakingRight)
                // not enough space to overtake?
                || (MSGlobals::gSemiImplicitEulerUpdate && myLeftSpace - myLeadingBlockerLength - myVehicle.getCarFollowModel().brakeGap(myVehicle.getSpeed()) < overtakeDist)
                // using brakeGap() without headway seems adequate in a situation where the obstacle (the lane end) is not moving [XXX implemented in branch ticket860, can be used in general if desired, refs. #2575] (Leo).
                || (!MSGlobals::gSemiImplicitEulerUpdate && myLeftSpace - myLeadingBlockerLength - myVehicle.getCarFollowModel().brakeGap(myVehicle.getSpeed(), myCarFollowModel.getMaxDecel(), 0.) < overtakeDist)
                // not enough time to overtake?        (skipped for a stopped leader [currently only for ballistic update XXX: check if appropriate for euler, too, refs. #2575] to ensure that it can be overtaken if only enough space is exists) (Leo)
                || (remainingSeconds < overtakeTime && (MSGlobals::gSemiImplicitEulerUpdate || !nv->isStopped())))
                // opposite driving and must overtake
                && !(isOpposite() && neighLead.second < 0 && neighLead.first->isStopped())) {
            // cannot overtake
            msgPass.informNeighLeader(new Info(std::numeric_limits<SUMOReal>::max(), dir | LCA_AMBLOCKINGLEADER), &myVehicle);
            // slow down smoothly to follow leader
            const SUMOReal targetSpeed = myCarFollowModel.followSpeed(
                                             &myVehicle, myVehicle.getSpeed(), neighLead.second, nv->getSpeed(), nv->getCarFollowModel().getMaxDecel());
            if (targetSpeed < myVehicle.getSpeed()) {
                // slow down smoothly to follow leader
                const SUMOReal decel = remainingSeconds == 0. ? myVehicle.getCarFollowModel().getMaxDecel() :
                                       MIN2(myVehicle.getCarFollowModel().getMaxDecel(),
                                            MAX2(MIN_FALLBEHIND, (myVehicle.getSpeed() - targetSpeed) / remainingSeconds));
                const SUMOReal nextSpeed = MIN2(plannedSpeed, myVehicle.getSpeed() - ACCEL2SPEED(decel));
#ifdef DEBUG_INFORMER
                if (DEBUG_COND) {
                    std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                              << " cannot overtake leader nv=" << nv->getID()
                              << " dv=" << dv
                              << " myLookAheadSpeed=" << myLookAheadSpeed
                              << " myLeftSpace=" << myLeftSpace
                              << " overtakeDist=" << overtakeDist
                              << " overtakeTime=" << overtakeTime
                              << " remainingSeconds=" << remainingSeconds
                              << " currentGap=" << neighLead.second
                              << " brakeGap=" << myVehicle.getCarFollowModel().brakeGap(myVehicle.getSpeed(), myCarFollowModel.getMaxDecel(), 0.)
                              << " secureGap=" << nv->getCarFollowModel().getSecureGap(nv->getSpeed(), myVehicle.getSpeed(), myVehicle.getCarFollowModel().getMaxDecel())
                              << " targetSpeed=" << targetSpeed
                              << " nextSpeed=" << nextSpeed
                              << "\n";
                }
#endif
                myVSafes.push_back(nextSpeed);
                return nextSpeed;
            } else {
                // leader is fast enough anyway
#ifdef DEBUG_INFORMER
                if (DEBUG_COND) {
                    std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                              << " cannot overtake fast leader nv=" << nv->getID()
                              << " dv=" << dv
                              << " myLookAheadSpeed=" << myLookAheadSpeed
                              << " myLeftSpace=" << myLeftSpace
                              << " overtakeDist=" << overtakeDist
                              << " myLeadingBlockerLength=" << myLeadingBlockerLength
                              << " overtakeTime=" << overtakeTime
                              << " remainingSeconds=" << remainingSeconds
                              << " currentGap=" << neighLead.second
                              << " targetSpeed=" << targetSpeed
                              << "\n";
                }
#endif
                myVSafes.push_back(targetSpeed);
                return plannedSpeed;
            }
        } else {
            // overtaking, leader should not accelerate
#ifdef DEBUG_INFORMER
            if (DEBUG_COND) {
                std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                          << " wants to overtake leader nv=" << nv->getID()
                          << " dv=" << dv
                          << " overtakeDist=" << overtakeDist
                          << " remainingSeconds=" << remainingSeconds
                          << " overtakeTime=" << overtakeTime
                          << " currentGap=" << neighLead.second
                          << " secureGap=" << nv->getCarFollowModel().getSecureGap(nv->getSpeed(), myVehicle.getSpeed(), myVehicle.getCarFollowModel().getMaxDecel())
                          << "\n";
            }
#endif
            msgPass.informNeighLeader(new Info(nv->getSpeed(), dir | LCA_AMBLOCKINGLEADER), &myVehicle);
            return -1;  // XXX: using -1 is ambiguous for the ballistic update! Currently this is being catched in patchSpeed() (Leo), consider returning INVALID_SPEED, refs. #2577
        }
    } else if (neighLead.first != 0) { // (remainUnblocked)
        // we are not blocked now. make sure we stay far enough from the leader
        MSVehicle* nv = neighLead.first;
        const SUMOReal nextNVSpeed = nv->getSpeed() - HELP_OVERTAKE; // conservative
        const SUMOReal dv = SPEED2DIST(myVehicle.getSpeed() - nextNVSpeed);
        const SUMOReal targetSpeed = myCarFollowModel.followSpeed(
                                         &myVehicle, myVehicle.getSpeed(), neighLead.second - dv, nextNVSpeed, nv->getCarFollowModel().getMaxDecel());
        myVSafes.push_back(targetSpeed);
#ifdef DEBUG_INFORMER
        if (DEBUG_COND) {
            std::cout << " not blocked by leader nv=" <<  nv->getID()
                      << " nvSpeed=" << nv->getSpeed()
                      << " gap=" << neighLead.second
                      << " nextGap=" << neighLead.second - dv
                      << " needGap=" << myVehicle.getCarFollowModel().getSecureGap(myVehicle.getSpeed(), nv->getSpeed(), nv->getCarFollowModel().getMaxDecel())
                      << " targetSpeed=" << targetSpeed
                      << "\n";
        }
#endif
        return MIN2(targetSpeed, plannedSpeed);
    } else {
        // not overtaking
        return plannedSpeed;
    }
}

void
MSLCM_LC2013::informFollower(MSAbstractLaneChangeModel::MSLCMessager& msgPass,
                             int blocked,
                             int dir,
                             const std::pair<MSVehicle*, SUMOReal>& neighFollow,
                             SUMOReal remainingSeconds,
                             SUMOReal plannedSpeed) {

    MSVehicle* nv = neighFollow.first;
    const SUMOReal plannedAccel = SPEED2ACCEL(MAX2(MIN2(myCarFollowModel.getMaxAccel(), plannedSpeed - myVehicle.getSpeed()), -myCarFollowModel.getMaxDecel()));

#ifdef DEBUG_INFORMER
    if (DEBUG_COND) {
        std::cout << "\nINFORM_FOLLOWER"
                  << "\nspeed=" <<  myVehicle.getSpeed() << " planned=" << plannedSpeed << "\n";
    }

#endif
    if ((blocked & LCA_BLOCKED_BY_FOLLOWER) != 0) {
        assert(nv != 0);
#ifdef DEBUG_INFORMER
        if (DEBUG_COND) {
            std::cout << " blocked by follower nv=" <<  nv->getID() << " nvSpeed=" << nv->getSpeed() << " needGap="
                      << nv->getCarFollowModel().getSecureGap(nv->getSpeed(), myVehicle.getSpeed(), myVehicle.getCarFollowModel().getMaxDecel()) << " planned=" << plannedSpeed <<  "\n";
        }
#endif

        // are we fast enough to cut in without any help?
        if (MAX2(plannedSpeed, (SUMOReal)0.) - nv->getSpeed() >= HELP_OVERTAKE) {
            const SUMOReal neededGap = nv->getCarFollowModel().getSecureGap(nv->getSpeed(), plannedSpeed, myVehicle.getCarFollowModel().getMaxDecel());
            if ((neededGap - neighFollow.second) / remainingSeconds < (MAX2(plannedSpeed, (SUMOReal)0.) - nv->getSpeed())) {
#ifdef DEBUG_INFORMER
                if (DEBUG_COND) {
                    std::cout << " wants to cut in before  nv=" << nv->getID() << " without any help." << "\nneededGap = " << neededGap << "\n";
                }
#endif
                // follower might even accelerate but not to much
                // XXX: I don't understand this. The needed gap was determined for nv->getSpeed(), not for (plannedSpeed - HELP_OVERTAKE)?! (Leo), refs. #2578
                msgPass.informNeighFollower(new Info(MAX2(plannedSpeed, (SUMOReal)0.) - HELP_OVERTAKE, dir | LCA_AMBLOCKINGFOLLOWER), &myVehicle);
                return;
            }
        }

        // decide whether we will request help to cut in before the follower or allow to be overtaken

        // PARAMETERS
        // assume other vehicle will assume the equivalent of 1 second of
        // maximum deceleration to help us (will probably be spread over
        // multiple seconds)
        // -----------
        const SUMOReal helpDecel = nv->getCarFollowModel().getMaxDecel() * HELP_DECEL_FACTOR;

        // follower's new speed in next step
        SUMOReal neighNewSpeed;
        // follower's new speed after 1s.
        SUMOReal neighNewSpeed1s;
        // velocity difference, gap after follower-deceleration
        SUMOReal dv, decelGap;

        if (MSGlobals::gSemiImplicitEulerUpdate) {
            // euler
            neighNewSpeed = MAX2((SUMOReal)0, nv->getSpeed() - ACCEL2SPEED(helpDecel));
            neighNewSpeed1s = MAX2((SUMOReal)0, nv->getSpeed() - helpDecel); // TODO: consider introduction of a configurable anticipationTime here (see far below in the !blocked part). Refs. #2578
            // change in the gap between ego and blocker over 1 second (not STEP!)
            // XXX: though here it is calculated as if it were one step!? (Leo) Refs. #2578
            dv = plannedSpeed - neighNewSpeed1s; // XXX: what is this quantity (if TS!=1)?
            // new gap between follower and self in case the follower does brake for 1s
            // XXX: if the step-length is not 1s., this is not the gap after 1s. deceleration!
            //      And this formula overestimates the real gap. Isn't that problematic? (Leo)
            //      Below, it seems that decelGap > secureGap is taken to indicate the possibility
            //      to cut in within the next time-step. However, this is not the case, if TS<1s.,
            //      since decelGap is (not exactly, though!) the gap after 1s. Refs. #2578
            decelGap = neighFollow.second + dv;
        } else {
            // ballistic
            // negative newSpeed-extrapolation possible, if stop lies within the next time-step
            // XXX: this code should work for the euler case as well, since gapExtrapolation() takes
            //      care of this, but for TS!=1 we will have different behavior (see previous remark) Refs. #2578
            neighNewSpeed = nv->getSpeed() - ACCEL2SPEED(helpDecel);
            neighNewSpeed1s = nv->getSpeed() - helpDecel;

            dv = myVehicle.getSpeed() - nv->getSpeed(); // current velocity difference
            decelGap = myCarFollowModel.gapExtrapolation(1., neighFollow.second, myVehicle.getSpeed(),
                       nv->getSpeed(), plannedAccel, -helpDecel, myVehicle.getMaxSpeedOnLane(), nv->getMaxSpeedOnLane());
        }

        const SUMOReal secureGap = nv->getCarFollowModel().getSecureGap(MAX2(neighNewSpeed1s, (SUMOReal)0.),
                                   MAX2(plannedSpeed, (SUMOReal)0.), myVehicle.getCarFollowModel().getMaxDecel());

#ifdef DEBUG_INFORMER
        if (DEBUG_COND) {
            std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                      << " speed=" << myVehicle.getSpeed()
                      << " plannedSpeed=" << plannedSpeed
                      << " neighNewSpeed=" << neighNewSpeed
                      << " neighNewSpeed1s=" << neighNewSpeed1s
                      << " dv=" << dv
                      << " gap=" << neighFollow.second
                      << " decelGap=" << decelGap
                      << " secureGap=" << secureGap
                      << "\n";
        }
#endif

        if (decelGap > 0 && decelGap >= secureGap) {
            // XXX: This does not assure that the leader can cut in in the next step if TS < 1 (see above)
            //      this seems to be supposed in the following (euler code)...?! (Leo) Refs. #2578

            // if the blocking follower brakes it could help
            // how hard does it actually need to be?
            // to be safe in the next step the following equation has to hold for the follower's vsafe:
            //   vsafe <= followSpeed(gap=currentGap - SPEED2DIST(vsafe), ...)
            SUMOReal vsafe, vsafe1;

            if (MSGlobals::gSemiImplicitEulerUpdate) {
                // euler
                // we compute an upper bound on vsafe by doing the computation twice
                vsafe1 = MAX2(neighNewSpeed, nv->getCarFollowModel().followSpeed(
                                  nv, nv->getSpeed(), neighFollow.second + SPEED2DIST(plannedSpeed), plannedSpeed, myCarFollowModel.getMaxDecel()));
                vsafe = MAX2(neighNewSpeed, nv->getCarFollowModel().followSpeed(
                                 nv, nv->getSpeed(), neighFollow.second + SPEED2DIST(plannedSpeed - vsafe1), plannedSpeed, myCarFollowModel.getMaxDecel()));
                assert(vsafe <= vsafe1);
            } else {
                // ballistic

                // XXX: This block should actually do as well for euler update (TODO: test!), refs #2575
                // we compute an upper bound on vsafe
                // next step's gap without help deceleration (nv's speed assumed constant)
                SUMOReal nextGap = myCarFollowModel.gapExtrapolation(TS,
                                   neighFollow.second, myVehicle.getSpeed(),
                                   nv->getSpeed(), plannedAccel, 0,
                                   myVehicle.getMaxSpeedOnLane(), nv->getMaxSpeedOnLane());
#ifdef DEBUG_INFORMER
                if (DEBUG_COND) {
                    std::cout << "nextGap=" << nextGap << " (without help decel) \n";
                }
#endif

                // NOTE: the second argument of MIN2() can get larger than nv->getSpeed()
                vsafe1 = MIN2(nv->getSpeed(), MAX2(neighNewSpeed,
                                                   nv->getCarFollowModel().followSpeed(nv,
                                                           nv->getSpeed(), nextGap,
                                                           MAX2((SUMOReal)0., plannedSpeed),
                                                           myCarFollowModel.getMaxDecel())));


                // next step's gap with possibly less than maximal help deceleration (in case vsafe1 > neighNewSpeed)
                SUMOReal decel2 = SPEED2ACCEL(nv->getSpeed() - vsafe1);
                nextGap = myCarFollowModel.gapExtrapolation(TS,
                          neighFollow.second, myVehicle.getSpeed(),
                          nv->getSpeed(), plannedAccel, -decel2,
                          myVehicle.getMaxSpeedOnLane(), nv->getMaxSpeedOnLane());

                // vsafe = MAX(neighNewSpeed, safe speed assuming next_gap)
                // Thus, the gap resulting from vsafe is larger or equal to next_gap
                // in contrast to the euler case, where nv's follow speed doesn't depend on the actual speed,
                // we need to assure, that nv doesn't accelerate
                vsafe = MIN2(nv->getSpeed(),
                             MAX2(neighNewSpeed,
                                  nv->getCarFollowModel().followSpeed(nv,
                                          nv->getSpeed(), nextGap,
                                          MAX2((SUMOReal)0., plannedSpeed),
                                          myCarFollowModel.getMaxDecel())));

                assert(vsafe >= vsafe1);

#ifdef DEBUG_INFORMER
                if (DEBUG_COND) {
                    std::cout << "nextGap=" << nextGap
                              << " (with vsafe1 and help decel) \nvsafe1=" << vsafe1
                              << " vsafe=" << vsafe
                              << "\n";
                }
#endif

                // For subsecond simulation, this might not lead to secure gaps for a long time,
                // we seek to establish a secure gap as soon as possible
                SUMOReal nextSecureGap = nv->getCarFollowModel().getSecureGap(vsafe, plannedSpeed, myCarFollowModel.getMaxDecel());

                if (nextGap < nextSecureGap) {
                    // establish a secureGap as soon as possible
                    vsafe = neighNewSpeed;
                }

#ifdef DEBUG_INFORMER
                if (DEBUG_COND) {
                    std::cout << "nextGap=" << nextGap
                              << " minNextSecureGap=" << nextSecureGap
                              << " vsafe=" << vsafe << "\n";
                }
#endif

            }
            msgPass.informNeighFollower(
                new Info(vsafe, dir | LCA_AMBLOCKINGFOLLOWER), &myVehicle);

#ifdef DEBUG_INFORMER
            if (DEBUG_COND) {
                std::cout << " wants to cut in before nv=" << nv->getID()
                          << " vsafe1=" << vsafe1 << " vsafe=" << vsafe
                          << " newSecGap="
                          << nv->getCarFollowModel().getSecureGap(vsafe,
                                  plannedSpeed,
                                  myVehicle.getCarFollowModel().getMaxDecel())
                          << "\n";
            }
#endif
        } else if ((MSGlobals::gSemiImplicitEulerUpdate && dv > 0 && dv * remainingSeconds > (secureGap - decelGap + POSITION_EPS))
                   || (!MSGlobals::gSemiImplicitEulerUpdate && dv > 0 && dv * (remainingSeconds - 1) > secureGap - decelGap + POSITION_EPS)
                  ) {

            // XXX: Alternative formulation (encapsulating differences of euler and ballistic) TODO: test, refs. #2575
            // SUMOReal eventualGap = myCarFollowModel.gapExtrapolation(remainingSeconds - 1., decelGap, plannedSpeed, neighNewSpeed1s);
            // } else if (eventualGap > secureGap + POSITION_EPS) {


            // NOTE: This case corresponds to the situation, where some time is left to perform the lc
            // For the ballistic case this is interpreted as follows:
            // If the follower breaks with helpDecel for one second, this vehicle maintains the plannedSpeed,
            // and both continue with their speeds for remainingSeconds seconds the gap will suffice for a laneChange
            // For the euler case we had the following comment:
            // 'decelerating once is sufficient to open up a large enough gap in time', but:
            // XXX: 1) Decelerating *once* does not necessarily lead to the gap decelGap! (if TS<1s.) (Leo)
            //      2) Probably, the if() for euler should test for dv * (remainingSeconds-1) > ..., too ?!, refs. #2578
            msgPass.informNeighFollower(new Info(neighNewSpeed, dir | LCA_AMBLOCKINGFOLLOWER), &myVehicle);
#ifdef DEBUG_INFORMER
            if (DEBUG_COND) {
                std::cout << " wants to cut in before nv=" << nv->getID() << " (eventually)\n";
            }
#endif
        } else if (dir == LCA_MRIGHT && !myAllowOvertakingRight && !nv->congested()) {
            // XXX: check if this requires a special treatment for the ballistic update, refs. #2575
            const SUMOReal vhelp = MAX2(neighNewSpeed, HELP_OVERTAKE);
            msgPass.informNeighFollower(new Info(vhelp, dir | LCA_AMBLOCKINGFOLLOWER), &myVehicle);
#ifdef DEBUG_INFORMER
            if (DEBUG_COND) {
                std::cout << " wants to cut in before nv=" << nv->getID() << " (nv cannot overtake right)\n";
            }
#endif
        } else {
            SUMOReal vhelp = MAX2(nv->getSpeed(), myVehicle.getSpeed() + HELP_OVERTAKE);
            //if (dir == LCA_MRIGHT && myVehicle.getWaitingSeconds() > LCA_RIGHT_IMPATIENCE &&
            //        nv->getSpeed() > myVehicle.getSpeed()) {
            if (nv->getSpeed() > myVehicle.getSpeed() &&
                    ((dir == LCA_MRIGHT && myVehicle.getWaitingSeconds() > LCA_RIGHT_IMPATIENCE) // NOTE: it might be considered to use myVehicle.getAccumulatedWaitingSeconds() > LCA_RIGHT_IMPATIENCE instead (Leo). Refs. #2578
                     || (dir == LCA_MLEFT && plannedSpeed > CUT_IN_LEFT_SPEED_THRESHOLD) // VARIANT_22 (slowDownLeft)
                     // XXX this is a hack to determine whether the vehicles is on an on-ramp. This information should be retrieved from the network itself
                     || (dir == LCA_MLEFT && myLeftSpace > MAX_ONRAMP_LENGTH)
                    )) {
                // let the follower slow down to increase the likelihood that later vehicles will be slow enough to help
                // follower should still be fast enough to open a gap
                // XXX: The probability for that success would be larger if the slow down of the appropriate following vehicle
                //      would take place without the immediate follower slowing down. We might consider to model reactions of
                //      vehicles that are not immediate followers. (Leo) -> see ticket #2532
                vhelp = MAX2(neighNewSpeed, myVehicle.getSpeed() + HELP_OVERTAKE);
#ifdef DEBUG_INFORMER
                if (DEBUG_COND) {
                    // NOTE: the condition labeled "VARIANT_22" seems to imply that this could as well concern the *left* follower?! (Leo)
                    //       Further, vhelp might be larger than nv->getSpeed(), so the request issued below is not to slow down!? (see below) Refs. #2578
                    std::cout << " wants right follower to slow down a bit\n";
                }
#endif
                if (MSGlobals::gSemiImplicitEulerUpdate) {
                    // euler
                    if ((nv->getSpeed() - myVehicle.getSpeed()) / helpDecel < remainingSeconds) {

#ifdef DEBUG_INFORMER
                        if (DEBUG_COND) {
                            // NOTE: the condition labeled "VARIANT_22" seems to imply that this could as well concern the *left* follower?! Refs. #2578
                            std::cout << " wants to cut in before right follower nv=" << nv->getID() << " (eventually)\n";
                        }
#endif
                        // XXX: I don't understand. This vhelp might be larger than nv->getSpeed() but the above condition seems to rely
                        //      on the reasoning that if nv breaks with helpDecel for remaining Seconds, nv will be so slow, that this
                        //      vehicle will be able to cut in. But nv might have overtaken this vehicle already (or am I missing sth?). (Leo)
                        //      Ad: To my impression, the intention behind allowing larger speeds for the blocking follower is to prevent a
                        //      situation, where an overlapping follower keeps blocking the ego vehicle. Refs. #2578
                        msgPass.informNeighFollower(new Info(vhelp, dir | LCA_AMBLOCKINGFOLLOWER), &myVehicle);
                        return;
                    }
                } else {

                    // ballistic (this block is a bit different to the logic in the euler part, but in general suited to work on euler as well.. must be tested <- TODO, refs. #2575)
                    // estimate gap after remainingSeconds.
                    // Assumptions:
                    // (A1) leader continues with currentSpeed. (XXX: That might be wrong: Think of accelerating on an on-ramp or of a congested region ahead!)
                    // (A2) follower breaks with helpDecel.
                    const SUMOReal gapAfterRemainingSecs = myCarFollowModel.gapExtrapolation(
                            remainingSeconds, neighFollow.second, myVehicle.getSpeed(), nv->getSpeed(), 0, -helpDecel, myVehicle.getMaxSpeedOnLane(), nv->getMaxSpeedOnLane());
                    const SUMOReal secureGapAfterRemainingSecs = nv->getCarFollowModel().getSecureGap(
                                MAX2(nv->getSpeed() - remainingSeconds * helpDecel, (SUMOReal)0.), myVehicle.getSpeed(), myVehicle.getCarFollowModel().getMaxDecel());
                    if (gapAfterRemainingSecs >= secureGapAfterRemainingSecs) { // XXX: here it would be wise to check whether there is enough space for eventual braking if the maneuver doesn't succeed
#ifdef DEBUG_INFORMER
                        if (DEBUG_COND) {
                            std::cout << " wants to cut in before follower nv=" << nv->getID() << " (eventually)\n";
                        }
#endif
                        // NOTE: ballistic uses neighNewSpeed instead of vhelp, see my note above. (Leo)
                        // TODO: recheck if this might cause suboptimal behaviour in some LC-situations. Refs. #2578
                        msgPass.informNeighFollower(new Info(neighNewSpeed, dir | LCA_AMBLOCKINGFOLLOWER), &myVehicle);
                        return;
                    }
                }


            }

#ifdef DEBUG_INFORMER
            if (DEBUG_COND) {
                std::cout << SIMTIME
                          << " veh=" << myVehicle.getID()
                          << " informs follower " << nv->getID()
                          << " vhelp=" << vhelp
                          << "\n";
            }
#endif

            msgPass.informNeighFollower(new Info(vhelp, dir | LCA_AMBLOCKINGFOLLOWER), &myVehicle);
            // This follower is supposed to overtake us. Slow down smoothly to allow this.
            const SUMOReal overtakeDist = overtakeDistance(nv, &myVehicle, neighFollow.second, vhelp, plannedSpeed);
            // speed difference to create a sufficiently large gap
            const SUMOReal needDV = overtakeDist / remainingSeconds;
            // make sure the deceleration is not to strong (XXX: should be assured in moveHelper -> TODO: remove the MAX2 if agreed) -> prob with possibly non-existing maximal deceleration for som CF Models(?) Refs. #2578
            myVSafes.push_back(MAX2(vhelp - needDV, myVehicle.getSpeed() - ACCEL2SPEED(myVehicle.getCarFollowModel().getMaxDecel())));

#ifdef DEBUG_INFORMER
            if (DEBUG_COND) {
                std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                          << " veh=" << myVehicle.getID()
                          << " wants to be overtaken by=" << nv->getID()
                          << " overtakeDist=" << overtakeDist
                          << " vneigh=" << nv->getSpeed()
                          << " vhelp=" << vhelp
                          << " needDV=" << needDV
                          << " vsafe=" << myVSafes.back()
                          << "\n";
            }
#endif
        }
    } else if (neighFollow.first != 0) {
        // XXX: Shouldn't the condition be extended by '&& (blocked & LCA_BLOCKED_BY_LEADER) != 0'? Refs. #2578
        // Otherwise we don't need to inform the follower but simply cut in

        // we are not blocked by the follower now, make sure it remains that way
        // XXX: Does the below code for the euler case really assure that? Refs. #2578
        SUMOReal vsafe, vsafe1;
        if (MSGlobals::gSemiImplicitEulerUpdate) {
            // euler
            MSVehicle* nv = neighFollow.first;
            vsafe1 = nv->getCarFollowModel().followSpeed(
                         nv, nv->getSpeed(), neighFollow.second + SPEED2DIST(plannedSpeed), plannedSpeed, myVehicle.getCarFollowModel().getMaxDecel());
            vsafe = nv->getCarFollowModel().followSpeed(
                        nv, nv->getSpeed(), neighFollow.second + SPEED2DIST(plannedSpeed - vsafe1), plannedSpeed, myVehicle.getCarFollowModel().getMaxDecel());
            // NOTE: since vsafe1 > nv->getSpeed() is possible, we don't have vsafe1 < vsafe < nv->getSpeed here (similar pattern above works differently)

        } else {
            // ballistic
            // XXX This should actually do for euler and ballistic cases (TODO: test!) Refs. #2575

            SUMOReal anticipationTime = 1.;
            SUMOReal anticipatedSpeed =  MIN2(myVehicle.getSpeed() + plannedAccel * anticipationTime, myVehicle.getMaxSpeedOnLane());
            SUMOReal anticipatedGap = myCarFollowModel.gapExtrapolation(anticipationTime, neighFollow.second, myVehicle.getSpeed(), nv->getSpeed(),
                                      plannedAccel, 0, myVehicle.getMaxSpeedOnLane(), nv->getMaxSpeedOnLane());
            SUMOReal secureGap = nv->getCarFollowModel().getSecureGap(nv->getSpeed(), anticipatedSpeed, myCarFollowModel.getMaxDecel());

            // propose follower speed corresponding to first estimation of gap
            SUMOReal vsafe = nv->getCarFollowModel().followSpeed(
                                 nv, nv->getSpeed(), anticipatedGap, plannedSpeed, myCarFollowModel.getMaxDecel());
            SUMOReal helpAccel = SPEED2ACCEL(vsafe - nv->getSpeed()) / anticipationTime;

            if (anticipatedGap > secureGap) {
                // follower may accelerate, implying vhelp >= vsafe >= nv->getSpeed()
                // calculate gap for the assumed acceleration
                anticipatedGap = myCarFollowModel.gapExtrapolation(anticipationTime, neighFollow.second, myVehicle.getSpeed(), nv->getSpeed(),
                                 plannedAccel, helpAccel, myVehicle.getMaxSpeedOnLane(), nv->getMaxSpeedOnLane());
                SUMOReal anticipatedHelpSpeed = MIN2(nv->getSpeed() + anticipationTime * helpAccel, nv->getMaxSpeedOnLane());
                secureGap = nv->getCarFollowModel().getSecureGap(anticipatedHelpSpeed, anticipatedSpeed, myCarFollowModel.getMaxDecel());
                if (anticipatedGap < secureGap) {
                    // don't accelerate
                    vsafe = nv->getSpeed();
                }
            } else {
                // follower is too fast, implying that vhelp <= vsafe <= nv->getSpeed()
                // we use the above vhelp
            }
        }
        msgPass.informNeighFollower(new Info(vsafe, dir), &myVehicle);

#ifdef DEBUG_INFORMER
        if (DEBUG_COND) {
            std::cout << " wants to cut in before non-blocking follower nv=" << nv->getID() << "\n";
        }
#endif
    }
}


void
MSLCM_LC2013::prepareStep() {
    // keep information about strategic change direction
    myOwnState = (myOwnState & LCA_STRATEGIC) ? (myOwnState & LCA_WANTS_LANECHANGE) : 0;
    myLeadingBlockerLength = 0;
    myLeftSpace = 0;
    myVSafes.clear();
    myDontBrake = false;
    // truncate to work around numerical instability between different builds
    mySpeedGainProbability = ceil(mySpeedGainProbability * 100000.0) * 0.00001;
    myKeepRightProbability = ceil(myKeepRightProbability * 100000.0) * 0.00001;
}


void
MSLCM_LC2013::changed() {
    myOwnState = 0;
    mySpeedGainProbability = 0;
    myKeepRightProbability = 0;
    if (myVehicle.getBestLaneOffset() == 0) {
        // if we are not yet on our best lane there might still be unseen blockers
        // (during patchSpeed)
        myLeadingBlockerLength = 0;
        myLeftSpace = 0;
    }
    myLookAheadSpeed = LOOK_AHEAD_MIN_SPEED;
    myVSafes.clear();
    myDontBrake = false;
}


int
MSLCM_LC2013::_wantsChange(
    int laneOffset,
    MSAbstractLaneChangeModel::MSLCMessager& msgPass,
    int blocked,
    const std::pair<MSVehicle*, SUMOReal>& leader,
    const std::pair<MSVehicle*, SUMOReal>& neighLead,
    const std::pair<MSVehicle*, SUMOReal>& neighFollow,
    const MSLane& neighLane,
    const std::vector<MSVehicle::LaneQ>& preb, // XXX: What does "preb" stand for? Please comment. (Leo) Refs. #2578, #2604
    MSVehicle** lastBlocked,
    MSVehicle** firstBlocked) {
    assert(laneOffset == 1 || laneOffset == -1);
    const SUMOTime currentTime = MSNet::getInstance()->getCurrentTimeStep();
    // compute bestLaneOffset
    MSVehicle::LaneQ curr, neigh, best;
    int bestLaneOffset = 0;
    // What do these "dists" mean? Please comment. (Leo) Ad: I now think the following:
    // currentDist is the distance that the vehicle can go on its route without having to
    // change lanes from the current lane. neighDist as currentDist for the considered target lane (i.e., neigh)
    // If this is true I suggest to put this into the docu of wantsChange()
    SUMOReal currentDist = 0;
    SUMOReal neighDist = 0;
    int currIdx = 0;
    MSLane* prebLane = myVehicle.getLane();
    if (prebLane->getEdge().getPurpose() == MSEdge::EDGEFUNCTION_INTERNAL) {
        // internal edges are not kept inside the bestLanes structure
        prebLane = prebLane->getLinkCont()[0]->getLane();
    }
    // XXX: What does the following code do? Please comment. (Leo) Refs. #2578
    const bool checkOpposite = &neighLane.getEdge() != &myVehicle.getLane()->getEdge();
    const int prebOffset = (checkOpposite ? 0 : laneOffset);
    for (int p = 0; p < (int) preb.size(); ++p) {
        if (preb[p].lane == prebLane && p + laneOffset >= 0) {
            assert(p + prebOffset < (int)preb.size());
            curr = preb[p];
            neigh = preb[p + prebOffset];
            currentDist = curr.length;
            neighDist = neigh.length;
            bestLaneOffset = curr.bestLaneOffset;
            if (bestLaneOffset == 0 && preb[p + prebOffset].bestLaneOffset == 0) {
#ifdef DEBUG_WANTS_CHANGE
                if (DEBUG_COND) {
                    std::cout << STEPS2TIME(currentTime)
                              << " veh=" << myVehicle.getID()
                              << " bestLaneOffsetOld=" << bestLaneOffset
                              << " bestLaneOffsetNew=" << laneOffset
                              << "\n";
                }
#endif
                bestLaneOffset = prebOffset;
            }
            best = preb[p + bestLaneOffset];
            currIdx = p;
            break;
        }
    }
    // direction specific constants
    const bool right = (laneOffset == -1);
    if (isOpposite() && right) {
        neigh = preb[preb.size() - 1];
        curr = neigh;
        best = neigh;
        bestLaneOffset = -1;
        curr.bestLaneOffset = -1;
        neighDist = neigh.length;
        currentDist = curr.length;
    }
    const SUMOReal posOnLane = isOpposite() ? myVehicle.getLane()->getOppositePos(myVehicle.getPositionOnLane()) : myVehicle.getPositionOnLane();
    const int lca = (right ? LCA_RIGHT : LCA_LEFT);
    const int myLca = (right ? LCA_MRIGHT : LCA_MLEFT);
    const int lcaCounter = (right ? LCA_LEFT : LCA_RIGHT);
    const bool changeToBest = (right && bestLaneOffset < 0) || (!right && bestLaneOffset > 0);
    // keep information about being a leader/follower
    int ret = (myOwnState & 0xffff0000);
    int req = 0; // the request to change or stay

    ret = slowDownForBlocked(lastBlocked, ret);
    if (lastBlocked != firstBlocked) {
        ret = slowDownForBlocked(firstBlocked, ret);
    }

#ifdef DEBUG_WANTS_CHANGE
    if (DEBUG_COND) {
        std::cout << SIMTIME
                  << " veh=" << myVehicle.getID()
                  << " _wantsChange state=" << myOwnState
                  << " myVSafes=" << toString(myVSafes)
                  << " firstBlocked=" << Named::getIDSecure(*firstBlocked)
                  << " lastBlocked=" << Named::getIDSecure(*lastBlocked)
                  << " leader=" << Named::getIDSecure(leader.first)
                  << " leaderGap=" << leader.second
                  << " neighLead=" << Named::getIDSecure(neighLead.first)
                  << " neighLeadGap=" << neighLead.second
                  << " neighFollow=" << Named::getIDSecure(neighFollow.first)
                  << " neighFollowGap=" << neighFollow.second
                  << "\n";
    }
#endif

    // we try to estimate the distance which is necessary to get on a lane
    //  we have to get on in order to keep our route
    // we assume we need something that depends on our velocity
    // and compare this with the free space on our wished lane
    //
    // if the free space is somehow(<-?) less than the space we need, we should
    //  definitely try to get to the desired lane
    //
    // this rule forces our vehicle to change the lane if a lane changing is necessary soon


    // we do not want the lookahead distance to change all the time so we let it decay slowly
    // (in contrast, growth is applied instantaneously)
    if (myVehicle.getSpeed() > myLookAheadSpeed) {
        myLookAheadSpeed = myVehicle.getSpeed();
    } else {
        // memory decay factor for this time step
        const SUMOReal memoryFactor = 1. - (1. - LOOK_AHEAD_SPEED_MEMORY) * TS;
        assert(memoryFactor > 0.);
        myLookAheadSpeed = MAX2(LOOK_AHEAD_MIN_SPEED,
                                (memoryFactor * myLookAheadSpeed + (1 - memoryFactor) * myVehicle.getSpeed()));
    }
    SUMOReal laDist = myLookAheadSpeed * (right ? LOOK_FORWARD_RIGHT : LOOK_FORWARD_LEFT) * myStrategicParam;
    laDist += myVehicle.getVehicleType().getLengthWithGap() * (SUMOReal) 2.;


    if (bestLaneOffset == 0 && leader.first != 0 && leader.first->isStopped()) {
        // react to a stopped leader on the current lane
        // The value of laDist is doubled below for the check whether the lc-maneuver can be taken out
        // on the remaining distance (because the vehicle has to change back and forth). Therefore multiply with 0.5.
        laDist = 0.5 * (myVehicle.getVehicleType().getLengthWithGap()
                        + leader.first->getVehicleType().getLengthWithGap());
    } else if (bestLaneOffset == laneOffset && neighLead.first != 0 && neighLead.first->isStopped()) {
        // react to a stopped leader on the target lane (if it is the bestLane)
        laDist = myVehicle.getVehicleType().getLengthWithGap()
                 + neighLead.first->getVehicleType().getLengthWithGap();
    }

    // free space that is available for changing
    //const SUMOReal neighSpeed = (neighLead.first != 0 ? neighLead.first->getSpeed() :
    //        neighFollow.first != 0 ? neighFollow.first->getSpeed() :
    //        best.lane->getSpeedLimit());
    // @note: while this lets vehicles change earlier into the correct direction
    // it also makes the vehicles more "selfish" and prevents changes which are necessary to help others



    // Next we assign to roundabout edges a larger distance than to normal edges
    // in order to decrease sense of lc urgency and induce higher usage of inner roundabout lanes.
    // 1) get information about the next upcoming roundabout
    SUMOReal roundaboutDistanceAhead = 0;
    SUMOReal roundaboutDistanceAheadNeigh = 0;
    int roundaboutEdgesAhead = 0;
    int roundaboutEdgesAheadNeigh = 0;
    if (!isOpposite()) {
        getRoundaboutAheadInfo(this, curr, neigh, roundaboutDistanceAhead, roundaboutDistanceAheadNeigh, roundaboutEdgesAhead, roundaboutEdgesAheadNeigh);
    }
    // 2) add a distance bonus for roundabout edges
    currentDist += roundaboutDistBonus(roundaboutDistanceAhead, roundaboutEdgesAhead);
    neighDist += roundaboutDistBonus(roundaboutDistanceAheadNeigh, roundaboutEdgesAheadNeigh);

#ifdef DEBUG_WANTS_CHANGE
    if (DEBUG_COND) {
        if (roundaboutEdgesAhead > 0) {
            std::cout << " roundaboutEdgesAhead=" << roundaboutEdgesAhead << " roundaboutEdgesAheadNeigh=" << roundaboutEdgesAheadNeigh << "\n";
//            std::cout << " roundaboutDistanceAhead=" << roundaboutDistanceAhead << " roundaboutDistanceAheadNeigh=" << roundaboutDistanceAheadNeigh << "\n";
        }
    }
#endif

    const SUMOReal usableDist = (currentDist - posOnLane - best.occupation *  JAM_FACTOR);
    //- (best.lane->getVehicleNumber() * neighSpeed)); // VARIANT 9 jfSpeed
    const SUMOReal maxJam = MAX2(preb[currIdx + prebOffset].occupation, preb[currIdx].occupation);
    const SUMOReal neighLeftPlace = MAX2((SUMOReal) 0, neighDist - posOnLane - maxJam);

#ifdef DEBUG_WANTS_CHANGE
    if (DEBUG_COND) {
        std::cout << STEPS2TIME(currentTime)
                  << " veh=" << myVehicle.getID()
                  << " laSpeed=" << myLookAheadSpeed
                  << " laDist=" << laDist
                  << " currentDist=" << currentDist
                  << " usableDist=" << usableDist
                  << " bestLaneOffset=" << bestLaneOffset
                  << " best.occupation=" << best.occupation
                  << " best.length=" << best.length
                  << " maxJam=" << maxJam
                  << " neighLeftPlace=" << neighLeftPlace
                  << "\n";
    }
#endif

    if (changeToBest && bestLaneOffset == curr.bestLaneOffset
            && currentDistDisallows(usableDist, bestLaneOffset, laDist)) {
        /// @brief we urgently need to change lanes to follow our route
        ret = ret | lca | LCA_STRATEGIC | LCA_URGENT;
    } else {
        // VARIANT_20 (noOvertakeRight)
        if (!myAllowOvertakingRight && !right && !myVehicle.congested() && neighLead.first != 0) {
            // check for slower leader on the left. we should not overtake but
            // rather move left ourselves (unless congested)
            MSVehicle* nv = neighLead.first;
            if (nv->getSpeed() < myVehicle.getSpeed()) {
                const SUMOReal vSafe = myCarFollowModel.followSpeed(
                                           &myVehicle, myVehicle.getSpeed(), neighLead.second, nv->getSpeed(), nv->getCarFollowModel().getMaxDecel());
                myVSafes.push_back(vSafe);
                if (vSafe < myVehicle.getSpeed()) {
                    mySpeedGainProbability += TS * myChangeProbThresholdLeft / 3;
                }
#ifdef DEBUG_WANTS_CHANGE
                if (DEBUG_COND) {
                    std::cout << STEPS2TIME(currentTime)
                              << " avoid overtaking on the right nv=" << nv->getID()
                              << " nvSpeed=" << nv->getSpeed()
                              << " mySpeedGainProbability=" << mySpeedGainProbability
                              << " plannedSpeed=" << myVSafes.back()
                              << "\n";
                }
#endif
            }
        }

        if (!changeToBest && (currentDistDisallows(neighLeftPlace, abs(bestLaneOffset) + 2, laDist))) {
            // the opposite lane-changing direction should be done than the one examined herein
            //  we'll check whether we assume we could change anyhow and get back in time...
            //
            // this rule prevents the vehicle from moving in opposite direction of the best lane
            //  unless the way till the end where the vehicle has to be on the best lane
            //  is long enough
#ifdef DEBUG_WANTS_CHANGE
            if (DEBUG_COND) {
                std::cout << " veh=" << myVehicle.getID() << " could not change back and forth in time (1) neighLeftPlace=" << neighLeftPlace << "\n";
            }
#endif
            ret = ret | LCA_STAY | LCA_STRATEGIC;
        } else if (bestLaneOffset == 0 && (neighLeftPlace * 2. < laDist)) {
            // the current lane is the best and a lane-changing would cause a situation
            //  of which we assume we will not be able to return to the lane we have to be on.
            // this rule prevents the vehicle from leaving the current, best lane when it is
            //  close to this lane's end
#ifdef DEBUG_WANTS_CHANGE
            if (DEBUG_COND) {
                std::cout << " veh=" << myVehicle.getID() << " could not change back and forth in time (2) neighLeftPlace=" << neighLeftPlace << "\n";
            }
#endif
            ret = ret | LCA_STAY | LCA_STRATEGIC;
        } else if (bestLaneOffset == 0
                   && (leader.first == 0 || !leader.first->isStopped())
                   && neigh.bestContinuations.back()->getLinkCont().size() != 0
                   && roundaboutEdgesAhead == 0
                   && neighDist < TURN_LANE_DIST) {
            // VARIANT_21 (stayOnBest)
            // we do not want to leave the best lane for a lane which leads elsewhere
            // unless our leader is stopped or we are approaching a roundabout
#ifdef DEBUG_WANTS_CHANGE
            if (DEBUG_COND) {
                std::cout << " veh=" << myVehicle.getID() << " does not want to leave the bestLane (neighDist=" << neighDist << ")\n";
            }
#endif
            ret = ret | LCA_STAY | LCA_STRATEGIC;
        }
    }
    // check for overriding TraCI requests
#ifdef DEBUG_WANTS_CHANGE
    if (DEBUG_COND) {
        std::cout << STEPS2TIME(currentTime) << " veh=" << myVehicle.getID() << " ret=" << ret;
    }
#endif
    ret = myVehicle.influenceChangeDecision(ret);
    if ((ret & lcaCounter) != 0) {
        // we are not interested in traci requests for the opposite direction here
        ret &= ~(LCA_TRACI | lcaCounter | LCA_URGENT);
    }
#ifdef DEBUG_WANTS_CHANGE
    if (DEBUG_COND) {
        std::cout << " retAfterInfluence=" << ret << "\n";
    }
#endif

    if ((ret & LCA_STAY) != 0) {
        return ret;
    }
    if ((ret & LCA_URGENT) != 0) {
        // prepare urgent lane change maneuver
        // save the left space
        myLeftSpace = currentDist - posOnLane;
        if (changeToBest && abs(bestLaneOffset) > 1) {
            // there might be a vehicle which needs to counter-lane-change one lane further and we cannot see it yet
#ifdef DEBUG_WANTS_CHANGE
            if (DEBUG_COND) {
                std::cout << "  reserving space for unseen blockers\n";
            }
#endif
            myLeadingBlockerLength = MAX2((SUMOReal)(right ? 20.0 : 40.0), myLeadingBlockerLength);
        }

        // letting vehicles merge in at the end of the lane in case of counter-lane change, step#1
        //   if there is a leader and he wants to change to the opposite direction
        saveBlockerLength(neighLead.first, lcaCounter);
        if (*firstBlocked != neighLead.first) {
            saveBlockerLength(*firstBlocked, lcaCounter);
        }

        const SUMOReal remainingSeconds = ((ret & LCA_TRACI) == 0 ?
                                           // MAX2((SUMOReal)STEPS2TIME(TS), (myLeftSpace-myLeadingBlockerLength) / MAX2(myLookAheadSpeed, NUMERICAL_EPS) / abs(bestLaneOffset) / URGENCY) :
                                           MAX2((SUMOReal)STEPS2TIME(TS), myLeftSpace / MAX2(myLookAheadSpeed, NUMERICAL_EPS) / abs(bestLaneOffset) / URGENCY) :
                                           myVehicle.getInfluencer().changeRequestRemainingSeconds(currentTime));
        const SUMOReal plannedSpeed = informLeader(msgPass, blocked, myLca, neighLead, remainingSeconds);
        // NOTE: for the  ballistic update case negative speeds may indicate a stop request,
        //       while informLeader returns -1 in that case. Refs. #2577
        if (plannedSpeed >= 0 || (!MSGlobals::gSemiImplicitEulerUpdate && plannedSpeed != -1)) {
            // maybe we need to deal with a blocking follower
            informFollower(msgPass, blocked, myLca, neighFollow, remainingSeconds, plannedSpeed);
        }

#ifdef DEBUG_WANTS_CHANGE
        if (DEBUG_COND) {
            std::cout << STEPS2TIME(currentTime)
                      << " veh=" << myVehicle.getID()
                      << " myLeftSpace=" << myLeftSpace
                      << " remainingSeconds=" << remainingSeconds
                      << " plannedSpeed=" << plannedSpeed
                      << "\n";
        }
#endif

        return ret;
    }
    // a high inconvenience prevents cooperative changes.
    const SUMOReal inconvenience = MIN2((SUMOReal)1.0, (laneOffset < 0
                                        ? mySpeedGainProbability / myChangeProbThresholdRight
                                        : -mySpeedGainProbability / myChangeProbThresholdLeft));
    const bool speedGainInconvenient = inconvenience > myCooperativeParam;
    const bool neighOccupancyInconvenient = neigh.lane->getBruttoOccupancy() > curr.lane->getBruttoOccupancy();

    // VARIANT_15
    if (roundaboutEdgesAhead > 1) {

#ifdef DEBUG_WANTS_CHANGE
        if (DEBUG_COND) {
            std::cout << STEPS2TIME(currentTime)
                      << " veh=" << myVehicle.getID()
                      << " roundaboutEdgesAhead=" << roundaboutEdgesAhead
                      << " myLeftSpace=" << myLeftSpace
                      << "\n";
        }
#endif
        // try to use the inner lanes of a roundabout to increase throughput
        // unless we are approaching the exit
        if (lca == LCA_LEFT) {
            // if inconvenience is not too high, request collaborative change (currently only for ballistic update)
            // TODO: test this for euler update! Refs. #2575
            if (MSGlobals::gSemiImplicitEulerUpdate || !neighOccupancyInconvenient) {
//                if(MSGlobals::gSemiImplicitEulerUpdate || !speedGainInconvenient){
                req = ret | lca | LCA_COOPERATIVE;
            }
        } else {
            // if inconvenience is not too high, request collaborative change (currently only for ballistic update)
            if (MSGlobals::gSemiImplicitEulerUpdate || neighOccupancyInconvenient) {
//            if(MSGlobals::gSemiImplicitEulerUpdate || speedGainInconvenient){
                req = ret | LCA_STAY | LCA_COOPERATIVE;
            }
        }
        if (!cancelRequest(req)) {
            return ret | req;
        }
    }

    // let's also regard the case where the vehicle is driving on a highway...
    //  in this case, we do not want to get to the dead-end of an on-ramp
    if (right) {
        if (bestLaneOffset == 0 && myVehicle.getLane()->getSpeedLimit() > 80. / 3.6 && myLookAheadSpeed > SUMO_const_haltingSpeed) {
#ifdef DEBUG_WANTS_CHANGE
            if (DEBUG_COND) {
                std::cout << " veh=" << myVehicle.getID() << " does not want to get stranded on the on-ramp of a highway\n";
            }
#endif
            req = ret | LCA_STAY | LCA_STRATEGIC;
            if (!cancelRequest(req)) {
                return ret | req;
            }
        }
    }
    // --------

    // -------- make place on current lane if blocking follower
    //if (amBlockingFollowerPlusNB()) {
    //    std::cout << myVehicle.getID() << ", " << currentDistAllows(neighDist, bestLaneOffset, laDist)
    //        << " neighDist=" << neighDist
    //        << " currentDist=" << currentDist
    //        << "\n";
    //}

    if (amBlockingFollowerPlusNB()
            && (!speedGainInconvenient)
            //&& ((myOwnState & lcaCounter) == 0) // VARIANT_6 : counterNoHelp
            && (changeToBest || currentDistAllows(neighDist, abs(bestLaneOffset) + 1, laDist))) {

        // VARIANT_2 (nbWhenChangingToHelp)
#ifdef DEBUG_WANTS_CHANGE
        if (DEBUG_COND) {
            std::cout << STEPS2TIME(currentTime)
                      << " veh=" << myVehicle.getID()
                      << " wantsChangeToHelp=" << (right ? "right" : "left")
                      << " state=" << myOwnState
                      // << (((myOwnState & lcaCounter) != 0) ? " (counter)" : "")
                      << "\n";
        }
#endif
        req = ret | lca | LCA_COOPERATIVE | LCA_URGENT ;//| LCA_CHANGE_TO_HELP;
        if (!cancelRequest(req)) {
            return ret | req;
        }
    }

    // --------


    //// -------- security checks for krauss
    ////  (vsafe fails when gap<0)
    //if ((blocked & LCA_BLOCKED) != 0) {
    //    return ret;
    //}
    //// --------

    // -------- higher speed
    //if ((congested(neighLead.first) && neighLead.second < 20) || predInteraction(leader.first)) { //!!!
    //    return ret;
    //}

    SUMOReal thisLaneVSafe = myVehicle.getLane()->getVehicleMaxSpeed(&myVehicle);
    SUMOReal neighLaneVSafe = neighLane.getVehicleMaxSpeed(&myVehicle);

    /* First attempt to fix #2126
     * should rather be considering anticipated speeds further in the future, maybe combined with maximal speed as tried here
    if (neighLead.first == 0) {
        neighLaneVSafe = MIN2(neighLaneVSafe, myCarFollowModel.maximumSafeStopSpeed(neighDist, myVehicle.getSpeed(),true));
    } else {
        // @todo: what if leader is below safe gap?!!!
        neighLaneVSafe = MIN2(neighLaneVSafe, myCarFollowModel.maximumSafeFollowSpeed(neighLead.second, myVehicle.getSpeed(),
                neighLead.first->getSpeed(),neighLead.first->getCarFollowModel().getMaxDecel(),true));
    }
    if (leader.first == 0) {
        thisLaneVSafe = MIN2(thisLaneVSafe, myCarFollowModel.maximumSafeStopSpeed(currentDist, myVehicle.getSpeed(),true));
    } else {
        // @todo: what if leader is below safe gap?!!!
        thisLaneVSafe = MIN2(thisLaneVSafe, myCarFollowModel.maximumSafeFollowSpeed(leader.second, myVehicle.getSpeed(),
                leader.first->getSpeed(),leader.first->getCarFollowModel().getMaxDecel(),true));
    }
    */

    const SUMOReal correctedSpeed = (myVehicle.getSpeed() + myVehicle.getCarFollowModel().getMaxAccel()
                                     - ACCEL2SPEED(myVehicle.getCarFollowModel().getMaxAccel()));
    if (neighLead.first == 0) {
        neighLaneVSafe = MIN2(neighLaneVSafe, myCarFollowModel.followSpeed(&myVehicle, correctedSpeed, neighDist, 0, 0));
    } else {
        // @todo: what if leader is below safe gap?!!!
        neighLaneVSafe = MIN2(neighLaneVSafe, myCarFollowModel.followSpeed(
                                  &myVehicle, correctedSpeed, neighLead.second, neighLead.first->getSpeed(), neighLead.first->getCarFollowModel().getMaxDecel()));
    }
    if (leader.first == 0) {
        thisLaneVSafe = MIN2(thisLaneVSafe, myCarFollowModel.followSpeed(&myVehicle, correctedSpeed, currentDist, 0, 0));
    } else {
        // @todo: what if leader is below safe gap?!!!
        thisLaneVSafe = MIN2(thisLaneVSafe, myCarFollowModel.followSpeed(
                                 &myVehicle, correctedSpeed, leader.second, leader.first->getSpeed(), leader.first->getCarFollowModel().getMaxDecel()));
    }

    const SUMOReal vMax = MIN2(myVehicle.getVehicleType().getMaxSpeed(), myVehicle.getLane()->getVehicleMaxSpeed(&myVehicle));
    thisLaneVSafe = MIN2(thisLaneVSafe, vMax);
    neighLaneVSafe = MIN2(neighLaneVSafe, vMax);
    const SUMOReal relativeGain = (neighLaneVSafe - thisLaneVSafe) / MAX2(neighLaneVSafe,
                                  RELGAIN_NORMALIZATION_MIN_SPEED);

//    // maybe this (acceleration-difference) would be a good quantity to take into account for lc-considerations..., refs. #2126
//    const SUMOReal accelDifference = ACCEL2SPEED(neighLaneVSafe - thisLaneVSafe);


#ifdef DEBUG_WANTS_CHANGE
    if (DEBUG_COND) {
        std::cout << STEPS2TIME(currentTime)
                  << " veh=" << myVehicle.getID()
                  << " currentDist=" << currentDist
                  << " neighDist=" << neighDist
//                  << "\n thisLaneVSafe=" << toString(thisLaneVSafe,24)
//                  << "\nneighLaneVSafe=" << toString(neighLaneVSafe,24)
//                  << "\nrelativeGain=" << toString(relativeGain,24)
                  << "\n";
    }
#endif

    if (right) {
        // ONLY FOR CHANGING TO THE RIGHT
        if (thisLaneVSafe - 5 / 3.6 > neighLaneVSafe) {
            // ok, the current lane is faster than the right one...
            if (mySpeedGainProbability < 0) {
                mySpeedGainProbability *= pow(0.5, TS);
                //myKeepRightProbability /= 2.0;
            }
        } else {
            // ok, the current lane is not (much) faster than the right one
            // @todo recheck the 5 km/h discount on thisLaneVSafe, refs. #2068

            // do not promote changing to the left just because changing to the right is bad
            // XXX: The following code may promote it, though!? (recheck!)
            //      (Think of small negative mySpeedGainProbability and larger negative relativeGain)
            //      One might think of replacing '||' by '&&' to exclude that possibility...
            //      Still, for negative relativeGain, we might want to decrease the inclination for
            //      changing to the left. Another solution could be the seperation of mySpeedGainProbability into
            //      two variables (one for left and one for right). Refs #2578
            if (mySpeedGainProbability < 0 || relativeGain > 0) {
                mySpeedGainProbability -= TS * relativeGain;
            }

            // honor the obligation to keep right (Rechtsfahrgebot)
            // XXX consider fast approaching followers on the current lane. Refs #2578
            //const SUMOReal vMax = myLookAheadSpeed;
            const SUMOReal acceptanceTime = KEEP_RIGHT_ACCEPTANCE * vMax * MAX2((SUMOReal)1, myVehicle.getSpeed()) / myVehicle.getLane()->getSpeedLimit();
            SUMOReal fullSpeedGap = MAX2((SUMOReal)0, neighDist - myVehicle.getCarFollowModel().brakeGap(vMax));
            SUMOReal fullSpeedDrivingSeconds = MIN2(acceptanceTime, fullSpeedGap / vMax);
            if (neighLead.first != 0 && neighLead.first->getSpeed() < vMax) {
                fullSpeedGap = MAX2((SUMOReal)0, MIN2(fullSpeedGap,
                                                      neighLead.second - myVehicle.getCarFollowModel().getSecureGap(
                                                              vMax, neighLead.first->getSpeed(), neighLead.first->getCarFollowModel().getMaxDecel())));
                fullSpeedDrivingSeconds = MIN2(fullSpeedDrivingSeconds, fullSpeedGap / (vMax - neighLead.first->getSpeed()));
            }
            const SUMOReal deltaProb = (myChangeProbThresholdRight
                                        * STEPS2TIME(DELTA_T)
                                        * (fullSpeedDrivingSeconds / acceptanceTime) / KEEP_RIGHT_TIME);
            myKeepRightProbability -= TS * deltaProb;

#ifdef DEBUG_WANTS_CHANGE
            if (DEBUG_COND) {
                std::cout << STEPS2TIME(currentTime)
                          << " veh=" << myVehicle.getID()
                          << " vMax=" << vMax
                          << " neighDist=" << neighDist
                          << " brakeGap=" << myVehicle.getCarFollowModel().brakeGap(myVehicle.getSpeed())
                          << " leaderSpeed=" << (neighLead.first == 0 ? -1 : neighLead.first->getSpeed())
                          << " secGap=" << (neighLead.first == 0 ? -1 : myVehicle.getCarFollowModel().getSecureGap(
                                                myVehicle.getSpeed(), neighLead.first->getSpeed(), neighLead.first->getCarFollowModel().getMaxDecel()))
                          << " acceptanceTime=" << acceptanceTime
                          << " fullSpeedGap=" << fullSpeedGap
                          << " fullSpeedDrivingSeconds=" << fullSpeedDrivingSeconds
                          << " dProb=" << deltaProb
                          << " myKeepRightProbability=" << myKeepRightProbability
                          << "\n";
            }
#endif
            if (myKeepRightProbability * myKeepRightParam < -myChangeProbThresholdRight) {
                req = ret | lca | LCA_KEEPRIGHT;
                if (!cancelRequest(req)) {
                    return ret | req;
                }
            }
        }

#ifdef DEBUG_WANTS_CHANGE
        if (DEBUG_COND) {
            std::cout << STEPS2TIME(currentTime)
                      << " veh=" << myVehicle.getID()
                      << " speed=" << myVehicle.getSpeed()
                      << " mySpeedGainProbability=" << mySpeedGainProbability
                      << " thisLaneVSafe=" << thisLaneVSafe
                      << " neighLaneVSafe=" << neighLaneVSafe
                      << " relativeGain=" << relativeGain
                      << " blocked=" << blocked
                      << "\n";
        }
#endif

        if (mySpeedGainProbability < -myChangeProbThresholdRight
                && neighDist / MAX2((SUMOReal) .1, myVehicle.getSpeed()) > 20.) { //./MAX2((SUMOReal) .1, myVehicle.getSpeed())) { // -.1
            req = ret | lca | LCA_SPEEDGAIN;
            if (!cancelRequest(req)) {
                return ret | req;
            }
        }
    } else {
        // ONLY FOR CHANGING TO THE LEFT
        if (thisLaneVSafe > neighLaneVSafe) {
            // this lane is better
            if (mySpeedGainProbability > 0) {
                mySpeedGainProbability *= pow(0.5, TS);
            }
        } else {
            // left lane is better
            mySpeedGainProbability += TS * relativeGain;
        }
        // VARIANT_19 (stayRight)
        //if (neighFollow.first != 0) {
        //    MSVehicle* nv = neighFollow.first;
        //    const SUMOReal secGap = nv->getCarFollowModel().getSecureGap(nv->getSpeed(), myVehicle.getSpeed(), myVehicle.getCarFollowModel().getMaxDecel());
        //    if (neighFollow.second < secGap * KEEP_RIGHT_HEADWAY) {
        //        // do not change left if it would inconvenience faster followers
        //        return ret | LCA_STAY | LCA_SPEEDGAIN;
        //    }
        //}

#ifdef DEBUG_WANTS_CHANGE
        if (DEBUG_COND) {
            std::cout << STEPS2TIME(currentTime)
                      << " veh=" << myVehicle.getID()
                      << " speed=" << myVehicle.getSpeed()
                      << " mySpeedGainProbability=" << mySpeedGainProbability
                      << " thisLaneVSafe=" << thisLaneVSafe
                      << " neighLaneVSafe=" << neighLaneVSafe
                      << " relativeGain=" << relativeGain
                      << " blocked=" << blocked
                      << "\n";
        }
#endif

        if (mySpeedGainProbability > myChangeProbThresholdLeft && neighDist / MAX2((SUMOReal) .1, myVehicle.getSpeed()) > 20.) { // .1
            req = ret | lca | LCA_SPEEDGAIN;
            if (!cancelRequest(req)) {
                return ret | req;
            }
        }
    }
    // --------
    if (changeToBest && bestLaneOffset == curr.bestLaneOffset
            && (right ? mySpeedGainProbability < 0 : mySpeedGainProbability > 0)) {
        // change towards the correct lane, speedwise it does not hurt
        req = ret | lca | LCA_STRATEGIC;
        if (!cancelRequest(req)) {
            return ret | req;
        }
    }
#ifdef DEBUG_WANTS_CHANGE
    if (DEBUG_COND) {
        std::cout << STEPS2TIME(currentTime)
                  << " veh=" << myVehicle.getID()
                  << " mySpeedGainProbability=" << mySpeedGainProbability
                  << " myKeepRightProbability=" << myKeepRightProbability
                  << " thisLaneVSafe=" << thisLaneVSafe
                  << " neighLaneVSafe=" << neighLaneVSafe
                  << "\n";
    }
#endif

    return ret;
}


void
MSLCM_LC2013::getRoundaboutAheadInfo(const MSLCM_LC2013* lcm, const MSVehicle::LaneQ& curr, const MSVehicle::LaneQ& neigh,
                                     SUMOReal& roundaboutDistanceAhead, SUMOReal& roundaboutDistanceAheadNeigh, int& roundaboutEdgesAhead, int& roundaboutEdgesAheadNeigh) {

    const MSVehicle& veh = lcm->myVehicle;

    // In what follows, we check whether a roundabout is ahead (or the vehicle is on a roundabout)
    // We calculate the lengths of the continuations described by curr and neigh,
    // which are part of the roundabout. Currently only takes effect for ballistic update, refs #1807, #2576 (Leo)
    SUMOReal pos = lcm->isOpposite() ? veh.getLane()->getLength() - veh.getPositionOnLane() : veh.getPositionOnLane();
    roundaboutDistanceAhead = distanceAlongNextRoundabout(pos, veh.getLane(), curr.bestContinuations);

    // For the distance on the neigh.lane, we need to do a little hack since we may not
    // have access to the right initial lane (neigh.lane is only the first non-null lane of neigh.bestContinuations).
    roundaboutDistanceAheadNeigh = 0;
    SUMOReal neighPosition = pos;
#ifdef HAVE_INTERNAL_LANES
    if (veh.getLane()->getEdge().getPurpose() == MSEdge::EDGEFUNCTION_INTERNAL) {
        // take care of the distance on internal lanes
        neighPosition = 0.;
        if (veh.getLane()->getEdge().isRoundabout()) {
            // vehicle is on internal roundabout lane -> neigh.lane is not a parallel internal lane, but the next non-internal lane
            // add remaining length on current, internal lane to roundabout distance on neigh continuation
            roundaboutDistanceAheadNeigh = veh.getLane()->getLength() - veh.getPositionOnLane();
            MSLane* nextLane = 0;
            for (std::vector<MSLane*>::const_iterator i = curr.bestContinuations.begin(); i != curr.bestContinuations.end(); i++) {
                if (*i != 0 && *i != veh.getLane()) {
                    nextLane = *i;
                    break;
                }
            }
            assert(nextLane != 0);
            // add all lengths remaining internal lanes of current continuations until nextLane
            const MSLink* link = MSLinkContHelper::getConnectingLink(*veh.getLane(), *nextLane);
            roundaboutDistanceAheadNeigh += link->getInternalLengthsAfter();
        }
    }
#endif
    // add roundabout distance from neigh.lane on
    roundaboutDistanceAheadNeigh += distanceAlongNextRoundabout(neighPosition, neigh.lane, neigh.bestContinuations);

#ifdef DEBUG_WANTS_CHANGE
    if (lcm->debugVehicle()) {
        std::cout << "roundaboutDistanceAhead = " << roundaboutDistanceAhead
                  << " roundaboutDistanceAheadNeigh = " << roundaboutDistanceAheadNeigh
                  << "\n";
    }
#endif

    // count the number of roundabout edges ahead to determine whether
    // special LC behavior is required (promoting the use of the inner lane, mainly)
    roundaboutEdgesAhead = 0;
    for (std::vector<MSLane*>::const_iterator it = curr.bestContinuations.begin(); it != curr.bestContinuations.end(); ++it) {
        const MSLane* lane = *it;
        if (lane != 0 && lane->getEdge().isRoundabout()) {
            roundaboutEdgesAhead += 1;
        } else if (roundaboutEdgesAhead > 0) {
            // only check the next roundabout
            break;
        }
    }
    roundaboutEdgesAheadNeigh = 0;
    for (std::vector<MSLane*>::const_iterator it = neigh.bestContinuations.begin(); it != neigh.bestContinuations.end(); ++it) {
        if ((*it) != 0 && (*it)->getEdge().isRoundabout()) {
            roundaboutEdgesAheadNeigh += 1;
        } else if (roundaboutEdgesAheadNeigh > 0) {
            // only check the next roundabout
            break;
        }
    }
    return;
}


SUMOReal
MSLCM_LC2013::roundaboutDistBonus(SUMOReal roundaboutDistAhead, int roundaboutEdgesAhead) const {
    // NOTE: Currently there are two variants, one taking into account only the number
    //       of upcoming non-internal roundabout edges and adding ROUNDABOUT_DIST_BONUS per upcoming edge except the first.
    //       Another variant uses the actual distance and multiplies it by a factor ROUNDABOUT_DIST_FACTOR.
    //       Currently, the update rule decides which variant to take (because the second was experimentally implemented
    //       in the ballistic branch (ticket860)). Both variants may be combined in future. Refs. #2576
    if (MSGlobals::gSemiImplicitEulerUpdate) {
        if (roundaboutEdgesAhead > 1) {
            // Here we add a bonus length for each upcoming roundabout edge to the distance.
            // XXX: That becomes problematic, if the vehicle enters the last round about edge,
            // realizes suddenly that the change is very urgent and finds itself with very
            // few space to complete the urgent strategic change frequently leading to
            // a hang up on the inner lane.
            return roundaboutEdgesAhead * ROUNDABOUT_DIST_BONUS * myCooperativeParam;
        } else {
            return 0.;
        }
    } else {
        // This weighs the roundabout edges' distance with a larger factor
        if (roundaboutDistAhead > ROUNDABOUT_DIST_TRESH) {
            return (roundaboutDistAhead - ROUNDABOUT_DIST_TRESH) * (ROUNDABOUT_DIST_FACTOR - 1.);
        } else {
            return 0.;
        }
    }
}


SUMOReal
MSLCM_LC2013::distanceAlongNextRoundabout(SUMOReal position, const MSLane* initialLane, const std::vector<MSLane*>& continuationLanes) {
    for (std::vector<MSLane*>::const_iterator i = continuationLanes.begin(); i != continuationLanes.end(); i++) {
        assert((*i) == 0 || (*i)->getEdge().getPurpose() != MSEdge::EDGEFUNCTION_INTERNAL);
    }

    // We start with the current edge.
    bool encounteredRoundabout = false;
    SUMOReal roundaboutDistanceAhead = 0.;

    // set an iterator to the first non-zero entry of continuationLanes
    std::vector<MSLane*>::const_iterator j = continuationLanes.begin();
    while (j != continuationLanes.end() && *j == 0) {
        ++j;
    }

    // differentiate possible situations
    if (j == continuationLanes.end()) {
        // continuations end here
        assert(initialLane == 0);
        return 0.;
    } else if (initialLane == 0) {
        // NOTE: this may occur when calling distanceAlongNextRoundabout() with neigh.lane in _wantsChange().
        // In that case, the possible internal lengths have been taken into account in _wantsChange().
        // Thus we set initialLane to the first non-zero lane in continuationLanes
        initialLane = *j;
    } else if (!initialLane->isInternal() && initialLane != *j) {
        // this may occur during opposite direction driving where the initial Lane
        // is the reverse lane of *j. This should not happen on a roundabout! Therefore we can skip initialLane.
        assert(!initialLane->getEdge().isRoundabout());
        initialLane = *j;
    } else if (initialLane->getEdge().isRoundabout()) {
        // initial lane is on roundabout
        assert(position >= 0. && position <= initialLane->getLength());
        if (!initialLane->isInternal()) {
            assert(initialLane == *j);
            roundaboutDistanceAhead += initialLane->getLength() - position;
            if (j + 1 == continuationLanes.end() || *(j + 1) == 0 || !(*(j + 1))->getEdge().isRoundabout()) {
                // following connection is not part of the roundabout
            } else {
                // add internal lengths
                const MSLane* nextLane = *(j + 1);
                const MSLink* link = MSLinkContHelper::getConnectingLink(*initialLane, *nextLane);
                assert(link != 0);
                roundaboutDistanceAhead += link->getInternalLengthsAfter();
            }
            j++;
        } else {
            // initialLane is an internal roundabout lane -> add length to roundaboutDistanceAhead
            roundaboutDistanceAhead += initialLane->getLength() - position;
            assert(initialLane->getLinkCont().size() == 1);
            roundaboutDistanceAhead += initialLane->getLinkCont()[0]->getInternalLengthsAfter();
        }
    }

    // treat lanes beyond the initial one
    for (std::vector<MSLane*>::const_iterator it = j; it != continuationLanes.end(); ++it) {
        const MSLane* lane = *it;
        assert(lane != 0); // possible leading NULL lanes in continuationLanes were treated above
        if (lane->getEdge().isRoundabout()) {
            encounteredRoundabout = true;
            // add roundabout lane length
            roundaboutDistanceAhead += lane->getLength();

            // since bestContinuations contains no internal edges
            // add consecutive connection lengths if it is part of the route and the
            // consecutive edge is on the roundabout as well
            if (it + 1 != continuationLanes.end() && *(it + 1) != 0 && (*(it + 1))->getEdge().isRoundabout()) {
                // find corresponding link for the current lane
                const MSLink* link = MSLinkContHelper::getConnectingLink(*lane, **(it + 1));
                assert(link != 0);
                SUMOReal linkLength = link->getInternalLengthsAfter();
                roundaboutDistanceAhead += linkLength;
            }
        } else if (encounteredRoundabout) {
            // only check the next roundabout
            break;
        }
    }
    return roundaboutDistanceAhead;
}



int
MSLCM_LC2013::slowDownForBlocked(MSVehicle** blocked, int state) {
    //  if this vehicle is blocking someone in front, we maybe decelerate to let him in
    if ((*blocked) != 0) {
        SUMOReal gap = (*blocked)->getPositionOnLane() - (*blocked)->getVehicleType().getLength() - myVehicle.getPositionOnLane() - myVehicle.getVehicleType().getMinGap();
#ifdef DEBUG_SLOW_DOWN
        if (DEBUG_COND) {
            std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                      << " veh=" << myVehicle.getID()
                      << " blocked=" << Named::getIDSecure(*blocked)
                      << " gap=" << gap
                      << "\n";
        }
#endif
        if (gap > POSITION_EPS) {
            //const bool blockedWantsUrgentRight = (((*blocked)->getLaneChangeModel().getOwnState() & LCA_RIGHT != 0)
            //    && ((*blocked)->getLaneChangeModel().getOwnState() & LCA_URGENT != 0));

            if (myVehicle.getSpeed() < ACCEL2SPEED(myVehicle.getCarFollowModel().getMaxDecel())
                    //|| blockedWantsUrgentRight  // VARIANT_10 (helpblockedRight)
               ) {
                if ((*blocked)->getSpeed() < SUMO_const_haltingSpeed) {
                    state |= LCA_AMBACKBLOCKER_STANDING;
                } else {
                    state |= LCA_AMBACKBLOCKER;
                }
                myVSafes.push_back(myCarFollowModel.followSpeed(
                                       &myVehicle, myVehicle.getSpeed(),
                                       (SUMOReal)(gap - POSITION_EPS), (*blocked)->getSpeed(),
                                       (*blocked)->getCarFollowModel().getMaxDecel()));

                //(*blocked) = 0; // VARIANT_14 (furtherBlock)
#ifdef DEBUG_SLOW_DOWN
                if (DEBUG_COND) {
                    std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                              << " veh=" << myVehicle.getID()
                              << " slowing down for"
                              << " blocked=" << Named::getIDSecure(*blocked)
                              << " helpSpeed=" << myVSafes.back()
                              << "\n";
                }
#endif
            } /* else {
            	// experimental else-branch...
                state |= LCA_AMBACKBLOCKER;
                myVSafes.push_back(myCarFollowModel.followSpeed(
                                       &myVehicle, myVehicle.getSpeed(),
                                       (SUMOReal)(gap - POSITION_EPS), (*blocked)->getSpeed(),
                                       (*blocked)->getCarFollowModel().getMaxDecel()));
            }*/
        }
    }
    return state;
}


void
MSLCM_LC2013::saveBlockerLength(MSVehicle* blocker, int lcaCounter) {
#ifdef DEBUG_SAVE_BLOCKER_LENGTH
    if (DEBUG_COND) {
        std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                  << " veh=" << myVehicle.getID()
                  << " saveBlockerLength blocker=" << Named::getIDSecure(blocker)
                  << " bState=" << (blocker == 0 ? "None" : toString(blocker->getLaneChangeModel().getOwnState()))
                  << "\n";
    }
#endif
    if (blocker != 0 && (blocker->getLaneChangeModel().getOwnState() & lcaCounter) != 0) {
        // is there enough space in front of us for the blocker?
        const SUMOReal potential = myLeftSpace - myVehicle.getCarFollowModel().brakeGap(
                                       myVehicle.getSpeed(), myVehicle.getCarFollowModel().getMaxDecel(), 0);
        if (blocker->getVehicleType().getLengthWithGap() <= potential) {
            // save at least his length in myLeadingBlockerLength
            myLeadingBlockerLength = MAX2(blocker->getVehicleType().getLengthWithGap(), myLeadingBlockerLength);
#ifdef DEBUG_SAVE_BLOCKER_LENGTH
            if (DEBUG_COND) {
                std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                          << " veh=" << myVehicle.getID()
                          << " blocker=" << Named::getIDSecure(blocker)
                          << " saving myLeadingBlockerLength=" << myLeadingBlockerLength
                          << "\n";
            }
#endif
        } else {
            // we cannot save enough space for the blocker. It needs to save
            // space for ego instead
#ifdef DEBUG_SAVE_BLOCKER_LENGTH
            if (DEBUG_COND) {
                std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                          << " veh=" << myVehicle.getID()
                          << " blocker=" << Named::getIDSecure(blocker)
                          << " cannot save space=" << blocker->getVehicleType().getLengthWithGap()
                          << " potential=" << potential
                          << "\n";
            }
#endif
            blocker->getLaneChangeModel().saveBlockerLength(myVehicle.getVehicleType().getLengthWithGap());
        }
    }
}
/****************************************************************************/

