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
    bool notifyMove(SUMOVehicle& veh, double oldPos,  double newPos, double newSpeed);

    /** @brief Saves departure info on insertion
    *
    * @param[in] veh The entering vehicle.
    * @param[in] reason how the vehicle enters the lane
    * @return Always true
    * @see MSMoveReminder::notifyEnter
    * @see MSMoveReminder::Notification
    */
    bool notifyEnter(SUMOVehicle& veh, MSMoveReminder::Notification reason, const MSLane* enteredLane = 0);
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
    MSDevice_Battery(SUMOVehicle& holder, const std::string& id, const double actualBatteryCapacity, const double maximumBatteryCapacity, 
                     const double powerMax, const double mass, const double frontSurfaceArea, const double airDragCoefficient, 
                     const double internalMomentOfInertia, const double radialDragCoefficient, const double rollDragCoefficient, 
                     const double constantPowerIntake, const double propulsionEfficiency, const double recuperationEfficiency, 
                     const double stoppingTreshold, const double lastAngle, const double lastEnergy);

public:
    /// @brief Get the actual vehicle's Battery Capacity in kWh
    double getActualBatteryCapacity() const;

    /// @brief Get the total vehicle's Battery Capacity in kWh
    double getMaximumBatteryCapacity() const;

    /// @brief Get the maximum power when accelerating
    double getMaximumPower() const;

    /// @brief Get vehicle's mass
    double getMass() const;

    /// @brief Get vehicle's front surface Area
    double getFrontSurfaceArea() const;

    /// @brief Get vehicle's drag coefficient
    double getAirDragCoefficient() const;

    /// @brief Get vehicles's internal moment of inertia
    double getInternalMomentOfInertia() const;

    /// @brief Get vehicles's radial friction coefficient
    double getRadialDragCoefficient() const;

    /// @brief Get vehicles's roll friction coefficient
    double getRollDragCoefficient() const;

    /// @brief Get vehicles's constant power intake
    double getConstantPowerIntake() const;

    /// @brief Get vehicles's Propulsion efficiency
    double getPropulsionEfficiency() const;

    /// @brief Get vehicles's Recuparation efficiency
    double getRecuperationEfficiency() const;

    /// @brief Get vehicles's last angle
    double getLastAngle() const;

    /// @brief Get vehicles's last Energy
    double getLastEnergy() const;

    /// @brief Get true if Vehicle is charging, false if not.
    bool isChargingStopped() const;

    /// @brief Get true if Vehicle it's charging, false if not.
    bool isChargingInTransit() const;

    /// @brief Get charging start time.
    double getChargingStartTime() const;

    /// @brief Get consum
    double getConsum() const;

    /// @brief Get current Charging Station ID
    std::string getChargingStationID() const;

    /// @brief Get charged energy
    double getEnergyCharged() const;

    /// @brief Get number of timestep that vehicle is stopped
    int getVehicleStopped() const;

    /// @brief Get stopping treshold
    double getStoppingTreshold() const;

    /// @brief get propulsion energy
    double getPropEnergy(SUMOVehicle& veh);

    /// @brief Set actual vehicle's Battery Capacity in kWh
    void setActualBatteryCapacity(const double actualBatteryCapacity);

    /// @brief Set total vehicle's Battery Capacity in kWh
    void setMaximumBatteryCapacity(const double maximumBatteryCapacity);

    /// @brief Set maximum power when accelerating
    void setPowerMax(const double new_Pmax);

    /// @brief Set vehicle's myMass
    void setMass(const double mass);

    /// @brief Set vehicle's front surface Area
    void setFrontSurfaceArea(const double frontSurfaceArea);

    /// @brief Set vehicle's drag coefficient
    void setAirDragCoefficient(const double new_C_Veh);

    /// @brief Set vehicle's internal moment of inertia
    void setInternalMomentOfInertia(const double internalMomentOfInertia);

    /// @brief Set vehicle's radial friction coefficient
    void setRadialDragCoefficient(const double radialDragCoefficient);

    /// @brief Set vehicle's roll friction coefficient
    void setRollDragCoefficient(const double rollDragCoefficient);

    /// @brief Set vehicle's constant power intake
    void setConstantPowerIntake(const double constantPowerIntake);

    /// @brief Set vehicle's Propulsion efficiency
    void setPropulsionEfficiency(const double propulsionEfficiency);

    /// @brief Set vehicle's Recuparation efficiency
    void setRecuperationEfficiency(const double recuperationEfficiency);

    /// @brief Set vehicle's last Angle
    void setLastAngle(const double lastAngle);

    /// @brief Set vehicle's last Energy
    void setLastEnergy(const double lastEnergy);

    /// @brief Set vehicle's stopping treshold
    void setStoppingTreshold(const double stoppingTreshold);

    /// @brief Reset charging start time
    void resetChargingStartTime();

    /// @brief Increase Charging Start time
    void increaseChargingStartTime();

    /// @brief Reset myVehicleStopped
    void resetVehicleStoppedTimer();

    /// @brief Increase myVehicleStopped
    void increaseVehicleStoppedTimer();

