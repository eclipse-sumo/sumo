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
/// @file    MSPModel_Interacting.h
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 13 Jan 2014
///
// The abstract superclass for pedestrian models which actually interact with vehicles
/****************************************************************************/
#pragma once
#include <config.h>

#include "MSPModel.h"


// ===========================================================================
// class declarations
// ===========================================================================
class MSPModel_InteractingState;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSPModel_Interacting
 * @brief The abstract superclass for pedestrian models which actually interact with vehicles
 *
 */
class MSPModel_Interacting : public MSPModel {
public:
    ~MSPModel_Interacting();

    /// @brief Resets pedestrians when quick-loading state
    virtual void clearState();

    /// @brief remove the specified person from the pedestrian simulation
    virtual void remove(MSTransportableStateAdapter* state);

    /** @brief whether a pedestrian is blocking the crossing of lane for the given vehicle bondaries
     * @param[in] ego The object that inquires about blockage (and may electively ignore foes)
     * @param[in] lane The crossing to check
     * @param[in] vehside The offset to the vehicle side near the start of the crossing
     * @param[in] vehWidth The width of the vehicle
     * @param[in] oncomingGap The distance which the vehicle wants to keep from oncoming pedestrians
     * @param[in] collectBlockers The list of persons blocking the crossing
     * @return Whether the vehicle must wait
     */
    bool blockedAtDist(const SUMOTrafficObject* ego, const MSLane* lane, double vehSide, double vehWidth,
                       double oncomingGap, std::vector<const MSPerson*>* collectBlockers);

    /** @brief returns the next pedestrian beyond minPos that is laterally between minRight and maxLeft or nullptr
     * @param[in] lane the lane to check
     * @param[in] minPos The minimum offset along the lane after which to check
     * @param[in] minRight The rightmost border of the vehicle (0 indicates driving on the right border)
     * @param[in] maxLeft The leftmost border of the vehicle
     * @param[in] stopTime The time it would take the vehicle to come to a stop
     * @param[in] bidi Whether the vehicle is driving against the flow
     * @return The closest person (or nullptr) and the distance to it
     */
    PersonDist nextBlocking(const MSLane* lane, double minPos, double minRight, double maxLeft, double stopTime = 0, bool bidi = false);

    /// @brief whether the given lane has pedestrians on it
    bool hasPedestrians(const MSLane* lane);

    /// @brief whether movements on intersections are modelled
    //// @note function declared as member for sake of inheritance (delegates to static function)
    bool usingInternalLanes();

    static bool usingInternalLanesStatic();

    /// @brief return the number of active pedestrians
    int getActiveNumber() {
        return myNumActivePedestrians;
    }

    /// @brief increase the number of active pedestrians
    void registerActive() {
        myNumActivePedestrians++;
    }

    /// @brief unregister pedestrian approach with the junction model
    static void unregisterCrossingApproach(const MSPModel_InteractingState& ped, const MSLane* crossing);

    /// @brief placeholder function for the stripe width
    virtual double getStripeWidth() {
        return 0.;
    }

protected:
    typedef std::vector<MSPModel_InteractingState*> Pedestrians;
    typedef std::map<const MSLane*, Pedestrians, ComparatorNumericalIdLess> ActiveLanes;

    /// @brief retrieves the pedestrian vector for the given lane (may be empty)
    Pedestrians& getPedestrians(const MSLane* lane);

    /// @brief the total number of active pedestrians
    int myNumActivePedestrians = 0;

    /// @brief store of all lanes which have pedestrians on them
    ActiveLanes myActiveLanes;

    /// @brief whether an event for pedestrian processing was added
    bool myAmActive = false;

    /// @brief all crossings being approached by pedestrians
    std::set<MSLink*> myApproachedCrossings;

    /// @brief empty pedestrian vector
    static Pedestrians noPedestrians;

};


/**
 * @class MSPModel_InteractingState
 * This contains trivial implementations and members for the most used access methods
 * @brief Container for pedestrian state and individual position update function
 */
class MSPModel_InteractingState : public MSTransportableStateAdapter {
public:
    /// @brief constructor
    MSPModel_InteractingState(MSPerson* person, MSStageMoving* stage, const MSLane* lane) :
        myPerson(person),
        myStage(stage),
        myLane(lane),
        myEdgePos(stage != nullptr ? stage->getDepartPos() : 0.),
        myPosLat(stage != nullptr ? stage->getDepartPosLat() : 0.),
        myDir(MSPModel::UNDEFINED_DIRECTION),
        myWaitingToEnter(person != nullptr) {}

    ~MSPModel_InteractingState() {};

    /// @brief abstract methods inherited from MSTransportableStateAdapter
    /// @{
    /// @brief return the offset from the start of the current edge measured in its natural direction
    inline double getEdgePos(SUMOTime /* now */) const {
        return myEdgePos;
    }

    /// @brief return the walking direction (FORWARD, BACKWARD, UNDEFINED_DIRECTION)
    inline int getDirection() const {
        return myDir;
    }

    /// @brief return the time the transportable spent standing
    inline SUMOTime getWaitingTime() const {
        return myWaitingTime;
    }

    /// @brief return the current speed of the transportable
    inline double getSpeed(const MSStageMoving& /* stage */) const {
        return mySpeed;
    }

    /// @brief whether the transportable is jammed
    inline bool isJammed() const {
        return myAmJammed;
    }

    /// @brief the current lane of the transportable
    inline const MSLane* getLane() const {
        return myLane;
    }
    /// @}

    /// @brief placeholder function for the accessing the next crossing
    virtual const MSLane* getNextCrossing() const {
        return nullptr;
    }

    /// @brief return the lateral offset
    inline double getPosLat() const {
        return myPosLat;
    }

    /// @brief return the represented person
    inline MSPerson* getPerson() const {
        return myPerson;
    }

    /// @brief return the current stage
    inline MSStageMoving* getStage() const {
        return myStage;
    }

    /// @brief whether the person still waits to entere the network
    inline bool isWaitingToEnter() const {
        return myWaitingToEnter;
    }

    /// @brief return the remote position if being controlled by TraCI or JuPedSim
    inline const Position& getRemotePosition() const {
        return myRemoteXYPos;
    }

    /// @brief return ID of the person (or sometimes vehicle) being represented
    virtual const std::string& getID() const {
        return myPerson->getID();
    }

    /// @brief return the current orientation in degrees
    virtual double getAngle(const MSStageMoving&, SUMOTime) const {
        return myAngle;
    }

protected:
    /// @brief the person who is being represented
    MSPerson* myPerson = nullptr;
    /// @brief the current stage of this pedestrian
    MSStageMoving* myStage = nullptr;
    /// @brief the current lane of this pedestrian
    const MSLane* myLane = nullptr;
    /// @brief the advancement along the current lane
    double myEdgePos = 0.;
    /// @brief the orthogonal shift on the current lane
    double myPosLat = 0.;
    /// @brief the walking direction on the current lane (1 forward, -1 backward)
    int myDir = MSPModel::UNDEFINED_DIRECTION;
    /// @brief the current walking speed
    double mySpeed = 0.;
    /// @brief the current lateral walking speed
    double mySpeedLat = 0.;
    /// @brief whether the pedestrian is waiting to start its walk
    bool myWaitingToEnter = false;
    /// @brief the consecutive time spent at speed 0
    SUMOTime myWaitingTime = 0;
    /// @brief whether the person is jammed
    bool myAmJammed = false;
    /// @brief remote-controlled position
    Position myRemoteXYPos = Position::INVALID;
    /// @brief cached angle
    mutable double myAngle = std::numeric_limits<double>::max();

private:
    /// @brief Invalidated assignment operator.
    MSPModel_InteractingState& operator=(const MSPModel_InteractingState&) = delete;
};
