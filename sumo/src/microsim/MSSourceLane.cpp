/****************************************************************************/
/// @file    MSSourceLane.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 25 Nov 2002
/// @version $Id$
///
// Same as MSLane, but with another emission
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/UtilExceptions.h>
#include "MSVehicle.h"
#include "MSVehicleType.h"
#include "MSEdge.h"
#include "MSJunction.h"
#include "MSLogicJunction.h"
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

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// member method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * methods of MSSourceLane
 * ----------------------------------------------------------------------- */
MSSourceLane::~MSSourceLane()
{}

/////////////////////////////////////////////////////////////////////////////

MSSourceLane::MSSourceLane(string id, SUMOReal maxSpeed,
                           SUMOReal length, MSEdge* edge, size_t numericalID,
                           const Position2DVector &shape,
                           const std::vector<SUMOVehicleClass> &allowed,
                           const std::vector<SUMOVehicleClass> &disallowed)
        : MSLane(id, maxSpeed, length, edge, numericalID, shape, allowed, disallowed)
{}

/////////////////////////////////////////////////////////////////////////////

bool
MSSourceLane::emit(MSVehicle& veh, bool isReinsertion) throw()
{
    return MSLane::emit(veh, isReinsertion);

    //return isEmissionSuccess(&veh, MSVehicle::State(0, 0));
    /*
    // If this lane is empty, set newVeh on position beyond safePos =
    // brakeGap(laneMaxSpeed) + MaxVehicleLength. (in the hope of that
    // the precening lane hasn't a much higher MaxSpeed)
    // This safePos is ugly, but we will live with it in this revision.
    /*    SUMOReal safePos = pow( myMaxSpeed, 2 ) / ( 2 * MSVehicleType::minDecel() ) +
                        MSVehicle::tau() + MSVehicleType::maxLength();
        assert( safePos < myLength ); // Lane has to be longer than safePos,
        // otherwise emission (this kind of emission) makes no sense.
        !!! not in Source lanes
        /


    // Here the emission starts
    if (empty()) {

        return emitTry(veh);
    }

    // Try to emit as last veh. (in driving direction)
    VehCont::iterator leaderIt = myVehicles.begin();
    if (emitTry(veh, leaderIt)) {

        return true;
    }

    // do not try to push vehicles in between on sources
    return false;
    */
}

/////////////////////////////////////////////////////////////////////////////

bool
MSSourceLane::emitTry(MSVehicle& veh)
{
    // on sources, no vehicles may arrive from the back
    bool wasInactive = myVehicles.size()==0;
    myVehicles.push_front(&veh);
    myVehicleLengthSum += veh.getLength();
    if (wasInactive) {
        MSNet::getInstance()->getEdgeControl().gotActive(this);
    }
    MSVehicle::State state(myLength>1 ? (SUMOReal)(myLength - 1.) : 0, 0);
    veh.enterLaneAtEmit(this, state);
    add2MeanDataEmitted();
    return true;
}

/////////////////////////////////////////////////////////////////////////////

bool
MSSourceLane::emitTry(MSVehicle& veh, VehCont::iterator leaderIt)
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
    if (frontMax>0) {
        // emit vehicle if so
        MSVehicle::State state(frontMax, 0);
        veh.enterLaneAtEmit(this, state);
        bool wasInactive = myVehicles.size()==0;
        myVehicles.push_front(&veh);
        myVehicleLengthSum += veh.getLength();
        if (wasInactive) {
            MSNet::getInstance()->getEdgeControl().gotActive(this);
        }
        add2MeanDataEmitted();
        return true;
    }
    return false;
}


/****************************************************************************/

