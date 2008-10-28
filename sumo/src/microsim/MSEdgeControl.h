/****************************************************************************/
/// @file    MSEdgeControl.h
/// @author  Christian Roessel
/// @date    Mon, 09 Apr 2001
/// @version $Id$
///
// Stores edges and lanes, performs moving of vehicle
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
#include "MSEdge.h"
#include <microsim/output/MSMeanData_Net.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;
class BinaryInputDevice;


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
 *  add lanes on which a vehicle was emitted, separately, doing this into
 *  ("myChangedStateLanes") which entries are integrated at the begin of is step
 *  in "patchActiveLanes".
 */
class MSEdgeControl
{
public:
    /// @brief Container for edges.
    typedef std::vector< MSEdge* > EdgeCont;

public:
    /** @brief Constructor
     *
     * Builds LaneUsage information for each lane and assigns them to lanes.
     *
     * @param[in] singleLane Container of single lane edge
     * @param[in] multiLane Container of multi lane edge
     * @todo Assure both containers are not 0
     */
    MSEdgeControl(EdgeCont *singleLane, EdgeCont *multiLane) throw();


    /// @brief Destructor.
    ~MSEdgeControl() throw();


    /** @brief Resets information whether a lane is active for all lanes
     *
     * For each lane in "myChangedStateLanes": if the lane has at least one vehicle
     *  and is not marked as being active, it is added to the list og active lanes
     *  and marked as being active.
     */
    void patchActiveLanes() throw();


    /// @name Interfaces for longitudinal vehicle movement
    /// @{

    /** @brief Moves non critical vehicles
     *
     * "Non-critical" are those vehicles that are so far away from the lane
     *  end that they do not reach it within the brake gap (and always the
     *  first one on a lane).
     *
     * This method goes through all active lanes calling their "moveNonCritical"
     *  implementation. If this call returns true, the lane is removed from the
     *  list of active lanes.
     *
     * @see MSLane::moveNonCritical
     */
    void moveNonCritical() throw();


    /** @brief Moves (precomputes) critical vehicles
     *
     * "Critical" are those vehicles that interact with the next junction and
     *  all first vehicles. They are not moved, in fact, but their speed along
     *  the next path is precomputed.
     *
     * This method goes through all active lanes calling their "moveCritical"
     *  implementation. If this call returns true, the lane is removed from the
     *  list of active lanes.
     *
     * @see MSLane::moveCritical
     */
    void moveCritical() throw();


    /** @brief Really moves critical vehicles
     *
     * "Critical" are those vehicles that interact with the next junction and
     *  all first vehicles.
     *
     * At first, this method goes through all active lanes calling their
     *  "setCritical" implementation. If this call returns true, the lane is removed
     *  from the list of active lanes. During this call, "myWithVehicles2Integrate"
     *  is filled with lanes that obtain new vehicles.
     *
     * Then, myWithVehicles2Integrate is gone through, calling "integrateNewVehicle"
     *  of each of the stored instances. If this call returns true and the lane
     *  was not active before, it is added to the list of active lanes.
     *
     * @see MSLane::setCritical
     * @see MSLane::integrateNewVehicle
     * @todo When moving to parallel processing, the usage of myWithVehicles2Integrate would get insecure!!
     */
    void moveFirst() throw();
    /// @}


    /** @brief Moves (precomputes) critical vehicles
     *
     * Calls "changeLanes" of each of the multi-lane edges. Check then for this
     *  edge whether a lane got active, adding it to "myActiveLanes" and marking
     *  it as active in such cases.
     *
     * @see MSEdge::changeLanes
     */
    void changeLanes() throw();


    /** @brief Detect collisions
     *
     * Calls "detectCollisions" of each lane.
     * Shouldn't be necessary if model-implementation is correct.
     * The parameter is simply passed to the lane-instance for reporting.
     *
     * @param[in] timestep The current time step
     */
    void detectCollisions(SUMOTime timestep) throw();


    /** @brief Returns edges with only one lane
     *
     * @return the container storing one-lane edges
     * @todo Check: Is this secure?
     */
    const EdgeCont &getSingleLaneEdges() const throw();


    /** @brief Returns edges with more than one lane
     *
     * @return the container storing multi-lane edges
     * @todo Check: Is this secure?
     */
    const EdgeCont &getMultiLaneEdges() const throw();


    /** @brief Returns the list of names of all known edges
     *
     * @return a vector of names of all known edges
     */
    std::vector<std::string> getEdgeNames() const throw();


    /** @brief Informs the control that the given lane got active
     *
     * @param[in] l The activated lane
     * @todo Check for l==0?
     */
    void gotActive(MSLane *l) throw();


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
        MSLane *lane;
        /// @brief The lane left to the described lane (==lastNeigh if none)
        MSEdge::LaneCont::const_iterator firstNeigh;
        /// @brief The end of this lane's edge's lane container
        MSEdge::LaneCont::const_iterator lastNeigh;
        /// @brief Information whether this lane is active
        bool amActive;
        /// @brief Information whether this lane belongs to a multi-lane edge
        bool haveNeighbors;
    };

private:
    /// @brief Single lane edges.
    EdgeCont* mySingleLaneEdges;

    /// @brief Multi lane edges.
    EdgeCont* myMultiLaneEdges;

    /// @brief Copy constructor.
    MSEdgeControl(const MSEdgeControl&);

    /// @brief Assignment operator.
    MSEdgeControl& operator=(const MSEdgeControl&);

private:
    /// @brief Definition of a container about a lane's number of vehicles and neighbors
    typedef std::vector<LaneUsage> LaneUsageVector;

    /// @brief Information about lanes' number of vehicles and neighbors
    LaneUsageVector myLanes;

    /// @brief The list of active (not empty) lanes
    std::list<MSLane*> myActiveLanes;

    /// @brief A storage for lanes which shall be integrated because vehicles have moved onto them
    std::vector<MSLane*> myWithVehicles2Integrate;

    /// @brief Lanes which changed the state without informing the control
    std::set<MSLane*> myChangedStateLanes;

    /// @brief The list of active (not empty) lanes
    std::vector<SUMOTime> myLastLaneChange;

};


#endif

/****************************************************************************/

