#ifndef MSMEANDETECTOR_H
#define MSMEANDETECTOR_H

/**
 * @file   MSMeanDetector.h
 * @author Christian Roessel
 * @date   Started Thu Sep 18 13:51:35 2003
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

//

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

#include <iterator>
#include <string>
#include <microsim/MSUnit.h>
#include "MSSumDetector.h"
#ifdef WIN32
#include <utils/helpers/msvc6_TypeManip.h>
#endif
#ifndef WIN32
#include <utils/helpers/gcc_TypeManip.h>
#endif


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
template < class DetectorType
           , bool hasTimeValueCont = false >
class MSMeanDetector : public MSSumDetector< DetectorType, hasTimeValueCont >
{
    friend class MSE2Collector; // only MSE2/3Collector has
    friend class MSE3Collector; // access to ctor

public:
    typedef typename DetectorType::DetectorAggregate DetAggregate;
//     typedef typename DetectorType::VehicleCont::iterator VehicleContIter;
    typedef typename DetectorType::AggregatesContIter AggregatesContIter;
    typedef typename DetectorType::Container DetectorContainer;

    DetAggregate getAggregate( MSUnit::Seconds lastNSeconds )
        {
            // returns the mean value of the lastNSeconds
            AggregatesContIter startIt =
                getAggrContStartIterator(
					(MSUnit::Steps) MSUnit::getInstance()->getIntegerSteps( lastNSeconds ) );
            if(startIt==aggregatesM.end()) {
                return 0;
            }
            MSUnit::Seconds seconds =
                MSUnit::getInstance()->getSeconds(
					(MSUnit::Steps) std::distance( startIt, aggregatesM.end() ) );
            if(seconds==0) {
                return 0;
            }
            return getSum( lastNSeconds, startIt,
                           Loki::Int2Type< hasTimeValueCont >()) / seconds;
        }

    // E2 ctor
    MSMeanDetector( std::string id,
                    SUMOReal lengthInMeters,
                    MSUnit::Seconds deleteDataAfterSeconds,
                    const DetectorContainer& container )
        : MSSumDetector< DetectorType, hasTimeValueCont >(
            id, lengthInMeters,
            deleteDataAfterSeconds,
            container )
        {
            detNameM = getDetectorName() + "Mean";
        }

    // Another E2 ctor
    MSMeanDetector( std::string id,
                    SUMOReal lengthInMeters,
                    MSUnit::Seconds deleteDataAfterSeconds,
                    const TD::MSDetectorInterface& helperDetector )
        : MSSumDetector< DetectorType, hasTimeValueCont >(
            id, lengthInMeters,
            deleteDataAfterSeconds,
            helperDetector )
        {
            detNameM = getDetectorName() + "Mean";
        }

    // E3 ctors
    MSMeanDetector( std::string id,
                    MSUnit::Seconds deleteDataAfterSeconds,
                    const DetectorContainer& container )
        : MSSumDetector< DetectorType, hasTimeValueCont >(
            id,
            deleteDataAfterSeconds,
            container )
        {
            detNameM = getDetectorName() + "Mean";
        }

    MSMeanDetector( std::string id,
                    MSUnit::Seconds deleteDataAfterSeconds,
                    DetectorContainer& container )
        : MSSumDetector< DetectorType, hasTimeValueCont >(
            id,
            deleteDataAfterSeconds,
            container )
        {
            detNameM = getDetectorName() + "Mean";
        }

    // E* ctors follow here

    ~MSMeanDetector( void )
        {}

private:
    MSMeanDetector( const MSMeanDetector& );
    MSMeanDetector& operator=( const MSMeanDetector& );
};


#endif // MSMEANDETECTOR_H

// Local Variables:
// mode:C++
// End:
