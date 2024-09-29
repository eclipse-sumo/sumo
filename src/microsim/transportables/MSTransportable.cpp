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
/// @file    MSTransportable.cpp
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @author  Michael Behrisch
/// @date    Thu, 12 Jun 2014
///
// The common superclass for modelling transportable objects like persons and containers
/****************************************************************************/
#include <config.h>

#include <utils/common/StringTokenizer.h>
#include <utils/geom/GeomHelper.h>
#include <utils/vehicle/SUMOVehicleParameter.h>
#include <utils/router/PedestrianRouter.h>
#include <utils/router/IntermodalRouter.h>
#include <libsumo/TraCIConstants.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSNet.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSStoppingPlace.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/devices/MSTransportableDevice.h>
#include <microsim/transportables/MSTransportableControl.h>
#include <microsim/transportables/MSPerson.h>
#include <microsim/transportables/MSStageDriving.h>
#include <microsim/transportables/MSStageTrip.h>
#include <microsim/transportables/MSStageWaiting.h>
#include <microsim/transportables/MSTransportable.h>

SUMOTrafficObject::NumericalID MSTransportable::myCurrentNumericalIndex = 0;

//#define DEBUG_PARKING

// ===========================================================================
// method definitions
// ===========================================================================
MSTransportable::MSTransportable(const SUMOVehicleParameter* pars, MSVehicleType* vtype, MSTransportablePlan* plan, const bool isPerson) :
    SUMOTrafficObject(pars->id),
    myParameter(pars), myVType(vtype), myPlan(plan),
    myAmPerson(isPerson),
    myNumericalID(myCurrentNumericalIndex++) {
    myStep = myPlan->begin();
    // init devices
    MSDevice::buildTransportableDevices(*this, myDevices);
    for (MSStage* const stage : * myPlan) {
        stage->init(this);
    }
}


MSTransportable::~MSTransportable() {
    if (myStep != myPlan->end() && getCurrentStageType() == MSStageType::DRIVING) {
        MSStageDriving* const stage = dynamic_cast<MSStageDriving*>(*myStep);
        if (stage->getVehicle() != nullptr) {
            stage->getVehicle()->removeTransportable(this);
        } else if (stage->getOriginStop() != nullptr)  {
            stage->getOriginStop()->removeTransportable(this);
        }
    }
    if (myPlan != nullptr) {
        for (MSTransportablePlan::const_iterator i = myPlan->begin(); i != myPlan->end(); ++i) {
            delete *i;
        }
        delete myPlan;
        myPlan = nullptr;
    }
    for (MSTransportableDevice* dev : myDevices) {
        delete dev;
    }
    delete myParameter;
    if (myVType->isVehicleSpecific()) {
        MSNet::getInstance()->getVehicleControl().removeVType(myVType);
    }
}

SumoRNG*
MSTransportable::getRNG() const {
    return getEdge()->getLanes()[0]->getRNG();
}

int
MSTransportable::getRNGIndex() const {
    return getEdge()->getLanes()[0]->getRNGIndex();
}

bool
MSTransportable::proceed(MSNet* net, SUMOTime time, const bool vehicleArrived) {
    MSStage* const prior = *myStep;
    const std::string& error = prior->setArrived(net, this, time, vehicleArrived);
    // must be done before increasing myStep to avoid invalid state for rendering
    prior->getEdge()->removeTransportable(this);
    myStep++;
    if (error != "") {
        throw ProcessError(error);
    }
    /* We need to check whether an access stage is needed (or maybe even two).
       The general scheme is: If the prior stage ended at a stop and the next stage
       starts at an edge which is not the one the stop is at, but the stop has an access to it
       we need an access stage. The same is true if prior ends at an edge, the next stage
       is allowed to start at any stop the edge has access to.
       If we start at a stop or end at a stop no access is needed.
    */
    bool accessToStop = false;
    if (prior->getStageType() == MSStageType::WALKING || prior->getStageType() == MSStageType::DRIVING) {
        accessToStop = checkAccess(prior);
    } else if (prior->getStageType() == MSStageType::WAITING_FOR_DEPART) {
        for (MSTransportableDevice* const dev : myDevices) {
            dev->notifyEnter(*this, MSMoveReminder::NOTIFICATION_DEPARTED, nullptr);
        }
    }
    if (!accessToStop && (myStep == myPlan->end()
                          || ((*myStep)->getStageType() != MSStageType::DRIVING
                              && (*myStep)->getStageType() != MSStageType::TRIP))) {
        MSStoppingPlace* priorStop = prior->getStageType() == MSStageType::TRIP ? prior->getOriginStop() : prior->getDestinationStop();
        // a trip might resolve to DRIVING so we would have to stay at the stop
        // if a trip resolves to something else, this step will do stop removal
        if (priorStop != nullptr) {
            priorStop->removeTransportable(this);
        }
    }
    if (myStep != myPlan->end()) {
        if ((*myStep)->getStageType() == MSStageType::WALKING && (prior->getStageType() != MSStageType::ACCESS || prior->getDestination() != (*myStep)->getFromEdge())) {
            checkAccess(prior, false);
        }
        (*myStep)->proceed(net, this, time, prior);
        return true;
    } else {
        MSNet::getInstance()->getPersonControl().addArrived();
        return false;
    }
}


void
MSTransportable::setID(const std::string& /*newID*/) {
    throw ProcessError(TL("Changing a transportable ID is not permitted"));
}

SUMOTime
MSTransportable::getDesiredDepart() const {
    return myParameter->depart;
}

void
MSTransportable::setDeparted(SUMOTime now) {
    (*myStep)->setDeparted(now);
}

SUMOTime
MSTransportable::getDeparture() const {
    for (const MSStage* const stage : *myPlan) {
        if (stage->getDeparted() >= 0) {
            return stage->getDeparted();
        }
    }
    return -1;
}


double
MSTransportable::getEdgePos() const {
    return (*myStep)->getEdgePos(MSNet::getInstance()->getCurrentTimeStep());
}

double
MSTransportable::getBackPositionOnLane(const MSLane* /*lane*/) const {
    return getEdgePos() - getVehicleType().getLength();
}

int
MSTransportable::getDirection() const {
    return (*myStep)->getDirection();
}

Position
MSTransportable::getPosition() const {
    return (*myStep)->getPosition(MSNet::getInstance()->getCurrentTimeStep());
}

double
MSTransportable::getAngle() const {
    return (*myStep)->getAngle(MSNet::getInstance()->getCurrentTimeStep());
}

double
MSTransportable::getWaitingSeconds() const {
    return STEPS2TIME((*myStep)->getWaitingTime(MSNet::getInstance()->getCurrentTimeStep()));
}

double
MSTransportable::getSpeed() const {
    return (*myStep)->getSpeed();
}


void
MSTransportable::tripInfoOutput(OutputDevice& os) const {
    SUMOTime departure = myPlan->front()->getDeparted();
    os.openTag(isPerson() ? "personinfo" : "containerinfo");
    os.writeAttr(SUMO_ATTR_ID, getID());
    os.writeAttr(SUMO_ATTR_DEPART, departure >= 0 ? time2string(departure) : "-1");
    os.writeAttr(SUMO_ATTR_TYPE, getVehicleType().getID());
    if (isPerson()) {
        os.writeAttr(SUMO_ATTR_SPEEDFACTOR, getChosenSpeedFactor());
    }
    SUMOTime duration = 0;
    SUMOTime waitingTime = 0;
    SUMOTime timeLoss = 0;
    SUMOTime travelTime = 0;
    bool durationOK = true;
    bool waitingTimeOK = true;
    bool timeLossOK = true;
    bool travelTimeOK = true;
    for (MSStage* const i : *myPlan) {
        SUMOTime t = i->getDuration();
        if (t != SUMOTime_MAX) {
            duration += t;
        } else {
            durationOK = false;
        }
        t = i->getWaitingTime();
        if (t != SUMOTime_MAX) {
            waitingTime += t;
        } else {
            waitingTimeOK = false;
        }
        t = i->getTimeLoss(this);
        if (t != SUMOTime_MAX) {
            timeLoss += t;
        } else {
            timeLossOK = false;
        }
        t = i->getTravelTime();
        if (t != SUMOTime_MAX) {
            travelTime += t;
        } else {
            travelTimeOK = false;
        }
    }
    os.writeAttr(SUMO_ATTR_DURATION, durationOK ? time2string(duration) : "-1");
    os.writeAttr(SUMO_ATTR_WAITINGTIME, waitingTimeOK ? time2string(waitingTime) : "-1");
    os.writeAttr(SUMO_ATTR_TIMELOSS, timeLossOK ? time2string(timeLoss) : "-1");
    os.writeAttr(SUMO_ATTR_TRAVELTIME, travelTimeOK ? time2string(travelTime) : "-1");
    for (MSStage* const i : *myPlan) {
        i->tripInfoOutput(os, this);
    }
    os.closeTag();
}


void
MSTransportable::routeOutput(OutputDevice& os, const bool withRouteLength) const {
    const std::string typeID = (
                                   (isPerson() && getVehicleType().getID() == DEFAULT_PEDTYPE_ID)
                                   || (isContainer() && getVehicleType().getID() == DEFAULT_CONTAINERTYPE_ID)) ? "" : getVehicleType().getID();
    myParameter->write(os, OptionsCont::getOptions(), isPerson() ? SUMO_TAG_PERSON : SUMO_TAG_CONTAINER, typeID);
    if (hasArrived()) {
        os.writeAttr("arrival", time2string(MSNet::getInstance()->getCurrentTimeStep()));
    }
    const MSStage* previous = nullptr;
    for (const MSStage* const stage : *myPlan) {
        stage->routeOutput(myAmPerson, os, withRouteLength, previous);
        previous = stage;
    }
    myParameter->writeParams(os);
    os.closeTag();
    os.lf();
}


void
MSTransportable::setAbortWaiting(const SUMOTime timeout) {
    if (timeout < 0 && myAbortCommand != nullptr) {
        myAbortCommand->deschedule();
        myAbortCommand = nullptr;
        return;
    }
    myAbortCommand = new WrappingCommand<MSTransportable>(this, &MSTransportable::abortStage);
    MSNet::getInstance()->getBeginOfTimestepEvents()->addEvent(myAbortCommand, SIMSTEP + timeout);
}


SUMOTime
MSTransportable::abortStage(SUMOTime step) {
    WRITE_WARNINGF(TL("Teleporting % '%'; waited too long, from edge '%', time=%."),
                   isPerson() ? "person" : "container", getID(), (*myStep)->getEdge()->getID(), time2string(step));
    MSTransportableControl& tc = isPerson() ? MSNet::getInstance()->getPersonControl() : MSNet::getInstance()->getContainerControl();
    tc.registerTeleportAbortWait();
    (*myStep)->abort(this);
    if (!proceed(MSNet::getInstance(), step)) {
        tc.erase(this);
    }
    return 0;
}


void
MSTransportable::appendStage(MSStage* stage, int next) {
    // myStep is invalidated upon modifying myPlan
    const int stepIndex = (int)(myStep - myPlan->begin());
    if (next < 0) {
        myPlan->push_back(stage);
    } else {
        if (stepIndex + next > (int)myPlan->size()) {
            throw ProcessError("invalid index '" + toString(next) + "' for inserting new stage into plan of '" + getID() + "'");
        }
        myPlan->insert(myPlan->begin() + stepIndex + next, stage);
    }
    myStep = myPlan->begin() + stepIndex;
}


void
MSTransportable::removeStage(int next, bool stayInSim) {
    assert(myStep + next < myPlan->end());
    assert(next >= 0);
    if (next > 0) {
        // myStep is invalidated upon modifying myPlan
        int stepIndex = (int)(myStep - myPlan->begin());
        delete *(myStep + next);
        myPlan->erase(myStep + next);
        myStep = myPlan->begin() + stepIndex;
    } else {
        if (myStep + 1 == myPlan->end() && stayInSim) {
            // stay in the simulation until the start of simStep to allow appending new stages (at the correct position)
            appendStage(new MSStageWaiting(getEdge(), nullptr, 0, 0, getEdgePos(), "last stage removed", false));
        }
        (*myStep)->abort(this);
        if (!proceed(MSNet::getInstance(), SIMSTEP)) {
            MSNet::getInstance()->getPersonControl().erase(this);
        } else if (myPlan->front()->getDeparted() < 0) {
            myPlan->front()->setDeparted(SIMSTEP);
        }
    }
}


void
MSTransportable::setSpeed(double speed) {
    for (MSTransportablePlan::const_iterator i = myStep; i != myPlan->end(); ++i) {
        (*i)->setSpeed(speed);
    }
    getSingularType().setMaxSpeed(speed);
}


bool
MSTransportable::replaceRoute(ConstMSRoutePtr newRoute, const std::string& /* info */, bool /* onInit */, int /* offset */, bool /* addRouteStops */, bool /* removeStops */, std::string* /* msgReturn */) {
    if (isPerson()) {
        static_cast<MSPerson*>(this)->replaceWalk(newRoute->getEdges(), getPositionOnLane(), 0, 1);
        return true;
    }
    return false;
}


bool
MSTransportable::reroute(SUMOTime t, const std::string& /* info */, MSTransportableRouter& router, const bool /* onInit */, const bool /* withTaz */, const bool /* silent */, const MSEdge* /* sink */) {
    MSStageTrip* trip = getCurrentStage()->getTrip();
    if (trip == nullptr) {
        // TODO this should be possible after factoring out MSStageTrip::reroute
        return false;
    }
    if (getCurrentStage()->getVehicle() != nullptr) {
        // TODO rerouting during a ride still needs to be implemented
        return false;
    }
    // find the final stage of the trip
    int tripEndOffset = -1;
    for (int i = getNumRemainingStages() - 1; i >= 0; i--) {
        if (getNextStage(i)->getTrip() == trip) {
            tripEndOffset = i;
            break;
        }
    }
    std::vector<MSStage*> stages;
    MSStageWaiting start(getEdge(), getCurrentStage()->getOriginStop(), -1, t, getEdgePos(), "start", true);
    if (trip->reroute(t, router, this, &start, getEdge(), getRerouteDestination(), stages) == "") {
        // check whether the new plan actually differs
        while (tripEndOffset >= 0 && !stages.empty() && stages.back()->equals(*getNextStage(tripEndOffset))) {
            delete stages.back();
            stages.pop_back();
            tripEndOffset--;
        }
        bool abortCurrent = true;
        // check whether the future route of the current stage is identical to the route
        if (!stages.empty() && stages.front()->isWalk() && getCurrentStage()->isWalk()) {
            // TODO this check should be done for rides as well
            MSStageMoving* s = static_cast<MSStageMoving*>(getCurrentStage());
            int routeIndex = (int)(s->getRouteStep() - s->getRoute().begin());
            ConstMSEdgeVector oldEdges = s->getEdges();
            oldEdges.erase(oldEdges.begin(), oldEdges.begin() + routeIndex);
            ConstMSEdgeVector newEdges = stages.front()->getEdges();
            if (newEdges == oldEdges) {
                delete stages.front();
                stages.erase(stages.begin());
                abortCurrent = false;
            }
        }
        if (stages.empty()) {
            return false;
        }
        // remove future stages of the trip
        for (int i = tripEndOffset; i >= 1; i--) {
            removeStage(i);
        }
        // insert new stages of the rerouting
        int idx = 1;
        for (MSStage* stage : stages) {
            appendStage(stage, idx++);
        }
        if (abortCurrent) {
            removeStage(0);
        }
        return true;
    }
    return false;
}


void
MSTransportable::replaceVehicleType(MSVehicleType* type) {
    const SUMOVehicleClass oldVClass = myVType->getVehicleClass();
    if (myVType->isVehicleSpecific()) {
        MSNet::getInstance()->getVehicleControl().removeVType(myVType);
    }
    if (isPerson()
            && type->getVehicleClass() != oldVClass
            && type->getVehicleClass() != SVC_PEDESTRIAN
            && !type->getParameter().wasSet(VTYPEPARS_VEHICLECLASS_SET)) {
        WRITE_WARNINGF(TL("Person '%' receives type '%' which implicitly uses unsuitable vClass '%'."), getID(), type->getID(), toString(type->getVehicleClass()));
    }
    myVType = type;
}


MSVehicleType&
MSTransportable::getSingularType() {
    if (myVType->isVehicleSpecific()) {
        return *myVType;
    }
    MSVehicleType* type = myVType->buildSingularType(myVType->getID() + "@" + getID());
    replaceVehicleType(type);
    return *type;
}


PositionVector
MSTransportable::getBoundingBox() const {
    PositionVector centerLine;
    const Position p = getPosition();
    const double angle = getAngle();
    const double length = getVehicleType().getLength();
    const Position back = p + Position(-cos(angle) * length, -sin(angle) * length);
    centerLine.push_back(p);
    centerLine.push_back(back);
    centerLine.move2side(0.5 * getVehicleType().getWidth());
    PositionVector result = centerLine;
    centerLine.move2side(-getVehicleType().getWidth());
    result.append(centerLine.reverse(), POSITION_EPS);
    //std::cout << " transp=" << getID() << " p=" << p << " angle=" << GeomHelper::naviDegree(angle) << " back=" << back << " result=" << result << "\n";
    return result;
}


std::string
MSTransportable::getStageSummary(int stageIndex) const {
    assert(stageIndex < (int)myPlan->size());
    assert(stageIndex >= 0);
    return (*myPlan)[stageIndex]->getStageSummary(myAmPerson);
}


const std::set<SUMOTrafficObject::NumericalID>
MSTransportable::getUpcomingEdgeIDs() const {
    std::set<SUMOTrafficObject::NumericalID> result;
    for (auto step = myStep; step != myPlan->end(); ++step) {
        for (const MSEdge* const e : (*step)->getEdges()) {
            result.insert(e->getNumericalID());
        }
    }
    return result;
}


bool
MSTransportable::hasArrived() const {
    return myStep == myPlan->end();
}

bool
MSTransportable::hasDeparted() const {
    return myPlan->size() > 0 && (myPlan->front()->getDeparted() >= 0 || myStep > myPlan->begin());
}


void
MSTransportable::rerouteParkingArea(MSStoppingPlace* orig, MSStoppingPlace* replacement) {
    // check whether the transportable was riding to the orignal stop
    // @note: parkingArea can currently not be set as myDestinationStop so we
    // check for stops on the edge instead
#ifdef DEBUG_PARKING
    std::cout << SIMTIME << " person=" << getID() << " rerouteParkingArea orig=" << orig->getID() << " replacement=" << replacement->getID() << "\n";
#endif
    assert(getCurrentStageType() == MSStageType::DRIVING);
    if (!myAmPerson) {
        WRITE_WARNING(TL("parkingAreaReroute not supported for containers"));
        return;
    }
    if (getDestination() == &orig->getLane().getEdge()) {
        MSStageDriving* const stage = dynamic_cast<MSStageDriving*>(*myStep);
        assert(stage != 0);
        assert(stage->getVehicle() != 0);
        // adapt plan
        stage->setDestination(&replacement->getLane().getEdge(), replacement);
        stage->setArrivalPos((replacement->getBeginLanePosition() + replacement->getEndLanePosition()) / 2);
#ifdef DEBUG_PARKING
        std::cout << " set ride destination\n";
#endif
        if (myStep + 1 == myPlan->end()) {
            return;
        }
        // if the next step is a walk, adapt the route
        MSStage* nextStage = *(myStep + 1);
        if (nextStage->getStageType() == MSStageType::TRIP) {
            dynamic_cast<MSStageTrip*>(nextStage)->setOrigin(stage->getDestination());
#ifdef DEBUG_PARKING
            std::cout << " set subsequent trip origin\n";
#endif
        } else if (nextStage->getStageType() == MSStageType::WALKING) {
#ifdef DEBUG_PARKING
            std::cout << " replace subsequent walk with a trip\n";
#endif
            MSStageTrip* newStage = new MSStageTrip(stage->getDestination(), nullptr, nextStage->getDestination(),
                                                    nextStage->getDestinationStop(), -1, 0, "", -1, 1, getID(), 0, true, nextStage->getArrivalPos());
            removeStage(1);
            appendStage(newStage, 1);
        } else if (nextStage->getStageType() == MSStageType::WAITING) {
#ifdef DEBUG_PARKING
            std::cout << " add subsequent walk to reach stop\n";
            std::cout << "   arrivalPos=" << nextStage->getArrivalPos() << "\n";
#endif
            MSStageTrip* newStage = new MSStageTrip(stage->getDestination(), nullptr, nextStage->getDestination(),
                                                    nextStage->getDestinationStop(), -1, 0, "", -1, 1, getID(), 0, true, nextStage->getArrivalPos());
            appendStage(newStage, 1);
        }
        // if the plan contains another ride with the same vehicle from the same
        // parking area, adapt the preceeding walk to end at the replacement
        for (auto it = myStep + 2; it != myPlan->end(); it++) {
            MSStage* const futureStage = *it;
            MSStage* const prevStage = *(it - 1);
            if (futureStage->getStageType() == MSStageType::DRIVING) {
                MSStageDriving* const ds = static_cast<MSStageDriving*>(futureStage);
                // ride origin is set implicitly from the walk destination
                ds->setOrigin(nullptr);
                if (ds->getLines() == stage->getLines()
                        && prevStage->getDestination() == &orig->getLane().getEdge()) {
                    if (prevStage->getStageType() == MSStageType::TRIP) {
                        dynamic_cast<MSStageTrip*>(prevStage)->setDestination(stage->getDestination(), replacement);
#ifdef DEBUG_PARKING
                        std::cout << " replace later trip before ride (" << (it - myPlan->begin()) << ")\n";
#endif
                    } else if (prevStage->getStageType() == MSStageType::WALKING) {
#ifdef DEBUG_PARKING
                        std::cout << " replace later walk before ride (" << (it - myPlan->begin()) << ")\n";
#endif
                        MSStageTrip* newStage = new MSStageTrip(prevStage->getFromEdge(), nullptr, stage->getDestination(),
                                                                replacement, -1, 0, "", -1, 1, getID(), 0, true, stage->getArrivalPos());
                        int prevStageRelIndex = (int)(it - 1 - myStep);
                        removeStage(prevStageRelIndex);
                        appendStage(newStage, prevStageRelIndex);
                    }
                    break;
                }
            }
        }
    }
}


MSDevice*
MSTransportable::getDevice(const std::type_info& type) const {
    for (MSTransportableDevice* const dev : myDevices) {
        if (typeid(*dev) == type) {
            return dev;
        }
    }
    return nullptr;
}


void
MSTransportable::setJunctionModelParameter(const std::string& key, const std::string& value) {
    if (key == toString(SUMO_ATTR_JM_IGNORE_IDS) || key == toString(SUMO_ATTR_JM_IGNORE_TYPES)) {
        getParameter().parametersSet |= VEHPARS_JUNCTIONMODEL_PARAMS_SET;
        const_cast<SUMOVehicleParameter&>(getParameter()).setParameter(key, value);
        // checked in MSLink::ignoreFoe
    } else {
        throw InvalidArgument(getObjectType() + " '" + getID() + "' does not support junctionModel parameter '" + key + "'");
    }
}


double
MSTransportable::getSlope() const {
    const MSEdge* edge = getEdge();
    const double ep = getEdgePos();
    const double gp = edge->getLanes()[0]->interpolateLanePosToGeometryPos(ep);
    return edge->getLanes()[0]->getShape().slopeDegreeAtOffset(gp);
}


SUMOTime
MSTransportable::getWaitingTime(const bool /* accumulated */) const {
    return (*myStep)->getWaitingTime(MSNet::getInstance()->getCurrentTimeStep());
}


double
MSTransportable::getMaxSpeed() const {
    return MIN2(getVehicleType().getMaxSpeed(), getVehicleType().getDesiredMaxSpeed() * getChosenSpeedFactor());
}


SUMOVehicleClass
MSTransportable::getVClass() const {
    return getVehicleType().getVehicleClass();
}


int
MSTransportable::getRoutingMode() const {
    /// @todo: allow configuring routing mode
    return libsumo::ROUTING_MODE_DEFAULT;
}

void
MSTransportable::saveState(OutputDevice& out) {
    // this saves lots of departParameters which are only needed for transportables that did not yet depart
    // the parameters may hold the name of a vTypeDistribution but we are interested in the actual type
    const SUMOTime desiredDepart = myParameter->depart;
    if (myPlan->front()->getDeparted() >= 0) {
        // this is only relevant in the context of delayed departure (max-num-persons)
        const_cast<SUMOVehicleParameter*>(myParameter)->depart = myPlan->front()->getDeparted();
    }
    myParameter->write(out, OptionsCont::getOptions(), myAmPerson ? SUMO_TAG_PERSON : SUMO_TAG_CONTAINER, getVehicleType().getID());
    const_cast<SUMOVehicleParameter*>(myParameter)->depart = desiredDepart;
    if (!myParameter->wasSet(VEHPARS_SPEEDFACTOR_SET) && getChosenSpeedFactor() != 1) {
        out.setPrecision(MAX2(gPrecisionRandom, gPrecision));
        out.writeAttr(SUMO_ATTR_SPEEDFACTOR, getChosenSpeedFactor());
        out.setPrecision(gPrecision);
    }
    int stepIdx = (int)(myStep - myPlan->begin());
    for (auto it = myPlan->begin(); it != myStep; ++it) {
        const MSStageType st = (*it)->getStageType();
        if (st == MSStageType::TRIP || st == MSStageType::ACCESS) {
            stepIdx--;
        }
    }
    std::ostringstream state;
    state << myParameter->parametersSet << " " << stepIdx;
    (*myStep)->saveState(state);
    out.writeAttr(SUMO_ATTR_STATE, state.str());
    const MSStage* previous = nullptr;
    for (const MSStage* const stage : *myPlan) {
        stage->routeOutput(myAmPerson, out, false, previous);
        previous = stage;
    }
    out.closeTag();
}


void
MSTransportable::loadState(const std::string& state) {
    std::istringstream iss(state);
    int step;
    iss >> myParameter->parametersSet >> step;
    myPlan->front()->setDeparted(myParameter->depart);
    myStep = myPlan->begin() + step;
    (*myStep)->loadState(this, iss);
}


/****************************************************************************/
