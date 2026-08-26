/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2007-2026 German Aerospace Center (DLR) and others.
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
/// @file    MSRoutingEngine.cpp
/// @author  Michael Behrisch
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Christoph Sommer
/// @author  Jakob Erdmann
/// @date    Tue, 04 Dec 2007
///
// A device that performs vehicle rerouting based on current edge speeds
/****************************************************************************/
#include <config.h>

#include "MSRoutingEngine.h"
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSJunction.h>
#include <microsim/MSEdge.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSVehicleType.h>
#include <set>
#include <microsim/MSInsertionControl.h>
#include <microsim/transportables/MSTransportable.h>
#include <microsim/devices/MSDevice_Taxi.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/WrappingCommand.h>
#include <utils/common/StaticCommand.h>
#include <utils/common/StringUtils.h>
#include <utils/xml/SUMOSAXAttributes.h>
#include <utils/router/DijkstraRouter.h>
#include <utils/router/AStarRouter.h>
#include <utils/router/CHRouter.h>
#include <utils/router/CHRouterWrapper.h>
#include <utils/vehicle/SUMOVehicleParserHelper.h>
#include <utils/router/CCHGraphBase.h>
#include <utils/router/CCHRouter.h>
#include <routingkit/customizable_contraction_hierarchy.h>

//#define DEBUG_SEPARATE_TURNS
#define DEBUG_COND(obj) (obj->isSelected())

// ===========================================================================
// static member variables
// ===========================================================================
std::vector<double> MSRoutingEngine::myEdgeSpeeds;
std::vector<double> MSRoutingEngine::myEdgeBikeSpeeds;
std::vector<MSRoutingEngine::TimeAndCount> MSRoutingEngine::myEdgeTravelTimes;
std::vector<std::vector<double> > MSRoutingEngine::myPastEdgeSpeeds;
std::vector<std::vector<double> > MSRoutingEngine::myPastEdgeBikeSpeeds;
Command* MSRoutingEngine::myEdgeWeightSettingCommand = nullptr;
double MSRoutingEngine::myAdaptationWeight(0);
int MSRoutingEngine::myAdaptationSteps;
int MSRoutingEngine::myAdaptationStepsIndex = 0;
SUMOTime MSRoutingEngine::myAdaptationInterval = -1;
SUMOTime MSRoutingEngine::myLastAdaptation = -1;
bool MSRoutingEngine::myWithTaz;
bool MSRoutingEngine::myBikeSpeeds;
MSRouterProvider* MSRoutingEngine::myRouterProvider = nullptr;
std::map<std::pair<const MSEdge*, const MSEdge*>, ConstMSRoutePtr> MSRoutingEngine::myCachedRoutes;
double MSRoutingEngine::myPriorityFactor(0);
double MSRoutingEngine::myMinEdgePriority(std::numeric_limits<double>::max());
double MSRoutingEngine::myEdgePriorityRange(0);
bool MSRoutingEngine::myDynamicRandomness(false);
bool MSRoutingEngine::myHaveExtras(false);

SUMOAbstractRouter<MSEdge, SUMOVehicle>::Operation MSRoutingEngine::myEffortFunc = &MSRoutingEngine::getEffort;
#ifdef HAVE_FOX
FXMutex MSRoutingEngine::myRouteCacheMutex;
#endif
CCHGraph* MSRoutingEngine::myCCHGraph = nullptr;
std::vector<MSRoutingEngine::CCHClass*> MSRoutingEngine::myCCHClasses;
std::map<SUMOVehicleClass, MSRoutingEngine::CCHClass*> MSRoutingEngine::myCCHByClass;
std::atomic<bool> MSRoutingEngine::myCCHQueried(false);
std::vector<unsigned> MSRoutingEngine::myCCHScratchWeight;
double MSRoutingEngine::myCCHUpdateFactor = 1.;
double MSRoutingEngine::myCCHUpdateConstant = 0.;
std::vector<double> MSRoutingEngine::myCCHAppliedEffort[2];
std::vector<char> MSRoutingEngine::myCCHPendingFlag[2];
std::vector<const MSEdge*> MSRoutingEngine::myCCHPendingList[2];
bool MSRoutingEngine::myCCHValidate = false;


// ===========================================================================
// method definitions
// ===========================================================================
void
MSRoutingEngine::initWeightUpdate(SUMOTime lastAdaptation) {
    if (myAdaptationInterval == -1) {
        myEdgeWeightSettingCommand = nullptr;
        myLastAdaptation = lastAdaptation;
        const OptionsCont& oc = OptionsCont::getOptions();
        myWithTaz = oc.getBool("device.rerouting.with-taz");
        myAdaptationInterval = string2time(oc.getString("device.rerouting.adaptation-interval"));
        myAdaptationWeight = oc.getFloat("device.rerouting.adaptation-weight");
        const SUMOTime period = string2time(oc.getString("device.rerouting.period"));
        if (myAdaptationWeight < 1. && myAdaptationInterval > 0) {
            SUMOTime nextAdaptation = -1;
            if (lastAdaptation >= 0) {
                nextAdaptation = lastAdaptation + myAdaptationInterval;
            }
            myEdgeWeightSettingCommand = new StaticCommand<MSRoutingEngine>(&MSRoutingEngine::adaptEdgeEfforts);
            MSNet::getInstance()->getEndOfTimestepEvents()->addEvent(myEdgeWeightSettingCommand, nextAdaptation);
        } else if (period > 0) {
            WRITE_WARNING(TL("Rerouting is useless if the edge weights do not get updated!"));
        }
        OutputDevice::createDeviceByOption("device.rerouting.output", "weights", "meandata_file.xsd");
    }
}


void
MSRoutingEngine::initEdgeWeights(SUMOVehicleClass svc, SUMOTime lastAdaption, int index) {
    const OptionsCont& oc = OptionsCont::getOptions();
    if (myAdaptationWeight == 0 || !oc.isDefault("device.rerouting.adaptation-steps")) {
        myAdaptationSteps = oc.getInt("device.rerouting.adaptation-steps");
    }
    if (myBikeSpeeds && svc == SVC_BICYCLE) {
        _initEdgeWeights(myEdgeBikeSpeeds, myPastEdgeBikeSpeeds);
    } else {
        _initEdgeWeights(myEdgeSpeeds, myPastEdgeSpeeds);
    }
    if (lastAdaption >= 0) {
        myLastAdaptation = lastAdaption;
    }
    if (index >= 0 && myAdaptationSteps > 0) {
        assert(index < myAdaptationSteps);
        myAdaptationStepsIndex = index;
    }
}


void
MSRoutingEngine::initWeightConstants(const OptionsCont& oc) {
    if (oc.getFloat("weights.priority-factor") != 0) {
        myPriorityFactor = oc.getFloat("weights.priority-factor");
        if (myPriorityFactor < 0) {
            throw ProcessError(TL("weights.priority-factor cannot be negative."));
        }
        myMinEdgePriority = std::numeric_limits<double>::max();
        double maxEdgePriority = -std::numeric_limits<double>::max();
        for (const MSEdge* const edge : MSNet::getInstance()->getEdgeControl().getEdges()) {
            maxEdgePriority = MAX2(maxEdgePriority, (double)edge->getPriority());
            myMinEdgePriority = MIN2(myMinEdgePriority, (double)edge->getPriority());
        }
        myEdgePriorityRange = maxEdgePriority - myMinEdgePriority;
        if (myEdgePriorityRange == 0) {
            WRITE_WARNING(TL("Option weights.priority-factor does not take effect because all edges have the same priority"));
            myPriorityFactor = 0;
        }
    }
    myDynamicRandomness = oc.getBool("weights.random-factor.dynamic");
    myHaveExtras = gRoutingPreferences || myPriorityFactor != 0 || gWeightsRandomFactor != 0;
}


void
MSRoutingEngine::_initEdgeWeights(std::vector<double>& edgeSpeeds, std::vector<std::vector<double> >& pastEdgeSpeeds) {
    if (edgeSpeeds.empty()) {
        const OptionsCont& oc = OptionsCont::getOptions();
        const bool useLoaded = oc.getBool("device.rerouting.init-with-loaded-weights");
        const double currentSecond = SIMTIME;
        for (const MSEdge* const edge : MSNet::getInstance()->getEdgeControl().getEdges()) {
            while (edge->getNumericalID() >= (int)edgeSpeeds.size()) {
                edgeSpeeds.push_back(0);
                if (myAdaptationSteps > 0) {
                    pastEdgeSpeeds.push_back(std::vector<double>());
                }
                if (MSGlobals::gWeightsSeparateTurns && edgeSpeeds == myEdgeSpeeds) {
                    myEdgeTravelTimes.push_back(TimeAndCount(0, 0));
                }
            }
            if (useLoaded) {
                edgeSpeeds[edge->getNumericalID()] = edge->getLength() / MSNet::getTravelTime(edge, nullptr, currentSecond);
            } else {
                edgeSpeeds[edge->getNumericalID()] = edge->getMeanSpeed();
            }
            if (myAdaptationSteps > 0) {
                pastEdgeSpeeds[edge->getNumericalID()] = std::vector<double>(myAdaptationSteps, edgeSpeeds[edge->getNumericalID()]);
            }
        }
        myLastAdaptation = MSNet::getInstance()->getCurrentTimeStep();
    }
}


double
MSRoutingEngine::getEffort(const MSEdge* const e, const SUMOVehicle* const v, double) {
    const int id = e->getNumericalID();
    if (id < (int)myEdgeSpeeds.size()) {
        return MAX2(e->getLength() / MAX2(myEdgeSpeeds[id], NUMERICAL_EPS), e->getMinimumTravelTime(v));
    }
    return e->getMinimumTravelTime(v);
}


double
MSRoutingEngine::getEffortBike(const MSEdge* const e, const SUMOVehicle* const v, double) {
    const int id = e->getNumericalID();
    if (id < (int)myEdgeBikeSpeeds.size()) {
        return MAX2(e->getLength() / MAX2(myEdgeBikeSpeeds[id], NUMERICAL_EPS), e->getMinimumTravelTime(v));
    }
    return e->getMinimumTravelTime(v);
}


double
MSRoutingEngine::getEffortExtra(const MSEdge* const e, const SUMOVehicle* const v, double t) {
    double effort = (!myBikeSpeeds || v == nullptr || v->getVClass() != SVC_BICYCLE
                     ? getEffort(e, v, t)
                     : getEffortBike(e, v, t));
    applyExtras(e, v, SIMSTEP, effort);
    return effort;
}


double
MSRoutingEngine::getAssumedSpeed(const MSEdge* edge, const SUMOVehicle* veh) {
    return edge->getLength() / myEffortFunc(edge, veh, 0);
}


SUMOTime
MSRoutingEngine::adaptEdgeEfforts(SUMOTime currentTime) {
    initEdgeWeights(SVC_PASSENGER);
    if (myBikeSpeeds) {
        initEdgeWeights(SVC_BICYCLE);
    }
    if (MSNet::getInstance()->getVehicleControl().getDepartedVehicleNo() == 0) {
        return myAdaptationInterval;
    }
    {
#ifdef HAVE_FOX
        FXMutexLock lock(myRouteCacheMutex);
#endif
        myCachedRoutes.clear();
    }
    const MSEdgeVector& edges = MSNet::getInstance()->getEdgeControl().getEdges();
    const double newWeightFactor = (double)(1. - myAdaptationWeight);
    for (const MSEdge* const e : edges) {
        if (e->isDelayed()) {
            const int id = e->getNumericalID();
            double currSpeed = e->getMeanSpeed();
            if (MSGlobals::gWeightsSeparateTurns > 0 && e->getNumSuccessors() > 1) {
                currSpeed = patchSpeedForTurns(e, currSpeed);
            }
#ifdef DEBUG_SEPARATE_TURNS
            if (DEBUG_COND(e->getLanes()[0])) {
                std::cout << SIMTIME << " edge=" << e->getID()
                          << " meanSpeed=" << e->getMeanSpeed()
                          << " currSpeed=" << currSpeed
                          << " oldestSpeed=" << myPastEdgeSpeeds[id][myAdaptationStepsIndex]
                          << " oldAvg=" << myEdgeSpeeds[id]
                          << "\n";
            }
#endif
            const double oldSmoothedSpeed = myEdgeSpeeds[id];
            if (myAdaptationSteps > 0) {
                // moving average
                myEdgeSpeeds[id] += (currSpeed - myPastEdgeSpeeds[id][myAdaptationStepsIndex]) / myAdaptationSteps;
                myPastEdgeSpeeds[id][myAdaptationStepsIndex] = currSpeed;
                if (myBikeSpeeds) {
                    const double currBikeSpeed = e->getMeanSpeedBike();
                    myEdgeBikeSpeeds[id] += (currBikeSpeed - myPastEdgeBikeSpeeds[id][myAdaptationStepsIndex]) / myAdaptationSteps;
                    myPastEdgeBikeSpeeds[id][myAdaptationStepsIndex] = currBikeSpeed;
                }
            } else {
                // exponential moving average
                if (currSpeed != myEdgeSpeeds[id]) {
                    myEdgeSpeeds[id] = myEdgeSpeeds[id] * myAdaptationWeight + currSpeed * newWeightFactor;
                }
                if (myBikeSpeeds) {
                    const double currBikeSpeed = e->getMeanSpeedBike();
                    if (currBikeSpeed != myEdgeBikeSpeeds[id]) {
                        myEdgeBikeSpeeds[id] = myEdgeBikeSpeeds[id] * myAdaptationWeight + currBikeSpeed * newWeightFactor;
                    }
                }
            }
            // sparse CCH re-customization: remember which edges actually
            // moved; the update threshold is applied at the customize barrier
            if (myCCHGraph != nullptr && myEdgeSpeeds[id] != oldSmoothedSpeed) {
                markCCHEdgeDirty(e);
            }
        }
    }
    if (myAdaptationSteps > 0) {
        myAdaptationStepsIndex = (myAdaptationStepsIndex + 1) % myAdaptationSteps;
    }
    myLastAdaptation = currentTime;
    // Re-customize the CCH metric from the freshly updated speed tables and
    // publish it (double-buffered). Runs on the main thread after the speed
    // update; no worker query is in flight at this point.
    //
    // Lazy, following the CH weightPeriod pattern (initRouter): only rebuild
    // a metric somebody routed on. If no query consumed the published metric
    // since the last customize, the current one is still unread -- skip the
    // rebuild (measured on Lausanne: the unconditional per-tick customize was
    // 80% of meso wall time at adaptation-interval 2, and ran even with
    // device.rerouting.probability 0).
    if (myCCHGraph != nullptr && myCCHQueried.exchange(false, std::memory_order_acq_rel)) {
        customizeCCH();
    }
    if (OptionsCont::getOptions().isSet("device.rerouting.output")) {
        OutputDevice& dev = OutputDevice::getDeviceByOption("device.rerouting.output");
        dev.openTag(SUMO_TAG_INTERVAL);
        dev.writeAttr(SUMO_ATTR_ID, "device.rerouting");
        dev.writeAttr(SUMO_ATTR_BEGIN, STEPS2TIME(currentTime));
        dev.writeAttr(SUMO_ATTR_END, STEPS2TIME(currentTime + myAdaptationInterval));
        for (const MSEdge* e : edges) {
            dev.openTag(SUMO_TAG_EDGE);
            dev.writeAttr(SUMO_ATTR_ID, e->getID());
            dev.writeAttr("traveltime", myEffortFunc(e, nullptr, STEPS2TIME(currentTime)));
            if (myBikeSpeeds) {
                // @note edge-priority is not included here
                dev.writeAttr("traveltimeBike", getEffortBike(e, nullptr, STEPS2TIME(currentTime)));
            }
            dev.closeTag();
        }
        dev.closeTag();
    }
    return myAdaptationInterval;
}


void
MSRoutingEngine::initCCH() {
    if (myCCHGraph != nullptr) {
        return;  // built once
    }
    // One metric per vehicle class actually present in the demand (plus
    // passenger). All classes share ONE CCH topology (the union); they differ
    // only by which arcs are inf_weight (permissions + closures). Enumerate
    // loaded vTypes -- parsed from additional files before any routing.
    std::set<SUMOVehicleClass> classes;
    classes.insert(SVC_PASSENGER);
    std::vector<std::string> vtypeIDs;
    MSNet::getInstance()->getVehicleControl().insertVTypeIDs(vtypeIDs);
    for (const std::string& id : vtypeIDs) {
        MSVehicleType* vt = MSNet::getInstance()->getVehicleControl().getVType(id, nullptr, true);
        if (vt != nullptr) {
            classes.insert(vt->getVehicleClass());
        }
    }
    myCCHGraph = new CCHGraph(MSEdge::getAllEdges());  // union topology; class masks prime at first fill
    const OptionsCont& oc = OptionsCont::getOptions();
    myCCHUpdateFactor = oc.getFloat("device.rerouting.cch-update-threshold.factor");
    myCCHUpdateConstant = STEPS2TIME(string2time(oc.getString("device.rerouting.cch-update-threshold.constant")));
    myCCHValidate = getenv("SUMO_CCH_VALIDATE") != nullptr;
    int edgeSpace = 0;
    for (const MSEdge* e : MSEdge::getAllEdges()) {
        edgeSpace = MAX2(edgeSpace, e->getNumericalID() + 1);
    }
    for (int i = 0; i < 2; i++) {
        // NaN = "never applied": the first pending occurrence always passes
        // the threshold and primes the entry
        myCCHAppliedEffort[i].assign(edgeSpace, std::numeric_limits<double>::quiet_NaN());
        myCCHPendingFlag[i].assign(edgeSpace, 0);
        myCCHPendingList[i].clear();
    }
    for (const SUMOVehicleClass vc : classes) {
        CCHClass* c = new CCHClass();
        c->vClass = vc;
        c->weight[0].resize(myCCHGraph->arcCount());
        c->weight[1].resize(myCCHGraph->arcCount());
        c->partial = std::make_shared<RoutingKit::CustomizableContractionHierarchyPartialCustomization>(myCCHGraph->cch());
        myCCHClasses.push_back(c);
        myCCHByClass[vc] = c;
    }
    customizeCCH();  // publish initial metrics so the first queries succeed
}


void
MSRoutingEngine::markCCHEdgeDirty(const MSEdge* e) {
    const int id = e->getNumericalID();
    if (id < 0 || id >= (int)myCCHPendingFlag[0].size()) {
        return;
    }
    for (int i = 0; i < 2; i++) {
        if (!myCCHPendingFlag[i][id]) {
            myCCHPendingFlag[i][id] = 1;
            myCCHPendingList[i].push_back(e);
        }
    }
}


void
MSRoutingEngine::invalidateCCHEdge(const MSEdge* e) {
    if (myCCHGraph == nullptr) {
        return;
    }
    const int id = e->getNumericalID();
    if (id < 0 || id >= (int)myCCHPendingFlag[0].size()) {
        return;
    }
    markCCHEdgeDirty(e);
    // NaN sentinel: the threshold always passes, so a permission flip
    // (closure / re-opening) reaches the metric at the next barrier even
    // though the speed table did not move
    myCCHAppliedEffort[0][id] = std::numeric_limits<double>::quiet_NaN();
    myCCHAppliedEffort[1][id] = std::numeric_limits<double>::quiet_NaN();
}


void
MSRoutingEngine::customizeCCH() {
    const double now = STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep());
    if (myCCHClasses.empty()) {
        return;
    }
    // buffers flip in lockstep across classes (every call processes all)
    const int backShared = 1 - myCCHClasses.front()->frontIndex;
    // Threshold pass, once (efforts read the class-shared speed table):
    // accept edges whose effort moved by more than BOTH bounds since this
    // buffer last applied them -- the sufficientSaving() analog. Rejected
    // edges stay pending; their drift keeps accumulating against the same
    // applied value, so a slow trend eventually passes while filter jitter
    // never does.
    std::vector<const MSEdge*> accepted;
    if (myCCHClasses.front()->metric[backShared] != nullptr) {
        std::vector<const MSEdge*> stillPending;
        for (const MSEdge* e : myCCHPendingList[backShared]) {
            const int id = e->getNumericalID();
            const double effNow = myEffortFunc(e, nullptr, now);
            const double effApplied = myCCHAppliedEffort[backShared][id];
            bool pass = true;
            if (!std::isnan(effApplied) && effApplied > 0. && effNow > 0.) {
                const double hi = MAX2(effNow, effApplied);
                const double lo = MIN2(effNow, effApplied);
                pass = (hi / lo > myCCHUpdateFactor) && (hi - lo > myCCHUpdateConstant);
            }
            if (pass) {
                accepted.push_back(e);
                myCCHAppliedEffort[backShared][id] = effNow;
                myCCHPendingFlag[backShared][id] = 0;
            } else {
                stillPending.push_back(e);
            }
        }
        myCCHPendingList[backShared].swap(stillPending);
    }
    bool fullRebuild = false;
    for (CCHClass* c : myCCHClasses) {
        const int back = 1 - c->frontIndex;
        // Fill from live speeds via myEffortFunc, masking arcs the class is
        // not permitted on -- this is where per-class permissions AND active
        // closures (a permissions change) become inf_weight.
        //
        // myEffortFunc (not plain getEffort) keeps the metric consistent with
        // every other cost evaluation in the router: TAZ source seeding in
        // CCHRouter::compute and recomputeCosts (the equivalence oracle) both
        // go through myOperation == myEffortFunc. When weights.priority-factor
        // is active, myEffortFunc is getEffortExtra and the static per-edge
        // priority multiplier is baked into the metric here -- including each
        // folded via edge, which gets its own multiplier inside
        // viaChainEffort, mirroring A*'s updateViaEdgeCost behavior.
        //
        // INVARIANT (enforced by the guard in initRouter): the only extra that
        // may be active together with CCH is the priority factor. The
        // vehicle-dependent extras (weights.random-factor, routing
        // preferences, bike-speeds) stay rejected -- getEffortExtra with the
        // nullptr reference vehicle used here is only safe because the random
        // and preference branches (which dereference the vehicle) are
        // guaranteed off. Stock SUMO relies on the same nullptr-safety when
        // writing device.rerouting.output.
        if (c->metric[back] == nullptr) {
            // first use of this buffer: no previous state to diff against
            myCCHGraph->fillInputWeights(myEffortFunc, c->vClass, nullptr, now, c->weight[back]);
            c->metric[back] = std::make_shared<RoutingKit::CustomizableContractionHierarchyMetric>(
                                  myCCHGraph->cch(), c->weight[back]);
            c->metric[back]->customize();  // serial (~2.7ms/class); avoids OpenMP-in-FOX oversubscription
            fullRebuild = true;
        } else {
            // SPARSE PATH: metric[back] is the customization of the current
            // contents of weight[back]. Recompute only the arcs of accepted
            // edges (edge->arc reverse image), write the ones that moved,
            // and propagate through affected triangles only. Cost scales
            // with traffic transitions, not with the network.
            std::vector<unsigned>& applied = c->weight[back];
            c->partial->reset(myCCHGraph->cch());
            unsigned changed = 0;
            for (const MSEdge* e : accepted) {
                for (const unsigned a : myCCHGraph->arcsOfEdge(e)) {
                    const unsigned newW = myCCHGraph->computeArcWeight(a, myEffortFunc, c->vClass, nullptr, now);
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
            if (myCCHValidate) {
                // debug: full-customize the same weights into a scratch metric
                // and compare -- abort on the first divergent arc
                RoutingKit::CustomizableContractionHierarchyMetric ref(myCCHGraph->cch(), applied);
                ref.customize();
                for (unsigned a = 0; a < (unsigned)ref.forward.size(); ++a) {
                    if (ref.forward[a] != c->metric[back]->forward[a]
                            || ref.backward[a] != c->metric[back]->backward[a]) {
                        throw ProcessError("CCH partial validation failed at t=" + toString(now)
                                           + " class=" + toString(c->vClass) + " cchArc=" + toString(a)
                                           + " marked=" + toString(changed)
                                           + " fw=" + toString(c->metric[back]->forward[a]) + "/" + toString(ref.forward[a])
                                           + " bw=" + toString(c->metric[back]->backward[a]) + "/" + toString(ref.backward[a]));
                    }
                }
            }
        }
        // Publish lock-free (see class doc): main-thread release store at the
        // barrier, worker acquire-loads on the hot path.
        c->frontIndex = back;
        c->front.store(c->metric[back].get(), std::memory_order_release);
    }
    if (fullRebuild) {
        // a full fill matched every arc to the live efforts: pending entries
        // for this buffer are stale; NaN re-arms the first-change auto-pass
        for (const MSEdge* e : myCCHPendingList[backShared]) {
            myCCHPendingFlag[backShared][e->getNumericalID()] = 0;
        }
        myCCHPendingList[backShared].clear();
        myCCHAppliedEffort[backShared].assign(myCCHAppliedEffort[backShared].size(),
                                              std::numeric_limits<double>::quiet_NaN());
    }
}


const RoutingKit::CustomizableContractionHierarchyMetric*
MSRoutingEngine::getPublishedCCHMetric(SUMOVehicleClass vClass) {
    const auto it = myCCHByClass.find(vClass);
    if (it == myCCHByClass.end()) {
        return nullptr;  // class has no CCH metric -> caller falls back to A*
    }
    // mark the metric as consumed so the next adaptation barrier refreshes it.
    // Test before set: an unconditional store from every worker query would
    // ping-pong the cache line between cores; the read is shared and the
    // store fires once per adaptation window (relaxed: only gates the
    // customize cadence, never data visibility)
    if (!myCCHQueried.load(std::memory_order_relaxed)) {
        myCCHQueried.store(true, std::memory_order_relaxed);
    }
    return it->second->front.load(std::memory_order_acquire);
}


double
MSRoutingEngine::patchSpeedForTurns(const MSEdge* edge, double currSpeed) {
    const double length = edge->getLength();
    double maxSpeed = 0;
    for (const auto& pair : edge->getViaSuccessors()) {
        if (pair.second == nullptr) {
            continue;
        }
        TimeAndCount& tc = myEdgeTravelTimes[pair.second->getNumericalID()];
        if (tc.second > 0) {
            const double avgSpeed = length / STEPS2TIME(tc.first / tc.second);
            maxSpeed = MAX2(avgSpeed, maxSpeed);
        }
    }
    if (maxSpeed > 0) {
        // perform correction
        const double correctedSpeed = MSGlobals::gWeightsSeparateTurns * maxSpeed + (1 - MSGlobals::gWeightsSeparateTurns) * currSpeed;
        for (const auto& pair : edge->getViaSuccessors()) {
            if (pair.second == nullptr) {
                continue;
            }
            const int iid = pair.second->getNumericalID();
            TimeAndCount& tc = myEdgeTravelTimes[iid];
            if (tc.second > 0) {
                const double avgSpeed = length / STEPS2TIME(tc.first / tc.second);
                if (avgSpeed < correctedSpeed) {
                    double internalTT = pair.second->getLength() / pair.second->getSpeedLimit();
                    internalTT += (length / avgSpeed - length / correctedSpeed) * MSGlobals::gWeightsSeparateTurns;
                    const double origInternalSpeed = myEdgeSpeeds[iid];
                    const double newInternalSpeed = pair.second->getLength() / internalTT;
                    const double origCurrSpeed = myPastEdgeSpeeds[iid][myAdaptationStepsIndex];

                    myEdgeSpeeds[iid] = newInternalSpeed;
                    // to ensure myEdgeSpeed reverts to the speed limit
                    // when there are no updates, we also have to patch
                    // myPastEdgeSpeeds with a virtual value that is consistent
                    // with the updated speed
                    // note: internal edges were handled before the normal ones
                    const double virtualSpeed = (newInternalSpeed - (origInternalSpeed - origCurrSpeed / myAdaptationSteps)) * myAdaptationSteps;
                    myPastEdgeSpeeds[iid][myAdaptationStepsIndex] = virtualSpeed;

#ifdef DEBUG_SEPARATE_TURNS
                    if (DEBUG_COND(pair.second->getLanes()[0])) {
                        std::cout << SIMTIME << " edge=" << edge->getID() << " to=" << pair.first->getID() << " via=" << pair.second->getID()
                                  << " origSpeed=" << currSpeed
                                  << " maxSpeed=" << maxSpeed
                                  << " correctedSpeed=" << correctedSpeed
                                  << " avgSpeed=" << avgSpeed
                                  << " internalTT=" << internalTT
                                  << " internalSpeed=" << origInternalSpeed
                                  << " newInternalSpeed=" << newInternalSpeed
                                  << " virtualSpeed=" << virtualSpeed
                                  << "\n";
                    }
#endif
                }
                if (myAdaptationStepsIndex == 0) {
                    tc.first = 0;
                    tc.second = 0;
                }
            }
        }
        return correctedSpeed;
    }
    return currSpeed;
}


ConstMSRoutePtr
MSRoutingEngine::getCachedRoute(const std::pair<const MSEdge*, const MSEdge*>& key) {
#ifdef HAVE_FOX
    // worker threads insert into the cache concurrently (RoutingTask::run)
    FXMutexLock lock(myRouteCacheMutex);
#endif
    auto routeIt = myCachedRoutes.find(key);
    if (routeIt != myCachedRoutes.end()) {
        return routeIt->second;
    }
    return nullptr;
}


void
MSRoutingEngine::initRouter(SUMOVehicle* vehicle) {
    OptionsCont& oc = OptionsCont::getOptions();
    const std::string routingAlgorithm = oc.getString("routing-algorithm");
    const bool hasPermissions = MSNet::getInstance()->hasPermissions();
    myBikeSpeeds = oc.getBool("device.rerouting.bike-speeds");
    myEffortFunc = ((gWeightsRandomFactor != 1 || myPriorityFactor != 0 || myBikeSpeeds || gRoutingPreferences) ? &MSRoutingEngine::getEffortExtra : &MSRoutingEngine::getEffort);

    SUMOAbstractRouter<MSEdge, SUMOVehicle>* router = nullptr;
    if (routingAlgorithm == "dijkstra") {
        router = new DijkstraRouter<MSEdge, SUMOVehicle>(MSEdge::getAllEdges(), true, myEffortFunc, nullptr, false, nullptr, true);
    } else if (routingAlgorithm == "astar") {
        typedef AStarRouter<MSEdge, SUMOVehicle, MSMapMatcher> AStar;
        std::shared_ptr<const AStar::LookupTable> lookup = nullptr;
        if (oc.isSet("astar.all-distances")) {
            lookup = std::make_shared<const AStar::FLT>(oc.getString("astar.all-distances"), (int)MSEdge::getAllEdges().size());
        } else if (oc.isSet("astar.landmark-distances") && vehicle != nullptr) {
            const double speedFactor = vehicle->getChosenSpeedFactor();
            // we need an exemplary vehicle with speedFactor 1
            vehicle->setChosenSpeedFactor(1);
            CHRouterWrapper<MSEdge, SUMOVehicle> chrouter(
                MSEdge::getAllEdges(), true, &MSNet::getTravelTime,
                string2time(oc.getString("begin")), string2time(oc.getString("end")), SUMOTime_MAX, hasPermissions, 1);
            lookup = std::make_shared<const AStar::LMLT>(oc.getString("astar.landmark-distances"), MSEdge::getAllEdges(), &chrouter,
                     nullptr, vehicle, "", oc.getInt("device.rerouting.threads"), MSNet::getInstance()->getMapMatcher());
            vehicle->setChosenSpeedFactor(speedFactor);
        }
        router = new AStar(MSEdge::getAllEdges(), true, myEffortFunc, lookup, true);
    } else if (routingAlgorithm == "CH" && !hasPermissions) {
        const SUMOTime weightPeriod = myAdaptationInterval > 0 ? myAdaptationInterval : SUMOTime_MAX;
        router = new CHRouter<MSEdge, SUMOVehicle>(
            MSEdge::getAllEdges(), true, myEffortFunc, vehicle == nullptr ? SVC_PASSENGER : vehicle->getVClass(), weightPeriod, true, false);
    } else if (routingAlgorithm == "CHWrapper" || routingAlgorithm == "CH") {
        // use CHWrapper instead of CH if the net has permissions
        const SUMOTime weightPeriod = myAdaptationInterval > 0 ? myAdaptationInterval : SUMOTime_MAX;
        router = new CHRouterWrapper<MSEdge, SUMOVehicle>(
            MSEdge::getAllEdges(), true, myEffortFunc,
            string2time(oc.getString("begin")), string2time(oc.getString("end")), weightPeriod, hasPermissions, oc.getInt("device.rerouting.threads"));
    } else if (routingAlgorithm == "CCH") {
        // CCH compiles all costs into a shared, per-class metric, so it can
        // express any effort modifier that is per-edge and vehicle-independent:
        // weights.priority-factor is supported -- the priority multiplier is
        // baked into the metric at customization time because the fill goes
        // through myEffortFunc (see customizeCCH). What a shared metric cannot
        // express are the per-VEHICLE modifiers among the getEffortExtra
        // extras: stochastic weight randomization (per-vehicle noise), routing
        // preferences (per-vType divisor) and the separate bicycle speed
        // table (per-class effort function). Those remain rejected here; the
        // customizeCCH invariant on the nullptr reference vehicle depends on
        // this rejection.
        if (gWeightsRandomFactor != 1 || gRoutingPreferences || myBikeSpeeds) {
            throw ProcessError(TL("Routing algorithm 'CCH' is incompatible with weights.random-factor != 1, routing preferences or bike-speeds. Disable these to use CCH."));
        }
        initCCH();  // build the immutable topology + publish an initial metric (once)
        // embedded fallback for non-passenger / prohibited / unreachable queries
        SUMOAbstractRouter<MSEdge, SUMOVehicle>* fallback =
            new AStarRouter<MSEdge, SUMOVehicle, MSMapMatcher>(MSEdge::getAllEdges(), true, myEffortFunc, nullptr, true);
        router = new CCHRouter<MSEdge, SUMOVehicle, CCHGraph>(
            myCCHGraph, &MSRoutingEngine::getPublishedCCHMetric, myEffortFunc, fallback);
    } else {
        throw ProcessError(TLF("Unknown routing algorithm '%'!", routingAlgorithm));
    }

    RailwayRouter<MSEdge, SUMOVehicle>* railRouter = nullptr;
    if (MSNet::getInstance()->hasBidiEdges()) {
        railRouter = new RailwayRouter<MSEdge, SUMOVehicle>(MSEdge::getAllEdges(), true, myEffortFunc, nullptr, false, true, false,
                oc.getFloat("railway.max-train-length"),
                oc.getFloat("weights.reversal-penalty"));
    }
    const int carWalk = SUMOVehicleParserHelper::parseCarWalkTransfer(oc, MSDevice_Taxi::hasFleet() || MSNet::getInstance()->getInsertionControl().hasTaxiFlow());
    const double taxiWait = STEPS2TIME(string2time(OptionsCont::getOptions().getString("persontrip.taxi.waiting-time")));
    MSTransportableRouter* transRouter = new MSTransportableRouter(MSNet::adaptIntermodalRouter, carWalk, taxiWait, routingAlgorithm, 0);
    myRouterProvider = new MSRouterProvider(router, nullptr, transRouter, railRouter);
#ifndef THREAD_POOL
#ifdef HAVE_FOX
    MFXWorkerThread::Pool& threadPool = MSNet::getInstance()->getEdgeControl().getThreadPool();
    if (threadPool.size() > 0) {
        const std::vector<MFXWorkerThread*>& threads = threadPool.getWorkers();
        if (static_cast<MSEdgeControl::WorkerThread*>(threads.front())->setRouterProvider(myRouterProvider)) {
            for (std::vector<MFXWorkerThread*>::const_iterator t = threads.begin() + 1; t != threads.end(); ++t) {
                static_cast<MSEdgeControl::WorkerThread*>(*t)->setRouterProvider(myRouterProvider->clone());
            }
        }
    }
#endif
#endif
}


void
MSRoutingEngine::reroute(SUMOVehicle& vehicle, const SUMOTime currentTime, const std::string& info,
                         const bool onInit, const bool silent, const Prohibitions& prohibited) {
    if (myRouterProvider == nullptr) {
        initRouter(&vehicle);
    }
    auto& router = myRouterProvider->getVehicleRouter(vehicle.getVClass());
#ifndef THREAD_POOL
#ifdef HAVE_FOX
    MFXWorkerThread::Pool& threadPool = MSNet::getInstance()->getEdgeControl().getThreadPool();
    if (threadPool.size() > 0) {
        threadPool.add(new RoutingTask(vehicle, currentTime, info, onInit, silent, prohibited));
        return;
    }
#endif
#endif
    if (!prohibited.empty()) {
        router.prohibit(prohibited);
    }
    try {
        vehicle.reroute(currentTime, info, router, onInit, myWithTaz, silent);
    } catch (ProcessError&) {
        if (!silent) {
            if (!prohibited.empty()) {
                router.prohibit(Prohibitions());
            }
            throw;
        }
    }
    if (!prohibited.empty()) {
        router.prohibit(Prohibitions());
    }
}


void
MSRoutingEngine::reroute(MSTransportable& t, const SUMOTime currentTime, const std::string& info,
                         const bool onInit, const bool silent, const Prohibitions& prohibited) {
    MSTransportableRouter& router = getIntermodalRouterTT(t.getRNGIndex(), prohibited);
#ifndef THREAD_POOL
#ifdef HAVE_FOX
    MFXWorkerThread::Pool& threadPool = MSNet::getInstance()->getEdgeControl().getThreadPool();
    if (threadPool.size() > 0) {
        // threadPool.add(new RoutingTask(t, currentTime, info, onInit, silent, prohibited));
        return;
    }
#endif
#endif
    if (!prohibited.empty()) {
        router.prohibit(prohibited);
    }
    try {
        t.reroute(currentTime, info, router, onInit, myWithTaz, silent);
    } catch (ProcessError&) {
        if (!silent) {
            if (!prohibited.empty()) {
                router.prohibit(Prohibitions());
            }
            throw;
        }
    }
    if (!prohibited.empty()) {
        router.prohibit(Prohibitions());
    }
}


void
MSRoutingEngine::setEdgeTravelTime(const MSEdge* const edge, const double travelTime) {
    myEdgeSpeeds[edge->getNumericalID()] = edge->getLength() / travelTime;
}

void
MSRoutingEngine::addEdgeTravelTime(const MSEdge& edge, const SUMOTime travelTime) {
    TimeAndCount& tc = myEdgeTravelTimes[edge.getNumericalID()];
    tc.first += travelTime;
    tc.second += 1;
}


MSVehicleRouter&
MSRoutingEngine::getRouterTT(const int rngIndex, SUMOVehicleClass svc, const Prohibitions& prohibited) {
    if (myRouterProvider == nullptr) {
        initWeightUpdate();
        initEdgeWeights(svc);
        initRouter();
    }
#ifndef THREAD_POOL
#ifdef HAVE_FOX
    MFXWorkerThread::Pool& threadPool = MSNet::getInstance()->getEdgeControl().getThreadPool();
    if (threadPool.size() > 0) {
        auto& router = static_cast<MSEdgeControl::WorkerThread*>(threadPool.getWorkers()[rngIndex % MSGlobals::gNumThreads])->getRouter(svc);
        router.prohibit(prohibited);
        return router;
    }
#else
    UNUSED_PARAMETER(rngIndex);
#endif
#endif
    myRouterProvider->getVehicleRouter(svc).prohibit(prohibited);
    return myRouterProvider->getVehicleRouter(svc);
}


MSTransportableRouter&
MSRoutingEngine::getIntermodalRouterTT(const int rngIndex, const Prohibitions& prohibited) {
    if (myRouterProvider == nullptr) {
        initWeightUpdate();
        initEdgeWeights(SVC_PEDESTRIAN);
        initRouter();
    }
#ifndef THREAD_POOL
#ifdef HAVE_FOX
    MFXWorkerThread::Pool& threadPool = MSNet::getInstance()->getEdgeControl().getThreadPool();
    if (threadPool.size() > 0) {
        auto& router = static_cast<MSEdgeControl::WorkerThread*>(threadPool.getWorkers()[rngIndex % MSGlobals::gNumThreads])->getIntermodalRouter();
        router.prohibit(prohibited);
        return router;
    }
#else
    UNUSED_PARAMETER(rngIndex);
#endif
#endif
    myRouterProvider->getIntermodalRouter().prohibit(prohibited);
    return myRouterProvider->getIntermodalRouter();
}


void
MSRoutingEngine::cleanup() {
    myAdaptationInterval = -1; // responsible for triggering initEdgeWeights
    myPastEdgeSpeeds.clear();
    myEdgeSpeeds.clear();
    myEdgeTravelTimes.clear();
    myPastEdgeBikeSpeeds.clear();
    myEdgeBikeSpeeds.clear();
    // @todo recheck. calling release crashes in parallel routing
    //for (auto& item : myCachedRoutes) {
    //    item.second->release();
    //}
    {
#ifdef HAVE_FOX
        FXMutexLock lock(myRouteCacheMutex);
#endif
        myCachedRoutes.clear();
    }
    myAdaptationStepsIndex = 0;
    // free the CCH state so a subsequent load (libsumo / GUI reload) rebuilds
    // it against the new network; the router clones referencing it were
    // deleted together with the worker threads / router provider
    delete myCCHGraph;
    myCCHGraph = nullptr;
    for (CCHClass* c : myCCHClasses) {
        delete c;
    }
    myCCHClasses.clear();
    myCCHByClass.clear();
    myCCHQueried.store(false, std::memory_order_relaxed);
    myCCHScratchWeight.clear();
    for (int i = 0; i < 2; i++) {
        myCCHAppliedEffort[i].clear();
        myCCHPendingFlag[i].clear();
        myCCHPendingList[i].clear();
    }
#ifdef HAVE_FOX
    if (MSGlobals::gNumThreads > 1) {
        // router deletion is done in thread destructor
        myRouterProvider = nullptr;
        return;
    }
#endif
    delete myRouterProvider;
    myRouterProvider = nullptr;
}


void
MSRoutingEngine::saveState(OutputDevice& out) {
    if (myEdgeSpeeds.size() == 0) {
        return;
    }
    out.openTag(SUMO_TAG_ROUTINGENGINE);
    out.writeAttr(SUMO_ATTR_LAST, myLastAdaptation);
    out.writeAttr(SUMO_ATTR_INDEX, myAdaptationStepsIndex);
    const MSEdgeVector& edges = MSNet::getInstance()->getEdgeControl().getEdges();
    for (const MSEdge* const e : edges) {
        if (e->isDelayed()) {
            const int id = e->getNumericalID();
            out.openTag(SUMO_TAG_EDGE);
            out.writeAttr(SUMO_ATTR_ID, e->getID());
            out.writeAttr(SUMO_ATTR_SPEED, myEdgeSpeeds[id]);
            if (myAdaptationSteps > 0) {
                out.writeAttr(SUMO_ATTR_PASTSPEED, myPastEdgeSpeeds[id]);
            }
            if (myBikeSpeeds) {
                out.writeAttr(SUMO_ATTR_BIKESPEED, myEdgeBikeSpeeds[id]);
                if (myAdaptationSteps > 0) {
                    out.writeAttr(SUMO_ATTR_PASTBIKESPEED, myPastEdgeBikeSpeeds[id]);
                }
            }
            out.closeTag();
        }
    }
    out.closeTag();
}


void
MSRoutingEngine::loadState(const SUMOSAXAttributes& attrs) {
    const MSEdge* const e = MSEdge::dictionary(attrs.getString(SUMO_ATTR_ID));
    e->markDelayed();
    const int id = e->getNumericalID();
    bool checkedSteps = false;
    bool checkedBikeSpeeds = false;
    bool ok = true;
    if ((int)myEdgeSpeeds.size() > id) {
        myEdgeSpeeds[id] = attrs.get<double>(SUMO_ATTR_SPEED, nullptr, ok);
        if (myBikeSpeeds) {
            if (attrs.hasAttribute(SUMO_ATTR_BIKESPEED)) {
                myEdgeBikeSpeeds[id] = attrs.get<double>(SUMO_ATTR_BIKESPEED, nullptr, ok);
            } else if (!checkedBikeSpeeds) {
                checkedBikeSpeeds = true;
                WRITE_WARNING("Bike speeds missing in loaded state");
            }
        }
        if (myAdaptationSteps > 0) {
            const std::vector<double> speeds = attrs.getOpt<std::vector<double> >(SUMO_ATTR_PASTSPEED, nullptr, ok);
            if ((int)speeds.size() == myAdaptationSteps) {
                myPastEdgeSpeeds[id] = speeds;
                if (myBikeSpeeds && attrs.hasAttribute(SUMO_ATTR_PASTBIKESPEED)) {
                    myPastEdgeBikeSpeeds[id] = attrs.getOpt<std::vector<double> >(SUMO_ATTR_PASTBIKESPEED, nullptr, ok);
                }
            } else if (!checkedSteps) {
                checkedSteps = true;
                WRITE_WARNING("Number of adaptation speeds in loaded state doesn't match option --device.rerouting.adaptation-steps");
            }
        }
    }
}


#ifdef HAVE_FOX
void
MSRoutingEngine::waitForAll() {
#ifndef THREAD_POOL
    MFXWorkerThread::Pool& threadPool = MSNet::getInstance()->getEdgeControl().getThreadPool();
    if (threadPool.size() > 0) {
        threadPool.waitAll();
    }
#endif
}


// ---------------------------------------------------------------------------
// MSRoutingEngine::RoutingTask-methods
// ---------------------------------------------------------------------------
void
MSRoutingEngine::RoutingTask::run(MFXWorkerThread* context) {
    SUMOAbstractRouter<MSEdge, SUMOVehicle>& router = static_cast<MSEdgeControl::WorkerThread*>(context)->getRouter(myVehicle.getVClass());
    if (!myProhibited.empty()) {
        router.prohibit(myProhibited);
    }
    try {
        myVehicle.reroute(myTime, myInfo, router, myOnInit, myWithTaz, mySilent);
    } catch (ProcessError&) {
        if (!mySilent) {
            if (!myProhibited.empty()) {
                router.prohibit(Prohibitions());
            }
            throw;
        }
    }
    if (!myProhibited.empty()) {
        router.prohibit(Prohibitions());
    }
    const MSEdge* source = *myVehicle.getRoute().begin();
    const MSEdge* dest = myVehicle.getRoute().getLastEdge();
    if (source->isTazConnector() && dest->isTazConnector()) {
        const std::pair<const MSEdge*, const MSEdge*> key = std::make_pair(source, dest);
        FXMutexLock lock(myRouteCacheMutex);
        if (MSRoutingEngine::myCachedRoutes.find(key) == MSRoutingEngine::myCachedRoutes.end()) {
            MSRoutingEngine::myCachedRoutes[key] = myVehicle.getRoutePtr();
        }
    }
}


#endif


/****************************************************************************/
