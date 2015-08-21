/****************************************************************************/
/// @file    MSDevice_Battery.cpp
/// @author  Tamas Kurczveil
/// @author  Pablo Alvarez López
/// @date    20.12.2013    
/// @version $Id: MSDevice_Example.cpp 14577 2013-09-04 11:50:49Z dkrajzew $ // ATENCION
///
// ATENCION Descripcion
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSVehicle.h>
#include "MSDevice_Tripinfo.h"
#include "MSDevice_Battery.h"

#define PI 3.141592654

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
MSDevice_Battery::insertOptions(OptionsCont& oc) 
{
    insertDefaultAssignmentOptions("battery", "Battery", oc);
}


void
MSDevice_Battery::buildVehicleDevices(SUMOVehicle& v, std::vector<MSDevice*>& into) 
{
    if (!equippedByDefaultAssignmentOptions(OptionsCont::getOptions(), "battery", v)) {
        return;
    }

    // Declare default parameters
    SUMOReal new_ActBatKap = 0;
    SUMOReal new_MaxBatKap = 0;
    SUMOReal new_PowerMax = 100;                                                                                    
    SUMOReal new_Mass = 1000;                                                                                    
    SUMOReal new_FrontSurfaceArea = 2;                                                                                    
    SUMOReal new_AirDragCoefficient = 0.4;                                                                                    
    SUMOReal new_InternalMomentOfInertia = 10;                                                                                    
    SUMOReal new_RadialDragCoefficient = 1;                                                                        
    SUMOReal new_RollDragCoefficient = 0.5;                                                                        
    SUMOReal new_ConstantPowerIntake = 10;                                                                        
    SUMOReal new_PropulsionEfficiency = 0.5;                                                                    
    SUMOReal new_RecuperationEfficiency = 0;
    SUMOReal new_LastAngle = 0;
    SUMOReal new_LastEnergy = 0;

    // MaxBatKap
    new_MaxBatKap = TplConvert::_2SUMOReal(v.getVehicleType().getParameter().getParameter("MaxBatKap", "0").c_str());

    // ActBatKap
    if (v.getParameter().getParameter("ActBatKap", "-") == "-")
    {
        new_ActBatKap = new_MaxBatKap / 2.0;
    }
    else
    {
        new_ActBatKap = TplConvert::_2SUMOReal(v/*.getVehicleType()*/.getParameter().getParameter("ActBatKap", "0").c_str());
    }

    // Power
    new_PowerMax = TplConvert::_2SUMOReal(v.getVehicleType().getParameter().getParameter("PowerMax", "100").c_str());

    // Mass
    new_Mass = TplConvert::_2SUMOReal(v.getVehicleType().getParameter().getParameter("Mass", "1000").c_str());

    // FrontSurfaceArea
    new_FrontSurfaceArea = TplConvert::_2SUMOReal(v.getVehicleType().getParameter().getParameter("FrontSurfaceArea", "2").c_str());

    // AirDragCoefficient
    new_AirDragCoefficient = TplConvert::_2SUMOReal(v.getVehicleType().getParameter().getParameter("AirDragCoefficient", "0.4").c_str());

    // InternalMomentOfInertia
    new_InternalMomentOfInertia = TplConvert::_2SUMOReal(v.getVehicleType().getParameter().getParameter("InternalMomentOfInertia", "10").c_str());

    // Radial Drag Coefficient
    new_RadialDragCoefficient = TplConvert::_2SUMOReal(v.getVehicleType().getParameter().getParameter("RadialDragCoefficient", "1").c_str());

    // RollDragCoefficient
    new_RollDragCoefficient = TplConvert::_2SUMOReal(v.getVehicleType().getParameter().getParameter("RollDragCoefficient", "0.5").c_str());

    // ConstantPowerIntake
    new_ConstantPowerIntake = TplConvert::_2SUMOReal(v.getVehicleType().getParameter().getParameter("ConstantPowerIntake", "10").c_str());

    // PropulsionEfficiency
    new_PropulsionEfficiency = TplConvert::_2SUMOReal(v.getVehicleType().getParameter().getParameter("PropulsionEfficiency", "0.5").c_str());

    // RecuperationEfficiency
    new_RecuperationEfficiency = TplConvert::_2SUMOReal(v.getVehicleType().getParameter().getParameter("RecuperationEfficiency", "0").c_str());

    // constructor
    MSDevice_Battery* device = new MSDevice_Battery(v, "battery_" + v.getID(),
        new_ActBatKap, new_MaxBatKap, new_PowerMax, new_Mass, new_FrontSurfaceArea, new_AirDragCoefficient, 
        new_InternalMomentOfInertia, new_RadialDragCoefficient, new_RollDragCoefficient, 
        new_ConstantPowerIntake, new_PropulsionEfficiency, new_RecuperationEfficiency, 
        new_LastAngle, new_LastEnergy);

    into.push_back(device);
}


bool MSDevice_Battery::notifyMove(SUMOVehicle& veh, SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed)
{
    // Start vehicleStoppedTimer if the vehicle is stopped (that's mean, speed is < 0.2). In other case reset timer
    if(veh.getSpeed() < 0.2)
    {
        // Increase vehicle stopped timer
        increaseVehicleStoppedTimer();
    }
    else
    {
        // Reset vehicle Stopped
        resetVehicleStoppedTimer();
    }

    // Update Energy from the battery                
    if (getMaxBatKap() != 0) 
    {
        Consum = getPropEnergy(veh);

        // Energy lost/gained from vehicle movement (via vehicle energy model) [kWh]
        setActBatKap(getActBatKap() - Consum);    

        // saturate between 0 and MaxBatKap [kWh]                        
        if (getActBatKap() < 0)
        {                                        
            setActBatKap(0);

            if (getMaxBatKap() > 0)            
                WRITE_WARNING("Battery from vehicle with ID =  '" + veh.getID() + "' is depleted.");
                                                                
        } else if (getActBatKap() > getMaxBatKap()) 
            setActBatKap(getMaxBatKap());

        setLastAngle(veh.getAngle());                    
    }

    // Check if vehicle has under their position one charge Station
    std::string ChargingStationID = MSNet::getInstance()->getChrgStnID(veh.getLane(), veh.getPositionOnLane());

    // If vehicle is over a charging station
    if (ChargingStationID != "")
    {
        // Declare a pointer to the charging station
        MSChrgStn* ChargingStationPointer = MSNet::getInstance()->getChrgStn(ChargingStationID);

        // if the vehicle is almost stopped, or charge in transit is enabled, then charge vehicle
        if ((veh.getSpeed() < 0.2) || (ChargingStationPointer->getChargeInTransit() == 1))
        {
            // Set Flags Stopped/intransit to 
            if(veh.getSpeed() < 0.2)
            {
                // vehicle ist almost stopped, then is charging stopped
                ItsChargingStopped = true;

                // therefore isn't charging in transit
                ItsChargingInTransit = false;
            }
            else
            {
                // vehicle is moving, and the Charging station allow charge in transit
                ItsChargingStopped = false;

                // Therefore charge in transit 
                ItsChargingInTransit = true;
            }

            // Set actChrgStn parameter 
            actChrgStn = ChargingStationID;

            // Only update charging start time if vehicle allow charge in transit, or in other case
            // if the vehicle not allow charge in transit but it's stopped.
            if (ChargingStationPointer->getChargeInTransit() == 1 || veh.getSpeed()<0.2)
            {
                // Update Charging start time
                increaseChargingStartTime();
            }

            // time it takes the vehicle at the station < charging station time delay?
            if (getChargingStartTime() > ChargingStationPointer->getChargeDelay())
            {
                // Calulate energy charged (Fix);                                                                                                                        
                energyCharged = ChargingStationPointer->getChrgPower() * ChargingStationPointer->getEfficency();

                // Convert from [kWs] to [kWh] (3600s / 1h):        
                energyCharged /= 3600;

                // Update Battery charge
                if ((energyCharged + getActBatKap()) > getMaxBatKap())
                {
                    setActBatKap(getMaxBatKap());
                }
                else
                {
                    setActBatKap(getActBatKap() + energyCharged);
                }
            }
        }
    }
    // In other case, vehicle will be not charged
    else
    {
        // Disable flags
        ItsChargingInTransit = false;
        ItsChargingStopped = false;

        // Disable charging station
        actChrgStn = "NULL";

        // Set energy charged to 0
        energyCharged = 0.00;

        // Reset timer
        resetChargingStartTime();
    }

    // Always return true.
    return true;
}


bool MSDevice_Battery::notifyEnter(SUMOVehicle& veh, MSMoveReminder::Notification reason)
{
    // Set last Energy ONLY when the vehicle is introduced in the simulation (That's mean, that the reason is 0)
    if (reason== 0)
    {
        setLastEnergy(getMass() * veh.getSpeed() * veh.getSpeed() / 2 + getMass() * 9.81 * veh.getLane()->getShape().front().z() + getInternalMomentOfInertia() * 0.5 * veh.getSpeed() * veh.getSpeed());
        setLastAngle(veh.getLane()->getShape().beginEndAngle() * 180 / PI);                                            
    }

    // This function return always true
    return true;
}


void MSDevice_Battery::generateOutput() const 
{
    //Function implemented in MSBatteryExport
    if (false) /*OptionsCont::getOptions().isSet("tripinfo-output")*/
    {
        OutputDevice& os = OutputDevice::getDeviceByOption("tripinfo-output");
        
        os.openTag("Battery");
            os.writeAttr("Act Bat Kap", toString(ActBatKap));
            os.writeAttr("Max Bat Kap", toString(MaxBatKap));
            os.writeAttr("Power Max", toString(PowerMax));
            os.writeAttr("Mass", toString(Mass));
            os.writeAttr("Front Surface Area", toString(FrontSurfaceArea));
            os.writeAttr("Air Drag Coefficient", toString(AirDragCoefficient));
            os.writeAttr("Radial Drag Coefficient", toString(RadialDragCoefficient));
            os.writeAttr("RollDrag Coefficient", toString(RollDragCoefficient));
            os.writeAttr("Constant Power Intake", toString(ConstantPowerIntake));
            os.writeAttr("Propulsion Efficiency", toString(PropulsionEfficiency));
            os.writeAttr("Recuperation Efficiency", toString(RecuperationEfficiency));
            os.writeAttr("Last Angle", toString(LastAngle));
            os.writeAttr("Last Energy", toString(LastEnergy));
        os.closeTag();
    }
}


// ---------------------------------------------------------------------------
// MSDevice_Battery-methods
// ---------------------------------------------------------------------------
MSDevice_Battery::MSDevice_Battery(SUMOVehicle& holder, const std::string& id, const SUMOReal new_ActBatKap, const SUMOReal new_MaxBatKap, const SUMOReal new_PowerMax, const SUMOReal new_Mass, const SUMOReal new_FrontSurfaceArea, const SUMOReal new_AirDragCoefficient, const SUMOReal new_InternalMomentOfInertia, const SUMOReal new_RadialDragCoefficient, const SUMOReal new_RollDragCoefficient, const SUMOReal new_ConstantPowerIntake, const SUMOReal new_PropulsionEfficiency, const SUMOReal new_RecuperationEfficiency, const SUMOReal new_LastAngle, const SUMOReal new_LastEnergy)
:
        MSDevice(holder, id),
        ActBatKap(new_ActBatKap),                                // [ActBatKap <= MaxBatKap]
        MaxBatKap(new_MaxBatKap),                                // [MaxBatKap >= 0]
        PowerMax(new_PowerMax),                                    // [PowerMax >= 0]
        Mass(new_Mass),                                            // [Mass >= 0]
        FrontSurfaceArea(new_FrontSurfaceArea),                    // [FrontSurfaceArea >= 0]
        AirDragCoefficient(new_AirDragCoefficient),                // [AirDragCoefficient >=0]
        InternalMomentOfInertia(new_InternalMomentOfInertia),    // [InternalMomentOfInertia >= 0]
        RadialDragCoefficient(new_RadialDragCoefficient),        // [RadialDragCoefficient >=0]
        RollDragCoefficient(new_RollDragCoefficient),            // [RollDragCoefficient >= 0]
        ConstantPowerIntake(new_ConstantPowerIntake),            // [ConstantPowerIntake >= 0]
        PropulsionEfficiency(new_PropulsionEfficiency),            // [1 >= PropulsionEfficiency >= 0]
        RecuperationEfficiency(new_RecuperationEfficiency),        // [1 >= RecuperationEfficiency >= 0]
        LastAngle(new_LastAngle),                                // Limit not needed
        LastEnergy(new_LastEnergy)                                // Limit not needed
{
    // Initially the Vehicle is not charging and is not in a Chargin Station
    ItsChargingStopped = false;
    ItsChargingInTransit = false;

    // Initially the energy charged is null
    energyCharged = 0;

    // Initially the Vehicle stopped and the consum is cero.
    Consum = 0;

    // Initially the vehicle is stopped and the corresponding variable is 0
    vehicleStopped = 0;

    // Initially the Vehicle are not over a Charging Station
    actChrgStn = "NULL";

    if(ActBatKap > MaxBatKap)
        WRITE_WARNING("Battery builder: Vehicle with id = " + getID() + " has a actual battery capacity ("  + SUMOReal_str(ActBatKap) + ") greater than it's max battery capacity(" + SUMOReal_str(MaxBatKap) + ").");
    
    if(MaxBatKap < 0)
        WRITE_WARNING("Battery builder: Vehicle with id = " + getID() + " has not a valid battery capacity (" + SUMOReal_str(MaxBatKap) + ").");
    
    if(PowerMax < 0)
        WRITE_WARNING("Battery builder: Vehicle with id = " + getID() + " has not a valid power max (" + SUMOReal_str(PowerMax) + ").");

    if(Mass < 0)
        WRITE_WARNING("Battery builder: Vehicle with id = " + getID() + " has not a valid mass (" + SUMOReal_str(Mass) + ").");

    if(FrontSurfaceArea < 0)
        WRITE_WARNING("Battery builder: Vehicle with id = " + getID() + " has not a valid front surface Area (" + SUMOReal_str(FrontSurfaceArea) + ").");
    
    if(AirDragCoefficient < 0)
        WRITE_WARNING("Battery builder: Vehicle with id = " + getID() + " has not a valid drag coefficient (" + SUMOReal_str(AirDragCoefficient) + ").");
    
    if(InternalMomentOfInertia < 0)
        WRITE_WARNING("Battery builder: Vehicle with id = " + getID() + " has not a valid internal moment of inertia (" + SUMOReal_str(InternalMomentOfInertia) + ").");
    
    if(RadialDragCoefficient < 0)
        WRITE_WARNING("Battery builder: Vehicle with id = " + getID() + " has not a valid radial friction coefficient (" + SUMOReal_str(RadialDragCoefficient) + ").");
    
    if(RollDragCoefficient < 0)
        WRITE_WARNING("Battery builder: Vehicle with id = " + getID() + " has not a valid roll friction coefficient (" + SUMOReal_str(RollDragCoefficient) + ").");
    
    if(ConstantPowerIntake < 0)
        WRITE_WARNING("Battery builder: Vehicle with id = " + getID() + " has not a valid constant power intake (" + SUMOReal_str(ConstantPowerIntake) + ").");
    
    if(PropulsionEfficiency < 0 || PropulsionEfficiency > 1)
        WRITE_WARNING("Battery builder: Vehicle with id = " + getID() + " has not a valid propulsion efficiency (" + SUMOReal_str(PropulsionEfficiency) + ").");
    
    if(RecuperationEfficiency < 0 || RecuperationEfficiency > 1)
        WRITE_WARNING("Battery builder: Vehicle with id = " + getID() + " has not a valid Recuparation efficiency (" + SUMOReal_str(RecuperationEfficiency) + ").");        
}


MSDevice_Battery::~MSDevice_Battery() 
{}


// SET FUNCTIONS

void MSDevice_Battery::setActBatKap(const SUMOReal new_ActBatKap)
{
    ActBatKap = new_ActBatKap;

    if(ActBatKap > MaxBatKap)
        WRITE_WARNING("Function setActBatKap: Actual battery capacity ("  + SUMOReal_str(ActBatKap) + ") from Vehicle with id = " + getID() + " is greater than it's max battery capacity(" + SUMOReal_str(MaxBatKap) + ").");
}


