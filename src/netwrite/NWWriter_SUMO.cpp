/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    NWWriter_SUMO.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Leonhard Luecken
/// @date    Tue, 04.05.2011
///
// Exporter writing networks using the SUMO format
/****************************************************************************/
#include <config.h>
#include <cmath>
#include <algorithm>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/common/ToString.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringUtils.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/geom/GeomConvHelper.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBNetBuilder.h>
#include <netbuild/NBTrafficLightLogic.h>
#include <netbuild/NBDistrict.h>
#include <netbuild/NBHelpers.h>
#include "NWFrame.h"
#include "NWWriter_SUMO.h"


//#define DEBUG_OPPOSITE_INTERNAL

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
    attrs[SUMO_ATTR_VERSION] = toString(NETWORK_VERSION);
    if (oc.getBool("lefthand") != oc.getBool("flip-y-axis")) {
        attrs[SUMO_ATTR_LEFTHAND] = "true";
    } else if (oc.getBool("lefthand")) {
        // network was flipped, correct written link directions
        OptionsCont::getOptions().resetWritable();
        OptionsCont::getOptions().set("lefthand", "false");
    }
    const int cornerDetail = oc.getInt("junctions.corner-detail");
    if (cornerDetail > 0) {
        attrs[SUMO_ATTR_CORNERDETAIL] = toString(cornerDetail);
    }
    if (!oc.isDefault("junctions.internal-link-detail")) {
        attrs[SUMO_ATTR_LINKDETAIL] = toString(oc.getInt("junctions.internal-link-detail"));
    }
    if (oc.getBool("rectangular-lane-cut")) {
        attrs[SUMO_ATTR_RECTANGULAR_LANE_CUT] = "true";
    }
    if (oc.getBool("crossings.guess") || oc.getBool("walkingareas")) {
        attrs[SUMO_ATTR_WALKINGAREAS] = "true";
    }
    if (oc.getFloat("junctions.limit-turn-speed") > 0) {
        attrs[SUMO_ATTR_LIMIT_TURN_SPEED] = toString(oc.getFloat("junctions.limit-turn-speed"));
    }
    if (!oc.isDefault("check-lane-foes.all")) {
        attrs[SUMO_ATTR_CHECKLANEFOES_ALL] = toString(oc.getBool("check-lane-foes.all"));
    }
    if (!oc.isDefault("check-lane-foes.roundabout")) {
        attrs[SUMO_ATTR_CHECKLANEFOES_ROUNDABOUT] = toString(oc.getBool("check-lane-foes.roundabout"));
    }
    if (!oc.isDefault("tls.ignore-internal-junction-jam")) {
        attrs[SUMO_ATTR_TLS_IGNORE_INTERNAL_JUNCTION_JAM] = toString(oc.getBool("tls.ignore-internal-junction-jam"));
    }
    if (oc.getString("default.spreadtype") == "roadCenter") {
        // it makes no sense to store the default=center in the net since
        // centered edges would have the attribute written anyway and edges that
        // should have 'right' would be misinterpreted
        attrs[SUMO_ATTR_SPREADTYPE] = oc.getString("default.spreadtype");
    }
    if (oc.exists("geometry.avoid-overlap") && !oc.getBool("geometry.avoid-overlap")) {
        attrs[SUMO_ATTR_AVOID_OVERLAP] = toString(oc.getBool("geometry.avoid-overlap"));
    }
    if (oc.exists("junctions.higher-speed") && oc.getBool("junctions.higher-speed")) {
        attrs[SUMO_ATTR_HIGHER_SPEED] = toString(oc.getBool("junctions.higher-speed"));
    }
    if (oc.exists("internal-junctions.vehicle-width") && !oc.isDefault("internal-junctions.vehicle-width")) {
        attrs[SUMO_ATTR_INTERNAL_JUNCTIONS_VEHICLE_WIDTH] = toString(oc.getFloat("internal-junctions.vehicle-width"));
    }
    if (!oc.isDefault("junctions.minimal-shape")) {
        attrs[SUMO_ATTR_JUNCTIONS_MINIMAL_SHAPE] = toString(oc.getBool("junctions.minimal-shape"));
    }
    if (!oc.isDefault("junctions.endpoint-shape")) {
        attrs[SUMO_ATTR_JUNCTIONS_ENDPOINT_SHAPE] = toString(oc.getBool("junctions.endpoint-shape"));
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
    std::set<std::string> usedTypes = ec.getUsedTypes();
    nb.getTypeCont().writeEdgeTypes(device, usedTypes);

    // write inner lanes
    if (!oc.getBool("no-internal-links")) {
        bool hadAny = false;
        for (std::map<std::string, NBNode*>::const_iterator i = nc.begin(); i != nc.end(); ++i) {
            hadAny |= writeInternalEdges(device, ec, *(*i).second);
        }
        if (hadAny) {
            device.lf();
        }
    }

    // write edges with lanes and connected edges
    bool noNames = !oc.getBool("output.street-names");
    for (std::map<std::string, NBEdge*>::const_iterator i = ec.begin(); i != ec.end(); ++i) {
        writeEdge(device, *(*i).second, noNames);
    }
    device.lf();

    // write tls logics
    writeTrafficLights(device, nb.getTLLogicCont());

    // write the nodes (junctions)
    for (std::map<std::string, NBNode*>::const_iterator i = nc.begin(); i != nc.end(); ++i) {
        writeJunction(device, *(*i).second);
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
        const std::vector<NBEdge::Connection>& connections = from->getConnections();
        numConnections += (int)connections.size();
        for (const NBEdge::Connection& con : connections) {
            writeConnection(device, *from, con, includeInternal);
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
        std::vector<NBNode::Crossing*> crossings = node->getCrossings();
        for (auto c : crossings) {
            NWWriter_SUMO::writeInternalConnection(device, c->id, c->nextWalkingArea, 0, 0, "", LinkDirection::STRAIGHT, c->tlID, c->tlLinkIndex2);
        }
        // write connections from pedestrian walking areas
        for (const NBNode::WalkingArea& wa : node->getWalkingAreas()) {
            for (const std::string& cID : wa.nextCrossings) {
                const NBNode::Crossing& nextCrossing = *node->getCrossing(cID);
                // connection to next crossing (may be tls-controlled)
                device.openTag(SUMO_TAG_CONNECTION);
                device.writeAttr(SUMO_ATTR_FROM, wa.id);
                device.writeAttr(SUMO_ATTR_TO, cID);
                device.writeAttr(SUMO_ATTR_FROM_LANE, 0);
                device.writeAttr(SUMO_ATTR_TO_LANE, 0);
                if (nextCrossing.tlID != "") {
                    device.writeAttr(SUMO_ATTR_TLID, nextCrossing.tlID);
                    assert(nextCrossing.tlLinkIndex >= 0);
                    device.writeAttr(SUMO_ATTR_TLLINKINDEX, nextCrossing.tlLinkIndex);
                }
                device.writeAttr(SUMO_ATTR_DIR, LinkDirection::STRAIGHT);
                device.writeAttr(SUMO_ATTR_STATE, nextCrossing.priority ? LINKSTATE_MAJOR : LINKSTATE_MINOR);
                device.closeTag();
            }
            // optional connections from/to sidewalk
            std::string edgeID;
            int laneIndex;
            for (const std::string& sw : wa.nextSidewalks) {
                NBHelpers::interpretLaneID(sw, edgeID, laneIndex);
                NWWriter_SUMO::writeInternalConnection(device, wa.id, edgeID, 0, laneIndex, "");
            }
            for (const std::string& sw : wa.prevSidewalks) {
                NBHelpers::interpretLaneID(sw, edgeID, laneIndex);
                NWWriter_SUMO::writeInternalConnection(device, edgeID, wa.id, laneIndex, 0, "");
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
    if (dc.size() != 0 && oc.isDefault("taz-output")) {
        WRITE_WARNING(TL("Embedding TAZ-data inside the network is deprecated. Use option --taz-output instead"));
        for (std::map<std::string, NBDistrict*>::const_iterator i = dc.begin(); i != dc.end(); i++) {
            writeDistrict(device, *(*i).second);
        }
        device.lf();
    }
    device.close();
}


std::string
NWWriter_SUMO::getOppositeInternalID(const NBEdgeCont& ec, const NBEdge* from, const NBEdge::Connection& con, double& oppositeLength) {
    const NBEdge::Lane& succ = con.toEdge->getLanes()[con.toLane];
    const NBEdge::Lane& pred = from->getLanes()[con.fromLane];
    const bool lefthand = OptionsCont::getOptions().getBool("lefthand");
    if (succ.oppositeID != "" && succ.oppositeID != "-" && pred.oppositeID != "" && pred.oppositeID != "-") {
#ifdef DEBUG_OPPOSITE_INTERNAL
        std::cout << "getOppositeInternalID con=" << con.getDescription(from) << " (" << con.getInternalLaneID() << ")\n";
#endif
        // find the connection that connects succ.oppositeID to pred.oppositeID
        const NBEdge* succOpp = ec.retrieve(succ.oppositeID.substr(0, succ.oppositeID.rfind("_")));
        const NBEdge* predOpp = ec.retrieve(pred.oppositeID.substr(0, pred.oppositeID.rfind("_")));
        assert(succOpp != 0);
        assert(predOpp != 0);
        const std::vector<NBEdge::Connection>& connections = succOpp->getConnections();
        for (std::vector<NBEdge::Connection>::const_iterator it_c = connections.begin(); it_c != connections.end(); it_c++) {
            const NBEdge::Connection& conOpp = *it_c;
            if (succOpp != from // turnaround
                    && predOpp == conOpp.toEdge
                    && succOpp->getLaneID(conOpp.fromLane) == succ.oppositeID
                    && predOpp->getLaneID(conOpp.toLane) == pred.oppositeID
                    && from->getToNode()->getDirection(from, con.toEdge, lefthand) == LinkDirection::STRAIGHT
                    && from->getToNode()->getDirection(succOpp, predOpp, lefthand) == LinkDirection::STRAIGHT
               ) {
#ifdef DEBUG_OPPOSITE_INTERNAL
                std::cout << "  found " << conOpp.getInternalLaneID() << "\n";
#endif
                oppositeLength = conOpp.length;
                return conOpp.getInternalLaneID();
            } else {
                /*
                #ifdef DEBUG_OPPOSITE_INTERNAL
                std::cout << "  rejected " << conOpp.getInternalLaneID()
                    << "\n     succ.oppositeID=" << succ.oppositeID
                    << "\n         succOppLane=" << succOpp->getLaneID(conOpp.fromLane)
                    << "\n     pred.oppositeID=" << pred.oppositeID
                    << "\n         predOppLane=" << predOpp->getLaneID(conOpp.toLane)
                    << "\n      predOpp=" << predOpp->getID()
                    << "\n     conOppTo=" << conOpp.toEdge->getID()
                    << "\n     len1=" << con.shape.length()
                    << "\n     len2=" << conOpp.shape.length()
                    << "\n";
                #endif
                */
            }
        }
        return "";
    } else {
        return "";
    }
}


bool
NWWriter_SUMO::writeInternalEdges(OutputDevice& into, const NBEdgeCont& ec, const NBNode& n) {
    bool ret = false;
    const EdgeVector& incoming = n.getIncomingEdges();
    // first pass: determine opposite internal edges and average their length
    std::map<std::string, std::string> oppositeLaneID;
    std::map<std::string, double> oppositeLengths;
    for (NBEdge* e : incoming) {
        for (const NBEdge::Connection& c : e->getConnections()) {
            double oppositeLength = 0;
            const std::string op = getOppositeInternalID(ec, e, c, oppositeLength);
            oppositeLaneID[c.getInternalLaneID()] = op;
            if (op != "") {
                oppositeLengths[c.id] = oppositeLength;
            }
        }
    }
    if (oppositeLengths.size() > 0) {
        for (NBEdge* e : incoming) {
            for (NBEdge::Connection& c : e->getConnections()) {
                if (oppositeLengths.count(c.id) > 0) {
                    c.length = (c.length + oppositeLengths[c.id]) / 2;
                }
            }
        }
    }

    for (NBEdge* e : incoming) {
        const std::vector<NBEdge::Connection>& elv = e->getConnections();
        if (elv.size() > 0) {
            bool haveVia = false;
            std::string edgeID = "";
            double bidiLength = -1;
            // second pass: write non-via edges
            for (const NBEdge::Connection& k : elv) {
                if (k.toEdge == nullptr) {
                    assert(false); // should never happen. tell me when it does
                    continue;
                }
                if (edgeID != k.id) {
                    if (edgeID != "") {
                        // close the previous edge
                        into.closeTag();
                    }
                    edgeID = k.id;
                    into.openTag(SUMO_TAG_EDGE);
                    into.writeAttr(SUMO_ATTR_ID, edgeID);
                    into.writeAttr(SUMO_ATTR_FUNCTION, SumoXMLEdgeFunc::INTERNAL);
                    if (k.edgeType != "") {
                        into.writeAttr(SUMO_ATTR_TYPE, k.edgeType);
                    }
                    bidiLength = -1;
                    if (e->getBidiEdge() && k.toEdge->getBidiEdge() &&
                            e != k.toEdge->getTurnDestination(true)) {
                        const std::string bidiEdge = getInternalBidi(e, k, bidiLength);
                        if (bidiEdge != "") {
                            into.writeAttr(SUMO_ATTR_BIDI, bidiEdge);
                        }
                    }
                    // open a new edge
                }
                // to avoid changing to an internal lane which has a successor
                // with the wrong permissions we need to inherit them from the successor
                const NBEdge::Lane& successor = k.toEdge->getLanes()[k.toLane];
                SVCPermissions permissions = (k.permissions != SVC_UNSPECIFIED) ? k.permissions : (
                                                 successor.permissions & e->getPermissions(k.fromLane));
                SVCPermissions changeLeft = k.changeLeft != SVC_UNSPECIFIED ? k.changeLeft : SVCAll;
                SVCPermissions changeRight = k.changeRight != SVC_UNSPECIFIED ? k.changeRight : SVCAll;
                const double width = e->getInternalLaneWidth(n, k, successor, false);
                const double length = bidiLength > 0 ? bidiLength : k.length;
                writeLane(into, k.getInternalLaneID(), k.vmax, k.friction,
                          permissions, successor.preferred,
                          changeLeft, changeRight,
                          NBEdge::UNSPECIFIED_OFFSET, NBEdge::UNSPECIFIED_OFFSET,
                          StopOffset(), width, k.shape, &k,
                          length, k.internalLaneIndex, oppositeLaneID[k.getInternalLaneID()], "");
                haveVia = haveVia || k.haveVia;
            }
            ret = true;
            into.closeTag(); // close the last edge
            // third pass: write via edges
            if (haveVia) {
                for (const NBEdge::Connection& k : elv) {
                    if (!k.haveVia) {
                        continue;
                    }
                    if (k.toEdge == nullptr) {
                        assert(false); // should never happen. tell me when it does
                        continue;
                    }
                    const NBEdge::Lane& successor = k.toEdge->getLanes()[k.toLane];
                    into.openTag(SUMO_TAG_EDGE);
                    into.writeAttr(SUMO_ATTR_ID, k.viaID);
                    into.writeAttr(SUMO_ATTR_FUNCTION, SumoXMLEdgeFunc::INTERNAL);
                    if (k.edgeType != "") {
                        into.writeAttr(SUMO_ATTR_TYPE, k.edgeType);
                    }
                    SVCPermissions permissions = (k.permissions != SVC_UNSPECIFIED) ? k.permissions : (
                                                     successor.permissions & e->getPermissions(k.fromLane));
                    const double width = e->getInternalLaneWidth(n, k, successor, true);
                    writeLane(into, k.viaID + "_0", k.vmax, k.friction, permissions, successor.preferred,
                              SVCAll, SVCAll, // #XXX todo
                              NBEdge::UNSPECIFIED_OFFSET, NBEdge::UNSPECIFIED_OFFSET,
                              StopOffset(), width, k.viaShape, &k,
                              MAX2(k.viaLength, POSITION_EPS), // microsim needs positive length
                              0, "", "");
                    into.closeTag();
                }
            }
        }
    }
    // write pedestrian crossings
    const double crossingSpeed = OptionsCont::getOptions().getFloat("default.crossing-speed");
    for (auto c : n.getCrossings()) {
        into.openTag(SUMO_TAG_EDGE);
        into.writeAttr(SUMO_ATTR_ID, c->id);
        into.writeAttr(SUMO_ATTR_FUNCTION, SumoXMLEdgeFunc::CROSSING);
        into.writeAttr(SUMO_ATTR_CROSSING_EDGES, c->edges);
        writeLane(into, c->id + "_0", crossingSpeed, NBEdge::UNSPECIFIED_FRICTION, SVC_PEDESTRIAN, 0, SVCAll, SVCAll,
                  NBEdge::UNSPECIFIED_OFFSET, NBEdge::UNSPECIFIED_OFFSET,
                  StopOffset(), c->width, c->shape, c,
                  MAX2(c->shape.length(), POSITION_EPS), 0, "", "", false, c->customShape.size() != 0, c->outlineShape);
        into.closeTag();
    }
    // write pedestrian walking areas
    const double walkingareaSpeed = OptionsCont::getOptions().getFloat("default.walkingarea-speed");
    const std::vector<NBNode::WalkingArea>& WalkingAreas = n.getWalkingAreas();
    for (std::vector<NBNode::WalkingArea>::const_iterator it = WalkingAreas.begin(); it != WalkingAreas.end(); it++) {
        const NBNode::WalkingArea& wa = *it;
        into.openTag(SUMO_TAG_EDGE);
        into.writeAttr(SUMO_ATTR_ID, wa.id);
        into.writeAttr(SUMO_ATTR_FUNCTION, SumoXMLEdgeFunc::WALKINGAREA);
        writeLane(into, wa.id + "_0", walkingareaSpeed, NBEdge::UNSPECIFIED_FRICTION, SVC_PEDESTRIAN, 0, SVCAll, SVCAll,
                  NBEdge::UNSPECIFIED_OFFSET, NBEdge::UNSPECIFIED_OFFSET,
                  StopOffset(), wa.width, wa.shape, nullptr, wa.length, 0, "", "", false, wa.hasCustomShape);
        into.closeTag();
    }
    return ret;
}


std::string
NWWriter_SUMO::getInternalBidi(const NBEdge* e, const NBEdge::Connection& k, double& length) {
    const NBEdge* fromBidi = e->getTurnDestination(true);
    const NBEdge* toBidi = k.toEdge->getTurnDestination(true);
    const std::vector<NBEdge::Connection> cons = toBidi->getConnectionsFromLane(-1, fromBidi, -1);
    if (cons.size() > 0) {
        if (e->getNumLanes() == 1 && k.toEdge->getNumLanes() == 1 && fromBidi->getNumLanes() == 1 && toBidi->getNumLanes() == 1) {
            length = (k.length + cons.back().length) / 2;
            return cons.back().id;
        }
        // do a more careful check in case there are parallel internal edges
        // note: k is the first connection with the new id
        for (const NBEdge::Connection& c : e->getConnections()) {
            if (c.id == k.id) {
                PositionVector rShape = c.shape.reverse();
                for (const NBEdge::Connection& k2 : cons) {
                    if (k2.shape.almostSame(rShape, POSITION_EPS)) {
                        length = (c.length + k2.length) / 2;
                        return k2.id;
                    }
                }
            }
        }
    } else {
        WRITE_WARNINGF(TL("Could not find bidi-connection for edge '%'"), k.id)
    }
    return "";
}

void
NWWriter_SUMO::writeEdge(OutputDevice& into, const NBEdge& e, bool noNames) {
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
        into.writeAttr(SUMO_ATTR_FUNCTION, SumoXMLEdgeFunc::CONNECTOR);
    }
    // write the spread type if not default ("right")
    if (e.getLaneSpreadFunction() != LaneSpreadFunction::RIGHT) {
        into.writeAttr(SUMO_ATTR_SPREADTYPE, e.getLaneSpreadFunction());
    }
    if (e.hasLoadedLength()) {
        into.writeAttr(SUMO_ATTR_LENGTH, e.getLoadedLength());
    }
    if (!e.hasDefaultGeometry()) {
        into.writeAttr(SUMO_ATTR_SHAPE, e.getGeometry());
    }
    if (e.getEdgeStopOffset().isDefined()) {
        writeStopOffsets(into, e.getEdgeStopOffset());
    }
    if (e.getBidiEdge()) {
        into.writeAttr(SUMO_ATTR_BIDI, e.getBidiEdge()->getID());
    }
    if (e.getDistance() != 0) {
        into.writeAttr(SUMO_ATTR_DISTANCE, e.getDistance());
    }

    // write the lanes
    const std::vector<NBEdge::Lane>& lanes = e.getLanes();

    double length = e.getFinalLength();
    if (e.getBidiEdge() != nullptr) {
        length = (length + e.getBidiEdge()->getFinalLength()) / 2;
    }
    double startOffset = e.isBidiRail() ? e.getTurnDestination(true)->getEndOffset() : 0;
    for (int i = 0; i < (int) lanes.size(); i++) {
        const NBEdge::Lane& l = lanes[i];
        StopOffset stopOffset;
        if (l.laneStopOffset != e.getEdgeStopOffset()) {
            stopOffset = l.laneStopOffset;
        }
        writeLane(into, e.getLaneID(i), l.speed, l.friction,
                  l.permissions, l.preferred,
                  l.changeLeft, l.changeRight,
                  startOffset, l.endOffset,
                  stopOffset, l.width, l.shape, &l,
                  length, i, l.oppositeID, l.type, l.accelRamp, l.customShape.size() > 0);
    }
    // close the edge
    e.writeParams(into);
    into.closeTag();
}


void
NWWriter_SUMO::writeLane(OutputDevice& into, const std::string& lID,
                         double speed, double friction,
                         SVCPermissions permissions, SVCPermissions preferred,
                         SVCPermissions changeLeft, SVCPermissions changeRight,
                         double startOffset, double endOffset,
                         const StopOffset& stopOffset, double width, PositionVector shape,
                         const Parameterised* params, double length, int index,
                         const std::string& oppositeID,
                         const std::string& type,
                         bool accelRamp, bool customShape,
                         const PositionVector& outlineShape) {
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
    into.writeAttr(SUMO_ATTR_SPEED, MAX2(0.0, speed));
    if (friction != NBEdge::UNSPECIFIED_FRICTION) {
        into.writeAttr(SUMO_ATTR_FRICTION, friction);
    }
    into.writeAttr(SUMO_ATTR_LENGTH, length);
    if (endOffset != NBEdge::UNSPECIFIED_OFFSET) {
        into.writeAttr(SUMO_ATTR_ENDOFFSET, endOffset);
    }
    if (width != NBEdge::UNSPECIFIED_WIDTH) {
        into.writeAttr(SUMO_ATTR_WIDTH, width);
    }
    if (accelRamp) {
        into.writeAttr<bool>(SUMO_ATTR_ACCELERATION, accelRamp);
    }
    if (customShape) {
        into.writeAttr(SUMO_ATTR_CUSTOMSHAPE, true);
    }
    if (endOffset > 0 || startOffset > 0) {
        startOffset = MIN2(startOffset, shape.length() - POSITION_EPS);
        endOffset = MIN2(endOffset, shape.length() - startOffset - POSITION_EPS);
        assert(startOffset + endOffset < shape.length());
        shape = shape.getSubpart(startOffset, shape.length() - endOffset);
    }
    into.writeAttr(SUMO_ATTR_SHAPE, shape);
    if (type != "") {
        into.writeAttr(SUMO_ATTR_TYPE, type);
    }
    if (changeLeft != SVC_UNSPECIFIED && changeLeft != SVCAll && changeLeft != SVC_IGNORING) {
        into.writeAttr(SUMO_ATTR_CHANGE_LEFT, getVehicleClassNames(changeLeft));
    }
    if (changeRight != SVC_UNSPECIFIED && changeRight != SVCAll && changeRight != SVC_IGNORING) {
        into.writeAttr(SUMO_ATTR_CHANGE_RIGHT, getVehicleClassNames(changeRight));
    }
    if (stopOffset.isDefined()) {
        writeStopOffsets(into, stopOffset);
    }
    if (outlineShape.size() != 0) {
        into.writeAttr(SUMO_ATTR_OUTLINESHAPE, outlineShape);
    }

    if (oppositeID != "" && oppositeID != "-") {
        into.openTag(SUMO_TAG_NEIGH);
        into.writeAttr(SUMO_ATTR_LANE, oppositeID);
        into.closeTag();
    }

    if (params != nullptr) {
        params->writeParams(into);
    }

    into.closeTag();
}


void
NWWriter_SUMO::writeJunction(OutputDevice& into, const NBNode& n) {
    // write the attributes
    into.openTag(SUMO_TAG_JUNCTION).writeAttr(SUMO_ATTR_ID, n.getID());
    into.writeAttr(SUMO_ATTR_TYPE, n.getType());
    NWFrame::writePositionLong(n.getPosition(), into);
    // write the incoming lanes
    std::vector<std::string> incLanes;
    const std::vector<NBEdge*>& incoming = n.getIncomingEdges();
    for (std::vector<NBEdge*>::const_iterator i = incoming.begin(); i != incoming.end(); ++i) {
        int noLanes = (*i)->getNumLanes();
        for (int j = 0; j < noLanes; j++) {
            incLanes.push_back((*i)->getLaneID(j));
        }
    }
    std::vector<NBNode::Crossing*> crossings = n.getCrossings();
    std::set<std::string> prevWAs;
    // avoid duplicates
    for (auto c : crossings) {
        if (prevWAs.count(c->prevWalkingArea) == 0) {
            incLanes.push_back(c->prevWalkingArea + "_0");
            prevWAs.insert(c->prevWalkingArea);
        }
    }
    into.writeAttr(SUMO_ATTR_INCLANES, incLanes);
    // write the internal lanes
    std::vector<std::string> intLanes;
    if (!OptionsCont::getOptions().getBool("no-internal-links")) {
        for (EdgeVector::const_iterator i = incoming.begin(); i != incoming.end(); i++) {
            const std::vector<NBEdge::Connection>& elv = (*i)->getConnections();
            for (std::vector<NBEdge::Connection>::const_iterator k = elv.begin(); k != elv.end(); ++k) {
                if ((*k).toEdge == nullptr) {
                    continue;
                }
                if (!(*k).haveVia) {
                    intLanes.push_back((*k).getInternalLaneID());
                } else {
                    intLanes.push_back((*k).viaID + "_0");
                }
            }
        }
    }
    if (n.getType() != SumoXMLNodeType::DEAD_END && n.getType() != SumoXMLNodeType::NOJUNCTION) {
        for (auto c : crossings) {
            intLanes.push_back(c->id + "_0");
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
    if (n.getRightOfWay() != RightOfWay::DEFAULT) {
        into.writeAttr<std::string>(SUMO_ATTR_RIGHT_OF_WAY, toString(n.getRightOfWay()));
    }
    if (n.getFringeType() != FringeType::DEFAULT) {
        into.writeAttr<std::string>(SUMO_ATTR_FRINGE, toString(n.getFringeType()));
    }
    if (n.getName() != "") {
        into.writeAttr<std::string>(SUMO_ATTR_NAME, StringUtils::escapeXML(n.getName()));
    }
    if (n.getType() != SumoXMLNodeType::DEAD_END) {
        // write right-of-way logics
        n.writeLogic(into);
    }
    n.writeParams(into);
    into.closeTag();
}


bool
NWWriter_SUMO::writeInternalNodes(OutputDevice& into, const NBNode& n) {
    bool ret = false;
    const std::vector<NBEdge*>& incoming = n.getIncomingEdges();
    // build the list of internal lane ids
    std::vector<std::string> internalLaneIDs;
    std::map<std::string, std::string> viaIDs;
    for (EdgeVector::const_iterator i = incoming.begin(); i != incoming.end(); i++) {
        const std::vector<NBEdge::Connection>& elv = (*i)->getConnections();
        for (std::vector<NBEdge::Connection>::const_iterator k = elv.begin(); k != elv.end(); ++k) {
            if ((*k).toEdge != nullptr) {
                internalLaneIDs.push_back((*k).getInternalLaneID());
                viaIDs[(*k).getInternalLaneID()] = ((*k).viaID);
            }
        }
    }
    for (auto c : n.getCrossings()) {
        internalLaneIDs.push_back(c->id + "_0");
    }
    // write the internal nodes
    for (std::vector<NBEdge*>::const_iterator i = incoming.begin(); i != incoming.end(); i++) {
        const std::vector<NBEdge::Connection>& elv = (*i)->getConnections();
        for (std::vector<NBEdge::Connection>::const_iterator k = elv.begin(); k != elv.end(); ++k) {
            if ((*k).toEdge == nullptr || !(*k).haveVia) {
                continue;
            }
            Position pos = (*k).shape[-1];
            into.openTag(SUMO_TAG_JUNCTION).writeAttr(SUMO_ATTR_ID, (*k).viaID + "_0");
            into.writeAttr(SUMO_ATTR_TYPE, SumoXMLNodeType::INTERNAL);
            NWFrame::writePositionLong(pos, into);
            std::string incLanes = (*k).getInternalLaneID();
            std::vector<std::string> foeIDs;
            for (std::string incLane : (*k).foeIncomingLanes) {
                if (incLane[0] == ':') {
                    // intersecting left turns
                    const int index = StringUtils::toInt(incLane.substr(1));
                    incLane = internalLaneIDs[index];
                    if (viaIDs[incLane] != "") {
                        foeIDs.push_back(viaIDs[incLane] + "_0");
                    }
                }
                incLanes += " " + incLane;
            }
            into.writeAttr(SUMO_ATTR_INCLANES, incLanes);
            const std::vector<int>& foes = (*k).foeInternalLinks;
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
                               bool includeInternal, ConnectionStyle style, bool geoAccuracy) {
    assert(c.toEdge != 0);
    into.openTag(SUMO_TAG_CONNECTION);
    into.writeAttr(SUMO_ATTR_FROM, from.getID());
    into.writeAttr(SUMO_ATTR_TO, c.toEdge->getID());
    into.writeAttr(SUMO_ATTR_FROM_LANE, c.fromLane);
    into.writeAttr(SUMO_ATTR_TO_LANE, c.toLane);
    if (style != TLL) {
        if (c.mayDefinitelyPass) {
            into.writeAttr(SUMO_ATTR_PASS, c.mayDefinitelyPass);
        }
        if (c.keepClear == KEEPCLEAR_FALSE) {
            into.writeAttr<bool>(SUMO_ATTR_KEEP_CLEAR, false);
        }
        if (c.contPos != NBEdge::UNSPECIFIED_CONTPOS) {
            into.writeAttr(SUMO_ATTR_CONTPOS, c.contPos);
        }
        if (c.permissions != SVC_UNSPECIFIED) {
            writePermissions(into, c.permissions);
        }
        if (c.changeLeft != SVC_UNSPECIFIED && c.changeLeft != SVCAll && c.changeLeft != SVC_IGNORING) {
            into.writeAttr(SUMO_ATTR_CHANGE_LEFT, getVehicleClassNames(c.changeLeft));
        }
        if (c.changeRight != SVC_UNSPECIFIED && c.changeRight != SVCAll && c.changeRight != SVC_IGNORING) {
            into.writeAttr(SUMO_ATTR_CHANGE_RIGHT, getVehicleClassNames(c.changeRight));
        }
        if (c.speed != NBEdge::UNSPECIFIED_SPEED) {
            into.writeAttr(SUMO_ATTR_SPEED, c.speed);
        }
        if (c.customLength != NBEdge::UNSPECIFIED_LOADED_LENGTH) {
            into.writeAttr(SUMO_ATTR_LENGTH, c.customLength);
        }
        if (c.customShape.size() != 0) {
            if (geoAccuracy) {
                into.setPrecision(gPrecisionGeo);
            }
            into.writeAttr(SUMO_ATTR_SHAPE, c.customShape);
            if (geoAccuracy) {
                into.setPrecision();
            }
        }
        if (c.uncontrolled != false) {
            into.writeAttr(SUMO_ATTR_UNCONTROLLED, c.uncontrolled);
        }
        if (c.indirectLeft != false) {
            into.writeAttr(SUMO_ATTR_INDIRECT, c.indirectLeft);
        }
        if (c.edgeType != "") {
            into.writeAttr(SUMO_ATTR_TYPE, c.edgeType);
        }
    }
    if (style != PLAIN) {
        if (includeInternal) {
            into.writeAttr(SUMO_ATTR_VIA, c.getInternalLaneID());
        }
        // set information about the controlling tl if any
        if (c.tlID != "") {
            into.writeAttr(SUMO_ATTR_TLID, c.tlID);
            into.writeAttr(SUMO_ATTR_TLLINKINDEX, c.tlLinkIndex);
            if (c.tlLinkIndex2 >= 0) {
                into.writeAttr(SUMO_ATTR_TLLINKINDEX2, c.tlLinkIndex2);
            }
        }
    }
    if (style != TLL) {
        if (style == SUMONET) {
            // write the direction information
            LinkDirection dir = from.getToNode()->getDirection(&from, c.toEdge, OptionsCont::getOptions().getBool("lefthand"));
            assert(dir != LinkDirection::NODIR);
            into.writeAttr(SUMO_ATTR_DIR, toString(dir));
            // write the state information
            const LinkState linkState = from.getToNode()->getLinkState(
                                            &from, c.toEdge, c.fromLane, c.toLane, c.mayDefinitelyPass, c.tlID);
            into.writeAttr(SUMO_ATTR_STATE, linkState);
            if (linkState == LINKSTATE_MINOR
                    && c.visibility == NBEdge::UNSPECIFIED_VISIBILITY_DISTANCE
                    && c.toEdge->getJunctionPriority(c.toEdge->getToNode()) == NBEdge::JunctionPriority::ROUNDABOUT) {
                const double visibilityDistance = OptionsCont::getOptions().getFloat("roundabouts.visibility-distance");
                if (visibilityDistance != NBEdge::UNSPECIFIED_VISIBILITY_DISTANCE) {
                    into.writeAttr(SUMO_ATTR_VISIBILITY_DISTANCE, visibilityDistance);
                }
            }
        }
        if (c.visibility != NBEdge::UNSPECIFIED_VISIBILITY_DISTANCE) {
            into.writeAttr(SUMO_ATTR_VISIBILITY_DISTANCE, c.visibility);
        }
    }
    c.writeParams(into);
    into.closeTag();
}


bool
NWWriter_SUMO::writeInternalConnections(OutputDevice& into, const NBNode& n) {
    bool ret = false;
    const bool lefthand = OptionsCont::getOptions().getBool("lefthand");
    for (const NBEdge* const from : n.getIncomingEdges()) {
        for (const NBEdge::Connection& c : from->getConnections()) {
            LinkDirection dir = n.getDirection(from, c.toEdge, lefthand);
            assert(c.toEdge != 0);
            if (c.haveVia) {
                // internal split with optional signal
                std::string tlID = "";
                int linkIndex2 = NBConnection::InvalidTlIndex;
                if (c.tlLinkIndex2 != NBConnection::InvalidTlIndex) {
                    linkIndex2 = c.tlLinkIndex2;
                    tlID = c.tlID;
                }
                writeInternalConnection(into, c.id, c.toEdge->getID(), c.internalLaneIndex, c.toLane, c.viaID + "_0", dir, tlID, linkIndex2, false, c.visibility);
                writeInternalConnection(into, c.viaID, c.toEdge->getID(), 0, c.toLane, "", dir, "", NBConnection::InvalidTlIndex, n.brakeForCrossingOnExit(c.toEdge));
            } else {
                // no internal split
                writeInternalConnection(into, c.id, c.toEdge->getID(), c.internalLaneIndex, c.toLane, "", dir);
            }
            ret = true;
        }
    }
    return ret;
}


void
NWWriter_SUMO::writeInternalConnection(OutputDevice& into,
                                       const std::string& from, const std::string& to,
                                       int fromLane, int toLane, const std::string& via,
                                       LinkDirection dir,
                                       const std::string& tlID, int linkIndex,
                                       bool minor,
                                       double visibility) {
    into.openTag(SUMO_TAG_CONNECTION);
    into.writeAttr(SUMO_ATTR_FROM, from);
    into.writeAttr(SUMO_ATTR_TO, to);
    into.writeAttr(SUMO_ATTR_FROM_LANE, fromLane);
    into.writeAttr(SUMO_ATTR_TO_LANE, toLane);
    if (via != "") {
        into.writeAttr(SUMO_ATTR_VIA, via);
    }
    if (tlID != "" && linkIndex != NBConnection::InvalidTlIndex) {
        // used for the reverse direction of pedestrian crossings
        into.writeAttr(SUMO_ATTR_TLID, tlID);
        into.writeAttr(SUMO_ATTR_TLLINKINDEX, linkIndex);
    }
    into.writeAttr(SUMO_ATTR_DIR, dir);
    into.writeAttr(SUMO_ATTR_STATE, ((via != "" || minor) ? "m" : "M"));
    if (visibility != NBEdge::UNSPECIFIED_VISIBILITY_DISTANCE) {
        into.writeAttr(SUMO_ATTR_VISIBILITY_DISTANCE, visibility);
    }
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
        if (edge != nullptr) {
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
    std::vector<double> sourceW = d.getSourceWeights();
    VectorHelper<double>::normaliseSum(sourceW, 1.0);
    std::vector<double> sinkW = d.getSinkWeights();
    VectorHelper<double>::normaliseSum(sinkW, 1.0);
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
    double time = STEPS2TIME(steps);
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
    for (NBTrafficLightLogic* logic : logics) {
        writeTrafficLight(into, logic);
        // only raise warnings on write instead of on compute (to avoid cluttering netedit)
        NBTrafficLightDefinition* def = tllCont.getDefinition(logic->getID(), logic->getProgramID());
        assert(def != nullptr);
        def->finalChecks();
    }
    if (logics.size() > 0) {
        into.lf();
    }
}


void
NWWriter_SUMO::writeTrafficLight(OutputDevice& into, const NBTrafficLightLogic* logic) {
    into.openTag(SUMO_TAG_TLLOGIC);
    into.writeAttr(SUMO_ATTR_ID, logic->getID());
    into.writeAttr(SUMO_ATTR_TYPE, logic->getType());
    into.writeAttr(SUMO_ATTR_PROGRAMID, logic->getProgramID());
    into.writeAttr(SUMO_ATTR_OFFSET, writeSUMOTime(logic->getOffset()));
    // write the phases
    const bool varPhaseLength = logic->getType() != TrafficLightType::STATIC;
    for (const NBTrafficLightLogic::PhaseDefinition& phase : logic->getPhases()) {
        into.openTag(SUMO_TAG_PHASE);
        into.writeAttr(SUMO_ATTR_DURATION, writeSUMOTime(phase.duration));
        if (phase.duration < TIME2STEPS(10)) {
            into.writePadding(" ");
        }
        into.writeAttr(SUMO_ATTR_STATE, phase.state);
        if (varPhaseLength) {
            if (phase.minDur != NBTrafficLightDefinition::UNSPECIFIED_DURATION) {
                into.writeAttr(SUMO_ATTR_MINDURATION, writeSUMOTime(phase.minDur));
            }
            if (phase.maxDur != NBTrafficLightDefinition::UNSPECIFIED_DURATION) {
                into.writeAttr(SUMO_ATTR_MAXDURATION, writeSUMOTime(phase.maxDur));
            }
            if (phase.earliestEnd != NBTrafficLightDefinition::UNSPECIFIED_DURATION) {
                into.writeAttr(SUMO_ATTR_EARLIEST_END, writeSUMOTime(phase.earliestEnd));
            }
            if (phase.latestEnd != NBTrafficLightDefinition::UNSPECIFIED_DURATION) {
                into.writeAttr(SUMO_ATTR_LATEST_END, writeSUMOTime(phase.latestEnd));
            }
            // NEMA attributes
            if (phase.vehExt != NBTrafficLightDefinition::UNSPECIFIED_DURATION) {
                into.writeAttr(SUMO_ATTR_VEHICLEEXTENSION, writeSUMOTime(phase.vehExt));
            }
            if (phase.yellow != NBTrafficLightDefinition::UNSPECIFIED_DURATION) {
                into.writeAttr(SUMO_ATTR_YELLOW, writeSUMOTime(phase.yellow));
            }
            if (phase.red != NBTrafficLightDefinition::UNSPECIFIED_DURATION) {
                into.writeAttr(SUMO_ATTR_RED, writeSUMOTime(phase.red));
            }
        }
        if (phase.name != "") {
            into.writeAttr(SUMO_ATTR_NAME, StringUtils::escapeXML(phase.name));
        }
        if (phase.next.size() > 0) {
            into.writeAttr(SUMO_ATTR_NEXT, phase.next);
        }
        into.closeTag();
    }
    // write params
    logic->writeParams(into);
    into.closeTag();
}


void
NWWriter_SUMO::writeStopOffsets(OutputDevice& into, const StopOffset& stopOffset) {
    if (stopOffset.isDefined()) {
        const std::string ss_vclasses = getVehicleClassNames(stopOffset.getPermissions());
        if (ss_vclasses.length() == 0) {
            // This stopOffset would have no effect...
            return;
        }
        into.openTag(SUMO_TAG_STOPOFFSET);
        const std::string ss_exceptions = getVehicleClassNames(~stopOffset.getPermissions());
        if (ss_vclasses.length() <= ss_exceptions.length()) {
            into.writeAttr(SUMO_ATTR_VCLASSES, ss_vclasses);
        } else {
            if (ss_exceptions.length() == 0) {
                into.writeAttr(SUMO_ATTR_VCLASSES, "all");
            } else {
                into.writeAttr(SUMO_ATTR_EXCEPTIONS, ss_exceptions);
            }
        }
        into.writeAttr(SUMO_ATTR_VALUE, stopOffset.getOffset());
        into.closeTag();
    }
}


/****************************************************************************/
