/****************************************************************************/
/// @file    NLHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// The XML-Handler for network loading
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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

#include <string>
#include "NLHandler.h"
#include "NLEdgeControlBuilder.h"
#include "NLJunctionControlBuilder.h"
#include "NLDetectorBuilder.h"
#include "NLTriggerBuilder.h"
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/TplConvert.h>
#include <utils/common/TplConvertSec.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/RGBColor.h>
#include <utils/geom/GeomConvHelper.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSLane.h>
#include <microsim/MSBitSetLogic.h>
#include <microsim/MSJunctionLogic.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/output/MSInductLoop.h>
#include <microsim/output/MSE2Collector.h>
#include <microsim/output/MS_E2_ZS_CollectorOverLanes.h>
#include <microsim/traffic_lights/MSAgentbasedTrafficLightLogic.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/UtilExceptions.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/shapes/ShapeContainer.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NLHandler::NLHandler(const std::string &file, MSNet &net,
                     NLDetectorBuilder &detBuilder,
                     NLTriggerBuilder &triggerBuilder,
                     NLEdgeControlBuilder &edgeBuilder,
                     NLJunctionControlBuilder &junctionBuilder) throw()
        : MSRouteHandler(file, true),
        myNet(net), myActionBuilder(net),
        myCurrentIsInternalToSkip(false),
        myDetectorBuilder(detBuilder), myTriggerBuilder(triggerBuilder),
        myEdgeControlBuilder(edgeBuilder), myJunctionControlBuilder(junctionBuilder),
        mySucceedingLaneBuilder(junctionBuilder),
        myAmInTLLogicMode(false), myCurrentIsBroken(false),
        myHaveWarnedAboutDeprecatedVClass(false),
        myHaveWarnedAboutDeprecatedPhases(false) {}


NLHandler::~NLHandler() throw() {}


void
NLHandler::myStartElement(SumoXMLTag element,
                          const SUMOSAXAttributes &attrs) throw(ProcessError) {
    try {
        switch (element) {
        case SUMO_TAG_EDGE:
            beginEdgeParsing(attrs);
            break;
        case SUMO_TAG_LANE:
            addLane(attrs);
            break;
        case SUMO_TAG_POLY:
            addPoly(attrs);
            break;
        case SUMO_TAG_POI:
            addPOI(attrs);
            break;
        case SUMO_TAG_JUNCTION:
            openJunction(attrs);
            break;
        case SUMO_TAG_PHASE:
            addPhase(attrs);
            break;
        case SUMO_TAG_SUCC:
            openSucc(attrs);
            break;
        case SUMO_TAG_SUCCLANE:
            addSuccLane(attrs);
            break;
        case SUMO_TAG_ROWLOGIC:
            initJunctionLogic(attrs);
            break;
        case SUMO_TAG_TLLOGIC:
            initTrafficLightLogic(attrs);
            break;
        case SUMO_TAG_LOGICITEM:
            addLogicItem(attrs);
            break;
        case SUMO_TAG_WAUT:
            openWAUT(attrs);
            break;
        case SUMO_TAG_WAUT_SWITCH:
            addWAUTSwitch(attrs);
            break;
        case SUMO_TAG_WAUT_JUNCTION:
            addWAUTJunction(attrs);
            break;
            /// @deprecated begins
        case SUMO_TAG_DETECTOR:
            addDetector(attrs);
            break;
            /// @deprecated ends
#ifdef _MESSAGES
        case SUMO_TAG_MSG_EMITTER:
            addMsgEmitter(attrs);
            break;
        case SUMO_TAG_MSG:
            addMsgDetector(attrs);
            break;
#endif
        case SUMO_TAG_E1DETECTOR:
            addE1Detector(attrs);
            break;
        case SUMO_TAG_E2DETECTOR:
            addE2Detector(attrs);
            break;
        case SUMO_TAG_E3DETECTOR:
            beginE3Detector(attrs);
            break;
        case SUMO_TAG_DET_ENTRY:
            addE3Entry(attrs);
            break;
        case SUMO_TAG_DET_EXIT:
            addE3Exit(attrs);
            break;
        case SUMO_TAG_VSS:
            myTriggerBuilder.parseAndBuildLaneSpeedTrigger(myNet, attrs, getFileName());
            break;
        case SUMO_TAG_CALIBRATOR:
            myTriggerBuilder.parseAndBuildCalibrator(myNet, attrs, getFileName());
            break;
        case SUMO_TAG_REROUTER:
            myTriggerBuilder.parseAndBuildRerouter(myNet, attrs, getFileName());
            break;
        case SUMO_TAG_BUS_STOP:
            myTriggerBuilder.parseAndBuildBusStop(myNet, attrs);
            break;
        case SUMO_TAG_VTYPEPROBE:
            addVTypeProbeDetector(attrs);
            break;
        case SUMO_TAG_ROUTEPROBE:
            addRouteProbeDetector(attrs);
            break;
        case SUMO_TAG_MEANDATA_EDGE:
            addEdgeLaneMeanData(attrs, "meandata_edge");
            break;
        case SUMO_TAG_MEANDATA_LANE:
            addEdgeLaneMeanData(attrs, "meandata_lane");
            break;
        case SUMO_TAG_SOURCE:
            addSource(attrs);
            break;
        case SUMO_TAG_TRIGGER:
            addTrigger(attrs);
            break;
        case SUMO_TAG_TIMEDEVENT:
            myActionBuilder.addAction(attrs, getFileName());
            break;
        case SUMO_TAG_VAPORIZER:
            myTriggerBuilder.buildVaporizer(attrs);
            break;
        case SUMO_TAG_LOCATION:
            setLocation(attrs);
            break;
        case SUMO_TAG_DISTRICT:
            addDistrict(attrs);
            break;
        case SUMO_TAG_DSOURCE:
            addDistrictEdge(attrs, true);
            break;
        case SUMO_TAG_DSINK:
            addDistrictEdge(attrs, false);
            break;
        default:
            break;
        }
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    }
    MSRouteHandler::myStartElement(element, attrs);
    if (element==SUMO_TAG_PARAM) {
        addParam(attrs);
    }
}


void
NLHandler::myCharacters(SumoXMLTag element,
                        const std::string &chars) throw(ProcessError) {
    UNUSED_PARAMETER(element);
    UNUSED_PARAMETER(chars);
}


void
NLHandler::myEndElement(SumoXMLTag element) throw(ProcessError) {
    switch (element) {
    case SUMO_TAG_EDGE:
        closeEdge();
        break;
    case SUMO_TAG_JUNCTION:
        closeJunction();
        break;
    case SUMO_TAG_SUCC:
        closeSuccLane();
        break;
    case SUMO_TAG_ROWLOGIC:
        try {
            myJunctionControlBuilder.closeJunctionLogic();
        } catch (InvalidArgument &e) {
            MsgHandler::getErrorInstance()->inform(e.what());
        }
        break;
    case SUMO_TAG_TLLOGIC:
        try {
            myJunctionControlBuilder.closeTrafficLightLogic();
        } catch (InvalidArgument &e) {
            MsgHandler::getErrorInstance()->inform(e.what());
        }
        myAmInTLLogicMode = false;
        break;
    case SUMO_TAG_WAUT:
        closeWAUT();
        break;
    case SUMO_TAG_E3DETECTOR:
        endE3Detector();
        break;
    case SUMO_TAG_DETECTOR:
        endDetector();
        break;
    default:
        break;
    }
    MSRouteHandler::myEndElement(element);
}



// ---- the root/edge - element
void
NLHandler::beginEdgeParsing(const SUMOSAXAttributes &attrs) {
    myCurrentIsBroken = false;
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes(id)) {
        myCurrentIsBroken = true;
        return;
    }
    // omit internal edges if not wished
    if (!MSGlobals::gUsingInternalLanes&&id[0]==':') {
        myCurrentIsInternalToSkip = true;
        return;
    }
    myCurrentIsInternalToSkip = false;
    // get the function
    bool ok = true;
    std::string func = attrs.getStringReporting(SUMO_ATTR_FUNCTION, id.c_str(), ok);
    if (!ok) {
        myCurrentIsBroken = true;
        return;
    }
    // parse the function
    MSEdge::EdgeBasicFunction funcEnum = MSEdge::EDGEFUNCTION_UNKNOWN;
    if (func=="normal") {
        funcEnum = MSEdge::EDGEFUNCTION_NORMAL;
    } else if (func=="connector"||func=="sink"||func=="source") {
        funcEnum = MSEdge::EDGEFUNCTION_CONNECTOR;
    } else if (func=="internal") {
        funcEnum = MSEdge::EDGEFUNCTION_INTERNAL;
    }
    if (funcEnum<0) {
        MsgHandler::getErrorInstance()->inform("Edge '" + id + "' has an invalid type ('" + func + "').");
        myCurrentIsBroken = true;
        return;
    }
    //
    try {
        myEdgeControlBuilder.beginEdgeParsing(id, funcEnum);
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
        myCurrentIsBroken = true;
    }
}


void
NLHandler::closeEdge() {
    // omit internal edges if not wished and broken edges
    if (myCurrentIsInternalToSkip||myCurrentIsBroken) {
        return;
    }
    try {
        MSEdge& edge = *myEdgeControlBuilder.closeEdge();
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    }
}


//             ---- the root/edge/lanes/lane - element
void
NLHandler::addLane(const SUMOSAXAttributes &attrs) {
    // omit internal edges if not wished and broken edges
    if (myCurrentIsInternalToSkip||myCurrentIsBroken) {
        return;
    }
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes(id)) {
        myCurrentIsBroken = true;
        return;
    }
    bool ok = true;
    bool laneIsDepart = attrs.getBoolReporting(SUMO_ATTR_DEPART, id.c_str(), ok);
    SUMOReal maxSpeed = attrs.getSUMORealReporting(SUMO_ATTR_MAXSPEED, id.c_str(), ok);
    SUMOReal length = attrs.getSUMORealReporting(SUMO_ATTR_LENGTH, id.c_str(), ok);
    std::string allow = attrs.getOptStringReporting(SUMO_ATTR_ALLOW, id.c_str(), ok, "");
    std::string disallow = attrs.getOptStringReporting(SUMO_ATTR_DISALLOW, id.c_str(), ok, "");
    std::string vclasses = attrs.getOptStringReporting(SUMO_ATTR_VCLASSES, id.c_str(), ok, "");
    Position2DVector shape = GeomConvHelper::parseShapeReporting(attrs.getStringReporting(SUMO_ATTR_SHAPE, id.c_str(), ok), "lane", id.c_str(), ok, false);
    if (shape.size()<2) {
        MsgHandler::getErrorInstance()->inform("Shape of lane '" + id + "' is broken.\n Can not build according edge.");
        myCurrentIsBroken = true;
        return;
    }
    std::vector<SUMOVehicleClass> allowedClasses;
    std::vector<SUMOVehicleClass> disallowedClasses;
    parseVehicleClasses(vclasses, allow, disallow, allowedClasses, disallowedClasses, myHaveWarnedAboutDeprecatedVClass);
    myCurrentIsBroken |= !ok;
    if(!myCurrentIsBroken) {
        try {
            MSLane *lane = myEdgeControlBuilder.addLane(id, maxSpeed, length, laneIsDepart, shape, allowedClasses, disallowedClasses);
            // insert the lane into the lane-dictionary, checking
            if (!MSLane::dictionary(id, lane)) {
                delete lane;
                MsgHandler::getErrorInstance()->inform("Another lane with the id '" + id + "' exists.");
                myCurrentIsBroken = true;
            }
        } catch (InvalidArgument &e) {
            MsgHandler::getErrorInstance()->inform(e.what());
        }
    }
}


// ---- the root/junction - element
void
NLHandler::openJunction(const SUMOSAXAttributes &attrs) {
    myCurrentIsBroken = false;
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes(id)) {
        myCurrentIsBroken = true;
        return;
    }
    bool ok = true;
    Position2DVector shape;
    if (attrs.hasAttribute(SUMO_ATTR_SHAPE)) {
        // @deprecated: at some time, all junctions should have a shape attribute (moved from characters)
        shape = GeomConvHelper::parseShapeReporting(attrs.getStringSecure(SUMO_ATTR_SHAPE, ""), attrs.getObjectType(), id.c_str(), ok, true);
    }
    SUMOReal x = attrs.getSUMORealReporting(SUMO_ATTR_X, id.c_str(), ok);
    SUMOReal y = attrs.getSUMORealReporting(SUMO_ATTR_Y, id.c_str(), ok);
    std::string type = attrs.getStringReporting(SUMO_ATTR_TYPE, id.c_str(), ok);
    std::string key = attrs.getOptStringReporting(SUMO_ATTR_KEY, id.c_str(), ok, "");
    if (!ok) {
        myCurrentIsBroken = true;
    } else {
        try {
            myJunctionControlBuilder.openJunction(id, key, type, x, y, shape);
        } catch (InvalidArgument &e) {
            MsgHandler::getErrorInstance()->inform(e.what() + std::string("\n Can not build according junction."));
            myCurrentIsBroken = true;
        }
        //
        if (!myCurrentIsBroken&&attrs.hasAttribute(SUMO_ATTR_INCLANES)) {
            addIncomingLanes(attrs.getStringSecure(SUMO_ATTR_INCLANES, ""));
        }
#ifdef HAVE_INTERNAL_LANES
        if (!myCurrentIsBroken&&attrs.hasAttribute(SUMO_ATTR_INTLANES)) {
            addInternalLanes(attrs.getStringSecure(SUMO_ATTR_INTLANES, ""));
        }
#endif
    }
}


void
NLHandler::closeJunction() {
    if (myCurrentIsBroken) {
        return;
    }
    try {
        myJunctionControlBuilder.closeJunction();
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    }
}
// ----

void
NLHandler::addParam(const SUMOSAXAttributes &attrs) {
    bool ok = true;
    std::string key = attrs.getStringReporting(SUMO_ATTR_KEY, 0, ok);
    std::string val = attrs.getStringReporting(SUMO_ATTR_VALUE, 0, ok);
    // set
    if (ok&&myAmInTLLogicMode) {
        assert(key!="");
        assert(val!="");
        myJunctionControlBuilder.addParam(key, val);
    }
}


void
NLHandler::openWAUT(const SUMOSAXAttributes &attrs) {
    myCurrentIsBroken = false;

    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes(id)) {
        myCurrentIsBroken = true;
        return;
    }
    bool ok = true;
    SUMOTime t = attrs.getOptSUMOTimeReporting(SUMO_ATTR_REF_TIME, id.c_str(), ok, 0);
    std::string pro = attrs.getStringReporting(SUMO_ATTR_START_PROG, id.c_str(), ok);
    if (!ok) {
        myCurrentIsBroken = true;
    }
    if (!myCurrentIsBroken) {
        myCurrentWAUTID = id;
        try {
            myJunctionControlBuilder.getTLLogicControlToUse().addWAUT(t, id, pro);
        } catch (InvalidArgument &e) {
            MsgHandler::getErrorInstance()->inform(e.what());
            myCurrentIsBroken = true;
        }
    }
}


void
NLHandler::addWAUTSwitch(const SUMOSAXAttributes &attrs) {
    bool ok = true;
    SUMOTime t = attrs.getSUMOTimeReporting(SUMO_ATTR_TIME, myCurrentWAUTID.c_str(), ok);
    std::string to = attrs.getStringReporting(SUMO_ATTR_TO, myCurrentWAUTID.c_str(), ok);
    if (!ok) {
        myCurrentIsBroken = true;
    }
    if (!myCurrentIsBroken) {
        try {
            myJunctionControlBuilder.getTLLogicControlToUse().addWAUTSwitch(myCurrentWAUTID, t, to);
        } catch (InvalidArgument &e) {
            MsgHandler::getErrorInstance()->inform(e.what());
            myCurrentIsBroken = true;
        }
    }
}


void
NLHandler::addWAUTJunction(const SUMOSAXAttributes &attrs) {
    bool ok = true;
    std::string wautID = attrs.getStringReporting(SUMO_ATTR_WAUT_ID, 0, ok);
    std::string junctionID = attrs.getStringReporting(SUMO_ATTR_JUNCTION_ID, 0, ok);
    std::string procedure = attrs.getOptStringReporting(SUMO_ATTR_PROCEDURE, 0, ok, "");
    bool synchron = attrs.getOptBoolReporting(SUMO_ATTR_SYNCHRON, 0, ok, false);
    if (!ok) {
        myCurrentIsBroken = true;
    }
    try {
        if (!myCurrentIsBroken) {
            myJunctionControlBuilder.getTLLogicControlToUse().addWAUTJunction(wautID, junctionID, procedure, synchron);
        }
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
        myCurrentIsBroken = true;
    }
}







void
NLHandler::addPOI(const SUMOSAXAttributes &attrs) {
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes(id)) {
        return;
    }
    const SUMOReal INVALID_POSITION(-1000000);
    bool ok = true;
    SUMOReal x = attrs.getOptSUMORealReporting(SUMO_ATTR_X, id.c_str(), ok, INVALID_POSITION);
    SUMOReal y = attrs.getOptSUMORealReporting(SUMO_ATTR_Y, id.c_str(), ok, INVALID_POSITION);
    SUMOReal lanePos = attrs.getOptSUMORealReporting(SUMO_ATTR_POSITION, id.c_str(), ok, INVALID_POSITION);
    int layer = attrs.getOptIntReporting(SUMO_ATTR_LAYER, id.c_str(), ok, 1);
    std::string type = attrs.getOptStringReporting(SUMO_ATTR_TYPE, id.c_str(), ok, "");
    std::string laneID = attrs.getOptStringReporting(SUMO_ATTR_LANE, id.c_str(), ok, "");
    std::string colorStr = attrs.getOptStringReporting(SUMO_ATTR_COLOR, id.c_str(), ok, "1,0,0");
    RGBColor color = RGBColor::parseColorReporting(colorStr, attrs.getObjectType(), id.c_str(), true, ok);
    if (!ok) {
        return;
    }
    Position2D pos(x, y);
    if (x==INVALID_POSITION||y==INVALID_POSITION) {
        MSLane *lane = MSLane::dictionary(laneID);
        if (lane==0) {
            MsgHandler::getErrorInstance()->inform("Lane '" + laneID + "' to place a poi '" + id + "'on is not known.");
            return;
        }
        if (lanePos<0) {
            lanePos = lane->getLength() + lanePos;
        }
        pos = lane->getShape().positionAtLengthPosition(lanePos);
    }
    if(!myNet.getShapeContainer().addPoI(id, layer, type, color, pos)) {
        MsgHandler::getErrorInstance()->inform("PoI '" + id + "' already exists.");
    }
}


