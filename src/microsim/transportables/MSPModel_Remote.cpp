/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2014-2022 German Aerospace Center (DLR) and others.
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

#include <algorithm>
#include <jupedsim/jupedsim.h>
#include "microsim/MSEdge.h"
#include "microsim/MSLane.h"
#include "microsim/MSEdgeControl.h"
#include "microsim/MSEventControl.h"
#include "utils/geom/Position.h"
#include "utils/geom/PositionVector.h"
#include "MSPModel_Remote.h"


struct JPS_ErrorMessage_t {
    std::string message;
};


const double MSPModel_Remote::JPS_AREA_RATIO = 0.1;
const SUMOTime MSPModel_Remote::JPS_DELTA_T = 10;
const double MSPModel_Remote::JPS_EXIT_TOLERANCE = 1;


MSPModel_Remote::MSPModel_Remote(const OptionsCont& oc, MSNet* net) : myNet(net) {
    initialize();
    Event* e = new Event(this);
    net->getBeginOfTimestepEvents()->addEvent(e, net->getCurrentTimeStep() + DELTA_T);
}


MSPModel_Remote::~MSPModel_Remote() {
    JPS_Simulation_Free(mySimulation);
    JPS_OperationalModel_Free(myModel);
    JPS_Areas_Free(myAreas);
    JPS_AreasBuilder_Free(myAreasBuilder);
    JPS_Geometry_Free(myGeometry);
    JPS_GeometryBuilder_Free(myGeometryBuilder);
}


MSTransportableStateAdapter*
MSPModel_Remote::add(MSTransportable* person, MSStageMoving* stage, SUMOTime now) {
	assert(person->getCurrentStageType() == MSStageType::WALKING);
	
    Position departurePosition = myLane->getShape().positionAtOffset(stage->getDepartPos());
    departurePosition.add(0.1, 0, 0);
    Position arrivalPosition = myLane->getShape().positionAtOffset(stage->getArrivalPos());
    arrivalPosition.sub(0.1, 0.0);

	JPS_Waypoint waypoints[] = { {{arrivalPosition.x(), arrivalPosition.y()}, JPS_EXIT_TOLERANCE} };
	JPS_Journey journey = JPS_Journey_Create_SimpleJourney(waypoints, sizeof(waypoints));
    JPS_JourneyId journeyId = JPS_Simulation_AddJourney(mySimulation, journey, nullptr);

	JPS_AgentParameters agent_parameters{};
	agent_parameters.v0 = 1.0;
	agent_parameters.AMin = 0.15;
	agent_parameters.BMax = 0.15;
	agent_parameters.BMin = 0.15;
	agent_parameters.Av = 0.53;
	agent_parameters.T = 1;
	agent_parameters.Tau = 0.5;
	agent_parameters.journeyId = journeyId;
	agent_parameters.orientationX = 1.0;
	agent_parameters.orientationY = 0.0;
	agent_parameters.positionX = departurePosition.x();
	agent_parameters.positionY = departurePosition.y();

    JPS_AgentId agentId = JPS_Simulation_AddAgent(mySimulation, agent_parameters, nullptr);
    PState* state = new PState(static_cast<MSPerson*>(person), stage, journey, arrivalPosition, agentId);
	myPedestrianStates.push_back(state);
    myNumActivePedestrians++;
		
    /*assert(person->getCurrentStageType() == MSStageType::WALKING);

    PState* state = new PState(static_cast<MSPerson*>(person), stage);

    // JPS_Agent req;
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
    */
    return state;
}


void
MSPModel_Remote::remove(MSTransportableStateAdapter* state) {
    // This function is called only when using TraCI.
    // Not sure what to do here.
}


SUMOTime
MSPModel_Remote::execute(SUMOTime time) {
    int nbrIterations = (int)(DELTA_T / JPS_DELTA_T);
	for (int i = 0; i < nbrIterations; ++i)
	{
		JPS_ErrorMessage mess = new JPS_ErrorMessage_t{};
		bool ok = JPS_Simulation_Iterate(mySimulation, &mess);
        if (!ok) {
            std::cout << "Error during iteration " << i << ": " << JPS_ErrorMessage_GetMessage(mess) << std::endl;
        }
        JPS_ErrorMessage_Free(mess);

        for (PState* state : myPedestrianStates)
        {
            // Updates the agent position.
            JPS_Agent agent = JPS_Simulation_ReadAgent(mySimulation, state->getAgentId(), nullptr);
            double newPositionX = JPS_Agent_PositionX(agent);
            double newPositionY = JPS_Agent_PositionY(agent);
            state->setPosition(newPositionX, newPositionY);

            // If near the last waypoint, remove the agent.
            Position newPosition(newPositionX, newPositionY);
            if (newPosition.distanceTo2D(state->getDestination()) < JPS_EXIT_TOLERANCE) {
                registerArrived();
                JPS_Simulation_RemoveAgent(mySimulation, state->getAgentId(), nullptr);
                myPedestrianStates.erase(std::find(myPedestrianStates.begin(), myPedestrianStates.end(), state));
                dynamic_cast<MSStageMoving*>(state->getPerson()->getCurrentStage())->moveToNextEdge(state->getPerson(), time, 1, nullptr);
                break;
            }

            double newOrientationX = JPS_Agent_OrientationX(agent);
            double newOrientationY = JPS_Agent_OrientationY(agent);
            state->setAngle(atan2(newOrientationY, newOrientationX));
        }
	}
	
	/*
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
    */
    return DELTA_T;
}


bool
MSPModel_Remote::usingInternalLanes() {
    return MSGlobals::gUsingInternalLanes && MSNet::getInstance()->hasInternalLinks();
}


void MSPModel_Remote::registerArrived() {
    myNumActivePedestrians--;
}


int MSPModel_Remote::getActiveNumber() {
    return myNumActivePedestrians;
}


void MSPModel_Remote::clearState() {
    // Not sure if useful because it seems the reload button triggers
    // the creation of a new MSPModel_Remote object.
    myNumActivePedestrians = 0;
    myPedestrianStates.clear();
}


MSLane*
MSPModel_Remote::getFirstPedestrianLane(const MSEdge* const& edge) {
    for (MSLane* lane : edge->getLanes()) {
        if (lane->getPermissions() == SVC_PEDESTRIAN) {
            return lane;
        }
    }
	return nullptr;
}


void
MSPModel_Remote::initialize() {
	myGeometryBuilder = JPS_GeometryBuilder_Create();
    myAreasBuilder = JPS_AreasBuilder_Create();

	MSEdgeVector edges = (myNet->getEdgeControl()).getEdges();
	for (const MSEdge* const edge : edges) {
		MSLane* lane = getFirstPedestrianLane(edge);
		if (lane) {
            double amount = lane->getWidth() / 2.0;
            PositionVector shape = lane->getShape();
            Position leftPoint = shape[0];
			Position rightPoint = shape[1];
			
			double angle = leftPoint.angleTo2D(rightPoint);
			Position toRotate = leftPoint + Position(amount * cos(angle), amount * sin(angle));
			Position topLeftCorner = toRotate.rotateAround2D(M_PI / 2.0, leftPoint);
			Position bottomLeftCorner = toRotate.rotateAround2D(-M_PI / 2.0, leftPoint);
			
			angle = rightPoint.angleTo2D(leftPoint);
			toRotate = rightPoint + Position(amount * cos(angle), amount * sin(angle));
			Position topRightCorner = toRotate.rotateAround2D(-M_PI / 2.0, rightPoint);
			Position bottomRightCorner = toRotate.rotateAround2D(M_PI / 2.0, rightPoint);
            
			std::vector<Position> lanePolygon{ topLeftCorner, bottomLeftCorner, bottomRightCorner, topRightCorner };
			std::vector<double> lanePolygonCoordinates;
			for (const Position& position : lanePolygon) {
				lanePolygonCoordinates.push_back(position.x());
				lanePolygonCoordinates.push_back(position.y());
			}

			JPS_GeometryBuilder_AddAccessibleArea(myGeometryBuilder, lanePolygonCoordinates.data(), lanePolygonCoordinates.size() / 2);

            // Temporary!
			myLane = lane;
			break;
		}
	}

    // The line below doesn't work on Windows at least.
    // JPS_ErrorMessage mess{}; 
    JPS_ErrorMessage mess = new JPS_ErrorMessage_t{};
    myGeometry = JPS_GeometryBuilder_Build(myGeometryBuilder, &mess);
    if (myGeometry == nullptr) {
        std::cout << "Error while creating the geometry: " << JPS_ErrorMessage_GetMessage(mess) << std::endl;
    }
    JPS_ErrorMessage_Free(mess);

    // Areas are built (although unused) because the JPS_Simulation object needs them.
	myAreas = JPS_AreasBuilder_Build(myAreasBuilder, nullptr);

    mess = new JPS_ErrorMessage_t{};
	myModel = JPS_OperationalModel_Create_VelocityModel(8, 0.1, 5, 0.02, nullptr);
    if (myModel == nullptr) {
        std::cout << "Error while creating the pedestrian model: " << JPS_ErrorMessage_GetMessage(mess) << std::endl;
    }
    JPS_ErrorMessage_Free(mess);

    mess = new JPS_ErrorMessage_t{};
	mySimulation = JPS_Simulation_Create(myModel, myGeometry, myAreas, STEPS2TIME(JPS_DELTA_T), nullptr);
    if (mySimulation == nullptr) {
        std::cout << "Error while creating the simulation: " << JPS_ErrorMessage_GetMessage(mess) << std::endl;
    }
    JPS_ErrorMessage_Free(mess);
}


// ===========================================================================
// MSPModel_Remote::PState method definitions
// ===========================================================================
MSPModel_Remote::PState::PState(MSPerson* person, MSStageMoving* stage, JPS_Journey journey, Position destination, JPS_AgentId agentId)
    : myPerson(person), myAngle(0), myPosition(0, 0), myStage(stage), myJourney(journey), myDestination(destination), myAgentId(agentId) {
}


MSPModel_Remote::PState::~PState() {
    JPS_Journey_Free(myJourney);
}


Position MSPModel_Remote::PState::getPosition(const MSStageMoving& stage, SUMOTime now) const {
    return myPosition;
}


void MSPModel_Remote::PState::setPosition(double x, double y) {
    myPosition.set(x, y);
}


double MSPModel_Remote::PState::getAngle(const MSStageMoving& stage, SUMOTime now) const {
    return myAngle;
}


void MSPModel_Remote::PState::setAngle(double angle) {
	myAngle = angle;
}


MSStageMoving* MSPModel_Remote::PState::getStage() {
    return myStage;
}


MSPerson* MSPModel_Remote::PState::getPerson() {
    return myPerson;
}


double MSPModel_Remote::PState::getEdgePos(const MSStageMoving& stage, SUMOTime now) const {
    return 0;
}


int MSPModel_Remote::PState::getDirection(const MSStageMoving& stage, SUMOTime now) const {
    return UNDEFINED_DIRECTION;
}


SUMOTime MSPModel_Remote::PState::getWaitingTime(const MSStageMoving& stage, SUMOTime now) const {
    return 0;
}


double MSPModel_Remote::PState::getSpeed(const MSStageMoving& stage) const {
    return 0;
}


const MSEdge* MSPModel_Remote::PState::getNextEdge(const MSStageMoving& stage) const {
    return nullptr;
}


Position MSPModel_Remote::PState::getDestination(void) const {
    return myDestination;
}


JPS_AgentId MSPModel_Remote::PState::getAgentId(void) const {
    return myAgentId;
}