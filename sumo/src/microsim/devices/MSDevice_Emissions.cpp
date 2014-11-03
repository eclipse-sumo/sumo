/****************************************************************************/
/// @file    MSDevice_Emissions.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    Fri, 30.01.2009
/// @version $Id$
///
// A device which collects vehicular emissions
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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

#include "MSDevice_Emissions.h"
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicleControl.h>
#include <utils/options/OptionsCont.h>
#include <utils/emissions/PollutantsInterface.h>
#include <utils/iodevices/OutputDevice.h>

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
MSDevice_Emissions::insertOptions() {
    insertDefaultAssignmentOptions("emissions", "Emissions", OptionsCont::getOptions());
}


void
MSDevice_Emissions::buildVehicleDevices(SUMOVehicle& v, std::vector<MSDevice*>& into) {
    if (equippedByDefaultAssignmentOptions(OptionsCont::getOptions(), "emissions", v)) {
        // build the device
        MSDevice_Emissions* device = new MSDevice_Emissions(v, "emissions_" + v.getID());
        into.push_back(device);
    }
}


// ---------------------------------------------------------------------------
// MSDevice_Emissions-methods
// ---------------------------------------------------------------------------
MSDevice_Emissions::MSDevice_Emissions(SUMOVehicle& holder, const std::string& id)
    : MSDevice(holder, id), myEmissions() {
}


MSDevice_Emissions::~MSDevice_Emissions() {
}


bool
MSDevice_Emissions::notifyMove(SUMOVehicle& veh, SUMOReal /*oldPos*/, SUMOReal /*newPos*/, SUMOReal newSpeed) {
    const SUMOEmissionClass c = veh.getVehicleType().getEmissionClass();
    const SUMOReal a = veh.getAcceleration();
    const SUMOReal slope = veh.getSlope();
    myEmissions.addScaled(PollutantsInterface::computeAll(c, newSpeed, a, slope), TS);
    return true;
}


void
MSDevice_Emissions::generateOutput() const {
    if (OptionsCont::getOptions().isSet("tripinfo-output")) {
        OutputDevice& os = OutputDevice::getDeviceByOption("tripinfo-output");
        (os.openTag("emissions") <<
         " CO_abs=\"" << OutputDevice::realString(myEmissions.CO, 6) <<
         "\" CO2_abs=\"" << OutputDevice::realString(myEmissions.CO2, 6) <<
         "\" HC_abs=\"" << OutputDevice::realString(myEmissions.HC, 6) <<
         "\" PMx_abs=\"" << OutputDevice::realString(myEmissions.PMx, 6) <<
         "\" NOx_abs=\"" << OutputDevice::realString(myEmissions.NOx, 6) <<
         "\" fuel_abs=\"" << OutputDevice::realString(myEmissions.fuel, 6) <<
         "\"").closeTag();
    }
}



/****************************************************************************/

