/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
    MSStage(destination, toStop, arrivalPos, MSStageType::TRIP, group),
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
    return new MSStageTrip(myOrigin, const_cast<MSStoppingPlace*>(myOriginStop),
                           myDestination, myDestinationStop, myDuration,
                           myModeSet, myVTypes, mySpeed, myWalkFactor, myGroup, myDepartPosLat, myHaveArrivalPos, myArrivalPos);
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


const std::string
MSStageTrip::setArrived(MSNet* net, MSTransportable* transportable, SUMOTime now, const bool vehicleArrived) {
    MSStage::setArrived(net, transportable, now, vehicleArrived);
    if (myOrigin->isTazConnector() && myOrigin->getSuccessors().size() == 0) {
        // previous stage ended at a taz sink-edge
        myOrigin = transportable->getNextStage(-1)->getDestination();
    }
    MSVehicleControl& vehControl = net->getVehicleControl();
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
        } else if ((myModeSet & SVC_TAXI) != 0) {
            pars.push_back(new SUMOVehicleParameter());
            pars.back()->vtypeid = DEFAULT_TAXITYPE_ID;
            pars.back()->id = transportable->getID() + "_taxi";
            pars.back()->line = "taxi";
        } else if ((myModeSet & SVC_BICYCLE) != 0) {
            pars.push_back(new SUMOVehicleParameter());
            pars.back()->vtypeid = DEFAULT_BIKETYPE_ID;
            pars.back()->id = transportable->getID() + "_b0";
            pars.back()->departProcedure = DepartDefinition::TRIGGERED;
        } else {
            // allow shortcut via busStop even when not intending to ride
            pars.push_back(nullptr);
        }
    }
    MSStage* previous;
    SUMOTime time = MSNet::getInstance()->getCurrentTimeStep();
    if (transportable->getNumStages() == transportable->getNumRemainingStages()) { // this is a difficult way to check that we are the first stage
        myDepartPos = transportable->getParameter().departPos;
        if (transportable->getParameter().departPosProcedure == DepartPosDefinition::RANDOM) {
            myDepartPos = RandHelper::rand(myOrigin->getLength());
        }
        previous = new MSStageWaiting(myOrigin, nullptr, -1, transportable->getParameter().depart, myDepartPos, "start", true);
        time = transportable->getParameter().depart;
    } else {
        previous = transportable->getNextStage(-1);
        myDepartPos = previous->getArrivalPos();
    }
    // TODO This works currently only for a single vehicle type
    const int oldNumStages = transportable->getNumStages();
    for (SUMOVehicleParameter* vehPar : pars) {
        SUMOVehicle* vehicle = nullptr;
        bool isTaxi = false;
        if (vehPar != nullptr) {
            isTaxi = vehPar->vtypeid == DEFAULT_TAXITYPE_ID && vehPar->line == "taxi";
            if (myDepartPos != 0) {
                vehPar->departPosProcedure = DepartPosDefinition::GIVEN;
                vehPar->departPos = myDepartPos;
                vehPar->parametersSet |= VEHPARS_DEPARTPOS_SET;
            }
            pars.back()->parametersSet |= VEHPARS_ARRIVALPOS_SET;
            pars.back()->arrivalPosProcedure = ArrivalPosDefinition::GIVEN;
            pars.back()->parametersSet |= VEHPARS_ARRIVALSPEED_SET;
            pars.back()->arrivalSpeedProcedure = ArrivalSpeedDefinition::GIVEN;
            pars.back()->arrivalSpeed = 0;

            MSVehicleType* type = vehControl.getVType(vehPar->vtypeid);
            if (type->getVehicleClass() != SVC_IGNORING && (myOrigin->getPermissions() & type->getVehicleClass()) == 0 && !isTaxi) {
                WRITE_WARNINGF(TL("Ignoring vehicle type '%' when routing person '%' because it is not allowed on the start edge."), type->getID(), transportable->getID());
                delete vehPar;
            } else {
                ConstMSRoutePtr const routeDummy = std::make_shared<MSRoute>(vehPar->id, ConstMSEdgeVector({ myOrigin }), false, nullptr, std::vector<SUMOVehicleParameter::Stop>());
                vehicle = vehControl.buildVehicle(vehPar, routeDummy, type, !MSGlobals::gCheckRoutes);
            }
        }
        bool carUsed = false;
        std::vector<MSNet::MSIntermodalRouter::TripItem> result;
        int stageIndex = 1;
        double departPos = previous->getArrivalPos();
        MSStoppingPlace* const prevStop = previous->getDestinationStop();
        if (MSGlobals::gUseMesoSim && prevStop != nullptr) {
            departPos = (prevStop->getBeginLanePosition() + prevStop->getEndLanePosition()) / 2.;
        }
        if (net->getIntermodalRouter(0).compute(myOrigin, myDestination,
                                                departPos, myOriginStop == nullptr ? "" : myOriginStop->getID(),
                                                myArrivalPos, myDestinationStop == nullptr ? "" : myDestinationStop->getID(),
                                                transportable->getMaxSpeed() * myWalkFactor, vehicle, myModeSet, time, result)) {
            for (std::vector<MSNet::MSIntermodalRouter::TripItem>::iterator it = result.begin(); it != result.end(); ++it) {
                if (!it->edges.empty()) {
                    MSStoppingPlace* bs = MSNet::getInstance()->getStoppingPlace(it->destStop, SUMO_TAG_BUS_STOP);
                    double localArrivalPos = bs != nullptr ? bs->getAccessPos(it->edges.back()) : it->edges.back()->getLength() / 2.;
                    const MSEdge* const rideOrigin = myOrigin->isTazConnector() && (transportable->getNumStages() == oldNumStages) ? it->edges.front() : nullptr;
                    if (it + 1 == result.end() && myHaveArrivalPos) {
                        localArrivalPos = myArrivalPos;
                    }
                    if (it->line == "") {
                        // determine walk departPos
                        double depPos = previous->getArrivalPos();
                        if (previous->getDestinationStop() != nullptr) {
                            depPos = previous->getDestinationStop()->getAccessPos(it->edges.front());
                        } else if (myOrigin->isTazConnector()) {
                            // walk the whole length of the first edge
                            const MSEdge* first = it->edges.front();
                            if (std::find(first->getPredecessors().begin(), first->getPredecessors().end(), myOrigin) != first->getPredecessors().end()) {
                                depPos = 0;
                            } else {
                                depPos = first->getLength();
                            }
                        } else if (previous->getDestination() != it->edges.front()) {
                            if ((previous->getDestination()->getToJunction() == it->edges.front()->getToJunction())
                                    || (previous->getDestination()->getFromJunction() == it->edges.front()->getToJunction())) {
                                depPos = it->edges.front()->getLength();
                            } else {
                                depPos = 0.;
                            }
                        }
                        if (myDestination->isTazConnector()) {
                            // walk the whole length of the last edge
                            const MSEdge* last = it->edges.back();
                            if (std::find(last->getSuccessors().begin(), last->getSuccessors().end(), myDestination) != last->getSuccessors().end()) {
                                localArrivalPos = last->getLength();
                            } else {
                                localArrivalPos = 0;
                            }
                        }
                        previous = new MSPerson::MSPersonStage_Walking(transportable->getID(), it->edges, bs, myDuration, mySpeed, depPos, localArrivalPos, myDepartPosLat);
                        transportable->appendStage(previous, stageIndex++);
                    } else if (isTaxi) {
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
                        previous = new MSStageDriving(rideOrigin, it->edges.back(), bs, localArrivalPos, std::vector<std::string>({ "taxi" }), myGroup);
                        transportable->appendStage(previous, stageIndex++);
                    } else if (vehicle != nullptr && it->line == vehicle->getID()) {
                        if (bs == nullptr && it + 1 != result.end()) {
                            // we have no defined endpoint and are in the middle of the trip, drive as far as possible
                            localArrivalPos = it->edges.back()->getLength();
                        }
                        previous = new MSStageDriving(rideOrigin, it->edges.back(), bs, localArrivalPos, std::vector<std::string>({ it->line }));
                        transportable->appendStage(previous, stageIndex++);
                        vehicle->replaceRouteEdges(it->edges, -1, 0, "person:" + transportable->getID(), true);
                        vehicle->setArrivalPos(localArrivalPos);
                        const_cast<SUMOVehicleParameter&>(vehicle->getParameter()).arrivalPos = localArrivalPos;
                        vehControl.addVehicle(vehPar->id, vehicle);
                        carUsed = true;
                    } else {
                        previous = new MSStageDriving(rideOrigin, it->edges.back(), bs, localArrivalPos, std::vector<std::string>({ it->line }), myGroup, it->intended, TIME2STEPS(it->depart));
                        transportable->appendStage(previous, stageIndex++);
                    }
                }
            }
            if (wasSet(VEHPARS_ARRIVALPOS_SET) && stageIndex > 1) {
                // mark the last stage
                transportable->getNextStage(stageIndex - 1)->markSet(VEHPARS_ARRIVALPOS_SET);
            }
        } else {
            // append stage so the GUI won't crash due to inconsistent state
            transportable->appendStage(new MSPerson::MSPersonStage_Walking(transportable->getID(), ConstMSEdgeVector({ myOrigin, myDestination }), myDestinationStop, myDuration, mySpeed, previous->getArrivalPos(), myArrivalPos, myDepartPosLat), stageIndex++);
            if (MSGlobals::gCheckRoutes) {  // if not pedestrians will teleport
                if (vehicle != nullptr) {
                    vehControl.deleteVehicle(vehicle, true);
                }
                return "No connection found between edge '" + myOrigin->getID() + "' and edge '" + (myDestinationStop != nullptr ? myDestinationStop->getID() : myDestination->getID()) + "' for person '" + transportable->getID() + "'.";
            }
        }
        if (vehicle != nullptr && (isTaxi || !carUsed)) {
            vehControl.deleteVehicle(vehicle, true);
        }
    }
    if (transportable->getNumStages() == oldNumStages) {
        // append stage so the GUI won't crash due to inconsistent state
        transportable->appendStage(new MSPerson::MSPersonStage_Walking(transportable->getID(), ConstMSEdgeVector({ myOrigin, myDestination }), myDestinationStop, myDuration, mySpeed, previous->getArrivalPos(), myArrivalPos, myDepartPosLat), -1);
        if (MSGlobals::gCheckRoutes) {  // if not pedestrians will teleport
            return "Empty route between edge '" + myOrigin->getID() + "' and edge '" + (myDestinationStop != nullptr ? myDestinationStop->getID() : myDestination->getID()) + "' for person '" + transportable->getID() + "'.";
        }
    }
    return "";
}


void
MSStageTrip::proceed(MSNet* net, MSTransportable* transportable, SUMOTime now, MSStage* /* previous */) {
    // just skip the stage, every interesting happens in setArrived
    transportable->proceed(net, now);
}


std::string
MSStageTrip::getStageSummary(const bool) const {
    return "trip from '" + myOrigin->getID() + "' to '" + getDestination()->getID() + "'";
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
        os.closeTag();
    }
}

/****************************************************************************/
