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
// Revision 1.6  2003/06/04 16:20:04  roessel
// Moved modified code from .h to .cpp.
//
// Revision 1.5  2003/05/28 11:19:51  roessel
// Changed ctor argument from reference to pointer.
// Added private default-ctor, copy-ctor, assignement-operator declarations.
//
// Revision 1.4  2003/05/27 17:27:15  dkrajzew
// unworking default constructor overriding removed
//
// Revision 1.3  2003/05/22 13:45:34  roessel
// Added condition in isActivatedByEmitOrLaneChange.
//
// Revision 1.2  2003/05/22 12:44:10  roessel
// Changed void activateByEmit... to bool isActivatedByEmit. Not activated
// reminders will be erased from the vehicles reminder-list.
//
// Revision 1.1  2003/05/21 16:21:45  dkrajzew
// further work detectors
//

#include "MSMoveReminder.h"
class MSLaneState;
class MSLane;


class MSLaneStateReminder : public MSMoveReminder
{
public:
    MSLaneStateReminder( double startPos,
                         double endPos,
                         MSLaneState* ls,
                         MSLane* l ) :
        MSMoveReminder( l ),
        startPosM( startPos ),
        endPosM( endPos ),
        laneStateM( ls )
        {}

    ~MSLaneStateReminder( void )
        {}

    bool isStillActive( MSVehicle& veh,
                        double oldPos,
                        double newPos,
                        double newSpeed );

    void dismissByLaneChange( MSVehicle& veh );
    
    bool isActivatedByEmitOrLaneChange( MSVehicle& veh );
    
private:
    double startPosM;
    double endPosM;
    MSLaneState* laneStateM;

    /// Default constructor.
    MSLaneStateReminder();

    /// Copy constructor.
    MSLaneStateReminder( const MSLaneStateReminder& );

    /// Assignment operator.
    MSLaneStateReminder& operator=( const MSLaneStateReminder& );
};



#endif

// Local Variables:
// mode:C++
// End:
