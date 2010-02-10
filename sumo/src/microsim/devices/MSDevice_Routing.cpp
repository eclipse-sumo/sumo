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
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// static member variables
// ===========================================================================
int MSDevice_Routing::myVehicleIndex = 0;
std::map<const MSEdge*, SUMOReal> MSDevice_Routing::myEdgeEfforts;
Command *MSDevice_Routing::myEdgeWeightSettingCommand = 0;
SUMOReal MSDevice_Routing::myAdaptationWeight;
SUMOTime MSDevice_Routing::myAdaptationInterval;


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

    oc.doRegister("device.routing.probability", new Option_Float(0.));//!!! describe
    oc.addDescription("device.routing.probability", "Routing", "The probability for a vehicle to have a routing device");

    oc.doRegister("device.routing.knownveh", new Option_String());//!!! describe
    oc.addDescription("device.routing.knownveh", "Routing", "Assign a device to named vehicles");

    oc.doRegister("device.routing.deterministic", new Option_Bool(false)); //!!! describe
    oc.addDescription("device.routing.deterministic", "Routing", "The devices are set deterministic using a fraction of 1000");

    oc.doRegister("device.routing.period", new Option_Integer(0));//!!! describe
    oc.addDescription("device.routing.period", "Routing", "The period with which the vehicle shall be rerouted");

    oc.doRegister("device.routing.adaptation-weight", new Option_Float(.5));//!!! describe
    oc.addDescription("device.routing.adaptation-weight", "Routing", "The weight of prior edge weights.");

    oc.doRegister("device.routing.adaptation-interval", new Option_Integer(1));//!!! describe
    oc.addDescription("device.routing.adaptation-interval", "Routing", "The interval for updating the edge weights.");

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
        MSDevice_Routing* device = new MSDevice_Routing(v, "routing_" + v.getID(), (SUMOTime) oc.getInt("device.routing.period")); // !!! SUMOTime-option
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
            myAdaptationInterval = oc.GET_XML_SUMO_TIME("device.routing.adaptation-interval");
        }
        // the following is just to give the vehicle a valid route before the route is checked at init
        DijkstraRouterTT_ByProxi<MSEdge, SUMOVehicle, prohibited_withRestrictions<MSEdge, SUMOVehicle>, MSDevice_Routing>
        router(MSEdge::dictSize(), true, device, &MSDevice_Routing::getEffort);
        v.reroute(MSNet::getInstance()->getCurrentTimeStep(), router);
    }
    myVehicleIndex++;
}


// ---------------------------------------------------------------------------
// MSDevice_Routing-methods
// ---------------------------------------------------------------------------
MSDevice_Routing::MSDevice_Routing(MSVehicle &holder, const std::string &id,
                                   SUMOTime period) throw()
        : MSDevice(holder, id), myPeriod(period), myRerouteCommand(0) {
}


MSDevice_Routing::~MSDevice_Routing() throw() {
    // make the rerouting command invalid if there is one
    if (myRerouteCommand!=0) {
        myRerouteCommand->deschedule();
    }
}


void
MSDevice_Routing::enterLaneAtEmit(MSLane* enteredLane, const MSVehicle::State &) {
    DijkstraRouterTT_ByProxi<MSEdge, SUMOVehicle, prohibited_withRestrictions<MSEdge, SUMOVehicle>, MSDevice_Routing>
    router(MSEdge::dictSize(), true, this, &MSDevice_Routing::getEffort);
    myHolder.reroute(MSNet::getInstance()->getCurrentTimeStep(), router);
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
MSDevice_Routing::getEffort(const MSEdge * const e, const SUMOVehicle * const v, SUMOTime) const {
    assert(myEdgeEfforts.find(e)!=myEdgeEfforts.end());
    return MAX2(myEdgeEfforts.find(e)->second, e->getLanes()[0]->getLength()/v->getMaxSpeed());
}


SUMOTime
MSDevice_Routing::adaptEdgeEfforts(SUMOTime currentTime) throw(ProcessError) {
    SUMOReal newWeight = (SUMOReal)(1. - myAdaptationWeight);
    const std::vector<MSEdge*> &edges = MSNet::getInstance()->getEdgeControl().getEdges();
    for (std::vector<MSEdge*>::const_iterator i=edges.begin(); i!=edges.end(); ++i) {
        myEdgeEfforts[*i] = myEdgeEfforts[*i] * myAdaptationWeight + (*i)->getCurrentTravelTime() * newWeight;
    }
    return myAdaptationInterval;
}



/****************************************************************************/

