/****************************************************************************/
/// @file    NIImporter_SUMO.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 14.04.2008
/// @version $Id$
///
// Importer for networks stored in SUMO format
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/TplConvert.h>
#include <utils/common/MsgHandler.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBNetBuilder.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/FileHelpers.h>
#include <utils/xml/XMLSubSys.h>
#include "NIImporter_SUMO.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static methods (interface in this case)
// ---------------------------------------------------------------------------
void
NIImporter_SUMO::loadNetwork(const OptionsCont &oc, NBNetBuilder &nb) {
    // check whether the option is set (properly)
    if (!oc.isUsableFileList("sumo-net")) {
        return;
    }
    // build the handler
    NIImporter_SUMO handler(nb.getNodeCont());
    // parse file(s)
    std::vector<std::string> files = oc.getStringVector("sumo-net");
    for (std::vector<std::string>::const_iterator file=files.begin(); file!=files.end(); ++file) {
        if (!FileHelpers::exists(*file)) {
            MsgHandler::getErrorInstance()->inform("Could not open sumo-net-file '" + *file + "'.");
            return;
        }
        handler.setFileName(*file);
        MsgHandler::getMessageInstance()->beginProcessMsg("Parsing sumo-net from '" + *file + "'...");
        XMLSubSys::runParser(handler, *file);
        MsgHandler::getMessageInstance()->endProcessMsg("done.");
    }
    // build edges
    std::map<std::string, EdgeAttrs*> &loadedEdges = handler.myEdges;
    NBNodeCont &nodesCont = nb.getNodeCont();
    NBEdgeCont &edgesCont = nb.getEdgeCont();
    for (std::map<std::string, EdgeAttrs*>::const_iterator i=loadedEdges.begin(); i!=loadedEdges.end(); ++i) {
        EdgeAttrs *ed = (*i).second;
        // skip internal edges (provisionally)
        if (ed->func == "internal") {
            continue;
        }
        // get and check the nodes
        NBNode *from = nodesCont.retrieve(ed->fromNode);
        NBNode *to = nodesCont.retrieve(ed->toNode);
        if (from==0) {
            MsgHandler::getErrorInstance()->inform("Edge's '" + ed->id + "' from-node '" + ed->fromNode + "' is not known.");
            continue;
        }
        if (to==0) {
            MsgHandler::getErrorInstance()->inform("Edge's '" + ed->id + "' to-node '" + ed->toNode + "' is not known.");
            continue;
        }
        // build and insert the edge
        NBEdge *e = new NBEdge(ed->id, from, to, ed->type, ed->maxSpeed, (unsigned int) ed->lanes.size(), ed->priority);
        if (!edgesCont.insert(e)) {
            MsgHandler::getErrorInstance()->inform("Could not insert edge '" + ed->id + "'.");
            delete e;
            continue;
        }
        ed->builtEdge = edgesCont.retrieve(ed->id);
    }
    // assign lane attributes (edges are built)
    for (std::map<std::string, EdgeAttrs*>::const_iterator i=loadedEdges.begin(); i!=loadedEdges.end(); ++i) {
        EdgeAttrs *ed = (*i).second;
        if (ed->builtEdge==0) {
            // earlier errors
            continue;
        }
        for (unsigned int j=0; j<(unsigned int) ed->lanes.size(); ++j) {
            const std::vector<EdgeLane> &connections = ed->lanes[j]->connections;
            for (std::vector<EdgeLane>::const_iterator k=connections.begin(); k!=connections.end(); ++k) {
                if ((*k).lane!="SUMO_NO_DESTINATION") {
                    std::string lane = (*k).lane;
                    std::string edge;
                    size_t index;
                    interpretLaneID(lane, edge, index);
                    if (loadedEdges.find(edge)==loadedEdges.end()) {
                        MsgHandler::getErrorInstance()->inform("Unknown edge given in succlane (for lane '" + lane + "').");
                        continue;
                    }
                    NBEdge *ce = loadedEdges.find(edge)->second->builtEdge;
                    if (ce==0) {
                        // earlier error or edge removal
                        continue;
                    }
                    ed->builtEdge->addLane2LaneConnection(j, ce, index, NBEdge::L2L_VALIDATED);
                }
            }
        }
    }
    // clean up
    for (std::map<std::string, EdgeAttrs*>::const_iterator i=loadedEdges.begin(); i!=loadedEdges.end(); ++i) {
        EdgeAttrs *ed = (*i).second;
        for (std::vector<LaneAttrs*>::const_iterator j=ed->lanes.begin(); j!=ed->lanes.end(); ++j) {
            delete *j;
        }
        delete ed;
    }
}



// ---------------------------------------------------------------------------
// loader methods
// ---------------------------------------------------------------------------
NIImporter_SUMO::NIImporter_SUMO(NBNodeCont &nc)
        : SUMOSAXHandler("sumo-network"),
        myNodeCont(nc), myCurrentEdge(0) {}


NIImporter_SUMO::~NIImporter_SUMO() throw() {
}


void
NIImporter_SUMO::myStartElement(SumoXMLTag element,
                                const SUMOSAXAttributes &attrs) throw(ProcessError) {
    switch (element) {
    case SUMO_TAG_EDGE:
        addEdge(attrs);
        break;
    case SUMO_TAG_LANE:
        if (myCurrentEdge!=0) {
            addLane(attrs);
        }
        break;
    case SUMO_TAG_JUNCTION:
        addJunction(attrs);
        break;
    case SUMO_TAG_SUCC:
        addSuccEdge(attrs);
        break;
    case SUMO_TAG_SUCCLANE:
        addSuccLane(attrs);
        break;
    default:
        break;
    }
}


void
NIImporter_SUMO::myCharacters(SumoXMLTag element,
                              const std::string &chars) throw(ProcessError) {
    switch (element) {
    case SUMO_TAG_LANE:
        // @deprecated At some time, SUMO_ATTR_SHAPE will be mandatory
        if (myCurrentLane!=0&&chars.length()!=0) {
            bool ok = true;
            myCurrentLane->shape = GeomConvHelper::parseShapeReporting(chars, "lane", 0, ok, false);
        }
        break;
    default:
        break;
    }
}



void
NIImporter_SUMO::myEndElement(SumoXMLTag element) throw(ProcessError) {
    switch (element) {
    case SUMO_TAG_EDGE:
        if (myEdges.find(myCurrentEdge->id)!=myEdges.end()) {
            MsgHandler::getErrorInstance()->inform("Edge '" + myCurrentEdge->id + "' occured at least twice in the input.");
        } else {
            myEdges[myCurrentEdge->id] = myCurrentEdge;
        }
        myCurrentEdge = 0;
        break;
    case SUMO_TAG_LANE:
        if (myCurrentEdge!=0) {
            myCurrentEdge->maxSpeed = MAX2(myCurrentEdge->maxSpeed, myCurrentLane->maxSpeed);
            myCurrentEdge->lanes.push_back(myCurrentLane);
        }
        myCurrentLane = 0;
        break;
    default:
        break;
    }
}


void
NIImporter_SUMO::addEdge(const SUMOSAXAttributes &attrs) {
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes("edge", id)) {
        return;
    }
    bool ok = true;
    myCurrentEdge = new EdgeAttrs;
    myCurrentEdge->id = id;
    // get the type
    myCurrentEdge->type = attrs.getOptStringReporting(SUMO_ATTR_TYPE, "edge", id.c_str(), ok, "");
    // get the function
    myCurrentEdge->func = attrs.getOptStringReporting(SUMO_ATTR_FUNCTION, "edge", id.c_str(), ok, "normal");
    // get the origin and the destination node
    myCurrentEdge->fromNode = attrs.getOptStringReporting(SUMO_ATTR_FROM, "edge", id.c_str(), ok, "");
    myCurrentEdge->toNode = attrs.getOptStringReporting(SUMO_ATTR_TO, "edge", id.c_str(), ok, "");
    myCurrentEdge->priority = attrs.getOptIntReporting(SUMO_ATTR_PRIORITY, "edge", id.c_str(), ok, -1);
    myCurrentEdge->maxSpeed = 0;
    myCurrentEdge->builtEdge = 0;
}


void
NIImporter_SUMO::addLane(const SUMOSAXAttributes &attrs) {
    myCurrentLane = new LaneAttrs;
    bool ok = true;
    myCurrentLane->maxSpeed = attrs.getOptSUMORealReporting(SUMO_ATTR_MAXSPEED, "lane", 0, ok, -1);
    myCurrentLane->depart = attrs.getOptBoolReporting(SUMO_ATTR_DEPART, 0, 0, ok, false);
    if (attrs.hasAttribute(SUMO_ATTR_SHAPE)) {
        // @deprecated At some time, SUMO_ATTR_SHAPE will be mandatory
        myCurrentLane->shape = GeomConvHelper::parseShapeReporting(attrs.getStringReporting(SUMO_ATTR_SHAPE, "lane", 0, ok), "lane", 0, ok, false);
    }
}


void
NIImporter_SUMO::addJunction(const SUMOSAXAttributes &attrs) {
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes("junction", id)) {
        return;
    }
    if (id[0]==':') {
        return;
    }
    bool ok = true;
    SUMOReal x = attrs.getOptSUMORealReporting(SUMO_ATTR_X, "junction", id.c_str(), ok, -1);
    SUMOReal y = attrs.getOptSUMORealReporting(SUMO_ATTR_Y, "junction", id.c_str(), ok, -1);
    // !!! this is too simplified! A proper error check should be done
    if (x==-1||y==-1) {
        MsgHandler::getErrorInstance()->inform("Junction '" + id + "' has an invalid position.");
        return;
    }
    Position2D pos(x, y);
    if (!GeoConvHelper::x2cartesian(pos)) {
        MsgHandler::getErrorInstance()->inform("Unable to project coordinates for junction " + id + ".");
        return;
    }
    NBNode *node = new NBNode(id, pos);
    if (!myNodeCont.insert(node)) {
        MsgHandler::getErrorInstance()->inform("Problems on adding junction '" + id + "'.");
        delete node;
        return;
    }
}


void
NIImporter_SUMO::addSuccEdge(const SUMOSAXAttributes &attrs) {
    bool ok = true;
    std::string edge_id = attrs.getStringReporting(SUMO_ATTR_EDGE, "succ", 0, ok); // 
    myCurrentEdge = 0;
    if (myEdges.find(edge_id)==myEdges.end()) {
        MsgHandler::getErrorInstance()->inform("Unknown edge '" + edge_id + "' given in succedge.");
        return;
    }
    myCurrentEdge = myEdges.find(edge_id)->second;
    std::string lane_id = attrs.getStringReporting(SUMO_ATTR_LANE, "succ", 0, ok); // 
    myCurrentLane = getLaneAttrsFromID(myCurrentEdge, lane_id);
}


void
NIImporter_SUMO::addSuccLane(const SUMOSAXAttributes &attrs) {
    if (myCurrentLane==0) {
        // had error
        return;
    }
    bool ok = true;
    std::string lane = attrs.getOptStringReporting(SUMO_ATTR_LANE, 0, 0, ok, "");
    EdgeLane el;
    el.lane = lane;
    myCurrentLane->connections.push_back(el);
}


NIImporter_SUMO::LaneAttrs* 
NIImporter_SUMO::getLaneAttrsFromID(EdgeAttrs* edge, std::string lane_id) {
    std::string edge_id;
    size_t index;
    interpretLaneID(lane_id, edge_id, index);
    assert(edge->id == edge_id);
    if (edge->lanes.size()<(size_t) index) {
        MsgHandler::getErrorInstance()->inform("Unknown lane '" + lane_id + "' given in succedge.");
        return 0;
    } else {
        return edge->lanes[(size_t) index];
    }
}


void
NIImporter_SUMO::interpretLaneID(const std::string &lane_id, std::string &edge_id, size_t &index) {
    // assume lane_id = edge_id + '_' + index
    size_t sep_index = lane_id.rfind('_');
    if (sep_index == std::string::npos) {
        MsgHandler::getErrorInstance()->inform("Invalid lane id '" + lane_id + "' (missing '_').");
    }
    edge_id = lane_id.substr(0, sep_index);
    std::string index_string = lane_id.substr(sep_index + 1);
    try {
        index = TplConvert<char>::_2int(index_string.c_str());
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform("Invalid lane index '" + index_string + "' for lane '" + lane_id + "'.");
    }
}

/****************************************************************************/

