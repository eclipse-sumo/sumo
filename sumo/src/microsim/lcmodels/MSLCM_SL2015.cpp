/****************************************************************************/
/// @file    MSLCM_SL2015.h
/// @author  Jakob Erdmann
/// @date    Tue, 06.10.2015
/// @version $Id$
///
// A lane change model for heterogeneous traffic (based on sub-lanes)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2013-2017 DLR (http://www.dlr.de/) and contributors
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
#include <microsim/MSGlobals.h>
#include "MSLCM_SL2015.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

//#define DEBUG_VEHICLE_GUI_SELECTION 1

// ===========================================================================
// variable definitions
// ===========================================================================
// 80km/h will be the threshold for dividing between long/short foresight
#define LOOK_FORWARD_SPEED_DIVIDER (SUMOReal)14.

// VARIANT_1 (lf*2)
//#define LOOK_FORWARD_FAR  30.
//#define LOOK_FORWARD_NEAR 10.

#define LOOK_FORWARD_RIGHT (SUMOReal)10.
#define LOOK_FORWARD_LEFT  (SUMOReal)20.

#define JAM_FACTOR (SUMOReal)1.
//#define JAM_FACTOR 2. // VARIANT_8 (makes vehicles more focused but also more "selfish")

#define LCA_RIGHT_IMPATIENCE (SUMOReal)-1.
#define CUT_IN_LEFT_SPEED_THRESHOLD (SUMOReal)27.
#define MAX_ONRAMP_LENGTH (SUMOReal)200.

#define LOOK_AHEAD_MIN_SPEED (SUMOReal)0.0
#define LOOK_AHEAD_SPEED_MEMORY (SUMOReal)0.9
#define LOOK_AHEAD_SPEED_DECREMENT 6.

#define HELP_DECEL_FACTOR (SUMOReal)1.0

#define HELP_OVERTAKE  (SUMOReal)(10.0 / 3.6)
#define MIN_FALLBEHIND  (SUMOReal)(7.0 / 3.6)

#define KEEP_RIGHT_HEADWAY (SUMOReal)2.0

#define URGENCY (SUMOReal)2.0

#define ROUNDABOUT_DIST_BONUS (SUMOReal)100.0

#define KEEP_RIGHT_TIME (SUMOReal)5.0 // the number of seconds after which a vehicle should move to the right lane
#define KEEP_RIGHT_ACCEPTANCE (SUMOReal)7.0 // calibration factor for determining the desire to keep right

#define RELGAIN_NORMALIZATION_MIN_SPEED (SUMOReal)10.0

#define TURN_LANE_DIST (SUMOReal)200.0 // the distance at which a lane leading elsewhere is considered to be a turn-lane that must be avoided
#define GAIN_PERCEPTION_THRESHOLD (SUMOReal)0.05 // the minimum relative speed gain which affects the behavior

#define SPEED_GAIN_MIN_SECONDS 20.0

#define ARRIVALPOS_LAT_THRESHOLD 100.0

//#define DEBUG_COND (myVehicle.getID() == "moped.18" || myVehicle.getID() == "moped.16")
//#define DEBUG_COND (myVehicle.getID() == "A")
#define DEBUG_COND (myVehicle.getID() == "disabled")
//#define DEBUG_COND (myVehicle.getID() == "pkw150478" || myVehicle.getID() == "pkw150494" || myVehicle.getID() == "pkw150289")
//#define DEBUG_COND (myVehicle.getID() == "A" || myVehicle.getID() == "B") // fail change to left
//#define DEBUG_COND (myVehicle.getID() == "disabled") // test stops_overtaking
//#define DEBUG_COND false


// ===========================================================================
// member method definitions
// ===========================================================================
MSLCM_SL2015::MSLCM_SL2015(MSVehicle& v) :
    MSAbstractLaneChangeModel(v, LCM_SL2015),
    mySpeedGainProbabilityRight(0),
    mySpeedGainProbabilityLeft(0),
    myKeepRightProbability(0),
    myLeadingBlockerLength(0),
    myLeftSpace(0),
    myLookAheadSpeed(LOOK_AHEAD_MIN_SPEED),
    myLastEdge(0),
    myCanChangeFully(true),
    myPreviousState(0),
    myOrigLatDist(0),
    myStrategicParam(v.getVehicleType().getParameter().getLCParam(SUMO_ATTR_LCA_STRATEGIC_PARAM, 1)),
    myCooperativeParam(v.getVehicleType().getParameter().getLCParam(SUMO_ATTR_LCA_COOPERATIVE_PARAM, 1)),
    mySpeedGainParam(v.getVehicleType().getParameter().getLCParam(SUMO_ATTR_LCA_SPEEDGAIN_PARAM, 1)),
    myKeepRightParam(v.getVehicleType().getParameter().getLCParam(SUMO_ATTR_LCA_KEEPRIGHT_PARAM, 1)),
    mySublaneParam(v.getVehicleType().getParameter().getLCParam(SUMO_ATTR_LCA_SUBLANE_PARAM, 1)),
    myPushy(v.getVehicleType().getParameter().getLCParam(SUMO_ATTR_LCA_PUSHY, 0)),
    myAssertive(v.getVehicleType().getParameter().getLCParam(SUMO_ATTR_LCA_ASSERTIVE, 0)),
    myChangeProbThresholdRight(2.0 * myKeepRightParam / MAX2(NUMERICAL_EPS, mySpeedGainParam)),
    myChangeProbThresholdLeft(0.2 / MAX2(NUMERICAL_EPS, mySpeedGainParam)),
    mySpeedLossProbThreshold(-0.01 + (1 - mySublaneParam)) {
    if (MSGlobals::gLateralResolution <= 0) {
        throw ProcessError("laneChangeModel 'MSLCM_SL2015' is only meant to be used when simulating with '--lateral-resoluion' > 0");
    }
}

MSLCM_SL2015::~MSLCM_SL2015() {
    changed();
}


bool
MSLCM_SL2015::debugVehicle() const {
    return DEBUG_COND;
}


int
MSLCM_SL2015::wantsChangeSublane(
    int laneOffset,
    const MSLeaderDistanceInfo& leaders,
    const MSLeaderDistanceInfo& followers,
    const MSLeaderDistanceInfo& blockers,
    const MSLeaderDistanceInfo& neighLeaders,
    const MSLeaderDistanceInfo& neighFollowers,
    const MSLeaderDistanceInfo& neighBlockers,
    const MSLane& neighLane,
    const std::vector<MSVehicle::LaneQ>& preb,
    MSVehicle** lastBlocked,
    MSVehicle** firstBlocked,
    SUMOReal& latDist, int& blocked) {

    gDebugFlag2 = DEBUG_COND;
    const std::string changeType = laneOffset == -1 ? "right" : (laneOffset == 1 ? "left" : "current");


    if (gDebugFlag2) {
        std::cout << "\n" << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                  //<< std::setprecision(10)
                  << " veh=" << myVehicle.getID()
                  << " lane=" << myVehicle.getLane()->getID()
                  << " pos=" << myVehicle.getPositionOnLane()
                  << " posLat=" << myVehicle.getLateralPositionOnLane()
                  << " speed=" << myVehicle.getSpeed()
                  << " considerChangeTo=" << changeType
                  << "\n";
    }

    int result = _wantsChangeSublane(laneOffset,
                                     leaders, followers, blockers,
                                     neighLeaders, neighFollowers, neighBlockers,
                                     neighLane, preb,
                                     lastBlocked, firstBlocked, latDist, blocked);

    if ((result & LCA_STAY) != 0) {
        // checkBlocking was not called yet
        assert(latDist == 0);
        myOrigLatDist = 0;
        myCanChangeFully = true;
    }

    result = keepLatGap(result, leaders, followers, blockers,
                        neighLeaders, neighFollowers, neighBlockers,
                        neighLane, laneOffset, latDist, blocked);

    result |= getLCA(result, latDist);

    if (gDebugFlag2) {
        if (result & LCA_WANTS_LANECHANGE) {
            std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                      << " veh=" << myVehicle.getID()
                      << " latDist=" << latDist
                      << " state=" << toString((LaneChangeAction)result)
                      << ((blocked & LCA_BLOCKED) ? " (blocked)" : "")
                      << ((blocked & LCA_OVERLAPPING) ? " (overlap)" : "")
                      << "\n\n";
        } else {
            std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                      << " veh=" << myVehicle.getID()
                      << " wantsNoChangeTo=" << changeType
                      << " state=" << toString((LaneChangeAction)result)
                      << "\n\n";
        }
    }
    gDebugFlag2 = false;
    return result;
}


SUMOReal
MSLCM_SL2015::patchSpeed(const SUMOReal min, const SUMOReal wanted, const SUMOReal max, const MSCFModel& cfModel) {
    gDebugFlag2 = DEBUG_COND;

    const SUMOReal newSpeed = _patchSpeed(min, wanted, max, cfModel);
    if (gDebugFlag2) {
        const std::string patched = (wanted != newSpeed ? " patched=" + toString(newSpeed) : "");
        std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                  << " veh=" << myVehicle.getID()
                  << " lane=" << myVehicle.getLane()->getID()
                  << " pos=" << myVehicle.getPositionOnLane()
                  << " v=" << myVehicle.getSpeed()
                  << " wanted=" << wanted
                  << patched
                  << "\n\n";
    }
    gDebugFlag2 = false;
    return newSpeed;
}


SUMOReal
MSLCM_SL2015::_patchSpeed(const SUMOReal min, const SUMOReal wanted, const SUMOReal max, const MSCFModel& cfModel) {

    const SUMOReal time = STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep());

    int state = myOwnState;

    // letting vehicles merge in at the end of the lane in case of counter-lane change, step#2
    SUMOReal MAGIC_offset = 1.;
    //   if we want to change and have a blocking leader and there is enough room for him in front of us
    if (myLeadingBlockerLength != 0) {
        SUMOReal space = myLeftSpace - myLeadingBlockerLength - MAGIC_offset - myVehicle.getVehicleType().getMinGap();
        if (gDebugFlag2) {
            std::cout << time << " veh=" << myVehicle.getID() << " myLeadingBlockerLength=" << myLeadingBlockerLength << " space=" << space << "\n";
        }
        if (space > 0) { // XXX space > -MAGIC_offset
            // compute speed for decelerating towards a place which allows the blocking leader to merge in in front
            SUMOReal safe = cfModel.stopSpeed(&myVehicle, myVehicle.getSpeed(), space);
            // if we are approaching this place
            if (safe < wanted) {
                if (gDebugFlag2) {
                    std::cout << time << " veh=" << myVehicle.getID() << " slowing down for leading blocker, safe=" << safe << (safe + NUMERICAL_EPS < min ? " (not enough)" : "") << "\n";
                }
                return MAX2(min, safe);
            }
        }
    }

    SUMOReal nVSafe = wanted;
    bool gotOne = false;
    for (std::vector<SUMOReal>::const_iterator i = myVSafes.begin(); i != myVSafes.end(); ++i) {
        SUMOReal v = (*i);
        if (v >= min && v <= max) {
            nVSafe = MIN2(v, nVSafe);
            gotOne = true;
            if (gDebugFlag2) {
                std::cout << time << " veh=" << myVehicle.getID() << " got nVSafe=" << nVSafe << "\n";
            }
        } else {
            if (v < min) {
                if (gDebugFlag2) {
                    std::cout << time << " veh=" << myVehicle.getID() << " ignoring low nVSafe=" << v << " min=" << min << "\n";
                }
            } else {
                if (gDebugFlag2) {
                    std::cout << time << " veh=" << myVehicle.getID() << " ignoring high nVSafe=" << v << " max=" << max << "\n";
                }
            }
        }
    }

    if (gotOne && !myDontBrake) {
        if (gDebugFlag2) {
            std::cout << time << " veh=" << myVehicle.getID() << " got vSafe\n";
        }
        return nVSafe;
    }

    // check whether the vehicle is blocked
    if ((state & LCA_WANTS_LANECHANGE) != 0 && (state & LCA_BLOCKED) != 0) {
        if ((state & LCA_STRATEGIC) != 0) {
            // necessary decelerations are controlled via vSafe. If there are
            // none it means we should speed up
            if (gDebugFlag2) {
                std::cout << time << " veh=" << myVehicle.getID() << " LCA_WANTS_LANECHANGE (strat, no vSafe)\n";
            }
            return (max + wanted) / (SUMOReal) 2.0;
        } else if ((state & LCA_COOPERATIVE) != 0) {
            // only minor adjustments in speed should be done
            if ((state & LCA_BLOCKED_BY_LEADER) != 0) {
                if (gDebugFlag2) {
                    std::cout << time << " veh=" << myVehicle.getID() << " LCA_BLOCKED_BY_LEADER (coop)\n";
                }
                return (min + wanted) / (SUMOReal) 2.0;
            }
            if ((state & LCA_BLOCKED_BY_FOLLOWER) != 0) {
                if (gDebugFlag2) {
                    std::cout << time << " veh=" << myVehicle.getID() << " LCA_BLOCKED_BY_FOLLOWER (coop)\n";
                }
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
        if (gDebugFlag2) {
            std::cout << time << " veh=" << myVehicle.getID() << " LCA_AMBLOCKINGLEADER\n";
        }
        return (max + wanted) / (SUMOReal) 2.0;
    }

    if ((state & LCA_AMBLOCKINGFOLLOWER_DONTBRAKE) != 0) {
        if (gDebugFlag2) {
            std::cout << time << " veh=" << myVehicle.getID() << " LCA_AMBLOCKINGFOLLOWER_DONTBRAKE\n";
        }
        /*
        // VARIANT_4 (dontbrake)
        if (max <= myVehicle.getCarFollowModel().maxNextSpeed(myVehicle.getSpeed(), &myVehicle) && min == 0) { // !!! was standing
            return wanted;
        }
        return (min + wanted) / (SUMOReal) 2.0;
        */
    }
    return wanted;
}


void*
MSLCM_SL2015::inform(void* info, MSVehicle* sender) {
    Info* pinfo = (Info*) info;
    if (pinfo->first >= 0) {
        myVSafes.push_back(pinfo->first);
    }
    //myOwnState &= 0xffffffff; // reset all bits of MyLCAEnum but only those
    myOwnState |= pinfo->second;
    if (gDebugFlag2 || DEBUG_COND) {
        std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                  << " veh=" << myVehicle.getID()
                  << " informedBy=" << sender->getID()
                  << " info=" << pinfo->second
                  << " vSafe=" << pinfo->first
                  << "\n";
    }
    delete pinfo;
    return (void*) true;
}


void
MSLCM_SL2015::msg(const CLeaderDist& cld, SUMOReal speed, int state) {
    assert(cld.first != 0);
    ((MSVehicle*)cld.first)->getLaneChangeModel().inform(new Info(speed, state), &myVehicle);
}


SUMOReal
MSLCM_SL2015::informLeader(int blocked,
                           int dir,
                           const CLeaderDist& neighLead,
                           SUMOReal remainingSeconds) {
    SUMOReal plannedSpeed = MIN2(myVehicle.getSpeed(),
                                 myVehicle.getCarFollowModel().stopSpeed(&myVehicle, myVehicle.getSpeed(), myLeftSpace - myLeadingBlockerLength));
    for (std::vector<SUMOReal>::const_iterator i = myVSafes.begin(); i != myVSafes.end(); ++i) {
        SUMOReal v = (*i);
        if (v >= myVehicle.getSpeed() - ACCEL2SPEED(myVehicle.getCarFollowModel().getMaxDecel())) {
            plannedSpeed = MIN2(plannedSpeed, v);
        }
    }
    if (gDebugFlag2) {
        std::cout << " informLeader speed=" <<  myVehicle.getSpeed() << " planned=" << plannedSpeed << "\n";
    }

    if ((blocked & LCA_BLOCKED_BY_LEADER) != 0) {
        assert(neighLead.first != 0);
        const MSVehicle* nv = neighLead.first;
        if (gDebugFlag2) std::cout << " blocked by leader nv=" <<  nv->getID() << " nvSpeed=" << nv->getSpeed() << " needGap="
                                       << myVehicle.getCarFollowModel().getSecureGap(myVehicle.getSpeed(), nv->getSpeed(), nv->getCarFollowModel().getMaxDecel()) << "\n";
        // decide whether we want to overtake the leader or follow it
        const SUMOReal dv = plannedSpeed - nv->getSpeed();
        const SUMOReal overtakeDist = (neighLead.second // drive to back of follower
                                       + nv->getVehicleType().getLengthWithGap() // drive to front of follower
                                       + myVehicle.getVehicleType().getLength() // ego back reaches follower front
                                       + nv->getCarFollowModel().getSecureGap( // save gap to follower
                                           nv->getSpeed(), myVehicle.getSpeed(), myVehicle.getCarFollowModel().getMaxDecel()));

        if (dv < 0
                // overtaking on the right on an uncongested highway is forbidden (noOvertakeLCLeft)
                || (dir == LCA_MLEFT && !myVehicle.congested() && !myAllowOvertakingRight)
                // not enough space to overtake? (we will start to brake when approaching a dead end)
                || myLeftSpace - myLeadingBlockerLength - myVehicle.getCarFollowModel().brakeGap(myVehicle.getSpeed()) < overtakeDist
                // not enough time to overtake?
                || dv * remainingSeconds < overtakeDist) {
            // cannot overtake
            msg(neighLead, -1, dir | LCA_AMBLOCKINGLEADER);
            // slow down smoothly to follow leader
            const SUMOReal targetSpeed = myCarFollowModel.followSpeed(
                                             &myVehicle, myVehicle.getSpeed(), neighLead.second, nv->getSpeed(), nv->getCarFollowModel().getMaxDecel());
            if (targetSpeed < myVehicle.getSpeed()) {
                // slow down smoothly to follow leader
                const SUMOReal decel = ACCEL2SPEED(MIN2(myVehicle.getCarFollowModel().getMaxDecel(),
                                                        MAX2(MIN_FALLBEHIND, (myVehicle.getSpeed() - targetSpeed) / remainingSeconds)));
                //const SUMOReal nextSpeed = MAX2((SUMOReal)0, MIN2(plannedSpeed, myVehicle.getSpeed() - decel));
                const SUMOReal nextSpeed = MIN2(plannedSpeed, myVehicle.getSpeed() - decel);
                if (gDebugFlag2) {
                    std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                              << " cannot overtake leader nv=" << nv->getID()
                              << " dv=" << dv
                              << " remainingSeconds=" << remainingSeconds
                              << " targetSpeed=" << targetSpeed
                              << " nextSpeed=" << nextSpeed
                              << "\n";
                }
                myVSafes.push_back(nextSpeed);
                return nextSpeed;
            } else {
                // leader is fast enough anyway
                if (gDebugFlag2) {
                    std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                              << " cannot overtake fast leader nv=" << nv->getID()
                              << " dv=" << dv
                              << " remainingSeconds=" << remainingSeconds
                              << " targetSpeed=" << targetSpeed
                              << "\n";
                }
                myVSafes.push_back(targetSpeed);
                return plannedSpeed;
            }
        } else {
            if (gDebugFlag2) {
                std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                          << " wants to overtake leader nv=" << nv->getID()
                          << " dv=" << dv
                          << " remainingSeconds=" << remainingSeconds
                          << " currentGap=" << neighLead.second
                          << " secureGap=" << nv->getCarFollowModel().getSecureGap(nv->getSpeed(), myVehicle.getSpeed(), myVehicle.getCarFollowModel().getMaxDecel())
                          << " overtakeDist=" << overtakeDist
                          << "\n";
            }
            // overtaking, leader should not accelerate
            msg(neighLead, nv->getSpeed(), dir | LCA_AMBLOCKINGLEADER);
            return -1;
        }
    } else if (neighLead.first != 0) { // (remainUnblocked)
        // we are not blocked now. make sure we stay far enough from the leader
        const MSVehicle* nv = neighLead.first;
        const SUMOReal nextNVSpeed = nv->getSpeed() - HELP_OVERTAKE; // conservative
        const SUMOReal dv = SPEED2DIST(myVehicle.getSpeed() - nextNVSpeed);
        const SUMOReal targetSpeed = myCarFollowModel.followSpeed(
                                         &myVehicle, myVehicle.getSpeed(), neighLead.second - dv, nextNVSpeed, nv->getCarFollowModel().getMaxDecel());
        myVSafes.push_back(targetSpeed);
        if (gDebugFlag2) {
            std::cout << " not blocked by leader nv=" <<  nv->getID()
                      << " nvSpeed=" << nv->getSpeed()
                      << " gap=" << neighLead.second
                      << " nextGap=" << neighLead.second - dv
                      << " needGap=" << myVehicle.getCarFollowModel().getSecureGap(myVehicle.getSpeed(), nv->getSpeed(), nv->getCarFollowModel().getMaxDecel())
                      << " targetSpeed=" << targetSpeed
                      << "\n";
        }
        return MIN2(targetSpeed, plannedSpeed);
    } else {
        // not overtaking
        return plannedSpeed;
    }
}


void
MSLCM_SL2015::informFollower(int blocked,
                             int dir,
                             const CLeaderDist& neighFollow,
                             SUMOReal remainingSeconds,
                             SUMOReal plannedSpeed) {
    if ((blocked & LCA_BLOCKED_BY_FOLLOWER) != 0) {
        assert(neighFollow.first != 0);
        const MSVehicle* nv = neighFollow.first;
        if (gDebugFlag2) std::cout << " blocked by follower nv=" <<  nv->getID() << " nvSpeed=" << nv->getSpeed() << " needGap="
                                       << nv->getCarFollowModel().getSecureGap(nv->getSpeed(), myVehicle.getSpeed(), myVehicle.getCarFollowModel().getMaxDecel()) << "\n";

        // are we fast enough to cut in without any help?
        if (plannedSpeed - nv->getSpeed() >= HELP_OVERTAKE) {
            const SUMOReal neededGap = nv->getCarFollowModel().getSecureGap(nv->getSpeed(), plannedSpeed, myVehicle.getCarFollowModel().getMaxDecel());
            if ((neededGap - neighFollow.second) / remainingSeconds < (plannedSpeed - nv->getSpeed())) {
                if (gDebugFlag2) {
                    std::cout << " wants to cut in before  nv=" << nv->getID() << " without any help neededGap=" << neededGap << "\n";
                }
                // follower might even accelerate but not to much
                msg(neighFollow, plannedSpeed - HELP_OVERTAKE, dir | LCA_AMBLOCKINGFOLLOWER);
                return;
            }
        }
        // decide whether we will request help to cut in before the follower or allow to be overtaken

        // PARAMETERS
        // assume other vehicle will assume the equivalent of 1 second of
        // maximum deceleration to help us (will probably be spread over
        // multiple seconds)
        // -----------
        const SUMOReal helpDecel = nv->getCarFollowModel().getMaxDecel() * HELP_DECEL_FACTOR ;

        // change in the gap between ego and blocker over 1 second (not STEP!)
        const SUMOReal neighNewSpeed = MAX2((SUMOReal)0, nv->getSpeed() - ACCEL2SPEED(helpDecel));
        const SUMOReal neighNewSpeed1s = MAX2((SUMOReal)0, nv->getSpeed() - helpDecel);
        const SUMOReal dv = plannedSpeed - neighNewSpeed1s;
        // new gap between follower and self in case the follower does brake for 1s
        const SUMOReal decelGap = neighFollow.second + dv;
        const SUMOReal secureGap = nv->getCarFollowModel().getSecureGap(neighNewSpeed1s, plannedSpeed, myVehicle.getCarFollowModel().getMaxDecel());
        if (gDebugFlag2) {
            std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                      << " egoV=" << myVehicle.getSpeed()
                      << " egoNV=" << plannedSpeed
                      << " nvNewSpeed=" << neighNewSpeed
                      << " nvNewSpeed1s=" << neighNewSpeed1s
                      << " deltaGap=" << dv
                      << " decelGap=" << decelGap
                      << " secGap=" << secureGap
                      << "\n";
        }
        if (decelGap > 0 && decelGap >= secureGap) {
            // if the blocking neighbor brakes it could actually help
            // how hard does it actually need to be?
            // to be safe in the next step the following equation has to hold:
            //   vsafe <= followSpeed(gap=currentGap - SPEED2DIST(vsafe), ...)
            // we compute an upper bound on vsafe by doing the computation twice
            const SUMOReal vsafe1 = MAX2(neighNewSpeed, nv->getCarFollowModel().followSpeed(
                                             nv, nv->getSpeed(), neighFollow.second + SPEED2DIST(plannedSpeed), plannedSpeed, myVehicle.getCarFollowModel().getMaxDecel()));
            const SUMOReal vsafe = MAX2(neighNewSpeed, nv->getCarFollowModel().followSpeed(
                                            nv, nv->getSpeed(), neighFollow.second + SPEED2DIST(plannedSpeed - vsafe1), plannedSpeed, myVehicle.getCarFollowModel().getMaxDecel()));
            // the following assertion cannot be guaranteed because the CFModel handles small gaps differently, see MSCFModel::maximumSafeStopSpeed
            // assert(vsafe <= vsafe1);
            msg(neighFollow, vsafe, dir | LCA_AMBLOCKINGFOLLOWER);
            if (gDebugFlag2) {
                std::cout << " wants to cut in before nv=" << nv->getID()
                          << " vsafe1=" << vsafe1
                          << " vsafe=" << vsafe
                          << " newSecGap=" << nv->getCarFollowModel().getSecureGap(vsafe, plannedSpeed, myVehicle.getCarFollowModel().getMaxDecel())
                          << "\n";
            }
        } else if (dv > 0 && dv * remainingSeconds > (secureGap - decelGap + POSITION_EPS)) {
            // decelerating once is sufficient to open up a large enough gap in time
            msg(neighFollow, neighNewSpeed, dir | LCA_AMBLOCKINGFOLLOWER);
            if (gDebugFlag2) {
                std::cout << " wants to cut in before nv=" << nv->getID() << " (eventually)\n";
            }
        } else if (dir == LCA_MRIGHT && !myAllowOvertakingRight && !nv->congested()) {
            const SUMOReal vhelp = MAX2(neighNewSpeed, HELP_OVERTAKE);
            msg(neighFollow, vhelp, dir | LCA_AMBLOCKINGFOLLOWER);
            if (gDebugFlag2) {
                std::cout << " wants to cut in before nv=" << nv->getID() << " (nv cannot overtake right)\n";
            }
        } else {
            SUMOReal vhelp = MAX2(nv->getSpeed(), myVehicle.getSpeed() + HELP_OVERTAKE);
            if (nv->getSpeed() > myVehicle.getSpeed() &&
                    ((dir == LCA_MRIGHT && myVehicle.getWaitingSeconds() > LCA_RIGHT_IMPATIENCE)
                     || (dir == LCA_MLEFT && plannedSpeed > CUT_IN_LEFT_SPEED_THRESHOLD) // VARIANT_22 (slowDownLeft)
                     // XXX this is a hack to determine whether the vehicles is on an on-ramp. This information should be retrieved from the network itself
                     || (dir == LCA_MLEFT && myLeftSpace > MAX_ONRAMP_LENGTH)
                    )) {
                // let the follower slow down to increase the likelyhood that later vehicles will be slow enough to help
                // follower should still be fast enough to open a gap
                vhelp = MAX2(neighNewSpeed, myVehicle.getSpeed() + HELP_OVERTAKE);
                if (gDebugFlag2) {
                    std::cout << " wants right follower to slow down a bit\n";
                }
                if ((nv->getSpeed() - myVehicle.getSpeed()) / helpDecel < remainingSeconds) {
                    if (gDebugFlag2) {
                        std::cout << " wants to cut in before right follower nv=" << nv->getID() << " (eventually)\n";
                    }
                    msg(neighFollow, vhelp, dir | LCA_AMBLOCKINGFOLLOWER);
                    return;
                }
            }
            msg(neighFollow, vhelp, dir | LCA_AMBLOCKINGFOLLOWER);
            // this follower is supposed to overtake us. slow down smoothly to allow this
            const SUMOReal overtakeDist = (neighFollow.second // follower reaches ego back
                                           + myVehicle.getVehicleType().getLengthWithGap() // follower reaches ego front
                                           + nv->getVehicleType().getLength() // follower back at ego front
                                           + myVehicle.getCarFollowModel().getSecureGap( // follower has safe dist to ego
                                               plannedSpeed, vhelp, nv->getCarFollowModel().getMaxDecel()));
            // speed difference to create a sufficiently large gap
            const SUMOReal needDV = overtakeDist / remainingSeconds;
            // make sure the deceleration is not to strong
            myVSafes.push_back(MAX2(vhelp - needDV, myVehicle.getSpeed() - ACCEL2SPEED(myVehicle.getCarFollowModel().getMaxDecel())));

            if (gDebugFlag2) {
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
        }
    } else if (neighFollow.first != 0) {
        // we are not blocked no, make sure it remains that way
        const MSVehicle* nv = neighFollow.first;
        const SUMOReal vsafe1 = nv->getCarFollowModel().followSpeed(
                                    nv, nv->getSpeed(), neighFollow.second + SPEED2DIST(plannedSpeed), plannedSpeed, myVehicle.getCarFollowModel().getMaxDecel());
        const SUMOReal vsafe = nv->getCarFollowModel().followSpeed(
                                   nv, nv->getSpeed(), neighFollow.second + SPEED2DIST(plannedSpeed - vsafe1), plannedSpeed, myVehicle.getCarFollowModel().getMaxDecel());
        msg(neighFollow, vsafe, dir | LCA_AMBLOCKINGFOLLOWER);
        if (gDebugFlag2) {
            std::cout << " wants to cut in before non-blocking follower nv=" << nv->getID() << "\n";
        }
    }
}

SUMOReal
MSLCM_SL2015::informLeaders(int blocked, int dir,
                            const std::vector<CLeaderDist>& blockers,
                            SUMOReal remainingSeconds) {
    SUMOReal plannedSpeed = myVehicle.getSpeed();
    for (std::vector<CLeaderDist>::const_iterator it = blockers.begin(); it != blockers.end(); ++it) {
        plannedSpeed = MIN2(plannedSpeed, informLeader(blocked, dir, *it, remainingSeconds));
    }
    return plannedSpeed;
}


void
MSLCM_SL2015::informFollowers(int blocked, int dir,
                              const std::vector<CLeaderDist>& blockers,
                              SUMOReal remainingSeconds,
                              SUMOReal plannedSpeed) {
    for (std::vector<CLeaderDist>::const_iterator it = blockers.begin(); it != blockers.end(); ++it) {
        informFollower(blocked, dir, *it, remainingSeconds, plannedSpeed);
    }
}


void
MSLCM_SL2015::prepareStep() {
    // keep information about strategic change direction
    myOwnState = (myOwnState & LCA_STRATEGIC) ? (myOwnState & LCA_WANTS_LANECHANGE) : 0;
    if (myCanChangeFully) {
        myOrigLatDist = 0;
    }
    myLeadingBlockerLength = 0;
    myLeftSpace = 0;
    myVSafes.clear();
    myDontBrake = false;
    // truncate to work around numerical instability between different builds
    mySpeedGainProbabilityRight = ceil(mySpeedGainProbabilityRight * 100000.0) * 0.00001;
    mySpeedGainProbabilityLeft = ceil(mySpeedGainProbabilityLeft * 100000.0) * 0.00001;
    myKeepRightProbability = ceil(myKeepRightProbability * 100000.0) * 0.00001;
    // updated myExpectedSublaneSpeeds
    // XXX only do this when (sub)lane changing is possible
    std::vector<SUMOReal> newExpectedSpeeds;
    //std::cout << SIMTIME << " veh=" << myVehicle.getID() << " myExpectedSublaneSpeeds=" << toString(myExpectedSublaneSpeeds) << "\n";
    if (myExpectedSublaneSpeeds.size() != myVehicle.getLane()->getEdge().getSubLaneSides().size()) {
        // initialize
        const MSEdge* currEdge = &myVehicle.getLane()->getEdge();
        const std::vector<MSLane*>& lanes = currEdge->getLanes();
        for (std::vector<MSLane*>::const_iterator it_lane = lanes.begin(); it_lane != lanes.end(); ++it_lane) {
            const int subLanes = MAX2(1, int(ceil((*it_lane)->getWidth() / MSGlobals::gLateralResolution)));
            for (int i = 0; i < subLanes; ++i) {
                newExpectedSpeeds.push_back((*it_lane)->getVehicleMaxSpeed(&myVehicle));
            }
        }
        if (myExpectedSublaneSpeeds.size() > 0) {
            // copy old values
            assert(myLastEdge != 0);
            if (myLastEdge->getSubLaneSides().size() == myExpectedSublaneSpeeds.size()) {
                const int subLaneShift = computeSublaneShift(myLastEdge, currEdge);
                if (subLaneShift < std::numeric_limits<int>::max()) {
                    for (int i = 0; i < (int)myExpectedSublaneSpeeds.size(); ++i) {
                        const int newI = i + subLaneShift;
                        if (newI > 0 && newI < (int)newExpectedSpeeds.size()) {
                            newExpectedSpeeds[newI] = myExpectedSublaneSpeeds[i];
                        }
                    }
                }
            }
        }
        myExpectedSublaneSpeeds = newExpectedSpeeds;
        myLastEdge = currEdge;
    }
    assert(myExpectedSublaneSpeeds.size() == myVehicle.getLane()->getEdge().getSubLaneSides().size());
}


int
MSLCM_SL2015::computeSublaneShift(const MSEdge* prevEdge, const MSEdge* curEdge) {
    // find the first lane that targets the new edge
    int prevShift = 0;
    const std::vector<MSLane*>& lanes = prevEdge->getLanes();
    for (std::vector<MSLane*>::const_iterator it_lane = lanes.begin(); it_lane != lanes.end(); ++it_lane) {
        const MSLane* lane = *it_lane;
        for (MSLinkCont::const_iterator it_link = lane->getLinkCont().begin(); it_link != lane->getLinkCont().end(); ++it_link) {
            if (&((*it_link)->getLane()->getEdge()) == curEdge) {
                int curShift = 0;
                const MSLane* target = (*it_link)->getLane();
                const std::vector<MSLane*>& lanes2 = curEdge->getLanes();
                for (std::vector<MSLane*>::const_iterator it_lane2 = lanes2.begin(); it_lane2 != lanes2.end(); ++it_lane2) {
                    const MSLane* lane2 = *it_lane2;
                    if (lane2 == target) {
                        return prevShift + curShift;
                    }
                    MSLeaderInfo ahead(lane2);
                    curShift += ahead.numSublanes();
                }
                assert(false);
            }
        }
        MSLeaderInfo ahead(lane);
        prevShift -= ahead.numSublanes();
    }
    return std::numeric_limits<int>::max();
}


void
MSLCM_SL2015::changed() {
    if (!myCanChangeFully) {
        // do not reset state yet
        if (DEBUG_COND) {
            std::cout << SIMTIME << " veh=" << myVehicle.getID() << " state not reset\n";
        }
        return;
    }
    myOwnState = 0;
    // XX do not reset values for unfinished maneuvers
    mySpeedGainProbabilityRight = 0;
    mySpeedGainProbabilityLeft = 0;
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
    if (DEBUG_COND) {
        std::cout << SIMTIME << " veh=" << myVehicle.getID() << " changed()\n";
    }
}


int
MSLCM_SL2015::_wantsChangeSublane(
    int laneOffset,
    const MSLeaderDistanceInfo& leaders,
    const MSLeaderDistanceInfo& followers,
    const MSLeaderDistanceInfo& blockers,
    const MSLeaderDistanceInfo& neighLeaders,
    const MSLeaderDistanceInfo& neighFollowers,
    const MSLeaderDistanceInfo& neighBlockers,
    const MSLane& neighLane,
    const std::vector<MSVehicle::LaneQ>& preb,
    MSVehicle** lastBlocked,
    MSVehicle** firstBlocked,
    SUMOReal& latDist, int& blocked) {

    const SUMOTime currentTime = MSNet::getInstance()->getCurrentTimeStep();
    // compute bestLaneOffset
    MSVehicle::LaneQ curr, neigh;
    int bestLaneOffset = 0;
    SUMOReal currentDist = 0;
    SUMOReal neighDist = 0;
    int currIdx = 0;
    MSLane* prebLane = myVehicle.getLane();
    if (prebLane->getEdge().getPurpose() == MSEdge::EDGEFUNCTION_INTERNAL) {
        // internal edges are not kept inside the bestLanes structure
        prebLane = prebLane->getLinkCont()[0]->getLane();
    }
    for (int p = 0; p < (int) preb.size(); ++p) {
        if (preb[p].lane == prebLane && p + laneOffset >= 0) {
            assert(p + laneOffset < (int)preb.size());
            curr = preb[p];
            neigh = preb[p + laneOffset];
            currentDist = curr.length;
            neighDist = neigh.length;
            bestLaneOffset = curr.bestLaneOffset;
            // VARIANT_13 (equalBest)
            if (bestLaneOffset == 0 && preb[p + laneOffset].bestLaneOffset == 0) {
                if (gDebugFlag2) {
                    std::cout << STEPS2TIME(currentTime)
                              << " veh=" << myVehicle.getID()
                              << " bestLaneOffsetOld=" << bestLaneOffset
                              << " bestLaneOffsetNew=" << laneOffset
                              << "\n";
                }
                bestLaneOffset = laneOffset;
            }
            currIdx = p;
            break;
        }
    }
    // direction specific constants
    const bool right = (laneOffset == -1);
    const bool left = (laneOffset == 1);
    const int myLca = (right ? LCA_MRIGHT : (left ? LCA_MLEFT : 0));
    const int lcaCounter = (right ? LCA_LEFT : (left ? LCA_RIGHT : LCA_NONE));
    const int myLcaCounter = (right ? LCA_MLEFT : LCA_MRIGHT);
    const bool changeToBest = (right && bestLaneOffset < 0) || (left && bestLaneOffset > 0) || (laneOffset == 0 && bestLaneOffset == 0);
    // keep information about being a leader/follower but remove information
    // about previous lane change request or urgency
    int ret = (myOwnState & 0xffff0000);

    // compute the distance when changing to the neighboring lane
    // (ensure we do not lap into the line behind neighLane since there might be unseen blockers)
    const SUMOReal halfCurrentLaneWidth = 0.5 * myVehicle.getLane()->getWidth();
    const SUMOReal halfVehWidth = 0.5 * myVehicle.getVehicleType().getWidth();
    const SUMOReal latPos = myVehicle.getLateralPositionOnLane();
    SUMOReal leftLimit = halfCurrentLaneWidth - halfVehWidth - latPos;
    SUMOReal rightLimit = -halfCurrentLaneWidth + halfVehWidth - latPos;
    SUMOReal latLaneDist = 0;  // minimum distance to move the vehicle fully onto the new lane
    if (laneOffset == -1) {
        latLaneDist = rightLimit - myVehicle.getVehicleType().getWidth();
        rightLimit -= neighLane.getWidth();
    } else if (laneOffset == 1) {
        latLaneDist = leftLimit + myVehicle.getVehicleType().getWidth();
        leftLimit += neighLane.getWidth();
    }
    // VARIANT_5 (disableAMBACKBLOCKER1)
    /*
    if (leader.first != 0
            && (myOwnState & LCA_AMBLOCKINGFOLLOWER_DONTBRAKE) != 0
            && (leader.first->getLaneChangeModel().getOwnState() & LCA_AMBLOCKINGFOLLOWER_DONTBRAKE) != 0) {

        myOwnState &= (0xffffffff - LCA_AMBLOCKINGFOLLOWER_DONTBRAKE);
        if (myVehicle.getSpeed() > SUMO_const_haltingSpeed) {
            myOwnState |= LCA_AMBACKBLOCKER;
        } else {
            ret |= LCA_AMBACKBLOCKER;
            myDontBrake = true;
        }
    }
    */

    if (gDebugFlag2) {
        std::cout << STEPS2TIME(currentTime)
                  << " veh=" << myVehicle.getID()
                  << " myState=" << toString((LaneChangeAction)myOwnState)
                  << " firstBlocked=" << Named::getIDSecure(*firstBlocked)
                  << " lastBlocked=" << Named::getIDSecure(*lastBlocked)
                  << " leaders=" << leaders.toString()
                  << " followers=" << followers.toString()
                  << " blockers=" << blockers.toString()
                  << " neighLeaders=" << neighLeaders.toString()
                  << " neighFollowers=" << neighFollowers.toString()
                  << " neighBlockers=" << neighBlockers.toString()
                  << " changeToBest=" << changeToBest
                  << " latLaneDist=" << latLaneDist
                  << " leftLimit=" << leftLimit
                  << " rightLimit=" << rightLimit
                  << " expectedSpeeds=" << toString(myExpectedSublaneSpeeds)
                  << "\n";
    }

    ret = slowDownForBlocked(lastBlocked, ret);
    // VARIANT_14 (furtherBlock)
    if (lastBlocked != firstBlocked) {
        ret = slowDownForBlocked(firstBlocked, ret);
    }


    // we try to estimate the distance which is necessary to get on a lane
    //  we have to get on in order to keep our route
    // we assume we need something that depends on our velocity
    // and compare this with the free space on our wished lane
    //
    // if the free space is somehow less than the space we need, we should
    //  definitely try to get to the desired lane
    //
    // this rule forces our vehicle to change the lane if a lane changing is necessary soon
    // lookAheadDistance:
    // we do not want the lookahead distance to change all the time so we discrectize the speed a bit

    // VARIANT_18 (laHyst)
    if (myVehicle.getSpeed() > myLookAheadSpeed) {
        myLookAheadSpeed = myVehicle.getSpeed();
    } else {
        myLookAheadSpeed = MAX2(LOOK_AHEAD_MIN_SPEED,
                                (LOOK_AHEAD_SPEED_MEMORY * myLookAheadSpeed + (1 - LOOK_AHEAD_SPEED_MEMORY) * myVehicle.getSpeed()));
    }
    //myLookAheadSpeed = myVehicle.getLane()->getVehicleMaxSpeed(&myVehicle);

    //SUMOReal laDist = laSpeed > LOOK_FORWARD_SPEED_DIVIDER
    //              ? laSpeed *  LOOK_FORWARD_FAR
    //              : laSpeed *  LOOK_FORWARD_NEAR;
    SUMOReal laDist = myLookAheadSpeed * (right ? LOOK_FORWARD_RIGHT : LOOK_FORWARD_LEFT) * myStrategicParam;
    laDist += myVehicle.getVehicleType().getLengthWithGap() * (SUMOReal) 2.;

    // react to a stopped leader on the current lane
    if (bestLaneOffset == 0 && leaders.hasStoppedVehicle()) {
        // value is doubled for the check since we change back and forth
        // laDist = 0.5 * (myVehicle.getVehicleType().getLengthWithGap() + leader.first->getVehicleType().getLengthWithGap());
        // XXX determine lenght of longest stopped vehicle
        laDist = myVehicle.getVehicleType().getLengthWithGap();
    }

    // free space that is available for changing
    //const SUMOReal neighSpeed = (neighLead.first != 0 ? neighLead.first->getSpeed() :
    //        neighFollow.first != 0 ? neighFollow.first->getSpeed() :
    //        best.lane->getSpeedLimit());
    // @note: while this lets vehicles change earlier into the correct direction
    // it also makes the vehicles more "selfish" and prevents changes which are necessary to help others

    // VARIANT_15 (insideRoundabout)
    int roundaboutEdgesAhead = 0;
    for (std::vector<MSLane*>::iterator it = curr.bestContinuations.begin(); it != curr.bestContinuations.end(); ++it) {
        if ((*it) != 0 && (*it)->getEdge().isRoundabout()) {
            roundaboutEdgesAhead += 1;
        } else if (roundaboutEdgesAhead > 0) {
            // only check the next roundabout
            break;
        }
    }
    int roundaboutEdgesAheadNeigh = 0;
    for (std::vector<MSLane*>::iterator it = neigh.bestContinuations.begin(); it != neigh.bestContinuations.end(); ++it) {
        if ((*it) != 0 && (*it)->getEdge().isRoundabout()) {
            roundaboutEdgesAheadNeigh += 1;
        } else if (roundaboutEdgesAheadNeigh > 0) {
            // only check the next roundabout
            break;
        }
    }
    if (roundaboutEdgesAhead > 1) {
        currentDist += roundaboutEdgesAhead * ROUNDABOUT_DIST_BONUS * myCooperativeParam;
        neighDist += roundaboutEdgesAheadNeigh * ROUNDABOUT_DIST_BONUS * myCooperativeParam;
    }
    if (roundaboutEdgesAhead > 0) {
        if (gDebugFlag2) {
            std::cout << " roundaboutEdgesAhead=" << roundaboutEdgesAhead << " roundaboutEdgesAheadNeigh=" << roundaboutEdgesAheadNeigh << "\n";
        }
    }

    if (laneOffset != 0) {
        ret = checkStrategicChange(ret,
                                   laneOffset,
                                   preb,
                                   leaders,
                                   neighLeaders,
                                   currIdx,
                                   bestLaneOffset,
                                   changeToBest,
                                   lcaCounter,
                                   currentDist,
                                   neighDist,
                                   laDist,
                                   roundaboutEdgesAhead);
    }

    if ((ret & LCA_STAY) != 0) {
        return ret;
    }
    if ((ret & LCA_URGENT) != 0) {
        // prepare urgent lane change maneuver
        // save the left space
        myLeftSpace = currentDist - myVehicle.getPositionOnLane();
        if (changeToBest && abs(bestLaneOffset) > 1) {
            // there might be a vehicle which needs to counter-lane-change one lane further and we cannot see it yet
            if (gDebugFlag2) {
                std::cout << "  reserving space for unseen blockers\n";
            }
            myLeadingBlockerLength = MAX2((SUMOReal)(right ? 20.0 : 40.0), myLeadingBlockerLength);
        }

        // letting vehicles merge in at the end of the lane in case of counter-lane change, step#1
        //   if there is a leader and he wants to change to the opposite direction
        const MSVehicle* neighLeadLongest = getLongest(neighLeaders).first;
        saveBlockerLength(neighLeadLongest, lcaCounter);
        if (*firstBlocked != neighLeadLongest) {
            saveBlockerLength(*firstBlocked, lcaCounter);
        }
        latDist = latLaneDist;
        std::vector<CLeaderDist> collectLeadBlockers;
        std::vector<CLeaderDist> collectFollowBlockers;

        blocked = checkBlocking(neighLane, latDist, laneOffset,
                                leaders, followers, blockers,
                                neighLeaders, neighFollowers, neighBlockers, &collectLeadBlockers, &collectFollowBlockers);

        const SUMOReal remainingSeconds = ((ret & LCA_TRACI) == 0 ?
                                           MAX2((SUMOReal)STEPS2TIME(TS), myLeftSpace / MAX2(myLookAheadSpeed, NUMERICAL_EPS) / abs(bestLaneOffset) / URGENCY) :
                                           myVehicle.getInfluencer().changeRequestRemainingSeconds(currentTime));
        const SUMOReal plannedSpeed = informLeaders(blocked, myLca, collectLeadBlockers, remainingSeconds);
        // coordinate with direct obstructions
        if (plannedSpeed >= 0) {
            // maybe we need to deal with a blocking follower
            informFollowers(blocked, myLca, collectFollowBlockers, remainingSeconds, plannedSpeed);
        }

        if (gDebugFlag2) {
            std::cout << STEPS2TIME(currentTime)
                      << " veh=" << myVehicle.getID()
                      << " myLeftSpace=" << myLeftSpace
                      << " remainingSeconds=" << remainingSeconds
                      << " plannedSpeed=" << plannedSpeed
                      << "\n";
        }
        return ret;
    }

    // VARIANT_15
    if (roundaboutEdgesAhead > 1) {
        // try to use the inner lanes of a roundabout to increase throughput
        // unless we are approaching the exit
        if (left) {
            ret |= LCA_COOPERATIVE;
        } else {
            ret |= LCA_STAY | LCA_COOPERATIVE;
        }
        if (!cancelRequest(ret)) {
            if ((ret & LCA_STAY) == 0) {
                latDist = latLaneDist;
                blocked = checkBlocking(neighLane, latDist, laneOffset,
                                        leaders, followers, blockers,
                                        neighLeaders, neighFollowers, neighBlockers);
            }
            return ret;
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
    const SUMOReal inconvenience = (latLaneDist < 0
                                    ? -mySpeedGainProbabilityRight / myChangeProbThresholdRight
                                    : -mySpeedGainProbabilityLeft / myChangeProbThresholdLeft);
    if (laneOffset != 0
            && amBlockingFollowerPlusNB()
            && (inconvenience < myCooperativeParam)
            //&& ((myOwnState & myLcaCounter) == 0) // VARIANT_6 : counterNoHelp
            && (changeToBest || currentDistAllows(neighDist, abs(bestLaneOffset) + 1, laDist))) {

        // VARIANT_2 (nbWhenChangingToHelp)
        if (gDebugFlag2) {
            std::cout << STEPS2TIME(currentTime)
                      << " veh=" << myVehicle.getID()
                      << " wantsChangeToHelp=" << (right ? "right" : "left")
                      << " state=" << myOwnState
                      << (((myOwnState & myLcaCounter) != 0) ? " (counter)" : "")
                      << "\n";
        }

        ret |= LCA_COOPERATIVE | LCA_URGENT ;//| LCA_CHANGE_TO_HELP;
        if (!cancelRequest(ret)) {
            latDist = latLaneDist;
            blocked = checkBlocking(neighLane, latDist, laneOffset,
                                    leaders, followers, blockers,
                                    neighLeaders, neighFollowers, neighBlockers);
            return ret;
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

    // iterate over all possible combinations of sublanes this vehicle might cover and check the potential speed
    const MSEdge& edge = myVehicle.getLane()->getEdge();
    const std::vector<SUMOReal>& sublaneSides = edge.getSubLaneSides();
    assert(sublaneSides.size() == myExpectedSublaneSpeeds.size());
    const SUMOReal vehWidth = myVehicle.getVehicleType().getWidth();
    const SUMOReal rightVehSide = myVehicle.getRightSideOnEdge();
    const SUMOReal leftVehSide = rightVehSide + vehWidth;
    // figure out next speed when staying where we are
    SUMOReal defaultNextSpeed = std::numeric_limits<SUMOReal>::max();
    /// determine the leftmost and rightmost sublanes currently occupied
    int leftmostOnEdge = (int)sublaneSides.size() - 1;
    while (leftmostOnEdge > 0 && sublaneSides[leftmostOnEdge] > leftVehSide) {
        leftmostOnEdge--;
    }
    int rightmostOnEdge = leftmostOnEdge;
    while (rightmostOnEdge > 0 && sublaneSides[rightmostOnEdge] > rightVehSide + NUMERICAL_EPS) {
        defaultNextSpeed = MIN2(defaultNextSpeed, myExpectedSublaneSpeeds[rightmostOnEdge]);
        if (gDebugFlag2) {
            std::cout << "   adapted to current sublane=" << rightmostOnEdge << " defaultNextSpeed=" << defaultNextSpeed << "\n";
        }
        if (gDebugFlag2) {
            std::cout << "   sublaneSides[rightmostOnEdge]=" << sublaneSides[rightmostOnEdge] << " rightVehSide=" << rightVehSide << "\n";
        }
        rightmostOnEdge--;
    }
    defaultNextSpeed = MIN2(defaultNextSpeed, myExpectedSublaneSpeeds[rightmostOnEdge]);
    if (gDebugFlag2) {
        std::cout << "   adapted to current sublane=" << rightmostOnEdge << " defaultNextSpeed=" << defaultNextSpeed << "\n";
    }
    if (gDebugFlag2) {
        std::cout << "   sublaneSides[rightmostOnEdge]=" << sublaneSides[rightmostOnEdge] << " rightVehSide=" << rightVehSide << "\n";
    }
    SUMOReal maxGain = -std::numeric_limits<SUMOReal>::max();
    SUMOReal maxGainRight = -std::numeric_limits<SUMOReal>::max();
    SUMOReal maxGainLeft = -std::numeric_limits<SUMOReal>::max();
    SUMOReal latDistNice = std::numeric_limits<SUMOReal>::max();

    const int iMin = MIN2(myVehicle.getLane()->getRightmostSublane(), neighLane.getRightmostSublane());
    const SUMOReal leftMax = MAX2(
                                 myVehicle.getLane()->getRightSideOnEdge() + myVehicle.getLane()->getWidth(),
                                 neighLane.getRightSideOnEdge() + neighLane.getWidth());
    assert(leftMax <= edge.getWidth());
    int sublaneCompact = MAX2(iMin, rightmostOnEdge - 1); // try to compactify to the right by default

    if (gDebugFlag2) std::cout
                << "  checking sublanes rightmostOnEdge=" << rightmostOnEdge
                << " leftmostOnEdge=" << leftmostOnEdge
                << " iMin=" << iMin
                << " leftMax=" << leftMax
                << " sublaneCompact=" << sublaneCompact
                << "\n";
    for (int i = iMin; i < (int)sublaneSides.size(); ++i) {
        if (sublaneSides[i] + vehWidth < leftMax) {
            // i is the rightmost sublane and the left side of vehicles still fits on the edge,
            // compute min speed of all sublanes covered by the vehicle in this case
            SUMOReal vMin = myExpectedSublaneSpeeds[i];
            //std::cout << "   i=" << i << "\n";
            int j = i;
            while (vMin > 0 && j < (int)sublaneSides.size() && sublaneSides[j] < sublaneSides[i] + vehWidth) {
                vMin = MIN2(vMin, myExpectedSublaneSpeeds[j]);
                //std::cout << "     j=" << j << " vMin=" << vMin << " sublaneSides[j]=" << sublaneSides[j] << " leftVehSide=" << leftVehSide << " rightVehSide=" << rightVehSide << "\n";
                ++j;
            }
            const SUMOReal relativeGain = (vMin - defaultNextSpeed) / MAX2(vMin, RELGAIN_NORMALIZATION_MIN_SPEED);
            // @note this is biased for changing to the left since we compare the sublanes in ascending order
            if (relativeGain > GAIN_PERCEPTION_THRESHOLD && relativeGain > maxGain) {
                maxGain = relativeGain;
                sublaneCompact = i;
                if (maxGain > 0) {
                    latDist = sublaneSides[i] - rightVehSide;
                    if (gDebugFlag2) {
                        std::cout << "      i=" << i << " newLatDist=" << latDist << " relGain=" << relativeGain << "\n";
                    }
                }
            }
            if (gDebugFlag2) {
                std::cout << "    i=" << i << " rightmostOnEdge=" << rightmostOnEdge << " vMin=" << vMin << " relGain=" << relativeGain << " sublaneCompact=" << sublaneCompact << "\n";
            }
            if (i < rightmostOnEdge) {
                maxGainRight = MAX2(maxGainRight, relativeGain);
            } else if (i > rightmostOnEdge) {
                maxGainLeft = MAX2(maxGainLeft, relativeGain);
            }
            const SUMOReal subAlignDist = sublaneSides[i] - rightVehSide;
            if (fabs(subAlignDist) < fabs(latDistNice)) {
                latDistNice = subAlignDist;
                if (gDebugFlag2) std::cout
                            << "    nicest sublane=" << i
                            << " side=" << sublaneSides[i]
                            << " rightSide=" << rightVehSide
                            << " latDistNice=" << latDistNice
                            << "\n";
            }
        }
    }
    // updated change probabilities
    if (maxGainRight != -std::numeric_limits<SUMOReal>::max()) {
        mySpeedGainProbabilityRight += TS * maxGainRight;
    }
    if (maxGainLeft != -std::numeric_limits<SUMOReal>::max()) {
        mySpeedGainProbabilityLeft += TS * maxGainLeft;
    }
    // decay
    if (maxGainRight < NUMERICAL_EPS) {
        mySpeedGainProbabilityRight *= pow(0.5, TS);
    }
    if (maxGainLeft < NUMERICAL_EPS) {
        mySpeedGainProbabilityLeft *= pow(0.5, TS);
    }


    if (gDebugFlag2) std::cout << SIMTIME
                                   << " veh=" << myVehicle.getID()
                                   << " defaultNextSpeed=" << defaultNextSpeed
                                   << " maxGain=" << maxGain
                                   << " maxGainRight=" << maxGainRight
                                   << " maxGainLeft=" << maxGainLeft
                                   << " latDist=" << latDist
                                   << " latDistNice=" << latDistNice
                                   << " sublaneCompact=" << sublaneCompact
                                   << "\n";

    if (!left) {
        // ONLY FOR CHANGING TO THE RIGHT
        if (right && maxGain >= 0 && latDist <= 0) {
            // honor the obligation to keep right (Rechtsfahrgebot)
            // XXX consider fast approaching followers on the current lane
            //const SUMOReal vMax = myLookAheadSpeed;
            const SUMOReal vMax = myVehicle.getLane()->getVehicleMaxSpeed(&myVehicle);
            const SUMOReal acceptanceTime = KEEP_RIGHT_ACCEPTANCE * vMax * MAX2((SUMOReal)1, myVehicle.getSpeed()) / myVehicle.getLane()->getSpeedLimit();
            SUMOReal fullSpeedGap = MAX2((SUMOReal)0, neighDist - myVehicle.getCarFollowModel().brakeGap(vMax));
            SUMOReal fullSpeedDrivingSeconds = MIN2(acceptanceTime, fullSpeedGap / vMax);
            CLeaderDist neighLead = getSlowest(neighLeaders);
            if (neighLead.first != 0 && neighLead.first->getSpeed() < vMax) {
                fullSpeedGap = MAX2((SUMOReal)0, MIN2(fullSpeedGap,
                                                      neighLead.second - myVehicle.getCarFollowModel().getSecureGap(
                                                              vMax, neighLead.first->getSpeed(), neighLead.first->getCarFollowModel().getMaxDecel())));
                fullSpeedDrivingSeconds = MIN2(fullSpeedDrivingSeconds, fullSpeedGap / (vMax - neighLead.first->getSpeed()));
            }
            const SUMOReal deltaProb = (myChangeProbThresholdRight
                                        * STEPS2TIME(DELTA_T)
                                        * (fullSpeedDrivingSeconds / acceptanceTime) / KEEP_RIGHT_TIME);
            myKeepRightProbability += TS * deltaProb;

            if (gDebugFlag2) {
                std::cout << STEPS2TIME(currentTime)
                          << " considering keepRight:"
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
                          << " keepRight=" << myKeepRightProbability
                          << " speedGainL=" << mySpeedGainProbabilityLeft
                          << "\n";
            }
            if (myKeepRightProbability * myKeepRightParam > MAX2(myChangeProbThresholdRight, mySpeedGainProbabilityLeft)) {
                ret |= LCA_KEEPRIGHT;
                assert(myVehicle.getLane()->getIndex() > neighLane.getIndex());
                if (!cancelRequest(ret)) {
                    latDist = latLaneDist;
                    blocked = checkBlocking(neighLane, latDist, laneOffset,
                                            leaders, followers, blockers,
                                            neighLeaders, neighFollowers, neighBlockers);
                    return ret;
                }
            }
        }

        if (gDebugFlag2) {
            std::cout << STEPS2TIME(currentTime)
                      << " speedGainR=" << mySpeedGainProbabilityRight
                      << " latDist=" << latDist
                      << "\n";
        }

        if (latDist < 0 && mySpeedGainProbabilityRight >= MAX2(myChangeProbThresholdRight, mySpeedGainProbabilityLeft)
                && neighDist / MAX2((SUMOReal) .1, myVehicle.getSpeed()) > 20.) {
            ret |= LCA_SPEEDGAIN;
            if (!cancelRequest(ret)) {
                blocked = checkBlocking(neighLane, latDist, laneOffset,
                                        leaders, followers, blockers,
                                        neighLeaders, neighFollowers, neighBlockers);
                return ret;
            }
        }
    }
    if (!right) {

        const bool stayInLane = myVehicle.getLateralPositionOnLane() + latDist < 0.5 * myVehicle.getLane()->getWidth();
        if (gDebugFlag2) {
            std::cout << STEPS2TIME(currentTime)
                      << " speedGainL=" << mySpeedGainProbabilityLeft
                      << " latDist=" << latDist
                      << " stayInLane=" << stayInLane
                      << "\n";
        }

        if (latDist > 0 && mySpeedGainProbabilityLeft > myChangeProbThresholdLeft &&
                // if we leave our lane, we should be able to stay in the new
                // lane for some time
                (stayInLane || neighDist / MAX2((SUMOReal) .1, myVehicle.getSpeed()) > SPEED_GAIN_MIN_SECONDS)) {
            ret |= LCA_SPEEDGAIN;
            if (!cancelRequest(ret)) {
                blocked = checkBlocking(neighLane, latDist, laneOffset,
                                        leaders, followers, blockers,
                                        neighLeaders, neighFollowers, neighBlockers);
                return ret;
            }
        }
    }

    // factor in preferred lateral alignment (unless we are in the middle of an unfinished non-alignment maneuver)
    if (fabs(latDist) <= NUMERICAL_EPS && (myCanChangeFully || (myPreviousState | LCA_SUBLANE) != 0)) {
        const SUMOReal halfLaneWidth = myVehicle.getLane()->getWidth() * 0.5;
        const SUMOReal halfVehWidth = myVehicle.getVehicleType().getWidth() * 0.5;
        if (myVehicle.getParameter().arrivalPosLatProcedure != ARRIVAL_POSLAT_DEFAULT
                && myVehicle.getRoute().getLastEdge() == &myVehicle.getLane()->getEdge()
                && bestLaneOffset == 0
                && (myVehicle.getArrivalPos() - myVehicle.getPositionOnLane()) < ARRIVALPOS_LAT_THRESHOLD) {
            // vehicle is on its final edge, on the correct lane and close to
            // its arrival position. Change to the desired lateral position
            switch (myVehicle.getParameter().arrivalPosLatProcedure) {
                case ARRIVAL_POSLAT_GIVEN:
                    latDist = myVehicle.getParameter().arrivalPosLat - myVehicle.getLateralPositionOnLane();
                    break;
                case ARRIVAL_POSLAT_RIGHT:
                    latDist = -halfLaneWidth + halfVehWidth - myVehicle.getLateralPositionOnLane();
                    break;
                case ARRIVAL_POSLAT_CENTER:
                    latDist = -myVehicle.getLateralPositionOnLane();
                    break;
                case ARRIVAL_POSLAT_LEFT:
                    latDist = halfLaneWidth - halfVehWidth - myVehicle.getLateralPositionOnLane();
                    break;
                default:
                    assert(false);
            }
            if (gDebugFlag2) std::cout << SIMTIME
                                           << " arrivalPosLatProcedure=" << myVehicle.getParameter().arrivalPosLatProcedure
                                           << " arrivalPosLat=" << myVehicle.getParameter().arrivalPosLat << "\n";

        } else {
            switch (myVehicle.getVehicleType().getPreferredLateralAlignment()) {
                case LATALIGN_RIGHT:
                    latDist = -halfLaneWidth + halfVehWidth - myVehicle.getLateralPositionOnLane();
                    break;
                case LATALIGN_LEFT:
                    latDist = halfLaneWidth - halfVehWidth - myVehicle.getLateralPositionOnLane();
                    break;
                case LATALIGN_CENTER:
                    latDist = -myVehicle.getLateralPositionOnLane();
                    break;
                case LATALIGN_NICE:
                    latDist = latDistNice;
                    break;
                case LATALIGN_COMPACT:
                    latDist = sublaneSides[sublaneCompact] - rightVehSide;
                    break;
                case LATALIGN_ARBITRARY:
                    break;
            }
        }
        if (gDebugFlag2) std::cout << SIMTIME
                                       << " alignment=" << toString(myVehicle.getVehicleType().getPreferredLateralAlignment())
                                       << " mySpeedGainR=" << mySpeedGainProbabilityRight
                                       << " mySpeedGainL=" << mySpeedGainProbabilityLeft
                                       << " latDist=" << latDist
                                       << " myCanChangeFully=" << myCanChangeFully
                                       << " prevState=" << toString((LaneChangeAction)myPreviousState)
                                       << "\n";
        if ((latDist < 0 && mySpeedGainProbabilityRight < mySpeedLossProbThreshold)
                || (latDist > 0 && mySpeedGainProbabilityLeft < mySpeedLossProbThreshold)) {
            // do not risk losing speed
            latDist = 0;
        }
        // XXX first compute preferred adaptation and then override with speed
        // (this way adaptation is still done if changing for speedgain is
        // blocked)
        if (fabs(latDist) >= NUMERICAL_EPS) {
            if (gDebugFlag2) std::cout << SIMTIME
                                           << " adapting to preferred alignment=" << toString(myVehicle.getVehicleType().getPreferredLateralAlignment())
                                           << " latDist=" << latDist
                                           << "\n";
            ret |= LCA_SUBLANE;
            if (!cancelRequest(ret)) {
                blocked = checkBlocking(neighLane, latDist, laneOffset,
                                        leaders, followers, blockers,
                                        neighLeaders, neighFollowers, neighBlockers);
                return ret;
            }
        } else {
            latDist = 0;
            ret |= LCA_SUBLANE | LCA_STAY;
        }
    }


    // --------
    /*
    if (changeToBest && bestLaneOffset == curr.bestLaneOffset && laneOffset != 0
            && (right
                ? mySpeedGainProbabilityRight > MAX2((SUMOReal)0, mySpeedGainProbabilityLeft)
                : mySpeedGainProbabilityLeft  > MAX2((SUMOReal)0, mySpeedGainProbabilityRight))) {
        // change towards the correct lane, speedwise it does not hurt
        ret |= LCA_STRATEGIC;
        if (!cancelRequest(ret)) {
            latDist = latLaneDist;
            blocked = checkBlocking(neighLane, latDist, laneOffset,
                    leaders, followers, blockers,
                    neighLeaders, neighFollowers, neighBlockers);
            return ret;
        }
    }
    */
    if (gDebugFlag2) {
        std::cout << STEPS2TIME(currentTime)
                  << " veh=" << myVehicle.getID()
                  << " mySpeedGainR=" << mySpeedGainProbabilityRight
                  << " mySpeedGainL=" << mySpeedGainProbabilityLeft
                  << " myKeepRight=" << myKeepRightProbability
                  << "\n";
    }
    return ret;
}


int
MSLCM_SL2015::slowDownForBlocked(MSVehicle** blocked, int state) {
    //  if this vehicle is blocking someone in front, we maybe decelerate to let him in
    if ((*blocked) != 0) {
        SUMOReal gap = (*blocked)->getPositionOnLane() - (*blocked)->getVehicleType().getLength() - myVehicle.getPositionOnLane() - myVehicle.getVehicleType().getMinGap();
        if (gDebugFlag2) {
            std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                      << " veh=" << myVehicle.getID()
                      << " blocked=" << Named::getIDSecure(*blocked)
                      << " gap=" << gap
                      << "\n";
        }
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
            }
        }
    }
    return state;
}


void
MSLCM_SL2015::saveBlockerLength(const MSVehicle* blocker, int lcaCounter) {
    if (gDebugFlag2) {
        std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                  << " veh=" << myVehicle.getID()
                  << " saveBlockerLength blocker=" << Named::getIDSecure(blocker)
                  << " bState=" << (blocker == 0 ? "None" : toString(blocker->getLaneChangeModel().getOwnState()))
                  << "\n";
    }
    if (blocker != 0 && (blocker->getLaneChangeModel().getOwnState() & lcaCounter) != 0) {
        // is there enough space in front of us for the blocker?
        const SUMOReal potential = myLeftSpace - myVehicle.getCarFollowModel().brakeGap(
                                       myVehicle.getSpeed(), myVehicle.getCarFollowModel().getMaxDecel(), 0);
        if (blocker->getVehicleType().getLengthWithGap() <= potential) {
            // save at least his length in myLeadingBlockerLength
            myLeadingBlockerLength = MAX2(blocker->getVehicleType().getLengthWithGap(), myLeadingBlockerLength);
            if (gDebugFlag2) {
                std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                          << " veh=" << myVehicle.getID()
                          << " blocker=" << Named::getIDSecure(blocker)
                          << " saving myLeadingBlockerLength=" << myLeadingBlockerLength
                          << "\n";
            }
        } else {
            // we cannot save enough space for the blocker. It needs to save
            // space for ego instead
            if (gDebugFlag2) {
                std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                          << " veh=" << myVehicle.getID()
                          << " blocker=" << Named::getIDSecure(blocker)
                          << " cannot save space=" << blocker->getVehicleType().getLengthWithGap()
                          << " potential=" << potential
                          << "\n";
            }
            ((MSVehicle*)blocker)->getLaneChangeModel().saveBlockerLength(myVehicle.getVehicleType().getLengthWithGap());
        }
    }
}


void
MSLCM_SL2015::updateExpectedSublaneSpeeds(const MSLeaderInfo& ahead, int sublaneOffset, int laneIndex) {
    const std::vector<MSLane*>& lanes = myVehicle.getLane()->getEdge().getLanes();
    const std::vector<MSVehicle::LaneQ>& preb = myVehicle.getBestLanes();
    const MSLane* lane = lanes[laneIndex];
    const SUMOReal vMax = lane->getVehicleMaxSpeed(&myVehicle);
    assert(preb.size() == lanes.size());

    for (int sublane = 0; sublane < (int)ahead.numSublanes(); ++sublane) {
        const int edgeSublane = sublane + sublaneOffset;
        if (lane->allowsVehicleClass(myVehicle.getVehicleType().getVehicleClass())) {
            // lane allowed, find potential leaders and compute safe speeds
            // XXX anticipate future braking if leader has a lower speed than myVehicle
            const MSVehicle* leader = ahead[sublane];
            SUMOReal vSafe;
            if (leader == 0) {
                vSafe = MIN2(vMax, myCarFollowModel.followSpeed(&myVehicle, vMax, preb[laneIndex].length, 0, 0));
            } else {
                const SUMOReal gap = leader->getBackPositionOnLane(lane) - myVehicle.getPositionOnLane() - myVehicle.getVehicleType().getMinGap();
                vSafe = MIN2(vMax, myCarFollowModel.followSpeed(
                                 &myVehicle, vMax, gap, leader->getSpeed(), leader->getCarFollowModel().getMaxDecel()));
            }
            // XXX calibrate weightFactor?
            const SUMOReal memoryFactor = 0.5;
            myExpectedSublaneSpeeds[edgeSublane] = memoryFactor * myExpectedSublaneSpeeds[edgeSublane] + (1 - memoryFactor) * vSafe;
        } else {
            // lane forbidden
            myExpectedSublaneSpeeds[edgeSublane] = -1;
        }
    }
    // XXX deal with leaders on subsequent lanes based on preb
}


CLeaderDist
MSLCM_SL2015::getLongest(const MSLeaderDistanceInfo& ldi) {
    int iMax = 0;
    SUMOReal maxLength = -1;
    for (int i = 0; i < ldi.numSublanes(); ++i) {
        if (ldi[i].first != 0) {
            const SUMOReal length = ldi[i].first->getVehicleType().getLength();
            if (length > maxLength) {
                maxLength = length;
                iMax = i;
            }
        }
    }
    return ldi[iMax];
}


CLeaderDist
MSLCM_SL2015::getSlowest(const MSLeaderDistanceInfo& ldi) {
    int iMax = 0;
    SUMOReal minSpeed = std::numeric_limits<SUMOReal>::max();
    for (int i = 0; i < ldi.numSublanes(); ++i) {
        if (ldi[i].first != 0) {
            const SUMOReal speed = ldi[i].first->getSpeed();
            if (speed < minSpeed) {
                minSpeed = speed;
                iMax = i;
            }
        }
    }
    return ldi[iMax];
}


int
MSLCM_SL2015::checkBlocking(const MSLane& neighLane, SUMOReal& latDist, int laneOffset,
                            const MSLeaderDistanceInfo& leaders,
                            const MSLeaderDistanceInfo& followers,
                            const MSLeaderDistanceInfo& /*blockers */,
                            const MSLeaderDistanceInfo& neighLeaders,
                            const MSLeaderDistanceInfo& neighFollowers,
                            const MSLeaderDistanceInfo& /* neighBlockers */,
                            std::vector<CLeaderDist>* collectLeadBlockers,
                            std::vector<CLeaderDist>* collectFollowBlockers,
                            bool keepLatGapManeuver) {
    // truncate latDist according to maxSpeedLat
    if (!keepLatGapManeuver) {
        myOrigLatDist = latDist;
    }
    const SUMOReal maxDist = SPEED2DIST(myVehicle.getVehicleType().getMaxSpeedLat());
    latDist = MAX2(MIN2(latDist, maxDist), -maxDist);

    // reduce latDist to avoid blockage with overlapping vehicles
    const SUMOReal halfWidth = myVehicle.getVehicleType().getWidth() * 0.5;
    const SUMOReal center = myVehicle.getCenterOnEdge();
    SUMOReal surplusGapRight = MIN2(maxDist, center - halfWidth);
    SUMOReal surplusGapLeft = MIN2(maxDist, myVehicle.getLane()->getEdge().getWidth() - center - halfWidth);
    updateGaps(leaders, myVehicle.getLane()->getRightSideOnEdge(), center, 0, surplusGapRight, surplusGapLeft);
    updateGaps(followers, myVehicle.getLane()->getRightSideOnEdge(), center, 0, surplusGapRight, surplusGapLeft);
    if (laneOffset != 0) {
        updateGaps(neighLeaders, neighLane.getRightSideOnEdge(), center, 0, surplusGapRight, surplusGapLeft);
        updateGaps(neighFollowers, neighLane.getRightSideOnEdge(), center, 0, surplusGapRight, surplusGapLeft);
    }
    if (gDebugFlag2) {
        std::cout << "    checkBlocking latDist=" << latDist << " surplusGapRight=" << surplusGapRight << " surplusGapLeft=" << surplusGapLeft << "\n";
    }
    if (latDist < 0) {
        if (surplusGapRight <= 0) {
            return LCA_BLOCKED_RIGHT | LCA_OVERLAPPING;
        } else {
            latDist = MAX2(latDist, -surplusGapRight);
        }
    } else {
        if (surplusGapLeft <= 0) {
            return LCA_BLOCKED_LEFT | LCA_OVERLAPPING;
        } else {
            latDist = MIN2(latDist, surplusGapLeft);
        }
    }

    myCanChangeFully = (myOrigLatDist == 0 || latDist == myOrigLatDist);
    if (gDebugFlag2) {
        std::cout << "    checkBlocking fully=" << myCanChangeFully << " latDist=" << latDist << " origLatDist=" << myOrigLatDist << "\n";
    }
    // destination sublanes must be safe
    // intermediate sublanes must not be blocked by overlapping vehicles

    // XXX avoid checking the same leader multiple times
    // XXX ensure that only changes within the same lane are undertaken if laneOffset = 0

    int blocked = 0;
    blocked |= checkBlockingVehicles(&myVehicle, leaders, latDist, myVehicle.getLane()->getRightSideOnEdge(), true, LCA_BLOCKED_BY_LEADER, collectLeadBlockers);
    blocked |= checkBlockingVehicles(&myVehicle, followers, latDist, myVehicle.getLane()->getRightSideOnEdge(), false, LCA_BLOCKED_BY_FOLLOWER, collectFollowBlockers);
    if (laneOffset != 0) {
        blocked |= checkBlockingVehicles(&myVehicle, neighLeaders, latDist, neighLane.getRightSideOnEdge(), true,
                                         (laneOffset == -1 ? LCA_BLOCKED_BY_RIGHT_LEADER : LCA_BLOCKED_BY_LEFT_LEADER), collectLeadBlockers);
        blocked |= checkBlockingVehicles(&myVehicle, neighFollowers, latDist, neighLane.getRightSideOnEdge(), false,
                                         (laneOffset == -1 ? LCA_BLOCKED_BY_RIGHT_FOLLOWER : LCA_BLOCKED_BY_LEFT_FOLLOWER), collectFollowBlockers);
    }

    if (blocked == 0 && !myCanChangeFully && myPushy == 0 && !keepLatGapManeuver) {
        // aggressive drivers immediately start moving towards potential
        // blockers and only check that the start of their maneuver (latDist) is safe. In
        // contrast, cautious drivers need to check latDist and origLatDist to
        // ensure that the maneuver can be finished without encroaching on other vehicles.
        blocked |= checkBlockingVehicles(&myVehicle, leaders, myOrigLatDist, myVehicle.getLane()->getRightSideOnEdge(), true, LCA_BLOCKED_BY_LEADER, collectLeadBlockers);
        blocked |= checkBlockingVehicles(&myVehicle, followers, myOrigLatDist, myVehicle.getLane()->getRightSideOnEdge(), false, LCA_BLOCKED_BY_FOLLOWER, collectFollowBlockers);
        if (laneOffset != 0) {
            blocked |= checkBlockingVehicles(&myVehicle, neighLeaders, myOrigLatDist, neighLane.getRightSideOnEdge(), true,
                                             (laneOffset == -1 ? LCA_BLOCKED_BY_RIGHT_LEADER : LCA_BLOCKED_BY_LEFT_LEADER), collectLeadBlockers);
            blocked |= checkBlockingVehicles(&myVehicle, neighFollowers, myOrigLatDist, neighLane.getRightSideOnEdge(), false,
                                             (laneOffset == -1 ? LCA_BLOCKED_BY_RIGHT_FOLLOWER : LCA_BLOCKED_BY_LEFT_FOLLOWER), collectFollowBlockers);
        }
    }
    if (collectFollowBlockers != 0 && collectLeadBlockers != 0) {
        // prevent vehicles from being classified as leader and follower simultaneously
        for (std::vector<CLeaderDist>::const_iterator it2 = collectLeadBlockers->begin(); it2 != collectLeadBlockers->end(); ++it2) {
            for (std::vector<CLeaderDist>::iterator it = collectFollowBlockers->begin(); it != collectFollowBlockers->end();) {
                if ((*it2).first == (*it).first) {
                    if (gDebugFlag2) {
                        std::cout << "    removed follower " << (*it).first->getID() << " because it is already a leader\n";
                    }
                    it = collectFollowBlockers->erase(it);
                } else {
                    ++it;
                }
            }
        }
    }
    return blocked;
}


int
MSLCM_SL2015::checkBlockingVehicles(
    const MSVehicle* ego, const MSLeaderDistanceInfo& vehicles,
    SUMOReal latDist, SUMOReal foeOffset, bool leaders, LaneChangeAction blockType,
    std::vector<CLeaderDist>* collectBlockers) const {
    // determine borders where safety/no-overlap conditions must hold
    const SUMOReal vehWidth = ego->getVehicleType().getWidth();
    const SUMOReal rightVehSide = ego->getRightSideOnEdge();
    const SUMOReal leftVehSide = rightVehSide + vehWidth;
    const SUMOReal rightVehSideDest = rightVehSide + latDist;
    const SUMOReal leftVehSideDest = leftVehSide + latDist;
    const SUMOReal rightNoOverlap = MIN2(rightVehSideDest, rightVehSide);
    const SUMOReal leftNoOverlap = MAX2(leftVehSideDest, leftVehSide);

    int result = 0;
    for (int i = 0; i < vehicles.numSublanes(); ++i) {
        CLeaderDist vehDist = vehicles[i];
        if (vehDist.first != 0) {
            // only check the current stripe occuped by foe (transform into edge-coordinates)
            const SUMOReal foeRight = i * MSGlobals::gLateralResolution + foeOffset;
            const SUMOReal foeLeft = foeRight + MSGlobals::gLateralResolution;
            if (gDebugFlag2 && false) {
                const MSVehicle* leader = vehDist.first;
                const MSVehicle* follower = ego;
                if (!leaders) {
                    std::swap(leader, follower);
                }
                std::cout << "  checkBlocking"
                          << " leaders=" << leaders
                          << " foe=" << vehDist.first->getID()
                          << " gap=" << vehDist.second
                          << " secGap=" << follower->getCarFollowModel().getSecureGap(follower->getSpeed(), leader->getSpeed(), leader->getCarFollowModel().getMaxDecel())
                          << " foeRight=" << foeRight
                          << " foeLeft=" << foeLeft
                          << " rightNoOverlap=" << rightNoOverlap
                          << " leftNoOverlap=" << leftNoOverlap
                          << " rightVehSideDest=" << rightVehSideDest
                          << " leftVehSideDest=" << leftVehSideDest
                          << " overlap=" << overlap(rightNoOverlap, leftNoOverlap, foeRight, foeLeft)
                          << " overlapDest=" << overlap(rightVehSideDest, leftVehSideDest, foeRight, foeLeft)
                          << "\n";
            }
            if (overlap(rightNoOverlap, leftNoOverlap, foeRight, foeLeft)) {
                if (vehDist.second < 0) {
                    if (gDebugFlag2) {
                        std::cout << "    overlap\n";
                    }
                    result |= (blockType | LCA_OVERLAPPING);
                    if (collectBlockers == 0) {
                        return result;
                    } else {
                        collectBlockers->push_back(vehDist);
                    }
                } else if (overlap(rightVehSideDest, leftVehSideDest, foeRight, foeLeft)) {
                    const MSVehicle* leader = vehDist.first;
                    const MSVehicle* follower = ego;
                    if (!leaders) {
                        std::swap(leader, follower);
                    }
                    const SUMOReal decelFactor = 1 + ego->getImpatience() * myAssertive;
                    // see MSCFModel::getSecureGap
                    // for decelFactor == 1 this is equivalent to
                    // follower->getCarFollowModel().getSecureGap(follower->getSpeed(), leader->getSpeed(), leader->getCarFollowModel().getMaxDecel())
                    const SUMOReal followDecel = MIN2(follower->getCarFollowModel().getMaxDecel(), leader->getCarFollowModel().getMaxDecel()) * decelFactor;
                    const SUMOReal secureGap = MAX2((SUMOReal) 0, MSCFModel::brakeGap(follower->getSpeed(), followDecel, follower->getCarFollowModel().getHeadwayTime())
                                                    - MSCFModel::brakeGap(leader->getSpeed(), leader->getCarFollowModel().getMaxDecel(), 0));
                    if (vehDist.second < secureGap) {
                        if (gDebugFlag2) {
                            std::cout << "    blocked decelFactor=" << decelFactor << "\n";
                        }
                        result |= blockType;
                        if (collectBlockers == 0) {
                            return result;
                        } else {
                            collectBlockers->push_back(vehDist);
                        }
                    }
                }
            }
        }
    }
    return result;

}


bool
MSLCM_SL2015::overlap(SUMOReal right, SUMOReal left, SUMOReal right2, SUMOReal left2) {
    assert(right <= left);
    assert(right2 <= left2);
    return left2 >= right + NUMERICAL_EPS && left >= right2 + NUMERICAL_EPS;
}


MSLCM_SL2015::StateAndDist
MSLCM_SL2015::decideDirection(StateAndDist sd1, StateAndDist sd2) const {
    // ignore dummy decisions (returned if mayChange() failes)
    if (sd1.state == 0) {
        return sd2;
    } else if (sd2.state == 0) {
        return sd1;
    }
    // LCA_SUBLANE is special because LCA_STAY|LCA_SUBLANE may override another LCA_SUBLANE command
    const bool want1 = ((sd1.state & LCA_WANTS_LANECHANGE) != 0) || ((sd1.state & LCA_SUBLANE) != 0 && (sd1.state & LCA_STAY) != 0);
    const bool want2 = ((sd2.state & LCA_WANTS_LANECHANGE) != 0) || ((sd2.state & LCA_SUBLANE) != 0 && (sd2.state & LCA_STAY) != 0);
    const bool can1 = ((sd1.state & LCA_BLOCKED) == 0);
    if (DEBUG_COND) std::cout << SIMTIME
                                  << " veh=" << myVehicle.getID()
                                  << " state1=" << toString((LaneChangeAction)sd1.state)
                                  << " want1=" << (sd1.state & LCA_WANTS_LANECHANGE)
                                  << " dist1=" << sd1.latDist
                                  << " dir1=" << sd1.dir
                                  << " state2=" << toString((LaneChangeAction)sd2.state)
                                  << " want2=" << (sd2.state & LCA_WANTS_LANECHANGE)
                                  << " dist2=" << sd2.latDist
                                  << " dir2=" << sd2.dir
                                  << "\n";
    if (want1) {
        if (want2) {
            // decide whether right or left has higher priority (lower value in enum LaneChangeAction)
            if ((sd1.state & LCA_CHANGE_REASONS) < (sd2.state & LCA_CHANGE_REASONS)) {
                //if (DEBUG_COND) std::cout << "   " << (sd1.state & LCA_CHANGE_REASONS) << " < " << (sd2.state & LCA_CHANGE_REASONS) << "\n";
                return sd1;
            } else if ((sd1.state & LCA_CHANGE_REASONS) > (sd2.state & LCA_CHANGE_REASONS)) {
                //if (DEBUG_COND) std::cout << "   " << (sd1.state & LCA_CHANGE_REASONS) << " > " << (sd2.state & LCA_CHANGE_REASONS) << "\n";
                return sd2;
            } else {
                // same priority.
                if ((sd1.state & LCA_SUBLANE) != 0) {
                    // special treatment: prefer action with dir != 0
                    if (sd1.dir == 0) {
                        return sd2;
                    } else if (sd2.dir == 0) {
                        return sd1;
                    } else {
                        // prefer further right
                        return sd1.latDist <= sd2.latDist ? sd1 : sd2;
                    }
                } else {
                    // see which one is allowed
                    return can1 ? sd1 : sd2;
                }
            }
        } else {
            return sd1;
        }
    } else {
        return sd2;
    }

}


LaneChangeAction
MSLCM_SL2015::getLCA(int state, SUMOReal latDist) {
    return ((latDist == 0 || (state & LCA_CHANGE_REASONS) == 0)
            ? LCA_NONE : (latDist < 0 ? LCA_RIGHT : LCA_LEFT));
}


int
MSLCM_SL2015::checkStrategicChange(int ret,
                                   int laneOffset,
                                   const std::vector<MSVehicle::LaneQ>& preb,
                                   const MSLeaderDistanceInfo& leaders,
                                   const MSLeaderDistanceInfo& neighLeaders,
                                   int currIdx,
                                   int bestLaneOffset,
                                   bool changeToBest,
                                   int lcaCounter,
                                   SUMOReal currentDist,
                                   SUMOReal neighDist,
                                   SUMOReal laDist,
                                   int roundaboutEdgesAhead
                                  ) {
    const bool right = (laneOffset == -1);
    const bool left = (laneOffset == 1);
    const MSVehicle::LaneQ& curr = preb[currIdx];
    const MSVehicle::LaneQ& neigh = preb[currIdx + laneOffset];
    const MSVehicle::LaneQ& best = preb[currIdx + bestLaneOffset];

    const SUMOReal usableDist = (currentDist - myVehicle.getPositionOnLane() - best.occupation *  JAM_FACTOR);
    //- (best.lane->getVehicleNumber() * neighSpeed)); // VARIANT 9 jfSpeed
    const SUMOReal maxJam = MAX2(preb[currIdx + laneOffset].occupation, preb[currIdx].occupation);
    const SUMOReal neighLeftPlace = MAX2((SUMOReal) 0, neighDist - myVehicle.getPositionOnLane() - maxJam);

    if (gDebugFlag2) {
        std::cout << SIMTIME
                  << " veh=" << myVehicle.getID()
                  << " laSpeed=" << myLookAheadSpeed
                  << " laDist=" << laDist
                  << " currentDist=" << currentDist
                  << " usableDist=" << usableDist
                  << " bestLaneOffset=" << bestLaneOffset
                  << " best.length=" << best.length
                  << " maxJam=" << maxJam
                  << " neighLeftPlace=" << neighLeftPlace
                  << "\n";
    }

    if (laneOffset != 0 && changeToBest && bestLaneOffset == curr.bestLaneOffset
            && currentDistDisallows(usableDist, bestLaneOffset, laDist)) {
        /// @brief we urgently need to change lanes to follow our route
        ret |= LCA_STRATEGIC | LCA_URGENT;
    } else {
        // VARIANT_20 (noOvertakeRight)
        if (!myAllowOvertakingRight && left && !myVehicle.congested() && neighLeaders.hasVehicles()) {
            // check for slower leader on the left. we should not overtake but
            // rather move left ourselves (unless congested)
            // XXX only adapt as much as possible to get a lateral gap
            CLeaderDist cld = getSlowest(neighLeaders);
            const MSVehicle* nv = cld.first;
            if (nv->getSpeed() < myVehicle.getSpeed()) {
                const SUMOReal vSafe = myCarFollowModel.followSpeed(
                                           &myVehicle, myVehicle.getSpeed(), cld.second, nv->getSpeed(), nv->getCarFollowModel().getMaxDecel());
                myVSafes.push_back(vSafe);
                if (vSafe < myVehicle.getSpeed()) {
                    mySpeedGainProbabilityRight += TS * myChangeProbThresholdLeft / 3;
                }
                if (gDebugFlag2) {
                    std::cout << SIMTIME
                              << " avoid overtaking on the right nv=" << nv->getID()
                              << " nvSpeed=" << nv->getSpeed()
                              << " mySpeedGainProbabilityR=" << mySpeedGainProbabilityRight
                              << " plannedSpeed=" << myVSafes.back()
                              << "\n";
                }
            }
        }

        if (!changeToBest && (currentDistDisallows(neighLeftPlace, abs(bestLaneOffset) + 2, laDist))) {
            // the opposite lane-changing direction should be done than the one examined herein
            //  we'll check whether we assume we could change anyhow and get back in time...
            //
            // this rule prevents the vehicle from moving in opposite direction of the best lane
            //  unless the way till the end where the vehicle has to be on the best lane
            //  is long enough
            if (gDebugFlag2) {
                std::cout << " veh=" << myVehicle.getID() << " could not change back and forth in time (1) neighLeftPlace=" << neighLeftPlace << "\n";
            }
            ret |= LCA_STAY | LCA_STRATEGIC;
        } else if (laneOffset != 0 && bestLaneOffset == 0 && (neighLeftPlace * 2. < laDist)) {
            // the current lane is the best and a lane-changing would cause a situation
            //  of which we assume we will not be able to return to the lane we have to be on.
            // this rule prevents the vehicle from leaving the current, best lane when it is
            //  close to this lane's end
            if (gDebugFlag2) {
                std::cout << " veh=" << myVehicle.getID() << " could not change back and forth in time (2) neighLeftPlace=" << neighLeftPlace << "\n";
            }
            ret |= LCA_STAY | LCA_STRATEGIC;
        } else if (
            laneOffset != 0
            && bestLaneOffset == 0
            && !leaders.hasStoppedVehicle()
            && neigh.bestContinuations.back()->getLinkCont().size() != 0
            && roundaboutEdgesAhead == 0
            && neighDist < TURN_LANE_DIST) {
            // VARIANT_21 (stayOnBest)
            // we do not want to leave the best lane for a lane which leads elsewhere
            // unless our leader is stopped or we are approaching a roundabout
            if (gDebugFlag2) {
                std::cout << " veh=" << myVehicle.getID() << " does not want to leave the bestLane (neighDist=" << neighDist << ")\n";
            }
            ret |= LCA_STAY | LCA_STRATEGIC;
        } else if (right
                   && bestLaneOffset == 0
                   && myVehicle.getLane()->getSpeedLimit() > 80. / 3.6
                   && myLookAheadSpeed > SUMO_const_haltingSpeed
                  ) {
            // let's also regard the case where the vehicle is driving on a highway...
            //  in this case, we do not want to get to the dead-end of an on-ramp
            if (gDebugFlag2) {
                std::cout << " veh=" << myVehicle.getID() << " does not want to get stranded on the on-ramp of a highway\n";
            }
            ret |= LCA_STAY | LCA_STRATEGIC;
        }
    }
    // check for overriding TraCI requests
    if (gDebugFlag2) {
        std::cout << SIMTIME << " veh=" << myVehicle.getID() << " ret=" << ret;
    }
    ret = myVehicle.influenceChangeDecision(ret);
    if ((ret & lcaCounter) != 0) {
        // we are not interested in traci requests for the opposite direction here
        ret &= ~(LCA_TRACI | lcaCounter | LCA_URGENT);
    }
    if (gDebugFlag2) {
        std::cout << " reqAfterInfluence=" << ret << " ret=" << ret << "\n";
    }
    return ret;
}


int
MSLCM_SL2015::keepLatGap(int state,
                         const MSLeaderDistanceInfo& leaders,
                         const MSLeaderDistanceInfo& followers,
                         const MSLeaderDistanceInfo& blockers,
                         const MSLeaderDistanceInfo& neighLeaders,
                         const MSLeaderDistanceInfo& neighFollowers,
                         const MSLeaderDistanceInfo& neighBlockers,
                         const MSLane& neighLane,
                         int laneOffset,
                         SUMOReal& latDist,
                         int& blocked) {

    /* @notes
     * vehicles may need to compromise between fulfilling lane change objectives
     * (LCA_STRATEGIC, LCA_SPEED etc) and maintaining lateral gap. The minimum
     * acceptable lateral gap depends on
     * - the cultural context (China vs Europe)
     * - the driver agressiveness (willingness to encroach on other vehicles to force them to move laterally as well)
     *    - see @note in checkBlocking
     * - the vehicle type (car vs motorcycle)
     * - the current speed
     * - the speed difference
     * - the importance / urgency of the desired maneuver
     *
     * the object of this method is to evaluate the above circumstances and
     * either:
     * - allow the current maneuver (state, latDist)
     * - to override the current maneuver with a distance-keeping maneuver
     *
     *
     * laneChangeModel/driver parameters
     * - bool pushy (willingness to encroach)
     * - float minGap at 100km/h (to be interpolated for lower speeds (assume 0 at speed 0)
     * - gapFactors (a factor for each of the change reasons
     *
     * further assumptions
     * - the maximum of egoSpeed and deltaSpeed can be used when interpolating minGap
     * - distance keeping to the edges of the road can be ignored (for now)
     *
     * currentMinGap = minGap * min(1.0, max(v, abs(v - vOther)) / 100) * gapFactor[lc_reason]
     *
     * */

    /// XXX to be made configurable
    const SUMOReal gapFactor = (state & LCA_STRATEGIC) != 0 ? 0.0 : 1.0;
    const bool stayInLane = laneOffset == 0 || ((state & LCA_STRATEGIC) != 0 && (state & LCA_STAY) != 0);
    const SUMOReal oldLatDist = latDist;

    /// XXX todo
    // - compute lateral gap after executing the current maneuver (may be LCA_NONE)
    // - decide if override is needed
    //   - compute alternative maneuver to improve lateralGap
    //   - update blocking (checkBlocking)

    // compute gaps after maneuver
    const SUMOReal halfWidth = myVehicle.getVehicleType().getWidth() * 0.5;
    // if the current maneuver is blocked we will stay where we are
    const SUMOReal newCenter = myVehicle.getCenterOnEdge() + (blocked == 0 ? latDist : 0);
    // surplus gaps after the context-dependend value of currentMinGap has ben subtracted
    // if this value goes negative, we should override the current maneuver to better maintain distance
    SUMOReal surplusGapRight = newCenter - halfWidth;
    SUMOReal surplusGapLeft = myVehicle.getLane()->getEdge().getWidth() - newCenter - halfWidth;

    updateGaps(leaders, myVehicle.getLane()->getRightSideOnEdge(), newCenter, gapFactor, surplusGapRight, surplusGapLeft);
    updateGaps(followers, myVehicle.getLane()->getRightSideOnEdge(), newCenter, gapFactor, surplusGapRight, surplusGapLeft);
    if (laneOffset != 0) {
        updateGaps(neighLeaders, neighLane.getRightSideOnEdge(), newCenter, gapFactor, surplusGapRight, surplusGapLeft);
        updateGaps(neighFollowers, neighLane.getRightSideOnEdge(), newCenter, gapFactor, surplusGapRight, surplusGapLeft);
    }
    if (stayInLane) {
        // stay fully within the current lane
        const SUMOReal halfLaneWidth = myVehicle.getLane()->getWidth() * 0.5;
        surplusGapRight = MIN2(surplusGapRight, halfLaneWidth + myVehicle.getLateralPositionOnLane() - halfWidth);
        surplusGapLeft = MIN2(surplusGapLeft, halfLaneWidth - myVehicle.getLateralPositionOnLane() - halfWidth);
    }
    if (gDebugFlag2) {
        std::cout << "    keepLatGap laneOffset=" << laneOffset
                  << " latDist=" << latDist
                  << " gapFactor=" << gapFactor
                  << " stayInLane=" << stayInLane
                  << " surplusGapRight=" << surplusGapRight
                  << " surplusGapLeft=" << surplusGapLeft
                  << " state=" << toString((LaneChangeAction)state)
                  << " blockedBefore=" << toString((LaneChangeAction)blocked);
    }
    const SUMOReal maxDist = SPEED2DIST(myVehicle.getVehicleType().getMaxSpeedLat());
    if (surplusGapRight < -NUMERICAL_EPS) {
        if (surplusGapLeft > 0) {
            // move left to increase gap
            latDist = MIN3(latDist - surplusGapRight, latDist + surplusGapLeft, maxDist);
        } else {
            blocked |= LCA_OVERLAPPING | LCA_BLOCKED_RIGHT;
        }
    } else if (surplusGapLeft < -NUMERICAL_EPS) {
        if (surplusGapRight > 0) {
            // move right to increase gap
            latDist = MAX3(latDist + surplusGapLeft, latDist - surplusGapRight, -maxDist);
        } else {
            blocked |= LCA_OVERLAPPING | LCA_BLOCKED_LEFT;
        }
    }
    if (latDist != oldLatDist) {
        blocked = checkBlocking(neighLane, latDist, laneOffset, leaders, followers, blockers, neighLeaders, neighFollowers, neighBlockers, 0, 0, true);
    }
    if (latDist != 0) {
        state = (state & ~LCA_STAY);
    }
    if (gDebugFlag2) {
        std::cout << "    keepLatGap (checked)"
                  << " latDist2=" << latDist
                  << " blockedAfter=" << toString((LaneChangeAction)blocked)
                  << "\n";
    }
    return state;
}


void
MSLCM_SL2015::updateGaps(const MSLeaderDistanceInfo& others, SUMOReal foeOffset, SUMOReal newCenter, SUMOReal gapFactor, SUMOReal& surplusGapRight, SUMOReal& surplusGapLeft) const {
    if (others.hasVehicles()) {
        const SUMOReal halfWidth = myVehicle.getVehicleType().getWidth() * 0.5 + NUMERICAL_EPS;
        const SUMOReal baseMinGap = myVehicle.getVehicleType().getMinGapLat();
        for (int i = 0; i < others.numSublanes(); ++i) {
            if (others[i].first != 0 && others[i].second <= 0) {
                /// foe vehicle occupies full sublanes
                const MSVehicle* foe = others[i].first;
                const SUMOReal foeRight = i * MSGlobals::gLateralResolution + foeOffset;
                const SUMOReal foeLeft = foeRight + MSGlobals::gLateralResolution;
                const SUMOReal foeCenter = foeRight + 0.5 * MSGlobals::gLateralResolution;
                const SUMOReal gap = MIN2(fabs(foeRight - newCenter), fabs(foeLeft - newCenter)) - halfWidth;
                const SUMOReal currentMinGap = baseMinGap * MIN2(1.0, MAX2(myVehicle.getSpeed(), (SUMOReal)fabs(myVehicle.getSpeed() - foe->getSpeed())) / (100 / 3.6)) * gapFactor;
                if (gDebugFlag2 && false) std::cout << "  updateGaps"
                                                        << " foe=" << foe->getID()
                                                        << " foeRight=" << foeRight
                                                        << " foeLeft=" << foeLeft
                                                        << " gap=" << others[i].second
                                                        << " latgap=" << gap
                                                        << " currentMinGap=" << currentMinGap
                                                        << " surplusGapRight=" << surplusGapRight
                                                        << " surplusGapLeft=" << surplusGapLeft
                                                        << "\n";
                if (foeCenter < newCenter) {
                    surplusGapRight = MIN2(surplusGapRight, gap - currentMinGap);
                } else {
                    surplusGapLeft = MIN2(surplusGapLeft, gap - currentMinGap);
                }
            }
        }
    }
}


/****************************************************************************/
