/****************************************************************************/
/// @file    MSLCM_Krauss.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 29.05.2005
/// @version $Id$
///
//
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

#include "MSLCM_Krauss.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


MSLCM_Krauss::MSLCM_Krauss(MSVehicle &v)
        : MSAbstractLaneChangeModel(v)
{}

MSLCM_Krauss::~MSLCM_Krauss()
{}

int
MSLCM_Krauss::wantsChangeToRight(MSAbstractLaneChangeModel::MSLCMessager &/*msgPass*/,
                                 int /*blocked*/,
                                 const MSVehicle * const leader,
                                 const MSVehicle * const neighLead,
                                 const MSVehicle * const /*neighFollow*/,
                                 const MSLane &/*neighLane*/,
                                 int bestLaneOffset, SUMOReal /*bestDist*/,
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

    // krauss: do not change lanes if congested or if following another vehicle
    if (congested(neighLead)||predInteraction(leader)) {
        return false;
    }

    // krauss: change for higher speed
    SUMOReal thisLaneVSafe;
    if (leader==0) {
        thisLaneVSafe =
            myVehicle.ffeV(myVehicle.getSpeed(), myVehicle.getLane().length() - myVehicle.getPositionOnLane(), 0);
    } else {
        thisLaneVSafe =
            myVehicle.ffeV(myVehicle.getSpeed(), leader->getPositionOnLane() - leader->getLength() - myVehicle.getPositionOnLane(),
                           leader->getSpeed());
    }
    if (thisLaneVSafe
            >=
            MIN2(myVehicle.getVehicleType().getMaxSpeed(), myVehicle.getLane().maxSpeed())) {

        // lane change due to a higher speed on the other lane
        return LCA_RIGHT|LCA_SPEEDGAIN;
    }
    // no lane change
    return 0;
}


int
MSLCM_Krauss::wantsChangeToLeft(MSAbstractLaneChangeModel::MSLCMessager &/*msgPass*/,
                                int /*blocked*/,
                                const MSVehicle * const leader,
                                const MSVehicle * const neighLead,
                                const MSVehicle * const /*neighFollow*/,
                                const MSLane &/*neighLane*/,
                                int bestLaneOffset, SUMOReal /*bestDist*/,
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

    // krauss: do not change lanes if congested
    if (congested(neighLead)) {
        return false;
    }
    // krauss: change for higher speed
    SUMOReal thisLaneVSafe;
    if (leader==0) {
        thisLaneVSafe =
            myVehicle.ffeV(myVehicle.getSpeed(), myVehicle.getLane().length() - myVehicle.getPositionOnLane(), 0);
    } else {
        thisLaneVSafe =
            myVehicle.ffeV(myVehicle.getSpeed(), leader->getPositionOnLane() - leader->getLength() - myVehicle.getPositionOnLane(),
                           leader->getSpeed());
    }
    if (thisLaneVSafe
            <
            MIN2(myVehicle.getVehicleType().getMaxSpeed(), myVehicle.getLane().maxSpeed())) {

        // lane change due to a higher speed on the other lane
        return LCA_LEFT|LCA_SPEEDGAIN;
    }
    // no lane change
    return 0;
}



/****************************************************************************/

