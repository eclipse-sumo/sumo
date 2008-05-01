/****************************************************************************/
/// @file    NLHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// The XML-Handler for network loading
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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
#include <microsim/logging/LoggedValue_TimeFloating.h>
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
// using namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
NLHandler::NLHandler(const std::string &file, MSNet &net,
                     NLDetectorBuilder &detBuilder,
                     NLTriggerBuilder &triggerBuilder,
                     NLEdgeControlBuilder &edgeBuilder,
                     NLJunctionControlBuilder &junctionBuilder,
                     NLGeomShapeBuilder &shapeBuilder,
                     int incDUABase, int incDUAStage) throw()
        : MSRouteHandler(file, net.getVehicleControl(), true, incDUABase, incDUAStage),
        myNet(net), myActionBuilder(net),
        myCurrentIsInternalToSkip(false),
        myDetectorBuilder(detBuilder), myTriggerBuilder(triggerBuilder),
        myEdgeControlBuilder(edgeBuilder), myJunctionControlBuilder(junctionBuilder),
        myShapeBuilder(shapeBuilder), mySucceedingLaneBuilder(junctionBuilder),
        myAmInTLLogicMode(false), myCurrentIsBroken(false)
{}


NLHandler::~NLHandler() throw()
{}


void
NLHandler::myStartElement(SumoXMLTag element,
                          const SUMOSAXAttributes &attrs) throw(ProcessError)
{
    switch (element) {
    case SUMO_TAG_EDGES:
        setEdgeNumber(attrs);
        break;
    case SUMO_TAG_EDGE:
        chooseEdge(attrs);
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
    case SUMO_TAG_CEDGE:
        openAllowedEdge(attrs);
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
        myJunctionControlBuilder.initJunctionLogic();
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
    case SUMO_TAG_VTYPEPROBE:
        addVTypeProbeDetector(attrs);
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
    default:
        break;
    }
    MSRouteHandler::myStartElement(element, attrs);
    if (element==SUMO_TAG_PARAM) {
        addParam(attrs);
    }
}


void
NLHandler::myCharacters(SumoXMLTag element,
                        const std::string &chars) throw(ProcessError)
{
    // check static net information
        switch (element) {
        case SUMO_TAG_EDGES:
            allocateEdges(chars);
            break;
        case SUMO_TAG_NODECOUNT:
            setNodeNumber(chars);
            break;
        case SUMO_TAG_CEDGE:
            addAllowedEdges(chars);
            break;
        case SUMO_TAG_POLY:
            addPolyPosition(chars);
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
            addLaneShape(chars);
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
        default:
            break;
        }
        MSRouteHandler::myCharacters(element, chars);
}


void
NLHandler::myEndElement(SumoXMLTag element) throw(ProcessError)
{
        switch (element) {
        case SUMO_TAG_EDGE:
            closeEdge();
            break;
        case SUMO_TAG_LANES:
            closeLanes();
            break;
        case SUMO_TAG_LANE:
            closeLane();
            break;
        case SUMO_TAG_CEDGE:
            closeAllowedEdge();
            break;
        case SUMO_TAG_JUNCTION:
            closeJunction();
            break;
        case SUMO_TAG_SUCC:
            closeSuccLane();
            break;
        default:
            break;
        }
        switch (element) {
        case SUMO_TAG_ROWLOGIC:
            myJunctionControlBuilder.closeJunctionLogic();
            break;
        case SUMO_TAG_TLLOGIC:
            myJunctionControlBuilder.closeTrafficLightLogic();
            myAmInTLLogicMode = false;
            break;
        default:
            break;
        }
    // !!!
    if (element==SUMO_TAG_WAUT) {
        closeWAUT();
    }
        switch (element) {
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


// ---- the root/edges - element
void
NLHandler::setEdgeNumber(const SUMOSAXAttributes &attrs)
{
    try {
        myEdgeControlBuilder.prepare(attrs.getInt( SUMO_ATTR_NO));
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Missing number of edges.");
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("The number of edges is not numeric.");
    }
}


void
NLHandler::allocateEdges(const std::string &chars)
{
    size_t found = 0;
    size_t wanted = myEdgeControlBuilder.getEdgeCapacity();
    try {
        size_t beg = 0;
        size_t idx = chars.find(' ');
        while (idx!=string::npos) {
            string edgeid = chars.substr(beg, idx-beg);
            found++;
            // skip internal edges if not wished
            if (MSGlobals::gUsingInternalLanes||edgeid[0]!=':') {
                myEdgeControlBuilder.addEdge(edgeid);
            }
            beg = idx + 1;
            idx = chars.find(' ', beg);
        }
        string edgeid = chars.substr(beg);
        // skip internal edges if not wished
        //  (the last one shouldn't be internal anyway)
        if (!MSGlobals::gUsingInternalLanes&&edgeid[0]==':') {
            return;
        }
        found++;
        myEdgeControlBuilder.addEdge(edgeid);
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    }
    if (wanted!=found) {
        throw ProcessError("The number of edges in the list mismatches the edge count.");
    }
}
// ----


// ---- the root/node_count - element
void
NLHandler::setNodeNumber(const std::string &chars)
{
    try {
        myJunctionControlBuilder.prepare(TplConvert<char>::_2int(chars.c_str()));
    } catch (EmptyData &) {
        throw ProcessError("Missing number of nodes.");
    } catch (NumberFormatException &) {
        throw ProcessError("The number of nodes is not numeric.");
    }
}
// ----


// ---- the root/edge - element
void
NLHandler::chooseEdge(const SUMOSAXAttributes &attrs)
{
    myCurrentIsBroken = false;
    // get the id, report an error if not given or empty...
    string id;
    if(!attrs.setIDFromAttribues("edge", id)) {
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
    string func;
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
    }
    if (func=="source") {
        funcEnum = MSEdge::EDGEFUNCTION_SOURCE;
    }
    if (func=="sink") {
        funcEnum = MSEdge::EDGEFUNCTION_SINK;
    }
    if (func=="internal") {
        funcEnum = MSEdge::EDGEFUNCTION_INTERNAL;
    }
    if (funcEnum<0) {
        MsgHandler::getErrorInstance()->inform("Edge '" + id + "' has an invalid type ('" + func + "').");
        myCurrentIsBroken = true;
        return;
    }
    // check is inner
    bool isInner = attrs.getBoolSecure( SUMO_ATTR_INNER, false);
    //
    try {
        myEdgeControlBuilder.chooseEdge(id, funcEnum, isInner);
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
        myCurrentIsBroken = true;
    }
}


void
NLHandler::closeEdge()
{
    // omit internal edges if not wished and broken edges
    if (myCurrentIsInternalToSkip||myCurrentIsBroken) {
        return;
    }
    try {
        MSEdge *edge = myEdgeControlBuilder.closeEdge();
#ifdef HAVE_MESOSIM
        if (MSGlobals::gUseMesoSim) {
            MSGlobals::gMesoNet->buildSegmentsFor(edge, *(MSNet::getInstance()), OptionsCont::getOptions());
        }
#endif
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    }
}


//       ---- the root/edge/lanes - element
void
NLHandler::closeLanes()
{
    myEdgeControlBuilder.closeLanes();
}


//             ---- the root/edge/lanes/lane - element
void
NLHandler::addLane(const SUMOSAXAttributes &attrs)
{
    // omit internal edges if not wished and broken edges
    if (myCurrentIsInternalToSkip||myCurrentIsBroken) {
        return;
    }
    // get the id, report an error if not given or empty...
    string id;
    if(!attrs.setIDFromAttribues("lane", id)) {
        myCurrentIsBroken = true;
        return;
    }
    try {
        myCurrentLaneID = id;
        myLaneIsDepart = attrs.getBool( SUMO_ATTR_DEPART);
        myCurrentMaxSpeed = attrs.getFloat( SUMO_ATTR_MAXSPEED);
        myCurrentLength = attrs.getFloat( SUMO_ATTR_LENGTH);
        myVehicleClasses = attrs.getStringSecure(SUMO_ATTR_VCLASSES, "");
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
}


void
NLHandler::addLaneShape(const std::string &chars)
{
    // omit internal edges if not wished and broken edges
    if (myCurrentIsInternalToSkip||myCurrentIsBroken) {
        return;
    }
    myShape.clear();
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
NLHandler::closeLane()
{
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
            myEdgeControlBuilder.addLane(myCurrentLaneID, myCurrentMaxSpeed, myCurrentLength, myLaneIsDepart, myShape, myVehicleClasses);
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


//       ---- the root/edge/cedge - element
void
NLHandler::openAllowedEdge(const SUMOSAXAttributes &attrs)
{
    // omit internal edges if not wished and broken edges
    if (myCurrentIsInternalToSkip||myCurrentIsBroken) {
        return;
    }
    // get the id, report an error if not given or empty...
    string id;
    if(!attrs.setIDFromAttribues("cedge", id)) {
        return;
    }
    MSEdge *edge = MSEdge::dictionary(id);
    if (edge==0) {
        MsgHandler::getErrorInstance()->inform("Trying to reference an unknown edge ('" + id + "') in edge '" + myEdgeControlBuilder.getActiveEdge()->getID() + "'.");
        myCurrentIsBroken = true;
        return;
    }
    myEdgeControlBuilder.openAllowedEdge(edge);
    // continuation
    myContinuations.add(edge, myEdgeControlBuilder.getActiveEdge());
}


void
NLHandler::closeAllowedEdge()
{
    // omit internal edges if not wished and broken edges
    if (myCurrentIsInternalToSkip||myCurrentIsBroken) {
        return;
    }
    myEdgeControlBuilder.closeAllowedEdge();
}
// ----


// ---- the root/junction - element
void
NLHandler::openJunction(const SUMOSAXAttributes &attrs)
{
    myCurrentIsBroken = false;
    // get the id, report an error if not given or empty...
    string id;
    if(!attrs.setIDFromAttribues("junction", id)) {
        myCurrentIsBroken = true;
        return;
    }
    try {
        myJunctionControlBuilder.openJunction(id,
            attrs.getStringSecure(SUMO_ATTR_KEY, ""),
            attrs.getString(SUMO_ATTR_TYPE),
            attrs.getFloat( SUMO_ATTR_X),
            attrs.getFloat( SUMO_ATTR_Y));
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Missing attribute in junction '" + id + "'.\n Can not build according junction.");
        myCurrentIsBroken = true;
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what() + string("\n Can not build according junction."));
        myCurrentIsBroken = true;
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("Position of junction '" + id + "' is not numeric.\n Can not build according junction.");
        myCurrentIsBroken = true;
    }
}


void
NLHandler::closeJunction()
{
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
NLHandler::addParam(const SUMOSAXAttributes &attrs)
{
    string key, val;
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
NLHandler::openWAUT(const SUMOSAXAttributes &attrs)
{
    myCurrentIsBroken = false;
    SUMOTime t;
    // get the id, report an error if not given or empty...
    string id;
    if(!attrs.setIDFromAttribues("waut", id)) {
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
            myJunctionControlBuilder.addWAUT(t, id, pro);
        } catch (InvalidArgument &e) {
            MsgHandler::getErrorInstance()->inform(e.what());
            myCurrentIsBroken = true;
        }
    }
}


void
NLHandler::addWAUTSwitch(const SUMOSAXAttributes &attrs)
{
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
            myJunctionControlBuilder.addWAUTSwitch(myCurrentWAUTID, t, to);
        } catch (InvalidArgument &e) {
            MsgHandler::getErrorInstance()->inform(e.what());
            myCurrentIsBroken = true;
        }
    }
}


void
NLHandler::addWAUTJunction(const SUMOSAXAttributes &attrs)
{
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
        bool synchron = attrs.getBoolSecure( SUMO_ATTR_SYNCHRON, false);
        if (!myCurrentIsBroken) {
            myJunctionControlBuilder.addWAUTJunction(wautID, junctionID, procedure, synchron);
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
NLHandler::addPOI(const SUMOSAXAttributes &attrs)
{
    // get the id, report an error if not given or empty...
    string id;
    if(!attrs.setIDFromAttribues("poi", id)) {
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
NLHandler::addPoly(const SUMOSAXAttributes &attrs)
{
    // get the id, report an error if not given or empty...
    string id;
    if(!attrs.setIDFromAttribues("poly", id)) {
        return;
    }
    try {
        myShapeBuilder.polygonBegin(id,
            attrs.getIntSecure(SUMO_ATTR_LAYER, -1),
            attrs.getStringSecure(SUMO_ATTR_TYPE, ""),
            RGBColor::parseColor(attrs.getString( SUMO_ATTR_COLOR)),
            attrs.getBoolSecure( SUMO_ATTR_FILL, false));
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("The color of polygon '" + id + "' could not be parsed.");
    } catch (BoolFormatException &) {
        MsgHandler::getErrorInstance()->inform("The attribute 'fill' of polygon '" + id + "' is not a valid bool.");
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Polygon '" + id + "' misses an attribute.");
    }
}


void
NLHandler::addLogicItem(const SUMOSAXAttributes &attrs)
{
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
    string response;
    try {
        response = attrs.getString(SUMO_ATTR_RESPONSE);
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Missing respond for a request");
        return;
    }
    // parse the internal links information (when wished)
    string foes;
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
            cont = attrs.getBoolSecure( SUMO_ATTR_CONT, false);
        } catch (BoolFormatException &) {
            MsgHandler::getErrorInstance()->inform("The definition whether a link is a cont-link is not a valid bool.");
            return;
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
NLHandler::initTrafficLightLogic(const SUMOSAXAttributes &attrs)
{
    size_t absDuration = 0;
    int requestSize = -1;
    SUMOReal detectorOffset = -1;
    myJunctionControlBuilder.initIncomingLanes();
    try {
        string type = attrs.getString(SUMO_ATTR_TYPE);
        // get the detector offset
        {
            try {
                detectorOffset = attrs.getFloatSecure(SUMO_ATTR_DET_OFFSET, -1);
            } catch (EmptyData&) {
                MsgHandler::getErrorInstance()->inform("A detector offset of a traffic light logic is empty.");
            } catch (NumberFormatException&) {
                MsgHandler::getErrorInstance()->inform("A detector offset of a traffic light logic is not numeric.");
                return;
            }
        }
        myJunctionControlBuilder.initTrafficLightLogic(type,
                absDuration, requestSize, detectorOffset);
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Missing traffic light type.");
        return;
    }
    myAmInTLLogicMode = true;
}


void
NLHandler::addPhase(const SUMOSAXAttributes &attrs)
{
    // try to get the phase definition
    string phase;
    try {
        phase = attrs.getString(SUMO_ATTR_PHASE);
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Missing phase definition.");
        return;
    }
    // try to get the break definition
    string brakeMask;
    try {
        brakeMask = attrs.getString(SUMO_ATTR_BRAKE);
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Missing break definition.");
        return;
    }
    // try to get the yellow definition
    string yellowMask;
    try {
        yellowMask = attrs.getString(SUMO_ATTR_YELLOW);
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Missing yellow definition.");
        return;
    }
    // try to get the phase duration
    size_t duration;
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
    int min = duration;
    int max = duration;
    try {
        min = attrs.getIntSecure(SUMO_ATTR_MINDURATION, -1);
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("The phase minimum duration is empty.");
        return;
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("The phase minimum duration is not numeric.");
        return;
    }
    try {
        max = attrs.getIntSecure(SUMO_ATTR_MAXDURATION, -1);
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("The phase maximum duration is empty.");
        return;
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("The phase maximum duration is not numeric.");
        return;
    }
    // build the brake mask
    std::bitset<64> prios(brakeMask);
    prios.flip();
    myJunctionControlBuilder.addPhase(duration, std::bitset<64>(phase),
                                      prios, std::bitset<64>(yellowMask), min, max);
}


#ifdef _MESSAGES
void
NLHandler::addMsgEmitter(const SUMOSAXAttributes& attrs)
{
	string id;
	try {
		id = attrs.getString(SUMO_ATTR_ID);
	} catch (EmptyData &) {
		MsgHandler::getErrorInstance()->inform("Missing id of a message emitter object.");
		return;
	}
	string file = attrs.getStringSecure(SUMO_ATTR_FILE, "");
	// if no file given, use stdout
	if (file=="") {
		file = "-";
	}
#ifdef _DEBUG
	cout << "id: '"+ id + "'" << endl
		 << "file: '" + file + "'" << endl
		 << "getFileName(): '" + getFileName() + "'" << endl;
#endif
	string whatemit;
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
NLHandler::addDetector(const SUMOSAXAttributes &attrs)
{
    // get the id, report an error if not given or empty...
    string id;
    if(!attrs.setIDFromAttribues("detector", id)) {
        return;
    }
    // try to get the type
    string type = attrs.getStringSecure(SUMO_ATTR_TYPE, "induct_loop");
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
NLHandler::addMsgDetector(const SUMOSAXAttributes &attrs)
{
#ifdef _DEBUG
	cout << "=====DEBUG OUTPUT=====" << endl << "Hier kommen die Detektoren rein..." << endl;
#endif
	string id = attrs.getStringSecure(SUMO_ATTR_ID, "");
	if(id=="") {
		MsgHandler::getErrorInstance()->inform("Missing id of a e4-detector-object.");
		return;
	}
#ifdef _DEBUG
	cout << "ID: " << id << endl;
#endif
	string file = attrs.getStringSecure(SUMO_ATTR_FILE, "");
	if (file=="") {
		MsgHandler::getErrorInstance()->inform("Missing output definition for detector '" + id + "'.");
		return;
	}
#ifdef _DEBUG
	cout << "File: " << file << endl;
#endif
	string msg = attrs.getStringSecure(SUMO_ATTR_MSG, "");
	if (msg=="") {
		MsgHandler::getErrorInstance()->inform("Missing message for detector '" + id + "'.");
		return;
	}
#ifdef _DEBUG
	cout << "Message: " << msg << endl;
#endif
	try {
		myDetectorBuilder.buildMsgDetector(id,
										   attrs.getString(SUMO_ATTR_LANE),
										   attrs.getFloat(SUMO_ATTR_POSITION),
										   attrs.getInt(SUMO_ATTR_FREQUENCY),
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
#ifdef _DEBUG
	cout << "=====END DEBUG=====" << endl;
#endif
}
#endif


void
NLHandler::addE1Detector(const SUMOSAXAttributes &attrs)
{
    // get the id, report an error if not given or empty...
    string id;
    if(!attrs.setIDFromAttribues("e1-detector", id)) {
        return;
    }
    string file = attrs.getStringSecure(SUMO_ATTR_FILE, "");
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
                                          attrs.getFloat( SUMO_ATTR_POSITION),
                                          attrs.getInt(SUMO_ATTR_FREQUENCY),
                                          OutputDevice::getDevice(attrs.getString( SUMO_ATTR_FILE), getFileName()),
                                          attrs.getBoolSecure( SUMO_ATTR_FRIENDLY_POS, false));
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
NLHandler::addVTypeProbeDetector(const SUMOSAXAttributes &attrs)
{
    // get the id, report an error if not given or empty...
    string id;
    if(!attrs.setIDFromAttribues("vtypeprobe", id)) {
        return;
    }
    string file = attrs.getStringSecure(SUMO_ATTR_FILE, "");
    if (file=="") {
        MsgHandler::getErrorInstance()->inform("Missing output definition for vtypeprobe '" + id + "'.");
        return;
    }
    try {
        myDetectorBuilder.buildVTypeProbe(id,
                                          attrs.getStringSecure(SUMO_ATTR_TYPE, ""),
                                          attrs.getInt(SUMO_ATTR_FREQUENCY),
                                          OutputDevice::getDevice(attrs.getString( SUMO_ATTR_FILE), getFileName()));
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
NLHandler::addE2Detector(const SUMOSAXAttributes &attrs)
{
    // get the id, report an error if not given or empty...
    string id;
    if(!attrs.setIDFromAttribues("e2-detector", id)) {
        return;
    }
    // check whether this is a detector connected to a tls an optionally to a link
    std::string lsaid = attrs.getStringSecure(SUMO_ATTR_TLID, "<invalid>");
    std::string toLane = attrs.getStringSecure(SUMO_ATTR_TO, "<invalid>");
    // get the file name; it should not be empty
    string file = attrs.getStringSecure(SUMO_ATTR_FILE, "");
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
                                                  attrs.getFloat( SUMO_ATTR_POSITION),
                                                  attrs.getFloat( SUMO_ATTR_LENGTH),
                                                  attrs.getBoolSecure( SUMO_ATTR_CONT, false),
                                                  myJunctionControlBuilder.getTLLogic(lsaid),
                                                  OutputDevice::getDevice(file, getFileName()),
                                                  (SUMOTime) attrs.getFloatSecure(SUMO_ATTR_HALTING_TIME_THRESHOLD, 1.0f),
                                                  attrs.getFloatSecure(SUMO_ATTR_HALTING_SPEED_THRESHOLD, 5.0f/3.6f),
                                                  attrs.getFloatSecure(SUMO_ATTR_JAM_DIST_THRESHOLD, 10.0f),
                                                  attrs.getBoolSecure( SUMO_ATTR_FRIENDLY_POS, false)
                                                 );
            } else {
                myDetectorBuilder.buildE2Detector(myContinuations,
                                                  id,
                                                  attrs.getString(SUMO_ATTR_LANE),
                                                  attrs.getFloat( SUMO_ATTR_POSITION),
                                                  attrs.getFloat( SUMO_ATTR_LENGTH),
                                                  attrs.getBoolSecure( SUMO_ATTR_CONT, false),
                                                  myJunctionControlBuilder.getTLLogic(lsaid), toLane,
                                                  OutputDevice::getDevice(file, getFileName()),
                                                  (SUMOTime) attrs.getFloatSecure(SUMO_ATTR_HALTING_TIME_THRESHOLD, 1.0f),
                                                  attrs.getFloatSecure(SUMO_ATTR_HALTING_SPEED_THRESHOLD, 5.0f/3.6f),
                                                  attrs.getFloatSecure(SUMO_ATTR_JAM_DIST_THRESHOLD, 10.0f),
                                                  attrs.getBoolSecure( SUMO_ATTR_FRIENDLY_POS, false)
                                                 );
            }
        } else {
            myDetectorBuilder.buildE2Detector(myContinuations,
                                              id,
                                              attrs.getString(SUMO_ATTR_LANE),
                                              attrs.getFloat( SUMO_ATTR_POSITION),
                                              attrs.getFloat( SUMO_ATTR_LENGTH),
                                              attrs.getBoolSecure( SUMO_ATTR_CONT, false),
                                              attrs.getInt(SUMO_ATTR_FREQUENCY),
                                              OutputDevice::getDevice(file, getFileName()),
                                              (SUMOTime) attrs.getFloatSecure(SUMO_ATTR_HALTING_TIME_THRESHOLD, 1.0f),
                                              attrs.getFloatSecure(SUMO_ATTR_HALTING_SPEED_THRESHOLD, 5.0f/3.6f),
                                              attrs.getFloatSecure(SUMO_ATTR_JAM_DIST_THRESHOLD, 10.0f),
                                              attrs.getBoolSecure( SUMO_ATTR_FRIENDLY_POS, false)
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
NLHandler::beginE3Detector(const SUMOSAXAttributes &attrs)
{
    // get the id, report an error if not given or empty...
    string id;
    if(!attrs.setIDFromAttribues("e3-detector", id)) {
        return;
    }
    // get the file name; it should not be empty
    string file = attrs.getStringSecure(SUMO_ATTR_FILE, "");
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
NLHandler::addE3Entry(const SUMOSAXAttributes &attrs)
{
    try {
        myDetectorBuilder.addE3Entry(
            attrs.getString(SUMO_ATTR_LANE),
            attrs.getFloat( SUMO_ATTR_POSITION),
            attrs.getBoolSecure( SUMO_ATTR_FRIENDLY_POS, false));
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("Position of an entry of detector '" + myDetectorBuilder.getCurrentE3ID() + "' is not numeric.");
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("The description of an entry of the detector '" + myDetectorBuilder.getCurrentE3ID() + "' does not contain a needed value.");
    }
}


void
NLHandler::addE3Exit(const SUMOSAXAttributes &attrs)
{
    try {
        myDetectorBuilder.addE3Exit(
            attrs.getString(SUMO_ATTR_LANE),
            attrs.getFloat( SUMO_ATTR_POSITION),
            attrs.getBoolSecure( SUMO_ATTR_FRIENDLY_POS, false));
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("Position of an exit of detector '" + myDetectorBuilder.getCurrentE3ID() + "' is not numeric.");
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("The description of an exit of the detector '" + myDetectorBuilder.getCurrentE3ID() + "' does not contain a needed value.");
    }
}





void
NLHandler::addSource(const SUMOSAXAttributes &attrs)
{
    // get the id, report an error if not given or empty...
    string id;
    if(!attrs.setIDFromAttribues("source", id)) {
        return;
    }
    try {
        myTriggerBuilder.buildTrigger(myNet, attrs, getFileName());
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("The description of trigger '" + id + "' does not contain a needed value.");
    } catch (IOError &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    }
}


void
NLHandler::addTrigger(const SUMOSAXAttributes &attrs)
{
    // get the id, report an error if not given or empty...
    string id;
    if(!attrs.setIDFromAttribues("trigger", id)) {
        return;
    }
    try {
        myTriggerBuilder.buildTrigger(myNet, attrs, getFileName());
        return;
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("The description of the trigger '" + id + "' does not contain a needed value.");
    } catch (IOError &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    }
}


void
NLHandler::openSucc(const SUMOSAXAttributes &attrs)
{
    try {
        string id = attrs.getString(SUMO_ATTR_LANE);
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
NLHandler::addSuccLane(const SUMOSAXAttributes &attrs)
{
    // do not process internal lanes if not wished
    if (myCurrentIsInternalToSkip) {
        return;
    }
    try {
        string tlID = attrs.getStringSecure(SUMO_ATTR_TLID, "");
        if (tlID!="") {
            mySucceedingLaneBuilder.addSuccLane(
                attrs.getBool( SUMO_ATTR_YIELD),
                attrs.getString(SUMO_ATTR_LANE),
#ifdef HAVE_INTERNAL_LANES
                attrs.getStringSecure(SUMO_ATTR_VIA, ""),
                attrs.getFloatSecure(SUMO_ATTR_PASS, -1),
#endif
                parseLinkDir(attrs.getString( SUMO_ATTR_DIR)[0]),
                parseLinkState(attrs.getString( SUMO_ATTR_STATE)[0]),
                attrs.getBoolSecure( SUMO_ATTR_INTERNALEND, false),
                tlID, attrs.getInt(SUMO_ATTR_TLLINKNO));
        } else {
            mySucceedingLaneBuilder.addSuccLane(
                attrs.getBool( SUMO_ATTR_YIELD),
                attrs.getString(SUMO_ATTR_LANE),
#ifdef HAVE_INTERNAL_LANES
                attrs.getStringSecure(SUMO_ATTR_VIA, ""),
                attrs.getFloatSecure(SUMO_ATTR_PASS, -1),
#endif
                parseLinkDir(attrs.getString( SUMO_ATTR_DIR)[0]),
                parseLinkState(attrs.getString( SUMO_ATTR_STATE)[0]),
                attrs.getBoolSecure( SUMO_ATTR_INTERNALEND, false));
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
NLHandler::parseLinkDir(char dir)
{
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
NLHandler::parseLinkState(char state)
{
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
NLHandler::addAllowedEdges(const std::string &chars)
{
    // omit internal edges if not wished
    if (myCurrentIsInternalToSkip) {
        return;
    }
    StringTokenizer st(chars);
    while (st.hasNext()) {
        string set = st.next();
        try {
            MSLane *lane = MSLane::dictionary(set);
            if (lane==0) {
                MsgHandler::getErrorInstance()->inform("Trying to add an unknown lane ('" + set + "') as continuation.");
                return;
            }
            myEdgeControlBuilder.addAllowed(lane);
        } catch (InvalidArgument &e) {
            MsgHandler::getErrorInstance()->inform(e.what());
        }
    }
}


void
NLHandler::setRequestSize(const std::string &chars)
{
    try {
        myJunctionControlBuilder.setRequestSize(TplConvert<char>::_2int(chars.c_str()));
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Missing request size.");
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("One of an edge's SUMOSAXAttributes must be numeric but is not.");
    }
}


void
NLHandler::setLaneNumber(const std::string &chars)
{
    try {
        myJunctionControlBuilder.setLaneNumber(TplConvert<char>::_2int(chars.c_str()));
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Missing lane number.");
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("One of an edge's SUMOSAXAttributes must be numeric but is not.");
    }
}


void
NLHandler::setKey(const std::string &chars)
{
    if (chars.length()==0) {
        MsgHandler::getErrorInstance()->inform("No key given for the current junction logic.");
        return;
    }
    myJunctionControlBuilder.setKey(chars);
}


void
NLHandler::setSubKey(const std::string &chars)
{
    if (chars.length()==0) {
        MsgHandler::getErrorInstance()->inform("No subkey given for the current junction logic.");
        return;
    }
    myJunctionControlBuilder.setSubKey(chars);
}


void
NLHandler::setOffset(const std::string &chars)
{
    try {
        myJunctionControlBuilder.setOffset(TplConvertSec<char>::_2intSec(chars.c_str(), 0));
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("The offset for a junction is not numeric.");
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("The offset for a junction is not empty.");
    } // !!! can chars have length 0?
}


void
NLHandler::setNetOffset(const std::string &chars)
{
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
NLHandler::setNetConv(const std::string &chars)
{
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
NLHandler::setNetOrig(const std::string &chars)
{
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
NLHandler::addIncomingLanes(const std::string &chars)
{
    StringTokenizer st(chars);
    while (st.hasNext()) {
        string set = st.next();
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

//-----------------------------------------------------------------------------------


void
NLHandler::addPolyPosition(const std::string &chars)
{
    try {
        myShapeBuilder.polygonEnd(GeomConvHelper::parseShape(chars));
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    }
}


#ifdef HAVE_INTERNAL_LANES
void
NLHandler::addInternalLanes(const std::string &chars)
{
    // do not parse internal lanes if not wished
    if (!MSGlobals::gUsingInternalLanes) {
        return;
    }
    StringTokenizer st(chars);
    while (st.hasNext()) {
        string set = st.next();
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
NLHandler::closeSuccLane()
{
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
NLHandler::endDetector()
{
    if (myCurrentDetectorType=="e3") {
        endE3Detector();
    }
    myCurrentDetectorType = "";
}


void
NLHandler::endE3Detector()
{
    try {
        myDetectorBuilder.endE3Detector();
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    }
}


void
NLHandler::closeWAUT()
{
    myCurrentWAUTID = "";
}


const MSEdgeContinuations &
NLHandler::getContinuations() const
{
    return myContinuations;
}



/****************************************************************************/
