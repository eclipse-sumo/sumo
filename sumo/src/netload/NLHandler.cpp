/****************************************************************************/
/// @file    NLHandler.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Clemens Honomichl
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Felix Brack
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// The XML-Handler for network loading
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
#include <utils/common/StringTokenizer.h>
#include <utils/common/RGBColor.h>
#include <utils/geom/GeomConvHelper.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSLane.h>
#include <microsim/MSBitSetLogic.h>
#include <microsim/MSJunctionLogic.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/traffic_lights/MSAgentbasedTrafficLightLogic.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/UtilExceptions.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/shapes/ShapeContainer.h>
#include <utils/shapes/Shape.h>
#include <utils/gui/globjects/GUIGlObject.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NLHandler::NLHandler(const std::string& file, MSNet& net,
                     NLDetectorBuilder& detBuilder,
                     NLTriggerBuilder& triggerBuilder,
                     NLEdgeControlBuilder& edgeBuilder,
                     NLJunctionControlBuilder& junctionBuilder)
    : MSRouteHandler(file, true),
      myNet(net), myActionBuilder(net),
      myCurrentIsInternalToSkip(false),
      myDetectorBuilder(detBuilder), myTriggerBuilder(triggerBuilder),
      myEdgeControlBuilder(edgeBuilder), myJunctionControlBuilder(junctionBuilder),
      myAmInTLLogicMode(false), myCurrentIsBroken(false),
      myHaveWarnedAboutDeprecatedLanes(false),
      myLastParameterised(0),
      myHaveSeenInternalEdge(false) {}


NLHandler::~NLHandler() {}


void
NLHandler::myStartElement(int element,
                          const SUMOSAXAttributes& attrs) {
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
                initJunctionLogic(attrs);
                break;
            case SUMO_TAG_PHASE:
                addPhase(attrs);
                break;
            case SUMO_TAG_CONNECTION:
                addConnection(attrs);
                break;
            case SUMO_TAG_TLLOGIC:
                initTrafficLightLogic(attrs);
                break;
            case SUMO_TAG_REQUEST:
                addRequest(attrs);
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
            case SUMO_TAG_E1DETECTOR:
            case SUMO_TAG_INDUCTION_LOOP:
                addE1Detector(attrs);
                break;
            case SUMO_TAG_E2DETECTOR:
            case SUMO_TAG_LANE_AREA_DETECTOR:
                addE2Detector(attrs);
                break;
            case SUMO_TAG_E3DETECTOR:
            case SUMO_TAG_ENTRY_EXIT_DETECTOR:
                beginE3Detector(attrs);
                break;
            case SUMO_TAG_DET_ENTRY:
                addE3Entry(attrs);
                break;
            case SUMO_TAG_DET_EXIT:
                addE3Exit(attrs);
                break;
            case SUMO_TAG_INSTANT_INDUCTION_LOOP:
                addInstantE1Detector(attrs);
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
                addEdgeLaneMeanData(attrs, SUMO_TAG_MEANDATA_EDGE);
                break;
            case SUMO_TAG_MEANDATA_LANE:
                addEdgeLaneMeanData(attrs, SUMO_TAG_MEANDATA_LANE);
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
            case SUMO_TAG_TAZ:
                addDistrict(attrs);
                break;
            case SUMO_TAG_TAZSOURCE:
                addDistrictEdge(attrs, true);
                break;
            case SUMO_TAG_TAZSINK:
                addDistrictEdge(attrs, false);
                break;
            case SUMO_TAG_ROUNDABOUT:
                addRoundabout(attrs);
                break;
            default:
                break;
        }
    } catch (InvalidArgument& e) {
        WRITE_ERROR(e.what());
    }
    MSRouteHandler::myStartElement(element, attrs);
    if (element == SUMO_TAG_PARAM) {
        addParam(attrs);
    }
}


void
NLHandler::myEndElement(int element) {
    switch (element) {
        case SUMO_TAG_EDGE:
            closeEdge();
            break;
        case SUMO_TAG_JUNCTION:
            if (!myCurrentIsBroken) {
                try {
                    myJunctionControlBuilder.closeJunctionLogic();
                    myJunctionControlBuilder.closeJunction();
                } catch (InvalidArgument& e) {
                    WRITE_ERROR(e.what());
                }
            }
            break;
        case SUMO_TAG_TLLOGIC:
            try {
                myJunctionControlBuilder.closeTrafficLightLogic();
            } catch (InvalidArgument& e) {
                WRITE_ERROR(e.what());
            }
            myAmInTLLogicMode = false;
            break;
        case SUMO_TAG_WAUT:
            closeWAUT();
            break;
        case SUMO_TAG_E3DETECTOR:
        case SUMO_TAG_ENTRY_EXIT_DETECTOR:
            endE3Detector();
            break;
        case SUMO_TAG_NET:
            myJunctionControlBuilder.postLoadInitialization();
            break;
        default:
            break;
    }
    MSRouteHandler::myEndElement(element);
    if (element != SUMO_TAG_PARAM) {
        myLastParameterised = 0;
    }
}



