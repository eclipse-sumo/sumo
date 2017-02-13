/****************************************************************************/
/// @file    MSTriggeredRerouter.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 25 July 2005
/// @version $Id$
///
// Reroutes vehicles passing an edge
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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

#include <string>
#include <algorithm>
#include <utils/options/OptionsCont.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/Command.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include <utils/common/TplConvert.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/vehicle/DijkstraRouterTT.h>
#include <utils/common/RandHelper.h>
#include <utils/common/WrappingCommand.h>
#include <microsim/MSEdgeWeightsStorage.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSRoute.h>
#include <microsim/MSEdge.h>
#include <microsim/MSNet.h>
#include <microsim/MSGlobals.h>
#include <microsim/devices/MSDevice_Routing.h>
#include "MSTriggeredRerouter.h"

#include <mesosim/MELoop.h>
#include <mesosim/MESegment.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

//#define DEBUG_REROUTER
#define DEBUGCOND (veh.getID() == "disabled")

// ===========================================================================
// static member defintion
// ===========================================================================
MSEdge MSTriggeredRerouter::mySpecialDest_keepDestination("MSTriggeredRerouter_keepDestination", -1, MSEdge::EDGEFUNCTION_UNKNOWN, "", "", -1);
MSEdge MSTriggeredRerouter::mySpecialDest_terminateRoute("MSTriggeredRerouter_terminateRoute", -1, MSEdge::EDGEFUNCTION_UNKNOWN, "", "", -1);

// ===========================================================================
// method definitions
// ===========================================================================
MSTriggeredRerouter::MSTriggeredRerouter(const std::string& id,
        const MSEdgeVector& edges,
        SUMOReal prob, const std::string& file, bool off) :
    MSTrigger(id),
    MSMoveReminder(id),
    SUMOSAXHandler(file),
    myProbability(prob), myUserProbability(prob), myAmInUserMode(false) {
    // build actors
    for (MSEdgeVector::const_iterator j = edges.begin(); j != edges.end(); ++j) {
        if (MSGlobals::gUseMesoSim) {
            MESegment* s = MSGlobals::gMesoNet->getSegmentForEdge(**j);
            s->addDetector(this);
            continue;
        }
        const std::vector<MSLane*>& destLanes = (*j)->getLanes();
        for (std::vector<MSLane*>::const_iterator i = destLanes.begin(); i != destLanes.end(); ++i) {
            (*i)->addMoveReminder(this);
        }
    }
    if (off) {
        setUserMode(true);
        setUserUsageProbability(0);
    }
}


MSTriggeredRerouter::~MSTriggeredRerouter() {
}

// ------------ loading begin
void
MSTriggeredRerouter::myStartElement(int element,
                                    const SUMOSAXAttributes& attrs) {
    if (element == SUMO_TAG_INTERVAL) {
        bool ok = true;
        myCurrentIntervalBegin = attrs.getOptSUMOTimeReporting(SUMO_ATTR_BEGIN, 0, ok, -1);
        myCurrentIntervalEnd = attrs.getOptSUMOTimeReporting(SUMO_ATTR_END, 0, ok, -1);
    }
    if (element == SUMO_TAG_DEST_PROB_REROUTE) {
        // by giving probabilities of new destinations
        // get the destination edge
        std::string dest = attrs.getStringSecure(SUMO_ATTR_ID, "");
        if (dest == "") {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": No destination edge id given.");
        }
        MSEdge* to = MSEdge::dictionary(dest);
        if (to == 0) {
            if (dest == "keepDestination") {
                to = &mySpecialDest_keepDestination;
            } else if (dest == "terminateRoute") {
                to = &mySpecialDest_terminateRoute;
            } else {
                throw ProcessError("MSTriggeredRerouter " + getID() + ": Destination edge '" + dest + "' is not known.");
            }
        }
        // get the probability to reroute
        bool ok = true;
        SUMOReal prob = attrs.getOpt<SUMOReal>(SUMO_ATTR_PROB, getID().c_str(), ok, 1.);
        if (!ok) {
            throw ProcessError();
        }
        if (prob < 0) {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": Attribute 'probability' for destination '" + dest + "' is negative (must not).");
        }
        // add
        myCurrentEdgeProb.add(prob, to);
    }


    if (element == SUMO_TAG_CLOSING_REROUTE) {
        // by closing
        std::string closed_id = attrs.getStringSecure(SUMO_ATTR_ID, "");
        MSEdge* closed = MSEdge::dictionary(closed_id);
        if (closed == 0) {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": Edge '" + closed_id + "' to close is not known.");
        }
        myCurrentClosed.push_back(closed);
        bool ok;
        const std::string allow = attrs.getOpt<std::string>(SUMO_ATTR_ALLOW, getID().c_str(), ok, "", false);
        const std::string disallow = attrs.getOpt<std::string>(SUMO_ATTR_DISALLOW, getID().c_str(), ok, "");
        myCurrentPermissions = parseVehicleClasses(allow, disallow);
    }

    if (element == SUMO_TAG_CLOSING_LANE_REROUTE) {
        // by closing lane
        std::string closed_id = attrs.getStringSecure(SUMO_ATTR_ID, "");
        MSLane* closed = MSLane::dictionary(closed_id);
        if (closed == 0) {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": Lane '" + closed_id + "' to close is not known.");
        }
        myCurrentClosedLanes.push_back(closed);
        bool ok;
        if (attrs.hasAttribute(SUMO_ATTR_ALLOW) || attrs.hasAttribute(SUMO_ATTR_DISALLOW)) {
            const std::string allow = attrs.getOpt<std::string>(SUMO_ATTR_ALLOW, getID().c_str(), ok, "", false);
            const std::string disallow = attrs.getOpt<std::string>(SUMO_ATTR_DISALLOW, getID().c_str(), ok, "");
            myCurrentPermissions = parseVehicleClasses(allow, disallow);
        } else {
            // lane closing only makes sense if the lane really receives reduced
            // permissions
            myCurrentPermissions = SVC_AUTHORITY;
        }
    }

    if (element == SUMO_TAG_ROUTE_PROB_REROUTE) {
        // by explicit rerouting using routes
        // check if route exists
        std::string routeStr = attrs.getStringSecure(SUMO_ATTR_ID, "");
        if (routeStr == "") {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": No route id given.");
        }
        const MSRoute* route = MSRoute::dictionary(routeStr);
        if (route == 0) {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": Route '" + routeStr + "' does not exist.");
        }

        // get the probability to reroute
        bool ok = true;
        SUMOReal prob = attrs.getOpt<SUMOReal>(SUMO_ATTR_PROB, getID().c_str(), ok, 1.);
        if (!ok) {
            throw ProcessError();
        }
        if (prob < 0) {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": Attribute 'probability' for route '" + routeStr + "' is negative (must not).");
        }
        // add
        myCurrentRouteProb.add(prob, route);
    }

    if (element == SUMO_TAG_PARKING_ZONE_REROUTE) {
        // by giving probabilities of new destinations
        // get the destination edge
        std::string parkingarea = attrs.getStringSecure(SUMO_ATTR_ID, "");
        if (parkingarea == "") {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": No parking area id given.");
        }
        MSParkingArea* pa = MSNet::getInstance()->getParkingArea(parkingarea);
        if (pa == 0) {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": Parking area '" + parkingarea + "' is not known.");
        }
        // get the probability to reroute
        bool ok = true;
        SUMOReal prob = attrs.getOpt<SUMOReal>(SUMO_ATTR_PROB, getID().c_str(), ok, 1.);
        if (!ok) {
            throw ProcessError();
        }
        if (prob < 0) {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": Attribute 'probability' for destination '" + parkingarea + "' is negative (must not).");
        }
        // add
        myCurrentParkProb.add(prob, pa);
        //MSEdge* to = &(pa->getLane().getEdge());
        //myCurrentEdgeProb.add(prob, to);
    }
}


void
MSTriggeredRerouter::myEndElement(int element) {
    if (element == SUMO_TAG_INTERVAL) {
        RerouteInterval ri;
        ri.begin = myCurrentIntervalBegin;
        ri.end = myCurrentIntervalEnd;
        ri.closed = myCurrentClosed;
        ri.closedLanes = myCurrentClosedLanes;
        ri.edgeProbs = myCurrentEdgeProb;
        ri.routeProbs = myCurrentRouteProb;
        ri.permissions = myCurrentPermissions;
        ri.parkProbs = myCurrentParkProb;
        if (ri.closedLanes.size() > 0) {
            // collect edges that are affect by a closed lane
            std::set<MSEdge*> affected;
            for (std::vector<MSLane*>::iterator l = ri.closedLanes.begin(); l != ri.closedLanes.end(); ++l) {
                affected.insert(&((*l)->getEdge()));
            }
            ri.closedLanesAffected.insert(ri.closedLanesAffected.begin(), affected.begin(), affected.end());
        }
        myCurrentClosed.clear();
        myCurrentClosedLanes.clear();
        myCurrentEdgeProb.clear();
        myCurrentRouteProb.clear();
        myCurrentParkProb.clear();
        myIntervals.push_back(ri);
        myIntervals.back().id = (long)&myIntervals.back();
        if (!(ri.closed.empty() && ri.closedLanes.empty()) && ri.permissions != SVCAll) {
            MSNet::getInstance()->getBeginOfTimestepEvents()->addEvent(
                new WrappingCommand<MSTriggeredRerouter>(this, &MSTriggeredRerouter::setPermissions), ri.begin,
                MSEventControl::ADAPT_AFTER_EXECUTION);
        }
    }
}


// ------------ loading end


SUMOTime
MSTriggeredRerouter::setPermissions(const SUMOTime currentTime) {
    for (std::vector<RerouteInterval>::iterator i = myIntervals.begin(); i != myIntervals.end(); ++i) {
        if (i->begin == currentTime && !(i->closed.empty() && i->closedLanes.empty()) && i->permissions != SVCAll) {
            for (MSEdgeVector::iterator e = i->closed.begin(); e != i->closed.end(); ++e) {
                for (std::vector<MSLane*>::const_iterator l = (*e)->getLanes().begin(); l != (*e)->getLanes().end(); ++l) {
                    //std::cout << SIMTIME << " closing: intervalID=" << i->id << " lane=" << (*l)->getID() << " prevPerm=" << getVehicleClassNames((*l)->getPermissions()) << " new=" << getVehicleClassNames(i->permissions) << "\n";
                    (*l)->setPermissions(i->permissions, i->id);
                }
                (*e)->rebuildAllowedLanes();
            }
            for (std::vector<MSLane*>::iterator l = i->closedLanes.begin(); l != i->closedLanes.end(); ++l) {
                (*l)->setPermissions(i->permissions, i->id);
                (*l)->getEdge().rebuildAllowedLanes();
            }
            MSNet::getInstance()->getBeginOfTimestepEvents()->addEvent(
                new WrappingCommand<MSTriggeredRerouter>(this, &MSTriggeredRerouter::setPermissions), i->end,
                MSEventControl::ADAPT_AFTER_EXECUTION);
        }
        if (i->end == currentTime && !(i->closed.empty() && i->closedLanes.empty()) && i->permissions != SVCAll) {
            for (MSEdgeVector::iterator e = i->closed.begin(); e != i->closed.end(); ++e) {
                for (std::vector<MSLane*>::const_iterator l = (*e)->getLanes().begin(); l != (*e)->getLanes().end(); ++l) {
                    (*l)->resetPermissions(i->id);
                    //std::cout << SIMTIME << " opening: intervalID=" << i->id << " lane=" << (*l)->getID() << " restore prevPerm=" << getVehicleClassNames((*l)->getPermissions()) << "\n";
                }
                (*e)->rebuildAllowedLanes();
            }
            for (std::vector<MSLane*>::iterator l = i->closedLanes.begin(); l != i->closedLanes.end(); ++l) {
                (*l)->resetPermissions(i->id);
                (*l)->getEdge().rebuildAllowedLanes();
            }
        }
    }
    return 0;
}


const MSTriggeredRerouter::RerouteInterval*
MSTriggeredRerouter::getCurrentReroute(SUMOTime time, SUMOVehicle& veh) const {
    for (std::vector<RerouteInterval>::const_iterator i = myIntervals.begin(); i != myIntervals.end(); ++i) {
        if (i->begin <= time && i->end > time) {
            if (
                // destProbReroute
                i->edgeProbs.getOverallProb() > 0 ||
                // routeProbReroute
                i->routeProbs.getOverallProb() > 0 ||
                // parkingZoneReroute
                i->parkProbs.getOverallProb() > 0 ||
                // affected by closingReroute
                veh.getRoute().containsAnyOf(i->closed) ||
                // affected by closingLaneReroute
                veh.getRoute().containsAnyOf(i->closedLanesAffected)) {
                return &*i;
            }
        }
    }
    return 0;
}


const MSTriggeredRerouter::RerouteInterval*
MSTriggeredRerouter::getCurrentReroute(SUMOTime time) const {
    for (std::vector<RerouteInterval>::const_iterator i = myIntervals.begin(); i != myIntervals.end(); ++i) {
        if (i->begin <= time && i->end > time) {
            if (i->parkProbs.getOverallProb() != 0 || i->edgeProbs.getOverallProb() != 0 || i->routeProbs.getOverallProb() != 0 || !i->closed.empty()) {
                return &*i;
            }
        }
    }
    return 0;
}


bool
MSTriggeredRerouter::notifyMove(SUMOVehicle& veh, SUMOReal /*oldPos*/,
                                SUMOReal /*newPos*/, SUMOReal /*newSpeed*/) {
    return notifyEnter(veh, NOTIFICATION_JUNCTION);
}


bool
MSTriggeredRerouter::notifyLeave(SUMOVehicle& /*veh*/, SUMOReal /*lastPos*/,
                                 MSMoveReminder::Notification reason) {
    return reason == NOTIFICATION_LANE_CHANGE;
}


bool
MSTriggeredRerouter::notifyEnter(SUMOVehicle& veh, MSMoveReminder::Notification /*reason*/) {
    // check whether the vehicle shall be rerouted
    const SUMOTime time = MSNet::getInstance()->getCurrentTimeStep();
    const MSTriggeredRerouter::RerouteInterval* rerouteDef = getCurrentReroute(time, veh);
    if (rerouteDef == 0) {
        return true; // an active interval could appear later
    }
    SUMOReal prob = myAmInUserMode ? myUserProbability : myProbability;
    if (RandHelper::rand() > prob) {
        return false; // XXX another interval could appear later but we would have to track whether the current interval was already tried
    }
    // if we have a closingLaneReroute, only vehicles with a rerouting device can profit from rerouting (otherwise, edge weights will not reflect local jamming)
    const bool hasReroutingDevice = veh.getDevice(typeid(MSDevice_Routing)) != 0;
    if (rerouteDef->closedLanes.size() > 0 && !hasReroutingDevice) {
        return true; // an active interval could appear later
    }
    // get vehicle params
    const MSRoute& route = veh.getRoute();
    const MSEdge* lastEdge = route.getLastEdge();
#ifdef DEBUG_REROUTER
    if (DEBUGCOND) {
        std::cout << SIMTIME << " veh=" << veh.getID() << " check rerouter " << getID() << " lane=" << veh.getLane()->getID() << " edge=" << veh.getEdge()->getID() << " finalEdge=" << lastEdge->getID() << " arrivalPos=" << veh.getArrivalPos() << "\n";
    }
#endif

    if (rerouteDef->parkProbs.getOverallProb() > 0) {
        MSParkingArea* newParkingArea = rerouteParkingZone(rerouteDef, veh);
        if (newParkingArea != 0) {
            const MSEdge* newEdge = &(newParkingArea->getLane().getEdge());

            SUMOAbstractRouter<MSEdge, SUMOVehicle>& router = hasReroutingDevice
                    ? MSDevice_Routing::getRouterTT(rerouteDef->closed)
                    : MSNet::getInstance()->getRouterTT(rerouteDef->closed);

            // Compute the route from the current edge to the parking area edge
            ConstMSEdgeVector edgesToPark;
            router.compute(veh.getEdge(), newEdge, &veh, MSNet::getInstance()->getCurrentTimeStep(), edgesToPark);

            // Compute the route from the parking area edge to the end of the route
            ConstMSEdgeVector edgesFromPark;
            router.compute(newEdge, lastEdge, &veh, MSNet::getInstance()->getCurrentTimeStep(), edgesFromPark);

            // we have a new destination, let's replace the vehicle route
            ConstMSEdgeVector edges = edgesToPark;
            if (edgesFromPark.size() > 0) {
                edges.insert(edges.end(), edgesFromPark.begin() + 1, edgesFromPark.end());
            }

            veh.replaceRouteEdges(edges, false, false, false);
            std::string errorMsg;
            if (!veh.replaceParkingArea(newParkingArea, errorMsg)) {
                WRITE_WARNING("Vehicle '" + veh.getID() + "' at rerouter '" + getID()
                              + "' could not reroute to new parkingArea '" + newParkingArea->getID()
                              + "' reason=" + errorMsg + ", time=" + time2string(MSNet::getInstance()->getCurrentTimeStep()) + ".");
            }
        }
        return false;
    }

    // get rerouting params
    const MSRoute* newRoute = rerouteDef->routeProbs.getOverallProb() > 0 ? rerouteDef->routeProbs.get() : 0;
    // we will use the route if given rather than calling our own dijsktra...
    if (newRoute != 0) {
#ifdef DEBUG_REROUTER
        if (DEBUGCOND) {
            std::cout << "    replacedRoute from routeDist " << newRoute->getID() << "\n";
        }
#endif
        veh.replaceRoute(newRoute);
        return false; // XXX another interval could appear later but we would have to track whether the currenty interval was already used
    }
    const MSEdge* newEdge = lastEdge;
    // ok, try using a new destination
    SUMOReal newArrivalPos = -1;
    const bool destUnreachable = std::find(rerouteDef->closed.begin(), rerouteDef->closed.end(), lastEdge) != rerouteDef->closed.end();
    // if we have a closingReroute, only assign new destinations to vehicles which cannot reach their original destination
    // if we have a closingLaneReroute, no new destinations should be assigned
    if (rerouteDef->closed.size() == 0 || destUnreachable) {
        newEdge = rerouteDef->edgeProbs.getOverallProb() > 0 ? rerouteDef->edgeProbs.get() : route.getLastEdge();
        if (newEdge == &mySpecialDest_terminateRoute) {
            newEdge = veh.getEdge();
            newArrivalPos = veh.getPositionOnLane(); // instant arrival
        } else if (newEdge == &mySpecialDest_keepDestination || newEdge == lastEdge) {
            if (destUnreachable && rerouteDef->permissions == SVCAll) {
                // if permissions aren't set vehicles will simply drive through
                // the closing unless terminated. If the permissions are specified, assume that the user wants
                // vehicles to stand and wait until the closing ends
                WRITE_WARNING("Cannot keep destination edge '" + lastEdge->getID() + "' for vehicle '" + veh.getID() + "' due to closed edges. Terminating route.");
                newEdge = veh.getEdge();
            } else {
                newEdge = lastEdge;
            }
        } else if (newEdge == 0) {
#ifdef DEBUG_REROUTER
            if (DEBUGCOND) {
                std::cout << "   could not find new edge!\n";
            }
#endif
            assert(false); // this should never happen
            newEdge = veh.getEdge();
        }
    }
    // we have a new destination, let's replace the vehicle route (if it is affected)
    if (rerouteDef->closed.size() == 0 || destUnreachable || veh.getRoute().containsAnyOf(rerouteDef->closed)) {
        ConstMSEdgeVector edges;
        SUMOAbstractRouter<MSEdge, SUMOVehicle>& router = hasReroutingDevice
                ? MSDevice_Routing::getRouterTT(rerouteDef->closed)
                : MSNet::getInstance()->getRouterTT(rerouteDef->closed);
        router.compute(
            veh.getEdge(), newEdge, &veh, MSNet::getInstance()->getCurrentTimeStep(), edges);
        const bool useNewRoute = veh.replaceRouteEdges(edges);
#ifdef DEBUG_REROUTER
        if (DEBUGCOND) std::cout << "   rerouting:  newEdge=" << newEdge->getID() << " useNewRoute=" << useNewRoute << " newArrivalPos=" << newArrivalPos << " numClosed=" << rerouteDef->closed.size()
                                     << " destUnreachable=" << destUnreachable << " containsClosed=" << veh.getRoute().containsAnyOf(rerouteDef->closed) << "\n";
#endif
        if (useNewRoute && newArrivalPos != -1) {
            // must be called here because replaceRouteEdges may also set the arrivalPos
            veh.setArrivalPos(newArrivalPos);
        }
    }
    return false; // XXX another interval could appear later but we would have to track whether the currenty interval was already used
}


void
MSTriggeredRerouter::setUserMode(bool val) {
    myAmInUserMode = val;
}


void
MSTriggeredRerouter::setUserUsageProbability(SUMOReal prob) {
    myUserProbability = prob;
}


bool
MSTriggeredRerouter::inUserMode() const {
    return myAmInUserMode;
}


SUMOReal
MSTriggeredRerouter::getProbability() const {
    return myAmInUserMode ? myUserProbability : myProbability;
}


SUMOReal
MSTriggeredRerouter::getUserProbability() const {
    return myUserProbability;
}


SUMOReal
MSTriggeredRerouter::getWeight(SUMOVehicle& veh, const std::string param, const SUMOReal defaultWeight) const {
    // get custom vehicle parameter
    if (veh.getParameter().knowsParameter(param)) {
        try {
            return TplConvert::_2SUMOReal(veh.getParameter().getParameter(param, "-1").c_str());
        } catch (...) {
            WRITE_WARNING("Invalid value '" + veh.getParameter().getParameter(param, "-1") + "' for vehicle parameter '" + param + "'");
        }
    } else {
        // get custom vType parameter
        if (veh.getVehicleType().getParameter().knowsParameter(param)) {
            try {
                return TplConvert::_2SUMOReal(veh.getVehicleType().getParameter().getParameter(param, "-1").c_str());
            } catch (...) {
                WRITE_WARNING("Invalid value '" + veh.getVehicleType().getParameter().getParameter(param, "-1") + "' for vType parameter '" + param + "'");
            }
        }
    }
    //WRITE_MESSAGE("Vehicle '" +veh.getID() + "' does not supply vehicle parameter '" + param + "'. Using default of " + toString(defaultWeight) + "\n";
    return defaultWeight;
}


MSParkingArea*
MSTriggeredRerouter::rerouteParkingZone(const MSTriggeredRerouter::RerouteInterval* rerouteDef, SUMOVehicle& veh) const {

    MSParkingArea* nearParkArea = 0;

    // get vehicle params
    MSParkingArea* destParkArea = veh.getNextParkingArea();
    const MSRoute& route = veh.getRoute();

    // I reroute destination from initial parking area to the near parking area
    // if the next stop is a parking area and if it is full
    if (destParkArea != 0 &&
            destParkArea->getOccupancy() == destParkArea->getCapacity()) {

        typedef std::map<std::string, SUMOReal> ParkingParamMap_t;
        typedef std::map<MSParkingArea*, ParkingParamMap_t> MSParkingAreaMap_t;

        ParkingParamMap_t weights;

        // The probability of choosing this area inside the zone
        weights["probability"] = getWeight(veh, "parking.probability.weight", 0.0);

        // The capacity of this area
        weights["capacity"] = getWeight(veh, "parking.capacity.weight", 0.0);

        // The absolute number of free spaces
        weights["absfreespace"] = getWeight(veh, "parking.absfreespace.weight", 0.0);

        // The relative number of free spaces
        weights["relfreespace"] = getWeight(veh, "parking.relfreespace.weight", 0.0);

        // The distance to the new parking area
        weights["distanceto"] = getWeight(veh, "parking.distanceto.weight", getWeight(veh, "parking.distance.weight", 1.0));

        // The time to reach this area
        weights["timeto"] = getWeight(veh, "parking.timeto.weight", 0.0);

        // The distance from the new parking area
        weights["distancefrom"] = getWeight(veh, "parking.distancefrom.weight", 0.0);

        // The time to reach the end from this area
        weights["timefrom"] = getWeight(veh, "parking.timefrom.weight", 0.0);

        // a map stores maximum values to normalize parking values
        ParkingParamMap_t maxValues;

        maxValues["probability"] = 0.0;
        maxValues["capacity"] = 0.0;
        maxValues["absfreespace"] = 0.0;
        maxValues["relfreespace"] = 0.0;
        maxValues["distanceto"] = 0.0;
        maxValues["timeto"] = 0.0;
        maxValues["distancefrom"] = 0.0;
        maxValues["timefrom"] = 0.0;

        // a map stores elegible parking areas
        MSParkingAreaMap_t parkAreas;

        SUMOAbstractRouter<MSEdge, SUMOVehicle>& router = MSNet::getInstance()->getRouterTT(rerouteDef->closed);

        std::vector<MSParkingArea*> parks = rerouteDef->parkProbs.getVals();
        std::vector<SUMOReal> probs = rerouteDef->parkProbs.getProbs();

        for (int i = 0; i < (int)parks.size(); ++i) {
            MSParkingArea* pa = parks[i];
            const SUMOReal prob = probs[i];
            if (pa->getOccupancy() < pa->getCapacity()) {

                // a map stores the parking values
                ParkingParamMap_t parkValues;

                const RGBColor& c = route.getColor();
                const MSEdge* parkEdge = &(pa->getLane().getEdge());

                const bool includeInternalLengths = MSGlobals::gUsingInternalLanes && MSNet::getInstance()->hasInternalLinks();

                // Compute the route from the current edge to the parking area edge
                ConstMSEdgeVector edgesToPark;
                router.compute(veh.getEdge(), parkEdge, &veh, MSNet::getInstance()->getCurrentTimeStep(), edgesToPark);

                if (edgesToPark.size() > 0) {
                    // Compute the route from the parking area edge to the end of the route
                    ConstMSEdgeVector edgesFromPark;
                    router.compute(parkEdge, route.getLastEdge(), &veh, MSNet::getInstance()->getCurrentTimeStep(), edgesFromPark);

                    if (edgesFromPark.size() > 0) {

                        parkValues["probability"] = prob;

                        if (parkValues["probability"] > maxValues["probability"]) {
                            maxValues["probability"] = parkValues["probability"];
                        }

                        parkValues["capacity"] = (double)(pa->getCapacity());
                        parkValues["absfreespace"] = (double)(pa->getCapacity() - pa->getOccupancy());
                        parkValues["relfreespace"] = parkValues["absfreespace"] / parkValues["capacity"];

                        if (parkValues["capacity"] > maxValues["capacity"]) {
                            maxValues["capacity"] = parkValues["capacity"];
                        }

                        if (parkValues["absfreespace"] > maxValues["absfreespace"]) {
                            maxValues["absfreespace"] = parkValues["absfreespace"];
                        }

                        if (parkValues["relfreespace"] > maxValues["relfreespace"]) {
                            maxValues["relfreespace"] = parkValues["relfreespace"];
                        }

                        MSRoute routeToPark(route.getID() + "!topark#1", edgesToPark, false, &c == &RGBColor::DEFAULT_COLOR ? 0 : new RGBColor(c), route.getStops());

                        // The distance from the current edge to the new parking area
                        parkValues["distanceto"] = routeToPark.getDistanceBetween(veh.getPositionOnLane(), pa->getBeginLanePosition(),
                                                   routeToPark.begin(), routeToPark.end(), includeInternalLengths);

                        // The time to reach the new parking area
                        parkValues["timeto"] = router.recomputeCosts(edgesToPark, &veh, MSNet::getInstance()->getCurrentTimeStep());

                        if (parkValues["distanceto"] > maxValues["distanceto"]) {
                            maxValues["distanceto"] = parkValues["distanceto"];
                        }

                        if (parkValues["timeto"] > maxValues["timeto"]) {
                            maxValues["timeto"] = parkValues["timeto"];
                        }

                        MSRoute routeFromPark(route.getID() + "!frompark#1", edgesFromPark, false, &c == &RGBColor::DEFAULT_COLOR ? 0 : new RGBColor(c), route.getStops());

                        // The distance from the new parking area to the end of the route
                        parkValues["distancefrom"] = routeFromPark.getDistanceBetween(pa->getBeginLanePosition(), routeFromPark.getLastEdge()->getLength(),
                                                     routeFromPark.begin(), routeFromPark.end(), includeInternalLengths);

                        // The time to reach this area
                        parkValues["timefrom"] = router.recomputeCosts(edgesFromPark, &veh, MSNet::getInstance()->getCurrentTimeStep());

                        if (parkValues["distancefrom"] > maxValues["distancefrom"]) {
                            maxValues["distancefrom"] = parkValues["distancefrom"];
                        }

                        if (parkValues["timefrom"] > maxValues["timefrom"]) {
                            maxValues["timefrom"] = parkValues["timefrom"];
                        }

                        parkAreas[pa] = parkValues;
                    }
                }
            }
        }

        // minimum cost to get the parking area
        SUMOReal minParkingCost = 0.0;

        for (MSParkingAreaMap_t::iterator it = parkAreas.begin(); it != parkAreas.end(); ++it) {
            // get the parking values
            ParkingParamMap_t parkValues = it->second;

            // normalizing parking values with maximum values (we want to maximize some parameters then we reverse the value)
            parkValues["probability"] = maxValues["probability"] > 0.0 ? 1.0 - parkValues["probability"] / maxValues["probability"] : 0.0;
            parkValues["capacity"] = maxValues["capacity"] > 0.0 ? 1.0 - parkValues["capacity"] / maxValues["capacity"] : 0.0;
            parkValues["absfreespace"] = maxValues["absfreespace"] > 0.0 ? 1.0 - parkValues["absfreespace"] / maxValues["absfreespace"] : 0.0;
            parkValues["relfreespace"] = maxValues["relfreespace"] > 0.0 ? 1.0 - parkValues["relfreespace"] / maxValues["relfreespace"] : 0.0;

            parkValues["distanceto"] = maxValues["distanceto"] > 0.0 ? parkValues["distanceto"] / maxValues["distanceto"] : 0.0;
            parkValues["timeto"] = maxValues["timeto"] > 0.0 ? parkValues["timeto"] / maxValues["timeto"] : 0.0;

            parkValues["distancefrom"] = maxValues["distancefrom"] > 0.0 ? parkValues["distancefrom"] / maxValues["distancefrom"] : 0.0;
            parkValues["timefrom"] = maxValues["timefrom"] > 0.0 ? parkValues["timefrom"] / maxValues["timefrom"] : 0.0;

            // get the parking area cost
            SUMOReal parkingCost = 0.0;

            // sum every index with its weight
            for (ParkingParamMap_t::iterator pc = parkValues.begin(); pc != parkValues.end(); ++pc) {
                parkingCost += weights[pc->first] * pc->second;
            }

            // get the parking area with minimum cost
            if (nearParkArea == 0 || parkingCost < minParkingCost) {
                minParkingCost = parkingCost;
                nearParkArea = it->first;
            }
        }
    }

    return nearParkArea;
}

/****************************************************************************/

