/***************************************************************************
                          MSVehicle.cpp  -  Base for all
                          micro-simulation Vehicles.
                             -------------------
    begin                : Mon, 05 Mar 2001
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.17  2003/04/16 10:05:06  dkrajzew
// uah, debugging
//
// Revision 1.16  2003/04/14 08:33:02  dkrajzew
// some further bugs removed
//
// Revision 1.15  2003/04/10 15:43:43  dkrajzew
// emission on non-source lanes debugged
//
// Revision 1.14  2003/04/09 15:32:29  dkrajzew
// periodical vehicles must have a period over zero now to be reasserted
//
// Revision 1.13  2003/04/07 12:12:38  dkrajzew
// eps reduced for small segments (dawdle2)
//
// Revision 1.12  2003/03/20 17:31:42  dkrajzew
// StringUtils moved from utils/importio to utils/common
//
// Revision 1.11  2003/03/20 16:21:12  dkrajzew
// windows eol removed; multiple vehicle emission added
//
// Revision 1.10  2003/03/12 16:50:56  dkrajzew
// lane retrival added for centering a vehicle on the display
//
// Revision 1.9  2003/03/03 14:56:26  dkrajzew
// some debugging; new detector types added; actuated traffic lights added
//
// Revision 1.8  2003/02/07 10:41:50  dkrajzew
// updated
//
// Revision 1.7  2002/10/29 10:42:51  dkrajzew
// problems accured due to the deletion of a vehicle that reached his destination debugged
//
// Revision 1.6  2002/10/28 12:59:38  dkrajzew
// vehicles are now deleted whe the tour is over
//
// Revision 1.5  2002/10/21 09:55:40  dkrajzew
// begin of the implementation of multireferenced, dynamically loadable routes
//
// Revision 1.4  2002/10/17 06:11:48  dkrajzew
// forgot setting of drive request when regarding a critical non-first vehicle added
//
// Revision 1.3  2002/10/16 17:33:10  dkrajzew
// error in moveCritical yielding in collisions removed
//
// Revision 1.2  2002/10/16 16:45:41  dkrajzew
// debugged
//
// Revision 1.1  2002/10/16 14:48:26  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.21  2002/09/25 17:14:42  roessel
// MeanData calculation and output implemented.
//
// Revision 1.20  2002/07/31 17:33:01  roessel
// Changes since sourceforge cvs request.
//
// Revision 1.23  2002/07/31 14:42:34  croessel
// Use of new VehicleType methods.
//
// Revision 1.22  2002/07/24 16:33:49  croessel
// New methods isInsertTimeHeadWayCond() and isInsertBrakeCond() to check
// the Krauss conditions during emit/laneChange.
//
// Revision 1.21  2002/07/16 17:50:36  croessel
// Removed debug code.
//
// Revision 1.20  2002/07/03 15:57:11  croessel
// Redundant code removal.
// New methods isSafeChange, hasSafeGap, safeEmitGap instead of safeGap.
// Removed safeLaneChangeGap, not used.
// New method vNeighEqualPos for "don't overtake on the right".
// move() respects now neighbours taht shouldn't be overtaken.
//
// Revision 1.19  2002/06/25 10:53:36  croessel
// Little mistake in destructor.
//
// Revision 1.18  2002/06/25 10:41:17  croessel
// safeGap using fabs is not the answer to collisions. Removing it.
//
// Revision 1.17  2002/06/21 10:59:09  dkrajzew
// inclusion of .cpp-files in .cpp files removed
//
// Revision 1.16  2002/06/20 13:44:58  dkrajzew
// safeGap is now using fabs
//
// Revision 1.15  2002/06/20 08:00:59  dkrajzew
// template and .cpp inclusion inserted due to problems with MSVC++; should
// be revalidated and removed as soon as possible
//
// Revision 1.14  2002/06/19 15:09:12  croessel
// Changed *Gap methods to check for timeheadway < deltaT states.
//
// Revision 1.13  2002/06/11 19:38:22  croessel
// Bugfix: in safeGap(), vDecel should be max(...), not
// min(...). Otherwise gap is always < 0 and LaneChanger will almost
// always allow a change. This may lead to collisions in the next
// timesteps.
//
// Revision 1.12  2002/06/06 07:21:10  croessel
// Changed inclusion from .iC to .icc
//
// Revision 1.11  2002/05/29 17:06:03  croessel
// Inlined some methods. See the .icc files.
//
// Revision 1.10  2002/05/17 13:23:36  croessel
// Changed novehicles to MSNet::noVehicles
//
// Revision 1.9  2002/05/17 13:04:24  croessel
// Added _SPEEDCHECK code in all move-methos.
// Added upper bound (= vaacel) for vSafe in nextState().
// nextStateCompete():
// - Removed vMax and vLaneMax, they are already considered in vaccel().
// - Calculate nextPos with vNext instead of vSafe.
// New method laneChangeBrake2much() added.
//
// Revision 1.8  2002/05/08 13:24:21  croessel
// safeGap(): vDecel shouldn't be less than 0.
//
// Revision 1.7  2002/05/08 11:36:26  croessel
// destReached() changed to allow routes to include the destination-edge
// several times before reaching the destination.
//
// Revision 1.6  2002/04/24 15:22:08  croessel
// Bugfix: changed myType->decel() into myType->accel() in dadwle().
//
// Revision 1.5  2002/04/18 15:05:18  croessel
// In nextStateCompete(): Returns now correct position instead of 0.
//
// Revision 1.4  2002/04/18 14:30:24  croessel
// Bug in Revision 1.3 changes. Return State( pos, speed = 0 ) instead of
// myState.
//
// Revision 1.3  2002/04/17 10:58:24  croessel
// Introduced dontMoveGap to handle floating-point-inaccuracy. Vehicles
// will keep their state if gap2pred is smaller.
//
// Revision 1.2  2002/04/11 15:25:56  croessel
// Changed float to double.
//
// Revision 1.1.1.1  2002/04/08 07:21:23  traffic
// new project name
//
// Revision 2.4  2002/03/20 16:02:23  croessel
// Initialisation and update of allowedLanes-container fixed.
//
// Revision 2.3  2002/03/13 17:44:38  croessel
// Assert removed because it causes not intended crashes for
// YIELD_ON_SUCC.
//
// Revision 2.2  2002/03/13 17:42:19  croessel
// In nextState() we need to dawdle _before_ we set the new state.
//
// Revision 2.1  2002/03/13 16:56:35  croessel
// Changed the simpleOutput to XMLOutput by introducing nested classes
// XMLOut. Output is now indented.
//
// Revision 2.0  2002/02/14 14:43:19  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.14  2002/02/05 13:51:53  croessel
// GPL-Notice included.
// In *.cpp files also config.h included.
//
// Revision 1.13  2002/02/05 11:52:11  croessel
// Changed method vAccel() to accelState().
// Introduced new mezhods for lane-change:
// State::advantage()
// onAllowed()
// overlap()
// congested()
//
// Revision 1.12  2002/02/01 13:57:07  croessel
// Changed methods and members bmax/dmax/sigma to more meaningful names
// accel/decel/dawdle.
//
// Revision 1.11  2002/01/31 13:53:08  croessel
// Assertion in move() added.
//
// Revision 1.10  2002/01/24 18:29:43  croessel
// New and modified gap-methods: interactionDist renamed to brakeGap,
// interactionGap introduced and calculation of gaps fixed.
//
// Revision 1.9  2002/01/16 10:07:50  croessel
// Modified some speed calculations to the use of MSNet::deltaT() instead
// of myTau.
//
// Revision 1.8  2001/12/20 14:50:07  croessel
// new method: id() (moved from .h)
//
// Revision 1.7  2001/12/13 14:11:58  croessel
// Introduction of MSVehicle::State methods.
//
// Revision 1.6  2001/12/06 13:16:41  traffic
// speed computation added (use -D _SPEEDCHECK)
//
// Revision 1.5  2001/11/21 15:25:13  croessel
// Numerous changes during debug session.
//
// Revision 1.4  2001/11/15 17:17:36  croessel
// Outcommented the inclusion of the inline *.iC files. Currently not needed.
// Vehicle-State introduced. Simulation is now independant of vehicle's
// speed. Still, the position is fundamental, also a gap between
// vehicles. But that's it.
//
// Revision 1.3  2001/11/14 15:47:34  croessel
// Merged the diffs between the .C and .cpp versions. Numerous changes
// in MSLane, MSVehicle and MSJunction.
//
// Revision 1.2  2001/11/14 10:49:07  croessel
// CR-line-end removed.
//
// Revision 1.1  2001/10/24 07:17:13  traffic
// new extension
//
// Revision 1.6  2001/10/23 09:31:41  traffic
// parser bugs removed
//
// Revision 1.4  2001/09/06 15:35:50  croessel
// Added operator<< to class MSVehicle for simple text output and minor
// changes.
//
// Revision 1.3  2001/07/25 12:17:59  traffic
// CC problems with make_pair repaired
//
// Revision 1.2  2001/07/16 12:55:47  croessel
// Changed id type from unsigned int to string. Added string-pointer
// dictionaries and dictionary methods.
//
// Revision 1.1.1.1  2001/07/11 15:51:13  traffic
// new start
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "MSLane.h"
#include "MSVehicle.h"
#include "MSEdge.h"
#include "MSVehicleType.h"
#include "MSNet.h"
#include "MSRoute.h"
#include "MSLinkCont.h"
#include <utils/common/StringUtils.h>
#include <iostream>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <algorithm>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static member variables
 * ======================================================================= */
MSVehicle::DictType MSVehicle::myDict;
double MSVehicle::myTau = 1;

// It is possible to get collisions because of arithmetic-inaccuracy
// at small gaps. Therefore we introduce "dontMoveGap"; if gap2pred is
// smaller, than vehicle will keep it's state.
const double dontMoveGap = 0.01;


/* =========================================================================
 * method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * methods of MSVehicle::State
 * ----------------------------------------------------------------------- */
MSVehicle::State::State() : myPos( 0 ), mySpeed( 0 )
{
}

/////////////////////////////////////////////////////////////////////////////

MSVehicle::State::State( const State& state )
{
    myPos = state.myPos;
    mySpeed = state.mySpeed;
}

/////////////////////////////////////////////////////////////////////////////

MSVehicle::State&
MSVehicle::State::operator=( const State& state )
{
    myPos   = state.myPos;
    mySpeed = state.mySpeed;
    return *this;
}

/////////////////////////////////////////////////////////////////////////////

bool
MSVehicle::State::operator!=( const State& state )
{
    return ( myPos   != state.myPos ||
             mySpeed != state.mySpeed );
}

/////////////////////////////////////////////////////////////////////////////

double
MSVehicle::State::pos() const
{
    return myPos;
}

/////////////////////////////////////////////////////////////////////////////

void
MSVehicle::State::setPos( double pos )
{
    assert( pos >= 0 );
    myPos = pos;
}

/////////////////////////////////////////////////////////////////////////////

bool
MSVehicle::State::advantage( const State& preferState,
                             const State& compareState )
{
    return preferState.mySpeed >= compareState.mySpeed;
}

/////////////////////////////////////////////////////////////////////////////

MSVehicle::State::State( double pos, double speed ) :
    myPos( pos ), mySpeed( speed )
{
}


/* -------------------------------------------------------------------------
 * departSortCriterium
 * ----------------------------------------------------------------------- */
bool departTimeSortCrit( const MSVehicle* x, const MSVehicle* y )
{
    return x->myDesiredDepart < y->myDesiredDepart;
}


/* -------------------------------------------------------------------------
 * MSVehicle-methods
 * ----------------------------------------------------------------------- */
MSVehicle::~MSVehicle()
{
    //myWaitingPersons.clear();
    if(!myRoute->inFurtherUse()) {
        MSRoute::erase(myRoute->getID());
    }
}

/////////////////////////////////////////////////////////////////////////////

MSVehicle::MSVehicle( string id,
                      MSRoute* route,
                      MSNet::Time departTime,
                      const MSVehicleType* type,
                      size_t noMeanData,
                      int repNo, int repOffset) :
    myLastLaneChangeOffset(0),
    myTarget(0),
    myWaitingTime( 0 ),
    myRepetitionNumber(repNo),
    myPeriod(repOffset),
    myID(id),
    myState(),
    myRoute(route),
    myCurrEdge(0),
    myAllowedLanes(0),
    myDesiredDepart(departTime),
    myType(type),
//      myMeanData( 3 ),
    myMeanData( noMeanData ),
    myLane( 0 )
{
    myCurrEdge = myRoute->begin();
    myAllowedLanes = ( *myCurrEdge )->allowedLanes( **( myCurrEdge + 1 ) );
}

/////////////////////////////////////////////////////////////////////////////

MSVehicle::State
MSVehicle::state() const
{
    return myState;
}

/////////////////////////////////////////////////////////////////////////////

MSEdge &
MSVehicle::departEdge()
{
    return **myCurrEdge;
}

/////////////////////////////////////////////////////////////////////////////

const MSEdge::LaneCont &
MSVehicle::departLanes()
{
    return *myAllowedLanes;
}

/////////////////////////////////////////////////////////////////////////////

MSNet::Time
MSVehicle::desiredDepart() const
{
    return myDesiredDepart;
}

/////////////////////////////////////////////////////////////////////////////

const MSEdge*
MSVehicle::succEdge(unsigned int nSuccs) const
{
    if ( hasSuccEdge( nSuccs ) ) {
        return *( myCurrEdge + nSuccs );
    }
    else {
        return 0;
    }
}

/////////////////////////////////////////////////////////////////////////////

bool
MSVehicle::hasSuccEdge(unsigned int nSuccs) const
{
    // MSNet::RouteIterator myCurrEdge must be a random access iterator.
    if (myCurrEdge + nSuccs >= myRoute->end()) {
        return false;
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////

bool
MSVehicle::destReached( const MSEdge* targetEdge )
{
    // search for the target in the vehicle's route. Usually there is
    // only one iteration. Only for very short edges a vehicle can
    // "jump" over one ore more edges in one timestep.
    MSRouteIterator edgeIt = myCurrEdge;

    while ( *edgeIt != targetEdge ) {

        ++edgeIt;
        assert( edgeIt != myRoute->end() );
    }

    myCurrEdge = edgeIt;

    // Check if destination-edge is reached. Update allowedLanes makes
    // only sense if destination isn't reached.
    MSRouteIterator destination = myRoute->end() - 1;

    if ( myCurrEdge == destination ) {

        return true;
    }
    else {

        myAllowedLanes =
            ( *myCurrEdge )->allowedLanes( **( myCurrEdge + 1 ) );
        assert(myAllowedLanes!=0);
        return false;
    }
}

/////////////////////////////////////////////////////////////////////////////

double
MSVehicle::brakeGap( const MSLane* lane ) const
{
    // Resolve the vsafe equation to gap. Assume predecessor has
    // speed == 0 and that vsafe will be the current speed plus acceleration,
    // i.e that with this gap there will be no interaction.
    double vNext = vaccel( lane );
    double gap = vNext *
        ( myState.mySpeed * myType->inversTwoDecel() + myTau );

    // If we are very slow, the distance driven with an accelerated speed
    // might be longer.
    double accelDist = myState.mySpeed * MSNet::deltaT() + myType->accelDist();

    // Don't allow timeHeadWay < deltaT situations.
    return max( max( gap, timeHeadWayGap( vNext ) ), accelDist );
}


double
MSVehicle::rigorousBrakeGap(const double &speed) const
{
//    double speed = state.mySpeed - myType->decel() * MSNet::deltaT();
    return speed * speed * myType->inversTwoDecel();
 /* !!! ha ha ha
    speed *
        ( speed * myType->inversTwoDecel() + myTau );
        */
}



/////////////////////////////////////////////////////////////////////////////

double
MSVehicle::interactionGap( const MSLane* lane, const MSVehicle& pred ) const
{
    // Resolve the vsafe equation to gap. Assume predecessor has
    // speed != 0 and that vsafe will be the current speed plus acceleration,
    // i.e that with this gap there will be no interaction.
    double vF = myState.mySpeed;
    double vL = pred.myState.mySpeed;
    double vNext = vaccel( lane );
    double gap = ( vNext - vL  ) *
        ( ( vF + vL ) * myType->inversTwoDecel() + myTau ) +
        vL * myTau;

    // Don't allow timeHeadWay < deltaT situations.
    return max( gap, timeHeadWayGap( vNext ) );
}

/////////////////////////////////////////////////////////////////////////////

bool
MSVehicle::isSafeChange( const MSVehicle& pred, const MSLane* lane ) const
{
    double gap   = gap2pred( pred );
    return hasSafeGap( pred, lane, gap );
}

/////////////////////////////////////////////////////////////////////////////

bool
MSVehicle::isSafeChange_WithDistance( double distance,
                                     const MSVehicle& pred,
                                     const MSLane* lane ) const
{
    return hasSafeGap( pred, lane, distance );
}

/////////////////////////////////////////////////////////////////////////////

bool
MSVehicle::hasSafeGap( const MSVehicle& pred,
                       const MSLane* lane,
                       double gap ) const
{
    double vSafe = vsafe( this->speed(), myType->decel(),
                          gap, pred.speed() );
    double vNext = min( vaccel( lane ), vSafe );

//      return ( vNext >= this->speed() - myType->decelSpeed() &&
//               gap   >= timeHeadWayGap( vNext ) );

    return ( vNext >= this->speed() - myType->decelSpeed() &&
             gap   >= timeHeadWayGap( pred.speed() ) );
}



/////////////////////////////////////////////////////////////////////////////

double
MSVehicle::safeEmitGap( void ) const
{
    double vNextMin = max( this->speed() - myType->decelSpeed(),
                           double( 0 ) ); // ok, minimum next speed
    double safeGap  = vNextMin *
        ( this->speed() * myType->inversTwoDecel() + myTau );
    return max( safeGap, timeHeadWayGap( myState.mySpeed ) ) + myType->accelDist();
}


double
MSVehicle::accelDist() const
{
    return myType->accelDist();
}


/////////////////////////////////////////////////////////////////////////////

double
MSVehicle::vNeighEqualPos( const MSVehicle& neigh )
{
    double v = ( neigh.pos() - this->pos() +
                 neigh.speed() * MSNet::deltaT() ) / MSNet::deltaT();
    assert( v >= 0 );
    // Don't break too hard.
    if ( v < speed() - myType->decel() * MSNet::deltaT() ) {

        return speed() - myType->decelSpeed();
    }

    // Be able to slow down to neighbours speed in following timesteps.
    if ( v >= neigh.speed() + myType->decelSpeed() ) {

        return neigh.speed() + myType->decelSpeed();
    }
    else {

        return v;
    }
}

/////////////////////////////////////////////////////////////////////////////

double
MSVehicle::driveDist( State state ) const
{
    return state.mySpeed * MSNet::deltaT();
}

/////////////////////////////////////////////////////////////////////////////

double
MSVehicle::decelDist() const
{
    return myType->decelDist();
}

/////////////////////////////////////////////////////////////////////////////

MSVehicle::State
MSVehicle::accelState( const MSLane* lane ) const
{
    return State( 0, vaccel( lane ) );
}

/////////////////////////////////////////////////////////////////////////////

void
MSVehicle::move( MSLane* lane,
                 const MSVehicle* pred,
                 const MSVehicle* neigh )
{
#ifdef _SPEEDCHECK
    MSNet::noVehicles++;
#endif
#ifdef ABS_DEBUG
    if(MSNet::globaltime>MSNet::searchedtime && (myID==MSNet::searched1||myID==MSNet::searched2)) {
        cout << "movea/1:" << MSNet::globaltime << ": " << myID << " at " << myLane->id() << ": " << pos() << ", " << speed() << endl;
    }
#endif
    double gap = gap2pred(*pred);
    if(gap<0.1) {
        assert(gap>-0.1);
        gap = 0;
    }
    double vAccel = vaccel( lane );
    double vSafe  = vsafe( myState.mySpeed, myType->decel(),
                           gap, pred->speed() );

    double vNext;
    // !!! non - Krauﬂ brake when urgent lane changing failed
    if( !myLane->appropriate(this) &&
        (_lcAction&LCA_LANEBEGIN)==0 ) {
        vNext = min(vSafe, myState.mySpeed-myType->decelSpeed()/2.0); // !!! full deceleration causes floating point problems
    }
    // !!! non - Krauﬂ brake when urgent lane changing failed
    else {
        if ( neigh != 0 ) {

            vNext = dawdle( min( vAccel,
                                 min( vSafe, vNeighEqualPos( *neigh ) ) ) );
        }
        else {
            vNext = dawdle( min( vAccel, vSafe ) );
        }
    }
    if(vNext<myState.mySpeed-myType->decelSpeed()) {
        vNext = myState.mySpeed-myType->decelSpeed();
    }
    vNext = max(0, vNext);

    if(vNext<=0.1) {
        myWaitingTime++;
    } else {
        myWaitingTime = 0;
    }


    double assertCheck = myState.mySpeed;
    // update position and speed
    myState.myPos  += vNext * MSNet::deltaT();
    assert( myState.myPos < lane->length() );
    myState.mySpeed = vNext;
    assert(assertCheck-myType->decel() <= myState.mySpeed);
#ifdef ABS_DEBUG
    if(MSNet::globaltime>MSNet::searchedtime && (myID==MSNet::searched1||myID==MSNet::searched2)) {
        cout << (assertCheck-myType->decel() - myState.mySpeed) << endl;
        cout << "movea/2:" << MSNet::globaltime << ": " << myID << " at " << myLane->id() << ": " << pos() << ", " << speed() << endl;
    }
#endif
}


void
MSVehicle::moveRegardingCritical(MSLane* lane,
                                 const MSVehicle* pred,
                                 const MSVehicle* neigh )
{
#ifdef _SPEEDCHECK
    MSNet::noVehicles++;
#endif
#ifdef ABS_DEBUG
    if(MSNet::globaltime>MSNet::searchedtime && (myID==MSNet::searched1||myID==MSNet::searched2)) {
        cout << "moveb/1:" << MSNet::globaltime << ": " << myID << " at " << myLane->id() << ": " << pos() << ", " << speed() << endl;
    }
#endif
    myLFLinkLanes.clear();
    // check whether the vehicle is not on an appropriate lane
    //  decelerate when yes
    if(!myLane->appropriate(this)) {
        myVWish =
            vsafe(myState.mySpeed,
                myType->decel() * MSNet::deltaT(),
                myLane->length()-myState.myPos-MSVehicleType::maxLength(),
                0);

        if( (_lcAction&LCA_LANEBEGIN)==0 ) {
            myVWish = min(myVWish, myState.mySpeed-myType->decelSpeed()/2.0); // !!! full deceleration causes floating point problems
        }

        if(pred!=0) {
            myVWish = max(0, min(myVWish,
                vsafe(myState.mySpeed, myType->decel() * MSNet::deltaT(),
                    gap2pred(*pred), pred->speed()) ) );
        } else {
            myVWish = max(0, myVWish);
        }
    }


    else {
        // compute other values as in move
        double vAccel = vaccel( lane );
        if(pred==0) {
            myVWish  =
                vsafe(myState.mySpeed, myType->decel(), myLane->length()-myState.myPos, 0);
            // set next links, computing possible speeds
            //  the vehcile is only bound by the lane speed
            myVWish = min(vAccel, myVWish);
            vsafeCriticalCont(vAccel);
        } else {
            myVWish =
                vsafe(myState.mySpeed, myType->decel(), gap2pred( *pred ), pred->speed());
            // set next links, computing possible speeds
            //  the vehcile is bound by the lane speed and must not drive faster
            //  than vsafe to the next vehicle
            myVWish = min(vAccel, myVWish);
            vsafeCriticalCont(myVWish);
        }
    }
}




void
MSVehicle::moveFirstChecked()
{
    // get vsafe
    double vsafe = myVWish;

#ifdef ABS_DEBUG
    if(MSNet::globaltime>MSNet::searchedtime && (myID==MSNet::searched1||myID==MSNet::searched2) ) {
	    cout << endl << "moveFirstChecked; vsafe:" << vsafe << endl;
    }
#endif

    DriveItemVector::iterator i;
    for(i=myLFLinkLanes.begin(); i!=myLFLinkLanes.end(); i++) {
	    MSLink *link = (*i).myLink;

#ifdef ABS_DEBUG
    if(MSNet::globaltime>MSNet::searchedtime && (myID==MSNet::searched1||myID==MSNet::searched2) ) {
        cout << myID << ":" << link << "->";
    	if(link!=0) {
	        cout << "(opened="<< link->opened() << ")" ;
	    }
	    cout << endl;
    }
#endif

        if(link==0||link->opened()) {
	        vsafe = (*i).mySpeed;
        } else {
            break;
        }

    }

#ifdef ABS_DEBUG
    if(MSNet::globaltime>MSNet::searchedtime && (myID==MSNet::searched1||myID==MSNet::searched2) ) {
    	cout << "vsafe:" << vsafe << endl;
    }
#endif

    double vNext;
    if(myState.speed()==0&&vsafe<myType->accelSpeed()) {
        // do not dawdle as much on short segments
        vNext = max(double(0), dawdle2( min(vsafe, vaccel(myLane)) ));
    } else {
        vNext = max(double(0), dawdle( min(vsafe, vaccel(myLane)) ));
    }

#ifdef ABS_DEBUG
    if(MSNet::globaltime>MSNet::searchedtime && (myID==MSNet::searched1||myID==MSNet::searched2) ) {
    	cout << "vnext:" << vNext << endl;
    }
#endif

    if(vNext<myState.mySpeed-myType->decelSpeed()) {
        vNext = myState.mySpeed-myType->decelSpeed();
    }

#ifdef ABS_DEBUG
    if(MSNet::globaltime>MSNet::searchedtime && (myID==MSNet::searched1||myID==MSNet::searched2) ) {
    	cout << "vnext:" << vNext << endl;
    }
#endif

    if(vNext<=0.1) {
        myWaitingTime++;
    } else {
        myWaitingTime = 0;
    }

    // check whether the vehicle was allowed to enter lane
    //  otherwise it is decelareted and we do not need to test for it's
    //  approach on the following lanes when a lane changing is performed
    bool approachAllowed = false;
    bool approachInformationReached = false;
    // dummy for assertion
    double assertbla = myState.mySpeed;
    // update position
    myState.myPos += vNext * MSNet::deltaT();
    myTarget = myLane;
    for(i=myLFLinkLanes.begin(); myState.myPos>myTarget->length(); i++) {
        MSLane *next = myTarget;
        // check whether the next driving information contains a link to
        //  a new lane
        if((*i).myLink!=0) {
            next = (*i).myLink->myLane;
        }
        // check whether the next lane is a really new lane
        //  update position information if so
        if(next!=myTarget) {
            myState.myPos -= myTarget->length();
            myTarget = next;
            assert(myState.myPos<myTarget->length());
            // we assume there is only one lane the vehicle can pass in
            //  a single step (otherise it would brake in vSafeCriticalCont)
            //  so the approached lane was found
            approachInformationReached = true;
            approachAllowed = true;
        }
    }

    // update speed
    myState.mySpeed = vNext;
    assert(assertbla-this->myType->decel() <= myState.mySpeed);

    // when we did not reach the approached lane yet, we have to
    //  search it, as the vehicle must register itself as the approaching one
    if(!approachInformationReached) {
        double checkDistance = rigorousBrakeGap(myState.mySpeed);
#ifdef ABS_DEBUG
    if(MSNet::globaltime>MSNet::searchedtime && (myID==MSNet::searched1||myID==MSNet::searched2) ) {
    	cout << myID << " rigBrakeGap = " << checkDistance << "( laneenddist=" << myTarget->length()-myState.myPos<<")"<< endl;
    }
#endif
        for(; !approachInformationReached&&i!=myLFLinkLanes.end(); i++) {
#ifdef ABS_DEBUG
    if(MSNet::globaltime>MSNet::searchedtime && (myID==MSNet::searched1||myID==MSNet::searched2) ) {
    	cout << "Link:" << (*i).myLink;
        if((*i).myLink!=0) {
            cout << " opened=" << (*i).myLink->opened() << " isTarget=" << ((*i).myLink->myLane!=myTarget);
        }
        cout << endl;
    }
#endif
            if((*i).myLink!=0) {
                if(!(*i).myLink->opened()&&checkDistance<myTarget->length()-myState.myPos) {
                    break;
                }
                if((*i).myLink->myLane!=myTarget) {
                    approachAllowed = true;
                }
            }
        }
    }
#ifdef ABS_DEBUG
    if(MSNet::globaltime>MSNet::searchedtime && (myID==MSNet::searched1||myID==MSNet::searched2) ) {
    	cout << myID << " approaching = " << approachAllowed << endl;
    }
#endif

#ifdef ABS_DEBUG
    if(MSNet::globaltime>MSNet::searchedtime && (myID==MSNet::searched1||myID==MSNet::searched2) ) {
	    cout << "vnext:" << vNext << endl;
    }
#endif

    // set information for lane changing (approaching vehicle)
    if(approachAllowed&&myApproachedLane!=0&&myApproachedLane!=myTarget) {
        myApproachedLane->setApproaching(myTarget->length() - myState.pos(), this);
    }
}

void
MSVehicle::_assertPos() const
{
    assert(myState.myPos<=myLane->length());
}


void
MSVehicle::vsafeCriticalCont( double minVSafe )
{
    myApproachedLane = 0;
    double decelAbility = myType->decel();
    // the vehicle may have just to look into the next lane
    //  compute this information and use it only once in the next loop
    double drove = myLane->length() - myState.myPos;
    bool nextOnly = (
        drove > 0 &&
        drove - MSVehicleType::maxLength() < 0 &&
        myState.mySpeed < decelAbility );
    if(nextOnly) {
        drove -= MSVehicleType::maxLength();
    }
    // compute the way the vehicle may drive when accelerating
/*    double vAccel = vaccel(myLane);
    double dist = vAccel +
        rigorousBrakeGap(vAccel); //myState.mySpeed * MSNet::deltaT() + myType->accelDist();
    */
    double dist = minVSafe +
        rigorousBrakeGap(minVSafe); //myState.mySpeed * MSNet::deltaT() + myType->accelDist();

#ifdef ABS_DEBUG
    if(MSNet::globaltime>MSNet::searchedtime && (myID==MSNet::searched1||myID==MSNet::searched2)) {
        cout << "vsafeCriticalCont/" << MSNet::globaltime << ":" << myID << ":" << dist << ":" << drove << endl;
    }
#endif

    // loop until all the way is checked or vsafe can not get smaller
    //  check junctions and last vehicles on following lanes
    MSLane *currentLane = myLane;
    size_t count = 0;
    while( (dist>drove || nextOnly) && count<2) {
        MSLinkCont::iterator link =
            myLane->succLinkSec( *this, 1, *currentLane );
        // compute a secure drove value
        double checkDrove = drove;
        if(checkDrove<0) {
            checkDrove = 0;
        }
        // the vehicle must change the lane on one of the next lanes
        if(currentLane->isLinkEnd(link)) {
            myLFLinkLanes.push_back(
                DriveProcessItem(0,
                    min(minVSafe,
                        vsafe(myState.mySpeed, decelAbility, checkDrove, 0))));
            return;
        }
        // vehicles being at a lower priorised lane must brake
        if(!(*link)->myPrio) {
            // as the vehicle is directly in front of a junction where
            //  it has a low priority, it would not be able to move further
            //  without the next case; Still, this is only true for vehicles
            //  waiting in front of the junction; fast approaching vehicles
            //  must slow down
            if(nextOnly) {
                // let the vehicle move to the lane's end
                myLFLinkLanes.push_back(
                    DriveProcessItem(0,
                        vsafe(myState.mySpeed, decelAbility,
                            myLane->length() - myState.myPos, 0)));
                // the vehicle may pass the link, when allowed
                myLFLinkLanes.push_back(
                    DriveProcessItem(*link,
                        min(minVSafe, decelAbility)));
            } else {
                // set information about deceleration to the link end
                //  the vehicle will decelerate
                minVSafe = min(minVSafe,
                    vsafe(myState.mySpeed, decelAbility,
                        checkDrove, 0));
                myLFLinkLanes.push_back(
                    DriveProcessItem(0, minVSafe));
            }
        }
        // check the progress on the following lane
        //  (incrementally from lane to lane)
        currentLane = (*link)->myLane;
        // check whether the vehicle must slow down due to a lower
        //  maximum lane speed
        minVSafe = min(vsafe(myState.mySpeed, decelAbility,
                checkDrove, currentLane->maxSpeed()), minVSafe);
        // check whether the next lane is empty;
        //  if not, check the last vehicle on this lane
        const State &nextPred = currentLane->myLastState;
#ifdef ABS_DEBUG
    if(MSNet::globaltime>MSNet::searchedtime && (myID==MSNet::searched1||myID==MSNet::searched2)) {
        cout << "nextPred v=" << nextPred.speed() << ", x=" << nextPred.pos() << endl;
    }
#endif
            if(drove+nextPred.pos()-MSVehicleType::maxLength()<0) {
                myLFLinkLanes.push_back(
                    DriveProcessItem(0, 0) );
                return;
            } else {
        		minVSafe = min(
	        	    minVSafe,
		            vsafe(myState.mySpeed, decelAbility,
			            drove+nextPred.pos()-MSVehicleType::maxLength(),
			            nextPred.speed())); // !!! 13.12.
	        }
//        }
        myLFLinkLanes.push_back(
            DriveProcessItem((*link), minVSafe) );
        // must brake before the end of the next lane
        if(count==1) {
            myLFLinkLanes.push_back(
                DriveProcessItem(0,
                    vsafe(myState.mySpeed, decelAbility,
                        drove, 0)));
            return;
        }
        if(minVSafe==0) {
            return;
        }
        // mark the next link as being approached
        (*link)->setApproaching(this);
        // set information about lane approaching
        //  this is done only for vehicles which do not
        if(!nextOnly) {
            myApproachedLane = currentLane;
        }
            // somekind of a workaround:
            //  when approaching very short lanes, try to slow down
            //  to make deceleration possible
            if(currentLane->length()<decelAbility) {
                myLFLinkLanes.push_back(
                    DriveProcessItem(0,
                        vsafe(myState.mySpeed, decelAbility, drove, 0)) );
                return;
            }
        // check whether vehicle will stop at the next lane
        //  if yes, the currently smallest vsafe can be used
        if(!nextOnly) {
            drove += currentLane->length();
        } else {
            drove = dist+1;
        }
        // when the vehicle ends on the following lane, return
        if(endsOn(*currentLane)) {
            if(drove<0) {
                return;/* min(
                    vsafe(myState.mySpeed, decelAbility, 0, 0),
                    minVSafe);*/
            } else {
                myLFLinkLanes.push_back(
                    DriveProcessItem(0,
                        min(
                            vsafe(myState.mySpeed, decelAbility, drove, 0),
                            minVSafe)));
            }
        }
        nextOnly = false;
        count++;
    }
}


bool
MSVehicle::endsOn(const MSLane &lane) const
{
    return lane.inEdge(myRoute->getLastEdge());
}

////////////////////////////////////////////////////////////////////////////

void
MSVehicle::moveDecel2laneEnd( MSLane* lane )
{
#ifdef _SPEEDCHECK
    MSNet::noVehicles++;
#endif
    double gap = lane->length() - myState.myPos; // !!!
//!!!    assert( gap <= brakeGap( lane ) );

    // Slow down and dawdle.
    double vAccel = vaccel(myLane);
    double vSafe  = vsafe( myState.mySpeed, myType->decel(), gap, 0 );
    double vNext  = dawdle( min(vSafe, vAccel) );
    if(vNext<myState.mySpeed-myType->decelSpeed()) {
        vNext = myState.mySpeed-myType->decelSpeed();
    }

    // update position and speed
    myState.myPos  += vNext * MSNet::deltaT();
//    assert( myState.myPos < lane->length() );
    myState.mySpeed = vNext;
}

////////////////////////////////////////////////////////////////////////////


void
MSVehicle::moveUpdateState( const State newState )
{
#ifdef _SPEEDCHECK
    MSNet::noVehicles++;
#endif

    myState.myPos  += newState.mySpeed * MSNet::deltaT();
    assert( myState.myPos >= 0 );

    myState.mySpeed = newState.mySpeed;
    assert( myState.mySpeed >= 0 );
}

////////////////////////////////////////////////////////////////////////////

void
MSVehicle::moveSetState( const State newState )
{
#ifdef _SPEEDCHECK
    MSNet::noVehicles++;
#endif

    myState = newState;
    assert( myState.myPos >= 0 );
    assert( myState.mySpeed >= 0 );
}

/////////////////////////////////////////////////////////////////////////////

MSVehicle::State
MSVehicle::nextState( MSLane* lane, double gap ) const
{
    // Don't move if gap < dontMoveGap to handle arithmetic inaccuracy.
    if ( gap < dontMoveGap ) {

        return State( myState.pos(), 0 );
    }

    // TODO
    // If we know that we will slow down, is there still the need to dawdle?

    double vAccel  = vaccel( lane );
    double vSafe   = vsafe( myState.mySpeed, myType->decel(), gap, 0 );
    double vNext   = dawdle( min( vSafe, vAccel ) );
    if(vNext<myState.mySpeed-myType->decelSpeed()) {
        vNext = myState.mySpeed-myType->decelSpeed();
    }
    double nextPos = myState.myPos + vNext * MSNet::deltaT(); // Will be
    // overridden if veh leaves lane.

    return State( nextPos, vNext );
}

/////////////////////////////////////////////////////////////////////////////

MSVehicle::State
MSVehicle::nextStateCompete( MSLane* lane,
                             State predState,
                             double gap2pred ) const
{
    double vAccel   = vaccel( lane );
    double vSafe    = vsafe( myState.mySpeed, myType->decel(),
                             gap2pred, predState.mySpeed );

    double vNext    = dawdle( min( vAccel, vSafe ) );
    if(vNext<myState.mySpeed-myType->decelSpeed()) {
        vNext = myState.mySpeed-myType->decelSpeed();
    }
    double nextPos  = myState.myPos + vNext * MSNet::deltaT();
    return State( nextPos, vNext );
}

/////////////////////////////////////////////////////////////////////////////

bool
MSVehicle::dictionary(string id, MSVehicle* ptr)
{
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        myDict.insert(DictType::value_type(id, ptr));
        return true;
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////

MSVehicle*
MSVehicle::dictionary(string id)
{
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        return 0;
    }
    return it->second;
}

/////////////////////////////////////////////////////////////////////////////

void
MSVehicle::clear()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        MSVehicle *veh = (*i).second;
        delete (*i).second;
    }
    myDict.clear();
}

/////////////////////////////////////////////////////////////////////////////

void
MSVehicle::remove(const std::string &id)
{
    DictType::iterator i = myDict.find(id);
    MSVehicle *veh = (*i).second;
    delete (*i).second;
    myDict.erase(id);
}



/////////////////////////////////////////////////////////////////////////////


double
MSVehicle::speed() const
{
    return myState.mySpeed;
}

/////////////////////////////////////////////////////////////////////////////

bool
MSVehicle::laneChangeBrake2much( const State brakeState )
{
    // SK-vnext can reduce speed about decel, dawdle about accel.
    double minAllowedNextSpeed =
        max( myState.mySpeed - myType->accelPlusDecelSpeed(),
             static_cast< double >( 0 ) );

    if ( brakeState.mySpeed < minAllowedNextSpeed ) {

        return true;
    }

    return false;
}

/////////////////////////////////////////////////////////////////////////////

string
MSVehicle::id() const
{
    return myID;
}

/////////////////////////////////////////////////////////////////////////////

ostream&
operator<<( ostream& os, const MSVehicle& veh )
{
    os << "MSVehicle: Id = " << veh.myID << endl;
    return os;
}

/////////////////////////////////////////////////////////////////////////////

MSVehicle::XMLOut::XMLOut( const MSVehicle& obj,
                           unsigned indentWidth,
                           bool withChildElemes ) :
    myObj( obj ),
    myIndentWidth( indentWidth ),
    myWithChildElemes( withChildElemes )
{
}

/////////////////////////////////////////////////////////////////////////////

ostream&
operator<<( ostream& os, const MSVehicle::XMLOut& obj )
{
    string indent( obj.myIndentWidth , ' ' );

    os << indent << "<vehicle id=\"" << obj.myObj.myID << "\" pos=\""
       << obj.myObj.pos() << "\" speed=\"" << obj.myObj.speed()
       << "\"/>" << endl;

// Currently there are no child elements. Maybe persons will be in future.
//    if ( obj.myWithChildElemes ) {
//
//    }

    return os;
}

/////////////////////////////////////////////////////////////////////////////

double
MSVehicle::vaccel( const MSLane* lane ) const
{
    // Accelerate until vehicle's max speed reached.
    double vVehicle = min( myState.mySpeed + myType->accelSpeed(),
                           myType->myMaxSpeed );

    // But don't drive faster than max lane speed.
    return min( vVehicle, lane->maxSpeed() );
}

/////////////////////////////////////////////////////////////////////////////

bool
MSVehicle::onAllowed( const MSLane* lane ) const
{
     MSEdge::LaneCont::const_iterator compare =
        find( myAllowedLanes->begin(), myAllowedLanes->end(), lane );
     return ( compare != myAllowedLanes->end() );
}


bool
MSVehicle::onAllowed( ) const
{
     MSEdge::LaneCont::const_iterator compare =
        find( myAllowedLanes->begin(), myAllowedLanes->end(), myLane );
     return ( compare != myAllowedLanes->end() );
}


