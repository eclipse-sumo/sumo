/****************************************************************************/
/// @file    MSEdge.h
/// @author  Christian Roessel
/// @date    Mon, 12 Mar 2001
/// @version $Id$
///
// A road/street connecting two junctions
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
#include "MSLinkCont.h"
#include <utils/common/SUMOTime.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/FloatValueTimeLine.h>
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;
class MSLaneChanger;
class OutputDevice;


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
class MSEdge
{
public:
    /**
     * @enum EdgeBasicFunction
     * For different purposes, it is necessary to know whether the edge
     * is a normal street or just a sink or a source
     * This information is represented by values from this enumeration
     */
    enum EdgeBasicFunction {
        /// the purpose of the edge is not known
        EDGEFUNCTION_UNKNOWN = -1,
        /// the edge is a normal street
        EDGEFUNCTION_NORMAL = 0,
        /// the edge is only used for vehicle emission (begin of trips)
        EDGEFUNCTION_SOURCE = 1,
        /// the edge is only used for vehicle deletion (end of trips)
        EDGEFUNCTION_SINK = 2,
        /// the edge is an internal edge
        EDGEFUNCTION_INTERNAL = 3,
        /// the edge is an within a junction
        EDGEFUNCTION_INNERJUNCTION = 4
    };


    /// @brief Container for lanes.
    typedef std::vector< MSLane* > LaneCont;

    /** @brief Suceeding edges (keys) and allowed lanes to reach these edges (values). */
    typedef std::map< const MSEdge*, LaneCont* > AllowedLanesCont;

    /** @brief Map from vehicle types to lanes that may be used to reach one of the next edges */
    typedef std::map< SUMOVehicleClass, AllowedLanesCont > ClassedAllowedLanesCont;


public:
    /// for access to the dictionary
    /// @todo rework
    friend class GUIGrid;


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
     * @param[in] lcOutput The iodevice to write lane change events to (0 if not used)
     */
    void initialize(
        MSLane* departLane, LaneCont* lanes,
        EdgeBasicFunction function, OutputDevice *lcOutput) throw();

    void closeBuilding();

    /** @brief Get the allowed lanes to reach the destination-edge.
     *
     * If there is no such edge, get 0. Then you are on the wrong edge.
     *
     * @param[in] destination The edge to reach
     * @param[in] vclass The vehicle class for which this information shall be returned
     * @return The lanes that may be used to reach the given edge, 0 if no such lanes exist
     * @todo At the begin, three checks are done for determining whether the class is of importance. Consolidate this
     * @todo There is also a further remark in the code that should be checked
     */
    const LaneCont* allowedLanes(const MSEdge& destination,
                                 SUMOVehicleClass vclass) const throw();


    /// @name Access to the edge's lanes
    /// @{

    /** @brief Returns the lane left to the one given, 0 if the given lane is leftmost 
     *
     * @param[in] lane The lane right to the one to be returned
     * @return The lane left to the given, 0 if no such lane exists
     * @todo This method searches for the given in the container; probably, this could be done faster
     */
    MSLane * const leftLane(const MSLane * const lane) const throw();


    /** @brief Returns the lane right to the one given, 0 if the given lane is rightmost 
     *
     * @param[in] lane The lane left to the one to be returned
     * @return The lane right to the given, 0 if no such lane exists
     * @todo This method searches for the given in the container; probably, this could be done faster
     */
    MSLane * const rightLane(const MSLane* const lane) const throw();


    /** @brief Returns this edge's lanes
     *
     * @return This edge's lanes
     * @todo Why not a reference?
     */
    const LaneCont * const getLanes() const throw() {
        return myLanes;
    }
    /// @}



    /// @name Access to other edge attributes
    /// @{

    /** @brief Returns the edge's number of lanes. 
     * @return This edge's number of lanes
     */
    unsigned int nLanes() const throw() {
        return (unsigned int) myLanes->size();
    }


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



    /// @name Access to succeeding/predecciding edges
    /// @{
    /** @brief Returns the list of edges which may be reached from this edge
     * @return Edges reachable from this edge
     * @todo too slow
     */
    std::vector<MSEdge*> getFollowingEdges() const throw();


    /** @brief Returns the list of edges from which this edge may be reached
     * @return Edges from which this edge may be reached
     * @todo too slow!
     */
    std::vector<MSEdge*> getIncomingEdges() const throw();


    /** @brief Returns the number of edges that may be reached from this edge
     * @return The number of following edges
     */
    unsigned int getNoFollowing() const throw() {
        return (unsigned int) myAllowed->size();
    }

    /** @brief Returns the n-th of the following edges
     * @param[in] n The index within following edges of the edge to return
     * @return The n-th of the following edges
     */
    const MSEdge * const getFollower(unsigned int n) const throw();
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



    /// @name Effort I/O
    /// @{
    /** @brief Returns the effort (travel time) for this edge and the given time
     *
     * If no travel time values have been loaded, the edge's length
     *  divided by the allowed speed is returned. 
     * Otherwise, the loaded weight matching the given time is returned.
     * 
     * @param[in] forTime The (entry) time for which the effort shall be returned
     * @return The effort (travel time) to pass the edge for the given time
     */
    SUMOReal getEffort(SUMOReal forTime) const throw();


    /** @brief Computes and returns the current effort (travel time) for this edge
     *
     * The mean travel time of all lanes is summed and divided by the number
     *  of lanes. If the result is zero, 1000000. (a very large number) is 
     *  returned, otherwise the edge's length divided by this speed.
     * 
     * @return The current effort (travel time) to pass the edge
     */
    SUMOReal getCurrentEffort() const throw();


    /** @brief Returns the given vehicle's effort (travel time) for this edge and the given time
     *
     * If a call to the vehicle's member "getEffort" with this edge
     *  and the given time returns a positive value, it is returned.
     * Otherwise the value from "getEffort" is returned.
     * 
     * @param[in] v The vehicle which may have the effort stored
     * @param[in] t The (entry) time for which the effort shall be returned
     * @return The effort (travel time) to pass the edge for the given time
     * @see getEffort
     * @see MSVehicle::getEffort
     * @deprecated This explicite combination is not variable enough; see determination in C2C-device
     */
    SUMOReal getVehicleEffort(const MSVehicle * const v, SUMOReal t) const throw();


    /** @brief Adds a loaded weight
     *
     * @param[in] value The loaded effort (travel time)
     * @param[in] timeBegin The first time step for which this value is valid
     * @param[in] timeEnd The last time step for which this value is valid
     * @todo The weights storage should be completely redesigned
     */
    void addWeight(SUMOReal value, SUMOTime timeBegin, SUMOTime timeEnd) throw();
    /// @}



    /// @name Methods releated to vehicle emission
    /// @{

    /** @brief Emits the given vehicle
     *
     * The procedure for choosing the proper lane is determined, first.
     *  In dependance to this, the proper emission lane is chosen.
     *  Emission on th most free lane, is done by a call to
     *  "freeLaneEmit". All other cases are perfomed directly
     *  within "emit".
     *
     * Emission itself is done by calling the chose lane's "emit"
     *  method.
     *
     * @param[in] v The vehicle to emit
     * @param[in] time The current simulation time
     * @return Whether the vehicle could be emitted
     * @see freeLaneEmit
     * @see MSLane::emit
     */
    virtual bool emit(MSVehicle &v, SUMOTime time) const throw();


    /** @brief Emits the given vehicle on the emptiest lane
     *
     * The emptiest lane is determined, first, and the vehicle
     *  is tried to be emitted on it. If this fails,
     *  the vehicle is tried to be emitted on any of this edge's
     *  lanes.
     * 
     * Emission itself is done by calling the chose lane's "emit"
     *  method.
     *
     * @param[in] v The vehicle to emit
     * @param[in] time The current simulation time
     * @return Whether the vehicle could be emitted
     * @see MSLane::emit
     */
    bool freeLaneEmit(MSVehicle &v, SUMOTime time, bool isReinsertion=false) const throw();


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
    virtual void changeLanes() throw();


#ifdef HAVE_INTERNAL_LANES
    /// @extension: inner junctions are not filled
    const MSEdge *getInternalFollowingEdge(MSEdge *followerAfterInternal) const throw();
#endif

    /// @todo recheck usage
    virtual bool prohibits(const MSVehicle *) const throw() {
        return false;
    }









    /** @brief Inserts edge into the static dictionary
        Returns true if the key id isn't already in the dictionary. Otherwise
        returns false. */
    static bool dictionary(const std::string &id, MSEdge* edge) throw();

    /** @brief IReturns the MSEdge associated to the key id if exists, otherwise returns 0. */
    static MSEdge* dictionary(const std::string &id) throw();

    /** @brief IReturns the MSEdge at the index */
    static MSEdge* dictionary(size_t index) throw();

    /// @brief Returns the number of edges
    static size_t dictSize() throw();

    /** @brief Clears the dictionary */
    static void clear() throw();


protected:
    /// @brief Unique ID.
    std::string myID;

    /// @brief This edge's numerical id
    unsigned int myNumericalID;

    /** @brief Container for the edge's lane.
     * Should be sorted: (right-hand-traffic) the more left the lane, the higher the container-index. */
    LaneCont* myLanes;


    /// @name Storages for allowed lanes (depending on vehicle classes)
    /// @{

    /** @brief Associative container from destination-edge to allowed-lanes. */
    AllowedLanesCont* myAllowed;

    /** @brief From vehicle class to lanes allowed to be used by it */
    ClassedAllowedLanesCont myClassedAllowed;

    /** @brief From vehicle class to lanes that may not be used by it */
    ClassedAllowedLanesCont myClassedNotAllowed;

    /// @brief Whether any class constraints exist for this edge
    bool myHaveClassConstraints;
    /// @}


    /** @brief Lane from which vehicles will depart, usually the rightmost */
    MSLane* myDepartLane;

    /** @brief This member will do the lane-change. */
    MSLaneChanger* myLaneChanger;

    /// @brief the purpose of the edge
    EdgeBasicFunction myFunction;

    /// @brief Vaporizer counter
    int myVaporizationRequests;


    /// @name Static edge container
    /// @{

    /// @brief definition of the static dictionary type
    typedef std::map< std::string, MSEdge* > DictType;

    /** @brief Static dictionary to associate string-ids with objects.
     * @depracted Move to MSEdgeControl, make non-static
     */
    static DictType myDict;

    /** @brief Static list of edges
     * @depracted Move to MSEdgeControl, make non-static
     */
    static std::vector<MSEdge*> myEdges;
    /// @}


    /// @brief The time of last emission failure
    mutable SUMOTime myLastFailedEmissionTime;


    /// @name Edge weights container
    /// @deprecated
    /// @{
    FloatValueTimeLine myOwnValueLine;
    mutable bool myHaveBuildShortCut;
    bool myHaveLoadedWeights;
    mutable SUMOReal *myPackedValueLine;
    mutable SUMOTime myShortCutBegin, myShortCutEnd, myShortCutInterval;
    mutable size_t myLastPackedIndex;
    bool myUseBoundariesOnOverride;
    /// @}


private:
    /// @brief Invalidated copy constructor.
    MSEdge(const MSEdge&);

    /// @brief assignment operator.
    MSEdge& operator=(const MSEdge&);

};


#endif

/****************************************************************************/

