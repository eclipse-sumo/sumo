/****************************************************************************/
/// @file    MSDevice_Vehroutes.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Fri, 30.01.2009
/// @version $Id$
///
// A device which collects info on the vehicle trip
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2009-2014 DLR (http://www.dlr.de/) and contributors
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

#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSRoute.h>
#include <microsim/MSVehicleType.h>
#include <utils/vehicle/SUMOVehicle.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice_String.h>
#include "MSDevice_Vehroutes.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member variables
// ===========================================================================
bool MSDevice_Vehroutes::mySaveExits = false;
bool MSDevice_Vehroutes::myLastRouteOnly = false;
bool MSDevice_Vehroutes::myDUAStyle = false;
bool MSDevice_Vehroutes::mySorted = false;
bool MSDevice_Vehroutes::myIntendedDepart = false;
bool MSDevice_Vehroutes::myWithTaz = false;
MSDevice_Vehroutes::StateListener MSDevice_Vehroutes::myStateListener;
std::map<const SUMOTime, int> MSDevice_Vehroutes::myDepartureCounts;
std::map<const SUMOTime, std::map<const std::string, std::string> > MSDevice_Vehroutes::myRouteInfos;


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
void
MSDevice_Vehroutes::init() {
    if (OptionsCont::getOptions().isSet("vehroute-output")) {
        OutputDevice::createDeviceByOption("vehroute-output", "routes", "routes_file.xsd");
        mySaveExits = OptionsCont::getOptions().getBool("vehroute-output.exit-times");
        myLastRouteOnly = OptionsCont::getOptions().getBool("vehroute-output.last-route");
        myDUAStyle = OptionsCont::getOptions().getBool("vehroute-output.dua");
        mySorted = myDUAStyle || OptionsCont::getOptions().getBool("vehroute-output.sorted");
        myIntendedDepart = OptionsCont::getOptions().getBool("vehroute-output.intended-depart");
        myWithTaz = OptionsCont::getOptions().getBool("device.rerouting.with-taz");
        MSNet::getInstance()->addVehicleStateListener(&myStateListener);
    }
}


MSDevice_Vehroutes*
MSDevice_Vehroutes::buildVehicleDevices(SUMOVehicle& v, std::vector<MSDevice*>& into, unsigned int maxRoutes) {
    if (maxRoutes < INT_MAX) {
        return new MSDevice_Vehroutes(v, "vehroute_" + v.getID(), maxRoutes);
    }
    if (OptionsCont::getOptions().isSet("vehroute-output")) {
        if (myLastRouteOnly) {
            maxRoutes = 0;
        }
        myStateListener.myDevices[&v] = new MSDevice_Vehroutes(v, "vehroute_" + v.getID(), maxRoutes);
        into.push_back(myStateListener.myDevices[&v]);
        return myStateListener.myDevices[&v];
    }
    return 0;
}


// ---------------------------------------------------------------------------
// MSDevice_Vehroutes::StateListener-methods
// ---------------------------------------------------------------------------
void
MSDevice_Vehroutes::StateListener::vehicleStateChanged(const SUMOVehicle* const vehicle, MSNet::VehicleState to) {
    if (to == MSNet::VEHICLE_STATE_NEWROUTE) {
        myDevices[vehicle]->addRoute();
    }
}


// ---------------------------------------------------------------------------
// MSDevice_Vehroutes-methods
// ---------------------------------------------------------------------------
MSDevice_Vehroutes::MSDevice_Vehroutes(SUMOVehicle& holder, const std::string& id, unsigned int maxRoutes)
    : MSDevice(holder, id), myCurrentRoute(&holder.getRoute()), myMaxRoutes(maxRoutes), myLastSavedAt(0) {
    myCurrentRoute->addReference();
}


MSDevice_Vehroutes::~MSDevice_Vehroutes() {
    for (std::vector<RouteReplaceInfo>::iterator i = myReplacedRoutes.begin(); i != myReplacedRoutes.end(); ++i) {
        (*i).route->release();
    }
    myCurrentRoute->release();
    myStateListener.myDevices.erase(&myHolder);
}


bool
MSDevice_Vehroutes::notifyEnter(SUMOVehicle& veh, MSMoveReminder::Notification reason) {
    if (mySorted && reason == NOTIFICATION_DEPARTED && myStateListener.myDevices[&veh] == this) {
        const SUMOTime departure = myIntendedDepart ? myHolder.getParameter().depart : MSNet::getInstance()->getCurrentTimeStep();
        myDepartureCounts[departure]++;
    }
    return mySaveExits;
}


bool
MSDevice_Vehroutes::notifyLeave(SUMOVehicle& veh, SUMOReal /*lastPos*/, MSMoveReminder::Notification reason) {
    if (mySaveExits && reason != NOTIFICATION_LANE_CHANGE) {
        if (reason != NOTIFICATION_TELEPORT && myLastSavedAt == veh.getEdge()) { // need to check this for internal lanes
            myExits.back() = MSNet::getInstance()->getCurrentTimeStep();
        } else {
            myExits.push_back(MSNet::getInstance()->getCurrentTimeStep());
            myLastSavedAt = veh.getEdge();
        }
    }
    return mySaveExits;
}


void
MSDevice_Vehroutes::writeXMLRoute(OutputDevice& os, int index) const {
    // check if a previous route shall be written
    os.openTag(SUMO_TAG_ROUTE);
    if (index >= 0) {
        assert((int) myReplacedRoutes.size() > index);
        // write edge on which the vehicle was when the route was valid
        os << " replacedOnEdge=\"";
        if (myReplacedRoutes[index].edge) {
            os << myReplacedRoutes[index].edge->getID();
        }
        // write the time at which the route was replaced
        os << "\" replacedAtTime=\"" << time2string(myReplacedRoutes[index].time) << "\" probability=\"0\" edges=\"";
        // get the route
        int i = index;
        while (i > 0 && myReplacedRoutes[i - 1].edge) {
            i--;
        }
        const MSEdge* lastEdge = 0;
        for (; i < index; ++i) {
            myReplacedRoutes[i].route->writeEdgeIDs(os, lastEdge, myReplacedRoutes[i].edge);
            lastEdge = myReplacedRoutes[i].edge;
        }
        myReplacedRoutes[index].route->writeEdgeIDs(os, lastEdge);
    } else {
        os << " edges=\"";
        const MSEdge* lastEdge = 0;
        int numWritten = 0;
        if (myHolder.getNumberReroutes() > 0) {
            assert(myReplacedRoutes.size() <= myHolder.getNumberReroutes());
            unsigned int i = static_cast<unsigned int>(myReplacedRoutes.size());
            while (i > 0 && myReplacedRoutes[i - 1].edge) {
                i--;
            }
            for (; i < myReplacedRoutes.size(); ++i) {
                numWritten += myReplacedRoutes[i].route->writeEdgeIDs(os, lastEdge, myReplacedRoutes[i].edge);
                lastEdge = myReplacedRoutes[i].edge;
            }
        }
        const MSEdge* upTo = 0;
        if (mySaveExits) {
            int remainingWithExitTime = (int)myExits.size() - numWritten;
            assert(remainingWithExitTime >= 0);
            assert(remainingWithExitTime <= (int)myCurrentRoute->size());
            if (remainingWithExitTime < (int)myCurrentRoute->size()) {
                upTo = *(myCurrentRoute->begin() + remainingWithExitTime);
            }
        }
        myCurrentRoute->writeEdgeIDs(os, lastEdge, upTo);
        if (mySaveExits) {
            os << "\" exitTimes=\"";
            for (std::vector<SUMOTime>::const_iterator it = myExits.begin(); it != myExits.end(); ++it) {
                if (it != myExits.begin()) {
                    os << " ";
                }
                os << time2string(*it);
            }
        }
    }
    (os << "\"").closeTag();
}


void
MSDevice_Vehroutes::generateOutput() const {
    OutputDevice& routeOut = OutputDevice::getDeviceByOption("vehroute-output");
    OutputDevice_String od(routeOut.isBinary(), 1);
    const SUMOTime departure = myIntendedDepart ? myHolder.getParameter().depart : myHolder.getDeparture();
    od.openTag(SUMO_TAG_VEHICLE).writeAttr(SUMO_ATTR_ID, myHolder.getID());
    if (myHolder.getVehicleType().getID() != DEFAULT_VTYPE_ID) {
        od.writeAttr(SUMO_ATTR_TYPE, myHolder.getVehicleType().getID());
    }
    od.writeAttr(SUMO_ATTR_DEPART, time2string(departure));
    if (myHolder.hasArrived()) {
        od.writeAttr("arrival", time2string(MSNet::getInstance()->getCurrentTimeStep()));
    }
    if (myWithTaz) {
        od.writeAttr(SUMO_ATTR_FROM_TAZ, myHolder.getParameter().fromTaz).writeAttr(SUMO_ATTR_TO_TAZ, myHolder.getParameter().toTaz);
    }
    if (myDUAStyle) {
        const RandomDistributor<const MSRoute*>* const routeDist = MSRoute::distDictionary("!" + myHolder.getID());
        if (routeDist != 0) {
            const std::vector<const MSRoute*>& routes = routeDist->getVals();
            unsigned index = 0;
            while (index < routes.size() && routes[index] != myCurrentRoute) {
                ++index;
            }
            od.openTag(SUMO_TAG_ROUTE_DISTRIBUTION).writeAttr(SUMO_ATTR_LAST, index);
            const std::vector<SUMOReal>& probs = routeDist->getProbs();
            for (unsigned int i = 0; i < routes.size(); ++i) {
                od.setPrecision();
                od.openTag(SUMO_TAG_ROUTE).writeAttr(SUMO_ATTR_COST, routes[i]->getCosts());
                od.setPrecision(8);
                od.writeAttr(SUMO_ATTR_PROB, probs[i]);
                od.setPrecision();
                od << " edges=\"";
                routes[i]->writeEdgeIDs(od, *routes[i]->begin());
                (od << "\"").closeTag();
            }
            od.closeTag();
        } else {
            writeXMLRoute(od);
        }
    } else {
        if (myReplacedRoutes.size() > 0) {
            od.openTag(SUMO_TAG_ROUTE_DISTRIBUTION);
            for (unsigned int i = 0; i < myReplacedRoutes.size(); ++i) {
                writeXMLRoute(od, i);
            }
        }
        writeXMLRoute(od);
        if (myReplacedRoutes.size() > 0) {
            od.closeTag();
        }
    }
    od.closeTag();
    od.lf();
    if (mySorted) {
        myRouteInfos[departure][myHolder.getID()] = od.getString();
        myDepartureCounts[departure]--;
        std::map<const SUMOTime, int>::iterator it = myDepartureCounts.begin();
        while (it != myDepartureCounts.end() && it->second == 0) {
            std::map<const std::string, std::string>& infos = myRouteInfos[it->first];
            for (std::map<const std::string, std::string>::const_iterator it2 = infos.begin(); it2 != infos.end(); ++it2) {
                routeOut << it2->second;
            }
            myRouteInfos.erase(it->first);
            myDepartureCounts.erase(it);
            it = myDepartureCounts.begin();
        }
    } else {
        routeOut << od.getString();
    }
}


const MSRoute*
MSDevice_Vehroutes::getRoute(int index) const {
    return myReplacedRoutes[index].route;
}


void
MSDevice_Vehroutes::addRoute() {
    if (myMaxRoutes > 0) {
        if (myHolder.hasDeparted()) {
            myReplacedRoutes.push_back(RouteReplaceInfo(myHolder.getEdge(), MSNet::getInstance()->getCurrentTimeStep(), myCurrentRoute));
        } else {
            myReplacedRoutes.push_back(RouteReplaceInfo(0, MSNet::getInstance()->getCurrentTimeStep(), myCurrentRoute));
        }
        if (myReplacedRoutes.size() > myMaxRoutes) {
            myReplacedRoutes.front().route->release();
            myReplacedRoutes.erase(myReplacedRoutes.begin());
        }
    } else {
        myCurrentRoute->release();
    }
    myCurrentRoute = &myHolder.getRoute();
    myCurrentRoute->addReference();
}


void
MSDevice_Vehroutes::generateOutputForUnfinished() {
    for (std::map<const SUMOVehicle*, MSDevice_Vehroutes*, Named::NamedLikeComparatorIdLess<SUMOVehicle> >::const_iterator it = myStateListener.myDevices.begin();
            it != myStateListener.myDevices.end(); ++it) {
        if (it->first->hasDeparted()) {
            it->second->generateOutput();
        }
    }
}


/****************************************************************************/

