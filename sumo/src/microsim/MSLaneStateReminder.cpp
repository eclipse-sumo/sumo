/**
 * @file   MSLaneStateReminder.cpp
 * @author Christian Roessel
 * @date   Started Wed May 21 10:42:24 2003
 *
 * $Revision$ from $Date$ by $Author$.
 *
 * @brief Definitions of class MSLaneStateReminder.
 *
 */


//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//

// $Log$
// Revision 1.4  2003/06/23 15:53:27  roessel
// Refinement of if-condition in dismissByLaneChange and
// isActivatedByEmitOrLaneChange.
//
// Revision 1.3  2003/06/10 13:24:08  roessel
// Added documentation.
//
// Revision 1.2  2003/06/04 16:20:03  roessel
// Moved modified code from .h to .cpp.
//
// Revision 1.1  2003/05/21 16:21:45  dkrajzew
// further work detectors
//

#include "MSLaneStateReminder.h"
#include "MSVehicle.h"
#include "MSLaneState.h"
#include "MSLane.h"
#include "MSNet.h"

bool
MSLaneStateReminder::isStillActive( MSVehicle& veh,
                                    double oldPos,
                                    double newPos,
                                    double newSpeed )
{          
    // if vehicle has passed the detector completely we shouldn't
    // be here.
    // fraction of timestep the vehicle is on the detector after entry.
    double timestepFraction = MSNet::deltaT();
    if ( newPos <= startPosM ) {
        return true;
    }
    if ( oldPos <= startPosM && newPos > startPosM ) {
        // vehicle will enter detector
        timestepFraction = ( newPos-startPosM ) / newSpeed;
        assert( timestepFraction <= MSNet::deltaT() &&
                timestepFraction >= 0 );
        laneStateM->enterDetectorByMove( veh, 1.0 - timestepFraction );
    }
    if ( newPos - veh.length() > endPosM ) {
        // vehicle will leave detector
        // fraction of timestep the vehicle is not on the detector
        // after leave.
        double fractionReduce = ( newPos - veh.length() - endPosM ) /
            newSpeed;
        assert( fractionReduce <= MSNet::deltaT() &&
                fractionReduce >= 0 &&
                timestepFraction - fractionReduce > 0 );
        laneStateM->addMoveData(
            veh, newSpeed, timestepFraction - fractionReduce );
        laneStateM->leaveDetectorByMove( veh, MSNet::deltaT() -
                                         fractionReduce );
        return false;
    }  
    laneStateM->addMoveData( veh, newSpeed, timestepFraction );
    return true;
}

void
MSLaneStateReminder::dismissByLaneChange( MSVehicle& veh )
{
    if ( veh.pos() >= startPosM && veh.pos() - veh.length() < endPosM ) {
        // vehicle is on detector
        laneStateM->leaveDetectorByLaneChange( veh );
    }
}

bool
MSLaneStateReminder::isActivatedByEmitOrLaneChange( MSVehicle& veh )
{
    if ( veh.pos() >= startPosM && veh.pos() - veh.length() < endPosM ) {
        // vehicle is on detector
        laneStateM->enterDetectorByEmitOrLaneChange( veh );
        return true;
    }
    if ( veh.pos() - veh.length() > endPosM ){
        // vehicle is beyond detector
        return false;
    }
    // vehicle is in front of detector
    return true;
}

// Local Variables:
// mode:C++
// End:
