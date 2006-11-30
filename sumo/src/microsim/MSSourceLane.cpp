/***************************************************************************
                          MSSourceLane.cpp  -
                          Same as MSLane, but with another emission
                          behaviour
                             -------------------
    begin                : Mon, 25 Nov 2002
    copyright            : (C) 2001 by DLR http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
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
// Revision 1.19  2006/11/30 12:47:36  dkrajzew
// debugging c2c based rerouting
//
// Revision 1.18  2006/10/12 08:09:15  dkrajzew
// fastened up lane changing
//
// Revision 1.17  2006/09/18 10:08:33  dkrajzew
// added vehicle class support to microsim
//
// Revision 1.16  2006/07/06 07:33:22  dkrajzew
// rertrieval-methods have the "get" prependix; EmitControl has no dictionary; MSVehicle is completely scheduled by MSVehicleControl; new lanechanging algorithm
//
// Revision 1.15  2006/05/15 05:53:56  dkrajzew
// began with the extraction of the car-following-model from MSVehicle
//
// Revision 1.15  2006/05/08 10:59:34  dkrajzew
// began with the extraction of the car-following-model from MSVehicle
//
// Revision 1.14  2006/04/05 05:27:34  dkrajzew
// retrieval of microsim ids is now also done using getID() instead of id()
//
// Revision 1.13  2006/03/17 09:01:12  dkrajzew
// .icc-files removed
//
// Revision 1.12  2005/10/07 11:37:45  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.11  2005/09/22 13:45:51  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.10  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.9  2005/07/12 12:26:29  dkrajzew
// debugging vehicle emission
//
// Revision 1.8  2005/05/04 07:55:29  dkrajzew
// added the possibility to load lane geometries into the non-gui simulation; simulation speedup due to avoiding multiplication with 1;
//
// Revision 1.7  2004/11/23 10:20:10  dkrajzew
// new detectors and tls usage applied; debugging
//
// Revision 1.6  2004/07/02 09:58:08  dkrajzew
// MeanData refactored (moved to microsim/output); numerical id for online routing added
//
// Revision 1.5  2003/09/05 15:15:23  dkrajzew
// removed some unneeded code
//
// Revision 1.4  2003/07/16 15:28:00  dkrajzew
// MSEmitControl now only simulates lanes which do have vehicles; the edges do not go through the lanes, the EdgeControl does
//
// Revision 1.3  2003/06/18 11:30:26  dkrajzew
// debug outputs now use a DEBUG_OUT macro instead of cout; this shall ease the search for further couts which must be redirected to the messaaging subsystem
//
// Revision 1.2  2003/05/20 09:31:46  dkrajzew
// emission debugged; movement model reimplemented (seems ok); detector output debugged; setting and retrieval of some parameter added
//
// Revision 1.1  2003/02/07 10:41:50  dkrajzew
// updated
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

#include <utils/common/UtilExceptions.h>
#include "MSVehicle.h"
#include "MSVehicleType.h"
#include "MSEdge.h"
#include "MSJunction.h"
#include "MSLogicJunction.h"
#include "output/MSInductLoop.h"
#include "MSLink.h"
#include "MSSourceLane.h"
#include <cmath>
#include <bitset>
#include <iostream>
#include <cassert>
#include <functional>
#include <algorithm>
#include <iterator>
#include <exception>
#include <climits>

#ifdef ABS_DEBUG
#include "MSDebugHelper.h"
#endif

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * some definitions (debugging only)
 * ======================================================================= */
#define DEBUG_OUT cout


/* =========================================================================
 * member method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * methods of MSSourceLane
 * ----------------------------------------------------------------------- */
MSSourceLane::~MSSourceLane()
{
}

/////////////////////////////////////////////////////////////////////////////

MSSourceLane::MSSourceLane(string id, SUMOReal maxSpeed,
                           SUMOReal length, MSEdge* edge, size_t numericalID,
                           const Position2DVector &shape,
                           const std::vector<SUMOVehicleClass> &allowed,
                           const std::vector<SUMOVehicleClass> &disallowed)
    : MSLane(id, maxSpeed, length, edge, numericalID, shape, allowed, disallowed)
{
}

/////////////////////////////////////////////////////////////////////////////

bool
MSSourceLane::emit( MSVehicle& veh )
{
    // If this lane is empty, set newVeh on position beyond safePos =
    // brakeGap(laneMaxSpeed) + MaxVehicleLength. (in the hope of that
    // the precening lane hasn't a much higher MaxSpeed)
    // This safePos is ugly, but we will live with it in this revision.
/*    SUMOReal safePos = pow( myMaxSpeed, 2 ) / ( 2 * MSVehicleType::minDecel() ) +
                    MSVehicle::tau() + MSVehicleType::maxLength();
    assert( safePos < myLength ); // Lane has to be longer than safePos,
    // otherwise emission (this kind of emission) makes no sense.
    !!! not in Source lanes
    */


    // Here the emission starts
    if ( empty() ) {

        return emitTry( veh );
    }

    // Try to emit as last veh. (in driving direction)
    VehCont::iterator leaderIt = myVehicles.begin();
    if ( emitTry( veh, leaderIt ) ) {

        return true;
    }

    // do not try to push vehicles in between on sources
    return false;
}

/////////////////////////////////////////////////////////////////////////////

bool
MSSourceLane::emitTry( MSVehicle& veh )
{
    // on sources, no vehicles may arrive from the back
    myVehicles.push_front( &veh );
    myUseDefinition->noVehicles++;
    myUseDefinition->vehLenSum += veh.getLength();
    MSVehicle::State state( myLength>1 ? (SUMOReal) (myLength - 1.) : 0, 0 );
    veh.enterLaneAtEmit( this, state );
    assert(myUseDefinition->noVehicles==myVehicles.size());
#ifdef ABS_DEBUG
    if(debug_searched2==veh.getID()||debug_searched2==veh.getID()) {
        DEBUG_OUT << "Using emitTry( MSVehicle& veh )/2:" << debug_globaltime << endl;
    }
#endif

    return true;
}

/////////////////////////////////////////////////////////////////////////////

bool
MSSourceLane::emitTry( MSVehicle& veh, VehCont::iterator leaderIt )
{
    // emission as last car (in driving direction)
    MSVehicle *leader = *leaderIt;
    // get invoked vehicles' positions
    SUMOReal leaderPos = (*leaderIt)->getPositionOnLane() - (*leaderIt)->getLength();
    // get secure gaps
    SUMOReal frontGapNeeded = veh.getSecureGap(veh.getSpeed(), leader->getSpeed(), leader->getLength());
    // compute needed room
    SUMOReal frontMax = leaderPos - frontGapNeeded;
    // check whether there is enough room
    if(frontMax>0) {
        // emit vehicle if so
        MSVehicle::State state(frontMax, 0);
        veh.enterLaneAtEmit( this, state );
        myVehicles.push_front( &veh );
        myUseDefinition->noVehicles++;
        myUseDefinition->vehLenSum += veh.getLength();
        assert(myUseDefinition->noVehicles==myVehicles.size());

#ifdef ABS_DEBUG
    if(debug_searched1==veh.getID()||debug_searched2==veh.getID()) {
        DEBUG_OUT << "Using emitTry( MSVehicle& veh, VehCont::iterator leaderIt )/1:" << debug_globaltime << endl;
    }
#endif

        return true;
    }
    return false;
}
/*
bool
MSSourceLane::emitTry( MSVehicle& veh, VehCont::iterator leaderIt )
{
    // on sources, no vehicles may arrive from the back
    if(!veh.overlap(&veh, *leaderIt) &&
        veh.gap2pred(*(*leaderIt))>veh.vaccel(this) ) { // !!!

        veh.enterLaneAtEmit( this );
        myVehicles.push_front( &veh );

#ifdef ABS_DEBUG
    if(debug_searched1==veh.id()||debug_searched2==veh.id()) {
        cout << "Using source::emitTry( MSVehicle& veh, VehCont::iterator leaderIt )/1" << veh.pos() << ", " <<veh.speed() <<  endl;
    }
#endif

        return true;
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////

bool
MSSourceLane::enoughSpace( MSVehicle& veh,
                     SUMOReal followPos, SUMOReal leaderPos, SUMOReal safeSpace )
{
    SUMOReal free = leaderPos - followPos - safeSpace;
    if ( free >= 0.01 ) {

        // prepare vehicle with it's position
        MSVehicle::State state;
        state.setPos( followPos + safeSpace + free / 2 );
        veh.moveSetState( state );
        return true;
    }
    return false;
}*/


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
