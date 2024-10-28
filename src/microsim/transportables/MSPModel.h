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
/// @file    MSPModel.h
/// @author  Jakob Erdmann
/// @date    Mon, 13 Jan 2014
///
// The pedestrian following model (prototype)
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <limits>
#include <utils/common/SUMOTime.h>
#include <utils/common/Command.h>
#include <utils/common/MsgHandler.h>
#include <utils/geom/GeomHelper.h>
#include <microsim/transportables/MSPerson.h>
#ifdef HAVE_FOX
#include <utils/foxtools/fxheader.h>
#endif

// ===========================================================================
// class declarations
// ===========================================================================
class MSNet;
class MSLane;
class MSJunction;

typedef std::pair<const MSPerson*, double> PersonDist;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSPModel
 * @brief The pedestrian (and also sometimes container) movement model
 *
 */
class MSPModel {
public:
    // @brief walking directions
    static const int FORWARD;
    static const int BACKWARD;
    static const int UNDEFINED_DIRECTION;

    // @brief the safety gap to keep between the car and the pedestrian in all directions
    static const double SAFETY_GAP;

    /// @brief the offset for computing person positions when walking on edges without a sidewalk
    static const double SIDEWALK_OFFSET;

    /// @brief the default lateral offset for persons when starting a walk
    static const double UNSPECIFIED_POS_LAT;

    /// @brief magic value to encode randomized lateral offset for persons when starting a walk
    static const double RANDOM_POS_LAT;

    virtual ~MSPModel() {};

    /// @brief register the given person as a pedestrian
    virtual MSTransportableStateAdapter* add(MSTransportable* transportable, MSStageMoving* stage, SUMOTime now) = 0;

    /// @brief load the state of the given transportable
    virtual MSTransportableStateAdapter* loadState(MSTransportable* transportable, MSStageMoving* stage, std::istringstream& state) {
        UNUSED_PARAMETER(transportable);
        UNUSED_PARAMETER(stage);
        UNUSED_PARAMETER(state);
        return nullptr;
    }

    /// @brief Resets pedestrians when quick-loading state
    virtual void clearState() = 0;

    /// @brief remove the specified person from the pedestrian simulation
    virtual void remove(MSTransportableStateAdapter* state) = 0;

    /** @brief whether a pedestrian is blocking the crossing of lane for the given vehicle bondaries
     * @param[in] ego The object that inquires about blockage (and may electively ignore foes)
     * @param[in] lane The crossing to check
     * @param[in] vehSide The offset to the vehicle side near the start of the crossing
     * @param[in] vehWidth The width of the vehicle
     * @param[in] oncomingGap The distance which the vehicle wants to keep from oncoming pedestrians
     * @param[in] collectBlockers The list of persons blocking the crossing
     * @return Whether the vehicle must wait
     */
    virtual bool blockedAtDist(const SUMOTrafficObject* ego, const MSLane* lane, double vehSide, double vehWidth,
                               double oncomingGap, std::vector<const MSPerson*>* collectBlockers) {
        UNUSED_PARAMETER(ego);
        UNUSED_PARAMETER(lane);
        UNUSED_PARAMETER(vehSide);
        UNUSED_PARAMETER(vehWidth);
        UNUSED_PARAMETER(oncomingGap);
        UNUSED_PARAMETER(collectBlockers);
        return false;
    }

    /// @brief whether the given lane has pedestrians on it
    virtual bool hasPedestrians(const MSLane* lane) {
        UNUSED_PARAMETER(lane);
        return false;
    }

    /** @brief returns the next pedestrian beyond minPos that is laterally between minRight and maxLeft or nullptr
     * @param[in] lane the lane to check
     * @param[in] minPos The minimum offset along the lane after which to check
     * @param[in] minRight The rightmost border of the vehicle (0 indicates driving on the right border)
     * @param[in] maxLeft The leftmost border of the vehicle
     * @param[in] stopTime The time it would take the vehicle to come to a stop
     * @param[in] bidi Whether the vehicle is driving against the flow
     * @return The closest person (or nullptr) and the distance to it
     */
    virtual PersonDist nextBlocking(const MSLane* lane, double minPos, double minRight, double maxLeft, double stopTime = 0, bool bidi = false) {
        UNUSED_PARAMETER(lane);
        UNUSED_PARAMETER(minPos);
        UNUSED_PARAMETER(minRight);
        UNUSED_PARAMETER(maxLeft);
        UNUSED_PARAMETER(stopTime);
        UNUSED_PARAMETER(bidi);
        return PersonDist(nullptr, -1);
    }

    /// @brief whether movements on intersections are modelled
    virtual bool usingInternalLanes() = 0;

    /// @brief whether travel times and distances can reliably be calculated from the network alone
    virtual bool usingShortcuts() {
        return false;
    }

    /// @brief return the number of active objects
    virtual int getActiveNumber() = 0;

    /* @brief return the arrival direction if the route may be traversed with the given starting direction.
     * param[out] passedEdges: the number of edges that were successfully traversed
     * returns UNDEFINED_DIRECTION if the route cannot be traversed
     */
    static int canTraverse(int dir, const ConstMSEdgeVector& route, int& passedEdges);
};


/// @brief abstract base class for managing callbacks to retrieve various state information from the model
class MSTransportableStateAdapter {
public:
    virtual ~MSTransportableStateAdapter() {};

    /// @brief return the offset from the start of the current edge measured in its natural direction
    virtual double getEdgePos(SUMOTime now = 0) const = 0;

    /// @brief return the walking direction (FORWARD, BACKWARD, UNDEFINED_DIRECTION)
    virtual int getDirection() const = 0;

    /// @brief return the network coordinate of the transportable
    virtual Position getPosition(const MSStageMoving& stage, SUMOTime now) const = 0;

    /// @brief return the direction in which the transportable faces in degrees
    virtual double getAngle(const MSStageMoving& stage, SUMOTime now) const = 0;

    /// @brief return the time the transportable spent standing
    virtual SUMOTime getWaitingTime() const = 0;

    /// @brief return the current speed of the transportable
    virtual double getSpeed(const MSStageMoving& stage) const = 0;

    /// @brief return the list of internal edges if the transportable is on an intersection
    virtual const MSEdge* getNextEdge(const MSStageMoving& stage) const = 0;

    /// @brief try to move transportable to the given position
    virtual void moveTo(MSPerson* p, MSLane* lane, double lanePos, double lanePosLat, SUMOTime t) {
        UNUSED_PARAMETER(p);
        UNUSED_PARAMETER(lane);
        UNUSED_PARAMETER(lanePos);
        UNUSED_PARAMETER(lanePosLat);
        UNUSED_PARAMETER(t);
        WRITE_WARNING(TL("moveTo is ignored by the current movement model"));
    }

    /// @brief try to move transportable to the given position
    virtual void moveToXY(MSPerson* p, Position pos, MSLane* lane, double lanePos,
                          double lanePosLat, double angle, int routeOffset,
                          const ConstMSEdgeVector& edges, SUMOTime t) {
        UNUSED_PARAMETER(p);
        UNUSED_PARAMETER(pos);
        UNUSED_PARAMETER(lane);
        UNUSED_PARAMETER(lanePos);
        UNUSED_PARAMETER(lanePosLat);
        UNUSED_PARAMETER(angle);
        UNUSED_PARAMETER(routeOffset);
        UNUSED_PARAMETER(edges);
        UNUSED_PARAMETER(t);
        WRITE_WARNING(TL("moveToXY is ignored by the current movement model"));
    }

    /// @brief whether the transportable is jammed
    virtual bool isJammed() const {
        return false;
    }

    /// @brief whether the transportable has finished walking
    virtual bool isFinished() const {
        return true;
    }

    /// @brief the current lane of the transportable
    virtual const MSLane* getLane() const {
        return nullptr;
    }

    /// @brief return the total length of the current lane (in particular for on a walkingarea)
    virtual double getPathLength() const {
        return 0;
    }

    /** @brief Saves the current state into the given stream
     */
    virtual void saveState(std::ostringstream& out) {
        UNUSED_PARAMETER(out);
    }

};
