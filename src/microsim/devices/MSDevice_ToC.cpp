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
// The ToC Device controls the transition of control between automated and manual driving.
//
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/common/TplConvert.h>
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
#include "MSDevice_ToC.h"


// ===========================================================================
// debug constants
// ===========================================================================
//#define DEBUG_TOC


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
// The default value for the deceleration rate applied during a 'minimum risk maneuver'
#define DEFAULT_MRM_DECEL 1.5

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

    oc.doRegister("device.toc.manualType", new Option_String());
    oc.addDescription("device.toc.manualType", "ToC Device", "Vehicle type for manual driving regime.");
    oc.doRegister("device.toc.automatedType", new Option_String());
    oc.addDescription("device.toc.automatedType", "ToC Device", "Vehicle type for automated driving regime.");
    oc.doRegister("device.toc.responseTime", new Option_Float(DEFAULT_RESPONSE_TIME));
    oc.addDescription("device.toc.responseTime", "ToC Device", "Average response time needed by a driver to take back control.");
    oc.doRegister("device.toc.recoveryRate", new Option_Float(DEFAULT_RECOVERY_RATE));
    oc.addDescription("device.toc.recoveryRate", "ToC Device", "Recovery rate for the driver's awareness after a ToC.");
    oc.doRegister("device.toc.initialAwareness", new Option_Float(DEFAULT_INITIAL_AWARENESS));
    oc.addDescription("device.toc.initialAwareness", "ToC Device", "Average awareness a driver has initially after a ToC.");
    oc.doRegister("device.toc.mrmDecel", new Option_Float(DEFAULT_MRM_DECEL));
    oc.addDescription("device.toc.mrmDecel", "ToC Device", "Deceleration rate applied during a 'minimum risk maneuver'.");
    oc.doRegister("device.toc.useColorScheme", new Option_Bool(true));
    oc.addDescription("device.toc.useColorScheme", "ToC Device", "Whether a coloring scheme shall by applied to indicate the different ToC stages.");
}


void
MSDevice_ToC::buildVehicleDevices(SUMOVehicle& v, std::vector<MSDevice*>& into) {
    OptionsCont& oc = OptionsCont::getOptions();
    if (equippedByDefaultAssignmentOptions(oc, "toc", v, false)) {
        std::string manualType = getManualType(v, oc);
        std::string automatedType = getAutomatedType(v, oc);
        SUMOTime responseTime = TIME2STEPS(getResponseTime(v, oc));
        double recoveryRate = getRecoveryRate(v, oc);
        double initialAwareness = getInitialAwareness(v, oc);
        double mrmDecel = getMRMDecel(v, oc);
        bool useColoring = useColorScheme(v, oc);
        // build the device
        MSDevice_ToC* device = new MSDevice_ToC(v, "toc_" + v.getID(),
                                                manualType, automatedType, responseTime, recoveryRate,
                                                initialAwareness, mrmDecel, useColoring);
        into.push_back(device);
    }
}

std::string
MSDevice_ToC::getManualType(const SUMOVehicle& v, const OptionsCont& oc) {
    return getStringParam(v, oc, "toc.manualType", DEFAULT_MANUAL_TYPE, true);
}

std::string
MSDevice_ToC::getAutomatedType(const SUMOVehicle& v, const OptionsCont& oc) {
    return getStringParam(v, oc, "toc.automatedType", DEFAULT_AUTOMATED_TYPE, true);
}

double
MSDevice_ToC::getResponseTime(const SUMOVehicle& v, const OptionsCont& oc) {
    return getFloatParam(v, oc, "toc.responseTime", DEFAULT_RESPONSE_TIME, false);
}

double
MSDevice_ToC::getRecoveryRate(const SUMOVehicle& v, const OptionsCont& oc) {
    return getFloatParam(v, oc, "toc.recoveryRate", DEFAULT_RECOVERY_RATE, false);
}

double
MSDevice_ToC::getInitialAwareness(const SUMOVehicle& v, const OptionsCont& oc) {
    return getFloatParam(v, oc, "toc.initialAwareness", DEFAULT_INITIAL_AWARENESS, false);
}

