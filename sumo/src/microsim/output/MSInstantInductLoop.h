/****************************************************************************/
/// @file    MSInstantInductLoop.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2011-09.08
/// @version $Id$
///
// An instantaneous induction loop
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2011-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSInstantInductLoop_h
#define MSInstantInductLoop_h


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
#include <microsim/output/MSDetectorFileOutput.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;
class MSVehicle;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSInstantInductLoop
 * @brief An instantaneous induction loop
 *
 * @see MSMoveReminder
 * @see MSDetectorFileOutput
 */
class MSInstantInductLoop
        : public MSMoveReminder, public MSDetectorFileOutput {
public:
    /**
     * @brief Constructor.
     *
     * @param[in] id Unique id
     * @param[in] od The device to write to
     * @param[in] lane Lane where detector woks on.
     * @param[in] position Position of the detector within the lane.
     */
    MSInstantInductLoop(const std::string& id, OutputDevice& od,
                        MSLane* const lane, SUMOReal positionInMeters);


    /// @brief Destructor
    ~MSInstantInductLoop();



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
     * @see MSMoveReminder::notifyMove
     * @see enterDetectorByMove
     * @see leaveDetectorByMove
     */
    bool notifyMove(SUMOVehicle& veh, SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed);


    /** @brief Dismisses the vehicle if it is on the detector due to a lane change
     *
     * If the vehicle is on the detector, it will be dismissed by incrementing
     *  myDismissedVehicleNumber and removing this vehicle's entering time from
     *  myVehiclesOnDet.
     *
     * @param[in] veh The leaving vehicle.
     * @param[in] lastPos Position on the lane when leaving.
     * @param[in] isArrival whether the vehicle arrived at its destination
     * @param[in] isLaneChange whether the vehicle changed from the lane
     * @see leaveDetectorByLaneChange
     * @see MSMoveReminder
     * @see MSMoveReminder::notifyLeave
     */
    bool notifyLeave(SUMOVehicle& veh, SUMOReal lastPos, MSMoveReminder::Notification reason);
    //@}



    /** @brief Write the generated output to the given device
     *
     * This method is not used - output is written as soon as a vehicle visits the detector.
     *
     * @param[in] dev The output device to write the data into
     * @param[in] startTime First time step the data were gathered
     * @param[in] stopTime Last time step the data were gathered
     * @exception IOError If an error on writing occurs
     */
    void writeXMLOutput(OutputDevice& dev,
                        SUMOTime startTime, SUMOTime stopTime) {
        UNUSED_PARAMETER(dev);
        UNUSED_PARAMETER(startTime);
        UNUSED_PARAMETER(stopTime);
    }


    /** @brief Open the XML-output
     *
     * The implementing function should open an xml element using
     *  OutputDevice::writeXMLHeader.
     *
     * @param[in] dev The output device to write the root into
     * @exception IOError If an error on writing occurs
     */
    void writeXMLDetectorProlog(OutputDevice& dev) const;


protected:
    /** @brief Writes an event line
     * @param[in] state The current state to report
     * @param[in] t The event time
     * @param[in] veh The vehicle responsible for the event
     * @param[in] speed The speed of the vehicle
     * @param[in] add An optional attribute to report
     * @param[in] addValue The value of the optional attribute
     */
    void write(const char* state, SUMOReal t, SUMOVehicle& veh, SUMOReal speed, const char* add = 0, SUMOReal addValue = -1);


protected:
    /// @brief The output device to use
    OutputDevice& myOutputDevice;

    /// @brief Detector's position on lane [m]
    const SUMOReal myPosition;

    /// @brief The last exit time
    SUMOReal myLastExitTime;

    /// @brief The last exit time
    std::map<SUMOVehicle*, SUMOReal> myEntryTimes;

private:
    /// @brief Invalidated copy constructor.
    MSInstantInductLoop(const MSInstantInductLoop&);

    /// @brief Invalidated assignment operator.
    MSInstantInductLoop& operator=(const MSInstantInductLoop&);


};


#endif

/****************************************************************************/

