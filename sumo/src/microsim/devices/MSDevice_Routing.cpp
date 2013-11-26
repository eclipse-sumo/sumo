/****************************************************************************/
/// @file    MSDevice_Routing.cpp
/// @author  Michael Behrisch
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Christoph Sommer
/// @date    Tue, 04 Dec 2007
/// @version $Id$
///
// A device that performs vehicle rerouting based on current edge speeds
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
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
#include <utils/options/OptionsCont.h>
#include <utils/common/WrappingCommand.h>
#include <utils/common/StaticCommand.h>
#include <utils/common/DijkstraRouterTT.h>
#include <utils/common/AStarRouter.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member variables
// ===========================================================================
std::map<const MSEdge*, SUMOReal> MSDevice_Routing::myEdgeEfforts;
Command* MSDevice_Routing::myEdgeWeightSettingCommand = 0;
SUMOReal MSDevice_Routing::myAdaptationWeight;
SUMOTime MSDevice_Routing::myAdaptationInterval;
bool MSDevice_Routing::myWithTaz;
std::map<std::pair<const MSEdge*, const MSEdge*>, const MSRoute*> MSDevice_Routing::myCachedRoutes;
SUMOAbstractRouter<MSEdge, SUMOVehicle>* MSDevice_Routing::myRouter = 0;


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
void
MSDevice_Routing::insertOptions(OptionsCont& oc) {
    oc.addOptionSubTopic("Routing");
    insertDefaultAssignmentOptions("rerouting", "Routing", oc);

    oc.doRegister("device.rerouting.period", new Option_String("0", "TIME"));
    oc.addSynonyme("device.rerouting.period", "device.routing.period", true);
    oc.addDescription("device.rerouting.period", "Routing", "The period with which the vehicle shall be rerouted");

    oc.doRegister("device.rerouting.pre-period", new Option_String("0", "TIME"));
    oc.addSynonyme("device.rerouting.pre-period", "device.routing.pre-period", true);
    oc.addDescription("device.rerouting.pre-period", "Routing", "The rerouting period before depart");

    oc.doRegister("device.rerouting.adaptation-weight", new Option_Float(.5));
    oc.addSynonyme("device.rerouting.adaptation-weight", "device.routing.adaptation-weight", true);
    oc.addDescription("device.rerouting.adaptation-weight", "Routing", "The weight of prior edge weights");

    oc.doRegister("device.rerouting.adaptation-interval", new Option_String("1", "TIME"));
    oc.addSynonyme("device.rerouting.adaptation-interval", "device.routing.adaptation-interval", true);
    oc.addDescription("device.rerouting.adaptation-interval", "Routing", "The interval for updating the edge weights");

    oc.doRegister("device.rerouting.with-taz", new Option_Bool(false));
    oc.addSynonyme("device.rerouting.with-taz", "device.routing.with-taz", true);
    oc.addDescription("device.rerouting.with-taz", "Routing", "Use zones (districts) as routing end points");

    oc.doRegister("device.rerouting.init-with-loaded-weights", new Option_Bool(false));
    oc.addDescription("device.rerouting.init-with-loaded-weights", "Routing", "Use given weight files for initializing edge weights");

    myEdgeWeightSettingCommand = 0;
    myEdgeEfforts.clear();
}


void
MSDevice_Routing::buildVehicleDevices(SUMOVehicle& v, std::vector<MSDevice*>& into) {
    bool needRerouting = v.getParameter().wasSet(VEHPARS_FORCE_REROUTE);
    OptionsCont& oc = OptionsCont::getOptions();
    if (!needRerouting && oc.getFloat("device.rerouting.probability") == 0 && !oc.isSet("device.rerouting.explicit")) {
        // no route computation is modelled
        return;
    }
    needRerouting |= equippedByDefaultAssignmentOptions(OptionsCont::getOptions(), "rerouting", v);
    if (needRerouting) {
        // route computation is enabled
        myWithTaz = oc.getBool("device.rerouting.with-taz");
        // build the device
        MSDevice_Routing* device = new MSDevice_Routing(v, "routing_" + v.getID(),
                string2time(oc.getString("device.rerouting.period")),
                string2time(oc.getString("device.rerouting.pre-period")));
        into.push_back(device);
        // initialise edge efforts if not done before
        if (myEdgeEfforts.size() == 0) {
            const std::vector<MSEdge*>& edges = MSNet::getInstance()->getEdgeControl().getEdges();
            const bool useLoaded = oc.getBool("device.rerouting.init-with-loaded-weights");
            const SUMOReal currentSecond = STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep());
            for (std::vector<MSEdge*>::const_iterator i = edges.begin(); i != edges.end(); ++i) {
                if (useLoaded) {
                    myEdgeEfforts[*i] = MSNet::getTravelTime(*i, 0, currentSecond);
                } else {
                    myEdgeEfforts[*i] = (*i)->getCurrentTravelTime();
                }
            }
        }
        // make the weights be updated
        if (myEdgeWeightSettingCommand == 0) {
            myEdgeWeightSettingCommand = new StaticCommand< MSDevice_Routing >(&MSDevice_Routing::adaptEdgeEfforts);
            MSNet::getInstance()->getEndOfTimestepEvents().addEvent(
                myEdgeWeightSettingCommand, 0, MSEventControl::ADAPT_AFTER_EXECUTION);
            myAdaptationWeight = oc.getFloat("device.rerouting.adaptation-weight");
            myAdaptationInterval = string2time(oc.getString("device.rerouting.adaptation-interval"));
        }
        if (myWithTaz) {
            if (MSEdge::dictionary(v.getParameter().fromTaz + "-source") == 0) {
                WRITE_ERROR("Source district '" + v.getParameter().fromTaz + "' not known when rerouting '" + v.getID() + "'!");
                return;
            }
            if (MSEdge::dictionary(v.getParameter().toTaz + "-sink") == 0) {
                WRITE_ERROR("Destination district '" + v.getParameter().toTaz + "' not known when rerouting '" + v.getID() + "'!");
                return;
            }
        }
    }
}


// ---------------------------------------------------------------------------
// MSDevice_Routing-methods
// ---------------------------------------------------------------------------
MSDevice_Routing::MSDevice_Routing(SUMOVehicle& holder, const std::string& id,
                                   SUMOTime period, SUMOTime preInsertionPeriod)
    : MSDevice(holder, id), myPeriod(period), myPreInsertionPeriod(preInsertionPeriod), myRerouteCommand(0) {
    if (myWithTaz) {
        myRerouteCommand = new WrappingCommand< MSDevice_Routing >(this, &MSDevice_Routing::preInsertionReroute);
        MSNet::getInstance()->getInsertionEvents().addEvent(
            myRerouteCommand, holder.getParameter().depart,
            MSEventControl::ADAPT_AFTER_EXECUTION);
    }
}


MSDevice_Routing::~MSDevice_Routing() {
    // make the rerouting command invalid if there is one
    if (myRerouteCommand != 0) {
        myRerouteCommand->deschedule();
    }
}


bool
MSDevice_Routing::notifyEnter(SUMOVehicle& /*veh*/, MSMoveReminder::Notification reason) {
    if (reason == MSMoveReminder::NOTIFICATION_DEPARTED) {
        if (myRerouteCommand != 0) { // clean up pre depart rerouting
            if (myPreInsertionPeriod > 0) {
                myRerouteCommand->deschedule();
            }
            myRerouteCommand = 0;
        }
        if (!myWithTaz) {
            wrappedRerouteCommandExecute(MSNet::getInstance()->getCurrentTimeStep());
        }
        // build repetition trigger if routing shall be done more often
        if (myPeriod > 0) {
            myRerouteCommand = new WrappingCommand< MSDevice_Routing >(this, &MSDevice_Routing::wrappedRerouteCommandExecute);
            MSNet::getInstance()->getBeginOfTimestepEvents().addEvent(
                myRerouteCommand, myPeriod + MSNet::getInstance()->getCurrentTimeStep(),
                MSEventControl::ADAPT_AFTER_EXECUTION);
        }
    }
    return false;
}


SUMOTime
MSDevice_Routing::preInsertionReroute(SUMOTime currentTime) {
    const MSEdge* source = MSEdge::dictionary(myHolder.getParameter().fromTaz + "-source");
    const MSEdge* dest = MSEdge::dictionary(myHolder.getParameter().toTaz + "-sink");
    if (source && dest) {
        const std::pair<const MSEdge*, const MSEdge*> key = std::make_pair(source, dest);
        if (myCachedRoutes.find(key) == myCachedRoutes.end()) {
            myHolder.reroute(currentTime, getRouter(), true);
            myCachedRoutes[key] = &myHolder.getRoute();
            myHolder.getRoute().addReference();
        } else {
            myHolder.replaceRoute(myCachedRoutes[key], true);
        }
    }
    return myPreInsertionPeriod;
}


SUMOTime
MSDevice_Routing::wrappedRerouteCommandExecute(SUMOTime currentTime) {
    myHolder.reroute(currentTime, getRouter());
    return myPeriod;
}


SUMOReal
MSDevice_Routing::getEffort(const MSEdge* const e, const SUMOVehicle* const v, SUMOReal) {
    if (myEdgeEfforts.find(e) != myEdgeEfforts.end()) {
        return MAX2(myEdgeEfforts.find(e)->second, e->getMinimumTravelTime(v));
    }
    return 0;
}


SUMOTime
MSDevice_Routing::adaptEdgeEfforts(SUMOTime /*currentTime*/) {
    std::map<std::pair<const MSEdge*, const MSEdge*>, const MSRoute*>::iterator it = myCachedRoutes.begin();
    for (; it != myCachedRoutes.end(); ++it) {
        it->second->release();
    }
    myCachedRoutes.clear();
    SUMOReal newWeight = (SUMOReal)(1. - myAdaptationWeight);
    const std::vector<MSEdge*>& edges = MSNet::getInstance()->getEdgeControl().getEdges();
    for (std::vector<MSEdge*>::const_iterator i = edges.begin(); i != edges.end(); ++i) {
        myEdgeEfforts[*i] = myEdgeEfforts[*i] * myAdaptationWeight + (*i)->getCurrentTravelTime() * newWeight;
    }
    return myAdaptationInterval;
}


SUMOAbstractRouter<MSEdge, SUMOVehicle>&
MSDevice_Routing::getRouter() {
    if (myRouter == 0) {
        const std::string routingAlgorithm = OptionsCont::getOptions().getString("routing-algorithm");
        if (routingAlgorithm == "dijkstra") {
            myRouter = new DijkstraRouterTT_ByProxi<MSEdge, SUMOVehicle, prohibited_withRestrictions<MSEdge, SUMOVehicle> >(
                MSEdge::numericalDictSize(), true, &MSDevice_Routing::getEffort);
        } else if (routingAlgorithm == "astar") {
            myRouter = new AStarRouterTT_ByProxi<MSEdge, SUMOVehicle, prohibited_withRestrictions<MSEdge, SUMOVehicle> >(
                MSEdge::numericalDictSize(), true, &MSDevice_Routing::getEffort);
        } else {
            throw ProcessError("Unknown routing Algorithm '" + routingAlgorithm + "'!");
        }
    }
    return *myRouter;
}


void
MSDevice_Routing::cleanup() {
    delete myRouter;
    myRouter = 0;
}


/****************************************************************************/

