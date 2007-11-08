/****************************************************************************/
/// @file    MSApproachingVehiclesStates.h
/// @author  Christian Roessel
/// @date    Thu Oct 16 14:45:00 2003
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
#ifndef MSApproachingVehiclesStates_h
#define MSApproachingVehiclesStates_h

// ===========================================================================
// included modules
// ===========================================================================

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSDetectorContainerWrapper.h"
#include <microsim/MSUnit.h>
#include <string>
#include <vector>

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSApproachingVehiclesStates
 */
class MSApproachingVehiclesStates
{
    friend class MSE2Collector;
public:

    class VehicleState
    {
    public:
        VehicleState(SUMOReal gap2DetectorEnd,
                     MSUnit::CellsPerStep speed)
                : gap2DetectorEndM(gap2DetectorEnd), speedM(MSUnit::getInstance()->getMetersPerSecond(speed)) {}

        SUMOReal getGap2DetectorEnd(void) const {
            return gap2DetectorEndM;
        }

        MSUnit::MetersPerSecond getSpeed(void) const {
            return speedM;
        }

    private:
        SUMOReal gap2DetectorEndM;
        MSUnit::MetersPerSecond speedM;

    };

    // the first element of the container corresponds to first vehicle
    // in driving direction
    typedef std::vector< VehicleState > DetectorAggregate;

protected:
    typedef DetectorContainer::VehiclesList Container;
    typedef Container::InnerContainer VehicleCont;

    MSApproachingVehiclesStates(SUMOReal detectorEndPos,
                                const Container& vehicleCont)
            : detectorEndPosM(detectorEndPos),
            containerM(vehicleCont.containerM),
            statesM() {}

    virtual ~MSApproachingVehiclesStates(void) {
        statesM.clear();
    }

    const DetectorAggregate& getDetectorAggregate(unsigned nApproachingVeh) {
        statesM.clear();

        VehicleCont::const_iterator vehIt = containerM.begin();
        if ((*vehIt)->getPositionOnLane() > detectorEndPosM) {
            // first vehicle left detector partially, start with the next
            // one.
            ++vehIt;
        }
        unsigned nValidVehicles = std::distance(vehIt, containerM.end());
        if (nValidVehicles < nApproachingVeh) {
            nApproachingVeh = nValidVehicles;
        }

        for (unsigned index = 0; index < nApproachingVeh;
                ++index, ++vehIt) {
            statesM.push_back(
                VehicleState(
                    detectorEndPosM - (*vehIt)->getPositionOnLane(),
                    (*vehIt)->getSpeed()));
        }
        return statesM;
    }

    static std::string getDetectorName(void) {
        return "approachingVehiclesStates";
    }

private:
    SUMOReal detectorEndPosM;
    const VehicleCont& containerM;
    DetectorAggregate statesM;
};


#endif

/****************************************************************************/

