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
/// @file    RODUACCHGraph.cpp
/// @author  Pranav Sateesh
/// @date    2026
///
// ROEdge graph adapter for the CCHRouter template (see RODUACCHGraph.h).
/****************************************************************************/
#include <config.h>


#include "RODUACCHGraph.h"

#include <cmath>
#include <limits>
#include <router/ROEdge.h>
#include <router/ROLane.h>
#include <router/RONode.h>
#include <utils/geom/Position.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StdDefs.h>
#include <utils/common/ToString.h>
#include <routingkit/nested_dissection.h>

// ===========================================================================
// static member definitions
// ===========================================================================
const unsigned RODUACCHGraph::INVALID_NODE = RoutingKit::invalid_id;

const RODUACCHGraph* RODUACCHMetrics::myGraph = nullptr;
RODUACCHGraph::EffortOperation RODUACCHMetrics::myEffort = nullptr;
std::map<SUMOVehicleClass, RODUACCHMetrics::ClassMetric> RODUACCHMetrics::myMetrics;
std::mutex RODUACCHMetrics::myLock;


// ===========================================================================
// helpers
// ===========================================================================
namespace {
/// @brief effort accumulated crossing the internal/via chain that leads from
/// one real edge onto its successor (mirrors SUMOAbstractRouter::updateViaEdgeCost).
inline double
viaChainEffort(const ROEdge* via, RODUACCHGraph::EffortOperation effort,
               const ROVehicle* veh, double time) {
    double sum = 0.;
    while (via != nullptr && via->isInternal()) {
        sum += effort(via, veh, time);
        const ROConstEdgePairVector& vs = via->getViaSuccessors();
        via = vs.empty() ? nullptr : vs.front().second;
    }
    return sum;
}
} // namespace


// ===========================================================================
// RODUACCHGraph method definitions
// ===========================================================================
RODUACCHGraph::RODUACCHGraph(const std::vector<ROEdge*>& allEdges) {
    // 1. dense node indexing over real (non-internal, non-taz) edges.
    unsigned maxNumID = 0;
    for (const ROEdge* e : allEdges) {
        maxNumID = MAX2(maxNumID, (unsigned)e->getNumericalID());
    }
    myEdgeToNode.assign(maxNumID + 1, INVALID_NODE);
    for (const ROEdge* e : allEdges) {
        // Nodes are the PURE road graph: internal (junction geometry) edges
        // are folded into arc weights; TAZ connectors are excluded entirely
        // (a connector is a degree-d star whose elimination forms a d-clique
        // and poisons the hierarchy). TAZ routing happens at query time via
        // multi-source/target seeding of the member edges.
        if (e->isInternal() || e->isTazConnector()) {
            continue;
        }
        const unsigned node = (unsigned)myNodeToEdge.size();
        myEdgeToNode[e->getNumericalID()] = node;
        myNodeToEdge.push_back(e);
    }
    const unsigned nNodes = (unsigned)myNodeToEdge.size();

    // 2. arcs from the class-union successors; coordinates for the orderer.
    std::vector<float> lon(nNodes);
    std::vector<float> lat(nNodes);
    for (unsigned n = 0; n < nNodes; n++) {
        const ROEdge* e = myNodeToEdge[n];
        Position p;
        if (!e->getLanes().empty()) {
            p = e->getLanes()[0]->geometryPositionAtOffset(e->getLength() * 0.5);
        } else {
            const RONode* j = e->getToJunction() != nullptr ? e->getToJunction() : e->getFromJunction();
            p = (j != nullptr) ? j->getPosition() : Position(0., 0.);
        }
        if (GeoConvHelper::getFinal().usingGeoProjection()) {
            GeoConvHelper::getFinal().cartesian2geo(p);
        }
        lon[n] = (float)p.x();
        lat[n] = (float)p.y();
    }
    for (unsigned n = 0; n < nNodes; n++) {
        const ROEdge* u = myNodeToEdge[n];
        // UNION topology (SVC_IGNORING = raw unfiltered successors): the arc
        // set must be class-independent so every class exists. Per-class
        // permissions become inf_weight in the metric, never removed arcs.
        for (const auto& follower : u->getViaSuccessors(SVC_IGNORING)) {
            const ROEdge* to = follower.first;
            if (to == nullptr || to->getNumericalID() >= (int)myEdgeToNode.size()) {
                continue;
            }
            const unsigned toNode = myEdgeToNode[to->getNumericalID()];
            if (toNode == INVALID_NODE) {
                continue;  // successor is internal/taz -> not a routable node
            }
            myArcOf[std::make_pair(n, toNode)] = (unsigned)myArcTail.size();
            myArcTail.push_back(n);
            myArcHead.push_back(toNode);
            myArcVia.push_back(follower.second);  // leading internal edge or nullptr
        }
    }
    myHasVia = false;
    for (const ROEdge* via : myArcVia) {
        if (via != nullptr) {
            myHasVia = true;
            break;
        }
    }

    // 3. TAZ member sets (query-time "phantom nodes").
    unsigned nTazSrc = 0, nTazSnk = 0;
    for (const ROEdge* e : allEdges) {
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
        for (const ROEdge* pred : e->getPredecessors()) {
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
    std::vector<unsigned> order = RoutingKit::compute_nested_node_dissection_order_using_inertial_flow(
                                      nNodes, myArcTail, myArcHead, lat, lon);
    myCCH = RoutingKit::CustomizableContractionHierarchy(order, myArcTail, myArcHead);

    WRITE_MESSAGEF(TL("CCH: % road nodes, % arcs, % cch-arcs (fill x%), % TAZ sources, % TAZ sinks."),
                   toString(nNodes), toString(arcCount()),
                   toString(myCCH.cch_arc_count()),
                   toString((double)myCCH.cch_arc_count() / MAX2((unsigned)1, arcCount())),
                   toString(nTazSrc), toString(nTazSnk));
}


unsigned
RODUACCHGraph::nodeOf(const ROEdge* e) const {
    if (e == nullptr || e->getNumericalID() < 0 || e->getNumericalID() >= (int)myEdgeToNode.size()) {
        return INVALID_NODE;
    }
    return myEdgeToNode[e->getNumericalID()];
}


const std::vector<unsigned>&
RODUACCHGraph::tazSources(const ROEdge* taz) const {
    static const std::vector<unsigned> empty;
    const auto it = myTazSrcNodes.find(taz);
    return it == myTazSrcNodes.end() ? empty : it->second;
}


const std::vector<unsigned>&
RODUACCHGraph::tazSinks(const ROEdge* taz) const {
    static const std::vector<unsigned> empty;
    const auto it = myTazSnkNodes.find(taz);
    return it == myTazSnkNodes.end() ? empty : it->second;
}


void
RODUACCHGraph::fillInputWeights(EffortOperation effort, SUMOVehicleClass maskClass,
                                const ROVehicle* veh, double time,
                                std::vector<unsigned>& weight) const {
    // Which arcs may this class traverse? Walk getViaSuccessors(maskClass)
    // live -- exactly the connections the exact routers would relax.
    std::vector<bool> allowed(arcCount(), maskClass == SVC_IGNORING);
    if (maskClass != SVC_IGNORING) {
        for (unsigned n = 0; n < (unsigned)myNodeToEdge.size(); n++) {
            for (const auto& follower : myNodeToEdge[n]->getViaSuccessors(maskClass)) {
                const unsigned toNode = nodeOf(follower.first);
                if (toNode == INVALID_NODE) {
                    continue;
                }
                const auto it = myArcOf.find(std::make_pair(n, toNode));
                if (it != myArcOf.end()) {
                    allowed[it->second] = true;
                }
            }
        }
    }
    weight.resize(arcCount());
    const double bigEffort = (double)(RoutingKit::inf_weight - 1) / 100.0;
    for (unsigned a = 0; a < arcCount(); a++) {
        const ROEdge* to = myNodeToEdge[myArcHead[a]];
        if (!allowed[a] || (maskClass != SVC_IGNORING && (to->getPermissions() & maskClass) == 0)) {
            weight[a] = RoutingKit::inf_weight;
            continue;
        }
        // arc weight = via-chain effort + destination-edge effort (the same
        // convention as microsim's CCHGraph; the source edge's own effort is
        // added back per query as the source seed).
        double eff = viaChainEffort(myArcVia[a], effort, veh, time)
                     + effort(to, veh, time);
        if (!(eff < bigEffort)) {  // NaN / inf / overflow -> forbidden
            weight[a] = RoutingKit::inf_weight;
        } else {
            long long cs = std::llround(eff * 100.0);
            weight[a] = (unsigned)(cs < 0 ? 0 : (cs >= (long long)RoutingKit::inf_weight
                                                 ? RoutingKit::inf_weight - 1 : cs));
        }
    }
}


void
RODUACCHGraph::expandNodePath(const std::vector<unsigned>& nodePath,
                              std::vector<const ROEdge*>& into) const {
    into.reserve(into.size() + nodePath.size());
    // Fast path for nets without internal edges: graph nodes ARE the edges.
    if (!myHasVia) {
        for (unsigned node : nodePath) {
            into.push_back(myNodeToEdge[node]);
        }
        return;
    }
    // Re-insert folded internal/via edges between consecutive real edges.
    for (size_t i = 0; i < nodePath.size(); i++) {
        const ROEdge* e = myNodeToEdge[nodePath[i]];
        into.push_back(e);
        if (i + 1 < nodePath.size()) {
            const ROEdge* next = myNodeToEdge[nodePath[i + 1]];
            for (const auto& follower : e->getViaSuccessors(SVC_IGNORING)) {
                if (follower.first == next) {
                    const ROEdge* via = follower.second;
                    while (via != nullptr && via->isInternal()) {
                        into.push_back(via);
                        const ROConstEdgePairVector& vs = via->getViaSuccessors();
                        via = vs.empty() ? nullptr : vs.front().second;
                    }
                    break;
                }
            }
        }
    }
}


// ===========================================================================
// RODUACCHMetrics method definitions
// ===========================================================================
void
RODUACCHMetrics::init(const RODUACCHGraph* graph, RODUACCHGraph::EffortOperation effort) {
    myGraph = graph;
    myEffort = effort;
    myMetrics.clear();
}


const RoutingKit::CustomizableContractionHierarchyMetric*
RODUACCHMetrics::get(SUMOVehicleClass vClass) {
    if (myGraph == nullptr) {
        return nullptr;  // not initialised -> caller falls back to A*
    }
    // Lazy build on first query for a class (duarouter streams vehicles, so
    // classes are not known up front). Weights are static for the whole run,
    // so a metric is customized exactly once. The mutex also serialises
    // concurrent first queries from parallel routing threads; afterwards the
    // map is read-only for that key (references into std::map are stable).
    std::lock_guard<std::mutex> lock(myLock);
    auto it = myMetrics.find(vClass);
    if (it == myMetrics.end()) {
        ClassMetric& cm = myMetrics[vClass];
        myGraph->fillInputWeights(myEffort, vClass, nullptr, 0., cm.weights);
        cm.metric.reset(new RoutingKit::CustomizableContractionHierarchyMetric(
                            myGraph->cch(), cm.weights));
        cm.metric->customize();
        return cm.metric.get();
    }
    return it->second.metric.get();
}

