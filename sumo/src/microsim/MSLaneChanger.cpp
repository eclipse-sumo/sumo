/****************************************************************************/
/// @file    MSLaneChanger.cpp
/// @author  Christian Roessel
/// @date    Fri, 01 Feb 2002
/// @version $Id$
///
// the edge's lanes.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif

// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
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

#ifdef ABS_DEBUG
#include "MSDebugHelper.h"
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// some definitions (debugging only)
// ===========================================================================
#define DEBUG_OUT cout


// ===========================================================================
// member method definitions
// ===========================================================================
MSLaneChanger::~MSLaneChanger()
{}

//-------------------------------------------------------------------------//

MSLaneChanger::MSLaneChanger(MSEdge::LaneCont* lanes)
{
    assert(lanes->size() > 1);

    // Fill the changer with the lane-data.
    myChanger.reserve(lanes->size());
    for (MSEdge::LaneCont::iterator lane = lanes->begin(); lane != lanes->end(); ++lane) {
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

//-------------------------------------------------------------------------//

void
MSLaneChanger::laneChange()
{
    // This is what happens in one timestep. After initialization of the
    // changer, each vehicle will try to change. After that the changer
    // nedds an update to prevent multiple changes of one vehicle.
    // Finally, the change-result has to be given back to the lanes.
    initChanger();
    while (vehInChanger()) {

        bool haveChanged = change();
        updateChanger(haveChanged);
    }
    updateLanes();
}

//-------------------------------------------------------------------------//

void
MSLaneChanger::initChanger()
{
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

//-------------------------------------------------------------------------//
#ifdef GUI_DEBUG
#include <utils/gui/div/GUIGlobalSelection.h>
#include <guisim/GUIVehicle.h>
#endif


bool
MSLaneChanger::change()
{
    // Find change-candidate. If it is on an allowed lane, try to change
    // to the right (there is a rule in Germany that you have to change
    // to the right, unless you are overtaking). If change to the right
    // isn't possible, check if there is a possibility to overtake (on the
    // left.
    // If candidate isn't on an allowed lane, changing to an allowed has
    // priority.
    myCandi = findCandidate();
    MSVehicle* vehicle = veh(myCandi);
#ifdef GUI_DEBUG
    if (gSelected.isSelected(GLO_VEHICLE, static_cast<GUIVehicle*>(vehicle)->getGlID())) {
        int blb = 0;
    }
#endif
#ifdef ABS_DEBUG
    if (debug_globaltime>=debug_searchedtime && (vehicle->getID()==debug_searched1||vehicle->getID()==debug_searched2)) {
        DEBUG_OUT << "change:" << vehicle->getID() << ": " << vehicle->getPositionOnLane() << ", " << vehicle->getSpeed() << endl;
    }
#endif
    const std::vector<std::vector<MSVehicle::LaneQ> > &preb = vehicle->getBestLanes();
    assert(preb[0].size()==myChanger.size());
    for (int i=0; i<(int) myChanger.size(); i++) {
        ((std::vector<std::vector<MSVehicle::LaneQ> >&) preb)[0][i].hindernisPos = myChanger[i].dens + preb[0][i].v;
    }

    vehicle->getLaneChangeModel().prepareStep();
    // check whether the vehicle wants and is able to change to right lane
    std::pair<MSVehicle*, SUMOReal> rLead = getRealRightLeader();
    std::pair<MSVehicle*, SUMOReal> lLead = getRealLeftLeader();
    std::pair<MSVehicle*, SUMOReal> rFollow = getRealRightFollower();
    std::pair<MSVehicle*, SUMOReal> lFollow = getRealLeftFollower();
    std::pair<MSVehicle*, SUMOReal> leader = getRealThisLeader(myCandi);
    int state1 =
        change2right(leader, rLead, rFollow, preb);
//            changePreference.first, changePreference.second, currentLaneDist);
    bool changingAllowed =
        (state1&(LCA_BLOCKEDBY_LEADER|LCA_BLOCKEDBY_FOLLOWER))==0
        /*||
        (state1&LCA_OVERLAPPING)==0*//*&&currentLaneDist<100.0&&changePreference.second>100.0*/;
    if ((state1&LCA_URGENT)!=0||(state1&LCA_SPEEDGAIN)!=0) {
        state1 |= LCA_RIGHT;
    }
    // change if the vehicle wants to and is allowed to change
    if ((state1&LCA_RIGHT)!=0&&changingAllowed) {
        (myCandi - 1)->hoppedVeh = veh(myCandi);
        (myCandi - 1)->lane->myTmpVehicles.push_front(veh(myCandi));
        vehicle->leaveLaneAtLaneChange();
        myCandi->lane->myUseDefinition->vehLenSum -= veh(myCandi)->getLength();
        vehicle->enterLaneAtLaneChange((myCandi - 1)->lane);
        (myCandi - 1)->lane->myUseDefinition->vehLenSum += veh(myCandi)->getLength();
        vehicle->myLastLaneChangeOffset = 0;
#ifdef ABS_DEBUG
        if (debug_globaltime>debug_searchedtime && (vehicle->getID()==debug_searched1||vehicle->getID()==debug_searched2)) {
            DEBUG_OUT << "changed2right" << endl;
        }
#endif
        vehicle->getLaneChangeModel().changed();
        (myCandi - 1)->dens += (myCandi - 1)->hoppedVeh->getLength();
        return true;
    }
    if ((state1&LCA_RIGHT)!=0&&(state1&LCA_URGENT)!=0) {
        /*!!!
        if( myCandi->veh==myCandi->lane->myVehicles.rbegin()
            &&
            !vehicle->onAllowed()
            &&
            vehicle->getSpeed()<MSGlobals::gMaxVehV4FalseLaneTeleport
            &&
            myCandi->lane->maxSpeed()>MSGlobals::gMinLaneVMax4FalseLaneTeleport
            &&
            currentLaneDist<MSGlobals::gMinVehDist4FalseLaneTeleport ) {

            WRITE_WARNING("Teleporting '" + veh(myCandi)->getID()+ "' due to failed lane changing.");
            MSVehicleTransfer::getInstance()->addVeh(veh(myCandi));
            return true;
        }
        */
        (myCandi - 1)->lastBlocked = vehicle;
    }

    // check whether the vehicle wants and is able to change to left lane
    int state2 =
        change2left(leader, lLead, lFollow,preb);
//            changePreference.first, changePreference.second, currentLaneDist);
    if ((state2&LCA_URGENT)!=0||(state2&LCA_SPEEDGAIN)!=0) {
        state2 |= LCA_LEFT;
    }
    changingAllowed =
        (state2&(LCA_BLOCKEDBY_LEADER|LCA_BLOCKEDBY_FOLLOWER))==0
        /*||
        (state2&LCA_OVERLAPPING)==0*//*&&currentLaneDist<100.0&&changePreference.second>100.0*/;
    vehicle->getLaneChangeModel().setState(state2|state1);
    // change if the vehicle wants to and is allowed to change
    if ((state2&LCA_LEFT)!=0&&changingAllowed) {
        (myCandi + 1)->hoppedVeh = veh(myCandi);
        (myCandi + 1)->lane->myTmpVehicles.push_front(veh(myCandi));
        vehicle->leaveLaneAtLaneChange();
        myCandi->lane->myUseDefinition->vehLenSum -= veh(myCandi)->getLength();
        vehicle->enterLaneAtLaneChange((myCandi + 1)->lane);
        (myCandi + 1)->lane->myUseDefinition->vehLenSum += veh(myCandi)->getLength();
        vehicle->myLastLaneChangeOffset = 0;
#ifdef ABS_DEBUG
        if (debug_globaltime>debug_searchedtime-5 && (vehicle->getID()==debug_searched1||vehicle->getID()==debug_searched2)) {
            DEBUG_OUT << "changed2left" << endl;
        }
#endif
        vehicle->getLaneChangeModel().changed();
        (myCandi + 1)->dens += (myCandi + 1)->hoppedVeh->getLength();
        return true;
    }
    if ((state2&LCA_LEFT)!=0&&(state2&LCA_URGENT)!=0) {
        /*
        if( myCandi->veh==myCandi->lane->myVehicles.rbegin()
            &&
            !vehicle->onAllowed()
            &&
            vehicle->getSpeed()<MSGlobals::gMaxVehV4FalseLaneTeleport
            &&
            myCandi->lane->maxSpeed()>MSGlobals::gMinLaneVMax4FalseLaneTeleport
            &&
            currentLaneDist<MSGlobals::gMinVehDist4FalseLaneTeleport ) {

            WRITE_WARNING("Teleporting '" + veh(myCandi)->getID()+ "' due to failed lane changing.");
            MSVehicleTransfer::getInstance()->addVeh(veh(myCandi));
            return true;
        }
        */
        (myCandi + 1)->lastBlocked = vehicle;
    }

    if ((state1&(LCA_URGENT))!=0&&(state2&(LCA_URGENT))!=0) {
        // ... wants to go to the left AND to the right
        // just let them go to the right lane...
        state2 = 0;
        vehicle->getLaneChangeModel().setState(state1);
    }
    // check whether the vehicles should be swapped
    if ((state1&(LCA_URGENT))!=0||(state2&(LCA_URGENT))!=0) {
        // get the direction ...
        ChangerIt target;
        if ((state1&(LCA_URGENT))!=0) {
            // ... wants to go right
            target = myCandi - 1;
        }
        if ((state2&(LCA_URGENT))!=0) {
            // ... wants to go left
            target = myCandi + 1;
        }
        MSVehicle *prohibitor = target->lead;//veh(target);//->follow;
        if (target->hoppedVeh!=0) {
            SUMOReal hoppedPos = target->hoppedVeh->getPositionOnLane();
            if (prohibitor==0||(
                        hoppedPos>vehicle->getPositionOnLane() && prohibitor->getPositionOnLane()>hoppedPos)) {

                prohibitor = 0;// !!! vehicles should not jump over more than one lanetarget->hoppedVeh;
            }
        }
        if (prohibitor!=0
                &&
                ((prohibitor->getLaneChangeModel().getState()&(LCA_URGENT/*|LCA_SPEEDGAIN*/))!=0
                 &&
                 (prohibitor->getLaneChangeModel().getState()&(LCA_LEFT|LCA_RIGHT))
                 !=
                 (vehicle->getLaneChangeModel().getState()&(LCA_LEFT|LCA_RIGHT))
                )
           ) {

            // check for position and speed
            if (/*prohibitor->speed()<0.1&&vehicle->speed()<0.1
                                                                &&
                                                                fabs(prohibitor->pos()-vehicle->pos())<0.1
                                                                &&*/
                prohibitor->getLength()-vehicle->getLength()==0
            ) {

                // ok, may be swapped
                // remove vehicle to swap with
                MSLane::VehCont::iterator i =
                    find(
                        target->lane->myTmpVehicles.begin(),
                        target->lane->myTmpVehicles.end(),
                        prohibitor);
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
                    vehicle->leaveLaneAtLaneChange();
                    prohibitor->leaveLaneAtLaneChange();
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
#ifdef ABS_DEBUG
                    if (debug_globaltime>debug_searchedtime-5 && (vehicle->getID()==debug_searched1||vehicle->getID()==debug_searched2)) {
                        DEBUG_OUT << "swapped:"
                        << vehicle->getID() << ": at" << vehicle->getLane().getID() << ", " << vehicle->getPositionOnLane() << ", " << vehicle->getSpeed()
                        << " with:"
                        << prohibitor->getID() << ": at" << prohibitor->getLane().getID() << ", " << prohibitor->getPositionOnLane() << ", " << vehicle->getSpeed()
                        << endl;
                    }
#endif
                    (myCandi)->dens += prohibitor->getLength();
                    (target)->dens += vehicle->getLength();
                    return true;
                }
            }
        }
    }
    // Candidate didn't change lane.
    myCandi->lane->myTmpVehicles.push_front(veh(myCandi));
    vehicle->myLastLaneChangeOffset++;
#ifdef ABS_DEBUG
    if (debug_globaltime>debug_searchedtime-5 && (vehicle->getID()==debug_searched1||vehicle->getID()==debug_searched2)) {
        DEBUG_OUT << "kept" << endl;
    }
#endif
    (myCandi)->dens += vehicle->getLength();
    return false;
}

//-------------------------------------------------------------------------//
std::pair<MSVehicle *, SUMOReal>
MSLaneChanger::getRealThisLeader(const ChangerIt &target)
{
    // get the leading vehicle on the lane to change to
    MSVehicle* leader = target->lead;
    if (leader==0) {
        MSLane* targetLane = target->lane;
        MSLinkCont::const_iterator link =
            targetLane->succLinkSec(*veh(myCandi), 1, *targetLane);
        if (targetLane->isLinkEnd(link)) {
            return std::pair<MSVehicle *, SUMOReal>(0, -1);
        }
        MSLane *nextLane = (*link)->getLane();
        if (nextLane==0) {
            return std::pair<MSVehicle *, SUMOReal>(0, -1);
        }
        leader = nextLane->getLastVehicle(*this);
        if (leader==0) {
            return std::pair<MSVehicle *, SUMOReal>(0, -1);
        }
        SUMOReal gap =
            leader->getPositionOnLane()-leader->getLength()
            +
            (myCandi->lane->length()-veh(myCandi)->getPositionOnLane());
        return std::pair<MSVehicle *, SUMOReal>(leader, MAX2((SUMOReal) 0, gap));
    } else {
        MSVehicle *candi = veh(myCandi);
        SUMOReal gap = leader->getPositionOnLane()-leader->getLength()-candi->getPositionOnLane();
        return std::pair<MSVehicle *, SUMOReal>(leader, MAX2((SUMOReal) 0, gap));
    }
}


std::pair<MSVehicle *, SUMOReal>
MSLaneChanger::getRealLeader(const ChangerIt &target)
{
    // get the leading vehicle on the lane to change to
    MSVehicle* neighLead = target->lead;
    // check whether the hopped vehicle got the leader
    if (target->hoppedVeh!=0) {
        SUMOReal hoppedPos = target->hoppedVeh->getPositionOnLane();
        if (hoppedPos>veh(myCandi)->getPositionOnLane() &&
                (neighLead==0 || neighLead->getPositionOnLane()>hoppedPos)) {

            neighLead = target->hoppedVeh;
        }
    }

    if (neighLead==0) {
        MSLane* targetLane = target->lane;
        MSLinkCont::const_iterator link =
            targetLane->succLinkSec(*veh(myCandi), 1, *targetLane);
        if (targetLane->isLinkEnd(link)) {
            return std::pair<MSVehicle *, SUMOReal>(0, -1);
        }
        MSLane *nextLane = (*link)->getLane();
        if (nextLane==0) {
            return std::pair<MSVehicle *, SUMOReal>(0, -1);
        }
        neighLead = nextLane->getLastVehicle(*this);
        if (neighLead==0) {
            return std::pair<MSVehicle *, SUMOReal>(0, -1);
        }
        return std::pair<MSVehicle *, SUMOReal>(neighLead,
                                                neighLead->getPositionOnLane()-neighLead->getLength()
                                                +
                                                (myCandi->lane->length()-veh(myCandi)->getPositionOnLane()));
    } else {
        MSVehicle *candi = veh(myCandi);
        return std::pair<MSVehicle *, SUMOReal>(neighLead,
                                                neighLead->getPositionOnLane()-neighLead->getLength()-candi->getPositionOnLane());
    }
}


std::pair<MSVehicle *, SUMOReal>
MSLaneChanger::getRealRightLeader()
{
    // there is no right lane
    if (myCandi == myChanger.begin()) {
        return std::pair<MSVehicle *, SUMOReal>(0, -1);
    }
    ChangerIt target = myCandi - 1;
    return getRealLeader(target);
}


std::pair<MSVehicle *, SUMOReal>
MSLaneChanger::getRealLeftLeader()
{
    // there is no left lane
    if ((myCandi+1) == myChanger.end()) {
        return std::pair<MSVehicle *, SUMOReal>(0, -1);
    }
    ChangerIt target = myCandi + 1;
    return getRealLeader(target);
}


std::pair<MSVehicle *, SUMOReal>
MSLaneChanger::getRealFollower(const ChangerIt &target)
{
    MSVehicle* neighFollow = veh(target);//->follow;
    // check whether the hopped vehicle got the follower
    if (target->hoppedVeh!=0) {
        SUMOReal hoppedPos = target->hoppedVeh->getPositionOnLane();
        if (hoppedPos<=veh(myCandi)->getPositionOnLane() &&
                (neighFollow==0 || neighFollow->getPositionOnLane()>hoppedPos)) {

            neighFollow = target->hoppedVeh;
        }
    }
    if (neighFollow==0) {
        neighFollow = target->lane->myApproaching;
        if (neighFollow==0) {
            return std::pair<MSVehicle *, SUMOReal>(0, -1);
        }
        MSVehicle *candi = veh(myCandi);
        SUMOReal gap = candi->getPositionOnLane()-candi->getLength()
                       + target->lane->myBackDistance;
        return std::pair<MSVehicle *, SUMOReal>(neighFollow, gap);
        //(neighFollow->getLane().length()-neighFollow->pos()));
    } else {
        MSVehicle *candi = veh(myCandi);
        return std::pair<MSVehicle *, SUMOReal>(neighFollow,
                                                candi->getPositionOnLane()-candi->getLength()-neighFollow->getPositionOnLane());
    }
}


std::pair<MSVehicle *, SUMOReal>
MSLaneChanger::getRealRightFollower()
{
    // there is no right lane
    if (myCandi == myChanger.begin()) {
        return std::pair<MSVehicle *, SUMOReal>(0, -1);
    }
    ChangerIt target = myCandi - 1;
    return getRealFollower(target);
}


std::pair<MSVehicle *, SUMOReal>
MSLaneChanger::getRealLeftFollower()
{
    // there is no left lane
    if ((myCandi+1) == myChanger.end()) {
        return std::pair<MSVehicle *, SUMOReal>(0, -1);
    }
    ChangerIt target = myCandi + 1;
    return getRealFollower(target);
}



void
MSLaneChanger::updateChanger(bool vehHasChanged)
{
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

//-------------------------------------------------------------------------//

void
MSLaneChanger::updateLanes()
{

    // Update the lane's vehicle-container.
    // First: it is bad style to change other classes members, but for
    // this release, other attempts were too time-consuming. In a next
    // release we will change from this lane-centered design to a vehicle-
    // centered. This will solve many problems.
    // Second: this swap would be faster if vehicle-containers would have
    // been pointers, but then I had to change too much of the MSLane code.
    for (ChangerIt ce = myChanger.begin(); ce != myChanger.end(); ++ce) {

        ce->lane->swapAfterLaneChange();
    }
}

//-------------------------------------------------------------------------//

MSLaneChanger::ChangerIt
MSLaneChanger::findCandidate()
{
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

//-------------------------------------------------------------------------//

int
MSLaneChanger::change2right(const std::pair<MSVehicle*, SUMOReal> &leader,
                            const std::pair<MSVehicle*, SUMOReal> &rLead,
                            const std::pair<MSVehicle*, SUMOReal> &rFollow,
                            const std::vector<std::vector<MSVehicle::LaneQ> > &preb)
/*
int bestLaneOffset, SUMOReal bestDist,
SUMOReal currentDist)
*/
{
    // Try to change to the right-lane if there is one. If this lane isn't
    // an allowed one, cancel the try. Otherwise, check some conditions. If
    // they are simultaniously fulfilled, a change is possible.

    // no right lane -> exit
    if (myCandi == myChanger.begin()) {
        return 0;
    }

    ChangerIt target = myCandi - 1;
    if (!target->lane->allowsVehicleClass(veh(myCandi)->getVehicleClass())) {
        return 0;
    }
    int blocked = overlapWithHopped(target)
                  ? target->hoppedVeh->getPositionOnLane()<veh(myCandi)->getPositionOnLane()
                  ? LCA_BLOCKEDBY_FOLLOWER
                  : LCA_BLOCKEDBY_LEADER
              : 0;
    setOverlap(rLead, rFollow, /*target, */blocked);
    setIsSafeChange(rLead, rFollow, target, blocked);
    return blocked
           |
           advan2right(leader, rLead, rFollow,
                       blocked, preb);//bestLaneOffset, bestDist, currentDist);
}

//-------------------------------------------------------------------------//

int
MSLaneChanger::change2left(const std::pair<MSVehicle*, SUMOReal> &leader,
                           const std::pair<MSVehicle*, SUMOReal> &rLead,
                           const std::pair<MSVehicle*, SUMOReal> &rFollow,
                           const std::vector<std::vector<MSVehicle::LaneQ> > &preb)
/*
int bestLaneOffset, SUMOReal bestDist,
SUMOReal currentDist)
*/
{
    // Try to change to the left-lane, if there is one. If this lane isn't
    // an allowed one, cancel the try. Otherwise, check some conditions.
    // If they are simultaniously fulfilled, a change is possible.

    // no left lane, overlapping or left lane not allowed -> exit
    ChangerIt target = myCandi + 1;
    if (target == myChanger.end()) {
        return 0;
    }
    if (!target->lane->allowsVehicleClass(veh(myCandi)->getVehicleClass())) {
        return 0;
    }
    int blocked = overlapWithHopped(target)
                  ? target->hoppedVeh->getPositionOnLane()<veh(myCandi)->getPositionOnLane()
                  ? LCA_BLOCKEDBY_FOLLOWER
                  : LCA_BLOCKEDBY_LEADER
              : 0;
    setOverlap(rLead, rFollow, /*target, */blocked);
    setIsSafeChange(rLead, rFollow, target, blocked);
    return blocked
           |
           advan2left(leader, rLead, rFollow,
                      blocked, preb);//bestLaneOffset, bestDist, currentDist);
}

//-------------------------------------------------------------------------//

MSLaneChanger::ChangerIt
MSLaneChanger::findTarget()
{
    // If candidate is on a not allowed lane, it has to search for a
    // neighboured lane that will bring it closer to an allowed one.
    // Certainly there is a more elegant way than looping over all lanes.
    ChangerIt nearestTarget = myChanger.end();
    unsigned int minDist    = myChanger.size();

    for (ChangerIt ce = myChanger.begin(); ce != myChanger.end(); ++ce) {

        if (ce == myCandi) {

            continue;
        }
        if (candiOnAllowed(ce)) {

            unsigned int dist = abs(distance(myCandi, ce));
            assert(dist > 0);
            assert(dist < myChanger.size());
            if (dist < minDist) {

                minDist       = dist;
                nearestTarget = ce;
            }
        }
    }
    assert(nearestTarget != myChanger.end());
    if (distance(myCandi, nearestTarget) > 0) {
        assert(myCandi + 1 != myChanger.end());
        return myCandi + 1;
    }
    assert(myCandi != myChanger.begin());
    return myCandi - 1;
}

//-------------------------------------------------------------------------//

void
MSLaneChanger::setOverlap(const std::pair<MSVehicle*, SUMOReal> &rLead,
                          const std::pair<MSVehicle*, SUMOReal> &rFollow,
                          /*const ChangerIt &target, */int &blocked)
{
    // check the follower only if not already known that...
    if ((blocked&LCA_BLOCKEDBY_FOLLOWER)==0) {
        if (rFollow.first!=0&&rFollow.second<0) {
            blocked |= (LCA_BLOCKEDBY_FOLLOWER|LCA_OVERLAPPING);
        }
    }
    // check the leader only if not already known that...
    if ((blocked&LCA_BLOCKEDBY_LEADER)==0) {
        if (rLead.first!=0&&rLead.second<0) {
            blocked |= (LCA_BLOCKEDBY_LEADER|LCA_OVERLAPPING);
        }
    }
}

//-------------------------------------------------------------------------//

void
MSLaneChanger::setIsSafeChange(const std::pair<MSVehicle*, SUMOReal> &neighLead,
                               const std::pair<MSVehicle*, SUMOReal> &neighFollow,
                               const ChangerIt &target, int &blocked)
{
    // Check if candidate's change to target-lane will be safe, i.e. is there
    // enough back-gap to the neighFollow to drive collision-free (if there is
    // no neighFollow, keep a safe-gap to the beginning of the lane) and is
    // there enough gap for the candidate to neighLead to drive collision-
    // free (if there is no neighLead, be sure that candidate is able to slow-
    // down towards the lane end).
    MSVehicle* vehicle     = veh(myCandi);

    // check back gap
    if ((blocked&LCA_BLOCKEDBY_FOLLOWER)==0) {
        if (neighFollow.first!=0) {
            MSLane* targetLane = target->lane;
            // !!! eigentlich: vsafe braucht die Max. Geschwindigkeit beider Spuren
            if (!neighFollow.first->hasSafeGap(neighFollow.first->getSpeed(), neighFollow.second, vehicle->getSpeed(), targetLane->maxSpeed())) {
//                neighFollow.second, *vehicle, targetLane)) {
                blocked |= LCA_BLOCKEDBY_FOLLOWER;
            }
        }
    }

    // check front gap
    if ((blocked&LCA_BLOCKEDBY_LEADER)==0) {
        if (neighLead.first!=0) {
            MSLane* targetLane = target->lane;
            // !!! eigentlich: vsafe braucht die Max. Geschwindigkeit beider Spuren
            if (!vehicle->hasSafeGap(vehicle->getSpeed(), neighLead.second, neighLead.first->getSpeed(), targetLane->maxSpeed())) {
//                neighLead.second, *neighLead.first, targetLane)) {
                blocked |= LCA_BLOCKEDBY_LEADER;
            }
        }
    }
}

//-------------------------------------------------------------------------//

int
MSLaneChanger::advan2right(const std::pair<MSVehicle*, SUMOReal> &leader,
                           const std::pair<MSVehicle*, SUMOReal> &neighLead,
                           const std::pair<MSVehicle*, SUMOReal> &neighFollow,
                           int blocked,
                           const std::vector<std::vector<MSVehicle::LaneQ> > &preb)
/*
int bestLaneOffset, SUMOReal bestDist,
SUMOReal currentDist)
*/
{
    MSAbstractLaneChangeModel::MSLCMessager
    msg(leader.first, neighLead.first, neighFollow.first);
    return veh(myCandi)->getLaneChangeModel().wantsChangeToRight(
               msg, blocked,
               leader, neighLead, neighFollow, *(myCandi-1)->lane,
               preb,
               &(myCandi->lastBlocked));
}

//-------------------------------------------------------------------------//

int
MSLaneChanger::advan2left(const std::pair<MSVehicle*, SUMOReal> &leader,
                          const std::pair<MSVehicle*, SUMOReal> &neighLead,
                          const std::pair<MSVehicle*, SUMOReal> &neighFollow,
                          int blocked,
                          const std::vector<std::vector<MSVehicle::LaneQ> > &preb)
/*
int bestLaneOffset, SUMOReal bestDist,
SUMOReal currentDist)
*/
{
    MSAbstractLaneChangeModel::MSLCMessager
    msg(leader.first, neighLead.first, neighFollow.first);
    return veh(myCandi)->getLaneChangeModel().wantsChangeToLeft(
               msg, blocked,
               leader, neighLead, neighFollow, *(myCandi+1)->lane,
               preb,
               &(myCandi->lastBlocked));
}



/****************************************************************************/

