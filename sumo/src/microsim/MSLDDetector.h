#ifndef MSLDDETECTOR_H
#define MSLDDETECTOR_H

///
/// @file    MSLDDetector.h
/// @author  Christian Roessel <christian.roessel@dlr.de>
/// @date    Started Tue Dec 02 2003 20:05 CET
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

#include <deque>
#include "MSUnit.h"
#include "MSLDDetectorInterface.h"
#include "MSEventControl.h"
#include "helpers/SimpleCommand.h"
#include <string>
#include <algorithm>

namespace LD
{
    
    template< class ConcreteDetector >
    class MSDetector
        :
        public LD::MSDetectorInterface,
        public ConcreteDetector
    {
    public:
    
        typedef typename ConcreteDetector::DetectorAggregate DetAggregate;
        typedef typename ConcreteDetector::Container DetectorContainer;    
    
        struct TimeValue
        {
            TimeValue( MSUnit::Seconds leaveSecond
                       , DetAggregate value ) 
                : leaveSecM( leaveSecond )
                , valueM( value )
                {}
            MSUnit::Seconds leaveSecM;
            DetAggregate valueM;
        };

        typedef std::deque< TimeValue > AggregatesCont;
        typedef typename AggregatesCont::iterator AggregatesContIter;
    
        void leave( MSVehicle& veh )
            {
                if ( ! hasVehicle( veh ) ) {
                    // vehicle left detector but did not enter it.
                    return;
                }
                aggregatesM.push_back(
                    TimeValue( MSNet::getInstance()->simSeconds(),
                               getValue( veh ) ) );
            }
    
        virtual DetAggregate getAggregate( MSUnit::Seconds lastNSeconds ) = 0;

    protected:

        MSDetector( std::string id
                    , MSUnit::Seconds deleteDataAfterSeconds
                    , const DetectorContainer& container )
            :
            LD::MSDetectorInterface( id )
            , ConcreteDetector( container )
            , aggregatesM()
            , deleteDataAfterStepsM(
                MSUnit::getInstance()->getIntegerSteps(
                    deleteDataAfterSeconds ) )
            {
                startOldDataRemoval();
            }

        MSDetector( std::string id
                    , MSUnit::Seconds deleteDataAfterSeconds
                    , DetectorContainer& container )
            :
            LD::MSDetectorInterface( id )
            , ConcreteDetector( container )
            , aggregatesM()
            , deleteDataAfterStepsM(
                MSUnit::getInstance()->getIntegerSteps(
                    deleteDataAfterSeconds ) )
            {
                startOldDataRemoval();
            }

        virtual ~MSDetector( void )
            {
                aggregatesM.clear();
            }

        struct TimeLesser :
            public std::binary_function< TimeValue, MSUnit::Seconds, bool >
        {
            bool operator()( const TimeValue& aTimeValue,
                             MSUnit::Seconds timeBound ) const
                {
                    return aTimeValue.leaveSecM < timeBound;
                }
        };

        AggregatesContIter getAggrContStartIterator(
            MSUnit::Steps lastNTimesteps )
            {
                return std::lower_bound(
                    aggregatesM.begin(), aggregatesM.end(),
                    getStartTime( lastNTimesteps ),
                    TimeLesser() );
            }

        MSUnit::Seconds getStartTime( MSUnit::Steps lastNTimesteps )
            {
                MSUnit::Steps timestep =
                    MSNet::getInstance()->timestep() - lastNTimesteps;
                if ( timestep < 0 ) {
                    return MSUnit::Seconds(0);
                }
                return MSUnit::getInstance()->getSeconds( timestep );
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
    
        MSUnit::IntSteps freeContainer( void )
            {
                AggregatesContIter end =
                    getAggrContStartIterator( deleteDataAfterStepsM );
                aggregatesM.erase( aggregatesM.begin(), end );
                return deleteDataAfterStepsM;
            }

        AggregatesCont aggregatesM;
    
    private:
    
        MSUnit::IntSteps deleteDataAfterStepsM;
    
    };
 
} // end namespace LD

    
// Local Variables:
// mode:C++
// End:

#endif // MSLDDETECTOR_H
