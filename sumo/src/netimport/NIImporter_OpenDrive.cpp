/****************************************************************************/
/// @file    NIImporter_OpenDrive.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Mon, 14.04.2008
/// @version $Id$
///
// Importer for networks stored in openDrive format
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
#include <cmath>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/MsgHandler.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBNetBuilder.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/geom/GeomConvHelper.h>
#include <foreign/eulerspiral/euler.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/FileHelpers.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/geom/Boundary.h>
#include "NILoader.h"
#include "NIImporter_OpenDrive.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// definitions
// ===========================================================================
#define C_LENGTH 10.


// ===========================================================================
// static variables
// ===========================================================================
StringBijection<int>::Entry NIImporter_OpenDrive::openDriveTags[] = {
    { "header",           NIImporter_OpenDrive::OPENDRIVE_TAG_HEADER },
    { "road",             NIImporter_OpenDrive::OPENDRIVE_TAG_ROAD },
    { "predecessor",      NIImporter_OpenDrive::OPENDRIVE_TAG_PREDECESSOR },
    { "successor",        NIImporter_OpenDrive::OPENDRIVE_TAG_SUCCESSOR },
    { "geometry",         NIImporter_OpenDrive::OPENDRIVE_TAG_GEOMETRY },
    { "line",             NIImporter_OpenDrive::OPENDRIVE_TAG_LINE },
    { "spiral",           NIImporter_OpenDrive::OPENDRIVE_TAG_SPIRAL },
    { "arc",              NIImporter_OpenDrive::OPENDRIVE_TAG_ARC },
    { "poly3",            NIImporter_OpenDrive::OPENDRIVE_TAG_POLY3 },
    { "laneSection",      NIImporter_OpenDrive::OPENDRIVE_TAG_LANESECTION },
    { "left",             NIImporter_OpenDrive::OPENDRIVE_TAG_LEFT },
    { "center",           NIImporter_OpenDrive::OPENDRIVE_TAG_CENTER },
    { "right",            NIImporter_OpenDrive::OPENDRIVE_TAG_RIGHT },
    { "lane",             NIImporter_OpenDrive::OPENDRIVE_TAG_LANE },

    { "",                 NIImporter_OpenDrive::OPENDRIVE_TAG_NOTHING }
};


StringBijection<int>::Entry NIImporter_OpenDrive::openDriveAttrs[] = {
    { "revMajor",       NIImporter_OpenDrive::OPENDRIVE_ATTR_REVMAJOR },
    { "revMinor",       NIImporter_OpenDrive::OPENDRIVE_ATTR_REVMINOR },
    { "id",             NIImporter_OpenDrive::OPENDRIVE_ATTR_ID },
    { "length",         NIImporter_OpenDrive::OPENDRIVE_ATTR_LENGTH },
    { "junction",       NIImporter_OpenDrive::OPENDRIVE_ATTR_JUNCTION },
    { "elementType",    NIImporter_OpenDrive::OPENDRIVE_ATTR_ELEMENTTYPE },
    { "elementId",      NIImporter_OpenDrive::OPENDRIVE_ATTR_ELEMENTID },
    { "contactPoint",   NIImporter_OpenDrive::OPENDRIVE_ATTR_CONTACTPOINT },
    { "s",              NIImporter_OpenDrive::OPENDRIVE_ATTR_S },
    { "x",              NIImporter_OpenDrive::OPENDRIVE_ATTR_X },
    { "y",              NIImporter_OpenDrive::OPENDRIVE_ATTR_Y },
    { "hdg",            NIImporter_OpenDrive::OPENDRIVE_ATTR_HDG },
    { "curvStart",      NIImporter_OpenDrive::OPENDRIVE_ATTR_CURVSTART },
    { "curvEnd",        NIImporter_OpenDrive::OPENDRIVE_ATTR_CURVEND },
    { "curvature",      NIImporter_OpenDrive::OPENDRIVE_ATTR_CURVATURE },
    { "a",              NIImporter_OpenDrive::OPENDRIVE_ATTR_A },
    { "b",              NIImporter_OpenDrive::OPENDRIVE_ATTR_B },
    { "c",              NIImporter_OpenDrive::OPENDRIVE_ATTR_C },
    { "d",              NIImporter_OpenDrive::OPENDRIVE_ATTR_D },
    { "type",           NIImporter_OpenDrive::OPENDRIVE_ATTR_TYPE },
    { "level",          NIImporter_OpenDrive::OPENDRIVE_ATTR_LEVEL },

    { "",               NIImporter_OpenDrive::OPENDRIVE_ATTR_NOTHING }
};


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static methods (interface in this case)
// ---------------------------------------------------------------------------
void
NIImporter_OpenDrive::loadNetwork(const OptionsCont& oc, NBNetBuilder& nb) {
    // check whether the option is set (properly)
    if (!oc.isUsableFileList("opendrive-files")) {
        return;
    }
    // build the handler
    std::vector<OpenDriveEdge> innerEdges, outerEdges;
    NIImporter_OpenDrive handler(innerEdges, outerEdges);
    // parse file(s)
    std::vector<std::string> files = oc.getStringVector("opendrive-files");
    for (std::vector<std::string>::const_iterator file = files.begin(); file != files.end(); ++file) {
        if (!FileHelpers::exists(*file)) {
            WRITE_ERROR("Could not open opendrive file '" + *file + "'.");
            return;
        }
        handler.setFileName(*file);
        PROGRESS_BEGIN_MESSAGE("Parsing opendrive from '" + *file + "'");
        XMLSubSys::runParser(handler, *file);
        PROGRESS_DONE_MESSAGE();
    }
    // convert geometries into a discretised representation
    computeShapes(innerEdges);
    computeShapes(outerEdges);

    // -------------------------
    // node building
    // -------------------------
    // build nodes#1
    //  look at all links which belong to a node, collect their bounding boxes
    //  and place the node in the middle of this bounding box
    std::map<std::string, Boundary> posMap;
    std::map<std::string, std::string> edge2junction;
    //   compute node positions
    for (std::vector<OpenDriveEdge>::iterator i = innerEdges.begin(); i != innerEdges.end(); ++i) {
        OpenDriveEdge& e = *i;
        assert(e.junction != "-1" && e.junction != "");
        edge2junction[e.id] = e.junction;
        if (posMap.find(e.junction) == posMap.end()) {
            posMap[e.junction] = Boundary();
        }
        posMap[e.junction].add(e.geom.getBoxBoundary());
    }
    //   build nodes
    for (std::map<std::string, Boundary>::iterator i = posMap.begin(); i != posMap.end(); ++i) {
        if (!nb.getNodeCont().insert((*i).first, (*i).second.getCenter())) {
            throw ProcessError("Could not add node '" + (*i).first + "'.");
        }
    }
    //  assign built nodes
    for (std::vector<OpenDriveEdge>::iterator i = outerEdges.begin(); i != outerEdges.end(); ++i) {
        OpenDriveEdge& e = *i;
        for (std::vector<OpenDriveLink>::iterator j = e.links.begin(); j != e.links.end(); ++j) {
            OpenDriveLink& l = *j;
            if (l.elementType != OPENDRIVE_ET_ROAD) {
                // set node information
                setNodeSecure(nb.getNodeCont(), e, l.elementID, l.linkType);
                continue;
            }
            if (edge2junction.find(l.elementID) != edge2junction.end()) {
                // set node information of an internal road
                setNodeSecure(nb.getNodeCont(), e, edge2junction[l.elementID], l.linkType);
                continue;
            }
        }
    }
    //  we should now have all nodes set for links which are not outer edge-to-outer edge links


    // build nodes#2
    //  build nodes for all outer edge-to-outer edge connections
    for (std::vector<OpenDriveEdge>::iterator i = outerEdges.begin(); i != outerEdges.end(); ++i) {
        OpenDriveEdge& e = *i;
        for (std::vector<OpenDriveLink>::iterator j = e.links.begin(); j != e.links.end(); ++j) {
            OpenDriveLink& l = *j;
            if (l.elementType != OPENDRIVE_ET_ROAD || edge2junction.find(l.elementID) != edge2junction.end()) {
                // is a connection to an internal edge, or a node, skip
                continue;
            }
            // we have a direct connection between to external edges
            std::string id1 = e.id;
            std::string id2 = l.elementID;
            if (id1 < id2) {
                std::swap(id1, id2);
            }
            std::string nid = id1 + "." + id2;
            if (nb.getNodeCont().retrieve(nid) == 0) {
                // not yet seen, build
                Position pos = l.linkType == OPENDRIVE_LT_SUCCESSOR ? e.geom[(int)e.geom.size() - 1] : e.geom[0];
                if (!nb.getNodeCont().insert(nid, pos)) {
                    throw ProcessError("Could not build node '" + nid + "'.");
                }
            }
            /* debug-stuff
            else {
                Position pos = l.linkType==OPENDRIVE_LT_SUCCESSOR ? e.geom[e.geom.size()-1] : e.geom[0];
                cout << nid << " " << pos << " " << nb.getNodeCont().retrieve(nid)->getPosition() << endl;
            }
            */
            setNodeSecure(nb.getNodeCont(), e, nid, l.linkType);
        }
    }
    // we should now have start/end nodes for all outer edge-to-outer edge connections


    // build nodes#3
    //  assign further nodes generated from inner-edges
    //  these nodes have not been assigned earlier, because the connectiosn are referenced in inner-edges
    for (std::vector<OpenDriveEdge>::iterator i = outerEdges.begin(); i != outerEdges.end(); ++i) {
        OpenDriveEdge& e = *i;
        if (e.to != 0 && e.from != 0) {
            continue;
        }
        for (std::vector<OpenDriveEdge>::iterator j = innerEdges.begin(); j != innerEdges.end(); ++j) {
            OpenDriveEdge& ie = *j;
            for (std::vector<OpenDriveLink>::iterator k = ie.links.begin(); k != ie.links.end(); ++k) {
                OpenDriveLink& il = *k;
                if (il.elementType != OPENDRIVE_ET_ROAD || il.elementID != e.id) {
                    // not conneted to the currently investigated outer edge
                    continue;
                }
                std::string nid = edge2junction[ie.id];
                if (il.contactPoint == OPENDRIVE_CP_START) {
                    setNodeSecure(nb.getNodeCont(), e, nid, OPENDRIVE_LT_PREDECESSOR);
                } else {
                    setNodeSecure(nb.getNodeCont(), e, nid, OPENDRIVE_LT_SUCCESSOR);
                }
            }
        }

    }
    //


    // build start/end nodes which were not defined previously
    for (std::vector<OpenDriveEdge>::iterator i = outerEdges.begin(); i != outerEdges.end(); ++i) {
        OpenDriveEdge& e = *i;
        if (e.from == 0) {
            std::string nid = e.id + ".begin";
            Position pos(e.geometries[0].x, e.geometries[0].y);
            e.from = getOrBuildNode(nid, e.geom[0], nb.getNodeCont());
        }
        if (e.to == 0) {
            std::string nid = e.id + ".end";
            Position pos(e.geometries[e.geometries.size() - 1].x, e.geometries[e.geometries.size() - 1].y);
            e.to = getOrBuildNode(nid, e.geom[(int)e.geom.size() - 1], nb.getNodeCont());
        }
    }


    // -------------------------
    // edge building
    // -------------------------
    std::map<NBEdge*, std::map<int, int> > fromLaneMap;
    std::map<NBEdge*, std::map<int, int> > toLaneMap;
    // build edges
    for (std::vector<OpenDriveEdge>::iterator i = outerEdges.begin(); i != outerEdges.end(); ++i) {
        OpenDriveEdge& e = *i;
        SUMOReal speed = nb.getTypeCont().getSpeed("");
        int priority = nb.getTypeCont().getPriority("");
        LaneSpreadFunction lsf = LANESPREAD_CENTER;
        unsigned int noLanesRight = e.getMaxLaneNumber(OPENDRIVE_TAG_RIGHT);
        unsigned int noLanesLeft = e.getMaxLaneNumber(OPENDRIVE_TAG_LEFT);
        if (noLanesRight != 0 || noLanesLeft != 0) {
            lsf = LANESPREAD_RIGHT;
        } else {
            WRITE_WARNING("Edge '" + e.id + "' has no lanes.");
        }
        if (noLanesRight > 0) {
            NBEdge* nbe = new NBEdge("-" + e.id, e.from, e.to, "", speed, noLanesRight, priority, -1, -1, e.geom, "", lsf, true);
            if (!nb.getEdgeCont().insert(nbe)) {
                throw ProcessError("Could not add edge '" + std::string("-") + e.id + "'.");
            }
            fromLaneMap[nbe] = e.laneSections.back().buildLaneMapping(OPENDRIVE_TAG_RIGHT);
            toLaneMap[nbe] = e.laneSections[0].buildLaneMapping(OPENDRIVE_TAG_RIGHT);
        }
        if (noLanesLeft > 0) {
            NBEdge* nbe = new NBEdge(e.id, e.to, e.from, "", speed, noLanesLeft, priority, -1, -1, e.geom.reverse(), "", lsf, true);
            if (!nb.getEdgeCont().insert(nbe)) {
                throw ProcessError("Could not add edge '" + e.id + "'.");
            }
            fromLaneMap[nbe] = e.laneSections[0].buildLaneMapping(OPENDRIVE_TAG_LEFT);
            toLaneMap[nbe] = e.laneSections.back().buildLaneMapping(OPENDRIVE_TAG_LEFT);
        }
    }


    // -------------------------
    // connections building
    // -------------------------
    std::vector<Connection> connections;
    // connections#1
    //  build connections between outer-edges
    for (std::vector<OpenDriveEdge>::iterator i = outerEdges.begin(); i != outerEdges.end(); ++i) {
        OpenDriveEdge& e = *i;
        for (std::vector<OpenDriveLink>::iterator j = e.links.begin(); j != e.links.end(); ++j) {
            OpenDriveLink& l = *j;
            if (l.elementType != OPENDRIVE_ET_ROAD) {
                // we are not interested in connections to nodes
                continue;
            }
            if (edge2junction.find(l.elementID) != edge2junction.end()) {
                // connection via an inner-road
                addViaConnectionSecure(nb.getEdgeCont(),
                                       nb.getNodeCont().retrieve(edge2junction[l.elementID]),
                                       e, l.linkType, l.elementID, connections);
            } else {
                // connection between two outer-edges; can be used directly
                std::vector<OpenDriveEdge>::iterator p = std::find_if(outerEdges.begin(), outerEdges.end(), edge_by_id_finder(l.elementID));
                if (p == outerEdges.end()) {
                    throw ProcessError("Could not find connection edge.");
                }
                std::string id1 = e.id;
                std::string id2 = (*p).id;
                if (id1 < id2) {
                    std::swap(id1, id2);
                }
                std::string nid = id1 + "." + id2;
                if (l.linkType == OPENDRIVE_LT_PREDECESSOR) {
                    addE2EConnectionsSecure(nb.getEdgeCont(), nb.getNodeCont().retrieve(nid), *p, e, connections);
                } else {
                    addE2EConnectionsSecure(nb.getEdgeCont(), nb.getNodeCont().retrieve(nid), e, *p, connections);
                }
            }
        }
    }

    // build inner-edge connections
    for (std::vector<OpenDriveEdge>::iterator i = innerEdges.begin(); i != innerEdges.end(); ++i) {
        OpenDriveEdge& e = *i;
        std::string pred, succ;
        ContactPoint predC, succC;
        for (std::vector<OpenDriveLink>::iterator j = e.links.begin(); j != e.links.end(); ++j) {
            OpenDriveLink& l = *j;
            if (l.elementType != OPENDRIVE_ET_ROAD) {
                // we are not interested in connections to nodes
                std::cout << "unsupported" << std::endl;
                continue;
            }
            if (edge2junction.find(l.elementID) != edge2junction.end()) {
                // not supported
                std::cout << "unsupported" << std::endl;
                continue;
            }
            if (l.linkType == OPENDRIVE_LT_SUCCESSOR) {
                if (succ != "") {
                    std::cout << "double succ" << std::endl;
                }
                succ = l.elementID;
                succC = l.contactPoint;
            } else {
                if (pred != "") {
                    std::cout << "double pred" << std::endl;
                }
                pred = l.elementID;
                predC = l.contactPoint;
            }
        }

        if (e.getMaxLaneNumber(OPENDRIVE_TAG_LEFT) != 0 && e.getMaxLaneNumber(OPENDRIVE_TAG_RIGHT) != 0) {
            std::cout << "Both dirs given!" << std::endl;
        }

        bool isReversed = false;
        if (e.getMaxLaneNumber(OPENDRIVE_TAG_LEFT) != 0) {
            //            std::swap(pred, succ);
            //std::swap(predC, succC);
            isReversed = true;
        }

        if (succ == "" || pred == "") {
            std::cout << "Missing edge." << std::endl;
            continue; // yes, occurs
        }
        // !!!why edge2junction; e already has the junction...
        NBNode* n = nb.getNodeCont().retrieve(edge2junction[e.id]);
        std::vector<OpenDriveEdge>::iterator predEdge = std::find_if(outerEdges.begin(), outerEdges.end(), edge_by_id_finder(pred));
        if (predEdge == outerEdges.end()) {
            throw ProcessError("Could not find connection edge.");
        }
        std::vector<OpenDriveEdge>::iterator succEdge = std::find_if(outerEdges.begin(), outerEdges.end(), edge_by_id_finder(succ));
        if (succEdge == outerEdges.end()) {
            throw ProcessError("Could not find connection edge.");
        }
        NBEdge* fromEdge, *toEdge;
        if (!isReversed) {
            fromEdge = predC == OPENDRIVE_CP_END ? nb.getEdgeCont().retrieve("-" + pred) : nb.getEdgeCont().retrieve(pred);
            toEdge = succC == OPENDRIVE_CP_START ? nb.getEdgeCont().retrieve("-" + succ) : nb.getEdgeCont().retrieve(succ);
        } else {
            fromEdge = predC != OPENDRIVE_CP_END ? nb.getEdgeCont().retrieve("-" + pred) : nb.getEdgeCont().retrieve(pred);
            toEdge = succC != OPENDRIVE_CP_START ? nb.getEdgeCont().retrieve("-" + succ) : nb.getEdgeCont().retrieve(succ);
        }
        /*
        Connection c(
            n->hasIncoming(nb.getEdgeCont().retrieve("-" + pred)) ? nb.getEdgeCont().retrieve("-" + pred) : nb.getEdgeCont().retrieve(pred),
            e.id,
            n->hasOutgoing(nb.getEdgeCont().retrieve("-" + succ)) ? nb.getEdgeCont().retrieve("-" + succ) : nb.getEdgeCont().retrieve(succ));
            */
        Connection c(fromEdge, e.id, toEdge);
        if (c.from == 0 || c.to == 0 || c.from == c.to) {
            throw ProcessError("Something's false");
        }
        setLaneConnections(c,
                           *predEdge, c.from->getID()[0] != '-', c.from->getID()[0] == '-' ? OPENDRIVE_TAG_RIGHT : OPENDRIVE_TAG_LEFT,
                           e, isReversed, !isReversed ? OPENDRIVE_TAG_RIGHT : OPENDRIVE_TAG_LEFT,
                           *succEdge, c.to->getID()[0] != '-', c.to->getID()[0] == '-' ? OPENDRIVE_TAG_RIGHT : OPENDRIVE_TAG_LEFT);
        connections.push_back(c);
    }
    for (std::vector<Connection>::const_iterator i = connections.begin(); i != connections.end(); ++i) {
        if ((*i).from == 0 || (*i).to == 0) {
            std::cout << "Nope." << std::endl;
            continue;
        }
        (*i).from->addEdge2EdgeConnection((*i).to);
        std::map<int, int> fromMap = fromLaneMap[(*i).from];
        std::map<int, int> toMap = fromLaneMap[(*i).to];
        for (std::vector<std::pair<int, int> >::const_iterator j = (*i).lanes.begin(); j != (*i).lanes.end(); ++j) {
            int fromLane = fromMap[(*j).first];
            int toLane = toMap[(*j).second];
            if (static_cast<unsigned int>(fromLane) >= (*i).from->getNumLanes() || fromLane < 0) {
                std::cout << "False " << std::endl;
            }
            if (static_cast<unsigned int>(toLane) >= (*i).to->getNumLanes() || toLane < 0) {
                std::cout << "False " << std::endl;
            }

            (*i).from->addLane2LaneConnection(fromLane, (*i).to, toLane, NBEdge::L2L_VALIDATED, true);
        }
    }
}


void
NIImporter_OpenDrive::addViaConnectionSecure(const NBEdgeCont& ec,
        const NBNode* const node, const OpenDriveEdge& e,
        LinkType lt, const std::string& via,
        std::vector<NIImporter_OpenDrive::Connection> &connections) {
    NBEdge* from = 0;
    NBEdge* to = 0;
    if (node == e.to) {
        // the connection is at the end of the "positive" direction
        if (lt == OPENDRIVE_LT_PREDECESSOR) {
            // via -> edge
            to = ec.retrieve(e.id);
        } else {
            // -edge -> via
            //  "ambigous?"
            from = ec.retrieve("-" + e.id);
        }
    } else {
        // the connection is at the begin of the "positive" direction
        if (lt == OPENDRIVE_LT_PREDECESSOR) {
            // via -> -edge
            to = ec.retrieve("-" + e.id);
        } else {
            // edge -> via
            //  "ambigous?"
            from = ec.retrieve(e.id);
        }
    }
    if (from == 0 && to == 0) {
        throw ProcessError("Missing edge");
    }
    Connection c(from, via, to);
    connections.push_back(c);
}


void
NIImporter_OpenDrive::addE2EConnectionsSecure(const NBEdgeCont& ec, const NBNode* const node,
        const NIImporter_OpenDrive::OpenDriveEdge& from, const NIImporter_OpenDrive::OpenDriveEdge& to,
        std::vector<NIImporter_OpenDrive::Connection> &connections) {
    // positive direction (from is incoming, to is outgoing)
    NBEdge* fromEdge = ec.retrieve("-" + from.id);
    if (fromEdge == 0 || !node->hasIncoming(fromEdge)) {
        fromEdge = ec.retrieve(from.id);
    }
    NBEdge* toEdge = ec.retrieve("-" + to.id);
    if (toEdge == 0 || !node->hasOutgoing(toEdge)) {
        toEdge = ec.retrieve(to.id);
    }
    if (fromEdge != 0 && toEdge != 0) {
        Connection c(fromEdge, "", toEdge);
        setLaneConnections(c,
                           from, c.from->getID()[0] != '-', c.from->getID()[0] == '-' ? OPENDRIVE_TAG_RIGHT : OPENDRIVE_TAG_LEFT,
                           to, c.to->getID()[0] != '-', c.to->getID()[0] == '-' ? OPENDRIVE_TAG_RIGHT : OPENDRIVE_TAG_LEFT);
        connections.push_back(c);
    }
    // negative direction (to is incoming, from is outgoing)
    fromEdge = ec.retrieve("-" + from.id);
    if (fromEdge == 0 || !node->hasOutgoing(fromEdge)) {
        fromEdge = ec.retrieve(from.id);
    }
    toEdge = ec.retrieve("-" + to.id);
    if (toEdge == 0 || !node->hasIncoming(toEdge)) {
        toEdge = ec.retrieve(to.id);
    }
    if (fromEdge != 0 && toEdge != 0) {
        Connection c(toEdge, "", fromEdge);
        setLaneConnections(c,
                           to, c.to->getID()[0] != '-', c.to->getID()[0] == '-' ? OPENDRIVE_TAG_RIGHT : OPENDRIVE_TAG_LEFT,
                           from, c.from->getID()[0] != '-', c.from->getID()[0] == '-' ? OPENDRIVE_TAG_RIGHT : OPENDRIVE_TAG_LEFT);
        connections.push_back(c);
    }
}


void
NIImporter_OpenDrive::setLaneConnections(NIImporter_OpenDrive::Connection& c,
        const OpenDriveEdge& from, bool fromAtBegin, OpenDriveXMLTag fromLaneDir,
        const OpenDriveEdge& to, bool toAtEnd, OpenDriveXMLTag toLaneDir) {
    const OpenDriveLaneSection& fromLS = fromAtBegin ? from.laneSections[0] : from.laneSections.back();
    const std::vector<OpenDriveLane> &fromLanes = fromLS.lanesByDir.find(fromLaneDir)->second;
    const OpenDriveLaneSection& toLS = toAtEnd ? to.laneSections.back() : to.laneSections[0];
    const std::vector<OpenDriveLane> &toLanes = toLS.lanesByDir.find(toLaneDir)->second;
    // in the following, we are probably using the same information twice, stored once
    //  in the from-edge's successor field and in the to-edge's precessor field.
    // though, we have no proof or information that this is always redundant
    for (std::vector<OpenDriveLane>::const_iterator i = fromLanes.begin(); i != fromLanes.end(); ++i) {
        if ((*i).type != "driving") {
            continue;
        }
        if (!fromAtBegin && (*i).successor != UNSET_CONNECTION) {
            c.lanes.push_back(std::make_pair((*i).id, (*i).successor));
        }
        if (fromAtBegin && (*i).predecessor != UNSET_CONNECTION) {
            c.lanes.push_back(std::make_pair((*i).id, (*i).predecessor));
        }
    }
    for (std::vector<OpenDriveLane>::const_iterator i = toLanes.begin(); i != toLanes.end(); ++i) {
        if ((*i).type != "driving") {
            continue;
        }
        if (!toAtEnd && (*i).predecessor != UNSET_CONNECTION) {
            c.lanes.push_back(std::make_pair((*i).predecessor, (*i).id));
        }
        if (toAtEnd && (*i).successor != UNSET_CONNECTION) {
            c.lanes.push_back(std::make_pair((*i).successor, (*i).id));
        }
    }
}


