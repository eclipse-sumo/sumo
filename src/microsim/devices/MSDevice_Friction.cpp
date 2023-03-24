/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2013-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    MSDevice_Friction.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    11.06.2013
///
// A device which stands as an implementation Friction and which outputs movereminder calls
/****************************************************************************/
#include <config.h>

#include <utils/common/RandHelper.h>
#include <utils/common/StringUtils.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/vehicle/SUMOVehicle.h>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSVehicle.h>
#include "MSDevice_Tripinfo.h"
#include "MSDevice_Friction.h"


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
void
MSDevice_Friction::insertOptions(OptionsCont& oc) {
    oc.addOptionSubTopic("Friction Device");
    insertDefaultAssignmentOptions("friction", "Friction Device", oc);
    oc.doRegister("device.friction.stdDev", new Option_Float(.1)); //default .1
    oc.addDescription("device.friction.stdDev", "Friction Device", TL("The measurement noise parameter which can be applied to the friction device"));
    oc.doRegister("device.friction.offset", new Option_Float(0.)); //default no offset
    oc.addDescription("device.friction.offset", "Friction Device", TL("The measurement offset parameter which can be applied to the friction device -> e.g. to force false measurements"));
}


void
MSDevice_Friction::buildVehicleDevices(SUMOVehicle& v, std::vector<MSVehicleDevice*>& into) {
    OptionsCont& oc = OptionsCont::getOptions();
    if (equippedByDefaultAssignmentOptions(oc, "friction", v, false)) {
        // build the device
        MSDevice_Friction* device = new MSDevice_Friction(v, "friction_" + v.getID(),
                getFloatParam(v, oc, "friction.stdDev", .1, false), // stdDev noise deviation
                getFloatParam(v, oc, "friction.offset", 0., false)); // static offset
        into.push_back(device);
    }
}


// ---------------------------------------------------------------------------
// MSDevice_Friction-methods
// ---------------------------------------------------------------------------
MSDevice_Friction::MSDevice_Friction(SUMOVehicle& holder, const std::string& id, double stdDev, double offset) :
    MSVehicleDevice(holder, id),
    myMeasuredFrictionCoefficient(1.),
    myRawFriction(1.),
    myStdDeviation(stdDev),
    myOffset(offset) {
}


MSDevice_Friction::~MSDevice_Friction() {
}


bool
MSDevice_Friction::notifyMove(SUMOTrafficObject& /* tObject */, double /* oldPos */,
                              double /* newPos */, double /* newSpeed */) {
    myRawFriction = myHolder.getLane()->getFrictionCoefficient();
    myMeasuredFrictionCoefficient = myOffset + RandHelper::randNorm(myRawFriction, myStdDeviation, myHolder.getRNG());
    return true; // keep the device
}


std::string
MSDevice_Friction::getParameter(const std::string& key) const {
    if (key == "frictionCoefficient") {
        return toString(myMeasuredFrictionCoefficient);
    } else if (key == "stdDev") {
        return toString(myStdDeviation);
    } else if (key == "offset") {
        return toString(myOffset);
    } else if (key == "rawFriction") {
        return toString(myRawFriction);
    }
    throw InvalidArgument("Parameter '" + key + "' is not supported for device of type '" + deviceName() + "'");
}


void
MSDevice_Friction::setParameter(const std::string& key, const std::string& value) {
    try {
        const double doubleValue = StringUtils::toDouble(value);
        if (key == "frictionCoefficient") {
            myMeasuredFrictionCoefficient = doubleValue;
        } else if (key == "stdDev") {
            myStdDeviation = doubleValue;
        } else if (key == "offset") {
            myOffset = doubleValue;
        } else {
            throw InvalidArgument("Setting parameter '" + key + "' is not supported for device of type '" + deviceName() + "'");
        }
    } catch (NumberFormatException&) {
        throw InvalidArgument("Setting parameter '" + key + "' requires a number for device of type '" + deviceName() + "'");
    }
}


/****************************************************************************/
