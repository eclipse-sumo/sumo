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

#ifndef MSVehicle_H
#define MSVehicle_H

#ifdef _SPEEDCHECK
extern long myvehicles;
#endif

#include "MSNet.h"
#include "MSEdge.h"
#include "MSRoute.h"
#include <helpers/Counter.h>
#include <map>
#include <string>

class MSLane;
class MSVehicleType;

/**
 */
class MSVehicle : private Counter< MSVehicle >
{
public:
#ifdef _DEBUG
    friend class MSLaneChanger;
#endif

    using Counter< MSVehicle >::howMany;

    friend class XMLOut;
    /** Class to generate XML-output for an edges and all lanes hold by
        this edge.
        Usage, e.g.: cout << XMLOut( edge, 4, true) << endl; */
    class XMLOut
    {
    public:
        XMLOut( const MSVehicle& obj,
                unsigned indentWidth ,
                bool withChildElemes );
        friend std::ostream& operator<<( std::ostream& os,
                                         const XMLOut& obj );
    private:
        const MSVehicle& myObj;
        unsigned myIndentWidth;
        bool myWithChildElemes;
    };

    friend std::ostream& operator<<( std::ostream& os,
                                     const XMLOut& obj );

    /// container that holds the vehicles driving state. May vary from
    /// model to model. here: SK, holds position and speed.
    class State
    {
        friend class MSVehicle;
        friend class MSTriggeredSource;

#ifdef _DEBUG
        friend class MSLaneChanger;
#endif

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

//!!!    private:
        /// Constructor.
        State( double pos, double speed );
    private:
        double myPos;
        double mySpeed;
    };

    /// Sort criterion for vehiles is the departure time.
    friend bool departTimeSortCrit( const MSVehicle* x, const MSVehicle* y );

    /// Destructor.
    ~MSVehicle();

    /// Use this constructor only.
    MSVehicle( std::string id, MSRoute* route, MSNet::Time
               departTime, const MSVehicleType* type);

    /// Returns the vehicles current state.
    State state() const;

    /// Returns the lane from where vehicle will depart.
    MSLane& departLane();

    /// Returns the desired departure time.
    MSNet::Time desiredDepart() const;

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
    double rigorousBrakeGap(const State &state) const;

    void _assertPos() const;

    /** In "gap2predecessor < interactionGap" region interaction between
        vehicle and predecessor with speed != 0 takes place. Else vehicle
        drives freely. */
    double interactionGap( const MSLane* lane, const MSVehicle& pred ) const;

    /** Checks if this vehicle and pred will be in a safe state if one
     * changes to the others lane. */
    bool isSafeChange( const MSVehicle& pred, const MSLane* lane ) const;

    /** Checks if the gap between this vehicle and pred is sufficient
     * for safe driving. */
    bool hasSafeGap( const MSVehicle& pred,
                     const MSLane* lane,
                     double gap ) const;

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

    /** Returns the vehicels driving distance during one timestep when
        driving with speed. */
    double driveDist( State state ) const;

    /** Returns the distance-difference between driving at constant
        speed and maximum braking in one timestep. */
    double decelDist() const;

    // Return the vehicles state after maximum acceleration.
    State accelState( const MSLane* lane ) const;

    double decelAbility() const;

    double accelDist() const;


///////////////////////////////////////////////////////////////////////////
// vnext for vehicles except the first in a lane

    // If there is no neigh, pass 0 to neigh.
    // If neigh is on curr lane, pass 0 to gap2neigh,
    // otherwise gap.
    // Updates drive parameters.
    void move( MSLane* lane,
               const MSVehicle* pred,
               const MSVehicle* neigh );


    void moveRegardingCritical( MSLane* lane,
        const MSVehicle* pred, const MSVehicle* neigh );


    // Slow down towards lane end. Updates state. For first vehicles only.
    void moveDecel2laneEnd( MSLane* lane );

    // Use this move for first vehicles that won't leave it's lane.
    void moveUpdateState( const State newState );

    // Use this move for first vehicles that will leave it's lane.
    void moveSetState( const State newState );

////////////////////////////////////////////////////////////////////////////
// vnext for first vehicles

    // Returns the next State of a (first) vehicle. Use this if moving is
    // delayed, e.g. by junctions, and you need to store the state.
    // Here: in the SKModell, only the speed is of interest, the position
    // is fundamental to all modells in this design.
    // If there is no pred/neigh, pass 0 to pred/neigh.
    // If pred/neigh is on curr lane pass 0 to gap2pred/gap2neigh,
    // otherwise the gap.
    State nextState( MSLane* lane,
                     State predState,  double gap2pred,
                     State neighState, double gap2neigh ) const;

    // Slow down to one's lane end, don't respect neighbours. Lane-end
    // need not to be the lane-end of the current lane.
    State nextState( MSLane* lane, double gap ) const;

    // Use this form if pred would give the wrong position, e.g. if you
    // want to know what might will happen if this vehicle would have
    // a new pred.
    State nextStateCompete( MSLane* lane,
                            State predState,
                            double gap2pred ) const;


///////////////////////////////////////////////////////////////////////////
    bool endsOn(const MSLane &lane) const;

    static double tau(); // returns timeconstant

    /// Get the vehicle's position.
    double pos() const;

    /// Get the vehicle's length.
    double length() const;

    /** Inserts a MSVehicle into the static dictionary and returns true
        if the key id isn't already in the dictionary. Otherwise returns
        false. */
    static bool dictionary( std::string id, MSVehicle* veh );

    /** Returns the MSVehicle associated to the key id if exists,
        otherwise returns 0. */
    static MSVehicle* dictionary( std::string id );

    /** Clears the dictionary */
    static void clear();

    std::string id() const;

    friend std::ostream& operator<<(std::ostream& os, const MSVehicle& veh);

    /** Return true if vehicle is on an allowed lane. */
    bool onAllowed( const MSLane* lane ) const;

    /** Returns true if the two vehicles overlap. */
    static bool overlap( const MSVehicle* veh1, const MSVehicle* veh2 );

    /** Returns true if vehicle's speed is below 60km/h. This is only relevant
        on highways. Overtaking on the right is allowed then. */
    bool congested();

    /// Returns current speed
    double speed() const;

    /** Return true if prioritized first vehicle will brake too much
	during lane-change calculations. This is a problem because the
	lane-changer doesn't look beyond the lane but will assume the
	vehicle has to slow down towards the lane end.*/
    bool laneChangeBrake2much( const State brakeState );

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
    void dumpData( unsigned index );

    /** Update of members if vehicle enters a new lane in the move step.
        @param Pointer to the entered Lane. */
    void enterLaneAtMove( MSLane* enteredLane );

    /** Update of members if vehicle enters a new lane in the emit step.
        @param Pointer to the entered Lane. */
    void enterLaneAtEmit( MSLane* enteredLane );

    /** Update of members if vehicle enters a new lane in the laneChange step.
        @param Pointer to the entered Lane. */
    void enterLaneAtLaneChange( MSLane* enteredLane );

    /** Update of members if vehicle leaves a new lane in the move step. */
    void leaveLaneAtMove( void );

    /** Update of members if vehicle leaves a new lane in the
        laneChange step. */
    void leaveLaneAtLaneChange( void );

    /** Update of MeanData members at every move a vehicle performs. */
    void meanDataMove( void );

    bool reachingCritical(double laneLength) const;

protected:

    /** Returns the SK-vsafe. */
    double vsafe( double currentSpeed, double decelAbility,
                  double gap2pred, double predSpeed ) const;

    double vsafeCritical( const MSVehicle *pred) const;

    /** Return the vehicle's maximum possible speed after acceleration. */
    double vaccel( const MSLane* lane ) const;

    /** Dawdle according the vehicles dawdle parameter. Return value >= 0 */
    double dawdle( double speed ) const;

    /** Returns the minimum of four doubles. */
    double vMin( double v1, double v2, double v3, double v4 ) const;

    /** Reset meanData of indexed intervall after a dump.
        @param The index of the intervall to clear. */
    void resetMeanData( unsigned index );
    
    /** Helper for enterLaneAt* methods.
        @param Timestep when vehicle entered the lane.
        @param Position where vehicle entered the lane.
        @param Speed at which vehicle entered the lane. */
    void updateMeanData( double entryTimestep,
                         double pos,
                         double speed );
    
    
private:
    /// Reaction time [sec]
    static double myTau;

    /// Unique ID.
    std::string myID;

    /// Vehicles driving state. here: pos and speed
    State myState;

    /// Vehicle's route.
    MSRoute* myRoute;

    /** Iterator to current route-edge.  */
    MSRouteIterator myCurrEdge;

    /** The vehicle's allowed lanes on it'S current edge to drive
        according to it's route. */
    const MSEdge::LaneCont* myAllowedLanes;

    /// Desired departure time (seconds).
    MSNet::Time myDesiredDepart;

    /// Vehicle-type.
    const MSVehicleType* myType;

    /// Static dictionary to associate string-ids with objects.
    typedef std::map< std::string, MSVehicle* > DictType;
    static DictType myDict;

    /// Collection of meanDataValues
    struct MeanDataValues
    {
        double entryContTimestep;
        unsigned entryDiscreteTimestep;
        double entryPos;
        double speedSum;
        double speedSquareSum;
    };

    /// Container of meanDataValues, one element for each mean-interval.
    std::vector< MeanDataValues > myMeanData;

    MSLane* myLane;
    
    /// Default constructor.
    MSVehicle();

    /// Copy constructor.
    MSVehicle(const MSVehicle&);

    /// Assignment operator.
    MSVehicle& operator=(const MSVehicle&);

    /// We need our own min/max methods because MSVC++ can't use the STL-ones.
    inline double min(double v1, double v2) const
        { return ((v1 < v2) ? v1 : v2); };
    inline double max(double v1, double v2) const
        { return ((v1 > v2) ? v1 : v2); };

};



/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#ifndef DISABLE_INLINE
#include "MSVehicle.icc"
#endif

#endif

// Local Variables:
// mode:C++
// End:








