/****************************************************************************/
/// @file    MSEdgeControl.h
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Christoph Sommer
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Mon, 09 Apr 2001
/// @version $Id$
///
// Stores edges and lanes, performs moving of vehicle
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSEdgeControl_h
#define MSEdgeControl_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <list>
#include <set>
#include <utils/common/SUMOTime.h>
#include <utils/common/Named.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSEdge;
class MSLane;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSEdgeControl
 * @brief Stores edges and lanes, performs moving of vehicle
 *
 * In order to avoid touching all lanes, even the empty ones, this class stores
 *  and updates the information about "active" lanes, those that have at least
 *  one vehicle on them. During longitudinal movement, this can be simply
 *  achieved through return values of the MSLane-methods, signalling either
 *  that the lane got active or inactive. This is but not possible when
 *  changing lanes, we have to go through the lanes, here. Also, we have to
 *  add lanes on which a vehicle was inserted, separately, doing this into
 *  ("myChangedStateLanes") which entries are integrated at the begin of is step
 *  in "patchActiveLanes".
 */
class MSEdgeControl {
public:
    /// @brief Container for edges.
    typedef std::vector< MSEdge* > EdgeCont;

public:
    /** @brief Constructor
     *
     * Builds LaneUsage information for each lane and assigns them to lanes.
     *
     * @param[in] edges The loaded edges
     * @todo Assure both containers are not 0
     */
    MSEdgeControl(const std::vector< MSEdge* >& edges);


    /// @brief Destructor.
    ~MSEdgeControl();


    /** @brief Resets information whether a lane is active for all lanes
     *
     * For each lane in "myChangedStateLanes": if the lane has at least one vehicle
     *  and is not marked as being active, it is added to the list og active lanes
     *  and marked as being active.
     */
    void patchActiveLanes();


    /// @name Interfaces for longitudinal vehicle movement
    /// @{

    /** @brief Compute safe velocities for all vehicles based on positions and
     * speeds from the last time step. Also registers
     * ApproachingVehicleInformation for all links
     *
     * This method goes through all active lanes calling their "planMovements" method.
     * @see MSLane::planMovements
     */
    void planMovements(SUMOTime t);


    /** @brief Executes planned vehicle movements with regards to right-of-way
     *
     * This method goes through all active lanes calling their executeMovements
     * method which causes vehicles to update their positions and speeds.
     * Lanes which receive new vehicles are stored in myWithVehicles2Integrate
     * After movements are executed the vehicles in myWithVehicles2Integrate are
     * put onto their new lanes
     * This method also updates the "active" status of lanes
     *
     * @see MSLane::executeMovements
     * @see MSLane::integrateNewVehicle
     * @todo When moving to parallel processing, the usage of myWithVehicles2Integrate would get insecure!!
     */
    void executeMovements(SUMOTime t);
    /// @}


    /** @brief Moves (precomputes) critical vehicles
     *
     * Calls "changeLanes" of each of the multi-lane edges. Check then for this
     *  edge whether a lane got active, adding it to "myActiveLanes" and marking
     *  it as active in such cases.
     *
     * @see MSEdge::changeLanes
     */
    void changeLanes(SUMOTime t);


    /** @brief Detect collisions
     *
     * Calls "detectCollisions" of each lane.
     * Shouldn't be necessary if model-implementation is correct.
     * The parameter is simply passed to the lane-instance for reporting.
     *
     * @param[in] timestep The current time step
     * @param[in] stage The current stage within the simulation step
     * @note see MSNet::simulationStep
     */
    void detectCollisions(SUMOTime timestep, const std::string& stage);


    /** @brief Returns loaded edges
     *
     * @return the container storing one-lane edges
     * @todo Check: Is this secure?
     */
    const std::vector<MSEdge*>& getEdges() const {
        return myEdges;
    }


    /** @brief Returns the list of names of all known edges
     *
     * @return a vector of names of all known edges
     */
    std::vector<std::string> getEdgeNames() const;


    /** @brief Informs the control that the given lane got active
     *
     * @param[in] l The activated lane
     * @todo Check for l==0?
     */
    void gotActive(MSLane* l);


public:
    /**
     * @struct LaneUsage
     * @brief A structure holding some basic information about a simulated lane
     *
     * To fasten up speed, this structure holds the number of vehicles using
     *  a lane and the lane's neighbours. Only lanes that are occupied are
     *  forced to compute the vehicles longitunidal movement.
     *
     * The information about a lane's neighbours speed up the computation
     *  of the lane changing.
     */
    struct LaneUsage {
        /// @brief The described lane
        MSLane* lane;
        /// @brief The lane left to the described lane (==lastNeigh if none)
        std::vector<MSLane*>::const_iterator firstNeigh;
        /// @brief The end of this lane's edge's lane container
        std::vector<MSLane*>::const_iterator lastNeigh;
        /// @brief Information whether this lane is active
        bool amActive;
        /// @brief Information whether this lane belongs to a multi-lane edge
        bool haveNeighbors;
    };

private:
    /// @brief Loaded edges
    std::vector<MSEdge*> myEdges;

    /// @brief Definition of a container about a lane's number of vehicles and neighbors
    typedef std::vector<LaneUsage> LaneUsageVector;

    /// @brief Information about lanes' number of vehicles and neighbors
    LaneUsageVector myLanes;

    /// @brief The list of active (not empty) lanes
    std::list<MSLane*> myActiveLanes;

    /// @brief A storage for lanes which shall be integrated because vehicles have moved onto them
    std::vector<MSLane*> myWithVehicles2Integrate;

    /// @brief Lanes which changed the state without informing the control
    std::set<MSLane*, Named::ComparatorIdLess> myChangedStateLanes;

    /// @brief The list of active (not empty) lanes
    std::vector<SUMOTime> myLastLaneChange;

private:
    /// @brief Copy constructor.
    MSEdgeControl(const MSEdgeControl&);

    /// @brief Assignment operator.
    MSEdgeControl& operator=(const MSEdgeControl&);

};


#endif

/****************************************************************************/

