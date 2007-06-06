/****************************************************************************/
/// @file    MSHaltDuration.h
/// @author  Christian Roessel
/// @date    Mon Dec 01 2003 12:58 CET
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
#ifndef MSHaltDuration_h
#define MSHaltDuration_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSDetectorHaltingContainerWrapper.h"
#include <string>

// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class MSHaltDuration
            :
            public halt::HaltEndObserver
{
public:

protected:
    typedef SUMOReal DetectorAggregate;
    typedef halt::HaltEndObserver::ParameterType ParameterType;
    typedef DetectorContainer::HaltingsList Container;
    typedef DetectorContainer::HaltingsList HaltingsList;
    typedef HaltingsList::HaltingsIt HaltListIt;
    typedef DetectorContainer::TimeMap HaltDurationMap;
    typedef HaltDurationMap::ContainerIt DurationMapIt;

    MSHaltDuration(HaltingsList& toObserve)
            :
            haltListM(toObserve)
    {
        haltListM.attach(this);
        durationMapM = new HaltDurationMap();
        for (HaltListIt it = haltListM.containerM.begin();
                it != haltListM.containerM.end(); ++it) {
            durationMapM->enterDetectorByMove(it->vehM);
        }
    }

    virtual ~MSHaltDuration(void)
    {
        haltListM.detach(this);
        delete durationMapM;
    }

    bool hasVehicle(MSVehicle& veh) const
    {
        return durationMapM->hasVehicle(&veh);
    }

    void update(ParameterType aObserved)
    {
        MSVehicle* veh = aObserved.vehM;
        MSUnit::Seconds toAdd = MSUnit::getInstance()->getSeconds(
                                    aObserved.haltingDurationM);

        DurationMapIt pair = durationMapM->containerM.find(veh);
        if (pair == durationMapM->containerM.end()) {
            durationMapM->enterDetectorByMove(veh);
        }
        pair = durationMapM->containerM.find(veh);
        pair->second += toAdd;
    }

    DetectorAggregate getValue(MSVehicle& veh)   // [s]
    {
        DurationMapIt pair = durationMapM->containerM.find(&veh);
        assert(pair != durationMapM->containerM.end());

        DetectorAggregate haltDuration = pair->second;

        durationMapM->containerM.erase(&veh);
        return haltDuration;
    }

    static std::string getDetectorName(void)
    {
        return "haltDuration";
    }

private:
    HaltingsList& haltListM;
    HaltDurationMap* durationMapM;

};


#endif

/****************************************************************************/

