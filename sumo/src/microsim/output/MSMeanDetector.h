/****************************************************************************/
/// @file    MSMeanDetector.h
/// @author  Christian Roessel
/// @date    Thu Sep 18 13:51:35 2003
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
#ifndef MSMeanDetector_h
#define MSMeanDetector_h
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


// ===========================================================================
// class definitions
// ===========================================================================
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

    DetAggregate getAggregate(MSUnit::Seconds lastNSeconds)
    {
        // returns the mean value of the lastNSeconds
        AggregatesContIter startIt =
            this->getAggrContStartIterator(
                (MSUnit::Steps) MSUnit::getInstance()->getIntegerSteps(lastNSeconds));
        if (startIt==this->aggregatesM.end()) {
            return 0;
        }
        MSUnit::Seconds seconds =
            MSUnit::getInstance()->getSeconds(
                (MSUnit::Steps) std::distance(startIt, this->aggregatesM.end()));
        if (seconds==0) {
            return 0;
        }
        return getSum(lastNSeconds, startIt,
                      Loki::Int2Type< hasTimeValueCont >()) / seconds;
    }

    // E2 ctor
    MSMeanDetector(std::string id,
                   SUMOReal lengthInMeters,
                   MSUnit::Seconds deleteDataAfterSeconds,
                   const DetectorContainer& container)
            : MSSumDetector< DetectorType, hasTimeValueCont >(
                id, lengthInMeters,
                deleteDataAfterSeconds,
                container)
    {
        this->detNameM = this->getDetectorName() + "Mean";
    }

    // Another E2 ctor
    MSMeanDetector(std::string id,
                   SUMOReal lengthInMeters,
                   MSUnit::Seconds deleteDataAfterSeconds,
                   const TD::MSDetectorInterface& helperDetector)
            : MSSumDetector< DetectorType, hasTimeValueCont >(
                id, lengthInMeters,
                deleteDataAfterSeconds,
                helperDetector)
    {
        this->detNameM = this->getDetectorName() + "Mean";
    }

    // E3 ctors
    MSMeanDetector(std::string id,
                   MSUnit::Seconds deleteDataAfterSeconds,
                   const DetectorContainer& container)
            : MSSumDetector< DetectorType, hasTimeValueCont >(
                id,
                deleteDataAfterSeconds,
                container)
    {
        this->detNameM = this->getDetectorName() + "Mean";
    }

    MSMeanDetector(std::string id,
                   MSUnit::Seconds deleteDataAfterSeconds,
                   DetectorContainer& container)
            : MSSumDetector< DetectorType, hasTimeValueCont >(
                id,
                deleteDataAfterSeconds,
                container)
    {
        this->detNameM = this->getDetectorName() + "Mean";
    }

    // E* ctors follow here

    ~MSMeanDetector(void)
    {}

private:
    MSMeanDetector(const MSMeanDetector&);
    MSMeanDetector& operator=(const MSMeanDetector&);
};


#endif

/****************************************************************************/

