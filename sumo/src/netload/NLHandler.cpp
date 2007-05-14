/****************************************************************************/
/// @file    NLHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// }
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
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#pragma warning(disable: 4503)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
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
#include <utils/xml/XMLBuildingExceptions.h>
#include <utils/common/StringTokenizer.h>
#include <utils/gfx/RGBColor.h>
#include <utils/gfx/GfxConvHelper.h>
#include <utils/geom/GeomConvHelper.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSBitSetLogic.h>
#include <microsim/MSJunctionLogic.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/output/MSInductLoop.h>
#include <microsim/output/e2_detectors/MSE2Collector.h>
#include <microsim/output/e2_detectors/MS_E2_ZS_CollectorOverLanes.h>
#include <microsim/traffic_lights/MSAgentbasedTrafficLightLogic.h>
#include <microsim/logging/LoggedValue_TimeFloating.h>
#include <utils/iodevices/SharedOutputDevices.h>
#include <utils/common/UtilExceptions.h>
#include <utils/geoconv/GeoConvHelper.h>
#include "NLLoadFilter.h"
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
                     bool wantsVehicleColor,
                     int incDUABase, int incDUAStage)
        : MSRouteHandler(file, net.getVehicleControl(), true, wantsVehicleColor, incDUABase, incDUAStage),
        myNet(net), myActionBuilder(net),
        myCurrentIsInternalToSkip(false),
        myDetectorBuilder(detBuilder), myTriggerBuilder(triggerBuilder),
        myEdgeControlBuilder(edgeBuilder), myJunctionControlBuilder(junctionBuilder),
        myShapeBuilder(shapeBuilder), m_pSLB(junctionBuilder),
        myAmInTLLogicMode(false)
{}


NLHandler::~NLHandler()
{}


void
NLHandler::myStartElement(SumoXMLTag element, const std::string &name,
                          const Attributes &attrs)
{
    // check static net information
    if (wanted(LOADFILTER_NET)) {
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
        default:
            break;
        }
    }
    // check junction logics
//    if(wanted(LOADFILTER_LOGICS)) {
    switch (element) {
    case SUMO_TAG_ROWLOGIC:
        myJunctionControlBuilder.initJunctionLogic();
        break;
    case SUMO_TAG_TLLOGIC:
        initTrafficLightLogic(attrs);
        break;
    case SUMO_TAG_LOGICITEM:
        addLogicItem(attrs);
        break;
    default:
        break;
    }
//    }
    // !!!
    if (name=="WAUT") {
        openWAUT(attrs);
    }
    if (name=="wautSwitch") {
        addWAUTSwitch(attrs);
    }
    if (name=="wautJunction") {
        addWAUTJunction(attrs);
    }
    // !!!!
    // process detectors when wished
    if (wanted(LOADFILTER_NETADD)) {
        switch (element) {
        case SUMO_TAG_DETECTOR:
            addDetector(attrs);
            break;
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
        case SUMO_TAG_SOURCE:
            addSource(attrs);
            break;
        case SUMO_TAG_TRIGGER:
            addTrigger(attrs);
            break;
        case SUMO_TAG_TIMEDEVENT:
            myActionBuilder.addAction(*this, attrs, _file);
            break;
        }
    }
    if (wanted(LOADFILTER_DYNAMIC)) {
        MSRouteHandler::myStartElement(element, name, attrs);
    }
    if (element==SUMO_TAG_PARAM) {
        addParam(attrs);
    }
}


void
NLHandler::addParam(const Attributes &attrs)
{
    string key, val;
    try {
        key = getString(attrs, SUMO_ATTR_KEY);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Error in description: missing key for a parameter.");
        return;
    }
    try {
        val = getString(attrs, SUMO_ATTR_VALUE);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Error in description: missing value for a parameter.");
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
NLHandler::openWAUT(const Attributes &attrs)
{
    SUMOTime t;
    std::string id, pro;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
    } catch (EmptyData&) {
        MsgHandler::getErrorInstance()->inform("Missing id for a WAUT (attribute 'id').");
        throw ProcessError();
    }
    try {
        t = getIntSecure(attrs, SUMO_ATTR_REF_TIME, 0);
    } catch (NumberFormatException&) {
        MsgHandler::getErrorInstance()->inform("The reference time for WAUT '" + id + "' is not numeric.");
        throw ProcessError();
    }
    try {
        pro = getString(attrs, SUMO_ATTR_START_PROG);
    } catch (EmptyData&) {
        MsgHandler::getErrorInstance()->inform("Missing start program for WAUT '" + id + "'.");
        throw ProcessError();
    }
    myCurrentWAUTID = id;
    myJunctionControlBuilder.addWAUT(t, id, pro);
}


void
NLHandler::addWAUTSwitch(const Attributes &attrs)
{
    SUMOTime t;
    std::string to;
    try {
        t = getInt(attrs, SUMO_ATTR_TIME);
    } catch (NumberFormatException&) {
        MsgHandler::getErrorInstance()->inform("The switch time for WAUT '" + myCurrentWAUTID + "' is not numeric.");
        throw ProcessError();
    } catch (EmptyData&) {
        MsgHandler::getErrorInstance()->inform("Missing switch time for WAUT '" + myCurrentWAUTID + "'.");
        throw ProcessError();
    }
    try {
        to = getString(attrs, SUMO_ATTR_TO);
    } catch (EmptyData&) {
        MsgHandler::getErrorInstance()->inform("Missing destination program for WAUT '" + myCurrentWAUTID + "'.");
        throw ProcessError();
    }
    myJunctionControlBuilder.addWAUTSwitch(myCurrentWAUTID, t, to);
}


void
NLHandler::addWAUTJunction(const Attributes &attrs)
{
    std::string wautID, junctionID, procedure;
    try {
        wautID = getString(attrs, SUMO_ATTR_WAUT_ID);
    } catch (EmptyData&) {
        MsgHandler::getErrorInstance()->inform("Missing WAUT id in wautJunction.");
        throw ProcessError();
    }
    try {
        junctionID = getString(attrs, SUMO_ATTR_JUNCTION_ID);
    } catch (EmptyData&) {
        MsgHandler::getErrorInstance()->inform("Missing junction id in wautJunction.");
        throw ProcessError();
    }
    procedure = getStringSecure(attrs, SUMO_ATTR_PROCEDURE, "");
    bool synchron = getBoolSecure(attrs, SUMO_ATTR_SYNCHRON, false);
    myJunctionControlBuilder.addWAUTJunction(wautID, junctionID, procedure, synchron);
}


void
NLHandler::setEdgeNumber(const Attributes &attrs)
{
    try {
        myEdgeControlBuilder.prepare(getInt(attrs, SUMO_ATTR_NO));
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Error in description: missing number of edges.");
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform("Error in description: non-digit number of edges.");
    }
}


void
NLHandler::chooseEdge(const Attributes &attrs)
{
    // get the id
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
        // omit internal edges if not wished
        if (!MSGlobals::gUsingInternalLanes&&id[0]==':') {
            myCurrentIsInternalToSkip = true;
            return;
        }
        myCurrentIsInternalToSkip = false;
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Error in description: missing id of an edge-object.");
    } catch (XMLIdNotKnownException &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    }
    // get the function
    string func;
    try {
        func = getString(attrs, SUMO_ATTR_FUNC);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Error in description: missing function of an edge-object.");
    }

    // get the type
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
        throw XMLIdNotKnownException("purpose", func);
    }
    //
    myEdgeControlBuilder.chooseEdge(id, funcEnum);
    // continuation
    myCurrentID = id;
}



void
NLHandler::addLaneShape(const std::string &chars)
{
    myShape = GeomConvHelper::parseShape(chars);
}


void
NLHandler::addLane(const Attributes &attrs)
{
    // omit internal edges if not wished
    if (myCurrentIsInternalToSkip) {
        return;
    }
    try {
        string id = getString(attrs, SUMO_ATTR_ID);
        try {
            myID = id;
            myLaneIsDepart = getBool(attrs, SUMO_ATTR_DEPART);
            myCurrentMaxSpeed = getFloat(attrs, SUMO_ATTR_MAXSPEED);
            myCurrentLength = getFloat(attrs, SUMO_ATTR_LENGTH);
            myVehicleClasses = getStringSecure(attrs, SUMO_ATTR_VCLASSES, "");
        } catch (XMLIdAlreadyUsedException &e) {
            MsgHandler::getErrorInstance()->inform(e.what());
        } catch (XMLDepartLaneDuplicationException &e) {
            MsgHandler::getErrorInstance()->inform(e.what());
        } catch (EmptyData) {
            MsgHandler::getErrorInstance()->inform("Error in description: missing attribute in an edge-object.");
        } catch (NumberFormatException) {
            MsgHandler::getErrorInstance()->inform("Error in description: one of an edge's attributes must be numeric but is not.");
        }
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Error in description: missing id of an edge-object.");
    }
}


void
NLHandler::addPOI(const Attributes &attrs)
{
    try {
        std::string name = getString(attrs, SUMO_ATTR_ID);
        try {
            myShapeBuilder.addPoint(name,
                                    getIntSecure(attrs, SUMO_ATTR_LAYER, 1),
                                    getStringSecure(attrs, SUMO_ATTR_TYPE, ""),
                                    GfxConvHelper::parseColor(getStringSecure(attrs, SUMO_ATTR_COLOR, "1,0,0")),
                                    getFloatSecure(attrs, SUMO_ATTR_X, INVALID_POSITION),
                                    getFloatSecure(attrs, SUMO_ATTR_Y, INVALID_POSITION),
                                    getStringSecure(attrs, SUMO_ATTR_LANE, ""),
                                    getFloatSecure(attrs, SUMO_ATTR_POS, INVALID_POSITION));
        } catch (XMLIdAlreadyUsedException &e) {
            MsgHandler::getErrorInstance()->inform(e.what());
        } catch (NumberFormatException &) {
            MsgHandler::getErrorInstance()->inform("The color of POI '" + name + "' could not be parsed.");
        } catch (EmptyData &) {
            MsgHandler::getErrorInstance()->inform("POI '" + name + "' misses an attribute.");
        }
    } catch (EmptyData&) {
        MsgHandler::getErrorInstance()->inform("Error in description: missing name of a POI-object.");
    }
}


void
NLHandler::addPoly(const Attributes &attrs)
{
    try {
        std::string name = getString(attrs, SUMO_ATTR_ID);
        try {
            myShapeBuilder.polygonBegin(name,
                                        getIntSecure(attrs, SUMO_ATTR_LAYER, -1),
                                        getStringSecure(attrs, SUMO_ATTR_TYPE, ""),
                                        GfxConvHelper::parseColor(getString(attrs, SUMO_ATTR_COLOR)),
                                        getBoolSecure(attrs, SUMO_ATTR_FILL, false));// !!!
        } catch (XMLIdAlreadyUsedException &e) {
            MsgHandler::getErrorInstance()->inform(e.what());
        } catch (NumberFormatException &) {
            MsgHandler::getErrorInstance()->inform("The color of polygon '" + name + "' could not be parsed.");
        } catch (EmptyData &) {
            MsgHandler::getErrorInstance()->inform("Polygon '" + name + "' misses an attribute.");
        }
    } catch (EmptyData&) {
        MsgHandler::getErrorInstance()->inform("Error in description: missing name of a poly-object.");
    }
}


void
NLHandler::openAllowedEdge(const Attributes &attrs)
{
    // omit internal edges if not wished
    if (myCurrentIsInternalToSkip) {
        return;
    }
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
        MSEdge *edge = MSEdge::dictionary(id);
        if (edge==0) {
            throw XMLIdNotKnownException("edge", id);
        }
        myEdgeControlBuilder.openAllowedEdge(edge);
        // continuation
        myContinuations.add(edge, myEdgeControlBuilder.getActiveEdge());
    } catch (XMLIdNotKnownException &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Error in description: missing id of an cedge-object.");
    }
}


void
NLHandler::addLogicItem(const Attributes &attrs)
{
    // parse the request
    int request = -1;
    try {
        request = getInt(attrs, SUMO_ATTR_REQUEST);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Missing request key...");
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform("Error in description: one of the request keys is not numeric.");
    }
    // parse the response
    string response;
    try {
        response = getString(attrs, SUMO_ATTR_RESPONSE);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Missing respond for a request");
    }
    // parse the internal links information (when wished)
    string foes;
    try {
        foes = getString(attrs, SUMO_ATTR_FOES);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Missing foes for a request");
    }
    bool cont = false;
#ifdef HAVE_INTERNAL_LANES
    if (MSGlobals::gUsingInternalLanes) {
        cont = getBoolSecure(attrs, SUMO_ATTR_CONT, false);
    }
#endif
    // store received information
    if (request>=0 && response.length()>0) {
        myJunctionControlBuilder.addLogicItem(request, response, foes, cont);
    }
}


void
NLHandler::initTrafficLightLogic(const Attributes &attrs)
{
    size_t absDuration = 0;
    int requestSize = -1;
    SUMOReal detectorOffset = -1;
    myJunctionControlBuilder.initIncomingLanes();
    try {
        string type = getString(attrs, SUMO_ATTR_TYPE);
        // get the detector offset
        {
            try {
                detectorOffset = getFloatSecure(attrs, SUMO_ATTR_DET_OFFSET, -1);
            } catch (NumberFormatException&) {
                MsgHandler::getErrorInstance()->inform(
                    "A detector offset of a traffic light logic is not numeric!");
            }
        }
        myJunctionControlBuilder.initTrafficLightLogic(type,
                absDuration, requestSize, detectorOffset);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Missing traffic light type.");
    }
    myAmInTLLogicMode = true;
}


void
NLHandler::addPhase(const Attributes &attrs)
{
    // try to get the phase definition
    string phase;
    try {
        phase = getString(attrs, SUMO_ATTR_PHASE);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Missing phase definition.");
        return;
    }
    // try to get the break definition
    string brakeMask;
    try {
        brakeMask = getString(attrs, SUMO_ATTR_BRAKE);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Missing break definition.");
        return;
    }
    // try to get the yellow definition
    string yellowMask;
    try {
        yellowMask = getString(attrs, SUMO_ATTR_YELLOW);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Missing yellow definition.");
        return;
    }
    // try to get the phase duration
    size_t duration;
    try {
        duration = getInt(attrs, SUMO_ATTR_DURATION);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Missing phase duration...");
        return;
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform("The phase duration is not numeric.");
        return;
    }
    if (duration==0) {
        MsgHandler::getErrorInstance()->inform("The duration of a tls-logic must not be zero. Is in '" + m_Key + "'.");
        return;
    }
    // if the traffic light is an actuated traffic light, try to get
    //  the minimum and maximum durations
    int min = duration;
    int max = duration;
    try {
        min = getIntSecure(attrs, SUMO_ATTR_MINDURATION, -1);
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform("The phase minimum duration is not numeric.");
        return;
    }
    try {
        max = getIntSecure(attrs, SUMO_ATTR_MAXDURATION, -1);
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform("The phase maximum duration is not numeric.");
        return;
    }
    // build the brake mask
    std::bitset<64> prios(brakeMask);
    prios.flip();
    myJunctionControlBuilder.addPhase(duration, std::bitset<64>(phase),
                                      prios, std::bitset<64>(yellowMask), min, max);
}


void
NLHandler::openJunction(const Attributes &attrs)
{
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
        try {
            myJunctionControlBuilder.openJunction(id,
                                                  getStringSecure(attrs, SUMO_ATTR_KEY, ""),
                                                  getString(attrs, SUMO_ATTR_TYPE),
                                                  getFloat(attrs, SUMO_ATTR_X),
                                                  getFloat(attrs, SUMO_ATTR_Y));
        } catch (EmptyData) {
            MsgHandler::getErrorInstance()->inform("Error in description: missing attribute in a junction-object.");
        }
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            "Error in description: missing id of a junction-object.");
    }
}



void
NLHandler::addDetector(const Attributes &attrs)
{
    // try to get the id first
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Error in description: missing id of a detector-object.");
        return;
    }
    // try to get the type
    string type = getStringSecure(attrs, SUMO_ATTR_TYPE, "induct_loop");
    // build in dependence to type
    // induct loops (E1-detectors)
    if (type=="induct_loop"||type=="E1"||type=="e1") {
        addE1Detector(attrs);
        myDetectorType = "e1";
        return;
    }
    // lane-based areal detectors (E2-detectors)
    if (type=="lane_based"||type=="E2"||type=="e2") {
        addE2Detector(attrs);
        myDetectorType = "e2";
        return;
    }
    // multi-origin/multi-destination detectors (E3-detectors)
    if (type=="multi_od"||type=="E3"||type=="e3") {
        beginE3Detector(attrs);
        myDetectorType = "e3";
        return;
    }
}


void
NLHandler::addE1Detector(const Attributes &attrs)
{
    // try to get the id first
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Error in description: missing id of a detector-object.");
        return;
    }

    // check whether it is a detector storing data to a file or sending it over the network
    bool isfd= true;
    try {
        getString(attrs, SUMO_ATTR_FILE);
    } catch (EmptyData) {
        isfd= false;
    }

    try {
#ifdef USE_SOCKETS
        if (isfd) {
            // this detector stores it's data in a file
            myDetectorBuilder.buildInductLoop(id,
                                              getString(attrs, SUMO_ATTR_LANE),
                                              getFloat(attrs, SUMO_ATTR_POSITION),
                                              getInt(attrs, SUMO_ATTR_SPLINTERVAL),
                                              SharedOutputDevices::getInstance()->getOutputDeviceChecking(
                                                  _file, getString(attrs, SUMO_ATTR_FILE)),
                                              getBoolSecure(attrs, SUMO_ATTR_FRIENDLY_POS, false),
                                              getStringSecure(attrs, SUMO_ATTR_STYLE, ""));
        } else {
            // this detector sends it's data to some host on the network
            myDetectorBuilder.buildInductLoop(id,
                                              getString(attrs, SUMO_ATTR_LANE),
                                              getFloat(attrs, SUMO_ATTR_POSITION),
                                              getInt(attrs, SUMO_ATTR_SPLINTERVAL),
                                              SharedOutputDevices::getInstance()->getOutputDevice(getString(attrs, SUMO_ATTR_HOST),
                                                      getInt(attrs, SUMO_ATTR_PORT),
                                                      getString(attrs, SUMO_ATTR_PROTOCOL)),
                                              getBoolSecure(attrs, SUMO_ATTR_FRIENDLY_POS, false),
                                              getStringSecure(attrs, SUMO_ATTR_STYLE, ""));
        }
#else //#ifdef USE_SOCKETS
        myDetectorBuilder.buildInductLoop(id,
                                          getString(attrs, SUMO_ATTR_LANE),
                                          getFloat(attrs, SUMO_ATTR_POSITION),
                                          getInt(attrs, SUMO_ATTR_SPLINTERVAL),
                                          SharedOutputDevices::getInstance()->getOutputDeviceChecking(
                                              _file, getString(attrs, SUMO_ATTR_FILE)),
                                          getBoolSecure(attrs, SUMO_ATTR_FRIENDLY_POS, false),
                                          getStringSecure(attrs, SUMO_ATTR_STYLE, ""));
#endif //#ifdef USE_SOCKETS
    } catch (XMLBuildingException &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("The description of the detector '" + id + "' does not contain a needed value.");
    } catch (FileBuildError &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (NetworkError &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    }
}


void
NLHandler::addE2Detector(const Attributes &attrs)
{
    // try to get the id first
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Error in description: missing id of a detector-object.");
        return;
    }
    // check whether this is a lsa-based detector or one that uses a sample
    //  interval
    MSTLLogicControl::TLSLogicVariants tll;
    try {
        string lsaid = getString(attrs, SUMO_ATTR_TLID);
        tll = myJunctionControlBuilder.getTLLogic(lsaid);
        if (tll.ltVariants.size()==0) {
            MsgHandler::getErrorInstance()->inform("The detector '" + id + "' refers to the unknown lsa '" + lsaid + "'.");
            return;
        }
    } catch (EmptyData) {}
    // check whether this is a detector connected to a link
    std::string toLane = getStringSecure(attrs, SUMO_ATTR_TO, "");
    //
    try {
        if (tll.ltVariants.size()!=0) {
            if (toLane.length()==0) {
                myDetectorBuilder.buildE2Detector(myContinuations,
                                                  id,
                                                  getString(attrs, SUMO_ATTR_LANE),
                                                  getFloat(attrs, SUMO_ATTR_POSITION),
                                                  getFloat(attrs, SUMO_ATTR_LENGTH),
                                                  getBoolSecure(attrs, SUMO_ATTR_CONT, false),
                                                  tll,
                                                  getStringSecure(attrs, SUMO_ATTR_STYLE, ""),
                                                  SharedOutputDevices::getInstance()->getOutputDeviceChecking(
                                                      _file, getString(attrs, SUMO_ATTR_FILE)),
                                                  getStringSecure(attrs, SUMO_ATTR_MEASURES, "ALL"),
                                                  getFloatSecure(attrs, SUMO_ATTR_HALTING_TIME_THRESHOLD, 1.0f),
                                                  getFloatSecure(attrs, SUMO_ATTR_HALTING_SPEED_THRESHOLD, 5.0f/3.6f),
                                                  getFloatSecure(attrs, SUMO_ATTR_JAM_DIST_THRESHOLD, 10.0f),
                                                  GET_XML_SUMO_TIME_SECURE(attrs, SUMO_ATTR_DELETE_DATA_AFTER_SECONDS, 1800)
                                                 );
            } else {
                myDetectorBuilder.buildE2Detector(myContinuations,
                                                  id,
                                                  getString(attrs, SUMO_ATTR_LANE),
                                                  getFloat(attrs, SUMO_ATTR_POSITION),
                                                  getFloat(attrs, SUMO_ATTR_LENGTH),
                                                  getBoolSecure(attrs, SUMO_ATTR_CONT, false),
                                                  tll, toLane,
                                                  getStringSecure(attrs, SUMO_ATTR_STYLE, ""),
                                                  SharedOutputDevices::getInstance()->getOutputDeviceChecking(
                                                      _file, getString(attrs, SUMO_ATTR_FILE)),
                                                  getStringSecure(attrs, SUMO_ATTR_MEASURES, "ALL"),
                                                  getFloatSecure(attrs, SUMO_ATTR_HALTING_TIME_THRESHOLD, 1.0f),
                                                  getFloatSecure(attrs, SUMO_ATTR_HALTING_SPEED_THRESHOLD, 5.0f/3.6f),
                                                  getFloatSecure(attrs, SUMO_ATTR_JAM_DIST_THRESHOLD, 10.0f),
                                                  GET_XML_SUMO_TIME_SECURE(attrs, SUMO_ATTR_DELETE_DATA_AFTER_SECONDS, 1800)
                                                 );
            }
        } else {
            myDetectorBuilder.buildE2Detector(myContinuations,
                                              id,
                                              getString(attrs, SUMO_ATTR_LANE),
                                              getFloat(attrs, SUMO_ATTR_POSITION),
                                              getFloat(attrs, SUMO_ATTR_LENGTH),
                                              getBoolSecure(attrs, SUMO_ATTR_CONT, false),
                                              getInt(attrs, SUMO_ATTR_SPLINTERVAL),
                                              getStringSecure(attrs, SUMO_ATTR_STYLE, ""),
                                              SharedOutputDevices::getInstance()->getOutputDeviceChecking(
                                                  _file, getString(attrs, SUMO_ATTR_FILE)),
                                              getStringSecure(attrs, SUMO_ATTR_MEASURES, "ALL"),
                                              getFloatSecure(attrs, SUMO_ATTR_HALTING_TIME_THRESHOLD, 1.0f),
                                              getFloatSecure(attrs, SUMO_ATTR_HALTING_SPEED_THRESHOLD, 5.0f/3.6f),
                                              getFloatSecure(attrs, SUMO_ATTR_JAM_DIST_THRESHOLD, 10.0f),
                                              GET_XML_SUMO_TIME_SECURE(attrs, SUMO_ATTR_DELETE_DATA_AFTER_SECONDS, 1800)
                                             );
        }
    } catch (XMLBuildingException &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("The description of the detector '" + id + "' does not contain a needed value.");
    } catch (FileBuildError &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    }
}


void
NLHandler::beginE3Detector(const Attributes &attrs)
{
    // try to get the id first
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
        m_Key = id;
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Error in description: missing id of a detector-object.");
        return;
    }
    try {
        myDetectorBuilder.beginE3Detector(id,
                                          SharedOutputDevices::getInstance()->getOutputDeviceChecking(
                                              _file, getString(attrs, SUMO_ATTR_FILE)),
                                          getInt(attrs, SUMO_ATTR_SPLINTERVAL),
                                          getStringSecure(attrs, SUMO_ATTR_MEASURES, "ALL"),
                                          getFloatSecure(attrs, SUMO_ATTR_HALTING_TIME_THRESHOLD, 1.0f),
                                          getFloatSecure(attrs, SUMO_ATTR_HALTING_SPEED_THRESHOLD, 5.0f/3.6f));
    } catch (XMLBuildingException &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("The description of the detector '" + id + "' does not contain a needed value.");
    } catch (FileBuildError &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    }
}


void
NLHandler::addE3Entry(const Attributes &attrs)
{
    try {
        myDetectorBuilder.addE3Entry(
            getString(attrs, SUMO_ATTR_LANE),
            getFloat(attrs, SUMO_ATTR_POSITION));
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("The description of the detector '" + m_Key + "' does not contain a needed value.");
    }
}


void
NLHandler::addE3Exit(const Attributes &attrs)
{
    try {
        myDetectorBuilder.addE3Exit(
            getString(attrs, SUMO_ATTR_LANE),
            getFloat(attrs, SUMO_ATTR_POSITION));
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("The description of the detector '" + m_Key + "' does not contain a needed value.");
    } catch (FileBuildError &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    }
}





void
NLHandler::addSource(const Attributes &attrs)
{
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
        try {
            myTriggerBuilder.buildTrigger(
                myNet, attrs, _file, *this);
            return;
        } catch (XMLBuildingException &e) {
            MsgHandler::getErrorInstance()->inform(e.what());
        } catch (InvalidArgument &e) {
            MsgHandler::getErrorInstance()->inform(e.what());
        } catch (EmptyData) {
            MsgHandler::getErrorInstance()->inform("The description of trigger '" + id + "' does not contain a needed value.");
        }
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Error in description: missing id of a detector-object.");
    } catch (FileBuildError &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    }
}


void
NLHandler::addTrigger(const Attributes &attrs)
{
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
        try {
            myTriggerBuilder.buildTrigger(myNet, attrs, _file, *this);
            return;
        } catch (XMLBuildingException &e) {
            MsgHandler::getErrorInstance()->inform(e.what());
        } catch (InvalidArgument &e) {
            MsgHandler::getErrorInstance()->inform(e.what());
        } catch (EmptyData) {
            MsgHandler::getErrorInstance()->inform("The description of the trigger '" + id + "' does not contain a needed value.");
        } catch (FileBuildError &e) {
            MsgHandler::getErrorInstance()->inform(e.what());
        }
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Error in description: missing id of a trigger-object.");
    }
}


void
NLHandler::openSucc(const Attributes &attrs)
{
    try {
        string id = getString(attrs, SUMO_ATTR_LANE);
        if (!MSGlobals::gUsingInternalLanes&&id[0]==':') {
            myCurrentIsInternalToSkip = true;
            return;
        }
        myCurrentIsInternalToSkip = false;
        m_pSLB.openSuccLane(id);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Error in description: missing id of a succ-object.");
    }
}

void
NLHandler::addSuccLane(const Attributes &attrs)
{
    // do not process internal lanes if not wished
    if (myCurrentIsInternalToSkip) {
        return;
    }
    try {
        string tlID = getStringSecure(attrs, SUMO_ATTR_TLID, "");
        if (tlID!="") {
            m_pSLB.addSuccLane(
                getBool(attrs, SUMO_ATTR_YIELD),
                getString(attrs, SUMO_ATTR_LANE),
#ifdef HAVE_INTERNAL_LANES
                getStringSecure(attrs, SUMO_ATTR_VIA, ""),
                getFloatSecure(attrs, SUMO_ATTR_PASS, -1),
#endif
                parseLinkDir(getString(attrs, SUMO_ATTR_DIR)[0]),
                parseLinkState(getString(attrs, SUMO_ATTR_STATE)[0]),
                getBoolSecure(attrs, SUMO_ATTR_INTERNALEND, false),
                tlID, getInt(attrs, SUMO_ATTR_TLLINKNO));
        } else {
            m_pSLB.addSuccLane(
                getBool(attrs, SUMO_ATTR_YIELD),
                getString(attrs, SUMO_ATTR_LANE),
#ifdef HAVE_INTERNAL_LANES
                getStringSecure(attrs, SUMO_ATTR_VIA, ""),
                getFloatSecure(attrs, SUMO_ATTR_PASS, -1),
#endif
                parseLinkDir(getString(attrs, SUMO_ATTR_DIR)[0]),
                parseLinkState(getString(attrs, SUMO_ATTR_STATE)[0]),
                getBoolSecure(attrs, SUMO_ATTR_INTERNALEND, false));
        }
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Error in description: missing attribute in a succlane-object.");
    } catch (XMLIdNotKnownException &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
        MsgHandler::getErrorInstance()->inform("While building lane '" + m_pSLB.getSuccingLaneName() + "'");
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform("Something is wrong with the definition of a link");
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
        throw NumberFormatException();
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
        throw NumberFormatException();
    }
}



// ----------------------------------


void
NLHandler::myCharacters(SumoXMLTag element, const std::string &name,
                        const std::string &chars)
{
    // check static net information
    if (wanted(LOADFILTER_NET)) {
        switch (element) {
        case SUMO_TAG_EDGES:
            allocateEdges(chars);
            break;
        case SUMO_TAG_CEDGE:
            addAllowedEdges(chars);
            break;
        case SUMO_TAG_POLY:
            addPolyPosition(chars);
            break;
        case SUMO_TAG_NODECOUNT:
            setNodeNumber(chars);
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
            /*
            default:
            break;
            }
            }
            // check junction logics
            if(wanted(LOADFILTER_LOGICS)) {
            switch(element) {
            */
        case SUMO_TAG_REQUESTSIZE:
            if (m_Key.length()!=0) {
                setRequestSize(chars);
            }
            break;
        case SUMO_TAG_LANENUMBER:
            if (m_Key.length()!=0) {
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
        default:
            break;
        }
        if (name=="net-offset") { // !!!!6 change to tag*
            setNetOffset(chars);
        }
        if (name=="conv-boundary") { // !!!!6 change to tag*
            setNetConv(chars);
        }
        if (name=="orig-boundary") { // !!!!6 change to tag*
            setNetOrig(chars);
        }
        if (name=="orig-proj") { // !!!!6 change to tag*
            GeoConvHelper::init(chars, myNetworkOffset, myOrigBoundary, myConvBoundary);
        }
    }
    if (wanted(LOADFILTER_DYNAMIC)) {
        MSRouteHandler::myCharacters(element, name, chars);
    }
}


void
NLHandler::allocateEdges(const std::string &chars)
{
    size_t beg = 0;
    size_t idx = chars.find(' ');
    while (idx!=string::npos) {
        string edgeid = chars.substr(beg, idx-beg);
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
    myEdgeControlBuilder.addEdge(edgeid);
}


void
NLHandler::setNodeNumber(const std::string &chars)
{
    try {
        myJunctionControlBuilder.prepare(TplConvert<char>::_2int(chars.c_str()));
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Error in description: missing number of nodes.");
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform("Error in description: non-digit number of nodes.");
    }
}


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
                throw XMLIdNotKnownException("lane", set);
            }
            myEdgeControlBuilder.addAllowed(lane);
        } catch (XMLIdNotKnownException &e) {
            MsgHandler::getErrorInstance()->inform(e.what());
        } catch (XMLInvalidChildException &e) {
            MsgHandler::getErrorInstance()->inform(e.what());
        }
    }
}


void
NLHandler::setRequestSize(const std::string &chars)
{
    try {
        myJunctionControlBuilder.setRequestSize(TplConvert<char>::_2int(chars.c_str()));
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Missing request size.");
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform("Error in description: one of an edge's attributes must be numeric but is not.");
    }
}


void
NLHandler::setLaneNumber(const std::string &chars)
{
    try {
        myJunctionControlBuilder.setLaneNumber(TplConvert<char>::_2int(chars.c_str()));
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Missing lane number.");
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform("Error in description: one of an edge's attributes must be numeric but is not.");
    }
}


void
NLHandler::setKey(const std::string &chars)
{
    if (chars.length()==0) {
        MsgHandler::getErrorInstance()->inform("No key given for the current junction logic.");
        return;
    }
    m_Key = chars;
    myJunctionControlBuilder.setKey(m_Key);
}


void
NLHandler::setSubKey(const std::string &chars)
{
    if (chars.length()==0) {
        MsgHandler::getErrorInstance()->inform("No subkey given for the current junction logic.");
        return;
    }
    m_Key = chars;
    myJunctionControlBuilder.setSubKey(m_Key);
}


void
NLHandler::setOffset(const std::string &chars)
{
    try {
        myJunctionControlBuilder.setOffset(TplConvertSec<char>::_2intSec(chars.c_str(), 0));
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform("Invalid offset for a junction.");
        return;
    }
}


void
NLHandler::setNetOffset(const std::string &chars)
{
    try {
        Position2DVector s = GeomConvHelper::parseShape(chars);
        myNetworkOffset = s[0];
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform("Invalid network offset.");
        return;
    }
}


void
NLHandler::setNetConv(const std::string &chars)
{
    try {
        myConvBoundary = GeomConvHelper::parseBoundary(chars);
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform("Invalid converted network boundary.");
        return;
    }
}


void
NLHandler::setNetOrig(const std::string &chars)
{
    try {
        myOrigBoundary = GeomConvHelper::parseBoundary(chars);
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform("Invalid original network boundary.");
        return;
    }
}



void
NLHandler::addIncomingLanes(const std::string &chars)
{
    StringTokenizer st(chars);
    while (st.hasNext()) {
        string set = st.next();
        try {
            MSLane *lane = MSLane::dictionary(set);
            if (!MSGlobals::gUsingInternalLanes&&set[0]==':') {
                continue;
            }
            if (lane==0) {
                throw XMLIdNotKnownException("lane", set);
            }
            myJunctionControlBuilder.addIncomingLane(lane);
        } catch (XMLIdNotKnownException &e) {
            MsgHandler::getErrorInstance()->inform(e.what());
        }
    }
}

//-----------------------------------------------------------------------------------


void
NLHandler::addPolyPosition(const std::string &chars)
{
    myShapeBuilder.polygonEnd(GeomConvHelper::parseShape(chars));
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
        try {
            MSLane *lane = MSLane::dictionary(set);
            if (lane==0) {
                throw XMLIdNotKnownException("lane", set);
            }
            myJunctionControlBuilder.addInternalLane(lane);
        } catch (XMLIdNotKnownException &e) {
            MsgHandler::getErrorInstance()->inform(e.what());
        }
    }
}
#endif

// ----------------------------------

void
NLHandler::myEndElement(SumoXMLTag element, const std::string &name)
{
    if (wanted(LOADFILTER_NET)) {
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
        }
    }
    if (wanted(LOADFILTER_NET)) {
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
    }
    // !!!
    if (name=="WAUT") {
        closeWAUT();
    }
    // !!!!
    if (wanted(LOADFILTER_NETADD)) {
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
    }
    if (wanted(LOADFILTER_DYNAMIC)) {
        MSRouteHandler::myEndElement(element, name);
    }
}


void
NLHandler::closeEdge()
{
    // do not process internal lanes if not wished
    if (!myCurrentIsInternalToSkip) {
        MSEdge *edge = myEdgeControlBuilder.closeEdge();
#ifdef HAVE_MESOSIM
        if (MSGlobals::gUseMesoSim) {
            MSGlobals::gMesoNet->buildSegmentsFor(edge, *(MSNet::getInstance()), OptionsSubSys::getOptions());
        }
#endif
    }
}


void
NLHandler::closeLane()
{
    // do not process internal lanes if not wished
    if (!myCurrentIsInternalToSkip) {
        MSLane *lane =
            myEdgeControlBuilder.addLane(myID, myCurrentMaxSpeed, myCurrentLength, myLaneIsDepart, myShape, myVehicleClasses);
        // insert the lane into the lane-dictionary, checking
        if (!MSLane::dictionary(myID, lane)) {
            throw XMLIdAlreadyUsedException("Lanes", myID);
        }
    }
}

void
NLHandler::closeLanes()
{
    myEdgeControlBuilder.closeLanes();
}


void
NLHandler::closeAllowedEdge()
{
    // do not process internal lanes if not wished
    if (!myCurrentIsInternalToSkip) {
        myEdgeControlBuilder.closeAllowedEdge();
    }
}



void
NLHandler::closeJunction()
{
    try {
        myJunctionControlBuilder.closeJunction();
    } catch (XMLIdAlreadyUsedException &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (XMLIdNotKnownException &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    }
}



void
NLHandler::closeSuccLane()
{
    // do not process internal lanes if not wished
    if (myCurrentIsInternalToSkip) {
        return;
    }
    try {
        m_pSLB.closeSuccLane();
    } catch (XMLIdNotKnownException &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    }
}


void
NLHandler::endDetector()
{
    if (myDetectorType=="e3") {
        endE3Detector();
    }
    myDetectorType = "";
}


void
NLHandler::endE3Detector()
{
    try {
        myDetectorBuilder.endE3Detector();
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
    } catch (ProcessError) {}
}


void
NLHandler::closeWAUT()
{
    myCurrentWAUTID = "";
}






std::string
NLHandler::getMessage() const
{
    return "Loading routes, lanes and vehicle types...";
}


bool
NLHandler::wanted(NLLoadFilter filter) const
{
    return (_filter&filter)!=0;
}


void
NLHandler::setWanted(NLLoadFilter filter)
{
    _filter = filter;
}


const MSEdgeContinuations &
NLHandler::getContinuations() const
{
    return myContinuations;
}



/****************************************************************************/

