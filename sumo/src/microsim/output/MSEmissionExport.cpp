/****************************************************************************/
/// @file    MSEmissionExport.cpp
/// @author  Mario Krumnow
/// @version $Id$
///
// Realises dumping Emission Data
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
#include "MSEmissionExport.h"
#include <microsim/MSNet.h>
#include <microsim/MSVehicle.h>

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
MSEmissionExport::write(OutputDevice& of, SUMOTime timestep) {

    of.openTag("timestep").writeAttr("time", time2string(timestep));

    MSVehicleControl& vc = MSNet::getInstance()->getVehicleControl();
    MSVehicleControl::constVehIt it = vc.loadedVehBegin();
    MSVehicleControl::constVehIt end = vc.loadedVehEnd();
    for (; it != end; ++it) {
        const MSVehicle* veh = static_cast<const MSVehicle*>((*it).second);
        if (!veh->isOnRoad()) {
            continue;
        }

        std::string fclass = veh->getVehicleType().getID();
        fclass = fclass.substr(0, fclass.find_first_of("@"));

        Position pos = veh->getLane()->getShape().positionAtOffset(veh->getPositionOnLane());
        of.openTag("vehicle").writeAttr("id", veh->getID()).writeAttr("eclass", veh->getVehicleType().getEmissionClass()).writeAttr("co2", veh->getHBEFA_CO2Emissions());
        of.writeAttr("co", veh->getHBEFA_COEmissions()).writeAttr("hc", veh->getHBEFA_HCEmissions()).writeAttr("nox", veh->getHBEFA_NOxEmissions());
        of.writeAttr("pmx", veh->getHBEFA_PMxEmissions()).writeAttr("fuel", veh->getHBEFA_FuelConsumption()).writeAttr("noise", veh->getHarmonoise_NoiseEmissions());
        of.writeAttr("route", veh->getRoute().getID()).writeAttr("type", fclass).writeAttr("waiting", veh->getWaitingSeconds());
        of.writeAttr("lane", veh->getLane()->getID()).writeAttr("pos", veh->getPositionOnLane()).writeAttr("speed", veh->getSpeed() * 3.6);
        of.writeAttr("angle", veh->getAngle()).writeAttr("x", pos.x()).writeAttr("y", pos.y());
        of.closeTag();
    }
    of.closeTag();
}
