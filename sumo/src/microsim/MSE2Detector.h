#ifndef MSE2DETECTOR_H
#define MSE2DETECTOR_H

/**
 * @file   MSE2Detector.h
 * @author Christian Roessel
 * @date   Started Tue Sep  9 22:59:27 2003
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

#include "MSUnit.h"
#include "MSPredicates.h"
#include "MSE2DetectorInterface.h"
#include "MSEventControl.h"
// #include "MSDetectorContainer.h"
// #include "MSThresholdDetectorContainer.h"
#include "helpers/SimpleCommand.h"
#include <deque>
#include <string>
#include <algorithm>
#include <functional>
#include "helpers/SingletonDictionary.h"

class MSVehicle;

template < class ConcreteDetector >
class MSE2Detector : public MSE2DetectorInterface,
                     public ConcreteDetector
{
public:
    typedef typename ConcreteDetector::DetectorAggregate DetAggregate;
//     typedef typename ConcreteDetector::ContainerItem ContainerItem;
//     typedef typename ConcreteDetector::VehicleCont VehicleCont;
    typedef typename ConcreteDetector::Container DetectorContainer;
//     typedef typename VehicleCont::iterator VehicleContIter;
    
    
    const std::string& getId( void ) const
        {
            return idM;
        }

    // call every timestep. Stores detector-data in a container
    void update( void )
        {
            aggregatesM.push_back( getDetectorAggregate( vehOnDetectorM ) );
        }

    // returns the last aggregated data value
    DetAggregate getCurrent( void ) const
        {
            return aggregatesM.back();
        }
  
    virtual DetAggregate getAggregate( MSUnit::Seconds lastNSeconds ) = 0;

protected:
    MSE2Detector( std::string id,
                  double lengthInMeters,
                  MSUnit::Seconds deleteDataAfterSeconds,
                  const DetectorContainer& container )
        : ConcreteDetector( lengthInMeters, container ),
          idM( id ),
          deleteDataAfterStepsM( MSUnit::getInstance()->getIntegerSteps(
                                     deleteDataAfterSeconds ) )// ,
//           vehOnDetectorM()
        {
            // start old-data removal through MSEventControl
            Command* deleteData = new SimpleCommand< MSE2Detector >(
                this, &MSE2Detector::freeContainer );
            MSEventControl::getEndOfTimestepEvents()->addEvent(
                deleteData,
                deleteDataAfterStepsM,
                MSEventControl::ADAPT_AFTER_EXECUTION );
        }

    virtual ~MSE2Detector( void )
        {
            aggregatesM.clear();
//             clearVehicleCont( vehOnDetectorM );
        }

    typedef typename std::deque< DetAggregate > AggregatesCont;
    typedef typename AggregatesCont::iterator AggregatesContIter;
    
    AggregatesCont aggregatesM; // stores one value each timestep
    
    AggregatesContIter getAggrContStartIterator( MSUnit::Steps lastNTimesteps )
        {
            AggregatesContIter start = aggregatesM.begin();
            typedef typename AggregatesCont::difference_type Distance;
            Distance steps = static_cast< Distance >( lastNTimesteps );
            if ( aggregatesM.size() > lastNTimesteps ) {
                start = aggregatesM.end() - steps;
            }
            return start;
        }

private:
    const std::string idM;
    MSUnit::IntSteps deleteDataAfterStepsM;

//     VehicleCont vehOnDetectorM; // vehicles or vehicle-collector-pairs of
//     // the vehicles that are currently on the
//     // detector.
    
//     VehicleContIter getVehContInsertIterator( MSVehicle& veh )
//         {
//             return std::find_if( vehOnDetectorM.begin(),
//                                  vehOnDetectorM.end(),
//                                  std::bind2nd(
//                                      Predicate::PosGreater< ContainerItem >(),
//                                      veh.pos() ) );
//         }

//     VehicleContIter getVehContEraseIterator( MSVehicle& veh )
//         {
//             typedef typename Predicate::VehEquals< ContainerItem > Pred;
//             return std::find_if( vehOnDetectorM.begin(),
//                                  vehOnDetectorM.end(),
//                                  std::bind2nd(
//                                      //                                     Predicate::VehEquals< ContainerItem >(),
//                                      Pred(),
//                                      &veh ) );
//         }   
    
//     void enterDetectorByMove( MSVehicle& veh )
//         {
//             vehOnDetectorM.push_front( getNewContainerItem( veh ) );
//         }    

//     void enterDetectorByEmitOrLaneChange( MSVehicle& veh )
//         {
//             vehOnDetectorM.insert( getVehContInsertIterator( veh ),
//                                    getNewContainerItem( veh ) );
//         }
    
//     void leaveDetectorByMove( MSVehicle& veh )
//         {
//             vehOnDetectorM.pop_back();
//         }
    
    
//     void leaveDetectorByLaneChange( MSVehicle& veh )
//         {
//             vehOnDetectorM.erase( getVehContEraseIterator( veh ) );
//         }

    MSUnit::IntSteps freeContainer( void )
        {
            AggregatesContIter end = aggregatesM.end();
            if ( aggregatesM.size() > deleteDataAfterStepsM ) {
                end -= deleteDataAfterStepsM;
                aggregatesM.erase( aggregatesM.begin(), end );
            }
            return deleteDataAfterStepsM;
        }
    
};

#endif // MSE2DETECTOR_H

// Local Variables:
// mode:C++
// End:
