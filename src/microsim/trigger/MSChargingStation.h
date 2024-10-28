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
/// @file    MSChargingStation.h
/// @author  Daniel Krajzewicz
/// @author  Tamas Kurczveil
/// @author  Pablo Alvarez Lopez
/// @date    20-12-13
///
// Charging Station for Electric vehicles
/****************************************************************************/
#pragma once
#include <config.h>

#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <microsim/MSStoppingPlace.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;
class MSBusStop;
class OptionsCont;
class MSDevice_Battery;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ChargingStation
 * @brief Definition of charging stations
 */
class MSChargingStation : public MSStoppingPlace {

public:
    /// @brief constructor
    MSChargingStation(const std::string& chargingStationID, MSLane& lane, double startPos, double endPos,
                      const std::string& name, double chargingPower, double efficency, bool chargeInTransit,
                      SUMOTime chargeDelay, const std::string& chargeType, SUMOTime waitingTime);

    MSChargingStation(const std::string& chargingStationID, const MSParkingArea* parkingArea, const std::string& name, double chargingPower,
                      double efficency, bool chargeInTransit, SUMOTime chargeDelay, const std::string& chargeType,
                      SUMOTime waitingTime);

    /// @brief destructor
    ~MSChargingStation();

    /// @brief Get charging station's charging power
    double getChargingPower(bool usingFuel) const;

    /// @brief Get efficiency of the charging station
    double getEfficency() const;

    /// @brief Get chargeInTransit
    bool getChargeInTransit() const;

    /// @brief Get Charge Delay
    SUMOTime getChargeDelay() const;

    /// @brief Get charge type
    const std::string& getChargeType() const;

    /// @brief Get waiting time
    SUMOTime getWaitingTime() const;

    /** @brief Get the parking area the charging station is placed on
     * @return pointer to the parking area or nullptr
     */
    const MSParkingArea* getParkingArea() const;

    /// @brief set charging station's charging power
    void setChargingPower(double chargingPower);

    /// @brief set efficiency of the charging station
    void setEfficiency(double efficiency);

    /// @brief set charging delay of the charging station
    void setChargeDelay(SUMOTime delay);

    /// @brief set charging in transit
    void setChargeInTransit(bool value);

    /// @brief enable or disable charging vehicle
    void setChargingVehicle(bool value);

    /** @brief Check if a vehicle is inside in  the Charge Station
     * @param[in] position Position of vehicle in the LANE
     * @return true if is between StartPostion and EndPostion
     */
    bool vehicleIsInside(const double position) const;

    /// @brief Return true if in the current time step charging station is charging a vehicle
    bool isCharging() const;

    double getTotalCharged() const {
        return myTotalCharge;
    }

    /// @brief add charge value for output
    void addChargeValueForOutput(double WCharged, MSDevice_Battery* battery);

    /// @brief write charging station values
    void writeChargingStationOutput(OutputDevice& output);

    /// @brief write ungrouped output (flush data after writing)
    void writeAggregatedChargingStationOutput(OutputDevice& output);

protected:

    /// @brief struct to save information for the chargingStation output
    struct Charge {
        /// @brief constructor
        Charge(SUMOTime _timeStep, std::string _vehicleID, std::string _vehicleType, std::string _status,
               double _WCharged, double _actualBatteryCapacity, double _maxBatteryCapacity, double _chargingPower,
               double _chargingEfficiency, double _totalEnergyCharged) :
            timeStep(_timeStep),
            vehicleID(_vehicleID),
            vehicleType(_vehicleType),
            status(_status),
            WCharged(_WCharged),
            actualBatteryCapacity(_actualBatteryCapacity),
            maxBatteryCapacity(_maxBatteryCapacity),
            chargingPower(_chargingPower),
            chargingEfficiency(_chargingEfficiency),
            totalEnergyCharged(_totalEnergyCharged) {}

        // @brief vehicle TimeStep
        SUMOTime timeStep;
        // @brief vehicle ID
        std::string vehicleID;
        // @brief vehicle Type
        std::string vehicleType;
        /// @brief status
        std::string status;
        // @brief W charged
        double WCharged;
        // @brief actual battery capacity AFTER charging
        double actualBatteryCapacity;
        // @brief battery max capacity
        double maxBatteryCapacity;
        // @brief current charging power of charging station
        double chargingPower;
        // @brief current efficiency of charging station
        double chargingEfficiency;
        // @brief current energy charged by charging stations AFTER charging
        double totalEnergyCharged;
    };

    static void writeVehicle(OutputDevice& out, const std::vector<Charge>& chargeSteps, int iStart, int iEnd, double charged);

    /// @brief Charging station's charging power
    double myChargingPower = 0;

    /// @brief Efficiency of the charging station
    double myEfficiency = 0;

    /// @brief Allow charge in transit
    bool myChargeInTransit;

    /// @brief Charge Delay
    SUMOTime myChargeDelay = 0;

    /// @brief charge type
    const std::string myChargeType = "normal";

    /// @brief waiting time
    SUMOTime myWaitingTime = 0;

    /// @brief Check if in the current TimeStep chargingStation is charging a vehicle
    bool myChargingVehicle = false;

    /// @brief total energy charged by this charging station
    double myTotalCharge = 0;

    /// @brief parkingArea the charging station is placed on
    const MSParkingArea* myParkingArea = nullptr;

    /// @brief map with the charges of this charging station (key = vehicleID)
    std::map<std::string, std::vector<Charge> > myChargeValues;
    /// @brief order vehicles by time of first charge
    std::vector<std::string> myChargedVehicles;

private:
    /// @brief Invalidated copy constructor.
    MSChargingStation(const MSChargingStation&) = delete;

    /// @brief Invalidated assignment operator.
    MSChargingStation& operator=(const MSChargingStation&) = delete;
};
