/****************************************************************************/
/// @file    NIImporter_SUMO.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 14.04.2008
/// @version $Id$
///
// Importer for networks stored in SUMO format
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
#include <utils/common/UtilExceptions.h>
#include <utils/common/TplConvert.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/ToString.h>
#include <utils/common/TplConvertSec.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/options/OptionsCont.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBNetBuilder.h>
#include "NILoader.h"
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
NIImporter_SUMO::loadNetwork(const OptionsCont& oc, NBNetBuilder& nb) {
    NIImporter_SUMO importer(nb);
    importer._loadNetwork(oc);
}


// ---------------------------------------------------------------------------
// loader methods
// ---------------------------------------------------------------------------
NIImporter_SUMO::NIImporter_SUMO(NBNetBuilder& nb)
    : SUMOSAXHandler("sumo-network"),
      myNetBuilder(nb),
      myNodeCont(nb.getNodeCont()),
      myTLLCont(nb.getTLLogicCont()),
      myCurrentEdge(0),
      myCurrentLane(0),
      myCurrentTL(0),
      myLocation(0),
      mySuspectKeepShape(false),
      myHaveWarnedAboutDeprecatedSpreadType(false),
      myHaveWarnedAboutDeprecatedMaxSpeed(false) {}


NIImporter_SUMO::~NIImporter_SUMO() {
    for (std::map<std::string, EdgeAttrs*>::const_iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        EdgeAttrs* ed = (*i).second;
        for (std::vector<LaneAttrs*>::const_iterator j = ed->lanes.begin(); j != ed->lanes.end(); ++j) {
            delete *j;
        }
        delete ed;
    }
    delete myLocation;
}


void
NIImporter_SUMO::_loadNetwork(const OptionsCont& oc) {
    // check whether the option is set (properly)
    if (!oc.isUsableFileList("sumo-net-file")) {
        return;
    }
    // parse file(s)
    std::vector<std::string> files = oc.getStringVector("sumo-net-file");
    for (std::vector<std::string>::const_iterator file = files.begin(); file != files.end(); ++file) {
        if (!FileHelpers::exists(*file)) {
            WRITE_ERROR("Could not open sumo-net-file '" + *file + "'.");
            return;
        }
        setFileName(*file);
        PROGRESS_BEGIN_MESSAGE("Parsing sumo-net from '" + *file + "'");
        XMLSubSys::runParser(*this, *file);
        PROGRESS_DONE_MESSAGE();
    }
    // build edges
    for (std::map<std::string, EdgeAttrs*>::const_iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        EdgeAttrs* ed = (*i).second;
        // skip internal edges
        if (ed->func == toString(EDGEFUNC_INTERNAL)) {
            continue;
        }
        // get and check the nodes
        NBNode* from = myNodeCont.retrieve(ed->fromNode);
        NBNode* to = myNodeCont.retrieve(ed->toNode);
        if (from == 0) {
            WRITE_ERROR("Edge's '" + ed->id + "' from-node '" + ed->fromNode + "' is not known.");
            continue;
        }
        if (to == 0) {
            WRITE_ERROR("Edge's '" + ed->id + "' to-node '" + ed->toNode + "' is not known.");
            continue;
        }
        // edge shape
        PositionVector geom;
        if (ed->shape.size() > 0) {
            geom = ed->shape;
            mySuspectKeepShape = false; // no problem with reconstruction if edge shape is given explicit
        } else {
            // either the edge has default shape consisting only of the two node
            // positions or we have a legacy network
            geom = reconstructEdgeShape(ed, from->getPosition(), to->getPosition());
        }
        // build and insert the edge
        NBEdge* e = new NBEdge(ed->id, from, to,
                               ed->type, ed->maxSpeed,
                               (unsigned int) ed->lanes.size(),
                               ed->priority, NBEdge::UNSPECIFIED_WIDTH, NBEdge::UNSPECIFIED_OFFSET,
                               geom, ed->streetName, ed->lsf, true); // always use tryIgnoreNodePositions to keep original shape
        e->setLoadedLength(ed->length);
        if (!myNetBuilder.getEdgeCont().insert(e)) {
            WRITE_ERROR("Could not insert edge '" + ed->id + "'.");
            delete e;
            continue;
        }
        ed->builtEdge = myNetBuilder.getEdgeCont().retrieve(ed->id);
    }
    // assign further lane attributes (edges are built)
    for (std::map<std::string, EdgeAttrs*>::const_iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        EdgeAttrs* ed = (*i).second;
        NBEdge* nbe = ed->builtEdge;
        if (nbe == 0) { // inner edge or removed by explicit list, vclass, ...
            continue;
        }
        for (unsigned int fromLaneIndex = 0; fromLaneIndex < (unsigned int) ed->lanes.size(); ++fromLaneIndex) {
            LaneAttrs* lane = ed->lanes[fromLaneIndex];
            // connections
            const std::vector<Connection> &connections = lane->connections;
            for (std::vector<Connection>::const_iterator c_it = connections.begin(); c_it != connections.end(); c_it++) {
                const Connection& c = *c_it;
                if (myEdges.count(c.toEdgeID) == 0) {
                    WRITE_ERROR("Unknown edge '" + c.toEdgeID + "' given in connection.");
                    continue;
                }
                NBEdge* toEdge = myEdges[c.toEdgeID]->builtEdge;
                if (toEdge == 0) { // removed by explicit list, vclass, ...
                    continue;
                }
                nbe->addLane2LaneConnection(
                    fromLaneIndex, toEdge, c.toLaneIdx, NBEdge::L2L_VALIDATED,
                    false, c.mayDefinitelyPass);

                // maybe we have a tls-controlled connection
                if (c.tlID != "") {
                    const std::map<std::string, NBTrafficLightDefinition*>& programs = myTLLCont.getPrograms(c.tlID);
                    if (programs.size() > 0) {
                        std::map<std::string, NBTrafficLightDefinition*>::const_iterator it;
                        for (it = programs.begin(); it != programs.end(); it++) {
                            NBLoadedSUMOTLDef* tlDef = dynamic_cast<NBLoadedSUMOTLDef*>(it->second);
                            if (tlDef) {
                                tlDef->addConnection(nbe, toEdge, fromLaneIndex, c.toLaneIdx, c.tlLinkNo);
                            } else {
                                throw ProcessError("Corrupt traffic light definition '"
                                                   + c.tlID + "' (program '" + it->first + "')");
                            }
                        }
                    } else {
                        WRITE_ERROR("The traffic light '" + c.tlID + "' is not known.");
                    }
                }
            }
            // allow/disallow
            SUMOVehicleClasses allowed;
            SUMOVehicleClasses disallowed;
            parseVehicleClasses(lane->allow, lane->disallow, allowed, disallowed);
            nbe->setVehicleClasses(allowed, disallowed, fromLaneIndex);
            // width, offset
            nbe->setWidth(fromLaneIndex, lane->width);
            nbe->setOffset(fromLaneIndex, lane->offset);
            nbe->setSpeed(fromLaneIndex, lane->maxSpeed);
        }
        nbe->declareConnectionsAsLoaded();
    }
    // insert loaded prohibitions
    for (std::vector<Prohibition>::const_iterator it = myProhibitions.begin(); it != myProhibitions.end(); it++) {
        NBEdge* prohibitedFrom = myEdges[it->prohibitedFrom]->builtEdge;
        if (prohibitedFrom == 0) {
            WRITE_ERROR("Edge '" + it->prohibitedFrom + "' in prohibition was not built");
        } else {
            NBNode* n = prohibitedFrom->getToNode();
            n->addSortedLinkFoes(
                NBConnection(myEdges[it->prohibitorFrom]->builtEdge, myEdges[it->prohibitorTo]->builtEdge),
                NBConnection(prohibitedFrom, myEdges[it->prohibitedTo]->builtEdge));
        }
    }

    // final warning
    if (mySuspectKeepShape) {
        WRITE_WARNING("The input network may have been built using option 'xml.keep-shape'.\n... Accuracy of junction positions cannot be guaranteed.");
    }

}



void
NIImporter_SUMO::myStartElement(int element,
                                const SUMOSAXAttributes& attrs) {
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
        case SUMO_TAG_CONNECTION:
            addConnection(attrs);
            break;
        case SUMO_TAG_TLLOGIC__DEPRECATED:
        case SUMO_TAG_TLLOGIC:
            myCurrentTL = initTrafficLightLogic(attrs, myCurrentTL);
            break;
        case SUMO_TAG_PHASE:
            addPhase(attrs, myCurrentTL);
            break;
        case SUMO_TAG_LOCATION:
            myLocation = loadLocation(attrs);
            break;
        case SUMO_TAG_PROHIBITION:
            addProhibition(attrs);
            break;
        default:
            break;
    }
}


void
NIImporter_SUMO::myCharacters(int element,
                              const std::string& chars) {
    UNUSED_PARAMETER(element);
    UNUSED_PARAMETER(chars);
}


void
NIImporter_SUMO::myEndElement(int element) {
    switch (element) {
        case SUMO_TAG_EDGE:
            if (myEdges.find(myCurrentEdge->id) != myEdges.end()) {
                WRITE_ERROR("Edge '" + myCurrentEdge->id + "' occured at least twice in the input.");
            } else {
                myEdges[myCurrentEdge->id] = myCurrentEdge;
            }
            myCurrentEdge = 0;
            break;
        case SUMO_TAG_LANE:
            if (myCurrentEdge != 0) {
                myCurrentEdge->maxSpeed = MAX2(myCurrentEdge->maxSpeed, myCurrentLane->maxSpeed);
                myCurrentEdge->lanes.push_back(myCurrentLane);
            }
            myCurrentLane = 0;
            break;
        case SUMO_TAG_TLLOGIC__DEPRECATED:
        case SUMO_TAG_TLLOGIC:
            if (!myCurrentTL) {
                WRITE_ERROR("Unmatched closing tag for tl-logic.");
            } else {
                if (!myTLLCont.insert(myCurrentTL)) {
                    WRITE_WARNING("Could not add program '" + myCurrentTL->getProgramID() +
                                  "' for traffic light '" + myCurrentTL->getID() + "'");
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
NIImporter_SUMO::addEdge(const SUMOSAXAttributes& attrs) {
    // get the id, report an error if not given or empty...
    bool ok = true;
    std::string id = attrs.getStringReporting(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        return;
    }
    myCurrentEdge = new EdgeAttrs();
    myCurrentEdge->builtEdge = 0;
    myCurrentEdge->id = id;
    // get the function
    myCurrentEdge->func = attrs.getOptStringReporting(SUMO_ATTR_FUNCTION, id.c_str(), ok, "normal");
    if (myCurrentEdge->func == toString(EDGEFUNC_INTERNAL)) {
        return; // skip internal edges
    }
    // get the type
    myCurrentEdge->type = attrs.getOptStringReporting(SUMO_ATTR_TYPE, id.c_str(), ok, "");
    // get the origin and the destination node
    myCurrentEdge->fromNode = attrs.getOptStringReporting(SUMO_ATTR_FROM, id.c_str(), ok, "");
    myCurrentEdge->toNode = attrs.getOptStringReporting(SUMO_ATTR_TO, id.c_str(), ok, "");
    myCurrentEdge->priority = attrs.getOptIntReporting(SUMO_ATTR_PRIORITY, id.c_str(), ok, -1);
    myCurrentEdge->type = attrs.getOptStringReporting(SUMO_ATTR_TYPE, id.c_str(), ok, "");
    myCurrentEdge->shape = GeomConvHelper::parseShapeReporting(
                               attrs.getOptStringReporting(SUMO_ATTR_SHAPE, id.c_str(), ok, ""),
                               attrs.getObjectType(), id.c_str(), ok, true);
    NILoader::transformCoordinates(myCurrentEdge->shape, true, myLocation);
    myCurrentEdge->length = attrs.getOptSUMORealReporting(SUMO_ATTR_LENGTH, id.c_str(), ok, NBEdge::UNSPECIFIED_LOADED_LENGTH);
    myCurrentEdge->maxSpeed = 0;
    myCurrentEdge->streetName = attrs.getOptStringReporting(SUMO_ATTR_NAME, id.c_str(), ok, "");

    std::string lsfS = toString(LANESPREAD_RIGHT);
    if (attrs.hasAttribute(SUMO_ATTR_SPREADFUNC__DEPRECATED)) {
        lsfS = attrs.getStringReporting(SUMO_ATTR_SPREADFUNC__DEPRECATED, id.c_str(), ok);
        if (!myHaveWarnedAboutDeprecatedSpreadType) {
            WRITE_WARNING("'" + toString(SUMO_ATTR_SPREADFUNC__DEPRECATED) + "' is deprecated; please use '" + toString(SUMO_ATTR_SPREADTYPE) + "'.");
            myHaveWarnedAboutDeprecatedSpreadType = true;
        }
    } else {
        lsfS = attrs.getOptStringReporting(SUMO_ATTR_SPREADTYPE, id.c_str(), ok, lsfS);
    }
    if (SUMOXMLDefinitions::LaneSpreadFunctions.hasString(lsfS)) {
        myCurrentEdge->lsf = SUMOXMLDefinitions::LaneSpreadFunctions.get(lsfS);
    } else {
        WRITE_ERROR("Unknown spreadType '" + lsfS + "' for edge '" + id + "'.");
    }
}


void
NIImporter_SUMO::addLane(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string id = attrs.getStringReporting(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        return;
    }
    if (!myCurrentEdge) {
        WRITE_ERROR("Found lane '" + id  + "' not within edge element");
        return;
    }
    myCurrentLane = new LaneAttrs;
    if (myCurrentEdge->func == toString(EDGEFUNC_INTERNAL)) {
        return; // skip internal lanes
    }
    if (attrs.hasAttribute(SUMO_ATTR_MAXSPEED__DEPRECATED)) {
        myCurrentLane->maxSpeed = attrs.getSUMORealReporting(SUMO_ATTR_MAXSPEED__DEPRECATED, id.c_str(), ok);
        if (!myHaveWarnedAboutDeprecatedMaxSpeed) {
            myHaveWarnedAboutDeprecatedMaxSpeed = true;
            WRITE_WARNING("'" + toString(SUMO_ATTR_MAXSPEED__DEPRECATED) + "' is deprecated, please use '" + toString(SUMO_ATTR_SPEED) + "' instead.");
        }
    } else {
        myCurrentLane->maxSpeed = attrs.getSUMORealReporting(SUMO_ATTR_SPEED, id.c_str(), ok);
    }
    myCurrentLane->allow = attrs.getOptStringReporting(SUMO_ATTR_ALLOW, id.c_str(), ok, "");
    myCurrentLane->disallow = attrs.getOptStringReporting(SUMO_ATTR_DISALLOW, id.c_str(), ok, "");
    myCurrentLane->width = attrs.getOptSUMORealReporting(SUMO_ATTR_WIDTH, id.c_str(), ok, (SUMOReal) NBEdge::UNSPECIFIED_WIDTH);
    myCurrentLane->offset = attrs.getOptSUMORealReporting(SUMO_ATTR_ENDOFFSET, id.c_str(), ok, (SUMOReal) NBEdge::UNSPECIFIED_OFFSET);
    myCurrentLane->shape = GeomConvHelper::parseShapeReporting(
                               attrs.getStringReporting(SUMO_ATTR_SHAPE, id.c_str(), ok),
                               attrs.getObjectType(), id.c_str(), ok, false);
    // lane coordinates are derived (via lane spread) do not include them in convex boundary
    NILoader::transformCoordinates(myCurrentLane->shape, false, myLocation);
}


void
NIImporter_SUMO::addJunction(const SUMOSAXAttributes& attrs) {
    // get the id, report an error if not given or empty...
    bool ok = true;
    std::string id = attrs.getStringReporting(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        return;
    }
    if (id[0] == ':') { // internal node
        return;
    }
    SumoXMLNodeType type = NODETYPE_UNKNOWN;
    std::string typeS = attrs.getStringReporting(SUMO_ATTR_TYPE, id.c_str(), ok);
    if (SUMOXMLDefinitions::NodeTypes.hasString(typeS)) {
        type = SUMOXMLDefinitions::NodeTypes.get(typeS);
        if (type == NODETYPE_DEAD_END_DEPRECATED) { // patch legacy type
            type = NODETYPE_DEAD_END;
        }
    } else {
        WRITE_WARNING("Unknown node type '" + typeS + "' for junction '" + id + "'.");
    }
    Position pos = readPosition(attrs, id, ok);
    NILoader::transformCoordinates(pos, true, myLocation);
    // the network may have been built with the option "plain.keep-edge-shape" this
    // makes accurate reconstruction of legacy networks impossible. We ought to warn about this
    std::string shapeS = attrs.getStringReporting(SUMO_ATTR_SHAPE, id.c_str(), ok, false);
    if (shapeS != "") {
        PositionVector shape = GeomConvHelper::parseShapeReporting(
                                   shapeS, attrs.getObjectType(), id.c_str(), ok, false);
        shape.push_back_noDoublePos(shape[0]); // need closed shape
        if (!shape.around(pos) && shape.distance(pos) > 1) { // MAGIC_THRESHOLD
            // WRITE_WARNING("Junction '" + id + "': distance between pos and shape is " + toString(shape.distance(pos)));
            mySuspectKeepShape = true;
        }
    }
    NBNode* node = new NBNode(id, pos, type);
    if (!myNodeCont.insert(node)) {
        WRITE_ERROR("Problems on adding junction '" + id + "'.");
        delete node;
        return;
    }
}


void
NIImporter_SUMO::addSuccEdge(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string edge_id = attrs.getStringReporting(SUMO_ATTR_EDGE, 0, ok);
    myCurrentEdge = 0;
    if (myEdges.count(edge_id) == 0) {
        WRITE_ERROR("Unknown edge '" + edge_id + "' given in succedge.");
        return;
    }
    myCurrentEdge = myEdges[edge_id];
    std::string lane_id = attrs.getStringReporting(SUMO_ATTR_LANE, 0, ok);
    myCurrentLane = getLaneAttrsFromID(myCurrentEdge, lane_id);
}


void
NIImporter_SUMO::addSuccLane(const SUMOSAXAttributes& attrs) {
    if (myCurrentLane == 0) {
        WRITE_ERROR("Found succlane outside succ element");
        return;
    }
    bool ok = true;
    Connection conn;
    std::string laneID = attrs.getStringReporting(SUMO_ATTR_LANE, 0, ok);
    if (laneID == "SUMO_NO_DESTINATION") { // legacy check
        // deprecated
        return;
    }
    interpretLaneID(laneID, conn.toEdgeID, conn.toLaneIdx);
    conn.tlID = attrs.getOptStringReporting(SUMO_ATTR_TLID, 0, ok, "");
    conn.mayDefinitelyPass = false; // (attrs.getStringReporting(SUMO_ATTR_STATE, 0, ok, "") == "M");
    if (conn.tlID != "") {
        conn.tlLinkNo = attrs.hasAttribute(SUMO_ATTR_TLLINKINDEX)
                        ? attrs.getIntReporting(SUMO_ATTR_TLLINKINDEX, 0, ok)
                        : attrs.getIntReporting(SUMO_ATTR_TLLINKNO__DEPRECATED, 0, ok);
    }
    myCurrentLane->connections.push_back(conn);
}


void
NIImporter_SUMO::addConnection(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string fromID = attrs.getStringReporting(SUMO_ATTR_FROM, 0, ok);
    if (myEdges.count(fromID) == 0) {
        WRITE_ERROR("Unknown edge '" + fromID + "' given in connection.");
        return;
    }
    EdgeAttrs* from = myEdges[fromID];
    Connection conn;
    conn.toEdgeID = attrs.getStringReporting(SUMO_ATTR_TO, 0, ok);
    unsigned int fromLaneIdx = attrs.getIntReporting(SUMO_ATTR_FROM_LANE, 0, ok);
    conn.toLaneIdx = attrs.getIntReporting(SUMO_ATTR_TO_LANE, 0, ok);
    conn.tlID = attrs.getOptStringReporting(SUMO_ATTR_TLID, 0, ok, "");
    conn.mayDefinitelyPass = false; // (attrs.getStringReporting(SUMO_ATTR_STATE, 0, ok, "") == "M");
    if (conn.tlID != "") {
        conn.tlLinkNo = attrs.getIntReporting(SUMO_ATTR_TLLINKINDEX, 0, ok);
    }

    if (from->lanes.size() <= (size_t) fromLaneIdx) {
        WRITE_ERROR("Invalid lane index '" + toString(fromLaneIdx) + "' for connection from '" + fromID + "'.");
        return;
    }
    from->lanes[fromLaneIdx]->connections.push_back(conn);
}


void
NIImporter_SUMO::addProhibition(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string prohibitor = attrs.getOptStringReporting(SUMO_ATTR_PROHIBITOR, 0, ok, "");
    std::string prohibited = attrs.getOptStringReporting(SUMO_ATTR_PROHIBITED, 0, ok, "");
    if (!ok) {
        return;
    }
    Prohibition p;
    parseProhibitionConnection(prohibitor, p.prohibitorFrom, p.prohibitorTo, ok);
    parseProhibitionConnection(prohibited, p.prohibitedFrom, p.prohibitedTo, ok);
    if (!ok) {
        return;
    }
    myProhibitions.push_back(p);
}


NIImporter_SUMO::LaneAttrs*
NIImporter_SUMO::getLaneAttrsFromID(EdgeAttrs* edge, std::string lane_id) {
    std::string edge_id;
    unsigned int index;
    interpretLaneID(lane_id, edge_id, index);
    assert(edge->id == edge_id);
    if (edge->lanes.size() <= (size_t) index) {
        WRITE_ERROR("Unknown lane '" + lane_id + "' given in succedge.");
        return 0;
    } else {
        return edge->lanes[index];
    }
}


void
NIImporter_SUMO::interpretLaneID(const std::string& lane_id, std::string& edge_id, unsigned int& index) {
    // assume lane_id = edge_id + '_' + index
    size_t sep_index = lane_id.rfind('_');
    if (sep_index == std::string::npos) {
        WRITE_ERROR("Invalid lane id '" + lane_id + "' (missing '_').");
    }
    edge_id = lane_id.substr(0, sep_index);
    std::string index_string = lane_id.substr(sep_index + 1);
    try {
        index = (unsigned int)TplConvert<char>::_2int(index_string.c_str());
    } catch (NumberFormatException) {
        WRITE_ERROR("Invalid lane index '" + index_string + "' for lane '" + lane_id + "'.");
    }
}


NBLoadedSUMOTLDef*
NIImporter_SUMO::initTrafficLightLogic(const SUMOSAXAttributes& attrs, NBLoadedSUMOTLDef* currentTL) {
    if (currentTL) {
        WRITE_ERROR("Definition of tl-logic '" + currentTL->getID() + "' was not finished.");
        return 0;
    }
    bool ok = true;
    std::string id = attrs.getStringReporting(SUMO_ATTR_ID, 0, ok);
    SUMOTime offset = TIME2STEPS(attrs.getSUMORealReporting(SUMO_ATTR_OFFSET, id.c_str(), ok));
    std::string programID = attrs.getOptStringReporting(SUMO_ATTR_PROGRAMID, id.c_str(), ok, "<unknown>");
    std::string type = attrs.getStringReporting(SUMO_ATTR_TYPE, 0, ok);
    if (type != toString(TLTYPE_STATIC)) {
        WRITE_WARNING("Traffic light '" + id + "' has unsupported type '" + type + "' and will be converted to '" +
                      toString(TLTYPE_STATIC) + "'");
    }
    if (ok) {
        return new NBLoadedSUMOTLDef(id, programID, offset);
    } else {
        return 0;
    }
}


void
NIImporter_SUMO::addPhase(const SUMOSAXAttributes& attrs, NBLoadedSUMOTLDef* currentTL) {
    if (!currentTL) {
        WRITE_ERROR("found phase without tl-logic");
        return;
    }
    const std::string& id = currentTL->getID();
    bool ok = true;
    std::string state = attrs.getStringReporting(SUMO_ATTR_STATE, id.c_str(), ok);
    SUMOTime duration = TIME2STEPS(attrs.getSUMORealReporting(SUMO_ATTR_DURATION, id.c_str(), ok));
    if (duration < 0) {
        WRITE_ERROR("Phase duration for tl-logic '" + id + "/" + currentTL->getProgramID() + "' must be positive.");
        return;
    }
    // if the traffic light is an actuated traffic light, try to get
    //  the minimum and maximum durations
    //SUMOTime minDuration = attrs.getOptSUMOTimeReporting(SUMO_ATTR_MINDURATION, id.c_str(), ok, -1);
    //SUMOTime maxDuration = attrs.getOptSUMOTimeReporting(SUMO_ATTR_MAXDURATION, id.c_str(), ok, -1);
    if (ok) {
        currentTL->addPhase(duration, state);
    }
}


PositionVector
NIImporter_SUMO::reconstructEdgeShape(const EdgeAttrs* edge, const Position& from, const Position& to) {
    const PositionVector& firstLane = edge->lanes[0]->shape;
    PositionVector result;
    result.push_back(from);

    // reverse logic of NBEdge::computeLaneShape
    // !!! this will only work for old-style constant width lanes
    const size_t noLanes = edge->lanes.size();
    for (unsigned int i = 1; i < firstLane.size() - 1; i++) {
        Position from = firstLane[i - 1];
        Position me = firstLane[i];
        Position to = firstLane[i + 1];
        std::pair<SUMOReal, SUMOReal> offsets = NBEdge::laneOffset(
                from, me, SUMO_const_laneWidthAndOffset, (unsigned int)noLanes - 1,
                noLanes, edge->lsf, false);
        std::pair<SUMOReal, SUMOReal> offsets2 = NBEdge::laneOffset(
                    me, to, SUMO_const_laneWidthAndOffset, (unsigned int)noLanes - 1,
                    noLanes, edge->lsf, false);

        Line l1(
            Position(from.x() + offsets.first, from.y() + offsets.second),
            Position(me.x() + offsets.first, me.y() + offsets.second));
        l1.extrapolateBy(100);
        Line l2(
            Position(me.x() + offsets2.first, me.y() + offsets2.second),
            Position(to.x() + offsets2.first, to.y() + offsets2.second));
        l2.extrapolateBy(100);
        if (l1.intersects(l2)) {
            result.push_back(l1.intersectsAt(l2));
        } else {
            WRITE_WARNING("Could not reconstruct shape for edge '" + edge->id + "'.");
        }
    }

    result.push_back(to);
    return result;
}


GeoConvHelper*
NIImporter_SUMO::loadLocation(const SUMOSAXAttributes& attrs) {
    // @todo refactor parsing of location since its duplicated in NLHandler and PCNetProjectionLoader
    bool ok = true;
    GeoConvHelper* result = 0;
    PositionVector s = GeomConvHelper::parseShapeReporting(
                           attrs.getStringReporting(SUMO_ATTR_NET_OFFSET, 0, ok),
                           attrs.getObjectType(), 0, ok, false);
    Boundary convBoundary = GeomConvHelper::parseBoundaryReporting(
                                attrs.getStringReporting(SUMO_ATTR_CONV_BOUNDARY, 0, ok),
                                attrs.getObjectType(), 0, ok);
    Boundary origBoundary = GeomConvHelper::parseBoundaryReporting(
                                attrs.getStringReporting(SUMO_ATTR_ORIG_BOUNDARY, 0, ok),
                                attrs.getObjectType(), 0, ok);
    std::string proj = attrs.getStringReporting(SUMO_ATTR_ORIG_PROJ, 0, ok);
    if (ok) {
        Position networkOffset = s[0];
        result = new GeoConvHelper(proj, networkOffset, origBoundary, convBoundary);
        GeoConvHelper::setLoaded(*result);
    }
    return result;
}


Position
NIImporter_SUMO::readPosition(const SUMOSAXAttributes& attrs, const std::string& id, bool& ok) {
    SUMOReal x = attrs.getSUMORealReporting(SUMO_ATTR_X, id.c_str(), ok);
    SUMOReal y = attrs.getSUMORealReporting(SUMO_ATTR_Y, id.c_str(), ok);
    SUMOReal z = 0;
    if (attrs.hasAttribute(SUMO_ATTR_Z)) {
        z = attrs.getSUMORealReporting(SUMO_ATTR_Z, id.c_str(), ok);
    }
    return Position(x, y, z);
}


void
NIImporter_SUMO::parseProhibitionConnection(const std::string& attr, std::string& from, std::string& to, bool& ok) {
    // split from/to
    size_t div = attr.find("->");
    if (div == std::string::npos) {
        WRITE_ERROR("Missing connection divider in prohibition attribute '" + attr + "'");
        ok = false;
    }
    from = attr.substr(0, div);
    to = attr.substr(div + 2);
    // check whether the definition includes a lane information and discard it
    if (from.find('_') != std::string::npos) {
        from = from.substr(0, from.find('_'));
    }
    if (to.find('_') != std::string::npos) {
        to = to.substr(0, to.find('_'));
    }
    // check whether the edges are known
    if (myEdges.count(from) == 0) {
        WRITE_ERROR("Unknown edge prohibition '" + from + "'");
        ok = false;
    }
    if (myEdges.count(to) == 0) {
        WRITE_ERROR("Unknown edge prohibition '" + to + "'");
        ok = false;
    }
}
/****************************************************************************/
