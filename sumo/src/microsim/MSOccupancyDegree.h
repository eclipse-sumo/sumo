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
    const VehicleCont& containerM;
    const MSUnit::Meters detectorLengthM;
};

namespace 
{    
    inline double occupancySum( double sumSoFar,
                                const MSOccupancyDegree::Vehicle data )
    {
        return sumSoFar + data->length();
    }
}

MSOccupancyDegree::DetectorAggregate 
MSOccupancyDegree::getDetectorAggregate( void )
{
    int size = containerM.size();
    if ( size == 0 ) {
        return 0;
    }
    double occupanceDegree =
        ( std::accumulate( containerM.begin(), containerM.end(),
                           0.0, occupancySum )
          - containerM.front()->length() * getOccupancyEntryCorrection()
          - containerM.back()->length() *  getOccupancyLeaveCorrection() ) /
        detectorLengthM;
    assert ( occupanceDegree >= 0 && occupanceDegree <= 1 );
    return occupanceDegree;
}   



#endif // MSOCCUPANCYDEGREE_H

// Local Variables:
// mode:C++
// End:
