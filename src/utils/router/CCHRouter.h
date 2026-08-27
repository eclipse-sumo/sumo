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
/// @file    CCHRouter.h
/// @author  Pranav Sateesh
/// @date    2026
///
// A SUMOAbstractRouter that answers queries via a RoutingKit Customizable
// Contraction Hierarchy over the edge graph. The metric is re-customized
// out-of-band (MSRoutingEngine::adaptEdgeEfforts) and published as a
// shared_ptr snapshot; each query snapshots the current metric, so a
// publish that lands mid-query is safe (double buffer).
//
// Following the AStarRouter<E, V, M> lookup-table pattern, the graph-mapping
// class is the GRAPH template parameter (the simulation supplies its concrete
// mapper, e.g. microsim's CCHGraph), so this header carries no simulation
// dependencies. GRAPH must provide:
//   unsigned nodeOf(const E*)                          (INVALID_NODE if not a node)
//   const E* edgeOf(unsigned node)
//   const std::vector<unsigned>& tazSources(const E*)  (entry-edge node sets)
//   const std::vector<unsigned>& tazSinks(const E*)    (exit-edge node sets)
//   void expandNodePath(const std::vector<unsigned>&, std::vector<const E*>&)
//   static const unsigned INVALID_NODE
//
// Every vehicle class routes on CCH via its own published metric. Closures are
// permission changes the per-class metric already encodes as inf_weight, so
// they need no query-time handling. The embedded A* fallback is used only when
// a class has no metric yet or a prohibition is NOT expressible as a live
// permission closure (an arbitrary per-query prohibition, which no shared CCH
// metric can represent) -- see compute() / prohibitionsCoveredByMetric().
/****************************************************************************/
#pragma once
#include <config.h>


#include <memory>
#include <vector>
#include <cmath>
#include <utils/common/SUMOTime.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/router/SUMOAbstractRouter.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <routingkit/customizable_contraction_hierarchy.h>
#pragma GCC diagnostic pop


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class CCHRouter
 * @brief Contraction-hierarchy router over the edge graph mapped by GRAPH.
 */
template<class E, class V, class GRAPH>
class CCHRouter : public SUMOAbstractRouter<E, V> {
public:
    typedef const RoutingKit::CustomizableContractionHierarchyMetric* MetricPtr;
    /// @brief supplies the metric for a vehicle class at a query time, or
    /// nullptr => fall back. The simulation ignores time and vehicle (its
    /// single metric tracks the live speeds); duarouter keys one metric per
    /// (class, restriction profile, weight period) with them.
    typedef MetricPtr(*MetricProvider)(SUMOVehicleClass, SUMOTime, const V*);
    /// @brief the end of the weight period containing the given time
    /// (SUMOTime_MAX = weights are static). Lets compute() detect queries
    /// whose trip crosses into the next period and re-query on that
    /// period's metric (see the boundary handling in compute()).
    typedef SUMOTime(*PeriodEnd)(SUMOTime);
    typedef typename SUMOAbstractRouter<E, V>::Operation Operation;
    typedef typename SUMOAbstractRouter<E, V>::Prohibitions Prohibitions;

    /** @brief Constructor.
     * @param[in] graph     the shared immutable CCH topology mapper (not owned)
     * @param[in] provider  static accessor for the published metric snapshot
     * @param[in] operation the effort callback (same one A-star and CH use)
     * @param[in] fallback  embedded router for non-CCH cases (OWNED)
     */
    CCHRouter(const GRAPH* graph, MetricProvider provider,
              Operation operation, SUMOAbstractRouter<E, V>* fallback,
              PeriodEnd periodEnd = nullptr) :
        SUMOAbstractRouter<E, V>("CCHRouter", true, operation, nullptr, false, false),
        myGraph(graph), myMetricProvider(provider), myFallback(fallback),
        myPeriodEnd(periodEnd), myProhibitionActive(false) {
    }

    /// @brief clone constructor: share graph + provider, clone the fallback, fresh query scratch
    CCHRouter(CCHRouter* other) :
        SUMOAbstractRouter<E, V>(other),
        myGraph(other->myGraph), myMetricProvider(other->myMetricProvider),
        myFallback(other->myFallback->clone()), myPeriodEnd(other->myPeriodEnd),
        myProhibitionActive(other->myProhibitionActive), myProhibited(other->myProhibited) {
    }

    virtual ~CCHRouter() {
        delete myFallback;
    }

    virtual SUMOAbstractRouter<E, V>* clone() {
        return new CCHRouter<E, V, GRAPH>(this);
    }

    bool compute(const E* from, const E* to, const V* const vehicle,
                 SUMOTime msTime, std::vector<const E*>& into, bool silent = false) {
        // A CCH query can only avoid an edge the metric already encodes as inf:
        // RoutingKit's query has no per-arc blacklist, and a query-time skip is
        // unsound because shortcut arcs bake in shortest paths THROUGH the edge
        // (SUMO's own CHRouter refuses prohibitions for the same reason). So we
        // run CCH when the metric can express the request and fall back to the
        // exact A* router otherwise: (a) no metric for this class, or (b) an
        // active prohibition that is NOT already a live permission-closure this
        // class's metric encodes (prohibitionsCoveredByMetric).
        const SUMOVehicleClass vClass = vehicle == nullptr ? SVC_PASSENGER : vehicle->getVClass();
        MetricPtr metric = myMetricProvider(vClass, msTime, vehicle);
        if (metric == nullptr || (myProhibitionActive && !prohibitionsCoveredByMetric(vClass))) {
            return myFallback->compute(from, to, vehicle, msTime, into, silent);
        }
        // Endpoints -> node SETS: a district star connector expands to its
        // member edges (phantom-node seeding); everything that is a graph
        // node -- including legacy function="connector" net edges -- routes
        // as a single node. TAZ member edges are the class-union set, so
        // filter to edges this class may enter.
        const bool fromTaz = from->isTazConnector() && myGraph->nodeOf(from) == GRAPH::INVALID_NODE;
        const bool toTaz = to->isTazConnector() && myGraph->nodeOf(to) == GRAPH::INVALID_NODE;
        std::vector<unsigned> srcBuf, tgtBuf;
        const std::vector<unsigned>* sources;
        const std::vector<unsigned>* targets;
        if (fromTaz) {
            for (const unsigned m : myGraph->tazSources(from)) {
                if ((myGraph->edgeOf(m)->getPermissions() & vClass) != 0) {
                    srcBuf.push_back(m);
                }
            }
            sources = &srcBuf;
        } else {
            const unsigned s = myGraph->nodeOf(from);
            if (s != GRAPH::INVALID_NODE) {
                srcBuf.push_back(s);
            }
            sources = &srcBuf;
        }
        if (toTaz) {
            for (const unsigned m : myGraph->tazSinks(to)) {
                if ((myGraph->edgeOf(m)->getPermissions() & vClass) != 0) {
                    tgtBuf.push_back(m);
                }
            }
            targets = &tgtBuf;
        } else {
            const unsigned t = myGraph->nodeOf(to);
            if (t != GRAPH::INVALID_NODE) {
                tgtBuf.push_back(t);
            }
            targets = &tgtBuf;
        }
        if (sources->empty() || targets->empty()) {
            // unmappable endpoint (e.g. internal edge, or a TAZ with no member
            // in this class's graph) -> exact fallback.
            return myFallback->compute(from, to, vehicle, msTime, into, silent);
        }

        this->startQuery();
        const double t = STEPS2TIME(msTime);
        if (!runQuery(metric, *sources, *targets, vehicle, t)) {
            this->endQuery(0);
            if (!silent && this->myErrorMsgHandler != nullptr) {
                this->myErrorMsgHandler->informf(TL("No connection between edge '%' and edge '%' found."),
                                                 from->getID(), to->getID());
            }
            return false;
        }
        std::vector<const E*> path;
        buildPath(from, to, fromTaz, toTaz, path);
        int visited = (int)path.size();
        // Weight-period boundary: the metric of the depart period is exact as
        // long as the trip finishes inside that period (efforts are constant
        // within it, and no time-propagated optimum can cross the boundary if
        // this trip does not: any path entering the next period costs at
        // least the remaining period, which already exceeds this result).
        // When the trip does cross, ALSO query the next period's metric and
        // keep whichever path is cheaper under the true time-propagating
        // walk (recomputeCosts). Exact for the common single-crossing case
        // where the optimum follows one period's structure; trips spanning
        // several periods keep the better of the two candidates.
        if (myPeriodEnd != nullptr && myQuery.get_distance() > 0) {
            const SUMOTime boundary = myPeriodEnd(msTime);
            if (boundary != SUMOTime_MAX
                    && msTime + TIME2STEPS((double)myQuery.get_distance() / 100.) >= boundary) {
                MetricPtr altMetric = myMetricProvider(vClass, boundary, vehicle);
                if (altMetric != nullptr && altMetric != metric
                        && runQuery(altMetric, *sources, *targets, vehicle, t)) {
                    std::vector<const E*> altPath;
                    buildPath(from, to, fromTaz, toTaz, altPath);
                    visited += (int)altPath.size();
                    if (this->recomputeCosts(altPath, vehicle, msTime)
                            < this->recomputeCosts(path, vehicle, msTime)) {
                        path.swap(altPath);
                    }
                }
            }
        }
        into.insert(into.end(), path.begin(), path.end());
        this->endQuery(visited);
        return true;
    }

