/****************************************************************************/
/// @file    GUILaneStateReporter.cpp
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

#include "GUILaneStateReporter.h"
#include <microsim/logging/LoggedValue_TimeFloating.h>
#include <gui/GUIGlobals.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
GUILaneStateReporter::GUILaneStateReporter(
    LoggedValue_TimeFloating<SUMOReal> *densityRetriever,
    LoggedValue_TimeFloating<SUMOReal> *speedRetriever,
    LoggedValue_TimeFloating<SUMOReal> *haltingDurRetriever,
    SUMOReal &floatingDensity, SUMOReal &floatingSpeed, SUMOReal &floatingHaltings,
    const std::string &id, MSLane *lane, SUMOTime interval)
        : MSE2Collector(id, DU_SUMO_INTERNAL, lane, (SUMOReal) 0.1, lane->length()-(SUMOReal) 0.2, //interval,
                        /*haltingTimeThreshold*/ 1, /*haltingSpeedThreshold*/(SUMOReal)(5.0/3.6),
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
{}


bool
GUILaneStateReporter::updateEachTimestep(void)
{
    // density
    SUMOReal val = (SUMOReal) getAggregate(E2::DENSITY, 1);
    if (myDensityRetriever!=0) {
        myDensityRetriever->add(val);
    }
    myFloatingDensity =
        myFloatingDensity * gAggregationRememberingFactor
        + val * (1.0f - gAggregationRememberingFactor);
    // speed
    val = (SUMOReal) getAggregate(E2::SPACE_MEAN_SPEED, 1);
    if (mySpeedRetriever!=0) {
        mySpeedRetriever->add(val);
    }
    myFloatingSpeed =
        myFloatingSpeed * gAggregationRememberingFactor
        + val * (1.0f - gAggregationRememberingFactor);
    // halts
    val = (SUMOReal) getAggregate(E2::HALTING_DURATION_MEAN, 1);
    if (myHaltingDurRetriever!=0) {
        myHaltingDurRetriever->add(val);
    }
    myFloatingHaltings =
        myFloatingHaltings * gAggregationRememberingFactor
        + val * (1.0f - gAggregationRememberingFactor);
    return true;
}



/****************************************************************************/

