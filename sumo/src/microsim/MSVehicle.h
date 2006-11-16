#ifndef MSVehicle_H
#define MSVehicle_H
/***************************************************************************
                          MSVehicle.h  -  Base for all
                          micro-simulation Vehicles.
                             -------------------
    begin                : Mon, 12 Mar 2001
    copyright            : (C) 2001 by ZAIK http://www.zaik.uni-koeln.de/AFS
    author               : Christian Roessel
    email                : roessel@zpr.uni-koeln.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// $Log$
// Revision 1.64  2006/11/16 07:02:17  dkrajzew
// warnings removed
//
// Revision 1.63  2006/11/14 13:02:12  dkrajzew
// warnings removed
//
// Revision 1.62  2006/11/14 06:46:07  dkrajzew
// lane change speed-up; first steps towards car2car-based rerouting
//
// Revision 1.61  2006/11/02 11:44:50  dkrajzew
// added Danilo Teta-Boyom's changes to car2car-communication
//
// Revision 1.60  2006/10/12 08:09:41  dkrajzew
// fastened up lane changing; added current car2car-code
//
// Revision 1.59  2006/09/21 09:47:06  dkrajzew
// debugging vehicle movement; insertion of Danilot's current sources
//
// Revision 1.58  2006/09/18 10:09:04  dkrajzew
// c2c added; vehicle classes added
//
// Revision 1.57  2006/08/01 07:00:32  dkrajzew
// removed unneeded API parts
//
// Revision 1.56  2006/07/07 11:51:52  dkrajzew
// further work on lane changing
//
// Revision 1.55  2006/07/06 07:33:22  dkrajzew
// rertrieval-methods have the "get" prependix; EmitControl has no dictionary; MSVehicle is completely scheduled by MSVehicleControl; new lanechanging algorithm
//
// Revision 1.54  2006/05/15 05:50:40  dkrajzew
// began with the extraction of the car-following-model from MSVehicle
//
// Revision 1.54  2006/05/08 11:09:36  dkrajzew
// began with the extraction of the car-following-model from MSVehicle
//
// Revision 1.53  2006/04/18 08:13:52  dkrajzew
// debugging rerouting
//
// Revision 1.52  2006/04/05 05:27:34  dkrajzew
// retrieval of microsim ids is now also done using getID() instead of id()
//
// Revision 1.51  2006/03/28 06:20:43  dkrajzew
// removed the unneded slow lane changer
//
// Revision 1.50  2006/03/17 09:01:12  dkrajzew
// .icc-files removed
//
// Revision 1.49  2006/02/27 12:08:15  dkrajzew
// raknet-support added
//
// Revision 1.48  2006/01/09 11:53:00  dkrajzew
// bus stops implemented
//
// Revision 1.47  2005/10/07 11:37:45  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.46  2005/09/23 13:16:40  dkrajzew
// debugging the building process
//
// Revision 1.45  2005/09/22 13:45:51  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.44  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.43  2005/07/12 12:06:12  dkrajzew
// first devices (mobile phones) added
//
// Revision 1.42  2005/05/04 08:34:20  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added; new mead data functionality; lane-changing offset computation debugged; simulation speed-up by avoiding multiplication with 1
//
// Revision 1.41  2005/02/17 10:33:38  dkrajzew
// code beautifying;
// Linux building patched;
// warnings removed;
// new configuration usage within guisim
//
// Revision 1.40  2005/02/01 10:10:42  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.39  2004/12/16 12:24:45  dkrajzew
// debugging
//
// Revision 1.38  2004/11/23 10:20:11  dkrajzew
// new detectors and tls usage applied; debugging
//
// Revision 1.37  2004/08/02 12:40:55  dkrajzew
// debugging; refactoring; lane-changing API
//
// Revision 1.36  2004/07/02 09:57:37  dkrajzew
// handling of routes added
//
// Revision 1.35  2004/04/02 11:36:28  dkrajzew
// "compute or not"-structure added; added two further simulation-wide output
//  (emission-stats and single vehicle trip-infos)
//
// Revision 1.34  2004/03/19 13:09:40  dkrajzew
// debugging
//
// Revision 1.33  2004/02/16 15:21:58  dkrajzew
// movedDistance-retrival reworked; forgetting predecessors when driving over
//  more than one lane patched
//
// Revision 1.32  2004/02/05 16:37:51  dkrajzew
// e3-debugging: only e3-detectors have to remove killed vehicles; storage
//  for detectors to be informed added
//
// Revision 1.31  2004/01/26 15:55:55  dkrajzew
// the vehicle is now informed about being emitted (as we want to display the
//  information about the real departure time witin the gui - within microsim,
//  this information may be used for some other stuff)
//
// Revision 1.30  2003/12/12 12:37:42  dkrajzew
// proper usage of lane states applied; scheduling of vehicles into the beamer
//  on push failures added
//
// Revision 1.29  2003/12/11 06:31:45  dkrajzew
// implemented MSVehicleControl as the instance responsible for vehicles
//
// Revision 1.28  2003/12/04 13:30:41  dkrajzew
// work on internal lanes
//
// Revision 1.27  2003/11/24 10:22:57  dkrajzew
// patched the false usage of oldLaneMoveReminders when more than one street
//  is within
//
// Revision 1.26  2003/11/20 14:59:17  dkrajzew
// detector usage patched
//
// Revision 1.25  2003/10/24 16:48:37  roessel
// Added new method getMovedDistance and corresponding member.
//
// Revision 1.24  2003/10/22 07:06:04  dkrajzew
// patching of lane states on force vehicle removal added
//
// Revision 1.23  2003/10/20 07:59:43  dkrajzew
// grid lock dissolving by vehicle teleportation added
//
// Revision 1.22  2003/10/15 11:43:50  dkrajzew
// false lane-changing rules removed; an (far too large information interface
//  between vehicle and lane-changer implemented
//
// Revision 1.21  2003/09/05 15:14:52  dkrajzew
// first steps for reading of internal lanes
//
// Revision 1.20  2003/08/20 11:44:11  dkrajzew
// min and max-functions moved to an own definition file
//
// Revision 1.19  2003/07/30 09:18:42  dkrajzew
// the waiting time is now an unsigned int
//
// Revision 1.18  2003/06/19 10:58:34  dkrajzew
// too conservative computation of the braking gap patched
//
// Revision 1.17  2003/06/05 10:19:45  roessel
// Added previous lane reminder-container and workOnMoveReminders().
//
// Revision 1.16  2003/05/25 17:52:29  roessel
// Added typedefs.
//
// Revision 1.15  2003/05/22 12:46:36  roessel
// New method activateRemindersByEmitOrLaneChange.
//
// Revision 1.14  2003/05/21 16:20:45  dkrajzew
// further work detectors
//
// Revision 1.13  2003/05/20 09:31:46  dkrajzew
// emission debugged; movement model reimplemented (seems ok); detector output
// debugged; setting and retrieval of some parameter added
//
// Revision 1.12  2003/04/16 10:05:07  dkrajzew
// uah, debugging
//
// Revision 1.11  2003/04/14 08:33:04  dkrajzew
// some further bugs removed
//
// Revision 1.10  2003/04/10 15:43:43  dkrajzew
// emission on non-source lanes debugged
//
// Revision 1.9  2003/04/07 12:12:39  dkrajzew
// eps reduced for small segments (dawdle2)
//
// Revision 1.8  2003/04/04 15:36:30  roessel
// Moved static DictType myDict from private to public
//
// Revision 1.7  2003/03/20 16:21:13  dkrajzew
// windows eol removed; multiple vehicle emission added
//
// Revision 1.6  2003/03/12 16:50:57  dkrajzew
// lane retrival added for centering a vehicle on the display
//
// Revision 1.5  2003/02/07 10:41:51  dkrajzew
// updated
//
// Revision 1.4  2002/10/28 12:59:38  dkrajzew
// vehicles are now deleted whe the tour is over
//
// Revision 1.3  2002/10/21 09:55:40  dkrajzew
// begin of the implementation of multireferenced, dynamically loadable routes
//
// Revision 1.2  2002/10/16 16:45:42  dkrajzew
// debugged
//
// Revision 1.1  2002/10/16 14:48:26  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.9  2002/09/25 17:14:42  roessel
// MeanData calculation and output implemented.
//
// Revision 1.8  2002/07/31 17:33:01  roessel
// Changes since sourceforge cvs request.
//
// Revision 1.10  2002/07/24 16:32:52  croessel
// New method speed() for MSVehicle::State.
// timeHeadWayGap public now.
// New methods isInsertTimeHeadWayCond() and isInsertBrakeCond() to check
// the Krauss conditions during emit/laneChange.
//
// Revision 1.9  2002/07/23 16:42:34  croessel
// New friend class MSTriggeredSource.
//
// Revision 1.8  2002/07/03 15:58:52  croessel
// New methods isSafeChange, hasSafeGap, safeEmitGap instead of safeGap.
// Removed safeLaneChangeGap, not used.
// New method vNeighEqualPos for "don't overtake on the right".
// Signature of move() changed.
//
// Revision 1.7  2002/06/20 13:44:58  dkrajzew
// safeGap is now using fabs
//
// Revision 1.6  2002/06/19 15:07:54  croessel
// Added method timeHeadWayGap( speed ) to forbid timeheadway < deltaT states.
//
// Revision 1.5  2002/05/29 17:06:04  croessel
// Inlined some methods. See the .icc files.
//
// Revision 1.4  2002/05/17 12:46:09  croessel
// Added new method laneChangeBrake2much() because of lane-changers
// disability to look beyond lanes.
// Added some debug ifdefs.
//
// Revision 1.3  2002/04/11 15:25:56  croessel
// Changed SUMOReal to SUMOReal.
//
// Revision 1.2  2002/04/10 16:29:08  croessel
// Made speed() public.
//
// Revision 1.1.1.1  2002/04/08 07:21:23  traffic
// new project name
//
// Revision 2.4  2002/03/20 16:02:23  croessel
// Initialisation and update of allowedLanes-container fixed.
//
// Revision 2.3  2002/03/20 11:11:09  croessel
// Splitted friend from class-declaration.
//
// Revision 2.2  2002/03/13 16:56:35  croessel
// Changed the simpleOutput to XMLOutput by introducing nested classes
// XMLOut. Output is now indented.
//
// Revision 2.1  2002/02/27 13:14:05  croessel
// Prefixed ostream with "std::".
//
// Revision 2.0  2002/02/14 14:43:20  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.18  2002/02/05 13:51:53  croessel
// GPL-Notice included.
// In *.cpp files also config.h included.
//
// Revision 1.17  2002/02/05 11:52:11  croessel
// Changed method vAccel() to accelState().
// Introduced new mezhods for lane-change:
// State::advantage()
// onAllowed()
// overlap()
// congested()
//
// Revision 1.16  2002/02/01 11:24:13  croessel
// Added min/max method due to MSVC++ inabilities.
//
// Revision 1.15  2002/01/24 18:29:43  croessel
// New and modified gap-methods: interactionDist renamed to brakeGap,
// interactionGap introduced and calculation of gaps fixed.
//
// Revision 1.14  2002/01/16 10:06:22  croessel
// Comment added.
//
// Revision 1.13  2002/01/09 15:07:39  croessel
// Vehicle counter added. MSVehicle::howMany() knows how many vehicles are
// alive.
//
// Revision 1.12  2001/12/20 14:49:30  croessel
// using namespace std replaced by std::
// Moved id() definition to .cpp
//
// Revision 1.11  2001/12/13 14:11:18  croessel
// Moved State definitions to cpp-file.
//
// Revision 1.10  2001/12/06 13:16:41  traffic
// speed computation added (use -D _SPEEDCHECK)
//
// Revision 1.9  2001/11/21 15:25:13  croessel
// Numerous changes during debug session.
//
// Revision 1.8  2001/11/15 17:17:36  croessel
// Outcommented the inclusion of the inline *.iC files. Currently not needed.
// Vehicle-State introduced. Simulation is now independant of vehicle's
// speed. Still, the position is fundamental, also a gap between
// vehicles. But that's it.
//
// Revision 1.7  2001/11/14 11:45:55  croessel
// Resolved conflicts which appeared during suffix-change and
// CR-line-end commits.
//
// Revision 1.6  2001/11/14 10:49:08  croessel
// CR-line-end removed.
//
// Revision 1.5  2001/10/23 09:31:47  traffic
// parser bugs removed
//
// Revision 1.3  2001/09/06 15:35:50  croessel
// Added operator<< to class MSVehicle for simple text output and minor
// changes.
//
// Revision 1.2  2001/07/16 12:55:47  croessel
// Changed id type from unsigned int to string. Added string-pointer
// dictionaries and dictionary methods.
//
// Revision 1.1.1.1  2001/07/11 15:51:13  traffic
// new start
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

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

#ifdef RAKNET_DEMO
#include <raknet_demo/vehicle.h>
#endif


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSLane;
class MSMoveReminder;
class MSLaneChanger;
class MSVehicleTransfer;
class MSAbstractLaneChangeModel;
class MSBusStop;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSVehicle
 */
