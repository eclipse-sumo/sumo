/****************************************************************************/
/// @file    MSInstantInductLoop.h
/// @author  Daniel Krajzewicz
/// @date    2011-09.08
/// @version $Id: MSInstantInductLoop.h 9715 2011-02-10 14:03:15Z dkrajzew $
///
// An unextended detector measuring at a fixed position on a fixed lane.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
class MSInstantInductLoop
        : public MSMoveReminder, public MSDetectorFileOutput {
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
    MSInstantInductLoop(const std::string& id, OutputDevice &od,
                 MSLane * const lane,
                 SUMOReal positionInMeters) throw();


    /// @brief Destructor
    ~MSInstantInductLoop() throw();



    void write(char *state, SUMOReal t, SUMOVehicle& veh, SUMOReal speed);


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
    bool notifyMove(SUMOVehicle& veh, SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed) throw();


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
    bool notifyLeave(SUMOVehicle& veh, SUMOReal lastPos, MSMoveReminder::Notification reason) throw();


    /** @brief Returns whether the detector may has to be concerned during the vehicle's further movement
     *
     * If the detector is in front of the vehicle, true is returned. If
     *  the vehicle's front has passed the detector, false, because
     *  the vehicle is no longer relevant for the detector.
     *
     * @param[in] veh The entering vehicle.
     * @param[in] reason how the vehicle enters the lane
     * @return True if vehicle is on or in front of the detector.
     * @see MSMoveReminder
     * @see MSMoveReminder::notifyEnter
     * @see MSMoveReminder::Notification
     */
    bool notifyEnter(SUMOVehicle& veh, MSMoveReminder::Notification reason) throw();
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
    void writeXMLOutput(OutputDevice &dev,
        SUMOTime startTime, SUMOTime stopTime) throw(IOError) { }


    /** @brief Open the XML-output
     *
     * The implementing function should open an xml element using
     *  OutputDevice::writeXMLHeader.
     *
     * @param[in] dev The output device to write the root into
     * @exception IOError If an error on writing occurs
     */
    void writeXMLDetectorProlog(OutputDevice &dev) const throw(IOError);

protected:
    OutputDevice &myOutputDevice;

    /// @brief Detector's position on lane [m]
    const SUMOReal myPosition;

private:
    /// @brief Invalidated copy constructor.
    MSInstantInductLoop(const MSInstantInductLoop&);

    /// @brief Invalidated assignment operator.
    MSInstantInductLoop& operator=(const MSInstantInductLoop&);


};


#endif

/****************************************************************************/

