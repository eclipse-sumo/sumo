/****************************************************************************/
/// @file    MSLCM_DK2002.cpp
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

#include "MSLCM_DK2002.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// member method definitions
// ===========================================================================
MSLCM_DK2002::MSLCM_DK2002(MSVehicle &v)
        : MSAbstractLaneChangeModel(v)
{}

MSLCM_DK2002::~MSLCM_DK2002()
{}

int
MSLCM_DK2002::wantsChangeToRight(MSAbstractLaneChangeModel::MSLCMessager &/*msgPass*/,
                                 int blocked,
                                 const MSVehicle * const leader,
                                 const MSVehicle * const neighLead,
                                 const MSVehicle * const /*neighFollow*/,
                                 const MSLane &neighLane,
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

    // security checks for krauss
    //  (vsafe fails when gap<0)
    if ((blocked&(LCA_BLOCKEDBY_FOLLOWER|LCA_BLOCKEDBY_LEADER))!=0) {
        return 0;
    }

    // higher speed
    SUMOReal neighLaneVSafe, thisLaneVSafe;
    if (neighLead == 0) {
        neighLaneVSafe =
            myVehicle.ffeV(
                myVehicle.getSpeed(), neighLane.length() - myVehicle.getPositionOnLane(), 0);
        /*
            myVehicle.speed(), myVehicle.decelAbility(),
            neighLane.length() - myVehicle.pos(), 0);
            */
    } else {
        neighLaneVSafe =
            myVehicle.ffeV(
                myVehicle.getSpeed(), neighLead->getPositionOnLane() - neighLead->getLength() - myVehicle.getPositionOnLane(),
                neighLead->getSpeed());
        /*
            myVehicle.vsafe(myVehicle.speed(), myVehicle.decelAbility(),
                neighLead->pos() - neighLead->length() - myVehicle.pos(),
                neighLead->speed());
                */
    }
    if (leader==0) {
        thisLaneVSafe =
            myVehicle.ffeV(myVehicle.getSpeed(), myVehicle.getLane().length() - myVehicle.getPositionOnLane(), 0);
        /*
            myVehicle.vsafe(
                myVehicle.speed(), myVehicle.decelAbility(),
                myVehicle.getLane().length() - myVehicle.pos(), 0);
                */
    } else {
        thisLaneVSafe =
            myVehicle.ffeV(myVehicle.getSpeed(), leader->getPositionOnLane() - leader->getLength() - myVehicle.getPositionOnLane(),
                           leader->getSpeed());
        /*
            myVehicle.vsafe(
                myVehicle.speed(), myVehicle.decelAbility(),
                leader->pos() - leader->length() - myVehicle.pos(),
                leader->speed());
                */
    }
    if (thisLaneVSafe+0.1<neighLaneVSafe) {
        return LCA_RIGHT|LCA_SPEEDGAIN;
    }
    return 0;
}


int
MSLCM_DK2002::wantsChangeToLeft(MSAbstractLaneChangeModel::MSLCMessager &/*msgPass*/,
                                int blocked,
                                const MSVehicle * const leader,
                                const MSVehicle * const neighLead,
                                const MSVehicle * const /*neighFollow*/,
                                const MSLane &neighLane,
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

    // security checks for krauss
    //  (vsafe fails when gap<0)
    if ((blocked&(LCA_BLOCKEDBY_FOLLOWER|LCA_BLOCKEDBY_LEADER))!=0) {
        return 0;
    }

    // higher speed
    SUMOReal neighLaneVSafe, thisLaneVSafe;
    if (neighLead == 0) {
        neighLaneVSafe =
            myVehicle.ffeV(myVehicle.getSpeed(), neighLane.length() - myVehicle.getPositionOnLane(), 0);
        /*
            myVehicle.vsafe(
                myVehicle.speed(), myVehicle.decelAbility(),
                neighLane.length() - myVehicle.pos(), 0);
                */
    } else {
        neighLaneVSafe =
            myVehicle.ffeV(myVehicle.getSpeed(),
                           neighLead->getSpeed() - neighLead->getLength() - myVehicle.getPositionOnLane(),
                           neighLead->getPositionOnLane());
        /*
            myVehicle.vsafe(myVehicle.speed(), myVehicle.decelAbility(),
                neighLead->pos() - neighLead->length() - myVehicle.pos(),
                neighLead->speed());
                */
    }
    if (leader==0) {
        thisLaneVSafe =
            myVehicle.ffeV(
                myVehicle.getSpeed(),
                myVehicle.getLane().length() - myVehicle.getPositionOnLane(), 0);
        /*
            myVehicle.vsafe(
                myVehicle.speed(), myVehicle.decelAbility(),
                myVehicle.getLane().length() - myVehicle.pos(), 0);
                */
    } else {
        thisLaneVSafe =
            myVehicle.ffeV(
                myVehicle.getSpeed(),
                leader->getPositionOnLane() - leader->getLength() - myVehicle.getPositionOnLane(),
                leader->getSpeed());
        /*
            myVehicle.vsafe(
                myVehicle.speed(), myVehicle.decelAbility(),
                leader->pos() - leader->length() - myVehicle.pos(),
                leader->speed());
                */
    }
    if (thisLaneVSafe+0.1<neighLaneVSafe) {
        return LCA_LEFT|LCA_SPEEDGAIN;
    }
    return 0;
}



/****************************************************************************/

