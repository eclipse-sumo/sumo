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
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/FileHelpers.h>
#include <utils/xml/XMLSubSys.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBNetBuilder.h>
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
    NIImporter_SUMO handler(nb.getNodeCont(), nb.getTLLogicCont());
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
        Position2DVector geom = approximateEdgeShape(ed);
        // build and insert the edge
        NBEdge *e = new NBEdge(ed->id, from, to, ed->type, ed->maxSpeed, (unsigned int) ed->lanes.size(), ed->priority, geom);
        if (!edgesCont.insert(e)) {
            MsgHandler::getErrorInstance()->inform("Could not insert edge '" + ed->id + "'.");
            delete e;
            continue;
        }
        ed->builtEdge = edgesCont.retrieve(ed->id);
    }
    // assign further lane attributes (edges are built)
    for (std::map<std::string, EdgeAttrs*>::const_iterator i=loadedEdges.begin(); i!=loadedEdges.end(); ++i) {
        EdgeAttrs *ed = (*i).second;
        NBEdge *nbe = ed->builtEdge;
        if (nbe == 0) { // inner edge
            continue;
        }
        for (unsigned int fromLaneIndex=0; fromLaneIndex<(unsigned int) ed->lanes.size(); ++fromLaneIndex) {
            LaneAttrs *lane = ed->lanes[fromLaneIndex];
            // connections
            const std::vector<Connection> &connections = lane->connections;
            for (std::vector<Connection>::const_iterator conn=connections.begin(); conn!=connections.end(); ++conn) {
                if (conn->lane!="SUMO_NO_DESTINATION") {
                    std::string toEdgeID;
                    size_t toLaneIndex;
                    interpretLaneID(conn->lane, toEdgeID, toLaneIndex);
                    if (loadedEdges.find(toEdgeID)==loadedEdges.end()) {
                        MsgHandler::getErrorInstance()->inform(
                                "Unknown edge '" + toEdgeID + "' given in succlane (for lane '" + conn->lane + "').");
                        continue;
                    }
                    NBEdge *toEdge = loadedEdges.find(toEdgeID)->second->builtEdge;
                    if (toEdge==0) {
                        WRITE_WARNING("target edge '" + toEdgeID + "' not built");
                        continue;
                    }
                    nbe->addLane2LaneConnection(fromLaneIndex, toEdge, (unsigned int)toLaneIndex, NBEdge::L2L_VALIDATED);

                    // maybe we have a tls-controlled connection
                    if (conn->tlID != "") {
                        NBLoadedSUMOTLDef *tl = (NBLoadedSUMOTLDef*)handler.myTLLCont.getDefinition(conn->tlID);
                        if (tl) {
                            tl->addConnection(nbe, toEdge, fromLaneIndex, (unsigned int)toLaneIndex, conn->tlLinkNo);
                        } else {
                            WRITE_ERROR("The traffic light '" + conn->tlID + "' is not known.");
                        }
                    }
                }
            }
            // allow/disallow
            SUMOVehicleClasses allowed;
            SUMOVehicleClasses disallowed;
            parseVehicleClasses(lane->allow, lane->disallow, allowed, disallowed);
            nbe->setVehicleClasses(allowed, disallowed, fromLaneIndex);
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
NIImporter_SUMO::NIImporter_SUMO(NBNodeCont &nc, NBTrafficLightLogicCont &tllc)
        : SUMOSAXHandler("sumo-network"),
        myNodeCont(nc), 
        myTLLCont(tllc),
        myCurrentEdge(0),
        myCurrentLane(0),
        myCurrentTL(0)
{}


NIImporter_SUMO::~NIImporter_SUMO() throw() {
}


void
NIImporter_SUMO::myStartElement(SumoXMLTag element,
                                const SUMOSAXAttributes &attrs) throw(ProcessError) {
    /* our goal is to reproduce the input net faithfully
     * there are different types of objects in the netfile:
     * 1) those which must be loaded into NBNetBuilder-Containers for processing
     * 2) those which can be ignored because they are recomputed based on group 1
     * 3) those which are of no concern to NBNetBuilder but should be exposed to
     *      NETEDIT. We will probably have to patch NBNetBuilder to contain them
     *      and hand them over to NETEDIT 
     *    alternative idea: those shouldn't really be contained within the
     *    network but rather in separate files. teach NETEDIT how to open those
     *    (POI?)
     * 4) those which are of concern neither to NBNetBuilder nor NETEDIT and
     *    must be copied over - need to patch NBNetBuilder for this.
     *    copy unknown by default
     *    (MSCalibrator ?)
     */
    switch (element) {
    case SUMO_TAG_EDGE:
        addEdge(attrs);
        break;
    case SUMO_TAG_LANE:
        addLane(attrs);
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
    case SUMO_TAG_TLLOGIC:
        initTrafficLightLogic(attrs);
        break;
    case SUMO_TAG_PHASE:
        addPhase(attrs);
        break;
    default:
        break;
    }
}


void
NIImporter_SUMO::myCharacters(SumoXMLTag element,
                              const std::string &chars) throw(ProcessError) {
    UNUSED_PARAMETER(element);
    UNUSED_PARAMETER(chars);
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
    case SUMO_TAG_TLLOGIC:
        if (!myCurrentTL) {
            WRITE_ERROR("Unmatched closing tag for tl-logic.");
        } else {
            if (!myTLLCont.insert(myCurrentTL)) {
                WRITE_WARNING("Could not add program '" + myCurrentTL->getProgramID() + 
                        "' for traffic light '" + myCurrentTL->getID() + 
                        "'. Multiple programs for the same traffic light are not supported");
                delete myCurrentTL;
            }
            myCurrentTL = 0;
        }
        break;
    default:
        break;
    }
}


void
NIImporter_SUMO::addEdge(const SUMOSAXAttributes &attrs) {
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes(id)) {
        return;
    }
    bool ok = true;
    myCurrentEdge = new EdgeAttrs;
    myCurrentEdge->id = id;
    // get the type
    myCurrentEdge->type = attrs.getOptStringReporting(SUMO_ATTR_TYPE, id.c_str(), ok, "");
    // get the function
    myCurrentEdge->func = attrs.getOptStringReporting(SUMO_ATTR_FUNCTION, id.c_str(), ok, "normal");
    // get the origin and the destination node
    myCurrentEdge->fromNode = attrs.getOptStringReporting(SUMO_ATTR_FROM, id.c_str(), ok, "");
    myCurrentEdge->toNode = attrs.getOptStringReporting(SUMO_ATTR_TO, id.c_str(), ok, "");
    myCurrentEdge->priority = attrs.getOptIntReporting(SUMO_ATTR_PRIORITY, id.c_str(), ok, -1);
    myCurrentEdge->type = attrs.getOptStringReporting(SUMO_ATTR_TYPE, id.c_str(), ok, "");
    myCurrentEdge->maxSpeed = 0;
    myCurrentEdge->builtEdge = 0;
}


