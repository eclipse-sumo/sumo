/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    CCHGraph.cpp
/// @author  Pranav Sateesh
/// @date    2026
///
// SUMO edge-graph <-> RoutingKit CCH graph mapping (see CCHGraph.h).
/****************************************************************************/
#include <config.h>


#include "CCHGraph.h"

#include <cmath>
#include <limits>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSJunction.h>
#include <utils/geom/Position.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/common/MsgHandler.h>
#include <routingkit/nested_dissection.h>

// ===========================================================================
// static member definitions
// ===========================================================================
const unsigned CCHGraph::INVALID_NODE = RoutingKit::invalid_id;


// ===========================================================================
// helpers
// ===========================================================================
namespace {
/// @brief effort accumulated crossing the internal/via chain that leads from
/// one real edge onto its successor (mirrors SUMOAbstractRouter::updateViaEdgeCost).
inline double
viaChainEffort(const MSEdge* via, CCHGraph::EffortOperation effort,
               const SUMOVehicle* veh, double time) {
    double sum = 0.;
    while (via != nullptr && via->isInternal()) {
        sum += effort(via, veh, time);
        const MSConstEdgePairVector& vs = via->getViaSuccessors();
        via = vs.empty() ? nullptr : vs.front().second;
    }
    return sum;
}
} // namespace


// ===========================================================================
// method definitions
// ===========================================================================
CCHGraph::CCHGraph(const std::set<SUMOVehicleClass>& classes, const std::string& orderCacheFile) {
    const MSEdgeVector& allEdges = MSEdge::getAllEdges();

    // 1. dense node indexing over real (non-internal, non-taz) edges.
    unsigned maxNumID = 0;
    for (const MSEdge* e : allEdges) {
        maxNumID = MAX2(maxNumID, (unsigned)e->getNumericalID());
    }
    myEdgeToNode.assign(maxNumID + 1, INVALID_NODE);
    for (const MSEdge* e : allEdges) {
        // Nodes are the PURE road graph: internal (junction geometry) edges
        // are folded into arc weights; TAZ connectors are excluded entirely
        // (see class doc -- a connector is a degree-d star whose elimination
        // forms a d-clique and poisons the hierarchy). TAZ routing is handled
        // at query time via multi-source/target seeding of the member edges.
        if (e->isInternal() || e->isTazConnector()) {
            continue;
        }
        const unsigned node = (unsigned)myNodeToEdge.size();
        myEdgeToNode[e->getNumericalID()] = node;
        myNodeToEdge.push_back(e);
    }
    const unsigned nNodes = (unsigned)myNodeToEdge.size();

    // 2. arcs from allowed connections; coordinates for the orderer.
    myLon.resize(nNodes);
    myLat.resize(nNodes);
    for (unsigned n = 0; n < nNodes; n++) {
        const MSEdge* e = myNodeToEdge[n];
        Position p;
        if (!e->getLanes().empty()) {
            p = e->getLanes()[0]->geometryPositionAtOffset(e->getLength() * 0.5);
        } else {
            // TAZ connectors have no lane geometry; use the junction they
            // attach to (only one side is set for source/sink connectors).
            const MSJunction* j = e->getToJunction() != nullptr ? e->getToJunction() : e->getFromJunction();
            p = (j != nullptr) ? j->getPosition() : Position(0., 0.);
        }
        if (GeoConvHelper::getFinal().usingGeoProjection()) {
            GeoConvHelper::getFinal().cartesian2geo(p);
        }
        myLon[n] = (float)p.x();
        myLat[n] = (float)p.y();
    }
    for (unsigned n = 0; n < nNodes; n++) {
        const MSEdge* u = myNodeToEdge[n];
        // UNION topology (SVC_IGNORING = raw unfiltered successors): the CCH
        // arc set must be class-independent so every class (incl. tram-only
        // rail arcs, bus/truck lanes) exists. Per-class permissions are
        // applied later as inf_weight in the metric, never by removing arcs.
        for (const auto& follower : u->getViaSuccessors(SVC_IGNORING)) {
            const MSEdge* to = follower.first;
            if (to == nullptr || to->getNumericalID() >= (int)myEdgeToNode.size()) {
                continue;
            }
            const unsigned toNode = myEdgeToNode[to->getNumericalID()];
            if (toNode == INVALID_NODE) {
                continue;  // successor is internal/taz -> not a routable node
            }
            myArcTail.push_back(n);
            myArcHead.push_back(toNode);
            myArcVia.push_back(follower.second);  // leading internal edge or nullptr
        }
    }
    myHasVia = false;
    for (const MSEdge* via : myArcVia) {
        if (via != nullptr) {
            myHasVia = true;
            break;
        }
    }

    // 2a-bis. reverse image for sparse re-customization: for every arc, the
    // edges its weight reads at fill time (head edge + folded via chain; the
    // tail edge is deliberately absent from the weight, see the arc weight
    // convention in CCHGraph.h)
    myEdgeToArcs.resize(myEdgeToNode.size());
    for (unsigned a = 0; a < (unsigned)myArcHead.size(); a++) {
        myEdgeToArcs[myNodeToEdge[myArcHead[a]]->getNumericalID()].push_back(a);
        const MSEdge* via = myArcVia[a];
        while (via != nullptr && via->isInternal()) {
            if (via->getNumericalID() >= 0 && via->getNumericalID() < (int)myEdgeToArcs.size()) {
                myEdgeToArcs[via->getNumericalID()].push_back(a);
            }
            const MSConstEdgePairVector& vs = via->getViaSuccessors();
            via = vs.empty() ? nullptr : vs.front().second;
        }
    }

    // 2b. per-arc CONNECTION-level permissions: for each present class, mark
    // the arcs whose connection that class may traverse -- exactly the arcs
    // getViaSuccessors(vClass) would return. Edge-level getPermissions() is too
    // coarse on mixed-lane edges (a car edge with a bus-only connection), which
    // is what made passenger routes drift from A*. Built once at startup
    // permissions; runtime closures are AND'd in live at fill time.
    myArcPerm.assign(myArcTail.size(), 0);
    std::map<std::pair<unsigned, unsigned>, unsigned> arcOf;
    for (unsigned a = 0; a < (unsigned)myArcTail.size(); a++) {
        arcOf[std::make_pair(myArcTail[a], myArcHead[a])] = a;
    }
    for (const SUMOVehicleClass vc : classes) {
        for (unsigned n = 0; n < nNodes; n++) {
            const MSEdge* u = myNodeToEdge[n];
            for (const auto& follower : u->getViaSuccessors(vc)) {
                const unsigned toNode = nodeOf(follower.first);
                if (toNode == INVALID_NODE) {
                    continue;
                }
                const auto it = arcOf.find(std::make_pair(n, toNode));
                if (it != arcOf.end()) {
                    myArcPerm[it->second] |= (SVCPermissions)vc;
                }
            }
        }
    }

    // 3. TAZ member sets (host-side "phantom nodes"): for each TAZ connector,
    // record the road nodes it links -- successors for a source connector
    // (entry edges), predecessors for a sink connector (exit edges). A given
    // connector is one or the other; we store both and use the relevant one.
    unsigned nTazSrc = 0, nTazSnk = 0;
    for (const MSEdge* e : allEdges) {
        if (!e->isTazConnector()) {
            continue;
        }
        std::vector<unsigned> srcNodes;
        for (const auto& follower : e->getViaSuccessors(SVC_IGNORING)) {
            const unsigned m = nodeOf(follower.first);
            if (m != INVALID_NODE) {
                srcNodes.push_back(m);
            }
        }
        std::vector<unsigned> snkNodes;
        for (const MSEdge* pred : e->getPredecessors()) {
            const unsigned m = nodeOf(pred);
            if (m != INVALID_NODE) {
                snkNodes.push_back(m);
            }
        }
        if (!srcNodes.empty()) {
            myTazSrcNodes[e] = std::move(srcNodes);
            nTazSrc++;
        }
        if (!snkNodes.empty()) {
            myTazSnkNodes[e] = std::move(snkNodes);
            nTazSnk++;
        }
    }

    // 4. metric-independent order + CCH topology (the one-time work).
    std::vector<unsigned> order = buildOrder(orderCacheFile);
    myCCH = RoutingKit::CustomizableContractionHierarchy(order, myArcTail, myArcHead);

    WRITE_MESSAGEF(TL("CCH: % road nodes, % arcs, % cch-arcs (fill x%), % TAZ sources, % TAZ sinks."),
                   toString(nNodes), toString(arcCount()),
                   toString(myCCH.cch_arc_count()),
                   toString((double)myCCH.cch_arc_count() / MAX2((unsigned)1, arcCount())),
                   toString(nTazSrc), toString(nTazSnk));
}


const std::vector<unsigned>&
CCHGraph::tazSources(const MSEdge* taz) const {
    static const std::vector<unsigned> empty;
    const auto it = myTazSrcNodes.find(taz);
    return it == myTazSrcNodes.end() ? empty : it->second;
}


const std::vector<unsigned>&
CCHGraph::tazSinks(const MSEdge* taz) const {
    static const std::vector<unsigned> empty;
    const auto it = myTazSnkNodes.find(taz);
    return it == myTazSnkNodes.end() ? empty : it->second;
}


std::vector<unsigned>
CCHGraph::buildOrder(const std::string& /*cacheFile*/) const {
    // TODO(cch): persist keyed by a network hash; recompute is ~150ms so fine for now.
    return RoutingKit::compute_nested_node_dissection_order_using_inertial_flow(
               (unsigned)myNodeToEdge.size(), myArcTail, myArcHead, myLat, myLon);
}


unsigned
CCHGraph::nodeOf(const MSEdge* e) const {
    if (e == nullptr || e->getNumericalID() < 0 || e->getNumericalID() >= (int)myEdgeToNode.size()) {
        return INVALID_NODE;
    }
    return myEdgeToNode[e->getNumericalID()];
}


const std::vector<unsigned>&
CCHGraph::arcsOfEdge(const MSEdge* e) const {
    static const std::vector<unsigned> empty;
    if (e == nullptr || e->getNumericalID() < 0 || e->getNumericalID() >= (int)myEdgeToArcs.size()) {
        return empty;
    }
    return myEdgeToArcs[e->getNumericalID()];
}


unsigned
CCHGraph::computeArcWeight(unsigned a, EffortOperation effort, SUMOVehicleClass maskClass,
                           const SUMOVehicle* veh, double time) const {
    const MSEdge* to = myNodeToEdge[myArcHead[a]];
    // Per-class / closure gate. Forbidden (inf_weight) if EITHER:
    //  - the connection does not permit the class (static, connection-level,
    //    matches A*'s getViaSuccessors(vClass)); OR
    //  - the destination edge does not currently permit the class (dynamic,
    //    edge-level -- catches runtime closures via MSLane::setPermissions).
    if (maskClass != SVC_IGNORING && (((myArcPerm[a] & maskClass) == 0)
                                      || ((to->getPermissions() & maskClass) == 0))) {
        return RoutingKit::inf_weight;
    }
    // arc weight = via-chain effort + destination-edge effort (see CCHGraph.h).
    const double bigEffort = (double)(RoutingKit::inf_weight - 1) / 100.0;
    double eff = viaChainEffort(myArcVia[a], effort, veh, time)
                 + effort(to, veh, time);
    if (!(eff < bigEffort)) {  // NaN / inf / overflow -> forbidden
        return RoutingKit::inf_weight;
    }
    long long cs = std::llround(eff * 100.0);
    return (unsigned)(cs < 0 ? 0 : (cs >= (long long)RoutingKit::inf_weight
                                    ? RoutingKit::inf_weight - 1 : cs));
}


void
CCHGraph::fillInputWeights(EffortOperation effort, SUMOVehicleClass maskClass,
                           const SUMOVehicle* veh, double time,
                           std::vector<unsigned>& weight) const {
    weight.resize(arcCount());
    for (unsigned a = 0; a < arcCount(); a++) {
        weight[a] = computeArcWeight(a, effort, maskClass, veh, time);
    }
}


void
CCHGraph::expandNodePath(const std::vector<unsigned>& nodePath,
                         std::vector<const MSEdge*>& into) const {
    into.reserve(into.size() + nodePath.size());
    // Fast path for nets without internal edges (e.g. --no-internal-links):
    // graph nodes ARE the SUMO edges, so the route is a direct id map -- no
    // per-hop getViaSuccessors lookup (which would lock under multithreading).
    if (!myHasVia) {
        for (unsigned node : nodePath) {
            into.push_back(myNodeToEdge[node]);
        }
        return;
    }
    // Re-insert folded internal/via edges between consecutive real edges so
    // the returned route is a valid SUMO edge sequence.
    for (size_t i = 0; i < nodePath.size(); i++) {
        const MSEdge* e = myNodeToEdge[nodePath[i]];
        into.push_back(e);
        if (i + 1 < nodePath.size()) {
            const MSEdge* next = myNodeToEdge[nodePath[i + 1]];
            for (const auto& follower : e->getViaSuccessors(SVC_IGNORING)) {
                if (follower.first == next) {
                    const MSEdge* via = follower.second;
                    while (via != nullptr && via->isInternal()) {
                        into.push_back(via);
                        const MSConstEdgePairVector& vs = via->getViaSuccessors();
                        via = vs.empty() ? nullptr : vs.front().second;
                    }
                    break;
                }
            }
        }
    }
}

