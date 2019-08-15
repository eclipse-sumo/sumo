/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2007-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSRoutingEngine.cpp
/// @author  Michael Behrisch
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Christoph Sommer
/// @author  Jakob Erdmann
/// @date    Tue, 04 Dec 2007
/// @version $Id$
///
// A device that performs vehicle rerouting based on current edge speeds
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "MSRoutingEngine.h"
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSVehicleControl.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/WrappingCommand.h>
#include <utils/common/StaticCommand.h>
#include <utils/common/StringUtils.h>
#include <utils/xml/SUMOSAXAttributes.h>
#include <utils/router/DijkstraRouter.h>
#include <utils/router/AStarRouter.h>
#include <utils/router/CHRouter.h>
#include <utils/router/CHRouterWrapper.h>


// ===========================================================================
// static member variables
// ===========================================================================
std::vector<double> MSRoutingEngine::myEdgeSpeeds;
std::vector<std::vector<double> > MSRoutingEngine::myPastEdgeSpeeds;
Command* MSRoutingEngine::myEdgeWeightSettingCommand = nullptr;
double MSRoutingEngine::myAdaptationWeight;
int MSRoutingEngine::myAdaptationSteps;
int MSRoutingEngine::myAdaptationStepsIndex = 0;
SUMOTime MSRoutingEngine::myAdaptationInterval = -1;
SUMOTime MSRoutingEngine::myLastAdaptation = -1;
bool MSRoutingEngine::myWithTaz;
SUMOAbstractRouter<MSEdge, SUMOVehicle>* MSRoutingEngine::myRouter = nullptr;
AStarRouter<MSEdge, SUMOVehicle, SUMOAbstractRouterPermissions<MSEdge, SUMOVehicle> >* MSRoutingEngine::myRouterWithProhibited = nullptr;
std::map<std::pair<const MSEdge*, const MSEdge*>, const MSRoute*> MSRoutingEngine::myCachedRoutes;
SUMOAbstractRouter<MSEdge, SUMOVehicle>::Operation MSRoutingEngine::myEffortFunc = &MSRoutingEngine::getEffort;
#ifdef HAVE_FOX
FXWorkerThread::Pool MSRoutingEngine::myThreadPool;
#endif


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
void
MSRoutingEngine::initWeightUpdate() {
    if (myAdaptationInterval == -1) {
        myEdgeWeightSettingCommand = nullptr;
        myEdgeSpeeds.clear();
        myAdaptationInterval = -1;
        myAdaptationSteps = -1;
        myLastAdaptation = -1;
        const OptionsCont& oc = OptionsCont::getOptions();
        myWithTaz = oc.getBool("device.rerouting.with-taz");
        myAdaptationInterval = string2time(oc.getString("device.rerouting.adaptation-interval"));
        myAdaptationWeight = oc.getFloat("device.rerouting.adaptation-weight");
        const SUMOTime period = string2time(oc.getString("device.rerouting.period"));
        if (myAdaptationWeight < 1. && myAdaptationInterval > 0) {
            myEdgeWeightSettingCommand = new StaticCommand<MSRoutingEngine>(&MSRoutingEngine::adaptEdgeEfforts);
            MSNet::getInstance()->getEndOfTimestepEvents()->addEvent(myEdgeWeightSettingCommand);
        } else if (period > 0) {
            WRITE_WARNING("Rerouting is useless if the edge weights do not get updated!");
        }
        OutputDevice::createDeviceByOption("device.rerouting.output", "weights", "meandata_file.xsd");
    }
}

// ---------------------------------------------------------------------------
// MSRoutingEngine-methods
// ---------------------------------------------------------------------------
void
MSRoutingEngine::initEdgeWeights() {
    if (myEdgeSpeeds.empty()) {
        const OptionsCont& oc = OptionsCont::getOptions();
        if (myAdaptationWeight == 0 || !oc.isDefault("device.rerouting.adaptation-steps")) {
            myAdaptationSteps = oc.getInt("device.rerouting.adaptation-steps");
        }
        const bool useLoaded = oc.getBool("device.rerouting.init-with-loaded-weights");
        const double currentSecond = SIMTIME;
        for (const MSEdge* const edge : MSNet::getInstance()->getEdgeControl().getEdges()) {
            while (edge->getNumericalID() >= (int)myEdgeSpeeds.size()) {
                myEdgeSpeeds.push_back(0);
                if (myAdaptationSteps > 0) {
                    myPastEdgeSpeeds.push_back(std::vector<double>());
                }
            }
            if (useLoaded) {
                myEdgeSpeeds[edge->getNumericalID()] = edge->getLength() / MSNet::getTravelTime(edge, nullptr, currentSecond);
            } else {
                myEdgeSpeeds[edge->getNumericalID()] = edge->getMeanSpeed();
            }
            if (myAdaptationSteps > 0) {
                myPastEdgeSpeeds[edge->getNumericalID()] = std::vector<double>(myAdaptationSteps, myEdgeSpeeds[edge->getNumericalID()]);
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
    return 0.;
}


double
MSRoutingEngine::getEffortExtra(const MSEdge* const e, const SUMOVehicle* const v, double t) {
    if (e->getBidiEdge() != nullptr && !e->getBidiEdge()->getLanes()[0]->isEmpty()) {
        // using std::numeric_limits<double>::max() causing router warnings
        return e->getLength() / NUMERICAL_EPS;
    }
    double effort = getEffort(e, v, t);
    if (gWeightsRandomFactor != 1.) {
        effort *= RandHelper::rand(1., gWeightsRandomFactor);
    }
    return effort;
}


double
MSRoutingEngine::getAssumedSpeed(const MSEdge* edge) {
    return edge->getLength() / myEffortFunc(edge, nullptr, 0);
}


SUMOTime
MSRoutingEngine::adaptEdgeEfforts(SUMOTime currentTime) {
    initEdgeWeights();
    if (MSNet::getInstance()->getVehicleControl().getDepartedVehicleNo() == 0) {
        return myAdaptationInterval;
    }
    std::map<std::pair<const MSEdge*, const MSEdge*>, const MSRoute*>::iterator it = myCachedRoutes.begin();
    for (; it != myCachedRoutes.end(); ++it) {
        it->second->release();
    }
    myCachedRoutes.clear();
    const MSEdgeVector& edges = MSNet::getInstance()->getEdgeControl().getEdges();
    if (myAdaptationSteps > 0) {
        // moving average
        for (MSEdgeVector::const_iterator i = edges.begin(); i != edges.end(); ++i) {
            if ((*i)->isDelayed()) {
                const int id = (*i)->getNumericalID();
                const double currSpeed = (*i)->getMeanSpeed();
                myEdgeSpeeds[id] += (currSpeed - myPastEdgeSpeeds[id][myAdaptationStepsIndex]) / myAdaptationSteps;
                myPastEdgeSpeeds[id][myAdaptationStepsIndex] = currSpeed;
            }
        }
        myAdaptationStepsIndex = (myAdaptationStepsIndex + 1) % myAdaptationSteps;
    } else {
        // exponential moving average
        const double newWeightFactor = (double)(1. - myAdaptationWeight);
        for (MSEdgeVector::const_iterator i = edges.begin(); i != edges.end(); ++i) {
            if ((*i)->isDelayed()) {
                const int id = (*i)->getNumericalID();
                const double currSpeed = (*i)->getMeanSpeed();
                if (currSpeed != myEdgeSpeeds[id]) {
                    myEdgeSpeeds[id] = myEdgeSpeeds[id] * myAdaptationWeight + currSpeed * newWeightFactor;
                }
            }
        }
    }
    myLastAdaptation = currentTime + DELTA_T; // because we run at the end of the time step
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
            dev.closeTag();
        }
        dev.closeTag();
    }
    return myAdaptationInterval;
}


const MSRoute*
MSRoutingEngine::getCachedRoute(const std::pair<const MSEdge*, const MSEdge*>& key) {
    auto routeIt = myCachedRoutes.find(key);
    if (routeIt != myCachedRoutes.end()) {
        return routeIt->second;
    }
    return nullptr;
}


void
MSRoutingEngine::reroute(SUMOVehicle& vehicle, const SUMOTime currentTime, const bool onInit) {
#ifdef HAVE_FOX
    const bool needThread = (myRouter == nullptr && myThreadPool.isFull());
#else
    const bool needThread = true;
#endif
    if (needThread && myRouter == nullptr) {
        OptionsCont& oc = OptionsCont::getOptions();
        const std::string routingAlgorithm = oc.getString("routing-algorithm");
        const bool mayHaveRestrictions = MSNet::getInstance()->hasPermissions() || oc.getInt("remote-port") != 0;
        myEffortFunc = (gWeightsRandomFactor != 1 || MSNet::getInstance()->hasBidiEdges()
                        ? &MSRoutingEngine::getEffortExtra
                        : &MSRoutingEngine::getEffort);
        if (routingAlgorithm == "dijkstra") {
            if (mayHaveRestrictions) {
                myRouter = new DijkstraRouter<MSEdge, SUMOVehicle, SUMOAbstractRouterPermissions<MSEdge, SUMOVehicle> >(
                    MSEdge::getAllEdges(), true, myEffortFunc);
            } else {
                myRouter = new DijkstraRouter<MSEdge, SUMOVehicle, SUMOAbstractRouter<MSEdge, SUMOVehicle> >(
                    MSEdge::getAllEdges(), true, myEffortFunc);
            }
        } else if (routingAlgorithm == "astar") {
            if (mayHaveRestrictions) {
                typedef AStarRouter<MSEdge, SUMOVehicle, SUMOAbstractRouterPermissions<MSEdge, SUMOVehicle> > AStar;
                std::shared_ptr<const AStar::LookupTable> lookup;
                if (oc.isSet("astar.all-distances")) {
                    lookup = std::make_shared<const AStar::FLT>(oc.getString("astar.all-distances"), (int)MSEdge::getAllEdges().size());
                } else if (oc.isSet("astar.landmark-distances")) {
                    const double speedFactor = vehicle.getChosenSpeedFactor();
                    // we need an exemplary vehicle with speedFactor 1
                    vehicle.setChosenSpeedFactor(1);
                    CHRouterWrapper<MSEdge, SUMOVehicle, SUMOAbstractRouterPermissions<MSEdge, SUMOVehicle> > router(
                        MSEdge::getAllEdges(), true, &MSNet::getTravelTime,
                        string2time(oc.getString("begin")), string2time(oc.getString("end")), std::numeric_limits<int>::max(), 1);
                    lookup = std::make_shared<const AStar::LMLT>(oc.getString("astar.landmark-distances"), MSEdge::getAllEdges(), &router, &vehicle, "", oc.getInt("device.rerouting.threads"));
                    vehicle.setChosenSpeedFactor(speedFactor);
                }
                myRouter = new AStar(MSEdge::getAllEdges(), true, myEffortFunc, lookup);
            } else {
                typedef AStarRouter<MSEdge, SUMOVehicle, SUMOAbstractRouter<MSEdge, SUMOVehicle> > AStar;
                std::shared_ptr<const AStar::LookupTable> lookup;
                if (oc.isSet("astar.all-distances")) {
                    lookup = std::shared_ptr<const AStar::LookupTable> (new AStar::FLT(oc.getString("astar.all-distances"), (int)MSEdge::getAllEdges().size()));
                } else if (oc.isSet("astar.landmark-distances")) {
                    const double speedFactor = vehicle.getChosenSpeedFactor();
                    // we need an exemplary vehicle with speedFactor 1
                    vehicle.setChosenSpeedFactor(1);
                    CHRouterWrapper<MSEdge, SUMOVehicle, SUMOAbstractRouter<MSEdge, SUMOVehicle> > router(
                        MSEdge::getAllEdges(), true, &MSNet::getTravelTime,
                        string2time(oc.getString("begin")), string2time(oc.getString("end")), std::numeric_limits<int>::max(), 1);
                    lookup = std::make_shared<const AStar::LMLT>(oc.getString("astar.landmark-distances"), MSEdge::getAllEdges(), &router, &vehicle, "", oc.getInt("device.rerouting.threads"));
                    vehicle.setChosenSpeedFactor(speedFactor);
                }
                myRouter = new AStar(MSEdge::getAllEdges(), true, myEffortFunc, lookup);
            }
        } else if (routingAlgorithm == "CH") {
            const SUMOTime weightPeriod = myAdaptationInterval > 0 ? myAdaptationInterval : std::numeric_limits<int>::max();
            if (mayHaveRestrictions) {
                myRouter = new CHRouter<MSEdge, SUMOVehicle, SUMOAbstractRouterPermissions<MSEdge, SUMOVehicle> >(
                    MSEdge::getAllEdges(), true, myEffortFunc, vehicle.getVClass(), weightPeriod, true);
            } else {
                myRouter = new CHRouter<MSEdge, SUMOVehicle, SUMOAbstractRouter<MSEdge, SUMOVehicle> >(
                    MSEdge::getAllEdges(), true, myEffortFunc, vehicle.getVClass(), weightPeriod, false);
            }
        } else if (routingAlgorithm == "CHWrapper") {
            const SUMOTime weightPeriod = myAdaptationInterval > 0 ? myAdaptationInterval : std::numeric_limits<int>::max();
            myRouter = new CHRouterWrapper<MSEdge, SUMOVehicle, SUMOAbstractRouterPermissions<MSEdge, SUMOVehicle> >(
                MSEdge::getAllEdges(), true, myEffortFunc,
                string2time(oc.getString("begin")), string2time(oc.getString("end")), weightPeriod, oc.getInt("device.rerouting.threads"));
        } else {
            throw ProcessError("Unknown routing algorithm '" + routingAlgorithm + "'!");
        }
    }
#ifdef HAVE_FOX
    if (needThread) {
        const int numThreads = OptionsCont::getOptions().getInt("device.rerouting.threads");
        if (myThreadPool.size() < numThreads) {
            new WorkerThread(myThreadPool, myRouter);
        }
        if (myThreadPool.size() < numThreads) {
            myRouter = nullptr;
        }
    }
    if (myThreadPool.size() > 0) {
        myThreadPool.add(new RoutingTask(vehicle, currentTime, onInit));
        return;
    }
#endif
    vehicle.reroute(currentTime, "device.rerouting", *myRouter, onInit, myWithTaz);
}


void
MSRoutingEngine::setEdgeTravelTime(const MSEdge* const edge, const double travelTime) {
    myEdgeSpeeds[edge->getNumericalID()] = edge->getLength() / travelTime;
}


SUMOAbstractRouter<MSEdge, SUMOVehicle>&
MSRoutingEngine::getRouterTT(const MSEdgeVector& prohibited) {
    if (myRouterWithProhibited == nullptr) {
        initWeightUpdate();
        initEdgeWeights();
        myEffortFunc = (gWeightsRandomFactor != 1 || MSNet::getInstance()->hasBidiEdges()
                        ? &MSRoutingEngine::getEffortExtra
                        : &MSRoutingEngine::getEffort);
        myRouterWithProhibited = new AStarRouter<MSEdge, SUMOVehicle, SUMOAbstractRouterPermissions<MSEdge, SUMOVehicle> >(
            MSEdge::getAllEdges(), true, myEffortFunc);
    }
    myRouterWithProhibited->prohibit(prohibited);
    return *myRouterWithProhibited;
}


void
MSRoutingEngine::cleanup() {
    delete myRouterWithProhibited;
    myRouterWithProhibited = nullptr;
    myAdaptationInterval = -1; // responsible for triggering initEdgeWeights
    myPastEdgeSpeeds.clear();
    myEdgeSpeeds.clear();
    // @todo recheck. calling release crashes in parallel routing
    //for (auto& item : myCachedRoutes) {
    //    item.second->release();
    //}
    myCachedRoutes.clear();
    myAdaptationStepsIndex = 0;
#ifdef HAVE_FOX
    if (myThreadPool.size() > 0) {
        // we cannot wait for the static destructor to do the cleanup
        // because the output devices are gone by then
        myThreadPool.clear();
        // router deletion is done in thread destructor
        myRouter = nullptr;
        return;
    }
#endif
    delete myRouter;
    myRouter = nullptr;
}


#ifdef HAVE_FOX
void
MSRoutingEngine::waitForAll() {
    if (myThreadPool.size() > 0) {
        myThreadPool.waitAll();
    }
}


// ---------------------------------------------------------------------------
// MSRoutingEngine::RoutingTask-methods
// ---------------------------------------------------------------------------
void
MSRoutingEngine::RoutingTask::run(FXWorkerThread* context) {
    myVehicle.reroute(myTime, "device.rerouting", static_cast<WorkerThread*>(context)->getRouter(), myOnInit, myWithTaz);
    const MSEdge* source = *myVehicle.getRoute().begin();
    const MSEdge* dest = myVehicle.getRoute().getLastEdge();
    if (source->isTazConnector() && dest->isTazConnector()) {
        const std::pair<const MSEdge*, const MSEdge*> key = std::make_pair(source, dest);
        lock();
        if (MSRoutingEngine::myCachedRoutes.find(key) == MSRoutingEngine::myCachedRoutes.end()) {
            MSRoutingEngine::myCachedRoutes[key] = &myVehicle.getRoute();
            myVehicle.getRoute().addReference();
        }
        unlock();
    }
}
#endif


/****************************************************************************/

