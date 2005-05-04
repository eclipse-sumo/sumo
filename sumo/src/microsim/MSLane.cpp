/***************************************************************************
                          MSLane.cpp  -  The place where Vehicles
                          operate.
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
// Revision 1.53  2005/05/04 07:55:28  dkrajzew
// added the possibility to load lane geometries into the non-gui simulation; simulation speedup due to avoiding multiplication with 1;
//
// Revision 1.52  2005/02/17 10:33:34  dkrajzew
// code beautifying;
// Linux building patched;
// warnings removed;
// new configuration usage within guisim
//
// Revision 1.51  2005/02/01 10:10:40  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.50  2005/01/27 14:23:34  dkrajzew
// patched undefined state of teleporter if a negative number was given
//
// Revision 1.49  2004/12/16 12:25:26  dkrajzew
// started a better vss handling
//
// Revision 1.48  2004/11/23 10:20:09  dkrajzew
// new detectors and tls usage applied; debugging
//
// Revision 1.47  2004/08/02 12:14:29  dkrajzew
// raw-output extracted; debugging of collision handling
//
// Revision 1.46  2004/07/02 09:58:08  dkrajzew
// MeanData refactored (moved to microsim/output); numerical id for online routing added
//
// Revision 1.45  2004/04/23 12:38:43  dkrajzew
// warnings and errors are now reported to MsgHandler, not cerr
//
// Revision 1.44  2004/03/19 13:09:40  dkrajzew
// debugging
//
// Revision 1.43  2004/01/26 07:45:36  dkrajzew
// added the forgotton vehicle departure method call
//
// Revision 1.42  2004/01/12 15:03:40  dkrajzew
// removed some unneeded debug-variables
//
// Revision 1.41  2003/12/12 12:37:42  dkrajzew
// proper usage of lane states applied; scheduling of vehicles into the beamer on push failures added
//
// Revision 1.40  2003/12/11 06:31:45  dkrajzew
// implemented MSVehicleControl as the instance responsible for vehicles
//
// Revision 1.39  2003/12/04 13:30:41  dkrajzew
// work on internal lanes
//
// Revision 1.38  2003/11/26 10:59:42  dkrajzew
// messages from the simulation are now also passed to the message handler; debug couts removed
//
// Revision 1.37  2003/11/20 14:43:45  dkrajzew
// push() debugged; dead code removed
//
// Revision 1.36  2003/11/19 16:39:11  roessel
// If vehicle reaches it's destination edge, modify it's speed as if it
// entered the new lane completely. Rationale: If the vehicle leaves the
// simulation and entered the destination lane only partially, it will
// never get removed from a previous lane's detector that may contain
// this vehicle partially.
//
// Revision 1.35  2003/11/12 13:50:30  dkrajzew
// MSLink-members are now secured from the outer world
//
// Revision 1.34  2003/10/27 10:49:21  dkrajzew
// keeping the pointer to a deleted vehicle - bug patched
//
// Revision 1.33  2003/10/22 11:26:07  dkrajzew
// removeing from lane corrected
//
// Revision 1.32  2003/10/22 07:02:13  dkrajzew
// patching of lane states on force vehicle removal added
//
// Revision 1.31  2003/10/20 07:59:43  dkrajzew
// grid lock dissolving by vehicle teleportation added
//
// Revision 1.30  2003/10/15 12:11:56  dkrajzew
// removed the prohibition of overtaking on the right side; false deletion
//  patched
//
// Revision 1.29  2003/10/06 07:41:35  dkrajzew
// MSLane::push changed due to some inproper Vissim-behaviour; now removes
//  a vehicle and reports an error if push fails
//
// Revision 1.28  2003/09/05 15:10:29  dkrajzew
// first tries for an implementation of aggregated views
//
// Revision 1.27  2003/08/20 11:44:11  dkrajzew
// min and max-functions moved to an own definition file
//
// Revision 1.26  2003/07/30 09:08:09  dkrajzew
// forgotten setting of an infinite distance to the last vehicle patched
//
// Revision 1.25  2003/07/22 15:07:40  dkrajzew
// warnings removed
//
// Revision 1.24  2003/07/18 12:36:29  dkrajzew
// missing reset of vehicle nuzmber after lane change added
//
// Revision 1.23  2003/07/16 15:28:00  dkrajzew
// MSEmitControl now only simulates lanes which do have vehicles; the edges
//  do not go through the lanes, the EdgeControl does
//
// Revision 1.22  2003/07/07 08:18:43  dkrajzew
// due to an ugly inheritance between lanes, sourcelanes and their
//  gui-versions, a method for the retrival of a GUILaneWrapper had to be
//  added;we should redesign it in the future
//
// Revision 1.21  2003/06/24 08:09:28  dkrajzew
// implemented SystemFrame and applied the changes to all applications
//
// Revision 1.20  2003/06/19 10:57:32  dkrajzew
// division by zero in meandata computation patched
//
// Revision 1.19  2003/06/18 11:30:26  dkrajzew
// debug outputs now use a DEBUG_OUT macro instead of cout; this shall ease
//  the search for further couts which must be redirected to the messaaging
//  subsystem
//
// Revision 1.18  2003/06/04 16:29:42  roessel
// Vehicles are emtted completely on current lane in emitTry()
//  (needed for detectors). Vehicles that will be destroyed because they
//  reached their destination will dismiss their active reminders in push().
//
// Revision 1.17  2003/05/21 16:20:44  dkrajzew
// further work detectors
//
// Revision 1.16  2003/05/21 15:15:41  dkrajzew
// yellow lights implemented (vehicle movements debugged
//
// Revision 1.15  2003/05/20 09:31:46  dkrajzew
// emission debugged; movement model reimplemented (seems ok); detector output
// debugged; setting and retrieval of some parameter added
//
// Revision 1.14  2003/04/16 10:05:03  dkrajzew
// uah, debugging
//
// Revision 1.13  2003/04/14 08:32:57  dkrajzew
// some further bugs removed
//
// Revision 1.11  2003/04/01 15:13:22  dkrajzew
// bug in emission of vehicles from MSTriggeredSource removed
//
// Revision 1.10  2003/03/17 14:15:55  dkrajzew
// first steps of network reinitialisation implemented
//
// Revision 1.9  2003/03/03 14:56:20  dkrajzew
// some debugging; new detector types added; actuated traffic lights added
//
// Revision 1.8  2003/02/07 10:41:50  dkrajzew
// updated
//
// Revision 1.7  2002/10/29 10:43:38  dkrajzew
// bug of trying to set the destination lane for vehicles that vanish before
// they reach the point of halt removed
//
// Revision 1.6  2002/10/28 12:59:38  dkrajzew
// vehicles are now deleted whe the tour is over
//
// Revision 1.5  2002/10/18 11:51:03  dkrajzew
// breakRequest or driveRequest may be set, althoug no first vehicle exists
//  due to regarding a longer break gap...; assertion in moveFirst replaced
//  by a check with a normal exit
//
// Revision 1.4  2002/10/17 13:35:23  dkrajzew
// insecure usage of potentially null-link-lanes patched
//
// Revision 1.3  2002/10/17 10:43:35  dkrajzew
// error of setting of link-driverequests out of a vehicles route repaired
//
// Revision 1.2  2002/10/16 16:43:48  dkrajzew
// regard of artifactsarising from traffic lights implemented; debugged
//
// Revision 1.1  2002/10/16 14:48:26  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.14  2002/10/15 10:24:30  roessel
// MSLane::MeanData constructor checks now for illegal intervals.
//
// Revision 1.13  2002/09/25 17:14:42  roessel
// MeanData calculation and output implemented.
//
// Revision 1.12  2002/07/31 17:33:00  roessel
// Changes since sourceforge cvs request.
//
// Revision 1.15  2002/07/26 11:10:50  dkrajzew
// unset myAllowedLanes when departing from a source debugged; gap on
// URGENT_LANECHANGE_WISH added
//
// Revision 1.14  2002/07/24 16:29:40  croessel
// New method isEmissionSuccess(), used by MSTriggeredSource.
//
// Revision 1.13  2002/07/08 05:07:49  dkrajzew
// MSVC++-compilation problems removed
//
// Revision 1.12  2002/07/03 15:48:40  croessel
// Implementation of findNeigh changed.
// Call to MSVehicle::move adapted to new signature.
// Emit methods use now MSVehicle::safeEmitGap instead of safeGap.
// decel2much uses now MSVehicle::hasSafeGap instead of safeGap.
//
// Revision 1.11  2002/06/20 11:14:13  dkrajzew
// False assertion corrected
//
// Revision 1.10  2002/06/19 15:12:00  croessel
// In moveExceptFirst: Check for timeheadway < deltaT situations.
//
// Revision 1.9  2002/05/29 17:06:03  croessel
// Inlined some methods. See the .icc files.
//
// Revision 1.8  2002/04/24 13:06:47  croessel
// Changed signature of void detectCollisions()
//  to void detectCollisions(MSNet::Time )
//
// Revision 1.7  2002/04/18 14:51:11  croessel
// In setDriveRequests(): set gap to UINT_MAX instead of 0 for vehicles
// without a predecessor.
//
// Revision 1.6  2002/04/18 14:05:21  croessel
// Changed detectCollisions() output from pointer-address to vehicle-id.
//
// Revision 1.5  2002/04/17 14:44:32  croessel
// Forgot to reset visited lanes (see previous revision).
//
// Revision 1.4  2002/04/17 14:02:11  croessel
// Bugfix in setLookForwardState: myGap may be < 0 in the PRED_ON_SUCC
// state if pred just entered the succ-lane from another source lane. In
// this case the lane's first vehicle shouldn't leave this lane.
//
// Revision 1.3  2002/04/11 15:25:55  croessel
// Changed float to double.
//
// Revision 1.2  2002/04/11 12:32:07  croessel
// Added new lookForwardState "URGENT_LANECHANGE_WISH" for vehicles that
// may drive beyond the lane but are not on a lane that is linked to
// their next route-edge. A second succLink method, named succLinkSec was
// needed.
//
// Revision 1.1.1.1  2002/04/08 07:21:23  traffic
// new project name
//
// Revision 2.7  2002/03/27 12:11:09  croessel
// In constructor initialization of member myLastVeh added.
//
// Revision 2.6  2002/03/27 10:35:10  croessel
// moveExceptFirst(...): Last vehicle state must be buffered
// for multi-lane traffic too.
//
// Revision 2.5  2002/03/20 15:58:32  croessel
// Return to previous revision.
//
// Revision 2.3  2002/03/13 17:40:59  croessel
// Calculation of rearPos in setLookForwardState() fixed by introducing
// the new member myLastVeh in addition to myLastVehState. We need both,
// the state and the length of the vehicle.
//
// Revision 2.2  2002/03/13 16:56:35  croessel
// Changed the simpleOutput to XMLOutput by introducing nested classes
// XMLOut. Output is now indented.
//
// Revision 2.1  2002/03/06 15:57:25  croessel
// Modifications to setLookForwardState() so that overlapping vehicles
// from the succeeding lane won't cause collisions.
//
// Revision 2.0  2002/02/14 14:43:16  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.16  2002/02/13 10:05:26  croessel
// Front-gap calculation in emit() and emitTry() modified by usage of
// MSVehicleType::maxLength() and MSVehicleType::minDecel().
//
// Revision 1.15  2002/02/05 13:51:52  croessel
// GPL-Notice included.
// In *.cpp files also config.h included.
//
// Revision 1.14  2002/02/05 11:47:45  croessel
// Adaption of method-name-change in MSVehicle.
//
// Revision 1.13  2002/01/30 16:05:51  croessel
// Bug in detectCollisions() fixed. myVehicles must contain two or more
// vehicles to enter the for-loop.
//
// Revision 1.12  2002/01/24 18:30:40  croessel
// Changes due to renaming of MSVehicle's gap methods.
//
// Revision 1.11  2002/01/23 11:16:28  croessel
// emit() rewritten to avoid redundant code and make things clearer. This
// leads to some overloaded emitTry()'s and a new enoughSpace() method.
//
// Revision 1.10  2002/01/16 15:41:42  croessel
// New implementation of emit(). Should be collision free now. Introduced
// a new protected method "bool emitTry()" for convenience.
//
// Revision 1.9  2002/01/09 14:58:42  croessel
// Added MSLane::Link::setPriority( bool ) for StopLights to modify the
// link's priority. Used to implement the "green arrow".
//
// Revision 1.8  2001/12/19 16:29:31  croessel
// New std::-files included.
//
// Revision 1.7  2001/12/13 15:59:47  croessel
// In request(): Adaption due to new junction hierarchy.
//
// Revision 1.6  2001/12/06 13:13:25  traffic
// Junction types extracted into a separate class
//
// Revision 1.5  2001/11/21 15:21:01  croessel
// Bug fixes.
// Adapted setLookForwardState() to the new member myLastState and the
// MSVehicle::nextState to changed parameter list.
//
// Revision 1.4  2001/11/15 17:17:35  croessel
// Outcommented the inclusion of the inline *.iC files. Currently not needed.
// Vehicle-State introduced. Simulation is now independant of vehicle's
// speed. Still, the position is fundamental, also a gap between
// vehicles. But that's it.
//
// Revision 1.3  2001/11/14 15:47:34  croessel
// Merged the diffs between the .C and .cpp versions. Numerous changes
// in MSLane, MSVehicle and MSJunction.
//
// Revision 1.2  2001/11/14 10:49:06  croessel
// CR-line-end removed.
//
// Revision 1.1  2001/10/24 07:12:36  traffic
// new extension
//
// Revision 1.7  2001/10/23 09:31:00  traffic
// parser bugs removed
//
// Revision 1.5  2001/09/06 15:47:08  croessel
// Numerous changes during debugging session.
//
// Revision 1.4  2001/08/16 13:11:46  traffic
// minor MSVC++-problems solved
//
// Revision 1.3  2001/07/25 12:17:39  traffic
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
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <helpers/PreStartInitialised.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StdDefs.h>
#include "MSVehicle.h"
#include "MSVehicleType.h"
#include "MSEdge.h"
#include "MSJunction.h"
#include "MSLogicJunction.h"
#include "output/MSInductLoop.h"
#include "MSLink.h"
#include "MSLane.h"
#include "MSVehicleTransfer.h"
#include "MSGlobals.h"
#include "MSVehicleControl.h"
#include <cmath>
#include <bitset>
#include <iostream>
#include <cassert>
#include <functional>
#include <algorithm>
#include <iterator>
#include <exception>
#include <climits>
#include <utils/common/MsgHandler.h>
#include <utils/convert/ToString.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * some definitions (debugging only)
 * ======================================================================= */
#define DEBUG_OUT cout


/* =========================================================================
 * static member definitions
 * ======================================================================= */
MSLane::DictType MSLane::myDict;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * methods of MSLane::PosGreater
 * ----------------------------------------------------------------------- */
MSLane::PosGreater::result_type
MSLane::PosGreater::operator() (first_argument_type veh1,
                                second_argument_type veh2) const
{
    return veh1->pos() > veh2->pos();
}


/* -------------------------------------------------------------------------
 * methods of MSLane
 * ----------------------------------------------------------------------- */
MSLane::~MSLane()
{
    for(MSLinkCont::iterator i=myLinks.begin(); i!=myLinks.end(); i++) {
        delete *i;
    }
    // TODO
}

/////////////////////////////////////////////////////////////////////////////

MSLane::MSLane( MSNet &net,
                string id,
                double maxSpeed,
                double length,
                MSEdge* edge,
                size_t numericalID,
				const Position2DVector &shape
                )  :
    PreStartInitialised(net),
    myApproaching(0),
    myID( id ),
    myNumericalID(numericalID),
    myVehicles(),
    myLength( length ),
    myEdge( edge ),
    myMaxSpeed( maxSpeed ),
    myVehBuffer( 0 ),
    myMeanData(),
	myShape(shape)
{
    myLastState = MSVehicle::State(10000, 10000);
    myFirstUnsafe = 0;
}

/////////////////////////////////////////////////////////////////////////////

void
MSLane::initialize( MSLinkCont* links )
{
    myLinks = *links;
    delete links;
}

/////////////////////////////////////////////////////////////////////////////

void
MSLane::resetApproacherDistance()
{
    myBackDistance = 100000;
    myApproaching = 0;
}

void
MSLane::resetApproacherDistance(MSVehicle *v)
{
    if(myApproaching!=v) {
        return;
    }
    myBackDistance = 100000;
    myApproaching = 0;
}

void
MSLane::moveNonCritical()
{
    assert(myVehicles.size()!=0);
    assert(myVehicles.size()==myUseDefinition->noVehicles);
    // Set the information about the last vehicle
    myLastState = (*myVehicles.begin())->myState;
    myFirstUnsafe = 0;

    // Move vehicles except first and all vehicles that may reach something
    //  that forces them to stop
    VehCont::iterator lastBeforeEnd = myVehicles.end() - 1;
    VehCont::iterator veh;

    for ( veh = myVehicles.begin();
            !(*veh)->reachingCritical(myLength) && veh != lastBeforeEnd;
            ++veh,++myFirstUnsafe ) {
        VehCont::const_iterator pred( veh + 1 );
        ( *veh )->move( this, *pred, 0);
//        ( *veh )->meanDataMove();
        // Check for timeheadway < deltaT
        MSVehicle *vehicle = (*veh);
        MSVehicle *predec = (*pred);
        assert(&vehicle->getLane()==this);
        assert(&predec->getLane()==this);
        /*
        assert( ( *veh )->pos() < ( *pred )->pos() );
        assert( ( *veh )->pos() <= myLength );
        */
    }
    assert(myVehicles.size()==myUseDefinition->noVehicles);
}


void
MSLane::moveCritical()
{
    assert(myVehicles.size()!=0);
    assert(myVehicles.size()==myUseDefinition->noVehicles);
    VehCont::iterator lastBeforeEnd = myVehicles.end() - 1;
    VehCont::iterator veh;
    // Move all next vehicles beside the first
    for ( veh=myVehicles.begin()+myFirstUnsafe;veh != lastBeforeEnd; ++veh ) {
        VehCont::const_iterator pred( veh + 1 );
        ( *veh )->moveRegardingCritical( this, *pred, 0);
//        ( *veh )->meanDataMove();
        // Check for timeheadway < deltaT
        // Check for timeheadway < deltaT
        MSVehicle *vehicle = (*veh);
        MSVehicle *predec = (*pred);
        assert(&vehicle->getLane()==this);
        assert(&predec->getLane()==this);
        assert( ( *veh )->pos() < ( *pred )->pos() );
        assert( ( *veh )->pos() <= myLength );
    }
    ( *veh )->moveRegardingCritical( this, 0, 0 );
//    ( *veh )->meanDataMove();
    assert( ( *veh )->pos() <= myLength );
    assert(myVehicles.size()==myUseDefinition->noVehicles);
    assert(&( *veh )->getLane()==this);
}

/////////////////////////////////////////////////////////////////////////////

void
MSLane::detectCollisions( SUMOTime timestep )
{
    assert(myVehicles.size()==myUseDefinition->noVehicles);
    if ( myVehicles.size() < 2 ) {
        return;
    }

    VehCont::iterator lastVeh = myVehicles.end() - 1;
    for ( VehCont::iterator veh = myVehicles.begin();
          veh != lastVeh; ) {

        VehCont::iterator pred = veh + 1;
        double gap = ( *pred )->pos() - ( *pred )->length() - ( *veh )->pos();
#ifdef ABS_DEBUG
    if(MSNet::globaltime>=21868 && ((*veh)->id()==MSNet::searched1||(*veh)->id()==MSNet::searched2)) {
        DEBUG_OUT << gap << endl;
    }
#endif
        if ( gap < 0 ) {
#ifdef ABS_DEBUG
    if(MSNet::globaltime>MSNet::searchedtime-5 && ((*veh)->id()==MSNet::searched1||(*veh)->id()==MSNet::searched2)) {
        int blb = 0;
    }
#endif
            MSVehicle *predV = *pred;
            MSVehicle *vehV = *veh;
            MsgHandler *handler = 0;
            if(!OptionsSubSys::getOptions().getBool("quit-on-accident")) {
                handler = MsgHandler::getWarningInstance();
            } else {
                handler = MsgHandler::getErrorInstance();
            }
            handler->inform(
                string("MSLane::detectCollision: Collision of ")
                + ( *veh )->id() + string(" with ") + ( *pred )->id()
                + string(" on MSLane ") + myID
                + string(" during timestep ") + toString<int>(timestep));
//            DEBUG_OUT << ( *veh )->id() << ":" << ( *veh )->pos() << ", " << ( *veh )->speed() << endl;
//            DEBUG_OUT << ( *pred )->id() << ":" << ( *pred )->pos() << ", " << ( *pred )->speed() << endl;
            if(OptionsSubSys::getOptions().getBool("quit-on-accident")) {
                throw ProcessError();
            } else {
                vehV->leaveLaneAtLaneChange();
                vehV->onTripEnd(/* *this*/);
                resetApproacherDistance(); // !!! correct? is it (both lines) really necessary during this simulation part?
                vehV->removeApproachingInformationOnKill(/*this*/);
                MSVehicleTransfer::getInstance()->addVeh(vehV);
            }
            veh = myVehicles.erase(veh); // remove current vehicle
            lastVeh = myVehicles.end() - 1;
            myUseDefinition->noVehicles--;
            if(veh==myVehicles.end()) {
                break;
            }
        } else {
            ++veh;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////

bool
MSLane::emit( MSVehicle& veh )
{
    // Here the emission starts
    if ( empty() ) {

        return emitTry( veh );
    }

    // Try to emit as last veh. (in driving direction)
    VehCont::iterator leaderIt = myVehicles.begin();
    if ( emitTry( veh, leaderIt ) ) {

        return true;
    }

    // if there is only one veh on this lane, try to
    // emit in front of this veh. (leader becomes follower)
    if ( leaderIt + 1 ==  myVehicles.end() ) {

        return emitTry( leaderIt, veh );
    }

    // At least two vehicles on lane.
    // iterate over follow/leader -pairs
    VehCont::iterator followIt = leaderIt;
    ++leaderIt;
    for (;;) {

        // try to emit between follower and leader
        if ( emitTry( followIt, veh, leaderIt ) ) {

            return true;
        }

        // if leader is the first veh on this lane, try
        // to emit in front of it.
        if ( leaderIt + 1 == myVehicles.end() ) {

            return emitTry( leaderIt, veh );
        }

        // iterate
        ++leaderIt;
        ++followIt;
    }
}

/////////////////////////////////////////////////////////////////////////////

bool
MSLane::isEmissionSuccess( MSVehicle* aVehicle )
{
//    aVehicle->departLane();
    MSLane::VehCont::iterator predIt =
        find_if( myVehicles.begin(), myVehicles.end(),
                 bind2nd( VehPosition(), aVehicle->pos() ) );

    if ( predIt != myVehicles.end() ) {

        MSVehicle* pred = *predIt;
        if ( ! aVehicle->isInsertTimeHeadWayCond( *pred ) ||
             ! aVehicle->isInsertBrakeCond( *pred ) ) {

            return false;
        }

        // emit
        aVehicle->enterLaneAtEmit( this );
        myVehicles.insert( predIt, aVehicle );
        myUseDefinition->noVehicles++;
        assert(myVehicles.size()==myUseDefinition->noVehicles);
        return true;
    }
    // emit
    aVehicle->enterLaneAtEmit( this );
    myVehicles.push_back( aVehicle );
    myUseDefinition->noVehicles++;
    assert(myVehicles.size()==myUseDefinition->noVehicles);
    return true;
}

/////////////////////////////////////////////////////////////////////////////

bool
MSLane::emitTry( MSVehicle& veh )
{
    double safeSpace =
        myApproaching==0
            ? 0
            : myApproaching->getSecureGap(*this, veh);
    safeSpace = MAX2( safeSpace, veh.length() );
    if ( safeSpace<length() ) {
        MSVehicle::State state;
        state.setPos(safeSpace);
        veh.moveSetState( state );
        veh.enterLaneAtEmit( this );
        myVehicles.push_front( &veh );
        myUseDefinition->noVehicles++;
        assert(myUseDefinition->noVehicles==myVehicles.size());

#ifdef ABS_DEBUG
    if(MSNet::searched1==veh.id()||MSNet::searched2==veh.id()) {
        DEBUG_OUT << "Using emitTry( MSVehicle& veh )/2:" << MSNet::globaltime << endl;
    }
#endif

        return true;
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////

bool
MSLane::emitTry( MSVehicle& veh, VehCont::iterator leaderIt )
{
    if(myApproaching==0) {
        // emission as last car (in driving direction)
        MSVehicle *leader = *leaderIt;
        // get invoked vehicles' positions
        double leaderPos = (*leaderIt)->pos() - (*leaderIt)->length();
        // get secure gaps
        double frontGapNeeded = veh.getSecureGap(*this, *leader);
        // compute needed room
        double frontMax = leaderPos - frontGapNeeded;
        // check whether there is enough room
        if(frontMax>0) {
            // emit vehicle if so
            MSVehicle::State state;
            state.setPos(frontMax);
            veh.moveSetState( state );
            veh.enterLaneAtEmit( this );
            myVehicles.push_front( &veh );
            myUseDefinition->noVehicles++;
            assert(myUseDefinition->noVehicles==myVehicles.size());

#ifdef ABS_DEBUG
    if(MSNet::searched1==veh.id()||MSNet::searched2==veh.id()) {
        DEBUG_OUT << "Using emitTry( MSVehicle& veh, VehCont::iterator leaderIt )/1:" << MSNet::globaltime << endl;
    }
#endif

            return true;
        }
        return false;
    } else {
        // another vehicle is approaching this lane
        MSVehicle *leader = *leaderIt;
        MSVehicle *follow = myApproaching;
        // get invoked vehicles' positions
        double followPos = follow->pos();
        double leaderPos = leader->pos() - leader->length();
        // get secure gaps
        double frontGapNeeded = veh.getSecureGap(*this, *leader);
        double backGapNeeded = follow->getSecureGap(*this, veh);
        // compute needed room
        double frontMax = leaderPos - frontGapNeeded;
        double backMin = followPos + backGapNeeded + veh.length();
        // check whether there is enough room
        if(frontMax>0 && backMin<frontMax) {
            // emit vehicle if so
            MSVehicle::State state;
            state.setPos((frontMax+backMin)/2.0);
            veh.moveSetState( state );
            veh.enterLaneAtEmit( this );
            myVehicles.insert( leaderIt, &veh );
            myUseDefinition->noVehicles++;
            assert(myUseDefinition->noVehicles==myVehicles.size());
#ifdef ABS_DEBUG
    if(MSNet::searched1==veh.id()||MSNet::searched2==veh.id()) {
        DEBUG_OUT << "Using emitTry( MSVehicle& veh, VehCont::iterator leaderIt )/2:" << MSNet::globaltime << endl;
    }
#endif

            return true;
        }
        return false;
    }
}

/////////////////////////////////////////////////////////////////////////////

bool
MSLane::emitTry( VehCont::iterator followIt, MSVehicle& veh )
{
    // emission as first car (in driving direction)
    MSVehicle *follow = *followIt;
    // get invoked vehicles' positions
    double followPos = follow->pos();
    // get secure gaps
    double backGapNeeded = follow->getSecureGap(*this, veh);
    // compute needed room
    double backMin = followPos + backGapNeeded + veh.length();
    // check whether there is enough room
    if(backMin<length()) {
        // emit vehicle if so
        MSVehicle::State state;
        state.setPos(backMin);
        veh.moveSetState( state );
        veh.enterLaneAtEmit( this );
        myVehicles.push_back( &veh );
        myUseDefinition->noVehicles++;
        assert(myUseDefinition->noVehicles==myVehicles.size());
#ifdef ABS_DEBUG
    if(MSNet::searched1==veh.id()||MSNet::searched2==veh.id()) {
        DEBUG_OUT << "Using emitTry( VehCont::iterator followIt, MSVehicle& veh )/1:" << MSNet::globaltime << endl;
    }
#endif

        return true;
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////

bool
MSLane::emitTry( VehCont::iterator followIt, MSVehicle& veh,
                 VehCont::iterator leaderIt )
{
    MSVehicle *leader = *leaderIt;
    MSVehicle *follow = *followIt;
    // get invoked vehicles' positions
    double followPos = follow->pos();
    double leaderPos = leader->pos() - leader->length();
    // get secure gaps
    double frontGapNeeded = veh.getSecureGap(*this, *leader);
    double backGapNeeded = follow->getSecureGap(*this, veh);
    // compute needed room
    double frontMax = leaderPos - frontGapNeeded;
    double backMin = followPos + backGapNeeded + veh.length();
    // check whether there is enough room
    if(frontMax>0 && backMin<frontMax) {
        // emit vehicle if so
        MSVehicle::State state;
        state.setPos((frontMax + backMin) / 2.0);
        veh.moveSetState( state );
        veh.enterLaneAtEmit( this );
        myVehicles.insert( leaderIt, &veh );
        myUseDefinition->noVehicles++;
        assert(myUseDefinition->noVehicles==myVehicles.size());
#ifdef ABS_DEBUG
    if(MSNet::searched1==veh.id()||MSNet::searched2==veh.id()) {
        DEBUG_OUT << "Using emitTry( followIt, veh, leaderIt )/1:" << MSNet::globaltime << endl;
    }
#endif

        return true;
    }
    return false;
}

void
MSLane::setCritical()
{
    assert(myVehicles.size()==myUseDefinition->noVehicles);
    // move critical vehicles
    int to_pop = 0;
    bool lastPopped = false;
    VehCont::iterator i;
    for(i=myVehicles.begin() + myFirstUnsafe; i!=myVehicles.end(); i++) {
        MSVehicle *v = *i;
        (*i)->moveFirstChecked();
        MSLane *target = (*i)->getTargetLane();
        if(target!=this) {
            assert(to_pop==0||lastPopped);
            lastPopped = true;
            to_pop++;
        } else {
            lastPopped = false;
        }
//        (*i)->_assertPos();

    }
    for(int j = 0; j<to_pop; j++) {
        MSVehicle *v = *(myVehicles.end() - 1);
        MSVehicle *p = pop();
        assert(v==p);
        MSLane *target = p->getTargetLane();
        target->push(p);
    }
    assert(myVehicles.size()==myUseDefinition->noVehicles);
    // check whether the lane is free
    if(myVehicles.size()==0) {
        myLastState = MSVehicle::State(10000, 10000);
        myFirstUnsafe = 0;//myVehicles.size();
    }
    if(myVehicles.size()>0) {
        if( MSGlobals::gTimeToGridlock>0
            &&
            (*(myVehicles.end()-1))->getWaitingTime()>MSGlobals::gTimeToGridlock) {

            MSVehicleTransfer *vt = MSVehicleTransfer::getInstance();
            MSVehicle *veh = removeFirstVehicle();
            veh->removeApproachingInformationOnKill();
            vt->addVeh(veh);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////

bool
MSLane::dictionary(string id, MSLane* ptr)
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

MSLane*
MSLane::dictionary(string id)
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
MSLane::clear()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete (*i).second;
    }
    myDict.clear();
}

/////////////////////////////////////////////////////////////////////////////

bool
MSLane::push(MSVehicle* veh)
{
#ifdef ABS_DEBUG
    if(myVehBuffer!=0) {
        DEBUG_OUT << "Push Failed on Lane:" << myID << endl;
        DEBUG_OUT << myVehBuffer->id() << ", " << myVehBuffer->pos() << ", " << myVehBuffer->speed() << endl;
        DEBUG_OUT << veh->id() << ", " << veh->pos() << ", " << veh->speed() << endl;
    }
#endif
    MSVehicle *last = myVehicles.size()!=0
        ? myVehicles.front()
        : 0;

    // Insert vehicle only if it's destination isn't reached.
    //  and it does not collide with previous
    if( myVehBuffer != 0 || (last!=0 && last->pos() < veh->pos()) ) {
        MSVehicle *prev = myVehBuffer!=0
            ? myVehBuffer : last;
        WRITE_WARNING(string("Vehicle '") + veh->id()+ string("' beamed due to a collision on push!\n")+ string("  Lane: '") + id() + string("', previous vehicle: '")+ prev->id() + string("', time: ")+ toString<SUMOTime>(MSNet::getInstance()->getCurrentTimeStep())+ string("."));
        veh->onTripEnd(/* *this*/);
        resetApproacherDistance();
        veh->removeApproachingInformationOnKill(/*this*/);
        MSVehicleTransfer::getInstance()->addVeh(veh);
//        MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(veh);
        return true;
    }
    // check whether the vehicle has ended his route
    if ( ! veh->destReached( myEdge ) ) { // adjusts vehicles routeIterator
        myVehBuffer = veh;
        veh->enterLaneAtMove( this, SPEED2DIST(veh->speed()) - veh->pos() );
        double pspeed = veh->speed();
        double oldPos = veh->pos() - SPEED2DIST(veh->speed());
        veh->workOnMoveReminders( oldPos, veh->pos(), pspeed );
        veh->_assertPos();
//        setApproaching(veh->pos(), veh);
        return false;
    } else {
        veh->onTripEnd(/* *this*/);
        resetApproacherDistance();
        veh->removeApproachingInformationOnKill(/*this*/);
        MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(veh);
        return true;
    }
}

/////////////////////////////////////////////////////////////////////////////

MSVehicle*
MSLane::pop()
{
    assert( ! myVehicles.empty() );
    assert(myVehicles.size()==myUseDefinition->noVehicles);
    MSVehicle* first = myVehicles.back( );
    first->leaveLaneAtMove( SPEED2DIST(first->speed())/* - first->pos()*/ );
    myVehicles.pop_back();
    myUseDefinition->noVehicles--;
    assert(myVehicles.size()==myUseDefinition->noVehicles);
    if(myVehicles.size()==0) {
        myLastState = MSVehicle::State(10000, 10000);
    }
    return first;
}


bool
MSLane::appropriate(const MSVehicle *veh)
{
    if(myEdge->getPurpose()==MSEdge::EDGEFUNCTION_INTERNAL) {

        return true;
    } else {

        MSLinkCont::iterator link = succLinkSec( *veh, 1, *this );
        return ( link != myLinks.end() );
    }
}

//////////////////////////////////////////////////////////////////////////

void
MSLane::integrateNewVehicle()
{
    if ( myVehBuffer ) {
        assert(myUseDefinition->noVehicles==myVehicles.size());
        myVehicles.push_front( myVehBuffer );
        myVehBuffer = 0;
        myUseDefinition->noVehicles++;
        assert(myUseDefinition->noVehicles==myVehicles.size());
    }
}

/////////////////////////////////////////////////////////////////////////////

MSLinkCont::const_iterator
MSLane::succLinkOneLane(const MSEdge* nRouteEdge,
                        const MSLane& succLinkSource) const
{
    // the link must be from a lane to the right or left from the current lane
    //  we have to do it via the edge
    for ( MSLinkCont::const_iterator link = succLinkSource.myLinks.begin();
          link != succLinkSource.myLinks.end() ; ++link ) {

        if ( ( *link )->getLane()!=0 && ( *link )->getLane()->myEdge == nRouteEdge ) {
            return link;
        }
    }
    return succLinkSource.myLinks.end();
}

/////////////////////////////////////////////////////////////////////////////

bool
MSLane::isLinkEnd(MSLinkCont::const_iterator &i) const
{
    return i==myLinks.end();
}

bool
MSLane::isLinkEnd(MSLinkCont::iterator &i)
{
    return i==myLinks.end();
}

/////////////////////////////////////////////////////////////////////////////

bool
MSLane::inEdge(const MSEdge *edge) const
{
    return myEdge==edge;
}

/////////////////////////////////////////////////////////////////////////////

const MSVehicle * const
MSLane::getLastVehicle() const
{
    if(myVehicles.size()==0) {
        return 0;
    }
    return *myVehicles.begin();
}

/////////////////////////////////////////////////////////////////////////////

MSLinkCont::iterator
MSLane::succLinkSec(const MSVehicle& veh, unsigned int nRouteSuccs,
                    MSLane& succLinkSource)
{
    const MSEdge* nRouteEdge = veh.succEdge( nRouteSuccs );
    // check whether the vehicle tried to look beyond its route
    if(nRouteEdge==0 ) {
        return succLinkSource.myLinks.end();
    }

    // the link must be from a lane to the right or left from the current lane
    //  we have to do it via the edge
    for ( MSLinkCont::iterator link = succLinkSource.myLinks.begin();
            link != succLinkSource.myLinks.end() ; ++link ) {

        if ( ( *link )->getLane()!=0 && ( *link )->getLane()->myEdge == nRouteEdge ) {
            return link;
        }
    }
    return succLinkSource.myLinks.end();
}


/////////////////////////////////////////////////////////////////////////////

const MSVehicle*
MSLane::findNeigh( MSVehicle* veh,
                   MSEdge::LaneCont::const_iterator first,
                   MSEdge::LaneCont::const_iterator last )
{
    MSVehicle* neighbour = veh;
    double vNeighEqual( 0 );

    for ( MSEdge::LaneCont::const_iterator neighLane = first;
          neighLane != last; ++neighLane ) {

        VehCont::const_iterator tmpNeighbour =
            find_if( ( *neighLane )->myVehicles.begin(),
                     ( *neighLane )->myVehicles.end(),
                     bind2nd( PosGreater(), veh ) );
        if ( tmpNeighbour == ( *neighLane )->myVehicles.end() ) {

            continue;
        }

        // neighbour found
        if ( (*tmpNeighbour)-> congested() ) {
            continue;
        }

        double tmpVNeighEqual = veh->vNeighEqualPos( **tmpNeighbour );
        if ( neighbour == veh || tmpVNeighEqual < vNeighEqual ) {

            neighbour   = *tmpNeighbour;
            vNeighEqual = tmpVNeighEqual;
        }
    }
    return neighbour;
}

/////////////////////////////////////////////////////////////////////////////

void
MSLane::resetMeanData( unsigned index )
{
    assert(index<myMeanData.size());
    MSLaneMeanDataValues& md = myMeanData[ index ];
    md.nVehEntireLane = 0;
    md.nVehContributed = 0;
    md.nVehLeftLane = 0;
    md.nVehEnteredLane = 0;
    md.contTimestepSum = 0;
    md.discreteTimestepSum = 0;
    md.speedSum = 0;
    md.speedSquareSum = 0;
    md.traveltimeStepSum = 0;
    md.haltSum = 0;

}

/////////////////////////////////////////////////////////////////////////////
/*
void
MSLane::addVehicleData( double contTimesteps,
                        unsigned discreteTimesteps,
                        double speedSum,
                        double speedSquareSum,
                        unsigned index,
                        bool hasFinishedEntireLane,
                        bool hasLeftLane,
                        bool hasEnteredLane,
                        double travelTimesteps)
{
    /*
    assert(index<myMeanData.size());
    myMeanData[index].addVehicleData(contTimesteps, discreteTimesteps,
        speedSum, speedSquareSum, hasFinishedEntireLane,
        hasLeftLane, hasEnteredLane, travelTimesteps);
        */
//}


void
MSLane::addMean2(double v)
{
    for(size_t i=0; i<myMeanData.size(); i++) {
        myMeanData[i].nVehContributed++;
        myMeanData[i].speedSum += v;
        if(v<0.1) { // !!! swell
            myMeanData[i].haltSum++;
        }
    }
}


/////////////////////////////////////////////////////////////////////////////
/*
void
MSLane::collectVehicleData( unsigned index )
{
    /*
    for ( VehCont::iterator it = myVehicles.begin();
          it != myVehicles.end(); ++it ) {

        (*it)->dumpData( index );
    }
    */
//}


/////////////////////////////////////////////////////////////////////////////

ostream&
operator<<( ostream& os, const MSLane& lane )
{
    os << "MSLane: Id = " << lane.myID << endl;
    return os;
}


/////////////////////////////////////////////////////////////////////////////

void
MSLane::setLinkPriorities(const std::bitset<64> &prios,
                          const std::bitset<64> &yellowMask,
                          size_t &beginPos)
{
    for(MSLinkCont::iterator i=myLinks.begin(); i!=myLinks.end()&&beginPos<64; i++) {// !!! hell happens when i>=64
        (*i)->setPriority(prios.test(beginPos), yellowMask.test(beginPos));
        beginPos++;
    }
}

/////////////////////////////////////////////////////////////////////////////

const MSLinkCont &
MSLane::getLinkCont() const
{
    return myLinks;
}

/////////////////////////////////////////////////////////////////////////////

void
MSLane::init(MSNet &net)
{
    // reset mean data information
    myMeanData.clear();
    size_t noIntervals = net.getNDumpIntervalls();
    myMeanData.insert( myMeanData.end(), noIntervals, MSLaneMeanDataValues() );
    // empty vehicle buffers
    myVehicles.clear();
    myTmpVehicles.clear();
    // remove information about the approaching vehicle
    myApproaching = 0;
}


const std::string &
MSLane::id() const
{
    return myID;
}


void
MSLane::releaseVehicles()
{
}


const MSLane::VehCont &
MSLane::getVehiclesSecure()
{
    return myVehicles;
}


void
MSLane::swapAfterLaneChange()
{
    myVehicles = myTmpVehicles;
    myTmpVehicles.clear();
    myUseDefinition->noVehicles = myVehicles.size();
    if(myVehicles.size()==0) {
        myLastState = MSVehicle::State(10000, 10000);
        myFirstUnsafe = 0;
    }
    assert(myUseDefinition->noVehicles==myVehicles.size());
}


void
MSLane::setApproaching(double dist, MSVehicle *veh)
{
    myBackDistance = dist;
    myApproaching = veh;
}


MSLane::VehCont::const_iterator
MSLane::findNextVehicleByPosition(double pos) const
{
    assert(pos<myLength);
    // returns if no vehicle is available
    if(myVehicles.size()==0) {
        return myVehicles.end();
    }
    // some kind of a binary search
    size_t off1 = 0;
    size_t off2 = myVehicles.size() - 1;
    while(true) {
        size_t middle = (off1+off2)/2;
        MSVehicle *v1 = myVehicles[middle];
        if(v1->pos()>pos) {
            off2 = middle;
        } else if(v1->pos()<pos) {
            off1 = middle;
        }
        if(off1==off2) {
            return myVehicles.begin() + off1;
        }
    }
}


MSLane::VehCont::const_iterator
MSLane::findPrevVehicleByPosition(const VehCont::const_iterator &beginAt,
                                  double pos) const
{
    assert(pos<myLength);
    // returns if no vehicle is available
    if(myVehicles.size()==0) {
        return myVehicles.end();
    }
    // some kind of a binary search
    size_t off1 = distance(myVehicles.begin(), beginAt);
    size_t off2 = myVehicles.size() - 1;
    while(true) {
        size_t middle = (off1+off2)/2;
        MSVehicle *v1 = myVehicles[middle];
        if(v1->pos()>pos) {
            off2 = middle;
        } else if(v1->pos()<pos) {
            off1 = middle;
        }
        if(off1==off2) {
            // there may be no vehicle before
            if(off1==0) {
                return myVehicles.end();
            }
            off1--;
            return myVehicles.begin() + off1;
        }
    }
}


void
MSLane::addMoveReminder( MSMoveReminder* rem )
{
    moveRemindersM.push_back( rem );
}


MSLane::MoveReminderCont
MSLane::getMoveReminders( void )
{
    return moveRemindersM;
}


GUILaneWrapper *
MSLane::buildLaneWrapper(GUIGlObjectStorage &)
{
    throw "Only within the gui-version";
}


void
MSLane::init(MSEdgeControl &ctrl, MSEdgeControl::LaneUsage *useDefinition)
{
    myUseDefinition = useDefinition;
}


size_t
MSLane::getVehicleNumber() const
{
    return myUseDefinition->noVehicles;
}


MSVehicle *
MSLane::removeFirstVehicle()
{
    MSVehicle *veh = *(myVehicles.end()-1);
    veh->leaveLaneAtLaneChange();
    myVehicles.erase(myVehicles.end()-1);
    myUseDefinition->noVehicles--;
    return veh;
}


size_t
MSLane::getNumericalID() const
{
    return myNumericalID;
}


void
MSLane::add(MSMeanData_Net *newMeanData)
{
    myMeanData.push_back(MSLaneMeanDataValues());
}


MSVehicle *
MSLane::getLastVehicle(MSLaneChanger &lc) const
{
    if(myVehicles.size()==0) {
        return 0;
    }
    return *myVehicles.begin();
}


const std::string &
MSLane::getID() const
{
    return myID;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#ifdef DISABLE_INLINE
#include "MSLane.icc"
#endif

// Local Variables:
// mode:C++
// End:
