#ifndef MSE3COLLECTOR_H
#define MSE3COLLECTOR_H

///
/// @file    MSE3Collector.h
/// @author  Christian Roessel <christian.roessel@dlr.de>
/// @date    Started Tue Dec 02 2003 22:17 CET
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
#include "MSLDDetectorInterface.h"
#include "MSDetectorTypedefs.h"
#include "MSVehicleQuitReminded.h"
#include <utils/convert/ToString.h>

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

class MSE3Collector : public MSDetectorFileOutput,
        public MSVehicleQuitReminded
{
public:

    typedef LD::MSDetectorInterface LDDetector;
    typedef std::vector< LDDetector* > DetectorCont;
    typedef DetectorCont::iterator DetContIter;
    typedef std::vector< MSDetectorContainerWrapperBase* > ContainerCont;
    typedef ContainerCont::iterator ContainerContIter;
    typedef SingletonDictionary<
        std::string, MSE3Collector* > E3Dictionary;
    typedef std::vector< Detector::E3EntryReminder* > EntryReminders;
    typedef std::vector< Detector::E3LeaveReminder* > LeaveReminders;


    MSE3Collector(
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
            for ( Detector::CrossSectionsIt crossSec1 = entries.begin();
                  crossSec1 != entries.end(); ++crossSec1 ) {
                entryRemindersM.push_back(
                    new Detector::E3EntryReminder( id, *crossSec1, *this ) );
            }
            for ( Detector::CrossSectionsIt crossSec2 = exits.begin();
                  crossSec2 != exits.end(); ++crossSec2 ) {
                leaveRemindersM.push_back(
                    new Detector::E3LeaveReminder( id, *crossSec2, *this ) );
            }

            // insert object into dictionary
            if ( ! E3Dictionary::getInstance()->isInsertSuccess( idM, this ) ){
                assert( false );
            }
        }

    virtual ~MSE3Collector( void )
        {
            deleteContainer( entryRemindersM );
            deleteContainer( leaveRemindersM );
            deleteContainer( detectorsM );
            deleteContainer( containersM );
        }



    // is called from LD::MSMoveReminder if vehicle touches entry-crossSection
    void enter( MSVehicle& veh )
        {
            for ( ContainerContIter it = containersM.begin();
                  it != containersM.end(); ++it ) {
                if ( *it != 0 ) {
                    (*it)->enterDetectorByMove( &veh );
                }
            }
            veh.quitRemindedEntered(this);
        }

    // is called from LD::MSMoveReminder if vehicle passes entry-crossSection
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
            veh.quitRemindedLeft(this);
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
            LDDetector* det = getDetector( type );
            if ( det != 0 ){
                return det->getAggregate( lastNSeconds );
            }
            // requested type not present
            // create it and return nonsens value for the first access
            addDetector( type, std::string("") );
            return -1;//!!!std::numeric_limits< double >::max();
        }

    const std::string &getId() {
        return idM;
    }

    void removeOnTripEnd( MSVehicle *veh ) {
        for ( ContainerContIter cont = containersM.begin();
                  cont != containersM.end(); ++cont  ) {
                if ( *cont != 0 ) {
                    (*cont)->removeOnTripEnd( veh );
                }
        }
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
            return std::string("MSE3Collector");
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
            result += getXMLOutput( detectorsM, lastNTimesteps );
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
                    crossSec->laneM->id() + "\" pos=\"" +
                    toString( crossSec->posM ) + "\" />\n";
            }
            std::string exits;
            for ( crossSec = exitsM.begin(); crossSec != exitsM.end();
                  ++crossSec ) {
                exits += "  <exit lane=\"" +
                    crossSec->laneM->id() + "\" pos=\"" +
                    toString( crossSec->posM ) + "\" />\n";
            }

            std::string
                detectorInfo("<detector type=\"E3_Collector\" id=\"" + idM +
                             "\" >\n" + entries + exits );
            return detectorInfo;
        }

    const std::string& getXMLDetectorInfoEnd( void ) const
    {
        return infoEndM;
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

    LDDetector* getDetector( E3::DetType type ) const
        {
            assert( type != E3::ALL );
            return detectorsM[ type ];
        }

protected:
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
                            new DetectorContainer::VehicleMap();
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

    template< class Cont >
    std::string getXMLOutput( Cont& container, MSUnit::IntSteps lastNTimesteps)
        {
            MSUnit::Seconds lastNSeconds =
                MSUnit::getInstance()->getSeconds( lastNTimesteps );
            std::string result;
            for ( typename Cont::iterator it = container.begin();
                  it != container.end(); ++it ) {

                if ( *it == 0 ) {
                    continue;
                }
                result += (*it)->getName() +
                    std::string("=\"") +
                    toString( (*it)->getAggregate( lastNSeconds ) ) +
                    std::string("\" ");
            }
            return result;
        }

    template< class Cont >
    void deleteContainer( Cont& cont )
        {
            for ( typename Cont::iterator it = cont.begin();
                  it != cont.end(); ++it ) {
                if ( *it != 0 ) {
                    delete *it;
                }
            }
        }

private:
    static std::string infoEndM;
};


// Local Variables:
// mode:C++
// End:

#endif // MSE3COLLECTOR_H
