#ifndef MSE2COLLECTOR_H
#define MSE2COLLECTOR_H

///
/// @file    MSE2Collector.h
/// @author  Christian Roessel <christian.roessel@dlr.de>
/// @date    Started Tue Dec 02 2003 22:13 CET
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
#include "MSDetectorTypedefs.h"
#include "MSUnit.h"
#include "MSLane.h"
#include "MSDetectorFileOutput.h"
#include "MSDetectorContainerWrapperBase.h"
#include "MSDetectorContainerWrapper.h"
#include "MSDetectorHaltingContainerWrapper.h"
#include "MSDetectorCounterContainerWrapper.h"
#include "MSDetectorOccupancyCorrection.h"
#include "helpers/SingletonDictionary.h"
#include "utils/convert/ToString.h"
#include <string>
#include <cassert>
#include <vector>
#include <limits>
#include "MSApproachingVehiclesStates.h"

namespace E2
{
    enum DetType { DENSITY = 0, // TD
                   MAX_JAM_LENGTH_IN_VEHICLES,
                   MAX_JAM_LENGTH_IN_METERS,
                   JAM_LENGTH_SUM_IN_VEHICLES,
                   JAM_LENGTH_SUM_IN_METERS,
                   QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_VEHICLES,
                   QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_METERS,
                   N_VEHICLES,
                   OCCUPANCY_DEGREE,
                   SPACE_MEAN_SPEED,
                   CURRENT_HALTING_DURATION_SUM_PER_VEHICLE,
                   N_STARTED_HALTS, // ED
                   HALTING_DURATION_SUM, // ED
                   HALTING_DURATION_MEAN, // LD
                   APPROACHING_VEHICLES_STATES, // special treatment
                   ALL }; 

    const DetType lastTD = CURRENT_HALTING_DURATION_SUM_PER_VEHICLE;
    const DetType lastED = HALTING_DURATION_SUM;
    const DetType lastLD = HALTING_DURATION_MEAN;
    
    DetType& operator++( DetType& det );      

    enum Containers { COUNTER = 0,
                      VEHICLES,
                      HALTINGS };
    
    Containers& operator++( Containers& cont );
}

//using namespace E2;

class
MSE2Collector : public MSMoveReminder,
                     public MSDetectorFileOutput
{
public:

    typedef TD::MSDetectorInterface TDDetector;
    typedef std::vector< TDDetector* > DetTDCont;
    typedef DetTDCont::iterator DetTDContIter;
    
    typedef ED::MSDetectorInterface EDDetector;
    typedef std::vector< EDDetector* > DetEDCont;
    typedef DetEDCont::iterator DetEDContIter;

    typedef LD::MSDetectorInterface LDDetector;
    typedef std::vector< LDDetector* > DetLDCont;
    typedef DetLDCont::iterator DetLDContIter;
    typedef std::vector< MSDetectorContainerWrapperBase* > ContainerCont;
    typedef ContainerCont::iterator ContainerContIter;
    typedef SingletonDictionary< std::string,
                                 MSE2Collector* > E2Dictionary;

    MSE2Collector( std::string id, //daraus sollte sich der Filename
                        // ergeben
                        MSLane* lane,
                        MSUnit::Meters startPos,
                        MSUnit::Meters detLength,
                        MSUnit::Seconds haltingTimeThreshold = 1,
                        MSUnit::MetersPerSecond haltingSpeedThreshold =5.0/3.6,
                        MSUnit::Meters jamDistThreshold = 10,
                        MSUnit::Seconds deleteDataAfterSeconds = 1800 )
        : MSMoveReminder( lane, id ),
          startPosM( startPos ),
          endPosM( startPos + detLength ),
          deleteDataAfterSecondsM( deleteDataAfterSeconds ),
          haltingTimeThresholdM( MSUnit::getInstance()->getSteps(
                                     haltingTimeThreshold ) ),
          haltingSpeedThresholdM( MSUnit::getInstance()->getCellsPerStep(
                                      haltingSpeedThreshold ) ),
          jamDistThresholdM( MSUnit::getInstance()->getCells(
                                 jamDistThreshold ) ),
          detectorsTDM(11),
          detectorsEDM(2),
          detectorsE3M(1),
          containersM(3),
          occupancyCorrectionM(),
          approachingVehStatesDetectorM(0)
        {
            assert( laneM != 0 );
            MSUnit::Meters laneLength =
                MSUnit::getInstance()->getMeters( laneM->length() );
            assert( startPosM >= 0 &&
                    startPosM < laneLength );
            assert( endPosM - startPosM > 0 && endPosM < laneLength );

            // insert object into dictionary
            if ( ! E2Dictionary::getInstance()->isInsertSuccess(
                     idM, this ) ) {
                assert( false );
            }
        }

    virtual ~MSE2Collector( void )
        {
            deleteContainer( detectorsTDM );
            deleteContainer( detectorsEDM );
            deleteContainer( detectorsE3M );
            deleteContainer( containersM );
            if ( approachingVehStatesDetectorM != 0 ) {
                delete approachingVehStatesDetectorM;
            }
        }

    virtual bool amVisible() const {
        return false;
    }


    const MSApproachingVehiclesStates::DetectorAggregate
    getCurrentApproachingStates( unsigned nApproachingVeh ) 
        {
            if ( approachingVehStatesDetectorM == 0 ) {
                addDetector( E2::APPROACHING_VEHICLES_STATES );
            }
            return approachingVehStatesDetectorM->getDetectorAggregate(
                nApproachingVeh );
        }
    
    
    double getCurrent( E2::DetType type )
        {
            assert( type <= E2::lastTD );
            
            TDDetector* det = static_cast< TDDetector* >( getDetector( type ));
            if ( det != 0 ){
                return det->getCurrent();
            }
            // requested type not present
            // create it and return nonsens value for the first access
            addDetector( type, std::string("") );
            return std::numeric_limits< double >::max();
        }



    double getAggregate( E2::DetType type, MSUnit::Seconds lastNSeconds )
        {
            assert( type <= E2::lastLD );
            
            MSDetectorInterfaceCommon* det = getDetector( type );
            if ( det != 0 ){
                return det->getAggregate( lastNSeconds );
            }
            
            // requested type not present
            // create it and return nonsens value for the first access
            addDetector( type, std::string("") );
            return std::numeric_limits< double >::max();
        }
    
    bool hasDetector( E2::DetType type )
        {
            return getDetector( type ) != 0;
        }

    void addDetector( E2::DetType type, std::string detId = "" )
        {
            if ( detId == "" ) {
                detId = idM;
            }
            if ( type != E2::ALL ) {
                createDetector( type, detId );
            }
            else {
                for ( E2::DetType typ = E2::DENSITY; typ < E2::ALL; ++typ ){
                    createDetector( typ, detId );
                }
            }
        }
        
    
    void resetQueueLengthAheadOfTrafficLights( void )
        {
            using namespace Detector;
            MSQueueLengthAheadOfTrafficLightsInVehicles* det1 = 0;
            if ((det1 = static_cast<
                 E2QueueLengthAheadOfTrafficLightsInVehicles* >(
                     detectorsTDM[
                         E2::QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_VEHICLES ]))){
                det1->resetMax();
            }
            MSQueueLengthAheadOfTrafficLightsInMeters* det2 = 0;
            if ((det2 = static_cast<
                 E2QueueLengthAheadOfTrafficLightsInMeters* >(
                     detectorsTDM[
                         E2::QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_METERS ]))){
                det2->resetMax();
            }
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
                        double newPos,
                        double )
        {
            if ( newPos <= startPosM ) {
                // detector not yet reached
                return true;
            }
            if ( oldPos <= startPosM && newPos > startPosM ) {
                // vehicle will enter detectors
                for ( ContainerContIter it = containersM.begin();
                      it != containersM.end(); ++it ) {
                    if ( *it != 0 ) {
                        (*it)->enterDetectorByMove( &veh );
                    }
                }
            }
            if ( newPos - veh.length() < startPosM ) {
                // vehicle entered detector partially
                occupancyCorrectionM.setOccupancyEntryCorrection(
                    veh, ( newPos - startPosM ) / veh.length() );
            }
            if ( newPos > endPosM && newPos - veh.length() <= endPosM ) {
                // vehicle left detector partially
                occupancyCorrectionM.setOccupancyLeaveCorrection(
                    veh, (endPosM - (newPos - veh.length() ) ) /
                    veh.length() );
            }
            if ( newPos - veh.length() > endPosM ) {
                // vehicle will leave detector
                for ( ContainerContIter it = containersM.begin();
                      it != containersM.end(); ++it ) {
                    if ( *it != 0 ) {
                        (*it)->leaveDetectorByMove( &veh );
                    }
                }
                return false;
            }
            return true;
        }

    void dismissByLaneChange( MSVehicle& veh )
        {
            if (veh.pos() >= startPosM && veh.pos() - veh.length() < endPosM) {
                // vehicle is on detector
                for ( ContainerContIter it = containersM.begin();
                      it != containersM.end(); ++it ) {
                    if ( *it != 0 ) {
                        (*it)->leaveDetectorByLaneChange( &veh );
                    }
                }
                if ( veh.pos() - veh.length() < startPosM ||
                     veh.pos()>endPosM && veh.pos()-veh.length()<=endPosM ) {
                    // vehicle partially on det
                    occupancyCorrectionM.dismissOccupancyCorrection( veh );
                }
            }
        }

    bool isActivatedByEmitOrLaneChange( MSVehicle& veh )
        {
            if (veh.pos() >= startPosM && veh.pos() - veh.length() < endPosM) {
                // vehicle is on detector
                for ( ContainerContIter it = containersM.begin();
                      it != containersM.end(); ++it ) {
                    if ( *it != 0 ) {
                        (*it)->enterDetectorByEmitOrLaneChange( &veh );
                    }
                }
                if ( veh.pos() - veh.length() < startPosM ) {
                    // vehicle entered detector partially
                    occupancyCorrectionM.setOccupancyEntryCorrection(
                        veh, (veh.pos() - startPosM ) / veh.length() );
                }
                if ( veh.pos()>endPosM && veh.pos()-veh.length()<=endPosM ) {
                    // vehicle left detector partially
                    occupancyCorrectionM.setOccupancyLeaveCorrection(
                        veh, ( endPosM - (veh.pos() - veh.length() ) ) /
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
            return std::string("MSE2Collector");
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
            result += getXMLOutput( detectorsTDM, lastNTimesteps );
            result += getXMLOutput( detectorsEDM, lastNTimesteps );
            result += getXMLOutput( detectorsE3M, lastNTimesteps );
            return result;
        }

    /**
     * Get an opening XML-element containing information about the detector.
     */
    std::string getXMLDetectorInfoStart( void ) const
        {
            std::string
                detectorInfo("<detector type=\"E2_Collector\" id=\"" + idM +
                             "\" lane=\"" +
                             laneM->id() + "\" startpos=\"" +
                             toString(startPosM) + "\" length=\"" +
                             toString(endPosM - startPosM) +
                             "\" >\n");
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

    double getStartPos() const
        {
            return startPosM;
        }

    double getEndPos() const
        {
            return endPosM;
        }

protected:
    MSDetectorInterfaceCommon* getDetector( E2::DetType type ) const
        {
            assert(type < E2::lastLD );
            
            if ( type <= E2::lastTD ){
                return detectorsTDM[ type ];
            }
            if ( type <= E2::lastED ) {
                return detectorsEDM[ type - E2::lastTD - 1 ];
            }
            else {
                return detectorsE3M[ type - E2::lastED - 1 ];
            }
        }

    E2::DetType getIndex( E2::DetType type ) const
        {
            assert(type <= E2::lastLD );
            
            if ( type <= E2::lastTD ){
                return type;
            }
            if ( type <= E2::lastED ) {
                return E2::DetType( type - E2::lastTD - 1 );
            }
            else {
                return E2::DetType( type - E2::lastED - 1 );
            }
        }
    
private:
    MSUnit::Meters startPosM;
    MSUnit::Meters endPosM;

    MSUnit::Seconds deleteDataAfterSecondsM;
    MSUnit::Steps haltingTimeThresholdM;
    MSUnit::CellsPerStep haltingSpeedThresholdM;
    MSUnit::Cells jamDistThresholdM;

    DetTDCont detectorsTDM;
    DetEDCont detectorsEDM;
    DetLDCont detectorsE3M;
    
    ContainerCont containersM;

    MSDetectorOccupancyCorrection occupancyCorrectionM;

    MSApproachingVehiclesStates* approachingVehStatesDetectorM;

    static std::string xmlHeaderM;

    void createContainer( E2::Containers type )
        {
            switch( type ){
                case E2::COUNTER:
                {
                    if ( containersM[ E2::COUNTER ] == 0 ) {
                        containersM[ E2::COUNTER ] =
                            new DetectorContainer::Count(
                                occupancyCorrectionM );
                    }
                    break;
                }
                case E2::VEHICLES:
                {
                    if ( containersM[ E2::VEHICLES ] == 0 ) {
                        containersM[ E2::VEHICLES ] =
                            new DetectorContainer::VehiclesList(
                                occupancyCorrectionM );
                    }
                    break;
                }
                case E2::HALTINGS:
                {
                    if ( containersM[ E2::HALTINGS ] == 0 ) {
                        containersM[ E2::HALTINGS ] =
                            new DetectorContainer::HaltingsList(
                                //occupancyCorrectionM,
                                haltingTimeThresholdM,
                                haltingSpeedThresholdM,
                                jamDistThresholdM );
                    }
                    break;
                }
                default:
                {
                    assert( 0 );
                }
            }
        }

    void createDetector( E2::DetType type, std::string detId )
        {
            if ( hasDetector( type ) ) {
                return;
            }
            using namespace Detector;
            switch ( type ) {
                case E2::DENSITY:
                {
                    createContainer( E2::COUNTER );
                    detectorsTDM[ E2::DENSITY ] =
                        new E2Density(
                            E2Density::getDetectorName() + detId,
                            endPosM - startPosM,
                            deleteDataAfterSecondsM,
                            *static_cast< DetectorContainer::Count* >(
                                containersM[ E2::COUNTER ] ) );
                    break;
                }
                case E2::MAX_JAM_LENGTH_IN_VEHICLES:
                {
                    createContainer( E2::HALTINGS );
                    detectorsTDM[ E2::MAX_JAM_LENGTH_IN_VEHICLES ] =
                        new E2MaxJamLengthInVehicles(
                            E2MaxJamLengthInVehicles::getDetectorName() +detId,
                            endPosM - startPosM,
                            deleteDataAfterSecondsM,
                            *static_cast< DetectorContainer::HaltingsList* >(
                                containersM[ E2::HALTINGS ] ) );
                    break;
                }
                case E2::MAX_JAM_LENGTH_IN_METERS:
                {
                    createContainer( E2::HALTINGS );
                    detectorsTDM[ E2::MAX_JAM_LENGTH_IN_METERS ] =
                        new E2MaxJamLengthInMeters(
                            E2MaxJamLengthInMeters::getDetectorName() + detId,
                            endPosM - startPosM,
                            deleteDataAfterSecondsM,
                            *static_cast< DetectorContainer::HaltingsList* >(
                                containersM[ E2::HALTINGS ] ) );
                    break;
                }
                case E2::JAM_LENGTH_SUM_IN_VEHICLES:
                {
                    createContainer( E2::HALTINGS );
                    detectorsTDM[ E2::JAM_LENGTH_SUM_IN_VEHICLES ] =
                        new E2JamLengthSumInVehicles(
                            E2JamLengthSumInVehicles::getDetectorName()+detId,
                            endPosM - startPosM,
                            deleteDataAfterSecondsM,
                            *static_cast< DetectorContainer::HaltingsList* >(
                                containersM[ E2::HALTINGS ] ) );
                    break;
                }
                case E2::JAM_LENGTH_SUM_IN_METERS:
                {
                    createContainer( E2::HALTINGS );
                    detectorsTDM[ E2::JAM_LENGTH_SUM_IN_METERS ] =
                        new E2JamLengthSumInMeters(
                            E2JamLengthSumInMeters::getDetectorName() + detId,
                            endPosM - startPosM,
                            deleteDataAfterSecondsM,
                            *static_cast< DetectorContainer::HaltingsList* >(
                                containersM[ E2::HALTINGS ] ) );

                    break;
                }
                case E2::QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_VEHICLES:
                {
                    if ( detectorsTDM[ E2::MAX_JAM_LENGTH_IN_VEHICLES ] == 0 ) {
                        createDetector( E2::MAX_JAM_LENGTH_IN_VEHICLES, detId );
                    }
                    detectorsTDM[
                        E2::QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_VEHICLES ] =
                        new E2QueueLengthAheadOfTrafficLightsInVehicles(
                            E2QueueLengthAheadOfTrafficLightsInVehicles::getDetectorName() + detId,
                            endPosM - startPosM,
                            deleteDataAfterSecondsM,
                            *detectorsTDM[ E2::MAX_JAM_LENGTH_IN_VEHICLES ] );
                    break;
                }
                case E2::QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_METERS:
                {
                    if ( detectorsTDM[ E2::MAX_JAM_LENGTH_IN_METERS ] == 0 ) {
                        createDetector( E2::MAX_JAM_LENGTH_IN_METERS, detId );
                    }
                    detectorsTDM[
                        E2::QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_METERS ] =
                        new E2QueueLengthAheadOfTrafficLightsInMeters(
                            E2QueueLengthAheadOfTrafficLightsInMeters::getDetectorName() + detId,
                            endPosM - startPosM,
                            deleteDataAfterSecondsM,
                            *detectorsTDM[ E2::MAX_JAM_LENGTH_IN_METERS ] );
                    break;
                }
                case E2::N_VEHICLES:
                {
                    createContainer( E2::COUNTER );
                    detectorsTDM[ E2::N_VEHICLES ] =
                        new E2NVehicles(
                            E2NVehicles::getDetectorName() + detId,
                            endPosM - startPosM,
                            deleteDataAfterSecondsM,
                            *static_cast< DetectorContainer::Count* >(
                                containersM[ E2::COUNTER ] ) );
                    break;
                }
                case E2::OCCUPANCY_DEGREE:
                {
                    createContainer( E2::VEHICLES );
                    detectorsTDM[ E2::OCCUPANCY_DEGREE ] =
                        new E2OccupancyDegree(
                            E2OccupancyDegree::getDetectorName() + detId,
                            endPosM - startPosM,
                            deleteDataAfterSecondsM,
                            *static_cast< DetectorContainer::VehiclesList* >(
                                containersM[ E2::VEHICLES ] ) );
                    break;
                }
                case E2::SPACE_MEAN_SPEED:
                {
                    createContainer( E2::VEHICLES );
                    detectorsTDM[ E2::SPACE_MEAN_SPEED ] =
                        new E2SpaceMeanSpeed(
                            E2SpaceMeanSpeed::getDetectorName() + detId,
                            endPosM - startPosM,
                            deleteDataAfterSecondsM,
                            *static_cast< DetectorContainer::VehiclesList* >(
                                containersM[ E2::VEHICLES ] ) );
                    break;
                }
                case E2::CURRENT_HALTING_DURATION_SUM_PER_VEHICLE:
                {
                    createContainer( E2::HALTINGS );
                    detectorsTDM[ E2::CURRENT_HALTING_DURATION_SUM_PER_VEHICLE] =
                        new E2CurrentHaltingDurationSumPerVehicle(
                            E2CurrentHaltingDurationSumPerVehicle::getDetectorName() + detId,
                            endPosM - startPosM,
                            deleteDataAfterSecondsM,
                            *static_cast< DetectorContainer::HaltingsList* >(
                                containersM[ E2::HALTINGS ] ) );
                    break;
                }
                case E2::APPROACHING_VEHICLES_STATES:
                {
                    createContainer( E2::VEHICLES );
                    approachingVehStatesDetectorM = new MSApproachingVehiclesStates(
                        endPosM,
                        *static_cast< DetectorContainer::VehiclesList* >(
                            containersM[ E2::VEHICLES ] ) );
                    break;
                }
                case E2::N_STARTED_HALTS:
                {
                    createContainer( E2::HALTINGS );
                    detectorsEDM[ getIndex( E2::N_STARTED_HALTS ) ] =
                        new E2NStartedHalts(
                            E2NStartedHalts::getDetectorName() + detId,
                            deleteDataAfterSecondsM,
                            *static_cast< DetectorContainer::HaltingsList* >(
                                containersM[ E2::HALTINGS ] ) );
                    break;
                }
                case E2::HALTING_DURATION_SUM:
                {
                    createContainer( E2::HALTINGS );
                    detectorsEDM[ getIndex( E2::HALTING_DURATION_SUM ) ] =
                        new E2NStartedHalts(
                            E2NStartedHalts::getDetectorName() + detId,
                            deleteDataAfterSecondsM,
                            *static_cast< DetectorContainer::HaltingsList* >(
                                containersM[ E2::HALTINGS ] ) );
                    break;
                }
                case E2::HALTING_DURATION_MEAN:
                {
                    createContainer( E2::HALTINGS );
                    detectorsE3M[ getIndex( E2::HALTING_DURATION_MEAN ) ] =
                        new E2HaltingDurationMean(
                            E2HaltingDurationMean::getDetectorName() + detId,
                            deleteDataAfterSecondsM,
                            *static_cast< DetectorContainer::HaltingsList* >(
                                containersM[ E2::HALTINGS ] ) );
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
                result += std::string("<") +
                    (*it)->getName() +
                    std::string(" value=\"") +
                    toString( (*it)->getAggregate( lastNSeconds ) ) +
                    std::string("\"/>\n");
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

};


// Local Variables:
// mode:C++
// End:

#endif // MSE2COLLECTOR_H