double
MSDevice_ToC::getMRMDecel(const SUMOVehicle& v, const OptionsCont& oc) {
    return getFloatParam(v, oc, "toc.mrmDecel", DEFAULT_MRM_DECEL, false);
}

bool
MSDevice_ToC::useColorScheme(const SUMOVehicle& v, const OptionsCont& oc) {
    return getBoolParam(v, oc, "toc.useColorScheme", "false", false);
}



// ---------------------------------------------------------------------------
// MSDevice_ToC-methods
// ---------------------------------------------------------------------------
MSDevice_ToC::MSDevice_ToC(SUMOVehicle& holder, const std::string& id,
                           std::string manualType, std::string automatedType,
                           SUMOTime responseTime, double recoveryRate, double initialAwareness,
                           double mrmDecel, bool useColoring) :
    MSDevice(holder, id),
    myManualTypeID(manualType),
    myAutomatedTypeID(automatedType),
    myResponseTime(responseTime),
    myRecoveryRate(recoveryRate),
    myInitialAwareness(initialAwareness),
    myMRMDecel(mrmDecel),
    myCurrentAwareness(1.),
    myUseColorScheme(useColoring),
    myTriggerMRMCommand(nullptr),
    myTriggerToCCommand(nullptr),
    myRecoverAwarenessCommand(nullptr),
    myExecuteMRMCommand(nullptr),
    myPrepareToCCommand(nullptr) {
    // Take care! Holder is currently being constructed. Cast occurs before completion.
    myHolderMS = static_cast<MSVehicle*>(&holder);
    // Ensure that the holder receives a driver state as soon as it is created (can't be done here, since myHolderMS is incomplete)
    MSNet::getInstance()->getBeginOfTimestepEvents()->addEvent(new WrappingCommand<MSDevice_ToC>(this, &MSDevice_ToC::ensureDriverStateExistence), SIMSTEP);


    // Check if the given vTypes for the ToC Device are vTypeDistributions
    MSVehicleControl& vehCtrl = MSNet::getInstance()->getVehicleControl();
    const bool automatedVTypeIsDist = vehCtrl.hasVTypeDistribution(myAutomatedTypeID);
    const bool manualVTypeIsDist = vehCtrl.hasVTypeDistribution(myManualTypeID);

    // Check if the vType of the holder matches one of the given vTypes
    std::string holderVTypeID = holder.getVehicleType().getID();
    if (holderVTypeID == myManualTypeID) {
        myState = ToCState::MANUAL;
    } else if (holderVTypeID == myAutomatedTypeID) {
        myState = ToCState::AUTOMATED;
    } else if (manualVTypeIsDist && holderVTypeID.find(myManualTypeID) == 0) {
        // Holder type id starts with type distribution name.
        // We assume that this means that it is from the given distribution.
        myState = ToCState::MANUAL;
        myManualTypeID = holderVTypeID;
    } else if (automatedVTypeIsDist && holderVTypeID.find(myAutomatedTypeID) == 0) {
        // Holder type id starts with type distribution name.
        // We assume that this means that it is from the given distribution.
        myState = ToCState::AUTOMATED;
        myAutomatedTypeID = holderVTypeID;
    } else {
        throw ProcessError("Vehicle type of vehicle '" + holder.getID() + "' ('" + holder.getVehicleType().getID()
                           + "') must coincide with manualType ('" + manualType + "') or automatedType ('" + automatedType
                           + "') specified for its ToC-device (or drawn from the specified vTypeDistributions).");
    }

    // Eventually instantiate given vTypes from distributions
    if (myState == ToCState::MANUAL && automatedVTypeIsDist) {
        myAutomatedTypeID = vehCtrl.getVType(myAutomatedTypeID, MSRouteHandler::getParsingRNG())->getID();
    } else if (myState == ToCState::AUTOMATED && manualVTypeIsDist) {
        myManualTypeID = vehCtrl.getVType(myManualTypeID, MSRouteHandler::getParsingRNG())->getID();
    }

    initColorScheme();

#ifdef DEBUG_TOC
    std::cout << "initialized device '" << id << "' with "
              << "myManualType=" << myManualTypeID << ", "
              << "myAutomatedType=" << myAutomatedTypeID << ", "
              << "myResponseTime=" << myResponseTime << ", "
              << "myRecoveryRate=" << myRecoveryRate << ", "
              << "myInitialAwareness=" << myInitialAwareness << ", "
              << "myMRMDecel=" << myMRMDecel << ", "
              << "myCurrentAwareness=" << myCurrentAwareness << ", "
              << "myState=" << _2string(myState) << std::endl;
#endif

    assert(myInitialAwareness <= 1.0 && myInitialAwareness >= 0.0);
}



void
MSDevice_ToC::initColorScheme() {
    //RGBColor(red, green, blue)
    myColorScheme[MANUAL] = MSNet::getInstance()->getVehicleControl().getVType(myManualTypeID)->getColor();
    myColorScheme[AUTOMATED] = MSNet::getInstance()->getVehicleControl().getVType(myAutomatedTypeID)->getColor();
    myColorScheme[PREPARING_TOC] = RGBColor(200, 200, 250); // light blue
    myColorScheme[MRM] = RGBColor(250, 50, 50); // red
    myColorScheme[RECOVERING] = RGBColor(250, 210, 150); // light yellow
    myColorScheme[UNDEFINED] = RGBColor(150, 150, 150); // gray
}


SUMOTime
MSDevice_ToC::ensureDriverStateExistence(SUMOTime /* t */) {
#ifdef DEBUG_TOC
    std::cout << SIMTIME << " ensureDriverStateExistence() for vehicle '" << myHolder.getID() << "'" << std::endl;
#endif
    // Ensure that the holder has a driver state
    if (myHolderMS->getDriverState() == nullptr
            && !myHolderMS->hasDevice("driverstate")) {
        // Create an MSDriverState for the vehicle if it hasn't one already,
        // and has no DriverState Device attached (that will create the driver state, then)
        myHolderMS->createDriverState();
    }
    return 0;
}

MSDevice_ToC::~MSDevice_ToC() {
    // deschedule commands associated to this device
    if (myTriggerMRMCommand != nullptr) {
        myTriggerMRMCommand->deschedule();
    }
    if (myTriggerToCCommand != nullptr) {
        myTriggerToCCommand->deschedule();
    }
    if (myRecoverAwarenessCommand != nullptr) {
        myRecoverAwarenessCommand->deschedule();
    }
    if (myExecuteMRMCommand != nullptr) {
        myExecuteMRMCommand->deschedule();
    }
    if (myPrepareToCCommand != nullptr) {
        myPrepareToCCommand->deschedule();
    }
}

void
MSDevice_ToC::setAwareness(double value) {
    if (value > 1.0 || value < 0.0) {
        std::stringstream ss;
        ss << "Truncating invalid value for awareness (" << value << ") to lie in [0,1].";
        WRITE_WARNING(ss.str());
        value = MAX2(0.0, MIN2(1.0, value));
    }
    myCurrentAwareness = value;
    std::shared_ptr<MSSimpleDriverState> ds = myHolderMS->getDriverState();
    ds->setAwareness(value);
}


void
MSDevice_ToC::setState(ToCState state) {
    myState = state;
    if (myUseColorScheme) {
        setVehicleColor();
    }
}

void
MSDevice_ToC::setVehicleColor() {
    const SUMOVehicleParameter& p = myHolder.getParameter();
    p.color = myColorScheme[myState];
    p.parametersSet |= VEHPARS_COLOR_SET;
}

void
MSDevice_ToC::requestMRM() {
    // Clean up previous MRM Commands
    descheduleMRM();
    // Remove any preparatory process
    descheduleToCPreparation();
    // .. and any recovery process
    descheduleRecovery();
    // ... and any pending ToC to manual
    descheduleToC();
    // Immediately trigger the MRM process
    triggerMRM(0);
}


void
MSDevice_ToC::requestToC(SUMOTime timeTillMRM) {
#ifdef DEBUG_TOC
    std::cout << SIMTIME << " requestToC() for vehicle '" << myHolder.getID() << "' , timeTillMRM=" << timeTillMRM << std::endl;
#endif
    if (myState == AUTOMATED) {
        // Initialize preparation phase

        // @todo: Sample response time from distribution
        SUMOTime responseTime = myResponseTime;

        // Schedule ToC Event
        myTriggerToCCommand = new WrappingCommand<MSDevice_ToC>(this, &MSDevice_ToC::triggerDownwardToC);
        MSNet::getInstance()->getBeginOfTimestepEvents()->addEvent(myTriggerToCCommand, SIMSTEP + responseTime);

        // Clear eventually prior scheduled MRM
//        descheduleMRM();
        assert(myExecuteMRMCommand == nullptr);
        assert(myTriggerMRMCommand == nullptr);
        if (responseTime > timeTillMRM) {
            // Schedule new MRM if driver response time is higher than permitted
            myTriggerMRMCommand = new WrappingCommand<MSDevice_ToC>(this, &MSDevice_ToC::triggerMRM);
            MSNet::getInstance()->getBeginOfTimestepEvents()->addEvent(myTriggerMRMCommand, SIMSTEP + timeTillMRM);
        }

        // Start ToC preparation process
        myPrepareToCCommand = new WrappingCommand<MSDevice_ToC>(this, &MSDevice_ToC::ToCPreparationStep);
        MSNet::getInstance()->getBeginOfTimestepEvents()->addEvent(myPrepareToCCommand, SIMSTEP + DELTA_T);
        setState(PREPARING_TOC);
    } else {
        // Switch to automated mode is performed immediately
        // Note that the transition MRM/PREPARING_TOC->AUTOMATED, where a downward ToC is aborted, is handled here as well.
        if (timeTillMRM > 0.) {
            std::stringstream ss;
            ss << "[t=" << SIMTIME << "] Positive transition time (" << timeTillMRM / 1000. << "s.) for upward ToC of vehicle '" << myHolder.getID() << "' is ignored.";
            WRITE_WARNING(ss.str());
        }
        triggerUpwardToC(SIMSTEP + DELTA_T);
    }
}


SUMOTime
MSDevice_ToC::triggerMRM(SUMOTime /* t */) {
#ifdef DEBUG_TOC
    std::cout << SIMTIME << " triggerMRM() for vehicle '" << myHolder.getID() << "'" << std::endl;
#endif
    // Clear ongoing MRM
    descheduleMRM();

    // Start MRM process
    myExecuteMRMCommand = new WrappingCommand<MSDevice_ToC>(this, &MSDevice_ToC::MRMExecutionStep);
    MSNet::getInstance()->getBeginOfTimestepEvents()->addEvent(myExecuteMRMCommand, SIMSTEP + DELTA_T);
    setState(MRM);
    switchHolderType(myAutomatedTypeID);
    setAwareness(1.);

    return 0;
}


SUMOTime
MSDevice_ToC::triggerUpwardToC(SUMOTime /* t */) {
#ifdef DEBUG_TOC
    std::cout << SIMTIME << " triggerUpwardToC() for vehicle '" << myHolder.getID() << "'" << std::endl;
#endif
    switchHolderType(myAutomatedTypeID);
    setAwareness(1.);
    setState(AUTOMATED);

    descheduleToC();
    // Eventually stop ToC preparation process
    descheduleToCPreparation();
    // Eventually abort MRM
    descheduleMRM();
    // Eventually abort awareness recovery process
    descheduleRecovery();

    return 0;
}


SUMOTime
MSDevice_ToC::triggerDownwardToC(SUMOTime /* t */) {
#ifdef DEBUG_TOC
    std::cout << SIMTIME << " triggerDownwardToC() for vehicle '" << myHolder.getID() << "'" << std::endl;
#endif
    switchHolderType(myManualTypeID);

    // @todo: Sample initial awareness
    double initialAwareness = myInitialAwareness;
    setAwareness(initialAwareness);

#ifdef DEBUG_TOC
    std::cout << SIMTIME << " Initial awareness after ToC: " << myCurrentAwareness << std::endl;
#endif

    // Start awareness recovery process
    myRecoverAwarenessCommand = new WrappingCommand<MSDevice_ToC>(this, &MSDevice_ToC::awarenessRecoveryStep);
    MSNet::getInstance()->getBeginOfTimestepEvents()->addEvent(myRecoverAwarenessCommand, SIMSTEP + DELTA_T);
    setState(RECOVERING);

    descheduleToC();
    // Eventually stop ToC preparation process
    descheduleToCPreparation();
    // Eventually abort MRM
    descheduleMRM();

    return 0;
}

void
MSDevice_ToC::descheduleMRM() {
    // Eventually abort scheduled MRM
    if (myTriggerMRMCommand != nullptr) {
        myTriggerMRMCommand->deschedule();
        myTriggerMRMCommand = nullptr;
    }
    // Eventually abort ongoing MRM
    if (myExecuteMRMCommand != nullptr) {
        myExecuteMRMCommand->deschedule();
        myExecuteMRMCommand = nullptr;
    }
}


void
MSDevice_ToC::descheduleToC() {
    if (myTriggerToCCommand != nullptr) {
        myTriggerToCCommand->deschedule();
        myTriggerToCCommand = nullptr;
    }
}

void
MSDevice_ToC::descheduleToCPreparation() {
    // Eventually stop ToC preparation process
    if (myPrepareToCCommand != nullptr) {
        myPrepareToCCommand->deschedule();
        myPrepareToCCommand = nullptr;
    }
}

void
MSDevice_ToC::descheduleRecovery() {
    // Eventually stop ToC preparation process
    if (myRecoverAwarenessCommand != nullptr) {
        myRecoverAwarenessCommand->deschedule();
        myRecoverAwarenessCommand = nullptr;
    }
}


void
MSDevice_ToC::switchHolderType(const std::string& targetTypeID) {
#ifdef DEBUG_TOC
    std::cout << SIMTIME << " Switching type of vehicle '" << myHolder.getID() << "' to '" << targetTypeID << "'" << std::endl;
#endif
    MSVehicleType* targetType = MSNet::getInstance()->getVehicleControl().getVType(targetTypeID);
    if (targetType == 0) {
        WRITE_ERROR("vType '" + targetType->getID() + "' for vehicle '" + myHolder.getID() + "' is not known.");
        return;
    }
    myHolderMS->replaceVehicleType(targetType);
}


SUMOTime
MSDevice_ToC::ToCPreparationStep(SUMOTime /* t */) {
#ifdef DEBUG_TOC
    std::cout << SIMTIME << " ToC preparation step for vehicle '" << myHolder.getID() << "'" << std::endl;
#endif
    // TODO: Devise preparation of ToC (still needs discussion). At least: do not overtake. Perhaps, increase gap to leader.

    if (myState == PREPARING_TOC) {
        return DELTA_T;
    } else {
#ifdef DEBUG_TOC
        std::cout << SIMTIME << " Aborting ToC preparation for vehicle '" << myHolder.getID() << "'" << std::endl;
#endif
        descheduleToCPreparation();
        return 0;
    }
}


SUMOTime
MSDevice_ToC::MRMExecutionStep(SUMOTime t) {
    const double currentSpeed = myHolderMS->getSpeed();
#ifdef DEBUG_TOC
    std::cout << SIMTIME << " MRM step for vehicle '" << myHolder.getID() << "', currentSpeed=" << currentSpeed << std::endl;
#endif

    // Induce slowdown with MRMDecel
    std::vector<std::pair<SUMOTime, double> > speedTimeLine;
    const double nextSpeed = MAX2(0., currentSpeed - ACCEL2SPEED(myMRMDecel));
    speedTimeLine.push_back(std::make_pair(t - DELTA_T, currentSpeed));
    speedTimeLine.push_back(std::make_pair(t, nextSpeed));
    myHolderMS->getInfluencer().setSpeedTimeLine(speedTimeLine);

    // Prevent lane changes during MRM
    // TODO: seems not to function
    std::vector<std::pair<SUMOTime, int> > laneTimeLine;
    const int currentLane = myHolderMS->getLaneIndex();
    laneTimeLine.push_back(std::make_pair(t - DELTA_T, currentLane));
    laneTimeLine.push_back(std::make_pair(t, currentLane));
    myHolderMS->getInfluencer().setLaneTimeLine(laneTimeLine);

    if (myState == MRM) {
        return DELTA_T;
    } else {
#ifdef DEBUG_TOC
        std::cout << SIMTIME << " Aborting MRM for vehicle '" << myHolder.getID() << "'" << std::endl;
#endif
        return 0;
    }
}


SUMOTime
MSDevice_ToC::awarenessRecoveryStep(SUMOTime /* t */) {
#ifdef DEBUG_TOC
    std::cout << SIMTIME << " Awareness recovery step for vehicle '" << myHolder.getID() << "'" << std::endl;
#endif
    // Proceed with awareness recovery
    if (myCurrentAwareness < 1.0) {
        setAwareness(MIN2(1.0, myCurrentAwareness + TS * myRecoveryRate));
    }

#ifdef DEBUG_TOC
    std::cout << SIMTIME << " currentAwareness = " << myCurrentAwareness << std::endl;
#endif

    const bool awarenessRecoveryCompleted = myCurrentAwareness == 1.0;
    if (awarenessRecoveryCompleted) {
#ifdef DEBUG_TOC
        std::cout << SIMTIME << " Awareness recovery completed for veh '" << myHolder.getID() << "'" << std::endl;
#endif
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
        return myManualTypeID;
    } else if (key == "automatedType") {
        return myAutomatedTypeID;
    } else if (key == "responseTime") {
        return toString(STEPS2TIME(myResponseTime));
    } else if (key == "recoveryRate") {
        return toString(myRecoveryRate);
    } else if (key == "initialAwareness") {
        return toString(myInitialAwareness);
    } else if (key == "mrmDecel") {
        return toString(myMRMDecel);
    } else if (key == "currentAwareness") {
        return toString(myCurrentAwareness);
    } else if (key == "state") {
        return _2string(myState);
    } else if (key == "holder") {
        return myHolder.getID();
    }
    throw InvalidArgument("Parameter '" + key + "' is not supported for device of type '" + deviceName() + "'");
}


void
MSDevice_ToC::setParameter(const std::string& key, const std::string& value) {
#ifdef DEBUG_TOC
    std::cout << "MSDevice_ToC::setParameter(key=" << key << ", value=" << value << ")" << std::endl;
#endif
    if (key == "manualType") {
        myManualTypeID = value;
        myColorScheme[MANUAL] = MSNet::getInstance()->getVehicleControl().getVType(myManualTypeID)->getColor();
        if (myState == MANUAL) {
            switchHolderType(value);
        }
    } else if (key == "automatedType") {
        myAutomatedTypeID = value;
        myColorScheme[AUTOMATED] = MSNet::getInstance()->getVehicleControl().getVType(myAutomatedTypeID)->getColor();
        if (myState == AUTOMATED || myState == PREPARING_TOC || myState == MRM) {
            switchHolderType(value);
        }
    } else if (key == "responseTime") {
        myResponseTime = TIME2STEPS(TplConvert::_2double(value.c_str()));
    } else if (key == "recoveryRate") {
        myRecoveryRate = TplConvert::_2double(value.c_str());
    } else if (key == "initialAwareness") {
        myInitialAwareness = TplConvert::_2double(value.c_str());
    } else if (key == "currentAwareness") {
        myCurrentAwareness = TplConvert::_2double(value.c_str());
    } else if (key == "mrmDecel") {
        myMRMDecel = TplConvert::_2double(value.c_str());
    } else if (key == "requestToC") {
        // setting this magic parameter gives the interface for inducing a ToC
        const SUMOTime timeTillMRM = TIME2STEPS(TplConvert::_2double(value.c_str()));
        requestToC(timeTillMRM);
    } else if (key == "requestMRM") {
        // setting this magic parameter gives the interface for inducing an MRM
        requestMRM();
    } else if (key == "awareness") {
        // setting this magic parameter gives the interface for setting the driverstate's awareness
        setAwareness(TplConvert::_2double(value.c_str()));
    } else {
        throw InvalidArgument("Parameter '" + key + "' is not supported for device of type '" + deviceName() + "'");
    }
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
    } else if (str == "RECOVERING") {
        return RECOVERING;
    } else {
        WRITE_WARNING("Unknown ToCState '" + str + "'");
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
    } else if (state == RECOVERING) {
        return "RECOVERING";
    } else {
        WRITE_WARNING("Unknown ToCState '" + toString(state) + "'");
        return toString(state);
    }
}

/****************************************************************************/

