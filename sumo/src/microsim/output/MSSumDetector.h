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
#include <microsim/MSUnit.h>
#ifdef WIN32
#include "helpers/msvc6_TypeManip.h"
#endif
#ifndef WIN32
#include "helpers/gcc_TypeManip.h"
#endif

#include "MSTDDetectorInterface.h"

template < class DetectorType
           , bool hasTimeValueCont = false
           >
class MSSumDetector : public DetectorType
{
    enum ContainerType { JUST_VALUE = 0, TIME_VALUE_STRUCT };

    friend class MSE2Collector; // only MSE2/3Collector have
    friend class MSE3Collector; // access to ctor

public:
    typedef typename DetectorType::DetectorAggregate DetAggregate;
//     typedef typename DetectorType::VehicleCont::iterator VehicleContIter;
    typedef typename DetectorType::AggregatesContIter AggregatesContIter;
    typedef typename DetectorType::Container DetectorContainer;

    DetAggregate getAggregate( MSUnit::Seconds lastNSeconds )
        {
            // returns the sum of the lastNSeconds
            AggregatesContIter startIt =
                getAggrContStartIterator(
                    MSUnit::getInstance()->getIntegerSteps( lastNSeconds ) );
            return getSum( lastNSeconds, startIt,
                           Loki::Int2Type< hasTimeValueCont >() );
        }

protected:
    // E2 ZS Ctor
    MSSumDetector( std::string id,
                   double lengthInMeters,
                   MSUnit::Seconds deleteDataAfterSeconds,
                   const DetectorContainer& container )
        : DetectorType( id, lengthInMeters,
                        deleteDataAfterSeconds,
                        container )
        {
            if ( detNameM == "" ) {
                detNameM = getDetectorName() + "Sum";
            }
        }

    // Another E2 ZS ctor
    MSSumDetector( std::string id,
                   double lengthInMeters,
                   MSUnit::Seconds deleteDataAfterSeconds,
                   const TD::MSDetectorInterface& helperDetector )
        : DetectorType( id, lengthInMeters,
                        deleteDataAfterSeconds,
                        helperDetector )
        {
            if ( detNameM == "" ) {
                detNameM = getDetectorName() + "Sum";
            }
        }

    // E2 ED ctor
    MSSumDetector( std::string id,
                   MSUnit::Seconds deleteDataAfterSeconds,
                   DetectorContainer& container )
        : DetectorType( id,
                        deleteDataAfterSeconds,
                        container )
        {
            if ( detNameM == "" ) {
                detNameM = getDetectorName() + "Sum";
            }
        }

    // E3 ctor
    MSSumDetector( std::string id,
                   MSUnit::Seconds deleteDataAfterSeconds,
                   const DetectorContainer& container )
        : DetectorType( id,
                        deleteDataAfterSeconds,
                        container )
        {
            if ( detNameM == "" ) {
                detNameM = getDetectorName() + "Sum";
            }
        }

    // E* ctors follow here


    virtual ~MSSumDetector( void )
        {}

    DetAggregate getSum( MSUnit::Seconds,
                         AggregatesContIter startIt,
                         Loki::Int2Type< false > )
        {
            // returns the sum of the lastNSeconds
            return std::accumulate( startIt , aggregatesM.end(),
                                    static_cast< DetAggregate >( 0 ) );
        }

    DetAggregate getSum( MSUnit::Seconds,
                         AggregatesContIter startIt,
                         Loki::Int2Type< true > )
        {
            // returns the sum of the lastNSeconds
            DetAggregate retVal = 0;
            for (; startIt != aggregatesM.end(); ++startIt ) {
                retVal += startIt->valueM;
            }
            return retVal;
        }
};


#endif // MSSUMDETECTOR_H

// Local Variables:
// mode:C++
// End:
