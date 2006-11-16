#ifndef MSLCM_DK2002_h
#define MSLCM_DK2002_h
//---------------------------------------------------------------------------//
//                        MSLCM_DK2002.h -
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
// $Log$
// Revision 1.8  2006/11/16 10:50:45  dkrajzew
// warnings removed
//
// Revision 1.7  2006/11/14 13:02:32  dkrajzew
// warnings removed
//
// Revision 1.6  2006/10/12 10:14:28  dkrajzew
// synchronized with internal CVS (mainly the documentation has changed)
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



class MSLCM_DK2002 : public MSAbstractLaneChangeModel {
public:
    MSLCM_DK2002(MSVehicle &v);

    virtual ~MSLCM_DK2002();

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

    virtual void inform(LaneChangeAction , MSVehicle * /*sender*/) { }


};
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
