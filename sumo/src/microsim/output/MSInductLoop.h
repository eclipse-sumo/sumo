/****************************************************************************/
/// @file    MSInductLoop.h
/// @author  Christian Roessel
/// @date    2004-11-23
/// @version $Id$
///
// An unextended detector measuring at a fixed position on a fixed lane.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSInductLoop_h
#define MSInductLoop_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <deque>
#include <map>
#include <functional>
#include <microsim/MSMoveReminder.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSNet.h>
#include <microsim/output/MSDetectorFileOutput.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/Named.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;
class GUIDetectorWrapper;
class GUIGlObjectStorage;
class GUILaneWrapper;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSInductLoop
 * @brief An unextended detector measuring at a fixed position on a fixed lane.
 *
 * Only vehicles that passed the entire detector are counted. We
 *  ignore vehicles that are emitted onto the detector and vehicles
 *  that change their lane while they are on the detector, because we
 *  cannot determine a meaningful enter/leave-times.
 *
 * This detector uses the MSMoveReminder mechanism, i.e. the vehicles
 *  call the detector if they pass it.
 *
 * Due to the inheritance from MSDetectorFileOutput this detector can
 *  print data to a file at fixed intervals via MSDetector2File.
 *
 * @see MSDetector2File
 * @see MSMoveReminder
 * @see MSDetectorFileOutput
 */
class MSInductLoop
        : public MSMoveReminder, public MSDetectorFileOutput,
            public MSVehicleQuitReminded, public Named {
public:
    /**
     * @brief Constructor.
     *
     * Adds reminder to MSLane.
     *
     * @param id Unique id.
     * @param lane Lane where detector woks on.
     * @param position Position of the detector within the lane.
     */
    MSInductLoop(const std::string& id,
                 MSLane * const lane,
                 SUMOReal positionInMeters) throw();


    /// @brief Destructor
    ~MSInductLoop() throw();


    /** @brief Resets all generated values to allow computation of next interval
     */
    void reset() throw();


    /// @name Methods inherited from MSMoveReminder
    /// @{

    /** @brief Checks whether the vehicle shall be counted and/or shall still touch this MSMoveReminder
     *
     * As soon a vehicle enters the detector, its entry time is computed and stored
     *  in myVehiclesOnDet via enterDetectorByMove. If it passes the detector, the
     *  according leaving time is computed and stored, too, using leaveDetectorByMove.
     *
     * @param[in] veh Vehicle that asks this remider.
     * @param[in] oldPos Position before move.
     * @param[in] newPos Position after move with newSpeed.
     * @param[in] newSpeed Moving speed.
     * @return True if vehicle hasn't passed the detector completely.
     * @see MSMoveReminder
     * @see MSMoveReminder::isStillActive
     * @see enterDetectorByMove
     * @see leaveDetectorByMove
     */
    bool isStillActive(MSVehicle& veh, SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed) throw();


    /** @brief Dismisses the vehicle if it is on the detector due to a lane change
     *
     * If the vehicle is on the detector, it will be dismissed by incrementing
     *  myDismissedVehicleNumber and removing this vehicle's entering time from
     *  myVehiclesOnDet.
     *
     * @param[in] veh The leaving vehicle.
     * @param[in] isArrival whether the vehicle arrived at its destination
     * @param[in] isLaneChange whether the vehicle changed from the lane
     * @see leaveDetectorByLaneChange
     * @see MSMoveReminder
     * @see MSMoveReminder::notifyLeave
     */
    void notifyLeave(MSVehicle& veh, bool isArrival, bool isLaneChange) throw();


    /** @brief Returns whether the detector may has to be concerned during the vehicle's further movement
     *
     * If the detector is in front of the vehicle, true is returned. If
     *  the vehicle's front has passed the detector, false, because
     *  the vehicle is no longer relevant for the detector.
     *
     * @param[in] veh The entering vehicle.
     * @param[in] isEmit whether the vehicle was just emitted into the net
     * @param[in] isLaneChange whether the vehicle changed to the lane
     * @return True if vehicle is on or in front of the detector.
     * @see MSMoveReminder
     * @see MSMoveReminder::notifyEnter
     */
    bool notifyEnter(MSVehicle& veh, bool isEmit, bool isLaneChange) throw();
    //@}



    /// @name Methods returning current values
    /// @{

    /** @brief Returns the speed of the vehicle on the detector
     *
     * If no vehicle is on the detector, -1 is returned, otherwise
     *  this vehicle's current speed.
     *
     * @return The speed [m/s] of the vehicle if one is on the detector, -1 otherwise
     */
    SUMOReal getCurrentSpeed() const throw();


    /** @brief Returns the length of the vehicle on the detector
     *
     * If no vehicle is on the detector, -1 is returned, otherwise
     *  this vehicle's length.
     *
     * @return The length [m] of the vehicle if one is on the detector, -1 otherwise
     */
    SUMOReal getCurrentLength() const throw();


    /** @brief Returns the current occupancy
     *
     * If a vehicle is on the detector, 1 is returned. If a vehicle has passed the detector
     *  in this timestep, its occupancy value is returned. If no vehicle has passed,
     *  0 is returned.
     *
     * @return This detector's current occupancy
     * @todo recheck (especially if more than one vehicle has passed)
     */
    SUMOReal getCurrentOccupancy() const throw();


    /** @brief Returns the number of vehicles that have passed the detector
     *
     * If a vehicle is on the detector, 1 is returned. If a vehicle has passed the detector
     *  in this timestep, 1 is returned. If no vehicle has passed,
     *  0 is returned.
     *
     * @return The number of vehicles that have passed the detector
     * @todo recheck (especially if more than one vehicle has passed)
     */
    SUMOReal getCurrentPassedNumber() const throw();


    /** @brief Returns the ids of vehicles that have passed the detector
     *
     * @return The ids of vehicles that have passed the detector
     * @todo recheck (especially if more than one vehicle has passed)
     */
    std::vector<std::string> getCurrentVehicleIDs() const throw();


    /** @brief Returns the time since the last vehicle left the detector
     *
     * @return Timesteps from last leaving (detection) of the detector
     */
    SUMOReal getTimestepsSinceLastDetection() const throw();
    //@}



    /// @name Methods returning aggregated values
    /// @{

    unsigned getNVehContributed() const throw();
    //@}



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


    /** @brief Opens the XML-output using "detector" as root element
     *
     * @param[in] dev The output device to write the root into
     * @see MSDetectorFileOutput::writeXMLDetectorProlog
     * @exception IOError If an error on writing occures (!!! not yet implemented)
     */
    void writeXMLDetectorProlog(OutputDevice &dev) const throw(IOError);
    /// @}



    /// @name Methods inherited from MSVehicleQuitReminded.
    /// @{

    /** @brief Removes the information that the vehicle is on the detector
     *
     * @param[in] veh The vehicle that was on the detector and leaves the simuation
     */
    void removeOnTripEnd(MSVehicle *veh) throw();
    /// @}


protected:
    /// @name Methods that add and remove vehicles from internal container
    /// @{

    /** @brief Introduces a vehicle to the detector's map myVehiclesOnDet.
     * @param veh The entering vehicle.
     * @param entryTimestep Timestep (not necessary integer) of entrance.
     */
    void enterDetectorByMove(MSVehicle& veh, SUMOReal entryTimestep) throw();


    /** @brief Processes a vehicle that leaves the detector
     *
     * Removes a vehicle from the detector's map myVehiclesOnDet and
     * adds the vehicle data to the internal myVehicleDataCont.
     *
     * @param veh The leaving vehicle.
     * @param leaveTimestep Timestep (not necessary integer) of leaving.
     */
    void leaveDetectorByMove(MSVehicle& veh, SUMOReal leaveTimestep) throw();


    /** @brief Removes a vehicle from the detector's map myVehiclesOnDet.
     * @param veh The leaving vehicle.
     */
    void leaveDetectorByLaneChange(MSVehicle& veh) throw();
    /// @}


protected:
    /** @brief Struct to store the data of the counted vehicle internally.
     *
     * These data is fed into a container.
     *
     * @see myVehicleDataCont
     */
    struct VehicleData {
        /** @brief Constructor
         *
         * Used if the vehicle has passed the induct loop completely
         *
         * @param[in] vehLength The length of the vehicle
         * @param[in] entryTimestep The time at which the vehicle entered the detector
         * @param[in] leaveTimestep The time at which the vehicle left the detector
         */
        VehicleData(const std::string &id, SUMOReal vehLength, SUMOReal entryTimestep, SUMOReal leaveTimestep) throw()
                : idM(id), lengthM(vehLength), entryTimeM(entryTimestep), leaveTimeM(leaveTimestep),
                speedM(lengthM / (leaveTimeM - entryTimeM)),
                occupancyM(leaveTimeM - entryTimeM) {}

        /** @brief The id of the vehicle */
        std::string idM;
        /** @brief Length of the vehicle. */
        SUMOReal lengthM;
        /** @brief Entry-time of the vehicle in [s]. */
        SUMOReal entryTimeM;
        /** @brief Leave-time of the vehicle in [s]. */
        SUMOReal leaveTimeM;
        /** @brief Speed of the vehicle in [m/s]. */
        SUMOReal speedM;
        /** @brief Occupancy of the detector in [s]. */
        SUMOReal occupancyM;
    };


protected:
    /// @name Function for summing up values
    ///@{

    /// @brief Adds up VehicleData::speedM
    static inline SUMOReal speedSum(SUMOReal sumSoFar, const MSInductLoop::VehicleData& data) throw() {
        return sumSoFar + data.speedM;
    }

    /// @brief Adds up VehicleData::occupancyM
    static inline SUMOReal occupancySum(SUMOReal sumSoFar, const MSInductLoop::VehicleData& data) throw() {
        return sumSoFar + data.occupancyM;
    }

    /// @brief Adds up VehicleData::lengthM
    static inline SUMOReal lengthSum(SUMOReal sumSoFar, const MSInductLoop::VehicleData& data) throw() {
        return sumSoFar + data.lengthM;
    }
    ///@}


    /** @brief Returns vehicle data for vehicles that have been on the detector starting at the given time
     *
     *
     *
     *
     * @param[in] t The time from which vehicles shall be counted
     * @return The list of vehicles
     */
    std::vector<VehicleData> collectVehiclesOnDet(SUMOTime t) const throw();


protected:
    /// @brief The vehicle that is currently on the detector
    MSVehicle *myCurrentVehicle;

    /// @brief Detector's position on lane [m]
    const SUMOReal myPosition;

    /// @brief Leave-timestep of the last vehicle detected.
    SUMOReal myLastLeaveTimestep;

    /// @brief Occupancy by the last vehicle detected.
    SUMOReal myLastOccupancy;

    /// @brief The number of dismissed vehicles
    unsigned myDismissedVehicleNumber;


    /// @brief Type of myVehicleDataCont.
    typedef std::deque< VehicleData > VehicleDataCont;

    /// @brief Data of vehicles that have completely passed the detector
    VehicleDataCont myVehicleDataCont;

    /// @brief Data of vehicles that have completely passed the detector in the last time interval
    VehicleDataCont myLastVehicleDataCont;


    /// @brief Type of myVehiclesOnDet
    typedef std::map< MSVehicle*, SUMOReal > VehicleMap;

    /// @brief Data for vehicles that have entered the detector (vehicle -> enter time)
    VehicleMap myVehiclesOnDet;


private:
    /// @brief Invalidated copy constructor.
    MSInductLoop(const MSInductLoop&);

    /// @brief Invalidated assignment operator.
    MSInductLoop& operator=(const MSInductLoop&);


};


#endif

/****************************************************************************/

