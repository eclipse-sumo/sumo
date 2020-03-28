/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2014-2020 German Aerospace Center (DLR) and others.
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
/// @file    MSPModel_Remote.cpp
/// @author  Gregor Laemmel
/// @date    Mon, 13 Jan 2014
///
// The pedestrian following model for remote controlled pedestrian movement
/****************************************************************************/

#include "MSPModel_Remote.h"
#include <grpc++/grpc++.h>
#include "microsim/MSEdge.h"
#include "microsim/MSLane.h"
#include "microsim/MSEdgeControl.h"
#include "microsim/MSEventControl.h"
#include "microsim/MSGlobals.h"
#include "hybridsim.grpc.pb.h"
#include "utils/geom/Position.h"
#include "utils/geom/PositionVector.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

MSPModel_Remote::MSPModel_Remote(const OptionsCont& oc, MSNet* net)
    : myNet(net) {
    const std::string address = oc.getString("pedestrian.remote.address");
    myHybridsimStub = hybridsim::HybridSimulation::NewStub(grpc::CreateChannel(
                          address, grpc::InsecureChannelCredentials()));

    initialize();

    Event* e = new Event(this);
    net->getBeginOfTimestepEvents()->addEvent(e, net->getCurrentTimeStep() + DELTA_T);
}
PedestrianState* MSPModel_Remote::add(MSPerson* person, MSPerson::MSPersonStage_Walking* stage, SUMOTime now) {

    assert(person->getCurrentStageType() == MSTransportable::MOVING_WITHOUT_VEHICLE);

    PState* state = new PState(person, stage);

    hybridsim::Agent req;
    int id = myLastId++;
    remoteIdPStateMapping[id] = state;
    req.set_id(id);

    MSLane* departureLane = getFirstPedestrianLane(*(stage->getRoute().begin()));
    double departureOffsetAlongLane = stage->getDepartPos();

    //TODO fix this on casim side [GL]
    double offset = departureOffsetAlongLane == 0 ? 0.4 : -0.4;
    departureOffsetAlongLane += offset;

    Position departurePos = departureLane->getShape().positionAtOffset(departureOffsetAlongLane);
    hybridsim::Coordinate* departureCoordinate = req.mutable_enterlocation();
    departureCoordinate->set_x(departurePos.x());
    departureCoordinate->set_y(departurePos.y());

    MSLane* arrivalLane = getFirstPedestrianLane(*(stage->getRoute().end() - 1));
    double arrivalOffsetAlongLange = stage->getArrivalPos();
    Position arrivalPos = arrivalLane->getShape().positionAtOffset(arrivalOffsetAlongLange);
    hybridsim::Coordinate* arrivalCoordinate = req.mutable_leavelocation();
    arrivalCoordinate->set_x(arrivalPos.x());
    arrivalCoordinate->set_y(arrivalPos.y());


    const MSEdge* prv = 0;
    for (ConstMSEdgeVector::const_iterator it = stage->getRoute().begin(); it != stage->getRoute().end(); it++) {
        const MSEdge* edge = *it;
        int dir = FORWARD;//default
        if (prv == 0) {
            if (stage->getRoute().size() > 1) {
                const MSEdge* nxt = *(it + 1);
                dir = (edge->getFromJunction() == nxt->getFromJunction()
                       || edge->getFromJunction() == nxt->getToJunction()) ? BACKWARD : FORWARD;
            } else {
                dir = stage->getDepartPos() == 0 ? FORWARD : BACKWARD;
            }
        } else {
            dir = (edge->getFromJunction() == prv->getToJunction()
                   || edge->getFromJunction() == prv->getFromJunction()) ? FORWARD : BACKWARD;
        }
        if (edgesTransitionsMapping.find(edge) == edgesTransitionsMapping.end()) {
            throw ProcessError("Cannot map edge : " + edge->getID() + " to remote simulation");
        };
        std::tuple<int, int> transitions = edgesTransitionsMapping[edge];

        int frId = dir == FORWARD ? std::get<0>(transitions) : std::get<1>(transitions);
        int toId = dir == FORWARD ? std::get<1>(transitions) : std::get<0>(transitions);
        hybridsim::Destination* destFr = req.add_dests();
        destFr->set_id(frId);
        hybridsim::Destination* destTo = req.add_dests();
        destTo->set_id(toId);
        prv = edge;
    }

    hybridsim::Boolean rpl;
    ClientContext context;
    Status st = myHybridsimStub->transferAgent(&context, req, &rpl);
    if (!st.ok()) {
        throw ProcessError("Person: " + person->getID() + " could not be transferred to remote simulation");
    }
    if (!rpl.val()) {
        //TODO not yet implemented
        throw ProcessError("Remote simulation declined to accept person: " + person->getID() + ".");
    }

    return state;
}

MSPModel_Remote::~MSPModel_Remote() {

    hybridsim::Empty req;
    hybridsim::Empty rpl;
    ClientContext context1;
    Status st = myHybridsimStub->shutdown(&context1, req, &rpl);
    if (!st.ok()) {
        throw ProcessError("Could not shutdown remote server");
    }


}

SUMOTime MSPModel_Remote::execute(SUMOTime time) {

    hybridsim::LeftClosedRightOpenTimeInterval interval;
    interval.set_fromtimeincluding(time / DELTA_T);
    interval.set_totimeexcluding((time + DELTA_T) / DELTA_T);


    //1. simulate time interval
    hybridsim::Empty rpl;
    ClientContext context1;
    Status st = myHybridsimStub->simulatedTimeInerval(&context1, interval, &rpl);
    if (!st.ok()) {
        throw ProcessError("Could not simulated time interval from: " + toString(time) + " to: " + toString(time + DELTA_T));
    }

    //2. receive trajectories
    hybridsim::Empty req2;
    hybridsim::Trajectories trajectories;
    ClientContext context2;
    Status st2 = myHybridsimStub->receiveTrajectories(&context2, req2, &trajectories);
    if (!st2.ok()) {
        throw ProcessError("Could not receive trajectories from remote simulation");
    }
    for (hybridsim::Trajectory trajectory : trajectories.trajectories()) {
        if (remoteIdPStateMapping.find(trajectory.id()) != remoteIdPStateMapping.end()) {
            PState* pState = remoteIdPStateMapping[trajectory.id()];
            pState->setPosition(trajectory.x(), trajectory.y());
            pState->setPhi(trajectory.phi());
            if (transitionsEdgesMapping.find(trajectory.currentdest().id()) != transitionsEdgesMapping.end()) {
                const MSEdge* nextTargetEdge = transitionsEdgesMapping[trajectory.currentdest().id()];
                const MSEdge* nextStageEdge = pState->getStage()->getNextRouteEdge();
//                const MSEdge* currentStageEdge = pState->getStage()->getEdge();
                if (nextTargetEdge == nextStageEdge) {
                    const bool arrived = pState->getStage()->moveToNextEdge(pState->getPerson(), time);
                    std::cout << "next edge" << std::endl;
                }
            }
//            pState.
        } else {
            throw ProcessError("Pedestrian with id: " + toString(trajectory.id()) + " is not known.");
        }
    }

    //3. retrieve agents that are ready to come back home to SUMO
    hybridsim::Empty req3;
    hybridsim::Agents agents;
    ClientContext context3;
    Status st3 = myHybridsimStub->queryRetrievableAgents(&context3, req3, &agents);
    if (!st3.ok()) {
        throw ProcessError("Could not query retrievable agents");
    }
    //TODO check whether agents can be retrieved
    for (hybridsim::Agent agent : agents.agents()) {
        if (remoteIdPStateMapping.find(agent.id()) != remoteIdPStateMapping.end()) {
            PState* pState = remoteIdPStateMapping[agent.id()];
            while (!pState->getStage()->moveToNextEdge(pState->getPerson(), time)) {
                remoteIdPStateMapping.erase(agent.id());
            }
        }
    }

    //4. confirm transferred agents
    hybridsim::Empty rpl2;
    ClientContext context4;
    Status st4 = myHybridsimStub->confirmRetrievedAgents(&context4, agents, &rpl2);
    if (!st4.ok()) {
        throw ProcessError("Could not confirm retrieved agents");
    }

    return DELTA_T;
}
MSLane* MSPModel_Remote::getFirstPedestrianLane(const MSEdge* const& edge) {
    for (MSLane* lane : edge->getLanes()) {
        if (lane->getPermissions() == SVC_PEDESTRIAN) {
            return lane;
        }
    }
    throw ProcessError("Edge: " + edge->getID() + " does not allow pedestrians.");
}

void MSPModel_Remote::remove(PedestrianState* state) {

}

void MSPModel_Remote::initialize() {
    hybridsim::Scenario req;

    //1. for all edges
    for (MSEdge* e : myNet->getEdgeControl().getEdges()) {
        if (e->isInternal()) {
            continue;
        }
        if (e->isWalkingArea()) {
            handleWalkingArea(e, req);
            continue;
        }
        for (MSLane* l : e->getLanes()) {
            if (l->getPermissions() == SVC_PEDESTRIAN) {
                handlePedestrianLane(l, req);
            }
        }
    }

    //add boundary
    hybridsim::Edge* edge = req.add_edges();
    edge->mutable_c0()->set_x(myBoundary.xmin());
    edge->mutable_c0()->set_y(myBoundary.ymin());
    edge->mutable_c1()->set_x(myBoundary.xmax());
    edge->mutable_c1()->set_y(myBoundary.ymin());
    edge->set_type(hybridsim::Edge_Type_OBSTACLE);

    edge = req.add_edges();
    edge->mutable_c0()->set_x(myBoundary.xmax());
    edge->mutable_c0()->set_y(myBoundary.ymin());
    edge->mutable_c1()->set_x(myBoundary.xmax());
    edge->mutable_c1()->set_y(myBoundary.ymax());
    edge->set_type(hybridsim::Edge_Type_OBSTACLE);

    edge = req.add_edges();
    edge->mutable_c0()->set_x(myBoundary.xmax());
    edge->mutable_c0()->set_y(myBoundary.ymax());
    edge->mutable_c1()->set_x(myBoundary.xmin());
    edge->mutable_c1()->set_y(myBoundary.ymax());
    edge->set_type(hybridsim::Edge_Type_OBSTACLE);

    edge = req.add_edges();
    edge->mutable_c0()->set_x(myBoundary.xmin());
    edge->mutable_c0()->set_y(myBoundary.ymax());
    edge->mutable_c1()->set_x(myBoundary.xmin());
    edge->mutable_c1()->set_y(myBoundary.ymin());
    edge->set_type(hybridsim::Edge_Type_OBSTACLE);


    hybridsim::Empty rpl;
    ClientContext context;
    Status st = myHybridsimStub->initScenario(&context, req, &rpl);
    if (!st.ok()) {
        throw ProcessError("Remote side could not initialize scenario!");
    }

}
void MSPModel_Remote::handleWalkingArea(MSEdge* msEdge, hybridsim::Scenario& scenario) {
    MSLane* l = *(msEdge->getLanes().begin());

    const PositionVector shape = l->getShape();
    if (shape.size() < 2) {//should never happen
        return;
    }

    handleShape(shape, scenario);


    //close walking area
    Position frst = *shape.begin();
    Position scnd = *(shape.end() - 1);
    hybridsim::Edge* edge = scenario.add_edges();
    edge->mutable_c0()->set_x(frst.x());
    edge->mutable_c0()->set_y(frst.y());
    edge->mutable_c1()->set_x(scnd.x());
    edge->mutable_c1()->set_y(scnd.y());
    edge->set_type(hybridsim::Edge_Type_OBSTACLE);


}
void MSPModel_Remote::handlePedestrianLane(MSLane* l, hybridsim::Scenario& scenario) {
    double width = l->getWidth();
    PositionVector centerLine = PositionVector(l->getShape());
    if (centerLine.size() < 2) {//should never happen
        return;
    }


    int fromId = myLastTransitionId++;
    int toId = myLastTransitionId++;
    edgesTransitionsMapping[&(l->getEdge())] = std::make_tuple(fromId, toId);
    transitionsEdgesMapping[fromId] = &(l->getEdge());
    transitionsEdgesMapping[toId] = &(l->getEdge());

    hybridsim::Edge_Type edgeType;
    if (l->getEdge().isCrossing()) {
        edgeType = hybridsim::Edge_Type_TRANSITION_HOLDOVER;
    } else if (l->getEdge().isWalkingArea()) {
        edgeType = hybridsim::Edge_Type_TRANSITION_INTERNAL;
    } else {
        edgeType = hybridsim::Edge_Type_TRANSITION;
    }

    //start and end
    Position frst = *centerLine.begin();
    Position scnd = *(centerLine.begin() + 1);
    makeStartOrEndTransition(frst, scnd, width, scenario, edgeType, fromId);
    Position thrd = *(centerLine.end() - 1);
    Position frth = *(centerLine.end() - 2);
    makeStartOrEndTransition(thrd, frth, width, scenario, edgeType, toId);

    centerLine.move2side(-width / 2.);
    handleShape(centerLine, scenario);
    centerLine.move2side(width);
    handleShape(centerLine, scenario);

}
void
MSPModel_Remote::makeStartOrEndTransition(Position frst, Position scnd, double width, hybridsim::Scenario& scenario,
        hybridsim::Edge_Type type, int id) {

    double dx = scnd.x() - frst.x();
    double dy = scnd.y() - frst.y();
    double length = sqrt(dx * dx + dy * dy);
    dx /= length;
    dy /= length;
    double x0 = frst.x() - dy * width / 2.;
    double y0 = frst.y() + dx * width / 2.;
    double x1 = frst.x() + dy * width / 2.;
    double y1 = frst.y() - dx * width / 2.;
    hybridsim::Edge* edge = scenario.add_edges();
    edge->mutable_c0()->set_x(x0);
    edge->mutable_c0()->set_y(y0);
    edge->mutable_c1()->set_x(x1);
    edge->mutable_c1()->set_y(y1);
    edge->set_type(type);
    edge->set_id(id);

}
void MSPModel_Remote::handleShape(const PositionVector& shape, hybridsim::Scenario& scenario) {
    PositionVector::const_iterator it = shape.begin();
    Position frst = *it;

    myBoundary.add(frst.x(), frst.y());
    it++;
    for (; it != shape.end(); it++) {
        hybridsim::Edge* edge = scenario.add_edges();
        edge->mutable_c0()->set_x(frst.x());
        edge->mutable_c0()->set_y(frst.y());
        edge->mutable_c1()->set_x((*it).x());
        edge->mutable_c1()->set_y((*it).y());
        edge->set_type(hybridsim::Edge_Type_OBSTACLE);
        frst = *it;
        myBoundary.add(frst.x(), frst.y());
    }
}



// ===========================================================================
// MSPModel_Remote::PState method definitions
// ===========================================================================
MSPModel_Remote::PState::PState(MSPerson* person, MSPerson::MSPersonStage_Walking* stage)
    : myPerson(person), myPhi(0), myPosition(0, 0), myStage(stage) {


}
MSPModel_Remote::PState::~PState() {

}
double MSPModel_Remote::PState::getEdgePos(const MSPerson::MSPersonStage_Walking& stage, SUMOTime now) const {
    return 0;
}
Position MSPModel_Remote::PState::getPosition(const MSPerson::MSPersonStage_Walking& stage, SUMOTime now) const {
    return myPosition;
}
double MSPModel_Remote::PState::getAngle(const MSPerson::MSPersonStage_Walking& stage, SUMOTime now) const {
    return myPhi;
}
SUMOTime MSPModel_Remote::PState::getWaitingTime(const MSPerson::MSPersonStage_Walking& stage, SUMOTime now) const {
    return 0;
}
double MSPModel_Remote::PState::getSpeed(const MSPerson::MSPersonStage_Walking& stage) const {
    return 0;
}
const MSEdge* MSPModel_Remote::PState::getNextEdge(const MSPerson::MSPersonStage_Walking& stage) const {
    return nullptr;
}
void MSPModel_Remote::PState::setPosition(double x, double y) {
    myPosition.set(x, y);
}
void MSPModel_Remote::PState::setPhi(double phi) {
    myPhi = phi;
}
MSPerson::MSPersonStage_Walking* MSPModel_Remote::PState::getStage() {
    return myStage;
}
MSPerson* MSPModel_Remote::PState::getPerson() {
    return myPerson;
}

bool
MSPModel_Remote::usingInternalLanes() {
    return MSGlobals::gUsingInternalLanes && MSNet::getInstance()->hasInternalLinks();
}

