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
// Revision 1.58  2004/12/16 12:24:45  dkrajzew
// debugging
//
// Revision 1.57  2004/11/23 10:20:10  dkrajzew
// new detectors and tls usage applied; debugging
//
// Revision 1.56  2004/08/02 12:40:55  dkrajzew
// debugging; refactoring; lane-changing API
//
// Revision 1.55  2004/07/02 09:57:37  dkrajzew
// handling of routes added
//
// Revision 1.54  2004/04/02 11:36:28  dkrajzew
// "compute or not"-structure added;
//  added two further simulation-wide output
//  (emission-stats and single vehicle trip-infos)
//
// Revision 1.53  2004/04/01 16:39:03  roessel
// Bug fix: the value of the first parameter in the call to
// updateMeanData in the methods enterLaneAtEmit and
// enterLaneAtLaneChange could have been unreasonably high
// (unsigned(0)-1) because of wrong placement of parentheses. The example
// in data/examples/cross3ltl_meandata/ does perform without assertion
// now.
//
// Revision 1.52  2004/03/19 13:09:40  dkrajzew
// debugging
//
// Revision 1.51  2004/02/18 05:30:09  dkrajzew
// removal of all moveReminders on lane change added
//
// Revision 1.50  2004/02/16 15:21:58  dkrajzew
// movedDistance-retrival reworked; forgetting predecessors when driving over
//  more than one lane patched
//
// Revision 1.49  2004/02/05 16:37:51  dkrajzew
// e3-debugging: only e3-detectors have to remove killed vehicles; storage
//  for detectors to be informed added
//
// Revision 1.48  2004/01/26 15:55:55  dkrajzew
// the vehicle is now informed about being emitted (as we want to display
//  the information about the real departure time witin the gui - within
//  microsim, this information may be used for some other stuff)
//
// Revision 1.47  2004/01/26 07:51:44  dkrajzew
// the vehicle leaves his move reminders when leaving the simulation, now
//  (is still false)
//
// Revision 1.46  2004/01/12 15:03:40  dkrajzew
// removed some unneeded debug-variables
//
// Revision 1.45  2003/12/12 12:37:42  dkrajzew
// proper usage of lane states applied; scheduling of vehicles into the
//  beamer on push failures added
//
// Revision 1.44  2003/12/11 06:31:45  dkrajzew
// implemented MSVehicleControl as the instance responsible for vehicles
//
// Revision 1.43  2003/12/04 13:30:41  dkrajzew
// work on internal lanes
//
// Revision 1.42  2003/11/24 10:22:56  dkrajzew
// patched the false usage of oldLaneMoveReminders when more than one street
//  is within
//
// Revision 1.41  2003/11/20 14:59:17  dkrajzew
// detector usage patched
//
// Revision 1.40  2003/11/12 13:50:30  dkrajzew
// MSLink-members are now secured from the outer world
//
// Revision 1.39  2003/11/11 08:36:21  dkrajzew
// removed some debug-variables
//
// Revision 1.38  2003/10/31 08:04:18  dkrajzew
// an invalid assertion removed
//
// Revision 1.37  2003/10/28 08:37:01  dkrajzew
// retelportation avoidance added
//
// Revision 1.36  2003/10/24 16:48:37  roessel
// Added new method getMovedDistance and corresponding member.
//
// Revision 1.35  2003/10/22 07:06:04  dkrajzew
// patching of lane states on force vehicle removal added
//
// Revision 1.34  2003/10/20 07:59:43  dkrajzew
// grid lock dissolving by vehicle teleportation added
//
// Revision 1.33  2003/10/17 06:52:01  dkrajzew
// acceleration is now time-dependent
//
// Revision 1.32  2003/10/16 08:33:49  dkrajzew
// new lane changing rules implemented
//
// Revision 1.31  2003/10/15 11:43:50  dkrajzew
// false lane-changing rules removed; an (far too large information interface
//  between vehicle and lane-changer implemented
//
// Revision 1.30  2003/09/22 12:35:03  dkrajzew
// vehicle does not decelerate on yellow when halting is not possible
//
// Revision 1.29  2003/09/05 15:14:42  dkrajzew
// first steps for reading of internal lanes
//
// Revision 1.28  2003/08/20 11:44:11  dkrajzew
// min and max-functions moved to an own definition file
//
// Revision 1.27  2003/08/04 11:47:05  dkrajzew
// vehicle collision on yellow tl patched
//
// Revision 1.26  2003/07/18 12:35:04  dkrajzew
// removed some warnings
//
// Revision 1.25  2003/06/19 10:58:34  dkrajzew
// too conservative computation of the braking gap patched
//
// Revision 1.24  2003/06/18 11:30:26  dkrajzew
// debug outputs now use a DEBUG_OUT macro instead of cout; this shall ease
//  the search for further couts which must be redirected to the messaging
//  subsystem
//
// Revision 1.23  2003/06/05 10:19:44  roessel
// Added previous lane reminder-container and workOnMoveReminders().
//
// Revision 1.22  2003/05/25 16:15:10  roessel
// Rewrite of workOnMoveReminders and activateRemindersByEmitOrLaneChange.
//
// Revision 1.21  2003/05/22 12:48:12  roessel
// New method activateRemindersByEmitOrLaneChange. Exchanged for-loop by new
// method.
//
// Revision 1.20  2003/05/21 16:20:45  dkrajzew
// further work detectors
//
// Revision 1.19  2003/05/21 15:15:42  dkrajzew
// yellow lights implemented (vehicle movements debugged
//
// Revision 1.18  2003/05/20 09:31:46  dkrajzew
// emission debugged; movement model reimplemented (seems ok); detector output
// debugged; setting and retrieval of some parameter added
//
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
// problems accured due to the deletion of a vehicle that reached his
// destination debugged
//
// Revision 1.6  2002/10/28 12:59:38  dkrajzew
// vehicles are now deleted whe the tour is over
//
// Revision 1.5  2002/10/21 09:55:40  dkrajzew
// begin of the implementation of multireferenced, dynamically loadable routes
//
// Revision 1.4  2002/10/17 06:11:48  dkrajzew
// forgot setting of drive request when regarding a critical non-first vehicle
// added
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
#include "MSVehicleQuitReminded.h"
#include <utils/common/StringUtils.h>
#include <utils/common/StdDefs.h>
#include <utils/gfx/RGBColor.h>
#include <microsim/MSVehicleControl.h>
#include <iostream>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include "MSMoveReminder.h"
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>
#include "lanechanging/MSLCM_Krauss.h"
#include "lanechanging/MSLCM_DK2004.h"
#include <utils/convert/ToString.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * some definitions (debugging only)
 * ======================================================================= */
#define DEBUG_OUT cout


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
/*
    bool departTimeSortCrit( const MSVehicle* x, const MSVehicle* y )
{
    return x->myDesiredDepart < y->myDesiredDepart;
}
*/

/* -------------------------------------------------------------------------
 * MSVehicle-methods
 * ----------------------------------------------------------------------- */
MSVehicle::~MSVehicle()
{
    for(QuitRemindedVector::iterator i=myQuitReminded.begin(); i!=myQuitReminded.end(); ++i) {
        (*i)->removeOnTripEnd(this);
    }
    //myWaitingPersons.clear();
    if(!myRoute->inFurtherUse()) {
        MSRoute::erase(myRoute->getID());
    }
    // delete
    if(myDoubleCORNMap.find(MSCORN::CORN_VEH_NUMBERROUTE)!=myDoubleCORNMap.end()) {
        int noReroutes = (int) myDoubleCORNMap[MSCORN::CORN_VEH_NUMBERROUTE];
        for(int i=0; i<noReroutes; ++i) {
            delete myPointerCORNMap[(MSCORN::Pointer) (i+noReroutes)];
        }
    }
    /*
    for(std::map<MSCORN::Pointer, void*>::iterator j=myPointerCORNMap.begin(); j!=myPointerCORNMap.end(); ++j) {
        delete (*j).second;
    } !!!
    */
    delete myLaneChangeModel;
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
    myRoute(route),
    myDesiredDepart(departTime),
    myType(type),
    myState(),
    myLane( 0 ),
    myCurrEdge(0),
    myAllowedLanes(0),
    myMeanData( noMeanData ),
    myMoveReminders( 0 ),
    myOldLaneMoveReminders( 0 ),
    myOldLaneMoveReminderOffsets( 0 ),
    movedDistanceDuringStepM(0)
{
    myCurrEdge = myRoute->begin();
    rebuildAllowedLanes();
//    myAllowedLanes = ( *myCurrEdge )->allowedLanes( **( myCurrEdge + 1 ) );
//    assert(myAllowedLanes!=0);
    myLaneChangeModel = new MSLCM_DK2004(*this);
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
    return *(myAllowedLanes[0]);
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
    if(targetEdge->getPurpose()==MSEdge::EDGEFUNCTION_INTERNAL) {
        return false;
    }
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
    } else {
        rebuildAllowedLanes();
//        myAllowedLanes =
//            ( *myCurrEdge )->allowedLanes( **( myCurrEdge + 1 ) );
//        assert(myAllowedLanes!=0);
        return false;
    }
}

/////////////////////////////////////////////////////////////////////////////

double
MSVehicle::brakeGap( const MSLane* lane ) const
{
    return myState.mySpeed * myState.mySpeed * myType->inversTwoDecel()
        + myState.mySpeed * myTau;
}


double
MSVehicle::brakeGap( double speed ) const
{
    return speed * speed * myType->inversTwoDecel()
        + speed * myTau;
}

double
MSVehicle::rigorousBrakeGap(const double &speed) const
{
    throw 1; // !!!
    return speed * speed * myType->inversTwoDecel();
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
    return MAX( gap, timeHeadWayGap( vNext ) );
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
    double vSafe = vsafe( speed(), myType->decel(), gap, pred.speed() );
    double vNext = MIN3( vaccel( lane ), vSafe, myType->maxSpeed() );
    return (vNext >= speed() - myType->decelSpeed()
            &&
            gap   >= timeHeadWayGap( pred.speed() ) );
}


bool
MSVehicle::hasSafeGap(  double gap, double predSpeed,
                       const MSLane* lane) const
{
    double vSafe = vsafe( speed(), myType->decel(), gap, predSpeed );
    double vNext = MIN3( vaccel( lane ), vSafe, myType->maxSpeed() );
    return (vNext >=speed() - myType->decelSpeed()
            &&
            gap   >= timeHeadWayGap( predSpeed ) );
}


/////////////////////////////////////////////////////////////////////////////

double
MSVehicle::safeEmitGap( void ) const
{
    double vNextMin = MAX( speed() - myType->decelSpeed(),
                           double( 0 ) ); // ok, minimum next speed
    double safeGap  = vNextMin *
        ( speed() * myType->inversTwoDecel() + myTau );
    return MAX(
        safeGap,
        timeHeadWayGap( myState.mySpeed ) ) +
            myType->accelDist(myState.mySpeed);
}


double
MSVehicle::accelDist() const
{
    return myType->accelDist(myState.mySpeed);
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
    myTarget = 0;
#ifdef ABS_DEBUG
    if(MSNet::globaltime>MSNet::searchedtime && (myID==MSNet::searched1||myID==MSNet::searched2)) {
        DEBUG_OUT << "movea/1:" << MSNet::globaltime << ": " << myID << " at " << myLane->id() << ": " << pos() << ", " << speed() << endl;
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

    // !!! non - Krauß brake when urgent lane changing failed
/*    if( !myLane->appropriate(this) &&
        (_lcAction&LCA_LANEBEGIN)==0 ) {
        vNext = MIN(vSafe, myState.mySpeed-myType->decelSpeed()/2.0); // !!! full deceleration causes floating point problems
    }*/
    // !!! non - Krauß brake when urgent lane changing failed
//    else {
            // lane-change interaction
    ///* !!!!!
      //      */
        if ( neigh != 0 ) {

            vNext = dawdle( MIN3( vAccel, vSafe, vNeighEqualPos( *neigh ) ) );
        }
        else {
            vNext = dawdle( MIN( vAccel, vSafe ) );
        }
    vNext =
        myLaneChangeModel->patchSpeed(
            MAX(0, myState.mySpeed-myType->decel()*MSNet::deltaT()),
            vNext,
            MIN(vSafe, vaccel(myLane)),
            vSafe);
    vNext = MIN3(vNext, vSafe, vaccel(myLane));
//    }
    // check needed for the Krauss-model
    /*!!!
    double accelSpace = accelDist()*MSNet::deltaT();
    if( gap<accelSpace &&
        //pred->speed()<accelSpace &&
        myState.mySpeed<vaccel(myLane) ) {

        vNext = gap / MSNet::deltaT();
    }
*/
    double predDec = MAX(0, pred->speed()-decelAbility() /* !!! decelAbility of leader! */);
    if(brakeGap(vNext)+vNext*myTau > brakeGap(predDec) + gap) {

        vNext = MIN(vNext, gap / MSNet::deltaT());
    }

    // check whether the driver wants to let someone in
    //  but only if the vehicle is far enough from the lane's end
    if(myState.myPos+MSVehicleType::maxLength()-2<myLane->length()) {
//        vNext = myLaneChangeState.modifySpeed(vNext, myState.mySpeed-decelAbility());
    }
    vNext = MAX(0, vNext);
    if(vNext<=0.1) {
        myWaitingTime++;
    } else {
        myWaitingTime = 0;
    }

    // call reminders after vNext is set
    workOnMoveReminders( myState.myPos,
                         myState.myPos + vNext * MSNet::deltaT(), vNext );
    // update position and speed
    myState.myPos  += vNext * MSNet::deltaT();
    assert( myState.myPos < lane->length() );
    myState.mySpeed = vNext;
#ifdef ABS_DEBUG
    if(MSNet::globaltime>MSNet::searchedtime && (myID==MSNet::searched1||myID==MSNet::searched2)) {
        DEBUG_OUT << "movea/2:" << MSNet::globaltime << ": " << myID << " at " << myLane->id() << ": " << pos() << ", " << speed() << endl;
    }
#endif
    if(hasCORNDoubleValue(MSCORN::CORN_VEH_LASTREROUTEOFFSET)) {
        myDoubleCORNMap[MSCORN::CORN_VEH_LASTREROUTEOFFSET] =
            myDoubleCORNMap[MSCORN::CORN_VEH_LASTREROUTEOFFSET] + 1;
    }
}


void
MSVehicle::moveRegardingCritical(MSLane* lane,
                                 const MSVehicle* pred,
                                 const MSVehicle* neigh )
{
#ifdef ABS_DEBUG
    if(MSNet::globaltime>MSNet::searchedtime && (myID==MSNet::searched1||myID==MSNet::searched2)) {
        DEBUG_OUT << "moveb/1:" << MSNet::globaltime << ": " << myID << " at " << myLane->id() << ": " << pos() << ", " << speed() << endl;
    }
#endif
    myLFLinkLanes.clear();
    // check whether the vehicle is not on an appropriate lane
    if(!myLane->appropriate(this)) {
        // decelerate to lane end when yes
        double vWish =
            vsafe(myState.mySpeed,
                myType->decel() * MSNet::deltaT(),
                myLane->length()-myState.myPos/*-MSVehicleType::maxLength()*/,
                0);
        if(pred!=0) {
            vWish = MIN(vWish,
                vsafe(myState.mySpeed, myType->decel() * MSNet::deltaT(),
                    gap2pred(*pred), pred->speed()) );
        }
        vWish = MAX(0, vWish);
        // check whether the driver wants to let someone in
        if(myState.myPos+MSVehicleType::maxLength()-2<myLane->length()) {
//            vWish = myLaneChangeState.modifySpeed(vWish, myState.mySpeed-decelAbility());
        }
        myLFLinkLanes.push_back(
            DriveProcessItem(0, vWish, vWish));
    } else {
        // compute other values as in move
        double vBeg = vaccel( lane );
        if(pred!=0) {
            double vSafe =
                vsafe(myState.mySpeed, myType->decel(),
                    gap2pred( *pred ), pred->speed());
            //  the vehcile is bound by the lane speed and must not drive faster
            //  than vsafe to the next vehicle
            vBeg = MIN(vBeg, vSafe);
        }
        // check whether the driver wants to let someone in
        if(myState.myPos+MSVehicleType::maxLength()-2<myLane->length()) {
//            vBeg = myLaneChangeState.modifySpeed(vBeg, myState.mySpeed-decelAbility());
        }
        // set next links, computing possible speeds
        vsafeCriticalCont(vBeg);
    }
    if(hasCORNDoubleValue(MSCORN::CORN_VEH_LASTREROUTEOFFSET)) {
        myDoubleCORNMap[MSCORN::CORN_VEH_LASTREROUTEOFFSET] =
            myDoubleCORNMap[MSCORN::CORN_VEH_LASTREROUTEOFFSET] + 1;
    }
}




void
MSVehicle::moveFirstChecked()
{
    myTarget = 0;
#ifdef ABS_DEBUG
    if(MSNet::globaltime>MSNet::searchedtime && (myID==MSNet::searched1||myID==MSNet::searched2) ) {
        int textdummy = 0;
    }
#endif
    // get vsafe
    double vSafe = 0;
    assert(myLFLinkLanes.size()!=0);
    DriveItemVector::iterator i;
    MSLane *currentLane = myLane;
    bool cont = true;
    for(i=myLFLinkLanes.begin(); i!=myLFLinkLanes.end()&&cont; i++) {
        MSLink *link = (*i).myLink;
        bool onLinkEnd = link==0;
        // the vehicle must change the lane on one of the next lanes
        if(!onLinkEnd) {
            if(link->havePriority()) {
                vSafe = (*i).myVLinkPass;
            } else {
                if(link->opened()) {
                    vSafe = (*i).myVLinkPass;
                } else {
                    if(vSafe<myState.mySpeed-myType->decelSpeed()&&link->amYellow()) {
                        vSafe = (*i).myVLinkPass;
                    } else {
                        vSafe = (*i).myVLinkWait;
                        cont = false;
                    }
                }
            }
        } else {
            vSafe = (*i).myVLinkWait;
            cont = false;
            break;
        }
        currentLane = link->getLane();
    }
    // compute vNext in considering dawdling
    double vNext;
    if(myState.speed()==0&&vSafe<myType->accelSpeed(0)) {
        // do not dawdle too much on short segments
        vNext = MAX(double(0), dawdle2( MIN(vSafe, vaccel(myLane)) ));
    } else {
        // lane-change interaction
        ///* !!!!!
        //*/
        vNext = MAX(double(0), dawdle( MIN(vSafe, vaccel(myLane)) ));
    vNext =
        myLaneChangeModel->patchSpeed(
            MAX(0, myState.mySpeed-myType->decel()*MSNet::deltaT()),
            vNext,
            MIN(vSafe, vaccel(myLane)),
            vSafe);
    vNext = MIN3(vNext, vSafe, vaccel(myLane));
    }

    // visit waiting time
    if(vNext<=0.1) {
        myWaitingTime++;
    } else {
        myWaitingTime = 0;
    }
    // call reminders after vNext is set
    double pos = myState.myPos;

    // update position
    myState.myPos += vNext * MSNet::deltaT();
    // update speed
    myState.mySpeed = vNext;
    MSLane *approachedLane = myLane;

    // move the vehicle forward
    size_t no = 0;
    double driven = approachedLane->length() - pos;
    for(i=myLFLinkLanes.begin(); i!=myLFLinkLanes.end()
        &&
        myState.myPos>approachedLane->length();
        ++i) {

        if(approachedLane!=myLane) {
            leaveLaneAtMove(driven);
        }
        MSLink *link = (*i).myLink;
        // check whether the vehicle was allowed to enter lane
        //  otherwise it is decelareted and we do not need to test for it's
        //  approach on the following lanes when a lane changing is performed
        assert(approachedLane!=0);
        myState.myPos -= approachedLane->length();
        assert(myState.myPos>0);
        if(approachedLane!=myLane) {
            enterLaneAtMove(approachedLane, driven);
            driven += approachedLane->length();
        }
        // proceed to the next lane
        if(link!=0/*approachedLane->isLinkEnd(link)*/) {
            approachedLane = link->getViaLane();
            if(approachedLane==0) {
                approachedLane = link->getLane();
            }
        }
        // set information about approaching
        approachedLane->setApproaching(myState.pos(), this);
        no++;
    }
    // set approaching information for consecutive lanes the vehicle may reach in the
    //  next steps
    MSLane *tmpApproached = approachedLane;
    double dist = brakeGap(myState.mySpeed) - driven;
    double tmpPos = myState.myPos + dist;
    for(; dist>0&&tmpApproached->length()<tmpPos&&i!=myLFLinkLanes.end(); i++) {
        MSLink *link = (*i).myLink;
        if(link==0) {
            break;
        }
        tmpPos -= approachedLane->length();
//        dist -= approachedLane->length();
//        if(!tmpApproached->isLinkEnd(link)) {
            tmpApproached = link->getViaLane();
            if(tmpApproached==0) {
                tmpApproached = link->getLane();
            }
  //      }
        tmpApproached->setApproaching(tmpPos, this);
    }
    // needed as the lane changer maybe looks back
    //  !!! needed?
    /*
    MSLane *nextLane = myTarget==0 ? myLane : myTarget;
    double distToEnd = nextLane->length() - myState.myPos;
    if(distToEnd<MSVehicleType::maxLength()) {
        MSLinkCont::iterator link =
            nextLane->succLinkSec( *this, 1, *nextLane );
        if(!nextLane->isLinkEnd(link)) {
            nextLane = (*link)->getViaLane();
            if(nextLane==0) {
                nextLane = (*link)->getLane();
            }
            nextLane->setApproaching(-distToEnd, this);
        }

    }
    */
    // enter lane herein if no push occures (otherwise, do it there)
    if(no==0) {
        workOnMoveReminders( pos, pos + vNext * MSNet::deltaT(), vNext );
    }
    myTarget = approachedLane;
    assert(myTarget!=0);
#ifdef ABS_DEBUG
    if(MSNet::globaltime>MSNet::searchedtime && (myID==MSNet::searched1||myID==MSNet::searched2)) {
        DEBUG_OUT << "moveb/1:" << MSNet::globaltime << ": " << id() << " at " << getLane().id() << ": " << myState.myPos << ", " << myState.mySpeed << endl;
    }
#endif
    assert(myTarget->length()>=myState.myPos);
}


void
MSVehicle::_assertPos() const
{
    assert(myState.myPos<=myLane->length());
}


void
MSVehicle::vsafeCriticalCont( double boundVSafe )
{
#ifdef ABS_DEBUG
    if(MSNet::globaltime>MSNet::searchedtime && (myID==MSNet::searched1||myID==MSNet::searched2)) {
        DEBUG_OUT << "vsafeCriticalCont/" << MSNet::globaltime << ":" << myID << endl;
    }
#endif
    double decelAbility = myType->decel();
    // the vehicle may have just to look into the next lane
    //  compute this information and use it only once in the next loop
    double seen = myLane->length() - myState.myPos;
    MSLane *nextLane = myLane;
    // compute the way the vehicle may drive when accelerating
    double dist = boundVSafe + brakeGap(myLane);
    double vLinkPass = boundVSafe;
    double vLinkWait = vLinkPass;
    size_t view = 1;
    bool nextInternal =
        nextLane->edge().getPurpose()==MSEdge::EDGEFUNCTION_INTERNAL;
    // loop over following lanes
    while(true) {
        // get the next link used
        MSLinkCont::iterator link =
            myLane->succLinkSec( *this, view, *nextLane );
        // check whether the lane is a dead end
        //  (should be valid only on further loop iterations
        if(nextLane->isLinkEnd(link)) {
            double laneEndVSafe =
                vsafe(myState.mySpeed, decelAbility, seen, 0);
            myLFLinkLanes.push_back(
                DriveProcessItem(0,
                    MIN(vLinkPass, laneEndVSafe),
                    MIN(vLinkPass, laneEndVSafe)));
            // the vehicle will not drive further
            return;
        }
        vLinkWait = vLinkPass; // the link was passed
        // if the vehicle drives over the end of the lane, inform the link

        // get the following lane
        nextLane = (*link)->getViaLane();
        if(nextLane==0) {
            nextInternal = false;
            nextLane = (*link)->getLane();
        } else {
            nextInternal = true;
        }

        // compute the velocity to use when the link is not blocked by oter vehicles
            // the vehicle shall be not fastern when reaching the next lane than allowed
        double vmaxNextLane =
            vsafe(myState.mySpeed, decelAbility, seen, nextLane->maxSpeed());

            // the vehicle shall keep a secure distance to its predecessor
            //  (or approach the lane end if the predeccessor is too near)
        const State &nextLanePred = nextLane->myLastState;
        double dist2Pred = seen+nextLanePred.pos()-MSVehicleType::maxLength(); // @!!! die echte Länge des fahrzeugs
        double vsafePredNextLane;
        if(dist2Pred>=0) {
            // leading vehicle is not overlapping
            vsafePredNextLane =
                vsafe(myState.mySpeed, decelAbility, dist2Pred, nextLanePred.speed());
            double predDec = MAX(0, nextLanePred.speed()-decelAbility /* !!! decelAbility of leader! */);
            if(brakeGap(vsafePredNextLane)+vsafePredNextLane*myTau > brakeGap(predDec) + dist2Pred) {

                vsafePredNextLane =
                    MIN(vsafePredNextLane, dist2Pred / MSNet::deltaT());
            }
        } else {
            // leading vehicle is overlapping (stands within the junction)
            vsafePredNextLane =
                vsafe(myState.mySpeed, decelAbility, seen, 0);
        }


            // the vehicle shall not driver over more than two junctions (by now @!!!)
//      double vsafeNextLaneEnd =
//          vsafe(myState.mySpeed, decelAbility, seen+nextLane->length(), 0);

            // compute the velocity to use when the link may be used
        vLinkPass =
            MIN3(vLinkPass, vmaxNextLane, vsafePredNextLane/*, vsafeNextLaneEnd*/);


        // if the link may not be used (is blocked by another vehicle) then let the
        //  vehicle decelerate until the end of the street
        vLinkWait =
            MIN(vLinkWait, vsafe(myState.mySpeed, decelAbility, seen, 0));

        // valid, when a vehicle is not on a priorised lane
        if(!(*link)->havePriority()) {
            // if it has already decelerated to let priorised vehicles pass
            //  and when the distance to the vehicle on the next lane allows moving
            //  (the check whether other incoming vehicles may stop this one is done later)
            // then let it pass
            if(seen<decelAbility&&dist2Pred>0) {
                vLinkPass =
                    MIN3(vLinkPass, vsafePredNextLane, vaccel(myLane)); // otherwise vsafe may become incredibly large
                (*link)->setApproaching(this);
            } else {
                // let it wait in the other cases
                vLinkPass = vLinkWait;
            }
        }
        myLFLinkLanes.push_back(
            DriveProcessItem(*link, vLinkPass, vLinkWait));
        if( vsafePredNextLane>0&&dist-seen>0 ) {
            (*link)->setApproaching(this);
        } else {
            return;
        }
        seen += nextLane->length();
        if(seen>dist) {
            return;
        }
        if(!nextInternal) {
            view++;
        }
    }
}


bool
MSVehicle::endsOn(const MSLane &lane) const
{
    return lane.inEdge(myRoute->getLastEdge());
}

////////////////////////////////////////////////////////////////////////////
/*
void
MSVehicle::moveDecel2laneEnd( MSLane* lane )
{
    double gap = lane->length() - myState.myPos; // !!!

    // Slow down and dawdle.
    double vAccel = vaccel(myLane);
    double vSafe  = vsafe( myState.mySpeed, myType->decel(), gap, 0 );
    double vNext  = dawdle( MIN(vSafe, vAccel) );
    if(vNext<myState.mySpeed-myType->decelSpeed()) {
        vNext = myState.mySpeed-myType->decelSpeed();
    }
    // update position and speed
    myState.myPos  += vNext * MSNet::deltaT();
    myState.mySpeed = vNext;
}
*/
////////////////////////////////////////////////////////////////////////////

/*
void
MSVehicle::moveUpdateState( const State newState )
{
    myState.myPos  += newState.mySpeed * MSNet::deltaT();
    assert( myState.myPos >= 0 );
    myState.mySpeed = newState.mySpeed;
    assert( myState.mySpeed >= 0 );
}
*/
////////////////////////////////////////////////////////////////////////////

void
MSVehicle::moveSetState( const State newState )
{
    myState = newState;
    assert( myState.myPos >= 0 );
    assert( myState.mySpeed >= 0 );
}

/////////////////////////////////////////////////////////////////////////////
/*
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
    double vNext   = dawdle( MIN( vSafe, vAccel ) );
    if(vNext<myState.mySpeed-myType->decelSpeed()) {
        vNext = myState.mySpeed-myType->decelSpeed();
    }
    double nextPos = myState.myPos + vNext * MSNet::deltaT(); // Will be
    // overridden if veh leaves lane.
    return State( nextPos, vNext );
}
*/
/////////////////////////////////////////////////////////////////////////////
/*
MSVehicle::State
MSVehicle::nextStateCompete( MSLane* lane,
                             State predState,
                             double gap2pred ) const
{
    double vAccel   = vaccel( lane );
    double vSafe    = vsafe( myState.mySpeed, myType->decel(),
                             gap2pred, predState.mySpeed );
    double vNext    = dawdle( MIN( vAccel, vSafe ) );
    if(vNext<myState.mySpeed-myType->decelSpeed()) {
        vNext = myState.mySpeed-myType->decelSpeed();
    }
    double nextPos  = myState.myPos + vNext * MSNet::deltaT();
    return State( nextPos, vNext );
}
*/
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
/*
bool
MSVehicle::laneChangeBrake2much( const State brakeState )
{
    // SK-vnext can reduce speed about decel, dawdle about accel.
    double minAllowedNextSpeed =
        MAX( myState.mySpeed - myType->accelPlusDecelSpeed(myState.mySpeed), double( 0 ) );
    if ( brakeState.mySpeed < minAllowedNextSpeed ) {
        return true;
    }

    return false;
}
*/
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

double
MSVehicle::vaccel( const MSLane* lane ) const
{
    // Accelerate until vehicle's max speed reached.
    double vVehicle = MIN( myState.mySpeed + myType->accelSpeed(myState.mySpeed), myType->myMaxSpeed );
    // But don't drive faster than max lane speed.
    return MIN( vVehicle, lane->maxSpeed() );
}

/////////////////////////////////////////////////////////////////////////////

bool
MSVehicle::onAllowed( const MSLane* lane ) const
{
    if(myLane->edge().getPurpose()==MSEdge::EDGEFUNCTION_INTERNAL) {
        return true;
    }
    if(myAllowedLanes.size()==0) {
        return false; // check (was assertion only)!!!
    }
    assert(myAllowedLanes.size()!=0);
    MSEdge::LaneCont::const_iterator compare =
        find( myAllowedLanes[0]->begin(), myAllowedLanes[0]->end(), lane );
    return ( compare != myAllowedLanes[0]->end() );
}


bool
MSVehicle::onAllowed( const MSLane* lane, size_t offset ) const
{
    if(myLane->edge().getPurpose()==MSEdge::EDGEFUNCTION_INTERNAL) {
        return true;
    }
    if(myAllowedLanes.size()==0) {
        return false; // check (was assertion only)!!!
    }
    assert(myAllowedLanes.size()!=0);
    MSEdge::LaneCont::const_iterator compare =
        find( myAllowedLanes[offset]->begin(), myAllowedLanes[offset]->end(), lane );
    return ( compare != myAllowedLanes[offset]->end() );
}


bool
MSVehicle::onAllowed( ) const
{
    if(myLane->edge().getPurpose()==MSEdge::EDGEFUNCTION_INTERNAL) {
        return true;
    }
    if(myAllowedLanes.size()==0) {
        return false; // check (was assertion only)!!!
    }
    assert(myAllowedLanes.size()!=0);
    MSEdge::LaneCont::const_iterator compare =
        find( myAllowedLanes[0]->begin(), myAllowedLanes[0]->end(), myLane );
    return ( compare != myAllowedLanes[0]->end() );
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
                            md.speedSum,
                            md.speedSquareSum,
                            index,
                            false,
                            false,
                            md.enteredLaneWithinIntervall );
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
    md.speedSum              = 0;
    md.speedSquareSum        = 0;
    md.enteredLaneWithinIntervall = 0;
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
        md->speedSum       = speed;
        md->speedSquareSum = speedSquare;
        if ( pos == 0 ) {
            md->enteredAtLaneStart = true;
            md->entryTravelTimestep = entryTimestep;
        }
        else {
            md->enteredAtLaneStart = false;
            md->entryTravelTimestep = 1e-6;
        }
        md->enteredLaneWithinIntervall = true;
    }
}

/////////////////////////////////////////////////////////////////////////////

void
MSVehicle::enterLaneAtMove( MSLane* enteredLane, double driven )
{
#ifdef ABS_DEBUG
    if(MSNet::globaltime>MSNet::searchedtime && (myID==MSNet::searched1||myID==MSNet::searched2)) {
        int textdummy = 0;
    }
#endif
    // save the old work reminders, patching the position information
    // add the information about the new offset to the old lane reminders
    double oldLaneLength = myLane->length();
    OffsetVector::iterator i;
    for(i=myOldLaneMoveReminderOffsets.begin(); i!=myOldLaneMoveReminderOffsets.end(); i++) {
        (*i) += oldLaneLength;
    }
    for(size_t j=0; j<myMoveReminders.size(); j++) {
        myOldLaneMoveReminderOffsets.push_back(oldLaneLength);
    }
    copy(myMoveReminders.begin(), myMoveReminders.end(),
        back_inserter(myOldLaneMoveReminders));
    assert(myOldLaneMoveReminders.size()==myOldLaneMoveReminderOffsets.size());
    // set the entered lane as the current lane
    myLane = enteredLane;
    myTarget = enteredLane;
    // and update the mean data
    double entryTimestep =
        static_cast< double >( MSNet::getInstance()->timestep() ) - 1 +
        driven / myState.mySpeed;
    assert(entryTimestep<MSNet::globaltime);
    updateMeanData( entryTimestep, 0, myState.mySpeed );
    // get new move reminder
    myMoveReminders = enteredLane->getMoveReminders();
    // proceed in route
    const MSEdge &enteredEdge = enteredLane->edge();
    if(enteredEdge.getPurpose()!=MSEdge::EDGEFUNCTION_INTERNAL) {
        MSRouteIterator edgeIt = myCurrEdge;
        while ( *edgeIt != &enteredEdge ) {
            ++edgeIt;
            assert( edgeIt != myRoute->end() );
        }
        myCurrEdge = edgeIt;
    }
    if(MSCORN::wished(MSCORN::CORN_VEHCONTROL_WANTS_DEPARTURE_INFO)) {
        MSNet::getInstance()->getVehicleControl().vehicleMoves(this);
    }
}

/////////////////////////////////////////////////////////////////////////////

void
MSVehicle::enterLaneAtLaneChange( MSLane* enteredLane )
{
    myLane = enteredLane;
    updateMeanData(
        static_cast< double >( MSNet::getInstance()->timestep() ) - 1,
        myState.myPos, 0 );
    // switch to and activate the new lane's reminders
    // keep OldLaneReminders
    myMoveReminders = enteredLane->getMoveReminders();
    myAllowedLanes.clear();
    rebuildAllowedLanes();
    activateRemindersByEmitOrLaneChange();
}

/////////////////////////////////////////////////////////////////////////////

void
MSVehicle::enterLaneAtEmit( MSLane* enteredLane )
{
    myWaitingTime = 0;
    myLane = enteredLane;
    updateMeanData(
        static_cast< double >( MSNet::getInstance()->timestep() ) - 1 ,
        myState.myPos, myState.mySpeed );
    // set and activate the new lane's reminders
    myMoveReminders = enteredLane->getMoveReminders();
    activateRemindersByEmitOrLaneChange();
}

/////////////////////////////////////////////////////////////////////////////

void
MSVehicle::leaveLaneAtMove( double driven )
{
    double leaveTimestep =
        static_cast< double >( MSNet::getInstance()->timestep() ) - 1 +
        ( driven / myState.mySpeed );
    for ( unsigned index = 0; index < myMeanData.size(); ++index ) {
        assert(myMeanData.size()>index);
        MeanDataValues& md = myMeanData[ index ];
        myLane->addVehicleData( leaveTimestep - md.entryContTimestep,
                                MSNet::getInstance()->timestep() -
                                md.entryDiscreteTimestep - 1,
                                md.speedSum,
                                md.speedSquareSum,
                                index,
                                md.enteredAtLaneStart,
                                true,
                                md.enteredLaneWithinIntervall,
                                leaveTimestep - md.entryTravelTimestep );
    }
    if(!myAllowedLanes.empty()) {
        myAllowedLanes.pop_front();
//        myLanesWishes.pop_front();
    }
}

/////////////////////////////////////////////////////////////////////////////

void
MSVehicle::leaveLaneAtLaneChange( void )
{
    double leaveTimestep =
        static_cast< double >( MSNet::getInstance()->timestep() );

    double savePos = myState.myPos; // have to do this due to double-precision errors
    for ( unsigned index = 0; index < myMeanData.size(); ++index ) {
        assert(myMeanData.size()>index);
        MeanDataValues& md = myMeanData[ index ];
        myLane->addVehicleData( leaveTimestep - md.entryContTimestep,
                                MSNet::getInstance()->timestep() -
                                md.entryDiscreteTimestep,
                                md.speedSum,
                                md.speedSquareSum,
                                index,
                                false,
                                false,
                                md.enteredLaneWithinIntervall );
    }
    // dismiss the old lane's reminders
    vector< MSMoveReminder* >::iterator rem;
    for ( rem = myMoveReminders.begin();
          rem != myMoveReminders.end(); ++rem ){
        (*rem)->dismissByLaneChange( *this );
    }
    std::vector<double>::iterator off = myOldLaneMoveReminderOffsets.begin();
    for ( rem = myOldLaneMoveReminders.begin();
          rem != myOldLaneMoveReminders.end(); ++rem, ++off  ){
        myState.myPos += (*off);
        (*rem)->dismissByLaneChange( *this );
        myState.myPos -= (*off);
    }
    myState.myPos = savePos; // have to do this due to double-precision errors
    myMoveReminders.clear();
    myOldLaneMoveReminders.clear();
    myOldLaneMoveReminderOffsets.clear();
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
    return (laneLength - myState.myPos - brakeGap(myLane))
        <= 0;
}


double
MSVehicle::decelAbility() const
{
    return myType->decel() * MSNet::deltaT();
}


double
MSVehicle::accelAbility() const
{
    return myType->accel(myState.mySpeed) * MSNet::deltaT();
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
    return MSNet::getInstance()->getVehicleControl().buildVehicle(
        StringUtils::version1(myID), myRoute, myDesiredDepart+myPeriod,
        myType, myRepetitionNumber-1, myPeriod);
}


bool
MSVehicle::running() const
{
    return myLane!=0;
}


double
MSVehicle::getSecureGap( const MSLane &lane, const MSVehicle &pred ) const
{
    double safeSpace2 = brakeGap(myLane);
    double vSafe = vsafe(0, decelAbility(), 0, pred.speed());
    double safeSpace3 =
        ( (vSafe - pred.speed())
        * ((vSafe+pred.speed()) / 2.0 / (2.0 * MSVehicleType::minDecel()) + MSVehicle::tau()) )
        + pred.speed() * MSVehicle::tau();
    double safeSpace = safeSpace2 > safeSpace3
        ? safeSpace2 : safeSpace3;
    safeSpace = safeSpace > decelAbility()
        ? safeSpace : decelAbility();
    safeSpace += pred.length();
    safeSpace += accelAbility();
    return safeSpace;

}


void
MSVehicle::workOnMoveReminders( double oldPos, double newPos, double newSpeed,
                                MoveOnReminderMode mode )
{
#ifdef ABS_DEBUG
    if(MSNet::globaltime>MSNet::searchedtime && (myID==MSNet::searched1||myID==MSNet::searched2)) {
        int textdummy = 0;
    }
#endif
    movedDistanceDuringStepM = newPos - oldPos;
    assert( movedDistanceDuringStepM >= 0 );
    // This erasure-idiom works for all stl-sequence-containers
    // See Meyers: Effective STL, Item 9
    for ( MoveReminderContIt rem = myMoveReminders.begin();
          rem != myMoveReminders.end(); /* empty */ ) {
        if ( ! (*rem)->isStillActive( *this, oldPos, newPos, newSpeed ) ) {
            rem = myMoveReminders.erase( rem );
        }
        else {
            ++rem;
        }
    }
    if ( mode != CURRENT ) {
        OffsetVector::iterator off = myOldLaneMoveReminderOffsets.begin();
        for ( MoveReminderContIt rem = myOldLaneMoveReminders.begin();
              rem != myOldLaneMoveReminders.end(); /* empty */ ) {
            double oldLaneLength = *off;
            if ( ! (*rem)->isStillActive( *this, oldLaneLength + oldPos,
                                          oldLaneLength + newPos, newSpeed) ) {
                rem = myOldLaneMoveReminders.erase( rem );
                off = myOldLaneMoveReminderOffsets.erase(off);
            }
            else {
                ++rem;
                ++off;
            }
        }
    }
}


void
MSVehicle::activateRemindersByEmitOrLaneChange()
{
    // This erasure-idiom works for all stl-sequence-containers
    // See Meyers: Effective STL, Item 9
    for ( MoveReminderContIt rem = myMoveReminders.begin();
          rem != myMoveReminders.end(); /* empty */ ) {
        if ( ! (*rem)->isActivatedByEmitOrLaneChange( *this ) ) {
            rem = myMoveReminders.erase( rem );
        }
        else {
            ++rem;
        }
    }
}


MSAbstractLaneChangeModel &
MSVehicle::getLaneChangeModel()
{
    return *myLaneChangeModel;
}


const MSAbstractLaneChangeModel &
MSVehicle::getLaneChangeModel() const
{
    return *myLaneChangeModel;
}


size_t
MSVehicle::getWaitingTime() const
{
    return myWaitingTime;
}


bool
MSVehicle::proceedVirtualReturnWhetherEnded(MSVehicleTransfer &securityCheck,
                                            MSEdge *newEdge)
{
    bool _destReached = destReached(newEdge);
    myAllowedLanes.clear(); // !!! not really necessary!?
//    myLanesWishes.clear();
    rebuildAllowedLanes();
    return _destReached;
    /*
    if() {
        // Dismiss reminders by passing them completely.
        return true;
    }
//    myAllowedLanes =
//        ( *myCurrEdge )->allowedLanes( **( myCurrEdge + 1 ) );
    return false;*/
}


void
MSVehicle::onTripEnd(/*MSLane &caller, */bool wasAlreadySet)
{
    double pspeed = myState.mySpeed;
    double pos = myState.myPos;
    double oldPos = pos - pspeed * MSNet::deltaT();
    if ( pos - myType->length() < 0 ) {
        double pdist = (myType->length() + 0.01) - oldPos;
        pspeed = pdist / MSNet::deltaT();
        pos = myType->length() + 0.1;
    }
    pos += myLane->length();
    oldPos += myLane->length();

    // process reminder
        // current
    vector< MSMoveReminder* >::iterator rem;
    for ( rem = myMoveReminders.begin();
          rem != myMoveReminders.end(); ++rem ) {
        // the vehicle may only be at the entry occupancy correction
        if( (*rem)->isStillActive( *this, oldPos, pos, pspeed) ) {
            assert(false);
        }
    }
        // old
    rem = myOldLaneMoveReminders.begin();
    OffsetVector::iterator off = myOldLaneMoveReminderOffsets.begin();
    for (; rem != myOldLaneMoveReminders.end(); ++rem, ++off ) {
        double oldLaneLength = *off;
        if( (*rem)->isStillActive( *this,
                oldPos+oldLaneLength, pos+oldLaneLength, pspeed) ) {
            assert(false);
        }
    }
    // remove from structures to be informed about it
    for(QuitRemindedVector::iterator i=myQuitReminded.begin(); i!=myQuitReminded.end(); ++i) {
        (*i)->removeOnTripEnd(this);
    }
    myQuitReminded.clear();
}


void
MSVehicle::removeApproachingInformationOnKill()
{
    DriveItemVector::iterator i = myLFLinkLanes.begin();
    while(i!=myLFLinkLanes.end()&&(*i).myLink!=0/*&&(*i).myLink->getLane()!=begin&&(*i).myLink->getViaLane()!=begin*/) {
        MSLane *tmp = (*i).myLink->getLane();
        if(tmp!=0) {
            tmp->resetApproacherDistance(this);
        }
        tmp = (*i).myLink->getViaLane();
        if(tmp!=0) {
            tmp->resetApproacherDistance(this);
        }
        ++i;
    }
    /*
    if(myLFLinkLanes.size()==0) {
        return;
    }
    DriveProcessItem &item = *(myLFLinkLanes.begin());
//    if(!myLane->isLinkEnd((*(myLFLinkLanes.begin())).myLink)) {
    if(item.myLink!=0) {
        MSLane *l = item.myLink->getViaLane();
        if(l==0) {
            l = item.myLink->getLane();
        }
        removeApproachingInformationOnKill(l);
    }
}


void
MSVehicle::removeApproachingInformationOnKill(MSLane *begin)
{
#ifdef ABS_DEBUG
    if(MSNet::globaltime>=MSNet::searchedtime && (myID==MSNet::searched1||myID==MSNet::searched2)) {
        DEBUG_OUT << "rmApp:" << MSNet::globaltime << ": " << id() << " at " << getLane().id() << ": " << myState.myPos << ", " << myState.mySpeed << endl;
    }
#endif
    if(begin==myLane&&myLFLinkLanes.size()>0&&myLFLinkLanes[0].myLink!=0) {
        begin = myLFLinkLanes[0].myLink->getViaLane();
        if(begin==0) {
            begin = myLFLinkLanes[0].myLink->getLane();
        }
    }
    DriveItemVector::iterator i = myLFLinkLanes.begin();
    // pass lanes already passed
    while(i!=myLFLinkLanes.end()&&(*i).myLink!=0&&(*i).myLink->getLane()!=begin&&(*i).myLink->getViaLane()!=begin) {
        ++i;
    }
    // reset lanes to come
    while(i!=myLFLinkLanes.end()&&(*i).myLink!=0/*&&(*i).myLink->getLane()!=begin&&(*i).myLink->getViaLane()!=begin*//*) {
        MSLane *tmp = (*i).myLink->getLane();
        if(tmp!=0) {
            tmp->resetApproacherDistance(this);
        }
        tmp = (*i).myLink->getViaLane();
        if(tmp!=0) {
            tmp->resetApproacherDistance(this);
        }
        ++i;
    }
    /*
    for(; i!=myLFLinkLanes.end(); i++) {
        const DriveProcessItem &item = *i;
        // ok, leave if a dead end occured
        //  (the vehicle does not drive any further)
        if((*i).myLink==0) {
            break;
        }
        //
        if(item.myLink->getViaLane()==begin||item.myLink->getLane()==begin) {
            MSLane *l1 = item.myLink->getViaLane();
            MSLane *l2 = item.myLink->getLane();
            if(l1!=0) {
                l1->resetApproacherDistance(this);
            }
            if(l2!=0) {
                l2->resetApproacherDistance(this);
            }
            begin = l2;
        }
    }
    */
}


void
MSVehicle::onDepart()
{
    // check whether the vehicle's departure time shall be saved
    if( MSCORN::wished(MSCORN::CORN_VEH_REALDEPART) ) {
        myDoubleCORNMap[MSCORN::CORN_VEH_REALDEPART] =
            MSNet::getInstance()->getCurrentTimeStep();
    }
    // check whether the vehicle control shall be informed
    if(MSCORN::wished(MSCORN::CORN_VEHCONTROL_WANTS_DEPARTURE_INFO)) {
        MSNet::getInstance()->getVehicleControl().vehicleEmitted(this);
    }

}


void
MSVehicle::quitRemindedEntered(MSVehicleQuitReminded *r)
{
    myQuitReminded.push_back(r);
}


void
MSVehicle::quitRemindedLeft(MSVehicleQuitReminded *r)
{
    QuitRemindedVector::iterator i =
        find(myQuitReminded.begin(), myQuitReminded.end(), r);
    if(i!=myQuitReminded.end()) {
        myQuitReminded.erase(i);
    }
}


double
MSVehicle::getCORNDoubleValue(MSCORN::Function f)
{
    return myDoubleCORNMap[f];
}


bool
MSVehicle::hasCORNDoubleValue(MSCORN::Function f)
{
    return myDoubleCORNMap.find(f)!=myDoubleCORNMap.end();
}


const MSRoute &
MSVehicle::getRoute() const
{
    return *myRoute;
}


const MSRoute &
MSVehicle::getRoute(int index) const
{
    if(index==0) {
        return *myRoute;
    }
    int routeOffset = (int) MSCORN::CORN_P_VEH_OLDROUTE + index - 1;
    std::map<MSCORN::Pointer, void*>::const_iterator i =
        myPointerCORNMap.find((MSCORN::Pointer) routeOffset);
    assert(i!=myPointerCORNMap.end());
    return *((MSRoute*) (*i).second);
}


bool
MSVehicle::replaceRoute(const MSEdgeVector &edges, size_t simTime)
{
#ifdef ABS_DEBUG
    if(MSNet::globaltime>MSNet::searchedtime && (myID==MSNet::searched1||myID==MSNet::searched2)) {
        int textdummy = 0;
        for(MSEdgeVector::const_iterator i=edges.begin(); i!=edges.end(); ++i) {
            cout << (*i)->id() << ", ";
        }
        cout << "-------------" << endl;
        for(MSRouteIterator i2=myRoute->begin(); i2!=myRoute->end(); ++i2) {
            cout << (*i2)->id() << ", ";
        }
        cout << "-------------" << endl;
    }
#endif
    MSRoute *otherr = 0;
    MSEdge *currentEdge = *myCurrEdge;
    // check whether the information shall be saved
    if(MSCORN::wished(MSCORN::CORN_VEH_SAVEREROUTING)) {
        otherr = new MSRoute(*myRoute);
    }
    // try to replace the current route
    bool replaced = myRoute->replaceBy(edges, myCurrEdge);
    if(replaced) {
        // rebuild in-vehicle route information
        myCurrEdge = myRoute->find(currentEdge);
        myAllowedLanes.clear();
        rebuildAllowedLanes();
        // save information that the vehicle was rerouted
        myDoubleCORNMap[MSCORN::CORN_VEH_WASREROUTET] = 1;
            // ... maybe the route information shall be saved for output?
        if( MSCORN::wished(MSCORN::CORN_VEH_SAVEREROUTING)) {
            int routeOffset = (int) MSCORN::CORN_P_VEH_OLDROUTE +
                (int) myDoubleCORNMap[MSCORN::CORN_VEH_NUMBERROUTE];
            myPointerCORNMap[(MSCORN::Pointer) routeOffset] = (void*) otherr;
            int begEdgeOffset = (int) MSCORN::CORN_P_VEH_ROUTE_BEGIN_EDGE +
                (int) myDoubleCORNMap[MSCORN::CORN_VEH_NUMBERROUTE];
            myPointerCORNMap[(MSCORN::Pointer) begEdgeOffset] = (void*) *myCurrEdge;
            int timeOffset = (int) MSCORN::CORN_VEH_REROUTE_TIME +
                (int) myDoubleCORNMap[MSCORN::CORN_VEH_NUMBERROUTE];
            myDoubleCORNMap[(MSCORN::Function) timeOffset] = (double) simTime;
        }
        myDoubleCORNMap[MSCORN::CORN_VEH_LASTREROUTEOFFSET] = 0;
        myDoubleCORNMap[MSCORN::CORN_VEH_NUMBERROUTE] =
            myDoubleCORNMap[MSCORN::CORN_VEH_NUMBERROUTE] + 1;
    }
#ifdef ABS_DEBUG
    if(MSNet::globaltime>MSNet::searchedtime && (myID==MSNet::searched1||myID==MSNet::searched2)) {
        int textdummy = 0;
        for(MSRouteIterator i=myRoute->begin(); i!=myRoute->end(); ++i) {
            cout << (*i)->id() << ", ";
        }
        cout << "-------------" << endl;
    }
#endif
    return replaced;
}


const MSVehicleType &
MSVehicle::getVehicleType() const
{
    return *myType;
}


void
MSVehicle::rebuildAllowedLanes()
{
    double dist = 0;
    // check what was already computed
    for(NextAllowedLanes::const_iterator i=myAllowedLanes.begin(); i!=myAllowedLanes.end(); ++i) {
        dist += ((*(*i))[0])->length();
    }
    double MIN_DIST = 3000;
    if(dist<MIN_DIST) {
        size_t pos = distance(myRoute->begin(), myCurrEdge) + myAllowedLanes.size();
        if(pos>=myRoute->size()-1) {
            return;
        }
        const MSEdge::LaneCont *al = ( *myRoute )[pos]->allowedLanes( *( *myRoute )[pos+1] );
        while(al!=0&&dist<MIN_DIST&&pos<myRoute->size()-1/*&&*ce!=myRoute->getLastEdge()*/) {
            assert(al!=0);
            myAllowedLanes.push_back(al);
//            myLanesWishes.push_back(vector<float>(al->size(), 0));
//            ++ce;
            pos++;
            if(pos<myRoute->size()-1) {
                dist += ((*al)[0])->length();
                al = ( *myRoute )[pos]->allowedLanes( *( *myRoute )[pos+1] );
            }
        }
/*

        MSRouteIterator ce = myCurrEdge + myAllowedLanes.size();
        if(/*ce==myRoute->end()-2||*//*ce==myRoute->end()-1/* || *ce==myRoute->getLastEdge()*//*) {
            return;
        }
        const MSEdge::LaneCont *al = ( *ce )->allowedLanes( **( ce + 1 ) );
        while(al!=0&&dist<MIN_DIST&&ce!=myRoute->end()-1/*&&*ce!=myRoute->getLastEdge()*//*) {
            assert(al!=0);
            myAllowedLanes.push_back(al);
//            myLanesWishes.push_back(vector<float>(al->size(), 0));
            ++ce;
            if(ce!=myRoute->end()-1) {
                dist += ((*al)[0])->length();
                al = ( *ce )->allowedLanes( **( ce + 1 ) );
            }
        }*/
    }
}


int
MSVehicle::countAllowedContinuations(const MSLane *lane) const
{
    int ret = 0;
    double MIN_DIST = 3000;
    MSRouteIterator ce = myCurrEdge;
    double dist = -myState.pos();
    while(dist<MIN_DIST&&ce!=myRoute->end()-2/**ce!=myRoute->getLastEdge()*/) {
        const MSEdge::LaneCont *al = ( *ce )->allowedLanes( **( ce + 1 ) );
        if(al==0) {
            return ret;
        }
        MSEdge::LaneCont::const_iterator i =
            find(al->begin(), al->end(), lane);
        if(i==al->end()) {
            return ret;
        }
        ret++;
        lane = (*(lane->succLinkOneLane(*( ce + 1 ), *lane)))->getLane();
        assert(al!=0);
        ++ce;
        dist += ((*al)[0])->length();
    }
    return ret;
}


double
MSVehicle::allowedContinuationsLength(const MSLane *lane) const
{
    double dist = lane->length()-myState.pos();
    MSRouteIterator ce = myCurrEdge;
    while(ce!=myRoute->end()-2/**ce!=myRoute->getLastEdge()*/) {
        const MSEdge::LaneCont *al = ( *ce )->allowedLanes( **( ce + 1 ) );
        if(al==0) {
            return dist;
        }
        MSEdge::LaneCont::const_iterator i =
            find(al->begin(), al->end(), lane);
        if(i==al->end()) {
            return dist;
        }
        if(ce!=myCurrEdge) {
            dist += ((*al)[0])->length();
        }
        lane = (*(lane->succLinkOneLane(*( ce + 1 ), *lane)))->getLane();
        assert(al!=0);
        ++ce;
//        dist += ((*al)[0])->length();
    }
    return dist;
}


void
MSVehicle::writeXMLRoute(std::ostream &os, int index) const
{
    MSRoute *route2Write = myRoute;
    // check if a previous route shall be written
    os << "      <route";
    if(index>=0) {
        // write edge on which the vehicle was when the route was valid
        std::map<MSCORN::Pointer, void*>::const_iterator j =
            myPointerCORNMap.find(
                (MSCORN::Pointer) (MSCORN::CORN_P_VEH_ROUTE_BEGIN_EDGE+index));
        os << " replacedOnEdge=\"" << ((MSEdge*) (*j).second)->id() << "\" ";
        // write the time at which the route was replaced
        std::map<MSCORN::Function, double>::const_iterator j2 =
            myDoubleCORNMap.find(
                (MSCORN::Function) (MSCORN::CORN_VEH_REROUTE_TIME+index));
        os << " replacedAtTime=\"" << toString<size_t>((size_t) (*j2).second) << "\"";
        // get the route
        j = myPointerCORNMap.find((MSCORN::Pointer) (MSCORN::CORN_P_VEH_OLDROUTE+index));
        assert(j!=myPointerCORNMap.end());
        route2Write = (MSRoute*) j->second;
    }
    os << ">";
    // write the route
    route2Write->writeEdgeIDs(os);
    os << "</route>" << endl;
}

void
MSVehicle::interactWith(const std::vector<MSVehicle*> &vehicles)
{
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#ifdef DISABLE_INLINE
#include "MSVehicle.icc"
#endif

// Local Variables:
// mode:C++
// End:
