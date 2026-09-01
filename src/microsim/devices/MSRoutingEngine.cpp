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
#include <microsim/MSVehicle.h>
#include <microsim/MSRoute.h>
#include <microsim/MSEdgeWeightsStorage.h>
#include <mesosim/MEVehicle.h>
#include <libsumo/TraCIConstants.h>
#include <utils/vehicle/SUMOVehicleParameter.h>
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
#include <utils/router/CCHGraph.h>
#include <utils/router/CCHMetricFamily.h>
#include <utils/router/CCHRouter.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <routingkit/customizable_contraction_hierarchy.h>
#pragma GCC diagnostic pop

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
MSCCHGraph* MSRoutingEngine::myCCHGraph = nullptr;
MSCCHMetricFamily* MSRoutingEngine::myCCHLive = nullptr;
MSCCHMetricFamily* MSRoutingEngine::myCCHFreeflow = nullptr;


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
    bool cchEdgeMoved = false;
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
            // moved; the update deadband is applied at the customize barrier
            if (myCCHLive != nullptr && myEdgeSpeeds[id] != oldSmoothedSpeed) {
                myCCHLive->markDirty(e);
                cchEdgeMoved = true;
            }
        }
    }
    if (myAdaptationSteps > 0) {
        myAdaptationStepsIndex = (myAdaptationStepsIndex + 1) % myAdaptationSteps;
    }
    myLastAdaptation = currentTime;
    // The device's CCH customization barrier: re-customize the metrics from
    // the freshly updated speed tables and publish (double-buffered). Runs
    // on the main thread after the speed update; no worker query is in
    // flight at this point. The family skips quiet barriers and runs that
    // never route (see CCHMetricFamily::atBarrier: measured on Lausanne, the
    // unconditional per-tick customize was 80% of meso wall time at
    // adaptation-interval 2, and ran even with rerouting probability 0).
    if (myCCHLive != nullptr) {
        myCCHLive->atBarrier(STEPS2TIME(currentTime), cchEdgeMoved);
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


MSCCHGraph*
MSRoutingEngine::ensureCCHGraph() {
    if (myCCHGraph == nullptr) {
        myCCHGraph = new MSCCHGraph(MSEdge::getAllEdges());  // union topology; class masks prime at first fill
    }
    return myCCHGraph;
}


void
MSRoutingEngine::initCCH() {
    if (myCCHLive != nullptr) {
        return;  // device-side state built once (the graph alone may already
        // exist for MSNet's free-flow router)
    }
    // One LIVE metric per vehicle TYPE known when routing starts (see
    // utils/router/CCHMetricFamily.h for the keying and buffer semantics).
    // All types share ONE CCH topology (the union); route files stream, so
    // types appearing later register through the family's wanted list and
    // get their metric at the next customization barrier.
    ensureCCHGraph();
    const OptionsCont& oc = OptionsCont::getOptions();
    myCCHLive = new MSCCHMetricFamily(
        myCCHGraph, myEffortFunc, &MSRoutingEngine::getEffort,
        oc.getFloat("device.rerouting.cch-update-threshold.factor"),
        STEPS2TIME(string2time(oc.getString("device.rerouting.cch-update-threshold.constant"))),
        &MSRoutingEngine::buildCCHRefVehicle);
    std::vector<std::string> vtypeIDs;
    MSNet::getInstance()->getVehicleControl().insertVTypeIDs(vtypeIDs);
    for (const std::string& id : vtypeIDs) {
        const MSVehicleType* vt = MSNet::getInstance()->getVehicleControl().getVType(id, nullptr, true);
        if (vt != nullptr && !vt->isVehicleSpecific()) {
            myCCHLive->seedKey(vt);
        }
    }
    // publish initial metrics so the first queries succeed
    myCCHLive->customize(STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep()));
}


SUMOVehicle*
MSRoutingEngine::buildCCHRefVehicle(const MSVehicleType* type) {
    // The effort-reference vehicle: constructed DIRECTLY (what buildVehicle
    // does minus initVehicle), so it is never counted in the vehicle
    // statistics, never given devices and never inserted -- it exists only
    // so every metric fill evaluates the same reference. The mean speed
    // factor makes it deterministic (the findRoute precedent) and the fixed
    // id makes the frozen weights.random-factor realization reproducible
    // (the random seed is a hash of the id).
    SUMOVehicleParameter* pars = new SUMOVehicleParameter();
    pars->id = "cchRef:" + type->getID();
    const MSEdge* refEdge = nullptr;
    for (const MSEdge* e : MSEdge::getAllEdges()) {
        if (!e->isInternal() && !e->isTazConnector()) {
            refEdge = e;
            break;
        }
    }
    ConstMSRoutePtr route = std::make_shared<MSRoute>(pars->id, ConstMSEdgeVector({refEdge}), false, nullptr, StopParVector());
    if (MSGlobals::gUseMesoSim) {
        return new MEVehicle(pars, route, const_cast<MSVehicleType*>(type),
                             type->getSpeedFactor().getParameter(0));
    }
    return new MSVehicle(pars, route, const_cast<MSVehicleType*>(type),
                         type->getSpeedFactor().getParameter(0));
}


void
MSRoutingEngine::invalidateCCHEdge(const MSEdge* e) {
    if (myCCHGraph == nullptr) {
        return;
    }
    // the primed connection masks reflect the OLD successor lists; the next
    // fill of either family re-primes from the live ones. Safe to do right
    // here: masks are only read at fill time, and every fill runs on the
    // main thread (the device barrier, the free-flow repair) -- never on a
    // query thread.
    myCCHGraph->invalidateClassMasks();
    if (myCCHFreeflow != nullptr) {
        myCCHFreeflow->flagPermissionsStale();
    }
    if (myCCHLive != nullptr) {
        myCCHLive->invalidateEdge(e);
    }
}


const RoutingKit::CustomizableContractionHierarchyMetric*
MSRoutingEngine::getPublishedCCHMetric(SUMOVehicleClass /* vClass */, SUMOTime /* time */, const SUMOVehicle* veh) {
    // the published metrics always track the live speeds; the query time
    // only matters for duarouter's per-weight-period metrics. Lookup is by
    // vehicle TYPE (the metric's exactness key); a vehicle-specific type
    // (TraCI-modified singular copy) routes on the exact fallback rather
    // than on another type's metric.
    if (veh == nullptr || myCCHLive == nullptr) {
        return nullptr;
    }
    const MSVehicleType* type = &veh->getVehicleType();
    if (type->isVehicleSpecific()) {
        return nullptr;
    }
    return myCCHLive->published(type);
}


const RoutingKit::CustomizableContractionHierarchyMetric*
MSRoutingEngine::getFreeflowCCHMetric(SUMOVehicleClass /* vClass */, SUMOTime /* time */, const SUMOVehicle* veh) {
    // MAIN-THREAD ONLY (see header): MSNet's routers serve TraCI, triggers
    // and the GUI, never the rerouting worker threads, so the family's lazy
    // creation and stale repair may run synchronously right here.
    if (veh == nullptr) {
        return nullptr;
    }
    // Everything MSNet::getTravelTime reads ahead of the free-flow layer is
    // per-vehicle and cannot live in a shared metric -> exact fallback:
    // individual TraCI edge weights, global TraCI edge weights, a routing
    // mode other than DEFAULT (the AGGREGATED modes belong to the device
    // metrics; the transient-permission modes already fall back inside
    // CCHRouter::compute).
    if (veh->getRoutingMode() != libsumo::ROUTING_MODE_DEFAULT
            || !MSNet::getInstance()->getWeightsStorage().empty()) {
        return nullptr;
    }
    const MSVehicle* const msVeh = dynamic_cast<const MSVehicle*>(veh);
    if (msVeh != nullptr && !msVeh->getWeightsStorage().empty()) {
        return nullptr;
    }
    const MSVehicleType* type = &veh->getVehicleType();
    if (type->isVehicleSpecific()) {
        return nullptr;
    }
    if (myCCHFreeflow == nullptr) {
        // free-flow efforts are static: a STATIC family with a single weight
        // period, lazily customized per type on first query
        myCCHFreeflow = new MSCCHMetricFamily(
            ensureCCHGraph(), &MSNet::getTravelTime, 0, SUMOTime_MAX,
            &MSRoutingEngine::buildCCHRefVehicle, nullptr);
    }
    return myCCHFreeflow->get(type, type->getVehicleClass(),
                              MSNet::getInstance()->getCurrentTimeStep(), veh);
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
        // CCH metrics are keyed by vehicle TYPE and filled with a reference
        // vehicle of the type (customizeCCH), mirroring duarouter's
        // RODUACCHMetrics: type/class-specific routing preferences, the
        // bicycle speed table, the type's maximum speed and the static
        // priority multiplier are captured exactly per metric.
        // weights.random-factor freezes one realization per metric -- the
        // same approximation CHRouterWrapper makes when building its
        // hierarchies; exact per-vehicle randomization remains the domain of
        // dijkstra and astar.
        initCCH();  // build the immutable topology + publish an initial metric (once)
        // embedded fallback for non-passenger / prohibited / unreachable queries
        SUMOAbstractRouter<MSEdge, SUMOVehicle>* fallback =
            new AStarRouter<MSEdge, SUMOVehicle, MSMapMatcher>(MSEdge::getAllEdges(), true, myEffortFunc, nullptr, true);
        router = new CCHRouter<MSEdge, SUMOVehicle, MSCCHGraph>(
            myCCHGraph, &MSRoutingEngine::getPublishedCCHMetric, myEffortFunc, true, fallback);
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
    delete myCCHLive;
    myCCHLive = nullptr;
    delete myCCHFreeflow;
    myCCHFreeflow = nullptr;
    delete myCCHGraph;
    myCCHGraph = nullptr;
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
