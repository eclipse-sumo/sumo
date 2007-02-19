/****************************************************************************/
/// @file    MSQueueLengthAheadOfTrafficLights.h
/// @author  Christian Roessel
/// @date    Mon Sep 29 09:45:17 2003
/// @version $Id$
///
// * @author Christian Roessel
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
#ifndef MSQueueLengthAheadOfTrafficLights_h
#define MSQueueLengthAheadOfTrafficLights_h
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

#include "MSDetectorHaltingContainerWrapper.h"
#include <microsim/MSUnit.h>
#include "MSTDDetectorInterface.h"
#include <string>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class MSQueueLengthAheadOfTrafficLightsInVehicles
{
    friend class MSE2Collector;
protected:
    typedef SUMOReal DetectorAggregate;
    typedef DetectorContainer::HaltingsList Container;

    MSQueueLengthAheadOfTrafficLightsInVehicles(
        SUMOReal,
        const TD::MSDetectorInterface& helperDetector)
            : helperDetectorM(helperDetector),
            maxNVehM(0)
    {}

    virtual ~MSQueueLengthAheadOfTrafficLightsInVehicles(void)
    {}

    DetectorAggregate getDetectorAggregate(void)
    {
        // helperDet.getDetectorAggregate must be called
        // earlier. E2_Collector is responsible for this.
        DetectorAggregate helperAggr =
            helperDetectorM.getCurrent();
        if (helperAggr > maxNVehM) {
            maxNVehM = helperAggr;
        }
        return maxNVehM;
    }

    void resetMax(void)
    {
        maxNVehM = 0.0;
    }

    static std::string getDetectorName(void)
    {
        return "queueLengthAheadOfTrafficLightsInVehicles";
    }
private:
    const TD::MSDetectorInterface& helperDetectorM;
    SUMOReal maxNVehM;
};


class MSQueueLengthAheadOfTrafficLightsInMeters
{
    friend class MSE2Collector;
protected:
    typedef SUMOReal DetectorAggregate;
    typedef DetectorContainer::HaltingsList Container;

    MSQueueLengthAheadOfTrafficLightsInMeters(
        SUMOReal,
        const TD::MSDetectorInterface& helperDetector)
            : helperDetectorM(helperDetector),
            maxJamLengthM(0)
    {}

    virtual ~MSQueueLengthAheadOfTrafficLightsInMeters(void)
    {}

    DetectorAggregate getDetectorAggregate(void)
    {
        // helperDet.getDetectorAggregate must be called
        // earlier. E2_Collector is responsible for this.
        DetectorAggregate helperAggr =
            helperDetectorM.getCurrent();
        if (helperAggr > maxJamLengthM) {
            maxJamLengthM = helperAggr;
        }
        return maxJamLengthM;
    }

    void resetMax(void)
    {
        maxJamLengthM = 0.0;
    }

    static std::string getDetectorName(void)
    {
        return "queueLengthAheadOfTrafficLightsInMeters";
    }

private:
    const TD::MSDetectorInterface& helperDetectorM;
    SUMOReal maxJamLengthM;
};


#endif

/****************************************************************************/

