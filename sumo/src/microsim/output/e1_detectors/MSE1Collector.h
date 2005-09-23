#ifndef MSE1COLLECTOR_H
#define MSE1COLLECTOR_H

///
/// @file    MSE1Collector.h
/// @author  Christian Roessel <christian.roessel@gmx.de>
/// @date    Started Wed Jun 9 16:40:56 CEST 2004
/// @version $Id$
///
/// @brief
///

/* Copyright (C) 2004 by Christian Roessel <christian.roessel@gmx.de> */

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Id$
// $Log$
// Revision 1.9  2005/09/23 13:16:41  dkrajzew
// debugging the building process
//
// Revision 1.8  2005/09/22 13:45:51  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.7  2005/09/15 11:08:09  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.6  2005/05/04 08:10:12  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.5  2005/02/01 10:10:43  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.4  2004/12/16 12:14:59  dkrajzew
// got rid of an unnecessary detector parameter/debugging
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <microsim/MSMoveReminder.h>
#include <microsim/output/MSDetectorFileOutput.h>
#include <string>
#include <cassert>
#include <vector>
#include <microsim/output/MSCrossSection.h>
#include "MSE1MoveReminder.h"
#include <microsim/MSVehicleQuitReminded.h>
#include <microsim/output/MSLDDetectorInterface.h>
#include <microsim/MSVehicle.h>
#include <microsim/output/MSDetectorTypedefs.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/XMLDevice.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>

#include "MSE1.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
class MSE1Collector : public MSDetectorFileOutput,
                      public MSVehicleQuitReminded
{
public:
    typedef LD::MSDetectorInterface LDDetector;
    typedef std::vector< LDDetector* > DetectorCont;
    typedef DetectorCont::iterator DetContIter;

    MSE1Collector(
        std::string id
        , MSCrossSection crossSection
        , SUMOTime deleteDataAfterSeconds = 1800
        )
        :
        idM( id )
        , crossSectionM( crossSection )
        , deleteDataAfterSecondsM( deleteDataAfterSeconds )
        , detectorsM(5)
        , vehOnDetectorM(E1::E1Container())
        {
            entryReminderM =
                new Detector::E1EntryReminder( id, crossSectionM, *this );
            leaveReminderM =
                new Detector::E1LeaveReminder( id, crossSectionM, *this );
/*
            // insert object into dictionary
            if ( ! E1Dictionary::getInstance()->isInsertSuccess( idM, this ) ){
                MsgHandler::getErrorInstance()->inform(
                    "e1-detector '" + idM + "' could not be build;");
                MsgHandler::getErrorInstance()->inform(
                    " (declared twice?)");
                throw ProcessError();            }*/
        }

    /// Dtor. Deletes the created detectors.
    virtual ~MSE1Collector( void )
        {
            delete entryReminderM;
            delete leaveReminderM;
            deleteContainer( detectorsM );
        }

    /// Call if a vehicle touches an entry-cross-section. Usually
    /// called by LD::MSMoveReminder. Inserts vehicle into internal
    /// containers.
    ///
    /// @param veh The entering vehicle.
    ///
    void enter( MSVehicle& veh, MSUnit::Seconds entryTime )
        {
            // SAGE DETEKTOREN BESCHEID. BISHER: SAGE CONTAINERN BESCHEID
            vehOnDetectorM.enter( &veh, entryTime );
            veh.quitRemindedEntered(this);
        }

    /// Remove vehicles that entered the detector but reached their
    /// destination before passing the leave-cross-section.
    ///
    /// @param veh The vehicle to remove.
    ///
    void removeOnTripEnd( MSVehicle *veh )
        {
            // MACHE IRGENDWAS
        }

    /// Call if a vehicle passes a leave-cross-section. Usually called
    /// by LD::MSMoveReminder. Removed vehicle from internal
    /// containers.
    ///
    /// @param veh The leaving vehicle.
    ///
    void leave( MSVehicle& veh, MSUnit::Seconds leaveTime )
        {
            vehOnDetectorM.setLeaveTime( &veh, leaveTime );
            for ( DetContIter det = detectorsM.begin();
                  det != detectorsM.end(); ++det ) {
                if ( *det != 0 ) {
                    (*det)->leave( veh );
                }
            }
            veh.quitRemindedLeft(this);
            vehOnDetectorM.clear( &veh );
        }

    /// If the vehicle leaves the crossSection by lane-change, it
    /// need to be removed from some internal E1::E1Container. This is
    /// done here.
    ///
    /// @param veh The leaving vehicle.
    ///
    void dismissByLaneChange( MSVehicle& veh )
        {
            vehOnDetectorM.clear( &veh );
        }

    /// Add, i.e. create the requested detector or all detectors out
    /// of E1::DetType.
    ///
    /// @param type One detetcor out of E1::DetType. ALL will create
    /// all detectors.
    /// @param detId Optional id of the newly created detector(s).
    ///
    void addDetector( E1::DetType type, std::string detId = "" )
        {
            if ( detId == "" ) {
                detId = idM;
            }
            if ( type != E1::ALL ) {
                createDetector( type, detId );
            }
            else {
                for ( E1::DetType typ = E1::MEAN_SPEED;
                      typ < E1::ALL; ++typ ){
                    createDetector( typ, detId );
                }
            }
        }

    /// Checks if the requested detector is present.
    ///
    /// @param type The detector out of E1::DetType you are interested in.
    ///
    /// @return True if the detector exists, false otherwise.
    ///
    bool hasDetector( E1::DetType type )
        {
            return getDetector( type ) != 0;
        }

    /// Get the aggregated value of a detector. If the detector
    /// doesn't exist, create it and return -1 for this first call.
    ///
    /// @param type The detector of E1::DetType you are interested in.
    /// @param lastNSeconds Length of the aggregation intervall
    /// (now-lastNSeconds, now].
    ///
    /// @return If the detector exists, return it's aggregated value,
    /// else return -1.
    ///
    SUMOReal getAggregate( E1::DetType type, MSUnit::Seconds lastNSeconds )
        {
            assert( type != E1::ALL );
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
    /// @return The detectors unique id.
    ///
    const std::string& getId() const
        {
            return idM;
        }

    ///
    /// @name Methods, inherited from MSDetectorFileOutput.
    ///
    /// @{

    /// Returns a string indentifying this class. Used to create
    /// distinct filenames.
    ///
    /// @return Always "MSE1Collector".
    ///
    std::string  getNamePrefix( void ) const
        {
            return std::string("MSE1Collector");
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
    /// @return XML-output of all existing concrete detectors.
    ///
    void writeXMLOutput( XMLDevice &dev, SUMOTime startTime, SUMOTime stopTime )
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
            dev.writeString("<detector type=\"E1_Collector\" id=\"").writeString(
                idM).writeString("\" >\n");
            dev.writeString("<location lane=\"").writeString(
                crossSectionM.laneM->id()).writeString("\" pos=\"").writeString(
                toString( crossSectionM.posM )).writeString("\" />\n" );
        }

    /// Get an closing XML-tag corresponding to the opening tag from
    /// getXMLDetectorInfoStart().
    ///
    /// @return The member infoEndM.
    ///
    const std::string& getXMLDetectorInfoEnd( void ) const
    {
        return infoEndM;
    }

    /// Get the clean-up interval length.
    ///
    /// @return Interval-length in steps.
    ///
    SUMOTime getDataCleanUpSteps( void ) const
        {
            return deleteDataAfterSecondsM; /// !!! Konvertierung
        }

    /// @}

protected:


    std::string idM;            ///< The detector's unique id.

    MSCrossSection crossSectionM;

    Detector::E1EntryReminder* entryReminderM;
    Detector::E1LeaveReminder* leaveReminderM;

    SUMOTime deleteDataAfterSecondsM; ///< Data removal interval.

    DetectorCont detectorsM;    ///< Container of E1-detectors.

    E1::E1Container vehOnDetectorM;

    static std::string xmlHeaderM; ///< Header for the XML-output.

    static std::string infoEndM; ///< Closing detector tag.

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

    /// Create a detector out of E1::DetType. The created detector is
    /// put into member detectorsM. On creation, the detector gets one
    /// of the "vehicle"-containers out of E1::Containers.
    ///
    /// @param type The detector you are interested in.
    /// @param detId Detector-id, need not be unique.
    ///
    void createDetector( E1::DetType type, std::string detId )
        {
            if ( hasDetector( type ) ) {
                return;
            }
            using namespace Detector;
            switch ( type ) {
                case E1::MEAN_SPEED:
                {
                    detectorsM[ E1::MEAN_SPEED ] =
                        new E1MeanSpeed(
                            E1MeanSpeed::getDetectorName() + detId,
							(MSUnit::Seconds) deleteDataAfterSecondsM,
                            vehOnDetectorM );
                    break;
                }
                case E1::NUMBER_OF_VEHICLES:
                {
//                     detectorsM[ E1::NUMBER_OF_VEHICLES ] =
//                         new E1NVehicles(
//                             E1NVehicles::getDetectorName() + detId,
//                             deleteDataAfterSecondsM );
                    break;
                }
                case E1::FLOW:
                {
//                     detectorsM[ E1::FLOW ] =
//                         new E1Flow(
//                             E1Flow::getDetectorName() + detId,
//                             deleteDataAfterSecondsM );
                    break;
                }
                case E1::MEAN_OCCUPANCY:
                {
//                     detectorsM[ E1::MEAN_OCCUPANCY ] =
//                         new E1MeanOccupancy(
//                             E1MeanOccupancy::getDetectorName() + detId,
//                             deleteDataAfterSecondsM );
                    break;
                }
                case E1::MEAN_VEHICLE_LENGTH:
                {
//                     detectorsM[ E1::MEAN_VEHICLE_LENGTH ] =
//                         new E1MeanVehicleLength(
//                             E1MeanVehicleLength::getDetectorName() + detId,
//                             deleteDataAfterSecondsM );
                    break;
                }
                default:
                {
                    assert( 0 );
                }
            }
        }

    /// Get a pointer to the requested detector out of E1::DetType.
    ///
    /// @param type The detector you are interested in.
    ///
    /// @return If the detector exists, a pointer to the valid object,
    /// 0 otherwise.
    ///
    LDDetector* getDetector( E1::DetType type ) const
        {
            assert( type != E1::ALL );
            return detectorsM[ type ];
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
				(MSUnit::Seconds) MSUnit::getInstance()->getSeconds( (MSUnit::Steps) (stopTime-startTime) ) + 1;
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

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

