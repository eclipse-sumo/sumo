/****************************************************************************/
/// @file    MSDetectorTypedefs.h
/// @author  Christian Roessel
/// @date    Thu Oct 16 13:28:50 2003
/// @version $Id$
///
//	»missingDescription«
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
#ifndef MSDetectorTypedefs_h
#define MSDetectorTypedefs_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

// concrete E3 detectors, all LD
#include <microsim/output/e3_detectors/MSE3Collector.h>

#include "MSMeanDetector.h"
#include "MSSumDetector.h"
#include "MSTDDetector.h"
#include "MSLDDetector.h"
#include "MSEDDetector.h"

// concrete E2 detectors
// E2 TD
#include "MSDensity.h"
#include "MSMaxJamLength.h"
#include "MSJamLengthSum.h"
#include "MSQueueLengthAheadOfTrafficLights.h"
#include "MSNVehicles.h"
#include "MSOccupancyDegree.h"
#include "MSSpaceMeanSpeed.h"
#include "MSCurrentHaltingDurationSumPerVehicle.h"
// E2 ED
#include "MSNStartedHalts.h"
#include "MSHaltingDurationSum.h"
// E2 LD
#include "MSHaltDuration.h"



// ===========================================================================
// namespace
// ===========================================================================
namespace Detector
{
// E2_TD
typedef MSMeanDetector< TD::MSDetector< MSDensity > > E2Density;

typedef MSMeanDetector< TD::MSDetector< MSMaxJamLengthInVehicles > >
E2MaxJamLengthInVehicles;

typedef MSMeanDetector< TD::MSDetector< MSMaxJamLengthInMeters > >
E2MaxJamLengthInMeters;

typedef MSMeanDetector< TD::MSDetector< MSJamLengthSumInVehicles > >
E2JamLengthSumInVehicles;

typedef MSMeanDetector< TD::MSDetector< MSJamLengthSumInMeters > >
E2JamLengthSumInMeters;

typedef MSMeanDetector< TD::MSDetector<
MSQueueLengthAheadOfTrafficLightsInVehicles > >
E2QueueLengthAheadOfTrafficLightsInVehicles;

typedef MSMeanDetector< TD::MSDetector<
MSQueueLengthAheadOfTrafficLightsInMeters > >
E2QueueLengthAheadOfTrafficLightsInMeters;

typedef MSMeanDetector< TD::MSDetector< MSNVehicles > > E2NVehicles;

typedef MSMeanDetector< TD::MSDetector<
MSOccupancyDegree > > E2OccupancyDegree;

typedef MSMeanDetector< TD::MSDetector<
MSSpaceMeanSpeed > > E2SpaceMeanSpeed;

typedef MSMeanDetector< TD::MSDetector<
MSCurrentHaltingDurationSumPerVehicle > >
E2CurrentHaltingDurationSumPerVehicle;

// E2 ED
typedef MSSumDetector< ED::MSDetector<
MSNStartedHalts >, true > E2NStartedHalts;

typedef MSSumDetector< ED::MSDetector<
MSHaltingDurationSum >, true > E2HaltingDurationSum;

// E2 LD
typedef MSMeanDetector< LD::MSDetector<
MSHaltDuration>, true > E2HaltingDurationMean;

}


enum DetectorUsage {
    DU_USER_DEFINED,
    DU_SUMO_INTERNAL,
    DU_TL_CONTROL
};


#endif

/****************************************************************************/

