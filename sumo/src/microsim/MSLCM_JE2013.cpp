/****************************************************************************/
/// @file    MSLCM_JE2013.h
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Fri, 08.10.2013
/// @version $Id$
///
// A lane change model developed by J. Erdmann
// based on the model of D. Krajzewicz developed between 2004 and 2011 (MSLCM_DK2004)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2013-2014 DLR (http://www.dlr.de/) and contributors
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
#include "MSEdge.h"
#include "MSLane.h"
#include "MSNet.h"
#include "MSLCM_JE2013.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

//#define DEBUG_VEHICLE_GUI_SELECTION 1
#ifdef DEBUG_VEHICLE_GUI_SELECTION
#include <utils/gui/div/GUIGlobalSelection.h>
#include <guisim/GUIVehicle.h>
#include <guisim/GUILane.h>
#endif



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
#define MIN_FALLBEHIND  (SUMOReal)(14.0 / 3.6)

#define KEEP_RIGHT_HEADWAY (SUMOReal)2.0

#define URGENCY (SUMOReal)2.0

#define ROUNDABOUT_DIST_BONUS (SUMOReal)100.0

#define CHANGE_PROB_THRESHOLD_RIGHT (SUMOReal)2.0
#define CHANGE_PROB_THRESHOLD_LEFT (SUMOReal)0.2
#define KEEP_RIGHT_TIME (SUMOReal)5.0 // the number of seconds after which a vehicle should move to the right lane
#define KEEP_RIGHT_ACCEPTANCE (SUMOReal)7.0 // calibration factor for determining the desire to keep right

#define RELGAIN_NORMALIZATION_MIN_SPEED (SUMOReal)10.0

#define TURN_LANE_DIST (SUMOReal)200.0 // the distance at which a lane leading elsewhere is considered to be a turn-lane that must be avoided

//#define DEBUG_COND (myVehicle.getID() == "1501_27271428" || myVehicle.getID() == "1502_27270000")
//#define DEBUG_COND (myVehicle.getID() == "175129_26220000")
//#define DEBUG_COND (myVehicle.getID() == "pkw150478" || myVehicle.getID() == "pkw150494" || myVehicle.getID() == "pkw150289")
//#define DEBUG_COND (myVehicle.getID() == "A" || myVehicle.getID() == "B") // fail change to left
//#define DEBUG_COND (myVehicle.getID() == "Costa_12_13") // test stops_overtaking
#define DEBUG_COND false

// debug function
std::string
tryID(const MSVehicle* v) {
    return v == 0 ? "NULL" : v->getID();
}


// ===========================================================================
// member method definitions
// ===========================================================================
MSLCM_JE2013::MSLCM_JE2013(MSVehicle& v) :
    MSAbstractLaneChangeModel(v),
    mySpeedGainProbability(0),
    myKeepRightProbability(0),
    myLeadingBlockerLength(0),
    myLeftSpace(0),
    myLookAheadSpeed(LOOK_AHEAD_MIN_SPEED)
{}

MSLCM_JE2013::~MSLCM_JE2013() {
    changed(0);
}


int
MSLCM_JE2013::wantsChange(
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
    gDebugFlag2 = DEBUG_COND;

    if (gDebugFlag2) {
        std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                  << " veh=" << myVehicle.getID()
                  << " lane=" << myVehicle.getLane()->getID()
                  << " pos=" << myVehicle.getPositionOnLane()
                  << " speed=" << myVehicle.getSpeed()
                  << " considerChangeTo=" << (laneOffset == -1  ? "right" : "left")
                  << "\n";
    }

    const int result = _wantsChange(laneOffset, msgPass, blocked, leader, neighLead, neighFollow, neighLane, preb, lastBlocked, firstBlocked);
    if (gDebugFlag2) {
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
    gDebugFlag2 = false;
    return result;
}


SUMOReal
MSLCM_JE2013::patchSpeed(const SUMOReal min, const SUMOReal wanted, const SUMOReal max, const MSCFModel& cfModel) {
    gDebugFlag1 = DEBUG_COND;

    const SUMOReal newSpeed = _patchSpeed(min, wanted, max, cfModel);
    if (gDebugFlag1) {
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
    gDebugFlag1 = false;
    return newSpeed;
}


SUMOReal
MSLCM_JE2013::_patchSpeed(const SUMOReal min, const SUMOReal wanted, const SUMOReal max, const MSCFModel& cfModel) {

    const SUMOReal time = STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep());

    int state = myOwnState;

    // letting vehicles merge in at the end of the lane in case of counter-lane change, step#2
    SUMOReal MAGIC_offset = 1.;
    //   if we want to change and have a blocking leader and there is enough room for him in front of us
    if (myLeadingBlockerLength != 0) {
        SUMOReal space = myLeftSpace - myLeadingBlockerLength - MAGIC_offset - myVehicle.getVehicleType().getMinGap();
        if (gDebugFlag1) {
            std::cout << time << " veh=" << myVehicle.getID() << " myLeadingBlockerLength=" << myLeadingBlockerLength << " space=" << space << "\n";
        }
        if (space > 0) { // XXX space > -MAGIC_offset
            // compute speed for decelerating towards a place which allows the blocking leader to merge in in front
            SUMOReal safe = cfModel.stopSpeed(&myVehicle, myVehicle.getSpeed(), space);
            // if we are approaching this place
            if (safe < wanted) {
                // return this speed as the speed to use
                if (gDebugFlag1) {
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
            if (gDebugFlag1) {
                std::cout << time << " veh=" << myVehicle.getID() << " got nVSafe=" << nVSafe << "\n";
            }
        } else {
            if (v < min) {
                if (gDebugFlag1) {
                    std::cout << time << " veh=" << myVehicle.getID() << " ignoring low nVSafe=" << v << " min=" << min << "\n";
                }
            } else {
                if (gDebugFlag1) {
                    std::cout << time << " veh=" << myVehicle.getID() << " ignoring high nVSafe=" << v << " max=" << max << "\n";
                }
            }
        }
    }

    if (gotOne && !myDontBrake) {
        if (gDebugFlag1) {
            std::cout << time << " veh=" << myVehicle.getID() << " got vSafe\n";
        }
        return nVSafe;
    }

    // check whether the vehicle is blocked
    if ((state & LCA_WANTS_LANECHANGE) != 0 && (state & LCA_BLOCKED) != 0) {
        if ((state & LCA_STRATEGIC) != 0) {
            // necessary decelerations are controlled via vSafe. If there are
            // none it means we should speed up
            if (gDebugFlag1) {
                std::cout << time << " veh=" << myVehicle.getID() << " LCA_WANTS_LANECHANGE (strat, no vSafe)\n";
            }
            return (max + wanted) / (SUMOReal) 2.0;
        } else if ((state & LCA_COOPERATIVE) != 0) {
            // only minor adjustments in speed should be done
            if ((state & LCA_BLOCKED_BY_LEADER) != 0) {
                if (gDebugFlag1) {
                    std::cout << time << " veh=" << myVehicle.getID() << " LCA_BLOCKED_BY_LEADER (coop)\n";
                }
                return (min + wanted) / (SUMOReal) 2.0;
            }
            if ((state & LCA_BLOCKED_BY_FOLLOWER) != 0) {
                if (gDebugFlag1) {
                    std::cout << time << " veh=" << myVehicle.getID() << " LCA_BLOCKED_BY_FOLLOWER (coop)\n";
                }
                return (max + wanted) / (SUMOReal) 2.0;
            }
            //} else { // VARIANT_16
            //    // only accelerations should be performed
            //    if ((state & LCA_BLOCKED_BY_FOLLOWER) != 0) {
            //        if (gDebugFlag1) std::cout << time << " veh=" << myVehicle.getID() << " LCA_BLOCKED_BY_FOLLOWER\n";
            //        return (max + wanted) / (SUMOReal) 2.0;
            //    }
        }
    }

    /*
    // decelerate if being a blocking follower
    //  (and does not have to change lanes)
    if ((state & LCA_AMBLOCKINGFOLLOWER) != 0) {
        if (fabs(max - myVehicle.getCarFollowModel().maxNextSpeed(myVehicle.getSpeed(), &myVehicle)) < 0.001 && min == 0) { // !!! was standing
            if (gDebugFlag1) std::cout << time << " veh=" << myVehicle.getID() << " LCA_AMBLOCKINGFOLLOWER (standing)\n";
            return 0;
        }
        if (gDebugFlag1) std::cout << time << " veh=" << myVehicle.getID() << " LCA_AMBLOCKINGFOLLOWER\n";

        //return min; // VARIANT_3 (brakeStrong)
        return (min + wanted) / (SUMOReal) 2.0;
    }
    if ((state & LCA_AMBACKBLOCKER) != 0) {
        if (max <= myVehicle.getCarFollowModel().maxNextSpeed(myVehicle.getSpeed(), &myVehicle) && min == 0) { // !!! was standing
            if (gDebugFlag1) std::cout << time << " veh=" << myVehicle.getID() << " LCA_AMBACKBLOCKER (standing)\n";
            //return min; VARIANT_9 (backBlockVSafe)
            return nVSafe;
        }
    }
    if ((state & LCA_AMBACKBLOCKER_STANDING) != 0) {
        if (gDebugFlag1) std::cout << time << " veh=" << myVehicle.getID() << " LCA_AMBACKBLOCKER_STANDING\n";
        //return min;
        return nVSafe;
    }
    */

    // accelerate if being a blocking leader or blocking follower not able to brake
    //  (and does not have to change lanes)
    if ((state & LCA_AMBLOCKINGLEADER) != 0) {
        if (gDebugFlag1) {
            std::cout << time << " veh=" << myVehicle.getID() << " LCA_AMBLOCKINGLEADER\n";
        }
        return (max + wanted) / (SUMOReal) 2.0;
    }

    if ((state & LCA_AMBLOCKINGFOLLOWER_DONTBRAKE) != 0) {
        if (gDebugFlag1) {
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
    if (myVehicle.getLane()->getEdge().getLanes().size() == 1) {
        // remove chaning information if on a road with a single lane
        changed(0);
    }
    return wanted;
}


void*
MSLCM_JE2013::inform(void* info, MSVehicle* sender) {
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


SUMOReal
MSLCM_JE2013::informLeader(MSAbstractLaneChangeModel::MSLCMessager& msgPass,
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
    if (gDebugFlag2) {
        std::cout << " informLeader speed=" <<  myVehicle.getSpeed() << " planned=" << plannedSpeed << "\n";
    }

    if ((blocked & LCA_BLOCKED_BY_LEADER) != 0) {
        assert(neighLead.first != 0);
        MSVehicle* nv = neighLead.first;
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
                || myLeftSpace - myVehicle.getCarFollowModel().brakeGap(myVehicle.getSpeed()) < overtakeDist
                // not enough time to overtake?
                || dv * remainingSeconds < overtakeDist) {
            // cannot overtake
            msgPass.informNeighLeader(new Info(-1, dir | LCA_AMBLOCKINGLEADER), &myVehicle);
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
            msgPass.informNeighLeader(new Info(nv->getSpeed(), dir | LCA_AMBLOCKINGLEADER), &myVehicle);
            return -1;
        }
    } else if (neighLead.first != 0) { // (remainUnblocked)
        // we are not blocked now. make sure we stay far enough from the leader
        MSVehicle* nv = neighLead.first;
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
MSLCM_JE2013::informFollower(MSAbstractLaneChangeModel::MSLCMessager& msgPass,
                             int blocked,
                             int dir,
                             const std::pair<MSVehicle*, SUMOReal>& neighFollow,
                             SUMOReal remainingSeconds,
                             SUMOReal plannedSpeed) {
    if ((blocked & LCA_BLOCKED_BY_FOLLOWER) != 0) {
        assert(neighFollow.first != 0);
        MSVehicle* nv = neighFollow.first;
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
                msgPass.informNeighFollower(new Info(plannedSpeed - HELP_OVERTAKE, dir | LCA_AMBLOCKINGFOLLOWER), &myVehicle);
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
            const SUMOReal vsafe = MAX2(neighNewSpeed, nv->getCarFollowModel().followSpeed(
                                            nv, nv->getSpeed(), neighFollow.second, plannedSpeed, myVehicle.getCarFollowModel().getMaxDecel()));
            msgPass.informNeighFollower(new Info(vsafe, dir | LCA_AMBLOCKINGFOLLOWER), &myVehicle);
            if (gDebugFlag2) {
                std::cout << " wants to cut in before nv=" << nv->getID()
                          << " vsafe=" << vsafe
                          << " newSecGap=" << nv->getCarFollowModel().getSecureGap(vsafe, plannedSpeed, myVehicle.getCarFollowModel().getMaxDecel())
                          << "\n";
            }
        } else if (dv > 0 && dv * remainingSeconds > (secureGap - decelGap + POSITION_EPS)) {
            // decelerating once is sufficient to open up a large enough gap in time
            msgPass.informNeighFollower(new Info(neighNewSpeed, dir | LCA_AMBLOCKINGFOLLOWER), &myVehicle);
            if (gDebugFlag2) {
                std::cout << " wants to cut in before nv=" << nv->getID() << " (eventually)\n";
            }
        } else if (dir == LCA_MRIGHT && !myAllowOvertakingRight && !nv->congested()) {
            const SUMOReal vhelp = MAX2(neighNewSpeed, HELP_OVERTAKE);
            msgPass.informNeighFollower(new Info(vhelp, dir | LCA_AMBLOCKINGFOLLOWER), &myVehicle);
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
                    msgPass.informNeighFollower(new Info(vhelp, dir | LCA_AMBLOCKINGFOLLOWER), &myVehicle);
                    return;
                }
            }
            msgPass.informNeighFollower(new Info(vhelp, dir | LCA_AMBLOCKINGFOLLOWER), &myVehicle);
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
        MSVehicle* nv = neighFollow.first;
        const SUMOReal vsafe = nv->getCarFollowModel().followSpeed(
                                   nv, nv->getSpeed(), neighFollow.second, plannedSpeed, myVehicle.getCarFollowModel().getMaxDecel());
        msgPass.informNeighFollower(new Info(vsafe, dir), &myVehicle);
        if (gDebugFlag2) {
            std::cout << " wants to cut in before non-blocking follower nv=" << nv->getID() << "\n";
        }
    }
}


void
MSLCM_JE2013::prepareStep() {
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
MSLCM_JE2013::changed(int dir) {
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
    initLastLaneChangeOffset(dir);
}


int
MSLCM_JE2013::_wantsChange(
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
    assert(laneOffset == 1 || laneOffset == -1);
    const SUMOTime currentTime = MSNet::getInstance()->getCurrentTimeStep();
    // compute bestLaneOffset
    MSVehicle::LaneQ curr, neigh, best;
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
            best = preb[p + bestLaneOffset];
            currIdx = p;
            break;
        }
    }
    // direction specific constants
    const bool right = (laneOffset == -1);
    const int lca = (right ? LCA_RIGHT : LCA_LEFT);
    const int myLca = (right ? LCA_MRIGHT : LCA_MLEFT);
    const int lcaCounter = (right ? LCA_LEFT : LCA_RIGHT);
    const int myLcaCounter = (right ? LCA_MLEFT : LCA_MRIGHT);
    const bool changeToBest = (right && bestLaneOffset < 0) || (!right && bestLaneOffset > 0);
    // keep information about being a leader/follower
    int ret = (myOwnState & 0xffff0000);
    int req = 0; // the request to change or stay

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
                  << " firstBlocked=" << tryID(*firstBlocked)
                  << " lastBlocked=" << tryID(*lastBlocked)
                  << " neighLead=" << tryID(neighLead.first)
                  << " neighLeadGap=" << neighLead.second
                  << " neighFollow=" << tryID(neighFollow.first)
                  << " neighFollowGap=" << neighFollow.second
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
    SUMOReal laDist = myLookAheadSpeed * (right ? LOOK_FORWARD_RIGHT : LOOK_FORWARD_LEFT);
    laDist += myVehicle.getVehicleType().getLengthWithGap() * (SUMOReal) 2.;
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
        currentDist += roundaboutEdgesAhead * ROUNDABOUT_DIST_BONUS;
        neighDist += roundaboutEdgesAheadNeigh * ROUNDABOUT_DIST_BONUS;
    }
    if (roundaboutEdgesAhead > 0) {
        if (gDebugFlag2) {
            std::cout << " roundaboutEdgesAhead=" << roundaboutEdgesAhead << " roundaboutEdgesAheadNeigh=" << roundaboutEdgesAheadNeigh << "\n";
        }
    }

    const SUMOReal usableDist = (currentDist - myVehicle.getPositionOnLane() - best.occupation *  JAM_FACTOR);
    //- (best.lane->getVehicleNumber() * neighSpeed)); // VARIANT 9 jfSpeed
    const SUMOReal maxJam = MAX2(preb[currIdx + laneOffset].occupation, preb[currIdx].occupation);
    const SUMOReal neighLeftPlace = MAX2((SUMOReal) 0, neighDist - myVehicle.getPositionOnLane() - maxJam);

    if (gDebugFlag2) {
        std::cout << STEPS2TIME(currentTime)
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
                    mySpeedGainProbability += CHANGE_PROB_THRESHOLD_LEFT / 3;
                }
                if (gDebugFlag2) {
                    std::cout << STEPS2TIME(currentTime)
                              << " avoid overtaking on the right nv=" << nv->getID()
                              << " nvSpeed=" << nv->getSpeed()
                              << " mySpeedGainProbability=" << mySpeedGainProbability
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
            ret = ret | LCA_STAY | LCA_STRATEGIC;
        } else if (bestLaneOffset == 0 && (neighLeftPlace * 2. < laDist)) {
            // the current lane is the best and a lane-changing would cause a situation
            //  of which we assume we will not be able to return to the lane we have to be on.
            // this rule prevents the vehicle from leaving the current, best lane when it is
            //  close to this lane's end
            if (gDebugFlag2) {
                std::cout << " veh=" << myVehicle.getID() << " could not change back and forth in time (2) neighLeftPlace=" << neighLeftPlace << "\n";
            }
            ret = ret | LCA_STAY | LCA_STRATEGIC;
        } else if (bestLaneOffset == 0
                   && (leader.first == 0 || !leader.first->isStopped())
                   && neigh.bestContinuations.back()->getLinkCont().size() != 0
                   && roundaboutEdgesAhead == 0
                   && neighDist < TURN_LANE_DIST) {
            // VARIANT_21 (stayOnBest)
            // we do not want to leave the best lane for a lane which leads elsewhere
            // unless our leader is stopped or we are approaching a roundabout
            if (gDebugFlag2) {
                std::cout << " veh=" << myVehicle.getID() << " does not want to leave the bestLane (neighDist=" << neighDist << ")\n";
            }
            ret = ret | LCA_STAY | LCA_STRATEGIC;
        }
    }
    // check for overriding TraCI requests
    if (gDebugFlag2) {
        std::cout << STEPS2TIME(currentTime) << " veh=" << myVehicle.getID() << " ret=" << ret;
    }
    ret = myVehicle.influenceChangeDecision(ret);
    if ((ret & lcaCounter) != 0) {
        // we are not interested in traci requests for the opposite direction here
        ret &= ~(LCA_TRACI | lcaCounter | LCA_URGENT);
    }
    if (gDebugFlag2) {
        std::cout << " retAfterInfluence=" << ret << "\n";
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
        saveBlockerLength(neighLead.first, lcaCounter);
        if (*firstBlocked != neighLead.first) {
            saveBlockerLength(*firstBlocked, lcaCounter);
        }

        const SUMOReal remainingSeconds = ((ret & LCA_TRACI) == 0 ?
                                           MAX2((SUMOReal)STEPS2TIME(TS), myLeftSpace / MAX2(myLookAheadSpeed, NUMERICAL_EPS) / abs(bestLaneOffset) / URGENCY) :
                                           myVehicle.getInfluencer().changeRequestRemainingSeconds(currentTime));
        const SUMOReal plannedSpeed = informLeader(msgPass, blocked, myLca, neighLead, remainingSeconds);
        if (plannedSpeed >= 0) {
            // maybe we need to deal with a blocking follower
            informFollower(msgPass, blocked, myLca, neighFollow, remainingSeconds, plannedSpeed);
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
        if (lca == LCA_LEFT) {
            req = ret | lca | LCA_COOPERATIVE;
        } else {
            req = ret | LCA_STAY | LCA_COOPERATIVE;
        }
        if (!cancelRequest(req)) {
            return ret | req;
        }
    }

    // let's also regard the case where the vehicle is driving on a highway...
    //  in this case, we do not want to get to the dead-end of an on-ramp
    if (right) {
        if (bestLaneOffset == 0 && myVehicle.getLane()->getSpeedLimit() > 80. / 3.6 && myLookAheadSpeed > SUMO_const_haltingSpeed) {
            if (gDebugFlag2) {
                std::cout << " veh=" << myVehicle.getID() << " does not want to get stranded on the on-ramp of a highway\n";
            }
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
    if (neighLead.first == 0) {
        neighLaneVSafe = MIN2(neighLaneVSafe, myCarFollowModel.followSpeed(&myVehicle, myVehicle.getSpeed(), neighDist, 0, 0));
    } else {
        // @todo: what if leader is below safe gap?!!!
        neighLaneVSafe = MIN2(neighLaneVSafe, myCarFollowModel.followSpeed(
                                  &myVehicle, myVehicle.getSpeed(), neighLead.second, neighLead.first->getSpeed(), neighLead.first->getCarFollowModel().getMaxDecel()));
    }
    if (leader.first == 0) {
        thisLaneVSafe = MIN2(thisLaneVSafe, myCarFollowModel.followSpeed(&myVehicle, myVehicle.getSpeed(), currentDist, 0, 0));
    } else {
        // @todo: what if leader is below safe gap?!!!
        thisLaneVSafe = MIN2(thisLaneVSafe, myCarFollowModel.followSpeed(&myVehicle, myVehicle.getSpeed(), leader.second, leader.first->getSpeed(), leader.first->getCarFollowModel().getMaxDecel()));
    }

    const SUMOReal vMax = MIN2(myVehicle.getVehicleType().getMaxSpeed(), myVehicle.getLane()->getVehicleMaxSpeed(&myVehicle));
    thisLaneVSafe = MIN2(thisLaneVSafe, vMax);
    neighLaneVSafe = MIN2(neighLaneVSafe, vMax);
    const SUMOReal relativeGain = (neighLaneVSafe - thisLaneVSafe) / MAX2(neighLaneVSafe,
                                  RELGAIN_NORMALIZATION_MIN_SPEED);

    if (right) {
        // ONLY FOR CHANGING TO THE RIGHT
        if (thisLaneVSafe - 5 / 3.6 > neighLaneVSafe) {
            // ok, the current lane is faster than the right one...
            if (mySpeedGainProbability < 0) {
                mySpeedGainProbability /= 2.0;
            }
        } else {
            // ok, the current lane is not faster than the right one
            mySpeedGainProbability -= relativeGain;

            // honor the obligation to keep right (Rechtsfahrgebot)
            // XXX consider fast approaching followers on the current lane
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
            const SUMOReal deltaProb = (CHANGE_PROB_THRESHOLD_RIGHT
                                        * STEPS2TIME(DELTA_T)
                                        * (fullSpeedDrivingSeconds / acceptanceTime) / KEEP_RIGHT_TIME);
            myKeepRightProbability -= deltaProb;

            if (gDebugFlag2) {
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
                          << "\n";
            }
            if (myKeepRightProbability < -CHANGE_PROB_THRESHOLD_RIGHT) {
                req = ret | lca | LCA_KEEPRIGHT;
                if (!cancelRequest(req)) {
                    return ret | req;
                }
            }
        }

        if (gDebugFlag2) {
            std::cout << STEPS2TIME(currentTime)
                      << " veh=" << myVehicle.getID()
                      << " speed=" << myVehicle.getSpeed()
                      << " myKeepRightProbability=" << myKeepRightProbability
                      << " thisLaneVSafe=" << thisLaneVSafe
                      << " neighLaneVSafe=" << neighLaneVSafe
                      << " relativeGain=" << relativeGain
                      << " blocked=" << blocked
                      << "\n";
        }

        if (mySpeedGainProbability < -CHANGE_PROB_THRESHOLD_RIGHT
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
                mySpeedGainProbability /= 2.0;
            }
        } else {
            // left lane is better
            mySpeedGainProbability += relativeGain;
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
        if (mySpeedGainProbability > CHANGE_PROB_THRESHOLD_LEFT && neighDist / MAX2((SUMOReal) .1, myVehicle.getSpeed()) > 20.) { // .1
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
    if (gDebugFlag2) {
        std::cout << STEPS2TIME(currentTime)
                  << " veh=" << myVehicle.getID()
                  << " mySpeedGainProbability=" << mySpeedGainProbability
                  << " myKeepRightProbability=" << myKeepRightProbability
                  << " thisLaneVSafe=" << thisLaneVSafe
                  << " neighLaneVSafe=" << neighLaneVSafe
                  << "\n";
    }
    return ret;
}


int
MSLCM_JE2013::slowDownForBlocked(MSVehicle** blocked, int state) {
    //  if this vehicle is blocking someone in front, we maybe decelerate to let him in
    if ((*blocked) != 0) {
        SUMOReal gap = (*blocked)->getPositionOnLane() - (*blocked)->getVehicleType().getLength() - myVehicle.getPositionOnLane() - myVehicle.getVehicleType().getMinGap();
        if (gDebugFlag2) {
            std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                      << " veh=" << myVehicle.getID()
                      << " blocked=" << tryID(*blocked)
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
MSLCM_JE2013::saveBlockerLength(MSVehicle* blocker, int lcaCounter) {
    if (gDebugFlag2) {
        std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                  << " veh=" << myVehicle.getID()
                  << " saveBlockerLength blocker=" << tryID(blocker)
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
                          << " blocker=" << tryID(blocker)
                          << " saving myLeadingBlockerLength=" << myLeadingBlockerLength
                          << "\n";
            }
        } else {
            // we cannot save enough space for the blocker. It needs to save
            // space for ego instead
            if (gDebugFlag2) {
                std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                          << " veh=" << myVehicle.getID()
                          << " blocker=" << tryID(blocker)
                          << " cannot save space=" << blocker->getVehicleType().getLengthWithGap()
                          << " potential=" << potential
                          << "\n";
            }
            blocker->getLaneChangeModel().saveBlockerLength(myVehicle.getVehicleType().getLengthWithGap());
        }
    }
}
/****************************************************************************/

