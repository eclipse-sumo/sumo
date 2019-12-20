/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2014-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSPModel_NonInteracting.h
/// @author  Jakob Erdmann
/// @date    Mon, 13 Jan 2014
///
// The pedestrian following model (prototype)
/****************************************************************************/
#ifndef MSPModel_NonInteracting_h
#define MSPModel_NonInteracting_h

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <limits>
#include <utils/common/SUMOTime.h>
#include <utils/common/Command.h>
#include "MSPerson.h"
#include "MSPModel.h"

// ===========================================================================
// class declarations
// ===========================================================================
class MSNet;
class MSLink;
class MSLane;
class MSJunction;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSPModel_NonInteracting
 * @brief The pedestrian following model
 *
 */
class MSPModel_NonInteracting : public MSPModel {
public:

    /// @brief Constructor (it should not be necessary to construct more than one instance)
    MSPModel_NonInteracting(const OptionsCont& oc, MSNet* net);

    ~MSPModel_NonInteracting();

    /// @brief register the given person as a pedestrian
    MSTransportableStateAdapter* add(MSPerson* person, MSStageMoving* stage, SUMOTime now);

    /// @brief remove the specified person from the pedestrian simulation
    void remove(MSTransportableStateAdapter* state);

    /// @brief whether movements on intersections are modelled
    bool usingInternalLanes() {
        return false;
    }

private:
    class MoveToNextEdge : public Command {
    public:
        MoveToNextEdge(MSPerson* person, MSStageMoving& walk) : myParent(walk), myPerson(person) {}
        ~MoveToNextEdge() {}
        SUMOTime execute(SUMOTime currentTime);
        void abortWalk() {
            myPerson = 0;
        }
        const MSPerson* getPerson() const {
            return myPerson;
        }

    private:
        MSStageMoving& myParent;
        MSPerson* myPerson;
    private:
        /// @brief Invalidated assignment operator.
        MoveToNextEdge& operator=(const MoveToNextEdge&);
    };

    /// @brief abstract base class for managing callbacks to retrieve various state information from the model
    class PState : public MSTransportableStateAdapter {
    public:
        PState(MoveToNextEdge* cmd): myCommand(cmd) {};

        /// @brief abstract methods inherited from PedestrianState
        /// @{
        double getEdgePos(const MSStageMoving& stage, SUMOTime now) const;
        Position getPosition(const MSStageMoving& stage, SUMOTime now) const;
        double getAngle(const MSStageMoving& stage, SUMOTime now) const;
        SUMOTime getWaitingTime(const MSStageMoving& stage, SUMOTime now) const;
        double getSpeed(const MSStageMoving& stage) const;
        const MSEdge* getNextEdge(const MSStageMoving& stage) const;
        /// @}

        /// @brief compute walking time on edge and update state members
        SUMOTime computeWalkingTime(const MSEdge* prev, const MSStageMoving& stage, SUMOTime currentTime);
        MoveToNextEdge* getCommand() {
            return myCommand;
        }

    private:
        SUMOTime myLastEntryTime;
        SUMOTime myCurrentDuration;
        double myCurrentBeginPos;
        double myCurrentEndPos;
        MoveToNextEdge* myCommand;

    };

private:
    /// @brief the net to which to issue moveToNextEdge commands
    MSNet* myNet;

};


#endif /* MSPModel_NonInteracting_h */

