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
#include "utils/convert/ToString.h"

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

//using namespace E3;

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
        , MSUnit::MetersPerSecond haltingSpeedThreshold = 5.0 / 3.6
        , MSUnit::Seconds deleteDataAfterSeconds = 1800
        )
        :
        idM( id )
        , entriesM( entries )
        , exitsM( exits )
        , haltingTimeThresholdM(
            MSUnit::getInstance()->getSteps( haltingTimeThreshold ) )
        , haltingSpeedThresholdM(
            MSUnit::getInstance()->getCellsPerStep( haltingSpeedThreshold ) )
        , deleteDataAfterSecondsM( deleteDataAfterSeconds )
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

    void addDetector( E3::DetType type, std::string detId = "" )
        {
            if ( detId == "" ) {
                detId = idM;
            }
            if ( type != E3::ALL ) {
                createDetector( type, detId );
            }
            else {
                for ( E3::DetType typ = E3::MEAN_TRAVELTIME;
                      typ < E3::ALL; ++typ ){    
                    createDetector( typ, detId );
                }
            }
        }
    
    bool hasDetector( E3::DetType type )
        {
            return getDetector( type ) != 0;
        }
    
    double getAggregate( E3::DetType type, MSUnit::Seconds lastNSeconds )
        {
            assert( type != E3::ALL );
            E3Detector* det = getDetector( type );
            if ( det != 0 ){
                return det->getAggregate( lastNSeconds );
            }
            // requested type not present
            // create it and return nonsens value for the first access
            addDetector( type, std::string("") );
            return std::numeric_limits< double >::max();
        }
    
    /**
     * @name Inherited MSDetectorFileOutput methods.
     *
     */
    //@{
    /**
     * Returns a string indentifying an object of this class. Used for
     * distinct filenames.
     */
    std::string  getNamePrefix( void ) const
        {
            return std::string("MS_E3_Collector");
        }

    /**
     * Get a header for file output which shall contain some
     * explanation of the output generated by getXMLOutput.
     */
    std::string& getXMLHeader( void ) const
        {
            return xmlHeaderM;
        }

    /**
     * Get the XML-formatted output of the concrete detector.
     *
     * @param lastNTimesteps Generate data out of the interval
     * (now-lastNTimesteps, now].
     */
    std::string getXMLOutput( MSUnit::IntSteps lastNTimesteps )
        {
            std::string result;
            MSUnit::Seconds lastNSeconds =
                MSUnit::getInstance()->getSeconds( lastNTimesteps );
            for ( DetContIter it = detectorsM.begin();
                  it != detectorsM.end(); ++it ) {

                if ( *it == 0 ) {
                    continue;
                }

                std::string name = getDetectorName( *it );
                std::string aggregate =
                    toString( (*it)->getAggregate( lastNSeconds ) );
                
                result += std::string("<") +
                    name +
                    std::string(" value=\"") +
                    aggregate +
                    std::string("\"/>\n");
            }
            return result;
        }

    /**
     * Get an opening XML-element containing information about the detector.
     */
    std::string  getXMLDetectorInfoStart( void ) const
        {
            std::string entries;
            Detector::CrossSections::const_iterator crossSec;
            for ( crossSec = entriesM.begin(); crossSec != entriesM.end();
                  ++crossSec ) {
                entries += "  <entry lane=\"" +
                    crossSec->laneM.id() + "\" pos=\"" +
                    toString( crossSec->posM ) + "\" />\n";
            }
            std::string exits;
            for ( crossSec = exitsM.begin(); crossSec != exitsM.end();
                  ++crossSec ) {
                exits += "  <entry lane=\"" +
                    crossSec->laneM.id() + "\" pos=\"" +
                    toString( crossSec->posM ) + "\" />\n";
            }            
                
            std::string
                detectorInfo("<detector type=\"E3_Collector\" id=\"" + idM +
                             "\" >\n" + entries + exits );
            return detectorInfo;
        }

    /**
     * Get the data-clean up interval in timesteps.
     */
    MSUnit::IntSteps getDataCleanUpSteps( void ) const
        {
            return MSUnit::getInstance()->getIntegerSteps(
                deleteDataAfterSecondsM );
        }
    //@}

    
