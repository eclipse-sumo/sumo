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
/// @file    MSStageTrip.cpp
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Wed, 1 Jun 2022
///
// An intermodal routing request (to be transformed into a sequence of walks and rides)
/****************************************************************************/
#include <config.h>

#include <utils/common/StringTokenizer.h>
#include <utils/geom/GeomHelper.h>
#include <utils/options/OptionsCont.h>
#include <utils/vehicle/SUMOVehicleParameter.h>
#include <utils/router/PedestrianRouter.h>
#include <utils/router/IntermodalRouter.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSNet.h>
#include <microsim/MSStoppingPlace.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/transportables/MSStageDriving.h>
#include <microsim/transportables/MSStageWaiting.h>
#include <microsim/transportables/MSStageWalking.h>
#include <microsim/transportables/MSTransportable.h>
#include <microsim/transportables/MSPerson.h>
#include <microsim/transportables/MSStageTrip.h>


// ===========================================================================
// method definitions
// ===========================================================================

/* -------------------------------------------------------------------------
* MSStageTrip - methods
* ----------------------------------------------------------------------- */
MSStageTrip::MSStageTrip(const MSEdge* origin, MSStoppingPlace* fromStop,
                         const MSEdge* destination, MSStoppingPlace* toStop,
                         const SUMOTime duration, const SVCPermissions modeSet,
                         const std::string& vTypes, const double speed, const double walkFactor,
                         const std::string& group,
                         const double departPosLat, const bool hasArrivalPos, const double arrivalPos):
    MSStage(MSStageType::TRIP, destination, toStop, arrivalPos, 0.0, group),
    myOrigin(origin),
    myOriginStop(fromStop),
    myDuration(duration),
    myModeSet(modeSet),
    myVTypes(vTypes),
    mySpeed(speed),
    myWalkFactor(walkFactor),
    myDepartPosLat(departPosLat),
    myHaveArrivalPos(hasArrivalPos) {
}


MSStageTrip::~MSStageTrip() {}

MSStage*
MSStageTrip::clone() const {
    MSStage* const clon = new MSStageTrip(myOrigin, const_cast<MSStoppingPlace*>(myOriginStop),
                                          myDestination, myDestinationStop, myDuration,
                                          myModeSet, myVTypes, mySpeed, myWalkFactor, myGroup,
                                          myDepartPosLat, myHaveArrivalPos, myArrivalPos);
    clon->setParameters(*this);
    return clon;
}


Position
MSStageTrip::getPosition(SUMOTime /* now */) const {
    // may be called concurrently while the trip is still being routed
    return getEdgePosition(myOrigin, myDepartPos, ROADSIDE_OFFSET * (MSGlobals::gLefthand ? -1 : 1));
}


double
MSStageTrip::getAngle(SUMOTime /* now */) const {
    // may be called concurrently while the trip is still being routed
    return getEdgeAngle(myOrigin, myDepartPos) + M_PI / 2 * (MSGlobals::gLefthand ? -1 : 1);
}


const MSEdge*
MSStageTrip::getEdge() const {
    return myOrigin;
}


double
MSStageTrip::getEdgePos(SUMOTime /* now */) const {
    return myDepartPos;
}


