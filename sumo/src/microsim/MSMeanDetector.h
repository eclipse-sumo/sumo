#ifndef MSMEANDETECTOR_H
#define MSMEANDETECTOR_H

/**
 * @file   MSMeanDetector.h
 * @author Christian Roessel
 * @date   Started Thu Sep 18 13:51:35 2003
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

#include <iterator>
#include <string>
#include "MSUnit.h"
#include "MSSumDetector.h"

template < class DetectorType >
class MSMeanDetector : public MSSumDetector< DetectorType >
{
public:
    typedef typename DetectorType::DetectorAggregate DetAggregate;
    typedef typename DetectorType::VehicleCont::iterator VehicleContIter;
    typedef typename DetectorType::AggregatesContIter AggregatesContIter;

    DetAggregate getAggregate( MSUnit::Seconds lastNSeconds ) 
        {
            // returns the mean value of the lastNSeconds
            AggregatesContIter startIt =
                getAggrContStartIterator(
                    MSUnit::getInstance()->getIntegerSteps( lastNSeconds ) );
            MSUnit::Seconds seconds = 
                MSUnit::getInstance()->getSeconds(
                    std::distance( startIt, aggregatesM.end() ) );
            return getSum( lastNSeconds, startIt ) / seconds;
        }

protected:
    // E2 ctor
    MSMeanDetector( std::string id,
                    const double lengthInMeters ) 
        : MSSumDetector< DetectorType >( id, lengthInMeters )
        {}

    // E* ctors follow here    
};


#endif // MSMEANDETECTOR_H

// Local Variables:
// mode:C++
// End:
