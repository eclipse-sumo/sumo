/****************************************************************************/
/// @file    MSLCM_JE2013.h
/// @author  Jakob Erdmann
/// @date    Fri, 08.10.2013
/// @version $Id$
///
// A lane change model developed by J. Erdmann 
// based on the model of D. Krajzewicz developed between 2004 and 2011 (MSLCM_DK2004)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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

#define LOOK_AHEAD_MIN_SPEED (SUMOReal)0.0
#define LOOK_AHEAD_SPEED_MEMORY (SUMOReal)0.9
#define LOOK_AHEAD_SPEED_DECREMENT 6.

#define HELP_DECEL_FACTOR (SUMOReal)1.0

#define HELP_OVERTAKE  (SUMOReal)(10.0 / 3.6)
#define MIN_FALLBEHIND  (SUMOReal)(14.0 / 3.6)

#define KEEP_RIGHT_HEADWAY (SUMOReal)2.0

#define URGENCY (SUMOReal)2.0 

#define ROUNDABOUT_DIST_BONUS (SUMOReal)80.0 

//#define DEBUG_COND (myVehicle.getID() == "pkw22806" || myVehicle.getID() == "pkw22823")
//#define DEBUG_COND (myVehicle.getID() == "emitter_SST92-150 FG 1 DE 3_26966400" || myVehicle.getID() == "emitter_SST92-150 FG 1 DE 1_26932941" || myVehicle.getID() == "emitter_SST92-175 FG 1 DE 129_27105000") 
//#define DEBUG_COND (myVehicle.getID() == "Costa_200_153" || myVehicle.getID() == "Costa_12_154") // fail change to left
//#define DEBUG_COND (myVehicle.getID() == "v3.6") // test stops_overtaking
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
    changed();
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
        MSVehicle** firstBlocked) 
{
    MSGlobals::gDebugFlag2 = DEBUG_COND;

    if (MSGlobals::gDebugFlag2) {
        std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
            << " veh=" << myVehicle.getID()
            << " lane=" << myVehicle.getLane()->getID()
            << " pos=" << myVehicle.getPositionOnLane()
            << " speed=" << myVehicle.getSpeed()
            << " considerChangeTo=" << (laneOffset == -1  ? "right" : "left")
            << "\n";
    }

    const int result = _wantsChange(laneOffset, msgPass, blocked, leader, neighLead, neighFollow, neighLane, preb, lastBlocked, firstBlocked);
    if (MSGlobals::gDebugFlag2) {
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
                << ((blocked & LCA_BLOCKED) ? " (blocked)" : "")
                << ((blocked & LCA_OVERLAPPING) ? " (overlap)" : "")
                << "\n";
        }
    }
    MSGlobals::gDebugFlag2 = false;
    return result;
}