void MSDevice_Battery::setMaxBatKap(const SUMOReal new_MaxBatKap)
{
    MaxBatKap = new_MaxBatKap;

    if(MaxBatKap < 0)
        WRITE_WARNING("Function setMaxBatKap: Vehicle with id = " + getID() + " has not a valid battery capacity (" + SUMOReal_str(MaxBatKap) + ").");
}


void MSDevice_Battery::setMass(const SUMOReal new_Mass)
{
    Mass = new_Mass;

    if(Mass < 0)
        WRITE_WARNING("Function setMass: Vehicle with id = " + getID() + " has not a valid mass (" + SUMOReal_str(Mass) + ").");
}


void MSDevice_Battery::setPowerMax(const SUMOReal new_PowerMax)
{
    PowerMax = new_PowerMax;

    if(PowerMax < 0)
        WRITE_WARNING("Function setPowerMax: Vehicle with id = " + getID() + " has not a valid power max (" + SUMOReal_str(PowerMax) + ").");
}

void MSDevice_Battery::setFrontSurfaceArea(const SUMOReal new_FrontSurfaceArea)
{
    FrontSurfaceArea = new_FrontSurfaceArea;

    if(FrontSurfaceArea < 0)
        WRITE_WARNING("Function setFrontSurfaceArea: Vehicle with id = " + getID() + " has not a valid front surface Area (" + SUMOReal_str(FrontSurfaceArea) + ").");
}


void MSDevice_Battery::setAirDragCoefficient(const SUMOReal new_AirDragCoefficient)
{
    AirDragCoefficient = new_AirDragCoefficient;

    if(AirDragCoefficient < 0)
        WRITE_WARNING("Function setAirDragCoefficient: Vehicle with id = " + getID() + " has not a valid drag coefficient (" + SUMOReal_str(AirDragCoefficient) + ").");
}


void MSDevice_Battery::setInternalMomentOfInertia(const SUMOReal new_InternalMomentOfInertia)
{
    InternalMomentOfInertia = new_InternalMomentOfInertia;

    if(InternalMomentOfInertia < 0)
        WRITE_WARNING("Function setInternalMomentOfInertia: Vehicle with id = " + getID() + " has not a valid internal moment of inertia (" + SUMOReal_str(InternalMomentOfInertia) + ").");
}


void MSDevice_Battery::setRadialDragCoefficient(const SUMOReal new_RadialDragCoefficient)
{
    RadialDragCoefficient = new_RadialDragCoefficient;

    if(RadialDragCoefficient < 0)
        WRITE_WARNING("Function setRadialDragCoefficient: Vehicle with id = " + getID() + " has not a valid radial friction coefficient (" + SUMOReal_str(RadialDragCoefficient) + ").");
}


void MSDevice_Battery::setRollDragCoefficient(const SUMOReal new_RollDragCoefficient)
{
    RollDragCoefficient = new_RollDragCoefficient;
    
    if(RollDragCoefficient < 0)
        WRITE_WARNING("Function setRollDragCoefficient: Vehicle with id = " + getID() + " has not a valid roll friction coefficient (" + SUMOReal_str(RollDragCoefficient) + ").");
}


void MSDevice_Battery::setConstantPowerIntake(const SUMOReal new_ConstantPowerIntake)
{
    ConstantPowerIntake = new_ConstantPowerIntake;

    if(ConstantPowerIntake < 0)
        WRITE_WARNING("Function setConstantPowerIntake: Vehicle with id = " + getID() + " has not a valid constant power intake (" + SUMOReal_str(ConstantPowerIntake) + ").");
}


void MSDevice_Battery::setPropulsionEfficiency(const SUMOReal new_PropulsionEfficiency)
{
    PropulsionEfficiency = new_PropulsionEfficiency;

    if(PropulsionEfficiency < 0 || PropulsionEfficiency > 1)
        WRITE_WARNING("Function setPropulsionEfficiency: Vehicle with id = " + getID() + " has not a valid propulsion efficiency (" + SUMOReal_str(PropulsionEfficiency) + ").");
}


