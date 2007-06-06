/****************************************************************************/
/// @file    MSNVehicles.cpp
/// @author  Christian Roessel
/// @date    Wed Oct 15 13:33:22 2003
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSNVehicles.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
MSNVehicles::MSNVehicles(const DetectorContainer::Count& counter) :
        counterM(counter)
{}

MSNVehicles::MSNVehicles(SUMOReal,
                         const DetectorContainer::Count& counter) :
        counterM(counter)
{}

MSNVehicles::DetectorAggregate
MSNVehicles::getDetectorAggregate(void)   // [veh]
{
    SUMOReal nVehOnDet = counterM.vehicleCountM;
    return nVehOnDet;
}



/****************************************************************************/

