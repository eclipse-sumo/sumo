/****************************************************************************/
/// @file    MSChargingStation.h
/// @author  Daniel Krajzewicz
/// @author  Tamas Kurczveil
/// @author  Pablo Alvarez Lopez
/// @date    20-12-13
/// @version $Id$
///
// Chargin Station for Electric vehicles
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSChargingStation_h
#define MSChargingStation_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

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
    MSChargingStation(const std::string& chargingStationID, MSLane& lane, SUMOReal startPos, SUMOReal endPos,
                      SUMOReal chargingPower, SUMOReal efficency, bool chargeInTransit, int chargeDelay);

    /// @brief destructor
    ~MSChargingStation();

    /// @brief Get charging station's charging power
    SUMOReal getChargingPower() const;

    /// @brief Get efficiency of the charging station
    SUMOReal getEfficency() const;

    /// @brief Get chargeInTransit
    bool getChargeInTransit() const;

    /// @brief Get Charge Delay
    SUMOReal getChargeDelay() const;

    /// @brief Set charging station's charging power
    void setChargingPower(SUMOReal chargingPower);

    /// @brief Set efficiency of the charging station
    void setEfficency(SUMOReal efficency);

    /// @brief Set charge in transit of the charging station
    void setChargeInTransit(bool chargeInTransit);

    /// @brief Set charge delay of the charging station
    void setChargeDelay(int chargeDelay);

    /// @brief enable or disable charging vehicle
    void setChargingVehicle(bool value);

    /** @brief Check if a vehicle is inside in  the Charge Station
     * @param[in] position Position of vehicle in the LANE
     * @return true if is between StartPostion and EndPostion
     */
    bool vehicleIsInside(const SUMOReal position) const;

    /// @brief Return true if in the current time step charging station is charging a vehicle
    bool isCharging() const;

protected:

    /// @brief Charging station's charging power
    SUMOReal myChargingPower;

    /// @brief Efficiency of the charging station
    SUMOReal myEfficiency;

    /// @brief Allow charge in transit
    bool myChargeInTransit;

    /// @brief Charge Delay
    int myChargeDelay;

    /// @brief Check if in the current TimeStep chargingStation is charging a vehicle
    bool myChargingVehicle;

private:

    /// @brief Invalidated copy constructor.
    MSChargingStation(const MSChargingStation&);

    /// @brief Invalidated assignment operator.
    MSChargingStation& operator=(const MSChargingStation&);
};

#endif