void MSDevice_Battery::setRecuperationEfficiency(const SUMOReal new_RecuperationEfficiency)
{
    RecuperationEfficiency = new_RecuperationEfficiency;

    if(RecuperationEfficiency < 0 || RecuperationEfficiency > 1)
        WRITE_WARNING("Function setRecuperationEfficiency: Vehicle with id = " + getID() + " has not a valid recuparation efficiency (" + SUMOReal_str(RecuperationEfficiency) + ").");
}


void MSDevice_Battery::setLastAngle(const SUMOReal new_LastAngle)
{
    LastAngle = new_LastAngle;
}


void MSDevice_Battery::setLastEnergy(const SUMOReal new_LastEnergy)
{
    LastEnergy = new_LastEnergy ;
}    

inline void MSDevice_Battery::resetChargingStartTime()
{
    ChargingStartTime = 0;
}

inline void MSDevice_Battery::increaseChargingStartTime()
{
    ChargingStartTime++;
}

inline void MSDevice_Battery::resetVehicleStoppedTimer()
{
    vehicleStopped = 0;
}

inline void MSDevice_Battery::increaseVehicleStoppedTimer()
{
    vehicleStopped++;
}

//GET FUNCTIONS

const SUMOReal MSDevice_Battery::getActBatKap()
{
    return ActBatKap;
}

const SUMOReal MSDevice_Battery::getMaxBatKap()
{
    return MaxBatKap;
}

const SUMOReal MSDevice_Battery::getPowerMax()
{
    return PowerMax;
}

const SUMOReal MSDevice_Battery::getMass()
{
    return Mass;
}

const SUMOReal MSDevice_Battery::getFrontSurfaceArea()
{
    return FrontSurfaceArea;
}

const SUMOReal MSDevice_Battery::getAirDragCoefficient()
{
    return AirDragCoefficient;
}

const SUMOReal MSDevice_Battery::getInternalMomentOfInertia() {
    return InternalMomentOfInertia;
}

const SUMOReal MSDevice_Battery::getRadialDragCoefficient() {
    return RadialDragCoefficient;
}

const SUMOReal MSDevice_Battery::getRollDragCoefficient() {
    return RollDragCoefficient;
}

const SUMOReal MSDevice_Battery::getConstantPowerIntake() {
    return ConstantPowerIntake;
}

const SUMOReal MSDevice_Battery::getPropulsionEfficiency() {
    return PropulsionEfficiency;
}

const SUMOReal MSDevice_Battery::getRecuperationEfficiency() {
    return RecuperationEfficiency;
}

const SUMOReal MSDevice_Battery::getLastAngle() {
    return LastAngle;
}

const SUMOReal MSDevice_Battery::getLastEnergy() {
    return LastEnergy;    
}

const SUMOReal MSDevice_Battery::getConsum() {
    return Consum;
}

bool MSDevice_Battery::isChargingStopped() {
    return ItsChargingStopped;
}

bool MSDevice_Battery::isChargingInTransit() {
    return ItsChargingInTransit;
}

const SUMOReal MSDevice_Battery::getChargingStartTime() {
    return ChargingStartTime;
}

const std::string& MSDevice_Battery::getChrgStnID() {
    return actChrgStn;
}

const SUMOReal MSDevice_Battery::getChrgEnergy() {
    return energyCharged;
}


const int MSDevice_Battery::getVehicleStopped() {
   return vehicleStopped;
}


const SUMOReal MSDevice_Battery::getPropEnergy(SUMOVehicle& veh) 
{    
    // calculate current kinetic energy
    SUMOReal height_cur = veh.getPositionOnLane() / veh.getLane()->getLength() * (veh.getLane()->getShape().back().z() - veh.getLane()->getShape().front().z());

    // kinetic energy of vehicle with current velocity                                                                            
    SUMOReal currentEnergy = 0.5 * getMass() * veh.getSpeed() * veh.getSpeed();    
                                                                                                                
    // add current potential energy of vehicle at current position                                                                    
    currentEnergy += getMass() * 9.81 * height_cur;                                                                        
        
    // Calculate the radius of the vehicle's current path if is distintc (r = ds / dphi)                                                            
    SUMOReal radius = 0;                                                                                                                
                                                                                                                                    
    if (getLastAngle() != veh.getAngle())
        radius = veh.getSpeed() * 180 / (PI * (
                    (fabs(getLastAngle() - veh.getAngle()) < fabs(veh.getAngle() - getLastAngle())) ? 
                    fabs(getLastAngle() - veh.getAngle()): 
                    fabs(veh.getAngle() - getLastAngle())));                                        
                                                                                                                        
    // add current rotational energy of internal rotating elements                                                                    
    currentEnergy += getInternalMomentOfInertia() * veh.getSpeed() * veh.getSpeed();    
                                                                                                                        
    // kinetic + potential + rotational energy gain [Ws] (MODIFICATED LAST ANGLE)                                                                            
    SUMOReal EnergyLoss = (currentEnergy - getLastEnergy());    
                                                                                                                        
    // save current total energy for next time step                                                                                    
    setLastEnergy(currentEnergy);                                                                                                        
                                                                                                                                    
    // Calculate energy losses:                                                                                                        
    // EnergyLoss,Air = 1/2 * rho_air [kg/m^3] * FrontSurfaceArea [m^2] * AirDragCoefficient [-] * v_Veh^2 [m/s] * s [m]                                            
    //                    ... with rho_air [kg/m^3] = 1,2041 kg/m^3 (at T = 20°C)                                                        
    //                    ... with s [m] = v_Veh [m/s] * 1 [s]                                                                        
    EnergyLoss += 0.5 * 1.2041 * getFrontSurfaceArea() * getAirDragCoefficient() * fabs(veh.getSpeed() * veh.getSpeed() * veh.getSpeed());                    

    // Energy loss through Air resistance [Ws]                                                                                        
    // EnergyLoss,Tire = c_R [-] * F_N [N] * s [m]                                                                                        
    //                    ... with c_R = ~0.012    (car tire on asphalt)                                                                
    //                    ... with F_N [N] = Mass [kg] * g [m/s^2]                                                                    
    EnergyLoss += getRollDragCoefficient() * 9.81 * getMass() * fabs(veh.getSpeed());                                        

    // Energy loss through Roll resistance [Ws]                                                                                        
    //                    ... (fabs(veh.getSpeed())>=0.01) = 0, if vehicle isn't moving                                                            
    // EnergyLoss,internalFrictionRadialForce = c [m] * F_rad [N];                                                                        
    if (getLastAngle() != veh.getAngle()) {
        // Energy loss through friction by radial force [Ws]                                                                        
        EnergyLoss += getRadialDragCoefficient() * getMass() * veh.getSpeed() * veh.getSpeed() / radius;        
    }                                                                                                                                
                                                                                                                                    
    // EnergyLoss,constantConsumers                                                                                                        
    // Energy loss through constant loads (e.g. A/C) [Ws]                                                                            
    EnergyLoss += getConstantPowerIntake();
                                                                                                                        
    //E_Bat = E_kin_pot + EnergyLoss;                                                                                                    
    if (EnergyLoss > 0)
    {                                                                                                                
        // Assumption: Efficiency of PropulsionEfficiency when accelerating                                                                        
        EnergyLoss = EnergyLoss / getPropulsionEfficiency();
    } 
    else 
    {                                                                                                                        
        // Assumption: Efficiency of RecuperationEfficiency when recuperating                                                                    
        EnergyLoss = EnergyLoss * getRecuperationEfficiency();
    }                                                                                                                                
                                                                                                                                    
    // convert from [Ws] to [kWh] (3600s / 1h):                                                                                        
    EnergyLoss = EnergyLoss / 3600 ; // EnergyLoss[Ws] * 1[h]/3600[s] * 1[k]/1000
    
    
    // original EnergyLoss = EnergyLoss / 3600 / 1000 ; // EnergyLoss[Ws] * 1[h]/3600[s] * 1[k]/1000
    
    //BATTERYS CAPACITY CHANGE !!

    return(EnergyLoss);    
}                                                                                                                                    


// Private methods

std::string MSDevice_Battery::SUMOReal_str(const SUMOReal &var)
{
    std::ostringstream convert;
    convert << var; 
    return convert.str();
}
