/****************************************************************************/
/// @file    MSEdge.h
/// @author  Christian Roessel
/// @date    Mon, 12 Mar 2001
/// @version $Id$
///
// A road/street connecting two junctions
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
#include <utils/common/SUMOTime.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/ValueTimeLine.h>
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;
class MSLaneChanger;
class OutputDevice;
class SUMOVehicle;
class SUMOVehicleParameter;
class MSVehicle;


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
class MSEdge {
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
     */
    MSEdge(const std::string &id, unsigned int numericalID) throw();


    /// @brief Destructor.
    virtual ~MSEdge() throw();


    /** @brief Initialize the edge.
     *
     * @param[in] allowed Information which edges may be reached from which lanes
     * @param[in] departLane The default departure lane (may be 0)
     * @param[in] lanes List of this edge's lanes
     * @param[in] function A basic type of the edge
     */
    void initialize(MSLane* departLane, std::vector<MSLane*>* lanes, EdgeBasicFunction function) throw();


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
    MSLane* leftLane(const MSLane * const lane) const throw();


    /** @brief Returns the lane right to the one given, 0 if the given lane is rightmost
     *
     * @param[in] lane The lane left to the one to be returned
     * @return The lane right to the given, 0 if no such lane exists
     * @todo This method searches for the given in the container; probably, this could be done faster
     */
    MSLane* rightLane(const MSLane* const lane) const throw();


    /** @brief Returns this edge's lanes
     *
     * @return This edge's lanes
     */
    const std::vector<MSLane*> &getLanes() const throw() {
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
            SUMOVehicleClass vclass=SVC_UNKNOWN) const throw();


    /** @brief Get the allowed lanes for the given vehicle class.
     *
     * If there is no such edge, get 0. Then you are on the wrong edge.
     *
     * @param[in] vclass The vehicle class for which this information shall be returned
     * @return The lanes that may be used by the given vclass
     */
    const std::vector<MSLane*>* allowedLanes(SUMOVehicleClass vclass=SVC_UNKNOWN) const throw();
    /// @}



    /// @name Access to other edge attributes
    /// @{

    /** @brief Returns the id of the edge
     * @return This edge's id
     */
    const std::string &getID() const throw() {
        return myID;
    }


    /** @brief Returns the edge type (EdgeBasicFunction)
     * @return This edge's EdgeBasicFunction
     * @see EdgeBasicFunction
     */
    EdgeBasicFunction getPurpose() const throw() {
        return myFunction;
    }


    /** @brief Returns the numerical id of the edge
     * @return This edge's numerical id
     */
    unsigned int getNumericalID() const throw() {
        return myNumericalID;
    }
    /// @}



    /// @name Access to succeeding/predecessing edges
    /// @{

    /** @brief Returns the list of edges which may be reached from this edge
     * @return Edges reachable from this edge
     */
    void addFollower(MSEdge* edge) throw() {
        mySuccessors.push_back(edge);
    }


    /** @brief Returns the list of edges from which this edge may be reached
     * @return Edges from which this edge may be reached
     */
    const std::vector<MSEdge*> &getIncomingEdges() const throw() {
        return myPredeccesors;
    }


    /** @brief Returns the number of edges that may be reached from this edge
     * @return The number of following edges
     */
    unsigned int getNoFollowing() const throw() {
        return (unsigned int) mySuccessors.size();
    }

    /** @brief Returns the n-th of the following edges
     * @param[in] n The index within following edges of the edge to return
     * @return The n-th of the following edges
     */
    const MSEdge* getFollower(unsigned int n) const throw() {
        return mySuccessors[n];
    }
    /// @}



    /// @name Access to vaporizing interface
    /// @{

    /** @brief Returns whether vehicles on this edge shall be vaporized
     * @return Whether no vehicle shall be on this edge
     */
    bool isVaporizing() const throw() {
        return myVaporizationRequests>0;
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
    SUMOTime incVaporization(SUMOTime t) throw(ProcessError);


    /** @brief Disables vaporization
     *
     * The internal vaporization counter is decreased what disables
     *  the vaporization if it was only once enabled.
     * Called from the event handler.
     * @param[in] t The current time (unused)
     * @return Time to next call (always 0)
     * @exception ProcessError not thrown by this method, just derived
     */
    SUMOTime decVaporization(SUMOTime t) throw(ProcessError);
    /// @}


    /** @brief Computes and returns the current travel time for this edge
     *
     * The mean travel time of all lanes is summed and divided by the number
     *  of lanes. If the result is zero, 1000000. (a very large number) is
     *  returned, otherwise the edge's length divided by this speed.
     *
     * @return The current effort (travel time) to pass the edge
     */
    SUMOReal getCurrentTravelTime() const throw();



    /// @name Methods releated to vehicle emission
    /// @{

    /** @brief Emits the given vehicle
     *
     * The procedure for choosing the proper lane is determined, first.
     *  In dependance to this, the proper emission lane is chosen.
     *
     * Emission itself is done by calling the chose lane's "emit"
     *  method.
     *
     * @param[in] v The vehicle to emit
     * @param[in] time The current simulation time
     * @return Whether the vehicle could be emitted
     * @see MSLane::emit
     */
    bool emit(SUMOVehicle &v, SUMOTime time) const throw(ProcessError);


    /** @brief Finds the emptiest lane allowing the vehicle class
     *
     * The emptiest lane is the one with the fewest vehicles on.
     *  If there is more than one, the first according to its
     *  index in the lane container is chosen.
     *
     * If allowed==0, the lanes allowed for the given vehicle class
     *  will be used
     *
     * @param[in] allowed The lanes to choose from
     * @param[in] vclass The vehicle class to look for
     * @return the least occupied lane
     * @see MSLane::emit
     * @see allowedLanes
     */
    MSLane* getFreeLane(const std::vector<MSLane*>* allowed, const SUMOVehicleClass vclass) const throw();


    /** @brief Finds a depart lane for the given vehicle parameters
     *
     * Depending on the depart lane procedure a depart lane is chosen.
     *  Repeated calls with the same vehicle may return different results
     *  if the procedure is "random" or "free". In case no appropriate
     *  lane was found, 0 is returned.
     *
     * @param[in] veh The vehicle to get the depart lane for
     * @return a possible/chosen depart lane, 0 if no lane can be used
     * @see MSLane::emit
     */
    MSLane* getDepartLane(const MSVehicle &veh) const throw();


    /** @brief Returns the last time a vehicle could not be inserted
     * @return The current value
     */
    inline SUMOTime getLastFailedEmissionTime() const throw() {
        return myLastFailedEmissionTime;
    }


    /** @brief Sets the last time a vehicle could not be inserted
     * @param[in] time the new value
     */
    inline void setLastFailedEmissionTime(SUMOTime time) const throw() {
        myLastFailedEmissionTime = time;
    }
    /// @}


    /** @brief Performs lane changing on this edge */
    virtual void changeLanes(SUMOTime t) throw();


#ifdef HAVE_INTERNAL_LANES
    /// @todo extension: inner junctions are not filled
    const MSEdge *getInternalFollowingEdge(MSEdge *followerAfterInternal) const throw();
#endif

    /// @brief Returns whether the vehicle (class) is not allowed on the edge
    bool prohibits(const SUMOVehicle * const vehicle) const throw();

    void rebuildAllowedLanes() throw();



    /** @brief Inserts edge into the static dictionary
        Returns true if the key id isn't already in the dictionary. Otherwise
        returns false. */
    static bool dictionary(const std::string &id, MSEdge* edge) throw();

    /** @brief Returns the MSEdge associated to the key id if exists, otherwise returns 0. */
    static MSEdge* dictionary(const std::string &id) throw();

    /** @brief Returns the MSEdge at the index */
    static MSEdge* dictionary(size_t index) throw();

    /// @brief Returns the number of edges
    static size_t dictSize() throw();

    /** @brief Clears the dictionary */
    static void clear() throw();

    /** @brief Inserts IDs of all known edges into the given vector */
    static void insertIDs(std::vector<std::string> &into) throw();


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
    static void parseEdgesList(const std::string &desc, std::vector<const MSEdge*> &into,
                               const std::string &rid) throw(ProcessError);


    /** @brief Parses the given string vector assuming it edge ids
     * @param[in] desc The string vector containing edge ids
     * @param[out] into The vector to fill
     * @param[in] rid The id of the route these description belongs to; used for error message generation
     * @exception ProcessError If one of the strings contained is not a known edge id
     */
    static void parseEdgesList(const std::vector<std::string> &desc, std::vector<const MSEdge*> &into,
                               const std::string &rid) throw(ProcessError);
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
        int operator()(const MSEdge * const e1, const MSEdge * const e2) const {
            return e1->getID()<e2->getID();
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
    const std::vector<MSLane*>* allowedLanes(const MSEdge *destination,
            SUMOVehicleClass vclass=SVC_UNKNOWN) const throw();

protected:
    /// @brief Unique ID.
    std::string myID;

    /// @brief This edge's numerical id
    unsigned int myNumericalID;

    /// @brief Container for the edge's lane; should be sorted: (right-hand-traffic) the more left the lane, the higher the container-index
    std::vector<MSLane*>* myLanes;

    /// @brief Lane from which vehicles will depart, usually the rightmost
    MSLane* myDepartLane;

    /// @brief This member will do the lane-change
    MSLaneChanger* myLaneChanger;

    /// @brief the purpose of the edge
    EdgeBasicFunction myFunction;

    /// @brief Vaporizer counter
    int myVaporizationRequests;

    /// @brief The time of last emission failure
    mutable SUMOTime myLastFailedEmissionTime;

    /// @brief The succeeding edges
    std::vector<MSEdge*> mySuccessors;

    /// @brief The preceeding edges
    std::vector<MSEdge*> myPredeccesors;



    /// @name Storages for allowed lanes (depending on vehicle classes)
    /// @{

    /// @brief Associative container from destination-edge to allowed-lanes.
    AllowedLanesCont myAllowed;

    /// @brief From vehicle class to lanes allowed to be used by it
    ClassedAllowedLanesCont myClassedAllowed;

    /// @brief Whether any class constraints exist for this edge
    bool myHaveClassConstraints;
    /// @}



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

