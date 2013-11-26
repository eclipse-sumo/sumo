/****************************************************************************/
/// @file    MSLane.h
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Christoph Sommer
/// @author  Tino Morenz
/// @author  Michael Behrisch
/// @author  Mario Krumnow
/// @date    Mon, 12 Mar 2001
/// @version $Id$
///
// Representation of a lane in the micro simulation
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
#ifndef MSLane_h
#define MSLane_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <deque>
#include <cassert>
#include <utils/common/Named.h>
#include <utils/common/Parameterised.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/SUMOVehicle.h>
#include <utils/common/NamedRTree.h>
#include <utils/geom/PositionVector.h>
#include "MSLinkCont.h"
#include "MSMoveReminder.h"
#ifndef NO_TRACI
#include <traci-server/TraCIServerAPI_Lane.h>
#endif


// ===========================================================================
// class declarations
// ===========================================================================
class MSEdge;
class MSVehicle;
class MSLaneChanger;
class MSLink;
class MSVehicleTransfer;
class MSVehicleControl;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSLane
 * @brief Representation of a lane in the micro simulation
 *
 * Class which represents a single lane. Somekind of the main class of the
 *  simulation. Allows moving vehicles.
 */
class MSLane : public Named, public Parameterised {
public:
    /// needs access to myTmpVehicles (this maybe should be done via double-buffering!!!)
    friend class MSLaneChanger;

    friend class MSXMLRawOut;

    friend class MSQueueExport;


    /// Container for vehicles.
    typedef std::vector< MSVehicle* > VehCont;

    /** Function-object in order to find the vehicle, that has just
        passed the detector. */
    struct VehPosition : public std::binary_function < const MSVehicle*, SUMOReal, bool > {
        /// compares vehicle position to the detector position
        bool operator()(const MSVehicle* cmp, SUMOReal pos) const;
    };

public:
    /** @brief Constructor
     *
     * @param[in] id The lane's id
     * @param[in] maxSpeed The speed allowed on this lane
     * @param[in] length The lane's length
     * @param[in] edge The edge this lane belongs to
     * @param[in] numericalID The numerical id of the lane
     * @param[in] shape The shape of the lane
     * @param[in] width The width of the lane
     * @param[in] permissions Encoding of the Vehicle classes that may drive on this lane
     * @see SUMOVehicleClass
     */
    MSLane(const std::string& id, SUMOReal maxSpeed, SUMOReal length, MSEdge* const edge,
           unsigned int numericalID, const PositionVector& shape, SUMOReal width,
           SVCPermissions permissions);


    /// @brief Destructor
    virtual ~MSLane();



    /// @name Additional initialisation
    /// @{

    /** @brief Delayed initialization
     *
     *  Not all lane-members are known at the time the lane is born, above all the pointers
     *   to other lanes, so we have to add them later.
     *
     * @param[in] link An outgoing link
     */
    void addLink(MSLink* link);
    ///@}



    /// @name interaction with MSMoveReminder
    /// @{

    /** @brief Add a move-reminder to move-reminder container
     *
     * The move reminder will not be deleted by the lane.
     *
     * @param[in] rem The move reminder to add
     */
    virtual void addMoveReminder(MSMoveReminder* rem);


    /** @brief Return the list of this lane's move reminders
     * @return Previously added move reminder
     */
    inline const std::vector< MSMoveReminder* >& getMoveReminders() const {
        return myMoveReminders;
    }
    ///@}



    /// @name Vehicle insertion
    ///@{

    /** @brief Tries to insert the given vehicle
     *
     * The insertion position and speed are determined in dependence
     *  to the vehicle's departure definition, first.
     *
     * Then, the vehicle is tried to be inserted into the lane
     *  using these values by a call to "isInsertionSuccess". The result of
     *  "isInsertionSuccess" is returned.
     *
     * @param[in] v The vehicle to insert
     * @return Whether the vehicle could be inserted
     * @see isInsertionSuccess
     * @see MSVehicle::getDepartureDefinition
     * @see MSVehicle::DepartArrivalDefinition
     */
    bool insertVehicle(MSVehicle& v);


    /** @brief Tries to insert the given vehicle with the given state (speed and pos)
     *
     * Checks whether the vehicle can be inserted at the given position with the
     *  given speed so that no collisions with leader/follower occur and the speed
     *  does not cause unexpected behaviour on consecutive lanes. Returns false
     *  if the vehicle can not be inserted.
     *
     * If the insertion can take place, incorporateVehicleis called and true is returned.
     *
     * @param[in] vehicle The vehicle to insert
     * @param[in] speed The speed with which it shall be inserted
     * @param[in] pos The position at which it shall be inserted
     * @param[in] recheckNextLanes Forces patching the speed for not being too fast on next lanes
     * @param[in] notification The cause of insertion (i.e. departure, teleport, parking) defaults to departure
     * @return Whether the vehicle could be inserted
     * @see MSVehicle::enterLaneAtInsertion
     */
    virtual bool isInsertionSuccess(MSVehicle* vehicle, SUMOReal speed, SUMOReal pos,
                                    bool recheckNextLanes,
                                    MSMoveReminder::Notification notification = MSMoveReminder::NOTIFICATION_DEPARTED);

    bool checkFailure(MSVehicle* aVehicle, SUMOReal& speed, SUMOReal& dist, const SUMOReal nspeed, const bool patchSpeed, const std::string errorMsg) const;
    bool pWagGenericInsertion(MSVehicle& veh, SUMOReal speed, SUMOReal maxPos, SUMOReal minPos);
    bool pWagSimpleInsertion(MSVehicle& veh, SUMOReal speed, SUMOReal maxPos, SUMOReal minPos);
    bool maxSpeedGapInsertion(MSVehicle& veh, SUMOReal mspeed);

    /** @brief Tries to insert the given vehicle on any place
     *
     * @param[in] veh The vehicle to insert
     * @param[in] speed The maximum insertion speed
     * @param[in] notification The cause of insertion (i.e. departure, teleport, parking) defaults to departure
     * @return Whether the vehicle could be inserted
     */
    bool freeInsertion(MSVehicle& veh, SUMOReal speed,
                       MSMoveReminder::Notification notification = MSMoveReminder::NOTIFICATION_DEPARTED);


    /** @brief Inserts the given vehicle at the given position
     *
     * No checks are done, vehicle insertion using this method may
     *  generate collisions (possibly delayed).
     * @param[in] veh The vehicle to insert
     * @param[in] pos The position at which the vehicle shall be inserted
     */
    void forceVehicleInsertion(MSVehicle* veh, SUMOReal pos);
    /// @}



    /// @name Handling vehicles lapping into lanes
    /// @{

    /** @brief Sets the information about a vehicle lapping into this lane
     *
     * The given left length of vehicle which laps into this lane is used
     *  to determine the vehicle's end position in regard to this lane's length.
     * This information is set into myInlappingVehicleState; additionally, the
     *  vehicle pointer is stored in myInlappingVehicle;
     * Returns this lane's length for subtracting it from the left vehicle length.
     * @param[in] v The vehicle which laps into this lane
     * @param[in] leftVehicleLength The distance the vehicle laps into this lane
     * @return This lane's length
     */
    SUMOReal setPartialOccupation(MSVehicle* v, SUMOReal leftVehicleLength);


    /** @brief Removes the information about a vehicle lapping into this lane
     * @param[in] v The vehicle which laps into this lane
     */
    void resetPartialOccupation(MSVehicle* v);


    /** @brief Returns the vehicle which laps into this lane
     * @return The vehicle which laps into this lane, 0 if there is no such
     */
    MSVehicle* getPartialOccupator() const {
        return myInlappingVehicle;
    }


    /** @brief Returns the position of the in-lapping vehicle's end
     * @return Information about how far the vehicle laps into this lane
     */
    SUMOReal getPartialOccupatorEnd() const {
        return myInlappingVehicleEnd;
    }


    /** @brief Returns the last vehicle which is still on the lane
     *
     * The information about the last vehicle in this lane's que is returned.
     *  If there is no such vehicle, the information about the vehicle which
     *  laps into this lane is returned. If there is no such vehicle, the first
     *  returned member is 0.
     * @return Information about the last vehicle and it's back position
     */
    std::pair<MSVehicle*, SUMOReal> getLastVehicleInformation() const;
    /// @}



    /// @name Access to vehicles
    /// @{

    /** @brief Returns the number of vehicles on this lane
     * @return The number of vehicles on this lane
     */
    unsigned int getVehicleNumber() const {
        return (unsigned int) myVehicles.size();
    }


    /** @brief Returns the vehicles container; locks it for microsimulation
     *
     * Please note that it is necessary to release the vehicles container
     *  afterwards using "releaseVehicles".
     * @return The vehicles on this lane
     */
    virtual const VehCont& getVehiclesSecure() const {
        return myVehicles;
    }


    /** @brief Allows to use the container for microsimulation again
     */
    virtual void releaseVehicles() const { }
    /// @}



    /// @name Atomar value getter
    /// @{


    /** @brief Returns this lane's numerical id
     * @return This lane's numerical id
     */
    inline size_t getNumericalID() const {
        return myNumericalID;
    }


    /** @brief Returns this lane's shape
     * @return This lane's shape
     */
    inline const PositionVector& getShape() const {
        return myShape;
    }

    /* @brief fit the given lane position to a visibly suitable geometry position
     * (lane length might differ from geometry length) */
    inline SUMOReal interpolateLanePosToGeometryPos(SUMOReal lanePos) const {
        return lanePos * myLengthGeometryFactor;
    }

    /* @brief fit the given lane position to a visibly suitable geometry position
     * and return the coordinates */
    inline const Position geometryPositionAtOffset(SUMOReal offset) const {
        return myShape.positionAtOffset(interpolateLanePosToGeometryPos(offset));
    }

    /* @brief fit the given geomtry position to a valid lane position
     * (lane length might differ from geometry length) */
    inline SUMOReal interpolateGeometryPosToLanePos(SUMOReal geometryPos) const {
        return geometryPos / myLengthGeometryFactor;
    }

    /** @brief Returns the lane's maximum speed, given a vehicle's speed limit adaptation
     * @param[in] The vehicle to return the adapted speed limit for
     * @return This lane's resulting max. speed
     */
    inline SUMOReal getVehicleMaxSpeed(const SUMOVehicle* const veh) const {
        return myMaxSpeed * veh->getChosenSpeedFactor();
    }


    /** @brief Returns the lane's maximum allowed speed
     * @return This lane's maximum allowed speed
     */
    inline SUMOReal getSpeedLimit() const {
        return myMaxSpeed;
    }


    /** @brief Returns the lane's length
     * @return This lane's length
     */
    inline SUMOReal getLength() const {
        return myLength;
    }


    /** @brief Returns the vehicle class permissions for this lane
     * @return This lane's allowed vehicle classes
     */
    inline SVCPermissions getPermissions() const {
        return myPermissions;
    }


    /** @brief Returns the lane's width
     * @return This lane's width
     */
    SUMOReal getWidth() const {
        return myWidth;
    }
    /// @}



    /// @name Vehicle movement (longitudinal)
    /// @{

    /** @brief Compute safe velocities for all vehicles based on positions and
     * speeds from the last time step. Also registers
     * ApproachingVehicleInformation for all links
     *
     * This method goes through all vehicles calling their "planMove" method.
     * @see MSVehicle::planMove
     */
    virtual void planMovements(const SUMOTime t);

    /** @brief Executes planned vehicle movements with regards to right-of-way
     *
     * This method goes through all vehicles calling their executeMove method
     * which causes vehicles to update their positions and speeds.
     * Vehicles wich move to the next lane are stored in the targets lane buffer
     *
     * @see MSVehicle::executeMove
     */
    virtual bool executeMovements(SUMOTime t, std::vector<MSLane*>& into);

    /// Insert buffered vehicle into the real lane.
    virtual bool integrateNewVehicle(SUMOTime t);
    ///@}



    /// Check if vehicles are too close.
    virtual void detectCollisions(SUMOTime timestep, int stage);


    /** Returns the information whether this lane may be used to continue
        the current route */
    virtual bool appropriate(const MSVehicle* veh);


    /// returns the container with all links !!!
    const MSLinkCont& getLinkCont() const;


    /// Returns true if there is not a single vehicle on the lane.
    bool empty() const {
        assert(myVehBuffer.size() == 0);
        return myVehicles.empty();
    }

    void setMaxSpeed(SUMOReal val) {
        myMaxSpeed = val;
    }

    void setLength(SUMOReal val) {
        myLength = val;
    }


    /** @brief Returns the lane's edge
     * @return This lane's edge
     */
    MSEdge& getEdge() const {
        return *myEdge;
    }



    /// @brief Static (sic!) container methods
    /// {

    /** @brief Inserts a MSLane into the static dictionary
     *
     * Returns true if the key id isn't already in the dictionary.
     *  Otherwise returns false.
     * @param[in] id The id of the lane
     * @param[in] lane The lane itself
     * @return Whether the lane was added
     * @todo make non-static
     * @todo why is the id given? The lane is named
     */
    static bool dictionary(const std::string& id, MSLane* lane);


    /** @brief Returns the MSLane associated to the key id
     *
     * The lane is returned if exists, otherwise 0 is returned.
     * @param[in] id The id of the lane
     * @return The lane
     */
    static MSLane* dictionary(const std::string& id);


    /** @brief Clears the dictionary */
    static void clear();


    /** @brief Returns the number of stored lanes
     * @return The number of stored lanes
     */
    static size_t dictSize() {
        return myDict.size();
    }


    /** @brief Adds the ids of all stored lanes into the given vector
     * @param[in, filled] into The vector to add the IDs into
     */
    static void insertIDs(std::vector<std::string>& into);


    /** @brief Fills the given RTree with lane instances
     * @param[in, filled] into The RTree to fill
     * @see TraCILaneRTree
     */
    template<class RTREE>
    static void fill(RTREE& into);
    /// @}



    /** Same as succLink, but does not throw any assertions when
        the succeeding link could not be found;
        Returns the myLinks.end() instead; Further, the number of edges to
        look forward may be given */
    virtual MSLinkCont::const_iterator succLinkSec(const SUMOVehicle& veh,
            unsigned int nRouteSuccs,
            const MSLane& succLinkSource,
            const std::vector<MSLane*>& conts) const;


    /** Returns the information whether the given link shows at the end
        of the list of links (is not valid) */
    bool isLinkEnd(MSLinkCont::const_iterator& i) const;

    /** Returns the information whether the given link shows at the end
        of the list of links (is not valid) */
    bool isLinkEnd(MSLinkCont::iterator& i);

    /// returns the last vehicle
    virtual MSVehicle* getLastVehicle() const;
    virtual const MSVehicle* getFirstVehicle() const;




    /// @brief remove the vehicle from this lane
    virtual MSVehicle* removeVehicle(MSVehicle* remVehicle, MSMoveReminder::Notification notification);

    /// The shape of the lane
    PositionVector myShape;



    void leftByLaneChange(MSVehicle* v);
    void enteredByLaneChange(MSVehicle* v);


    /** @brief Returns the lane with the given offset parallel to this one or 0 if it does not exist
     * @param[in] offset The offset of the result lane
     */
    MSLane* getParallelLane(int offset) const;


    inline void setPermissions(SVCPermissions permissions) {
        myPermissions = permissions;
    }


    inline bool allowsVehicleClass(SUMOVehicleClass vclass) const {
        return (myPermissions & vclass) == vclass;
    }

    void addIncomingLane(MSLane* lane, MSLink* viaLink);


    struct IncomingLaneInfo {
        MSLane* lane;
        SUMOReal length;
        MSLink* viaLink;
    };

    const std::vector<IncomingLaneInfo>& getIncomingLanes() const {
        return myIncomingLanes;
    }


    void addApproachingLane(MSLane* lane);
    bool isApproachedFrom(MSEdge* const edge);
    bool isApproachedFrom(MSEdge* const edge, MSLane* const lane);



    std::pair<MSVehicle* const, SUMOReal> getFollowerOnConsecutive(SUMOReal dist, SUMOReal seen,
            SUMOReal leaderSpeed, SUMOReal backOffset, SUMOReal predMaxDecel) const;


    /// @brief return by how much further the leader must be inserted to avoid rear end collisions
    SUMOReal getMissingRearGap(SUMOReal dist, SUMOReal backOffset,
                               SUMOReal leaderSpeed, SUMOReal leaderMaxDecel) const;


    /** @brief Returns the leader and the distance to him
     *
     * Goes along the vehicle's estimated used lanes (bestLaneConts). For each link,
     *  it is determined whether the vehicle will pass it. If so, the subsequent lane
     *  is investigated. If a vehicle (leader) is found, it is returned, together with the length
     *  of the investigated lanes until this vehicle's end, including the already seen
     *  place (seen).
     *
     * If no leading vehicle was found, <0, -1> is returned.
     *
     * Pretty slow, as it has to go along lanes.
     *
     * @todo: There are some oddities:
     * - what about crossing a link at red, or if a link is closed? Has a following vehicle to be regarded or not?
     *
     * @param[in] dist The distance to investigate
     * @param[in] seen The already seen place (normally the place in front on own lane)
     * @param[in] speed The speed of the vehicle used for determining whether a subsequent link will be opened at arrival time
     * @param[in] veh The vehicle for which the information shall be computed
     * @param[in] bestLaneConts The lanes the vehicle will use in future
     * @return
     */
    std::pair<MSVehicle* const, SUMOReal> getLeaderOnConsecutive(SUMOReal dist, SUMOReal seen,
            SUMOReal speed, const MSVehicle& veh, const std::vector<MSLane*>& bestLaneConts) const;


    MSLane* getLogicalPredecessorLane() const;


    /// @name Current state retrieval
    //@{

    /** @brief Returns the mean speed on this lane
     * @return The average speed of vehicles during the last step; default speed if no vehicle was on this lane
     */
    SUMOReal getMeanSpeed() const;

    /** @brief Returns the overall waiting time on this lane
    * @return The sum of the waiting time of all vehicles during the last step;
    */
    SUMOReal getWaitingSeconds() const;


    /** @brief Returns the brutto (including minGaps) occupancy of this lane during the last step
     * @return The occupancy during the last step
     */
    SUMOReal getBruttoOccupancy() const;


    /** @brief Returns the netto (excluding minGaps) occupancy of this lane during the last step (including minGaps)
     * @return The occupancy during the last step
     */
    SUMOReal getNettoOccupancy() const;


    /** @brief Returns the sum of lengths of vehicles, including their minGaps, which were on the lane during the last step
     * @return The sum of vehicle lengths of vehicles in the last step
     */
    SUMOReal getBruttoVehLenSum() const;


    /** @brief Returns the sum of last step CO2 emissions
     * @return CO2 emissions of vehicles on this lane during the last step
     */
    SUMOReal getHBEFA_CO2Emissions() const;


    /** @brief Returns the sum of last step CO emissions
     * @return CO emissions of vehicles on this lane during the last step
     */
    SUMOReal getHBEFA_COEmissions() const;


    /** @brief Returns the sum of last step PMx emissions
     * @return PMx emissions of vehicles on this lane during the last step
     */
    SUMOReal getHBEFA_PMxEmissions() const;


    /** @brief Returns the sum of last step NOx emissions
     * @return NOx emissions of vehicles on this lane during the last step
     */
    SUMOReal getHBEFA_NOxEmissions() const;


    /** @brief Returns the sum of last step HC emissions
     * @return HC emissions of vehicles on this lane during the last step
     */
    SUMOReal getHBEFA_HCEmissions() const;


    /** @brief Returns the sum of last step fuel consumption
     * @return fuel consumption of vehicles on this lane during the last step
     */
    SUMOReal getHBEFA_FuelConsumption() const;


    /** @brief Returns the sum of last step noise emissions
     * @return noise emissions of vehicles on this lane during the last step
     */
    SUMOReal getHarmonoise_NoiseEmissions() const;
    /// @}


    /// @name State saving/loading
    /// @{

    /** @brief Saves the state of this lane into the given stream
     *
     * Basically, a list of vehicle ids
     *
     * @param[in, filled] out The (possibly binary) device to write the state into
     * @todo What about throwing an IOError?
     */
    void saveState(OutputDevice& out);

    /** @brief Loads the state of this segment with the given parameters
     *
     * This method is called for every internal que the segment has.
     *  Every vehicle is retrieved from the given MSVehicleControl and added to this
     *  lane.
     *
     * @param[in] vehIDs The vehicle ids for the current que
     * @param[in] vc The vehicle control to retrieve references vehicles from
     * @todo What about throwing an IOError?
     * @todo What about throwing an error if something else fails (a vehicle can not be referenced)?
     */
    void loadState(std::vector<std::string>& vehIDs, MSVehicleControl& vc);
    /// @}


#ifndef NO_TRACI
    /** @brief Callback for visiting the lane when traversing an RTree
     *
     * This is used in the TraCIServerAPI_Lane for context subscriptions.
     *
     * @param[in] cont The context doing all the work
     * @see TraCIServerAPI_Lane::StoringVisitor::add
     */
    void visit(const TraCIServerAPI_Lane::StoringVisitor& cont) const {
        cont.add(this);
    }
#endif

protected:
    /// moves myTmpVehicles int myVehicles after a lane change procedure
    virtual void swapAfterLaneChange(SUMOTime t);

    /** @brief Inserts the vehicle into this lane, and informs it about entering the network
     *
     * Calls the vehicles enterLaneAtInsertion function,
     *  updates statistics and modifies the active state as needed
     * @param[in] veh The vehicle to be incorporated
     * @param[in] pos The position of the vehicle
     * @param[in] speed The speed of the vehicle
     * @param[in] at
     * @param[in] notification The cause of insertion (i.e. departure, teleport, parking) defaults to departure
     */
    virtual void incorporateVehicle(MSVehicle* veh, SUMOReal pos, SUMOReal speed,
                                    const MSLane::VehCont::iterator& at,
                                    MSMoveReminder::Notification notification = MSMoveReminder::NOTIFICATION_DEPARTED);


protected:
    /// Unique numerical ID (set on reading by netload)
    size_t myNumericalID;

    /** @brief The lane's vehicles.
        The entering vehicles are inserted at the front
        of  this container and the leaving ones leave from the back, e.g. the
        vehicle in front of the junction (often called first) is
        myVehicles.back() (if it exists). And if it is an iterator at a
        vehicle, ++it points to the vehicle in front. This is the interaction
        vehicle. */
    VehCont myVehicles;

    /// Lane length [m]
    SUMOReal myLength;

    /// Lane width [m]
    SUMOReal myWidth;

    /// The lane's edge, for routing only.
    MSEdge* myEdge;

    /// Lane-wide speedlimit [m/s]
    SUMOReal myMaxSpeed;

    /** Container for lane-changing vehicles. After completion of lane-change-
        process, the two containers will be swapped. */
    VehCont myTmpVehicles;


    /** buffer for vehicles that moved from their previous lane onto this one*/
    std::vector<MSVehicle*> myVehBuffer;

    /// The vClass permissions for this lane
    SVCPermissions myPermissions;

    std::vector<IncomingLaneInfo> myIncomingLanes;
    mutable MSLane* myLogicalPredecessorLane;


    /// @brief The current length of all vehicles on this lane, including their minGaps
    SUMOReal myBruttoVehicleLengthSum;

    /// @brief The current length of all vehicles on this lane, excluding their minGaps
    SUMOReal myNettoVehicleLengthSum;

    /// @brief End position of a vehicle which laps into this lane
    SUMOReal myInlappingVehicleEnd;

    /// @brief The vehicle which laps into this lane
    MSVehicle* myInlappingVehicle;


    /** The lane's Links to it's succeeding lanes and the default
        right-of-way rule, i.e. blocked or not blocked. */
    MSLinkCont myLinks;

    std::map<MSEdge*, std::vector<MSLane*> > myApproachingLanes;

    // precomputed myShape.length / myLength
    const SUMOReal myLengthGeometryFactor;

    /// definition of the static dictionary type
    typedef std::map< std::string, MSLane* > DictType;

    /// Static dictionary to associate string-ids with objects.
    static DictType myDict;

private:
    /// @brief This lane's move reminder
    std::vector< MSMoveReminder* > myMoveReminders;


    /**
     * @class vehicle_position_sorter
     * @brief Sorts vehicles by their position (descending)
     */
    class vehicle_position_sorter {
    public:
        /// @brief Constructor
        explicit vehicle_position_sorter() { }


        /** @brief Comparing operator
         * @param[in] v1 First vehicle to compare
         * @param[in] v2 Second vehicle to compare
         * @return Whether the first vehicle is further on the lane than the second
         */
        int operator()(MSVehicle* v1, MSVehicle* v2) const;

    };

    /** @class by_id_sorter
     * @brief Sorts edges by their ids
     */
    class by_connections_to_sorter {
    public:
        /// @brief constructor
        explicit by_connections_to_sorter(const MSEdge* const e);

        /// @brief comparing operator
        int operator()(const MSEdge* const e1, const MSEdge* const e2) const;

    private:
        by_connections_to_sorter& operator=(const by_connections_to_sorter&); // just to avoid a compiler warning
    private:
        const MSEdge* const myEdge;
        SUMOReal myLaneDir;
    };

    /**
     * @class edge_finder
     */
    class edge_finder {
    public:
        edge_finder(MSEdge* e) : myEdge(e) {}
        bool operator()(const IncomingLaneInfo& ili) const {
            return &(ili.lane->getEdge()) == myEdge;
        }
    private:
        edge_finder& operator=(const edge_finder&); // just to avoid a compiler warning
    private:
        const MSEdge* const myEdge;
    };

private:
    /// @brief invalidated copy constructor
    MSLane(const MSLane&);

    /// @brief invalidated assignment operator
    MSLane& operator=(const MSLane&);


};


#endif

/****************************************************************************/

