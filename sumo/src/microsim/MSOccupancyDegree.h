#ifndef MSOCCUPANCYDEGREE_H
#define MSOCCUPANCYDEGREE_H

/**
 * @file   MSOccupancyDegree.h
 * @author Christian Roessel
 * @date   Started Wed Oct 15 17:35:12 2003
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

#include "MSDetectorContainer.h"
#include "MSOccupancyCorrection.h"
#include "MSUnit.h"
#include <numeric>
#include <cassert>

class MSOccupancyDegree : virtual public MSOccupancyCorrection< double >
{
public:
    typedef DetectorContainer::Vehicles::InnerCont::value_type Vehicle;

protected:
    typedef double DetectorAggregate;
    typedef DetectorContainer::Vehicles Container;
    typedef Container::InnerCont VehicleCont;
    
    MSOccupancyDegree( double lengthInMeters, const Container& vehicleCont ) 
        : detectorLengthM( lengthInMeters ),
          containerM( vehicleCont.containerM )
        {}

    virtual ~MSOccupancyDegree( void )
        {}
    
    DetectorAggregate getDetectorAggregate( void ); // [0,1]
    
    static std::string getDetectorName( void )
        {
            return "occupancyDegree";
        }

private:
    const MSUnit::Meters detectorLengthM;
    const VehicleCont& containerM;
};

namespace 
{    
    inline double occupancySumUp( double sumSoFar,
                                  const MSOccupancyDegree::Vehicle data )
    {
        return sumSoFar + data->length();
    }
}

inline
MSOccupancyDegree::DetectorAggregate 
MSOccupancyDegree::getDetectorAggregate( void )
{
    int size = containerM.size();
    if ( size == 0 ) {
        return 0;
    }
    double entryCorr(0);
    double leaveCorr(0);
    if ( getOccupancyEntryCorrection() > 0 ){
        entryCorr = ( 1 - getOccupancyEntryCorrection() );
    }
    if ( getOccupancyLeaveCorrection() > 0 ){
        leaveCorr = ( 1 - getOccupancyEntryCorrection() );
    }
    double occupancyDegree =
        ( std::accumulate( containerM.begin(), containerM.end(),
                           0.0, occupancySumUp )
          - containerM.front()->length() * entryCorr
          - containerM.back()->length() *  leaveCorr ) /
        detectorLengthM;
    // Note: in reality a occupancyDegree value should be in
    // [0,1]. Due to size-less intersections and some inability of
    // vehicles to see a leading vehicle that partially left a lane,
    // it is possible that the value is > 1. The vehicle that
    // partially left the lane overlaps with a vehicle that changed to
    // the lane the leaving vehicle has been before. If the lane is
    // already jammed, the occupancyDegree may be > 1. We will return
    // 1 then until this look-forward at lane change works correct.
//     assert ( occupancyDegree >= 0 && occupancyDegree <= 1 );
    assert ( occupancyDegree >= 0 );
    if ( occupancyDegree > 1 ) {
        occupancyDegree = 1;
    }
    resetOccupancyCorrection();
    return occupancyDegree;
}   



#endif // MSOCCUPANCYDEGREE_H

// Local Variables:
// mode:C++
// End:
