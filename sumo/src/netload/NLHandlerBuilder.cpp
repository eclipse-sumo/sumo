/***************************************************************************
                          NLHandlerBuilder.cpp
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

/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <sax/HandlerBase.hpp>
#include <sax/AttributeList.hpp>
#include <sax/SAXParseException.hpp>
#include <sax/SAXException.hpp>
#include "NLContainer.h"
#include "NLHandlerBuilder.h"
#include "NLDetectorBuilder.h"
#include "NLSourceBuilder.h"
#include <utils/common/SErrorHandler.h>
#include <utils/convert/TplConvert.h>
#include <utils/convert/TplConvertSec.h>
#include <utils/convert/STRConvert.h>
#include <utils/xml/XMLBuildingExceptions.h>
#include <utils/common/StringTokenizer.h>
#include <utils/xml/AttributesHandler.h>
#include <microsim/MSBitSetLogic.h>
#include <microsim/MSJunctionLogic.h>
#include <microsim/MSTrafficLightLogic.h>
#include <utils/common/UtilExceptions.h>
#include "NLSAXHandler.h"
#include "NLLoadFilter.h"
#include "NLTags.h"

#ifdef MSVC
#include <microsim/MSBitSetLogic.cpp>
#include <microsim/MSSimpleTrafficLightLogic.cpp>
#endif

/* =========================================================================
 * using namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
NLHandlerBuilder::NLHandlerBuilder(NLContainer &container)
    : NLSAXHandler(container), _tlLogicNo(-1)
{
    _attrHandler.add(ATTR_ID, "id");
    _attrHandler.add(ATTR_DEPART, "depart");
    _attrHandler.add(ATTR_MAXSPEED, "maxspeed");
    _attrHandler.add(ATTR_LENGTH, "length");
    _attrHandler.add(ATTR_CHANGEURGE, "changeurge");
    _attrHandler.add(ATTR_ACCEL, "accel");
    _attrHandler.add(ATTR_DECEL, "decel");
    _attrHandler.add(ATTR_SIGMA, "sigma");
    _attrHandler.add(ATTR_KEY, "key");
    _attrHandler.add(ATTR_REQUEST, "request");
    _attrHandler.add(ATTR_RESPONSE, "response");
    _attrHandler.add(ATTR_TO, "to");
    _attrHandler.add(ATTR_FROM, "from");
    _attrHandler.add(ATTR_TYPE, "type");
    _attrHandler.add(ATTR_ROUTE, "route");
    _attrHandler.add(ATTR_LANE, "lane");
    _attrHandler.add(ATTR_POSITION, "pos");
    _attrHandler.add(ATTR_SPLINTERVAL, "freq");
    _attrHandler.add(ATTR_STYLE, "style");
    _attrHandler.add(ATTR_FILE, "file");
    _attrHandler.add(ATTR_X, "x");
    _attrHandler.add(ATTR_Y, "y");
    _attrHandler.add(ATTR_JUNCTION, "junction");
    _attrHandler.add(ATTR_YIELD, "yield");
    _attrHandler.add(ATTR_NO, "no");
    _attrHandler.add(ATTR_PHASE, "phase");
    _attrHandler.add(ATTR_BRAKE, "brake");
    _attrHandler.add(ATTR_DURATION, "duration");
    _attrHandler.add(ATTR_FUNC, "function");
}

NLHandlerBuilder::~NLHandlerBuilder()
{
}

void
NLHandlerBuilder::myStartElement(int element, const std::string &name,
                                  const Attributes &attrs)
{
    // check static net information
    if(wanted(LOADFILTER_NET)) {
        switch(element) {
        case NLTag_edges:
            setEdgeNumber(attrs);
            break;
        case NLTag_edge:
            chooseEdge(attrs);
            break;
        case NLTag_lane:
            addLane(attrs);
            break;
        case NLTag_cedge:
            openAllowedEdge(attrs);
            break;
        case NLTag_junction:
            openJunction(attrs);
            addJunctionKey(attrs);
            break;
        case NLTag_phase:
            addPhase(attrs);
            break;
        case NLTag_succ:
            openSucc(attrs);
            break;
        case NLTag_succlane:
            addSuccLane(attrs);
            break;
        default:
            break;
        }
    }
    // check dynamic components
    if(wanted(LOADFILTER_DYNAMIC)) {
        switch(element) {
        case NLTag_vehicle:
            addVehicle(attrs);
            break;
        case NLTag_vtype:
            addVehicleType(attrs);
            break;
        case NLTag_route:
            openRoute(attrs);
            break;
        default:
            break;
        }
    }
    // check junction logics
    if(wanted(LOADFILTER_LOGICS)) {
        switch(element) {
        case NLTag_rowlogic:
            initJunctionLogic();
            break;
        case NLTag_tllogic:
            initTrafficLightLogic();
            break;
        case NLTag_logicitem:
            addLogicItem(attrs);
            break;
        case NLTag_trafoitem:
            addTrafoItem(attrs);
            break;
        default:
            break;
        }
    }
    // process detectors when wished
    if(wanted(LOADFILTER_DETECTORS)&&element==NLTag_detector) {
        addDetector(attrs);
    }
    // process sources when wished
    if(wanted(LOADFILTER_SOURCES)&&element==NLTag_source) {
        addSource(attrs);
    }
}


void
NLHandlerBuilder::setEdgeNumber(const Attributes &attrs) {
    try {
        myContainer.setEdgeNumber(_attrHandler.getInt(attrs, ATTR_NO));
    } catch (EmptyData &e) {
        SErrorHandler::add(
            "Error in description: missing number of edges.");
    } catch(NumberFormatException) {
        SErrorHandler::add(
            "Error in description: non-digit number of edges.");
    }
}


void
NLHandlerBuilder::chooseEdge(const Attributes &attrs) {
    string id;
    try {
        id = _attrHandler.getString(attrs, ATTR_ID);
        myContainer.chooseEdge(id);
    } catch (EmptyData &e) {
        SErrorHandler::add(
            "Error in description: missing id of an edge-object.");
    } catch (XMLIdNotKnownException &e) {
        SErrorHandler::add(e.getMessage("edge", id));
    }
}


void
NLHandlerBuilder::addLane(const Attributes &attrs) {
    try {
        string id = _attrHandler.getString(attrs, ATTR_ID);
        try {
            myContainer.addLane(id,
                _attrHandler.getBool(attrs, ATTR_DEPART),
                _attrHandler.getFloat(attrs, ATTR_MAXSPEED),
                _attrHandler.getFloat(attrs, ATTR_LENGTH),
                _attrHandler.getFloat(attrs, ATTR_CHANGEURGE));
        } catch (XMLInvalidParentException &e) {
            SErrorHandler::add(e.getMessage("lane", id));
        } catch (XMLIdAlreadyUsedException &e) {
            SErrorHandler::add(e.getMessage("lane", id));
        } catch (XMLDepartLaneDuplicationException &e) {
            SErrorHandler::add(e.getMessage("lane", id));
        } catch (EmptyData &e) {
            SErrorHandler::add(
                "Error in description: missing attribute in an edge-object.");
        } catch (NumberFormatException &e) {
            SErrorHandler::add(
                "Error in description: one of an edge's attributes must be numeric but is not.");
        }
    } catch (EmptyData &e) {
        SErrorHandler::add(
            "Error in description: missing id of an edge-object.");
    }
}


void
NLHandlerBuilder::openAllowedEdge(const Attributes &attrs) {
    string id;
    try {
        id = _attrHandler.getString(attrs, ATTR_ID);
        myContainer.openAllowedEdge(id);
    } catch (XMLIdNotKnownException &e) {
        SErrorHandler::add(e.getMessage("cedge", id));
    } catch (EmptyData &e) {
        SErrorHandler::add(
            "Error in description: missing id of an cedge-object.");
    }
}


void
NLHandlerBuilder::addVehicleType(const Attributes &attrs) {
    try {
        string id = _attrHandler.getString(attrs, ATTR_ID);
        try {
            myContainer.addVehicleType(id,
                _attrHandler.getFloat(attrs, ATTR_LENGTH),
                _attrHandler.getFloat(attrs, ATTR_MAXSPEED),
                _attrHandler.getFloat(attrs, ATTR_ACCEL),
                _attrHandler.getFloat(attrs, ATTR_DECEL),
                _attrHandler.getFloat(attrs, ATTR_SIGMA));
        } catch (XMLIdAlreadyUsedException &e) {
            SErrorHandler::add(e.getMessage("vehicletype", id));
        } catch (EmptyData &e) {
            SErrorHandler::add(
                "Error in description: missing attribute in a vehicletype-object.");
        } catch (NumberFormatException &e) {
            SErrorHandler::add(
                "Error in description: one of an vehtype's attributes must be numeric but is not.");
        }
    } catch (EmptyData &e) {
        SErrorHandler::add(
            "Error in description: missing id of a vehicle-object.");
    }
}


void
NLHandlerBuilder::openRoute(const Attributes &attrs) {
    string id;
    try {
        id = _attrHandler.getString(attrs, ATTR_ID);
        myContainer.openRoute(id);
    } catch (EmptyData &e) {
        SErrorHandler::add(
            "Error in description: missing id of a route-object.");
    } catch (XMLIdNotKnownException &e) {
        SErrorHandler::add(e.getMessage("route", "(ID_UNKNOWN!)"));
    }
}


void
NLHandlerBuilder::addJunctionKey(const Attributes &attrs) {
    try {
        string key = _attrHandler.getString(attrs, ATTR_KEY);
        myContainer.addKey(key); // !!! wozu?
    } catch (EmptyData &e) {
    }
}


void
NLHandlerBuilder::initJunctionLogic() {
    m_Key = "";
    m_pActiveLogic = new MSBitsetLogic::Logic();
    m_pActiveTrafo = new MSBitsetLogic::Link2LaneTrafo();
    _requestSize = -1;
    _responseSize = -1;
    _laneNo = -1;
    _trafoItems = 0;
    _requestItems = 0;
}


void
NLHandlerBuilder::addLogicItem(const Attributes &attrs) {
    if(_responseSize>0&&_requestSize>0) {
        int request = -1;
        string response;
        try {
            request = _attrHandler.getInt(attrs, ATTR_REQUEST);
        } catch (EmptyData &e) {
            SErrorHandler::add("Missing request key...");
        } catch (NumberFormatException &e) {
            SErrorHandler::add(
                "Error in description: one of the request keys is not numeric.");
        }
        try {
            response = _attrHandler.getString(attrs, ATTR_RESPONSE);
        } catch (EmptyData &e) {
            SErrorHandler::add("Missing respond for a request");
        }
        if(request>=0 && response.length()>0)
            addLogicItem(request, response);
    } else {
        SErrorHandler::add(
            "The request size,  the response size or the number of lanes is not given! Contact your net supplier");
    }
}


void
NLHandlerBuilder::addTrafoItem(const Attributes &attrs) {
    int lane = -1;
    string links;
    try {
        lane = _attrHandler.getInt(attrs, ATTR_TO);
    } catch (EmptyData &e) {
        SErrorHandler::add("Missing lane number...");
    } catch (NumberFormatException &e) {
        SErrorHandler::add("The lane number is not numeric.");
    }
    try {
        links = _attrHandler.getString(attrs, ATTR_FROM);
    } catch (EmptyData &e) {
        SErrorHandler::add("Missing links in a lane transformation.");
    }
    if(lane>=0 && links.length()>0) {
        addTrafoItem(links, lane);
    }
}


void
NLHandlerBuilder::initTrafficLightLogic()
{
    m_Key = "";
    m_ActivePhases.clear();
    _requestSize = -1;
    _tlLogicNo = -1;
}


void
NLHandlerBuilder::addPhase(const Attributes &attrs) {
    if(_tlLogicNo!=0) {
        return;
    }
    // try to get the phase definition
    string phase;
    try {
        phase = _attrHandler.getString(attrs, ATTR_PHASE);
    } catch (EmptyData &e) {
        SErrorHandler::add("Missing phase definition.");
        return;
    }
    // try to get the break definition
    string brakeMask;
    try {
        brakeMask = _attrHandler.getString(attrs, ATTR_BRAKE);
    } catch (EmptyData &e) {
        SErrorHandler::add("Missing break definition.");
        return;
    }
    // try to get the phase duration
    try {
        size_t duration = _attrHandler.getInt(attrs, ATTR_DURATION);
        std::bitset<64> prios(brakeMask);
        prios.flip();
        m_ActivePhases.push_back(
            MSSimpleTrafficLightLogic<64>::PhaseDefinition(
            duration, std::bitset<64>(phase), prios));
    } catch (EmptyData &e) {
        SErrorHandler::add("Missing phase duration...");
    } catch (NumberFormatException &e) {
        SErrorHandler::add("The phase duration is not numeric.");
    }
}


void
NLHandlerBuilder::openJunction(const Attributes &attrs) {
    string id;
    try {
        id = _attrHandler.getString(attrs, ATTR_ID);
        try {
            myContainer.openJunction(id,
                _attrHandler.getStringSecure(attrs, ATTR_KEY, ""),
                _attrHandler.getString(attrs, ATTR_TYPE),
                _attrHandler.getFloat(attrs, ATTR_X),
                _attrHandler.getFloat(attrs, ATTR_Y));
        } catch (EmptyData &e) {
            SErrorHandler::add(
                "Error in description: missing attribute in a junction-object.");
        }
    } catch (EmptyData &e) {
        SErrorHandler::add(
            "Error in description: missing id of a junction-object.");
    }
}


void
NLHandlerBuilder::addVehicle(const Attributes &attrs) {
    string id;
    try {
        id = _attrHandler.getString(attrs, ATTR_ID);
        try {
            myContainer.addVehicle(id,
                _attrHandler.getString(attrs, ATTR_TYPE),
                _attrHandler.getString(attrs, ATTR_ROUTE),
                _attrHandler.getLong(attrs, ATTR_DEPART));
        } catch (EmptyData &e) {
            SErrorHandler::add(
                "Error in description: missing attribute in a vehicle-object.");
        } catch(XMLIdNotKnownException &e) {
            SErrorHandler::add(e.getMessage("", ""));
        } catch(XMLIdAlreadyUsedException &e) {
            SErrorHandler::add(e.getMessage("vehicle", id));
        }
    } catch (EmptyData &e) {
        SErrorHandler::add(
            "Error in description: missing id of a vehicle-object.");
    }
}


void
NLHandlerBuilder::addDetector(const Attributes &attrs) {
    string id;
    try {
        id = _attrHandler.getString(attrs, ATTR_ID);
        try {
            myContainer.addDetector(
                NLDetectorBuilder::buildInductLoop(id,
                    _attrHandler.getString(attrs, ATTR_LANE),
                    _attrHandler.getFloat(attrs, ATTR_POSITION),
                    _attrHandler.getFloat(attrs, ATTR_SPLINTERVAL),
                    _attrHandler.getString(attrs, ATTR_STYLE),
                    _attrHandler.getString(attrs, ATTR_FILE),
                    _file));
        } catch (XMLBuildingException &e) {
            SErrorHandler::add(e.getMessage("detector", id));
        } catch (InvalidArgument &e) {
            SErrorHandler::add(e.msg());
        } catch (EmptyData &e) {
            SErrorHandler::add(
                string("The description of the detector '")
                + id + string("' does not contain a needed value."), true);
        }
    } catch (EmptyData &e) {
        SErrorHandler::add(
            "Error in description: missing id of a detector-object.");
    }
}


void
NLHandlerBuilder::addSource(const Attributes &attrs) {
    string id;
    try {
        id = _attrHandler.getString(attrs, ATTR_ID);
        try {
            NLSourceBuilder::buildTriggeredSource(
                myContainer.getEventControl(), id,
                _attrHandler.getString(attrs, ATTR_FILE),
                _file);
            return;
        } catch (XMLBuildingException &e) {
            SErrorHandler::add(e.getMessage("detector", id));
        } catch (InvalidArgument &e) {
            SErrorHandler::add(e.msg());
        } catch (EmptyData &e) {
            SErrorHandler::add(
                string("The description of the source '")
                + id + string("' does not contain a needed value."));
        }
    } catch (EmptyData &e) {
        SErrorHandler::add(
            "Error in description: missing id of a detector-object.");
    }
}


/*void
NLHandlerBuilder::openSuccLane(const Attributes &attrs) {
    try {
        string id = _attrHandler.getString(attrs, ATTR_ID);
        myContainer.openSuccLane(id);
        m_LaneId = id;
    } catch (EmptyData &e) {
        SErrorHandler::add(
            "Error in description: missing id of a succ-object.");
    }
}*/


void
NLHandlerBuilder::openSucc(const Attributes &attrs) {
    try {
        string id = _attrHandler.getString(attrs, ATTR_LANE);
        myContainer.openSuccLane(id);
        m_LaneId = id;
    } catch (EmptyData &e) {
        SErrorHandler::add(
            "Error in description: missing id of a succ-object.");
    }
    try {
        myContainer.setSuccJunction(_attrHandler.getString(attrs, ATTR_JUNCTION));
    } catch (EmptyData &e) {
        SErrorHandler::add(
            "Error in description: missing id of a succ junction-object.");
    }
}

void
NLHandlerBuilder::addSuccLane(const Attributes &attrs) {
    try {
        myContainer.addSuccLane(
            _attrHandler.getBool(attrs, ATTR_YIELD),
            _attrHandler.getString(attrs, ATTR_LANE));
    } catch (EmptyData &e) {
        SErrorHandler::add(
            "Error in description: missing attribute in a succlane-object.");
    } catch (XMLIdNotKnownException &e) {
        SErrorHandler::add(e.getMessage("", ""));
        SErrorHandler::add(
            string(" While building lane '")
            + myContainer.getSuccingLaneName()
            + string("'"));
    }
}




// ----------------------------------


void
NLHandlerBuilder::myCharacters(int element, const std::string &name,
                                const std::string &chars)
{
    // check static net information
    if(wanted(LOADFILTER_NET)) {
        switch(element) {
        case NLTag_edges:
            allocateEdges(chars);
            break;
        case NLTag_cedge:
            addAllowedEdges(chars);
            break;
        case NLTag_nodecount:
            setNodeNumber(chars);
            break;
        case NLTag_inlane:
            addInLanes(chars);
            break;
        default:
            break;
        }
    }
    // check dynamic components
    if(wanted(LOADFILTER_DYNAMIC)) {
        switch(element) {
        case NLTag_route:
            addRouteElements(name, chars);
            break;
        default:
            break;
        }
    }
    // check junction logics
    if(wanted(LOADFILTER_LOGICS)) {
        switch(element) {
        case NLTag_requestsize:
            if(m_Key.length()!=0)
                setRequestSize(chars);
            break;
        case NLTag_responsesize:
            if(m_Key.length()!=0)
                setResponseSize(chars);
            break;
        case NLTag_lanenumber:
            if(m_Key.length()!=0)
                setLaneNumber(chars);
            break;
        case NLTag_key:
            setKey(chars);
            break;
        case NLTag_logicno:
            setTLLogicNo(chars);
            break;
        default:
            break;
        }
    }

}


void
NLHandlerBuilder::allocateEdges(const std::string &chars) {
    StringTokenizer st(chars);
    while(st.hasNext()) {
        myContainer.addEdge(st.next());
    }
}


void
NLHandlerBuilder::setNodeNumber(const std::string &chars) {
    try {
        myContainer.setNodeNumber(TplConvert<char>::_2int(chars.c_str()));
    } catch (EmptyData &e) {
        SErrorHandler::add(
            "Error in description: missing number of nodes.");
    } catch(NumberFormatException) {
        SErrorHandler::add(
            "Error in description: non-digit number of nodes.");
    }
}


void
NLHandlerBuilder::addAllowedEdges(const std::string &chars) {
    StringTokenizer st(chars);
    while(st.hasNext()) {
        string set = st.next();
        try {
            myContainer.addAllowed(set);
        } catch (XMLIdNotKnownException &e) {
            SErrorHandler::add(e.getMessage("clane", set));
        } catch (XMLInvalidChildException &e) {
            SErrorHandler::add(e.getMessage("clane", set));
        }
    }
}

void
NLHandlerBuilder::addRouteElements(const std::string &name,
                                    const std::string &chars) {
    StringTokenizer st(chars);
    if(/* NLNetBuilder::check&& */st.size()==0)
        SErrorHandler::add("Empty route (" + name + ")");
    else {
        while(st.hasNext()) {
            string set = st.next();
            try {
                myContainer.addRoutesEdge(set);
            } catch (XMLIdNotKnownException &e) {
                SErrorHandler::add(e.getMessage("routes edge", ""));
            }
        }
    }
}

void
NLHandlerBuilder::setRequestSize(const std::string &chars) {
    try {
        _requestSize = STRConvert::_2int(chars);
        m_pActiveLogic->resize(_requestSize);
    } catch (EmptyData &e) {
        SErrorHandler::add("Missing request size.");
    } catch (NumberFormatException &e) {
        SErrorHandler::add(
            "Error in description: one of an edge's attributes must be numeric but is not.");
    }
}

void
NLHandlerBuilder::setResponseSize(const std::string &chars) {
    try {
        _responseSize = STRConvert::_2int(chars);
        m_pActiveTrafo->resize(_responseSize);
    } catch (EmptyData &e) {
        SErrorHandler::add("Missing response size.");
    } catch (NumberFormatException &e) {
        SErrorHandler::add(
            "Response size is not numeric! Contact your netconvert-programmer.");
    }
}

void
NLHandlerBuilder::setLaneNumber(const std::string &chars) {
    try {
        _laneNo = STRConvert::_2int(chars);
        m_pActiveTrafo->resize(_responseSize);
    } catch (EmptyData &e) {
        SErrorHandler::add("Missing lane number.");
    } catch (NumberFormatException &e) {
        SErrorHandler::add(
            "Error in description: one of an edge's attributes must be numeric but is not.");
    }
}


void
NLHandlerBuilder::setKey(const std::string &chars) 
{
    if(chars.length()==0) {
        SErrorHandler::add("No key given for the current junction logic.");
        return;
    }
    m_Key = chars;
}

void
NLHandlerBuilder::setTLLogicNo(const std::string &chars) {
    _tlLogicNo = TplConvertSec<char>::_2intSec(chars.c_str(), -1);
    if(_tlLogicNo<0) {
        SErrorHandler::add("Somenthing is wrong with a traffic light logic number.");
        SErrorHandler::add(
            string(" In logic '") + m_Key + string("'."));
    }
}


void
NLHandlerBuilder::addLogicItem(int request, const string &response) {
    bitset<64> use(response);
    (*m_pActiveLogic)[request] = use;
    _requestItems++;
}

void
NLHandlerBuilder::addTrafoItem(const string &links, int lane) {
    bitset<64> use(links);
    (*m_pActiveTrafo)[lane] = use;
    _trafoItems++;
}


void
NLHandlerBuilder::addInLanes(const std::string &chars) {
    StringTokenizer st(chars);
    while(st.hasNext()) {
        string set = st.next();
        try {
            myContainer.addInLane(set);
        } catch (XMLIdNotKnownException &e) {
            SErrorHandler::add(e.getMessage("lane", set));
        }
    }
}


// ----------------------------------

void
NLHandlerBuilder::myEndElement(int element, const std::string &name)
{
    if(wanted(LOADFILTER_NET)) {
        switch(element) {
        case NLTag_edge:
            myContainer.closeEdge();
            break;
        case NLTag_lanes:
            myContainer.closeLanes();
            break;
        case NLTag_cedge:
            myContainer.closeAllowedEdge();
            break;
        case NLTag_junction:
            closeJunction();
            break;
        case NLTag_succ:
            closeSuccLane();
            break;
        }
    }
    if(wanted(LOADFILTER_DYNAMIC)) {
        switch(element) {
        case NLTag_route:
            try {
                myContainer.closeRoute();
            } catch (XMLListEmptyException &e) {
                SErrorHandler::add(e.getMessage("route", ""));
            } catch (XMLIdAlreadyUsedException &e) {
                SErrorHandler::add(e.getMessage("route", ""));
            }
            break;
        }
    }
    if(wanted(LOADFILTER_NET)) {
        switch(element) {
        case NLTag_rowlogic:
            closeJunctionLogic();
            break;
        case NLTag_tllogic:
            closeTrafficLightLogic();
            break;
        default:
            break;
        }
    }
}


void
NLHandlerBuilder::closeJunction() {
    try {
        myContainer.closeJunction();
    } catch (XMLIdAlreadyUsedException &e) {
        SErrorHandler::add(e.getMessage("junction", ""));
    } catch (XMLIdNotKnownException &e) {
        SErrorHandler::add(e.getMessage("junction", ""));
    }
}


void
NLHandlerBuilder::closeJunctionLogic() {
    if(_trafoItems!=_laneNo||_requestItems!=_requestSize) {
        SErrorHandler::add(
	        string("The description for the junction logic '") +
	        m_Key +
	        string("' is malicious."));
    }
    MSJunctionLogic *logic =
        new MSBitsetLogic(_requestSize, _laneNo,
            m_pActiveLogic, m_pActiveTrafo);
    MSJunctionLogic::dictionary(m_Key, logic); // !!! replacement within the dictionary
}


void
NLHandlerBuilder::closeTrafficLightLogic() {
    if(_tlLogicNo!=0) {
        return;
    }
    MSTrafficLightLogic *tlLogic =
        new MSSimpleTrafficLightLogic<64>(m_Key, m_ActivePhases);
    MSTrafficLightLogic::dictionary(m_Key, tlLogic); // !!! replacement within the dictionary
    m_ActivePhases.clear();
}

void
NLHandlerBuilder::closeSuccLane() {
    try {
        myContainer.closeSuccLane();
    } catch (XMLIdNotKnownException &e) {
        SErrorHandler::add(e.getMessage("", ""));
    }
}




std::string
NLHandlerBuilder::getMessage() const {
    return "Loading routes, lanes and vehicle types...";
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NLHandlerBuilder.icc"
//#endif

// Local Variables:
// mode:C++
// End:
