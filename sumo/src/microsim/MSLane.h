/****************************************************************************/
/// @file    MSLane.h
/// @author  Christian Roessel
/// @date    Mon, 12 Mar 2001
/// @version $Id$
///
// Representation of a lane in the micro simulation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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

#include "MSEdge.h"
#include "MSVehicle.h"
#include "MSEdgeControl.h"
#include <bitset>
#include <deque>
#include <vector>
#include <utility>
#include <map>
#include <string>
#include <iostream>
#include "MSNet.h"
#include <utils/geom/Position2DVector.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/SUMOVehicleClass.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSLaneChanger;
class MSLink;
class MSMoveReminder;
class GUILaneWrapper;
class GUIGlObjectStorage;
class MSVehicleTransfer;
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
class MSLane {
public:
    /// needs access to myTmpVehicles (this maybe should be done via SUMOReal-buffering!!!)
    friend class MSLaneChanger;

    friend class GUILaneWrapper;

    friend class MSXMLRawOut;

    /** Function-object in order to find the vehicle, that has just
        passed the detector. */
    struct VehPosition : public std::binary_function< const MSVehicle*,
                SUMOReal, bool > {
        /// compares vehicle position to the detector position
        bool operator()(const MSVehicle* cmp, SUMOReal pos) const {
            return cmp->getPositionOnLane() >= pos;
        }
    };


public:
    /** @brief Constructor
     *
     * @param[in] id The lane's id
     * @param[in] maxSpeed The speed allwoed on this lane
     * @param[in] length The lane's length
     * @param[in] edge The edge this lane belongs to
     * @param[in] numericalID The numerical id of the lane
     * @param[in] allowed Vehicle classes that explicitely may drive on this lane
     * @param[in] disallowed Vehicle classes that are explicitaly forbidden on this lane
     * @see SUMOVehicleClass
     */
    MSLane(const std::string &id, SUMOReal maxSpeed, SUMOReal length, MSEdge * const edge,
           unsigned int numericalID, const Position2DVector &shape,
           const std::vector<SUMOVehicleClass> &allowed,
           const std::vector<SUMOVehicleClass> &disallowed) throw();


    /// @brief Destructor
    virtual ~MSLane() throw();



    /// @name Additional initialisation
    /// @{

    /** @brief Delayed initialization
     *
     *  Not all lane-members are known at the time the lane is born, above all the pointers
     *   to other lanes, so we have to initialize later.
     *
     * @param[in] succs The list of (outgoing) links
     * @todo Why are succs not const?
     */
    void initialize(MSLinkCont* succs);
    ///@}



    /// @name interaction with MSMoveReminder
    /// @{

    /// @brief Definition of a container for move reminder
    typedef std::vector< MSMoveReminder* > MoveReminderCont;


    /** @brief Add a move-reminder to move-reminder container
     *
     * The move reminder will not be deleted by the lane.
     *
     * @param[in] rem The move reminder to add
     */
    virtual void addMoveReminder(MSMoveReminder* rem) throw();


    /** @brief Return the list of this lane's move reminders
     * @return Previously added move reminder
     */
    inline const MoveReminderCont &getMoveReminders() const throw() {
        return myMoveReminders;
    }
    ///@}



    /// @name Vehicle emission
    ///@{

    /** @brief Tries to emit the given vehicle
     *
     * The emission position and speed are determined in dependence
     *  to the vehicle's departure definition, first.  If "isReinsertion" is set,
     *  meaning the vehicle tries to end a teleportation, then the values for
     *  the fastest emission are used (speed=max, pos=free).
     *
     * Then, the vehicle is tried to be inserted into the lane
     *  using these values by a call to "isEmissionSuccess". The result of
     *  "isEmissionSuccess" is returned.
     *
     * @param[in] v The vehicle to emit
     * @return Whether the vehicle could be emitted
     * @see isEmissionSuccess
     * @see MSVehicle::getDepartureDefinition
     * @see MSVehicle::DepartArrivalDefinition
     */
    bool emit(MSVehicle& v) throw();


    /** @brief Tries to emit the given vehicle with the given state (speed and pos)
     *
     * Checks whether the vehicle can be inserted at the given position with the
     *  given speed so that no collisions with leader/follower occure and the speed
     *  does not yield in unexpected behaviour on consecutive lanes. Returns false
     *  if the vehicle can not be inserted.
     *
     * If the insertion can take place, the vehicle's "enterLaneAtEmit" method is called,
     *  the vehicle is inserted into the lane's vehicle container ("myVehicles"), the
     *  lane's statistical information is patched (including the mean data). true is returned.
     *
     * @param[in] vehicle The vehicle to emit
     * @param[in] speed The speed with which it shall be emitted
     * @param[in] pos The position at which it shall be emitted
     * @param[in] recheckNextLanes Forces patching the speed for not being too fast on next lanes
     * @return Whether the vehicle could be emitted
     * @see MSVehicle::enterLaneAtEmit
     */
    virtual bool isEmissionSuccess(MSVehicle* vehicle, SUMOReal speed, SUMOReal pos,
                                   bool recheckNextLanes) throw();


    /** @brief Tries to emit the given vehicle on any place
     *
     * @param[in] veh The vehicle to emit
     * @param[in] speed The maximum emission speed
     * @return Whether the vehicle could be emitted
     */
    bool freeEmit(MSVehicle& veh, SUMOReal speed) throw();
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
    SUMOReal setPartialOccupation(MSVehicle *v, SUMOReal leftVehicleLength) throw();


    /** @brief Removes the information about a vehicle lapping into this lane
     * @param[in] v The vehicle which laps into this lane
     */
    void resetPartialOccupation(MSVehicle *v) throw();


    /** @brief Returns the vehicle which laps into this lane
     * @return The vehicle which laps into this lane, 0 if there is no such
     */
    MSVehicle *getPartialOccupator() const throw() {
        return myInlappingVehicle;
    }


    /** @brief Returns the position of the in-lapping vehicle's end
     * @return Information about how far the vehicle laps into this lane
     */
    SUMOReal getPartialOccupatorEnd() const throw() {
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
    std::pair<MSVehicle*, SUMOReal> getLastVehicleInformation() const throw();
    /// @}



    /// @name Access to vehicles
    /// @{

    /** @brief Returns the number of vehicles on this lane
     * @return The number of vehicles on this lane
     */
    unsigned int getVehicleNumber() const throw() {
        return (unsigned int) myVehicles.size();
    }


    /** @brief Returns the vehicles container; locks it for microsimulation
     *
     * Please note that it is necessary to release the vehicles container
     *  afterwards using "releaseVehicles".
     * @return The vehicles on this lane
     */
    virtual const std::deque< MSVehicle* > &getVehiclesSecure() const throw() {
        return myVehicles;
    }


    /** @brief Allows to use the container for microsimulation again
     */
    virtual void releaseVehicles() const throw() { }
    /// @}



    /// @name Atomar value getter
    /// @{

    /** @brief Returns this lane's id
     * @return This lane's id
     */
    const std::string &getID() const throw() {
        return myID;
    }


    /** @brief Returns this lane's numerical id
     * @return This lane's numerical id
     */
    size_t getNumericalID() const throw() {
        return myNumericalID;
    }


    /** @brief Returns this lane's shape
     * @return This lane's shape
     */
    const Position2DVector &getShape() const throw() {
        return myShape;
    }


    /** @brief Returns the lane's maximum speed
     * @return This lane's maximum speed
     */
    SUMOReal getMaxSpeed() const throw() {
        return myMaxSpeed;
    }


    /** @brief Returns the lane's length
     * @return This lane's length
     */
    SUMOReal getLength() const throw() {
        return myLength;
    }


    /** @brief Returns vehicle classes explicitely allowed on this lane
     * @return This lane's allowed vehicle classes
     */
    const std::vector<SUMOVehicleClass> &getAllowedClasses() const throw() {
        return myAllowedClasses;
    }


    /** @brief Returns vehicle classes explicitely disallowed on this lane
     * @return This lane's disallowed vehicle classes
     */
    const std::vector<SUMOVehicleClass> &getNotAllowedClasses() const throw() {
        return myNotAllowedClasses;
    }
    /// @}



    /// @name Vehicle movement (longitudinal)
    /// @{

    virtual bool moveCritical(SUMOTime t);

    /** Moves the critical vehicles
        This step is done after the responds have been set */
    virtual bool setCritical(SUMOTime t, std::vector<MSLane*> &into);

    /// Insert buffered vehicle into the real lane.
    virtual bool integrateNewVehicle(SUMOTime t);
    ///@}



    /// Check if vehicles are too close.
    virtual void detectCollisions(SUMOTime timestep);


    /** Returns the information whether this lane may be used to continue
        the current route */
    virtual bool appropriate(const MSVehicle *veh);


    /// returns the container with all links !!!
    const MSLinkCont &getLinkCont() const;


    /// Returns true if there is not a single vehicle on the lane.
    bool empty() const {
        assert(myVehBuffer.size()==0);
        return myVehicles.empty();
    }

    void setMaxSpeed(SUMOReal val) throw() {
        myMaxSpeed = val;
    }

    void setLength(SUMOReal val) throw() {
        myLength = val;
    }


    /** @brief Returns the lane's edge
     * @return This lane's edge
     */
    MSEdge &getEdge() const throw() {
        return *myEdge;
    }

    /** @brief Inserts a MSLane into the static dictionary
        Returns true if the key id isn't already in the dictionary.
        Otherwise returns false. */
    static bool dictionary(std::string id, MSLane* lane);

    /** @brief Returns the MSEdgeControl associated to the key id if exists
       Otherwise returns 0. */
    static MSLane* dictionary(std::string id);

    /** Clears the dictionary */
    static void clear();

    static size_t dictSize() {
        return myDict.size();
    }

    static void insertIDs(std::vector<std::string> &into) throw();

    /// Container for vehicles.
    typedef std::deque< MSVehicle* > VehCont;

    /** Same as succLink, but does not throw any assertions when
        the succeeding link could not be found;
        Returns the myLinks.end() instead; Further, the number of edges to
        look forward may be given */
    virtual MSLinkCont::const_iterator succLinkSec(const SUMOVehicle& veh,
            unsigned int nRouteSuccs,
            const MSLane& succLinkSource,
            const std::vector<MSLane*> &conts) const;


    /** Returns the information whether the given link shows at the end
        of the list of links (is not valid) */
    bool isLinkEnd(MSLinkCont::const_iterator &i) const;

    /** Returns the information whether the given link shows at the end
        of the list of links (is not valid) */
    bool isLinkEnd(MSLinkCont::iterator &i);

    /// returns the last vehicle
    virtual MSVehicle * const getLastVehicle() const;
    virtual const MSVehicle * const getFirstVehicle() const;

    void init(MSEdgeControl &, std::vector<MSLane*>::const_iterator firstNeigh, std::vector<MSLane*>::const_iterator lastNeigh);



    // valid for gui-version only
    virtual GUILaneWrapper *buildLaneWrapper(GUIGlObjectStorage &idStorage);

    virtual MSVehicle *removeFirstVehicle();
    virtual MSVehicle *removeVehicle(MSVehicle *remVehicle);

    /// The shape of the lane
    Position2DVector myShape;



    void leftByLaneChange(MSVehicle *v);
    void enteredByLaneChange(MSVehicle *v);


    MSLane * const getLeftLane() const;
    MSLane * const getRightLane() const;

    void setAllowedClasses(const std::vector<SUMOVehicleClass> &classes) throw() {
        myAllowedClasses = classes;
    }


    void setNotAllowedClasses(const std::vector<SUMOVehicleClass> &classes) throw() {
        myNotAllowedClasses = classes;
    }


    bool allowsVehicleClass(SUMOVehicleClass vclass) const;

    void addIncomingLane(MSLane *lane, MSLink *viaLink);

    struct IncomingLaneInfo {
        MSLane *lane;
        SUMOReal length;
        MSLink *viaLink;
    };

    const std::vector<IncomingLaneInfo> &getIncomingLanes() const {
        return myIncomingLanes;
    }

    std::pair<MSVehicle * const, SUMOReal> getFollowerOnConsecutive(SUMOReal dist, SUMOReal seen,
            SUMOReal leaderSpeed, SUMOReal backOffset) const;

    std::pair<MSVehicle * const, SUMOReal> getLeaderOnConsecutive(SUMOReal dist, SUMOReal seen,
            SUMOReal leaderSpeed, const MSVehicle &veh, const std::vector<MSLane*> &bestLaneConts) const;


    /// @name Current state retrieval
    //@{

    /** @brief Returns the mean speed on this lane
     * @return The average speed of vehicles during the last step; default speed if no vehicle was on this lane
     */
    SUMOReal getMeanSpeed() const throw();


    /** @brief Returns the occupancy of this lane during the last step
     * @return The occupancy during the last step
     */
    SUMOReal getOccupancy() const throw();


    /** @brief Returns the sum of lengths of vehicles which were on the lane during the last step
     * @return The sum of vehicle lengths of vehicles in the last step
     */
    SUMOReal getVehLenSum() const throw();


    /** @brief Returns the sum of last step CO2 emissions
     * @return CO2 emissions of vehicles on this lane during the last step
     */
    SUMOReal getHBEFA_CO2Emissions() const throw();


    /** @brief Returns the sum of last step CO emissions
     * @return CO emissions of vehicles on this lane during the last step
     */
    SUMOReal getHBEFA_COEmissions() const throw();


    /** @brief Returns the sum of last step PMx emissions
     * @return PMx emissions of vehicles on this lane during the last step
     */
    SUMOReal getHBEFA_PMxEmissions() const throw();


    /** @brief Returns the sum of last step NOx emissions
     * @return NOx emissions of vehicles on this lane during the last step
     */
    SUMOReal getHBEFA_NOxEmissions() const throw();


    /** @brief Returns the sum of last step HC emissions
     * @return HC emissions of vehicles on this lane during the last step
     */
    SUMOReal getHBEFA_HCEmissions() const throw();


    /** @brief Returns the sum of last step fuel consumption
     * @return fuel consumption of vehicles on this lane during the last step
     */
    SUMOReal getHBEFA_FuelConsumption() const throw();


    /** @brief Returns the sum of last step noise emissions
     * @return noise emissions of vehicles on this lane during the last step
     */
    SUMOReal getHarmonoise_NoiseEmissions() const throw();
    /// @}


protected:
    /** @brief Insert a vehicle into the lane's vehicle buffer.
        After processing done from moveCritical, when a vehicle exits it's lane.
        Returned is the information whether the vehicle was removed. */
    virtual bool push(MSVehicle* veh);

    /** Returns the first/front vehicle of the lane and removing it from the lane. */
    virtual MSVehicle* pop(SUMOTime t);

    /// moves myTmpVehicles int myVehicles after a lane change procedure
    virtual void swapAfterLaneChange(SUMOTime t);



protected:
    /// Unique ID.
    std::string myID;

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

    /// The lane's edge, for routing only.
    MSEdge* myEdge;

    /// Lane-wide speedlimit [m/s]
    SUMOReal myMaxSpeed;

    /** Container for lane-changing vehicles. After completion of lane-change-
        process, the two containers will be swapped. */
    VehCont myTmpVehicles;


    SUMOReal myBackDistance;

    /** Vehicle-buffer for vehicle that was put onto this lane by a
        junction. The  buffer is necessary, because of competing
        push- and pop-operations on myVehicles during
        Junction::moveFirst() */
    std::vector<MSVehicle*> myVehBuffer;

    /// The list of allowed vehicle classes
    std::vector<SUMOVehicleClass> myAllowedClasses;

    /// The list of disallowed vehicle classes
    std::vector<SUMOVehicleClass> myNotAllowedClasses;

    std::vector<IncomingLaneInfo> myIncomingLanes;

    /// @brief The current length of all vehicles on this lane
    SUMOReal myVehicleLengthSum;

    /// @brief End position of a vehicle which laps into this lane
    SUMOReal myInlappingVehicleEnd;

    /// @brief The vehicle which laps into this lane
    MSVehicle *myInlappingVehicle;


    /// @brief The lane left to the described lane (==lastNeigh if none)
    std::vector<MSLane*>::const_iterator myFirstNeigh;

    /// @brief The end of this lane's edge's lane container
    std::vector<MSLane*>::const_iterator myLastNeigh;

    /// @brief Not yet seen vehicle lengths
    SUMOReal myLeftVehLength;

    /** The lane's Links to it's succeeding lanes and the default
        right-of-way rule, i.e. blocked or not blocked. */
    MSLinkCont myLinks;

    /// definition of the tatic dictionary type
    typedef std::map< std::string, MSLane* > DictType;

    /// Static dictionary to associate string-ids with objects.
    static DictType myDict;

private:
    /// @brief This lane's move reminder
    MoveReminderCont myMoveReminders;


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
        int operator()(MSVehicle *v1, MSVehicle *v2) const {
            return v1->getPositionOnLane()>v2->getPositionOnLane();
        }

    };


private:
    /// @brief invalidated copy constructor
    MSLane(const MSLane&);

    /// @brief invalidated assignment operator
    MSLane& operator=(const MSLane&);


};


#endif

/****************************************************************************/

