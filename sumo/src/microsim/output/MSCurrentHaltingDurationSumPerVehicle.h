/****************************************************************************/
/// @file    MSCurrentHaltingDurationSumPerVehicle.h
/// @author  Christian Roessel
/// @date    Thu Oct 16 11:44:47 2003
/// @version $Id: $
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
#ifndef MSCurrentHaltingDurationSumPerVehicle_h
#define MSCurrentHaltingDurationSumPerVehicle_h
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
#include <numeric>
// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class MSCurrentHaltingDurationSumPerVehicle
{
public:
    typedef DetectorContainer::HaltingsList::InnerContainer::value_type Halting;

protected:
    typedef SUMOReal DetectorAggregate;
    typedef DetectorContainer::HaltingsList Container;
    typedef Container::InnerContainer HaltingCont;

    MSCurrentHaltingDurationSumPerVehicle(SUMOReal,
                                          const Container& vehicleCont)
            : containerM(vehicleCont.containerM)
    {}

    virtual ~MSCurrentHaltingDurationSumPerVehicle(void)
    {}

    DetectorAggregate getDetectorAggregate(void);   // [s]

    static std::string getDetectorName(void)
    {
        return "currentHaltingDurationSumPerVehicle";
    }

private:
    const HaltingCont& containerM;
};

namespace
{
inline SUMOReal haltingDurationSumUp(
    SUMOReal sumSoFar,
    const MSCurrentHaltingDurationSumPerVehicle::Halting& data)
{
    return sumSoFar + data.haltingDurationM;
}
}

inline
MSCurrentHaltingDurationSumPerVehicle::DetectorAggregate
MSCurrentHaltingDurationSumPerVehicle::getDetectorAggregate(void)
{
    int size = containerM.size();
    if (size == 0) {
        return 0;
    }
    SUMOReal haltingDurationSum = (SUMOReal)
                                  (std::accumulate(containerM.begin(), containerM.end(),
                                                   (SUMOReal) 0.0, haltingDurationSumUp) / size);
    return MSUnit::getInstance()->getSeconds(haltingDurationSum);
}


#endif

/****************************************************************************/

