/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    MSTriggeredRerouter.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Mirco Sturari
/// @author  Mirko Barthauer
/// @date    Mon, 25 July 2005
///
// Reroutes vehicles passing an edge
/****************************************************************************/
#include <config.h>

#include <string>
#include <algorithm>
#ifdef HAVE_FOX
#include <utils/common/ScopedLocker.h>
#endif
#include <utils/options/OptionsCont.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/Command.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include <utils/common/StringUtils.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/router/DijkstraRouter.h>
#include <utils/common/RandHelper.h>
#include <utils/common/WrappingCommand.h>
#include <microsim/MSEdgeWeightsStorage.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSRoute.h>
#include <microsim/MSEdge.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSParkingArea.h>
#include <microsim/MSStop.h>
#include <microsim/transportables/MSPerson.h>
#include <microsim/devices/MSDevice_Routing.h>
#include <microsim/devices/MSRoutingEngine.h>
#include "MSTriggeredRerouter.h"

#include <mesosim/MELoop.h>
#include <mesosim/MESegment.h>

//#define DEBUG_REROUTER
#define DEBUGCOND (veh.isSelected())
//#define DEBUGCOND (true)
//#define DEBUGCOND (veh.getID() == "")

// ===========================================================================
// static member definition
// ===========================================================================
MSEdge MSTriggeredRerouter::mySpecialDest_keepDestination("MSTriggeredRerouter_keepDestination", -1, SumoXMLEdgeFunc::UNKNOWN, "", "", -1, 0);
MSEdge MSTriggeredRerouter::mySpecialDest_terminateRoute("MSTriggeredRerouter_terminateRoute", -1, SumoXMLEdgeFunc::UNKNOWN, "", "", -1, 0);
std::map<std::string, MSTriggeredRerouter*> MSTriggeredRerouter::myInstances;


// ===========================================================================
// method definitions
// ===========================================================================
MSTriggeredRerouter::MSTriggeredRerouter(const std::string& id,
        const MSEdgeVector& edges, double prob, bool off, bool optional,
        SUMOTime timeThreshold, const std::string& vTypes, const Position& pos) :
    Named(id),
    MSMoveReminder(id),
    MSStoppingPlaceRerouter(SUMO_TAG_PARKING_AREA, "parking"),
    myEdges(edges),
    myProbability(prob),
    myUserProbability(prob),
    myAmInUserMode(false),
    myAmOptional(optional),
    myPosition(pos),
    myTimeThreshold(timeThreshold),
    myHaveParkProbs(false) {
    myInstances[id] = this;
    // build actors
    for (const MSEdge* const e : edges) {
        if (MSGlobals::gUseMesoSim) {
            MSGlobals::gMesoNet->getSegmentForEdge(*e)->addDetector(this);
        }
        for (MSLane* const lane : e->getLanes()) {
            lane->addMoveReminder(this);
        }
    }
    if (off) {
        setUserMode(true);
        setUserUsageProbability(0);
    }
    const std::vector<std::string> vt = StringTokenizer(vTypes).getVector();
    myVehicleTypes.insert(vt.begin(), vt.end());
}


MSTriggeredRerouter::~MSTriggeredRerouter() {
    myInstances.erase(getID());
}


// ------------ loading begin
void
MSTriggeredRerouter::myStartElement(int element,
                                    const SUMOSAXAttributes& attrs) {
    if (element == SUMO_TAG_INTERVAL) {
        bool ok = true;
        myParsedRerouteInterval = RerouteInterval();
        myParsedRerouteInterval.begin = attrs.getOptSUMOTimeReporting(SUMO_ATTR_BEGIN, nullptr, ok, -1);
        myParsedRerouteInterval.end = attrs.getOptSUMOTimeReporting(SUMO_ATTR_END, nullptr, ok, SUMOTime_MAX);
    }
    if (element == SUMO_TAG_DEST_PROB_REROUTE) {
        // by giving probabilities of new destinations
        // get the destination edge
        std::string dest = attrs.getStringSecure(SUMO_ATTR_ID, "");
        if (dest == "") {
            throw ProcessError(TLF("MSTriggeredRerouter %: No destination edge id given.", getID()));
        }
        MSEdge* to = MSEdge::dictionary(dest);
        if (to == nullptr) {
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
        double prob = attrs.getOpt<double>(SUMO_ATTR_PROB, getID().c_str(), ok, 1.);
        if (!ok) {
            throw ProcessError();
        }
        if (prob < 0) {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": Attribute 'probability' for destination '" + dest + "' is negative (must not).");
        }
        // add
        myParsedRerouteInterval.edgeProbs.add(to, prob);
    }

    if (element == SUMO_TAG_CLOSING_REROUTE) {
        // by closing edge
        const std::string& closed_id = attrs.getStringSecure(SUMO_ATTR_ID, "");
        MSEdge* const closed = MSEdge::dictionary(closed_id);
        if (closed == nullptr) {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": Edge '" + closed_id + "' to close is not known.");
        }
        myParsedRerouteInterval.closed.push_back(closed);
        bool ok;
        const std::string allow = attrs.getOpt<std::string>(SUMO_ATTR_ALLOW, getID().c_str(), ok, "", false);
        const std::string disallow = attrs.getOpt<std::string>(SUMO_ATTR_DISALLOW, getID().c_str(), ok, "");
        myParsedRerouteInterval.permissions = parseVehicleClasses(allow, disallow);
    }

    if (element == SUMO_TAG_CLOSING_LANE_REROUTE) {
        // by closing lane
        std::string closed_id = attrs.getStringSecure(SUMO_ATTR_ID, "");
        MSLane* closed = MSLane::dictionary(closed_id);
        if (closed == nullptr) {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": Lane '" + closed_id + "' to close is not known.");
        }
        myParsedRerouteInterval.closedLanes.push_back(closed);
        bool ok;
        if (attrs.hasAttribute(SUMO_ATTR_ALLOW) || attrs.hasAttribute(SUMO_ATTR_DISALLOW)) {
            const std::string allow = attrs.getOpt<std::string>(SUMO_ATTR_ALLOW, getID().c_str(), ok, "", false);
            const std::string disallow = attrs.getOpt<std::string>(SUMO_ATTR_DISALLOW, getID().c_str(), ok, "");
            myParsedRerouteInterval.permissions = parseVehicleClasses(allow, disallow);
        } else {
            // lane closing only makes sense if the lane really receives reduced permissions
            myParsedRerouteInterval.permissions = SVC_AUTHORITY;
        }
    }

    if (element == SUMO_TAG_ROUTE_PROB_REROUTE) {
        // by explicit rerouting using routes
        // check if route exists
        std::string routeStr = attrs.getStringSecure(SUMO_ATTR_ID, "");
        if (routeStr == "") {
            throw ProcessError(TLF("MSTriggeredRerouter %: No route id given.", getID()));
        }
        ConstMSRoutePtr route = MSRoute::dictionary(routeStr);
        if (route == nullptr) {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": Route '" + routeStr + "' does not exist.");
        }

        // get the probability to reroute
        bool ok = true;
        double prob = attrs.getOpt<double>(SUMO_ATTR_PROB, getID().c_str(), ok, 1.);
        if (!ok) {
            throw ProcessError();
        }
        if (prob < 0) {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": Attribute 'probability' for route '" + routeStr + "' is negative (must not).");
        }
        // add
        myParsedRerouteInterval.routeProbs.add(route, prob);
    }

    if (element == SUMO_TAG_PARKING_AREA_REROUTE) {
        std::string parkingarea = attrs.getStringSecure(SUMO_ATTR_ID, "");
        if (parkingarea == "") {
            throw ProcessError(TLF("MSTriggeredRerouter %: No parking area id given.", getID()));
        }
        MSParkingArea* pa = static_cast<MSParkingArea*>(MSNet::getInstance()->getStoppingPlace(parkingarea, SUMO_TAG_PARKING_AREA));
        if (pa == nullptr) {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": Parking area '" + parkingarea + "' is not known.");
        }
        // get the probability to reroute
        bool ok = true;
        const double prob = attrs.getOpt<double>(SUMO_ATTR_PROB, getID().c_str(), ok, 1.);
        if (!ok) {
            throw ProcessError();
        }
        if (prob < 0) {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": Attribute 'probability' for destination '" + parkingarea + "' is negative (must not).");
        }
        const bool visible = attrs.getOpt<bool>(SUMO_ATTR_VISIBLE, getID().c_str(), ok, false);
        // add
        myParsedRerouteInterval.parkProbs.add(std::make_pair(pa, visible), prob);
        myHaveParkProbs = true;
    }

    if (element == SUMO_TAG_VIA_PROB_REROUTE) {
        // by giving probabilities of vias
        std::string viaID  = attrs.getStringSecure(SUMO_ATTR_ID, "");
        if (viaID == "") {
            throw ProcessError(TLF("MSTriggeredRerouter %: No via edge id given.", getID()));
        }
        MSEdge* const via = MSEdge::dictionary(viaID);
        if (via == nullptr) {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": Via edge '" + viaID + "' is not known.");
        }
        // get the probability to reroute
        bool ok = true;
        double prob = attrs.getOpt<double>(SUMO_ATTR_PROB, getID().c_str(), ok, 1.);
        if (!ok) {
            throw ProcessError();
        }
        if (prob < 0) {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": Attribute 'probability' for via '" + viaID + "' is negative (must not).");
        }
        // add
        myParsedRerouteInterval.edgeProbs.add(via, prob);
        myParsedRerouteInterval.isVia = true;
    }
}


void
MSTriggeredRerouter::myEndElement(int element) {
    if (element == SUMO_TAG_INTERVAL) {
        for (auto paVi : myParsedRerouteInterval.parkProbs.getVals()) {
            dynamic_cast<MSParkingArea*>(paVi.first)->setNumAlternatives((int)myParsedRerouteInterval.parkProbs.getVals().size() - 1);
        }
        if (myParsedRerouteInterval.closedLanes.size() > 0) {
            // collect edges that are affect by a closed lane
            std::set<MSEdge*> affected;
            for (const MSLane* const  l : myParsedRerouteInterval.closedLanes) {
                affected.insert(&l->getEdge());
            }
            myParsedRerouteInterval.closedLanesAffected.insert(myParsedRerouteInterval.closedLanesAffected.begin(), affected.begin(), affected.end());
        }
        const SUMOTime closingBegin = myParsedRerouteInterval.begin;
        const SUMOTime simBegin = string2time(OptionsCont::getOptions().getString("begin"));
        if (closingBegin < simBegin && myParsedRerouteInterval.end > simBegin) {
            // interval started before simulation begin but is still active at
            // the start of the simulation
            myParsedRerouteInterval.begin = simBegin;
        }
        myIntervals.push_back(myParsedRerouteInterval);
        myIntervals.back().id = (long long int)&myIntervals.back();
        if (!(myParsedRerouteInterval.closed.empty() && myParsedRerouteInterval.closedLanes.empty()) && myParsedRerouteInterval.permissions != SVCAll) {
            MSNet::getInstance()->getBeginOfTimestepEvents()->addEvent(
                new WrappingCommand<MSTriggeredRerouter>(this, &MSTriggeredRerouter::setPermissions), myParsedRerouteInterval.begin);
        }
    }
}


// ------------ loading end


SUMOTime
MSTriggeredRerouter::setPermissions(const SUMOTime currentTime) {
    bool updateVehicles = false;
    for (const RerouteInterval& i : myIntervals) {
        if (i.begin == currentTime && !(i.closed.empty() && i.closedLanes.empty()) && i.permissions != SVCAll) {
            for (MSEdge* const e : i.closed) {
                for (MSLane* lane : e->getLanes()) {
                    //std::cout << SIMTIME << " closing: intervalID=" << i.id << " lane=" << (*l)->getID() << " prevPerm=" << getVehicleClassNames((*l)->getPermissions()) << " new=" << getVehicleClassNames(i.permissions) << "\n";
                    lane->setPermissions(i.permissions, i.id);
                }
                e->rebuildAllowedLanes();
                updateVehicles = true;
            }
            for (MSLane* const lane : i.closedLanes) {
                lane->setPermissions(i.permissions, i.id);
                lane->getEdge().rebuildAllowedLanes();
                updateVehicles = true;
            }
            MSNet::getInstance()->getBeginOfTimestepEvents()->addEvent(
                new WrappingCommand<MSTriggeredRerouter>(this, &MSTriggeredRerouter::setPermissions), i.end);
        }
        if (i.end == currentTime && !(i.closed.empty() && i.closedLanes.empty()) && i.permissions != SVCAll) {
            for (MSEdge* const e : i.closed) {
                for (MSLane* lane : e->getLanes()) {
                    lane->resetPermissions(i.id);
                    //std::cout << SIMTIME << " opening: intervalID=" << i.id << " lane=" << (*l)->getID() << " restore prevPerm=" << getVehicleClassNames((*l)->getPermissions()) << "\n";
                }
                e->rebuildAllowedLanes();
                updateVehicles = true;
            }
            for (MSLane* lane : i.closedLanes) {
                lane->resetPermissions(i.id);
                lane->getEdge().rebuildAllowedLanes();
                updateVehicles = true;
            }
        }
    }
    if (updateVehicles) {
        // only vehicles on the affected lanes had their bestlanes updated so far
        for (MSEdge* e : myEdges) {
            // also updates vehicles
            e->rebuildAllowedTargets();
        }
    }
    return 0;
}


const MSTriggeredRerouter::RerouteInterval*
MSTriggeredRerouter::getCurrentReroute(SUMOTime time, SUMOTrafficObject& obj) const {
    for (const RerouteInterval& ri : myIntervals) {
        if (ri.begin <= time && ri.end > time) {
            if (
                // destProbReroute
                ri.edgeProbs.getOverallProb() > 0 ||
                // routeProbReroute
                ri.routeProbs.getOverallProb() > 0 ||
                // parkingZoneReroute
                ri.parkProbs.getOverallProb() > 0) {
                return &ri;
            }
            if (!ri.closed.empty() || !ri.closedLanesAffected.empty()) {
                const std::set<SUMOTrafficObject::NumericalID>& edgeIndices = obj.getUpcomingEdgeIDs();
                if (affected(edgeIndices, ri.closed) || affected(edgeIndices, ri.closedLanesAffected)) {
                    return &ri;
                }
            }
        }
    }
    return nullptr;
}


const MSTriggeredRerouter::RerouteInterval*
MSTriggeredRerouter::getCurrentReroute(SUMOTime time) const {
    for (const RerouteInterval& ri : myIntervals) {
        if (ri.begin <= time && ri.end > time) {
            if (ri.edgeProbs.getOverallProb() != 0 || ri.routeProbs.getOverallProb() != 0 || ri.parkProbs.getOverallProb() != 0
                    || !ri.closed.empty() || !ri.closedLanesAffected.empty()) {
                return &ri;
            }
        }
    }
    return nullptr;
}


bool
MSTriggeredRerouter::notifyEnter(SUMOTrafficObject& tObject, MSMoveReminder::Notification reason, const MSLane* /* enteredLane */) {
    if (myAmOptional) {
        return true;
    }
    return triggerRouting(tObject, reason);
}


bool
MSTriggeredRerouter::notifyMove(SUMOTrafficObject& veh, double /*oldPos*/,
                                double /*newPos*/, double /*newSpeed*/) {
    return triggerRouting(veh, NOTIFICATION_JUNCTION);
}


bool
MSTriggeredRerouter::notifyLeave(SUMOTrafficObject& /*veh*/, double /*lastPos*/,
                                 MSMoveReminder::Notification reason, const MSLane* /* enteredLane */) {
    return reason == NOTIFICATION_LANE_CHANGE;
}


bool
MSTriggeredRerouter::triggerRouting(SUMOTrafficObject& tObject, MSMoveReminder::Notification reason) {
    if (!applies(tObject)) {
        return false;
    }
    // check whether the vehicle shall be rerouted
    const SUMOTime now = MSNet::getInstance()->getCurrentTimeStep();
    const MSTriggeredRerouter::RerouteInterval* const rerouteDef = getCurrentReroute(now, tObject);
    if (rerouteDef == nullptr) {
        return true; // an active interval could appear later
    }
    const double prob = myAmInUserMode ? myUserProbability : myProbability;
    if (prob < 1 && RandHelper::rand(tObject.getRNG()) > prob) {
        return false; // XXX another interval could appear later but we would have to track whether the current interval was already tried
    }
    if (myTimeThreshold > 0 && MAX2(tObject.getWaitingTime(), tObject.getWaitingTime(true)) < myTimeThreshold) {
        return true; // waiting time may be reached later
    }
    if (reason == NOTIFICATION_LANE_CHANGE) {
        return false;
    }
    // if we have a closingLaneReroute, only vehicles with a rerouting device can profit from rerouting (otherwise, edge weights will not reflect local jamming)
    const bool hasReroutingDevice = tObject.getDevice(typeid(MSDevice_Routing)) != nullptr;
    if (rerouteDef->closedLanes.size() > 0 && !hasReroutingDevice) {
        return true; // an active interval could appear later
    }
    const MSEdge* lastEdge = tObject.getRerouteDestination();
#ifdef DEBUG_REROUTER
    if (DEBUGCOND) {
        std::cout << SIMTIME << " veh=" << veh.getID() << " check rerouter " << getID() << " lane=" << Named::getIDSecure(veh.getLane()) << " edge=" << veh.getEdge()->getID() << " finalEdge=" << lastEdge->getID() << " arrivalPos=" << veh.getArrivalPos() << "\n";
    }
#endif

    if (rerouteDef->parkProbs.getOverallProb() > 0) {
#ifdef HAVE_FOX
        ScopedLocker<> lock(myNotificationMutex, MSGlobals::gNumSimThreads > 1);
#endif
        if (!tObject.isVehicle()) {
            return false;
        }
        SUMOVehicle& veh = static_cast<SUMOVehicle&>(tObject);
        bool newDestination = false;
        ConstMSEdgeVector newRoute;
        MSParkingArea* newParkingArea = rerouteParkingArea(rerouteDef, veh, newDestination, newRoute);
        if (newParkingArea != nullptr) {
            // adapt plans of any riders
            for (MSTransportable* p : veh.getPersons()) {
                p->rerouteParkingArea(veh.getNextParkingArea(), newParkingArea);
            }

            if (newDestination) {
                // update arrival parameters
                SUMOVehicleParameter* newParameter = new SUMOVehicleParameter();
                *newParameter = veh.getParameter();
                newParameter->arrivalPosProcedure = ArrivalPosDefinition::GIVEN;
                newParameter->arrivalPos = newParkingArea->getEndLanePosition();
                veh.replaceParameter(newParameter);
            }

            SUMOAbstractRouter<MSEdge, SUMOVehicle>& router = hasReroutingDevice
                    ? MSRoutingEngine::getRouterTT(veh.getRNGIndex(), veh.getVClass(), rerouteDef->closed)
                    : MSNet::getInstance()->getRouterTT(veh.getRNGIndex(), rerouteDef->closed);
            const double routeCost = router.recomputeCosts(newRoute, &veh, MSNet::getInstance()->getCurrentTimeStep());
            ConstMSEdgeVector prevEdges(veh.getCurrentRouteEdge(), veh.getRoute().end());
            const double previousCost = router.recomputeCosts(prevEdges, &veh, MSNet::getInstance()->getCurrentTimeStep());
            const double savings = previousCost - routeCost;
            hasReroutingDevice
            ? MSRoutingEngine::getRouterTT(veh.getRNGIndex(), veh.getVClass())
            : MSNet::getInstance()->getRouterTT(veh.getRNGIndex()); // reset closed edges
            //if (getID() == "ego") std::cout << SIMTIME << " pCost=" << previousCost << " cost=" << routeCost
            //        << " prevEdges=" << toString(prevEdges)
            //        << " newEdges=" << toString(edges)
            //        << "\n";

            std::string errorMsg;
            if (veh.replaceParkingArea(newParkingArea, errorMsg)) {
                veh.replaceRouteEdges(newRoute, routeCost, savings, getID() + ":" + toString(SUMO_TAG_PARKING_AREA_REROUTE), false, false, false);
            } else {
                WRITE_WARNING("Vehicle '" + veh.getID() + "' at rerouter '" + getID()
                              + "' could not reroute to new parkingArea '" + newParkingArea->getID()
                              + "' reason=" + errorMsg + ", time=" + time2string(MSNet::getInstance()->getCurrentTimeStep()) + ".");
            }
        }
        return false;
    }

    // get rerouting params
    ConstMSRoutePtr newRoute = rerouteDef->routeProbs.getOverallProb() > 0 ? rerouteDef->routeProbs.get() : nullptr;
    // we will use the route if given rather than calling our own dijsktra...
    if (newRoute != nullptr) {
#ifdef DEBUG_REROUTER
        if (DEBUGCOND) {
            std::cout << "    replacedRoute from routeDist " << newRoute->getID() << "\n";
        }
#endif
        tObject.replaceRoute(newRoute, getID());
        return false; // XXX another interval could appear later but we would have to track whether the currenty interval was already used
    }
    const MSEdge* newEdge = lastEdge;
    // ok, try using a new destination
    double newArrivalPos = -1;
    const bool destUnreachable = std::find(rerouteDef->closed.begin(), rerouteDef->closed.end(), lastEdge) != rerouteDef->closed.end();
    bool keepDestination = false;
    // if we have a closingReroute, only assign new destinations to vehicles which cannot reach their original destination
    // if we have a closingLaneReroute, no new destinations should be assigned
    if (rerouteDef->closed.empty() || destUnreachable || rerouteDef->isVia) {
        newEdge = rerouteDef->edgeProbs.getOverallProb() > 0 ? rerouteDef->edgeProbs.get() : lastEdge;
        assert(newEdge != nullptr);
        if (newEdge == &mySpecialDest_terminateRoute) {
            keepDestination = true;
            newEdge = tObject.getEdge();
            newArrivalPos = tObject.getPositionOnLane(); // instant arrival
        } else if (newEdge == &mySpecialDest_keepDestination || newEdge == lastEdge) {
            if (destUnreachable && rerouteDef->permissions == SVCAll) {
                // if permissions aren't set vehicles will simply drive through
                // the closing unless terminated. If the permissions are specified, assume that the user wants
                // vehicles to stand and wait until the closing ends
                WRITE_WARNINGF(TL("Cannot keep destination edge '%' for vehicle '%' due to closed edges. Terminating route."), lastEdge->getID(), tObject.getID());
                newEdge = tObject.getEdge();
            } else {
                newEdge = lastEdge;
            }
        }
    }
    ConstMSEdgeVector edges;
    std::vector<MSTransportableRouter::TripItem> items;
    // we have a new destination, let's replace the route (if it is affected)
    if (rerouteDef->closed.empty() || destUnreachable || rerouteDef->isVia || affected(tObject.getUpcomingEdgeIDs(), rerouteDef->closed)) {
        if (tObject.isVehicle()) {
            SUMOVehicle& veh = static_cast<SUMOVehicle&>(tObject);
            MSVehicleRouter& router = hasReroutingDevice
                                      ? MSRoutingEngine::getRouterTT(veh.getRNGIndex(), veh.getVClass(), rerouteDef->closed)
                                      : MSNet::getInstance()->getRouterTT(veh.getRNGIndex(), rerouteDef->closed);
            router.compute(veh.getEdge(), newEdge, &veh, now, edges);
            if (edges.size() == 0 && !keepDestination && rerouteDef->edgeProbs.getOverallProb() > 0) {
                // destination unreachable due to closed intermediate edges. pick among alternative targets
                RandomDistributor<MSEdge*> edgeProbs2 = rerouteDef->edgeProbs;
                edgeProbs2.remove(const_cast<MSEdge*>(newEdge));
                while (edges.size() == 0 && edgeProbs2.getVals().size() > 0) {
                    newEdge = edgeProbs2.get();
                    edgeProbs2.remove(const_cast<MSEdge*>(newEdge));
                    if (newEdge == &mySpecialDest_terminateRoute) {
                        newEdge = veh.getEdge();
                        newArrivalPos = veh.getPositionOnLane(); // instant arrival
                    }
                    if (newEdge == &mySpecialDest_keepDestination && rerouteDef->permissions != SVCAll) {
                        newEdge = lastEdge;
                        break;
                    }
                    router.compute(veh.getEdge(), newEdge, &veh, now, edges);
                }

            }
            if (!rerouteDef->isVia) {
                const double routeCost = router.recomputeCosts(edges, &veh, now);
                hasReroutingDevice
                ? MSRoutingEngine::getRouterTT(veh.getRNGIndex(), veh.getVClass())
                : MSNet::getInstance()->getRouterTT(veh.getRNGIndex()); // reset closed edges
                const bool useNewRoute = veh.replaceRouteEdges(edges, routeCost, 0, getID());
#ifdef DEBUG_REROUTER
                if (DEBUGCOND) std::cout << "   rerouting:  newDest=" << newEdge->getID()
                                             << " newEdges=" << toString(edges)
                                             << " useNewRoute=" << useNewRoute << " newArrivalPos=" << newArrivalPos << " numClosed=" << rerouteDef->closed.size()
                                             << " destUnreachable=" << destUnreachable << " containsClosed=" << veh.getRoute().containsAnyOf(rerouteDef->closed) << "\n";
#endif
                if (useNewRoute && newArrivalPos != -1) {
                    // must be called here because replaceRouteEdges may also set the arrivalPos
                    veh.setArrivalPos(newArrivalPos);
                }

            }
        } else {
            // person rerouting here
            MSTransportableRouter& router = hasReroutingDevice
                                            ? MSRoutingEngine::getIntermodalRouterTT(tObject.getRNGIndex(), rerouteDef->closed)
                                            : MSNet::getInstance()->getIntermodalRouter(tObject.getRNGIndex(), 0, rerouteDef->closed);
            const bool success = router.compute(tObject.getEdge(), newEdge, tObject.getPositionOnLane(), "",
                                                rerouteDef->isVia ? newEdge->getLength() / 2. : tObject.getParameter().arrivalPos, "",
                                                tObject.getMaxSpeed(), nullptr, 0, now, items);
            if (!rerouteDef->isVia) {
                if (success) {
                    for (const MSTransportableRouter::TripItem& it : items) {
                        if (!it.edges.empty() && !edges.empty() && edges.back() == it.edges.front()) {
                            edges.pop_back();
                        }
                        edges.insert(edges.end(), std::make_move_iterator(it.edges.begin()), std::make_move_iterator(it.edges.end()));
                        if (!edges.empty()) {
                            static_cast<MSPerson&>(tObject).replaceWalk(edges, tObject.getPositionOnLane(), 0, 1);
                        }
                    }
                } else {
                    // maybe the pedestrian model still finds a way (JuPedSim)
                    static_cast<MSPerson&>(tObject).replaceWalk({tObject.getEdge(), newEdge}, tObject.getPositionOnLane(), 0, 1);
                }
            }
        }
    }
    // it was only a via so calculate the remaining part
    if (rerouteDef->isVia) {
        if (tObject.isVehicle()) {
            SUMOVehicle& veh = static_cast<SUMOVehicle&>(tObject);
            if (!edges.empty()) {
                edges.pop_back();
            }
            MSVehicleRouter& router = hasReroutingDevice
                                      ? MSRoutingEngine::getRouterTT(veh.getRNGIndex(), veh.getVClass(), rerouteDef->closed)
                                      : MSNet::getInstance()->getRouterTT(veh.getRNGIndex(), rerouteDef->closed);
            router.compute(newEdge, lastEdge, &veh, now, edges);
            const double routeCost = router.recomputeCosts(edges, &veh, now);
            hasReroutingDevice
            ? MSRoutingEngine::getRouterTT(veh.getRNGIndex(), veh.getVClass())
            : MSNet::getInstance()->getRouterTT(veh.getRNGIndex()); // reset closed edges
            const bool useNewRoute = veh.replaceRouteEdges(edges, routeCost, 0, getID());
#ifdef DEBUG_REROUTER
            if (DEBUGCOND) std::cout << "   rerouting:  newDest=" << newEdge->getID()
                                         << " newEdges=" << toString(edges)
                                         << " useNewRoute=" << useNewRoute << " newArrivalPos=" << newArrivalPos << " numClosed=" << rerouteDef->closed.size()
                                         << " destUnreachable=" << destUnreachable << " containsClosed=" << veh.getRoute().containsAnyOf(rerouteDef->closed) << "\n";
#endif
            if (useNewRoute && newArrivalPos != -1) {
                // must be called here because replaceRouteEdges may also set the arrivalPos
                veh.setArrivalPos(newArrivalPos);
            }
        } else {
            // person rerouting here
            bool success = !items.empty();
            if (success) {
                MSTransportableRouter& router = hasReroutingDevice
                                                ? MSRoutingEngine::getIntermodalRouterTT(tObject.getRNGIndex(), rerouteDef->closed)
                                                : MSNet::getInstance()->getIntermodalRouter(tObject.getRNGIndex(), 0, rerouteDef->closed);
                success = router.compute(newEdge, lastEdge, newEdge->getLength() / 2., "",
                                         tObject.getParameter().arrivalPos, "",
                                         tObject.getMaxSpeed(), nullptr, 0, now, items);
            }
            if (success) {
                for (const MSTransportableRouter::TripItem& it : items) {
                    if (!it.edges.empty() && !edges.empty() && edges.back() == it.edges.front()) {
                        edges.pop_back();
                    }
                    edges.insert(edges.end(), std::make_move_iterator(it.edges.begin()), std::make_move_iterator(it.edges.end()));
                }
                if (!edges.empty()) {
                    static_cast<MSPerson&>(tObject).replaceWalk(edges, tObject.getPositionOnLane(), 0, 1);
                }
            } else {
                // maybe the pedestrian model still finds a way (JuPedSim)
                static_cast<MSPerson&>(tObject).replaceWalk({tObject.getEdge(), newEdge, lastEdge}, tObject.getPositionOnLane(), 0, 1);
            }
        }
    }
    return false; // XXX another interval could appear later but we would have to track whether the currenty interval was already used
}


void
MSTriggeredRerouter::setUserMode(bool val) {
    myAmInUserMode = val;
}


void
MSTriggeredRerouter::setUserUsageProbability(double prob) {
    myUserProbability = prob;
}


bool
MSTriggeredRerouter::inUserMode() const {
    return myAmInUserMode;
}


double
MSTriggeredRerouter::getProbability() const {
    return myAmInUserMode ? myUserProbability : myProbability;
}


double
MSTriggeredRerouter::getUserProbability() const {
    return myUserProbability;
}


double
MSTriggeredRerouter::getStoppingPlaceOccupancy(MSStoppingPlace* parkingArea) {
    return dynamic_cast<MSParkingArea*>(parkingArea)->getOccupancy();
}


double
MSTriggeredRerouter::getStoppingPlaceCapacity(MSStoppingPlace* stoppingPlace) {
    MSParkingArea* pa = dynamic_cast<MSParkingArea*>(stoppingPlace);
    return pa->getCapacity();
}


void
MSTriggeredRerouter::rememberBlockedStoppingPlace(SUMOVehicle& veh, const MSStoppingPlace* stoppingPlace, bool blocked) {
    veh.rememberBlockedParkingArea(stoppingPlace, blocked);
}


void
MSTriggeredRerouter::rememberStoppingPlaceScore(SUMOVehicle& veh, MSStoppingPlace* place, const std::string& score) {
    veh.rememberParkingAreaScore(place, score);
}


void
MSTriggeredRerouter::resetStoppingPlaceScores(SUMOVehicle& veh) {
    veh.resetParkingAreaScores();
}


SUMOTime
MSTriggeredRerouter::sawBlockedStoppingPlace(SUMOVehicle& veh, MSStoppingPlace* place, bool local) {
    return veh.sawBlockedParkingArea(place, local);
}


int
MSTriggeredRerouter::getNumberStoppingPlaceReroutes(SUMOVehicle& veh) {
    return veh.getNumberParkingReroutes();
}


void
MSTriggeredRerouter::setNumberStoppingPlaceReroutes(SUMOVehicle& veh, int value) {
    veh.setNumberParkingReroutes(value);
}


MSParkingArea*
MSTriggeredRerouter::rerouteParkingArea(const MSTriggeredRerouter::RerouteInterval* rerouteDef,
                                        SUMOVehicle& veh, bool& newDestination, ConstMSEdgeVector& newRoute) {
    std::vector<StoppingPlaceVisible> parks = rerouteDef->parkProbs.getVals();
    return dynamic_cast<MSParkingArea*>(reroute(parks, rerouteDef->parkProbs.getProbs(), veh, newDestination, newRoute, rerouteDef->closed));
}


bool
MSTriggeredRerouter::applies(const SUMOTrafficObject& obj) const {
    if (myVehicleTypes.empty() || myVehicleTypes.count(obj.getVehicleType().getOriginalID()) > 0) {
        return true;
    } else {
        std::set<std::string> vTypeDists = MSNet::getInstance()->getVehicleControl().getVTypeDistributionMembership(obj.getVehicleType().getOriginalID());
        for (auto vTypeDist : vTypeDists) {
            if (myVehicleTypes.count(vTypeDist) > 0) {
                return true;
            }
        }
        return false;
    }
}


bool
MSTriggeredRerouter::affected(const std::set<SUMOTrafficObject::NumericalID>& edgeIndices, const MSEdgeVector& closed) {
    for (const MSEdge* const e : closed) {
        if (edgeIndices.count(e->getNumericalID()) > 0) {
            return true;
        }
    }
    return false;
}


void
MSTriggeredRerouter::checkParkingRerouteConsistency() {
    // if a parkingArea is a rerouting target, it should generally have a
    // rerouter on its edge or vehicles will be stuck there once it's full.
    // The user should receive a Warning in this case
    std::set<MSEdge*> parkingRerouterEdges;
    std::map<MSParkingArea*, std::string, ComparatorIdLess> targetedParkingArea; // paID -> targetingRerouter
    for (const auto& rr : myInstances) {
        bool hasParkingReroute = false;
        for (const RerouteInterval& interval : rr.second->myIntervals) {
            if (interval.parkProbs.getOverallProb() > 0) {
                hasParkingReroute = true;
                for (const StoppingPlaceVisible& pav : interval.parkProbs.getVals()) {
                    targetedParkingArea[dynamic_cast<MSParkingArea*>(pav.first)] = rr.first;
                }
            }
        }
        if (hasParkingReroute) {
            parkingRerouterEdges.insert(rr.second->myEdges.begin(), rr.second->myEdges.end());
        }
    }
    for (const auto& item : targetedParkingArea) {
        if (parkingRerouterEdges.count(&item.first->getLane().getEdge()) == 0) {
            WRITE_WARNINGF(TL("ParkingArea '%' is targeted by rerouter '%' but doesn't have its own rerouter. This may cause parking search to abort."),
                           item.first->getID(), item.second);
        }
    }
}


/****************************************************************************/
