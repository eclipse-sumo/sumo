/****************************************************************************/
/// @file    MSPModel_NonInteracting.h
/// @author  Jakob Erdmann
/// @date    Mon, 13 Jan 2014
/// @version $Id$
///
// The pedestrian following model (prototype)
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
#ifndef MSPModel_NonInteracting_h
#define MSPModel_NonInteracting_h

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
    PedestrianState* add(MSPerson* person, MSPerson::MSPersonStage_Walking* stage, SUMOTime now);

    /// @brief remove the specified person from the pedestrian simulation
    void remove(PedestrianState* state);

    /// @brief whether a pedestrian is blocking the crossing of lane at offset distToCrossing
    bool blockedAtDist(const MSLane* lane, SUMOReal distToCrossing, std::vector<const MSPerson*>* collectBlockers);

private:
    class MoveToNextEdge : public Command {
    public:
        MoveToNextEdge(MSPerson* person, MSPerson::MSPersonStage_Walking& walk) : myParent(walk), myPerson(person) {}
        ~MoveToNextEdge() {}
        SUMOTime execute(SUMOTime currentTime);
        void abortWalk() {
            myPerson = 0;
        }

    private:
        MSPerson::MSPersonStage_Walking& myParent;
        MSPerson* myPerson;
    private:
        /// @brief Invalidated assignment operator.
        MoveToNextEdge& operator=(const MoveToNextEdge&);
    };

    /// @brief abstract base class for managing callbacks to retrieve various state information from the model
    class PState : public PedestrianState {
    public:
        PState(MoveToNextEdge* cmd): myCommand(cmd) {};

        /// @brief abstract methods inherited from PedestrianState
        /// @{
        SUMOReal getEdgePos(const MSPerson::MSPersonStage_Walking& stage, SUMOTime now) const;
        Position getPosition(const MSPerson::MSPersonStage_Walking& stage, SUMOTime now) const;
        SUMOReal getAngle(const MSPerson::MSPersonStage_Walking& stage, SUMOTime now) const;
        SUMOTime getWaitingTime(const MSPerson::MSPersonStage_Walking& stage, SUMOTime now) const;
        SUMOReal getSpeed(const MSPerson::MSPersonStage_Walking& stage) const;
        const MSEdge* getNextEdge(const MSPerson::MSPersonStage_Walking& stage) const;
        /// @}

        /// @brief compute walking time on edge and update state members
        SUMOTime computeWalkingTime(const MSEdge* prev, const MSPerson::MSPersonStage_Walking& stage, SUMOTime currentTime);
        MoveToNextEdge* getCommand() {
            return myCommand;
        }

    private:
        SUMOTime myLastEntryTime;
        SUMOTime myCurrentDuration;
        SUMOReal myCurrentBeginPos;
        SUMOReal myCurrentEndPos;
        MoveToNextEdge* myCommand;

    };

private:
    /// @brief the net to which to issue moveToNextEdge commands
    MSNet* myNet;

};


#endif /* MSPModel_NonInteracting_h */

