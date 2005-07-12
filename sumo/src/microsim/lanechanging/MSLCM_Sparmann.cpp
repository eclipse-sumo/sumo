//---------------------------------------------------------------------------//
//                        MSLCM_Sparmann.cpp -
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
    "";
}
//

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include "MSLCM_Sparmann.h"

/* =========================================================================
 * member method definitions
 * ======================================================================= */

MSLCM_Sparmann::MSLCM_Sparmann(MSVehicle &v, bool pkw)
    : MSAbstractLaneChangeModel(v)
{
}

MSLCM_Sparmann::~MSLCM_Sparmann()
{
}

int
MSLCM_Sparmann::wantsChangeToRight(MSAbstractLaneChangeModel::MSLCMessager &msgPass,
                                   int blocked,
                                 const MSVehicle * const leader,
                                 const MSVehicle * const neighLead,
                                 const MSVehicle * const neighFollow,
                                 const MSLane &neighLane,
                                 int bestLaneOffset, double bestDist,
                                 double currentDist)
{
    // forced changing
    if(currentDist<400&&bestLaneOffset<0) {
        return LCA_RIGHT|LCA_URGENT;
    }
    // no further process to the left
    if(bestLaneOffset>0) {
        return 0;
    }

/*
    double sdxp_V =
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
                                int bestLaneOffset, double bestDist,
                                double currentDist)
{
    // forced changing
    if(currentDist<400&&bestLaneOffset>0) {
        return LCA_LEFT|LCA_URGENT;
    }
    // no further process to the right
    if(bestLaneOffset<0) {
        return 0;
    }

throw 1;
}
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