class MSVehicle : public MSVehicleQuitReminded
#ifdef RAKNET_DEMO
    , public Vehicle
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
        State( SUMOReal pos, SUMOReal speed );

        /// Default constructor. Members are initialized to 0.
        //State();

        /// Copy constructor.
        State( const State& state );

        /// Assignment operator.
        State& operator=( const State& state );

        /// Operator !=
        bool operator!=( const State& state );

        /// Position of this state.
        SUMOReal pos() const;

        /// Speed of this state
        SUMOReal speed() const { return mySpeed; };

    private:
        /// the stored position
        SUMOReal myPos;

        /// the stored speed
        SUMOReal mySpeed;

    };

    /// Destructor.
    virtual ~MSVehicle();


    //@{ needed during the emission
    /// returns the edge the vehicle starts from
    const MSEdge &departEdge();

    /// Returns the lanes the vehicle may be emitted onto
    const MSEdge::LaneCont& departLanes();

    /// Returns the desired departure time.
    SUMOTime desiredDepart() const;
    //@}

    // add a new Vehicle
	void addVehNeighbors(MSVehicle *veh, int time);

	    // delete all vehicle in the list
	void removeAllVehNeighbors(void);

	/// update the list of neighbors each timestep
	//void updateNeighbors(int time);
	void cleanUpConnections(int time);

	/// compute the distance between two equipped vehicle
	bool computeDistance(MSVehicle* veh1, MSVehicle* veh2);

	// erase the Information, if the information is older than 30 minutes
	void updateInfos(int time);

	// set the Id of the Cluster to them the vehicle belong
	void setClusterId(int Id);

	// get the Id of the Cluster
	int getClusterId(void);

	void sendInfos(SUMOTime time);

	// Build a Cluster for the WLAN simulation
	/* A Cluster is a set of Vehicle, where all vehicle is or the Neighbor of this Vehicle
	 * other the Neighbor of the Neighbor of this Vehicle.
	 * A Vehicle cannot belong to two different cluster
	 */
	int buildMyCluster(int myStep, int clId);

	// set the Actual sender in my Cluster
	//void setActSender(MSVehicle* actS);

    void removeOnTripEnd( MSVehicle *veh );


    //@{ interaction with the route
    /** Returns the nSuccs'th successor of the vehicles current
        route-edge or 0 if there is no nSucc'th edge. */
    const MSEdge* succEdge( unsigned int nSuccs ) const;

    /** Returns true if nSuccs'th successor of the vehicles current
        route-edge exists. */
    bool hasSuccEdge( unsigned int nSuccs ) const;

    /** Returns true if vehicle is going to enter it's destination
        edge. False otherwise. Adjusts in both cases the route
        iterator and the allowedLanes-container. */
    bool destReached( const MSEdge* targetEdge );

    /// Returns the information whether the route ends on the given lane
    bool endsOn(const MSLane &lane) const;

    /// Moves vehicle one edge forward, returns true if the route has ended
    bool proceedVirtualReturnWhetherEnded(const MSEdge *const to);

    /// Returns the current route
    const MSRoute &getRoute() const;

    /// Returns the current or a prior route
    const MSRoute &getRoute(int index) const;

    /// Replaces the current route by the given edges
    bool replaceRoute(const MSEdgeVector &edges, size_t simTime);

    /// Replaces the current route by the given route
    bool replaceRoute(MSRoute *r, size_t simTime);
    //@}


    /// moves the vehicles after their responds (right-of-way rules) are known
    void moveFirstChecked();

    /** @brief An position assertion
        (needed as state and lane are not accessable) */
    void _assertPos() const;


    /** Returns the gap between pred and this vehicle. Assumes they
     * are on parallel lanes. Requires a positive gap. */
    SUMOReal gap2pred( const MSVehicle& pred ) const {
        SUMOReal gap = pred.getPositionOnLane() - pred.getLength() - getPositionOnLane();
        if(gap<0&&gap>-1.0e-12) {
            gap = 0;
        }
        if(gap<0) {
            WRITE_WARNING("Vehicle " + getID() + " collides with pred in " + toString(MSNet::getInstance()->getCurrentTimeStep()));
            gap = 0;
        }
        assert( gap >= SUMOReal( 0 ) );
        return gap;
    }

    static inline SUMOReal gap(SUMOReal predPos, SUMOReal predLength, SUMOReal pos) {
        return predPos - predLength - pos;
    }




	void interactWith(const std::vector<MSVehicle*> &vehicles);

    //@{ Retrieval and setting of CORN values
    /// Gets the named CORN double value
    SUMOReal getCORNDoubleValue(MSCORN::Function f) const;

    /// Gets the named CORN pointer value
    void *getCORNPointerValue(MSCORN::Pointer p) const;

    /// Returns whether this vehicle has the named value stored
    bool hasCORNDoubleValue(MSCORN::Function f) const;
    //@}


    /** moves a vehicle if it is not meant to be running out of the lane
        If there is no neigh, pass 0 to neigh.
        If neigh is on curr lane, pass 0 to gap2neigh,
        otherwise gap.
        Updates drive parameters. */
    void move( MSLane* lane, const MSVehicle* pred, const MSVehicle* neigh );

    /** Moves vehicles which may run out of the lane
        Same semantics as move */
    void moveRegardingCritical( MSLane* lane, const MSVehicle* pred, const MSVehicle* neigh );


    //@{ State setter/getter
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

    SUMOReal interactionGap( SUMOReal vF, SUMOReal laneMaxSpeed, SUMOReal vL ) const {
        return myType->interactionGap( vF, laneMaxSpeed, vL );
    }

    bool hasSafeGap(  SUMOReal speed, SUMOReal gap, SUMOReal predSpeed, SUMOReal laneMaxSpeed) const {
        return myType->hasSafeGap(speed, gap, predSpeed, laneMaxSpeed);
    }

    SUMOReal getSecureGap( SUMOReal speed, SUMOReal predSpeed, SUMOReal predLength ) const {
        return myType->getSecureGap(speed, predSpeed, predLength );
    }

    SUMOReal ffeV(SUMOReal speed, SUMOReal gap2pred, SUMOReal predSpeed) const {
        return myType->ffeV(speed, gap2pred, predSpeed);
    }

    SUMOReal decelAbility() const {
        return myType->decelAbility(); // !!! really the speed?
    }

    SUMOReal maxNextSpeed( SUMOReal v ) const {
        return myType->maxNextSpeed(v);
    }

    SUMOReal getSpeedAfterMaxDecel(SUMOReal v) const {
        return myType->getSpeedAfterMaxDecel(v);
    }

    SUMOReal getMaxSpeed() const {
        return myType->getMaxSpeed();
    }

    /** */
    SUMOReal timeHeadWayGap(SUMOReal speed) const {
        assert( speed >= 0 );
        return SPEED2DIST(speed);
    }


    /// return true if the vehicle is eqquiped with WLAN
    bool isEquipped() const;


    /// Returns the name of the vehicle
    const std::string &getID() const;

    /** Return true if the lane is allowed */
    bool onAllowed( const MSLane* lane ) const;

    /** Return true if the lane is allowed */
    //bool onAllowed( const MSLane* lane, size_t offset ) const;

    /** Return true if vehicle is on an allowed lane. */
    bool onAllowed( ) const;


    /** Returns true if the two vehicles overlap. */
    static bool overlap( const MSVehicle* veh1, const MSVehicle* veh2 ) {
        if ( veh1->myState.myPos < veh2->myState.myPos ) {
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


    //@{ State IO
    /// Saves the states of a vehicle
    void saveState(std::ostream &os, long what);
    //@}


    const MSEdge * const getEdge() const;


    /** Update of members if vehicle enters a new lane in the move step.
        @param Pointer to the entered Lane. */
    void enterLaneAtMove( MSLane* enteredLane, SUMOReal driven );

    /** Update of members if vehicle enters a new lane in the emit step.
        @param Pointer to the entered Lane. */
    void enterLaneAtEmit( MSLane* enteredLane, const State &state );

    /** Update of members if vehicle enters a new lane in the laneChange step.
        @param Pointer to the entered Lane. */
    void enterLaneAtLaneChange( MSLane* enteredLane );

    /** Update of members if vehicle leaves a new lane in the move step. */
    void leaveLaneAtMove( SUMOReal driven );

    /** Update of members if vehicle leaves a new lane in the
        laneChange step. */
    void leaveLaneAtLaneChange( void );

    bool reachingCritical(SUMOReal laneLength) const;

    friend class MSVehicleControl;

    void vsafeCriticalCont( SUMOReal minVSafe );

    MSLane *getTargetLane() const;

    /// Returns the lane the vehicle is on
    const MSLane &getLane() const;


    //@{ Usage of multiple vehicle emissions
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

    void removeApproachingInformationOnKill();

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

    SUMOReal getMovedDistance( void ) const
        {
        return SPEED2DIST(myState.mySpeed);
        }

    const MSVehicleType &getVehicleType() const;

    void setCORNColor(SUMOReal red, SUMOReal green, SUMOReal blue);

    enum MoveOnReminderMode {BOTH, CURRENT};

    void workOnMoveReminders( SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed,
                              MoveOnReminderMode = BOTH);

    void onDepart();

    void onTripEnd(/*MSLane &caller, */bool wasAlreadySet=false);
	void writeXMLRoute(std::ostream &os, int index=-1) const;

    struct Stop {
        MSLane *lane;
        MSBusStop *busstop;
        SUMOReal pos;
        SUMOTime duration;
        bool reached;
    };

    std::list<Stop> myStops;

    void addStop(const Stop &stop) {
        myStops.push_back(stop);
    }

    SUMOVehicleClass getVehicleClass() const { return myType->getVehicleClass(); }
    bool knowsEdgeTest(MSEdge &edge) const;


	struct Information {
		std::string infoTyp;
		std::string edge;
		SUMOReal neededTime; // how long needed the vehicle to travel on the edge
		int time; // the Time, when the Info was saved
	};

	// enumeration for all type of Connection
	enum C2CConnectionState {dialing, connected, sending, receiving, disconnected};

	// structure for Car2Car Connection
	struct C2CConnection {
		MSVehicle  *connectedVeh;
		C2CConnectionState state;
		int timeSinceSeen;
		int timeSinceConnect;
		int lastTimeSeen;
		std::vector<Information *> toSend;
		std::vector<Information *> transmitted;
		std::vector<Information *>::iterator nextToSend;
	};
	typedef std::map<std::string, C2CConnection *> VehCont;

    const VehCont &getConnections() const;

    SUMOReal getC2CEffort(const MSEdge * const e, SUMOTime t) const;

/*
    SUMOTime getSendingTimeEnd() const;
    bool maySend() const;
    void send(SUMOTime time);
*/

protected:
    /// Use this constructor only.
    MSVehicle( std::string id, MSRoute* route, SUMOTime departTime,
        const MSVehicleType* type,
        int repNo, int repOffset);

     void initDevices();

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

	/// is true, if the vehicle is abble to send Informations to another vehicle
	bool equipped;

	// for save the time
	int lastUp;

	// the Id of the Cluster
	int clusterId;

	//Aktuelle Information
	//wird erst gespiechert, wenn vehicle die Lane verlässt !!!!!!!!!
	Information *akt;

	// a Pointer on the actual sender in the Cluster
	//MSVehicle* actSender;

    /// The lane the vehicle is on
    MSLane* myLane;

    MSAbstractLaneChangeModel *myLaneChangeModel;

    const MSVehicleType * const myType;


	VehCont myNeighbors;

	typedef std::map<std::string, Information *> InfoCont;
	InfoCont infoCont;

	typedef std::vector<C2CConnection*> ClusterCont;
	ClusterCont clusterCont;

    // transfert the N Information in infos into my own InformationsContainer
    void transferInformation(std::string senderID, InfoCont infos, int N);

	//compute accordant the distance, the Number of Infos that can be transmit
	int numOfInfos(MSVehicle *veh1, MSVehicle* veh2);

	//InfoCont getInfosToSend(void);

    mutable const MSEdge *myLastBestLanesEdge;
    mutable std::vector<std::vector<LaneQ> > myBestLanes;

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

    struct DriveProcessItem
    {
        MSLink *myLink;
        SUMOReal myVLinkPass;
        SUMOReal myVLinkWait;
        DriveProcessItem( MSLink *link, SUMOReal vPass, SUMOReal vWait  ) :
            myLink( link ), myVLinkPass(vPass), myVLinkWait(vWait) { };
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

    void activateRemindersByEmitOrLaneChange( void );

    typedef std::vector<MSVehicleQuitReminded*> QuitRemindedVector;
    QuitRemindedVector myQuitReminded;

    std::map<MSCORN::Function, SUMOReal> myDoubleCORNMap;
	std::map<MSCORN::Pointer, void*> myPointerCORNMap;

};



/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
