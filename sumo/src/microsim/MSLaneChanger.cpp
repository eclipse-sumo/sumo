/****************************************************************************/
/// @file    MSLaneChanger.cpp
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @author  Friedemann Wesner
/// @author  Jakob Erdmann
/// @date    Fri, 01 Feb 2002
/// @version $Id$
///
// Performs lane changing of vehicles
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2002-2017 DLR (http://www.dlr.de/) and contributors
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

#include "MSLaneChanger.h"
#include "MSNet.h"
#include "MSVehicle.h"
#include "MSVehicleType.h"
#include "MSVehicleTransfer.h"
#include "MSGlobals.h"
#include <cassert>
#include <iterator>
#include <cstdlib>
#include <cmath>
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include <utils/common/MsgHandler.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

#define OPPOSITE_OVERTAKING_SAFE_TIMEGAP 0.0
// XXX maxLookAhead should be higher if all leaders are stopped and lower when they are jammed/queued
#define OPPOSITE_OVERTAKING_MAX_LOOKAHEAD 150.0 // just a guess
// this is used for finding oncoming vehicles while driving in the opposite direction
#define OPPOSITE_OVERTAKING_ONCOMING_LOOKAHEAD 200.0 // just a guess

// ===========================================================================
// debug defines
// ===========================================================================
//#define DEBUG_VEHICLE_GUI_SELECTION

//#define DEBUG_CONTINUE_CHANGE
//#define DEBUG_CHECK_CHANGE
//#define DEBUG_SURROUNDING_VEHICLES // debug getRealFollower() and getRealLeader()
//#define DEBUG_CHANGE_OPPOSITE
//#define DEBUG_COND (vehicle->getLaneChangeModel().debugVehicle())
#define DEBUG_COND false



// ===========================================================================
// ChangeElem member method definitions
// ===========================================================================
MSLaneChanger::ChangeElem::ChangeElem(MSLane* _lane) :
    lead(0),
    lane(_lane),
    hoppedVeh(0),
    lastBlocked(0),
    firstBlocked(0),
    ahead(lane) {
}

// ===========================================================================
// member method definitions
// ===========================================================================
MSLaneChanger::MSLaneChanger(const std::vector<MSLane*>* lanes, bool allowChanging) :
    myAllowsChanging(allowChanging),
    myChangeToOpposite(lanes->front()->getEdge().canChangeToOpposite()) {

    // Fill the changer with the lane-data.
    myChanger.reserve(lanes->size());
    for (std::vector<MSLane*>::const_iterator lane = lanes->begin(); lane != lanes->end(); ++lane) {
        myChanger.push_back(ChangeElem(*lane));
    }
}


MSLaneChanger::~MSLaneChanger() {
}


void
MSLaneChanger::laneChange(SUMOTime t) {
    // This is what happens in one timestep. After initialization of the
    // changer, each vehicle will try to change. After that the changer
    // needs an update to prevent multiple changes of one vehicle.
    // Finally, the change-result has to be given back to the lanes.
    initChanger();
    try {
        while (vehInChanger()) {
            const bool haveChanged = change();
            updateChanger(haveChanged);
        }
        updateLanes(t);
    } catch (const ProcessError&) {
        // clean up locks or the gui may hang
        for (ChangerIt ce = myChanger.begin(); ce != myChanger.end(); ++ce) {
            ce->lane->releaseVehicles();
        }
        throw;
    }
}


void
MSLaneChanger::initChanger() {
    // Prepare myChanger with a safe state.
    for (ChangerIt ce = myChanger.begin(); ce != myChanger.end(); ++ce) {
        ce->lead = 0;
        ce->hoppedVeh = 0;
        ce->lastBlocked = 0;
        ce->firstBlocked = 0;
        ce->dens = 0;
        ce->ahead.clear();
        ce->lane->getVehiclesSecure();

        //std::cout << SIMTIME << " initChanger lane=" << ce->lane->getID() << " vehicles=" << toString(ce->lane->myVehicles) << "\n";

    }
}


void
MSLaneChanger::updateChanger(bool vehHasChanged) {
    assert(veh(myCandi) != 0);

    // "Push" the vehicles to the back, i.e. follower becomes vehicle,
    // vehicle becomes leader, and leader becomes predecessor of vehicle,
    // if it exists.
    if (!vehHasChanged || MSGlobals::gLaneChangeDuration > DELTA_T) {
        //std::cout << SIMTIME << " updateChanger: lane=" << myCandi->lane->getID() << " has new lead=" << veh(myCandi)->getID() << "\n";
        myCandi->lead = veh(myCandi);
    }

    MSLane::VehCont& vehicles = myCandi->lane->myVehicles;
    vehicles.pop_back();
    //std::cout << SIMTIME << " updateChanger lane=" << myCandi->lane->getID() << " vehicles=" << toString(myCandi->lane->myVehicles) << "\n";
}


void
MSLaneChanger::updateLanes(SUMOTime t) {

    // Update the lane's vehicle-container.
    // First: it is bad style to change other classes members, but for
    // this release, other attempts were too time-consuming. In a next
    // release we will change from this lane-centered design to a vehicle-
    // centered. This will solve many problems.
    // Second: this swap would be faster if vehicle-containers would have
    // been pointers, but then I had to change too much of the MSLane code.
    for (ChangerIt ce = myChanger.begin(); ce != myChanger.end(); ++ce) {
        //std::cout << SIMTIME << " updateLanes lane=" << ce->lane->getID() << " myVehicles=" << toString(ce->lane->myVehicles) << " myTmpVehicles=" << toString(ce->lane->myTmpVehicles) << "\n";
        ce->lane->swapAfterLaneChange(t);
        ce->lane->releaseVehicles();
    }
}


MSLaneChanger::ChangerIt
MSLaneChanger::findCandidate() {
    // Find the vehicle in myChanger with the largest position. If there
    // is no vehicle in myChanger (shouldn't happen) , return myChanger.end().
    ChangerIt max = myChanger.end();
    for (ChangerIt ce = myChanger.begin(); ce != myChanger.end(); ++ce) {
        if (veh(ce) == 0) {
            continue;
        }
        if (max == myChanger.end()) {
            //std::cout << SIMTIME << " new max vehicle=" << veh(ce)->getID() << " pos=" << veh(ce)->getPositionOnLane() << " lane=" << ce->lane->getID() << " isFrontOnLane=" << veh(ce)->isFrontOnLane(ce->lane)  << "\n";
            max = ce;
            continue;
        }
        assert(veh(ce)  != 0);
        assert(veh(max) != 0);
        if (veh(max)->getPositionOnLane() < veh(ce)->getPositionOnLane()) {
            //std::cout << SIMTIME << " new max vehicle=" << veh(ce)->getID() << " pos=" << veh(ce)->getPositionOnLane() << " lane=" << ce->lane->getID() << " isFrontOnLane=" << veh(ce)->isFrontOnLane(ce->lane)  << " oldMaxPos=" << veh(max)->getPositionOnLane() << "\n";
            max = ce;
        }
    }
    assert(max != myChanger.end());
    assert(veh(max) != 0);
    return max;
}


bool
MSLaneChanger::mayChange(int direction) const {
    if (direction == 0) {
        return true;
    }
    if (!myAllowsChanging) {
        return false;
    }
    if (direction == -1) {
        return (myCandi != myChanger.begin()    && (myCandi - 1)->lane->allowsVehicleClass(veh(myCandi)->getVehicleType().getVehicleClass()));
    } else if (direction == 1) {
        return (myCandi + 1) != myChanger.end() && (myCandi + 1)->lane->allowsVehicleClass(veh(myCandi)->getVehicleType().getVehicleClass());
    } else {
        return false;
    }
}


bool
MSLaneChanger::change() {
    // Find change-candidate. If it is on an allowed lane, try to change
    // to the right (there is a rule in Germany that you have to change
    // to the right, unless you are overtaking). If change to the right
    // isn't possible, check if there is a possibility to overtake (on the
    // left.
    // If candidate isn't on an allowed lane, changing to an allowed has
    // priority.
    myCandi = findCandidate();
    MSVehicle* vehicle = veh(myCandi);

#ifdef DEBUG_VEHICLE_GUI_SELECTION
    if (gDebugSelectedVehicle == vehicle->getID()) {
        int bla = 0;
    }
#endif
    if (vehicle->getLaneChangeModel().isChangingLanes()) {
        return continueChange(vehicle, myCandi);
    }
    if (!myAllowsChanging || vehicle->getLaneChangeModel().alreadyChanged()) {
        registerUnchanged(vehicle);
        return false;
    }
    std::pair<MSVehicle* const, SUMOReal> leader = getRealLeader(myCandi);
    if (myChanger.size() == 1 || vehicle->getLaneChangeModel().isOpposite()) {
        if (changeOpposite(leader)) {
            return true;
        }
        registerUnchanged(vehicle);
        return false;
    }

#ifndef NO_TRACI
    if (vehicle->isRemoteControlled()) {
        return false; // !!! temporary; just because it broke, here
    }
#endif
    vehicle->updateBestLanes(); // needed?
    for (int i = 0; i < (int) myChanger.size(); ++i) {
        vehicle->adaptBestLanesOccupation(i, myChanger[i].dens);
    }
    const std::vector<MSVehicle::LaneQ>& preb = vehicle->getBestLanes();
    // check whether the vehicle wants and is able to change to right lane
    int state1 = 0;
    if (mayChange(-1)) {
        state1 = checkChangeWithinEdge(-1, leader, preb);
        bool changingAllowed1 = (state1 & LCA_BLOCKED) == 0;
        // change if the vehicle wants to and is allowed to change
        if ((state1 & LCA_RIGHT) != 0 && changingAllowed1) {
            vehicle->getLaneChangeModel().setOwnState(state1);
            startChange(vehicle, myCandi, -1);
            return true;
        }
        if ((state1 & LCA_RIGHT) != 0 && (state1 & LCA_URGENT) != 0) {
            (myCandi - 1)->lastBlocked = vehicle;
            if ((myCandi - 1)->firstBlocked == 0) {
                (myCandi - 1)->firstBlocked = vehicle;
            }
        }
    }



    // check whether the vehicle wants and is able to change to left lane
    int state2 = 0;
    if (mayChange(1)) {
        state2 = checkChangeWithinEdge(1, leader, preb);
        bool changingAllowed2 = (state2 & LCA_BLOCKED) == 0;
        // change if the vehicle wants to and is allowed to change
        if ((state2 & LCA_LEFT) != 0 && changingAllowed2) {
            vehicle->getLaneChangeModel().setOwnState(state2);
            startChange(vehicle, myCandi, 1);
            return true;
        }
        if ((state2 & LCA_LEFT) != 0 && (state2 & LCA_URGENT) != 0) {
            (myCandi + 1)->lastBlocked = vehicle;
            if ((myCandi + 1)->firstBlocked == 0) {
                (myCandi + 1)->firstBlocked = vehicle;
            }
        }
    }

    if ((state1 & (LCA_URGENT)) != 0 && (state2 & (LCA_URGENT)) != 0) {
        // ... wants to go to the left AND to the right
        // just let them go to the right lane...
        state2 = 0;
    }
    vehicle->getLaneChangeModel().setOwnState(state2 | state1);

    // only emergency vehicles should change to the opposite side on a
    // multi-lane road
    if (vehicle->getVehicleType().getVehicleClass() == SVC_EMERGENCY
            && changeOpposite(leader)) {
        return true;
    } else {
        registerUnchanged(vehicle);
        return false;
    }
}


void
MSLaneChanger::registerUnchanged(MSVehicle* vehicle) {
    myCandi->lane->myTmpVehicles.insert(myCandi->lane->myTmpVehicles.begin(), veh(myCandi));
    myCandi->dens += vehicle->getVehicleType().getLengthWithGap();
    vehicle->getLaneChangeModel().unchanged();
}


void
MSLaneChanger::startChange(MSVehicle* vehicle, ChangerIt& from, int direction) {
    ChangerIt to = from + direction;
    // @todo delay entering the target lane until the vehicle intersects it
    //       physically (considering lane width and vehicle width)
    //if (to->lane->getID() == "beg_1") std::cout << SIMTIME << " startChange to lane=" << to->lane->getID() << " myTmpVehiclesBefore=" << toString(to->lane->myTmpVehicles) << "\n";
    const bool continuous = vehicle->getLaneChangeModel().startLaneChangeManeuver(from->lane, to->lane, direction);
    if (continuous) {
        continueChange(vehicle, myCandi);
    } else {
        to->lane->myTmpVehicles.insert(to->lane->myTmpVehicles.begin(), vehicle);
        to->dens += vehicle->getVehicleType().getLengthWithGap();
        to->hoppedVeh = vehicle;
    }
}


bool
MSLaneChanger::continueChange(MSVehicle* vehicle, ChangerIt& from) {
    MSAbstractLaneChangeModel& lcm = vehicle->getLaneChangeModel();
    const int direction = lcm.getLaneChangeDirection();
    const bool pastMidpoint = lcm.updateCompletion();
    vehicle->myState.myPosLat += lcm.getLateralSpeed();
    vehicle->myCachedPosition = Position::INVALID;
    if (pastMidpoint) {
        ChangerIt to = from + direction;
        MSLane* source = myCandi->lane;
        MSLane* target = to->lane;
        vehicle->myState.myPosLat -= direction * 0.5 * (source->getWidth() + target->getWidth());
        lcm.primaryLaneChanged(source, target, direction);
        to->lane->myTmpVehicles.insert(to->lane->myTmpVehicles.begin(), vehicle);
        to->dens += vehicle->getVehicleType().getLengthWithGap();
        to->hoppedVeh = vehicle;
    } else {
        from->lane->myTmpVehicles.insert(from->lane->myTmpVehicles.begin(), vehicle);
        from->dens += vehicle->getVehicleType().getLengthWithGap();
        from->hoppedVeh = vehicle;
    }
    if (!lcm.isChangingLanes()) {
        vehicle->myState.myPosLat = 0;
        lcm.endLaneChangeManeuver();
    }
    lcm.updateShadowLane();
    if (lcm.getShadowLane() != 0) {
        // set as hoppedVeh on the shadow lane so it is found as leader on both lanes
        ChangerIt shadow = pastMidpoint ? from : from + lcm.getShadowDirection();
        shadow->hoppedVeh = vehicle;
    }
    vehicle->myAngle = vehicle->computeAngle();

#ifdef DEBUG_CONTINUE_CHANGE
    if (DEBUG_COND) {
        std::cout << SIMTIME
                  << " continueChange veh=" << vehicle->getID()
                  << " from=" << Named::getIDSecure(from->lane)
                  << " dir=" << direction
                  << " pastMidpoint=" << pastMidpoint
                  << " posLat=" << vehicle->getLateralPositionOnLane()
                  //<< " completion=" << lcm.getLaneChangeCompletion()
                  << " shadowLane=" << Named::getIDSecure(lcm.getShadowLane())
                  << " shadowHopped=" << Named::getIDSecure(shadow->lane)
                  << "\n";
    }
#endif
    return pastMidpoint;
}


std::pair<MSVehicle* const, SUMOReal>
MSLaneChanger::getRealLeader(const ChangerIt& target) const {
    assert(veh(myCandi) != 0);

#ifdef DEBUG_SURROUNDING_VEHICLES
    MSVehicle* vehicle = veh(myCandi);
    if (DEBUG_COND) {
        std::cout << SIMTIME << " veh '" << vehicle->getID() << "' looks for leader on lc-target lane '" << target->lane->getID() << "'." << std::endl;
    }
#endif
    // get the leading vehicle on the lane to change to
    MSVehicle* neighLead = target->lead;

#ifdef DEBUG_SURROUNDING_VEHICLES
    if (DEBUG_COND) {
        if (neighLead != 0) {
            std::cout << "Considering '" << neighLead->getID() << "' at position " << neighLead->getPositionOnLane() << std::endl;
        }
    }
#endif

    //if (veh(myCandi)->getID() == "disabled") std::cout << SIMTIME
    //    << " target=" << target->lane->getID()
    //    << " neighLead=" << Named::getIDSecure(neighLead)
    //    << " hopped=" << Named::getIDSecure(target->hoppedVeh)
    //        << " (416)\n";
    // check whether the hopped vehicle became the leader
    if (target->hoppedVeh != 0) {
        SUMOReal hoppedPos = target->hoppedVeh->getPositionOnLane();
#ifdef DEBUG_SURROUNDING_VEHICLES
        if (DEBUG_COND) {
            std::cout << "Considering hopped vehicle '" << target->hoppedVeh->getID() << "' at position " << hoppedPos << std::endl;
        }
#endif
        if (hoppedPos > veh(myCandi)->getPositionOnLane() && (neighLead == 0 || neighLead->getPositionOnLane() > hoppedPos)) {
            neighLead = target->hoppedVeh;
            //if (veh(myCandi)->getID() == "flow.21") std::cout << SIMTIME << " neighLead=" << Named::getIDSecure(neighLead) << " (422)\n";
        }
    }
    if (neighLead == 0) {
#ifdef DEBUG_SURROUNDING_VEHICLES
        if (DEBUG_COND) {
            std::cout << "Looking for leader on consecutive lanes." << std::endl;
        }
#endif
        // There's no leader on the target lane. Look for leaders on consecutive lanes.
        MSLane* targetLane = target->lane;
        if (targetLane->myPartialVehicles.size() > 0) {
            assert(targetLane->myPartialVehicles.size() > 0);
            std::vector<MSVehicle*>::const_iterator i = targetLane->myPartialVehicles.begin();
            MSVehicle* leader = *i;
            SUMOReal leaderPos = leader->getBackPositionOnLane(targetLane);
            while (++i != targetLane->myPartialVehicles.end()) {
                if ((*i)->getBackPositionOnLane(targetLane) < leader->getBackPositionOnLane(targetLane)) {
                    leader = *i;
                    leaderPos = leader->getBackPositionOnLane(targetLane);
                }
            }
            return std::pair<MSVehicle*, SUMOReal>(leader, leaderPos - veh(myCandi)->getPositionOnLane() - veh(myCandi)->getVehicleType().getMinGap());
        }
        SUMOReal seen = myCandi->lane->getLength() - veh(myCandi)->getPositionOnLane();
        SUMOReal speed = veh(myCandi)->getSpeed();
        SUMOReal dist = veh(myCandi)->getCarFollowModel().brakeGap(speed) + veh(myCandi)->getVehicleType().getMinGap();
        if (seen > dist) {
            return std::pair<MSVehicle* const, SUMOReal>(static_cast<MSVehicle*>(0), -1);
        }
        const std::vector<MSLane*>& bestLaneConts = veh(myCandi)->getBestLanesContinuation(targetLane);
        return target->lane->getLeaderOnConsecutive(dist, seen, speed, *veh(myCandi), bestLaneConts);
    } else {
        MSVehicle* candi = veh(myCandi);
        return std::pair<MSVehicle* const, SUMOReal>(neighLead, neighLead->getBackPositionOnLane(target->lane) - candi->getPositionOnLane() - candi->getVehicleType().getMinGap());
    }
}


std::pair<MSVehicle* const, SUMOReal>
MSLaneChanger::getRealFollower(const ChangerIt& target) const {
    assert(veh(myCandi) != 0);

#ifdef DEBUG_SURROUNDING_VEHICLES
    MSVehicle* vehicle = veh(myCandi);
    if (DEBUG_COND) {
        std::cout << SIMTIME << " veh '" << vehicle->getID() << "' looks for follower on lc-target lane '" << target->lane->getID() << "'." << std::endl;
    }
#endif
    MSVehicle* candi = veh(myCandi);
    const SUMOReal candiPos = candi->getPositionOnLane();
    MSVehicle* neighFollow = veh(target);

#ifdef DEBUG_SURROUNDING_VEHICLES
    if (DEBUG_COND) {
        if (neighFollow != 0) {
            std::cout << "veh(target) returns '" << neighFollow->getID() << "' at position " << neighFollow->getPositionOnLane() << std::endl;
        } else {
            std::cout << "veh(target) returns none." << std::endl;
        }
    }
#endif


#ifdef DEBUG_SURROUNDING_VEHICLES
    if (DEBUG_COND) {
        if (getCloserFollower(candiPos, neighFollow, target->hoppedVeh) != neighFollow) {
            std::cout << "Hopped vehicle '" << target->hoppedVeh->getID() << "' at position " << target->hoppedVeh->getPositionOnLane() << " is closer." <<  std::endl;
        }
    }
#endif

    // check whether the hopped vehicle became the follower
    neighFollow = getCloserFollower(candiPos, neighFollow, target->hoppedVeh);


#ifdef DEBUG_SURROUNDING_VEHICLES
    if (DEBUG_COND) {
        MSVehicle* partialBehind = getCloserFollower(candiPos, neighFollow, target->lane->getPartialBehind(candi));
        if (partialBehind != 0 && partialBehind != neighFollow) {
            std::cout << "'Partial behind'-vehicle '" << target->lane->getPartialBehind(candi)->getID() << "' at position " << target->hoppedVeh->getPositionOnLane() << " is closer." <<  std::endl;
        }
    }
#endif
    // or a follower which is partially lapping into the target lane
    neighFollow = getCloserFollower(candiPos, neighFollow, target->lane->getPartialBehind(candi));

    if (neighFollow == 0) {
        std::pair<MSVehicle* const, SUMOReal> consecutiveFollower = target->lane->getFollowerOnConsecutive(
                    candi->getPositionOnLane() - candi->getVehicleType().getLength(),
                    candi->getSpeed(), candi->getCarFollowModel().getMaxDecel());
#ifdef DEBUG_SURROUNDING_VEHICLES
        if (DEBUG_COND) {
            if (consecutiveFollower.first == 0) {
                std::cout << "no follower found." <<  std::endl;
            } else {
                std::cout << "found follower '" << consecutiveFollower.first->getID() << "' on consecutive lanes." <<  std::endl;
            }
        }
#endif
        return consecutiveFollower;
    } else {
#ifdef DEBUG_SURROUNDING_VEHICLES
        if (DEBUG_COND) {
            std::cout << "found follower '" << neighFollow->getID() << "'." <<  std::endl;
        }
#endif
        MSVehicle* candi = veh(myCandi);
        return std::pair<MSVehicle* const, SUMOReal>(neighFollow,
                candi->getPositionOnLane() - candi->getVehicleType().getLength() - neighFollow->getPositionOnLane() - neighFollow->getVehicleType().getMinGap());
    }
}


MSVehicle*
MSLaneChanger::getCloserFollower(const SUMOReal maxPos, MSVehicle* follow1, MSVehicle* follow2) {
    if (follow1 == 0 || follow1->getPositionOnLane() > maxPos) {
        return follow2;
    } else if (follow2 == 0 || follow2->getPositionOnLane() > maxPos) {
        return follow1;
    } else {
        if (follow1->getPositionOnLane() > follow2->getPositionOnLane()) {
            return follow1;
        } else {
            return follow2;
        }
    }
}

int
MSLaneChanger::checkChangeWithinEdge(
    int laneOffset,
    const std::pair<MSVehicle* const, SUMOReal>& leader,
    const std::vector<MSVehicle::LaneQ>& preb) const {

    std::pair<MSVehicle* const, SUMOReal> neighLead = getRealLeader(myCandi + laneOffset);
    std::pair<MSVehicle*, SUMOReal> neighFollow = getRealFollower(myCandi + laneOffset);
    if (neighLead.first != 0 && neighLead.first == neighFollow.first) {
        // vehicles should not be leader and follower at the same time to avoid
        // contradictory behavior
        neighFollow.first = 0;
    }
    ChangerIt target = myCandi + laneOffset;
    return checkChange(laneOffset, target->lane, leader, neighLead, neighFollow, preb);
}

int
MSLaneChanger::checkChange(
    int laneOffset,
    const MSLane* targetLane,
    const std::pair<MSVehicle* const, SUMOReal>& leader,
    const std::pair<MSVehicle* const, SUMOReal>& neighLead,
    const std::pair<MSVehicle* const, SUMOReal>& neighFollow,
    const std::vector<MSVehicle::LaneQ>& preb) const {

    MSVehicle* vehicle = veh(myCandi);

    // Debug (Leo)
#ifdef DEBUG_CHECK_CHANGE
    if (DEBUG_COND) {
        std::cout
                << "\n" << SIMTIME << " checkChange() for vehicle '" << vehicle->getID() << "'"
                << std::endl;
    }
#endif


    int blocked = 0;
    int blockedByLeader = (laneOffset == -1 ? LCA_BLOCKED_BY_RIGHT_LEADER : LCA_BLOCKED_BY_LEFT_LEADER);
    int blockedByFollower = (laneOffset == -1 ? LCA_BLOCKED_BY_RIGHT_FOLLOWER : LCA_BLOCKED_BY_LEFT_FOLLOWER);
    // overlap
    if (neighFollow.first != 0 && neighFollow.second < 0) {
        blocked |= (blockedByFollower | LCA_OVERLAPPING);

        // Debug (Leo)
#ifdef DEBUG_CHECK_CHANGE
        if (DEBUG_COND) {
            std::cout << SIMTIME
                      << " overlapping with follower..."
                      << std::endl;
        }
#endif

    }
    if (neighLead.first != 0 && neighLead.second < 0) {
        blocked |= (blockedByLeader | LCA_OVERLAPPING);

        // Debug (Leo)
#ifdef DEBUG_CHECK_CHANGE
        if (DEBUG_COND) {
            std::cout << SIMTIME
                      <<  " overlapping with leader..."
                      << std::endl;
        }
#endif

    }

    // safe back gap
    if ((blocked & blockedByFollower) == 0 && neighFollow.first != 0) {
        // !!! eigentlich: vsafe braucht die Max. Geschwindigkeit beider Spuren
        if (neighFollow.second < neighFollow.first->getCarFollowModel().getSecureGap(neighFollow.first->getSpeed(), vehicle->getSpeed(), vehicle->getCarFollowModel().getMaxDecel())) {
            blocked |= blockedByFollower;

            // Debug (Leo)
#ifdef DEBUG_CHECK_CHANGE
            if (DEBUG_COND) {
                std::cout << SIMTIME
                          << " back gap unsafe: "
                          << "gap = " << neighFollow.second
                          << ", secureGap = "
                          << neighFollow.first->getCarFollowModel().getSecureGap(neighFollow.first->getSpeed(),
                                  vehicle->getSpeed(), vehicle->getCarFollowModel().getMaxDecel())
                          << std::endl;
            }
#endif

        }
    }

    // safe front gap
    if ((blocked & blockedByLeader) == 0 && neighLead.first != 0) {
        // !!! eigentlich: vsafe braucht die Max. Geschwindigkeit beider Spuren
        if (neighLead.second < vehicle->getCarFollowModel().getSecureGap(vehicle->getSpeed(), neighLead.first->getSpeed(), neighLead.first->getCarFollowModel().getMaxDecel())) {
            blocked |= blockedByLeader;

            // Debug (Leo)
#ifdef DEBUG_CHECK_CHANGE
            if (DEBUG_COND) {
                std::cout << SIMTIME
                          << " front gap unsafe: "
                          << "gap = " << neighLead.second
                          << ", secureGap = "
                          << vehicle->getCarFollowModel().getSecureGap(vehicle->getSpeed(),
                                  neighLead.first->getSpeed(), neighLead.first->getCarFollowModel().getMaxDecel())
                          << std::endl;
            }
#endif

        }
    }


    MSAbstractLaneChangeModel::MSLCMessager msg(leader.first, neighLead.first, neighFollow.first);
    int state = blocked | vehicle->getLaneChangeModel().wantsChange(
                    laneOffset, msg, blocked, leader, neighLead, neighFollow, *targetLane, preb, &(myCandi->lastBlocked), &(myCandi->firstBlocked));

    if (blocked == 0 && (state & LCA_WANTS_LANECHANGE) != 0 && neighLead.first != 0) {
        // do are more carefull (but expensive) check to ensure that a
        // safety-critical leader is not being overloocked
        const SUMOReal seen = myCandi->lane->getLength() - vehicle->getPositionOnLane();
        const SUMOReal speed = vehicle->getSpeed();
        const SUMOReal dist = vehicle->getCarFollowModel().brakeGap(speed) + vehicle->getVehicleType().getMinGap();
        if (seen < dist) {
            std::pair<MSVehicle* const, SUMOReal> neighLead2 = targetLane->getCriticalLeader(dist, seen, speed, *vehicle);
            if (neighLead2.first != 0 && neighLead2.first != neighLead.first
                    && (neighLead2.second < vehicle->getCarFollowModel().getSecureGap(
                            vehicle->getSpeed(), neighLead2.first->getSpeed(), neighLead2.first->getCarFollowModel().getMaxDecel()))) {
                state |= blockedByLeader;
            }
        }
    }
    if (blocked == 0 && (state & LCA_WANTS_LANECHANGE)) {
        // ensure that merging is safe for any upcoming zipper links after changing
        if (vehicle->unsafeLinkAhead(targetLane)) {
            state |= blockedByLeader;
        }
    }

    if ((state & LCA_BLOCKED) == 0 && (state & LCA_WANTS_LANECHANGE) != 0 && MSGlobals::gLaneChangeDuration > DELTA_T) {
        // ensure that a continuous lane change manoeuvre can be completed
        // before the next turning movement
        SUMOReal seen = myCandi->lane->getLength() - vehicle->getPositionOnLane();
        const SUMOReal decel = vehicle->getCarFollowModel().getMaxDecel() * STEPS2TIME(MSGlobals::gLaneChangeDuration);
        const SUMOReal avgSpeed = 0.5 * (
                                      MAX2((SUMOReal)0, vehicle->getSpeed() - ACCEL2SPEED(vehicle->getCarFollowModel().getMaxDecel())) +
                                      MAX2((SUMOReal)0, vehicle->getSpeed() - decel));
        const SUMOReal space2change = avgSpeed * STEPS2TIME(MSGlobals::gLaneChangeDuration);
        // for finding turns it doesn't matter whether we look along the current lane or the target lane
        const std::vector<MSLane*>& bestLaneConts = vehicle->getBestLanesContinuation();
        int view = 1;
        MSLane* nextLane = vehicle->getLane();
        MSLinkCont::const_iterator link = MSLane::succLinkSec(*vehicle, view, *nextLane, bestLaneConts);
        while (!nextLane->isLinkEnd(link) && seen <= space2change) {
            if ((*link)->getDirection() == LINKDIR_LEFT || (*link)->getDirection() == LINKDIR_RIGHT
                    // the lanes after an internal junction are on different
                    // edges and do not allow lane-changing
                    || (nextLane->getEdge().isInternal() && (*link)->getViaLaneOrLane()->getEdge().isInternal())
               ) {
                state |= LCA_INSUFFICIENT_SPACE;
                break;
            }
#ifdef HAVE_INTERNAL_LANES
            if ((*link)->getViaLane() == 0) {
                view++;
            }
#else
            view++;
#endif
            nextLane = (*link)->getViaLaneOrLane();
            seen += nextLane->getLength();
            // get the next link used
            link = MSLane::succLinkSec(*vehicle, view, *nextLane, bestLaneConts);
        }
        if (nextLane->isLinkEnd(link) && seen < space2change) {
#ifdef DEBUG_CHECK_CHANGE
            if (DEBUG_COND) {
                std::cout << SIMTIME << " checkChange insufficientSpace: seen=" << seen << " space2change=" << space2change << "\n";
            }
#endif
            state |= LCA_INSUFFICIENT_SPACE;
        }

        if ((state & LCA_BLOCKED) == 0) {
            // check for dangerous leaders in case the target lane changes laterally between
            // now and the lane-changing midpoint
            const SUMOReal speed = vehicle->getSpeed();
            seen = myCandi->lane->getLength() - vehicle->getPositionOnLane();
            nextLane = vehicle->getLane();
            view = 1;
            const SUMOReal dist = vehicle->getCarFollowModel().brakeGap(speed) + vehicle->getVehicleType().getMinGap();
            MSLinkCont::const_iterator link = MSLane::succLinkSec(*vehicle, view, *nextLane, bestLaneConts);
            while (!nextLane->isLinkEnd(link) && seen <= space2change && seen <= dist) {
                nextLane = (*link)->getViaLaneOrLane();
                MSLane* targetLane = nextLane->getParallelLane(laneOffset);
                if (targetLane == 0) {
                    state |= LCA_INSUFFICIENT_SPACE;
                    break;
                } else {
                    std::pair<MSVehicle* const, SUMOReal> neighLead2 = targetLane->getLeader(vehicle, -seen, std::vector<MSLane*>());
                    if (neighLead2.first != 0 && neighLead2.first != neighLead.first
                            && (neighLead2.second < vehicle->getCarFollowModel().getSecureGap(
                                    vehicle->getSpeed(), neighLead2.first->getSpeed(), neighLead2.first->getCarFollowModel().getMaxDecel()))) {
                        state |= blockedByLeader;
                        break;
                    }
                }
#ifdef HAVE_INTERNAL_LANES
                if ((*link)->getViaLane() == 0) {
                    view++;
                }
#else
                view++;
#endif
                seen += nextLane->getLength();
                // get the next link used
                link = MSLane::succLinkSec(*vehicle, view, *nextLane, bestLaneConts);
            }
        }
    }
#ifndef NO_TRACI
#ifdef DEBUG_CHECK_CHANGE
    const int oldstate = state;
#endif
    // let TraCI influence the wish to change lanes and the security to take
    state = vehicle->influenceChangeDecision(state);
#endif
#ifdef DEBUG_CHECK_CHANGE
    if (DEBUG_COND) {
        std::cout << SIMTIME
                  << " veh=" << vehicle->getID()
                  << " oldState=" << toString((LaneChangeAction)oldstate)
                  << " newState=" << toString((LaneChangeAction)state)
                  << ((blocked & LCA_BLOCKED) ? " (blocked)" : "")
                  << ((blocked & LCA_OVERLAPPING) ? " (overlap)" : "")
                  << "\n";
    }
#endif
    return state;
}


bool
MSLaneChanger::changeOpposite(std::pair<MSVehicle*, SUMOReal> leader) {
    if (!myChangeToOpposite) {
        return false;
    }
    myCandi = findCandidate();
    MSVehicle* vehicle = veh(myCandi);
    MSLane* source = vehicle->getLane();
    if (vehicle->isStopped()) {
        // stopped vehicles obviously should not change lanes. Usually this is
        // prevent by appropriate bestLane distances
        return false;
    }
    const bool isOpposite = vehicle->getLaneChangeModel().isOpposite();
    if (!isOpposite && leader.first == 0) {
        // no reason to change unless there is a leader
        // or we are changing back to the propper direction
        // XXX also check whether the leader is so far away as to be irrelevant
        return false;
    }
    MSLane* opposite = source->getOpposite();
    if (opposite == 0) {
        return false;
    }

    // changing into the opposite direction is always to the left (XXX except for left-hand networkds)
    int direction = isOpposite ? -1 : 1;
    std::pair<MSVehicle*, SUMOReal> neighLead((MSVehicle*)0, -1);

    // preliminary sanity checks for overtaking space
    SUMOReal timeToOvertake;
    SUMOReal spaceToOvertake;
    if (!isOpposite) {
        assert(leader.first != 0);
        // find a leader vehicle with sufficient space ahead for merging back
        const SUMOReal overtakingSpeed = source->getVehicleMaxSpeed(vehicle); // just a guess
        const SUMOReal mergeBrakeGap = vehicle->getCarFollowModel().brakeGap(overtakingSpeed);
        std::pair<MSVehicle*, SUMOReal> columnLeader = leader;
        SUMOReal egoGap = leader.second;
        bool foundSpaceAhead = false;
        SUMOReal seen = leader.second + leader.first->getVehicleType().getLengthWithGap();
        std::vector<MSLane*> conts = vehicle->getBestLanesContinuation();
        while (!foundSpaceAhead) {
            const SUMOReal requiredSpaceAfterLeader = (columnLeader.first->getCarFollowModel().getSecureGap(
                        columnLeader.first->getSpeed(), overtakingSpeed, vehicle->getCarFollowModel().getMaxDecel())
                    + vehicle->getVehicleType().getLengthWithGap());


            // all leader vehicles on the current laneChanger edge are already moved into MSLane::myTmpVehicles
            const bool checkTmpVehicles = (&columnLeader.first->getLane()->getEdge() == &source->getEdge());
            std::pair<MSVehicle* const, SUMOReal> leadLead = columnLeader.first->getLane()->getLeader(
                        columnLeader.first, columnLeader.first->getPositionOnLane(), conts, requiredSpaceAfterLeader + mergeBrakeGap,
                        checkTmpVehicles);

#ifdef DEBUG_CHANGE_OPPOSITE
            if (DEBUG_COND) {
                std::cout << "   leadLead=" << Named::getIDSecure(leadLead.first) << " gap=" << leadLead.second << "\n";
            }
#endif
            if (leadLead.first == 0) {
                foundSpaceAhead = true;
            } else {
                const SUMOReal requiredSpace = (requiredSpaceAfterLeader
                                                + vehicle->getCarFollowModel().getSecureGap(overtakingSpeed, leadLead.first->getSpeed(), leadLead.first->getCarFollowModel().getMaxDecel()));
                if (leadLead.second > requiredSpace) {
                    foundSpaceAhead = true;
                } else {
#ifdef DEBUG_CHANGE_OPPOSITE
                    if (DEBUG_COND) {
                        std::cout << "   not enough space after columnLeader=" << columnLeader.first->getID() << " required=" << requiredSpace << "\n";
                    }
#endif
                    seen += MAX2((SUMOReal)0, leadLead.second) + leadLead.first->getVehicleType().getLengthWithGap();
                    if (seen > OPPOSITE_OVERTAKING_MAX_LOOKAHEAD) {
#ifdef DEBUG_CHANGE_OPPOSITE
                        if (DEBUG_COND) {
                            std::cout << "   cannot changeOpposite due to insufficient free space after columnLeader (seen=" << seen << " columnLeader=" << columnLeader.first->getID() << ")\n";
                        }
#endif
                        return false;
                    }
                    // see if merging after leadLead is possible
                    egoGap += columnLeader.first->getVehicleType().getLengthWithGap() + leadLead.second;
                    columnLeader = leadLead;
#ifdef DEBUG_CHANGE_OPPOSITE
                    if (DEBUG_COND) {
                        std::cout << "   new columnLeader=" << columnLeader.first->getID() << "\n";
                    }
#endif
                }
            }
        }
#ifdef DEBUG_CHANGE_OPPOSITE
        if (DEBUG_COND) {
            std::cout << "   compute time/space to overtake for columnLeader=" << columnLeader.first->getID() << " gap=" << columnLeader.second << "\n";
        }
#endif
        computeOvertakingTime(vehicle, columnLeader.first, egoGap, timeToOvertake, spaceToOvertake);
        // check for upcoming stops
        if (vehicle->nextStopDist() < spaceToOvertake) {
#ifdef DEBUG_CHANGE_OPPOSITE
            if (DEBUG_COND) {
                std::cout << "   cannot changeOpposite due to upcoming stop (dist=" << vehicle->nextStopDist() << " spaceToOvertake=" << spaceToOvertake << ")\n";
            }
#endif
            return false;
        }
        neighLead = opposite->getOppositeLeader(vehicle, timeToOvertake * opposite->getSpeedLimit() * 2 + spaceToOvertake, true);

#ifdef DEBUG_CHANGE_OPPOSITE
        if (DEBUG_COND) {
            std::cout << SIMTIME
                      << " veh=" << vehicle->getID()
                      << " changeOpposite opposite=" << opposite->getID()
                      << " lead=" << Named::getIDSecure(leader.first)
                      << " timeToOvertake=" << timeToOvertake
                      << " spaceToOvertake=" << spaceToOvertake
                      << "\n";
        }
#endif

        // check for dangerous oncoming leader
        if (neighLead.first != 0) {
            const MSVehicle* oncoming = neighLead.first;

#ifdef DEBUG_CHANGE_OPPOSITE
            if (DEBUG_COND) {
                std::cout << SIMTIME
                          << " oncoming=" << oncoming->getID()
                          << " oncomingGap=" << neighLead.second
                          << " leaderGap=" << leader.second
                          << "\n";
            }
#endif
            if (neighLead.second - spaceToOvertake - timeToOvertake * oncoming->getSpeed() < 0) {

#ifdef DEBUG_CHANGE_OPPOSITE
                if (DEBUG_COND) {
                    std::cout << "   cannot changeOpposite due to dangerous oncoming\n";
                }
#endif
                return false;
            }
        }
    } else {
        timeToOvertake = -1;
        // look forward as far as possible
        spaceToOvertake = std::numeric_limits<SUMOReal>::max();
        leader = source->getOppositeLeader(vehicle, OPPOSITE_OVERTAKING_ONCOMING_LOOKAHEAD, true);
        // -1 will use getMaximumBrakeDist() as look-ahead distance
        neighLead = opposite->getOppositeLeader(vehicle, -1, false);
    }

    // compute remaining space on the opposite side
    // 1. the part that remains on the current lane
    SUMOReal usableDist = isOpposite ? vehicle->getPositionOnLane() : source->getLength() - vehicle->getPositionOnLane();
    if (usableDist < spaceToOvertake) {
        // look forward along the next lanes
        const std::vector<MSLane*>& bestLaneConts = vehicle->getBestLanesContinuation();
        assert(bestLaneConts.size() >= 1);
        std::vector<MSLane*>::const_iterator it = bestLaneConts.begin() + 1;
        while (usableDist < spaceToOvertake && it != bestLaneConts.end()) {
#ifdef DEBUG_CHANGE_OPPOSITE
            if (DEBUG_COND) {
                std::cout << "      usableDist=" << usableDist << " opposite=" << Named::getIDSecure((*it)->getOpposite()) << "\n";
            }
#endif
            if ((*it)->getOpposite() == 0) {
                // opposite lane ends
                break;
            }
            // do not overtake past a minor link or turn
            if (*(it - 1) != 0) {
                MSLink* link = MSLinkContHelper::getConnectingLink(**(it - 1), **it);
                if (link == 0 || !link->havePriority() || link->getState() == LINKSTATE_ZIPPER || link->getDirection() != LINKDIR_STRAIGHT) {
                    break;
                }
            }
            usableDist += (*it)->getLength();
            ++it;
        }
    }
    if (!isOpposite && usableDist < spaceToOvertake) {
#ifdef DEBUG_CHANGE_OPPOSITE
        if (DEBUG_COND) {
            std::cout << "   cannot changeOpposite due to insufficient space (seen=" << usableDist << " spaceToOvertake=" << spaceToOvertake << ")\n";
        }
#endif
        return false;
    }
#ifdef DEBUG_CHANGE_OPPOSITE
    if (DEBUG_COND) {
        std::cout << "   usableDist=" << usableDist << " spaceToOvertake=" << spaceToOvertake << " timeToOvertake=" << timeToOvertake << "\n";
    }
#endif

    // compute wish to change
    std::vector<MSVehicle::LaneQ> preb = vehicle->getBestLanes();
    if (isOpposite) {
        // compute the remaining distance that can be drive on the opposite side
        // this value will put into LaneQ.length of the leftmost lane
        // @note: length counts from the start of the current lane
        // @note: see MSLCM_LC2013::_wantsChange @1092 (isOpposite()
        MSVehicle::LaneQ& laneQ = preb[preb.size() - 1];
        // position on the target lane
        const SUMOReal forwardPos = source->getOppositePos(vehicle->getPositionOnLane());

        // consider usableDist (due to minor links or end of opposite lanes)
        laneQ.length = MIN2(laneQ.length, usableDist + forwardPos);
        // consider upcoming stops
        laneQ.length = MIN2(laneQ.length, vehicle->nextStopDist() + forwardPos);
        // consider oncoming leaders
        if (leader.first != 0) {
            laneQ.length = MIN2(laneQ.length, leader.second / 2 + forwardPos);
#ifdef DEBUG_CHANGE_OPPOSITE
            if (DEBUG_COND) {
                std::cout << SIMTIME << " found oncoming leader=" << leader.first->getID() << " gap=" << leader.second << "\n";
            }
#endif
            leader.first = 0; // ignore leader after this
        }
#ifdef DEBUG_CHANGE_OPPOSITE
        if (DEBUG_COND) {
            std::cout << SIMTIME << " veh=" << vehicle->getID() << " remaining dist=" << laneQ.length - forwardPos << " forwardPos=" << forwardPos << " laneQ.length=" << laneQ.length << "\n";
        }
#endif
    }
    std::pair<MSVehicle* const, SUMOReal> neighFollow = opposite->getOppositeFollower(vehicle);
    int state = checkChange(direction, opposite, leader, neighLead, neighFollow, preb);

    bool changingAllowed = (state & LCA_BLOCKED) == 0;
    // change if the vehicle wants to and is allowed to change
    if ((state & LCA_WANTS_LANECHANGE) != 0 && changingAllowed
            // do not change to the opposite direction for cooperative reasons
            && (isOpposite || (state & LCA_COOPERATIVE) == 0)) {
        vehicle->getLaneChangeModel().startLaneChangeManeuver(source, opposite, direction);
        /// XXX use a dedicated transformation function
        vehicle->myState.myPos = source->getOppositePos(vehicle->myState.myPos);
        /// XXX compute a better lateral position
        opposite->forceVehicleInsertion(vehicle, vehicle->getPositionOnLane(), MSMoveReminder::NOTIFICATION_LANE_CHANGE, 0);
        if (!isOpposite) {
            vehicle->myState.myBackPos = source->getOppositePos(vehicle->myState.myBackPos);
        }
#ifdef DEBUG_CHANGE_OPPOSITE
        if (DEBUG_COND) {
            std::cout << SIMTIME << " changing to opposite veh=" << vehicle->getID() << " dir=" << direction << " opposite=" << Named::getIDSecure(opposite) << " state=" << state << "\n";
        }
#endif
        return true;
    }
#ifdef DEBUG_CHANGE_OPPOSITE
    if (DEBUG_COND) {
        std::cout << SIMTIME << " not changing to opposite veh=" << vehicle->getID() << " dir=" << direction
                  << " opposite=" << Named::getIDSecure(opposite) << " state=" << toString((LaneChangeAction)state) << "\n";
    }
#endif
    return false;
}


void
MSLaneChanger::computeOvertakingTime(const MSVehicle* vehicle, const MSVehicle* leader, SUMOReal gap, SUMOReal& timeToOvertake, SUMOReal& spaceToOvertake) {
    // Assumptions:
    // - leader maintains the current speed
    // - vehicle merges with maxSpeed ahead of leader
    // XXX affected by ticket #860 (the formula is invalid for the current position update rule)

    // first compute these values for the case where vehicle is accelerating
    // without upper bound on speed
    const SUMOReal vMax = vehicle->getLane()->getVehicleMaxSpeed(vehicle);
    const SUMOReal v = vehicle->getSpeed();
    const SUMOReal u = leader->getSpeed();
    const SUMOReal a = vehicle->getCarFollowModel().getMaxAccel();
    const SUMOReal d = vehicle->getCarFollowModel().getMaxDecel();
    const SUMOReal g = (
                           // drive up to the rear of leader
                           gap + vehicle->getVehicleType().getMinGap()
                           // drive head-to-head with the leader
                           + leader->getVehicleType().getLengthWithGap()
                           // drive past the leader
                           + vehicle->getVehicleType().getLength()
                           // allow for safe gap between leader and vehicle
                           + leader->getCarFollowModel().getSecureGap(v, vMax, d));
    const SUMOReal sign = -1; // XXX recheck
    // v*t + t*t*a*0.5 = g + u*t
    // solve t
    // t = ((u - v - (((((2.0*(u - v))**2.0) + (8.0*a*g))**(1.0/2.0))*sign/2.0))/a)
    SUMOReal t = (u - v - sqrt(4 * (u - v) * (u - v) + 8 * a * g) * sign * 0.5) / a;

    // allow for a safety time gap
    t += OPPOSITE_OVERTAKING_SAFE_TIMEGAP;
    // round to multiples of step length (TS)
    t = ceil(t / TS) * TS;

    /// XXX ignore speed limit when overtaking through the opposite lane?
    const SUMOReal timeToMaxSpeed = (vMax - v) / a;

    if (t <= timeToMaxSpeed) {
        timeToOvertake = t;
        spaceToOvertake = v * t + t * t * a * 0.5;
        //if (gDebugFlag1) std::cout << "    t below " << timeToMaxSpeed << " vMax=" << vMax << "\n";
    } else {
        // space until max speed is reached
        const SUMOReal s = v * timeToMaxSpeed + timeToMaxSpeed * timeToMaxSpeed * a * 0.5;
        const SUMOReal m = timeToMaxSpeed;
        // s + (t-m) * vMax = g + u*t
        // solve t
        t = (g - s + m * vMax) / (vMax - u);

        // allow for a safety time gap
        t += OPPOSITE_OVERTAKING_SAFE_TIMEGAP;
        // round to multiples of step length (TS)
        t = ceil(t / TS) * TS;

        timeToOvertake = t;
        spaceToOvertake = s + (t - m) * vMax;
        //if (gDebugFlag1) std::cout << "     s=" << s << " m=" << m << " vMax=" << vMax << "\n";
    }
}

/****************************************************************************/

