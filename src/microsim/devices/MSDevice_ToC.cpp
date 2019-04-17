/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2013-2019 German Aerospace Center (DLR) and others.
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

#include <memory>
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
#include "MSDevice_ToC.h"


// ===========================================================================
// debug constants
// ===========================================================================
//#define DEBUG_TOC
//#define DEBUG_DYNAMIC_TOC

// ===========================================================================
// parameter defaults
// ===========================================================================

// default value for the average response time, that a driver needs to take back control
#define DEFAULT_RESPONSE_TIME 5.0
// default value for the average rate at which the driver's awareness recovers to
// 1.0 after a ToC has been performed
#define DEFAULT_RECOVERY_RATE 0.1
// Default value of the awareness below which no lane-changes are performed
#define DEFAULT_LCABSTINENCE 0.0
// The default value for the average awareness a driver has initially after a ToC
#define DEFAULT_INITIAL_AWARENESS 0.5
// The default value for the deceleration rate applied during a 'minimum risk maneuver'
#define DEFAULT_MRM_DECEL 1.5
// The default value for the dynamic ToC threshold indicates that the dynamic ToCs are deactivated
#define DEFAULT_DYNAMIC_TOC_THRESHOLD 0.0

// The factor by which the dynamic ToC threshold time is multiplied to yield the lead time given for the corresponding ToC
#define DYNAMIC_TOC_LEADTIME_FACTOR 0.75
// A factor applied to the check for the dynamic ToC condition to resist aborting an ongoing dynamic ToC (and prevent oscillations)
#define DYNAMIC_TOC_ABORT_RESISTANCE_FACTOR 2.0


// The default values for the openGap parameters applied for gap creation in preparation for a ToC
#define DEFAULT_OPENGAP_TIMEGAP -1.0
#define DEFAULT_OPENGAP_SPACING 0.0
#define DEFAULT_OPENGAP_CHANGERATE 1.0
#define DEFAULT_OPENGAP_MAXDECEL 1.0


#define DEFAULT_MANUAL_TYPE ""
#define DEFAULT_AUTOMATED_TYPE ""




// ---------------------------------------------------------------------------
// static members
// ---------------------------------------------------------------------------
std::set<MSDevice_ToC*> MSDevice_ToC::instances = std::set<MSDevice_ToC*>();
std::set<std::string> MSDevice_ToC::createdOutputFiles;
int MSDevice_ToC::LCModeMRM = 768; // = 0b001100000000 - no autonomous changes, no speed adaptation

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
    oc.doRegister("device.toc.lcAbstinence", new Option_Float(DEFAULT_LCABSTINENCE));
    oc.addDescription("device.toc.lcAbstinence", "ToC Device", "Attention level below which a driver restrains from performing lane changes (value in [0,1]).");
    oc.doRegister("device.toc.initialAwareness", new Option_Float(DEFAULT_INITIAL_AWARENESS));
    oc.addDescription("device.toc.initialAwareness", "ToC Device", "Average awareness a driver has initially after a ToC (value in [0,1]).");
    oc.doRegister("device.toc.mrmDecel", new Option_Float(DEFAULT_MRM_DECEL));
    oc.addDescription("device.toc.mrmDecel", "ToC Device", "Deceleration rate applied during a 'minimum risk maneuver'.");
    oc.doRegister("device.toc.dynamicToCThreshold", new Option_Float(DEFAULT_DYNAMIC_TOC_THRESHOLD));
    oc.addDescription("device.toc.dynamicToCThreshold", "ToC Device", "Time, which the vehicle requires to have ahead to continue in automated mode. The default value of 0 indicates no dynamic triggering of ToCs.");
    oc.doRegister("device.toc.ogNewTimeHeadway", new Option_Float(-1.0));
    oc.addDescription("device.toc.ogNewTimeHeadway", "ToC Device", "Timegap for ToC preparation phase.");
    oc.doRegister("device.toc.ogNewSpaceHeadway", new Option_Float(-1.0));
    oc.addDescription("device.toc.ogNewSpaceHeadway", "ToC Device", "Additional spacing for ToC preparation phase.");
    oc.doRegister("device.toc.ogMaxDecel", new Option_Float(-1.0));
    oc.addDescription("device.toc.ogMaxDecel", "ToC Device", "Maximal deceleration applied for establishing increased gap in ToC preparation phase.");
    oc.doRegister("device.toc.ogChangeRate", new Option_Float(-1.0));
    oc.addDescription("device.toc.ogChangeRate", "ToC Device", "Rate of adaptation towards the increased headway during ToC preparation.");
    oc.doRegister("device.toc.useColorScheme", new Option_Bool(true));
    oc.addDescription("device.toc.useColorScheme", "ToC Device", "Whether a coloring scheme shall by applied to indicate the different ToC stages.");
    oc.doRegister("device.toc.file", new Option_String());
    oc.addDescription("device.toc.file", "ToC Device", "Switches on output by specifying an output filename.");
}


void
MSDevice_ToC::buildVehicleDevices(SUMOVehicle& v, std::vector<MSVehicleDevice*>& into) {
    OptionsCont& oc = OptionsCont::getOptions();
    if (equippedByDefaultAssignmentOptions(oc, "toc", v, false)) {
        std::string manualType = getManualType(v, oc);
        std::string automatedType = getAutomatedType(v, oc);
        SUMOTime responseTime = TIME2STEPS(getResponseTime(v, oc));
        double recoveryRate = getRecoveryRate(v, oc);
        double lcAbstinence = getLCAbstinence(v, oc);
        double initialAwareness = getInitialAwareness(v, oc);
        double mrmDecel = getMRMDecel(v, oc);
        bool useColoring = useColorScheme(v, oc);
        std::string deviceID = "toc_" + v.getID();
        std::string file = getOutputFilename(v, oc);
        OpenGapParams ogp = getOpenGapParams(v, oc);
        const double dynamicToCThreshold = getDynamicToCThreshold(v, oc);
        // build the device
        MSDevice_ToC* device = new MSDevice_ToC(v, deviceID, file,
                                                manualType, automatedType, responseTime, recoveryRate,
                                                lcAbstinence, initialAwareness, mrmDecel, dynamicToCThreshold,
												useColoring, ogp);
        into.push_back(device);
    }
}


std::string
MSDevice_ToC::getOutputFilename(const SUMOVehicle& v, const OptionsCont& oc) {
    // Default of "" means no output
    std::string file = "";
    if (v.getParameter().knowsParameter("device.toc.file")) {
        try {
            file = v.getParameter().getParameter("device.toc.file", file);
        } catch (...) {
            WRITE_WARNING("Invalid value '" + v.getParameter().getParameter("device.toc.file", file) + "'for vehicle parameter 'ssm.measures'");
        }
    } else if (v.getVehicleType().getParameter().knowsParameter("device.toc.file")) {
        try {
            file = v.getVehicleType().getParameter().getParameter("device.toc.file", file);
        } catch (...) {
            WRITE_WARNING("Invalid value '" + v.getVehicleType().getParameter().getParameter("device.toc.file", file) + "'for vType parameter 'ssm.measures'");
        }
    } else {
        file = oc.getString("device.toc.file") == "" ? file : oc.getString("device.toc.file");
    }
    return file;
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
MSDevice_ToC::getLCAbstinence(const SUMOVehicle& v, const OptionsCont& oc) {
    return getFloatParam(v, oc, "toc.lcAbstinence", DEFAULT_LCABSTINENCE, false);
}

double
MSDevice_ToC::getInitialAwareness(const SUMOVehicle& v, const OptionsCont& oc) {
    return getFloatParam(v, oc, "toc.initialAwareness", DEFAULT_INITIAL_AWARENESS, false);
}

double
MSDevice_ToC::getMRMDecel(const SUMOVehicle& v, const OptionsCont& oc) {
    return getFloatParam(v, oc, "toc.mrmDecel", DEFAULT_MRM_DECEL, false);
}

double
MSDevice_ToC::getDynamicToCThreshold(const SUMOVehicle& v, const OptionsCont& oc) {
    return getFloatParam(v, oc, "toc.dynamicToCThreshold", DEFAULT_DYNAMIC_TOC_THRESHOLD, false);
}

bool
MSDevice_ToC::useColorScheme(const SUMOVehicle& v, const OptionsCont& oc) {
    return getBoolParam(v, oc, "toc.useColorScheme", "false", false);
}

MSDevice_ToC::OpenGapParams
MSDevice_ToC::getOpenGapParams(const SUMOVehicle& v, const OptionsCont& oc) {
    double timegap = getFloatParam(v, oc, "toc.ogNewTimeHeadway", -1.0, false);
    double spacing = getFloatParam(v, oc, "toc.ogNewSpaceHeadway", -1.0, false);
    double changeRate = getFloatParam(v, oc, "toc.ogChangeRate", -1.0, false);
    double maxDecel = getFloatParam(v, oc, "toc.ogMaxDecel", -1.0, false);

    bool specifiedAny = false;
    if (changeRate == -1.0) {
        changeRate = DEFAULT_OPENGAP_CHANGERATE;
    } else {
        specifiedAny = true;
    }
    if (maxDecel == -1.0) {
        maxDecel = DEFAULT_OPENGAP_MAXDECEL;
    } else {
        specifiedAny = true;
    }
    if (specifiedAny && timegap == -1 && spacing == -1) {
        WRITE_ERROR("If any openGap parameters for the ToC model are specified, then at least one of ogTimeGap and ogSpacing must be defined.")
    }
    if (timegap == -1) {
        timegap = DEFAULT_OPENGAP_TIMEGAP;
    } else {
        specifiedAny = true;
    }
    if (spacing == -1) {
        spacing = DEFAULT_OPENGAP_SPACING;
    } else {
        specifiedAny = true;
    }


#ifdef DEBUG_TOC
    std::cout << "Parsed openGapParams: \n"
              << "  timegap=" << timegap
              << ", spacing=" << spacing
              << ", changeRate=" << changeRate
              << ", maxDecel=" << maxDecel
              << std::endl;
#endif

    return OpenGapParams(timegap, spacing, changeRate, maxDecel, specifiedAny);
}

// ---------------------------------------------------------------------------
// MSDevice_ToC-methods
// ---------------------------------------------------------------------------
MSDevice_ToC::MSDevice_ToC(SUMOVehicle& holder, const std::string& id, const std::string& outputFilename,
                           std::string manualType, std::string automatedType, SUMOTime responseTime, double recoveryRate,
                           double lcAbstinence, double initialAwareness, double mrmDecel,
						   double dynamicToCThreshold, bool useColoring, OpenGapParams ogp) :
    MSVehicleDevice(holder, id),
    myManualTypeID(manualType),
    myAutomatedTypeID(automatedType),
    myResponseTime(responseTime),
    myRecoveryRate(recoveryRate),
    myLCAbstinence(lcAbstinence),
    myInitialAwareness(initialAwareness),
    myMRMDecel(mrmDecel),
    myCurrentAwareness(1.),
    myUseColorScheme(useColoring),
    myTriggerMRMCommand(nullptr),
    myTriggerToCCommand(nullptr),
    myRecoverAwarenessCommand(nullptr),
    myExecuteMRMCommand(nullptr),
    myPrepareToCCommand(nullptr),
    myOutputFile(nullptr),
    myEvents(),
    myPreviousLCMode(-1),
    myOpenGapParams(ogp),
    myDynamicToCThreshold(dynamicToCThreshold),
    myDynamicToCActive(dynamicToCThreshold > 0),
	myDynamicToCLane(-1) {
    // Take care! Holder is currently being constructed. Cast occurs before completion.
    myHolderMS = static_cast<MSVehicle*>(&holder);

    if (outputFilename != "") {
        myOutputFile = &OutputDevice::getDevice(outputFilename);
        // TODO: make xsd, include header
        // myOutputFile.writeXMLHeader("ToCDeviceLog", "ToCDeviceLog.xsd");
        if (createdOutputFiles.count(outputFilename) == 0) {
            myOutputFile->openTag("ToCDeviceLog");
            createdOutputFiles.insert(outputFilename);
        }
    }
    // register at static instance container
    instances.insert(this);

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
              << "outputFilename=" << outputFilename << ", "
              << "myManualType=" << myManualTypeID << ", "
              << "myAutomatedType=" << myAutomatedTypeID << ", "
              << "myResponseTime=" << myResponseTime << ", "
              << "myRecoveryRate=" << myRecoveryRate << ", "
              << "myInitialAwareness=" << myInitialAwareness << ", "
              << "myMRMDecel=" << myMRMDecel << ", "
              << "ogTimeHeadway=" << myOpenGapParams.newTimeHeadway << ", "
              << "ogSpaceHeadway=" << myOpenGapParams.newSpaceHeadway << ", "
              << "ogChangeRate=" << myOpenGapParams.changeRate << ", "
              << "ogMaxDecel=" << myOpenGapParams.maxDecel << ", "
              << "ogActive=" << myOpenGapParams.active << ", "
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


MSDevice_ToC::~MSDevice_ToC() {
    // unregister from static instance container
    instances.erase(this);
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
        resetDeliberateLCs();
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
    if (myCurrentAwareness >= myLCAbstinence && value < myLCAbstinence) {
        // Awareness is now below LC abstinence level -> prevent deliberate LCs
        deactivateDeliberateLCs();
    } else if (myCurrentAwareness < myLCAbstinence && value >= myLCAbstinence) {
        // Awareness is now above LC abstinence level -> allow deliberate LCs
        resetDeliberateLCs();
    }
    myCurrentAwareness = value;
    myHolderMS->getDriverState()->setAwareness(value);
}


void
MSDevice_ToC::setState(ToCState state) {
    if (myOpenGapParams.active && myState == PREPARING_TOC && state != PREPARING_TOC) {
        // Deactivate gap control at preparation phase end
        myHolderMS->getInfluencer().deactivateGapController();
    }

    if (myIssuedDynamicToC) {
    	// Reset dynamic ToC flag
    	// TODO: Reset response time if appropriate
    	myIssuedDynamicToC = false;
    }

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
        if (myOpenGapParams.active) {
            // Start gap controller
            double originalTau = myHolderMS->getCarFollowModel().getHeadwayTime();
            myHolderMS->getInfluencer().activateGapController(originalTau,
                    myOpenGapParams.newTimeHeadway, myOpenGapParams.newSpaceHeadway, -1,
                    myOpenGapParams.changeRate, myOpenGapParams.maxDecel);
        }
        // Record event
        if (generatesOutput()) {
            myEvents.push(std::make_pair(SIMSTEP, "TOR"));
        }
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
    deactivateDeliberateLCs();

    // Record event
    if (generatesOutput()) {
        myEvents.push(std::make_pair(SIMSTEP, "MRM"));
    }

    return 0;
}


SUMOTime
MSDevice_ToC::triggerUpwardToC(SUMOTime /* t */) {
#ifdef DEBUG_TOC
    std::cout << SIMTIME << " triggerUpwardToC() for vehicle '" << myHolder.getID() << "'" << std::endl;
#endif
    descheduleToC();
    // Eventually stop ToC preparation process
    descheduleToCPreparation();
    // Eventually abort MRM
    descheduleMRM();
    // Eventually abort awareness recovery process
    descheduleRecovery();

    switchHolderType(myAutomatedTypeID);
    setAwareness(1.);
    setState(AUTOMATED);

    // Record event
    if (generatesOutput()) {
        myEvents.push(std::make_pair(SIMSTEP, "ToCup"));
    }

    return 0;
}


SUMOTime
MSDevice_ToC::triggerDownwardToC(SUMOTime /* t */) {
#ifdef DEBUG_TOC
    std::cout << SIMTIME << " triggerDownwardToC() for vehicle '" << myHolder.getID() << "'" << std::endl;
#endif
    descheduleToC();
    // Eventually stop ToC preparation process
    descheduleToCPreparation();
    // Eventually abort MRM
    descheduleMRM();

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

    // Record event
    if (generatesOutput()) {
        myEvents.push(std::make_pair(SIMSTEP, "ToCdown"));
    }
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
        resetDeliberateLCs();
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
    if (targetType == nullptr) {
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
    // TODO: Prevent overtaking (lc to left). Gap increase to leader is controlled otherwise, now (see openGap)
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
    deactivateDeliberateLCs();
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

    if (myState == MRM) {
        return DELTA_T;
    } else {
#ifdef DEBUG_TOC
        std::cout << SIMTIME << " Aborting MRM for vehicle '" << myHolder.getID() << "'" << std::endl;
#endif
        resetDeliberateLCs();
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

bool
MSDevice_ToC::notifyMove(SUMOTrafficObject& /*veh*/,
                        double /*oldPos*/,
                        double /*newPos*/,
                        double /*newSpeed*/) {
	if (myState == AUTOMATED && checkDynamicToC()) {
		// Initiate a ToC
        // Record event
        if (generatesOutput()) {
            myEvents.push(std::make_pair(SIMSTEP, "DYNTOR"));
        }
		// Leadtime for dynamic ToC is proportional to the time assumed for the dynamic ToC threshold
        const double leadTime = myDynamicToCThreshold*1000*DYNAMIC_TOC_LEADTIME_FACTOR;
		requestToC((SUMOTime) leadTime);
		// TODO: Alter the response time according to the given lead time. Consider re-sampling it at each call of requestToC(). (Conditional to whether a non-negative response time was given in the configuration)
		myIssuedDynamicToC = true;
		myDynamicToCLane = myHolderMS->getLane()->getNumericalID();
	} else if (myIssuedDynamicToC && myState == PREPARING_TOC && !checkDynamicToC()) {
		// Abort dynamic ToC, FIXME: This could abort an externally requested ToC in rare occasions... (needs test)
        // Record event
        if (generatesOutput()) {
            myEvents.push(std::make_pair(SIMSTEP, "~DYNTOR"));
        }
		// NOTE: This should not occur if lane changing is prevented during ToC preparation...
		// TODO: Reset response time to the original value (unnecessary if re-sampling for each call to requestToC)
		triggerUpwardToC(0);
	}
	return true;
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
    } else if (key == "lcAbstinence") {
        return toString(myLCAbstinence);
    } else if (key == "state") {
        return _2string(myState);
    } else if (key == "holder") {
        return myHolder.getID();
    } else if (key == "hasDynamicToC") {
        return toString(myDynamicToCActive);
    } else if (key == "dynamicToCThreshold") {
        return toString(myDynamicToCThreshold);
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
        myResponseTime = TIME2STEPS(StringUtils::toDouble(value));
    } else if (key == "recoveryRate") {
        myRecoveryRate = StringUtils::toDouble(value);
    } else if (key == "initialAwareness") {
        myInitialAwareness = StringUtils::toDouble(value);
    } else if (key == "lcAbstinence") {
        myLCAbstinence = StringUtils::toDouble(value);
        if (isManuallyDriven()) {
            setAwareness(myCurrentAwareness); // to eventually trigger LC-prevention
        }
    } else if (key == "currentAwareness") {
        if (isManuallyDriven()) {
            setAwareness(StringUtils::toDouble(value));
        } else {
            WRITE_WARNING("Setting device.toc.currentAwareness during automated mode has no effect.")
        }
    } else if (key == "mrmDecel") {
        myMRMDecel = StringUtils::toDouble(value);
    } else if (key == "requestToC") {
        // setting this magic parameter gives the interface for inducing a ToC
        const SUMOTime timeTillMRM = TIME2STEPS(StringUtils::toDouble(value));
        requestToC(timeTillMRM);
    } else if (key == "requestMRM") {
        // setting this magic parameter gives the interface for inducing an MRM
        requestMRM();
    } else if (key == "awareness") {
        // setting this magic parameter gives the interface for setting the driverstate's awareness
        setAwareness(StringUtils::toDouble(value));
    } else if (key == "dynamicToCThreshold") {
    	const double newValue = StringUtils::toDouble(value);
    	if (newValue < 0) {
    		WRITE_WARNING("Value of dynamicToCThreshold must be non-negative. (Given value " + value + " for vehicle " + myHolderMS->getID() + " is ignored)");
    	} else if (newValue == 0) {
    		myDynamicToCThreshold = newValue;
    		myDynamicToCActive = false;
    	} else {
    		myDynamicToCThreshold = newValue;
    	}
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


void
MSDevice_ToC::writeOutput() {
    if (!generatesOutput()) {
        assert(myEvents.empty());
        return;
    }
    while (!myEvents.empty()) {
        std::pair<SUMOTime, std::string>& e = myEvents.front();
        myOutputFile->openTag(e.second);
        myOutputFile->writeAttr("id", myHolder.getID()).writeAttr("t", STEPS2TIME(e.first));
        myOutputFile->closeTag();
        myEvents.pop();
    }
}


void
MSDevice_ToC::cleanup() {
    // Close xml bodies for all existing files
    // TODO: Check if required
    for (auto& fn : createdOutputFiles) {
        OutputDevice* file = &OutputDevice::getDevice(fn);
        file->closeTag();
    }
}


void
MSDevice_ToC::resetDeliberateLCs() {
    if (myPreviousLCMode != -1) {
        myHolderMS->getInfluencer().setLaneChangeMode(myPreviousLCMode);
#ifdef DEBUG_TOC
        std::cout << "MSDevice_ToC::resetLCMode() restoring LC Mode of vehicle '" << myHolder.getID() << "' to " << myPreviousLCMode << std::endl;
#endif
    }
    myPreviousLCMode = -1;
}


void
MSDevice_ToC::deactivateDeliberateLCs() {
    const int lcModeHolder = myHolderMS->getInfluencer().getLaneChangeMode();
    if (lcModeHolder != LCModeMRM) {
        myPreviousLCMode = lcModeHolder;
#ifdef DEBUG_TOC
        std::cout << "MSDevice_ToC::setLCModeMRM() setting LC Mode of vehicle '" << myHolder.getID()
                  << "' from " << myPreviousLCMode << " to " << LCModeMRM << std::endl;
#endif
    }
    myHolderMS->getInfluencer().setLaneChangeMode(LCModeMRM);
}

bool
MSDevice_ToC::isManuallyDriven() {
    return (myState == MANUAL || myState == RECOVERING);
}

bool
MSDevice_ToC::isAutomated() {
    return (myState == AUTOMATED || myState == PREPARING_TOC || myState == MRM);
}

bool
MSDevice_ToC::checkDynamicToC() {
#ifdef DEBUG_DYNAMIC_TOC
	std::cout << SIMTIME << " # MSDevice_ToC::checkDynamicToC() for veh '" << myHolder.getID() << "'" << std::endl;
#endif
	if (!myDynamicToCActive) {
		return false;
	}
	// The vehicle's current lane
	const MSLane * currentLane = myHolderMS->getLane();

	if (currentLane->isInternal()) {
		// Don't start or abort dynamic ToCs on internal lanes
		return myIssuedDynamicToC;
	}

	if (myIssuedDynamicToC) {
#ifdef DEBUG_DYNAMIC_TOC
		std::cout << SIMTIME << " Dynamic ToC is ongoing." << std::endl;
#endif
		// Dynamic ToC in progress. Resist to aborting it if lane was not changed.
		if (myDynamicToCLane == currentLane->getNumericalID()){
			return true;
		}
	}
	// Length for which the current route can be followed
	const std::vector<MSVehicle::LaneQ>& bestLanes = myHolderMS->getBestLanes();
	// Maximal distance for route continuation without LCs over the possible start lanes
	double maximalContinuationDistance = 0;
	// Distance for route continuation without LCs from the vehicle's current lane
	double continuationDistanceOnCurrent = 0;
	// Lane of the next stop
	const MSLane * nextStopLane = nullptr;

	if (myHolderMS->hasStops()) {
		nextStopLane = myHolderMS->getNextStop().lane;
	}
	for (auto& i : bestLanes) {
		maximalContinuationDistance = MAX2(maximalContinuationDistance, i.length);
		if (currentLane == i.lane) {
			if (myHolderMS->hasStops()) {
				// Check if the next stop lies on the route continuation from the current lane
				for (MSLane* l : i.bestContinuations) {
					if (l == nextStopLane) {
#ifdef DEBUG_DYNAMIC_TOC
						std::cout << SIMTIME << " Stop found on the route continuation from the current lane. => No ToC" << std::endl;
#endif
						// Stop found on the route continuation from the current lane => no ToC necessary
						return false;
					}
				}
			}
			continuationDistanceOnCurrent = i.length;
		}
	}
	if (continuationDistanceOnCurrent == maximalContinuationDistance) {
		// There is no better lane than the current, hence no desire to change lanes,
		// which the driver could pursue better than the automation => no reason for ToC.
		return false;
	}
	const double distFromCurrent = continuationDistanceOnCurrent - myHolderMS->getPositionOnLane();
	double distThreshold = myHolderMS->getSpeed()*myDynamicToCThreshold;
#ifdef DEBUG_DYNAMIC_TOC
	std::cout << "  speed=" << myHolderMS->getSpeed()
					<< ", distFromCurrent=" << distFromCurrent
					<< ", maximal dist=" << maximalContinuationDistance - myHolderMS->getPositionOnLane()
			<< ", distThreshold=" << distThreshold
						<< std::endl;
#endif

	if (myIssuedDynamicToC) {
		// In case of an ongoing ToC, add an additional resistance to abort it.
		// (The lane-check above does not capture lanes subsequent to the dynamic ToC lane)
		distThreshold *= DYNAMIC_TOC_ABORT_RESISTANCE_FACTOR;
	}

	if (distFromCurrent < distThreshold) {
		// TODO: Make this more sophisticated in dealing with low speeds/stops and route ends
#ifdef DEBUG_DYNAMIC_TOC
		std::cout << SIMTIME << "  * distAlongBest is below threshold! *" << std::endl;
#endif
		return true;
	}

	return false;
}

/****************************************************************************/

