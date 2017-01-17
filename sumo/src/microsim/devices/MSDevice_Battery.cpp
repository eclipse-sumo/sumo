/****************************************************************************/
/// @file    MSDevice_Battery.cpp
/// @author  Tamas Kurczveil
/// @author  Pablo Alvarez Lopez
/// @date    20.12.2013
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

#include <utils/common/TplConvert.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/SUMOTime.h>
#include <utils/geom/GeomHelper.h>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSVehicle.h>
#include "MSDevice_Tripinfo.h"
#include "MSDevice_Battery.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
void
MSDevice_Battery::insertOptions(OptionsCont& oc) {
    insertDefaultAssignmentOptions("battery", "Battery", oc);
}


void
MSDevice_Battery::buildVehicleDevices(SUMOVehicle& v, std::vector<MSDevice*>& into) {
    if (!equippedByDefaultAssignmentOptions(OptionsCont::getOptions(), "battery", v)) {
        return;
    }

    // myMaximumBatteryCapacity
    SUMOReal maximumBatteryCapacity = TplConvert::_2SUMOReal(v.getVehicleType().getParameter().getParameter("maximumBatteryCapacity", "0").c_str());

    // myActualBatteryCapacity
    SUMOReal actualBatteryCapacity = 0;
    if (v.getParameter().getParameter("actualBatteryCapacity", "-") == "-") {
        actualBatteryCapacity = maximumBatteryCapacity / 2.0;
    } else {
        actualBatteryCapacity = TplConvert::_2SUMOReal(v.getParameter().getParameter("actualBatteryCapacity", "0").c_str());
    }

    // powerMax
    SUMOReal powerMax = TplConvert::_2SUMOReal(v.getVehicleType().getParameter().getParameter("maximumPower", "100").c_str());

    // mass
    SUMOReal mass = TplConvert::_2SUMOReal(v.getVehicleType().getParameter().getParameter("vehicleMass", "1000").c_str());

    // frontSurfaceArea
    SUMOReal frontSurfaceArea = TplConvert::_2SUMOReal(v.getVehicleType().getParameter().getParameter("frontSurfaceArea", "2").c_str());

    // airDragCoefficient
    SUMOReal airDragCoefficient = TplConvert::_2SUMOReal(v.getVehicleType().getParameter().getParameter("airDragCoefficient", "0.4").c_str());

    // internalMomentOfInertia
    SUMOReal internalMomentOfInertia = TplConvert::_2SUMOReal(v.getVehicleType().getParameter().getParameter("internalMomentOfInertia", "10").c_str());

    // radialDragCoefficient
    SUMOReal radialDragCoefficient = TplConvert::_2SUMOReal(v.getVehicleType().getParameter().getParameter("radialDragCoefficient", "1").c_str());

    // rollDragCoefficient
    SUMOReal rollDragCoefficient = TplConvert::_2SUMOReal(v.getVehicleType().getParameter().getParameter("rollDragCoefficient", "0.5").c_str());

    // constantPowerIntake
    SUMOReal constantPowerIntake = TplConvert::_2SUMOReal(v.getVehicleType().getParameter().getParameter("constantPowerIntake", "10").c_str());

    // propulsionEfficiency
    SUMOReal propulsionEfficiency = TplConvert::_2SUMOReal(v.getVehicleType().getParameter().getParameter("propulsionEfficiency", "0.5").c_str());

    // recuperationEfficiency
    SUMOReal recuperationEfficiency = TplConvert::_2SUMOReal(v.getVehicleType().getParameter().getParameter("recuperationEfficiency", "0").c_str());

    // constructor
    MSDevice_Battery* device = new MSDevice_Battery(v, "battery_" + v.getID(),
            actualBatteryCapacity, maximumBatteryCapacity, powerMax, mass, frontSurfaceArea, airDragCoefficient,
            internalMomentOfInertia, radialDragCoefficient, rollDragCoefficient,
            constantPowerIntake, propulsionEfficiency, recuperationEfficiency, 0, 0);

    // Add device to vehicle
    into.push_back(device);
}


bool MSDevice_Battery::notifyMove(SUMOVehicle& veh, SUMOReal /* oldPos */, SUMOReal /* newPos */, SUMOReal /* newSpeed */) {
    // Start vehicleStoppedTimer if the vehicle is stopped. In other case reset timer
    if (veh.getSpeed() < SUMO_const_haltingSpeed)
        // Increase vehicle stopped timer
    {
        increaseVehicleStoppedTimer();
    } else
        // Reset vehicle Stopped
    {
        resetVehicleStoppedTimer();
    }

    // Update Energy from the battery
    if (getMaximumBatteryCapacity() != 0) {
        myConsum = getPropEnergy(veh);

        // Energy lost/gained from vehicle movement (via vehicle energy model) [kWh]
        setActualBatteryCapacity(getActualBatteryCapacity() - myConsum);

        // saturate between 0 and myMaximumBatteryCapacity [kWh]
        if (getActualBatteryCapacity() < 0) {
            setActualBatteryCapacity(0);

            // Show  warning if battery is depleted
            if (getMaximumBatteryCapacity() > 0)
                WRITE_WARNING("Battery of vehicle '" + veh.getID() + "' is depleted.")

            } else if (getActualBatteryCapacity() > getMaximumBatteryCapacity()) {
            setActualBatteryCapacity(getMaximumBatteryCapacity());
        }

        setLastAngle(veh.getAngle());
    }

    // Check if vehicle has under their position one charge Station
    std::string ChargingStationID = MSNet::getInstance()->getChargingStationID(veh.getLane(), veh.getPositionOnLane());

    // If vehicle is over a charging station
    if (ChargingStationID != "") {
        // if the vehicle is almost stopped, or charge in transit is enabled, then charge vehicle
        if ((veh.getSpeed() < SUMO_const_haltingSpeed) || (MSNet::getInstance()->getChargingStation(ChargingStationID)->getChargeInTransit() == 1)) {
            // Set Flags Stopped/intransit to
            if (veh.getSpeed() < SUMO_const_haltingSpeed) {
                // vehicle ist almost stopped, then is charging stopped
                myChargingStopped = true;

                // therefore isn't charging in transit
                myChargingInTransit = false;
            } else {
                // vehicle is moving, and the Charging station allow charge in transit
                myChargingStopped = false;

                // Therefore charge in transit
                myChargingInTransit = true;
            }

            // get pointer to charging station
            myActChargingStation = MSNet::getInstance()->getChargingStation(ChargingStationID);

            // Only update charging start time if vehicle allow charge in transit, or in other case
            // if the vehicle not allow charge in transit but it's stopped.
            if (myActChargingStation->getChargeInTransit() == true || veh.getSpeed() < SUMO_const_haltingSpeed)
                // Update Charging start time
            {
                increaseChargingStartTime();
            }

            // time it takes the vehicle at the station < charging station time delay?
            if (getChargingStartTime() > myActChargingStation->getChargeDelay()) {
                // Enable charging vehicle
                myActChargingStation->setChargingVehicle(true);

                // Calulate energy charged
                myEnergyCharged = myActChargingStation->getChargingPower() * myActChargingStation->getEfficency();

                // Convert from [kWs] to [kWh] (3600s / 1h):
                myEnergyCharged /= 3600;

                // Update Battery charge
                if ((myEnergyCharged + getActualBatteryCapacity()) > getMaximumBatteryCapacity()) {
                    setActualBatteryCapacity(getMaximumBatteryCapacity());
                } else {
                    setActualBatteryCapacity(getActualBatteryCapacity() + myEnergyCharged);
                }
            }
        }
    }
    // In other case, vehicle will be not charged
    else {
        // Disable flags
        myChargingInTransit = false;
        myChargingStopped = false;

        // Disable charging vehicle
        if (myActChargingStation != NULL) {
            myActChargingStation->setChargingVehicle(false);
        }

        // Set charging station pointer to NULL
        myActChargingStation = NULL;

        // Set energy charged to 0
        myEnergyCharged = 0.00;

        // Reset timer
        resetChargingStartTime();
    }

    // Always return true.
    return true;
}


bool MSDevice_Battery::notifyEnter(SUMOVehicle& veh, MSMoveReminder::Notification reason) {
    // Set last Energy ONLY when the vehicle is introduced in the simulation
    if (reason == MSMoveReminder::NOTIFICATION_DEPARTED) {
        setLastEnergy(getMass() * veh.getSpeed() * veh.getSpeed() / 2 + getMass() * 9.81 * veh.getLane()->getShape().front().z() + getInternalMomentOfInertia() * 0.5 * veh.getSpeed() * veh.getSpeed());
        setLastAngle(veh.getLane()->getShape().beginEndAngle());
    }

    // This function return always true
    return true;
}


void MSDevice_Battery::generateOutput() const {
    //Function implemented in MSBatteryExport
}


// ---------------------------------------------------------------------------
// MSDevice_Battery-methods
// ---------------------------------------------------------------------------
MSDevice_Battery::MSDevice_Battery(SUMOVehicle& holder, const std::string& id, const SUMOReal actualBatteryCapacity, const SUMOReal maximumBatteryCapacity, const SUMOReal powerMax, const SUMOReal mass, const SUMOReal frontSurfaceArea, const SUMOReal airDragCoefficient, const SUMOReal internalMomentOfInertia, const SUMOReal radialDragCoefficient, const SUMOReal rollDragCoefficient, const SUMOReal constantPowerIntake, const SUMOReal propulsionEfficiency, const SUMOReal recuperationEfficiency, const SUMOReal lastAngle, const SUMOReal lastEnergy) :
    MSDevice(holder, id),
    myActualBatteryCapacity(0),         // [actualBatteryCapacity <= maximumBatteryCapacity]
    myMaximumBatteryCapacity(0),        // [maximumBatteryCapacity >= 0]
    myPowerMax(0),                      // [maximumPower >= 0]
    myMass(0),                          // [vehicleMass >= 0]
    myFrontSurfaceArea(0),              // [frontSurfaceArea >= 0]
    myAirDragCoefficient(0),            // [airDragCoefficient >=0]
    myInternalMomentOfInertia(0),       // [internalMomentOfInertia >= 0]
    myRadialDragCoefficient(0),         // [radialDragCoefficient >=0]
    myRollDragCoefficient(0),           // [rollDragCoefficient >= 0]
    myConstantPowerIntake(0),           // [constantPowerIntake >= 0]
    myPropulsionEfficiency(0),          // [1 >= propulsionEfficiency >= 0]
    myRecuperationEfficiency(0),        // [1 >= recuperationEfficiency >= 0]
    myLastAngle(lastAngle),             // Limit not needed
    myLastEnergy(lastEnergy),           // Limit not needed
    myChargingStopped(false),           // Initially vehicle don't charge stopped
    myChargingInTransit(false),         // Initially vehicle don't charge in transit
    myConsum(0),                        // Initially the vehicle is stopped and therefore the consum is zero.
    myActChargingStation(NULL),         // Initially the vehicle isn't over a Charging Station
    myEnergyCharged(0),                 // Initially the energy charged is zero
    myVehicleStopped(0) {               // Initially the vehicle is stopped and the corresponding variable is 0

    if (maximumBatteryCapacity < 0)
        WRITE_WARNING("Battery builder: Vehicle '" + getID() + "' doesn't have a valid value for parameter maximum battery capacity (" + TplConvert::_2str(maximumBatteryCapacity) + ").")
        else {
            myMaximumBatteryCapacity = maximumBatteryCapacity;
        }

    if (actualBatteryCapacity > maximumBatteryCapacity) {
        WRITE_WARNING("Battery builder: Vehicle '" + getID() + "' has a actual battery capacity ("  + TplConvert::_2str(actualBatteryCapacity) + ") greater than it's max battery capacity(" + TplConvert::_2str(maximumBatteryCapacity) + "). A max battery capacity value will be asigned");
        myActualBatteryCapacity = myMaximumBatteryCapacity;
    } else {
        myActualBatteryCapacity = actualBatteryCapacity;
    }

    if (powerMax < 0)
        WRITE_WARNING("Battery builder: Vehicle '" + getID() + "' doesn't have a valid value for parameter maximum power (" + TplConvert::_2str(powerMax) + ").")
        else {
            myPowerMax = powerMax;
        }

    if (mass < 0)
        WRITE_WARNING("Battery builder: Vehicle '" + getID() + "' doesn't have a valid value for parameter mass (" + TplConvert::_2str(mass) + ").")
        else {
            myMass = mass;
        }

    if (frontSurfaceArea < 0)
        WRITE_WARNING("Battery builder: Vehicle '" + getID() + "' doesn't have a valid value for parameter front surface Area (" + TplConvert::_2str(frontSurfaceArea) + ").")
        else {
            myFrontSurfaceArea = frontSurfaceArea;
        }

    if (airDragCoefficient < 0)
        WRITE_WARNING("Battery builder: Vehicle '" + getID() + "' doesn't have a valid value for parameter drag coefficient (" + TplConvert::_2str(airDragCoefficient) + ").")
        else {
            myAirDragCoefficient = airDragCoefficient;
        }

    if (internalMomentOfInertia < 0)
        WRITE_WARNING("Battery builder: Vehicle '" + getID() + "' doesn't have a valid value for parameter internal moment of insertia (" + TplConvert::_2str(internalMomentOfInertia) + ").")
        else {
            myInternalMomentOfInertia = internalMomentOfInertia;
        }

    if (radialDragCoefficient < 0)
        WRITE_WARNING("Battery builder: Vehicle '" + getID() + "' doesn't have a valid value for parameter radial friction coefficient (" + TplConvert::_2str(radialDragCoefficient) + ").")
        else {
            myRadialDragCoefficient = radialDragCoefficient;
        }

    if (rollDragCoefficient < 0)
        WRITE_WARNING("Battery builder: Vehicle '" + getID() + "' doesn't have a valid value for parameter roll friction coefficient (" + TplConvert::_2str(rollDragCoefficient) + ").")
        else {
            myRollDragCoefficient = rollDragCoefficient;
        }

    if (constantPowerIntake < 0)
        WRITE_WARNING("Battery builder: Vehicle '" + getID() + "' doesn't have a valid value for parameter constant power intake (" + TplConvert::_2str(constantPowerIntake) + ").")
        else {
            myConstantPowerIntake = constantPowerIntake;
        }

    if (propulsionEfficiency < 0 || propulsionEfficiency > 1)
        WRITE_WARNING("Battery builder: Vehicle '" + getID() + "' doesn't have a valid value for parameter propulsion efficiency (" + TplConvert::_2str(propulsionEfficiency) + ").")
        else {
            myPropulsionEfficiency = propulsionEfficiency;
        }

    if (recuperationEfficiency < 0 || recuperationEfficiency > 1)
        WRITE_WARNING("Battery builder: Vehicle '" + getID() + "' doesn't have a valid value for parameter Recuparation efficiency (" + TplConvert::_2str(recuperationEfficiency) + ").")
        else {
            myRecuperationEfficiency = recuperationEfficiency;
        }
}


MSDevice_Battery::~MSDevice_Battery() {
}


void
MSDevice_Battery::setActualBatteryCapacity(const SUMOReal actualBatteryCapacity) {
    if (actualBatteryCapacity < 0) {
        myActualBatteryCapacity = 0;
    } else if (actualBatteryCapacity > myMaximumBatteryCapacity) {
        myActualBatteryCapacity = myMaximumBatteryCapacity;
    } else {
        myActualBatteryCapacity = actualBatteryCapacity;
    }
}


void
MSDevice_Battery::setMaximumBatteryCapacity(const SUMOReal maximumBatteryCapacity) {
    if (myMaximumBatteryCapacity < 0)
        WRITE_WARNING("Function setMaximumBatteryCapacity: Trying to insert into a battery device of vehicle '" + getID() + "' a invalid maximum battery capacity (" + TplConvert::_2str(maximumBatteryCapacity) + ").")
        else {
            myMaximumBatteryCapacity = maximumBatteryCapacity;
        }
}


void
MSDevice_Battery::setMass(const SUMOReal mass) {
    if (myMass < 0)
        WRITE_WARNING("Function setMass: Trying to insert into a battery device of vehicle '" + getID() + "' a invalid mass value (" + TplConvert::_2str(mass) + ").")
        else {
            myMass = mass;
        }
}


void
MSDevice_Battery::setPowerMax(const SUMOReal powerMax) {
    if (myPowerMax < 0)
        WRITE_WARNING("Function setPowerMax: Trying to insert into a battery device of vehicle '" + getID() + "' a invalid power max value (" + TplConvert::_2str(powerMax) + ").")
        else {
            myPowerMax = powerMax;
        }
}


void
MSDevice_Battery::setFrontSurfaceArea(const SUMOReal frontSurfaceArea) {
    if (myFrontSurfaceArea < 0)
        WRITE_WARNING("Function setFrontSurfaceArea: Trying to insert into a battery device of vehicle '" + getID() + "' a invalid front surface Area value (" + TplConvert::_2str(frontSurfaceArea) + ").")
        else {
            myFrontSurfaceArea = frontSurfaceArea;
        }
}


void
MSDevice_Battery::setAirDragCoefficient(const SUMOReal airDragCoefficient) {
    if (myAirDragCoefficient < 0)
        WRITE_WARNING("Function setAirDragCoefficient: Trying to insert into a battery device of vehicle '" + getID() + "' a invalid drag coefficient value (" + TplConvert::_2str(airDragCoefficient) + ").")
        else {
            myAirDragCoefficient = airDragCoefficient;
        }
}


void
MSDevice_Battery::setInternalMomentOfInertia(const SUMOReal internalMomentOfInertia) {
    if (myInternalMomentOfInertia < 0)
        WRITE_WARNING("Function setInternalMomentOfInertia: Trying to insert into a battery device of vehicle '" + getID() + "' a invalid internal moment of insertia value (" + TplConvert::_2str(internalMomentOfInertia) + ").")
        else {
            myInternalMomentOfInertia = internalMomentOfInertia;
        }
}


void
MSDevice_Battery::setRadialDragCoefficient(const SUMOReal radialDragCoefficient) {
    if (myRadialDragCoefficient < 0)
        WRITE_WARNING("Function setRadialDragCoefficient: Trying to insert into a battery device of vehicle '" + getID() + "' a invalid radial friction coefficient value (" + TplConvert::_2str(radialDragCoefficient) + ").")
        else {
            myRadialDragCoefficient = radialDragCoefficient;
        }
}


void
MSDevice_Battery::setRollDragCoefficient(const SUMOReal rollDragCoefficient) {
    if (myRollDragCoefficient < 0)
        WRITE_WARNING("Function setRollDragCoefficient: Trying to insert into a battery device of vehicle '" + getID() + "' a invalid roll friction coefficient value (" + TplConvert::_2str(rollDragCoefficient) + ").")
        else {
            myRollDragCoefficient = rollDragCoefficient;
        }
}


void
MSDevice_Battery::setConstantPowerIntake(const SUMOReal constantPowerIntake) {
    if (myConstantPowerIntake < 0)
        WRITE_WARNING("Function setConstantPowerIntake: Trying to insert into a battery device of vehicle '" + getID() + "' a invalid constant power intake value (" + TplConvert::_2str(constantPowerIntake) + ").")
        else {
            myConstantPowerIntake = constantPowerIntake;
        }
}


void
MSDevice_Battery::setPropulsionEfficiency(const SUMOReal propulsionEfficiency) {
    if (myPropulsionEfficiency < 0 || myPropulsionEfficiency > 1)
        WRITE_WARNING("Function setPropulsionEfficiency: Trying to insert into a battery device of vehicle '" + getID() + "' a invalid propulsion efficiency value (" + TplConvert::_2str(propulsionEfficiency) + ").")
        else {
            myPropulsionEfficiency = propulsionEfficiency;
        }
}


void
MSDevice_Battery::setRecuperationEfficiency(const SUMOReal recuperationEfficiency) {
    if (myRecuperationEfficiency < 0 || myRecuperationEfficiency > 1)
        WRITE_WARNING("Function setRecuperationEfficiency: Trying to insert into a battery device of vehicle '" + getID() + "' a invalid recuparation efficiency value (" + TplConvert::_2str(recuperationEfficiency) + ").")
        else {
            myRecuperationEfficiency = recuperationEfficiency;
        }
}


void
MSDevice_Battery::setLastAngle(const SUMOReal lastAngle) {
    myLastAngle = lastAngle;
}


void
MSDevice_Battery::setLastEnergy(const SUMOReal lastEnergy) {
    myLastEnergy = lastEnergy ;
}


void
MSDevice_Battery::resetChargingStartTime() {
    myChargingStartTime = 0;
}


void
MSDevice_Battery::increaseChargingStartTime() {
    myChargingStartTime++;
}


void
MSDevice_Battery::resetVehicleStoppedTimer() {
    myVehicleStopped = 0;
}


void
MSDevice_Battery::increaseVehicleStoppedTimer() {
    myVehicleStopped++;
}


SUMOReal
MSDevice_Battery::getActualBatteryCapacity() const {
    return myActualBatteryCapacity;
}


SUMOReal
MSDevice_Battery::getMaximumBatteryCapacity() const {
    return myMaximumBatteryCapacity;
}


SUMOReal
MSDevice_Battery::getMaximumPower() const {
    return myPowerMax;
}


SUMOReal
MSDevice_Battery::getMass() const {
    return myMass;
}


SUMOReal
MSDevice_Battery::getFrontSurfaceArea() const {
    return myFrontSurfaceArea;
}


SUMOReal
MSDevice_Battery::getAirDragCoefficient() const {
    return myAirDragCoefficient;
}


SUMOReal
MSDevice_Battery::getInternalMomentOfInertia() const {
    return myInternalMomentOfInertia;
}


SUMOReal
MSDevice_Battery::getRadialDragCoefficient() const {
    return myRadialDragCoefficient;
}


SUMOReal
MSDevice_Battery::getRollDragCoefficient() const {
    return myRollDragCoefficient;
}


SUMOReal
MSDevice_Battery::getConstantPowerIntake() const {
    return myConstantPowerIntake;
}


SUMOReal
MSDevice_Battery::getPropulsionEfficiency() const {
    return myPropulsionEfficiency;
}


SUMOReal
MSDevice_Battery::getRecuperationEfficiency() const {
    return myRecuperationEfficiency;
}


SUMOReal
MSDevice_Battery::getLastAngle() const {
    return myLastAngle;
}


SUMOReal
MSDevice_Battery::getLastEnergy() const {
    return myLastEnergy;
}


SUMOReal
MSDevice_Battery::getConsum() const {
    return myConsum;
}


bool
MSDevice_Battery::isChargingStopped() const {
    return myChargingStopped;
}


bool
MSDevice_Battery::isChargingInTransit() const {
    return myChargingInTransit;
}


SUMOReal
MSDevice_Battery::getChargingStartTime() const {
    return myChargingStartTime;
}


std::string
MSDevice_Battery::getChargingStationID() const {
    if (myActChargingStation != NULL) {
        return myActChargingStation->getID();
    } else {
        return "NULL";
    }
}


SUMOReal
MSDevice_Battery::getEnergyCharged() const {
    return myEnergyCharged;
}


int MSDevice_Battery::getVehicleStopped() const {
    return myVehicleStopped;
}


SUMOReal MSDevice_Battery::getPropEnergy(SUMOVehicle& veh) {
    assert(veh.getSpeed() >= 0.);

    //XXX: All formulas below work with the logic of the euler update (refs #860).
    //     Approximation order could be improved. Refs. #2592.

    // calculate current height
    SUMOReal height_cur = veh.getPositionOnLane() / veh.getLane()->getLength() * (veh.getLane()->getShape().back().z() - veh.getLane()->getShape().front().z());

    // kinetic energy of vehicle with current velocity
    SUMOReal currentEnergy = 0.5 * getMass() * veh.getSpeed() * veh.getSpeed();

    // add current potential energy of vehicle at current position
    currentEnergy += getMass() * 9.81 * height_cur;

    // Calculate the radius of the vehicle's current path if is distinct (r = ds / dphi)
    SUMOReal radius = 0;

    // add current rotational energy of internal rotating elements
    currentEnergy += getInternalMomentOfInertia() * veh.getSpeed() * veh.getSpeed();

    // kinetic + potential + rotational energy gain [Ws] (MODIFICATED LAST ANGLE)
    SUMOReal EnergyLoss = (currentEnergy - getLastEnergy());

    // save current total energy for next time step
    setLastEnergy(currentEnergy);


    // Energy loss through Air resistance [Ws]
    // Calculate energy losses:
    // EnergyLoss,Air = 1/2 * rho_air [kg/m^3] * myFrontSurfaceArea [m^2] * myAirDragCoefficient [-] * v_Veh^2 [m/s] * s [m]
    //                    ... with rho_air [kg/m^3] = 1,2041 kg/m^3 (at T = 20C)
    //                    ... with s [m] = v_Veh [m/s] * TS [s]
    EnergyLoss += 0.5 * 1.2041 * getFrontSurfaceArea() * getAirDragCoefficient() * veh.getSpeed() * veh.getSpeed() * SPEED2DIST(veh.getSpeed());


    // Energy loss through Roll resistance [Ws]
    //                    ... (fabs(veh.getSpeed())>=0.01) = 0, if vehicle isn't moving
    // EnergyLoss,Tire = c_R [-] * F_N [N] * s [m]
    //                    ... with c_R = ~0.012    (car tire on asphalt)
    //                    ... with F_N [N] = myMass [kg] * g [m/s^2]
    EnergyLoss += getRollDragCoefficient() * 9.81 * getMass() * SPEED2DIST(veh.getSpeed());


    // Energy loss through friction by radial force [Ws]
    // If angle of vehicle was changed
    if (getLastAngle() != veh.getAngle()) {
        // Compute new radio
        radius = SPEED2DIST(veh.getSpeed()) / fabs(GeomHelper::angleDiff(getLastAngle(), veh.getAngle()));

        // Check if radius is in the interval [0.0001 - 10000] (To avoid overflow and division by zero)
        if (radius < 0.0001) {
            radius = 0.0001;
        } else if (radius > 10000) {
            radius = 10000;
        }
    }
    // EnergyLoss,internalFrictionRadialForce = c [m] * F_rad [N];
    if (getLastAngle() != veh.getAngle()) {
        // Energy loss through friction by radial force [Ws]
        EnergyLoss += getRadialDragCoefficient() * getMass() * veh.getSpeed() * veh.getSpeed() / radius;
    }

    // EnergyLoss,constantConsumers
    // Energy loss through constant loads (e.g. A/C) [Ws]
    EnergyLoss += getConstantPowerIntake();

    //E_Bat = E_kin_pot + EnergyLoss;
    if (EnergyLoss > 0) {
        // Assumption: Efficiency of myPropulsionEfficiency when accelerating
        EnergyLoss = EnergyLoss / getPropulsionEfficiency();
    } else {
        // Assumption: Efficiency of myRecuperationEfficiency when recuperating
        EnergyLoss = EnergyLoss * getRecuperationEfficiency();
    }

    // convert from [Ws] to [kWh] (3600s / 1h):
    EnergyLoss = EnergyLoss / 3600 ; // EnergyLoss[Ws] * 1[h]/3600[s] * 1[k]/1000

    // Return calculated energy
    return EnergyLoss;
}

