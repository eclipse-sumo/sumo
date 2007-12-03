/****************************************************************************/
/// @file    MSVehicle.h
/// @author  Christian Roessel
/// @date    Mon, 12 Mar 2001
/// @version $Id$
///
// micro-simulation Vehicles.
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
#ifndef MSVehicle_h
#define MSVehicle_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSEdge.h"
#include "MSNet.h"
#include "MSRoute.h"
#include "MSUnit.h"
#include "MSCORN.h"
#include <list>
#include <deque>
#include <map>
#include <string>
#include <vector>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include "MSVehicleQuitReminded.h"
#include <utils/common/SUMOVehicleClass.h>
#include "MSVehicleType.h"

#ifdef HAVE_MESOSIM
#include <mesosim/MEVehicle.h>
#endif


// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;
class MSMoveReminder;
class MSLaneChanger;
class MSVehicleTransfer;
class MSAbstractLaneChangeModel;
class MSBusStop;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSVehicle
 */
class MSVehicle : public MSVehicleQuitReminded
#ifdef HAVE_MESOSIM
            , public MEVehicle
#endif
{
public:

    /// the lane changer sets myLastLaneChangeOffset
    friend class MSLaneChanger;

    /** container that holds the vehicles driving state. May vary from
        model to model. here: SK, holds position and speed. */
    class State
    {
        /// vehicle sets states directly
        friend class MSVehicle;
        friend class MSLaneChanger;

    public:
        /// Constructor.
        State(SUMOReal pos, SUMOReal speed);

        /// Default constructor. Members are initialized to 0.
        //State();

        /// Copy constructor.
        State(const State& state);

        /// Assignment operator.
        State& operator=(const State& state);

        /// Operator !=
        bool operator!=(const State& state);

        /// Position of this state.
        SUMOReal pos() const;

        /// Speed of this state
        SUMOReal speed() const {
            return mySpeed;
        };

    private:
        /// the stored position
        SUMOReal myPos;

        /// the stored speed
        SUMOReal mySpeed;

    };

    /// Destructor.
    virtual ~MSVehicle();


    /// @name needed during the emission
    //@{
    /// returns the edge the vehicle starts from
    const MSEdge &departEdge();

    /// Returns the lanes the vehicle may be emitted onto
    const MSEdge::LaneCont& departLanes();

    /// Returns the desired departure time.
    SUMOTime desiredDepart() const;
    //@}

    // add a new Vehicle
    void addVehNeighbors(MSVehicle *veh, SUMOTime time);

    /// update the list of neighbors
    void cleanUpConnections(SUMOTime time);

    /// compute the distance between two equipped vehicle
    bool computeDistance(MSVehicle* veh1, MSVehicle* veh2);

    /** C2C: update own information
     * a) insert the current edge if the vehicle is standing for a long period
     * b) remove information older then a specified amount of time (MSGlobals::gLANRefuseOldInfosOffset)
     */
    void updateInfos(SUMOTime time);

    // set the Id of the Cluster to them the vehicle belong
    void setClusterId(int Id);

    // get the Id of the Cluster
    int getClusterId(void) const;

    void sendInfos(SUMOTime time);

    // Build a Cluster for the WLAN simulation
    /* A Cluster is a set of vehicles, where all vehicles or the Neighbor of this Vehicle
     * other the Neighbor of the Neighbor of this Vehicle.
     * A Vehicle cannot belong to two different cluster
     */
    int buildMyCluster(int myStep, int clId);

    // set the Actual sender in my Cluster
    //void setActSender(MSVehicle* actS);

    void removeOnTripEnd(MSVehicle *veh);


    /// @name interaction with the route
    //@{
    /** Returns the nSuccs'th successor of the vehicles current
        route-edge or 0 if there is no nSucc'th edge. */
    const MSEdge* succEdge(unsigned int nSuccs) const;

    /** Returns true if nSuccs'th successor of the vehicles current
        route-edge exists. */
    bool hasSuccEdge(unsigned int nSuccs) const;

    /** Returns true if vehicle is going to enter it's destination
        edge. False otherwise. Adjusts in both cases the route
        iterator and the allowedLanes-container. */
    bool destReached(const MSEdge* targetEdge);

    /// Returns the information whether the route ends on the given lane
    bool endsOn(const MSLane &lane) const;

    /// Moves vehicle one edge forward, returns true if the route has ended
    bool proceedVirtualReturnWhetherEnded(const MSEdge *const to);

    /// Returns the current route
    const MSRoute &getRoute() const;

    /// Returns the current or a prior route
    const MSRoute &getRoute(int index) const;

    /// Replaces the current route by the given edges
    bool replaceRoute(const MSEdgeVector &edges, SUMOTime simTime);

    /// Replaces the current route by the given route
    bool replaceRoute(MSRoute *r, SUMOTime simTime);
    //@}


    /// moves the vehicles after their responds (right-of-way rules) are known
    void moveFirstChecked();

    /** @brief An position assertion
        (needed as state and lane are not accessable) */
    void _assertPos() const;


    /** Returns the gap between pred and this vehicle. Assumes they
     * are on parallel lanes. Requires a positive gap. */
    SUMOReal gap2pred(const MSVehicle& pred) const {
        SUMOReal gap = pred.getPositionOnLane() - pred.getLength() - getPositionOnLane();
        if (gap<0&&gap>-1.0e-12) {
            gap = 0;
        }
        if (gap<0) {
            WRITE_WARNING("Vehicle " + getID() + " collides with pred in " + toString(MSNet::getInstance()->getCurrentTimeStep()));
            gap = 0;
        }
        assert(gap >= SUMOReal(0));
        return gap;
    }

    static inline SUMOReal gap(SUMOReal predPos, SUMOReal predLength, SUMOReal pos) {
        return predPos - predLength - pos;
    }




    /// @name retrieval and setting of CORN values
    //@{
    /// Gets the named CORN double value
    int getCORNIntValue(MSCORN::Function f) const;

    /// Gets the named CORN pointer value
    void *getCORNPointerValue(MSCORN::Pointer p) const;

    /// Returns whether this vehicle has the named value stored
    bool hasCORNIntValue(MSCORN::Function f) const;

    /// Returns whether this vehicle has the named value stored
    bool hasCORNPointerValue(MSCORN::Pointer p) const;
    //@}


    /** moves a vehicle if it is not meant to be running out of the lane
        If there is no neigh, pass 0 to neigh.
        If neigh is on curr lane, pass 0 to gap2neigh,
        otherwise gap.
        Updates drive parameters. */
    void move(MSLane* lane, const MSVehicle* pred, const MSVehicle* neigh);

    /** Moves vehicles which may run out of the lane
        Same semantics as move */
    void moveRegardingCritical(MSLane* lane, const MSVehicle* pred, const MSVehicle* neigh);


    /// @name state setter/getter
    //@{
    /// Returns the vehicle state
    const State &getState() const {
        return myState;
    }

    /// Get the vehicle's position.
    SUMOReal getPositionOnLane() const {
        return myState.myPos;
    }

    /// Returns current speed
    SUMOReal getSpeed() const {
        return myState.mySpeed;
    }
    //@}

    SUMOReal getLength() const {
        return myType->getLength();
    }

    SUMOReal interactionGap(SUMOReal vF, SUMOReal laneMaxSpeed, SUMOReal vL) const {
        return myType->interactionGap(vF, laneMaxSpeed, vL);
    }

    bool hasSafeGap(SUMOReal speed, SUMOReal gap, SUMOReal predSpeed, SUMOReal laneMaxSpeed) const {
        return myType->hasSafeGap(speed, gap, predSpeed, laneMaxSpeed);
    }

    SUMOReal getSecureGap(SUMOReal speed, SUMOReal predSpeed, SUMOReal predLength) const {
        return myType->getSecureGap(speed, predSpeed, predLength);
    }

    SUMOReal ffeV(SUMOReal speed, SUMOReal gap2pred, SUMOReal predSpeed) const {
        return myType->ffeV(speed, gap2pred, predSpeed);
    }

    SUMOReal decelAbility() const {
        return myType->decelAbility(); // !!! really the speed?
    }

    SUMOReal maxNextSpeed(SUMOReal v) const {
        return myType->maxNextSpeed(v);
    }

    SUMOReal getSpeedAfterMaxDecel(SUMOReal v) const {
        return myType->getSpeedAfterMaxDecel(v);
    }

    SUMOReal getMaxSpeed() const {
        if (isIndividualMaxSpeedSet())
            return getIndividualMaxSpeed();

        return myType->getMaxSpeed();
    }

    void setIndividualMaxSpeed(SUMOReal individualMaxSpeed) {
        myIsIndividualMaxSpeedSet = true;
        myIndividualMaxSpeed = individualMaxSpeed;
    }

    void unsetIndividualMaxSpeed(void) {
        myIsIndividualMaxSpeedSet = false;
        myIndividualMaxSpeed = 0.0;
    }

    SUMOReal getIndividualMaxSpeed(void) const {
        return myIndividualMaxSpeed;
    }

    bool isIndividualMaxSpeedSet(void) const {
        return myIsIndividualMaxSpeedSet;
    }

    /** */
    SUMOReal timeHeadWayGap(SUMOReal speed) const {
        assert(speed >= 0);
        return SPEED2DIST(speed);
    }


#ifdef HAVE_BOYOM_C2C
    /// return true if the vehicle is eqquiped with WLAN
    bool isEquipped() const;
#endif


    /// Returns the name of the vehicle
    const std::string &getID() const;

    /** Return true if the lane is allowed */
    bool onAllowed(const MSLane* lane) const;

    /** Return true if the lane is allowed */
    //bool onAllowed( const MSLane* lane, size_t offset ) const;

    /** Return true if vehicle is on an allowed lane. */
    bool onAllowed() const;


    /** Returns true if the two vehicles overlap. */
    static bool overlap(const MSVehicle* veh1, const MSVehicle* veh2) {
        if (veh1->myState.myPos < veh2->myState.myPos) {
            return veh2->myState.myPos - veh2->getLength() < veh1->myState.myPos;
        }
        return veh1->myState.myPos - veh1->getLength() < veh2->myState.myPos;
    }


    /** Returns true if vehicle's speed is below 60km/h. This is only relevant
        on highways. Overtaking on the right is allowed then. */
    bool congested() const {
        return myState.mySpeed < SUMOReal(60) / SUMOReal(3.6);
    }

    /// Return current Position
    Position2D getPosition() const;


    /// @name state io
    //@{
    /// Saves the states of a vehicle
    void saveState(std::ostream &os);
    //@}


    const MSEdge * const getEdge() const;


    /** Update of members if vehicle enters a new lane in the move step.
        @param Pointer to the entered Lane. */
    void enterLaneAtMove(MSLane* enteredLane, SUMOReal driven,
                         bool inBetweenJump=false);

    /** Update of members if vehicle enters a new lane in the emit step.
        @param Pointer to the entered Lane. */
    void enterLaneAtEmit(MSLane* enteredLane, const State &state);

    /** Update of members if vehicle enters a new lane in the laneChange step.
        @param Pointer to the entered Lane. */
    void enterLaneAtLaneChange(MSLane* enteredLane);

    /** Update of members if vehicle leaves a new lane in the move step. */
    void leaveLaneAtMove(SUMOReal driven);

    /** Update of members if vehicle leaves a new lane in the
        laneChange step. */
    void leaveLaneAtLaneChange(void);

    bool reachingCritical(SUMOReal laneLength) const;

    friend class MSVehicleControl;

    void vsafeCriticalCont(SUMOReal minVSafe);

    MSLane *getTargetLane() const;

    /// Returns the lane the vehicle is on
    const MSLane &getLane() const;

    /// Returns whether the vehicle is driving, what means myLane is not null.
    bool getInTransit() const;

    /// @name usage of multiple vehicle emissions
    //@{
    /// Returns the information whether further vehicles of this type shall be emitted periodically
    bool periodical() const;

    /// Returns the information whether the vehicle was aready emitted
    bool running() const;

    /** @brief Returns the next "periodical" vehicle with the same route
        We have to duplicate the vehicle if a further has to be emitted with
        the same settings */
    virtual MSVehicle *getNextPeriodical() const;
    //@}


    /** @brief returns the number of steps waited
        A vehicle is meant to be "waiting" when it's speed is less than 0.1
        It is only computed for "critical" vehicles
        The method return a size_t, now, as we assume a vehicle will not wait for
        longer than about 50 hours which still fits into a size_t when the simulation
        runs in ms */
    size_t getWaitingTime() const;


    void rebuildAllowedLanes();

    void quitRemindedEntered(MSVehicleQuitReminded *r);
    void quitRemindedLeft(MSVehicleQuitReminded *r);


    MSAbstractLaneChangeModel &getLaneChangeModel();
    const MSAbstractLaneChangeModel &getLaneChangeModel() const;
    typedef std::deque<const MSEdge::LaneCont*> NextAllowedLanes;
    const NextAllowedLanes &getAllowedLanes(MSLaneChanger &lc);

    struct LaneQ {
        MSLane *lane;
        float length;
        float alllength;
        float v;
        float hindernisPos;
        float wish;
        int dir;
        bool t1;
        std::vector<MSLane*> joined;
    };

    const std::vector<std::vector<LaneQ> > &getBestLanes() const;

    SUMOReal getMovedDistance(void) const {
        return SPEED2DIST(myState.mySpeed);
    }

    const MSVehicleType &getVehicleType() const;

    enum MoveOnReminderMode {
        BOTH, CURRENT
    };

    void workOnMoveReminders(SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed,
                             MoveOnReminderMode = BOTH);

    void onDepart();

    void onTripEnd(/*MSLane &caller, */bool wasAlreadySet=false);
    void writeXMLRoute(OutputDevice &os, int index=-1) const;

    struct Stop {
        MSLane *lane;
        MSBusStop *busstop;
        SUMOReal pos;
        SUMOTime duration;
        SUMOTime until;
        bool reached;
    };

    std::list<Stop> myStops;

    void addStop(const Stop &stop) {
        myStops.push_back(stop);
    }

    SUMOVehicleClass getVehicleClass() const {
        return myType->getVehicleClass();
    }
    bool knowsEdgeTest(MSEdge &edge) const;

    class Information
    {
    public:
        Information(SUMOReal neededTime_, SUMOTime time_)
                : neededTime(neededTime_), time(time_) { }
        SUMOReal neededTime; // how long needed the vehicle to travel on the edge
        SUMOTime time; // the Time, when the Info was saved
    };

#ifdef HAVE_BOYOM_C2C
    // enumeration for all type of Connection
    enum C2CConnectionState {
        dialing, connected, sending, receiving, disconnected
    };

    // structure for Car2Car Connection
    struct C2CConnection {
        MSVehicle  *connectedVeh;
        C2CConnectionState state;
        SUMOTime lastTimeSeen;
    };
    typedef std::map<MSVehicle * const, C2CConnection *> VehCont;

    const VehCont &getConnections() const;

    SUMOReal getC2CEffort(const MSEdge * const e, SUMOTime t) const;
    void checkReroute(SUMOTime t);

    int getTotalInformationNumber() const {
        return totalNrOfSavedInfos;
    }
    bool hasRouteInformation() const {
        return myHaveRouteInfo;
    }
    SUMOTime getLastInfoTime() const {
        return myLastInfoTime;
    }
    size_t getConnectionsNumber() const {
        return clusterCont.size();
    }
    size_t getInformationNumber() const {
        return infoCont.size();
    }

    size_t getNoGot() const;
    size_t getNoSent() const;
    size_t getNoGotRelevant() const;
#endif

    /*
        SUMOTime getSendingTimeEnd() const;
        bool maySend() const;
        void send(SUMOTime time);
    */
#ifdef TRACI

	/**
	 * Used by TraCIServer to change the weight of an edge locally for a specific vehicle
	 * @param edgeID: ID of the edge to change
	 * @param travelTime: the new time to be set for the edge
	 * @param currentTime: the current simulation time
	 */
	bool changeEdgeWeightLocally(std::string edgeID, double travelTime, SUMOTime currentTime);

	/**
	 * Used by TraCIServer to restore the weight of an edge that was changed previously 
	 * via "changeRoute" command
	 * @param edgeID: ID of the edge to restore
	 */
	bool restoreEdgeWeightLocally(std::string edgeID, SUMOTime currentTime);
#endif

protected:
    /// Use this constructor only.
    MSVehicle(std::string id, MSRoute* route, SUMOTime departTime,
              const MSVehicleType* type,
              int repNo, int repOffset, int vehicleIndex);

    void initDevices(int vehicleIndex);

    /// information how long ago the vehicle has performed a lane-change
    SUMOTime myLastLaneChangeOffset;

    /// the lane, the vehicle will be within the next time step
    MSLane *myTarget;

    /** @brief The time the vehicle waits
        This is the number of simulation steps the vehicle was not faster than 0.1m/s
        It's a size_t, now, as we assume a vehicle will not wait for
        longer than about 50 hours which still fits into a size_t when the simulation
        runs in ms */
    size_t myWaitingTime;

    // The time the vehicle waits, may mean the same like myWaitingTime
    int timeSinceStop;

    /// The number of cars that shall be emitted with the same settings
    int myRepetitionNumber;

    /// The period of time to wait between emissions of cars with the same settings
    int myPeriod;

    /// Unique ID.
    std::string myID;

    /// Vehicle's route.
    MSRoute* myRoute;

    /// Desired departure time (seconds).
    SUMOTime myDesiredDepart;

    /// Vehicles driving state. here: pos and speed
    State myState;

    /// An individual speed for an vehicle that is used (iff set) instead of
    /// the maximal speed of the vehicle class.
    /// NOTE: This is just a little workaround for allowing an external
    ///       influence on the actual speed
    SUMOReal myIndividualMaxSpeed;

    /// is true if there has an individual speed been set
    bool myIsIndividualMaxSpeedSet;

#ifdef HAVE_BOYOM_C2C
    /// is true, if the vehicle is abble to send Informations to another vehicle
    bool equipped;

    // for save the time
    int lastUp;

    // the Id of the Cluster
    int clusterId;

    //recent information
    //is saved when the vehicle leaves the lane!!!
    Information *akt;
    SUMOTime myLastInfoTime;
    bool myHaveRouteInfo;

    // count how much Informations this vehicle have saved during the simulation
    int totalNrOfSavedInfos;
#endif

    /// The lane the vehicle is on
    MSLane* myLane;

    MSAbstractLaneChangeModel *myLaneChangeModel;

    const MSVehicleType * const myType;

#if defined(HAVE_BOYOM_C2C) || defined(TRACI)
    typedef std::map<const MSEdge * const, Information *> InfoCont;
    typedef std::vector<C2CConnection*> ClusterCont;
#endif

#ifdef HAVE_BOYOM_C2C
    VehCont myNeighbors;

    typedef std::vector<C2CConnection*> ClusterCont;
    ClusterCont clusterCont;
#endif

    bool willPass(const MSEdge * const edge) const;

#ifdef HAVE_BOYOM_C2C
    // transfert the N Information in infos into my own InformationsContainer
    void transferInformation(const std::string &senderID, const InfoCont &infos, int N,
                             SUMOTime currentTime);

    //compute accordant the distance, the Number of Infos that can be transmit
    size_t numOfInfos(MSVehicle *veh1, MSVehicle* veh2);
#endif


    mutable const MSEdge *myLastBestLanesEdge;
    mutable std::vector<std::vector<LaneQ> > myBestLanes;

    std::map<MSCORN::Pointer, void*> myPointerCORNMap;

private:

    /** Iterator to current route-edge.  */
    MSRouteIterator myCurrEdge;

    /** The vehicle's allowed lanes on it'S current edge to drive
        according to it's route. */
    NextAllowedLanes myAllowedLanes;


    /// Default constructor.
    MSVehicle();

    /// Copy constructor.
    MSVehicle(const MSVehicle&);

    /// Assignment operator.
    MSVehicle& operator=(const MSVehicle&);

    struct DriveProcessItem {
        MSLink *myLink;
        SUMOReal myVLinkPass;
        SUMOReal myVLinkWait;
        DriveProcessItem(MSLink *link, SUMOReal vPass, SUMOReal vWait) :
                myLink(link), myVLinkPass(vPass), myVLinkWait(vWait) { };
    };

    typedef std::vector< DriveProcessItem > DriveItemVector;

    /// Container for used Links/visited Lanes during lookForward.
    DriveItemVector myLFLinkLanes;

    typedef std::vector< MSMoveReminder* > MoveReminderCont;
    typedef MoveReminderCont::iterator MoveReminderContIt;
    MoveReminderCont myMoveReminders;
    MoveReminderCont myOldLaneMoveReminders;
    typedef std::vector<SUMOReal> OffsetVector;
    OffsetVector myOldLaneMoveReminderOffsets;

    void activateRemindersByEmitOrLaneChange(void);

    typedef std::vector<MSVehicleQuitReminded*> QuitRemindedVector;
    QuitRemindedVector myQuitReminded;

    std::map<MSCORN::Function, int> myIntCORNMap;


#ifdef HAVE_BOYOM_C2C
    size_t myNoGot, myNoSent, myNoGotRelevant;
#endif

    /**
     * @class RouteReplaceInfo
     * @brief Information about a replaced route
     *
     * Generated optionally and stored in a vector within the Pointer-CORN-map
     *  this structure contains information about a replaced route: the edge
     *  the route was replaced at by a new one, the time this was done, and
     *  the previous route.
     */
    class RouteReplaceInfo
    {
    public:
        /// Constructor
        RouteReplaceInfo(const MSEdge * const edge_, SUMOTime time_, MSRoute *route_)
                : edge(edge_), time(time_), route(route_) {}

        /// Destructor
        ~RouteReplaceInfo() { }

        /// The edge the vehicle was on when the route was replaced
        const MSEdge *edge;

        /// The time the route was replaced
        SUMOTime time;

        /// The prior route
        MSRoute *route;

    };

    /// Definition of the vector which stores information about replaced routes
    typedef std::vector<RouteReplaceInfo> ReplacedRoutesVector;

#ifdef TRACI
	/** 
	 * if true, indicates that a TraCI message "changeRoute" was sent to this vehicle,
	 * thus it checks for a new route when the next simulation step is performed
	 */
	bool myWeightChangedViaTraci;

	/**
	 * all edges in this list are marked as "changed by TracI", this means infoCont data
	 * of this edge must not be changed except by a TraCI "changeRoute" message.
	 * The corresponding Information value of the map holds the Information data
	 * related to an edge before it was changed by TraCI
	 */
	InfoCont edgesChangedByTraci;
#endif

};


#endif

/****************************************************************************/

