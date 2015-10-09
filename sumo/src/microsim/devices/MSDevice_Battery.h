/****************************************************************************/
/// @file    MSDevice_Battery.h
/// @author  Tamas Kurczveil
/// @author  Pablo Alvarez López
/// @date    20-12-13
/// @version $Id$
///
// The Battery parameters for the vehicle
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2013-2015 DLR (http://www.dlr.de/) and contributors
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
#include <microsim/trigger/MSChrgStn.h>
#include <utils/common/SUMOTime.h>
#include <iostream>
#include <sstream>
#include <string>


// ===========================================================================
// class declarations
// ===========================================================================
class SUMOVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSDevice_Battery
 * @brief The battery parametereter
 *
 * MSDevice_Battery //COMPLETAR

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

    /** @brief Checks for waiting steps when the vehicle moves     // IMPLEMENTADA!!
    *
    * @param[in] veh Vehicle that asks this reminder.
    * @param[in] oldPos Position before move.
    * @param[in] newPos Position after move with newSpeed.
    * @param[in] newSpeed Moving speed.
    *
    * @return True (always).
    */
    bool notifyMove(SUMOVehicle& veh, SUMOReal oldPos,
                    SUMOReal newPos, SUMOReal newSpeed);


    /** @brief Saves departure info on insertion
    *
    * @param[in] veh The entering vehicle.
    * @param[in] reason how the vehicle enters the lane
    * @return Always true
    * @see MSMoveReminder::notifyEnter
    * @see MSMoveReminder::Notification
    */
    bool notifyEnter(SUMOVehicle& veh, MSMoveReminder::Notification reason);


    /** @brief Saves arrival info
    *
    * @param[in] veh The leaving vehicle.
    * @param[in] lastPos Position on the lane when leaving.
    * @param[in] isArrival whether the vehicle arrived at its destination
    * @param[in] isLaneChange whether the vehicle changed from the lane
    * @return True if it did not leave the net.
    *
    bool notifyLeave(SUMOVehicle& veh, SUMOReal lastPos,
                 MSMoveReminder::Notification reason);
    /// @}

    */

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
    MSDevice_Battery(SUMOVehicle& holder, const std::string& id, const SUMOReal new_ActBatKap, const SUMOReal new_MaxBatKap, const SUMOReal new_PowerMax, const SUMOReal new_Mass, const SUMOReal new_FrontSurfaceArea, const SUMOReal new_AirDragCoefficient, const SUMOReal new_InternalMomentOfInertia, const SUMOReal new_RadialDragCoefficient, const SUMOReal new_RollDragCoefficient, const SUMOReal new_ConstantPowerIntake, const SUMOReal new_PropulsionEfficiency, const SUMOReal new_RecuperationEfficiency, const SUMOReal new_LastAngle, const SUMOReal new_LastEnergy);


public:
    // GET FUNCTIONS

    /// @brief Get parameter 01, the actual vehicles' Battery Capacity in kWh
    SUMOReal getActBatKap() const;

    /// @brief Get parameter 02, the total vehicles' Battery Capacity in kWh
    SUMOReal getMaxBatKap() const;

    /// @brief Get parameter 03, the maximum power when accelerating
    inline SUMOReal getPowerMax() const;

    /// @brief Get parameter 04, vehicle's Mass
    inline SUMOReal getMass() const;

    /// @brief Get parameter 05, vehicle's front surface Area
    inline SUMOReal getFrontSurfaceArea() const;

    /// @brief Get parameter 06, vehicle's drag coefficient
    inline SUMOReal getAirDragCoefficient() const;

    /// @brief Get parameter 07, vehicles' internal moment of inertia
    inline SUMOReal getInternalMomentOfInertia() const;

    /// @brief Get parameter 08, vehicles' radial friction coefficient
    inline SUMOReal getRadialDragCoefficient() const;

    /// @brief Get parameter 09, vehicles' roll friction coefficient
    inline SUMOReal getRollDragCoefficient() const;

    /// @brief Get parameter 10, vehicles' constant power intake
    inline SUMOReal getConstantPowerIntake() const;

    /// @brief Get parameter 11, vehicles' Propulsion efficiency
    inline SUMOReal getPropulsionEfficiency() const;

    /// @brief Get parameter 12, vehicles' Recuparation efficiency
    inline SUMOReal getRecuperationEfficiency() const;

    /// @brief Get parameter 13, vehicles' last angle
    inline SUMOReal getLastAngle() const;

    /// @brief Get parameter 14, vehicles' last Energy
    inline SUMOReal getLastEnergy() const;

    /// @brief Get parameter 15, true if Vehicle it's charging, false if not.
    bool isChargingStopped() const;

    /// @brief Get parameter 16, true if Vehicle it's charging, false if not.
    bool isChargingInTransit() const;

    /// @brief Get parameter 17, charging start time.
    inline SUMOReal getChargingStartTime() const;

    /// @brief Get parameter 18, consum (Strange error with linker)
    SUMOReal getConsum() const;

    /// @brief Get parameter 19, Charging Station I
    const std::string& getChrgStnID() const;

    /// @brief Get parameter 20, charging energy
    SUMOReal getChrgEnergy() const;

    /// @brief Get parameter 20, charging energy
    int getVehicleStopped() const;

    /// @brief get propulsion energy
    SUMOReal getPropEnergy(SUMOVehicle& veh);


    // SET FUNCTIONS

    /// @brief Set parameter 01, The actual vehicles' Battery Capacity in kWh
    inline void setActBatKap(const SUMOReal new_ActBatKap);

    /// @brief Set parameter 02, The total vehicles' Battery Capacity in kWh
    inline void setMaxBatKap(const SUMOReal new_MaxBatKap);

    /// @brief Set parameter 03, The maximum power when accelerating
    inline void setPowerMax(const SUMOReal new_Pmax);

    /// @brief Set parameter 04, vehicle's Mass
    inline void setMass(const SUMOReal new_Mass);

    /// @brief Set parameter 05, vehicle's front surface Area
    inline void setFrontSurfaceArea(const SUMOReal new_FrontSurfaceArea);

    /// @brief Set parameter 06, vehicle's drag coefficient
    inline void setAirDragCoefficient(const SUMOReal new_C_Veh);

    /// @brief Set parameter 07, vehicles' internal moment of inertia
    inline void setInternalMomentOfInertia(const SUMOReal new_InternalMomentOfInertia);

    /// @brief Set parameter 08, Vehicles' radial friction coefficient
    inline void setRadialDragCoefficient(const SUMOReal new_RadialDragCoefficient);

    /// @brief Set parameter 09, vehicles' roll friction coefficient
    inline void setRollDragCoefficient(const SUMOReal new_RollDragCoefficient);

    /// @brief Set parameter 10, vehicles' constant power intake
    inline void setConstantPowerIntake(const SUMOReal new_ConstantPowerIntake);

    /// @brief Set parameter 11, vehicles' Propulsion efficiency
    inline void setPropulsionEfficiency(const SUMOReal new_PropulsionEfficiency);

    /// @brief Set parameter 12, vehicles' Recuparation efficiency
    inline void setRecuperationEfficiency(const SUMOReal new_RecuperationEfficiency);

    /// @brief Set parameter 13, vehicles' last Angle
    inline void setLastAngle(const SUMOReal new_LastAngle);

    /// @brief Set parameter 14, vehicles' last Energy
    inline void setLastEnergy(const SUMOReal new_LastEnergy);

    /// @brief Set parameter 17a, reset charging start time
    inline void resetChargingStartTime();

    /// @brief Set parameter 17b, increase Charging Start time
    inline void increaseChargingStartTime();

    /// @brief Set parameter 21a, reset vehicleStopped
    inline void resetVehicleStoppedTimer();

    /// @brief Set parameter 21a, increase vehicleStopped
    inline void increaseVehicleStoppedTimer();







private:

    /// @brief Parameter 01, The actual vehicles' Battery Capacity in kWh
    /// @condition [ActBatKap <= MaxBatKap]
    SUMOReal ActBatKap;

    /// @brief Parameter 02, The total vehicles' Battery Capacity in kWh
    /// @condition [MaxBatKap >= 0]
    SUMOReal MaxBatKap;

    /// @brief Parameter 03, The Maximum Power when accelerating
    /// @condition [PowerMax >= 0]
    SUMOReal PowerMax;

    /// @brief Parameter 04, Vehicle's Mass
    /// @condition [Mass >= 0]
    SUMOReal Mass;

    /// @brief Parameter 05, Vehicle's front surface Area
    /// @condition [FrontSurfaceArea >= 0]
    SUMOReal FrontSurfaceArea;

    /// @brief Parameter 06, Vehicle's drag coefficient
    /// @condition [AirDragCoefficient >=0]
    SUMOReal AirDragCoefficient;

    /// @brief Parameter 07, Vehicles' internal moment of inertia
    /// @condition [InternalMomentOfInertia >= 0]
    SUMOReal InternalMomentOfInertia;

    /// @brief Parameter 08, Vehicles' radial friction coefficient
    /// @condition [RadialDragCoefficient >=0]
    SUMOReal RadialDragCoefficient;

    /// @brief Parameter 09, Vehicles' roll friction coefficient
    /// @condition [RollDragCoefficient >= 0]
    SUMOReal RollDragCoefficient;

    /// @brief Parameter 10, Vehicles' constant power intake
    /// @condition [ConstantPowerIntake >= 0]
    SUMOReal ConstantPowerIntake;

    /// @brief Parameter 11, Vehicles' propulsion efficiency
    /// @condition [1 >= PropulsionEfficiency >= 0]
    SUMOReal PropulsionEfficiency;

    /// @brief Parameter 12, Vehicles' recuparation efficiency
    /// @condition [1 >= RecuperationEfficiency >= 0]
    SUMOReal RecuperationEfficiency;

    /// @brief Parameter 13, Vehicles' last angle
    /// @condition none
    SUMOReal LastAngle;

    /// @brief Parameter 14, Vehicles' last energy
    /// @condition none
    SUMOReal LastEnergy;

    /// @brief Parameter 15, Flag: Vehicles it's charging stopped
    /// @condition by default is false.
    bool ItsChargingStopped;

    /// @brief Parameter 16, Flag: Vehicles it's charging in transit
    /// @condition by default is false.
    bool ItsChargingInTransit;

    /// @brief Parameter 17, Moment, wich the vehicle has beging to charging
    /// @condition none
    SUMOReal ChargingStartTime;

    /// @brief Parameter 18, Vehicle consum during a time step
    /// @condition by default is 0.
    SUMOReal Consum;

    /// @brief Parameter 19, Vehicle consum during a time step
    /// @condition by default is "".
    std::string actChrgStn;

    /// @brief Parameter 20, Energy charged in each timestep
    SUMOReal energyCharged;

    /// @brief Parameter 21, How many timestep the vehicle is stopped
    int vehicleStopped;


    /** @brief convert from SUMOReal to String
     * @param[in] var Variable in SUMOReal format
    * @return Variable var in String format
    */
    std::string SUMOReal_str(const SUMOReal& var);


    /// @brief Invalidated copy constructor.
    MSDevice_Battery(const MSDevice_Battery&);


    /// @brief Invalidated assignment operator.
    MSDevice_Battery& operator=(const MSDevice_Battery&);
};

#endif