/////////////////////////////////////////////////////////////////////////////

bool
MSVehicle::isInsertTimeHeadWayCond( double predSpeed, double gap2pred )
{
    return gap2pred >= timeHeadWayGap( predSpeed );
}

/////////////////////////////////////////////////////////////////////////////

bool
MSVehicle::isInsertTimeHeadWayCond( MSVehicle& aPred )
{
    return gap2predSec( aPred ) >= timeHeadWayGap( aPred.speed() );
}

/////////////////////////////////////////////////////////////////////////////

bool
MSVehicle::isInsertBrakeCond( double predSpeed, double gap2pred )
{
    return vsafe( speed(), myType->decel(), gap2pred, predSpeed )
        >= speed() - myType->decelSpeed();
}

/////////////////////////////////////////////////////////////////////////////

bool
MSVehicle::isInsertBrakeCond( MSVehicle& aPred )
{
    return vsafe( speed(), myType->decel(), gap2pred( aPred ), aPred.speed() )
        >= speed() - myType->decelSpeed();
}

/////////////////////////////////////////////////////////////////////////////

void
MSVehicle::dumpData( unsigned index )
{
    assert(myMeanData.size()>index);
    MeanDataValues& md = myMeanData[ index ];

    double leaveTimestep =
        static_cast< double >( MSNet::getInstance()->timestep() );

    myLane->addVehicleData( leaveTimestep - md.entryContTimestep,
                            MSNet::getInstance()->timestep() -
                            md.entryDiscreteTimestep,
                            myState.myPos - md.entryPos,
                            md.speedSum,
                            md.speedSquareSum,
                            index,
                            false );

    resetMeanData( index );
}
/////////////////////////////////////////////////////////////////////////////

void
MSVehicle::resetMeanData( unsigned index )
{
    assert(myMeanData.size()>index);
    MeanDataValues& md = myMeanData[ index ];

    MSNet::Time timestep = MSNet::getInstance()->timestep();

    md.entryContTimestep     = static_cast< double >( timestep );
    md.entryDiscreteTimestep = timestep;
    md.entryPos              = myState.myPos;
    md.speedSum              = 0;
    md.speedSquareSum        = 0;
}

/////////////////////////////////////////////////////////////////////////////

void
MSVehicle::updateMeanData( double entryTimestep,
                           double pos,
                           double speed )
{
    double speedSquare = speed * speed;
    unsigned discreteTimestep = MSNet::getInstance()->timestep();

    for ( vector< MeanDataValues >::iterator md = myMeanData.begin();
          md != myMeanData.end(); ++md ) {

        md->entryContTimestep  = entryTimestep;
        md->entryDiscreteTimestep = discreteTimestep;
        md->entryPos       = pos;
        md->speedSum       = speed;
        md->speedSquareSum = speedSquare;
    }
}


/////////////////////////////////////////////////////////////////////////////

void
MSVehicle::enterLaneAtMove( MSLane* enteredLane )
{
    myLane = enteredLane;
    myTarget = enteredLane;
    myApproachedLane = 0;
    double entryTimestep =
        static_cast< double >( MSNet::getInstance()->timestep() ) -
        myState.myPos / myState.mySpeed;
    updateMeanData( entryTimestep, 0, myState.mySpeed );
}

/////////////////////////////////////////////////////////////////////////////

void
MSVehicle::enterLaneAtLaneChange( MSLane* enteredLane )
{
    myApproachedLane = 0;
    myLane = enteredLane;
    updateMeanData( static_cast< double >( MSNet::getInstance()->timestep() ),
                    myState.myPos, myState.mySpeed );
}

/////////////////////////////////////////////////////////////////////////////

void
MSVehicle::enterLaneAtEmit( MSLane* enteredLane )
{
    myApproachedLane = 0;
    myLane = enteredLane;
    updateMeanData( static_cast< double >( MSNet::getInstance()->timestep() ),
                    myState.myPos, myState.mySpeed );
}

/////////////////////////////////////////////////////////////////////////////

void
MSVehicle::leaveLaneAtMove( void )
{
    double leaveTimestep =
        static_cast< double >( MSNet::getInstance()->timestep() ) - 1 +
        ( MSNet::deltaT() - myState.myPos / myState.mySpeed );

    for ( unsigned index = 0; index < myMeanData.size(); ++index ) {

        assert(myMeanData.size()>index);
        MeanDataValues& md = myMeanData[ index ];

//          // LeaveTimestep shouldn't contribute to meanSpeed. At measure
//          // time the vehicle isn't any longer on current myLane.
//          double speedSum = md.speedSum - myState.mySpeed;
//          double speedSquareSum =  md.speedSquareSum -
//              myState.mySpeed * myState.mySpeed;
//          if ( speedSum < 0 ) {
//              speedSum = 0;
//              speedSquareSum = 0;
//          }

        myLane->addVehicleData( leaveTimestep - md.entryContTimestep,
                                MSNet::getInstance()->timestep() -
                                md.entryDiscreteTimestep - 1,
                                myLane->length() - md.entryPos,
                                md.speedSum,
                                md.speedSquareSum,
                                index,
                                true );
    }
}

/////////////////////////////////////////////////////////////////////////////

void
MSVehicle::leaveLaneAtLaneChange( void )
{
    double leaveTimestep =
        static_cast< double >( MSNet::getInstance()->timestep() );

    for ( unsigned index = 0; index < myMeanData.size(); ++index ) {

        assert(myMeanData.size()>index);
        MeanDataValues& md = myMeanData[ index ];

//          // LeaveTimestep shouldn't contribute to meanSpeed. At measure
//          // time the vehicle isn't any longer on current myLane.
//          double speedSum = md.speedSum - myState.mySpeed;
//          double speedSquareSum =  md.speedSquareSum -
//              myState.mySpeed * myState.mySpeed;
//          if ( speedSum < 0 ) {
//              speedSum = 0;
//              speedSquareSum = 0;
//          }

        myLane->addVehicleData( leaveTimestep - md.entryContTimestep,
                                MSNet::getInstance()->timestep() -
                                md.entryDiscreteTimestep - 1,
                                myState.myPos - md.entryPos,
                                md.speedSum,
                                md.speedSquareSum,
                                index,
                                false );
    }
}

/////////////////////////////////////////////////////////////////////////////

void
MSVehicle::meanDataMove( void )
{
    double speed = myState.mySpeed;
    double speedSquare = speed * speed;

    for ( vector< MeanDataValues >::iterator md = myMeanData.begin();
          md != myMeanData.end(); ++md ) {

        md->speedSum       += speed;
        md->speedSquareSum += speedSquare;
    }
}



/////////////////////////////////////////////////////////////////////////////

const MSEdge * const
MSVehicle::getEdge() const
{
    return *myCurrEdge;
}



bool
MSVehicle::reachingCritical(double laneLength) const
{
    // check whether the vehicle will run over the lane when accelerating
    return (laneLength
        - myState.myPos - brakeGap(myLane)) <= 0;
}


double
MSVehicle::decelAbility() const
{
    return myType->decel() * MSNet::deltaT();
}


double
MSVehicle::accelAbility() const
{
    return myType->accel() * MSNet::deltaT();
}


MSLane *
MSVehicle::getTargetLane() const
{
    return myTarget;
}



const MSLane &
MSVehicle::getLane() const
{
    return *myLane;
}


bool
MSVehicle::periodical() const
{
    return myPeriod>0;
}


MSVehicle *
MSVehicle::getNextPeriodical() const
{
    // check whether another one shall be repated
    if(myRepetitionNumber<=0) {
        return 0;
    }
    return MSNet::getInstance()->buildNewVehicle(StringUtils::version1(myID),
        myRoute, myDesiredDepart+myPeriod, myType, myRepetitionNumber-1,
        myPeriod, 0);
}


bool
MSVehicle::running() const
{
    return myLane!=0;
}


double
MSVehicle::getSecureGap( const MSLane &lane, const MSVehicle &pred ) const
{
    double safeSpace1 = pow( lane.maxSpeed(), 2 ) /
                      ( decelAbility() ) +
                      MSVehicle::tau() + pred.accelDist() * 2.0;
    double safeSpace2 = vaccel(&lane) * MSNet::deltaT() +
        rigorousBrakeGap(vaccel(&lane))
        + pred.length();
    double vSafe = vsafe(0, decelAbility(), 0, pred.speed());
    double safeSpace3 =
        ( (vSafe - pred.speed())
        * ((vSafe+pred.speed()) / 2.0 / (2.0 * MSVehicleType::minDecel()) + MSVehicle::tau()) )
        + pred.speed() * MSVehicle::tau();
    double safeSpace = safeSpace1 > safeSpace2
        ? safeSpace1 : safeSpace2;
    safeSpace = safeSpace > safeSpace3
        ? safeSpace : safeSpace3;
    safeSpace = safeSpace > decelAbility()
        ? safeSpace : decelAbility();
    safeSpace += pred.length();
    safeSpace += accelAbility();
    return safeSpace;

}


double // !!! rename to "getApproachingSecureGap"
MSVehicle::getSecureGap( const MSVehicle &pred ) const
{
    double safeSpace1 = pow( myLane->maxSpeed(), 2 ) /
                      ( decelAbility() ) +
                      MSVehicle::tau() + pred.accelDist() * 2.0;
    double safeSpace2 = vaccel(myLane) * MSNet::deltaT() +
        rigorousBrakeGap(vaccel(myLane))
        + pred.length();
    double vSafe = vsafe(0, decelAbility(), 0, pred.speed());
    double safeSpace3 =
        ( (vSafe - pred.speed())
        * ((vSafe+pred.speed()) / 2.0 / (2.0 * MSVehicleType::minDecel()) + MSVehicle::tau()) )
        + pred.speed() * MSVehicle::tau();
    double safeSpace = safeSpace1 > safeSpace2
        ? safeSpace1 : safeSpace2;
    safeSpace = safeSpace > safeSpace3
        ? safeSpace : safeSpace3;
    safeSpace = safeSpace > decelAbility()
        ? safeSpace : decelAbility();
    safeSpace += pred.length();
    safeSpace += accelAbility();
    return safeSpace - myLane->length();

}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#ifdef DISABLE_INLINE
#include "MSVehicle.icc"
#endif

// Local Variables:
// mode:C++
// End:
