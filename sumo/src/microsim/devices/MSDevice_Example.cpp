/****************************************************************************/
/// @file    MSDevice_Example.cpp
/// @author  Jakob Erdmann
/// @date    11.06.2013
/// @version $Id$
///
// A device which stands as an implementation example and which outputs movereminder calls
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
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

#include <utils/common/TplConvert.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/SUMOVehicle.h>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSVehicle.h>
#include "MSDevice_Tripinfo.h"
#include "MSDevice_Example.h"

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
MSDevice_Example::insertOptions(OptionsCont& oc) {
    oc.addOptionSubTopic("Example Device");

    oc.doRegister("device.example.explicit", new Option_String());
    oc.addDescription("device.example.explicit", "Example Device", "Assign a device to named vehicles");

    oc.doRegister("device.example.parameter", new Option_Float(0.0));
    oc.addDescription("device.example.parameter", "Example Device", "An exemplary parameter which can be used by all instances of the example device");
}


void
MSDevice_Example::buildVehicleDevices(SUMOVehicle& v, std::vector<MSDevice*>& into) {
    OptionsCont& oc = OptionsCont::getOptions();
    if (equippedByDefaultAssignmentOptions(oc, "example", v)) {
        // build the device
        // get custom vehicle parameter
        SUMOReal customParameter2 = -1;
        if (v.getParameter().knowsParameter("example")) {
            try {
                customParameter2 = TplConvert::_2SUMOReal(v.getParameter().getParameter("example", "-1").c_str());
            } catch (...) {
                WRITE_WARNING("Invalid value '" + v.getParameter().getParameter("example", "-1") + "'for vehicle parameter 'example'");
            }

        } else {
            std::cout << "vehicle '" << v.getID() << "' does not supply vehicle parameter 'example'. Using default of " << customParameter2 << "\n";
        }
        // get custom vType parameter
        SUMOReal customParameter3 = -1;
        if (v.getVehicleType().getParameter().knowsParameter("example")) {
            try {
                customParameter3 = TplConvert::_2SUMOReal(v.getVehicleType().getParameter().getParameter("example", "-1").c_str());
            } catch (...) {
                WRITE_WARNING("Invalid value '" + v.getVehicleType().getParameter().getParameter("example", "-1") + "'for vType parameter 'example'");
            }

        } else {
            std::cout << "vehicle '" << v.getID() << "' does not supply vType parameter 'example'. Using default of " << customParameter3 << "\n";
        }
        MSDevice_Example* device = new MSDevice_Example(v, "example_" + v.getID(),
                oc.getFloat("device.example.parameter"),
                customParameter2,
                customParameter3);
        into.push_back(device);
    }
}


// ---------------------------------------------------------------------------
// MSDevice_Example-methods
// ---------------------------------------------------------------------------
MSDevice_Example::MSDevice_Example(SUMOVehicle& holder, const std::string& id,
                                   SUMOReal customValue1, SUMOReal customValue2, SUMOReal customValue3) :
    MSDevice(holder, id),
    myCustomValue1(customValue1),
    myCustomValue2(customValue2),
    myCustomValue3(customValue3) {
    std::cout << "initialized device '" << id << "' with myCustomValue1=" << myCustomValue1 << ", myCustomValue2=" << myCustomValue2 << ", myCustomValue3=" << myCustomValue3 << "\n";
}


MSDevice_Example::~MSDevice_Example() {
}


bool
MSDevice_Example::notifyMove(SUMOVehicle& veh, SUMOReal /* oldPos */,
                             SUMOReal /* newPos */, SUMOReal newSpeed) {
    std::cout << "device '" << getID() << "' notifyMove: newSpeed=" << newSpeed << "\n";
    // check whether another device is present on the vehicle:
    MSDevice_Tripinfo* otherDevice = static_cast<MSDevice_Tripinfo*>(veh.getDevice(typeid(MSDevice_Tripinfo)));
    if (otherDevice != 0) {
        std::cout << "  veh '" << veh.getID() << " has device '" << otherDevice->getID() << "'\n";
    }
    return true; // keep the device
}


bool
MSDevice_Example::notifyEnter(SUMOVehicle& veh, MSMoveReminder::Notification reason) {
    std::cout << "device '" << getID() << "' notifyEnter: reason=" << reason << " currentEdge=" << veh.getEdge()->getID() << "\n";
    return true; // keep the device
}


bool
MSDevice_Example::notifyLeave(SUMOVehicle& veh, SUMOReal /*lastPos*/,
                              MSMoveReminder::Notification reason) {
    std::cout << "device '" << getID() << "' notifyLeave: reason=" << reason << " currentEdge=" << veh.getEdge()->getID() << "\n";
    return true; // keep the device
}


void
MSDevice_Example::generateOutput() const {
    if (OptionsCont::getOptions().isSet("tripinfo-output")) {
        OutputDevice& os = OutputDevice::getDeviceByOption("tripinfo-output");
        os.openTag("example_device");
        os.writeAttr("customValue1", toString(myCustomValue1));
        os.writeAttr("customValue2", toString(myCustomValue2));
        os.closeTag();
    }
}



/****************************************************************************/

