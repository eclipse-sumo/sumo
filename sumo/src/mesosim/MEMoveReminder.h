/****************************************************************************/
/// @file    MEMoveReminder.h
/// @author  unknown_author
/// @date    unknown_date
/// @version $Id: MEMoveReminder.h 96 2007-06-06 07:40:46Z behr_mi $
///
// missing_desc
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
#ifndef MEMoveReminder_h
#define MEMoveReminder_h

#ifdef HAVE_MESOSIM


class MEMoveReminder
{
public:
    MEMoveReminder()
    { }
    ~MEMoveReminder()
    { }
    virtual void vehicleEntered(MSVehicle &v) = 0;
    virtual void vehicleLeft(MSVehicle &v) = 0;


};

#endif // HAVE_MESOSIM

#endif

/****************************************************************************/

