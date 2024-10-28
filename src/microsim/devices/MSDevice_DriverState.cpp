/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2013-2024 German Aerospace Center (DLR) and others.
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
/// @file    MSDevice_DriverState.cpp
/// @author  Leonhard Luecken
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    15.06.2018
///
/// The Driver State Device mainly provides a configuration and interaction interface for the vehicle's driver state.
/// @see microsim/MSDriverState.h
///
/****************************************************************************/
#include <config.h>

#include <utils/common/StringUtils.h>
#include <utils/options/OptionsCont.h>
#include <utils/vehicle/SUMOVehicle.h>
#include <utils/common/WrappingCommand.h>
#include <utils/common/RGBColor.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSRouteHandler.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSDriverState.h>
#include "MSDevice_DriverState.h"


// ===========================================================================
// debug constants
// ===========================================================================
//#define DEBUG_DSDEVICE
//#define DEBUG_COND (myHolder.isSelected())


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
void
MSDevice_DriverState::insertOptions(OptionsCont& oc) {
    oc.addOptionSubTopic("Driver State Device");
    insertDefaultAssignmentOptions("driverstate", "Driver State Device", oc);
    oc.doRegister("device.driverstate.initialAwareness", new Option_Float(DriverStateDefaults::initialAwareness));
    oc.addDescription("device.driverstate.initialAwareness", "Driver State Device", TL("Initial value assigned to the driver's awareness."));
    oc.doRegister("device.driverstate.errorTimeScaleCoefficient", new Option_Float(DriverStateDefaults::errorTimeScaleCoefficient));
    oc.addDescription("device.driverstate.errorTimeScaleCoefficient", "Driver State Device", TL("Time scale for the error process."));
    oc.doRegister("device.driverstate.errorNoiseIntensityCoefficient", new Option_Float(DriverStateDefaults::errorNoiseIntensityCoefficient));
    oc.addDescription("device.driverstate.errorNoiseIntensityCoefficient", "Driver State Device", TL("Noise intensity driving the error process."));
    oc.doRegister("device.driverstate.speedDifferenceErrorCoefficient", new Option_Float(DriverStateDefaults::speedDifferenceErrorCoefficient));
    oc.addDescription("device.driverstate.speedDifferenceErrorCoefficient", "Driver State Device", TL("General scaling coefficient for applying the error to the perceived speed difference (error also scales with distance)."));
    oc.doRegister("device.driverstate.headwayErrorCoefficient", new Option_Float(DriverStateDefaults::headwayErrorCoefficient));
    oc.addDescription("device.driverstate.headwayErrorCoefficient", "Driver State Device", TL("General scaling coefficient for applying the error to the perceived distance (error also scales with distance)."));
    oc.doRegister("device.driverstate.freeSpeedErrorCoefficient", new Option_Float(DriverStateDefaults::freeSpeedErrorCoefficient));
    oc.addDescription("device.driverstate.freeSpeedErrorCoefficient", "Driver State Device", TL("General scaling coefficient for applying the error to the vehicle's own speed when driving without a leader (error also scales with own speed)."));
    oc.doRegister("device.driverstate.speedDifferenceChangePerceptionThreshold", new Option_Float(DriverStateDefaults::speedDifferenceChangePerceptionThreshold));
    oc.addDescription("device.driverstate.speedDifferenceChangePerceptionThreshold", "Driver State Device", TL("Base threshold for recognizing changes in the speed difference (threshold also scales with distance)."));
    oc.doRegister("device.driverstate.headwayChangePerceptionThreshold", new Option_Float(DriverStateDefaults::headwayChangePerceptionThreshold));
    oc.addDescription("device.driverstate.headwayChangePerceptionThreshold", "Driver State Device", TL("Base threshold for recognizing changes in the headway (threshold also scales with distance)."));
    oc.doRegister("device.driverstate.minAwareness", new Option_Float(DriverStateDefaults::minAwareness));
    oc.addDescription("device.driverstate.minAwareness", "Driver State Device", TL("Minimal admissible value for the driver's awareness."));
    oc.doRegister("device.driverstate.maximalReactionTime", new Option_Float(-1.0));
    oc.addDescription("device.driverstate.maximalReactionTime", "Driver State Device", TL("Maximal reaction time (~action step length) induced by decreased awareness level (reached for awareness=minAwareness)."));
}


void
MSDevice_DriverState::buildVehicleDevices(SUMOVehicle& v, std::vector<MSVehicleDevice*>& into) {
    OptionsCont& oc = OptionsCont::getOptions();
    // ToC device implies driverstate
    if (equippedByDefaultAssignmentOptions(oc, "driverstate", v, false) || equippedByDefaultAssignmentOptions(oc, "toc", v, false)) {
        // build the device
        MSDevice_DriverState* device = new MSDevice_DriverState(v, "driverstate" + v.getID(),
                v.getFloatParam("device.driverstate.minAwareness"),
                v.getFloatParam("device.driverstate.initialAwareness"),
                v.getFloatParam("device.driverstate.errorTimeScaleCoefficient"),
                v.getFloatParam("device.driverstate.errorNoiseIntensityCoefficient"),
                v.getFloatParam("device.driverstate.speedDifferenceErrorCoefficient"),
                v.getFloatParam("device.driverstate.speedDifferenceChangePerceptionThreshold"),
                v.getFloatParam("device.driverstate.headwayChangePerceptionThreshold"),
                v.getFloatParam("device.driverstate.headwayErrorCoefficient"),
                v.getFloatParam("device.driverstate.freeSpeedErrorCoefficient"),
                v.getFloatParam("device.driverstate.maximalReactionTime"));
        into.push_back(device);
    }
}


// ---------------------------------------------------------------------------
// MSDevice_DriverState-methods
// ---------------------------------------------------------------------------
MSDevice_DriverState::MSDevice_DriverState(SUMOVehicle& holder, const std::string& id,
        double minAwareness,
        double initialAwareness,
        double errorTimeScaleCoefficient,
        double errorNoiseIntensityCoefficient,
        double speedDifferenceErrorCoefficient,
        double speedDifferenceChangePerceptionThreshold,
        double headwayChangePerceptionThreshold,
        double headwayErrorCoefficient,
        double freeSpeedErrorCoefficient,
        double maximalReactionTime) :
    MSVehicleDevice(holder, id),
    myMinAwareness(minAwareness),
    myInitialAwareness(initialAwareness),
    myErrorTimeScaleCoefficient(errorTimeScaleCoefficient),
    myErrorNoiseIntensityCoefficient(errorNoiseIntensityCoefficient),
    mySpeedDifferenceErrorCoefficient(speedDifferenceErrorCoefficient),
    mySpeedDifferenceChangePerceptionThreshold(speedDifferenceChangePerceptionThreshold),
    myHeadwayChangePerceptionThreshold(headwayChangePerceptionThreshold),
    myHeadwayErrorCoefficient(headwayErrorCoefficient),
    myFreeSpeedErrorCoefficient(freeSpeedErrorCoefficient),
    myMaximalReactionTime(maximalReactionTime) {
    // Take care! Holder is currently being constructed. Cast occurs before completion.
    myHolderMS = static_cast<MSVehicle*>(&holder);
    initDriverState();


#ifdef DEBUG_DSDEVICE
    std::cout << "initialized device '" << id << "' with "
              << "myMinAwareness=" << myMinAwareness << ", "
              << "myInitialAwareness=" << myInitialAwareness << ", "
              << "myErrorTimeScaleCoefficient=" << myErrorTimeScaleCoefficient << ", "
              << "myErrorNoiseIntensityCoefficient=" << myErrorNoiseIntensityCoefficient << ", "
              << "mySpeedDifferenceErrorCoefficient=" << mySpeedDifferenceErrorCoefficient << ", "
              << "mySpeedDifferenceChangePerceptionThreshold=" << mySpeedDifferenceChangePerceptionThreshold << ", "
              << "myHeadwayChangePerceptionThreshold=" << myHeadwayChangePerceptionThreshold << ", "
              << "myHeadwayErrorCoefficient=" << myHeadwayErrorCoefficient << std::endl;
            << "myFreeSpeedErrorCoefficient=" << myFreeSpeedErrorCoefficient << std::endl;
#endif

}

void
MSDevice_DriverState::initDriverState() {
    myDriverState = std::make_shared<MSSimpleDriverState>(myHolderMS);
    myDriverState->setMinAwareness(myMinAwareness);
    myDriverState->setInitialAwareness(myInitialAwareness);
    myDriverState->setErrorTimeScaleCoefficient(myErrorTimeScaleCoefficient);
    myDriverState->setErrorNoiseIntensityCoefficient(myErrorNoiseIntensityCoefficient);
    myDriverState->setSpeedDifferenceErrorCoefficient(mySpeedDifferenceErrorCoefficient);
    myDriverState->setHeadwayErrorCoefficient(myHeadwayErrorCoefficient);
    myDriverState->setFreeSpeedErrorCoefficient(myFreeSpeedErrorCoefficient);
    myDriverState->setSpeedDifferenceChangePerceptionThreshold(mySpeedDifferenceChangePerceptionThreshold);
    myDriverState->setHeadwayChangePerceptionThreshold(myHeadwayChangePerceptionThreshold);
    myDriverState->setAwareness(myInitialAwareness);
    if (myMaximalReactionTime > 0) {
        myDriverState->setMaximalReactionTime(myMaximalReactionTime);
    }
}

void
MSDevice_DriverState::update() {
    myDriverState->update();
}

std::string
MSDevice_DriverState::getParameter(const std::string& key) const {
#ifdef DEBUG_DSDEVICE
    std::cout << "MSDevice_DriverState::getParameter(key=" << key << ")" << std::endl;
#endif
    if (key == "awareness") {
        return toString(myDriverState->getAwareness());
    } else if (key == "errorState") {
        return toString(myDriverState->getErrorState());
    } else if (key == "errorTimeScale") {
        return toString(myDriverState->getErrorTimeScale());
    } else if (key == "errorNoiseIntensity") {
        return toString(myDriverState->getErrorNoiseIntensity());
    } else if (key == "minAwareness") {
        return toString(myDriverState->getMinAwareness());
    } else if (key == "initialAwareness") {
        return toString(myDriverState->getInitialAwareness());
    } else if (key == "errorTimeScaleCoefficient") {
        return toString(myDriverState->getErrorTimeScaleCoefficient());
    } else if (key == "errorNoiseIntensityCoefficient") {
        return toString(myDriverState->getErrorNoiseIntensityCoefficient());
    } else if (key == "speedDifferenceErrorCoefficient") {
        return toString(myDriverState->getSpeedDifferenceErrorCoefficient());
    } else if (key == "headwayErrorCoefficient") {
        return toString(myDriverState->getHeadwayErrorCoefficient());
    } else if (key == "speedDifferenceChangePerceptionThreshold") {
        return toString(myDriverState->getSpeedDifferenceChangePerceptionThreshold());
    } else if (key == "headwayChangePerceptionThreshold") {
        return toString(myDriverState->getHeadwayChangePerceptionThreshold());
    } else if (key == "maximalReactionTime") {
        return toString(myDriverState->getMaximalReactionTime());
    } else if (key == "originalReactionTime") {
        return toString(myDriverState->getOriginalReactionTime());
    } else if (key == "actionStepLength") {
        return toString(myDriverState->getActionStepLength());
    }
    throw InvalidArgument("Parameter '" + key + "' is not supported for device of type '" + deviceName() + "'");
}


void
MSDevice_DriverState::setParameter(const std::string& key, const std::string& value) {
#ifdef DEBUG_DSDEVICE
    std::cout << "MSDevice_DriverState::setParameter(key=" << key << ", value=" << value << ")" << std::endl;
#endif
    if (key == "awareness") {
        myDriverState->setAwareness(StringUtils::toDouble(value));
    } else if (key == "errorState") {
        myDriverState->setErrorState(StringUtils::toDouble(value));
    } else if (key == "errorTimeScale") {
        myDriverState->setErrorTimeScale(StringUtils::toDouble(value));
    } else if (key == "errorNoiseIntensity") {
        myDriverState->setErrorNoiseIntensity(StringUtils::toDouble(value));
    } else if (key == "minAwareness") {
        myDriverState->setMinAwareness(StringUtils::toDouble(value));
    } else if (key == "initialAwareness") {
        myDriverState->setInitialAwareness(StringUtils::toDouble(value));
    } else if (key == "errorTimeScaleCoefficient") {
        myDriverState->setErrorTimeScaleCoefficient(StringUtils::toDouble(value));
    } else if (key == "errorNoiseIntensityCoefficient") {
        myDriverState->setErrorNoiseIntensityCoefficient(StringUtils::toDouble(value));
    } else if (key == "speedDifferenceErrorCoefficient") {
        myDriverState->setSpeedDifferenceErrorCoefficient(StringUtils::toDouble(value));
    } else if (key == "headwayErrorCoefficient") {
        myDriverState->setHeadwayErrorCoefficient(StringUtils::toDouble(value));
    } else if (key == "freeSpeedErrorCoefficient") {
        myDriverState->setFreeSpeedErrorCoefficient(StringUtils::toDouble(value));
    } else if (key == "speedDifferenceChangePerceptionThreshold") {
        myDriverState->setSpeedDifferenceChangePerceptionThreshold(StringUtils::toDouble(value));
    } else if (key == "headwayChangePerceptionThreshold") {
        myDriverState->setHeadwayChangePerceptionThreshold(StringUtils::toDouble(value));
    } else if (key == "maximalReactionTime") {
        myDriverState->setMaximalReactionTime(StringUtils::toDouble(value));
    } else if (key == "originalReactionTime") {
        myDriverState->setOriginalReactionTime(StringUtils::toDouble(value));
    } else {
        throw InvalidArgument("Parameter '" + key + "' is not supported for device of type '" + deviceName() + "'");
    }
}


/****************************************************************************/
