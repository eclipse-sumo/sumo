/****************************************************************************/
/// @file    NIImporter_OpenDrive.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 14.04.2008
/// @version $Id$
///
// Importer for networks stored in openDrive format
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
#include "NIImporter_OpenDrive.h"
#include <utils/geom/GeoConvHelper.h>
#include <utils/geom/GeomConvHelper.h>
#include <foreign/eulerspiral/euler.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/FileHelpers.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/geom/Boundary.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// definitions
// ===========================================================================
#define C_LENGTH 10.


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static methods (interface in this case)
// ---------------------------------------------------------------------------
void
NIImporter_OpenDrive::loadNetwork(const OptionsCont &oc, NBNetBuilder &nb) {
    // check whether the option is set (properly)
    if (!oc.isUsableFileList("opendrive")) {
        return;
    }
    // build the handler
    std::vector<OpenDriveEdge> innerEdges, outerEdges;
    NIImporter_OpenDrive handler(nb.getNodeCont(), innerEdges, outerEdges);
    // parse file(s)
    std::vector<std::string> files = oc.getStringVector("opendrive");
    for (std::vector<std::string>::const_iterator file=files.begin(); file!=files.end(); ++file) {
        if (!FileHelpers::exists(*file)) {
            MsgHandler::getErrorInstance()->inform("Could not open opendrive file '" + *file + "'.");
            return;
        }
        handler.setFileName(*file);
        MsgHandler::getMessageInstance()->beginProcessMsg("Parsing opendrive from '" + *file + "'...");
        XMLSubSys::runParser(handler, *file);
        MsgHandler::getMessageInstance()->endProcessMsg("done.");
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
    for (std::vector<OpenDriveEdge>::iterator i=innerEdges.begin(); i!=innerEdges.end(); ++i) {
        OpenDriveEdge &e = *i;
        assert(e.junction!="-1" && e.junction!="");
        edge2junction[e.id] = e.junction;
        if (posMap.find(e.junction)==posMap.end()) {
            posMap[e.junction] = Boundary();
        }
        posMap[e.junction].add(e.geom.getBoxBoundary());
    }
    //   build nodes
    for (std::map<std::string, Boundary>::iterator i=posMap.begin(); i!=posMap.end(); ++i) {
        if (!nb.getNodeCont().insert((*i).first, (*i).second.getCenter())) {
            throw ProcessError("Could not add node '" + (*i).first + "'.");
        }
    }
    //  assign built nodes
    for (std::vector<OpenDriveEdge>::iterator i=outerEdges.begin(); i!=outerEdges.end(); ++i) {
        OpenDriveEdge &e = *i;
        for (std::vector<OpenDriveLink>::iterator j=e.links.begin(); j!=e.links.end(); ++j) {
            OpenDriveLink &l = *j;
            if (l.elementType!=OPENDRIVE_ET_ROAD) {
                // set node information
                setNodeSecure(nb.getNodeCont(), e, l.elementID, l.linkType);
                continue;
            }
            if (edge2junction.find(l.elementID)!=edge2junction.end()) {
                // set node information of an internal road
                setNodeSecure(nb.getNodeCont(), e, edge2junction[l.elementID], l.linkType);
                continue;
            }
        }
    }
    //  we should now have all nodes set for links which are not outer edge-to-outer edge links


    // build nodes#2
    //  build nodes for all outer edge-to-outer edge connections
    for (std::vector<OpenDriveEdge>::iterator i=outerEdges.begin(); i!=outerEdges.end(); ++i) {
        OpenDriveEdge &e = *i;
        for (std::vector<OpenDriveLink>::iterator j=e.links.begin(); j!=e.links.end(); ++j) {
            OpenDriveLink &l = *j;
            if (l.elementType!=OPENDRIVE_ET_ROAD || edge2junction.find(l.elementID)!=edge2junction.end()) {
                // is a connection to an internal edge, or a node, skip
                continue;
            }
            // we have a direct connection between to external edges
            std::string id1 = e.id;
            std::string id2 = l.elementID;
            if (id1<id2) {
                std::swap(id1, id2);
            }
            std::string nid = id1+"."+id2;
            if (nb.getNodeCont().retrieve(nid)==0) {
                // not yet seen, build
                Position2D pos = l.linkType==OPENDRIVE_LT_SUCCESSOR ? e.geom[(int)e.geom.size()-1] : e.geom[0];
                if (!nb.getNodeCont().insert(nid, pos)) {
                    throw ProcessError("Could not build node '" + nid + "'.");
                }
            }
            /* debug-stuff
            else {
                Position2D pos = l.linkType==OPENDRIVE_LT_SUCCESSOR ? e.geom[e.geom.size()-1] : e.geom[0];
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
    for (std::vector<OpenDriveEdge>::iterator i=outerEdges.begin(); i!=outerEdges.end(); ++i) {
        OpenDriveEdge &e = *i;
        if (e.to!=0&&e.from!=0) {
            continue;
        }
        for (std::vector<OpenDriveEdge>::iterator j=innerEdges.begin(); j!=innerEdges.end(); ++j) {
            OpenDriveEdge &ie = *j;
            for (std::vector<OpenDriveLink>::iterator k=ie.links.begin(); k!=ie.links.end(); ++k) {
                OpenDriveLink &il = *k;
                if (il.elementType!=OPENDRIVE_ET_ROAD || il.elementID!=e.id) {
                    // not conneted to the currently investigated outer edge
                    continue;
                }
                std::string nid = edge2junction[ie.id];
                if (il.contactPoint==OPENDRIVE_CP_START) {
                    setNodeSecure(nb.getNodeCont(), e, nid, OPENDRIVE_LT_PREDECESSOR);
                } else {
                    setNodeSecure(nb.getNodeCont(), e, nid, OPENDRIVE_LT_SUCCESSOR);
                }
            }
        }

    }
    //


    // build start/end nodes which were not defined previously
    for (std::vector<OpenDriveEdge>::iterator i=outerEdges.begin(); i!=outerEdges.end(); ++i) {
        OpenDriveEdge &e = *i;
        if (e.from==0) {
            std::string nid = e.id + ".begin";
            Position2D pos(e.geometries[0].x, e.geometries[0].y);
            e.from = getOrBuildNode(nid, e.geom[0], nb.getNodeCont());
        }
        if (e.to==0) {
            std::string nid = e.id + ".end";
            Position2D pos(e.geometries[e.geometries.size()-1].x, e.geometries[e.geometries.size()-1].y);
            e.to = getOrBuildNode(nid, e.geom[(int)e.geom.size()-1], nb.getNodeCont());
        }
    }


    // -------------------------
    // edge building
    // -------------------------
    std::map<NBEdge*, std::map<int, int> > fromLaneMap;
    std::map<NBEdge*, std::map<int, int> > toLaneMap;
    // build edges
    for (std::vector<OpenDriveEdge>::iterator i=outerEdges.begin(); i!=outerEdges.end(); ++i) {
        OpenDriveEdge &e = *i;
        SUMOReal speed = nb.getTypeCont().getDefaultSpeed();
        int priority = nb.getTypeCont().getDefaultPriority();
        unsigned int nolanes = e.getMaxLaneNumber(SUMO_TAG_OPENDRIVE_RIGHT);
        if (nolanes>0) {
            NBEdge *nbe = new NBEdge("-" + e.id, e.from, e.to, "", speed, nolanes, priority, e.geom, NBEdge::LANESPREAD_RIGHT, true);
            if (!nb.getEdgeCont().insert(nbe)) {
                throw ProcessError("Could not add edge '" + std::string("-") + e.id + "'.");
            }
            fromLaneMap[nbe] = e.laneSections.back().buildLaneMapping(SUMO_TAG_OPENDRIVE_RIGHT);
            toLaneMap[nbe] = e.laneSections[0].buildLaneMapping(SUMO_TAG_OPENDRIVE_RIGHT);
        }
        nolanes = e.getMaxLaneNumber(SUMO_TAG_OPENDRIVE_LEFT);
        if (nolanes>0) {
            NBEdge *nbe = new NBEdge(e.id, e.to, e.from, "", speed, nolanes, priority, e.geom.reverse(), NBEdge::LANESPREAD_RIGHT, true);
            if (!nb.getEdgeCont().insert(nbe)) {
                throw ProcessError("Could not add edge '" + e.id + "'.");
            }
            fromLaneMap[nbe] = e.laneSections[0].buildLaneMapping(SUMO_TAG_OPENDRIVE_LEFT);
            toLaneMap[nbe] = e.laneSections.back().buildLaneMapping(SUMO_TAG_OPENDRIVE_LEFT);
        }
    }


    // -------------------------
    // connections building
    // -------------------------
    std::vector<Connection> connections;
    // connections#1
    //  build connections between outer-edges
    for (std::vector<OpenDriveEdge>::iterator i=outerEdges.begin(); i!=outerEdges.end(); ++i) {
        OpenDriveEdge &e = *i;
        for (std::vector<OpenDriveLink>::iterator j=e.links.begin(); j!=e.links.end(); ++j) {
            OpenDriveLink &l = *j;
            if (l.elementType!=OPENDRIVE_ET_ROAD) {
                // we are not interested in connections to nodes
                continue;
            }
            if (edge2junction.find(l.elementID)!=edge2junction.end()) {
                // connection via an inner-road
                addViaConnectionSecure(nb.getEdgeCont(),
                                       nb.getNodeCont().retrieve(edge2junction[l.elementID]),
                                       e, l.linkType, l.elementID, connections);
            } else {
                // connection between two outer-edges; can be used directly
                std::vector<OpenDriveEdge>::iterator p = std::find_if(outerEdges.begin(), outerEdges.end(), edge_by_id_finder(l.elementID));
                if (p==outerEdges.end()) {
                    throw ProcessError("Could not find connection edge.");
                }
                std::string id1 = e.id;
                std::string id2 = (*p).id;
                if (id1<id2) {
                    std::swap(id1, id2);
                }
                std::string nid = id1+"."+id2;
                if (l.linkType==OPENDRIVE_LT_PREDECESSOR) {
                    addE2EConnectionsSecure(nb.getEdgeCont(), nb.getNodeCont().retrieve(nid), *p, e, connections);
                } else {
                    addE2EConnectionsSecure(nb.getEdgeCont(), nb.getNodeCont().retrieve(nid), e, *p, connections);
                }
            }
        }
    }

    /*
    for (std::vector<OpenDriveEdge>::iterator i=innerEdges.begin(); i!=innerEdges.end(); ++i) {
        OpenDriveEdge &e = *i;
        std::string pred, succ;
        ContactPoint predC, succC;
        for (std::vector<OpenDriveLink>::iterator j=e.links.begin(); j!=e.links.end(); ++j) {
            OpenDriveLink &l = *j;
            if (l.elementType!=OPENDRIVE_ET_ROAD) {
                // we are not interested in connections to nodes
                cout << "unsupported" << endl;
                continue;
            }
            if(edge2junction.find(l.elementID)!=edge2junction.end()) {
                // not supported
                cout << "unsupported" << endl;
                continue;
            }
            if(l.linkType==OPENDRIVE_LT_SUCCESSOR) {
                if(succ!="") {
                    cout << "double succ" << endl;
                }
                succ = l.elementID;
                succC = l.contactPoint;
            } else {
                if(pred!="") {
                    cout << "double pred" << endl;
                }
                pred = l.elementID;
                predC = l.contactPoint;
            }
        }

        if(e.getMaxLaneNumber(SUMO_TAG_OPENDRIVE_LEFT)!=0&&e.getMaxLaneNumber(SUMO_TAG_OPENDRIVE_RIGHT)!=0) {
            cout << "Both dirs given!" << endl;
        }

        bool isReversed = false;
        if(e.getMaxLaneNumber(SUMO_TAG_OPENDRIVE_LEFT)!=0) {
    //            std::swap(pred, succ);
            //std::swap(predC, succC);
            isReversed = true;
        }

        if(succ==""||pred=="") {
            cout << "Missing edge." << endl;
            continue; // yes, occurs
        }
        NBNode *n = nb.getNodeCont().retrieve(edge2junction[e.id]);
        std::vector<OpenDriveEdge>::iterator predEdge = std::find_if(outerEdges.begin(), outerEdges.end(), edge_by_id_finder(pred));
        if(predEdge==outerEdges.end()) {
            throw ProcessError("Could not find connection edge.");
        }
        std::vector<OpenDriveEdge>::iterator succEdge = std::find_if(outerEdges.begin(), outerEdges.end(), edge_by_id_finder(succ));
        if(succEdge==outerEdges.end()) {
            throw ProcessError("Could not find connection edge.");
        }
        NBEdge *fromEdge, *toEdge;
        if(!isReversed) {
            fromEdge = predC==OPENDRIVE_CP_END ? nb.getEdgeCont().retrieve("-" + pred) : nb.getEdgeCont().retrieve(pred);
            toEdge = succC==OPENDRIVE_CP_START ? nb.getEdgeCont().retrieve("-" + succ) : nb.getEdgeCont().retrieve(succ);
        } else {
            fromEdge = predC!=OPENDRIVE_CP_END ? nb.getEdgeCont().retrieve("-" + pred) : nb.getEdgeCont().retrieve(pred);
            toEdge = succC!=OPENDRIVE_CP_START ? nb.getEdgeCont().retrieve("-" + succ) : nb.getEdgeCont().retrieve(succ);
        }
        /*
        Connection c(
            n->hasIncoming(nb.getEdgeCont().retrieve("-" + pred)) ? nb.getEdgeCont().retrieve("-" + pred) : nb.getEdgeCont().retrieve(pred),
            e.id,
            n->hasOutgoing(nb.getEdgeCont().retrieve("-" + succ)) ? nb.getEdgeCont().retrieve("-" + succ) : nb.getEdgeCont().retrieve(succ));
            /
        Connection c(fromEdge, e.id, toEdge);
        if(c.from==0||c.to==0||c.from==c.to) {
            throw ProcessError("Something's false");
        }
        setLaneConnections(c,
            *predEdge, c.from->getID()[0]!='-', c.from->getID()[0]=='-' ? SUMO_TAG_OPENDRIVE_RIGHT : SUMO_TAG_OPENDRIVE_LEFT,
            e, isReversed, !isReversed ? SUMO_TAG_OPENDRIVE_RIGHT : SUMO_TAG_OPENDRIVE_LEFT,
            *succEdge, c.to->getID()[0]!='-', c.to->getID()[0]=='-' ? SUMO_TAG_OPENDRIVE_RIGHT : SUMO_TAG_OPENDRIVE_LEFT);
        connections.push_back(c);
    }
    */
    for (std::vector<Connection>::const_iterator i=connections.begin(); i!=connections.end(); ++i) {
        if ((*i).from==0 || (*i).to==0) {
            std::cout << "Nope." << std::endl;
            continue;
        }
        (*i).from->addEdge2EdgeConnection((*i).to);
        std::map<int, int> fromMap = fromLaneMap[(*i).from];
        std::map<int, int> toMap = fromLaneMap[(*i).to];
        for (std::vector<std::pair<int, int> >::const_iterator j=(*i).lanes.begin(); j!=(*i).lanes.end(); ++j) {
            int fromLane = fromMap[(*j).first];
            int toLane = toMap[(*j).second];
            if (static_cast<unsigned int>(fromLane)>=(*i).from->getNoLanes()||fromLane<0) {
                std::cout << "False " << std::endl;
            }
            if (static_cast<unsigned int>(toLane)>=(*i).to->getNoLanes()||toLane<0) {
                std::cout << "False " << std::endl;
            }

            (*i).from->addLane2LaneConnection(fromLane, (*i).to, toLane, NBEdge::L2L_VALIDATED, true);
        }
    }
}


void
NIImporter_OpenDrive::addViaConnectionSecure(const NBEdgeCont &ec,
        const NBNode * const node, const OpenDriveEdge &e,
        LinkType lt, const std::string &via,
        std::vector<NIImporter_OpenDrive::Connection> &connections) {
    NBEdge *from = 0;
    NBEdge *to = 0;
    if (node==e.to) {
        // the connection is at the end of the "positive" direction
        if (lt==OPENDRIVE_LT_PREDECESSOR) {
            // via -> edge
            to = ec.retrieve(e.id);
        } else {
            // -edge -> via
            //  "ambigous?"
            from = ec.retrieve("-" + e.id);
        }
    } else {
        // the connection is at the begin of the "positive" direction
        if (lt==OPENDRIVE_LT_PREDECESSOR) {
            // via -> -edge
            to = ec.retrieve("-" + e.id);
        } else {
            // edge -> via
            //  "ambigous?"
            from = ec.retrieve(e.id);
        }
    }
    if (from==0&&to==0) {
        throw ProcessError("Missing edge");
    }
    Connection c(from, via, to);
    connections.push_back(c);
}


void
NIImporter_OpenDrive::addE2EConnectionsSecure(const NBEdgeCont &ec, const NBNode * const node,
        const NIImporter_OpenDrive::OpenDriveEdge &from, const NIImporter_OpenDrive::OpenDriveEdge &to,
        std::vector<NIImporter_OpenDrive::Connection> &connections) {
    // positive direction (from is incoming, to is outgoing)
    NBEdge *fromEdge = ec.retrieve("-" + from.id);
    if (fromEdge==0||!node->hasIncoming(fromEdge)) {
        fromEdge = ec.retrieve(from.id);
    }
    NBEdge *toEdge = ec.retrieve("-" + to.id);
    if (toEdge==0||!node->hasOutgoing(toEdge)) {
        toEdge = ec.retrieve(to.id);
    }
    if (fromEdge!=0&&toEdge!=0) {
        Connection c(fromEdge, "", toEdge);
        setLaneConnections(c,
                           from, c.from->getID()[0]!='-', c.from->getID()[0]=='-' ? SUMO_TAG_OPENDRIVE_RIGHT : SUMO_TAG_OPENDRIVE_LEFT,
                           to, c.to->getID()[0]!='-', c.to->getID()[0]=='-' ? SUMO_TAG_OPENDRIVE_RIGHT : SUMO_TAG_OPENDRIVE_LEFT);
        connections.push_back(c);
    }
    // negative direction (to is incoming, from is outgoing)
    fromEdge = ec.retrieve("-" + from.id);
    if (fromEdge==0||!node->hasOutgoing(fromEdge)) {
        fromEdge = ec.retrieve(from.id);
    }
    toEdge = ec.retrieve("-" + to.id);
    if (toEdge==0||!node->hasIncoming(toEdge)) {
        toEdge = ec.retrieve(to.id);
    }
    if (fromEdge!=0&&toEdge!=0) {
        Connection c(toEdge, "", fromEdge);
        setLaneConnections(c,
                           to, c.to->getID()[0]!='-', c.to->getID()[0]=='-' ? SUMO_TAG_OPENDRIVE_RIGHT : SUMO_TAG_OPENDRIVE_LEFT,
                           from, c.from->getID()[0]!='-', c.from->getID()[0]=='-' ? SUMO_TAG_OPENDRIVE_RIGHT : SUMO_TAG_OPENDRIVE_LEFT);
        connections.push_back(c);
    }
}


void
NIImporter_OpenDrive::setLaneConnections(NIImporter_OpenDrive::Connection &c,
        const OpenDriveEdge &from, bool fromAtBegin, SumoXMLTag fromLaneDir,
        const OpenDriveEdge &to, bool toAtEnd, SumoXMLTag toLaneDir) {
    const OpenDriveLaneSection &fromLS = fromAtBegin ? from.laneSections[0] : from.laneSections.back();
    const std::vector<OpenDriveLane> &fromLanes = fromLS.lanesByDir.find(fromLaneDir)->second;
    const OpenDriveLaneSection &toLS = toAtEnd ? to.laneSections.back() : to.laneSections[0];
    const std::vector<OpenDriveLane> &toLanes = toLS.lanesByDir.find(toLaneDir)->second;
    // in the following, we are probably using the same information twice, stored once
    //  in the from-edge's successor field and in the to-edge's precessor field.
    // though, we have no proof or information that this is always redundant
    for (std::vector<OpenDriveLane>::const_iterator i=fromLanes.begin(); i!=fromLanes.end(); ++i) {
        if ((*i).type!="driving") {
            continue;
        }
        if (!fromAtBegin && (*i).successor!=UNSET_CONNECTION) {
            c.lanes.push_back(std::make_pair((*i).id, (*i).successor));
        }
        if (fromAtBegin && (*i).predecessor!=UNSET_CONNECTION) {
            c.lanes.push_back(std::make_pair((*i).id, (*i).predecessor));
        }
    }
    for (std::vector<OpenDriveLane>::const_iterator i=toLanes.begin(); i!=toLanes.end(); ++i) {
        if ((*i).type!="driving") {
            continue;
        }
        if (!toAtEnd && (*i).predecessor!=UNSET_CONNECTION) {
            c.lanes.push_back(std::make_pair((*i).predecessor, (*i).id));
        }
        if (toAtEnd && (*i).successor!=UNSET_CONNECTION) {
            c.lanes.push_back(std::make_pair((*i).successor, (*i).id));
        }
    }
}


void
NIImporter_OpenDrive::setLaneConnections(NIImporter_OpenDrive::Connection &c,
        const OpenDriveEdge &from, bool fromAtBegin, SumoXMLTag fromLaneDir,
        const OpenDriveEdge &via, bool viaIsReversed, SumoXMLTag viaLaneDir,
        const OpenDriveEdge &to, bool toAtEnd, SumoXMLTag toLaneDir) {
    Connection from2via(0, "", 0);
    setLaneConnections(from2via, from, fromAtBegin, fromLaneDir, via, viaIsReversed, viaLaneDir);
    Connection via2to(0, "", 0);
    setLaneConnections(via2to, via, viaIsReversed, viaLaneDir, to, toAtEnd, toLaneDir);
    for (std::vector<std::pair<int, int> >::const_iterator i=from2via.lanes.begin(); i!=from2via.lanes.end(); ++i) {
        int fromLane = (*i).first;
        int viaLane = (*i).second;
        for (std::vector<std::pair<int, int> >::const_iterator j=via2to.lanes.begin(); j!=via2to.lanes.end(); ++j) {
            if ((*j).first==viaLane) {
                c.lanes.push_back(std::make_pair(fromLane, (*j).second));
                break;
            }
        }
    }
}


NBNode *
NIImporter_OpenDrive::getOrBuildNode(const std::string &id, Position2D &pos,
                                     NBNodeCont &nc) throw(ProcessError) {
    if (nc.retrieve(id)==0) {
        // not yet built; build now
        if (!nc.insert(id, pos)) {
            // !!! clean up
            throw ProcessError("Could not add node '" + id + "'.");
        }
    }
    return nc.retrieve(id);
}


void
NIImporter_OpenDrive::setNodeSecure(NBNodeCont &nc, OpenDriveEdge &e,
                                    const std::string &nodeID, NIImporter_OpenDrive::LinkType lt) throw(ProcessError) {
    NBNode *n = nc.retrieve(nodeID);
    if (n==0) {
        throw ProcessError("Could not find node '" + nodeID + "'.");
    }
    if (lt==OPENDRIVE_LT_SUCCESSOR) {
        if (e.to!=0&&e.to!=n) {
            throw ProcessError("Edge '" + e.id + "' has two end nodes.");
        }
        e.to = n;
    } else {
        if (e.from!=0&&e.from!=n) {
            throw ProcessError("Edge '" + e.id + "' has two start nodes.");
        }
        e.from = n;
    }
}





NBEdge *
NIImporter_OpenDrive::getOutgoingDirectionalEdge(const NBEdgeCont &ec,
        const NBNodeCont &nc,
        const std::string &edgeID,
        const std::string &nodeID) throw() {
    NBNode *node = nc.retrieve(nodeID);
    NBEdge *e = ec.retrieve(edgeID);
    if (e!=0 && node->hasOutgoing(e)) {
        return e;
    }
    e = ec.retrieve("-" + edgeID);
    if (e!=0 && node->hasOutgoing(e)) {
        return e;
    }
    return 0;
}


NBEdge *
NIImporter_OpenDrive::getIncomingDirectionalEdge(const NBEdgeCont &ec,
        const NBNodeCont &nc,
        const std::string &edgeID,
        const std::string &nodeID) throw() {
    NBNode *node = nc.retrieve(nodeID);
    NBEdge *e = ec.retrieve(edgeID);
    if (e!=0 && node->hasIncoming(e)) {
        return e;
    }
    e = ec.retrieve("-" + edgeID);
    if (e!=0 && node->hasIncoming(e)) {
        return e;
    }
    return 0;
}


void
NIImporter_OpenDrive::computeShapes(std::vector<OpenDriveEdge> &edges) throw() {
    for (std::vector<OpenDriveEdge>::iterator i=edges.begin(); i!=edges.end(); ++i) {
        OpenDriveEdge &e = *i;
        for (std::vector<OpenDriveGeometry>::iterator j=e.geometries.begin(); j!=e.geometries.end(); ++j) {
            OpenDriveGeometry &g = *j;
            std::vector<Position2D> geom;
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
            for (std::vector<Position2D>::iterator k=geom.begin(); k!=geom.end(); ++k) {
                e.geom.push_back_noDoublePos(*k);
            }
        }
        for (unsigned int j=0; j<e.geom.size(); ++j) {
            if (!GeoConvHelper::x2cartesian(e.geom[j])) {
                MsgHandler::getErrorInstance()->inform("Unable to project coordinates for.");
            }
        }
    }
}

std::vector<Position2D>
NIImporter_OpenDrive::geomFromLine(const OpenDriveEdge &e, const OpenDriveGeometry &g) throw() {
	UNUSED_PARAMETER(e);
    std::vector<Position2D> ret;
    ret.push_back(Position2D(g.x, g.y));
    ret.push_back(calculateStraightEndPoint(g.hdg, g.length, Position2D(g.x, g.y)));
    return ret;
}


std::vector<Position2D>
NIImporter_OpenDrive::geomFromSpiral(const OpenDriveEdge &e, const OpenDriveGeometry &g) throw() {
	UNUSED_PARAMETER(e);
    std::vector<Position2D> ret;
    SUMOReal curveStart = g.params[0];
    SUMOReal curveEnd = g.params[1];
    Point2D<double> end;
    EulerSpiral s(Point2D<double>(g.x, g.y), g.hdg, curveStart, (curveEnd-curveStart)/g.length, g.length);
    std::vector<Point2D<double> > into;
    s.computeSpiral(into, 1.);
    for (std::vector<Point2D<double> >::iterator i=into.begin(); i!=into.end(); ++i) {
        ret.push_back(Position2D((*i).getX(), (*i).getY()));
    }
    return ret;
}


std::vector<Position2D>
NIImporter_OpenDrive::geomFromArc(const OpenDriveEdge &e, const OpenDriveGeometry &g) throw() {
	UNUSED_PARAMETER(e);
    std::vector<Position2D> ret;
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
    int index1 = 0;
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
            hdgE = g.hdg + dist/fabs(radius);
        } else {
            hdgE = g.hdg - dist/fabs(radius);
        }
        //
        ret.push_back(Position2D(startX, startY));
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


std::vector<Position2D>
NIImporter_OpenDrive::geomFromPoly(const OpenDriveEdge &e, const OpenDriveGeometry &g) throw() {
	UNUSED_PARAMETER(g);
	UNUSED_PARAMETER(e);
    std::vector<Position2D> ret;
    return ret;
}


Position2D
NIImporter_OpenDrive::calculateStraightEndPoint(double hdg, double length, const Position2D &start) throw() {
    double normx = 1.0f;
    double normy = 0.0f;
    double x2 = normx * cos(hdg) - normy * sin(hdg);
    double y2 = normx * sin(hdg) + normy * cos(hdg);
    normx = x2 * length;
    normy = y2 * length;
    return Position2D(start.x() + normx, start.y() + normy);
}


void
NIImporter_OpenDrive::calculateCurveCenter(SUMOReal *ad_x, SUMOReal *ad_y, SUMOReal ad_radius, SUMOReal ad_hdg) throw() {
    SUMOReal normX = 1.0;
    SUMOReal normY = 0.0;
    SUMOReal tmpX;
    SUMOReal turn;
    if (ad_radius > 0)
        turn = -1.0;
    else
        turn = 1.0;

    tmpX = normX;
    normX = normX * cos(ad_hdg) + normY * sin(ad_hdg);
    normY = tmpX * sin(ad_hdg) + normY * cos(ad_hdg);

    tmpX = normX;
    normX = normX * cos(90 * PI / 180) + turn * normY * sin(90 * PI / 180);
    normY = -1 * turn * tmpX * sin(90 * PI / 180) + normY * cos(90 * PI / 180);

    normX = abs(ad_radius) * normX;
    normY = abs(ad_radius) * normY;

    *ad_x += normX;
    *ad_y += normY;
}


void
NIImporter_OpenDrive::calcPointOnCurve(SUMOReal *ad_x, SUMOReal *ad_y, SUMOReal ad_centerX, SUMOReal ad_centerY,
                                       SUMOReal ad_r, SUMOReal ad_length) throw() {
    double rotAngle = ad_length/abs(ad_r);
    double vx = *ad_x - ad_centerX;
    double vy = *ad_y - ad_centerY;
    double tmpx;

    double turn;
    if (ad_r > 0)
        turn = -1; //left
    else
        turn = 1; //right
    tmpx = vx;
    vx = vx * cos(rotAngle) + turn * vy * sin(rotAngle);
    vy = -1 * turn * tmpx * sin(rotAngle) + vy * cos(rotAngle);
    *ad_x = vx + ad_centerX;
    *ad_y = vy + ad_centerY;
}

// ---------------------------------------------------------------------------
// loader methods
// ---------------------------------------------------------------------------
NIImporter_OpenDrive::NIImporter_OpenDrive(NBNodeCont &nc,
        std::vector<OpenDriveEdge> &innerEdges,
        std::vector<OpenDriveEdge> &outerEdges)
        : SUMOSAXHandler("opendrive"), myCurrentEdge("", "", -1),
        myInnerEdges(innerEdges), myOuterEdges(outerEdges) {
	UNUSED_PARAMETER(nc);
}


NIImporter_OpenDrive::~NIImporter_OpenDrive() throw() {
}


void
NIImporter_OpenDrive::myStartElement(SumoXMLTag element,
                                     const SUMOSAXAttributes &attrs) throw(ProcessError) {
    bool ok = true;
    switch (element) {
    case SUMO_TAG_OPENDRIVE_HEADER: {
        int majorVersion = attrs.getIntReporting(SUMO_ATTR_OPENDRIVE_REVMAJOR, "opendrive header", 0, ok);
        int minorVersion = attrs.getIntReporting(SUMO_ATTR_OPENDRIVE_REVMINOR, "opendrive header", 0, ok);
        if (majorVersion!=1||minorVersion!=2) {
            MsgHandler::getWarningInstance()->inform("Given openDrive file '" + getFileName() + "' uses version " + toString(majorVersion) + "." + toString(minorVersion) + ";\n Version 1.2 is supported.");
        }
    }
    break;
    case SUMO_TAG_OPENDRIVE_ROAD: {
        std::string id =
            attrs.hasAttribute(SUMO_ATTR_OPENDRIVE_ID)
            ? attrs.getStringReporting(SUMO_ATTR_OPENDRIVE_ID, "road", 0, ok)
            : attrs.getStringReporting(SUMO_ATTR_ID, "road", 0, ok);
        std::cout << "found edge '" << id << "'" << std::endl;
        std::string junction = attrs.getStringReporting(SUMO_ATTR_OPENDRIVE_JUNCTION, "road", id.c_str(), ok);
        SUMOReal length = attrs.getSUMORealReporting(SUMO_ATTR_OPENDRIVE_LENGTH, "road", id.c_str(), ok);
        myCurrentEdge = OpenDriveEdge(id, junction, length);
    }
    break;
    case SUMO_TAG_OPENDRIVE_PREDECESSOR: {
        if (myElementStack.size()>=2&&myElementStack[myElementStack.size()-2]==SUMO_TAG_OPENDRIVE_ROAD) {
            std::string elementType = attrs.getStringReporting(SUMO_ATTR_OPENDRIVE_ELEMENTTYPE, "predecessor", myCurrentEdge.id.c_str(), ok);
            std::string elementID = attrs.getStringReporting(SUMO_ATTR_OPENDRIVE_ELEMENTID, "predecessor", myCurrentEdge.id.c_str(), ok);
            std::string contactPoint = attrs.hasAttribute(SUMO_ATTR_OPENDRIVE_CONTACTPOINT)
                                       ? attrs.getStringReporting(SUMO_ATTR_OPENDRIVE_CONTACTPOINT, "predecessor", myCurrentEdge.id.c_str(), ok)
                                       : "end";
            addLink(OPENDRIVE_LT_PREDECESSOR, elementType, elementID, contactPoint);
        }
        if (myElementStack.size()>=2&&myElementStack[myElementStack.size()-2]==SUMO_TAG_OPENDRIVE_LANE||myElementStack[myElementStack.size()-2]==SUMO_TAG_LANE) { // !!!
            int no = attrs.getIntReporting(SUMO_ATTR_ID, "predecessor", myCurrentEdge.id.c_str(), ok);
            OpenDriveLane &l = myCurrentEdge.laneSections[myCurrentEdge.laneSections.size()-1].lanesByDir[myCurrentLaneDirection].back();
            l.predecessor = no;
        }
    }
    break;
    case SUMO_TAG_OPENDRIVE_SUCCESSOR: {
        if (myElementStack.size()>=2&&myElementStack[myElementStack.size()-2]==SUMO_TAG_OPENDRIVE_ROAD) {
            std::string elementType = attrs.getStringReporting(SUMO_ATTR_OPENDRIVE_ELEMENTTYPE, "successor", myCurrentEdge.id.c_str(), ok);
            std::string elementID = attrs.getStringReporting(SUMO_ATTR_OPENDRIVE_ELEMENTID, "successor", myCurrentEdge.id.c_str(), ok);
            std::string contactPoint = attrs.hasAttribute(SUMO_ATTR_OPENDRIVE_CONTACTPOINT)
                                       ? attrs.getStringReporting(SUMO_ATTR_OPENDRIVE_CONTACTPOINT, "successor", myCurrentEdge.id.c_str(), ok)
                                       : "start";
            addLink(OPENDRIVE_LT_SUCCESSOR, elementType, elementID, contactPoint);
        }
        if (myElementStack.size()>=2&&myElementStack[myElementStack.size()-2]==SUMO_TAG_OPENDRIVE_LANE||myElementStack[myElementStack.size()-2]==SUMO_TAG_LANE) { // !!!
            int no = attrs.getIntReporting(SUMO_ATTR_ID, "predecessor", myCurrentEdge.id.c_str(), ok);
            OpenDriveLane &l = myCurrentEdge.laneSections[myCurrentEdge.laneSections.size()-1].lanesByDir[myCurrentLaneDirection].back();
            l.successor = no;
        }
    }
    break;
    case SUMO_TAG_OPENDRIVE_GEOMETRY: {
        SUMOReal length = attrs.getSUMORealReporting(SUMO_ATTR_OPENDRIVE_LENGTH, "geometry", myCurrentEdge.id.c_str(), ok);
        SUMOReal s = attrs.getSUMORealReporting(SUMO_ATTR_OPENDRIVE_S, "geometry", myCurrentEdge.id.c_str(), ok);
        SUMOReal x = attrs.getSUMORealReporting(SUMO_ATTR_OPENDRIVE_X, "geometry", myCurrentEdge.id.c_str(), ok);
        SUMOReal y = attrs.getSUMORealReporting(SUMO_ATTR_OPENDRIVE_Y, "geometry", myCurrentEdge.id.c_str(), ok);
        SUMOReal hdg = attrs.getSUMORealReporting(SUMO_ATTR_OPENDRIVE_HDG, "geometry", myCurrentEdge.id.c_str(), ok);
        myCurrentEdge.geometries.push_back(OpenDriveGeometry(length, s, x, y, hdg));
    }
    break;
    case SUMO_TAG_OPENDRIVE_LINE: {
        std::vector<SUMOReal> vals;
        addGeometryShape(OPENDRIVE_GT_LINE, vals);
    }
    break;
    case SUMO_TAG_OPENDRIVE_SPIRAL: {
        std::vector<SUMOReal> vals;
        vals.push_back(attrs.getSUMORealReporting(SUMO_ATTR_OPENDRIVE_CURVSTART, "spiral", myCurrentEdge.id.c_str(), ok));
        vals.push_back(attrs.getSUMORealReporting(SUMO_ATTR_OPENDRIVE_CURVEND, "spiral", myCurrentEdge.id.c_str(), ok));
        addGeometryShape(OPENDRIVE_GT_SPIRAL, vals);
    }
    break;
    case SUMO_TAG_OPENDRIVE_ARC: {
        std::vector<SUMOReal> vals;
        vals.push_back(attrs.getSUMORealReporting(SUMO_ATTR_OPENDRIVE_CURVATURE, "arc", myCurrentEdge.id.c_str(), ok));
        addGeometryShape(OPENDRIVE_GT_ARC, vals);
    }
    break;
    case SUMO_TAG_OPENDRIVE_POLY3: {
        std::vector<SUMOReal> vals;
        vals.push_back(attrs.getSUMORealReporting(SUMO_ATTR_OPENDRIVE_A, "poly3", myCurrentEdge.id.c_str(), ok));
        vals.push_back(attrs.getSUMORealReporting(SUMO_ATTR_OPENDRIVE_B, "poly3", myCurrentEdge.id.c_str(), ok));
        vals.push_back(attrs.getSUMORealReporting(SUMO_ATTR_OPENDRIVE_C, "poly3", myCurrentEdge.id.c_str(), ok));
        vals.push_back(attrs.getSUMORealReporting(SUMO_ATTR_OPENDRIVE_D, "poly3", myCurrentEdge.id.c_str(), ok));
        addGeometryShape(OPENDRIVE_GT_POLY3, vals);
    }
    break;
    case SUMO_TAG_OPENDRIVE_LANESECTION: {
        SUMOReal s = attrs.getSUMORealReporting(SUMO_ATTR_OPENDRIVE_S, "geometry", myCurrentEdge.id.c_str(), ok);
        myCurrentEdge.laneSections.push_back(OpenDriveLaneSection(s));
    }
    break;
    case SUMO_TAG_OPENDRIVE_LEFT:
    case SUMO_TAG_OPENDRIVE_CENTER:
    case SUMO_TAG_OPENDRIVE_RIGHT:
        myCurrentLaneDirection = element;
        break;
    case SUMO_TAG_LANE: // !!!
    case SUMO_TAG_OPENDRIVE_LANE: {
        std::string type = attrs.getStringReporting(SUMO_ATTR_OPENDRIVE_TYPE, "lane", myCurrentEdge.id.c_str(), ok);
        int id = attrs.hasAttribute(SUMO_ATTR_OPENDRIVE_ID)
                 ? attrs.getIntReporting(SUMO_ATTR_OPENDRIVE_ID, "lane", myCurrentEdge.id.c_str(), ok)
                 : attrs.getIntReporting(SUMO_ATTR_ID, "lane", myCurrentEdge.id.c_str(), ok);
        int level = attrs.hasAttribute(SUMO_ATTR_OPENDRIVE_LEVEL)
                    ? attrs.getIntReporting(SUMO_ATTR_OPENDRIVE_LEVEL, "lane", myCurrentEdge.id.c_str(), ok)
                    : 0;
        OpenDriveLaneSection &ls = myCurrentEdge.laneSections[myCurrentEdge.laneSections.size()-1];
        ls.lanesByDir[myCurrentLaneDirection].push_back(OpenDriveLane(id, level, type));
    }
    default:
        break;
    }
    myElementStack.push_back(element);
}


void
NIImporter_OpenDrive::myCharacters(SumoXMLTag element,
                                   const std::string &chars) throw(ProcessError) {
	UNUSED_PARAMETER(element);
	UNUSED_PARAMETER(chars);
}



void
NIImporter_OpenDrive::myEndElement(SumoXMLTag element) throw(ProcessError) {
    myElementStack.pop_back();
    switch (element) {
    case SUMO_TAG_OPENDRIVE_ROAD:
        if (myCurrentEdge.junction=="" || myCurrentEdge.junction=="-1") {
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
NIImporter_OpenDrive::addLink(LinkType lt, const std::string &elementType,
                              const std::string &elementID,
                              const std::string &contactPoint) throw(ProcessError) {
    OpenDriveLink l(lt, elementID);
    // elementType
    if (elementType=="road") {
        l.elementType = OPENDRIVE_ET_ROAD;
    } else if (elementType=="junction") {
        l.elementType = OPENDRIVE_ET_JUNCTION;
    }
    // contact point
    if (contactPoint=="start") {
        l.contactPoint = OPENDRIVE_CP_START;
    } else if (contactPoint=="end") {
        l.contactPoint = OPENDRIVE_CP_END;
    }
    // add
    myCurrentEdge.links.push_back(l);
}


void
NIImporter_OpenDrive::addGeometryShape(GeometryType type, const std::vector<SUMOReal> &vals) throw(ProcessError) {
    // checks
    if (myCurrentEdge.geometries.size()==0) {
        throw ProcessError("Mismatching paranthesis in geometry definition for road '" + myCurrentEdge.id + "'");
    }
    OpenDriveGeometry &last = myCurrentEdge.geometries[myCurrentEdge.geometries.size()-1];
    if (last.type!=OPENDRIVE_GT_UNKNOWN) {
        throw ProcessError("Double geometry information for road '" + myCurrentEdge.id + "'");
    }
    // set
    last.type = type;
    last.params = vals;
}


/****************************************************************************/

