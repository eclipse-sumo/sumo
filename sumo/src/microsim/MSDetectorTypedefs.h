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
#include "MSE2Detector.h"
#include "MSE3Detector.h"

// concrete E2 detectors
#include "MSDensity.h"
#include "MSMaxJamLength.h"
#include "MSJamLengthSum.h"
#include "MSQueueLengthAheadOfTrafficLights.h"
#include "MSNVehicles.h"
#include "MSOccupancyDegree.h"
#include "MSSpaceMeanSpeed.h"
#include "MSCurrentHaltingDurationSumPerVehicle.h"

// concrete E3 detectors
#include "MSE3Traveltime.h"
#include "MSE3NVehicles.h"
#include "MSE3MeanNHaltings.h"

namespace Detector 
{
    typedef MSMeanDetector< MSE2Detector< MSDensity > > E2Density;

    typedef MSMeanDetector< MSE2Detector< MSMaxJamLengthInVehicles > >
    E2MaxJamLengthInVehicles;
    
    typedef MSMeanDetector< MSE2Detector< MSMaxJamLengthInMeters > >
    E2MaxJamLengthInMeters;
    
    typedef MSMeanDetector< MSE2Detector< MSJamLengthSumInVehicles > >
    E2JamLengthSumInVehicles;
    
    typedef MSMeanDetector< MSE2Detector< MSJamLengthSumInMeters > >
    E2JamLengthSumInMeters;
    
    typedef MSMeanDetector< MSE2Detector<
        MSQueueLengthAheadOfTrafficLightsInVehicles > >
    E2QueueLengthAheadOfTrafficLightsInVehicles;

    typedef MSMeanDetector< MSE2Detector<
        MSQueueLengthAheadOfTrafficLightsInMeters > >
    E2QueueLengthAheadOfTrafficLightsInMeters;

    typedef MSMeanDetector< MSE2Detector< MSNVehicles > > E2NVehicles;

    typedef MSMeanDetector< MSE2Detector<
        MSOccupancyDegree > > E2OccupancyDegree;

    typedef MSMeanDetector< MSE2Detector<
        MSSpaceMeanSpeed > > E2SpaceMeanSpeed;

    typedef MSMeanDetector< MSE2Detector<
        MSCurrentHaltingDurationSumPerVehicle > >
    E2CurrentHaltingDurationSumPerVehicle;

    // E3
    typedef MSSumDetector< MSE3Detector< MSE3NVehicles >, true > E3NVehicles;

    typedef MSMeanDetector< MSE3Detector< MSE3Traveltime >, true> E3Traveltime;

    typedef MSMeanDetector< MSE3Detector< MSE3MeanNHaltings >, true >
    E3MeanNHaltings;
}

#endif // MSDETECTORTYPEDEFS_H

// Local Variables:
// mode:C++
// End:
