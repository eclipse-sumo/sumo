#ifndef MSLCM_Sparmann_h
#define MSLCM_Sparmann_h
//---------------------------------------------------------------------------//
//                        MSLCM_Sparmann.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Fri, 29.04.2005
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

#include <microsim/MSAbstractLaneChangeModel.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class MSLCM_Sparmann : public MSAbstractLaneChangeModel {
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

    virtual void inform(LaneChangeAction lca, MSVehicle *sender) { }


};
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
