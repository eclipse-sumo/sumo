/****************************************************************************/
/// @file    MSEdge.h
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Mon, 12 Mar 2001
/// @version $Id$
///
// A road/street connecting two junctions
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSEdge_h
#define MSEdge_h


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
#include <utils/common/Named.h>
#include <utils/common/Parameterised.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/SUMOVehicle.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/ValueTimeLine.h>
#include <utils/common/UtilExceptions.h>
#include "MSVehicleType.h"


// ===========================================================================
// class declarations
// ===========================================================================
class MSLaneChanger;
class OutputDevice;
class SUMOVehicle;
class SUMOVehicleParameter;
class MSVehicle;
class MSLane;
class MSPerson;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSEdge
 * @brief A road/street connecting two junctions
 *
 * A single connection between two junctions.
 * Holds lanes which are reponsible for vehicle movements.
 */
class MSEdge : public Named, public Parameterised {
public:
    /**
     * @enum EdgeBasicFunction
     * @brief Defines possible edge types
     *
     * For different purposes, it is necessary to know whether the edge
     *  is a normal street or something special.
     */
    enum EdgeBasicFunction {
        /// @brief The purpose of the edge is not known
        EDGEFUNCTION_UNKNOWN = -1,
        /// @brief The edge is a normal street
        EDGEFUNCTION_NORMAL = 0,
        /// @brief The edge is a macroscopic connector (source/sink)
        EDGEFUNCTION_CONNECTOR = 1,
        /// @brief The edge is an internal edge
        EDGEFUNCTION_INTERNAL = 2,
        /// @brief The edge is a district edge
        EDGEFUNCTION_DISTRICT = 3
    };


    /** @brief Suceeding edges (keys) and allowed lanes to reach these edges (values). */
    typedef std::map< const MSEdge*, std::vector<MSLane*>* > AllowedLanesCont;

    /** @brief Map from vehicle types to lanes that may be used to reach one of the next edges */
    typedef std::map< SUMOVehicleClass, AllowedLanesCont > ClassedAllowedLanesCont;


public:
    /** @brief Constructor.
     *
     * After calling this constructor, the edge is not yet initialised
     *  completely. A call to "initialize" with proper values is needed
     *  for this.
     *
     * @param[in] id The id of the edge
     * @param[in] numericalID The numerical id (index) of the edge
     * @param[in] function A basic type of the edge
     * @param[in] streetName The street name for that edge
     */
    MSEdge(const std::string& id, int numericalID, const EdgeBasicFunction function, const std::string& streetName = "");


    /// @brief Destructor.
    virtual ~MSEdge();


    /** @brief Initialize the edge.
     *
     * @param[in] allowed Information which edges may be reached from which lanes
     * @param[in] lanes List of this edge's lanes
     */
    void initialize(std::vector<MSLane*>* lanes);


    /// @todo Has to be called after all edges were built and all connections were set...; Still, is not very nice
    void closeBuilding();


    /// @name Access to the edge's lanes
    /// @{

    /** @brief Returns the lane left to the one given, 0 if the given lane is leftmost
     *
     * @param[in] lane The lane right to the one to be returned
     * @return The lane left to the given, 0 if no such lane exists
     * @todo This method searches for the given in the container; probably, this could be done faster
     */
    MSLane* leftLane(const MSLane* const lane) const;


    /** @brief Returns the lane right to the one given, 0 if the given lane is rightmost
     *
     * @param[in] lane The lane left to the one to be returned
     * @return The lane right to the given, 0 if no such lane exists
     * @todo This method searches for the given in the container; probably, this could be done faster
     */
    MSLane* rightLane(const MSLane* const lane) const;


    /** @brief Returns the lane with the given offset parallel to the given lane one or 0 if it does not exist
     *
     * @param[in] lane The base lane
     * @param[in] offset The offset of the result lane
     * @todo This method searches for the given in the container; probably, this could be done faster
     */
    MSLane* parallelLane(const MSLane* const lane, int offset) const;


    /** @brief Returns this edge's lanes
     *
     * @return This edge's lanes
     */
    const std::vector<MSLane*>& getLanes() const {
        return *myLanes;
    }


    /** @brief Get the allowed lanes to reach the destination-edge.
     *
     * If there is no such edge, get 0. Then you are on the wrong edge.
     *
     * @param[in] destination The edge to reach
     * @param[in] vclass The vehicle class for which this information shall be returned
     * @return The lanes that may be used to reach the given edge, 0 if no such lanes exist
     */
    const std::vector<MSLane*>* allowedLanes(const MSEdge& destination,
            SUMOVehicleClass vclass = SVC_UNKNOWN) const;


    /** @brief Get the allowed lanes for the given vehicle class.
     *
     * If there is no such edge, get 0. Then you are on the wrong edge.
     *
     * @param[in] vclass The vehicle class for which this information shall be returned
     * @return The lanes that may be used by the given vclass
     */
    const std::vector<MSLane*>* allowedLanes(SUMOVehicleClass vclass = SVC_UNKNOWN) const;
    /// @}



    /// @name Access to other edge attributes
    /// @{

    /** @brief Returns the edge type (EdgeBasicFunction)
     * @return This edge's EdgeBasicFunction
     * @see EdgeBasicFunction
     */
    EdgeBasicFunction getPurpose() const {
        return myFunction;
    }


    /** @brief Returns the numerical id of the edge
     * @return This edge's numerical id
     */
    int getNumericalID() const {
        return myNumericalID;
    }


    /** @brief Returns the street name of the edge
     */
    const std::string& getStreetName() const {
        return myStreetName;
    }
    /// @}



    /// @name Access to succeeding/predecessing edges
    /// @{

    /** @brief Returns the list of edges which may be reached from this edge
     * @return Edges reachable from this edge
     */
    void addFollower(MSEdge* edge) {
        mySuccessors.push_back(edge);
    }


    /** @brief Returns the list of edges from which this edge may be reached
     * @return Edges from which this edge may be reached
     */
    const std::vector<MSEdge*>& getIncomingEdges() const {
        return myPredeccesors;
    }


    /** @brief Returns the number of edges that may be reached from this edge
     * @return The number of following edges
     */
    unsigned int getNoFollowing() const {
        return (unsigned int) mySuccessors.size();
    }


    /** @brief Returns the n-th of the following edges
     * @param[in] n The index within following edges of the edge to return
     * @return The n-th of the following edges
     */
    const MSEdge* getFollower(unsigned int n) const {
        return mySuccessors[n];
    }
    /// @}



    /// @name Access to vaporizing interface
    /// @{

    /** @brief Returns whether vehicles on this edge shall be vaporized
     * @return Whether no vehicle shall be on this edge
     */
    bool isVaporizing() const {
        return myVaporizationRequests > 0;
    }


    /** @brief Enables vaporization
     *
     * The internal vaporization counter is increased enabling the
     *  vaporization.
     * Called from the event handler.
     * @param[in] t The current time (unused)
     * @return Time to next call (always 0)
     * @exception ProcessError not thrown by this method, just derived
     */
    SUMOTime incVaporization(SUMOTime t);


    /** @brief Disables vaporization
     *
     * The internal vaporization counter is decreased what disables
     *  the vaporization if it was only once enabled.
     * Called from the event handler.
     * @param[in] t The current time (unused)
     * @return Time to next call (always 0)
     * @exception ProcessError not thrown by this method, just derived
     */
    SUMOTime decVaporization(SUMOTime t);
    /// @}


    /** @brief Computes and returns the current travel time for this edge
     *
     * The mean speed of all lanes is used to compute the travel time.
     * To avoid infinite travel times, the given minimum speed is used.
     *
     * @param[in] minSpeed The minimumSpeed to assume if traffic on this edge is stopped
     * @return The current effort (travel time) to pass the edge
     */
    SUMOReal getCurrentTravelTime(const SUMOReal minSpeed = 0.00001) const;


    /// @brief returns the minimum travel time for the given vehicle
    inline SUMOReal getMinimumTravelTime(const SUMOVehicle* const veh) const {
        if (veh != 0) {
            return getLength() / MIN2(veh->getMaxSpeed(), getVehicleMaxSpeed(veh));
        } else {
            return getLength() / getSpeedLimit();
        }
    }


    /// @name Methods releated to vehicle insertion
    /// @{

    /** @brief Tries to insert the given vehicle into the network
     *
     * The procedure for choosing the proper lane is determined, first.
     *  In dependance to this, the proper lane is chosen.
     *
     * Insertion itself is done by calling the chose lane's "insertVehicle"
     *  method.
     *
     * @param[in] v The vehicle to insert
     * @param[in] time The current simulation time
     * @return Whether the vehicle could be inserted
     * @see MSLane::insertVehicle
     */
    bool insertVehicle(SUMOVehicle& v, SUMOTime time) const;


    /** @brief Finds the emptiest lane allowing the vehicle class
     *
     * The emptiest lane is the one with the fewest vehicles on.
     *  If there is more than one, the first according to its
     *  index in the lane container is chosen.
     *
     * If allowed==0, the lanes allowed for the given vehicle class
     *  will be used.
     *
     * @param[in] allowed The lanes to choose from
     * @param[in] vclass The vehicle class to look for
     * @return the least occupied lane
     * @see allowedLanes
     */
    MSLane* getFreeLane(const std::vector<MSLane*>* allowed, const SUMOVehicleClass vclass) const;


    /** @brief Finds a depart lane for the given vehicle parameters
     *
     * Depending on the depart lane procedure a depart lane is chosen.
     *  Repeated calls with the same vehicle may return different results
     *  if the procedure is "random" or "free". In case no appropriate
     *  lane was found, 0 is returned.
     *
     * @param[in] veh The vehicle to get the depart lane for
     * @return a possible/chosen depart lane, 0 if no lane can be used
     */
    MSLane* getDepartLane(const MSVehicle& veh) const;


    /** @brief Returns the last time a vehicle could not be inserted
     * @return The current value
     */
    inline SUMOTime getLastFailedInsertionTime() const {
        return myLastFailedInsertionTime;
    }


    /** @brief Sets the last time a vehicle could not be inserted
     * @param[in] time the new value
     */
    inline void setLastFailedInsertionTime(SUMOTime time) const {
        myLastFailedInsertionTime = time;
    }
    /// @}


    /** @brief Performs lane changing on this edge */
    virtual void changeLanes(SUMOTime t);


#ifdef HAVE_INTERNAL_LANES
    /// @todo extension: inner junctions are not filled
    const MSEdge* getInternalFollowingEdge(MSEdge* followerAfterInternal) const;
#endif

    /// @brief Returns whether the vehicle (class) is not allowed on the edge
    inline bool prohibits(const SUMOVehicle* const vehicle) const {
        if (vehicle == 0) {
            return false;
        }
        const SUMOVehicleClass svc = vehicle->getVehicleType().getVehicleClass();
        return (myCombinedPermissions & svc) != svc;
    }

    void rebuildAllowedLanes();


    /** @brief optimistic air distance heuristic for use in routing
     * @param[in] other The edge to which the distance shall be returned
     * @return The distance to the other edge
     */
    SUMOReal getDistanceTo(const MSEdge* other) const;


    /** @brief return the length of the edge
     * @return The edge's length
     */
    SUMOReal getLength() const;


    /** @brief Returns the speed limit of the edge
     * @caution The speed limit of the first lane is retured; should probably be the fastest edge
     * @return The maximum speed allowed on this edge
     */
    SUMOReal getSpeedLimit() const;


    /** @brief Returns the maximum speed the vehicle may use on this edge
     *
     * Note that the vehicle's max. speed is not considered herein, only the edge's speed limit and the
     *  driver's adaptation of this speed.
     * @caution Only the first lane is considered
     * @return The maximum velocity on this edge for the given vehicle
     */
    SUMOReal getVehicleMaxSpeed(const SUMOVehicle* const veh) const;

    virtual void addPerson(MSPerson* p) const {
        myPersons.insert(p);
    }

    virtual void removePerson(MSPerson* p) const {
        std::set<MSPerson*>::iterator i = myPersons.find(p);
        if (i != myPersons.end()) {
            myPersons.erase(i);
        }
    }

    inline bool isRoundabout() const {
        return myAmRoundabout;
    }

    void markAsRoundabout() {
        myAmRoundabout = true;
    }

    /** @brief Inserts edge into the static dictionary
        Returns true if the key id isn't already in the dictionary. Otherwise
        returns false. */
    static bool dictionary(const std::string& id, MSEdge* edge);

    /** @brief Returns the MSEdge associated to the key id if exists, otherwise returns 0. */
    static MSEdge* dictionary(const std::string& id);

    /** @brief Returns the MSEdge at the index */
    static MSEdge* dictionary(size_t index);

    /// @brief Returns the number of edges
    static size_t dictSize();

    /// @brief Returns the number of edges with a numerical id
    static size_t numericalDictSize();

    /** @brief Clears the dictionary */
    static void clear();

    /** @brief Inserts IDs of all known edges into the given vector */
    static void insertIDs(std::vector<std::string>& into);


public:
    /// @name Static parser helper
    /// @{

    /** @brief Parses the given string assuming it contains a list of edge ids divided by spaces
     *
     * Splits the string at spaces, uses polymorph method to generate edge vector.
     * @param[in] desc The string containing space-separated edge ids
     * @param[out] into The vector to fill
     * @param[in] rid The id of the route these description belongs to; used for error message generation
     * @exception ProcessError If one of the strings contained is not a known edge id
     */
    static void parseEdgesList(const std::string& desc, std::vector<const MSEdge*>& into,
                               const std::string& rid);


    /** @brief Parses the given string vector assuming it edge ids
     * @param[in] desc The string vector containing edge ids
     * @param[out] into The vector to fill
     * @param[in] rid The id of the route these description belongs to; used for error message generation
     * @exception ProcessError If one of the strings contained is not a known edge id
     */
    static void parseEdgesList(const std::vector<std::string>& desc, std::vector<const MSEdge*>& into,
                               const std::string& rid);
    /// @}


protected:
    /** @class by_id_sorter
     * @brief Sorts edges by their ids
     */
    class by_id_sorter {
    public:
        /// @brief constructor
        explicit by_id_sorter() { }

        /// @brief comparing operator
        int operator()(const MSEdge* const e1, const MSEdge* const e2) const {
            return e1->getID() < e2->getID();
        }

    };


    /** @brief Get the allowed lanes to reach the destination-edge.
     *
     * If there is no such edge, get 0. Then you are on the wrong edge.
     *
     * @param[in] destination The edge to reach
     * @param[in] vclass The vehicle class for which this information shall be returned
     * @return The lanes that may be used to reach the given edge, 0 if no such lanes exist
     */
    const std::vector<MSLane*>* allowedLanes(const MSEdge* destination,
            SUMOVehicleClass vclass = SVC_UNKNOWN) const;


    /// @brief lookup in map and return 0 if not found
    const std::vector<MSLane*>* getAllowedLanesWithDefault(const AllowedLanesCont& c, const MSEdge* dest) const;

protected:
    /// @brief This edge's numerical id
    int myNumericalID;

    /// @brief Container for the edge's lane; should be sorted: (right-hand-traffic) the more left the lane, the higher the container-index
    std::vector<MSLane*>* myLanes;

    /// @brief This member will do the lane-change
    MSLaneChanger* myLaneChanger;

    /// @brief the purpose of the edge
    const EdgeBasicFunction myFunction;

    /// @brief Vaporizer counter
    int myVaporizationRequests;

    /// @brief The time of last insertion failure
    mutable SUMOTime myLastFailedInsertionTime;

    /// @brief The succeeding edges
    std::vector<MSEdge*> mySuccessors;

    /// @brief The preceeding edges
    std::vector<MSEdge*> myPredeccesors;

    /// @brief Persons on the edge (only for drawing)
    mutable std::set<MSPerson*> myPersons;

    /// @name Storages for allowed lanes (depending on vehicle classes)
    /// @{

    /// @brief Associative container from destination-edge to allowed-lanes.
    AllowedLanesCont myAllowed;

    /// @brief From vehicle class to lanes allowed to be used by it
    // @note: this map is filled on demand
    mutable ClassedAllowedLanesCont myClassedAllowed;

    /// @brief The intersection of lane permissions for this edge
    SVCPermissions myMinimumPermissions;
    /// @brief The union of lane permissions for this edge
    SVCPermissions myCombinedPermissions;
    /// @}

    /// @brief the real-world name of this edge (need not be unique)
    std::string myStreetName;

    /// @brief whether this edge belongs to a roundabout
    bool myAmRoundabout;

    /// @name Static edge container
    /// @{

    /// @brief definition of the static dictionary type
    typedef std::map< std::string, MSEdge* > DictType;

    /** @brief Static dictionary to associate string-ids with objects.
     * @deprecated Move to MSEdgeControl, make non-static
     */
    static DictType myDict;

    /** @brief Static list of edges
     * @deprecated Move to MSEdgeControl, make non-static
     */
    static std::vector<MSEdge*> myEdges;
    /// @}



private:
    /// @brief Invalidated copy constructor.
    MSEdge(const MSEdge&);

    /// @brief assignment operator.
    MSEdge& operator=(const MSEdge&);

};


#endif

/****************************************************************************/

