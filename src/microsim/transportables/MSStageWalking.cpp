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
/// @file    MSStageWalking.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Mon, 9 Jul 2001
///
// A stage performing walking on a sequence of edges.
/****************************************************************************/
#include <config.h>

#include <string>
#include <vector>
#include <utils/iodevices/OutputDevice.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/ToString.h>
#include <utils/common/StringUtils.h>
#include <utils/geom/GeomHelper.h>
#include <utils/router/IntermodalNetwork.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/transportables/MSTransportableControl.h>
#include <microsim/MSInsertionControl.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSStoppingPlace.h>
#include <microsim/MSRouteHandler.h>
#include <microsim/devices/MSDevice_Tripinfo.h>
#include <microsim/devices/MSDevice_Taxi.h>
#include <microsim/trigger/MSTriggeredRerouter.h>
#include "MSPModel_Striping.h"
#include "MSStageTrip.h"
#include "MSPerson.h"
#include "MSStageWalking.h"


// ===========================================================================
// static member definition
// ===========================================================================
bool MSStageWalking::myWarnedInvalidTripinfo = false;


// ===========================================================================
// method definitions
// ===========================================================================
MSStageWalking::MSStageWalking(const std::string& personID,
                               const ConstMSEdgeVector& route,
                               MSStoppingPlace* toStop,
                               SUMOTime walkingTime, double speed,
                               double departPos, double arrivalPos, double departPosLat, int departLane,
                               const std::string& routeID) :
    MSStageMoving(route, routeID, toStop, speed, departPos, arrivalPos, departPosLat, departLane, MSStageType::WALKING),
    myWalkingTime(walkingTime),
    myExitTimes(nullptr),
    myInternalDistance(0) {
    myDepartPos = SUMOVehicleParameter::interpretEdgePos(departPos, route.front()->getLength(), SUMO_ATTR_DEPARTPOS,
                  "person '" + personID + "' walking from edge '" + route.front()->getID() + "'");
    myArrivalPos = SUMOVehicleParameter::interpretEdgePos(arrivalPos, route.back()->getLength(), SUMO_ATTR_ARRIVALPOS,
                   "person '" + personID + "' walking to edge '" + route.back()->getID() + "'");
    if (walkingTime > 0) {
        mySpeed = computeAverageSpeed();
    }
}


MSStageWalking::~MSStageWalking() {
    delete myExitTimes;
}


MSStage*
MSStageWalking::clone() const {
    std::vector<const MSEdge*> route = myRoute;
    double departPos = myDepartPos;
    double arrivalPos = myArrivalPos;
    int departLane = myDepartLane;
    if (myRouteID != "" && MSRoute::distDictionary(myRouteID) != nullptr) {
        route = MSRoute::dictionary(myRouteID, MSRouteHandler::getParsingRNG())->getEdges();
        if (departPos > route[0]->getLength()) {
            WRITE_WARNINGF(TL("Adjusting departPos for cloned walk with routeDistribution '%'"), myRouteID);
            departPos = route[0]->getLength();
        }
        if (arrivalPos > route.back()->getLength()) {
            WRITE_WARNINGF(TL("Adjusting arrivalPos for cloned walk with routeDistribution '%'"), myRouteID);
            arrivalPos = route.back()->getLength();
        }
        if (departLane >= route[0]->getNumLanes()) {
            WRITE_WARNINGF(TL("Adjusting departLane for cloned walk with routeDistribution '%'"), myRouteID);
            departLane = route[0]->getNumLanes() - 1;
        }
    }
    MSStage* clon = new MSStageWalking("dummyID", route, myDestinationStop, myWalkingTime, mySpeed, departPos, arrivalPos, myDepartPosLat, departLane, myRouteID);
    clon->setParameters(*this);
    return clon;
}


void
MSStageWalking::proceed(MSNet* net, MSTransportable* person, SUMOTime now, MSStage* previous) {
    myDeparted = now;
    myRouteStep = myRoute.begin();
    myLastEdgeEntryTime = now;
    if (myWalkingTime == 0) {
        if (!person->proceed(net, now)) {
            MSNet::getInstance()->getPersonControl().erase(person);
        }
        return;
    }
    if (previous->getEdgePos(now) >= 0 && previous->getEdge() == *myRouteStep) {
        // we need to adapt to the arrival position of the vehicle unless we have an explicit access
        myDepartPos = previous->getEdgePos(now);
        if (previous->getStageType() == MSStageType::ACCESS) {
            const Position& lastPos = previous->getPosition(now);
            // making a wild guess on where we actually want to depart laterally
            const double possibleDepartPosLat = lastPos.distanceTo(previous->getEdgePosition(previous->getEdge(), myDepartPos, 0.));
            const Position& newPos = previous->getEdgePosition(previous->getEdge(), myDepartPos, -possibleDepartPosLat); // Minus sign is here for legacy reasons.
            if (lastPos.almostSame(newPos)) {
                myDepartPosLat = possibleDepartPosLat;
            } else if (lastPos.almostSame(previous->getEdgePosition(previous->getEdge(), myDepartPos, possibleDepartPosLat))) {
                // maybe the other side of the street
                myDepartPosLat = -possibleDepartPosLat;
            }
        }
        if (myWalkingTime > 0) {
            mySpeed = computeAverageSpeed();
        }
    }
    MSTransportableControl& pControl = net->getPersonControl();
    myPState = pControl.getMovementModel()->add(person, this, now);
    if (myPState == nullptr) {
        pControl.erase(person);
        return;
    }
    if (previous->getStageType() != MSStageType::WALKING || previous->getEdge() != getEdge()) {
        // we only need new move reminders if we are walking a different edge (else it is probably a rerouting)
        activateEntryReminders(person, true);
    }
    if (OptionsCont::getOptions().getBool("vehroute-output.exit-times")) {
        myExitTimes = new std::vector<SUMOTime>();
    }
    (*myRouteStep)->addTransportable(person);
}


void
MSStageWalking::abort(MSTransportable*) {
    MSNet::getInstance()->getPersonControl().getMovementModel()->remove(myPState);
}


void
MSStageWalking::setSpeed(double speed) {
    mySpeed = speed;
}


double
MSStageWalking::computeAverageSpeed() const {
    return walkDistance() / STEPS2TIME(myWalkingTime + 1); // avoid systematic rounding errors
}


bool
MSPerson::isJammed() const {
    MSStageWalking* stage = dynamic_cast<MSStageWalking*>(getCurrentStage());
    if (stage != nullptr) {
        return stage->getPState()->isJammed();
    }
    return false;
}


double
MSStageWalking::walkDistance(bool partial) const {
    double length = 0;
    auto endIt = partial && myArrived < 0 ? myRouteStep + 1 : myRoute.end();
    for (ConstMSEdgeVector::const_iterator i = myRoute.begin(); i != endIt; ++i) {
        length += (*i)->getLength();
    }
    if (myRoute.size() > 1 && MSNet::getInstance()->getPersonControl().getMovementModel()->usingInternalLanes()) {
        if (myInternalDistance > 0) {
            length += myInternalDistance;
        } else {
            // use lower bound for distance to pass the intersection
            for (ConstMSEdgeVector::const_iterator i = myRoute.begin(); i != endIt - 1; ++i) {
                const MSEdge* fromEdge = *i;
                const MSEdge* toEdge = *(i + 1);
                const MSLane* from = getSidewalk<MSEdge, MSLane>(fromEdge);
                const MSLane* to = getSidewalk<MSEdge, MSLane>(toEdge);
                Position fromPos;
                Position toPos;
                if (from != nullptr && to != nullptr) {
                    if (fromEdge->getToJunction() == toEdge->getFromJunction()) {
                        fromPos = from->getShape().back();
                        toPos = to->getShape().front();
                    } else if (fromEdge->getToJunction() == toEdge->getToJunction()) {
                        fromPos = from->getShape().back();
                        toPos = to->getShape().back();
                    } else if (fromEdge->getFromJunction() == toEdge->getFromJunction()) {
                        fromPos = from->getShape().front();
                        toPos = to->getShape().front();
                    } else if (fromEdge->getFromJunction() == toEdge->getToJunction()) {
                        fromPos = from->getShape().front();
                        toPos = to->getShape().back();
                    }
                    //std::cout << " from=" << from->getID() << " to=" << to->getID() << " junctionLength=" << fromPos.distanceTo2D(toPos) << "\n";
                    length += fromPos.distanceTo2D(toPos);
                }
            }
        }
    }
    // determine walking direction for depart and arrival
    const int departFwdArrivalDir = MSPModel::canTraverse(MSPModel::FORWARD, myRoute);
    const int departBwdArrivalDir = MSPModel::canTraverse(MSPModel::BACKWARD, myRoute);
    const bool mayStartForward = departFwdArrivalDir != MSPModel::UNDEFINED_DIRECTION;
    const bool mayStartBackward = departBwdArrivalDir != MSPModel::UNDEFINED_DIRECTION;
    const double arrivalPos = partial && myArrived < 0 ? getEdgePos(SIMSTEP) : myArrivalPos;
    const double lengthFwd = (length - myDepartPos - (
                                  departFwdArrivalDir == MSPModel::BACKWARD
                                  ? arrivalPos
                                  : myRoute.back()->getLength() - arrivalPos));
    const double lengthBwd = (length - (myRoute.front()->getLength() - myDepartPos) - (
                                  departBwdArrivalDir == MSPModel::BACKWARD
                                  ? arrivalPos
                                  : myRoute.back()->getLength() - arrivalPos));
    //std::cout << " length=" << length << " lengthFwd=" << lengthFwd << " lengthBwd=" << lengthBwd << " mayStartForward=" << mayStartForward << " mayStartBackward=" << mayStartBackward << "\n";

    if (myRoute.size() == 1) {
        if (myDepartPos > myArrivalPos) {
            length = lengthBwd;
        } else {
            length = lengthFwd;
        }
    } else {
        if (mayStartForward && mayStartBackward) {
            length = lengthFwd < lengthBwd ? lengthFwd : lengthBwd;
        } else if (mayStartForward) {
            length = lengthFwd;
        } else if (mayStartBackward) {
            length = lengthBwd;
        } else {
            length = lengthFwd;
        }
    }
    //std::cout << SIMTIME << " route=" << toString(myRoute)
    //    << " depPos=" << myDepartPos << " arPos=" << myArrivalPos
    //    << " dFwdADir=" << departFwdArrivalDir
    //    << " dBwdADir=" << departBwdArrivalDir
    //    << " lengthFwd=" << lengthFwd
    //    << " lengthBwd=" << lengthBwd
    //    << "\n";

    return MAX2(POSITION_EPS, length);
}


void
MSStageWalking::tripInfoOutput(OutputDevice& os, const MSTransportable* const person) const {
    if (!myWarnedInvalidTripinfo && MSNet::getInstance()->getPersonControl().getMovementModel()->usingShortcuts()) {
        WRITE_WARNING(TL("The pedestrian model uses infrastructure which is not in the network, timeLoss and routeLength may be invalid."));
        myWarnedInvalidTripinfo = true;
    }
    const double distance = walkDistance(true);
    const double maxSpeed = getMaxSpeed(person);
    const SUMOTime duration = myArrived - myDeparted;
    SUMOTime timeLoss = myArrived == -1 ? 0 : duration - TIME2STEPS(distance / maxSpeed);
    if (timeLoss < 0 && timeLoss > TIME2STEPS(-0.1)) {
        // avoid negative timeLoss due to rounding errors
        timeLoss = 0;
    }
    MSDevice_Tripinfo::addPedestrianData(distance, duration, timeLoss);
    os.openTag("walk");
    os.writeAttr("depart", myDeparted >= 0 ? time2string(myDeparted) : "-1");
    os.writeAttr("departPos", myDepartPos);
    os.writeAttr("arrival", myArrived >= 0 ? time2string(myArrived) : "-1");
    os.writeAttr("arrivalPos", myArrived >= 0 ? toString(myArrivalPos) : "-1");
    os.writeAttr("duration", myDeparted < 0 ? "-1" :
                 time2string(myArrived >= 0 ? duration : MSNet::getInstance()->getCurrentTimeStep() - myDeparted));
    os.writeAttr("routeLength", myArrived >= 0 ? toString(distance) : "-1");
    os.writeAttr("timeLoss", time2string(timeLoss));
    os.writeAttr("maxSpeed", maxSpeed);
    os.closeTag();
}


void
MSStageWalking::routeOutput(const bool /* isPerson */, OutputDevice& os, const bool withRouteLength, const MSStage* const /* previous */) const {
    os.openTag("walk").writeAttr(SUMO_ATTR_EDGES, myRoute);
    std::string comment = "";
    if (myDestinationStop != nullptr) {
        os.writeAttr(toString(myDestinationStop->getElement()), myDestinationStop->getID());
        if (myDestinationStop->getMyName() != "") {
            comment =  " <!-- " + StringUtils::escapeXML(myDestinationStop->getMyName(), true) + " -->";
        }
    } else if (wasSet(VEHPARS_ARRIVALPOS_SET)) {
        os.writeAttr(SUMO_ATTR_ARRIVALPOS, myArrivalPos);
    }
    if (myWalkingTime > 0) {
        os.writeAttr(SUMO_ATTR_DURATION, time2string(myWalkingTime));
    } else if (mySpeed > 0) {
        os.writeAttr(SUMO_ATTR_SPEED, mySpeed);
    }
    if (withRouteLength) {
        if (myDeparted >= 0) {
            os.writeAttr("routeLength", walkDistance(true));
        } else {
            os.writeAttr("routeLength", "-1");
        }
    }
    if (myExitTimes != nullptr) {
        std::vector<std::string> exits;
        for (SUMOTime t : *myExitTimes) {
            exits.push_back(time2string(t));
        }
        std::vector<std::string> missing(MAX2(0, (int)myRoute.size() - (int)myExitTimes->size()), "-1");
        exits.insert(exits.end(), missing.begin(), missing.end());
        os.writeAttr("exitTimes", exits);
        os.writeAttr(SUMO_ATTR_STARTED, myDeparted >= 0 ? time2string(myDeparted) : "-1");
        os.writeAttr(SUMO_ATTR_ENDED, myArrived >= 0 ? time2string(myArrived) : "-1");
    }
    os.closeTag(comment);
}


bool
MSStageWalking::moveToNextEdge(MSTransportable* person, SUMOTime currentTime, int prevDir, MSEdge* nextInternal) {
    ((MSEdge*)getEdge())->removeTransportable(person);
    const MSLane* lane = getSidewalk<MSEdge, MSLane>(getEdge());
    const bool arrived = myRouteStep == myRoute.end() - 1;
    if (lane != nullptr) {
        const double tl = person->getVehicleType().getLength();
        const double lastPos = (arrived
                                ? (prevDir == MSPModel::FORWARD
                                   ? getArrivalPos() + tl
                                   : getArrivalPos() - tl)
                                : person->getPositionOnLane());
        activateLeaveReminders(person, lane, lastPos, currentTime, arrived);
    }
    if (myExitTimes != nullptr && nextInternal == nullptr) {
        myExitTimes->push_back(currentTime);
    }
    myMoveReminders.clear();
    myLastEdgeEntryTime = currentTime;
    //std::cout << SIMTIME << " moveToNextEdge person=" << person->getID() << "\n";
    if (myCurrentInternalEdge != nullptr) {
        myInternalDistance += (myPState->getPathLength() == 0 ? myCurrentInternalEdge->getLength() : myPState->getPathLength());
    }
    if (arrived) {
        MSPerson* p = dynamic_cast<MSPerson*>(person);
        if (p->hasInfluencer() && p->getInfluencer().isRemoteControlled()) {
            myCurrentInternalEdge = nextInternal;
            ((MSEdge*) getEdge())->addTransportable(person);
            return false;
        }
        if (myDestinationStop != nullptr) {
            myDestinationStop->addTransportable(person);
        }
        if (!person->proceed(MSNet::getInstance(), currentTime)) {
            MSNet::getInstance()->getPersonControl().erase(person);
        }
        //std::cout << " end walk. myRouteStep=" << (*myRouteStep)->getID() << "\n";
        return true;
    } else {
        if (nextInternal == nullptr) {
            ++myRouteStep;
        }
        myCurrentInternalEdge = nextInternal;
        ((MSEdge*) getEdge())->addTransportable(person);
        return false;
    }
}


void
MSStageWalking::activateLeaveReminders(MSTransportable* person, const MSLane* lane, double lastPos, SUMOTime t, bool arrived) {
    MSMoveReminder::Notification notification = arrived ? MSMoveReminder::NOTIFICATION_ARRIVED : MSMoveReminder::NOTIFICATION_JUNCTION;
    for (MSMoveReminder* const rem : myMoveReminders) {
        rem->updateDetector(*person, 0.0, lane->getLength(), myLastEdgeEntryTime, t, t, true);
        rem->notifyLeave(*person, lastPos, notification);
    }
}


void
MSStageWalking::activateEntryReminders(MSTransportable* person, const bool isDepart) {
    const MSLane* const nextLane = getSidewalk<MSEdge, MSLane>(getEdge());
    if (nextLane != nullptr) {
        for (MSMoveReminder* const rem : nextLane->getMoveReminders()) {
            if (rem->notifyEnter(*person, isDepart ? MSMoveReminder::NOTIFICATION_DEPARTED : MSMoveReminder::NOTIFICATION_JUNCTION, nextLane)) {
                myMoveReminders.push_back(rem);
            }
        }
    }
    if (hasParameter("rerouter")) {
        double minDist = std::numeric_limits<double>::max();
        MSTriggeredRerouter* nearest = nullptr;
        for (MSMoveReminder* const rem : myMoveReminders) {
            MSTriggeredRerouter* rerouter = dynamic_cast<MSTriggeredRerouter*>(rem);
            if (rerouter != nullptr) {
                const double dist2 = rerouter->getPosition().distanceSquaredTo2D(person->getPosition());
                if (dist2 < minDist) {
                    nearest = rerouter;
                    minDist = dist2;
                }
            }
        }
        if (nearest != nullptr) {
            nearest->triggerRouting(*person, MSMoveReminder::NOTIFICATION_JUNCTION);
        }
        // TODO maybe removal of the reminders? Or can we rely on movetonextedge to clean everything up?
    }
}


int
MSStageWalking::getRoutePosition() const {
    return (int)(myRouteStep - myRoute.begin());
}


double
MSStageWalking::getMaxSpeed(const MSTransportable* const person) const {
    return mySpeed >= 0 ? mySpeed : person->getMaxSpeed();
}

std::string
MSStageWalking::getStageSummary(const bool /* isPerson */) const {
    const std::string dest = (getDestinationStop() == nullptr ?
                              " edge '" + getDestination()->getID() + "'" :
                              " stop '" + getDestinationStop()->getID() + "'" + (
                                  getDestinationStop()->getMyName() != "" ? " (" + getDestinationStop()->getMyName() + ")" : ""));
    return "walking to " + dest;
}


void
MSStageWalking::saveState(std::ostringstream& out) {
    out << " " << myDeparted << " " << (myRouteStep - myRoute.begin()) << " " << myLastEdgeEntryTime;
    myPState->saveState(out);
}


void
MSStageWalking::loadState(MSTransportable* transportable, std::istringstream& state) {
    int stepIdx;
    state >> myDeparted >> stepIdx >> myLastEdgeEntryTime;
    myRouteStep = myRoute.begin() + stepIdx;
    myPState = MSNet::getInstance()->getPersonControl().getMovementModel()->loadState(transportable, this, state);
    if (myPState->getLane() && !myPState->getLane()->isNormal()) {
        myCurrentInternalEdge = &myPState->getLane()->getEdge();
        myCurrentInternalEdge->addTransportable(transportable);
    } else {
        (*myRouteStep)->addTransportable(transportable);
    }
}


/****************************************************************************/
