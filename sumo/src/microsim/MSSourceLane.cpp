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
// Revision 1.2  2003/05/20 09:31:46  dkrajzew
// emission debugged; movement model reimplemented (seems ok); detector output debugged; setting and retrieval of some parameter added
//
// Revision 1.1  2003/02/07 10:41:50  dkrajzew
// updated
//
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <utils/common/UtilExceptions.h>
#include "MSVehicle.h"
#include "MSVehicleType.h"
#include "MSEdge.h"
#include "MSJunction.h"
#include "MSLogicJunction.h"
#include "MSInductLoop.h"
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


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


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

MSSourceLane::MSSourceLane( MSNet &net,
                string id,
                double maxSpeed,
                double length,
                MSEdge* edge
                )
    : MSLane(net, id, maxSpeed, length, edge)
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
    double safePos = pow( myMaxSpeed, 2 ) / ( 2 * MSVehicleType::minDecel() ) +
                    MSVehicle::tau() + MSVehicleType::maxLength();
    assert( safePos < myLength ); // Lane has to be longer than safePos,
    // otherwise emission (this kind of emission) makes no sense.

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
    veh.enterLaneAtEmit( this );
    myVehicles.push_front( &veh );
#ifdef ABS_DEBUG
	if(MSNet::searched1==veh.id()||MSNet::searched2==veh.id()) {
		cout << "Using emitTry( MSVehicle& veh )/2:" << MSNet::globaltime << endl;
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

#ifdef ABS_DEBUG
	if(MSNet::searched1==veh.id()||MSNet::searched2==veh.id()) {
		cout << "Using emitTry( MSVehicle& veh, VehCont::iterator leaderIt )/1:" << MSNet::globaltime << endl;
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
	if(MSNet::searched1==veh.id()||MSNet::searched2==veh.id()) {
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
                     double followPos, double leaderPos, double safeSpace )
{
    double free = leaderPos - followPos - safeSpace;
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

#ifdef DISABLE_INLINE
#include "MSSourceLane.icc"
#endif

// Local Variables:
// mode:C++
// End:
