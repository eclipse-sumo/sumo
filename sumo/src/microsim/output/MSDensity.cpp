/****************************************************************************/
/// @file    MSDensity.cpp
/// @author  Christian Roessel
/// @date    Thu Sep 11 13:35:55 2003
/// @version $Id$
///
// * @author Christian Roessel
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
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif

// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSDensity.h"
#include <microsim/MSUnit.h>
#include <microsim/MSLane.h>
#include <cassert>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;

// ===========================================================================
// member method definitions
// ===========================================================================
MSDensity::MSDensity(SUMOReal lengthInMeters,
                     const DetectorContainer::Count& counter) :
        detectorLengthM(lengthInMeters / (SUMOReal) 1000.0),
        counterM(counter)
{}


MSDensity::DetectorAggregate
MSDensity::getDetectorAggregate(void)   // [veh/km]
{
    SUMOReal nVehOnDet = counterM.vehicleCountM -
                         counterM.occupancyCorrectionM->getOccupancyEntryCorrection() -
                         counterM.occupancyCorrectionM->getOccupancyLeaveCorrection();
    return nVehOnDet / detectorLengthM;
}



/****************************************************************************/

