/****************************************************************************/
/// @file    MSDevice_Container.h
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @date    Mon, 16 Jun 2014
/// @version $Id$
///
// A device which is used to keep track of containers riding with a vehicle
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
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
#ifndef MSDevice_Container_h
#define MSDevice_Container_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

//#include <set>
#include <vector>
//#include <map>
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
 * @class MSDevice_Container
 * @see MSDevice
 */
class MSDevice_Container : public MSDevice {
public:
    /** @brief Build devices for the given vehicle, if needed
     *
     * The built device is stored in the given vector.
     *
     * @param[in] v The vehicle for which a device may be built
     * @param[filled] into The vector to store the built device in
     */
    static MSDevice_Container* buildVehicleDevices(SUMOVehicle& v, std::vector<MSDevice*>& into);



public:
    /// @brief Destructor.
    ~MSDevice_Container();


    /// @name Methods called on vehicle movement / state change, overwriting MSDevice
    /// @{

    /** @brief Checks whether the vehicle is at a stop and container action is needed.
     * @param[in] veh The regarded vehicle
     * @param[in] oldPos Position before the move-micro-timestep.
     * @param[in] newPos Position after the move-micro-timestep.
     * @param[in] newSpeed The vehicle's current speed
     * @return false, if the vehicle is beyond the lane, true otherwise
     * @see MSMoveReminder
     * @see MSMoveReminder::notifyMove
     */
    bool notifyMove(SUMOVehicle& veh, SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed);


    /** @brief Adds containers on vehicle insertion
     *
     * @param[in] veh The entering vehicle.
     * @param[in] reason how the vehicle enters the lane
     * @return Always true
     * @see MSMoveReminder::notifyEnter
     * @see MSMoveReminder::Notification
     */
    bool notifyEnter(SUMOVehicle& veh, MSMoveReminder::Notification reason);


    /** @brief Containers leaving on arrival
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



    /** @brief Add a container
     *
     * @param[in] container The container to add.
     */
    void addContainer(MSContainer* container);


    /** @brief Return the number of containers
     * @return The number of stored containers
     */
    unsigned int size() const {
        return static_cast<unsigned int>(myContainers.size());
    }


    /** @brief Returns the list of containers using this vehicle
     * @return Containers within this vehicle
     */
    const std::vector<MSContainer*>& getContainers() const {
        return myContainers;
    }



private:
    /** @brief Constructor
     *
     * @param[in] holder The vehicle that holds this device
     * @param[in] id The ID of the device
     */
    MSDevice_Container(SUMOVehicle& holder, const std::string& id);



private:
    /// @brief The containers of the vehicle
    std::vector<MSContainer*> myContainers;

    /// @brief Whether the vehicle is at a stop
    bool myStopped;



private:
    /// @brief Invalidated copy constructor.
    MSDevice_Container(const MSDevice_Container&);

    /// @brief Invalidated assignment operator.
    MSDevice_Container& operator=(const MSDevice_Container&);


};


#endif

/****************************************************************************/

