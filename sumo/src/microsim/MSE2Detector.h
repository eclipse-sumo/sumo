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

// $Id

#include "MSMoveReminder.h"
#include "MSNet.h"
#include "MSLane.h"
#include "MSUnit.h"
#include "MSVehicle.h"
#include <deque>
#include <list>
#include <string>
#include <utility>
#include <algorithm>
#include <functional>




template < class Detector >
class MSE2Detector : public MSMoveReminder, public Detector
{
public:
    typedef typename Detector::DetAggregate DetAggregate;
    typedef typename Detector::VehicleCont VehicleCont;
    typedef typename VehicleCont::iterator VehicleContIter;

    MSE2Detector( std::string id,
                  const MSLane* lane,
                  const double beginInMeters,
                  const double lengthInMeters )
        : MSMoveReminder( lane, id ),
          Detector( lane, lengthInMeters ),
          startPosM( MSUnit::getInstance()->getCells( beginInMeters ) )
        {
            assert( startPosM >= 0 );
            double length = MSUnit::getInstance()->getCells( lengthInMeters );
            assert( startPosM + length <= laneM->length() );
            endPosM = startPosM + length;
        }

    // call every timestep. Stores detector-data in a container
    void update( void )
        {
            aggregatesM.push_back( getDetAggregate( vehOnDetectorM ) );
        }

    // returns the last aggregated data value
    DetAggregate getCurrent( void ) const
        {
            return aggregatesM.back();
        }

    // returns the mean value of the lastNSeconds
    DetAggregate getMean( MSNet::Time lastNSeconds )
        {
            MSUnit* unit = MSUnit::getInstance();
            AggregatesContIter start =
                getStartIterator( unit->getStep( lastNSeconds ) );
            seconds = unit->getSecond(std::distance(start, aggregatesM.end()));
            return std::accumulate( start, aggregatesM.end(),
                                    static_cast< DetAggregate >( 0 ) ) /
                seconds;
        }

    /**
     * @name Inherited MSMoveReminder methods.
     *
     * Methods in this group are inherited from MSMoveReminder. They are
     * called by the moving, entering and leaving vehicles.
     *
     */    
    //@{
    bool isStillActive( MSVehicle& veh,
                        double oldPos,
                        double newPos )
        {
            if ( newPos <= startPosM ) {
                // detector not yet reached
                return true;
            }
            if ( oldPos <= startPosM && newPos > startPosM ) {
                // vehicle will enter detector
                enterDetectorByMove( veh );
            }
            if ( newPos - veh.length() < startPosM ) {
                // vehicle entered detector partially
                occupancyEntryCorrection( veh, ( newPos - startPosM ) /
                                          veh.length() );
            }
            if ( newPos > endPosM && newPos - veh.length() <= endPosM ) {
                // vehicle left detector partially
                occupancyLeaveCorrection( veh, ( endPosM -
                                                 (newPos - veh.length() ) ) /
                                          veh.length() );
            }               
            if ( newPos - veh.length() > endPosM ) {
                // vehicle will leave detector
                leaveDetectorByMove( veh );
                return false;
            }
            return true;
        }

    void dismissByLaneChange( MSVehicle& veh )
        {
            if (veh.pos() >= startPosM && veh.pos() - veh.length() < endPosM) {
                // vehicle is on detector
                leaveDetectorByLaneChange( veh );
                if ( veh.pos() - veh.length() < startPosM ||
                     veh.pos()>endPosM && veh.pos()-veh.length()<=endPosM ) {
                    // vehicle partially on det
                    dismissOccupancyCorrection( veh );
                }
            }
        }
    
    bool isActivatedByEmitOrLaneChange( MSVehicle& veh )
        {
            if (veh.pos() >= startPosM && veh.pos() - veh.length() < endPosM) {
                // vehicle is on detector
                enterDetectorByEmitOrLaneChange( veh );
                if ( veh.pos() - veh.length() < startPosM ) {
                    // vehicle entered detector partially
                    occupancyEntryCorrection(veh, ( veh.pos() - startPosM ) /
                                             veh.length() );
                }
                if ( veh.pos()>endPosM && veh.pos()-veh.length()<=endPosM ) {
                    // vehicle left detector partially
                    occupancyLeaveCorrection(veh, ( endPosM -
                                                    (veh.pos()-veh.length())) /
                                             veh.length() );
                }               
                return true;
            }
            if ( veh.pos() - veh.length() > endPosM ){
                // vehicle is beyond detector
                return false;
            }
            // vehicle is in front of detector
            return true;
        }
    //@}

private:    
    typedef typename std::deque< DetAggregate > AggregatesCont;
    typedef typename AggregatesCont::iterator AggregatesContIter;

    double startPosM; // in cells
    double endPosM;   // in cells

    AggregatesCont aggregatesM; // stores one value each timestep

    VehicleCont vehOnDetectorM; // vehicles or vehicle-collector-pairs of
                                // the vehicles that are currently on the
                                // detector.

    AggregatesContIter getStartIterator( MSNet::Time lastNTimesteps )
        {
            AggregatesContIter start = aggregatesM.begin();
            if ( aggregatesM.size() > lastNTimesteps ) {
                start = aggregatesM.end() - lastNTimesteps;
            }
            return start;
        }
    
    VehicleContIter getInsertIterator( MSVehicle& veh )
        {
            typedef typename Detector::PosGreater PosGreater;
            return std::find_if( vehOnDetectorM.begin(),
                                 vehOnDetectorM.end(),
                                 std::bind2nd( PosGreater(),
                                               veh.pos() ) );
        }

    VehicleContIter getEraseIterator( MSVehicle& veh )
        {
            typedef typename Detector::VehEquals VehEquals;
            return std::find_if( vehOnDetectorM.begin(),
                                 vehOnDetectorM.end(),
                                 std::bind2nd( VehEquals(),
                                               &veh ) );
        }   
    
    void enterDetectorByMove( MSVehicle& veh )
        {
            vehOnDetectorM.push_front( getNewContainerItem( veh ) );
        }    

    void enterDetectorByEmitOrLaneChange( MSVehicle& veh )
        {
            vehOnDetectorM.insert( getInsertIterator( veh ),
                                   getNewContainerItem( veh ) );
        }
    
    void leaveDetectorByMove( MSVehicle& veh )
        {
            vehOnDetectorM.pop_back();
        }
    
    
    void leaveDetectorByLaneChange( MSVehicle& veh )
        {
            vehOnDetectorM.erase( getEraseIterator( veh ) );
        }
};


#include "MSDensity.h"
// typedefs for concrete detectors.
namespace E2Detector 
{
    typedef MSE2Detector< MSDensity > Density;
};

#endif // MSE2DETECTOR_H

// Local Variables:
// mode:C++
// End:
