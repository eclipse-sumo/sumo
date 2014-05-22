/****************************************************************************/
/// @file    NIXMLNodesHandler.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Importer for network nodes stored in XML
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/StringTokenizer.h>
#include <utils/options/OptionsCont.h>
#include <utils/geom/GeoConvHelper.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBTrafficLightLogicCont.h>
#include <netbuild/NBOwnTLDef.h>
#include <netbuild/NBNetBuilder.h>
#include "NIXMLNodesHandler.h"
#include "NIImporter_SUMO.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NIXMLNodesHandler::NIXMLNodesHandler(NBNodeCont& nc,
                                     NBTrafficLightLogicCont& tlc,
                                     OptionsCont& options) :
    SUMOSAXHandler("xml-nodes - file"),
    myOptions(options),
    myNodeCont(nc),
    myTLLogicCont(tlc),
    myLocation(0)
{}


NIXMLNodesHandler::~NIXMLNodesHandler() {
    delete myLocation;
}


void
NIXMLNodesHandler::myStartElement(int element,
                                  const SUMOSAXAttributes& attrs) {
    switch (element) {
        case SUMO_TAG_LOCATION:
            myLocation = NIImporter_SUMO::loadLocation(attrs);
            break;
        case SUMO_TAG_NODE:
            addNode(attrs);
            break;
        case SUMO_TAG_JOIN:
            addJoinCluster(attrs);
            break;
        case SUMO_TAG_JOINEXCLUDE:
            addJoinExclusion(attrs);
            break;
        case SUMO_TAG_DELETE:
            deleteNode(attrs);
            break;
        default:
            break;
    }
}


void
NIXMLNodesHandler::addNode(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    // get the id, report a warning if not given or empty...
    myID = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        return;
    }
    NBNode* node = myNodeCont.retrieve(myID);
    // retrieve the position of the node
    bool xOk = false;
    bool yOk = false;
    bool needConversion = true;
    if (node != 0) {
        myPosition = node->getPosition();
        xOk = yOk = true;
        needConversion = false;
    }
    if (attrs.hasAttribute(SUMO_ATTR_X)) {
        myPosition.set(attrs.get<SUMOReal>(SUMO_ATTR_X, myID.c_str(), ok), myPosition.y());
        xOk = true;
        needConversion = true;
    }
    if (attrs.hasAttribute(SUMO_ATTR_Y)) {
        myPosition.set(myPosition.x(), attrs.get<SUMOReal>(SUMO_ATTR_Y, myID.c_str(), ok));
        yOk = true;
        needConversion = true;
    }
    if (attrs.hasAttribute(SUMO_ATTR_Z)) {
        myPosition.set(myPosition.x(), myPosition.y(), attrs.get<SUMOReal>(SUMO_ATTR_Z, myID.c_str(), ok));
    }
    if (xOk && yOk) {
        if (needConversion && !NBNetBuilder::transformCoordinates(myPosition, true, myLocation)) {
            WRITE_ERROR("Unable to project coordinates for node '" + myID + "'.");
        }
    } else {
        WRITE_ERROR("Missing position (at node ID='" + myID + "').");
    }
    bool updateEdgeGeometries = node != 0 && myPosition != node->getPosition();
    // check whether the y-axis shall be flipped
    if (myOptions.getBool("flip-y-axis")) {
        myPosition.mul(1.0, -1.0);
    }
    // get the type
    SumoXMLNodeType type = NODETYPE_UNKNOWN;
    if (node != 0) {
        type = node->getType();
    }
    std::string typeS = attrs.getOpt<std::string>(SUMO_ATTR_TYPE, myID.c_str(), ok, "");
    if (SUMOXMLDefinitions::NodeTypes.hasString(typeS)) {
        type = SUMOXMLDefinitions::NodeTypes.get(typeS);
        if (type == NODETYPE_DEAD_END_DEPRECATED || type == NODETYPE_DEAD_END) {
            // dead end is a computed status. Reset this to unknown so it will
            // be corrected if additional connections are loaded
            type = NODETYPE_UNKNOWN;
        }
    }
    // check whether a prior node shall be modified
    if (node == 0) {
        node = new NBNode(myID, myPosition, type);
        if (!myNodeCont.insert(node)) {
            throw ProcessError("Could not insert node though checked this before (id='" + myID + "').");
        }
    } else {
        // remove previously set tls if this node is not controlled by a tls
        std::set<NBTrafficLightDefinition*> tls = node->getControllingTLS();
        node->removeTrafficLights();
        for (std::set<NBTrafficLightDefinition*>::iterator i = tls.begin(); i != tls.end(); ++i) {
            if ((*i)->getNodes().size() == 0) {
                myTLLogicCont.removeFully((*i)->getID());
            }
        }
        // patch information
        node->reinit(myPosition, type, updateEdgeGeometries);
    }
    // process traffic light definition
    if (type == NODETYPE_TRAFFIC_LIGHT || type == NODETYPE_TRAFFIC_LIGHT_NOJUNCTION) {
        processTrafficLightDefinitions(attrs, node);
    }
    // set optional shape
    PositionVector shape;
    if (attrs.hasAttribute(SUMO_ATTR_SHAPE)) {
        shape = attrs.getOpt<PositionVector>(SUMO_ATTR_SHAPE, myID.c_str(), ok, PositionVector());
        if (shape.size() > 2) {
            shape.closePolygon();
        }
        node->setCustomShape(shape);
    }
}


void
NIXMLNodesHandler::deleteNode(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    // get the id, report a warning if not given or empty...
    myID = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        return;
    }
    NBNode* node = myNodeCont.retrieve(myID);
    if (node == 0) {
        WRITE_WARNING("Ignoring tag '" + toString(SUMO_TAG_DELETE) + "' for unknown node '" +
                      myID + "'");
        return;
    } else {
        myNodeCont.extract(node, true);
    }
}


void
NIXMLNodesHandler::addJoinCluster(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    const std::string clusterString = attrs.get<std::string>(SUMO_ATTR_NODES, 0, ok);
    const std::vector<std::string> ids = StringTokenizer(clusterString).getVector();
    if (ok) {
        myNodeCont.addCluster2Join(std::set<std::string>(ids.begin(), ids.end()));
    }
}


void
NIXMLNodesHandler::addJoinExclusion(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    const std::vector<std::string> ids = StringTokenizer(
            attrs.get<std::string>(SUMO_ATTR_NODES, 0, ok)).getVector();
    if (ok) {
        myNodeCont.addJoinExclusion(ids);
    }
}


void
NIXMLNodesHandler::processTrafficLightDefinitions(const SUMOSAXAttributes& attrs,
        NBNode* currentNode) {
    // try to get the tl-id
    // if a tl-id is given, we will look whether this tl already exists
    //  if so, we will add the node to it (and to all programs with this id), otherwise allocate a new one with this id
    // if no tl-id exists, we will build a tl with the node's id
    std::set<NBTrafficLightDefinition*> tlDefs;
    bool ok = true;
    std::string tlID = attrs.getOpt<std::string>(SUMO_ATTR_TLID, 0, ok, "");
    std::string typeS = attrs.getOpt<std::string>(SUMO_ATTR_TLTYPE, 0, ok,
                        OptionsCont::getOptions().getString("tls.default-type"));
    TrafficLightType type;
    if (SUMOXMLDefinitions::TrafficLightTypes.hasString(typeS)) {
        type = SUMOXMLDefinitions::TrafficLightTypes.get(typeS);
    } else {
        WRITE_ERROR("Unknown traffic light type '" + typeS + "' for node '" + myID + "'.");
        return;
    }
    if (tlID != "" && myTLLogicCont.getPrograms(tlID).size() > 0) {
        // we already have definitions for this tlID
        const std::map<std::string, NBTrafficLightDefinition*>& programs = myTLLogicCont.getPrograms(tlID);
        std::map<std::string, NBTrafficLightDefinition*>::const_iterator it;
        for (it = programs.begin(); it != programs.end(); it++) {
            if (it->second->getType() != type) {
                WRITE_ERROR("Mismatched traffic light type '" + typeS + "' for tl '" + tlID + "'.");
                ok = false;
            } else {
                tlDefs.insert(it->second);
                it->second->addNode(currentNode);
            }
        }
    } else {
        // we need to add a new defition
        tlID = (tlID == "" ? myID : tlID);
        NBTrafficLightDefinition* tlDef = new NBOwnTLDef(tlID, currentNode, 0, type);
        if (!myTLLogicCont.insert(tlDef)) {
            // actually, nothing should fail here
            delete tlDef;
            throw ProcessError("Could not allocate tls '" + myID + "'.");
        }
        tlDefs.insert(tlDef);
    }
    // process inner edges which shall be controlled
    std::vector<std::string> controlledInner;
    SUMOSAXAttributes::parseStringVector(attrs.getOpt<std::string>(SUMO_ATTR_CONTROLLED_INNER, 0, ok, ""), controlledInner);
    if (controlledInner.size() != 0) {
        for (std::set<NBTrafficLightDefinition*>::iterator it = tlDefs.begin(); it != tlDefs.end(); it++) {
            (*it)->addControlledInnerEdges(controlledInner);
        }
    }
}



/****************************************************************************/

