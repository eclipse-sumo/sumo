/****************************************************************************/
/// @file    MSE3Collector.h
/// @author  Christian Roessel
/// @date    Tue Dec 02 2003 22:17 CET
/// @version $Id$
///
// A detector of vehicles passing an area between entry/exit points
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSE3Collector_h
#define MSE3Collector_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/PhysicalTypeDefs.h>
#include <microsim/MSMoveReminder.h>
#include <microsim/output/MSDetectorFileOutput.h>
#include <microsim/output/MSCrossSection.h>
#include <string>
#include <cassert>
#include <vector>
#include <limits>
#include <microsim/MSVehicleQuitReminded.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSE3Collector
 * @brief A detector of vehicles passing an area between entry/exit points
 *
 * E3-detectors are defined by a set of in-cross-sections and out-cross-sections.
 * Vehicles, that pass an in- and out-cross-section are detected when they pass the
 *  out-cross-section. Vehicles passing the out-cross-section without having
 *  passed the in-cross-section are not detected.
 */
class MSE3Collector : public MSDetectorFileOutput,
            public MSVehicleQuitReminded {
public:
    /**
     * @class MSE3EntryReminder
     * @brief A place on the road net (at a certain lane and position on it) where the E3-area begins
     */
    class MSE3EntryReminder : public MSMoveReminder {
    public:
        /** @brief Constructor
         *
         * @param[in] crossSection The position at which the entry lies
         * @param[in] collector The detector the entry belongs to
         */
        MSE3EntryReminder(const MSCrossSection &crossSection, MSE3Collector& collector) throw();


        /// @name Methods inherited from MSMoveReminder.
        /// @{

        /** @brief Checks whether the vehicle enters
         *
         * As soon as the reported vehicle enters the detector area (position>myPosition)
         *  the entering time is computed and both are added to the parent detector using
         *  "enter".
         *
         * @param[in] veh The vehicle in question.
         * @param[in] oldPos Position before the move-micro-timestep.
         * @param[in] newPos Position after the move-micro-timestep.
         * @param[in] newSpeed Unused here.
         * @return False, if vehicle passed the detector entierly, else true.
         * @see MSMoveReminder
         * @see MSMoveReminder::isStillActive
         * @see MSE3Collector::enter
         */
        bool isStillActive(MSVehicle& veh, SUMOReal , SUMOReal newPos, SUMOReal) throw();


        /** @brief Returns whether the vehicle shall be aware of this entry
         *
         * Returns true if the vehicle is in front of the entry, so that it
         *  may enter it in later steps.
         *
         * @param[in] veh The vehicle that enters the lane
         * @param[in] isEmit whether the vehicle was just emitted into the net
         * @param[in] isLaneChange whether the vehicle changed to the lane
         * @see MSMoveReminder::notifyEnter
         * @return False, if vehicle passed the entry, else true.
         */
        bool notifyEnter(MSVehicle& veh, bool isEmit, bool isLaneChange) throw();
        /// @}


    private:
        /// @brief The parent collector
        MSE3Collector& myCollector;

        /// @brief The position on the lane
        SUMOReal myPosition;

    private:
        /// @brief Invalidated copy constructor.
        MSE3EntryReminder(const MSE3EntryReminder&);

        /// @brief Invalidated assignment operator.
        MSE3EntryReminder& operator=(const MSE3EntryReminder&);

    };



    /**
     * @class MSE3LeaveReminder
     * @brief A place on the road net (at a certain lane and position on it) where the E3-area ends
     */
    class MSE3LeaveReminder : public MSMoveReminder {
    public:
        /** @brief Constructor
         *
         * @param[in] crossSection The position at which the exit lies
         * @param[in] collector The detector the exit belongs to
         */
        MSE3LeaveReminder(const MSCrossSection &crossSection, MSE3Collector& collector) throw();


        /// @name methods from MSMoveReminder
        //@{

        /** @brief Checks whether the vehicle leaves
         *
         * As soon as the reported vehicle leaves the detector area (position-length>myPosition)
         *  the leaving time is computed and both are made known to the parent detector using
         *  "leave".
         *
         * @param[in] veh The vehicle in question.
         * @param[in] oldPos Position before the move-micro-timestep.
         * @param[in] newPos Position after the move-micro-timestep.
         * @param[in] newSpeed Unused here.
         * @return False, if vehicle passed the detector entierly, else true.
         * @see MSMoveReminder
         * @see MSMoveReminder::isStillActive
         * @see MSE3Collector::leave
         */
        bool isStillActive(MSVehicle& veh, SUMOReal , SUMOReal newPos, SUMOReal) throw();


        /** @brief Returns whether the vehicle shall be aware of this entry
         *
         * Returns true if the vehicle is in front of the exit, so that it
         *  may enter it in later steps.
         *
         * @param[in] veh The vehicle that enters the lane
         * @param[in] isEmit whether the vehicle was just emitted into the net
         * @param[in] isLaneChange whether the vehicle changed to the lane
         * @see MSMoveReminder::notifyEnter
         * @return False, if vehicle passed the exit completely, else true.
         */
        bool notifyEnter(MSVehicle& veh, bool isEmit, bool isLaneChange) throw();
        //@}


    private:
        /// @brief The parent collector
        MSE3Collector& myCollector;

        /// @brief The position on the lane
        SUMOReal myPosition;

    private:
        /// @brief Invalidated copy constructor.
        MSE3LeaveReminder(const MSE3LeaveReminder&);

        /// @brief Invalidated assignment operator.
        MSE3LeaveReminder& operator=(const MSE3LeaveReminder&);

    };


    /** @brief Constructor
     *
     * Sets reminder objects on entry- and leave-lanes
     *
     *  @param[in] id The detector's unique id.
     *  @param[in] entries Entry-cross-sections.
     *  @param[in] exits Leavey-cross-sections.
     *  @param[in] haltingSpeedThreshold A vehicle must not drive a greater speed than haltingSpeedThreshold to be a "halting" vehicle.
     *  @param[in] haltingTimeThreshold A vehicle must not drive a greater speed for more than haltingTimeThreshold to be a "halting" vehicle.
     */
    MSE3Collector(const std::string &id,
                  const CrossSectionVector &entries, const CrossSectionVector &exits,
                  MetersPerSecond haltingSpeedThreshold,
                  SUMOTime haltingTimeThreshold) throw();


    /// @brief Destructor
    virtual ~MSE3Collector() throw();


    /** @brief Resets all generated values to allow computation of next interval
     */
    void reset() throw();


    /** @brief Called if a vehicle touches an entry-cross-section.
     *
     * Inserts vehicle into internal containers.
     *
     *  @param[in] veh The vehicle that entered the area
     *  @param[in] entryTimestep The time step the vehicle entered the area
     */
    void enter(MSVehicle& veh, SUMOReal entryTimestep) throw();


    /** @brief Called if a vehicle passes a leave-cross-section.
     *
     * Removes vehicle from internal containers.
     *
     *  @param[in] veh The vehicle that left the area
     *  @param[in] entryTimestep The time step the vehicle left the area
     */
    void leave(MSVehicle& veh, SUMOReal leaveTimestep) throw();


    /** @brief Returns the id of the detector
     * @return The id of the detector
     */
    const std::string& getID() const throw();



    /// @name Methods inherited from MSVehicleQuitReminded.
    /// @{

    /** @brief Removes a vehicle which entered the area but quitted before leaving it
     *
     * Remove vehicles that entered the detector but reached their destination before
     *  passing the leave-cross-section from internal containers.
     *
     * @param[in] veh The vehicle to remove
     */
    void removeOnTripEnd(MSVehicle *veh) throw();
    /// @}



    /// @name Methods returning current values
    /// @{

    /** @brief Returns the mean speed within the area
     *
     * If no vehicle is within the area, -1 is returned.
     *
     * @return The mean speed [m/s] of all vehicles within the area, -1 if there is none
     */
    SUMOReal getCurrentMeanSpeed() const throw();


    /** @brief Returns the number of current haltings within the area
     *
     * If no vehicle is within the area, 0 is returned.
     *
     * @return The mean number of haltings within the area
     */
    SUMOReal getCurrentHaltingNumber() const throw();


    /** @brief Returns the number of vehicles within the area
     * @return The number of vehicles that passed the entry collector
     */
    SUMOReal getVehiclesWithin() const throw();


    /** @brief Returns the number of vehicles within the area
     *
     * @return The number of vehicles that have passed the entry, but not yet an exit point
     */
    std::vector<std::string> getCurrentVehicleIDs() const throw();
    /// @}


    /// @name Methods inherited from MSDetectorFileOutput.
    /// @{

    /** @brief Writes collected values into the given stream
     *
     * @param[in] dev The output device to write the data into
     * @param[in] startTime First time step the data were gathered
     * @param[in] stopTime Last time step the data were gathered
     * @see MSDetectorFileOutput::writeXMLOutput
     * @exception IOError If an error on writing occures (!!! not yet implemented)
     */
    void writeXMLOutput(OutputDevice &dev, SUMOTime startTime, SUMOTime stopTime) throw(IOError);


    /** @brief Opens the XML-output using "e3-detector" as root element
     *
     * The lists of entries/exists are written, too.
     *
     * @param[in] dev The output device to write the root into
     * @see MSDetectorFileOutput::writeXMLDetectorProlog
     * @exception IOError If an error on writing occures (!!! not yet implemented)
     */
    void writeXMLDetectorProlog(OutputDevice &dev) const throw(IOError);
    /// @}



    /** @brief Computes the detector values in each time step
     *
     * This method should be called at the end of a simulation step, when
     *  all vehicles have moved. The current values are computed and
     *  summed up with the previous.
     *
     * @param[in] currentTime The current simulation time (unused)
     */
    void update(SUMOTime currentTime) throw();


protected:
    /// @brief The detector's id
    std::string myID;

    /// @brief The detector's entries
    CrossSectionVector myEntries;

    /// @brief The detector's exits
    CrossSectionVector myExits;

    /// @brief The detector's built entry reminder
    std::vector<MSE3EntryReminder*> myEntryReminders;

    /// @brief The detector's built exit reminder
    std::vector<MSE3LeaveReminder*> myLeaveReminders;


    // @brief Time-threshold to determine if a vehicle is halting.
    SUMOTime myHaltingTimeThreshold;

    /// @brief Speed-threshold to determine if a vehicle is halting.
    MetersPerSecond myHaltingSpeedThreshold;

    /**
     * @struct E3Values
     * @brief Internal storage for values from a vehicle
     *
     * For each vehicle within the area (that entered through an entry point),
     *  this structure is allocated. All values gathered from the vehicle are aggregated
     *  within this structure.
     */
    struct E3Values {
        /// @brief The vehicle's entry time
        SUMOReal entryTime;
        /// @brief The vehicle's leaving time
        SUMOReal leaveTime;
        /// @brief The sum of registered speeds the vehicle has/had inside the area
        SUMOReal speedSum;
        /// @brief The sum of haltings the vehicle has/had within the area
        unsigned haltings;
        /// @brief Number of times the vehicle was touched when being within the container
        unsigned samples;
        /// @brief Begin time of last halt begin
        SUMOReal haltingBegin;
        /// @brief The sum of registered speeds the vehicle has/had inside the area during the current interval
        SUMOReal intervalSpeedSum;
        /// @brief The sum of haltings the vehicle has/had within the area during the current interval
        unsigned intervalHaltings;
        /// @brief An internal information whether the update step was performed
        bool hadUpdate;
    };

    /// @brief Container for vehicles that have entered the area
    std::map<MSVehicle*, E3Values> myEnteredContainer;

    /// @brief Container for vehicles that have left the area
    std::map<MSVehicle*, E3Values> myLeftContainer;


    /// @name Storages for current values
    /// @{

    /// @brief The current mean speed of known vehicles (inside)
    SUMOReal myCurrentMeanSpeed;

    /// @brief The current number of haltings (inside)
    SUMOReal myCurrentHaltingsNumber;

    /** @brief The current number of vehicles inside;
     *
     * Please note, that vehicles that enter the area are given as a fraction
     * @see execute
     */
    SUMOReal myCurrentTouchedVehicles;
    /// @}


    /// @brief Information when the last reset has been done
    SUMOTime myLastResetTime;


private:
    /// @brief Invalidated copy constructor.
    MSE3Collector(const MSE3Collector&);

    /// @brief Invalidated assignment operator.
    MSE3Collector& operator=(const MSE3Collector&);


};


#endif

/****************************************************************************/