std::vector<SUMOVehicle*>
MSStageTrip::getVehicles(MSVehicleControl& vehControl, MSTransportable* transportable, const MSEdge* origin) {
    std::vector<SUMOVehicleParameter*> pars;
    for (StringTokenizer st(myVTypes); st.hasNext();) {
        pars.push_back(new SUMOVehicleParameter());
        pars.back()->vtypeid = st.next();
        pars.back()->parametersSet |= VEHPARS_VTYPE_SET;
        pars.back()->departProcedure = DepartDefinition::TRIGGERED;
        pars.back()->id = transportable->getID() + "_" + toString(pars.size() - 1);
    }
    if (pars.empty()) {
        if ((myModeSet & SVC_PASSENGER) != 0) {
            pars.push_back(new SUMOVehicleParameter());
            pars.back()->id = transportable->getID() + "_0";
            pars.back()->departProcedure = DepartDefinition::TRIGGERED;
        }
        if ((myModeSet & SVC_TAXI) != 0) {
            pars.push_back(new SUMOVehicleParameter());
            pars.back()->vtypeid = DEFAULT_TAXITYPE_ID;
            pars.back()->id = transportable->getID() + "_taxi";
            pars.back()->line = "taxi";
        }
        if ((myModeSet & SVC_BICYCLE) != 0) {
            pars.push_back(new SUMOVehicleParameter());
            pars.back()->vtypeid = DEFAULT_BIKETYPE_ID;
            pars.back()->id = transportable->getID() + "_b0";
            pars.back()->departProcedure = DepartDefinition::TRIGGERED;
        }
    }
    ConstMSRoutePtr const routeDummy = std::make_shared<MSRoute>(transportable->getID() + "_0", ConstMSEdgeVector({ origin }), false, nullptr, StopParVector());
    std::vector<SUMOVehicle*> result;
    for (SUMOVehicleParameter* vehPar : pars) {
        const bool isTaxi = vehPar->vtypeid == DEFAULT_TAXITYPE_ID && vehPar->line == "taxi";
        if (myDepartPos != 0) {
            vehPar->departPosProcedure = DepartPosDefinition::GIVEN;
            vehPar->departPos = myDepartPos;
            vehPar->parametersSet |= VEHPARS_DEPARTPOS_SET;
        }
        vehPar->parametersSet |= VEHPARS_ARRIVALPOS_SET;
        vehPar->arrivalPosProcedure = ArrivalPosDefinition::GIVEN;
        vehPar->parametersSet |= VEHPARS_ARRIVALSPEED_SET;
        vehPar->arrivalSpeedProcedure = ArrivalSpeedDefinition::GIVEN;
        vehPar->arrivalSpeed = 0;
        MSVehicleType* type = vehControl.getVType(vehPar->vtypeid);
        if (type->getVehicleClass() != SVC_IGNORING && (origin->getPermissions() & type->getVehicleClass()) == 0 && !isTaxi) {
            WRITE_WARNINGF(TL("Ignoring vehicle type '%' when routing person '%' because it is not allowed on the start edge."), type->getID(), transportable->getID());
            delete vehPar;
        } else {
            result.push_back(vehControl.buildVehicle(vehPar, routeDummy, type, !MSGlobals::gCheckRoutes));
        }
    }
    if (result.empty()) {
        // walking only but may use access infrastructure
        result.push_back(nullptr);
    }
    return result;
}


