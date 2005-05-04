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
// "compute or not"-structure added; added two further simulation-wide output (emission-stats and single vehicle trip-infos)
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
// Changed float to double.
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
#include "MSEdge.h"
#include "MSNet.h"
#include "MSRoute.h"
#include "MSUnit.h"
#include "MSCORN.h"
#include <helpers/Counter.h>
#include <map>
#include <string>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSLane;
class MSVehicleType;
class MSMoveReminder;
class MSLaneChanger;
class MSVehicleTransfer;
class MSVehicleQuitReminded;
class MSAbstractLaneChangeModel;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSVehicle
 * A single vehicle. Holds model-methods, io-methods and methods which compute
 * standard physical values such as the gap needed to stop.
 */
class MSVehicle
{
public:

    /// the lane changer sets myLastLaneChangeOffset
    friend class MSLaneChanger;
    friend class MSSlowLaneChanger;

    /** container that holds the vehicles driving state. May vary from
        model to model. here: SK, holds position and speed. */
    class State
    {
        /// vehicle sets states directly
        friend class MSVehicle;
        friend class MSLaneChanger;
        friend class MSSlowLaneChanger;

    public:
        /// Default constructor. Members are initialized to 0.
        State();

        /// Copy constructor.
        State( const State& state );

        /// Assignment operator.
        State& operator=( const State& state );

        /// Operator !=
        bool operator!=( const State& state );

        /// Position of this state.
        double pos() const;

        /// Set position of this state.
        void setPos( double pos );

        /// Return true if vehicle would prefer preferState.
        static bool advantage( const State& preferState,
                               const State& compareState );

        /// Speed of this state
        double speed() const { return mySpeed; };

        /// Constructor.
        State( double pos, double speed );

    private:
        /// the stored position
        double myPos;

        /// the stored speed
        double mySpeed;

    };



    /// Sort criterion for vehiles is the departure time.
//    friend bool departTimeSortCrit( const MSVehicle* x, const MSVehicle* y );

    /// Destructor.
    virtual ~MSVehicle();

    /// Returns the vehicles current state.
    State state() const;

    /// Returns the lanes the vehicle may be emitted onto
    const MSEdge::LaneCont& departLanes();

    /// returns the edge the vehicle starts from
    MSEdge &departEdge();

    /// moves the vehicles after their responds (right-of-way rules) are known
    void moveFirstChecked();

    /// Returns the desired departure time.
    SUMOTime desiredDepart() const;

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

    /** In "gap2predecessor < brakeGap" region interaction between
        vehicle and predecessor with speed == 0 takes place. If
        vehicle has no predecessor, it has to search within brakeGap
        for collision-free driving. */
    double brakeGap( const MSLane* lane ) const;

    double brakeGap( double speed ) const;

    /// minimum brake gap
    double rigorousBrakeGap(const double &state) const;

    /** @brief An position assertion
        (needed as state and lane are not accessable) */
    void _assertPos() const;

    /** In "gap2predecessor < interactionGap" region interaction between
        vehicle and predecessor with speed != 0 takes place. Else vehicle
        drives freely. */
    double interactionGap( const MSLane* lane, const MSVehicle& pred ) const;

    /** Checks if this vehicle and pred will be in a safe state if one
     * changes to the others lane. */
    bool isSafeChange( const MSVehicle& pred, const MSLane* lane ) const;


    bool isSafeChange_WithDistance( double dist,
        const MSVehicle& pred, const MSLane* lane ) const;

    double getSecureGap( const MSLane &lane,
        const MSVehicle &pred ) const;

    /** Checks if the gap between this vehicle and pred is sufficient
     * for safe driving. */
    bool hasSafeGap( const MSVehicle& pred,
                     const MSLane* lane,
                     double gap ) const;

    bool hasSafeGap(  double gap, double predSpeed, const MSLane* lane) const;

    /** Returns the minimum gap between this driving vehicle and a
     * possibly emitted vehicle with speed 0. */
    double safeEmitGap( void ) const;


    /** Returns the velocity that is neccessary to match the positions
     * of the two vehicles assuming that the neighbour will keep his
     * speed. Because overtaking on the right is forbidden, this
     * vehicle is not allowed to drive faster than vNeighEqualPos. */
    double vNeighEqualPos( const MSVehicle& neigh );

    /** Returns the gap between pred and this vehicle. Assumes they
     * are on parallel lanes. Requires a positive gap. */
    double gap2pred( const MSVehicle& pred ) const;

    /** Returns the gap between pred and this vehicle. Assumes they
     * are on parallel lanes. Requires a positive gap. */
    double gap2predSec( const MSVehicle& pred ) const;

    /** Returns the vehicels driving distance during one timestep when
        driving with speed. */
    double driveDist( State state ) const;

    /** Returns the distance-difference between driving at constant
        speed and maximum braking in one timestep. */
    double decelDist() const;

    // -----------------------------



    void interactWith(const std::vector<MSVehicle*> &vehicles);
    // -----------------------------

    /// Return the vehicles state after maximum acceleration.
    State accelState( const MSLane* lane ) const;

    /// The amount the vehicle can decelerate with
    double decelAbility() const;

    /// The amount the vehicle can accelerate with
    double accelAbility() const;

    /// The amount the vehicle can accelerate with
    double accelDist() const;


    double getCORNDoubleValue(MSCORN::Function f);
    bool hasCORNDoubleValue(MSCORN::Function f);


    /** moves a vehicle if it is not meant to be running out of the lane
        If there is no neigh, pass 0 to neigh.
        If neigh is on curr lane, pass 0 to gap2neigh,
        otherwise gap.
        Updates drive parameters. */
    void move( MSLane* lane,
               const MSVehicle* pred,
               const MSVehicle* neigh );

    /** Moves vehicles which may run out of the lane
        Same semantics as move */
    void moveRegardingCritical( MSLane* lane,
        const MSVehicle* pred, const MSVehicle* neigh );

    /// Slow down towards lane end. Updates state. For first vehicles only.
//    void moveDecel2laneEnd( MSLane* lane );

    /// Use this move for first vehicles that won't leave it's lane.
//    void moveUpdateState( const State newState );

    /// Use this move for first vehicles that will leave it's lane.
    void moveSetState( const State newState );

    // Slow down to one's lane end, don't respect neighbours. Lane-end
    // need not to be the lane-end of the current lane.
//    State nextState( MSLane* lane, double gap ) const;

    // Use this form if pred would give the wrong position, e.g. if you
    // want to know what might will happen if this vehicle would have
    // a new pred.
/*    State nextStateCompete( MSLane* lane,
                            State predState,
                            double gap2pred ) const;
*/

///////////////////////////////////////////////////////////////////////////
    /// Returns the information whether the route ends on the given lane
    bool endsOn(const MSLane &lane) const;

    /// timeconstant (driver reaction time)
    static double tau();

    /// Get the vehicle's position.
    double pos() const;

    /// Get the vehicle's length.
    double length() const;

    /** @brief Inserts a MSVehicle into the static dictionary
        Returns true if the key id isn't already in the dictionary.
        Otherwise returns false. */
    static bool dictionary( std::string id, MSVehicle* veh );

    /** @brief Returns the MSVehicle associated to the key id if exists
        Otherwise returns 0. */
    static MSVehicle* dictionary( std::string id );

    /// Removes the named vehicle from the dictionary and deletes it
    static void remove(const std::string &id);

    /** Clears the dictionary */
    static void clear();

    /// Returns the name of the vehicle
    std::string id() const;

    /// Prints the vehicle's name
    friend std::ostream& operator<<(std::ostream& os, const MSVehicle& veh);

    /** Return true if the lane is allowed */
    bool onAllowed( const MSLane* lane ) const;

    /** Return true if the lane is allowed */
    bool onAllowed( const MSLane* lane, size_t offset ) const;

    /** Return true if vehicle is on an allowed lane. */
    bool onAllowed( ) const;

    /** Returns true if the two vehicles overlap. */
    static bool overlap( const MSVehicle* veh1, const MSVehicle* veh2 );

    /** Returns true if vehicle's speed is below 60km/h. This is only relevant
        on highways. Overtaking on the right is allowed then. */
    bool congested() const;

    /// Returns current speed
    double speed() const;


	/// Return current Position
	Position2D position() const;


    /** Return true if prioritized first vehicle will brake too much
    during lane-change calculations. This is a problem because the
    lane-changer doesn't look beyond the lane but will assume the
    vehicle has to slow down towards the lane end.*/
//    bool laneChangeBrake2much( const State brakeState );

    /** timeHeadWay < deltaT situations may cause crashes because two
    vehicles want to leave the same lane in one timestep. These
    situations are caused by emissions and lanechanges.
    @param A speed.
    @return The distance driven with speed in the
    next timestep.*/
    double timeHeadWayGap( double speed ) const;


    /** Checks if Krauss' timeHeadWay condition "gap >= vPred *
    deltaT" is true. If you want to insert (emit or lanechenage) a vehicle
    behind a predecessor, this condition must be true for collision-free
    driving.
    @see isInsertBrakeCond
    @param Predecessors speed.
    @param Gap to predecessor.
    @return gap2pred >= vPred * deltaT */
    bool isInsertTimeHeadWayCond( double aPredSpeed, double aGap2Pred );

    /** Checks if Krauss' timeHeadWay condition "gap >= vPred *
    deltaT" is true. If you want to insert (emit or lanechenage) a vehicle
    behind a predecessor, this condition must be true for collision-free
    driving.
    @see isInsertBrakeCond
    @param The vehicle's predecessor.
    @return gap2pred >= vPred * deltaT */
    bool isInsertTimeHeadWayCond( MSVehicle& aPred );

    /** Checks if Krauss' dont't brake too much condition "vsafe >= v
    - decel * deltaT" is true. If you want to insert (emit or
    lanechenage) a vehicle behind a predecessor, this condition must
    be true for collision-free driving.
    @see isInsertTimeHeadWayCond
    @param Predecessors speed.
    @param Gap to predecessor.
    @return vsafe >= v - decel * deltaT */
    bool isInsertBrakeCond( double aPredSpeed, double aGap2Pred );

    /** Checks if Krauss' dont't brake too much condition "vsafe >= v
    - decel * deltaT" is true. If you want to insert (emit or
    lanechenage) a vehicle behind a predecessor, this condition must
    be true for collision-free driving.
    @see isInsertTimeHeadWayCond
    @param The vehicle's predecessor.
    @return vsafe >= v - decel * deltaT */
    bool isInsertBrakeCond( MSVehicle& aPred );

    const MSEdge * const getEdge() const;
    /** Dumps the collected meanData of the indexed interval to myLane.
        @param The index of the intervall to dump. */
//    void dumpData( unsigned index );

    /** Update of members if vehicle enters a new lane in the move step.
        @param Pointer to the entered Lane. */
    void enterLaneAtMove( MSLane* enteredLane, double driven );

    /** Update of members if vehicle enters a new lane in the emit step.
        @param Pointer to the entered Lane. */
    void enterLaneAtEmit( MSLane* enteredLane );

    /** Update of members if vehicle enters a new lane in the laneChange step.
        @param Pointer to the entered Lane. */
    void enterLaneAtLaneChange( MSLane* enteredLane );

    /** Update of members if vehicle leaves a new lane in the move step. */
    void leaveLaneAtMove( double driven );

    /** Update of members if vehicle leaves a new lane in the
        laneChange step. */
    void leaveLaneAtLaneChange( void );

    /* Update of MeanData members at every move a vehicle performs. */
//    void meanDataMove( void );

    bool reachingCritical(double laneLength) const;

    friend class MSVehicleControl;

    /** Returns the SK-vsafe. */
    double vsafe( double currentSpeed, double decelAbility,
                  double gap2pred, double predSpeed ) const;

    void vsafeCriticalCont( double minVSafe );

    /** Return the vehicle's maximum possible speed after acceleration. */
    double vaccel( const MSLane* lane ) const;

    /** Dawdle according the vehicles dawdle parameter. Return value >= 0 */
    double dawdle( double speed ) const;

    /** @brief Dawdle according the vehicles dawdle parameter in case of starting
        Regards that the vehicle should not dawdle more than his acceleration
        Return value >= 0 */
    double dawdle2( double speed ) const;

    MSLane *getTargetLane() const;

    /// Returns the lane the vehicle is on
    const MSLane &getLane() const;

    /// Returns the information whether further vehicles of this type shall be emitted periodically
    bool periodical() const;

    /// Returns the information whether the vehicle was aready emitted
    bool running() const;

    /** @brief Returns the next "periodical" vehicle with the same route
        We have to duplicate the vehicle if a further has to be emitted with
        the same settings */
    virtual MSVehicle *getNextPeriodical() const;

    bool proceedVirtualReturnWhetherEnded(MSVehicleTransfer &securityCheck,
        MSEdge *to);

    size_t getWaitingTime() const;
    void removeApproachingInformationOnKill();
//    void removeApproachingInformationOnKill(MSLane *begin);

    void rebuildAllowedLanes();

    friend class MSLane; // !!!
    friend class GUIInternalLane; // !!!
    friend class GUILane; // !!!

    void quitRemindedEntered(MSVehicleQuitReminded *r);
    void quitRemindedLeft(MSVehicleQuitReminded *r);

    /// Static dictionary to associate string-ids with objects.
    typedef std::map< std::string, MSVehicle* > DictType;
    static DictType myDict;


    MSAbstractLaneChangeModel &getLaneChangeModel();
    const MSAbstractLaneChangeModel &getLaneChangeModel() const;
    typedef std::deque<const MSEdge::LaneCont*> NextAllowedLanes;
    const NextAllowedLanes &getAllowedLanes(MSLaneChanger &lc);
    int countAllowedContinuations(const MSLane *lane) const;
    double allowedContinuationsLength(const MSLane *lane) const;

    MSUnit::Cells getMovedDistance( void ) const
        {
        return SPEED2DIST(myState.mySpeed);
        }

    const MSRoute &getRoute() const;

    const MSRoute &getRoute(int index) const;

    bool replaceRoute(const MSEdgeVector &edges, size_t simTime);

    const MSVehicleType &getVehicleType() const;

public:
    void onDepart();

    void onTripEnd(/*MSLane &caller, */bool wasAlreadySet=false);
    void writeXMLRoute(std::ostream &os, int index=-1) const;
protected:
    /// Use this constructor only.
    MSVehicle( std::string id, MSRoute* route, SUMOTime departTime,
        const MSVehicleType* type, size_t noMeanData,
        int repNo, int repOffset);


    /** Returns the minimum of four doubles. */
    double vMin( double v1, double v2, double v3, double v4 ) const;

    /** Reset meanData of indexed intervall after a dump.
        @param The index of the intervall to clear. */
//    void resetMeanData( unsigned index );

    /* Helper for enterLaneAt* methods.
        @param Timestep when vehicle entered the lane.
        @param Position where vehicle entered the lane.
        @param Speed at which vehicle entered the lane. */
    /*
    void updateMeanData( double entryTimestep,
                         double pos,
                         double speed );
*/

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

    /// Vehicle-type.
    const MSVehicleType* myType;

    /// Vehicles driving state. here: pos and speed
    State myState;

    /// The lane the vehicle is on
    MSLane* myLane;

    MSAbstractLaneChangeModel *myLaneChangeModel;

private:

    /// Reaction time [sec]
    static double myTau;

    /** Iterator to current route-edge.  */
    MSRouteIterator myCurrEdge;

    /** The vehicle's allowed lanes on it'S current edge to drive
        according to it's route. */
    NextAllowedLanes myAllowedLanes;

    // Collection of meanDataValues
    /*
    struct MeanDataValues
    {
        double entryContTimestep;
        unsigned entryDiscreteTimestep;
        double speedSum;
        double speedSquareSum;
        bool enteredAtLaneStart;
        double entryTravelTimestep;
        bool enteredLaneWithinIntervall;
    };

    /// Container of meanDataValues, one element for each mean-interval.
    std::vector< MeanDataValues > myMeanData;
*/
    /// Default constructor.
    MSVehicle();

    /// Copy constructor.
    MSVehicle(const MSVehicle&);

    /// Assignment operator.
    MSVehicle& operator=(const MSVehicle&);

    struct DriveProcessItem
    {
        MSLink *myLink;
        double myVLinkPass;
        double myVLinkWait;
        DriveProcessItem( MSLink *link, double vPass, double vWait  ) :
            myLink( link ), myVLinkPass(vPass), myVLinkWait(vWait) { };
    };

    typedef std::vector< DriveProcessItem > DriveItemVector;

    /// Container for used Links/visited Lanes during lookForward.
    DriveItemVector myLFLinkLanes;

/*    /// We need our own min/max methods because MSVC++ can't use the STL-ones.
    inline double min(double v1, double v2) const
        { return ((v1 < v2) ? v1 : v2); };
    inline double max(double v1, double v2) const
        { return ((v1 > v2) ? v1 : v2); };*/


//  double myVLinkPass;
//  double myVLinkWait;

//    struct

    typedef std::vector< MSMoveReminder* > MoveReminderCont;
    typedef MoveReminderCont::iterator MoveReminderContIt;
    MoveReminderCont myMoveReminders;
    MoveReminderCont myOldLaneMoveReminders;
    typedef std::vector<double> OffsetVector;
    OffsetVector myOldLaneMoveReminderOffsets;
    enum MoveOnReminderMode {BOTH, CURRENT};

    void workOnMoveReminders( double oldPos, double newPos, double newSpeed,
                              MoveOnReminderMode = BOTH);
    void activateRemindersByEmitOrLaneChange( void );

//    MSUnit::Cells movedDistanceDuringStepM;

    typedef std::vector<MSVehicleQuitReminded*> QuitRemindedVector;
    QuitRemindedVector myQuitReminded;

    std::map<MSCORN::Function, double> myDoubleCORNMap;
    std::map<MSCORN::Pointer, void*> myPointerCORNMap;

};



/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#ifndef DISABLE_INLINE
#include "MSVehicle.icc"
#endif

#endif

// Local Variables:
// mode:C++
// End:
