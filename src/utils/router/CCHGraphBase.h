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
// The topology (arcs/order/CCH) is metric-INDEPENDENT and built once from
// the class-union successor sets. Weights are metric-DEPENDENT centisecond
// integers recomputed from the caller's effort function (fillInputWeights /
// computeArcWeight); forbidden arcs get exactly RoutingKit::inf_weight so
// the arc set (topology) never changes.
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
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <routingkit/customizable_contraction_hierarchy.h>
#include <routingkit/nested_dissection.h>
#pragma GCC diagnostic pop

// #define CCH_DEBUG


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
            // excluded are junction-internal edges and the district STAR
            // connectors ("<taz>-source"/"-sink", the high-degree phantom
            // edges -- see the class documentation). Legacy net edges with
            // function="connector" are ordinary routable geometry for the
            // exact routers and therefore stay ordinary nodes here; only the
            // paired district connectors (identified by their
            // otherTazConnector link) are kept out of the hierarchy.
            if (e->isInternal() || isStarConnector(e)) {
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
        myArcPerm.assign(myArcTail.size(), 0);
        myPrimedClasses = 0;
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
            if (!isStarConnector(e)) {
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
#ifdef CCH_DEBUG
        std::cout << "CCH: " << nNodes << " road nodes, "
                  << arcCount() << " arcs, "
                  << myCCH.cch_arc_count() << " cch-arcs (fill x" << (double)myCCH.cch_arc_count() / MAX2((unsigned)1, arcCount())
                  << "), " << nTazSrc<< " TAZ sources, " << nTazSnk << " TAZ sinks." << std::endl;
#endif
    }

    virtual ~CCHGraphBase() {}

    /// @brief the immutable CCH (share const& across clones)
    const RoutingKit::CustomizableContractionHierarchy& cch() const {
        return myCCH;
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

    /** @brief Map a RoutingKit node path back to the edge sequence.
     *
     * The result lists REAL edges only, exactly like the exact routers:
     * junction-internal edges are never part of SUMO route vectors -- their
     * cost is folded into the arc weights for the query and re-added from
     * consecutive real edges by SUMOAbstractRouter::recomputeCosts. */
    void expandNodePath(const std::vector<unsigned>& nodePath,
                        std::vector<const E*>& into) const {
        into.reserve(into.size() + nodePath.size());
        for (unsigned node : nodePath) {
            into.push_back(myNodeToEdge[node]);
        }
    }

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

public:
    /** @brief Recompute the input weight of one arc -- the exact per-arc body
     * of fillInputWeights (same masking, folding and rounding), exposed so a
     * sparse update can refresh only the arcs of edges that actually moved.
     *
     * Forbidden (inf_weight) if EITHER the connection does not permit the
     * class (static, connection-level, matches getViaSuccessors(vClass); see
     * primeClassMask) OR the destination edge does not currently permit the
     * class (dynamic, edge-level -- catches runtime closures via
     * MSLane::setPermissions; a no-op where permissions are static). */
    unsigned computeArcWeight(unsigned a, EffortOperation effort, SUMOVehicleClass maskClass,
                              const V* veh, double time) const {
        if (maskClass != SVC_IGNORING
                && (((myArcPerm[a] & maskClass) == 0)
                    || ((edgeOf(myArcHead[a])->getPermissions() & maskClass) == 0))) {
            return RoutingKit::inf_weight;
        }
        return computeArcWeightRaw(a, effort, veh, time);
    }

    /** @brief Fill a centisecond input-weight buffer for one vehicle class.
     *
     * Primes the class's connection mask on first use (see primeClassMask for
     * the synchronization contract), then applies computeArcWeight per arc.
     * @param[in] effort     the effort Operation
     * @param[in] maskClass  arcs whose connection or destination edge does NOT
     *   permit this class become inf_weight; SVC_IGNORING masks nothing
     * @param[in] veh    reference vehicle for the effort floor (may be null)
     * @param[in] time   seconds, passed to the effort fn
     * @param[out] weight resized to arcCount(); w[a] in [0, inf_weight]
     */
    void fillInputWeights(EffortOperation effort, SUMOVehicleClass maskClass,
                          const V* veh, double time,
                          std::vector<unsigned>& weight) const {
        primeClassMask(maskClass);
        weight.resize(arcCount());
        for (unsigned a = 0; a < arcCount(); a++) {
            weight[a] = computeArcWeight(a, effort, maskClass, veh, time);
        }
    }

private:
    /// @brief whether the edge is a district star connector (the paired
    /// "<taz>-source"/"-sink" phantom edge) as opposed to a legacy
    /// function="connector" net edge, which stays routable
    static bool isStarConnector(const E* e) {
        return e->isTazConnector() && e->getOtherTazConnector() != nullptr;
    }

    /** @brief Record which arcs @p vClass may traverse in the per-arc
     * CONNECTION-level permission bitmask -- exactly the arcs
     * getViaSuccessors(vClass) yields (edge-level getPermissions() is too
     * coarse on mixed-lane edges). No-op if the class was primed before.
     *
     * NOT internally synchronized: concurrent calls for an unprimed class
     * must be serialized by the caller. In practice the simulation primes on
     * the main thread at the customization barrier (no query in flight) and
     * duarouter primes under the RODUACCHMetrics mutex; primed classes make
     * this a lock-free bit test on the hot path. */
    void primeClassMask(SUMOVehicleClass vClass) const {
        if (vClass == SVC_IGNORING || (myPrimedClasses & vClass) == vClass) {
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
                    myArcPerm[it->second] |= (SVCPermissions)vClass;
                }
            }
        }
        myPrimedClasses |= (SVCPermissions)vClass;
    }

    /** @brief The unmasked weight of one arc: via-chain effort + head-edge
     * effort, rounded to centiseconds and clamped below inf_weight (NaN /
     * inf / overflow become inf_weight = forbidden). */
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
    /// @brief per-arc CONNECTION-level permission bitmask, accumulated per
    /// primed class (see primeClassMask); mutable lazy cache
    mutable std::vector<SVCPermissions> myArcPerm;
    /// @brief the classes already primed into myArcPerm
    mutable SVCPermissions myPrimedClasses;
    /// @brief edge numerical id -> arcs whose weight reads that edge (head +
    /// folded via edges); the reverse image of computeArcWeightRaw's inputs
    std::vector<std::vector<unsigned> > myEdgeToArcs;
    /// @brief TAZ-source connector edge -> its entry-edge road node ids
    std::map<const E*, std::vector<unsigned> > myTazSrcNodes;
    /// @brief TAZ-sink connector edge -> its exit-edge road node ids
    std::map<const E*, std::vector<unsigned> > myTazSnkNodes;
    /// @brief the immutable hierarchy
    RoutingKit::CustomizableContractionHierarchy myCCH;

private:
    CCHGraphBase(const CCHGraphBase&) = delete;
    CCHGraphBase& operator=(const CCHGraphBase&) = delete;
};


template<class E, class V>
const unsigned CCHGraphBase<E, V>::INVALID_NODE = RoutingKit::invalid_id;