void
NLHandler::addPoly(const SUMOSAXAttributes &attrs) {
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes(id)) {
        return;
    }
    bool ok = true;
    int layer = attrs.getOptIntReporting(SUMO_ATTR_LAYER, id.c_str(), ok, 1);
    bool fill = attrs.getOptBoolReporting(SUMO_ATTR_FILL, id.c_str(), ok, false);
    std::string type = attrs.getOptStringReporting(SUMO_ATTR_TYPE, id.c_str(), ok, "");
    std::string colorStr = attrs.getStringReporting(SUMO_ATTR_COLOR, id.c_str(), ok);
    RGBColor color = RGBColor::parseColorReporting(colorStr, attrs.getObjectType(), id.c_str(), true, ok);
    Position2DVector shape = GeomConvHelper::parseShapeReporting(attrs.getStringReporting(SUMO_ATTR_SHAPE, id.c_str(), ok), attrs.getObjectType(), id.c_str(), ok, false);
    if(!myNet.getShapeContainer().addPolygon(id, layer, type, color, fill, shape)) {
        MsgHandler::getErrorInstance()->inform("Polygon '" + id + "' already exists.");
    }
}


void
NLHandler::addLogicItem(const SUMOSAXAttributes &attrs) {
    bool ok = true;
    int request = attrs.getIntReporting(SUMO_ATTR_REQUEST, 0, ok);
    bool cont = false;
#ifdef HAVE_INTERNAL_LANES
    cont = attrs.getOptBoolReporting(SUMO_ATTR_CONT, 0, ok, false);
#endif
    std::string response = attrs.getStringReporting(SUMO_ATTR_RESPONSE, 0, ok);
    std::string foes = attrs.getStringReporting(SUMO_ATTR_FOES, 0, ok);
    if (!ok) {
        return;
    }
    // store received information
    if (request>=0 && response.length()>0) {
        try {
            myJunctionControlBuilder.addLogicItem(request, response, foes, cont);
        } catch (InvalidArgument &e) {
            MsgHandler::getErrorInstance()->inform(e.what());
        }
    }
}


void
NLHandler::initJunctionLogic(const SUMOSAXAttributes &attrs) {
    if (!attrs.hasAttribute(SUMO_ATTR_ID)) {
        // @deprecated: assuming a net could still use characters for the id
        myJunctionControlBuilder.initJunctionLogic("", -1);
        return;
    }
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes(id)) {
        return;
    }
    bool ok = true;
    int requestSize = attrs.getIntReporting(SUMO_ATTR_REQUESTSIZE, id.c_str(), ok);
    if (ok) {
        myJunctionControlBuilder.initJunctionLogic(id, requestSize);
    }
}


void
NLHandler::initTrafficLightLogic(const SUMOSAXAttributes &attrs) {
    myAmInTLLogicMode = true;
    SUMOReal detectorOffset = -1;
    myJunctionControlBuilder.initIncomingLanes(); // @deprecated (is this still used?)
    bool ok = true;
    std::string type = attrs.getStringReporting(SUMO_ATTR_TYPE, 0, ok);
    detectorOffset = attrs.getOptSUMORealReporting(SUMO_ATTR_DET_OFFSET, 0, ok, -1);
    //
    if (!attrs.hasAttribute(SUMO_ATTR_ID)) {
        // @deprecated: assuming a net could still use characters for the id
        myJunctionControlBuilder.initTrafficLightLogic("", "", type, 0, detectorOffset);
        return;
    }
    std::string id = attrs.getStringReporting(SUMO_ATTR_ID, 0, ok);
    int offset = attrs.getOptSUMOTimeReporting(SUMO_ATTR_OFFSET, id.c_str(), ok, 0);
    if (!ok) {
        return;
    }
    std::string programID = attrs.getOptStringReporting(SUMO_ATTR_PROGRAMID, id.c_str(), ok, "<unknown>");
    myJunctionControlBuilder.initTrafficLightLogic(id, programID, type, offset, detectorOffset);
}


void
NLHandler::addPhase(const SUMOSAXAttributes &attrs) {
    // try to get the phase definition
    std::string state;
    std::string phase;
    std::string brakeMask;
    std::string yellowMask;
    bool ok = true;
    if (attrs.hasAttribute(SUMO_ATTR_STATE)) {
        // ok, doing it the new way
        state = attrs.getStringReporting(SUMO_ATTR_STATE, 0, ok);
    } else {
        phase = attrs.getStringReporting(SUMO_ATTR_PHASE, 0, ok);
        brakeMask = attrs.getStringReporting(SUMO_ATTR_BRAKE, 0, ok);
        yellowMask = attrs.getStringReporting(SUMO_ATTR_YELLOW, 0, ok);
        // check
        if (phase.length()!=brakeMask.length()||phase.length()!=yellowMask.length()) {
            MsgHandler::getErrorInstance()->inform("Definition of traffic light is broken - descriptions have different lengths.");
            return;
        }
        if (!myHaveWarnedAboutDeprecatedPhases) {
            myHaveWarnedAboutDeprecatedPhases = true;
            MsgHandler::getWarningInstance()->inform("Deprecated tls phase definition found; replace by one using states.");
        }
        // convert to new
        state = MSPhaseDefinition::old2new(phase, brakeMask, yellowMask);
    }
    if (!ok) {
        return;
    }
    // try to get the phase duration
    SUMOTime duration = attrs.getSUMOTimeReporting(SUMO_ATTR_DURATION, myJunctionControlBuilder.getActiveKey().c_str(), ok);
    if (duration==0) {
        MsgHandler::getErrorInstance()->inform("Duration of tls-logic '" + myJunctionControlBuilder.getActiveKey() + "/" + myJunctionControlBuilder.getActiveSubKey() + "' is zero.");
        return;
    }
    // if the traffic light is an actuated traffic light, try to get
    //  the minimum and maximum durations
    SUMOTime minDuration = attrs.getOptSUMOTimeReporting(SUMO_ATTR_MINDURATION, myJunctionControlBuilder.getActiveKey().c_str(), ok, -1);
    SUMOTime maxDuration = attrs.getOptSUMOTimeReporting(SUMO_ATTR_MAXDURATION, myJunctionControlBuilder.getActiveKey().c_str(), ok, -1);
    myJunctionControlBuilder.addPhase(duration, state, minDuration, maxDuration);
}


#ifdef _MESSAGES
void
NLHandler::addMsgEmitter(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string id = attrs.getStringReporting(SUMO_ATTR_ID, 0, ok);
    std::string file = attrs.getOptStringReporting(SUMO_ATTR_FILE, 0, ok, "");
    // if no file given, use stdout
    if (file=="") {
        file = "-";
    }
    SUMOTime step = attrs.getOptSUMOTimeReporting(SUMO_ATTR_STEP, id.c_str(), ok, 1);
    bool reverse = attrs.getOptBoolReporting(SUMO_ATTR_REVERSE, 0, ok, false);
    bool table = attrs.getOptBoolReporting(SUMO_ATTR_TABLE, 0, ok, false);
    bool xycoord = attrs.getOptBoolReporting(SUMO_ATTR_XY, 0, ok, false);
    std::string whatemit = attrs.getStringReporting(SUMO_ATTR_EVENTS, 0, ok);
    if (!ok) {
        return;
    }
    myNet.createMsgEmitter(id, file, getFileName(), whatemit, reverse, table, xycoord, step);
}
#endif


void
NLHandler::addDetector(const SUMOSAXAttributes &attrs) {
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes(id)) {
        return;
    }
    // try to get the type
    bool ok = true;
    std::string type = attrs.getOptStringReporting(SUMO_ATTR_TYPE, 0, ok, "induct_loop");
    // build in dependence to type
    // induct loops (E1-detectors)
    if (type=="induct_loop"||type=="E1"||type=="e1") {
        addE1Detector(attrs);
        myCurrentDetectorType = "e1";
        return;
    }
    // lane-based areal detectors (E2-detectors)
    if (type=="lane_based"||type=="E2"||type=="e2") {
        addE2Detector(attrs);
        myCurrentDetectorType = "e2";
        return;
    }
    // multi-origin/multi-destination detectors (E3-detectors)
    if (type=="multi_od"||type=="E3"||type=="e3") {
        beginE3Detector(attrs);
        myCurrentDetectorType = "e3";
        return;
    }
#ifdef _MESSAGES
    // new induct loop, for static messages
    if (type=="il_msg"||type=="E4"||type=="e4") {
        addMsgDetector(attrs);
        myCurrentDetectorType="e4";
    }
#endif
}

#ifdef _MESSAGES
void
NLHandler::addMsgDetector(const SUMOSAXAttributes &attrs) {
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes(id)) {
        return;
    }
    bool ok = true;
    SUMOReal position = attrs.getSUMORealReporting(SUMO_ATTR_POSITION, id.c_str(), ok);
    bool friendlyPos = attrs.getOptBoolReporting(SUMO_ATTR_FRIENDLY_POS, id.c_str(), ok, false);
    std::string lane = attrs.getStringReporting(SUMO_ATTR_LANE, id.c_str(), ok);
    std::string msg = attrs.getStringReporting(SUMO_ATTR_MSG, id.c_str(), ok);
    std::string file = attrs.getStringReporting(SUMO_ATTR_FILE, id.c_str(), ok);
    if (!ok) {
        return;
    }
    try {
        myDetectorBuilder.buildMsgDetector(id, lane, position, 1, msg,
                                           OutputDevice::getDevice(file, getFileName()), friendlyPos);
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (IOError &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    }
}
#endif


void
NLHandler::addE1Detector(const SUMOSAXAttributes &attrs) {
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes(id)) {
        return;
    }
    // inform the user about deprecated values
    bool ok = true;
    if (attrs.getOptStringReporting(SUMO_ATTR_STYLE, id.c_str(), ok, "<invalid>")!="<invalid>") {
        MsgHandler::getWarningInstance()->inform("While parsing E1-detector '" + id + "': 'style' is deprecated.");
    }
    SUMOTime frequency = attrs.getSUMOTimeReporting(SUMO_ATTR_FREQUENCY, id.c_str(), ok);
    SUMOReal position = attrs.getSUMORealReporting(SUMO_ATTR_POSITION, id.c_str(), ok);
    bool friendlyPos = attrs.getOptBoolReporting(SUMO_ATTR_FRIENDLY_POS, id.c_str(), ok, false);
    std::string lane = attrs.getStringReporting(SUMO_ATTR_LANE, id.c_str(), ok);
    std::string file = attrs.getStringReporting(SUMO_ATTR_FILE, id.c_str(), ok);
    if (!ok) {
        return;
    }
    try {
        myDetectorBuilder.buildInductLoop(id, lane, position, frequency,
                                          OutputDevice::getDevice(file, getFileName()),
                                          friendlyPos);
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (IOError &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    }
}


void
NLHandler::addVTypeProbeDetector(const SUMOSAXAttributes &attrs) {
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes(id)) {
        return;
    }
    bool ok = true;
    SUMOTime frequency = attrs.getSUMOTimeReporting(SUMO_ATTR_FREQUENCY, id.c_str(), ok);
    std::string type = attrs.getStringSecure(SUMO_ATTR_TYPE, "");
    std::string file = attrs.getStringReporting(SUMO_ATTR_FILE, id.c_str(), ok);
    if (!ok) {
        return;
    }
    try {
        myDetectorBuilder.buildVTypeProbe(id, type, frequency, OutputDevice::getDevice(file, getFileName()));
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (IOError &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    }
}


void
NLHandler::addRouteProbeDetector(const SUMOSAXAttributes &attrs) {
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes(id)) {
        return;
    }
    bool ok = true;
    SUMOTime frequency = attrs.getSUMOTimeReporting(SUMO_ATTR_FREQUENCY, id.c_str(), ok);
    SUMOTime begin = attrs.getOptSUMOTimeReporting(SUMO_ATTR_BEGIN, id.c_str(), ok, -1);
    std::string edge = attrs.getStringReporting(SUMO_ATTR_EDGE, id.c_str(), ok);
    std::string file = attrs.getStringReporting(SUMO_ATTR_FILE, id.c_str(), ok);
    if (!ok) {
        return;
    }
    try {
        myDetectorBuilder.buildRouteProbe(id, edge, frequency, begin,
                                          OutputDevice::getDevice(file, getFileName()));
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (IOError &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    }
}



void
NLHandler::addE2Detector(const SUMOSAXAttributes &attrs) {
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes(id)) {
        return;
    }
    // check whether this is a detector connected to a tls an optionally to a link
    bool ok = true;
    std::string lsaid = attrs.getOptStringReporting(SUMO_ATTR_TLID, id.c_str(), ok, "<invalid>");
    std::string toLane = attrs.getOptStringReporting(SUMO_ATTR_TO, id.c_str(), ok, "<invalid>");
    // inform the user about deprecated values
    if (attrs.getOptStringReporting(SUMO_ATTR_MEASURES, id.c_str(), ok, "<invalid>")!="<invalid>") {
        MsgHandler::getWarningInstance()->inform("While parsing E2-detector '" + id + "': 'measures' is deprecated.");
    }
    if (attrs.getOptStringReporting(SUMO_ATTR_STYLE, id.c_str(), ok, "<invalid>")!="<invalid>") {
        MsgHandler::getWarningInstance()->inform("While parsing E2-detector '" + id + "': 'style' is deprecated.");
    }
    //
    try {
        const SUMOTime haltingTimeThreshold = attrs.getOptSUMOTimeReporting(SUMO_ATTR_HALTING_TIME_THRESHOLD, id.c_str(), ok, TIME2STEPS(1));
        const SUMOReal haltingSpeedThreshold = attrs.getOptSUMORealReporting(SUMO_ATTR_HALTING_SPEED_THRESHOLD, id.c_str(), ok, 5.0f/3.6f);
        const SUMOReal jamDistThreshold = attrs.getOptSUMORealReporting(SUMO_ATTR_JAM_DIST_THRESHOLD, id.c_str(), ok, 10.0f);
        const SUMOReal position = attrs.getSUMORealReporting(SUMO_ATTR_POSITION, id.c_str(), ok);
        const SUMOReal length = attrs.getSUMORealReporting(SUMO_ATTR_LENGTH, id.c_str(), ok);
        const bool friendlyPos = attrs.getOptBoolReporting(SUMO_ATTR_FRIENDLY_POS, id.c_str(), ok, false);
        const bool cont = attrs.getOptBoolReporting(SUMO_ATTR_CONT, id.c_str(), ok, false);
        const std::string lane = attrs.getStringReporting(SUMO_ATTR_LANE, id.c_str(), ok);
        const std::string file = attrs.getStringReporting(SUMO_ATTR_FILE, id.c_str(), ok);
        if (!ok) {
            return;
        }
        if (lsaid!="<invalid>") {
            if (toLane=="<invalid>") {
                myDetectorBuilder.buildE2Detector(id, lane, position, length, cont,
                                                  myJunctionControlBuilder.getTLLogic(lsaid),
                                                  OutputDevice::getDevice(file, getFileName()),
                                                  (SUMOTime) haltingSpeedThreshold, haltingSpeedThreshold, jamDistThreshold,
                                                  friendlyPos);
            } else {
                myDetectorBuilder.buildE2Detector(id, lane, position, length, cont,
                                                  myJunctionControlBuilder.getTLLogic(lsaid), toLane,
                                                  OutputDevice::getDevice(file, getFileName()),
                                                  (SUMOTime) haltingSpeedThreshold, haltingSpeedThreshold, jamDistThreshold,
                                                  friendlyPos);
            }
        } else {
            bool ok = true;
            SUMOTime frequency = attrs.getSUMOTimeReporting(SUMO_ATTR_FREQUENCY, id.c_str(), ok);
            if (!ok) {
                return;
            }
            myDetectorBuilder.buildE2Detector(id, lane, position, length, cont, frequency,
                                              OutputDevice::getDevice(file, getFileName()),
                                              (SUMOTime) haltingSpeedThreshold, haltingSpeedThreshold, jamDistThreshold,
                                              friendlyPos);
        }
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (IOError &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    }
}


void
NLHandler::beginE3Detector(const SUMOSAXAttributes &attrs) {
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes(id)) {
        return;
    }
    bool ok = true;
    // inform the user about deprecated values
    if (attrs.getOptStringReporting(SUMO_ATTR_MEASURES, id.c_str(), ok, "<invalid>")!="<invalid>") {
        MsgHandler::getWarningInstance()->inform("While parsing E3-detector '" + id + "': 'measures' is deprecated.");
    }
    if (attrs.getOptStringReporting(SUMO_ATTR_STYLE, id.c_str(), ok, "<invalid>")!="<invalid>") {
        MsgHandler::getWarningInstance()->inform("While parsing E3-detector '" + id + "': 'style' is deprecated.");
    }
    const SUMOTime frequency = attrs.getSUMOTimeReporting(SUMO_ATTR_FREQUENCY, id.c_str(), ok);
    const SUMOTime haltingTimeThreshold = attrs.getOptSUMOTimeReporting(SUMO_ATTR_HALTING_TIME_THRESHOLD, id.c_str(), ok, TIME2STEPS(1));
    const SUMOReal haltingSpeedThreshold = attrs.getOptSUMORealReporting(SUMO_ATTR_HALTING_SPEED_THRESHOLD, id.c_str(), ok, 5.0f/3.6f);
    const std::string file = attrs.getStringReporting(SUMO_ATTR_FILE, id.c_str(), ok);
    if (!ok) {
        return;
    }
    try {
        myDetectorBuilder.beginE3Detector(id,
                                          OutputDevice::getDevice(file, getFileName()),
                                          frequency, haltingSpeedThreshold, haltingTimeThreshold);
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (IOError &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    }
}


void
NLHandler::addE3Entry(const SUMOSAXAttributes &attrs) {
    bool ok = true;
    const SUMOReal position = attrs.getSUMORealReporting(SUMO_ATTR_POSITION, myDetectorBuilder.getCurrentE3ID().c_str(), ok);
    const bool friendlyPos = attrs.getOptBoolReporting(SUMO_ATTR_FRIENDLY_POS, myDetectorBuilder.getCurrentE3ID().c_str(), ok, false);
    const std::string lane = attrs.getStringReporting(SUMO_ATTR_LANE, myDetectorBuilder.getCurrentE3ID().c_str(), ok);
    if (!ok) {
        return;
    }
    myDetectorBuilder.addE3Entry(lane, position, friendlyPos);
}


void
NLHandler::addE3Exit(const SUMOSAXAttributes &attrs) {
    bool ok = true;
    const SUMOReal position = attrs.getSUMORealReporting(SUMO_ATTR_POSITION, myDetectorBuilder.getCurrentE3ID().c_str(), ok);
    const bool friendlyPos = attrs.getOptBoolReporting(SUMO_ATTR_FRIENDLY_POS, myDetectorBuilder.getCurrentE3ID().c_str(), ok, false);
    const std::string lane = attrs.getStringReporting(SUMO_ATTR_LANE, myDetectorBuilder.getCurrentE3ID().c_str(), ok);
    if (!ok) {
        return;
    }
    myDetectorBuilder.addE3Exit(lane, position, friendlyPos);
}


void
NLHandler::addEdgeLaneMeanData(const SUMOSAXAttributes &attrs, const char* objecttype) {
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes(id)) {
        return;
    }
    bool ok = true;
    const SUMOReal maxTravelTime = attrs.getOptSUMORealReporting(SUMO_ATTR_MAX_TRAVELTIME, id.c_str(), ok, 100000);
    const SUMOReal minSamples = attrs.getOptSUMORealReporting(SUMO_ATTR_MIN_SAMPLES, id.c_str(), ok, 0);
    const SUMOReal haltingSpeedThreshold = attrs.getOptSUMORealReporting(SUMO_ATTR_HALTING_SPEED_THRESHOLD, id.c_str(), ok, POSITION_EPS);
    const bool excludeEmpty = attrs.getOptBoolReporting(SUMO_ATTR_EXCLUDE_EMPTY, id.c_str(), ok, false);
    const bool withInternal = attrs.getOptBoolReporting(SUMO_ATTR_WITH_INTERNAL, id.c_str(), ok, false);
    const bool trackVehicles = attrs.getOptBoolReporting(SUMO_ATTR_TRACK_VEHICLES, id.c_str(), ok, false);
    const std::string file = attrs.getStringReporting(SUMO_ATTR_FILE, id.c_str(), ok);
    const std::string type = attrs.getOptStringReporting(SUMO_ATTR_TYPE, id.c_str(), ok, "performance");
    const std::string vtypes = attrs.getOptStringReporting(SUMO_ATTR_VTYPES, id.c_str(), ok, "");
    const SUMOTime frequency = attrs.getOptSUMOTimeReporting(SUMO_ATTR_FREQUENCY, id.c_str(), ok, -1);
    const SUMOTime begin = attrs.getOptSUMOTimeReporting(SUMO_ATTR_BEGIN, id.c_str(), ok, string2time(OptionsCont::getOptions().getString("begin")));
    const SUMOTime end = attrs.getOptSUMOTimeReporting(SUMO_ATTR_END, id.c_str(), ok, string2time(OptionsCont::getOptions().getString("end")));
    if (!ok) {
        return;
    }
    try {
        myDetectorBuilder.createEdgeLaneMeanData(id, frequency, begin, end,
                type, std::string(objecttype)=="meandata_lane", !excludeEmpty, withInternal, trackVehicles,
                maxTravelTime, minSamples, haltingSpeedThreshold, vtypes,
                OutputDevice::getDevice(file, getFileName()));
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (IOError &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    }
}



void
NLHandler::addSource(const SUMOSAXAttributes &attrs) {
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes(id)) {
        return;
    }
    try {
        myTriggerBuilder.buildTrigger(myNet, attrs, getFileName());
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    }
}


void
NLHandler::addTrigger(const SUMOSAXAttributes &attrs) {
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes(id)) {
        return;
    }
    try {
        myTriggerBuilder.buildTrigger(myNet, attrs, getFileName());
        return;
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    }
}


void
NLHandler::openSucc(const SUMOSAXAttributes &attrs) {
    bool ok = true;
    std::string id = attrs.getStringReporting(SUMO_ATTR_LANE, 0, ok);
    if (!MSGlobals::gUsingInternalLanes&&id[0]==':') {
        myCurrentIsInternalToSkip = true;
        return;
    }
    myCurrentIsInternalToSkip = false;
    mySucceedingLaneBuilder.openSuccLane(id);
}


void
NLHandler::addSuccLane(const SUMOSAXAttributes &attrs) {
    // do not process internal lanes if not wished
    if (myCurrentIsInternalToSkip) {
        return;
    }
    try {
        bool ok = true;
        SUMOReal pass = attrs.getOptSUMORealReporting(SUMO_ATTR_PASS, 0, ok, -1);
        bool yield = attrs.getBoolReporting(SUMO_ATTR_YIELD, 0, ok);
        bool internalEnd = attrs.getOptBoolReporting(SUMO_ATTR_YIELD, 0, ok, false);
        std::string lane = attrs.getStringReporting(SUMO_ATTR_LANE, 0, ok);
        std::string dir = attrs.getStringReporting(SUMO_ATTR_DIR, 0, ok);
        std::string state = attrs.getStringReporting(SUMO_ATTR_STATE, 0, ok);
        std::string tlID = attrs.getOptStringReporting(SUMO_ATTR_TLID, 0, ok, "");
#ifdef HAVE_INTERNAL_LANES
        std::string via = attrs.getOptStringReporting(SUMO_ATTR_VIA, 0, ok, "");
#endif
        if (!ok) {
            return;
        }
        if (tlID!="") {
            int linkNumber = attrs.getIntReporting(SUMO_ATTR_TLLINKNO, 0, ok);
            if (!ok) {
                return;
            }
            mySucceedingLaneBuilder.addSuccLane(lane,
#ifdef HAVE_INTERNAL_LANES
                                                via, pass,
#endif
                                                parseLinkDir(dir[0]), parseLinkState(state[0]), internalEnd, tlID, linkNumber);
        } else {
            mySucceedingLaneBuilder.addSuccLane(lane,
#ifdef HAVE_INTERNAL_LANES
                                                via, pass,
#endif
                                                parseLinkDir(dir[0]), parseLinkState(state[0]), internalEnd);
        }
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    }
}



MSLink::LinkDirection
NLHandler::parseLinkDir(char dir) {
    switch (dir) {
    case 's':
        return MSLink::LINKDIR_STRAIGHT;
    case 'l':
        return MSLink::LINKDIR_LEFT;
    case 'r':
        return MSLink::LINKDIR_RIGHT;
    case 't':
        return MSLink::LINKDIR_TURN;
    case 'L':
        return MSLink::LINKDIR_PARTLEFT;
    case 'R':
        return MSLink::LINKDIR_PARTRIGHT;
    default:
        throw InvalidArgument("Unrecognised link direction '" + toString(dir) + "'.");
    }
}


MSLink::LinkState
NLHandler::parseLinkState(char state) {
    switch (state) {
    case 't':
    case 'o':
        return MSLink::LINKSTATE_TL_OFF_BLINKING;
    case 'O':
        return MSLink::LINKSTATE_TL_OFF_NOSIGNAL;
    case 'M':
        return MSLink::LINKSTATE_MAJOR;
    case 'm':
        return MSLink::LINKSTATE_MINOR;
    case '=':
        return MSLink::LINKSTATE_EQUAL;
    default:
        throw InvalidArgument("Unrecognised link state '" + toString(state) + "'.");
    }
}



// ----------------------------------
void
NLHandler::setRequestSize(const std::string &chars) {
    // @deprecated: assuming a net could still use characters for the request size
    try {
        myJunctionControlBuilder.setRequestSize(TplConvert<char>::_2int(chars.c_str()));
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Missing request size.");
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("One of an edge's SUMOSAXAttributes must be numeric but is not.");
    }
}


void
NLHandler::setLocation(const SUMOSAXAttributes &attrs) {
    bool ok = true;
    Position2DVector s = GeomConvHelper::parseShapeReporting(
            attrs.getStringReporting(SUMO_ATTR_NET_OFFSET, 0, ok),
            attrs.getObjectType(), 0, ok, false);
    Boundary convBoundary = GeomConvHelper::parseBoundaryReporting(
            attrs.getStringReporting(SUMO_ATTR_CONV_BOUNDARY, 0, ok),
            attrs.getObjectType(), 0, ok);
    Boundary origBoundary = GeomConvHelper::parseBoundaryReporting(
            attrs.getStringReporting(SUMO_ATTR_ORIG_BOUNDARY, 0, ok),
            attrs.getObjectType(), 0, ok);
    std::string proj = attrs.getStringReporting(SUMO_ATTR_ORIG_PROJ, 0, ok);
    if (ok) {
        Position2D networkOffset = s[0];
        GeoConvHelper::init(proj, networkOffset, origBoundary, convBoundary);
    }
}


void
NLHandler::addDistrict(const SUMOSAXAttributes &attrs) throw(ProcessError) {
    myCurrentIsBroken = false;
    // get the id, report an error if not given or empty...
    if (!attrs.setIDFromAttributes(myCurrentDistrictID)) {
        myCurrentIsBroken = true;
        return;
    }
    try {
        MSEdge* sink = myEdgeControlBuilder.buildEdge(myCurrentDistrictID + "-sink");
        if (!MSEdge::dictionary(myCurrentDistrictID + "-sink", sink)) {
            delete sink;
            throw InvalidArgument("Another edge with the id '" + myCurrentDistrictID + "-sink' exists.");
        }
        sink->initialize(0, 0, MSEdge::EDGEFUNCTION_DISTRICT);
        MSEdge* source = myEdgeControlBuilder.buildEdge(myCurrentDistrictID + "-source");
        if (!MSEdge::dictionary(myCurrentDistrictID + "-source", source)) {
            delete source;
            throw InvalidArgument("Another edge with the id '" + myCurrentDistrictID + "-source' exists.");
        }
        source->initialize(0, 0, MSEdge::EDGEFUNCTION_DISTRICT);
        if (attrs.hasAttribute(SUMO_ATTR_EDGES)) {
            std::vector<std::string> desc = StringTokenizer(attrs.getString(SUMO_ATTR_EDGES)).getVector();
            for (std::vector<std::string>::const_iterator i=desc.begin(); i!=desc.end(); ++i) {
                MSEdge *edge = MSEdge::dictionary(*i);
                // check whether the edge exists
                if (edge==0) {
                    throw InvalidArgument("The edge '" + *i + "' within district '" + myCurrentDistrictID + "' is not known.");
                }
                source->addFollower(edge);
                edge->addFollower(sink);
            }
        }
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
        myCurrentIsBroken = true;
    }
}


void
NLHandler::addDistrictEdge(const SUMOSAXAttributes &attrs, bool isSource) {
    if (myCurrentIsBroken) {
        // earlier error
        return;
    }
    bool ok = true;
    std::string id = attrs.getStringReporting(SUMO_ATTR_ID, myCurrentDistrictID.c_str(), ok);
    MSEdge *succ = MSEdge::dictionary(id);
    if (succ!=0) {
        // connect edge
        if (isSource) {
            MSEdge::dictionary(myCurrentDistrictID + "-source")->addFollower(succ);
        } else {
            succ->addFollower(MSEdge::dictionary(myCurrentDistrictID + "-sink"));
        }
    } else {
        MsgHandler::getErrorInstance()->inform("At district '" + myCurrentDistrictID + "': succeeding edge '" + id + "' does not exist.");
    }
}




void
NLHandler::addIncomingLanes(const std::string &chars) {
    // @deprecated: at some time, all junctions should have a shape attribute (moved from characters)
    StringTokenizer st(chars);
    while (st.hasNext()) {
        std::string set = st.next();
        MSLane *lane = MSLane::dictionary(set);
        if (!MSGlobals::gUsingInternalLanes&&set[0]==':') {
            continue;
        }
        if (lane==0) {
            MsgHandler::getErrorInstance()->inform("An unknown lane ('" + set + "') was tried to be set as incoming to junction '" + myJunctionControlBuilder.getActiveID() + "'.");
            return;
        }
        myJunctionControlBuilder.addIncomingLane(lane);
    }
}


#ifdef HAVE_INTERNAL_LANES
void
NLHandler::addInternalLanes(const std::string &chars) {
    // @deprecated: at some time, all junctions should have a shape attribute (moved from characters)
    // do not parse internal lanes if not wished
    if (!MSGlobals::gUsingInternalLanes) {
        return;
    }
    StringTokenizer st(chars);
    while (st.hasNext()) {
        std::string set = st.next();
        MSLane *lane = MSLane::dictionary(set);
        if (lane==0) {
            MsgHandler::getErrorInstance()->inform("An unknown lane ('" + set + "') was tried to be set as internal.");
            return;
        }
        myJunctionControlBuilder.addInternalLane(lane);
    }
}
#endif

// ----------------------------------







void
NLHandler::closeSuccLane() {
    // do not process internal lanes if not wished
    if (myCurrentIsInternalToSkip) {
        return;
    }
    try {
        mySucceedingLaneBuilder.closeSuccLane();
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    }
}


void
NLHandler::endDetector() {
    if (myCurrentDetectorType=="e3") {
        endE3Detector();
    }
    myCurrentDetectorType = "";
}


void
NLHandler::endE3Detector() {
    try {
        myDetectorBuilder.endE3Detector();
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    }
}


void
NLHandler::closeWAUT() {
    if (!myCurrentIsBroken) {
        try {
            myJunctionControlBuilder.getTLLogicControlToUse().closeWAUT(myCurrentWAUTID);
        } catch (InvalidArgument &e) {
            MsgHandler::getErrorInstance()->inform(e.what());
            myCurrentIsBroken = true;
        }
    }
    myCurrentWAUTID = "";
}


/****************************************************************************/
