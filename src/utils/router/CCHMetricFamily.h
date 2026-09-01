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
/// @file    CCHMetricFamily.h
/// @author  Pranav Sateesh
/// @date    2026
///
// A family of per-vehicle-type CCH metrics over one shared CCHGraph -- the
// MetricProvider machinery behind CCHRouter, shared between the simulation
// and duarouter the same way CCHGraph shares the topology.
//
// Keying by the vehicle TYPE (rather than the class) makes everything the
// effort function reads from the vehicle exact per metric: the type's
// maximum speed, per-class edge speed restrictions, routing preferences,
// the bicycle speed table and one frozen weights.random-factor realization;
// only individual speed-factor draws within one type share a metric (the
// same approximation the CH family makes with its per-class prototype
// vehicles, but at finer granularity).
//
// Two modes:
//
// STATIC -- weights never change while a (type, weight period) pair is in
//   use. Metrics are customized lazily on the first query of a pair under a
//   mutex and cached (duarouter and the simulation's free-flow routers).
//   A runtime permission change re-customizes every cached metric on the
//   next query (flagPermissionsStale).
//
// LIVE -- weights track an adaptive speed table (the rerouting device).
//   Metrics are double-buffered: worker threads acquire-load the published
//   front while the owner refills and customizes the back at its barrier
//   and flips, so queries never observe a metric mid-customization. The
//   customize is sparse (RoutingKit partial customization over the edges
//   whose effort actually moved, behind a configurable deadband) with
//   wholesale refills after permission flips. Staleness is bounded to one
//   barrier -- CH's weightPeriod semantics: once any metric exists,
//   customize(now) must run at every barrier where an effort moved (see
//   atBarrier), and an epoch pair guards the bound as an invariant.
//
// The family is intentionally ignorant of WHEN to customize -- only the
// owner knows when no query is in flight (the simulation's adaptation
// barrier, duarouter's mutex) -- and of HOW reference vehicles are built
// (injected factory; V construction is host-specific).
/****************************************************************************/
#pragma once
#include <config.h>

#include <atomic>
#include <cmath>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>
#include <utils/common/SUMOTime.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/router/CCHGraph.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class CCHMetricFamily
 * @brief per-vehicle-type CCH metric store over a shared CCHGraph
 *
 * @tparam E the edge type (MSEdge / ROEdge)
 * @tparam V the vehicle type of the effort callback (SUMOVehicle / ROVehicle)
 * @tparam K the vehicle-type key (MSVehicleType / SUMOVTypeParameter); used
 *   only as a stable map key, never dereferenced
 */
template<class E, class V, class K>
class CCHMetricFamily {
public:
    typedef CCHGraph<E, V> Graph;
    typedef typename Graph::EffortOperation EffortOperation;
    typedef const RoutingKit::CustomizableContractionHierarchyMetric* MetricPtr;
    /// @brief builds an OWNED effort-reference vehicle for a type: never
    /// registered, counted or inserted -- it exists only so every fill of
    /// that type's metric evaluates the same reference (deterministic speed
    /// factor and frozen random realization are the factory's business).
    /// The second argument is the ensemble slot (see the LIVE constructor);
    /// slot 0 must reproduce the pre-ensemble reference exactly, further
    /// slots give distinct frozen weights.random-factor realizations
    /// (typically by varying the reference vehicle's id, which seeds them)
    typedef V* (*RefVehicleFactory)(const K*, int);
    /// @brief post-fill hook on the input weights (STATIC mode), e.g.
    /// duarouter masking the edges that restrict a type to inf_weight
    typedef void (*WeightPatch)(const Graph*, const V*, std::vector<unsigned>&);

    /** @brief construct a STATIC family (lazy build per (type, period))
     * @param[in] graph  the shared topology
     * @param[in] fillEffort the effort the weights are computed from
     * @param[in] begin  begin of the weight-period grid
     * @param[in] weightPeriod period length; SUMOTime_MAX = a single period
     * @param[in] factory reference-vehicle factory; nullptr = fill with the
     *   QUERYING vehicle (each pair is customized exactly once, so the first
     *   vehicle of a type is that type's reference -- duarouter's contract)
     * @param[in] patch  optional post-fill weight hook */
    CCHMetricFamily(const Graph* graph, EffortOperation fillEffort,
                    SUMOTime begin, SUMOTime weightPeriod,
                    RefVehicleFactory factory, WeightPatch patch) :
        myGraph(graph), myFillEffort(fillEffort), myGateEffort(nullptr),
        myFactory(factory), myPatch(patch), myLive(false),
        myBegin(begin), myWeightPeriod(weightPeriod),
        myUpdateFactor(1.), myUpdateConstant(0.) {
    }

    /** @brief construct a LIVE family (double-buffered, barrier-customized)
     * @param[in] graph  the shared topology
     * @param[in] fillEffort the (possibly per-type) effort the weights are
     *   computed from, evaluated with the type's reference vehicle
     * @param[in] gateEffort the type-INDEPENDENT effort used by the deadband
     *   (evaluated with a null vehicle: per-type extras are static
     *   multipliers that cancel out of the moved-or-not decision)
     * @param[in] updateFactor / updateConstant the deadband: an edge's arcs
     *   are only refilled when its gate effort moved by MORE than BOTH the
     *   relative factor AND the absolute constant (seconds) since this
     *   buffer last applied it -- the sufficientSaving() analog. The
     *   moving-average speed filter smears every traffic event into many
     *   ticks of sub-percent drift; without a deadband that drift marks
     *   thousands of arcs per barrier and partial re-customization
     *   degenerates (RoutingKit documents it for the sparse "new traffic
     *   jam" case). 1/0 = every change propagates. Rejected edges stay
     *   pending and their drift accumulates against the same applied value,
     *   so a slow trend eventually passes while filter jitter never does.
     * @param[in] factory reference-vehicle factory (required in LIVE mode)
     * @param[in] ensembleK how many frozen weights.random-factor
     *   realizations (= metrics) to keep per type. The exact routers draw a
     *   fresh perturbation per vehicle; a shared metric freezes ONE
     *   realization, losing the feature's route diversity. With K > 1 every
     *   type keeps K metrics -- each filled with its own reference vehicle
     *   (factory slot k) and therefore its own frozen realization -- and a
     *   vehicle is assigned its slot by a stable hash of its id, so each
     *   vehicle persistently routes on one of K perturbed networks:
     *   K = 1 is exactly the single-realization behavior, growing K
     *   converges toward the exact routers' per-vehicle diversity at K x
     *   customization and memory cost. Pointless without
     *   weights.random-factor (the K references fill identical weights). */
    CCHMetricFamily(const Graph* graph, EffortOperation fillEffort,
                    EffortOperation gateEffort, double updateFactor,
                    double updateConstant, RefVehicleFactory factory,
                    int ensembleK = 1) :
        myGraph(graph), myFillEffort(fillEffort), myGateEffort(gateEffort),
        myFactory(factory), myPatch(nullptr), myLive(true),
        myBegin(0), myWeightPeriod(SUMOTime_MAX),
        myUpdateFactor(updateFactor), myUpdateConstant(updateConstant),
        myEnsembleK(ensembleK > 1 ? ensembleK : 1) {
        const unsigned space = myGraph->edgeIdSpace();
        for (int i = 0; i < 2; i++) {
            // NaN = "never applied": the first pending occurrence always
            // passes the deadband and primes the entry
            myAppliedEffort[i].assign(space, std::numeric_limits<double>::quiet_NaN());
            myPendingFlag[i].assign(space, 0);
        }
    }

    ~CCHMetricFamily() {
        for (LiveMetric* c : myLiveMetrics) {
            delete c->refVehicle;
            delete c;
        }
        for (auto& item : myStaticMetrics) {
            delete item.second.refVehicle;
        }
    }

    /// @name STATIC mode
    /// @{

    /** @brief the metric for (type key, period of @p time), built on the
     * first query of the pair; nullptr before init. Thread-safe: the mutex
     * serialises concurrent first queries from parallel routing threads
     * (satisfying primeClassMask's synchronization contract); afterwards the
     * map is read-only for that key (map references are address-stable).
     * A pending permission flip first re-customizes every cached metric from
     * the live permissions (main-thread callers only -- see
     * flagPermissionsStale). */
    MetricPtr get(const K* key, SUMOVehicleClass vClass, SUMOTime time, const V* veh) {
        std::lock_guard<std::mutex> lock(myStaticLock);
        if (myPermissionsStale) {
            const double now = STEPS2TIME(time);
            for (auto& item : myStaticMetrics) {
                StaticMetric& sm = item.second;
                myGraph->fillInputWeights(myFillEffort, sm.vClass,
                                          sm.refVehicle != nullptr ? sm.refVehicle : veh, now, sm.weights);
                sm.metric->customize();
            }
            myPermissionsStale = false;
        }
        // period 0 covers everything before begin and the whole run when the
        // weights are static
        int period = 0;
        if (myWeightPeriod > 0 && myWeightPeriod != SUMOTime_MAX && time > myBegin) {
            period = (int)((time - myBegin) / myWeightPeriod);
        }
        const std::pair<const K*, int> mapKey(key, period);
        auto it = myStaticMetrics.find(mapKey);
        if (it == myStaticMetrics.end()) {
            StaticMetric& sm = myStaticMetrics[mapKey];
            sm.vClass = vClass;
            const V* ref = veh;
            if (myFactory != nullptr) {
                sm.refVehicle = myFactory(key, 0);
                ref = sm.refVehicle;
            }
            // the weights are evaluated at the period's begin, so each pair
            // is customized exactly once (a single period evaluates at begin,
            // where free-flow style efforts are time-invariant anyway)
            const double fillTime = myWeightPeriod == SUMOTime_MAX
                                    ? STEPS2TIME(time) : STEPS2TIME(myBegin + period * myWeightPeriod);
            myGraph->fillInputWeights(myFillEffort, vClass, ref, fillTime, sm.weights);
            if (myPatch != nullptr) {
                myPatch(myGraph, veh, sm.weights);
            }
            sm.metric.reset(new RoutingKit::CustomizableContractionHierarchyMetric(
                                myGraph->cch(), sm.weights));
            sm.metric->customize();
            return sm.metric.get();
        }
        return it->second.metric.get();
    }

    /// @brief the end of the weight period containing the given time
    /// (SUMOTime_MAX when the weights are static); CCHRouter's PeriodEnd hook
    SUMOTime periodEnd(SUMOTime time) const {
        if (myWeightPeriod <= 0 || myWeightPeriod == SUMOTime_MAX) {
            return SUMOTime_MAX;
        }
        const int period = time > myBegin ? (int)((time - myBegin) / myWeightPeriod) : 0;
        return myBegin + (period + 1) * myWeightPeriod;
    }

    /// @brief a runtime permission change invalidated the cached metrics;
    /// every metric is re-customized from the live permissions on the next
    /// get(). The caller must also invalidate the graph's class masks.
    void flagPermissionsStale() {
        std::lock_guard<std::mutex> lock(myStaticLock);
        myPermissionsStale = true;
    }
    /// @}

    /// @name LIVE mode
    /// @{

    /// @brief allocate the metric state (all ensemble slots) for one type;
    /// call only while no query is in flight (seeding at init, or the
    /// customization barrier)
    void seedKey(const K* key) {
        if (myByKey.count(key) == 0) {
            std::vector<LiveMetric*>& slots = myByKey[key];
            for (int k = 0; k < myEnsembleK; k++) {
                slots.push_back(buildLiveMetric(key, k));
            }
        }
    }

    /// @brief queue an edge whose gate effort changed (both buffers; the
    /// deadband applies later, at the customize barrier)
    void markDirty(const E* e) {
        const int id = e->getNumericalID();
        if (id < 0 || id >= (int)myPendingFlag[0].size()) {
            return;
        }
        for (int i = 0; i < 2; i++) {
            if (!myPendingFlag[i][id]) {
                myPendingFlag[i][id] = 1;
                myPendingList[i].push_back(e);
            }
        }
    }

    /** @brief a runtime permission change (closure / re-opening) hit this
     * edge: queue it bypassing the deadband and divert every query to the
     * exact fallback until customize() has refilled both buffers wholesale
     * (the sparse path cannot repair tail-side arcs of a connection-level
     * change). The caller must also invalidate the graph's class masks. */
    void invalidateEdge(const E* e) {
        const int id = e->getNumericalID();
        if (id < 0 || id >= (int)myPendingFlag[0].size()) {
            return;
        }
        markDirty(e);
        // NaN sentinel: the deadband always passes, so the flip reaches the
        // metric at the next barrier even though the speed table did not move
        myAppliedEffort[0][id] = std::numeric_limits<double>::quiet_NaN();
        myAppliedEffort[1][id] = std::numeric_limits<double>::quiet_NaN();
        myFullFillsPending = 2;
        myMetricStale.store(true, std::memory_order_release);
    }

    /** @brief the owner's customization barrier: bump the speed epoch when
     * an effort moved and re-customize when it matters. Lazy, following the
     * CH weightPeriod pattern: metric state is only built for types somebody
     * routes on (myLiveMetrics stays empty in a run that never routes, which
     * then pays nothing here). Once any metric exists, every barrier where
     * an effort moved re-customizes, so a query never sees a metric more
     * than one barrier old -- CH's weightPeriod bound. The queried flag only
     * bootstraps type creation; a pending permission flip forces the
     * customize even without queries. */
    void atBarrier(double now, bool effortsMoved) {
        if (effortsMoved) {
            mySpeedEpoch.fetch_add(1, std::memory_order_release);
        }
        if (myQueried.exchange(false, std::memory_order_acq_rel)
                || myMetricStale.load(std::memory_order_acquire)
                || (effortsMoved && !myLiveMetrics.empty())) {
            customize(now);
        }
    }

    /** @brief refill + customize + publish every type's metric from the live
     * efforts and permissions. Call only while no query is in flight (the
     * publish itself is lock-free: release store here, acquire loads on the
     * query hot path). */
    void customize(double now) {
        // types that queried since the last barrier without a metric
        // (streamed route files, TraCI-added types): create their state now
        {
            std::lock_guard<std::mutex> lock(myWantedLock);
            for (const K* key : myWantedKeys) {
                seedKey(key);
            }
            myWantedKeys.clear();
        }
        if (myLiveMetrics.empty()) {
            return;
        }
        // buffers flip in lockstep across types (every call processes all)
        const int backShared = 1 - myLiveMetrics.front()->frontIndex;
        // Permission flip pending: run a FULL refill on this pass's back
        // buffer (the other buffer gets its full refill on the next pass --
        // myFullFillsPending counts both down). The fills below re-prime the
        // graph's connection masks, which the flip's caller invalidated.
        bool forceFull = false;
        if (myFullFillsPending > 0) {
            myFullFillsPending--;
            forceFull = true;
        }
        // Deadband pass, once, on the type-shared gate effort: accept edges
        // whose effort moved by more than BOTH bounds since this buffer last
        // applied them; rejected edges stay pending (see the constructor doc)
        std::vector<const E*> accepted;
        if (!forceFull && myLiveMetrics.front()->metric[backShared] != nullptr) {
            std::vector<const E*> stillPending;
            for (const E* e : myPendingList[backShared]) {
                const int id = e->getNumericalID();
                const double effNow = myGateEffort(e, nullptr, now);
                const double effApplied = myAppliedEffort[backShared][id];
                bool pass = true;
                if (!std::isnan(effApplied) && effApplied > 0. && effNow > 0.) {
                    const double hi = MAX2(effNow, effApplied);
                    const double lo = MIN2(effNow, effApplied);
                    pass = (hi / lo > myUpdateFactor) && (hi - lo > myUpdateConstant);
                }
                if (pass) {
                    accepted.push_back(e);
                    myAppliedEffort[backShared][id] = effNow;
                    myPendingFlag[backShared][id] = 0;
                } else {
                    stillPending.push_back(e);
                }
            }
            myPendingList[backShared].swap(stillPending);
        }
        bool fullRebuild = false;
        for (LiveMetric* c : myLiveMetrics) {
            const int back = 1 - c->frontIndex;
            // Fill from the live efforts, masking arcs the class is not
            // permitted on -- this is where per-class permissions AND active
            // closures become inf_weight. The fill reference is the type's
            // OWNED refVehicle, so the effort captures the type's extras
            // exactly (including each folded via edge inside viaChainEffort)
            // and freezes ONE random realization per metric -- the same
            // approximation CHRouterWrapper makes per hierarchy; exact
            // per-vehicle randomization remains the domain of the exact
            // routers.
            if (c->metric[back] == nullptr) {
                // first use of this buffer: no previous state to diff against
                myGraph->fillInputWeights(myFillEffort, c->vClass, c->refVehicle, now, c->weight[back]);
                c->metric[back] = std::make_shared<RoutingKit::CustomizableContractionHierarchyMetric>(
                                      myGraph->cch(), c->weight[back]);
                c->metric[back]->customize();  // serial; avoids OpenMP oversubscription under FOX
                fullRebuild = true;
            } else if (forceFull) {
                // permission flip: wholesale refill in place from the
                // re-primed masks and live permissions (the metric already
                // references weight[back]), then a full customize
                myGraph->fillInputWeights(myFillEffort, c->vClass, c->refVehicle, now, c->weight[back]);
                c->metric[back]->customize();
                fullRebuild = true;
            } else {
                // SPARSE PATH: metric[back] is the customization of the
                // current contents of weight[back]. Recompute only the arcs
                // of accepted edges (edge->arc reverse image), write the
                // ones that moved, and propagate through affected triangles
                // only. Cost scales with traffic transitions, not with the
                // network.
                std::vector<unsigned>& applied = c->weight[back];
                c->partial->reset(myGraph->cch());
                unsigned changed = 0;
                for (const E* e : accepted) {
                    for (const unsigned a : myGraph->arcsOfEdge(e)) {
                        const unsigned newW = myGraph->computeArcWeight(a, myFillEffort, c->vClass, c->refVehicle, now);
                        if (newW != applied[a]) {
                            applied[a] = newW;
                            c->partial->update_arc(a);  // takes INPUT arc ids
                            ++changed;
                        }
                    }
                }
                if (changed > 0) {
                    c->partial->customize(*c->metric[back]);
                }
            }
            // publish lock-free: release store at the barrier, worker
            // acquire-loads on the hot path
            c->frontIndex = back;
            c->front.store(c->metric[back].get(), std::memory_order_release);
        }
        if (fullRebuild) {
            // a full fill matched every arc to the live efforts: pending
            // entries for this buffer are stale; NaN re-arms the
            // first-change auto-pass
            for (const E* e : myPendingList[backShared]) {
                myPendingFlag[backShared][e->getNumericalID()] = 0;
            }
            myPendingList[backShared].clear();
            myAppliedEffort[backShared].assign(myAppliedEffort[backShared].size(),
                                               std::numeric_limits<double>::quiet_NaN());
        }
        // every published front now reflects the live permissions (the other
        // buffer, if still pending a full refill, is repaired before its
        // next publish) and the live efforts as of this barrier (both epoch
        // counters only move on the owner's thread at the barrier, so the
        // pair cannot tear)
        myMetricStale.store(false, std::memory_order_release);
        myPublishedEpoch.store(mySpeedEpoch.load(std::memory_order_relaxed),
                               std::memory_order_release);
    }

    /** @brief the published metric for a type key and querying vehicle id
     * (the id picks the ensemble slot -- see the LIVE constructor), or
     * nullptr if the query must route on the exact fallback. Query hot
     * path, any thread. */
    MetricPtr published(const K* key, const std::string& vehID) {
        const auto it = myByKey.find(key);
        if (it == myByKey.end()) {
            // streamed-in type without a metric yet: register it for
            // creation at the next barrier and fall back
            {
                std::lock_guard<std::mutex> lock(myWantedLock);
                myWantedKeys.push_back(key);
            }
            myQueried.store(true, std::memory_order_relaxed);
            return nullptr;
        }
        // mark the metric as consumed. Test before set: an unconditional
        // store from every query would ping-pong the cache line between
        // cores; the read is shared and the store fires once per barrier
        // window (relaxed: only gates bootstrap/re-arm, never visibility)
        if (!myQueried.load(std::memory_order_relaxed)) {
            myQueried.store(true, std::memory_order_relaxed);
        }
        // A permission flip that is not yet customized in makes every
        // published metric unusable: it may still route THROUGH a
        // just-closed edge, and CCHRouter's coverage test consults the LIVE
        // permissions, which would wrongly bless the query. Route exactly
        // until the barrier repairs the metrics.
        if (myMetricStale.load(std::memory_order_acquire)) {
            return nullptr;
        }
        // Safety invariant: never serve a metric the efforts have moved away
        // from -- the exact routers see live weights and CH is bounded to
        // one weightPeriod. atBarrier re-customizes at every moved barrier
        // once metrics exist, so this cannot fire in steady state; if a gap
        // ever opens, this burst routes on the exact fallback (live weights)
        // and the queried flag set above arms the customize that closes it.
        if (myPublishedEpoch.load(std::memory_order_acquire)
                != mySpeedEpoch.load(std::memory_order_acquire)) {
            return nullptr;
        }
        // stable slot assignment: FNV-1a over the id (std::hash is
        // implementation-defined and would break cross-platform test
        // reproducibility)
        const LiveMetric* c = myEnsembleK == 1 ? it->second.front()
                              : it->second[fnv1a(vehID) % myEnsembleK];
        return c->front.load(std::memory_order_acquire);
    }

    /// @brief whether any type has live metric state yet
    bool empty() const {
        return myLiveMetrics.empty();
    }
    /// @}

private:
    /// @brief per-type LIVE metric state. Heap-owned: the atomic makes it
    /// non-movable, so it cannot live in a map by value.
    struct LiveMetric {
        std::vector<unsigned> weight[2];  // ping-pong input-weight buffers, live whole run
        std::shared_ptr<RoutingKit::CustomizableContractionHierarchyMetric> metric[2];
        std::atomic<const RoutingKit::CustomizableContractionHierarchyMetric*> front{nullptr};
        int frontIndex = 1;               // first customize uses back = 0
        SUMOVehicleClass vClass = SVC_PASSENGER;
        /// @brief OWNED effort-reference vehicle (see RefVehicleFactory)
        V* refVehicle = nullptr;
        /// @brief partial-customization worker (queue over the shared CCH);
        /// one per metric -- its queue is drained by every customize()
        std::shared_ptr<RoutingKit::CustomizableContractionHierarchyPartialCustomization> partial;
    };

    /// @brief per-(type, period) STATIC metric state. RoutingKit metrics
    /// BORROW their input-weight buffer, so buffer and metric live together
    /// in the map (std::map nodes are address-stable).
    struct StaticMetric {
        std::vector<unsigned> weights;
        std::unique_ptr<RoutingKit::CustomizableContractionHierarchyMetric> metric;
        SUMOVehicleClass vClass = SVC_PASSENGER;
        /// @brief OWNED reference vehicle (nullptr when filling with the
        /// querying vehicle)
        V* refVehicle = nullptr;
    };

    /// @brief stable 64-bit FNV-1a for the ensemble slot assignment
    static uint64_t fnv1a(const std::string& s) {
        uint64_t h = 1469598103934665603ull;
        for (const char ch : s) {
            h = (h ^ (unsigned char)ch) * 1099511628211ull;
        }
        return h;
    }

    /// @brief allocate LIVE metric state for one (type, ensemble slot)
    /// (owner's thread only)
    LiveMetric* buildLiveMetric(const K* key, int slot) {
        LiveMetric* c = new LiveMetric();
        c->refVehicle = myFactory(key, slot);
        c->vClass = c->refVehicle->getVClass();
        c->weight[0].resize(myGraph->arcCount());
        c->weight[1].resize(myGraph->arcCount());
        c->partial = std::make_shared<RoutingKit::CustomizableContractionHierarchyPartialCustomization>(myGraph->cch());
        myLiveMetrics.push_back(c);
        return c;
    }

    /// @brief the immutable shared topology
    const Graph* myGraph;
    /// @brief the effort the metrics are filled from
    EffortOperation myFillEffort;
    /// @brief the type-independent effort of the LIVE deadband
    EffortOperation myGateEffort;
    /// @brief reference-vehicle factory (required LIVE, optional STATIC)
    RefVehicleFactory myFactory;
    /// @brief post-fill weight hook (STATIC)
    WeightPatch myPatch;
    /// @brief LIVE or STATIC (fixed at construction)
    const bool myLive;

    /// @name STATIC state
    /// @{
    SUMOTime myBegin;
    SUMOTime myWeightPeriod;
    std::map<std::pair<const K*, int>, StaticMetric> myStaticMetrics;
    std::mutex myStaticLock;
    bool myPermissionsStale = false;
    /// @}

    /// @name LIVE state
    /// @{
    /// @brief the deadband bounds (see the LIVE constructor)
    double myUpdateFactor;
    double myUpdateConstant;
    /// @brief frozen random-factor realizations per type (see the LIVE ctor)
    int myEnsembleK = 1;
    /// @brief every type's state, in creation order (barrier iteration) and
    /// by key (query lookup; only mutated while no query is in flight)
    std::vector<LiveMetric*> myLiveMetrics;
    std::map<const K*, std::vector<LiveMetric*> > myByKey;
    /// @brief types that queried but have no metric yet (demand streams, so
    /// types can appear after seeding); queries register them under the lock
    /// and route via the fallback until the next barrier builds their state
    std::vector<const K*> myWantedKeys;
    std::mutex myWantedLock;
    /// @brief whether a query arrived since the last barrier (bootstraps
    /// type creation and re-arms after resets -- see atBarrier)
    std::atomic<bool> myQueried{false};
    /// @brief a permission flip has not yet been customized into the
    /// published metrics (queries divert to the exact fallback while set)
    std::atomic<bool> myMetricStale{false};
    /// @brief barrier counter of the tracked efforts and the counter value
    /// the published metrics were customized from: the staleness bound's
    /// safety invariant (see atBarrier / published). Both only change on the
    /// owner's thread at the barrier -- no query is in flight there -- the
    /// atomics just publish the values to the query threads.
    std::atomic<uint64_t> mySpeedEpoch{0};
    std::atomic<uint64_t> myPublishedEpoch{0};
    /// @brief how many customize passes must run a FULL refill after a
    /// permission flip: 2 = both ping-pong buffers (the sparse path cannot
    /// repair connection-level mask changes: arcsOfEdge deliberately omits
    /// the tail side)
    int myFullFillsPending = 0;
    /// @brief per-buffer edge state for the sparse path: the gate effort
    /// each edge's arcs were last filled from (NaN = never applied /
    /// forced), the pending-dirty flag, and the pending list
    std::vector<double> myAppliedEffort[2];
    std::vector<char> myPendingFlag[2];
    std::vector<const E*> myPendingList[2];
    /// @}

private:
    CCHMetricFamily(const CCHMetricFamily&) = delete;
    CCHMetricFamily& operator=(const CCHMetricFamily&) = delete;
};