SUMOReal
MSLCM_JE2013::patchSpeed(const SUMOReal min, const SUMOReal wanted, const SUMOReal max, const MSCFModel& cfModel) {
    MSGlobals::gDebugFlag1 = DEBUG_COND;

    const SUMOReal newSpeed = _patchSpeed(min, wanted, max, cfModel);
    if (MSGlobals::gDebugFlag1) {
        const std::string patched = (wanted != newSpeed ? " patched=" + toString(newSpeed) : "");
        std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
            << " veh=" << myVehicle.getID()
            << " lane=" << myVehicle.getLane()->getID()
            << " pos=" << myVehicle.getPositionOnLane()
            << " v=" << myVehicle.getSpeed()
            << " wanted=" << wanted
            << patched 
            << "\n";
    }
    MSGlobals::gDebugFlag1 = false;
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
        if (MSGlobals::gDebugFlag1) std::cout << time << " veh=" << myVehicle.getID() << " myLeadingBlockerLength=" << myLeadingBlockerLength << " space=" << space << "\n";
        if (space > 0) {
            // compute speed for decelerating towards a place which allows the blocking leader to merge in in front
            SUMOReal safe = cfModel.stopSpeed(&myVehicle, myVehicle.getSpeed(), space);
            // if we are approaching this place
            if (safe < wanted) {
                // return this speed as the speed to use
                if (MSGlobals::gDebugFlag1) std::cout << time << " veh=" << myVehicle.getID() << " slowing down for leading blocker" << (safe < min ? " (not enough)": "") << "\n";
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
            if (MSGlobals::gDebugFlag1) std::cout << time << " veh=" << myVehicle.getID() << " got nVSafe=" << nVSafe << "\n";
        } else {
            if (MSGlobals::gDebugFlag1) std::cout << time << " veh=" << myVehicle.getID() << " ignoring nVSafe=" << v << "\n";
        }
    }

    if (gotOne && !myDontBrake) {
        if (MSGlobals::gDebugFlag1) std::cout << time << " veh=" << myVehicle.getID() << " got vSafe\n";
        return nVSafe;
    }

    // check whether the vehicle is blocked
    if ((state & LCA_WANTS_LANECHANGE) != 0 && (state & LCA_BLOCKED) != 0) {
        if ((state & LCA_STRATEGIC) != 0) {
            // necessary decelerations are controlled via vSafe. If there are
            // none it means we should speed up
            if (MSGlobals::gDebugFlag1) std::cout << time << " veh=" << myVehicle.getID() << " LCA_WANTS_LANECHANGE (strat, no vSafe)\n";
            return (max + wanted) / (SUMOReal) 2.0;
        } else if ((state & LCA_COOPERATIVE) != 0) {
            // only minor adjustments in speed should be done
            if ((state & LCA_BLOCKED_BY_LEADER) != 0) {
                if (MSGlobals::gDebugFlag1) std::cout << time << " veh=" << myVehicle.getID() << " LCA_BLOCKED_BY_LEADER (coop)\n";
                return (min + wanted) / (SUMOReal) 2.0;
            }
            if ((state & LCA_BLOCKED_BY_FOLLOWER) != 0) {
                if (MSGlobals::gDebugFlag1) std::cout << time << " veh=" << myVehicle.getID() << " LCA_BLOCKED_BY_FOLLOWER (coop)\n";
                return (max + wanted) / (SUMOReal) 2.0;
            }
        //} else { // VARIANT_16
        //    // only accelerations should be performed
        //    if ((state & LCA_BLOCKED_BY_FOLLOWER) != 0) {
        //        if (MSGlobals::gDebugFlag1) std::cout << time << " veh=" << myVehicle.getID() << " LCA_BLOCKED_BY_FOLLOWER\n";
        //        return (max + wanted) / (SUMOReal) 2.0;
        //    }
        }
    }

    /*
    // decelerate if being a blocking follower
    //  (and does not have to change lanes)
    if ((state & LCA_AMBLOCKINGFOLLOWER) != 0) {
        if (fabs(max - myVehicle.getCarFollowModel().maxNextSpeed(myVehicle.getSpeed(), &myVehicle)) < 0.001 && min == 0) { // !!! was standing
            if (MSGlobals::gDebugFlag1) std::cout << time << " veh=" << myVehicle.getID() << " LCA_AMBLOCKINGFOLLOWER (standing)\n";
            return 0;
        }
        if (MSGlobals::gDebugFlag1) std::cout << time << " veh=" << myVehicle.getID() << " LCA_AMBLOCKINGFOLLOWER\n";
        
        //return min; // VARIANT_3 (brakeStrong)
        return (min + wanted) / (SUMOReal) 2.0; 
    }
    if ((state & LCA_AMBACKBLOCKER) != 0) {
        if (max <= myVehicle.getCarFollowModel().maxNextSpeed(myVehicle.getSpeed(), &myVehicle) && min == 0) { // !!! was standing
            if (MSGlobals::gDebugFlag1) std::cout << time << " veh=" << myVehicle.getID() << " LCA_AMBACKBLOCKER (standing)\n";
            //return min; VARIANT_9 (backBlockVSafe)
            return nVSafe;
        }
    }
    if ((state & LCA_AMBACKBLOCKER_STANDING) != 0) {
        if (MSGlobals::gDebugFlag1) std::cout << time << " veh=" << myVehicle.getID() << " LCA_AMBACKBLOCKER_STANDING\n";
        //return min;
        return nVSafe;
    }
    */

    // accelerate if being a blocking leader or blocking follower not able to brake
    //  (and does not have to change lanes)
    if ((state & LCA_AMBLOCKINGLEADER) != 0) {
        if (MSGlobals::gDebugFlag1) std::cout << time << " veh=" << myVehicle.getID() << " LCA_AMBLOCKINGLEADER\n";
        return (max + wanted) / (SUMOReal) 2.0;
    }

    if ((state & LCA_AMBLOCKINGFOLLOWER_DONTBRAKE) != 0) {
        if (MSGlobals::gDebugFlag1) std::cout << time << " veh=" << myVehicle.getID() << " LCA_AMBLOCKINGFOLLOWER_DONTBRAKE\n";
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
        changed();
    }
    return wanted;
}


void*
MSLCM_JE2013::inform(void* info, MSVehicle* sender) {
    Info* pinfo = (Info*) info;
    myVSafes.push_back(pinfo->first);
    //myOwnState &= 0xffffffff; // reset all bits of MyLCAEnum but only those
    myOwnState |= pinfo->second;
    if (MSGlobals::gDebugFlag2 || DEBUG_COND) {
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
                            SUMOReal remainingSeconds) 
{
    SUMOReal plannedSpeed = MIN2(myVehicle.getSpeed(), 
            myVehicle.getCarFollowModel().stopSpeed(&myVehicle, myVehicle.getSpeed(), myLeftSpace - myLeadingBlockerLength));
    for (std::vector<SUMOReal>::const_iterator i = myVSafes.begin(); i != myVSafes.end(); ++i) {
        SUMOReal v = (*i);
        if (v >= myVehicle.getSpeed() - ACCEL2SPEED(myVehicle.getCarFollowModel().getMaxDecel())) {
            plannedSpeed = MIN2(plannedSpeed, v);
        }
    }
    if (MSGlobals::gDebugFlag2) std::cout << " informLeader speed=" <<  myVehicle.getSpeed() << " planned=" << plannedSpeed << "\n";

    if ((blocked & LCA_BLOCKED_BY_LEADER) != 0) {
        assert(neighLead.first != 0);
        MSVehicle* nv = neighLead.first;
        if (MSGlobals::gDebugFlag2) std::cout << " blocked by leader nv=" <<  nv->getID() << " nvSpeed=" << nv->getSpeed()<< " needGap=" 
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
                || (dir == LCA_MLEFT && !myVehicle.congested())
                // not enough space to overtake?
                || myLeftSpace < overtakeDist
                // not enough time to overtake?
                || dv * remainingSeconds < overtakeDist) {
            // cannot overtake
            msgPass.informNeighLeader(new Info(-1, dir | LCA_AMBLOCKINGLEADER), &myVehicle);
            // slow down smoothly to follow leader 
            const SUMOReal targetSpeed = myCarFollowModel.followSpeed(
                    &myVehicle, myVehicle.getSpeed(), neighLead.second, nv->getSpeed(), nv->getCarFollowModel().getMaxDecel());
            if (targetSpeed < myVehicle.getSpeed()) {
                // slow down smoothly to follow leader 
                const SUMOReal decel = MIN2(myVehicle.getCarFollowModel().getMaxDecel(), 
                        MAX2(MIN_FALLBEHIND, (myVehicle.getSpeed() - targetSpeed) / remainingSeconds));
                const SUMOReal nextSpeed = MIN2(plannedSpeed, myVehicle.getSpeed() - decel);
                if (MSGlobals::gDebugFlag2) {
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
                if (MSGlobals::gDebugFlag2) {
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
            if (MSGlobals::gDebugFlag2) {
                std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                    << " wants to overtake leader nv=" << nv->getID()
                    << " dv=" << dv
                    << " remainingSeconds=" << remainingSeconds
                    << " currentGap=" << neighLead.second
                    << " secureGap=" << nv->getCarFollowModel().getSecureGap(nv->getSpeed(), myVehicle.getSpeed(), myVehicle.getCarFollowModel().getMaxDecel())
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
        if (MSGlobals::gDebugFlag2) { 
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
                            SUMOReal plannedSpeed) 
{
    if ((blocked & LCA_BLOCKED_BY_FOLLOWER) != 0) {
        assert(neighFollow.first != 0);
        MSVehicle* nv = neighFollow.first;
        if (MSGlobals::gDebugFlag2) std::cout << " blocked by follower nv=" <<  nv->getID() << " nvSpeed=" << nv->getSpeed()<< " needGap=" 
            << nv->getCarFollowModel().getSecureGap(nv->getSpeed(), myVehicle.getSpeed(), myVehicle.getCarFollowModel().getMaxDecel()) << "\n";

        // are we fast enough to cut in without any help?
        if (plannedSpeed - nv->getSpeed() >= HELP_OVERTAKE) {
            const SUMOReal neededGap = nv->getCarFollowModel().getSecureGap(nv->getSpeed(), plannedSpeed, myVehicle.getCarFollowModel().getMaxDecel());
            if ((neededGap - neighFollow.second) / remainingSeconds < (plannedSpeed - nv->getSpeed())) {
                if (MSGlobals::gDebugFlag2) std::cout << " wants to cut in before  nv=" << nv->getID() << " without any help neededGap=" << neededGap << "\n";
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
        const SUMOReal neighNewSpeed = MAX2((SUMOReal)0, nv->getSpeed() - helpDecel);
        const SUMOReal dv = plannedSpeed - neighNewSpeed; 
        // new gap between follower and self in case the follower does brake for 1s
        const SUMOReal decelGap = neighFollow.second + dv;
        const SUMOReal secureGap = nv->getCarFollowModel().getSecureGap(neighNewSpeed, plannedSpeed, myVehicle.getCarFollowModel().getMaxDecel());
        if (MSGlobals::gDebugFlag2) {
            std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                << " egoV=" << myVehicle.getSpeed()
                << " egoNV=" << plannedSpeed
                << " nvNewSpeed=" << neighNewSpeed
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
            if (MSGlobals::gDebugFlag2) std::cout << " wants to cut in before nv=" << nv->getID() << "\n";
        } else if (dv > 0 && dv * remainingSeconds > (secureGap - decelGap + POSITION_EPS)) {
            // decelerating once is sufficient to open up a large enough gap in time
            msgPass.informNeighFollower(new Info(neighNewSpeed, dir | LCA_AMBLOCKINGFOLLOWER), &myVehicle);
            if (MSGlobals::gDebugFlag2) std::cout << " wants to cut in before nv=" << nv->getID() << " (eventually)\n";
        } else { 
            SUMOReal vhelp = MAX2(nv->getSpeed(), myVehicle.getSpeed() + HELP_OVERTAKE);
            if (dir == LCA_MRIGHT && myVehicle.getWaitingSeconds() > LCA_RIGHT_IMPATIENCE &&
                    nv->getSpeed() > myVehicle.getSpeed()) {
                // let the follower slow down to increase the likelyhood that later vehicles will be slow enough to help
                // follower should still be fast enough to open a gap
                vhelp = MAX2(neighNewSpeed, myVehicle.getSpeed() + HELP_OVERTAKE);
                if (MSGlobals::gDebugFlag2) std::cout << " wants right follower to slow down a bit\n";
                if ((nv->getSpeed() - myVehicle.getSpeed()) / helpDecel < remainingSeconds) {
                    if (MSGlobals::gDebugFlag2) std::cout << " wants to cut in before right follower nv=" << nv->getID() << " (eventually)\n";
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
            myVSafes.push_back(MAX2(vhelp - needDV, myVehicle.getSpeed() - myVehicle.getCarFollowModel().getMaxDecel()));

            if (MSGlobals::gDebugFlag2) {
                std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                    << " veh=" << myVehicle.getID()
                    << " wants to be overtaken by=" << nv->getID()
                    << " overtakeDist=" << overtakeDist
                    << " vneigh=" << nv->getSpeed()
                    << " vhelp=" << vhelp
                    << " vsafe=" << myVSafes.back()
                    << "\n";
            }
        }
    }
}


void
MSLCM_JE2013::prepareStep() {
    myOwnState = 0;
    myLeadingBlockerLength = 0;
    myLeftSpace = 0;
    myVSafes.clear();
    myDontBrake = false;
    // truncate to work around numerical instability between different builds
    mySpeedGainProbability = ceil(mySpeedGainProbability * 100000.0) * 0.00001;
    myKeepRightProbability = ceil(myKeepRightProbability * 100000.0) * 0.00001;
}


void
MSLCM_JE2013::changed() {
    myOwnState = 0;
    myLastLaneChangeOffset = 0;
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
        MSVehicle** firstBlocked) 
{
    assert(laneOffset == 1 || laneOffset == -1);
    // compute bestLaneOffset
    MSVehicle::LaneQ curr, neigh, best;
    int bestLaneOffset = 0;
    SUMOReal currentDist = 0;
    SUMOReal neighDist = 0;
    SUMOReal neighExtDist = 0;
    SUMOReal currExtDist = 0;
    int currIdx = 0;
    MSLane* prebLane = myVehicle.getLane();
    if (prebLane->getEdge().getPurpose() == MSEdge::EDGEFUNCTION_INTERNAL) {
        // internal edges are not kept inside the bestLanes structure
        prebLane = prebLane->getLinkCont()[0]->getLane();
    }
    for (int p = 0; p < (int) preb.size(); ++p) {
        if (preb[p].lane == prebLane && p + laneOffset >= 0) {
            assert(p + laneOffset < preb.size());
            curr = preb[p];
            neigh = preb[p + laneOffset];
            currentDist = curr.length;
            currExtDist = curr.lane->getLength();
            neighDist = neigh.length;
            neighExtDist = neigh.lane->getLength();
            bestLaneOffset = curr.bestLaneOffset;
            // VARIANT_13 (equalBest)
            if (bestLaneOffset == 0 && preb[p + laneOffset].bestLaneOffset == 0) { 
                if (MSGlobals::gDebugFlag2) {
                    std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
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

    if (MSGlobals::gDebugFlag2) {
        std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
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
        if (MSGlobals::gDebugFlag2) std::cout << " roundaboutEdgesAhead=" << roundaboutEdgesAhead << " roundaboutEdgesAheadNeigh=" << roundaboutEdgesAheadNeigh << "\n";
    }

    const SUMOReal usableDist = (currentDist - myVehicle.getPositionOnLane() - best.occupation *  JAM_FACTOR);
            //- (best.lane->getVehicleNumber() * neighSpeed)); // VARIANT 9 jfSpeed

    if (MSGlobals::gDebugFlag2) {
        std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
            << " veh=" << myVehicle.getID()
            << " laSpeed=" << myLookAheadSpeed
            << " laDist=" << laDist
            << " currentDist=" << currentDist 
            << " usableDist=" << usableDist 
            << " bestLaneOffset=" << bestLaneOffset
            << " best.length=" << best.length
            << "\n";
    }

    if (changeToBest && bestLaneOffset == curr.bestLaneOffset
            && currentDistDisallows(usableDist, bestLaneOffset, laDist)) {

        // save the left space
        myLeftSpace = currentDist - myVehicle.getPositionOnLane();
        // VARIANT_14 (furtherBlock)
        if (changeToBest && abs(bestLaneOffset) > 1) {
            // there might be a vehicle which needs to counter-lane-change one lane further and we cannot see it yet
            if (MSGlobals::gDebugFlag2) std::cout << "  reserving space for unseen blockers\n";
            myLeadingBlockerLength = MAX2((SUMOReal)(right ? 20.0 : 40.0), myLeadingBlockerLength);
        }

        // letting vehicles merge in at the end of the lane in case of counter-lane change, step#1
        //   if there is a leader and he wants to change to the opposite direction
        saveBlockerLength(neighLead.first, lcaCounter);
        if (*firstBlocked != neighLead.first) {
            saveBlockerLength(*firstBlocked, lcaCounter);
        }

        const SUMOReal remainingSeconds = MAX2((SUMOReal)STEPS2TIME(TS), myLeftSpace / myLookAheadSpeed / abs(bestLaneOffset) / URGENCY);
        const SUMOReal plannedSpeed = informLeader(msgPass, blocked, myLca, neighLead, remainingSeconds);
        if (plannedSpeed >= 0) {
            // maybe we need to deal with a blocking follower
            informFollower(msgPass, blocked, myLca, neighFollow, remainingSeconds, plannedSpeed);
        }

        if (MSGlobals::gDebugFlag2) {
            std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                << " veh=" << myVehicle.getID()
                << " myLeftSpace=" << myLeftSpace
                << " remainingSeconds=" << remainingSeconds
                << " plannedSpeed=" << plannedSpeed
                << "\n";
        }
        //
        return ret | lca | LCA_STRATEGIC | LCA_URGENT;
    }

    // VARIANT_20 (noOvertakeRight)
    if (!right && !myVehicle.congested() && neighLead.first !=0) {
        // check for slower leader on the left. we should not overtake but
        // rather move left ourselves (unless congested)
        MSVehicle* nv = neighLead.first;
        if (nv->getSpeed() < myVehicle.getSpeed()) {
            myVSafes.push_back(myCarFollowModel.followSpeed(
                    &myVehicle, myVehicle.getSpeed(), neighLead.second, nv->getSpeed(), nv->getCarFollowModel().getMaxDecel()));
            mySpeedGainProbability += 0.3;
            if (MSGlobals::gDebugFlag2) {
                std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                    << " avoid overtaking on the right nv=" << nv->getID()
                    << " nvSpeed=" << nv->getSpeed()
                    << " mySpeedGainProbability=" << mySpeedGainProbability
                    << " plannedSpeed=" << myVSafes.back()
                    << "\n";
            }
        }
    }

    // the opposite lane-changing direction should be done than the one examined herein
    //  we'll check whether we assume we could change anyhow and get back in time...
    //
    // this rule prevents the vehicle from moving in opposite direction of the best lane
    //  unless the way till the end where the vehicle has to be on the best lane
    //  is long enough
    SUMOReal maxJam = MAX2(preb[currIdx + laneOffset].occupation, preb[currIdx].occupation);
    SUMOReal neighLeftPlace = MAX2((SUMOReal) 0, neighDist - myVehicle.getPositionOnLane() - maxJam);
    if (!changeToBest && (currentDistDisallows(neighLeftPlace, abs(bestLaneOffset) + 2, laDist))) {
        // ...we will not change the lane if not
        if (MSGlobals::gDebugFlag2) std::cout << " veh=" << myVehicle.getID() << " could not change back and forth in time (1) neighLeftPlace=" << neighLeftPlace << "\n";
        return ret | LCA_STAY | LCA_STRATEGIC;
    }


    // if the current lane is the best and a lane-changing would cause a situation
    //  of which we assume we will not be able to return to the lane we have to be on...
    //
    // this rule prevents the vehicle from leaving the current, best lane when it is
    //  close to this lane's end
    if (bestLaneOffset == 0 && (neighLeftPlace * 2. < laDist)) {
        if (MSGlobals::gDebugFlag2) std::cout << " veh=" << myVehicle.getID() << " could not change back and forth in time (2) currExtDist=" << currExtDist << " neighExtDist=" << neighExtDist << " neighLeftPlace=" << neighLeftPlace << "\n";
        return ret | LCA_STAY | LCA_STRATEGIC;
    }

    // VARIANT_15
    if (roundaboutEdgesAhead > 1) {
        // try to use the inner lanes of a roundabout to increase throughput
        // unless we are approaching the exit
        if (lca == LCA_LEFT) {
            return ret | lca | LCA_COOPERATIVE;
        } else {
            return ret | LCA_STAY | LCA_COOPERATIVE;
        }
    } 

    // let's also regard the case where the vehicle is driving on a highway...
    //  in this case, we do not want to get to the dead-end of an on-ramp
    if (right) {
        if (bestLaneOffset == 0 && myVehicle.getLane()->getVehicleMaxSpeed(&myVehicle) > 80. / 3.6 && myLookAheadSpeed > SUMO_const_haltingSpeed) {
        if (MSGlobals::gDebugFlag2) std::cout << " veh=" << myVehicle.getID() << " does not want to get stranded on the on-ramp of a highway\n";
            return ret | LCA_STAY | LCA_STRATEGIC;
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
        if (MSGlobals::gDebugFlag2) {
            std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                << " veh=" << myVehicle.getID()
                << " wantsChangeToHelp=" << (right ? "right" : "left")
                << " state=" << myOwnState
                << (((myOwnState & myLcaCounter) != 0) ? " (counter)" : "")
                << "\n";
        }
        return ret | lca | LCA_COOPERATIVE | LCA_URGENT ;//| LCA_CHANGE_TO_HELP;
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

    thisLaneVSafe = MIN3(thisLaneVSafe, myVehicle.getVehicleType().getMaxSpeed(), myVehicle.getLane()->getVehicleMaxSpeed(&myVehicle));
    neighLaneVSafe = MIN3(neighLaneVSafe,myVehicle.getVehicleType().getMaxSpeed(), neighLane.getVehicleMaxSpeed(&myVehicle));

    if (right) {
        // ONLY FOR CHANGING TO THE RIGHT
        if (thisLaneVSafe - neighLaneVSafe > 5. / 3.6) {
            // ok, the current lane is faster than the right one...
            if (mySpeedGainProbability < 0) {
                mySpeedGainProbability /= 2.0;
                myKeepRightProbability /= 2.0;
            }
        } else {
            // ok, the right lane is faster than the current
            mySpeedGainProbability -= (SUMOReal)((neighLaneVSafe - thisLaneVSafe) / (myVehicle.getLane()->getVehicleMaxSpeed(&myVehicle)));
        }

        // let's recheck the "Rechtsfahrgebot"
        //keepRight(neighLead.first);
        keepRight(neighFollow.first);
        if (MSGlobals::gDebugFlag2) {
            std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                << " veh=" << myVehicle.getID()
                << " mySpeedGainProbability=" << mySpeedGainProbability
                << " myKeepRightProbability=" << myKeepRightProbability
                << " thisLaneVSafe=" << thisLaneVSafe
                << " neighLaneVSafe=" << neighLaneVSafe
                << "\n";
        }

        if (mySpeedGainProbability < -2 && neighDist / MAX2((SUMOReal) .1, myVehicle.getSpeed()) > 20.) { //./MAX2((SUMOReal) .1, myVehicle.getSpeed())) { // -.1
            if (mySpeedGainProbability - myKeepRightProbability >= -2) {
                return ret | lca | LCA_KEEPRIGHT;
            } else {
                return ret | lca | LCA_SPEEDGAIN;
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
            mySpeedGainProbability += (SUMOReal)((neighLaneVSafe - thisLaneVSafe) / (myVehicle.getLane()->getVehicleMaxSpeed(&myVehicle))); // !!! Fahrzeuggeschw.!
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
        if (mySpeedGainProbability > .2 && neighDist / MAX2((SUMOReal) .1, myVehicle.getSpeed()) > 20.) { // .1
            return ret | lca | LCA_SPEEDGAIN;
        }
    }
    // --------
    if (changeToBest && bestLaneOffset == curr.bestLaneOffset
            && (right ? mySpeedGainProbability < 0 : mySpeedGainProbability > 0)) {
        // change towards the correct lane, speedwise it does not hurt
        return ret | lca | LCA_STRATEGIC;
    }
    if (MSGlobals::gDebugFlag2) {
        std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
            << " veh=" << myVehicle.getID()
            << " mySpeedGainProbability=" << mySpeedGainProbability
            << " myKeepRightProbability=" << myKeepRightProbability
            << " thisLaneVSafe=" << thisLaneVSafe
            << " neighLaneVSafe=" << neighLaneVSafe
            << "\n";
    }
    return ret;
}


void 
MSLCM_JE2013::keepRight(MSVehicle* neigh) {
    if (neigh!= 0 && neigh->getSpeed() > myVehicle.getSpeed()) {
        const SUMOReal dProb = (neigh->getSpeed() - myVehicle.getSpeed()) / neigh->getSpeed();
        myKeepRightProbability -= dProb;
        mySpeedGainProbability -= dProb;
    }
}


int 
MSLCM_JE2013::slowDownForBlocked(MSVehicle** blocked, int state) {
    //  if this vehicle is blocking someone in front, we maybe decelerate to let him in
    if ((*blocked) != 0) {
        SUMOReal gap = (*blocked)->getPositionOnLane() - (*blocked)->getVehicleType().getLength() - myVehicle.getPositionOnLane() - myVehicle.getVehicleType().getMinGap();
        if (MSGlobals::gDebugFlag2) {
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
    if (blocker != 0 && (blocker->getLaneChangeModel().getOwnState() & lcaCounter) != 0) {
        // is there enough space in front of us for the blocker?
        const SUMOReal potential = myLeftSpace - myVehicle.getCarFollowModel().brakeGap(
                myVehicle.getSpeed(), myVehicle.getCarFollowModel().getMaxDecel(), 0);
        if (blocker->getVehicleType().getLengthWithGap() <= potential) {
            // save at least his length in myLeadingBlockerLength
            myLeadingBlockerLength = MAX2(blocker->getVehicleType().getLengthWithGap(), myLeadingBlockerLength);
            if (MSGlobals::gDebugFlag2) {
                std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                    << " veh=" << myVehicle.getID()
                    << " blocker=" << tryID(blocker)
                    << " saving myLeadingBlockerLength=" << myLeadingBlockerLength
                    << "\n";
            }
        } else {
            // we cannot save enough space for the blocker. It needs to save
            // space for ego instead
            if (MSGlobals::gDebugFlag2) {
                std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
                    << " veh=" << myVehicle.getID()
                    << " blocker=" << tryID(blocker)
                    << " cannot save space=" << blocker->getVehicleType().getLengthWithGap()
                    << "\n";
            }
            blocker->getLaneChangeModel().saveBlockerLength(myVehicle.getVehicleType().getLengthWithGap());
        }
    }
}
/****************************************************************************/

