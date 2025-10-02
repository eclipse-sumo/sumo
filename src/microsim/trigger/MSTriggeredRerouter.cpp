/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
#include <microsim/MSLink.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSBaseVehicle.h>
#include <microsim/MSRoute.h>
#include <microsim/MSEdge.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSParkingArea.h>
#include <microsim/MSStop.h>
#include <microsim/traffic_lights/MSRailSignal.h>
#include <microsim/traffic_lights/MSRailSignalConstraint.h>
#include <microsim/transportables/MSPerson.h>
#include <microsim/devices/MSDevice_Routing.h>
#include <microsim/devices/MSRoutingEngine.h>
#include "MSTriggeredRerouter.h"

#include <mesosim/MELoop.h>
#include <mesosim/MESegment.h>

//#define DEBUG_REROUTER
//#define DEBUG_OVERTAKING
#define DEBUGCOND(veh) (veh.isSelected())
//#define DEBUGCOND(veh) (true)
//#define DEBUGCOND(veh) (veh.getID() == "")

/// assume that a faster train has more priority and a slower train doesn't matter
#define DEFAULT_PRIO_OVERTAKER 1
#define DEFAULT_PRIO_OVERTAKEN 0.001

// ===========================================================================
// static member definition
// ===========================================================================
MSEdge MSTriggeredRerouter::mySpecialDest_keepDestination("MSTriggeredRerouter_keepDestination", -1, SumoXMLEdgeFunc::UNKNOWN, "", "", "", -1, 0);
MSEdge MSTriggeredRerouter::mySpecialDest_terminateRoute("MSTriggeredRerouter_terminateRoute", -1, SumoXMLEdgeFunc::UNKNOWN, "", "", "", -1, 0);
const double MSTriggeredRerouter::DEFAULT_MAXDELAY(7200);
std::map<std::string, MSTriggeredRerouter*> MSTriggeredRerouter::myInstances;


// ===========================================================================
// method definitions
// ===========================================================================
MSTriggeredRerouter::MSTriggeredRerouter(const std::string& id,
        const MSEdgeVector& edges, double prob, bool off, bool optional,
        SUMOTime timeThreshold, const std::string& vTypes, const Position& pos, const double radius) :
    Named(id),
    MSMoveReminder(id),
    MSStoppingPlaceRerouter("parking"),
    myEdges(edges),
    myProbability(prob),
    myUserProbability(prob),
    myAmInUserMode(false),
    myAmOptional(optional),
    myPosition(pos),
    myRadius(radius),
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
    if (myPosition == Position::INVALID) {
        myPosition = edges.front()->getLanes()[0]->getShape()[0];
    }
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
        if (myParsedRerouteInterval.begin >= myParsedRerouteInterval.end) {
            throw ProcessError(TLF("rerouter '%': interval end % is not after begin %.", getID(),
                                   time2string(myParsedRerouteInterval.end),
                                   time2string(myParsedRerouteInterval.begin)));
        }
    }
    if (element == SUMO_TAG_DEST_PROB_REROUTE) {
        // by giving probabilities of new destinations
        // get the destination edge
        std::string dest = attrs.getStringSecure(SUMO_ATTR_ID, "");
        if (dest == "") {
            throw ProcessError(TLF("rerouter '%': destProbReroute has no destination edge id.", getID()));
        }
        MSEdge* to = MSEdge::dictionary(dest);
        if (to == nullptr) {
            if (dest == "keepDestination") {
                to = &mySpecialDest_keepDestination;
            } else if (dest == "terminateRoute") {
                to = &mySpecialDest_terminateRoute;
            } else {
                throw ProcessError(TLF("rerouter '%': Destination edge '%' is not known.", getID(), dest));
            }
        }
        // get the probability to reroute
        bool ok = true;
        double prob = attrs.getOpt<double>(SUMO_ATTR_PROB, getID().c_str(), ok, 1.);
        if (!ok) {
            throw ProcessError();
        }
        if (prob < 0) {
            throw ProcessError(TLF("rerouter '%': Attribute 'probability' for destination '%' is negative (must not).", getID(), dest));
        }
        // add
        myParsedRerouteInterval.edgeProbs.add(to, prob);
    }

    if (element == SUMO_TAG_CLOSING_REROUTE) {
        // by closing edge
        const std::string& closed_id = attrs.getStringSecure(SUMO_ATTR_ID, "");
        MSEdge* const closedEdge = MSEdge::dictionary(closed_id);
        if (closedEdge == nullptr) {
            throw ProcessError(TLF("rerouter '%': Edge '%' to close is not known.", getID(), closed_id));
        }
        bool ok;
        const std::string allow = attrs.getOpt<std::string>(SUMO_ATTR_ALLOW, getID().c_str(), ok, "", false);
        const std::string disallow = attrs.getOpt<std::string>(SUMO_ATTR_DISALLOW, getID().c_str(), ok, "");
        const SUMOTime until = attrs.getOptSUMOTimeReporting(SUMO_ATTR_UNTIL, nullptr, ok, -1);
        SVCPermissions permissions = parseVehicleClasses(allow, disallow);
        myParsedRerouteInterval.closed[closedEdge] = std::make_pair(permissions, STEPS2TIME(until));
    }

    if (element == SUMO_TAG_CLOSING_LANE_REROUTE) {
        // by closing lane
        std::string closed_id = attrs.getStringSecure(SUMO_ATTR_ID, "");
        MSLane* closedLane = MSLane::dictionary(closed_id);
        if (closedLane == nullptr) {
            throw ProcessError(TLF("rerouter '%': Lane '%' to close is not known.", getID(), closed_id));
        }
        bool ok;
        SVCPermissions permissions = SVC_AUTHORITY;
        if (attrs.hasAttribute(SUMO_ATTR_ALLOW) || attrs.hasAttribute(SUMO_ATTR_DISALLOW)) {
            const std::string allow = attrs.getOpt<std::string>(SUMO_ATTR_ALLOW, getID().c_str(), ok, "", false);
            const std::string disallow = attrs.getOpt<std::string>(SUMO_ATTR_DISALLOW, getID().c_str(), ok, "");
            permissions = parseVehicleClasses(allow, disallow);
        }
        myParsedRerouteInterval.closedLanes[closedLane] = permissions;
    }

    if (element == SUMO_TAG_ROUTE_PROB_REROUTE) {
        // by explicit rerouting using routes
        // check if route exists
        std::string routeStr = attrs.getStringSecure(SUMO_ATTR_ID, "");
        if (routeStr == "") {
            throw ProcessError(TLF("rerouter '%': routeProbReroute has no alternative route id.", getID()));
        }
        ConstMSRoutePtr route = MSRoute::dictionary(routeStr);
        if (route == nullptr) {
            throw ProcessError(TLF("rerouter '%': Alternative route '%' does not exist.", getID(), routeStr));
        }

        // get the probability to reroute
        bool ok = true;
        double prob = attrs.getOpt<double>(SUMO_ATTR_PROB, getID().c_str(), ok, 1.);
        if (!ok) {
            throw ProcessError();
        }
        if (prob < 0) {
            throw ProcessError(TLF("rerouter '%': Attribute 'probability' for alternative route '%' is negative (must not).", getID(), routeStr));
        }
        // add
        myParsedRerouteInterval.routeProbs.add(route, prob);
    }

    if (element == SUMO_TAG_PARKING_AREA_REROUTE) {
        std::string parkingarea = attrs.getStringSecure(SUMO_ATTR_ID, "");
        if (parkingarea == "") {
            throw ProcessError(TLF("rerouter '%': parkingAreaReroute requires a parkingArea id.", getID()));
        }
        MSParkingArea* pa = static_cast<MSParkingArea*>(MSNet::getInstance()->getStoppingPlace(parkingarea, SUMO_TAG_PARKING_AREA));
        if (pa == nullptr) {
            throw ProcessError(TLF("rerouter '%': parkingArea '%' is not known.", getID(), parkingarea));
        }
        // get the probability to reroute
        bool ok = true;
        const double prob = attrs.getOpt<double>(SUMO_ATTR_PROB, getID().c_str(), ok, 1.);
        if (!ok) {
            throw ProcessError();
        }
        if (prob < 0) {
            throw ProcessError(TLF("rerouter '%': Attribute 'probability' for parkingArea '%' is negative (must not).", getID(), parkingarea));
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
            throw ProcessError(TLF("rerouter '%': No via edge id given.", getID()));
        }
        MSEdge* const via = MSEdge::dictionary(viaID);
        if (via == nullptr) {
            throw ProcessError(TLF("rerouter '%': Via Edge '%' is not known.", getID(), viaID));
        }
        // get the probability to reroute
        bool ok = true;
        double prob = attrs.getOpt<double>(SUMO_ATTR_PROB, getID().c_str(), ok, 1.);
        if (!ok) {
            throw ProcessError();
        }
        if (prob < 0) {
            throw ProcessError(TLF("rerouter '%': Attribute 'probability' for via '%' is negative (must not).", getID(), viaID));
        }
        // add
        myParsedRerouteInterval.edgeProbs.add(via, prob);
        myParsedRerouteInterval.isVia = true;
    }
    if (element == SUMO_TAG_OVERTAKING_REROUTE) {
        // for letting a slow train use a siding to be overtaken by a fast train
        OvertakeLocation oloc;
        bool ok = true;
        for (const std::string& edgeID : attrs.get<std::vector<std::string> >(SUMO_ATTR_MAIN, getID().c_str(), ok)) {
            MSEdge* edge = MSEdge::dictionary(edgeID);
            if (edge == nullptr) {
                throw InvalidArgument(TLF("The main edge '%' to use within rerouter '%' is not known.", edgeID, getID()));
            }
            oloc.main.push_back(edge);
            oloc.cMain.push_back(edge);
        }
        for (const std::string& edgeID : attrs.get<std::vector<std::string> >(SUMO_ATTR_SIDING, getID().c_str(), ok)) {
            MSEdge* edge = MSEdge::dictionary(edgeID);
            if (edge == nullptr) {
                throw InvalidArgument(TLF("The siding edge '%' to use within rerouter '%' is not known.", edgeID, getID()));
            }
            oloc.siding.push_back(edge);
            oloc.cSiding.push_back(edge);
        }
        oloc.sidingExit = findSignal(oloc.cSiding.begin(), oloc.cSiding.end());
        if (oloc.sidingExit == nullptr) {
            throw InvalidArgument(TLF("The siding within rerouter '%' does not have a rail signal.", getID()));
        }
        for (auto it = oloc.cSiding.begin(); it != oloc.cSiding.end(); it++) {
            oloc.sidingLength += (*it)->getLength();
            if ((*it)->getToJunction()->getID() == oloc.sidingExit->getID()) {
                break;
            }
        }
        oloc.minSaving = attrs.getOpt<double>(SUMO_ATTR_MINSAVING, getID().c_str(), ok, 300);
        myParsedRerouteInterval.overtakeLocations.push_back(oloc);
    }
    if (element == SUMO_TAG_STATION_REROUTE) {
        // for letting a train switch it's stopping place in case of conflict
        const std::string stopID = attrs.getStringSecure(SUMO_ATTR_ID, "");
        if (stopID == "") {
            throw ProcessError(TLF("rerouter '%': stationReroute requires a stopping place id.", getID()));
        }
        MSStoppingPlace* stop = MSNet::getInstance()->getStoppingPlace(stopID);
        if (stop == nullptr) {
            throw ProcessError(TLF("rerouter '%': stopping place '%' is not known.", getID(), stopID));
        }
        myParsedRerouteInterval.stopAlternatives.push_back(std::make_pair(stop, true));
    }
}


void
MSTriggeredRerouter::myEndElement(int element) {
    if (element == SUMO_TAG_INTERVAL) {
        // precompute permissionsAllowAll
        bool allowAll = true;
        for (const auto& entry : myParsedRerouteInterval.closed) {
            allowAll = allowAll && entry.second.first == SVCAll;
            if (!allowAll) {
                break;
            }
        }
        myParsedRerouteInterval.permissionsAllowAll = allowAll;

        for (auto paVi : myParsedRerouteInterval.parkProbs.getVals()) {
            dynamic_cast<MSParkingArea*>(paVi.first)->setNumAlternatives((int)myParsedRerouteInterval.parkProbs.getVals().size() - 1);
        }
        if (myParsedRerouteInterval.closedLanes.size() > 0) {
            // collect edges that are affect by a closed lane
            std::set<MSEdge*> affected;
            for (std::pair<MSLane*, SVCPermissions> settings : myParsedRerouteInterval.closedLanes) {
                affected.insert(&settings.first->getEdge());
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
        if (!(myParsedRerouteInterval.closed.empty() && myParsedRerouteInterval.closedLanes.empty())) {
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
        if (i.begin == currentTime && !(i.closed.empty() && i.closedLanes.empty()) /*&& i.permissions != SVCAll*/) {
            for (const auto& settings : i.closed) {
                for (MSLane* lane : settings.first->getLanes()) {
                    //std::cout << SIMTIME << " closing: intervalID=" << i.id << " lane=" << lane->getID() << " prevPerm=" << getVehicleClassNames(lane->getPermissions()) << " new=" << getVehicleClassNames(i.permissions) << "\n";
                    lane->setPermissions(settings.second.first, i.id);
                }
                settings.first->rebuildAllowedLanes();
                updateVehicles = true;
            }
            for (std::pair<MSLane*, SVCPermissions> settings : i.closedLanes) {
                settings.first->setPermissions(settings.second, i.id);
                settings.first->getEdge().rebuildAllowedLanes();
                updateVehicles = true;
            }
            MSNet::getInstance()->getBeginOfTimestepEvents()->addEvent(
                new WrappingCommand<MSTriggeredRerouter>(this, &MSTriggeredRerouter::setPermissions), i.end);
        }
        if (i.end == currentTime && !(i.closed.empty() && i.closedLanes.empty()) /*&& i.permissions != SVCAll*/) {
            for (auto settings : i.closed) {
                for (MSLane* lane : settings.first->getLanes()) {
                    lane->resetPermissions(i.id);
                    //std::cout << SIMTIME << " opening: intervalID=" << i.id << " lane=" << lane->getID() << " restore prevPerm=" << getVehicleClassNames(lane->getPermissions()) << "\n";
                }
                settings.first->rebuildAllowedLanes();
                updateVehicles = true;
            }
            for (std::pair<MSLane*, SVCPermissions> settings : i.closedLanes) {
                settings.first->resetPermissions(i.id);
                settings.first->getEdge().rebuildAllowedLanes();
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
                ri.parkProbs.getOverallProb() > 0 ||
                // stationReroute
                ri.stopAlternatives.size() > 0) {
                return &ri;
            }
            if (!ri.closed.empty() || !ri.closedLanesAffected.empty() || !ri.overtakeLocations.empty()) {
                const std::set<SUMOTrafficObject::NumericalID>& edgeIndices = obj.getUpcomingEdgeIDs();
                if (affected(edgeIndices, ri.getClosedEdges())
                        || affected(edgeIndices, ri.closedLanesAffected)) {
                    return &ri;
                }
                for (const OvertakeLocation& oloc : ri.overtakeLocations) {
                    if (affected(edgeIndices, oloc.main)) {
                        return &ri;
                    }
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
                    || !ri.closed.empty() || !ri.closedLanesAffected.empty() || !ri.overtakeLocations.empty()) {
                return &ri;
            }
        }
    }
    return nullptr;
}


bool
MSTriggeredRerouter::notifyEnter(SUMOTrafficObject& tObject, MSMoveReminder::Notification reason, const MSLane* /* enteredLane */) {
    if (myAmOptional || myRadius != std::numeric_limits<double>::max()) {
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
    if (myRadius != std::numeric_limits<double>::max() && tObject.getPosition().distanceTo(myPosition) > myRadius) {
        return true;
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
    if (DEBUGCOND(tObject)) {
        std::cout << SIMTIME << " veh=" << tObject.getID() << " check rerouter " << getID() << " lane=" << Named::getIDSecure(tObject.getLane()) << " edge=" << tObject.getEdge()->getID() << " finalEdge=" << lastEdge->getID() /*<< " arrivalPos=" << tObject.getArrivalPos()*/ << "\n";
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
                    ? MSRoutingEngine::getRouterTT(veh.getRNGIndex(), veh.getVClass(), rerouteDef->getClosed())
                    : MSNet::getInstance()->getRouterTT(veh.getRNGIndex(), rerouteDef->getClosed());
            const double routeCost = router.recomputeCosts(newRoute, &veh, MSNet::getInstance()->getCurrentTimeStep());
            ConstMSEdgeVector prevEdges(veh.getCurrentRouteEdge(), veh.getRoute().end());
            const double previousCost = router.recomputeCosts(prevEdges, &veh, MSNet::getInstance()->getCurrentTimeStep());
            const double savings = previousCost - routeCost;
            resetClosedEdges(hasReroutingDevice, veh);
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
    if (rerouteDef->overtakeLocations.size() > 0) {
        if (!tObject.isVehicle()) {
            return false;
        }
        SUMOVehicle& veh = static_cast<SUMOVehicle&>(tObject);
        const ConstMSEdgeVector& oldEdges = veh.getRoute().getEdges();
        double bestSavings = -std::numeric_limits<double>::max();
        double netSaving;
        int bestIndex = -1;
        MSRouteIterator bestMainStart = oldEdges.end();
        std::pair<const SUMOVehicle*, MSRailSignal*> best_overtaker_signal(nullptr, nullptr);
        int index = -1;
        // sort locations by descending distance to vehicle
        std::vector<std::pair<int, int> > sortedLocs;
        for (const OvertakeLocation& oloc : rerouteDef->overtakeLocations) {
            index++;
            if (veh.getLength() > oloc.sidingLength) {
                continue;
            }
            auto mainStart = std::find(veh.getCurrentRouteEdge(), oldEdges.end(), oloc.main.front());
            if (mainStart == oldEdges.end()
                    // exit main within
                    || ConstMSEdgeVector(mainStart, mainStart + oloc.main.size()) != oloc.cMain
                    // stop in main
                    || (veh.hasStops() && veh.getNextStop().edge < (mainStart + oloc.main.size()))) {
                //std::cout << SIMTIME << " veh=" << veh.getID() << " wrong route or stop\n";
                continue;
            }
            // negated iterator distance for descending order
            sortedLocs.push_back(std::make_pair(-(mainStart - veh.getCurrentRouteEdge()), index));
        }
        std::sort(sortedLocs.begin(), sortedLocs.end());
        for (auto item : sortedLocs) {
            index = item.second;
            const OvertakeLocation& oloc = rerouteDef->overtakeLocations[index];
            auto mainStart = veh.getCurrentRouteEdge() - item.first;  // subtracting negative difference
            std::pair<const SUMOVehicle*, MSRailSignal*> overtaker_signal = overtakingTrain(veh, mainStart, oloc, netSaving);
            if (overtaker_signal.first != nullptr && netSaving > bestSavings) {
                bestSavings = netSaving;
                bestIndex = index;
                best_overtaker_signal = overtaker_signal;
                bestMainStart = mainStart;
#ifdef DEBUG_OVERTAKING
                std::cout << "    newBest index=" << bestIndex << " saving=" << bestSavings << "\n";
#endif
            }
        }
        if (bestIndex >= 0) {
            const OvertakeLocation& oloc = rerouteDef->overtakeLocations[bestIndex];
            SUMOAbstractRouter<MSEdge, SUMOVehicle>& router = hasReroutingDevice
                    ? MSRoutingEngine::getRouterTT(veh.getRNGIndex(), veh.getVClass(), rerouteDef->getClosed())
                    : MSNet::getInstance()->getRouterTT(veh.getRNGIndex(), rerouteDef->getClosed());
            ConstMSEdgeVector newEdges(veh.getCurrentRouteEdge(), bestMainStart);
            newEdges.insert(newEdges.end(), oloc.siding.begin(), oloc.siding.end());
            newEdges.insert(newEdges.end(), bestMainStart + oloc.main.size(), oldEdges.end());
            const double routeCost = router.recomputeCosts(newEdges, &veh, MSNet::getInstance()->getCurrentTimeStep());
            const double savings = (router.recomputeCosts(oloc.cMain, &veh, MSNet::getInstance()->getCurrentTimeStep())
                                    - router.recomputeCosts(oloc.cSiding, &veh, MSNet::getInstance()->getCurrentTimeStep()));
            const std::string info = getID() + ":" + toString(SUMO_TAG_OVERTAKING_REROUTE) + ":" + best_overtaker_signal.first->getID();
            veh.replaceRouteEdges(newEdges, routeCost, savings, info, false, false, false);
            oloc.sidingExit->addConstraint(veh.getID(), new MSRailSignalConstraint_Predecessor(
                    MSRailSignalConstraint::PREDECESSOR, best_overtaker_signal.second, best_overtaker_signal.first->getID(), 100, true));
            resetClosedEdges(hasReroutingDevice, veh);
        }
        return false;
    }
    if (rerouteDef->stopAlternatives.size() > 0) {
        // somewhat similar to parkProbs but taking into account public transport schedule
        if (!tObject.isVehicle()) {
            return false;
        }
        checkStopSwitch(static_cast<MSBaseVehicle&>(tObject), rerouteDef);
    }
    // get rerouting params
    ConstMSRoutePtr newRoute = rerouteDef->routeProbs.getOverallProb() > 0 ? rerouteDef->routeProbs.get() : nullptr;
    // we will use the route if given rather than calling our own dijsktra...
    if (newRoute != nullptr) {
#ifdef DEBUG_REROUTER
        if (DEBUGCOND(tObject)) {
            std::cout << "    replacedRoute from routeDist " << newRoute->getID() << "\n";
        }
#endif
        tObject.replaceRoute(newRoute, getID());
        return false; // XXX another interval could appear later but we would have to track whether the currenty interval was already used
    }
    const MSEdge* newEdge = lastEdge;
    // ok, try using a new destination
    double newArrivalPos = -1;
    const MSEdgeVector closedEdges = rerouteDef->getClosedEdges();
    const bool destUnreachable = std::find(closedEdges.begin(), closedEdges.end(), lastEdge) != closedEdges.end();
    bool keepDestination = false;
    // if we have a closingReroute, only assign new destinations to vehicles which cannot reach their original destination
    // if we have a closingLaneReroute, no new destinations should be assigned
    if (closedEdges.empty() || destUnreachable || rerouteDef->isVia) {
        newEdge = rerouteDef->edgeProbs.getOverallProb() > 0 ? rerouteDef->edgeProbs.get() : lastEdge;
        assert(newEdge != nullptr);
        if (newEdge == &mySpecialDest_terminateRoute) {
            keepDestination = true;
            newEdge = tObject.getEdge();
            newArrivalPos = tObject.getPositionOnLane(); // instant arrival
        } else if (newEdge == &mySpecialDest_keepDestination || newEdge == lastEdge) {
            if (destUnreachable && rerouteDef->permissionsAllowAll) {
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
    MSEdgeVector closed = rerouteDef->getClosedEdges();
    Prohibitions prohibited = rerouteDef->getClosed();
    if (rerouteDef->closed.empty() || destUnreachable || rerouteDef->isVia || affected(tObject.getUpcomingEdgeIDs(), closed)) {
        if (tObject.isVehicle()) {
            SUMOVehicle& veh = static_cast<SUMOVehicle&>(tObject);
            const bool canChangeDest = rerouteDef->edgeProbs.getOverallProb() > 0;
            MSVehicleRouter& router = hasReroutingDevice
                                      ? MSRoutingEngine::getRouterTT(veh.getRNGIndex(), veh.getVClass(), prohibited)
                                      : MSNet::getInstance()->getRouterTT(veh.getRNGIndex(), prohibited);
            bool ok = veh.reroute(now, getID(), router, false, false, canChangeDest, newEdge);
            if (!ok && !keepDestination && canChangeDest) {
                // destination unreachable due to closed intermediate edges. pick among alternative targets
                RandomDistributor<MSEdge*> edgeProbs2 = rerouteDef->edgeProbs;
                edgeProbs2.remove(const_cast<MSEdge*>(newEdge));
                while (!ok && edgeProbs2.getVals().size() > 0) {
                    newEdge = edgeProbs2.get();
                    edgeProbs2.remove(const_cast<MSEdge*>(newEdge));
                    if (newEdge == &mySpecialDest_terminateRoute) {
                        newEdge = veh.getEdge();
                        newArrivalPos = veh.getPositionOnLane(); // instant arrival
                    }
                    if (newEdge == &mySpecialDest_keepDestination && !rerouteDef->permissionsAllowAll) {
                        newEdge = lastEdge;
                        break;
                    }
                    ok = veh.reroute(now, getID(), router, false, false, true, newEdge);
                }

            }
            if (!rerouteDef->isVia) {
#ifdef DEBUG_REROUTER
                if (DEBUGCOND(tObject)) std::cout << "   rerouting:  newDest=" << newEdge->getID()
                                                      << " newEdges=" << toString(edges)
                                                      << " newArrivalPos=" << newArrivalPos << " numClosed=" << rerouteDef->closed.size()
                                                      << " destUnreachable=" << destUnreachable << " containsClosed=" << veh.getRoute().containsAnyOf(rerouteDef->getClosedEdges()) << "\n";
#endif
                if (ok && newArrivalPos != -1) {
                    // must be called here because replaceRouteEdges may also set the arrivalPos
                    veh.setArrivalPos(newArrivalPos);
                }

            }
        } else {
            // person rerouting here
            MSTransportableRouter& router = hasReroutingDevice
                                            ? MSRoutingEngine::getIntermodalRouterTT(tObject.getRNGIndex(), prohibited)
                                            : MSNet::getInstance()->getIntermodalRouter(tObject.getRNGIndex(), 0, prohibited);
            const bool success = router.compute(tObject.getEdge(), newEdge, tObject.getPositionOnLane(), "",
                                                rerouteDef->isVia ? newEdge->getLength() / 2. : tObject.getParameter().arrivalPos, "",
                                                tObject.getMaxSpeed(), nullptr, tObject.getVTypeParameter(), 0, now, items);
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
        if (!prohibited.empty()) {
            resetClosedEdges(hasReroutingDevice, tObject);
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
                                      ? MSRoutingEngine::getRouterTT(veh.getRNGIndex(), veh.getVClass(), prohibited)
                                      : MSNet::getInstance()->getRouterTT(veh.getRNGIndex(), prohibited);
            router.compute(newEdge, lastEdge, &veh, now, edges);
            const double routeCost = router.recomputeCosts(edges, &veh, now);
            hasReroutingDevice
            ? MSRoutingEngine::getRouterTT(veh.getRNGIndex(), veh.getVClass())
            : MSNet::getInstance()->getRouterTT(veh.getRNGIndex()); // reset closed edges
            const bool useNewRoute = veh.replaceRouteEdges(edges, routeCost, 0, getID());
#ifdef DEBUG_REROUTER
            if (DEBUGCOND(tObject)) std::cout << "   rerouting:  newDest=" << newEdge->getID()
                                                  << " newEdges=" << toString(edges)
                                                  << " useNewRoute=" << useNewRoute << " newArrivalPos=" << newArrivalPos << " numClosed=" << rerouteDef->closed.size()
                                                  << " destUnreachable=" << destUnreachable << " containsClosed=" << veh.getRoute().containsAnyOf(rerouteDef->getClosedEdges()) << "\n";
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
                                                ? MSRoutingEngine::getIntermodalRouterTT(tObject.getRNGIndex(), prohibited)
                                                : MSNet::getInstance()->getIntermodalRouter(tObject.getRNGIndex(), 0, prohibited);
                success = router.compute(newEdge, lastEdge, newEdge->getLength() / 2., "",
                                         tObject.getParameter().arrivalPos, "",
                                         tObject.getMaxSpeed(), nullptr, tObject.getVTypeParameter(), 0, now, items);
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
        if (!prohibited.empty()) {
            resetClosedEdges(hasReroutingDevice, tObject);
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
MSTriggeredRerouter::getStoppingPlaceOccupancy(MSStoppingPlace* sp) {
    return (double)(sp->getElement() == SUMO_TAG_PARKING_AREA
                    ? dynamic_cast<MSParkingArea*>(sp)->getOccupancy()
                    : sp->getStoppedVehicles().size());
}


double
MSTriggeredRerouter::getLastStepStoppingPlaceOccupancy(MSStoppingPlace* sp) {
    return (double)(sp->getElement() == SUMO_TAG_PARKING_AREA
                    ? dynamic_cast<MSParkingArea*>(sp)->getLastStepOccupancy()
                    : sp->getStoppedVehicles().size());
}


double
MSTriggeredRerouter::getStoppingPlaceCapacity(MSStoppingPlace* sp) {
    if (myBlockedStoppingPlaces.count(sp) == 0) {
        return (double)(sp->getElement() == SUMO_TAG_PARKING_AREA
                        ? dynamic_cast<MSParkingArea*>(sp)->getCapacity()
                        // assume only one vehicle at a time (for stationReroute)
                        : 1.);
    } else {
        return 0.;
    }
}


void
MSTriggeredRerouter::rememberBlockedStoppingPlace(SUMOVehicle& veh, const MSStoppingPlace* parkingArea, bool blocked) {
    veh.rememberBlockedParkingArea(parkingArea, blocked);
}


void
MSTriggeredRerouter::rememberStoppingPlaceScore(SUMOVehicle& veh, MSStoppingPlace* parkingArea, const std::string& score) {
    veh.rememberParkingAreaScore(parkingArea, score);
}


void
MSTriggeredRerouter::resetStoppingPlaceScores(SUMOVehicle& veh) {
    veh.resetParkingAreaScores();
}


SUMOTime
MSTriggeredRerouter::sawBlockedStoppingPlace(SUMOVehicle& veh, MSStoppingPlace* parkingArea, bool local) {
    return veh.sawBlockedParkingArea(parkingArea, local);
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
    MSStoppingPlace* destStoppingPlace = veh.getNextParkingArea();
    if (destStoppingPlace == nullptr) {
        // not driving towards the right type of stop
        return nullptr;
    }
    std::vector<StoppingPlaceVisible> parks;
    for (auto cand : rerouteDef->parkProbs.getVals()) {
        if (cand.first->accepts(&veh)) {
            parks.push_back(cand);
        }
    }
    StoppingPlaceParamMap_t addInput = {};
    return dynamic_cast<MSParkingArea*>(rerouteStoppingPlace(destStoppingPlace, parks, rerouteDef->parkProbs.getProbs(), veh, newDestination, newRoute, addInput, rerouteDef->getClosed()));
}


std::pair<const SUMOVehicle*, MSRailSignal*>
MSTriggeredRerouter::overtakingTrain( const SUMOVehicle& veh,
        ConstMSEdgeVector::const_iterator mainStart,
        const OvertakeLocation& oloc,
        double& netSaving) {
    const ConstMSEdgeVector& route = veh.getRoute().getEdges();
    const MSEdgeVector& main = oloc.main;
    const double vMax = veh.getMaxSpeed();
    const double prio = veh.getFloatParam(toString(SUMO_TAG_OVERTAKING_REROUTE) + ".prio", false, DEFAULT_PRIO_OVERTAKEN, false);
    MSVehicleControl& c = MSNet::getInstance()->getVehicleControl();
    for (MSVehicleControl::constVehIt it_veh = c.loadedVehBegin(); it_veh != c.loadedVehEnd(); ++it_veh) {
        const MSBaseVehicle* veh2 = dynamic_cast<const MSBaseVehicle*>((*it_veh).second);
        if (veh2->isOnRoad() && veh2->getMaxSpeed() > vMax) {
            const double arrivalDelay = veh2->getStopArrivalDelay();
            const double delay = MAX2(veh2->getStopDelay(), arrivalDelay == INVALID_DOUBLE ? 0 : arrivalDelay);
            if (delay > veh2->getFloatParam(toString(SUMO_TAG_OVERTAKING_REROUTE) + ".maxDelay", false, DEFAULT_MAXDELAY, false)) {
                continue;
            }
            const ConstMSEdgeVector& route2 = veh2->getRoute().getEdges();
            auto itOnMain2 = route2.end();
            int mainIndex = 0;
            for (const MSEdge* m : main) {
                itOnMain2 = std::find(veh2->getCurrentRouteEdge(), route2.end(), m);
                if (itOnMain2 != route2.end()) {
                    break;
                }
                mainIndex++;
            }
            if (itOnMain2 != route2.end() && itOnMain2 > veh2->getCurrentRouteEdge()) {
                auto itOnMain = mainStart + mainIndex;
                double timeToMain = 0;
                for (auto it = veh.getCurrentRouteEdge(); it != itOnMain; it++) {
                    timeToMain += (*it)->getMinimumTravelTime(&veh);
                }
                // veh2 may be anywhere on the current edge so we have to discount
                double timeToMain2 = -veh2->getEdge()->getMinimumTravelTime(veh2) * veh2->getPositionOnLane() / veh2->getEdge()->getLength();
                double timeToLastSignal2 = timeToMain2;
                for (auto it = veh2->getCurrentRouteEdge(); it != itOnMain2; it++) {
                    timeToMain2 += (*it)->getMinimumTravelTime(veh2);
                    auto signal = getRailSignal(*it);
                    if (signal) {
                        timeToLastSignal2 = timeToMain2;
#ifdef DEBUG_OVERTAKING
                        std::cout << "   lastBeforeMain2 " << signal->getID() << "\n";
#endif
                    }
                }
                double exitMainTime = timeToMain;
                double exitMainBlockTime2 = timeToMain2;
                double commonTime = 0;
                double commonTime2 = 0;
                int nCommon = 0;
                auto exitMain2 = itOnMain2;
                const MSRailSignal* firstAfterMain = nullptr;
                const MSEdge* common = nullptr;
                double vMinCommon = (*itOnMain)->getVehicleMaxSpeed(&veh);
                double vMinCommon2 = (*itOnMain2)->getVehicleMaxSpeed(veh2);
                while (itOnMain2 != route2.end()
                        && itOnMain != route.end()
                        && *itOnMain == *itOnMain2) {
                    common = *itOnMain;
                    commonTime += common->getMinimumTravelTime(&veh);
                    commonTime2 += common->getMinimumTravelTime(veh2);
                    vMinCommon = MIN2(vMinCommon, common->getVehicleMaxSpeed(&veh));
                    vMinCommon2 = MIN2(vMinCommon2, common->getVehicleMaxSpeed(veh2));
                    const bool onMain = nCommon < (int)main.size() - mainIndex;
                    if (onMain) {
                        exitMainTime = timeToMain + commonTime;
                    }
                    if (firstAfterMain == nullptr) {
                        exitMainBlockTime2 = timeToMain2 + commonTime2;
                    }
                    auto signal = getRailSignal(common);
                    if (signal) {
                        if (!onMain && firstAfterMain == nullptr) {
                            firstAfterMain = signal;
#ifdef DEBUG_OVERTAKING
                            std::cout << "   firstAfterMain " << signal->getID() << "\n";
#endif
                        }
                    }
                    nCommon++;
                    itOnMain++;
                    itOnMain2++;
                }
                const double vMaxLast = common->getVehicleMaxSpeed(&veh);
                const double vMaxLast2 = common->getVehicleMaxSpeed(veh2);
                commonTime += veh.getLength() / vMaxLast;
                exitMainBlockTime2 += veh2->getLength() / vMaxLast2;
                exitMain2 += MIN2(nCommon, (int)main.size() - mainIndex);
                double timeLoss2 = MAX2(0.0, timeToMain + veh.getLength() / oloc.siding.front()->getVehicleMaxSpeed(&veh) - timeToLastSignal2);
                const double saving = timeToMain + commonTime - (timeToMain2 + commonTime2) - timeLoss2;
                const double loss = exitMainBlockTime2 - exitMainTime;
                const double prio2 = veh2->getFloatParam(toString(SUMO_TAG_OVERTAKING_REROUTE) + ".prio", false, DEFAULT_PRIO_OVERTAKER, false);
                // losses from acceleration after stopping at a signal
                const double accelTimeLoss = loss > 0 ? 0.5 * vMinCommon / veh.getVehicleType().getCarFollowModel().getMaxAccel() : 0;
                const double accelTimeLoss2 = timeLoss2 > 0 ? 0.5 * vMinCommon2 / veh2->getVehicleType().getCarFollowModel().getMaxAccel() : 0;
                netSaving = prio2 * (saving - accelTimeLoss2) - prio * (loss + accelTimeLoss);
#ifdef DEBUG_OVERTAKING
                std::cout << SIMTIME << " veh=" << veh.getID() << " veh2=" << veh2->getID()
                    << " sidingStart=" << oloc.siding.front()->getID()
                    << " ttm=" << timeToMain << " ttm2=" << timeToMain2
                    << " nCommon=" << nCommon << " cT=" << commonTime << " cT2=" << commonTime2
                    << " em=" << exitMainTime << " emb2=" << exitMainBlockTime2
                    << " ttls2=" << timeToLastSignal2
                    << " saving=" << saving << " loss=" << loss
                    << " atl=" << accelTimeLoss << " atl2=" << accelTimeLoss2 << " tl2=" << timeLoss2
                    << " prio=" << prio << " prio2=" << prio2 << " netSaving=" << netSaving << "\n";
#endif
                if (netSaving > oloc.minSaving) {
                    MSRailSignal* s = findSignal(veh2->getCurrentRouteEdge(), exitMain2);
                    if (s != nullptr) {
                        return std::make_pair(veh2, s);
                    }
                }
            }
        }
    }
    return std::make_pair(nullptr, nullptr);
}


void
MSTriggeredRerouter::checkStopSwitch(MSBaseVehicle& ego, const MSTriggeredRerouter::RerouteInterval* def) {
    myBlockedStoppingPlaces.clear();
#ifdef DEBUG_REROUTER
    std::cout << SIMTIME << " " << getID() << " ego=" << ego.getID() << "\n";
#endif
    if (!ego.hasStops()) {
        return;
    }
    const MSStop& stop = ego.getNextStop();
    if (stop.reached || stop.joinTriggered || (stop.pars.arrival < 0 && stop.pars.until < 0)) {
        return;
    }
    MSStoppingPlace* cur = nullptr;
    for (MSStoppingPlace* sp : stop.getPlaces()) {
        for (auto item : def->stopAlternatives) {
            if (sp == item.first) {
                cur = sp;
                break;
            }
        }
    }
    if (cur == nullptr) {
        return;
    }
    std::vector<const SUMOVehicle*> stopped = cur->getStoppedVehicles();
#ifdef DEBUG_REROUTER
    std::cout << SIMTIME << " " << getID() << " ego=" << ego.getID() << " stopped=" << toString(stopped) << "\n";
#endif
    SUMOTime stoppedDuration = -1;
    if (stopped.empty()) {
        /// look upstream for vehicles that stop on this lane before ego arrives
        const MSLane& stopLane = cur->getLane();
        MSVehicleControl& c = MSNet::getInstance()->getVehicleControl();
        for (MSVehicleControl::constVehIt it_veh = c.loadedVehBegin(); it_veh != c.loadedVehEnd(); ++it_veh) {
            const MSBaseVehicle* veh = dynamic_cast<const MSBaseVehicle*>((*it_veh).second);
            if (veh->isOnRoad() && veh->hasStops()) {
                const MSStop& vehStop = veh->getNextStop();
                if (vehStop.pars.lane == stopLane.getID()) {
                    myBlockedStoppingPlaces.insert(cur);
                    if (veh->isStopped()) {
                        // stopped somewhere else on the same lane
                        stoppedDuration = MAX3((SUMOTime)0, stoppedDuration, veh->getStopDuration());
                    } else {
                        std::pair<double, double> timeDist = veh->estimateTimeToNextStop();
                        SUMOTime timeTo = TIME2STEPS(timeDist.first);
                        stoppedDuration = MAX3((SUMOTime)0, stoppedDuration, timeTo + vehStop.getMinDuration(SIMSTEP + timeTo));
                    }
                }
            }
        }
    } else {
        stoppedDuration = 0;
        for (const SUMOVehicle* veh : cur->getStoppedVehicles()) {
            stoppedDuration = MAX2(stoppedDuration, veh->getStopDuration());
        }
    }
    if (stoppedDuration < 0) {
        return;
    }
    /// @todo: consider time for conflict veh to leave the block
    const SUMOTime stopFree = SIMSTEP + stoppedDuration;
    const SUMOTime scheduledArrival = stop.pars.arrival >= 0 ? stop.pars.arrival : stop.pars.until - stop.pars.duration;
#ifdef DEBUG_REROUTER
    std::cout << SIMTIME << " " << getID() << " ego=" << ego.getID() << " stopFree=" << stopFree << " scheduledArrival=" << time2string(scheduledArrival) << "\n";
#endif
    if (stopFree < scheduledArrival) {
        // no conflict according to the schedule
        return;
    }
    const SUMOTime estimatedArrival = SIMSTEP + (stop.pars.arrival >= 0
                                      ? TIME2STEPS(ego.getStopArrivalDelay())
                                      : TIME2STEPS(ego.getStopDelay()) - stop.pars.duration);
#ifdef DEBUG_REROUTER
    std::cout << SIMTIME << " " << getID() << " ego=" << ego.getID() << " stopFree=" << stopFree << " estimatedArrival=" << time2string(estimatedArrival) << "\n";
#endif
    if (stopFree < estimatedArrival) {
        // no conflict when considering current delay
        return;
    }
    const std::vector<double> probs(def->stopAlternatives.size(), 1.);
    StoppingPlaceParamMap_t scores = {};
    bool newDestination;
    ConstMSEdgeVector newRoute;
    // @todo: consider future conflicts caused by rerouting
    // @todo: reject alternatives with large detour
    const MSStoppingPlace* alternative = rerouteStoppingPlace(nullptr, def->stopAlternatives, probs, ego, newDestination, newRoute, scores);
#ifdef DEBUG_REROUTER
    std::cout << SIMTIME << " " << getID() << " ego=" << ego.getID() << " alternative=" << Named::getIDSecure(alternative) << "\n";
#endif
    if (alternative != nullptr) {
        // @todo adapt plans of any riders
        //for (MSTransportable* p : ego.getPersons()) {
        //    p->rerouteParkingArea(ego.getNextParkingArea(), newParkingArea);
        //}

        if (newDestination) {
            // update arrival parameters
            SUMOVehicleParameter* newParameter = new SUMOVehicleParameter();
            *newParameter = ego.getParameter();
            newParameter->arrivalPosProcedure = ArrivalPosDefinition::GIVEN;
            newParameter->arrivalPos = alternative->getEndLanePosition();
            ego.replaceParameter(newParameter);
        }

        SUMOVehicleParameter::Stop newStop = stop.pars;
        newStop.lane = alternative->getLane().getID();
        newStop.startPos = alternative->getBeginLanePosition();
        newStop.endPos = alternative->getEndLanePosition();
        switch (alternative->getElement()) {
            case SUMO_TAG_PARKING_AREA:
                newStop.parkingarea = alternative->getID();
                break;
            case SUMO_TAG_CONTAINER_STOP:
                newStop.containerstop = alternative->getID();
                break;
            case SUMO_TAG_CHARGING_STATION:
                newStop.chargingStation = alternative->getID();
                break;
            case SUMO_TAG_OVERHEAD_WIRE_SEGMENT:
                newStop.overheadWireSegment = alternative->getID();
                break;
            case SUMO_TAG_BUS_STOP:
            case SUMO_TAG_TRAIN_STOP:
            default:
                newStop.busstop = alternative->getID();
        }
        std::string errorMsg;
        if (!ego.replaceStop(0, newStop, getID() + ":" + toString(SUMO_TAG_STATION_REROUTE), false, errorMsg)) {
            WRITE_WARNING("Vehicle '" + ego.getID() + "' at rerouter '" + getID()
                          + "' could not perform stationReroute to '" + alternative->getID()
                          + "' reason=" + errorMsg + ", time=" + time2string(MSNet::getInstance()->getCurrentTimeStep()) + ".");
        }
    }
}


MSRailSignal*
MSTriggeredRerouter::findSignal(ConstMSEdgeVector::const_iterator begin, ConstMSEdgeVector::const_iterator end) {
    auto it = end;
    do {
        it--;
        auto signal = getRailSignal(*it);
        if (signal != nullptr) {
            return signal;
        }
    } while (it != begin);
    return nullptr;
}


MSRailSignal*
MSTriggeredRerouter::getRailSignal(const MSEdge* edge) {
    if (edge->getToJunction()->getType() == SumoXMLNodeType::RAIL_SIGNAL) {
        for (const MSLink* link : edge->getLanes().front()->getLinkCont()) {
            if (link->getTLLogic() != nullptr) {
                return dynamic_cast<MSRailSignal*>(const_cast<MSTrafficLightLogic*>(link->getTLLogic()));
            }
        }
    }
    return nullptr;
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


void
MSTriggeredRerouter::resetClosedEdges(bool hasReroutingDevice, const SUMOTrafficObject& o) {
    // getRouterTT without prohibitions removes previous prohibitions
    if (o.isVehicle()) {
        hasReroutingDevice
        ? MSRoutingEngine::getRouterTT(o.getRNGIndex(), o.getVClass())
        : MSNet::getInstance()->getRouterTT(o.getRNGIndex());
    } else {
        hasReroutingDevice
        ? MSRoutingEngine::getIntermodalRouterTT(o.getRNGIndex())
        : MSNet::getInstance()->getIntermodalRouter(o.getRNGIndex(), 0);
    }
}

/****************************************************************************/
