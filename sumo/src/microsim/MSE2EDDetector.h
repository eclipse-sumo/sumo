#ifndef MSE2EDDETECTOR_H
#define MSE2EDDETECTOR_H

///
/// @file    MSE2EDDetector.h
/// @author  Christian Roessel <christian.roessel@dlr.de>
/// @date    Started Fri Nov 28 2003 18:19 CET
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

#include "MSE2EDDetectorInterface.h"


template< class ConcreteDetector >
class MSE2EDDetector
    :
    public MSE2EDDetectorInterface,
    public ConcreteDetector
{
public:
    typedef typename ConcreteDetector::DetectorAggregate DetAggregate;
    typedef typename ConcreteDetector::Observed Observed;
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
    
    const std::string& getId( void ) const
        {
            return idM;
        }

    // inherited from ConcreteDetector form MSObserver
    void update( Observed& aObserved )
        {
            aggregatesM.push_back(
                TimeValue(
                    MSNet::getInstance()->simSeconds(), aObserved ) );
        }
    
    
    virtual double getAggregate( MSUnit::Seconds lastNSeconds ) = 0;
    
protected:
    
    MSE2EDDetector( std::string id
                    , MSUnit::Seconds deleteDataAfterSeconds
                    , DetectorContainer& container )
        :
        ConcreteDetector( container )
        , idM( id )
        , deleteDataAfterStepsM( MSUnit::getInstance()->getIntegerSteps(
                                     deleteDataAfterSeconds ) )
        {
            startOldDataRemoval();
        }

    virtual ~MSE2EDDetector( void )
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

    AggregatesContIter getAggrContStartIterator(MSUnit::Steps lastNTimesteps)
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
            Command* deleteData = new SimpleCommand< MSE2EDDetector >(
                this, &MSE2EDDetector::freeContainer );
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
    const std::string idM;
    MSUnit::IntSteps deleteDataAfterStepsM;

};


// Local Variables:
// mode:C++
// End:

#endif // MSE2EDDETECTOR_H
