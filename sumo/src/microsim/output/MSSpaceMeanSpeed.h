/****************************************************************************/
/// @file    MSSpaceMeanSpeed.h
/// @author  Christian Roessel
/// @date    Wed Oct 15 16:37:58 2003
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
#ifndef MSSpaceMeanSpeed_h
#define MSSpaceMeanSpeed_h
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

#include "MSDetectorContainerWrapper.h"
#include <microsim/MSUnit.h>
#include <numeric>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class MSSpaceMeanSpeed
{
public:
    typedef DetectorContainer::VehiclesList::InnerContainer::value_type
    Vehicle;

protected:
    typedef SUMOReal DetectorAggregate;
    typedef DetectorContainer::VehiclesList Container;
    typedef Container::InnerContainer VehicleCont;

    MSSpaceMeanSpeed(SUMOReal, const Container& vehicleCont)
            : containerM(vehicleCont.containerM)
    {}

    virtual ~MSSpaceMeanSpeed(void)
    {}

    DetectorAggregate getDetectorAggregate(void);   // [m/s]

    static std::string getDetectorName(void)
    {
        return "spaceMeanSpeed";
    }

private:
    const VehicleCont& containerM;
};

namespace
{
inline SUMOReal speedSumUp(SUMOReal sumSoFar,
                           const MSSpaceMeanSpeed::Vehicle data)
{
    return sumSoFar + data->getSpeed();
}
}

inline
MSSpaceMeanSpeed::DetectorAggregate
MSSpaceMeanSpeed::getDetectorAggregate(void)
{
    int size = containerM.size();
    if (size == 0) {
        return 0;
    }
    SUMOReal spaceMeanSpeed = (SUMOReal)
                              (std::accumulate(containerM.begin(), containerM.end(),
                                               (SUMOReal) 0.0, speedSumUp) / size);
    return MSUnit::getInstance()->getMetersPerSecond(spaceMeanSpeed);
}


#endif

/****************************************************************************/

