/****************************************************************************/
/// @file    MSDevice_BTsender.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    14.08.2013
/// @version $Id$
///
// A BT sender
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2013-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSDevice_BTsender_h
#define MSDevice_BTsender_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <set>
#include <string>
#include "MSDevice.h"
#include <utils/common/SUMOTime.h>
#include <utils/geom/Position.h>


// ===========================================================================
// class declarations
// ===========================================================================
class SUMOVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSDevice_BTsender
 * @brief A BT sender
 *
 * @see MSDevice
 */
class MSDevice_BTsender : public MSDevice {
public:
    /** @brief Inserts MSDevice_BTsender-options
     * @param[filled] oc The options container to add the options to
     */
    static void insertOptions(OptionsCont& oc);


    /** @brief Build devices for the given vehicle, if needed
     *
     * The options are read and evaluated whether a bt-sender-device shall be built
     *  for the given vehicle.
     *
     * The built device is stored in the given vector.
     *
     * @param[in] v The vehicle for which a device may be built
     * @param[filled] into The vector to store the built device in
     */
    static void buildVehicleDevices(SUMOVehicle& v, std::vector<MSDevice*>& into);


    /// for accessing the maps of running/arrived vehicles
    friend class MSDevice_BTreceiver;



public:
    /// @brief Destructor.
    ~MSDevice_BTsender();



    /// @name Methods inherited from MSMoveReminder.
    /// @{

    /** @brief Adds the vehicle to running vehicles if it (re-) enters the network
     *
     * @param[in] veh The entering vehicle.
     * @param[in] reason how the vehicle enters the lane
     * @return Always true
     * @see MSMoveReminder::notifyEnter
     * @see MSMoveReminder::Notification
     */
    bool notifyEnter(SUMOVehicle& veh, Notification reason);


    /** @brief Checks whether the reminder still has to be notified about the vehicle moves
     *
     * Indicator if the reminders is still active for the passed
     * vehicle/parameters. If false, the vehicle will erase this reminder
     * from it's reminder-container.
     *
     * @param[in] veh Vehicle that asks this reminder.
     * @param[in] oldPos Position before move.
     * @param[in] newPos Position after move with newSpeed.
     * @param[in] newSpeed Moving speed.
     *
     * @return True if vehicle hasn't passed the reminder completely.
     */
    bool notifyMove(SUMOVehicle& veh, SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed);


    /** @brief Moves (the known) vehicle from running to arrived vehicles' list
     *
     * @param[in] veh The leaving vehicle.
     * @param[in] lastPos Position on the lane when leaving.
     * @param[in] isArrival whether the vehicle arrived at its destination
     * @param[in] isLaneChange whether the vehicle changed from the lane
     * @see leaveDetectorByLaneChange
     * @see MSMoveReminder
     * @see MSMoveReminder::notifyLeave
     */
    bool notifyLeave(SUMOVehicle& veh, SUMOReal lastPos, Notification reason);
    /// @}



    /** @class VehicleState
     * @brief A single movement state of the vehicle
     */
    class VehicleState {
    public:
        /** @brief Constructor
         * @param[in] _time The current time
         * @param[in] _speed The speed of the vehicle
         * @param[in] _angle The angle of the vehicle
         * @param[in] _position The position of the vehicle
         * @param[in] _laneID The id of the lane the vehicle is located at
         * @param[in] _lanePos The position of the vehicle along the lane
         */
        VehicleState(SUMOReal _time, SUMOReal _speed, SUMOReal _angle, const Position& _position, const std::string& _laneID, SUMOReal _lanePos)
            : time(_time), speed(_speed), angle(_angle), position(_position), laneID(_laneID), lanePos(_lanePos) {}

        /// @brief Destructor
        ~VehicleState() {}

        /// @brief The current time
        SUMOReal time;
        /// @brief The speed of the vehicle
        SUMOReal speed;
        /// @brief The angle of the vehicle
        SUMOReal angle;
        /// @brief The position of the vehicle
        Position position;
        /// @brief The lane the vehicle was at
        std::string laneID;
        /// @brief The position at the lane of the vehicle
        SUMOReal lanePos;

    };



    /** @class VehicleInformation
     * @brief Stores the information of a vehicle
     */
    class VehicleInformation : public Named {
    public:
        /** @brief Constructor
         * @param[in] id The id of the vehicle
         */
        VehicleInformation(const std::string& id) : Named(id), amOnNet(true), haveArrived(false)  {}

        /// @brief Destructor
        ~VehicleInformation() {}

        /** @brief Returns the boundary of passed positions
         * @return The positions boundary
         */
        Boundary getBoxBoundary() const {
            Boundary ret;
            for (std::vector<VehicleState>::const_iterator i = updates.begin(); i != updates.end(); ++i) {
                ret.add((*i).position);
            }
            return ret;
        }

        /// @brief List of position updates during last step
        std::vector<VehicleState> updates;

        /// @brief Whether the vehicle is within the simulated network
        bool amOnNet;

        /// @brief Whether the vehicle was removed from the simulation
        bool haveArrived;

    };



private:
    /** @brief Constructor
     *
     * @param[in] holder The vehicle that holds this device
     * @param[in] id The ID of the device
     */
    MSDevice_BTsender(SUMOVehicle& holder, const std::string& id);



protected:
    /// @brief The list of arrived senders
    static std::map<std::string, VehicleInformation*> sVehicles;



private:
    /// @brief Invalidated copy constructor.
    MSDevice_BTsender(const MSDevice_BTsender&);

    /// @brief Invalidated assignment operator.
    MSDevice_BTsender& operator=(const MSDevice_BTsender&);


};


#endif

/****************************************************************************/

