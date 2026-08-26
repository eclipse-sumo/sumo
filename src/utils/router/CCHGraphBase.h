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
/// @file    CCHGraphBase.h
/// @author  Pranav Sateesh
/// @date    2026
///
// Edge-graph <-> RoutingKit CCH graph mapping, shared between the
// simulation (CCHGraph over MSEdge) and duarouter (RODUACCHGraph over
// ROEdge) -- the GRAPH template argument of CCHRouter.
//
// RoutingKit node = one non-internal, non-taz edge (densely re-indexed).
// RoutingKit arc  = one edge-to-edge connection (u -> to) from
//                   getViaSuccessors, with any internal/via edge chain
//                   between them folded into the arc weight.
//
// The topology (tail/head/order/CCH) is metric-INDEPENDENT and built once
// from the class-union successor sets. Weights are metric-DEPENDENT
// centisecond integers recomputed by the derived classes from their effort
// functions; forbidden arcs get exactly RoutingKit::inf_weight so the arc
// set (topology) never changes.
//
// ARC WEIGHT CONVENTION (load-bearing):
//   w(u -> to) = round(100 * ( viaEffort(u..to) + effort(to) ))   [centiseconds]
// so a path u0->u1->...->uk sums arc weights = via + effort(u1..uk), and the
// FULL route cost equals  query_distance + effort(u0)  (the source edge's
// own effort, added back by the caller as the source seed). This is NOT
// round(100*recomputeCosts({u,to})): that would double-count effort(u) on
// every hop of a multi-edge path.
/****************************************************************************/
#pragma once
#include <config.h>

#include <cmath>
#include <limits>
#include <map>
#include <vector>
#include <utils/common/MsgHandler.h>
#include <utils/common/StdDefs.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/geom/Position.h>
#include <routingkit/customizable_contraction_hierarchy.h>
#include <routingkit/nested_dissection.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class CCHGraphBase
 * @brief Metric-independent RoutingKit CCH topology over the PURE road graph.
 *
 * Nodes are the non-internal, non-TAZ edges. TAZ connectors are deliberately
 * NOT nodes: a connector is a high-degree "star" (it links every member edge
 * of its zone), and eliminating a degree-d node in a contraction hierarchy
 * forms a d-clique => treewidth >= d, which poisons the whole hierarchy
 * regardless of elimination order. Instead, TAZ connectors are kept host-side
 * as query-time source/target SETS (the "phantom node" pattern) and resolved
 * with RoutingKit's native multi-source/multi-target query.
 *
 * Build once at start; hold a single owned instance and hand out a const
 * reference to every router clone (the CustomizableContractionHierarchy is
 * read-only after construction and safe to share across threads).
 *
 * @tparam E the edge type (MSEdge / ROEdge)
 * @tparam V the vehicle type of the effort callback (SUMOVehicle / ROVehicle)
 */
template<class E, class V>
class CCHGraphBase {
public:
    /// @brief effort callback signature, matching SUMOAbstractRouter::Operation
    typedef double (*EffortOperation)(const E* const, const V* const, double);

    /// @brief sentinel for "not a routable node"
    static const unsigned INVALID_NODE;

