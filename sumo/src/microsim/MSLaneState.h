#ifndef MSLaneState_H
#define MSLaneState_H

/**
 * @file   MSLaneState.h
 * @author Christian Roessel
 * @date   Started Tue, 18 Feb 2003
 * @version $Id$
 * @brief  Declaration of class MSLaneState and helper functions.
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

#include "MSNet.h"
#include "MSDetectorFileOutput.h"
#include "MSMoveReminder.h"
#include <string>
#include <deque>
#include <map>

class MSLane;


/**
 * Overhead-detector that works on a part of a MSLane. It provides current and
 * aggregated data for waitingQueueLength, MeanSpeed, MeanSpeedSquare,
 * MeanDensity, Traveltime and counts of vehicles that contributed to the
 * data calculation, that enterd the detector, left the detector by move and
 * that passed the detector entirely. See the get* methods for details.
 *
 * It provides XML-Output for use with MSTravelcostDetector and
 * MSDetector2File via the MSDetectorFileOutput methods.
 *
 * Tis detector contacts the vehicles via the MSMoveReminder mechanism.
 *
 * @see MSMoveReminder
 * @see MSTravelcostDetector
 * @see MSDetectorFileOutput
 */
class MSLaneState
    : public MSMoveReminder,
      public MSDetectorFileOutput
{
public:
    /** 
     * Constructor. Adds object into a SingletonDictionary. Set old-data
     * removal event. Adds reminder to MSLane.
     * 
     * @param id Unique id.
     * @param lane Lane where detector woks on.
     * @param beginInMeters Startposition of detector [m].
     * @param lengthInMeters Detectorlength [m].
     * @param deleteDataAfterSeconds Dismiss-time for collected data [s].  
     */
    MSLaneState( std::string    id,
                 MSLane*        lane,
                 double         beginInMeters,
                 double         lengthInMeters,
                 MSNet::Time    deleteDataAfterSeconds = 900 );

    /** 
     * Destructor. Clears containers.
     */
    virtual ~MSLaneState();


    /**
     * @name Inherited MSMoveReminder methods.
     *
     * Methods in this group are inherited from MSMoveReminder. They are
     * called by the moving, entering and leaving vehicles.
     * 
     */
    //@{
    /** 
     * Indicator if the reminders is still active for the passed
     * vehicle/parameters. If false, the vehicle will erase this
     * reminder from it's reminder-container. This method will
     * determine the entry- and leave-time of the counted vehicle and
     * pass this information to the methods enterDetectorByMove() and
     * eaveDetectorByMove().
     * 
     * @param veh Vehicle that asks this remider.
     * @param oldPos Position before move.
     * @param newPos Position after move with newSpeed.
     * @param newSpeed Moving speed.
     * 
     * @return True if vehicle hasn't passed the detector completely.
     *
     * @see enterDetectorByMove
     * @see leaveDetectorByMove
     */
    bool isStillActive( MSVehicle& veh,
                        double oldPos,
                        double newPos,
                        double newSpeed );
    

    /** 
     *  Informs corresponding detector via leaveDetectorByLaneChange()
     *  if vehicle leaves by lanechange.
     * 
     * @param veh The leaving vehicle.
     *
     * @see leaveDetectorByLaneChange
     */
    void dismissByLaneChange( MSVehicle& veh );
 
    /** 
     * Informs corresponding detector if vehicle enters the reminder
     * by emit or lanechange. Only vehicles that are completely in
     * front of the detector will return true.
     * 
     * @param veh The entering vehilcle.
     * 
     * @return True if vehicle is on or in front of the detector.
     */
    bool isActivatedByEmitOrLaneChange( MSVehicle& veh );
    //@}

    
    /**
     * Calculates the meanValue of the waiting-queue length during the
     * lastNTimesteps. Vehicles in a waiting-queue have a gap <= vehLength.
     *
     * @param lastNTimesteps take data out of the interval
     * (now-lastNTimesteps, now]
     *
     * @return mean waiting-queue length
     */
    double getNumberOfWaiting( MSNet::Time lastNTimesteps );

    /**
     * Get the waitingQueueLength.
     * Vehicles in a waiting-queue have a gap <= vehLength.
     * If called before the vehicles are
     * moved, the value of the previous timestep is returned, if called
     * after move, the value of the current timestep is returned. Currently
     * the junctions do their job before the move, so if you call the method
     * by a junction, you will get the value of the previous timestep.
     *
     * @return waitingQueueLength of the current or previous timestep
     */
    int getCurrentNumberOfWaiting( void );

    /** 
     * Calculates the mean speed averaged over the lastNTimesteps.
     * 
     * @param lastNTimesteps take data out of the intervall
     * (now-lastNTimesteps, now]
     * 
     * @return Mean speed averaged over the lastNTimesteps
     */
    double getMeanSpeed( MSNet::Time lastNTimesteps );

    /** 
     * Calculates the mean speed of the current timestep.
     * 
     * 
     * @return Current mean speed.
     */
    double getCurrentMeanSpeed( void );

    /** 
     * Calculates the mean speed square averaged over the lastNTimesteps.
     * 
     * @param lastNTimesteps take data out of the intervall
     * (now-lastNTimesteps, now]
     * 
     * @return Mean speed square averaged over the lastNTimesteps
     */
    double getMeanSpeedSquare( MSNet::Time lastNTimesteps );

    /** 
     * Calculates the mean speed square of the current timestep.
     * 
     * 
     * @return Current mean speed square.
     */
    double getCurrentMeanSpeedSquare( void );

    /** 
     * Calculates the mean density averaged over the lastNTimesteps.
     * 
     * @param lastNTimesteps  take data out of the intervall
     * (now-lastNTimesteps, now]
     * 
     * @return Mean density averaged over the lastNTimesteps.
     */
    double getMeanDensity( MSNet::Time lastNTimesteps );
    
    /** 
     * Calculates the density of the current timestep.
     * 
     * 
     * @return Current density.
     */
    double getCurrentDensity( void );

    /** 
     * Calculates the mean traveltime averaged over the lastNTimesteps. Only
     * vehicles that passed the entire detector contribute. These vehicles
     * may have entered the detector during a previous intervall.
     * 
     * @param lastNTimesteps take data out of the intervall
     * (now-lastNTimesteps, now]
     * 
     * @return Mean traveltime averaged over the lastNTimesteps.
     */
    double getMeanTraveltime( MSNet::Time lastNTimesteps );

    /** 
     * Counts the vehicles that added data through addMoveData() over
     * the lastNTimesteps. These vehicles contribute to
     * getMeanDensity(), getMeanSpeed() and
     * getCurrentMeanSpeedSquare().
     * 
     * @param lastNTimesteps take data out of the intervall
     * (now-lastNTimesteps, now] 
     * 
     * @return Number of vehicles that added data over the lastNTimesteps.
     */
    int getNVehContributed( MSNet::Time lastNTimesteps );

    /** 
     * Counts the vehicles that entered the detector by move, emit or
     * lanechange over the lastNTimesteps.
     * 
     * @param lastNTimesteps take data out of the intervall
     * (now-lastNTimesteps, now]  
     * 
     * @return Number of vehicles that antered the detector over the
     * lastNTimesteps.
     */
    int getNVehEnteredDetector( MSNet::Time lastNTimesteps );

    /** 
     * Counts the vehicles that left the detector by move over the
     * lastNTimesteps. Vehicles that left by lanechange or that reached their
     * destination do not contribute.
     * 
     * @param lastNTimesteps take data out of the intervall
     * (now-lastNTimesteps, now]  
     * 
     * @return Number of vehilces that left the detector by move over the
     * lastNTimesteps.
     */
    int getNVehLeftDetectorByMove( MSNet::Time lastNTimesteps );

    /** 
     * Counts the vehicles that left the detector over the lastNTimesteps
     * and passed the entire detector without interruption. These vehicles
     * may have entered the detector before "now - lastNTimesteps". Only
     * those contribute to getMeanTraveltime().
     * 
     * @param lastNTimesteps take data out of the intervall
     * (now-lastNTimesteps, now] 
     * 
     * @return Numner of vehicles that left the detector over the
     * lastNTimesteps and passed the entire detector.
     */
    int getNVehPassedEntireDetector( MSNet::Time lastNTimesteps );

    /**
     * @name Inherited MSDetectorFileOutput methods. 
     *
     * @see MSDetectorFileOutput
     */
    //@{
    /** 
     * Returns a string indentifiing the class. Used for filenames in
     * MSTravelcostDetector
     * 
     * @see MSTravelcostDetector
     * @return String "MSLaneState"
     */
    std::string getNamePrefix( void ) const;

    
    /** 
     * Creates a XML-header and -comment explaining the data written by
     * getXMLOutput().
     * 
     * @return XML-header and comment.
     */
    std::string& getXMLHeader( void ) const;
    

    /** 
     * Creates XML-output for the interval (now-lastNTimesteps, now] containing
     * getMeanTraveltime(), getMeanSpeed(), getMeanSpeedSquare(),
     * getNVehContributed(), getNVehPassedEntireDetector(),
     * getNVehEnteredDetector() and getNVehLeftDetectorByMove().
     * 
     * @param lastNTimesteps take data out of the intervall
     * (now-lastNTimesteps, now]
     * 
     * @return XML-output of calculated data for the intervall
     * (now-lastNTimesteps, now] 
     */
    std::string getXMLOutput( MSNet::Time lastNTimesteps );


    /** 
     * Creates an open xml tag with information about the detector. You
     * should close this tag with getXMLDetectorInfoEnd().
     *
     * @see getXMLDetectorInfoEnd()
     * 
     * @return XML-header and comment.
     */
    std::string getXMLDetectorInfoStart( void ) const;


    /** 
     * Closes the detector-info-tag opened by getXMLDetectorInfoStart()
     *
     * @see getXMLDetectorInfoStart()
     * 
     * @return String </detector>
     */
    std::string& getXMLDetectorInfoEnd( void ) const;


    /** 
     * Get the data-clean up interval in timesteps.
     * 
     */
    MSNet::Time getDataCleanUpSteps( void ) const;
    //@}

    
    /**
     * @name Reminder methods.
     *
     * Methods in this group are called by MSLaneStateReminder
     * only. They add the data to calculate the get* values.
     *
     * @see MSLaneStateReminder
     */
    //@{
    
    /** 
     * Add data from moving vehicles. Each moving vehicle on the
     * detector has to call this method in every timestep via the reminder
     * mechanism. 
     *
     * @see MSLaneStateReminder
     * @param veh A reference to the vehicle that adds data.
     * @param newSpeed The speed with which the vehicle will move in this
     * timestep.
     * @param timestepFraction Fraction of the timestep the
     * vehicle is on the detector.
     */
    void addMoveData( MSVehicle& veh, double newSpeed,
                      double timestepFraction );

    /** 
     * Introduces a vehicle to the detector that enters it by move. Called
     * by MSLaneStateReminder only.
     *
     * @see MSLaneStateReminder
     * @param veh The entering vehicle.
     * @param enterTimestepFraction Fraction of the timestep the
     * vehicle is on the detector.
     */
    void enterDetectorByMove( MSVehicle& veh, double enterTimestepFraction );

    /** 
     * Introduces a vehicle to the detetctor that enters it by
     * emission or lanechange. Called by MSLaneStateReminder only.
     *
     * @see MSLaneStateReminder
     * @param veh The entering vehocle.
     */
    void enterDetectorByEmitOrLaneChange( MSVehicle& veh );

    /** 
     * Removes a vehicle from the detector that leaves by move. Vehicles that
     * reached their destination are also handled by this method. Called by
     * MSLaneStateReminder only.
     *
     * @see MSLaneStateReminder
     * @param veh The leaving vehicle.
     * @param leaveTimestepFraction Fraction of the timestep the
     * vehicle is on the detector.
     */
    void leaveDetectorByMove( MSVehicle& veh, double leaveTimestepFraction );

    /** 
     * Removes a vehicle from the detector that leaves by lanechange. Called by
     * MSLaneStateReminder only.
     *
     * @see MSLaneStateReminder
     * @param veh The leaving vehicle.
     */
    void leaveDetectorByLaneChange( MSVehicle& veh );

    //@}

    /**
     * @name Simulationloop methods.
     *
     * Call the following methods once in every timestep in the
     * simulation loop. Neccessary for proper operation of the
     * detector.
     * 
     */
    //@{
    /** 
     * Calls for all MSLaneState objects actionBeforeMoveAndEmit() during the
     * simulation loop. Call it before move and emit is activated.
     * 
     */
    static void actionsBeforeMoveAndEmit( void );

    /** 
     * Calls for all MSLaneState objects actionAfterMoveAndEmit() during the
     * simulation loop. Call it after move and emit.
     * 
     */
    static void actionsAfterMoveAndEmit( void );
    //@}



    // forward declarations
    struct TimestepData;
    struct WaitingQueueElem;
    struct VehicleData;
    
    /// Type of the container where all TimestepData are stored.
    typedef std::deque< TimestepData >           TimestepDataCont;
    /// Type of the container where all WaitingQueueElem are stored.
    typedef std::vector< WaitingQueueElem >      WaitingQueueElemCont;
    /**
     * Type of the map where VehicleData of vehicles on the detector
     * are stored.
     */
    typedef std::map< std::string, VehicleData > VehicleDataMap;
    /**
     * Type of the container where VehicleData of vehicles that left the
     * detector are stored. 
     */
    typedef std::deque< VehicleData >            VehicleDataCont;

    /**
     * Collection of data for each timestep.
     */
    struct TimestepData
    {
        /// Sole ctor.
        TimestepData( MSNet::Time timestep ) :
            timestepM( timestep ),
            speedSumM(0),
            speedSquareSumM(0),
            contTimestepSumM(0),
            timestepSumM(0),
            queueLengthM(-1),
            nVehEnteredDetectorM(0)
            {}

        MSNet::Time timestepM;  /**< The timestep the data belongs to. */
        double speedSumM;       /**< Sum of the speeds the vehicles on the
                                 * detector added via addMoveData() during
                                 * timestep. [cells/timestep] */
        double speedSquareSumM; /**< Sum of the speedsquares the
                                 * vehicles on the detector added via
                                 * addMoveData() during timestep. */
        double contTimestepSumM; /**< Continuous sum of timesteps the
                                  * vehicles spend on the
                                  * detector. Continuous because
                                  * vehicles enter/leave the detector
                                  * and contribute only a fraction of
                                  * a timestep.*/
        double timestepSumM;    /**< Discrete sum of timesteps the
                                  * vehicles spend on the
                                  * detector. Every vehicle
                                  * contributes 1 even if it spend
                                  * only a fraction of a timestep. */
        int queueLengthM;       /**< Length of the waiting queue
                                 * calculated by
                                 * actionsAfterMoveAndEmit() after
                                 * move and emit of all vehicles. */
        int nVehEnteredDetectorM; /**< Number of vehicles that entered the
                                   * detector either by move or lanechange or
                                   * emit.*/
    };

    /**
     * Collection of data for each vehicle that is or has been on the detector.
     */
    struct VehicleData
    {
        /// Sole ctor.
        VehicleData( double entryContTimestep,
                     bool enteredDetectorByMove ) :
            entryContTimestepM ( entryContTimestep ),
            leaveContTimestepM ( -1 ),
            passedEntireDetectorM ( enteredDetectorByMove ),
            leftDetectorByMoveM ( false )
            {}
        
        double entryContTimestepM; /**< The vehicles entry timestep
                                    * (continuous) */
        double leaveContTimestepM; /**< The vehicles leave timestep
                                    * (continuous). */
        bool passedEntireDetectorM; /**< Indicator if vehicle passed
                                     * the entire dtetctor without
                                     * interruption. */
        bool leftDetectorByMoveM; /**< Indicator if vehicle left the
                                   * detector by move. */
    };

    /**
     * Collection of data for calculation of the
     * waiting-queue-length. Updated every timestep.
     */
    struct WaitingQueueElem
    {
        /// Sole ctor.
        WaitingQueueElem( double pos, double vehLength ) :
            posM( pos ), vehLengthM( vehLength )
            {}

        double posM;            /**< Current position of vehicle on
                                 * the detector. */
        double vehLengthM;      /**< Length of the vehicle.  */
    };

    /**
     * @name Predicate classes.
     * 
     * Predicate classes working on the nested structs VehicleData and
     * WaitingQueueElem. They are included in these structs because of
     * MSVC++ compile-problems.
     * 
     */
    //@{
    /**
     * Returns true if passed VehicleData's entryContTimestepM is lesser
     * than the passed entryTimestepBound.
     * 
     */
    struct entryTimestepLesser :
        public std::binary_function< VehicleData, double, bool >
    {
        bool operator()( const VehicleData& data,
                         double entryTimestepBound ) const
        {
            return data.entryContTimestepM < entryTimestepBound;
        }
    };

    /** 
     * Returns true if passed VehicleDataMap's entryContTimestepM is lesser
     * than the passed entryTimestepBound.
     * 
     */
    struct entryTimestepLesserMap :
            public std::binary_function< VehicleDataMap::value_type,
                double, bool >
    {
        bool operator()( const VehicleDataMap::value_type& data,
                         double entryTimestepBound ) const
        {
            return data.second.entryContTimestepM < entryTimestepBound;
        }
    };

    /**
     * Returns true if passed VehicleData's leaveContTimestepM is lesser 
     * than the passed leaveTimestepBound.
     * 
     */
    struct leaveTimestepLesser :
            public std::binary_function< VehicleData, double, bool >
    {
        bool operator()( const VehicleData& data,
                         double leaveTimestepBound ) const
        {
            return data.leaveContTimestepM < leaveTimestepBound;
        }
    };

    /**
     * Returns true if first WaitingQueueElem's posM is greater than
     * the second WaitingQueueElem's posM.
     * 
     */
    struct PosGreater : public std::binary_function<
        const WaitingQueueElem, const WaitingQueueElem, bool >
    {
        // Sort criterion for std::vector< WaitingQueueElem >
        // We sort in descending order
        bool operator() ( const WaitingQueueElem p1,
                          const WaitingQueueElem p2 ) const {
            return p1.posM > p2.posM;
        }
    };
    //@}


protected:
    /** 
     * Creates a new TimestepData mand adds it to timestepDataM.
     * 
     * @return true. MSVC++ requires a return value, but there is no
     * need for it.
     */
    bool actionBeforeMoveAndEmit( void );

    /** 
     * Currently just calls calcWaitingQueueLength().
     * 
     * @return true. MSVC++ requires a return value, but there is no
     * need for it.
     */
    bool actionAfterMoveAndEmit( void );

    /** 
     * Deletes data from timestepDataM and vehLeftDetectorM if 
     * deleteDataAfterStepsM is over. Is called via MSEventControl.
     * 
     * @return deleteDataAfterStepsM to recur the event. 
     */
    MSNet::Time deleteOldData( void );

    /** 
     * Calculates the waiting-queue-length every timestep from
     * waitingQueueElemsM and stores value in timestepDataM.
     * 
     */
    void calcWaitingQueueLength( void );

    /** 
     * Calculates a valid timestep out of [0, now] from where accumulation
     * in the get* methods will start.
     * 
     * @param lastNTimesteps Number of timesteps we want to look back.
     * @see getStartIterator()
     * @return max( now - lastNTimesteps, 0 )
     */
    double getStartTimestep( MSNet::Time lastNTimesteps );

    /** 
     * Indicator if stored value nVehContributedM is still valid.
     * nVehContributedM needs to be recalculated if one of the Reminder methods
     * has been called since the last calculation or if the requested interval
     * has changed.
     * 
     * @param lastNTimesteps Interval that is requested.
     * @see Reminder methods
     * @return True if new calculation is neccessary, false otherwise.
     */
    bool needsNewCalculation( MSNet::Time lastNTimesteps );

    /** 
     * Calculates a valid interator to container from where accumulation
     * in the get* methods will start.
     * 
     * @param lastNTimesteps Number of timesteps we want to look back.
     * @param container Reference to the container we are interesed in.
     * @see getStartTimestep()
     * @return An interator out of [container.begin(), container.end() ).
     */
    template< class T >
    typename T::iterator getStartIterator( MSNet::Time lastNTimesteps,
                                           T& container )
        {
            typename T::iterator start = container.begin();
            if ( container.size() > lastNTimesteps ) {
                start =  container.end() - lastNTimesteps;
            }
            return start;
        }


private:
    TimestepDataCont     timestepDataM; /**< Container for
                                         * TimestepData. There is one
                                         * entry for each
                                         * timestep. This entry
                                         * accumulates the data from
                                         * all vehicles on the
                                         * detector at this
                                         * timestep.  */
    
    VehicleDataMap       vehOnDetectorM; /**< Container for VehicleData of
                                          * vehicles that are currently on
                                          * the detector. */
    WaitingQueueElemCont waitingQueueElemsM; /**< Container for
                                              * WaitingQueueElem. Cleared every
                                              * timestep after
                                              * calcWaitingQueueLength(). */
    
    VehicleDataCont      vehLeftDetectorM; /**< Container for VehicleData of
                                            * vehicles that already left
                                            * the detector. */

    double startPosM;                /**< Start-position of the
                                      * detector [cells]. */

    double endPosM;             /**< End-position of the detector [cells]. */

    const MSNet::Time deleteDataAfterStepsM; /**< Keep at least data from
                                              * the last
                                              * deleteDataAfterStepsM steps. */

    bool modifiedSinceLastLookupM; /**< Indicator if container have been
                                    * modified since last
                                    * getNVehContributed() call. */

    MSNet::Time lookedUpLastNTimestepsM; /**< Interval of last
                                          * getNVehContributed() call. */

    int nVehContributedM;       /**< Returned value of last
                                 * getNVehContributed() call. */

    /**< Container that holds all MSLaneState objects. Used by
     * actionsBeforeMoveAndEmit() and actionsAfterMoveAndEmit(). */
    static std::vector< MSLaneState* > laneStateDetectorsM; 

    static std::string xmlHeaderM; /**< String containing a xml-comment. */

    static std::string detectorInfoEndM; /**< String containing closing tag
                                             * of detector. */
    
    /// Hidden constructor.
    MSLaneState();

    /// Hidden copy constructor.
    MSLaneState( const MSLaneState& );

    /// Hidden assignment operator.
    MSLaneState& operator=( const MSLaneState& );
};


namespace
{
    /**
     * @name Binary-functions to use with std::accumulate.
     * 
     */
    //@{
    /// Adds up TimestepData::speedSumM.
    inline double speedSum( double sumSoFar,
                            const MSLaneState::TimestepData& data )
    {
        return sumSoFar + data.speedSumM;
    }

    /// Adds up TimestepData::speedSquareSumM.
    inline double speedSquareSum( double sumSoFar,
                                  const MSLaneState::TimestepData& data )
    {
        return sumSoFar + data.speedSquareSumM;
    }

    /// Adds up TimestepData::contTimestepSumM.
    inline double contTimestepSum( double sumSoFar,
                                   const MSLaneState::TimestepData& data )
    {
        return sumSoFar + data.contTimestepSumM;
    }

    /// Adds up TimestepData::timestepSumM.
    inline double timestepSum( double sumSoFar,
                               const MSLaneState::TimestepData& data )
    {
        return sumSoFar + data.timestepSumM;
    }

    /// Adds up TimestepData::queueLengthM.
    inline double waitingQueueSum( double sumSoFar,
                                   const MSLaneState::TimestepData& data )
    {
        return sumSoFar + data.queueLengthM;
    }

    /// Adds up traveltimes if vehicles has passed the entire detetctor.
    inline double traveltimeSum( double sumSoFar,
                                 const MSLaneState::VehicleData& data )
    {
        if ( data.passedEntireDetectorM ) {
            return sumSoFar + data.leaveContTimestepM -
                data.entryContTimestepM;
        }
        return sumSoFar;
    }

    /// Adds up VehicleData::leftDetectorByMoveM.
    inline int leftByMoveSum( int sumSoFar,
                              const MSLaneState::VehicleData& data )
    {
        return sumSoFar + data.leftDetectorByMoveM;
    }

    /// Adds up VehicleData::passedEntireDetectorM.
    inline int passedEntireSum( int sumSoFar,
                                const MSLaneState::VehicleData& data )
    {
        return sumSoFar + data.passedEntireDetectorM;
    }

    /// Adds up TimestepData::nVehEnteredDetectorM.
    inline int nVehEnteredSum( int sumSoFar,
                               const MSLaneState::TimestepData& data )
    {
        return sumSoFar + data.nVehEnteredDetectorM;
    }
    //@}
}

#endif

// Local Variables:
// mode:C++
// End:
