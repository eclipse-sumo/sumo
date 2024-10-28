/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    MSAbstractLaneChangeModel.cpp
/// @author  Daniel Krajzewicz
/// @author  Friedemann Wesner
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @author  Leonhard Luecken
/// @date    Fri, 29.04.2005
///
// Interface for lane-change models
/****************************************************************************/

// ===========================================================================
// DEBUG
// ===========================================================================
//#define DEBUG_TARGET_LANE
//#define DEBUG_SHADOWLANE
//#define DEBUG_OPPOSITE
//#define DEBUG_MANEUVER
#define DEBUG_COND (myVehicle.isSelected())

#include <config.h>

#include <utils/options/OptionsCont.h>
#include <utils/xml/SUMOSAXAttributes.h>
#include <utils/geom/GeomHelper.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSLink.h>
#include <microsim/MSStop.h>
#include <microsim/MSDriverState.h>
#include <microsim/MSGlobals.h>
#include <microsim/devices/MSDevice_Bluelight.h>
#include "MSLCM_DK2008.h"
#include "MSLCM_LC2013.h"
#include "MSLCM_LC2013_CC.h"
#include "MSLCM_SL2015.h"
#include "MSAbstractLaneChangeModel.h"

/* -------------------------------------------------------------------------
 * static members
 * ----------------------------------------------------------------------- */
bool MSAbstractLaneChangeModel::myAllowOvertakingRight(false);
bool MSAbstractLaneChangeModel::myLCOutput(false);
bool MSAbstractLaneChangeModel::myLCStartedOutput(false);
bool MSAbstractLaneChangeModel::myLCEndedOutput(false);
bool MSAbstractLaneChangeModel::myLCXYOutput(false);
const double MSAbstractLaneChangeModel::NO_NEIGHBOR(std::numeric_limits<double>::max());

#define LC_ASSUMED_DECEL 1.0 // the minimal constant deceleration assumed to estimate the duration of a continuous lane-change at its initiation.

/* -------------------------------------------------------------------------
 * MSAbstractLaneChangeModel-methods
 * ----------------------------------------------------------------------- */

void
MSAbstractLaneChangeModel::initGlobalOptions(const OptionsCont& oc) {
    myAllowOvertakingRight = oc.getBool("lanechange.overtake-right");
    myLCOutput = oc.isSet("lanechange-output");
    myLCStartedOutput = oc.getBool("lanechange-output.started");
    myLCEndedOutput = oc.getBool("lanechange-output.ended");
    myLCXYOutput = oc.getBool("lanechange-output.xy");
}


MSAbstractLaneChangeModel*
MSAbstractLaneChangeModel::build(LaneChangeModel lcm, MSVehicle& v) {
    if (MSGlobals::gLateralResolution > 0 && lcm != LaneChangeModel::SL2015 && lcm != LaneChangeModel::DEFAULT) {
        throw ProcessError(TLF("Lane change model '%' is not compatible with sublane simulation", toString(lcm)));
    }
    switch (lcm) {
        case LaneChangeModel::DK2008:
            return new MSLCM_DK2008(v);
        case LaneChangeModel::LC2013:
            return new MSLCM_LC2013(v);
        case LaneChangeModel::LC2013_CC:
            return new MSLCM_LC2013_CC(v);
        case LaneChangeModel::SL2015:
            return new MSLCM_SL2015(v);
        case LaneChangeModel::DEFAULT:
            if (MSGlobals::gLateralResolution <= 0) {
                return new MSLCM_LC2013(v);
            } else {
                return new MSLCM_SL2015(v);
            }
        default:
            throw ProcessError(TLF("Lane change model '%' not implemented", toString(lcm)));
    }
}


MSAbstractLaneChangeModel::MSAbstractLaneChangeModel(MSVehicle& v, const LaneChangeModel model) :
    myVehicle(v),
    myOwnState(0),
    myPreviousState(0),
    myPreviousState2(0),
    myCanceledStateRight(LCA_NONE),
    myCanceledStateCenter(LCA_NONE),
    myCanceledStateLeft(LCA_NONE),
    mySpeedLat(0),
    myAccelerationLat(0),
    myAngleOffset(0),
    myPreviousAngleOffset(0),
    myCommittedSpeed(0),
    myLaneChangeCompletion(1.0),
    myLaneChangeDirection(0),
    myAlreadyChanged(false),
    myShadowLane(nullptr),
    myTargetLane(nullptr),
    myModel(model),
    myLastLateralGapLeft(0.),
    myLastLateralGapRight(0.),
    myLastLeaderGap(0.),
    myLastFollowerGap(0.),
    myLastLeaderSecureGap(0.),
    myLastFollowerSecureGap(0.),
    myLastOrigLeaderGap(0.),
    myLastOrigLeaderSecureGap(0.),
    myLastLeaderSpeed(0),
    myLastFollowerSpeed(0),
    myLastOrigLeaderSpeed(0),
    myDontResetLCGaps(false),
    myMaxSpeedLatStanding(v.getVehicleType().getParameter().getLCParam(SUMO_ATTR_LCA_MAXSPEEDLATSTANDING, v.getVehicleType().getMaxSpeedLat())),
    myMaxSpeedLatFactor(v.getVehicleType().getParameter().getLCParam(SUMO_ATTR_LCA_MAXSPEEDLATFACTOR, 1)),
    myMaxDistLatStanding(v.getVehicleType().getParameter().getLCParam(SUMO_ATTR_LCA_MAXDISTLATSTANDING,
                         // prevent lateral sliding for cars but permit for two-wheelers due to better maneuverability
                         (v.getVClass() & (SVC_BICYCLE | SVC_MOTORCYCLE | SVC_MOPED)) != 0 ? std::numeric_limits<double>::max() : 1.6)),
    mySigma(v.getVehicleType().getParameter().getLCParam(SUMO_ATTR_LCA_SIGMA, 0.0)),
    myOvertakeRightParam(v.getVehicleType().getParameter().getLCParam(SUMO_ATTR_LCA_OVERTAKE_RIGHT, 0)),
    myHaveBlueLight(v.getDevice(typeid(MSDevice_Bluelight)) != nullptr), // see MSVehicle::initDevices
    myLastLaneChangeOffset(0),
    myAmOpposite(false),
    myManeuverDist(0.),
    myPreviousManeuverDist(0.) {
    saveLCState(-1, LCA_UNKNOWN, LCA_UNKNOWN);
    saveLCState(0, LCA_UNKNOWN, LCA_UNKNOWN);
    saveLCState(1, LCA_UNKNOWN, LCA_UNKNOWN);
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
    if (DEBUG_COND) {
        std::cout << SIMTIME
                  << " veh=" << myVehicle.getID()
                  << " setManeuverDist() old=" << myManeuverDist << " new=" << dist
                  << std::endl;
    }
#endif
    myManeuverDist = fabs(dist) < NUMERICAL_EPS ? 0. : dist;
    // store value which may be modified by the model during the next step
    myPreviousManeuverDist = myManeuverDist;
}


double
MSAbstractLaneChangeModel::getManeuverDist() const {
    return myManeuverDist;
}

double
MSAbstractLaneChangeModel::getPreviousManeuverDist() const {
    return myPreviousManeuverDist;
}

void
MSAbstractLaneChangeModel::saveNeighbors(const int dir, const MSLeaderDistanceInfo& followers, const MSLeaderDistanceInfo& leaders) {
    if (dir == -1) {
        myLeftFollowers = std::make_shared<MSLeaderDistanceInfo>(followers);
        myLeftLeaders = std::make_shared<MSLeaderDistanceInfo>(leaders);
    } else if (dir == 1) {
        myRightFollowers = std::make_shared<MSLeaderDistanceInfo>(followers);
        myRightLeaders = std::make_shared<MSLeaderDistanceInfo>(leaders);
    } else {
        // dir \in {-1,1} !
        assert(false);
    }
}


void
MSAbstractLaneChangeModel::saveNeighbors(const int dir, const std::pair<MSVehicle* const, double>& follower, const std::pair<MSVehicle* const, double>& leader) {
    if (dir == -1) {
        myLeftFollowers = std::make_shared<MSLeaderDistanceInfo>(follower, myVehicle.getLane()->getWidth());
        myLeftLeaders = std::make_shared<MSLeaderDistanceInfo>(leader, myVehicle.getLane()->getWidth());
    } else if (dir == 1) {
        myRightFollowers = std::make_shared<MSLeaderDistanceInfo>(follower, myVehicle.getLane()->getWidth());
        myRightLeaders = std::make_shared<MSLeaderDistanceInfo>(leader, myVehicle.getLane()->getWidth());
    } else {
        // dir \in {-1,1} !
        assert(false);
    }
}


void
MSAbstractLaneChangeModel::clearNeighbors() {
    myLeftFollowers = nullptr;
    myLeftLeaders = nullptr;
    myRightFollowers = nullptr;
    myRightLeaders = nullptr;
}


const std::shared_ptr<MSLeaderDistanceInfo>
MSAbstractLaneChangeModel::getFollowers(const int dir) {
    if (dir == -1) {
        return myLeftFollowers;
    } else if (dir == 1) {
        return myRightFollowers;
    } else {
        // dir \in {-1,1} !
        assert(false);
    }
    return nullptr;
}

const std::shared_ptr<MSLeaderDistanceInfo>
MSAbstractLaneChangeModel::getLeaders(const int dir) {
    if (dir == -1) {
        return myLeftLeaders;
    } else if (dir == 1) {
        return myRightLeaders;
    } else {
        // dir \in {-1,1} !
        assert(false);
    }
    return nullptr;
}


bool
MSAbstractLaneChangeModel::congested(const MSVehicle* const neighLeader) {
    if (neighLeader == nullptr) {
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
MSAbstractLaneChangeModel::avoidOvertakeRight() const {
    return (!myAllowOvertakingRight
            && !myVehicle.congested()
            && myVehicle.getVehicleType().getVehicleClass() != SVC_EMERGENCY
            && (myOvertakeRightParam == 0 || myOvertakeRightParam < RandHelper::rand(myVehicle.getRNG())));
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
    return leader.second < getCarFollowModel().interactionGap(&myVehicle, leader.first->getSpeed());
}


bool
MSAbstractLaneChangeModel::startLaneChangeManeuver(MSLane* source, MSLane* target, int direction) {
    if (MSGlobals::gLaneChangeDuration > DELTA_T) {
        myLaneChangeCompletion = 0;
        myLaneChangeDirection = direction;
        setManeuverDist((target->getWidth() + source->getWidth()) * 0.5 * direction);
        myVehicle.switchOffSignal(MSVehicle::VEH_SIGNAL_BLINKER_RIGHT | MSVehicle::VEH_SIGNAL_BLINKER_LEFT);
        myVehicle.switchOnSignal(((direction == 1) != MSGlobals::gLefthand) ? MSVehicle::VEH_SIGNAL_BLINKER_LEFT : MSVehicle::VEH_SIGNAL_BLINKER_RIGHT);
        if (myLCOutput) {
            memorizeGapsAtLCInit();
        }
        return true;
    } else {
        primaryLaneChanged(source, target, direction);
        return false;
    }
}

void
MSAbstractLaneChangeModel::memorizeGapsAtLCInit() {
    myDontResetLCGaps = true;
}

void
MSAbstractLaneChangeModel::clearGapsAtLCInit() {
    myDontResetLCGaps = false;
}

void
MSAbstractLaneChangeModel::primaryLaneChanged(MSLane* source, MSLane* target, int direction) {
    initLastLaneChangeOffset(direction);
    myVehicle.leaveLane(MSMoveReminder::NOTIFICATION_LANE_CHANGE, target);
    source->leftByLaneChange(&myVehicle);
    laneChangeOutput("change", source, target, direction); // record position on the source edge in case of opposite change
    if (&source->getEdge() != &target->getEdge()) {
        changedToOpposite();
#ifdef DEBUG_OPPOSITE
        if (debugVehicle()) {
            std::cout << SIMTIME << " veh=" << myVehicle.getID() << " primaryLaneChanged source=" << source->getID() << " target=" << target->getID() << " nowOpposite=" << myAmOpposite << "\n";
        }
#endif
        myVehicle.setTentativeLaneAndPosition(target, source->getOppositePos(myVehicle.getPositionOnLane()), -myVehicle.getLateralPositionOnLane());
        target->forceVehicleInsertion(&myVehicle, myVehicle.getPositionOnLane(), MSMoveReminder::NOTIFICATION_LANE_CHANGE, myVehicle.getLateralPositionOnLane());
    } else if (myAmOpposite) {
#ifdef DEBUG_OPPOSITE
        if (debugVehicle()) {
            std::cout << SIMTIME << " veh=" << myVehicle.getID() << " primaryLaneChanged source=" << source->getID() << " target=" << target->getID() << " stayOpposite\n";
        }
#endif
        myAlreadyChanged = true;
        myVehicle.setTentativeLaneAndPosition(target, myVehicle.getPositionOnLane(), myVehicle.getLateralPositionOnLane());
        if (!MSGlobals::gSublane) {
            // in the continous case, the vehicle is added to the target lane via MSLaneChanger::continueChange / registerHop
            // in the sublane case, the vehicle is added to the target lane via MSLaneChangerSublane::checkChangeOppositeSublane / MSLane::myTmpVehicles
            target->forceVehicleInsertion(&myVehicle, myVehicle.getPositionOnLane(), MSMoveReminder::NOTIFICATION_LANE_CHANGE, myVehicle.getLateralPositionOnLane());
        }
    } else {
        myVehicle.enterLaneAtLaneChange(target);
        target->enteredByLaneChange(&myVehicle);
    }
    // Assure that the drive items are up to date (even if the following step is no actionstep for the vehicle).
    // This is necessary because the lane advance uses the target lane from the corresponding drive item.
    myVehicle.updateDriveItems();
    changed();
}

void
MSAbstractLaneChangeModel::laneChangeOutput(const std::string& tag, MSLane* source, MSLane* target, int direction, double maneuverDist) {
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
        of.writeAttr("reason", toString((LaneChangeAction)(myOwnState & ~(
                                            LCA_RIGHT | LCA_LEFT
                                            | LCA_AMBLOCKINGLEADER | LCA_AMBLOCKINGFOLLOWER
                                            | LCA_MRIGHT | LCA_MLEFT
                                            | LCA_AMBACKBLOCKER | LCA_AMBACKBLOCKER_STANDING))) + myVehicle.getParameter().getParameter("lcReason"));
        of.writeAttr("leaderGap", myLastLeaderGap == NO_NEIGHBOR ? "None" : toString(myLastLeaderGap));
        of.writeAttr("leaderSecureGap", myLastLeaderSecureGap == NO_NEIGHBOR ? "None" : toString(myLastLeaderSecureGap));
        of.writeAttr("leaderSpeed", myLastLeaderSpeed == NO_NEIGHBOR ? "None" : toString(myLastLeaderSpeed));
        of.writeAttr("followerGap", myLastFollowerGap == NO_NEIGHBOR ? "None" : toString(myLastFollowerGap));
        of.writeAttr("followerSecureGap", myLastFollowerSecureGap == NO_NEIGHBOR ? "None" : toString(myLastFollowerSecureGap));
        of.writeAttr("followerSpeed", myLastFollowerSpeed == NO_NEIGHBOR ? "None" : toString(myLastFollowerSpeed));
        of.writeAttr("origLeaderGap", myLastOrigLeaderGap == NO_NEIGHBOR ? "None" : toString(myLastOrigLeaderGap));
        of.writeAttr("origLeaderSecureGap", myLastOrigLeaderSecureGap == NO_NEIGHBOR ? "None" : toString(myLastOrigLeaderSecureGap));
        of.writeAttr("origLeaderSpeed", myLastOrigLeaderSpeed == NO_NEIGHBOR ? "None" : toString(myLastOrigLeaderSpeed));
        if (MSGlobals::gLateralResolution > 0) {
            const double latGap = direction < 0 ? myLastLateralGapRight : myLastLateralGapLeft;
            of.writeAttr("latGap", latGap == NO_NEIGHBOR ? "None" : toString(latGap));
            if (maneuverDist != 0) {
                of.writeAttr("maneuverDistance", toString(maneuverDist));
            }
        }
        if (myLCXYOutput) {
            of.writeAttr(SUMO_ATTR_X, myVehicle.getPosition().x());
            of.writeAttr(SUMO_ATTR_Y, myVehicle.getPosition().y());
        }
        of.closeTag();
        if (MSGlobals::gLaneChangeDuration > DELTA_T) {
            clearGapsAtLCInit();
        }
    }
}


double
MSAbstractLaneChangeModel::computeSpeedLat(double /*latDist*/, double& maneuverDist, bool /*urgent*/) const {
    if (myVehicle.getVehicleType().wasSet(VTYPEPARS_MAXSPEED_LAT_SET)) {
        int stepsToChange = (int)ceil(fabs(maneuverDist) / SPEED2DIST(myVehicle.getVehicleType().getMaxSpeedLat()));
        return DIST2SPEED(maneuverDist / stepsToChange);
    } else {
        return maneuverDist / STEPS2TIME(MSGlobals::gLaneChangeDuration);
    }
}


double
MSAbstractLaneChangeModel::getAssumedDecelForLaneChangeDuration() const {
    return MAX2(LC_ASSUMED_DECEL, -myVehicle.getAcceleration());
}

void
MSAbstractLaneChangeModel::setSpeedLat(double speedLat) {
    myAccelerationLat = SPEED2ACCEL(speedLat - mySpeedLat);
    mySpeedLat = speedLat;
}


void
MSAbstractLaneChangeModel::resetSpeedLat() {
    if (MSGlobals::gLaneChangeDuration > 0 && !isChangingLanes()) {
        setSpeedLat(0);
    }
}


bool
MSAbstractLaneChangeModel::updateCompletion() {
    const bool pastBefore = pastMidpoint();
    // maneuverDist is not updated in the context of continuous lane changing but represents the full LC distance
    double maneuverDist = getManeuverDist();
    setSpeedLat(computeSpeedLat(0, maneuverDist, (myOwnState & LCA_URGENT) != 0));
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
    if (myAmOpposite && reason != MSMoveReminder::NOTIFICATION_LANE_CHANGE) {
        if (reason == MSMoveReminder::NOTIFICATION_PARKING && myVehicle.getNextStop().isOpposite) {
            // opposite driving continues after parking
        } else {
            // aborted maneuver
#ifdef DEBUG_OPPOSITE
            if (debugVehicle()) {
                std::cout << SIMTIME << " veh=" << myVehicle.getID() << " aborted maneuver (no longer opposite)\n";
            }
#endif
            changedToOpposite();
        }
    }
}


MSLane*
MSAbstractLaneChangeModel::getShadowLane(const MSLane* lane, double posLat) const {
    if (std::find(myNoPartiallyOccupatedByShadow.begin(), myNoPartiallyOccupatedByShadow.end(), lane) == myNoPartiallyOccupatedByShadow.end()) {
        // initialize shadow lane
        const double overlap = myVehicle.getLateralOverlap(posLat, lane);
#ifdef DEBUG_SHADOWLANE
        if (debugVehicle()) {
            std::cout << SIMTIME << " veh=" << myVehicle.getID() << " posLat=" << posLat << " overlap=" << overlap << "\n";
        }
#endif
        if (myAmOpposite) {
            // return the neigh-lane in forward direction
            return lane->getParallelLane(1);
        } else if (overlap > NUMERICAL_EPS) {
            const int shadowDirection = posLat < 0 ? -1 : 1;
            return lane->getParallelLane(shadowDirection);
        } else if (isChangingLanes() && myLaneChangeCompletion < 0.5) {
            // "reserve" target lane even when there is no overlap yet
            return lane->getParallelLane(myLaneChangeDirection);
        } else {
            return nullptr;
        }
    } else {
        return nullptr;
    }
}


MSLane*
MSAbstractLaneChangeModel::getShadowLane(const MSLane* lane) const {
    return getShadowLane(lane, myVehicle.getLateralPositionOnLane());
}


void
MSAbstractLaneChangeModel::cleanupShadowLane() {
    if (myShadowLane != nullptr) {
        if (debugVehicle()) {
            std::cout << SIMTIME << " cleanupShadowLane\n";
        }
        myShadowLane->resetPartialOccupation(&myVehicle);
        myShadowLane = nullptr;
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
    if (myTargetLane != nullptr) {
        if (debugVehicle()) {
            std::cout << SIMTIME << " cleanupTargetLane\n";
        }
        myTargetLane->resetManeuverReservation(&myVehicle);
        myTargetLane = nullptr;
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
MSAbstractLaneChangeModel::cancelRequest(int state, int laneOffset) {
    // store request before canceling
    getCanceledState(laneOffset) |= state;
    int ret = myVehicle.influenceChangeDecision(state);
    return ret != state;
}

double
MSAbstractLaneChangeModel::getMaxSpeedLat2() const {
    return MAX2(myVehicle.getVehicleType().getMaxSpeedLat(), myMaxSpeedLatStanding);
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
    if (!MSGlobals::gSublane) {
        // assume each vehicle drives at the center of its lane and act as if it fits
        return;
    }
    if (myShadowLane != nullptr) {
#ifdef DEBUG_SHADOWLANE
        if (debugVehicle()) {
            std::cout << SIMTIME << " updateShadowLane()\n";
        }
#endif
        myShadowLane->resetPartialOccupation(&myVehicle);
    }
    myShadowLane = getShadowLane(myVehicle.getLane());
    std::vector<MSLane*> passed;
    if (myShadowLane != nullptr) {
        myShadowLane->setPartialOccupation(&myVehicle);
        const std::vector<MSLane*>& further = myVehicle.getFurtherLanes();
        if (myAmOpposite) {
            assert(further.size() == 0);
        } else {
            const std::vector<double>& furtherPosLat = myVehicle.getFurtherLanesPosLat();
            assert(further.size() == furtherPosLat.size());
            passed.push_back(myShadowLane);
            for (int i = 0; i < (int)further.size(); ++i) {
                MSLane* shadowFurther = getShadowLane(further[i], furtherPosLat[i]);
#ifdef DEBUG_SHADOWLANE
                if (debugVehicle()) {
                    std::cout << SIMTIME << "   further=" << further[i]->getID() << " (posLat=" << furtherPosLat[i] << ") shadowFurther=" << Named::getIDSecure(shadowFurther) << "\n";
                }
#endif
                if (shadowFurther != nullptr && shadowFurther->getLinkTo(passed.back()) != nullptr) {
                    passed.push_back(shadowFurther);
                }
            }
            std::reverse(passed.begin(), passed.end());
        }
    } else {
        if (isChangingLanes() && myVehicle.getLateralOverlap() > NUMERICAL_EPS) {
            WRITE_WARNING("Vehicle '" + myVehicle.getID() + "' could not finish continuous lane change (lane disappeared) time=" +
                          time2string(MSNet::getInstance()->getCurrentTimeStep()) + ".");
            endLaneChangeManeuver();
        }
    }
#ifdef DEBUG_SHADOWLANE
    if (debugVehicle()) {
        std::cout << SIMTIME << " updateShadowLane() veh=" << myVehicle.getID()
                  << " newShadowLane=" << Named::getIDSecure(myShadowLane)
                  << "\n   before:" << " myShadowFurtherLanes=" << toString(myShadowFurtherLanes) << " further=" << toString(myVehicle.getFurtherLanes()) << " passed=" << toString(passed);
        std::cout << std::endl;
    }
#endif
    myVehicle.updateFurtherLanes(myShadowFurtherLanes, myShadowFurtherLanesPosLat, passed);
#ifdef DEBUG_SHADOWLANE
    if (debugVehicle()) std::cout
                << "\n   after:" << " myShadowFurtherLanes=" << toString(myShadowFurtherLanes) << "\n";
#endif
}


int
MSAbstractLaneChangeModel::getShadowDirection() const {
    if (isChangingLanes()) {
        if (pastMidpoint()) {
            return -myLaneChangeDirection;
        } else {
            return myLaneChangeDirection;
        }
    } else if (myShadowLane == nullptr) {
        return 0;
    } else if (myAmOpposite) {
        // return neigh-lane in forward direction
        return 1;
    } else if (&myShadowLane->getEdge() == &myVehicle.getLane()->getEdge()) {
        return myShadowLane->getIndex() - myVehicle.getLane()->getIndex();
    } else {
        // overlap with opposite direction lane
        return 1;
    }
}


MSLane*
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
        std::cout << "\n   newTarget (maneuverDist=" << myManeuverDist << " offset=" << targetDir << "): " << (myTargetLane == nullptr ? "NULL" : myTargetLane->getID())
                  << " newFurtherTargets: " << toString(myFurtherTargetLanes)
                  << std::endl;
    }
#endif
    return myTargetLane;
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
MSAbstractLaneChangeModel::calcAngleOffset() {
    double result = 0.;
    if (!(fabs(mySpeedLat) < NUMERICAL_EPS && fabs(myPreviousAngleOffset * 180 / M_PI) < NUMERICAL_EPS)) {
        if (myVehicle.getLength() < sqrt(SPEED2DIST(mySpeedLat) * SPEED2DIST(mySpeedLat) + SPEED2DIST(myVehicle.getSpeed()) * SPEED2DIST(myVehicle.getSpeed()))) {
            result = atan2(mySpeedLat, myVehicle.getSpeed());
        } else {
            result = myPreviousAngleOffset + asin((sin(M_PI / 2 - myPreviousAngleOffset) * (SPEED2DIST(mySpeedLat) - tan(myPreviousAngleOffset) * SPEED2DIST(myVehicle.getSpeed()))) / myVehicle.getLength());
        }
    }

    myAngleOffset = result;
    return result;
}


double
MSAbstractLaneChangeModel::estimateLCDuration(const double speed, const double remainingManeuverDist, const double decel, bool urgent) const {

    const SUMOVTypeParameter::SubParams& lcParams = myVehicle.getVehicleType().getParameter().getLCParams();
    if (lcParams.find(SUMO_ATTR_LCA_MAXSPEEDLATSTANDING) == lcParams.end() && lcParams.find(SUMO_ATTR_LCA_MAXSPEEDLATFACTOR) == lcParams.end()) {
        if (!myVehicle.getVehicleType().wasSet(VTYPEPARS_MAXSPEED_LAT_SET)) {
            // no dependency of lateral speed on longitudinal speed. (Only called prior to LC initialization to determine whether it could be completed)
            return STEPS2TIME(MSGlobals::gLaneChangeDuration);
        } else {
            return remainingManeuverDist / myVehicle.getVehicleType().getMaxSpeedLat();
        }
    }

    if (remainingManeuverDist == 0) {
        return 0;
    }

    // Check argument assumptions
    assert(speed >= 0);
    assert(remainingManeuverDist >= 0);
    assert(decel > 0);
    assert(myVehicle.getVehicleType().getMaxSpeedLat() > 0);
    assert(myMaxSpeedLatStanding <= myVehicle.getVehicleType().getMaxSpeedLat());
    assert(myMaxSpeedLatStanding >= 0);

    // for brevity
    const double v0 = speed;
    const double D = remainingManeuverDist;
    const double b = decel;
    const double wmin = myMaxSpeedLatStanding;
    const double f = myMaxSpeedLatFactor;
    const double wmax = myVehicle.getVehicleType().getMaxSpeedLat();

    /* Here's the approach for the calculation of the required time for the LC:
     * To obtain the maximal LC-duration, for v(t) we assume that v(t)=max(0, v0-b*t),
     * Where v(t)=0 <=> t >= ts:=v0/b
     * For the lateral speed w(t) this gives:
     * w(t) = min(wmax, wmin + f*v(t))
     * The lateral distance covered until t is
     * d(t) = int_0^t w(s) ds
     * We distinguish three possibilities for the solution d(T)=D, where T is the time of the LC completion.
     * 1) w(T) = wmax, i.e. v(T)>(wmax-wmin)/f
     * 2) wmin < w(T) < wmax, i.e. (wmax-wmin)/f > v(T) > 0
     * 3) w(T) = wmin, i.e., v(T)=0
     */
    const double vm = (wmax - wmin) / f;
    double distSoFar = 0.;
    double timeSoFar = 0.;
    double v = v0;
    if (v > vm) {
        const double wmaxTime = (v0 - vm) / b;
        const double d1 = wmax * wmaxTime;
        if (d1 >= D) {
            return D / wmax;
        } else {
            distSoFar += d1;
            timeSoFar += wmaxTime;
            v = vm;
        }
    }
    if (v > 0) {
        /* Here, w(t1+t) = wmin + f*v(t1+t) = wmin + f*(v - b*t)
         * Thus, the additional lateral distance covered after time t is:
         * d2 = (wmin + f*v)*t - 0.5*f*b*t^2
         * and the additional lateral distance covered until v=0 at t=v/b is:
         * d2 = (wmin + 0.5*f*v)*t
         */
        const double t = v / b; // stop time
        const double d2 = (wmin + 0.5 * f * v) * t; // lateral distance covered until stop
        assert(d2 > 0);
        if (distSoFar + d2 >= D) {
            // LC is completed during this phase
            const double x = 0.5 * f * b;
            const double y = wmin + f * v;
            /* Solve D - distSoFar = y*t - x*t^2.
             * 0 = x*t^2 - y*t/x + (D - distSoFar)/x
             */
            const double p = 0.5 * y / x;
            const double q = (D - distSoFar) / x;
            assert(p * p - q > 0);
            const double t2 = p + sqrt(p * p - q);
            return timeSoFar + t2;
        } else {
            distSoFar += d2;
            timeSoFar += t;
            //v = 0;
        }
    }
    // If we didn't return yet this means the LC was not completed until the vehicle stops (if braking with rate b)
    if (wmin == 0) {
        // LC won't be completed if vehicle stands
        double maneuverDist = remainingManeuverDist;
        const double vModel = computeSpeedLat(maneuverDist, maneuverDist, urgent);
        double result = D / vModel;
        // make sure that the vehicle isn't braking to a stop during the manuever
        if (vModel > SUMO_const_haltingSpeed && (vModel + myVehicle.getAcceleration() * result) > SUMO_const_haltingSpeed) {
            // unless the model tells us something different
            return result;
        } else {
            return -1;
        }
    } else {
        // complete LC with lateral speed wmin
        return timeSoFar + (D - distSoFar) / wmin;
    }
}

SUMOTime
MSAbstractLaneChangeModel::remainingTime() const {
    assert(isChangingLanes()); // Only to be called during ongoing lane change
    const SUMOVTypeParameter::SubParams& lcParams = myVehicle.getVehicleType().getParameter().getLCParams();
    if (lcParams.find(SUMO_ATTR_LCA_MAXSPEEDLATSTANDING) == lcParams.end() && lcParams.find(SUMO_ATTR_LCA_MAXSPEEDLATFACTOR) == lcParams.end()) {
        if (myVehicle.getVehicleType().wasSet(VTYPEPARS_MAXSPEED_LAT_SET)) {
            return TIME2STEPS((1. - myLaneChangeCompletion) * myManeuverDist / myVehicle.getVehicleType().getMaxSpeedLat());
        } else {
            return (SUMOTime)((1. - myLaneChangeCompletion) * (double)MSGlobals::gLaneChangeDuration);
        }
    }
    // Using maxSpeedLat(Factor/Standing)
    const bool urgent = (myOwnState & LCA_URGENT) != 0;
    return TIME2STEPS(estimateLCDuration(myVehicle.getSpeed(),
                                         fabs(myManeuverDist * (1 - myLaneChangeCompletion)),
                                         myVehicle.getCarFollowModel().getMaxDecel(), urgent));
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
    if (gDebugFlag2) {
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
        myLastFollowerSpeed = follower.first->getSpeed();
    }
}

void
MSAbstractLaneChangeModel::setLeaderGaps(CLeaderDist leader, double secGap) {
    if (leader.first != 0) {
        myLastLeaderGap = leader.second + myVehicle.getVehicleType().getMinGap();
        myLastLeaderSecureGap = secGap;
        myLastLeaderSpeed = leader.first->getSpeed();
    }
}

void
MSAbstractLaneChangeModel::setOrigLeaderGaps(CLeaderDist leader, double secGap) {
    if (leader.first != 0) {
        myLastOrigLeaderGap = leader.second + myVehicle.getVehicleType().getMinGap();
        myLastOrigLeaderSecureGap = secGap;
        myLastOrigLeaderSpeed = leader.first->getSpeed();
    }
}

void
MSAbstractLaneChangeModel::prepareStep() {
    getCanceledState(-1) = LCA_NONE;
    getCanceledState(0) = LCA_NONE;
    getCanceledState(1) = LCA_NONE;
    saveLCState(-1, LCA_UNKNOWN, LCA_UNKNOWN);
    saveLCState(0, LCA_UNKNOWN, LCA_UNKNOWN);
    saveLCState(1, LCA_UNKNOWN, LCA_UNKNOWN);
    myLastLateralGapRight = NO_NEIGHBOR;
    myLastLateralGapLeft = NO_NEIGHBOR;
    if (!myDontResetLCGaps) {
        myLastLeaderGap = NO_NEIGHBOR;
        myLastLeaderSecureGap = NO_NEIGHBOR;
        myLastFollowerGap = NO_NEIGHBOR;
        myLastFollowerSecureGap = NO_NEIGHBOR;
        myLastOrigLeaderGap = NO_NEIGHBOR;
        myLastOrigLeaderSecureGap = NO_NEIGHBOR;
        myLastLeaderSpeed = NO_NEIGHBOR;
        myLastFollowerSpeed = NO_NEIGHBOR;
        myLastOrigLeaderSpeed = NO_NEIGHBOR;
    }
    myCommittedSpeed = 0;
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
                myLastFollowerSecureGap = follower->getCarFollowModel().getSecureGap(follower, leader, follower->getSpeed(), leader->getSpeed(), leader->getCarFollowModel().getMaxDecel());
                myLastFollowerSpeed = follower->getSpeed();
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
                myLastLeaderSecureGap = follower->getCarFollowModel().getSecureGap(follower, leader, follower->getSpeed(), leader->getSpeed(), leader->getCarFollowModel().getMaxDecel());
                myLastLeaderSpeed = leader->getSpeed();
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
                myLastOrigLeaderSecureGap = follower->getCarFollowModel().getSecureGap(follower, leader, follower->getSpeed(), leader->getSpeed(), leader->getCarFollowModel().getMaxDecel());
                myLastOrigLeaderSpeed = leader->getSpeed();
            }
        }
    }
}


bool
MSAbstractLaneChangeModel::isStrategicBlocked() const {
    const int stateRight = mySavedStateRight.second;
    if (
        (stateRight & LCA_STRATEGIC) != 0
        && (stateRight & LCA_RIGHT) != 0
        && (stateRight & LCA_BLOCKED) != 0) {
        return true;
    }
    const int stateLeft = mySavedStateLeft.second;
    if (
        (stateLeft & LCA_STRATEGIC) != 0
        && (stateLeft & LCA_LEFT) != 0
        && (stateLeft & LCA_BLOCKED) != 0) {
        return true;
    }
    return false;
}

double
MSAbstractLaneChangeModel::getForwardPos() const {
    return myAmOpposite ? myVehicle.getLane()->getLength() - myVehicle.getPositionOnLane() : myVehicle.getPositionOnLane();
}


int
MSAbstractLaneChangeModel::getNormalizedLaneIndex() {
    const int i = myVehicle.getLane()->getIndex();
    if (myAmOpposite) {
        return myVehicle.getLane()->getParallelOpposite()->getEdge().getNumLanes() + myVehicle.getLane()->getEdge().getNumLanes() - 1 - i;
    } else {
        return i;
    }
}

void
MSAbstractLaneChangeModel::addLCSpeedAdvice(const double vSafe, bool ownAdvice) {
    const double accel = SPEED2ACCEL(vSafe - myVehicle.getSpeed());
    myLCAccelerationAdvices.push_back({accel, ownAdvice});
}


void
MSAbstractLaneChangeModel::saveState(OutputDevice& out) const {
    std::vector<std::string> lcState;
    if (MSGlobals::gLaneChangeDuration > 0) {
        lcState.push_back(toString(mySpeedLat));
        lcState.push_back(toString(myLaneChangeCompletion));
        lcState.push_back(toString(myLaneChangeDirection));
    }
    if (lcState.size() > 0) {
        out.writeAttr(SUMO_ATTR_LCSTATE, lcState);
    }
}

void
MSAbstractLaneChangeModel::loadState(const SUMOSAXAttributes& attrs) {
    if (attrs.hasAttribute(SUMO_ATTR_LCSTATE)) {
        std::istringstream bis(attrs.getString(SUMO_ATTR_LCSTATE));
        bis >> mySpeedLat;
        bis >> myLaneChangeCompletion;
        bis >> myLaneChangeDirection;
    }
}
