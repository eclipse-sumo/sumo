/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    MSInsertionControl.h
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Mon, 12 Mar 2001
///
// Inserts vehicles into the network when their departure time is reached
/****************************************************************************/
#pragma once
#include <config.h>

#include <vector>
#include <map>
#include <string>
#include <set>
#include <utils/foxtools/MFXSynchSet.h>
#include <microsim/MSRouterDefs.h>
#include "MSVehicleContainer.h"


// ===========================================================================
// class declarations
// ===========================================================================
class MSVehicle;
class MSVehicleControl;
class SUMOVehicle;
class SUMOVehicleParameter;


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
 * @todo When a vehicle is deleted due to waiting too long or because of vaporizing, this is not reported anywhere
 */
class MSInsertionControl {
public:
    /** @brief Constructor
     *
     * @param[in] vc The assigned vehicle control (needed for vehicle re-insertion and deletion)
     * @param[in] maxDepartDelay Vehicles waiting for insertion longer than this time are deleted (-1: no deletion)
     * @param[in] checkEdgesOnce Whether an edge on which a vehicle could not depart should be ignored in the same step
     * @param[in] maxVehicleNumber The maximum number of vehicles that should not be exceeded
     */
    MSInsertionControl(MSVehicleControl& vc, SUMOTime maxDepartDelay, bool checkEdgesOnce, int maxVehicleNumber, SUMOTime randomDepartOffset);


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
    int emitVehicles(SUMOTime time);


    /** @brief Adds a single vehicle for departure
     *
     * The vehicle is added to "myAllVeh".
     *
     * @param[in] veh The vehicle to add for later insertion
     */
    void add(SUMOVehicle* veh);


    /** @brief Adds parameter for a vehicle flow for departure
     *
     * @param[in] pars The flow parameters to add for later insertion
     * @param[in] index The current index when loading this flow from a simulation state
     * @return whether it could be added (no other flow with the same id was present)
     */
    bool addFlow(SUMOVehicleParameter* const pars, int index = -1);


    /** @brief Returns the number of waiting vehicles
     *
     * The sizes of refused emits (sum of vehicles in "myRefusedEmits1" and
     *  "myRefusedEmits2") is returned.
     *
     * @return The number of vehicles that could not (yet) be inserted into the net
     */
    int getWaitingVehicleNo() const;

    /// @brief retrieve vehicles waiting for insertion
    const MSVehicleContainer::VehicleVector& getPendingVehicles() const {
        return myPendingEmits;
    }

    /** @brief Returns the number of flows that are still active
     *
     * @return number of active flows
     */
    int getPendingFlowCount() const;

    /// @brief stops trying to emit the given vehicle (because it already departed)
    void alreadyDeparted(SUMOVehicle* veh);

    /// @brief stops trying to emit the given vehicle (and delete it)
    void descheduleDeparture(const SUMOVehicle* veh);

    /// @brief reverts a previous call to descheduleDeparture (only needed for departPos="random_free")
    void retractDescheduleDeparture(const SUMOVehicle* veh);

    /// @brief clears out all pending vehicles from a route, "" for all routes
    void clearPendingVehicles(const std::string& route);


    /** @brief Checks for all vehicles whether they can be emitted
     *
     * @param[in] time The current simulation time
     */
    void determineCandidates(SUMOTime time);

    /// @brief return the number of pending emits for the given lane
    int getPendingEmits(const MSLane* lane);

    void adaptIntermodalRouter(MSTransportableRouter& router) const;

    /// @brief compute (optional) random offset to the departure time
    SUMOTime computeRandomDepartOffset() const;

    /** @brief Saves the current state into the given stream
     */
    void saveState(OutputDevice& out);

    /** @brief Remove all vehicles before quick-loading state */
    void clearState();

    /// @brief retrieve internal RNG
    SumoRNG* getFlowRNG() {
        return &myFlowRNG;
    }

    /// @brief checks whether the given flow still exists
    bool hasFlow(const std::string& id) const {
        return myFlowIDs.count(id) != 0;
    }

    /// @brief return parameters for the given flow
    const SUMOVehicleParameter* getFlowPars(const std::string& id) const;

    /// @brief return the last vehicle for the given flow
    SUMOVehicle* getLastFlowVehicle(const std::string& id) const;

    /// @brief updates the flow scale value to keep track of TraCI-induced change
    void updateScale(const std::string vtypeid);

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
    int tryInsert(SUMOTime time, SUMOVehicle* veh,
                  MSVehicleContainer::VehicleVector& refusedEmits);


    /** @brief Adds all vehicles that should have been emitted earlier to the refuse container
     *
     * @param[in] time The current simulation time
     * @todo recheck
     */
    void checkCandidates(SUMOTime time, const bool preCheck);


private:

    /// @brief init scale value of flow
    static double initScale(const std::string vtypeid);

private:
    /// @brief The assigned vehicle control (needed for vehicle re-insertion and deletion)
    MSVehicleControl& myVehicleControl;

    /// @brief All loaded vehicles sorted by their departure time
    MSVehicleContainer myAllVeh;

    /// @brief Buffers for vehicles that could not be inserted
    MSVehicleContainer::VehicleVector myPendingEmits;

    /// @brief Buffer for vehicles that may be inserted in the current step
    std::set<SUMOVehicle*> myEmitCandidates;

    /// @brief Set of vehicles which shall not be inserted anymore

#ifdef HAVE_FOX
    MFXSynchSet<const SUMOVehicle*> myAbortedEmits;
#else
    std::set<const SUMOVehicle*> myAbortedEmits;
#endif

    /** @struct Flow
     * @brief Definition of vehicle flow with the current index for vehicle numbering
     */
    struct Flow {
        /// @brief The parameters
        SUMOVehicleParameter* pars;
        /// @brief the running index
        int index;
        /// @brief the type scaling of this flow. Negative value indicates inhomogenous type distribution
        double scale;
    };

    /// @brief Container for periodical vehicle parameters
    std::vector<Flow> myFlows;

    /// @brief Cache for periodical vehicle ids and their most recent index for quicker checking
    std::map<std::string, int> myFlowIDs;

    /// @brief The maximum waiting time; vehicles waiting longer are deleted (-1: no deletion)
    SUMOTime myMaxDepartDelay;

    /// @brief Whether an edge on which a vehicle could not depart should be ignored in the same step
    bool myEagerInsertionCheck;

    /// @brief Storage for maximum vehicle number
    int myMaxVehicleNumber;

    /// @brief Last time at which pending emits for each edge where counted
    SUMOTime myPendingEmitsUpdateTime;

    /// @brief the number of pending emits for each edge in the current time step
    std::map<const MSLane*, int> myPendingEmitsForLane;

    /// @brief The maximum random offset to be added to vehicles departure times (non-negative)
    SUMOTime myMaxRandomDepartOffset;

private:
    /// @brief Invalidated copy constructor.
    MSInsertionControl(const MSInsertionControl&);

    /// @brief Invalidated assignment operator.
    MSInsertionControl& operator=(const MSInsertionControl&);

    /// @brief A random number generator for probabilistic flows
    SumoRNG myFlowRNG;

};
