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
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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
#include <utils/common/TplConvert.h>
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
#include <netbuild/NBAlgorithms_Ramps.h>
#include <netbuild/NBNetBuilder.h>
#include "NILoader.h"
#include "NIXMLEdgesHandler.h"
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
NIImporter_SUMO::loadNetwork(OptionsCont& oc, NBNetBuilder& nb) {
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
      myHaveSeenInternalEdge(false),
      myAmLefthand(false),
      myCornerDetail(0),
      myLinkDetail(-1) {
}


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
NIImporter_SUMO::_loadNetwork(OptionsCont& oc) {
    // check whether the option is set (properly)
    if (!oc.isUsableFileList("sumo-net-file")) {
        return;
    }
    // parse file(s)
    std::vector<std::string> files = oc.getStringVector("sumo-net-file");
    for (std::vector<std::string>::const_iterator file = files.begin(); file != files.end(); ++file) {
        if (!FileHelpers::isReadable(*file)) {
            WRITE_ERROR("Could not open sumo-net-file '" + *file + "'.");
            return;
        }
        setFileName(*file);
        PROGRESS_BEGIN_MESSAGE("Parsing sumo-net from '" + *file + "'");
        XMLSubSys::runParser(*this, *file, true);
        PROGRESS_DONE_MESSAGE();
    }
    // build edges
    for (std::map<std::string, EdgeAttrs*>::const_iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        EdgeAttrs* ed = (*i).second;
        // skip internal edges
        if (ed->func == EDGEFUNC_INTERNAL || ed->func == EDGEFUNC_CROSSING || ed->func == EDGEFUNC_WALKINGAREA) {
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
        if (from == to) {
            WRITE_ERROR("Edge's '" + ed->id + "' from-node and to-node '" + ed->toNode + "' art identical.");
            continue;
        }
        // edge shape
        PositionVector geom;
        if (ed->shape.size() > 0) {
            geom = ed->shape;
        } else {
            // either the edge has default shape consisting only of the two node
            // positions or we have a legacy network
            geom = reconstructEdgeShape(ed, from->getPosition(), to->getPosition());
        }
        // build and insert the edge
        NBEdge* e = new NBEdge(ed->id, from, to,
                               ed->type, ed->maxSpeed,
                               (int) ed->lanes.size(),
                               ed->priority, NBEdge::UNSPECIFIED_WIDTH, NBEdge::UNSPECIFIED_OFFSET,
                               geom, ed->streetName, "", ed->lsf, true); // always use tryIgnoreNodePositions to keep original shape
        e->setLoadedLength(ed->length);
        if (!myNetBuilder.getEdgeCont().insert(e)) {
            WRITE_ERROR("Could not insert edge '" + ed->id + "'.");
            delete e;
            continue;
        }
        ed->builtEdge = myNetBuilder.getEdgeCont().retrieve(ed->id);
    }
    // assign further lane attributes (edges are built)
    EdgeVector toRemove;
    for (std::map<std::string, EdgeAttrs*>::const_iterator i = myEdges.begin(); i != myEdges.end(); ++i) {
        EdgeAttrs* ed = (*i).second;
        NBEdge* nbe = ed->builtEdge;
        if (nbe == 0) { // inner edge or removed by explicit list, vclass, ...
            continue;
        }
        for (int fromLaneIndex = 0; fromLaneIndex < (int) ed->lanes.size(); ++fromLaneIndex) {
            LaneAttrs* lane = ed->lanes[fromLaneIndex];
            // connections
            const std::vector<Connection>& connections = lane->connections;
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
                if (nbe->hasConnectionTo(toEdge, c.toLaneIdx)) {
                    WRITE_WARNING("Target lane '" + toEdge->getLaneID(c.toLaneIdx) + "' has multiple connections from '" + nbe->getID() + "'.");
                }
                nbe->addLane2LaneConnection(
                    fromLaneIndex, toEdge, c.toLaneIdx, NBEdge::L2L_VALIDATED,
                    true, c.mayDefinitelyPass, c.keepClear, c.contPos, c.visibility);

                // maybe we have a tls-controlled connection
                if (c.tlID != "" && myRailSignals.count(c.tlID) == 0) {
                    const std::map<std::string, NBTrafficLightDefinition*>& programs = myTLLCont.getPrograms(c.tlID);
                    if (programs.size() > 0) {
                        std::map<std::string, NBTrafficLightDefinition*>::const_iterator it;
                        for (it = programs.begin(); it != programs.end(); it++) {
                            NBLoadedSUMOTLDef* tlDef = dynamic_cast<NBLoadedSUMOTLDef*>(it->second);
                            if (tlDef) {
                                tlDef->addConnection(nbe, toEdge, fromLaneIndex, c.toLaneIdx, c.tlLinkNo);
                            } else {
                                throw ProcessError("Corrupt traffic light definition '" + c.tlID + "' (program '" + it->first + "')");
                            }
                        }
                    } else {
                        WRITE_ERROR("The traffic light '" + c.tlID + "' is not known.");
                    }
                }
            }
            // allow/disallow XXX preferred
            nbe->setPermissions(parseVehicleClasses(lane->allow, lane->disallow), fromLaneIndex);
            // width, offset
            nbe->setLaneWidth(fromLaneIndex, lane->width);
            nbe->setEndOffset(fromLaneIndex, lane->endOffset);
            nbe->setSpeed(fromLaneIndex, lane->maxSpeed);
            nbe->getLaneStruct(fromLaneIndex).oppositeID = lane->oppositeID;
        }
        nbe->declareConnectionsAsLoaded();
        if (!nbe->hasLaneSpecificWidth() && nbe->getLanes()[0].width != NBEdge::UNSPECIFIED_WIDTH) {
            nbe->setLaneWidth(-1, nbe->getLaneWidth(0));
        }
        if (!nbe->hasLaneSpecificEndOffset() && nbe->getEndOffset(0) != NBEdge::UNSPECIFIED_OFFSET) {
            nbe->setEndOffset(-1, nbe->getEndOffset(0));
        }
        // check again after permissions are set
        if (myNetBuilder.getEdgeCont().ignoreFilterMatch(nbe)) {
            myNetBuilder.getEdgeCont().ignore(nbe->getID());
            toRemove.push_back(nbe);
        }
    }
    for (EdgeVector::iterator i = toRemove.begin(); i != toRemove.end(); ++i) {
        myNetBuilder.getEdgeCont().erase(myNetBuilder.getDistrictCont(), *i);
    }
    // insert loaded prohibitions
    for (std::vector<Prohibition>::const_iterator it = myProhibitions.begin(); it != myProhibitions.end(); it++) {
        NBEdge* prohibitedFrom = myEdges[it->prohibitedFrom]->builtEdge;
        NBEdge* prohibitedTo = myEdges[it->prohibitedTo]->builtEdge;
        NBEdge* prohibitorFrom = myEdges[it->prohibitorFrom]->builtEdge;
        NBEdge* prohibitorTo = myEdges[it->prohibitorTo]->builtEdge;
        if (prohibitedFrom == 0) {
            WRITE_WARNING("Edge '" + it->prohibitedFrom + "' in prohibition was not built");
        } else if (prohibitedTo == 0) {
            WRITE_WARNING("Edge '" + it->prohibitedTo + "' in prohibition was not built");
        } else if (prohibitorFrom == 0) {
            WRITE_WARNING("Edge '" + it->prohibitorFrom + "' in prohibition was not built");
        } else if (prohibitorTo == 0) {
            WRITE_WARNING("Edge '" + it->prohibitorTo + "' in prohibition was not built");
        } else {
            NBNode* n = prohibitedFrom->getToNode();
            n->addSortedLinkFoes(
                NBConnection(prohibitorFrom, prohibitorTo),
                NBConnection(prohibitedFrom, prohibitedTo));
        }
    }
    if (!myHaveSeenInternalEdge) {
        myNetBuilder.haveLoadedNetworkWithoutInternalEdges();
    }
    if (oc.isDefault("lefthand")) {
        oc.set("lefthand", toString(myAmLefthand));
    }
    if (oc.isDefault("junctions.corner-detail")) {
        oc.set("junctions.corner-detail", toString(myCornerDetail));
    }
    if (oc.isDefault("junctions.internal-link-detail") && myLinkDetail > 0) {
        oc.set("junctions.internal-link-detail", toString(myLinkDetail));
    }
    if (!deprecatedVehicleClassesSeen.empty()) {
        WRITE_WARNING("Deprecated vehicle class(es) '" + toString(deprecatedVehicleClassesSeen) + "' in input network.");
        deprecatedVehicleClassesSeen.clear();
    }
    // add loaded crossings
    if (!oc.getBool("no-internal-links")) {
        for (std::map<std::string, std::vector<Crossing> >::const_iterator it = myPedestrianCrossings.begin(); it != myPedestrianCrossings.end(); ++it) {
            NBNode* node = myNodeCont.retrieve((*it).first);
            for (std::vector<Crossing>::const_iterator it_c = (*it).second.begin(); it_c != (*it).second.end(); ++it_c) {
                const Crossing& crossing = (*it_c);
                EdgeVector edges;
                for (std::vector<std::string>::const_iterator it_e = crossing.crossingEdges.begin(); it_e != crossing.crossingEdges.end(); ++it_e) {
                    NBEdge* edge = myNetBuilder.getEdgeCont().retrieve(*it_e);
                    // edge might have been removed due to options
                    if (edge != 0) {
                        edges.push_back(edge);
                    }
                }
                if (edges.size() > 0) {
                    node->addCrossing(edges, crossing.width, crossing.priority, true);
                }
            }
        }
    }
    // add roundabouts
    for (std::vector<std::vector<std::string> >::const_iterator it = myRoundabouts.begin(); it != myRoundabouts.end(); ++it) {
        EdgeSet roundabout;
        for (std::vector<std::string>::const_iterator it_r = it->begin(); it_r != it->end(); ++it_r) {
            NBEdge* edge = myNetBuilder.getEdgeCont().retrieve(*it_r);
            if (edge == 0) {
                if (!myNetBuilder.getEdgeCont().wasIgnored(*it_r)) {
                    WRITE_ERROR("Unknown edge '" + (*it_r) + "' in roundabout");
                }
            } else {
                roundabout.insert(edge);
            }
        }
        myNetBuilder.getEdgeCont().addRoundabout(roundabout);
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
        case SUMO_TAG_NET: {
            bool ok;
            myAmLefthand = attrs.getOpt<bool>(SUMO_ATTR_LEFTHAND, 0, ok, false);
            myCornerDetail = attrs.getOpt<int>(SUMO_ATTR_CORNERDETAIL, 0, ok, 0);
            myLinkDetail = attrs.getOpt<int>(SUMO_ATTR_LINKDETAIL, 0, ok, -1);
            break;
        }
        case SUMO_TAG_EDGE:
            addEdge(attrs);
            break;
        case SUMO_TAG_LANE:
            addLane(attrs);
            break;
        case SUMO_TAG_NEIGH:
            myCurrentLane->oppositeID = attrs.getString(SUMO_ATTR_LANE);
            break;
        case SUMO_TAG_JUNCTION:
            addJunction(attrs);
            break;
        case SUMO_TAG_REQUEST:
            addRequest(attrs);
            break;
        case SUMO_TAG_CONNECTION:
            addConnection(attrs);
            break;
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
        case SUMO_TAG_ROUNDABOUT:
            addRoundabout(attrs);
            break;
        default:
            break;
    }
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
        case SUMO_TAG_TLLOGIC:
            if (!myCurrentTL) {
                WRITE_ERROR("Unmatched closing tag for tl-logic.");
            } else {
                if (!myTLLCont.insert(myCurrentTL)) {
                    WRITE_WARNING("Could not add program '" + myCurrentTL->getProgramID() + "' for traffic light '" + myCurrentTL->getID() + "'");
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
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        return;
    }
    myCurrentEdge = new EdgeAttrs();
    myCurrentEdge->builtEdge = 0;
    myCurrentEdge->id = id;
    // get the function
    myCurrentEdge->func = attrs.getEdgeFunc(ok);
    if (myCurrentEdge->func == EDGEFUNC_CROSSING) {
        // add the crossing but don't do anything else
        Crossing c;
        c.edgeID = id;
        SUMOSAXAttributes::parseStringVector(attrs.get<std::string>(SUMO_ATTR_CROSSING_EDGES, 0, ok), c.crossingEdges);
        myPedestrianCrossings[SUMOXMLDefinitions::getJunctionIDFromInternalEdge(id)].push_back(c);
        return;
    } else if (myCurrentEdge->func == EDGEFUNC_INTERNAL || myCurrentEdge->func == EDGEFUNC_WALKINGAREA) {
        return; // skip internal edges
    }
    // get the type
    myCurrentEdge->type = attrs.getOpt<std::string>(SUMO_ATTR_TYPE, id.c_str(), ok, "");
    // get the origin and the destination node
    myCurrentEdge->fromNode = attrs.getOpt<std::string>(SUMO_ATTR_FROM, id.c_str(), ok, "");
    myCurrentEdge->toNode = attrs.getOpt<std::string>(SUMO_ATTR_TO, id.c_str(), ok, "");
    myCurrentEdge->priority = attrs.getOpt<int>(SUMO_ATTR_PRIORITY, id.c_str(), ok, -1);
    myCurrentEdge->type = attrs.getOpt<std::string>(SUMO_ATTR_TYPE, id.c_str(), ok, "");
    myCurrentEdge->shape = attrs.getOpt<PositionVector>(SUMO_ATTR_SHAPE, id.c_str(), ok, PositionVector());
    NBNetBuilder::transformCoordinates(myCurrentEdge->shape, true, myLocation);
    myCurrentEdge->length = attrs.getOpt<SUMOReal>(SUMO_ATTR_LENGTH, id.c_str(), ok, NBEdge::UNSPECIFIED_LOADED_LENGTH);
    myCurrentEdge->maxSpeed = 0;
    myCurrentEdge->streetName = attrs.getOpt<std::string>(SUMO_ATTR_NAME, id.c_str(), ok, "");
    if (myCurrentEdge->streetName != "" && OptionsCont::getOptions().isDefault("output.street-names")) {
        OptionsCont::getOptions().set("output.street-names", "true");
    }

    std::string lsfS = toString(LANESPREAD_RIGHT);
    lsfS = attrs.getOpt<std::string>(SUMO_ATTR_SPREADTYPE, id.c_str(), ok, lsfS);
    if (SUMOXMLDefinitions::LaneSpreadFunctions.hasString(lsfS)) {
        myCurrentEdge->lsf = SUMOXMLDefinitions::LaneSpreadFunctions.get(lsfS);
    } else {
        WRITE_ERROR("Unknown spreadType '" + lsfS + "' for edge '" + id + "'.");
    }
}


void
NIImporter_SUMO::addLane(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        return;
    }
    if (!myCurrentEdge) {
        WRITE_ERROR("Found lane '" + id  + "' not within edge element");
        return;
    }
    if (attrs.getOpt<bool>(SUMO_ATTR_CUSTOMSHAPE, 0, ok, false)) {
        const std::string nodeID = NBNode::getNodeIDFromInternalLane(id);
        myCustomShapeMaps[nodeID][id] = attrs.get<PositionVector>(SUMO_ATTR_SHAPE, id.c_str(), ok);
    }
    myCurrentLane = new LaneAttrs();
    if (myCurrentEdge->func == EDGEFUNC_CROSSING) {
        // save the width and the lane id of the crossing but don't do anything else
        std::vector<Crossing>& crossings = myPedestrianCrossings[SUMOXMLDefinitions::getJunctionIDFromInternalEdge(myCurrentEdge->id)];
        assert(crossings.size() > 0);
        crossings.back().width = attrs.get<SUMOReal>(SUMO_ATTR_WIDTH, id.c_str(), ok);
        return;
    } else if (myCurrentEdge->func == EDGEFUNC_INTERNAL || myCurrentEdge->func == EDGEFUNC_WALKINGAREA) {
        myHaveSeenInternalEdge = true;
        return; // skip internal lanes
    }
    if (attrs.hasAttribute("maxspeed")) {
        // !!! deprecated
        myCurrentLane->maxSpeed = attrs.getFloat("maxspeed");
    } else {
        myCurrentLane->maxSpeed = attrs.get<SUMOReal>(SUMO_ATTR_SPEED, id.c_str(), ok);
    }
    try {
        myCurrentLane->allow = attrs.getOpt<std::string>(SUMO_ATTR_ALLOW, id.c_str(), ok, "", false);
    } catch (EmptyData e) {
        // !!! deprecated
        myCurrentLane->allow = "";
    }
    myCurrentLane->disallow = attrs.getOpt<std::string>(SUMO_ATTR_DISALLOW, id.c_str(), ok, "");
    myCurrentLane->width = attrs.getOpt<SUMOReal>(SUMO_ATTR_WIDTH, id.c_str(), ok, (SUMOReal) NBEdge::UNSPECIFIED_WIDTH);
    myCurrentLane->endOffset = attrs.getOpt<SUMOReal>(SUMO_ATTR_ENDOFFSET, id.c_str(), ok, (SUMOReal) NBEdge::UNSPECIFIED_OFFSET);
    myCurrentLane->shape = attrs.get<PositionVector>(SUMO_ATTR_SHAPE, id.c_str(), ok);
    // lane coordinates are derived (via lane spread) do not include them in convex boundary
    NBNetBuilder::transformCoordinates(myCurrentLane->shape, false, myLocation);
}


void
NIImporter_SUMO::addJunction(const SUMOSAXAttributes& attrs) {
    // get the id, report an error if not given or empty...
    myCurrentJunction.node = 0;
    myCurrentJunction.intLanes.clear();
    myCurrentJunction.response.clear();
    bool ok = true;
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        return;
    }
    if (id[0] == ':') { // internal node
        return;
    }
    SumoXMLNodeType type = attrs.getNodeType(ok);
    if (ok) {
        if (type == NODETYPE_DEAD_END_DEPRECATED || type == NODETYPE_DEAD_END) {
            // dead end is a computed status. Reset this to unknown so it will
            // be corrected if additional connections are loaded
            type = NODETYPE_UNKNOWN;
        }
    } else {
        WRITE_WARNING("Unknown node type for junction '" + id + "'.");
    }
    Position pos = readPosition(attrs, id, ok);
    NBNetBuilder::transformCoordinate(pos, true, myLocation);
    NBNode* node = new NBNode(id, pos, type);
    if (!myNodeCont.insert(node)) {
        WRITE_ERROR("Problems on adding junction '" + id + "'.");
        delete node;
        return;
    }
    myCurrentJunction.node = node;
    SUMOSAXAttributes::parseStringVector(attrs.get<std::string>(SUMO_ATTR_INTLANES, 0, ok, false), myCurrentJunction.intLanes);
    // set optional radius
    if (attrs.hasAttribute(SUMO_ATTR_RADIUS)) {
        node->setRadius(attrs.get<SUMOReal>(SUMO_ATTR_RADIUS, id.c_str(), ok));
    }
    // handle custom shape
    if (attrs.getOpt<bool>(SUMO_ATTR_CUSTOMSHAPE, 0, ok, false)) {
        node->setCustomShape(attrs.get<PositionVector>(SUMO_ATTR_SHAPE, id.c_str(), ok));
    }
    if (myCustomShapeMaps.count(id) > 0) {
        NBNode::CustomShapeMap customShapes = myCustomShapeMaps[id];
        for (NBNode::CustomShapeMap::const_iterator it = customShapes.begin(); it != customShapes.end(); ++it) {
            node->setCustomLaneShape(it->first, it->second);
        }
    }
    if (type == NODETYPE_RAIL_SIGNAL || type == NODETYPE_RAIL_CROSSING) {
        // both types of nodes come without a tlLogic
        myRailSignals.insert(id);
    }
}


void
NIImporter_SUMO::addRequest(const SUMOSAXAttributes& attrs) {
    if (myCurrentJunction.node != 0) {
        bool ok = true;
        myCurrentJunction.response.push_back(attrs.get<std::string>(SUMO_ATTR_RESPONSE, 0, ok));
    }
}


void
NIImporter_SUMO::addConnection(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string fromID = attrs.get<std::string>(SUMO_ATTR_FROM, 0, ok);
    if (myEdges.count(fromID) == 0) {
        WRITE_ERROR("Unknown edge '" + fromID + "' given in connection.");
        return;
    }
    EdgeAttrs* from = myEdges[fromID];
    Connection conn;
    conn.toEdgeID = attrs.get<std::string>(SUMO_ATTR_TO, 0, ok);
    int fromLaneIdx = attrs.get<int>(SUMO_ATTR_FROM_LANE, 0, ok);
    conn.toLaneIdx = attrs.get<int>(SUMO_ATTR_TO_LANE, 0, ok);
    conn.tlID = attrs.getOpt<std::string>(SUMO_ATTR_TLID, 0, ok, "");
    conn.mayDefinitelyPass = attrs.getOpt<bool>(SUMO_ATTR_PASS, 0, ok, false);
    conn.keepClear = attrs.getOpt<bool>(SUMO_ATTR_KEEP_CLEAR, 0, ok, true);
    conn.contPos = attrs.getOpt<SUMOReal>(SUMO_ATTR_CONTPOS, 0, ok, NBEdge::UNSPECIFIED_CONTPOS);
    conn.visibility = attrs.getOpt<SUMOReal>(SUMO_ATTR_VISIBILITY_DISTANCE, 0, ok, NBEdge::UNSPECIFIED_VISIBILITY_DISTANCE);
    if (conn.tlID != "") {
        conn.tlLinkNo = attrs.get<int>(SUMO_ATTR_TLLINKINDEX, 0, ok);
    }
    if ((int)from->lanes.size() <= fromLaneIdx) {
        WRITE_ERROR("Invalid lane index '" + toString(fromLaneIdx) + "' for connection from '" + fromID + "'.");
        return;
    }
    from->lanes[fromLaneIdx]->connections.push_back(conn);

    // determine crossing priority
    if (myPedestrianCrossings.size() > 0
            && from->func == EDGEFUNC_WALKINGAREA
            && myEdges[conn.toEdgeID]->func == EDGEFUNC_CROSSING) {
        std::vector<Crossing>& crossings = myPedestrianCrossings[SUMOXMLDefinitions::getJunctionIDFromInternalEdge(fromID)];
        for (std::vector<Crossing>::iterator it = crossings.begin(); it != crossings.end(); ++it) {
            if (conn.toEdgeID == (*it).edgeID) {
                if (conn.tlID != "") {
                    (*it).priority = true;
                } else {
                    LinkState state = SUMOXMLDefinitions::LinkStates.get(attrs.get<std::string>(SUMO_ATTR_STATE, 0, ok));
                    (*it).priority = state == LINKSTATE_MAJOR;
                }
            }
        }
    }
}


void
NIImporter_SUMO::addProhibition(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string prohibitor = attrs.getOpt<std::string>(SUMO_ATTR_PROHIBITOR, 0, ok, "");
    std::string prohibited = attrs.getOpt<std::string>(SUMO_ATTR_PROHIBITED, 0, ok, "");
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
    int index;
    interpretLaneID(lane_id, edge_id, index);
    assert(edge->id == edge_id);
    if ((int)edge->lanes.size() <= index) {
        WRITE_ERROR("Unknown lane '" + lane_id + "' given in succedge.");
        return 0;
    } else {
        return edge->lanes[index];
    }
}


void
NIImporter_SUMO::interpretLaneID(const std::string& lane_id, std::string& edge_id, int& index) {
    // assume lane_id = edge_id + '_' + index
    const std::string::size_type sep_index = lane_id.rfind('_');
    if (sep_index == std::string::npos) {
        WRITE_ERROR("Invalid lane id '" + lane_id + "' (missing '_').");
    }
    edge_id = lane_id.substr(0, sep_index);
    std::string index_string = lane_id.substr(sep_index + 1);
    try {
        index = TplConvert::_2int(index_string.c_str());
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
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    SUMOTime offset = TIME2STEPS(attrs.get<SUMOReal>(SUMO_ATTR_OFFSET, id.c_str(), ok));
    std::string programID = attrs.getOpt<std::string>(SUMO_ATTR_PROGRAMID, id.c_str(), ok, "<unknown>");
    std::string typeS = attrs.get<std::string>(SUMO_ATTR_TYPE, 0, ok);
    TrafficLightType type;
    if (SUMOXMLDefinitions::TrafficLightTypes.hasString(typeS)) {
        type = SUMOXMLDefinitions::TrafficLightTypes.get(typeS);
    } else {
        WRITE_ERROR("Unknown traffic light type '" + typeS + "' for tlLogic '" + id + "'.");
        return 0;
    }
    if (ok) {
        return new NBLoadedSUMOTLDef(id, programID, offset, type);
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
    std::string state = attrs.get<std::string>(SUMO_ATTR_STATE, id.c_str(), ok);
    SUMOTime duration = TIME2STEPS(attrs.get<SUMOReal>(SUMO_ATTR_DURATION, id.c_str(), ok));
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
    const int noLanes = (int)edge->lanes.size();
    SUMOReal offset;
    if (edge->lsf == LANESPREAD_RIGHT) {
        offset = (SUMO_const_laneWidth + SUMO_const_laneOffset) / 2.; // @todo: why is the lane offset counted in here?
    } else {
        offset = (SUMO_const_laneWidth) / 2. - (SUMO_const_laneWidth * (SUMOReal)noLanes - 1) / 2.; ///= -2.; // @todo: actually, when looking at the road networks, the center line is not in the center
    }
    for (int i = 1; i < (int)firstLane.size() - 1; i++) {
        const Position& from = firstLane[i - 1];
        const Position& me = firstLane[i];
        const Position& to = firstLane[i + 1];
        Position offsets = PositionVector::sideOffset(from, me, offset);
        Position offsets2 = PositionVector::sideOffset(me, to, offset);

        PositionVector l1(from - offsets, me - offsets);
        l1.extrapolate(100);
        PositionVector l2(me - offsets2, to - offsets2);
        l2.extrapolate(100);
        if (l1.intersects(l2)) {
            result.push_back(l1.intersectionPosition2D(l2));
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
    PositionVector s = attrs.get<PositionVector>(SUMO_ATTR_NET_OFFSET, 0, ok);
    Boundary convBoundary = attrs.get<Boundary>(SUMO_ATTR_CONV_BOUNDARY, 0, ok);
    Boundary origBoundary = attrs.get<Boundary>(SUMO_ATTR_ORIG_BOUNDARY, 0, ok);
    std::string proj = attrs.get<std::string>(SUMO_ATTR_ORIG_PROJ, 0, ok);
    if (ok) {
        Position networkOffset = s[0];
        result = new GeoConvHelper(proj, networkOffset, origBoundary, convBoundary);
        GeoConvHelper::setLoaded(*result);
    }
    return result;
}


Position
NIImporter_SUMO::readPosition(const SUMOSAXAttributes& attrs, const std::string& id, bool& ok) {
    SUMOReal x = attrs.get<SUMOReal>(SUMO_ATTR_X, id.c_str(), ok);
    SUMOReal y = attrs.get<SUMOReal>(SUMO_ATTR_Y, id.c_str(), ok);
    SUMOReal z = 0;
    if (attrs.hasAttribute(SUMO_ATTR_Z)) {
        z = attrs.get<SUMOReal>(SUMO_ATTR_Z, id.c_str(), ok);
    }
    return Position(x, y, z);
}


void
NIImporter_SUMO::parseProhibitionConnection(const std::string& attr, std::string& from, std::string& to, bool& ok) {
    // split from/to
    const std::string::size_type div = attr.find("->");
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


void
NIImporter_SUMO::addRoundabout(const SUMOSAXAttributes& attrs) {
    if (attrs.hasAttribute(SUMO_ATTR_EDGES)) {
        myRoundabouts.push_back(attrs.getStringVector(SUMO_ATTR_EDGES));
    } else {
        WRITE_ERROR("Empty edges in roundabout.");
    }
}


/****************************************************************************/
