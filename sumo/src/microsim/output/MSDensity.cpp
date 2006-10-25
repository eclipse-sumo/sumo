/**
 * @file   MSDensity.cpp
 * @author Christian Roessel
 * @date   Started Thu Sep 11 13:35:55 2003
 * @version
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

// $Log$
// Revision 1.7  2006/10/25 12:22:36  dkrajzew
// updated
//
// Revision 1.6  2005/10/06 13:39:18  dksumo
// using of a configuration file rechecked
//
// Revision 1.5  2005/09/20 06:11:15  dksumo
// floats and doubles replaced by SUMOReal; warnings removed
//
// Revision 1.4  2005/09/09 12:51:21  dksumo
// complete code rework: debug_new and config added
//
// Revision 1.3  2005/06/14 11:21:04  dksumo
// documentation added
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

#include "MSDensity.h"
#include <microsim/MSUnit.h>
#include <microsim/MSLane.h>
#include <cassert>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * member method definitions
 * ======================================================================= */
MSDensity::MSDensity( SUMOReal lengthInMeters,
                      const DetectorContainer::Count& counter ) :
    detectorLengthM( lengthInMeters / (SUMOReal) 1000.0 ),
    counterM( counter )
{}


MSDensity::DetectorAggregate
MSDensity::getDetectorAggregate( void ) // [veh/km]
{
    SUMOReal nVehOnDet = counterM.vehicleCountM -
        counterM.occupancyCorrectionM->getOccupancyEntryCorrection() -
        counterM.occupancyCorrectionM->getOccupancyLeaveCorrection();
    return nVehOnDet / detectorLengthM;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/


// Local Variables:
// mode:C++
// End:
