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
// Contraction Hierarchy over the SUMO edge graph (CCHGraph). The metric is
// re-customized out-of-band (MSRoutingEngine::adaptEdgeEfforts) and published
// as a shared_ptr snapshot; each query snapshots the current metric, so a
// publish that lands mid-query is safe (double buffer).
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

#ifdef HAVE_ROUTINGKIT

#include <memory>
#include <vector>
#include <cmath>
#include <microsim/MSEdge.h>
#include <utils/vehicle/SUMOVehicle.h>
#include <utils/router/SUMOAbstractRouter.h>
#include <routingkit/customizable_contraction_hierarchy.h>
#include "CCHGraph.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class CCHRouter
 * @brief Contraction-hierarchy router for the microsim edge graph.
 */
class CCHRouter : public SUMOAbstractRouter<MSEdge, SUMOVehicle> {
public:
    typedef const RoutingKit::CustomizableContractionHierarchyMetric* MetricPtr;
    /// @brief supplies the published metric for a vehicle class (lock-free); nullptr => fall back
    typedef MetricPtr (*MetricProvider)(SUMOVehicleClass);

    /** @brief Constructor.
     * @param[in] graph     the shared immutable CCH topology (not owned)
     * @param[in] provider  static accessor for the published metric snapshot
     * @param[in] operation the effort callback (same one A-star and CH use)
     * @param[in] fallback  embedded router for non-CCH cases (OWNED)
     */
    CCHRouter(const CCHGraph* graph, MetricProvider provider,
              Operation operation, SUMOAbstractRouter<MSEdge, SUMOVehicle>* fallback) :
        SUMOAbstractRouter<MSEdge, SUMOVehicle>("CCHRouter", true, operation, nullptr, false, false),
        myGraph(graph), myMetricProvider(provider), myFallback(fallback),
        myBoundMetric(nullptr), myProhibitionActive(false) {
    }

    /// @brief clone constructor: share graph + provider, clone the fallback, fresh query scratch
    CCHRouter(CCHRouter* other) :
        SUMOAbstractRouter<MSEdge, SUMOVehicle>(other),
        myGraph(other->myGraph), myMetricProvider(other->myMetricProvider),
        myFallback(other->myFallback->clone()), myBoundMetric(nullptr),
        myProhibitionActive(other->myProhibitionActive), myProhibited(other->myProhibited) {
    }

    virtual ~CCHRouter() {
        delete myFallback;
    }

    virtual SUMOAbstractRouter<MSEdge, SUMOVehicle>* clone() {
        return new CCHRouter(this);
    }

    bool compute(const MSEdge* from, const MSEdge* to, const SUMOVehicle* const vehicle,
                 SUMOTime msTime, std::vector<const MSEdge*>& into, bool silent = false) {
        // A CCH query can only avoid an edge the metric already encodes as inf:
        // RoutingKit's query has no per-arc blacklist, and a query-time skip is
        // unsound because shortcut arcs bake in shortest paths THROUGH the edge
        // (SUMO's own CHRouter refuses prohibitions for the same reason). So we
        // run CCH when the metric can express the request and fall back to the
        // exact A* router otherwise: (a) no metric for this class, or (b) an
        // active prohibition that is NOT already a live permission-closure this
        // class's metric encodes (prohibitionsCoveredByMetric).
        const SUMOVehicleClass vClass = vehicle == nullptr ? SVC_PASSENGER : vehicle->getVClass();
        MetricPtr metric = myMetricProvider(vClass);
        if (metric == nullptr || (myProhibitionActive && !prohibitionsCoveredByMetric(vClass))) {
            return myFallback->compute(from, to, vehicle, msTime, into, silent);
        }
        // Endpoints -> node SETS: a TAZ connector expands to its member edges
        // (phantom-node seeding); a real edge is a single node. TAZ member edges
        // are the class-union set, so filter to edges this class may enter.
        const bool fromTaz = from->isTazConnector();
        const bool toTaz = to->isTazConnector();
        std::vector<unsigned> srcBuf, tgtBuf;
        const std::vector<unsigned>* sources;
        const std::vector<unsigned>* targets;
        if (fromTaz) {
            for (const unsigned m : myGraph->tazSources(from)) {
                if ((myGraph->edgeOf(m)->getPermissions() & vClass) != 0) { srcBuf.push_back(m); }
            }
            sources = &srcBuf;
        } else {
            const unsigned s = myGraph->nodeOf(from);
            if (s != CCHGraph::INVALID_NODE) { srcBuf.push_back(s); }
            sources = &srcBuf;
        }
        if (toTaz) {
            for (const unsigned m : myGraph->tazSinks(to)) {
                if ((myGraph->edgeOf(m)->getPermissions() & vClass) != 0) { tgtBuf.push_back(m); }
            }
            targets = &tgtBuf;
        } else {
            const unsigned t = myGraph->nodeOf(to);
            if (t != CCHGraph::INVALID_NODE) { tgtBuf.push_back(t); }
            targets = &tgtBuf;
        }
        if (sources->empty() || targets->empty()) {
            // unmappable endpoint (e.g. internal edge, or a TAZ with no member
            // in this class's graph) -> exact fallback.
            return myFallback->compute(from, to, vehicle, msTime, into, silent);
        }

        this->startQuery();
        // Rebind only when the metric changed (~every 180s); else reuse binding.
        if (metric != myBoundMetric) {
            myQuery.reset(*metric);
            myBoundMetric = metric;
        } else {
            myQuery.reset();
        }
        // Seed all sources and targets; RoutingKit's multi-source/target query
        // finds the min-cost (source, target) pair in one search.
        //
        // Source seed = the entry edge's OWN centisecond effort. The arc-weight
        // convention (arc weight = effort of destination) omits the source
        // edge's effort, so choosing the cheapest zone-entry among members
        // requires adding it back per source -- otherwise the argmin ignores
        // how expensive each entry edge is and picks a different entry than A*.
        // Targets seed 0: the arc INTO a target already carries its effort.
        const double t = STEPS2TIME(msTime);
        const double bigEff = (double)(RoutingKit::inf_weight - 1) / 100.0;
        for (const unsigned s : *sources) {
            const double eff = this->getEffort(myGraph->edgeOf(s), nullptr, t);
            const unsigned d = eff < bigEff ? (unsigned)llround(eff * 100.0) : RoutingKit::inf_weight - 1;
            myQuery.add_source(s, d);
        }
        for (const unsigned tt : *targets) { myQuery.add_target(tt, 0); }
        myQuery.run();
        if (myQuery.get_distance() >= RoutingKit::inf_weight) {
            this->endQuery(0);
            if (!silent && myErrorMsgHandler != nullptr) {
                myErrorMsgHandler->informf(TL("No connection between edge '%' and edge '%' found."),
                                           from->getID(), to->getID());
            }
            return false;
        }
        const std::vector<unsigned> nodePath = myQuery.get_node_path();  // chosen s*..t*
        if (fromTaz) { into.push_back(from); }   // prepend the TAZ-source connector
        myGraph->expandNodePath(nodePath, into);
        if (toTaz) { into.push_back(to); }       // append the TAZ-sink connector
        this->endQuery((int)nodePath.size());
        return true;
    }

    /// @brief prohibitions: a closure that is already a live permission change
    /// is served by the per-class metric (CCH); anything else routes via the
    /// exact fallback. Keep the set so compute() can classify each query.
    void prohibit(const Prohibitions& toProhibit) {
        myProhibited = toProhibit;
        myProhibitionActive = !toProhibit.empty();
        myFallback->prohibit(toProhibit);
    }

    bool supportsProhibitions() const {
        return true;  // handled by delegating to the fallback
    }

    void setBulkMode(const bool mode) {
        SUMOAbstractRouter<MSEdge, SUMOVehicle>::setBulkMode(mode);
        myFallback->setBulkMode(mode);
    }

private:
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
    const CCHGraph* myGraph;                 // shared, immutable, not owned
    MetricProvider myMetricProvider;
    RoutingKit::CustomizableContractionHierarchyQuery myQuery;  // per-clone scratch
    MetricPtr myBoundMetric;                 // metric myQuery is currently bound to
    SUMOAbstractRouter<MSEdge, SUMOVehicle>* myFallback;        // owned
    bool myProhibitionActive;
    Prohibitions myProhibited;               // last set installed via prohibit()

private:
    CCHRouter& operator=(const CCHRouter&) = delete;
};

#endif // HAVE_ROUTINGKIT