const std::string
MSStageTrip::reroute(const SUMOTime time, MSTransportableRouter& router, MSTransportable* const transportable,
                     MSStage* previous, const MSEdge* origin, const MSEdge* destination, std::vector<MSStage*>& stages) {
    if (origin->isTazConnector() && origin->getSuccessors().size() == 0) {
        // previous stage ended at a taz sink-edge
        origin = transportable->getNextStage(-1)->getDestination();
    }
    MSVehicleControl& vehControl = MSNet::getInstance()->getVehicleControl();
    double minCost = std::numeric_limits<double>::max();
    std::vector<MSTransportableRouter::TripItem> minResult;
    SUMOVehicle* minVehicle = nullptr;
    for (SUMOVehicle* vehicle : getVehicles(vehControl, transportable, origin)) {
        std::vector<MSTransportableRouter::TripItem> result;
        double departPos = previous->getArrivalPos();
        MSStoppingPlace* const prevStop = previous->getDestinationStop();
        if (MSGlobals::gUseMesoSim && prevStop != nullptr) {
            departPos = (prevStop->getBeginLanePosition() + prevStop->getEndLanePosition()) / 2.;
        }
        if (router.compute(origin, destination, departPos, prevStop == nullptr ? "" : prevStop->getID(),
                           myArrivalPos, myDestinationStop == nullptr ? "" : myDestinationStop->getID(),
                           transportable->getMaxSpeed() * myWalkFactor, vehicle, transportable->getVTypeParameter(), myModeSet, time, result)) {
            double totalCost = 0;
            for (const MSTransportableRouter::TripItem& item : result) {
                totalCost += item.cost;
            }
            if (totalCost < minCost) {
                minCost = totalCost;
                minResult.swap(result);
                std::swap(minVehicle, vehicle);
            }
        }
        if (vehicle != nullptr) {
            vehControl.deleteVehicle(vehicle, true);
            vehControl.discountRoutingVehicle();
        }
    }
    if (minCost != std::numeric_limits<double>::max()) {
        const bool isTaxi = minVehicle != nullptr && minVehicle->getParameter().vtypeid == DEFAULT_TAXITYPE_ID && minVehicle->getParameter().line == "taxi";
        bool carUsed = false;
        for (std::vector<MSTransportableRouter::TripItem>::iterator it = minResult.begin(); it != minResult.end(); ++it) {
            if (!it->edges.empty()) {
                MSStoppingPlace* bs = MSNet::getInstance()->getStoppingPlace(it->destStop);
                double localArrivalPos = bs != nullptr ? bs->getAccessPos(it->edges.back()) : it->arrivalPos;
                const MSEdge* const first = it->edges.front();
                const MSEdge* const rideOrigin = origin->isTazConnector() && stages.empty() ? first : nullptr;
                if (it + 1 == minResult.end() && myHaveArrivalPos) {
                    localArrivalPos = myArrivalPos;
                }
                if (it->line == "") {
                    // determine walk departPos
                    double depPos = previous->getArrivalPos();
                    if (previous->getDestinationStop() != nullptr) {
                        depPos = previous->getDestinationStop()->getAccessPos(first, first->getLanes()[0]->getRNG());
                    } else if (origin->isTazConnector()) {
                        // walk the whole length of the first edge
                        if (std::find(first->getPredecessors().begin(), first->getPredecessors().end(), origin) != first->getPredecessors().end()) {
                            depPos = 0;
                        } else {
                            depPos = first->getLength();
                        }
                    } else if (previous->getDestination() != first) {
                        if ((previous->getDestination()->getToJunction() == first->getToJunction())
                                || (previous->getDestination()->getFromJunction() == first->getToJunction())) {
                            depPos = first->getLength();
                        } else {
                            depPos = 0.;
                        }
                    }
                    if (destination->isTazConnector()) {
                        // walk the whole length of the last edge
                        const MSEdge* last = it->edges.back();
                        if (std::find(last->getSuccessors().begin(), last->getSuccessors().end(), destination) != last->getSuccessors().end()) {
                            localArrivalPos = last->getLength();
                        } else {
                            localArrivalPos = 0;
                        }
                    }
                    previous = new MSStageWalking(transportable->getID(), it->edges, bs, myDuration, mySpeed, depPos, localArrivalPos, myDepartPosLat);
                    previous->setParameters(*this);
                    previous->setCosts(it->cost);
                    previous->setTrip(this);
                    stages.push_back(previous);
                } else if (isTaxi && it->line == minVehicle->getID()) {
                    const ConstMSEdgeVector& prevEdges = previous->getEdges();
                    if (prevEdges.size() >= 2 && previous->getDestinationStop() == nullptr) {
                        // determine walking direction and let the previous
                        // stage end after entering its final edge
                        const MSEdge* last = prevEdges.back();
                        const MSEdge* prev = prevEdges[prevEdges.size() - 2];
                        if (last->getFromJunction() == prev->getToJunction() || prev->getFromJunction() == last->getFromJunction()) {
                            previous->setArrivalPos(MIN2(last->getLength(), 10.0));
                        } else {
                            previous->setArrivalPos(MAX2(0.0, last->getLength() - 10));
                        }
                    }
                    previous = new MSStageDriving(rideOrigin, it->edges.back(), bs, localArrivalPos, 0.0, std::vector<std::string>({ "taxi" }), myGroup);
                    previous->setParameters(*this);
                    previous->setCosts(it->cost);
                    previous->setTrip(this);
                    stages.push_back(previous);
                } else if (minVehicle != nullptr && it->line == minVehicle->getID()) {
                    previous = new MSStageDriving(rideOrigin, it->edges.back(), bs, localArrivalPos, 0.0, std::vector<std::string>({ it->line }));
                    previous->setParameters(*this);
                    previous->setCosts(it->cost);
                    previous->setTrip(this);
                    stages.push_back(previous);
                    minVehicle->replaceRouteEdges(it->edges, -1, 0, "person:" + transportable->getID(), true);
                    minVehicle->setArrivalPos(localArrivalPos);
                    const_cast<SUMOVehicleParameter&>(minVehicle->getParameter()).arrivalPos = localArrivalPos;
                    vehControl.addVehicle(minVehicle->getID(), minVehicle);
                    carUsed = true;
                } else {
                    const std::string line = OptionsCont::getOptions().getBool("persontrip.ride-public-line") ? it->line : LINE_ANY;
                    previous = new MSStageDriving(rideOrigin, it->edges.back(), bs, localArrivalPos, 0.0, std::vector<std::string>({ line }), myGroup, it->intended, TIME2STEPS(it->depart));
                    previous->setParameters(*this);
                    previous->setCosts(it->cost);
                    previous->setTrip(this);
                    stages.push_back(previous);
                }
            }
        }
        if (wasSet(VEHPARS_ARRIVALPOS_SET) && !stages.empty()) {
            // mark the last stage
            stages.back()->markSet(VEHPARS_ARRIVALPOS_SET);
        }
        setCosts(minCost);
        if (minVehicle != nullptr && (isTaxi || !carUsed)) {
            vehControl.deleteVehicle(minVehicle, true);
            vehControl.discountRoutingVehicle();
        }
    } else {
        // append stage so the GUI won't crash due to inconsistent state
        previous = new MSStageWalking(transportable->getID(), ConstMSEdgeVector({ origin, destination }), myDestinationStop, myDuration, mySpeed, previous->getArrivalPos(), myArrivalPos, myDepartPosLat);
        previous->setParameters(*this);
        stages.push_back(previous);
        if (MSGlobals::gCheckRoutes) {  // if not pedestrians will teleport
            return "No connection found between " + getOriginDescription() + " and " + getDestinationDescription() + " for person '" + transportable->getID() + "'.";
        }
    }
    if (stages.empty()) {
        // append stage so the GUI won't crash due to inconsistent state
        if (myOriginStop != nullptr && myOriginStop == myDestinationStop) {
            stages.push_back(new MSStageWaiting(destination, myDestinationStop, 0, -1, previous->getArrivalPos(), "sameStop", false));
        } else {
            stages.push_back(new MSStageWalking(transportable->getID(), ConstMSEdgeVector({ origin, destination }), myDestinationStop, myDuration, mySpeed, previous->getArrivalPos(), myArrivalPos, myDepartPosLat));
            if (MSGlobals::gCheckRoutes) {  // if not pedestrians will teleport
                return "Empty route between " + getOriginDescription() + " and " + getDestinationDescription() + " for person '" + transportable->getID() + "'.";
            }
        }
    }
    return "";
}


const std::string
MSStageTrip::setArrived(MSNet* net, MSTransportable* transportable, SUMOTime now, const bool vehicleArrived) {
    MSStage::setArrived(net, transportable, now, vehicleArrived);
    std::vector<MSStage*> stages;
    std::string result;
    if (transportable->getCurrentStageIndex() == 0) {
        myDepartPos = transportable->getParameter().departPos;
        if (transportable->getParameter().departPosProcedure == DepartPosDefinition::RANDOM) {
            // TODO we should probably use the rng of the lane here
            myDepartPos = RandHelper::rand(myOrigin->getLength());
        }
        MSStageWaiting start(myOrigin, myOriginStop, -1, transportable->getParameter().depart, myDepartPos, "start", true);
        result = reroute(transportable->getParameter().depart, net->getIntermodalRouter(0), transportable, &start, myOrigin, myDestination, stages);
    } else {
        MSStage* previous = transportable->getNextStage(-1);
        myDepartPos = previous->getArrivalPos();
        result = reroute(now, net->getIntermodalRouter(0), transportable, previous, myOrigin, myDestination, stages);
    }
    int idx = 1;
    for (MSStage* stage : stages) {
        transportable->appendStage(stage, idx++);
    }
    return result;
}


void
MSStageTrip::proceed(MSNet* net, MSTransportable* transportable, SUMOTime now, MSStage* /* previous */) {
    // just skip the stage, every interesting happens in setArrived
    transportable->proceed(net, now);
}


std::string
MSStageTrip::getOriginDescription() const {
    return (myOriginStop != nullptr
            ? toString(myOriginStop->getElement()) + " '" + myOriginStop->getID()
            : "edge '" + myOrigin->getID()) + "'";
}

std::string
MSStageTrip::getDestinationDescription() const {
    return (myDestinationStop != nullptr
            ? toString(myDestinationStop->getElement()) + " '" + myDestinationStop->getID()
            : "edge '" + myDestination->getID()) + "'";
}

std::string
MSStageTrip::getStageSummary(const bool) const {
    return "trip from " + getOriginDescription() + " to " + getDestinationDescription();
}

void
MSStageTrip::routeOutput(const bool /*isPerson*/, OutputDevice& os, const bool /*withRouteLength*/, const MSStage* const previous) const {
    if (myArrived < 0) {
        const bool walkFactorSet = myWalkFactor != OptionsCont::getOptions().getFloat("persontrip.walkfactor");
        const bool groupSet = myGroup != OptionsCont::getOptions().getString("persontrip.default.group");
        // could still be a persontrip but most likely it was a walk in the input
        SumoXMLTag tag = myModeSet == 0 && !walkFactorSet && !groupSet ? SUMO_TAG_WALK : SUMO_TAG_PERSONTRIP;
        os.openTag(tag);
        if (previous == nullptr || previous->getStageType() == MSStageType::WAITING_FOR_DEPART) {
            os.writeAttr(SUMO_ATTR_FROM, myOrigin->getID());
        }
        if (myDestinationStop == nullptr) {
            os.writeAttr(SUMO_ATTR_TO, myDestination->getID());
            if (wasSet(VEHPARS_ARRIVALPOS_SET)) {
                os.writeAttr(SUMO_ATTR_ARRIVALPOS, myArrivalPos);
            }
        } else {
            os.writeAttr(toString(myDestinationStop->getElement()), myDestinationStop->getID());
        }
        std::vector<std::string> modes;
        if ((myModeSet & SVC_PASSENGER) != 0) {
            modes.push_back("car");
        }
        if ((myModeSet & SVC_BICYCLE) != 0) {
            modes.push_back("bicycle");
        }
        if ((myModeSet & SVC_TAXI) != 0) {
            modes.push_back("taxi");
        }
        if ((myModeSet & SVC_BUS) != 0) {
            modes.push_back("public");
        }
        if (modes.size() > 0) {
            os.writeAttr(SUMO_ATTR_MODES, modes);
        }
        if (myVTypes.size() > 0) {
            os.writeAttr(SUMO_ATTR_VTYPES, myVTypes);
        }
        if (groupSet) {
            os.writeAttr(SUMO_ATTR_GROUP, myGroup);
        }
        if (walkFactorSet) {
            os.writeAttr(SUMO_ATTR_WALKFACTOR, myWalkFactor);
        }
        if (OptionsCont::getOptions().getBool("vehroute-output.cost")) {
            os.writeAttr(SUMO_ATTR_COST, getCosts());
        }
        os.closeTag();
    }
}

/****************************************************************************/
