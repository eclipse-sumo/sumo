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
// Revision 1.122  2007/01/11 06:33:53  dkrajzew
// speeded up c2c computation
//
// Revision 1.121  2007/01/10 08:29:34  dkrajzew
// Debugged the c2x.saved-info-freq-output problems occuring when when not all vehicles have left the simulation
//
// Revision 1.120  2006/12/22 12:08:11  dkrajzew
// made c2c defaults variable
//
// Revision 1.119  2006/12/20 10:47:10  dkrajzew
// debugging c2c
//
// Revision 1.118  2006/12/20 08:38:04  dkrajzew
// removed memory leaks
//
// Revision 1.117  2006/12/19 08:03:34  dkrajzew
// debugging c2c
//
// Revision 1.116  2006/12/18 14:43:57  dkrajzew
// debugging c2c
//
// Revision 1.115  2006/12/18 08:23:15  dkrajzew
// fastened c2c storages
//
// Revision 1.114  2006/12/12 12:14:08  dkrajzew
// debugging of loading weights
//
// Revision 1.113  2006/12/04 08:00:47  dkrajzew
// debugging of c2c-rerouting
//
// Revision 1.112  2006/12/01 09:14:41  dkrajzew
// debugging cell phones
//
// Revision 1.111  2006/11/30 12:47:36  dkrajzew
// debugging c2c based rerouting
//
// Revision 1.110  2006/11/29 07:48:36  dkrajzew
// debugging
//
// Revision 1.109  2006/11/28 12:16:31  dkrajzew
// debugged c2c-communication failures on vehicle teleportation
//
// Revision 1.107  2006/11/23 11:40:24  dkrajzew
// removed unneeded code
//
// Revision 1.106  2006/11/17 11:15:05  dkrajzew
// removed an unneeded method
//
// Revision 1.105  2006/11/17 09:09:58  dkrajzew
// warnings removed
//
// Revision 1.104  2006/11/16 13:56:45  dkrajzew
// warnings removed
//
// Revision 1.103  2006/11/16 10:50:44  dkrajzew
// warnings removed
//
// Revision 1.102  2006/11/16 07:02:17  dkrajzew
// warnings removed
//
// Revision 1.101  2006/11/14 13:02:05  dkrajzew
// warnings removed
//
// Revision 1.100  2006/11/14 06:46:07  dkrajzew
// lane change speed-up; first steps towards car2car-based rerouting
//
// Revision 1.99  2006/11/02 11:44:50  dkrajzew
// added Danilo Teta-Boyom's changes to car2car-communication
//
// Revision 1.98  2006/11/01 08:12:42  dkrajzew
// debugged building under Linux
//
// Revision 1.97  2006/10/31 12:20:31  dkrajzew
// further work on internal lanes
//
// Revision 1.96  2006/10/25 12:22:34  dkrajzew
// updated
//
// Revision 1.95  2006/10/12 13:35:34  dkrajzew
// debugging
//
// Revision 1.94  2006/10/12 10:14:27  dkrajzew
// synchronized with internal CVS (mainly the documentation has changed)
//
// Revision 1.93  2006/10/12 08:09:41  dkrajzew
// fastened up lane changing; added current car2car-code
//
// Revision 1.92  2006/10/06 07:13:40  dkrajzew
// debugging internal lanes
//
// Revision 1.91  2006/10/04 13:18:17  dkrajzew
// debugging internal lanes, multiple vehicle emission and net building
//
// Revision 1.89  2006/09/21 09:57:52  dkrajzew
// debugging
//
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
#include <utils/helpers/SUMODijkstraRouter.h>


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
    // delete the route
    myRoute->decReferenceCnt();
    if(!myRoute->inFurtherUse()) {
        MSRoute::erase(myRoute->getID());
    }
    // delete values in CORN
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
	delete akt;
    {
        for(VehCont::iterator i=myNeighbors.begin(); i!=myNeighbors.end(); ++i) {
            delete (*i).second;
        }
	    myNeighbors.clear();
    }
    {
        for(ClusterCont::iterator i=clusterCont.begin(); i!=clusterCont.end(); ++i) {
            delete (*i);
        }
	    clusterCont.clear();
    }
    {
        for(InfoCont::iterator i=infoCont.begin(); i!=infoCont.end(); ++i) {
            delete (*i).second;
        }
	    infoCont.clear();
    }
}

/////////////////////////////////////////////////////////////////////////////

MSVehicle::MSVehicle( string id,
                      MSRoute* route,
                      SUMOTime departTime,
                      const MSVehicleType* type,
                      int repNo, int repOffset) :
#ifdef RAKNET_DEMO
    Vehicle(),
#endif
    myLastLaneChangeOffset(0),
    myTarget(0),
    myWaitingTime( 0 ),
    myRepetitionNumber(repNo),
    myPeriod(repOffset),
    myID(id),
    myRoute(route),
    myDesiredDepart(departTime),
    myState(0, 0), //
	equipped(false),
	lastUp(0),
	clusterId(-1),
	totalNrOfSavedInfos(0),
	timeSinceStop(0),
	akt(0),
    myLastInfoTime(0),
    myHaveRouteInfo(false),
    myLane( 0 ),
    myType(type),
    myLastBestLanesEdge(0),
    myCurrEdge(0),
    myAllowedLanes(0),
    myMoveReminders( 0 ),
    myOldLaneMoveReminders( 0 ),
    myOldLaneMoveReminderOffsets( 0 ),
    myNoGot(0), myNoSent(0), myNoGotRelevant(0)
{
    if(myRepetitionNumber>0) {
        myRoute->incReferenceCnt();
    }
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
            for(int np=0; np<noCellPhones; np++) {
                string phoneid = getID() + "_cphone#" + toString( np );
    		    myPointerCORNMap[(MSCORN::Pointer) (MSCORN::CORN_P_VEH_DEV_CPHONE+np)] =
                    (void*) new MSDevice_CPhone(*this, phoneid);
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
                                 const MSVehicle* /*neigh*/ )
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
    //@ to be optimized (move to somewhere else)
    if(hasCORNDoubleValue(MSCORN::CORN_VEH_LASTREROUTEOFFSET)) {
        myDoubleCORNMap[MSCORN::CORN_VEH_LASTREROUTEOFFSET] =
            myDoubleCORNMap[MSCORN::CORN_VEH_LASTREROUTEOFFSET] + 1;
    }
    //@ to be optimized (move to somewhere else)
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

    assert(myLFLinkLanes.size()!=0);
    DriveItemVector::iterator i;
    MSLane *currentLane = myLane;
    bool cont = true;
    for(i=myLFLinkLanes.begin(); i!=myLFLinkLanes.end()&&cont; i++) {
        MSLink *link = (*i).myLink;
        bool onLinkEnd = link==0;
        // the vehicle must change the lane on one of the next lanes
        if(!onLinkEnd) {
            if(link->havePriority()&&link->opened()) {
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
            enterLaneAtMove(approachedLane, driven, true);
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
    // compute the way the vehicle may drive when accelerating // !!!?
    SUMOReal dist = boundVSafe + myType->brakeGap(myState.mySpeed);
    SUMOReal vLinkPass = boundVSafe;
    SUMOReal vLinkWait = vLinkPass;
    if(seen>boundVSafe + myType->brakeGap(myState.mySpeed)) {
        // just for the case the vehicle is still very far away from the lane end
        myLFLinkLanes.push_back(DriveProcessItem(0, vLinkPass, vLinkPass));
            return;
    }

    size_t view = 1;
    // loop over following lanes
    while(true) {
        SUMOReal laneLength = nextLane->length();
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


        // !!! optimize this - make this optional
        //  needed to let vehicles wait for all overlapping vehicles in front
        const MSLinkCont &lc = nextLane->getLinkCont();

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
        SUMOReal vsafePredNextLane = 1000;

        // !!! optimize this - make this optional
        SUMOReal r_dist2Pred = seen;
        if(nextLane->getLastVehicle()!=0) {
            r_dist2Pred = r_dist2Pred + nextLane->myLastState.pos() - nextLane->getLastVehicle()->getLength();
        } else {
            r_dist2Pred = r_dist2Pred + nextLane->length();
        }
//        +nextLane->myLastState.pos()-MSVehicleType::getMaxVehicleLength(); // @!!! die echte Länge des fahrzeugs;

#ifdef HAVE_INTERNAL_LANES
        if(MSGlobals::gUsingInternalLanes) {
            for(size_t j=0; j<lc.size(); ++j) {
                MSLane *nl = lc[j]->getViaLane();
                if(nl==0) {
                    nl = lc[j]->getLane();
                }
                if(nl==0) {
                    continue;
                }

                const State &nextLanePred = nl->myLastState;
                SUMOReal dist2Pred = seen;
                if(nl->getLastVehicle()!=0) {
                    dist2Pred = dist2Pred + nextLanePred.pos() - nl->getLastVehicle()->getLength();
                } else {
                    dist2Pred = dist2Pred + nl->length();
                }
//                seen+nextLanePred.pos()-MSVehicleType::getMaxVehicleLength(); // @!!! die echte Länge des fahrzeugs
//            if(nl->length()<dist2Pred&&nl->length()<MSVehicleType::getMaxVehicleLength()) { // @!!! die echte Länge des fahrzeugs


                if(dist2Pred>=0) {
                    // leading vehicle is not overlapping
                    vsafePredNextLane =
                        MIN2(vsafePredNextLane, myType->ffeV(myState.mySpeed, dist2Pred, nextLanePred.speed()));
                    SUMOReal predDec = MAX2((SUMOReal) 0, nextLanePred.speed()-myType->decelAbility() /* !!! decelAbility of leader! */);
                    if(myType->brakeGap(vsafePredNextLane)+vsafePredNextLane*myType->getTau() > myType->brakeGap(predDec) + dist2Pred) {

                        vsafePredNextLane = MIN2(vsafePredNextLane, DIST2SPEED(dist2Pred));
                    }
                } else {
                    // leading vehicle is overlapping (stands within the junction)
                    vsafePredNextLane = MIN2(vsafePredNextLane, myType->ffeV(myState.mySpeed, 0, 0));//dist2Pred/*MAX2((SUMOReal) 0, seen-dist2Pred, 0);
                    // we have to wait in any case
                    break;
                }

//                if(MSGlobals::gUsingInternalLanes) {
                if(nextLanePred.pos()>9000) {
                    dist2Pred = seen + nl->length();
                }

                const MSLinkCont &lc2 = nl->getLinkCont();
                for(size_t j2=0; j2<lc2.size(); ++j2) {
                    MSLane *nl2 = lc2[j2]->getViaLane();
                    if(nl2==0) {
                        nl2 = lc2[j2]->getLane();
                    }
                    if(nl2==0) {
                        continue;
                    }
                    const State &nextLanePred2 = nl2->myLastState;
                    SUMOReal dist2Pred2 = dist2Pred;//dist2Pred+nextLanePred2.pos()-MSVehicleType::getMaxVehicleLength(); // @!!! die echte Länge des fahrzeugs
                    if(nl2->getLastVehicle()!=0) {
                        dist2Pred2 = dist2Pred2 + nextLanePred2.pos() - nl2->getLastVehicle()->getLength();
                    } else {
                        dist2Pred2 = dist2Pred2 + nl2->length();
                    }
                    if(dist2Pred2>=0) {
                        // leading vehicle is not overlapping
                        vsafePredNextLane =
                            MIN2(vsafePredNextLane, myType->ffeV(myState.mySpeed, dist2Pred2, nextLanePred2.speed()));
                        SUMOReal predDec = MAX2((SUMOReal) 0, nextLanePred2.speed()-myType->decelAbility() /* !!! decelAbility of leader! */);
                        if(myType->brakeGap(vsafePredNextLane)+vsafePredNextLane*myType->getTau() > myType->brakeGap(predDec) + dist2Pred2) {

                            vsafePredNextLane = MIN2(vsafePredNextLane, DIST2SPEED(dist2Pred2));
                        }
                    } else {
                        // leading vehicle is overlapping (stands within the junction)
                        vsafePredNextLane = MIN2(vsafePredNextLane, myType->ffeV(myState.mySpeed, 0, 0));//dist2Pred/*MAX2((SUMOReal) 0, seen-dist2Pred, 0);
                        break;
                    }
                }
            }
        } else {
#endif
            const State &nextLanePred = nextLane->myLastState;
            SUMOReal dist2Pred = seen;
            if(nextLane->getLastVehicle()!=0) {
                dist2Pred = dist2Pred + nextLanePred.pos() - nextLane->getLastVehicle()->getLength();
            } else {
                dist2Pred = dist2Pred + nextLane->length();
            }
//                seen+nextLanePred.pos()-MSVehicleType::getMaxVehicleLength(); // @!!! die echte Länge des fahrzeugs
//            if(nl->length()<dist2Pred&&nl->length()<MSVehicleType::getMaxVehicleLength()) { // @!!! die echte Länge des fahrzeugs


            if(dist2Pred>=0) {
                // leading vehicle is not overlapping
                vsafePredNextLane =
                    MIN2(vsafePredNextLane, myType->ffeV(myState.mySpeed, dist2Pred, nextLanePred.speed()));
                SUMOReal predDec = MAX2((SUMOReal) 0, nextLanePred.speed()-myType->decelAbility() /* !!! decelAbility of leader! */);
                if(myType->brakeGap(vsafePredNextLane)+vsafePredNextLane*myType->getTau() > myType->brakeGap(predDec) + dist2Pred) {

                    vsafePredNextLane = MIN2(vsafePredNextLane, DIST2SPEED(dist2Pred));
                }
            } else {
                // leading vehicle is overlapping (stands within the junction)
                vsafePredNextLane = MIN2(vsafePredNextLane, myType->ffeV(myState.mySpeed, 0, 0));//dist2Pred/*MAX2((SUMOReal) 0, seen-dist2Pred, 0);
            }
#ifdef HAVE_INTERNAL_LANES
        }
#endif
                /*
            } else {
                if(dist2Pred>=0) {
                    // leading vehicle is not overlapping
                    vsafePredNextLane =
                        MIN2(vsafePredNextLane, myType->ffeV(myState.mySpeed, dist2Pred, nextLanePred.speed()));
                    SUMOReal predDec = MAX2((SUMOReal) 0, nextLanePred.speed()-myType->decelAbility() / !!! decelAbility of leader! /);
                    if(myType->brakeGap(vsafePredNextLane)+vsafePredNextLane*myType->getTau() > myType->brakeGap(predDec) + dist2Pred) {

                        vsafePredNextLane = MIN2(vsafePredNextLane, DIST2SPEED(dist2Pred));
                    }
                } else {
                    // leading vehicle is overlapping (stands within the junction)
                    vsafePredNextLane = MIN2(vsafePredNextLane, myType->ffeV(myState.mySpeed, 0, 0));//dist2Pred/MAX2((SUMOReal) 0, seen-dist2Pred, 0);
                }
            }
            */

            // compute the velocity to use when the link may be used
        vLinkPass =
            MIN3(vLinkPass, vmaxNextLane, vsafePredNextLane/*, vsafeNextLaneEnd*/);

        // if the link may not be used (is blocked by another vehicle) then let the
        //  vehicle decelerate until the end of the street
        vLinkWait =
            MIN3(vLinkPass, vLinkWait, myType->ffeS(myState.mySpeed, seen));

        if((*link)->amYellow()&&SPEED2DIST(vLinkWait)+myState.myPos<laneLength) {
            myLFLinkLanes.push_back(DriveProcessItem(*link, vLinkWait, vLinkWait));
            return;
        }
        // valid, when a vehicle is not on a priorised lane
        if(!(*link)->havePriority()) {
            // if it has already decelerated to let priorised vehicles pass
            //  and when the distance to the vehicle on the next lane allows moving
            //  (the check whether other incoming vehicles may stop this one is done later)
            // then let it pass
            //  [m]>
            if(seen>=myType->approachingBrakeGap(myState.mySpeed)&&r_dist2Pred>0) {
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
    if(myLane==0) {
        return Position2D(-1000, -1000);
    }
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
MSVehicle::isEquipped() const
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


/////////////////////////////////////////////////////////////////////////////

void
MSVehicle::enterLaneAtMove( MSLane* enteredLane, SUMOReal driven, bool inBetweenJump )
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
		(*myCurrEdge)->addEquippedVehicle(getID(), this);
        delete akt;
        akt = new Information(0, MSNet::getInstance()->getCurrentTimeStep());
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
        delete akt;
		(*myCurrEdge)->addEquippedVehicle(getID(), this);
        akt = new Information(0, MSNet::getInstance()->getCurrentTimeStep());
	}
}

/////////////////////////////////////////////////////////////////////////////

void
MSVehicle::leaveLaneAtMove( SUMOReal /*driven*/ )
{
#ifdef GUI_DEBUG
    if(gSelected.isSelected(GLO_VEHICLE, ((GUIVehicle*) this)->getGlID())) {
        int blb = 0;
    }
#endif
    if(isEquipped()){
        // leave the c2c-edge
        (*myCurrEdge)->removeEquippedVehicle(getID());
        // checke whether the vehicle needed longer than expected
        float factor = (*myCurrEdge)->getEffort(this, MSNet::getInstance()->getCurrentTimeStep());
//        std::map<const MSEdge * const, Information *>::iterator i = infoCont.find(*myCurrEdge);
        float nt = (float) (MSNet::getInstance()->getCurrentTimeStep() - akt->time);
        if(nt>10&&nt>factor*MSGlobals::gAddInfoFactor){ // !!! explicite
            // if so, check whether an information about the edge was already existing
            std::map<const MSEdge * const, Information *>::iterator i = infoCont.find(*myCurrEdge);
            if(i==infoCont.end()) {
                // no, add the new information
                Information *info = new Information(*akt);
                info->neededTime = nt;
                infoCont[*myCurrEdge] = info;
                i = infoCont.find(*myCurrEdge);
            } else {
                // yes, update the existing information
                (*i).second->neededTime = nt;
            }
            // save the information
            MSCORN::saveSavedInformationData(MSNet::getInstance()->getCurrentTimeStep(),
                getID(),(*myCurrEdge)->getID(),"congestion",(*i).second->time,nt,-1);
            totalNrOfSavedInfos++;
        } else if(infoCont.find(*myCurrEdge)!=infoCont.end()) {
            // ok, we could pass the edge faster than assumed; remove the information
            infoCont.erase(*myCurrEdge);
        }
        delete akt;
		akt = 0;
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
    MSVehicle *ret = MSNet::getInstance()->getVehicleControl().buildVehicle(
        StringUtils::version1(myID), myRoute, myDesiredDepart+myPeriod,
        myType, myRepetitionNumber-1, myPeriod);
    for(std::list<Stop>::const_iterator i=myStops.begin(); i!=myStops.end(); ++i) {
        ret->myStops.push_back(*i);
    }
    return ret;
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
MSVehicle::onTripEnd(bool /*wasAlreadySet*/)
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
    // remove c2c connections // !!! delete them ,too!!!
    {
        for(VehCont::iterator i=myNeighbors.begin(); i!=myNeighbors.end(); ++i) {
            delete (*i).second;
        }
	    myNeighbors.clear();
    }
    {
        for(ClusterCont::iterator i=clusterCont.begin(); i!=clusterCont.end(); ++i) {
            delete (*i);
        }
	    clusterCont.clear();
    }
    {
        for(InfoCont::iterator i=infoCont.begin(); i!=infoCont.end(); ++i) {
            delete (*i).second;
        }
	    infoCont.clear();
    }
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
        myLastBestLanesEdge = 0;
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
        myAllowedLanes.clear();
        rebuildAllowedLanes();
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
    myLastBestLanesEdge = 0;
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


const std::vector<std::vector<MSVehicle::LaneQ> > &
MSVehicle::getBestLanes() const
{
#ifdef GUI_DEBUG
    if(gSelected.isSelected(GLO_VEHICLE, ((GUIVehicle*) this)->getGlID())) {
        int blb = 0;
    }
#endif
    if(myLastBestLanesEdge==myLane->getEdge()) {
        std::vector<LaneQ> &lanes = *myBestLanes.begin();
        std::vector<LaneQ>::iterator i;
        for(i=lanes.begin(); i!=lanes.end(); ++i) {
            SUMOReal v = 0;
            for(std::vector<MSLane*>::const_iterator j=(*lanes.begin()).joined.begin(); j!=(*lanes.begin()).joined.end(); ++j) {
                v += (*j)->getVehLenSum();
            }
            v += (*lanes.begin()).lane->getVehLenSum();
            (*lanes.begin()).v = v;
        }
        return myBestLanes;
    }

    myBestLanes.clear();
    myLastBestLanesEdge = myLane->getEdge();
    SUMOReal MIN_DIST = 3000;
    MSRouteIterator ce = myCurrEdge;
    int seen = 0;
    float dist = -(*myLastBestLanesEdge->getLanes())[0]->length();//-getPositionOnLane();
    // compute initial list
    // each item in the list is a list of lane descriptions
    while(seen<4&&dist<MIN_DIST&&ce!=myRoute->end()) {
        const MSEdge::LaneCont * const lanes = (*ce)->getLanes();
        myBestLanes.push_back(std::vector<LaneQ>());
        std::vector<LaneQ> &curr = *(myBestLanes.end()-1);
        bool gotOne = false;
        size_t i;
        for(i=0; i<lanes->size(); i++) {
            curr.push_back(LaneQ());
            LaneQ &currQ = *(curr.end()-1);
            if((ce+1)!=myRoute->end()) {
                const MSEdge::LaneCont *allowed = (*ce)->allowedLanes(**(ce+1), myType->getVehicleClass());
                if(allowed!=0&&find(allowed->begin(), allowed->end(), (*lanes)[i])!=allowed->end()) {
                    currQ.t1 = true;
                    gotOne = true;
                } else {
                    currQ.t1 = false;
                }
            } else {
                currQ.t1 = true;
                gotOne = true;
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
        ce = myCurrEdge + myBestLanes.size() - 1;
        std::vector<std::vector<LaneQ> >::reverse_iterator i;
        for(i=myBestLanes.rbegin()+1; i!=myBestLanes.rend(); ++i, --ce) {
            std::vector<LaneQ> &curr = *i;
            size_t j;
            std::vector<int> bestNext;
            SUMOReal bestLength = -1;
            bool gotOne = false;
            for(j=0; j<curr.size(); ++j) {
                if(curr[j].length>bestLength) {
                    bestNext.clear();
                    bestLength = curr[j].length;
                    bestNext.push_back(j);
                } else if(curr[j].length==bestLength) {
                    bestNext.push_back(j);
                }
                if(!curr[j].t1) {
                    continue;
                }
                std::vector<LaneQ> &next = *(i-1);
                const MSLinkCont &lc = curr[j].lane->getLinkCont();
                bool oneFound = false;
                for(MSLinkCont::const_iterator k=lc.begin(); k!=lc.end()&&!oneFound; ++k) {
                    MSLane *c = (*k)->getLane();
                    for(std::vector<LaneQ>::iterator l=next.begin(); l!=next.end()&&!oneFound; ++l) {
                        if((*l).lane==c/*&&curr[j].t1*/&&(*l).t1) {
                            gotOne = true;
                            /*
                            (*l).length += next[j].length;
                            (*l).v += next[j].v;
                            (*l).wish++;
                            (*l).alllength += next[j].alllength;
                            */
                            oneFound = true;
                            curr[j].length += (*l).length;
                            curr[j].v += (*l).v;
                            curr[j].wish++;// += (*l).length;
                            curr[j].alllength = (*l).alllength;
                            if((*l).joined.size()!=0) {
                                copy((*l).joined.begin(), (*l).joined.end(), back_inserter(curr[j].joined));
                            } else {
                                (*l).joined.push_back((*l).lane);
                            }
                        }
                    }
                }
            }
            if(!gotOne) {
                // ok, there was no direct matching connection
                // first hack: get the first to the next edge
                for(j=0; j<curr.size(); ++j) {
                    if(!curr[j].t1) {
                        continue;
                    }
                    std::vector<LaneQ> &next = *(i-1);
                    const MSLinkCont &lc = curr[j].lane->getLinkCont();
                    bool oneFound = false;
                    for(MSLinkCont::const_iterator k=lc.begin(); k!=lc.end()&&!oneFound; ++k) {
                        MSLane *c = (*k)->getLane();
                        for(std::vector<LaneQ>::iterator l=next.begin(); l!=next.end(); ++l) {
                            if((*l).lane==c/*&&curr[j].t1&&(*l).t1*/) {
                                /*
                                cout << "c3111 " << endl;
                                (*l).length += next[j].lane->length();//.length;
                                cout << "c3112 " << endl;
                                (*l).v += next[j].lane->getDensity();//;
                                cout << "c3113 " << endl;
                                (*l).wish++;
                                cout << "c3114 " << endl;
                                (*l).alllength += next[j].lane->length();//.alllength;
                                cout << "c3115 " << endl;
                                */
                                curr[j].length += (*l).lane->length();//.length;
                                curr[j].v += (*l).lane->getDensity();//.v;
                                curr[j].wish++;// += (*l).length;
                                curr[j].alllength = (*l).alllength;
                                (*l).joined.push_back((*l).lane);
                            }
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
        for(i=myBestLanes.begin(); i!=myBestLanes.end(); ++i) {
            std::vector<LaneQ> &curr = *i;
            int best = 0;
            SUMOReal bestLength = 0;
            size_t j;
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
    return myBestLanes;

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
MSVehicle::setCORNColor(SUMOReal red, SUMOReal green, SUMOReal blue)
{
    myDoubleCORNMap[MSCORN::CORN_VEH_OWNCOL_RED] = red;
    myDoubleCORNMap[MSCORN::CORN_VEH_OWNCOL_GREEN] = green;
    myDoubleCORNMap[MSCORN::CORN_VEH_OWNCOL_BLUE] = blue;
}


void
MSVehicle::saveState(std::ostream &os, long /*what*/)
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
}




void
MSVehicle::addVehNeighbors(MSVehicle *veh, SUMOTime time)
{
    if(&veh->getLane()==0||myLane==0) {
        // obviously, one of the vehicles is being teleported
        return;
    }

    // check whether the other vehicle is in range
    if(computeDistance(this, veh)) {
        Position2D pos1 = getPosition();
	    Position2D pos2 = veh->getPosition();
		std::map<MSVehicle * const, C2CConnection*>::iterator i = myNeighbors.find(veh);
		if(i==myNeighbors.end()) {
            // the vehicles will establish a new connection
			C2CConnection *con = new C2CConnection;
            con->connectedVeh = veh;
			con->state = disconnected;
//			con->timeSinceSeen = 1;
//			con->timeSinceConnect = 0;
			con->lastTimeSeen = time;
			myNeighbors[veh] = con;
            // the other car must inform THIS vehicle if it's removed from the network
            veh->quitRemindedEntered(this);
		}else{
            // ok, the vehicles already interact
            //  increment the connection time
            (*i).second->lastTimeSeen = time;
		}
		MSCORN::saveVehicleInRangeData(time, getID(), veh->getID(),
            pos1.x(),pos1.y(), pos2.x(),pos2.y(),-1);
	}
}


void
MSVehicle::cleanUpConnections(SUMOTime time)
{
    std::vector<MSVehicle *> toErase;
    std::map<MSVehicle * const, C2CConnection*>::iterator i;
    // recheck connections
    for(i=myNeighbors.begin(); i!=myNeighbors.end(); ++i) {
        MSVehicle * const neigh = (*i).first;
        if((*i).second->lastTimeSeen != time) {
            // the vehicle is not longer in range
            toErase.push_back(neigh);
            // the other vehicle must no longer inform us about being removed from the network
            neigh->quitRemindedLeft(this);
        }/* !!! else {
            // the vehicle is still in range
            ((*i).second->timeSinceSeen)++;
            if(((*i).second->state!=dialing) && ((*i).second->state!=disconnected)){
	            ((*i).second->timeSinceConnect)++;
            }
        }*/
    }

    // go through the list of invalid connections, erase them
    for(vector<MSVehicle *>::iterator j=toErase.begin(); j!=toErase.end(); ++j) {
        i = myNeighbors.find(*j);
        delete (*i).second;
        myNeighbors.erase(i);
    }
}


bool
MSVehicle::computeDistance(MSVehicle* veh1, MSVehicle* veh2)
{
	bool b = false;
	Position2D pos1 = veh1->getPosition();
	Position2D pos2 = veh2->getPosition();
    if(pos1.x()==-1000||pos2.x()==-1000) {
        return false;
    }
	if(fabs(pos1.x()-pos2.x())<MSGlobals::gLANRange && fabs(pos1.y()-pos2.y())<MSGlobals::gLANRange){
		SUMOReal distance = sqrt(pow(pos1.x()-pos2.x(),2) + pow(pos1.y()-pos2.y(),2));
		if((distance>0)&&(distance<=MSGlobals::gLANRange)){
			b = true;
		}
	}
	return b;
}


void
MSVehicle::updateInfos(SUMOTime time)
{
    // first, count how long the vehicle is waiting at the same position
    if(myState.speed()<1.) {
        timeSinceStop++;
    } else {
        timeSinceStop = 0;
    }

    // second, save the information as "Congestion", if the vehicle is at the same
    // position longer as 2 minutes
    if(timeSinceStop > 120) {
        SUMOReal timeByMeanSpeed1 = (SUMOReal) timeSinceStop;
        if(akt!=0&&myLane!=0) {
            SUMOReal neededTime = (SUMOReal) (MSNet::getInstance()->getCurrentTimeStep() - akt->time);
            timeByMeanSpeed1 = myLane->length() / (myState.pos()/neededTime);
        }
        SUMOReal estimatedTime = timeByMeanSpeed1;
        map<const MSEdge * const, Information *>::iterator i = infoCont.find(*myCurrEdge);
        if(i == infoCont.end()){
            // this is a new information about an edge where speed is too low
            Information *info = new Information(estimatedTime, time);
            infoCont[*myCurrEdge] = info;
            MSCORN::saveSavedInformationData(time, getID(), (*myCurrEdge)->getID(), "congestion", info->time, 0, -1);
            totalNrOfSavedInfos++;
        } else {
            // this edge is already known as being too slow
            // - replace prior information by own
            (*i).second->neededTime = estimatedTime;
            (*i).second->time = time;
		}
    }

    // remove information older than wished
    std::vector<const MSEdge * > toErase;
    std::map<const MSEdge * const, Information*>::iterator j = infoCont.begin();
    for(; j!= infoCont.end(); ++j) {
        if((*j).second->time < time - MSGlobals::gLANRefuseOldInfosOffset) {
            toErase.push_back((*j).first);
        }
    }
        // go through the list of invalid information, erase them
    for(vector<const MSEdge *>::iterator k=toErase.begin(); k!=toErase.end(); ++k) {
        infoCont.erase(infoCont.find(*k));
    }
}


void
MSVehicle::removeOnTripEnd( MSVehicle *veh )
{
    assert(myNeighbors.find(veh)!=myNeighbors.end());
    std::map<MSVehicle * const, C2CConnection*>::iterator i = myNeighbors.find(veh);
    delete (*i).second;
    myNeighbors.erase(i);
    quitRemindedLeft(veh);
}


bool
MSVehicle::knowsEdgeTest(MSEdge &edge) const
{
    return infoCont.find(&edge)!=infoCont.end();
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
MSVehicle::getClusterId(void) const
{
	return clusterId;
}


int
MSVehicle::buildMyCluster(int myStep, int clId)
{
    int count = 1;
    // build the cluster
    {
        clusterId = clId;
        std::map<MSVehicle * const, C2CConnection*>::iterator i;
        for(i=myNeighbors.begin(); i!=myNeighbors.end(); i++){
            if((*i).first->getClusterId()<0){
                count++;
                (*i).second->connectedVeh->setClusterId(clId);
                clusterCont.push_back((*i).second);
                std::map<MSVehicle * const, C2CConnection*>::iterator j;
                for(j=(*i).first->myNeighbors.begin(); j!=(*i).second->connectedVeh->myNeighbors.end(); j++){
                    if((*i).first->getClusterId()<0){
                        count++;
                        (*i).first->setClusterId(clId);
                        clusterCont.push_back((*j).second);
                    }
                }
            } else if((*i).second->connectedVeh->getClusterId()==clusterId){
                // du bist zwar mein Nachbarn, aber du würdest von einem anderen Nachbarn von mir schon eingeladen,
                // dann werde ich deine nachbarn einladen.
                std::map<MSVehicle * const, C2CConnection*>::iterator j;
                for(j=(*i).first->myNeighbors.begin(); j!=(*i).second->connectedVeh->myNeighbors.end(); j++){
                    if((*i).first->getClusterId()<0){
                        count++;
                        (*i).first->setClusterId(clId);
                        clusterCont.push_back((*j).second);
                    }
                }
            }
        }
    }

    // write output
    {
        ostringstream vehs;
        for(std::vector<C2CConnection*>::const_iterator i=clusterCont.begin(); i!=clusterCont.end(); ++i) {
            if(i!=clusterCont.begin()) {
                vehs << ' ';
            }
            vehs << (*i)->connectedVeh->getID();
        }
	    MSCORN::saveClusterInfoData(myStep, clId, vehs.str(), count, -1);
    }
	return count;
}


void
MSVehicle::sendInfos(SUMOTime time)
{
    // the number of possible packets
	size_t numberOfSendingPos = MSGlobals::gNumberOfSendingPos; // 732
    // the number of information per packet
	size_t infoPerPaket = MSGlobals::gInfoPerPaket; // 14
	size_t numberOfInfo = numberOfSendingPos*infoPerPaket; // 10248

	if(infoCont.size()>0 && numberOfSendingPos>0){
		// send information to direct neighbors
		for(VehCont::const_iterator i=myNeighbors.begin(); i!=myNeighbors.end(); ++i) {
            // compute the number of netto-sendable packets
            // !!! maybe it would be prettier to have computed this by the retriever
			int nofP = numOfInfos(this, (*i).first);
            if(nofP>numberOfInfo) {
				nofP = (int) numberOfInfo;
            }
            // send the computed number of information to the neighbor
			(*i).first->transferInformation(getID(), infoCont, nofP, time);
		}
        // reduce the number of packets that still may be sent
        size_t sentBruttoP = MIN2((size_t) ceil((SUMOReal) (infoCont.size())/(SUMOReal) infoPerPaket), (size_t) numberOfSendingPos);
        myNoSent += sentBruttoP;
		numberOfSendingPos = numberOfSendingPos - sentBruttoP;
		numberOfInfo = numberOfInfo - infoCont.size();
	}

    if(numberOfInfo<=0) {
        return;
    }

    // now let all neighbors send
    for(ClusterCont::const_iterator o=clusterCont.begin(); o!=clusterCont.end()&&numberOfInfo>0&&numberOfSendingPos>0; ++o) {
        if((*o)->connectedVeh->infoCont.size()>0 && numberOfSendingPos>0){
            for(VehCont::const_iterator j=(*o)->connectedVeh->myNeighbors.begin(); j!=(*o)->connectedVeh->myNeighbors.end(); ++j){
                // compute the number of netto-sendable packets
                // !!! maybe it would be prettier to have computed this by the retriever
                int nofP = numOfInfos((*j).second->connectedVeh,(*o)->connectedVeh);
                if(nofP>numberOfInfo) {
                    nofP = (int) numberOfInfo;
                }
                // send the computed number of information to the neighbor
                (*j).second->connectedVeh->transferInformation((*o)->connectedVeh->getID(),(*o)->connectedVeh->infoCont, nofP, time);
            }
            size_t sentBruttoP = MIN2((size_t) ceil((SUMOReal) ((*o)->connectedVeh->infoCont.size())/(SUMOReal) infoPerPaket), (size_t) numberOfSendingPos);
		    numberOfSendingPos = numberOfSendingPos - sentBruttoP;
            numberOfInfo = numberOfInfo - (*o)->connectedVeh->infoCont.size();
        }
    }
	clusterCont.clear();
}


int
MSVehicle::numOfInfos(MSVehicle *veh1, MSVehicle* veh2)
{
	Position2D pos1 = veh1->getPosition();
	Position2D pos2 = veh2->getPosition();
	SUMOReal distance = sqrt(pow(pos1.x()-pos2.x(),2) + pow(pos1.y()-pos2.y(),2));
	SUMOReal x = (SUMOReal) (((-2.3*distance + 1650.)*MSGlobals::gNumberOfSendingPos)/1500.); //approximation function
	return (int) (x*MSGlobals::gInfoPerPaket);
}


bool
MSVehicle::willPass(const MSEdge * const edge) const
{
    return find(myCurrEdge, myRoute->end(), edge)!=myRoute->end();
}


void
MSVehicle::transferInformation(const std::string &senderID, const InfoCont &infos,
                               int NofP, SUMOTime currentTime)
{
    if(NofP>0&&infos.size()>0) {
        myLastInfoTime = currentTime;
    }
    int count = 0;
    std::map<const MSEdge * const, Information *>::const_iterator i;
    // go through the saved information
    for(i=infos.begin(); i!=infos.end() && count<NofP; ++i) {
        std::map<const MSEdge * const, Information *>::iterator j = infoCont.find((*i).first);
        if(j==infoCont.end()) {
            if((*i).second->neededTime > 0) {
                infoCont[(*i).first] = new Information(*(*i).second);
                ++myNoGot;
            }
        } else if(((*i).second->time > (*j).second->time) &&  (*i).second->neededTime > 0){
            // save the information about a previously known edge
            //  (it is newer than the stored)
            delete infoCont[(*i).first];
            infoCont[(*i).first] = new Information(*(*i).second);
            ++myNoGot;
        }
        count++;
        MSCORN::saveTransmittedInformationData(-1,senderID,getID(),(*i).first->getID(),(*i).second->time,(*i).second->neededTime,-1);
        // if the edge is on the route, mark that a relevant information has been added
        bool bWillPass = willPass((*i).first);
        if(bWillPass) {
            myHaveRouteInfo = true;
            ++myNoGotRelevant;
        }
    }
}


SUMOReal
MSVehicle::getC2CEffort(const MSEdge * const e, SUMOTime /*t*/) const
{
    if(infoCont.find(e)==infoCont.end()) {
        return -1;
    }
    return infoCont.find(e)->second->neededTime;
}


void
MSVehicle::checkReroute(SUMOTime t)
{
    // do not try to reroute when no new information is available
    if(myLastInfoTime!=t) {
        return;
    }
    // do not try to reroute when no information about the own route is available
    if(!myHaveRouteInfo) {
        return;
    }
    // try to reroute
#ifdef GUI_DEBUG
    if(gSelected.isSelected(GLO_VEHICLE, ((GUIVehicle*) this)->getGlID())) {
        int blb = 0;
    }
#endif

    if(myStops.size()==0) {
        myHaveRouteInfo = false;
        // check whether to reroute
        SUMODijkstraRouter<MSEdge, MSVehicle, prohibited_withRestrictions<MSEdge, MSVehicle>, MSEdge> router(MSEdge::dictSize(), true, &MSEdge::getC2CEffort);
        std::vector<const MSEdge*> edges;
        router.compute(*myCurrEdge, myRoute->getLastEdge(), (const MSVehicle * const) this,
            MSNet::getInstance()->getCurrentTimeStep(), edges);
        // check whether the new route is the same as the prior
        MSRouteIterator ri = myCurrEdge;
        std::vector<const MSEdge*>::iterator ri2 = edges.begin();
        while(ri!=myRoute->end()&&ri2!=edges.end()&&*ri==*ri2) {
            ri++;
            ri2++;
        }
        if(ri!=myRoute->end()||ri2!=edges.end()) {
            int rerouteIndex = 0;
            if(myDoubleCORNMap.find(MSCORN::CORN_VEH_NUMBERROUTE)!=myDoubleCORNMap.end()) {
                rerouteIndex = (int) myDoubleCORNMap[MSCORN::CORN_VEH_NUMBERROUTE];
            }
            string nid = myRoute->getID() + "#" + toString(rerouteIndex);
            MSRoute *rep = new MSRoute(nid, edges, true);
            if(!MSRoute::dictionary(nid, rep)) {
                //cout << "Error: Could not insert route ''" << endl;
            } else {
                MSCORN::setWished(MSCORN::CORN_VEH_SAVEREROUTING);
                replaceRoute(rep, MSNet::getInstance()->getCurrentTimeStep());
            }
        }
    }
}


size_t
MSVehicle::getNoGot() const
{
    return myNoGot;
}


size_t
MSVehicle::getNoSent() const
{
    return myNoSent;
}


size_t
MSVehicle::getNoGotRelevant() const
{
    return myNoGotRelevant;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
