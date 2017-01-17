/****************************************************************************/
/// @file    MSCModel_NonInteracting.h
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @date    Tue, 29 July 2014
/// @version $Id$
///
// The container following model for tranship (prototype)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2014-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSCModel_NonInteracting_h
#define MSCModel_NonInteracting_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <limits>
#include <utils/common/SUMOTime.h>
#include <utils/common/Command.h>
#include <microsim/MSContainer.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSNet;
class MSLink;
class MSLane;
class MSJunction;
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
    CState* add(MSTransportable* container, MSContainer::MSContainerStage_Tranship* stage, SUMOTime now);

private:
    static MSCModel_NonInteracting* myModel;

private:
    class MoveToNextEdge : public Command {
    public:
        MoveToNextEdge(MSTransportable* container, MSContainer::MSContainerStage_Tranship& tranship) : myParent(tranship), myContainer(container) {}
        ~MoveToNextEdge() {}
        SUMOTime execute(SUMOTime currentTime);

    private:
        MSContainer::MSContainerStage_Tranship& myParent;
        MSTransportable* myContainer;
    private:
        /// @brief Invalidated assignment operator.
        MoveToNextEdge& operator=(const MoveToNextEdge&);
    };


private:
    /// @brief the net to which to issue moveToNextEdge commands
    MSNet* myNet;

};

class CState {
public:
    CState() {};

    ~CState() {};

    // @brief walking directions
    static const int FORWARD;
    static const int BACKWARD;
    static const int UNDEFINED_DIRECTION;

    /// @brief the offset for computing container positions when being transhiped
    static const SUMOReal LATERAL_OFFSET;

    /// @brief return the offset from the start of the current edge measured in its natural direction
    SUMOReal getEdgePos(const MSContainer::MSContainerStage_Tranship& stage, SUMOTime now) const;
    /// @brief return the network coordinate of the container
    Position getPosition(const MSContainer::MSContainerStage_Tranship& stage, SUMOTime now) const;
    /// @brief return the direction in which the container heading to
    SUMOReal getAngle(const MSContainer::MSContainerStage_Tranship& stage, SUMOTime now) const;
    /// @brief return the current speed of the container
    SUMOReal getSpeed(const MSContainer::MSContainerStage_Tranship& stage) const;
    /// @brief compute tranship time on edge and update state members
    SUMOTime computeTranshipTime(const MSEdge* prev, const MSContainer::MSContainerStage_Tranship& stage, SUMOTime currentTime);


private:
    SUMOTime myLastEntryTime;
    SUMOTime myCurrentDuration;
    SUMOReal myCurrentBeginPos;
    SUMOReal myCurrentEndPos;
    Position myCurrentBeginPosition;  //the position the container is moving from during its tranship stage
    Position myCurrentEndPosition;  //the position the container is moving to during its tranship stage

};


#endif /* MSCModel_NonInteracting_h */

