/****************************************************************************/
/// @file    MSDevice_HBEFA.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    Fri, 30.01.2009
/// @version $Id$
///
// A device which collects vehicular emissions (using HBEFA-reformulation)
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

#include "MSDevice_HBEFA.h"
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicleControl.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/HelpersHBEFA.h>
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
MSDevice_HBEFA::insertOptions(OptionsCont& oc) {
    insertDefaultAssignmentOptions("hbefa", "Emissions", oc);
}


void
MSDevice_HBEFA::buildVehicleDevices(SUMOVehicle& v, std::vector<MSDevice*>& into) {
    OptionsCont& oc = OptionsCont::getOptions();
    // route computation is enabled
    if (equippedByDefaultAssignmentOptions(oc, "hbefa", v)) {
        // build the device
        MSDevice_HBEFA* device = new MSDevice_HBEFA(v, "hbefa_" + v.getID());
        into.push_back(device);
    }
}


// ---------------------------------------------------------------------------
// MSDevice_HBEFA-methods
// ---------------------------------------------------------------------------
MSDevice_HBEFA::MSDevice_HBEFA(SUMOVehicle& holder, const std::string& id)
    : MSDevice(holder, id),
      myCO2(0), myCO(0), myHC(0), myPMx(0), myNOx(0), myFuel(0) {
}


MSDevice_HBEFA::~MSDevice_HBEFA() {
}


bool
MSDevice_HBEFA::notifyMove(SUMOVehicle& veh, SUMOReal /*oldPos*/, SUMOReal /*newPos*/, SUMOReal newSpeed) {
    const SUMOEmissionClass c = veh.getVehicleType().getEmissionClass();
    const SUMOReal a = veh.getAcceleration();
    myCO2 += TS * HelpersHBEFA::computeCO2(c, newSpeed, a);
    myCO += TS * HelpersHBEFA::computeCO(c, newSpeed, a);
    myHC += TS * HelpersHBEFA::computeHC(c, newSpeed, a);
    myPMx += TS * HelpersHBEFA::computePMx(c, newSpeed, a);
    myNOx += TS * HelpersHBEFA::computeNOx(c, newSpeed, a);
    myFuel += TS * HelpersHBEFA::computeFuel(c, newSpeed, a);
    return true;
}


void
MSDevice_HBEFA::generateOutput() const {
    if (!OptionsCont::getOptions().isSet("tripinfo-output")) {
        return;
    }
    OutputDevice& os = OutputDevice::getDeviceByOption("tripinfo-output");
    os.openTag("emissions").writeAttr("CO_abs", OutputDevice::realString(myCO, 6))
    .writeAttr("CO2_abs", OutputDevice::realString(myCO2, 6))
    .writeAttr("HC_abs", OutputDevice::realString(myHC, 6))
    .writeAttr("PMx_abs", OutputDevice::realString(myPMx, 6))
    .writeAttr("NOx_abs", OutputDevice::realString(myNOx, 6))
    .writeAttr("fuel_abs", OutputDevice::realString(myFuel, 6)).closeTag();
}



/****************************************************************************/

