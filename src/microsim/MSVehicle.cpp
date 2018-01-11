/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSVehicle.cpp
/// @author  Christian Roessel
/// @author  Jakob Erdmann
/// @author  Bjoern Hendriks
/// @author  Daniel Krajzewicz
/// @author  Thimor Bohn
/// @author  Friedemann Wesner
/// @author  Laura Bieker
/// @author  Clemens Honomichl
/// @author  Michael Behrisch
/// @author  Axel Wegener
/// @author  Christoph Sommer
/// @author  Leonhard Luecken
/// @date    Mon, 05 Mar 2001
/// @version $Id$
///
// Representation of a vehicle in the micro simulation
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
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <map>
#include <utils/common/ToString.h>
#include <utils/common/FileHelpers.h>
#include <utils/vehicle/DijkstraRouter.h>
#include <utils/common/RandHelper.h>
#include <utils/emissions/PollutantsInterface.h>
#include <utils/emissions/HelpersHarmonoise.h>
#include <utils/common/StringUtils.h>
#include <utils/common/StdDefs.h>
#include <utils/geom/GeomHelper.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/iodevices/BinaryInputDevice.h>
#include <utils/xml/SUMOSAXAttributes.h>
#include <utils/xml/SUMOVehicleParserHelper.h>
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include <microsim/pedestrians/MSPerson.h>
#include <microsim/pedestrians/MSPModel.h>
#include <microsim/devices/MSDevice_Transportable.h>
#include <microsim/devices/MSDevice_Routing.h>
#include <microsim/devices/MSDevice_Vehroutes.h>
#include <microsim/output/MSStopOut.h>
#include <microsim/trigger/MSChargingStation.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include "MSVehicleControl.h"
#include "MSVehicleTransfer.h"
#include "MSGlobals.h"
#include "MSStoppingPlace.h"
#include "MSParkingArea.h"
#include "devices/MSDevice_Transportable.h"
#include "MSEdgeWeightsStorage.h"
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include "MSMoveReminder.h"
#include "MSTransportableControl.h"
#include "MSLane.h"
#include "MSJunction.h"
#include "MSVehicle.h"
#include "MSEdge.h"
#include "MSVehicleType.h"
#include "MSNet.h"
#include "MSRoute.h"
#include "MSLinkCont.h"
#include "MSLeaderInfo.h"

//#define DEBUG_PLAN_MOVE
//#define DEBUG_CHECKREWINDLINKLANES
//#define DEBUG_EXEC_MOVE
//#define DEBUG_FURTHER
//#define DEBUG_TARGET_LANE
//#define DEBUG_STOPS
//#define DEBUG_BESTLANES
//#define DEBUG_IGNORE_RED
//#define DEBUG_ACTIONSTEPS
//#define DEBUG_COND (getID() == "blocker")
//#define DEBUG_COND (true)
#define DEBUG_COND (isSelected())


#define STOPPING_PLACE_OFFSET 0.5

#define CRLL_LOOK_AHEAD 5

// @todo Calibrate with real-world values / make configurable
#define DIST_TO_STOPLINE_EXPECT_PRIORITY 1.0

// ===========================================================================
// static value definitions
// ===========================================================================
std::vector<MSLane*> MSVehicle::myEmptyLaneVector;
std::vector<MSTransportable*> MSVehicle::myEmptyTransportableVector;


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * methods of MSVehicle::State
 * ----------------------------------------------------------------------- */
MSVehicle::State::State(const State& state) {
    myPos = state.myPos;
    mySpeed = state.mySpeed;
    myPosLat = state.myPosLat;
    myBackPos = state.myBackPos;
    myPreviousSpeed = state.myPreviousSpeed;
    myLastCoveredDist = state.myLastCoveredDist;
}


MSVehicle::State&
MSVehicle::State::operator=(const State& state) {
    myPos   = state.myPos;
    mySpeed = state.mySpeed;
    myPosLat   = state.myPosLat;
    myBackPos = state.myBackPos;
    myPreviousSpeed = state.myPreviousSpeed;
    myLastCoveredDist = state.myLastCoveredDist;
    return *this;
}


bool
MSVehicle::State::operator!=(const State& state) {
    return (myPos    != state.myPos ||
            mySpeed  != state.mySpeed ||
            myPosLat != state.myPosLat ||
            myLastCoveredDist != state.myLastCoveredDist ||
            myPreviousSpeed != state.myPreviousSpeed ||
            myBackPos != state.myBackPos);
}


MSVehicle::State::State(double pos, double speed, double posLat, double backPos) :
    myPos(pos), mySpeed(speed), myPosLat(posLat), myBackPos(backPos), myPreviousSpeed(speed), myLastCoveredDist(SPEED2DIST(speed)) {}



/* -------------------------------------------------------------------------
 * methods of MSVehicle::WaitingTimeCollector
 * ----------------------------------------------------------------------- */

MSVehicle::WaitingTimeCollector::WaitingTimeCollector(SUMOTime memory) : myMemorySize(memory) {}

MSVehicle::WaitingTimeCollector::WaitingTimeCollector(const WaitingTimeCollector& wt) : myMemorySize(wt.getMemorySize()), myWaitingIntervals(wt.getWaitingIntervals()) {}

MSVehicle::WaitingTimeCollector&
MSVehicle::WaitingTimeCollector::operator=(const WaitingTimeCollector& wt) {
    myMemorySize = wt.getMemorySize();
    myWaitingIntervals = wt.getWaitingIntervals();
    return *this;
}

MSVehicle::WaitingTimeCollector&
MSVehicle::WaitingTimeCollector::operator=(SUMOTime t) {
    myWaitingIntervals.clear();
    passTime(t, true);
    return *this;
}

SUMOTime
MSVehicle::WaitingTimeCollector::cumulatedWaitingTime(SUMOTime memorySpan) const {
    assert(memorySpan <= myMemorySize);
    if (memorySpan == -1) {
        memorySpan = myMemorySize;
    }
    SUMOTime totalWaitingTime = 0;
    for (waitingIntervalList::const_iterator i = myWaitingIntervals.begin(); i != myWaitingIntervals.end(); i++) {
        if (i->second >= memorySpan) {
            if (i->first >= memorySpan) {
                break;
            } else {
                totalWaitingTime += memorySpan - i->first;
            }
        } else {
            totalWaitingTime += i->second - i->first;
        }
    }
    return totalWaitingTime;
}

void
MSVehicle::WaitingTimeCollector::passTime(SUMOTime dt, bool waiting) {
    waitingIntervalList::iterator i = myWaitingIntervals.begin();
    waitingIntervalList::iterator end = myWaitingIntervals.end();
    bool startNewInterval = i == end || (i->first != 0);
    while (i != end) {
        i->first += dt;
        if (i->first >= myMemorySize) {
            break;
        }
        i->second += dt;
        i++;
    }

    // remove intervals beyond memorySize
    waitingIntervalList::iterator::difference_type d = std::distance(i, end);
    while (d > 0) {
        myWaitingIntervals.pop_back();
        d--;
    }

    if (!waiting) {
        return;
    } else if (!startNewInterval) {
        myWaitingIntervals.begin()->first = 0;
    } else {
        myWaitingIntervals.push_front(std::make_pair(0, dt));
    }
    return;
}





/* -------------------------------------------------------------------------
 * methods of MSVehicle::Influencer
 * ----------------------------------------------------------------------- */
MSVehicle::Influencer::Influencer() :
    myLatDist(0),
    mySpeedAdaptationStarted(true),
    myConsiderSafeVelocity(true),
    myConsiderMaxAcceleration(true),
    myConsiderMaxDeceleration(true),
    myRespectJunctionPriority(true),
    myEmergencyBrakeRedLight(true),
    myLastRemoteAccess(-TIME2STEPS(20)),
    myStrategicLC(LC_NOCONFLICT),
    myCooperativeLC(LC_NOCONFLICT),
    mySpeedGainLC(LC_NOCONFLICT),
    myRightDriveLC(LC_NOCONFLICT),
    mySublaneLC(LC_NOCONFLICT),
    myTraciLaneChangePriority(LCP_URGENT),
    myTraCISignals(-1),
    myRoutingMode(0) {
}


MSVehicle::Influencer::~Influencer() {}


void
MSVehicle::Influencer::setSpeedTimeLine(const std::vector<std::pair<SUMOTime, double> >& speedTimeLine) {
    mySpeedAdaptationStarted = true;
    mySpeedTimeLine = speedTimeLine;
}


void
MSVehicle::Influencer::setLaneTimeLine(const std::vector<std::pair<SUMOTime, int> >& laneTimeLine) {
    myLaneTimeLine = laneTimeLine;
}

void
MSVehicle::Influencer::setSublaneChange(double latDist) {
    myLatDist = latDist;
}

int
MSVehicle::Influencer::getSpeedMode() const {
    return (1 * myConsiderSafeVelocity +
            2 * myConsiderMaxAcceleration +
            4 * myConsiderMaxDeceleration +
            8 * myRespectJunctionPriority +
            16 * myEmergencyBrakeRedLight);
}


int
MSVehicle::Influencer::getLanechangeMode() const {
    return (1 * myStrategicLC +
            4 * myCooperativeLC +
            16 * mySpeedGainLC +
            64 * myRightDriveLC +
            256 * myTraciLaneChangePriority +
            1024 * mySublaneLC);
}


double
MSVehicle::Influencer::influenceSpeed(SUMOTime currentTime, double speed, double vSafe, double vMin, double vMax) {
    // keep original speed
    myOriginalSpeed = speed;
    // remove leading commands which are no longer valid
    while (mySpeedTimeLine.size() == 1 || (mySpeedTimeLine.size() > 1 && currentTime > mySpeedTimeLine[1].first)) {
        mySpeedTimeLine.erase(mySpeedTimeLine.begin());
    }
    // do nothing if the time line does not apply for the current time
    if (mySpeedTimeLine.size() < 2 || currentTime < mySpeedTimeLine[0].first) {
        return speed;
    }
    // compute and set new speed
    if (!mySpeedAdaptationStarted) {
        mySpeedTimeLine[0].second = speed;
        mySpeedAdaptationStarted = true;
    }
    currentTime += DELTA_T;
    const double td = STEPS2TIME(currentTime - mySpeedTimeLine[0].first) / STEPS2TIME(mySpeedTimeLine[1].first + DELTA_T - mySpeedTimeLine[0].first);
    speed = mySpeedTimeLine[0].second - (mySpeedTimeLine[0].second - mySpeedTimeLine[1].second) * td;
    if (myConsiderSafeVelocity) {
        speed = MIN2(speed, vSafe);
    }
    if (myConsiderMaxAcceleration) {
        speed = MIN2(speed, vMax);
    }
    if (myConsiderMaxDeceleration) {
        speed = MAX2(speed, vMin);
    }
    return speed;
}


double
MSVehicle::Influencer::getOriginalSpeed() const {
    return mySpeedTimeLine.empty() ? -1 : myOriginalSpeed;
}


int
MSVehicle::Influencer::influenceChangeDecision(const SUMOTime currentTime, const MSEdge& currentEdge, const int currentLaneIndex, int state) {
    // remove leading commands which are no longer valid
    while (myLaneTimeLine.size() == 1 || (myLaneTimeLine.size() > 1 && currentTime > myLaneTimeLine[1].first)) {
        myLaneTimeLine.erase(myLaneTimeLine.begin());
    }
    ChangeRequest changeRequest = REQUEST_NONE;
    // do nothing if the time line does not apply for the current time
    if (myLaneTimeLine.size() >= 2 && currentTime >= myLaneTimeLine[0].first) {
        const int destinationLaneIndex = myLaneTimeLine[1].second;
        if (destinationLaneIndex < (int)currentEdge.getLanes().size()) {
            if (currentLaneIndex > destinationLaneIndex) {
                changeRequest = REQUEST_RIGHT;
            } else if (currentLaneIndex < destinationLaneIndex) {
                changeRequest = REQUEST_LEFT;
            } else {
                changeRequest = REQUEST_HOLD;
            }
        }
    }
    // check whether the current reason shall be canceled / overridden
    if ((state & LCA_WANTS_LANECHANGE_OR_STAY) != 0) {
        // flags for the current reason
        LaneChangeMode mode = LC_NEVER;
        if ((state & LCA_TRACI) != 0 && myLatDist != 0) {
            // continue sublane change manoeuvre
            return state;
        } else if ((state & LCA_STRATEGIC) != 0) {
            mode = myStrategicLC;
        } else if ((state & LCA_COOPERATIVE) != 0) {
            mode = myCooperativeLC;
        } else if ((state & LCA_SPEEDGAIN) != 0) {
            mode = mySpeedGainLC;
        } else if ((state & LCA_KEEPRIGHT) != 0) {
            mode = myRightDriveLC;
        } else if ((state & LCA_SUBLANE) != 0) {
            mode = mySublaneLC;
        } else if ((state & LCA_TRACI) != 0) {
            mode = LC_NEVER;
        } else {
            WRITE_WARNING("Lane change model did not provide a reason for changing (state=" + toString(state) + ", time=" + time2string(currentTime) + "\n");
        }
        if (mode == LC_NEVER) {
            // cancel all lcModel requests
            state &= ~LCA_WANTS_LANECHANGE_OR_STAY;
            state &= ~LCA_URGENT;
        } else if (mode == LC_NOCONFLICT && changeRequest != REQUEST_NONE) {
            if (
                ((state & LCA_LEFT) != 0 && changeRequest != REQUEST_LEFT) ||
                ((state & LCA_RIGHT) != 0 && changeRequest != REQUEST_RIGHT) ||
                ((state & LCA_STAY) != 0 && changeRequest != REQUEST_HOLD)) {
                // cancel conflicting lcModel request
                state &= ~LCA_WANTS_LANECHANGE_OR_STAY;
                state &= ~LCA_URGENT;
            }
        } else if (mode == LC_ALWAYS) {
            // ignore any TraCI requests
            return state;
        }
    }
    // apply traci requests
    if (changeRequest == REQUEST_NONE) {
        return state;
    } else {
        state |= LCA_TRACI;
        // security checks
        if ((myTraciLaneChangePriority == LCP_ALWAYS)
                || (myTraciLaneChangePriority == LCP_NOOVERLAP && (state & LCA_OVERLAPPING) == 0)) {
            state &= ~(LCA_BLOCKED | LCA_OVERLAPPING);
        }
        if (changeRequest != REQUEST_HOLD && myTraciLaneChangePriority != LCP_OPPORTUNISTIC) {
            state |= LCA_URGENT;
        }
        switch (changeRequest) {
            case REQUEST_HOLD:
                return state | LCA_STAY;
            case REQUEST_LEFT:
                return state | LCA_LEFT;
            case REQUEST_RIGHT:
                return state | LCA_RIGHT;
            default:
                throw ProcessError("should not happen");
        }
    }
}


double
MSVehicle::Influencer::changeRequestRemainingSeconds(const SUMOTime currentTime) const {
    assert(myLaneTimeLine.size() >= 2);
    assert(currentTime >= myLaneTimeLine[0].first);
    return STEPS2TIME(myLaneTimeLine[1].first - currentTime);
}


void
MSVehicle::Influencer::setSpeedMode(int speedMode) {
    myConsiderSafeVelocity = ((speedMode & 1) != 0);
    myConsiderMaxAcceleration = ((speedMode & 2) != 0);
    myConsiderMaxDeceleration = ((speedMode & 4) != 0);
    myRespectJunctionPriority = ((speedMode & 8) != 0);
    myEmergencyBrakeRedLight = ((speedMode & 16) != 0);
}


void
MSVehicle::Influencer::setLaneChangeMode(int value) {
    myStrategicLC = (LaneChangeMode)(value & (1 + 2));
    myCooperativeLC = (LaneChangeMode)((value & (4 + 8)) >> 2);
    mySpeedGainLC = (LaneChangeMode)((value & (16 + 32)) >> 4);
    myRightDriveLC = (LaneChangeMode)((value & (64 + 128)) >> 6);
    myTraciLaneChangePriority = (TraciLaneChangePriority)((value & (256 + 512)) >> 8);
    mySublaneLC = (LaneChangeMode)((value & (1024 + 2048)) >> 10);
}


void
MSVehicle::Influencer::setRemoteControlled(Position xyPos, MSLane* l, double pos, double posLat, double angle, int edgeOffset, const ConstMSEdgeVector& route, SUMOTime t) {
    myRemoteXYPos = xyPos;
    myRemoteLane = l;
    myRemotePos = pos;
    myRemotePosLat = posLat;
    myRemoteAngle = angle;
    myRemoteEdgeOffset = edgeOffset;
    myRemoteRoute = route;
    myLastRemoteAccess = t;
}


bool
MSVehicle::Influencer::isRemoteControlled() const {
    return myLastRemoteAccess == MSNet::getInstance()->getCurrentTimeStep();
}


bool
MSVehicle::Influencer::isRemoteAffected(SUMOTime t) const {
    return myLastRemoteAccess >= t - TIME2STEPS(10);
}

void
MSVehicle::Influencer::postProcessRemoteControl(MSVehicle* v) {
    const bool wasOnRoad = v->isOnRoad();
    if (v->isOnRoad()) {
        v->onRemovalFromNet(MSMoveReminder::NOTIFICATION_TELEPORT);
        v->getLane()->removeVehicle(v, MSMoveReminder::NOTIFICATION_TELEPORT);
    }
    if (myRemoteRoute.size() != 0) {
        v->replaceRouteEdges(myRemoteRoute, true);
    }
    v->myCurrEdge = v->getRoute().begin() + myRemoteEdgeOffset;
    if (myRemoteLane != 0 && myRemotePos > myRemoteLane->getLength()) {
        myRemotePos = myRemoteLane->getLength();
    }
    if (myRemoteLane != 0 && fabs(myRemotePosLat) < 0.5 * (myRemoteLane->getWidth() + v->getVehicleType().getWidth())) {
        myRemoteLane->forceVehicleInsertion(v, myRemotePos, MSMoveReminder::NOTIFICATION_TELEPORT, myRemotePosLat);
        v->updateBestLanes();
        if (!wasOnRoad) {
            v->drawOutsideNetwork(false);
        }
        //std::cout << "on road network p=" << myRemoteXYPos << " a=" << myRemoteAngle << " l=" << Named::getIDSecure(myRemoteLane) << " pos=" << myRemotePos << " posLat=" << myRemotePosLat << "\n";
    } else {
        if (v->getDeparture() == NOT_YET_DEPARTED) {
            v->onDepart();
        }
        v->drawOutsideNetwork(true);
        //std::cout << "outside network p=" << myRemoteXYPos << " a=" << myRemoteAngle << " l=" << Named::getIDSecure(myRemoteLane) << "\n";
    }
    // ensure that the position is correct (i.e. when the lanePosition is ambiguous at corners)
    v->setRemoteState(myRemoteXYPos);
    // inverse of GeomHelper::naviDegree
    v->setAngle(M_PI / 2. - DEG2RAD(myRemoteAngle));
}


double
MSVehicle::Influencer::implicitSpeedRemote(const MSVehicle* veh, double oldSpeed) {
    double dist = 0;
    if (myRemoteLane == 0) {
        dist = veh->getPosition().distanceTo2D(myRemoteXYPos);
    } else {
        dist = veh->getDistanceToPosition(myRemotePos, &myRemoteLane->getEdge());
    }
    if (DIST2SPEED(dist) > veh->getMaxSpeed()) {
        return oldSpeed;
    } else {
        return DIST2SPEED(dist);
    }
}

double
MSVehicle::Influencer::implicitDeltaPosRemote(const MSVehicle* veh) {
    double dist = 0;
    if (myRemoteLane == 0) {
        dist = veh->getPosition().distanceTo2D(myRemoteXYPos);
    } else {
        dist = veh->getDistanceToPosition(myRemotePos, &myRemoteLane->getEdge());
    }
    if (DIST2SPEED(dist) > veh->getMaxSpeed()) {
        return 0;
    } else {
        return dist;
    }
}


SUMOAbstractRouter<MSEdge, SUMOVehicle>&
MSVehicle::Influencer::getRouterTT() const {
    if (myRoutingMode == 1) {
        return MSDevice_Routing::getRouterTT();
    } else {
        return MSNet::getInstance()->getRouterTT();
    }
}


/* -------------------------------------------------------------------------
 * Stop-methods
 * ----------------------------------------------------------------------- */
double
MSVehicle::Stop::getEndPos(const SUMOVehicle& veh) const {
    if (busstop != 0) {
        return busstop->getLastFreePos(veh);
    } else if (containerstop != 0) {
        return containerstop->getLastFreePos(veh);
    } else if (parkingarea != 0) {
        return parkingarea->getLastFreePos(veh);
    } else if (chargingStation != 0) {
        return chargingStation->getLastFreePos(veh);
    }
    return pars.endPos;
}


std::string
MSVehicle::Stop::getDescription() const {
    if (parkingarea != 0) {
        return "parkingArea:" + parkingarea->getID();
    } else if (containerstop != 0) {
        return "containerStop:" + containerstop->getID();
    } else if (busstop != 0) {
        return "busStop:" + busstop->getID();
    } else if (chargingStation != 0) {
        return "chargingStation:" + chargingStation->getID();
    } else {
        return "lane:" + lane->getID() + " pos:" + toString(pars.endPos);
    }
}


void
MSVehicle::Stop::write(OutputDevice& dev) const {
    // lots of duplication with SUMOVehicleParameter::Stop::write()
    dev.openTag(SUMO_TAG_STOP);
    if (busstop != 0) {
        dev.writeAttr(SUMO_ATTR_BUS_STOP, busstop->getID());
    }
    if (containerstop != 0) {
        dev.writeAttr(SUMO_ATTR_CONTAINER_STOP, containerstop->getID());
    }
    if (busstop == 0 && containerstop == 0) {
        dev.writeAttr(SUMO_ATTR_LANE, lane->getID());
        dev.writeAttr(SUMO_ATTR_STARTPOS, pars.startPos);
        dev.writeAttr(SUMO_ATTR_ENDPOS, pars.endPos);
    }
    if (duration >= 0) {
        dev.writeAttr(SUMO_ATTR_DURATION, STEPS2TIME(duration));
    }
    if (pars.until >= 0) {
        dev.writeAttr(SUMO_ATTR_UNTIL, STEPS2TIME(pars.until));
    }
    if (pars.triggered) {
        dev.writeAttr(SUMO_ATTR_TRIGGERED, pars.triggered);
    }
    if (pars.containerTriggered) {
        dev.writeAttr(SUMO_ATTR_CONTAINER_TRIGGERED, pars.containerTriggered);
    }
    if (pars.parking) {
        dev.writeAttr(SUMO_ATTR_PARKING, pars.parking);
    }
    if (pars.awaitedPersons.size() > 0) {
        dev.writeAttr(SUMO_ATTR_EXPECTED, joinToString(pars.awaitedPersons, " "));
    }
    if (pars.awaitedContainers.size() > 0) {
        dev.writeAttr(SUMO_ATTR_EXPECTED_CONTAINERS, joinToString(pars.awaitedContainers, " "));
    }
    dev.closeTag();
}


/* -------------------------------------------------------------------------
 * MSVehicle-methods
 * ----------------------------------------------------------------------- */
MSVehicle::MSVehicle(SUMOVehicleParameter* pars, const MSRoute* route,
                     MSVehicleType* type, const double speedFactor) :
    MSBaseVehicle(pars, route, type, speedFactor),
    myWaitingTime(0),
    myWaitingTimeCollector(),
    myTimeLoss(0),
    myState(0, 0, 0, 0),
    myActionStep(true),
    myLastActionTime(0),
    myLane(0),
    myLastBestLanesEdge(0),
    myLastBestLanesInternalLane(0),
    myPersonDevice(0),
    myContainerDevice(0),
    myAcceleration(0),
    mySignals(0),
    myAmOnNet(false),
    myAmRegisteredAsWaitingForPerson(false),
    myAmRegisteredAsWaitingForContainer(false),
    myHaveToWaitOnNextLink(false),
    myAngle(0),
    myStopDist(std::numeric_limits<double>::max()),
    myCollisionImmunity(-1),
    myCachedPosition(Position::INVALID),
    myEdgeWeights(0),
    myInfluencer(0)
{
    if (!(*myCurrEdge)->isTazConnector()) {
        if (pars->departLaneProcedure == DEPART_LANE_GIVEN) {
            if ((*myCurrEdge)->getDepartLane(*this) == 0) {
                throw ProcessError("Invalid departlane definition for vehicle '" + pars->id + "'.");
            }
        } else {
            if ((*myCurrEdge)->allowedLanes(type->getVehicleClass()) == 0) {
                throw ProcessError("Vehicle '" + pars->id + "' is not allowed to depart on any lane of its first edge.");
            }
        }
        if (pars->departSpeedProcedure == DEPART_SPEED_GIVEN && pars->departSpeed > type->getMaxSpeed()) {
            throw ProcessError("Departure speed for vehicle '" + pars->id +
                               "' is too high for the vehicle type '" + type->getID() + "'.");
        }
    }
    myLaneChangeModel = MSAbstractLaneChangeModel::build(type->getLaneChangeModel(), *this);
    myCFVariables = type->getCarFollowModel().createVehicleVariables();
    myNextDriveItem = myLFLinkLanes.begin();
}


MSVehicle::~MSVehicle() {
    delete myEdgeWeights;
    for (std::vector<MSLane*>::iterator i = myFurtherLanes.begin(); i != myFurtherLanes.end(); ++i) {
        (*i)->resetPartialOccupation(this);
    }
    removeApproachingInformation(myLFLinkLanes);
    myLaneChangeModel->cleanupShadowLane();
    myLaneChangeModel->cleanupTargetLane();
    // still needed when calling resetPartialOccupation (getShadowLane) and when removing
    // approach information from parallel links
    delete myLaneChangeModel;
    myFurtherLanes.clear();
    myFurtherLanesPosLat.clear();
    //
    if (myType->isVehicleSpecific()) {
        MSNet::getInstance()->getVehicleControl().removeVType(myType);
    }
    delete myInfluencer;
}


void
MSVehicle::onRemovalFromNet(const MSMoveReminder::Notification reason) {
#ifdef DEBUG_ACTIONSTEPS
    if DEBUG_COND {
    std::cout << SIMTIME << " Removing vehicle '" << getID() << "' (reason: " << toString(reason) << ")" << std::endl;
    }
#endif
    MSVehicleTransfer::getInstance()->remove(this);
    removeApproachingInformation(myLFLinkLanes);
    leaveLane(reason);
}


