#ifndef MSDETECTORCONTAINERBASE_H
#define MSDETECTORCONTAINERBASE_H

/**
 * @file   MSDetectorContainerBase.h
 * @author Christian Roessel
 * @date   Started Fri Sep 26 19:11:26 2003
 * @version $Id$
 * @brief
 *
 *
 */

/* Copyright (C) 2003 by German Aerospace Center (http://www.dlr.de) */

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//

class MSVehicle;

struct MSDetectorContainerBase
{
    virtual void enterDetectorByMove( MSVehicle* veh ) = 0;
    virtual void enterDetectorByEmitOrLaneChange( MSVehicle* veh ) = 0;
    virtual void leaveDetectorByMove( MSVehicle* veh ) = 0;
    virtual void leaveDetectorByLaneChange( MSVehicle* veh ) = 0;
    virtual void update( void ) = 0;
    virtual ~MSDetectorContainerBase( void ) { }
};

#endif // MSDETECTORCONTAINERBASE_H

// Local Variables:
// mode:C++
// End:
