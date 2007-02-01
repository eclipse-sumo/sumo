/****************************************************************************/
/// @file    MSSumDetector.h
/// @author  Christian Roessel
/// @date    Thu Sep 18 13:50:25 2003
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
#ifndef MSSumDetector_h
#define MSSumDetector_h
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

#include <numeric>
#include <string>
#include <microsim/MSUnit.h>
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

    DetAggregate getAggregate(MSUnit::Seconds lastNSeconds)
    {
        // returns the sum of the lastNSeconds
        AggregatesContIter startIt =
            this->getAggrContStartIterator(
                (MSUnit::Steps) MSUnit::getInstance()->getIntegerSteps(lastNSeconds));
        return getSum(lastNSeconds, startIt,
                      Loki::Int2Type< hasTimeValueCont >());
    }

protected:
    // E2 ZS Ctor
    MSSumDetector(std::string id,
                  SUMOReal lengthInMeters,
                  MSUnit::Seconds deleteDataAfterSeconds,
                  const DetectorContainer& container)
            : DetectorType(id, lengthInMeters,
                           deleteDataAfterSeconds,
                           container)
    {
        if (this->detNameM == "") {
            this->detNameM = this->getDetectorName() + "Sum";
        }
    }

    // Another E2 ZS ctor
    MSSumDetector(std::string id,
                  SUMOReal lengthInMeters,
                  MSUnit::Seconds deleteDataAfterSeconds,
                  const TD::MSDetectorInterface& helperDetector)
            : DetectorType(id, lengthInMeters,
                           deleteDataAfterSeconds,
                           helperDetector)
    {
        if (this->detNameM == "") {
            this->detNameM = this->getDetectorName() + "Sum";
        }
    }

    // E2 ED ctor
    MSSumDetector(std::string id,
                  MSUnit::Seconds deleteDataAfterSeconds,
                  DetectorContainer& container)
            : DetectorType(id,
                           deleteDataAfterSeconds,
                           container)
    {
        if (this->detNameM == "") {
            this->detNameM = this->getDetectorName() + "Sum";
        }
    }

    // E3 ctor
    MSSumDetector(std::string id,
                  MSUnit::Seconds deleteDataAfterSeconds,
                  const DetectorContainer& container)
            : DetectorType(id,
                           deleteDataAfterSeconds,
                           container)
    {
        if (this->detNameM == "") {
            this->detNameM = this->getDetectorName() + "Sum";
        }
    }

    // E* ctors follow here


    virtual ~MSSumDetector(void)
    {}

    DetAggregate getSum(MSUnit::Seconds,
                        AggregatesContIter startIt,
                        Loki::Int2Type< false >)
    {
        // returns the sum of the lastNSeconds
        return std::accumulate(startIt , this->aggregatesM.end(),
                               static_cast< DetAggregate >(0));
    }

    DetAggregate getSum(MSUnit::Seconds,
                        AggregatesContIter startIt,
                        Loki::Int2Type< true >)
    {
        // returns the sum of the lastNSeconds
        DetAggregate retVal = 0;
        for (; startIt != this->aggregatesM.end(); ++startIt) {
            retVal += startIt->valueM;
        }
        return retVal;
    }
};


#endif

/****************************************************************************/

