//---------------------------------------------------------------------------//
//                        MSLCM_Krauss.cpp -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 29.05.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.6  2005/09/22 13:45:51  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.5  2005/09/15 11:07:36  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/07/15 07:18:39  dkrajzew
// code style applied
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "MSLCM_Krauss.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


MSLCM_Krauss::MSLCM_Krauss(MSVehicle &v)
    : MSAbstractLaneChangeModel(v)
{
}

MSLCM_Krauss::~MSLCM_Krauss()
{
}

int
MSLCM_Krauss::wantsChangeToRight(MSAbstractLaneChangeModel::MSLCMessager &msgPass,
                                 int blocked,
                                 const MSVehicle * const leader,
                                 const MSVehicle * const neighLead,
                                 const MSVehicle * const neighFollow,
                                 const MSLane &neighLane,
                                 int bestLaneOffset, SUMOReal bestDist,
                                 SUMOReal currentDist)
{
    // forced changing
    if(currentDist<400&&bestLaneOffset<0) {
        return LCA_RIGHT|LCA_URGENT;
    }
    // no further process to the left
    if(bestLaneOffset>0) {
        return 0;
    }

    // krauss: do not change lanes if congested or if following another vehicle
    if(congested( neighLead )||predInteraction(leader)) {
        return false;
    }

    // krauss: change for higher speed
    SUMOReal thisLaneVSafe;
    if(leader==0) {
        thisLaneVSafe =
            myVehicle.vsafe(
                myVehicle.speed(), myVehicle.decelAbility(),
                myVehicle.getLane().length() - myVehicle.pos(), 0);
    } else {
        thisLaneVSafe =
            myVehicle.vsafe(
                myVehicle.speed(), myVehicle.decelAbility(),
                leader->pos() - leader->length() - myVehicle.pos(),
                leader->speed());
    }
    if(thisLaneVSafe
        >=
        MIN2(myVehicle.getVehicleType().maxSpeed(), myVehicle.getLane().maxSpeed())) {

        // lane change due to a higher speed on the other lane
        return LCA_RIGHT|LCA_SPEEDGAIN;
    }
    // no lane change
    return 0;
}


int
MSLCM_Krauss::wantsChangeToLeft(MSAbstractLaneChangeModel::MSLCMessager &msgPass,
                                int blocked,
                                const MSVehicle * const leader,
                                const MSVehicle * const neighLead,
                                const MSVehicle * const neighFollow,
                                const MSLane &neighLane,
                                int bestLaneOffset, SUMOReal bestDist,
                                SUMOReal currentDist)
{
    // forced changing
    if(currentDist<400&&bestLaneOffset>0) {
        return LCA_LEFT|LCA_URGENT;
    }
    // no further process to the right
    if(bestLaneOffset<0) {
        return 0;
    }

    // krauss: do not change lanes if congested
    if(congested( neighLead )) {
        return false;
    }
    // krauss: change for higher speed
    SUMOReal thisLaneVSafe;
    if(leader==0) {
        thisLaneVSafe =
            myVehicle.vsafe(
                myVehicle.speed(), myVehicle.decelAbility(),
                myVehicle.getLane().length() - myVehicle.pos(), 0);
    } else {
        thisLaneVSafe =
            myVehicle.vsafe(
                myVehicle.speed(), myVehicle.decelAbility(),
                leader->pos() - leader->length() - myVehicle.pos(),
                leader->speed());
    }
    if(thisLaneVSafe
        <
        MIN2(myVehicle.getVehicleType().maxSpeed(), myVehicle.getLane().maxSpeed())) {

        // lane change due to a higher speed on the other lane
        return LCA_LEFT|LCA_SPEEDGAIN;
    }
    // no lane change
    return 0;
}
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
