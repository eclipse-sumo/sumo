/****************************************************************************/
/// @file    MSDevice_Person.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 30.01.2009
/// @version $Id$
///
// A device which collects vehicular emissions (using HBEFA-reformulation)
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
 * @brief A device which collects vehicular emissions (using HBEFA-reformulation)
 *
 * Each device collects the vehicular emissions / fuel consumption by being
 *  called each time step, computing the current values using HelpersHBEFA, and
 *  aggregating them into internal storages over the whol journey.
 *
 * @see MSDevice
 * @see HelpersHBEFA
 */
class MSDevice_Person : public MSDevice {
public:
    /** @brief Build devices for the given vehicle, if needed
     *
     * The built device is stored in the given vector.
     *
     * @param[in] v The vehicle for which a device may be built
     * @param[in, filled] into The vector to store the built device in
     */
    static MSDevice_Person* buildVehicleDevices(SUMOVehicle &v, std::vector<MSDevice*> &into) throw();


public:
    /// @name Methods called on vehicle movement / state change, overwriting MSDevice
    /// @{

    /** @brief Checks whether the vehicle is at a stop and person action is needed.
     *
     * The fraction of time the vehicle is on the lane is computed and
     *  used as a weight for the vehicle's current emission values
     *  which are computed using the current velocity and acceleration.
     *
     * @param[in] veh The regarded vehicle
     * @param[in] oldPos Position before the move-micro-timestep.
     * @param[in] newPos Position after the move-micro-timestep.
     * @param[in] newSpeed The vehicle's current speed
     * @return false, if the vehicle is beyond the lane, true otherwise
     * @see MSMoveReminder
     * @see MSMoveReminder::isStillActive
     * @see HelpersHBEFA
     */
    bool isStillActive(SUMOVehicle& veh, SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed) throw();

    /** @brief Adds passengers on vehicle emission
     *
     * @param[in] veh The entering vehicle.
     * @param[in] isEmit whether the vehicle was just emitted into the net
     * @param[in] isLaneChange whether the vehicle changed to the lane
     *
     */
    bool notifyEnter(SUMOVehicle& veh, bool isEmit, bool isLaneChange) throw();

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
                     bool isArrival, bool isLaneChange) throw();
    /// @}


    void addPerson(MSPerson* person) throw();

private:
    /** @brief Constructor
     *
     * @param[in] holder The vehicle that holds this device
     * @param[in] id The ID of the device
     */
    MSDevice_Person(SUMOVehicle &holder, const std::string &id) throw();


    /// @brief Destructor.
    ~MSDevice_Person() throw();


private:
    /// @brief The passengers of the vehicle
    std::vector<MSPerson*> myPersons;

    /// @brief whether the vehicle is at a stop
    bool myStopped;


private:
    /// @brief Invalidated copy constructor.
    MSDevice_Person(const MSDevice_Person&);

    /// @brief Invalidated assignment operator.
    MSDevice_Person& operator=(const MSDevice_Person&);


};


#endif

/****************************************************************************/

