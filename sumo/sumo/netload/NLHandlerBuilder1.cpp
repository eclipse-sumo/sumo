/***************************************************************************
                          NLHandlerBuilder1.cpp
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
// Revision 1.4  2002/04/25 09:26:05  dkrajzew
// New names for the acceleration and the deceleration parameter applied
//
// Revision 1.3  2002/04/17 11:17:01  dkrajzew
// windows-newlines removed
//
// Revision 1.2  2002/04/15 07:05:36  dkrajzew
// new loading paradigm implemented
//
// Revision 1.1.1.1  2002/04/08 07:21:24  traffic
// new project name
//
// Revision 2.1  2002/03/20 08:16:54  dkrajzew
// strtok replaced by a StringTokenizer; NumericFormatException handling added
//
// Revision 2.0  2002/02/14 14:43:21  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.4  2002/02/13 15:40:41  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
// Revision 1.1  2001/12/06 13:36:05  traffic
// moved from netbuild
//
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
#include "NLHandlerBuilder1.h"
#include "SErrorHandler.h"
#include "../utils/XMLConvert.h"
#include "../utils/STRConvert.h"
#include "../utils/XMLBuildingExceptions.h"
#include "../utils/StringTokenizer.h"
#include "../utils/AttributesHandler.h"
#include "../microsim/MSBitSetLogic.h"
#include "../microsim/MSJunctionLogic.h"
#include "NLSAXHandler.h"
#include "NLLoadFilter.h"
#include "NLTags.h"
//#ifdef EXTERNAL_TEMPLATE_DEFINITION
#include "../microsim/MSBitSetLogic.cpp"
//#endif // EXTERNAL_TEMPLATE_DEFINITION


/* =========================================================================
 * using namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
NLHandlerBuilder1::NLHandlerBuilder1(NLContainer &container, LoadFilter filter)
    : NLSAXHandler(container, filter)
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
}

NLHandlerBuilder1::~NLHandlerBuilder1()
{
}

void
NLHandlerBuilder1::myStartElement(int element, const std::string &name, const Attributes &attrs)
{
    // check static net information
    if(wanted(LOADFILTER_NET)) {
        switch(element) {
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
            addJunctionKey(attrs);
            break;
        case NLTag_bitsetlogic:
            initLogic();
            break;
        default:
            break;
        }
    }
    // check dynamic components
    if(wanted(LOADFILTER_DYNAMIC)) {
        switch(element) {
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
}

void
NLHandlerBuilder1::chooseEdge(const Attributes &attrs) {
    string id;
    try {
        id = _attrHandler.getString(attrs, ATTR_ID);
        myContainer.chooseEdge(id);
    } catch (XMLUngivenParameterException &e) {
        SErrorHandler::add(e.getMessage("edge", "(ID_UNKNOWN!)"));
    } catch (XMLIdNotKnownException &e) {
        SErrorHandler::add(e.getMessage("edge", id));
    }
}

void
NLHandlerBuilder1::addLane(const Attributes &attrs) {
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
        } catch (XMLUngivenParameterException &e) {
            SErrorHandler::add(e.getMessage("edge", id));
        } catch (XMLNumericFormatException &e) {
            SErrorHandler::add(e.getMessage("edge", id));
        }
    } catch (XMLUngivenParameterException &e) {
        SErrorHandler::add(e.getMessage("edge", "(ID_UNKNOWN!)"));
    }
}

void
NLHandlerBuilder1::openAllowedEdge(const Attributes &attrs) {
    string id;
    try {
        id = _attrHandler.getString(attrs, ATTR_ID);
        myContainer.openAllowedEdge(id);
    } catch (XMLIdNotKnownException &e) {
        SErrorHandler::add(e.getMessage("cedge", id));
    } catch (XMLUngivenParameterException &e) {
        SErrorHandler::add(e.getMessage("edge", "(ID_UNKNOWN!)"));
    }
}

void
NLHandlerBuilder1::addVehicleType(const Attributes &attrs) {
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
        } catch (XMLUngivenParameterException &e) {
            SErrorHandler::add(e.getMessage("vehicletype", id));
        } catch (XMLNumericFormatException &e) {
            SErrorHandler::add(e.getMessage("vehicletype", id));
        }
    } catch (XMLUngivenParameterException &e) {
        SErrorHandler::add(e.getMessage("vehicletype", "(ID_UNKNOWN!)"));
    }
}

void
NLHandlerBuilder1::openRoute(const Attributes &attrs) {
    string id;
    try {
        id = _attrHandler.getString(attrs, ATTR_ID);
        myContainer.openRoute(id);
    } catch (XMLUngivenParameterException &e) {
        SErrorHandler::add(e.getMessage("route", id));
    } catch (XMLIdNotKnownException &e) {
        SErrorHandler::add(e.getMessage("route", "(ID_UNKNOWN!)"));
    }
}

void
NLHandlerBuilder1::addJunctionKey(const Attributes &attrs) {
    try {
        string key = _attrHandler.getString(attrs, ATTR_KEY);
        myContainer.addKey(key);
    } catch (XMLUngivenParameterException &e) {
        try {
            string id = _attrHandler.getString(attrs, ATTR_KEY);
            SErrorHandler::add(e.getMessage("key", id));
        }  catch (XMLUngivenParameterException &e) {
            SErrorHandler::add(e.getMessage("junction", "(!ID_UNKNOWN)"));
        }
    }
}

void
NLHandlerBuilder1::initLogic() {
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
NLHandlerBuilder1::addLogicItem(const Attributes &attrs) {
    if(_responseSize>0&&_requestSize>0) {
        int request = -1;
        string response;
        try {
            request = _attrHandler.getInt(attrs, ATTR_REQUEST);
        } catch (XMLUngivenParameterException &e) {
            SErrorHandler::add("Missing request key...");
        } catch (XMLNumericFormatException e) {
            SErrorHandler::add("The request key is not numeric.");
        }
        try {
            response = _attrHandler.getString(attrs, ATTR_RESPONSE);
        } catch (XMLUngivenParameterException &e) {
            SErrorHandler::add("Missing respond for a request");
        }
        if(request>=0 && response.length()>0)
            addLogicItem(request, response);
    } else {
        SErrorHandler::add("The request size,  the response size or the number of lanes is not given! Contact your net supplier");
    }
}

void
NLHandlerBuilder1::addTrafoItem(const Attributes &attrs) {
    int lane = -1;
    string links;
    try {
        lane = _attrHandler.getInt(attrs, ATTR_TO);
    } catch (XMLUngivenParameterException &e) {
        SErrorHandler::add("Missing lane number...");
    } catch (XMLNumericFormatException e) {
        SErrorHandler::add("The lane number is not numeric.");
    }
    try {
        links = _attrHandler.getString(attrs, ATTR_FROM);
    } catch (XMLUngivenParameterException &e) {
        SErrorHandler::add("Missing links in a lane transformation.");
    }
    if(lane>=0 && links.length()>0)
        addTrafoItem(links, lane);
}


void
NLHandlerBuilder1::myEndElement(int element, const std::string &name)
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
        default:
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
    if(wanted(LOADFILTER_DYNAMIC)) {
        switch(element) {
        case NLTag_bitsetlogic:
            closeLogic();
            break;
        }
    }
}


void
NLHandlerBuilder1::myCharacters(int element, const std::string &name, const std::string &chars)
{
    // check static net information
    if(wanted(LOADFILTER_NET)) {
        switch(element) {
        case NLTag_cedge:
            addAllowedEdges(chars);
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
        }
    }

}

void
NLHandlerBuilder1::addAllowedEdges(const std::string &chars) {
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
NLHandlerBuilder1::addRouteElements(const std::string &name, const std::string &chars) {
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
NLHandlerBuilder1::setRequestSize(const std::string &chars) {
    try {
        _requestSize = STRConvert::_2int(chars);
        m_pActiveLogic->reserve(_requestSize);
    } catch (XMLUngivenParameterException &e) {
        SErrorHandler::add("Missing request size.");
    } catch (XMLNumericFormatException e) {
        SErrorHandler::add("The request size is not numeric! Contact your netconvert-programmer.");
    }
}

void
NLHandlerBuilder1::setResponseSize(const std::string &chars) {
    try {
        _responseSize = STRConvert::_2int(chars);
        m_pActiveTrafo->reserve(_responseSize);
    } catch (XMLUngivenParameterException &e) {
        SErrorHandler::add("Missing response size.");
    } catch (XMLNumericFormatException e) {
        SErrorHandler::add("Response size is not numeric! Contact your netconvert-programmer.");
    }
}

void
NLHandlerBuilder1::setLaneNumber(const std::string &chars) {
    try {
        _laneNo = STRConvert::_2int(chars);
        m_pActiveTrafo->reserve(_responseSize);
    } catch (XMLUngivenParameterException &e) {
        SErrorHandler::add("Missing lane number.");
    } catch (XMLNumericFormatException e) {
        SErrorHandler::add("Lane number is not numeric! Contact your netconvert-programmer.");
    }
}

void
NLHandlerBuilder1::setKey(const std::string &chars) {
    if(chars.length()==0) {
        SErrorHandler::add("No key given for the current junction logic.");
        return;
    }
    m_Key = chars;
}


void
NLHandlerBuilder1::addLogicItem(int request, string response) {
    bitset<64> use(response);
    (*m_pActiveLogic)[request] = use;
    _requestItems++;
}

void
NLHandlerBuilder1::addTrafoItem(string links, int lane) {
    bitset<64> use(links);
    (*m_pActiveTrafo)[lane] = use;
    _trafoItems++;
}

void
NLHandlerBuilder1::closeLogic() {
    if(_trafoItems!=_laneNo||_requestItems!=_requestSize) {
        SErrorHandler::add(
	        string("The description for the junction logic '") +
	        m_Key +
	        string("' is malicious."));
    }
    MSJunctionLogic *logic = new MSBitsetLogic(_requestSize, _laneNo, m_pActiveLogic, m_pActiveTrafo);
    MSJunctionLogic::dictionary(m_Key, logic);
}

std::string
NLHandlerBuilder1::getMessage() const {
    return "Loading routes, lanes and vehicle types...";
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NLHandlerBuilder1.icc"
//#endif

// Local Variables:
// mode:C++
// End:
