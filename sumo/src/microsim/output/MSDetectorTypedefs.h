#ifndef MSDETECTORTYPEDEFS_H
#define MSDETECTORTYPEDEFS_H

/**
 * @file   MSDetectorTypedefs.h
 * @author Christian Roessel
 * @date   Started Thu Oct 16 13:28:50 2003
 * @version $Id$
 * @brief
 *
 *
 */

/* Copyright (C) 2003 by German Aerospace Center (http://www.dlr.de) */

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//

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


// concrete E3 detectors, all LD
#include <microsim/output/e3_detectors/MSE3Traveltime.h>
#include <microsim/output/e3_detectors/MSE3NVehicles.h>
#include <microsim/output/e3_detectors/MSE3MeanNHaltings.h>

// concrete E1 detectors, all LD
#include <microsim/output/e1_detectors/MSE1MeanSpeed.h>

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


    // E3
    typedef MSSumDetector< LD::MSDetector< MSE3NVehicles >, true > E3NVehicles;

    typedef MSMeanDetector< LD::MSDetector<
        MSE3Traveltime >, true> E3Traveltime;

    typedef MSMeanDetector< LD::MSDetector< MSE3MeanNHaltings >, true >
    E3MeanNHaltings;

    // E1 are all LD
    typedef MSMeanDetector< LD::MSDetector<
        MSE1MeanSpeed >, true> E1MeanSpeed;
}


enum DetectorUsage {
    DU_USER_DEFINED,
    DU_SUMO_INTERNAL,
    DU_TL_CONTROL
};

#endif // MSDETECTORTYPEDEFS_H

// Local Variables:
// mode:C++
// End:
