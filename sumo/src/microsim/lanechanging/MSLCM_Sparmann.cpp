/****************************************************************************/
/// @file    MSLCM_Sparmann.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 29.05.2005
/// @version $Id$
///
//  »missingDescription«
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

#include "MSLCM_Sparmann.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// member method definitions
// ===========================================================================

MSLCM_Sparmann::MSLCM_Sparmann(MSVehicle &v, bool pkw)
        : MSAbstractLaneChangeModel(v)
{}

MSLCM_Sparmann::~MSLCM_Sparmann()
{}

int
MSLCM_Sparmann::wantsChangeToRight(MSAbstractLaneChangeModel::MSLCMessager &msgPass,
                                   int blocked,
                                   const MSVehicle * const leader,
                                   const MSVehicle * const neighLead,
                                   const MSVehicle * const neighFollow,
                                   const MSLane &neighLane,
                                   int bestLaneOffset, SUMOReal bestDist,
                                   SUMOReal currentDist)
{
    // forced changing
    if (currentDist<400&&bestLaneOffset<0) {
        return LCA_RIGHT|LCA_URGENT;
    }
    // no further process to the left
    if (bestLaneOffset>0) {
        return 0;
    }

    /*
        SUMOReal sdxp_V =
      */
    throw 1;
}


int
MSLCM_Sparmann::wantsChangeToLeft(MSAbstractLaneChangeModel::MSLCMessager &msgPass,
                                  int blocked,
                                  const MSVehicle * const leader,
                                  const MSVehicle * const neighLead,
                                  const MSVehicle * const neighFollow,
                                  const MSLane &neighLane,
                                  int bestLaneOffset, SUMOReal bestDist,
                                  SUMOReal currentDist)
{
    // forced changing
    if (currentDist<400&&bestLaneOffset>0) {
        return LCA_LEFT|LCA_URGENT;
    }
    // no further process to the right
    if (bestLaneOffset<0) {
        return 0;
    }

    throw 1;
}



/****************************************************************************/

