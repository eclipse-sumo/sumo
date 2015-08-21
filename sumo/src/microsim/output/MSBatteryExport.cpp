/****************************************************************************/
/// @file    MSBatteryExport.cpp
/// @author  Mario Krumnow
/// @author  Tamas Kurczveil
/// @author  Pablo Alvarez LÛpez
/// @date    20-12-13
/// @version $Id$
///
// Realises dumping Battery Data
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

#include <microsim/MSEdgeControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSGlobals.h>
#include <utils/iodevices/OutputDevice.h>
#include "MSBatteryExport.h"
#include <microsim/MSNet.h>
#include <microsim/MSVehicle.h>
#include <microsim/devices/MSDevice_Battery.h>

#ifdef HAVE_MESOSIM
#include <mesosim/MELoop.h>
#include <mesosim/MESegment.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
void
MSBatteryExport::write(OutputDevice& of, SUMOTime timestep, int precision) 
{
    of.openTag("timestep").writeAttr("time", time2string(timestep));
    of.setPrecision(precision);

    MSVehicleControl& vc = MSNet::getInstance()->getVehicleControl();
    MSVehicleControl::constVehIt it = vc.loadedVehBegin();
    MSVehicleControl::constVehIt end = vc.loadedVehEnd();
    for (; it != end; ++it) 
    {
        const MSVehicle* veh = static_cast<const MSVehicle*>((*it).second);
        
        if (!veh->isOnRoad()) 
        {
            continue;
        }

        std::string fclass = veh->getVehicleType().getID();
        fclass = fclass.substr(0, fclass.find_first_of("@"));

        Position pos = veh->getLane()->getShape().positionAtOffset(veh->getPositionOnLane());
     
        if(static_cast<MSDevice_Battery*>(veh->getDevice(typeid(MSDevice_Battery))) != 0)
        {
            // Get battery
            MSDevice_Battery* batteryToExport = dynamic_cast<MSDevice_Battery*>(veh->getDevice(typeid(MSDevice_Battery)));

            // Open Row
            of.openTag("vehicle");

            // Write ID
            of.writeAttr("id", veh->getID());

            // Write consum
            of.writeAttr("Consum", batteryToExport->getConsum());

            // Write ActBatKap
            of.writeAttr("ActBatKap", batteryToExport->getActBatKap());

            // Write MaxBatKap
            of.writeAttr("MaxBatKap", batteryToExport->getMaxBatKap());

            // Write Charging Station ID
            of.writeAttr("ChrgStnId", batteryToExport->getChrgStnID());

            // Write Charge charged in the Battery
            of.writeAttr("Charge", batteryToExport->getChrgEnergy());

            // Write ChargeInTransit
            if(batteryToExport->isChargingInTransit())
            {
                of.writeAttr("ChargeInTransit",batteryToExport->getChrgEnergy());
            }
            else
            {
                of.writeAttr("ChargeInTransit", 0.00);
            }

            // Write ChargingStopped
            if(batteryToExport->isChargingStopped() )
            {
                of.writeAttr("ChargeStopped", batteryToExport->getChrgEnergy());
            }
            else
            {
                of.writeAttr("ChargeStopped", 0.00);
            }
            
            // Write Speed
            of.writeAttr("speed", veh->getSpeed());

            // Write Acceleration
            of.writeAttr("acceleration", veh->getAcceleration());

            // Write pos x
            of.writeAttr("x", veh->getPosition().x());

            // Write pos y
            of.writeAttr("y", veh->getPosition().y());

            // Write Lane ID
            of.writeAttr("lane", veh->getLane()->getID());

            // Write vehicle position in the lane
            of.writeAttr("posOnLane", veh->getPositionOnLane());
            
            // Write Time stopped (In all cases)
            of.writeAttr("timeStopped", batteryToExport->getVehicleStopped());

            /*0
                1
                2
                3
                4
            vehiclestopped‹berein station

                wielangevehiceladen
                */
            // Close Row
            of.closeTag();
        }
    }
    of.closeTag();
    
}
