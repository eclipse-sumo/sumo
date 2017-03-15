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
    // Check if vehicle should get a battery
    if (equippedByDefaultAssignmentOptions(OptionsCont::getOptions(), "battery", v)) {
        // obtain maximumBatteryCapacity
        double maximumBatteryCapacity = TplConvert::_2double(v.getVehicleType().getParameter().getParameter("maximumBatteryCapacity", "0").c_str());

        // obtain actualBatteryCapacity
        double actualBatteryCapacity = 0;
        if (v.getParameter().getParameter("actualBatteryCapacity", "-") == "-") {
            actualBatteryCapacity = maximumBatteryCapacity / 2.0;
        } else {
            actualBatteryCapacity = TplConvert::_2double(v.getParameter().getParameter("actualBatteryCapacity", "0").c_str());
        }

        // obtain powerMax
        double powerMax = TplConvert::_2double(v.getVehicleType().getParameter().getParameter("maximumPower", "100").c_str());

        // obtain mass
        double mass = TplConvert::_2double(v.getVehicleType().getParameter().getParameter("vehicleMass", "1000").c_str());

        // obtain frontSurfaceArea
        double frontSurfaceArea = TplConvert::_2double(v.getVehicleType().getParameter().getParameter("frontSurfaceArea", "2").c_str());

        // obtain airDragCoefficient
        double airDragCoefficient = TplConvert::_2double(v.getVehicleType().getParameter().getParameter("airDragCoefficient", "0.4").c_str());

        // obtain internalMomentOfInertia
        double internalMomentOfInertia = TplConvert::_2double(v.getVehicleType().getParameter().getParameter("internalMomentOfInertia", "10").c_str());

        // obtain radialDragCoefficient
        double radialDragCoefficient = TplConvert::_2double(v.getVehicleType().getParameter().getParameter("radialDragCoefficient", "1").c_str());

        // obtain rollDragCoefficient
        double rollDragCoefficient = TplConvert::_2double(v.getVehicleType().getParameter().getParameter("rollDragCoefficient", "0.5").c_str());

        // obtain constantPowerIntake
        double constantPowerIntake = TplConvert::_2double(v.getVehicleType().getParameter().getParameter("constantPowerIntake", "10").c_str());

        // obtain propulsionEfficiency
        double propulsionEfficiency = TplConvert::_2double(v.getVehicleType().getParameter().getParameter("propulsionEfficiency", "0.5").c_str());

        // obtain recuperationEfficiency
        double recuperationEfficiency = TplConvert::_2double(v.getVehicleType().getParameter().getParameter("recuperationEfficiency", "0").c_str());

        // stoppingTreshold
        double stoppingTreshold = TplConvert::_2double(v.getVehicleType().getParameter().getParameter("stoppingTreshold", "0.1").c_str());

        // battery constructor
        MSDevice_Battery* device = new MSDevice_Battery(v, "battery_" + v.getID(),
                actualBatteryCapacity, maximumBatteryCapacity, powerMax, mass, frontSurfaceArea, airDragCoefficient,
                internalMomentOfInertia, radialDragCoefficient, rollDragCoefficient,
                constantPowerIntake, propulsionEfficiency, recuperationEfficiency, stoppingTreshold, 0, 0);

        // Add device to vehicle
        into.push_back(device);
    }
}


