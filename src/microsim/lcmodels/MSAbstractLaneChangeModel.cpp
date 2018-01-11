/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSAbstractLaneChangeModel.cpp
/// @author  Daniel Krajzewicz
/// @author  Friedemann Wesner
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @author  Leonhard Luecken
/// @date    Fri, 29.04.2005
/// @version $Id$
///
// Interface for lane-change models
/****************************************************************************/

// ===========================================================================
// DEBUG
// ===========================================================================
//#define DEBUG_TARGET_LANE
#define DEBUG_COND (myVehicle.isSelected())


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/options/OptionsCont.h>
#include "MSAbstractLaneChangeModel.h"
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSGlobals.h>
#include "MSLCM_DK2008.h"
#include "MSLCM_LC2013.h"
#include "MSLCM_SL2015.h"

/* -------------------------------------------------------------------------
 * static members
 * ----------------------------------------------------------------------- */
bool MSAbstractLaneChangeModel::myAllowOvertakingRight(false);
bool MSAbstractLaneChangeModel::myLCOutput(false);
bool MSAbstractLaneChangeModel::myLCStartedOutput(false);
bool MSAbstractLaneChangeModel::myLCEndedOutput(false);
const double MSAbstractLaneChangeModel::NO_NEIGHBOR(std::numeric_limits<double>::max());

/* -------------------------------------------------------------------------
 * MSAbstractLaneChangeModel-methods
 * ----------------------------------------------------------------------- */

void
MSAbstractLaneChangeModel::initGlobalOptions(const OptionsCont& oc) {
    myAllowOvertakingRight = oc.getBool("lanechange.overtake-right");
    myLCOutput = oc.isSet("lanechange-output");
    myLCStartedOutput = oc.getBool("lanechange-output.started");
    myLCEndedOutput = oc.getBool("lanechange-output.ended");
}


MSAbstractLaneChangeModel*
MSAbstractLaneChangeModel::build(LaneChangeModel lcm, MSVehicle& v) {
    if (MSGlobals::gLateralResolution > 0 && lcm != LCM_SL2015 && lcm != LCM_DEFAULT) {
        throw ProcessError("Lane change model '" + toString(lcm) + "' is not compatible with sublane simulation");
    }
    switch (lcm) {
        case LCM_DK2008:
            return new MSLCM_DK2008(v);
        case LCM_LC2013:
            return new MSLCM_LC2013(v);
        case LCM_SL2015:
            return new MSLCM_SL2015(v);
        case LCM_DEFAULT:
            if (MSGlobals::gLateralResolution <= 0) {
                return new MSLCM_LC2013(v);
            } else {
                return new MSLCM_SL2015(v);
            }
        default:
            throw ProcessError("Lane change model '" + toString(lcm) + "' not implemented");
    }
}


MSAbstractLaneChangeModel::MSAbstractLaneChangeModel(MSVehicle& v, const LaneChangeModel model) :
    myVehicle(v),
    myOwnState(0),
    myPreviousState(0),
    myPreviousState2(0),
    mySpeedLat(0),
    myCommittedSpeed(0),
    myLaneChangeCompletion(1.0),
    myLaneChangeDirection(0),
    myManeuverDist(0.),
    myAlreadyChanged(false),
    myShadowLane(nullptr),
    myTargetLane(nullptr),
    myCarFollowModel(v.getCarFollowModel()),
    myModel(model),
    myLastLateralGapLeft(0.),
    myLastLateralGapRight(0.),
    myLastLeaderGap(0.),
    myLastFollowerGap(0.),
    myLastLeaderSecureGap(0.),
    myLastFollowerSecureGap(0.),
    myLastOrigLeaderGap(0.),
    myLastOrigLeaderSecureGap(0.),
    myLastLaneChangeOffset(0),
    myAmOpposite(false) {
}


MSAbstractLaneChangeModel::~MSAbstractLaneChangeModel() {
}

void
MSAbstractLaneChangeModel::setOwnState(const int state) {
    myPreviousState2 = myPreviousState;
    myOwnState = state;
    myPreviousState = state; // myOwnState is modified in prepareStep so we make a backup
}

void
MSAbstractLaneChangeModel::updateSafeLatDist(const double travelledLatDist) {
    UNUSED_PARAMETER(travelledLatDist);
}


void
MSAbstractLaneChangeModel::setManeuverDist(const double dist) {
#ifdef DEBUG_MANEUVER
    if DEBUG_COND {
    std::cout << SIMTIME
    << " veh=" << myVehicle.getID()
        << " setManeuverDist() old=" << myManeuverDist << " new=" << dist
        << std::endl;
    }
#endif
    myManeuverDist = dist;
}


double
MSAbstractLaneChangeModel::getManeuverDist() const {
    return myManeuverDist;
}


bool
MSAbstractLaneChangeModel::congested(const MSVehicle* const neighLeader) {
    if (neighLeader == 0) {
        return false;
    }
    // Congested situation are relevant only on highways (maxSpeed > 70km/h)
    // and congested on German Highways means that the vehicles have speeds
    // below 60km/h. Overtaking on the right is allowed then.
    if ((myVehicle.getLane()->getSpeedLimit() <= 70.0 / 3.6) || (neighLeader->getLane()->getSpeedLimit() <= 70.0 / 3.6)) {

        return false;
    }
    if (myVehicle.congested() && neighLeader->congested()) {
        return true;
    }
    return false;
}



bool
MSAbstractLaneChangeModel::predInteraction(const std::pair<MSVehicle*, double>& leader) {
    if (leader.first == 0) {
        return false;
    }
    // let's check it on highways only
    if (leader.first->getSpeed() < (80.0 / 3.6)) {
        return false;
    }
    return leader.second < myCarFollowModel.interactionGap(&myVehicle, leader.first->getSpeed());
}


bool
MSAbstractLaneChangeModel::startLaneChangeManeuver(MSLane* source, MSLane* target, int direction) {
    if (&source->getEdge() != &target->getEdge()) {
        changedToOpposite();
    }
    if (MSGlobals::gLaneChangeDuration > DELTA_T) {
        myLaneChangeCompletion = 0;
        myLaneChangeDirection = direction;
        setManeuverDist(target->getCenterOnEdge() - source->getCenterOnEdge());
        myVehicle.switchOffSignal(MSVehicle::VEH_SIGNAL_BLINKER_RIGHT | MSVehicle::VEH_SIGNAL_BLINKER_LEFT);
        myVehicle.switchOnSignal(direction == 1 ? MSVehicle::VEH_SIGNAL_BLINKER_LEFT : MSVehicle::VEH_SIGNAL_BLINKER_RIGHT);
        return true;
    } else {
        primaryLaneChanged(source, target, direction);
        return false;
    }
}


void
MSAbstractLaneChangeModel::primaryLaneChanged(MSLane* source, MSLane* target, int direction) {
    initLastLaneChangeOffset(direction);
    myVehicle.leaveLane(MSMoveReminder::NOTIFICATION_LANE_CHANGE, target);
    source->leftByLaneChange(&myVehicle);
    myVehicle.enterLaneAtLaneChange(target);
    target->enteredByLaneChange(&myVehicle);
    laneChangeOutput("change", source, target, direction);
    changed();
}

void
MSAbstractLaneChangeModel::laneChangeOutput(const std::string& tag, MSLane* source, MSLane* target, int direction) {
    if (myLCOutput) {
        OutputDevice& of = OutputDevice::getDeviceByOption("lanechange-output");
        of.openTag(tag);
        of.writeAttr(SUMO_ATTR_ID, myVehicle.getID());
        of.writeAttr(SUMO_ATTR_TYPE, myVehicle.getVehicleType().getID());
        of.writeAttr(SUMO_ATTR_TIME, time2string(MSNet::getInstance()->getCurrentTimeStep()));
        of.writeAttr(SUMO_ATTR_FROM, source->getID());
        of.writeAttr(SUMO_ATTR_TO, target->getID());
        of.writeAttr(SUMO_ATTR_DIR, direction);
        of.writeAttr(SUMO_ATTR_SPEED, myVehicle.getSpeed());
        of.writeAttr(SUMO_ATTR_POSITION, myVehicle.getPositionOnLane());
        of.writeAttr("reason", toString((LaneChangeAction)(myOwnState & ~(LCA_RIGHT | LCA_LEFT))));
        of.writeAttr("leaderGap", myLastLeaderGap == NO_NEIGHBOR ? "None" : toString(myLastLeaderGap));
        of.writeAttr("leaderSecureGap", myLastLeaderSecureGap == NO_NEIGHBOR ? "None" : toString(myLastLeaderSecureGap));
        of.writeAttr("followerGap", myLastFollowerGap == NO_NEIGHBOR ? "None" : toString(myLastFollowerGap));
        of.writeAttr("followerSecureGap", myLastFollowerSecureGap == NO_NEIGHBOR ? "None" : toString(myLastFollowerSecureGap));
        of.writeAttr("origLeaderGap", myLastOrigLeaderGap == NO_NEIGHBOR ? "None" : toString(myLastOrigLeaderGap));
        of.writeAttr("origLeaderSecureGap", myLastOrigLeaderSecureGap == NO_NEIGHBOR ? "None" : toString(myLastOrigLeaderSecureGap));
        if (MSGlobals::gLateralResolution > 0) {
            const double latGap = direction < 0 ? myLastLateralGapRight : myLastLateralGapLeft;
            of.writeAttr("latGap", latGap == NO_NEIGHBOR ? "None" : toString(latGap));
        }
        of.closeTag();
    }
    // Assure that the drive items are up to date (even if the following step is no actionstep for the vehicle).
    myVehicle.updateDriveItems();
}


double 
MSAbstractLaneChangeModel::computeSpeedLat(double /*latDist*/, double& maneuverDist) {
    if (myVehicle.getVehicleType().wasSet(VTYPEPARS_MAXSPEED_LAT_SET)) {
        int stepsToChange = (int)ceil(maneuverDist / SPEED2DIST(myVehicle.getVehicleType().getMaxSpeedLat()));
        return DIST2SPEED(maneuverDist / stepsToChange);
    } else {
        return maneuverDist / STEPS2TIME(MSGlobals::gLaneChangeDuration);
    }
}


bool
MSAbstractLaneChangeModel::updateCompletion() {
    const bool pastBefore = pastMidpoint();
    // not updated in the context of continous lane changing
    double maneuverDist = getManeuverDist();
    mySpeedLat = computeSpeedLat(0, maneuverDist);
    /*
    std::cout << SIMTIME << " veh=" << myVehicle.getID() 
        << " md=" << myManeuverDist
        << " mySpeedLat=" << mySpeedLat
        << " update1=" << (double)DELTA_T / (double)MSGlobals::gLaneChangeDuration
        << " update2=" << (SPEED2DIST(mySpeedLat) / myManeuverDist)
        << "\n";
    myLaneChangeCompletion += (double)DELTA_T / (double)MSGlobals::gLaneChangeDuration;
    */
    myLaneChangeCompletion += (SPEED2DIST(mySpeedLat) / myManeuverDist);
    return !pastBefore && pastMidpoint();
}


void
MSAbstractLaneChangeModel::endLaneChangeManeuver(const MSMoveReminder::Notification reason) {
    UNUSED_PARAMETER(reason);
    myLaneChangeCompletion = 1;
    cleanupShadowLane();
    cleanupTargetLane();
    myNoPartiallyOccupatedByShadow.clear();
    myVehicle.switchOffSignal(MSVehicle::VEH_SIGNAL_BLINKER_RIGHT | MSVehicle::VEH_SIGNAL_BLINKER_LEFT);
    myVehicle.fixPosition();
    if (myAmOpposite) {
        changedToOpposite();
    }
}


MSLane*
MSAbstractLaneChangeModel::getShadowLane(const MSLane* lane, double posLat) const {
    if (std::find(myNoPartiallyOccupatedByShadow.begin(), myNoPartiallyOccupatedByShadow.end(), lane) == myNoPartiallyOccupatedByShadow.end()) {
        // initialize shadow lane
        const double overlap = myVehicle.getLateralOverlap(posLat);
        if (debugVehicle()) {
            std::cout << SIMTIME << " veh=" << myVehicle.getID() << " posLat=" << posLat << " overlap=" << overlap << "\n";
        }
        if (overlap > NUMERICAL_EPS ||
                // "reserve" target lane even when there is no overlap yet
                (isChangingLanes() && myLaneChangeCompletion < 0.5)) {
            const int shadowDirection = posLat < 0 ? -1 : 1;
            return lane->getParallelLane(shadowDirection);
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}


MSLane*
MSAbstractLaneChangeModel::getShadowLane(const MSLane* lane) const {
    return getShadowLane(lane, myVehicle.getLateralPositionOnLane());
}


void
MSAbstractLaneChangeModel::cleanupShadowLane() {
    if (myShadowLane != 0) {
        if (debugVehicle()) {
            std::cout << SIMTIME << " cleanupShadowLane\n";
        }
        myShadowLane->resetPartialOccupation(&myVehicle);
        myShadowLane = 0;
    }
    for (std::vector<MSLane*>::const_iterator it = myShadowFurtherLanes.begin(); it != myShadowFurtherLanes.end(); ++it) {
        if (debugVehicle()) {
            std::cout << SIMTIME << " cleanupShadowLane2\n";
        }
        (*it)->resetPartialOccupation(&myVehicle);
    }
    myShadowFurtherLanes.clear();
    myNoPartiallyOccupatedByShadow.clear();
}

void
MSAbstractLaneChangeModel::cleanupTargetLane() {
    if (myTargetLane != 0) {
        if (debugVehicle()) {
            std::cout << SIMTIME << " cleanupTargetLane\n";
        }
        myTargetLane->resetManeuverReservation(&myVehicle);
        myTargetLane = 0;
    }
    for (std::vector<MSLane*>::const_iterator it = myFurtherTargetLanes.begin(); it != myFurtherTargetLanes.end(); ++it) {
        if (debugVehicle()) {
            std::cout << SIMTIME << " cleanupTargetLane\n";
        }
        if (*it != nullptr) {
            (*it)->resetManeuverReservation(&myVehicle);
        }
    }
    myFurtherTargetLanes.clear();
//    myNoPartiallyOccupatedByShadow.clear();
}


bool
MSAbstractLaneChangeModel::cancelRequest(int state) {
    int ret = myVehicle.influenceChangeDecision(state);
    return ret != state;
}


void
MSAbstractLaneChangeModel::initLastLaneChangeOffset(int dir) {
    if (dir > 0) {
        myLastLaneChangeOffset = 1;
    } else if (dir < 0) {
        myLastLaneChangeOffset = -1;
    }
}

void
MSAbstractLaneChangeModel::updateShadowLane() {
    if (myShadowLane != 0) {
        if (debugVehicle()) {
            std::cout << SIMTIME << " updateShadowLane()\n";
        }
        myShadowLane->resetPartialOccupation(&myVehicle);
    }
    myShadowLane = getShadowLane(myVehicle.getLane());
    std::vector<MSLane*> passed;
    if (myShadowLane != 0) {
        myShadowLane->setPartialOccupation(&myVehicle);
        const std::vector<MSLane*>& further = myVehicle.getFurtherLanes();
        const std::vector<double>& furtherPosLat = myVehicle.getFurtherLanesPosLat();
        assert(further.size() == furtherPosLat.size());
        passed.push_back(myShadowLane);
        for (int i = 0; i < (int)further.size(); ++i) {
            MSLane* shadowFurther = getShadowLane(further[i], furtherPosLat[i]);
            if (debugVehicle()) {
                std::cout << SIMTIME << "   further=" << further[i]->getID() << " (posLat=" << furtherPosLat[i] << ") shadowFurther=" << Named::getIDSecure(shadowFurther) << "\n";
            }
            if (shadowFurther != 0 && MSLinkContHelper::getConnectingLink(*shadowFurther, *passed.back()) != 0) {
                passed.push_back(shadowFurther);
            }
        }
        std::reverse(passed.begin(), passed.end());
    } else {
        if (isChangingLanes() && myVehicle.getLateralOverlap() > NUMERICAL_EPS) {
            WRITE_WARNING("Vehicle '" + myVehicle.getID() + "' could not finish continuous lane change (lane disappeared) time=" +
                          time2string(MSNet::getInstance()->getCurrentTimeStep()) + ".");
            endLaneChangeManeuver();
        }
    }
    if (debugVehicle()) {
        std::cout << SIMTIME << " updateShadowLane() veh=" << myVehicle.getID()
                  << " newShadowLane=" << Named::getIDSecure(myShadowLane)
                  << "\n   before:" << " myShadowFurtherLanes=" << toString(myShadowFurtherLanes) << " further=" << toString(myVehicle.getFurtherLanes()) << " passed=" << toString(passed);
        std::cout << std::endl;
    }
    myVehicle.updateFurtherLanes(myShadowFurtherLanes, myShadowFurtherLanesPosLat, passed);
    if (debugVehicle()) std::cout
                << "\n   after:" << " myShadowFurtherLanes=" << toString(myShadowFurtherLanes) << "\n";
}


int
MSAbstractLaneChangeModel::getShadowDirection() const {
    if (isChangingLanes()) {
        if (pastMidpoint()) {
            return -myLaneChangeDirection;
        } else {
            return myLaneChangeDirection;
        }
    } else if (myShadowLane == 0) {
        return 0;
    } else {
        assert(&myShadowLane->getEdge() == &myVehicle.getLane()->getEdge());
        return myShadowLane->getIndex() - myVehicle.getLane()->getIndex();
    }
}


void
MSAbstractLaneChangeModel::updateTargetLane() {
#ifdef DEBUG_TARGET_LANE
    MSLane* oldTarget = myTargetLane;
    std::vector<MSLane*> oldFurtherTargets = myFurtherTargetLanes;
    if (debugVehicle()) {
        std::cout << SIMTIME << " veh '" << myVehicle.getID() << "' (lane=" << myVehicle.getLane()->getID() << ") updateTargetLane()"
                  << "\n   oldTarget: " << (oldTarget == nullptr ? "NULL" : oldTarget->getID())
                  << " oldFurtherTargets: " << toString(oldFurtherTargets);
    }
#endif
    if (myTargetLane != nullptr) {
        myTargetLane->resetManeuverReservation(&myVehicle);
    }
    // Clear old further target lanes
    for (MSLane* oldTargetLane : myFurtherTargetLanes) {
        if (oldTargetLane != nullptr) {
            oldTargetLane->resetManeuverReservation(&myVehicle);
        }
    }
    myFurtherTargetLanes.clear();

    // Get new target lanes and issue a maneuver reservation.
    int targetDir;
    myTargetLane = determineTargetLane(targetDir);
    if (myTargetLane != nullptr) {
        myTargetLane->setManeuverReservation(&myVehicle);
        // further targets are just the target lanes corresponding to the vehicle's further lanes
        // @note In a neglectable amount of situations we might add a reservation for a shadow further lane.
        for (MSLane* furtherLane : myVehicle.getFurtherLanes()) {
            MSLane* furtherTargetLane = furtherLane->getParallelLane(targetDir);
            myFurtherTargetLanes.push_back(furtherTargetLane);
            if (furtherTargetLane != nullptr) {
                furtherTargetLane->setManeuverReservation(&myVehicle);
            }
        }
    }
#ifdef DEBUG_TARGET_LANE
    if (debugVehicle()) {
        std::cout << "\n   newTarget (offset=" << targetDir << "): " << (myTargetLane == nullptr ? "NULL" : myTargetLane->getID())
                  << " newFurtherTargets: " << toString(myFurtherTargetLanes)
                  << std::endl;
    }
#endif
}


MSLane*
MSAbstractLaneChangeModel::determineTargetLane(int& targetDir) const {
    targetDir = 0;
    if (myManeuverDist == 0) {
        return nullptr;
    }
    // Current lateral boundaries of the vehicle
    const double vehRight = myVehicle.getLateralPositionOnLane() - 0.5 * myVehicle.getWidth();
    const double vehLeft = myVehicle.getLateralPositionOnLane() + 0.5 * myVehicle.getWidth();
    const double halfLaneWidth = 0.5 * myVehicle.getLane()->getWidth();

    if (vehRight + myManeuverDist < -halfLaneWidth) {
        // Vehicle intends to traverse the right lane boundary
        targetDir = -1;
    } else if (vehLeft + myManeuverDist > halfLaneWidth) {
        // Vehicle intends to traverse the left lane boundary
        targetDir = 1;
    }
    if (targetDir == 0) {
        // Presently, no maneuvering into another lane is begun.
        return nullptr;
    }
    MSLane* target = myVehicle.getLane()->getParallelLane(targetDir);
    if (target == nullptr || target == myShadowLane) {
        return nullptr;
    } else {
        return target;
    }
}



double
MSAbstractLaneChangeModel::getAngleOffset() const {
    const double angleOffset = 60 / STEPS2TIME(MSGlobals::gLaneChangeDuration) * (pastMidpoint() ? 1 - myLaneChangeCompletion : myLaneChangeCompletion);
    return myLaneChangeDirection * angleOffset;
}


SUMOTime
MSAbstractLaneChangeModel::remainingTime() const {
    return (SUMOTime)((1. - myLaneChangeCompletion) * MSGlobals::gLaneChangeDuration);
}


void
MSAbstractLaneChangeModel::setShadowApproachingInformation(MSLink* link) const {
    //std::cout << SIMTIME << " veh=" << myVehicle.getID() << " @=" << &myVehicle << " set shadow approaching=" << link->getViaLaneOrLane()->getID() << "\n";
    myApproachedByShadow.push_back(link);
}

void
MSAbstractLaneChangeModel::removeShadowApproachingInformation() const {
    for (std::vector<MSLink*>::iterator it = myApproachedByShadow.begin(); it != myApproachedByShadow.end(); ++it) {
        //std::cout << SIMTIME << " veh=" << myVehicle.getID() << " @=" << &myVehicle << " remove shadow approaching=" << (*it)->getViaLaneOrLane()->getID() << "\n";
        (*it)->removeApproaching(&myVehicle);
    }
    myApproachedByShadow.clear();
}



void
MSAbstractLaneChangeModel::checkTraCICommands() {
    int newstate = myVehicle.influenceChangeDecision(myOwnState);
    int oldstate = myVehicle.getLaneChangeModel().getOwnState();
    if (myOwnState != newstate) {
        if (MSGlobals::gLateralResolution > 0.) {
            // Calculate and set the lateral maneuver distance corresponding to the change request
            // to induce a corresponding sublane change.
            const int dir = (newstate & LCA_RIGHT) != 0 ? -1 : ((newstate & LCA_LEFT) != 0 ? 1 : 0);
            // minimum distance to move the vehicle fully onto the lane at offset dir
            const double latLaneDist = myVehicle.lateralDistanceToLane(dir);
            if ((newstate & LCA_TRACI) != 0) {
                if ((newstate & LCA_STAY) != 0) {
                    setManeuverDist(0.);
                } else if (((newstate & LCA_RIGHT) != 0 && dir < 0)
                           || ((newstate & LCA_LEFT) != 0 && dir > 0)) {
                    setManeuverDist(latLaneDist);
                }
            }
            if (myVehicle.hasInfluencer()) {
                // lane change requests override sublane change requests
                myVehicle.getInfluencer().resetLatDist();
            }

        }
        setOwnState(newstate);
    } else {
        // Check for sublane change requests
        if (myVehicle.hasInfluencer() && myVehicle.getInfluencer().getLatDist() != 0) {
            const double maneuverDist = myVehicle.getInfluencer().getLatDist();
            myVehicle.getLaneChangeModel().setManeuverDist(maneuverDist);
            myVehicle.getInfluencer().resetLatDist();
            newstate |= LCA_TRACI;
            if (myOwnState != newstate) {
                setOwnState(newstate);
            }
            if (gDebugFlag2) {
                std::cout << "     traci influenced maneuverDist=" << maneuverDist << "\n";
            }
        }
    }
    if (DEBUG_COND) {
        std::cout << SIMTIME << " veh=" << myVehicle.getID() << " stateAfterTraCI=" << toString((LaneChangeAction)newstate) << " original=" << toString((LaneChangeAction)oldstate) << "\n";
    }
}

void
MSAbstractLaneChangeModel::changedToOpposite() {
    myAmOpposite = !myAmOpposite;
    myAlreadyChanged = true;
}

void
MSAbstractLaneChangeModel::setFollowerGaps(CLeaderDist follower, double secGap)  {
    if (follower.first != 0) {
        myLastFollowerGap = follower.second + follower.first->getVehicleType().getMinGap();
        myLastFollowerSecureGap = secGap;
    }
}

void
MSAbstractLaneChangeModel::setLeaderGaps(CLeaderDist leader, double secGap) {
    if (leader.first != 0) {
        myLastLeaderGap = leader.second + myVehicle.getVehicleType().getMinGap();
        myLastLeaderSecureGap = secGap;
    }
}

void
MSAbstractLaneChangeModel::setOrigLeaderGaps(CLeaderDist leader, double secGap) {
    if (leader.first != 0) {
        myLastOrigLeaderGap = leader.second + myVehicle.getVehicleType().getMinGap();
        myLastOrigLeaderSecureGap = secGap;
    }
}

void
MSAbstractLaneChangeModel::setFollowerGaps(const MSLeaderDistanceInfo& vehicles) {
    int rightmost;
    int leftmost;
    vehicles.getSubLanes(&myVehicle, 0, rightmost, leftmost);
    for (int i = rightmost; i <= leftmost; ++i) {
        CLeaderDist vehDist = vehicles[i];
        if (vehDist.first != 0) {
            const MSVehicle* leader = &myVehicle;
            const MSVehicle* follower = vehDist.first;
            const double netGap = vehDist.second + follower->getVehicleType().getMinGap();
            if (netGap < myLastFollowerGap && netGap >= 0) {
                myLastFollowerGap = netGap;
                myLastFollowerSecureGap = follower->getCarFollowModel().getSecureGap(follower->getSpeed(), leader->getSpeed(), leader->getCarFollowModel().getMaxDecel());
            }
        }
    }
}

void
MSAbstractLaneChangeModel::setLeaderGaps(const MSLeaderDistanceInfo& vehicles) {
    int rightmost;
    int leftmost;
    vehicles.getSubLanes(&myVehicle, 0, rightmost, leftmost);
    for (int i = rightmost; i <= leftmost; ++i) {
        CLeaderDist vehDist = vehicles[i];
        if (vehDist.first != 0) {
            const MSVehicle* leader = vehDist.first;
            const MSVehicle* follower = &myVehicle;
            const double netGap = vehDist.second + follower->getVehicleType().getMinGap();
            if (netGap < myLastLeaderGap && netGap >= 0) {
                myLastLeaderGap = netGap;
                myLastLeaderSecureGap = follower->getCarFollowModel().getSecureGap(follower->getSpeed(), leader->getSpeed(), leader->getCarFollowModel().getMaxDecel());
            }
        }
    }
}

void
MSAbstractLaneChangeModel::setOrigLeaderGaps(const MSLeaderDistanceInfo& vehicles) {
    int rightmost;
    int leftmost;
    vehicles.getSubLanes(&myVehicle, 0, rightmost, leftmost);
    for (int i = rightmost; i <= leftmost; ++i) {
        CLeaderDist vehDist = vehicles[i];
        if (vehDist.first != 0) {
            const MSVehicle* leader = vehDist.first;
            const MSVehicle* follower = &myVehicle;
            const double netGap = vehDist.second + follower->getVehicleType().getMinGap();
            if (netGap < myLastOrigLeaderGap && netGap >= 0) {
                myLastOrigLeaderGap = netGap;
                myLastOrigLeaderSecureGap = follower->getCarFollowModel().getSecureGap(follower->getSpeed(), leader->getSpeed(), leader->getCarFollowModel().getMaxDecel());
            }
        }
    }
}
