#ifndef MSE3COLLECTOR_H
#define MSE3COLLECTOR_H

///
/// @file    MSE3Collector.h
/// @author  Christian Roessel <christian.roessel@dlr.de>
/// @date    Started Tue Dec 02 2003 22:17 CET
/// @version
///
/// @brief
///
///

/* Copyright (C) 2003, 2004 by German Aerospace Center (http://www.dlr.de) */

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)

/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <microsim/MSMoveReminder.h>
#include <microsim/output/MSDetectorFileOutput.h>
#include <string>
#include <cassert>
#include <vector>
#include <limits>
#include <microsim/output/MSDetectorContainerWrapperBase.h>
#include <microsim/output/MSDetectorContainerWrapper.h>
#include <microsim/output/MSDetectorCounterContainerWrapper.h>
#include <microsim/output/MSDetectorHaltingContainerWrapper.h>
#include <microsim/output/MSDetectorOccupancyCorrection.h>
#include <microsim/output/MSCrossSection.h>
#include "MSE3MoveReminder.h"
#include <microsim/output/MSLDDetectorInterface.h>
#include <microsim/output/MSDetectorTypedefs.h>
#include <microsim/MSVehicleQuitReminded.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/XMLDevice.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>


///
/// Introduces some enums and consts for use in MSE3Collector.
///
namespace E3
{
    ///
    /// Collection of all possible E3-detectors. Names should be
    /// self-explanatory. All E3-detectors are LD-detectors (leave-data).
    ///
    enum DetType { MEAN_TRAVELTIME = 0
                   , MEAN_NUMBER_OF_HALTINGS_PER_VEHICLE
                   , NUMBER_OF_VEHICLES
                   , ALL        ///< Use this to generate all possible
                                ///detectors in
                                ///MSE3Collector::addDetector().
    };

    /// Increment operator that allows us to iterate over the
    /// E3::DetType detectors.
    ///
    /// @param det A detector out of E3::DetType (an integral type).
    ///
    /// @return The detector that follows det.
    ///
    DetType& operator++( DetType& det );

    ///
    /// Collection of different "vehicle" containers used by MSE3Collector.
    ///
    enum Containers { VEHICLES = 0
                      , HALTINGS
                      , TRAVELTIME
    };

    /// Increment operator that allows us to iterate over the
    /// E2::Containers detectors.
    ///
    /// @param cont A container out of E2::Containers (an integral type).
    ///
    /// @return The container that follows cont.
    Containers& operator++( Containers& cont );
}

/* =========================================================================
 * class definitions
 * ======================================================================= */

/// Collection of E3-detectors. E3-detectors are defined by a set of
/// in-cross-sections and out-cross-sections. Vehicles, that pass an
/// in- and out-cross-section are detected when they pass the
/// out-cross-section. Vehicles passing the out-cross-section without
/// having passed the in-cross-section are not detected. You can add
/// detectors via addDetector() out of E3::DetType to work on the
/// detection area. You get a sampled value via getAggregate(). As
/// MSE3Collector inherits from MSDetectorFileOutput there is the
/// possibility to get file output by calling
/// MSDetector2File::addDetectorAndInterval().
///
class MSE3Collector : public MSDetectorFileOutput,
                      public MSVehicleQuitReminded
{
public:

    typedef LD::MSDetectorInterface LDDetector;
    typedef std::vector< LDDetector* > DetectorCont;
    typedef DetectorCont::iterator DetContIter;

    typedef std::vector< MSDetectorContainerWrapperBase* > ContainerCont;
    typedef ContainerCont::iterator ContainerContIter;
    typedef std::vector< Detector::E3EntryReminder* > EntryReminders;
    typedef std::vector< Detector::E3LeaveReminder* > LeaveReminders;

    /// Ctor. Sets reminder objects on entry- and leave-lanes and
    /// inserts itself to an E3Dictionary for global access to all
    /// MSE3Collector objects.
    ///
    /// @param id The detector's unique id.
    /// @param entries Entry-cross-sections.
    /// @param exits Leavey-cross-sections.
    /// @param haltingTimeThreshold A vehicle must at least drive that time
    /// at a speed lesser than haltingSpeedThreshold to be a "halting" vehicle.
    /// @param haltingSpeedThreshold A vehicle must not drive a greater speed
    /// for at more than haltingTimeThreshold  to be a "halting" vehicle.
    /// @param deleteDataAfterSeconds Delete the data that is collected every
    /// timestep so that data is always present for at least
    /// deleteDataAfterSeconds.
    ///
    MSE3Collector(
        std::string id
        , Detector::CrossSections entries
        , Detector::CrossSections exits
        , MSUnit::Seconds haltingTimeThreshold
        , MSUnit::MetersPerSecond haltingSpeedThreshold
        , SUMOTime deleteDataAfterSeconds
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
/*
            // insert object into dictionary
            if ( ! E3Dictionary::getInstance()->isInsertSuccess( idM, this ) ){
                MsgHandler::getErrorInstance()->inform(
                    "e3-detector '" + idM + "' could not be build;");
                MsgHandler::getErrorInstance()->inform(
                    " (declared twice?)");
                throw ProcessError();
            }
            */
        }

    /// Dtor. Deletes the created detectors.
    virtual ~MSE3Collector( void )
        {
            deleteContainer( entryRemindersM );
            deleteContainer( leaveRemindersM );
            deleteContainer( detectorsM );
            deleteContainer( containersM );
        }

    /// Call if a vehicle touches an entry-cross-section. Usually
    /// called by LD::MSMoveReminder. Inserts vehicle into internal
    /// containers.
    ///
    /// @param veh The entering vehicle.
    ///
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

    /// Call if a vehicle passes a leave-cross-section. Usually called
    /// by LD::MSMoveReminder. Removed vehicle from internal
    /// containers.
    ///
    /// @param veh The leaving vehicle.
    ///
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

    /// Add, i.e. create the requested detector or all detectors out
    /// of E3::DetType.
    ///
    /// @param type One detetcor out of E3::DetType. ALL will create
    /// all detectors.
    /// @param detId Optional id of the newly created detector(s).
    ///
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

    /// Checks if the requested detector is present.
    ///
    /// @param type The detector out of E3::DetType you are interested in.
    ///
    /// @return True if the detector exists, false otherwise.
    ///
    bool hasDetector( E3::DetType type )
        {
            return getDetector( type ) != 0;
        }

    /// Get the aggregated value of a detector. If the detector
    /// doesn't exist, create it and return -1 for this first call.
    ///
    /// @param type The detector of E3::DetType you are interested in.
    /// @param lastNSeconds Length of the aggregation intervall
    /// (now-lastNSeconds, now].
    ///
    /// @return If the detector exists, return it's aggregated value,
    /// else return -1.
    ///
    SUMOReal getAggregate( E3::DetType type, MSUnit::Seconds lastNSeconds )
        {
            assert( type != E3::ALL );
            LDDetector* det = getDetector( type );
            if ( det != 0 ){
                return det->getAggregate( lastNSeconds );
            }
            // requested type not present
            // create it and return nonsens value for the first access
            addDetector( type, std::string("") );
            return -1;
        }

    /// Get the detectors unique id.
    ///
    /// @return Th detectors unique id.
    ///
    const std::string& getId() const
        {
            return idM;
        }

    /// Remove vehicles that entered the detector but reached their
    /// destination before passing the leave-cross-section from
    /// internal containers.
    ///
    /// @param veh The vehicle to remove.
    ///
    void removeOnTripEnd( MSVehicle *veh ) {
        for ( ContainerContIter cont = containersM.begin();
                  cont != containersM.end(); ++cont  ) {
                if ( *cont != 0 ) {
                    (*cont)->removeOnTripEnd( veh );
                }
        }
    }

    ///
    /// @name Methods, inherited from MSDetectorFileOutput.
    ///
    /// @{

    /// Returns a string indentifying this class. Used to create
    /// distinct filenames.
    ///
    /// @return Always "MSE3Collector".
    ///
    std::string  getNamePrefix( void ) const
        {
            return std::string("MSE3Collector");
        }

    /// Get a header for file output which shall contain some
    /// explanation of the output generated by getXMLOutput.
    ///
    /// @return The member xmlHeaderM.
    ///
    void writeXMLHeader( XMLDevice &dev ) const
        {
            dev.writeString(xmlHeaderM);
        }

    /// Get the XML-formatted output of the concrete detector.  Calls
    /// resetQueueLengthAheadOfTrafficLights() if the detector
    /// E2::QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_VEHICLES exists.
    ///
    /// @param lastNTimesteps Generate data out of the interval
    /// (now-lastNTimesteps, now].
    ///
    /// @return XML-output of all existing concrete detectors. Except
    /// APPROACHING_VEHICLES_STATES.
    ///
    void writeXMLOutput( XMLDevice &dev,
        SUMOTime startTime, SUMOTime stopTime )
        {
            dev.writeString("<interval begin=\"").writeString(
                toString(startTime)).writeString("\" end=\"").writeString(
                toString(stopTime)).writeString("\" ");
            if(dev.needsDetectorName()) {
                dev.writeString("id=\"").writeString(idM).writeString("\" ");
            }
            writeXMLOutput( dev, detectorsM, startTime, stopTime );
            dev.writeString("/>");
        }

    /// Get an opening XML-tag containing information about the detector.
    ///
    /// @return String describing the detetctor-collection.
    ///
    void writeXMLDetectorInfoStart( XMLDevice &dev ) const
        {
            dev.writeString("<detector type=\"E3_Collector\" id=\"");
            dev.writeString(idM).writeString("\" >\n");
            std::string entries;
            Detector::CrossSections::const_iterator crossSec;
            for ( crossSec = entriesM.begin(); crossSec != entriesM.end();
                  ++crossSec ) {
                dev.writeString("  <entry lane=\"").writeString(
                    crossSec->laneM->id()).writeString("\" pos=\"").writeString(
                    toString( crossSec->posM )).writeString("\" />\n");
            }
            std::string exits;
            for ( crossSec = exitsM.begin(); crossSec != exitsM.end();
                  ++crossSec ) {
                dev.writeString("  <exit lane=\"").writeString(
                    crossSec->laneM->id()).writeString("\" pos=\"").writeString(
                    toString( crossSec->posM )).writeString("\" />\n");
            }
        }

    /// Get an closing XML-tag corresponding to the opening tag from
    /// getXMLDetectorInfoStart().
    ///
    /// @return The member infoEndM.
    ///
    void writeXMLDetectorInfoEnd( XMLDevice &dev ) const
    {
        dev.writeString(infoEndM);
    }

    /// Get the clean-up interval length.
    ///
    /// @return Interval-length in steps.
    ///
    SUMOTime getDataCleanUpSteps( void ) const
        {
            return deleteDataAfterSecondsM; // !!! Konvertierung
        }

    /// @}

protected:

    /// Get a pointer to the requested detector out of E3::DetType.
    ///
    /// @param type The detector you are interested in.
    ///
    /// @return If the detector exists, a pointer to the valid object,
    /// 0 otherwise.
    ///
    LDDetector* getDetector( E3::DetType type ) const
        {
            assert( type != E3::ALL );
            return detectorsM[ type ];
        }

    std::string idM;            ///< The detector's unique id.

    Detector::CrossSections entriesM; ///< Container of detector entries.
    Detector::CrossSections exitsM; ///< Container of detector exits.

    EntryReminders entryRemindersM; ///< Container of entryReminders.
    LeaveReminders leaveRemindersM; ///< Container of leaveReminders.

    MSUnit::Steps haltingTimeThresholdM; ///< Time-theshold to
                                         ///determine if a vehicle is
                                         ///halting.
    MSUnit::CellsPerStep haltingSpeedThresholdM; ///< Speed-theshold
                                                 ///to determine if a
                                                 ///vehicle is
                                                 ///halting.
    SUMOTime deleteDataAfterSecondsM; ///< Data removal interval.

    DetectorCont detectorsM;    ///< Container of E3-detectors.

    ContainerCont containersM;  ///< Container of helper-containers.

    static std::string xmlHeaderM; ///< Header for the XML-output.

    static std::string infoEndM; ///< Closing detector tag.

    /// Create a "vehicle"-container out of E3::Containers. They may
    /// be shared among several detectors. The created container is
    /// put into the member containersM. A container holds pointers to
    /// all "vehicles" currently on the detector.
    ///
    /// @param type The container you are interested in.
    ///
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

    /// Create a detector out of E3::DetType. The created detector is
    /// put into member detectorsM. On creation, the detector gets one
    /// of the "vehicle"-containers out of E3::Containers.
    ///
    /// @param type The detector you are interested in.
    /// @param detId Detector-id, need not be unique.
    ///
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
                            (MSUnit::Seconds) deleteDataAfterSecondsM,
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
                            (MSUnit::Seconds) deleteDataAfterSecondsM,
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
                            (MSUnit::Seconds) deleteDataAfterSecondsM,
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

    /// Get aggregated XML-output for all detectors in a container.
    ///
    /// @param container A container holding detectors.
    /// @param lastNTimesteps The length of the aggregation interval.
    ///
    /// @return A XML-formatted string.
    ///
    template< class Cont >
    void writeXMLOutput( XMLDevice &dev, Cont& container,
            SUMOTime startTime, SUMOTime stopTime)
        {
            MSUnit::Seconds lastNSeconds =
                (MSUnit::Seconds) stopTime-startTime+1;
            for ( typename Cont::iterator it = container.begin();
                  it != container.end(); ++it ) {

                if ( *it == 0 ) {
                    continue;
                }
                dev.writeString((*it)->getName()).writeString("=\"").writeString(
                    toString( (*it)->getAggregate( lastNSeconds ))).writeString(
                    "\" ");
            }
        }

    /// Deletes the elements of a container.
    ///
    /// @param cont The container whose elements shall be deleted.
    ///
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

};
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/



// Local Variables:
// mode:C++
// End:

#endif // MSE3COLLECTOR_H
