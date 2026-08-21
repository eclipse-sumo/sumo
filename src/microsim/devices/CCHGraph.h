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
/// @file    CCHGraph.h
/// @author  Pranav Sateesh
/// @date    2026
///
// SUMO edge-graph  <->  RoutingKit CCH graph mapping.
//
// RoutingKit node = one non-internal, non-taz SUMO edge (densely re-indexed).
// RoutingKit arc  = one allowed edge-to-edge connection (u -> to) from
//                   MSEdge::getViaSuccessors, with any internal/via edge
//                   chain between them folded into the arc weight.
//
// The topology (tail/head/coords/order/CCH) is metric-INDEPENDENT and built
// once. Weights are metric-DEPENDENT centisecond integers recomputed each
// customization from the live effort function; forbidden/closed arcs get
// exactly RoutingKit::inf_weight so the arc set (topology) never changes.
//
// ARC WEIGHT CONVENTION (load-bearing, unit-tested in M1-A):
//   w(u -> to) = round(100 * ( viaEffort(u..to) + effort(to) ))   [centiseconds]
// so a path u0->u1->...->uk sums arc weights = via + effort(u1..uk), and the
// FULL SUMO route cost equals  query_distance + effort(u0)  (the source edge's
// own effort, added by the caller). Equivalently
//   w(u -> to) = round(100 * ( recomputeCosts({u,to}) - effort(u) )).
// This is NOT round(100*recomputeCosts({u,to})): that double-counts effort(u)
// on every hop of a multi-edge path.
/****************************************************************************/
#pragma once
#include <config.h>

#ifdef HAVE_ROUTINGKIT

#include <string>
#include <vector>
#include <map>
#include <set>
#include <cstdint>
#include <utils/common/SUMOVehicleClass.h>
#include <routingkit/customizable_contraction_hierarchy.h>

class MSEdge;
class SUMOVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class CCHGraph
 * @brief Metric-independent RoutingKit CCH topology over the PURE road graph.
 *
 * Nodes are non-internal, non-TAZ SUMO edges. TAZ connectors are
 * deliberately NOT nodes: a connector is a high-degree "star" (it links every
 * member edge of its zone), and eliminating a degree-d node in a contraction
 * hierarchy forms a d-clique => treewidth >= d, which poisons the whole
 * hierarchy regardless of elimination order (measured: fill 5x -> 18x,
 * per-query 4.7us -> 211us; TAZ-last was worse still). This is a treewidth
 * theorem, not bad luck. Instead, TAZ connectors are kept host-side as
 * query-time source/target SETS (the OSRM "phantom node" pattern) and
 * resolved with RoutingKit's native multi-source/multi-target query (M0b:
 * 5.5us mean, p99 11us at real Geneva TAZ sizes).
 *
 * Build once at engine start; hold a single owned instance and hand out a
 * const reference to every router clone (the CustomizableContractionHierarchy
 * is read-only after construction and safe to share across threads).
 */
class CCHGraph {
public:
    /// @brief effort callback signature, matching SUMOAbstractRouter::Operation
    typedef double (*EffortOperation)(const MSEdge* const, const SUMOVehicle* const, double);

    /** @brief Build the union line graph + CCH + per-arc class permissions.
     * @param[in] classes  the vehicle classes present in the demand; a
     *   per-arc CONNECTION-level permission bitmask is precomputed for these
     *   so the metric can mask exactly what A* (getViaSuccessors(vClass))
     *   would -- edge-level getPermissions() is too coarse on mixed-lane edges.
     * @param[in] orderCacheFile  optional path to load/save the nested
     *   dissection order (the expensive one-time step); empty = recompute.
     */
    CCHGraph(const std::set<SUMOVehicleClass>& classes, const std::string& orderCacheFile = "");

    ~CCHGraph() {}

    /// @brief the immutable CCH (share const& across clones)
    const RoutingKit::CustomizableContractionHierarchy& cch() const { return myCCH; }

    /// @brief number of RoutingKit nodes (== number of real SUMO edges)
    unsigned nodeCount() const { return (unsigned)myNodeToEdge.size(); }

    /// @brief number of input arcs (== number of mapped connections)
    unsigned arcCount() const { return (unsigned)myArcTail.size(); }

    /// @brief RoutingKit node index for a SUMO edge, or INVALID_NODE if not a node
    unsigned nodeOf(const MSEdge* e) const;

    /// @brief road member nodes of a TAZ-source connector (its entry edges),
    /// to seed multi-SOURCE queries; empty if @p taz is not a known TAZ source.
    const std::vector<unsigned>& tazSources(const MSEdge* taz) const;

    /// @brief road member nodes of a TAZ-sink connector (its exit edges),
    /// to seed multi-TARGET queries; empty if @p taz is not a known TAZ sink.
    const std::vector<unsigned>& tazSinks(const MSEdge* taz) const;

    /// @brief the SUMO edge backing a RoutingKit node
    const MSEdge* edgeOf(unsigned node) const { return myNodeToEdge[node]; }

    /// @brief per-arc endpoints (size arcCount()), for CCH construction / metric
    const std::vector<unsigned>& tail() const { return myArcTail; }
    const std::vector<unsigned>& head() const { return myArcHead; }

    /// @brief the arcs whose weight depends on the given edge (arcs it heads
    /// plus arcs whose folded via chain contains it), for sparse
    /// re-customization; empty for edges that are not part of the graph
    const std::vector<unsigned>& arcsOfEdge(const MSEdge* e) const;

    /** @brief Recompute the input weight of one arc -- the exact per-arc body
     * of fillInputWeights (same masking, folding and rounding), exposed so a
     * sparse update can refresh only the arcs of edges that actually moved. */
    unsigned computeArcWeight(unsigned a, EffortOperation effort, SUMOVehicleClass maskClass,
                              const SUMOVehicle* veh, double time) const;

    /** @brief Fill a centisecond input-weight buffer for one vehicle class.
     * @param[in] effort     the effort Operation (getEffort, or getEffortBike for bikes)
     * @param[in] maskClass  arcs whose destination edge does NOT permit this
     *   class become inf_weight; SVC_IGNORING masks nothing. This is what makes
     *   a per-class metric class-correct AND closure-aware: a closure is a
     *   global permissions change (MSLane::setPermissions), so a closed edge
     *   simply stops permitting the class here and its arcs go to inf.
     * @param[in] veh    reference vehicle for the effort floor (may be null)
     * @param[in] time   seconds, passed to the effort fn
     * @param[out] weight resized to arcCount(); w[a] in [0, inf_weight]
     */
    void fillInputWeights(EffortOperation effort, SUMOVehicleClass maskClass,
                          const SUMOVehicle* veh, double time,
                          std::vector<unsigned>& weight) const;

    /** @brief Re-expand a RoutingKit node path into the full SUMO edge list,
     * re-inserting any folded internal/via edges. */
    void expandNodePath(const std::vector<unsigned>& nodePath,
                        std::vector<const MSEdge*>& into) const;

    /// @brief sentinel for "not a routable node"
    static const unsigned INVALID_NODE;

private:
    /// @brief compute or load the inertial-flow nested dissection order
    std::vector<unsigned> buildOrder(const std::string& cacheFile) const;

private:
    /// @brief node index -> backing SUMO edge
    std::vector<const MSEdge*> myNodeToEdge;
    /// @brief SUMO edge numerical id -> node index (INVALID_NODE if not a node)
    std::vector<unsigned> myEdgeToNode;
    /// @brief per-arc endpoints
    std::vector<unsigned> myArcTail;
    std::vector<unsigned> myArcHead;
    /// @brief per-arc leading via/internal edge (nullptr if none) for path re-expansion
    std::vector<const MSEdge*> myArcVia;
    /// @brief per-arc CONNECTION-level permission bitmask (which classes may
    /// traverse this connection, at build-time permissions). Matches A*'s
    /// getViaSuccessors(vClass); dynamic closures are AND'd in at fill time.
    std::vector<SVCPermissions> myArcPerm;
    /// @brief edge numerical id -> arcs whose weight reads that edge (head +
    /// folded via edges); the reverse image of computeArcWeight's inputs
    std::vector<std::vector<unsigned> > myEdgeToArcs;
    /// @brief TAZ-source connector edge -> its entry-edge road node ids
    std::map<const MSEdge*, std::vector<unsigned> > myTazSrcNodes;
    /// @brief TAZ-sink connector edge -> its exit-edge road node ids
    std::map<const MSEdge*, std::vector<unsigned> > myTazSnkNodes;
    /// @brief node coordinates (lon/lat) for the inertial-flow orderer
    std::vector<float> myLon;
    std::vector<float> myLat;
    /// @brief whether any arc folds a real internal/via edge chain. False for
    /// --no-internal-links nets (Geneva): lets path expansion skip the
    /// per-hop getViaSuccessors lookup (which locks under multithreading).
    bool myHasVia;
    /// @brief the immutable hierarchy
    RoutingKit::CustomizableContractionHierarchy myCCH;

private:
    CCHGraph(const CCHGraph&) = delete;
    CCHGraph& operator=(const CCHGraph&) = delete;
};

#endif // HAVE_ROUTINGKIT
