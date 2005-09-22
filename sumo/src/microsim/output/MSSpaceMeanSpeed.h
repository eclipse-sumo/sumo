#ifndef MSSPACEMEANSPEED_H
#define MSSPACEMEANSPEED_H

/**
 * @file   MSSpaceMeanSpeed.h
 * @author Christian Roessel
 * @date   Started Wed Oct 15 16:37:58 2003
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
#include <config.h>
#endif // HAVE_CONFIG_H

#include "MSDetectorContainerWrapper.h"
#include <microsim/MSUnit.h>
#include <numeric>


/* =========================================================================
 * class definitions
 * ======================================================================= */
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

    MSSpaceMeanSpeed( SUMOReal, const Container& vehicleCont )
        : containerM( vehicleCont.containerM )
        {}

    virtual ~MSSpaceMeanSpeed( void )
        {}

    DetectorAggregate getDetectorAggregate( void ); // [m/s]

        static std::string getDetectorName( void )
        {
            return "spaceMeanSpeed";
        }

private:
    const VehicleCont& containerM;
};

namespace
{
    inline SUMOReal speedSumUp( SUMOReal sumSoFar,
                            const MSSpaceMeanSpeed::Vehicle data )
    {
        return sumSoFar + data->speed();
    }
}

inline
MSSpaceMeanSpeed::DetectorAggregate
MSSpaceMeanSpeed::getDetectorAggregate( void )
{
    int size = containerM.size();
    if ( size == 0 ) {
        return 0;
    }
    SUMOReal spaceMeanSpeed = (SUMOReal)
        (std::accumulate( containerM.begin(), containerM.end(),
                         (SUMOReal) 0.0, speedSumUp ) / size);
    return MSUnit::getInstance()->getMetersPerSecond( spaceMeanSpeed );
}

#endif // MSSPACEMEANSPEED_H

// Local Variables:
// mode:C++
// End:
