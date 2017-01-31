/****************************************************************************/
/// @file    MSEmissionExport.cpp
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    2012-04-26
/// @version $Id$
///
// Realises dumping Emission Data
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2012-2017 DLR (http://www.dlr.de/) and contributors
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

#include <utils/iodevices/OutputDevice.h>
#include <utils/emissions/PollutantsInterface.h>
#include <utils/emissions/HelpersHarmonoise.h>
#include <utils/geom/GeomHelper.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicle.h>
#include "MSEmissionExport.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
void
MSEmissionExport::write(OutputDevice& of, SUMOTime timestep, int precision) {
    of.openTag("timestep").writeAttr("time", time2string(timestep));
    of.setPrecision(precision);
    MSVehicleControl& vc = MSNet::getInstance()->getVehicleControl();
    for (MSVehicleControl::constVehIt it = vc.loadedVehBegin(); it != vc.loadedVehEnd(); ++it) {
        const SUMOVehicle* veh = it->second;
        const MSVehicle* microVeh = dynamic_cast<const MSVehicle*>(veh);
        if (veh->isOnRoad()) {
            std::string fclass = veh->getVehicleType().getID();
            fclass = fclass.substr(0, fclass.find_first_of("@"));
            PollutantsInterface::Emissions emiss = PollutantsInterface::computeAll(veh->getVehicleType().getEmissionClass(), veh->getSpeed(), veh->getAcceleration(), veh->getSlope());
            of.openTag("vehicle").writeAttr("id", veh->getID()).writeAttr("eclass", PollutantsInterface::getName(veh->getVehicleType().getEmissionClass()));
            of.writeAttr("CO2", emiss.CO2).writeAttr("CO", emiss.CO).writeAttr("HC", emiss.HC).writeAttr("NOx", emiss.NOx);
            of.writeAttr("PMx", emiss.PMx).writeAttr("fuel", emiss.fuel).writeAttr("electricity", emiss.electricity);
            of.writeAttr("noise", HelpersHarmonoise::computeNoise(veh->getVehicleType().getEmissionClass(), veh->getSpeed(), veh->getAcceleration()));
            of.writeAttr("route", veh->getRoute().getID()).writeAttr("type", fclass);
            if (microVeh != 0) {
                of.writeAttr("waiting", microVeh->getWaitingSeconds());
                of.writeAttr("lane", microVeh->getLane()->getID());
            }
            of.writeAttr("pos", veh->getPositionOnLane()).writeAttr("speed", veh->getSpeed());
            of.writeAttr("angle", GeomHelper::naviDegree(veh->getAngle())).writeAttr("x", veh->getPosition().x()).writeAttr("y", veh->getPosition().y());
            of.closeTag();
        }
    }
    of.setPrecision(gPrecision);
    of.closeTag();
}
