/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2013-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSDevice_ToC.cpp
/// @author  Leonhard Luecken
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    01.04.2018
/// @version $Id$
///
// The ToC Device controls transition of control between automated and manual driving.
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
#include <utils/vehicle/SUMOVehicle.h>
#include <utils/common/WrappingCommand.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSEventControl.h>
#include "MSDevice_ToC.h"


// ===========================================================================
// debug constants
// ===========================================================================
#define DEBUG_TOC


// ===========================================================================
// parameter defaults
// ===========================================================================

// default value for the average response time, that a driver needs to take back control
#define DEFAULT_RESPONSE_TIME 5.0
// default value for the average rate at which the driver's awareness recovers to
// 1.0 after a ToC has been performed
#define DEFAULT_RECOVERY_RATE 0.1
// The default value for the average awareness a driver has initially after a ToC
#define DEFAULT_INITIAL_AWARENESS 0.5

#define DEFAULT_MANUAL_TYPE ""
#define DEFAULT_AUTOMATED_TYPE ""

// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
void
MSDevice_ToC::insertOptions(OptionsCont& oc) {
    oc.addOptionSubTopic("ToC Device");
    insertDefaultAssignmentOptions("toc", "ToC Device", oc);

    oc.doRegister("device.toc.manualType", new Option_String(""));
    oc.addDescription("device.toc.manualType", "ToC Device", "Vehicle type for manual driving regime.");
    oc.doRegister("device.toc.automatedType", new Option_String(""));
    oc.addDescription("device.toc.automatedType", "ToC Device", "Vehicle type for automated driving regime.");
    oc.doRegister("device.toc.responseTime", new Option_Float(DEFAULT_RESPONSE_TIME));
    oc.addDescription("device.toc.responseTime", "ToC Device", "Average response time needed by a driver to take back control.");
    oc.doRegister("device.toc.recoveryRate", new Option_Float(DEFAULT_RECOVERY_RATE));
    oc.addDescription("device.toc.recoveryRate", "ToC Device", "Recovery rate for the driver's awareness after a ToC.");
    oc.doRegister("device.toc.initialAwareness", new Option_Float(DEFAULT_INITIAL_AWARENESS));
    oc.addDescription("device.toc.initialAwareness", "ToC Device", "Average awareness a driver has initially after a ToC.");
}


void
MSDevice_ToC::buildVehicleDevices(SUMOVehicle& v, std::vector<MSDevice*>& into) {
    OptionsCont& oc = OptionsCont::getOptions();
    if (equippedByDefaultAssignmentOptions(oc, "toc", v, false)) {
        std::string manualType = getManualType(v, oc);
        std::string automatedType = getAutomatedType(v, oc);
        double responseTime = getResponseTime(v, oc);
        double recoveryRate = getRecoveryRate(v, oc);
        double initialAwareness = getInitialAwareness(v, oc);
        // build the device
        MSDevice_ToC* device = new MSDevice_ToC(v, "toc_" + v.getID(),
                manualType, automatedType, responseTime, recoveryRate, initialAwareness);
        into.push_back(device);
    }
}



std::string
MSDevice_ToC::getStringParam(const SUMOVehicle& v, const OptionsCont& oc, std::string paramName, std::string deflt) {
    std::string result;
    if (v.getParameter().knowsParameter("device.toc."+paramName)) {
        try {
            result = v.getParameter().getParameter("device.toc."+paramName, "").c_str();
        } catch (...) {
            WRITE_WARNING("Invalid value '" + v.getParameter().getParameter("device.toc."+paramName, "") + "'for vehicle parameter 'toc."+paramName+"'");
        }
    } else if (v.getVehicleType().getParameter().knowsParameter("device.toc."+paramName)) {
        try {
            result = v.getVehicleType().getParameter().getParameter("device.toc."+paramName, "").c_str();
        } catch (...) {
            WRITE_WARNING("Invalid value '" + v.getVehicleType().getParameter().getParameter("device.toc."+paramName, "") + "'for vType parameter 'toc."+paramName+"'");
        }
    } else {
        result = oc.getString("device.toc."+paramName);
#ifdef DEBUG_TOC
        std::cout << "vehicle '" << v.getID() << "' does not supply vehicle parameter 'device.toc."+paramName+"'. Using default of '" << result << "'\n";
#endif
    }
    if (result == deflt) {
        WRITE_WARNING("No parameter 'device.toc."+paramName+"' given for vehicle '" + v.getID());
    }
    return result;
}




double
MSDevice_ToC::getFloatParam(const SUMOVehicle& v, const OptionsCont& oc, std::string paramName, double deflt) {
    double result;
    if (v.getParameter().knowsParameter("device.toc."+paramName)) {
        try {
            result = TplConvert::_2double(v.getParameter().getParameter("device.toc."+paramName, "").c_str());
        } catch (...) {
            WRITE_WARNING("Invalid value '" + v.getParameter().getParameter("device.toc."+paramName, "") + "'for vehicle parameter 'toc."+paramName+"'");
        }
    } else if (v.getVehicleType().getParameter().knowsParameter("device.toc."+paramName)) {
        try {
            result = TplConvert::_2double(v.getVehicleType().getParameter().getParameter("device.toc."+paramName, "").c_str());
        } catch (...) {
            WRITE_WARNING("Invalid value '" + v.getVehicleType().getParameter().getParameter("device.toc."+paramName, "") + "'for vType parameter 'toc."+paramName+"'");
        }
    } else {
        result = oc.getFloat("device.toc."+paramName);
#ifdef DEBUG_TOC
        std::cout << "vehicle '" << v.getID() << "' does not supply vehicle parameter 'device.toc."+paramName+"'. Using default of '" << result << "'\n";
#endif
    }
    if (result == deflt) {
        WRITE_WARNING("No parameter 'device.toc."+paramName+"' given for vehicle '" + v.getID());
    }
    return result;
}


std::string
MSDevice_ToC::getManualType(const SUMOVehicle& v, const OptionsCont& oc) {
    return getStringParam(v, oc, "manualType", DEFAULT_MANUAL_TYPE);
}

std::string
MSDevice_ToC::getAutomatedType(const SUMOVehicle& v, const OptionsCont& oc) {
    return getStringParam(v, oc, "automatedType", DEFAULT_AUTOMATED_TYPE);
}

double
MSDevice_ToC::getResponseTime(const SUMOVehicle& v, const OptionsCont& oc) {
    return getFloatParam(v, oc, "responseTime", DEFAULT_RESPONSE_TIME);
}

double
MSDevice_ToC::getRecoveryRate(const SUMOVehicle& v, const OptionsCont& oc) {
    return getFloatParam(v, oc, "recoveryRate", DEFAULT_RECOVERY_RATE);
}

double
MSDevice_ToC::getInitialAwareness(const SUMOVehicle& v, const OptionsCont& oc) {
    return getFloatParam(v, oc, "initialAwareness", DEFAULT_INITIAL_AWARENESS);
}



// ---------------------------------------------------------------------------
// MSDevice_ToC-methods
// ---------------------------------------------------------------------------
MSDevice_ToC::MSDevice_ToC(SUMOVehicle& holder, const std::string& id,
        std::string manualType, std::string automatedType,
        double responseTime, double recoveryRate, double initialAwareness) :
    MSDevice(holder, id),
    myManualType(manualType),
    myAutomatedType(automatedType),
    myResponseTime(responseTime),
    myRecoveryRate(recoveryRate),
    myInitialAwareness(initialAwareness),
    myTriggerMRMCommand(nullptr),
    myTriggerToCCommand(nullptr),
    myRecoverAwarenessCommand(nullptr),
    myExecuteMRMCommand(nullptr),
    myPrepareToCCommand(nullptr)
    {
    // Take care! Holder is currently being constructed. Cast occurs before completion.
    myHolderMS = static_cast<MSVehicle*>(&holder);

    if (holder.getVehicleType().getID() == manualType) {
        myState = ToCState::MANUAL;
        setAwareness(1.);
    } else if (holder.getVehicleType().getID() == automatedType) {
        myState = ToCState::AUTOMATED;
        setAwareness(0.);
    } else {
        myState = ToCState::UNDEFINED;
        setAwareness(1.);
    }

    std::cout << "initialized device '" << id << "' with "
            << "myManualType =" << myManualType << ", "
            << "myAutomatedType =" << myAutomatedType << ", "
            << "myResponseTime =" << myResponseTime << ", "
            << "myRecoveryRate =" << myRecoveryRate << ", "
            << "myInitialAwareness =" << myInitialAwareness << ", "
            << "myCurrentAwareness =" << myCurrentAwareness << ", "
            << "myState =" << myState << std::endl;

    assert(myInitialAwareness <= 1.0 && myInitialAwareness >= 0.0);
}


MSDevice_ToC::~MSDevice_ToC() {
    // deschedule commands associated to this device
    if (myTriggerMRMCommand != nullptr) myTriggerMRMCommand->deschedule();
    if (myTriggerToCCommand != nullptr) myTriggerToCCommand->deschedule();
    if (myRecoverAwarenessCommand != nullptr) myRecoverAwarenessCommand->deschedule();
    if (myExecuteMRMCommand != nullptr) myExecuteMRMCommand->deschedule();
    if (myPrepareToCCommand != nullptr) myPrepareToCCommand->deschedule();
}

void
MSDevice_ToC::setAwareness(double value) {
    /// @todo Set holder's driver state accordingly
    assert(value <= 1.0 && value >= 0.0);
    myCurrentAwareness = value;
}


void
MSDevice_ToC::setState(ToCState state) {
    myState = state;
}


void
MSDevice_ToC::requestToC(SUMOTime timeTillMRM) {
    if(myTriggerToCCommand != nullptr) {
        WRITE_WARNING("Ignoring ToC request for vehicle '" + myHolder.getID() + "', which is already performing a ToC.");
        return;
    }
    if (myState == MANUAL || myState == UNDEFINED) {
        // Switch to automated mode is performed immediately
        triggerUpwardToC(SIMSTEP + DELTA_T);
    } else if (myState == AUTOMATED || myState == MRM) {
        // @todo: Sample response time from distribution
        double responseTime = myResponseTime;

        // Schedule ToC Event
        myTriggerToCCommand = new WrappingCommand<MSDevice_ToC>(this, &MSDevice_ToC::triggerDownwardToC);
        MSNet::getInstance()->getBeginOfTimestepEvents()->addEvent(myTriggerToCCommand, SIMSTEP + responseTime);

        if (responseTime > timeTillMRM) {
            // Schedule MRM Event
            myTriggerMRMCommand = new WrappingCommand<MSDevice_ToC>(this, &MSDevice_ToC::triggerMRM);
            MSNet::getInstance()->getBeginOfTimestepEvents()->addEvent(myTriggerMRMCommand, SIMSTEP + timeTillMRM);
        }

        // Start ToC preparation process
        myPrepareToCCommand = new WrappingCommand<MSDevice_ToC>(this, &MSDevice_ToC::ToCPreparationStep);
        MSNet::getInstance()->getBeginOfTimestepEvents()->addEvent(myPrepareToCCommand, SIMSTEP + DELTA_T);
        setState(PREPARING_TOC);
    }
}

SUMOTime
MSDevice_ToC::triggerMRM(SUMOTime t) {
    // Start MRM process
    myExecuteMRMCommand = new WrappingCommand<MSDevice_ToC>(this, &MSDevice_ToC::MRMExecutionStep);
    MSNet::getInstance()->getBeginOfTimestepEvents()->addEvent(myExecuteMRMCommand, SIMSTEP + DELTA_T);
    setState(MRM);

    if(myTriggerMRMCommand != nullptr) {
        myTriggerMRMCommand->deschedule();
        myTriggerMRMCommand = nullptr;
    }

    return 0;
}

SUMOTime
MSDevice_ToC::triggerUpwardToC(SUMOTime t) {
    switchHolderType(myAutomatedType);
    setAwareness(0.);
    setState(AUTOMATED);

    if(myTriggerToCCommand != nullptr) {
        myTriggerToCCommand->deschedule();
        myTriggerToCCommand = nullptr;
    }

    return 0;
}

SUMOTime
MSDevice_ToC::triggerDownwardToC(SUMOTime t) {
    switchHolderType(myManualType);
    // @todo: Sample initial awareness
    double initialAwareness = myInitialAwareness;
    setAwareness(initialAwareness);

    // Start awareness recovery process
    myRecoverAwarenessCommand = new WrappingCommand<MSDevice_ToC>(this, &MSDevice_ToC::awarenessRecoveryStep);
    MSNet::getInstance()->getBeginOfTimestepEvents()->addEvent(myRecoverAwarenessCommand, SIMSTEP + DELTA_T);
    setState(RECOVERING);

    if(myTriggerToCCommand != nullptr) {
        myTriggerToCCommand->deschedule();
        myTriggerToCCommand = nullptr;
    }

    // Eventually stop ToC preparation process
    if(myPrepareToCCommand != nullptr) {
        myPrepareToCCommand->deschedule();
        myPrepareToCCommand = nullptr;
    }

    // Eventually abort MRM
    if(myExecuteMRMCommand != nullptr) {
        myExecuteMRMCommand->deschedule();
        myExecuteMRMCommand = nullptr;
    }

    return 0;
}


void
MSDevice_ToC::switchHolderType(const std::string& targetTypeID) {
    MSVehicleType* targetType = MSNet::getInstance()->getVehicleControl().getVType(targetTypeID);
    if (targetType == 0) {
        WRITE_ERROR("vType '" + targetType->getID() + "' for vehicle '" + myHolder.getID() + "' is not known.");
        return;
    }
    myHolderMS->replaceVehicleType(targetType);
}


SUMOTime
MSDevice_ToC::ToCPreparationStep(SUMOTime t) {
    // TODO
    return DELTA_T;
}


SUMOTime
MSDevice_ToC::MRMExecutionStep(SUMOTime t) {
    // TODO
    return DELTA_T;
}


SUMOTime
MSDevice_ToC::awarenessRecoveryStep(SUMOTime t) {
    // Proceed with awareness recovery
    if (myCurrentAwareness < 1.0) {
        setAwareness(MIN2(1.0, myCurrentAwareness + TS*myRecoveryRate));
    }
    const bool awarenessRecoveryCompleted = myCurrentAwareness == 1.0;
    if (awarenessRecoveryCompleted) {
        myRecoverAwarenessCommand->deschedule();
        myRecoverAwarenessCommand = nullptr;
        setState(MANUAL);
        return 0;
    }
    return DELTA_T;
}


std::string
MSDevice_ToC::getParameter(const std::string& key) const {
    if (key == "manualType") {
        return myManualType;
    } else if (key == "automatedType") {
        return myAutomatedType;
    } else if (key == "responseTime") {
        return toString(myResponseTime);
    } else if (key == "recoveryRate") {
        return toString(myRecoveryRate);
    } else if (key == "initialAwareness") {
        return toString(myInitialAwareness);
    } else if (key == "currentAwareness") {
        return toString(myCurrentAwareness);
    } else if (key == "state") {
        return _2string(myState);
    }
    throw InvalidArgument("Parameter '" + key + "' is not supported for device of type '" + deviceName() + "'");
}


void
MSDevice_ToC::setParameter(const std::string& key, const std::string& value) {
    if (key == "manualType") {
        /// @todo induce vType switch for vehicle??
        myManualType = value;
    } else if (key == "automatedType") {
        /// @todo induce vType switch for vehicle??
        myAutomatedType = value;
    } else if (key == "responseTime") {
        myResponseTime = TplConvert::_2double(value.c_str());
    } else if (key == "recoveryRate") {
        myRecoveryRate = TplConvert::_2double(value.c_str());
    } else if (key == "initialAwareness") {
        myInitialAwareness = TplConvert::_2double(value.c_str());
    } else if (key == "currentAwareness") {
        myCurrentAwareness = TplConvert::_2double(value.c_str());
    } else if (key == "requestToC") {
        // setting this magic parameter gives the interface for inducing a ToC
        const SUMOTime timeTillMRM = TIME2STEPS(TplConvert::_2double(value.c_str()));
        requestToC(timeTillMRM);
    }
    throw InvalidArgument("Parameter '" + key + "' is not supported for device of type '" + deviceName() + "'");
}


MSDevice_ToC::ToCState
MSDevice_ToC::_2ToCState(const std::string& str) {
    if (str == "UNDEFINED") {
        return UNDEFINED;
    } else if (str == "MANUAL") {
        return MANUAL;
    } else if (str == "AUTOMATED") {
        return AUTOMATED;
    } else if (str == "PREPARING_TOC") {
        return PREPARING_TOC;
    } else if (str == "MRM") {
        return MRM;
    } else {
        WRITE_WARNING("Unknown ToCState '"+str+"'");
        return UNDEFINED;
    }
}


std::string
MSDevice_ToC::_2string(ToCState state) {
    if (state == UNDEFINED) {
        return "UNDEFINED";
    } else if (state == MANUAL) {
        return "MANUAL";
    } else if (state == AUTOMATED) {
        return "AUTOMATED";
    } else if (state == PREPARING_TOC) {
        return "PREPARING_TOC";
    } else if (state == MRM) {
        return "MRM";
    } else {
        WRITE_WARNING("Unknown ToCState '"+toString(state)+"'");
        return toString(state);
    }
}

/****************************************************************************/