void
NIImporter_SUMO::addLane(const SUMOSAXAttributes &attrs) {
    std::string id;
    if (!attrs.setIDFromAttributes(id)) {
        return;
    }
    if (!myCurrentEdge) {
        WRITE_ERROR("Found lane '" + id  + "' not within edge element");
    }
    myCurrentLane = new LaneAttrs;
    bool ok = true;
    myCurrentLane->maxSpeed = attrs.getOptSUMORealReporting(SUMO_ATTR_MAXSPEED, id.c_str(), ok, -1);
    myCurrentLane->depart = attrs.getOptBoolReporting(SUMO_ATTR_DEPART, id.c_str(), ok, false);
    myCurrentLane->allow = attrs.getOptStringReporting(SUMO_ATTR_ALLOW, id.c_str(), ok, "");
    myCurrentLane->disallow = attrs.getOptStringReporting(SUMO_ATTR_DISALLOW, id.c_str(), ok, "");
    myCurrentLane->shape = GeomConvHelper::parseShapeReporting(
            attrs.getStringReporting(SUMO_ATTR_SHAPE, id.c_str(), ok),
            attrs.getObjectType(), id.c_str(), ok, false);
}


void
NIImporter_SUMO::addJunction(const SUMOSAXAttributes &attrs) {
    // get the id, report an error if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes(id)) {
        return;
    }
    if (id[0]==':') { // internal node
        return;
    }
    bool ok = true;
    SumoXMLNodeType type = NODETYPE_UNKNOWN;
    SUMOReal x = attrs.getSUMORealReporting(SUMO_ATTR_X, id.c_str(), ok);
    SUMOReal y = attrs.getSUMORealReporting(SUMO_ATTR_Y, id.c_str(), ok);
    std::string typeS = attrs.getStringReporting(SUMO_ATTR_TYPE, id.c_str(), ok);
    if (SUMOXMLDefinitions::NodeTypes.hasString(typeS)) {
        type = SUMOXMLDefinitions::NodeTypes.get(typeS);
    } else {
        WRITE_WARNING("Unknown node type '" + typeS + "' for junction '" + id + "'.");
    }

    Position2D pos(x, y);
    if (!GeoConvHelper::x2cartesian(pos)) {
        WRITE_ERROR("Unable to project coordinates for junction '" + id + "'.");
        return;
    }
    NBNode *node = new NBNode(id, pos, type);
    if (!myNodeCont.insert(node)) {
        WRITE_ERROR("Problems on adding junction '" + id + "'.");
        delete node;
        return;
    }
}


void
NIImporter_SUMO::addSuccEdge(const SUMOSAXAttributes &attrs) {
    bool ok = true;
    std::string edge_id = attrs.getStringReporting(SUMO_ATTR_EDGE, 0, ok); // 
    myCurrentEdge = 0;
    if (myEdges.find(edge_id)==myEdges.end()) {
        MsgHandler::getErrorInstance()->inform("Unknown edge '" + edge_id + "' given in succedge.");
        return;
    }
    myCurrentEdge = myEdges.find(edge_id)->second;
    std::string lane_id = attrs.getStringReporting(SUMO_ATTR_LANE, 0, ok); // 
    myCurrentLane = getLaneAttrsFromID(myCurrentEdge, lane_id);
}


void
NIImporter_SUMO::addSuccLane(const SUMOSAXAttributes &attrs) {
    if (myCurrentLane==0) {
        WRITE_ERROR("Found succlane outside succ element");
        return;
    }
    bool ok = true;
    Connection conn;
    conn.lane = attrs.getStringReporting(SUMO_ATTR_LANE, 0, ok);
    conn.tlID = attrs.getOptStringReporting(SUMO_ATTR_TLID, 0, ok, "");
    if (conn.tlID != "") {
        conn.tlLinkNo = attrs.getIntReporting(SUMO_ATTR_TLLINKNO, 0, ok);
    }
    myCurrentLane->connections.push_back(conn);
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


void
NIImporter_SUMO::initTrafficLightLogic(const SUMOSAXAttributes &attrs) {
    if (myCurrentTL) {
        WRITE_ERROR("Definition of tl-logic '" + myCurrentTL->getID() + "' was not finished.");
        return;
    }
    bool ok = true;
    std::string id = attrs.getStringReporting(SUMO_ATTR_ID, 0, ok);
    int offset = attrs.getOptSUMOTimeReporting(SUMO_ATTR_OFFSET, id.c_str(), ok, 0);
    std::string programID = attrs.getOptStringReporting(SUMO_ATTR_PROGRAMID, id.c_str(), ok, "<unknown>");
    //std::string type = attrs.getStringReporting(SUMO_ATTR_TYPE, id.c_str(), ok);
    // this attribute is never used within myJunctionControlBuilder
    //SUMOReal detectorOffset = attrs.getOptSUMORealReporting(SUMO_ATTR_DET_OFFSET, id.c_str(), ok, -1);

    if (ok) {
        myCurrentTL = new NBLoadedSUMOTLDef(id, programID, offset);
    }
    //myJunctionControlBuilder.initTrafficLightLogic(id, programID, type, offset, detectorOffset);
}


void
NIImporter_SUMO::addPhase(const SUMOSAXAttributes &attrs) {
    if (!myCurrentTL) {
        WRITE_ERROR("found phase without tl-logic");
        return;
    }
    const std::string &id = myCurrentTL->getID();
    bool ok = true;
    std::string state = attrs.getStringReporting(SUMO_ATTR_STATE, id.c_str(), ok);
    int duration = attrs.getIntReporting(SUMO_ATTR_DURATION, id.c_str(), ok);
    if (duration < 0) {
        WRITE_ERROR("Phase duration for tl-logic '" + id + "/" + myCurrentTL->getProgramID() + "' must be positive.");
        return;
    }
    // if the traffic light is an actuated traffic light, try to get
    //  the minimum and maximum durations
    //SUMOTime minDuration = attrs.getOptSUMOTimeReporting(SUMO_ATTR_MINDURATION, id.c_str(), ok, -1);
    //SUMOTime maxDuration = attrs.getOptSUMOTimeReporting(SUMO_ATTR_MAXDURATION, id.c_str(), ok, -1);
    if (ok) {
        myCurrentTL->addPhase(duration, state);
    }
}


Position2DVector 
NIImporter_SUMO::approximateEdgeShape(const EdgeAttrs* edge) {
    // reverse logic of NBEdge::computeLaneShape
    const Position2DVector &firstLane = edge->lanes[0]->shape;
    const size_t noLanes = edge->lanes.size();
    Position2DVector result;
    // start- and end- positions are added automatically in NBEdge::init
    for (unsigned int i=1; i < firstLane.size() - 1; i++) {
        Position2D from = firstLane[i-1];
        Position2D me = firstLane[i];
        Position2D to = firstLane[i+1];
        std::pair<SUMOReal, SUMOReal> offsets = NBEdge::laneOffset(
                from, me, SUMO_const_laneWidthAndOffset, (unsigned int)noLanes-1, 
                noLanes, NBEdge::LANESPREAD_RIGHT, false);
        std::pair<SUMOReal, SUMOReal> offsets2 = NBEdge::laneOffset(
                me, to, SUMO_const_laneWidthAndOffset, (unsigned int)noLanes-1, 
                noLanes, NBEdge::LANESPREAD_RIGHT, false);

        Line2D l1(
                Position2D(from.x()+offsets.first, from.y()+offsets.second),
                Position2D(me.x()+offsets.first, me.y()+offsets.second));
        l1.extrapolateBy(100);
        Line2D l2(
                Position2D(me.x()+offsets2.first, me.y()+offsets2.second),
                Position2D(to.x()+offsets2.first, to.y()+offsets2.second));
        l2.extrapolateBy(100);
        if (l1.intersects(l2)) {
            result.push_back(l1.intersectsAt(l2));
        } else {
            MsgHandler::getWarningInstance()->inform("Could not reconstruct shape for edge '" + edge->id + "'.");
        }
    }
    return result;
}
/****************************************************************************/
