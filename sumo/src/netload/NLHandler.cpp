/****************************************************************************/
/// @file    NLHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// The XML-Handler for network loading
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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

#ifdef HAVE_MESOSIM
#include <mesosim/MELoop.h>
#endif

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
        myShapeBuilder(shapeBuilder), mySucceedingLaneBuilder(junctionBuilder, myContinuations),
        myAmInTLLogicMode(false), myCurrentIsBroken(false),
        myHaveWarnedAboutDeprecatedVClass(false),
        myHaveWarnedAboutDeprecatedJunctionShape(false),
        myHaveWarnedAboutDeprecatedLaneShape(false),
        myHaveWarnedAboutDeprecatedPolyShape(false),
        myHaveWarnedAboutDeprecatedLocation(false) {}


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
    std::string func;
    try {
        func = attrs.getString(SUMO_ATTR_FUNCTION);
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Missing function of an edge-object.");
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
        MSEdge *edge = myEdgeControlBuilder.closeEdge();
#ifdef HAVE_MESOSIM
        if (MSGlobals::gUseMesoSim) {
            MSGlobals::gMesoNet->buildSegmentsFor(edge, OptionsCont::getOptions());
        }
#endif
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
    try {
        myCurrentLaneID = id;
        myLaneIsDepart = attrs.getBool(SUMO_ATTR_DEPART);
        myCurrentMaxSpeed = attrs.getFloat(SUMO_ATTR_MAXSPEED);
        myCurrentLength = attrs.getFloat(SUMO_ATTR_LENGTH);
        myAllowedClasses.clear();
        myDisallowedClasses.clear();
        parseVehicleClasses(attrs.getStringSecure(SUMO_ATTR_VCLASSES , ""),
                            attrs.getStringSecure(SUMO_ATTR_ALLOW , ""),
                            attrs.getStringSecure(SUMO_ATTR_DISALLOW , ""),
                            myAllowedClasses, myDisallowedClasses, myHaveWarnedAboutDeprecatedVClass);
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Missing attribute in a lane-object (id='" + id + "').\n Can not build according edge.");
        myCurrentIsBroken = true;
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("One of a lane's attributes must be numeric but is not (id='" + id + "').\n Can not build according edge.");
        myCurrentIsBroken = true;
    } catch (BoolFormatException &) {
        MsgHandler::getErrorInstance()->inform("Value of depart definition of lane '" + id + "' is invalid.\n Can not build according edge.");
        myCurrentIsBroken = true;
    }
    if (!myCurrentIsBroken) {
        if (attrs.hasAttribute(SUMO_ATTR_SHAPE)) {
            addLaneShape(attrs.getString(SUMO_ATTR_SHAPE));
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
            shape = GeomConvHelper::parseShape(attrs.getString(SUMO_ATTR_SHAPE));
        } catch (OutOfBoundsException &) {
        } catch (NumberFormatException &) {
        } catch (EmptyData &) {
        }
    }
    try {
        myJunctionControlBuilder.openJunction(id,
                                              attrs.getStringSecure(SUMO_ATTR_KEY, ""),
                                              attrs.getString(SUMO_ATTR_TYPE),
                                              attrs.getFloat(SUMO_ATTR_X),
                                              attrs.getFloat(SUMO_ATTR_Y),
                                              shape);
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Missing attribute in junction '" + id + "'.\n Can not build according junction.");
        myCurrentIsBroken = true;
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what() + std::string("\n Can not build according junction."));
        myCurrentIsBroken = true;
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("Position of junction '" + id + "' is not numeric.\n Can not build according junction.");
        myCurrentIsBroken = true;
    }
    //
    if (!myCurrentIsBroken&&attrs.hasAttribute(SUMO_ATTR_INCLANES)) {
        addIncomingLanes(attrs.getString(SUMO_ATTR_INCLANES));
    }
#ifdef HAVE_INTERNAL_LANES
    if (!myCurrentIsBroken&&attrs.hasAttribute(SUMO_ATTR_INTLANES)) {
        addInternalLanes(attrs.getString(SUMO_ATTR_INTLANES));
    }
#endif
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
    std::string key, val;
    try {
        key = attrs.getString(SUMO_ATTR_KEY);
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Missing key for a parameter.");
        return;
    }
    try {
        val = attrs.getString(SUMO_ATTR_VALUE);
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Missing value for a parameter.");
        return;
    }
    // set
    if (myAmInTLLogicMode) {
        assert(key!="");
        assert(val!="");
        myJunctionControlBuilder.addParam(key, val);
    }
}


void
NLHandler::openWAUT(const SUMOSAXAttributes &attrs) {
    myCurrentIsBroken = false;
    SUMOTime t;
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes("waut", id)) {
        myCurrentIsBroken = true;
        return;
    }
    std::string pro;
    try {
        t = attrs.getIntSecure(SUMO_ATTR_REF_TIME, 0);
    } catch (NumberFormatException&) {
        MsgHandler::getErrorInstance()->inform("The reference time for WAUT '" + id + "' is not numeric.");
        myCurrentIsBroken = true;
    }
    try {
        pro = attrs.getString(SUMO_ATTR_START_PROG);
    } catch (EmptyData&) {
        MsgHandler::getErrorInstance()->inform("Missing start program for WAUT '" + id + "'.");
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
    SUMOTime t;
    std::string to;
    try {
        t = attrs.getInt(SUMO_ATTR_TIME);
    } catch (NumberFormatException&) {
        MsgHandler::getErrorInstance()->inform("The switch time for WAUT '" + myCurrentWAUTID + "' is not numeric.");
        myCurrentIsBroken = true;
    } catch (EmptyData&) {
        MsgHandler::getErrorInstance()->inform("Missing switch time for WAUT '" + myCurrentWAUTID + "'.");
        myCurrentIsBroken = true;
    }
    try {
        to = attrs.getString(SUMO_ATTR_TO);
    } catch (EmptyData&) {
        MsgHandler::getErrorInstance()->inform("Missing destination program for WAUT '" + myCurrentWAUTID + "'.");
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
    std::string wautID, junctionID, procedure;
    try {
        wautID = attrs.getString(SUMO_ATTR_WAUT_ID);
    } catch (EmptyData&) {
        MsgHandler::getErrorInstance()->inform("Missing WAUT id in wautJunction.");
        myCurrentIsBroken = true;
    }
    try {
        junctionID = attrs.getString(SUMO_ATTR_JUNCTION_ID);
    } catch (EmptyData&) {
        MsgHandler::getErrorInstance()->inform("Missing junction id in wautJunction.");
        myCurrentIsBroken = true;
    }
    procedure = attrs.getStringSecure(SUMO_ATTR_PROCEDURE, "");
    try {
        bool synchron = attrs.getBoolSecure(SUMO_ATTR_SYNCHRON, false);
        if (!myCurrentIsBroken) {
            myJunctionControlBuilder.getTLLogicControlToUse().addWAUTJunction(wautID, junctionID, procedure, synchron);
        }
    } catch (BoolFormatException &) {
        MsgHandler::getErrorInstance()->inform("The information whether WAUT '" + wautID + "' is uncontrolled is not a valid bool.");
        myCurrentIsBroken = true;
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
    try {
        myShapeBuilder.addPoint(id,
                                attrs.getIntSecure(SUMO_ATTR_LAYER, 1),
                                attrs.getStringSecure(SUMO_ATTR_TYPE, ""),
                                RGBColor::parseColor(attrs.getStringSecure(SUMO_ATTR_COLOR, "1,0,0")),
                                attrs.getFloatSecure(SUMO_ATTR_X, INVALID_POSITION),
                                attrs.getFloatSecure(SUMO_ATTR_Y, INVALID_POSITION),
                                attrs.getStringSecure(SUMO_ATTR_LANE, ""),
                                attrs.getFloatSecure(SUMO_ATTR_POSITION, INVALID_POSITION));
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (OutOfBoundsException &) {
        MsgHandler::getErrorInstance()->inform("Color definition of POI '" + id + "' seems to be broken.");
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("One of POI's '" + id + "' SUMOSAXAttributes should be numeric but is not.");
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("POI '" + id + "' misses an attribute.");
    }
}


void
NLHandler::addPoly(const SUMOSAXAttributes &attrs) {
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes("poly", id)) {
        return;
    }
    try {
        myShapeBuilder.polygonBegin(id,
                                    attrs.getIntSecure(SUMO_ATTR_LAYER, -1),
                                    attrs.getStringSecure(SUMO_ATTR_TYPE, ""),
                                    RGBColor::parseColor(attrs.getString(SUMO_ATTR_COLOR)),
                                    attrs.getBoolSecure(SUMO_ATTR_FILL, false));
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("The color of polygon '" + id + "' could not be parsed.");
    } catch (BoolFormatException &) {
        MsgHandler::getErrorInstance()->inform("The attribute 'fill' of polygon '" + id + "' is not a valid bool.");
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Polygon '" + id + "' misses an attribute.");
    }
    if (attrs.hasAttribute(SUMO_ATTR_SHAPE)) {
        // @deprecated; at some time, this is mandatory (no character usage)
        addPolyPosition(attrs.getString(SUMO_ATTR_SHAPE));
    } else if (!myHaveWarnedAboutDeprecatedPolyShape) {
        myHaveWarnedAboutDeprecatedPolyShape = true;
        MsgHandler::getWarningInstance()->inform("You use a deprecated polygon shape description; use attribute 'shape' instead.");
    }
}


void
NLHandler::addLogicItem(const SUMOSAXAttributes &attrs) {
    // parse the request
    int request = -1;
    try {
        request = attrs.getInt(SUMO_ATTR_REQUEST);
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Missing request key.");
        return;
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("One of the request keys is not numeric.");
        return;
    }
    // parse the response
    std::string response;
    try {
        response = attrs.getString(SUMO_ATTR_RESPONSE);
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Missing respond for a request");
        return;
    }
    // parse the internal links information (when wished)
    std::string foes;
    try {
        foes = attrs.getString(SUMO_ATTR_FOES);
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Missing foes for a request");
        return;
    }
    bool cont = false;
#ifdef HAVE_INTERNAL_LANES
    if (MSGlobals::gUsingInternalLanes) {
        try {
            cont = attrs.getBoolSecure(SUMO_ATTR_CONT, false);
        } catch (BoolFormatException &) {
            throw InvalidArgument("The definition whether a link is a cont-link is not a valid bool.");
        }
    }
#endif
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
    int requestSize = -1;
    try {
        requestSize = attrs.getInt(SUMO_ATTR_REQUESTSIZE);
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Missing request size in row-logic '" + id + "'.");
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("Request size in row-logic '" + id + "' is not numeric.");
    }
    int laneNumber = -1;
    try {
        laneNumber = attrs.getInt(SUMO_ATTR_LANENUMBER);
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Missing lane number in row-logic '" + id + "'.");
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("Lane number in row-logic '" + id + "' is not numeric.");
    }
    if (requestSize!=-1&&laneNumber!=-1) {
        myJunctionControlBuilder.initJunctionLogic(id, requestSize, laneNumber);
    }
}


void
NLHandler::initTrafficLightLogic(const SUMOSAXAttributes &attrs) {
    SUMOReal detectorOffset = -1;
    myJunctionControlBuilder.initIncomingLanes(); // @deprecated (is this still used?)
    try {
        std::string type = attrs.getString(SUMO_ATTR_TYPE);
        // get the detector offset
        try {
            detectorOffset = attrs.getFloatSecure(SUMO_ATTR_DET_OFFSET, -1);
        } catch (EmptyData&) {
            MsgHandler::getErrorInstance()->inform("A detector offset of a traffic light logic is empty.");
        } catch (NumberFormatException&) {
            MsgHandler::getErrorInstance()->inform("A detector offset of a traffic light logic is not numeric.");
            return;
        }
        //
        if (!attrs.hasAttribute(SUMO_ATTR_ID)) {
            // @deprecated: assuming a net could still use characters for the id
            myJunctionControlBuilder.initTrafficLightLogic("", "", type, 0, detectorOffset);
            return;
        }
        std::string id = attrs.getStringSecure(SUMO_ATTR_ID, "");
        if (id.length()==0) {
            MsgHandler::getErrorInstance()->inform("No id given for a tls.");
            return;
        }
        int offset = 0;
        try {
            offset = attrs.getFloatSecure(SUMO_ATTR_OFFSET, 0);
        } catch (EmptyData&) {
            MsgHandler::getErrorInstance()->inform("Offset of tls '" + id + "' is empty.");
        } catch (NumberFormatException&) {
            MsgHandler::getErrorInstance()->inform("Offset of tls '" + id + "' is not numeric.");
            return;
        }
        std::string programID = attrs.getStringSecure(SUMO_ATTR_PROGRAMID, "<unknown>");
        myJunctionControlBuilder.initTrafficLightLogic(id, programID, type, offset, detectorOffset);
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Missing traffic light type.");
        return;
    }
    myAmInTLLogicMode = true;
}


void
NLHandler::addPhase(const SUMOSAXAttributes &attrs) {
    // try to get the phase definition
    std::string state;
    std::string phase;
    std::string brakeMask;
    std::string yellowMask;
    if (attrs.hasAttribute(SUMO_ATTR_STATE)) {
        // ok, doing it the new way
        state = attrs.getString(SUMO_ATTR_STATE);
    } else {
        // old, deprecated definition
        try {
            phase = attrs.getString(SUMO_ATTR_PHASE);
        } catch (EmptyData &) {
            MsgHandler::getErrorInstance()->inform("Missing phase definition.");
            return;
        }
        try {
            brakeMask = attrs.getString(SUMO_ATTR_BRAKE);
        } catch (EmptyData &) {
            MsgHandler::getErrorInstance()->inform("Missing break definition.");
            return;
        }
        try {
            yellowMask = attrs.getString(SUMO_ATTR_YELLOW);
        } catch (EmptyData &) {
            MsgHandler::getErrorInstance()->inform("Missing yellow definition.");
            return;
        }
        // check
        if (phase.length()!=brakeMask.length()||phase.length()!=yellowMask.length()) {
            MsgHandler::getErrorInstance()->inform("Definition of traffic light is broken - descriptions have different lengths.");
            return;
        }
        // convert to new
        //  color, first
        state = MSPhaseDefinition::old2new(phase, brakeMask, yellowMask);
    }

    // try to get the phase duration
    int duration;
    try {
        duration = attrs.getInt(SUMO_ATTR_DURATION);
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Missing phase duration.");
        return;
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("The phase duration is not numeric.");
        return;
    }
    if (duration==0) {
        MsgHandler::getErrorInstance()->inform("Duration of tls-logic '" + myJunctionControlBuilder.getActiveKey() + "/" + myJunctionControlBuilder.getActiveSubKey() + "' is zero.");
        return;
    }
    // if the traffic light is an actuated traffic light, try to get
    //  the minimum and maximum durations
    int minDuration = duration;
    int maxDuration = duration;
    try {
        minDuration = attrs.getIntSecure(SUMO_ATTR_MINDURATION, -1);
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("The phase minimum duration is empty.");
        return;
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("The phase minimum duration is not numeric.");
        return;
    }
    try {
        maxDuration = attrs.getIntSecure(SUMO_ATTR_MAXDURATION, -1);
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("The phase maximum duration is empty.");
        return;
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("The phase maximum duration is not numeric.");
        return;
    }
    myJunctionControlBuilder.addPhase(duration, state, minDuration, maxDuration);
}


#ifdef _MESSAGES
void
NLHandler::addMsgEmitter(const SUMOSAXAttributes& attrs) {
    std::string id;
    try {
        id = attrs.getString(SUMO_ATTR_ID);
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Missing id of a message emitter object.");
        return;
    }
    std::string file = attrs.getStringSecure(SUMO_ATTR_FILE, "");
    // if no file given, use stdout
    if (file=="") {
        file = "-";
    }
    std::string whatemit;
    bool reverse = attrs.getBoolSecure(SUMO_ATTR_REVERSE, false);
    bool table = attrs.getBoolSecure(SUMO_ATTR_TABLE, true);
    bool xycoord = attrs.getBoolSecure(SUMO_ATTR_XY, false);
    SUMOReal step = (SUMOReal)attrs.getIntSecure(SUMO_ATTR_STEP, 1);
    try {
        whatemit = attrs.getString(SUMO_ATTR_EVENTS);
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Missing emit_msg of a message emitter object with id '" + id + "'.");
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
    std::string type = attrs.getStringSecure(SUMO_ATTR_TYPE, "induct_loop");
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
    std::string id = attrs.getStringSecure(SUMO_ATTR_ID, "");
    if (id=="") {
        MsgHandler::getErrorInstance()->inform("Missing id of a e4-detector-object.");
        return;
    }
    std::string file = attrs.getStringSecure(SUMO_ATTR_FILE, "");
    if (file=="") {
        MsgHandler::getErrorInstance()->inform("Missing output definition for detector '" + id + "'.");
        return;
    }
    std::string msg = attrs.getStringSecure(SUMO_ATTR_MSG, "");
    if (msg=="") {
        MsgHandler::getErrorInstance()->inform("Missing message for detector '" + id + "'.");
        return;
    }
    try {
        myDetectorBuilder.buildMsgDetector(id,
                                           attrs.getString(SUMO_ATTR_LANE),
                                           attrs.getFloat(SUMO_ATTR_POSITION),
                                           //attrs.getInt(SUMO_ATTR_FREQUENCY),
                                           1,
                                           attrs.getString(SUMO_ATTR_MSG),
                                           OutputDevice::getDevice(attrs.getString(SUMO_ATTR_FILE), getFileName()),
                                           attrs.getBoolSecure(SUMO_ATTR_FRIENDLY_POS, false));
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("The description of the detector '" + id + "' does not contain a needed value.");
    } catch (BoolFormatException &) {
        MsgHandler::getErrorInstance()->inform("The description of the detector '" + id + "' contains a broken boolean.");
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("The description of the detector '" + id + "' contains a broken number.");
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
    std::string file = attrs.getStringSecure(SUMO_ATTR_FILE, "");
    if (file=="") {
        MsgHandler::getErrorInstance()->inform("Missing output definition for detector '" + id + "'.");
        return;
    }
    // inform the user about deprecated values
    if (attrs.getStringSecure(SUMO_ATTR_STYLE, "<invalid>")!="<invalid>") {
        MsgHandler::getWarningInstance()->inform("While parsing E1-detector '" + id + "': 'style' is deprecated.");
    }
    try {
        myDetectorBuilder.buildInductLoop(id,
                                          attrs.getString(SUMO_ATTR_LANE),
                                          attrs.getFloat(SUMO_ATTR_POSITION),
                                          attrs.getInt(SUMO_ATTR_FREQUENCY),
                                          OutputDevice::getDevice(attrs.getString(SUMO_ATTR_FILE), getFileName()),
                                          attrs.getBoolSecure(SUMO_ATTR_FRIENDLY_POS, false));
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("The description of the detector '" + id + "' does not contain a needed value.");
    } catch (BoolFormatException &) {
        MsgHandler::getErrorInstance()->inform("The description of the detector '" + id + "' contains a broken boolean.");
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("The description of the detector '" + id + "' contains a broken number.");
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
    std::string file = attrs.getStringSecure(SUMO_ATTR_FILE, "");
    if (file=="") {
        MsgHandler::getErrorInstance()->inform("Missing output definition for vtypeprobe '" + id + "'.");
        return;
    }
    try {
        myDetectorBuilder.buildVTypeProbe(id,
                                          attrs.getStringSecure(SUMO_ATTR_TYPE, ""),
                                          attrs.getInt(SUMO_ATTR_FREQUENCY),
                                          OutputDevice::getDevice(attrs.getString(SUMO_ATTR_FILE), getFileName()));
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("The description of the vtypeprobe '" + id + "' does not contain a needed value.");
    } catch (BoolFormatException &) {
        MsgHandler::getErrorInstance()->inform("The description of the vtypeprobe '" + id + "' contains a broken boolean.");
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("The description of the vtypeprobe '" + id + "' contains a broken number.");
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
    std::string file = attrs.getStringSecure(SUMO_ATTR_FILE, "");
    if (file=="") {
        MsgHandler::getErrorInstance()->inform("Missing output definition for routeprobe '" + id + "'.");
        return;
    }
    try {
        myDetectorBuilder.buildRouteProbe(id,
                                          attrs.getString(SUMO_ATTR_EDGE),
                                          attrs.getInt(SUMO_ATTR_FREQUENCY),
                                          attrs.getIntSecure(SUMO_ATTR_BEGIN, -1),
                                          OutputDevice::getDevice(attrs.getString(SUMO_ATTR_FILE), getFileName()));
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("The description of the routeprobe '" + id + "' does not contain a needed value.");
    } catch (BoolFormatException &) {
        MsgHandler::getErrorInstance()->inform("The description of the routeprobe '" + id + "' contains a broken boolean.");
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("The description of the routeprobe '" + id + "' contains a broken number.");
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
    std::string lsaid = attrs.getStringSecure(SUMO_ATTR_TLID, "<invalid>");
    std::string toLane = attrs.getStringSecure(SUMO_ATTR_TO, "<invalid>");
    // get the file name; it should not be empty
    std::string file = attrs.getStringSecure(SUMO_ATTR_FILE, "");
    if (file=="") {
        MsgHandler::getErrorInstance()->inform("Missing output definition for detector '" + id + "'.");
        return;
    }
    // inform the user about deprecated values
    if (attrs.getStringSecure(SUMO_ATTR_MEASURES, "<invalid>")!="<invalid>") {
        MsgHandler::getWarningInstance()->inform("While parsing E2-detector '" + id + "': 'measures' is deprecated.");
    }
    if (attrs.getStringSecure(SUMO_ATTR_STYLE, "<invalid>")!="<invalid>") {
        MsgHandler::getWarningInstance()->inform("While parsing E2-detector '" + id + "': 'style' is deprecated.");
    }
    //
    try {
        if (lsaid!="<invalid>") {
            if (toLane=="<invalid>") {
                myDetectorBuilder.buildE2Detector(myContinuations,
                                                  id,
                                                  attrs.getString(SUMO_ATTR_LANE),
                                                  attrs.getFloat(SUMO_ATTR_POSITION),
                                                  attrs.getFloat(SUMO_ATTR_LENGTH),
                                                  attrs.getBoolSecure(SUMO_ATTR_CONT, false),
                                                  myJunctionControlBuilder.getTLLogic(lsaid),
                                                  OutputDevice::getDevice(file, getFileName()),
                                                  (SUMOTime) attrs.getFloatSecure(SUMO_ATTR_HALTING_TIME_THRESHOLD, 1.0f),
                                                  attrs.getFloatSecure(SUMO_ATTR_HALTING_SPEED_THRESHOLD, 5.0f/3.6f),
                                                  attrs.getFloatSecure(SUMO_ATTR_JAM_DIST_THRESHOLD, 10.0f),
                                                  attrs.getBoolSecure(SUMO_ATTR_FRIENDLY_POS, false)
                                                 );
            } else {
                myDetectorBuilder.buildE2Detector(myContinuations,
                                                  id,
                                                  attrs.getString(SUMO_ATTR_LANE),
                                                  attrs.getFloat(SUMO_ATTR_POSITION),
                                                  attrs.getFloat(SUMO_ATTR_LENGTH),
                                                  attrs.getBoolSecure(SUMO_ATTR_CONT, false),
                                                  myJunctionControlBuilder.getTLLogic(lsaid), toLane,
                                                  OutputDevice::getDevice(file, getFileName()),
                                                  (SUMOTime) attrs.getFloatSecure(SUMO_ATTR_HALTING_TIME_THRESHOLD, 1.0f),
                                                  attrs.getFloatSecure(SUMO_ATTR_HALTING_SPEED_THRESHOLD, 5.0f/3.6f),
                                                  attrs.getFloatSecure(SUMO_ATTR_JAM_DIST_THRESHOLD, 10.0f),
                                                  attrs.getBoolSecure(SUMO_ATTR_FRIENDLY_POS, false)
                                                 );
            }
        } else {
            myDetectorBuilder.buildE2Detector(myContinuations,
                                              id,
                                              attrs.getString(SUMO_ATTR_LANE),
                                              attrs.getFloat(SUMO_ATTR_POSITION),
                                              attrs.getFloat(SUMO_ATTR_LENGTH),
                                              attrs.getBoolSecure(SUMO_ATTR_CONT, false),
                                              attrs.getInt(SUMO_ATTR_FREQUENCY),
                                              OutputDevice::getDevice(file, getFileName()),
                                              (SUMOTime) attrs.getFloatSecure(SUMO_ATTR_HALTING_TIME_THRESHOLD, 1.0f),
                                              attrs.getFloatSecure(SUMO_ATTR_HALTING_SPEED_THRESHOLD, 5.0f/3.6f),
                                              attrs.getFloatSecure(SUMO_ATTR_JAM_DIST_THRESHOLD, 10.0f),
                                              attrs.getBoolSecure(SUMO_ATTR_FRIENDLY_POS, false)
                                             );
        }
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
NLHandler::beginE3Detector(const SUMOSAXAttributes &attrs) {
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes("e3-detector", id)) {
        return;
    }
    // get the file name; it should not be empty
    std::string file = attrs.getStringSecure(SUMO_ATTR_FILE, "");
    if (file=="") {
        MsgHandler::getErrorInstance()->inform("Missing output definition for detector '" + id + "'.");
        return;
    }
    // inform the user about deprecated values
    if (attrs.getStringSecure(SUMO_ATTR_MEASURES, "<invalid>")!="<invalid>") {
        MsgHandler::getWarningInstance()->inform("While parsing E3-detector '" + id + "': 'measures' is deprecated.");
    }
    if (attrs.getStringSecure(SUMO_ATTR_STYLE, "<invalid>")!="<invalid>") {
        MsgHandler::getWarningInstance()->inform("While parsing E3-detector '" + id + "': 'style' is deprecated.");
    }
    // build the detector
    try {
        myDetectorBuilder.beginE3Detector(id,
                                          OutputDevice::getDevice(file, getFileName()),
                                          attrs.getInt(SUMO_ATTR_FREQUENCY),
                                          attrs.getFloatSecure(SUMO_ATTR_HALTING_SPEED_THRESHOLD, 5.0f/3.6f),
                                          attrs.getFloatSecure(SUMO_ATTR_HALTING_TIME_THRESHOLD, 1.f));
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
    try {
        myDetectorBuilder.addE3Entry(
            attrs.getString(SUMO_ATTR_LANE),
            attrs.getFloat(SUMO_ATTR_POSITION),
            attrs.getBoolSecure(SUMO_ATTR_FRIENDLY_POS, false));
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("Position of an entry of detector '" + myDetectorBuilder.getCurrentE3ID() + "' is not numeric.");
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("The description of an entry of the detector '" + myDetectorBuilder.getCurrentE3ID() + "' does not contain a needed value.");
    }
}


void
NLHandler::addE3Exit(const SUMOSAXAttributes &attrs) {
    try {
        myDetectorBuilder.addE3Exit(
            attrs.getString(SUMO_ATTR_LANE),
            attrs.getFloat(SUMO_ATTR_POSITION),
            attrs.getBoolSecure(SUMO_ATTR_FRIENDLY_POS, false));
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("Position of an exit of detector '" + myDetectorBuilder.getCurrentE3ID() + "' is not numeric.");
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("The description of an exit of the detector '" + myDetectorBuilder.getCurrentE3ID() + "' does not contain a needed value.");
    }
}


void
NLHandler::addEdgeMeanData(const SUMOSAXAttributes &attrs) {
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes("meandata_edge", id)) {
        return;
    }
    std::string file = attrs.getStringSecure(SUMO_ATTR_FILE, "");
    if (file=="") {
        MsgHandler::getErrorInstance()->inform("Missing output definition for meandata_edge '" + id + "'.");
        return;
    }
    try {
        myDetectorBuilder.buildEdgeMeanData(id, attrs.GET_XML_SUMO_TIME_SECURE(SUMO_ATTR_FREQUENCY, -1),
                                            attrs.GET_XML_SUMO_TIME_SECURE(SUMO_ATTR_BEGIN, OptionsCont::getOptions().getInt("begin")),
                                            attrs.GET_XML_SUMO_TIME_SECURE(SUMO_ATTR_END, OptionsCont::getOptions().getInt("end")),
                                            attrs.getStringSecure(SUMO_ATTR_TYPE, "performance"),
                                            attrs.getStringSecure(SUMO_ATTR_EDGES, ""),
                                            attrs.getBoolSecure(SUMO_ATTR_EXCLUDE_EMPTY, false),
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
    std::string file = attrs.getStringSecure(SUMO_ATTR_FILE, "");
    if (file=="") {
        MsgHandler::getErrorInstance()->inform("Missing output definition for meandata_lane '" + id + "'.");
        return;
    }
    try {
        myDetectorBuilder.buildLaneMeanData(id, attrs.GET_XML_SUMO_TIME_SECURE(SUMO_ATTR_FREQUENCY, -1),
                                            attrs.GET_XML_SUMO_TIME_SECURE(SUMO_ATTR_BEGIN, OptionsCont::getOptions().getInt("begin")),
                                            attrs.GET_XML_SUMO_TIME_SECURE(SUMO_ATTR_END, OptionsCont::getOptions().getInt("end")),
                                            attrs.getStringSecure(SUMO_ATTR_TYPE, "performance"),
                                            attrs.getStringSecure(SUMO_ATTR_EDGES, ""),
                                            attrs.getBoolSecure(SUMO_ATTR_EXCLUDE_EMPTY, false),
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
    try {
        std::string id = attrs.getString(SUMO_ATTR_LANE);
        if (!MSGlobals::gUsingInternalLanes&&id[0]==':') {
            myCurrentIsInternalToSkip = true;
            return;
        }
        myCurrentIsInternalToSkip = false;
        mySucceedingLaneBuilder.openSuccLane(id);
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Missing id of a succ-object.");
    }
}

void
NLHandler::addSuccLane(const SUMOSAXAttributes &attrs) {
    // do not process internal lanes if not wished
    if (myCurrentIsInternalToSkip) {
        return;
    }
    try {
        std::string tlID = attrs.getStringSecure(SUMO_ATTR_TLID, "");
        if (tlID!="") {
            mySucceedingLaneBuilder.addSuccLane(
                attrs.getBool(SUMO_ATTR_YIELD),
                attrs.getString(SUMO_ATTR_LANE),
#ifdef HAVE_INTERNAL_LANES
                attrs.getStringSecure(SUMO_ATTR_VIA, ""),
                attrs.getFloatSecure(SUMO_ATTR_PASS, -1),
#endif
                parseLinkDir(attrs.getString(SUMO_ATTR_DIR)[0]),
                parseLinkState(attrs.getString(SUMO_ATTR_STATE)[0]),
                attrs.getBoolSecure(SUMO_ATTR_INTERNALEND, false),
                tlID, attrs.getInt(SUMO_ATTR_TLLINKNO));
        } else {
            mySucceedingLaneBuilder.addSuccLane(
                attrs.getBool(SUMO_ATTR_YIELD),
                attrs.getString(SUMO_ATTR_LANE),
#ifdef HAVE_INTERNAL_LANES
                attrs.getStringSecure(SUMO_ATTR_VIA, ""),
                attrs.getFloatSecure(SUMO_ATTR_PASS, -1),
#endif
                parseLinkDir(attrs.getString(SUMO_ATTR_DIR)[0]),
                parseLinkState(attrs.getString(SUMO_ATTR_STATE)[0]),
                attrs.getBoolSecure(SUMO_ATTR_INTERNALEND, false));
        }
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Missing attribute in a succlane-object of lane '" + mySucceedingLaneBuilder.getCurrentLaneName() + "'.");
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("Something is wrong with the definition of a link of lane '" + mySucceedingLaneBuilder.getCurrentLaneName() + "'.");
    } catch (BoolFormatException &) {
        MsgHandler::getErrorInstance()->inform("Something is wrong with the definition of a link of lane '" + mySucceedingLaneBuilder.getCurrentLaneName() + "'.");
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
    Position2D networkOffset;
    try {
        Position2DVector s = GeomConvHelper::parseShape(attrs.getString(SUMO_ATTR_NET_OFFSET));
        networkOffset = s[0];
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Invalid network offset.");
    } catch (OutOfBoundsException &) {
        MsgHandler::getErrorInstance()->inform("Invalid network offset.");
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("Invalid network offset.");
    }
    Boundary convBoundary;
    try {
        convBoundary = GeomConvHelper::parseBoundary(attrs.getString(SUMO_ATTR_CONV_BOUNDARY));
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Invalid converted network boundary.");
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("Invalid converted network boundary.");
    }
    Boundary origBoundary;
    try {
        origBoundary = GeomConvHelper::parseBoundary(attrs.getString(SUMO_ATTR_ORIG_BOUNDARY));
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Invalid original network boundary.");
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("Invalid original network boundary.");
    }
    try {
        GeoConvHelper::init(attrs.getString(SUMO_ATTR_ORIG_PROJ), networkOffset, origBoundary, convBoundary);
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Invalid projection description.");
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


const MSEdgeContinuations &
NLHandler::getContinuations() const {
    return myContinuations;
}



/****************************************************************************/
