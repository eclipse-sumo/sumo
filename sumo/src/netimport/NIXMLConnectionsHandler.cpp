/****************************************************************************/
/// @file    NIXMLConnectionsHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 17 Oct 2002
/// @version $Id:NIXMLConnectionsHandler.cpp 4701 2007-11-09 14:29:29Z dkrajzew $
///
// Importer for edge connections stored in XML
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
#include <iostream>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>
#include "NIXMLConnectionsHandler.h"
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNode.h>
#include <utils/common/StringTokenizer.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/ToString.h>
#include <utils/common/TplConvert.h>
#include <utils/common/TplConvertSec.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
NIXMLConnectionsHandler::NIXMLConnectionsHandler(NBEdgeCont &ec) throw()
        : SUMOSAXHandler("xml-connection-description"), myEdgeCont(ec),
        myHaveReportedAboutFunctionDeprecation(false)
{}


NIXMLConnectionsHandler::~NIXMLConnectionsHandler() throw()
{}


void
NIXMLConnectionsHandler::myStartElement(SumoXMLTag element,
                                        const SUMOSAXAttributes &attrs) throw(ProcessError)
{
    if (element==SUMO_TAG_RESET) {
        string from = attrs.getStringSecure(SUMO_ATTR_FROM, "");
        string to = attrs.getStringSecure(SUMO_ATTR_TO, "");
        if (from.length()==0) {
            MsgHandler::getErrorInstance()->inform("A from-edge is not specified within one of the connections-resets.");
            return;
        }
        if (to.length()==0) {
            MsgHandler::getErrorInstance()->inform("A to-edge is not specified within one of the connection-resets.");
            return;
        }
        NBEdge *fromEdge = myEdgeCont.retrieve(from);
        NBEdge *toEdge = myEdgeCont.retrieve(to);
        if (fromEdge==0) {
            MsgHandler::getErrorInstance()->inform("The connection-source edge '" + from + "' to reset is not known.");
            return;
        }
        if (toEdge==0) {
            MsgHandler::getErrorInstance()->inform("The connection-destination edge '" + to + "' to reset is not known.");
            return;
        }
        fromEdge->removeFromConnections(toEdge);
    }

    if (element==SUMO_TAG_CONNECTION) {
        string from = attrs.getStringSecure(SUMO_ATTR_FROM, "");
        string to = attrs.getStringSecure(SUMO_ATTR_TO, "");
        if (from.length()==0) {
            MsgHandler::getErrorInstance()->inform("A from-edge is not specified within one of the connections");
            return;
        }
        // extract edges
        NBEdge *fromEdge = myEdgeCont.retrieve(from);
        NBEdge *toEdge = to.length()!=0 ? myEdgeCont.retrieve(to) : 0;
        // check whether they are valid
        if (fromEdge==0) {
            MsgHandler::getErrorInstance()->inform("The connection-source edge '" + from + "' is not known.");
            return;
        }
        if (toEdge==0 && to.length()!=0) {
            MsgHandler::getErrorInstance()->inform("The connection-destination edge '" + to + "' is not known.");
            return;
        }
        // parse optional lane information
        if (!myHaveReportedAboutFunctionDeprecation&&attrs.hasAttribute(SUMO_ATTR_TYPE)) {
            MsgHandler::getWarningInstance()->inform("While parsing connections: 'type' is deprecated.\n All occurences are ignored.");
            myHaveReportedAboutFunctionDeprecation = true;
        }
        string laneConn = attrs.getStringSecure(SUMO_ATTR_LANE, "");
        if (laneConn=="") {
            fromEdge->addEdge2EdgeConnection(toEdge);
        } else {
            parseLaneBound(attrs, fromEdge, toEdge);
        }
    }
    if (element==SUMO_TAG_PROHIBITION) {
        string prohibitor = attrs.getStringSecure(SUMO_ATTR_PROHIBITOR, "");
        string prohibited = attrs.getStringSecure(SUMO_ATTR_PROHIBITED, "");
        NBConnection prohibitorC = parseConnection("prohibitor", prohibitor);
        NBConnection prohibitedC = parseConnection("prohibited", prohibited);
        if (prohibitorC.getFrom()==0||prohibitedC.getFrom()==0) {
            // something failed
            return;
        }
        NBNode *n = prohibitorC.getFrom()->getToNode();
        n->addSortedLinkFoes(prohibitorC, prohibitedC);
    }
}


NBConnection
NIXMLConnectionsHandler::parseConnection(const std::string &defRole,
        const string &def) throw()
{
    // split from/to
    size_t div = def.find("->");
    if (div==string::npos) {
        MsgHandler::getErrorInstance()->inform("Missing connection divider in " + defRole + " '" + def + "'");
        return NBConnection(0, 0);
    }
    string fromDef = def.substr(0, div);
    string toDef = def.substr(div+2);

    // retrieve the edges
    // check whether the definition includes a lane information (do not process it)
    if (fromDef.find('_')!=string::npos) {
        fromDef = fromDef.substr(0, fromDef.find('_'));
    }
    if (toDef.find('_')!=string::npos) {
        toDef = toDef.substr(0, toDef.find('_'));
    }
    // retrieve them now
    NBEdge *fromE = myEdgeCont.retrieve(fromDef);
    NBEdge *toE = myEdgeCont.retrieve(toDef);
    // check
    if (fromE==0) {
        MsgHandler::getErrorInstance()->inform("Could not find edge '" + fromDef + "' in " + defRole + " '" + def + "'");
        return NBConnection(0, 0);
    }
    if (toE==0) {
        MsgHandler::getErrorInstance()->inform("Could not find edge '" + toDef + "' in " + defRole + " '" + def + "'");
        return NBConnection(0, 0);
    }
    return NBConnection(fromE, toE);
}


void
NIXMLConnectionsHandler::parseLaneBound(const SUMOSAXAttributes &attrs,
                                        NBEdge *from,
                                        NBEdge *to) throw()
{
    if (to==0) {
        // do nothing if it's a dead end
        return;
    }
    string laneConn = attrs.getStringSecure(SUMO_ATTR_LANE, "");
    if (laneConn.length()==0) {
        MsgHandler::getErrorInstance()->inform("Not specified lane to lane connection");
        return;
    } else {
        // split the information
        StringTokenizer st(laneConn, ':');
        if (st.size()!=2) {
            MsgHandler::getErrorInstance()->inform("False lane to lane connection occured.");
            return;
        }
        // get the begin and the end lane
        int fromLane;
        int toLane;
        try {
            fromLane = TplConvertSec<char>::_2intSec(st.next().c_str(), -1);
            toLane = TplConvertSec<char>::_2intSec(st.next().c_str(), -1);
            if (!from->addLane2LaneConnection(fromLane, to, toLane, NBEdge::L2L_USER, true)) {
                NBEdge *nFrom = from;
                bool toNext = true;
                do {
                    if (nFrom->getToNode()->getOutgoingEdges().size()!=1) {
                        toNext = false;
                        break;
                    }
                    NBEdge *t = nFrom->getToNode()->getOutgoingEdges()[0];
                    if (t->getID().substr(0, t->getID().find('/'))!=nFrom->getID().substr(0, nFrom->getID().find('/'))) {
                        toNext = false;
                        break;
                    }
                    if (toNext) {
                        nFrom = t;
                    }
                } while (toNext);
                if (nFrom==0||!nFrom->addLane2LaneConnection(fromLane, to, toLane, NBEdge::L2L_USER)) {
                    WRITE_WARNING("Could not set loaded connection from '" + from->getID() + "_" + toString<int>(fromLane) + "' to '" + to->getID() + "_" + toString<int>(toLane) + "'.");
                } else {
                    from = nFrom;
                }
            }
        } catch (NumberFormatException &) {
            MsgHandler::getErrorInstance()->inform("At least one of the defined lanes was not numeric");
        }
        //
        try {
            bool keepUncontrolled = attrs.getBoolSecure(SUMO_ATTR_UNCONTROLLED, false);
            if (keepUncontrolled) {
                from->disableConnection4TLS(fromLane, to, toLane);
            }
        } catch (BoolFormatException &) {
            MsgHandler::getErrorInstance()->inform("The definition about being (un)controlled is not a valid bool.");
        }
    }
}



/****************************************************************************/

