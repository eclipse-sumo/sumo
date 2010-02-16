/****************************************************************************/
/// @file    NIXMLConnectionsHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 17 Oct 2002
/// @version $Id$
///
// Importer for edge connections stored in XML
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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
        myHaveReportedAboutFunctionDeprecation(false) {}


NIXMLConnectionsHandler::~NIXMLConnectionsHandler() throw() {}


void
NIXMLConnectionsHandler::myStartElement(SumoXMLTag element,
                                        const SUMOSAXAttributes &attrs) throw(ProcessError) {
    if (element==SUMO_TAG_RESET) {
        bool ok = true;
        string from = attrs.getStringReporting(SUMO_ATTR_FROM, "reset", 0, ok);
        string to = attrs.getStringReporting(SUMO_ATTR_TO, "reset", 0, ok);
        if (!ok) {
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
        bool ok = true;
        string from = attrs.getOptStringReporting(SUMO_ATTR_FROM, "connection", 0, ok, "");
        string to = attrs.getOptStringReporting(SUMO_ATTR_TO, "connection", 0, ok, "");
        string laneConn = attrs.getOptStringReporting(SUMO_ATTR_LANE, "connection", 0, ok, "");
        if (!ok) {
            return;
        }
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
        if (laneConn=="") {
            fromEdge->addEdge2EdgeConnection(toEdge);
        } else {
            parseLaneBound(attrs, fromEdge, toEdge);
        }
    }
    if (element==SUMO_TAG_PROHIBITION) {
        bool ok = true;
        string prohibitor = attrs.getOptStringReporting(SUMO_ATTR_PROHIBITOR, "prohibition", 0, ok, "");
        string prohibited = attrs.getOptStringReporting(SUMO_ATTR_PROHIBITED, "prohibition", 0, ok, "");
        if (!ok) {
            return;
        }
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
        const string &def) throw() {
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
                                        NBEdge *to) throw() {
    if (to==0) {
        // do nothing if it's a dead end
        return;
    }
    bool ok = true;
    string laneConn = attrs.getOptStringReporting(SUMO_ATTR_LANE, "connection", 0, ok, "");
    // split the information
    StringTokenizer st(laneConn, ':');
    if (st.size()!=2) {
        MsgHandler::getErrorInstance()->inform("Invalid lane to lane connection from '" +
                                               from->getID() + "' to '" + to->getID() + "'.");
        return;
    }
    bool mayDefinitelyPass = attrs.getOptBoolReporting(SUMO_ATTR_PASS, "connection", 0, ok, false);
    if (!ok) {
        return;
    }
    // get the begin and the end lane
    int fromLane;
    int toLane;
    try {
        fromLane = TplConvertSec<char>::_2intSec(st.next().c_str(), -1);
        toLane = TplConvertSec<char>::_2intSec(st.next().c_str(), -1);
        if (fromLane<0 || fromLane>=from->getNoLanes() || toLane<0 || toLane>=to->getNoLanes()) {
            MsgHandler::getErrorInstance()->inform("False lane index in connection from '" + from->getID() + "' to '" + to->getID() + "'.");
            return;
        }
        if (from->hasConnectionTo(to, toLane)) {
            WRITE_WARNING("Target lane '" + to->getID() + "_" + toString(toLane) + "' is already connected from '" + from->getID() + "'.");
        }
        if (!from->addLane2LaneConnection(fromLane, to, toLane, NBEdge::L2L_USER, true, mayDefinitelyPass)) {
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
            if (nFrom==0||!nFrom->addLane2LaneConnection(fromLane, to, toLane, NBEdge::L2L_USER, false, mayDefinitelyPass)) {
                WRITE_WARNING("Could not set loaded connection from '" + from->getID() + "_" + toString<int>(fromLane) + "' to '" + to->getID() + "_" + toString<int>(toLane) + "'.");
            } else {
                from = nFrom;
            }
        }
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("At least one of the defined lanes was not numeric");
    }
    //
    bool keepUncontrolled = attrs.getOptBoolReporting(SUMO_ATTR_UNCONTROLLED, 0, 0, ok, false);
    if (keepUncontrolled) {
        from->disableConnection4TLS(fromLane, to, toLane);
    }
}



/****************************************************************************/

