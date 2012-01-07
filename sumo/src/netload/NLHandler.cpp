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
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
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
#include <utils/common/TplConvertSec.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/RGBColor.h>
#include <utils/geom/GeomConvHelper.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSLane.h>
#include <microsim/MSInternalLane.h>
#include <microsim/MSBitSetLogic.h>
#include <microsim/MSJunctionLogic.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/traffic_lights/MSAgentbasedTrafficLightLogic.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/UtilExceptions.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/shapes/ShapeContainer.h>
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
      mySucceedingLaneBuilder(junctionBuilder),
      myAmInTLLogicMode(false), myCurrentIsBroken(false),
      myHaveWarnedAboutDeprecatedE1(false),
      myHaveWarnedAboutDeprecatedE2(false),
      myHaveWarnedAboutDeprecatedE3(false),
      myHaveWarnedAboutDeprecatedDetEntry(false),
      myHaveWarnedAboutDeprecatedDetExit(false),
      myHaveWarnedAboutDeprecatedRowLogic(false),
      myHaveWarnedAboutDeprecatedTLLogic(false),
      myHaveWarnedAboutDeprecatedTimedEvent(false),
      myHaveWarnedAboutDeprecatedTLSTiming(false),
      myHaveWarnedAboutDeprecatedTimeThreshold(false),
      myHaveWarnedAboutDeprecatedSpeedThreshold(false),
      myHaveWarnedAboutDeprecatedJamDistThreshold(false),
      myHaveWarnedAboutDeprecatedVTypeProbe(false),
      myHaveWarnedAboutDeprecatedRouteProbe(false),
      myHaveWarnedAboutDeprecatedEdgeMean(false),
      myHaveWarnedAboutDeprecatedLaneMean(false),
      myHaveWarnedAboutDeprecatedVTypes(false),
      myHaveWarnedAboutDeprecatedLanes(false),
      myHaveWarnedAboutDeprecatedDistrict(false), myHaveWarnedAboutDeprecatedDSource(false), myHaveWarnedAboutDeprecatedDSink(false) {}


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
            case SUMO_TAG_SUCC:
                openSucc(attrs);
                break;
            case SUMO_TAG_SUCCLANE:
                addSuccLane(attrs);
                break;
            case SUMO_TAG_CONNECTION:
                addConnection(attrs);
                break;
            case SUMO_TAG_ROWLOGIC__DEPRECATED:
                if (!myHaveWarnedAboutDeprecatedRowLogic) {
                    myHaveWarnedAboutDeprecatedRowLogic = true;
                    WRITE_WARNING("Your network uses deprecated tags; please rebuild.");
                }
                initJunctionLogic(attrs);
                break;
            case SUMO_TAG_TLLOGIC__DEPRECATED:
                if (!myHaveWarnedAboutDeprecatedTLLogic) {
                    myHaveWarnedAboutDeprecatedTLLogic = true;
                    WRITE_WARNING("Deprecated tl-logic name; please rebuild.");
                }
            case SUMO_TAG_TLLOGIC:
                initTrafficLightLogic(attrs);
                break;
            case SUMO_TAG_LOGICITEM: // deprecated
                addLogicItem(attrs);
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
#ifdef _MESSAGES
            case SUMO_TAG_MSG_EMITTER:
                addMsgEmitter(attrs);
                break;
#endif
            case SUMO_TAG_E1DETECTOR__DEPRECATED:
                if (!myHaveWarnedAboutDeprecatedE1) {
                    myHaveWarnedAboutDeprecatedE1 = true;
                    WRITE_WARNING("'" + toString(SUMO_TAG_E1DETECTOR__DEPRECATED) + "' is deprecated; please use '" + toString(SUMO_TAG_E1DETECTOR) + "'.");
                }
            case SUMO_TAG_E1DETECTOR:
            case SUMO_TAG_INDUCTION_LOOP:
                addE1Detector(attrs);
                break;
            case SUMO_TAG_E2DETECTOR__DEPRECATED:
                if (!myHaveWarnedAboutDeprecatedE2) {
                    myHaveWarnedAboutDeprecatedE2 = true;
                    WRITE_WARNING("'" + toString(SUMO_TAG_E2DETECTOR__DEPRECATED) + "' is deprecated; please use '" + toString(SUMO_TAG_E2DETECTOR) + "'.");
                }
            case SUMO_TAG_E2DETECTOR:
            case SUMO_TAG_LANE_AREA_DETECTOR:
                addE2Detector(attrs);
                break;
            case SUMO_TAG_E3DETECTOR__DEPRECATED:
                if (!myHaveWarnedAboutDeprecatedE3) {
                    myHaveWarnedAboutDeprecatedE3 = true;
                    WRITE_WARNING("'" + toString(SUMO_TAG_E3DETECTOR__DEPRECATED) + "' is deprecated; please use '" + toString(SUMO_TAG_E3DETECTOR) + "'.");
                }
            case SUMO_TAG_E3DETECTOR:
            case SUMO_TAG_ENTRY_EXIT_DETECTOR:
                beginE3Detector(attrs);
                break;
            case SUMO_TAG_DET_ENTRY__DEPRECATED:
                if (!myHaveWarnedAboutDeprecatedDetEntry) {
                    myHaveWarnedAboutDeprecatedDetEntry = true;
                    WRITE_WARNING("'" + toString(SUMO_TAG_DET_ENTRY__DEPRECATED) + "' is deprecated; please use '" + toString(SUMO_TAG_DET_ENTRY) + "'.");
                }
            case SUMO_TAG_DET_ENTRY:
                addE3Entry(attrs);
                break;
            case SUMO_TAG_DET_EXIT__DEPRECATED:
                if (!myHaveWarnedAboutDeprecatedDetExit) {
                    myHaveWarnedAboutDeprecatedDetExit = true;
                    WRITE_WARNING("'" + toString(SUMO_TAG_DET_EXIT__DEPRECATED) + "' is deprecated; please use '" + toString(SUMO_TAG_DET_EXIT) + "'.");
                }
            case SUMO_TAG_DET_EXIT:
                addE3Exit(attrs);
                break;
            case SUMO_TAG_INSTANT_INDUCTION_LOOP:
                addInstantE1Detector(attrs);
                break;
            case SUMO_TAG_VSS:
                myTriggerBuilder.parseAndBuildLaneSpeedTrigger(myNet, attrs, getFileName());
                break;
#ifdef HAVE_MESOSIM
            case SUMO_TAG_CALIBRATOR:
                myTriggerBuilder.parseAndBuildCalibrator(myNet, attrs, getFileName());
                break;
#endif
            case SUMO_TAG_REROUTER:
                myTriggerBuilder.parseAndBuildRerouter(myNet, attrs, getFileName());
                break;
            case SUMO_TAG_BUS_STOP:
                myTriggerBuilder.parseAndBuildBusStop(myNet, attrs);
                break;
            case SUMO_TAG_VTYPEPROBE__DEPRECATED:
                if (!myHaveWarnedAboutDeprecatedVTypeProbe) {
                    myHaveWarnedAboutDeprecatedVTypeProbe = true;
                    WRITE_WARNING("'" + toString(SUMO_TAG_VTYPEPROBE__DEPRECATED) + "' is deprecated; please use '" + toString(SUMO_TAG_VTYPEPROBE) + "'.");
                }
            case SUMO_TAG_VTYPEPROBE:
                addVTypeProbeDetector(attrs);
                break;
            case SUMO_TAG_ROUTEPROBE__DEPRECATED:
                if (!myHaveWarnedAboutDeprecatedRouteProbe) {
                    myHaveWarnedAboutDeprecatedRouteProbe = true;
                    WRITE_WARNING("'" + toString(SUMO_TAG_ROUTEPROBE__DEPRECATED) + "' is deprecated; please use '" + toString(SUMO_TAG_ROUTEPROBE) + "'.");
                }
            case SUMO_TAG_ROUTEPROBE:
                addRouteProbeDetector(attrs);
                break;
            case SUMO_TAG_MEANDATA_EDGE__DEPRECATED:
                if (!myHaveWarnedAboutDeprecatedEdgeMean) {
                    myHaveWarnedAboutDeprecatedEdgeMean = true;
                    WRITE_WARNING("'" + toString(SUMO_TAG_MEANDATA_EDGE__DEPRECATED) + "' is deprecated; please use '" + toString(SUMO_TAG_MEANDATA_EDGE) + "'.");
                }
            case SUMO_TAG_MEANDATA_EDGE:
                addEdgeLaneMeanData(attrs, SUMO_TAG_MEANDATA_EDGE);
                break;
            case SUMO_TAG_MEANDATA_LANE__DEPRECATED:
                if (!myHaveWarnedAboutDeprecatedLaneMean) {
                    myHaveWarnedAboutDeprecatedLaneMean = true;
                    WRITE_WARNING("'" + toString(SUMO_TAG_MEANDATA_LANE__DEPRECATED) + "' is deprecated; please use '" + toString(SUMO_TAG_MEANDATA_LANE) + "'.");
                }
            case SUMO_TAG_MEANDATA_LANE:
                addEdgeLaneMeanData(attrs, SUMO_TAG_MEANDATA_LANE);
                break;
            case SUMO_TAG_TIMEDEVENT__DEPRECATED:
                if (!myHaveWarnedAboutDeprecatedTimedEvent) {
                    myHaveWarnedAboutDeprecatedTimedEvent = true;
                    WRITE_WARNING("'" + toString(SUMO_TAG_TIMEDEVENT__DEPRECATED) + "' is deprecated; please use '" + toString(SUMO_TAG_TIMEDEVENT) + "'.");
                }
            case SUMO_TAG_TIMEDEVENT:
                myActionBuilder.addAction(attrs, getFileName());
                break;
            case SUMO_TAG_VAPORIZER:
                myTriggerBuilder.buildVaporizer(attrs);
                break;
            case SUMO_TAG_LOCATION:
                setLocation(attrs);
                break;
            case SUMO_TAG_DISTRICT__DEPRECATED:
                if (!myHaveWarnedAboutDeprecatedDistrict) {
                    myHaveWarnedAboutDeprecatedDistrict = true;
                    WRITE_WARNING("'" + toString(SUMO_TAG_DISTRICT__DEPRECATED) + "' is deprecated, please use '" + toString(SUMO_TAG_TAZ) + "'.");
                }
            case SUMO_TAG_TAZ:
                addDistrict(attrs);
                break;
            case SUMO_TAG_DSOURCE__DEPRECATED:
                if (!myHaveWarnedAboutDeprecatedDSource) {
                    myHaveWarnedAboutDeprecatedDSource = true;
                    WRITE_WARNING("'" + toString(SUMO_TAG_DSOURCE__DEPRECATED) + "' is deprecated, please use '" + toString(SUMO_TAG_TAZSOURCE) + "'.");
                }
            case SUMO_TAG_TAZSOURCE:
                addDistrictEdge(attrs, true);
                break;
            case SUMO_TAG_DSINK__DEPRECATED:
                if (!myHaveWarnedAboutDeprecatedDSink) {
                    myHaveWarnedAboutDeprecatedDSink = true;
                    WRITE_WARNING("'" + toString(SUMO_TAG_DSINK__DEPRECATED) + "' is deprecated, please use '" + toString(SUMO_TAG_TAZSINK) + "'.");
                }
            case SUMO_TAG_TAZSINK:
                addDistrictEdge(attrs, false);
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
        case SUMO_TAG_SUCC:
            closeSuccLane();
            break;
        case SUMO_TAG_ROWLOGIC__DEPRECATED:
            try {
                myJunctionControlBuilder.closeJunctionLogic();
            } catch (InvalidArgument& e) {
                WRITE_ERROR(e.what());
            }
            break;
        case SUMO_TAG_TLLOGIC__DEPRECATED:
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
        case SUMO_TAG_E3DETECTOR__DEPRECATED:
        case SUMO_TAG_E3DETECTOR:
        case SUMO_TAG_ENTRY_EXIT_DETECTOR:
            endE3Detector();
            break;
        default:
            break;
    }
    MSRouteHandler::myEndElement(element);
}



// ---- the root/edge - element
void
NLHandler::beginEdgeParsing(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    myCurrentIsBroken = false;
    // get the id, report an error if not given or empty...
    std::string id = attrs.getStringReporting(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        myCurrentIsBroken = true;
        return;
    }
    // omit internal edges if not wished
    if (!MSGlobals::gUsingInternalLanes && id[0] == ':') {
        myCurrentIsInternalToSkip = true;
        return;
    }
    myCurrentIsInternalToSkip = false;
    // parse the function
    SumoXMLEdgeFunc func = EDGEFUNC_NORMAL;
    std::string funcString = attrs.getOptStringReporting(SUMO_ATTR_FUNCTION, id.c_str(), ok, toString(func));
    if (!ok) {
        myCurrentIsBroken = true;
        return;
    }
    if (SUMOXMLDefinitions::EdgeFunctions.hasString(funcString)) {
        func = SUMOXMLDefinitions::EdgeFunctions.get(funcString);
    } else {
        WRITE_ERROR("Edge '" + id + "' has an invalid type ('" + funcString + "').");
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
    std::string streetName = attrs.getOptStringReporting(SUMO_ATTR_NAME, id.c_str(), ok, "");
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
    std::string id = attrs.getStringReporting(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        myCurrentIsBroken = true;
        return;
    }
    SUMOReal maxSpeed = attrs.hasAttribute(SUMO_ATTR_SPEED)
                        ? attrs.getSUMORealReporting(SUMO_ATTR_SPEED, id.c_str(), ok)
                        : attrs.getSUMORealReporting(SUMO_ATTR_MAXSPEED__DEPRECATED, id.c_str(), ok);
    SUMOReal length = attrs.getSUMORealReporting(SUMO_ATTR_LENGTH, id.c_str(), ok);
    std::string allow = attrs.getOptStringReporting(SUMO_ATTR_ALLOW, id.c_str(), ok, "");
    std::string disallow = attrs.getOptStringReporting(SUMO_ATTR_DISALLOW, id.c_str(), ok, "");
    SUMOReal width = attrs.getOptSUMORealReporting(SUMO_ATTR_WIDTH, id.c_str(), ok, SUMO_const_laneWidth);
    int index = attrs.getOptIntReporting(SUMO_ATTR_INDEX, id.c_str(), ok, -1);
    PositionVector shape = GeomConvHelper::parseShapeReporting(attrs.getStringReporting(SUMO_ATTR_SHAPE, id.c_str(), ok), "lane", id.c_str(), ok, false);
    if (shape.size() < 2) {
        WRITE_ERROR("Shape of lane '" + id + "' is broken.\n Can not build according edge.");
        myCurrentIsBroken = true;
        return;
    }
    SUMOVehicleClasses allowedClasses;
    SUMOVehicleClasses disallowedClasses;
    parseVehicleClasses(allow, disallow, allowedClasses, disallowedClasses);
    myCurrentIsBroken |= !ok;
    if (!myCurrentIsBroken) {
        try {
            MSLane* lane = myEdgeControlBuilder.addLane(id, maxSpeed, length, shape, width, allowedClasses, disallowedClasses);
            // insert the lane into the lane-dictionary, checking
            if (!MSLane::dictionary(id, lane)) {
                delete lane;
                WRITE_ERROR("Another lane with the id '" + id + "' exists.");
                myCurrentIsBroken = true;
            }
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
    std::string id = attrs.getStringReporting(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        myCurrentIsBroken = true;
        return;
    }
    PositionVector shape;
    if (attrs.hasAttribute(SUMO_ATTR_SHAPE)) {
        // inner junctions have no shape
        shape = GeomConvHelper::parseShapeReporting(attrs.getStringSecure(SUMO_ATTR_SHAPE, ""), attrs.getObjectType(), id.c_str(), ok, true);
    }
    SUMOReal x = attrs.getSUMORealReporting(SUMO_ATTR_X, id.c_str(), ok);
    SUMOReal y = attrs.getSUMORealReporting(SUMO_ATTR_Y, id.c_str(), ok);
    std::string type = attrs.getStringReporting(SUMO_ATTR_TYPE, id.c_str(), ok);
    std::string key = attrs.getOptStringReporting(SUMO_ATTR_KEY, id.c_str(), ok, "");
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
                      const std::string& def, std::vector<MSLane*> &into, bool& ok) {
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
    std::string key = attrs.getStringReporting(SUMO_ATTR_KEY, 0, ok);
    std::string val = attrs.getStringReporting(SUMO_ATTR_VALUE, 0, ok);
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
    std::string id = attrs.getStringReporting(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        myCurrentIsBroken = true;
        return;
    }
    SUMOTime t = attrs.getOptSUMOTimeReporting(SUMO_ATTR_REF_TIME, id.c_str(), ok, 0);
    std::string pro = attrs.getStringReporting(SUMO_ATTR_START_PROG, id.c_str(), ok);
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
    std::string to = attrs.getStringReporting(SUMO_ATTR_TO, myCurrentWAUTID.c_str(), ok);
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
    } catch (InvalidArgument& e) {
        WRITE_ERROR(e.what());
        myCurrentIsBroken = true;
    }
}







void
NLHandler::addPOI(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    const SUMOReal INVALID_POSITION(-1000000);
    std::string id = attrs.getStringReporting(SUMO_ATTR_ID, 0, ok);
    SUMOReal x = attrs.getOptSUMORealReporting(SUMO_ATTR_X, id.c_str(), ok, INVALID_POSITION);
    SUMOReal y = attrs.getOptSUMORealReporting(SUMO_ATTR_Y, id.c_str(), ok, INVALID_POSITION);
    SUMOReal lanePos = attrs.getOptSUMORealReporting(SUMO_ATTR_POSITION, id.c_str(), ok, INVALID_POSITION);
    int layer = attrs.getOptIntReporting(SUMO_ATTR_LAYER, id.c_str(), ok, GLO_SHAPE);
    std::string type = attrs.getOptStringReporting(SUMO_ATTR_TYPE, id.c_str(), ok, "");
    std::string laneID = attrs.getOptStringReporting(SUMO_ATTR_LANE, id.c_str(), ok, "");
    std::string colorStr = attrs.getOptStringReporting(SUMO_ATTR_COLOR, id.c_str(), ok, "1,0,0");
    RGBColor color = RGBColor::parseColorReporting(colorStr, attrs.getObjectType(), id.c_str(), true, ok);
    if (!ok) {
        return;
    }
    Position pos(x, y);
    if (x == INVALID_POSITION || y == INVALID_POSITION) {
        MSLane* lane = MSLane::dictionary(laneID);
        if (lane == 0) {
            WRITE_ERROR("Lane '" + laneID + "' to place a poi '" + id + "'on is not known.");
            return;
        }
        if (lanePos < 0) {
            lanePos = lane->getLength() + lanePos;
        }
        pos = lane->getShape().positionAtLengthPosition(lanePos);
    }
    if (!myNet.getShapeContainer().addPoI(id, layer, type, color, pos)) {
        WRITE_ERROR("PoI '" + id + "' already exists.");
    }
}


void
NLHandler::addPoly(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string id = attrs.getStringReporting(SUMO_ATTR_ID, 0, ok);
    // get the id, report an error if not given or empty...
    if (!ok) {
        return;
    }
    int layer = attrs.getOptIntReporting(SUMO_ATTR_LAYER, id.c_str(), ok, GLO_SHAPE);
    bool fill = attrs.getOptBoolReporting(SUMO_ATTR_FILL, id.c_str(), ok, false);
    std::string type = attrs.getOptStringReporting(SUMO_ATTR_TYPE, id.c_str(), ok, "");
    std::string colorStr = attrs.getStringReporting(SUMO_ATTR_COLOR, id.c_str(), ok);
    RGBColor color = RGBColor::parseColorReporting(colorStr, attrs.getObjectType(), id.c_str(), true, ok);
    PositionVector shape = GeomConvHelper::parseShapeReporting(attrs.getStringReporting(SUMO_ATTR_SHAPE, id.c_str(), ok), attrs.getObjectType(), id.c_str(), ok, false);
    if (shape.size() != 0) {
        if (!myNet.getShapeContainer().addPolygon(id, layer, type, color, fill, shape)) {
            WRITE_ERROR("Polygon '" + id + "' already exists.");
        }
    }
}


void
NLHandler::addLogicItem(const SUMOSAXAttributes& attrs) {
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
    if (request >= 0 && response.length() > 0) {
        try {
            myJunctionControlBuilder.addLogicItem(request, response, foes, cont);
        } catch (InvalidArgument& e) {
            WRITE_ERROR(e.what());
        }
    }
}


void
NLHandler::addRequest(const SUMOSAXAttributes& attrs) {
    if (myCurrentIsBroken) {
        return;
    }
    bool ok = true;
    int request = attrs.getIntReporting(SUMO_ATTR_INDEX, 0, ok);
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
    std::string id = attrs.getStringReporting(SUMO_ATTR_ID, 0, ok);
    if (ok) {
        myJunctionControlBuilder.initJunctionLogic(id);
    }
}


void
NLHandler::initTrafficLightLogic(const SUMOSAXAttributes& attrs) {
    myAmInTLLogicMode = true;
    bool ok = true;
    std::string id = attrs.getStringReporting(SUMO_ATTR_ID, 0, ok);
    TrafficLightType type;
    std::string typeS = attrs.getStringReporting(SUMO_ATTR_TYPE, 0, ok);
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
    std::string programID = attrs.getOptStringReporting(SUMO_ATTR_PROGRAMID, id.c_str(), ok, "<unknown>");
    myJunctionControlBuilder.initTrafficLightLogic(id, programID, type, offset);
}


void
NLHandler::addPhase(const SUMOSAXAttributes& attrs) {
    // try to get the phase definition
    bool ok = true;
    std::string state = attrs.getStringReporting(SUMO_ATTR_STATE, 0, ok);
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
    SUMOTime minDuration = -1;
    if (attrs.hasAttribute(SUMO_ATTR_MINDURATION__DEPRECATED)) {
        minDuration = attrs.getSUMOTimeReporting(SUMO_ATTR_MINDURATION__DEPRECATED, myJunctionControlBuilder.getActiveKey().c_str(), ok);
        if (!myHaveWarnedAboutDeprecatedTLSTiming) {
            myHaveWarnedAboutDeprecatedTLSTiming = true;
            WRITE_WARNING("Your tls definition contains deprecated minimum/maximum duration attribute; use minDur and maxDur instead.");
        }
    } else {
        minDuration = attrs.getOptSUMOTimeReporting(SUMO_ATTR_MINDURATION, myJunctionControlBuilder.getActiveKey().c_str(), ok, -1);
    }
    SUMOTime maxDuration = -1;
    if (attrs.hasAttribute(SUMO_ATTR_MAXDURATION__DEPRECATED)) {
        maxDuration = attrs.getSUMOTimeReporting(SUMO_ATTR_MAXDURATION__DEPRECATED, myJunctionControlBuilder.getActiveKey().c_str(), ok);
        if (!myHaveWarnedAboutDeprecatedTLSTiming) {
            myHaveWarnedAboutDeprecatedTLSTiming = true;
            WRITE_WARNING("Your tls definition contains deprecated minimum/maximum duration attribute; use minDur and maxDur instead.");
        }
    } else {
        maxDuration = attrs.getOptSUMOTimeReporting(SUMO_ATTR_MAXDURATION, myJunctionControlBuilder.getActiveKey().c_str(), ok, -1);
    }
    myJunctionControlBuilder.addPhase(duration, state, minDuration, maxDuration);
}


#ifdef _MESSAGES
void
NLHandler::addMsgEmitter(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string id = attrs.getStringReporting(SUMO_ATTR_ID, 0, ok);
    std::string file = attrs.getOptStringReporting(SUMO_ATTR_FILE, 0, ok, "");
    // if no file given, use stdout
    if (file == "") {
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
NLHandler::addE1Detector(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    // get the id, report an error if not given or empty...
    std::string id = attrs.getStringReporting(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        return;
    }
    // inform the user about deprecated values
    SUMOTime frequency = attrs.getSUMOTimeReporting(SUMO_ATTR_FREQUENCY, id.c_str(), ok);
    SUMOReal position = attrs.getSUMORealReporting(SUMO_ATTR_POSITION, id.c_str(), ok);
    if (attrs.hasAttribute(SUMO_ATTR_FRIENDLY_POS__DEPRECATED) && !myHaveWarnedAboutDeprecatedFriendlyPos) {
        myHaveWarnedAboutDeprecatedFriendlyPos = true;
        WRITE_WARNING("'" + toString(SUMO_ATTR_FRIENDLY_POS__DEPRECATED) + "' is deprecated, use '" + toString(SUMO_ATTR_FRIENDLY_POS) + "' instead.");
    }
    bool friendlyPos = attrs.hasAttribute(SUMO_ATTR_FRIENDLY_POS__DEPRECATED)
                       ? attrs.getOptBoolReporting(SUMO_ATTR_FRIENDLY_POS__DEPRECATED, id.c_str(), ok, false)
                       : attrs.getOptBoolReporting(SUMO_ATTR_FRIENDLY_POS, id.c_str(), ok, false);
    bool splitByType = attrs.getOptBoolReporting(SUMO_ATTR_SPLIT_VTYPE, id.c_str(), ok, false);
    std::string lane = attrs.getStringReporting(SUMO_ATTR_LANE, id.c_str(), ok);
    std::string file = attrs.getStringReporting(SUMO_ATTR_FILE, id.c_str(), ok);
    if (!ok) {
        return;
    }
    try {
        myDetectorBuilder.buildInductLoop(id, lane, position, frequency,
                                          OutputDevice::getDevice(file, getFileName()),
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
    std::string id = attrs.getStringReporting(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        return;
    }
    // inform the user about deprecated values
    SUMOReal position = attrs.getSUMORealReporting(SUMO_ATTR_POSITION, id.c_str(), ok);
    if (attrs.hasAttribute(SUMO_ATTR_FRIENDLY_POS__DEPRECATED) && !myHaveWarnedAboutDeprecatedFriendlyPos) {
        myHaveWarnedAboutDeprecatedFriendlyPos = true;
        WRITE_WARNING("'" + toString(SUMO_ATTR_FRIENDLY_POS__DEPRECATED) + "' is deprecated, use '" + toString(SUMO_ATTR_FRIENDLY_POS) + "' instead.");
    }
    bool friendlyPos = attrs.hasAttribute(SUMO_ATTR_FRIENDLY_POS__DEPRECATED)
                       ? attrs.getOptBoolReporting(SUMO_ATTR_FRIENDLY_POS__DEPRECATED, id.c_str(), ok, false)
                       : attrs.getOptBoolReporting(SUMO_ATTR_FRIENDLY_POS, id.c_str(), ok, false);
    std::string lane = attrs.getStringReporting(SUMO_ATTR_LANE, id.c_str(), ok);
    std::string file = attrs.getStringReporting(SUMO_ATTR_FILE, id.c_str(), ok);
    if (!ok) {
        return;
    }
    try {
        myDetectorBuilder.buildInstantInductLoop(id, lane, position, OutputDevice::getDevice(file, getFileName()), friendlyPos);
    } catch (InvalidArgument& e) {
        WRITE_ERROR(e.what());
    } catch (IOError& e) {
        WRITE_ERROR(e.what());
    }
}


void
NLHandler::addVTypeProbeDetector(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string id = attrs.getStringReporting(SUMO_ATTR_ID, 0, ok);
    SUMOTime frequency = attrs.getSUMOTimeReporting(SUMO_ATTR_FREQUENCY, id.c_str(), ok);
    std::string type = attrs.getStringSecure(SUMO_ATTR_TYPE, "");
    std::string file = attrs.getStringReporting(SUMO_ATTR_FILE, id.c_str(), ok);
    if (!ok) {
        return;
    }
    try {
        myDetectorBuilder.buildVTypeProbe(id, type, frequency, OutputDevice::getDevice(file, getFileName()));
    } catch (InvalidArgument& e) {
        WRITE_ERROR(e.what());
    } catch (IOError& e) {
        WRITE_ERROR(e.what());
    }
}


void
NLHandler::addRouteProbeDetector(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string id = attrs.getStringReporting(SUMO_ATTR_ID, 0, ok);
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
    std::string id = attrs.getStringReporting(SUMO_ATTR_ID, 0, ok);
    std::string lsaid = attrs.getOptStringReporting(SUMO_ATTR_TLID, id.c_str(), ok, "<invalid>");
    std::string toLane = attrs.getOptStringReporting(SUMO_ATTR_TO, id.c_str(), ok, "<invalid>");
    //
    if (attrs.hasAttribute(SUMO_ATTR_HALTING_TIME_THRESHOLD__DEPRECATED)) {
        myHaveWarnedAboutDeprecatedTimeThreshold = true;
        WRITE_WARNING("'" + toString(SUMO_ATTR_HALTING_TIME_THRESHOLD__DEPRECATED) + "' is deprecated; please use '" + toString(SUMO_ATTR_HALTING_TIME_THRESHOLD) + "'.");
    }
    if (attrs.hasAttribute(SUMO_ATTR_HALTING_SPEED_THRESHOLD__DEPRECATED)) {
        myHaveWarnedAboutDeprecatedSpeedThreshold = true;
        WRITE_WARNING("'" + toString(SUMO_ATTR_HALTING_SPEED_THRESHOLD__DEPRECATED) + "' is deprecated; please use '" + toString(SUMO_ATTR_HALTING_SPEED_THRESHOLD) + "'.");
    }
    if (attrs.hasAttribute(SUMO_ATTR_JAM_DIST_THRESHOLD__DEPRECATED)) {
        myHaveWarnedAboutDeprecatedJamDistThreshold = true;
        WRITE_WARNING("'" + toString(SUMO_ATTR_JAM_DIST_THRESHOLD__DEPRECATED) + "' is deprecated; please use '" + toString(SUMO_ATTR_JAM_DIST_THRESHOLD) + "'.");
    }
    if (attrs.hasAttribute(SUMO_ATTR_FRIENDLY_POS__DEPRECATED) && !myHaveWarnedAboutDeprecatedFriendlyPos) {
        myHaveWarnedAboutDeprecatedFriendlyPos = true;
        WRITE_WARNING("'" + toString(SUMO_ATTR_FRIENDLY_POS__DEPRECATED) + "' is deprecated; please use '" + toString(SUMO_ATTR_FRIENDLY_POS) + "'.");
    }

    const SUMOTime haltingTimeThreshold = attrs.hasAttribute(SUMO_ATTR_HALTING_TIME_THRESHOLD__DEPRECATED)
                                          ? attrs.getOptSUMOTimeReporting(SUMO_ATTR_HALTING_TIME_THRESHOLD__DEPRECATED, id.c_str(), ok, TIME2STEPS(1))
                                          : attrs.getOptSUMOTimeReporting(SUMO_ATTR_HALTING_TIME_THRESHOLD, id.c_str(), ok, TIME2STEPS(1));
    const SUMOReal haltingSpeedThreshold = attrs.hasAttribute(SUMO_ATTR_HALTING_SPEED_THRESHOLD__DEPRECATED)
                                           ? attrs.getOptSUMORealReporting(SUMO_ATTR_HALTING_SPEED_THRESHOLD__DEPRECATED, id.c_str(), ok, 5.0f / 3.6f)
                                           : attrs.getOptSUMORealReporting(SUMO_ATTR_HALTING_SPEED_THRESHOLD, id.c_str(), ok, 5.0f / 3.6f);
    const SUMOReal jamDistThreshold = attrs.hasAttribute(SUMO_ATTR_JAM_DIST_THRESHOLD__DEPRECATED)
                                      ? attrs.getOptSUMORealReporting(SUMO_ATTR_JAM_DIST_THRESHOLD__DEPRECATED, id.c_str(), ok, 10.0f)
                                      : attrs.getOptSUMORealReporting(SUMO_ATTR_JAM_DIST_THRESHOLD, id.c_str(), ok, 10.0f);
    const SUMOReal position = attrs.getSUMORealReporting(SUMO_ATTR_POSITION, id.c_str(), ok);
    const SUMOReal length = attrs.getSUMORealReporting(SUMO_ATTR_LENGTH, id.c_str(), ok);
    const bool friendlyPos = attrs.hasAttribute(SUMO_ATTR_FRIENDLY_POS__DEPRECATED)
                             ? attrs.getOptBoolReporting(SUMO_ATTR_FRIENDLY_POS__DEPRECATED, id.c_str(), ok, false)
                             : attrs.getOptBoolReporting(SUMO_ATTR_FRIENDLY_POS, id.c_str(), ok, false);
    const bool cont = attrs.getOptBoolReporting(SUMO_ATTR_CONT, id.c_str(), ok, false);
    const std::string lane = attrs.getStringReporting(SUMO_ATTR_LANE, id.c_str(), ok);
    const std::string file = attrs.getStringReporting(SUMO_ATTR_FILE, id.c_str(), ok);
    if (!ok) {
        return;
    }
    try {
        if (lsaid != "<invalid>") {
            if (toLane == "<invalid>") {
                myDetectorBuilder.buildE2Detector(id, lane, position, length, cont,
                                                  myJunctionControlBuilder.getTLLogic(lsaid),
                                                  OutputDevice::getDevice(file, getFileName()),
                                                  haltingTimeThreshold, haltingSpeedThreshold, jamDistThreshold,
                                                  friendlyPos);
            } else {
                myDetectorBuilder.buildE2Detector(id, lane, position, length, cont,
                                                  myJunctionControlBuilder.getTLLogic(lsaid), toLane,
                                                  OutputDevice::getDevice(file, getFileName()),
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
                                              OutputDevice::getDevice(file, getFileName()),
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
    // inform the user about deprecated values
    std::string id = attrs.getStringReporting(SUMO_ATTR_ID, 0, ok);
    if (attrs.hasAttribute(SUMO_ATTR_HALTING_TIME_THRESHOLD__DEPRECATED)) {
        myHaveWarnedAboutDeprecatedTimeThreshold = true;
        WRITE_WARNING("'" + toString(SUMO_ATTR_HALTING_TIME_THRESHOLD__DEPRECATED) + "' is deprecated; please use '" + toString(SUMO_ATTR_HALTING_TIME_THRESHOLD) + "'.");
    }
    if (attrs.hasAttribute(SUMO_ATTR_HALTING_SPEED_THRESHOLD__DEPRECATED)) {
        myHaveWarnedAboutDeprecatedSpeedThreshold = true;
        WRITE_WARNING("'" + toString(SUMO_ATTR_HALTING_SPEED_THRESHOLD__DEPRECATED) + "' is deprecated; please use '" + toString(SUMO_ATTR_HALTING_SPEED_THRESHOLD) + "'.");
    }

    const SUMOTime frequency = attrs.getSUMOTimeReporting(SUMO_ATTR_FREQUENCY, id.c_str(), ok);
    const SUMOTime haltingTimeThreshold = attrs.hasAttribute(SUMO_ATTR_HALTING_TIME_THRESHOLD__DEPRECATED)
                                          ? attrs.getOptSUMOTimeReporting(SUMO_ATTR_HALTING_TIME_THRESHOLD__DEPRECATED, id.c_str(), ok, TIME2STEPS(1))
                                          : attrs.getOptSUMOTimeReporting(SUMO_ATTR_HALTING_TIME_THRESHOLD, id.c_str(), ok, TIME2STEPS(1));
    const SUMOReal haltingSpeedThreshold = attrs.hasAttribute(SUMO_ATTR_HALTING_SPEED_THRESHOLD__DEPRECATED)
                                           ? attrs.getOptSUMORealReporting(SUMO_ATTR_HALTING_SPEED_THRESHOLD__DEPRECATED, id.c_str(), ok, 5.0f / 3.6f)
                                           : attrs.getOptSUMORealReporting(SUMO_ATTR_HALTING_SPEED_THRESHOLD, id.c_str(), ok, 5.0f / 3.6f);
    const std::string file = attrs.getStringReporting(SUMO_ATTR_FILE, id.c_str(), ok);
    if (!ok) {
        return;
    }
    try {
        myDetectorBuilder.beginE3Detector(id,
                                          OutputDevice::getDevice(file, getFileName()),
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
    if (attrs.hasAttribute(SUMO_ATTR_FRIENDLY_POS__DEPRECATED) && !myHaveWarnedAboutDeprecatedFriendlyPos) {
        myHaveWarnedAboutDeprecatedFriendlyPos = true;
        WRITE_WARNING("'" + toString(SUMO_ATTR_FRIENDLY_POS__DEPRECATED) + "' is deprecated, use '" + toString(SUMO_ATTR_FRIENDLY_POS) + "' instead.");
    }
    const SUMOReal position = attrs.getSUMORealReporting(SUMO_ATTR_POSITION, myDetectorBuilder.getCurrentE3ID().c_str(), ok);
    const bool friendlyPos = attrs.hasAttribute(SUMO_ATTR_FRIENDLY_POS__DEPRECATED)
                             ? attrs.getOptBoolReporting(SUMO_ATTR_FRIENDLY_POS__DEPRECATED, myDetectorBuilder.getCurrentE3ID().c_str(), ok, false)
                             : attrs.getOptBoolReporting(SUMO_ATTR_FRIENDLY_POS, myDetectorBuilder.getCurrentE3ID().c_str(), ok, false);
    const std::string lane = attrs.getStringReporting(SUMO_ATTR_LANE, myDetectorBuilder.getCurrentE3ID().c_str(), ok);
    if (!ok) {
        return;
    }
    myDetectorBuilder.addE3Entry(lane, position, friendlyPos);
}


void
NLHandler::addE3Exit(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    const SUMOReal position = attrs.getSUMORealReporting(SUMO_ATTR_POSITION, myDetectorBuilder.getCurrentE3ID().c_str(), ok);
    if (attrs.hasAttribute(SUMO_ATTR_FRIENDLY_POS__DEPRECATED) && !myHaveWarnedAboutDeprecatedFriendlyPos) {
        myHaveWarnedAboutDeprecatedFriendlyPos = true;
        WRITE_WARNING("'" + toString(SUMO_ATTR_FRIENDLY_POS__DEPRECATED) + "' is deprecated, use '" + toString(SUMO_ATTR_FRIENDLY_POS) + "' instead.");
    }
    const bool friendlyPos = attrs.hasAttribute(SUMO_ATTR_FRIENDLY_POS__DEPRECATED)
                             ? attrs.getOptBoolReporting(SUMO_ATTR_FRIENDLY_POS__DEPRECATED, myDetectorBuilder.getCurrentE3ID().c_str(), ok, false)
                             : attrs.getOptBoolReporting(SUMO_ATTR_FRIENDLY_POS, myDetectorBuilder.getCurrentE3ID().c_str(), ok, false);
    const std::string lane = attrs.getStringReporting(SUMO_ATTR_LANE, myDetectorBuilder.getCurrentE3ID().c_str(), ok);
    if (!ok) {
        return;
    }
    myDetectorBuilder.addE3Exit(lane, position, friendlyPos);
}


void
NLHandler::addEdgeLaneMeanData(const SUMOSAXAttributes& attrs, int objecttype) {
    bool ok = true;
    std::string id = attrs.getStringReporting(SUMO_ATTR_ID, 0, ok);
    const SUMOReal maxTravelTime = attrs.getOptSUMORealReporting(SUMO_ATTR_MAX_TRAVELTIME, id.c_str(), ok, 100000);
    const SUMOReal minSamples = attrs.getOptSUMORealReporting(SUMO_ATTR_MIN_SAMPLES, id.c_str(), ok, 0);
    const SUMOReal haltingSpeedThreshold = attrs.getOptSUMORealReporting(SUMO_ATTR_HALTING_SPEED_THRESHOLD, id.c_str(), ok, POSITION_EPS);
    const bool excludeEmpty = attrs.getOptBoolReporting(SUMO_ATTR_EXCLUDE_EMPTY, id.c_str(), ok, false);
    const bool withInternal = attrs.getOptBoolReporting(SUMO_ATTR_WITH_INTERNAL, id.c_str(), ok, false);
    const bool trackVehicles = attrs.getOptBoolReporting(SUMO_ATTR_TRACK_VEHICLES, id.c_str(), ok, false);
    const std::string file = attrs.getStringReporting(SUMO_ATTR_FILE, id.c_str(), ok);
    const std::string type = attrs.getOptStringReporting(SUMO_ATTR_TYPE, id.c_str(), ok, "performance");
    std::string vtypes = attrs.getOptStringReporting(SUMO_ATTR_VTYPES, id.c_str(), ok, "");
    if (attrs.hasAttribute(SUMO_ATTR_VTYPES__DEPRECATED)) {
        vtypes = attrs.getStringReporting(SUMO_ATTR_VTYPES__DEPRECATED, id.c_str(), ok);
        if (!myHaveWarnedAboutDeprecatedVTypes) {
            WRITE_WARNING("'" + toString(SUMO_ATTR_VTYPES__DEPRECATED) + " is deprecated; please use '" + toString(SUMO_ATTR_VTYPES) + "'.");
            myHaveWarnedAboutDeprecatedVTypes = true;
        }
    }
    const SUMOTime frequency = attrs.getOptSUMOTimeReporting(SUMO_ATTR_FREQUENCY, id.c_str(), ok, -1);
    const SUMOTime begin = attrs.getOptSUMOTimeReporting(SUMO_ATTR_BEGIN, id.c_str(), ok, string2time(OptionsCont::getOptions().getString("begin")));
    const SUMOTime end = attrs.getOptSUMOTimeReporting(SUMO_ATTR_END, id.c_str(), ok, string2time(OptionsCont::getOptions().getString("end")));
    if (!ok) {
        return;
    }
    try {
        myDetectorBuilder.createEdgeLaneMeanData(id, frequency, begin, end,
                type, objecttype == SUMO_TAG_MEANDATA_LANE, !excludeEmpty, withInternal, trackVehicles,
                maxTravelTime, minSamples, haltingSpeedThreshold, vtypes,
                OutputDevice::getDevice(file, getFileName()));
    } catch (InvalidArgument& e) {
        WRITE_ERROR(e.what());
    } catch (IOError& e) {
        WRITE_ERROR(e.what());
    }
}



void
NLHandler::openSucc(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string id = attrs.getStringReporting(SUMO_ATTR_LANE, 0, ok);
    if (!MSGlobals::gUsingInternalLanes && id[0] == ':') {
        myCurrentIsInternalToSkip = true;
        return;
    }
    myCurrentIsInternalToSkip = false;
    mySucceedingLaneBuilder.openSuccLane(id);
}


void
NLHandler::addSuccLane(const SUMOSAXAttributes& attrs) {
    // do not process internal lanes if not wished
    if (myCurrentIsInternalToSkip) {
        return;
    }
    try {
        bool ok = true;
        SUMOReal pass = attrs.getOptSUMORealReporting(SUMO_ATTR_PASS, 0, ok, -1);
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
        if (tlID != "") {
            int linkNumber = attrs.hasAttribute(SUMO_ATTR_TLLINKINDEX)
                             ? attrs.getIntReporting(SUMO_ATTR_TLLINKINDEX, 0, ok)
                             : attrs.getIntReporting(SUMO_ATTR_TLLINKNO__DEPRECATED, 0, ok);
            if (!ok) {
                return;
            }
            mySucceedingLaneBuilder.addSuccLane(lane,
#ifdef HAVE_INTERNAL_LANES
                                                via, pass,
#endif
                                                parseLinkDir(dir), parseLinkState(state), tlID, linkNumber);
        } else {
            mySucceedingLaneBuilder.addSuccLane(lane,
#ifdef HAVE_INTERNAL_LANES
                                                via, pass,
#endif
                                                parseLinkDir(dir), parseLinkState(state));
        }
    } catch (InvalidArgument& e) {
        WRITE_ERROR(e.what());
    }
}



void
NLHandler::addConnection(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string fromID = attrs.getStringReporting(SUMO_ATTR_FROM, 0, ok);
    if (!MSGlobals::gUsingInternalLanes && fromID[0] == ':') {
        return;
    }

    try {
        bool ok = true;
        std::string toID = attrs.getStringReporting(SUMO_ATTR_TO, 0, ok);
        std::string laneIndices;
        if (attrs.hasAttribute(SUMO_ATTR_LANE)) {
            if (!myHaveWarnedAboutDeprecatedLanes) {
                myHaveWarnedAboutDeprecatedLanes = true;
                WRITE_WARNING("'" + toString(SUMO_ATTR_LANE) + "' is deprecated, please use '" + toString(SUMO_ATTR_FROM_LANE) +
                              "' and '" + toString(SUMO_ATTR_TO_LANE) + "' instead.");
            }
            laneIndices = attrs.getStringReporting(SUMO_ATTR_LANE, 0, ok);
        } else {
            laneIndices = attrs.getStringReporting(SUMO_ATTR_FROM_LANE, 0, ok) + ":" + attrs.getStringReporting(SUMO_ATTR_TO_LANE, 0, ok);
        }
        LinkDirection dir = parseLinkDir(attrs.getStringReporting(SUMO_ATTR_DIR, 0, ok));
        LinkState state = parseLinkState(attrs.getStringReporting(SUMO_ATTR_STATE, 0, ok));
        std::string tlID = attrs.getOptStringReporting(SUMO_ATTR_TLID, 0, ok, "");
#ifdef HAVE_INTERNAL_LANES
        std::string viaID = attrs.getOptStringReporting(SUMO_ATTR_VIA, 0, ok, "");
        SUMOReal pass = attrs.getOptSUMORealReporting(SUMO_ATTR_PASS, 0, ok, -1);
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
        std::pair<MSLane*, MSLane*> lanes = getLanesFromIndices(from, to, laneIndices, ok);
        if (!ok) {
            return;
        }
        MSLane* fromLane = lanes.first;
        MSLane* toLane = lanes.second;
        assert(fromLane);
        assert(toLane);

        int tlLinkIdx;
        if (tlID != "") {
            tlLinkIdx = attrs.hasAttribute(SUMO_ATTR_TLLINKINDEX)
                        ? attrs.getIntReporting(SUMO_ATTR_TLLINKINDEX, 0, ok)
                        : attrs.getIntReporting(SUMO_ATTR_TLLINKNO__DEPRECATED, 0, ok);
            // make sure that the index is in range
            MSTrafficLightLogic* logic = myJunctionControlBuilder.getTLLogic(tlID).getActive();
            if (tlLinkIdx >= (int)logic->getCurrentPhaseDef().getState().size()) {
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
        if (pass >= 0) {
            static_cast<MSInternalLane*>(toLane)->setPassPosition(pass);
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
            MSTLLogicControl::TLSLogicVariants& logics = myJunctionControlBuilder.getTLLogic(tlID);
            logics.addLink(link, fromLane, tlLinkIdx);
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


std::pair<MSLane*, MSLane*>
NLHandler::getLanesFromIndices(MSEdge* from, MSEdge* to, const std::string& laneIndices, bool& ok) {
    std::string error = "Invalid attribute in connection from '" + from->getID() + "' to '" + to->getID() + "' ";
    StringTokenizer st(laneIndices, ':');
    if (st.size() == 2) {
        int fromLaneIdx;
        int toLaneIdx;
        try {
            fromLaneIdx = TplConvertSec<char>::_2intSec(st.next().c_str(), -1);
            toLaneIdx = TplConvertSec<char>::_2intSec(st.next().c_str(), -1);
            if (fromLaneIdx >= 0 && static_cast<unsigned int>(fromLaneIdx) < from->getLanes().size() &&
                    toLaneIdx >= 0 && static_cast<unsigned int>(toLaneIdx) < to->getLanes().size()) {
                return std::pair<MSLane*, MSLane*>(from->getLanes()[fromLaneIdx], to->getLanes()[toLaneIdx]);
            } else {
                error += "(invalid index)";
            }
        } catch (NumberFormatException&) {
            error += "(number format)";
        }
    } else {
        error += "(malformed)";
    }
    WRITE_ERROR(error);
    ok = false;
    return std::pair<MSLane*, MSLane*>(static_cast<MSLane*>(0), static_cast<MSLane*>(0));
}


// ----------------------------------
void
NLHandler::setLocation(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    PositionVector s = GeomConvHelper::parseShapeReporting(
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
        Position networkOffset = s[0];
        GeoConvHelper::init(proj, networkOffset, origBoundary, convBoundary);
    }
}


void
NLHandler::addDistrict(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    myCurrentIsBroken = false;
    // get the id, report an error if not given or empty...
    myCurrentDistrictID = attrs.getStringReporting(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        myCurrentIsBroken = true;
        return;
    }
    try {
        MSEdge* sink = myEdgeControlBuilder.buildEdge(myCurrentDistrictID + "-sink");
        if (!MSEdge::dictionary(myCurrentDistrictID + "-sink", sink)) {
            delete sink;
            throw InvalidArgument("Another edge with the id '" + myCurrentDistrictID + "-sink' exists.");
        }
        sink->initialize(0, MSEdge::EDGEFUNCTION_DISTRICT);
        MSEdge* source = myEdgeControlBuilder.buildEdge(myCurrentDistrictID + "-source");
        if (!MSEdge::dictionary(myCurrentDistrictID + "-source", source)) {
            delete source;
            throw InvalidArgument("Another edge with the id '" + myCurrentDistrictID + "-source' exists.");
        }
        source->initialize(0, MSEdge::EDGEFUNCTION_DISTRICT);
        if (attrs.hasAttribute(SUMO_ATTR_EDGES)) {
            std::vector<std::string> desc = StringTokenizer(attrs.getString(SUMO_ATTR_EDGES)).getVector();
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
    std::string id = attrs.getStringReporting(SUMO_ATTR_ID, myCurrentDistrictID.c_str(), ok);
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


// ----------------------------------


void
NLHandler::closeSuccLane() {
    // do not process internal lanes if not wished
    if (myCurrentIsInternalToSkip) {
        return;
    }
    try {
        mySucceedingLaneBuilder.closeSuccLane();
    } catch (InvalidArgument& e) {
        WRITE_ERROR(e.what());
    }
}


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
