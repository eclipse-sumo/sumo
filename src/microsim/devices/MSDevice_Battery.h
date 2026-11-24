/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2013-2026 German Aerospace Center (DLR) and others.
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
/// @file    MSDevice_Battery.h
/// @author  Tamas Kurczveil
/// @author  Pablo Alvarez Lopez
/// @author  Mirko Barthauer
/// @date    20-12-13
///
// The Battery parameters for the vehicle
/****************************************************************************/
#pragma once
#include <config.h>

#include <microsim/devices/MSVehicleDevice.h>
#include <microsim/MSVehicle.h>
#include <microsim/trigger/MSChargingStation.h>
#include <utils/common/LinearApproxHelpers.h>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class SUMOVehicle;
class MSDevice_Emissions;
class MSDevice_StationFinder;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSDevice_Battery
 * @brief Battery device for electric vehicles
 */
class MSDevice_Battery : public MSVehicleDevice {
public:

    /** @brief Inserts MSDevice_Example-options
    * @param[filled] oc The options container to add the options to
    */
    static void insertOptions(OptionsCont& oc);

    /** @brief Build devices for the given vehicle, if needed
    *
    * The options are read and evaluated whether a example-device shall be built
    *  for the given vehicle.
    *
    * The built device is stored in the given vector.
    *
    * @param[in] v The vehicle for which a device may be built
    * @param[filled] into The vector to store the built device in
    */
    static void buildVehicleDevices(SUMOVehicle& v, std::vector<MSVehicleDevice*>& into, MSDevice_StationFinder* sf);

public:
    /// @brief Destructor.
    ~MSDevice_Battery();

    /// @name Methods called on vehicle movement / state change, overwriting MSDevice
    /// @{
    /** @brief Checks for waiting steps when the vehicle moves
    *
    * @param[in] tObject Vehicle that asks this reminder.
    * @param[in] oldPos Position before move.
    * @param[in] newPos Position after move with newSpeed.
    * @param[in] newSpeed Moving speed.
    *
    * @return True (always).
    */
    bool notifyMove(SUMOTrafficObject& tObject, double oldPos,  double newPos, double newSpeed) override;
    /// @}

    /// @brief return the name for this type of device
    const std::string deviceName() const override {
        return "battery";
    }

    /** @brief Saves the state of the device
     *
     * @param[in] out The OutputDevice to write the information into
     */
    void saveState(OutputDevice& out) const override;

    /** @brief Loads the state of the device from the given description
     *
     * @param[in] attrs XML attributes describing the current state
     */
    void loadState(const SUMOSAXAttributes& attrs) override;

    /// @brief try to retrieve the given parameter from this device. Throw exception for unsupported key
    std::string getParameter(const std::string& key) const override;

    /// @brief try to set the given parameter for this device. Throw exception for unsupported key
    void setParameter(const std::string& key, const std::string& value) override;

    /// @brief called to update state for parking vehicles
    void notifyParking() override;

    /// @brief Called on vehicle deletion to extend tripinfo
    void generateOutput(OutputDevice* tripinfoOut) const override;

protected:
    /** @brief Internal notification about the vehicle moves, see MSMoveReminder::notifyMoveInternal()
     *
     */
    void notifyMoveInternal(const SUMOTrafficObject& tObject,
                            const double frontOnLane,
                            const double timeOnLane,
                            const double meanSpeedFrontOnLane,
                            const double meanSpeedVehicleOnLane,
                            const double travelledDistanceFrontOnLane,
                            const double travelledDistanceVehicleOnLane,
                            const double meanLengthOnLane) override;

private:
    /** @brief Constructor
    *
    * @param[in] holder The vehicle that holds this device
    * @param[in] id The ID of the device
    * @param[in] actualBatteryCapacity The current battery capacity
    * @param[in] maximumBatteryCapacity The maximum battery capacity
    * @param[in] stoppingThreshold The speed below which charging may happen
    * @param[in] maximumChargeRate The maximum charging rate allowed by the battery control
    * @param[in] chargeLevelTable The axis values of the charge curve
    * @param[in] chargeCurveTable The charge curve state of charge values
    * @param[in] maximumDischargeRate The maximum rate for discharging into a charging station (vehicle-to-grid)
    * @param[in] minimumChargeLevel The charge level which is kept when discharging into a charging station
    * @param[in] minimumSoC The lower state-of-charge floor in [0, 1] kept when discharging into a charging station
    * @param[in] maximumSoC The upper state-of-charge ceiling in [0, 1] for charging at a charging station
    * @param[in] stateOfHealth The battery state of health in (0, 1] scaling maximum capacity
    * @param[in] capacityLossPerCycle The fractional state-of-health loss per equivalent full cycle
    */
    MSDevice_Battery(SUMOVehicle& holder, const std::string& id, const double actualBatteryCapacity, const double maximumBatteryCapacity,
                     const double stoppingThreshold, const double maximumChargeRate, const std::string& chargeLevelTable, const std::string& chargeCurveTable,
                     const double maximumDischargeRate, const double minimumChargeLevel,
                     const double minimumSoC, const double maximumSoC, const double stateOfHealth, const double capacityLossPerCycle);

public:
    /// @brief Get the actual vehicle's Battery Capacity in Wh
    double getActualBatteryCapacity() const;

    /// @brief Get the total vehicle's Battery Capacity in Wh
    double getMaximumBatteryCapacity() const;

    /// @brief Get the effective vehicle's Battery Capacity in Wh accounting for state of health
    double getEffectiveBatteryCapacity() const {
        return myMaximumBatteryCapacity * myStateOfHealth;
    }

    /// @brief Get the maximum power when accelerating
    double getMaximumPower() const;

    /// @brief Get true if Vehicle is charging, false if not.
    bool isChargingStopped() const;

    /// @brief Get true if Vehicle it's charging, false if not.
    bool isChargingInTransit() const;

    /// @brief Get charging start time.
    SUMOTime getChargingStartTime() const;

    /// @brief Estimate the charging duration given the current battery state
    SUMOTime estimateChargingDuration(const double toCharge, const double csPower) const;

    /// @brief Get consum
    double getConsum() const;

    /// @brief Get total consumption
    double getTotalConsumption() const;

    /// @brief Get total regenerated
    double getTotalRegenerated() const;

    /// @brief Get current Charging Station ID
    std::string getChargingStationID() const;

    MSChargingStation* getChargingStation() const {
        return myActChargingStation;
    }

    /// @brief Get charged energy
    double getEnergyCharged() const;

    /// @brief Get the total energy in Wh charged from charging stations
    double getTotalEnergyCharged() const {
        return myTotalEnergyCharged;
    }

    /// @brief Get the energy in Wh discharged into a charging station in the last step (vehicle-to-grid)
    double getEnergyDischarged() const {
        return myEnergyDischarged;
    }

    /// @brief Get the total energy in Wh discharged into charging stations (vehicle-to-grid)
    double getTotalEnergyDischarged() const {
        return myTotalEnergyDischarged;
    }

    /// @brief Get the maximum discharge rate in W (vehicle-to-grid, 0 = no discharging)
    double getMaximumDischargeRate() const {
        return myMaximumDischargeRate;
    }

    /// @brief Get the charge level in Wh which is kept when discharging (vehicle-to-grid)
    double getMinimumChargeLevel() const {
        return myMinimumChargeLevel;
    }

    /// @brief Get the BMS lower state-of-charge floor in [0, 1] for station V2G discharging
    double getMinimumSoC() const {
        return myMinimumSoC;
    }

    /// @brief Get the BMS upper state-of-charge ceiling in [0, 1] for station charging
    double getMaximumSoC() const {
        return myMaximumSoC;
    }

    /// @brief Get the battery state of health in (0, 1]
    double getStateOfHealth() const {
        return myStateOfHealth;
    }

    /// @brief Get the fractional SOH capacity fade per equivalent full cycle
    double getCapacityLossPerCycle() const {
        return myCapacityLossPerCycle;
    }

    /// @brief Get cumulative equivalent full cycles across driving, regeneration, charging, and V2G discharging
    double getEquivalentFullCycles() const;

    /// @brief Whether the battery exchanges energy bidirectionally with the given charging station (vehicle-to-grid)
    bool allowsDischarge(const MSChargingStation* cs) const;

    /// @brief Get number of timestep that vehicle is stopped
    int getVehicleStopped() const;

    /// @brief Get stopping threshold
    double getStoppingThreshold() const;

    /// @brief Get current charge rate in W depending on the state of charge
    double getMaximumChargeRate() const;

    /// @brief Whether the battery device is actually used as a tank of a combustion vehicle
    bool tracksFuel() const;

    /// @brief Get the charge type
    MSChargingStation::ChargeType getChargeType() const;

    /// @brief Set actual vehicle's Battery Capacity in kWh
    void setActualBatteryCapacity(const double actualBatteryCapacity);

    /// @brief Set total vehicle's Battery Capacity in kWh
    void setMaximumBatteryCapacity(const double maximumBatteryCapacity);

    /// @brief Set vehicle's stopping threshold
    void setStoppingThreshold(const double stoppingThreshold);

    /// @brief Set vehicle's stopping threshold
    void setMaximumChargeRate(const double chargeRate);

    /// @brief Set the maximum discharge rate in W (vehicle-to-grid, 0 = no discharging)
    void setMaximumDischargeRate(const double dischargeRate);

    /// @brief Set the charge level in Wh which is kept when discharging (vehicle-to-grid)
    void setMinimumChargeLevel(const double chargeLevel);

    /// @brief Set the BMS lower state-of-charge floor in [0, 1] for station V2G discharging
    void setMinimumSoC(const double minimumSoC);

    /// @brief Set the BMS upper state-of-charge ceiling in [0, 1] for station charging
    void setMaximumSoC(const double maximumSoC);

    /// @brief Set the battery state of health in (0, 1]
    void setStateOfHealth(const double stateOfHealth);

    /// @brief Set the fractional SOH capacity fade per equivalent full cycle
    void setCapacityLossPerCycle(const double capacityLossPerCycle);

    /// @brief Set (temporary) charge limit
    void setChargeLimit(const double limit);

    /// @brief Reset charging start time
    void resetChargingStartTime();

    /// @brief Increase Charging Start time
    void increaseChargingStartTime();

    /// @brief Reset myVehicleStopped
    void resetVehicleStoppedTimer();

    /// @brief Increase myVehicleStopped
    void increaseVehicleStoppedTimer();

    /// @brief Set Energy Charged in Wh
    void setEnergyCharged(const double energyCharged);

protected:
    /// @brief Read device parameters from input
    static double readParameterValue(SUMOVehicle& v, const SumoXMLAttr& attr, const std::string& paramName, double defaultVal);

    /// @brief Update battery state of health for cycled energy throughput in Wh
    void updateStateOfHealth(const double deltaThroughputWh);

    /// @brief Parameter, The actual vehicles's Battery Capacity in Wh, [myActualBatteryCapacity <= myMaximumBatteryCapacity]
    double myActualBatteryCapacity;

    /// @brief Parameter, The total vehicles's Battery Capacity in Wh, [myMaximumBatteryCapacity >= 0]
    double myMaximumBatteryCapacity;

    /// @brief Parameter, stopping vehicle threshold [myStoppingThreshold >= 0]
    double myStoppingThreshold;

    /// @brief Parameter, maximum charge rate in W
    double myMaximumChargeRate;

    /// @brief (Temporary) limitation in W of the maximum charge rate = charging strategy result
    double myChargeLimit;

    /// @brief Parameter, Vehicle's last angle
    double myLastAngle;

    /// @brief Parameter, Flag: Vehicles it's charging stopped (by default is false)
    bool myChargingStopped;

    /// @brief Parameter, Flag: Vehicles it's charging in transit (by default is false)
    bool myChargingInTransit;

    /// @brief Parameter, Moment, which the vehicle has beging to charging
    SUMOTime myChargingStartTime;

    /// @brief Parameter, Vehicle consum during a time step (by default is 0.)
    double myConsum;

    /// @brief Parameter, total vehicle energy consumption
    double myTotalConsumption;

    /// @brief Parameter, total vehicle energy regeneration
    double myTotalRegenerated;

    /// @brief Charge curve data points storage
    LinearApproxHelpers::LinearApproxMap myChargeCurve;

    /// @brief Parameter, Pointer to current charging station in which vehicle is placed (by default is NULL)
    MSChargingStation* myActChargingStation;

    /// @brief Parameter, Pointer to charging station neighbouring with myActChargingStation in which vehicle was placed previously (by default is NULL), i.e. auxiliar pointer for disabling charging vehicle from previous (not current) ChargingStation (if there is no gap between two different chargingStations)
    MSChargingStation* myPreviousNeighbouringChargingStation;

    /// @brief Parameter, Energy charged in each timestep
    double myEnergyCharged;

    /// @brief Total energy charged from charging stations
    double myTotalEnergyCharged;

    /// @brief Parameter, maximum rate in W for discharging into a charging station (vehicle-to-grid, 0 = no discharging)
    double myMaximumDischargeRate;

    /// @brief Parameter, charge level in Wh which is kept when discharging into a charging station
    double myMinimumChargeLevel;

    /// @brief Parameter, BMS lower state-of-charge floor in [0, 1] for station V2G discharging
    double myMinimumSoC;

    /// @brief Parameter, BMS upper state-of-charge ceiling in [0, 1] for station charging
    double myMaximumSoC;

    /// @brief Parameter, battery state of health in (0, 1]
    double myStateOfHealth;

    /// @brief Parameter, fractional SOH loss per equivalent full cycle (>= 0)
    double myCapacityLossPerCycle;

    /// @brief Energy discharged into a charging station in each timestep
    double myEnergyDischarged;

    /// @brief Total energy discharged into charging stations
    double myTotalEnergyDischarged;

    /// @brief Preserve V2G parameters explicitly set at runtime or restored from state.
    bool mySaveV2GState = false;

    /// @brief Parameter, How many timestep the vehicle is stopped
    int myVehicleStopped;

    /// @brief Count how many times the vehicle experienced a depleted battery
    int myDepletedCount;

    /// @brief whether to track fuel consumption instead of electricity
    bool myTrackFuel;

    /// @brief the accepted charge type
    MSChargingStation::ChargeType myChargeType;


private:
    /// @brief Invalidated copy constructor.
    MSDevice_Battery(const MSDevice_Battery&);

    /// @brief Invalidated assignment operator.
    MSDevice_Battery& operator=(const MSDevice_Battery&);
};
