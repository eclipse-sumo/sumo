/****************************************************************************/
/// @file    NWWriter_SUMO.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Tue, 04.05.2011
/// @version $Id$
///
// Exporter writing networks using the SUMO format
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
#include <cmath>
#include <algorithm>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/common/ToString.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringUtils.h>
#include <utils/geom/GeomConvHelper.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBNetBuilder.h>
#include <netbuild/NBTrafficLightLogic.h>
#include <netbuild/NBDistrict.h>
#include "NWFrame.h"
#include "NWWriter_SUMO.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS



// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static methods
// ---------------------------------------------------------------------------
void
NWWriter_SUMO::writeNetwork(const OptionsCont& oc, NBNetBuilder& nb) {
    // check whether a sumo net-file shall be generated
    if (!oc.isSet("output-file")) {
        return;
    }
    OutputDevice& device = OutputDevice::getDevice(oc.getString("output-file"));
    std::map<SumoXMLAttr, std::string> attrs;
    attrs[SUMO_ATTR_VERSION] = NWFrame::MAJOR_VERSION;
    if (oc.getBool("lefthand")) {
        attrs[SUMO_ATTR_LEFTHAND] = "true";
    }
    const int cornerDetail = oc.getInt("junctions.corner-detail");
    if (cornerDetail > 0) {
        attrs[SUMO_ATTR_CORNERDETAIL] = toString(cornerDetail);
    }
    if (!oc.isDefault("junctions.internal-link-detail")) {
        attrs[SUMO_ATTR_LINKDETAIL] = toString(oc.getInt("junctions.internal-link-detail"));
    }
    device.writeXMLHeader("net", "net_file.xsd", attrs); // street names may contain non-ascii chars
    device.lf();
    // get involved container
    const NBNodeCont& nc = nb.getNodeCont();
    const NBEdgeCont& ec = nb.getEdgeCont();
    const NBDistrictCont& dc = nb.getDistrictCont();

    // write network offsets and projection
    GeoConvHelper::writeLocation(device);

    // write edge types and restrictions
    nb.getTypeCont().writeTypes(device);

    // write inner lanes
    bool origNames = oc.getBool("output.original-names");
    if (!oc.getBool("no-internal-links")) {
        bool hadAny = false;
        for (std::map<std::string, NBNode*>::const_iterator i = nc.begin(); i != nc.end(); ++i) {
            hadAny |= writeInternalEdges(device, ec, *(*i).second, origNames);
        }
        if (hadAny) {
            device.lf();
        }
    }

    // write edges with lanes and connected edges
    bool noNames = !oc.getBool("output.street-names");
    for (std::map<std::string, NBEdge*>::const_iterator i = ec.begin(); i != ec.end(); ++i) {
        writeEdge(device, *(*i).second, noNames, origNames);
    }
    device.lf();

    // write tls logics
    writeTrafficLights(device, nb.getTLLogicCont());

    // write the nodes (junctions)
    std::set<NBNode*> roundaboutNodes;
    const bool checkLaneFoesAll = oc.getBool("check-lane-foes.all");
    const bool checkLaneFoesRoundabout = !checkLaneFoesAll && oc.getBool("check-lane-foes.roundabout");
    if (checkLaneFoesRoundabout) {
        const std::set<EdgeSet>& roundabouts = ec.getRoundabouts();
        for (std::set<EdgeSet>::const_iterator i = roundabouts.begin(); i != roundabouts.end(); ++i) {
            for (EdgeSet::const_iterator j = (*i).begin(); j != (*i).end(); ++j) {
                roundaboutNodes.insert((*j)->getToNode());
            }
        }
    }
    for (std::map<std::string, NBNode*>::const_iterator i = nc.begin(); i != nc.end(); ++i) {
        const bool checkLaneFoes = checkLaneFoesAll || (checkLaneFoesRoundabout && roundaboutNodes.count((*i).second) > 0);
        writeJunction(device, *(*i).second, checkLaneFoes);
    }
    device.lf();
    const bool includeInternal = !oc.getBool("no-internal-links");
    if (includeInternal) {
        // ... internal nodes if not unwanted
        bool hadAny = false;
        for (std::map<std::string, NBNode*>::const_iterator i = nc.begin(); i != nc.end(); ++i) {
            hadAny |= writeInternalNodes(device, *(*i).second);
        }
        if (hadAny) {
            device.lf();
        }
    }

    // write the successors of lanes
    int numConnections = 0;
    for (std::map<std::string, NBEdge*>::const_iterator it_edge = ec.begin(); it_edge != ec.end(); it_edge++) {
        NBEdge* from = it_edge->second;
        from->sortOutgoingConnectionsByIndex();
        const std::vector<NBEdge::Connection> connections = from->getConnections();
        numConnections += (int)connections.size();
        for (std::vector<NBEdge::Connection>::const_iterator it_c = connections.begin(); it_c != connections.end(); it_c++) {
            writeConnection(device, *from, *it_c, includeInternal);
        }
    }
    if (numConnections > 0) {
        device.lf();
    }
    if (includeInternal) {
        // ... internal successors if not unwanted
        bool hadAny = false;
        for (std::map<std::string, NBNode*>::const_iterator i = nc.begin(); i != nc.end(); ++i) {
            hadAny |= writeInternalConnections(device, *(*i).second);
        }
        if (hadAny) {
            device.lf();
        }
    }
    for (std::map<std::string, NBNode*>::const_iterator i = nc.begin(); i != nc.end(); ++i) {
        NBNode* node = (*i).second;
        // write connections from pedestrian crossings
        const std::vector<NBNode::Crossing>& crossings = node->getCrossings();
        for (std::vector<NBNode::Crossing>::const_iterator it = crossings.begin(); it != crossings.end(); it++) {
            NWWriter_SUMO::writeInternalConnection(device, (*it).id, (*it).nextWalkingArea, 0, 0, "");
        }
        // write connections from pedestrian walking areas
        const std::vector<NBNode::WalkingArea>& WalkingAreas = node->getWalkingAreas();
        for (std::vector<NBNode::WalkingArea>::const_iterator it = WalkingAreas.begin(); it != WalkingAreas.end(); it++) {
            if ((*it).nextCrossing != "") {
                const NBNode::Crossing& nextCrossing = node->getCrossing((*it).nextCrossing);
                // connection to next crossing (may be tls-controlled)
                device.openTag(SUMO_TAG_CONNECTION);
                device.writeAttr(SUMO_ATTR_FROM, (*it).id);
                device.writeAttr(SUMO_ATTR_TO, (*it).nextCrossing);
                device.writeAttr(SUMO_ATTR_FROM_LANE, 0);
                device.writeAttr(SUMO_ATTR_TO_LANE, 0);
                if (nextCrossing.tlID != "") {
                    device.writeAttr(SUMO_ATTR_TLID, nextCrossing.tlID);
                    assert(nextCrossing.tlLinkNo >= 0);
                    device.writeAttr(SUMO_ATTR_TLLINKINDEX, nextCrossing.tlLinkNo);
                }
                device.writeAttr(SUMO_ATTR_DIR, LINKDIR_STRAIGHT);
                device.writeAttr(SUMO_ATTR_STATE, nextCrossing.priority ? LINKSTATE_MAJOR : LINKSTATE_MINOR);
                device.closeTag();
            }
            // optional connections from/to sidewalk
            for (std::vector<std::string>::const_iterator it_sw = (*it).nextSidewalks.begin(); it_sw != (*it).nextSidewalks.end(); ++it_sw) {
                NWWriter_SUMO::writeInternalConnection(device, (*it).id, (*it_sw), 0, 0, "");
            }
            for (std::vector<std::string>::const_iterator it_sw = (*it).prevSidewalks.begin(); it_sw != (*it).prevSidewalks.end(); ++it_sw) {
                NWWriter_SUMO::writeInternalConnection(device, (*it_sw), (*it).id, 0, 0, "");
            }
        }
    }

    // write loaded prohibitions
    for (std::map<std::string, NBNode*>::const_iterator i = nc.begin(); i != nc.end(); ++i) {
        writeProhibitions(device, i->second->getProhibitions());
    }

    // write roundabout information
    writeRoundabouts(device, ec.getRoundabouts(), ec);

    // write the districts
    for (std::map<std::string, NBDistrict*>::const_iterator i = dc.begin(); i != dc.end(); i++) {
        writeDistrict(device, *(*i).second);
    }
    if (dc.size() != 0) {
        device.lf();
    }
    device.close();
}