    /** @brief Build the union line graph + CCH from the given edges.
     *
     * Arcs come from getViaSuccessors(SVC_IGNORING) (the raw, unfiltered
     * successor set): the arc set must be class-independent so every class
     * (including e.g. tram-only rail arcs) exists. Per-class permissions are
     * applied later as inf_weight in the metric, never by removing arcs.
     */
    explicit CCHGraphBase(const std::vector<E*>& allEdges) {
        // 1. dense node indexing over real (non-internal, non-taz) edges.
        unsigned maxNumID = 0;
        for (const E* e : allEdges) {
            maxNumID = MAX2(maxNumID, (unsigned)e->getNumericalID());
        }
        myEdgeToNode.assign(maxNumID + 1, INVALID_NODE);
        for (const E* e : allEdges) {
            if (e->isInternal() || e->isTazConnector()) {
                continue;
            }
            const unsigned node = (unsigned)myNodeToEdge.size();
            myEdgeToNode[e->getNumericalID()] = node;
            myNodeToEdge.push_back(e);
        }
        const unsigned nNodes = (unsigned)myNodeToEdge.size();

        // 2. arcs from the union successors; coordinates for the orderer.
        std::vector<float> lon(nNodes);
        std::vector<float> lat(nNodes);
        for (unsigned n = 0; n < nNodes; n++) {
            const E* e = myNodeToEdge[n];
            Position p;
            if (!e->getLanes().empty()) {
                p = e->getLanes()[0]->geometryPositionAtOffset(e->getLength() * 0.5);
            } else {
                const auto* j = e->getToJunction() != nullptr ? e->getToJunction() : e->getFromJunction();
                p = (j != nullptr) ? j->getPosition() : Position(0., 0.);
            }
            if (GeoConvHelper::getFinal().usingGeoProjection()) {
                GeoConvHelper::getFinal().cartesian2geo(p);
            }
            lon[n] = (float)p.x();
            lat[n] = (float)p.y();
        }
        for (unsigned n = 0; n < nNodes; n++) {
            const E* u = myNodeToEdge[n];
            for (const auto& follower : u->getViaSuccessors(SVC_IGNORING)) {
                const E* to = follower.first;
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
        for (const E* via : myArcVia) {
            if (via != nullptr) {
                myHasVia = true;
                break;
            }
        }

        // 2b. reverse image for sparse re-customization: for every arc, the
        // edges its weight reads at fill time (head edge + folded via chain;
        // the tail edge is deliberately absent from the weight, see the arc
        // weight convention above)
        myEdgeToArcs.resize(myEdgeToNode.size());
        for (unsigned a = 0; a < (unsigned)myArcHead.size(); a++) {
            myEdgeToArcs[myNodeToEdge[myArcHead[a]]->getNumericalID()].push_back(a);
            const E* via = myArcVia[a];
            while (via != nullptr && via->isInternal()) {
                if (via->getNumericalID() >= 0 && via->getNumericalID() < (int)myEdgeToArcs.size()) {
                    myEdgeToArcs[via->getNumericalID()].push_back(a);
                }
                const auto& vs = via->getViaSuccessors();
                via = vs.empty() ? nullptr : vs.front().second;
            }
        }

        // 3. TAZ member sets (query-time "phantom nodes"): successors for a
        // source connector (entry edges), predecessors for a sink connector.
        unsigned nTazSrc = 0, nTazSnk = 0;
        for (const E* e : allEdges) {
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
            for (const E* pred : e->getPredecessors()) {
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
        // @todo persist the order keyed by a network hash (recompute is fast)
        std::vector<unsigned> order = RoutingKit::compute_nested_node_dissection_order_using_inertial_flow(
                                          nNodes, myArcTail, myArcHead, lat, lon);
        myCCH = RoutingKit::CustomizableContractionHierarchy(order, myArcTail, myArcHead);

        WRITE_MESSAGEF(TL("CCH: % road nodes, % arcs, % cch-arcs (fill x%), % TAZ sources, % TAZ sinks."),
                       toString(nNodes), toString(arcCount()),
                       toString(myCCH.cch_arc_count()),
                       toString((double)myCCH.cch_arc_count() / MAX2((unsigned)1, arcCount())),
                       toString(nTazSrc), toString(nTazSnk));
    }

    virtual ~CCHGraphBase() {}

    /// @brief the immutable CCH (share const& across clones)
    const RoutingKit::CustomizableContractionHierarchy& cch() const {
        return myCCH;
    }

    /// @brief number of RoutingKit nodes (== number of real edges)
    unsigned nodeCount() const {
        return (unsigned)myNodeToEdge.size();
    }

    /// @brief number of input arcs (== number of mapped connections)
    unsigned arcCount() const {
        return (unsigned)myArcTail.size();
    }

    /// @brief RoutingKit node index for an edge, or INVALID_NODE if not a node
    unsigned nodeOf(const E* e) const {
        if (e == nullptr || e->getNumericalID() < 0 || e->getNumericalID() >= (int)myEdgeToNode.size()) {
            return INVALID_NODE;
        }
        return myEdgeToNode[e->getNumericalID()];
    }

    /// @brief the edge backing a RoutingKit node
    const E* edgeOf(unsigned node) const {
        return myNodeToEdge[node];
    }

    /// @brief road member nodes of a TAZ-source connector (its entry edges),
    /// to seed multi-SOURCE queries; empty if @p taz is not a known TAZ source.
    const std::vector<unsigned>& tazSources(const E* taz) const {
        static const std::vector<unsigned> empty;
        const auto it = myTazSrcNodes.find(taz);
        return it == myTazSrcNodes.end() ? empty : it->second;
    }

    /// @brief road member nodes of a TAZ-sink connector (its exit edges),
    /// to seed multi-TARGET queries; empty if @p taz is not a known TAZ sink.
    const std::vector<unsigned>& tazSinks(const E* taz) const {
        static const std::vector<unsigned> empty;
        const auto it = myTazSnkNodes.find(taz);
        return it == myTazSnkNodes.end() ? empty : it->second;
    }

    /// @brief per-arc endpoints (size arcCount()), for CCH construction / metric
    const std::vector<unsigned>& tail() const {
        return myArcTail;
    }
    const std::vector<unsigned>& head() const {
        return myArcHead;
    }

    /// @brief the arcs whose weight depends on the given edge (arcs it heads
    /// plus arcs whose folded via chain contains it), for sparse
    /// re-customization; empty for edges that are not part of the graph
    const std::vector<unsigned>& arcsOfEdge(const E* e) const {
        static const std::vector<unsigned> empty;
        if (e == nullptr || e->getNumericalID() < 0 || e->getNumericalID() >= (int)myEdgeToArcs.size()) {
            return empty;
        }
        return myEdgeToArcs[e->getNumericalID()];
    }

    /** @brief Re-expand a RoutingKit node path into the full edge list,
     * re-inserting any folded internal/via edges. */
    void expandNodePath(const std::vector<unsigned>& nodePath,
                        std::vector<const E*>& into) const {
        into.reserve(into.size() + nodePath.size());
        // Fast path for nets without internal edges (e.g. --no-internal-links):
        // graph nodes ARE the edges, so the route is a direct id map -- no
        // per-hop getViaSuccessors lookup (which locks under multithreading).
        if (!myHasVia) {
            for (unsigned node : nodePath) {
                into.push_back(myNodeToEdge[node]);
            }
            return;
        }
        // Re-insert folded internal/via edges between consecutive real edges so
        // the returned route is a valid edge sequence.
        for (size_t i = 0; i < nodePath.size(); i++) {
            const E* e = myNodeToEdge[nodePath[i]];
            into.push_back(e);
            if (i + 1 < nodePath.size()) {
                const E* next = myNodeToEdge[nodePath[i + 1]];
                for (const auto& follower : e->getViaSuccessors(SVC_IGNORING)) {
                    if (follower.first == next) {
                        const E* via = follower.second;
                        while (via != nullptr && via->isInternal()) {
                            into.push_back(via);
                            const auto& vs = via->getViaSuccessors();
                            via = vs.empty() ? nullptr : vs.front().second;
                        }
                        break;
                    }
                }
            }
        }
    }

protected:
    /// @brief effort accumulated crossing the internal/via chain that leads from
    /// one real edge onto its successor (mirrors SUMOAbstractRouter::updateViaEdgeCost).
    static double viaChainEffort(const E* via, EffortOperation effort,
                                 const V* veh, double time) {
        double sum = 0.;
        while (via != nullptr && via->isInternal()) {
            sum += effort(via, veh, time);
            const auto& vs = via->getViaSuccessors();
            via = vs.empty() ? nullptr : vs.front().second;
        }
        return sum;
    }

    /** @brief The unmasked weight of one arc: via-chain effort + head-edge
     * effort, rounded to centiseconds and clamped below inf_weight (NaN /
     * inf / overflow become inf_weight = forbidden). Permission masking is
     * the derived class's business. */
    unsigned computeArcWeightRaw(unsigned a, EffortOperation effort,
                                 const V* veh, double time) const {
        const E* to = myNodeToEdge[myArcHead[a]];
        const double bigEffort = (double)(RoutingKit::inf_weight - 1) / 100.0;
        const double eff = viaChainEffort(myArcVia[a], effort, veh, time)
                           + effort(to, veh, time);
        if (!(eff < bigEffort)) {
            return RoutingKit::inf_weight;
        }
        const long long cs = std::llround(eff * 100.0);
        return (unsigned)(cs < 0 ? 0 : (cs >= (long long)RoutingKit::inf_weight
                                        ? RoutingKit::inf_weight - 1 : cs));
    }

    /** @brief Mark the arcs the given class may traverse -- exactly the arcs
     * getViaSuccessors(vClass) yields, i.e. CONNECTION-level permissions
     * (edge-level getPermissions() is too coarse on mixed-lane edges).
     * SVC_IGNORING allows everything. */
    void markClassAllowedArcs(SUMOVehicleClass vClass, std::vector<bool>& allowed) const {
        allowed.assign(arcCount(), vClass == SVC_IGNORING);
        if (vClass == SVC_IGNORING) {
            return;
        }
        for (unsigned n = 0; n < (unsigned)myNodeToEdge.size(); n++) {
            for (const auto& follower : myNodeToEdge[n]->getViaSuccessors(vClass)) {
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

protected:
    /// @brief node index -> backing edge
    std::vector<const E*> myNodeToEdge;
    /// @brief edge numerical id -> node index (INVALID_NODE if not a node)
    std::vector<unsigned> myEdgeToNode;
    /// @brief per-arc endpoints
    std::vector<unsigned> myArcTail;
    std::vector<unsigned> myArcHead;
    /// @brief per-arc leading via/internal edge (nullptr if none) for path re-expansion
    std::vector<const E*> myArcVia;
    /// @brief (tail node, head node) -> arc index
    std::map<std::pair<unsigned, unsigned>, unsigned> myArcOf;
    /// @brief edge numerical id -> arcs whose weight reads that edge (head +
    /// folded via edges); the reverse image of computeArcWeightRaw's inputs
    std::vector<std::vector<unsigned> > myEdgeToArcs;
    /// @brief TAZ-source connector edge -> its entry-edge road node ids
    std::map<const E*, std::vector<unsigned> > myTazSrcNodes;
    /// @brief TAZ-sink connector edge -> its exit-edge road node ids
    std::map<const E*, std::vector<unsigned> > myTazSnkNodes;
    /// @brief whether any arc folds a real internal/via edge chain. False for
    /// --no-internal-links nets: lets path expansion skip the per-hop
    /// getViaSuccessors lookup (which locks under multithreading).
    bool myHasVia;
    /// @brief the immutable hierarchy
    RoutingKit::CustomizableContractionHierarchy myCCH;

private:
    CCHGraphBase(const CCHGraphBase&) = delete;
    CCHGraphBase& operator=(const CCHGraphBase&) = delete;
};


template<class E, class V>
const unsigned CCHGraphBase<E, V>::INVALID_NODE = RoutingKit::invalid_id;
