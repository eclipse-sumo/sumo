#ifndef MSSPACEMEANSPEED_H
#define MSSPACEMEANSPEED_H

/**
 * @file   MSSpaceMeanSpeed.h
 * @author Christian Roessel
 * @date   Started Wed Oct 15 16:37:58 2003
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

#include "MSDetectorContainerWrapper.h"
#include "MSUnit.h"
#include <numeric>

class MSSpaceMeanSpeed
{
public:
    typedef DetectorContainer::VehiclesList::InnerContainer::value_type
    Vehicle;
    
protected:
    typedef double DetectorAggregate;
    typedef DetectorContainer::VehiclesList Container;
    typedef Container::InnerContainer VehicleCont;
    
    MSSpaceMeanSpeed( double lengthInMeters, const Container& vehicleCont ) 
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
    inline double speedSumUp( double sumSoFar,
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
    double spaceMeanSpeed =
        std::accumulate( containerM.begin(), containerM.end(),
                         0.0, speedSumUp ) / size;
    return MSUnit::getInstance()->getMetersPerSecond( spaceMeanSpeed );
}   

#endif // MSSPACEMEANSPEED_H

// Local Variables:
// mode:C++
// End:
