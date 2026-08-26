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
/// @file    RODUACCHGraph.h
/// @author  Pranav Sateesh
/// @date    2026
///
// ROEdge graph adapter for the CCHRouter template (duarouter side).
//
// RODUACCHGraph is the duarouter counterpart of microsim's CCHGraph: it maps
// the ROEdge graph onto a RoutingKit Customizable Contraction Hierarchy.
// RoutingKit node = one non-internal, non-taz ROEdge (densely re-indexed);
// RoutingKit arc = one edge-to-edge connection from getViaSuccessors, with
// any internal/via chain folded into the arc weight. TAZ connectors are kept
// out of the hierarchy (a degree-d star forces treewidth >= d) and resolved
// as query-time multi-source/target seed sets. It provides the GRAPH
// interface documented in utils/router/CCHRouter.h.
//
// RODUACCHMetrics is the per-class metric store. duarouter streams vehicles,
// so the set of vehicle classes is not known up front: metrics are built
// lazily on first query for a class (mutex-guarded; ~ms each), all sharing
// the one weight-independent topology. Static-weight case only -- duarouter
// runs with time-independent efforts unless weight-files are loaded, which
// the CCH branch in duarouter_main rejects.
/****************************************************************************/
#pragma once
#include <config.h>


#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>
#include <utils/common/SUMOVehicleClass.h>
#include <routingkit/customizable_contraction_hierarchy.h>

class ROEdge;
class ROVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RODUACCHGraph
 * @brief Metric-independent RoutingKit CCH topology over the ROEdge graph.
 */
class RODUACCHGraph {
public:
    /// @brief effort callback signature (matches SUMOAbstractRouter::Operation)
    typedef double (*EffortOperation)(const ROEdge* const, const ROVehicle* const, double);

    /// @brief Build the line graph + nested dissection order + CCH (once).
    RODUACCHGraph(const std::vector<ROEdge*>& allEdges);

    ~RODUACCHGraph() {}

    /// @brief the immutable CCH (shared by all metrics)
    const RoutingKit::CustomizableContractionHierarchy& cch() const {
        return myCCH;
    }

    /// @brief number of input arcs (== number of mapped connections)
    unsigned arcCount() const {
        return (unsigned)myArcTail.size();
    }

    /// @brief RoutingKit node index for an edge, or INVALID_NODE if not a node
    unsigned nodeOf(const ROEdge* e) const;

    /// @brief the edge backing a RoutingKit node
    const ROEdge* edgeOf(unsigned node) const {
        return myNodeToEdge[node];
    }

    /// @brief road member nodes of a TAZ-source connector (its entry edges)
    const std::vector<unsigned>& tazSources(const ROEdge* taz) const;

    /// @brief road member nodes of a TAZ-sink connector (its exit edges)
    const std::vector<unsigned>& tazSinks(const ROEdge* taz) const;

    /** @brief Fill a centisecond input-weight buffer for one vehicle class.
     *
     * Arcs whose connection is not returned by getViaSuccessors(maskClass)
     * (or whose destination edge does not permit the class) become
     * inf_weight, so the arc set never changes; everything else is
     * round(100 * (viaChainEffort + effort(to))). @p veh is the reference
     * vehicle (nullptr => plain edge speeds).
     */
    void fillInputWeights(EffortOperation effort, SUMOVehicleClass maskClass,
                          const ROVehicle* veh, double time,
                          std::vector<unsigned>& weight) const;

    /// @brief Re-expand a RoutingKit node path into the full edge list,
    /// re-inserting any folded internal/via edges.
    void expandNodePath(const std::vector<unsigned>& nodePath,
                        std::vector<const ROEdge*>& into) const;

    /// @brief sentinel for "not a routable node"
    static const unsigned INVALID_NODE;

private:
    /// @brief node index -> backing edge
    std::vector<const ROEdge*> myNodeToEdge;
    /// @brief edge numerical id -> node index (INVALID_NODE if not a node)
    std::vector<unsigned> myEdgeToNode;
    /// @brief per-arc endpoints
    std::vector<unsigned> myArcTail;
    std::vector<unsigned> myArcHead;
    /// @brief per-arc leading via/internal edge (nullptr if none)
    std::vector<const ROEdge*> myArcVia;
    /// @brief (tail, head) -> arc index, for the per-class permission walk
    std::map<std::pair<unsigned, unsigned>, unsigned> myArcOf;
    /// @brief TAZ-source connector edge -> its entry-edge road node ids
    std::map<const ROEdge*, std::vector<unsigned> > myTazSrcNodes;
    /// @brief TAZ-sink connector edge -> its exit-edge road node ids
    std::map<const ROEdge*, std::vector<unsigned> > myTazSnkNodes;
    /// @brief whether any arc folds a real internal/via edge chain
    bool myHasVia;
    /// @brief the immutable hierarchy
    RoutingKit::CustomizableContractionHierarchy myCCH;

private:
    RODUACCHGraph(const RODUACCHGraph&) = delete;
    RODUACCHGraph& operator=(const RODUACCHGraph&) = delete;
};


/**
 * @class RODUACCHMetrics
 * @brief Lazy per-vehicle-class CCH metric store for duarouter.
 *
 * The metric provider handed to CCHRouter (a plain function pointer). On the
 * first query for a class, the metric is customized under a mutex from the
 * graph's fillInputWeights and cached for the rest of the run (weights are
 * static). RoutingKit metrics BORROW their input-weight buffer, so buffer
 * and metric live together in the map (std::map nodes are address-stable).
 */
class RODUACCHMetrics {
public:
    /// @brief install the shared graph + effort function (call once, before routing)
    static void init(const RODUACCHGraph* graph, RODUACCHGraph::EffortOperation effort);

    /// @brief the metric for a class, built on first use; matches
    /// CCHRouter::MetricProvider so it can be passed as the provider.
    static const RoutingKit::CustomizableContractionHierarchyMetric* get(SUMOVehicleClass vClass);

private:
    struct ClassMetric {
        std::vector<unsigned> weights;
        std::unique_ptr<RoutingKit::CustomizableContractionHierarchyMetric> metric;
    };
    static const RODUACCHGraph* myGraph;
    static RODUACCHGraph::EffortOperation myEffort;
    static std::map<SUMOVehicleClass, ClassMetric> myMetrics;
    static std::mutex myLock;
};

