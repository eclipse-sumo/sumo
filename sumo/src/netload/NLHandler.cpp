/****************************************************************************/
/// @file    NLHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// The XML-Handler for network loading
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>
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
#include "NLGeomShapeBuilder.h"

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
                     NLJunctionControlBuilder &junctionBuilder,
                     NLGeomShapeBuilder &shapeBuilder) throw()
        : MSRouteHandler(file, true),
        myNet(net), myActionBuilder(net),
        myCurrentIsInternalToSkip(false),
        myDetectorBuilder(detBuilder), myTriggerBuilder(triggerBuilder),
        myEdgeControlBuilder(edgeBuilder), myJunctionControlBuilder(junctionBuilder),
        myShapeBuilder(shapeBuilder), mySucceedingLaneBuilder(junctionBuilder),
        myAmInTLLogicMode(false), myCurrentIsBroken(false),
        myHaveWarnedAboutDeprecatedVClass(false),
        myHaveWarnedAboutDeprecatedJunctionShape(false),
        myHaveWarnedAboutDeprecatedLaneShape(false),
        myHaveWarnedAboutDeprecatedPolyShape(false),
        myHaveWarnedAboutDeprecatedLocation(false),
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
        case SUMO_TAG_EMITTER:
            myTriggerBuilder.parseAndBuildLaneEmitTrigger(myNet, attrs, getFileName());
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
            addEdgeMeanData(attrs);
            break;
        case SUMO_TAG_MEANDATA_LANE:
            addLaneMeanData(attrs);
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
    switch (element) {
    case SUMO_TAG_POLY:
        if (chars.length()!=0) {
            addPolyPosition(chars);
        }
        break;
    case SUMO_TAG_INCOMING_LANES:
        addIncomingLanes(chars);
        break;
#ifdef HAVE_INTERNAL_LANES
    case SUMO_TAG_INTERNAL_LANES:
        addInternalLanes(chars);
        break;
#endif
    case SUMO_TAG_LANE:
        if (chars.length()!=0) {
            addLaneShape(chars);
        }
        break;
    case SUMO_TAG_REQUESTSIZE:
        if (myJunctionControlBuilder.getActiveKey().length()!=0) {
            setRequestSize(chars);
        }
        break;
    case SUMO_TAG_LANENUMBER:
        if (myJunctionControlBuilder.getActiveKey().length()!=0) {
            setLaneNumber(chars);
        }
        break;
    case SUMO_TAG_KEY:
        setKey(chars);
        break;
    case SUMO_TAG_SUBKEY:
        setSubKey(chars);
        break;
    case SUMO_TAG_OFFSET:
        setOffset(chars);
        break;
    case SUMO_TAG_NET_OFFSET:
        setNetOffset(chars);
        break;
    case SUMO_TAG_CONV_BOUNDARY:
        setNetConv(chars);
        break;
    case SUMO_TAG_ORIG_BOUNDARY:
        setNetOrig(chars);
        break;
    case SUMO_TAG_ORIG_PROJ:
        GeoConvHelper::init(chars, myNetworkOffset, myOrigBoundary, myConvBoundary);
        break;
    case SUMO_TAG_SHAPE:
        if (chars.length()!=0) {
            addJunctionShape(chars);
        }
        break;
    default:
        break;
    }
    MSRouteHandler::myCharacters(element, chars);
}


void
NLHandler::myEndElement(SumoXMLTag element) throw(ProcessError) {
    switch (element) {
    case SUMO_TAG_EDGE:
        closeEdge();
        break;
    case SUMO_TAG_LANE:
        closeLane();
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
    if (!attrs.setIDFromAttributes("edge", id)) {
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
    std::string func = attrs.getStringReporting(SUMO_ATTR_FUNCTION, "edge", id.c_str(), ok);
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
    myShape.clear();
    // omit internal edges if not wished and broken edges
    if (myCurrentIsInternalToSkip||myCurrentIsBroken) {
        return;
    }
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes("lane", id)) {
        myCurrentIsBroken = true;
        return;
    }
    bool ok = true;
    try {
        myCurrentLaneID = id;
        myLaneIsDepart = attrs.getBoolReporting(SUMO_ATTR_DEPART, "lane", id.c_str(), ok);
        myCurrentMaxSpeed = attrs.getSUMORealReporting(SUMO_ATTR_MAXSPEED, "lane", id.c_str(), ok);
        myCurrentLength = attrs.getSUMORealReporting(SUMO_ATTR_LENGTH, "lane", id.c_str(), ok);
        std::string allow = attrs.getOptStringReporting(SUMO_ATTR_ALLOW, "lane", id.c_str(), ok, "");
        std::string disallow = attrs.getOptStringReporting(SUMO_ATTR_DISALLOW, "lane", id.c_str(), ok, "");
        std::string vclasses = attrs.getOptStringReporting(SUMO_ATTR_VCLASSES, "lane", id.c_str(), ok, "");
        myAllowedClasses.clear();
        myDisallowedClasses.clear();
        parseVehicleClasses(vclasses, allow, disallow,
                            myAllowedClasses, myDisallowedClasses, myHaveWarnedAboutDeprecatedVClass);
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Missing attribute in a lane-object (id='" + id + "').\n Can not build according edge.");
        myCurrentIsBroken = true;
    }
    if (!ok) {
        myCurrentIsBroken = true;
    }
    if (!myCurrentIsBroken) {
        if (attrs.hasAttribute(SUMO_ATTR_SHAPE)) {
            addLaneShape(attrs.getStringReporting(SUMO_ATTR_SHAPE, "lane", id.c_str(), ok));
        } else if (!myHaveWarnedAboutDeprecatedLaneShape) {
            myHaveWarnedAboutDeprecatedLaneShape = true;
            MsgHandler::getWarningInstance()->inform("Your network uses a deprecated lane shape description; please rebuild.");
        }
    }
}


void
NLHandler::addLaneShape(const std::string &chars) {
    // omit internal edges if not wished and broken edges
    if (myCurrentIsInternalToSkip||myCurrentIsBroken) {
        return;
    }
    try {
        myShape = GeomConvHelper::parseShape(chars);
        return;
    } catch (OutOfBoundsException &) {
    } catch (NumberFormatException &) {
    } catch (EmptyData &) {
    }
    MsgHandler::getErrorInstance()->inform("Could not parse shape of lane '" + myCurrentLaneID + "'.\n Can not build according edge.");
    myCurrentIsBroken = true;
}


void
NLHandler::closeLane() {
    // omit internal edges if not wished and broken edges
    if (myCurrentIsInternalToSkip||myCurrentIsBroken) {
        return;
    }
    // check shape
    if (myShape.size()<2) {
        MsgHandler::getErrorInstance()->inform("Shape of lane '" + myCurrentLaneID + "' is broken.\n Can not build according edge.");
        myCurrentIsBroken = true;
        return;
    }
    // build
    try {
        MSLane *lane =
            myEdgeControlBuilder.addLane(myCurrentLaneID, myCurrentMaxSpeed, myCurrentLength, myLaneIsDepart, myShape, myAllowedClasses, myDisallowedClasses);
        // insert the lane into the lane-dictionary, checking
        if (!MSLane::dictionary(myCurrentLaneID, lane)) {
            delete lane;
            MsgHandler::getErrorInstance()->inform("Another lane with the id '" + myCurrentLaneID + "' exists.");
            myCurrentIsBroken = true;
        }
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    }
}


// ---- the root/junction - element
void
NLHandler::openJunction(const SUMOSAXAttributes &attrs) {
    myCurrentIsBroken = false;
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes("junction", id)) {
        myCurrentIsBroken = true;
        return;
    }
    Position2DVector shape;
    if (attrs.hasAttribute(SUMO_ATTR_SHAPE)) {
        // @deprecated: at some time, all junctions should have a shape attribute (moved from characters)
        try {
            shape = GeomConvHelper::parseShape(attrs.getStringSecure(SUMO_ATTR_SHAPE, ""));
        } catch (OutOfBoundsException &) {
        } catch (NumberFormatException &) {
        } catch (EmptyData &) {
        }
    }
    bool ok = true;
    SUMOReal x = attrs.getSUMORealReporting(SUMO_ATTR_X, "junction", id.c_str(), ok);
    SUMOReal y = attrs.getSUMORealReporting(SUMO_ATTR_Y, "junction", id.c_str(), ok);
    std::string type = attrs.getStringReporting(SUMO_ATTR_TYPE, "junction", id.c_str(), ok);
    std::string key = attrs.getOptStringReporting(SUMO_ATTR_KEY, "junction", id.c_str(), ok, "");
    if (!ok) {
        myCurrentIsBroken = true;
    } else {
        try {
            myJunctionControlBuilder.openJunction(id, key, type, x, y, shape);
        } catch (EmptyData &) {
            MsgHandler::getErrorInstance()->inform("Missing attribute in junction '" + id + "'.\n Can not build according junction.");
            myCurrentIsBroken = true;
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
    std::string key = attrs.getStringReporting(SUMO_ATTR_KEY, 0, 0, ok);
    std::string val = attrs.getStringReporting(SUMO_ATTR_VALUE, 0, 0, ok);
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
    if (!attrs.setIDFromAttributes("waut", id)) {
        myCurrentIsBroken = true;
        return;
    }
    bool ok = true;
    SUMOTime t = attrs.getOptIntReporting(SUMO_ATTR_REF_TIME, "waut", id.c_str(), ok, 0);
    std::string pro = attrs.getStringReporting(SUMO_ATTR_START_PROG, "waut", id.c_str(), ok);
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
    SUMOTime t = attrs.getIntReporting(SUMO_ATTR_TIME, "wautSwitch", myCurrentWAUTID.c_str(), ok);
    std::string to = attrs.getStringReporting(SUMO_ATTR_TO, "wautSwitch", myCurrentWAUTID.c_str(), ok);
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
    std::string wautID = attrs.getStringReporting(SUMO_ATTR_WAUT_ID, "wautJunction", 0, ok);
    std::string junctionID = attrs.getStringReporting(SUMO_ATTR_JUNCTION_ID, "wautJunction", 0, ok);
    std::string procedure = attrs.getOptStringReporting(SUMO_ATTR_PROCEDURE, "wautJunction", 0, ok, "");
    bool synchron = attrs.getOptBoolReporting(SUMO_ATTR_SYNCHRON, "wautJunction", 0, ok, false);
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
    if (!attrs.setIDFromAttributes("poi", id)) {
        return;
    }
    bool ok = true;
    SUMOReal x = attrs.getOptSUMORealReporting(SUMO_ATTR_X, "poi", id.c_str(), ok, INVALID_POSITION);
    SUMOReal y = attrs.getOptSUMORealReporting(SUMO_ATTR_Y, "poi", id.c_str(), ok, INVALID_POSITION);
    SUMOReal lanePos = attrs.getOptSUMORealReporting(SUMO_ATTR_POSITION, "poi", id.c_str(), ok, INVALID_POSITION);
    int layer = attrs.getOptIntReporting(SUMO_ATTR_LAYER, "poi", id.c_str(), ok, 1);
    std::string type = attrs.getOptStringReporting(SUMO_ATTR_TYPE, "poi", id.c_str(), ok, "");
    std::string color = attrs.getOptStringReporting(SUMO_ATTR_COLOR, "poi", id.c_str(), ok, "1,0,0");
    std::string lane = attrs.getOptStringReporting(SUMO_ATTR_LANE, "poi", id.c_str(), ok, "");
    if (!ok) {
        return;
    }
    try {
        myShapeBuilder.addPoint(id, layer, type, RGBColor::parseColor(color),
                                x, y, lane, lanePos);
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (OutOfBoundsException &) {
        MsgHandler::getErrorInstance()->inform("Color definition of POI '" + id + "' seems to be broken.");
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("One of POI's '" + id + "' SUMOSAXAttributes should be numeric but is not.");
    }
}


void
NLHandler::addPoly(const SUMOSAXAttributes &attrs) {
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes("poly", id)) {
        return;
    }
    bool ok = true;
    int layer = attrs.getOptIntReporting(SUMO_ATTR_LAYER, "poly", id.c_str(), ok, 1);
    bool fill = attrs.getOptBoolReporting(SUMO_ATTR_FILL, "poly", id.c_str(), ok, false);
    std::string type = attrs.getOptStringReporting(SUMO_ATTR_FROM, "poly", id.c_str(), ok, "");
    std::string color = attrs.getStringReporting(SUMO_ATTR_COLOR, "poly", id.c_str(), ok);
    if (!ok) {
        return;
    }
    try {
        myShapeBuilder.polygonBegin(id, layer, type, RGBColor::parseColor(color), fill);
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("The color of polygon '" + id + "' could not be parsed.");
    } catch (BoolFormatException &) {
        MsgHandler::getErrorInstance()->inform("The attribute 'fill' of polygon '" + id + "' is not a valid bool.");
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Polygon '" + id + "' misses an attribute.");
    }
    if (attrs.hasAttribute(SUMO_ATTR_SHAPE)) {
        // @deprecated; at some time, this is mandatory (no character usage)
        addPolyPosition(attrs.getStringReporting(SUMO_ATTR_SHAPE, "poly", id.c_str(), ok));
    } else if (!myHaveWarnedAboutDeprecatedPolyShape) {
        myHaveWarnedAboutDeprecatedPolyShape = true;
        MsgHandler::getWarningInstance()->inform("You use a deprecated polygon shape description; use attribute 'shape' instead.");
    }
}


void
NLHandler::addLogicItem(const SUMOSAXAttributes &attrs) {
    bool ok = true;
    int request = attrs.getIntReporting(SUMO_ATTR_REQUEST, "request", 0, ok);
    bool cont = false;
#ifdef HAVE_INTERNAL_LANES
    cont = attrs.getOptBoolReporting(SUMO_ATTR_CONT, 0, 0, ok, false);
#endif
    std::string response = attrs.getStringReporting(SUMO_ATTR_RESPONSE, 0, 0, ok);
    std::string foes = attrs.getStringReporting(SUMO_ATTR_FOES, 0, 0, ok);
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
        myJunctionControlBuilder.initJunctionLogic("", -1, -1);
        return;
    }
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes("row-logic", id)) {
        return;
    }
    bool ok = true;
    int requestSize = attrs.getIntReporting(SUMO_ATTR_REQUESTSIZE, "row-logic", id.c_str(), ok);
    int laneNumber = attrs.getIntReporting(SUMO_ATTR_LANENUMBER, "row-logic", id.c_str(), ok);
    if (ok) {
        myJunctionControlBuilder.initJunctionLogic(id, requestSize, laneNumber);
    }
}


void
NLHandler::initTrafficLightLogic(const SUMOSAXAttributes &attrs) {
    SUMOReal detectorOffset = -1;
    myJunctionControlBuilder.initIncomingLanes(); // @deprecated (is this still used?)
    bool ok = true;
    std::string type = attrs.getStringReporting(SUMO_ATTR_TYPE, "tl-logic", 0, ok);
    detectorOffset = attrs.getOptSUMORealReporting(SUMO_ATTR_DET_OFFSET, "tl-logic", 0, ok, -1);
    //
    if (!attrs.hasAttribute(SUMO_ATTR_ID)) {
        // @deprecated: assuming a net could still use characters for the id
        myJunctionControlBuilder.initTrafficLightLogic("", "", type, 0, detectorOffset);
        return;
    }
    std::string id = attrs.getStringReporting(SUMO_ATTR_ID, "tl-logic", 0, ok);
    int offset = attrs.getOptSUMORealReporting(SUMO_ATTR_OFFSET, "tl-logic", id.c_str(), ok, 0);
    if (!ok) {
        return;
    }
    std::string programID = attrs.getOptStringReporting(SUMO_ATTR_PROGRAMID, "tl-logic", id.c_str(), ok, "<unknown>");
    myJunctionControlBuilder.initTrafficLightLogic(id, programID, type, offset, detectorOffset);
    myAmInTLLogicMode = true;
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
        state = attrs.getStringReporting(SUMO_ATTR_STATE, "phase", 0, ok);
    } else {
        phase = attrs.getStringReporting(SUMO_ATTR_PHASE, "phase", 0, ok);
        brakeMask = attrs.getStringReporting(SUMO_ATTR_BRAKE, "phase", 0, ok);
        yellowMask = attrs.getStringReporting(SUMO_ATTR_YELLOW, "phase", 0, ok);
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
    int duration = attrs.getIntReporting(SUMO_ATTR_DURATION, "phase", myJunctionControlBuilder.getActiveKey().c_str(), ok);
    if (duration==0) {
        MsgHandler::getErrorInstance()->inform("Duration of tls-logic '" + myJunctionControlBuilder.getActiveKey() + "/" + myJunctionControlBuilder.getActiveSubKey() + "' is zero.");
        return;
    }
    // if the traffic light is an actuated traffic light, try to get
    //  the minimum and maximum durations
    int minDuration = attrs.getOptIntReporting(SUMO_ATTR_MINDURATION, "phase", myJunctionControlBuilder.getActiveKey().c_str(), ok, -1);
    int maxDuration = attrs.getOptIntReporting(SUMO_ATTR_MAXDURATION, "phase", myJunctionControlBuilder.getActiveKey().c_str(), ok, -1);
    myJunctionControlBuilder.addPhase(duration, state, minDuration, maxDuration);
}


