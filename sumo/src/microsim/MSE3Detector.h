#ifndef MSE3DETECTOR_H
#define MSE3DETECTOR_H

///
/// @file    MSE3Detector.h
/// @author  Christian Roessel <christian.roessel@dlr.de>
/// @date    Started Thu Nov 27 2003 15:33 CET
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
#include "MSE3DetectorInterface.h"
#include "MSEventControl.h"
#include "helpers/SimpleCommand.h"
#include <string>
#include <algorithm>

template< class ConcreteDetector >
class MSE3Detector : public MSE3DetectorInterface,
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
    
    const std::string& getId( void ) const
        {
            return idM;
        }

    void leave( MSVehicle& veh )
        {
            if ( ! hasVehicle( veh ) ) {
                // vehicle left detector but did not enter it.
                return;
            }
            aggregatesM.push_back(
                TimeValue(
                    MSNet::getInstance()->simSecond(), getAggregate( veh ) ) );
        }
    
    
    virtual double getAggregate( MSUnit::Seconds lastNSeconds ) = 0;

protected:
    MSE3Detector( std::string id
                  , MSUnit::Seconds deleteDataAfterSeconds
                  , const DetectorContainer& container )
        :
        MSE3DetectorInterface( id )
        , ConcreteDetector( container )
        , aggregatesM()
        , idM( id )
        , deleteDataAfterStepsM(
            MSUnit::getInstance()->getIntegerSteps( deleteDataAfterSeconds ) )
        {
            startOldDataRemoval();
        }

    MSE3Detector( std::string id
                  , MSUnit::Seconds deleteDataAfterSeconds
                  , const MSE2DetectorInterface& helperDetector )
        :
        MSE3DetectorInterface( id )
        , ConcreteDetector( helperDetector )
        , aggregatesM()
        , idM( id )
        , deleteDataAfterStepsM(
            MSUnit::getInstance()->getIntegerSteps( deleteDataAfterSeconds ) )
        {
            startOldDataRemoval();
        }


    virtual ~MSE3Detector( void )
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

    AggregatesContIter getAggrContStartIter( MSUnit::Steps lastNTimesteps )
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
            Command* deleteData = new SimpleCommand< MSE3Detector >(
                this, &MSE3Detector::freeContainer );
            MSEventControl::getEndOfTimestepEvents()->addEvent(
                deleteData,
                deleteDataAfterStepsM,
                MSEventControl::ADAPT_AFTER_EXECUTION );
        }
    
    MSUnit::IntSteps freeContainer( void )
        {
            AggregatesContIter end =
                getAggrContStartIter( deleteDataAfterStepsM );
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

#endif // MSE3DETECTOR_H