// ------------ interaction with the route
bool
MSVehicle::hasArrived() const {
    return (myCurrEdge == myRoute->end() - 1
            && (myStops.empty() || myStops.front().edge != myCurrEdge)
            && myState.myPos > myArrivalPos - POSITION_EPS
            && !isRemoteControlled());
}


bool
MSVehicle::replaceRoute(const MSRoute* newRoute, bool onInit, int offset, bool addRouteStops, bool removeStops) {
    const ConstMSEdgeVector& edges = newRoute->getEdges();
    // assert the vehicle may continue (must not be "teleported" or whatever to another position)
    if (!onInit && !newRoute->contains(*myCurrEdge)) {
        return false;
    }

    // rebuild in-vehicle route information
    if (onInit) {
        myCurrEdge = newRoute->begin();
    } else {
        MSRouteIterator newCurrEdge = find(edges.begin() + offset, edges.end(), *myCurrEdge);;
        if (myLane->getEdge().isInternal() && (
                    (newCurrEdge + 1) == edges.end() || (*(newCurrEdge + 1)) != &(myLane->getOutgoingLanes()[0]->getEdge()))) {
            return false;
        }
        myCurrEdge = newCurrEdge;
    }
    const bool stopsFromScratch = onInit && myRoute->getStops().empty();
    // check whether the old route may be deleted (is not used by anyone else)
    newRoute->addReference();
    myRoute->release();
    // assign new route
    myRoute = newRoute;
    // update arrival definition
    calculateArrivalParams();
    // save information that the vehicle was rerouted
    myNumberReroutes++;
    MSNet::getInstance()->informVehicleStateListener(this, MSNet::VEHICLE_STATE_NEWROUTE);
    // if we did not drive yet it may be best to simply reassign the stops from scratch
    if (stopsFromScratch) {
        myStops.clear();
        addStops(!MSGlobals::gCheckRoutes);
    } else {
        // recheck old stops
        for (std::list<Stop>::iterator iter = myStops.begin(); iter != myStops.end();) {
            if (removeStops && find(myCurrEdge, edges.end(), &iter->lane->getEdge()) == edges.end()) {
                iter = myStops.erase(iter);
            } else {
                // iter->edge may point to edges.end() if removeStops is false but it should be replaced by the triggered rerouter anyway
                iter->edge = find(myCurrEdge, edges.end(), &iter->lane->getEdge());
                ++iter;
            }
        }
        // add new stops
        if (addRouteStops) {
            for (std::vector<SUMOVehicleParameter::Stop>::const_iterator i = newRoute->getStops().begin(); i != newRoute->getStops().end(); ++i) {
                std::string error;
                addStop(*i, error);
                if (error != "") {
                    WRITE_WARNING(error);
                }
            }
        }
    }
    // update best lanes (after stops were added)
    myLastBestLanesEdge = 0;
    myLastBestLanesInternalLane = 0;
    updateBestLanes(true, onInit ? (*myCurrEdge)->getLanes().front() : 0);
    return true;
}


bool
MSVehicle::willPass(const MSEdge* const edge) const {
    return find(myCurrEdge, myRoute->end(), edge) != myRoute->end();
}


int
MSVehicle::getRoutePosition() const {
    return (int) std::distance(myRoute->begin(), myCurrEdge);
}


void
MSVehicle::resetRoutePosition(int index) {
    myCurrEdge = myRoute->begin() + index;
    // !!! hack
    myArrivalPos = (*(myRoute->end() - 1))->getLanes()[0]->getLength();
}



const MSEdgeWeightsStorage&
MSVehicle::getWeightsStorage() const {
    return _getWeightsStorage();
}


MSEdgeWeightsStorage&
MSVehicle::getWeightsStorage() {
    return _getWeightsStorage();
}


MSEdgeWeightsStorage&
MSVehicle::_getWeightsStorage() const {
    if (myEdgeWeights == 0) {
        myEdgeWeights = new MSEdgeWeightsStorage();
    }
    return *myEdgeWeights;
}


// ------------ Interaction with move reminders
void
MSVehicle::workOnMoveReminders(double oldPos, double newPos, double newSpeed) {
    // This erasure-idiom works for all stl-sequence-containers
    // See Meyers: Effective STL, Item 9
    for (MoveReminderCont::iterator rem = myMoveReminders.begin(); rem != myMoveReminders.end();) {
        // XXX: calling notifyMove with newSpeed seems not the best choice. For the ballistic update, the average speed is calculated and used
        //      although a higher order quadrature-formula might be more adequate.
        //      For the euler case (where the speed is considered constant for each time step) it is conceivable that
        //      the current calculations may lead to systematic errors for large time steps (compared to reality). Refs. #2579
        if (!rem->first->notifyMove(*this, oldPos + rem->second, newPos + rem->second, MAX2(0., newSpeed))) {
#ifdef _DEBUG
            if (myTraceMoveReminders) {
                traceMoveReminder("notifyMove", rem->first, rem->second, false);
            }
#endif
            rem = myMoveReminders.erase(rem);
        } else {
#ifdef _DEBUG
            if (myTraceMoveReminders) {
                traceMoveReminder("notifyMove", rem->first, rem->second, true);
            }
#endif
            ++rem;
        }
    }
}


// XXX: consider renaming...
void
MSVehicle::adaptLaneEntering2MoveReminder(const MSLane& enteredLane) {
    // save the old work reminders, patching the position information
    //  add the information about the new offset to the old lane reminders
    const double oldLaneLength = myLane->getLength();
    for (MoveReminderCont::iterator rem = myMoveReminders.begin(); rem != myMoveReminders.end(); ++rem) {
        rem->second += oldLaneLength;
#ifdef _DEBUG
//        if (rem->first==0) std::cout << "Null reminder (?!)" << std::endl;
//        std::cout << "Adapted MoveReminder on lane " << ((rem->first->getLane()==0) ? "NULL" : rem->first->getLane()->getID()) <<" position to " << rem->second << std::endl;
        if (myTraceMoveReminders) {
            traceMoveReminder("adaptedPos", rem->first, rem->second, true);
        }
#endif
    }
    for (std::vector< MSMoveReminder* >::const_iterator rem = enteredLane.getMoveReminders().begin(); rem != enteredLane.getMoveReminders().end(); ++rem) {
        addReminder(*rem);
    }
}


// ------------ Other getter methods
double
MSVehicle::getSlope() const {
    if (myLane == 0) {
        return 0;
    }
    const double lp = getPositionOnLane();
    const double gp = myLane->interpolateLanePosToGeometryPos(lp);
    return myLane->getShape().slopeDegreeAtOffset(gp);
}


Position
MSVehicle::getPosition(const double offset) const {
    if (myLane == 0) {
        // when called in the context of GUI-Drawing, the simulation step is already incremented
        if (myInfluencer != 0 && myInfluencer->isRemoteAffected(MSNet::getInstance()->getCurrentTimeStep())) {
            return myCachedPosition;
        } else {
            return Position::INVALID;
        }
    }
    if (isParking()) {
        if (myStops.begin()->parkingarea != 0) {
            return myStops.begin()->parkingarea->getVehiclePosition(*this);
        } else {
            // position beside the road
            PositionVector shp = myLane->getEdge().getLanes()[0]->getShape();
            shp.move2side(SUMO_const_laneWidth * (MSNet::getInstance()->lefthand() ? -1 : 1));
            return shp.positionAtOffset(myLane->interpolateLanePosToGeometryPos(getPositionOnLane() + offset));
        }
    }
    const bool changingLanes = getLaneChangeModel().isChangingLanes();
    if (offset == 0. && !changingLanes) {
        if (myCachedPosition == Position::INVALID) {
            myCachedPosition = validatePosition(myLane->geometryPositionAtOffset(myState.myPos, -myState.myPosLat));
        }
        return myCachedPosition;
    }
    Position result = validatePosition(myLane->geometryPositionAtOffset(getPositionOnLane() + offset, -getLateralPositionOnLane()), offset);
    return result;
}


Position
MSVehicle::getPositionAlongBestLanes(double offset) const {
    assert(MSGlobals::gUsingInternalLanes);
    const std::vector<MSLane*>& bestLanes = getBestLanesContinuation();
    auto nextBestLane = bestLanes.begin();
    double pos = myState.myPos;
    const MSLane* lane = getLane();
    assert(lane != 0);
    bool success = true;

    while (offset > 0) {
        // take into account lengths along internal lanes
        while (lane->isInternal() && offset > 0) {
            if (offset > lane->getLength() - pos) {
                offset -= lane->getLength() - pos;
                lane = lane->getLinkCont()[0]->getViaLaneOrLane();
                pos = 0.;
                if (lane == 0) {
                    success = false;
                    offset = 0.;
                }
            } else {
                pos += offset;
                offset = 0;
            }
        }
        // set nextBestLane to next non-internal lane
        while (nextBestLane != bestLanes.end() && *nextBestLane == 0) {
            ++nextBestLane;
        }
        if (offset > 0) {
            assert(!lane->isInternal());
            assert(lane == *nextBestLane);
            if (offset > lane->getLength() - pos) {
                offset -= lane->getLength() - pos;
                ++nextBestLane;
                assert(nextBestLane == bestLanes.end() || *nextBestLane != 0);
                if (nextBestLane == bestLanes.end()) {
                    success = false;
                    offset = 0.;
                } else {
                    MSLink* link = lane->getLinkTo(*nextBestLane);
                    assert(link != 0);
                    lane = link->getViaLaneOrLane();
                    pos = 0.;
                }
            } else {
                pos += offset;
                offset = 0;
            }
        }

    }

    if (success) {
        return lane->geometryPositionAtOffset(pos, -getLateralPositionOnLane());
    } else {
        return Position::INVALID;
    }
}


Position
MSVehicle::validatePosition(Position result, double offset) const {
    int furtherIndex = 0;
    while (result == Position::INVALID) {
        if (furtherIndex >= (int)myFurtherLanes.size()) {
            //WRITE_WARNING("Could not compute position for vehicle '" + getID() + "', time=" + time2string(MSNet::getInstance()->getCurrentTimeStep()) + ".");
            break;
        }
        //std::cout << SIMTIME << " veh=" << getID() << " lane=" << myLane->getID() << " pos=" << getPositionOnLane() << " posLat=" << getLateralPositionOnLane() << " offset=" << offset << " result=" << result << " i=" << furtherIndex << " further=" << myFurtherLanes.size() << "\n";
        result = myFurtherLanes[furtherIndex]->geometryPositionAtOffset(myFurtherLanes[furtherIndex]->getLength() + offset, -getLateralPositionOnLane());
        furtherIndex++;
    }
    return result;
}


const MSEdge*
MSVehicle::getRerouteOrigin() const {
    // too close to the next junction, so avoid an emergency brake here
    if (myLane != 0 && (myCurrEdge + 1) != myRoute->end() &&
            myState.myPos > myLane->getLength() - getCarFollowModel().brakeGap(myState.mySpeed, getCarFollowModel().getMaxDecel(), 0.)) {
        return *(myCurrEdge + 1);
    }
    if (myLane != 0) {
        return myLane->getNextNormal();
    }
    return *myCurrEdge;
}

void
MSVehicle::setAngle(double angle, bool straightenFurther) {
#ifdef DEBUG_FURTHER
    if (DEBUG_COND) {
        std::cout << SIMTIME << " veh '" << getID() << " setAngle(" << angle <<  ") straightenFurther=" << straightenFurther << std::endl;
    }
#endif
    myAngle = angle;
    if (straightenFurther && myFurtherLanesPosLat.size() > 0) {
        myFurtherLanesPosLat[myFurtherLanesPosLat.size() - 1] = getLateralPositionOnLane();
    }
}


double
MSVehicle::computeAngle() const {
    Position p1;
    const double posLat = -myState.myPosLat; // @todo get rid of the '-'
    if (isParking()) {
        if (myStops.begin()->parkingarea != 0) {
            return myStops.begin()->parkingarea->getVehicleAngle(*this);
        } else {
            return myLane->getShape().rotationAtOffset(myLane->interpolateLanePosToGeometryPos(getPositionOnLane()));
        }
    }
    if (getLaneChangeModel().isChangingLanes()) {
        // cannot use getPosition() because it already includes the offset to the side and thus messes up the angle
        p1 = myLane->geometryPositionAtOffset(myState.myPos, posLat);
    } else {
        p1 = getPosition();
    }

    Position p2 = getBackPosition();
    if (p2 == Position::INVALID) {
        // Handle special case of vehicle's back reaching out of the network
        if (myFurtherLanes.size() > 0) {
            p2 = myFurtherLanes.back()->geometryPositionAtOffset(0, -myFurtherLanesPosLat.back());
        } else {
            p2 = myLane->geometryPositionAtOffset(0, posLat);
        }
    }
    double result = (p1 != p2 ? p2.angleTo2D(p1) :
                     myLane->getShape().rotationAtOffset(myLane->interpolateLanePosToGeometryPos(getPositionOnLane())));
    if (getLaneChangeModel().isChangingLanes()) {
        result += DEG2RAD(getLaneChangeModel().getAngleOffset());
    }
#ifdef DEBUG_FURTHER
    if (DEBUG_COND) {
        std::cout << SIMTIME << " computeAngle veh=" << getID() << " p1=" << p1 << " p2=" << p2 << " angle=" << RAD2DEG(result) << " naviDegree=" << GeomHelper::naviDegree(result) << "\n";
    }
#endif
    return result;
}


const Position
MSVehicle::getBackPosition() const {
    const double posLat = -myState.myPosLat; // @todo get rid of the '-'
    if (myState.myPos >= myType->getLength()) {
        // vehicle is fully on the new lane
        return myLane->geometryPositionAtOffset(myState.myPos - myType->getLength(), posLat);
    } else {
        if (getLaneChangeModel().isChangingLanes() && myFurtherLanes.size() > 0 && getLaneChangeModel().getShadowLane(myFurtherLanes.back()) == 0) {
            // special case where the target lane has no predecessor
            return myLane->geometryPositionAtOffset(0, posLat);
        } else {
#ifdef DEBUG_FURTHER
            if (DEBUG_COND) {
                std::cout << "    getBackPosition veh=" << getID() << " myLane=" << myLane->getID() << " further=" << toString(myFurtherLanes) << " myFurtherLanesPosLat=" << toString(myFurtherLanesPosLat) << "\n";
            }
#endif
            return myFurtherLanes.size() > 0 && !getLaneChangeModel().isChangingLanes()
                   ? myFurtherLanes.back()->geometryPositionAtOffset(getBackPositionOnLane(myFurtherLanes.back()), -myFurtherLanesPosLat.back())
                   : myLane->geometryPositionAtOffset(0, posLat);
        }
    }
}

// ------------
bool
MSVehicle::addStop(const SUMOVehicleParameter::Stop& stopPar, std::string& errorMsg, SUMOTime untilOffset, bool collision,
                   MSRouteIterator* searchStart) {
    Stop stop(stopPar);
    stop.lane = MSLane::dictionary(stopPar.lane);
    if (!stop.lane->allowsVehicleClass(myType->getVehicleClass())) {
        errorMsg = "Vehicle '" + myParameter->id + "' is not allowed to stop on lane '" + stopPar.lane + "'.";
        return false;
    }
    stop.busstop = MSNet::getInstance()->getStoppingPlace(stopPar.busstop, SUMO_TAG_BUS_STOP);
    stop.containerstop = MSNet::getInstance()->getStoppingPlace(stopPar.containerstop, SUMO_TAG_CONTAINER_STOP);
    stop.parkingarea = static_cast<MSParkingArea*>(MSNet::getInstance()->getStoppingPlace(stopPar.parkingarea, SUMO_TAG_PARKING_AREA));
    stop.chargingStation = MSNet::getInstance()->getStoppingPlace(stopPar.chargingStation, SUMO_TAG_CHARGING_STATION);
    stop.duration = stopPar.duration;
    stop.triggered = stopPar.triggered;
    stop.containerTriggered = stopPar.containerTriggered;
    stop.timeToBoardNextPerson = 0;
    stop.timeToLoadNextContainer = 0;
    if (stopPar.until != -1) {
        // !!! it would be much cleaner to invent a constructor for stops which takes "until" as an argument
        const_cast<SUMOVehicleParameter::Stop&>(stop.pars).until += untilOffset;
    }
    stop.collision = collision;
    stop.reached = false;
    stop.numExpectedPerson = (int)stopPar.awaitedPersons.size();
    stop.numExpectedContainer = (int)stopPar.awaitedContainers.size();
    std::string stopType = "stop";
    std::string stopID = "";
    if (stop.busstop != 0) {
        stopType = "busStop";
        stopID = stop.busstop->getID();
    } else if (stop.containerstop != 0) {
        stopType = "containerStop";
        stopID = stop.containerstop->getID();
    } else if (stop.chargingStation != 0) {
        stopType = "chargingStation";
        stopID = stop.chargingStation->getID();
    } else if (stop.parkingarea != 0) {
        stopType = "parkingArea";
        stopID = stop.parkingarea->getID();
    }
    const std::string errorMsgStart = stopID == "" ? stopType : stopType + " '" + stopID + "'";

    if (stop.pars.startPos < 0 || stop.pars.endPos > stop.lane->getLength()) {
        errorMsg = errorMsgStart + " for vehicle '" + myParameter->id + "' on lane '" + stopPar.lane + "' has an invalid position.";
        return false;
    }
    if (stopType != "stop" && stopType != "parkingArea" && myType->getLength() / 2. > stop.pars.endPos - stop.pars.startPos) {
        errorMsg = errorMsgStart + " on lane '" + stopPar.lane + "' is too short for vehicle '" + myParameter->id + "'.";
    }
    // if stop is on an internal edge the normal edge before the intersection is used
    const MSEdge* stopEdge = stop.lane->getEdge().getNormalBefore();
    if (searchStart == 0) {
        searchStart = &myCurrEdge;
    }
    stop.edge = find(*searchStart, myRoute->end(), stopEdge);
    MSRouteIterator prevStopEdge = myCurrEdge;
    double prevStopPos = myState.myPos;
    // where to insert the stop
    std::list<Stop>::iterator iter = myStops.begin();
    if (stopPar.index == STOP_INDEX_END || stopPar.index >= static_cast<int>(myStops.size())) {
        if (myStops.size() > 0) {
            prevStopEdge = myStops.back().edge;
            prevStopPos = myStops.back().pars.endPos;
            iter = myStops.end();
            stop.edge = find(prevStopEdge, myRoute->end(), stopEdge);
            if (prevStopEdge == stop.edge && prevStopPos > stop.pars.endPos) {
                stop.edge = find(prevStopEdge + 1, myRoute->end(), stopEdge);
            }
        }
    } else {
        if (stopPar.index == STOP_INDEX_FIT) {
            while (iter != myStops.end() && (iter->edge < stop.edge ||
                                             (iter->pars.endPos < stop.pars.endPos && iter->edge == stop.edge))) {
                prevStopEdge = iter->edge;
                prevStopPos = iter->pars.endPos;
                ++iter;
            }
        } else {
            int index = stopPar.index;
            while (index > 0) {
                prevStopEdge = iter->edge;
                prevStopPos = iter->pars.endPos;
                ++iter;
                --index;
            }
            stop.edge = find(prevStopEdge, myRoute->end(), stopEdge);
        }
    }
    if (stop.edge == myRoute->end() || prevStopEdge > stop.edge ||
            (prevStopEdge == stop.edge && prevStopPos > stop.pars.endPos && !collision)
            || (stop.lane->getEdge().isInternal() && stop.lane->getNextNormal() != *(stop.edge + 1))) {
        if (stop.edge != myRoute->end()) {
            // check if the edge occurs again later in the route
            MSRouteIterator next = stop.edge + 1;
            return addStop(stopPar, errorMsg, untilOffset, collision, &next);
        }
        errorMsg = errorMsgStart + " for vehicle '" + myParameter->id + "' on lane '" + stopPar.lane + "' is not downstream the current route.";
        return false;
    }
    // David.C:
    //if (!stop.parking && (myCurrEdge == stop.edge && myState.myPos > stop.endPos - getCarFollowModel().brakeGap(myState.mySpeed))) {
    const double endPosOffset = stop.lane->getEdge().isInternal() ? (*stop.edge)->getLength() : 0;
    if (collision) {
        assert(myCurrEdge == stop.edge);
        myState.myPos = stop.pars.endPos;
        myState.mySpeed = 0;
    } else if (myCurrEdge == stop.edge && myState.myPos > stop.pars.endPos + endPosOffset - getCarFollowModel().brakeGap(myState.mySpeed)) {
        errorMsg = errorMsgStart + " for vehicle '" + myParameter->id + "' on lane '" + stopPar.lane + "' is too close to break.";
        return false;
    }
    if (!hasDeparted() && myCurrEdge == stop.edge) {
        double pos = -1;
        if (myParameter->departPosProcedure == DEPART_POS_GIVEN) {
            pos = myParameter->departPos;
            if (pos < 0.) {
                pos += (*myCurrEdge)->getLength();
            }
        }
        if (myParameter->departPosProcedure == DEPART_POS_BASE || myParameter->departPosProcedure == DEPART_POS_DEFAULT) {
            pos = MIN2(static_cast<double>(getVehicleType().getLength() + POSITION_EPS), (*myCurrEdge)->getLength());
        }
        if (pos > stop.pars.endPos + endPosOffset) {
            if (stop.edge != myRoute->end()) {
                // check if the edge occurs again later in the route
                MSRouteIterator next = stop.edge + 1;
                return addStop(stopPar, errorMsg, untilOffset, collision, &next);
            }
            errorMsg = errorMsgStart + " for vehicle '" + myParameter->id + "' on lane '" + stopPar.lane + "' is before departPos.";
            return false;
        }
    }
    if (iter != myStops.begin()) {
        std::list<Stop>::iterator iter2 = iter;
        iter2--;
        if (stop.pars.until >= 0 && iter2->pars.until > stop.pars.until) {
            errorMsg = errorMsgStart + " for vehicle '" + myParameter->id + "' on lane '" + stopPar.lane + "' ends earlier than previous stop.";
        }
    }
    myStops.insert(iter, stop);
    return true;
}


bool
MSVehicle::replaceParkingArea(MSParkingArea* parkingArea, std::string& errorMsg) {
    // Check if there is a parking area to be replaced
    assert(parkingArea != 0);
    if (myStops.empty()) {
        errorMsg = "Vehicle '" + myParameter->id + "' has no stops.";
        return false;
    }
    SUMOVehicleParameter::Stop stopPar;
    Stop stop = myStops.front();
    if (stop.reached) {
        errorMsg = "current stop already reached";
        return false;
    }
    if (stop.parkingarea == 0) {
        errorMsg = "current stop is not a parkingArea";
        return false;
    }
    if (stop.parkingarea == parkingArea) {
        errorMsg = "current stop is the same as the new parking area";
        return false;
    }
    stopPar.lane = parkingArea->getLane().getID();

    // merge duplicated stops equals to parking area
    int removeStops = 0;
    SUMOTime duration = 0;

    for (std::list<Stop>::const_iterator iter = myStops.begin(); iter != myStops.end(); ++iter) {
        if (duration == 0) {
            duration = iter->duration;
            ++removeStops;
        } else {
            if (iter->parkingarea != 0 && iter->parkingarea == parkingArea) {
                duration += iter->duration;
                ++removeStops;
            } else {
                break;
            }
        }
    }

    stopPar.index = 0;
    stopPar.busstop = "";
    stopPar.chargingStation = "";
    stopPar.containerstop = "";
    stopPar.parkingarea = parkingArea->getID();
    stopPar.startPos = parkingArea->getBeginLanePosition();
    stopPar.endPos = parkingArea->getEndLanePosition();
    stopPar.duration = duration;
    stopPar.until = stop.pars.until;
    stopPar.awaitedPersons = stop.pars.awaitedPersons;
    stopPar.awaitedContainers = stop.pars.awaitedContainers;
    stopPar.triggered = stop.pars.triggered;
    stopPar.containerTriggered = stop.pars.containerTriggered;
    stopPar.parking = stop.pars.parking;
    stopPar.parametersSet = stop.pars.parametersSet;

    // remove stops equals to parking area
    while (removeStops > 0) {
        myStops.pop_front();
        --removeStops;
    }
    const bool result = addStop(stopPar, errorMsg);
    if (myLane != 0) {
        updateBestLanes(true);
    }
    return result;
}


MSParkingArea*
MSVehicle::getNextParkingArea() {
    MSParkingArea* nextParkingArea = 0;
    if (!myStops.empty()) {
        SUMOVehicleParameter::Stop stopPar;
        Stop stop = myStops.front();
        if (!stop.reached && stop.parkingarea != 0) {
            nextParkingArea = stop.parkingarea;
        }
    }
    return nextParkingArea;
}


bool
MSVehicle::isStopped() const {
    return !myStops.empty() && myStops.begin()->reached /*&& myState.mySpeed < SUMO_const_haltingSpeed @todo #1864#*/;
}

bool
MSVehicle::isStoppedOnLane() const {
    return isStopped() && myStops.front().lane == myLane;
}

bool
MSVehicle::keepStopping(bool afterProcessing) const {
    if (isStopped()) {
        // after calling processNextStop, DELTA_T has already been subtracted from the duration
        return myStops.front().duration + (afterProcessing ? DELTA_T : 0) > 0 || isStoppedTriggered() || myStops.front().collision;
    } else {
        return false;
    }
}

SUMOTime
MSVehicle::collisionStopTime() const {
    return (myStops.empty() || !myStops.front().collision) ? myCollisionImmunity : MAX2((SUMOTime)0, myStops.front().duration);
}


bool
MSVehicle::isParking() const {
    return isStopped() && myStops.begin()->pars.parking;
}


bool
MSVehicle::isStoppedTriggered() const {
    return isStopped() && (myStops.begin()->triggered || myStops.begin()->containerTriggered);
}


bool
MSVehicle::isStoppedInRange(double pos) const {
    return isStopped() && myStops.begin()->pars.startPos <= pos && myStops.begin()->pars.endPos >= pos;
}


double
MSVehicle::processNextStop(double currentVelocity) {
    if (myStops.empty()) {
        // no stops; pass
        return currentVelocity;
    }

#ifdef DEBUG_STOPS
    if (DEBUG_COND) {
        std::cout << "\nPROCESS_NEXT_STOP\n" << SIMTIME << " vehicle '" << getID() << "'" << std::endl;
    }
#endif

    Stop& stop = myStops.front();
    if (stop.reached) {

#ifdef DEBUG_STOPS
        if (DEBUG_COND) {
            std::cout << SIMTIME << " vehicle '" << getID() << "' reached stop.\n"
                      << "Remaining duration: " << STEPS2TIME(stop.duration) << std::endl;
        }
#endif
        // ok, we have already reached the next stop
        // any waiting persons may board now
        MSNet* const net = MSNet::getInstance();
        const bool boarded = net->hasPersons() && net->getPersonControl().boardAnyWaiting(&myLane->getEdge(), this, &stop) && stop.numExpectedPerson == 0;
        // load containers
        const bool loaded = net->hasContainers() && net->getContainerControl().loadAnyWaiting(&myLane->getEdge(), this, &stop) && stop.numExpectedContainer == 0;
        if (boarded) {
            if (stop.busstop != 0) {
                const std::vector<MSTransportable*>& persons = myPersonDevice->getTransportables();
                for (std::vector<MSTransportable*>::const_iterator i = persons.begin(); i != persons.end(); ++i) {
                    stop.busstop->removeTransportable(*i);
                }
            }
            // the triggering condition has been fulfilled. Maybe we want to wait a bit longer for additional riders (car pooling)
            stop.triggered = false;
            if (myAmRegisteredAsWaitingForPerson) {
                MSNet::getInstance()->getVehicleControl().unregisterOneWaiting(true);
                myAmRegisteredAsWaitingForPerson = false;
#ifdef DEBUG_STOPS
                if (DEBUG_COND) {
                    std::cout << SIMTIME << " vehicle '" << getID() << "' unregisters as waiting for person." << std::endl;
                }
#endif
            }
        }
        if (loaded) {
            if (stop.containerstop != 0) {
                const std::vector<MSTransportable*>& containers = myContainerDevice->getTransportables();
                for (std::vector<MSTransportable*>::const_iterator i = containers.begin(); i != containers.end(); ++i) {
                    stop.containerstop->removeTransportable(*i);
                }
            }
            // the triggering condition has been fulfilled
            stop.containerTriggered = false;
            if (myAmRegisteredAsWaitingForContainer) {
                MSNet::getInstance()->getVehicleControl().unregisterOneWaiting(false);
                myAmRegisteredAsWaitingForContainer = false;
#ifdef DEBUG_STOPS
                if (DEBUG_COND) {
                    std::cout << SIMTIME << " vehicle '" << getID() << "' unregisters as waiting for container." << std::endl;
                }
#endif
            }
        }
        if (!keepStopping() && isOnRoad()) {
#ifdef DEBUG_STOPS
            if (DEBUG_COND) {
                std::cout << SIMTIME << " vehicle '" << getID() << "' resumes from stopping." << std::endl;
            }
#endif
            resumeFromStopping();
        } else {
            if (stop.triggered && !myAmRegisteredAsWaitingForPerson) {
                if (getVehicleType().getPersonCapacity() == getPersonNumber()) {
                    WRITE_WARNING("Vehicle '" + getID() + "' ignores triggered stop on lane '" + stop.lane->getID() + "' due to capacity constraints.");
                    stop.triggered = false;
                }
                // we can only register after waiting for one step. otherwise we might falsely signal a deadlock
                MSNet::getInstance()->getVehicleControl().registerOneWaiting(true);
                myAmRegisteredAsWaitingForPerson = true;
#ifdef DEBUG_STOPS
                if (DEBUG_COND) {
                    std::cout << SIMTIME << " vehicle '" << getID() << "' registers as waiting for person." << std::endl;
                }
#endif
            }
            if (stop.containerTriggered && !myAmRegisteredAsWaitingForContainer) {
                if (getVehicleType().getContainerCapacity() == getContainerNumber()) {
                    WRITE_WARNING("Vehicle '" + getID() + "' ignores container triggered stop on lane '" + stop.lane->getID() + "' due to capacity constraints.");
                    stop.containerTriggered = false;
                }
                // we can only register after waiting for one step. otherwise we might falsely signal a deadlock
                MSNet::getInstance()->getVehicleControl().registerOneWaiting(false);
                myAmRegisteredAsWaitingForContainer = true;
#ifdef DEBUG_STOPS
                if (DEBUG_COND) {
                    std::cout << SIMTIME << " vehicle '" << getID() << "' registers as waiting for container." << std::endl;
                }
#endif
            }
            // we have to wait some more time
            stop.duration -= getActionStepLength();

            if (MSGlobals::gSemiImplicitEulerUpdate) {
                // euler
                return 0;
            } else {
                // ballistic:
                return getCarFollowModel().stopSpeed(this, getSpeed(), stop.getEndPos(*this) - myState.pos());
            }
        }
    } else {

#ifdef DEBUG_STOPS
        if (DEBUG_COND) {
            std::cout << SIMTIME << " vehicle '" << getID() << "' hasn't reached next stop." << std::endl;
        }
#endif

        // is the next stop on the current lane?
        if (stop.edge == myCurrEdge) {
            // get the stopping position
            double endPos = stop.pars.endPos;
            bool useStoppingPlace = false;
            bool fitsOnStoppingPlace = true;
            if (stop.busstop != 0) {
                useStoppingPlace = true;
                // on bus stops, we have to wait for free place if they are in use...
                endPos = stop.busstop->getLastFreePos(*this);
                // at least half the bus has to fit on non-empty bus stops
                if (endPos != stop.busstop->getEndLanePosition() && endPos - myType->getLength() / 2. < stop.busstop->getBeginLanePosition()) {
                    fitsOnStoppingPlace = false;
                }
            }
            // if the stop is a container stop we check if the vehicle fits into the last free position of the stop
            if (stop.containerstop != 0) {
                useStoppingPlace = true;
                // on container stops, we have to wait for free place if they are in use...
                endPos = stop.containerstop->getLastFreePos(*this);
                if (endPos != stop.containerstop->getEndLanePosition() && endPos - myType->getLength() / 2. < stop.containerstop->getBeginLanePosition()) {
                    fitsOnStoppingPlace = false;
                }
            }
            // if the stop is a parking area we check if there is a free position on the area
            if (stop.parkingarea != 0) {
                endPos = stop.parkingarea->getLastFreePos(*this);
                if (stop.parkingarea->getOccupancy() == stop.parkingarea->getCapacity()) {
                    fitsOnStoppingPlace = false;
                    // leave enough space so parking vehicles can exit
                    endPos -= getVehicleType().getMinGap();
                    // trigger potential parkingZoneReroute
                    for (std::vector< MSMoveReminder* >::const_iterator rem = myLane->getMoveReminders().begin(); rem != myLane->getMoveReminders().end(); ++rem) {
                        addReminder(*rem);
                    }
                    MSParkingArea* oldParkingArea = stop.parkingarea;
                    activateReminders(MSMoveReminder::NOTIFICATION_PARKING_REROUTE, myLane);
                    if (myStops.empty() || myStops.front().parkingarea != oldParkingArea) {
                        // rerouted, keep driving
                        return currentVelocity;
                    }
                }
            }
            if (stop.lane->getEdge().isInternal() && &myLane->getEdge() != &stop.lane->getEdge()) {
                // endPos is on subsequent edge
                endPos += myLane->getLength();
            }

            const double reachedThreshold = (useStoppingPlace ? endPos - STOPPING_PLACE_OFFSET : stop.pars.startPos) - NUMERICAL_EPS;
            if (myState.pos() >= reachedThreshold && fitsOnStoppingPlace && currentVelocity <= SUMO_const_haltingSpeed && myLane == stop.lane) {
                // ok, we may stop (have reached the stop)
                stop.reached = true;
#ifdef DEBUG_STOPS
                if (DEBUG_COND) {
                    std::cout << SIMTIME << " vehicle '" << getID() << "' reached next stop." << std::endl;
                }
#endif
                if (MSStopOut::active()) {
                    MSStopOut::getInstance()->stopStarted(this, getPersonNumber(), getContainerNumber());
                }
                MSNet::getInstance()->getVehicleControl().addWaiting(&myLane->getEdge(), this);
                MSNet::getInstance()->informVehicleStateListener(this, MSNet::VEHICLE_STATE_STARTING_STOP);
                // compute stopping time
                if (stop.pars.until >= 0) {
                    if (stop.duration == -1) {
                        stop.duration = stop.pars.until - MSNet::getInstance()->getCurrentTimeStep();
                    } else {
                        stop.duration = MAX2(stop.duration, stop.pars.until - MSNet::getInstance()->getCurrentTimeStep());
                    }
                }
                if (stop.busstop != 0) {
                    // let the bus stop know the vehicle
                    stop.busstop->enter(this, myState.pos() + getVehicleType().getMinGap(), myState.pos() - myType->getLength());
                }
                if (stop.containerstop != 0) {
                    // let the container stop know the vehicle
                    stop.containerstop->enter(this, myState.pos() + getVehicleType().getMinGap(), myState.pos() - myType->getLength());
                }
                if (stop.parkingarea != 0) {
                    // let the parking area know the vehicle
                    stop.parkingarea->enter(this, myState.pos() + getVehicleType().getMinGap(), myState.pos() - myType->getLength());
                }
            }
            // decelerate
            if (MSGlobals::gSemiImplicitEulerUpdate) {
                // euler
                return getCarFollowModel().stopSpeed(this, getSpeed(), endPos - myState.pos() + NUMERICAL_EPS);
            } else {
                // ballistic
                return getCarFollowModel().stopSpeed(this, myState.mySpeed, endPos - myState.myPos);
            }
        }
    }
    return currentVelocity;
}


const ConstMSEdgeVector
MSVehicle::getStopEdges() const {
    ConstMSEdgeVector result;
    for (std::list<Stop>::const_iterator iter = myStops.begin(); iter != myStops.end(); ++iter) {
        result.push_back(*iter->edge);
    }
    return result;
}


bool
MSVehicle::checkActionStep(const SUMOTime t) {
    myActionStep = isActionStep(t);
    if (myActionStep) {
        myLastActionTime = t;
    }
    return myActionStep;
}

void
MSVehicle::resetActionOffset(const SUMOTime timeUntilNextAction) {
    myLastActionTime = MSNet::getInstance()->getCurrentTimeStep() + timeUntilNextAction;
}

void
MSVehicle::updateActionOffset(const SUMOTime oldActionStepLength, const SUMOTime newActionStepLength) {
    SUMOTime now = MSNet::getInstance()->getCurrentTimeStep();
    SUMOTime timeSinceLastAction = now - myLastActionTime;
    if (timeSinceLastAction == 0) {
        // Action was scheduled now, may be delayed be new action step length
        timeSinceLastAction = oldActionStepLength;
    }
    if (timeSinceLastAction >= newActionStepLength) {
        // Action point required in this step
        myLastActionTime = now;
    } else {
        SUMOTime timeUntilNextAction = newActionStepLength - timeSinceLastAction;
        resetActionOffset(timeUntilNextAction);
    }
}



void
MSVehicle::planMove(const SUMOTime t, const MSLeaderInfo& ahead, const double lengthsInFront) {
#ifdef DEBUG_PLAN_MOVE
    if (DEBUG_COND) {
        std::cout
                << "\nPLAN_MOVE\n"
                << STEPS2TIME(t)
                << " veh=" << getID()
                << " lane=" << myLane->getID()
                << " pos=" << getPositionOnLane()
                << " posLat=" << getLateralPositionOnLane()
                << " speed=" << getSpeed()
                << "\n";
    }
#endif
    if (!checkActionStep(t)) {
#ifdef DEBUG_ACTIONSTEPS
        if DEBUG_COND {
        std::cout << STEPS2TIME(t) << " vehicle '" << getID() << "' skips action." << std::endl;
        }
#endif
        // During non-action passed drive items still need to be removed
        removePassedDriveItems();
        return;
    } else {
#ifdef DEBUG_ACTIONSTEPS
        if DEBUG_COND {
        std::cout << STEPS2TIME(t) << " vehicle = '" << getID() << "' takes action." << std::endl;
        }
#endif


        planMoveInternal(t, ahead, myLFLinkLanes, myStopDist);
#ifdef DEBUG_PLAN_MOVE
        if (DEBUG_COND) {
            DriveItemVector::iterator i;
            for (i = myLFLinkLanes.begin(); i != myLFLinkLanes.end(); ++i) {
                std::cout
                        << " vPass=" << (*i).myVLinkPass
                        << " vWait=" << (*i).myVLinkWait
                        << " linkLane=" << ((*i).myLink == 0 ? "NULL" : (*i).myLink->getViaLaneOrLane()->getID())
                        << " request=" << (*i).mySetRequest
                        << "\n";
            }
        }
#endif
        checkRewindLinkLanes(lengthsInFront, myLFLinkLanes);
        myNextDriveItem = myLFLinkLanes.begin();
#ifdef DEBUG_PLAN_MOVE
        if (DEBUG_COND) {
            std::cout << " after checkRewindLinkLanes\n";
            DriveItemVector::iterator i;
            for (i = myLFLinkLanes.begin(); i != myLFLinkLanes.end(); ++i) {
                std::cout
                        << " vPass=" << (*i).myVLinkPass
                        << " vWait=" << (*i).myVLinkWait
                        << " linkLane=" << ((*i).myLink == 0 ? "NULL" : (*i).myLink->getViaLaneOrLane()->getID())
                        << " request=" << (*i).mySetRequest
                        << " atime=" << (*i).myArrivalTime
                        << " atimeB=" << (*i).myArrivalTimeBraking
                        << "\n";
            }
        }
#endif
    }
    getLaneChangeModel().resetChanged();
}


void
MSVehicle::planMoveInternal(const SUMOTime t, MSLeaderInfo ahead, DriveItemVector& lfLinks, double& myStopDist) const {
    // remove information about approaching links, will be reset later in this step
    removeApproachingInformation(lfLinks);
    lfLinks.clear();
    myStopDist = std::numeric_limits<double>::max();
    //
    const MSCFModel& cfModel = getCarFollowModel();
    const double vehicleLength = getVehicleType().getLength();
    const double maxV = cfModel.maxNextSpeed(myState.mySpeed, this);
    const bool opposite = getLaneChangeModel().isOpposite();
    double laneMaxV = myLane->getVehicleMaxSpeed(this);
    // v is the initial maximum velocity of this vehicle in this step
    double v = MIN2(maxV, laneMaxV);
    if (myInfluencer != 0) {
        //const double vMin = MAX2(0., cfModel.getSpeedAfterMaxDecel(myState.mySpeed));
        const double vMin = MAX2(0., cfModel.minNextSpeed(myState.mySpeed, this));
        v = myInfluencer->influenceSpeed(MSNet::getInstance()->getCurrentTimeStep(), v, v, vMin, maxV);
    }
    // all links within dist are taken into account (potentially)
    // the distance already "seen"; in the following always up to the end of the current "lane"
    const double dist = SPEED2DIST(maxV) + cfModel.brakeGap(maxV);

    const std::vector<MSLane*>& bestLaneConts = getBestLanesContinuation();
#ifdef DEBUG_PLAN_MOVE
    if (DEBUG_COND) {
        std::cout << "   bestLaneConts=" << toString(bestLaneConts) << "\n";
    }
#endif
    assert(bestLaneConts.size() > 0);
    bool hadNonInternal = false;
    double seen = opposite ? myState.myPos : myLane->getLength() - myState.myPos; // the distance already "seen"; in the following always up to the end of the current "lane"
    double seenNonInternal = 0;
    double vLinkPass = MIN2(cfModel.estimateSpeedAfterDistance(seen, v, cfModel.getMaxAccel()), laneMaxV); // upper bound
    int view = 0;
    DriveProcessItem* lastLink = 0;
    bool slowedDownForMinor = false; // whether the vehicle already had to slow down on approach to a minor link
    // iterator over subsequent lanes and fill lfLinks until stopping distance or stopped
    const MSLane* lane = opposite ? myLane->getOpposite() : myLane;
    assert(lane != 0);
    const MSLane* leaderLane = myLane;
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4127) // do not warn about constant conditional expression
#endif
    while (true) {
#ifdef _MSC_VER
#pragma warning(pop)
#endif
        // check leader on lane
        //  leader is given for the first edge only
        if (opposite &&
                (leaderLane->getVehicleNumber() > 1
                 || (leaderLane != myLane && leaderLane->getVehicleNumber() > 0))) {
            // find opposite-driving leader that must be respected on the currently looked at lane
            // XXX make sure to look no further than leaderLane
            CLeaderDist leader = leaderLane->getOppositeLeader(this, getPositionOnLane(), true);
            ahead.clear();
            if (leader.first != 0 && leader.first->getLane() == leaderLane && leader.first->getLaneChangeModel().isOpposite()) {
                ahead.addLeader(leader.first, true);
            }
        }
        adaptToLeaders(ahead, 0, seen, lastLink, leaderLane, v, vLinkPass);
#ifdef DEBUG_PLAN_MOVE
        if (DEBUG_COND) {
            std::cout << "\nv = " << v << "\n";

        }
#endif
        // XXX efficiently adapt to shadow leaders using neighAhead by iteration over the whole edge in parallel (lanechanger-style)
        if (getLaneChangeModel().getShadowLane() != 0) {
            // also slow down for leaders on the shadowLane relative to the current lane
            const MSLane* shadowLane = getLaneChangeModel().getShadowLane(lane);
            if (shadowLane != 0) {
                const double latOffset = getLane()->getRightSideOnEdge() - getLaneChangeModel().getShadowLane()->getRightSideOnEdge();
                adaptToLeaders(shadowLane->getLastVehicleInformation(this, latOffset, lane->getLength() - seen),
                               latOffset,
                               seen, lastLink, shadowLane, v, vLinkPass);
            }
        }
        // adapt to pedestrians on the same lane
        if (lane->getEdge().getPersons().size() > 0 && MSPModel::getModel()->hasPedestrians(lane)) {
            const double relativePos = lane->getLength() - seen;
#ifdef DEBUG_PLAN_MOVE
            if (DEBUG_COND) {
                std::cout << SIMTIME << " adapt to pedestrians on lane=" << lane->getID() << " relPos=" << relativePos << "\n";
            }
#endif
            PersonDist leader = MSPModel::getModel()->nextBlocking(lane, relativePos,
                                getRightSideOnLane(), getRightSideOnLane() + getVehicleType().getWidth(), ceil(getSpeed() / cfModel.getMaxDecel()));
            if (leader.first != 0) {
                const double stopSpeed = cfModel.stopSpeed(this, getSpeed(), leader.second - getVehicleType().getMinGap());
                v = MIN2(v, stopSpeed);
#ifdef DEBUG_PLAN_MOVE
                if (DEBUG_COND) {
                    std::cout << SIMTIME << "    pedLeader=" << leader.first->getID() << " dist=" << leader.second << " v=" << v << "\n";
                }
#endif
            }
        }

        // process stops
        if (!myStops.empty() && &myStops.begin()->lane->getEdge() == &lane->getEdge() && !myStops.begin()->reached
                // ignore stops that occur later in a looped route
                && myStops.front().edge == myCurrEdge + view) {
            // we are approaching a stop on the edge; must not drive further
            const Stop& stop = *myStops.begin();
            const double endPos = stop.getEndPos(*this) + NUMERICAL_EPS;
            myStopDist = seen + endPos - lane->getLength();
            const double stopSpeed = cfModel.stopSpeed(this, getSpeed(), myStopDist);
            if (lastLink != 0) {
                lastLink->adaptLeaveSpeed(cfModel.stopSpeed(this, vLinkPass, endPos));
            }
            v = MIN2(v, stopSpeed);
            lfLinks.push_back(DriveProcessItem(v, myStopDist));

#ifdef DEBUG_PLAN_MOVE
            if (DEBUG_COND) {
                std::cout << "\n" << SIMTIME << " next stop: distance = " << myStopDist << " requires stopSpeed = " << stopSpeed << "\n";

            }
#endif

            break;
        }

        // move to next lane
        //  get the next link used
        MSLinkCont::const_iterator link = MSLane::succLinkSec(*this, view + 1, *lane, bestLaneConts);
        //  check whether the vehicle is on its final edge
        if (myCurrEdge + view + 1 == myRoute->end()) {
            const double arrivalSpeed = (myParameter->arrivalSpeedProcedure == ARRIVAL_SPEED_GIVEN ?
                                         myParameter->arrivalSpeed : laneMaxV);
            // subtract the arrival speed from the remaining distance so we get one additional driving step with arrival speed
            // XXX: This does not work for ballistic update refs #2579
            const double distToArrival = seen + myArrivalPos - lane->getLength() - SPEED2DIST(arrivalSpeed);
            const double va = MAX2(NUMERICAL_EPS, cfModel.freeSpeed(this, getSpeed(), distToArrival, arrivalSpeed));
            v = MIN2(v, va);
            if (lastLink != 0) {
                lastLink->adaptLeaveSpeed(va);
            }
            lfLinks.push_back(DriveProcessItem(v, seen, lane->getEdge().isFringe() ? 1000 : 0));
            break;
        }
        // check whether the lane or the shadowLane is a dead end (allow some leeway on intersections)
        if (lane->isLinkEnd(link) ||
                ((*link)->getViaLane() == 0
                 && getLateralOverlap() > POSITION_EPS
                 // do not get stuck on narrow edges
                 && getVehicleType().getWidth() <= lane->getEdge().getWidth()
                 // this is the exit link of a junction. The normal edge should support the shadow
                 && ((getLaneChangeModel().getShadowLane((*link)->getLane()) == 0)
                     // the internal lane after an internal junction has no parallel lane. make sure there is no shadow before continuing
                     || (lane->getEdge().isInternal() && lane->getIncomingLanes()[0].lane->getEdge().isInternal()))
                 // ignore situations where the shadow lane is part of a double-connection with the current lane
                 && (getLaneChangeModel().getShadowLane() == 0
                     || getLaneChangeModel().getShadowLane()->getLinkCont().size() == 0
                     || getLaneChangeModel().getShadowLane()->getLinkCont().front()->getLane() != (*link)->getLane())
                )) {
            double va = MIN2(cfModel.stopSpeed(this, getSpeed(), seen), laneMaxV);
            if (lastLink != 0) {
                lastLink->adaptLeaveSpeed(va);
            }
            if (getLaneChangeModel().getCommittedSpeed() > 0) {
                v = MIN2(getLaneChangeModel().getCommittedSpeed(), v);
            } else {
                v = MIN2(va, v);
            }
#ifdef DEBUG_PLAN_MOVE
            if (DEBUG_COND) {
                std::cout << "   braking for link end: overlap=" << getLateralOverlap() << " va=" << va << " committed=" << getLaneChangeModel().getCommittedSpeed() << " v=" << v << "\n";

            }
#endif
            if (lane->isLinkEnd(link)) {
                lfLinks.push_back(DriveProcessItem(v, seen));
                break;
            }
        }
        const bool yellowOrRed = (*link)->haveRed() || (*link)->haveYellow();
        // We distinguish 3 cases when determining the point at which a vehicle stops:
        // - links that require stopping: here the vehicle needs to stop close to the stop line
        //   to ensure it gets onto the junction in the next step. Otherwise the vehicle would 'forget'
        //   that it already stopped and need to stop again. This is necessary pending implementation of #999
        // - red/yellow light: here the vehicle 'knows' that it will have priority eventually and does not need to stop on a precise spot
        // - other types of minor links: the vehicle needs to stop as close to the junction as necessary
        //   to minimize the time window for passing the junction. If the
        //   vehicle 'decides' to accelerate and cannot enter the junction in
        //   the next step, new foes may appear and cause a collision (see #1096)
        // - major links: stopping point is irrelevant
        const double laneStopOffset = yellowOrRed || (*link)->havePriority() ? DIST_TO_STOPLINE_EXPECT_PRIORITY : POSITION_EPS;
        const double stopDist = MAX2(0., seen - laneStopOffset);
        // check whether we need to slow down in order to finish a continuous lane change
        if (getLaneChangeModel().isChangingLanes()) {
            if (    // slow down to finish lane change before a turn lane
                ((*link)->getDirection() == LINKDIR_LEFT || (*link)->getDirection() == LINKDIR_RIGHT) ||
                // slow down to finish lane change before the shadow lane ends
                (getLaneChangeModel().getShadowLane() != 0 &&
                 (*link)->getViaLaneOrLane()->getParallelLane(getLaneChangeModel().getShadowDirection()) == 0)) {
                // XXX maybe this is too harsh. Vehicles could cut some corners here
                const double timeRemaining = STEPS2TIME(getLaneChangeModel().remainingTime());
                assert(timeRemaining != 0); // we seem to suppose that isChangingLanes() implies this (Leo)
                // XXX: Euler-logic (#860), but I couldn't identify problems from this yet (Leo). Refs. #2575
                const double va = MAX2(0., (seen - POSITION_EPS) / timeRemaining);
#ifdef DEBUG_PLAN_MOVE
                if (DEBUG_COND) std::cout << SIMTIME << " veh=" << getID() << " slowing down to finish continuous change before"
                                              << " link=" << (*link)->getViaLaneOrLane()->getID()
                                              << " timeRemaining=" << timeRemaining
                                              << " v=" << v
                                              << " va=" << va
                                              << "\n";
#endif
                v = MIN2(va, v);
            }
        }

        // - always issue a request to leave the intersection we are currently on
        const bool leavingCurrentIntersection = myLane->getEdge().isInternal() && lastLink == 0;
        // - do not issue a request to enter an intersection after we already slowed down for an earlier one
        const bool abortRequestAfterMinor = slowedDownForMinor && (*link)->getInternalLaneBefore() == 0;
        // - even if red, if we cannot break we should issue a request
        bool setRequest = (v > 0 && !abortRequestAfterMinor) || (leavingCurrentIntersection);

        double vLinkWait = MIN2(v, cfModel.stopSpeed(this, getSpeed(), stopDist));
        const double brakeDist = cfModel.brakeGap(myState.mySpeed, cfModel.getMaxDecel(), 0.);
        const bool canBrake = seen >= brakeDist;
#ifdef DEBUG_PLAN_MOVE
        if (DEBUG_COND) {
            std::cout
                    << " stopDist=" << stopDist
                    << " vLinkWait=" << vLinkWait
                    << " brakeDist=" << brakeDist
                    << "\n";
        }
#endif
        if (yellowOrRed && canBrake && !ignoreRed(*link, canBrake)) {
            // the vehicle is able to brake in front of a yellow/red traffic light
            lfLinks.push_back(DriveProcessItem(*link, vLinkWait, vLinkWait, false, t + TIME2STEPS(seen / MAX2(vLinkWait, NUMERICAL_EPS)), vLinkWait, 0, 0, seen));
            //lfLinks.push_back(DriveProcessItem(0, vLinkWait, vLinkWait, false, 0, 0, stopDist));
            break;
        }

        if (ignoreRed(*link, canBrake) && STEPS2TIME(t - (*link)->getLastStateChange()) > 2) {
            // restrict speed when ignoring a red light
            const double redSpeed = MIN2(v, getVehicleType().getParameter().getJMParam(SUMO_ATTR_JM_DRIVE_RED_SPEED, v));
            const double va = MAX2(redSpeed, cfModel.freeSpeed(this, getSpeed(), seen, redSpeed));
            v = MIN2(va, v);
#ifdef DEBUG_EXEC_MOVE
            if (DEBUG_COND) std::cout
                        << "   ignoreRed spent=" << STEPS2TIME(t - (*link)->getLastStateChange())
                        << " redSpeed=" << redSpeed
                        << " va=" << va
                        << " v=" << v
                        << "\n";
#endif
        }

        if (MSGlobals::gUsingInternalLanes) {
            // we want to pass the link but need to check for foes on internal lanes
            checkLinkLeader(*link, lane, seen, lastLink, v, vLinkPass, vLinkWait, setRequest);
            if (getLaneChangeModel().getShadowLane() != 0) {
                MSLink* parallelLink = (*link)->getParallelLink(getLaneChangeModel().getShadowDirection());
                if (parallelLink != 0) {
                    checkLinkLeader(parallelLink, lane, seen, lastLink, v, vLinkPass, vLinkWait, setRequest, true);
                }
            }
        }

        if (lastLink != 0) {
            lastLink->adaptLeaveSpeed(laneMaxV);
        }
        double arrivalSpeed = vLinkPass;
        // vehicles should decelerate when approaching a minor link
        // - unless they are close enough to have clear visibility of all relevant foe lanes and may start to accelerate again
        // - and unless they are so close that stopping is impossible (i.e. when a green light turns to yellow when close to the junction)

        // whether the vehicle/driver is close enough to the link to see all possible foes #2123
        double visibilityDistance = (*link)->getFoeVisibilityDistance();
        double determinedFoePresence = seen < visibilityDistance;
//        // VARIANT: account for time needed to recognize whether relevant vehicles are on the foe lanes. (Leo)
//        double foeRecognitionTime = 0.0;
//        double determinedFoePresence = seen < visibilityDistance - myState.mySpeed*foeRecognitionTime;

#ifdef DEBUG_PLAN_MOVE
        if (DEBUG_COND) {
            std::cout << " approaching link=" << (*link)->getViaLaneOrLane()->getID() << " prio=" << (*link)->havePriority() << " seen=" << seen << " visibilityDistance=" << visibilityDistance << " brakeDist=" << brakeDist << "\n";
        }
#endif

        if (!(*link)->havePriority() && !determinedFoePresence && brakeDist < seen && !(*link)->lastWasContMajor()) {
            // vehicle decelerates just enough to be able to stop if necessary and then accelerates
            double maxSpeedAtVisibilityDist = cfModel.maximumSafeStopSpeed(visibilityDistance, myState.mySpeed, false, 0.);
            // XXX: estimateSpeedAfterDistance does not use euler-logic (thus returns a lower value than possible here...)
            double maxArrivalSpeed = cfModel.estimateSpeedAfterDistance(visibilityDistance, maxSpeedAtVisibilityDist, cfModel.getMaxAccel());
            arrivalSpeed = MIN2(vLinkPass, maxArrivalSpeed);
            slowedDownForMinor = true;
#ifdef DEBUG_PLAN_MOVE
            if (DEBUG_COND) {
                std::cout << "   slowedDownForMinor maxArrivalSpeed=" << maxArrivalSpeed << " arrivalSpeed=" << arrivalSpeed << "\n";
            }
#endif
        }

        SUMOTime arrivalTime;
        if (MSGlobals::gSemiImplicitEulerUpdate) {
            // @note intuitively it would make sense to compare arrivalSpeed with getSpeed() instead of v
            // however, due to the current position update rule (ticket #860) the vehicle moves with v in this step
            // subtract DELTA_T because t is the time at the end of this step and the movement is not carried out yet
            arrivalTime = t - DELTA_T + cfModel.getMinimalArrivalTime(seen, v, arrivalSpeed);
        } else {
            arrivalTime = t - DELTA_T + cfModel.getMinimalArrivalTime(seen, myState.mySpeed, arrivalSpeed);
        }

        // compute arrival speed and arrival time if vehicle starts braking now
        // if stopping is possible, arrivalTime can be arbitrarily large. A small value keeps fractional times (impatience) meaningful
        double arrivalSpeedBraking = 0;
        SUMOTime arrivalTimeBraking = arrivalTime + TIME2STEPS(30);
        if (seen < cfModel.brakeGap(v)) { // XXX: should this use the current speed (at least for the ballistic case)? (Leo) Refs. #2575
            // vehicle cannot come to a complete stop in time
            if (MSGlobals::gSemiImplicitEulerUpdate) {
                arrivalSpeedBraking = cfModel.getMinimalArrivalSpeedEuler(seen, v);
                // due to discrete/continuous mismatch (when using Euler update) we have to ensure that braking actually helps
                arrivalSpeedBraking = MIN2(arrivalSpeedBraking, arrivalSpeed);
            } else {
                arrivalSpeedBraking = cfModel.getMinimalArrivalSpeed(seen, myState.mySpeed);
            }
            arrivalTimeBraking = MAX2(arrivalTime, t + TIME2STEPS(seen / ((v + arrivalSpeedBraking) * 0.5)));
        }
        lfLinks.push_back(DriveProcessItem(*link, v, vLinkWait, setRequest,
                                           arrivalTime, arrivalSpeed,
                                           arrivalTimeBraking, arrivalSpeedBraking,
                                           seen,
                                           estimateLeaveSpeed(*link, arrivalSpeed)));
        if ((*link)->getViaLane() == 0) {
            hadNonInternal = true;
            ++view;
        }
        // we need to look ahead far enough to see available space for checkRewindLinkLanes
        if ((!setRequest || v <= 0 || seen > dist) && hadNonInternal && seenNonInternal > vehicleLength * CRLL_LOOK_AHEAD) {
            break;
        }
        // get the following lane
        lane = (*link)->getViaLaneOrLane();
        laneMaxV = lane->getVehicleMaxSpeed(this);
        // the link was passed
        // compute the velocity to use when the link is not blocked by other vehicles
        //  the vehicle shall be not faster when reaching the next lane than allowed
        const double va = cfModel.freeSpeed(this, getSpeed(), seen, laneMaxV);
        v = MIN2(va, v);
        seenNonInternal += lane->getEdge().isInternal() ? 0 : lane->getLength();
        // do not restrict results to the current vehicle to allow caching for the current time step
        leaderLane = opposite ? lane->getOpposite() : lane;
        if (leaderLane == 0) {
            break;
        }
        ahead = opposite ? MSLeaderInfo(leaderLane) : leaderLane->getLastVehicleInformation(0, 0);
        seen += lane->getLength();
        vLinkPass = MIN2(cfModel.estimateSpeedAfterDistance(lane->getLength(), v, cfModel.getMaxAccel()), laneMaxV); // upper bound
        lastLink = &lfLinks.back();
    }

//#ifdef DEBUG_PLAN_MOVE
//    if(DEBUG_COND){
//        std::cout << "planMoveInternal found safe speed v = " << v << std::endl;
//    }
//#endif

}


void
MSVehicle::adaptToLeaders(const MSLeaderInfo& ahead, double latOffset,
                          const double seen, DriveProcessItem* const lastLink,
                          const MSLane* const lane, double& v, double& vLinkPass) const {
    int rightmost;
    int leftmost;
    ahead.getSubLanes(this, latOffset, rightmost, leftmost);
#ifdef DEBUG_PLAN_MOVE
    if (DEBUG_COND) std::cout << SIMTIME
                                  << "\nADAPT_TO_LEADERS\nveh=" << getID()
                                  << " lane=" << lane->getID()
                                  << " rm=" << rightmost
                                  << " lm=" << leftmost
                                  << " ahead=" << ahead.toString()
                                  << "\n";
#endif
    /*
    if (getLaneChangeModel().getCommittedSpeed() > 0) {
        v = MIN2(v, getLaneChangeModel().getCommittedSpeed());
        vLinkPass = MIN2(vLinkPass, getLaneChangeModel().getCommittedSpeed());
    #ifdef DEBUG_PLAN_MOVE
        if (DEBUG_COND) std::cout << "   hasCommitted=" << getLaneChangeModel().getCommittedSpeed() << "\n";
    #endif
        return;
    }
    */
    for (int sublane = rightmost; sublane <= leftmost; ++sublane) {
        const MSVehicle* pred = ahead[sublane];
        if (pred != 0) {
            // @todo avoid multiple adaptations to the same leader
            const double predBack = pred->getBackPositionOnLane(lane);
            const double gap = (lastLink == 0
                                ? predBack - myState.myPos - getVehicleType().getMinGap()
                                : predBack + seen - lane->getLength() - getVehicleType().getMinGap());
#ifdef DEBUG_PLAN_MOVE
            if (DEBUG_COND) {
                std::cout << "     pred=" << pred->getID() << " predLane=" << pred->getLane()->getID() << " predPos=" << pred->getPositionOnLane() << " gap=" << gap << " predBack=" << predBack << " seen=" << seen << " lane=" << lane->getID() << " myLane=" << myLane->getID() << "\n";
            }
#endif
            adaptToLeader(std::make_pair(pred, gap), seen, lastLink, lane, v, vLinkPass);
        }
    }
}


void
MSVehicle::adaptToLeader(const std::pair<const MSVehicle*, double> leaderInfo,
                         const double seen, DriveProcessItem* const lastLink,
                         const MSLane* const lane, double& v, double& vLinkPass,
                         double distToCrossing) const {
    if (leaderInfo.first != 0) {
        const double vsafeLeader = getSafeFollowSpeed(leaderInfo, seen, lane, distToCrossing);
        if (lastLink != 0) {
            lastLink->adaptLeaveSpeed(vsafeLeader);
        }
        v = MIN2(v, vsafeLeader);
        vLinkPass = MIN2(vLinkPass, vsafeLeader);

#ifdef DEBUG_PLAN_MOVE
        if (DEBUG_COND) std::cout
                    << SIMTIME
                    //std::cout << std::setprecision(10);
                    << " veh=" << getID()
                    << " lead=" << leaderInfo.first->getID()
                    << " leadSpeed=" << leaderInfo.first->getSpeed()
                    << " gap=" << leaderInfo.second
                    << " leadLane=" << leaderInfo.first->getLane()->getID()
                    << " predPos=" << leaderInfo.first->getPositionOnLane()
                    << " seen=" << seen
                    << " lane=" << lane->getID()
                    << " myLane=" << myLane->getID()
                    << " dTC=" << distToCrossing
                    << " v=" << v
                    << " vSafeLeader=" << vsafeLeader
                    << " vLinkPass=" << vLinkPass
                    << "\n";
#endif
    }
}


void
MSVehicle::checkLinkLeader(const MSLink* link, const MSLane* lane, double seen,
                           DriveProcessItem* const lastLink, double& v, double& vLinkPass, double& vLinkWait, bool& setRequest,
                           bool isShadowLink) const {
    const MSLink::LinkLeaders linkLeaders = link->getLeaderInfo(this, seen, 0, isShadowLink);
    for (MSLink::LinkLeaders::const_iterator it = linkLeaders.begin(); it != linkLeaders.end(); ++it) {
        // the vehicle to enter the junction first has priority
        const MSVehicle* leader = (*it).vehAndGap.first;
        if (leader == 0) {
            // leader is a pedestrian. Passing 'this' as a dummy.
            //std::cout << SIMTIME << " veh=" << getID() << " is blocked on link to " << (*link)->getViaLaneOrLane()->getID() << " by pedestrian. dist=" << it->distToCrossing << "\n";
            adaptToLeader(std::make_pair(this, -1), seen, lastLink, lane, v, vLinkPass, it->distToCrossing);
        } else if (link->isLeader(this, leader)) {
            if (MSGlobals::gLateralResolution > 0 &&
                    // sibling link (XXX: could also be partial occupator where this check fails)
                    &leader->getLane()->getEdge() == &lane->getEdge()) {
                // check for sublane obstruction (trivial for sibling link leaders)
                const MSLane* conflictLane = link->getInternalLaneBefore();
                MSLeaderInfo linkLeadersAhead = MSLeaderInfo(conflictLane);
                linkLeadersAhead.addLeader(leader, false, 0); // assume sibling lane has the same geometry as the leader lane
                const double latOffset = isShadowLink ? (getLane()->getRightSideOnEdge() - getLaneChangeModel().getShadowLane()->getRightSideOnEdge()) : 0;
                // leader is neither on lane nor conflictLane (the conflict is only established geometrically)
                adaptToLeaders(linkLeadersAhead, latOffset, seen, lastLink, leader->getLane(), v, vLinkPass);
#ifdef DEBUG_PLAN_MOVE
                if (DEBUG_COND) {
                    std::cout << SIMTIME << " veh=" << getID()
                              << " siblingFoe link=" << link->getViaLaneOrLane()->getID()
                              << " isShadowLink=" << isShadowLink
                              << " lane=" << lane->getID()
                              << " foe=" << leader->getID()
                              << " foeLane=" << leader->getLane()->getID()
                              << " latOffset=" << latOffset
                              << " latOffsetFoe=" << leader->getLatOffset(lane)
                              << " linkLeadersAhead=" << linkLeadersAhead.toString()
                              << "\n";
                }
#endif
            } else {
                adaptToLeader(it->vehAndGap, seen, lastLink, lane, v, vLinkPass, it->distToCrossing);
            }
            if (lastLink != 0) {
                // we are not yet on the junction with this linkLeader.
                // at least we can drive up to the previous link and stop there
                v = MAX2(v, lastLink->myVLinkWait);
            }
            // if blocked by a leader from the same or next lane we must yield our request
            if (v < SUMO_const_haltingSpeed
                    //&& leader->getSpeed() < SUMO_const_haltingSpeed
                    && (leader->getLane()->getLogicalPredecessorLane() == myLane->getLogicalPredecessorLane()
                        || leader->getLane()->getLogicalPredecessorLane() == myLane)) {
                setRequest = false;
                if (lastLink != 0 && leader->getLane()->getLogicalPredecessorLane() == myLane) {
                    // we are not yet on the junction so must abort that request as well
                    // (or maybe we are already on the junction and the leader is a partial occupator beyond)
                    lastLink->mySetRequest = false;
                }
                // other vehicles may become junction leader when yielding
                link->passedJunction(this);
            }
        }
    }
    // if this is the link between two internal lanes we may have to slow down for pedestrians
    vLinkWait = MIN2(vLinkWait, v);
}


double
MSVehicle::getSafeFollowSpeed(const std::pair<const MSVehicle*, double> leaderInfo,
                              const double seen, const MSLane* const lane, double distToCrossing) const {
    assert(leaderInfo.first != 0);
    const MSCFModel& cfModel = getCarFollowModel();
    double vsafeLeader = 0;
    if (!MSGlobals::gSemiImplicitEulerUpdate) {
        vsafeLeader = -std::numeric_limits<double>::max();
    }
    if (leaderInfo.second >= 0) {
        vsafeLeader = cfModel.followSpeed(this, getSpeed(), leaderInfo.second, leaderInfo.first->getSpeed(), leaderInfo.first->getCarFollowModel().getApparentDecel());
    } else {
        // the leading, in-lapping vehicle is occupying the complete next lane
        // stop before entering this lane
        vsafeLeader = cfModel.stopSpeed(this, getSpeed(), seen - lane->getLength() - POSITION_EPS);
    }
    if (distToCrossing >= 0) {
        // drive up to the crossing point with the current link leader
        vsafeLeader = MAX2(vsafeLeader, cfModel.stopSpeed(this, getSpeed(), distToCrossing));
    }
    return vsafeLeader;
}

double
MSVehicle::getDeltaPos(double accel) {
    double vNext = myState.mySpeed + ACCEL2SPEED(accel);
    if (MSGlobals::gSemiImplicitEulerUpdate) {
        // apply implicit Euler positional update
        return SPEED2DIST(MAX2(vNext, 0.));
    } else {
        // apply ballistic update
        if (vNext >= 0) {
            // assume constant acceleration during this time step
            return SPEED2DIST(myState.mySpeed + 0.5 * ACCEL2SPEED(accel));
        } else {
            // negative vNext indicates a stop within the middle of time step
            // The corresponding stop time is s = mySpeed/deceleration \in [0,dt], and the
            // covered distance is therefore deltaPos = mySpeed*s - 0.5*deceleration*s^2.
            // Here, deceleration = (myState.mySpeed - vNext)/dt is the constant deceleration
            // until the vehicle stops.
            return -SPEED2DIST(0.5 * myState.mySpeed * myState.mySpeed / ACCEL2SPEED(accel));
        }
    }
}

void
MSVehicle::processLinkAproaches(double& vSafe, double& vSafeMin, double& vSafeMinDist) {

    // Speed limit due to zipper merging
    double vSafeZipper = std::numeric_limits<double>::max();

    myHaveToWaitOnNextLink = false;

    // Get safe velocities from DriveProcessItems.
    assert(myLFLinkLanes.size() != 0 || isRemoteControlled());
    for (DriveItemVector::iterator i = myLFLinkLanes.begin(); i != myLFLinkLanes.end(); ++i) {
        MSLink* link = (*i).myLink;

#ifdef DEBUG_EXEC_MOVE
        if (DEBUG_COND) {
            std::cout
                    << SIMTIME
                    << " veh=" << getID()
                    << " link=" << (link == 0 ? "NULL" : link->getViaLaneOrLane()->getID())
                    << " req=" << (*i).mySetRequest
                    << " vP=" << (*i).myVLinkPass
                    << " vW=" << (*i).myVLinkWait
                    << " d=" << (*i).myDistance
                    << "\n";
            gDebugFlag1 = true; // See MSLink_DEBUG_OPENED
        }
#endif

        // the vehicle must change the lane on one of the next lanes (XXX: refs to code further below???, Leo)
        if (link != 0 && (*i).mySetRequest) {

            const LinkState ls = link->getState();
            // vehicles should brake when running onto a yellow light if the distance allows to halt in front
            const bool yellow = link->haveYellow();
            const bool canBrake = ((*i).myDistance > getCarFollowModel().brakeGap(myState.mySpeed, getCarFollowModel().getMaxDecel(), 0.)
                                   || (MSGlobals::gSemiImplicitEulerUpdate && myState.mySpeed < ACCEL2SPEED(getCarFollowModel().getMaxDecel())));
            const bool ignoreRedLink = ignoreRed(link, canBrake);
            if (yellow && canBrake && !ignoreRedLink) {
                vSafe = (*i).myVLinkWait;
                myHaveToWaitOnNextLink = true;
                link->removeApproaching(this);
                break;
            }
            const bool influencerPrio = (myInfluencer != 0 && !myInfluencer->getRespectJunctionPriority());
            std::vector<const SUMOVehicle*> collectFoes;
            bool opened = (yellow || influencerPrio
                           || link->opened((*i).myArrivalTime, (*i).myArrivalSpeed, (*i).getLeaveSpeed(),
                                           getVehicleType().getLength(), getImpatience(),
                                           getCarFollowModel().getMaxDecel(),
                                           getWaitingTime(), getLateralPositionOnLane(),
                                           ls == LINKSTATE_ZIPPER ? &collectFoes : 0,
                                           ignoreRedLink, this));
            if (opened && getLaneChangeModel().getShadowLane() != 0) {
                MSLink* parallelLink = (*i).myLink->getParallelLink(getLaneChangeModel().getShadowDirection());
                if (parallelLink != 0) {
                    const double shadowLatPos = getLateralPositionOnLane() - getLaneChangeModel().getShadowDirection() * 0.5 * (
                                                    myLane->getWidth() + getLaneChangeModel().getShadowLane()->getWidth());
                    opened = yellow || influencerPrio || (opened & parallelLink->opened((*i).myArrivalTime, (*i).myArrivalSpeed, (*i).getLeaveSpeed(),
                                                          getVehicleType().getLength(), getImpatience(),
                                                          getCarFollowModel().getMaxDecel(),
                                                          getWaitingTime(), shadowLatPos, 0,
                                                          ignoreRedLink, this));
#ifdef DEBUG_EXEC_MOVE
                    if (DEBUG_COND) {
                        std::cout << SIMTIME
                                  << " veh=" << getID()
                                  << " shadowLane=" << getLaneChangeModel().getShadowLane()->getID()
                                  << " shadowDir=" << getLaneChangeModel().getShadowDirection()
                                  << " parallelLink=" << (parallelLink == 0 ? "NULL" : parallelLink->getViaLaneOrLane()->getID())
                                  << " opened=" << opened
                                  << "\n";
                    }
#endif
                }
            }
            // vehicles should decelerate when approaching a minor link
            if (opened && !influencerPrio && !link->havePriority() && !link->lastWasContMajor() && !link->isCont() && !ignoreRedLink) {
                double visibilityDistance = link->getFoeVisibilityDistance();
                double determinedFoePresence = i->myDistance <= visibilityDistance;
                if (!determinedFoePresence) {
                    vSafe = (*i).myVLinkWait;
                    myHaveToWaitOnNextLink = true;
                    if (ls == LINKSTATE_EQUAL) {
                        link->removeApproaching(this);
                    }
                    break;
                } else {
                    // past the point of no return. we need to drive fast enough
                    // to make it across the link. However, minor slowdowns
                    // should be permissible to follow leading traffic safely
                    // XXX: There is a problem in subsecond simulation: If we cannot
                    // make it across the minor link in one step, new traffic
                    // could appear on a major foe link and cause a collision. Refs. #1845, #2123
                    vSafeMinDist = myLane->getLength() - getPositionOnLane(); // distance that must be covered
                    if (MSGlobals::gSemiImplicitEulerUpdate) {
                        vSafeMin = MIN2((double) DIST2SPEED(vSafeMinDist + POSITION_EPS), (*i).myVLinkPass);
                    } else {
                        vSafeMin = MIN2((double) DIST2SPEED(2 * vSafeMinDist + NUMERICAL_EPS) - getSpeed(), (*i).myVLinkPass);
                    }
                }
            }
            // have waited; may pass if opened...
            if (opened) {
                vSafe = (*i).myVLinkPass;
                if (vSafe < getCarFollowModel().getMaxDecel() && vSafe <= (*i).myVLinkWait && vSafe < getCarFollowModel().maxNextSpeed(getSpeed(), this)) {
                    // this vehicle is probably not gonna drive across the next junction (heuristic)
                    myHaveToWaitOnNextLink = true;
                }
            } else if (link->getState() == LINKSTATE_ZIPPER) {
                vSafeZipper = MIN2(vSafeZipper,
                                   link->getZipperSpeed(this, (*i).myDistance, (*i).myVLinkPass, (*i).myArrivalTime, &collectFoes));
            } else {
                vSafe = (*i).myVLinkWait;
                myHaveToWaitOnNextLink = true;
                if (ls == LINKSTATE_EQUAL) {
                    link->removeApproaching(this);
                }
#ifdef DEBUG_EXEC_MOVE
                if (DEBUG_COND) {
                    std::cout << SIMTIME << " braking for closed link=" << link->getViaLaneOrLane()->getID() << "\n";
                }
#endif
                break;
            }
        } else {
            // we have: i->link == 0 || !i->setRequest
            vSafe = (*i).myVLinkWait;
            if (vSafe < getSpeed()) {
                myHaveToWaitOnNextLink = true;
            }
            break;
        }
    }

//#ifdef DEBUG_EXEC_MOVE
//    if (DEBUG_COND) {
//        std::cout << "\nvCurrent = " << toString(getSpeed(), 24) << "" << std::endl;
//        std::cout << "vSafe = " << toString(vSafe, 24) << "" << std::endl;
//        std::cout << "vSafeMin = " << toString(vSafeMin, 24) << "" << std::endl;
//        std::cout << "vSafeMinDist = " << toString(vSafeMinDist, 24) << "" << std::endl;
//
//        double gap = getLeader().second;
//        std::cout << "gap = " << toString(gap, 24) << std::endl;
//        std::cout << "vSafeStoppedLeader = " << toString(getCarFollowModel().stopSpeed(this, getSpeed(), gap), 24)
//                << "\n" << std::endl;
//    }
//#endif

    if ((MSGlobals::gSemiImplicitEulerUpdate && vSafe + NUMERICAL_EPS < vSafeMin)
            || (!MSGlobals::gSemiImplicitEulerUpdate && (vSafe + NUMERICAL_EPS < vSafeMin && vSafeMin != 0))) { // this might be good for the euler case as well
        // cannot drive across a link so we need to stop before it
        // XXX: (Leo) This often called stopSpeed with vSafeMinDist==0 (for the ballistic update), since vSafe can become negative
        //      For the Euler update the term '+ NUMERICAL_EPS' prevented a call here... Recheck, consider of -INVALID_SPEED instead of 0 to indicate absence of vSafeMin restrictions. Refs. #2577
        vSafe = MIN2(vSafe, getCarFollowModel().stopSpeed(this, getSpeed(), vSafeMinDist));
        vSafeMin = 0;
        myHaveToWaitOnNextLink = true;

#ifdef DEBUG_EXEC_MOVE
        if (DEBUG_COND) {
            std::cout << "vSafeMin Problem?" << std::endl;
        }
#endif

    }

    // vehicles inside a roundabout should maintain their requests
    if (myLane->getEdge().isRoundabout()) {
        myHaveToWaitOnNextLink = false;
    }

    vSafe = MIN2(vSafe, vSafeZipper);
}


double
MSVehicle::processTraCISpeedControl(double vSafe, double vNext) {
    if (myInfluencer != 0) {
        if (myInfluencer->isRemoteControlled()) {
            vNext = myInfluencer->implicitSpeedRemote(this, myState.mySpeed);
        }
        const double vMax = getVehicleType().getCarFollowModel().maxNextSpeed(myState.mySpeed, this);
        const double vMin = MAX2(0., getVehicleType().getCarFollowModel().minNextSpeed(myState.mySpeed, this));
        vNext = myInfluencer->influenceSpeed(MSNet::getInstance()->getCurrentTimeStep(), vNext, vSafe, vMin, vMax);
    }
    return vNext;
}


void
MSVehicle::removePassedDriveItems() {
#ifdef DEBUG_ACTIONSTEPS
    if DEBUG_COND {
    std::cout << SIMTIME << " veh=" << getID() << " removePassedDriveItems()\n"
        << "    Current items: ";
        for (auto& j : myLFLinkLanes) {
            if (j.myLink == 0) {
                std::cout << "\n    Stop at distance " << j.myDistance;
            } else {
                const MSLane* to = j.myLink->getViaLaneOrLane();
                const MSLane* from = j.myLink->getLaneBefore();
                std::cout << "\n    Link at distance " << j.myDistance << ": '"
                          << (from == 0 ? "NONE" : from->getID()) << "' -> '" << (to == 0 ? "NONE" : to->getID()) << "'";
            }
        }
        std::cout << "\n    myNextDriveItem: ";
        if (myLFLinkLanes.size() != 0) {
            if (myNextDriveItem->myLink == 0) {
                std::cout << "\n    Stop at distance " << myNextDriveItem->myDistance;
            } else {
                const MSLane* to = myNextDriveItem->myLink->getViaLaneOrLane();
                const MSLane* from = myNextDriveItem->myLink->getLaneBefore();
                std::cout << "\n    Link at distance " << myNextDriveItem->myDistance << ": '"
                          << (from == 0 ? "NONE" : from->getID()) << "' -> '" << (to == 0 ? "NONE" : to->getID()) << "'";
            }
        }
        std::cout << std::endl;
    }
#endif
    for (auto j = myLFLinkLanes.begin(); j != myNextDriveItem; ++j) {
#ifdef DEBUG_ACTIONSTEPS
        if DEBUG_COND {
        std::cout << "    Removing item: ";
        if (j->myLink == 0) {
                std::cout << "Stop at distance " << j->myDistance;
            } else {
                const MSLane* to = j->myLink->getViaLaneOrLane();
                const MSLane* from = j->myLink->getLaneBefore();
                std::cout << "Link at distance " << j->myDistance << ": '"
                << (from == 0 ? "NONE" : from->getID()) << "' -> '" << (to == 0 ? "NONE" : to->getID()) << "'";
            }
            std::cout << std::endl;
        }
#endif
        if (j->myLink != 0) {
            j->myLink->removeApproaching(this);
        }
    }
    myLFLinkLanes.erase(myLFLinkLanes.begin(), myNextDriveItem);
    myNextDriveItem = myLFLinkLanes.begin();
}


void
MSVehicle::updateDriveItems() {
#ifdef DEBUG_ACTIONSTEPS
    if (DEBUG_COND) {
        std::cout << SIMTIME << " updateDriveItems(), veh='" << getID() << "' (lane: '" << getLane()->getID() << "')\nCurrent drive items:" << std::endl;
        DriveItemVector::iterator i;
        for (i = myLFLinkLanes.begin(); i != myLFLinkLanes.end(); ++i) {
            std::cout
                    << " vPass=" << (*i).myVLinkPass
                    << " vWait=" << (*i).myVLinkWait
                    << " linkLane=" << ((*i).myLink == 0 ? "NULL" : (*i).myLink->getViaLaneOrLane()->getID())
                    << " request=" << (*i).mySetRequest
                    << "\n";
        }
        std::cout << " myNextDriveItem's linkLane: " << (myNextDriveItem->myLink == 0 ? "NULL" : myNextDriveItem->myLink->getViaLaneOrLane()->getID()) << std::endl;
    }
#endif
    if (myLFLinkLanes.size() == 0) {
        // nothing to update
        return;
    }
    const MSLink* nextPlannedLink = 0;
//    auto i = myLFLinkLanes.begin();
    auto i = myNextDriveItem;
    while (i != myLFLinkLanes.end() && nextPlannedLink == 0) {
        nextPlannedLink = i->myLink;
        ++i;
    }

    if (nextPlannedLink == 0) {
        // No link for upcoming item -> no need for an update
#ifdef DEBUG_ACTIONSTEPS
        if (DEBUG_COND) {
            std::cout << "Found no link-related drive item." << std::endl;
        }
#endif
        return;
    }

    if (getLane() == nextPlannedLink->getLaneBefore()) {
        // Current lane approaches the stored next link, i.e. no LC happend and no update is required.
#ifdef DEBUG_ACTIONSTEPS
        if (DEBUG_COND) {
            std::cout << "Continuing on planned lane sequence, no update required." << std::endl;
        }
#endif
        return;
    }
    // Lane must have been changed, determine the change direction
    MSLink* parallelLink = nextPlannedLink->getParallelLink(1);
    if (parallelLink != 0 && parallelLink->getLaneBefore() == getLane()) {
        // lcDir = 1;
    } else {
        parallelLink = nextPlannedLink->getParallelLink(-1);
        if (parallelLink != 0 && parallelLink->getLaneBefore() == getLane()) {
            // lcDir = -1;
        } else {
            // If the vehicle's current lane is not the approaching lane for the next
            // drive process item's link, it is expected to lead to a parallel link,
            // XXX: What if the lc was an overtaking maneuver and there is no upcoming link?
            //      Then a stop item should be scheduled! -> TODO!
            //assert(false);
            return;
        }
    }
#ifdef DEBUG_ACTIONSTEPS
    if (DEBUG_COND) {
        std::cout << "Changed lane. Drive items will be updated along the current lane continuation." << std::endl;
    }
#endif
    // Trace link sequence along current best lanes and transfer drive items to the corresponding links
//        DriveItemVector::iterator driveItemIt = myLFLinkLanes.begin();
    DriveItemVector::iterator driveItemIt = myNextDriveItem;
    // In the loop below, lane holds the currently considered lane on the vehicles continuation (including internal lanes)
    MSLane* lane = myLane;
    // *lit is a pointer to the next lane in best continuations for the current lane (always non-internal)
    std::vector<MSLane*>::const_iterator bestLaneIt = getBestLanesContinuation().begin();
    // if the vehicle's current lane is internal, the first entry in the best continuations is null
    assert((*bestLaneIt) == 0 || !myLane->isInternal());
    // Pointer to the new link for the current drive process item
    MSLink* newLink = 0;
    while (driveItemIt != myLFLinkLanes.end()) {
        if (driveItemIt->myLink == 0) {
            // Items not related to a specific link are not updated
            // (XXX: when a stop item corresponded to a dead end, which is overcome by the LC that made
            //       the update necessary, this may slow down the vehicle's continuation on the new lane...)
            ++driveItemIt;
            continue;
        }
        // Set nextBestLane to the next non-null consecutive lane entry
        while (bestLaneIt != getBestLanesContinuation().end() && (*bestLaneIt == 0 || (*bestLaneIt)->getID() == myLane->getID())) {
            ++bestLaneIt;
        }
        // Continuation links for current best lanes are less than for the former drive items (myLFLinkLanes)
        // We just remove the leftover link-items, as they cannot be mapped to new links.
        if (bestLaneIt == getBestLanesContinuation().end()) {
#ifdef DEBUG_ACTIONSTEPS
            if (DEBUG_COND) {
                std::cout << "Reached end of the new continuation sequence. Erasing leftover link-items." << std::endl;
            }
#endif
            while (driveItemIt != myLFLinkLanes.end()) {
                if (driveItemIt->myLink == 0) {
                    ++driveItemIt;
                    continue;
                } else {
                    driveItemIt->myLink->removeApproaching(this);
                    driveItemIt = myLFLinkLanes.erase(driveItemIt);
                }
            }
            break;
        }
        // Do the actual link-remapping for the item. And un/register approaching information on the corresponding links
        newLink = lane->getLinkTo(*bestLaneIt);

        if (newLink == driveItemIt->myLink) {
            // new continuation merged into previous - stop update
#ifdef DEBUG_ACTIONSTEPS
            if (DEBUG_COND) {
                std::cout << "Old and new continuation sequences merge at link\n"
                          << "'" << newLink->getLaneBefore()->getID() << "'->'" << newLink->getViaLaneOrLane()->getID() << "'"
                          << "\nNo update beyond merge required." << std::endl;
            }
#endif
            break;
        }

#ifdef DEBUG_ACTIONSTEPS
        if (DEBUG_COND) {
            std::cout << "Updating link\n'" << driveItemIt->myLink->getLaneBefore()->getID() << "'->'" << driveItemIt->myLink->getViaLaneOrLane()->getID() << "'"
                      << std::endl;
            std::cout << "\n'" << newLink->getLaneBefore()->getID() << "'->'" << newLink->getViaLaneOrLane()->getID() << "'" << std::endl;
        }
#endif
        newLink->setApproaching(this, driveItemIt->myLink->getApproaching(this));
        driveItemIt->myLink->removeApproaching(this);
        driveItemIt->myLink = newLink;
        lane = newLink->getViaLaneOrLane();
        ++driveItemIt;
        if (!lane->isInternal()) {
            ++bestLaneIt;
        }
    }
#ifdef DEBUG_ACTIONSTEPS
    if (DEBUG_COND) {
        std::cout << "Updated drive items:" << std::endl;
        DriveItemVector::iterator i;
        for (i = myLFLinkLanes.begin(); i != myLFLinkLanes.end(); ++i) {
            std::cout
                    << " vPass=" << (*i).myVLinkPass
                    << " vWait=" << (*i).myVLinkWait
                    << " linkLane=" << ((*i).myLink == 0 ? "NULL" : (*i).myLink->getViaLaneOrLane()->getID())
                    << " request=" << (*i).mySetRequest
                    << "\n";
        }
    }
#endif
}


void
MSVehicle::setBrakingSignals(double vNext) {
    // To avoid casual blinking brake lights at high speeds due to dawdling of the
    // leading vehicle, we don't show brake lights when the deceleration could be caused
    // by frictional forces and air resistance (i.e. proportional to v^2, coefficient could be adapted further)
    double pseudoFriction = (0.05 +  0.005 * getSpeed()) * getSpeed();
    bool brakelightsOn = vNext < getSpeed() - ACCEL2SPEED(pseudoFriction);

    if (vNext <= SUMO_const_haltingSpeed && !isStopped()) {
        brakelightsOn = true;
    }
    if (brakelightsOn) {
        switchOnSignal(VEH_SIGNAL_BRAKELIGHT);
    } else {
        switchOffSignal(VEH_SIGNAL_BRAKELIGHT);
    }
}


void
MSVehicle::updateWaitingTime(double vNext) {
    if (vNext <= SUMO_const_haltingSpeed && !isStopped()) {
        myWaitingTime += DELTA_T;
        myWaitingTimeCollector.passTime(DELTA_T, true);
    } else {
        myWaitingTime = 0;
        myWaitingTimeCollector.passTime(DELTA_T, false);
    }
}


void
MSVehicle::updateTimeLoss(double vNext) {
    // update time loss (depends on the updated edge)
    if (!isStopped()) {
        const double vmax = myLane->getVehicleMaxSpeed(this);
        if (vmax > 0) {
            myTimeLoss += TS * (vmax - vNext) / vmax;
        }
    }
}


void
MSVehicle::processLaneAdvances(std::vector<MSLane*>& passedLanes, bool& moved, std::string& emergencyReason) {
    for (std::vector<MSLane*>::reverse_iterator i = myFurtherLanes.rbegin(); i != myFurtherLanes.rend(); ++i) {
        passedLanes.push_back(*i);
    }
    if (passedLanes.size() == 0 || passedLanes.back() != myLane) {
        passedLanes.push_back(myLane);
    }
    // move on lane(s)
    if (myState.myPos > myLane->getLength()) {
        // The vehicle has moved at least to the next lane (maybe it passed even more than one)
        if (myCurrEdge != myRoute->end() - 1) {
            MSLane* approachedLane = myLane;
            // move the vehicle forward
            for (myNextDriveItem = myLFLinkLanes.begin(); myNextDriveItem != myLFLinkLanes.end() && approachedLane != 0 && myState.myPos > approachedLane->getLength(); ++myNextDriveItem) {
                MSLink* link = myNextDriveItem->myLink;
                // check whether the vehicle was allowed to enter lane
                //  otherwise it is decelerated and we do not need to test for it's
                //  approach on the following lanes when a lane changing is performed
                // proceed to the next lane
                if (link != 0) {
                    approachedLane = link->getViaLaneOrLane();
                    if (myInfluencer == 0 || myInfluencer->getEmergencyBrakeRedLight()) {
                        if (link->haveRed() && !ignoreRed(link, false)) {
                            emergencyReason = " because of a red traffic light";
                            ++myNextDriveItem;
                            break;
                        }
                    }
                } else if (myState.myPos < myLane->getLength() + NUMERICAL_EPS) {
                    // avoid warning due to numerical instability
                    approachedLane = myLane;
                    myState.myPos = myLane->getLength();
                } else {
                    emergencyReason = " because there is no connection to the next edge";
                    approachedLane = 0;
                    ++myNextDriveItem;
                    break;
                }
                if (approachedLane != myLane && approachedLane != 0) {
                    leaveLane(MSMoveReminder::NOTIFICATION_JUNCTION, approachedLane);
                    myState.myPos -= myLane->getLength();
                    assert(myState.myPos > 0);
                    enterLaneAtMove(approachedLane);
                    if (MSGlobals::gUsingInternalLanes) {
                        // erase leaders when past the junction
                        if (link->getViaLane() == 0) {
                            link->passedJunction(this);
                        }
                    }
                    if (hasArrived()) {
                        ++myNextDriveItem;
                        break;
                    }
                    if (getLaneChangeModel().isChangingLanes()) {
                        if (link->getDirection() == LINKDIR_LEFT || link->getDirection() == LINKDIR_RIGHT) {
                            // abort lane change
                            WRITE_WARNING("Vehicle '" + getID() + "' could not finish continuous lane change (turn lane) time=" +
                                          time2string(MSNet::getInstance()->getCurrentTimeStep()) + ".");
                            getLaneChangeModel().endLaneChangeManeuver();
                        }
                    }
                    moved = true;
                    if (approachedLane->getEdge().isVaporizing()) {
                        leaveLane(MSMoveReminder::NOTIFICATION_VAPORIZED);
                        ++myNextDriveItem;
                        break;
                    }
                    passedLanes.push_back(approachedLane);
                }
            }
            // NOTE: Passed drive items will be erased in the next simstep's planMove()

#ifdef DEBUG_ACTIONSTEPS
            if (DEBUG_COND && myNextDriveItem != myLFLinkLanes.begin()) {
                std::cout << "Updated drive items:" << std::endl;
                for (DriveItemVector::iterator i = myLFLinkLanes.begin(); i != myLFLinkLanes.end(); ++i) {
                    std::cout
                            << " vPass=" << (*i).myVLinkPass
                            << " vWait=" << (*i).myVLinkWait
                            << " linkLane=" << ((*i).myLink == 0 ? "NULL" : (*i).myLink->getViaLaneOrLane()->getID())
                            << " request=" << (*i).mySetRequest
                            << "\n";
                }
            }
#endif
        } else if (!hasArrived() && myState.myPos < myLane->getLength() + NUMERICAL_EPS) {
            // avoid warning due to numerical instability when stopping at the end of the route
            myState.myPos = myLane->getLength();
        }

    }
}



bool
MSVehicle::executeMove() {
#ifdef DEBUG_EXEC_MOVE
    if (DEBUG_COND) std::cout << "\nEXECUTE_MOVE\n"
                                  << SIMTIME
                                  << " veh=" << getID()
                                  << " speed=" << getSpeed() // toString(getSpeed(), 24)
                                  << std::endl;
#endif


    // Maximum safe velocity
    double vSafe = std::numeric_limits<double>::max();
    // Minimum safe velocity (lower bound).
    double vSafeMin = -std::numeric_limits<double>::max();
    // The distance to a link, which should either be crossed this step
    // or in front of which we need to stop.
    double vSafeMinDist = 0;

    if (myActionStep) {
        // Actuate control (i.e. choose bounds for safe speed in current simstep (euler), resp. after current sim step (ballistic))
        processLinkAproaches(vSafe, vSafeMin, vSafeMinDist);
#ifdef DEBUG_ACTIONSTEPS
        if DEBUG_COND {
        std::cout << SIMTIME << " vehicle '" << getID() << "'\n"
            "   vsafe from processLinkApproaches(): vsafe " << vSafe << std::endl;
        }
#endif
    } else {
        // Continue with current acceleration
        vSafe = getSpeed() + ACCEL2SPEED(myAcceleration);
#ifdef DEBUG_ACTIONSTEPS
        if DEBUG_COND {
        std::cout << SIMTIME << " vehicle '" << getID() << "' skips processLinkApproaches()\n"
            "   continues with constant accel " <<  myAcceleration << "...\n"
            << "speed: "  << getSpeed() << " -> " << vSafe << std::endl;
        }
#endif
    }


//#ifdef DEBUG_EXEC_MOVE
//    if (DEBUG_COND) {
//    	std::cout << "vSafe = " << toString(vSafe,12) << "\n" << std::endl;
//    }
//#endif

    // Determine vNext = speed after current sim step (ballistic), resp. in current simstep (euler)
    // Call to finalizeSpeed applies speed reduction due to dawdling / lane changing but ensures minimum safe speed
    double vNext = myActionStep ? MAX2(getCarFollowModel().finalizeSpeed(this, vSafe), vSafeMin) : vSafe;
    // (Leo) to avoid tiny oscillations (< 1e-10) of vNext in a standing vehicle column (observed for ballistic update), we cap off vNext
    //       (We assure to do this only for vNext<<NUMERICAL_EPS since otherwise this would nullify the workaround for #2995
    if (fabs(vNext) < 0.1 * NUMERICAL_EPS * TS) {
        vNext = 0.;
    }
#ifdef DEBUG_EXEC_MOVE
    if (DEBUG_COND) {
        std::cout << SIMTIME << " finalizeSpeed vSafe=" << vSafe << " vSafeMin=" << (vSafeMin == -std::numeric_limits<double>::max() ? "-Inf" : toString(vSafeMin))
                  << " vNext=" << vNext << " (i.e. accel=" << SPEED2ACCEL(vNext - getSpeed()) << "\n";
    }
#endif

    // vNext may be higher than vSafe without implying a bug:
    //  - when approaching a green light that suddenly switches to yellow
    //  - when using unregulated junctions
    //  - when using tau < step-size
    //  - when using unsafe car following models
    //  - when using TraCI and some speedMode / laneChangeMode settings
    //if (vNext > vSafe + NUMERICAL_EPS) {
    //    WRITE_WARNING("vehicle '" + getID() + "' cannot brake hard enough to reach safe speed "
    //            + toString(vSafe, 4) + ", moving at " + toString(vNext, 4) + " instead. time="
    //            + time2string(MSNet::getInstance()->getCurrentTimeStep()) + ".");
    //}

    if (MSGlobals::gSemiImplicitEulerUpdate) {
        vNext = MAX2(vNext, 0.);
    } else {
        // (Leo) Ballistic: negative vNext can be used to indicate a stop within next step.
    }

    // Check for speed advices from the traci client
    vNext = processTraCISpeedControl(vSafe, vNext);

    setBrakingSignals(vNext);
    updateWaitingTime(vNext);

    // update position and speed
    updateState(vNext);

    // Lanes, which the vehicle touched at some moment of the executed simstep
    std::vector<MSLane*> passedLanes;
    // Whether the vehicle did move to another lane
    bool moved = false;
    // Reason for a possible emergency stop
    std::string emergencyReason = " for unknown reasons";
    processLaneAdvances(passedLanes, moved, emergencyReason);

    updateTimeLoss(vNext);
    myCollisionImmunity = MAX2((SUMOTime) - 1, myCollisionImmunity - DELTA_T);

    if (!hasArrived() && !myLane->getEdge().isVaporizing()) {
        if (myState.myPos > myLane->getLength()) {
            WRITE_WARNING("Vehicle '" + getID() + "' performs emergency stop at the end of lane '" + myLane->getID()
                          + "'" + emergencyReason
                          + " (decel=" + toString(myAcceleration - myState.mySpeed)
                          + ", offset=" + toString(myState.myPos - myLane->getLength())
                          + "), time=" + time2string(MSNet::getInstance()->getCurrentTimeStep()) + ".");
            MSNet::getInstance()->getVehicleControl().registerEmergencyStop();
            myState.myPos = myLane->getLength();
            myState.mySpeed = 0;
            myAcceleration = 0;
            // reset drive items and link approaches
            removeApproachingInformation(myLFLinkLanes);
            myLFLinkLanes.clear();
            myNextDriveItem = myLFLinkLanes.begin();
        }
        const MSLane* oldBackLane = getBackLane();
        if (getLaneChangeModel().isOpposite()) {
            passedLanes.clear(); // ignore back occupation
        }
#ifdef DEBUG_ACTIONSTEPS
        if DEBUG_COND {
        std::cout << SIMTIME << " veh '" << getID() << "' updates further lanes." << std::endl;
        }
#endif
        myState.myBackPos = updateFurtherLanes(myFurtherLanes, myFurtherLanesPosLat, passedLanes);
        // bestLanes need to be updated before lane changing starts. NOTE: This call is also a presumption for updateDriveItems()
        updateBestLanes();
        if (moved || oldBackLane != getBackLane()) {
            if (getLaneChangeModel().getShadowLane() != 0 || getLateralOverlap() > POSITION_EPS) {
                // shadow lane must be updated if the front or back lane changed
                // either if we already have a shadowLane or if there is lateral overlap
                getLaneChangeModel().updateShadowLane();
            }
            // The vehicles target lane must be also be updated if the front or back lane changed
            getLaneChangeModel().updateTargetLane();
        }
        setBlinkerInformation(); // needs updated bestLanes
        //change the blue light only for emergency vehicles SUMOVehicleClass
        if (myType->getVehicleClass() == SVC_EMERGENCY) {
            setEmergencyBlueLight(MSNet::getInstance()->getCurrentTimeStep());
        }
        // State needs to be reset for all vehicles before the next call to MSEdgeControl::changeLanes
        if (myActionStep) {
            // check (#2681): Can this be skipped?
            getLaneChangeModel().prepareStep();
        } else {
#ifdef DEBUG_ACTIONSTEPS
            if DEBUG_COND {
            std::cout << SIMTIME << " veh '" << getID() << "' skips LCM->prepareStep()." << std::endl;
            }
#endif
        }
        myAngle = computeAngle();
    }

#ifdef DEBUG_EXEC_MOVE
    if (DEBUG_COND) {
        std::cout << SIMTIME << " executeMove finished veh=" << getID() << " lane=" << myLane->getID() << " myPos=" << getPositionOnLane() << " myPosLat=" << getLateralPositionOnLane() << "\n";
        gDebugFlag1 = false; // See MSLink_DEBUG_OPENED
    }
#endif
    if (getLaneChangeModel().isOpposite()) {
        // transform back to the opposite-direction lane
        if (myLane->getOpposite() == 0) {
            WRITE_WARNING("Unexpected end of opposite lane for vehicle '" + getID() + " at lane '" + myLane->getID() + "', time=" +
                          time2string(MSNet::getInstance()->getCurrentTimeStep()) + ".");
            getLaneChangeModel().changedToOpposite();
        } else {
            myState.myPos = myLane->getOppositePos(myState.myPos);
            myLane = myLane->getOpposite();
            myCachedPosition = Position::INVALID;
        }
    }
    workOnMoveReminders(myState.myPos - myState.myLastCoveredDist, myState.myPos, myState.mySpeed);
    return moved;
}


void
MSVehicle::updateState(double vNext) {
    // update position and speed
    double deltaPos; // positional change
    if (MSGlobals::gSemiImplicitEulerUpdate) {
        // euler
        deltaPos = SPEED2DIST(vNext);
    } else {
        // ballistic
        deltaPos = getDeltaPos(SPEED2ACCEL(vNext - myState.mySpeed));
    }

    // the *mean* acceleration during the next step (probably most appropriate for emission calculation)
    // NOTE: for the ballistic update vNext may be negative, indicating a stop.
    myAcceleration = SPEED2ACCEL(MAX2(vNext, 0.) - myState.mySpeed);

#ifdef DEBUG_EXEC_MOVE
    if (DEBUG_COND) {
        std::cout << SIMTIME << " updateState() for veh '" << getID() << "': deltaPos=" << deltaPos
                  << " pos=" << myState.myPos << " newPos=" << myState.myPos + deltaPos << std::endl;
    }
#endif

    myState.myPreviousSpeed = myState.mySpeed;
    myState.mySpeed = MAX2(vNext, 0.);

    if (myInfluencer != 0 && myInfluencer->isRemoteControlled()) {
        deltaPos = myInfluencer->implicitDeltaPosRemote(this);
    }

    if (getLaneChangeModel().isOpposite()) {
        // transform to the forward-direction lane, move and then transform back
        myState.myPos = myLane->getOppositePos(myState.myPos);
        myLane = myLane->getOpposite();
    }
    myState.myPos += deltaPos;
    myState.myLastCoveredDist = deltaPos;

    myCachedPosition = Position::INVALID;
}


const MSLane*
MSVehicle::getBackLane() const {
    if (myFurtherLanes.size() > 0) {
        return myFurtherLanes.back();
    } else {
        return myLane;
    }
}


double
MSVehicle::updateFurtherLanes(std::vector<MSLane*>& furtherLanes, std::vector<double>& furtherLanesPosLat,
                              const std::vector<MSLane*>& passedLanes) {
#ifdef DEBUG_FURTHER
    if (DEBUG_COND) std::cout << SIMTIME
                                  << " updateFurtherLanes oldFurther=" << toString(furtherLanes)
                                  << " oldFurtherPosLat=" << toString(furtherLanesPosLat)
                                  << " passed=" << toString(passedLanes)
                                  << "\n";
#endif
    for (std::vector<MSLane*>::iterator i = furtherLanes.begin(); i != furtherLanes.end(); ++i) {
        (*i)->resetPartialOccupation(this);
    }

    std::vector<MSLane*> newFurther;
    std::vector<double> newFurtherPosLat;
    double backPosOnPreviousLane = myState.myPos - getLength();
    if (passedLanes.size() > 1) {
        // There are candidates for further lanes. (passedLanes[-1] is the current lane, or current shadow lane in context of updateShadowLanes())
        std::vector<MSLane*>::const_iterator fi = furtherLanes.begin();
        std::vector<double>::const_iterator fpi = furtherLanesPosLat.begin();
        for (auto pi = passedLanes.rbegin() + 1; pi != passedLanes.rend() && backPosOnPreviousLane < 0; ++pi) {
            // As long as vehicle back reaches into passed lane, add it to the further lanes
            newFurther.push_back(*pi);
            backPosOnPreviousLane += (*pi)->setPartialOccupation(this);
            if (fi != furtherLanes.end() && *pi == *fi) {
                // Lateral position on this lane is already known. Assume constant and use old value.
                newFurtherPosLat.push_back(*fpi);
                ++fi;
                ++fpi;
            } else {
                // The lane *pi was not in furtherLanes before.
                // If it is downstream, we assume as lateral position the current position
                // If it is a new lane upstream (can appear as shadow further in case of LC-maneuvering, e.g.)
                // we assign the last known lateral position.
                if (newFurtherPosLat.size() == 0) {
                    newFurtherPosLat.push_back(myState.myPosLat);
                } else {
                    newFurtherPosLat.push_back(newFurtherPosLat.back());
                }
            }
#ifdef DEBUG_FURTHER
            if (DEBUG_COND) {
                std::cout << SIMTIME << " updateFurtherLanes \n"
                          << "    further lane '" << (*pi)->getID() << "' backPosOnPreviousLane=" << backPosOnPreviousLane
                          << std::endl;
            }
#endif
        }
        furtherLanes = newFurther;
        furtherLanesPosLat = newFurtherPosLat;
    } else {
        furtherLanes.clear();
        furtherLanesPosLat.clear();
    }
#ifdef DEBUG_FURTHER
    if (DEBUG_COND) std::cout
                << " newFurther=" << toString(furtherLanes)
                << " newFurtherPosLat=" << toString(furtherLanesPosLat)
                << " newBackPos=" << backPosOnPreviousLane
                << "\n";
#endif
    return backPosOnPreviousLane;
}


double
MSVehicle::getBackPositionOnLane(const MSLane* lane) const {
#ifdef DEBUG_FURTHER
    if (DEBUG_COND) {
        std::cout << SIMTIME
                  << " getBackPositionOnLane veh=" << getID()
                  << " lane=" << Named::getIDSecure(lane)
                  << " myLane=" << myLane->getID()
                  << " further=" << toString(myFurtherLanes)
                  << " furtherPosLat=" << toString(myFurtherLanesPosLat)
                  << "\n     shadowLane=" << Named::getIDSecure(getLaneChangeModel().getShadowLane())
                  << " shadowFurther=" << toString(getLaneChangeModel().getShadowFurtherLanes())
                  << " shadowFurtherPosLat=" << toString(getLaneChangeModel().getShadowFurtherLanesPosLat())
                  << "\n     targetLane=" << Named::getIDSecure(getLaneChangeModel().getTargetLane())
                  << " furtherTargets=" << toString(getLaneChangeModel().getFurtherTargetLanes())
                  << std::endl;
    }
#endif
    if (lane == myLane
            || lane == getLaneChangeModel().getShadowLane()
            || lane == getLaneChangeModel().getTargetLane()) {
        if (getLaneChangeModel().isOpposite()) {
            return myState.myPos + myType->getLength();
        } else {
            return myState.myPos - myType->getLength();
        }
    } else if ((myFurtherLanes.size() > 0 && lane == myFurtherLanes.back())
               || (getLaneChangeModel().getShadowFurtherLanes().size() > 0 && lane == getLaneChangeModel().getShadowFurtherLanes().back())
               || (getLaneChangeModel().getFurtherTargetLanes().size() > 0 && lane == getLaneChangeModel().getFurtherTargetLanes().back())) {
        return myState.myBackPos;
    } else {
        //if (DEBUG_COND) std::cout << SIMTIME << " veh=" << getID() << " myFurtherLanes=" << toString(myFurtherLanes) << "\n";
        double leftLength = myType->getLength() - myState.myPos;

        std::vector<MSLane*>::const_iterator i = myFurtherLanes.begin();
        while (leftLength > 0 && i != myFurtherLanes.end()) {
            leftLength -= (*i)->getLength();
            //if (DEBUG_COND) std::cout << " comparing i=" << (*i)->getID() << " lane=" << lane->getID() << "\n";
            if (*i == lane) {
                return -leftLength;
            }
            ++i;
        }
        //if (DEBUG_COND) std::cout << SIMTIME << " veh=" << getID() << " myShadowFurtherLanes=" << toString(getLaneChangeModel().getShadowFurtherLanes()) << "\n";
        leftLength = myType->getLength() - myState.myPos;
        i = getLaneChangeModel().getShadowFurtherLanes().begin();
        while (leftLength > 0 && i != getLaneChangeModel().getShadowFurtherLanes().end()) {
            leftLength -= (*i)->getLength();
            //if (DEBUG_COND) std::cout << " comparing i=" << (*i)->getID() << " lane=" << lane->getID() << "\n";
            if (*i == lane) {
                return -leftLength;
            }
            ++i;
        }
        //if (DEBUG_COND) std::cout << SIMTIME << " veh=" << getID() << " myFurtherTargetLanes=" << toString(getLaneChangeModel().getFurtherTargetLanes()) << "\n";
        leftLength = myType->getLength() - myState.myPos;
        i = getFurtherLanes().begin();
        const std::vector<MSLane*> furtherTargetLanes = getLaneChangeModel().getFurtherTargetLanes();
        auto j = furtherTargetLanes.begin();
        while (leftLength > 0 && j != furtherTargetLanes.end()) {
            leftLength -= (*i)->getLength();
            // if (DEBUG_COND) std::cout << " comparing i=" << (*i)->getID() << " lane=" << lane->getID() << "\n";
            if (*j == lane) {
                return -leftLength;
            }
            ++i;
            ++j;
        }
        WRITE_WARNING("Request backPos of vehicle '" + getID() + "' for invalid lane '" + Named::getIDSecure(lane)
                      + "' time=" + time2string(MSNet::getInstance()->getCurrentTimeStep()) + ".")
        assert(false);
        return  myState.myBackPos;
    }
}


double
MSVehicle::getPositionOnLane(const MSLane* lane) const {
    return getBackPositionOnLane(lane) + myType->getLength();
}


bool
MSVehicle::isFrontOnLane(const MSLane* lane) const {
    return lane == myLane || lane == getLaneChangeModel().getShadowLane();
}


double
MSVehicle::getSpaceTillLastStanding(const MSLane* l, bool& foundStopped) const {
    double lengths = 0;
    const MSLane::VehCont& vehs = l->getVehiclesSecure();
    for (MSLane::VehCont::const_iterator i = vehs.begin(); i != vehs.end(); ++i) {
        if ((*i)->getSpeed() < SUMO_const_haltingSpeed && !(*i)->getLane()->getEdge().isRoundabout()
                && (*i) != this
                // @todo recheck
                && (*i)->isFrontOnLane(l)) {
            foundStopped = true;
            const double ret = (*i)->getPositionOnLane() - (*i)->getVehicleType().getLengthWithGap() - lengths;
            l->releaseVehicles();
            return ret;
        }
        lengths += (*i)->getVehicleType().getLengthWithGap();
    }
    l->releaseVehicles();
    return l->getLength() - lengths;
}


void
MSVehicle::checkRewindLinkLanes(const double lengthsInFront, DriveItemVector& lfLinks) const {
    if (MSGlobals::gUsingInternalLanes && !myLane->getEdge().isRoundabout() && !getLaneChangeModel().isOpposite()) {
        bool hadVehicle = false;
        double seenSpace = -lengthsInFront;

        bool foundStopped = false;
        // compute available space until a stopped vehicle is found
        // this is the sum of non-interal lane length minus in-between vehicle lenghts
        for (int i = 0; i < (int)lfLinks.size(); ++i) {
            // skip unset links
            DriveProcessItem& item = lfLinks[i];
            if (item.myLink == 0 || foundStopped) {
                if (!foundStopped) {
                    item.availableSpace += seenSpace;
                } else {
                    item.availableSpace = seenSpace;
                }
                item.hadVehicle = hadVehicle;
                continue;
            }
            // get the next lane, determine whether it is an internal lane
            const MSLane* approachedLane = item.myLink->getViaLane();
            if (approachedLane != 0) {
                if (keepClear(item.myLink)) {
                    seenSpace = seenSpace - approachedLane->getBruttoVehLenSum();
                    hadVehicle |= approachedLane->getVehicleNumber() != 0;
                    if (approachedLane == myLane) {
                        seenSpace += getVehicleType().getLengthWithGap();
                    }
                } else {
                    seenSpace = seenSpace + getSpaceTillLastStanding(approachedLane, foundStopped);// - approachedLane->getBruttoVehLenSum() + approachedLane->getLength();
                    hadVehicle |= approachedLane->getVehicleNumber() != 0;
                }
                item.availableSpace = seenSpace;
                item.hadVehicle = hadVehicle;
#ifdef DEBUG_CHECKREWINDLINKLANES
                if (DEBUG_COND) std::cout
                            << SIMTIME
                            << " veh=" << getID()
                            << " approached=" << approachedLane->getID()
                            << " approachedBrutto=" << approachedLane->getBruttoVehLenSum()
                            << " avail=" << item.availableSpace
                            << " seenSpace=" << seenSpace
                            << " hadVehicle=" << item.hadVehicle
                            << " lengthsInFront=" << lengthsInFront
                            << "\n";
#endif
                continue;
            }
            approachedLane = item.myLink->getLane();
            const MSVehicle* last = approachedLane->getLastAnyVehicle();
            if (last == 0 || last == this) {
                seenSpace += approachedLane->getLength();
                item.availableSpace = seenSpace;
            } else if (!last->isFrontOnLane(approachedLane)) {
                /// XXX backward compatibility: why should partial occupators be treated differently here?
                /// XXX MAX2 redundant?
                item.availableSpace = MAX2(seenSpace, seenSpace + last->getBackPositionOnLane(approachedLane) + last->getCarFollowModel().brakeGap(last->getSpeed()));
                hadVehicle = true;
                /// XXX spaceTillLastStanding should already be covered by getPartialOccupatorEnd()
                seenSpace = seenSpace + getSpaceTillLastStanding(approachedLane, foundStopped);// - approachedLane->getBruttoVehLenSum() + approachedLane->getLength();
                /// XXX why not check BRAKELIGHT?
                if (last->myHaveToWaitOnNextLink) {
                    foundStopped = true;
                }
#ifdef DEBUG_CHECKREWINDLINKLANES
                if (DEBUG_COND) std::cout
                            << SIMTIME
                            << " veh=" << getID()
                            << " approached=" << approachedLane->getID()
                            << " lastPoc=" << last->getID()
                            << " avail=" << item.availableSpace
                            << " seenSpace=" << seenSpace
                            << " foundStopped=" << foundStopped
                            << "\n";
#endif
            } else {

                if (last->signalSet(VEH_SIGNAL_BRAKELIGHT)) {
                    const double lastBrakeGap = last->getCarFollowModel().brakeGap(last->getSpeed());
                    const double lastGap = last->getBackPositionOnLane(approachedLane) + lastBrakeGap - last->getSpeed() * last->getCarFollowModel().getHeadwayTime()
                                           // gap of last up to the next intersection
                                           - last->getVehicleType().getMinGap();
                    item.availableSpace = MAX2(seenSpace, seenSpace + lastGap);
                    seenSpace += getSpaceTillLastStanding(approachedLane, foundStopped);// - approachedLane->getBruttoVehLenSum() + approachedLane->getLength();
                } else {
                    seenSpace += getSpaceTillLastStanding(approachedLane, foundStopped);
                    item.availableSpace = seenSpace;
                }
                if (last->myHaveToWaitOnNextLink) {
                    foundStopped = true;
                }
                hadVehicle = true;
#ifdef DEBUG_CHECKREWINDLINKLANES
                if (DEBUG_COND) std::cout
                            << SIMTIME
                            << " veh=" << getID()
                            << " approached=" << approachedLane->getID()
                            << " last=" << last->getID()
                            << " lastHasToWait=" << last->myHaveToWaitOnNextLink
                            << " lastBrakeLight=" << last->signalSet(VEH_SIGNAL_BRAKELIGHT)
                            << " lastBrakeGap=" << last->getCarFollowModel().brakeGap(last->getSpeed())
                            << " lastGap=" << (last->getBackPositionOnLane(approachedLane) + last->getCarFollowModel().brakeGap(last->getSpeed()) - last->getSpeed() * last->getCarFollowModel().getHeadwayTime()
                                               // gap of last up to the next intersection
                                               - last->getVehicleType().getMinGap())
                            << " avail=" << item.availableSpace
                            << " seenSpace=" << seenSpace
                            << " foundStopped=" << foundStopped
                            << "\n";
#endif
            }
            item.hadVehicle = hadVehicle;
        }

        // check which links allow continuation and add pass available to the previous item
        for (int i = ((int)lfLinks.size() - 1); i > 0; --i) {
            DriveProcessItem& item = lfLinks[i - 1];
            const bool canLeaveJunction = item.myLink->getViaLane() == 0 || lfLinks[i].mySetRequest;
            const bool opened = item.myLink != 0 && canLeaveJunction && (item.myLink->havePriority() ||
                                (myInfluencer != 0 && !myInfluencer->getRespectJunctionPriority()) ||
                                item.myLink->opened(item.myArrivalTime, item.myArrivalSpeed,
                                                    item.getLeaveSpeed(), getVehicleType().getLength(),
                                                    getImpatience(), getCarFollowModel().getMaxDecel(), getWaitingTime(), getLateralPositionOnLane()));
            bool allowsContinuation = item.myLink == 0 || item.myLink->isCont() || !lfLinks[i].hadVehicle || opened;
            if (!opened && item.myLink != 0) {
                if (i > 1) {
                    DriveProcessItem& item2 = lfLinks[i - 2];
                    if (item2.myLink != 0 && item2.myLink->isCont()) {
                        allowsContinuation = true;
                    }
                }
            }
            if (allowsContinuation) {
                item.availableSpace = lfLinks[i].availableSpace;
            }
        }

        // find removalBegin
        int removalBegin = -1;
        for (int i = 0; hadVehicle && i < (int)lfLinks.size() && removalBegin < 0; ++i) {
            // skip unset links
            const DriveProcessItem& item = lfLinks[i];
            if (item.myLink == 0) {
                continue;
            }
            /*
            double impatienceCorrection = MAX2(0., double(double(myWaitingTime)));
            if (seenSpace<getVehicleType().getLengthWithGap()-impatienceCorrection/10.&&nextSeenNonInternal!=0) {
                removalBegin = lastLinkToInternal;
            }
            */

            const double leftSpace = item.availableSpace - getVehicleType().getLengthWithGap();
#ifdef DEBUG_CHECKREWINDLINKLANES
            if (DEBUG_COND) std::cout
                        << SIMTIME
                        << " veh=" << getID()
                        << " link=" << (item.myLink == 0 ? "NULL" : item.myLink->getViaLaneOrLane()->getID())
                        << " avail=" << item.availableSpace
                        << " leftSpace=" << leftSpace
                        << "\n";
#endif
            if (leftSpace < 0/* && item.myLink->willHaveBlockedFoe()*/) {
                double impatienceCorrection = 0;
                /*
                if(item.myLink->getState()==LINKSTATE_MINOR) {
                    impatienceCorrection = MAX2(0., STEPS2TIME(myWaitingTime));
                }
                */
                // may ignore keepClear rules
                if (leftSpace < -impatienceCorrection / 10. && keepClear(item.myLink)) {
                    removalBegin = i;
                }
                //removalBegin = i;
            }
        }
        // abort requests
        if (removalBegin != -1 && !(removalBegin == 0 && myLane->getEdge().isInternal())) {
            while (removalBegin < (int)(lfLinks.size())) {
                const double brakeGap = getCarFollowModel().brakeGap(myState.mySpeed, getCarFollowModel().getMaxDecel(), 0.);
                lfLinks[removalBegin].myVLinkPass = lfLinks[removalBegin].myVLinkWait;
                if (lfLinks[removalBegin].myDistance >= brakeGap || (lfLinks[removalBegin].myDistance > 0 && myState.mySpeed < ACCEL2SPEED(getCarFollowModel().getMaxDecel()))) {
                    lfLinks[removalBegin].mySetRequest = false;
                }
                ++removalBegin;
            }
        }
    }
    for (DriveItemVector::iterator i = lfLinks.begin(); i != lfLinks.end(); ++i) {
        if ((*i).myLink != 0) {
            if ((*i).myLink->getState() == LINKSTATE_ALLWAY_STOP) {
                (*i).myArrivalTime += (SUMOTime)RandHelper::rand((int)2); // tie braker
            }
            (*i).myLink->setApproaching(this, (*i).myArrivalTime, (*i).myArrivalSpeed, (*i).getLeaveSpeed(),
                                        (*i).mySetRequest, (*i).myArrivalTimeBraking, (*i).myArrivalSpeedBraking, getWaitingTime(), (*i).myDistance);
        }
    }
    if (getLaneChangeModel().getShadowLane() != 0) {
        // register on all shadow links
        for (DriveItemVector::iterator i = lfLinks.begin(); i != lfLinks.end(); ++i) {
            if ((*i).myLink != 0) {
                MSLink* parallelLink = (*i).myLink->getParallelLink(getLaneChangeModel().getShadowDirection());
                if (parallelLink != 0) {
                    parallelLink->setApproaching(this, (*i).myArrivalTime, (*i).myArrivalSpeed, (*i).getLeaveSpeed(),
                                                 (*i).mySetRequest, (*i).myArrivalTimeBraking, (*i).myArrivalSpeedBraking, getWaitingTime(), (*i).myDistance);
                    getLaneChangeModel().setShadowApproachingInformation(parallelLink);
                }
            }
        }
    }
}


void
MSVehicle::activateReminders(const MSMoveReminder::Notification reason, const MSLane* enteredLane) {
    for (MoveReminderCont::iterator rem = myMoveReminders.begin(); rem != myMoveReminders.end();) {
        // skip the reminder if it is a lane reminder but not for my lane (indicated by rem->second > 0.)
        if (rem->first->getLane() != 0 && rem->second > 0.) {
#ifdef _DEBUG
            if (myTraceMoveReminders) {
                traceMoveReminder("notifyEnter_skipped", rem->first, rem->second, true);
            }
#endif
            ++rem;
        } else {
            if (rem->first->notifyEnter(*this, reason, enteredLane)) {
#ifdef _DEBUG
                if (myTraceMoveReminders) {
                    traceMoveReminder("notifyEnter", rem->first, rem->second, true);
                }
#endif
                ++rem;
            } else {
#ifdef _DEBUG
                if (myTraceMoveReminders) {
                    traceMoveReminder("notifyEnter", rem->first, rem->second, false);
                }
//                std::cout << SIMTIME << " Vehicle '" << getID() << "' erases MoveReminder (with offset " << rem->second << ")" << std::endl;
#endif
                rem = myMoveReminders.erase(rem);
            }
        }
    }
}


bool
MSVehicle::enterLaneAtMove(MSLane* enteredLane, bool onTeleporting) {
    myAmOnNet = !onTeleporting;
    // vaporizing edge?
    /*
    if (enteredLane->getEdge().isVaporizing()) {
        // yep, let's do the vaporization...
        myLane = enteredLane;
        return true;
    }
    */
    // Adjust MoveReminder offset to the next lane
    adaptLaneEntering2MoveReminder(*enteredLane);
    // set the entered lane as the current lane
    myLane = enteredLane;
    myLastBestLanesEdge = 0;

    // internal edges are not a part of the route...
    if (!enteredLane->getEdge().isInternal()) {
        ++myCurrEdge;
    }
    if (!onTeleporting) {
        activateReminders(MSMoveReminder::NOTIFICATION_JUNCTION, enteredLane);
    } else {
        // normal move() isn't called so reset position here. must be done
        // before calling reminders
        myState.myPos = 0;
        myCachedPosition = Position::INVALID;
        activateReminders(MSMoveReminder::NOTIFICATION_TELEPORT, enteredLane);
    }
    // update via
    if (myParameter->via.size() > 0 &&  myLane->getEdge().getID() == myParameter->via.front()) {
        myParameter->via.erase(myParameter->via.begin());
    }
    return hasArrived();
}


void
MSVehicle::enterLaneAtLaneChange(MSLane* enteredLane) {
    myAmOnNet = true;
    myLane = enteredLane;
    myCachedPosition = Position::INVALID;
    // need to update myCurrentLaneInBestLanes
    updateBestLanes();
    // switch to and activate the new lane's reminders
    // keep OldLaneReminders
    for (std::vector< MSMoveReminder* >::const_iterator rem = enteredLane->getMoveReminders().begin(); rem != enteredLane->getMoveReminders().end(); ++rem) {
        addReminder(*rem);
    }
    activateReminders(MSMoveReminder::NOTIFICATION_LANE_CHANGE, enteredLane);
    MSLane* lane = myLane;
    double leftLength = getVehicleType().getLength() - myState.myPos;
    for (int i = 0; i < (int)myFurtherLanes.size(); i++) {
        if (lane != 0) {
            lane = lane->getLogicalPredecessorLane(myFurtherLanes[i]->getEdge());
        }
        if (lane != 0) {
#ifdef DEBUG_FURTHER
            if (DEBUG_COND) {
                std::cout << SIMTIME << " enterLaneAtLaneChange \n";
            }
#endif
            myFurtherLanes[i]->resetPartialOccupation(this);
            myFurtherLanes[i] = lane;
            myFurtherLanesPosLat[i] = myState.myPosLat;
#ifdef DEBUG_FURTHER
            if (DEBUG_COND) {
                std::cout << SIMTIME << " enterLaneAtLaneChange \n";
            }
#endif
            leftLength -= (lane)->setPartialOccupation(this);
        } else {
            // keep the old values, but ensure there is no shadow
            if (myLaneChangeModel->isChangingLanes()) {
                myLaneChangeModel->setNoShadowPartialOccupator(myFurtherLanes[i]);
            }
        }
    }
#ifdef DEBUG_FURTHER
    if (DEBUG_COND) {
        std::cout << SIMTIME << " enterLaneAtLaneChange new furtherLanes=" << toString(myFurtherLanes) << "\n";
    }
#endif
    myAngle = computeAngle();
}


void
MSVehicle::enterLaneAtInsertion(MSLane* enteredLane, double pos, double speed, double posLat, MSMoveReminder::Notification notification) {
    myState = State(pos, speed, posLat, pos - getVehicleType().getLength());
    if (myDeparture == NOT_YET_DEPARTED) {
        onDepart();
    }
    myCachedPosition = Position::INVALID;
    assert(myState.myPos >= 0);
    assert(myState.mySpeed >= 0);
    myLane = enteredLane;
    myAmOnNet = true;
    // schedule action for the next timestep
    myLastActionTime = MSNet::getInstance()->getCurrentTimeStep() + DELTA_T;
    if (notification != MSMoveReminder::NOTIFICATION_TELEPORT) {
        // set and activate the new lane's reminders, teleports already did that at enterLaneAtMove
        for (std::vector< MSMoveReminder* >::const_iterator rem = enteredLane->getMoveReminders().begin(); rem != enteredLane->getMoveReminders().end(); ++rem) {
            addReminder(*rem);
        }
        activateReminders(notification, enteredLane);
    }
    // build the list of lanes the vehicle is lapping into
    if (!myLaneChangeModel->isOpposite()) {
        double leftLength = myType->getLength() - pos;
        MSLane* clane = enteredLane;
        while (leftLength > 0) {
            clane = clane->getLogicalPredecessorLane();
            if (clane == 0 || clane == myLane) {
                break;
            }
            myFurtherLanes.push_back(clane);
            myFurtherLanesPosLat.push_back(myState.myPosLat);
            leftLength -= (clane)->setPartialOccupation(this);
        }
        myState.myBackPos = -leftLength;
    } else {
        // clear partial occupation
        for (std::vector<MSLane*>::iterator i = myFurtherLanes.begin(); i != myFurtherLanes.end(); ++i) {
#ifdef DEBUG_FURTHER
            if (DEBUG_COND) {
                std::cout << SIMTIME << " enterLaneAtInsertion \n";
            }
#endif
            (*i)->resetPartialOccupation(this);
        }
        myFurtherLanes.clear();
        myFurtherLanesPosLat.clear();
    }
    if (MSGlobals::gLateralResolution > 0 || MSGlobals::gLaneChangeDuration > 0) {
        getLaneChangeModel().updateShadowLane();
        getLaneChangeModel().updateTargetLane();
    }
    myAngle = computeAngle();
    if (getLaneChangeModel().isOpposite()) {
        myAngle += M_PI;
    }
}


void
MSVehicle::leaveLane(const MSMoveReminder::Notification reason, const MSLane* approachedLane) {
    for (MoveReminderCont::iterator rem = myMoveReminders.begin(); rem != myMoveReminders.end();) {
        if (rem->first->notifyLeave(*this, myState.myPos + rem->second, reason, approachedLane)) {
#ifdef _DEBUG
            if (myTraceMoveReminders) {
                traceMoveReminder("notifyLeave", rem->first, rem->second, true);
            }
#endif
            ++rem;
        } else {
#ifdef _DEBUG
            if (myTraceMoveReminders) {
                traceMoveReminder("notifyLeave", rem->first, rem->second, false);
            }
#endif
            rem = myMoveReminders.erase(rem);
        }
    }
    if (reason != MSMoveReminder::NOTIFICATION_JUNCTION && reason != MSMoveReminder::NOTIFICATION_LANE_CHANGE) {
        // @note. In case of lane change, myFurtherLanes and partial occupation
        // are handled in enterLaneAtLaneChange()
        for (std::vector<MSLane*>::iterator i = myFurtherLanes.begin(); i != myFurtherLanes.end(); ++i) {
#ifdef DEBUG_FURTHER
            if (DEBUG_COND) {
                std::cout << SIMTIME << " leaveLane \n";
            }
#endif
            (*i)->resetPartialOccupation(this);
        }
        myFurtherLanes.clear();
        myFurtherLanesPosLat.clear();
    }
    if (reason >= MSMoveReminder::NOTIFICATION_TELEPORT) {
        myAmOnNet = false;
        myWaitingTime = 0;
    }
    if (reason != MSMoveReminder::NOTIFICATION_PARKING && resumeFromStopping()) {
        WRITE_WARNING("Vehicle '" + getID() + "' aborts stop.");
    }
    if (reason != MSMoveReminder::NOTIFICATION_PARKING && reason != MSMoveReminder::NOTIFICATION_LANE_CHANGE) {
        while (!myStops.empty() && myStops.front().edge == myCurrEdge && &myStops.front().lane->getEdge() == &myLane->getEdge()) {
            WRITE_WARNING("Vehicle '" + getID() + "' skips stop on lane '" + myStops.front().lane->getID()
                          + "' time=" + time2string(MSNet::getInstance()->getCurrentTimeStep()) + ".")
            myStops.pop_front();
        }
    }
}


MSAbstractLaneChangeModel&
MSVehicle::getLaneChangeModel() {
    return *myLaneChangeModel;
}


const MSAbstractLaneChangeModel&
MSVehicle::getLaneChangeModel() const {
    return *myLaneChangeModel;
}


const std::vector<MSVehicle::LaneQ>&
MSVehicle::getBestLanes() const {
    return *myBestLanes.begin();
}


void
MSVehicle::updateBestLanes(bool forceRebuild, const MSLane* startLane) {
#ifdef DEBUG_BESTLANES
    if (DEBUG_COND) {
        std::cout << SIMTIME << " updateBestLanes veh=" << getID() << " startLane1=" << Named::getIDSecure(startLane) << " myLane=" << Named::getIDSecure(myLane) << "\n";
    }
#endif
    if (startLane == 0) {
        startLane = myLane;
    }
    assert(startLane != 0);
    if (getLaneChangeModel().isOpposite()) {
        // depending on the calling context, startLane might be the forward lane
        // or the reverse-direction lane. In the latter case we need to
        // transform it to the forward lane.
        bool startLaneIsOpposite = (startLane->isInternal()
                                    ? & (startLane->getLinkCont()[0]->getLane()->getEdge()) != *(myCurrEdge + 1)
                                    : &startLane->getEdge() != *myCurrEdge);
        if (startLaneIsOpposite) {
            startLane = startLane->getOpposite();
            assert(startLane != 0);
        }
    }
    if (myBestLanes.size() > 0 && !forceRebuild && myLastBestLanesEdge == &startLane->getEdge()) {
        updateOccupancyAndCurrentBestLane(startLane);
        return;
    }
    if (startLane->getEdge().isInternal()) {
        if (myBestLanes.size() == 0 || forceRebuild) {
            // rebuilt from previous non-internal lane (may backtrack twice if behind an internal junction)
            updateBestLanes(true, startLane->getLogicalPredecessorLane());
        }
        if (myLastBestLanesInternalLane == startLane && !forceRebuild) {
            return;
        }
        // adapt best lanes to fit the current internal edge:
        // keep the entries that are reachable from this edge
        const MSEdge* nextEdge = startLane->getNextNormal();
        assert(!nextEdge->isInternal());
        for (std::vector<std::vector<LaneQ> >::iterator it = myBestLanes.begin(); it != myBestLanes.end();) {
            std::vector<LaneQ>& lanes = *it;
            assert(lanes.size() > 0);
            if (&(lanes[0].lane->getEdge()) == nextEdge) {
                // keep those lanes which are successors of internal lanes from the edge of startLane
                std::vector<LaneQ> oldLanes = lanes;
                lanes.clear();
                const std::vector<MSLane*>& sourceLanes = startLane->getEdge().getLanes();
                for (std::vector<MSLane*>::const_iterator it_source = sourceLanes.begin(); it_source != sourceLanes.end(); ++it_source) {
                    for (std::vector<LaneQ>::iterator it_lane = oldLanes.begin(); it_lane != oldLanes.end(); ++it_lane) {
                        if ((*it_source)->getLinkCont()[0]->getLane() == (*it_lane).lane) {
                            lanes.push_back(*it_lane);
                            break;
                        }
                    }
                }
                assert(lanes.size() == startLane->getEdge().getLanes().size());
                // patch invalid bestLaneOffset and updated myCurrentLaneInBestLanes
                for (int i = 0; i < (int)lanes.size(); ++i) {
                    if (i + lanes[i].bestLaneOffset < 0) {
                        lanes[i].bestLaneOffset = -i;
                    }
                    if (i + lanes[i].bestLaneOffset >= (int)lanes.size()) {
                        lanes[i].bestLaneOffset = (int)lanes.size() - i - 1;
                    }
                    assert(i + lanes[i].bestLaneOffset >= 0);
                    assert(i + lanes[i].bestLaneOffset < (int)lanes.size());
                    if (lanes[i].bestContinuations[0] != 0) {
                        // patch length of bestContinuation to match expectations (only once)
                        lanes[i].bestContinuations.insert(lanes[i].bestContinuations.begin(), (MSLane*)0);
                    }
                    if (startLane->getLinkCont()[0]->getLane() == lanes[i].lane) {
                        myCurrentLaneInBestLanes = lanes.begin() + i;
                    }
                    assert(&(lanes[i].lane->getEdge()) == nextEdge);
                }
                myLastBestLanesInternalLane = startLane;
                updateOccupancyAndCurrentBestLane(startLane);
                return;
            } else {
                // remove passed edges
                it = myBestLanes.erase(it);
            }
        }
        assert(false); // should always find the next edge
    }
    // start rebuilding
    myLastBestLanesEdge = &startLane->getEdge();
    myBestLanes.clear();

    // get information about the next stop
    MSRouteIterator nextStopEdge = myRoute->end();
    const MSLane* nextStopLane = 0;
    double nextStopPos = 0;
    if (!myStops.empty()) {
        const Stop& nextStop = myStops.front();
        nextStopLane = nextStop.lane;
        nextStopEdge = nextStop.edge;
        nextStopPos = nextStop.pars.startPos;
    }
    if (myParameter->arrivalLaneProcedure == ARRIVAL_LANE_GIVEN && nextStopEdge == myRoute->end()) {
        nextStopEdge = (myRoute->end() - 1);
        nextStopLane = (*nextStopEdge)->getLanes()[myArrivalLane];
        nextStopPos = myArrivalPos;
    }
    if (nextStopEdge != myRoute->end()) {
        // make sure that the "wrong" lanes get a penalty. (penalty needs to be
        // large enough to overcome a magic threshold in MSLCM_DK2004.cpp:383)
        nextStopPos = MAX2(POSITION_EPS, MIN2((double)nextStopPos, (double)(nextStopLane->getLength() - 2 * POSITION_EPS)));
        if (nextStopLane->isInternal()) {
            nextStopPos += (*nextStopEdge)->getLength();
        }
    }

    // go forward along the next lanes;
    int seen = 0;
    double seenLength = 0;
    bool progress = true;
    for (MSRouteIterator ce = myCurrEdge; progress;) {
        std::vector<LaneQ> currentLanes;
        const std::vector<MSLane*>* allowed = 0;
        const MSEdge* nextEdge = 0;
        if (ce != myRoute->end() && ce + 1 != myRoute->end()) {
            nextEdge = *(ce + 1);
            allowed = (*ce)->allowedLanes(*nextEdge, myType->getVehicleClass());
        }
        const std::vector<MSLane*>& lanes = (*ce)->getLanes();
        for (std::vector<MSLane*>::const_iterator i = lanes.begin(); i != lanes.end(); ++i) {
            LaneQ q;
            MSLane* cl = *i;
            q.lane = cl;
            q.bestContinuations.push_back(cl);
            q.bestLaneOffset = 0;
            q.length = cl->allowsVehicleClass(myType->getVehicleClass()) ? cl->getLength() : 0;
            q.currentLength = q.length;
            q.allowsContinuation = allowed == 0 || find(allowed->begin(), allowed->end(), cl) != allowed->end();
            q.occupation = 0;
            q.nextOccupation = 0;
            currentLanes.push_back(q);
        }
        //
        if (nextStopEdge == ce && !nextStopLane->isInternal()) {
            progress = false;
            for (std::vector<LaneQ>::iterator q = currentLanes.begin(); q != currentLanes.end(); ++q) {
                if (nextStopLane != 0 && nextStopLane != (*q).lane) {
                    (*q).allowsContinuation = false;
                    (*q).length = nextStopPos;
                    (*q).currentLength = (*q).length;
                }
            }
        }

        myBestLanes.push_back(currentLanes);
        ++seen;
        seenLength += currentLanes[0].lane->getLength();
        ++ce;
        progress &= (seen <= 4 || seenLength < 3000);
        progress &= seen <= 8;
        progress &= ce != myRoute->end();
        /*
        if(progress) {
          progress &= (currentLanes.size()!=1||(*ce)->getLanes().size()!=1);
        }
        */
    }

    // we are examining the last lane explicitly
    if (myBestLanes.size() != 0) {
        double bestLength = -1;
        int bestThisIndex = 0;
        int index = 0;
        std::vector<LaneQ>& last = myBestLanes.back();
        for (std::vector<LaneQ>::iterator j = last.begin(); j != last.end(); ++j, ++index) {
            if ((*j).length > bestLength) {
                bestLength = (*j).length;
                bestThisIndex = index;
            }
        }
        index = 0;
        for (std::vector<LaneQ>::iterator j = last.begin(); j != last.end(); ++j, ++index) {
            if ((*j).length < bestLength) {
                (*j).bestLaneOffset = bestThisIndex - index;
            }
        }
    }
#ifdef DEBUG_BESTLANES
    if (DEBUG_COND) {
        std::cout << "   last edge:\n";
        std::vector<LaneQ>& laneQs = myBestLanes.back();
        for (std::vector<LaneQ>::iterator j = laneQs.begin(); j != laneQs.end(); ++j) {
            std::cout << "     lane=" << (*j).lane->getID() << " length=" << (*j).length << " bestOffset=" << (*j).bestLaneOffset << "\n";
        }
    }
#endif
    // go backward through the lanes
    // track back best lane and compute the best prior lane(s)
    for (std::vector<std::vector<LaneQ> >::reverse_iterator i = myBestLanes.rbegin() + 1; i != myBestLanes.rend(); ++i) {
        std::vector<LaneQ>& nextLanes = (*(i - 1));
        std::vector<LaneQ>& clanes = (*i);
        MSEdge& cE = clanes[0].lane->getEdge();
        int index = 0;
        double bestConnectedLength = -1;
        double bestLength = -1;
        for (std::vector<LaneQ>::iterator j = nextLanes.begin(); j != nextLanes.end(); ++j, ++index) {
            if ((*j).lane->isApproachedFrom(&cE) && bestConnectedLength < (*j).length) {
                bestConnectedLength = (*j).length;
            }
            if (bestLength < (*j).length) {
                bestLength = (*j).length;
            }
        }
        // compute index of the best lane (highest length and least offset from the best next lane)
        int bestThisIndex = 0;
        if (bestConnectedLength > 0) {
            index = 0;
            for (std::vector<LaneQ>::iterator j = clanes.begin(); j != clanes.end(); ++j, ++index) {
                LaneQ bestConnectedNext;
                bestConnectedNext.length = -1;
                if ((*j).allowsContinuation) {
                    for (std::vector<LaneQ>::const_iterator m = nextLanes.begin(); m != nextLanes.end(); ++m) {
                        if ((*m).lane->isApproachedFrom(&cE, (*j).lane)) {
                            if (bestConnectedNext.length < (*m).length || (bestConnectedNext.length == (*m).length && abs(bestConnectedNext.bestLaneOffset) > abs((*m).bestLaneOffset))) {
                                bestConnectedNext = *m;
                            }
                        }
                    }
                    if (bestConnectedNext.length == bestConnectedLength && abs(bestConnectedNext.bestLaneOffset) < 2) {
                        (*j).length += bestLength;
                    } else {
                        (*j).length += bestConnectedNext.length;
                    }
                    (*j).bestLaneOffset = bestConnectedNext.bestLaneOffset;
                }
                copy(bestConnectedNext.bestContinuations.begin(), bestConnectedNext.bestContinuations.end(), back_inserter((*j).bestContinuations));
                if (clanes[bestThisIndex].length < (*j).length
                        || (clanes[bestThisIndex].length == (*j).length && abs(clanes[bestThisIndex].bestLaneOffset) > abs((*j).bestLaneOffset))
                        || (clanes[bestThisIndex].length == (*j).length && abs(clanes[bestThisIndex].bestLaneOffset) == abs((*j).bestLaneOffset) &&
                            nextLinkPriority(clanes[bestThisIndex].bestContinuations) < nextLinkPriority((*j).bestContinuations))
                   ) {
                    bestThisIndex = index;
                }
            }
#ifdef DEBUG_BESTLANES
            if (DEBUG_COND) {
                std::cout << "   edge=" << cE.getID() << "\n";
                std::vector<LaneQ>& laneQs = clanes;
                for (std::vector<LaneQ>::iterator j = laneQs.begin(); j != laneQs.end(); ++j) {
                    std::cout << "     lane=" << (*j).lane->getID() << " length=" << (*j).length << " bestOffset=" << (*j).bestLaneOffset << "\n";
                }
            }
#endif

        } else {
            // only needed in case of disconnected routes
            int bestNextIndex = 0;
            int bestDistToNeeded = (int) clanes.size();
            index = 0;
            for (std::vector<LaneQ>::iterator j = clanes.begin(); j != clanes.end(); ++j, ++index) {
                if ((*j).allowsContinuation) {
                    int nextIndex = 0;
                    for (std::vector<LaneQ>::const_iterator m = nextLanes.begin(); m != nextLanes.end(); ++m, ++nextIndex) {
                        if ((*m).lane->isApproachedFrom(&cE, (*j).lane)) {
                            if (bestDistToNeeded > abs((*m).bestLaneOffset)) {
                                bestDistToNeeded = abs((*m).bestLaneOffset);
                                bestThisIndex = index;
                                bestNextIndex = nextIndex;
                            }
                        }
                    }
                }
            }
            clanes[bestThisIndex].length += nextLanes[bestNextIndex].length;
            copy(nextLanes[bestNextIndex].bestContinuations.begin(), nextLanes[bestNextIndex].bestContinuations.end(), back_inserter(clanes[bestThisIndex].bestContinuations));

        }
        // set bestLaneOffset for all lanes
        index = 0;
        for (std::vector<LaneQ>::iterator j = clanes.begin(); j != clanes.end(); ++j, ++index) {
            if ((*j).length < clanes[bestThisIndex].length
                    || ((*j).length == clanes[bestThisIndex].length && abs((*j).bestLaneOffset) > abs(clanes[bestThisIndex].bestLaneOffset))
                    || (nextLinkPriority((*j).bestContinuations)) < nextLinkPriority(clanes[bestThisIndex].bestContinuations)
               ) {
                (*j).bestLaneOffset = bestThisIndex - index;
                if ((nextLinkPriority((*j).bestContinuations)) < nextLinkPriority(clanes[bestThisIndex].bestContinuations)) {
                    // try to move away from the lower-priority lane before it ends
                    (*j).length = (*j).currentLength;
                }
            } else {
                (*j).bestLaneOffset = 0;
            }
        }
    }
    updateOccupancyAndCurrentBestLane(startLane);
#ifdef DEBUG_BESTLANES
    if (DEBUG_COND) {
        std::cout << SIMTIME << " veh=" << getID() << " bestCont=" << toString(getBestLanesContinuation()) << "\n";
    }
#endif
    return;
}


int
MSVehicle::nextLinkPriority(const std::vector<MSLane*>& conts) {
    if (conts.size() < 2) {
        return -1;
    } else {
        MSLink* link = MSLinkContHelper::getConnectingLink(*conts[0], *conts[1]);
        if (link != 0) {
            return link->havePriority() ? 1 : 0;
        } else {
            // disconnected route
            return -1;
        }
    }
}


void
MSVehicle::updateOccupancyAndCurrentBestLane(const MSLane* startLane) {
    std::vector<LaneQ>& currLanes = *myBestLanes.begin();
    std::vector<LaneQ>::iterator i;
    for (i = currLanes.begin(); i != currLanes.end(); ++i) {
        double nextOccupation = 0;
        for (std::vector<MSLane*>::const_iterator j = (*i).bestContinuations.begin() + 1; j != (*i).bestContinuations.end(); ++j) {
            nextOccupation += (*j)->getBruttoVehLenSum();
        }
        (*i).nextOccupation = nextOccupation;
        if ((*i).lane == startLane) {
            myCurrentLaneInBestLanes = i;
        }
    }
}


const std::vector<MSLane*>&
MSVehicle::getBestLanesContinuation() const {
    if (myBestLanes.empty() || myBestLanes[0].empty()) {
        return myEmptyLaneVector;
    }
    return (*myCurrentLaneInBestLanes).bestContinuations;
}


const std::vector<MSLane*>&
MSVehicle::getBestLanesContinuation(const MSLane* const l) const {
    const MSLane* lane = l;
    // XXX: shouldn't this be a "while" to cover more than one internal lane? (Leo) Refs. #2575
    if (lane->getEdge().isInternal()) {
        // internal edges are not kept inside the bestLanes structure
        lane = lane->getLinkCont()[0]->getLane();
    }
    if (myBestLanes.size() == 0) {
        return myEmptyLaneVector;
    }
    for (std::vector<LaneQ>::const_iterator i = myBestLanes[0].begin(); i != myBestLanes[0].end(); ++i) {
        if ((*i).lane == lane) {
            return (*i).bestContinuations;
        }
    }
    return myEmptyLaneVector;
}


int
MSVehicle::getBestLaneOffset() const {
    if (myBestLanes.empty() || myBestLanes[0].empty()) {
        return 0;
    } else {
        return (*myCurrentLaneInBestLanes).bestLaneOffset;
    }
}


void
MSVehicle::adaptBestLanesOccupation(int laneIndex, double density) {
    std::vector<MSVehicle::LaneQ>& preb = myBestLanes.front();
    assert(laneIndex < (int)preb.size());
    preb[laneIndex].occupation = density + preb[laneIndex].nextOccupation;
}


void
MSVehicle::fixPosition() {
    if (MSGlobals::gLaneChangeDuration > 0 && !getLaneChangeModel().isChangingLanes()) {
        myState.myPosLat = 0;
    }
}


double
MSVehicle::getDistanceToPosition(double destPos, const MSEdge* destEdge) const {
    double distance = std::numeric_limits<double>::max();
    if (isOnRoad() && destEdge != NULL) {
        if (&myLane->getEdge() == *myCurrEdge) {
            // vehicle is on a normal edge
            distance = myRoute->getDistanceBetween(getPositionOnLane(), destPos, *myCurrEdge, destEdge);
        } else {
            // vehicle is on inner junction edge
            distance = myLane->getLength() - getPositionOnLane();
            distance += myRoute->getDistanceBetween(0, destPos, *(myCurrEdge + 1), destEdge);
        }
    }
    return distance;
}


std::pair<const MSVehicle* const, double>
MSVehicle::getLeader(double dist) const {
    if (myLane == 0) {
        return std::make_pair(static_cast<const MSVehicle*>(0), -1);
    }
    if (dist == 0) {
        dist = getCarFollowModel().brakeGap(getSpeed()) + getVehicleType().getMinGap();
    }
    const MSVehicle* lead = 0;
    const MSLane::VehCont& vehs = myLane->getVehiclesSecure();
    // vehicle might be outside the road network
    MSLane::VehCont::const_iterator it = std::find(vehs.begin(), vehs.end(), this);
    if (it != vehs.end() && it + 1 != vehs.end()) {
        lead = *(it + 1);
    }
    if (lead != 0) {
        std::pair<const MSVehicle* const, double> result(
            lead, lead->getBackPositionOnLane(myLane) - getPositionOnLane() - getVehicleType().getMinGap());
        myLane->releaseVehicles();
        return result;
    }
    const double seen = myLane->getLength() - getPositionOnLane();
    const std::vector<MSLane*>& bestLaneConts = getBestLanesContinuation(myLane);
    std::pair<const MSVehicle* const, double> result = myLane->getLeaderOnConsecutive(dist, seen, getSpeed(), *this, bestLaneConts);
    myLane->releaseVehicles();
    return result;
}


double
MSVehicle::getTimeGapOnLane() const {
    // calling getLeader with 0 would induce a dist calculation but we only want to look for the leaders on the current lane
    std::pair<const MSVehicle* const, double> leaderInfo = getLeader(-1);
    if (leaderInfo.first == 0 || getSpeed() == 0) {
        return -1;
    }
    return (leaderInfo.second + getVehicleType().getMinGap()) / getSpeed();
}


double
MSVehicle::getCO2Emissions() const {
    return PollutantsInterface::compute(myType->getEmissionClass(), PollutantsInterface::CO2, myState.speed(), myAcceleration, getSlope());
}


double
MSVehicle::getCOEmissions() const {
    return PollutantsInterface::compute(myType->getEmissionClass(), PollutantsInterface::CO, myState.speed(), myAcceleration, getSlope());
}


double
MSVehicle::getHCEmissions() const {
    return PollutantsInterface::compute(myType->getEmissionClass(), PollutantsInterface::HC, myState.speed(), myAcceleration, getSlope());
}


double
MSVehicle::getNOxEmissions() const {
    return PollutantsInterface::compute(myType->getEmissionClass(), PollutantsInterface::NO_X, myState.speed(), myAcceleration, getSlope());
}


double
MSVehicle::getPMxEmissions() const {
    return PollutantsInterface::compute(myType->getEmissionClass(), PollutantsInterface::PM_X, myState.speed(), myAcceleration, getSlope());
}


double
MSVehicle::getFuelConsumption() const {
    return PollutantsInterface::compute(myType->getEmissionClass(), PollutantsInterface::FUEL, myState.speed(), myAcceleration, getSlope());
}


double
MSVehicle::getElectricityConsumption() const {
    return PollutantsInterface::compute(myType->getEmissionClass(), PollutantsInterface::ELEC, myState.speed(), myAcceleration, getSlope());
}


double
MSVehicle::getHarmonoise_NoiseEmissions() const {
    return HelpersHarmonoise::computeNoise(myType->getEmissionClass(), myState.speed(), myAcceleration);
}


void
MSVehicle::addPerson(MSTransportable* person) {
    if (myPersonDevice == 0) {
        myPersonDevice = MSDevice_Transportable::buildVehicleDevices(*this, myDevices, false);
        myMoveReminders.push_back(std::make_pair(myPersonDevice, 0.));
    }
    myPersonDevice->addTransportable(person);
    if (myStops.size() > 0 && myStops.front().reached && myStops.front().triggered && myStops.front().numExpectedPerson > 0) {
        myStops.front().numExpectedPerson -= (int)myStops.front().pars.awaitedPersons.count(person->getID());
        if (myStops.front().numExpectedPerson == 0) {
            myStops.front().duration = 0;
        }
    }
}

void
MSVehicle::addContainer(MSTransportable* container) {
    if (myContainerDevice == 0) {
        myContainerDevice = MSDevice_Transportable::buildVehicleDevices(*this, myDevices, true);
        myMoveReminders.push_back(std::make_pair(myContainerDevice, 0.));
    }
    myContainerDevice->addTransportable(container);
    if (myStops.size() > 0 && myStops.front().reached && myStops.front().pars.containerTriggered && myStops.front().numExpectedContainer > 0) {
        myStops.front().numExpectedContainer -= (int)myStops.front().pars.awaitedContainers.count(container->getID());
        if (myStops.front().numExpectedContainer == 0) {
            myStops.front().duration = 0;
        }
    }
}


void
MSVehicle::removeTransportable(MSTransportable* t) {
    const bool isPerson = dynamic_cast<MSPerson*>(t) != 0;
    MSDevice_Transportable* device = isPerson ? myPersonDevice : myContainerDevice;
    if (device != 0) {
        device->removeTransportable(t);
    }
}


const std::vector<MSTransportable*>&
MSVehicle::getPersons() const {
    if (myPersonDevice == 0) {
        return myEmptyTransportableVector;
    } else {
        return myPersonDevice->getTransportables();
    }
}


const std::vector<MSTransportable*>&
MSVehicle::getContainers() const {
    if (myContainerDevice == 0) {
        return myEmptyTransportableVector;
    } else {
        return myContainerDevice->getTransportables();
    }
}


int
MSVehicle::getPersonNumber() const {
    int boarded = myPersonDevice == 0 ? 0 : myPersonDevice->size();
    return boarded + myParameter->personNumber;
}

int
MSVehicle::getContainerNumber() const {
    int loaded = myContainerDevice == 0 ? 0 : myContainerDevice->size();
    return loaded + myParameter->containerNumber;
}


void
MSVehicle::setBlinkerInformation() {
    switchOffSignal(VEH_SIGNAL_BLINKER_RIGHT | VEH_SIGNAL_BLINKER_LEFT);
    int state = getLaneChangeModel().getOwnState();
    // do not set blinker for sublane changes or when blocked from changing to the right
    const bool blinkerManoeuvre = (((state & LCA_SUBLANE) == 0) && (
                                       (state & LCA_KEEPRIGHT) == 0 || (state & LCA_BLOCKED) == 0));
    if ((state & LCA_LEFT) != 0 && blinkerManoeuvre) {
        switchOnSignal(VEH_SIGNAL_BLINKER_LEFT);
    } else if ((state & LCA_RIGHT) != 0 && blinkerManoeuvre) {
        switchOnSignal(VEH_SIGNAL_BLINKER_RIGHT);
    } else if (getLaneChangeModel().isChangingLanes()) {
        if (getLaneChangeModel().getLaneChangeDirection() == 1) {
            switchOnSignal(VEH_SIGNAL_BLINKER_LEFT);
        } else {
            switchOnSignal(VEH_SIGNAL_BLINKER_RIGHT);
        }
    } else {
        const MSLane* lane = getLane();
        MSLinkCont::const_iterator link = MSLane::succLinkSec(*this, 1, *lane, getBestLanesContinuation());
        if (link != lane->getLinkCont().end() && lane->getLength() - getPositionOnLane() < lane->getVehicleMaxSpeed(this) * (double) 7.) {
            switch ((*link)->getDirection()) {
                case LINKDIR_TURN:
                case LINKDIR_LEFT:
                case LINKDIR_PARTLEFT:
                    switchOnSignal(VEH_SIGNAL_BLINKER_LEFT);
                    break;
                case LINKDIR_RIGHT:
                case LINKDIR_PARTRIGHT:
                    switchOnSignal(VEH_SIGNAL_BLINKER_RIGHT);
                    break;
                default:
                    break;
            }
        }
    }
    if (myInfluencer != 0 && myInfluencer->getSignals() >= 0) {
        mySignals = myInfluencer->getSignals();
        myInfluencer->setSignals(-1); // overwrite computed signals only once
    }
}

void
MSVehicle::setEmergencyBlueLight(SUMOTime currentTime) {
    if (currentTime % 1000 == 0) {
        if (signalSet(VEH_SIGNAL_EMERGENCY_BLUE)) {
            switchOffSignal(VEH_SIGNAL_EMERGENCY_BLUE);
        } else {
            switchOnSignal(VEH_SIGNAL_EMERGENCY_BLUE);
        }
    }
}


int
MSVehicle::getLaneIndex() const {
    std::vector<MSLane*>::const_iterator laneP = std::find(myLane->getEdge().getLanes().begin(), myLane->getEdge().getLanes().end(), myLane);
    return (int) std::distance(myLane->getEdge().getLanes().begin(), laneP);
}


void
MSVehicle::setTentativeLaneAndPosition(MSLane* lane, double pos, double posLat) {
    assert(lane != 0);
    myLane = lane;
    myState.myPos = pos;
    myState.myPosLat = posLat;
    myState.myBackPos = pos - getVehicleType().getLength();
}


double
MSVehicle::getRightSideOnLane() const {
    return myState.myPosLat + 0.5 * myLane->getWidth() - 0.5 * getVehicleType().getWidth();
}


double
MSVehicle::getRightSideOnEdge(const MSLane* lane) const {
    return getCenterOnEdge(lane) - 0.5 * getVehicleType().getWidth();
}


double
MSVehicle::getCenterOnEdge(const MSLane* lane) const {
    if (lane == 0 || &lane->getEdge() == &myLane->getEdge()) {
        return myLane->getRightSideOnEdge() + myState.myPosLat + 0.5 * myLane->getWidth();
    } else {
        assert(myFurtherLanes.size() == myFurtherLanesPosLat.size());
        for (int i = 0; i < (int)myFurtherLanes.size(); ++i) {
            if (myFurtherLanes[i] == lane) {
#ifdef DEBUG_FURTHER
                if (DEBUG_COND) std::cout << "    getCenterOnEdge veh=" << getID() << " lane=" << lane->getID() << " i=" << i << " furtherLat=" << myFurtherLanesPosLat[i]
                                              << " result=" << lane->getRightSideOnEdge() + myFurtherLanesPosLat[i] + 0.5 * lane->getWidth()
                                              << "\n";
#endif
                return lane->getRightSideOnEdge() + myFurtherLanesPosLat[i] + 0.5 * lane->getWidth();
            }
        }
        //if (DEBUG_COND) std::cout << SIMTIME << " veh=" << getID() << " myShadowFurtherLanes=" << toString(getLaneChangeModel().getShadowFurtherLanes()) << "\n";
        const std::vector<MSLane*>& shadowFurther = getLaneChangeModel().getShadowFurtherLanes();
        for (int i = 0; i < (int)shadowFurther.size(); ++i) {
            //if (DEBUG_COND) std::cout << " comparing i=" << (*i)->getID() << " lane=" << lane->getID() << "\n";
            if (shadowFurther[i] == lane) {
                assert(getLaneChangeModel().getShadowLane() != 0);
                return (lane->getRightSideOnEdge() + getLaneChangeModel().getShadowFurtherLanesPosLat()[i] + 0.5 * lane->getWidth()
                        + (myLane->getCenterOnEdge() - getLaneChangeModel().getShadowLane()->getCenterOnEdge()));
            }
        }
        assert(false);
        throw ProcessError("Request lateral pos of vehicle '" + getID() + "' for invalid lane '" + Named::getIDSecure(lane) + "'");
    }
}


double
MSVehicle::getLatOffset(const MSLane* lane) const {
    assert(lane != 0);
    if (&lane->getEdge() == &myLane->getEdge()) {
        return myLane->getRightSideOnEdge() - lane->getRightSideOnEdge();
    } else {
        // Check whether the lane is a further lane for the vehicle
        for (int i = 0; i < (int)myFurtherLanes.size(); ++i) {
            if (myFurtherLanes[i] == lane) {
#ifdef DEBUG_FURTHER
                if (DEBUG_COND) {
                    std::cout << "    getLatOffset veh=" << getID() << " lane=" << lane->getID() << " i=" << i << " posLat=" << myState.myPosLat << " furtherLat=" << myFurtherLanesPosLat[i] << "\n";
                }
#endif
                return myFurtherLanesPosLat[i] - myState.myPosLat;
            }
        }
#ifdef DEBUG_FURTHER
        if (DEBUG_COND) {
            std::cout << SIMTIME << " veh=" << getID() << " myShadowFurtherLanes=" << toString(getLaneChangeModel().getShadowFurtherLanes()) << "\n";
        }
#endif
        // Check whether the lane is a "shadow further lane" for the vehicle
        const std::vector<MSLane*>& shadowFurther = getLaneChangeModel().getShadowFurtherLanes();
        for (int i = 0; i < (int)shadowFurther.size(); ++i) {
            if (shadowFurther[i] == lane) {
#ifdef DEBUG_FURTHER
                if (DEBUG_COND) std::cout << "    getLatOffset veh=" << getID()
                                              << " shadowLane=" << Named::getIDSecure(getLaneChangeModel().getShadowLane())
                                              << " lane=" << lane->getID()
                                              << " i=" << i
                                              << " posLat=" << myState.myPosLat
                                              << " shadowPosLat=" << getLatOffset(getLaneChangeModel().getShadowLane())
                                              << " shadowFurtherLat=" << getLaneChangeModel().getShadowFurtherLanesPosLat()[i]
                                              <<  "\n";
#endif
                return getLatOffset(getLaneChangeModel().getShadowLane()) + getLaneChangeModel().getShadowFurtherLanesPosLat()[i] - myState.myPosLat;
            }
        }
        // Check whether the vehicle issued a maneuverReservation on the lane.
        assert(&getLaneChangeModel().getTargetLane()->getEdge() == &myLane->getEdge()); // should have been handled in (&lane->getEdge() == &myLane->getEdge())-block
        const std::vector<MSLane*>& furtherTargets = getLaneChangeModel().getFurtherTargetLanes();
        for (int i = 0; i < (int)myFurtherLanes.size(); ++i) {
            // Further target lanes are just neighboring lanes of the vehicle's further lanes, @see MSAbstractLaneChangeModel::updateTargetLane()
            MSLane* targetLane = furtherTargets[i];
            if (targetLane == lane) {
                const double targetDir = getLaneChangeModel().getManeuverDist() < 0 ? -1. : 1.;
                const double latOffset = myFurtherLanesPosLat[i] - myState.myPosLat + targetDir * 0.5 * (myFurtherLanes[i]->getWidth() + targetLane->getWidth());
#ifdef DEBUG_TARGET_LANE
                if (DEBUG_COND) {
                    std::cout << "    getLatOffset veh=" << getID()
                              << " wrt targetLane=" << Named::getIDSecure(getLaneChangeModel().getTargetLane())
                              << "\n    i=" << i
                              << " posLat=" << myState.myPosLat
                              << " furtherPosLat=" << myFurtherLanesPosLat[i]
                              << " maneuverDist=" << getLaneChangeModel().getManeuverDist()
                              << " targetDir=" << targetDir
                              << " latOffset=" << latOffset
                              <<  std::endl;
                }
#endif
                return latOffset;
            }
        }
        assert(false);
        throw ProcessError("Request lateral offset of vehicle '" + getID() + "' for invalid lane '" + Named::getIDSecure(lane) + "'");
    }
}


double
MSVehicle::lateralDistanceToLane(const int offset) const {
    // compute the distance when changing to the neighboring lane
    // (ensure we do not lap into the line behind neighLane since there might be unseen blockers)
    assert(offset == 0 || offset == 1 || offset == -1);
    assert(myLane != nullptr);
    assert(myLane->getParallelLane(offset) != nullptr);
    const double halfCurrentLaneWidth = 0.5 * myLane->getWidth();
    const double halfVehWidth = 0.5 * (getWidth() + NUMERICAL_EPS);
    const double latPos = getLateralPositionOnLane();
    double leftLimit = halfCurrentLaneWidth - halfVehWidth - latPos;
    double rightLimit = -halfCurrentLaneWidth + halfVehWidth - latPos;
    double latLaneDist = 0;  // minimum distance to move the vehicle fully onto the new lane
    if (offset == 0) {
        if (latPos + halfVehWidth > halfCurrentLaneWidth) {
            // correct overlapping left
            latLaneDist = halfCurrentLaneWidth - latPos - halfVehWidth;
        } else if (latPos - halfVehWidth < - halfCurrentLaneWidth) {
            // correct overlapping left
            latLaneDist = halfCurrentLaneWidth - latPos + halfVehWidth;
        }
    } else if (offset == -1) {
        latLaneDist = rightLimit - (getWidth() + NUMERICAL_EPS);
    } else if (offset == 1) {
        latLaneDist = leftLimit + (getWidth() + NUMERICAL_EPS);
    }
#ifdef DEBUG_ACTIONSTEPS
    if DEBUG_COND {
    std::cout << SIMTIME
    << " veh=" << getID()
        << " halfCurrentLaneWidth=" << halfCurrentLaneWidth
        << " halfVehWidth=" << halfVehWidth
        << " latPos=" << latPos
        << " latLaneDist=" << latLaneDist
        << " leftLimit=" << leftLimit
        << " rightLimit=" << rightLimit
        << "\n";
    }
#endif
    return latLaneDist;
}


double
MSVehicle::getLateralOverlap(double posLat) const {
    return (fabs(posLat) + 0.5 * getVehicleType().getWidth()
            - 0.5 * myLane->getWidth());
}


double
MSVehicle::getLateralOverlap() const {
    return getLateralOverlap(getLateralPositionOnLane());
}

void
MSVehicle::removeApproachingInformation(DriveItemVector& lfLinks) const {
    for (DriveItemVector::iterator i = lfLinks.begin(); i != lfLinks.end(); ++i) {
        if ((*i).myLink != 0) {
            (*i).myLink->removeApproaching(this);
        }
    }
    // unregister on all shadow links
    getLaneChangeModel().removeShadowApproachingInformation();
}


bool
MSVehicle::unsafeLinkAhead(const MSLane* lane) const {
    // the following links are unsafe:
    // - zipper links if they are close enough and have approaching vehicles in the relevant time range
    // - unprioritized links if the vehicle is currently approaching a prioritzed link and unable to stop in time
    double seen = myLane->getLength() - getPositionOnLane();
    const double dist = getCarFollowModel().brakeGap(getSpeed(), getCarFollowModel().getMaxDecel(), 0);
    if (seen < dist) {
        const std::vector<MSLane*>& bestLaneConts = getBestLanesContinuation(lane);
        int view = 1;
        MSLinkCont::const_iterator link = MSLane::succLinkSec(*this, view, *lane, bestLaneConts);
        DriveItemVector::const_iterator di = myLFLinkLanes.begin();
        while (!lane->isLinkEnd(link) && seen <= dist) {
            if (!lane->getEdge().isInternal()
                    && (((*link)->getState() == LINKSTATE_ZIPPER && seen < MSLink::ZIPPER_ADAPT_DIST)
                        || !(*link)->havePriority())) {
                // find the drive item corresponding to this link
                bool found = false;
                while (di != myLFLinkLanes.end() && !found) {
                    if ((*di).myLink != 0) {
                        const MSLane* diPredLane = (*di).myLink->getLaneBefore();
                        if (diPredLane != 0) {
                            if (&diPredLane->getEdge() == &lane->getEdge()) {
                                found = true;
                            }
                        }
                    }
                    if (!found) {
                        di++;
                    }
                }
                if (found) {
                    const SUMOTime leaveTime = (*link)->getLeaveTime((*di).myArrivalTime, (*di).myArrivalSpeed,
                                               (*di).getLeaveSpeed(), getVehicleType().getLength());
                    if ((*link)->hasApproachingFoe((*di).myArrivalTime, leaveTime, (*di).myArrivalSpeed, getCarFollowModel().getMaxDecel())) {
                        //std::cout << SIMTIME << " veh=" << getID() << " aborting changeTo=" << Named::getIDSecure(bestLaneConts.front()) << " linkState=" << toString((*link)->getState()) << " seen=" << seen << " dist=" << dist << "\n";
                        return true;
                    }
                }
                // no drive item is found if the vehicle aborts it's request within dist
            }
            lane = (*link)->getViaLaneOrLane();
            if (!lane->getEdge().isInternal()) {
                view++;
            }
            seen += lane->getLength();
            link = MSLane::succLinkSec(*this, view, *lane, bestLaneConts);
        }
    }
    return false;
}


PositionVector
MSVehicle::getBoundingBox() const {
    PositionVector centerLine;
    centerLine.push_back(getPosition());
    centerLine.push_back(getBackPosition());
    centerLine.move2side(0.5 * myType->getWidth());
    PositionVector result = centerLine;
    centerLine.move2side(-myType->getWidth());
    result.append(centerLine.reverse(), POSITION_EPS);
    return result;
}


PositionVector
MSVehicle::getBoundingPoly() const {
    // XXX implement more types
    switch (myType->getGuiShape()) {
        case SVS_PASSENGER:
        case SVS_PASSENGER_SEDAN:
        case SVS_PASSENGER_HATCHBACK:
        case SVS_PASSENGER_WAGON:
        case SVS_PASSENGER_VAN: {
            PositionVector result;
            PositionVector centerLine;
            centerLine.push_back(getPosition());
            centerLine.push_back(getBackPosition());
            PositionVector line1 = centerLine;
            PositionVector line2 = centerLine;
            line1.move2side(0.3 * myType->getWidth());
            line2.move2side(0.5 * myType->getWidth());
            line2.scaleRelative(0.8);
            result.push_back(line1[0]);
            result.push_back(line2[0]);
            result.push_back(line2[1]);
            result.push_back(line1[1]);
            line1.move2side(-0.6 * myType->getWidth());
            line2.move2side(-1.0 * myType->getWidth());
            result.push_back(line1[1]);
            result.push_back(line2[1]);
            result.push_back(line2[0]);
            result.push_back(line1[0]);
            return result;
        }
        default:
            return getBoundingBox();
    }
}


bool
MSVehicle::onFurtherEdge(const MSEdge* edge) const {
    for (std::vector<MSLane*>::const_iterator i = myFurtherLanes.begin(); i != myFurtherLanes.end(); ++i) {
        if (&(*i)->getEdge() == edge) {
            return true;
        }
    }
    return false;
}


bool
MSVehicle::addTraciStop(MSLane* const lane, const double startPos, const double endPos, const SUMOTime duration, const SUMOTime until,
                        const bool parking, const bool triggered, const bool containerTriggered, std::string& errorMsg) {
    //if the stop exists update the duration
    for (std::list<Stop>::iterator iter = myStops.begin(); iter != myStops.end(); iter++) {
        if (iter->lane == lane && fabs(iter->pars.endPos - endPos) < POSITION_EPS) {
            if (duration == 0 && !iter->reached) {
                myStops.erase(iter);
                // XXX also erase from myParameter->stops ?
                updateBestLanes(true);
            } else {
                iter->duration = duration;
            }
            return true;
        }
    }

    SUMOVehicleParameter::Stop newStop;
    newStop.lane = lane->getID();
    newStop.startPos = startPos;
    newStop.endPos = endPos;
    newStop.duration = duration;
    newStop.until = until;
    newStop.triggered = triggered;
    newStop.containerTriggered = containerTriggered;
    newStop.parking = parking;
    newStop.index = STOP_INDEX_FIT;
    newStop.parametersSet = STOP_START_SET | STOP_END_SET;
    if (triggered) {
        newStop.parametersSet |= STOP_TRIGGER_SET;
    }
    if (containerTriggered) {
        newStop.parametersSet |= STOP_CONTAINER_TRIGGER_SET;
    }
    if (parking) {
        newStop.parametersSet |= STOP_PARKING_SET;
    }
    const bool result = addStop(newStop, errorMsg);
    if (result) {
        /// XXX handle stops added out of order
        myParameter->stops.push_back(newStop);
    }
    if (myLane != 0) {
        updateBestLanes(true);
    }
    return result;
}


bool
MSVehicle::addTraciStopAtStoppingPlace(const std::string& stopId, const SUMOTime duration, const SUMOTime until, const bool parking,
                                       const bool triggered, const bool containerTriggered, const SumoXMLTag stoppingPlaceType, std::string& errorMsg) {
    //if the stop exists update the duration
    for (std::list<Stop>::iterator iter = myStops.begin(); iter != myStops.end(); iter++) {
        Named* stop = 0;
        switch (stoppingPlaceType) {
            case SUMO_TAG_BUS_STOP:
                stop = iter->busstop;
                break;
            case SUMO_TAG_CONTAINER_STOP:
                stop = iter->containerstop;
                break;
            case SUMO_TAG_CHARGING_STATION:
                stop = iter->chargingStation;
                break;
            case SUMO_TAG_PARKING_AREA:
                stop = iter->parkingarea;
                break;
            default:
                throw ProcessError("Invalid Stopping place type '" + toString(stoppingPlaceType) + "'");
        }
        if (stop != 0 && stop->getID() == stopId) {
            if (duration == 0 && !iter->reached) {
                myStops.erase(iter);
            } else {
                iter->duration = duration;
            }
            return true;
        }
    }

    SUMOVehicleParameter::Stop newStop;
    switch (stoppingPlaceType) {
        case SUMO_TAG_BUS_STOP:
            newStop.busstop = stopId;
            break;
        case SUMO_TAG_CONTAINER_STOP:
            newStop.containerstop = stopId;
            break;
        case SUMO_TAG_CHARGING_STATION:
            newStop.chargingStation = stopId;
            break;
        case SUMO_TAG_PARKING_AREA:
            newStop.parkingarea = stopId;
            break;
        default:
            throw ProcessError("Invalid stopping place type '" + toString(stoppingPlaceType) + "'");
    }
    MSStoppingPlace* bs = MSNet::getInstance()->getStoppingPlace(stopId, stoppingPlaceType);
    if (bs == 0) {
        errorMsg = "The " + toString(stoppingPlaceType) + " '" + stopId + "' is not known for vehicle '" + getID() + "'";
        return false;
    }
    newStop.duration = duration;
    newStop.until = until;
    newStop.triggered = triggered;
    newStop.containerTriggered = containerTriggered;
    newStop.parking = parking;
    newStop.index = STOP_INDEX_FIT;
    newStop.lane = bs->getLane().getID();
    newStop.endPos = bs->getEndLanePosition();
    newStop.startPos = bs->getBeginLanePosition();
    if (triggered) {
        newStop.parametersSet |= STOP_TRIGGER_SET;
    }
    if (containerTriggered) {
        newStop.parametersSet |= STOP_CONTAINER_TRIGGER_SET;
    }
    if (parking) {
        newStop.parametersSet |= STOP_PARKING_SET;
    }
    const bool result = addStop(newStop, errorMsg);
    if (myLane != 0) {
        updateBestLanes(true);
    }
    return result;
}


bool
MSVehicle::resumeFromStopping() {
    if (isStopped()) {
        if (myAmRegisteredAsWaitingForPerson) {
            MSNet::getInstance()->getVehicleControl().unregisterOneWaiting(true);
            myAmRegisteredAsWaitingForPerson = false;
        }
        if (myAmRegisteredAsWaitingForContainer) {
            MSNet::getInstance()->getVehicleControl().unregisterOneWaiting(false);
            myAmRegisteredAsWaitingForContainer = false;
        }
        // we have waited long enough and fulfilled any passenger-requirements
        if (myStops.front().busstop != 0) {
            // inform bus stop about leaving it
            myStops.front().busstop->leaveFrom(this);
        }
        // we have waited long enough and fulfilled any container-requirements
        if (myStops.front().containerstop != 0) {
            // inform container stop about leaving it
            myStops.front().containerstop->leaveFrom(this);
        }
        if (myStops.front().parkingarea != 0) {
            // inform parking area about leaving it
            myStops.front().parkingarea->leaveFrom(this);
        }
        // the current stop is no longer valid
        MSNet::getInstance()->getVehicleControl().removeWaiting(&myLane->getEdge(), this);
        MSDevice_Vehroutes* vehroutes = static_cast<MSDevice_Vehroutes*>(getDevice(typeid(MSDevice_Vehroutes)));
        if (vehroutes != 0) {
            vehroutes->stopEnded(myStops.front());
        }
        if (MSStopOut::active()) {
            MSStopOut::getInstance()->stopEnded(this, myStops.front());
        }
        if (myStops.front().collision && MSLane::getCollisionAction() == MSLane::COLLISION_ACTION_WARN) {
            myCollisionImmunity = TIME2STEPS(5); // leave the conflict area
        }
        myStops.pop_front();
        // do not count the stopping time towards gridlock time.
        // Other outputs use an independent counter and are not affected.
        myWaitingTime = 0;
        // maybe the next stop is on the same edge; let's rebuild best lanes
        updateBestLanes(true);
        // continue as wished...
        MSNet::getInstance()->informVehicleStateListener(this, MSNet::VEHICLE_STATE_ENDING_STOP);
        return true;
    }
    return false;
}


MSVehicle::Stop&
MSVehicle::getNextStop() {
    return myStops.front();
}


MSVehicle::Influencer&
MSVehicle::getInfluencer() {
    if (myInfluencer == 0) {
        myInfluencer = new Influencer();
    }
    return *myInfluencer;
}


const MSVehicle::Influencer*
MSVehicle::getInfluencer() const {
    return myInfluencer;
}


double
MSVehicle::getSpeedWithoutTraciInfluence() const {
    if (myInfluencer != 0 && myInfluencer->getOriginalSpeed() != -1) {
        return myInfluencer->getOriginalSpeed();
    }
    return myState.mySpeed;
}


int
MSVehicle::influenceChangeDecision(int state) {
    if (hasInfluencer()) {
        state = getInfluencer().influenceChangeDecision(
                    MSNet::getInstance()->getCurrentTimeStep(),
                    myLane->getEdge(),
                    getLaneIndex(),
                    state);
    }
    return state;
}


void
MSVehicle::setRemoteState(Position xyPos) {
    myCachedPosition = xyPos;
}


bool
MSVehicle::isRemoteControlled() const {
    return myInfluencer != 0 && myInfluencer->isRemoteControlled();
}


bool
MSVehicle::wasRemoteControlled(SUMOTime lookBack) const {
    return myInfluencer != 0 && myInfluencer->getLastAccessTimeStep() + lookBack >= MSNet::getInstance()->getCurrentTimeStep();
}


bool
MSVehicle::keepClear(const MSLink* link) const {
    if (link->hasFoes() && link->keepClear() /* && item.myLink->willHaveBlockedFoe()*/) {
        const double keepClearTime = getVehicleType().getParameter().getJMParam(SUMO_ATTR_JM_IGNORE_KEEPCLEAR_TIME, -1);
        //std::cout << SIMTIME << " veh=" << getID() << " keepClearTime=" << keepClearTime << " accWait=" << getAccumulatedWaitingSeconds() << " keepClear=" << (keepClearTime < 0 || getAccumulatedWaitingSeconds() < keepClearTime) << "\n";
        return keepClearTime < 0 || getAccumulatedWaitingSeconds() < keepClearTime;
    } else {
        return false;
    }
}


bool
MSVehicle::ignoreRed(const MSLink* link, bool canBrake) const {
    if ((myInfluencer != 0 && !myInfluencer->getEmergencyBrakeRedLight())) {
        return true;
    }
    const double ignoreRedTime = getVehicleType().getParameter().getJMParam(SUMO_ATTR_JM_DRIVE_AFTER_RED_TIME, -1);
#ifdef DEBUG_IGNORE_RED
    if (DEBUG_COND) {
        std::cout << SIMTIME << " veh=" << getID() << " link=" << link->getViaLaneOrLane()->getID() << " state=" << toString(link->getState()) << "\n";
    }
#endif
    if (ignoreRedTime < 0) {
        return false;
    } else if (link->haveYellow()) {
        // always drive at yellow when ignoring red
        return true;
    } else if (link->haveRed()) {
        assert(link->getTLLogic() != 0);
        const double redDuration = STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep() - link->getLastStateChange());
#ifdef DEBUG_IGNORE_RED
        if (DEBUG_COND) {
            std::cout
            // << SIMTIME << " veh=" << getID() << " link=" << link->getViaLaneOrLane()->getID()
                    << "   ignoreRedTime=" << ignoreRedTime
                    << " spentRed=" << redDuration
                    << " canBrake=" << canBrake << "\n";
        }
#endif
        // when activating ignoreRed behavior, vehicles will always drive if they cannot brake
        return !canBrake || ignoreRedTime > redDuration;
    } else {
        return false;
    }
}


bool
MSVehicle::passingMinor() const {
    // either on an internal lane that was entered via minor link
    // or on approach to minor link below visibility distance
    if (myLane == 0) {
        return false;
    }
    if (myLane->getEdge().isInternal()) {
        return !myLane->getIncomingLanes().front().viaLink->havePriority();
    } else if (myLFLinkLanes.size() > 0 && myLFLinkLanes.front().myLink != 0) {
        MSLink* link = myLFLinkLanes.front().myLink;
        return !link->havePriority() && myLFLinkLanes.front().myDistance <= link->getFoeVisibilityDistance();
    }
    return false;
}


void
MSVehicle::saveState(OutputDevice& out) {
    MSBaseVehicle::saveState(out);
    // here starts the vehicle internal part (see loading)
    std::vector<std::string> internals;
    internals.push_back(toString(myDeparture));
    internals.push_back(toString(distance(myRoute->begin(), myCurrEdge)));
    internals.push_back(toString(myDepartPos));
    internals.push_back(toString(myWaitingTime));
    internals.push_back(toString(myLastActionTime));
    out.writeAttr(SUMO_ATTR_STATE, internals);
    out.writeAttr(SUMO_ATTR_POSITION, myState.myPos);
    out.writeAttr(SUMO_ATTR_SPEED, myState.mySpeed);
    out.writeAttr(SUMO_ATTR_POSITION_LAT, myState.myPosLat);
    // save stops and parameters
    for (std::list<Stop>::const_iterator it = myStops.begin(); it != myStops.end(); ++it) {
        it->write(out);
    }
    myParameter->writeParams(out);
    for (std::vector<MSDevice*>::const_iterator dev = myDevices.begin(); dev != myDevices.end(); ++dev) {
        (*dev)->saveState(out);
    }
    out.closeTag();
}

void
MSVehicle::loadState(const SUMOSAXAttributes& attrs, const SUMOTime offset) {
    if (!attrs.hasAttribute(SUMO_ATTR_POSITION)) {
        throw ProcessError("Error: Invalid vehicles in state (may be a meso state)!");
    }
    int routeOffset;
    std::istringstream bis(attrs.getString(SUMO_ATTR_STATE));
    bis >> myDeparture;
    bis >> routeOffset;
    bis >> myDepartPos;
    bis >> myWaitingTime;
    bis >> myLastActionTime;
    if (hasDeparted()) {
        myCurrEdge += routeOffset;
        myDeparture -= offset;
    }
    myState.myPos = attrs.getFloat(SUMO_ATTR_POSITION);
    myState.mySpeed = attrs.getFloat(SUMO_ATTR_SPEED);
    myState.myPosLat = attrs.getFloat(SUMO_ATTR_POSITION_LAT);

    // no need to reset myCachedPosition here since state loading happens directly after creation
}

/****************************************************************************/