#ifdef _MESSAGES
void
NLHandler::addMsgEmitter(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string id = attrs.getStringReporting(SUMO_ATTR_ID, 0, 0, ok);
    std::string file = attrs.getOptStringReporting(SUMO_ATTR_FILE, 0, 0, ok, "");
    // if no file given, use stdout
    if (file=="") {
        file = "-";
    }
    SUMOReal step = attrs.getOptIntReporting(SUMO_ATTR_STEP, 0, id.c_str(), ok, 1);
    bool reverse = attrs.getOptBoolReporting(SUMO_ATTR_REVERSE, 0, 0, ok, false);
    bool table = attrs.getOptBoolReporting(SUMO_ATTR_TABLE, 0, 0, ok, false);
    bool xycoord = attrs.getOptBoolReporting(SUMO_ATTR_XY, 0, 0, ok, false);
    std::string whatemit = attrs.getStringReporting(SUMO_ATTR_EVENTS, 0, 0, ok);
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
    if (!attrs.setIDFromAttributes("detector", id)) {
        return;
    }
    // try to get the type
    bool ok = true;
    std::string type = attrs.getOptStringReporting(SUMO_ATTR_TYPE, 0, 0, ok, "induct_loop");
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
    bool ok = true;
    SUMOReal position = attrs.getSUMORealReporting(SUMO_ATTR_POSITION, "e4-detector", id.c_str(), ok);
    bool friendlyPos = attrs.getOptBoolReporting(SUMO_ATTR_FRIENDLY_POS, "e4-detector", id.c_str(), ok, false);
    std::string lane = attrs.getStringReporting(SUMO_ATTR_LANE, "e4-detector", id.c_str(), ok);
    std::string msg = attrs.getStringReporting(SUMO_ATTR_MSG, "e4-detector", id.c_str(), ok);
    std::string file = attrs.getStringReporting(SUMO_ATTR_FILE, "e4-detector", id.c_str(), ok);
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
    if (!attrs.setIDFromAttributes("e1-detector", id)) {
        return;
    }
    // inform the user about deprecated values
    bool ok = true;
    if (attrs.getOptStringReporting(SUMO_ATTR_STYLE, "e1-detector", id.c_str(), ok, "<invalid>")!="<invalid>") {
        MsgHandler::getWarningInstance()->inform("While parsing E1-detector '" + id + "': 'style' is deprecated.");
    }
    int frequency = attrs.getIntReporting(SUMO_ATTR_FREQUENCY, "e1-detector", id.c_str(), ok);
    SUMOReal position = attrs.getSUMORealReporting(SUMO_ATTR_POSITION, "e1-detector", id.c_str(), ok);
    bool friendlyPos = attrs.getOptBoolReporting(SUMO_ATTR_FRIENDLY_POS, "e1-detector", id.c_str(), ok, false);
    std::string lane = attrs.getStringReporting(SUMO_ATTR_LANE, "e1-detector", id.c_str(), ok);
    std::string file = attrs.getStringReporting(SUMO_ATTR_FILE, "e1-detector", id.c_str(), ok);
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
    if (!attrs.setIDFromAttributes("vtypeprobe", id)) {
        return;
    }
    bool ok = true;
    int frequency = attrs.getIntReporting(SUMO_ATTR_FREQUENCY, "vtypeprobe", id.c_str(), ok);
    std::string type = attrs.getStringSecure(SUMO_ATTR_TYPE, "");
    std::string file = attrs.getStringReporting(SUMO_ATTR_FILE, "vtypeprobe", id.c_str(), ok);
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
    if (!attrs.setIDFromAttributes("routeprobe", id)) {
        return;
    }
    bool ok = true;
    int frequency = attrs.getIntReporting(SUMO_ATTR_FREQUENCY, "routeprobe", id.c_str(), ok);
    int begin = attrs.getOptIntReporting(SUMO_ATTR_BEGIN, "routeprobe", id.c_str(), ok, -1);
    std::string edge = attrs.getStringReporting(SUMO_ATTR_EDGE, "routeprobe", id.c_str(), ok);
    std::string file = attrs.getStringReporting(SUMO_ATTR_FILE, "routeprobe", id.c_str(), ok);
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
    if (!attrs.setIDFromAttributes("e2-detector", id)) {
        return;
    }
    // check whether this is a detector connected to a tls an optionally to a link
    bool ok = true;
    std::string lsaid = attrs.getOptStringReporting(SUMO_ATTR_TLID, "e2-detector", id.c_str(), ok, "<invalid>");
    std::string toLane = attrs.getOptStringReporting(SUMO_ATTR_TO, "e2-detector", id.c_str(), ok, "<invalid>");
    // inform the user about deprecated values
    if (attrs.getOptStringReporting(SUMO_ATTR_MEASURES, "e2-detector", id.c_str(), ok, "<invalid>")!="<invalid>") {
        MsgHandler::getWarningInstance()->inform("While parsing E2-detector '" + id + "': 'measures' is deprecated.");
    }
    if (attrs.getOptStringReporting(SUMO_ATTR_STYLE, "e2-detector", id.c_str(), ok, "<invalid>")!="<invalid>") {
        MsgHandler::getWarningInstance()->inform("While parsing E2-detector '" + id + "': 'style' is deprecated.");
    }
    //
    try {
        SUMOReal haltingTimeThreshold = attrs.getOptSUMORealReporting(SUMO_ATTR_HALTING_TIME_THRESHOLD, "e2-detector", id.c_str(), ok, 1.0f);
        SUMOReal haltingSpeedThreshold = attrs.getOptSUMORealReporting(SUMO_ATTR_HALTING_SPEED_THRESHOLD, "e2-detector", id.c_str(), ok, 5.0f/3.6f);
        SUMOReal jamDistThreshold = attrs.getOptSUMORealReporting(SUMO_ATTR_JAM_DIST_THRESHOLD, "e2-detector", id.c_str(), ok, 10.0f);
        SUMOReal position = attrs.getSUMORealReporting(SUMO_ATTR_POSITION, "e2-detector", id.c_str(), ok);
        SUMOReal length = attrs.getSUMORealReporting(SUMO_ATTR_LENGTH, "e2-detector", id.c_str(), ok);
        bool friendlyPos = attrs.getOptBoolReporting(SUMO_ATTR_FRIENDLY_POS, "e2-detector", id.c_str(), ok, false);
        bool cont = attrs.getOptBoolReporting(SUMO_ATTR_CONT, "e2-detector", id.c_str(), ok, false);
        std::string lane = attrs.getStringReporting(SUMO_ATTR_LANE, "e2-detector", id.c_str(), ok);
        std::string file = attrs.getStringReporting(SUMO_ATTR_FILE, "e2-detector", id.c_str(), ok);
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
            int frequency = attrs.getIntReporting(SUMO_ATTR_FREQUENCY, "e2-detector", id.c_str(), ok);
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
    if (!attrs.setIDFromAttributes("e3-detector", id)) {
        return;
    }
    bool ok = true;
    // inform the user about deprecated values
    if (attrs.getOptStringReporting(SUMO_ATTR_MEASURES, "e3-detector", id.c_str(), ok, "<invalid>")!="<invalid>") {
        MsgHandler::getWarningInstance()->inform("While parsing E3-detector '" + id + "': 'measures' is deprecated.");
    }
    if (attrs.getOptStringReporting(SUMO_ATTR_STYLE, "e3-detector", id.c_str(), ok, "<invalid>")!="<invalid>") {
        MsgHandler::getWarningInstance()->inform("While parsing E3-detector '" + id + "': 'style' is deprecated.");
    }
    int frequency = attrs.getIntReporting(SUMO_ATTR_FREQUENCY, "e3-detector", id.c_str(), ok);
    SUMOReal haltingTimeThreshold = attrs.getOptSUMORealReporting(SUMO_ATTR_HALTING_TIME_THRESHOLD, "e3-detector", id.c_str(), ok, 1.0f);
    SUMOReal haltingSpeedThreshold = attrs.getOptSUMORealReporting(SUMO_ATTR_HALTING_SPEED_THRESHOLD, "e3-detector", id.c_str(), ok, 5.0f/3.6f);
    std::string file = attrs.getStringReporting(SUMO_ATTR_FILE, "e3-detector", id.c_str(), ok);
    if (!ok) {
        return;
    }
    try {
        myDetectorBuilder.beginE3Detector(id,
                                          OutputDevice::getDevice(file, getFileName()),
                                          frequency, haltingSpeedThreshold, haltingTimeThreshold);
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (BoolFormatException &) {
        MsgHandler::getErrorInstance()->inform("The description of the detector '" + id + "' contains a broken boolean.");
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("The description of the detector '" + id + "' contains a broken number.");
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("The description of the detector '" + id + "' does not contain a needed value.");
    } catch (IOError &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    }
}


void
NLHandler::addE3Entry(const SUMOSAXAttributes &attrs) {
    bool ok = true;
    SUMOReal position = attrs.getSUMORealReporting(SUMO_ATTR_POSITION, "e3-detector/entry", myDetectorBuilder.getCurrentE3ID().c_str(), ok);
    bool friendlyPos = attrs.getOptBoolReporting(SUMO_ATTR_FRIENDLY_POS, "e3-detector/entry", myDetectorBuilder.getCurrentE3ID().c_str(), ok, false);
    std::string lane = attrs.getStringReporting(SUMO_ATTR_LANE, "e3-detector/entry", myDetectorBuilder.getCurrentE3ID().c_str(), ok);
    if (!ok) {
        return;
    }
    myDetectorBuilder.addE3Entry(lane, position, friendlyPos);
}


void
NLHandler::addE3Exit(const SUMOSAXAttributes &attrs) {
    bool ok = true;
    SUMOReal position = attrs.getSUMORealReporting(SUMO_ATTR_POSITION, "e3-detector/exit", myDetectorBuilder.getCurrentE3ID().c_str(), ok);
    bool friendlyPos = attrs.getOptBoolReporting(SUMO_ATTR_FRIENDLY_POS, "e3-detector/exit", myDetectorBuilder.getCurrentE3ID().c_str(), ok, false);
    std::string lane = attrs.getStringReporting(SUMO_ATTR_LANE, "e3-detector/exit", myDetectorBuilder.getCurrentE3ID().c_str(), ok);
    if (!ok) {
        return;
    }
    myDetectorBuilder.addE3Exit(lane, position, friendlyPos);
}


void
NLHandler::addEdgeMeanData(const SUMOSAXAttributes &attrs) {
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes("meandata_edge", id)) {
        return;
    }
    bool ok = true;
    SUMOReal maxTravelTime = attrs.getOptSUMORealReporting(SUMO_ATTR_MAX_TRAVELTIME, "meandata_edge", id.c_str(), ok, 100000);
    SUMOReal minSamples = attrs.getOptSUMORealReporting(SUMO_ATTR_MIN_SAMPLES, "meandata_edge", id.c_str(), ok, 0);
    SUMOReal haltingSpeedThreshold = attrs.getOptSUMORealReporting(SUMO_ATTR_HALTING_SPEED_THRESHOLD, "meandata_edge", id.c_str(), ok, POSITION_EPS);
    bool excludeEmpty = attrs.getOptBoolReporting(SUMO_ATTR_EXCLUDE_EMPTY, "meandata_edge", id.c_str(), ok, false);
    bool withInternal = attrs.getOptBoolReporting(SUMO_ATTR_WITH_INTERNAL, "meandata_edge", id.c_str(), ok, false);
    std::string file = attrs.getStringReporting(SUMO_ATTR_FILE, "meandata_edge", id.c_str(), ok);
    std::string type = attrs.getOptStringReporting(SUMO_ATTR_TYPE, "meandata_edge", id.c_str(), ok, "performance");
    std::string vtypes = attrs.getOptStringReporting(SUMO_ATTR_VTYPES, "meandata_edge", id.c_str(), ok, "");
    if (!ok) {
        return;
    }
    try {
        myDetectorBuilder.createEdgeLaneMeanData(id, attrs.GET_XML_SUMO_TIME_SECURE(SUMO_ATTR_FREQUENCY, -1),
                attrs.GET_XML_SUMO_TIME_SECURE(SUMO_ATTR_BEGIN, OptionsCont::getOptions().getInt("begin")),
                attrs.GET_XML_SUMO_TIME_SECURE(SUMO_ATTR_END, OptionsCont::getOptions().getInt("end")),
                type, false, !excludeEmpty, withInternal,
                maxTravelTime, minSamples, haltingSpeedThreshold, vtypes,
                OutputDevice::getDevice(file, getFileName()));
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("The description of the meandata_edge '" + id + "' does not contain a needed value.");
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("The description of the meandata_edge '" + id + "' contains a broken number.");
    } catch (IOError &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    }
}


void
NLHandler::addLaneMeanData(const SUMOSAXAttributes &attrs) {
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes("meandata_lane", id)) {
        return;
    }
    bool ok = true;
    SUMOReal maxTravelTime = attrs.getOptSUMORealReporting(SUMO_ATTR_MAX_TRAVELTIME, "meandata_lane", id.c_str(), ok, 100000);
    SUMOReal minSamples = attrs.getOptSUMORealReporting(SUMO_ATTR_MIN_SAMPLES, "meandata_lane", id.c_str(), ok, 0);
    SUMOReal haltingSpeedThreshold = attrs.getOptSUMORealReporting(SUMO_ATTR_HALTING_SPEED_THRESHOLD, "meandata_lane", id.c_str(), ok, POSITION_EPS);
    bool excludeEmpty = attrs.getOptBoolReporting(SUMO_ATTR_EXCLUDE_EMPTY, "meandata_lane", id.c_str(), ok, false);
    bool withInternal = attrs.getOptBoolReporting(SUMO_ATTR_WITH_INTERNAL, "meandata_lane", id.c_str(), ok, false);
    std::string file = attrs.getStringReporting(SUMO_ATTR_FILE, "meandata_lane", id.c_str(), ok);
    std::string type = attrs.getOptStringReporting(SUMO_ATTR_TYPE, "meandata_lane", id.c_str(), ok, "performance");
    std::string vtypes = attrs.getOptStringReporting(SUMO_ATTR_VTYPES, "meandata_lane", id.c_str(), ok, "");
    if (!ok) {
        return;
    }
    try {
        myDetectorBuilder.createEdgeLaneMeanData(id, attrs.GET_XML_SUMO_TIME_SECURE(SUMO_ATTR_FREQUENCY, -1),
                attrs.GET_XML_SUMO_TIME_SECURE(SUMO_ATTR_BEGIN, OptionsCont::getOptions().getInt("begin")),
                attrs.GET_XML_SUMO_TIME_SECURE(SUMO_ATTR_END, OptionsCont::getOptions().getInt("end")),
                type, true, !excludeEmpty, withInternal,
                maxTravelTime, minSamples, haltingSpeedThreshold, vtypes,
                OutputDevice::getDevice(file, getFileName()));
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("The description of the meandata_lane '" + id + "' does not contain a needed value.");
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("The description of the meandata_lane '" + id + "' contains a broken number.");
    } catch (IOError &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    }
}





void
NLHandler::addSource(const SUMOSAXAttributes &attrs) {
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes("source", id)) {
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
    if (!attrs.setIDFromAttributes("trigger", id)) {
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
    std::string id = attrs.getStringReporting(SUMO_ATTR_LANE, "succ", 0, ok);
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
        SUMOReal pass = attrs.getOptSUMORealReporting(SUMO_ATTR_PASS, 0, 0, ok, -1);
        bool yield = attrs.getBoolReporting(SUMO_ATTR_YIELD, 0, 0, ok);
        bool internalEnd = attrs.getOptBoolReporting(SUMO_ATTR_YIELD, 0, 0, ok, false);
        std::string lane = attrs.getStringReporting(SUMO_ATTR_LANE, 0, 0, ok);
        std::string dir = attrs.getStringReporting(SUMO_ATTR_DIR, 0, 0, ok);
        std::string state = attrs.getStringReporting(SUMO_ATTR_STATE, 0, 0, ok);
        std::string tlID = attrs.getOptStringReporting(SUMO_ATTR_TLID, 0, 0, ok, "");
#ifdef HAVE_INTERNAL_LANES
        std::string via = attrs.getOptStringReporting(SUMO_ATTR_VIA, 0, 0, ok, "");
#endif
        if (!ok) {
            return;
        }
        if (tlID!="") {
            int linkNumber = attrs.getIntReporting(SUMO_ATTR_TLLINKNO, 0, 0, ok);
            if (!ok) {
                return;
            }
            mySucceedingLaneBuilder.addSuccLane(yield, lane,
#ifdef HAVE_INTERNAL_LANES
                                                via, pass,
#endif
                                                parseLinkDir(dir[0]), parseLinkState(state[0]), internalEnd, tlID, linkNumber);
        } else {
            mySucceedingLaneBuilder.addSuccLane(yield, lane,
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
NLHandler::setLaneNumber(const std::string &chars) {
    // @deprecated: assuming a net could still use characters for the lane number
    try {
        myJunctionControlBuilder.setLaneNumber(TplConvert<char>::_2int(chars.c_str()));
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Missing lane number.");
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("One of an edge's SUMOSAXAttributes must be numeric but is not.");
    }
}


void
NLHandler::setKey(const std::string &chars) {
    // @deprecated: assuming a net could still use characters for the id
    if (chars.length()==0) {
        MsgHandler::getErrorInstance()->inform("No key given for the current junction logic.");
        return;
    }
    myJunctionControlBuilder.setKey(chars);
}


void
NLHandler::setSubKey(const std::string &chars) {
    // @deprecated: assuming a net could still use characters for the sub id
    if (chars.length()==0) {
        MsgHandler::getErrorInstance()->inform("No subkey given for the current junction logic.");
        return;
    }
    myJunctionControlBuilder.setSubKey(chars);
}


void
NLHandler::setOffset(const std::string &chars) {
    // @deprecated: assuming a net could still use characters for the offset
    try {
        myJunctionControlBuilder.setOffset(TplConvertSec<char>::_2intSec(chars.c_str(), 0));
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("The offset for a junction is not numeric.");
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("The offset for a junction is not empty.");
    } // !!! can chars have length 0?
}


void
NLHandler::addJunctionShape(const std::string &chars) {
    // @deprecated: at some time, all junctions should have a shape attribute (moved from characters)
    if (!myHaveWarnedAboutDeprecatedJunctionShape) {
        myHaveWarnedAboutDeprecatedJunctionShape = true;
        MsgHandler::getWarningInstance()->inform("Your network uses a deprecated junction shape description; please rebuild.");
    }
    try {
        Position2DVector shape = GeomConvHelper::parseShape(chars);
        myJunctionControlBuilder.addJunctionShape(shape);
        return;
    } catch (OutOfBoundsException &) {
    } catch (NumberFormatException &) {
    } catch (EmptyData &) {
        return;
    }
    MsgHandler::getErrorInstance()->inform("Could not parse shape of junction '" + myJunctionControlBuilder.getActiveID() + "'.");
}


void
NLHandler::setLocation(const SUMOSAXAttributes &attrs) {
    bool ok = true;
    Position2DVector s;
    try {
        s = GeomConvHelper::parseShape(attrs.getStringReporting(SUMO_ATTR_NET_OFFSET, "net", 0, ok));
    } catch (OutOfBoundsException &) {
        MsgHandler::getErrorInstance()->inform("Invalid network offset.");
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("Invalid network offset.");
    }
    Position2D networkOffset = s[0];
    Boundary convBoundary;
    try {
        convBoundary = GeomConvHelper::parseBoundary(attrs.getStringReporting(SUMO_ATTR_CONV_BOUNDARY, "net", 0, ok));
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("Invalid converted network boundary.");
    }
    Boundary origBoundary;
    try {
        origBoundary = GeomConvHelper::parseBoundary(attrs.getStringReporting(SUMO_ATTR_ORIG_BOUNDARY, "net", 0, ok));
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("Invalid original network boundary.");
    }
    std::string proj = attrs.getStringReporting(SUMO_ATTR_ORIG_PROJ, "net", 0, ok);
    if (ok) {
        GeoConvHelper::init(proj, networkOffset, origBoundary, convBoundary);
    }
}

void
NLHandler::setNetOffset(const std::string &chars) {
    if (!myHaveWarnedAboutDeprecatedLocation) {
        myHaveWarnedAboutDeprecatedLocation = true;
        MsgHandler::getWarningInstance()->inform("Your network uses a deprecated network offset/projection definition.");
    }
    try {
        Position2DVector s = GeomConvHelper::parseShape(chars);
        myNetworkOffset = s[0];
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Invalid network offset.");
    } catch (OutOfBoundsException &) {
        MsgHandler::getErrorInstance()->inform("Invalid network offset.");
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("Invalid network offset.");
    }
}


void
NLHandler::setNetConv(const std::string &chars) {
    try {
        myConvBoundary = GeomConvHelper::parseBoundary(chars);
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Invalid network offset.");
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("Invalid network offset.");
    }
}


void
NLHandler::setNetOrig(const std::string &chars) {
    try {
        myOrigBoundary = GeomConvHelper::parseBoundary(chars);
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Invalid network offset.");
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("Invalid network offset.");
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


void
NLHandler::addPolyPosition(const std::string &chars) {
    try {
        myShapeBuilder.polygonEnd(GeomConvHelper::parseShape(chars));
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
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
