#ifndef MS_E2_ZS_COLLECTOR
#define MS_E2_ZS_COLLECTOR

#include "MSMoveReminder.h"
#include "MSDetectorTypedefs.h"
#include "MSUnit.h"
#include "MSLane.h"
#include "MSDetectorFileOutput.h"
#include "MSDetectorContainer.h"
#include "MSDetectorContainerBase.h"
#include "MSHaltingDetectorContainer.h"
#include "helpers/SingletonDictionary.h"
#include "utils/convert/ToString.h"
#include <string>
#include <cassert>
#include <vector>
#include <limits>


class
MS_E2_ZS_Collector : public MSMoveReminder,
                     public MSDetectorFileOutput
{
public:
    enum DetType { DENSITY = 0,
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
                   ALL };

    enum Containers { COUNTER = 0,
                      VEHICLES,
                      HALTINGS };

    typedef MSE2DetectorInterface E2ZSDetector;
    typedef std::vector< E2ZSDetector* > DetectorCont;
    typedef DetectorCont::iterator DetContIter;
    typedef std::vector< MSDetectorContainerBase* > ContainerCont;
    typedef ContainerCont::iterator ContainerContIter;
    typedef SingletonDictionary< std::string,
                                 MS_E2_ZS_Collector* > E2ZSDictionary;

    MS_E2_ZS_Collector( std::string id, //daraus ergibt sich der Filename
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
          detectorsM(11),
          containersM(3),
          approachingVehiclesStatesDetM(0)
        {
            assert( laneM != 0 );
            MSUnit::Meters laneLength =
                MSUnit::getInstance()->getMeters( laneM->length() );
            assert( startPosM >= 0 &&
                    startPosM < laneLength );
            assert( endPosM - startPosM > 0 && endPosM < laneLength );

            // insert object into dictionary
            if ( ! E2ZSDictionary::getInstance()->isInsertSuccess(
                     idM, this ) ) {
                assert( false );
            }
        }

    ~MS_E2_ZS_Collector( void )
        {
            for ( DetContIter it1 = detectorsM.begin();
                  it1 != detectorsM.end(); ++it1 ) {
                if ( *it1 != 0 ) {
                    delete *it1;
                }
            }
            if ( approachingVehiclesStatesDetM != 0 ) {
                delete approachingVehiclesStatesDetM;
            }
            for ( ContainerContIter it2 = containersM.begin();
                  it2 != containersM.end(); ++it2 ) {
                if ( *it2 != 0 ) {
                    delete *it2;
                }
            }
        }

    double getGurrent( DetType type )
        {
            assert(type < ALL );
            E2ZSDetector* det = getDetector( type );
            if ( det != 0 ){
                return det->getCurrent();
            }
            // requested type not present
            // create it and return nonsens value for the first access
            addDetector( type, std::string("") );
            return std::numeric_limits< double >::max();
        }

    const Detector::E2ApproachingVehiclesStates::DetectorAggregate&
    getApproachingVehiclesStates( unsigned nApproachingVehicles )
        {
            if ( approachingVehiclesStatesDetM == 0 ) {
                // requested type not present
                // create it
                addApproachingVehiclesStatesDetector("");
            }
            return approachingVehiclesStatesDetM->getDetectorAggregate(
                nApproachingVehicles );
        }
    
    double getAggregate( DetType type, MSUnit::Seconds lanstNSeconds )
        {
            assert(type != ALL );
            E2ZSDetector* det = getDetector( type );
            if ( det != 0 ){
                return det->getAggregate( lanstNSeconds );
            }
            // requested type not present
            // create it and return nonsens value for the first access
            addDetector( type, std::string("") );
            return std::numeric_limits< double >::max();
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
                createDetector( DENSITY, detId );
                createDetector( MAX_JAM_LENGTH_IN_VEHICLES, detId );
                createDetector( MAX_JAM_LENGTH_IN_METERS, detId );
                createDetector( JAM_LENGTH_SUM_IN_VEHICLES, detId );
                createDetector( JAM_LENGTH_SUM_IN_METERS, detId );
                createDetector(
                    QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_VEHICLES, detId );
                createDetector(
                    QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_METERS, detId );
                createDetector( N_VEHICLES, detId );
                createDetector( OCCUPANCY_DEGREE, detId );
                createDetector( SPACE_MEAN_SPEED, detId );
                createDetector(
                    CURRENT_HALTING_DURATION_SUM_PER_VEHICLE, detId );
                addApproachingVehiclesStatesDetector( detId );
            }
        }

    void addApproachingVehiclesStatesDetector( std::string detId = "" )
        {
            if ( approachingVehiclesStatesDetM != 0 ) {
                return;
            }
            if ( detId == "" ) {
                detId = idM;
            }
            createContainer( VEHICLES );
            approachingVehiclesStatesDetM = new
                Detector::E2ApproachingVehiclesStates(
                    endPosM, *static_cast< DetectorContainer::Vehicles* >(
                        containersM[ VEHICLES ] ) );
        }
    
    bool update( void )
        {
            for ( ContainerContIter it1 = containersM.begin();
                  it1 != containersM.end(); ++it1 ) {
                    if ( *it1 != 0 ) {
                        (*it1)->update();
                    }
                }
            for ( DetContIter it2 = detectorsM.begin();
                  it2 != detectorsM.end(); ++it2 ) {
                if ( *it2 != 0 ) {
                    (*it2)->update();
                }
            }
            return true;
        }

    void resetQueueLengthAheadOfTrafficLights( void )
        {
            MSQueueLengthAheadOfTrafficLightsInVehicles* det1 = 0;
            if ((det1 = dynamic_cast<
                 MSQueueLengthAheadOfTrafficLightsInVehicles* >(
                     detectorsM[
                         QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_VEHICLES ]))){
                det1->resetMax();
            }
            MSQueueLengthAheadOfTrafficLightsInMeters* det2 = 0;
            if ((det2 = dynamic_cast<
                 MSQueueLengthAheadOfTrafficLightsInMeters* >(
                     detectorsM[
                         QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_METERS ]))){
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
                        double newSpeed )
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
                for ( DetContIter it = detectorsM.begin();
                      it != detectorsM.end(); ++it ) {
                    if ( *it != 0 ) {
                        (*it)->setOccupancyEntryCorrection(
                            veh, ( newPos - startPosM ) / veh.length() );
                    }
                }
            }
            if ( newPos > endPosM && newPos - veh.length() <= endPosM ) {
                // vehicle left detector partially
                for ( DetContIter it = detectorsM.begin();
                      it != detectorsM.end(); ++it ) {
                    if ( *it != 0 ) {
                        (*it)->setOccupancyLeaveCorrection(
                            veh, (endPosM - (newPos - veh.length() ) ) /
                            veh.length() );
                    }
                }
            }
            if ( newPos - veh.length() > endPosM ) {
                // vehicle will leave detector
                for ( ContainerContIter it = containersM.begin();
                      it != containersM.end(); ++it ) {
                    if ( *it != 0 ) {
                        (*it)->leaveDetectorByMove();
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
                    for ( DetContIter it = detectorsM.begin();
                      it != detectorsM.end(); ++it ) {
                        if ( *it != 0 ) {
                            (*it)->dismissOccupancyCorrection( veh );
                        }
                    }
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
                    for ( DetContIter it = detectorsM.begin();
                      it != detectorsM.end(); ++it ) {
                        if ( *it != 0 ) {
                            (*it)->setOccupancyEntryCorrection(
                                veh, (veh.pos() - startPosM ) / veh.length() );
                        }
                    }
                }
                if ( veh.pos()>endPosM && veh.pos()-veh.length()<=endPosM ) {
                    // vehicle left detector partially
                    for ( DetContIter it = detectorsM.begin();
                      it != detectorsM.end(); ++it ) {
                        if ( *it != 0 ) {
                            (*it)->setOccupancyLeaveCorrection(
                                veh, ( endPosM - (veh.pos() - veh.length() ) )/
                                veh.length() );
                        }
                    }
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
            return std::string("MS_E2_ZS_Collector");
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
                std::string aggregate =
                    toString( (*it)->getAggregate( lastNSeconds ) );
                if ( dynamic_cast< Detector::E2Density* >( *it ) ) {
                    result += std::string("<") +
                        Detector::E2Density::getDetectorName() +
                        std::string(" value=\"") + aggregate +
                        std::string("\"/>\n");
                }
//                 else if ( ) // check other concrete detectors
                else {
                    assert( 0 );
                }
            }
            return result;
        }

    /**
     * Get an opening XML-element containing information about the detector.
     */
    std::string  getXMLDetectorInfoStart( void ) const
        {
            std::string
                detectorInfo("<detector type=\"E2_ZS_Collector\" id=\"" + idM +
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
    E2ZSDetector* getDetector( DetType type ) const
        {
            assert(type != ALL );
            return detectorsM[ type ];
        }

private:
    MSUnit::Meters startPosM;
    MSUnit::Meters endPosM;

    MSUnit::Seconds deleteDataAfterSecondsM;
    MSUnit::Steps haltingTimeThresholdM;
    MSUnit::CellsPerStep haltingSpeedThresholdM;
    MSUnit::Cells jamDistThresholdM;

    DetectorCont detectorsM;

    ContainerCont containersM;

    Detector::E2ApproachingVehiclesStates* approachingVehiclesStatesDetM;
    
    static std::string xmlHeaderM;

    void createContainer( Containers type )
        {
            switch( type ){
                case COUNTER:
                {
                    if ( containersM[ COUNTER ] == 0 ) {
                        containersM[ COUNTER ] =
                            new DetectorContainer::Count();
                    }
                    break;
                }
                case VEHICLES:
                {
                    if ( containersM[ VEHICLES ] == 0 ) {
                        containersM[ VEHICLES ] =
                            new DetectorContainer::Vehicles();
                    }
                    break;
                }
                case HALTINGS:
                {
                    if ( containersM[ HALTINGS ] == 0 ) {
                        containersM[ HALTINGS ] =
                            new DetectorContainer::Haltings(
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

    void createDetector( DetType type, std::string detId )
        {
            using namespace Detector;
            switch ( type ) {
                case DENSITY:
                {
                    createContainer( COUNTER );
                    detectorsM[ DENSITY ] =
                        new E2Density(
                            E2Density::getDetectorName() + detId,
                            endPosM - startPosM,
                            deleteDataAfterSecondsM,
                            *static_cast< DetectorContainer::Count* >(
                                containersM[ COUNTER ] ) );
                    break;
                }
                case MAX_JAM_LENGTH_IN_VEHICLES:
                {
                    createContainer( HALTINGS );
                    detectorsM[ MAX_JAM_LENGTH_IN_VEHICLES ] =
                        new E2MaxJamLengthInVehicles(
                            E2MaxJamLengthInVehicles::getDetectorName() +detId,
                            endPosM - startPosM,
                            deleteDataAfterSecondsM,
                            *static_cast< DetectorContainer::Haltings* >(
                                containersM[ HALTINGS ] ) );
                    break;
                }
                case MAX_JAM_LENGTH_IN_METERS:
                {
                    createContainer( HALTINGS );
                    detectorsM[ MAX_JAM_LENGTH_IN_METERS ] =
                        new E2MaxJamLengthInMeters(
                            E2MaxJamLengthInMeters::getDetectorName() + detId,
                            endPosM - startPosM,
                            deleteDataAfterSecondsM,
                            *static_cast< DetectorContainer::Haltings* >(
                                containersM[ HALTINGS ] ) );
                    break;
                }
                case JAM_LENGTH_SUM_IN_VEHICLES:
                {
                    createContainer( HALTINGS );
                    detectorsM[ JAM_LENGTH_SUM_IN_VEHICLES ] =
                        new E2JamLengthSumInVehicles(
                            E2JamLengthSumInVehicles::getDetectorName()+detId,
                            endPosM - startPosM,
                            deleteDataAfterSecondsM,
                            *static_cast< DetectorContainer::Haltings* >(
                                containersM[ HALTINGS ] ) );
                    break;
                }
                case JAM_LENGTH_SUM_IN_METERS:
                {
                    createContainer( HALTINGS );
                    detectorsM[ JAM_LENGTH_SUM_IN_METERS ] =
                        new E2JamLengthSumInMeters(
                            E2JamLengthSumInMeters::getDetectorName() + detId,
                            endPosM - startPosM,
                            deleteDataAfterSecondsM,
                            *static_cast< DetectorContainer::Haltings* >(
                                containersM[ HALTINGS ] ) );

                    break;
                }
                case QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_VEHICLES:
                {
                    if ( detectorsM[ MAX_JAM_LENGTH_IN_VEHICLES ] == 0 ) {
                        createDetector( MAX_JAM_LENGTH_IN_VEHICLES, detId );
                    }
                    detectorsM[
                        QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_VEHICLES ] =
                        new E2QueueLengthAheadOfTrafficLightsInVehicles(
                            E2QueueLengthAheadOfTrafficLightsInVehicles::getDetectorName() + detId,
                            endPosM - startPosM,
                            deleteDataAfterSecondsM,
                            *detectorsM[ MAX_JAM_LENGTH_IN_VEHICLES ] );
                    break;
                }
                case QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_METERS:
                {
                    if ( detectorsM[ MAX_JAM_LENGTH_IN_METERS ] == 0 ) {
                        createDetector( MAX_JAM_LENGTH_IN_METERS, detId );
                    }
                    detectorsM[
                        QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_METERS ] =
                        new E2QueueLengthAheadOfTrafficLightsInMeters(
                            E2QueueLengthAheadOfTrafficLightsInMeters::getDetectorName() + detId,
                            endPosM - startPosM,
                            deleteDataAfterSecondsM,
                            *detectorsM[ MAX_JAM_LENGTH_IN_METERS ] );
                    break;
                }
                case N_VEHICLES:
                {
                    createContainer( COUNTER );
                    detectorsM[ N_VEHICLES ] =
                        new E2NVehicles(
                            E2NVehicles::getDetectorName() + detId,
                            endPosM - startPosM,
                            deleteDataAfterSecondsM,
                            *static_cast< DetectorContainer::Count* >(
                                containersM[ COUNTER ] ) );
                    break;
                }
                case OCCUPANCY_DEGREE:
                {
                    createContainer( VEHICLES );
                    detectorsM[ OCCUPANCY_DEGREE ] =
                        new E2OccupancyDegree(
                            E2OccupancyDegree::getDetectorName() + detId,
                            endPosM - startPosM,
                            deleteDataAfterSecondsM,
                            *static_cast< DetectorContainer::Vehicles* >(
                                containersM[ VEHICLES ] ) );
                    break;
                }
                case SPACE_MEAN_SPEED:
                {
                    createContainer( VEHICLES );
                    detectorsM[ SPACE_MEAN_SPEED ] =
                        new E2SpaceMeanSpeed(
                            E2SpaceMeanSpeed::getDetectorName() + detId,
                            endPosM - startPosM,
                            deleteDataAfterSecondsM,
                            *static_cast< DetectorContainer::Vehicles* >(
                                containersM[ VEHICLES ] ) );
                    break;
                }
                case CURRENT_HALTING_DURATION_SUM_PER_VEHICLE:
                {
                    createContainer( HALTINGS );
                    detectorsM[ CURRENT_HALTING_DURATION_SUM_PER_VEHICLE ] =
                        new E2CurrentHaltingDurationSumPerVehicle(
                            E2CurrentHaltingDurationSumPerVehicle::getDetectorName() + detId,
                            endPosM - startPosM,
                            deleteDataAfterSecondsM,
                            *static_cast< DetectorContainer::Haltings* >(
                                containersM[ HALTINGS ] ) );
                    break;
                }
                default:
                {
                    assert( 0 );
                }
            }
        }
};



#endif // MS_E2_ZS_COLLECTOR

// Local Variables:
// mode:C++
// End:
