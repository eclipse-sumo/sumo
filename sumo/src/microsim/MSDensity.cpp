/**
 * @file   MSDensity.cpp
 * @author Christian Roessel
 * @date   Started Thu Sep 11 13:35:55 2003
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

// $Id$

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "MSDensity.h"
#include "MSUnit.h"
#include "MSLane.h"
#include <cassert>

using namespace std;

MSDensity::MSDensity( double lengthInMeters,
                      const DetectorContainer::Count& counter ) :
    detectorLengthM( lengthInMeters / 1000.0 ),
    counterM( counter )
{}


MSDensity::DetectorAggregate
MSDensity::getDetectorAggregate( void ) // [veh/km]
{
    double nVehOnDet = counterM.vehicleCountM -
        counterM.occupancyCorrectionM.getOccupancyEntryCorrection() -
        counterM.occupancyCorrectionM.getOccupancyLeaveCorrection();
    return nVehOnDet / detectorLengthM;
}


// Local Variables:
// mode:C++
// End:
