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
#include "NLLoadFilter.h"

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
NLNetHandler::NLNetHandler(bool verbose, bool warn, const std::string &file,
                           NLContainer &container)
    : MSRouteHandler(verbose, warn, file, true),
    myContainer(container), _tlLogicNo(-1)
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
        // all routes wil be read from the network description!
        MSRouteHandler::myStartElement(element, name, attrs);
    }
    // check junction logics
    if(wanted(LOADFILTER_LOGICS)) {
        switch(element) {
        case SUMO_TAG_ROWLOGIC:
            initJunctionLogic();
            break;
        case SUMO_TAG_TLLOGIC:
            initTrafficLightLogic();
            break;
        case SUMO_TAG_LOGICITEM:
            addLogicItem(attrs);
            break;
        case SUMO_TAG_TRAFOITEM:
            addTrafoItem(attrs);
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
        case SUMO_TAG_SOURCE:
            addSource(attrs);
            break;
        case SUMO_TAG_TRIGGER:
            addTrigger(attrs);
            break;
        }
    }
}


void
NLNetHandler::setEdgeNumber(const Attributes &attrs) {
    try {
        myContainer.setEdgeNumber(getInt(attrs, SUMO_ATTR_NO));
    } catch (EmptyData) {
        SErrorHandler::add(
            "Error in description: missing number of edges.");
    } catch(NumberFormatException) {
        SErrorHandler::add(
            "Error in description: non-digit number of edges.");
    }
}


void
NLNetHandler::chooseEdge(const Attributes &attrs) {
    // get the id
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
    } catch (EmptyData) {
        SErrorHandler::add(
            "Error in description: missing id of an edge-object.");
    } catch (XMLIdNotKnownException &e) {
        SErrorHandler::add(e.getMessage("edge", id));
    }
    // get the function
    string func;
    try {
        func = getString(attrs, SUMO_ATTR_FUNC);
    } catch (EmptyData) {
        SErrorHandler::add(
            "Error in description: missing function of an edge-object.");
    } catch (XMLIdNotKnownException &e) {
        SErrorHandler::add(e.getMessage("edge", id));
    }
    myContainer.chooseEdge(id, func);
}


void
NLNetHandler::addLane(const Attributes &attrs) {
    try {
        string id = getString(attrs, SUMO_ATTR_ID);
        try {
            myContainer.addLane(id,
                getBool(attrs, SUMO_ATTR_DEPART),
                getFloat(attrs, SUMO_ATTR_MAXSPEED),
                getFloat(attrs, SUMO_ATTR_LENGTH),
                getFloat(attrs, SUMO_ATTR_CHANGEURGE));
        } catch (XMLInvalidParentException &e) {
            SErrorHandler::add(e.getMessage("lane", id));
        } catch (XMLIdAlreadyUsedException &e) {
            SErrorHandler::add(e.getMessage("lane", id));
        } catch (XMLDepartLaneDuplicationException &e) {
            SErrorHandler::add(e.getMessage("lane", id));
        } catch (EmptyData) {
            SErrorHandler::add(
                "Error in description: missing attribute in an edge-object.");
        } catch (NumberFormatException) {
            SErrorHandler::add(
                "Error in description: one of an edge's attributes must be numeric but is not.");
        }
    } catch (EmptyData) {
        SErrorHandler::add(
            "Error in description: missing id of an edge-object.");
    }
}


void
NLNetHandler::openAllowedEdge(const Attributes &attrs) {
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
        myContainer.openAllowedEdge(id);
    } catch (XMLIdNotKnownException &e) {
        SErrorHandler::add(e.getMessage("cedge", id));
    } catch (EmptyData) {
        SErrorHandler::add(
            "Error in description: missing id of an cedge-object.");
    }
}


void
NLNetHandler::addJunctionKey(const Attributes &attrs) {
    try {
        string key = getString(attrs, SUMO_ATTR_KEY);
        myContainer.addKey(key); // !!! wozu?
    } catch (EmptyData) {
    }
}


void
NLNetHandler::initJunctionLogic() {
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
NLNetHandler::addLogicItem(const Attributes &attrs) {
    if(_responseSize>0&&_requestSize>0) {
        int request = -1;
        string response;
        try {
            request = getInt(attrs, SUMO_ATTR_REQUEST);
        } catch (EmptyData) {
            SErrorHandler::add("Missing request key...");
        } catch (NumberFormatException) {
            SErrorHandler::add(
                "Error in description: one of the request keys is not numeric.");
        }
        try {
            response = getString(attrs, SUMO_ATTR_RESPONSE);
        } catch (EmptyData) {
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
NLNetHandler::addTrafoItem(const Attributes &attrs) {
    int lane = -1;
    string links;
    try {
        lane = getInt(attrs, SUMO_ATTR_TO);
    } catch (EmptyData) {
        SErrorHandler::add("Missing lane number...");
    } catch (NumberFormatException) {
        SErrorHandler::add("The lane number is not numeric.");
    }
    try {
        links = getString(attrs, SUMO_ATTR_FROM);
    } catch (EmptyData) {
        SErrorHandler::add("Missing links in a lane transformation.");
    }
    if(lane>=0 && links.length()>0) {
        addTrafoItem(links, lane);
    }
}


void
NLNetHandler::initTrafficLightLogic()
{
    m_Key = "";
    m_ActivePhases.clear();
    _requestSize = -1;
    _tlLogicNo = -1;
}


void
NLNetHandler::addPhase(const Attributes &attrs) {
    if(_tlLogicNo!=0) {
        return;
    }
    // try to get the phase definition
    string phase;
    try {
        phase = getString(attrs, SUMO_ATTR_PHASE);
    } catch (EmptyData) {
        SErrorHandler::add("Missing phase definition.");
        return;
    }
    // try to get the break definition
    string brakeMask;
    try {
        brakeMask = getString(attrs, SUMO_ATTR_BRAKE);
    } catch (EmptyData) {
        SErrorHandler::add("Missing break definition.");
        return;
    }
    // try to get the phase duration
    try {
        size_t duration = getInt(attrs, SUMO_ATTR_DURATION);
        std::bitset<64> prios(brakeMask);
        prios.flip();
        m_ActivePhases.push_back(
            MSSimpleTrafficLightLogic<64>::PhaseDefinition(
            duration, std::bitset<64>(phase), prios));
    } catch (EmptyData) {
        SErrorHandler::add("Missing phase duration...");
    } catch (NumberFormatException) {
        SErrorHandler::add("The phase duration is not numeric.");
    }
}


void
NLNetHandler::openJunction(const Attributes &attrs) {
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
        if(id=="10") {
            int bla = 0;
        }
        try {
            myContainer.openJunction(id,
                getStringSecure(attrs, SUMO_ATTR_KEY, ""),
                getString(attrs, SUMO_ATTR_TYPE),
                getFloat(attrs, SUMO_ATTR_X),
                getFloat(attrs, SUMO_ATTR_Y));
        } catch (EmptyData) {
            SErrorHandler::add(
                "Error in description: missing attribute in a junction-object.");
        }
    } catch (EmptyData) {
        SErrorHandler::add(
            "Error in description: missing id of a junction-object.");
    }
}


void
NLNetHandler::addDetector(const Attributes &attrs) {
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
        try {
            myContainer.addDetector(
                NLDetectorBuilder::buildInductLoop(id,
                    getString(attrs, SUMO_ATTR_LANE),
                    getFloat(attrs, SUMO_ATTR_POSITION),
                    getFloat(attrs, SUMO_ATTR_SPLINTERVAL),
                    getString(attrs, SUMO_ATTR_STYLE),
                    getString(attrs, SUMO_ATTR_FILE),
                    _file));
        } catch (XMLBuildingException &e) {
            SErrorHandler::add(e.getMessage("detector", id));
        } catch (InvalidArgument &e) {
            SErrorHandler::add(e.msg());
        } catch (EmptyData) {
            SErrorHandler::add(
                string("The description of the detector '")
                + id + string("' does not contain a needed value."), true);
        }
    } catch (EmptyData) {
        SErrorHandler::add(
            "Error in description: missing id of a detector-object.");
    }
}


void
NLNetHandler::addSource(const Attributes &attrs) {
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
        try {
            NLSourceBuilder::buildTriggeredSource(
                myContainer.getEventControl(), id,
                getString(attrs, SUMO_ATTR_FILE),
                _file);
            return;
        } catch (XMLBuildingException &e) {
            SErrorHandler::add(e.getMessage("detector", id));
        } catch (InvalidArgument &e) {
            SErrorHandler::add(e.msg());
        } catch (EmptyData) {
            SErrorHandler::add(
                string("The description of the source '")
                + id + string("' does not contain a needed value."));
        }
    } catch (EmptyData) {
        SErrorHandler::add(
            "Error in description: missing id of a detector-object.");
    }
}


void
NLNetHandler::addTrigger(const Attributes &attrs) {
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
        try {
            NLTriggerBuilder::buildTrigger(
                myContainer.getNet(), id,
                getString(attrs, SUMO_ATTR_OBJECTTYPE),
                getString(attrs, SUMO_ATTR_OBJECTID),
                getString(attrs, SUMO_ATTR_ATTR),
                getString(attrs, SUMO_ATTR_FILE),
                _file);
            return;
        } catch (XMLBuildingException &e) {
            SErrorHandler::add(e.getMessage("trigger", id));
        } catch (InvalidArgument &e) {
            SErrorHandler::add(e.msg());
        } catch (EmptyData) {
            SErrorHandler::add(
                string("The description of the trigger '")
                + id + string("' does not contain a needed value."));
        }
    } catch (EmptyData) {
        SErrorHandler::add(
            "Error in description: missing id of a trigger-object.");
    }
}


void
NLNetHandler::openSucc(const Attributes &attrs) {
    try {
        string id = getString(attrs, SUMO_ATTR_LANE);
        myContainer.openSuccLane(id);
        m_LaneId = id;
    } catch (EmptyData) {
        SErrorHandler::add(
            "Error in description: missing id of a succ-object.");
    }
}

void
NLNetHandler::addSuccLane(const Attributes &attrs) {
    try {
        myContainer.addSuccLane(
            getBool(attrs, SUMO_ATTR_YIELD),
            getString(attrs, SUMO_ATTR_LANE));
    } catch (EmptyData) {
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
        case SUMO_TAG_NODECOUNT:
            setNodeNumber(chars);
            break;
        case SUMO_TAG_INLANES:
            addInLanes(chars);
            break;
        default:
            break;
        }
        // all routes wil be read from the network description!
        MSRouteHandler::myCharacters(element, name, chars);
    }
    // check junction logics
    if(wanted(LOADFILTER_LOGICS)) {
        switch(element) {
        case SUMO_TAG_REQUESTSIZE:
            if(m_Key.length()!=0)
                setRequestSize(chars);
            break;
        case SUMO_TAG_RESPONSESIZE:
            if(m_Key.length()!=0)
                setResponseSize(chars);
            break;
        case SUMO_TAG_LANENUMBER:
            if(m_Key.length()!=0)
                setLaneNumber(chars);
            break;
        case SUMO_TAG_KEY:
            setKey(chars);
            break;
        case SUMO_TAG_LOGICNO:
            setTLLogicNo(chars);
            break;
        default:
            break;
        }
    }
}


void
NLNetHandler::allocateEdges(const std::string &chars) {
    StringTokenizer st(chars);
    while(st.hasNext()) {
        myContainer.addEdge(st.next());
    }
}


void
NLNetHandler::setNodeNumber(const std::string &chars) {
    try {
        myContainer.setNodeNumber(TplConvert<char>::_2int(chars.c_str()));
    } catch (EmptyData) {
        SErrorHandler::add(
            "Error in description: missing number of nodes.");
    } catch(NumberFormatException) {
        SErrorHandler::add(
            "Error in description: non-digit number of nodes.");
    }
}


void
NLNetHandler::addAllowedEdges(const std::string &chars) {
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
NLNetHandler::setRequestSize(const std::string &chars) {
    try {
        _requestSize = STRConvert::_2int(chars);
        m_pActiveLogic->resize(_requestSize);
    } catch (EmptyData) {
        SErrorHandler::add("Missing request size.");
    } catch (NumberFormatException) {
        SErrorHandler::add(
            "Error in description: one of an edge's attributes must be numeric but is not.");
    }
}

void
NLNetHandler::setResponseSize(const std::string &chars) {
    try {
        _responseSize = STRConvert::_2int(chars);
        m_pActiveTrafo->resize(_responseSize);
    } catch (EmptyData) {
        SErrorHandler::add("Missing response size.");
    } catch (NumberFormatException) {
        SErrorHandler::add(
            "Response size is not numeric! Contact your netconvert-programmer.");
    }
}

void
NLNetHandler::setLaneNumber(const std::string &chars) {
    try {
        _laneNo = STRConvert::_2int(chars);
        m_pActiveTrafo->resize(_responseSize);
    } catch (EmptyData) {
        SErrorHandler::add("Missing lane number.");
    } catch (NumberFormatException) {
        SErrorHandler::add(
            "Error in description: one of an edge's attributes must be numeric but is not.");
    }
}


void
NLNetHandler::setKey(const std::string &chars)
{
    if(chars.length()==0) {
        SErrorHandler::add("No key given for the current junction logic.");
        return;
    }
    m_Key = chars;
}

void
NLNetHandler::setTLLogicNo(const std::string &chars) {
    _tlLogicNo = TplConvertSec<char>::_2intSec(chars.c_str(), -1);
    if(_tlLogicNo<0) {
        SErrorHandler::add("Somenthing is wrong with a traffic light logic number.");
        SErrorHandler::add(
            string(" In logic '") + m_Key + string("'."));
    }
}


void
NLNetHandler::addLogicItem(int request, const string &response) {
    bitset<64> use(response);
    assert(m_pActiveLogic->size()>request);
    (*m_pActiveLogic)[request] = use;
    _requestItems++;
}

void
NLNetHandler::addTrafoItem(const string &links, int lane) {
    bitset<64> use(links);
    assert(m_pActiveTrafo->size()>lane);
    (*m_pActiveTrafo)[lane] = use;
    _trafoItems++;
}


void
NLNetHandler::addInLanes(const std::string &chars) {
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
        // All routes will be read from the network description!
        MSRouteHandler::myEndElement(element, name);
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
}


void
NLNetHandler::closeJunction() {
    try {
        myContainer.closeJunction();
    } catch (XMLIdAlreadyUsedException &e) {
        SErrorHandler::add(e.getMessage("junction", ""));
    } catch (XMLIdNotKnownException &e) {
        SErrorHandler::add(e.getMessage("junction", ""));
    }
}


void
NLNetHandler::closeJunctionLogic() {
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
NLNetHandler::closeTrafficLightLogic() {
    if(_tlLogicNo!=0) {
        return;
    }
    MSTrafficLightLogic *tlLogic =
        new MSSimpleTrafficLightLogic<64>(m_Key, m_ActivePhases, 0);
    MSTrafficLightLogic::dictionary(m_Key, tlLogic); // !!! replacement within the dictionary
    m_ActivePhases.clear();
}

void
NLNetHandler::closeSuccLane() {
    try {
        myContainer.closeSuccLane();
    } catch (XMLIdNotKnownException &e) {
        SErrorHandler::add(e.getMessage("", ""));
    }
}




std::string
NLNetHandler::getMessage() const {
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
    SErrorHandler::add(buildErrorMessage(_file, type, exception), true);
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NLNetHandler.icc"
//#endif

// Local Variables:
// mode:C++
// End:
