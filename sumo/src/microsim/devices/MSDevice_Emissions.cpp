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
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
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
    OptionsCont& oc = OptionsCont::getOptions();

    oc.doRegister("device.emissions.probability", new Option_Float(0.));//!!! describe
    oc.addDescription("device.emissions.probability", "Emissions", "The probability for a vehicle to have an emission logging device");

    oc.doRegister("device.emissions.explicit", new Option_String());//!!! describe
    oc.addSynonyme("device.emissions.explicit", "device.emissions.knownveh", true);
    oc.addDescription("device.emissions.explicit", "Emissions", "Assign a device to named vehicles");

    oc.doRegister("device.emissions.deterministic", new Option_Bool(false)); //!!! describe
    oc.addDescription("device.emissions.deterministic", "Emissions", "The devices are set deterministic using a fraction of 1000");
}


void
MSDevice_Emissions::buildVehicleDevices(SUMOVehicle& v, std::vector<MSDevice*>& into) {
    OptionsCont& oc = OptionsCont::getOptions();
    if (oc.getFloat("device.emissions.probability") == 0 && !oc.isSet("device.emissions.explicit")) {
        // no route computation is modelled
        return;
    }
    // route computation is enabled
    bool haveByNumber = false;
    if (oc.getBool("device.emissions.deterministic")) {
        haveByNumber = MSNet::getInstance()->getVehicleControl().isInQuota(oc.getFloat("device.emissions.probability"));
    } else {
        haveByNumber = RandHelper::rand() <= oc.getFloat("device.emissions.probability");
    }
    bool haveByName = oc.isSet("device.emissions.explicit") && OptionsCont::getOptions().isInStringVector("device.emissions.explicit", v.getID());
    if (haveByNumber || haveByName) {
        // build the device
        MSDevice_Emissions* device = new MSDevice_Emissions(v, "emissions_" + v.getID());
        into.push_back(device);
    }
}


// ---------------------------------------------------------------------------
// MSDevice_Emissions-methods
// ---------------------------------------------------------------------------
MSDevice_Emissions::MSDevice_Emissions(SUMOVehicle& holder, const std::string& id)
    : MSDevice(holder, id),
      myCO2(0), myCO(0), myHC(0), myPMx(0), myNOx(0), myFuel(0) {
}


MSDevice_Emissions::~MSDevice_Emissions() {
}


bool
MSDevice_Emissions::notifyMove(SUMOVehicle& veh, SUMOReal /*oldPos*/, SUMOReal /*newPos*/, SUMOReal newSpeed) {
    const SUMOEmissionClass c = veh.getVehicleType().getEmissionClass();
    const SUMOReal a = veh.getAcceleration();
    const SUMOReal slope = veh.getSlope();
    myCO2 += TS * PollutantsInterface::computeCO2(c, newSpeed, a, slope);
    myCO += TS * PollutantsInterface::computeCO(c, newSpeed, a, slope);
    myHC += TS * PollutantsInterface::computeHC(c, newSpeed, a, slope);
    myPMx += TS * PollutantsInterface::computePMx(c, newSpeed, a, slope);
    myNOx += TS * PollutantsInterface::computeNOx(c, newSpeed, a, slope);
    myFuel += TS * PollutantsInterface::computeFuel(c, newSpeed, a, slope);
    return true;
}


void
MSDevice_Emissions::generateOutput() const {
    if (OptionsCont::getOptions().isSet("tripinfo-output")) {
        OutputDevice& os = OutputDevice::getDeviceByOption("tripinfo-output");
        (os.openTag("emissions") <<
         " CO_abs=\"" << OutputDevice::realString(myCO, 6) <<
         "\" CO2_abs=\"" << OutputDevice::realString(myCO2, 6) <<
         "\" HC_abs=\"" << OutputDevice::realString(myHC, 6) <<
         "\" PMx_abs=\"" << OutputDevice::realString(myPMx, 6) <<
         "\" NOx_abs=\"" << OutputDevice::realString(myNOx, 6) <<
         "\" fuel_abs=\"" << OutputDevice::realString(myFuel, 6) <<
         "\"").closeTag();
    }
}



/****************************************************************************/

