#ifndef MSTDDETECTOR_H
#define MSTDDETECTOR_H

///
/// @file    MSTDDetector.h
/// @author  Christian Roessel <christian.roessel@dlr.de>
/// @date    Started Tue Dec 02 2003 20:04 CET
/// @version $Id$
///
/// @brief
///
///

/* Copyright (C) 2003 by German Aerospace Center (http://www.dlr.de) */

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//

#include "MSUnit.h"
#include "MSPredicates.h"
#include "MSTDDetectorInterface.h"
#include "MSEventControl.h"
#include "helpers/SimpleCommand.h"
#include <deque>
#include <string>
#include <algorithm>
#include <functional>
#include "helpers/SingletonDictionary.h"

class MSVehicle;

namespace TD // timestep data
{

    template< class ConcreteDetector >
    class MSDetector
        :
        public TD::MSDetectorInterface
        , public ConcreteDetector
    {
    public:

        typedef typename ConcreteDetector::DetectorAggregate DetAggregate;
        typedef typename ConcreteDetector::Container DetectorContainer;

        // returns the last aggregated data value
        DetAggregate getCurrent( void ) const
            {
                if(aggregatesM.size()==0) {
                    return -1;
                }
                return aggregatesM.back();
            }

        virtual DetAggregate getAggregate( MSUnit::Seconds lastNSeconds ) = 0;

    protected:
        MSDetector( std::string id,
                      double lengthInMeters,
                      MSUnit::Seconds deleteDataAfterSeconds,
                      const DetectorContainer& container )
            : TD::MSDetectorInterface( id )
            , ConcreteDetector( lengthInMeters, container )
            , deleteDataAfterStepsM( MSUnit::getInstance()->getIntegerSteps(
                                         deleteDataAfterSeconds ) )
            {
                startOldDataRemoval();
            }

        MSDetector( std::string id,
                      double lengthInMeters,
                      MSUnit::Seconds deleteDataAfterSeconds,
                      const TD::MSDetectorInterface& helperDetector )
            : TD::MSDetectorInterface( id )
            , ConcreteDetector( lengthInMeters, helperDetector )
            , deleteDataAfterStepsM( MSUnit::getInstance()->getIntegerSteps(
                                         deleteDataAfterSeconds ) )
            {
                if ( detNameM == "" ) {
                    detNameM = getDetectorName() + "Sum";
                }
                startOldDataRemoval();
            }

        virtual ~MSDetector( void )
            {
                aggregatesM.clear();
            }

        // called every timestep by MSUpdateEachTimestep inherited
        // from base class.
        bool updateEachTimestep( void )
            {
                aggregatesM.push_back( getDetectorAggregate() );
                return false;
            }

        void startOldDataRemoval( void )
            {
                // start old-data removal through MSEventControl
                Command* deleteData = new SimpleCommand< MSDetector >(
                    this, &MSDetector::freeContainer );
                MSEventControl::getEndOfTimestepEvents()->addEvent(
                    deleteData,
                    deleteDataAfterStepsM,
                    MSEventControl::ADAPT_AFTER_EXECUTION );
            }

        typedef typename std::deque< DetAggregate > AggregatesCont;
        typedef typename AggregatesCont::iterator AggregatesContIter;

        AggregatesCont aggregatesM; // stores one value each timestep

        AggregatesContIter getAggrContStartIterator(
            MSUnit::Steps lastNTimesteps )
            {
                AggregatesContIter start = aggregatesM.begin();
                typedef typename AggregatesCont::difference_type Distance;
                Distance steps = static_cast< Distance >( lastNTimesteps );
                if ( aggregatesM.size() > lastNTimesteps ) {
                    start = aggregatesM.end() - steps;
                }
                return start;
            }

        MSUnit::IntSteps freeContainer( void )
            {
                AggregatesContIter end = aggregatesM.end();
                if ( aggregatesM.size() > deleteDataAfterStepsM ) {
                    end -= deleteDataAfterStepsM;
                    aggregatesM.erase( aggregatesM.begin(), end );
                }
                return deleteDataAfterStepsM;
            }

    private:

        MSUnit::IntSteps deleteDataAfterStepsM;
    };

} // end namespace TD

// Local Variables:
// mode:C++
// End:

#endif // MSTDDETECTOR_H
