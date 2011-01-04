/****************************************************************************/
/// @file    MSLaneChanger.cpp
/// @author  Christian Roessel
/// @date    Fri, 01 Feb 2002
/// @version $Id$
///
// Performs lane changing of vehicles
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
#include "MSVehicle.h"
#include "MSVehicleType.h"
#include "MSVehicleTransfer.h"
#include "MSGlobals.h"
#include <cassert>
#include <iterator>
#include <cstdlib>
#include <cmath>
#include <microsim/MSAbstractLaneChangeModel.h>
#include <utils/common/MsgHandler.h>

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
// member method definitions
// ===========================================================================
MSLaneChanger::MSLaneChanger(std::vector<MSLane*>* lanes, bool allowSwap)
        : myAllowsSwap(allowSwap) {
    assert(lanes->size() > 1);

    // Fill the changer with the lane-data.
    myChanger.reserve(lanes->size());
    for (std::vector<MSLane*>::iterator lane = lanes->begin(); lane != lanes->end(); ++lane) {
        ChangeElem ce;
        ce.follow    = 0;
        ce.lead      = 0;
        ce.lane      = *lane;
        ce.veh       = (*lane)->myVehicles.rbegin();
        ce.hoppedVeh = 0;
        ce.lastBlocked = 0;
        myChanger.push_back(ce);
    }
}


MSLaneChanger::~MSLaneChanger() {}


void
MSLaneChanger::laneChange(SUMOTime t) {
    // This is what happens in one timestep. After initialization of the
    // changer, each vehicle will try to change. After that the changer
    // nedds an update to prevent multiple changes of one vehicle.
    // Finally, the change-result has to be given back to the lanes.
    initChanger();
    while (vehInChanger()) {

        bool haveChanged = change();
        updateChanger(haveChanged);
    }
    updateLanes(t);
}


void
MSLaneChanger::initChanger() {
    // Prepare myChanger with a safe state.
    for (ChangerIt ce = myChanger.begin(); ce != myChanger.end(); ++ce) {
        ce->lead = 0;
        ce->hoppedVeh = 0;
        ce->lastBlocked = 0;
        ce->dens = 0;

        MSLane::VehCont& vehicles = ce->lane->myVehicles;
        if (vehicles.empty()) {
            ce->veh  = vehicles.rend();
            ce->follow = 0;
            continue;
        }
        ce->veh  = vehicles.rbegin();
        if (vehicles.size() == 1) {
            ce->follow = 0;
            continue;
        }
        ce->follow = *(vehicles.rbegin() + 1);
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
    if (gSelected.isSelected(GLO_VEHICLE, static_cast<const GUIVehicle*>(vehicle)->getGlID())) {
        int bla = 0;
    }
#endif
    const std::vector<MSVehicle::LaneQ> &preb = vehicle->getBestLanes();
    assert(preb.size()==myChanger.size());
    for (int i=0; i<(int) myChanger.size(); ++i) {
        ((std::vector<MSVehicle::LaneQ>&) preb)[i].occupation = myChanger[i].dens + preb[i].nextOccupation;
    }

    vehicle->getLaneChangeModel().prepareStep();
    std::pair<MSVehicle * const, SUMOReal> leader = getRealThisLeader(myCandi);
    // check whether the vehicle wants and is able to change to right lane
    int state1 = 0;
    if (myCandi != myChanger.begin() && (myCandi - 1)->lane->allowsVehicleClass(veh(myCandi)->getVehicleType().getVehicleClass())) {
        std::pair<MSVehicle * const, SUMOReal> rLead = getRealLeader(myCandi - 1);
        std::pair<MSVehicle * const, SUMOReal> rFollow = getRealFollower(myCandi - 1);
        state1 = change2right(leader, rLead, rFollow, preb);
        if ((state1&LCA_URGENT)!=0||(state1&LCA_SPEEDGAIN)!=0) {
            state1 |= LCA_RIGHT;
        }
        bool changingAllowed1 = (state1&LCA_BLOCKED)==0;
        // change if the vehicle wants to and is allowed to change
        if ((state1&LCA_RIGHT)!=0&&changingAllowed1) {
#ifndef NO_TRACI
            // inform lane change model about this change
            vehicle->getLaneChangeModel().fulfillChangeRequest(REQUEST_RIGHT);
#endif
            (myCandi - 1)->hoppedVeh = vehicle;
            (myCandi - 1)->lane->myTmpVehicles.push_front(vehicle);
            vehicle->leaveLane(MSMoveReminder::NOTIFICATION_LANE_CHANGE);
            myCandi->lane->leftByLaneChange(vehicle);
            vehicle->enterLaneAtLaneChange((myCandi - 1)->lane);
            (myCandi - 1)->lane->enteredByLaneChange(vehicle);
            vehicle->myLastLaneChangeOffset = 0;
            vehicle->getLaneChangeModel().changed();
            (myCandi - 1)->dens += (myCandi - 1)->hoppedVeh->getVehicleType().getLength();
            return true;
        }
        if ((state1&LCA_RIGHT)!=0&&(state1&LCA_URGENT)!=0) {
            (myCandi - 1)->lastBlocked = vehicle;
        }
    }



    // check whether the vehicle wants and is able to change to left lane
    int state2 = 0;
    if ((myCandi + 1) != myChanger.end() && (myCandi + 1)->lane->allowsVehicleClass(veh(myCandi)->getVehicleType().getVehicleClass())) {
        std::pair<MSVehicle * const, SUMOReal> lLead = getRealLeader(myCandi+1);
        std::pair<MSVehicle * const, SUMOReal> lFollow = getRealFollower(myCandi+1);
        state2 = change2left(leader, lLead, lFollow,preb);
        if ((state2&LCA_URGENT)!=0||(state2&LCA_SPEEDGAIN)!=0) {
            state2 |= LCA_LEFT;
        }
        bool changingAllowed2 = (state2&LCA_BLOCKED)==0;
        //vehicle->getLaneChangeModel().setOwnState(state2|state1);
        // change if the vehicle wants to and is allowed to change
        if ((state2&LCA_LEFT)!=0&&changingAllowed2) {
#ifndef NO_TRACI
            // inform lane change model about this change
            vehicle->getLaneChangeModel().fulfillChangeRequest(REQUEST_LEFT);
#endif
            (myCandi + 1)->hoppedVeh = veh(myCandi);
            (myCandi + 1)->lane->myTmpVehicles.push_front(veh(myCandi));
            vehicle->leaveLane(MSMoveReminder::NOTIFICATION_LANE_CHANGE);
            myCandi->lane->leftByLaneChange(vehicle);
            vehicle->enterLaneAtLaneChange((myCandi + 1)->lane);
            (myCandi + 1)->lane->enteredByLaneChange(vehicle);
            vehicle->myLastLaneChangeOffset = 0;
            vehicle->getLaneChangeModel().changed();
            (myCandi + 1)->dens += (myCandi + 1)->hoppedVeh->getVehicleType().getLength();
            return true;
        }
        if ((state2&LCA_LEFT)!=0&&(state2&LCA_URGENT)!=0) {
            (myCandi + 1)->lastBlocked = vehicle;
        }
    }
    vehicle->getLaneChangeModel().setOwnState(state2|state1);

    if ((state1&(LCA_URGENT))!=0&&(state2&(LCA_URGENT))!=0) {
        // ... wants to go to the left AND to the right
        // just let them go to the right lane...
        state2 = 0;
        vehicle->getLaneChangeModel().setOwnState(state1);
    }
    // check whether the vehicles should be swapped
    if (myAllowsSwap&&((state1&(LCA_URGENT))!=0||(state2&(LCA_URGENT))!=0)) {
        // get the direction ...
        ChangerIt target;
        int dir;
        if ((state1&(LCA_URGENT))!=0) {
            // ... wants to go right
            target = myCandi - 1;
            dir = -1;
        }
        if ((state2&(LCA_URGENT))!=0) {
            // ... wants to go left
            target = myCandi + 1;
            dir = 1;
        }
        MSVehicle *prohibitor = target->lead;
        if (target->hoppedVeh!=0) {
            SUMOReal hoppedPos = target->hoppedVeh->getPositionOnLane();
            if (prohibitor==0||(hoppedPos>vehicle->getPositionOnLane() && prohibitor->getPositionOnLane()>hoppedPos)) {
                prohibitor = 0;// !!! vehicles should not jump over more than one lanetarget->hoppedVeh;
            }
        }
        if (prohibitor!=0
                &&
                ((prohibitor->getLaneChangeModel().getOwnState()&(LCA_URGENT/*|LCA_SPEEDGAIN*/))!=0
                 &&
                 (prohibitor->getLaneChangeModel().getOwnState()&(LCA_LEFT|LCA_RIGHT))
                 !=
                 (vehicle->getLaneChangeModel().getOwnState()&(LCA_LEFT|LCA_RIGHT))
                )
           ) {

            // check for position and speed
            if (prohibitor->getVehicleType().getLength()-vehicle->getVehicleType().getLength()==0) {
                // ok, may be swapped
                // remove vehicle to swap with
                MSLane::VehCont::iterator i = find(target->lane->myTmpVehicles.begin(), target->lane->myTmpVehicles.end(), prohibitor);
                if (i!=target->lane->myTmpVehicles.end()) {
                    MSVehicle *bla = *i;
                    assert(bla==prohibitor);
                    target->lane->myTmpVehicles.erase(i);
                    // set this vehicle
                    target->hoppedVeh = vehicle;
                    target->lane->myTmpVehicles.push_front(vehicle);
                    myCandi->hoppedVeh = prohibitor;
                    myCandi->lane->myTmpVehicles.push_front(prohibitor);

                    // leave lane and detectors
                    vehicle->leaveLane(MSMoveReminder::NOTIFICATION_LANE_CHANGE);
                    prohibitor->leaveLane(MSMoveReminder::NOTIFICATION_LANE_CHANGE);
                    // patch position and speed
                    SUMOReal p1 = vehicle->getPositionOnLane();
                    vehicle->myState.myPos = prohibitor->myState.myPos;
                    prohibitor->myState.myPos = p1;
                    p1 = vehicle->getSpeed();
                    vehicle->myState.mySpeed = prohibitor->myState.mySpeed;
                    prohibitor->myState.mySpeed = p1;
                    // enter lane and detectors
                    vehicle->enterLaneAtLaneChange(target->lane);
                    prohibitor->enterLaneAtLaneChange(myCandi->lane);
                    // mark lane change
                    vehicle->getLaneChangeModel().changed();
                    vehicle->myLastLaneChangeOffset = 0;
                    prohibitor->getLaneChangeModel().changed();
                    prohibitor->myLastLaneChangeOffset = 0;
                    (myCandi)->dens += prohibitor->getVehicleType().getLength();
                    (target)->dens += vehicle->getVehicleType().getLength();
                    return true;
                }
            }
        }
    }
    // Candidate didn't change lane.
    myCandi->lane->myTmpVehicles.push_front(veh(myCandi));
    vehicle->myLastLaneChangeOffset += DELTA_T;
    (myCandi)->dens += vehicle->getVehicleType().getLength();
    return false;
}


std::pair<MSVehicle * const, SUMOReal>
MSLaneChanger::getRealThisLeader(const ChangerIt &target) const throw() {
    // get the leading vehicle on the lane to change to
    MSVehicle* leader = target->lead;
    if (leader==0) {
        MSLane* targetLane = target->lane;
        MSVehicle *predP = targetLane->getPartialOccupator();
        if (predP!=0) {
            return std::pair<MSVehicle *, SUMOReal>(predP, targetLane->getPartialOccupatorEnd() - veh(myCandi)->getPositionOnLane());
        }
        const std::vector<MSLane*> &bestLaneConts = veh(myCandi)->getBestLanesContinuation();
        MSLinkCont::const_iterator link = targetLane->succLinkSec(*veh(myCandi), 1, *targetLane, bestLaneConts);
        if (targetLane->isLinkEnd(link)) {
            return std::pair<MSVehicle *, SUMOReal>(static_cast<MSVehicle *>(0), -1);
        }
        MSLane *nextLane = (*link)->getLane();
        if (nextLane==0) {
            return std::pair<MSVehicle *, SUMOReal>(static_cast<MSVehicle *>(0), -1);
        }
        leader = nextLane->getLastVehicle();
        if (leader==0) {
            return std::pair<MSVehicle *, SUMOReal>(static_cast<MSVehicle *>(0), -1);
        }
        SUMOReal gap =
            leader->getPositionOnLane()-leader->getVehicleType().getLength()
            +
            (myCandi->lane->getLength()-veh(myCandi)->getPositionOnLane());
        return std::pair<MSVehicle * const, SUMOReal>(leader, MAX2((SUMOReal) 0, gap));
    } else {
        MSVehicle *candi = veh(myCandi);
        SUMOReal gap = leader->getPositionOnLane()-leader->getVehicleType().getLength()-candi->getPositionOnLane();
        return std::pair<MSVehicle * const, SUMOReal>(leader, MAX2((SUMOReal) 0, gap));
    }
}


std::pair<MSVehicle * const, SUMOReal>
MSLaneChanger::getRealLeader(const ChangerIt &target) const throw() {
    // get the leading vehicle on the lane to change to
    MSVehicle* neighLead = target->lead;
    // check whether the hopped vehicle got the leader
    if (target->hoppedVeh!=0) {
        SUMOReal hoppedPos = target->hoppedVeh->getPositionOnLane();
        if (hoppedPos>veh(myCandi)->getPositionOnLane() && (neighLead==0 || neighLead->getPositionOnLane()>hoppedPos)) {
            neighLead = target->hoppedVeh;
        }
    }
    if (neighLead==0) {
        MSLane* targetLane = target->lane;
        MSVehicle *predP = targetLane->getPartialOccupator();
        if (predP!=0) {
            return std::pair<MSVehicle *, SUMOReal>(predP, targetLane->getPartialOccupatorEnd() - veh(myCandi)->getPositionOnLane());
        }
        const std::vector<MSLane*> &bestLaneConts = veh(myCandi)->getBestLanesContinuation(myCandi->lane);
        SUMOReal seen = myCandi->lane->getLength() - veh(myCandi)->getPositionOnLane();
        SUMOReal speed = veh(myCandi)->getSpeed();
        SUMOReal dist = veh(myCandi)->getCarFollowModel().brakeGap(speed);
        if (seen>dist) {
            return std::pair<MSVehicle * const, SUMOReal>(static_cast<MSVehicle *>(0), -1);
        }
        return target->lane->getLeaderOnConsecutive(dist, seen, speed, *veh(myCandi), bestLaneConts);
    } else {
        MSVehicle *candi = veh(myCandi);
        return std::pair<MSVehicle * const, SUMOReal>(neighLead, neighLead->getPositionOnLane()-neighLead->getVehicleType().getLength()-candi->getPositionOnLane());
    }
}


std::pair<MSVehicle * const, SUMOReal>
MSLaneChanger::getRealFollower(const ChangerIt &target) const throw() {
    MSVehicle* neighFollow = veh(target);
    // check whether the hopped vehicle got the follower
    if (target->hoppedVeh!=0) {
        SUMOReal hoppedPos = target->hoppedVeh->getPositionOnLane();
        if (hoppedPos<=veh(myCandi)->getPositionOnLane() && (neighFollow==0 || neighFollow->getPositionOnLane()>hoppedPos)) {
            neighFollow = target->hoppedVeh;
        }
    }
    if (neighFollow==0) {
        SUMOReal speed = target->lane->getMaxSpeed();
        // in order to look back, we'd need the minimum braking ability of vehicles in the net...
        // we'll assume it to be 4m/s^2
        // !!!revisit
        SUMOReal dist = speed * speed / (2.*4.) + SPEED2DIST(speed);
        dist = MIN2(dist, (SUMOReal) 500.);
        MSVehicle *candi = veh(myCandi);
        SUMOReal seen = candi->getPositionOnLane()-candi->getVehicleType().getLength();
        return target->lane->getFollowerOnConsecutive(dist, seen, candi->getSpeed(), candi->getPositionOnLane() - candi->getVehicleType().getLength(), 4.5);
    } else {
        MSVehicle *candi = veh(myCandi);
        return std::pair<MSVehicle * const, SUMOReal>(neighFollow, candi->getPositionOnLane()-candi->getVehicleType().getLength()-neighFollow->getPositionOnLane());
    }
}




void
MSLaneChanger::updateChanger(bool vehHasChanged) {
    assert(myCandi->veh != myCandi->lane->myVehicles.rend());

    // "Push" the vehicles to the back, i.e. follower becomes vehicle,
    // vehicle becomes leader, and leader becomes predecessor of vehicle,
    // if it exists.
    if (!vehHasChanged) {
        myCandi->lead = veh(myCandi);
    }
    myCandi->veh    = myCandi->veh + 1;

    if (veh(myCandi) == 0) {
        assert(myCandi->follow == 0);
        // leader already 0.
        return;
    }
    if (myCandi->veh + 1 == myCandi->lane->myVehicles.rend()) {
        myCandi->follow = 0;
    } else {
        myCandi->follow = *(myCandi->veh + 1) ;
    }
    return;
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

        ce->lane->swapAfterLaneChange(t);
    }
}


MSLaneChanger::ChangerIt
MSLaneChanger::findCandidate() {
    // Find the vehicle in myChanger with the smallest position. If there
    // is no vehicle in myChanger (shouldn't happen) , return
    // myChanger.end().
    ChangerIt max = myChanger.end();
    for (ChangerIt ce = myChanger.begin(); ce != myChanger.end(); ++ce) {
        if (veh(ce) == 0) {
            continue;
        }
        if (max == myChanger.end()) {
            max = ce;
            continue;
        }
        assert(veh(ce)  != 0);
        assert(veh(max) != 0);
        if (veh(max)->getPositionOnLane() < veh(ce)->getPositionOnLane()) {
            max = ce;
        }
    }
    assert(max != myChanger.end());
    assert(veh(max) != 0);
    return max;
}


int
MSLaneChanger::change2right(const std::pair<MSVehicle * const, SUMOReal> &leader,
                            const std::pair<MSVehicle * const, SUMOReal> &rLead,
                            const std::pair<MSVehicle * const, SUMOReal> &rFollow,
                            const std::vector<MSVehicle::LaneQ> &preb) const throw() {
    ChangerIt target = myCandi - 1;
    int blocked = overlapWithHopped(target)
                  ? target->hoppedVeh->getPositionOnLane()<veh(myCandi)->getPositionOnLane()
                  ? LCA_BLOCKED_BY_RIGHT_FOLLOWER
                  : LCA_BLOCKED_BY_RIGHT_LEADER
                  : 0;
    // overlap
    if (rFollow.first!=0&&rFollow.second<0) {
        blocked |= (LCA_BLOCKED_BY_RIGHT_FOLLOWER);
    }
    if (rLead.first!=0&&rLead.second<0) {
        blocked |= (LCA_BLOCKED_BY_RIGHT_LEADER);
    }
    // safe back gap
    if (rFollow.first!=0) {
        MSLane* targetLane = target->lane;
        // !!! eigentlich: vsafe braucht die Max. Geschwindigkeit beider Spuren
        if (rFollow.second<rFollow.first->getCarFollowModel().getSecureGap(rFollow.first->getSpeed(), veh(myCandi)->getSpeed(), veh(myCandi)->getCarFollowModel().getMaxDecel())) {
            blocked |= LCA_BLOCKED_BY_RIGHT_FOLLOWER;
        }
    }

    // safe front gap
    if (rLead.first!=0) {
        MSLane* targetLane = target->lane;
        // !!! eigentlich: vsafe braucht die Max. Geschwindigkeit beider Spuren
        if (rLead.second<veh(myCandi)->getCarFollowModel().getSecureGap(veh(myCandi)->getSpeed(), rLead.first->getSpeed(), rLead.first->getCarFollowModel().getMaxDecel())) {
            blocked |= LCA_BLOCKED_BY_RIGHT_LEADER;
        }
    }

    MSAbstractLaneChangeModel::MSLCMessager msg(leader.first, rLead.first, rFollow.first);
    return blocked | veh(myCandi)->getLaneChangeModel().wantsChangeToRight(
               msg, blocked, leader, rLead, rFollow, *(myCandi-1)->lane, preb, &(myCandi->lastBlocked));
}


int
MSLaneChanger::change2left(const std::pair<MSVehicle * const, SUMOReal> &leader,
                           const std::pair<MSVehicle * const, SUMOReal> &rLead,
                           const std::pair<MSVehicle * const, SUMOReal> &rFollow,
                           const std::vector<MSVehicle::LaneQ> &preb) const throw() {
    ChangerIt target = myCandi + 1;
    int blocked = overlapWithHopped(target)
                  ? target->hoppedVeh->getPositionOnLane()<veh(myCandi)->getPositionOnLane()
                  ? LCA_BLOCKED_BY_LEFT_FOLLOWER
                  : LCA_BLOCKED_BY_LEFT_LEADER
                  : 0;
    // overlap
    if (rFollow.first!=0&&rFollow.second<0) {
        blocked |= (LCA_BLOCKED_BY_LEFT_FOLLOWER);
    }
    if (rLead.first!=0&&rLead.second<0) {
        blocked |= (LCA_BLOCKED_BY_LEFT_LEADER);
    }
    // safe back gap
    if (rFollow.first!=0) {
        MSLane* targetLane = target->lane;
        // !!! eigentlich: vsafe braucht die Max. Geschwindigkeit beider Spuren
        if (rFollow.second<rFollow.first->getCarFollowModel().getSecureGap(rFollow.first->getSpeed(), veh(myCandi)->getSpeed(), veh(myCandi)->getCarFollowModel().getMaxDecel())) {
            blocked |= LCA_BLOCKED_BY_LEFT_FOLLOWER;
        }
    }
    // safe front gap
    if (rLead.first!=0) {
        MSLane* targetLane = target->lane;
        // !!! eigentlich: vsafe braucht die Max. Geschwindigkeit beider Spuren
        if (rLead.second<veh(myCandi)->getCarFollowModel().getSecureGap(veh(myCandi)->getSpeed(), rLead.first->getSpeed(), rLead.first->getCarFollowModel().getMaxDecel())) {
            blocked |= LCA_BLOCKED_BY_LEFT_LEADER;
        }
    }
    MSAbstractLaneChangeModel::MSLCMessager msg(leader.first, rLead.first, rFollow.first);
    return blocked | veh(myCandi)->getLaneChangeModel().wantsChangeToLeft(
               msg, blocked, leader, rLead, rFollow, *(myCandi+1)->lane, preb, &(myCandi->lastBlocked));
}




/****************************************************************************/

