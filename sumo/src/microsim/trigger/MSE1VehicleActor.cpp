//---------------------------------------------------------------------------//
//                        MSE1VehicleActor.cpp -
//  An actor which changes a vehicle's state
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : 23.03.2006
//  copyright            : (C) 2006 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.1  2006/03/27 07:19:47  dkrajzew
// vehicle actors added
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


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

#include "MSE1VehicleActor.h"
#include <cassert>
#include <numeric>
#include <utility>
#include <utils/helpers/WrappingCommand.h>
#include <utils/common/ToString.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSLane.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
MSE1VehicleActor::MSE1VehicleActor( const string& id, MSLane* lane,
                                   SUMOReal positionInMeters)
    : MSMoveReminder( lane, id ), MSTrigger(id),
    posM( MSNet::getCells( positionInMeters ) )
{
    assert( posM >= 0 && posM <= laneM->length() );
}


MSE1VehicleActor::~MSE1VehicleActor()
{
}


bool
MSE1VehicleActor::isStillActive( MSVehicle& veh,
                             SUMOReal oldPos,
                             SUMOReal newPos,
                             SUMOReal newSpeed )
{
    if ( newPos < posM ) {
        // detector not reached yet
        return true;
    }
    // !!! do something
    return false;
}


void
MSE1VehicleActor::dismissByLaneChange( MSVehicle& veh )
{
}


bool
MSE1VehicleActor::isActivatedByEmitOrLaneChange( MSVehicle& veh )
{
    if ( veh.pos()-veh.length() > posM ) {
        // vehicle-end is beyond detector. Ignore
        return false;
    }
    // vehicle is in front of detector
    return true;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
