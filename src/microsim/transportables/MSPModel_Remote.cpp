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
#include <fstream>
#include <jupedsim/jupedsim.h>
#include "microsim/MSEdge.h"
#include "microsim/MSLane.h"
#include "microsim/MSLink.h"
#include "microsim/MSEdgeControl.h"
#include "microsim/MSEventControl.h"
#include "utils/geom/Position.h"
#include "utils/geom/PositionVector.h"
#include "MSPModel_Remote.h"
#include "MSPerson.h"


struct JPS_ErrorMessage_t {
    std::string message;
};


const SUMOTime MSPModel_Remote::JPS_DELTA_T = 10;
const double MSPModel_Remote::JPS_EXIT_TOLERANCE = 1;


MSPModel_Remote::MSPModel_Remote(const OptionsCont& oc, MSNet* net) : myNet(net) {
    initialize();
    Event* e = new Event(this);
    net->getBeginOfTimestepEvents()->addEvent(e, net->getCurrentTimeStep() + DELTA_T);
}


MSPModel_Remote::~MSPModel_Remote() {
    for (PState* state : myPedestrianStates) {
        delete state;
        state = nullptr;
    }
    JPS_Simulation_Free(mySimulation);
    JPS_OperationalModel_Free(myModel);
    JPS_Areas_Free(myAreas);
    JPS_AreasBuilder_Free(myAreasBuilder);
    JPS_Geometry_Free(myGeometry);
    JPS_GeometryBuilder_Free(myGeometryBuilder);

#ifdef DEBUG
    myTrajectoryDumpFile.close();
#endif
}


MSTransportableStateAdapter*
MSPModel_Remote::add(MSTransportable* person, MSStageMoving* stage, SUMOTime now) {
	assert(person->getCurrentStageType() == MSStageType::WALKING);
	
    const MSLane* departureLane = getSidewalk<MSEdge, MSLane>(stage->getRoute().front());
    Position departurePosition = departureLane->getShape().positionAtOffset(stage->getDepartPos());
    
    const MSLane* arrivalLane = getSidewalk<MSEdge, MSLane>(stage->getRoute().back());
    Position arrivalPosition = arrivalLane->getShape().positionAtOffset(stage->getArrivalPos());

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
		
    return state;
}


void
MSPModel_Remote::remove(MSTransportableStateAdapter* state) {
    // This function is called only when using TraCI.
    // Not sure what to do here.
}


SUMOTime
MSPModel_Remote::execute(SUMOTime time) {
    // Perform the JuPedSim iterations.
    int nbrIterations = (int)(DELTA_T / JPS_DELTA_T);
	for (int i = 0; i < nbrIterations; ++i)
	{
		JPS_ErrorMessage mess = new JPS_ErrorMessage_t{};
		bool ok = JPS_Simulation_Iterate(mySimulation, &mess);
        if (!ok) {
            std::cout << "Error during iteration " << i << ": " << JPS_ErrorMessage_GetMessage(mess) << std::endl;
        }
        JPS_ErrorMessage_Free(mess);

#ifdef DEBUG
        if (myNumActivePedestrians == 1) {
            for (PState* state : myPedestrianStates)
            {
                JPS_Agent agent = JPS_Simulation_ReadAgent(mySimulation, state->getAgentId(), nullptr);
                double newPositionX = JPS_Agent_PositionX(agent);
                double newPositionY = JPS_Agent_PositionY(agent);
                myTrajectoryDumpFile << newPositionX << " " << newPositionY << std::endl;
            }
        }
#endif
	}

    // Update the state of all pedestrians.
    for (PState* state : myPedestrianStates)
    {
        // Updates the agent position.
        JPS_Agent agent = JPS_Simulation_ReadAgent(mySimulation, state->getAgentId(), nullptr);
        double newPositionX = JPS_Agent_PositionX(agent);
        double newPositionY = JPS_Agent_PositionY(agent);
        state->setPosition(newPositionX, newPositionY);

        // Updates the agent direction.
        double newOrientationX = JPS_Agent_OrientationX(agent);
        double newOrientationY = JPS_Agent_OrientationY(agent);
        state->setAngle(atan2(newOrientationY, newOrientationX));

        Position newPosition(newPositionX, newPositionY);
        MSPerson* person = state->getPerson();
        MSPerson::MSPersonStage_Walking* stage = dynamic_cast<MSPerson::MSPersonStage_Walking*>(person->getCurrentStage());
        
        // Updates the edge to walk on.
        const MSEdge* currentEdge = stage->getEdge();
        const MSLane* currentLane = getSidewalk<MSEdge, MSLane>(currentEdge);
        if (currentEdge->isWalkingArea()) {
            std::vector<MSLink*> links = currentLane->getLinkCont();
            MSLane* nextLane = nullptr;
            for (MSLink* link : links) {
                MSLane* lane = link->getViaLaneOrLane();
                if (lane->getPermissions() == SVC_PEDESTRIAN) {
                    nextLane = lane;
                    break;
                }
            }
            
            PositionVector shape = nextLane->getShape();
            Position nextLaneDirection = shape[1] - shape[0];
            Position pedestrianLookAhead = newPosition - shape[0];
            if (pedestrianLookAhead.dotProduct(nextLaneDirection) > 0.0) {
                stage->moveToNextEdge(person, time, 1, nullptr);
            }
        }
        else {
            Position relativePosition = (currentLane->getShape()).transformToVectorCoordinates(newPosition);
            if (relativePosition == Position::INVALID) {
                std::vector<MSLink*> links = currentLane->getLinkCont();
                MSLane* nextInternalLane = nullptr;
                for (MSLink* link : links) {
                    MSLane* viaLane = link->getViaLaneOrLane();
                    if (viaLane->getPermissions() == SVC_PEDESTRIAN) {
                        nextInternalLane = viaLane;
                        break;
                    }
                }
                stage->moveToNextEdge(person, time, 1, nextInternalLane ? &(nextInternalLane->getEdge()) : nullptr);
            }
        }
        
        // If near the last waypoint, remove the agent.
        if (newPosition.distanceTo2D(state->getDestination()) < JPS_EXIT_TOLERANCE) {
            JPS_Simulation_RemoveAgent(mySimulation, state->getAgentId(), nullptr);
            myPedestrianStates.erase(std::find(myPedestrianStates.begin(), myPedestrianStates.end(), state));
            stage->moveToNextEdge(person, time, 1, nullptr);
            registerArrived();
        }
    }

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
    for (PState* state : myPedestrianStates) {
        delete state;
        state = nullptr;
    }
    myPedestrianStates.clear();
}

void
MSPModel_Remote::initialize() {
	myGeometryBuilder = JPS_GeometryBuilder_Create();
    myAreasBuilder = JPS_AreasBuilder_Create();

#ifdef DEBUG
    std::ofstream geometryDumpFile;
    geometryDumpFile.open("geometry.txt");
#endif

	for (const MSEdge* const edge : (myNet->getEdgeControl()).getEdges()) {
        const MSLane* lane = getSidewalk<MSEdge, MSLane>(edge);
        if (lane) {
            std::vector<double> lanePolygonCoordinates;
            
            if (edge->isWalkingArea()) {
                PositionVector shape = lane->getShape();
                shape = shape.reverse(); // Apparently CGAL expects polygons to be oriented CCW.
                for (const Position& position : shape) {
                    lanePolygonCoordinates.push_back(position.x());
                    lanePolygonCoordinates.push_back(position.y());
                }
            }
            else {
                double amount = lane->getWidth() / 2.0;

                PositionVector shape = lane->getShape();
                shape.move2side(amount);
                Position bottomFirstCorner = shape[0];
                Position bottomSecondCorner = shape[1];
                shape = lane->getShape();
                shape.move2side(-amount);
                Position topFirstCorner = shape[0];
                Position topSecondCorner = shape[1];

                std::vector<Position> lanePolygon{ topFirstCorner, bottomFirstCorner, bottomSecondCorner, topSecondCorner };
                for (const Position& position : lanePolygon) {
                    lanePolygonCoordinates.push_back(position.x());
                    lanePolygonCoordinates.push_back(position.y());
                }
            }
            
#ifdef DEBUG
            geometryDumpFile << "Lane " <<  lane->getID() << std::endl;
            for (double coordinate: lanePolygonCoordinates) {
                geometryDumpFile << coordinate << std::endl;
            }
#endif

            JPS_GeometryBuilder_AddAccessibleArea(myGeometryBuilder, lanePolygonCoordinates.data(), lanePolygonCoordinates.size() / 2);
        }
	}

#ifdef DEBUG
    geometryDumpFile.close();
#endif

    // The line below doesn't work on Windows at least.
    // JPS_ErrorMessage message{}; 
    JPS_ErrorMessage message = new JPS_ErrorMessage_t{};
    myGeometry = JPS_GeometryBuilder_Build(myGeometryBuilder, &message);
    if (myGeometry == nullptr) {
        std::cout << "Error while creating the geometry: " << JPS_ErrorMessage_GetMessage(message) << std::endl;
    }
    JPS_ErrorMessage_Free(message);

    // Areas are built (although unused) because the JPS_Simulation object needs them.
	myAreas = JPS_AreasBuilder_Build(myAreasBuilder, nullptr);

    message = new JPS_ErrorMessage_t{};
	myModel = JPS_OperationalModel_Create_VelocityModel(8, 0.1, 5, 0.02, nullptr);
    if (myModel == nullptr) {
        std::cout << "Error while creating the pedestrian model: " << JPS_ErrorMessage_GetMessage(message) << std::endl;
    }
    JPS_ErrorMessage_Free(message);

    message = new JPS_ErrorMessage_t{};
	mySimulation = JPS_Simulation_Create(myModel, myGeometry, myAreas, STEPS2TIME(JPS_DELTA_T), nullptr);
    if (mySimulation == nullptr) {
        std::cout << "Error while creating the simulation: " << JPS_ErrorMessage_GetMessage(message) << std::endl;
    }
    JPS_ErrorMessage_Free(message);

#ifdef DEBUG
    myTrajectoryDumpFile.open("trajectory.txt");
#endif
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