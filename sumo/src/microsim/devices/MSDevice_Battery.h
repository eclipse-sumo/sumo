/****************************************************************************/
/// @file    MSDevice_Battery.h
/// @author  Tamas Kurczveil
/// @author  Pablo Alvarez Lopez
/// @date    20-12-13
/// @version $Id$
///
// The Battery parameters for the vehicle
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2013-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifndef MSDevice_Battery_h
#define MSDevice_Battery_h

#include <microsim/devices/MSDevice.h>
#include <microsim/MSVehicle.h>
#include <microsim/trigger/MSChargingStation.h>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class SUMOVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSDevice_Battery
 * @brief Battery device for electric vehicles
 */
class MSDevice_Battery : public MSDevice {
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
    static void buildVehicleDevices(SUMOVehicle& v, std::vector<MSDevice*>& into);

public:
    /// @brief Destructor.
    ~MSDevice_Battery();

    /// @name Methods called on vehicle movement / state change, overwriting MSDevice
    /// @{
    /** @brief Checks for waiting steps when the vehicle moves
    *
    * @param[in] veh Vehicle that asks this reminder.
    * @param[in] oldPos Position before move.
    * @param[in] newPos Position after move with newSpeed.
    * @param[in] newSpeed Moving speed.
    *
    * @return True (always).
    */
    bool notifyMove(SUMOVehicle& veh, SUMOReal oldPos,  SUMOReal newPos, SUMOReal newSpeed);

    /** @brief Saves departure info on insertion
    *
    * @param[in] veh The entering vehicle.
    * @param[in] reason how the vehicle enters the lane
    * @return Always true
    * @see MSMoveReminder::notifyEnter
    * @see MSMoveReminder::Notification
    */
    bool notifyEnter(SUMOVehicle& veh, MSMoveReminder::Notification reason);
    /// @}

    /** @brief Called on writing tripinfo output
    *
    * @param[in] os The stream to write the information into
    * @exception IOError not yet implemented
    * @see MSDevice::generateOutput
    */
    void generateOutput() const;

private:
    /** @brief Constructor
    *
    * @param[in] holder The vehicle that holds this device
    * @param[in] id The ID of the device
    * @param[in] period The period with which a new route shall be searched
    * @param[in] preInsertionPeriod The route search period before insertion
    */
    MSDevice_Battery(SUMOVehicle& holder, const std::string& id, const SUMOReal actualBatteryCapacity, const SUMOReal maximumBatteryCapacity, const SUMOReal powerMax, const SUMOReal mass, const SUMOReal frontSurfaceArea, const SUMOReal airDragCoefficient, const SUMOReal internalMomentOfInertia, const SUMOReal radialDragCoefficient, const SUMOReal rollDragCoefficient, const SUMOReal constantPowerIntake, const SUMOReal propulsionEfficiency, const SUMOReal recuperationEfficiency, const SUMOReal lastAngle, const SUMOReal lastEnergy);

public:
    /// @brief Get the actual vehicle's Battery Capacity in kWh
    SUMOReal getActualBatteryCapacity() const;

    /// @brief Get the total vehicle's Battery Capacity in kWh
    SUMOReal getMaximumBatteryCapacity() const;

    /// @brief Get the maximum power when accelerating
    SUMOReal getMaximumPower() const;

    /// @brief Get vehicle's mass
    SUMOReal getMass() const;

    /// @brief Get vehicle's front surface Area
    SUMOReal getFrontSurfaceArea() const;

    /// @brief Get vehicle's drag coefficient
    SUMOReal getAirDragCoefficient() const;

    /// @brief Get vehicles's internal moment of inertia
    SUMOReal getInternalMomentOfInertia() const;

    /// @brief Get vehicles's radial friction coefficient
    SUMOReal getRadialDragCoefficient() const;

    /// @brief Get vehicles's roll friction coefficient
    SUMOReal getRollDragCoefficient() const;

    /// @brief Get vehicles's constant power intake
    SUMOReal getConstantPowerIntake() const;

    /// @brief Get vehicles's Propulsion efficiency
    SUMOReal getPropulsionEfficiency() const;

    /// @brief Get vehicles's Recuparation efficiency
    SUMOReal getRecuperationEfficiency() const;

    /// @brief Get vehicles's last angle
    SUMOReal getLastAngle() const;

    /// @brief Get vehicles's last Energy
    SUMOReal getLastEnergy() const;

    /// @brief Get true if Vehicle is charging, false if not.
    bool isChargingStopped() const;

    /// @brief Get true if Vehicle it's charging, false if not.
    bool isChargingInTransit() const;

    /// @brief Get charging start time.
    SUMOReal getChargingStartTime() const;

    /// @brief Get consum
    SUMOReal getConsum() const;

    /// @brief Get current Charging Station ID
    std::string getChargingStationID() const;

    /// @brief Get charged energy
    SUMOReal getEnergyCharged() const;

    /// @brief Get number of timestep that vehicle is stopped
    int getVehicleStopped() const;

    /// @brief get propulsion energy
    SUMOReal getPropEnergy(SUMOVehicle& veh);

    /// @brief Set actual vehicle's Battery Capacity in kWh
    void setActualBatteryCapacity(const SUMOReal actualBatteryCapacity);

    /// @brief Set total vehicle's Battery Capacity in kWh
    void setMaximumBatteryCapacity(const SUMOReal maximumBatteryCapacity);

    /// @brief Set maximum power when accelerating
    void setPowerMax(const SUMOReal new_Pmax);

    /// @brief Set vehicle's myMass
    void setMass(const SUMOReal mass);

    /// @brief Set vehicle's front surface Area
    void setFrontSurfaceArea(const SUMOReal frontSurfaceArea);

    /// @brief Set vehicle's drag coefficient
    void setAirDragCoefficient(const SUMOReal new_C_Veh);

    /// @brief Set vehicle's internal moment of inertia
    void setInternalMomentOfInertia(const SUMOReal internalMomentOfInertia);

    /// @brief Set vehicle's radial friction coefficient
    void setRadialDragCoefficient(const SUMOReal radialDragCoefficient);

    /// @brief Set vehicle's roll friction coefficient
    void setRollDragCoefficient(const SUMOReal rollDragCoefficient);

    /// @brief Set vehicle's constant power intake
    void setConstantPowerIntake(const SUMOReal constantPowerIntake);

    /// @brief Set vehicle's Propulsion efficiency
    void setPropulsionEfficiency(const SUMOReal propulsionEfficiency);

    /// @brief Set vehicle's Recuparation efficiency
    void setRecuperationEfficiency(const SUMOReal recuperationEfficiency);

    /// @brief Set vehicle's last Angle
    void setLastAngle(const SUMOReal lastAngle);

    /// @brief Set vehicle's last Energy
    void setLastEnergy(const SUMOReal lastEnergy);

    /// @brief Reset charging start time
    void resetChargingStartTime();

    /// @brief Increase Charging Start time
    void increaseChargingStartTime();

    /// @brief Reset myVehicleStopped
    void resetVehicleStoppedTimer();

    /// @brief Increase myVehicleStopped
    void increaseVehicleStoppedTimer();

protected:
    /// @brief Parameter, The actual vehicles's Battery Capacity in kWh
    /// @condition [myActualBatteryCapacity <= myMaximumBatteryCapacity]
    SUMOReal myActualBatteryCapacity;

    /// @brief Parameter, The total vehicles's Battery Capacity in kWh
    /// @condition [myMaximumBatteryCapacity >= 0]
    SUMOReal myMaximumBatteryCapacity;

    /// @brief Parameter, The Maximum Power when accelerating
    /// @condition [myPowerMax >= 0]
    SUMOReal myPowerMax;

    /// @brief Parameter, Vehicle's myMass
    /// @condition [myMass >= 0]
    SUMOReal myMass;

    /// @brief Parameter, Vehicle's front surface Area
    /// @condition [myFrontSurfaceArea >= 0]
    SUMOReal myFrontSurfaceArea;

    /// @brief Parameter, Vehicle's drag coefficient
    /// @condition [myAirDragCoefficient >=0]
    SUMOReal myAirDragCoefficient;

    /// @brief Parameter, Vehicle's internal moment of inertia
    /// @condition [myInternalMomentOfInertia >= 0]
    SUMOReal myInternalMomentOfInertia;

    /// @brief Parameter, Vehicle's radial friction coefficient
    /// @condition [myRadialDragCoefficient >=0]
    SUMOReal myRadialDragCoefficient;

    /// @brief Parameter, Vehicle's roll friction coefficient
    /// @condition [myRollDragCoefficient >= 0]
    SUMOReal myRollDragCoefficient;

    /// @brief Parameter, Vehicle's constant power intake
    /// @condition [myConstantPowerIntake >= 0]
    SUMOReal myConstantPowerIntake;

    /// @brief Parameter, Vehicle's propulsion efficiency
    /// @condition [1 >= myPropulsionEfficiency >= 0]
    SUMOReal myPropulsionEfficiency;

    /// @brief Parameter, Vehicle's recuparation efficiency
    /// @condition [1 >= myRecuperationEfficiency >= 0]
    SUMOReal myRecuperationEfficiency;

    /// @brief Parameter, Vehicle's last angle
    /// @condition none
    SUMOReal myLastAngle;

    /// @brief Parameter, Vehicle's last energy
    /// @condition none
    SUMOReal myLastEnergy;

    /// @brief Parameter, Flag: Vehicles it's charging stopped
    /// @condition by default is false.
    bool myChargingStopped;

    /// @brief Parameter, Flag: Vehicles it's charging in transit
    /// @condition by default is false.
    bool myChargingInTransit;

    /// @brief Parameter, Moment, wich the vehicle has beging to charging
    /// @condition none
    SUMOReal myChargingStartTime;

    /// @brief Parameter, Vehicle consum during a time step
    /// @condition by default is 0.
    SUMOReal myConsum;

    /// @brief Parameter, Pointer to current charging station in which vehicle is placed
    /// @condition by default is NULL.
    MSChargingStation* myActChargingStation;

    /// @brief Parameter, Energy charged in each timestep
    SUMOReal myEnergyCharged;

    /// @brief Parameter, How many timestep the vehicle is stopped
    int myVehicleStopped;

private:
    /// @brief Invalidated copy constructor.
    MSDevice_Battery(const MSDevice_Battery&);

    /// @brief Invalidated assignment operator.
    MSDevice_Battery& operator=(const MSDevice_Battery&);
};

#endif

