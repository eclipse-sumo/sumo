/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2014-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSCModel_NonInteracting.cpp
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @date    Tue, 29 July 2014
/// @version $Id$
///
// The container following model for tranship (prototype)
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>
//
#include <cmath>
#include <algorithm>
#include <utils/common/RandHelper.h>
#include <utils/geom/GeomHelper.h>
#include <utils/options/OptionsCont.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSJunction.h>
#include <microsim/MSEventControl.h>
#include "MSCModel_NonInteracting.h"

// ===========================================================================
// static members
// ===========================================================================
MSCModel_NonInteracting* MSCModel_NonInteracting::myModel(nullptr);


// named constants
const int CState::FORWARD(1);
const int CState::BACKWARD(-1);
const int CState::UNDEFINED_DIRECTION(0);
const double CState::LATERAL_OFFSET(0);

// ===========================================================================
// MSCModel_NonInteracting method definitions
// ===========================================================================

MSCModel_NonInteracting::MSCModel_NonInteracting(MSNet* net) :
    myNet(net) {
    assert(myNet != 0);
}


MSCModel_NonInteracting::~MSCModel_NonInteracting() {
}

MSCModel_NonInteracting*
MSCModel_NonInteracting::getModel() {
    if (myModel == nullptr) {
        MSNet* net = MSNet::getInstance();
        myModel = new MSCModel_NonInteracting(net);
    }
    return myModel;
}

CState*
MSCModel_NonInteracting::add(MSTransportable* container, MSContainer::MSContainerStage_Tranship* stage, SUMOTime now) {
    CState* state = new CState();
    const SUMOTime firstEdgeDuration = state->computeTranshipTime(nullptr, *stage, now);
    myNet->getBeginOfTimestepEvents()->addEvent(new MoveToNextEdge(container, *stage), now + firstEdgeDuration);
    return state;
}


void
MSCModel_NonInteracting::cleanup() {
    if (myModel != nullptr) {
        delete myModel;
        myModel = nullptr;
    }
}


SUMOTime
MSCModel_NonInteracting::MoveToNextEdge::execute(SUMOTime currentTime) {
    CState* state = myParent.getContainerState();
    const MSEdge* old = myParent.getEdge();
    const bool arrived = myParent.moveToNextEdge(myContainer, currentTime);
    if (arrived) {
        // tranship finished. clean up state
        delete state;
        return 0;
    } else {
        return state->computeTranshipTime(old, myParent, currentTime);
    }
}


double
CState::getEdgePos(const MSContainer::MSContainerStage_Tranship&, SUMOTime now) const {
    return myCurrentBeginPos + (myCurrentEndPos - myCurrentBeginPos) / myCurrentDuration * (now - myLastEntryTime);
}


Position
CState::getPosition(const MSContainer::MSContainerStage_Tranship& stage, SUMOTime now) const {
    const double dist = myCurrentBeginPosition.distanceTo2D(myCurrentEndPosition);    //distance between begin and end position of this tranship stage
    double pos = MIN2(STEPS2TIME(now - myLastEntryTime) *  stage.getMaxSpeed(), dist);    //the containerd shall not go beyond its end position
    return PositionVector::positionAtOffset2D(myCurrentBeginPosition, myCurrentEndPosition, pos, 0);
}


double
CState::getAngle(const MSContainer::MSContainerStage_Tranship& stage, SUMOTime now) const {
    double angle = stage.getEdgeAngle(stage.getEdge(), getEdgePos(stage, now)) + (myCurrentEndPos < myCurrentBeginPos ? 1.5 * M_PI : 0.5 * M_PI);
    if (angle > M_PI) {
        angle -= 2 * M_PI;
    }
    return angle;
}


double
CState::getSpeed(const MSContainer::MSContainerStage_Tranship& stage) const {
    return stage.getMaxSpeed();
}


SUMOTime
CState::computeTranshipTime(const MSEdge* /* prev */, const MSContainer::MSContainerStage_Tranship& stage, SUMOTime currentTime) {
    myLastEntryTime = currentTime;

    myCurrentBeginPos = stage.getDepartPos();
    myCurrentEndPos = stage.getArrivalPos();

    const MSLane* fromLane = stage.getFromEdge()->getLanes().front(); //the lane the container starts from during its tranship stage
    myCurrentBeginPosition = stage.getLanePosition(fromLane, myCurrentBeginPos, LATERAL_OFFSET);
    const MSLane* toLane = stage.getToEdge()->getLanes().front(); //the lane the container ends during its tranship stage
    myCurrentEndPosition = stage.getLanePosition(toLane, myCurrentEndPos, LATERAL_OFFSET);

    myCurrentDuration = MAX2((SUMOTime)1, TIME2STEPS(fabs(myCurrentEndPosition.distanceTo(myCurrentBeginPosition)) / stage.getMaxSpeed()));
    return myCurrentDuration;
}



/****************************************************************************/
