#ifndef MSQUEUELENGTHAHEADOFTRAFFICLIGHTS_H
#define MSQUEUELENGTHAHEADOFTRAFFICLIGHTS_H

/**
 * @file   MSQueueLengthAheadOfTrafficLights.h
 * @author Christian Roessel
 * @date   Started Mon Sep 29 09:45:17 2003
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

#include "MSDetectorHaltingContainerWrapper.h"
#include "MSUnit.h"
#include "MSTDDetectorInterface.h"
#include <string>

class MSQueueLengthAheadOfTrafficLightsInVehicles
{
    friend class MS_E2_ZS_Collector;
protected:
    typedef double DetectorAggregate;
    typedef DetectorContainer::HaltingsList Container;
    
    MSQueueLengthAheadOfTrafficLightsInVehicles(
        double,
        const TD::MSDetectorInterface& helperDetector )
        : helperDetectorM( helperDetector ),
          maxNVehM( 0 )
        {}

    virtual ~MSQueueLengthAheadOfTrafficLightsInVehicles( void )
        {}

    DetectorAggregate getDetectorAggregate( void )
        {
            // helperDet.getDetectorAggregate must be called
            // earlier. E2_Collector is responsible for this.
            DetectorAggregate helperAggr =
                helperDetectorM.getCurrent();
            if ( helperAggr > maxNVehM ) {
                maxNVehM = helperAggr;
            }
            return maxNVehM;
        }

    void resetMax( void )
        {
            maxNVehM = 0.0;
        }

    static std::string getDetectorName( void )
        {
            return "queueLengthAheadOfTrafficLightsInVehicles";
        }
private:
    const TD::MSDetectorInterface& helperDetectorM;
    double maxNVehM;
};


class MSQueueLengthAheadOfTrafficLightsInMeters
{
    friend class MS_E2_ZS_Collector;
protected:
    typedef double DetectorAggregate;
    typedef DetectorContainer::HaltingsList Container;
    
    MSQueueLengthAheadOfTrafficLightsInMeters(
        double,
        const TD::MSDetectorInterface& helperDetector )
        : helperDetectorM( helperDetector ),
          maxJamLengthM( 0 )
        {}
    
    virtual ~MSQueueLengthAheadOfTrafficLightsInMeters( void )
        {}

    DetectorAggregate getDetectorAggregate( void )
        {
            // helperDet.getDetectorAggregate must be called
            // earlier. E2_Collector is responsible for this.
            DetectorAggregate helperAggr =
                helperDetectorM.getCurrent();
            if ( helperAggr > maxJamLengthM ) {
                maxJamLengthM = helperAggr;
            }
            return MSUnit::getInstance()->getMeters( maxJamLengthM );
        }

    void resetMax( void )
        {
            maxJamLengthM = 0.0;
        }

    static std::string getDetectorName( void )
        {
            return "queueLengthAheadOfTrafficLightsInMeters";
        }
    
private:
    const TD::MSDetectorInterface& helperDetectorM;
    MSUnit::Cells maxJamLengthM;
};


#endif // MSQUEUELENGTHAHEADOFTRAFFICLIGHTS_H

// Local Variables:
// mode:C++
// End:
