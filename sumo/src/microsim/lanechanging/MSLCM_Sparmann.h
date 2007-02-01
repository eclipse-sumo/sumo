/****************************************************************************/
/// @file    MSLCM_Sparmann.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id: $
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
#ifndef MSLCM_Sparmann_h
#define MSLCM_Sparmann_h
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

#include <microsim/MSAbstractLaneChangeModel.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class MSLCM_Sparmann : public MSAbstractLaneChangeModel
{
public:
    MSLCM_Sparmann(MSVehicle &v, bool pkw);

    virtual ~MSLCM_Sparmann();

    /** @brief Called to examine whether the vehicle wants to change to right
        This method gets the information about the surrounding vehicles
        and whether another lane may be more preferable */
    virtual int wantsChangeToRight(
        MSAbstractLaneChangeModel::MSLCMessager &msgPass, int blocked,
        const MSVehicle * const leader,
        const MSVehicle * const neighLead,
        const MSVehicle * const neighFollow,
        const MSLane &neighLane,
        int bestLaneOffset, SUMOReal bestDist,
        SUMOReal currentDist);

    /** @brief Called to examine whether the vehicle wants to change to left
        This method gets the information about the surrounding vehicles
        and whether another lane may be more preferable */
    virtual int wantsChangeToLeft(
        MSAbstractLaneChangeModel::MSLCMessager &msgPass, int blocked,
        const MSVehicle * const leader,
        const MSVehicle * const neighLead,
        const MSVehicle * const neighFollow,
        const MSLane &neighLane,
    //        bool congested, bool predInteraction,
        int bestLaneOffset, SUMOReal bestDist,
        SUMOReal currentDist);

    virtual void inform(LaneChangeAction lca, MSVehicle *sender)
    { }


};


#endif

/****************************************************************************/

