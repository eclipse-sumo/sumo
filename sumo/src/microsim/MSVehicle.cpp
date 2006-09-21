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
// Revision 1.88  2006/09/21 09:52:02  dkrajzew
// debugging
//
// Revision 1.87  2006/09/21 09:47:06  dkrajzew
// debugging vehicle movement; insertion of Danilot's current sources
//
// Revision 1.86  2006/09/18 12:31:12  dkrajzew
// patched building under Linux
//
// Revision 1.85  2006/09/18 10:09:04  dkrajzew
// c2c added; vehicle classes added
//
// Revision 1.84  2006/08/02 11:58:23  dkrajzew
// first try to make junctions tls-aware
//
// Revision 1.83  2006/08/01 07:00:32  dkrajzew
// removed unneeded API parts
//
// Revision 1.82  2006/07/07 11:51:52  dkrajzew
// further work on lane changing
//
// Revision 1.81  2006/07/06 07:33:22  dkrajzew
// rertrieval-methods have the "get" prependix; EmitControl has no dictionary; MSVehicle is completely scheduled by MSVehicleControl; new lanechanging algorithm
//
// Revision 1.80  2006/05/22 13:27:15  dkrajzew
// debugged phone device usage
//
// Revision 1.79  2006/05/15 05:50:40  dkrajzew
// began with the extraction of the car-following-model from MSVehicle
//
// Revision 1.79  2006/05/08 11:09:36  dkrajzew
// began with the extraction of the car-following-model from MSVehicle
//
// Revision 1.78  2006/04/18 08:13:52  dkrajzew
// debugging rerouting
//
// Revision 1.77  2006/04/07 05:28:07  dkrajzew
// debugging
//
// Revision 1.76  2006/04/05 05:27:34  dkrajzew
// retrieval of microsim ids is now also done using getID() instead of id()
//
// Revision 1.75  2006/03/17 09:01:12  dkrajzew
// .icc-files removed
//
// Revision 1.74  2006/02/27 12:08:14  dkrajzew
// raknet-support added
//
// Revision 1.73  2006/02/23 11:31:09  dkrajzew
// TO SS2 output added
//
// Revision 1.72  2006/01/26 08:30:29  dkrajzew
// patched MSEdge in order to work with a generic router
//
// Revision 1.71  2006/01/23 10:25:29  dkrajzew
// post-release changes
//
// Revision 1.70  2006/01/09 11:53:00  dkrajzew
// bus stops implemented
//
// Revision 1.69  2005/12/01 07:37:35  dkrajzew
// introducing bus stops: eased building vehicles; vehicles may now have nested elements
//
// Revision 1.68  2005/11/09 06:39:38  dkrajzew
// usage of internal lanes is now optional at building
//
// Revision 1.67  2005/10/10 11:58:14  dkrajzew
// debugging
//
// Revision 1.66  2005/10/07 11:37:45  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.65  2005/09/23 13:16:40  dkrajzew
// debugging the building process
//
// Revision 1.64  2005/09/22 13:45:51  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.63  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.62  2005/07/12 12:06:11  dkrajzew
// first devices (mobile phones) added
//
// Revision 1.61  2005/05/04 08:34:20  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added; new mead data functionality; lane-changing offset computation debugged; simulation speed-up by avoiding multiplication with 1
//
// Revision 1.60  2005/02/17 10:33:38  dkrajzew
// code beautifying;
// Linux building patched;
// warnings removed;
// new configuration usage within guisim
//
// Revision 1.59  2005/02/01 10:10:42  dkrajzew
// got rid of MSNet::Time
//
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
// Introduced dontMoveGap to handle SUMORealing-point-inaccuracy. Vehicles
// will keep their state if gap2pred is smaller.
//
// Revision 1.2  2002/04/11 15:25:56  croessel
// Changed SUMOReal to SUMOReal.
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include "MSLane.h"
#include "MSVehicle.h"
#include "MSEdge.h"
#include "MSVehicleType.h"
#include "MSNet.h"
#include "MSRoute.h"
#include "MSLinkCont.h"
#include "MSVehicleQuitReminded.h"
#include "MSDebugHelper.h"
#include <utils/common/StringUtils.h>
#include <utils/common/StdDefs.h>
#include <utils/gfx/RGBColor.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSGlobals.h>
#include <iostream>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <map>
#include "MSMoveReminder.h"
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>
#include "lanechanging/MSLCM_Krauss.h"
#include "lanechanging/MSLCM_DK2004.h"
#include <utils/common/ToString.h>
#include <utils/common/FileHelpers.h>
#include <utils/bindevice/BinaryInputDevice.h>
#include "trigger/MSBusStop.h"


#include "devices/MSDevice_CPhone.h"

#ifdef GUI_DEBUG
#include <utils/gui/div/GUIGlobalSelection.h>
#include <guisim/GUIVehicle.h>
#endif

#ifdef ABS_DEBUG
#include "MSDebugHelper.h"
#endif

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


#define BUS_STOP_OFFSET 0.5


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * some definitions (debugging only)
 * ======================================================================= */
#define DEBUG_OUT cout


/* =========================================================================
 * method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * methods of MSVehicle::State
 * ----------------------------------------------------------------------- */
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

SUMOReal
MSVehicle::State::pos() const
{
    return myPos;
}

/////////////////////////////////////////////////////////////////////////////

MSVehicle::State::State( SUMOReal pos, SUMOReal speed ) :
    myPos( pos ), mySpeed( speed )
{
}


/* -------------------------------------------------------------------------
 * MSVehicle-methods
 * ----------------------------------------------------------------------- */
MSVehicle::~MSVehicle()
{
    // remove move reminder
    for(QuitRemindedVector::iterator i=myQuitReminded.begin(); i!=myQuitReminded.end(); ++i) {
        (*i)->removeOnTripEnd(this);
    }
    //myWaitingPersons.clear();
    myRoute->decReferenceCnt();
    if(!myRoute->inFurtherUse()) {
        MSRoute::erase(myRoute->getID());
    }
    // delete
        // prior routes
    if(myDoubleCORNMap.find(MSCORN::CORN_VEH_NUMBERROUTE)!=myDoubleCORNMap.end()) {
        int noReroutes = (int) myDoubleCORNMap[MSCORN::CORN_VEH_NUMBERROUTE];
        for(int i=0; i<noReroutes; ++i) {
	  delete (MSRoute*) myPointerCORNMap[(MSCORN::Pointer) (i+noReroutes)];
        }
    }
        // devices
    {
        // cell phones
        if(myDoubleCORNMap.find((MSCORN::Function) MSCORN::CORN_VEH_DEV_NO_CPHONE)!=myDoubleCORNMap.end()) {
            size_t no = (size_t) myDoubleCORNMap[(MSCORN::Function) MSCORN::CORN_VEH_DEV_NO_CPHONE];
            for(size_t np=0; np<no; np++) {
        	    delete ((MSDevice_CPhone*) myPointerCORNMap[(MSCORN::Pointer) (MSCORN::CORN_P_VEH_DEV_CPHONE+np)]);
            }
        }
    }
    delete myLaneChangeModel;
	myNeighbors.clear();
	infoCont.clear();
	clusterCont.clear();
}

/////////////////////////////////////////////////////////////////////////////

MSVehicle::MSVehicle( string id,
                      MSRoute* route,
                      SUMOTime departTime,
                      const MSVehicleType* type,
                      size_t noMeanData,
                      int repNo, int repOffset) :
#ifdef RAKNET_DEMO
    Vehicle(),
#endif
    myType(type),
    myLastLaneChangeOffset(0),
    myTarget(0),
    myWaitingTime( 0 ),
    myRepetitionNumber(repNo),
    myPeriod(repOffset),
    myID(id),
    myRoute(route),
    myDesiredDepart(departTime),
    myState(0, 0),
    myLane( 0 ),
    myCurrEdge(0),
    myAllowedLanes(0),
    myMoveReminders( 0 ),
    myOldLaneMoveReminders( 0 ),
    myOldLaneMoveReminderOffsets( 0 ),
	lastUp(0),
	equipped(false),
	clusterId(-1)
{
    myCurrEdge = myRoute->begin();
    rebuildAllowedLanes();
    myLaneChangeModel = new MSLCM_DK2004(*this);
    // init cell phones
    initDevices();
}


void
MSVehicle::initDevices()
{
    OptionsCont &oc = OptionsSubSys::getOptions();
    // cell phones
    if(oc.getFloat("device.cell-phone.probability")!=0||oc.isSet("device.cell-phone.knownveh")) {
        bool t1 = (((SUMOReal) rand()/(SUMOReal) RAND_MAX))<=oc.getFloat("device.cell-phone.probability");
        bool t2 = oc.isSet("device.cell-phone.knownveh") && OptionsSubSys::helper_CSVOptionMatches("device.cell-phone.knownveh", myID);
        if(t1||t2) {
            int noCellPhones = (int) (((SUMOReal) rand()/(SUMOReal) RAND_MAX)
                * (oc.getFloat("device.cell-phone.amount.max") - oc.getFloat("device.cell-phone.amount.min"))
                + oc.getFloat("device.cell-phone.amount.min"));
            myDoubleCORNMap[(MSCORN::Function) MSCORN::CORN_VEH_DEV_NO_CPHONE] =
                (SUMOReal) noCellPhones;
			std::string phoneid;
            for(int np=0; np<noCellPhones; np++) {
    		    myPointerCORNMap[(MSCORN::Pointer) (MSCORN::CORN_P_VEH_DEV_CPHONE+np)] =
                    (void*) new MSDevice_CPhone(*this);
				phoneid = this->getID();
				phoneid.append( toString( np ) );
				MSDevice_CPhone * tmp =	((MSDevice_CPhone*) myPointerCORNMap[(MSCORN::Pointer) (MSCORN::CORN_P_VEH_DEV_CPHONE+np)]);
				tmp->setId( phoneid );
            }
        }
    }
    // c2c communication
    if(oc.getFloat("device.c2x.probability")!=0||oc.isSet("device.c2x.knownveh")) {
        bool t1 = (((SUMOReal) rand()/(SUMOReal) RAND_MAX))<=oc.getFloat("device.c2x.probability");
        bool t2 = oc.isSet("device.c2x.knownveh") && OptionsSubSys::helper_CSVOptionMatches("device.c2x.knownveh", myID);
        if(t1||t2) {
            equipped = true;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////

const MSEdge &
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

SUMOTime
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
    } else {
        return 0;
    }
}

/////////////////////////////////////////////////////////////////////////////

bool
MSVehicle::hasSuccEdge(unsigned int nSuccs) const
{
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
        return false;
    }
}

/////////////////////////////////////////////////////////////////////////////

bool
MSVehicle::endsOn(const MSLane &lane) const
{
    return lane.inEdge(myRoute->getLastEdge());
}

/////////////////////////////////////////////////////////////////////////////

void
MSVehicle::move(MSLane* lane, const MSVehicle* pred, const MSVehicle* neigh)
{
    // reset move information
    myTarget = 0;
#ifdef ABS_DEBUG
    if(debug_globaltime>debug_searchedtime && (myID==debug_searched1||myID==debug_searched2)) {
        DEBUG_OUT << "movea/1:" << debug_globaltime << ": " << myID << " at " << myLane->getID() << ": " << getPositionOnLane() << ", " << getSpeed() << endl;
    }
#endif
    // compute gap to use
    SUMOReal gap = gap2pred(*pred);
    if(gap<0.1) {
        assert(gap>-0.1);
        gap = 0;
    }
    SUMOReal vSafe  = myType->ffeV(myState.mySpeed, gap, pred->getSpeed());
    if(neigh!=0&&neigh->getSpeed()>60./3.6) {
        SUMOReal mgap = MAX2((SUMOReal) 0, neigh->getPositionOnLane()-neigh->getLength()-getPositionOnLane());
        SUMOReal nVSafe = myType->ffeV(myState.mySpeed, mgap, neigh->getSpeed());
        vSafe = MIN2(vSafe, nVSafe);
    }


    if(!myStops.empty()) {
        if(myStops.begin()->reached) {
            if(myStops.begin()->duration==0) {
                if(myStops.begin()->busstop!=0) {
                    myStops.begin()->busstop->leaveFrom(this);
                }
                myStops.pop_front();
            } else {
                myStops.begin()->duration--;
                myTarget = myLane;
                myState.mySpeed = 0;
                myLane->addMean2(0, myType->getLength());
                return; // !!!detectore etc?
            }
        } else {
            if(myStops.begin()->lane==myLane) {
                Stop &bstop = *myStops.begin();
                SUMOReal endPos = bstop.pos;
                bool busStopsMustHaveSpace = true;
                if(bstop.busstop!=0) {
                    endPos = bstop.busstop->getLastFreePos();
                    if(endPos-5.<bstop.busstop->getBeginLanePosition()) { // !!! explicite offset
                        busStopsMustHaveSpace = false;
                    }
                }
                if(myState.pos()>=endPos-BUS_STOP_OFFSET&&busStopsMustHaveSpace) {
                    bstop.reached = true;
                    if(bstop.busstop!=0) {
                        bstop.busstop->enter(this, myState.pos(), myState.pos()-myType->getLength());
                    }
                }

                vSafe = MIN2(vSafe, myType->ffeS( myState.mySpeed, endPos-myState.pos()) );
            }
        }
    }


    SUMOReal vNext = myType->dawdle( MIN3( lane->maxSpeed(), myType->maxNextSpeed(myState.mySpeed), vSafe ) );
    vNext =
        myLaneChangeModel->patchSpeed(
            MAX2((SUMOReal) 0, ACCEL2SPEED(myState.mySpeed-myType->getMaxDecel())), //!!! reverify
            vNext,
            MIN3(vSafe, myLane->maxSpeed(), myType->maxNextSpeed(myState.mySpeed)),//vaccel(myState.mySpeed, myLane->maxSpeed())),
            vSafe);
    vNext = MIN4(vNext, vSafe, myLane->maxSpeed(), myType->maxNextSpeed(myState.mySpeed));//vaccel(myState.mySpeed, myLane->maxSpeed()));

    SUMOReal predDec = pred->getSpeedAfterMaxDecel(pred->getSpeed()); //!!!!q//-decelAbility() /* !!! decelAbility of leader! */);
    if(myType->brakeGap(vNext)+vNext*myType->getTau() > myType->brakeGap(predDec) + gap) {

        vNext = MIN2(vNext, DIST2SPEED(gap));
    }

    vNext = MAX2((SUMOReal) 0, vNext);
    if(vNext<=0.1) {
        myWaitingTime++;
    } else {
        myWaitingTime = 0;
    }

    // call reminders after vNext is set
    workOnMoveReminders( myState.myPos,
                         myState.myPos + SPEED2DIST(vNext), vNext );
    // update position and speed
    myState.myPos  += SPEED2DIST(vNext);
    assert( myState.myPos < lane->length() );
    myState.mySpeed = vNext;
#ifdef ABS_DEBUG
    if(debug_globaltime>debug_searchedtime && (myID==debug_searched1||myID==debug_searched2)) {
        DEBUG_OUT << "movea/2:" << debug_globaltime << ": " << myID << " at " << myLane->getID() << ": " << getPositionOnLane() << ", " << getSpeed() << endl;
    }
#endif
    //@ to be optimized (move to somewhere else)
    if(hasCORNDoubleValue(MSCORN::CORN_VEH_LASTREROUTEOFFSET)) {
        myDoubleCORNMap[MSCORN::CORN_VEH_LASTREROUTEOFFSET] =
            myDoubleCORNMap[MSCORN::CORN_VEH_LASTREROUTEOFFSET] + 1;
    }
    //@ to be optimized (move to somewhere else)
    myLane->addMean2(vNext, myType->getLength());
#ifdef RAKNET_DEMO
    setPosition(position().x(), 0, position().y());
#endif
}


void
MSVehicle::moveRegardingCritical(MSLane* lane,
                                 const MSVehicle* pred,
                                 const MSVehicle* neigh )
{
#ifdef GUI_DEBUG
    if(gSelected.isSelected(GLO_VEHICLE, static_cast<GUIVehicle*>(this)->getGlID())) {
        int blb = 0;
    }
#endif
#ifdef ABS_DEBUG
    if(debug_globaltime>debug_searchedtime && (myID==debug_searched1||myID==debug_searched2)) {
        DEBUG_OUT << "moveb/1:" << debug_globaltime << ": " << myID << " at " << myLane->getID() << ": " << getPositionOnLane() << ", " << getSpeed() << endl;
    }
#endif
    myLFLinkLanes.clear();
    // check whether the vehicle is not on an appropriate lane
    if(!myLane->appropriate(this)) {
        // decelerate to lane end when yes
        SUMOReal vWish = myType->ffeS(myState.mySpeed, myLane->length()-myState.myPos);
        if(pred!=0) {
            vWish = MIN2(vWish, myType->ffeV(myState.mySpeed, gap2pred(*pred), pred->getSpeed()) );
        }
        // !!! check whether the vehicle wants to stop somewhere
        if(!myStops.empty()&&myStops.begin()->lane->getEdge()==lane->getEdge()) {
            SUMOReal seen = lane->length() - myState.pos();
            SUMOReal vsafeStop = myType->ffeS(myState.mySpeed, seen-(lane->length()-myStops.begin()->pos));
            vWish = MIN2(vWish, vsafeStop);
        }
        vWish = MAX2((SUMOReal) 0, vWish);
        myLFLinkLanes.push_back(
            DriveProcessItem(0, vWish, vWish));
    } else {
        // compute other values as in move
        SUMOReal vBeg = MIN2(myType->maxNextSpeed(myState.mySpeed), lane->maxSpeed());//vaccel( myState.mySpeed, lane->maxSpeed() );
        if(pred!=0) {
            SUMOReal vSafe = myType->ffeV(myState.mySpeed, gap2pred( *pred ), pred->getSpeed());
            //  the vehicle is bound by the lane speed and must not drive faster
            //  than vsafe to the next vehicle
            vBeg = MIN2(vBeg, vSafe);
        }
        // check whether the driver wants to let someone in
        // set next links, computing possible speeds
        vsafeCriticalCont(vBeg);
    }
    // !!! remove this! make this somewhere else!
    if(hasCORNDoubleValue(MSCORN::CORN_VEH_LASTREROUTEOFFSET)) {
        myDoubleCORNMap[MSCORN::CORN_VEH_LASTREROUTEOFFSET] =
            myDoubleCORNMap[MSCORN::CORN_VEH_LASTREROUTEOFFSET] + 1;
    }
    // !!! remove this! make this somewhere else!
}


void
MSVehicle::moveFirstChecked()
{
    myTarget = 0;
#ifdef GUI_DEBUG
    if(gSelected.isSelected(GLO_VEHICLE, static_cast<GUIVehicle*>(this)->getGlID())) {
        int blb = 0;
    }
#endif
#ifdef ABS_DEBUG
    if(debug_globaltime>debug_searchedtime && (myID==debug_searched1||myID==debug_searched2) ) {
        int textdummy = 0;
    }
#endif
    // get vsafe
    SUMOReal vSafe = 0;
    if(!myStops.empty()) {
        if(myStops.begin()->reached) {
            if(myStops.begin()->duration==0) {
                if(myStops.begin()->busstop!=0) {
                    myStops.begin()->busstop->leaveFrom(this);
                }
                myStops.pop_front();
            } else {
                myStops.begin()->duration--;
                myTarget = myLane;
                myState.mySpeed = 0;
                myLane->addMean2(0, myType->getLength());
                return; // !!!detectore etc?
            }
        } else {
            if(myStops.begin()->lane==myLane) {
                Stop &bstop = *myStops.begin();
                SUMOReal endPos = bstop.pos;
                bool busStopsMustHaveSpace = true;
                if(bstop.busstop!=0) {
                    endPos = bstop.busstop->getLastFreePos();
                    if(endPos-5.<bstop.busstop->getBeginLanePosition()) { // !!! explicite offset
                        busStopsMustHaveSpace = false;
                    }
                }
                if(myState.pos()>=endPos-BUS_STOP_OFFSET&&busStopsMustHaveSpace) {
                    bstop.reached = true;
                    if(bstop.busstop!=0) {
                        bstop.busstop->enter(this, myState.pos(), myState.pos()-myType->getLength());
                    }
                }
            }
        }
    }

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
                    if(vSafe<getSpeedAfterMaxDecel(myState.mySpeed)&&link->amYellow()) {
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
    SUMOReal vNext = myType->dawdle(vSafe);
    vNext =
        myLaneChangeModel->patchSpeed(
            getSpeedAfterMaxDecel(myState.mySpeed),
            vNext,
            MIN3(vSafe, myType->maxNextSpeed(myState.mySpeed), myLane->maxSpeed() ),//vaccel(myState.mySpeed, myLane->maxSpeed())),
            vSafe);

    // visit waiting time
    if(vNext<=0.1) {
        myWaitingTime++;
    } else {
        myWaitingTime = 0;
    }
    // call reminders after vNext is set
    SUMOReal pos = myState.myPos;

    // update position
    myState.myPos += SPEED2DIST(vNext);
    // update speed
    myState.mySpeed = vNext;
    MSLane *approachedLane = myLane;
    approachedLane->addMean2(vNext, myType->getLength());


    // move the vehicle forward
    size_t no = 0;
    SUMOReal driven = approachedLane->length() - pos;
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
#ifdef HAVE_INTERNAL_LANES
            approachedLane = link->getViaLane();
            if(approachedLane==0) {
                approachedLane = link->getLane();
            }
#else
            approachedLane = link->getLane();
#endif
        }
        // set information about approaching
        approachedLane->setApproaching(myState.pos(), this);
        approachedLane->addMean2(vNext, getLength());
        no++;
    }
    // set approaching information for consecutive lanes the vehicle may reach in the
    //  next steps
    MSLane *tmpApproached = approachedLane;
    SUMOReal dist = myType->brakeGap(myState.mySpeed) - driven;
    SUMOReal tmpPos = myState.myPos + dist;
    for(; dist>0&&tmpApproached->length()<tmpPos&&i!=myLFLinkLanes.end(); i++) {
        MSLink *link = (*i).myLink;
        if(link==0) {
            break;
        }
        tmpPos -= tmpApproached->length();//approachedLane->length();
#ifdef HAVE_INTERNAL_LANES
        tmpApproached = link->getViaLane();
        if(tmpApproached==0) {
            tmpApproached = link->getLane();
        }
#else
        tmpApproached = link->getLane();
#endif
        tmpApproached->setApproaching(tmpPos, this);
    }

    // enter lane herein if no push occures (otherwise, do it there)
    if(no==0) {
        workOnMoveReminders( pos, pos + SPEED2DIST(vNext), vNext );
    }
    myTarget = approachedLane;
    assert(myTarget!=0);
#ifdef ABS_DEBUG
    if(debug_globaltime>debug_searchedtime && (myID==debug_searched1||myID==debug_searched2)) {
        DEBUG_OUT << "moveb/1:" << debug_globaltime << ": " << myID << " at " << getLane().getID() << ": " << myState.myPos << ", " << myState.mySpeed << endl;
    }
#endif
#ifdef RAKNET_DEMO
	if(myTarget==myLane) {
	    setPosition(position().x(), 0, position().y());
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
MSVehicle::vsafeCriticalCont( SUMOReal boundVSafe )
{
#ifdef GUI_DEBUG
    if(gSelected.isSelected(GLO_VEHICLE, static_cast<GUIVehicle*>(this)->getGlID())) {
        int blb = 0;
    }
#endif
#ifdef ABS_DEBUG
    if(debug_globaltime>debug_searchedtime && (myID==debug_searched1||myID==debug_searched2)) {
        DEBUG_OUT << "vsafeCriticalCont/" << debug_globaltime << ":" << myID << endl;
    }
#endif
    // the vehicle may have just to look into the next lane
    //  compute this information and use it only once in the next loop
    SUMOReal seen = myLane->length() - myState.myPos;
    MSLane *nextLane = myLane;
    // compute the way the vehicle may drive when accelerating
    SUMOReal dist = boundVSafe + myType->brakeGap(myState.mySpeed);
    SUMOReal vLinkPass = boundVSafe;
    SUMOReal vLinkWait = vLinkPass;
    if(seen>boundVSafe + myType->brakeGap(myState.mySpeed)) {
        // just for the case the vehicle is still very far away from the lane end
            myLFLinkLanes.push_back(
                DriveProcessItem(0, vLinkPass, vLinkPass));
            return;
    }

    size_t view = 1;
    // loop over following lanes
    while(true) {
        if(!myStops.empty()&&myStops.begin()->lane->getEdge()==nextLane->getEdge()) {
            SUMOReal vsafeStop = myType->ffeS(myState.mySpeed, seen-(nextLane->length()-myStops.begin()->pos));
            vLinkPass = MIN2(vLinkPass, vsafeStop);
            vLinkWait = MIN2(vLinkWait, vsafeStop);
        }

        // get the next link used
        MSLinkCont::const_iterator link =
            myLane->succLinkSec( *this, view, *nextLane );

        // check whether the lane is a dead end
        //  (should be valid only on further loop iterations
        if(nextLane->isLinkEnd(link)) {
            SUMOReal laneEndVSafe = myType->ffeS(myState.mySpeed, seen);
            myLFLinkLanes.push_back(
                DriveProcessItem(0, MIN2(vLinkPass, laneEndVSafe), MIN2(vLinkPass, laneEndVSafe)));
            // the vehicle will not drive further
            return;
        }
        // the link was passed
        vLinkWait = vLinkPass;

        // get the following lane
#ifdef HAVE_INTERNAL_LANES
        bool nextInternal = false;
        nextLane = (*link)->getViaLane();
        if(nextLane==0) {
            nextLane = (*link)->getLane();
        } else {
            nextInternal = true;
        }
#else
        nextLane = (*link)->getLane();
#endif

        // compute the velocity to use when the link is not blocked by oter vehicles
            // the vehicle shall be not fastern when reaching the next lane than allowed
        SUMOReal vmaxNextLane =
            myType->ffeV(myState.mySpeed, seen, nextLane->maxSpeed());

            // the vehicle shall keep a secure distance to its predecessor
            //  (or approach the lane end if the predeccessor is too near)
        const State &nextLanePred = nextLane->myLastState;
        SUMOReal dist2Pred = seen+nextLanePred.pos()-MSVehicleType::getMaxVehicleLength(); // @!!! die echte Länge des fahrzeugs
        SUMOReal vsafePredNextLane;
        if(dist2Pred>=0) {
            // leading vehicle is not overlapping
            vsafePredNextLane =
                myType->ffeV(myState.mySpeed, dist2Pred, nextLanePred.speed());
            SUMOReal predDec = MAX2((SUMOReal) 0, nextLanePred.speed()-myType->decelAbility() /* !!! decelAbility of leader! */);
            if(myType->brakeGap(vsafePredNextLane)+vsafePredNextLane*myType->getTau() > myType->brakeGap(predDec) + dist2Pred) {

                vsafePredNextLane =
                    MIN2(vsafePredNextLane, DIST2SPEED(dist2Pred));
            }
        } else {
            // leading vehicle is overlapping (stands within the junction)
            vsafePredNextLane =
                myType->ffeV(myState.mySpeed, seen, 0);
        }

            // compute the velocity to use when the link may be used
        vLinkPass =
            MIN3(vLinkPass, vmaxNextLane, vsafePredNextLane/*, vsafeNextLaneEnd*/);

        // if the link may not be used (is blocked by another vehicle) then let the
        //  vehicle decelerate until the end of the street
        vLinkWait =
            MIN2(vLinkWait, myType->ffeS(myState.mySpeed, seen));

        // valid, when a vehicle is not on a priorised lane
        if(!(*link)->havePriority()) {
            // if it has already decelerated to let priorised vehicles pass
            //  and when the distance to the vehicle on the next lane allows moving
            //  (the check whether other incoming vehicles may stop this one is done later)
            // then let it pass
            //  [m]>
            if(seen>myType->approachingBrakeGap(myState.mySpeed)&&dist2Pred>0) {
                vLinkPass = MIN3(vLinkPass, myType->maxNextSpeed(myState.mySpeed), myLane->maxSpeed());//vaccel(myState.mySpeed, myLane->maxSpeed())); // otherwise vsafe may become incredibly large
                (*link)->setApproaching(this);
            } else {
                // let it wait in the other cases
                vLinkPass = vLinkWait;
            }
        }
        myLFLinkLanes.push_back(DriveProcessItem(*link, vLinkPass, vLinkWait));
        if( vsafePredNextLane>0&&dist-seen>0 ) {
            (*link)->setApproaching(this);
        } else {
            return;
        }
        seen += nextLane->length();
        if(seen>dist) {
            return;
        }
#ifdef HAVE_INTERNAL_LANES
        if(!nextInternal) {
            view++;
        }
#else
        view++;
#endif
    }
}

/////////////////////////////////////////////////////////////////////////////

Position2D
MSVehicle::getPosition() const
{
	return myLane->getShape().positionAtLengthPosition(myState.pos());
}

/////////////////////////////////////////////////////////////////////////////

const string &
MSVehicle::getID() const
{
    return myID;
}

/////////////////////////////////////////////////////////////////////////////

bool
MSVehicle::isEquipped()
{
    return equipped;
}

/////////////////////////////////////////////////////////////////////////////

bool
MSVehicle::onAllowed( const MSLane* lane ) const
{
#ifdef GUI_DEBUG
    if(gSelected.isSelected(GLO_VEHICLE, static_cast<const GUIVehicle*>(this)->getGlID())) {
        int blb = 0;
    }
#endif
    if(lane->getEdge()->getPurpose()==MSEdge::EDGEFUNCTION_INTERNAL) {
        return true;
    }
    if(myAllowedLanes.size()==0) {
        return false; // check (was assertion only)!!!
    }
    if(!lane->allowsVehicleClass(myType->getVehicleClass())) {
        return false;
    }
    assert(myAllowedLanes.size()!=0);
    MSEdge::LaneCont::const_iterator compare =
        find( myAllowedLanes[0]->begin(), myAllowedLanes[0]->end(), lane );
    return ( compare != myAllowedLanes[0]->end() );
}


bool
MSVehicle::onAllowed( ) const
{
#ifdef GUI_DEBUG
    if(gSelected.isSelected(GLO_VEHICLE, static_cast<const GUIVehicle*>(this)->getGlID())) {
        int blb = 0;
    }
#endif
    if(myLane->getEdge()->getPurpose()==MSEdge::EDGEFUNCTION_INTERNAL) {
        return true;
    }
    if(myAllowedLanes.size()==0) {
        return false; // check (was assertion only)!!!
    }
    if(!myLane->allowsVehicleClass(myType->getVehicleClass())) {
        return false;
    }
    assert(myAllowedLanes.size()!=0);
    MSEdge::LaneCont::const_iterator compare =
        find( myAllowedLanes[0]->begin(), myAllowedLanes[0]->end(), myLane );
    return ( compare != myAllowedLanes[0]->end() );
}

	struct Information {
		std::string InfoTyp;
		MSEdge *edge;
		int neededTime; // how long needed the vehicle to travel on the edge
		int time; // the Time, when the Info was saved
	};
/////////////////////////////////////////////////////////////////////////////

void
MSVehicle::enterLaneAtMove( MSLane* enteredLane, SUMOReal driven )
{
#ifdef ABS_DEBUG
    if(debug_globaltime>debug_searchedtime && (myID==debug_searched1||myID==debug_searched2)) {
        int textdummy = 0;
    }
#endif
    // save the old work reminders, patching the position information
    // add the information about the new offset to the old lane reminders
    SUMOReal oldLaneLength = myLane->length();
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
    SUMOReal entryTimestep =
        static_cast< SUMOReal >( MSNet::getInstance()->getCurrentTimeStep() ) - 1 +
        driven / myState.mySpeed;
    assert(entryTimestep<=debug_globaltime);
    // get new move reminder
    myMoveReminders = enteredLane->getMoveReminders();
    // proceed in route
    const MSEdge * const enteredEdge = enteredLane->getEdge();
    if(enteredEdge->getPurpose()!=MSEdge::EDGEFUNCTION_INTERNAL) {
        MSRouteIterator edgeIt = myCurrEdge;
        while ( *edgeIt != enteredEdge ) {
            ++edgeIt;
            assert( edgeIt != myRoute->end() );
        }
        myCurrEdge = edgeIt;
    }

	if(isEquipped()){
		ofstream out2("EquippedVehInEdges.txt", ios_base::app);
		out2<<"--enterLaneAtMove Enter Lane: "<<(*myCurrEdge)->getID()<<" Vehicle: "<<getID()<<" Timestep: "<<MSNet::getInstance()->getCurrentTimeStep()<<" --"<<endl;
		out2.close();
		(*myCurrEdge)->addEquippedVehicle(getID(), this);

		std::map<std::string, Information *>::iterator i = infoCont.find((*myCurrEdge)->getID());
        if(i==infoCont.end()){
			Information *info = new Information;
			info->InfoTyp = "congestion";
			info->neededTime = 0;
			info->time = MSNet::getInstance()->getCurrentTimeStep();
			infoCont[(*myCurrEdge)->getID()] = info;
		}else{
            (*i).second->time = MSNet::getInstance()->getCurrentTimeStep();
			(*i).second->neededTime = 0;
		}
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
    // switch to and activate the new lane's reminders
    // keep OldLaneReminders
    myMoveReminders = enteredLane->getMoveReminders();
    myAllowedLanes.clear();
    rebuildAllowedLanes();
    activateRemindersByEmitOrLaneChange();
}

/////////////////////////////////////////////////////////////////////////////

void
MSVehicle::enterLaneAtEmit( MSLane* enteredLane, const State &state )
{
    myState = state;
    assert( myState.myPos >= 0 );
    assert( myState.mySpeed >= 0 );
    myWaitingTime = 0;
    myLane = enteredLane;
    // set and activate the new lane's reminders
    myMoveReminders = enteredLane->getMoveReminders();
    activateRemindersByEmitOrLaneChange();

    // for Car2Car
	if(isEquipped()){
		ofstream out2("EquippedVehInEdges.txt", ios_base::app);
		out2<<"--Enter enterLaneAtEmit Lane: "<<(*myCurrEdge)->getID()<<" Vehicle: "<<getID()<<" Timestep: "<<MSNet::getInstance()->getCurrentTimeStep()<<" --"<<endl;
		out2.close();
		(*myCurrEdge)->addEquippedVehicle(getID(), this);

		std::map<std::string, Information *>::iterator i = infoCont.find((*myCurrEdge)->getID());
        if(i==infoCont.end()){
			Information *info = new Information;
			info->InfoTyp = "congestion";
			info->neededTime = 0;
			info->time = MSNet::getInstance()->getCurrentTimeStep();
			infoCont[(*myCurrEdge)->getID()] = info;
		}else{
            (*i).second->time = MSNet::getInstance()->getCurrentTimeStep();
			(*i).second->neededTime = 0;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

void
MSVehicle::leaveLaneAtMove( SUMOReal driven )
{
	if(isEquipped()){ //

		ofstream out2("EquippedVehInEdges.txt", ios_base::app);
		out2<<"--Leave leaveLaneAtMove Lane: "<<(*myCurrEdge)->getID()<<" Vehicle: "<<getID()<<" Timestep: "<<MSNet::getInstance()->getCurrentTimeStep()<<" --"<<endl;
		out2.close();
		(*myCurrEdge)->removeEquippedVehicle(getID());


	    ofstream out("savedInformations.txt", ios_base::app);
		// save required Informations
		std::map<std::string, Information *>::iterator i = infoCont.find((*myCurrEdge)->getID());
		if(i != infoCont.end()){
			float nt = MSNet::getInstance()->getCurrentTimeStep() - (*i).second->time;
			float length = (*((*myCurrEdge)->getLanes()))[0]->length();
            float speed  = (*((*myCurrEdge)->getLanes()))[0]->maxSpeed();
			float factor = (length/speed)*2;
			if(nt > factor){
				(*i).second->neededTime = nt;
				out<<MSNet::getInstance()->getCurrentTimeStep()<<" 1"<<endl;
			}else{
				infoCont.erase((*myCurrEdge)->getID());
			}
		}else{ // should never arrive here
			Information *info = new Information;
			info->InfoTyp = "congestion";
			info->neededTime = 0;
			info->time = MSNet::getInstance()->getCurrentTimeStep();
			infoCont[(*myCurrEdge)->getID()] = info;
		}
		out.close();
	}


    if(!myAllowedLanes.empty()) {
        myAllowedLanes.pop_front();
    }
}

/////////////////////////////////////////////////////////////////////////////

void
MSVehicle::leaveLaneAtLaneChange( void )
{
    // dismiss the old lane's reminders
    SUMOReal savePos = myState.myPos; // have to do this due to SUMOReal-precision errors
    vector< MSMoveReminder* >::iterator rem;
    for(rem=myMoveReminders.begin(); rem != myMoveReminders.end(); ++rem ){
        (*rem)->dismissByLaneChange( *this );
    }
    std::vector<SUMOReal>::iterator off = myOldLaneMoveReminderOffsets.begin();
    for(rem=myOldLaneMoveReminders.begin(); rem!=myOldLaneMoveReminders.end(); ++rem, ++off) {
        myState.myPos += (*off);
        (*rem)->dismissByLaneChange( *this );
        myState.myPos -= (*off);
    }
    myState.myPos = savePos; // have to do this due to SUMOReal-precision errors
    myMoveReminders.clear();
    myOldLaneMoveReminders.clear();
    myOldLaneMoveReminderOffsets.clear();
}

/////////////////////////////////////////////////////////////////////////////

const MSEdge * const
MSVehicle::getEdge() const
{
    return *myCurrEdge;
}



bool
MSVehicle::reachingCritical(SUMOReal laneLength) const
{
    // check whether the vehicle will run over the lane when accelerating
    return (laneLength - myState.myPos - myType->brakeGap(myState.mySpeed)) <= 0;
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




void
MSVehicle::workOnMoveReminders( SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed,
                                MoveOnReminderMode mode )
{
#ifdef ABS_DEBUG
    if(debug_globaltime>debug_searchedtime && (myID==debug_searched1||myID==debug_searched2)) {
        int textdummy = 0;
    }
#endif
    // This erasure-idiom works for all stl-sequence-containers
    // See Meyers: Effective STL, Item 9
    for (MoveReminderContIt rem=myMoveReminders.begin(); rem!=myMoveReminders.end(); ) {
        if (!(*rem)->isStillActive(*this, oldPos, newPos, newSpeed)) {
            rem = myMoveReminders.erase(rem);
        } else {
            ++rem;
        }
    }
    if(mode!=CURRENT) {
        OffsetVector::iterator off=myOldLaneMoveReminderOffsets.begin();
        for (MoveReminderContIt rem=myOldLaneMoveReminders.begin(); rem!=myOldLaneMoveReminders.end(); ) {
            SUMOReal oldLaneLength = *off;
            if (!(*rem)->isStillActive(*this, oldLaneLength+oldPos, oldLaneLength+newPos, newSpeed) ) {
                rem = myOldLaneMoveReminders.erase( rem );
                off = myOldLaneMoveReminderOffsets.erase(off);
            } else {
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
    for (MoveReminderContIt rem=myMoveReminders.begin(); rem!=myMoveReminders.end(); ) {
        if (!(*rem)->isActivatedByEmitOrLaneChange( *this )) {
            rem = myMoveReminders.erase( rem );
        } else {
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
MSVehicle::proceedVirtualReturnWhetherEnded(const MSEdge *const newEdge)
{
    bool _destReached = destReached(newEdge);
    myAllowedLanes.clear(); // !!! not really necessary!?
    rebuildAllowedLanes();
    return _destReached;
}


void
MSVehicle::onTripEnd(bool wasAlreadySet)
{
    SUMOReal pspeed = myState.mySpeed;
    SUMOReal pos = myState.myPos;
    SUMOReal oldPos = pos - SPEED2DIST(pspeed);
    if ( pos - myType->getLength() < 0 ) {
        SUMOReal pdist = (SUMOReal) (myType->getLength() + 0.01) - oldPos;
        pspeed = DIST2SPEED(pdist);
        pos = (SUMOReal) (myType->getLength() + 0.1);
    }
    pos += myLane->length();
    oldPos += myLane->length();
    // process reminder
        // current
    vector< MSMoveReminder* >::iterator rem;
    for (rem=myMoveReminders.begin(); rem!=myMoveReminders.end(); ++rem) {
        // the vehicle may only be at the entry occupancy correction
        if( (*rem)->isStillActive( *this, oldPos, pos, pspeed) ) {
            assert(false);
        }
    }
        // old
    rem = myOldLaneMoveReminders.begin();
    OffsetVector::iterator off = myOldLaneMoveReminderOffsets.begin();
    for (;rem!=myOldLaneMoveReminders.end(); ++rem, ++off) {
        SUMOReal oldLaneLength = *off;
        if( (*rem)->isStillActive( *this, oldPos+oldLaneLength, pos+oldLaneLength, pspeed) ) {
            assert(false); // !!!
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
#ifdef HAVE_INTERNAL_LANES
        tmp = (*i).myLink->getViaLane();
        if(tmp!=0) {
            tmp->resetApproacherDistance(this);
        }
#endif
        ++i;
    }
}


void
MSVehicle::onDepart()
{
    // check whether the vehicle's departure time shall be saved
    myDoubleCORNMap[MSCORN::CORN_VEH_REALDEPART] =
        (SUMOReal) MSNet::getInstance()->getCurrentTimeStep();
    // check whether the vehicle control shall be informed
    if(MSCORN::wished(MSCORN::CORN_VEHCONTROL_WANTS_DEPARTURE_INFO)) {
        MSNet::getInstance()->getVehicleControl().vehicleEmitted(this);
    }
    // initialise devices
    {
        // cell phones
        size_t no = (size_t) myDoubleCORNMap[(MSCORN::Function) MSCORN::CORN_VEH_DEV_NO_CPHONE];
        for(size_t np=0; np<no; np++) {
    	    ((MSDevice_CPhone*) myPointerCORNMap[(MSCORN::Pointer) (MSCORN::CORN_P_VEH_DEV_CPHONE+np)])->onDepart();
        }
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
    QuitRemindedVector::iterator i = find(myQuitReminded.begin(), myQuitReminded.end(), r);
    if(i!=myQuitReminded.end()) {
        myQuitReminded.erase(i);
    } else {
        assert(false);
    }
}


SUMOReal
MSVehicle::getCORNDoubleValue(MSCORN::Function f) const
{
    return myDoubleCORNMap.find(f)->second;
}


void *
MSVehicle::getCORNPointerValue(MSCORN::Pointer f) const
{
    return myPointerCORNMap.find(f)->second;
}


bool
MSVehicle::hasCORNDoubleValue(MSCORN::Function f) const
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
    if(debug_globaltime>debug_searchedtime && (myID==debug_searched1||myID==debug_searched2)) {
        int textdummy = 0;
        for(MSEdgeVector::const_iterator i=edges.begin(); i!=edges.end(); ++i) {
            DEBUG_OUT << (*i)->getID() << ", ";
        }
        DEBUG_OUT << "-------------" << endl;
        for(MSRouteIterator i2=myRoute->begin(); i2!=myRoute->end(); ++i2) {
            DEBUG_OUT << (*i2)->getID() << ", ";
        }
        DEBUG_OUT << "-------------" << endl;
    }
#endif
    MSRoute *otherr = 0;
    const MSEdge *currentEdge = *myCurrEdge;
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
		    SUMOTime timeOffset = (SUMOTime) MSCORN::CORN_VEH_REROUTE_TIME +
			    (SUMOTime) myDoubleCORNMap[MSCORN::CORN_VEH_NUMBERROUTE];
            myDoubleCORNMap[(MSCORN::Function) timeOffset] = (SUMOReal) simTime;
        }
        myDoubleCORNMap[MSCORN::CORN_VEH_LASTREROUTEOFFSET] = 0;
        myDoubleCORNMap[MSCORN::CORN_VEH_NUMBERROUTE] =
            myDoubleCORNMap[MSCORN::CORN_VEH_NUMBERROUTE] + 1;
    }
#ifdef ABS_DEBUG
    if(debug_globaltime>debug_searchedtime && (myID==debug_searched1||myID==debug_searched2)) {
        int textdummy = 0;
        for(MSRouteIterator i=myRoute->begin(); i!=myRoute->end(); ++i) {
            DEBUG_OUT << (*i)->getID() << ", ";
        }
        DEBUG_OUT << "-------------" << endl;
    }
#endif
    return replaced;
}


bool
MSVehicle::replaceRoute(MSRoute *newRoute, size_t simTime)
{
#ifdef ABS_DEBUG
    if(debug_globaltime>debug_searchedtime && (myID==debug_searched1||myID==debug_searched2)) {
        int textdummy = 0;
        for(MSEdgeVector::const_iterator i=newRoute->begin(); i!=newRoute->end(); ++i) {
            DEBUG_OUT << (*i)->getID() << ", ";
        }
        DEBUG_OUT << "-------------" << endl;
        for(MSRouteIterator i2=myRoute->begin(); i2!=myRoute->end(); ++i2) {
            DEBUG_OUT << (*i2)->getID() << ", ";
        }
        DEBUG_OUT << "-------------" << endl;
    }
#endif
    MSRoute *otherr = 0;
    const MSEdge *currentEdge = *myCurrEdge;
    if(newRoute->find(currentEdge)==newRoute->end()) {
        return false;
    }
    // check whether the information shall be saved
    if(MSCORN::wished(MSCORN::CORN_VEH_SAVEREROUTING)) {
        otherr = new MSRoute(*myRoute);
    }
    // try to replace the current route
    myRoute = newRoute;
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
		SUMOTime timeOffset = (SUMOTime) MSCORN::CORN_VEH_REROUTE_TIME +
		    (SUMOTime) myDoubleCORNMap[MSCORN::CORN_VEH_NUMBERROUTE];
        myDoubleCORNMap[(MSCORN::Function) timeOffset] = (SUMOReal) simTime;
    }
    myDoubleCORNMap[MSCORN::CORN_VEH_LASTREROUTEOFFSET] = 0;
    myDoubleCORNMap[MSCORN::CORN_VEH_NUMBERROUTE] =
        myDoubleCORNMap[MSCORN::CORN_VEH_NUMBERROUTE] + 1;
#ifdef ABS_DEBUG
    if(debug_globaltime>debug_searchedtime && (myID==debug_searched1||myID==debug_searched2)) {
        int textdummy = 0;
        for(MSRouteIterator i=myRoute->begin(); i!=myRoute->end(); ++i) {
            DEBUG_OUT << (*i)->getID() << ", ";
        }
        DEBUG_OUT << "-------------" << endl;
    }
#endif
    assert((MSEdge*)succEdge(1)!=0);
    return true;
}


const MSVehicleType &
MSVehicle::getVehicleType() const
{
    return *myType;
}


void
MSVehicle::rebuildAllowedLanes()
{
    SUMOReal dist = 0;
    // check what was already computed
    for(NextAllowedLanes::const_iterator i=myAllowedLanes.begin(); i!=myAllowedLanes.end(); ++i) {
        dist += ((*(*i))[0])->length();
    }
    // compute next allowed lanes up to 1000m into the future
    SUMOReal MIN_DIST = 1000;
    if(dist<MIN_DIST) {
        size_t pos = distance(myRoute->begin(), myCurrEdge) + myAllowedLanes.size();
        if(pos>=myRoute->size()-1) {
            return;
        }
        const MSEdge::LaneCont *al = ( *myRoute )[pos]->allowedLanes( *( *myRoute )[pos+1], myType->getVehicleClass() );
        while(al!=0&&dist<MIN_DIST&&pos<myRoute->size()-1) {
            assert(al!=0);
            myAllowedLanes.push_back(al);
            pos++;
            if(pos<myRoute->size()-1) {
                dist += ((*al)[0])->length();
                al = ( *myRoute )[pos]->allowedLanes( *( *myRoute )[pos+1], myType->getVehicleClass() );
            }
        }
    }
}


std::vector<std::vector<MSVehicle::LaneQ> >
MSVehicle::getBestLanes() const
{
#ifdef GUI_DEBUG
    if(gSelected.isSelected(GLO_VEHICLE, ((GUIVehicle*) this)->getGlID())) {
        int blb = 0;
    }
#endif
    std::vector<std::vector<LaneQ> > ret;
    SUMOReal MIN_DIST = 3000;
    MSRouteIterator ce = myCurrEdge;
    int seen = 0;
    float dist = 0;
    // compute initial list
    while(seen<4&&dist<MIN_DIST&&ce!=myRoute->end()) {
        const MSEdge::LaneCont * const lanes = (*ce)->getLanes();
        ret.push_back(std::vector<LaneQ>());
        std::vector<LaneQ> &curr = *(ret.end()-1);
        for(int i=0; i<lanes->size(); i++) {
            curr.push_back(LaneQ());
            LaneQ &currQ = *(curr.end()-1);
            const MSEdge::LaneCont *allowed = (*ce)->allowedLanes(**(ce+1), myType->getVehicleClass());
            if(allowed!=0&&find(allowed->begin(), allowed->end(), (*lanes)[i])!=allowed->end()) {
//            if(onAllowed((*lanes)[i])) {
                currQ.t1 = true;
                //currQ.length = (*lanes)[i]->length();
            } else {
                currQ.t1 = false;
                //currQ.length = 0;
            }

            currQ.length = (*lanes)[i]->length();
            currQ.alllength = (*lanes)[i]->length();

                if(!myStops.empty()&&myStops.begin()->lane->getEdge()==(*lanes)[i]->getEdge()) {
                    if(myStops.begin()->lane!=(*lanes)[i]) {
                        currQ.length = 0;
                        currQ.alllength = 0;
                        currQ.t1 = false;
                    }
                }

            currQ.lane = (*lanes)[i];
            currQ.hindernisPos = (*lanes)[i]->getDensity() * currQ.lane->length();
            currQ.v = (*lanes)[i]->getVehLenSum();
            currQ.wish = 1;
            currQ.dir = 0;
        }
        ce++;
        seen++;
        dist += (*lanes)[0]->length();
    }
    // sum up consecutive lengths
    {
        ce = myCurrEdge + ret.size() - 1;
        std::vector<std::vector<LaneQ> >::reverse_iterator i;
        for(i=ret.rbegin()+1; i!=ret.rend(); ++i, --ce) {
            std::vector<LaneQ> &curr = *i;
            for(int j=0; j<curr.size(); ++j) {
                MSLane *lane = curr[j].lane;
                const MSLinkCont &lc = lane->getLinkCont();
                for(MSLinkCont::const_iterator k=lc.begin(); k!=lc.end(); ++k) {
                    MSLane *c = (*k)->getLane();
                    for(std::vector<LaneQ>::iterator l=(*(i-1)).begin(); l!=(*(i-1)).end(); ++l) {
                        if((*l).lane==c&&curr[j].t1) {
                            curr[j].length += (*l).length;
                            curr[j].v += (*l).v;
                            curr[j].wish++;// += (*l).length;
                            curr[j].alllength = (*l).alllength;
                        }
                    }
                }
            }
        }
    }
    // compute moving direction
    {
        // !!! optimize: maybe only for the current edge
        std::vector<std::vector<LaneQ> >::iterator i;
        for(i=ret.begin(); i!=ret.end(); ++i) {
            std::vector<LaneQ> &curr = *i;
            int best = 0;
            SUMOReal bestLength = 0;
            int j;
            for(j=0; j<curr.size(); ++j) {
                if(curr[j].length>bestLength) {
                    bestLength = curr[j].length;
                    best = j;
                }
            }
            for(j=0; j<curr.size(); ++j) {
                curr[j].dir = best-j;
            }
        }
    }
    return ret;

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
        os << " replacedOnEdge=\"" << ((MSEdge*) (*j).second)->getID() << "\" ";
        // write the time at which the route was replaced
        std::map<MSCORN::Function, SUMOReal>::const_iterator j2 =
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


void
MSVehicle::setCORNColor(SUMOReal red, SUMOReal green, SUMOReal blue)
{
    myDoubleCORNMap[MSCORN::CORN_VEH_OWNCOL_RED] = red;
    myDoubleCORNMap[MSCORN::CORN_VEH_OWNCOL_GREEN] = green;
    myDoubleCORNMap[MSCORN::CORN_VEH_OWNCOL_BLUE] = blue;
}


#ifdef HAVE_MESOSIM
#include <mesosim/MESegment.h>
#include <mesosim/MELoop.h>
#include "MSGlobals.h"
#endif

void
MSVehicle::saveState(std::ostream &os, long what)
{
    FileHelpers::writeString(os, myID);
    FileHelpers::writeInt(os, myLastLaneChangeOffset);
    FileHelpers::writeUInt(os, myWaitingTime);
    FileHelpers::writeInt(os, myRepetitionNumber);
    FileHelpers::writeInt(os, myPeriod);
    FileHelpers::writeString(os, myRoute->getID());
    FileHelpers::writeUInt(os, myDesiredDepart);
    FileHelpers::writeString(os, myType->getID());
    FileHelpers::writeUInt(os, myRoute->posInRoute(myCurrEdge));
    FileHelpers::writeUInt(os, (unsigned int) getCORNDoubleValue(MSCORN::CORN_VEH_REALDEPART));
#ifdef HAVE_MESOSIM
    // !!! several things may be missing
    if(seg==0) {
        FileHelpers::writeUInt(os, 0);
        FileHelpers::writeFloat(os, tEvent);
        FileHelpers::writeFloat(os, tLastEntry);
    } else {
        FileHelpers::writeUInt(os, seg->get_index());
        FileHelpers::writeFloat(os, tEvent);
        FileHelpers::writeFloat(os, tLastEntry);
    }
    FileHelpers::writeByte(os, inserted);
#endif
}




void
MSVehicle::addVehNeighbors(MSVehicle *veh, int time)
{
	if(computeDistance(this, veh)){
		ofstream out("VehicleNeighborsGNU.txt", ios_base::app);
		Position2D pos1 = this->getPosition();
	    Position2D pos2 = veh->getPosition();
		std::map<std::string, C2CConnection*>::iterator i = myNeighbors.find(veh->getID());
		if(i== myNeighbors.end()){
			out<<"TS_"<<time<<" Com Possible"<<endl;
			C2CConnection *con = new C2CConnection;
			con->connectedVeh=veh;
			con->state = disconnected;
			con->timeSinceSeen = 1;
			con->timeSinceConnect = 0;
			con->lastTimeSeen = time;
			myNeighbors[veh->getID()] = con;
            veh->quitRemindedEntered(this);
		}else{
			 out<<"TS_"<<time<<" Com Possible"<<endl;
			 (*i).second->lastTimeSeen = time;
		}
		out.close();
	}
}


void
MSVehicle::cleanUpConnections(int time)
{
	std::map<std::string, C2CConnection*>::iterator i;
	ofstream out("VehicleNeighborsGNU.txt", ios_base::app);
	for(i=myNeighbors.begin(); i!=myNeighbors.end(); ) {
		if((*i).second->lastTimeSeen != time) {
            MSNet::getInstance()->getVehicleControl().getVehicle((*i).first)->quitRemindedLeft(this);
			i = myNeighbors.erase(i);
		} else {
			((*i).second->timeSinceSeen)++;
			if(((*i).second->state != dialing)&&((*i).second->state != disconnected)){
				((*i).second->timeSinceConnect)++;
			}
			++i;
		}
	}
	out.close();
}


bool
MSVehicle::computeDistance(MSVehicle* veh1, MSVehicle* veh2)
{
	bool b = false;
	Position2D pos1 = veh1->getPosition();
	Position2D pos2 = veh2->getPosition();
	if(fabs(pos1.x()-pos2.x())<MSGlobals::gLANRange && fabs(pos1.y()-pos2.y())<MSGlobals::gLANRange){
		int distance = sqrt(pow(pos1.x()-pos2.x(),2) + pow(pos1.y()-pos2.y(),2));
		if((distance>0)&&(distance<=MSGlobals::gLANRange)){
			b = true;
		}
	}
	return b;
}


void
MSVehicle::updateInfos(int time)
{
	std::map<std::string, Information*>::iterator i = infoCont.begin();
	for(i; i!= infoCont.end(); ){
		if((*i).second->time < time - MSGlobals::gLANRefuseOldInfosOffset){
			i = infoCont.erase(i);
		}else{
			i++;
		}
	}
}


void
MSVehicle::removeAllVehNeighbors(void)
{
	myNeighbors.clear();

}


void
MSVehicle::removeOnTripEnd( MSVehicle *veh )
{
    quitRemindedLeft(veh);
    assert(myNeighbors.find(veh->getID())!=myNeighbors.end());
    myNeighbors.erase(myNeighbors.find(veh->getID()));
}


bool
MSVehicle::knowsEdgeTest(MSEdge &edge) const
{
    return infoCont.find(edge.getID())!=infoCont.end();
}


const MSVehicle::VehCont &
MSVehicle::getConnections() const
{
    return myNeighbors;
}

void
MSVehicle::setClusterId(int Id)
{
	clusterId = Id;
}

int
MSVehicle::getClusterId(void)
{
	return clusterId;
}

int
MSVehicle::buildMyCluster(int myStep, int clId){
	ofstream out("Cluster2.txt", ios_base::app);
	int count = 1; // sich selbst erst mal zählen
    clusterId = clId;
	out<<endl;
	out<<"#TimeStep "<<myStep<<endl;
	std::map<std::string, C2CConnection*>::iterator i;
	out<<"-- Anzahl Nachabrn "<<myNeighbors.size()<<endl;
	for(i=myNeighbors.begin(); i!=myNeighbors.end(); i++){
		if((*i).second->connectedVeh->getClusterId()<0){
			out<<"---  In mein Cluster "<<(*i).second->connectedVeh->getID()<<endl;
			count++;
			(*i).second->connectedVeh->setClusterId(clId);
			clusterCont.push_back((*i).second);
			std::map<std::string, C2CConnection*>::iterator j;
			for(j=(*i).second->connectedVeh->myNeighbors.begin(); j!=(*i).second->connectedVeh->myNeighbors.end(); j++){
				if((*j).second->connectedVeh->getClusterId()<0){
					out<<"---    In mein Cluster "<<(*j).second->connectedVeh->getID()<<endl;
                    count++;
					(*j).second->connectedVeh->setClusterId(clId);
					clusterCont.push_back((*j).second);
				}else{
					out<<"---    Nicht In mein Cluster "<<(*j).second->connectedVeh->getID()<<endl;

				}
			}
		}else if((*i).second->connectedVeh->getClusterId()==clusterId){
			// du bist zwar mein Nachbarn, aber du würdest von einem anderen Nachbarn von mir schon eingeladen,
	        // dann werde ich deine nachbarn einladen.
			std::map<std::string, C2CConnection*>::iterator j;
			for(j=(*i).second->connectedVeh->myNeighbors.begin(); j!=(*i).second->connectedVeh->myNeighbors.end(); j++){
				if((*j).second->connectedVeh->getClusterId()<0){
					out<<"---    In mein Cluster "<<(*j).second->connectedVeh->getID()<<endl;
                    count++;
					(*j).second->connectedVeh->setClusterId(clId);
					clusterCont.push_back((*j).second);
				}else{
					out<<"---    Nicht In mein Cluster "<<(*j).second->connectedVeh->getID()<<endl;
				}
			}

		} else{
			out<<"---  Nicht In mein Cluster "<<(*i).second->connectedVeh->getID()<<endl;
		}
	}
	out.close();
	return count;
}

void
MSVehicle::sendInfos(SUMOTime time)
{
	int NumberOfInfo = 732*14;
    int count = 0; // wieviel Info übertragen würden
	ofstream out("TransmittedInfosGNU.txt", ios_base::app);
	out<<time<<" ";
	// Als ClusterHeader, sende ich als erste
/*
	for(VehCont::const_iterator i=myNeighbors.begin(); i!=myNeighbors.end() && NumberOfInfo>0; ++i) {
		int transmit = (*i).second->connectedVeh->transferInformation(getInfosToSend(),NofP);
    }
*/
	for(VehCont::const_iterator i=myNeighbors.begin(); i!=myNeighbors.end() && NumberOfInfo>0; ++i) {
		int NofP = NumOfPack(this,(*i).second->connectedVeh);
		if(NofP>NumberOfInfo)
			NofP=NumberOfInfo;
        count = count + getInfosToSend().size();
		int transmit = (*i).second->connectedVeh->transferInformation(getInfosToSend(),NofP);
        NumberOfInfo = NumberOfInfo - (NumberOfInfo/NofP)*transmit;
	}

    // Alle Vehicle in mein Cluster das Senden erlauben.
	for(ClusterCont::const_iterator o=clusterCont.begin(); o!=clusterCont.end() && NumberOfInfo>0; ++o) {
       for(VehCont::const_iterator j=(*o)->connectedVeh->myNeighbors.begin(); j!=(*o)->connectedVeh->myNeighbors.end(); ++j) {
			int NofP = NumOfPack((*j).second->connectedVeh,(*o)->connectedVeh);
			if(NofP>NumberOfInfo)
				NofP=NumberOfInfo;
			int transmit = (*j).second->connectedVeh->transferInformation((*o)->connectedVeh->getInfosToSend(), NofP);
			NumberOfInfo = NumberOfInfo - (NumberOfInfo/NofP)*transmit;
			count = count+(*o)->connectedVeh->getInfosToSend().size();
		}
	}
	out<<count<<endl;
	out.close();
	clusterCont.clear();

}

int
MSVehicle::NumOfPack(MSVehicle *veh1, MSVehicle* veh2)
{
	Position2D pos1 = veh1->getPosition();
	Position2D pos2 = veh2->getPosition();

	int distance = sqrt(pow(pos1.x()-pos2.x(),2) + pow(pos1.y()-pos2.y(),2));
	int x = ((-2.3*distance + 1650)*732)/1500;
	return x*14;



}

MSVehicle::InfoCont
MSVehicle::getInfosToSend(void)
{
	InfoCont infos;
	InfoCont::iterator i;
	for(i = infoCont.begin(); i != infoCont.end(); i++){
		if((*i).second->neededTime>0){ // wenn er die Strasse schon vollständig befahren hat
			infos[(*i).first] = (*i).second;
		}
	}

	return infos;
}




int
MSVehicle::transferInformation(InfoCont infos, int NofP)
{
	int count = 0;
	std::map<std::string, Information *>::iterator i;
	for(i = infos.begin(); i != infos.end() && count < NofP; i++){
		std::map<std::string, Information *>::iterator j = infoCont.find((*i).first);
		if(j== infoCont.end() || (*i).second->time < (*j).second->time ){ // wenn noch nicht eine Info über diese Edge vorhanden
			infoCont[(*i).first] = (*i).second;                           // oder die Information älter ist, dann speichern
		}
		count++;
	}
	return count;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
