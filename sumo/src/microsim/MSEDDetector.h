#ifndef MSEDDETECTOR_H
#define MSEDDETECTOR_H

///
/// @file    MSEDDetector.h
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

#include "MSEDDetectorInterface.h"
#include <functional>

namespace ED
{
    
    template< class ConcreteDetector >
    class MSDetector
        :
        public ED::MSDetectorInterface
        , public ConcreteDetector
    {
    public:
    
        typedef typename ConcreteDetector::DetectorAggregate DetAggregate;
        typedef typename ConcreteDetector::ParameterType ParameterType;
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

        // inherited from ConcreteDetector form MSObserver
        void update( ParameterType aObserved )
            {
                DetAggregate value = getValue( aObserved );
                aggregatesM.push_back(
                    TimeValue(
                        MSNet::getInstance()->simSeconds(), value ) );
            }
    
        virtual double getAggregate( MSUnit::Seconds lastNSeconds ) = 0;
    
    protected:
    
        MSDetector( std::string id
                    , MSUnit::Seconds deleteDataAfterSeconds
                    , DetectorContainer& container )
            :
            ED::MSDetectorInterface( id )
            , ConcreteDetector( container )
            , deleteDataAfterStepsM( MSUnit::getInstance()->getIntegerSteps(
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

} // end namespace ED



// Local Variables:
// mode:C++
// End:

#endif // MSEDDETECTOR_H