protected:
    /// @brief Parameter, The actual vehicles's Battery Capacity in kWh, [myActualBatteryCapacity <= myMaximumBatteryCapacity]
    double myActualBatteryCapacity;

    /// @brief Parameter, The total vehicles's Battery Capacity in kWh, [myMaximumBatteryCapacity >= 0]
    double myMaximumBatteryCapacity;

    /// @brief Parameter, The Maximum Power when accelerating, [myPowerMax >= 0]
    double myPowerMax;

    /// @brief Parameter, Vehicle's myMass, [myMass >= 0]
    double myMass;

    /// @brief Parameter, Vehicle's front surface Area, [myFrontSurfaceArea >= 0]
    double myFrontSurfaceArea;

    /// @brief Parameter, Vehicle's drag coefficient, [myAirDragCoefficient >=0]
    double myAirDragCoefficient;

    /// @brief Parameter, Vehicle's internal moment of inertia, [myInternalMomentOfInertia >= 0]
    double myInternalMomentOfInertia;

    /// @brief Parameter, Vehicle's radial friction coefficient, [myRadialDragCoefficient >=0]
    double myRadialDragCoefficient;

    /// @brief Parameter, Vehicle's roll friction coefficient, [myRollDragCoefficient >= 0]
    double myRollDragCoefficient;

    /// @brief Parameter, Vehicle's constant power intake, [myConstantPowerIntake >= 0]
    double myConstantPowerIntake;

    /// @brief Parameter, Vehicle's propulsion efficiency, [1 >= myPropulsionEfficiency >= 0]
    double myPropulsionEfficiency;

    /// @brief Parameter, Vehicle's recuparation efficiency, [1 >= myRecuperationEfficiency >= 0]
    double myRecuperationEfficiency;

    /// @brief Parameter, Vehicle's last angle
    double myLastAngle;

    /// @brief Parameter, Vehicle's last energy
    double myLastEnergy;

    /// @brief Parameter, Flag: Vehicles it's charging stopped (by default is false)
    bool myChargingStopped;

    /// @brief Parameter, Flag: Vehicles it's charging in transit (by default is false)
    bool myChargingInTransit;

    /// @brief Parameter, Moment, wich the vehicle has beging to charging
    double myChargingStartTime;

    /// @brief Parameter, Vehicle consum during a time step (by default is 0.)
    double myConsum;

    /// @brief Parameter, Pointer to current charging station in which vehicle is placed (by default is NULL)
    MSChargingStation* myActChargingStation;

    /// @brief Parameter, Energy charged in each timestep
    double myEnergyCharged;

    /// @brief Parameter, How many timestep the vehicle is stopped
    int myVehicleStopped;

    /// @brief Parameter, stopping vehicle treshold [myStoppingTreshold >= 0]
    double myStoppingTreshold;

private:
    /// @brief Invalidated copy constructor.
    MSDevice_Battery(const MSDevice_Battery&);

    /// @brief Invalidated assignment operator.
    MSDevice_Battery& operator=(const MSDevice_Battery&);
};

#endif

