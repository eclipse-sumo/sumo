/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @date    Mon, 25 July 2005
///
// Reroutes vehicles passing an edge
/****************************************************************************/
#include <config.h>

#include <string>
#include <algorithm>
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
#include <microsim/transportables/MSTransportable.h>
#include <microsim/devices/MSDevice_Routing.h>
#include <microsim/devices/MSRoutingEngine.h>
#include "MSTriggeredRerouter.h"

#include <mesosim/MELoop.h>
#include <mesosim/MESegment.h>

//#define DEBUG_REROUTER
//#define DEBUG_PARKING
#define DEBUGCOND (veh.isSelected())
//#define DEBUGCOND (true)
//#define DEBUGCOND (veh.getID() == "")

// ===========================================================================
// static member defintion
// ===========================================================================
MSEdge MSTriggeredRerouter::mySpecialDest_keepDestination("MSTriggeredRerouter_keepDestination", -1, SumoXMLEdgeFunc::UNKNOWN, "", "", -1, 0);
MSEdge MSTriggeredRerouter::mySpecialDest_terminateRoute("MSTriggeredRerouter_terminateRoute", -1, SumoXMLEdgeFunc::UNKNOWN, "", "", -1, 0);
std::map<std::string, MSTriggeredRerouter*> MSTriggeredRerouter::myInstances;

// ===========================================================================
// method definitions
// ===========================================================================
MSTriggeredRerouter::MSTriggeredRerouter(const std::string& id,
        const MSEdgeVector& edges,
        double prob, const std::string& file, bool off,
        SUMOTime timeThreshold,
        const std::string& vTypes) :
    MSTrigger(id),
    MSMoveReminder(id),
    SUMOSAXHandler(file),
    myEdges(edges),
    myProbability(prob),
    myUserProbability(prob),
    myAmInUserMode(false),
    myTimeThreshold(timeThreshold) {
    myInstances[id] = this;
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
        myCurrentIntervalBegin = attrs.getOptSUMOTimeReporting(SUMO_ATTR_BEGIN, nullptr, ok, -1);
        myCurrentIntervalEnd = attrs.getOptSUMOTimeReporting(SUMO_ATTR_END, nullptr, ok, SUMOTime_MAX);
    }
    if (element == SUMO_TAG_DEST_PROB_REROUTE) {
        // by giving probabilities of new destinations
        // get the destination edge
        std::string dest = attrs.getStringSecure(SUMO_ATTR_ID, "");
        if (dest == "") {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": No destination edge id given.");
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
        myCurrentEdgeProb.add(to, prob);
    }


    if (element == SUMO_TAG_CLOSING_REROUTE) {
        // by closing
        std::string closed_id = attrs.getStringSecure(SUMO_ATTR_ID, "");
        MSEdge* closed = MSEdge::dictionary(closed_id);
        if (closed == nullptr) {
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
        if (closed == nullptr) {
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
        myCurrentRouteProb.add(route, prob);
    }

    if (element == SUMO_TAG_PARKING_AREA_REROUTE) {
        // by giving probabilities of new destinations
        // get the destination edge
        std::string parkingarea = attrs.getStringSecure(SUMO_ATTR_ID, "");
        if (parkingarea == "") {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": No parking area id given.");
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
        myCurrentParkProb.add(std::make_pair(pa, visible), prob);
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
        for (auto paVi : ri.parkProbs.getVals()) {
            paVi.first->setNumAlternatives((int)ri.parkProbs.getVals().size() - 1);
        }
        if (ri.closedLanes.size() > 0) {
            // collect edges that are affect by a closed lane
            std::set<MSEdge*> affected;
            for (const MSLane* const  l : ri.closedLanes) {
                affected.insert(&l->getEdge());
            }
            ri.closedLanesAffected.insert(ri.closedLanesAffected.begin(), affected.begin(), affected.end());
        }
        SUMOTime closingBegin = ri.begin;
        SUMOTime simBegin = string2time(OptionsCont::getOptions().getString("begin"));
        if (closingBegin < simBegin && ri.end > simBegin) {
            // interval started before simulation begin but is still active at
            // the start of the simulation
            ri.begin = simBegin;
        }
        myCurrentClosed.clear();
        myCurrentClosedLanes.clear();
        myCurrentEdgeProb.clear();
        myCurrentRouteProb.clear();
        myCurrentParkProb.clear();
        myIntervals.push_back(ri);
        myIntervals.back().id = (long long)&myIntervals.back();
        if (!(ri.closed.empty() && ri.closedLanes.empty()) && ri.permissions != SVCAll) {
            MSNet::getInstance()->getBeginOfTimestepEvents()->addEvent(
                new WrappingCommand<MSTriggeredRerouter>(this, &MSTriggeredRerouter::setPermissions), ri.begin);
        }
    }
}


// ------------ loading end


SUMOTime
MSTriggeredRerouter::setPermissions(const SUMOTime currentTime) {
    bool updateVehicles = false;
    for (const RerouteInterval& i : myIntervals) {
        if (i.begin == currentTime && !(i.closed.empty() && i.closedLanes.empty()) && i.permissions != SVCAll) {
            for (MSEdge* e : i.closed) {
                for (MSLane* lane : e->getLanes()) {
                    //std::cout << SIMTIME << " closing: intervalID=" << i.id << " lane=" << (*l)->getID() << " prevPerm=" << getVehicleClassNames((*l)->getPermissions()) << " new=" << getVehicleClassNames(i.permissions) << "\n";
                    lane->setPermissions(i.permissions, i.id);
                }
                e->rebuildAllowedLanes();
                updateVehicles = true;
            }
            for (MSLane* lane : i.closedLanes) {
                lane->setPermissions(i.permissions, i.id);
                lane->getEdge().rebuildAllowedLanes();
                updateVehicles = true;
            }
            MSNet::getInstance()->getBeginOfTimestepEvents()->addEvent(
                new WrappingCommand<MSTriggeredRerouter>(this, &MSTriggeredRerouter::setPermissions), i.end);
        }
        if (i.end == currentTime && !(i.closed.empty() && i.closedLanes.empty()) && i.permissions != SVCAll) {
            for (MSEdge* e : i.closed) {
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
    return nullptr;
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
    return nullptr;
}


bool
MSTriggeredRerouter::notifyMove(SUMOTrafficObject& veh, double /*oldPos*/,
                                double /*newPos*/, double /*newSpeed*/) {
    return notifyEnter(veh, NOTIFICATION_JUNCTION);
}


bool
MSTriggeredRerouter::notifyLeave(SUMOTrafficObject& /*veh*/, double /*lastPos*/,
                                 MSMoveReminder::Notification reason, const MSLane* /* enteredLane */) {
    return reason == NOTIFICATION_LANE_CHANGE;
}


bool
MSTriggeredRerouter::notifyEnter(SUMOTrafficObject& tObject, MSMoveReminder::Notification /*reason*/, const MSLane* /* enteredLane */) {
    if (!tObject.isVehicle()) {
        return false;
    }
    SUMOVehicle& veh = static_cast<SUMOVehicle&>(tObject);
    if (!vehicleApplies(veh)) {
        return false;
    }
    // check whether the vehicle shall be rerouted
    const SUMOTime time = MSNet::getInstance()->getCurrentTimeStep();
    const MSTriggeredRerouter::RerouteInterval* rerouteDef = getCurrentReroute(time, veh);
    if (rerouteDef == nullptr) {
        return true; // an active interval could appear later
    }
    const double prob = myAmInUserMode ? myUserProbability : myProbability;
    if (prob < 1 && RandHelper::rand(veh.getRNG()) > prob) {
        return false; // XXX another interval could appear later but we would have to track whether the current interval was already tried
    }
    if (myTimeThreshold > 0 && MAX2(veh.getWaitingTime(), veh.getAccumulatedWaitingTime()) < myTimeThreshold) {
        return true; // waiting time may be reached later
    }
    // if we have a closingLaneReroute, only vehicles with a rerouting device can profit from rerouting (otherwise, edge weights will not reflect local jamming)
    const bool hasReroutingDevice = veh.getDevice(typeid(MSDevice_Routing)) != nullptr;
    if (rerouteDef->closedLanes.size() > 0 && !hasReroutingDevice) {
        return true; // an active interval could appear later
    }
    // get vehicle params
    const MSRoute& route = veh.getRoute();
    const MSEdge* lastEdge = route.getLastEdge();
#ifdef DEBUG_REROUTER
    if (DEBUGCOND) {
        std::cout << SIMTIME << " veh=" << veh.getID() << " check rerouter " << getID() << " lane=" << Named::getIDSecure(veh.getLane()) << " edge=" << veh.getEdge()->getID() << " finalEdge=" << lastEdge->getID() << " arrivalPos=" << veh.getArrivalPos() << "\n";
    }
#endif

    if (rerouteDef->parkProbs.getOverallProb() > 0) {
#ifdef HAVE_FOX
        FXConditionalLock lock(myNotificationMutex, MSGlobals::gNumSimThreads > 1);
#endif
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
    const MSRoute* newRoute = rerouteDef->routeProbs.getOverallProb() > 0 ? rerouteDef->routeProbs.get() : 0;
    // we will use the route if given rather than calling our own dijsktra...
    if (newRoute != nullptr) {
#ifdef DEBUG_REROUTER
        if (DEBUGCOND) {
            std::cout << "    replacedRoute from routeDist " << newRoute->getID() << "\n";
        }
#endif
        veh.replaceRoute(newRoute, getID());
        return false; // XXX another interval could appear later but we would have to track whether the currenty interval was already used
    }
    const MSEdge* newEdge = lastEdge;
    // ok, try using a new destination
    double newArrivalPos = -1;
    const bool destUnreachable = std::find(rerouteDef->closed.begin(), rerouteDef->closed.end(), lastEdge) != rerouteDef->closed.end();
    bool keepDestination = false;
    // if we have a closingReroute, only assign new destinations to vehicles which cannot reach their original destination
    // if we have a closingLaneReroute, no new destinations should be assigned
    if (rerouteDef->closed.size() == 0 || destUnreachable) {
        newEdge = rerouteDef->edgeProbs.getOverallProb() > 0 ? rerouteDef->edgeProbs.get() : route.getLastEdge();
        if (newEdge == &mySpecialDest_terminateRoute) {
            keepDestination = true;
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
        } else if (newEdge == nullptr) {
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
                ? MSRoutingEngine::getRouterTT(veh.getRNGIndex(), veh.getVClass(), rerouteDef->closed)
                : MSNet::getInstance()->getRouterTT(veh.getRNGIndex(), rerouteDef->closed);
        router.compute(veh.getEdge(), newEdge, &veh, MSNet::getInstance()->getCurrentTimeStep(), edges);
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
                router.compute(
                    veh.getEdge(), newEdge, &veh, MSNet::getInstance()->getCurrentTimeStep(), edges);
            }

        }
        const double routeCost = router.recomputeCosts(edges, &veh, MSNet::getInstance()->getCurrentTimeStep());
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
MSTriggeredRerouter::getWeight(SUMOVehicle& veh, const std::string param, const double defaultWeight) const {
    // get custom vehicle parameter
    if (veh.getParameter().knowsParameter(param)) {
        try {
            return StringUtils::toDouble(veh.getParameter().getParameter(param, "-1"));
        } catch (...) {
            WRITE_WARNING("Invalid value '" + veh.getParameter().getParameter(param, "-1") + "' for vehicle parameter '" + param + "'");
        }
    } else {
        // get custom vType parameter
        if (veh.getVehicleType().getParameter().knowsParameter(param)) {
            try {
                return StringUtils::toDouble(veh.getVehicleType().getParameter().getParameter(param, "-1"));
            } catch (...) {
                WRITE_WARNING("Invalid value '" + veh.getVehicleType().getParameter().getParameter(param, "-1") + "' for vType parameter '" + param + "'");
            }
        }
    }
    //WRITE_MESSAGE("Vehicle '" +veh.getID() + "' does not supply vehicle parameter '" + param + "'. Using default of " + toString(defaultWeight) + "\n";
    return defaultWeight;
}


MSParkingArea*
MSTriggeredRerouter::rerouteParkingArea(const MSTriggeredRerouter::RerouteInterval* rerouteDef,
                                        SUMOVehicle& veh, bool& newDestination, ConstMSEdgeVector& newRoute) const {
    // Reroute destination from initial parking area to an alternative parking area
    // if the following conditions are met:
    // - next stop target is a parking area
    // - target is included in the current alternative set
    // - target is visibly full
    // Any parking areas that are visibly full at the current location are
    // committed to parking memory

    MSParkingArea* nearParkArea = nullptr;
    std::vector<ParkingAreaVisible> parks = rerouteDef->parkProbs.getVals();

    // get vehicle params
    MSParkingArea* destParkArea = veh.getNextParkingArea();
    const MSRoute& route = veh.getRoute();

    if (destParkArea == nullptr) {
        // not driving towards a parkingArea
        return nullptr;
    }

    bool destVisible = false;
    for (auto paVis : parks) {
        if (paVis.first == destParkArea
                && (paVis.second
                    // if the vehicle is on the destParkArea edge it is always visible
                    || &(destParkArea->getLane().getEdge()) == veh.getEdge())) {
            destVisible = true;
            break;
        }
    }

    MSParkingArea* onTheWay = nullptr;

    if (!destVisible) {
        // check whether we are ready to accept any free parkingArea along the
        // way to our destination
        const int parkAnywhere = getWeight(veh, "parking.anywhere", -1);
        if (parkAnywhere < 0 || parkAnywhere > veh.getNumberParkingReroutes()) {
            // cannot determine destination occupancy, only register visibly full
            for (const ParkingAreaVisible& pav : parks) {
                if (pav.second && pav.first->getLastStepOccupancy() == pav.first->getCapacity()) {
                    veh.rememberBlockedParkingArea(pav.first, &pav.first->getLane().getEdge() == veh.getEdge());
                }
            }
#ifdef DEBUG_PARKING
            if (DEBUGCOND) {
                //std::cout << SIMTIME << " rerouter=" << getID() << " veh=" << veh.getID()
                //    << " dest=" << destParkArea->getID() << " parkAnywhere=" << parkAnywhere << " parkingReroutes=" << veh.getNumberParkingReroutes() << " stay on original route\n";
            }
#endif
            return nullptr;
        }

        double bestDist = std::numeric_limits<double>::max();
        const double brakeGap = veh.getBrakeGap();
        for (ParkingAreaVisible& item : parks) {
            if (item.second) {
                MSParkingArea* pa = item.first;
                if (&pa->getLane().getEdge() == veh.getEdge()
                        && pa->getLastStepOccupancy() < pa->getCapacity()) {
                    const double distToStart = pa->getBeginLanePosition() - veh.getPositionOnLane();
                    const double distToEnd = pa->getEndLanePosition() - veh.getPositionOnLane();
                    if (distToEnd > brakeGap) {
                        veh.rememberParkingAreaScore(pa, "dist=" + toString(distToStart));
                        if (distToStart < bestDist) {
                            bestDist = distToStart;
                            onTheWay = pa;
                        }
                    } else {
                        veh.rememberParkingAreaScore(pa, "tooClose");
                    }
                }
            }
        }
#ifdef DEBUG_PARKING
        if (DEBUGCOND) {
            std::cout << SIMTIME << " rerouter=" << getID() << " veh=" << veh.getID()
                << " dest=" << destParkArea->getID() << " parkAnywhere=" << parkAnywhere << " parkingReroutes=" << veh.getNumberParkingReroutes() << " alongTheWay=" << Named::getIDSecure(onTheWay) << "\n";
        }
#endif
        if (onTheWay == nullptr) {
            return nullptr;
        }
        // otherwise compute new route
    }

    if (destParkArea->getLastStepOccupancy() == destParkArea->getCapacity() || onTheWay != nullptr) {

        // if the current route ends at the parking area, the new route will
        // also and at the new area
        newDestination = (&destParkArea->getLane().getEdge() == route.getLastEdge()
                          && veh.getArrivalPos() >= destParkArea->getBeginLanePosition()
                          && veh.getArrivalPos() <= destParkArea->getEndLanePosition());

#ifdef DEBUG_PARKING
        if (DEBUGCOND) {
            std::cout << SIMTIME << " rerouter=" << getID() << " veh=" << veh.getID()
                      << " rerouteParkingArea dest=" << destParkArea->getID()
                      << " onDestEdge=" << (&(destParkArea->getLane().getEdge()) == veh.getEdge())
                      << " newDest=" << newDestination
                      << "\n";
        }
#endif

        ParkingParamMap_t weights;
        std::map<MSParkingArea*, ConstMSEdgeVector> newRoutes;
        std::map<MSParkingArea*, ConstMSEdgeVector> parkApproaches;

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

        SUMOAbstractRouter<MSEdge, SUMOVehicle>& router = MSNet::getInstance()->getRouterTT(veh.getRNGIndex(), rerouteDef->closed);

        const std::vector<double>& probs = rerouteDef->parkProbs.getProbs();

        const double brakeGap = veh.getBrakeGap();

        if (onTheWay != nullptr) {
            // compute new route
            if (newDestination) {
                newRoute.push_back(veh.getEdge());
            } else {
                bool valid = addParkValues(veh, brakeGap, newDestination, onTheWay, onTheWay->getLastStepOccupancy(), 1, router, parkAreas, newRoutes, parkApproaches, maxValues);
                if (!valid) {
                    WRITE_WARNING("Parkingarea '" + onTheWay->getID() + "' along the way cannot be used by vehicle '" + veh.getID() + "' for unknown reason");
                    return nullptr;
                }
                newRoute = newRoutes[onTheWay];
            }
            return onTheWay;
        }

        int numAlternatives = 0;
        std::vector<std::tuple<SUMOTime, MSParkingArea*, int> > blockedTimes;
        veh.resetParkingAreaScores();
        veh.rememberParkingAreaScore(destParkArea, "occupied");
        veh.rememberBlockedParkingArea(destParkArea, &destParkArea->getLane().getEdge() == veh.getEdge());

        const SUMOTime parkingMemory = TIME2STEPS(getWeight(veh, "parking.memory", 600));
        const double parkingFrustration = getWeight(veh, "parking.frustration", 100);
        const double parkingKnowledge = getWeight(veh, "parking.knowledge", 0);

        for (int i = 0; i < (int)parks.size(); ++i) {
            MSParkingArea* pa = parks[i].first;
            // alternative occupancy is randomized (but never full) if invisible
            // current destination must be visible at this point
            const bool visible = parks[i].second || (pa == destParkArea && destVisible);
            double paOccupancy = pa->getOccupancy();
            if (!visible && (parkingKnowledge == 0 || parkingKnowledge < RandHelper::rand(veh.getRNG()))) {
                const double minOccupancy = MIN2((double)pa->getCapacity() - NUMERICAL_EPS, (veh.getNumberParkingReroutes() * pa->getCapacity() / parkingFrustration));
                paOccupancy = RandHelper::rand(minOccupancy, (double)pa->getCapacity());
                // previously visited?
                SUMOTime blockedTime = veh.sawBlockedParkingArea(pa, false);
                if (blockedTime >= 0 && SIMSTEP - blockedTime < parkingMemory) {
                    // assume it's still occupied
                    paOccupancy = pa->getCapacity();
                    blockedTimes.push_back(std::make_tuple(blockedTime, pa, i));
#ifdef DEBUG_PARKING
                    if (DEBUGCOND) {
                        std::cout << "    altPA=" << pa->getID() << " was blocked at " << time2string(blockedTime) << "\n";
                    }
#endif
                }
            }
            if (paOccupancy < pa->getCapacity()) {
                if (addParkValues(veh, brakeGap, newDestination, pa, paOccupancy, probs[i], router, parkAreas, newRoutes, parkApproaches, maxValues)) {
                    numAlternatives++;
                };
            } else if (visible) {
                // might only be visible now (i.e. because it's on the other
                // side of the street), so we should remember this for later.
                veh.rememberBlockedParkingArea(pa, &pa->getLane().getEdge() == veh.getEdge());
            }
        }
        if (numAlternatives == 0) {
            // use parkingArea with lowest blockedTime
            std::sort(blockedTimes.begin(), blockedTimes.end());
            for (auto item : blockedTimes) {
                MSParkingArea* pa = std::get<1>(item);
                double prob = probs[std::get<2>(item)];
                // all parking areas are occupied. We have no good basis for
                // prefering one or the other based on estimated occupancy
                double paOccupancy = RandHelper::rand((double)pa->getCapacity());
                if (addParkValues(veh, brakeGap, newDestination, pa, paOccupancy, prob, router, parkAreas, newRoutes, parkApproaches, maxValues)) {
                    numAlternatives = 1;
                    break;
                }
                //std::cout << "  candidate=" << item.second->getID() << " observed=" << time2string(item.first) << "\n";
            }
            if (numAlternatives == 0) {
                // take any random target but prefer that that haven't been visited yet
                std::vector<std::pair<SUMOTime, MSParkingArea*> > candidates;
                for (const ParkingAreaVisible& pav : parks) {
                    if (pav.first == destParkArea) {
                        continue;
                    }
                    SUMOTime dummy = veh.sawBlockedParkingArea(pav.first, true);
                    if (dummy < 0) {
                        // randomomize among the unvisited
                        dummy = TIME2STEPS(RandHelper::rand() * -1000);
                    }
                    candidates.push_back(std::make_pair(dummy, pav.first));
                }
                std::sort(candidates.begin(), candidates.end());
                for (auto item : candidates) {
                    MSParkingArea* pa = item.second;
                    if (addParkValues(veh, brakeGap, newDestination, pa, 0, 1, router, parkAreas, newRoutes, parkApproaches, maxValues)) {
                        numAlternatives = 1;
                        break;
                    }
                }
            }
        }

        MSNet::getInstance()->getRouterTT(veh.getRNGIndex()); // reset closed edges

#ifdef DEBUG_PARKING
        if (DEBUGCOND) {
            std::cout << "  maxValues=" << joinToString(maxValues, " ", ":") << "\n";
        }
#endif

        // minimum cost to get the parking area
        double minParkingCost = 0.0;

        for (MSParkingAreaMap_t::iterator it = parkAreas.begin(); it != parkAreas.end(); ++it) {
            // get the parking values
            ParkingParamMap_t parkValues = it->second;

            if (weights["probability"] > 0 && maxValues["probability"] > 0.0) {
                // random search should not drive past a usable parking area
                bool dominated = false;
                double endPos = it->first->getEndLanePosition();
                const ConstMSEdgeVector& to1 = parkApproaches[it->first];
                assert(to1.size() > 0);
                for (auto altPa : parkAreas) {
                    if (altPa.first == it->first) {
                        continue;
                    }
                    const ConstMSEdgeVector& to2 = parkApproaches[altPa.first];
                    assert(to2.size() > 0);
                    if (to1.size() > to2.size()) {
                        if (std::equal(to2.begin(), to2.end(), to1.begin())) {
                            // other target lies on the route to the current candidate
                            dominated = true;
                            //std::cout << SIMTIME << " rrP veh=" << veh.getID() << " full=" << destParkArea->getID() << " cand=" << it->first->getID() << " onTheWay=" << altPa.first->getID() << "\n";
                            break;
                        }
                    } else if (to1 == to2 && endPos > altPa.first->getEndLanePosition()) {
                        // other target is on the same edge but ahead of the current candidate
                        dominated = true;
                        //std::cout << SIMTIME << " rrP veh=" << veh.getID() << " full=" << destParkArea->getID() << " cand=" << it->first->getID() << " sameEdge=" << altPa.first->getID() << "\n";
                        break;
                    }
                }
                double prob = 0;
                if (!dominated) {
                    prob = RandHelper::rand(parkValues["probability"], veh.getRNG());
                    parkValues["probability"] = 1.0 - prob / maxValues["probability"];
                } else {
                    // worst probability score
                    parkValues["probability"] = 1.0;
                }
            } else {
                // value takes no effect due to weight=0
                parkValues["probability"] = 0;
            }
            // normalizing with maximum values (we want to maximize some parameters then we reverse the value)
            parkValues["capacity"] = maxValues["capacity"] > 0.0 ? 1.0 - parkValues["capacity"] / maxValues["capacity"] : 0.0;
            parkValues["absfreespace"] = maxValues["absfreespace"] > 0.0 ? 1.0 - parkValues["absfreespace"] / maxValues["absfreespace"] : 0.0;
            parkValues["relfreespace"] = maxValues["relfreespace"] > 0.0 ? 1.0 - parkValues["relfreespace"] / maxValues["relfreespace"] : 0.0;

            parkValues["distanceto"] = maxValues["distanceto"] > 0.0 ? parkValues["distanceto"] / maxValues["distanceto"] : 0.0;
            parkValues["timeto"] = maxValues["timeto"] > 0.0 ? parkValues["timeto"] / maxValues["timeto"] : 0.0;

            parkValues["distancefrom"] = maxValues["distancefrom"] > 0.0 ? parkValues["distancefrom"] / maxValues["distancefrom"] : 0.0;
            parkValues["timefrom"] = maxValues["timefrom"] > 0.0 ? parkValues["timefrom"] / maxValues["timefrom"] : 0.0;

            // get the parking area cost
            double parkingCost = 0.0;

            // sum every index with its weight
            for (ParkingParamMap_t::iterator pc = parkValues.begin(); pc != parkValues.end(); ++pc) {
                parkingCost += weights[pc->first] * pc->second;
            }
            veh.rememberParkingAreaScore(it->first, toString(parkingCost)
                    //+ " rfs=" + toString(parkValues["relfreespace"])
                    //+ " dt=" + toString(parkValues["distanceto"])
                    //+ " p=" + toString(parkValues["probability"])
                    );

            // get the parking area with minimum cost
            if (nearParkArea == nullptr || parkingCost < minParkingCost) {
                minParkingCost = parkingCost;
                nearParkArea = it->first;
                newRoute = newRoutes[nearParkArea];
            }

#ifdef DEBUG_PARKING
            if (DEBUGCOND) {
                std::cout << "    altPA=" << it->first->getID() << " score=" << parkingCost << " vals=" << joinToString(parkValues, " ", ":") << "\n";
            }
#endif
        }
        veh.setNumberParkingReroutes(veh.getNumberParkingReroutes() + 1);
    } else {
#ifdef DEBUG_PARKING
        if (DEBUGCOND) {
            std::cout << SIMTIME << " rerouter=" << getID() << " veh=" << veh.getID() << " rerouteParkingArea dest=" << destParkArea->getID() << " sufficient space\n";
        }
#endif
    }

#ifdef DEBUG_PARKING
    if (DEBUGCOND) {
        std::cout << "  parkingResult=" << Named::getIDSecure(nearParkArea) << "\n";
    }
#endif

    return nearParkArea;
}


bool
MSTriggeredRerouter::addParkValues(SUMOVehicle& veh, double brakeGap, bool newDestination,
        MSParkingArea* pa, double paOccupancy, double prob,
        SUMOAbstractRouter<MSEdge, SUMOVehicle>& router,
        MSParkingAreaMap_t& parkAreas,
        std::map<MSParkingArea*, ConstMSEdgeVector>& newRoutes,
        std::map<MSParkingArea*, ConstMSEdgeVector>& parkApproaches,
        ParkingParamMap_t& maxValues) const
{
    // a map stores the parking values
    ParkingParamMap_t parkValues;

    const MSRoute& route = veh.getRoute();
    const RGBColor& c = route.getColor();
    const MSEdge* parkEdge = &(pa->getLane().getEdge());

    const bool includeInternalLengths = MSGlobals::gUsingInternalLanes && MSNet::getInstance()->hasInternalLinks();

    // Compute the route from the current edge to the parking area edge
    ConstMSEdgeVector edgesToPark;
    const double parkPos = pa->getLastFreePos(veh);
    router.compute(veh.getEdge(), veh.getPositionOnLane(), parkEdge, parkPos, &veh, MSNet::getInstance()->getCurrentTimeStep(), edgesToPark, true);

#ifdef DEBUG_PARKING
    if (DEBUGCOND) {
        //std::cout << "    altPA=" << pa->getID() << " vehEdge=" << veh.getEdge()->getID() << " parkEdge " << parkEdge->getID() << " edgesToPark=" << edgesToPark.size() << "\n";
    }
#endif

    if (edgesToPark.size() > 0) {
        // Compute the route from the parking area edge to the end of the route
        ConstMSEdgeVector edgesFromPark;
        parkApproaches[pa] = edgesToPark;

        const MSEdge* nextDestination = route.getLastEdge();
        double nextPos = veh.getArrivalPos();
        int nextDestinationIndex = route.size() - 1;
        if (!newDestination) {
            std::vector<std::pair<int, double> > stopIndices = veh.getStopIndices();
            if (stopIndices.size() > 1) {
                nextDestinationIndex = stopIndices[1].first;
                nextDestination = route.getEdges()[nextDestinationIndex];
                nextPos = stopIndices[1].second;

            }
            router.compute(parkEdge, parkPos, nextDestination, nextPos,  &veh, MSNet::getInstance()->getCurrentTimeStep(), edgesFromPark, true);
        }
#ifdef DEBUG_PARKING
        if (DEBUGCOND) {
            //std::cout << "    altPA=" << pa->getID() << " parkEdge=" << parkEdge->getID() << " nextDest=" << nextDestination->getID() << " edgesFromPark=" << edgesFromPark.size() << "\n";
        }
#endif

        if (edgesFromPark.size() > 0 || newDestination) {

            parkValues["probability"] = prob;

            if (parkValues["probability"] > maxValues["probability"]) {
                maxValues["probability"] = parkValues["probability"];
            }

            parkValues["capacity"] = (double)(pa->getCapacity());
            parkValues["absfreespace"] = (double)(pa->getCapacity() - paOccupancy);
            // if capacity = 0 then absfreespace and relfreespace are also 0
            parkValues["relfreespace"] = parkValues["absfreespace"] / MAX2(1.0, parkValues["capacity"]);

            if (parkValues["capacity"] > maxValues["capacity"]) {
                maxValues["capacity"] = parkValues["capacity"];
            }

            if (parkValues["absfreespace"] > maxValues["absfreespace"]) {
                maxValues["absfreespace"] = parkValues["absfreespace"];
            }

            if (parkValues["relfreespace"] > maxValues["relfreespace"]) {
                maxValues["relfreespace"] = parkValues["relfreespace"];
            }

            MSRoute routeToPark(route.getID() + "!topark#1", edgesToPark, false, &c == &RGBColor::DEFAULT_COLOR ? nullptr : new RGBColor(c), route.getStops());

            // The distance from the current edge to the new parking area
            double toPos = pa->getBeginLanePosition();
            if (&pa->getLane().getEdge() == veh.getEdge()) {
                toPos = MAX2(veh.getPositionOnLane(), toPos);
            }
            parkValues["distanceto"] = routeToPark.getDistanceBetween(veh.getPositionOnLane(), toPos,
                    routeToPark.begin(), routeToPark.end() - 1, includeInternalLengths);

            if (parkValues["distanceto"] == std::numeric_limits<double>::max()) {
                WRITE_WARNING("Invalid distance computation for vehicle '" + veh.getID() + "' to parkingArea '" + pa->getID() + "' at time " + time2string(SIMTIME));
            }

            //std::cout << SIMTIME << " veh=" << veh.getID() << " candidate=" << pa->getID()
            //    << " distanceTo=" << parkValues["distanceto"]
            //    << " brakeGap=" << brakeGap
            //    << " routeToPark=" << toString(edgesToPark)
            //    << " fromPos=" << veh.getPositionOnLane()
            //    << " tPos=" << pa->getBeginLanePosition()
            //    << "\n";
            const double distToEnd = parkValues["distanceto"] - toPos + pa->getEndLanePosition();
            if (distToEnd < brakeGap) {
                veh.rememberParkingAreaScore(pa, "tooClose");
#ifdef DEBUG_PARKING
                if (DEBUGCOND) {
                    std::cout << "    altPA=" << pa->getID() << " too close to brake (dist=" << distToEnd << " brakeGap=" << brakeGap << "\n";
                }
#endif
                return false;
            }

            // The time to reach the new parking area
            parkValues["timeto"] = router.recomputeCosts(edgesToPark, &veh, MSNet::getInstance()->getCurrentTimeStep());

            if (parkValues["distanceto"] > maxValues["distanceto"]) {
                maxValues["distanceto"] = parkValues["distanceto"];
            }

            if (parkValues["timeto"] > maxValues["timeto"]) {
                maxValues["timeto"] = parkValues["timeto"];
            }

            ConstMSEdgeVector newEdges = edgesToPark;

            if (newDestination) {
                parkValues["distancefrom"] = 0;
                parkValues["timefrom"] = 0;
            } else {
                MSRoute routeFromPark(route.getID() + "!frompark#1", edgesFromPark, false,
                        &c == &RGBColor::DEFAULT_COLOR ? nullptr : new RGBColor(c), route.getStops());
                // The distance from the new parking area to the end of the route
                parkValues["distancefrom"] = routeFromPark.getDistanceBetween(pa->getBeginLanePosition(), routeFromPark.getLastEdge()->getLength(),
                        routeFromPark.begin(), routeFromPark.end() - 1, includeInternalLengths);
                if (parkValues["distancefrom"] == std::numeric_limits<double>::max()) {
                    WRITE_WARNING("Invalid distance computation for vehicle '" + veh.getID() + "' from parkingArea '" + pa->getID() + "' at time " + time2string(SIMTIME));
                }
                // The time to reach this area
                parkValues["timefrom"] = router.recomputeCosts(edgesFromPark, &veh, MSNet::getInstance()->getCurrentTimeStep());
                newEdges.insert(newEdges.end(), edgesFromPark.begin() + 1, edgesFromPark.end());
                newEdges.insert(newEdges.end(), route.begin() + nextDestinationIndex + 1, route.end());
            }

            if (parkValues["distancefrom"] > maxValues["distancefrom"]) {
                maxValues["distancefrom"] = parkValues["distancefrom"];
            }

            if (parkValues["timefrom"] > maxValues["timefrom"]) {
                maxValues["timefrom"] = parkValues["timefrom"];
            }

            parkAreas[pa] = parkValues;
            newRoutes[pa] = newEdges;

            return true;
        } else {
            veh.rememberParkingAreaScore(pa, "destUnreachable");
        }
    } else {
        veh.rememberParkingAreaScore(pa, "unreachable");
    }
#ifdef DEBUG_PARKING
    if (DEBUGCOND) {
        std::cout << "    altPA=" << pa->getID() << " disconnected\n";
    }
#endif
    // unreachable
    return false;
}


bool
MSTriggeredRerouter::vehicleApplies(const SUMOVehicle& veh) const {
    if (myVehicleTypes.empty() || myVehicleTypes.count(veh.getVehicleType().getOriginalID()) > 0) {
        return true;
    } else {
        std::set<std::string> vTypeDists = MSNet::getInstance()->getVehicleControl().getVTypeDistributionMembership(veh.getVehicleType().getOriginalID());
        for (auto vTypeDist : vTypeDists) {
            if (myVehicleTypes.count(vTypeDist) > 0) {
                return true;
            }
        }
        return false;
    }
}


void
MSTriggeredRerouter::checkParkingRerouteConsistency() {
    // if a parkingArea is a rerouting target, it should generally have a
    // rerouter on it's edge or vehicles will be stuck there once it's full.
    // The user should receive a Warning in this case
    std::set<MSEdge*> parkingRerouterEdges;
    std::map<MSParkingArea*, std::string, ComparatorIdLess> targetedParkingArea; // paID -> targetingRerouter
    for (const auto& rr : myInstances) {
        bool hasParkingReroute = false;
        for (const RerouteInterval& interval : rr.second->myIntervals) {
            if (interval.parkProbs.getOverallProb() > 0) {
                hasParkingReroute = true;
                for (const ParkingAreaVisible& pav : interval.parkProbs.getVals()) {
                    targetedParkingArea[pav.first] = rr.first;
                }
            }
        }
        if (hasParkingReroute) {
            parkingRerouterEdges.insert(rr.second->myEdges.begin(), rr.second->myEdges.end());
        }
    }
    for (const auto& item : targetedParkingArea) {
        if (parkingRerouterEdges.count(&item.first->getLane().getEdge()) == 0) {
            WRITE_WARNINGF("ParkingArea '%' is targeted by rerouter '%' but doesn't have it's own rerouter. This may cause parking search to abort.",
                           item.first->getID(), item.second);
        }
    }
}
/****************************************************************************/
