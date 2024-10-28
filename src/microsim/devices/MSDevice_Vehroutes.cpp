/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2009-2024 German Aerospace Center (DLR) and others.
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
/// @file    MSDevice_Vehroutes.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Fri, 30.01.2009
///
// A device which collects info on the vehicle trip
/****************************************************************************/
#include <config.h>

#include <microsim/MSGlobals.h>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSRoute.h>
#include <microsim/MSStop.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSVehicleType.h>
#include <microsim/transportables/MSTransportableControl.h>
#include <utils/vehicle/SUMOVehicle.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice_String.h>
#include <utils/xml/SUMOSAXAttributes.h>
#include "MSDevice_Vehroutes.h"


// ===========================================================================
// static member variables
// ===========================================================================
bool MSDevice_Vehroutes::mySaveExits = false;
bool MSDevice_Vehroutes::myLastRouteOnly = false;
bool MSDevice_Vehroutes::myDUAStyle = false;
bool MSDevice_Vehroutes::myWriteCosts = false;
bool MSDevice_Vehroutes::mySorted = false;
bool MSDevice_Vehroutes::myIntendedDepart = false;
bool MSDevice_Vehroutes::myRouteLength = false;
bool MSDevice_Vehroutes::mySkipPTLines = false;
bool MSDevice_Vehroutes::myIncludeIncomplete = false;
bool MSDevice_Vehroutes::myWriteStopPriorEdges = false;
bool MSDevice_Vehroutes::myWriteInternal = false;
MSDevice_Vehroutes::StateListener MSDevice_Vehroutes::myStateListener;
MSDevice_Vehroutes::SortedRouteInfo MSDevice_Vehroutes::myRouteInfos;


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
void
MSDevice_Vehroutes::init() {
    const OptionsCont& oc = OptionsCont::getOptions();
    if (oc.isSet("vehroute-output")) {
        OutputDevice::createDeviceByOption("vehroute-output", "routes", "routes_file.xsd");
        mySaveExits = oc.getBool("vehroute-output.exit-times");
        myLastRouteOnly = oc.getBool("vehroute-output.last-route");
        myDUAStyle = oc.getBool("vehroute-output.dua");
        myWriteCosts = oc.getBool("vehroute-output.cost");
        mySorted = myDUAStyle || oc.getBool("vehroute-output.sorted");
        myIntendedDepart = oc.getBool("vehroute-output.intended-depart");
        myRouteLength = oc.getBool("vehroute-output.route-length");
        mySkipPTLines = oc.getBool("vehroute-output.skip-ptlines");
        myIncludeIncomplete = oc.getBool("vehroute-output.incomplete");
        myWriteStopPriorEdges = oc.getBool("vehroute-output.stop-edges");
        myWriteInternal = oc.getBool("vehroute-output.internal");
        MSNet::getInstance()->addVehicleStateListener(&myStateListener);
        myRouteInfos.routeOut = &OutputDevice::getDeviceByOption("vehroute-output");
    }
}


void
MSDevice_Vehroutes::insertOptions(OptionsCont& oc) {
    oc.addOptionSubTopic("Vehroutes Device");
    insertDefaultAssignmentOptions("vehroute", "Vehroutes Device", oc);
}


MSDevice_Vehroutes*
MSDevice_Vehroutes::buildVehicleDevices(SUMOVehicle& v, std::vector<MSVehicleDevice*>& into, int maxRoutes) {
    if (maxRoutes < std::numeric_limits<int>::max()) {
        return new MSDevice_Vehroutes(v, "vehroute_" + v.getID(), maxRoutes);
    }
    if (mySkipPTLines && v.getParameter().line != "") {
        return nullptr;
    }
    OptionsCont& oc = OptionsCont::getOptions();
    if (equippedByDefaultAssignmentOptions(oc, "vehroute", v, oc.isSet("vehroute-output"))) {
        if (myLastRouteOnly) {
            maxRoutes = 0;
        }
        myStateListener.myDevices[&v] = new MSDevice_Vehroutes(v, "vehroute_" + v.getID(), maxRoutes);
        into.push_back(myStateListener.myDevices[&v]);
        return myStateListener.myDevices[&v];
    }
    return nullptr;
}


// ---------------------------------------------------------------------------
// MSDevice_Vehroutes::StateListener-methods
// ---------------------------------------------------------------------------
void
MSDevice_Vehroutes::StateListener::vehicleStateChanged(const SUMOVehicle* const vehicle, MSNet::VehicleState to, const std::string& info) {
    if (to == MSNet::VehicleState::NEWROUTE) {
        const auto& deviceEntry = myDevices.find(vehicle);
        if (deviceEntry != myDevices.end()) {
            deviceEntry->second->addRoute(info);
        }
    }
}


// ---------------------------------------------------------------------------
// MSDevice_Vehroutes-methods
// ---------------------------------------------------------------------------
MSDevice_Vehroutes::MSDevice_Vehroutes(SUMOVehicle& holder, const std::string& id, int maxRoutes) :
    MSVehicleDevice(holder, id),
    myCurrentRoute(holder.getRoutePtr()),
    myMaxRoutes(maxRoutes),
    myLastSavedAt(nullptr),
    myLastRouteIndex(-1),
    myDepartLane(-1),
    myDepartPos(-1),
    myDepartSpeed(-1),
    myDepartPosLat(0),
    myStopOut(2) {
}


MSDevice_Vehroutes::~MSDevice_Vehroutes() {
    myStateListener.myDevices.erase(&myHolder);
}


bool
MSDevice_Vehroutes::notifyEnter(SUMOTrafficObject& veh, MSMoveReminder::Notification reason, const MSLane* enteredLane) {
    if (reason == MSMoveReminder::NOTIFICATION_DEPARTED) {
        if (mySorted && myStateListener.myDevices[static_cast<SUMOVehicle*>(&veh)] == this) {
            const SUMOTime departure = myIntendedDepart ? myHolder.getParameter().depart : MSNet::getInstance()->getCurrentTimeStep();
            myRouteInfos.departureCounts[departure]++;
        }
        if (!MSGlobals::gUseMesoSim) {
            const MSVehicle& vehicle = static_cast<MSVehicle&>(veh);
            myDepartLane = vehicle.getLane()->getIndex();
            myDepartPosLat = vehicle.getLateralPositionOnLane();
        }
        myDepartSpeed = veh.getSpeed();
        myDepartPos = veh.getPositionOnLane();
    }
    if (myWriteStopPriorEdges) {
        if (MSGlobals::gUseMesoSim) {
            const MSEdge* e = veh.getEdge();
            if (myPriorEdges.empty() || myPriorEdges.back() != e) {
                myPriorEdges.push_back(e);
            }
        } else {
            myPriorEdges.push_back(&enteredLane->getEdge());
        }
    }
    myLastRouteIndex = myHolder.getRoutePosition();
    return true;
}


bool
MSDevice_Vehroutes::notifyLeave(SUMOTrafficObject& veh, double /*lastPos*/, MSMoveReminder::Notification reason, const MSLane* /* enteredLane */) {
    if (mySaveExits && reason != NOTIFICATION_LANE_CHANGE && reason != NOTIFICATION_PARKING && reason != NOTIFICATION_SEGMENT) {
        const MSEdge* edge = myWriteInternal ? dynamic_cast<MSBaseVehicle&>(veh).getCurrentEdge() : veh.getEdge();
        if (myLastSavedAt != edge) {
            myExits.push_back(MSNet::getInstance()->getCurrentTimeStep());
            myLastSavedAt = edge;
        }
    }
    return true;
}


void
MSDevice_Vehroutes::notifyStopEnded() {
    SUMOVehicleParameter::Stop stop = myHolder.getStops().front().pars;
    const bool closeLater = myWriteStopPriorEdges || mySaveExits;
    if (mySaveExits) {
        // prevent duplicate output
        stop.parametersSet &=  ~(STOP_STARTED_SET | STOP_ENDED_SET);
    }
    stop.write(myStopOut, !closeLater);
    if (myWriteStopPriorEdges) {
        // calculate length
        double priorEdgesLength = 0;
        for (int i = 0; i < (int)myPriorEdges.size(); i++) {
            if (i == 0) {
                priorEdgesLength += myPriorEdges.at(i)->getLength();
            } else if (myPriorEdges.at(i)->getID() != myPriorEdges.at(i - 1)->getID()) {
                priorEdgesLength += myPriorEdges.at(i)->getLength();
            }
        }
        myStopOut.writeAttr("priorEdges", myPriorEdges);
        myPriorEdges.clear();
        myStopOut.writeAttr("priorEdgesLength", priorEdgesLength);
    }
    if (mySaveExits) {
        myStopOut.writeAttr(SUMO_ATTR_STARTED, time2string(stop.started));
        myStopOut.writeAttr(SUMO_ATTR_ENDED, stop.ended < 0 ? "-1" : time2string(stop.ended));
    }
    if (closeLater) {
        myStopOut.closeTag();
    }
}


void
MSDevice_Vehroutes::writeXMLRoute(OutputDevice& os, int index) const {
    if (index == 0 && !myIncludeIncomplete && myReplacedRoutes[index].route->size() == 2 &&
            myReplacedRoutes[index].route->getEdges().front()->isTazConnector() &&
            myReplacedRoutes[index].route->getEdges().back()->isTazConnector()) {
        return;
    }
    // check if a previous route shall be written
    //std::cout << " writeXMLRoute index=" << index << " numReroutes=" << myHolder.getNumberReroutes() << "\n";
    const int routesToSkip = myHolder.getParameter().wasSet(VEHPARS_FORCE_REROUTE) ? 1 : 0;
    os.openTag(SUMO_TAG_ROUTE);
    if (index >= 0) {
        assert((int)myReplacedRoutes.size() > index);
        if (myDUAStyle || myWriteCosts) {
            os.writeAttr(SUMO_ATTR_COST, myReplacedRoutes[index].route->getCosts());
        }
        if (myWriteCosts) {
            os.writeAttr(SUMO_ATTR_SAVINGS, myReplacedRoutes[index].route->getSavings());
        }
        // write edge on which the vehicle was when the route was valid
        os.writeAttr("replacedOnEdge", (myReplacedRoutes[index].edge ?
                                        myReplacedRoutes[index].edge->getID() : ""));
        if (myReplacedRoutes[index].lastRouteIndex > 0) {
            // do not write the default
            os.writeAttr(SUMO_ATTR_REPLACED_ON_INDEX, myReplacedRoutes[index].lastRouteIndex);
        }
        // write the reason for replacement
        os.writeAttr("reason", myReplacedRoutes[index].info);

        // write the time at which the route was replaced
        os.writeAttr(SUMO_ATTR_REPLACED_AT_TIME, time2string(myReplacedRoutes[index].time));
        os.writeAttr(SUMO_ATTR_PROB, "0");
        OutputDevice_String edgesD;
        // always write the part that was actually driven and the rest of the current route that wasn't yet driven
        int start = 0;
        for (int i = routesToSkip; i < index; i++) {
            if (myReplacedRoutes[i].edge != nullptr) {
                int end = myReplacedRoutes[i].lastRouteIndex;
                myReplacedRoutes[i].route->writeEdgeIDs(edgesD, start, end, myWriteInternal, myHolder.getVClass());
            }
            start = myReplacedRoutes[i].newRouteIndex;
        }
        myReplacedRoutes[index].route->writeEdgeIDs(edgesD, start, -1, myWriteInternal, myHolder.getVClass());
        std::string edgesS = edgesD.getString();
        edgesS.pop_back(); // remove last ' '
        os.writeAttr(SUMO_ATTR_EDGES, edgesS);
        if (myRouteLength) {
            const bool includeInternalLengths = MSGlobals::gUsingInternalLanes && MSNet::getInstance()->hasInternalLinks();
            ConstMSRoutePtr route = myReplacedRoutes[index].route;
            const double routeLength = route->getDistanceBetween(myHolder.getDepartPos(), route->getEdges().back()->getLength(),
                                       route->begin(), route->end(), includeInternalLengths);
            os.writeAttr("routeLength", routeLength);
        }
    } else {
        if (myDUAStyle || myWriteCosts) {
            os.writeAttr(SUMO_ATTR_COST, myHolder.getRoute().getCosts());
        }
        if (myWriteCosts) {
            os.writeAttr(SUMO_ATTR_SAVINGS, myHolder.getRoute().getSavings());
        }
        OutputDevice_String edgesD;
        int numWritten = 0;
        int start = 0;
        if (myHolder.getNumberReroutes() > 0) {
            assert((int)myReplacedRoutes.size() <= myHolder.getNumberReroutes());
            for (int i = routesToSkip; i < (int)myReplacedRoutes.size(); i++) {
                if (myReplacedRoutes[i].edge != nullptr) {
                    int end = myReplacedRoutes[i].lastRouteIndex;
                    numWritten += myReplacedRoutes[i].route->writeEdgeIDs(edgesD, start, end, myWriteInternal, myHolder.getVClass());
                }
                start = myReplacedRoutes[i].newRouteIndex;
            }
        }
        numWritten += myCurrentRoute->writeEdgeIDs(edgesD, start, -1, myWriteInternal, myHolder.getVClass());
        std::string edgesS = edgesD.getString();
        edgesS.pop_back(); // remove last ' '
        os.writeAttr(SUMO_ATTR_EDGES, edgesS);

        if (mySaveExits) {
            std::vector<std::string> exits;
            for (SUMOTime t : myExits) {
                exits.push_back(time2string(t));
            }
            assert(numWritten >= (int)myExits.size());
            std::vector<std::string> missing(numWritten - (int)myExits.size(), "-1");
            exits.insert(exits.end(), missing.begin(), missing.end());
            os.writeAttr(SUMO_ATTR_EXITTIMES, exits);
        }
    }
    os.closeTag();
}


void
MSDevice_Vehroutes::generateOutput(OutputDevice* /*tripinfoOut*/) const {
    writeOutput(true);
}


void
MSDevice_Vehroutes::writeOutput(const bool hasArrived) const {
    const OptionsCont& oc = OptionsCont::getOptions();
    OutputDevice& routeOut = OutputDevice::getDeviceByOption("vehroute-output");
    OutputDevice_String od(1);
    SUMOVehicleParameter tmp = myHolder.getParameter();
    tmp.depart = myIntendedDepart ? myHolder.getParameter().depart : myHolder.getDeparture();
    if (!MSGlobals::gUseMesoSim) {
        if (tmp.wasSet(VEHPARS_DEPARTLANE_SET)) {
            tmp.departLaneProcedure = DepartLaneDefinition::GIVEN;
            tmp.departLane = myDepartLane;
        }
        if (tmp.wasSet(VEHPARS_DEPARTPOSLAT_SET)) {
            tmp.departPosLatProcedure = (tmp.departPosLatProcedure == DepartPosLatDefinition::RANDOM
                                         ? DepartPosLatDefinition::GIVEN_VEHROUTE
                                         : DepartPosLatDefinition::GIVEN);
            tmp.departPosLat = myDepartPosLat;
        }
    }
    if (tmp.wasSet(VEHPARS_DEPARTPOS_SET)) {
        tmp.departPosProcedure = ((tmp.departPosProcedure != DepartPosDefinition::GIVEN
                                   && tmp.departPosProcedure != DepartPosDefinition::STOP)
                                  ? DepartPosDefinition::GIVEN_VEHROUTE
                                  : DepartPosDefinition::GIVEN);
        tmp.departPos = myDepartPos;
    }
    if (tmp.wasSet(VEHPARS_DEPARTSPEED_SET)) {
        tmp.departSpeedProcedure = ((tmp.departSpeedProcedure != DepartSpeedDefinition::GIVEN
                                     && tmp.departSpeedProcedure != DepartSpeedDefinition::LIMIT)
                                    ? DepartSpeedDefinition::GIVEN_VEHROUTE
                                    : DepartSpeedDefinition::GIVEN);
        tmp.departSpeed = myDepartSpeed;
    }
    if (oc.getBool("vehroute-output.speedfactor") ||
            (oc.isDefault("vehroute-output.speedfactor") && tmp.wasSet(VEHPARS_DEPARTSPEED_SET))) {
        tmp.parametersSet |= VEHPARS_SPEEDFACTOR_SET;
        tmp.speedFactor = myHolder.getChosenSpeedFactor();
    }

    const std::string typeID = myHolder.getVehicleType().getID() != DEFAULT_VTYPE_ID ? myHolder.getVehicleType().getID() : "";
    tmp.write(od, oc, SUMO_TAG_VEHICLE, typeID);
    if (hasArrived) {
        od.writeAttr("arrival", time2string(MSNet::getInstance()->getCurrentTimeStep()));
    }
    if (myRouteLength) {
        const bool includeInternalLengths = MSGlobals::gUsingInternalLanes && MSNet::getInstance()->hasInternalLinks();
        const double finalPos = hasArrived ? myHolder.getArrivalPos() : myHolder.getPositionOnLane();
        const double routeLength = myHolder.getRoute().getDistanceBetween(myHolder.getDepartPos(), finalPos,
                                   myHolder.getRoute().begin(), myHolder.getCurrentRouteEdge(), includeInternalLengths);
        od.writeAttr("routeLength", routeLength);
    }
    if (myDUAStyle) {
        const RandomDistributor<ConstMSRoutePtr>* const routeDist = MSRoute::distDictionary("!" + myHolder.getID());
        if (routeDist != nullptr) {
            const std::vector<ConstMSRoutePtr>& routes = routeDist->getVals();
            unsigned index = 0;
            while (index < routes.size() && routes[index] != myCurrentRoute) {
                ++index;
            }
            od.openTag(SUMO_TAG_ROUTE_DISTRIBUTION).writeAttr(SUMO_ATTR_LAST, index);
            const std::vector<double>& probs = routeDist->getProbs();
            for (int i = 0; i < (int)routes.size(); ++i) {
                od.setPrecision();
                od.openTag(SUMO_TAG_ROUTE);
                od.writeAttr(SUMO_ATTR_COST, routes[i]->getCosts());
                if (myWriteCosts) {
                    od.writeAttr(SUMO_ATTR_SAVINGS, routes[i]->getSavings());
                }
                od.setPrecision(8);
                od.writeAttr(SUMO_ATTR_PROB, probs[i]);
                od.setPrecision();
                OutputDevice_String edgesD;
                routes[i]->writeEdgeIDs(edgesD, 0, -1, myWriteInternal, myHolder.getVClass());
                std::string edgesS = edgesD.getString();
                edgesS.pop_back(); // remove last ' '
                od.writeAttr(SUMO_ATTR_EDGES, edgesS);
                od.closeTag();
            }
            od.closeTag();
        } else {
            writeXMLRoute(od);
        }
    } else {
        std::string dummyMsg;
        const int routesToSkip = (myHolder.getParameter().wasSet(VEHPARS_FORCE_REROUTE)
                                  && !myIncludeIncomplete
                                  && myReplacedRoutes.size() > 0
                                  && !myHolder.hasValidRoute(dummyMsg, myReplacedRoutes[0].route) ? 1 : 0);
        if ((int)myReplacedRoutes.size() > routesToSkip) {
            od.openTag(SUMO_TAG_ROUTE_DISTRIBUTION);
            for (int i = routesToSkip; i < (int)myReplacedRoutes.size(); ++i) {
                writeXMLRoute(od, i);
            }
            writeXMLRoute(od);
            od.closeTag();
        } else {
            writeXMLRoute(od);
        }
    }
    od << myStopOut.getString();
    myHolder.getParameter().writeParams(od);
    od.closeTag();
    od.lf();
    if (mySorted) {
        // numerical id reflects loading order
        writeSortedOutput(&myRouteInfos, tmp.depart, toString(myHolder.getNumericalID()), od.getString());
    } else {
        routeOut << od.getString();
    }
}


ConstMSRoutePtr
MSDevice_Vehroutes::getRoute(int index) const {
    if (index < (int)myReplacedRoutes.size()) {
        return myReplacedRoutes[index].route;
    } else {
        return nullptr;
    }
}


void
MSDevice_Vehroutes::addRoute(const std::string& info) {
    if (myMaxRoutes > 0) {
        //std::cout << SIMTIME << " " << getID() << " departed=" << myHolder.hasDeparted() << " lastIndex=" << myLastRouteIndex << " start=" << myHolder.getRoutePosition() << "\n";
        myReplacedRoutes.push_back(RouteReplaceInfo(
                                       myHolder.hasDeparted() ?  myHolder.getEdge() : nullptr,
                                       MSNet::getInstance()->getCurrentTimeStep(), myCurrentRoute, info,
                                       myLastRouteIndex,
                                       myHolder.hasDeparted() ? myHolder.getRoutePosition() : 0));
        if ((int)myReplacedRoutes.size() > myMaxRoutes) {
            myReplacedRoutes.erase(myReplacedRoutes.begin());
        }
    }
    myCurrentRoute = myHolder.getRoutePtr();
}


void
MSDevice_Vehroutes::writePendingOutput(const bool includeUnfinished) {
    MSNet* const net = MSNet::getInstance();

    if (!includeUnfinished) {
        if (mySorted) {
            for (const auto& routeInfo : myRouteInfos.routeXML) {
                for (const auto& rouXML : routeInfo.second) {
                    (*myRouteInfos.routeOut) << rouXML.second;
                }
            }
            if (net->hasPersons()) {
                const SortedRouteInfo& personRouteInfos = net->getPersonControl().getRouteInfo();
                if (personRouteInfos.routeOut != myRouteInfos.routeOut) {
                    for (const auto& routeInfo : personRouteInfos.routeXML) {
                        for (const auto& rouXML : routeInfo.second) {
                            (*personRouteInfos.routeOut) << rouXML.second;
                        }
                    }
                }
            }
        }
        return;
    }
    for (const auto& it : myStateListener.myDevices) {
        if (it.first->hasDeparted()) {
            if (it.first->isStopped()) {
                it.second->notifyStopEnded();
            }
            it.second->writeOutput(false);
        }
    }
    // unfinished persons
    if (net->hasPersons()) {
        MSTransportableControl& pc = net->getPersonControl();
        while (pc.loadedBegin() != pc.loadedEnd()) {
            pc.erase(pc.loadedBegin()->second);
        }
    }
}


void
MSDevice_Vehroutes::registerTransportableDepart(SUMOTime depart) {
    myRouteInfos.departureCounts[depart]++;
}


void
MSDevice_Vehroutes::writeSortedOutput(MSDevice_Vehroutes::SortedRouteInfo* routeInfo, SUMOTime depart, const std::string& id, const std::string& xmlOutput) {
    if (routeInfo->routeOut == myRouteInfos.routeOut) {
        routeInfo = &myRouteInfos;
    }
    routeInfo->routeXML[depart][id] = xmlOutput;
    routeInfo->departureCounts[depart]--;
    std::map<const SUMOTime, int>::iterator it = routeInfo->departureCounts.begin();
    while (it != routeInfo->departureCounts.end() && it->second == 0) {
        for (const auto& rouXML : routeInfo->routeXML[it->first]) {
            (*routeInfo->routeOut) << rouXML.second;
        }
        routeInfo->routeXML.erase(it->first);
        it = routeInfo->departureCounts.erase(it);
    }
}


void
MSDevice_Vehroutes::saveState(OutputDevice& out) const {
    out.openTag(SUMO_TAG_DEVICE);
    out.writeAttr(SUMO_ATTR_ID, getID());
    std::vector<std::string> internals;
    if (!MSGlobals::gUseMesoSim) {
        internals.push_back(toString(myDepartLane));
        internals.push_back(toString(myDepartPosLat));
    }
    internals.push_back(toString(myDepartSpeed));
    internals.push_back(toString(myDepartPos));
    internals.push_back(toString(myReplacedRoutes.size()));
    for (int i = 0; i < (int)myReplacedRoutes.size(); ++i) {
        const std::string replacedOnEdge = myReplacedRoutes[i].edge == nullptr ? "!NULL" : myReplacedRoutes[i].edge->getID();
        internals.push_back(replacedOnEdge);
        internals.push_back(toString(myReplacedRoutes[i].time));
        internals.push_back(myReplacedRoutes[i].route->getID());
        internals.push_back(myReplacedRoutes[i].info);
        internals.push_back(toString(myReplacedRoutes[i].lastRouteIndex));
        internals.push_back(toString(myReplacedRoutes[i].newRouteIndex));
    }
    out.writeAttr(SUMO_ATTR_STATE, toString(internals));
    if (mySaveExits && myExits.size() > 0) {
        out.writeAttr(SUMO_ATTR_EXITTIMES, myExits);
        out.writeAttr(SUMO_ATTR_EDGE, myLastSavedAt->getID());
    }
    out.closeTag();
}


void
MSDevice_Vehroutes::loadState(const SUMOSAXAttributes& attrs) {
    std::istringstream bis(attrs.getString(SUMO_ATTR_STATE));
    if (!MSGlobals::gUseMesoSim) {
        bis >> myDepartLane;
        bis >> myDepartPosLat;
    }
    bis >> myDepartSpeed;
    bis >> myDepartPos;
    int size;
    bis >> size;
    for (int i = 0; i < size; ++i) {
        std::string edgeID;
        SUMOTime time;
        std::string routeID;
        std::string info;
        int lastIndex;
        int newIndex;
        bis >> edgeID;
        bis >> time;
        bis >> routeID;
        bis >> info;
        bis >> lastIndex;
        bis >> newIndex;

        ConstMSRoutePtr route = MSRoute::dictionary(routeID);
        if (route != nullptr) {
            myReplacedRoutes.push_back(RouteReplaceInfo(MSEdge::dictionary(edgeID), time, route, info, lastIndex, newIndex));
        }
    }
    if (mySaveExits && attrs.hasAttribute(SUMO_ATTR_EXITTIMES)) {
        bool ok = true;
        for (const std::string& t : attrs.get<std::vector<std::string> >(SUMO_ATTR_EXITTIMES, nullptr, ok)) {
            myExits.push_back(StringUtils::toLong(t));
        }
        if (attrs.hasAttribute(SUMO_ATTR_EDGE)) {
            myLastSavedAt = MSEdge::dictionary(attrs.getString(SUMO_ATTR_EDGE));
        }
    }
}


/****************************************************************************/
