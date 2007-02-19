/****************************************************************************/
/// @file    MSMaxJamLength.h
/// @author  Christian Roessel
/// @date    Mon Sep 29 09:39:10 2003
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
#ifndef MSMaxJamLength_h
#define MSMaxJamLength_h
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
#include <string>
// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class MSMaxJamLengthInVehicles
{
protected:
    typedef SUMOReal DetectorAggregate;
    typedef DetectorContainer::HaltingsList Container;
    typedef Container::HaltingsConstIt HaltingsConstIt;
    typedef Container::InnerContainer Haltings;

    MSMaxJamLengthInVehicles(SUMOReal,
                             const Container& container)
            : containerM(container)
    {}

    virtual ~MSMaxJamLengthInVehicles(void)
    {}

    DetectorAggregate getDetectorAggregate(void)
    {
        SUMOReal maxNVeh = 0.0;
        SUMOReal vehCount = 0.0;
        int pos = 0;
        for (HaltingsConstIt it = containerM.containerM.begin();
                it != containerM.containerM.end(); ++it) {
            if (it->isInJamM) {
                ++vehCount;
                if (pos==0) {
                    SUMOReal corr = containerM.occupancyCorrectionM->getOccupancyEntryCorrection();
                    if (corr!=0) {
                        vehCount -= (SUMOReal)(1.0 - corr);
                    }
                }
                if (pos==(int) containerM.containerM.size()-1) {
                    SUMOReal corr = containerM.occupancyCorrectionM->getOccupancyLeaveCorrection();
                    if (corr!=0) {
                        vehCount -= (SUMOReal)(1.0 - corr);
                    }
                }
                if (vehCount > maxNVeh) {
                    maxNVeh = vehCount;
                }
            } else {
                vehCount = 0;
            }
            pos++;
        }
        return maxNVeh;
    }

    static std::string getDetectorName(void)
    {
        return "maxJamLengthInVehicles";
    }
private:
    const Container& containerM;
};


class MSMaxJamLengthInMeters
{
protected:
    typedef SUMOReal DetectorAggregate;
    typedef DetectorContainer::HaltingsList Container;
    typedef Container::HaltingsConstIt HaltingsConstIt;
    typedef Container::InnerContainer Haltings;

    MSMaxJamLengthInMeters(SUMOReal,
                           const Container& container)
            : containerM(container)
    {}

    virtual ~MSMaxJamLengthInMeters(void)
    {}

    DetectorAggregate getDetectorAggregate(void)
    {
        SUMOReal maxDist = 0.0;
        const MSVehicle* startVeh = 0;
        const MSVehicle* stopVeh = 0;
        for (HaltingsConstIt it = containerM.containerM.begin();
                it != containerM.containerM.end(); ++it) {
            if (it->isInJamM) {
                if (startVeh == 0) {
                    startVeh = it->vehM;
                }
                stopVeh = it->vehM;
                SUMOReal dist = stopVeh->getPositionOnLane() - startVeh->getPositionOnLane() +
                                startVeh->getLength();
                if (startVeh==containerM.containerM.begin()->vehM) {
                    SUMOReal corr = containerM.occupancyCorrectionM->getOccupancyEntryCorrection();
                    if (corr!=0) {
                        dist -= (SUMOReal)((1.0 - corr) * startVeh->getLength());
                    }
                }
                if (stopVeh==(--containerM.containerM.end())->vehM) {
                    SUMOReal corr = containerM.occupancyCorrectionM->getOccupancyLeaveCorrection();
                    if (corr!=0) {
                        dist -= (SUMOReal)((1.0 - corr) * stopVeh->getLength());
                    }
                }
                if (dist > maxDist) {
                    maxDist = dist;
                }
            } else {
                startVeh = 0;
            }
        }
        return maxDist;
    }

    static std::string getDetectorName(void)
    {
        return "maxJamLengthInMeters";
    }

private:
    const Container& containerM;

};


#endif

/****************************************************************************/

