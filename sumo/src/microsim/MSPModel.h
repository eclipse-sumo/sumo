/****************************************************************************/
/// @file    MSPModel.h
/// @author  Jakob Erdmann
/// @date    Mon, 13 Jan 2014
/// @version $Id$
///
// The pedestrian following model (prototype)
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
#ifndef MSPModel_h
#define	MSPModel_h

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
#include <microsim/MSPerson.h>

// ===========================================================================
// class declarations
// ===========================================================================
class MSNet;
class MSLane;
class MSJunction;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSPModel
 * @brief The pedestrian following model
 *
 */
class MSPModel {
public:

    static MSPModel* getModel();

    /// @brief remove state at simulation end
    static void cleanup();

    /// @brief return the appropriate lane to walk on
    static MSLane* getSidewalk(const MSEdge* edge);

    virtual ~MSPModel() {};

    /// @brief register the given person as a pedestrian
    virtual PedestrianState* add(MSPerson* person, MSPerson::MSPersonStage_Walking* stage, SUMOTime now) = 0;

    /// @brief whether a pedestrian is blocking the crossing of lane at offset distToCrossing
    virtual bool blockedAtDist(const MSLane* lane, SUMOReal distToCrossing, std::vector<const MSPerson*>* collectBlockers) = 0;

    virtual void cleanupHelper() {};

    // @brief walking directions
    static const int FORWARD;
    static const int BACKWARD;
    static const int UNDEFINED_DIRECTION;

    // @brief the safety gap to keep between the car and the pedestrian in all directions
    static const SUMOReal SAFETY_GAP;

    /// @brief the offset for computing person positions when walking on edges without a sidewalk
    static const SUMOReal SIDEWALK_OFFSET;

    /// @brief return whether the route may traversed with the given starting direction
    static bool canTraverse(int dir, const std::vector<const MSEdge*>& route);

private:
    static MSPModel* myModel;

};


/// @brief abstract base class for managing callbacks to retrieve various state information from the model
class PedestrianState {
public:
    virtual ~PedestrianState() {};

    /// @brief return the offset from the start of the current edge measured in its natural direction
    virtual SUMOReal getEdgePos(const MSPerson::MSPersonStage_Walking& stage, SUMOTime now) const = 0;

    /// @brief return the network coordinate of the person
    virtual Position getPosition(const MSPerson::MSPersonStage_Walking& stage, SUMOTime now) const = 0;

    /// @brief return the direction in which the person faces in degrees
    virtual SUMOReal getAngle(const MSPerson::MSPersonStage_Walking& stage, SUMOTime now) const = 0;

    /// @brief return the time the person spent standing
    virtual SUMOTime getWaitingTime(const MSPerson::MSPersonStage_Walking& stage, SUMOTime now) const = 0;

    /// @brief return the current speed of the person
    virtual SUMOReal getSpeed(const MSPerson::MSPersonStage_Walking& stage) const = 0;
};



#endif	/* MSPModel_h */