bool MSDevice_Battery::notifyMove(SUMOVehicle& veh, double /* oldPos */, double /* newPos */, double /* newSpeed */) {
    // Start vehicleStoppedTimer if the vehicle is stopped. In other case reset timer
    if (veh.getSpeed() < myStoppingTreshold) {
        // Increase vehicle stopped timer
        increaseVehicleStoppedTimer();
    } else {
        // Reset vehicle Stopped
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
        if ((veh.getSpeed() < myStoppingTreshold) || (MSNet::getInstance()->getChargingStation(ChargingStationID)->getChargeInTransit() == 1)) {
            // Set Flags Stopped/intransit to
            if (veh.getSpeed() < myStoppingTreshold) {
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
            if ((myActChargingStation->getChargeInTransit() == true) || (veh.getSpeed() < myStoppingTreshold)) {
                // Update Charging start time
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


bool MSDevice_Battery::notifyEnter(SUMOVehicle& veh, MSMoveReminder::Notification reason, const MSLane* /* enteredLane */) {
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
MSDevice_Battery::MSDevice_Battery(SUMOVehicle& holder, const std::string& id, const double actualBatteryCapacity, const double maximumBatteryCapacity, 
    const double powerMax, const double mass, const double frontSurfaceArea, const double airDragCoefficient, const double internalMomentOfInertia, 
    const double radialDragCoefficient, const double rollDragCoefficient, const double constantPowerIntake, const double propulsionEfficiency, 
    const double recuperationEfficiency, const double stoppingTreshold, const double lastAngle, const double lastEnergy) :
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
    myStoppingTreshold(0),              // [stoppingTreshold >= 0]
    myLastAngle(lastAngle),             // Limit not needed
    myLastEnergy(lastEnergy),           // Limit not needed
    myChargingStopped(false),           // Initially vehicle don't charge stopped
    myChargingInTransit(false),         // Initially vehicle don't charge in transit
    myConsum(0),                        // Initially the vehicle is stopped and therefore the consum is zero.
    myActChargingStation(NULL),         // Initially the vehicle isn't over a Charging Station
    myEnergyCharged(0),                 // Initially the energy charged is zero
    myVehicleStopped(0) {               // Initially the vehicle is stopped and the corresponding variable is 0          

    if (maximumBatteryCapacity < 0) {
        WRITE_WARNING("Battery builder: Vehicle '" + getID() + "' doesn't have a valid value for parameter maximum battery capacity (" + toString(maximumBatteryCapacity) + ").")
    } else {
        myMaximumBatteryCapacity = maximumBatteryCapacity;
    }

    if (actualBatteryCapacity > maximumBatteryCapacity) {
        WRITE_WARNING("Battery builder: Vehicle '" + getID() + "' has a actual battery capacity ("  + toString(actualBatteryCapacity) + ") greater than it's max battery capacity(" + toString(maximumBatteryCapacity) + "). A max battery capacity value will be asigned");
        myActualBatteryCapacity = myMaximumBatteryCapacity;
    } else {
        myActualBatteryCapacity = actualBatteryCapacity;
    }

    if (powerMax < 0) {
        WRITE_WARNING("Battery builder: Vehicle '" + getID() + "' doesn't have a valid value for parameter maximum power (" + toString(powerMax) + ").")
    } else {
        myPowerMax = powerMax;
    }

    if (mass < 0) {
        WRITE_WARNING("Battery builder: Vehicle '" + getID() + "' doesn't have a valid value for parameter mass (" + toString(mass) + ").")
    } else {
        myMass = mass;
    }

    if (frontSurfaceArea < 0) {
        WRITE_WARNING("Battery builder: Vehicle '" + getID() + "' doesn't have a valid value for parameter front surface Area (" + toString(frontSurfaceArea) + ").")
    } else {
        myFrontSurfaceArea = frontSurfaceArea;
    }

    if (airDragCoefficient < 0) {
        WRITE_WARNING("Battery builder: Vehicle '" + getID() + "' doesn't have a valid value for parameter drag coefficient (" + toString(airDragCoefficient) + ").")
    } else {
        myAirDragCoefficient = airDragCoefficient;
    }

    if (internalMomentOfInertia < 0) {
        WRITE_WARNING("Battery builder: Vehicle '" + getID() + "' doesn't have a valid value for parameter internal moment of insertia (" + toString(internalMomentOfInertia) + ").")
    } else {
        myInternalMomentOfInertia = internalMomentOfInertia;
    }

    if (radialDragCoefficient < 0) {
        WRITE_WARNING("Battery builder: Vehicle '" + getID() + "' doesn't have a valid value for parameter radial friction coefficient (" + toString(radialDragCoefficient) + ").")
    } else {
        myRadialDragCoefficient = radialDragCoefficient;
    }

    if (rollDragCoefficient < 0) {
        WRITE_WARNING("Battery builder: Vehicle '" + getID() + "' doesn't have a valid value for parameter roll friction coefficient (" + toString(rollDragCoefficient) + ").")
    } else {
        myRollDragCoefficient = rollDragCoefficient;
    }

    if (constantPowerIntake < 0) {
        WRITE_WARNING("Battery builder: Vehicle '" + getID() + "' doesn't have a valid value for parameter constant power intake (" + toString(constantPowerIntake) + ").")
    } else {
        myConstantPowerIntake = constantPowerIntake;
    }

    if (propulsionEfficiency < 0 || propulsionEfficiency > 1) {
        WRITE_WARNING("Battery builder: Vehicle '" + getID() + "' doesn't have a valid value for parameter propulsion efficiency (" + toString(propulsionEfficiency) + ").")
    } else {
        myPropulsionEfficiency = propulsionEfficiency;
    }

    if (recuperationEfficiency < 0 || recuperationEfficiency > 1) {
        WRITE_WARNING("Battery builder: Vehicle '" + getID() + "' doesn't have a valid value for parameter Recuparation efficiency (" + toString(recuperationEfficiency) + ").")
    } else {
        myRecuperationEfficiency = recuperationEfficiency;
    }

    if (stoppingTreshold < 0) {
        WRITE_WARNING("Battery builder: Vehicle '" + getID() + "' doesn't have a valid value for parameter stoppingtreshold (" + toString(stoppingTreshold) + ").")
    } else {
        myStoppingTreshold = stoppingTreshold;
    }
}


MSDevice_Battery::~MSDevice_Battery() {
}


void
MSDevice_Battery::setActualBatteryCapacity(const double actualBatteryCapacity) {
    if (actualBatteryCapacity < 0) {
        myActualBatteryCapacity = 0;
    } else if (actualBatteryCapacity > myMaximumBatteryCapacity) {
        myActualBatteryCapacity = myMaximumBatteryCapacity;
    } else {
        myActualBatteryCapacity = actualBatteryCapacity;
    }
}


void
MSDevice_Battery::setMaximumBatteryCapacity(const double maximumBatteryCapacity) {
    if (myMaximumBatteryCapacity < 0) {
        WRITE_WARNING("Trying to set into the battery device of vehicle '" + getID() + "' a invalid maximum battery capacity (" + toString(maximumBatteryCapacity) + ").")
    } else {
        myMaximumBatteryCapacity = maximumBatteryCapacity;
    }
}


void
MSDevice_Battery::setMass(const double mass) {
    if (myMass < 0) {
        WRITE_WARNING("Trying to set into the battery device of vehicle '" + getID() + "' a invalid mass value (" + toString(mass) + ").")
    } else {
        myMass = mass;
    }
}


void
MSDevice_Battery::setPowerMax(const double powerMax) {
    if (myPowerMax < 0) {
        WRITE_WARNING("Trying to set into the battery device of vehicle '" + getID() + "' a invalid power max value (" + toString(powerMax) + ").")
    } else {
        myPowerMax = powerMax;
    }
}


void
MSDevice_Battery::setFrontSurfaceArea(const double frontSurfaceArea) {
    if (myFrontSurfaceArea < 0) {
        WRITE_WARNING("Trying to set into the battery device of vehicle '" + getID() + "' a invalid front surface Area value (" + toString(frontSurfaceArea) + ").")
    } else {
        myFrontSurfaceArea = frontSurfaceArea;
    }
}


void
MSDevice_Battery::setAirDragCoefficient(const double airDragCoefficient) {
    if (myAirDragCoefficient < 0) {
        WRITE_WARNING("Trying to set into the battery device of vehicle '" + getID() + "' a invalid drag coefficient value (" + toString(airDragCoefficient) + ").")
    } else {
        myAirDragCoefficient = airDragCoefficient;
    }
}


void
MSDevice_Battery::setInternalMomentOfInertia(const double internalMomentOfInertia) {
    if (myInternalMomentOfInertia < 0) {
        WRITE_WARNING("Trying to set into the battery device of vehicle '" + getID() + "' a invalid internal moment of insertia value (" + toString(internalMomentOfInertia) + ").")
    } else {
        myInternalMomentOfInertia = internalMomentOfInertia;
    }
}


void
MSDevice_Battery::setRadialDragCoefficient(const double radialDragCoefficient) {
    if (myRadialDragCoefficient < 0) {
        WRITE_WARNING("Trying to set into the battery device of vehicle '" + getID() + "' a invalid radial friction coefficient value (" + toString(radialDragCoefficient) + ").")
    } else {
        myRadialDragCoefficient = radialDragCoefficient;
    }
}


void
MSDevice_Battery::setRollDragCoefficient(const double rollDragCoefficient) {
    if (myRollDragCoefficient < 0) {
        WRITE_WARNING("Trying to set into the battery device of vehicle '" + getID() + "' a invalid roll friction coefficient value (" + toString(rollDragCoefficient) + ").")
    } else {
        myRollDragCoefficient = rollDragCoefficient;
    }
}


void
MSDevice_Battery::setConstantPowerIntake(const double constantPowerIntake) {
    if (myConstantPowerIntake < 0) {
        WRITE_WARNING("Trying to set into the battery device of vehicle '" + getID() + "' a invalid constant power intake value (" + toString(constantPowerIntake) + ").")
    } else {
        myConstantPowerIntake = constantPowerIntake;
    }
}


void
MSDevice_Battery::setPropulsionEfficiency(const double propulsionEfficiency) {
    if (myPropulsionEfficiency < 0 || myPropulsionEfficiency > 1) {
        WRITE_WARNING("Trying to set into the battery device of vehicle '" + getID() + "' a invalid propulsion efficiency value (" + toString(propulsionEfficiency) + ").")
    } else {
        myPropulsionEfficiency = propulsionEfficiency;
    }
}


void
MSDevice_Battery::setRecuperationEfficiency(const double recuperationEfficiency) {
    if (myRecuperationEfficiency < 0 || myRecuperationEfficiency > 1) {
        WRITE_WARNING("Trying to set into the battery device of vehicle '" + getID() + "' a invalid recuparation efficiency value (" + toString(recuperationEfficiency) + ").")
    } else {
        myRecuperationEfficiency = recuperationEfficiency;
    }
}


void
MSDevice_Battery::setLastAngle(const double lastAngle) {
    myLastAngle = lastAngle;
}


void
MSDevice_Battery::setLastEnergy(const double lastEnergy) {
    myLastEnergy = lastEnergy ;
}


void
MSDevice_Battery::setStoppingTreshold(const double stoppingTreshold) {
    if (stoppingTreshold < 0) {
        WRITE_WARNING("Trying to set into the battery device of vehicle '" + getID() + "' a invalid stopping treshold value (" + toString(stoppingTreshold) + ").")
    } else {
        myStoppingTreshold = stoppingTreshold;
    }
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


double
MSDevice_Battery::getActualBatteryCapacity() const {
    return myActualBatteryCapacity;
}


double
MSDevice_Battery::getMaximumBatteryCapacity() const {
    return myMaximumBatteryCapacity;
}


double
MSDevice_Battery::getMaximumPower() const {
    return myPowerMax;
}


double
MSDevice_Battery::getMass() const {
    return myMass;
}


double
MSDevice_Battery::getFrontSurfaceArea() const {
    return myFrontSurfaceArea;
}


double
MSDevice_Battery::getAirDragCoefficient() const {
    return myAirDragCoefficient;
}


double
MSDevice_Battery::getInternalMomentOfInertia() const {
    return myInternalMomentOfInertia;
}


double
MSDevice_Battery::getRadialDragCoefficient() const {
    return myRadialDragCoefficient;
}


double
MSDevice_Battery::getRollDragCoefficient() const {
    return myRollDragCoefficient;
}


double
MSDevice_Battery::getConstantPowerIntake() const {
    return myConstantPowerIntake;
}


double
MSDevice_Battery::getPropulsionEfficiency() const {
    return myPropulsionEfficiency;
}


double
MSDevice_Battery::getRecuperationEfficiency() const {
    return myRecuperationEfficiency;
}


double
MSDevice_Battery::getLastAngle() const {
    return myLastAngle;
}


double
MSDevice_Battery::getLastEnergy() const {
    return myLastEnergy;
}


double
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


double
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

double
MSDevice_Battery::getEnergyCharged() const {
    return myEnergyCharged;
}


int 
MSDevice_Battery::getVehicleStopped() const {
    return myVehicleStopped;
}


double 
MSDevice_Battery::getStoppingTreshold() const {
    return myStoppingTreshold;
}


double MSDevice_Battery::getPropEnergy(SUMOVehicle& veh) {

    //@ToDo: All formulas below work with the logic of the euler update (refs #860).
    //       Approximation order could be improved. Refs. #2592.

    // calculate current height
    double height_cur = veh.getPositionOnLane() / veh.getLane()->getLength() * (veh.getLane()->getShape().back().z() - veh.getLane()->getShape().front().z());

    // kinetic energy of vehicle with current velocity
    double currentEnergy = 0.5 * getMass() * veh.getSpeed() * veh.getSpeed();

    // add current potential energy of vehicle at current position
    currentEnergy += getMass() * 9.81 * height_cur;

    // Calculate the radius of the vehicle's current path if is distinct (r = ds / dphi)
    double radius = 0;

    // add current rotational energy of internal rotating elements
    currentEnergy += getInternalMomentOfInertia() * veh.getSpeed() * veh.getSpeed();

    // kinetic + potential + rotational energy gain [Ws] (MODIFICATED LAST ANGLE)
    double EnergyLoss = (currentEnergy - getLastEnergy());

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