protected:

    E3Detector* getDetector( E3::DetType type ) const
        {
            assert( type != E3::ALL );
            return detectorsM[ type ];
        }

private:
    std::string idM;

    Detector::CrossSections entriesM;
    Detector::CrossSections exitsM;
    
    EntryReminders entryRemindersM;
    LeaveReminders leaveRemindersM;  

    MSUnit::Steps haltingTimeThresholdM;
    MSUnit::CellsPerStep haltingSpeedThresholdM;
    MSUnit::Seconds deleteDataAfterSecondsM;
    
    DetectorCont detectorsM;
    
    ContainerCont containersM;

    static std::string xmlHeaderM;
    
    void createContainer( E3::Containers type )
        {
            switch( type ){
                case E3::VEHICLES:
                {
                    if ( containersM[ E3::VEHICLES ] == 0 ) {
                        containersM[ E3::VEHICLES ] =
                            new DetectorContainer::Count();
                    }
                    break;
                }
                case E3::TRAVELTIME:
                {
                    if ( containersM[ E3::TRAVELTIME ] == 0 ) {
                        containersM[ E3::TRAVELTIME ] =
                            new DetectorContainer::TimeMap();
                    }
                    break;
                }
                case E3::HALTINGS:
                {
                    if ( containersM[ E3::HALTINGS ] == 0 ) {
                        containersM[ E3::HALTINGS ] =
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
    
    void createDetector( E3::DetType type, std::string detId )
        {
            if ( hasDetector( type ) ) {
                return;
            }
            using namespace Detector;
            switch ( type ) {
                case E3::MEAN_TRAVELTIME:
                {
                    createContainer( E3::TRAVELTIME );
                    detectorsM[ E3::MEAN_TRAVELTIME ] =
                        new E3Traveltime(
                            E3Traveltime::getDetectorName() + detId,
                            deleteDataAfterSecondsM,
                            *static_cast< DetectorContainer::TimeMap* >(
                                containersM[ E3::TRAVELTIME ] ) );
                    break;
                }
                case E3::MEAN_NUMBER_OF_HALTINGS_PER_VEHICLE:
                {
                    createContainer( E3::HALTINGS );
                    detectorsM[ E3::MEAN_NUMBER_OF_HALTINGS_PER_VEHICLE ] =
                        new E3MeanNHaltings(
                            E3MeanNHaltings::getDetectorName() + detId,
                            deleteDataAfterSecondsM,
                            *static_cast< DetectorContainer::HaltingsMap* >(
                                containersM[ E3::HALTINGS ] ) );
                    break;
                }
                case E3::NUMBER_OF_VEHICLES:
                {
                    createContainer( E3::VEHICLES );
                    detectorsM[ E3::NUMBER_OF_VEHICLES ] =
                        new E3NVehicles(
                            E3NVehicles::getDetectorName() + detId,
                            deleteDataAfterSecondsM,
                            *static_cast< DetectorContainer::VehicleMap* >(
                                containersM[ E3::VEHICLES ] ) );
                    break;
                }
                default:
                {
                    assert( 0 );
                }
            }
        }

    std::string getDetectorName( E3Detector* det )
        {
            // get detector name. Unfortunately we have to cast
            // because the name is not known to
            // MSE2DetectorInterface.

            std::string name;
            if ( dynamic_cast< Detector::E3NVehicles* >( det ) ) {
                name = Detector::E3NVehicles::getDetectorName();
            }
            if ( dynamic_cast< Detector::E3Traveltime* >( det ) ) {
                name = Detector::E3Traveltime::getDetectorName();
            }
            if ( dynamic_cast< Detector::E3MeanNHaltings* >( det ) ) {
                name = Detector::E3MeanNHaltings::getDetectorName();
            }
            else {
                assert( 0 );
            }
        }
    
};



// Local Variables:
// mode:C++
// End:

#endif // MS_E3_COLLECTOR_H
