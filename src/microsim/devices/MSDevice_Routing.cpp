/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2007-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSDevice_Routing.cpp
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
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSDevice_Routing.h"
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
#include <utils/vehicle/DijkstraRouter.h>
#include <utils/vehicle/AStarRouter.h>
#include <utils/vehicle/CHRouter.h>
#include <utils/vehicle/CHRouterWrapper.h>


// ===========================================================================
// static member variables
// ===========================================================================
std::vector<double> MSDevice_Routing::myEdgeSpeeds;
std::vector<std::vector<double> > MSDevice_Routing::myPastEdgeSpeeds;
Command* MSDevice_Routing::myEdgeWeightSettingCommand = 0;
double MSDevice_Routing::myAdaptationWeight;
int MSDevice_Routing::myAdaptationSteps;
int MSDevice_Routing::myAdaptationStepsIndex = 0;
SUMOTime MSDevice_Routing::myAdaptationInterval = -1;
SUMOTime MSDevice_Routing::myLastAdaptation = -1;
bool MSDevice_Routing::myWithTaz;
std::map<std::pair<const MSEdge*, const MSEdge*>, const MSRoute*> MSDevice_Routing::myCachedRoutes;
SUMOAbstractRouter<MSEdge, SUMOVehicle>* MSDevice_Routing::myRouter = 0;
AStarRouter<MSEdge, SUMOVehicle, prohibited_withPermissions<MSEdge, SUMOVehicle> >* MSDevice_Routing::myRouterWithProhibited = 0;
double MSDevice_Routing::myRandomizeWeightsFactor = 0;
#ifdef HAVE_FOX
FXWorkerThread::Pool MSDevice_Routing::myThreadPool;
#endif


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
void
MSDevice_Routing::insertOptions(OptionsCont& oc) {
    insertDefaultAssignmentOptions("rerouting", "Routing", oc);

    oc.doRegister("device.rerouting.period", new Option_String("0", "TIME"));
    oc.addSynonyme("device.rerouting.period", "device.routing.period", true);
    oc.addDescription("device.rerouting.period", "Routing", "The period with which the vehicle shall be rerouted");

    oc.doRegister("device.rerouting.pre-period", new Option_String("1", "TIME"));
    oc.addSynonyme("device.rerouting.pre-period", "device.routing.pre-period", true);
    oc.addDescription("device.rerouting.pre-period", "Routing", "The rerouting period before depart");

    oc.doRegister("device.rerouting.adaptation-weight", new Option_Float(.5));
    oc.addSynonyme("device.rerouting.adaptation-weight", "device.routing.adaptation-weight", true);
    oc.addDescription("device.rerouting.adaptation-weight", "Routing", "The weight of prior edge weights for exponential moving average");

    oc.doRegister("device.rerouting.adaptation-steps", new Option_Integer(0));
    oc.addSynonyme("device.rerouting.adaptation-steps", "device.routing.adaptation-steps", true);
    oc.addDescription("device.rerouting.adaptation-steps", "Routing", "The number of steps for moving average weight of prior edge weights");

    oc.doRegister("device.rerouting.adaptation-interval", new Option_String("1", "TIME"));
    oc.addSynonyme("device.rerouting.adaptation-interval", "device.routing.adaptation-interval", true);
    oc.addDescription("device.rerouting.adaptation-interval", "Routing", "The interval for updating the edge weights");

    oc.doRegister("device.rerouting.with-taz", new Option_Bool(false));
    oc.addSynonyme("device.rerouting.with-taz", "device.routing.with-taz", true);
    oc.addSynonyme("device.rerouting.with-taz", "with-taz");
    oc.addDescription("device.rerouting.with-taz", "Routing", "Use zones (districts) as routing start- and endpoints");

    oc.doRegister("device.rerouting.init-with-loaded-weights", new Option_Bool(false));
    oc.addDescription("device.rerouting.init-with-loaded-weights", "Routing", "Use weight files given with option --weight-files for initializing edge weights");

    oc.doRegister("device.rerouting.threads", new Option_Integer(0));
    oc.addDescription("device.rerouting.threads", "Routing", "The number of parallel execution threads used for rerouting");

    oc.doRegister("device.rerouting.output", new Option_FileName());
    oc.addDescription("device.rerouting.output", "Routing", "Save adapting weights to FILE");

    myEdgeWeightSettingCommand = 0;
    myEdgeSpeeds.clear();
    myAdaptationInterval = -1;
    myAdaptationSteps = -1;
    myLastAdaptation = -1;
}


bool
MSDevice_Routing::checkOptions(OptionsCont& oc) {
    bool ok = true;
    if (oc.getInt("device.rerouting.adaptation-steps") > 0 && !oc.isDefault("device.rerouting.adaptation-weight")) {
        WRITE_ERROR("Only one of the options 'device.rerouting.adaptation-steps' or 'device.rerouting.adaptation-weight' may be given.");
        ok = false;
    }
    if (oc.getFloat("weights.random-factor") < 1) {
        WRITE_ERROR("weights.random-factor cannot be less than 1");
        ok = false;
    }
    if (string2time(oc.getString("device.rerouting.adaptation-interval")) < 0) {
        WRITE_ERROR("Negative value for device.rerouting.adaptation-interval!");
        ok = false;
    }
    if (oc.getFloat("device.rerouting.adaptation-weight") < 0.  ||
            oc.getFloat("device.rerouting.adaptation-weight") > 1.) {
        WRITE_ERROR("The value for device.rerouting.adaptation-weight must be between 0 and 1!");
        ok = false;
    }
#ifndef HAVE_FOX
    if (oc.getInt("device.rerouting.threads") > 1) {
        WRITE_ERROR("Parallel routing is only possible when compiled with Fox.");
        ok = false;
    }
#endif
    return ok;
}


void
MSDevice_Routing::buildVehicleDevices(SUMOVehicle& v, std::vector<MSDevice*>& into) {
    const OptionsCont& oc = OptionsCont::getOptions();
    if (v.getParameter().wasSet(VEHPARS_FORCE_REROUTE) || equippedByDefaultAssignmentOptions(oc, "rerouting", v)) {
        // route computation is enabled
        myWithTaz = oc.getBool("device.rerouting.with-taz");
        const SUMOTime period = string2time(oc.getString("device.rerouting.period"));
        const SUMOTime prePeriod = string2time(oc.getString("device.rerouting.pre-period"));
        // make the weights be updated
        if (myAdaptationInterval == -1) {
            myAdaptationInterval = string2time(oc.getString("device.rerouting.adaptation-interval"));
            myAdaptationWeight = oc.getFloat("device.rerouting.adaptation-weight");
            if (myAdaptationWeight < 1. && myAdaptationInterval > 0) {
                myEdgeWeightSettingCommand = new StaticCommand<MSDevice_Routing>(&MSDevice_Routing::adaptEdgeEfforts);
                MSNet::getInstance()->getEndOfTimestepEvents()->addEvent(myEdgeWeightSettingCommand);
            } else if (period > 0) {
                WRITE_WARNING("Rerouting is useless if the edge weights do not get updated!");
            }
            OutputDevice::createDeviceByOption("device.rerouting.output", "weights", "meandata_file.xsd");
        }
        // build the device
        into.push_back(new MSDevice_Routing(v, "routing_" + v.getID(), period, prePeriod));
    }
}


// ---------------------------------------------------------------------------
// MSDevice_Routing-methods
// ---------------------------------------------------------------------------
MSDevice_Routing::MSDevice_Routing(SUMOVehicle& holder, const std::string& id,
                                   SUMOTime period, SUMOTime preInsertionPeriod)
    : MSDevice(holder, id), myPeriod(period), myPreInsertionPeriod(preInsertionPeriod), myLastRouting(-1), mySkipRouting(-1), myRerouteCommand(0) {
    if (myPreInsertionPeriod > 0 || holder.getParameter().wasSet(VEHPARS_FORCE_REROUTE)) {
        // we do always a pre insertion reroute for trips to fill the best lanes of the vehicle with somehow meaningful values (especially for deaprtLane="best")
        myRerouteCommand = new WrappingCommand<MSDevice_Routing>(this, &MSDevice_Routing::preInsertionReroute);
        // if we don't update the edge weights, we might as well reroute now and hopefully use our threads better
        const SUMOTime execTime = myEdgeWeightSettingCommand == 0 ? -1 : holder.getParameter().depart;
        MSNet::getInstance()->getInsertionEvents()->addEvent(myRerouteCommand, execTime);
    }
}


MSDevice_Routing::~MSDevice_Routing() {
    // make the rerouting command invalid if there is one
    if (myRerouteCommand != 0 && MSNet::getInstance()->getInsertionEvents() != 0) {
        myRerouteCommand->deschedule();
    }
}


bool
MSDevice_Routing::notifyEnter(SUMOVehicle& /*veh*/, MSMoveReminder::Notification reason, const MSLane* /* enteredLane */) {
    if (reason == MSMoveReminder::NOTIFICATION_DEPARTED) {
        // clean up pre depart rerouting
        if (myPreInsertionPeriod > 0) {
            myRerouteCommand->deschedule();
        }
        myRerouteCommand = 0;
        // build repetition trigger if routing shall be done more often
        if (myPeriod > 0) {
            myRerouteCommand = new WrappingCommand<MSDevice_Routing>(this, &MSDevice_Routing::wrappedRerouteCommandExecute);
            MSNet::getInstance()->getBeginOfTimestepEvents()->addEvent(
                myRerouteCommand, myPeriod + MSNet::getInstance()->getCurrentTimeStep());
        }
    }
    return false;
}


void
MSDevice_Routing::initEdgeWeights() {
    if (myEdgeSpeeds.empty()) {
        const OptionsCont& oc = OptionsCont::getOptions();
        myAdaptationSteps = oc.getInt("device.rerouting.adaptation-steps");
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
                myEdgeSpeeds[edge->getNumericalID()] = edge->getLength() / MSNet::getTravelTime(edge, 0, currentSecond);
            } else {
                myEdgeSpeeds[edge->getNumericalID()] = edge->getMeanSpeed();
            }
            if (myAdaptationSteps > 0) {
                myPastEdgeSpeeds[edge->getNumericalID()] = std::vector<double>(myAdaptationSteps, myEdgeSpeeds[edge->getNumericalID()]);
            }
        }
        myLastAdaptation = MSNet::getInstance()->getCurrentTimeStep();
        myRandomizeWeightsFactor = oc.getFloat("weights.random-factor");
    }
}


SUMOTime
MSDevice_Routing::preInsertionReroute(const SUMOTime currentTime) {
    if (mySkipRouting == currentTime) {
        return DELTA_T;
    }
    const MSEdge* source = *myHolder.getRoute().begin();
    const MSEdge* dest = myHolder.getRoute().getLastEdge();
    if (source->isTazConnector() && dest->isTazConnector()) {
        const std::pair<const MSEdge*, const MSEdge*> key = std::make_pair(source, dest);
        if (myCachedRoutes.find(key) != myCachedRoutes.end()) {
            if (myCachedRoutes[key]->size() > 2) {
                myHolder.replaceRoute(myCachedRoutes[key], true);
                return myPreInsertionPeriod;
            }
        }
    }
    reroute(currentTime, true);
    return myPreInsertionPeriod;
}


SUMOTime
MSDevice_Routing::wrappedRerouteCommandExecute(SUMOTime currentTime) {
    reroute(currentTime);
    return myPeriod;
}


double
MSDevice_Routing::getEffort(const MSEdge* const e, const SUMOVehicle* const v, double) {
    const int id = e->getNumericalID();
    if (id < (int)myEdgeSpeeds.size()) {
        double effort = MAX2(e->getLength() / MAX2(myEdgeSpeeds[id], NUMERICAL_EPS), e->getMinimumTravelTime(v));
        if (myRandomizeWeightsFactor != 1) {
            effort *= RandHelper::rand((double)1, myRandomizeWeightsFactor);
        }
        return effort;
    }
    return 0;
}


double
MSDevice_Routing::getAssumedSpeed(const MSEdge* edge) {
    return edge->getLength() / getEffort(edge, 0, 0);
}


SUMOTime
MSDevice_Routing::adaptEdgeEfforts(SUMOTime currentTime) {
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
            const int id = (*i)->getNumericalID();
            const double currSpeed = (*i)->getMeanSpeed();
            myEdgeSpeeds[id] += (currSpeed - myPastEdgeSpeeds[id][myAdaptationStepsIndex]) / myAdaptationSteps;
            myPastEdgeSpeeds[id][myAdaptationStepsIndex] = currSpeed;
        }
        myAdaptationStepsIndex = (myAdaptationStepsIndex + 1) % myAdaptationSteps;
    } else {
        // exponential moving average
        const double newWeightFactor = (double)(1. - myAdaptationWeight);
        for (MSEdgeVector::const_iterator i = edges.begin(); i != edges.end(); ++i) {
            const int id = (*i)->getNumericalID();
            const double currSpeed = (*i)->getMeanSpeed();
            if (currSpeed != myEdgeSpeeds[id]) {
                myEdgeSpeeds[id] = myEdgeSpeeds[id] * myAdaptationWeight + currSpeed * newWeightFactor;
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
        for (MSEdgeVector::const_iterator i = edges.begin(); i != edges.end(); ++i) {
            const int id = (*i)->getNumericalID();
            dev.openTag(SUMO_TAG_EDGE);
            dev.writeAttr(SUMO_ATTR_ID, (*i)->getID());
            dev.writeAttr("traveltime", (*i)->getLength() / myEdgeSpeeds[id]);
            dev.closeTag();
        }
        dev.closeTag();
    }
    return myAdaptationInterval;
}


void
MSDevice_Routing::reroute(const SUMOTime currentTime, const bool onInit) {
    initEdgeWeights();
    //check whether the weights did change since the last reroute
    if (myLastRouting >= myLastAdaptation) {
        return;
    }
    myLastRouting = currentTime;
#ifdef HAVE_FOX
    const bool needThread = (myRouter == 0 && myThreadPool.isFull());
#else
    const bool needThread = true;
#endif
    if (needThread && myRouter == 0) {
        OptionsCont& oc = OptionsCont::getOptions();
        const std::string routingAlgorithm = oc.getString("routing-algorithm");
        const bool mayHaveRestrictions = MSNet::getInstance()->hasPermissions() || oc.getInt("remote-port") != 0;
        if (routingAlgorithm == "dijkstra") {
            if (mayHaveRestrictions) {
                myRouter = new DijkstraRouter<MSEdge, SUMOVehicle, prohibited_withPermissions<MSEdge, SUMOVehicle> >(
                    MSEdge::getAllEdges(), true, &MSDevice_Routing::getEffort);
            } else {
                myRouter = new DijkstraRouter<MSEdge, SUMOVehicle, noProhibitions<MSEdge, SUMOVehicle> >(
                    MSEdge::getAllEdges(), true, &MSDevice_Routing::getEffort);
            }
        } else if (routingAlgorithm == "astar") {
            if (mayHaveRestrictions) {
                typedef AStarRouter<MSEdge, SUMOVehicle, prohibited_withPermissions<MSEdge, SUMOVehicle> > AStar;
                const AStar::LookupTable* lookup = 0;
                if (oc.isSet("astar.all-distances")) {
                    lookup = new AStar::FLT(oc.getString("astar.all-distances"), (int)MSEdge::getAllEdges().size());
                } else if (oc.isSet("astar.landmark-distances")) {
                    const double speedFactor = myHolder.getChosenSpeedFactor();
                    // we need an exemplary vehicle with speedFactor 1
                    myHolder.setChosenSpeedFactor(1);
                    CHRouterWrapper<MSEdge, SUMOVehicle, prohibited_withPermissions<MSEdge, SUMOVehicle> > router(
                        MSEdge::getAllEdges(), true, &MSNet::getTravelTime,
                        string2time(oc.getString("begin")), string2time(oc.getString("end")), std::numeric_limits<int>::max(), 1);
                    lookup = new AStar::LMLT(oc.getString("astar.landmark-distances"), MSEdge::getAllEdges(), &router, &myHolder, "", oc.getInt("device.rerouting.threads"));
                    myHolder.setChosenSpeedFactor(speedFactor);
                }
                myRouter = new AStar(MSEdge::getAllEdges(), true, &MSDevice_Routing::getEffort, lookup);
            } else {
                typedef AStarRouter<MSEdge, SUMOVehicle, noProhibitions<MSEdge, SUMOVehicle> > AStar;
                const AStar::LookupTable* lookup = 0;
                if (oc.isSet("astar.all-distances")) {
                    lookup = new AStar::FLT(oc.getString("astar.all-distances"), (int)MSEdge::getAllEdges().size());
                } else if (oc.isSet("astar.landmark-distances")) {
                    const double speedFactor = myHolder.getChosenSpeedFactor();
                    // we need an exemplary vehicle with speedFactor 1
                    myHolder.setChosenSpeedFactor(1);
                    CHRouterWrapper<MSEdge, SUMOVehicle, noProhibitions<MSEdge, SUMOVehicle> > router(
                        MSEdge::getAllEdges(), true, &MSNet::getTravelTime,
                        string2time(oc.getString("begin")), string2time(oc.getString("end")), std::numeric_limits<int>::max(), 1);
                    lookup = new AStar::LMLT(oc.getString("astar.landmark-distances"), MSEdge::getAllEdges(), &router, &myHolder, "", oc.getInt("device.rerouting.threads"));
                    myHolder.setChosenSpeedFactor(speedFactor);
                }
                myRouter = new AStar(MSEdge::getAllEdges(), true, &MSDevice_Routing::getEffort, lookup);
            }
        } else if (routingAlgorithm == "CH") {
            const SUMOTime weightPeriod = myAdaptationInterval > 0 ? myAdaptationInterval : std::numeric_limits<int>::max();
            if (mayHaveRestrictions) {
                myRouter = new CHRouter<MSEdge, SUMOVehicle, prohibited_withPermissions<MSEdge, SUMOVehicle> >(
                    MSEdge::getAllEdges(), true, &MSDevice_Routing::getEffort, myHolder.getVClass(), weightPeriod, true);
            } else {
                myRouter = new CHRouter<MSEdge, SUMOVehicle, noProhibitions<MSEdge, SUMOVehicle> >(
                    MSEdge::getAllEdges(), true, &MSDevice_Routing::getEffort, myHolder.getVClass(), weightPeriod, false);
            }
        } else if (routingAlgorithm == "CHWrapper") {
            const SUMOTime weightPeriod = myAdaptationInterval > 0 ? myAdaptationInterval : std::numeric_limits<int>::max();
            myRouter = new CHRouterWrapper<MSEdge, SUMOVehicle, prohibited_withPermissions<MSEdge, SUMOVehicle> >(
                MSEdge::getAllEdges(), true, &MSDevice_Routing::getEffort,
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
            myRouter = 0;
        }
    }
    if (myThreadPool.size() > 0) {
        myThreadPool.add(new RoutingTask(myHolder, currentTime, onInit));
        return;
    }
#endif
    myHolder.reroute(currentTime, *myRouter, onInit, myWithTaz);
}


SUMOAbstractRouter<MSEdge, SUMOVehicle>&
MSDevice_Routing::getRouterTT(const MSEdgeVector& prohibited) {
    if (myRouterWithProhibited == 0) {
        myRouterWithProhibited = new AStarRouter<MSEdge, SUMOVehicle, prohibited_withPermissions<MSEdge, SUMOVehicle> >(
            MSEdge::getAllEdges(), true, &MSDevice_Routing::getEffort);
    }
    myRouterWithProhibited->prohibit(prohibited);
    return *myRouterWithProhibited;
}


std::string
MSDevice_Routing::getParameter(const std::string& key) const {
    if (StringUtils::startsWith(key, "edge:")) {
        const std::string edgeID = key.substr(5);
        const MSEdge* edge = MSEdge::dictionary(edgeID);
        if (edge == 0) {
            throw InvalidArgument("Edge '" + edgeID + "' is invalid for parameter retrieval of '" + deviceName() + "'");
        }
        return toString(getEffort(edge, &myHolder, 0));
    } else if (key == "period") {
        return time2string(myPeriod);
    }
    throw InvalidArgument("Parameter '" + key + "' is not supported for device of type '" + deviceName() + "'");
}


void
MSDevice_Routing::setParameter(const std::string& key, const std::string& value) {
    double doubleValue;
    try {
        doubleValue = TplConvert::_2double(value.c_str());
    } catch (NumberFormatException) {
        throw InvalidArgument("Setting parameter '" + key + "' requires a number for device of type '" + deviceName() + "'");
    }
    if (StringUtils::startsWith(key, "edge:")) {
        const std::string edgeID = key.substr(5);
        const MSEdge* edge = MSEdge::dictionary(edgeID);
        if (edge == 0) {
            throw InvalidArgument("Edge '" + edgeID + "' is invalid for parameter setting of '" + deviceName() + "'");
        }
        myEdgeSpeeds[edge->getNumericalID()] = edge->getLength() / doubleValue;
    } else if (key == "period") {
        const SUMOTime oldPeriod = myPeriod;
        myPeriod = TIME2STEPS(doubleValue);
        if (myPeriod <= 0) {
            myRerouteCommand->deschedule();
        } else if (oldPeriod <= 0) {
            // re-schedule routing command
            notifyEnter(myHolder, MSMoveReminder::NOTIFICATION_DEPARTED, 0);
        }
    } else {
        throw InvalidArgument("Setting parameter '" + key + "' is not supported for device of type '" + deviceName() + "'");
    }
}



void
MSDevice_Routing::cleanup() {
    delete myRouterWithProhibited;
    myRouterWithProhibited = 0;
#ifdef HAVE_FOX
    if (myThreadPool.size() > 0) {
        // we cannot wait for the static destructor to do the cleanup
        // because the output devices are gone by then
        myThreadPool.clear();
        // router deletion is done in thread destructor
        myRouter = 0;
        return;
    }
#endif
    delete myRouter;
    myRouter = 0;
}


#ifdef HAVE_FOX
void
MSDevice_Routing::waitForAll() {
    if (myThreadPool.size() > 0) {
        myThreadPool.waitAll();
    }
}


// ---------------------------------------------------------------------------
// MSDevice_Routing::RoutingTask-methods
// ---------------------------------------------------------------------------
void
MSDevice_Routing::RoutingTask::run(FXWorkerThread* context) {
    myVehicle.reroute(myTime, static_cast<WorkerThread*>(context)->getRouter(), myOnInit, myWithTaz);
    const MSEdge* source = *myVehicle.getRoute().begin();
    const MSEdge* dest = myVehicle.getRoute().getLastEdge();
    if (source->isTazConnector() && dest->isTazConnector()) {
        const std::pair<const MSEdge*, const MSEdge*> key = std::make_pair(source, dest);
        lock();
        if (MSDevice_Routing::myCachedRoutes.find(key) == MSDevice_Routing::myCachedRoutes.end()) {
            MSDevice_Routing::myCachedRoutes[key] = &myVehicle.getRoute();
            myVehicle.getRoute().addReference();
        }
        unlock();
    }
}
#endif


/****************************************************************************/

