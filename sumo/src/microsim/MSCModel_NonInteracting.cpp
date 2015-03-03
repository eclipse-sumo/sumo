/****************************************************************************/
/// @file    MSCModel_NonInteracting.h
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @date    Tue, 29 July 2014
/// @version $Id: MSCModel_NonInteracting.cpp 16797 2014-07-28 12:08:15Z kend-an $
///
// The container following model for transfer (prototype)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2014-2014 DLR (http://www.dlr.de/) and contributors
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
//
#include <math.h>
#include <algorithm>
#include <utils/common/RandHelper.h>
#include <utils/options/OptionsCont.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSJunction.h>
#include "MSCModel_NonInteracting.h"

// ===========================================================================
// static members
// ===========================================================================
MSCModel_NonInteracting* MSCModel_NonInteracting::myModel(0);


// named constants
const int CState::FORWARD(1);
const int CState::BACKWARD(-1);
const int CState::UNDEFINED_DIRECTION(0);
const SUMOReal CState::LATERAL_OFFSET(0);

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
    if (myModel == 0) {
        MSNet* net = MSNet::getInstance();
        myModel = new MSCModel_NonInteracting(net);
    }
    return myModel;
}

CState*
MSCModel_NonInteracting::add(MSContainer* container, MSContainer::MSContainerStage_Transfer* stage, SUMOTime now) {
    CState* state = new CState();
    const SUMOTime firstEdgeDuration = state->computeTransferTime(0, *stage, now);
    myNet->getBeginOfTimestepEvents()->addEvent(new MoveToNextEdge(container, *stage),
            now + firstEdgeDuration, MSEventControl::ADAPT_AFTER_EXECUTION);
    return state;
}


SUMOTime
MSCModel_NonInteracting::MoveToNextEdge::execute(SUMOTime currentTime) {
    CState* state = myParent.getContainerState();
    const MSEdge* old = myParent.getEdge();
    const bool arrived = myParent.moveToNextEdge(myContainer, currentTime);
    if (arrived) {
        // transfer finished. clean up state
        delete state;
        return 0;
    } else {
        return state->computeTransferTime(old, myParent, currentTime);
    }
}


SUMOReal
CState::getEdgePos(const MSContainer::MSContainerStage_Transfer&, SUMOTime now) const {
    return myCurrentBeginPos + (myCurrentEndPos - myCurrentBeginPos) / myCurrentDuration * (now - myLastEntryTime);
}


Position
CState::getPosition(const MSContainer::MSContainerStage_Transfer& stage, SUMOTime now) const {
    const SUMOReal dist = myCurrentBeginPosition.distanceTo2D(myCurrentEndPosition);    //distance between begin and end position of this transfer stage
    SUMOReal pos = MIN2(STEPS2TIME(now - myLastEntryTime) *  stage.getMaxSpeed(), dist);    //the containerd shall not go beyond its end position
    return PositionVector::positionAtOffset2D(myCurrentBeginPosition, myCurrentEndPosition, pos, 0);
}


SUMOReal
CState::getAngle(const MSContainer::MSContainerStage_Transfer& stage, SUMOTime now) const {
    //todo: change angle by 90 degree
    SUMOReal angle = stage.getEdgeAngle(stage.getEdge(), getEdgePos(stage, now)) + (myCurrentEndPos < myCurrentBeginPos ? 180 : 0);
    if (angle > 180) {
        angle -= 360;
    }
    return angle;
}


SUMOReal
CState::getSpeed(const MSContainer::MSContainerStage_Transfer& stage) const {
    return stage.getMaxSpeed();
}


SUMOTime
CState::computeTransferTime(const MSEdge* prev, const MSContainer::MSContainerStage_Transfer& stage, SUMOTime currentTime) {
    myLastEntryTime = currentTime;

    myCurrentBeginPos = stage.getDepartPos();  
    myCurrentEndPos = stage.getArrivalPos();    

    const MSLane* fromLane = stage.getFromEdge()->getLanes().front(); //the lane the container starts from during its transfer stage
    myCurrentBeginPosition = stage.getLanePosition(fromLane, myCurrentBeginPos, LATERAL_OFFSET);
    const MSLane* toLane = stage.getToEdge()->getLanes().front(); //the lane the container ends during its transfer stage
    myCurrentEndPosition = stage.getLanePosition(toLane, myCurrentEndPos, LATERAL_OFFSET);

    myCurrentDuration = MAX2((SUMOTime)1, TIME2STEPS(fabs(myCurrentEndPosition.distanceTo(myCurrentBeginPosition)) / stage.getMaxSpeed()));
    return myCurrentDuration;
}



/****************************************************************************/