std::string
NWWriter_SUMO::getOppositeInternalID(const NBEdgeCont& ec, const NBEdge* from, const NBEdge::Connection& con) {
    const NBEdge::Lane& succ = con.toEdge->getLanes()[con.toLane];
    const NBEdge::Lane& pred = from->getLanes()[con.fromLane];
    if (succ.oppositeID != "" && succ.oppositeID != "-" && pred.oppositeID != "" && pred.oppositeID != "-") {
        // find the connection that connects succ.oppositeID to pred.oppositeID
        const NBEdge* succOpp = ec.retrieve(succ.oppositeID.substr(0, succ.oppositeID.rfind("_")));
        const NBEdge* predOpp = ec.retrieve(pred.oppositeID.substr(0, pred.oppositeID.rfind("_")));
        assert(succOpp != 0);
        assert(predOpp != 0);
        const std::vector<NBEdge::Connection>& connections = succOpp->getConnections();
        for (std::vector<NBEdge::Connection>::const_iterator it_c = connections.begin(); it_c != connections.end(); it_c++) {
            const NBEdge::Connection& conOpp = *it_c;
            if (succOpp != from && // turnaround
                    succOpp->getLaneID(conOpp.fromLane) == succ.oppositeID &&
                    predOpp == conOpp.toEdge &&
                    predOpp->getLaneID(conOpp.toLane) == pred.oppositeID) {
                return conOpp.getInternalLaneID();
            }
        }
        return "";
    } else {
        return "";
    }
}


