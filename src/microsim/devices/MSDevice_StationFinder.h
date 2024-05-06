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
/// @file    MSDevice_StationFinder.h
/// @author  Michael Behrisch
/// @author  Mirko Barthauer
/// @date    2023-05-24
///
// A device which triggers rerouting to nearby charging stations
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/common/WrappingCommand.h>
#include "MSVehicleDevice.h"


#define DEFAULT_SOC_INTERVAL 0.1
#define DEFAULT_ENERGY_PER_DISTANCE 200 // Wh/km
#define DEFAULT_AVG_WAITING_TIME 900 // s
#define DEFAULT_CHARGINGSTATION_VIEW_DIST 10 // m
#define DEFAULT_CONSUMPTION_ESTIMATE_HISTORY 10 // s

// ===========================================================================
// class declarations
// ===========================================================================
class MSDevice_Battery;
class MSStoppingPlace;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSDevice_StationFinder
 * @brief A device which triggers rerouting to nearby charging stations
 *
 * Each device checks the battery status by being
 *  called each time step and initiaiting the search for a compatible
 *  charging station if the battery level is too low.
 *
 * @see MSDevice
 */
class MSDevice_StationFinder : public MSVehicleDevice {
public:
    enum ChargeType {
        CHARGETYPE_CHARGING,
        CHARGETYPE_BIDIRECTIONAL,
        CHARGETYPE_BATTERYEXCHANGE,
        CHARGETYPE_FUEL
    };

    enum RescueAction {
        RESCUEACTION_NONE,
        RESCUEACTION_REMOVE,
        RESCUEACTION_TOW
    };

    enum SearchState {
        SEARCHSTATE_NONE = 0,
        SEARCHSTATE_SUCCESSFUL,
        SEARCHSTATE_UNSUCCESSFUL,
        SEARCHSTATE_CHARGING,
        SEARCHSTATE_WAITING,
        SEARCHSTATE_BROKEN_DOWN
    };

    /** @brief Inserts MSDevice_StationFinder-options
     */
    static void insertOptions(OptionsCont& oc);


    /** @brief Build devices for the given vehicle, if needed
     *
     * The options are read and evaluated whether stationFinder-devices shall be built
     *  for the given vehicle.
     *
     * For each seen vehicle, the global vehicle index is increased.
     *
     * The built device is stored in the given vector.
     *
     * @param[in] v The vehicle for which a device may be built
     * @param[in, filled] into The vector to store the built device in
     */
    static void buildVehicleDevices(SUMOVehicle& v, std::vector<MSVehicleDevice*>& into);


    static void initRescueAction(const SUMOVehicle& v, const OptionsCont& oc, const std::string& option, RescueAction& myAction);

public:
    /** @brief Constructor
     *
     * @param[in] holder The vehicle that holds this device
     */
    MSDevice_StationFinder(SUMOVehicle& holder);

    /// @brief Destructor.
    ~MSDevice_StationFinder();

    /// @name Methods called on vehicle movement / state change, overwriting MSDevice
    /// @{

    /** @brief Computes current emission values and adds them to their sums
        *
        * The vehicle's current emission values
        *  are computed using the current velocity and acceleration.
        *
        * @param[in] veh The regarded vehicle
        * @param[in] oldPos Position before the move-micro-timestep.
        * @param[in] newPos Position after the move-micro-timestep.
        * @param[in] newSpeed The vehicle's current speed
        * @return false, if the vehicle is beyond the lane, true otherwise
        * @see MSMoveReminder
        * @see MSMoveReminder::notifyMove
        * @see PollutantsInterface
        */
    bool notifyMove(SUMOTrafficObject& veh, double oldPos, double newPos, double newSpeed);

    /** @brief Computes idling emission values and adds them to the emission sums
        *
        * Idling implied by zero velocity, acceleration and slope
        *
        * @param[in] veh The vehicle
        *
        * @see MSMoveReminder::notifyMove
        * @see PollutantsInterface
        */
    bool notifyIdle(SUMOTrafficObject& veh);

    /// @}

    /// @brief return the name for this type of device
    const std::string deviceName() const {
        return "stationfinder";
    }

    /** @brief Called on writing tripinfo output
     *
     * @param[in] os The stream to write the information into
     * @exception IOError not yet implemented
     * @see MSDevice::tripInfoOutput
     */
    void generateOutput(OutputDevice* tripinfoOut) const;

    void setBattery(MSDevice_Battery* battery) {
        myBattery = battery;
    }

    std::string getParameter(const std::string& key) const;


protected:
    /** @brief Internal notification about the vehicle moves, see MSMoveReminder::notifyMoveInternal()
     *
     */
    void notifyMoveInternal(const SUMOTrafficObject& veh,
                            const double frontOnLane,
                            const double timeOnLane,
                            const double meanSpeedFrontOnLane,
                            const double meanSpeedVehicleOnLane,
                            const double travelledDistanceFrontOnLane,
                            const double travelledDistanceVehicleOnLane,
                            const double meanLengthOnLane);

private:
    /** @brief central search function for close charging stations
     *
     * @param[in] router
     * @param[in] expectedConsumption
     * @param[in] constrainTT whether to constrain the search radius by a maximum travel time
     * @param[in] skipVisited whether to skip charging stations which have not been available when passing by recently
     * @return The found charging station, otherwise nullptr
     */
    MSChargingStation* findChargingStation(SUMOAbstractRouter<MSEdge, SUMOVehicle>& router, double expectedConsumption, bool constrainTT = true, bool skipVisited = true);

    /** @brief reroute to a charging station
     *
     * @param[in] replace if the already planned next stop should be replaced (a new stop will be prepended if false)
     * @return true if the vehicle has been redirected to a charging station, false otherwise
     */
    bool rerouteToChargingStation(bool replace = false);

    /** @brief search for a charging station and teleport the vehicle there as a rescue measure
     */
    SUMOTime teleportToChargingStation(const SUMOTime currentTime);

    /** @brief estimate the energy needed for the planned route / up to a target edge
     *
     * @param[in] target edge along the route up to which the consumption shall be estimated - the complete route will be used if defaulting to nullptr
     * @param[in] includeEmptySoC whether to add an additional buffer for the range up to the "empty" threshold
     * @param[in] stopDiscount duration in seconds to discount in the consumption estimation due to occurred stopping time
     * @return energy in Wh needed to complete the planned route
     */
    double estimateConsumption(const MSEdge* target = nullptr, const bool includeEmptySoC = true, const double stopDiscount = 0.) const;

    /** @brief adopt a planned charging stop outside of the device
     *
     * @return whether an already present stop was adopted to be used with the device logic
     */
    bool alreadyPlannedCharging();

    /** @brief create the event command for teleporting in case of brake-down
     */
    void initRescueCommand();

private:
    /// @brief myHolder cast to needed type
    MSVehicle& myVeh;

    /// @brief The corresponding battery device
    MSDevice_Battery* myBattery;

    /// @brief To which station we are currently travelling
    MSStoppingPlace* myChargingStation;

    /// @brief The command responsible for rescue actions
    WrappingCommand<MSDevice_StationFinder>* myRescueCommand;

    /// @brief The memory of lastly visited charging stations during the search before being able to charge
    std::vector<MSChargingStation*> myPassedChargingStations;

    /// @brief Last time the SoC was checked
    SUMOTime myLastChargeCheck;

    /// @brief Time interval after which the SoC has to be checked
    SUMOTime myCheckInterval;

    /// @brief Arrival time in the vicinity of the target charging station (to track the waiting time before accessing it)
    SUMOTime myArrivalAtChargingStation;

    /// @brief Last time charging stations have been searched
    SUMOTime myLastSearch;

    /// @brief The time to wait for a rescue vehicle in case the battery is empty
    double myRescueTime;

    /// @brief The safety buffer when calculating expected consumption
    double myReserveFactor;

    /// @brief The state of charge threshold below which rescue mode is activated
    double myEmptySoC;

    /// @brief The max travel time to the next charging station
    SUMOTime myRadius;

    /// @brief Time interval to search again for a charging station if the first attempt failed
    SUMOTime myRepeatInterval;

    /// @brief Accepted waiting time at the charging station before a place becomes available
    SUMOTime myWaitForCharge;

    /// @brief SoC the last time the station finder algorithm was run completely
    double myUpdateSoC;

    /// @brief The maximum charging speed of the vehicle battery in W
    double myMaxChargePower;

    /// @brief The target state of charge where the vehicle stops charging
    double myTargetSoC;

    /// @brief The state of charge at which the vehicle starts looking for charging stations
    double mySearchSoC;

    /// @brief The type of charging permitted by the battery (charging, bidirectional, battery exchange)
    ChargeType myChargeType;

    /// @brief What to do when the state of charge gets very low
    RescueAction myRescueAction;

    /// @brief The current state of the charging search (remember for decision logic)
    SearchState mySearchState = SEARCHSTATE_NONE;

private:
    /// @brief Invalidated copy constructor.
    MSDevice_StationFinder(const MSDevice_StationFinder&);

    /// @brief Invalidated assignment operator.
    MSDevice_StationFinder& operator=(const MSDevice_StationFinder&);
};
