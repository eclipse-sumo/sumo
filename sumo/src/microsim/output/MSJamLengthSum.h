/****************************************************************************/
/// @file    MSJamLengthSum.h
/// @author  Christian Roessel
/// @date    Mon Sep 29 09:42:08 2003
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
#ifndef MSJamLengthSum_h
#define MSJamLengthSum_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSDetectorHaltingContainerWrapper.h"
#include <microsim/MSUnit.h>
#include <microsim/MSLane.h>
#include <string>

class MSJamLengthSumInVehicles
{
protected:
    typedef SUMOReal DetectorAggregate;
    typedef DetectorContainer::HaltingsList Container;
    typedef Container::HaltingsConstIt HaltingsConstIt;
    typedef Container::InnerContainer Haltings;

    MSJamLengthSumInVehicles(SUMOReal,
                             const Container& container)
            : containerM(container)
    {}

    virtual ~MSJamLengthSumInVehicles(void)
    {}

    DetectorAggregate getDetectorAggregate(void)
    {
        int pos = 0;
        SUMOReal nVeh = 0.0;
        for (HaltingsConstIt it = containerM.containerM.begin();
                it != containerM.containerM.end(); ++it) {
            if (it->isInJamM) {
                ++nVeh;
                if (pos==0) {
                    SUMOReal corr = containerM.occupancyCorrectionM->getOccupancyEntryCorrection();
                    if (corr!=0) {
                        nVeh -= (SUMOReal)(1.0 - corr);
                    }
                }
                if (pos==(int) containerM.containerM.size()-1) {
                    SUMOReal corr = containerM.occupancyCorrectionM->getOccupancyLeaveCorrection();
                    if (corr!=0) {
                        nVeh -= (SUMOReal)(1.0 - corr);
                    }
                }
            }
            pos++;
        }
        return nVeh;
    }

    static std::string getDetectorName(void)
    {
        return "jamLengthSumInVehicles";
    }
private:
    const Container& containerM;
};


class MSJamLengthSumInMeters
{
protected:
    typedef SUMOReal DetectorAggregate;
    typedef DetectorContainer::HaltingsList Container;
    typedef Container::HaltingsConstIt HaltingsConstIt;
    typedef Container::InnerContainer Haltings;

    MSJamLengthSumInMeters( //const MSDetectorOccupancyCorrection& occupancyCorrection ,
        SUMOReal, const Container& container)
            : containerM(container/*.containerM */)/*,
                                            myOccupancyCorrection(occupancyCorrection)*/
    {}

    virtual ~MSJamLengthSumInMeters(void)
    {}

    DetectorAggregate getDetectorAggregate(void)
    {
        SUMOReal distSum = 0.0;
        for (HaltingsConstIt front = containerM.containerM.begin();
                front != containerM.containerM.end(); ++front) {
            if (front->isInJamM) {
                if (front == containerM.containerM.begin()) {
                    distSum += front->vehM->getLength();
                    SUMOReal corr = containerM.occupancyCorrectionM->getOccupancyEntryCorrection();
                    if (corr!=0) {
                        distSum -= (SUMOReal)((1.0 - corr) * front->vehM->getLength());
                    }
                    assert(distSum >= 0);
                } else {
                    HaltingsConstIt rear = front;
                    --rear;
                    if (rear->isInJamM) {
                        if (front->vehM->getPositionOnLane() < rear->vehM->getPositionOnLane()) {
                            distSum += rear->vehM->getLane().length() - rear->vehM->getPositionOnLane();
                        } else {
                            distSum += front->vehM->getPositionOnLane() - rear->vehM->getPositionOnLane();
                        }
                        if (rear==(--containerM.containerM.end())) {
                            SUMOReal corr = containerM.occupancyCorrectionM->getOccupancyLeaveCorrection();
                            if (corr!=0) {
                                distSum -= (SUMOReal)((1.0 - corr) * rear->vehM->getLength());
                            }
                        }
                    } else {
                        distSum += front->vehM->getLength();
                    }
                }
            }
        }
        assert(distSum >= 0);
        return distSum;
    }

    static std::string getDetectorName(void)
    {
        return "jamLengthSumInMeters";
    }
private:
    const Container& containerM;
//    const MSDetectorOccupancyCorrection& myOccupancyCorrection;
};


#endif

/****************************************************************************/

