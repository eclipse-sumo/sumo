#ifndef MSDETECTORTYPEDEFS_H
#define MSDETECTORTYPEDEFS_H

#include "MSMeanDetector.h"
#include "MSSumDetector.h"
#include "MSE2Detector.h"

// conrete detectors
#include "MSDensity.h"
#include "MSMaxJamLength.h"
#include "MSJamLengthSum.h"
#include "MSQueueLengthAheadOfTrafficLights.h"

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
    
}

#endif // MSE2DETECTORTYPEDEFS_H

// Local Variables:
// mode:C++
// End:
