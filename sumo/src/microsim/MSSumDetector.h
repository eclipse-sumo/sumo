#ifndef MSSUMDETECTOR_H
#define MSSUMDETECTOR_H

/**
 * @file   MSSumDetector.h
 * @author Christian Roessel
 * @date   Started Thu Sep 18 13:50:25 2003
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

// $Id$


#include <numeric>
#include <string>
#include "MSUnit.h"

template < class DetectorType >
class MSSumDetector : public DetectorType
{
public:
    typedef typename DetectorType::DetectorAggregate DetAggregate;
    typedef typename DetectorType::VehicleCont::iterator VehicleContIter;
    typedef typename DetectorType::AggregatesContIter AggregatesContIter;
    
    DetAggregate getAggregate( MSUnit::Seconds lastNSeconds ) 
        {
            // returns the sum of the lastNSeconds
            AggregatesContIter startIt =
                getAggrContStartIterator(
                    MSUnit::getInstance()->getIntegerSteps( lastNSeconds ) );
            return getSum( lastNSeconds, startIt );
        }
    
protected:
    // E2 Ctor
    MSSumDetector( std::string id,
                   const double lengthInMeters )
        : DetectorType( id, lengthInMeters )
        {}

    // E* ctors follow here
    
    DetAggregate getSum( MSUnit::Seconds lastNSeconds,
                         AggregatesContIter startIt ) 
        {
            // returns the sum of the lastNSeconds
            return std::accumulate( startIt , aggregatesM.end(),
                                    static_cast< DetAggregate >( 0 ) );
        }    
};


#endif // MSSUMDETECTOR_H

// Local Variables:
// mode:C++
// End:
