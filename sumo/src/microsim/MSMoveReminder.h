#ifndef MSMoveReminder_H
#define MSMoveReminder_H

/**
 * @file   MSMoveReminder.h
 * @author Christian Roessel
 * @date   Wed May 21 10:40:22 2003
 *
 * @brief  Base class for all move-reminders
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
// Revision 1.2  2003/05/22 12:44:47  roessel
// Changed void activateByEmit... to bool isActivatedByEmit. Not activated reminders will be erased from the vehicles reminder-list.
//
// Revision 1.1  2003/05/21 16:21:45  dkrajzew
// further work detectors
//

class MSVehicle;

class MSMoveReminder
{
public:
    MSMoveReminder( void ) {}
    virtual ~MSMoveReminder( void ) {}

    virtual bool isStillActive( MSVehicle& veh,
                                double oldPos,
                                double newPos,
                                double newSpeed ) = 0;
    virtual void dismissByLaneChange( MSVehicle& veh ) = 0;
    virtual bool isActivatedByEmitOrLaneChange( MSVehicle& veh ) = 0;

};


#endif

// Local Variables:
// mode:C++
// End:
