/***************************************************************************
                          NLNetHandler.cpp
              The third-step - handler building structures
                             -------------------
    project              : SUMO
    begin                : Mon, 9 Jul 2001
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
     const char rcsid[] = "$Id$";
}
// $Log$
// Revision 1.49  2004/11/23 10:12:46  dkrajzew
// new detectors usage applied
//
// Revision 1.48  2004/07/02 09:37:31  dkrajzew
// work on class derivation (for online-routing mainly)
//
// Revision 1.47  2004/06/17 13:08:15  dkrajzew
// Polygon visualisation added
//
// Revision 1.46  2004/04/02 11:23:52  dkrajzew
// extended traffic lights are now no longer templates; MSNet now handles all simulation-wide output
//
// Revision 1.45  2004/02/18 05:32:51  dkrajzew
// missing pass of lane continuation to detector builder added
//
// Revision 1.44  2004/02/16 13:49:08  dkrajzew
// loading of e2-link-dependent detectors added
//
// Revision 1.43  2004/02/02 16:18:32  dkrajzew
// a first try to patch the error on loading internal links when
//  they are not wished
//
// Revision 1.42  2004/01/27 10:32:25  dkrajzew
// patched some linux-warnings
//
// Revision 1.41  2004/01/26 11:07:50  dkrajzew
// error checking added
//
// Revision 1.40  2004/01/26 07:07:36  dkrajzew
// work on detectors: e3-detectors loading and visualisation;
//  variable offsets and lengths for lsa-detectors;
//  coupling of detectors to tl-logics;
//  different detector visualistaion in dependence to his controller
//
// Revision 1.39  2004/01/13 14:28:46  dkrajzew
// added alternative detector description; debugging
//
// Revision 1.38  2004/01/12 15:12:05  dkrajzew
// more wise definition of lane predeccessors implemented
//
// Revision 1.37  2004/01/12 14:46:21  dkrajzew
// handling of e2-detectors within the gui added
//
// Revision 1.36  2004/01/12 14:37:32  dkrajzew
// reading of e2-detectors from files added
//
// Revision 1.35  2003/12/05 10:26:10  dkrajzew
// handling of internal links when theyre not wished improved
//
// Revision 1.34  2003/12/04 13:18:23  dkrajzew
// handling of internal links added
//
// Revision 1.33  2003/12/04 13:14:08  dkrajzew
// gfx-module added temporary to sumo
//
// Revision 1.32  2003/12/02 21:48:27  roessel
// Renaming of MS_E2_ZS_ to MSE2 and MS_E3_ to MSE3.
//
// Revision 1.31  2003/11/26 09:35:03  dkrajzew
// special case of being unset (==-1) applied to min/max of actuated/agentbase
//  phase definitions
//
// Revision 1.30  2003/11/24 14:33:40  dkrajzew
// missing iterator initialisation failed
//
// Revision 1.29  2003/11/24 10:18:32  dkrajzew
// handling of definitions for minimum and maximum phase duration added;
//  modified the gld-offsets computation
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <sax/HandlerBase.hpp>
#include <sax/AttributeList.hpp>
#include <sax/SAXParseException.hpp>
#include <sax/SAXException.hpp>
#include "NLContainer.h"
#include "NLNetHandler.h"
#include "NLDetectorBuilder.h"
#include "NLSourceBuilder.h"
#include "NLTriggerBuilder.h"
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <utils/common/MsgHandler.h>
#include <utils/convert/TplConvert.h>
#include <utils/convert/TplConvertSec.h>
#include <utils/convert/STRConvert.h>
#include <utils/xml/XMLBuildingExceptions.h>
#include <utils/common/StringTokenizer.h>
#include <utils/xml/AttributesHandler.h>
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
#include <microsim/MSLaneState.h>
#include <microsim/traffic_lights/MSAgentbasedTrafficLightLogic.h>
#include <microsim/logging/LoggedValue_TimeFloating.h>
#include <utils/iodevices/SharedOutputDevices.h>
#include <utils/common/UtilExceptions.h>
#include "NLLoadFilter.h"


/* =========================================================================
 * using namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NLNetHandler::NLNetHandler(const std::string &file,
                           NLContainer &container,
                           NLDetectorBuilder &detBuilder,
                           NLTriggerBuilder &triggerBuilder,
                           double stdDetectorPositions,
                           double stdDetectorLengths,
                           int stdLearnHorizon, int stdDecisionHorizon,
                           double stdDeltaLimit, int stdTCycle)
    : MSRouteHandler(file, true),
    myContainer(container), _tlLogicNo(-1), m_Offset(0),
    myCurrentIsInternalToSkip(false),
    myStdDetectorPositions(stdDetectorPositions),
    myStdDetectorLengths(stdDetectorLengths),
    myDetectorBuilder(detBuilder), myTriggerBuilder(triggerBuilder),
    myStdLearnHorizon(stdLearnHorizon), myStdDecisionHorizon(stdDecisionHorizon),
    myStdDeltaLimit(stdDeltaLimit), myStdTCycle(stdTCycle)

{
}


NLNetHandler::~NLNetHandler()
{
}


void
NLNetHandler::myStartElement(int element, const std::string &name,
                             const Attributes &attrs)
{
    // check static net information
    if(wanted(LOADFILTER_NET)) {
        switch(element) {
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
        case SUMO_TAG_CEDGE:
            openAllowedEdge(attrs);
            break;
        case SUMO_TAG_JUNCTION:
            openJunction(attrs);
            addJunctionKey(attrs);
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
    if(wanted(LOADFILTER_LOGICS)) {
        switch(element) {
        case SUMO_TAG_ROWLOGIC:
            initJunctionLogic();
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
    }
    // process detectors when wished
    if(wanted(LOADFILTER_NETADD)) {
        switch(element) {
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
            myActionBuilder.addAction(attrs, _file);
            break;
        }
    }
    if(wanted(LOADFILTER_DYNAMIC)) {
        MSRouteHandler::myStartElement(element, name, attrs);
    }
}


void
NLNetHandler::setEdgeNumber(const Attributes &attrs)
{
    try {
        myContainer.setEdgeNumber(getInt(attrs, SUMO_ATTR_NO));
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            string("Error in description: missing number of edges."));
    } catch(NumberFormatException) {
        MsgHandler::getErrorInstance()->inform(
            string("Error in description: non-digit number of edges."));
    }
}


void
NLNetHandler::chooseEdge(const Attributes &attrs)
{
    // get the id
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
        // omit internal edges if not wished
        if(!MSGlobals::gUsingInternalLanes&&id[0]==':') {
            myCurrentIsInternalToSkip = true;
            return;
        }
        myCurrentIsInternalToSkip = false;
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            string("Error in description: missing id of an edge-object."));
    } catch (XMLIdNotKnownException &e) {
        MsgHandler::getErrorInstance()->inform(
            e.getMessage("edge", id));
    }
    // get the function
    string func;
    try {
        func = getString(attrs, SUMO_ATTR_FUNC);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            "Error in description: missing function of an edge-object.");
    } catch (XMLIdNotKnownException &e) {
        MsgHandler::getErrorInstance()->inform(
            e.getMessage("edge", id));
    }
    myContainer.chooseEdge(id, func);
}


void
NLNetHandler::addLane(const Attributes &attrs)
{
    // omit internal edges if not wished
    if(myCurrentIsInternalToSkip) {
        return;
    }
    try {
        string id = getString(attrs, SUMO_ATTR_ID);
        try {
            myContainer.addLane(id,
                getBool(attrs, SUMO_ATTR_DEPART),
                getFloat(attrs, SUMO_ATTR_MAXSPEED),
                getFloat(attrs, SUMO_ATTR_LENGTH),
                getFloat(attrs, SUMO_ATTR_CHANGEURGE));
        } catch (XMLInvalidParentException &e) {
            MsgHandler::getErrorInstance()->inform(e.getMessage("lane", id));
        } catch (XMLIdAlreadyUsedException &e) {
            MsgHandler::getErrorInstance()->inform(e.getMessage("lane", id));
        } catch (XMLDepartLaneDuplicationException &e) {
            MsgHandler::getErrorInstance()->inform(e.getMessage("lane", id));
        } catch (EmptyData) {
            MsgHandler::getErrorInstance()->inform(
                "Error in description: missing attribute in an edge-object.");
        } catch (NumberFormatException) {
            MsgHandler::getErrorInstance()->inform(
                "Error in description: one of an edge's attributes must be numeric but is not.");
        }
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            "Error in description: missing id of an edge-object.");
    }
}

/// fügt den Polygon hinzu
void
NLNetHandler::addPoly(const Attributes &attrs)
{
    try {
        std::string name = getString(attrs, SUMO_ATTR_NAME);
        actuell_poly_name = name;
        try {
            myContainer.addPoly(name,
                getString(attrs, SUMO_ATTR_TYPE),
                getString(attrs, SUMO_ATTR_COLOR));

        } catch (XMLIdAlreadyUsedException &e) {
            MsgHandler::getErrorInstance()->inform(e.getMessage("polygon", name));
        }
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            "Error in description: missing name of an poly-object.");
    }

}


void
NLNetHandler::openAllowedEdge(const Attributes &attrs)
{
    // omit internal edges if not wished
    if(myCurrentIsInternalToSkip) {
        return;
    }
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
        myContainer.openAllowedEdge(id);
    } catch (XMLIdNotKnownException &e) {
        MsgHandler::getErrorInstance()->inform(e.getMessage("cedge", id));
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            "Error in description: missing id of an cedge-object.");
    }
}


void
NLNetHandler::addJunctionKey(const Attributes &attrs)
{
    try {
        string key = getString(attrs, SUMO_ATTR_KEY);
        myContainer.addKey(key); // !!! wozu?
    } catch (EmptyData) {
    }
}


void
NLNetHandler::initJunctionLogic()
{
    m_Key = "";
    m_pActiveLogic = new MSBitsetLogic::Logic();
    m_pActiveFoes = new MSBitsetLogic::Foes();
    _requestSize = -1;
    _laneNo = -1;
    _requestItems = 0;
}


void
NLNetHandler::addLogicItem(const Attributes &attrs)
{
    if(_requestSize>0) {
        // parse the request
        int request = -1;
        try {
            request = getInt(attrs, SUMO_ATTR_REQUEST);
        } catch (EmptyData) {
            MsgHandler::getErrorInstance()->inform("Missing request key...");
        } catch (NumberFormatException) {
            MsgHandler::getErrorInstance()->inform(
                "Error in description: one of the request keys is not numeric.");
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
        if(MSGlobals::gUsingInternalLanes) {
            try {
                foes = getString(attrs, SUMO_ATTR_FOES);
            } catch (EmptyData) {
                MsgHandler::getErrorInstance()->inform("Missing foes for a request");
            }
        }
        // store received information
        if(request>=0 && response.length()>0) {
            addLogicItem(request, response, foes);
        }
    } else {
        MsgHandler::getErrorInstance()->inform(
            "The request size,  the response size or the number of lanes is not given! Contact your net supplier");
    }
}


void
NLNetHandler::initTrafficLightLogic(const Attributes &attrs)
{
    m_Key = "";
    m_ActivePhases.clear();
    myAbsDuration = 0;
    _requestSize = -1;
    _tlLogicNo = -1;
    myContainer.initIncomingLanes();
    try {
        m_Type = getString(attrs, SUMO_ATTR_TYPE);
        try {
            m_DetectorOffset = getFloatSecure(attrs, SUMO_ATTR_DET_OFFSET, -1);
        } catch (NumberFormatException&) {
            MsgHandler::getErrorInstance()->inform(
                "A detector offset of a triffic light logic is not numeric!");
        }
        // recheck the offset in dependence to the type if not given
        if(m_DetectorOffset==-1) {
            // agentbased
            if(m_Type=="agentbased") {
                m_DetectorOffset = myStdDetectorLengths;
            }
            // actuated
            if(m_Type=="actuated") {
                m_DetectorOffset = myStdDetectorPositions;
            }
        }
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Missing traffic light type.");
    }
}


void
NLNetHandler::addPhase(const Attributes &attrs)
{
    if(_tlLogicNo!=0) {
        return;
    }
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
        MsgHandler::getErrorInstance()->inform(
            "The phase duration is not numeric.");
        return;
    }
    if(duration==0) {
        MsgHandler::getErrorInstance()->inform(
            string("The duration of a tls-logic must not be zero. Is in '")
            + m_Key + string("'."));
        return;
    }
    // if the traffic light is an actuated traffic light, try to get
    //  the minimum and maximum durations
    size_t min = duration;
    size_t max = duration;
    try {
        if(m_Type=="actuated"||m_Type=="agentbased") {
            min = getIntSecure(attrs, SUMO_ATTR_MINDURATION, -1);
            max = getIntSecure(attrs, SUMO_ATTR_MAXDURATION, -1);
        }
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform(
            "The phase minimum or masimum duration is not numeric.");
        return;
    }
    // build the brake mask
    std::bitset<64> prios(brakeMask);
    prios.flip();
    // build and add the phase definition to the list
    if(m_Type=="actuated"||m_Type=="agentbased") {
        // for a controlled tls-logic
        m_ActivePhases.push_back(
            new MSActuatedPhaseDefinition(
                duration, std::bitset<64>(phase),
                prios, std::bitset<64>(yellowMask),
                min, max));
    } else {
        // for an controlled tls-logic
        m_ActivePhases.push_back(
            new MSPhaseDefinition(
                duration, std::bitset<64>(phase),
                prios, std::bitset<64>(yellowMask)));
    }
    // add phase duration to the absolute duration
    myAbsDuration += duration;
}


void
NLNetHandler::openJunction(const Attributes &attrs)
{
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
        try {
            myContainer.openJunction(id,
                getStringSecure(attrs, SUMO_ATTR_KEY, ""),
                getString(attrs, SUMO_ATTR_TYPE),
                getFloat(attrs, SUMO_ATTR_X),
                getFloat(attrs, SUMO_ATTR_Y));
        } catch (EmptyData) {
            MsgHandler::getErrorInstance()->inform(
                "Error in description: missing attribute in a junction-object.");
        }
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            "Error in description: missing id of a junction-object.");
    }
}


void
NLNetHandler::addDetector(const Attributes &attrs)
{
    // try to get the id first
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            "Error in description: missing id of a detector-object.");
        return;
    }
    // try to get the type
    string type = getStringSecure(attrs, SUMO_ATTR_TYPE, "induct_loop");
    // build in dependence to type
        // induct loops (E1-detectors)
    if(type=="induct_loop"||type=="E1"||type=="e1") {
        addE1Detector(attrs);
        myDetectorType = "e1";
        return;
    }
        // lane-based areal detectors (E2-detectors)
    if(type=="lane_based"||type=="E2"||type=="e2") {
        addE2Detector(attrs);
        myDetectorType = "e2";
        return;
    }
        // multi-origin/multi-destination detectors (E3-detectors)
    if(type=="multi_od"||type=="E3"||type=="e3") {
        beginE3Detector(attrs);
        myDetectorType = "e3";
        return;
    }
}


void
NLNetHandler::addE1Detector(const Attributes &attrs)
{
    // try to get the id first
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            "Error in description: missing id of a detector-object.");
        return;
    }
    try {
        myDetectorBuilder.buildInductLoop(id,
            getString(attrs, SUMO_ATTR_LANE),
            getFloat(attrs, SUMO_ATTR_POSITION),
            getInt(attrs, SUMO_ATTR_SPLINTERVAL),
            SharedOutputDevices::getInstance()->getOutputDeviceChecking(
                _file, getString(attrs, SUMO_ATTR_FILE)),
            getStringSecure(attrs, SUMO_ATTR_STYLE, "")
                );
    } catch (XMLBuildingException &e) {
        MsgHandler::getErrorInstance()->inform(e.getMessage("detector", id));
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.msg());
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            string("The description of the detector '")
            + id + string("' does not contain a needed value."));
    }
}


void
NLNetHandler::addE2Detector(const Attributes &attrs)
{
    // try to get the id first
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            "Error in description: missing id of a detector-object.");
        return;
    }
    // check whether this is a lsa-based detector or one that uses a sample
    //  interval
    MSTrafficLightLogic *tll = 0;
    try {
        string lsaid = getString(attrs, SUMO_ATTR_TLID);
        tll = myContainer.getTLLogic(lsaid);
        if(tll==0) {
            MsgHandler::getErrorInstance()->inform(
                string("The detector '") + id
                + string("' refers to the unknown lsa '") + lsaid
                + string("'."));
            return;
        }
    } catch (EmptyData) {
    }
    // check whether this is a detector connected to a link
    std::string toLane = getStringSecure(attrs, SUMO_ATTR_TO, "");
    //
    try {
        if(tll!=0) {
            if(toLane.length()==0) {
                myDetectorBuilder.buildE2Detector(myContainer.getLaneConts(),
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
                    getFloatSecure(attrs, SUMO_ATTR_DELETE_DATA_AFTER_SECONDS, 1800.0f)
                    );
            } else {
                myDetectorBuilder.buildE2Detector(myContainer.getLaneConts(),
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
                    getFloatSecure(attrs, SUMO_ATTR_DELETE_DATA_AFTER_SECONDS, 1800.0f)
                    );
            }
        } else {
            myDetectorBuilder.buildE2Detector(myContainer.getLaneConts(),
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
                getFloatSecure(attrs, SUMO_ATTR_DELETE_DATA_AFTER_SECONDS, 1800.0f)
                );
        }
    } catch (XMLBuildingException &e) {
        MsgHandler::getErrorInstance()->inform(e.getMessage("detector", id));
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.msg());
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            string("The description of the detector '")
            + id + string("' does not contain a needed value."));
    }
}


void
NLNetHandler::beginE3Detector(const Attributes &attrs)
{
    // try to get the id first
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
        m_Key = id;
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            "Error in description: missing id of a detector-object.");
        return;
    }
    try {
        myDetectorBuilder.beginE3Detector(id,
            SharedOutputDevices::getInstance()->getOutputDeviceChecking(
                _file, getString(attrs, SUMO_ATTR_FILE)),
            getInt(attrs, SUMO_ATTR_SPLINTERVAL),
            getStringSecure(attrs, SUMO_ATTR_MEASURES, "ALL"),
            getFloatSecure(attrs, SUMO_ATTR_HALTING_TIME_THRESHOLD, 1.0f),
            getFloatSecure(attrs, SUMO_ATTR_HALTING_SPEED_THRESHOLD, 5.0f/3.6f),
            getFloatSecure(attrs, SUMO_ATTR_DELETE_DATA_AFTER_SECONDS, 1800.0f)
            );
    } catch (XMLBuildingException &e) {
        MsgHandler::getErrorInstance()->inform(e.getMessage("detector", id));
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.msg());
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            string("The description of the detector '")
            + id + string("' does not contain a needed value."));
    }
}


void
NLNetHandler::addE3Entry(const Attributes &attrs)
{
    try {
        myDetectorBuilder.addE3Entry(
            getString(attrs, SUMO_ATTR_LANE),
            getFloat(attrs, SUMO_ATTR_POSITION));
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.msg());
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            string("The description of the detector '")
            + m_Key + string("' does not contain a needed value."));
    }
}


void
NLNetHandler::addE3Exit(const Attributes &attrs)
{
    try {
        myDetectorBuilder.addE3Exit(
            getString(attrs, SUMO_ATTR_LANE),
            getFloat(attrs, SUMO_ATTR_POSITION));
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.msg());
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            string("The description of the detector '")
            + m_Key + string("' does not contain a needed value."));
    }
}





void
NLNetHandler::addSource(const Attributes &attrs)
{
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
        try {
            NLSourceBuilder::buildTriggeredSource(id,
                getString(attrs, SUMO_ATTR_FILE), _file);
            return;
        } catch (XMLBuildingException &e) {
            MsgHandler::getErrorInstance()->inform(e.getMessage("detector", id));
        } catch (InvalidArgument &e) {
            MsgHandler::getErrorInstance()->inform(e.msg());
        } catch (EmptyData) {
            MsgHandler::getErrorInstance()->inform(
                string("The description of the source '")
                + id + string("' does not contain a needed value."));
        }
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            "Error in description: missing id of a detector-object.");
    }
}


void
NLNetHandler::addTrigger(const Attributes &attrs)
{
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
        try {
            myTriggerBuilder.buildTrigger(
                myContainer.getNet(), id,
                getString(attrs, SUMO_ATTR_OBJECTTYPE),
                getString(attrs, SUMO_ATTR_OBJECTID),
                getString(attrs, SUMO_ATTR_ATTR),
                getString(attrs, SUMO_ATTR_FILE),
                _file);
            return;
        } catch (XMLBuildingException &e) {
            MsgHandler::getErrorInstance()->inform(e.getMessage("trigger", id));
        } catch (InvalidArgument &e) {
            MsgHandler::getErrorInstance()->inform(e.msg());
        } catch (EmptyData) {
            MsgHandler::getErrorInstance()->inform(
                string("The description of the trigger '")
                + id + string("' does not contain a needed value."));
        }
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            "Error in description: missing id of a trigger-object.");
    }
}


void
NLNetHandler::openSucc(const Attributes &attrs)
{
    try {
        string id = getString(attrs, SUMO_ATTR_LANE);
        myContainer.openSuccLane(id);
        m_LaneId = id;
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            "Error in description: missing id of a succ-object.");
    }
}

void
NLNetHandler::addSuccLane(const Attributes &attrs)
{
    try {
        string tlID = getStringSecure(attrs, SUMO_ATTR_TLID, "");
        if(tlID!="") {
            myContainer.addSuccLane(
                getBool(attrs, SUMO_ATTR_YIELD),
                getString(attrs, SUMO_ATTR_LANE),
                getStringSecure(attrs, SUMO_ATTR_VIA, ""),
                parseLinkDir(getString(attrs, SUMO_ATTR_DIR)[0]),
                parseLinkState(getString(attrs, SUMO_ATTR_STATE)[0]),
                getBoolSecure(attrs, SUMO_ATTR_INTERNALEND, false),
                tlID, getInt(attrs, SUMO_ATTR_TLLINKNO));
        } else {
            myContainer.addSuccLane(
                getBool(attrs, SUMO_ATTR_YIELD),
                getString(attrs, SUMO_ATTR_LANE),
                getStringSecure(attrs, SUMO_ATTR_VIA, ""),
                parseLinkDir(getString(attrs, SUMO_ATTR_DIR)[0]),
                parseLinkState(getString(attrs, SUMO_ATTR_STATE)[0]),
                getBoolSecure(attrs, SUMO_ATTR_INTERNALEND, false));
        }
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            "Error in description: missing attribute in a succlane-object.");
    } catch (XMLIdNotKnownException &e) {
        MsgHandler::getErrorInstance()->inform(e.getMessage("", ""));
        MsgHandler::getErrorInstance()->inform(
            string(" While building lane '")
            + myContainer.getSuccingLaneName()
            + string("'"));
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform(
            string("Something is wrong with the definition of a link"));
    }
}



MSLink::LinkDirection
NLNetHandler::parseLinkDir(char dir)
{
    switch(dir) {
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
NLNetHandler::parseLinkState(char state)
{
    switch(state) {
    case 't':
        return MSLink::LINKSTATE_ABSTRACT_TL;
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
NLNetHandler::myCharacters(int element, const std::string &name,
                                const std::string &chars)
{
    // check static net information
    if(wanted(LOADFILTER_NET)) {
        switch(element) {
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
        case SUMO_TAG_INTERNAL_LANES:
            addInternalLanes(chars);
            break;
        default:
            break;
        }
    }
    // check junction logics
    if(wanted(LOADFILTER_LOGICS)) {
        switch(element) {
        case SUMO_TAG_REQUESTSIZE:
            if(m_Key.length()!=0) {
                setRequestSize(chars);
            }
            break;
        case SUMO_TAG_LANENUMBER:
            if(m_Key.length()!=0) {
                setLaneNumber(chars);
            }
            break;
        case SUMO_TAG_KEY:
            setKey(chars);
            break;
        case SUMO_TAG_OFFSET:
            setOffset(chars);
            break;
        case SUMO_TAG_LOGICNO:
            setTLLogicNo(chars);
            break;
        default:
            break;
        }
    }
    if(wanted(LOADFILTER_DYNAMIC)) {
        MSRouteHandler::myCharacters(element, name, chars);
    }
}


void
NLNetHandler::allocateEdges(const std::string &chars)
{
    size_t beg = 0;
    size_t idx = chars.find(' ');
    while(idx!=string::npos) {
        string edgeid = chars.substr(beg, idx-beg);
        // skip internal edges if not wished
        if(MSGlobals::gUsingInternalLanes||edgeid[0]!=':') {
            myContainer.addEdge(edgeid);
        }
        beg = idx + 1;
        idx = chars.find(' ', beg);
    }
    string edgeid = chars.substr(beg);
    // skip internal edges if not wished
    //  (the last one shouldn't be internal anyway)
    if(!MSGlobals::gUsingInternalLanes&&edgeid[0]==':') {
        return;
    }
    myContainer.addEdge(edgeid);
}


void
NLNetHandler::setNodeNumber(const std::string &chars)
{
    try {
        myContainer.setNodeNumber(TplConvert<char>::_2int(chars.c_str()));
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            "Error in description: missing number of nodes.");
    } catch(NumberFormatException) {
        MsgHandler::getErrorInstance()->inform(
            "Error in description: non-digit number of nodes.");
    }
}


void
NLNetHandler::addAllowedEdges(const std::string &chars)
{
    StringTokenizer st(chars);
    while(st.hasNext()) {
        string set = st.next();
        try {
            myContainer.addAllowed(set);
        } catch (XMLIdNotKnownException &e) {
            MsgHandler::getErrorInstance()->inform(e.getMessage("clane", set));
        } catch (XMLInvalidChildException &e) {
            MsgHandler::getErrorInstance()->inform(e.getMessage("clane", set));
        }
    }
}


void
NLNetHandler::setRequestSize(const std::string &chars)
{
    try {
        _requestSize = STRConvert::_2int(chars);
        m_pActiveLogic->resize(_requestSize);
        m_pActiveFoes->resize(_requestSize);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Missing request size.");
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform(
            "Error in description: one of an edge's attributes must be numeric but is not.");
    }
}


void
NLNetHandler::setLaneNumber(const std::string &chars)
{
    try {
        _laneNo = STRConvert::_2int(chars);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Missing lane number.");
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform(
            "Error in description: one of an edge's attributes must be numeric but is not.");
    }
}


void
NLNetHandler::setKey(const std::string &chars)
{
    if(chars.length()==0) {
        MsgHandler::getErrorInstance()->inform("No key given for the current junction logic.");
        return;
    }
    m_Key = chars;
}


void
NLNetHandler::setOffset(const std::string &chars)
{
    try {
        m_Offset = TplConvertSec<char>::_2intSec(chars.c_str(), 0);
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform("Invalid offset for a junction.");
        return;
    }
}


void
NLNetHandler::setTLLogicNo(const std::string &chars)
{
    _tlLogicNo = TplConvertSec<char>::_2intSec(chars.c_str(), -1);
    if(_tlLogicNo<0) {
        MsgHandler::getErrorInstance()->inform("Somenthing is wrong with a traffic light logic number.");
        MsgHandler::getErrorInstance()->inform(
            string(" In logic '") + m_Key + string("'."));
    }
}


void
NLNetHandler::addLogicItem(int request, const string &response,
                           const std::string &foes)
{
    // add the read response for the given request index
    bitset<64> use(response);
    assert(m_pActiveLogic->size()>(size_t) request);
    (*m_pActiveLogic)[request] = use;
    // add the read junction-internal foes for the given request index
    //  ...but only if junction-internal lanes shall be loaded
    if(MSGlobals::gUsingInternalLanes) {
        bitset<64> use2(foes);
        assert(m_pActiveFoes->size()>(size_t) request);
        (*m_pActiveFoes)[request] = use2;
    }
    // increse number of set information
    _requestItems++;
}


void
NLNetHandler::addIncomingLanes(const std::string &chars)
{
    StringTokenizer st(chars);
    while(st.hasNext()) {
        string set = st.next();
        try {
            myContainer.addIncomingLane(set);
        } catch (XMLIdNotKnownException &e) {
            MsgHandler::getErrorInstance()->inform(e.getMessage("lane", set));
        }
    }
}

//-----------------------------------------------------------------------------------


void
NLNetHandler::addPolyPosition(const std::string &chars)
{
    Polygon2D *ptr;
    Position2DVector shape;
    ptr = MSNet::getInstance()->poly_dic[actuell_poly_name];
    if(ptr==0) {
        return;
    }
    shape = GeomConvHelper::parseShape(chars);
    ptr->addPolyPosition(shape);
}


void
NLNetHandler::addInternalLanes(const std::string &chars)
{
    StringTokenizer st(chars);
    while(st.hasNext()) {
        string set = st.next();
        try {
            myContainer.addInternalLane(set);
        } catch (XMLIdNotKnownException &e) {
            MsgHandler::getErrorInstance()->inform(e.getMessage("lane", set));
        }
    }
}


// ----------------------------------

void
NLNetHandler::myEndElement(int element, const std::string &name)
{
    if(wanted(LOADFILTER_NET)) {
        switch(element) {
        case SUMO_TAG_EDGE:
            myContainer.closeEdge();
            break;
        case SUMO_TAG_LANES:
            myContainer.closeLanes();
            break;
        case SUMO_TAG_LANE:
            myContainer.closeLane();
            break;
        case SUMO_TAG_CEDGE:
            myContainer.closeAllowedEdge();
            break;
        case SUMO_TAG_JUNCTION:
            closeJunction();
            break;
        case SUMO_TAG_SUCC:
            closeSuccLane();
            break;
        }
    }
    if(wanted(LOADFILTER_NET)) {
        switch(element) {
        case SUMO_TAG_ROWLOGIC:
            closeJunctionLogic();
            break;
        case SUMO_TAG_TLLOGIC:
            closeTrafficLightLogic();
            break;
        default:
            break;
        }
    }
    if(wanted(LOADFILTER_NETADD)) {
        switch(element) {
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
    if(wanted(LOADFILTER_DYNAMIC)) {
        MSRouteHandler::myEndElement(element, name);
    }
}


void
NLNetHandler::closeJunction()
{
    try {
        myContainer.closeJunction();
    } catch (XMLIdAlreadyUsedException &e) {
        MsgHandler::getErrorInstance()->inform(e.getMessage("junction", ""));
    } catch (XMLIdNotKnownException &e) {
        MsgHandler::getErrorInstance()->inform(e.getMessage("junction", ""));
    }
}


void
NLNetHandler::closeJunctionLogic()
{
    if(_requestItems!=_requestSize) {
        MsgHandler::getErrorInstance()->inform(
            string("The description for the junction logic '") +
            m_Key +
            string("' is malicious."));
    }
    MSJunctionLogic *logic =
        new MSBitsetLogic(_requestSize, _laneNo,
            m_pActiveLogic, m_pActiveFoes);
    MSJunctionLogic::dictionary(m_Key, logic); // !!! replacement within the dictionary
}


void
NLNetHandler::closeTrafficLightLogic()
{
    if(_tlLogicNo!=0) {
        return;
    }
    // compute the initial step of the tls-logic
    size_t step = computeInitTLSStep();
    size_t firstEventOffset = computeInitTLSEventOffset();
    // build the tls-logic in dependance to its type
    if(m_Type=="actuated") {
        // build an actuated logic
        MSActuatedTrafficLightLogic *tlLogic =
            new MSActuatedTrafficLightLogic(m_Key, m_ActivePhases,
                step, firstEventOffset);
        MSTrafficLightLogic::dictionary(m_Key, tlLogic);
        // !!! replacement within the dictionary
        m_ActivePhases.clear();
        myContainer.addTLLogic(tlLogic);
        myContainer.addJunctionInitInfo(tlLogic,
            myContainer.getIncomingLanes(), m_DetectorOffset);
    } else if(m_Type=="agentbased") {
        // build an agentbased logic
        MSAgentbasedTrafficLightLogic *tlLogic =
            new MSAgentbasedTrafficLightLogic(m_Key, m_ActivePhases,
                step, firstEventOffset,
                myStdLearnHorizon, myStdDecisionHorizon,
                myStdDeltaLimit, myStdTCycle);
        MSTrafficLightLogic::dictionary(m_Key, tlLogic);
        // !!! replacement within the dictionary
        m_ActivePhases.clear();
        myContainer.addTLLogic(tlLogic);
        myContainer.addJunctionInitInfo(tlLogic,
            myContainer.getIncomingLanes(), m_DetectorOffset);
    } else {
        // build an uncontrolled (fix) tls-logic
        MSTrafficLightLogic *tlLogic =
            new MSSimpleTrafficLightLogic(
                m_Key, m_ActivePhases, step, firstEventOffset);
        MSTrafficLightLogic::dictionary(m_Key, tlLogic);
        // !!! replacement within the dictionary
        m_ActivePhases.clear();
        myContainer.addTLLogic(tlLogic);
    }
}


size_t
NLNetHandler::computeInitTLSStep()  const
{
    assert(m_ActivePhases.size()!=0);
    size_t offset = m_Offset % myAbsDuration;
    MSSimpleTrafficLightLogic::Phases::const_iterator i
        = m_ActivePhases.begin();
    size_t step = 0;
    while(true) {
        if(offset<(*i)->duration) {
            return step;
        }
        step++;
        offset -= (*i)->duration;
    }
}


size_t
NLNetHandler::computeInitTLSEventOffset()  const
{
    assert(m_ActivePhases.size()!=0);
    size_t offset = m_Offset % myAbsDuration;
    MSSimpleTrafficLightLogic::Phases::const_iterator i
        = m_ActivePhases.begin();
    while(true) {
        if(offset<(*i)->duration) {
            return (*i)->duration-offset;
        }
        offset -= (*i)->duration;
    }
}


void
NLNetHandler::closeSuccLane()
{
    try {
        myContainer.closeSuccLane();
    } catch (XMLIdNotKnownException &e) {
        MsgHandler::getErrorInstance()->inform(e.getMessage("", ""));
    }
}


void
NLNetHandler::endDetector()
{
    if(myDetectorType=="e3") {
        endE3Detector();
    }
    myDetectorType = "";
}


void
NLNetHandler::endE3Detector()
{
    myDetectorBuilder.endE3Detector();
}






std::string
NLNetHandler::getMessage() const
{
    return "Loading routes, lanes and vehicle types...";
}


bool
NLNetHandler::wanted(LoadFilter filter) const
{
    return (_filter&filter)!=0;
}


void
NLNetHandler::setWanted(LoadFilter filter)
{
    _filter = filter;
}


void
NLNetHandler::setError(const string &type,
                       const SAXParseException& exception)
{
    MsgHandler::getErrorInstance()->inform(
        buildErrorMessage(_file, type, exception));
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

