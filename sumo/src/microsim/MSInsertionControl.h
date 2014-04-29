/****************************************************************************/
/// @file    MSInsertionControl.h
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Mon, 12 Mar 2001
/// @version $Id$
///
// Inserts vehicles into the network when their departure time is reached
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
#ifndef MSInsertionControl_h
#define MSInsertionControl_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSVehicleContainer.h"
#include <vector>
#include <map>
#include <string>


// ===========================================================================
// class declarations
// ===========================================================================
class MSVehicle;
class MSVehicleControl;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSInsertionControl
 * @brief Inserts vehicles into the network when their departure time is reached
 *
 * Holds a list of vehicles which may be filled by vehicles
 *  read by SUMORouteLoaders. Tries to emit vehicles departing at a time into the
 *  network as soon this time is reached and keeps them as long the insertion
 *  fails.
 *
 * If a vehicle is emitted, the control about it is given to the lanes.
 *
 * Vehicles are not controlled (created, deleted) by this class.
 *
 * @todo Whe a vehicle is deleted due to waiting too long or because of vaporizing, ths is not reported anywhere
 */
class MSInsertionControl {
public:
    /** @brief Constructor
     *
     * @param[in] vc The assigned vehicle control (needed for vehicle re-insertion and deletion)
     * @param[in] maxDepartDelay Vehicles waiting for insertion longer than this time are deleted (-1: no deletion)
     * @param[in] checkEdgesOnce Whether an edge on which a vehicle could not depart should be ignored in the same step
     */
    MSInsertionControl(MSVehicleControl& vc, SUMOTime maxDepartDelay, bool checkEdgesOnce);


    /// @brief Destructor.
    ~MSInsertionControl();


    /** @brief Emits vehicles that want to depart at the given time
     *
     * All vehicles scheduled for this time are tried to be emitted. This
     *  includes those with a depart time as the given time and those that
     *  wait for being emitted due they could not be inserted in previous
     *  steps.
     *
     * For each vehicle, tryInsert is called. If this fails, a vehicle
     *  keeps within the refused emit containers ("myRefusedEmits1",
     *  "myRefusedEmits2") so that it may be emitted within the next steps.
     *
     * Returns the number of vehicles that could be inserted into the net.
     *
     * @param[in] time The current simulation time
     * @return The number of vehicles that could be inserted into the net
     */
    unsigned int emitVehicles(SUMOTime time);


    /** @brief Adds a single vehicle for departure
     *
     * The vehicle is added to "myAllVeh".
     *
     * @param[in] veh The vehicle to add for later insertion
     */
    void add(SUMOVehicle* veh);


    /** @brief Adds parameter for a vehicle flow for departure
     *
     * @param[in] flow The flow to add for later insertion
     */
    void add(SUMOVehicleParameter* pars);


    /** @brief Returns the number of waiting vehicles
     *
     * The sizes of refused emits (sum of vehicles in "myRefusedEmits1" and
     *  "myRefusedEmits2") is returned.
     *
     * @return The number of vehicles that could not (yet) be inserted into the net
     */
    unsigned int getWaitingVehicleNo() const;


    /** @brief Returns the number of flows that are still active
     *
     * @return number of active flows
     */
    int getPendingFlowCount() const;


    /// @brief stops trying to emit the given vehicle
    void descheduleDeparture(SUMOVehicle* veh);


    /// @brief clears out all pending vehicles from a route, "" for all routes
    void clearPendingVehicles(std::string& route);


private:
    /** @brief Tries to emit the vehicle
     *
     * If the insertion fails, it is examined whether the reason was a vaporizing
     *  edge. If so, the vehicle is deleted. Otherwise, it is checked whether the
     *  time the vehicle had to wait so far is larger than the maximum allowed
     *  waiting time. If so, the vehicle is deleted, too. If both does not match,
     *  the vehicle is reinserted to "refusedEmits" in order to be emitted in
     *  next steps.
     *
     * @param[in] time The current simulation time
     * @param[in] veh The vehicle to emit
     * @param[in] refusedEmits Container to insert vehicles that could not be emitted into
     * @return The number of emitted vehicles (0 or 1)
     */
    unsigned int tryInsert(SUMOTime time, SUMOVehicle* veh,
                           MSVehicleContainer::VehicleVector& refusedEmits);


    /** @brief Checks whether any flow is blocked due to this vehicle and clears the block
     *
     * @param[in] veh The vehicle to check for
     */
    void checkFlowWait(SUMOVehicle* veh);


    /** @brief Adds all vehicles that should have been emitted earlier to the refuse container
     *
     * @param[in] time The current simulation time
     * @todo recheck
     */
    void checkPrevious(SUMOTime time);


    /** @brief Checks for all vehicles coming from flows whether they can be emitted
     *
     * @param[in] time The current simulation time
     * @param[in] refusedEmits Container to insert vehicles that could not be emitted into
     * @return The number of emitted vehicles
     */
    unsigned int checkFlows(SUMOTime time,
                            MSVehicleContainer::VehicleVector& refusedEmits);


private:
    /// @brief The assigned vehicle control (needed for vehicle re-insertion and deletion)
    MSVehicleControl& myVehicleControl;

    /// @brief All loaded vehicles sorted by their departure time
    MSVehicleContainer myAllVeh;

    /// @brief Buffers for vehicles that could not be inserted
    MSVehicleContainer::VehicleVector myRefusedEmits1, myRefusedEmits2;

    /// @brief Set of vehicles which shall not be inserted anymore
    std::set<SUMOVehicle*> myAbortedEmits;

    /** @struct Stop
     * @brief Definition of vehicle stop (position and duration)
     */
    struct Flow {
        /// @brief The paramters
        SUMOVehicleParameter* pars;
        /// @brief whether it has route or vehicle type distribution
        bool isVolatile;
        /// @brief The last created vehicle
        SUMOVehicle* vehicle;
        /// @brief the running index
        unsigned int index;
    };

    /// @brief Container for periodical vehicle parameters
    std::vector<Flow> myFlows;

    /// @brief The maximum waiting time; vehicles waiting longer are deleted (-1: no deletion)
    SUMOTime myMaxDepartDelay;

    /// @brief Whether an edge on which a vehicle could not depart should be ignored in the same step
    bool myCheckEdgesOnce;


private:
    /// @brief Invalidated copy constructor.
    MSInsertionControl(const MSInsertionControl&);

    /// @brief Invalidated assignment operator.
    MSInsertionControl& operator=(const MSInsertionControl&);


};


#endif

/****************************************************************************/

