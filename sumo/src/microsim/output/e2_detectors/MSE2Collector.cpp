/****************************************************************************/
/// @file    MSE2Collector.cpp
/// @author  Christian Roessel
/// @date    Tue Dec 02 2003 22:13 CET
/// @version $Id$
///
// / @author  Christian Roessel <christian.roessel@dlr.de>
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

#include "MSE2Collector.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// variable declarations
// ===========================================================================

std::string MSE2Collector::xmlHeaderM(
    "<?xml version=\"1.0\" standalone=\"yes\"?>\n\n"
    "<!--\n"
    "- densityMean [veh/km]\n"
    "- maxJamLengthInVehiclesMean [veh]\n"
    "- maxJamLengthInMetersMean [m]\n"
    "- jamLengthSumInVehiclesMean [veh]\n"
    "- jamLengthSumInMetersMean [m]\n"
    "- queueLengthAheadOfTrafficLightsInVehiclesMean [veh]\n"
    "- queueLengthAheadOfTrafficLightsInMetersMean [m]\n"
    "- nE2VehiclesMean [veh]\n"
    "- occupancyDegreeMean [0,1]\n"
    "- spaceMeanSpeedMean [m/s]\n"
    "- currentHaltingDurationSumPerVehicleMean [s]\n"
    "- nStartedHalts [n]\n"
    //"- haltingDurationSum [s]\n"
    "- haltingDurationMean [s]\n"
    "-->\n\n");


std::string MSE2Collector::infoEndM = "</detector>";

namespace E2
{
DetType& operator++(DetType& det)
{
    return det = (ALL == det) ? DENSITY : DetType(det + 1);
}

Containers& operator++(Containers& cont)
{
    return cont =
               (HALTINGS == cont) ? COUNTER : Containers(cont + 1);
}
}



/****************************************************************************/