void
NIImporter_OpenDrive::setLaneConnections(NIImporter_OpenDrive::Connection& c,
        const OpenDriveEdge& from, bool fromAtBegin, OpenDriveXMLTag fromLaneDir,
        const OpenDriveEdge& via, bool viaIsReversed, OpenDriveXMLTag viaLaneDir,
        const OpenDriveEdge& to, bool toAtEnd, OpenDriveXMLTag toLaneDir) {
    Connection from2via(0, "", 0);
    setLaneConnections(from2via, from, fromAtBegin, fromLaneDir, via, viaIsReversed, viaLaneDir);
    Connection via2to(0, "", 0);
    setLaneConnections(via2to, via, viaIsReversed, viaLaneDir, to, toAtEnd, toLaneDir);
    for (std::vector<std::pair<int, int> >::const_iterator i = from2via.lanes.begin(); i != from2via.lanes.end(); ++i) {
        int fromLane = (*i).first;
        int viaLane = (*i).second;
        for (std::vector<std::pair<int, int> >::const_iterator j = via2to.lanes.begin(); j != via2to.lanes.end(); ++j) {
            if ((*j).first == viaLane) {
                c.lanes.push_back(std::make_pair(fromLane, (*j).second));
                break;
            }
        }
    }
}


NBNode*
NIImporter_OpenDrive::getOrBuildNode(const std::string& id, Position& pos,
                                     NBNodeCont& nc) {
    if (nc.retrieve(id) == 0) {
        // not yet built; build now
        if (!nc.insert(id, pos)) {
            // !!! clean up
            throw ProcessError("Could not add node '" + id + "'.");
        }
    }
    return nc.retrieve(id);
}


void
NIImporter_OpenDrive::setNodeSecure(NBNodeCont& nc, OpenDriveEdge& e,
                                    const std::string& nodeID, NIImporter_OpenDrive::LinkType lt) {
    NBNode* n = nc.retrieve(nodeID);
    if (n == 0) {
        throw ProcessError("Could not find node '" + nodeID + "'.");
    }
    if (lt == OPENDRIVE_LT_SUCCESSOR) {
        if (e.to != 0 && e.to != n) {
            throw ProcessError("Edge '" + e.id + "' has two end nodes.");
        }
        e.to = n;
    } else {
        if (e.from != 0 && e.from != n) {
            throw ProcessError("Edge '" + e.id + "' has two start nodes.");
        }
        e.from = n;
    }
}





NBEdge*
NIImporter_OpenDrive::getOutgoingDirectionalEdge(const NBEdgeCont& ec,
        const NBNodeCont& nc,
        const std::string& edgeID,
        const std::string& nodeID) {
    NBNode* node = nc.retrieve(nodeID);
    NBEdge* e = ec.retrieve(edgeID);
    if (e != 0 && node->hasOutgoing(e)) {
        return e;
    }
    e = ec.retrieve("-" + edgeID);
    if (e != 0 && node->hasOutgoing(e)) {
        return e;
    }
    return 0;
}


NBEdge*
NIImporter_OpenDrive::getIncomingDirectionalEdge(const NBEdgeCont& ec,
        const NBNodeCont& nc,
        const std::string& edgeID,
        const std::string& nodeID) {
    NBNode* node = nc.retrieve(nodeID);
    NBEdge* e = ec.retrieve(edgeID);
    if (e != 0 && node->hasIncoming(e)) {
        return e;
    }
    e = ec.retrieve("-" + edgeID);
    if (e != 0 && node->hasIncoming(e)) {
        return e;
    }
    return 0;
}


void
NIImporter_OpenDrive::computeShapes(std::vector<OpenDriveEdge> &edges) {
    for (std::vector<OpenDriveEdge>::iterator i = edges.begin(); i != edges.end(); ++i) {
        OpenDriveEdge& e = *i;
        for (std::vector<OpenDriveGeometry>::iterator j = e.geometries.begin(); j != e.geometries.end(); ++j) {
            OpenDriveGeometry& g = *j;
            std::vector<Position> geom;
            switch (g.type) {
                case OPENDRIVE_GT_UNKNOWN:
                    break;
                case OPENDRIVE_GT_LINE:
                    geom = geomFromLine(e, g);
                    break;
                case OPENDRIVE_GT_SPIRAL:
                    geom = geomFromSpiral(e, g);
                    break;
                case OPENDRIVE_GT_ARC:
                    geom = geomFromArc(e, g);
                    break;
                case OPENDRIVE_GT_POLY3:
                    geom = geomFromPoly(e, g);
                    break;
                default:
                    break;
            }
            for (std::vector<Position>::iterator k = geom.begin(); k != geom.end(); ++k) {
                e.geom.push_back_noDoublePos(*k);
            }
        }
        for (unsigned int j = 0; j < e.geom.size(); ++j) {
            if (!NILoader::transformCoordinates(e.geom[j])) {
                WRITE_ERROR("Unable to project coordinates for.");
            }
        }
    }
}

std::vector<Position>
NIImporter_OpenDrive::geomFromLine(const OpenDriveEdge& e, const OpenDriveGeometry& g) {
    UNUSED_PARAMETER(e);
    std::vector<Position> ret;
    ret.push_back(Position(g.x, g.y));
    ret.push_back(calculateStraightEndPoint(g.hdg, g.length, Position(g.x, g.y)));
    return ret;
}


std::vector<Position>
NIImporter_OpenDrive::geomFromSpiral(const OpenDriveEdge& e, const OpenDriveGeometry& g) {
    UNUSED_PARAMETER(e);
    std::vector<Position> ret;
    SUMOReal curveStart = g.params[0];
    SUMOReal curveEnd = g.params[1];
    Point2D<double> end;
    EulerSpiral s(Point2D<double>(g.x, g.y), g.hdg, curveStart, (curveEnd - curveStart) / g.length, g.length);
    std::vector<Point2D<double> > into;
    s.computeSpiral(into, 1.);
    for (std::vector<Point2D<double> >::iterator i = into.begin(); i != into.end(); ++i) {
        ret.push_back(Position((*i).getX(), (*i).getY()));
    }
    return ret;
}


std::vector<Position>
NIImporter_OpenDrive::geomFromArc(const OpenDriveEdge& e, const OpenDriveGeometry& g) {
    UNUSED_PARAMETER(e);
    std::vector<Position> ret;
    SUMOReal dist = 0.0;
    SUMOReal centerX = g.x;
    SUMOReal centerY = g.y;
    // left: positive value
    SUMOReal curvature = g.params[0];
    SUMOReal radius = 1. / curvature;
    // center point
    calculateCurveCenter(&centerX, &centerY, radius, g.hdg);
    SUMOReal endX = g.x;
    SUMOReal endY = g.y;
    SUMOReal startX = g.x;
    SUMOReal startY = g.y;
    SUMOReal hdgS = g.hdg;
    SUMOReal hdgE;
    SUMOReal geo_posS = g.s;
    SUMOReal geo_posE = g.s;
    bool end = false;
    do {
        geo_posE += C_LENGTH;
        if (geo_posE - g.s > g.length) {
            geo_posE = g.s + g.length;
        }
        if (geo_posE - g.s > g.length) {
            geo_posE = g.s + g.length;
        }
        calcPointOnCurve(&endX, &endY, centerX, centerY, radius, geo_posE - geo_posS);

        dist += (geo_posE - geo_posS);
        if (curvature > 0.0) {
            hdgE = g.hdg + dist / fabs(radius);
        } else {
            hdgE = g.hdg - dist / fabs(radius);
        }
        //
        ret.push_back(Position(startX, startY));
        //
        startX = endX;
        startY = endY;
        geo_posS = geo_posE;
        hdgS = hdgE;

        if (geo_posE  - (g.s + g.length) < 0.001 && geo_posE  - (g.s + g.length) > -0.001) {
            end = true;
        }
    } while (!end);
    return ret;
}


std::vector<Position>
NIImporter_OpenDrive::geomFromPoly(const OpenDriveEdge& e, const OpenDriveGeometry& g) {
    UNUSED_PARAMETER(g);
    UNUSED_PARAMETER(e);
    std::vector<Position> ret;
    return ret;
}


Position
NIImporter_OpenDrive::calculateStraightEndPoint(double hdg, double length, const Position& start) {
    double normx = 1.0f;
    double normy = 0.0f;
    double x2 = normx * cos(hdg) - normy * sin(hdg);
    double y2 = normx * sin(hdg) + normy * cos(hdg);
    normx = x2 * length;
    normy = y2 * length;
    return Position(start.x() + normx, start.y() + normy);
}


void
NIImporter_OpenDrive::calculateCurveCenter(SUMOReal* ad_x, SUMOReal* ad_y, SUMOReal ad_radius, SUMOReal ad_hdg) {
    SUMOReal normX = 1.0;
    SUMOReal normY = 0.0;
    SUMOReal tmpX;
    SUMOReal turn;
    if (ad_radius > 0) {
        turn = -1.0;
    } else {
        turn = 1.0;
    }

    tmpX = normX;
    normX = normX * cos(ad_hdg) + normY * sin(ad_hdg);
    normY = tmpX * sin(ad_hdg) + normY * cos(ad_hdg);

    tmpX = normX;
    normX = normX * cos(90 * PI / 180) + turn * normY * sin(90 * PI / 180);
    normY = -1 * turn * tmpX * sin(90 * PI / 180) + normY * cos(90 * PI / 180);

    normX = fabs(ad_radius) * normX;
    normY = fabs(ad_radius) * normY;

    *ad_x += normX;
    *ad_y += normY;
}


void
NIImporter_OpenDrive::calcPointOnCurve(SUMOReal* ad_x, SUMOReal* ad_y, SUMOReal ad_centerX, SUMOReal ad_centerY,
                                       SUMOReal ad_r, SUMOReal ad_length) {
    double rotAngle = ad_length / fabs(ad_r);
    double vx = *ad_x - ad_centerX;
    double vy = *ad_y - ad_centerY;
    double tmpx;

    double turn;
    if (ad_r > 0) {
        turn = -1;    //left
    } else {
        turn = 1;    //right
    }
    tmpx = vx;
    vx = vx * cos(rotAngle) + turn * vy * sin(rotAngle);
    vy = -1 * turn * tmpx * sin(rotAngle) + vy * cos(rotAngle);
    *ad_x = vx + ad_centerX;
    *ad_y = vy + ad_centerY;
}

// ---------------------------------------------------------------------------
// loader methods
// ---------------------------------------------------------------------------
NIImporter_OpenDrive::NIImporter_OpenDrive(
    std::vector<OpenDriveEdge> &innerEdges,
    std::vector<OpenDriveEdge> &outerEdges)
    : GenericSAXHandler(openDriveTags, OPENDRIVE_TAG_NOTHING, openDriveAttrs, OPENDRIVE_ATTR_NOTHING, "opendrive"),
      myCurrentEdge("", "", -1), myInnerEdges(innerEdges), myOuterEdges(outerEdges) {
}


NIImporter_OpenDrive::~NIImporter_OpenDrive() {
}


void
NIImporter_OpenDrive::myStartElement(int element,
                                     const SUMOSAXAttributes& attrs) {
    bool ok = true;
    switch (element) {
        case OPENDRIVE_TAG_HEADER: {
            int majorVersion = attrs.getIntReporting(OPENDRIVE_ATTR_REVMAJOR, 0, ok);
            int minorVersion = attrs.getIntReporting(OPENDRIVE_ATTR_REVMINOR, 0, ok);
            if (majorVersion != 1 || minorVersion != 2) {
                WRITE_WARNING("Given openDrive file '" + getFileName() + "' uses version " + toString(majorVersion) + "." + toString(minorVersion) + ";\n Version 1.2 is supported.");
            }
        }
        break;
        case OPENDRIVE_TAG_ROAD: {
            std::string id = attrs.getStringReporting(OPENDRIVE_ATTR_ID, 0, ok);
            std::string junction = attrs.getStringReporting(OPENDRIVE_ATTR_JUNCTION, id.c_str(), ok);
            SUMOReal length = attrs.getSUMORealReporting(OPENDRIVE_ATTR_LENGTH, id.c_str(), ok);
            myCurrentEdge = OpenDriveEdge(id, junction, length);
        }
        break;
        case OPENDRIVE_TAG_PREDECESSOR: {
            if (myElementStack.size() >= 2 && myElementStack[myElementStack.size() - 2] == OPENDRIVE_TAG_ROAD) {
                std::string elementType = attrs.getStringReporting(OPENDRIVE_ATTR_ELEMENTTYPE, myCurrentEdge.id.c_str(), ok);
                std::string elementID = attrs.getStringReporting(OPENDRIVE_ATTR_ELEMENTID, myCurrentEdge.id.c_str(), ok);
                std::string contactPoint = attrs.hasAttribute(OPENDRIVE_ATTR_CONTACTPOINT)
                                           ? attrs.getStringReporting(OPENDRIVE_ATTR_CONTACTPOINT, myCurrentEdge.id.c_str(), ok)
                                           : "end";
                addLink(OPENDRIVE_LT_PREDECESSOR, elementType, elementID, contactPoint);
            }
            if (myElementStack.size() >= 2 && myElementStack[myElementStack.size() - 2] == OPENDRIVE_TAG_LANE) {
                int no = attrs.getIntReporting(OPENDRIVE_ATTR_ID, myCurrentEdge.id.c_str(), ok);
                OpenDriveLane& l = myCurrentEdge.laneSections[myCurrentEdge.laneSections.size() - 1].lanesByDir[myCurrentLaneDirection].back();
                l.predecessor = no;
            }
        }
        break;
        case OPENDRIVE_TAG_SUCCESSOR: {
            if (myElementStack.size() >= 2 && myElementStack[myElementStack.size() - 2] == OPENDRIVE_TAG_ROAD) {
                std::string elementType = attrs.getStringReporting(OPENDRIVE_ATTR_ELEMENTTYPE, myCurrentEdge.id.c_str(), ok);
                std::string elementID = attrs.getStringReporting(OPENDRIVE_ATTR_ELEMENTID, myCurrentEdge.id.c_str(), ok);
                std::string contactPoint = attrs.hasAttribute(OPENDRIVE_ATTR_CONTACTPOINT)
                                           ? attrs.getStringReporting(OPENDRIVE_ATTR_CONTACTPOINT, myCurrentEdge.id.c_str(), ok)
                                           : "start";
                addLink(OPENDRIVE_LT_SUCCESSOR, elementType, elementID, contactPoint);
            }
            if (myElementStack.size() >= 2 && myElementStack[myElementStack.size() - 2] == OPENDRIVE_TAG_LANE) {
                int no = attrs.getIntReporting(OPENDRIVE_ATTR_ID, myCurrentEdge.id.c_str(), ok);
                OpenDriveLane& l = myCurrentEdge.laneSections[myCurrentEdge.laneSections.size() - 1].lanesByDir[myCurrentLaneDirection].back();
                l.successor = no;
            }
        }
        break;
        case OPENDRIVE_TAG_GEOMETRY: {
            SUMOReal length = attrs.getSUMORealReporting(OPENDRIVE_ATTR_LENGTH, myCurrentEdge.id.c_str(), ok);
            SUMOReal s = attrs.getSUMORealReporting(OPENDRIVE_ATTR_S, myCurrentEdge.id.c_str(), ok);
            SUMOReal x = attrs.getSUMORealReporting(OPENDRIVE_ATTR_X, myCurrentEdge.id.c_str(), ok);
            SUMOReal y = attrs.getSUMORealReporting(OPENDRIVE_ATTR_Y, myCurrentEdge.id.c_str(), ok);
            SUMOReal hdg = attrs.getSUMORealReporting(OPENDRIVE_ATTR_HDG, myCurrentEdge.id.c_str(), ok);
            myCurrentEdge.geometries.push_back(OpenDriveGeometry(length, s, x, y, hdg));
        }
        break;
        case OPENDRIVE_TAG_LINE: {
            std::vector<SUMOReal> vals;
            addGeometryShape(OPENDRIVE_GT_LINE, vals);
        }
        break;
        case OPENDRIVE_TAG_SPIRAL: {
            std::vector<SUMOReal> vals;
            vals.push_back(attrs.getSUMORealReporting(OPENDRIVE_ATTR_CURVSTART, myCurrentEdge.id.c_str(), ok));
            vals.push_back(attrs.getSUMORealReporting(OPENDRIVE_ATTR_CURVEND, myCurrentEdge.id.c_str(), ok));
            addGeometryShape(OPENDRIVE_GT_SPIRAL, vals);
        }
        break;
        case OPENDRIVE_TAG_ARC: {
            std::vector<SUMOReal> vals;
            vals.push_back(attrs.getSUMORealReporting(OPENDRIVE_ATTR_CURVATURE, myCurrentEdge.id.c_str(), ok));
            addGeometryShape(OPENDRIVE_GT_ARC, vals);
        }
        break;
        case OPENDRIVE_TAG_POLY3: {
            std::vector<SUMOReal> vals;
            vals.push_back(attrs.getSUMORealReporting(OPENDRIVE_ATTR_A, myCurrentEdge.id.c_str(), ok));
            vals.push_back(attrs.getSUMORealReporting(OPENDRIVE_ATTR_B, myCurrentEdge.id.c_str(), ok));
            vals.push_back(attrs.getSUMORealReporting(OPENDRIVE_ATTR_C, myCurrentEdge.id.c_str(), ok));
            vals.push_back(attrs.getSUMORealReporting(OPENDRIVE_ATTR_D, myCurrentEdge.id.c_str(), ok));
            addGeometryShape(OPENDRIVE_GT_POLY3, vals);
        }
        break;
        case OPENDRIVE_TAG_LANESECTION: {
            SUMOReal s = attrs.getSUMORealReporting(OPENDRIVE_ATTR_S, myCurrentEdge.id.c_str(), ok);
            myCurrentEdge.laneSections.push_back(OpenDriveLaneSection(s));
        }
        break;
        case OPENDRIVE_TAG_LEFT:
            myCurrentLaneDirection = OPENDRIVE_TAG_LEFT;
            break;
        case OPENDRIVE_TAG_CENTER:
            myCurrentLaneDirection = OPENDRIVE_TAG_CENTER;
            break;
        case OPENDRIVE_TAG_RIGHT:
            myCurrentLaneDirection = OPENDRIVE_TAG_RIGHT;
            break;
        case OPENDRIVE_TAG_LANE: {
            std::string type = attrs.getStringReporting(OPENDRIVE_ATTR_TYPE, myCurrentEdge.id.c_str(), ok);
            int id = attrs.getIntReporting(OPENDRIVE_ATTR_ID, myCurrentEdge.id.c_str(), ok);
            std::string level = attrs.hasAttribute(OPENDRIVE_ATTR_LEVEL)
                                ? attrs.getStringReporting(OPENDRIVE_ATTR_LEVEL, myCurrentEdge.id.c_str(), ok)
                                : "";
            OpenDriveLaneSection& ls = myCurrentEdge.laneSections[myCurrentEdge.laneSections.size() - 1];
            ls.lanesByDir[myCurrentLaneDirection].push_back(OpenDriveLane(id, level, type));
        }
        break;
        default:
            break;
    }
    myElementStack.push_back(element);
}


void
NIImporter_OpenDrive::myCharacters(int element,
                                   const std::string& chars) {
    UNUSED_PARAMETER(element);
    UNUSED_PARAMETER(chars);
}



void
NIImporter_OpenDrive::myEndElement(int element) {
    myElementStack.pop_back();
    switch (element) {
        case OPENDRIVE_TAG_ROAD:
            if (myCurrentEdge.junction == "" || myCurrentEdge.junction == "-1") {
                myOuterEdges.push_back(myCurrentEdge);
            } else {
                myInnerEdges.push_back(myCurrentEdge);
            }
            break;
        default:
            break;
    }
}



void
NIImporter_OpenDrive::addLink(LinkType lt, const std::string& elementType,
                              const std::string& elementID,
                              const std::string& contactPoint) {
    OpenDriveLink l(lt, elementID);
    // elementType
    if (elementType == "road") {
        l.elementType = OPENDRIVE_ET_ROAD;
    } else if (elementType == "junction") {
        l.elementType = OPENDRIVE_ET_JUNCTION;
    }
    // contact point
    if (contactPoint == "start") {
        l.contactPoint = OPENDRIVE_CP_START;
    } else if (contactPoint == "end") {
        l.contactPoint = OPENDRIVE_CP_END;
    }
    // add
    myCurrentEdge.links.push_back(l);
}


void
NIImporter_OpenDrive::addGeometryShape(GeometryType type, const std::vector<SUMOReal> &vals) {
    // checks
    if (myCurrentEdge.geometries.size() == 0) {
        throw ProcessError("Mismatching paranthesis in geometry definition for road '" + myCurrentEdge.id + "'");
    }
    OpenDriveGeometry& last = myCurrentEdge.geometries[myCurrentEdge.geometries.size() - 1];
    if (last.type != OPENDRIVE_GT_UNKNOWN) {
        throw ProcessError("Double geometry information for road '" + myCurrentEdge.id + "'");
    }
    // set
    last.type = type;
    last.params = vals;
}


/****************************************************************************/

