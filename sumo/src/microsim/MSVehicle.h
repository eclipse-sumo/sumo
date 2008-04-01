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
#include <utils/common/SUMOAbstractRouter.h>

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
class MSDevice;


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
    virtual ~MSVehicle() throw();


    /// @name needed during the emission
    //@{

    /// returns the edge the vehicle starts from
    const MSEdge &departEdge();

    /// Returns the lanes the vehicle may be emitted onto
    const MSEdge::LaneCont& departLanes();

    /// Returns the desired departure time.
    SUMOTime desiredDepart() const;
    //@}

    void removeOnTripEnd(MSVehicle *veh) throw();


    /// @name interaction with the route
    //@{

    /** Returns the nSuccs'th successor of the vehicles current
        route-edge or 0 if there is no nSucc'th edge. */
    const MSEdge* succEdge(unsigned int nSuccs) const;

    /** Returns the current edge as iterator of the vehicles route vector */
    const MSRouteIterator& currEdgeIt() const;

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


    bool willPass(const MSEdge * const edge) const;

    void reroute(SUMOTime t, SUMOAbstractRouter<MSEdge, MSVehicle> &router);

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


    SUMOReal getPositionOnActiveMoveReminderLane(const MSLane * const searchedLane) const;

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


    /// Returns the name of the vehicle
    const std::string &getID() const;

    /** Return true if the lane is allowed */
    bool onAllowed(const MSLane* lane) const;

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


    /** @brief Update when the vehicle enters a new lane in the move step.
     *
     * @param[in] enteredLane The lane the vehicle enters
     * @param[in] driven The distance driven by the vehicle within this time step
     */
    void enterLaneAtMove(MSLane* enteredLane, SUMOReal driven);



    /** @brief Update when the vehicle enters a new lane in the emit step
     *
     * @param[in] enteredLane The lane the vehicle enters
     * @param[in] state The vehicle's state during the emission
     */
    void enterLaneAtEmit(MSLane* enteredLane, const State &state);


    /** @brief Update when the vehicle enters a new lane in the laneChange step.
     *
     * @param[in] enteredLane The lane the vehicle enters
     */
    void enterLaneAtLaneChange(MSLane* enteredLane);


    /** @brief Update when the vehicle leaves a new lane in the move step.
     *
     * @param[in] driven The distance driven by the vehicle within this time step
     */
    void leaveLaneAtMove(SUMOReal driven);


    /** @brief Update of members if vehicle leaves a new lane in the lane change step. */
    void leaveLaneAtLaneChange();


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


    void rebuildAllowedLanes(bool reinit=true);
    const std::vector<MSLane*> &getBestLanesContinuation() const;

    void quitRemindedEntered(MSVehicleQuitReminded *r);
    void quitRemindedLeft(MSVehicleQuitReminded *r);


    MSAbstractLaneChangeModel &getLaneChangeModel();
    const MSAbstractLaneChangeModel &getLaneChangeModel() const;
    typedef std::deque<const MSEdge::LaneCont*> NextAllowedLanes;

    std::string buildDeviceIDList() const;

    struct LaneQ {
        MSLane *lane;
        SUMOReal length;
        SUMOReal alllength;
        SUMOReal v;
        SUMOReal hindernisPos;
        SUMOReal wish;
        int dir;
        bool t1;
        std::vector<MSLane*> joined;

        /*
        MSLane *lane;
        std::vector<LaneQ> follower;
        bool allowed2;
        SUMOReal laneLength2;
        SUMOReal seenLength2;
        SUMOReal seenVehicles2;
        SUMOReal leftDistance2;
        int dir2;
        */
    };

    virtual const std::vector<LaneQ> &getBestLanes() const;

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

    /** @brief Called when the vehicle leaves the lane */
    void onTripEnd();
    void writeXMLRoute(OutputDevice &os, int index=-1) const;

    struct Stop {
        MSLane *lane;
        MSBusStop *busstop;
        SUMOReal pos;
        SUMOTime duration;
        SUMOTime until;
        bool reached;
#ifdef TRACI
		bool isTraciStop;
		Stop(): isTraciStop(false) {};
#endif
    };

    std::list<Stop> myStops;

    void addStop(const Stop &stop) {
        myStops.push_back(stop);
    }

    bool hasStops() {
        return !myStops.empty();
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

    SUMOReal getEffort(const MSEdge * const e, SUMOReal t) const;


    const std::vector<MSDevice*> &getDevices() const {
        return myDevices;
    }

	/**
	 * Compute distance that will be covered, if the vehicle moves to a given position on its route,
	 * starting at its current position.
	 * @param destPos:	position on the destination edge that shall be reached
	 * @param destEdge: destination edge that shall be reached
	 * @return			distance from the vehicles current position to the destination position,
	 *					or a near infinite real value if the destination position is not contained 
	 *					within the vehicles route or the vehicle is not active
	 */
	SUMOReal getDistanceToPosition(SUMOReal destPos, const MSEdge* destEdge);

#ifdef TRACI
	/**
	 * schedule a new stop for the vehicle; each time a stop is reached, the vehicle
	 * will wait for the given duration before continuing on its route
	 * @param lane		lane on wich to stop
	 * @param pos		position on the given lane at wich to stop
	 * @param radius	the vehicle will stop if it is within the range [pos-radius, pos+radius]
	 * @duration		after waiting for the time period duration, the vehicle will
	 *					continue until the stop is reached again
	 */
	void addTraciStop(MSLane* lane, SUMOReal pos, SUMOReal radius, SUMOTime duration);

	/**
	 * Add a stop to the head of the vehicle's stop list to make it the next stop position.
	 * If a continuous array of stops which are located at the same edge exists at the list's 
	 * head, the new stop is sorted within the array by increasing position.
	 */
	void sortTraCIStopToStopList(Stop traciStop);

	/** 
	 * Comparison function for vehicle stops. 
	 * Stops are sorted by their positions, starting with the lowest position.
	 */
	//bool compareStop(const Stop& left, const Stop& right);

	

    void checkReroute(SUMOTime t);

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

    /**
     * The vehicle will slow down to newSpeed within the time interval duration.
     * This is done by limiting the maximum speed every time a simulation step
     * is performed by TraCI. Speed reduction is linear.
     * @param newSpeed speed to reduce to
     * @param duration time intervall for the speed adaption
     * @param currentTime current simulation time
     */
    bool startSpeedAdaption(float newSpeed, SUMOTime duration, SUMOTime currentTime);

    /**
     * called each simulation step to slow down if necessary (see above)
     */
    void adaptSpeed();

	/**
	 * Checks if the sticky time for a "changelane" command has passed already
	 * @param time current simulation time
	 */
	void checkLaneChangeConstraint(SUMOTime time);

	/**
	 * After each changed lane, check if the lane requested by TraCI command "changeLane"
	 * is reached and request for more changes if necessary.
	 */
	void checkForLaneChanges();

	/**
	 * Initiate a lane change requested by TraCI command "changeLane".
	 * @param lane	the lane index within the current edge, that is the destination of the change
	 * @param stickyTime	duration for wich the constraint takes effect
	 */ 
	void startLaneChange(int lane, SUMOTime stickyTime);

	/**
	 * Forces the vehicle to change the given number of lanes to the right side
	 * @param numLanes number of lanes that shall be passed
	 * @param stickyTime duration for wich the lane change constraint takes effect
	 */
	//void forceLaneChangeRight(int numLanes, SUMOTime stickyTime);

	/**
	 * Forces the vehicle to change the given number of lanes to the left side
	 * @param numLanes number of lanes that shall be passed
	 * @param stickyTime duration for wich the lane change constraint takes effect
	 */
	//void forceLaneChangeLeft(int numLanes, SUMOTime stickyTime);

	/**
	 * takes all action necessary during a simulation step to process any active command sent by TraCI 
	 * @param time the current simulation time
	 */
	void processTraCICommands(SUMOTime time);
#endif

protected:
    void rebuildContinuationsFor(LaneQ &q, MSLane *l, MSRouteIterator ce, int seen) const;
    virtual void setBlinkerInformation() { }


    /// Use this constructor only.
    MSVehicle(std::string id, MSRoute* route, SUMOTime departTime,
              const MSVehicleType* type,
              int repNo, int repOffset, int vehicleIndex);

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

    /// The lane the vehicle is on
    MSLane* myLane;

    MSAbstractLaneChangeModel *myLaneChangeModel;

    const MSVehicleType * const myType;

    mutable const MSEdge *myLastBestLanesEdge;
    mutable std::vector<std::vector<LaneQ> > myBestLanes;
    mutable std::vector<LaneQ>::iterator myCurrentLaneInBestLanes;
    static std::vector<MSLane*> myEmptyLaneVector;

    std::map<MSCORN::Pointer, void*> myPointerCORNMap;
    std::map<MSCORN::Function, int> myIntCORNMap;

private:
    std::vector<MSDevice*> myDevices;

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

    struct DepartArrivalInformation {
        SUMOTime time;
        MSLane *lane;
        SUMOReal pos;
        SUMOReal speed;
    };


#ifdef TRACI
	//struct TraciStop {
	//	MSLane *lane;
	//	MSEdge* edge;
	//	SUMOReal pos;
	//	SUMOReal radius;
	//	SUMOTime duration;
	//	SUMOTime remainingTime;
	//	bool reached;

	//	/* stops are sorted by their positions */
	//	bool operator<(TraciStop arg) {
	//		return ( (pos-radius) < (arg.pos - arg.radius) );
	//	}
	//};

	typedef std::list<Stop> TraciStopList;
	/* list of stops scheduled by TraCI*/
	std::map<std::string, TraciStopList> myTraciStops;

	/* iterator pointing to the closest, not yet passed stop on the current lane, if any*/
	//TraciStopList::iterator nextTraciStop;

    bool myHaveRouteInfo;
    typedef std::map<const MSEdge * const, Information *> InfoCont;
    InfoCont infoCont;

    /**
     * if true, indicates that a TraCI message "changeRoute" was sent to this vehicle,
     * thus it checks for a new route when the next simulation step is performed by TraCI
     */
    bool myWeightChangedViaTraci;

    /**
     * all edges in this list are marked as "changed by TracI", this means infoCont data
     * of this edge must not be changed except by a TraCI "changeRoute" message.
     * The corresponding Information value of the map holds the Information data
     * related to an edge before it was changed by TraCI
     */
    InfoCont edgesChangedByTraci;

    /* indicates whether the vehicle is adapting its speed caused by the TraCI command slowDown*/
    bool adaptingSpeed;
    bool isLastAdaption;

	/* speed of the vehicle before any speed adaption began */
    SUMOReal speedBeforeAdaption;

	/* the amount by wich the speed shall be reduced */
    SUMOReal speedReduction;

	/* simulation time, when the last speed adaption started */
    SUMOTime timeBeforeAdaption;

	/* duration of the last speed adaption */
    SUMOTime adaptDuration;

	/* simulation time when the last lane change was forced */
	SUMOTime timeBeforeLaneChange;

	/* duration for which the last lane change will be in effect */
	SUMOTime laneChangeStickyTime;

	/* lane index of the destination road map position for an active lane change*/
	int destinationLane;
	
	/* true if any forced lane change is in effect*/
	bool laneChangeConstraintActive;

#endif

};


#endif

/****************************************************************************/

