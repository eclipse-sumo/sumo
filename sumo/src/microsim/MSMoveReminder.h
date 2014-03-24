/****************************************************************************/
/// @file    MSMoveReminder.h
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    2003-05-21
/// @version $Id$
///
// Something on a lane to be noticed about vehicle movement
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2003-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSMoveReminder_h
#define MSMoveReminder_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <map>
#include <utils/common/SUMOTime.h>
#include <utils/common/StdDefs.h>


// ===========================================================================
// class declarations
// ===========================================================================
class SUMOVehicle;
class MSLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSMoveReminder
 * @brief Something on a lane to be noticed about vehicle movement
 *
 * Base class of all move-reminders. During move, the vehicles call
 *  notifyMove() for all reminders on their current lane (all lanes
 *  they pass during one step). If a vehicle enters the lane the reminder is
 *  positioned at during insertion or lanechange notifyEnter() is
 *  called. If a vehicle leaves the reminder lane it calls notifyLeave().
 *
 * The reminder knows whom to tell about move, insertion and lanechange. The
 * vehicles will remove the reminder that is not notifyMove() from
 * their reminder container.
 *
 * @see MSLane::addMoveReminder
 * @see MSLane::getMoveReminder
 * @note: cannot inherit from Named because it would couse double inheritance
 */
class MSMoveReminder {
public:
    /** @brief Constructor.
     *
     * @param[in] lane Lane on which the reminder will work.
     * @param[in] doAdd whether to add the reminder to the lane
     */
    MSMoveReminder(const std::string& description, MSLane* const lane = 0, const bool doAdd = true);


    /** @brief Destructor
     */
    virtual ~MSMoveReminder() {}


    /** @brief Returns the lane the reminder works on.
     *
     * @return The lane the reminder works on.
     */
    const MSLane* getLane() const {
        return myLane;
    }


    /// @brief Definition of a vehicle state
    enum Notification {
        /// @brief The vehicle has departed (was inserted into the network)
        NOTIFICATION_DEPARTED,
        /// @brief The vehicle arrived at a junction
        NOTIFICATION_JUNCTION,
        /// @brief The vehicle changes the segment (meso only)
        NOTIFICATION_SEGMENT,
        /// @brief The vehicle changes lanes (micro only)
        NOTIFICATION_LANE_CHANGE,
        /// @brief The vehicle is being teleported
        NOTIFICATION_TELEPORT,
        /// @brief The vehicle starts or ends parking
        NOTIFICATION_PARKING,
        /// @brief The vehicle arrived at its destination (is deleted)
        NOTIFICATION_ARRIVED, // arrived and everything after is treated as permanent deletion from the net
        /// @brief The vehicle got vaporized
        NOTIFICATION_VAPORIZED,
        /// @brief The vehicle was teleported out of the net
        NOTIFICATION_TELEPORT_ARRIVED
    };


    /// @name Interface methods, to be derived by subclasses
    /// @{

    /** @brief Checks whether the reminder is activated by a vehicle entering the lane
     *
     * Lane change means in this case that the vehicle changes to the lane
     *  the reminder is placed at.
     *
     * @param[in] veh The entering vehicle.
     * @param[in] reason how the vehicle enters the lane
     * @return True if vehicle enters the reminder.
     * @see Notification
     */
    virtual bool notifyEnter(SUMOVehicle& veh, Notification reason) {
        UNUSED_PARAMETER(reason);
        UNUSED_PARAMETER(&veh);
        return true;
    }


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
    virtual bool notifyMove(SUMOVehicle& veh,
                            SUMOReal oldPos,
                            SUMOReal newPos,
                            SUMOReal newSpeed) {
        UNUSED_PARAMETER(oldPos);
        UNUSED_PARAMETER(newPos);
        UNUSED_PARAMETER(newSpeed);
        UNUSED_PARAMETER(&veh);
        return true;
    }


    /** @brief Called if the vehicle leaves the reminder's lane
     *
     * Informs if vehicle leaves reminder lane (due to lane change, removal
     *  from the network, or leaving to the next lane).
     *  The default is to do nothing.
     *
     * @param[in] veh The leaving vehicle.
     * @param[in] lastPos Position on the lane when leaving.
     * @param[in] reason how the vehicle leaves the lane
     * @see Notification
     *
     * @return True if the reminder wants to receive further info.
     */
    virtual bool notifyLeave(SUMOVehicle& veh, SUMOReal lastPos,
                             Notification reason) {
        UNUSED_PARAMETER(reason);
        UNUSED_PARAMETER(lastPos);
        UNUSED_PARAMETER(&veh);
        return true;
    }


#ifdef HAVE_INTERNAL
    void updateDetector(SUMOVehicle& veh, SUMOReal entryPos, SUMOReal leavePos,
                        SUMOTime entryTime, SUMOTime currentTime, SUMOTime leaveTime);
#endif

    /// @}


    /** @brief Internal notification about the vehicle moves
     *
     * Indicator if the reminders is still active for the passed
     * vehicle/parameters. If false, the vehicle will erase this reminder
     * from it's reminder-container.
     *
     * @param[in] veh Vehicle that asks this reminder.
     * @param[in] timeOnLane time the vehicle spent on the lane.
     * @param[in] speed Moving speed.
     */
    virtual void notifyMoveInternal(SUMOVehicle& veh,
                                    SUMOReal timeOnLane,
                                    SUMOReal speed) {
        UNUSED_PARAMETER(speed);
        UNUSED_PARAMETER(timeOnLane);
        UNUSED_PARAMETER(&veh);
    }

    void setDescription(const std::string& description) {
        myDescription = description;
    }

    const std::string& getDescription() const {
        return myDescription;
    }


protected:
    /// @brief Lane on which the reminder works
    MSLane* const myLane;
    /// @brief a description of this moveReminder
    std::string myDescription;

#ifdef HAVE_INTERNAL
    std::map<SUMOVehicle*, std::pair<SUMOTime, SUMOReal> > myLastVehicleUpdateValues;
#endif


private:
    MSMoveReminder& operator=(const MSMoveReminder&); // just to avoid a compiler warning

};


#endif

/****************************************************************************/

