/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2023 German Aerospace Center (DLR) and others.
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
/// @file    MSStateHandler.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Thu, 13 Dec 2012
///
// Parser and output filter for routes and vehicles state saving and loading
/****************************************************************************/
#include <config.h>

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

#include <sstream>
#include <utils/common/StringUtils.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/xml/SUMOSAXReader.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/vehicle/SUMOVehicleParserHelper.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include <microsim/traffic_lights/MSRailSignalConstraint.h>
#include <microsim/traffic_lights/MSRailSignal.h>
#include <microsim/devices/MSDevice_Routing.h>
#include <microsim/devices/MSDevice_BTreceiver.h>
#include <microsim/devices/MSDevice_ToC.h>
#include <microsim/transportables/MSTransportableControl.h>
#include <microsim/traffic_lights/MSRailSignalControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSLink.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicleTransfer.h>
#include <microsim/MSInsertionControl.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSRoute.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSDriverState.h>
#include <netload/NLHandler.h>
#include "MSStateHandler.h"

#include <mesosim/MESegment.h>
#include <mesosim/MELoop.h>


// ===========================================================================
// MSStateTimeHandler method definitions
// ===========================================================================

SUMOTime
MSStateHandler::MSStateTimeHandler::getTime(const std::string& fileName) {
    // build handler and parser
    MSStateTimeHandler handler;
    handler.setFileName(fileName);
    handler.myTime = -1;
    SUMOSAXReader* parser = XMLSubSys::getSAXReader(handler);
    try {
        if (!parser->parseFirst(fileName)) {
            delete parser;
            throw ProcessError(TLF("Can not read XML-file '%'.", fileName));
        }
    } catch (ProcessError&) {
        delete parser;
        throw;
    }
    // parse
    while (parser->parseNext() && handler.myTime != -1);
    // clean up
    if (handler.myTime == -1) {
        delete parser;
        throw ProcessError(TLF("Could not parse time from state file '%'", fileName));
    }
    delete parser;
    return handler.myTime;
}

void
MSStateHandler::MSStateTimeHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    if (element == SUMO_TAG_SNAPSHOT) {
        myTime = string2time(attrs.getString(SUMO_ATTR_TIME));
    }
}

// ===========================================================================
// method definitions
// ===========================================================================
MSStateHandler::MSStateHandler(const std::string& file, const SUMOTime offset) :
    MSRouteHandler(file, true),
    myOffset(offset),
    mySegment(nullptr),
    myCurrentLane(nullptr),
    myCurrentLink(nullptr),
    myAttrs(nullptr),
    myVCAttrs(nullptr),
    myLastParameterised(nullptr),
    myRemoved(0),
    myConstrainedSignal(nullptr) {
    myAmLoadingState = true;
    const std::vector<std::string> vehIDs = OptionsCont::getOptions().getStringVector("load-state.remove-vehicles");
    myVehiclesToRemove.insert(vehIDs.begin(), vehIDs.end());
}


MSStateHandler::~MSStateHandler() {
    delete myVCAttrs;
}


void
MSStateHandler::saveState(const std::string& file, SUMOTime step, bool usePrefix) {
    OutputDevice& out = OutputDevice::getDevice(file, usePrefix);
    out.setPrecision(OptionsCont::getOptions().getInt("save-state.precision"));
    out.writeHeader<MSEdge>(SUMO_TAG_SNAPSHOT);
    out.writeAttr("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance").writeAttr("xsi:noNamespaceSchemaLocation", "http://sumo.dlr.de/xsd/state_file.xsd");
    out.writeAttr(SUMO_ATTR_VERSION, VERSION_STRING);
    out.writeAttr(SUMO_ATTR_TIME, time2string(step));
    out.writeAttr(SUMO_ATTR_TYPE, MSGlobals::gUseMesoSim ? "meso" : "micro");
    if (OptionsCont::getOptions().getBool("save-state.constraints")) {
        out.writeAttr(SUMO_ATTR_CONSTRAINTS, true);
    }
    if (OptionsCont::getOptions().getBool("save-state.rng")) {
        saveRNGs(out);
        if (!MSGlobals::gUseMesoSim) {
            MSNet::getInstance()->getEdgeControl().saveState(out);
        }
    }
    MSRoute::dict_saveState(out);
    MSNet::getInstance()->getVehicleControl().saveState(out);
    MSNet::getInstance()->getInsertionControl().saveState(out);
    if (OptionsCont::getOptions().getBool("save-state.transportables")) {
        if (MSNet::getInstance()->hasPersons()) {
            out.openTag(SUMO_TAG_TRANSPORTABLES).writeAttr(SUMO_ATTR_TYPE, "person");
            MSNet::getInstance()->getPersonControl().saveState(out);
            out.closeTag();
        }
        if (MSNet::getInstance()->hasContainers()) {
            out.openTag(SUMO_TAG_TRANSPORTABLES).writeAttr(SUMO_ATTR_TYPE, "container");
            MSNet::getInstance()->getContainerControl().saveState(out);
            out.closeTag();
        }
    }
    MSVehicleTransfer::getInstance()->saveState(out);
    for (MSEdge* const edge : MSEdge::getAllEdges()) {
        if (MSGlobals::gUseMesoSim) {
            for (MESegment* s = MSGlobals::gMesoNet->getSegmentForEdge(*edge); s != nullptr; s = s->getNextSegment()) {
                s->saveState(out);
            }
        } else {
            for (MSLane* const lane : edge->getLanes()) {
                lane->saveState(out);
            }
        }
    }
    MSNet::getInstance()->getTLSControl().saveState(out);
    out.close();
}


void
MSStateHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    MSRouteHandler::myStartElement(element, attrs);
    MSVehicleControl& vc = MSNet::getInstance()->getVehicleControl();
    switch (element) {
        case SUMO_TAG_SNAPSHOT: {
            myTime = string2time(attrs.getString(SUMO_ATTR_TIME));
            const std::string& version = attrs.getString(SUMO_ATTR_VERSION);
            if (version != VERSION_STRING) {
                WRITE_WARNINGF(TL("State was written with sumo version % (present: %)!"), version, VERSION_STRING);
            }
            bool ok;
            if (attrs.getOpt<bool>(SUMO_ATTR_CONSTRAINTS, nullptr, ok, false)) {
                MSRailSignalConstraint::clearAll();
            }
            break;
        }
        case SUMO_TAG_RNGSTATE: {
            if (attrs.hasAttribute(SUMO_ATTR_DEFAULT)) {
                RandHelper::loadState(attrs.getString(SUMO_ATTR_DEFAULT));
            }
            if (attrs.hasAttribute(SUMO_ATTR_RNG_ROUTEHANDLER)) {
                RandHelper::loadState(attrs.getString(SUMO_ATTR_RNG_ROUTEHANDLER), MSRouteHandler::getParsingRNG());
            }
            if (attrs.hasAttribute(SUMO_ATTR_RNG_INSERTIONCONTROL)) {
                RandHelper::loadState(attrs.getString(SUMO_ATTR_RNG_INSERTIONCONTROL), MSNet::getInstance()->getInsertionControl().getFlowRNG());
            }
            if (attrs.hasAttribute(SUMO_ATTR_RNG_DEVICE)) {
                RandHelper::loadState(attrs.getString(SUMO_ATTR_RNG_DEVICE), MSDevice::getEquipmentRNG());
            }
            if (attrs.hasAttribute(SUMO_ATTR_RNG_DEVICE_BT)) {
                RandHelper::loadState(attrs.getString(SUMO_ATTR_RNG_DEVICE_BT), MSVehicleDevice_BTreceiver::getEquipmentRNG());
            }
            if (attrs.hasAttribute(SUMO_ATTR_RNG_DRIVERSTATE)) {
                RandHelper::loadState(attrs.getString(SUMO_ATTR_RNG_DRIVERSTATE), OUProcess::getRNG());
            }
            if (attrs.hasAttribute(SUMO_ATTR_RNG_DEVICE_TOC)) {
                RandHelper::loadState(attrs.getString(SUMO_ATTR_RNG_DEVICE_TOC), MSDevice_ToC::getResponseTimeRNG());
            }
            break;
        }
        case SUMO_TAG_RNGLANE: {
            const int index = attrs.getInt(SUMO_ATTR_INDEX);
            const std::string state = attrs.getString(SUMO_ATTR_STATE);
            MSLane::loadRNGState(index, state);
            break;
        }
        case SUMO_TAG_EDGECONTROL: {
            bool ok;
            std::list<MSLane*> activeLanes;
            const std::vector<std::string>& laneIDs = attrs.get<std::vector<std::string> >(SUMO_ATTR_LANES, nullptr, ok, false);
            for (const std::string& laneID : laneIDs) {
                MSLane* lane = MSLane::dictionary(laneID);
                if (lane == nullptr) {
                    throw ProcessError(TLF("Unknown lane '%' in loaded state.", laneID));
                }
                activeLanes.push_back(lane);
            }
            MSNet::getInstance()->getEdgeControl().setActiveLanes(activeLanes);
            break;
        }
        case SUMO_TAG_DELAY: {
            if (myVCAttrs != nullptr) {
                delete myVCAttrs;
            }
            myVCAttrs = attrs.clone();
            break;
        }
        case SUMO_TAG_FLOWSTATE: {
            bool ok;
            SUMOVehicleParameter* pars = SUMOVehicleParserHelper::parseFlowAttributes(SUMO_TAG_FLOWSTATE, attrs, true, true, -1, -1);
            pars->repetitionsDone = attrs.get<int>(SUMO_ATTR_DONE, pars->id.c_str(), ok);
            pars->repetitionTotalOffset = attrs.getOptSUMOTimeReporting(SUMO_ATTR_NEXT, pars->id.c_str(), ok, 0);
            int index = attrs.getInt(SUMO_ATTR_INDEX);
            MSNet::getInstance()->getInsertionControl().addFlow(pars, index);
            break;
        }
        case SUMO_TAG_VTYPE: {
            myLastParameterised = myCurrentVType;
            break;
        }
        case SUMO_TAG_VEHICLE: {
            myLastParameterised = myVehicleParameter;
            myAttrs = attrs.clone();
            break;
        }
        case SUMO_TAG_DEVICE: {
            myDeviceAttrs.push_back(attrs.clone());
            break;
        }
        case SUMO_TAG_VEHICLETRANSFER: {
            MSVehicleTransfer::getInstance()->loadState(attrs, myOffset, vc);
            break;
        }
        case SUMO_TAG_SEGMENT: {
            const std::string& segmentID = attrs.getString(SUMO_ATTR_ID);
            const MSEdge* const edge = MSEdge::dictionary(segmentID.substr(0, segmentID.rfind(":")));
            int idx = StringUtils::toInt(segmentID.substr(segmentID.rfind(":") + 1));
            mySegment = MSGlobals::gMesoNet->getSegmentForEdge(*edge);
            while (idx-- > 0) {
                mySegment = mySegment->getNextSegment();
            }
            myQueIndex = 0;
            break;
        }
        case SUMO_TAG_LANE: {
            bool ok;
            const std::string laneID = attrs.get<std::string>(SUMO_ATTR_ID, nullptr, ok);
            myCurrentLane = MSLane::dictionary(laneID);
            if (myCurrentLane == nullptr) {
                throw ProcessError(TLF("Unknown lane '%' in loaded state.", laneID));
            }
            break;
        }
        case SUMO_TAG_VIEWSETTINGS_VEHICLES: {
            bool ok;
            const std::vector<std::string>& vehIDs = attrs.get<std::vector<std::string> >(SUMO_ATTR_VALUE, nullptr, ok, false);
            if (MSGlobals::gUseMesoSim) {
                mySegment->loadState(vehIDs, MSNet::getInstance()->getVehicleControl(), StringUtils::toLong(attrs.getString(SUMO_ATTR_TIME)) - myOffset, myQueIndex);
            } else {
                myCurrentLane->loadState(vehIDs, MSNet::getInstance()->getVehicleControl());
            }
            myQueIndex++;
            break;
        }
        case SUMO_TAG_LINK: {
            bool ok;
            myCurrentLink = nullptr;
            const std::string toLaneID = attrs.get<std::string>(SUMO_ATTR_TO, nullptr, ok);
            for (MSLink* link : myCurrentLane->getLinkCont()) {
                if (link->getViaLaneOrLane()->getID() == toLaneID) {
                    myCurrentLink = link;
                }
            }
            if (myCurrentLink == nullptr) {
                throw ProcessError("Unknown link from lane '" + myCurrentLane->getID() + "' to lane '" + toLaneID + "' in loaded state");
            }
            break;
        }
        case SUMO_TAG_APPROACHING: {
            bool ok;
            const std::string vehID = attrs.get<std::string>(SUMO_ATTR_ID, nullptr, ok);
            const SUMOTime arrivalTime = attrs.get<SUMOTime>(SUMO_ATTR_ARRIVALTIME, nullptr, ok);
            const double arrivalSpeed = attrs.get<double>(SUMO_ATTR_ARRIVALSPEED, nullptr, ok);
            const double leaveSpeed = attrs.get<double>(SUMO_ATTR_DEPARTSPEED, nullptr, ok);
            const bool setRequest = attrs.get<bool>(SUMO_ATTR_REQUEST, nullptr, ok);
            const double arrivalSpeedBraking = attrs.get<double>(SUMO_ATTR_ARRIVALSPEEDBRAKING, nullptr, ok);
            const SUMOTime waitingTime = attrs.get<SUMOTime>(SUMO_ATTR_WAITINGTIME, nullptr, ok);
            const double dist = attrs.get<double>(SUMO_ATTR_DISTANCE, nullptr, ok);
            const double latOffset = attrs.getOpt<double>(SUMO_ATTR_POSITION_LAT, nullptr, ok, 0);
            SUMOVehicle* veh = vc.getVehicle(vehID);
            myCurrentLink->setApproaching(veh, arrivalTime, arrivalSpeed, leaveSpeed, setRequest, arrivalSpeedBraking, waitingTime, dist, latOffset);
            if (!MSGlobals::gUseMesoSim) {
                MSVehicle* microVeh = dynamic_cast<MSVehicle*>(veh);
                microVeh->loadPreviousApproaching(myCurrentLink, setRequest, arrivalTime, arrivalSpeed, arrivalSpeedBraking, dist, leaveSpeed);
            }
            break;
        }
        case SUMO_TAG_RAILSIGNAL_CONSTRAINT_TRACKER: {
            MSRailSignalConstraint_Predecessor::loadState(attrs);
            break;
        }
        case SUMO_TAG_PARAM: {
            bool ok;
            const std::string key = attrs.get<std::string>(SUMO_ATTR_KEY, nullptr, ok);
            // circumventing empty string test
            const std::string val = attrs.hasAttribute(SUMO_ATTR_VALUE) ? attrs.getString(SUMO_ATTR_VALUE) : "";
            assert(myLastParameterised != 0);
            if (myLastParameterised != nullptr) {
                myLastParameterised->setParameter(key, val);
            }
            break;
        }
        case SUMO_TAG_TRANSPORTABLES:
            if (attrs.getString(SUMO_ATTR_TYPE) == "person") {
                MSNet::getInstance()->getPersonControl().loadState(attrs.getString(SUMO_ATTR_STATE));
            }
            if (attrs.getString(SUMO_ATTR_TYPE) == "container") {
                MSNet::getInstance()->getContainerControl().loadState(attrs.getString(SUMO_ATTR_STATE));
            }
            break;
        case SUMO_TAG_PERSON:
        case SUMO_TAG_CONTAINER:
            myAttrs = attrs.clone();
            break;
        case SUMO_TAG_RAILSIGNAL_CONSTRAINTS: {
            bool ok = true;
            const std::string signalID = attrs.get<std::string>(SUMO_ATTR_ID, nullptr, ok);
            if (!MSNet::getInstance()->getTLSControl().knows(signalID)) {
                throw InvalidArgument("Rail signal '" + signalID + "' in railSignalConstraints loaded from state is not known");
            }
            myConstrainedSignal = dynamic_cast<MSRailSignal*>(MSNet::getInstance()->getTLSControl().get(signalID).getDefault());
            if (myConstrainedSignal == nullptr) {
                throw InvalidArgument("Traffic light '" + signalID + "' is not a rail signal");
            }
            break;
        }
        case SUMO_TAG_PREDECESSOR: // intended fall-through
        case SUMO_TAG_INSERTION_PREDECESSOR: // intended fall-through
        case SUMO_TAG_FOE_INSERTION: // intended fall-through
        case SUMO_TAG_INSERTION_ORDER: // intended fall-through
        case SUMO_TAG_BIDI_PREDECESSOR:
            myLastParameterised = NLHandler::addPredecessorConstraint(element, attrs, myConstrainedSignal);
            break;
        case SUMO_TAG_TLLOGIC: {
            bool ok;
            const std::string tlID = attrs.get<std::string>(SUMO_ATTR_ID, nullptr, ok);
            const std::string programID = attrs.get<std::string>(SUMO_ATTR_PROGRAMID, tlID.c_str(), ok);
            const int phase = attrs.get<int>(SUMO_ATTR_PHASE, tlID.c_str(), ok);
            const SUMOTime spentDuration = attrs.get<SUMOTime>(SUMO_ATTR_DURATION, tlID.c_str(), ok);
            MSTLLogicControl& tlc = MSNet::getInstance()->getTLSControl();
            MSTrafficLightLogic* tl = tlc.get(tlID, programID);
            if (tl == nullptr) {
                if (programID == "online") {
                    WRITE_WARNINGF(TL("Ignoring program '%' for traffic light '%' in loaded state"), programID, tlID);
                    return;
                } else {
                    throw ProcessError("Unknown program '" + programID + "' for traffic light '" + tlID + "'");
                }
            }
            if (phase >= tl->getPhaseNumber()) {
                throw ProcessError("Invalid phase '" + toString(phase) + "' for traffic light '" + tlID + "'");
            }
            // might not be set if the phase happens to match and there are multiple programs
            tl->loadState(tlc, myTime, phase, spentDuration);
            break;
        }
        default:
            break;
    }
}


