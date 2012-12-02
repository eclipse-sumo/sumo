/****************************************************************************/
/// @file    NIXMLTrafficLightsHandler.h
/// @author  Jakob Erdmann
/// @date    2011-10-05
/// @version $Id$
///
// Importer for traffic lights stored in XML
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
#include <iostream>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <utils/common/StringTokenizer.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/ToString.h>
#include <utils/common/TplConvert.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/options/OptionsCont.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBOwnTLDef.h>
#include <netbuild/NBLoadedSUMOTLDef.h>
#include <netbuild/NBTrafficLightLogicCont.h>
#include "NIImporter_SUMO.h"
#include "NIXMLTrafficLightsHandler.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NIXMLTrafficLightsHandler::NIXMLTrafficLightsHandler(
    NBTrafficLightLogicCont& tlCont, NBEdgeCont& ec) :
    SUMOSAXHandler("xml-tllogics"),
    myTLLCont(tlCont),
    myEdgeCont(ec),
    myCurrentTL(0),
    myResetPhases(false)
{}


NIXMLTrafficLightsHandler::~NIXMLTrafficLightsHandler() {}


void
NIXMLTrafficLightsHandler::myStartElement(
    int element, const SUMOSAXAttributes& attrs) {
    switch (element) {
        case SUMO_TAG_TLLOGIC:
            myCurrentTL = initTrafficLightLogic(attrs, myCurrentTL);
            break;
        case SUMO_TAG_PHASE:
            if (myResetPhases) {
                myCurrentTL->getLogic()->resetPhases();
                myResetPhases = false;
            }
            NIImporter_SUMO::addPhase(attrs, myCurrentTL);
            break;
        case SUMO_TAG_CONNECTION:
            addTlConnection(attrs);
            break;
        case SUMO_TAG_DELETE:
            removeTlConnection(attrs);
            break;
        default:
            break;
    }
}


void
NIXMLTrafficLightsHandler::myEndElement(int element) {
    switch (element) {
        case SUMO_TAG_TLLOGIC:
            if (!myCurrentTL) {
                WRITE_ERROR("Unmatched closing tag for tl-logic.");
            } else {
                if (!myTLLCont.insert(myCurrentTL)) {
                    WRITE_MESSAGE("Updating program '" + myCurrentTL->getProgramID() +
                                  "' for traffic light '" + myCurrentTL->getID() + "'");
                }
                myCurrentTL = 0;
            }
            break;
        default:
            break;
    }
}


NBLoadedSUMOTLDef*
NIXMLTrafficLightsHandler::initTrafficLightLogic(const SUMOSAXAttributes& attrs, NBLoadedSUMOTLDef* currentTL) {
    if (currentTL) {
        WRITE_ERROR("Definition of tl-logic '" + currentTL->getID() + "' was not finished.");
        return 0;
    }
    bool ok = true;
    std::string id = attrs.getStringReporting(SUMO_ATTR_ID, 0, ok);
    std::string programID = attrs.getOptStringReporting(SUMO_ATTR_PROGRAMID, id.c_str(), ok, "<unknown>");
    SUMOTime offset = attrs.hasAttribute(SUMO_ATTR_OFFSET) ? TIME2STEPS(attrs.getSUMORealReporting(SUMO_ATTR_OFFSET, id.c_str(), ok)) : 0;

    // there are two scenarios to consider
    // 1) the tll.xml is loaded to update traffic lights defined in a net.xml:
    //   simply retrieve the loaded definitions and update them
    // 2) the tll.xml is loaded to define new traffic lights
    //   nod.xml will have triggered building of NBOwnTLDef. Replace it with NBLoadedSUMOTLDef
    NBLoadedSUMOTLDef* loadedDef = dynamic_cast<NBLoadedSUMOTLDef*>(myTLLCont.getDefinition(id, programID));
    if (loadedDef == 0) {
        // case 2
        NBOwnTLDef* newDef = dynamic_cast<NBOwnTLDef*>(myTLLCont.getDefinition(
                                 id, NBTrafficLightDefinition::DefaultProgramID));
        assert(newDef != 0);
        loadedDef = new NBLoadedSUMOTLDef(id, programID, offset);
        std::vector<NBNode*> nodes = newDef->getControlledNodes();
        for (std::vector<NBNode*>::iterator it = nodes.begin(); it != nodes.end(); it++) {
            (*it)->removeTrafficLight(newDef);
            (*it)->addTrafficLight(loadedDef);
        }
        myTLLCont.removeProgram(id, NBTrafficLightDefinition::DefaultProgramID);
        myTLLCont.insert(loadedDef);

        std::string type = attrs.getOptStringReporting(SUMO_ATTR_TYPE, 0, ok, toString(TLTYPE_STATIC));
        if (type != toString(TLTYPE_STATIC)) {
            WRITE_WARNING("Traffic light '" + id + "' has unsupported type '" + type + "' and will be converted to '" + toString(TLTYPE_STATIC) + "'");
        }
    }
    if (ok) {
        myResetPhases = true;
        return loadedDef;
    } else {
        return 0;
    }
}


void
NIXMLTrafficLightsHandler::addTlConnection(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    // parse identifying attributes
    NBEdge* from = retrieveEdge(attrs, SUMO_ATTR_FROM, ok);
    NBEdge* to = retrieveEdge(attrs, SUMO_ATTR_TO, ok);
    if (!ok) {
        return;
    }
    int fromLane = retrieveLaneIndex(attrs, SUMO_ATTR_FROM_LANE, from, ok);
    int toLane = retrieveLaneIndex(attrs, SUMO_ATTR_TO_LANE, to, ok);
    if (!ok) {
        return;
    }
    // retrieve connection
    const std::vector<NBEdge::Connection>& connections = from->getConnections();
    std::vector<NBEdge::Connection>::const_iterator con_it;
    con_it = find_if(connections.begin(), connections.end(),
                     NBEdge::connections_finder(fromLane, to, toLane));
    if (con_it == connections.end()) {
        WRITE_ERROR("Connection from=" + from->getID() + " to=" + to->getID() +
                    " fromLane=" + toString(fromLane) + " toLane=" + toString(toLane) + " not found");
        return;
    }
    NBEdge::Connection c = *con_it;
    // read other  attributes
    std::string tlID = attrs.getOptStringReporting(SUMO_ATTR_TLID, 0, ok, "");
    if (tlID == "") {
        // we are updating an existing tl-controlled connection
        tlID = c.tlID;
        assert(tlID != "");
    }
    int tlIndex = attrs.getOptIntReporting(SUMO_ATTR_TLLINKINDEX, 0, ok, -1);
    if (tlIndex == -1) {
        // we are updating an existing tl-controlled connection
        tlIndex = c.tlLinkNo;
    }

    // register the connection with all definitions
    const std::map<std::string, NBTrafficLightDefinition*>& programs = myTLLCont.getPrograms(tlID);
    if (programs.size() > 0) {
        std::map<std::string, NBTrafficLightDefinition*>::const_iterator it;
        for (it = programs.begin(); it != programs.end(); it++) {
            NBLoadedSUMOTLDef* tlDef = dynamic_cast<NBLoadedSUMOTLDef*>(it->second);
            if (tlDef) {
                tlDef->addConnection(from, c.toEdge, c.fromLane, c.toLane, tlIndex);
            } else {
                throw ProcessError("Corrupt traffic light definition '"
                                   + tlID + "' (program '" + it->first + "')");
            }
        }
    } else {
        WRITE_ERROR("The traffic light '" + tlID + "' is not known.");
    }
}


void
NIXMLTrafficLightsHandler::removeTlConnection(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string tlID = attrs.getStringReporting(SUMO_ATTR_TLID, 0, ok);
    // does the traffic light still exist?
    const std::map<std::string, NBTrafficLightDefinition*>& programs = myTLLCont.getPrograms(tlID);
    if (programs.size() > 0) {
        // parse identifying attributes
        NBEdge* from = retrieveEdge(attrs, SUMO_ATTR_FROM, ok);
        NBEdge* to = retrieveEdge(attrs, SUMO_ATTR_TO, ok);
        if (!ok) {
            return;
        }
        int fromLane = retrieveLaneIndex(attrs, SUMO_ATTR_FROM_LANE, from, ok);
        int toLane = retrieveLaneIndex(attrs, SUMO_ATTR_TO_LANE, to, ok);
        if (!ok) {
            return;
        }
        int tlIndex = attrs.getIntReporting(SUMO_ATTR_TLLINKINDEX, 0, ok);

        NBConnection conn(from, fromLane, to, toLane, tlIndex);
        // remove the connection from all definitions
        std::map<std::string, NBTrafficLightDefinition*>::const_iterator it;
        for (it = programs.begin(); it != programs.end(); it++) {
            NBLoadedSUMOTLDef* tlDef = dynamic_cast<NBLoadedSUMOTLDef*>(it->second);
            if (tlDef) {
                tlDef->removeConnection(conn, false);
            } else {
                throw ProcessError("Corrupt traffic light definition '"
                                   + tlID + "' (program '" + it->first + "')");
            }
        }
    }
}


NBEdge*
NIXMLTrafficLightsHandler::retrieveEdge(
    const SUMOSAXAttributes& attrs, SumoXMLAttr attr, bool& ok) {
    std::string edgeID = attrs.getStringReporting(attr, 0, ok);
    NBEdge* edge = myEdgeCont.retrieve(edgeID, true);
    if (edge == 0) {
        WRITE_ERROR("Unknown edge '" + edgeID + "' given in connection.");
        ok = false;
    }
    return edge;
}


int
NIXMLTrafficLightsHandler::retrieveLaneIndex(
    const SUMOSAXAttributes& attrs, SumoXMLAttr attr, NBEdge* edge, bool& ok) {
    int laneIndex = attrs.getIntReporting(attr, 0, ok);
    if (edge->getNumLanes() <= (size_t) laneIndex) {
        WRITE_ERROR("Invalid lane index '" + toString(laneIndex) + "' for edge '" + edge->getID() + "'.");
        ok = false;
    }
    return laneIndex;
}


/****************************************************************************/

