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
/// @file    MSLaneChangerSublane.cpp
/// @author  Jakob Erdmann
/// @author  Leonhard Luecken
/// @date    Oct 2015
///
// Performs sub-lane changing of vehicles
/****************************************************************************/
#include <config.h>

#include "MSLaneChangerSublane.h"
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
#include <utils/geom/GeomHelper.h>


// ===========================================================================
// DEBUG constants
// ===========================================================================
#define DEBUG_COND (vehicle->getLaneChangeModel().debugVehicle())
//#define DEBUG_COND (vehicle->getID() == "disabled")
//#define DEBUG_COND true
//#define DEBUG_DECISION
//#define DEBUG_ACTIONSTEPS
//#define DEBUG_STATE
//#define DEBUG_MANEUVER
//#define DEBUG_SURROUNDING
//#define DEBUG_CHANGE_OPPOSITE

// ===========================================================================
// member method definitions
// ===========================================================================
MSLaneChangerSublane::MSLaneChangerSublane(const std::vector<MSLane*>* lanes, bool allowChanging) :
    MSLaneChanger(lanes, allowChanging) {
    // initialize siblings
    if (myChanger.front().lane->isInternal()) {
        for (ChangerIt ce = myChanger.begin(); ce != myChanger.end(); ++ce) {
            for (ChangerIt ce2 = myChanger.begin(); ce2 != myChanger.end(); ++ce2) {
                if (ce != ce2 && ce->lane->getIncomingLanes().front().lane == ce2->lane->getIncomingLanes().front().lane) {
                    //std::cout << "addSibling lane=" << ce->lane->getID() << " offset=" << ce2->lane->getIndex() - ce->lane->getIndex() << "\n";
                    ce->siblings.push_back(ce2->lane->getIndex() - ce->lane->getIndex());
                }
            }
        }
    }
}


MSLaneChangerSublane::~MSLaneChangerSublane() {}

void
MSLaneChangerSublane::initChanger() {
    MSLaneChanger::initChanger();
    // Prepare myChanger with a safe state.
    for (ChangerIt ce = myChanger.begin(); ce != myChanger.end(); ++ce) {
        ce->ahead = ce->lane->getPartialBeyond();
        ce->outsideBounds.clear();
//        std::cout << SIMTIME << " initChanger lane=" << ce->lane->getID() << " vehicles=" << toString(ce->lane->myVehicles) << "\n";
//        std::cout << SIMTIME << " initChanger lane=" << ce->lane->getID() << " partial vehicles=" << toString(ce->lane->myPartialVehicles) << "\n";
//        std::cout << SIMTIME << " initChanger lane=" << ce->lane->getID() << " partial vehicles beyond=" << toString(ce->ahead.toString()) << "\n";
    }
}



void
MSLaneChangerSublane::updateChanger(bool vehHasChanged) {
    MSLaneChanger::updateChanger(vehHasChanged);
    if (!vehHasChanged) {
        MSVehicle* lead = myCandi->lead;
        //std::cout << SIMTIME << " updateChanger lane=" << myCandi->lane->getID() << " lead=" << Named::getIDSecure(lead) << " lsol=" << lead->getLeftSideOnLane() << " rsol=" << lead->getRightSideOnLane() << "\n";
        if (lead->getLeftSideOnLane() < 0 || lead->getRightSideOnLane() > myCandi->lane->getWidth()) {
            myCandi->outsideBounds.push_back(lead);
        } else {
            myCandi->ahead.addLeader(lead, false, 0);
        }
        MSLane* shadowLane = lead->getLaneChangeModel().getShadowLane();
        if (shadowLane != nullptr && &shadowLane->getEdge() == &lead->getLane()->getEdge()) {
            assert(shadowLane->getIndex() < (int)myChanger.size());
            const double latOffset = lead->getLane()->getRightSideOnEdge() - shadowLane->getRightSideOnEdge();
            //std::cout << SIMTIME << " updateChanger shadowLane=" << shadowLane->getID() << " lead=" << Named::getIDSecure(lead) << "\n";
            (myChanger.begin() + shadowLane->getIndex())->ahead.addLeader(lead, false, latOffset);
        }
    }
    //std::cout << SIMTIME << " updateChanger: lane=" << myCandi->lane->getID() << " lead=" << Named::getIDSecure(myCandi->lead) << " ahead=" << myCandi->ahead.toString() << " vehHasChanged=" << vehHasChanged << "\n";
    //for (ChangerIt ce = myChanger.begin(); ce != myChanger.end(); ++ce) {
    //    std::cout << " lane=" << ce->lane->getID() << " vehicles=" << toString(ce->lane->myVehicles) << "\n";
    //}
}


bool
MSLaneChangerSublane::change() {
    // variant of change() for the sublane case
    myCandi = findCandidate();
    MSVehicle* vehicle = veh(myCandi);
    vehicle->getLaneChangeModel().clearNeighbors();
#ifdef DEBUG_ACTIONSTEPS
    if (DEBUG_COND) {
        std::cout << "\n" << SIMTIME << " CHANGE veh=" << vehicle->getID() << " lane=" << vehicle->getLane()->getID() << "\n";
    }
#endif
    assert(vehicle->getLane() == (*myCandi).lane);
    assert(!vehicle->getLaneChangeModel().isChangingLanes());
    if (/*!myAllowsChanging*/ vehicle->getLaneChangeModel().alreadyChanged() || vehicle->isStoppedOnLane()) {
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

        bool changed;
        // let TraCI influence the wish to change lanes during non-actionsteps
        checkTraCICommands(vehicle);

        // Resume change
        changed = continueChangeSublane(vehicle, myCandi);
#ifdef DEBUG_ACTIONSTEPS
        if (DEBUG_COND) {
            std::cout << SIMTIME << " veh '" << vehicle->getID() << "' lcm->maneuverDist=" << vehicle->getLaneChangeModel().getManeuverDist()
                      << " lcm->speedLat=" << vehicle->getLaneChangeModel().getSpeedLat() << std::endl;
        }
#endif
        if (!changed) {
            registerUnchanged(vehicle);
        }
        return changed;
    }

#ifdef DEBUG_ACTIONSTEPS
    if (DEBUG_COND) {
        std::cout << "\n" << SIMTIME << " veh '" << vehicle->getID() << "'  plans lanechange maneuver." << std::endl;
    }
#endif
    vehicle->updateBestLanes(); // needed?
    const bool isOpposite = vehicle->getLaneChangeModel().isOpposite();
    if (!isOpposite) {
        for (int i = 0; i < (int) myChanger.size(); ++i) {
            vehicle->adaptBestLanesOccupation(i, myChanger[i].dens);
        }
    }
    // update leaders beyond the current edge for all lanes
    for (ChangerIt ce = myChanger.begin(); ce != myChanger.end(); ++ce) {
        ce->aheadNext = getLeaders(ce, vehicle);
    }
    // update expected speeds
    int sublaneIndex = 0;
    for (ChangerIt ce = myChanger.begin(); ce != myChanger.end(); ++ce) {
        vehicle->getLaneChangeModel().updateExpectedSublaneSpeeds(ce->aheadNext, sublaneIndex, ce->lane->getIndex());
        for (int offset : ce->siblings) {
            // treat sibling lanes (internal lanes with the same origin lane) as if they have the same geometry
            ChangerIt ceSib = ce + offset;
            vehicle->getLaneChangeModel().updateExpectedSublaneSpeeds(ceSib->aheadNext, sublaneIndex, ceSib->lane->getIndex());
        }
        sublaneIndex += ce->ahead.numSublanes();
    }

    // Check for changes to the opposite lane if vehicle is active
#ifdef DEBUG_ACTIONSTEPS
    if (DEBUG_COND) {
        std::cout << "  myChangeToOpposite=" << myChangeToOpposite << " isOpposite=" << isOpposite << " mayChangeRight=" << mayChange(-1) << " mayChangeLeft=" << mayChange(1) << "\n";
    }
#endif

    const bool stopOpposite = hasOppositeStop(vehicle);
    const int traciState = vehicle->influenceChangeDecision(0);
    const bool traciRequestOpposite = !mayChange(1) && (traciState & LCA_LEFT) != 0;

    if (myChangeToOpposite && (
                // cannot overtake since there is only one usable lane (or emergency)
                ((!mayChange(-1) && !mayChange(1)) || vehicle->getVClass() == SVC_EMERGENCY)
                || traciRequestOpposite
                || stopOpposite
                // can alway come back from the opposite side
                || isOpposite)) {
        const MSLeaderDistanceInfo& leaders = myCandi->aheadNext;
        if (leaders.hasVehicles() || isOpposite || stopOpposite || traciRequestOpposite) {
            std::pair<MSVehicle*, double> leader = findClosestLeader(leaders, vehicle);
            myCheckedChangeOpposite = false;
            if ((leader.first != nullptr || isOpposite || stopOpposite || traciRequestOpposite)
                    && changeOpposite(vehicle, leader, myCandi->lastStopped)) {
                return true;
            } else if (myCheckedChangeOpposite) {
                registerUnchanged(vehicle);
                return false;
            }
            // try sublane change within current lane otherwise
        }
    }

    LaneChangeAction alternatives = (LaneChangeAction)((mayChange(-1) ? LCA_RIGHT : LCA_NONE)
                                    | (mayChange(1) ? LCA_LEFT : LCA_NONE));

    StateAndDist right = checkChangeHelper(vehicle, -1, alternatives);
    StateAndDist left = checkChangeHelper(vehicle, 1, alternatives);
    StateAndDist current = checkChangeHelper(vehicle, 0, alternatives);

    StateAndDist decision = vehicle->getLaneChangeModel().decideDirection(current,
                            vehicle->getLaneChangeModel().decideDirection(right, left));
#ifdef DEBUG_DECISION
    if (vehicle->getLaneChangeModel().debugVehicle()) {
        std::cout << "\n" << SIMTIME << " decision=" << toString((LaneChangeAction)decision.state) << " dir=" << decision.dir << " latDist=" << decision.latDist << " maneuverDist=" << decision.maneuverDist << "\n";
    }
#endif
    vehicle->getLaneChangeModel().setOwnState(decision.state);
    if ((decision.state & LCA_WANTS_LANECHANGE) != 0 && (decision.state & LCA_BLOCKED) == 0) {
        // change if the vehicle wants to and is allowed to change
#ifdef DEBUG_MANEUVER
        if (DEBUG_COND) {
            std::cout << SIMTIME << " veh '" << vehicle->getID() << "' performing sublane change..." << std::endl;
        }
#endif
        const bool changed = startChangeSublane(vehicle, myCandi, decision.latDist, decision.maneuverDist);
        if (!changed) {
            registerUnchanged(vehicle);
        }
        return changed;
    }
    // @note this assumes vehicles can instantly abort any maneuvre in case of emergency
    abortLCManeuver(vehicle);
    registerUnchanged(vehicle);

    if ((right.state & (LCA_URGENT)) != 0 && (left.state & (LCA_URGENT)) != 0) {
        // ... wants to go to the left AND to the right
        // just let them go to the right lane...
        left.state = 0;
    }
    return false;
}


void
MSLaneChangerSublane::abortLCManeuver(MSVehicle* vehicle) {
#ifdef DEBUG_MANEUVER
    if (DEBUG_COND) {
        std::cout << SIMTIME << " veh '" << vehicle->getID() << "' aborts LC-continuation."
                  << std::endl;
    }
#endif
    const int priorReason = vehicle->getLaneChangeModel().getPrevState() & LCA_CHANGE_REASONS & ~LCA_SUBLANE;
    const int reason = vehicle->getLaneChangeModel().getOwnState() & LCA_CHANGE_REASONS & ~LCA_SUBLANE;
    if (priorReason != 0 && priorReason != reason && vehicle->getLaneChangeModel().getPreviousManeuverDist() != 0) {
        // original from cannot be reconstructed
        const int priorDirection = vehicle->getLaneChangeModel().getPreviousManeuverDist() > 0 ? 1 : -1;
#ifdef DEBUG_MANEUVER
        if (DEBUG_COND) {
            std::cout << SIMTIME << " abortLCManeuver priorReason=" << toString((LaneChangeAction)priorReason)
                      << " reason=" << toString((LaneChangeAction)reason) << " veh=" << vehicle->getID() << "\n";
        }
#endif
        outputLCEnded(vehicle, myCandi, myCandi, priorDirection);
    }
    const double updatedSpeedLat = vehicle->getLaneChangeModel().getSpeedLat() != 0;
    vehicle->getLaneChangeModel().setSpeedLat(0);
    vehicle->getLaneChangeModel().setManeuverDist(0.);
    vehicle->getLaneChangeModel().updateTargetLane();
    if (updatedSpeedLat) {
        // update angle after having reset lateral speed
        vehicle->setAngle(vehicle->computeAngle());
    }
}


MSLaneChangerSublane::StateAndDist
MSLaneChangerSublane::checkChangeHelper(MSVehicle* vehicle, int laneOffset, LaneChangeAction alternatives) {
    StateAndDist result = StateAndDist(0, 0, 0, 0);
    if (mayChange(laneOffset)) {
        if (laneOffset != 0 && vehicle->getLaneChangeModel().isOpposite()) {
            return result;
        }
        const std::vector<MSVehicle::LaneQ>& preb = (vehicle->getLaneChangeModel().isOpposite()
                ? getBestLanesOpposite(vehicle, nullptr, 1000)
                : vehicle->getBestLanes());
        result.state = checkChangeSublane(laneOffset, alternatives, preb, result.latDist, result.maneuverDist);
        result.dir = laneOffset;
        if ((result.state & LCA_WANTS_LANECHANGE) != 0 && (result.state & LCA_URGENT) != 0 && (result.state & LCA_BLOCKED) != 0) {
            (myCandi + laneOffset)->lastBlocked = vehicle;
            if ((myCandi + laneOffset)->firstBlocked == nullptr) {
                (myCandi + laneOffset)->firstBlocked = vehicle;
            }
        }
    }
    return result;
}


///  @brief Continue a sublane-lane change maneuver and return whether the midpoint was passed in this step
//          (used to continue sublane changing in non-action steps).
bool
MSLaneChangerSublane::continueChangeSublane(MSVehicle* vehicle, ChangerIt& from) {
    // lateral distance to complete maneuver
    double remLatDist = vehicle->getLaneChangeModel().getManeuverDist();
    if (remLatDist == 0) {
        return false;
    }
    const bool urgent = (vehicle->getLaneChangeModel().getOwnState() & LCA_URGENT) != 0;
    const double nextLatDist = SPEED2DIST(vehicle->getLaneChangeModel().computeSpeedLat(remLatDist, remLatDist, urgent));
#ifdef DEBUG_MANEUVER
    if (DEBUG_COND) {
        std::cout << SIMTIME << " vehicle '" << vehicle->getID() << "' continueChangeSublane()"
                  << " remLatDist=" << remLatDist << " nextLatDist=" << nextLatDist
                  << std::endl;
    }
#endif

    const bool changed = startChangeSublane(vehicle, from, nextLatDist, remLatDist);
    return changed;
}


bool
MSLaneChangerSublane::startChangeSublane(MSVehicle* vehicle, ChangerIt& from, double latDist, double maneuverDist) {
    if (vehicle->isRemoteControlled()) {
        return false;
    }
    MSLane* source = from->lane;
    // Prevent continuation of LC beyond lane borders if change is not allowed
    double distToRightLaneBorder = vehicle->getLane()->getWidth() * 0.5 + vehicle->getLateralPositionOnLane() - vehicle->getWidth() * 0.5;
    double distToLeftLaneBorder = vehicle->getLane()->getWidth() * 0.5 - vehicle->getLateralPositionOnLane() - vehicle->getWidth() * 0.5;
    if (vehicle->getLaneChangeModel().isOpposite()) {
        std::swap(distToRightLaneBorder, distToLeftLaneBorder);
    }
    // determine direction of LC
    int direction = 0;
    if (latDist > 0 && latDist > distToLeftLaneBorder) {
        direction = 1;
    } else if (latDist < 0 && -latDist > distToRightLaneBorder) {
        direction = -1;
    }
    const int changerDirection = vehicle->getLaneChangeModel().isOpposite() ? -direction : direction;
    ChangerIt to = from;
#ifdef DEBUG_MANEUVER
    if (DEBUG_COND) {
        std::cout << SIMTIME << " vehicle '" << vehicle->getID() << "' latDist=" << latDist << "  maneuverDist=" << maneuverDist
                  << " distRight=" << distToRightLaneBorder << " distLeft=" << distToLeftLaneBorder
                  << " dir=" << direction << " cDir=" << changerDirection << " mayChange=" << mayChange(changerDirection) << "\n";
    }
#endif
    if (mayChange(changerDirection)) {
        to = from + changerDirection;
    } else if (changerDirection == 1 && source->getOpposite() != nullptr) {
        // change to the opposite direction lane
        to = (source->getOpposite()->getEdge().myLaneChanger->getChanger().end() - 1);
    } else {
        // This may occur during maneuver continuation in non-actionsteps.
        // TODO: Understand better why and test later if additional sublane actionstep debugging resolves this
        // (XXX: perhaps one should try to extrapolate check for this case before to avoid maneuver initialization
        //       similar as for continuous LC in MSLaneChanger::checkChange())
        //assert(false);
        abortLCManeuver(vehicle);
        return false;
    }

    // The following does:
    // 1) update vehicles lateral position according to latDist and target lane
    // 2) distinguish several cases
    //   a) vehicle moves completely within the same lane
    //   b) vehicle intersects another lane
    //      - vehicle must be moved to the lane where it's midpoint is (either old or new)
    //      - shadow vehicle must be created/moved to the other lane if the vehicle intersects it
    // 3) updated dens of all lanes that hold the vehicle or its shadow

    vehicle->myState.myPosLat += latDist * (vehicle->getLaneChangeModel().isOpposite() ? -1 : 1);
    for (int i = 0; i < (int)vehicle->myFurtherLanesPosLat.size(); i++) {
        vehicle->myFurtherLanesPosLat[i] += latDist * (vehicle->getLaneChangeModel().isOpposite() ? -1 : 1);
    }
    vehicle->myCachedPosition = Position::INVALID;
    vehicle->getLaneChangeModel().setSpeedLat(DIST2SPEED(latDist));
#ifdef DEBUG_MANEUVER
    if (DEBUG_COND) {
        std::cout << SIMTIME << " vehicle '" << vehicle->getID() << "' with maneuverDist=" << maneuverDist
                  << " and committedSpeed=" << vehicle->getLaneChangeModel().getCommittedSpeed()
                  << " increments lateral position by latDist=" << latDist << std::endl;
    }
#endif
#ifdef DEBUG_SURROUNDING
    if (DEBUG_COND) {
        std::cout << SIMTIME << " vehicle '" << vehicle->getID() << "'\n    to->ahead=" << to->ahead.toString()
                  << "'\n    to->aheadNext=" << to->aheadNext.toString()
                  << std::endl;
    }
#endif
    const bool completedPriorManeuver = fabs(vehicle->getLaneChangeModel().getPreviousManeuverDist()) < NUMERICAL_EPS;
    const bool completedManeuver = fabs(maneuverDist - latDist) < NUMERICAL_EPS;
    vehicle->getLaneChangeModel().updateSafeLatDist(latDist);

    // current maneuver is aborted when direction or reason changes
    const int priorReason = vehicle->getLaneChangeModel().getPrevState() & LCA_CHANGE_REASONS & ~LCA_SUBLANE;
    const int reason = vehicle->getLaneChangeModel().getOwnState() & LCA_CHANGE_REASONS & ~LCA_SUBLANE;
#ifdef DEBUG_MANEUVER
    if (DEBUG_COND) {
        std::cout << SIMTIME << " vehicle '" << vehicle->getID()
                  << "' completedPriorManeuver=" << completedPriorManeuver
                  << " completedManeuver=" << completedManeuver
                  << " priorReason=" << toString((LaneChangeAction)priorReason)
                  << " reason=" << toString((LaneChangeAction)reason)
                  << " priorManeuverDist=" << vehicle->getLaneChangeModel().getPreviousManeuverDist()
                  << " maneuverDist=" << maneuverDist
                  << " latDist=" << latDist
                  << std::endl;
    }
#endif
    if (!completedManeuver && !completedPriorManeuver && priorReason != 0 &&
            (vehicle->getLaneChangeModel().getPreviousManeuverDist() * latDist < 0
             || priorReason != reason)) {
        const int priorDirection = vehicle->getLaneChangeModel().getPreviousManeuverDist() > 0 ? 1 : -1;
        // original from cannot be reconstructed
#ifdef DEBUG_MANEUVER
        if (DEBUG_COND) {
            std::cout << SIMTIME << " startChangeSublane abort priorReason=" << toString((LaneChangeAction)priorReason)
                      << " reason=" << toString((LaneChangeAction)reason) << " veh=" << vehicle->getID() << "\n";
        }
#endif
        outputLCEnded(vehicle, from, from, priorDirection);
    }

    outputLCStarted(vehicle, from, to, direction, maneuverDist);
    vehicle->getLaneChangeModel().setManeuverDist(maneuverDist - latDist);
    const bool changedToNewLane = checkChangeToNewLane(vehicle, direction, from, to);

    MSLane* oldShadowLane = vehicle->getLaneChangeModel().getShadowLane();
    vehicle->getLaneChangeModel().updateShadowLane();
    MSLane* shadowLane = vehicle->getLaneChangeModel().getShadowLane();
    if (shadowLane != nullptr && shadowLane != oldShadowLane
            && &shadowLane->getEdge() == &source->getEdge()) {
        assert(oldShadowLane == 0 || vehicle->getLaneChangeModel().isOpposite() || to != from);
        const double latOffset = vehicle->getLane()->getRightSideOnEdge() - shadowLane->getRightSideOnEdge();
        (myChanger.begin() + shadowLane->getIndex())->ahead.addLeader(vehicle, false, latOffset);
    }
    if (completedManeuver) {
        outputLCEnded(vehicle, from, to, direction);
    }

    // Update maneuver reservations on target lanes
    MSLane* targetLane = vehicle->getLaneChangeModel().updateTargetLane();
    if (!changedToNewLane && targetLane != nullptr
            && vehicle->getActionStepLength() > DELTA_T
            && &targetLane->getEdge() == &source->getEdge()
       ) {
        const int dir = (vehicle->getLaneChangeModel().getManeuverDist() > 0 ? 1 : -1);
        ChangerIt target = from + dir;
        const double actionStepDist = dir * vehicle->getVehicleType().getMaxSpeedLat() * vehicle->getActionStepLengthSecs();
        const double latOffset = vehicle->getLatOffset(targetLane) + actionStepDist;
        target->ahead.addLeader(vehicle, false, latOffset);
        //std::cout << SIMTIME << " veh=" << vehicle->getID() << " target=" << targetLane->getID()
        //    << " actionStepDist=" << actionStepDist << " latOffset=" << latOffset
        //    << " targetAhead=" << target->ahead.toString() << "\n";
    }

    // compute new angle of the vehicle from the x- and y-distances travelled within last time step
    // (should happen last because primaryLaneChanged() also triggers angle computation)
    // this part of the angle comes from the orientation of our current lane
    double laneAngle = vehicle->computeAngle();
    if (vehicle->getLaneChangeModel().isOpposite()) {
        // reverse lane angle
        laneAngle += M_PI;
    }
#ifdef DEBUG_MANEUVER
    if (DEBUG_COND) {
        std::cout << SIMTIME << " startChangeSublane"
                  << " oldLane=" << from->lane->getID()
                  << " newLane=" << to->lane->getID()
                  << " shadowLane=" << (shadowLane != nullptr ? shadowLane->getID() : "NULL")
                  << " targetLane=" << (targetLane != nullptr ? targetLane->getID() : "NULL")
                  << " maneuverDist=" << vehicle->getLaneChangeModel().getManeuverDist()
                  << " latDist=" << latDist
                  << " oldShadowLane=" << Named::getIDSecure(oldShadowLane)
                  << " newShadowLane=" << Named::getIDSecure(vehicle->getLaneChangeModel().getShadowLane())
                  << " laneA=" << RAD2DEG(laneAngle)
                  << " oldA=" << RAD2DEG(vehicle->getAngle())
                  << " oppposite=" << vehicle->getLaneChangeModel().isOpposite()
                  << " changedToNewLane=" << changedToNewLane
                  << "\n";
    }
#endif
    vehicle->setAngle(laneAngle, completedManeuver);

    // check if a traci maneuver must continue
    // getOwnState is reset to 0 when changing lanes so we use the stored reason
    if ((reason & LCA_TRACI) != 0) {
#ifdef DEBUG_MANEUVER
        if (DEBUG_COND) {
            std::cout << SIMTIME << " continue TraCI-maneuver remainingLatDist=" << vehicle->getLaneChangeModel().getManeuverDist() << "\n";
        }
#endif
        vehicle->getInfluencer().setSublaneChange(vehicle->getLaneChangeModel().getManeuverDist());
    }
    from->lane->requireCollisionCheck();
    to->lane->requireCollisionCheck();
    return changedToNewLane;
}

bool
MSLaneChangerSublane::checkChangeToNewLane(MSVehicle* vehicle, const int direction, ChangerIt from, ChangerIt to) {
    const int oppositeSign = vehicle->getLaneChangeModel().isOpposite() ? -1 : 1;
    const bool opposite = (&from->lane->getEdge() != &to->lane->getEdge());
    const bool changedToNewLane = (to->lane != from->lane
                                   && fabs(vehicle->getLateralPositionOnLane()) > 0.5 * vehicle->getLane()->getWidth()
                                   && (mayChange(direction * oppositeSign) || opposite));
    if (changedToNewLane) {
        vehicle->myState.myPosLat -= direction * 0.5 * (from->lane->getWidth() + to->lane->getWidth()) * oppositeSign;
        if (!opposite) {
            to->lane->myTmpVehicles.insert(to->lane->myTmpVehicles.begin(), vehicle);
            to->dens += vehicle->getVehicleType().getLengthWithGap();
        }
        if (MSAbstractLaneChangeModel::haveLCOutput()) {
            if (!vehicle->isActive()) {
                // update leaders beyond the current edge for all lanes
                // @note to->aheadNext and from->aheadNext are only needed for output in non-action steps.
                to->aheadNext = getLeaders(to, vehicle);
                from->aheadNext = getLeaders(from, vehicle);
            }
            vehicle->getLaneChangeModel().setLeaderGaps(to->aheadNext);
            vehicle->getLaneChangeModel().setFollowerGaps(to->lane->getFollowersOnConsecutive(vehicle, vehicle->getBackPositionOnLane(), true));
            vehicle->getLaneChangeModel().setOrigLeaderGaps(from->aheadNext);
        }
        vehicle->getLaneChangeModel().startLaneChangeManeuver(from->lane, to->lane, direction);
        if (!opposite) {
            to->ahead.addLeader(vehicle, false, 0);
        }
    } else {
        from->ahead.addLeader(vehicle, false, 0);
    }
    return changedToNewLane;
}

void
MSLaneChangerSublane::outputLCStarted(MSVehicle* vehicle, ChangerIt& from, ChangerIt& to, int direction, double maneuverDist) {
    if (MSAbstractLaneChangeModel::haveLCOutput() && MSAbstractLaneChangeModel::outputLCStarted()
            // non-sublane change started
            && ((vehicle->getLaneChangeModel().getOwnState() & (LCA_CHANGE_REASONS & ~LCA_SUBLANE)) != 0)
            && ((vehicle->getLaneChangeModel().getOwnState() & LCA_STAY) == 0)
            // no changing for the same reason in previous step (either not wanted or blocked)
            && ((vehicle->getLaneChangeModel().getPrevState() & LCA_CHANGE_REASONS & ~LCA_SUBLANE) !=
                (vehicle->getLaneChangeModel().getOwnState() & LCA_CHANGE_REASONS & ~LCA_SUBLANE)
                || ((vehicle->getLaneChangeModel().getPrevState() & LCA_STAY) != 0)
                || ((vehicle->getLaneChangeModel().getPrevState() & LCA_BLOCKED) != 0))
       ) {
#ifdef DEBUG_STATE
        if (DEBUG_COND) {
            std::cout << SIMTIME << " veh=" << vehicle->getID() << " laneChangeStarted state=" << toString((LaneChangeAction)vehicle->getLaneChangeModel().getOwnState())
                      << " prevState=" << toString((LaneChangeAction)vehicle->getLaneChangeModel().getPrevState())
                      << " filter=" << toString((LaneChangeAction)(LCA_CHANGE_REASONS & ~LCA_SUBLANE))
                      << " filtered=" << toString((LaneChangeAction)(vehicle->getLaneChangeModel().getOwnState() & (LCA_CHANGE_REASONS & ~LCA_SUBLANE)))
                      << "\n";
        }
#endif
        vehicle->getLaneChangeModel().setLeaderGaps(to->aheadNext);
        vehicle->getLaneChangeModel().setFollowerGaps(to->lane->getFollowersOnConsecutive(vehicle, vehicle->getBackPositionOnLane(), true));
        vehicle->getLaneChangeModel().setOrigLeaderGaps(from->aheadNext);
        vehicle->getLaneChangeModel().laneChangeOutput("changeStarted", from->lane, to->lane, direction, maneuverDist);
    }
}

void
MSLaneChangerSublane::outputLCEnded(MSVehicle* vehicle, ChangerIt& from, ChangerIt& to, int direction) {
    if (MSAbstractLaneChangeModel::haveLCOutput() && MSAbstractLaneChangeModel::outputLCEnded()
            // non-sublane change ended
            && ((vehicle->getLaneChangeModel().getOwnState() & (LCA_CHANGE_REASONS & ~LCA_SUBLANE)) != 0)) {
        vehicle->getLaneChangeModel().setLeaderGaps(to->aheadNext);
        vehicle->getLaneChangeModel().setFollowerGaps(to->lane->getFollowersOnConsecutive(vehicle, vehicle->getBackPositionOnLane(), true));
        vehicle->getLaneChangeModel().setOrigLeaderGaps(from->aheadNext);
        vehicle->getLaneChangeModel().laneChangeOutput("changeEnded", from->lane, to->lane, direction);
    }
}


MSLeaderDistanceInfo
MSLaneChangerSublane::getLeaders(const ChangerIt& target, const MSVehicle* vehicle) const {
    // get the leading vehicle on the lane to change to
#ifdef DEBUG_SURROUNDING
    if (DEBUG_COND) {
        std::cout << SIMTIME << " getLeaders lane=" << target->lane->getID() << " ego=" << vehicle->getID() << " ahead=" << target->ahead.toString() << "\n";
    }
#endif
    MSLeaderDistanceInfo result(target->lane->getWidth(), nullptr, 0);
    int sublaneShift = 0;
    if (target->lane == vehicle->getLane()) {
        if (vehicle->getLeftSideOnLane() < -MSGlobals::gLateralResolution) {
            sublaneShift = int(-vehicle->getLeftSideOnLane() / MSGlobals::gLateralResolution);
        } else if (vehicle->getRightSideOnLane() > target->lane->getWidth() + MSGlobals::gLateralResolution) {
            sublaneShift = -int((vehicle->getRightSideOnLane() - target->lane->getWidth()) / MSGlobals::gLateralResolution);
        }
        result.setSublaneOffset(sublaneShift);
    }
    for (int i = 0; i < target->ahead.numSublanes(); ++i) {
        const MSVehicle* veh = target->ahead[i];
        if (veh != nullptr) {
            const double gap = veh->getBackPositionOnLane(target->lane) - vehicle->getPositionOnLane() - vehicle->getVehicleType().getMinGap();
#ifdef DEBUG_SURROUNDING
            if (DEBUG_COND) {
                std::cout << " ahead lead=" << veh->getID() << " leadBack=" << veh->getBackPositionOnLane() << " gap=" << gap << " sublaneShift=" << sublaneShift << "\n";
            }
#endif
            if (i + sublaneShift >= 0 && i + sublaneShift < result.numSublanes()) {
                result.addLeader(veh, gap, 0, i + sublaneShift);
            }
        }
    }
    if (sublaneShift != 0) {
        for (MSVehicle* cand : target->outsideBounds) {
            const double gap = cand->getBackPositionOnLane() - vehicle->getPositionOnLane() - vehicle->getVehicleType().getMinGap();
            result.addLeader(cand, gap);
        }
    }
#ifdef DEBUG_SURROUNDING
    if (DEBUG_COND) {
        std::cout << "   outsideBounds=" << toString(target->outsideBounds) << " result=" << result.toString() << "\n";
    }
#endif
    target->lane->addLeaders(vehicle, vehicle->getPositionOnLane(), result);
    return result;
}


int
MSLaneChangerSublane::checkChangeSublane(
    int laneOffset,
    LaneChangeAction alternatives,
    const std::vector<MSVehicle::LaneQ>& preb,
    double& latDist,
    double& maneuverDist) const {

    ChangerIt target = myCandi + laneOffset;
    MSVehicle* vehicle = veh(myCandi);
    const MSLane& neighLane = *(target->lane);
    int blocked = 0;

    MSLeaderDistanceInfo neighLeaders = target->aheadNext;
    MSLeaderDistanceInfo neighFollowers = target->lane->getFollowersOnConsecutive(vehicle, vehicle->getBackPositionOnLane(), true);
    MSLeaderDistanceInfo neighBlockers(neighLane.getWidth(), vehicle, vehicle->getLane()->getRightSideOnEdge() - neighLane.getRightSideOnEdge());
    MSLeaderDistanceInfo leaders = myCandi->aheadNext;
    addOutsideLeaders(vehicle, leaders);
    MSLeaderDistanceInfo followers = myCandi->lane->getFollowersOnConsecutive(vehicle, vehicle->getBackPositionOnLane(), true);
    MSLeaderDistanceInfo blockers(vehicle->getLane()->getWidth(), vehicle, 0);

    // consider sibling lanes of the origin and target lane
    for (int offset : myCandi->siblings) {
        // treat sibling lanes (internal lanes with the same origin lane) as if they have the same geometry
        ChangerIt ceSib = myCandi + offset;
        MSLeaderDistanceInfo sibFollowers = ceSib->lane->getFollowersOnConsecutive(vehicle, vehicle->getBackPositionOnLane(), true);
        if (sibFollowers.hasVehicles()) {
            followers.addLeaders(sibFollowers);
        }
        if (ceSib->aheadNext.hasVehicles()) {
            leaders.addLeaders(ceSib->aheadNext);
        }
#ifdef DEBUG_SURROUNDING
        if (DEBUG_COND) {
            std::cout << SIMTIME << " ego=" << vehicle->getID() << " ahead=" << myCandi->aheadNext.toString() << " sib=" << ceSib->lane->getID() << " sibAhead=" << ceSib->aheadNext.toString() << " leaders=" << leaders.toString() << "\n";
        }
#endif
    }
    for (int offset : target->siblings) {
        // treat sibling lanes (internal lanes with the same origin lane) as if they have the same geometry
        ChangerIt ceSib = target + offset;
        MSLeaderDistanceInfo sibFollowers = ceSib->lane->getFollowersOnConsecutive(vehicle, vehicle->getBackPositionOnLane(), true);
        if (sibFollowers.hasVehicles()) {
            neighFollowers.addLeaders(sibFollowers);
        }
        if (ceSib->aheadNext.hasVehicles()) {
            neighLeaders.addLeaders(ceSib->aheadNext);
        }
#ifdef DEBUG_SURROUNDING
        if (DEBUG_COND) {
            std::cout << SIMTIME << " ego=" << vehicle->getID() << " neighAhead=" << target->aheadNext.toString() << " sib=" << ceSib->lane->getID() << " sibAhead=" << ceSib->aheadNext.toString() << " neighLeaders=" << neighLeaders.toString() << "\n";
        }
#endif
    }

    // break leader symmetry
    if (laneOffset == -1 && neighLeaders.hasVehicles()) {
        neighLeaders.moveSamePosTo(vehicle, neighFollowers);
    }

#ifdef DEBUG_SURROUNDING
    if (DEBUG_COND) std::cout << SIMTIME
                                  << " checkChangeSublane: veh=" << vehicle->getID()
                                  << " laneOffset=" << laneOffset
                                  << "\n  leaders=" << leaders.toString()
                                  << "\n  neighLeaders=" << neighLeaders.toString()
                                  << "\n  followers=" << followers.toString()
                                  << "\n  neighFollowers=" << neighFollowers.toString()
                                  << "\n";
#endif


    const int wish = vehicle->getLaneChangeModel().wantsChangeSublane(
                         laneOffset, alternatives,
                         leaders, followers, blockers,
                         neighLeaders, neighFollowers, neighBlockers,
                         neighLane, preb,
                         &(myCandi->lastBlocked), &(myCandi->firstBlocked), latDist, maneuverDist, blocked);
    int state = blocked | wish;

    // XXX
    // do are more careful (but expensive) check to ensure that a
    // safety-critical leader is not being overlooked

    // XXX
    // ensure that a continuous lane change manoeuvre can be completed
    // before the next turning movement

    // let TraCI influence the wish to change lanes and the security to take
    const int oldstate = state;
    state = vehicle->influenceChangeDecision(state);
#ifdef DEBUG_STATE
    if (DEBUG_COND && state != oldstate) {
        std::cout << SIMTIME << " veh=" << vehicle->getID() << " stateAfterTraCI=" << toString((LaneChangeAction)state) << " original=" << toString((LaneChangeAction)oldstate) << "\n";
    }
#endif
    vehicle->getLaneChangeModel().saveLCState(laneOffset, oldstate, state);
    if (laneOffset != 0) {
        vehicle->getLaneChangeModel().saveNeighbors(laneOffset, neighFollowers, neighLeaders);
    }
    return state;
}


bool
MSLaneChangerSublane::checkChangeOpposite(
    MSVehicle* vehicle,
    int laneOffset,
    MSLane* targetLane,
    const std::pair<MSVehicle* const, double>& leader,
    const std::pair<MSVehicle* const, double>& neighLead,
    const std::pair<MSVehicle* const, double>& neighFollow,
    const std::vector<MSVehicle::LaneQ>& preb) {
    myCheckedChangeOpposite = true;

    UNUSED_PARAMETER(leader);
    UNUSED_PARAMETER(neighLead);
    UNUSED_PARAMETER(neighFollow);

    const MSLane& neighLane = *targetLane;
    MSLane* curLane = myCandi->lane;

    MSLeaderDistanceInfo neighLeaders(targetLane->getWidth(), nullptr, 0);
    MSLeaderDistanceInfo neighFollowers(targetLane->getWidth(), nullptr, 0);
    MSLeaderDistanceInfo neighBlockers(targetLane->getWidth(), nullptr, 0);
    MSLeaderDistanceInfo leaders(curLane->getWidth(), nullptr, 0);
    MSLeaderDistanceInfo followers(curLane->getWidth(), nullptr, 0);
    MSLeaderDistanceInfo blockers(curLane->getWidth(), nullptr, 0);

    const double backPosOnTarget = vehicle->getLane()->getOppositePos(vehicle->getBackPositionOnLane());
    if (vehicle->getLaneChangeModel().isOpposite()) {
        leaders = curLane->getFollowersOnConsecutive(vehicle, vehicle->getPositionOnLane(), true, -1, MSLane::MinorLinkMode::FOLLOW_ONCOMING);
        leaders.fixOppositeGaps(false);
        curLane->addLeaders(vehicle, vehicle->getBackPositionOnLane(), followers);
        followers.fixOppositeGaps(true);
        neighFollowers = targetLane->getFollowersOnConsecutive(vehicle, backPosOnTarget, true);
        neighFollowers.fixOppositeGaps(false);
        // artificially increase the position to ensure that ego is not added as a leader
        const double posOnTarget = backPosOnTarget + vehicle->getVehicleType().getLength() + POSITION_EPS;
        targetLane->addLeaders(vehicle, posOnTarget, neighLeaders);
        neighLeaders.patchGaps(2 * POSITION_EPS);
        int sublaneIndex = 0;
        for (int i = 0; i < targetLane->getIndex(); i++) {
            sublaneIndex += MSLeaderInfo(targetLane->getEdge().getLanes()[i]->getWidth()).numSublanes();
        }
        vehicle->getLaneChangeModel().updateExpectedSublaneSpeeds(neighLeaders, sublaneIndex, targetLane->getIndex());
    } else {
        leaders = myCandi->aheadNext;
        followers = myCandi->lane->getFollowersOnConsecutive(vehicle, vehicle->getBackPositionOnLane(), true);
        const double posOnTarget = backPosOnTarget - vehicle->getVehicleType().getLength();
        targetLane->addLeaders(vehicle, backPosOnTarget, neighFollowers, true);
        neighFollowers.fixOppositeGaps(true);
        neighLeaders = targetLane->getFollowersOnConsecutive(vehicle, posOnTarget, true);
        neighLeaders.fixOppositeGaps(false);
    }


#ifdef DEBUG_CHANGE_OPPOSITE
    if (DEBUG_COND) std::cout << SIMTIME
                                  << " checkChangeOppositeSublane: veh=" << vehicle->getID()
                                  << " isOpposite=" << vehicle->getLaneChangeModel().isOpposite()
                                  << " laneOffset=" << laneOffset
                                  << "\n  leaders=" << leaders.toString()
                                  << "\n  neighLeaders=" << neighLeaders.toString()
                                  << "\n  followers=" << followers.toString()
                                  << "\n  neighFollowers=" << neighFollowers.toString()
                                  << "\n";
#endif

    LaneChangeAction alternatives = (LaneChangeAction)((mayChange(-1) ? LCA_RIGHT : LCA_NONE)
                                    | (mayChange(1) ? LCA_LEFT : LCA_NONE));

    int blocked = 0;
    double latDist = 0;
    double maneuverDist = 0;
    const int wish = vehicle->getLaneChangeModel().wantsChangeSublane(
                         laneOffset, alternatives,
                         leaders, followers, blockers,
                         neighLeaders, neighFollowers, neighBlockers,
                         neighLane, preb,
                         &(myCandi->lastBlocked), &(myCandi->firstBlocked), latDist, maneuverDist, blocked);
    int state = blocked | wish;

    const int oldstate = state;
    state = vehicle->influenceChangeDecision(state);
#ifdef DEBUG_CHANGE_OPPOSITE
    if (DEBUG_COND && state != oldstate) {
        std::cout << SIMTIME << " veh=" << vehicle->getID() << " stateAfterTraCI=" << toString((LaneChangeAction)state) << " original=" << toString((LaneChangeAction)oldstate) << "\n";
    }
#endif
    vehicle->getLaneChangeModel().saveLCState(laneOffset, oldstate, state);
    if (laneOffset != 0) {
        vehicle->getLaneChangeModel().saveNeighbors(laneOffset, neighFollowers, neighLeaders);
    }

    if ((state & LCA_WANTS_LANECHANGE) != 0 && (state & LCA_BLOCKED) == 0) {
        // change if the vehicle wants to and is allowed to change
#ifdef DEBUG_CHANGE_OPPOSITE
        if (DEBUG_COND) {
            std::cout << SIMTIME << " veh '" << vehicle->getID() << "' performing sublane change latDist=" << latDist << " maneuverDist=" << maneuverDist << "\n";
        }
#endif
        vehicle->getLaneChangeModel().setOwnState(state);
        return startChangeSublane(vehicle, myCandi, latDist, maneuverDist);
    } else {
        vehicle->getLaneChangeModel().setSpeedLat(0);
        return false;
    }
}

std::pair<MSVehicle*, double>
MSLaneChangerSublane::findClosestLeader(const MSLeaderDistanceInfo& leaders, const MSVehicle* vehicle) {
    const double egoWidth = vehicle->getVehicleType().getWidth() + vehicle->getVehicleType().getMinGapLat();
    std::pair<MSVehicle*, double> leader(nullptr, std::numeric_limits<double>::max());
    for (int i = 0; i < leaders.numSublanes(); ++i) {
        CLeaderDist cand = leaders[i];
        if (cand.first != nullptr) {
            const double rightSide = cand.first->getRightSideOnLane();
            if (cand.second < leader.second
                    && rightSide < egoWidth
                    && vehicle->getLane()->getWidth() - rightSide - cand.first->getVehicleType().getWidth() < egoWidth) {
                leader.first = const_cast<MSVehicle*>(cand.first);
                leader.second = cand.second;
            }
        }
    }
    return leader;
}


void
MSLaneChangerSublane::addOutsideLeaders(const MSVehicle* vehicle, MSLeaderDistanceInfo& leaders) const {
    if (vehicle->getLaneChangeModel().getShadowLane() == nullptr) {
        const MSLane* lane = vehicle->getLane();
        const double rightOL = vehicle->getRightSideOnLane(lane);
        const double leftOL = vehicle->getLeftSideOnLane(lane);
        const bool outsideLeft = rightOL > lane->getWidth();
#ifdef DEBUG_SURROUNDING
        if (DEBUG_COND) {
            std::cout << SIMTIME << " addOutsideLeaders veh=" << vehicle->getID() << " lane=" << lane->getID() << " rightOL=" << rightOL << " leftOL=" << leftOL << "\n";
        }
#endif
        if (leftOL < 0 || outsideLeft) {
            int sublaneOffset = 0;
            if (outsideLeft) {
                sublaneOffset = MIN2(0, -(int)ceil((leftOL - lane->getWidth()) / MSGlobals::gLateralResolution) + 1);
            } else {
                sublaneOffset = MAX2(0, (int)ceil(-rightOL / MSGlobals::gLateralResolution) - 1);
            }
            if (sublaneOffset != 0) {
                leaders.setSublaneOffset(sublaneOffset);
#ifdef DEBUG_SURROUNDING
                if (DEBUG_COND) {
                    std::cout << "   sublaneOffset=" << sublaneOffset << " outsideLeft=" << outsideLeft << "\n";
                }
#endif
                for (const MSVehicle* cand : lane->myTmpVehicles) {
#ifdef DEBUG_SURROUNDING
                    if (DEBUG_COND) {
                        std::cout << " cand=" << cand->getID() << " cLOE=" << cand->getLeftSideOnEdge() << " cROE=" << cand->getRightSideOnEdge() << "\n";
                    }
#endif
                    if (cand->getPositionOnLane() > vehicle->getPositionOnLane()
                            && ((!outsideLeft && cand->getLeftSideOnEdge() < 0)
                                || (outsideLeft && cand->getLeftSideOnEdge() > lane->getEdge().getWidth()))) {
                        const double gap = cand->getPositionOnLane() - vehicle->getPositionOnLane() - cand->getLength() - vehicle->getVehicleType().getMinGap();
                        leaders.addLeader(cand, gap);
#ifdef DEBUG_SURROUNDING
                        if (DEBUG_COND) {
                            std::cout << "   outsideLeader=" << cand->getID() << " ahead=" << leaders.toString() << "\n";
                        }
#endif
                    }
                }
            }
        }
    }
}

/****************************************************************************/
