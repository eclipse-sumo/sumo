/****************************************************************************/
/// @file    MSDevice_Routing.cpp
/// @author  Michael Behrisch, Daniel Krajzewicz
/// @date    Tue, 04 Dec 2007
/// @version $Id$
///
// A device that performs vehicle rerouting based on current edge speeds
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
#include <utils/options/OptionsCont.h>
#include <utils/common/WrappingCommand.h>
#include <utils/common/StaticCommand.h>
#include <utils/common/DijkstraRouterTT.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member variables
// ===========================================================================
int MSDevice_Routing::myVehicleIndex = 0;
std::map<const MSEdge*, SUMOReal> MSDevice_Routing::myEdgeEfforts;
Command *MSDevice_Routing::myEdgeWeightSettingCommand = 0;
SUMOReal MSDevice_Routing::myAdaptationWeight;
SUMOTime MSDevice_Routing::myAdaptationInterval;
bool MSDevice_Routing::myWithTaz;
std::map<std::pair<const MSEdge*, const MSEdge*>, const MSRoute*> MSDevice_Routing::myCachedRoutes;


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
void
MSDevice_Routing::insertOptions() throw() {
    OptionsCont &oc = OptionsCont::getOptions();
    oc.addOptionSubTopic("Routing");

    oc.doRegister("device.routing.probability", new Option_Float(0.));
    oc.addDescription("device.routing.probability", "Routing", "The probability for a vehicle to have a routing device");

    oc.doRegister("device.routing.knownveh", new Option_String());
    oc.addDescription("device.routing.knownveh", "Routing", "Assign a device to named vehicles");

    oc.doRegister("device.routing.deterministic", new Option_Bool(false));
    oc.addDescription("device.routing.deterministic", "Routing", "The devices are set deterministic using a fraction of 1000");

    oc.doRegister("device.routing.period", new Option_String("0"));
    oc.addDescription("device.routing.period", "Routing", "The period with which the vehicle shall be rerouted");

    oc.doRegister("device.routing.pre-period", new Option_String("0"));
    oc.addDescription("device.routing.pre-period", "Routing", "The rerouting period before emit");

    oc.doRegister("device.routing.adaptation-weight", new Option_Float(.5));
    oc.addDescription("device.routing.adaptation-weight", "Routing", "The weight of prior edge weights.");

    oc.doRegister("device.routing.adaptation-interval", new Option_String("1"));
    oc.addDescription("device.routing.adaptation-interval", "Routing", "The interval for updating the edge weights.");

    oc.doRegister("device.routing.with-taz", new Option_Bool(false));
    oc.addDescription("device.routing.with-taz", "Routing", "Use zones (districts) as routing end points");

    myVehicleIndex = 0;
    myEdgeWeightSettingCommand = 0;
    myEdgeEfforts.clear();
}


void
MSDevice_Routing::buildVehicleDevices(MSVehicle &v, std::vector<MSDevice*> &into) throw() {
    OptionsCont &oc = OptionsCont::getOptions();
    if (oc.getFloat("device.routing.probability")==0&&!oc.isSet("device.routing.knownveh")) {
        // no route computation is modelled
        return;
    }
    // route computation is enabled
    bool haveByNumber = false;
    if (oc.getBool("device.routing.deterministic")) {
        haveByNumber = ((myVehicleIndex%1000) < (int)(oc.getFloat("device.routing.probability")*1000.));
    } else {
        haveByNumber = RandHelper::rand()<=oc.getFloat("device.routing.probability");
    }
    bool haveByName = oc.isSet("device.routing.knownveh") && OptionsCont::getOptions().isInStringVector("device.routing.knownveh", v.getID());
    if (haveByNumber||haveByName) {
        // build the device
        MSDevice_Routing* device = new MSDevice_Routing(v, "routing_" + v.getID(),
                string2time(oc.getString("device.routing.period")),
                string2time(oc.getString("device.routing.pre-period")));
        into.push_back(device);
        // initialise edge efforts if not done before
        if (myEdgeEfforts.size()==0) {
            const std::vector<MSEdge*> &edges = MSNet::getInstance()->getEdgeControl().getEdges();
            for (std::vector<MSEdge*>::const_iterator i=edges.begin(); i!=edges.end(); ++i) {
                myEdgeEfforts[*i] = (*i)->getCurrentTravelTime();
            }
        }
        // make the weights be updated
        if (myEdgeWeightSettingCommand==0) {
            myEdgeWeightSettingCommand = new StaticCommand< MSDevice_Routing >(&MSDevice_Routing::adaptEdgeEfforts);
            MSNet::getInstance()->getEndOfTimestepEvents().addEvent(
                myEdgeWeightSettingCommand, 0, MSEventControl::ADAPT_AFTER_EXECUTION);
            myAdaptationWeight = oc.getFloat("device.routing.adaptation-weight");
            myAdaptationInterval = string2time(oc.getString("device.routing.adaptation-interval"));
        }
        myWithTaz = oc.getBool("device.routing.with-taz");
        if (myWithTaz) {
            if (MSEdge::dictionary(v.getParameter().fromTaz+"-source") == 0) {
                WRITE_ERROR("Source district '" + v.getParameter().fromTaz + "' not known when rerouting '" + v.getID() + "'!");
                return;
            }
            if (MSEdge::dictionary(v.getParameter().toTaz+"-sink") == 0) {
                WRITE_ERROR("Destination district '" + v.getParameter().toTaz + "' not known when rerouting '" + v.getID() + "'!");
                return;
            }
        }
    }
    myVehicleIndex++;
}


// ---------------------------------------------------------------------------
// MSDevice_Routing-methods
// ---------------------------------------------------------------------------
MSDevice_Routing::MSDevice_Routing(MSVehicle &holder, const std::string &id,
                                   SUMOTime period, SUMOTime preEmitPeriod) throw()
        : MSDevice(holder, id), myPeriod(period), myPreEmitPeriod(period),
        myLastPreEmitReroute(-1), myRerouteCommand(0) {
}


MSDevice_Routing::~MSDevice_Routing() throw() {
    // make the rerouting command invalid if there is one
    if (myRerouteCommand!=0) {
        myRerouteCommand->deschedule();
    }
}


void
MSDevice_Routing::onTryEmit() {
    if (myWithTaz) {
        const SUMOTime now = MSNet::getInstance()->getCurrentTimeStep();
        if (myLastPreEmitReroute == -1 ||
                (myPreEmitPeriod > 0 && myLastPreEmitReroute + myPreEmitPeriod <= now)) {
            const MSEdge* source = MSEdge::dictionary(myHolder.getParameter().fromTaz+"-source");
            const MSEdge* dest = MSEdge::dictionary(myHolder.getParameter().toTaz+"-sink");
            if (source && dest) {
                const std::pair<const MSEdge*, const MSEdge*> key = std::make_pair(source, dest);
                if (myCachedRoutes.find(key) == myCachedRoutes.end()) {
                    DijkstraRouterTT_ByProxi<MSEdge, SUMOVehicle, prohibited_withRestrictions<MSEdge, SUMOVehicle>, MSDevice_Routing>
                    router(MSEdge::dictSize(), true, this, &MSDevice_Routing::getEffort);
                    myHolder.reroute(MSNet::getInstance()->getCurrentTimeStep(), router, true);
                    myCachedRoutes[key] = &myHolder.getRoute();
                    myHolder.getRoute().addReference();
                } else {
                    myHolder.replaceRoute(myCachedRoutes[key], now, true);
                }
                myLastPreEmitReroute = now;
            }
        }
    }
}


void
MSDevice_Routing::enterLaneAtEmit() {
    if (myLastPreEmitReroute == -1) {
        DijkstraRouterTT_ByProxi<MSEdge, SUMOVehicle, prohibited_withRestrictions<MSEdge, SUMOVehicle>, MSDevice_Routing>
        router(MSEdge::dictSize(), true, this, &MSDevice_Routing::getEffort);
        myHolder.reroute(MSNet::getInstance()->getCurrentTimeStep(), router);
    }
    // build repetition trigger if routing shall be done more often
    if (myPeriod>0&&myRerouteCommand==0) {
        myRerouteCommand = new WrappingCommand< MSDevice_Routing >(this, &MSDevice_Routing::wrappedRerouteCommandExecute);
        MSNet::getInstance()->getBeginOfTimestepEvents().addEvent(
            myRerouteCommand, myPeriod+MSNet::getInstance()->getCurrentTimeStep(),
            MSEventControl::ADAPT_AFTER_EXECUTION);
    }
}


SUMOTime
MSDevice_Routing::wrappedRerouteCommandExecute(SUMOTime currentTime) throw(ProcessError) {
    DijkstraRouterTT_ByProxi<MSEdge, SUMOVehicle, prohibited_withRestrictions<MSEdge, SUMOVehicle>, MSDevice_Routing>
    router(MSEdge::dictSize(), true, this, &MSDevice_Routing::getEffort);
    myHolder.reroute(currentTime, router);
    return myPeriod;
}


SUMOReal
MSDevice_Routing::getEffort(const MSEdge * const e, const SUMOVehicle * const v, SUMOReal) const {
    if (myEdgeEfforts.find(e) != myEdgeEfforts.end()) {
        return MAX2(myEdgeEfforts.find(e)->second, e->getLanes()[0]->getLength()/v->getMaxSpeed());
    }
    return 0;
}


SUMOTime
MSDevice_Routing::adaptEdgeEfforts(SUMOTime currentTime) throw(ProcessError) {
    std::map<std::pair<const MSEdge*, const MSEdge*>, const MSRoute*>::iterator it = myCachedRoutes.begin();
    for (;it != myCachedRoutes.end(); ++it) {
        it->second->release();
    }
    myCachedRoutes.clear();
    SUMOReal newWeight = (SUMOReal)(1. - myAdaptationWeight);
    const std::vector<MSEdge*> &edges = MSNet::getInstance()->getEdgeControl().getEdges();
    for (std::vector<MSEdge*>::const_iterator i=edges.begin(); i!=edges.end(); ++i) {
        myEdgeEfforts[*i] = myEdgeEfforts[*i] * myAdaptationWeight + (*i)->getCurrentTravelTime() * newWeight;
    }
    return myAdaptationInterval;
}



/****************************************************************************/