// ---- the root/edge - element
void
NLHandler::beginEdgeParsing(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    myCurrentIsBroken = false;
    // get the id, report an error if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        myCurrentIsBroken = true;
        return;
    }
    // omit internal edges if not wished
    if (!MSGlobals::gUsingInternalLanes && id[0] == ':') {
        myHaveSeenInternalEdge = true;
        myCurrentIsInternalToSkip = true;
        return;
    }
    myCurrentIsInternalToSkip = false;
    // parse the function
    const SumoXMLEdgeFunc func = attrs.getEdgeFunc(ok);
    if (!ok) {
        WRITE_ERROR("Edge '" + id + "' has an invalid type.");
        myCurrentIsBroken = true;
        return;
    }
    // interpret the function
    MSEdge::EdgeBasicFunction funcEnum = MSEdge::EDGEFUNCTION_UNKNOWN;
    switch (func) {
        case EDGEFUNC_NORMAL:
            funcEnum = MSEdge::EDGEFUNCTION_NORMAL;
            break;
        case EDGEFUNC_CONNECTOR:
        case EDGEFUNC_SINK:
        case EDGEFUNC_SOURCE:
            funcEnum = MSEdge::EDGEFUNCTION_CONNECTOR;
            break;
        case EDGEFUNC_INTERNAL:
            funcEnum = MSEdge::EDGEFUNCTION_INTERNAL;
            break;
    }
    // get the street name
    std::string streetName = attrs.getOpt<std::string>(SUMO_ATTR_NAME, id.c_str(), ok, "");
    if (!ok) {
        myCurrentIsBroken = true;
        return;
    }
    //
    try {
        myEdgeControlBuilder.beginEdgeParsing(id, funcEnum, streetName);
    } catch (InvalidArgument& e) {
        WRITE_ERROR(e.what());
        myCurrentIsBroken = true;
    }
}


void
NLHandler::closeEdge() {
    // omit internal edges if not wished and broken edges
    if (myCurrentIsInternalToSkip || myCurrentIsBroken) {
        return;
    }
    try {
        MSEdge* e = myEdgeControlBuilder.closeEdge();
        MSEdge::dictionary(e->getID(), e);
    } catch (InvalidArgument& e) {
        WRITE_ERROR(e.what());
    }
}


//             ---- the root/edge/lanes/lane - element
void
NLHandler::addLane(const SUMOSAXAttributes& attrs) {
    // omit internal edges if not wished and broken edges
    if (myCurrentIsInternalToSkip || myCurrentIsBroken) {
        return;
    }
    bool ok = true;
    // get the id, report an error if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        myCurrentIsBroken = true;
        return;
    }
    SUMOReal maxSpeed = attrs.get<SUMOReal>(SUMO_ATTR_SPEED, id.c_str(), ok);
    SUMOReal length = attrs.get<SUMOReal>(SUMO_ATTR_LENGTH, id.c_str(), ok);
    std::string allow;
    try {
        bool dummy;
        allow = attrs.getOpt<std::string>(SUMO_ATTR_ALLOW, id.c_str(), dummy, "", false);
    } catch (EmptyData e) {
        // !!! deprecated
    }
    std::string disallow = attrs.getOpt<std::string>(SUMO_ATTR_DISALLOW, id.c_str(), ok, "");
    SUMOReal width = attrs.getOpt<SUMOReal>(SUMO_ATTR_WIDTH, id.c_str(), ok, SUMO_const_laneWidth);
    PositionVector shape = attrs.get<PositionVector>(SUMO_ATTR_SHAPE, id.c_str(), ok);
    if (shape.size() < 2) {
        WRITE_ERROR("Shape of lane '" + id + "' is broken.\n Can not build according edge.");
        myCurrentIsBroken = true;
        return;
    }
    SVCPermissions permissions = parseVehicleClasses(allow, disallow);
    myCurrentIsBroken |= !ok;
    if (!myCurrentIsBroken) {
        try {
            MSLane* lane = myEdgeControlBuilder.addLane(id, maxSpeed, length, shape, width, permissions);
            // insert the lane into the lane-dictionary, checking
            if (!MSLane::dictionary(id, lane)) {
                delete lane;
                WRITE_ERROR("Another lane with the id '" + id + "' exists.");
                myCurrentIsBroken = true;
            }
            myLastParameterised = lane;
        } catch (InvalidArgument& e) {
            WRITE_ERROR(e.what());
        }
    }
}


// ---- the root/junction - element
void
NLHandler::openJunction(const SUMOSAXAttributes& attrs) {
    myCurrentIsBroken = false;
    bool ok = true;
    // get the id, report an error if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        myCurrentIsBroken = true;
        return;
    }
    PositionVector shape;
    if (attrs.hasAttribute(SUMO_ATTR_SHAPE)) {
        // inner junctions have no shape
        shape = attrs.getOpt<PositionVector>(SUMO_ATTR_SHAPE, id.c_str(), ok, PositionVector());
        if (shape.size() > 2) {
            shape.closePolygon();
        }
    }
    SUMOReal x = attrs.get<SUMOReal>(SUMO_ATTR_X, id.c_str(), ok);
    SUMOReal y = attrs.get<SUMOReal>(SUMO_ATTR_Y, id.c_str(), ok);
    bool typeOK = true;
    SumoXMLNodeType type = attrs.getNodeType(typeOK);
    if (!typeOK) {
        WRITE_ERROR("An unknown or invalid junction type occured in junction '" + id + "'.");
        ok = false;
    }
    std::string key = attrs.getOpt<std::string>(SUMO_ATTR_KEY, id.c_str(), ok, "");
    // incoming lanes
    std::vector<MSLane*> incomingLanes;
    parseLanes(id, attrs.getStringSecure(SUMO_ATTR_INCLANES, ""), incomingLanes, ok);
    // internal lanes
    std::vector<MSLane*> internalLanes;
#ifdef HAVE_INTERNAL_LANES
    if (MSGlobals::gUsingInternalLanes) {
        parseLanes(id, attrs.getStringSecure(SUMO_ATTR_INTLANES, ""), internalLanes, ok);
    }
#endif
    if (!ok) {
        myCurrentIsBroken = true;
    } else {
        try {
            myJunctionControlBuilder.openJunction(id, key, type, x, y, shape, incomingLanes, internalLanes);
        } catch (InvalidArgument& e) {
            WRITE_ERROR(e.what() + std::string("\n Can not build according junction."));
            myCurrentIsBroken = true;
        }
    }
}


void
NLHandler::parseLanes(const std::string& junctionID,
                      const std::string& def, std::vector<MSLane*>& into, bool& ok) {
    StringTokenizer st(def);
    while (ok && st.hasNext()) {
        std::string laneID = st.next();
        MSLane* lane = MSLane::dictionary(laneID);
        if (!MSGlobals::gUsingInternalLanes && laneID[0] == ':') {
            continue;
        }
        if (lane == 0) {
            WRITE_ERROR("An unknown lane ('" + laneID + "') was tried to be set as incoming to junction '" + junctionID + "'.");
            ok = false;
            continue;
        }
        into.push_back(lane);
    }
}
// ----

void
NLHandler::addParam(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string key = attrs.get<std::string>(SUMO_ATTR_KEY, 0, ok);
    std::string val = attrs.get<std::string>(SUMO_ATTR_VALUE, 0, ok);
    if (myLastParameterised != 0) {
        myLastParameterised->addParameter(key, val);
    }
    // set
    if (ok && myAmInTLLogicMode) {
        assert(key != "");
        assert(val != "");
        myJunctionControlBuilder.addParam(key, val);
    }
}


void
NLHandler::openWAUT(const SUMOSAXAttributes& attrs) {
    myCurrentIsBroken = false;
    bool ok = true;
    // get the id, report an error if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        myCurrentIsBroken = true;
        return;
    }
    SUMOTime t = attrs.getOptSUMOTimeReporting(SUMO_ATTR_REF_TIME, id.c_str(), ok, 0);
    std::string pro = attrs.get<std::string>(SUMO_ATTR_START_PROG, id.c_str(), ok);
    if (!ok) {
        myCurrentIsBroken = true;
    }
    if (!myCurrentIsBroken) {
        myCurrentWAUTID = id;
        try {
            myJunctionControlBuilder.getTLLogicControlToUse().addWAUT(t, id, pro);
        } catch (InvalidArgument& e) {
            WRITE_ERROR(e.what());
            myCurrentIsBroken = true;
        }
    }
}


void
NLHandler::addWAUTSwitch(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    SUMOTime t = attrs.getSUMOTimeReporting(SUMO_ATTR_TIME, myCurrentWAUTID.c_str(), ok);
    std::string to = attrs.get<std::string>(SUMO_ATTR_TO, myCurrentWAUTID.c_str(), ok);
    if (!ok) {
        myCurrentIsBroken = true;
    }
    if (!myCurrentIsBroken) {
        try {
            myJunctionControlBuilder.getTLLogicControlToUse().addWAUTSwitch(myCurrentWAUTID, t, to);
        } catch (InvalidArgument& e) {
            WRITE_ERROR(e.what());
            myCurrentIsBroken = true;
        }
    }
}


void
NLHandler::addWAUTJunction(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string wautID = attrs.get<std::string>(SUMO_ATTR_WAUT_ID, 0, ok);
    std::string junctionID = attrs.get<std::string>(SUMO_ATTR_JUNCTION_ID, 0, ok);
    std::string procedure = attrs.getOpt<std::string>(SUMO_ATTR_PROCEDURE, 0, ok, "");
    bool synchron = attrs.getOpt<bool>(SUMO_ATTR_SYNCHRON, 0, ok, false);
    if (!ok) {
        myCurrentIsBroken = true;
    }
    try {
        if (!myCurrentIsBroken) {
            myJunctionControlBuilder.getTLLogicControlToUse().addWAUTJunction(wautID, junctionID, procedure, synchron);
        }
    } catch (InvalidArgument& e) {
        WRITE_ERROR(e.what());
        myCurrentIsBroken = true;
    }
}







void
NLHandler::addPOI(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    const SUMOReal INVALID_POSITION(-1000000);
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    SUMOReal x = attrs.getOpt<SUMOReal>(SUMO_ATTR_X, id.c_str(), ok, INVALID_POSITION);
    SUMOReal y = attrs.getOpt<SUMOReal>(SUMO_ATTR_Y, id.c_str(), ok, INVALID_POSITION);
    SUMOReal lon = attrs.getOpt<SUMOReal>(SUMO_ATTR_LON, id.c_str(), ok, INVALID_POSITION);
    SUMOReal lat = attrs.getOpt<SUMOReal>(SUMO_ATTR_LAT, id.c_str(), ok, INVALID_POSITION);
    SUMOReal lanePos = attrs.getOpt<SUMOReal>(SUMO_ATTR_POSITION, id.c_str(), ok, INVALID_POSITION);
    SUMOReal layer = attrs.getOpt<SUMOReal>(SUMO_ATTR_LAYER, id.c_str(), ok, (SUMOReal)GLO_POI);
    std::string type = attrs.getOpt<std::string>(SUMO_ATTR_TYPE, id.c_str(), ok, "");
    std::string laneID = attrs.getOpt<std::string>(SUMO_ATTR_LANE, id.c_str(), ok, "");
    RGBColor color = attrs.hasAttribute(SUMO_ATTR_COLOR) ? attrs.get<RGBColor>(SUMO_ATTR_COLOR, id.c_str(), ok) : RGBColor::RED;
    SUMOReal angle = attrs.getOpt<SUMOReal>(SUMO_ATTR_ANGLE, id.c_str(), ok, Shape::DEFAULT_ANGLE);
    std::string imgFile = attrs.getOpt<std::string>(SUMO_ATTR_IMGFILE, id.c_str(), ok, Shape::DEFAULT_IMG_FILE);
    if (imgFile != "" && !FileHelpers::isAbsolute(imgFile)) {
        imgFile = FileHelpers::getConfigurationRelative(getFileName(), imgFile);
    }
    SUMOReal width = attrs.getOpt<SUMOReal>(SUMO_ATTR_WIDTH, id.c_str(), ok, Shape::DEFAULT_IMG_WIDTH);
    SUMOReal height = attrs.getOpt<SUMOReal>(SUMO_ATTR_HEIGHT, id.c_str(), ok, Shape::DEFAULT_IMG_HEIGHT);
    if (!ok) {
        return;
    }
    Position pos(x, y);
    if (x == INVALID_POSITION || y == INVALID_POSITION) {
        // try computing x,y from lane,pos
        if (laneID != "") {
            MSLane* lane = MSLane::dictionary(laneID);
            if (lane == 0) {
                WRITE_ERROR("Lane '" + laneID + "' to place poi '" + id + "' on is not known.");
                return;
            }
            if (lanePos < 0) {
                lanePos = lane->getLength() + lanePos;
            }
            pos = lane->geometryPositionAtOffset(lanePos);
        } else {
            // try computing x,y from lon,lat
            if (lat == INVALID_POSITION || lon == INVALID_POSITION) {
                WRITE_ERROR("Either (x,y), (lon,lat) or (lane,pos) must be specified for poi '" + id + "'.");
                return;
            } else if (!GeoConvHelper::getFinal().usingGeoProjection()) {
                WRITE_ERROR("(lon, lat) is specified for poi '" + id + "' but no geo-conversion is specified for the network.");
                return;
            }
            pos.set(lon, lat);
            GeoConvHelper::getFinal().x2cartesian_const(pos);
        }
    }
    if (!myNet.getShapeContainer().addPOI(id, type, color, layer, angle, imgFile, pos, width, height)) {
        WRITE_ERROR("PoI '" + id + "' already exists.");
    }
}


void
NLHandler::addPoly(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    // get the id, report an error if not given or empty...
    if (!ok) {
        return;
    }
    SUMOReal layer = attrs.getOpt<SUMOReal>(SUMO_ATTR_LAYER, id.c_str(), ok, Shape::DEFAULT_LAYER);
    bool fill = attrs.getOpt<bool>(SUMO_ATTR_FILL, id.c_str(), ok, false);
    std::string type = attrs.getOpt<std::string>(SUMO_ATTR_TYPE, id.c_str(), ok, Shape::DEFAULT_TYPE);
    std::string colorStr = attrs.get<std::string>(SUMO_ATTR_COLOR, id.c_str(), ok);
    RGBColor color = attrs.get<RGBColor>(SUMO_ATTR_COLOR, id.c_str(), ok);
    PositionVector shape = attrs.get<PositionVector>(SUMO_ATTR_SHAPE, id.c_str(), ok);
    SUMOReal angle = attrs.getOpt<SUMOReal>(SUMO_ATTR_ANGLE, id.c_str(), ok, Shape::DEFAULT_ANGLE);
    std::string imgFile = attrs.getOpt<std::string>(SUMO_ATTR_IMGFILE, id.c_str(), ok, Shape::DEFAULT_IMG_FILE);
    if (imgFile != "" && !FileHelpers::isAbsolute(imgFile)) {
        imgFile = FileHelpers::getConfigurationRelative(getFileName(), imgFile);
    }
    if (shape.size() != 0) {
        if (!myNet.getShapeContainer().addPolygon(id, type, color, layer, angle, imgFile, shape, fill)) {
            WRITE_WARNING("Skipping redefinition of polygon '" + id + "'.");
        }
    }
}


void
NLHandler::addRequest(const SUMOSAXAttributes& attrs) {
    if (myCurrentIsBroken) {
        return;
    }
    bool ok = true;
    int request = attrs.get<int>(SUMO_ATTR_INDEX, 0, ok);
    bool cont = false;
#ifdef HAVE_INTERNAL_LANES
    cont = attrs.getOpt<bool>(SUMO_ATTR_CONT, 0, ok, false);
#endif
    std::string response = attrs.get<std::string>(SUMO_ATTR_RESPONSE, 0, ok);
    std::string foes = attrs.get<std::string>(SUMO_ATTR_FOES, 0, ok);
    if (!ok) {
        return;
    }
    // store received information
    if (request >= 0 && response.length() > 0) {
        try {
            myJunctionControlBuilder.addLogicItem(request, response, foes, cont);
        } catch (InvalidArgument& e) {
            WRITE_ERROR(e.what());
        }
    }
}


void
NLHandler::initJunctionLogic(const SUMOSAXAttributes& attrs) {
    if (myCurrentIsBroken) {
        return;
    }
    bool ok = true;
    // we either a have a junction or a legacy network with ROWLogic
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (ok) {
        myJunctionControlBuilder.initJunctionLogic(id);
    }
}


void
NLHandler::initTrafficLightLogic(const SUMOSAXAttributes& attrs) {
    myAmInTLLogicMode = true;
    bool ok = true;
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    TrafficLightType type;
    std::string typeS = attrs.get<std::string>(SUMO_ATTR_TYPE, 0, ok);
    if (SUMOXMLDefinitions::TrafficLightTypes.hasString(typeS)) {
        type = SUMOXMLDefinitions::TrafficLightTypes.get(typeS);
    } else {
        WRITE_ERROR("Traffic light '" + id + "' has unknown type '" + typeS + "'");
        return;
    }
    //
    SUMOTime offset = attrs.getOptSUMOTimeReporting(SUMO_ATTR_OFFSET, id.c_str(), ok, 0);
    if (!ok) {
        return;
    }
    std::string programID = attrs.getOpt<std::string>(SUMO_ATTR_PROGRAMID, id.c_str(), ok, "<unknown>");
    myJunctionControlBuilder.initTrafficLightLogic(id, programID, type, offset);
}


void
NLHandler::addPhase(const SUMOSAXAttributes& attrs) {
    // try to get the phase definition
    bool ok = true;
    std::string state = attrs.get<std::string>(SUMO_ATTR_STATE, 0, ok);
    if (!ok) {
        return;
    }
    // try to get the phase duration
    SUMOTime duration = attrs.getSUMOTimeReporting(SUMO_ATTR_DURATION, myJunctionControlBuilder.getActiveKey().c_str(), ok);
    if (duration == 0) {
        WRITE_ERROR("Duration of tls-logic '" + myJunctionControlBuilder.getActiveKey() + "/" + myJunctionControlBuilder.getActiveSubKey() + "' is zero.");
        return;
    }
    // if the traffic light is an actuated traffic light, try to get
    //  the minimum and maximum durations
    SUMOTime minDuration = attrs.getOptSUMOTimeReporting(
                               SUMO_ATTR_MINDURATION, myJunctionControlBuilder.getActiveKey().c_str(), ok, duration);
    SUMOTime maxDuration = attrs.getOptSUMOTimeReporting(
                               SUMO_ATTR_MAXDURATION, myJunctionControlBuilder.getActiveKey().c_str(), ok, duration);
    myJunctionControlBuilder.addPhase(duration, state, minDuration, maxDuration);
}


void
NLHandler::addE1Detector(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    // get the id, report an error if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        return;
    }
    const SUMOTime frequency = attrs.getSUMOTimeReporting(SUMO_ATTR_FREQUENCY, id.c_str(), ok);
    const SUMOReal position = attrs.get<SUMOReal>(SUMO_ATTR_POSITION, id.c_str(), ok);
    const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, id.c_str(), ok, false);
    const bool splitByType = attrs.getOpt<bool>(SUMO_ATTR_SPLIT_VTYPE, id.c_str(), ok, false);
    const std::string lane = attrs.get<std::string>(SUMO_ATTR_LANE, id.c_str(), ok);
    const std::string file = attrs.get<std::string>(SUMO_ATTR_FILE, id.c_str(), ok);
    if (!ok) {
        return;
    }
    try {
        myDetectorBuilder.buildInductLoop(id, lane, position, frequency,
                                          FileHelpers::checkForRelativity(file, getFileName()),
                                          friendlyPos, splitByType);
    } catch (InvalidArgument& e) {
        WRITE_ERROR(e.what());
    } catch (IOError& e) {
        WRITE_ERROR(e.what());
    }
}


void
NLHandler::addInstantE1Detector(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    // get the id, report an error if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        return;
    }
    const SUMOReal position = attrs.get<SUMOReal>(SUMO_ATTR_POSITION, id.c_str(), ok);
    const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, id.c_str(), ok, false);
    const std::string lane = attrs.get<std::string>(SUMO_ATTR_LANE, id.c_str(), ok);
    const std::string file = attrs.get<std::string>(SUMO_ATTR_FILE, id.c_str(), ok);
    if (!ok) {
        return;
    }
    try {
        myDetectorBuilder.buildInstantInductLoop(id, lane, position, FileHelpers::checkForRelativity(file, getFileName()), friendlyPos);
    } catch (InvalidArgument& e) {
        WRITE_ERROR(e.what());
    } catch (IOError& e) {
        WRITE_ERROR(e.what());
    }
}


void
NLHandler::addVTypeProbeDetector(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    SUMOTime frequency = attrs.getSUMOTimeReporting(SUMO_ATTR_FREQUENCY, id.c_str(), ok);
    std::string type = attrs.getStringSecure(SUMO_ATTR_TYPE, "");
    std::string file = attrs.get<std::string>(SUMO_ATTR_FILE, id.c_str(), ok);
    if (!ok) {
        return;
    }
    try {
        myDetectorBuilder.buildVTypeProbe(id, type, frequency, FileHelpers::checkForRelativity(file, getFileName()));
    } catch (InvalidArgument& e) {
        WRITE_ERROR(e.what());
    } catch (IOError& e) {
        WRITE_ERROR(e.what());
    }
}


void
NLHandler::addRouteProbeDetector(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    SUMOTime frequency = attrs.getSUMOTimeReporting(SUMO_ATTR_FREQUENCY, id.c_str(), ok);
    SUMOTime begin = attrs.getOptSUMOTimeReporting(SUMO_ATTR_BEGIN, id.c_str(), ok, -1);
    std::string edge = attrs.get<std::string>(SUMO_ATTR_EDGE, id.c_str(), ok);
    std::string file = attrs.get<std::string>(SUMO_ATTR_FILE, id.c_str(), ok);
    if (!ok) {
        return;
    }
    try {
        myDetectorBuilder.buildRouteProbe(id, edge, frequency, begin,
                                          FileHelpers::checkForRelativity(file, getFileName()));
    } catch (InvalidArgument& e) {
        WRITE_ERROR(e.what());
    } catch (IOError& e) {
        WRITE_ERROR(e.what());
    }
}



void
NLHandler::addE2Detector(const SUMOSAXAttributes& attrs) {
    // check whether this is a detector connected to a tls an optionally to a link
    bool ok = true;
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    const std::string lsaid = attrs.getOpt<std::string>(SUMO_ATTR_TLID, id.c_str(), ok, "<invalid>");
    const std::string toLane = attrs.getOpt<std::string>(SUMO_ATTR_TO, id.c_str(), ok, "<invalid>");
    const SUMOTime haltingTimeThreshold = attrs.getOptSUMOTimeReporting(SUMO_ATTR_HALTING_TIME_THRESHOLD, id.c_str(), ok, TIME2STEPS(1));
    const SUMOReal haltingSpeedThreshold = attrs.getOpt<SUMOReal>(SUMO_ATTR_HALTING_SPEED_THRESHOLD, id.c_str(), ok, 5.0f / 3.6f);
    const SUMOReal jamDistThreshold = attrs.getOpt<SUMOReal>(SUMO_ATTR_JAM_DIST_THRESHOLD, id.c_str(), ok, 10.0f);
    const SUMOReal position = attrs.get<SUMOReal>(SUMO_ATTR_POSITION, id.c_str(), ok);
    const SUMOReal length = attrs.get<SUMOReal>(SUMO_ATTR_LENGTH, id.c_str(), ok);
    const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, id.c_str(), ok, false);
    const bool cont = attrs.getOpt<bool>(SUMO_ATTR_CONT, id.c_str(), ok, false);
    const std::string lane = attrs.get<std::string>(SUMO_ATTR_LANE, id.c_str(), ok);
    const std::string file = attrs.get<std::string>(SUMO_ATTR_FILE, id.c_str(), ok);
    if (!ok) {
        return;
    }
    try {
        if (lsaid != "<invalid>") {
            if (toLane == "<invalid>") {
                myDetectorBuilder.buildE2Detector(id, lane, position, length, cont,
                                                  myJunctionControlBuilder.getTLLogic(lsaid),
                                                  FileHelpers::checkForRelativity(file, getFileName()),
                                                  haltingTimeThreshold, haltingSpeedThreshold, jamDistThreshold,
                                                  friendlyPos);
            } else {
                myDetectorBuilder.buildE2Detector(id, lane, position, length, cont,
                                                  myJunctionControlBuilder.getTLLogic(lsaid), toLane,
                                                  FileHelpers::checkForRelativity(file, getFileName()),
                                                  haltingTimeThreshold, haltingSpeedThreshold, jamDistThreshold,
                                                  friendlyPos);
            }
        } else {
            bool ok = true;
            SUMOTime frequency = attrs.getSUMOTimeReporting(SUMO_ATTR_FREQUENCY, id.c_str(), ok);
            if (!ok) {
                return;
            }
            myDetectorBuilder.buildE2Detector(id, lane, position, length, cont, frequency,
                                              FileHelpers::checkForRelativity(file, getFileName()),
                                              haltingTimeThreshold, haltingSpeedThreshold, jamDistThreshold,
                                              friendlyPos);
        }
    } catch (InvalidArgument& e) {
        WRITE_ERROR(e.what());
    } catch (IOError& e) {
        WRITE_ERROR(e.what());
    }
}


void
NLHandler::beginE3Detector(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    const SUMOTime frequency = attrs.getSUMOTimeReporting(SUMO_ATTR_FREQUENCY, id.c_str(), ok);
    const SUMOTime haltingTimeThreshold = attrs.getOptSUMOTimeReporting(SUMO_ATTR_HALTING_TIME_THRESHOLD, id.c_str(), ok, TIME2STEPS(1));
    const SUMOReal haltingSpeedThreshold = attrs.getOpt<SUMOReal>(SUMO_ATTR_HALTING_SPEED_THRESHOLD, id.c_str(), ok, 5.0f / 3.6f);
    const std::string file = attrs.get<std::string>(SUMO_ATTR_FILE, id.c_str(), ok);
    if (!ok) {
        return;
    }
    try {
        myDetectorBuilder.beginE3Detector(id,
                                          FileHelpers::checkForRelativity(file, getFileName()),
                                          frequency, haltingSpeedThreshold, haltingTimeThreshold);
    } catch (InvalidArgument& e) {
        WRITE_ERROR(e.what());
    } catch (IOError& e) {
        WRITE_ERROR(e.what());
    }
}


void
NLHandler::addE3Entry(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    const SUMOReal position = attrs.get<SUMOReal>(SUMO_ATTR_POSITION, myDetectorBuilder.getCurrentE3ID().c_str(), ok);
    const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, myDetectorBuilder.getCurrentE3ID().c_str(), ok, false);
    const std::string lane = attrs.get<std::string>(SUMO_ATTR_LANE, myDetectorBuilder.getCurrentE3ID().c_str(), ok);
    if (!ok) {
        return;
    }
    myDetectorBuilder.addE3Entry(lane, position, friendlyPos);
}


void
NLHandler::addE3Exit(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    const SUMOReal position = attrs.get<SUMOReal>(SUMO_ATTR_POSITION, myDetectorBuilder.getCurrentE3ID().c_str(), ok);
    const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, myDetectorBuilder.getCurrentE3ID().c_str(), ok, false);
    const std::string lane = attrs.get<std::string>(SUMO_ATTR_LANE, myDetectorBuilder.getCurrentE3ID().c_str(), ok);
    if (!ok) {
        return;
    }
    myDetectorBuilder.addE3Exit(lane, position, friendlyPos);
}


void
NLHandler::addEdgeLaneMeanData(const SUMOSAXAttributes& attrs, int objecttype) {
    bool ok = true;
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    const SUMOReal maxTravelTime = attrs.getOpt<SUMOReal>(SUMO_ATTR_MAX_TRAVELTIME, id.c_str(), ok, 100000);
    const SUMOReal minSamples = attrs.getOpt<SUMOReal>(SUMO_ATTR_MIN_SAMPLES, id.c_str(), ok, 0);
    const SUMOReal haltingSpeedThreshold = attrs.getOpt<SUMOReal>(SUMO_ATTR_HALTING_SPEED_THRESHOLD, id.c_str(), ok, POSITION_EPS);
    const std::string excludeEmpty = attrs.getOpt<std::string>(SUMO_ATTR_EXCLUDE_EMPTY, id.c_str(), ok, "false");
    const bool withInternal = attrs.getOpt<bool>(SUMO_ATTR_WITH_INTERNAL, id.c_str(), ok, false);
    const bool trackVehicles = attrs.getOpt<bool>(SUMO_ATTR_TRACK_VEHICLES, id.c_str(), ok, false);
    const std::string file = attrs.get<std::string>(SUMO_ATTR_FILE, id.c_str(), ok);
    const std::string type = attrs.getOpt<std::string>(SUMO_ATTR_TYPE, id.c_str(), ok, "performance");
    std::string vtypes = attrs.getOpt<std::string>(SUMO_ATTR_VTYPES, id.c_str(), ok, "");
    const SUMOTime frequency = attrs.getOptSUMOTimeReporting(SUMO_ATTR_FREQUENCY, id.c_str(), ok, -1);
    const SUMOTime begin = attrs.getOptSUMOTimeReporting(SUMO_ATTR_BEGIN, id.c_str(), ok, string2time(OptionsCont::getOptions().getString("begin")));
    const SUMOTime end = attrs.getOptSUMOTimeReporting(SUMO_ATTR_END, id.c_str(), ok, string2time(OptionsCont::getOptions().getString("end")));
    if (!ok) {
        return;
    }
    try {
        myDetectorBuilder.createEdgeLaneMeanData(id, frequency, begin, end,
                type, objecttype == SUMO_TAG_MEANDATA_LANE,
                // equivalent to TplConvert::_2bool used in SUMOSAXAttributes::getBool
                excludeEmpty[0] != 't' && excludeEmpty[0] != 'T' && excludeEmpty[0] != '1' && excludeEmpty[0] != 'x',
                excludeEmpty == "defaults", withInternal, trackVehicles,
                maxTravelTime, minSamples, haltingSpeedThreshold, vtypes,
                FileHelpers::checkForRelativity(file, getFileName()));
    } catch (InvalidArgument& e) {
        WRITE_ERROR(e.what());
    } catch (IOError& e) {
        WRITE_ERROR(e.what());
    }
}


void
NLHandler::addConnection(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string fromID = attrs.get<std::string>(SUMO_ATTR_FROM, 0, ok);
    if (!MSGlobals::gUsingInternalLanes && fromID[0] == ':') {
        return;
    }

    try {
        bool ok = true;
        const std::string toID = attrs.get<std::string>(SUMO_ATTR_TO, 0, ok);
        const int fromLaneIdx = attrs.get<int>(SUMO_ATTR_FROM_LANE, 0, ok);
        const int toLaneIdx = attrs.get<int>(SUMO_ATTR_TO_LANE, 0, ok);
        LinkDirection dir = parseLinkDir(attrs.get<std::string>(SUMO_ATTR_DIR, 0, ok));
        LinkState state = parseLinkState(attrs.get<std::string>(SUMO_ATTR_STATE, 0, ok));
        std::string tlID = attrs.getOpt<std::string>(SUMO_ATTR_TLID, 0, ok, "");
#ifdef HAVE_INTERNAL_LANES
        std::string viaID = attrs.getOpt<std::string>(SUMO_ATTR_VIA, 0, ok, "");
#endif

        MSEdge* from = MSEdge::dictionary(fromID);
        if (from == 0) {
            WRITE_ERROR("Unknown from-edge '" + fromID + "' in connection");
            return;
        }
        MSEdge* to = MSEdge::dictionary(toID);
        if (to == 0) {
            WRITE_ERROR("Unknown to-edge '" + toID + "' in connection");
            return;
        }
        if (fromLaneIdx < 0 || static_cast<unsigned int>(fromLaneIdx) >= from->getLanes().size() ||
                toLaneIdx < 0 || static_cast<unsigned int>(toLaneIdx) >= to->getLanes().size()) {
            WRITE_ERROR("Invalid lane index in connection from '" + from->getID() + "' to '" + to->getID() + "'.");
            return;
        }
        MSLane* fromLane = from->getLanes()[fromLaneIdx];
        MSLane* toLane = to->getLanes()[toLaneIdx];
        assert(fromLane);
        assert(toLane);

        int tlLinkIdx = -1;
        if (tlID != "") {
            tlLinkIdx = attrs.get<int>(SUMO_ATTR_TLLINKINDEX, 0, ok);
            // make sure that the index is in range
            MSTrafficLightLogic* logic = myJunctionControlBuilder.getTLLogic(tlID).getActive();
            if (tlLinkIdx < 0 || tlLinkIdx >= (int)logic->getCurrentPhaseDef().getState().size()) {
                WRITE_ERROR("Invalid " + toString(SUMO_ATTR_TLLINKINDEX) + " '" + toString(tlLinkIdx) +
                            "' in connection controlled by '" + tlID + "'");
                return;
            }
            if (!ok) {
                return;
            }
        }
        SUMOReal length = fromLane->getShape()[-1].distanceTo(toLane->getShape()[0]);
        MSLink* link = 0;

        // build the link
#ifdef HAVE_INTERNAL_LANES
        MSLane* via = 0;
        if (viaID != "" && MSGlobals::gUsingInternalLanes) {
            via = MSLane::dictionary(viaID);
            if (via == 0) {
                WRITE_ERROR("An unknown lane ('" + viaID +
                            "') should be set as a via-lane for lane '" + toLane->getID() + "'.");
                return;
            }
            length = via->getLength();
        }
        link = new MSLink(toLane, via, dir, state, length);
        if (via != 0) {
            via->addIncomingLane(fromLane, link);
        } else {
            toLane->addIncomingLane(fromLane, link);
        }
#else
        link = new MSLink(toLane, dir, state, length);
        toLane->addIncomingLane(fromLane, link);
#endif
        toLane->addApproachingLane(fromLane);

        // if a traffic light is responsible for it, inform the traffic light
        // check whether this link is controlled by a traffic light
        if (tlID != "") {
            myJunctionControlBuilder.getTLLogic(tlID).addLink(link, fromLane, tlLinkIdx);
        }
        // add the link
        fromLane->addLink(link);

    } catch (InvalidArgument& e) {
        WRITE_ERROR(e.what());
    }
}


LinkDirection
NLHandler::parseLinkDir(const std::string& dir) {
    if (SUMOXMLDefinitions::LinkDirections.hasString(dir)) {
        return SUMOXMLDefinitions::LinkDirections.get(dir);
    } else {
        throw InvalidArgument("Unrecognised link direction '" + dir + "'.");
    }
}


LinkState
NLHandler::parseLinkState(const std::string& state) {
    if (SUMOXMLDefinitions::LinkStates.hasString(state)) {
        return SUMOXMLDefinitions::LinkStates.get(state);
    } else {
        if (state == "t") { // legacy networks
            // WRITE_WARNING("Obsolete link state 't'. Use 'o' instead");
            return LINKSTATE_TL_OFF_BLINKING;
        } else {
            throw InvalidArgument("Unrecognised link state '" + state + "'.");
        }
    }
}


// ----------------------------------
void
NLHandler::setLocation(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    PositionVector s = attrs.get<PositionVector>(SUMO_ATTR_NET_OFFSET, 0, ok);
    Boundary convBoundary = attrs.get<Boundary>(SUMO_ATTR_CONV_BOUNDARY, 0, ok);
    Boundary origBoundary = attrs.get<Boundary>(SUMO_ATTR_ORIG_BOUNDARY, 0, ok);
    std::string proj = attrs.get<std::string>(SUMO_ATTR_ORIG_PROJ, 0, ok);
    if (ok) {
        Position networkOffset = s[0];
        GeoConvHelper::init(proj, networkOffset, origBoundary, convBoundary);
        if (OptionsCont::getOptions().getBool("fcd-output.geo") && !GeoConvHelper::getFinal().usingGeoProjection()) {
            WRITE_WARNING("no valid geo projection loaded from network. fcd-output.geo will not work");
        }
    }
}


void
NLHandler::addDistrict(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    myCurrentIsBroken = false;
    // get the id, report an error if not given or empty...
    myCurrentDistrictID = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        myCurrentIsBroken = true;
        return;
    }
    try {
        MSEdge* sink = myEdgeControlBuilder.buildEdge(myCurrentDistrictID + "-sink", MSEdge::EDGEFUNCTION_DISTRICT);
        if (!MSEdge::dictionary(myCurrentDistrictID + "-sink", sink)) {
            delete sink;
            throw InvalidArgument("Another edge with the id '" + myCurrentDistrictID + "-sink' exists.");
        }
        sink->initialize(new std::vector<MSLane*>());
        MSEdge* source = myEdgeControlBuilder.buildEdge(myCurrentDistrictID + "-source", MSEdge::EDGEFUNCTION_DISTRICT);
        if (!MSEdge::dictionary(myCurrentDistrictID + "-source", source)) {
            delete source;
            throw InvalidArgument("Another edge with the id '" + myCurrentDistrictID + "-source' exists.");
        }
        source->initialize(new std::vector<MSLane*>());
        if (attrs.hasAttribute(SUMO_ATTR_EDGES)) {
            std::vector<std::string> desc = attrs.getStringVector(SUMO_ATTR_EDGES);
            for (std::vector<std::string>::const_iterator i = desc.begin(); i != desc.end(); ++i) {
                MSEdge* edge = MSEdge::dictionary(*i);
                // check whether the edge exists
                if (edge == 0) {
                    throw InvalidArgument("The edge '" + *i + "' within district '" + myCurrentDistrictID + "' is not known.");
                }
                source->addFollower(edge);
                edge->addFollower(sink);
            }
        }
        if (attrs.hasAttribute(SUMO_ATTR_SHAPE)) {
            PositionVector shape = attrs.get<PositionVector>(SUMO_ATTR_SHAPE, myCurrentDistrictID.c_str(), ok);
            if (shape.size() != 0) {
                if (!myNet.getShapeContainer().addPolygon(myCurrentDistrictID, "taz", RGBColor::parseColor("1.0,.33,.33"), 0, 0, "", shape, false)) {
                    WRITE_WARNING("Skipping visualization of taz '" + myCurrentDistrictID + "', polygon already exists.");
                }
            }
        }
    } catch (InvalidArgument& e) {
        WRITE_ERROR(e.what());
        myCurrentIsBroken = true;
    }
}


void
NLHandler::addDistrictEdge(const SUMOSAXAttributes& attrs, bool isSource) {
    if (myCurrentIsBroken) {
        // earlier error
        return;
    }
    bool ok = true;
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, myCurrentDistrictID.c_str(), ok);
    MSEdge* succ = MSEdge::dictionary(id);
    if (succ != 0) {
        // connect edge
        if (isSource) {
            MSEdge::dictionary(myCurrentDistrictID + "-source")->addFollower(succ);
        } else {
            succ->addFollower(MSEdge::dictionary(myCurrentDistrictID + "-sink"));
        }
    } else {
        WRITE_ERROR("At district '" + myCurrentDistrictID + "': succeeding edge '" + id + "' does not exist.");
    }
}


void
NLHandler::addRoundabout(const SUMOSAXAttributes& attrs) {
    if (attrs.hasAttribute(SUMO_ATTR_EDGES)) {
        std::vector<std::string> edgeIDs = attrs.getStringVector(SUMO_ATTR_EDGES);
        for (std::vector<std::string>::iterator it = edgeIDs.begin(); it != edgeIDs.end(); ++it) {
            MSEdge* edge = MSEdge::dictionary(*it);
            if (edge == 0) {
                WRITE_ERROR("Unknown edge '" + (*it) + "' in roundabout");
            } else {
                edge->markAsRoundabout();
            }
        }
    } else {
        WRITE_ERROR("Empty edges in roundabout.");
    }
}


// ----------------------------------
void
NLHandler::endE3Detector() {
    try {
        myDetectorBuilder.endE3Detector();
    } catch (InvalidArgument& e) {
        WRITE_ERROR(e.what());
    }
}


void
NLHandler::closeWAUT() {
    if (!myCurrentIsBroken) {
        try {
            myJunctionControlBuilder.getTLLogicControlToUse().closeWAUT(myCurrentWAUTID);
        } catch (InvalidArgument& e) {
            WRITE_ERROR(e.what());
            myCurrentIsBroken = true;
        }
    }
    myCurrentWAUTID = "";
}


/****************************************************************************/
