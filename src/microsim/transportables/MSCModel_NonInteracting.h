/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2014-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSCModel_NonInteracting.h
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @date    Tue, 29 July 2014
///
// The container following model for tranship (prototype)
/****************************************************************************/
#ifndef MSCModel_NonInteracting_h
#define MSCModel_NonInteracting_h

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <limits>
#include <utils/common/SUMOTime.h>
#include <utils/common/Command.h>
#include <microsim/transportables/MSTransportable.h>
#include <microsim/transportables/MSPModel.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSNet;
class MSLink;
class MSLane;
class MSJunction;
class MSStageMoving;
class CState;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSCModel_NonInteracting
 * @brief The container following model for tranship
 *
 */
class MSCModel_NonInteracting {
public:

    /// @brief Constructor (it should not be necessary to construct more than one instance)
    MSCModel_NonInteracting(MSNet* net);

    ~MSCModel_NonInteracting();

    static MSCModel_NonInteracting* getModel();

    /// @brief remove state at simulation end
    static void cleanup();

    /// @brief register the given container as a transhiped container
    CState* add(MSTransportable* container, MSStageMoving* stage, SUMOTime now);

private:
    static MSCModel_NonInteracting* myModel;

private:
    class MoveToNextEdge : public Command {
    public:
        MoveToNextEdge(MSTransportable* container, MSStageMoving& tranship) : myParent(tranship), myContainer(container) {}
        ~MoveToNextEdge() {}
        SUMOTime execute(SUMOTime currentTime);

    private:
        MSStageMoving& myParent;
        MSTransportable* myContainer;
    private:
        /// @brief Invalidated assignment operator.
        MoveToNextEdge& operator=(const MoveToNextEdge&);
    };


private:
    /// @brief the net to which to issue moveToNextEdge commands
    MSNet* myNet;

};

class CState : public MSTransportableStateAdapter {
public:
    CState() {};

    ~CState() {};

    // @brief walking directions
    static const int FORWARD;
    static const int BACKWARD;
    static const int UNDEFINED_DIRECTION;

    /// @brief the offset for computing container positions when being transhiped
    static const double LATERAL_OFFSET;

    /// @brief return the offset from the start of the current edge measured in its natural direction
    double getEdgePos(const MSStageMoving& stage, SUMOTime now) const;
    /// @brief return the network coordinate of the container
    Position getPosition(const MSStageMoving& stage, SUMOTime now) const;
    /// @brief return the direction in which the container heading to
    double getAngle(const MSStageMoving& stage, SUMOTime now) const;
    /// @brief return the current speed of the container
    double getSpeed(const MSStageMoving& stage) const;
    /// @brief compute tranship time on edge and update state members
    SUMOTime computeTranshipTime(const MSEdge* prev, const MSStageMoving& stage, SUMOTime currentTime);
    /// @brief return the time the transportable spent standing
    SUMOTime getWaitingTime(const MSStageMoving& stage, SUMOTime now) const;
    /// @brief return the list of internal edges if the transportable is on an intersection
    const MSEdge* getNextEdge(const MSStageMoving& stage) const;



private:
    SUMOTime myLastEntryTime;
    SUMOTime myCurrentDuration;
    double myCurrentBeginPos;
    double myCurrentEndPos;
    Position myCurrentBeginPosition;  //the position the container is moving from during its tranship stage
    Position myCurrentEndPosition;  //the position the container is moving to during its tranship stage

};


#endif /* MSCModel_NonInteracting_h */