bool
NWWriter_SUMO::writeInternalEdges(OutputDevice& into, const NBEdgeCont& ec, const NBNode& n, bool origNames) {
    bool ret = false;
    const EdgeVector& incoming = n.getIncomingEdges();
    for (EdgeVector::const_iterator i = incoming.begin(); i != incoming.end(); i++) {
        const std::vector<NBEdge::Connection>& elv = (*i)->getConnections();
        if (elv.size() > 0) {
            bool haveVia = false;
            NBEdge* toEdge = 0;
            std::string internalEdgeID = "";
            // first pass: compute average lengths of non-via edges
            std::map<NBEdge*, SUMOReal> lengthSum;
            std::map<NBEdge*, int> numLanes;
            for (std::vector<NBEdge::Connection>::const_iterator k = elv.begin(); k != elv.end(); ++k) {
                lengthSum[(*k).toEdge] += MAX2((*k).shape.length(), POSITION_EPS);
                numLanes[(*k).toEdge] += 1;
            }
            // second pass: write non-via edges
            for (std::vector<NBEdge::Connection>::const_iterator k = elv.begin(); k != elv.end(); ++k) {
                if ((*k).toEdge == 0) {
                    assert(false); // should never happen. tell me when it does
                    continue;
                }
                if (toEdge != (*k).toEdge) {
                    internalEdgeID = (*k).id;
                    if (toEdge != 0) {
                        // close the previous edge
                        into.closeTag();
                    }
                    toEdge = (*k).toEdge;
                    into.openTag(SUMO_TAG_EDGE);
                    into.writeAttr(SUMO_ATTR_ID, internalEdgeID);
                    into.writeAttr(SUMO_ATTR_FUNCTION, EDGEFUNC_INTERNAL);
                    // open a new edge
                }
                // to avoid changing to an internal lane which has a successor
                // with the wrong permissions we need to inherit them from the successor
                const NBEdge::Lane& successor = (*k).toEdge->getLanes()[(*k).toLane];
                const SUMOReal length = lengthSum[toEdge] / numLanes[toEdge];
                // @note the actual length should be used once sumo supports lanes of
                // varying length within the same edge
                //const SUMOReal length = MAX2((*k).shape.length(), POSITION_EPS);
                writeLane(into, (*k).getInternalLaneID(), (*k).vmax,
                          successor.permissions, successor.preferred,
                          NBEdge::UNSPECIFIED_OFFSET, successor.width, (*k).shape, (*k).origID,
                          length, (*k).internalLaneIndex, origNames, getOppositeInternalID(ec, *i, *k), &n);
                haveVia = haveVia || (*k).haveVia;
            }
            ret = true;
            into.closeTag(); // close the last edge
            // third pass: write via edges
            if (haveVia) {
                for (std::vector<NBEdge::Connection>::const_iterator k = elv.begin(); k != elv.end(); ++k) {
                    if (!(*k).haveVia) {
                        continue;
                    }
                    if ((*k).toEdge == 0) {
                        assert(false); // should never happen. tell me when it does
                        continue;
                    }
                    const NBEdge::Lane& successor = (*k).toEdge->getLanes()[(*k).toLane];
                    into.openTag(SUMO_TAG_EDGE);
                    into.writeAttr(SUMO_ATTR_ID, (*k).viaID);
                    into.writeAttr(SUMO_ATTR_FUNCTION, EDGEFUNC_INTERNAL);
                    writeLane(into, (*k).viaID + "_0", (*k).viaVmax, SVCAll, SVCAll,
                              NBEdge::UNSPECIFIED_OFFSET, successor.width, (*k).viaShape, (*k).origID,
                              MAX2((*k).viaShape.length(), POSITION_EPS), // microsim needs positive length
                              0, origNames, "", &n);
                    into.closeTag();
                }
            }
        }
    }
    // write pedestrian crossings
    const std::vector<NBNode::Crossing>& crossings = n.getCrossings();
    for (std::vector<NBNode::Crossing>::const_iterator it = crossings.begin(); it != crossings.end(); it++) {
        into.openTag(SUMO_TAG_EDGE);
        into.writeAttr(SUMO_ATTR_ID, (*it).id);
        into.writeAttr(SUMO_ATTR_FUNCTION, EDGEFUNC_CROSSING);
        into.writeAttr(SUMO_ATTR_CROSSING_EDGES, (*it).edges);
        writeLane(into, (*it).id + "_0", 1, SVC_PEDESTRIAN, 0,
                  NBEdge::UNSPECIFIED_OFFSET, (*it).width, (*it).shape, "", (*it).shape.length(), 0, false, "", &n);
        into.closeTag();
    }
    // write pedestrian walking areas
    const std::vector<NBNode::WalkingArea>& WalkingAreas = n.getWalkingAreas();
    for (std::vector<NBNode::WalkingArea>::const_iterator it = WalkingAreas.begin(); it != WalkingAreas.end(); it++) {
        const NBNode::WalkingArea& wa = *it;
        into.openTag(SUMO_TAG_EDGE);
        into.writeAttr(SUMO_ATTR_ID, wa.id);
        into.writeAttr(SUMO_ATTR_FUNCTION, EDGEFUNC_WALKINGAREA);
        writeLane(into, wa.id + "_0", 1, SVC_PEDESTRIAN, 0,
                  NBEdge::UNSPECIFIED_OFFSET, wa.width, wa.shape, "", wa.length, 0, false, "", &n);
        into.closeTag();
    }
    return ret;
}


void
NWWriter_SUMO::writeEdge(OutputDevice& into, const NBEdge& e, bool noNames, bool origNames) {
    // write the edge's begin
    into.openTag(SUMO_TAG_EDGE).writeAttr(SUMO_ATTR_ID, e.getID());
    into.writeAttr(SUMO_ATTR_FROM, e.getFromNode()->getID());
    into.writeAttr(SUMO_ATTR_TO, e.getToNode()->getID());
    if (!noNames && e.getStreetName() != "") {
        into.writeAttr(SUMO_ATTR_NAME, StringUtils::escapeXML(e.getStreetName()));
    }
    into.writeAttr(SUMO_ATTR_PRIORITY, e.getPriority());
    if (e.getTypeID() != "") {
        into.writeAttr(SUMO_ATTR_TYPE, e.getTypeID());
    }
    if (e.isMacroscopicConnector()) {
        into.writeAttr(SUMO_ATTR_FUNCTION, EDGEFUNC_CONNECTOR);
    }
    // write the spread type if not default ("right")
    if (e.getLaneSpreadFunction() != LANESPREAD_RIGHT) {
        into.writeAttr(SUMO_ATTR_SPREADTYPE, e.getLaneSpreadFunction());
    }
    if (e.hasLoadedLength()) {
        into.writeAttr(SUMO_ATTR_LENGTH, e.getLoadedLength());
    }
    if (!e.hasDefaultGeometry()) {
        into.writeAttr(SUMO_ATTR_SHAPE, e.getGeometry());
    }
    // write the lanes
    const std::vector<NBEdge::Lane>& lanes = e.getLanes();

    const SUMOReal length = e.getFinalLength();
    for (int i = 0; i < (int) lanes.size(); i++) {
        const NBEdge::Lane& l = lanes[i];
        writeLane(into, e.getLaneID(i), l.speed,
                  l.permissions, l.preferred, l.endOffset, l.width, l.shape, l.origID,
                  length, i, origNames, l.oppositeID);
    }
    // close the edge
    into.closeTag();
}


void
NWWriter_SUMO::writeLane(OutputDevice& into, const std::string& lID,
                         SUMOReal speed, SVCPermissions permissions, SVCPermissions preferred,
                         SUMOReal endOffset, SUMOReal width, PositionVector shape,
                         const std::string& origID, SUMOReal length, int index, bool origNames,
                         const std::string& oppositeID, const NBNode* node) {
    // output the lane's attributes
    into.openTag(SUMO_TAG_LANE).writeAttr(SUMO_ATTR_ID, lID);
    // the first lane of an edge will be the depart lane
    into.writeAttr(SUMO_ATTR_INDEX, index);
    // write the list of allowed/disallowed vehicle classes
    if (permissions != SVC_UNSPECIFIED) {
        writePermissions(into, permissions);
    }
    writePreferences(into, preferred);
    // some further information
    if (speed == 0) {
        WRITE_WARNING("Lane '" + lID + "' has a maximum allowed speed of 0.");
    } else if (speed < 0) {
        throw ProcessError("Negative allowed speed (" + toString(speed) + ") on lane '" + lID + "', use --speed.minimum to prevent this.");
    }
    if (endOffset > 0) {
        length = length - endOffset;
    }
    into.writeAttr(SUMO_ATTR_SPEED, speed);
    into.writeAttr(SUMO_ATTR_LENGTH, length);
    if (endOffset != NBEdge::UNSPECIFIED_OFFSET) {
        into.writeAttr(SUMO_ATTR_ENDOFFSET, endOffset);
    }
    if (width != NBEdge::UNSPECIFIED_WIDTH) {
        into.writeAttr(SUMO_ATTR_WIDTH, width);
    }
    if (node != 0) {
        const NBNode::CustomShapeMap& cs = node->getCustomLaneShapes();
        NBNode::CustomShapeMap::const_iterator it = cs.find(lID);
        if (it != cs.end()) {
            shape = it->second;
            into.writeAttr(SUMO_ATTR_CUSTOMSHAPE, true);
        }
    }
    into.writeAttr(SUMO_ATTR_SHAPE, endOffset > 0 ?
                   shape.getSubpart(0, shape.length() - endOffset) : shape);
    if (oppositeID != "" && oppositeID != "-") {
        into.openTag(SUMO_TAG_NEIGH);
        into.writeAttr(SUMO_ATTR_LANE, oppositeID);
        into.closeTag();
    }
    if (origNames && origID != "") {
        into.openTag(SUMO_TAG_PARAM);
        into.writeAttr(SUMO_ATTR_KEY, "origId");
        into.writeAttr(SUMO_ATTR_VALUE, origID);
        into.closeTag();
    }
    into.closeTag();
}


void
NWWriter_SUMO::writeJunction(OutputDevice& into, const NBNode& n, const bool checkLaneFoes) {
    // write the attributes
    into.openTag(SUMO_TAG_JUNCTION).writeAttr(SUMO_ATTR_ID, n.getID());
    into.writeAttr(SUMO_ATTR_TYPE, n.getType());
    NWFrame::writePositionLong(n.getPosition(), into);
    // write the incoming lanes
    std::string incLanes;
    const std::vector<NBEdge*>& incoming = n.getIncomingEdges();
    for (std::vector<NBEdge*>::const_iterator i = incoming.begin(); i != incoming.end(); ++i) {
        int noLanes = (*i)->getNumLanes();
        for (int j = 0; j < noLanes; j++) {
            incLanes += (*i)->getLaneID(j);
            if (i != incoming.end() - 1 || j < noLanes - 1) {
                incLanes += ' ';
            }
        }
    }
    const std::vector<NBNode::Crossing>& crossings = n.getCrossings();
    for (std::vector<NBNode::Crossing>::const_iterator it = crossings.begin(); it != crossings.end(); it++) {
        incLanes += ' ' + (*it).prevWalkingArea + "_0";
    }
    into.writeAttr(SUMO_ATTR_INCLANES, incLanes);
    // write the internal lanes
    std::string intLanes;
    if (!OptionsCont::getOptions().getBool("no-internal-links")) {
        int l = 0;
        for (EdgeVector::const_iterator i = incoming.begin(); i != incoming.end(); i++) {
            const std::vector<NBEdge::Connection>& elv = (*i)->getConnections();
            for (std::vector<NBEdge::Connection>::const_iterator k = elv.begin(); k != elv.end(); ++k) {
                if ((*k).toEdge == 0) {
                    continue;
                }
                if (l != 0) {
                    intLanes += ' ';
                }
                if (!(*k).haveVia) {
                    intLanes += (*k).getInternalLaneID();
                } else {
                    intLanes += (*k).viaID + "_0";
                }
                l++;
            }
        }
    }
    if (n.getType() != NODETYPE_DEAD_END && n.getType() != NODETYPE_NOJUNCTION) {
        for (std::vector<NBNode::Crossing>::const_iterator it = crossings.begin(); it != crossings.end(); it++) {
            intLanes += ' ' + (*it).id + "_0";
        }
    }
    into.writeAttr(SUMO_ATTR_INTLANES, intLanes);
    // close writing
    into.writeAttr(SUMO_ATTR_SHAPE, n.getShape().simplified());
    // write optional radius
    if (n.getRadius() != NBNode::UNSPECIFIED_RADIUS) {
        into.writeAttr(SUMO_ATTR_RADIUS, n.getRadius());
    }
    // specify whether a custom shape was used
    if (n.hasCustomShape()) {
        into.writeAttr(SUMO_ATTR_CUSTOMSHAPE, true);
    }
    if (n.getType() == NODETYPE_DEAD_END) {
        into.closeTag();
    } else {
        // write right-of-way logics
        n.writeLogic(into, checkLaneFoes);
        into.closeTag();
    }
}


bool
NWWriter_SUMO::writeInternalNodes(OutputDevice& into, const NBNode& n) {
    bool ret = false;
    const std::vector<NBEdge*>& incoming = n.getIncomingEdges();
    // build the list of internal lane ids
    std::vector<std::string> internalLaneIDs;
    for (EdgeVector::const_iterator i = incoming.begin(); i != incoming.end(); i++) {
        const std::vector<NBEdge::Connection>& elv = (*i)->getConnections();
        for (std::vector<NBEdge::Connection>::const_iterator k = elv.begin(); k != elv.end(); ++k) {
            if ((*k).toEdge != 0) {
                internalLaneIDs.push_back((*k).getInternalLaneID());
            }
        }
    }
    const std::vector<NBNode::Crossing>& crossings = n.getCrossings();
    for (std::vector<NBNode::Crossing>::const_iterator it_c = crossings.begin(); it_c != crossings.end(); ++it_c) {
        internalLaneIDs.push_back((*it_c).id + "_0");
    }
    // write the internal nodes
    for (std::vector<NBEdge*>::const_iterator i = incoming.begin(); i != incoming.end(); i++) {
        const std::vector<NBEdge::Connection>& elv = (*i)->getConnections();
        for (std::vector<NBEdge::Connection>::const_iterator k = elv.begin(); k != elv.end(); ++k) {
            if ((*k).toEdge == 0 || !(*k).haveVia) {
                continue;
            }
            Position pos = (*k).shape[-1];
            into.openTag(SUMO_TAG_JUNCTION).writeAttr(SUMO_ATTR_ID, (*k).viaID + "_0");
            into.writeAttr(SUMO_ATTR_TYPE, NODETYPE_INTERNAL);
            NWFrame::writePositionLong(pos, into);
            std::string incLanes = (*k).getInternalLaneID();
            if ((*k).foeIncomingLanes.length() != 0) {
                incLanes += " " + (*k).foeIncomingLanes;
            }
            into.writeAttr(SUMO_ATTR_INCLANES, incLanes);
            const std::vector<int>& foes = (*k).foeInternalLinks;
            std::vector<std::string> foeIDs;
            for (std::vector<int>::const_iterator it = foes.begin(); it != foes.end(); ++it) {
                foeIDs.push_back(internalLaneIDs[*it]);
            }
            into.writeAttr(SUMO_ATTR_INTLANES, joinToString(foeIDs, " "));
            into.closeTag();
            ret = true;
        }
    }
    return ret;
}


void
NWWriter_SUMO::writeConnection(OutputDevice& into, const NBEdge& from, const NBEdge::Connection& c,
                               bool includeInternal, ConnectionStyle style) {
    assert(c.toEdge != 0);
    into.openTag(SUMO_TAG_CONNECTION);
    into.writeAttr(SUMO_ATTR_FROM, from.getID());
    into.writeAttr(SUMO_ATTR_TO, c.toEdge->getID());
    into.writeAttr(SUMO_ATTR_FROM_LANE, c.fromLane);
    into.writeAttr(SUMO_ATTR_TO_LANE, c.toLane);
    if (c.mayDefinitelyPass && style != TLL) {
        into.writeAttr(SUMO_ATTR_PASS, c.mayDefinitelyPass);
    }
    if ((from.getToNode()->getKeepClear() == false || c.keepClear == false) && style != TLL) {
        into.writeAttr<bool>(SUMO_ATTR_KEEP_CLEAR, false);
    }
    if (c.contPos != NBEdge::UNSPECIFIED_CONTPOS && style != TLL) {
        into.writeAttr(SUMO_ATTR_CONTPOS, c.contPos);
    }
    if (c.visibility != NBEdge::UNSPECIFIED_VISIBILITY_DISTANCE && style != TLL) {
        into.writeAttr(SUMO_ATTR_VISIBILITY_DISTANCE, c.visibility);
    }
    if (style != PLAIN) {
        if (includeInternal) {
            into.writeAttr(SUMO_ATTR_VIA, c.getInternalLaneID());
        }
        // set information about the controlling tl if any
        if (c.tlID != "") {
            into.writeAttr(SUMO_ATTR_TLID, c.tlID);
            into.writeAttr(SUMO_ATTR_TLLINKINDEX, c.tlLinkNo);
        }
        if (style == SUMONET) {
            // write the direction information
            LinkDirection dir = from.getToNode()->getDirection(&from, c.toEdge, OptionsCont::getOptions().getBool("lefthand"));
            assert(dir != LINKDIR_NODIR);
            into.writeAttr(SUMO_ATTR_DIR, toString(dir));
            // write the state information
            const LinkState linkState = from.getToNode()->getLinkState(
                                            &from, c.toEdge, c.fromLane, c.toLane, c.mayDefinitelyPass, c.tlID);
            into.writeAttr(SUMO_ATTR_STATE, linkState);
        }
    }
    into.closeTag();
}


bool
NWWriter_SUMO::writeInternalConnections(OutputDevice& into, const NBNode& n) {
    bool ret = false;
    const std::vector<NBEdge*>& incoming = n.getIncomingEdges();
    for (std::vector<NBEdge*>::const_iterator i = incoming.begin(); i != incoming.end(); ++i) {
        NBEdge* from = *i;
        const std::vector<NBEdge::Connection>& connections = from->getConnections();
        for (std::vector<NBEdge::Connection>::const_iterator j = connections.begin(); j != connections.end(); ++j) {
            const NBEdge::Connection& c = *j;
            assert(c.toEdge != 0);
            if (c.haveVia) {
                // internal split
                writeInternalConnection(into, c.id, c.toEdge->getID(), c.internalLaneIndex, c.toLane, c.viaID + "_0");
                writeInternalConnection(into, c.viaID, c.toEdge->getID(), 0, c.toLane, "");
            } else {
                // no internal split
                writeInternalConnection(into, c.id, c.toEdge->getID(), c.internalLaneIndex, c.toLane, "");
            }
            ret = true;
        }
    }
    return ret;
}


void
NWWriter_SUMO::writeInternalConnection(OutputDevice& into,
                                       const std::string& from, const std::string& to,
                                       int fromLane, int toLane, const std::string& via) {
    into.openTag(SUMO_TAG_CONNECTION);
    into.writeAttr(SUMO_ATTR_FROM, from);
    into.writeAttr(SUMO_ATTR_TO, to);
    into.writeAttr(SUMO_ATTR_FROM_LANE, fromLane);
    into.writeAttr(SUMO_ATTR_TO_LANE, toLane);
    if (via != "") {
        into.writeAttr(SUMO_ATTR_VIA, via);
    }
    into.writeAttr(SUMO_ATTR_DIR, "s");
    into.writeAttr(SUMO_ATTR_STATE, (via != "" ? "m" : "M"));
    into.closeTag();
}


void
NWWriter_SUMO::writeRoundabouts(OutputDevice& into, const std::set<EdgeSet>& roundabouts,
                                const NBEdgeCont& ec) {
    //  make output deterministic
    std::vector<std::vector<std::string> > edgeIDs;
    for (std::set<EdgeSet>::const_iterator i = roundabouts.begin(); i != roundabouts.end(); ++i) {
        std::vector<std::string> tEdgeIDs;
        for (EdgeSet::const_iterator j = (*i).begin(); j != (*i).end(); ++j) {
            // the edges may have been erased from NBEdgeCont but their pointers are still valid
            // we verify their existance in writeRoundabout()
            tEdgeIDs.push_back((*j)->getID());
        }
        std::sort(tEdgeIDs.begin(), tEdgeIDs.end());
        edgeIDs.push_back(tEdgeIDs);
    }
    std::sort(edgeIDs.begin(), edgeIDs.end());
    //  write
    for (std::vector<std::vector<std::string> >::const_iterator i = edgeIDs.begin(); i != edgeIDs.end(); ++i) {
        writeRoundabout(into, *i, ec);
    }
    if (roundabouts.size() != 0) {
        into.lf();
    }
}


void
NWWriter_SUMO::writeRoundabout(OutputDevice& into, const std::vector<std::string>& edgeIDs,
                               const NBEdgeCont& ec) {
    std::vector<std::string> validEdgeIDs;
    std::vector<std::string> invalidEdgeIDs;
    std::vector<std::string> nodeIDs;
    for (std::vector<std::string>::const_iterator i = edgeIDs.begin(); i != edgeIDs.end(); ++i) {
        const NBEdge* edge = ec.retrieve(*i);
        if (edge != 0) {
            nodeIDs.push_back(edge->getToNode()->getID());
            validEdgeIDs.push_back(edge->getID());
        } else {
            invalidEdgeIDs.push_back(*i);
        }
    }
    std::sort(nodeIDs.begin(), nodeIDs.end());
    if (validEdgeIDs.size() > 0) {
        into.openTag(SUMO_TAG_ROUNDABOUT);
        into.writeAttr(SUMO_ATTR_NODES, joinToString(nodeIDs, " "));
        into.writeAttr(SUMO_ATTR_EDGES, joinToString(validEdgeIDs, " "));
        into.closeTag();
        if (invalidEdgeIDs.size() > 0) {
            WRITE_WARNING("Writing incomplete roundabout. Edges: '"
                          + joinToString(invalidEdgeIDs, " ") + "' no longer exist'");
        }
    }
}


void
NWWriter_SUMO::writeDistrict(OutputDevice& into, const NBDistrict& d) {
    std::vector<SUMOReal> sourceW = d.getSourceWeights();
    VectorHelper<SUMOReal>::normaliseSum(sourceW, 1.0);
    std::vector<SUMOReal> sinkW = d.getSinkWeights();
    VectorHelper<SUMOReal>::normaliseSum(sinkW, 1.0);
    // write the head and the id of the district
    into.openTag(SUMO_TAG_TAZ).writeAttr(SUMO_ATTR_ID, d.getID());
    if (d.getShape().size() > 0) {
        into.writeAttr(SUMO_ATTR_SHAPE, d.getShape());
    }
    // write all sources
    const std::vector<NBEdge*>& sources = d.getSourceEdges();
    for (int i = 0; i < (int)sources.size(); i++) {
        // write the head and the id of the source
        into.openTag(SUMO_TAG_TAZSOURCE).writeAttr(SUMO_ATTR_ID, sources[i]->getID()).writeAttr(SUMO_ATTR_WEIGHT, sourceW[i]);
        into.closeTag();
    }
    // write all sinks
    const std::vector<NBEdge*>& sinks = d.getSinkEdges();
    for (int i = 0; i < (int)sinks.size(); i++) {
        // write the head and the id of the sink
        into.openTag(SUMO_TAG_TAZSINK).writeAttr(SUMO_ATTR_ID, sinks[i]->getID()).writeAttr(SUMO_ATTR_WEIGHT, sinkW[i]);
        into.closeTag();
    }
    // write the tail
    into.closeTag();
}


std::string
NWWriter_SUMO::writeSUMOTime(SUMOTime steps) {
    SUMOReal time = STEPS2TIME(steps);
    if (time == std::floor(time)) {
        return toString(int(time));
    } else {
        return toString(time);
    }
}


void
NWWriter_SUMO::writeProhibitions(OutputDevice& into, const NBConnectionProhibits& prohibitions) {
    for (NBConnectionProhibits::const_iterator j = prohibitions.begin(); j != prohibitions.end(); j++) {
        NBConnection prohibited = (*j).first;
        const NBConnectionVector& prohibiting = (*j).second;
        for (NBConnectionVector::const_iterator k = prohibiting.begin(); k != prohibiting.end(); k++) {
            NBConnection prohibitor = *k;
            into.openTag(SUMO_TAG_PROHIBITION);
            into.writeAttr(SUMO_ATTR_PROHIBITOR, prohibitionConnection(prohibitor));
            into.writeAttr(SUMO_ATTR_PROHIBITED, prohibitionConnection(prohibited));
            into.closeTag();
        }
    }
}


std::string
NWWriter_SUMO::prohibitionConnection(const NBConnection& c) {
    return c.getFrom()->getID() + "->" + c.getTo()->getID();
}


void
NWWriter_SUMO::writeTrafficLights(OutputDevice& into, const NBTrafficLightLogicCont& tllCont) {
    std::vector<NBTrafficLightLogic*> logics = tllCont.getComputed();
    for (std::vector<NBTrafficLightLogic*>::iterator it = logics.begin(); it != logics.end(); it++) {
        into.openTag(SUMO_TAG_TLLOGIC);
        into.writeAttr(SUMO_ATTR_ID, (*it)->getID());
        into.writeAttr(SUMO_ATTR_TYPE, (*it)->getType());
        into.writeAttr(SUMO_ATTR_PROGRAMID, (*it)->getProgramID());
        into.writeAttr(SUMO_ATTR_OFFSET, writeSUMOTime((*it)->getOffset()));
        // write params
        (*it)->writeParams(into);
        // write the phases
        const std::vector<NBTrafficLightLogic::PhaseDefinition>& phases = (*it)->getPhases();
        for (std::vector<NBTrafficLightLogic::PhaseDefinition>::const_iterator j = phases.begin(); j != phases.end(); ++j) {
            into.openTag(SUMO_TAG_PHASE);
            into.writeAttr(SUMO_ATTR_DURATION, writeSUMOTime(j->duration));
            into.writeAttr(SUMO_ATTR_STATE, j->state);
            into.closeTag();
        }
        into.closeTag();
    }
    if (logics.size() > 0) {
        into.lf();
    }
}
/****************************************************************************/

