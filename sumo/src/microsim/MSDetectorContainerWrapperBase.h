#ifndef MSDETECTORCONTAINERWRAPPERBASE_H
#define MSDETECTORCONTAINERWRAPPERBASE_H

/**
 * @file   MSDetectorContainerWrapperBase.h
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

#include "MSDetectorOccupancyCorrection.h"

class MSVehicle;

struct MSDetectorContainerWrapperBase
{
    virtual void enterDetectorByMove( MSVehicle* veh ) = 0;
    virtual void enterDetectorByEmitOrLaneChange( MSVehicle* veh ) = 0;
    virtual void leaveDetectorByMove( MSVehicle* veh ) = 0;
    virtual void leaveDetectorByLaneChange( MSVehicle* veh ) = 0;
    virtual void removeOnTripEnd( MSVehicle* veh ) = 0;


    MSDetectorContainerWrapperBase()
        : occupancyCorrectionM( 0 )
        {}

    MSDetectorContainerWrapperBase(
        const MSDetectorOccupancyCorrection& occupancyCorrection )
        : occupancyCorrectionM( &occupancyCorrection )
        {}

    virtual ~MSDetectorContainerWrapperBase( void ) {}

    const MSDetectorOccupancyCorrection* occupancyCorrectionM;
};

#endif // MSDETECTORCONTAINERWRAPPERBASE_H

// Local Variables:
// mode:C++
// End:
