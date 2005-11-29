//---------------------------------------------------------------------------//
//                        GUIDetectorDrawer_FGnT.cpp -
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

// $Log$
// Revision 1.17  2005/11/29 13:25:06  dkrajzew
// mispelled "floating" patched
//
// Revision 1.16  2005/10/07 11:37:17  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.15  2005/09/22 13:39:35  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.14  2005/09/15 11:06:37  dkrajzew
// LARGE CODE RECHECK
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include "GUILaneStateReporter.h"
#include <microsim/logging/LoggedValue_TimeFloating.h>
#include <gui/GUIGlobals.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUILaneStateReporter::GUILaneStateReporter(
        LoggedValue_TimeFloating<SUMOReal> *densityRetriever,
        LoggedValue_TimeFloating<SUMOReal> *speedRetriever,
        LoggedValue_TimeFloating<SUMOReal> *haltingDurRetriever,
        SUMOReal &floatingDensity, SUMOReal &floatingSpeed, SUMOReal &floatingHaltings,
        const std::string &id, MSLane *lane, SUMOTime interval)
    : MSE2Collector(id, DU_SUMO_INTERNAL, lane, (SUMOReal) 0.1, lane->length()-(SUMOReal) 0.2, //interval,
        /*haltingTimeThreshold*/ 1, /*haltingSpeedThreshold*/(SUMOReal) (5.0/3.6),
        /*jamDistThreshold*/ 10, /*deleteDataAfterSeconds*/ interval), // !!!
    myDensityRetriever(densityRetriever), mySpeedRetriever(speedRetriever),
    myHaltingDurRetriever(haltingDurRetriever),
    myFloatingDensity(floatingDensity),
    myFloatingSpeed(floatingSpeed),
    myFloatingHaltings(floatingHaltings)

{
    assert(lane->length()>0.2);
    addDetector(E2::DENSITY);
    addDetector(E2::SPACE_MEAN_SPEED);
//    addDetector(E2::HALTING_DURATION_MEAN);
}


GUILaneStateReporter::~GUILaneStateReporter()
{
}


bool
GUILaneStateReporter::updateEachTimestep( void )
{
    // density
    SUMOReal val = (SUMOReal) getAggregate(E2::DENSITY, 1);
    if(myDensityRetriever!=0) {
        myDensityRetriever->add(val);
    }
    myFloatingDensity =
        myFloatingDensity * gAggregationRememberingFactor
        + val * (1.0f - gAggregationRememberingFactor);
    // speed
    val = (SUMOReal) getAggregate(E2::SPACE_MEAN_SPEED, 1);
    if(mySpeedRetriever!=0) {
        mySpeedRetriever->add(val);
    }
    myFloatingSpeed =
        myFloatingSpeed * gAggregationRememberingFactor
        + val * (1.0f - gAggregationRememberingFactor);
    // halts
    val = (SUMOReal) getAggregate(E2::HALTING_DURATION_MEAN, 1);
    if(myHaltingDurRetriever!=0) {
        myHaltingDurRetriever->add(val);
    }
    myFloatingHaltings =
        myFloatingHaltings * gAggregationRememberingFactor
        + val * (1.0f - gAggregationRememberingFactor);
    return true;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
