#ifndef MSInductLoop_H
#define MSInductLoop_H

/**
 * @file   MSInductLoop.h
 * @author Christian Roessel
 * @date   Mon Jul 21 16:14:05 2003
 * @version $Id$
 * @brief  Declaration of class MSInductLoop
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
// $Log$
// Revision 1.3  2005/02/01 10:10:43  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.2  2004/12/16 12:14:59  dkrajzew
// got rid of an unnecessary detector parameter/debugging
//
// Revision 1.3  2004/12/10 11:42:53  dksumo
// detectors usage reworked
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <deque>
#include <map>
#include <functional>
#include <helpers/SingletonDictionary.h>
#include <microsim/MSMoveReminder.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSNet.h>
#include <microsim/output/MSDetectorFileOutput.h>
#include <utils/iodevices/XMLDevice.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSLane;
class GUIDetectorWrapper;
class GUIGlObjectStorage;
class GUILaneWrapper;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * An unextended detector that measures at a fixed position on a fixed
 * lane. Only vehicles that passed the entire detector are counted. We
 * ignore vehicles that are emitted onto the detector and vehicles
 * that change their lane while they are on the detector, because we
 * cannot determine a meaningful enter/leave-times.
 *
 * This detector uses the MSMoveReminder mechanism, i.e. the vehicles
 * call the detector if they pass it.
 *
 * This detector inserts itself into a SingletonDictionary so that you
 * can access all MSInductLoop instances globally. Use this dictionary
 * to delete the detectors.
 *
 * See the get*-methods to learn about what is measured. You can vary
 * the sample-intervall during runtime.
 *
 * Due to the inheritance from MSDetectorFileOutput this detector can
 * print data to a file at fixed intervals via MSDetector2File.
 *
 * @see MSDetector2File
 * @see MSMoveReminder
 * @see SingletonDictionary
 * @see MSDetectorFileOutput
 */
class MSInductLoop
    : public MSMoveReminder,
      public MSDetectorFileOutput
{
public:
    /// Type of the dictionary where all MSInductLoop are registered.
    typedef SingletonDictionary< std::string, MSInductLoop* > InductLoopDict;

    /**
     * Constructor. Adds object into a SingletonDictionary. Sets old-data
     * removal event. Adds reminder to MSLane.
     *
     * @param id Unique id.
     * @param lane Lane where detector woks on.
     * @param position Position of the detector within the lane.
     * @param deleteDataAfterSeconds Dismiss-time for collected data.
     */
    MSInductLoop( const std::string& id,
                  MSLane* lane,
                  double positionInMeters,
                  SUMOTime deleteDataAfterSeconds);


    /// Destructor. Clears containers.
    ~MSInductLoop();


    /**
     * @name MSMoveReminder methods.
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
     * @name Get-methods. These are the methods to access the
     * collected vehicle-data.
     *
     */
    //@{
    /**
     * Calculates the flow in [veh/h] over the given interval.
     *
     * @param lastNTimesteps take data out of the interval
     * (now-lastNTimesteps, now].
     *
     * @return Flow in [veh/h].
     */
    double getFlow( SUMOTime lastNTimesteps ) const;

    /**
     * Calculates the mean-speed in [m/s] over the given interval.
     *
     * @param lastNTimesteps take data out of the interval
     * (now-lastNTimesteps, now].
     *
     * @return Mean-speed in [m/s] averaged over the vehicles that
     * passed during the lastNTimesteps.
     */
    double getMeanSpeed( SUMOTime lastNTimesteps ) const;

    /*
     * Calculates the mean-speed-square in [m/(s^2)] over the given interval.
     *
     * @param lastNTimesteps take data out of the interval
     * (now-lastNTimesteps, now].
     *
     * @return Mean-speed-square in [m/(s^2)] averaged over the
     * vehicles that passed during the lastNTimesteps.
     */
//    double getMeanSpeedSquare( SUMOTime lastNTimesteps ) const;

    /**
     * Calculates the occupancy in [%], where 100% means lastNTimesteps.
     *
     * @param lastNTimesteps take data out of the interval
     * (now-lastNTimesteps, now].
     *
     * @return Occupancy in [%].
     */
    double getOccupancy( SUMOTime lastNTimesteps ) const;

    /**
     * Calculate the mean-vehicle-length in [m] averaged over the
     * vehicles that passed during the lastNTimesteps.
     *
     * @param lastNTimesteps take data out of the interval
     * (now-lastNTimesteps, now].
     *
     * @return Mean vehicle-length in [m] averaged over the vehicles
     * that passed during the lastNTimesteps.
     */
    double getMeanVehicleLength( SUMOTime lastNTimesteps ) const;

    /**
     * Counts the timesteps from the last leaving of the detector to
     * now. If the detector is currently occupied, we return 0.
     *
     * @return Timesteps from last leaving (detection) of the detector
     * to now or 0 if detector is occupied.
     */
    double getTimestepsSinceLastDetection() const;

    /**
     * How many vehicles passed the detector completely over the
     * lastNTimesteps.
     *
     * @param lastNTimesteps take data out of the interval
     * (now-lastNTimesteps, now].
     *
     * @return Number of vehicles that passed the detector completely
     * over the lastNTimesteps.
     */
    double getNVehContributed( SUMOTime lastNTimesteps ) const;
    //@}

    /**
     * @name Inherited MSDetectorFileOutput methods.
     *
     * @see MSDetectorFileOutput
     */
    //@{
    /**
     * Returns a string indentifying an object of this class. Used for
     * distinct filenames in MSDetector2File
     *
     * @see MSDetector2File
     * @return String "MSInductLoop_" + idM
     */
    std::string getNamePrefix( void ) const;


    /**
     * Get a header for file output via MSDetector2File
     *
     * @return The static XML-Header which explains the output of getXMLOutput.
     *
     * @see MSDetector2File
     * @see getXMLOutput
     */
    void writeXMLHeader( XMLDevice &dev ) const;


    /**
     * Get the XML-formatted output of all the get*-methods except
     * getTimestepsSinceLastDetection.
     *
     * @param lastNTimesteps take data out of the interval
     * (now-lastNTimesteps, now].
     *
     * @return XML-formatted output of all the get*-methods except
     * getTimestepsSinceLastDetection.
     *
     * @see MSDetector2File
     */
    void writeXMLOutput( XMLDevice &dev,
        SUMOTime startTime, SUMOTime stopTime );

    /**
     * Get an opening XML-element containing information about the detector.
     *
     * @return <detector type="inductionloop" id="det_id"
     * lane="lane_id" pos="det_pos">
     *
     * @see MSDetector2File
     * @see getXMLDetectorInfoEnd
     */
    void writeXMLDetectorInfoStart( XMLDevice &dev ) const;


    /**
     * Get the closing XML-element to getXMLDetectorInfoStart
     *
     * @return </detector>
     *
     * @see MSDetector2File
     * @see getXMLDetectorInfoStart
     */
    void writeXMLDetectorInfoEnd( XMLDevice &dev ) const;

    /**
     * Get the data-clean up interval in timesteps.
     *
     */
    SUMOTime getDataCleanUpSteps( void ) const;
    //@}

    /**
     *
     *
     *
     * @return
     */
    virtual GUIDetectorWrapper* buildDetectorWrapper(
        GUIGlObjectStorage& idStorage,
        GUILaneWrapper& wrapper);

    /**
     * Struct to store the data of the counted vehicle
     * internally. These data is fed into a container on which the
     * get*-methods work.
     *
     * @see vehicleDataContM
     */
    struct VehicleData
    {
        /// Use this constructor if the vehicle has passed the induct loop completely
        VehicleData( MSVehicle& veh,
                     double entryTimestep,
                     double leaveTimestep )
            : lengthM( veh.length() ),
              entryTimeM( MSNet::getSeconds( entryTimestep ) ),
              leaveTimeM( MSNet::getSeconds( leaveTimestep ) ),
              speedM( lengthM / ( leaveTimeM - entryTimeM ) ),
              speedSquareM( speedM * speedM ),
              occupancyM( leaveTimeM - entryTimeM )
            {}

        double lengthM;         /**< Length of the vehicle. */
        double entryTimeM;      /**< Entry-time of the vehicle in [s]. */
        double leaveTimeM;      /**< Leave-time of the vehicle in [s]. */
        double speedM;          /**< Speed of the vehicle in [m/s]. */
        double speedSquareM;    /**< SpeedSquare of the vehicle in [m/(s^2)].*/
        double occupancyM;      /**< Occupancy of the detector in [s]. */
    };


    typedef std::vector<double> DismissedCont;

protected:
    /**
     * @name Methods called by Reminder methods.
     *
     * Methods in this group are called by the MSMoveReminder methods
     * only. They collect data to calculate the get* values.
     *
     *  @see isStillActive
     *  @see dismissByLaneChange
     *  @see isActivatedByEmitOrLaneChange
     */
    //@{
    /**
     * Introduces a vehicle to the detector's map vehiclesOnDetM.
     *
     * @param veh The entering vehicle.
     * @param entryTimestep Timestep (not neccessary integer) of entrance.
     */
    void enterDetectorByMove( MSVehicle& veh,
                              double entryTimestep );

    /**
     * Removes a vehicle from the detector's map vehiclesOnDetM and
     * adds the vehicle data to the internal vehicleDataContM.
     *
     * @param veh The leaving vehicle.
     * @param leaveTimestep Timestep (not neccessary integer) of leaving.
     */
    void leaveDetectorByMove( MSVehicle& veh,
                              double leaveTimestep );

    /**
     * Removes a vehicle from the detector's map vehiclesOnDetM.
     *
     * @param veh The leaving vehicle.
     */
    void leaveDetectorByLaneChange( MSVehicle& veh );
    //@}

    /**
     * Binary predicate that compares the passed VehicleData's
     * leaveTime to a fixed leaveTimeBound and returns true if the
     * passed value is lesser than the fixed bound. Used by
     * deleteOldData and getStartIterator.
     *
     */
    struct leaveTimeLesser :
            public std::binary_function< VehicleData, double, bool >
    {
        bool operator()( const VehicleData& data,
                         double leaveTimeBound ) const
        {
            return data.leaveTimeM < leaveTimeBound;
        }
    };

    /**
     * Deletes data from vehicleDataContM if deleteDataAfterStepsM
     * is over. Is called via MSEventControl.
     *
     * @return deleteDataAfterStepsM to recur the event.
     *
     * @see MSEventControl
     */
    SUMOTime deleteOldData( void );

    typedef std::deque< VehicleData > VehicleDataCont; /**< Type of
                                                        * vehicleDataContM. */

    /**
     * Get the iterator to vehicleDataContM that corresponds to the
     * first element with a leaveTime that is greater than now -
     * lastNTimesteps.
     *
     * @param lastNTimesteps Time-bound to search in is now - lastNTimesteps.
     *
     * @return Iterator to vehicleDataContM.
     */
    VehicleDataCont::const_iterator getStartIterator(
        SUMOTime lastNTimesteps ) const;

    DismissedCont::const_iterator getDismissedStartIterator(
        SUMOTime lastNTimesteps ) const;


    const double posM;          /**< Detector's position on lane [cells]. */

    SUMOTime deleteDataAfterStepsM; /**< Deletion interval. */

    double lastLeaveTimestepM;  /**< Leave-timestep of the last
                                 * vehicle detected. */

    typedef std::map< MSVehicle*, double > VehicleMap; /**< Type of
                                                        * vehiclesOnDetM. */

    VehicleMap vehiclesOnDetM;  /**< Map that holds the vehicles that
                                 * are currently on the detector. */

    DismissedCont dismissedContM;

    /**
     * Container that hold the data of the counted vehicles. It is
     * used to calculate the get* values. After
     * deleteDataAfterSecondsM this container is cleared partially.
     */
    VehicleDataCont vehicleDataContM;

    static std::string xmlHeaderM; /**< Contains a XML-comment that
                                    * explain the printed data. */

    static std::string xmlDetectorInfoEndM; /**< Closing tag for detector. */

private:

    /// Hidden default constructor.
    MSInductLoop();

    /// Hidden copy constructor.
    MSInductLoop( const MSInductLoop& );

    /// Hidden assignment operator.
    MSInductLoop& operator=( const MSInductLoop& );
};

namespace
{

    /**
     * @name Binary-functions to use with std::accumulate.
     *
     */
    //@{
    /// Adds up VehicleData::speedM
    inline double speedSum( double sumSoFar,
                            const MSInductLoop::VehicleData& data )
    {
        return sumSoFar + data.speedM;
    }

    /// Adds up VehicleData::speedSquareM
    inline double speedSquareSum( double sumSoFar,
                                  const MSInductLoop::VehicleData& data )
    {
        return sumSoFar + data.speedSquareM;
    }

    /// Adds up VehicleData::occupancyM
    inline double occupancySum( double sumSoFar,
                                const MSInductLoop::VehicleData& data )
    {
        return sumSoFar + data.occupancyM;
    }

    /// Adds up VehicleData::lengthM
    inline double lengthSum( double sumSoFar,
                             const MSInductLoop::VehicleData& data )
    {
        return sumSoFar + data.lengthM;
    }
    //@}
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

