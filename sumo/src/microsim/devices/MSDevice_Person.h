/****************************************************************************/
/// @file    MSDevice_Person.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Fri, 30.01.2009
/// @version $Id$
///
// A device which is used to keep track of Persons riding with a vehicle
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSDevice_Person_h
#define MSDevice_Person_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <set>
#include <vector>
#include <map>
#include "MSDevice.h"
#include <utils/common/SUMOTime.h>
#include <microsim/MSVehicle.h>
#include <utils/common/WrappingCommand.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSDevice_Person
 * @see MSDevice
 */
class MSDevice_Person : public MSDevice {
public:
    /** @brief Build devices for the given vehicle, if needed
     *
     * The built device is stored in the given vector.
     *
     * @param[in] v The vehicle for which a device may be built
     * @param[filled] into The vector to store the built device in
     */
    static MSDevice_Person* buildVehicleDevices(SUMOVehicle& v, std::vector<MSDevice*>& into);



public:
    /// @brief Destructor.
    ~MSDevice_Person();


    /// @name Methods called on vehicle movement / state change, overwriting MSDevice
    /// @{

    /** @brief Checks whether the vehicle is at a stop and person action is needed.
     * @param[in] veh The regarded vehicle
     * @param[in] oldPos Position before the move-micro-timestep.
     * @param[in] newPos Position after the move-micro-timestep.
     * @param[in] newSpeed The vehicle's current speed
     * @return false, if the vehicle is beyond the lane, true otherwise
     * @see MSMoveReminder
     * @see MSMoveReminder::notifyMove
     * @see HelpersHBEFA
     */
    bool notifyMove(SUMOVehicle& veh, SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed);


    /** @brief Adds passengers on vehicle insertion
     *
     * @param[in] veh The entering vehicle.
     * @param[in] reason how the vehicle enters the lane
     * @return Always true
     * @see MSMoveReminder::notifyEnter
     * @see MSMoveReminder::Notification
     */
    bool notifyEnter(SUMOVehicle& veh, MSMoveReminder::Notification reason);


    /** @brief Passengers leaving on arrival
     *
     * @param[in] veh The leaving vehicle.
     * @param[in] lastPos Position on the lane when leaving.
     * @param[in] isArrival whether the vehicle arrived at its destination
     * @param[in] isLaneChange whether the vehicle changed from the lane
     *
     * @return True if it did not leave the net.
     */
    bool notifyLeave(SUMOVehicle& veh, SUMOReal lastPos,
                     MSMoveReminder::Notification reason);
    /// @}



    /** @brief Add a passenger
     *
     * @param[in] person The passenger to add.
     */
    void addPerson(MSPerson* person);


    /** @brief Return the number of passengers
     * @return The number of stored persons
     */
    unsigned int size() const {
        return static_cast<unsigned int>(myPersons.size());
    }


    /** @brief Returns the list of persons using this vehicle
     * @return Persons within this vehicle
     */
    const std::vector<MSPerson*>& getPersons() const {
        return myPersons;
    }



private:
    /** @brief Constructor
     *
     * @param[in] holder The vehicle that holds this device
     * @param[in] id The ID of the device
     */
    MSDevice_Person(SUMOVehicle& holder, const std::string& id);



private:
    /// @brief The passengers of the vehicle
    std::vector<MSPerson*> myPersons;

    /// @brief Whether the vehicle is at a stop
    bool myStopped;



private:
    /// @brief Invalidated copy constructor.
    MSDevice_Person(const MSDevice_Person&);

    /// @brief Invalidated assignment operator.
    MSDevice_Person& operator=(const MSDevice_Person&);


};


#endif

/****************************************************************************/