    /// @brief prohibitions: a closure that is already a live permission change
    /// is served by the per-class metric (CCH); anything else routes via the
    /// exact fallback. Keep our own copy of the set so compute() can classify
    /// each query -- deliberately NOT delegating to the base implementation,
    /// which writes per-edge prohibition state into myEdgeInfos that this
    /// router never populates.
    void prohibit(const Prohibitions& toProhibit) {
        myProhibited = toProhibit;
        myProhibitionActive = !toProhibit.empty();
        myFallback->prohibit(toProhibit);
    }

    bool supportsProhibitions() const {
        return true;  // handled by delegating to the fallback
    }

    void setBulkMode(const bool mode) {
        SUMOAbstractRouter<E, V>::setBulkMode(mode);
        myFallback->setBulkMode(mode);
    }

private:
    /** @brief Bind the query to @p metric (rebinding only on change), seed
     * all sources and targets and run it; true iff a connection was found.
     *
     * Source seed = the entry edge's OWN centisecond effort. The arc-weight
     * convention (arc weight = effort of destination) omits the source
     * edge's effort, so choosing the cheapest zone-entry among members
     * requires adding it back per source -- otherwise the argmin ignores
     * how expensive each entry edge is and picks a different entry than A*.
     * Targets seed 0: the arc INTO a target already carries its effort. */
    bool runQuery(MetricPtr metric, const std::vector<unsigned>& sources,
                  const std::vector<unsigned>& targets, const V* const vehicle, double t) {
        if (metric != myBoundMetric) {
            myQuery.reset(*metric);
            myBoundMetric = metric;
        } else {
            myQuery.reset();
        }
        const double bigEff = (double)(RoutingKit::inf_weight - 1) / 100.0;
        for (const unsigned s : sources) {
            const double eff = this->getEffort(myGraph->edgeOf(s), vehicle, t);
            const unsigned d = eff < bigEff ? (unsigned)llround(eff * 100.0) : RoutingKit::inf_weight - 1;
            myQuery.add_source(s, d);
        }
        for (const unsigned tt : targets) {
            myQuery.add_target(tt, 0);
        }
        myQuery.run();
        return myQuery.get_distance() < RoutingKit::inf_weight;
    }

    /// @brief expand the last query's node path, bracketing it with the TAZ
    /// connectors where the endpoints are zones
    void buildPath(const E* from, const E* to, bool fromTaz, bool toTaz,
                   std::vector<const E*>& path) {
        const std::vector<unsigned> nodePath = myQuery.get_node_path();
        if (fromTaz) {
            path.push_back(from);
        }
        myGraph->expandNodePath(nodePath, path);
        if (toTaz) {
            path.push_back(to);
        }
    }

    /** @brief Can the per-class CCH metric already express every edge this
     * prohibition set forbids for @p vClass? True iff each forbidden edge is
     * ALSO closed for the class in the live network (getPermissions()) -- the
     * exact condition fillInputWeights() turns into inf_weight. A prohibited
     * edge that still permits the class is an arbitrary per-query prohibition
     * no shared metric can encode, so the caller must use the exact fallback.
     * (For time-bounded closures A* itself avoids the edge via this same live
     * permission signal -- SUMOAbstractRouter::isProhibited -- not the
     * per-query branch, so this is faithful to A*'s own behavior.)
     */
    bool prohibitionsCoveredByMetric(SUMOVehicleClass vClass) const {
        for (const auto& item : myProhibited) {
            if ((item.second.permissions & vClass) != vClass          // forbids vClass here...
                    && (item.first->getPermissions() & vClass) != 0) {  // ...but edge still permits it live
                return false;
            }
        }
        return true;
    }

private:
    const GRAPH* myGraph;                    // shared, immutable, not owned
    MetricProvider myMetricProvider;
    RoutingKit::CustomizableContractionHierarchyQuery myQuery;  // per-clone scratch
    MetricPtr myBoundMetric = nullptr;       // metric myQuery is currently bound to
    SUMOAbstractRouter<E, V>* myFallback;    // owned
    PeriodEnd myPeriodEnd;                   // may be nullptr (static weights)
    bool myProhibitionActive;
    Prohibitions myProhibited;               // last set installed via prohibit()

private:
    CCHRouter& operator=(const CCHRouter&) = delete;
};

