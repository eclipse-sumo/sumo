/****************************************************************************/
/// @file    NIImporter_OpenDrive.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 14.04.2008
/// @version $Id$
///
// Importer for networks stored in openDrive format
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
#include <utils/options/OptionsCont.h>
#include <utils/common/FileHelpers.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/geom/Boundary.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;



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
    vector<string> files = oc.getStringVector("opendrive");
    for (vector<string>::const_iterator file=files.begin(); file!=files.end(); ++file) {
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
        if(posMap.find(e.junction)==posMap.end()) {
            posMap[e.junction] = Boundary();
        }
        posMap[e.junction].add(e.geom.getBoxBoundary());
    }
    //   build nodes
    for(std::map<std::string, Boundary>::iterator i=posMap.begin(); i!=posMap.end(); ++i) {
        if(!nb.getNodeCont().insert((*i).first, (*i).second.getCenter())) {
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
            if(edge2junction.find(l.elementID)!=edge2junction.end()) {
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
            if(id1<id2) {
                std::swap(id1, id2);
            }
            std::string nid = id1+"."+id2;
            if(nb.getNodeCont().retrieve(nid)==0) {
                // not yet seen, build
                Position2D pos = l.linkType==OPENDRIVE_LT_SUCCESSOR ? e.geom[e.geom.size()-1] : e.geom[0];
                if(!nb.getNodeCont().insert(nid, pos)) {
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
        if(e.to!=0&&e.from!=0) {
            continue;
        }
        for (std::vector<OpenDriveEdge>::iterator j=innerEdges.begin(); j!=innerEdges.end(); ++j) {
            OpenDriveEdge &ie = *j;
            for (std::vector<OpenDriveLink>::iterator k=ie.links.begin(); k!=ie.links.end(); ++k) {
                OpenDriveLink &il = *k;
                if(il.elementType!=OPENDRIVE_ET_ROAD || il.elementID!=e.id) {
                    // not conneted to the currently investigated outer edge
                    continue;
                }
                std::string nid = edge2junction[ie.id];
                if(il.contactPoint==OPENDRIVE_CP_START) {
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
        if(e.from==0) {
            std::string nid = e.id + ".begin";
            Position2D pos(e.geometries[0].x, e.geometries[0].y);
            e.from = getOrBuildNode(nid, e.geom[0], nb.getNodeCont());
        }
        if(e.to==0) {
            std::string nid = e.id + ".end";
            Position2D pos(e.geometries[e.geometries.size()-1].x, e.geometries[e.geometries.size()-1].y);
            e.to = getOrBuildNode(nid, e.geom[e.geom.size()-1], nb.getNodeCont());
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
        if(nolanes>0) {
            NBEdge *nbe = new NBEdge("-" + e.id, e.from, e.to, "", speed, nolanes, priority, e.geom, NBEdge::LANESPREAD_RIGHT, true);
            if (!nb.getEdgeCont().insert(nbe)) {
                throw ProcessError("Could not add edge '" + std::string("-") + e.id + "'.");
            }
            fromLaneMap[nbe] = e.laneSections.back().buildLaneMapping(SUMO_TAG_OPENDRIVE_RIGHT);
            toLaneMap[nbe] = e.laneSections[0].buildLaneMapping(SUMO_TAG_OPENDRIVE_RIGHT);
        }
        nolanes = e.getMaxLaneNumber(SUMO_TAG_OPENDRIVE_LEFT);
        if(nolanes>0) {
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
            if(edge2junction.find(l.elementID)!=edge2junction.end()) {
                // connection via an inner-road
                /* !!!
                addViaConnectionSecure(nb.getEdgeCont(),
                    nb.getNodeCont().retrieve(edge2junction[l.elementID]),
                    e, l.linkType, l.elementID, connections);
                    */
            } else {
                // connection between two outer-edges; can be used directly
                std::vector<OpenDriveEdge>::iterator p = std::find_if(outerEdges.begin(), outerEdges.end(), edge_by_id_finder(l.elementID));
                if(p==outerEdges.end()) {
                    throw ProcessError("Could not find connection edge.");
                }
            std::string id1 = e.id;
            std::string id2 = (*p).id;
            if(id1<id2) {
                std::swap(id1, id2);
            }
            std::string nid = id1+"."+id2;
                if(l.linkType==OPENDRIVE_LT_PREDECESSOR) {
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
                if(e.id=="2217") {
                    int bla = 0;
                }

        if(e.getMaxLaneNumber(SUMO_TAG_OPENDRIVE_LEFT)!=0&&e.getMaxLaneNumber(SUMO_TAG_OPENDRIVE_RIGHT)!=0) {
            cout << "Both dirs given!" << endl;
        }

        if(e.id=="2209") {
            int bla = 0;
        }
        bool isReversed = false;
        if(e.getMaxLaneNumber(SUMO_TAG_OPENDRIVE_LEFT)!=0) {
//            std::swap(pred, succ);
            //std::swap(predC, succC);
            isReversed = true;
        }

        if(succ==""||pred=="") {
            cout << "Missing edge." << endl;
            continue; // yes, occures
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
                if(e.id=="2217") {
                    int bla = 0;
                }
        setLaneConnections(c, 
            *predEdge, c.from->getID()[0]!='-', c.from->getID()[0]=='-' ? SUMO_TAG_OPENDRIVE_RIGHT : SUMO_TAG_OPENDRIVE_LEFT, 
            e, isReversed, !isReversed ? SUMO_TAG_OPENDRIVE_RIGHT : SUMO_TAG_OPENDRIVE_LEFT, 
            *succEdge, c.to->getID()[0]!='-', c.to->getID()[0]=='-' ? SUMO_TAG_OPENDRIVE_RIGHT : SUMO_TAG_OPENDRIVE_LEFT);
        connections.push_back(c);
    }
*/
    for(std::vector<Connection>::const_iterator i=connections.begin(); i!=connections.end(); ++i) {
        if((*i).from==0 || (*i).to==0) {
            cout << "Nope." << endl;
            continue;
        }
        if((*i).from->getID()=="-2512" && (*i).to->getID()=="-2203") {
            int bla = 0;
        }

        (*i).from->addEdge2EdgeConnection((*i).to);

        map<int, int> fromMap = fromLaneMap[(*i).from];
        map<int, int> toMap = fromLaneMap[(*i).to];
        for(std::vector<std::pair<int, int> >::const_iterator j=(*i).lanes.begin(); j!=(*i).lanes.end(); ++j) {
            /*
            int fromLane = abs((*j).first);
            //fromLane = (*i).from->getNoLanes() - fromLane;
            int toLane = abs((*j).second);
            //toLane = (*i).to->getNoLanes() - toLane;
            */
            int fromLane = fromMap[(*j).first];
            int toLane = toMap[(*j).second];
            if(fromLane>=(*i).from->getNoLanes()||fromLane<0) {
                cout << "False " << endl;
            }
            if(toLane>=(*i).to->getNoLanes()||toLane<0) {
                cout << "False " << endl;
            }
       
            (*i).from->addLane2LaneConnection(fromLane, (*i).to, toLane, NBEdge::L2L_VALIDATED, true);
        }
    }
}


void 
NIImporter_OpenDrive::addViaConnectionSecure(const NBEdgeCont &ec,
                                             const NBNode * const node, const OpenDriveEdge &e, 
                                             LinkType lt, const std::string &via,
                                             std::vector<NIImporter_OpenDrive::Connection> &connections)
{
    if(e.id=="2277" && via=="2201") {
        int bla = 0;
    }
    NBEdge *from = 0;
    NBEdge *to = 0;
    if(node==e.to) {
        // the connection is at the end of the "positive" direction
        if(lt==OPENDRIVE_LT_PREDECESSOR) {
            // via -> edge
            to = ec.retrieve(e.id);
        } else {
            // -edge -> via
            //  "ambigous?"
            from = ec.retrieve("-" + e.id);
        }
    } else {
        // the connection is at the begin of the "positive" direction
        if(lt==OPENDRIVE_LT_PREDECESSOR) {
            // via -> -edge
            to = ec.retrieve("-" + e.id);
        } else {
            // edge -> via
            //  "ambigous?"
            from = ec.retrieve(e.id);
        }
    }
    if(from==0&&to==0) {
        throw ProcessError("Missing edge");
    }
    Connection c(from, via, to);
    connections.push_back(c);
}


void
NIImporter_OpenDrive::addE2EConnectionsSecure(const NBEdgeCont &ec, const NBNode * const node,
                                              const NIImporter_OpenDrive::OpenDriveEdge &from, const NIImporter_OpenDrive::OpenDriveEdge &to, 
                                              std::vector<NIImporter_OpenDrive::Connection> &connections)
{
    if(from.id=="2203"||to.id=="2203") {
        int bla = 0;
    }
    // positive direction (from is incoming, to is outgoing)
    NBEdge *fromEdge = ec.retrieve("-" + from.id);
    if(fromEdge==0||!node->hasIncoming(fromEdge)) {
        fromEdge = ec.retrieve(from.id);
    }
    NBEdge *toEdge = ec.retrieve("-" + to.id);
    if(toEdge==0||!node->hasOutgoing(toEdge)) {
        toEdge = ec.retrieve(to.id);
    }
    if(fromEdge!=0&&toEdge!=0) {
        Connection c(fromEdge, "", toEdge);
        setLaneConnections(c, 
            from, c.from->getID()[0]!='-', c.from->getID()[0]=='-' ? SUMO_TAG_OPENDRIVE_RIGHT : SUMO_TAG_OPENDRIVE_LEFT, 
            to, c.to->getID()[0]!='-', c.to->getID()[0]=='-' ? SUMO_TAG_OPENDRIVE_RIGHT : SUMO_TAG_OPENDRIVE_LEFT);
        connections.push_back(c);
    }
    // negative direction (to is incoming, from is outgoing)
    fromEdge = ec.retrieve("-" + from.id);
    if(fromEdge==0||!node->hasOutgoing(fromEdge)) {
        fromEdge = ec.retrieve(from.id);
    }
    toEdge = ec.retrieve("-" + to.id);
    if(toEdge==0||!node->hasIncoming(toEdge)) {
        toEdge = ec.retrieve(to.id);
    }
    if(fromEdge!=0&&toEdge!=0) {
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
                                         const OpenDriveEdge &to, bool toAtEnd, SumoXMLTag toLaneDir)
{
    if(from.id=="2203"||to.id=="2203") {
        int bla = 0;
    }
    if(c.from->getID()=="2115"||c.to->getID()=="2111") {
        int bla = 0;
    }
    const OpenDriveLaneSection &fromLS = fromAtBegin ? from.laneSections[0] : from.laneSections.back();
    const std::vector<OpenDriveLane> &fromLanes = fromLS.lanesByDir.find(fromLaneDir)->second;
    const OpenDriveLaneSection &toLS = toAtEnd ? to.laneSections.back() : to.laneSections[0];
    const std::vector<OpenDriveLane> &toLanes = toLS.lanesByDir.find(toLaneDir)->second;
    // in the following, we are probably using the same information twice, stored once
    //  in the from-edge's successor field and in the to-edge's precessor field.
    // though, we have no proof or information that this is always redundant
    for(std::vector<OpenDriveLane>::const_iterator i=fromLanes.begin(); i!=fromLanes.end(); ++i) {
        if((*i).type!="driving") {
            continue;
        }
        if(!fromAtBegin && (*i).successor!=UNSET_CONNECTION) {
            c.lanes.push_back(make_pair((*i).id, (*i).successor));
        }
        if(fromAtBegin && (*i).predecessor!=UNSET_CONNECTION) {
            c.lanes.push_back(make_pair((*i).id, (*i).predecessor));
        }
    }
    for(std::vector<OpenDriveLane>::const_iterator i=toLanes.begin(); i!=toLanes.end(); ++i) {
        if((*i).type!="driving") {
            continue;
        }
        if(!toAtEnd && (*i).predecessor!=UNSET_CONNECTION) {
            c.lanes.push_back(make_pair((*i).predecessor, (*i).id));
        }
        if(toAtEnd && (*i).successor!=UNSET_CONNECTION) {
            c.lanes.push_back(make_pair((*i).successor, (*i).id));
        }
    }
}


void
NIImporter_OpenDrive::setLaneConnections(NIImporter_OpenDrive::Connection &c, 
                                         const OpenDriveEdge &from, bool fromAtBegin, SumoXMLTag fromLaneDir,
                                         const OpenDriveEdge &via, bool viaIsReversed, SumoXMLTag viaLaneDir,
                                         const OpenDriveEdge &to, bool toAtEnd, SumoXMLTag toLaneDir)
{
    if(from.id=="2212"||to.id=="2212"||via.id=="2212") {
       int bla = 0;
    }
    Connection from2via(0, "", 0);
    setLaneConnections(from2via, from, fromAtBegin, fromLaneDir, via, viaIsReversed, viaLaneDir);
    Connection via2to(0, "", 0);
    setLaneConnections(via2to, via, viaIsReversed, viaLaneDir, to, toAtEnd, toLaneDir);
    for(std::vector<std::pair<int, int> >::const_iterator i=from2via.lanes.begin(); i!=from2via.lanes.end(); ++i) {
        int fromLane = (*i).first;
        int viaLane = (*i).second;
        for(std::vector<std::pair<int, int> >::const_iterator j=via2to.lanes.begin(); j!=via2to.lanes.end(); ++j) {
            if((*j).first==viaLane) {
                c.lanes.push_back(make_pair(fromLane, (*j).second));
                break;
            }
        }
    }
}


NBNode *
NIImporter_OpenDrive::getOrBuildNode(const std::string &id, Position2D &pos, 
                                     NBNodeCont &nc) throw(ProcessError)
{
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
                                    const std::string &nodeID, NIImporter_OpenDrive::LinkType lt) throw(ProcessError)
{
    NBNode *n = nc.retrieve(nodeID);
    if(n==0) {
        throw ProcessError("Could not find node '" + nodeID + "'.");
    }
    if(lt==OPENDRIVE_LT_SUCCESSOR) {
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
                                                 const std::string &nodeID) throw()
{
    NBNode *node = nc.retrieve(nodeID);
    NBEdge *e = ec.retrieve(edgeID);
    if(e!=0 && node->hasOutgoing(e)) {
        return e;
    }
    e = ec.retrieve("-" + edgeID);
    if(e!=0 && node->hasOutgoing(e)) {
        return e;
    }
    return 0;
}


NBEdge *
NIImporter_OpenDrive::getIncomingDirectionalEdge(const NBEdgeCont &ec,
                                                 const NBNodeCont &nc, 
                                                 const std::string &edgeID, 
                                                 const std::string &nodeID) throw()
{
    NBNode *node = nc.retrieve(nodeID);
    NBEdge *e = ec.retrieve(edgeID);
    if(e!=0 && node->hasIncoming(e)) {
        return e;
    }
    e = ec.retrieve("-" + edgeID);
    if(e!=0 && node->hasIncoming(e)) {
        return e;
    }
    return 0;
}


void
NIImporter_OpenDrive::computeShapes(std::vector<OpenDriveEdge> &edges) throw()
{
    for (std::vector<OpenDriveEdge>::iterator i=edges.begin(); i!=edges.end(); ++i) {
        OpenDriveEdge &e = *i;
        for (std::vector<OpenDriveGeometry>::iterator j=e.geometries.begin(); j!=e.geometries.end(); ++j) {
            OpenDriveGeometry &g = *j;
            std::vector<Position2D> geom;
            switch(g.type) {
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
            for(std::vector<Position2D>::iterator k=geom.begin(); k!=geom.end(); ++k) {
                e.geom.push_back_noDoublePos(*k);
            }
        }
        for(unsigned int j=0; j<e.geom.size(); ++j) {
            if(!GeoConvHelper::x2cartesian(e.geom[j])) {
                MsgHandler::getErrorInstance()->inform("Unable to project coordinates for.");
            }
        }
    }
}

std::vector<Position2D> 
NIImporter_OpenDrive::geomFromLine(const OpenDriveEdge &e, const OpenDriveGeometry &g) throw()
{
    std::vector<Position2D> ret;
    ret.push_back(Position2D(g.x, g.y));
    ret.push_back(calculateStraightEndPoint(g.hdg, g.length, Position2D(g.x, g.y)));
    return ret;
}


std::vector<Position2D> 
NIImporter_OpenDrive::geomFromSpiral(const OpenDriveEdge &e, const OpenDriveGeometry &g) throw()
{
    std::vector<Position2D> ret;
    SUMOReal curveStart = g.params[0];
    SUMOReal curveEnd = g.params[1];
    SUMOReal start_x = g.x;
    SUMOReal start_y = g.y;
    SUMOReal start_Angle = g.hdg;
    SUMOReal length = g.length;

    SUMOReal curve;
    bool left;
    bool first_spiral;
    SUMOReal dist, tmpx, tmpy, tmphdg;
	if(fabs(curveEnd) > fabs(curveStart)) {
		curve = curveEnd;
		//Bestimmen der Richtung
		// wird bei der Berechnung der Endkoordinate des neuen Kurven-Segmentes benötigt
        if(curveEnd > 0.0) {
			left = true;
        } else {
			left = false;
        }
		first_spiral = true;
		dist = 0.0;
		tmpx = start_x;
		tmpy = start_y;
		tmphdg = start_Angle;
		//Berechnung des Startpunkts der Klothoide, bei der Start- und Endkrümmung != 0 sind
		if(fabs(curveStart) != 0.0) {
			//Berechnung der Länge bis Krümmung 0
			SUMOReal r1 = 1. / fabs(curveStart);
			SUMOReal r2 = 1. / fabs(curveEnd);
			dist = (r2 * length) / (r1 - r2);
			//Berechnung des ersten Punkts
			start_Angle += PI;
			calculateFirstClothoidPoint(&start_x, &start_y, &start_Angle, curveStart*-1. , dist);
			start_Angle -= PI;
			length += dist;
		}
	} else {
		if(curveEnd == 0.0) {
			curve = curveStart;
			calculateFirstClothoidPoint(&start_x, &start_y, &start_Angle, curve, length);
			start_Angle += PI;
			//Bestimmen der Richtung
			// wird bei der Berechnung der Endkoordinate des neuen Kurven-Segmentes benötigt
            if(curveStart > 0.0) {
				left = false;
            } else {
				left = true;
            }
			first_spiral = false;
			dist = length;
			tmpx = g.x;
			tmpy = g.y;
            tmphdg = g.hdg;
        } else {
			return ret;
        }
	}

	SUMOReal end_x, end_y, endZ, end_Angle;
	SUMOReal geo_posS = g.s;
	SUMOReal geo_posE = g.s;
	bool end = false;
	do {
		//setzen der Startkoordinate der original Klothoide
		//bei Berechnung der Endkoordinate wird immer vom original Startpunkt der Klothoide ausgegangen
		end_x = start_x;
		end_y = start_y;
		end_Angle = start_Angle;	
		geo_posE += C_LENGTH;
        if(geo_posE - g.s > g.length) {
			geo_posE = g.s + g.length;
		}
		//Bestimmen der Entfernung zum zu berechnenden Punkts
        if(first_spiral) {
			dist += (geo_posE - geo_posS);
        } else {
			dist -= (geo_posE - geo_posS);
        }
		//Berechnen der XY-Koordinate
		if(dist > 0.0) {
			calculateClothoidProperties(&end_x, &end_y, &end_Angle, curve, length, dist, left);
		} else {
			//Bei der Klothoide Start != 0 entspricht der Startwert dem zu berechnenden Endwert
			end_x = start_x;
			end_y = start_y;
			end_Angle = end_Angle - PI - PI;	
		}
        if(!first_spiral) {
			end_Angle += PI;
        }
        //
        ret.push_back(Position2D(tmpx, tmpy));
        //
		geo_posS = geo_posE;
		tmpx = end_x;
		tmpy = end_y;
		tmphdg = end_Angle;
		if (geo_posE  - (g.s + g.length) < 0.001 && geo_posE  - (g.s + g.length) > -0.001) {
			end = true;
		}
	} while(!end);
    return ret;
}


std::vector<Position2D> 
NIImporter_OpenDrive::geomFromArc(const OpenDriveEdge &e, const OpenDriveGeometry &g) throw()
{
    std::vector<Position2D> ret;
	SUMOReal dist = 0.0;
	SUMOReal centerX = g.x;
	SUMOReal centerY = g.y;
	//Positiv Linkskurve; Negativ Rechtskurve
    SUMOReal curvature = g.params[0];
    SUMOReal radius = 1. / curvature;
	// Berechnung des Mittelpunktes
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
		SUMOReal startds, endds;
		geo_posE += C_LENGTH;
		// Berechnung eines Punktes auf der Kurve abhängig von der Länge (Konstante)
        if(geo_posE - g.s > g.length) {
			geo_posE = g.s + g.length;
		}
        calcPointOnCurve(&endX, &endY, centerX, centerY, radius, geo_posE - geo_posS);
		//Berechnen des Richtungswinkels des Berechneten Punktes
		dist += (geo_posE - geo_posS);
		if(curvature > 0.0) {
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
	}while(!end);
    return ret;
}


std::vector<Position2D> 
NIImporter_OpenDrive::geomFromPoly(const OpenDriveEdge &e, const OpenDriveGeometry &g) throw()
{
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
NIImporter_OpenDrive::calculateClothoidProperties(SUMOReal *x, SUMOReal *y, SUMOReal *hdg, SUMOReal curve, 
									   SUMOReal length, SUMOReal dist, bool direction) throw() {
	
	double xtmp, ytmp;
	//Radius am Ende der Klothoide (Kreisradius)
	double r = abs(1 / curve);
	// 'A' Klothoiden-Parameter,immer Konstant
	double a = sqrt(length * r);
	// Radius am zum Berechneneden Endpunkt der Klothoide
	r = pow(a, 2) / dist;
	double l = a / r;
	// Tangenten Winkel = Richtungswinkel des Endpunktes
	double new_hdg = dist/(2 * (pow(a, 2)/dist));
	
	double omega_hdg;
	double lengthS;
	//Berechnung aus RAS-L (Richtlinien für die Anlage von Straßen Teil Linienführung)
	xtmp = a *( l - pow(l, 5.0)/40.0 + pow(l, 9.0)/3456.0 - pow(l, 13.0)/599040.0 + pow(l, 17.0)/175472640.0);
	ytmp = a * ( pow(l, 3.0)/6.0 - pow(l, 7.0)/366.0 + pow(l, 11.0)/42240.0 - pow(l, 15.0)/9767800.0);
	// Falls Rechtskurve -> Spiegeln der Klothoide an der X achse (X bleibt / Y * (-1))
	if(!direction)
		ytmp *= (-1);
	//Richtungswibnkel der Sehne
	omega_hdg = atan(ytmp/xtmp);
	//Länge der Sehne
	lengthS = sqrt(pow(xtmp,2) + pow(ytmp,2));

	long double tmp;
	tmp = xtmp;
	//Rotieren umd den Richtungswinkel des Startpunkts
	xtmp = xtmp * cos(*hdg) - ytmp * sin(*hdg);
	ytmp = tmp * sin(*hdg) + ytmp * cos(*hdg); 
 
	// Rückgabe der End-Koordinaten
	*x = *x + xtmp;
	*y = *y + ytmp;
	// Rückgabe des Richtungswinkels abhängig von der Richtung der Kurve
	if(direction)
		*hdg = new_hdg +*hdg;
	else
		*hdg = *hdg - new_hdg;
}
//Berechnung des Startpunktes der Klothoide
//Dabei wird zunächst die normale Klothoide Berechnet
//Dannach wird der Endpunkt auf den Koordinatenursprung verschoben
//Und zum Schluss der Startpunkt anhand der Sehne (Start- <-> Endpunkt) berechnet
void 
NIImporter_OpenDrive::calculateFirstClothoidPoint(SUMOReal* ad_X, SUMOReal* ad_Y, SUMOReal* ad_hdg, 
						SUMOReal ad_curvature, SUMOReal ad_lengthE) throw()
{
	//Entscheidung Links oder Rechtskurve
	//notwendig, da bei einer Linkskurve die Normale-Klothoide gespiegelt werden muss
	//BERECHNUNG RÜCKWÄRTS
	long double reflect;
	//Umgekehrt da Klothoide vom Ende betrachtet wird
	if(ad_curvature > 0.0)
		reflect = -1.0;
	else
	{
		reflect = 1.0;
	}
	//Klothoiden Parameter
	long double A = sqrt(ad_lengthE * 1/abs(ad_curvature)); 	
	long double xE, yE;
	long double rE = pow(A,2)/ad_lengthE;
	double lE = A / rE;
	//Berechnung der Endpunkte der Normalen-Klothoide anhand von A
	xE = A *( lE - pow(lE, 5.0)/40.0 + pow(lE, 9.0)/3456.0 - pow(lE, 13.0)/599040.0 + pow(lE, 17.0)/175472640.0);
	yE = reflect* A * ( pow(lE, 3.0)/6.0 - pow(lE, 7.0)/366.0 + pow(lE, 11.0)/42240.0 - pow(lE, 15.0)/9767800.0);
	//Berechnung des Tangentenwinkels der Normalen-Klotoide
	long double lok_hdg = pow(A, 2)/ (2 * pow(rE,2));
	//Richtungswinkel des Startpunktes
	long double new_hdg;
	if(reflect > 0.0)
		new_hdg = *ad_hdg - lok_hdg;
	else
		new_hdg = *ad_hdg + lok_hdg;

	//Abfangen der negativen Winkel-Werte
	if(new_hdg <0.0)
		new_hdg = 2 * PI + new_hdg;
	// Berechnung der Sehhne zw. Startpunkt und Endpunkt
	long double S = sqrt(pow(yE, 2)+ pow(xE, 2));
	//Richtungswinkel der Sehne
	long double omega = atan(yE/xE);
	
	//******* VEKTORRECHNUNG ************

	long double normxS = 1.0;
	long double normyS = 0.0;

	long double tmp;
	
	tmp = normxS;
	normxS = normxS * cos(PI + new_hdg + omega) - normyS * sin(PI + new_hdg + omega);
	normyS = tmp * sin(PI + new_hdg + omega) + normyS * cos(PI + new_hdg + omega);
	//Startpunkt der normalen Klothoide versetzt vom Koordinatenurprung
	//anhand der Sehne S
	normxS *= S;
	normyS *= S;
	
	//Rückgabe
	*ad_X -= normxS;
	*ad_Y -= normyS;
	*ad_hdg = new_hdg;
}


//Berechnung des Mittelpunkts einer Kurve
void
NIImporter_OpenDrive::calculateCurveCenter(SUMOReal *ad_x, SUMOReal *ad_y, SUMOReal ad_radius, SUMOReal ad_hdg) throw()
{
	SUMOReal normX = 1.0;
	SUMOReal normY = 0.0;
	SUMOReal tmpX;
	SUMOReal turn;
	if(ad_radius > 0)
		turn = -1.0;
	else
		turn = 1.0;
	//Rotation um den Richtungswinkel (KEINE BEACHTUNG DER RICHTUNG !!!!!)
	tmpX = normX;
	normX = normX * cos(ad_hdg) + normY * sin(ad_hdg);
	normY = tmpX * sin(ad_hdg) + normY * cos(ad_hdg);
	
	//Rotation um 90° (Beachtung der Richtung)
	tmpX = normX;
	normX = normX * cos(90 * PI / 180) + turn * normY * sin(90 * PI / 180);
	normY = -1 * turn * tmpX * sin(90 * PI / 180) + normY * cos(90 * PI / 180);

	//Verlängern um den Radius
	normX = abs(ad_radius) * normX;
	normY = abs(ad_radius) * normY;
	
	//Verschieben der Punkte
	*ad_x += normX;
	*ad_y += normY;
}

//Berechnung eines Punktes auf der Kurve in Abhängigkeit von ad_length
void
NIImporter_OpenDrive::calcPointOnCurve(SUMOReal *ad_x, SUMOReal *ad_y, SUMOReal ad_centerX, SUMOReal ad_centerY, 
						   SUMOReal ad_r, SUMOReal ad_length) throw()
{
	//Mittelpunktswinkel
	double rotAngle = ad_length/abs(ad_r);
	//Vektor vom Mittelpunkt zum Startpunkt
	double vx = *ad_x - ad_centerX;
	double vy = *ad_y - ad_centerY;
	double tmpx;

	double turn;
	if(ad_r > 0)
		turn = -1; //Links
	else
		turn = 1; //Rechts
	tmpx = vx;
	//Rotation (Beachtung der Richtung)
	vx = vx * cos(rotAngle) + turn * vy * sin(rotAngle);
	vy = -1 * turn * tmpx * sin(rotAngle) + vy * cos(rotAngle);
	//Verschiebung
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
        myInnerEdges(innerEdges), myOuterEdges(outerEdges) {}


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
        cout << "found edge '" << id << "'" << endl;
        std::string junction = attrs.getStringReporting(SUMO_ATTR_OPENDRIVE_JUNCTION, "road", id.c_str(), ok);
        SUMOReal length = attrs.getSUMORealReporting(SUMO_ATTR_OPENDRIVE_LENGTH, "road", id.c_str(), ok);
        myCurrentEdge = OpenDriveEdge(id, junction, length);
    }
    break;
    case SUMO_TAG_OPENDRIVE_PREDECESSOR: {
        if(myElementStack.size()>=2&&myElementStack[myElementStack.size()-2]==SUMO_TAG_OPENDRIVE_ROAD) {
            std::string elementType = attrs.getStringReporting(SUMO_ATTR_OPENDRIVE_ELEMENTTYPE, "predecessor", myCurrentEdge.id.c_str(), ok);
            std::string elementID = attrs.getStringReporting(SUMO_ATTR_OPENDRIVE_ELEMENTID, "predecessor", myCurrentEdge.id.c_str(), ok);
            std::string contactPoint = attrs.hasAttribute(SUMO_ATTR_OPENDRIVE_CONTACTPOINT)
                ? attrs.getStringReporting(SUMO_ATTR_OPENDRIVE_CONTACTPOINT, "predecessor", myCurrentEdge.id.c_str(), ok)
                : "end";
            addLink(OPENDRIVE_LT_PREDECESSOR, elementType, elementID, contactPoint);
        }
        if(myElementStack.size()>=2&&myElementStack[myElementStack.size()-2]==SUMO_TAG_OPENDRIVE_LANE||myElementStack[myElementStack.size()-2]==SUMO_TAG_LANE) { // !!!
            int no = attrs.getIntReporting(SUMO_ATTR_ID, "predecessor", myCurrentEdge.id.c_str(), ok);
            OpenDriveLane &l = myCurrentEdge.laneSections[myCurrentEdge.laneSections.size()-1].lanesByDir[myCurrentLaneDirection].back();
            l.predecessor = no;
        }
    }
    break;
    case SUMO_TAG_OPENDRIVE_SUCCESSOR: {
        if(myElementStack.size()>=2&&myElementStack[myElementStack.size()-2]==SUMO_TAG_OPENDRIVE_ROAD) {
            std::string elementType = attrs.getStringReporting(SUMO_ATTR_OPENDRIVE_ELEMENTTYPE, "successor", myCurrentEdge.id.c_str(), ok);
            std::string elementID = attrs.getStringReporting(SUMO_ATTR_OPENDRIVE_ELEMENTID, "successor", myCurrentEdge.id.c_str(), ok);
            std::string contactPoint = attrs.hasAttribute(SUMO_ATTR_OPENDRIVE_CONTACTPOINT)
                ? attrs.getStringReporting(SUMO_ATTR_OPENDRIVE_CONTACTPOINT, "successor", myCurrentEdge.id.c_str(), ok)
                : "start";
            addLink(OPENDRIVE_LT_SUCCESSOR, elementType, elementID, contactPoint);
        }
        if(myElementStack.size()>=2&&myElementStack[myElementStack.size()-2]==SUMO_TAG_OPENDRIVE_LANE||myElementStack[myElementStack.size()-2]==SUMO_TAG_LANE) { // !!!
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
        int level = attrs.getIntReporting(SUMO_ATTR_OPENDRIVE_LEVEL, "lane", myCurrentEdge.id.c_str(), ok);
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
}



void
NIImporter_OpenDrive::myEndElement(SumoXMLTag element) throw(ProcessError) {
    myElementStack.pop_back();
    switch (element) {
    case SUMO_TAG_OPENDRIVE_ROAD:
        if(myCurrentEdge.junction=="" || myCurrentEdge.junction=="-1") {
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

