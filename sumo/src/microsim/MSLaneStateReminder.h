#ifndef MSLaneStateReminder_H
#define MSLaneStateReminder_H

/**
 * @file   MSLaneStateReminder.h
 * @author Christian Roessel
 * @date   Wed May 21 10:50:44 2003
 *
 * @brief
 *
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
// Revision 1.1  2003/05/21 16:21:45  dkrajzew
// further work detectors
//

#include "MSMoveReminder.h"
#include "MSLaneState.h"
#include "MSNet.h"

class MSLaneStateReminder : public MSMoveReminder
{
public:
    MSLaneStateReminder( double startPos,
                         double endPos,
                         MSLaneState& ls ) :
        startPosM( startPos ),
        endPosM( endPos ),
        laneStateM( ls )
        {}

    ~MSLaneStateReminder( void )
        {}

    bool isStillActive( MSVehicle& veh,
                        double oldPos,
                        double newPos,
                        double newSpeed )
        {
            double timestepFraction = MSNet::deltaT();
            if ( newPos <= startPosM ) {
                return true;
            }
            if ( oldPos <= startPosM && newPos > startPosM ) {
                // vehicle will enter detector
                timestepFraction = ( newPos-startPosM ) / newSpeed;
                laneStateM.enterDetectorByMove( veh, timestepFraction );
            }
            if ( newPos > endPosM ) {
                timestepFraction = ( endPosM-oldPos ) / newSpeed;
                double timestepFractionReduce = 0;
                if ( oldPos <= startPosM ) {
                    // vehicle enterd and left dtector in one step
                    timestepFractionReduce = ( startPosM-oldPos ) / newSpeed;
                    assert( timestepFraction - timestepFractionReduce >= 0 );
                }
                laneStateM.addMoveData(
                    veh, timestepFraction - timestepFractionReduce );
                laneStateM.leaveDetectorByMove( veh, timestepFraction );
                return false;
            }
            laneStateM.addMoveData( veh, timestepFraction );
            return true;
        }

    void dismissByLaneChange( MSVehicle& veh )
        {
            if ( veh.pos() >= startPosM && veh.pos() < endPosM ) {
                laneStateM.leaveDetectorByLaneChange( veh );
            }
        }

    void activateByEmitOrLaneChange( MSVehicle& veh )
        {
            if ( veh.pos() >= startPosM && veh.pos() < endPosM ) {
                laneStateM.enterDetectorByEmitOrLaneChange( veh );
            }
        }

private:
    double startPosM;
    double endPosM;
    MSLaneState& laneStateM;

    MSLaneStateReminder( void )
        {}

};



#endif

// Local Variables:
// mode:C++
// End:
