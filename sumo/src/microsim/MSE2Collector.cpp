///
/// @file    MSE2Collector.cpp
/// @author  Christian Roessel <christian.roessel@dlr.de>
/// @date    Started Tue Dec 02 2003 22:13 CET
/// @version $Id$
///
/// @brief
///
///

// Copyright (C) 2003 by German Aerospace Center (http://www.dlr.de) 

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "MSE2Collector.h"

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


std::string MSE2Collector::infoEndM = std::string("</detector>");

namespace E2
{
    DetType& operator++( DetType& det )
    {
        return det = ( ALL == det ) ? DENSITY : DetType( det + 1 );
    }

    Containers& operator++( Containers& cont )
    {
        return cont =
            ( HALTINGS == cont ) ? COUNTER : Containers( cont + 1 );
    }
}
