/****************************************************************************/
/// @file    MSE2Collector.h
/// @author  Christian Roessel
/// @date    Tue Dec 02 2003 22:13 CET
/// @version $Id$
///
// An areal (along a single lane) detector
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSE2Collector_h
#define MSE2Collector_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <microsim/MSMoveReminder.h>

#include <microsim/MSLane.h>
#include <microsim/output/MSDetectorFileOutput.h>
#include <utils/common/ToString.h>
#include <string>
#include <cassert>
#include <vector>
#include <limits>
#include <set>
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/Named.h>
#include <microsim/MSEventControl.h>
#include <utils/common/Command.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSE2Collector
 * @brief An areal (along a single lane) detector
 *
 * This detector traces vehicles which are at a part of a single lane. A
 *  vehicle that enters the detector is stored and the stored vehicles' speeds
 *  are used within each timestep to compute the detector values. As soon as the
 *  vehicle leaves the detector, it is no longer tracked.
 *
 * Determining entering and leaving vehicles is done via the MSMoveReminder
 *  interface. The values are computed by an event-callback (at the end of
 *  a time step).
 *
 * Jams are determined as following: A vehicle must at least drive haltingTimeThreshold
 *  at a speed lesser than haltingSpeedThreshold to be a "halting" vehicle.
 *  Two or more vehicles are "in jam" if they are halting and their distance is
 *  lesser than jamDistThreshold.
 *
 * @see Named
 * @see MSMoveReminder
 * @see MSDetectorFileOutput
 * @see Command
 */
class MSE2Collector : public Named, public MSMoveReminder, public MSDetectorFileOutput, public Command
{
public:
    /** @brief Constructor
     *
     * @param[in] id The detector's unique id.
     * @param[in] usage Information how the detector is used
     * @param[in] lane The lane to place the detector at
     * @param[in] startPos Begin position of the detector
     * @param[in] detLength Length of the detector
     * @param[in] haltingTimeThreshold The time a vehicle's speed must be below haltingSpeedThreshold to be assigned as jammed
     * @param[in] haltingSpeedThreshold The speed a vehicle's speed must be below to be assigned as jammed
     * @param[in] jamDistThreshold The distance between two vehicles in order to not count them to one jam
     * @todo The lane should not be given as a pointer
     */
    MSE2Collector(const std::string &id, DetectorUsage usage,
                  MSLane* lane, SUMOReal startPos, SUMOReal detLength,
                  SUMOTime haltingTimeThreshold, SUMOReal haltingSpeedThreshold,
                  SUMOReal jamDistThreshold);


    /** @brief Destructor
     */
    virtual ~MSE2Collector();


    /** @brief Returns the detector's usage type
     *
     * @see DetectorUsage
     * @return How the detector is used.
     */
    virtual DetectorUsage getUsageType() const {
        return myUsage;
    }



    /// @name Methods inherited from MSMoveReminder.
    /// @{
    /** @brief Adds/removes vehicles from the list of vehicles to regard
     *
     * As soon as the reported vehicle enters the detector area (position>myStartPos)
     *  it is added to the list of vehicles to regard (myKnownVehicles). It
     *  is removed from this list if it leaves the detector (position<lengt>myEndPos).
     * The method returns true as long as the vehicle is not beyond the detector.
     *
     * @param[in] veh The vehicle in question.
     * @param[in] oldPos Position before the move-micro-timestep.
     * @param[in] newPos Position after the move-micro-timestep.
     * @param[in] newSpeed Unused here.
     * @return False, if vehicle passed the detector entierly, else true.
     * @see MSMoveReminder
     * @see MSMoveReminder::isStillActive
     */
    bool isStillActive(MSVehicle& veh, SUMOReal oldPos, SUMOReal newPos,
                       SUMOReal newSpeed);


    /** @brief Removes a known vehicle due to its lane-change
     *
     * If the reported vehicle is known, it is removed from the list of
     *  vehicles to regard (myKnownVehicles).
     *
     * @param[in] veh The leaving vehicle.
     * @see MSMoveReminder::dismissByLaneChange
     */
    void dismissByLaneChange(MSVehicle& veh);


    /** @brief Adds the vehicle to known vehicles if not beyond the dector
     *
     * If the vehicles is within the detector are, it is added to the list
     *  of known vehicles.
     * The method returns true as long as the vehicle is not beyond the detector.
     *
     * @param[in] veh The vehicle that enters the lane
     * @see MSMoveReminder::isActivatedByEmitOrLaneChange
     * @return False, if vehicle passed the detector entierly, else true.
     */
    bool isActivatedByEmitOrLaneChange(MSVehicle& veh);
    /// @}



    /// @name Methods inherited from Command.
    /// @{
    /** @brief Computes the detector values in each time step
     *
     * This method should be called at the end of a simulation step, when
     *  all vehicles have moved. The current values are computed and
     *  summed up with the previous.
     *
     * @param[in] veh The current simulation time (unused)
     * @return Always 1, this method must be recalled every time step
     * @see Command::execute
     */
    SUMOTime execute(SUMOTime currentTime);
    /// @}



    /// @name Methods inherited from MSDetectorFileOutput.
    /// @{
    /** @brief Writes collected values into the given stream
     *
     * @param[in] dev The output device to write the data into
     * @param[in] startTime First time step the data were gathered
     * @param[in] stopTime Last time step the data were gathered
     * @see MSDetectorFileOutput::writeXMLOutput
     */
    void writeXMLOutput(OutputDevice &dev, SUMOTime startTime, SUMOTime stopTime);


    /** @brief Opens the XML-output using "detector" as root element
     *
     * @param[in] dev The output device to write the root into
     * @see MSDetectorFileOutput::writeXMLDetectorProlog
     */
    void writeXMLDetectorProlog(OutputDevice &dev) const;
    /// @}



    /** @brief Returns the begin position of the detector
     *
     * @return The detector's begin position
     */
    SUMOReal getStartPos() const {
        return myStartPos;
    }


    /** @brief Returns the end position of the detector
     *
     * @return The detector's end position
     */
    SUMOReal getEndPos() const {
        return myEndPos;
    }


    /** @brief Resets all values
     *
     * This method is called on initialisation and as soon as the values
     *  were written. Values for the next interval may be collected, then.
     * The list of known vehicles stays untouched.
     */
    void reset();


    /// @name Methods returning current values
    /// @{
    /** @brief Returns the number of vehicles currently on the detector */
    unsigned getCurrentVehicleNumber() const;

    /** @brief Returns the curent detector occupancy */
    SUMOReal getCurrentOccupancy() const;

    /** @brief Returns the mean vehicle speed of vehicles currently on the detector*/
    SUMOReal getCurrentMeanSpeed() const;

    /** @brief Returns the mean vehicle length of vehicles currently on the detector*/
    SUMOReal getCurrentMeanLength() const;

    /** @brief Returns the current number of jams */
    unsigned getCurrentJamNumber() const;

    /** @brief Returns the length in vehicles of the currently largest jam */
    unsigned getCurrentMaxJamLengthInVehicles() const;

    /** @brief Returns the length in meters of the currently largest jam */
    SUMOReal getCurrentMaxJamLengthInMeters() const;

    /** @brief Returns the length of all jams in vehicles */
    unsigned getCurrentJamLengthInVehicles() const;

    /** @brief Returns the length of all jams in meters */
    SUMOReal getCurrentJamLengthInMeters() const;

    /** @brief Returns the length of all jams in meters */
    unsigned getCurrentStartedHalts() const;
    /// @}

protected:
    /** @brief Internal representation of a jam
     *
     * Used in execute, instances of this structure are used to track
     *  begin and end positions (as vehicles) of a jam.
     */
    struct JamInfo {
        /// @brief The first standing vehicle
        std::list<MSVehicle*>::const_iterator firstStandingVehicle;

        /// @brief The last standing vehicle
        std::list<MSVehicle*>::const_iterator lastStandingVehicle;
    };


    /** @brief A class used to sort known vehicles by their position
     *
     * Sorting is needed, because the order may change if a vehicle has
     *  entered the lane by lane changing.
     */
    class by_vehicle_position_sorter
    {
    public:
        /// @brief constructor
        explicit by_vehicle_position_sorter() { }

        /** @brief Comparison funtcion
         *
         * @param[in] v1 First vehicle to compare
         * @param[in] v2 Second vehicle to compare
         * @return Whether the position of the first vehicles is smaller than the one of the second
         */
        int operator()(const MSVehicle *v1, const MSVehicle *v2) {
            return v1->getPositionOnLane()<v2->getPositionOnLane();
        }
    };


private:
    /// @name Detector parameter
    /// @{
    /// @brief A vehicle must driver slower than this to be counted as a part of a jam
    SUMOReal myJamHaltingSpeedThreshold;
    /// @brief A vehicle must be that long beyond myJamHaltingSpeedThreshold to be counted as a part of a jam
    SUMOTime myJamHaltingTimeThreshold;
    /// @brief Two standing vehicles must be closer than this to be counted into the same jam
    SUMOReal myJamDistanceThreshold;
    /// @brief The position the detector starts at
    SUMOReal myStartPos;
    /// @brief The position the detector ends at
    SUMOReal myEndPos;
    /// @}

    /// @brief Information about how this detector is used
    DetectorUsage myUsage;

    /// @brief List of known vehicles
    std::list<MSVehicle*> myKnownVehicles;

    /// @brief Storage for halting durations of known vehicles (for halting vehicles)
    std::map<MSVehicle*, SUMOTime> myHaltingVehicleDurations;


    /// @name Values generated for aggregated file output
    /// @{
    /// @brief The sum of collected vehicle speeds [m/s]
    SUMOReal mySpeedSum;
    /// @brief The maximum halting duration so far [s]
    SUMOTime myMaxHaltingDuration;
    /// @brief The number of started halts [#]
    SUMOReal myStartedHalts;
    /// @brief The sum of jam lengths [m]
    SUMOReal myJamLengthInMetersSum;
    /// @brief The sum of jam lengths [#veh]
    unsigned myJamLengthInVehiclesSum;
    /// @brief The number of collected samples [#]
    unsigned myVehicleSamples;
    /// @brief The current aggregation duration [#steps]
    unsigned myTimeSamples;
    /// @brief The sum of occupancies [%]
    SUMOReal myOccupancySum;
    /// @brief The maximum occupancy [%]
    SUMOReal myMaxOccupancy;
    /// @brief The sum of halting durations
    SUMOTime myHaltingDurationSum;
    /// @brief The mean jam length [#veh]
    unsigned myMeanMaxJamInVehicles;
    /// @brief The mean jam length [m]
    SUMOReal myMeanMaxJamInMeters;
    /// @brief The max jam length [#veh]
    unsigned myMaxJamInVehicles;
    /// @brief The max jam length [m]
    SUMOReal myMaxJamInMeters;
    /// @brief The sum of jam length [#veh]
    unsigned myJamInVehiclesSum;
    /// @brief The sum of jam length [m]
    SUMOReal myJamInMetersSum;
    /// @}

    /// @name Values generated describing the current state
    /// @{
    /// @brief The current occupancy
    SUMOReal myCurrentOccupancy;
    /// @brief The current mean speed 
    SUMOReal myCurrentMeanSpeed;
    /// @brief The current mean length
    SUMOReal myCurrentMeanLength;
    /// @brief The current jam number
    unsigned myCurrentJamNo;
    /// @brief the current maximum jam length in meters
    SUMOReal myCurrentMaxJamLengthInMeters;
    /// @brief The current maximum jam length in vehicles
    unsigned myCurrentMaxJamLengthInVehicles;
    /// @brief The overall jam length in meters
    SUMOReal myCurrentJamLengthInMeters;
    /// @brief The everall jam length in vehicles
    unsigned myCurrentJamLengthInVehicles;
    /// @brief The number of started halts in the last step
    unsigned myCurrentStartedHalts;
    /// @}

};


#endif

/****************************************************************************/

