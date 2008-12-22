/****************************************************************************/
/// @file    MSLane.h
/// @author  Christian Roessel
/// @date    Mon, 12 Mar 2001
/// @version $Id$
///
// Representation of a lane in the micro simulation
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
class SSLaneMeanData;


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
class MSLane
{
public:
    /// needs access to myTmpVehicles (this maybe should be done via SUMOReal-buffering!!!)
    friend class MSLaneChanger;

    /// needs direct access to maxSpeed
    friend class MSLaneSpeedTrigger;

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
    ///@}



    /// @name Vehicle movement (longitudinal)
    /// @{

    virtual bool moveNonCritical();

    virtual bool moveCritical();

    /** Moves the critical vehicles
        This step is done after the responds have been set */
    virtual bool setCritical(std::vector<MSLane*> &into);

    /// Insert buffered vehicle into the real lane.
    virtual bool integrateNewVehicle();
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

    /// Returns the lane's maximum speed.
    SUMOReal maxSpeed() const {
        assert(myMaxSpeed>=0);
        return myMaxSpeed;
    }

    /// Returns the lane's length.
    SUMOReal length() const {
        return myLength;
    }

    /// Returns the lane's Edge.
    const MSEdge * const getEdge() const {
        return myEdge;
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

    /// Container for vehicles.
    typedef std::deque< MSVehicle* > VehCont;

    /** Same as succLink, but does not throw any assertions when
        the succeeding link could not be found;
        Returns the myLinks.end() instead; Further, the number of edges to
        look forward may be given */
    virtual MSLinkCont::const_iterator succLinkSec(const MSVehicle& veh,
            unsigned int nRouteSuccs,
            const MSLane& succLinkSource,
            const std::vector<MSLane*> &conts) const;


    /** Returns the information whether the given link shows at the end
        of the list of links (is not valid) */
    bool isLinkEnd(MSLinkCont::const_iterator &i) const;

    /** Returns the information whether the given link shows at the end
        of the list of links (is not valid) */
    bool isLinkEnd(MSLinkCont::iterator &i);

    /** @brief Returns the information whether the given edge is the parent edge
     * @param[in] edge The edge to test
     * @return Whether this lane is a part of the given edge
     */
    inline bool inEdge(const MSEdge * const edge) const throw() {
        return myEdge==edge;
    }

    /// returns the last vehicle
    virtual MSVehicle * const getLastVehicle() const;
    virtual const MSVehicle * const getFirstVehicle() const;

    MSVehicle::State myLastState;


    void init(MSEdgeControl &, MSEdge::LaneCont::const_iterator firstNeigh, MSEdge::LaneCont::const_iterator lastNeigh);


    /** does nothing; needed for GUI-versions where vehicles are locked
        when being displayed */
    virtual void releaseVehicles();

    /// returns the vehicles
    virtual const VehCont &getVehiclesSecure();

    unsigned int getVehicleNumber() const;



    // valid for gui-version only
    virtual GUILaneWrapper *buildLaneWrapper(GUIGlObjectStorage &idStorage);

    virtual MSVehicle *removeFirstVehicle();
    virtual MSVehicle *removeVehicle(MSVehicle *remVehicle);

    size_t getNumericalID() const;


    SUMOReal getMeanSpeed() const;

    const std::string &getID() const;

    /// The shape of the lane
    Position2DVector myShape;

    const Position2DVector &getShape() const {
        return myShape;
    }

    SUMOReal getDensity() const;
    SUMOReal getVehLenSum() const;


    void leftByLaneChange(MSVehicle *v);
    void enteredByLaneChange(MSVehicle *v);


    MSLane * const getLeftLane() const;
    MSLane * const getRightLane() const;

    const std::vector<SUMOVehicleClass> &getAllowedClasses() const;
    const std::vector<SUMOVehicleClass> &getNotAllowedClasses() const;
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
            SUMOReal leaderSpeed) const;

    std::pair<MSVehicle * const, SUMOReal> getLeaderOnConsecutive(SUMOReal dist, SUMOReal seen,
            SUMOReal leaderSpeed, const MSVehicle &veh, const std::vector<MSLane*> &bestLaneConts) const;



protected:
    /** @brief Function Object for use with Function Adapter on vehicle containers.
        Returns the information whether the position of the first vehicle
        is greater than the one of the second vehicle */
    class PosGreater
    {
    public:
        /// the first vehicle
        typedef const MSVehicle* first_argument_type;
        /// the second vehicle
        typedef const MSVehicle* second_argument_type;
        /// returns bool
        typedef bool result_type;

        /** Returns true if position of first vehicle is greater
            then positition of the second one. */
        result_type operator()(first_argument_type veh1,
                               second_argument_type veh2) const;
    };


    /** @brief Insert a vehicle into the lane's vehicle buffer.
        After processing done from moveCritical, when a vehicle exits it's lane.
        Returned is the information whether the vehicle was removed. */
    virtual bool push(MSVehicle* veh);

    /** Returns the first/front vehicle of the lane and removing it from the lane. */
    virtual MSVehicle* pop();

    /// moves myTmpVehicles int myVehicles after a lane change procedure
    virtual void swapAfterLaneChange();



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


    /// @brief The lane left to the described lane (==lastNeigh if none)
    MSEdge::LaneCont::const_iterator myFirstNeigh;

    /// @brief The end of this lane's edge's lane container
    MSEdge::LaneCont::const_iterator myLastNeigh;

    /// index of the first vehicle that may drive over the lane's end
    size_t myFirstUnsafe;

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
    class vehicle_position_sorter
    {
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