void
MSStateHandler::myEndElement(int element) {
    MSRouteHandler::myEndElement(element);
    switch (element) {
        case SUMO_TAG_PERSON:
        case SUMO_TAG_CONTAINER: {
            MSTransportableControl& tc = (element == SUMO_TAG_PERSON ? MSNet::getInstance()->getPersonControl() : MSNet::getInstance()->getContainerControl());
            MSTransportable* transportable = tc.get(myAttrs->getString(SUMO_ATTR_ID));
            transportable->loadState(myAttrs->getString(SUMO_ATTR_STATE));
            tc.fixLoadCount(transportable);
            delete myAttrs;
            myAttrs = nullptr;
            break;
        }
        case SUMO_TAG_SNAPSHOT: {
            if (myVCAttrs == nullptr) {
                throw ProcessError(TL("Could not load vehicle control state"));
            }
            MSVehicleControl& vc = MSNet::getInstance()->getVehicleControl();
            vc.setState(myVCAttrs->getInt(SUMO_ATTR_NUMBER),
                        myVCAttrs->getInt(SUMO_ATTR_BEGIN),
                        myVCAttrs->getInt(SUMO_ATTR_END),
                        myVCAttrs->getFloat(SUMO_ATTR_DEPART),
                        myVCAttrs->getFloat(SUMO_ATTR_TIME));
            if (myRemoved > 0) {
                WRITE_MESSAGEF(TL("Removed % vehicles while loading state."), toString(myRemoved));
                vc.discountStateRemoved(myRemoved);
            }
            break;
        }
        default:
            break;
    }
    if (element != SUMO_TAG_PARAM && myVehicleParameter == nullptr && myCurrentVType == nullptr) {
        myLastParameterised = nullptr;
    }
}


void
MSStateHandler::closeVehicle() {
    assert(myVehicleParameter != nullptr);
    myVehicleParameter->depart -= myOffset;
    // the vehicle was already counted in MSVehicleControl::setState
    MSVehicleControl& vc = MSNet::getInstance()->getVehicleControl();
    // make a copy because myVehicleParameter is reset in closeVehicle()
    const std::string vehID = myVehicleParameter->id;
    if (myVehiclesToRemove.count(vehID) == 0) {
        MSRouteHandler::closeVehicle();
        SUMOVehicle* v = vc.getVehicle(vehID);
        if (v == nullptr) {
            throw ProcessError(TLF("Could not load vehicle '%' from state", vehID));
        }
        v->setChosenSpeedFactor(myAttrs->getFloat(SUMO_ATTR_SPEEDFACTOR));
        v->loadState(*myAttrs, myOffset);
        if (v->hasDeparted()) {
            // vehicle already departed: disable pre-insertion rerouting and enable regular routing behavior
            MSDevice_Routing* routingDevice = static_cast<MSDevice_Routing*>(v->getDevice(typeid(MSDevice_Routing)));
            if (routingDevice != nullptr) {
                routingDevice->notifyEnter(*v, MSMoveReminder::NOTIFICATION_DEPARTED);
            }
            MSNet::getInstance()->getInsertionControl().alreadyDeparted(v);
            if (MSRailSignalControl::hasInstance()) {
                // register route for deadlock prevention (vehicleStateChanged would not be called otherwise)
                MSRailSignalControl::getInstance().vehicleStateChanged(v, MSNet::VehicleState::NEWROUTE, "loadState");
            }
        }
        while (!myDeviceAttrs.empty()) {
            const std::string attrID = myDeviceAttrs.back()->getString(SUMO_ATTR_ID);
            for (MSVehicleDevice* const dev : v->getDevices()) {
                if (dev->getID() == attrID) {
                    dev->loadState(*myDeviceAttrs.back());
                }
            }
            delete myDeviceAttrs.back();
            myDeviceAttrs.pop_back();
        }
    } else {
        delete myVehicleParameter;
        myVehicleParameter = nullptr;
        myRemoved++;
    }
    delete myAttrs;
}


void
MSStateHandler::saveRNGs(OutputDevice& out) {
    out.openTag(SUMO_TAG_RNGSTATE);
    out.writeAttr(SUMO_ATTR_DEFAULT, RandHelper::saveState());
    out.writeAttr(SUMO_ATTR_RNG_ROUTEHANDLER, RandHelper::saveState(MSRouteHandler::getParsingRNG()));
    out.writeAttr(SUMO_ATTR_RNG_INSERTIONCONTROL, RandHelper::saveState(MSNet::getInstance()->getInsertionControl().getFlowRNG()));
    out.writeAttr(SUMO_ATTR_RNG_DEVICE, RandHelper::saveState(MSDevice::getEquipmentRNG()));
    out.writeAttr(SUMO_ATTR_RNG_DEVICE_BT, RandHelper::saveState(MSDevice_BTreceiver::getRNG()));
    out.writeAttr(SUMO_ATTR_RNG_DRIVERSTATE, RandHelper::saveState(OUProcess::getRNG()));
    out.writeAttr(SUMO_ATTR_RNG_DEVICE_TOC, RandHelper::saveState(MSDevice_ToC::getResponseTimeRNG()));
    MSLane::saveRNGStates(out);
    out.closeTag();

}


/****************************************************************************/
