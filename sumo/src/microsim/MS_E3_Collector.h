#ifndef MS_E3_COLLECTOR_H
#define MS_E3_COLLECTOR_H

/**
 * @file   MS_E3_Collector.h
 * @author Christian Roessel
 * @date   Started Tue Nov 25 12:39:10 2003
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

#include "MSMoveReminder.h"
#include "MSDetectorFileOutput.h"
#include <string>
#include <cassert>
#include <vector>
#include <limits>
#include "helpers/SingletonDictionary.h"
#include "MSDetectorContainerWrapperBase.h"
#include "MSDetectorContainerWrapper.h"
#include "MSDetectorCounterContainerWrapper.h"
#include "MSDetectorHaltingContainerWrapper.h"
#include "MSDetectorOccupancyCorrection.h"
#include "MSCrossSection.h"
#include "MSE3MoveReminder.h"
#include "MSE3DetectorInterface.h"
#include "MSDetectorTypedefs.h"

namespace E3
{
    enum DetType { MEAN_TRAVELTIME = 0
                   , MEAN_NUMBER_OF_HALTINGS_PER_VEHICLE
                   , NUMBER_OF_VEHICLES
                   , ALL
    };

    DetType& operator++( DetType& det );
       
    enum Containers { VEHICLES = 0
                      , HALTINGS
                      , TRAVELTIME
    };

    Containers& operator++( Containers& cont );
}

using namespace E3;

class MS_E3_Collector : public MSDetectorFileOutput
{
public:

    typedef MSE3DetectorInterface E3Detector;
    typedef std::vector< E3Detector* > DetectorCont;
    typedef DetectorCont::iterator DetContIter;
    typedef std::vector< MSDetectorContainerWrapperBase* > ContainerCont;
    typedef ContainerCont::iterator ContainerContIter;
    typedef SingletonDictionary<
        std::string, MS_E3_Collector* > E3Dictionary;
    typedef std::vector< Detector::E3EntryReminder* > EntryReminders;
    typedef std::vector< Detector::E3LeaveReminder* > LeaveReminders;
    

    MS_E3_Collector(
        std::string id
        , Detector::CrossSections entries
        , Detector::CrossSections exits
        , MSUnit::Seconds haltingTimeThreshold = 1
        , MSUnit::MetersPerSecond haltingSpeedThreshold = 5.0/3.6
        , MSUnit::Seconds deleteDataAfterSeconds = 1800
        )
        :
        idM( id )
        , deleteDataAfterSecondsM( deleteDataAfterSeconds )
        , haltingTimeThresholdM(
            MSUnit::getInstance()->getSteps( haltingTimeThreshold ) )
        , haltingSpeedThresholdM(
            MSUnit::getInstance()->getCellsPerStep( haltingSpeedThreshold ) )
        , detectorsM(3)
        , containersM(3)
         
        {
    // Set MoveReminders to entries and exits
    for ( Detector::CrossSectionsIt crossSec = entries.begin();
                   crossSec != entries.end(); ++crossSec ) {
    entryRemindersM.push_back(
    new Detector::E3EntryReminder( id, *crossSec, *this ) );
            }
             for ( Detector::CrossSectionsIt crossSec = exits.begin();
                   crossSec != exits.end(); ++crossSec ) {
    leaveRemindersM.push_back(
    new Detector::E3LeaveReminder( id, *crossSec, *this ) );
            }
            
            // insert object into dictionary
             if ( ! E3Dictionary::getInstance()->isInsertSuccess( idM, this ) ){
    assert( false );
            }
        }
    
    virtual ~MS_E3_Collector( void )
        {
    for( EntryReminders::iterator it3 = entryRemindersM.begin();
                  it3 != entryRemindersM.end(); ++it3 ) {
    delete *it3;
            }
             for( LeaveReminders::iterator it4 = leaveRemindersM.begin();
                  it4 != leaveRemindersM.end(); ++it4 ) {
    delete *it4;
            }
             for ( DetContIter it1 = detectorsM.begin();
                   it1 != detectorsM.end(); ++it1 ) {
    if ( *it1 != 0 ) {
    delete *it1;
                }
            }
             for ( ContainerContIter it2 = containersM.begin();
                   it2 != containersM.end(); ++it2 ) {
    if ( *it2 != 0 ) {
    delete *it2;
                }
            }            
        }
    
    

    // is called from MSE3MoveReminder if vehicle touches entry-crossSection
    void enter( MSVehicle& veh )
        {
    for ( ContainerContIter it = containersM.begin();
                   it != containersM.end(); ++it ) {
    if ( *it != 0 ) {
    (*it)->enterDetectorByMove( &veh );
                }
            }
        }
    
    // is called from MSE3MoveReminder if vehicle passes entry-crossSection
    void leave( MSVehicle& veh )
        {
            for ( DetContIter det = detectorsM.begin();
                  det != detectorsM.end(); ++det ) {
                if ( *det != 0 ) {
                    (*det)->leave( veh );
                }
            }
            for ( ContainerContIter cont = containersM.begin();
                  cont != containersM.end(); ++cont ) {
                if ( *cont != 0 ) {
                    (*cont)->leaveDetectorByMove( &veh );
                }
            }
        }

    void addDetector( DetType type, std::string detId = "" )
        {
    if ( detId == "" ) {
    detId = idM;
            }
             if ( type != ALL ) {
    createDetector( type, detId );
            }
             else {
    for ( DetType typ = MEAN_TRAVELTIME; typ < ALL; ++typ ){    
    createDetector( typ, detId );
                }
            }
        }
    
    bool hasDetector( DetType type )
        {
    return getDetector( type ) != 0;
        }
    
    double getAggregate( DetType type, MSUnit::Seconds lastNSeconds )
        {
    assert( type != ALL );
             E3Detector* det = getDetector( type );
             if ( det != 0 ){
    return det->getAggregate( lastNSeconds );
            }
            // requested type not present
            // create it and return nonsens value for the first access
             addDetector( type, std::string("") );
             return std::numeric_limits< double >::max();
        }

protected:

    E3Detector* getDetector( DetType type ) const
        {
    assert( type != ALL );
             return detectorsM[ type ];
        }

private:
    std::string idM;
    
    EntryReminders entryRemindersM;
    LeaveReminders leaveRemindersM;
    
    MSUnit::Seconds deleteDataAfterSecondsM;
    MSUnit::Steps haltingTimeThresholdM;
    MSUnit::CellsPerStep haltingSpeedThresholdM;
    
    DetectorCont detectorsM;
    
    ContainerCont containersM;

    void createContainer( Containers type )
        {
            switch( type ){
                case VEHICLES:
                {
                    if ( containersM[ VEHICLES ] == 0 ) {
                        containersM[ VEHICLES ] =
                            new DetectorContainer::Count();
                    }
                    break;
                }
                case TRAVELTIME:
                {
                    if ( containersM[ TRAVELTIME ] == 0 ) {
                        containersM[ TRAVELTIME ] =
                            new DetectorContainer::TraveltimeMap();
                    }
                    break;
                }
                case HALTINGS:
                {
                    if ( containersM[ HALTINGS ] == 0 ) {
                        containersM[ HALTINGS ] =
                            new DetectorContainer::HaltingsMap(
                                haltingTimeThresholdM,
                                haltingSpeedThresholdM );
                    }
                    break;
                }
                default:
                {
                    assert( 0 );
                }
            }
        }
    
    void createDetector( DetType type, std::string detId )
        {
            if ( hasDetector( type ) ) {
                return;
            }
            using namespace Detector;
            switch ( type ) {
                case MEAN_TRAVELTIME:
                {
                    createContainer( TRAVELTIME );
                    detectorsM[ MEAN_TRAVELTIME ] =
                        new E3Traveltime(
                            E3Traveltime::getDetectorName() + detId,
                            deleteDataAfterSecondsM,
                            *static_cast< DetectorContainer::TraveltimeMap* >(
                                containersM[ TRAVELTIME ] ) );
                    break;
                }
                case MEAN_NUMBER_OF_HALTINGS_PER_VEHICLE:
                {
                    createContainer( HALTINGS );
                    detectorsM[ MEAN_NUMBER_OF_HALTINGS_PER_VEHICLE ] =
                        new E3MeanNHaltings(
                            E3MeanNHaltings::getDetectorName() + detId,
                            deleteDataAfterSecondsM,
                            *static_cast< DetectorContainer::HaltingsMap* >(
                                containersM[ HALTINGS ] ) );
                    break;
                }
                case NUMBER_OF_VEHICLES:
                {
                    createContainer( VEHICLES );
                    detectorsM[ NUMBER_OF_VEHICLES ] =
                        new E3NVehicles(
                            E3NVehicles::getDetectorName() + detId,
                            deleteDataAfterSecondsM,
                            *static_cast< DetectorContainer::VehicleMap* >(
                                containersM[ VEHICLES ] ) );
                    break;
                }
                default:
                {
                    assert( 0 );
                }
            }
        }
};



// Local Variables:
// mode:C++
// End:

#endif // MS_E3_COLLECTOR_H
