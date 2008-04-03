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

public:
    /// for access to the dictionary
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

    /// Container for lanes.
    typedef std::vector< MSLane* > LaneCont;

    /** Associative container with suceeding edges (keys) and allowed
        lanes to reach these edges. */
    typedef std::map< const MSEdge*, LaneCont* > AllowedLanesCont;
    typedef std::map< SUMOVehicleClass, AllowedLanesCont > ClassedAllowedLanesCont;

    /// @brief Destructor.
    virtual ~MSEdge() throw();


    /// Initialize the edge.
    virtual void initialize(
        AllowedLanesCont* allowed, MSLane* departLane, LaneCont* lanes,
        EdgeBasicFunction function);

    /** @brief Get the allowed lanes to reach the destination-edge.
        If there is no such edge, get 0. Then you are on the wrong edge. */
    const LaneCont* allowedLanes(const MSEdge& destination,
                                 SUMOVehicleClass vclass) const;

    /** Returns the left-lane of lane if there is one, 0 otherwise. */
    MSLane* leftLane(const MSLane* lane) const;
    MSLane* rightLane(const MSLane* lane) const;

    /** @brief Inserts edge into the static dictionary
        Returns true if the key id isn't already in the dictionary. Otherwise
        returns false. */
    static bool dictionary(std::string id, MSEdge* edge);

    /** Returns the MSEdge associated to the key id if exists, otherwise
     * returns 0. */
    static MSEdge* dictionary(std::string id);

    /** Returns the MSEdge associated to the key id if exists, otherwise
     * returns 0. */
    static MSEdge* dictionary(size_t index);

    static size_t dictSize();

    /** Clears the dictionary */
    static void clear();

    /** Returns the edge's number of lanes. */
    unsigned int nLanes() const;

    /// outputs the id of the edge
    friend std::ostream& operator<<(std::ostream& os, const MSEdge& edge);

    /** Let the edge's vehicles try to change their lanes. */
    virtual void changeLanes();

    /** returns the id of the edge */
    const std::string &getID() const;

    /** @brief Returns the edge type
        Returns information whether the edge is a sink, a source or a
        normal street; see EdgeBasicFunction */
    EdgeBasicFunction getPurpose() const;


    /// emits a vehicle on an appropriate lane
    virtual bool emit(MSVehicle &v, SUMOTime time) const;

    static std::vector< MSEdge* > getEdgeVector(void);

    const LaneCont * const getLanes(void) const;

#ifdef HAVE_INTERNAL_LANES
    const MSEdge *getInternalFollowingEdge(MSEdge *followerAfterInternal) const;
#endif

    SUMOTime getLastFailedEmissionTime() const;

    void setLastFailedEmissionTime(SUMOTime time) const;

    std::vector<MSEdge*> getFollowingEdges() const;

    std::vector<MSEdge*> getIncomingEdges() const;

    const std::string &getID() {
        return myID;
    }

    SUMOReal getEffort(SUMOReal forTime) const;
    SUMOReal getCurrentEffort() const;

    size_t getNumericalID() const {
        return myNumericalID;
    }

    size_t getNoFollowing() const {
        return myAllowed->size();
    }

    const MSEdge *getFollower(size_t num) const {
        AllowedLanesCont::const_iterator i = myAllowed->begin();
        while (num!=0) {
            ++i;
            --num;
        }
        return (*i).first;
    }

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



    virtual bool prohibits(const MSVehicle *) const {
        return false;
    }



    void addWeight(SUMOReal value, SUMOTime timeBegin, SUMOTime timeEnd);

    SUMOReal getVehicleEffort(const MSVehicle * const v, SUMOReal t) const;
    SUMOReal getCurrentVehicleEffort(const MSVehicle * const v, SUMOReal t) const;


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

