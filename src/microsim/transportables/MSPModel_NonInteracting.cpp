/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2014-2024 German Aerospace Center (DLR) and others.
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
/// @file    MSPModel_NonInteracting.cpp
/// @author  Jakob Erdmann
/// @date    Mon, 13 Jan 2014
///
// The pedestrian following model (prototype)
/****************************************************************************/
#include <config.h>

#include <cmath>
#include <algorithm>
#include <utils/common/RandHelper.h>
#include <utils/geom/GeomHelper.h>
#include <utils/options/OptionsCont.h>
#include <utils/router/IntermodalNetwork.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSJunction.h>
#include <microsim/MSEventControl.h>
#include "MSPModel_NonInteracting.h"


// ===========================================================================
// static members
// ===========================================================================
const double MSPModel_NonInteracting::CState::LATERAL_OFFSET(0);


// ===========================================================================
// MSPModel_NonInteracting method definitions
// ===========================================================================
MSPModel_NonInteracting::MSPModel_NonInteracting(const OptionsCont& oc, MSNet* net) :
    myNet(net),
    myNumActivePedestrians(0) {
    assert(myNet != 0);
    UNUSED_PARAMETER(oc);
}


MSPModel_NonInteracting::~MSPModel_NonInteracting() {
}


MSTransportableStateAdapter*
MSPModel_NonInteracting::add(MSTransportable* transportable, MSStageMoving* stage, SUMOTime now) {
    myNumActivePedestrians++;
    MoveToNextEdge* const cmd = new MoveToNextEdge(transportable, *stage, this);
    PState* const state = transportable->isPerson() ? new PState(cmd) : new CState(cmd);
    myNet->getBeginOfTimestepEvents()->addEvent(cmd, now + state->computeDuration(nullptr, *stage, now));
    return state;
}


MSTransportableStateAdapter*
MSPModel_NonInteracting::loadState(MSTransportable* transportable, MSStageMoving* stage, std::istringstream& in) {
    myNumActivePedestrians++;
    MoveToNextEdge* const cmd = new MoveToNextEdge(transportable, *stage, this);
    PState* const state = transportable->isPerson() ? new PState(cmd, &in) : new CState(cmd, &in);
    myNet->getBeginOfTimestepEvents()->addEvent(cmd, state->getEventTime());
    return state;
}

void
MSPModel_NonInteracting::clearState() {
    myNumActivePedestrians = 0;
}

void
MSPModel_NonInteracting::remove(MSTransportableStateAdapter* state) {
    myNumActivePedestrians--;
    dynamic_cast<PState*>(state)->getCommand()->abortWalk();
}


// ---------------------------------------------------------------------------
// MSPModel_NonInteracting::MoveToNextEdge method definitions
// ---------------------------------------------------------------------------
SUMOTime
MSPModel_NonInteracting::MoveToNextEdge::execute(SUMOTime currentTime) {
    if (myTransportable == nullptr) {
        return 0; // descheduled
    }
    const MSEdge* old = myParent.getEdge();
    const bool arrived = myParent.moveToNextEdge(myTransportable, currentTime, myParent.getPState()->getDirection());
    if (arrived) {
        myModel->registerArrived();
        return 0;
    }
    myParent.activateEntryReminders(myTransportable);
    return static_cast<PState*>(myParent.getPState())->computeDuration(old, myParent, currentTime);
}


// ---------------------------------------------------------------------------
// MSPModel_NonInteracting::PState method definitions
// ---------------------------------------------------------------------------
MSPModel_NonInteracting::PState::PState(MoveToNextEdge* cmd, std::istringstream* in) : myCommand(cmd) {
    if (in != nullptr) {
        (*in) >> myLastEntryTime >> myCurrentDuration;
    }
}


SUMOTime
MSPModel_NonInteracting::PState::computeDuration(const MSEdge* prev, const MSStageMoving& stage, SUMOTime currentTime) {
    myLastEntryTime = currentTime;
    const MSEdge* edge = stage.getEdge();
    const MSEdge* next = stage.getNextRouteEdge();
    int dir = UNDEFINED_DIRECTION;
    if (prev == nullptr) {
        myCurrentBeginPos = stage.getDepartPos();
    } else {
        // default to FORWARD if not connected
        dir = (edge->getToJunction() == prev->getToJunction() || edge->getToJunction() == prev->getFromJunction()) ? BACKWARD : FORWARD;
        myCurrentBeginPos = dir == FORWARD ? 0 : edge->getLength();
    }
    if (next == nullptr) {
        myCurrentEndPos = stage.getArrivalPos();
    } else {
        if (dir == UNDEFINED_DIRECTION) {
            // default to FORWARD if not connected
            dir = (edge->getFromJunction() == next->getFromJunction() || edge->getFromJunction() == next->getToJunction()) ? BACKWARD : FORWARD;
        }
        myCurrentEndPos = dir == FORWARD ? edge->getLength() : 0;
    }
    // ensure that a result > 0 is returned even if the walk ends immediately
    // adding 0.5ms is done to ensure proper rounding
    myCurrentDuration = MAX2((SUMOTime)1, TIME2STEPS(fabs(myCurrentEndPos - myCurrentBeginPos) / stage.getMaxSpeed(myCommand->getTransportable())));
    //std::cout << std::setprecision(8) << SIMTIME << " curBeg=" << myCurrentBeginPos << " curEnd=" << myCurrentEndPos << " speed=" << stage.getMaxSpeed(myCommand->getTransportable()) << " dur=" << myCurrentDuration << "\n";
    // round to the next timestep to avoid systematic higher walking speed
    if ((myCurrentDuration % DELTA_T) > 0) {
        myCurrentDuration += DELTA_T;
    }
    return myCurrentDuration;
}


double
MSPModel_NonInteracting::PState::getEdgePos(SUMOTime now) const {
    //std::cout << SIMTIME << " lastEntryTime=" << myLastEntryTime << " pos=" << (myCurrentBeginPos + (myCurrentEndPos - myCurrentBeginPos) / myCurrentDuration * (now - myLastEntryTime)) << "\n";
    return myCurrentBeginPos + (myCurrentEndPos - myCurrentBeginPos) / (double)myCurrentDuration * (double)(now - myLastEntryTime);
}

int
MSPModel_NonInteracting::PState::getDirection() const {
    if (myCurrentBeginPos == myCurrentEndPos) {
        return UNDEFINED_DIRECTION;
    } else {
        return myCurrentBeginPos < myCurrentEndPos ? FORWARD : BACKWARD;
    }
}


Position
MSPModel_NonInteracting::PState::getPosition(const MSStageMoving& stage, SUMOTime now) const {
    const MSLane* lane = getSidewalk<MSEdge, MSLane>(stage.getEdge());
    if (lane == nullptr) {
        //std::string error = "Pedestrian '" + myCommand->myPerson->getID() + "' could not find sidewalk on edge '" + state.getEdge()->getID() + "', time="
        //    + time2string(MSNet::getInstance()->getCurrentTimeStep()) + ".";
        //if (!OptionsCont::getOptions().getBool("ignore-route-errors")) {
        //    throw ProcessError(error);
        //}
        lane = stage.getEdge()->getLanes().front();
    }
    const double lateral_offset = (lane->allowsVehicleClass(SVC_PEDESTRIAN) ? 0 : SIDEWALK_OFFSET
                                   * (MSGlobals::gLefthand ? -1 : 1));
    return stage.getLanePosition(lane, getEdgePos(now), lateral_offset);
}


double
MSPModel_NonInteracting::PState::getAngle(const MSStageMoving& stage, SUMOTime now) const {
    //std::cout << SIMTIME << " rawAngle=" << stage.getEdgeAngle(stage.getEdge(), getEdgePos(stage, now)) << " angle=" << stage.getEdgeAngle(stage.getEdge(), getEdgePos(stage, now)) + (myCurrentEndPos < myCurrentBeginPos ? 180 : 0) << "\n";
    double angle = stage.getEdgeAngle(stage.getEdge(), getEdgePos(now)) + (myCurrentEndPos < myCurrentBeginPos ? M_PI : 0);
    if (angle > M_PI) {
        angle -= 2 * M_PI;
    }
    return angle;
}


double
MSPModel_NonInteracting::PState::getSpeed(const MSStageMoving& stage) const {
    return stage.getMaxSpeed(myCommand->getTransportable());
}


const MSEdge*
MSPModel_NonInteracting::PState::getNextEdge(const MSStageMoving& stage) const {
    return stage.getNextRouteEdge();
}


void
MSPModel_NonInteracting::PState::saveState(std::ostringstream& out) {
    out << " " << myLastEntryTime << " " << myCurrentDuration;
}


// ---------------------------------------------------------------------------
// MSPModel_NonInteracting::CState method definitions
// ---------------------------------------------------------------------------
MSPModel_NonInteracting::CState::CState(MoveToNextEdge* cmd, std::istringstream* in) : PState(cmd, in) {
}


Position
MSPModel_NonInteracting::CState::getPosition(const MSStageMoving& stage, SUMOTime now) const {
    const double dist = myCurrentBeginPosition.distanceTo2D(myCurrentEndPosition);    //distance between begin and end position of this tranship stage
    double pos = MIN2(STEPS2TIME(now - myLastEntryTime) * stage.getMaxSpeed(), dist);    //the container shall not go beyond its end position
    return PositionVector::positionAtOffset2D(myCurrentBeginPosition, myCurrentEndPosition, pos, 0);
}


double
MSPModel_NonInteracting::CState::getAngle(const MSStageMoving& stage, SUMOTime now) const {
    double angle = stage.getEdgeAngle(stage.getEdge(), getEdgePos(now)) + (myCurrentEndPos < myCurrentBeginPos ? 1.5 * M_PI : 0.5 * M_PI);
    if (angle > M_PI) {
        angle -= 2 * M_PI;
    }
    return angle;
}


SUMOTime
MSPModel_NonInteracting::CState::computeDuration(const MSEdge* /* prev */, const MSStageMoving& stage, SUMOTime currentTime) {
    myLastEntryTime = currentTime;

    myCurrentBeginPos = stage.getDepartPos();
    myCurrentEndPos = stage.getArrivalPos();

    const MSLane* fromLane = stage.getFromEdge()->getLanes().front(); //the lane the container starts from during its tranship stage
    myCurrentBeginPosition = stage.getLanePosition(fromLane, myCurrentBeginPos, LATERAL_OFFSET);
    const MSLane* toLane = stage.getEdges().back()->getLanes().front(); //the lane the container ends during its tranship stage
    myCurrentEndPosition = stage.getLanePosition(toLane, myCurrentEndPos, LATERAL_OFFSET);

    myCurrentDuration = MAX2((SUMOTime)1, TIME2STEPS(fabs(myCurrentEndPosition.distanceTo(myCurrentBeginPosition)) / stage.getMaxSpeed()));
    return myCurrentDuration;
}


/****************************************************************************/
