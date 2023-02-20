/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2023 German Aerospace Center (DLR) and others.
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
/// @file    MSLaneChanger.cpp
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @author  Friedemann Wesner
/// @author  Jakob Erdmann
/// @date    Fri, 01 Feb 2002
///
// Performs lane changing of vehicles
/****************************************************************************/
#include <config.h>

#include "MSLaneChanger.h"
#include "MSNet.h"
#include "MSLink.h"
#include "MSVehicle.h"
#include "MSVehicleType.h"
#include "MSVehicleTransfer.h"
#include "MSStop.h"
#include "MSGlobals.h"
#include <cassert>
#include <iterator>
#include <cstdlib>
#include <cmath>
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include <microsim/transportables/MSTransportableControl.h>
#include <microsim/transportables/MSPModel.h>
#include <utils/common/MsgHandler.h>

#define OPPOSITE_OVERTAKING_SAFE_TIMEGAP 0.0
#define OPPOSITE_OVERTAKING_SAFETYGAP_HEADWAY_FACTOR 0.0
#define OPPOSITE_OVERTAKING_SAFETY_FACTOR 1.2
// XXX maxLookAhead should be higher if all leaders are stopped and lower when they are jammed/queued
#define OPPOSITE_OVERTAKING_MAX_LOOKAHEAD 150.0 // just a guess
#define OPPOSITE_OVERTAKING_MAX_LOOKAHEAD_EMERGENCY 1000.0 // just a guess
// this is used for finding oncoming vehicles while driving in the opposite direction
#define OPPOSITE_OVERTAKING_ONCOMING_LOOKAHEAD 1000.0 // just a guess
// do not attempt overtaking maneuvers that would exceed this distance
#define OPPOSITE_OVERTAKING_MAX_SPACE_TO_OVERTAKE 1000.0 // just a guess
#define OPPOSITE_OVERTAKING_HILLTOP_THRESHOHOLD 5 // (m)

// halting time to signal yielding in deadlock
#define OPPOSITE_OVERTAKING_DEADLOCK_WAIT 1 // (s)

// ===========================================================================
// debug defines
// ===========================================================================

//#define DEBUG_CONTINUE_CHANGE
//#define DEBUG_CHECK_CHANGE
//#define DEBUG_SURROUNDING_VEHICLES // debug getRealFollower() and getRealLeader()
//#define DEBUG_CHANGE_OPPOSITE
//#define DEBUG_CHANGE_OPPOSITE_OVERTAKINGTIME
//#define DEBUG_CHANGE_OPPOSITE_DEADLOCK
//#define DEBUG_ACTIONSTEPS
//#define DEBUG_STATE
//#define DEBUG_CANDIDATE
//#define DEBUG_COND (vehicle->getLaneChangeModel().debugVehicle())
#define DEBUG_COND (vehicle->isSelected())
//#define DEBUG_COND (true)



// ===========================================================================
// ChangeElem member method definitions
// ===========================================================================
MSLaneChanger::ChangeElem::ChangeElem(MSLane* _lane) :
    lead(nullptr),
    lane(_lane),
    hoppedVeh(nullptr),
    lastBlocked(nullptr),
    firstBlocked(nullptr),
    lastStopped(nullptr),
    ahead(_lane->getWidth()),
    aheadNext(_lane->getWidth(), nullptr, 0.) {
}

void
MSLaneChanger::ChangeElem::registerHop(MSVehicle* vehicle) {
    //std::cout << SIMTIME << " registerHop lane=" << lane->getID() << " veh=" << vehicle->getID() << "\n";
    lane->myTmpVehicles.insert(lane->myTmpVehicles.begin(), vehicle);
    dens += vehicle->getVehicleType().getLengthWithGap();
    hoppedVeh = vehicle;
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
        myChanger.back().mayChangeRight = lane != lanes->begin();
        myChanger.back().mayChangeLeft = (lane + 1) != lanes->end();
        if ((*lane)->isInternal()) {
            // avoid changing on internal sibling lane
            if (myChanger.back().mayChangeRight && (*lane)->getLogicalPredecessorLane() == (*(lane - 1))->getLogicalPredecessorLane()) {
                myChanger.back().mayChangeRight = false;
            }
            if (myChanger.back().mayChangeLeft && (*lane)->getLogicalPredecessorLane() == (*(lane + 1))->getLogicalPredecessorLane()) {
                myChanger.back().mayChangeLeft = false;
            }
            // avoid changing if lanes have different lengths
            if (myChanger.back().mayChangeRight && (*lane)->getLength() != (*(lane - 1))->getLength()) {
                //std::cout << " cannot change right from lane=" << (*lane)->getID() << " len=" << (*lane)->getLength() << " to=" << (*(lane - 1))->getID() << " len2=" << (*(lane - 1))->getLength() << "\n";
                myChanger.back().mayChangeRight = false;
            }
            if (myChanger.back().mayChangeLeft && (*lane)->getLength() != (*(lane + 1))->getLength()) {
                //std::cout << " cannot change left from lane=" << (*lane)->getID() << " len=" << (*lane)->getLength() << " to=" << (*(lane + 1))->getID() << " len2=" << (*(lane + 1))->getLength() << "\n";
                myChanger.back().mayChangeLeft = false;
            }
        }
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
        ce->lead = nullptr;
        ce->hoppedVeh = nullptr;
        ce->lastBlocked = nullptr;
        ce->firstBlocked = nullptr;
        ce->lastStopped = nullptr;
        ce->dens = 0;
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
    if (!vehHasChanged) {
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
#ifdef DEBUG_CANDIDATE
    std::cout << SIMTIME << " findCandidate() on edge " << myChanger.begin()->lane->getEdge().getID() << std::endl;
#endif

    for (ChangerIt ce = myChanger.begin(); ce != myChanger.end(); ++ce) {
        if (veh(ce) == nullptr) {
            continue;
        }
#ifdef DEBUG_CANDIDATE
        std::cout << "     lane = " << ce->lane->getID() << "\n";
        std::cout << "     check vehicle=" << veh(ce)->getID() << " pos=" << veh(ce)->getPositionOnLane() << " lane=" << ce->lane->getID() << " isFrontOnLane=" << veh(ce)->isFrontOnLane(ce->lane)  << "\n";
#endif
        if (max == myChanger.end()) {
#ifdef DEBUG_CANDIDATE
            std::cout << "     new max vehicle=" << veh(ce)->getID() << " pos=" << veh(ce)->getPositionOnLane() << " lane=" << ce->lane->getID() << " isFrontOnLane=" << veh(ce)->isFrontOnLane(ce->lane)  << "\n";
#endif
            max = ce;
            continue;
        }
        assert(veh(ce)  != 0);
        assert(veh(max) != 0);
        if (veh(max)->getPositionOnLane() < veh(ce)->getPositionOnLane()) {
#ifdef DEBUG_CANDIDATE
            std::cout << "     new max vehicle=" << veh(ce)->getID() << " pos=" << veh(ce)->getPositionOnLane() << " lane=" << ce->lane->getID() << " isFrontOnLane=" << veh(ce)->isFrontOnLane(ce->lane)  << " oldMaxPos=" << veh(max)->getPositionOnLane() << "\n";
#endif
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
    SUMOVehicleClass svc = veh(myCandi)->getVClass();
    if (direction == -1) {
        return myCandi->mayChangeRight && (myCandi - 1)->lane->allowsVehicleClass(svc) && myCandi->lane->allowsChangingRight(svc);
    } else if (direction == 1) {
        return myCandi->mayChangeLeft && (myCandi + 1)->lane->allowsVehicleClass(svc) && myCandi->lane->allowsChangingLeft(svc);
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

#ifdef DEBUG_ACTIONSTEPS
//        std::cout<< "\nCHANGE" << std::endl;
#endif


    myCandi = findCandidate();
    MSVehicle* vehicle = veh(myCandi);
    vehicle->getLaneChangeModel().clearNeighbors();

    if (vehicle->getLaneChangeModel().isChangingLanes() && !vehicle->getLaneChangeModel().alreadyChanged()) {
        return continueChange(vehicle, myCandi);
    }
    if (!myAllowsChanging || vehicle->getLaneChangeModel().alreadyChanged() || vehicle->isStoppedOnLane()) {
        registerUnchanged(vehicle);
        if (vehicle->isStoppedOnLane()) {
            myCandi->lastStopped = vehicle;
        }
        return false;
    }

    if (!vehicle->isActive()) {
#ifdef DEBUG_ACTIONSTEPS
        if (DEBUG_COND) {
            std::cout << SIMTIME << " veh '" << vehicle->getID() << "' skips regular change checks." << std::endl;
        }
#endif
        bool changed = false;
        const int oldstate = vehicle->getLaneChangeModel().getOwnState();
        // let TraCI influence the wish to change lanes during non-actionsteps
        checkTraCICommands(vehicle);
        if (oldstate != vehicle->getLaneChangeModel().getOwnState()) {
            changed = applyTraCICommands(vehicle);
        }
        if (!changed) {
            registerUnchanged(vehicle);
        }
        return changed;
    }

    // Check for changes to the opposite lane if vehicle is active
    std::pair<MSVehicle* const, double> leader = getRealLeader(myCandi);
    if (myChanger.size() == 1 || vehicle->getLaneChangeModel().isOpposite() || (!mayChange(-1) && !mayChange(1))) {
        if (changeOpposite(vehicle, leader, myCandi->lastStopped)) {
            return true;
        }
        registerUnchanged(vehicle);
        return false;
    }

    vehicle->updateBestLanes(); // needed?
    for (int i = 0; i < (int) myChanger.size(); ++i) {
        vehicle->adaptBestLanesOccupation(i, myChanger[i].dens);
    }

    const std::vector<MSVehicle::LaneQ>& preb = vehicle->getBestLanes();
    // check whether the vehicle wants and is able to change to right lane
    int stateRight = 0;
    if (mayChange(-1)) {
        stateRight = checkChangeWithinEdge(-1, leader, preb);
        // change if the vehicle wants to and is allowed to change
        if ((stateRight & LCA_RIGHT) != 0 && (stateRight & LCA_BLOCKED) == 0) {
            vehicle->getLaneChangeModel().setOwnState(stateRight);
            return startChange(vehicle, myCandi, -1);
        }
        if ((stateRight & LCA_RIGHT) != 0 && (stateRight & LCA_URGENT) != 0) {
            (myCandi - 1)->lastBlocked = vehicle;
            if ((myCandi - 1)->firstBlocked == nullptr) {
                (myCandi - 1)->firstBlocked = vehicle;
            }
        }
    }

    // check whether the vehicle wants and is able to change to left lane
    int stateLeft = 0;
    if (mayChange(1)) {
        stateLeft = checkChangeWithinEdge(1, leader, preb);
        // change if the vehicle wants to and is allowed to change
        if ((stateLeft & LCA_LEFT) != 0 && (stateLeft & LCA_BLOCKED) == 0) {
            vehicle->getLaneChangeModel().setOwnState(stateLeft);
            return startChange(vehicle, myCandi, 1);
        }
        if ((stateLeft & LCA_LEFT) != 0 && (stateLeft & LCA_URGENT) != 0) {
            (myCandi + 1)->lastBlocked = vehicle;
            if ((myCandi + 1)->firstBlocked == nullptr) {
                (myCandi + 1)->firstBlocked = vehicle;
            }
        }
    }

    if ((stateRight & LCA_URGENT) != 0 && (stateLeft & LCA_URGENT) != 0) {
        // ... wants to go to the left AND to the right
        // just let them go to the right lane...
        stateLeft = 0;
    }
    vehicle->getLaneChangeModel().setOwnState(stateRight | stateLeft);

    // only emergency vehicles should change to the opposite side on a
    // multi-lane road (or vehicles that need to stop on the opposite side)
    if ((vehicle->getVehicleType().getVehicleClass() == SVC_EMERGENCY
            || hasOppositeStop(vehicle))
            && changeOpposite(vehicle, leader, myCandi->lastStopped)) {
        return true;
    }

    registerUnchanged(vehicle);
    return false;
}


void
MSLaneChanger::registerUnchanged(MSVehicle* vehicle) {
    //std::cout << SIMTIME << " registerUnchanged lane=" << myCandi->lane->getID() << " veh=" << vehicle->getID() << "\n";
    myCandi->lane->myTmpVehicles.insert(myCandi->lane->myTmpVehicles.begin(), veh(myCandi));
    myCandi->dens += vehicle->getVehicleType().getLengthWithGap();
    vehicle->getLaneChangeModel().unchanged();
}



void
MSLaneChanger::checkTraCICommands(MSVehicle* vehicle) {
#ifdef DEBUG_STATE
    const int oldstate = vehicle->getLaneChangeModel().getOwnState();
#endif
    vehicle->getLaneChangeModel().checkTraCICommands();
#ifdef DEBUG_STATE
    if (DEBUG_COND) {
        const int newstate = vehicle->getLaneChangeModel().getOwnState();
        std::cout << SIMTIME
                  << " veh=" << vehicle->getID()
                  << " oldState=" << toString((LaneChangeAction) oldstate)
                  << " newState=" << toString((LaneChangeAction) newstate)
                  << ((newstate & LCA_BLOCKED) != 0 ? " (blocked)" : "")
                  << ((newstate & LCA_OVERLAPPING) != 0 ? " (overlap)" : "")
                  << "\n";
    }
#endif
}


bool
MSLaneChanger::applyTraCICommands(MSVehicle* vehicle) {
    // Execute request if not blocked
    bool changed = false;
    const int state = vehicle->getLaneChangeModel().getOwnState();
    const int dir = (state & LCA_RIGHT) != 0 ? -1 : ((state & LCA_LEFT) != 0 ? 1 : 0);
    const bool execute = dir != 0 && ((state & LCA_BLOCKED) == 0);
    if (execute) {
        ChangerIt to = myCandi + dir;
        bool continuous = vehicle->getLaneChangeModel().startLaneChangeManeuver(myCandi->lane, to->lane, dir);
        if (continuous) {
            changed = continueChange(vehicle, myCandi);
        } else {
            // insert vehicle into target lane
            to->registerHop(vehicle);
            changed = true;
        }
    }
    return changed;
}


bool
MSLaneChanger::startChange(MSVehicle* vehicle, ChangerIt& from, int direction) {
    if (vehicle->isRemoteControlled()) {
        registerUnchanged(vehicle);
        return false;
    }
    ChangerIt to = from + direction;
    // @todo delay entering the target lane until the vehicle intersects it
    //       physically (considering lane width and vehicle width)
    //if (to->lane->getID() == "beg_1") std::cout << SIMTIME << " startChange to lane=" << to->lane->getID() << " myTmpVehiclesBefore=" << toString(to->lane->myTmpVehicles) << "\n";
    const bool continuous = vehicle->getLaneChangeModel().startLaneChangeManeuver(from->lane, to->lane, direction);
    if (continuous) {
        return continueChange(vehicle, myCandi);
    } else {
        to->registerHop(vehicle);
        to->lane->requireCollisionCheck();
        return true;
    }
}

bool
MSLaneChanger::continueChange(MSVehicle* vehicle, ChangerIt& from) {
    MSAbstractLaneChangeModel& lcm = vehicle->getLaneChangeModel();
    const int direction = lcm.isOpposite() ? -lcm.getLaneChangeDirection() : lcm.getLaneChangeDirection();
    const bool pastMidpoint = lcm.updateCompletion(); // computes lcm.mySpeedLat as a side effect
    const double speedLat = lcm.isOpposite() ? -lcm.getSpeedLat() : lcm.getSpeedLat();
    vehicle->myState.myPosLat += SPEED2DIST(speedLat);
    vehicle->myCachedPosition = Position::INVALID;
    //std::cout << SIMTIME << " veh=" << vehicle->getID() << " dir=" << direction << " pm=" << pastMidpoint << " speedLat=" << speedLat << " posLat=" << vehicle->myState.myPosLat << "\n";
    if (pastMidpoint) {
        MSLane* source = myCandi->lane;
        MSLane* target = source->getParallelLane(direction);
        vehicle->myState.myPosLat -= direction * 0.5 * (source->getWidth() + target->getWidth());
        lcm.primaryLaneChanged(source, target, direction);
        if (&source->getEdge() == &target->getEdge()) {
            ChangerIt to = from + direction;
            to->registerHop(vehicle);
        }
        target->requireCollisionCheck();
    } else {
        from->registerHop(vehicle);
        from->lane->requireCollisionCheck();
    }
    if (!lcm.isChangingLanes()) {
        vehicle->myState.myPosLat = 0;
        lcm.endLaneChangeManeuver();
    }
    lcm.updateShadowLane();
    if (lcm.getShadowLane() != nullptr && &lcm.getShadowLane()->getEdge() == &vehicle->getLane()->getEdge()) {
        // set as hoppedVeh on the shadow lane so it is found as leader on both lanes
        myChanger[lcm.getShadowLane()->getIndex()].hoppedVeh = vehicle;
        lcm.getShadowLane()->requireCollisionCheck();
    }
    vehicle->myAngle = vehicle->computeAngle();
    if (lcm.isOpposite()) {
        vehicle->myAngle += M_PI;
    }

#ifdef DEBUG_CONTINUE_CHANGE
    if (DEBUG_COND) {
        std::cout << SIMTIME
                  << " continueChange veh=" << vehicle->getID()
                  << " from=" << Named::getIDSecure(from->lane)
                  << " dir=" << direction
                  << " speedLat=" << speedLat
                  << " pastMidpoint=" << pastMidpoint
                  << " posLat=" << vehicle->getLateralPositionOnLane()
                  << " completion=" << lcm.getLaneChangeCompletion()
                  << " shadowLane=" << Named::getIDSecure(lcm.getShadowLane())
                  //<< " shadowHopped=" << Named::getIDSecure(shadow->lane)
                  << "\n";
    }
#endif
    return pastMidpoint && lcm.getShadowLane() == nullptr;
}


std::pair<MSVehicle* const, double>
MSLaneChanger::getRealLeader(const ChangerIt& target) const {
    assert(veh(myCandi) != 0);
    MSVehicle* vehicle = veh(myCandi);
#ifdef DEBUG_SURROUNDING_VEHICLES
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

    // check whether the hopped vehicle became the leader
    if (target->hoppedVeh != nullptr) {
        double hoppedPos = target->hoppedVeh->getPositionOnLane();
#ifdef DEBUG_SURROUNDING_VEHICLES
        if (DEBUG_COND) {
            std::cout << "Considering hopped vehicle '" << target->hoppedVeh->getID() << "' at position " << hoppedPos << std::endl;
        }
#endif
        if (hoppedPos > vehicle->getPositionOnLane() && (neighLead == nullptr || neighLead->getPositionOnLane() > hoppedPos)) {
            neighLead = target->hoppedVeh;
            //if (vehicle->getID() == "flow.21") std::cout << SIMTIME << " neighLead=" << Named::getIDSecure(neighLead) << " (422)\n";
        }
    }
    if (neighLead == nullptr) {
#ifdef DEBUG_SURROUNDING_VEHICLES
        if (DEBUG_COND) {
            std::cout << "Looking for leader on consecutive lanes." << std::endl;
        }
#endif
        // There's no leader on the target lane. Look for leaders on consecutive lanes.
        // (there might also be partial leaders due to continuous lane changing)
        MSLane* targetLane = target->lane;
        const double egoBack = vehicle->getBackPositionOnLane();
        double leaderBack = targetLane->getLength();
        for (MSVehicle* pl : targetLane->myPartialVehicles) {
            double plBack = pl->getBackPositionOnLane(targetLane);
            if (plBack < leaderBack &&
                    pl->getPositionOnLane(targetLane) + pl->getVehicleType().getMinGap() >= egoBack) {
                neighLead = pl;
                leaderBack = plBack;
            }
        }
        if (neighLead != nullptr) {
#ifdef DEBUG_SURROUNDING_VEHICLES
            if (DEBUG_COND) {
                std::cout << "  found leader=" << neighLead->getID() << " (partial)\n";
            }
#endif
            return std::pair<MSVehicle*, double>(neighLead, leaderBack - vehicle->getPositionOnLane() - vehicle->getVehicleType().getMinGap());
        }
        double seen = myCandi->lane->getLength() - vehicle->getPositionOnLane();
        double speed = vehicle->getSpeed();
        double dist = vehicle->getCarFollowModel().brakeGap(speed) + vehicle->getVehicleType().getMinGap();
        // always check for link leaders while on an internal lane
        if (seen > dist && !myCandi->lane->isInternal()) {
#ifdef DEBUG_SURROUNDING_VEHICLES
            if (DEBUG_COND) {
                std::cout << "  found no leader within dist=" << dist << "\n";
            }
#endif
            return std::pair<MSVehicle* const, double>(static_cast<MSVehicle*>(nullptr), -1);
        }
        const std::vector<MSLane*>& bestLaneConts = vehicle->getBestLanesContinuation(targetLane);

        std::pair<MSVehicle* const, double> result = target->lane->getLeaderOnConsecutive(dist, seen, speed, *vehicle, bestLaneConts);
#ifdef DEBUG_SURROUNDING_VEHICLES
        if (DEBUG_COND) {
            std::cout << "  found consecutiveLeader=" << Named::getIDSecure(result.first) << "\n";
        }
#endif
        return result;
    } else {
#ifdef DEBUG_SURROUNDING_VEHICLES
        if (DEBUG_COND) {
            std::cout << "  found leader=" << neighLead->getID() << "\n";
        }
#endif
        return std::pair<MSVehicle* const, double>(neighLead, neighLead->getBackPositionOnLane(target->lane) - vehicle->getPositionOnLane() - vehicle->getVehicleType().getMinGap());
    }
}


std::pair<MSVehicle* const, double>
MSLaneChanger::getRealFollower(const ChangerIt& target) const {
    assert(veh(myCandi) != 0);
    MSVehicle* vehicle = veh(myCandi);
#ifdef DEBUG_SURROUNDING_VEHICLES
    if (DEBUG_COND) {
        std::cout << SIMTIME << " veh '" << vehicle->getID() << "' looks for follower on lc-target lane '" << target->lane->getID() << "'." << std::endl;
    }
#endif
    const double candiPos = vehicle->getPositionOnLane();
    MSVehicle* neighFollow = nullptr;
    if (target != myCandi) {
        neighFollow = veh(target);
    } else {
        // veh(target) would return the ego vehicle so we use it's predecessor instead
        if (target->lane->myVehicles.size() > 1) {
            neighFollow = target->lane->myVehicles[target->lane->myVehicles.size() - 2];
        }
    }

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
        MSVehicle* partialBehind = getCloserFollower(candiPos, neighFollow, target->lane->getPartialBehind(vehicle));
        if (partialBehind != 0 && partialBehind != neighFollow) {
            std::cout << "'Partial behind'-vehicle '" << target->lane->getPartialBehind(vehicle)->getID() << "' at position " << partialBehind->getPositionOnLane() << " is closer." <<  std::endl;
        }
    }
#endif
    // or a follower which is partially lapping into the target lane
    neighFollow = getCloserFollower(candiPos, neighFollow, target->lane->getPartialBehind(vehicle));

    if (neighFollow == nullptr) {
        CLeaderDist consecutiveFollower = target->lane->getFollowersOnConsecutive(vehicle, vehicle->getBackPositionOnLane(), true)[0];
#ifdef DEBUG_SURROUNDING_VEHICLES
        if (DEBUG_COND) {
            if (consecutiveFollower.first == 0) {
                std::cout << "no follower found." <<  std::endl;
            } else {
                std::cout << "found follower '" << consecutiveFollower.first->getID() << "' on consecutive lanes." <<  std::endl;
            }
        }
#endif
        return std::make_pair(const_cast<MSVehicle*>(consecutiveFollower.first), consecutiveFollower.second);
    } else {
#ifdef DEBUG_SURROUNDING_VEHICLES
        if (DEBUG_COND) {
            std::cout << "found follower '" << neighFollow->getID() << "'." <<  std::endl;
        }
#endif
        return std::pair<MSVehicle* const, double>(neighFollow,
                vehicle->getPositionOnLane() - vehicle->getVehicleType().getLength() - neighFollow->getPositionOnLane() - neighFollow->getVehicleType().getMinGap());
    }
}


MSVehicle*
MSLaneChanger::getCloserFollower(const double maxPos, MSVehicle* follow1, MSVehicle* follow2) {
    if (follow1 == nullptr || follow1->getPositionOnLane() > maxPos) {
        return follow2;
    } else if (follow2 == nullptr || follow2->getPositionOnLane() > maxPos) {
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
    const std::pair<MSVehicle* const, double>& leader,
    const std::vector<MSVehicle::LaneQ>& preb) const {

    std::pair<MSVehicle*, double> follower = getRealFollower(myCandi);
    std::pair<MSVehicle* const, double> neighLead = getRealLeader(myCandi + laneOffset);
    std::pair<MSVehicle*, double> neighFollow = getRealFollower(myCandi + laneOffset);
    if (neighLead.first != nullptr && neighLead.first == neighFollow.first) {
        // vehicles should not be leader and follower at the same time to avoid
        // contradictory behavior
        neighFollow.first = 0;
    }
    ChangerIt target = myCandi + laneOffset;
    return checkChange(laneOffset, target->lane, leader, follower, neighLead, neighFollow, preb);
}

int
MSLaneChanger::checkChange(
    int laneOffset,
    const MSLane* targetLane,
    const std::pair<MSVehicle* const, double>& leader,
    const std::pair<MSVehicle* const, double>& follower,
    const std::pair<MSVehicle* const, double>& neighLead,
    const std::pair<MSVehicle* const, double>& neighFollow,
    const std::vector<MSVehicle::LaneQ>& preb) const {

    MSVehicle* vehicle = veh(myCandi);

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
    if (neighFollow.first != nullptr && neighFollow.second < 0) {
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
    if (neighLead.first != nullptr && neighLead.second < 0) {
        blocked |= (blockedByLeader | LCA_OVERLAPPING);

#ifdef DEBUG_CHECK_CHANGE
        if (DEBUG_COND) {
            std::cout << SIMTIME
                      <<  " overlapping with leader..."
                      << std::endl;
        }
#endif

    }
    double secureFrontGap = MSAbstractLaneChangeModel::NO_NEIGHBOR;
    double secureBackGap = MSAbstractLaneChangeModel::NO_NEIGHBOR;
    double secureOrigFrontGap = MSAbstractLaneChangeModel::NO_NEIGHBOR;

    const double tauRemainder = vehicle->getActionStepLength() == DELTA_T ? 0 : MAX2(vehicle->getCarFollowModel().getHeadwayTime() - TS, 0.);
    // safe back gap
    if ((blocked & blockedByFollower) == 0 && neighFollow.first != nullptr) {
        // Calculate secure gap conservatively with vNextFollower / vNextLeader as
        // extrapolated speeds after the driver's expected reaction time (tau).
        // NOTE: there exists a possible source for collisions if the follower and the leader
        //       have desynchronized action steps as the extrapolated speeds can be exceeded in this case

        // Expected reaction time (tau) for the follower-vehicle.
        // (subtracted TS since at this point the vehicles' states are already updated)
        const double vNextFollower = neighFollow.first->getSpeed() + MAX2(0., tauRemainder * neighFollow.first->getAcceleration());
        const double vNextLeader = vehicle->getSpeed() + MIN2(0., tauRemainder * vehicle->getAcceleration());
        // !!! eigentlich: vsafe braucht die Max. Geschwindigkeit beider Spuren
        secureBackGap = neighFollow.first->getCarFollowModel().getSecureGap(neighFollow.first, vehicle, vNextFollower,
                        vNextLeader, vehicle->getCarFollowModel().getMaxDecel());
#ifdef DEBUG_CHECK_CHANGE
        if (DEBUG_COND) {
            std::cout << SIMTIME
                      << " follower=" << neighFollow.first->getID()
                      << " backGap=" << neighFollow.second
                      << " vNextFollower=" << vNextFollower
                      << " vNextEgo=" << vNextLeader
                      << " secureGap=" << secureBackGap
                      << " safetyFactor=" << vehicle->getLaneChangeModel().getSafetyFactor()
                      << " blocked=" << (neighFollow.second < secureBackGap * vehicle->getLaneChangeModel().getSafetyFactor())
                      << "\n";
        }
#endif
        if (neighFollow.second < secureBackGap * vehicle->getLaneChangeModel().getSafetyFactor()) {
            if (vehicle->getLaneChangeModel().isOpposite()
                    && neighFollow.first->getLaneChangeModel().getLastLaneChangeOffset() == laneOffset) {
                // during opposite direction driving, the vehicle are handled in
                // downstream rather than upstream order, the neighFollower may have
                // been the actual follower in this simulation step and should not
                // block changing in this case
#ifdef DEBUG_CHECK_CHANGE
                if (DEBUG_COND) {
                    std::cout << "  ignoring opposite follower who changed in this step\n";
                }
#endif
            } else {
                blocked |= blockedByFollower;
            }
        }
    }

    // safe front gap
    if ((blocked & blockedByLeader) == 0 && neighLead.first != nullptr) {
        // Calculate secure gap conservatively with vNextFollower / vNextLeader as
        // extrapolated speeds after the driver's expected reaction time (tau).
        // NOTE: there exists a possible source for collisions if the follower and the leader
        //       have desynchronized action steps as the extrapolated speeds can be exceeded in this case

        // Expected reaction time (tau) for the follower-vehicle.
        // (subtracted TS since at this point the vehicles' states are already updated)
        const double vNextFollower = vehicle->getSpeed() + MAX2(0., tauRemainder * vehicle->getAcceleration());
        const double vNextLeader = neighLead.first->getSpeed() + MIN2(0., tauRemainder * neighLead.first->getAcceleration());
        // !!! eigentlich: vsafe braucht die Max. Geschwindigkeit beider Spuren
        secureFrontGap = vehicle->getCarFollowModel().getSecureGap(vehicle, neighLead.first, vNextFollower,
                         vNextLeader, neighLead.first->getCarFollowModel().getMaxDecel());
#ifdef DEBUG_CHECK_CHANGE
        if (DEBUG_COND) {
            std::cout << SIMTIME
                      << " leader=" << neighLead.first->getID()
                      << " frontGap=" << neighLead.second
                      << " vNextEgo=" << vNextFollower
                      << " vNextLeader=" << vNextLeader
                      << " secureGap=" << secureFrontGap
                      << " safetyFactor=" << vehicle->getLaneChangeModel().getSafetyFactor()
                      << " blocked=" << (neighLead.second < secureFrontGap * vehicle->getLaneChangeModel().getSafetyFactor())
                      << "\n";
        }
#endif
        if (neighLead.second < secureFrontGap * vehicle->getLaneChangeModel().getSafetyFactor()) {
            blocked |= blockedByLeader;
        }
    }
    if (blocked == 0 && targetLane->hasPedestrians()) {
        PersonDist nextLeader = targetLane->nextBlocking(vehicle->getBackPositionOnLane(),
                                vehicle->getRightSideOnLane(), vehicle->getRightSideOnLane() + vehicle->getVehicleType().getWidth(),
                                ceil(vehicle->getSpeed() / vehicle->getCarFollowModel().getMaxDecel()));
        if (nextLeader.first != 0) {
            const double brakeGap = vehicle->getCarFollowModel().brakeGap(vehicle->getSpeed());
            // returned gap value is relative to backPosition
            const double gap = nextLeader.second - vehicle->getVehicleType().getLengthWithGap();
#ifdef DEBUG_CHECK_CHANGE
            if (DEBUG_COND) {
                std::cout << SIMTIME << "  pedestrian on road " + leader.first->getID() << " gap=" << gap << " brakeGap=" << brakeGap << "\n";
            }
#endif
            if (brakeGap > gap) {
                blocked |= blockedByLeader;
#ifdef DEBUG_CHECK_CHANGE
                if (DEBUG_COND) {
                    std::cout << SIMTIME << "  blocked by pedestrian " + leader.first->getID() << "\n";
                }
#endif
            }
        }
    }

    if (leader.first != nullptr) {
        secureOrigFrontGap = vehicle->getCarFollowModel().getSecureGap(vehicle, leader.first, vehicle->getSpeed(), leader.first->getSpeed(), leader.first->getCarFollowModel().getMaxDecel());
    }

    MSAbstractLaneChangeModel::MSLCMessager msg(leader.first, neighLead.first, neighFollow.first);
    int state = blocked | vehicle->getLaneChangeModel().wantsChange(
                    laneOffset, msg, blocked, leader, follower, neighLead, neighFollow, *targetLane, preb, &(myCandi->lastBlocked), &(myCandi->firstBlocked));

    if (blocked == 0 && (state & LCA_WANTS_LANECHANGE) != 0 && neighLead.first != nullptr) {
        // do a more careful (but expensive) check to ensure that a
        // safety-critical leader is not being overlooked
        // while changing on an intersection, it is not sufficient to abort the
        // search with a leader on the current lane because all linkLeaders must
        // be considered as well
        const double seen = myCandi->lane->getLength() - vehicle->getPositionOnLane();
        const double speed = vehicle->getSpeed();
        const double dist = vehicle->getCarFollowModel().brakeGap(speed) + vehicle->getVehicleType().getMinGap();
        if (seen < dist || myCandi->lane->isInternal()) {
            std::pair<MSVehicle* const, double> neighLead2 = targetLane->getCriticalLeader(dist, seen, speed, *vehicle);
            if (neighLead2.first != nullptr && neighLead2.first != neighLead.first) {
                const double secureGap = vehicle->getCarFollowModel().getSecureGap(vehicle, neighLead2.first, vehicle->getSpeed(),
                                         neighLead2.first->getSpeed(), neighLead2.first->getCarFollowModel().getMaxDecel());
                const double secureGap2 = secureGap * vehicle->getLaneChangeModel().getSafetyFactor();
#ifdef DEBUG_SURROUNDING_VEHICLES
                if (DEBUG_COND) {
                    std::cout << SIMTIME << "   found critical leader=" << neighLead2.first->getID()
                              << " gap=" << neighLead2.second << " secGap=" << secureGap << " secGap2=" << secureGap2 << "\n";
                }
#endif
                if (neighLead2.second < secureGap2) {
                    state |= blockedByLeader;
                }
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
        // Ensure that a continuous lane change manoeuvre can be completed before the next turning movement.
        // Assume lateral position == 0. (If this should change in the future add + laneOffset*vehicle->getLateralPositionOnLane() to distToNeighLane)
        const double distToNeighLane = 0.5 * (vehicle->getLane()->getWidth() + targetLane->getWidth());
        // Extrapolate the LC duration if operating with speed dependent lateral speed.
        const MSAbstractLaneChangeModel& lcm = vehicle->getLaneChangeModel();
        const double assumedDecel = lcm.getAssumedDecelForLaneChangeDuration();
        const double estimatedLCDuration = lcm.estimateLCDuration(vehicle->getSpeed(), distToNeighLane, assumedDecel, (state & LCA_URGENT) != 0);
        if (estimatedLCDuration == -1) {
            // Can't guarantee that LC will succeed if vehicle is braking -> assert(lcm.myMaxSpeedLatStanding==0)
#ifdef DEBUG_CHECK_CHANGE
            if (DEBUG_COND) {
                std::cout << SIMTIME << " checkChange() too slow to guarantee completion of continuous lane change."
                          << "\nestimatedLCDuration=" << estimatedLCDuration
                          << "\ndistToNeighLane=" << distToNeighLane
                          << std::endl;
            }
#endif
            state |= LCA_INSUFFICIENT_SPEED;
        } else {
            // Compute covered distance, when braking for the whole lc duration
            const double decel = vehicle->getCarFollowModel().getMaxDecel() * estimatedLCDuration;
            const double avgSpeed = 0.5 * (
                                        MAX2(0., vehicle->getSpeed() - ACCEL2SPEED(vehicle->getCarFollowModel().getMaxDecel())) +
                                        MAX2(0., vehicle->getSpeed() - decel));
            // Distance required for lane change.
            const double space2change = avgSpeed * estimatedLCDuration;
            // Available distance for LC maneuver (distance till next turn)
            double seen = myCandi->lane->getLength() - vehicle->getPositionOnLane();
#ifdef DEBUG_CHECK_CHANGE
            if (DEBUG_COND) {
                std::cout << SIMTIME << " checkChange() checking continuous lane change..."
                          << "\ndistToNeighLane=" << distToNeighLane
                          << " estimatedLCDuration=" << estimatedLCDuration
                          << " space2change=" << space2change
                          << " avgSpeed=" << avgSpeed
                          << std::endl;
            }
#endif

            // for finding turns it doesn't matter whether we look along the current lane or the target lane
            const std::vector<MSLane*>& bestLaneConts = vehicle->getBestLanesContinuation();
            int view = 1;
            const MSLane* nextLane = vehicle->getLane();
            std::vector<MSLink*>::const_iterator link = MSLane::succLinkSec(*vehicle, view, *nextLane, bestLaneConts);
            while (!nextLane->isLinkEnd(link) && seen <= space2change) {
                if ((*link)->getDirection() == LinkDirection::LEFT || (*link)->getDirection() == LinkDirection::RIGHT
                        // the lanes after an internal junction are on different
                        // edges and do not allow lane-changing
                        || (nextLane->getEdge().isInternal() && (*link)->getViaLaneOrLane()->getEdge().isInternal())
                   ) {
                    state |= LCA_INSUFFICIENT_SPACE;
                    break;
                }
                if ((*link)->getViaLane() == nullptr) {
                    view++;
                }
                nextLane = (*link)->getViaLaneOrLane();
                seen += nextLane->getLength();
                // get the next link used
                link = MSLane::succLinkSec(*vehicle, view, *nextLane, bestLaneConts);
            }
#ifdef DEBUG_CHECK_CHANGE
            if (DEBUG_COND) {
                std::cout << " available distance=" << seen << std::endl;
            }
#endif
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
                const double speed = vehicle->getSpeed();
                seen = myCandi->lane->getLength() - vehicle->getPositionOnLane();
                nextLane = vehicle->getLane();
                view = 1;
                const double dist = vehicle->getCarFollowModel().brakeGap(speed) + vehicle->getVehicleType().getMinGap();
                std::vector<MSLink*>::const_iterator nextLink = MSLane::succLinkSec(*vehicle, view, *nextLane, bestLaneConts);
                while (!nextLane->isLinkEnd(nextLink) && seen <= space2change && seen <= dist) {
                    nextLane = (*nextLink)->getViaLaneOrLane();
                    const MSLane* const parallelLane = nextLane->getParallelLane(laneOffset);
                    if (parallelLane == nullptr) {
                        state |= LCA_INSUFFICIENT_SPACE;
                        break;
                    } else {
                        std::pair<MSVehicle* const, double> neighLead2 = parallelLane->getLeader(vehicle, -seen, std::vector<MSLane*>());
                        if (neighLead2.first != nullptr && neighLead2.first != neighLead.first
                                && (neighLead2.second < vehicle->getCarFollowModel().getSecureGap(vehicle, neighLead2.first,
                                        vehicle->getSpeed(), neighLead2.first->getSpeed(), neighLead2.first->getCarFollowModel().getMaxDecel()))) {
                            state |= blockedByLeader;
                            break;
                        }
                    }
                    if ((*nextLink)->getViaLane() == nullptr) {
                        view++;
                    }
                    seen += nextLane->getLength();
                    // get the next link used
                    nextLink = MSLane::succLinkSec(*vehicle, view, *nextLane, bestLaneConts);
                }
            }
        }
    }
    const int oldstate = state;
    // let TraCI influence the wish to change lanes and the security to take
    state = vehicle->influenceChangeDecision(state);
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
    vehicle->getLaneChangeModel().saveLCState(laneOffset, oldstate, state);
    if (blocked == 0 && (state & LCA_WANTS_LANECHANGE)) {
        // this lane change will be executed, save gaps
        vehicle->getLaneChangeModel().setFollowerGaps(neighFollow, secureBackGap);
        vehicle->getLaneChangeModel().setLeaderGaps(neighLead, secureFrontGap);
        vehicle->getLaneChangeModel().setOrigLeaderGaps(leader, secureOrigFrontGap);
    }
    if (laneOffset != 0) {
        vehicle->getLaneChangeModel().saveNeighbors(laneOffset, neighFollow, neighLead);
    }
    return state;
}

bool
MSLaneChanger::hasOppositeStop(MSVehicle* vehicle) {
    if (vehicle->hasStops()) {
        const MSStop& stop = vehicle->getNextStop();
        if (stop.isOpposite && vehicle->nextStopDist() < OPPOSITE_OVERTAKING_MAX_LOOKAHEAD) {
            return true;
        }
    }
    return false;
}


bool
MSLaneChanger::checkOppositeStop(MSVehicle* vehicle, const MSLane* oncomingLane, const MSLane* opposite, std::pair<MSVehicle*, double> leader) {
    const bool isOpposite = vehicle->getLaneChangeModel().isOpposite();
    double vMax = vehicle->getLane()->getVehicleMaxSpeed(vehicle);
    std::pair<MSVehicle*, double> neighLead(nullptr, -1);
    std::pair<MSVehicle*, double> oncoming(nullptr, -1);
    const std::vector<MSVehicle::LaneQ> preb = getBestLanesOpposite(vehicle, vehicle->getNextStop().lane, -1);
    const int laneIndex = vehicle->getLaneChangeModel().getNormalizedLaneIndex();
    const int bestOffset = preb[laneIndex].bestLaneOffset;
    //std::cout << SIMTIME << " veh=" << vehicle->getID() << " laneIndex=" << laneIndex << " bestOffset=" << bestOffset << " target=" << target->getID() << "\n";

    // compute safety constraints (assume vehicle is safe once stop is reached)
    const double spaceToStop = vehicle->nextStopDist();
    const double timeToStopForward = spaceToStop / MAX2(vehicle->getSpeed(), vehicle->getCarFollowModel().getMaxAccel());
    const double timeToStopLateral = (MSGlobals::gLaneChangeDuration > 0
                                      ? STEPS2TIME(MSGlobals::gLaneChangeDuration) * bestOffset
                                      : (MSGlobals::gLateralResolution > 0
                                         ? bestOffset * SUMO_const_laneWidth / vehicle->getVehicleType().getMaxSpeedLat()
                                         : 0.));
    const double timeToStop = MAX2(timeToStopForward, timeToStopLateral);
    if (!isOpposite) {
        // we keep neighLead distinct from oncoming because it determines blocking on the neigh lane
        // but also look for an oncoming leader to compute safety constraint
        const double searchDist = timeToStop * oncomingLane->getSpeedLimit() * 2 + spaceToStop;
        neighLead = oncomingLane->getOppositeLeader(vehicle, searchDist, true);
        oncoming = getOncomingVehicle(oncomingLane, neighLead, searchDist, vMax);
    } else {
        double searchDist = OPPOSITE_OVERTAKING_ONCOMING_LOOKAHEAD;
        oncoming = oncomingLane->getOppositeLeader(vehicle, searchDist, true);
        oncoming = getOncomingVehicle(oncomingLane, oncoming, searchDist, vMax);
    }
    double oncomingSpeed;
    const double surplusGap = computeSurplusGap(vehicle, opposite, oncoming, timeToStop, spaceToStop, oncomingSpeed);
    if (!isOpposite && surplusGap < 0) {
#ifdef DEBUG_CHANGE_OPPOSITE
        if (DEBUG_COND) {
            std::cout << "   cannot changeOppositeStop due to dangerous oncoming spaceToStop=" << spaceToStop
                      << " timeToStopForward=" << timeToStopForward << " timeToStopLateral=" << timeToStopLateral << " surplusGap=" << surplusGap << "\n";
        }
#endif
        return false;
    }

    if (bestOffset > 0) {
        MSLane* const target = preb[laneIndex + 1].lane;
        neighLead = target->getOppositeLeader(vehicle, OPPOSITE_OVERTAKING_MAX_LOOKAHEAD, true);
        std::pair<MSVehicle* const, double> neighFollow = target->getOppositeFollower(vehicle);
        return checkChangeOpposite(vehicle, 1, target, leader, neighLead, neighFollow, preb);
    } else {
        // return prematurely (due to foe?)
        //return checkChangeOpposite(vehicle, -1, target, leader, neighLead, neighFollow, preb);
        return false;
    }
}


std::vector<MSVehicle::LaneQ>
MSLaneChanger::getBestLanesOpposite(MSVehicle* vehicle, const MSLane* stopLane, double oppositeLength) {
    const bool isOpposite = vehicle->getLaneChangeModel().isOpposite();
    const MSEdge* forward = isOpposite ? vehicle->getLane()->getEdge().getOppositeEdge()->getNormalSuccessor() : vehicle->getLane()->getEdge().getNormalSuccessor();
    const MSEdge* opposite = forward->getOppositeEdge();
    const int numForward = forward->getNumLanes();
    const int numOpposite = opposite->getNumLanes();
    const std::vector<MSLane*>& oLanes = opposite->getLanes();
    std::vector<MSVehicle::LaneQ> preb = vehicle->getBestLanes();
    for (int i = 0; i < numOpposite; i++) {
        preb.push_back(preb.back());
        preb.back().lane = oLanes[numOpposite - 1 - i];
        preb.back().length = oppositeLength;
        if (isOpposite) {
            preb.back().bestLaneOffset = -1 - i;
            //std::cout << "  oi=" << i << " bestOffset =" << preb.back().bestLaneOffset << "\n";
        }
    }
    if (stopLane != nullptr) {
        const int stopIndex = numForward + numOpposite - stopLane->getIndex() - 1;
        for (int i = 0; i < (int)preb.size(); i++) {
            preb[i].bestLaneOffset = stopIndex - i;
            preb[i].length = vehicle->getLaneChangeModel().getForwardPos() + vehicle->nextStopDist();
            //std::cout << "  oi2=" << i << " stopIndex=" << stopIndex << " bestOffset =" << preb[i].bestLaneOffset << " stopDist=" << vehicle->nextStopDist() << " length=" << preb[i].length << "\n";
        }
    }
#ifdef DEBUG_CHANGE_OPPOSITE
    if (DEBUG_COND) {
        std::cout << SIMTIME << " getBestLanesOpposite " << vehicle->getID() << " isOpposite=" << isOpposite << "\n";
        for (int i = 0; i < (int)preb.size(); i++) {
            std::cout << "   i=" << i << " lane=" << preb[i].lane->getID() << " bestOffset=" << preb[i].bestLaneOffset << " length=" << preb[i].length << "\n";
        }
    }
#endif
    return preb;
}


bool
MSLaneChanger::changeOpposite(MSVehicle* vehicle, std::pair<MSVehicle*, double> leader, MSVehicle* lastStopped) {
    // Evaluate lane-changing between opposite direction lanes
    if (!myChangeToOpposite) {
        return false;
    }
    const bool isOpposite = vehicle->getLaneChangeModel().isOpposite();
    MSLane* source = vehicle->getMutableLane();
    MSLane* opposite = isOpposite ? source->getParallelLane(1) : source->getOpposite();

#ifdef DEBUG_CHANGE_OPPOSITE
    gDebugFlag5 = DEBUG_COND;
    if (DEBUG_COND) {
        std::cout << SIMTIME << " veh=" << vehicle->getID() << " considerChangeOpposite source=" << source->getID()
                  << " opposite=" << Named::getIDSecure(opposite) << " lead=" << Named::getIDSecure(leader.first) << " isOpposite=" << isOpposite << "\n";
    }
#endif
    //There is no lane for opposite driving
    if (opposite == nullptr) {
        return false;
    }
    if (vehicle->isStopped()) {
        // stopped vehicles obviously should not change lanes. Usually this is
        // prevent by appropriate bestLane distances
        return false;
    }
    int ret = 0;
    ret = vehicle->influenceChangeDecision(ret);
    bool oppositeChangeByTraci = false;
    // Check whether a lane change to the opposite direction was requested via TraCI
    if ((ret & (LCA_TRACI)) != 0) {
        if (isOpposite && (ret & LCA_LEFT) != 0) {
            // stay on the opposite side
            return false;
        }
        oppositeChangeByTraci = true;
    }
    if (!isOpposite && !oppositeChangeByTraci && !source->allowsChangingLeft(vehicle->getVClass())) {
        // lane changing explicitly forbidden from this lane
#ifdef DEBUG_CHANGE_OPPOSITE
        if (DEBUG_COND) {
            std::cout << "   not overtaking due to changeLeft restriction\n";
        }
#endif
        return false;
    }

    //lane for opposite driving is not permitted
    if (!opposite->allowsVehicleClass(vehicle->getVClass())) {
        return false;
    }

    const MSLane* oncomingLane = isOpposite ? source : opposite;
    //const MSLane* forwardLane = isOpposite ? opposite : source;
    // changing into the opposite direction is always to the left (XXX except for left-hand networkds)
    int direction = isOpposite ? -1 : 1;
    std::pair<MSVehicle*, double> neighLead(nullptr, -1);

    // distance that can safely be driven on the opposite side
    double surplusGap = std::numeric_limits<double>::max();

    // we need to find two vehicles:
    // 1) the leader that shall be overtaken (not necessarily the current leader but one of its leaders that has enough space in front)
    // 2) the oncoming vehicle (we need to look past vehicles that are currently overtaking through the opposite direction themselves)
    //
    // if the vehicle is driving normally, then the search for 1) starts on the current lane and 2) on the opposite lane
    // if the vehicle is driving on the opposite side then 1) is found on the neighboring lane and 2) on the current lane

    std::pair<MSVehicle*, double> overtaken(nullptr, -1);
    // oncoming vehicle that is driving in the "correct" direction
    std::pair<MSVehicle*, double> oncoming(nullptr, -1);
    // oncoming vehicle that is driving against the flow
    std::pair<MSVehicle*, double> oncomingOpposite(nullptr, -1);
    // the maximum speed while overtaking (may be lowered if slow vehicles are
    // currently overtaking ahead of vehicle)
    double vMax = vehicle->getLane()->getVehicleMaxSpeed(vehicle);
    double oncomingSpeed = oncomingLane->getSpeedLimit();

    // check for opposite direction stop
    if (!oppositeChangeByTraci && hasOppositeStop(vehicle)) {
        return checkOppositeStop(vehicle, oncomingLane, opposite, leader);
    }

    if (!isOpposite && leader.first == nullptr && !oppositeChangeByTraci) {
        // no reason to change unless there is a leader
        // or we are changing back to the propper direction
        // XXX also check whether the leader is so far away as to be irrelevant
        return false;
    }
    if (!isOpposite && !oppositeChangeByTraci
            && vehicle->getVClass() != SVC_EMERGENCY
            && leader.first != nullptr) {
        if (leader.first->signalSet(MSGlobals::gLefthand
                                    ? MSVehicle::VEH_SIGNAL_BLINKER_RIGHT : MSVehicle::VEH_SIGNAL_BLINKER_LEFT)) {
            // do not try to overtake a vehicle that is about to turn left or wants
            // to change left itself
#ifdef DEBUG_CHANGE_OPPOSITE
            if (DEBUG_COND) {
                std::cout << "   not overtaking leader " << leader.first->getID() << " that has blinker set\n";
            }
#endif
            if (lastStopped != nullptr && vehicle->getWaitingSeconds() >= OPPOSITE_OVERTAKING_DEADLOCK_WAIT) {
                neighLead = oncomingLane->getOppositeLeader(vehicle, OPPOSITE_OVERTAKING_ONCOMING_LOOKAHEAD, true, MSLane::MinorLinkMode::FOLLOW_ONCOMING);
                const double lastStoppedGap = lastStopped->getBackPositionOnLane() - vehicle->getPositionOnLane() - vehicle->getVehicleType().getMinGap();
                resolveDeadlock(vehicle, leader, neighLead, std::make_pair(lastStopped, lastStoppedGap));
            }
            return false;
        } else if (leader.second < 0) {
            // leaders is either a junction leader (that cannot be overtaken) or something else is wrong
#ifdef DEBUG_CHANGE_OPPOSITE
            if (DEBUG_COND) {
                std::cout << "   not overtaking leader " << leader.first->getID() << " with gap " << leader.second << "\n";
            }
#endif
            return false;
        }
    }

    // checks for overtaking space
    double timeToOvertake = std::numeric_limits<double>::max();
    double spaceToOvertake = std::numeric_limits<double>::max();
    double maxSpaceToOvertake = 0;

    if (oppositeChangeByTraci) {
        timeToOvertake = STEPS2TIME(vehicle->getInfluencer().getLaneTimeLineDuration());//todo discuss concept
        spaceToOvertake =  timeToOvertake * vehicle->getLane()->getVehicleMaxSpeed(vehicle);
    } else {
        if (isOpposite) {
            // -1 will use getMaximumBrakeDist() as look-ahead distance
            neighLead = opposite->getOppositeLeader(vehicle, -1, false);
            // make sure that overlapping vehicles on the neighboring lane are found by starting search at the back position
            overtaken = opposite->getLeader(vehicle, opposite->getOppositePos(vehicle->getBackPositionOnLane()), vehicle->getBestLanesContinuation(opposite));
            overtaken.second -= vehicle->getVehicleType().getLength();
            if (overtaken.first == nullptr && neighLead.first != nullptr) {
                overtaken = neighLead;
            }
            if (overtaken.first != nullptr) {
                overtaken = getColumnleader(maxSpaceToOvertake, vehicle, overtaken);
            }
#ifdef DEBUG_CHANGE_OPPOSITE
            if (DEBUG_COND) {
                std::cout << "    leaderOnSource=" << Named::getIDSecure(oncoming.first) << " gap=" << oncoming.second << "\n";
                std::cout << "    leaderOnTarget=" << Named::getIDSecure(neighLead.first) << " gap=" << neighLead.second << "\n";
                std::cout << "    overtaken=" << Named::getIDSecure(overtaken.first) << " gap=" << overtaken.second << "\n";
            }
#endif
        } else {
            overtaken = getColumnleader(maxSpaceToOvertake, vehicle, leader);
        }

        if (overtaken.first == 0) {
            if (!isOpposite) {
                // no reason to change to the opposite side
#ifdef DEBUG_CHANGE_OPPOSITE
                if (DEBUG_COND) {
                    std::cout << "   no leader found\n";
                }
#endif
                if (vehicle->getWaitingSeconds() >= OPPOSITE_OVERTAKING_DEADLOCK_WAIT) {
                    neighLead = oncomingLane->getOppositeLeader(vehicle, OPPOSITE_OVERTAKING_ONCOMING_LOOKAHEAD, true, MSLane::MinorLinkMode::FOLLOW_ONCOMING);
                    resolveDeadlock(vehicle, leader, neighLead, std::make_pair(nullptr, leader.second));
                }
                return false;
            }
        } else {
#ifdef DEBUG_CHANGE_OPPOSITE
            if (DEBUG_COND) {
                std::cout << "   compute time/space to overtake for columnLeader=" << overtaken.first->getID() << " egoGap=" << overtaken.second << "\n";
            }
#endif
            // if we have limited space to overtake, we may have to limit our maximum maneuver speed
            vMax = MIN2(vMax, getMaxOvertakingSpeed(vehicle, maxSpaceToOvertake));
            // there might be leader vehicles on the opposite side that also drive
            // against the flow which are slower than ego (must be factored into
            // overtaking time)
            computeOvertakingTime(vehicle, vMax, overtaken.first, overtaken.second, timeToOvertake, spaceToOvertake);
#ifdef DEBUG_CHANGE_OPPOSITE
            if (DEBUG_COND) {
                std::cout << SIMTIME
                          << " veh=" << vehicle->getID()
                          << " changeOpposite opposite=" << opposite->getID()
                          << " lead=" << Named::getIDSecure(leader.first)
                          << " maxSpaceToOvertake=" << maxSpaceToOvertake
                          << " vMax=" << vMax
                          << " timeToOvertake=" << timeToOvertake
                          << " spaceToOvertake=" << spaceToOvertake
                          << "\n";
            }
#endif

            if (!isOpposite && spaceToOvertake > OPPOSITE_OVERTAKING_MAX_SPACE_TO_OVERTAKE) {
#ifdef DEBUG_CHANGE_OPPOSITE
                if (DEBUG_COND) {
                    std::cout << "   cannot changeOpposite (cannot overtake fast leader " << Named::getIDSecure(overtaken.first) << " v=" << overtaken.first->getSpeed() << ")\n";
                }
#endif
                neighLead = oncomingLane->getOppositeLeader(vehicle, OPPOSITE_OVERTAKING_ONCOMING_LOOKAHEAD, true, MSLane::MinorLinkMode::FOLLOW_ONCOMING);
                bool wait = false;
                if (vehicle->getWaitingSeconds() >= OPPOSITE_OVERTAKING_DEADLOCK_WAIT) {
                    wait = resolveDeadlock(vehicle, leader, neighLead, overtaken);
                }
                if (!wait && lastStopped != nullptr) {
                    const double lastStoppedGap = lastStopped->getBackPositionOnLane() - vehicle->getPositionOnLane() - vehicle->getVehicleType().getMinGap();
#ifdef DEBUG_CHANGE_OPPOSITE
                    if (DEBUG_COND) {
                        std::cout << "  lastStopped=" << Named::getIDSecure(lastStopped) << " gap=" << lastStoppedGap << "\n";
                    }
#endif
                    avoidDeadlock(vehicle, neighLead, std::make_pair(lastStopped, lastStoppedGap), leader);
                }
                return false;
            }
        }

        // if we have a leader vehicle that is driving in the opposite
        // direction, it may slow us down (update vMax)
        if (!isOpposite) {
            assert(timeToOvertake != std::numeric_limits<double>::max());
            assert(spaceToOvertake != std::numeric_limits<double>::max());
            // we keep neighLead distinct from oncoming because it determines blocking on the neigh lane
            // but also look for an oncoming leader to compute safety constraint
            double searchDist = timeToOvertake * oncomingLane->getSpeedLimit() * 2 + spaceToOvertake;
            neighLead = oncomingLane->getOppositeLeader(vehicle, searchDist, true, MSLane::MinorLinkMode::FOLLOW_ONCOMING);
            oncoming = getOncomingVehicle(oncomingLane, neighLead, searchDist, vMax, overtaken.first, MSLane::MinorLinkMode::FOLLOW_ONCOMING);
            oncomingOpposite = getOncomingOppositeVehicle(vehicle, overtaken, searchDist);
        } else {
            double searchDist = OPPOSITE_OVERTAKING_ONCOMING_LOOKAHEAD;
            oncoming = oncomingLane->getOppositeLeader(vehicle, searchDist, true);
            oncoming = getOncomingVehicle(oncomingLane, oncoming, searchDist, vMax, overtaken.first);
            oncomingOpposite = getOncomingOppositeVehicle(vehicle, overtaken, searchDist);
        }
        if (oncoming.first != nullptr && (oncoming.first->isStopped()
                                          || oncoming.first->getWaitingSeconds() >= OPPOSITE_OVERTAKING_DEADLOCK_WAIT)) {
            // finish overtaking within the available space
            const double oncomingGap = oncoming.second - oncoming.first->getVehicleType().getMinGap();
            if (oncomingGap > 0) {
                vMax = MIN2(vMax, getMaxOvertakingSpeed(vehicle, oncomingGap));
            }
#ifdef DEBUG_CHANGE_OPPOSITE
            if (DEBUG_COND) {
                std::cout << "  oncoming=" << oncoming.first->getID() << " stopped=" << oncoming.first->isStopped()
                          << " halting=" << oncoming.first->getWaitingSeconds()
                          << " oncomingGap=" << oncomingGap
                          << " vMaxGap=" << getMaxOvertakingSpeed(vehicle, oncomingGap)
                          << " vMax=" << vMax << "\n";
            }
#endif
        }

        if (overtaken.first != nullptr && vMax != vehicle->getLane()->getVehicleMaxSpeed(vehicle)) {
            // recompute overtaking time with slow opposite leader
            computeOvertakingTime(vehicle, vMax, overtaken.first, overtaken.second, timeToOvertake, spaceToOvertake);
#ifdef DEBUG_CHANGE_OPPOSITE
            if (DEBUG_COND) {
                std::cout << "  recomputed overtaking time with vMax=" << vMax
                          << " timeToOvertake=" << timeToOvertake
                          << " spaceToOvertake=" << spaceToOvertake
                          << "\n";
            }
#endif
        }
        if (!isOpposite) {
            if (spaceToOvertake > OPPOSITE_OVERTAKING_MAX_SPACE_TO_OVERTAKE) {
#ifdef DEBUG_CHANGE_OPPOSITE
                if (DEBUG_COND) {
                    std::cout << "   cannot changeOpposite (check2: cannot overtake fast leader " << Named::getIDSecure(overtaken.first) << " v=" << overtaken.first->getSpeed() << ")\n";
                }
#endif
                resolveDeadlock(vehicle, leader, neighLead, overtaken);
                return false;
            }
            // check for upcoming stops
            if (vehicle->nextStopDist() < spaceToOvertake) {
#ifdef DEBUG_CHANGE_OPPOSITE
                if (DEBUG_COND) {
                    std::cout << "   cannot changeOpposite due to upcoming stop (dist=" << vehicle->nextStopDist() << " spaceToOvertake=" << spaceToOvertake << ")\n";
                }
#endif
                return false;
            }
            assert(timeToOvertake != std::numeric_limits<double>::max());
            assert(spaceToOvertake != std::numeric_limits<double>::max());
        }

        // check for dangerous oncoming leader
        surplusGap = computeSurplusGap(vehicle, opposite, oncoming, timeToOvertake, spaceToOvertake, oncomingSpeed);
        if (oncomingOpposite.first != nullptr) {
            double oncomingSpeed2;
            const double conservativeTime = ceil(timeToOvertake / TS) * TS;
            const double conservativeSpace = conservativeTime * vehicle->getLane()->getVehicleMaxSpeed(vehicle);
            const double surplusGap2 = computeSurplusGap(vehicle, opposite, oncomingOpposite, conservativeTime, conservativeSpace, oncomingSpeed2, true);
#ifdef DEBUG_CHANGE_OPPOSITE
            if (DEBUG_COND) {
                std::cout << "   oncomingOpposite=" << oncomingOpposite.first->getID() << " speed=" << oncomingSpeed2 << " gap=" << oncomingOpposite.second << " surplusGap2=" << surplusGap2 << "\n";
            }
#endif
            surplusGap = MIN2(surplusGap, surplusGap2);
            oncomingSpeed = MAX2(oncomingSpeed, oncomingSpeed2);
            if (!isOpposite && surplusGap >= 0 && oncoming.first != nullptr && oncoming.first->isStopped()
                    && oncomingOpposite.second > oncoming.second) {
                // even if ego can change back and forth sucessfully, we have to
                // make sure that the oncoming vehicle can also finsih it's lane
                // change in time
                const double oSpeed = MAX2(oncomingOpposite.first->getSpeed(), NUMERICAL_EPS);
                // conservative estimate
                const double closingSpeed = (vehicle->getLane()->getVehicleMaxSpeed(vehicle)
                                             + oncomingOpposite.first->getLane()->getVehicleMaxSpeed(oncomingOpposite.first));
                const double ooSTO = oncomingOpposite.second - oncoming.second + oncomingOpposite.first->getVehicleType().getLengthWithGap();
                double ooTTO = ooSTO / oSpeed;
                // round to multiples of step length (TS)
                ooTTO = ceil(ooTTO / TS) * TS;
                const double surplusGap3 = oncomingOpposite.second - ooTTO * closingSpeed;
#ifdef DEBUG_CHANGE_OPPOSITE
                if (DEBUG_COND) {
                    std::cout << "   oSpeed=" << oSpeed << " ooSTO=" << ooSTO << " ooTTO=" << ooTTO << " surplusGap3=" << surplusGap3 << "\n";
                }
#endif
                surplusGap = MIN2(surplusGap, surplusGap3);
            }
        }
        if (!isOpposite && surplusGap < 0) {
#ifdef DEBUG_CHANGE_OPPOSITE
            if (DEBUG_COND) {
                std::cout << "   cannot changeOpposite due to dangerous oncoming (surplusGap=" << surplusGap << ")\n";
            }
#endif

#ifdef DEBUG_CHANGE_OPPOSITE
            if (DEBUG_COND) {
                if (oncoming.first->getLaneChangeModel().isOpposite()) {
                    std::cout << SIMTIME << " ego=" << vehicle->getID() << " does not changeOpposite due to dangerous oncoming " << oncoming.first->getID() << "  (but the leader is also opposite)\n";
                }
            }
#endif
            avoidDeadlock(vehicle, neighLead, overtaken, leader);
            return false;
        }
    }
    // compute remaining space on the opposite side
    // 1. the part that remains on the current lane
    double usableDist = isOpposite ? vehicle->getPositionOnLane() : source->getLength() - vehicle->getPositionOnLane();

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
            if ((*it)->getOpposite() == nullptr || !(*it)->getOpposite()->allowsVehicleClass(vehicle->getVClass())) {
                // opposite lane ends
                break;
            }
            // do not overtake past a minor link or turn
            const MSLane* const prev = *(it - 1);
            if (prev != nullptr) {
                const MSLink* link = prev->getLinkTo(*it);
                if (link == nullptr || link->getState() == LINKSTATE_ZIPPER
                        || (link->getDirection() != LinkDirection::STRAIGHT && vehicle->getVehicleType().getVehicleClass() != SVC_EMERGENCY)
                        || (!link->havePriority()
                            // consider traci-influence
                            && (!vehicle->hasInfluencer() || vehicle->getInfluencer().getRespectJunctionPriority())
                            // consider junction model parameters
                            && ((!link->haveRed() && !link->haveYellow()) || !vehicle->ignoreRed(link, true)))) {
#ifdef DEBUG_CHANGE_OPPOSITE
                    if (DEBUG_COND) {
                        std::cout << "   stop lookahead at link=" << (link == 0 ? "NULL" : link->getViaLaneOrLane()->getID()) << " state=" << (link == 0 ? "?" : toString(link->getState())) << " ignoreRed=" << vehicle->ignoreRed(link, true) << "\n";
                    }
#endif
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
    if (!isOpposite && MSNet::getInstance()->hasElevation() && !overtaken.first->isStopped()) {
        // do not overtake before the top of a hill
        double searchDist = timeToOvertake * oncomingLane->getSpeedLimit() * 1.5 * vehicle->getLaneChangeModel().getOppositeSafetyFactor() + spaceToOvertake;
        int view = vehicle->getLane()->isInternal() ? 1 : 0;
        bool foundHill = vehicle->getSlope() > 0;
        if (foundHilltop(vehicle, foundHill, searchDist, vehicle->getBestLanesContinuation(), view, vehicle->getPositionOnLane(), vehicle->getPosition().z(), OPPOSITE_OVERTAKING_HILLTOP_THRESHOHOLD)) {
            return false;
        }
    }
#ifdef DEBUG_CHANGE_OPPOSITE
    if (DEBUG_COND) {
        std::cout << "   usableDist=" << usableDist << " spaceToOvertake=" << spaceToOvertake << " timeToOvertake=" << timeToOvertake << "\n";
    }
#endif
    // compute wish to change
    std::pair<MSVehicle* const, double> neighFollow = opposite->getOppositeFollower(vehicle);
    double oppositeLength = vehicle->getBestLanes().back().length;
    if (isOpposite) {
        const bool canOvertake = spaceToOvertake <= OPPOSITE_OVERTAKING_MAX_SPACE_TO_OVERTAKE;
        oppositeLength = computeSafeOppositeLength(vehicle, oppositeLength, source, usableDist, oncoming, vMax, oncomingSpeed, neighLead, overtaken, neighFollow, surplusGap, opposite, canOvertake);
        leader.first = nullptr;
        if (neighLead.first != nullptr && neighLead.first->getLaneChangeModel().isOpposite()) {
            // ignore oncoming vehicle on the target lane (it might even change back in this step)
            neighLead.first = nullptr;
        }
    } else {
        if (leader.first != nullptr && leader.first->getWaitingSeconds() >= OPPOSITE_OVERTAKING_DEADLOCK_WAIT
                && vehicle->getVehicleType().getVehicleClass() != SVC_EMERGENCY) {
#ifdef DEBUG_CHANGE_OPPOSITE
            if (DEBUG_COND) {
                std::cout << "   not changing to avoid deadlock\n";
            }
#endif
            return false;
        }
        if (neighLead.first != nullptr && neighLead.first->isStopped()) {
            // do not start overtaking if the opposite side has been waitin for longer
            if (yieldToOppositeWaiting(vehicle, neighLead.first, 10)) {
                return false;
            }
        }
        if (oncoming.first != nullptr && oncoming.first != neighLead.first && oncoming.first->isStopped()) {
            // only abort the current column of overtaking vehicles if the opposite side has been waiting long enough
            if (yieldToOppositeWaiting(vehicle, oncoming.first, 10, TIME2STEPS(60))) {
                return false;
            }
        }
    }
    std::vector<MSVehicle::LaneQ> preb = getBestLanesOpposite(vehicle, nullptr, oppositeLength);
    return checkChangeOpposite(vehicle, direction, opposite, leader, neighLead, neighFollow, preb);
}


bool
MSLaneChanger::avoidDeadlock(MSVehicle* vehicle,
                             std::pair<MSVehicle*, double> neighLead,
                             std::pair<MSVehicle*, double> overtaken,
                             std::pair<MSVehicle*, double> leader) {
    assert(!vehicle->getLaneChangeModel().isOpposite());
#ifdef DEBUG_CHANGE_OPPOSITE_DEADLOCK
    if (DEBUG_COND) {
        std::cout << SIMTIME << " veh=" << vehicle->getID() << " avoidDeadlock"
                  << " neighLead=" << Named::getIDSecure(neighLead.first)
                  << " overtaken=" << Named::getIDSecure(overtaken.first)
                  << " leader=" << Named::getIDSecure(leader.first)
                  << "\n";
    }
#endif
    if (leader.first == nullptr || neighLead.first == nullptr || overtaken.first == nullptr) {
        return false;
    } else if (!neighLead.first->isStopped()
               && vehicle->getWaitingSeconds() >= OPPOSITE_OVERTAKING_DEADLOCK_WAIT) {
        // possibly there is an oncoming vehicle before the stoppled leader that
        // could drive due to our yielding
        auto neighLeadFollow = neighLead.first->getFollower(overtaken.second);
        neighLead.second += neighLead.first->getVehicleType().getLengthWithGap() + neighLeadFollow.second;
        neighLead.first = const_cast<MSVehicle*>(neighLeadFollow.first);
#ifdef DEBUG_CHANGE_OPPOSITE_DEADLOCK
        if (DEBUG_COND) {
            std::cout << "  neighLead follower=" << Named::getIDSecure(neighLeadFollow.first) << "\n";
        }
#endif
        if (neighLead.first == nullptr) {
            return false;
        }
    }

    const bool yield = (yieldToDeadlockOncoming(vehicle, neighLead.first, overtaken.second)
                        || leader.first->getWaitingSeconds() >= OPPOSITE_OVERTAKING_DEADLOCK_WAIT);
    if (neighLead.first->isStopped()
            && (overtaken.first->isStopped()
                || leader.first->getLaneChangeModel().isOpposite()
                || yield)) {

        // estimate required gap
        double requiredGap = MAX2(vehicle->getVehicleType().getLengthWithGap(), neighLead.first->getVehicleType().getLengthWithGap());
        requiredGap = MAX2(requiredGap, overtaken.first->getVehicleType().getLengthWithGap());
        requiredGap = MAX2(requiredGap, leader.first->getVehicleType().getLengthWithGap());
        requiredGap += 1;
        const double distToStop = neighLead.second - requiredGap;

        // find the next non-stopped vehicle behind neighLead
        double neighStoppedBack = vehicle->getVehicleType().getMinGap();
        while (neighLead.first != nullptr && neighLead.first->isStopped()) {
            const double nextGap = neighLead.second + neighLead.first->getVehicleType().getLengthWithGap();
            if (neighStoppedBack + nextGap > overtaken.second) {
                break;
            }
            neighStoppedBack += nextGap;
            auto neighLeadFollow = neighLead.first->getFollower();
            neighLead.second = neighLeadFollow.second;
            neighLead.first = const_cast<MSVehicle*>(neighLeadFollow.first);
#ifdef DEBUG_CHANGE_OPPOSITE_DEADLOCK
            if (DEBUG_COND) {
                std::cout << " neighLeadFollower=" << Named::getIDSecure(neighLead.first) << "\n";
            }
#endif
            if (neighStoppedBack > overtaken.second) {
                break;
            }
        }

        const double leaderBGap = leader.first->getBrakeGap();
        const double leaderFGap = leader.first->getLane()->getLeader(leader.first, leader.first->getPositionOnLane(), vehicle->getBestLanesContinuation(), overtaken.second, true).second;
        const double extraGap = MAX2(leaderBGap, leaderFGap);
        const double gapWithEgo = leader.second + extraGap - neighStoppedBack - vehicle->getVehicleType().getLengthWithGap();
#ifdef DEBUG_CHANGE_OPPOSITE_DEADLOCK
        if (DEBUG_COND) {
            std::cout << SIMTIME << " veh=" << vehicle->getID() << " avoidDeadlock"
                      << " neighLeadGap=" << neighLead.second
                      << " leaderGap=" << leader.second
                      << " bGap=" << leaderBGap
                      << " fGap=" << leaderFGap
                      << " eGap=" << extraGap
                      << " neighStoppedBack=" << neighStoppedBack
                      << " neighStoppedBackPos=" << vehicle->getPositionOnLane() + neighStoppedBack
                      << " requiredGap=" << requiredGap
                      << " gapWithEgo=" << gapWithEgo
                      << " yield=" << yield
                      << "\n";
        }
#endif
        // vehicle must fit behind leader and still leave required gap
        if (leader.first->getLaneChangeModel().isOpposite() || yield || gapWithEgo < requiredGap) {
            const std::vector<MSVehicle::LaneQ>& preb = vehicle->getBestLanes();
            const double currentDist = preb[vehicle->getLane()->getIndex()].length;
            const double stopPos = vehicle->getPositionOnLane() + distToStop;
#ifdef DEBUG_CHANGE_OPPOSITE_DEADLOCK
            if (DEBUG_COND) {
                std::cout << "   currentDist=" << currentDist << " stopPos=" << stopPos << " lGap+eGap=" << leader.second + extraGap << " distToStop=" << distToStop << "\n";
            }
#endif
            if (leader.second + leaderBGap + leader.first->getLength() > distToStop) {
                const double blockerLength = currentDist - stopPos;
                const bool reserved = vehicle->getLaneChangeModel().saveBlockerLength(blockerLength, -1);
#ifdef DEBUG_CHANGE_OPPOSITE_DEADLOCK
                if (DEBUG_COND) {
                    std::cout << SIMTIME << " veh=" << vehicle->getID() << " avoidDeadlock"
                              << " blockerLength=" << blockerLength
                              << " reserved=" << reserved
                              << "\n";
                }
#endif
                return reserved;
            }
        }
    }
    return false;
}


bool
MSLaneChanger::yieldToDeadlockOncoming(const MSVehicle* vehicle, const MSVehicle* stoppedNeigh, double dist) {
    if (vehicle->getWaitingSeconds() >= OPPOSITE_OVERTAKING_DEADLOCK_WAIT && stoppedNeigh != nullptr) {
        // is there a vehicle waiting behind stoppedNeigh that is encouraged by
        // halting ego? Due to edge-ordering it might change-opposite this step
        // and not be visible as leader
        std::pair<const MSVehicle*, double> follower = stoppedNeigh->getFollower(dist);
        double followerGap = stoppedNeigh->getVehicleType().getLengthWithGap();
        while (follower.first != nullptr && followerGap < dist && follower.first->isStopped()) {
            followerGap += follower.second + follower.first->getVehicleType().getLengthWithGap();
            follower = follower.first->getFollower(dist);
        };
        if (follower.first != nullptr) {
            followerGap += follower.second;
        }
#ifdef DEBUG_CHANGE_OPPOSITE_DEADLOCK
        if (DEBUG_COND) {
            std::cout << SIMTIME << " veh=" << vehicle->getID() << " yieldToDeadlockOncoming"
                      << " dist=" << dist << " follower=" << Named::getIDSecure(follower.first) << " fGap=" << followerGap
                      << "\n";
        }
#endif
        return follower.first != nullptr && followerGap < dist && !follower.first->isStopped();
    }
    return false;
}


bool
MSLaneChanger::yieldToOppositeWaiting(const MSVehicle* vehicle, const MSVehicle* stoppedNeigh, double dist, SUMOTime deltaWait) {
    std::pair<const MSVehicle*, double> follower = stoppedNeigh->getFollower(dist);
    while (follower.first != nullptr && follower.second < dist && follower.first->isStopped()) {
        follower = follower.first->getFollower(dist);
    };
#ifdef DEBUG_CHANGE_OPPOSITE_DEADLOCK
    if (DEBUG_COND && follower.first != nullptr) {
        std::cout << SIMTIME << " yieldToOppositeWaiting veh=" << vehicle->getID() << " stoppedNeigh=" << stoppedNeigh->getID()
                  << " oncoming=" << follower.first->getID()
                  << " wait=" << follower.first->getWaitingSeconds()
                  << " vehWait=" << vehicle->getWaitingSeconds()
                  << " deltaWait=" << STEPS2TIME(deltaWait)
                  << "\n";
    }
#endif
    if (follower.first != nullptr && follower.second < dist && follower.first->getWaitingTime() > vehicle->getWaitingTime() + deltaWait) {
        return true;
    }
    return false;
}


bool
MSLaneChanger::resolveDeadlock(MSVehicle* vehicle,
                               std::pair<MSVehicle* const, double> leader,
                               std::pair<MSVehicle*, double> neighLead,
                               std::pair<MSVehicle*, double> overtaken) {
    const double deadLockZone = overtaken.second;
#ifdef DEBUG_CHANGE_OPPOSITE_DEADLOCK
    if (DEBUG_COND) {
        std::cout << SIMTIME << " veh=" << vehicle->getID() << " resolveDeadlock waiting=" << vehicle->getWaitingSeconds()
                  << " leader=" << Named::getIDSecure(leader.first)
                  << " gap=" << leader.second
                  << "\n";
    }
#endif
    if (vehicle->getWaitingSeconds() >= OPPOSITE_OVERTAKING_DEADLOCK_WAIT
            && leader.first != nullptr && leader.second > vehicle->getVehicleType().getLengthWithGap()) {
        // assume vehicle is halting to avoid deadlock (since there is enough
        // space to drive further)
        // keep halting as long as there is an oncoming vehicle
        std::pair<MSVehicle* const, double> oncomingOpposite = getOncomingOppositeVehicle(vehicle, std::make_pair(nullptr, -1), leader.second);
#ifdef DEBUG_CHANGE_OPPOSITE_DEADLOCK
        if (DEBUG_COND) {
            std::cout << SIMTIME << " veh=" << vehicle->getID() << " resolveDeadlock"
                      << " leader=" << leader.first->getID()
                      << " leaderGap=" << leader.second
                      << " neighLead=" << Named::getIDSecure(neighLead.first)
                      << " deadLockZone=" << deadLockZone
                      << "\n";
        }
#endif
        if (neighLead.first != nullptr && !neighLead.first->isStopped()) {
            // possibly there is an oncoming vehicle before the stoppled leader that
            // could drive due to our yielding
            auto neighLeadFollow = neighLead.first->getFollower(deadLockZone);
            neighLead.second += neighLead.first->getVehicleType().getLengthWithGap() + neighLeadFollow.second;
            neighLead.first = const_cast<MSVehicle*>(neighLeadFollow.first);
#ifdef DEBUG_CHANGE_OPPOSITE_DEADLOCK
            if (DEBUG_COND) {
                std::cout << "  neighLead follower=" << Named::getIDSecure(neighLeadFollow.first) << "\n";
            }
#endif
        }

        if (oncomingOpposite.first != nullptr ||
                (neighLead.first != nullptr && neighLead.first->isStopped()
                 && yieldToDeadlockOncoming(vehicle, neighLead.first, deadLockZone))) {
            const std::vector<MSVehicle::LaneQ>& preb = vehicle->getBestLanes();
            const double currentDist = preb[vehicle->getLane()->getIndex()].length;
            // mirror code in patchSpeed
            const double blockerLength = currentDist - vehicle->getPositionOnLane() - 1 - vehicle->getVehicleType().getMinGap() - NUMERICAL_EPS;
            const bool reserved = vehicle->getLaneChangeModel().saveBlockerLength(blockerLength, -1);
#ifdef DEBUG_CHANGE_OPPOSITE_DEADLOCK
            if (DEBUG_COND) {
                std::cout << SIMTIME << " veh=" << vehicle->getID() << " resolveDeadlock"
                          << " leader=" << leader.first->getID()
                          << " leaderGap=" << leader.second
                          << " oncoming=" << oncomingOpposite.first->getID()
                          << " currentDist=" << currentDist
                          << " blockerLength=" << blockerLength
                          << " reserved=" << reserved
                          << "\n";
            }
#else
            UNUSED_PARAMETER(reserved);
#endif
            return true;
        }
    }
    return false;
}


double
MSLaneChanger::computeSafeOppositeLength(MSVehicle* vehicle, double oppositeLength, const MSLane* source, double usableDist,
        std::pair<MSVehicle*, double> oncoming, double vMax, double oncomingSpeed,
        std::pair<MSVehicle*, double> neighLead,
        std::pair<MSVehicle*, double> overtaken,
        std::pair<MSVehicle*, double> neighFollow,
        double surplusGap, const MSLane* opposite,
        bool canOvertake) {
    // compute the remaining distance that can be driven on the opposite side
    // this value will put into oppositeLength of the opposite lanes
    // @note: length counts from the start of the current lane
    // @note: see MSLaneChangeModel::LC2013::_wantsChange @1092 (isOpposite()
    // position on the target lane
    const double forwardPos = source->getOppositePos(vehicle->getPositionOnLane());

    // consider usableDist (due to minor links or end of opposite lanes)
    oppositeLength = MIN2(oppositeLength, usableDist + forwardPos);
    // consider upcoming stops
    oppositeLength = MIN2(oppositeLength, vehicle->nextStopDist() + forwardPos);
#ifdef DEBUG_CHANGE_OPPOSITE
    if (DEBUG_COND) {
        std::cout << "   laneQLength=" << oppositeLength << " usableDist=" << usableDist << " forwardPos=" << forwardPos << " stopDist=" << vehicle->nextStopDist() << "\n";
    }
#endif
    // consider oncoming leaders
    const MSVehicle* oncomingVeh = oncoming.first;
    if (oncomingVeh != 0) {
        if (!oncomingVeh->getLaneChangeModel().isOpposite() && oncomingVeh->getLaneChangeModel().getShadowLane() != source) {
            const double egoSpeedFraction = MIN2(0.5, vMax / (vMax + oncomingSpeed));
            oppositeLength = MIN2(oppositeLength, forwardPos + oncoming.second * egoSpeedFraction);
#ifdef DEBUG_CHANGE_OPPOSITE
            if (DEBUG_COND) {
                std::cout << SIMTIME << " found oncoming leader=" << oncomingVeh->getID() << " gap=" << oncoming.second
                          << " egoSpeedFraction=" << egoSpeedFraction << " newDist=" << oppositeLength << "\n";
            }
#endif
        } else {
#ifdef DEBUG_CHANGE_OPPOSITE
            if (DEBUG_COND) {
                std::cout << SIMTIME << " opposite leader=" << oncomingVeh->getID() << " gap=" << oncoming.second << " is driving against the flow\n";
            }
#endif
        }
        if (neighLead.first != nullptr) {
            if (overtaken.first == nullptr) {
#ifdef DEBUG_CHANGE_OPPOSITE
                if (DEBUG_COND) {
                    std::cout << SIMTIME << " ego=" << vehicle->getID() << " did not find columnleader to overtake\n";
                }
#endif
            } else if (oncomingVeh != nullptr && oncomingVeh->isStopped()
                       && neighLead.second > 0
                       && neighFollow.second > 0
                       && yieldToOppositeWaiting(vehicle, oncomingVeh, 10, TIME2STEPS(60))) {
                // merge back into the forward lane
                oppositeLength = forwardPos + neighLead.second;
            } else {
                if (surplusGap > 0) {
                    // exaggerate remaining dist so that the vehicle continues
                    // overtaking (otherwise the lane change model might abort prematurely)
                    oppositeLength += 1000;
                } else {
                    // return from the opposite side ahead of the unpassable column (unless overlapping)
                    if (overtaken.second > 0) {
                        oppositeLength = MIN2(oppositeLength, forwardPos + overtaken.second);
                    }
                    // (don't set the distance so low as to imply emergency braking)
                    oppositeLength = MAX2(oppositeLength, forwardPos + vehicle->getCarFollowModel().brakeGap(vehicle->getSpeed()));
                }
#ifdef DEBUG_CHANGE_OPPOSITE
                if (DEBUG_COND) {
                    std::cout << SIMTIME << " ego=" << vehicle->getID() << " is overtaking " << overtaken.first->getID()
                              << " surplusGap=" << surplusGap
                              << " final laneQLength=" << oppositeLength
                              << "\n";
                }
#endif
            }
        }
    } else {
        if (overtaken.first == nullptr || !canOvertake) {
            // there is no reason to stay on the opposite side
            std::pair<MSVehicle* const, double> oppFollow = opposite->getOppositeFollower(vehicle);
            if (oppFollow.first == nullptr) {
                oppositeLength = forwardPos;
            } else {
                const double secureGap = oppFollow.first->getCarFollowModel().getSecureGap(
                                             oppFollow.first, vehicle, oppFollow.first->getSpeed(), vehicle->getSpeed(), vehicle->getCarFollowModel().getMaxDecel());
#ifdef DEBUG_CHANGE_OPPOSITE
                if (DEBUG_COND) {
                    std::cout << SIMTIME << " ego=" << vehicle->getID() << " neighFollow=" << oppFollow.first->getID() << " gap=" << oppFollow.second << " secureGap=" << secureGap << "\n";
                }
#endif
                if (oppFollow.second > secureGap) {
                    // back gap is safe for immidiate return
                    oppositeLength = forwardPos;
                }
            }
        }
    }
#ifdef DEBUG_CHANGE_OPPOSITE
    if (DEBUG_COND) {
        std::cout << SIMTIME << " veh=" << vehicle->getID() << " remaining dist=" << oppositeLength - forwardPos << " forwardPos=" << forwardPos << " oppositeLength=" << oppositeLength << "\n";
    }
#endif
    return oppositeLength;
}


std::pair<MSVehicle* const, double>
MSLaneChanger::getOncomingVehicle(const MSLane* opposite, std::pair<MSVehicle*, double> oncoming,
                                  double searchDist, double& vMax, const MSVehicle* overtaken, MSLane::MinorLinkMode mLinkMode) {
    double gap = oncoming.second;
    while (oncoming.first != nullptr && (oncoming.first->getLaneChangeModel().isOpposite() || oncoming.first->getLaneChangeModel().getShadowLane() == opposite)) {
        searchDist -= (oncoming.first->getVehicleType().getLengthWithGap() + MAX2(0.0, oncoming.second));
        // leader is itself overtaking through the opposite side. find real oncoming vehicle
        gap += oncoming.first->getVehicleType().getLengthWithGap();
        if (oncoming.first != overtaken) {
            vMax = MIN2(vMax, oncoming.first->getSpeed());
        } // else: might be the case if we are overtaking a vehicle that is stopped on the opposite side
#ifdef DEBUG_CHANGE_OPPOSITE
        if (gDebugFlag5) {
            std::cout << SIMTIME << " oncoming=" << oncoming.first->getID() << " isOpposite gap=" << oncoming.second
                      << " totalGap=" << gap << " searchDist=" << searchDist << " vMax=" << vMax << "\n";
        }
#endif
        if (searchDist < 0) {
            break;
        }
        // getOppositeLeader resets the search postion by ego length and may thus create cycles
        if (oncoming.first->getLaneChangeModel().getShadowLane() != opposite) {
            opposite = oncoming.first->getLane();
        }
        oncoming = opposite->getFollower(oncoming.first, oncoming.first->getPositionOnLane(opposite), searchDist, mLinkMode);
        if (oncoming.first != nullptr) {
            gap += oncoming.second + oncoming.first->getVehicleType().getLength();
#ifdef DEBUG_CHANGE_OPPOSITE
            if (gDebugFlag5) {
                std::cout << SIMTIME << " oncoming=" << oncoming.first->getID() << " gap=" << oncoming.second << " totalGap=" << gap << "\n";
            }
#endif
        }
    }
    oncoming.second = gap;
    return oncoming;
}


std::pair<MSVehicle* const, double>
MSLaneChanger::getOncomingOppositeVehicle(const MSVehicle* vehicle, std::pair<MSVehicle*, double> overtaken, double searchDist) {
    double gap = 0;
    const MSVehicle* front = nullptr;
    if (overtaken.first != nullptr) {
        gap += overtaken.second + overtaken.first->getVehicleType().getLengthWithGap();
        front = overtaken.first;
    } else {
        // only for special situations (i.e. traci-triggered)
        front = vehicle;
    }
    // we only need to look for the next leader: If it's driving in the forward
    // direction, it "protects" from oncoming opposite vehicles.
    // all leader vehicles on the current laneChanger edge are already moved into MSLane::myTmpVehicles
    const bool checkTmpVehicles = front->getLane() == vehicle->getLane();
    std::vector<MSLane*> conts = vehicle->getBestLanesContinuation();
    while (conts.size() > 0 && conts.front() != front->getLane()) {
        conts.erase(conts.begin());
    }
    std::pair<MSVehicle* const, double> oncoming = front->getLane()->getLeader(front, front->getPositionOnLane(), conts, searchDist, checkTmpVehicles);
    if (oncoming.first != nullptr) {
        const bool isOpposite = oncoming.first->getLaneChangeModel().isOpposite();
        const MSLane* shadowLane = oncoming.first->getLaneChangeModel().getShadowLane();
#ifdef DEBUG_CHANGE_OPPOSITE
        if (gDebugFlag5) {
            std::cout << SIMTIME
                      << " front=" << front->getID() << " searchDist=" << searchDist
                      << " oncomingOpposite=" << oncoming.first->getID()
                      << " gap=" << oncoming.second
                      << " isOpposite=" << isOpposite
                      << " shadowLane=" << Named::getIDSecure(shadowLane)
                      << "\n";
        }
#endif
        if (isOpposite && shadowLane != front->getLane()) {
            // distance was to back position (but the vehicle is oncoming)
            oncoming.second -= oncoming.first->getVehicleType().getLength();
            oncoming.second += gap;
            return oncoming;
        }
    }
    return std::make_pair(nullptr, -1);
}


double
MSLaneChanger::computeSurplusGap(const MSVehicle* vehicle, const MSLane* opposite, std::pair<MSVehicle*, double> oncoming,
                                 double timeToOvertake, double spaceToOvertake, double& oncomingSpeed, bool oncomingOpposite) {
    double surplusGap = std::numeric_limits<double>::max();
    const MSVehicle* oncomingVeh = oncoming.first;
    if (oncomingVeh != 0 && (oncomingOpposite
                             || (!oncomingVeh->getLaneChangeModel().isOpposite()
                                 && oncomingVeh->getLaneChangeModel().getShadowLane() != opposite))) {
        // conservative: assume that the oncoming vehicle accelerates to its maximum speed
        // unless it has been standing (then assume it is trying to let us pass
        // to avoid deadlock)
        oncomingSpeed = (oncomingVeh->isStopped() || oncomingVeh->getWaitingSeconds() >= OPPOSITE_OVERTAKING_DEADLOCK_WAIT
                         ? 0 : oncomingVeh->getLane()->getVehicleMaxSpeed(oncomingVeh));
        const double safetyGap = ((oncomingSpeed + vehicle->getLane()->getVehicleMaxSpeed(vehicle))
                                  * vehicle->getCarFollowModel().getHeadwayTime()
                                  * OPPOSITE_OVERTAKING_SAFETYGAP_HEADWAY_FACTOR);
        surplusGap = oncoming.second - spaceToOvertake - timeToOvertake * oncomingSpeed - safetyGap;
#ifdef DEBUG_CHANGE_OPPOSITE
        if (DEBUG_COND) {
            std::cout << SIMTIME
                      << " oncoming=" << oncomingVeh->getID()
                      << " oGap=" << oncoming.second
                      << " oSpeed=" << oncomingSpeed
                      << " sto=" << spaceToOvertake
                      << " tto=" << timeToOvertake
                      << " safetyGap=" << safetyGap
                      << " surplusGap=" << surplusGap
                      << "\n";
        }
#endif
    }
    return surplusGap;
}

bool
MSLaneChanger::foundHilltop(MSVehicle* vehicle, bool foundHill, double searchDist, const std::vector<MSLane*>& bestLanes, int view, double pos, double lastMax, double hilltopThreshold) {
    if (view >= (int)bestLanes.size()) {
        return false;
    }
    MSLane* lane = bestLanes[view];
    double laneDist = 0;
    const PositionVector& shape = lane->getShape();
    double lastZ = lastMax;
    for (int i = 1; i < (int)shape.size(); i++) {
        const double dist = lane->interpolateGeometryPosToLanePos(shape[i - 1].distanceTo(shape[i]));
        laneDist += dist;
        if (laneDist > pos) {
            const double z = shape[i].z();
            if (z > lastMax) {
                lastMax = z;
            }
            if (z > lastZ) {
                foundHill = true;
            }
            lastZ = z;
#ifdef DEBUG_CHANGE_OPPOSITE
            if (DEBUG_COND) {
                std::cout << SIMTIME << "   foundHill=" << foundHill << " searchDist=" << searchDist << " lastMax=" << lastMax << " lane=" << lane->getID() << " laneDist=" << laneDist << " z=" << z << "\n";
            }
#endif
            if (foundHill && z < lastMax) {
                const double drop = lastMax - z;
                //std::cout << SIMTIME << "   searchDist=" << searchDist << " hillDrop=" << drop << " lastMax=" << lastMax << " lane=" << lane->getID() << " laneDist=" << laneDist << " z=" << z << "\n";
                if (drop > hilltopThreshold) {
#ifdef DEBUG_CHANGE_OPPOSITE
                    if (DEBUG_COND) {
                        std::cout << "   cannot changeOpposite before the top of a hill searchDist=" << searchDist << " hillDrop=" << drop
                                  << " lastMax=" << lastMax << " lane=" << lane->getID() << " laneDist=" << laneDist << " z=" << z << "\n";
                    }
#endif
                    return true;
                }
            }
            if (pos == 0) {
                searchDist -= dist;
            } else {
                searchDist -= laneDist - pos;
                pos = 0;
            }
            if (searchDist <= 0) {
                return false;
            }
        }
    }
    return foundHilltop(vehicle, foundHill, searchDist, bestLanes, view + 1, 0, lastMax, hilltopThreshold);
}


bool
MSLaneChanger::checkChangeOpposite(
    MSVehicle* vehicle,
    int laneOffset,
    MSLane* targetLane,
    const std::pair<MSVehicle* const, double>& leader,
    const std::pair<MSVehicle* const, double>& neighLead,
    const std::pair<MSVehicle* const, double>& neighFollow,
    const std::vector<MSVehicle::LaneQ>& preb) {
    const bool isOpposite = vehicle->getLaneChangeModel().isOpposite();
    MSLane* source = vehicle->getMutableLane();
    const std::pair<MSVehicle* const, double> follower(nullptr, -1);
    int state = checkChange(laneOffset, targetLane, leader, follower, neighLead, neighFollow, preb);
    vehicle->getLaneChangeModel().setOwnState(state);
    bool changingAllowed = (state & LCA_BLOCKED) == 0;
    // change if the vehicle wants to and is allowed to change
    if ((state & LCA_WANTS_LANECHANGE) != 0 && changingAllowed
            // do not change to the opposite direction for cooperative reasons
            && (isOpposite || (state & LCA_COOPERATIVE) == 0)) {
        const bool continuous = vehicle->getLaneChangeModel().startLaneChangeManeuver(source, targetLane, laneOffset);
#ifdef DEBUG_CHANGE_OPPOSITE
        if (DEBUG_COND) {
            std::cout << SIMTIME << " changing to opposite veh=" << vehicle->getID() << " dir=" << laneOffset << " opposite=" << Named::getIDSecure(targetLane)
                      << " state=" << toString((LaneChangeAction)state) << "\n";
        }
#endif
        if (continuous) {
            continueChange(vehicle, myCandi);
        }
        return true;
    }
#ifdef DEBUG_CHANGE_OPPOSITE
    if (DEBUG_COND) {
        std::cout << SIMTIME << " not changing to opposite veh=" << vehicle->getID() << " dir=" << laneOffset
                  << " opposite=" << Named::getIDSecure(targetLane) << " state=" << toString((LaneChangeAction)state) << "\n";
    }
#endif
    return false;
}


void
MSLaneChanger::computeOvertakingTime(const MSVehicle* vehicle, double vMax, const MSVehicle* leader, double gap, double& timeToOvertake, double& spaceToOvertake) {
    // Assumptions:
    // - leader maintains the current speed
    // - vehicle merges with maxSpeed ahead of leader
    // XXX affected by ticket #860 (the formula is invalid for the current position update rule)

    // first compute these values for the case where vehicle is accelerating
    // without upper bound on speed
    const double v = vehicle->getSpeed();
    const double u = leader->getAcceleration() > 0 ? leader->getLane()->getVehicleMaxSpeed(leader) : leader->getSpeed();
    const double a = vehicle->getCarFollowModel().getMaxAccel();
    const double d = vehicle->getCarFollowModel().getMaxDecel();
    const double g = MAX2(0.0, (
                              // drive up to the rear of leader
                              gap + vehicle->getVehicleType().getMinGap()
                              // drive head-to-head with the leader
                              + leader->getVehicleType().getLengthWithGap()
                              // drive past the leader
                              + vehicle->getVehicleType().getLength()
                              // allow for safe gap between leader and vehicle
                              + leader->getCarFollowModel().getSecureGap(leader, vehicle, u, vMax, d))
                          // time to move between lanes
                          + (MSGlobals::gSublane ? vMax * vehicle->getLane()->getWidth() / vehicle->getVehicleType().getMaxSpeedLat() : 0));
    const double sign = -1; // XXX recheck
    // v*t + t*t*a*0.5 = g + u*t
    // solve t
    // t = ((u - v - (((((2.0*(u - v))**2.0) + (8.0*a*g))**(1.0/2.0))*sign/2.0))/a)
    double t = (u - v - sqrt(4 * (u - v) * (u - v) + 8 * a * g) * sign * 0.5) / a;
#ifdef DEBUG_CHANGE_OPPOSITE_OVERTAKINGTIME
    if (DEBUG_COND) {
        std::cout << " computeOvertakingTime v=" << v << " vMax=" << vMax << " u=" << u << " a=" << a << " d=" << d << " gap=" << gap << " g=" << g << " t=" << t
                  << " distEgo=" << v* t + t* t* a * 0.5 << " distLead=" << g + u* t
                  << "\n";
    }
#endif
    assert(t >= 0);
    if (vMax <= u) {
        // do not try to overtake faster leader
        timeToOvertake = std::numeric_limits<double>::max();
        spaceToOvertake = std::numeric_limits<double>::max();
        return;
    }

    // allow for a safety time gap
    t += OPPOSITE_OVERTAKING_SAFE_TIMEGAP;
    // round to multiples of step length (TS)
    if (u > 0) {
        t = ceil(t / TS) * TS;
    }

    /// XXX ignore speed limit when overtaking through the opposite lane?
    const double timeToMaxSpeed = (vMax - v) / a;

#ifdef DEBUG_CHANGE_OPPOSITE_OVERTAKINGTIME
    if (DEBUG_COND) {
        std::cout << "   t=" << t << "  tvMax=" << timeToMaxSpeed << "\n";
    }
#endif
    if (t <= timeToMaxSpeed) {
        timeToOvertake = t;
        spaceToOvertake = v * t + t * t * a * 0.5;
#ifdef DEBUG_CHANGE_OPPOSITE_OVERTAKINGTIME
        if (DEBUG_COND) {
            std::cout << "    sto=" << spaceToOvertake << "\n";
        }
#endif
    } else {
        // space until max speed is reached
        const double s = v * timeToMaxSpeed + timeToMaxSpeed * timeToMaxSpeed * a * 0.5;
        const double m = timeToMaxSpeed;
        // s + (t-m) * vMax = g + u*t
        // solve t
        t = (g - s + m * vMax) / (vMax - u);
        if (t < 0) {
            // cannot overtake in time
#ifdef DEBUG_CHANGE_OPPOSITE_OVERTAKINGTIME
            if (DEBUG_COND) {
                std::cout << "     t2=" << t << "\n";
            }
#endif
            timeToOvertake = std::numeric_limits<double>::max();
            spaceToOvertake = std::numeric_limits<double>::max();
            return;
        } else {
            // allow for a safety time gap
            t += OPPOSITE_OVERTAKING_SAFE_TIMEGAP;
            // round to multiples of step length (TS)
            if (u > 0) {
                t = ceil(t / TS) * TS;
            }
            timeToOvertake = t;
            spaceToOvertake = s + (t - m) * vMax;
#ifdef DEBUG_CHANGE_OPPOSITE_OVERTAKINGTIME
            if (DEBUG_COND) {
                std::cout << "     t2=" << t << " s=" << s << " sto=" << spaceToOvertake << " m=" << m << "\n";
            }
#endif
        }
    }
    const double safetyFactor = OPPOSITE_OVERTAKING_SAFETY_FACTOR * vehicle->getLaneChangeModel().getOppositeSafetyFactor();
    timeToOvertake *= safetyFactor;
    if (STEPS2TIME(leader->getStopDuration()) < timeToOvertake) {
        spaceToOvertake *= safetyFactor;
    }
    double frac = fmod(timeToOvertake, TS);
    if (frac > 0) {
        // round to full sim step
        timeToOvertake += TS - frac;
    }
#ifdef DEBUG_CHANGE_OPPOSITE_OVERTAKINGTIME
    if (DEBUG_COND) {
        if (safetyFactor != 1) {
            std::cout << "    applying safetyFactor=" << safetyFactor
                      << " leaderStopTime=" << STEPS2TIME(leader->getStopDuration())
                      << " tto=" << timeToOvertake << " sto=" << spaceToOvertake << "\n";
        }
    }
#endif
}



std::pair<MSVehicle*, double>
MSLaneChanger::getColumnleader(double& maxSpace, MSVehicle* vehicle, std::pair<MSVehicle*, double> leader, double maxLookAhead) {
    assert(leader.first != 0);
    const MSLane* source = vehicle->getLane();
    // find a leader vehicle with sufficient space ahead for merging back
    const double overtakingSpeed = source->getVehicleMaxSpeed(vehicle); // just a guess
    const double mergeBrakeGap = vehicle->getCarFollowModel().brakeGap(overtakingSpeed);
    std::pair<MSVehicle*, double> columnLeader = leader;
    double egoGap = leader.second;
    bool foundSpaceAhead = false;
    double seen = leader.second + leader.first->getVehicleType().getLengthWithGap();
    std::vector<MSLane*> conts = vehicle->getBestLanesContinuation();
    if (maxLookAhead == std::numeric_limits<double>::max()) {
        maxLookAhead = (vehicle->getVehicleType().getVehicleClass() == SVC_EMERGENCY
                        ? OPPOSITE_OVERTAKING_MAX_LOOKAHEAD_EMERGENCY
                        : OPPOSITE_OVERTAKING_MAX_LOOKAHEAD);
        maxLookAhead = MAX2(maxLookAhead, mergeBrakeGap + 10
                            + vehicle->getVehicleType().getLengthWithGap()
                            + leader.first->getVehicleType().getLengthWithGap());
    }
#ifdef DEBUG_CHANGE_OPPOSITE
    if (DEBUG_COND) {
        std::cout << " getColumnleader vehicle=" << vehicle->getID() << " leader=" << leader.first->getID() << " gap=" << leader.second << " maxLookAhead=" << maxLookAhead << "\n";
    }
#endif
    const double safetyFactor = OPPOSITE_OVERTAKING_SAFETY_FACTOR * vehicle->getLaneChangeModel().getOppositeSafetyFactor();
    while (!foundSpaceAhead) {
        const double requiredSpaceAfterLeader = (columnLeader.first->getCarFollowModel().getSecureGap(
                columnLeader.first, vehicle,
                columnLeader.first->getSpeed(), overtakingSpeed, vehicle->getCarFollowModel().getMaxDecel())
                                                + columnLeader.first->getVehicleType().getMinGap()
                                                + vehicle->getVehicleType().getLengthWithGap());


        // all leader vehicles on the current laneChanger edge are already moved into MSLane::myTmpVehicles
        const bool checkTmpVehicles = (&columnLeader.first->getLane()->getEdge() == &source->getEdge());
        double searchStart = columnLeader.first->getPositionOnLane();
        std::pair<MSVehicle*, double> leadLead = columnLeader.first->getLane()->getLeader(
                    columnLeader.first, searchStart, conts, requiredSpaceAfterLeader + mergeBrakeGap,
                    checkTmpVehicles);
        std::set<MSVehicle*> seenLeaders;
        while (leadLead.first != nullptr && leadLead.first->getLaneChangeModel().isOpposite()) {
#ifdef DEBUG_CHANGE_OPPOSITE
            if (DEBUG_COND) {
                std::cout << "   skipping opposite leadLead=" << leadLead.first->getID() << " gap=" << leadLead.second << "\n";
            }
#endif
            if (leadLead.second + seen > maxLookAhead || seenLeaders.count(leadLead.first) > 0) {
                leadLead.first = nullptr;
                break;
            }
            seenLeaders.insert(leadLead.first);
            // found via shadow lane, skip it
            const double searchStart2 = searchStart + MAX2(0.0, leadLead.second) + leadLead.first->getVehicleType().getLengthWithGap();
            leadLead = columnLeader.first->getLane()->getLeader(
                           columnLeader.first, searchStart2, conts, requiredSpaceAfterLeader + mergeBrakeGap,
                           checkTmpVehicles);
            leadLead.second += (searchStart2 - searchStart);
        }
        if (leadLead.first == nullptr) {
            double availableSpace = columnLeader.first->getLane()->getLength() - columnLeader.first->getPositionOnLane();
            double requiredSpace = safetyFactor * requiredSpaceAfterLeader;
            if (!columnLeader.first->isStopped()) {
                // if the leader is stopped we can trade space for time
                requiredSpace += safetyFactor * mergeBrakeGap;
            }
#ifdef DEBUG_CHANGE_OPPOSITE
            if (DEBUG_COND) {
                std::cout << "   no direct leader found after columnLeader " << columnLeader.first->getID()
                          << " availableSpace=" << availableSpace
                          << " reqAfterLeader=" << requiredSpaceAfterLeader
                          << " ovSpeed=" << overtakingSpeed
                          << " reqBGap=" << mergeBrakeGap
                          << " reqMin=" << requiredSpace / safetyFactor
                          << " req=" << requiredSpace
                          << "\n";
            }
#endif
            if (availableSpace > requiredSpace) {
                foundSpaceAhead = true;
            } else {
                // maybe the columnleader is stopped before a junction or takes a different turn.
                // try to find another columnleader on successive lanes
                const bool allowMinor = vehicle->getVehicleType().getVehicleClass() == SVC_EMERGENCY;
                bool contsEnd = false;
                const MSLane* next = getLaneAfter(columnLeader.first->getLane(), conts, allowMinor, contsEnd);
#ifdef DEBUG_CHANGE_OPPOSITE
                if (DEBUG_COND) {
                    std::cout << "   look for another leader on lane " << Named::getIDSecure(next) << "\n";
                }
#endif
                while (next != nullptr && seen < maxLookAhead) {
                    seen += next->getLength();
                    MSVehicle* cand = next->getLastAnyVehicle();
                    if (cand == nullptr) {
                        availableSpace += next->getLength();
                        if (availableSpace > requiredSpace) {
                            foundSpaceAhead = true;
                            break;
                        }
                        next = getLaneAfter(next, conts, allowMinor, contsEnd);
                    } else {
                        availableSpace += cand->getBackPositionOnLane();
                        if (availableSpace > requiredSpace) {
                            foundSpaceAhead = true;
                            break;
                        } else {
                            return getColumnleader(maxSpace, vehicle, std::make_pair(cand, availableSpace + cand->getPositionOnLane()), maxLookAhead - seen);
                        }
                    }
                }
#ifdef DEBUG_CHANGE_OPPOSITE
                if (DEBUG_COND) {
                    std::cout << "      foundSpaceAhead=" << foundSpaceAhead << " availableSpace=" << availableSpace << " next=" << Named::getIDSecure(next) << " contsEnd=" << contsEnd << " conts=" << toString(conts) << "\n";
                }
#endif
                if (!foundSpaceAhead && contsEnd) {
                    foundSpaceAhead = true;
                    availableSpace = requiredSpace;
                }
                if (!foundSpaceAhead) {
                    return std::make_pair(nullptr, -1);
                }
            }
            maxSpace = egoGap + columnLeader.first->getVehicleType().getLength() + availableSpace;
        } else {
            const double sGap = vehicle->getCarFollowModel().getSecureGap(vehicle, leadLead.first,
                                overtakingSpeed, leadLead.first->getSpeed(), leadLead.first->getCarFollowModel().getMaxDecel());
            double requiredSpace = safetyFactor * requiredSpaceAfterLeader;
            if (!columnLeader.first->isStopped()) {
                // if the leader is stopped we can trade space for time
                requiredSpace += safetyFactor * sGap;
            }
#ifdef DEBUG_CHANGE_OPPOSITE
            if (DEBUG_COND) {
                std::cout << "   leader's leader " << leadLead.first->getID() << " space=" << leadLead.second
                          << " reqAfterLeader=" << requiredSpaceAfterLeader
                          << " ovSpeed=" << overtakingSpeed
                          << " reqSGap=" << sGap
                          << " reqMin=" << requiredSpace / safetyFactor
                          << " req=" << requiredSpace
                          << "\n";
            }
#endif
            if (leadLead.second > requiredSpace) {
                foundSpaceAhead = true;
                maxSpace = egoGap + columnLeader.first->getVehicleType().getLength() + leadLead.second;
            } else {

                if (leadLead.second < 0) {
                    // must be a junction leader or some other dangerous situation
#ifdef DEBUG_CHANGE_OPPOSITE
                    if (DEBUG_COND) {
                        std::cout << "   leader's leader " << leadLead.first->getID() << " gap=" << leadLead.second << " is junction leader (aborting)\n";
                    }
#endif
                    return std::make_pair(nullptr, -1);
                }

#ifdef DEBUG_CHANGE_OPPOSITE
                if (DEBUG_COND) {
                    std::cout << "   not enough space after columnLeader=" << columnLeader.first->getID() << " required=" << requiredSpace << "\n";
                }
#endif
                seen += MAX2(0., leadLead.second) + leadLead.first->getVehicleType().getLengthWithGap();
                if (seen > maxLookAhead) {
#ifdef DEBUG_CHANGE_OPPOSITE
                    if (DEBUG_COND) {
                        std::cout << "   cannot changeOpposite due to insufficient free space after columnLeader (seen=" << seen << " columnLeader=" << columnLeader.first->getID() << ")\n";
                    }
#endif
                    return std::make_pair(nullptr, -1);
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
    columnLeader.second = egoGap;
    return columnLeader;
}


const MSLane*
MSLaneChanger::getLaneAfter(const MSLane* lane, const std::vector<MSLane*>& conts, bool allowMinor, bool& contsEnd) {
    for (auto it = conts.begin(); it != conts.end(); ++it) {
        if (*it == lane) {
            if (it + 1 != conts.end()) {
                // abort on minor link
                const MSLane* next = *(it + 1);
                const MSLink* link = lane->getLinkTo(next);
                if (link == nullptr || (!allowMinor && !link->havePriority())) {
                    return nullptr;
                }
                return next;
            } else {
                contsEnd = true;
                return nullptr;
            }
        }
    }
    return nullptr;
}

double
MSLaneChanger::getMaxOvertakingSpeed(const MSVehicle* vehicle, double maxSpaceToOvertake) {
    // v*v/2*a + v*v/2*d = maxSpaceToOvertake
    const double a = vehicle->getCarFollowModel().getMaxAccel();
    const double d = vehicle->getCarFollowModel().getMaxDecel();
    const double v = sqrt(2 * maxSpaceToOvertake * a * d / (a + d));
    return v;
}

/****************************************************************************/
