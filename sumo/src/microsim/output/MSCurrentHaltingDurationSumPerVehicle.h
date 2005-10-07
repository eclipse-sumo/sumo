#ifndef MSCURRENTHALTINGDURATIONSUMPERVEHICLE_H
#define MSCURRENTHALTINGDURATIONSUMPERVEHICLE_H

/**
 * @file   MSCurrentHaltingDurationSumPerVehicle.h
 * @author Christian Roessel
 * @date   Started Thu Oct 16 11:44:47 2003
 * @version
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

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */

#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include "MSDetectorHaltingContainerWrapper.h"
#include <numeric>
/* =========================================================================
 * class definitions
 * ======================================================================= */
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

    MSCurrentHaltingDurationSumPerVehicle( SUMOReal,
                                           const Container& vehicleCont )
        : containerM( vehicleCont.containerM )
        {}

    virtual ~MSCurrentHaltingDurationSumPerVehicle( void )
        {}

    DetectorAggregate getDetectorAggregate( void ); // [s]

        static std::string getDetectorName( void )
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
        const MSCurrentHaltingDurationSumPerVehicle::Halting& data )
    {
        return sumSoFar + data.haltingDurationM;
    }
}

inline
MSCurrentHaltingDurationSumPerVehicle::DetectorAggregate
MSCurrentHaltingDurationSumPerVehicle::getDetectorAggregate( void )
{
    int size = containerM.size();
    if ( size == 0 ) {
        return 0;
    }
    SUMOReal haltingDurationSum = (SUMOReal)
        (std::accumulate( containerM.begin(), containerM.end(),
                         (SUMOReal) 0.0, haltingDurationSumUp ) / size);
    return MSUnit::getInstance()->getSeconds( haltingDurationSum );
}
#endif // MSCURRENTHALTINGDURATIONSUMPERVEHICLE_H

// Local Variables:
// mode:C++
// End:
