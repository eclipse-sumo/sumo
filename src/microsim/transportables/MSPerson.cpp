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
/// @file    MSPerson.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Mon, 9 Jul 2001
///
// The class for modelling person-movements
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
#include "MSStageWalking.h"
#include "MSPerson.h"


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
* MSPerson::MSPersonStage_Access - methods
* ----------------------------------------------------------------------- */
MSPerson::MSPersonStage_Access::MSPersonStage_Access(const MSEdge* destination, MSStoppingPlace* toStop,
        const double arrivalPos, const double arrivalPosLat, const double dist, const bool isExit,
        const Position& startPos, const Position& endPos) :
    MSStage(MSStageType::ACCESS, destination, toStop, arrivalPos, arrivalPosLat),
    myDist(dist), myAmExit(isExit) {
    myPath.push_back(startPos);
    myPath.push_back(endPos);
}


MSPerson::MSPersonStage_Access::~MSPersonStage_Access() {}

MSStage*
MSPerson::MSPersonStage_Access::clone() const {
    return new MSPersonStage_Access(myDestination, myDestinationStop, myArrivalPos, myArrivalPosLat, myDist, myAmExit, myPath.front(), myPath.back());
}

void
MSPerson::MSPersonStage_Access::proceed(MSNet* net, MSTransportable* person, SUMOTime now, MSStage* /* previous */) {
    myDeparted = now;
    myEstimatedArrival = now + TIME2STEPS(myDist / person->getMaxSpeed());
    // TODO myEstimatedArrival is not a multiple of DELTA_T here. This might give a problem because the destination position will not be reached precisely
    net->getBeginOfTimestepEvents()->addEvent(new ProceedCmd(person, &myDestinationStop->getLane().getEdge()), myEstimatedArrival);
    net->getPersonControl().startedAccess();
    myDestinationStop->getLane().getEdge().addTransportable(person);
}


std::string
MSPerson::MSPersonStage_Access::getStageDescription(const bool /* isPerson */) const {
    return "access";
}


std::string
MSPerson::MSPersonStage_Access::getStageSummary(const bool /* isPerson */) const {
    return (myAmExit ? "access from stop '" : "access to stop '") + getDestinationStop()->getID() + "'";
}


Position
MSPerson::MSPersonStage_Access::getPosition(SUMOTime now) const {
    return myPath.positionAtOffset(myPath.length() * (double)(now - myDeparted) / (double)(myEstimatedArrival - myDeparted));
}


double
MSPerson::MSPersonStage_Access::getAngle(SUMOTime /* now */) const {
    return myPath.angleAt2D(0);
}


double
MSPerson::MSPersonStage_Access::getSpeed() const {
    return myDist / STEPS2TIME(MAX2((SUMOTime)1, myEstimatedArrival - myDeparted));
}

void
MSPerson::MSPersonStage_Access::tripInfoOutput(OutputDevice& os, const MSTransportable* const) const {
    os.openTag("access");
    os.writeAttr("stop", getDestinationStop()->getID());
    os.writeAttr("depart", time2string(myDeparted));
    os.writeAttr("arrival", myArrived >= 0 ? time2string(myArrived) : "-1");
    os.writeAttr("duration", myArrived > 0 ? time2string(getDuration()) : "-1");
    os.writeAttr("routeLength", myDist);
    os.closeTag();
}


SUMOTime
MSPerson::MSPersonStage_Access::ProceedCmd::execute(SUMOTime currentTime) {
    MSNet::getInstance()->getPersonControl().endedAccess();
    myStopEdge->removeTransportable(myPerson);
    if (!myPerson->proceed(MSNet::getInstance(), currentTime)) {
        MSNet::getInstance()->getPersonControl().erase(myPerson);
    }
    return 0;
}


/* -------------------------------------------------------------------------
 * MSPerson - methods
 * ----------------------------------------------------------------------- */
MSPerson::MSPerson(const SUMOVehicleParameter* pars, MSVehicleType* vtype, MSTransportable::MSTransportablePlan* plan, const double speedFactor) :
    MSTransportable(pars, vtype, plan, true),
    myInfluencer(nullptr),
    myChosenSpeedFactor(pars->speedFactor < 0 ? speedFactor : pars->speedFactor)
{ }


MSPerson::~MSPerson() {
    delete myInfluencer;
}


bool
MSPerson::checkAccess(const MSStage* const prior, const bool waitAtStop) {
    MSStoppingPlace* prevStop = prior->getDestinationStop();
    if (!waitAtStop && prior->getStageType() == MSStageType::TRIP) {
        prevStop = prior->getOriginStop();
    }
    if (prevStop != nullptr) {
        const MSEdge* const accessEdge = waitAtStop ? prior->getDestination() : (*myStep)->getFromEdge();
        const MSStoppingPlace::Access* const access = prevStop->getAccess(accessEdge);
        if (access != nullptr) {
            const MSLane* const lane = accessEdge->getLanes()[0];
            MSStage* newStage = nullptr;
            if (waitAtStop) {
                const MSEdge* const stopEdge = &prevStop->getLane().getEdge();
                const double arrivalAtBs = (prevStop->getBeginLanePosition() + prevStop->getEndLanePosition()) / 2;
                newStage = new MSPersonStage_Access(stopEdge, prevStop, arrivalAtBs, 0.0, access->length, false,
                                                    lane->geometryPositionAtOffset(access->endPos),
                                                    prevStop->getLane().geometryPositionAtOffset(arrivalAtBs));
            } else {
                const bool useDoors = access->exit == MSStoppingPlace::AccessExit::DOORS ||
                                      (OptionsCont::getOptions().getString("pedestrian.model") != "jupedsim" && access->exit == MSStoppingPlace::AccessExit::CARRIAGE);
                if (access->exit == MSStoppingPlace::AccessExit::CARRIAGE) {
                    const double startPos = prior->getStageType() == MSStageType::TRIP ? prior->getEdgePos(0) : prior->getArrivalPos();
                    const double startPosLat = prior->getStageType() == MSStageType::TRIP ? prior->getEdgePosLat(0) : prior->getArrivalPosLat();
                    // The start and end attributes of the access stage are equal in this case, but we need to compute the arrival position relatively
                    // to the current lane and not the lane of the previous stage.
                    const Position start = prevStop->getLane().geometryPositionAtOffset(startPos, startPosLat);
                    const Position end = lane->getShape().transformToVectorCoordinates(start);
                    newStage = new MSPersonStage_Access(accessEdge, prevStop, end.x(), -end.y(), access->length, true, start, start);
                } else {
                    const double startPos = prior->getStageType() == MSStageType::TRIP ? prior->getEdgePos(0) : prior->getArrivalPos();
                    const Position& trainExit = prevStop->getLane().geometryPositionAtOffset(startPos);
                    const double arrivalPos = useDoors ? lane->getShape().nearest_offset_to_point2D(trainExit) : access->endPos;
                    Position platformEntry = lane->geometryPositionAtOffset(arrivalPos);
                    if (useDoors) {
                        // find the closer side of the platform to enter
                        const double halfWidth = lane->getWidth() / 2. - MAX2(getVehicleType().getLength(), getVehicleType().getWidth()) / 2. - POSITION_EPS;
                        platformEntry = lane->geometryPositionAtOffset(arrivalPos, halfWidth);
                        const Position& plat2 = lane->geometryPositionAtOffset(arrivalPos, -halfWidth);
                        if (trainExit.distanceSquaredTo2D(plat2) < trainExit.distanceSquaredTo2D(platformEntry)) {
                            platformEntry = plat2;
                        }
                    }
                    newStage = new MSPersonStage_Access(accessEdge, prevStop, arrivalPos, 0.0, access->length, true,
                                                        trainExit, platformEntry);
                }
            }
            newStage->setTrip(prior->getTrip());
            myStep = myPlan->insert(myStep, newStage);
            return true;
        }
    }
    return false;
}


double
MSPerson::getImpatience() const {
    return MAX2(0., MIN2(1., getVehicleType().getImpatience()
                         + STEPS2TIME((*myStep)->getWaitingTime(SIMSTEP)) / MSPModel_Striping::MAX_WAIT_TOLERANCE));
}

const std::string&
MSPerson::getNextEdge() const {
//    if (getCurrentStageType() == WALKING) {
//        MSStageWalking* walkingStage =  dynamic_cast<MSStageWalking*>(*myStep);
//        assert(walkingStage != 0);
//        const MSEdge* nextEdge = walkingStage->getPedestrianState()->getNextEdge(*walkingStage);
//        if (nextEdge != 0) {
//            return nextEdge->getID();
//        }
//    }
//    return StringUtils::emptyString;
    const MSEdge* nextEdge = getNextEdgePtr();
    if (nextEdge != nullptr) {
        return nextEdge->getID();
    }
    return StringUtils::emptyString;
}


const MSEdge*
MSPerson::getNextEdgePtr() const {
    if (getCurrentStageType() == MSStageType::WALKING) {
        MSStageWalking* walkingStage =  dynamic_cast<MSStageWalking*>(*myStep);
        assert(walkingStage != nullptr);
        return walkingStage->getPState()->getNextEdge(*walkingStage);
    }
    return nullptr;
}



void
MSPerson::replaceWalk(const ConstMSEdgeVector& newEdges, double departPos, int firstIndex, int nextIndex) {
    assert(nextIndex > firstIndex);
    //std::cout << SIMTIME << " reroute person " << getID()
    //    << "  newEdges=" << toString(newEdges)
    //    << " firstIndex=" << firstIndex
    //    << " nextIndex=" << nextIndex
    //    << " departPos=" << getEdgePos()
    //    << " arrivalPos=" <<  getNextStage(nextIndex - 1)->getArrivalPos()
    //    << "\n";
    MSStage* const toBeReplaced = getNextStage(nextIndex - 1);
    MSStageWalking* newStage = new MSStageWalking(getID(), newEdges,
            toBeReplaced->getDestinationStop(), -1,
            -1,
            departPos,
            toBeReplaced->getArrivalPos(),
            MSPModel::UNSPECIFIED_POS_LAT);
    appendStage(newStage, nextIndex);
    // remove stages in reverse order so that proceed will only be called at the last removal
    for (int i = nextIndex - 1; i >= firstIndex; i--) {
        //std::cout << " removeStage=" << i << "\n";
        removeStage(i);
    }
}


MSPerson::Influencer&
MSPerson::getInfluencer() {
    if (myInfluencer == nullptr) {
        myInfluencer = new Influencer();
    }
    return *myInfluencer;
}


const MSPerson::Influencer*
MSPerson::getInfluencer() const {
    return myInfluencer;
}



/* -------------------------------------------------------------------------
 * methods of MSPerson::Influencer
 * ----------------------------------------------------------------------- */
MSPerson::Influencer::Influencer() {}


MSPerson::Influencer::~Influencer() {}


void
MSPerson::Influencer::setRemoteControlled(Position xyPos, MSLane* l, double pos, double posLat, double angle, int edgeOffset, const ConstMSEdgeVector& route, SUMOTime t) {
    myRemoteXYPos = xyPos;
    myRemoteLane = l;
    myRemotePos = pos;
    myRemotePosLat = posLat;
    myRemoteAngle = angle;
    myRemoteEdgeOffset = edgeOffset;
    myRemoteRoute = route;
    myLastRemoteAccess = t;
}


bool
MSPerson::Influencer::isRemoteControlled() const {
    return myLastRemoteAccess == MSNet::getInstance()->getCurrentTimeStep();
}


bool
MSPerson::Influencer::isRemoteAffected(SUMOTime t) const {
    return myLastRemoteAccess >= t - TIME2STEPS(10);
}


void
MSPerson::Influencer::postProcessRemoteControl(MSPerson* p) {
    /*
    std::cout << SIMTIME << " moveToXY person=" << p->getID()
        << " xyPos=" << myRemoteXYPos
        << " lane=" << Named::getIDSecure(myRemoteLane)
        << " pos=" << myRemotePos
        << " posLat=" << myRemotePosLat
        << " angle=" << myRemoteAngle
        << " eOf=" << myRemoteEdgeOffset
        << " route=" << toString(myRemoteRoute)
        << " aTime=" << time2string(myLastRemoteAccess)
        << "\n";
        */
    switch (p->getStageType(0)) {
        case MSStageType::WALKING: {
            MSStageWalking* s = dynamic_cast<MSStageWalking*>(p->getCurrentStage());
            assert(s != nullptr);
            s->getPState()->moveToXY(p, myRemoteXYPos, myRemoteLane, myRemotePos, myRemotePosLat,
                                     myRemoteAngle, myRemoteEdgeOffset, myRemoteRoute,
                                     MSNet::getInstance()->getCurrentTimeStep());
        }
        break;
        default:
            break;
    }
}


/****************************************************************************/
