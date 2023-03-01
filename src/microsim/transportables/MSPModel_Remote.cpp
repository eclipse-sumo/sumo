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
#include <geos.h>
#include <jupedsim/jupedsim.h>
#include "microsim/MSEdge.h"
#include "microsim/MSLane.h"
#include "microsim/MSLink.h"
#include "microsim/MSEdgeControl.h"
#include "microsim/MSEventControl.h"
#include "libsumo/Person.h"
#include "utils/geom/Position.h"
#include "utils/geom/PositionVector.h"
#include "MSPModel_Remote.h"
#include "MSPerson.h"


const SUMOTime MSPModel_Remote::JPS_DELTA_T = 10;
const double MSPModel_Remote::JPS_EXIT_TOLERANCE = 1;


MSPModel_Remote::MSPModel_Remote(const OptionsCont& oc, MSNet* net) : myNet(net) {
    initialize();
    Event* e = new Event(this);
    net->getBeginOfTimestepEvents()->addEvent(e, net->getCurrentTimeStep() + DELTA_T);
}


MSPModel_Remote::~MSPModel_Remote() {
    clearState();
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
    double halfDepartureLaneWidth = departureLane->getWidth() / 2.0;
    double departureRelativePositionX = stage->getDepartPos();
    double departureRelativePositionY = stage->getDepartPosLat();
    if (departureRelativePositionY == UNSPECIFIED_POS_LAT) {
        departureRelativePositionY = 0.0;
    }
    if (departureRelativePositionY == MSPModel::RANDOM_POS_LAT) {
        departureRelativePositionY = RandHelper::rand(-halfDepartureLaneWidth, halfDepartureLaneWidth);
    }
    Position departurePosition = departureLane->getShape().positionAtOffset(departureRelativePositionX, -departureRelativePositionY); // Minus sign is here for legacy reasons.
    
    const MSLane* arrivalLane = getSidewalk<MSEdge, MSLane>(stage->getRoute().back());
    Position arrivalPosition = arrivalLane->getShape().positionAtOffset(stage->getArrivalPos());

	JPS_Waypoint waypoints[] = { {{arrivalPosition.x(), arrivalPosition.y()}, JPS_EXIT_TOLERANCE} };
	JPS_Journey journey = JPS_Journey_Create_SimpleJourney(waypoints, sizeof(waypoints));
    JPS_JourneyId journeyId = JPS_Simulation_AddJourney(mySimulation, journey, nullptr);

	JPS_AgentParameters agent_parameters{};
	agent_parameters.journeyId = journeyId;
	agent_parameters.orientationX = 1.0;
	agent_parameters.orientationY = 0.0;
	agent_parameters.positionX = departurePosition.x();
	agent_parameters.positionY = departurePosition.y();
    agent_parameters.profileId = myParameterProfileId;

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
    int nbrIterations = (int)(DELTA_T / JPS_DELTA_T);
    JPS_ErrorMessage message = nullptr;
	for (int i = 0; i < nbrIterations; ++i)
	{
        // Perform one JuPedSim iteration.
		bool ok = JPS_Simulation_Iterate(mySimulation, &message);
        if (!ok) {
            std::ostringstream oss;
            oss << "Error during iteration " << i << ": " << JPS_ErrorMessage_GetMessage(message);
            WRITE_ERROR(oss.str());
        }

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
            const MSEdge* currentEdge = stage->getEdge();
            const MSLane* currentLane = getSidewalk<MSEdge, MSLane>(currentEdge);

            // Updates the edge to walk on.
            if (myRoutingMode == PedestrianRoutingMode::SUMO_ROUTING)
            {
                if (currentEdge->isWalkingArea()) { 
                    MSLane* nextLane = getNextPedestrianLane(currentLane);
                    PositionVector shape = nextLane->getShape();
                    Position nextLaneDirection = shape[1] - shape[0];
                    Position pedestrianLookAhead = newPosition - shape[0];
                    if (pedestrianLookAhead.dotProduct(nextLaneDirection) > 0.0) {
                        MSEdge& nextEdge = nextLane->getEdge();
                        if (nextEdge.isCrossing()) {
                            stage->moveToNextEdge(person, time, 1, &nextEdge);
                        }
                        else {
                            stage->moveToNextEdge(person, time, 1, nullptr);
                        }
                    }
                }
                else {
                    Position relativePosition = (currentLane->getShape()).transformToVectorCoordinates(newPosition);
                    if (relativePosition == Position::INVALID) {
                        MSLane* nextLane = getNextPedestrianLane(currentLane);
                        stage->moveToNextEdge(person, time, 1, nextLane ? &(nextLane->getEdge()) : nullptr);
                    }
                }
            }
            else { // PedestrianRoutingMode::JUPEDSIM_ROUTING
                libsumo::Person::moveToXY(person->getID(), currentEdge->getID(), newPositionX, newPositionY, libsumo::INVALID_DOUBLE_VALUE, 2);
            }
            
            // If near the last waypoint, remove the agent.
            if (newPosition.distanceTo2D(state->getDestination()) < JPS_EXIT_TOLERANCE) {
                JPS_Simulation_RemoveAgent(mySimulation, state->getAgentId(), nullptr);
                myPedestrianStates.erase(std::find(myPedestrianStates.begin(), myPedestrianStates.end(), state));
                stage->moveToNextEdge(person, time, 1, nullptr);
                registerArrived();
            }
        }
    }
    JPS_ErrorMessage_Free(message);

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
    myPedestrianStates.clear();
    myNumActivePedestrians = 0;
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
            PositionVector shape = lane->getShape();

            // Apparently CGAL expects polygons to be oriented CCW.
            if (shape.isClockwiseOriented()) {
                shape = shape.reverse();
            }
            assert(!shape.isClockwiseOriented());

            /* The code below is in theory more robust as there would be a guarantee that
               the shape is CCW-oriented. However at the moment the sort algorithm doesn't
               work for non-convex polygons.
               PositionVector shape = lane->getShape();
               shape.sortAsPolyCWByAngle();
               shape = shape.reverse();
               assert(!shape.isClockwiseOriented());
            */
            
            std::vector<double> lanePolygonCoordinatesFlattened;

            if (edge->isWalkingArea()) {
                if (shape.area() == 0.0) {
                    continue;
                }

                auto last = shape.isClosed() ? shape.end()-1 : shape.end();
                for (auto position = shape.begin(); position != last; position++) {
                    lanePolygonCoordinatesFlattened.push_back(position->x());
                    lanePolygonCoordinatesFlattened.push_back(position->y());
                } 

                /*std::vector<std::pair<double, double>> lanePolygonCoordinates;
                for (const Position& position : shape)
                    lanePolygonCoordinates.push_back(std::make_pair<double, double>(position.x(), position.y()));

                auto end = lanePolygonCoordinates.end();
                for (auto it = lanePolygonCoordinates.begin(); it != end; ++it) {
                    end = std::remove(it + 1, end, *it);
                }

                lanePolygonCoordinates.erase(end, lanePolygonCoordinates.end());

                for (auto position = lanePolygonCoordinates.begin(); position != lanePolygonCoordinates.end(); position++) {
                    lanePolygonCoordinatesFlattened.push_back(position->first);
                    lanePolygonCoordinatesFlattened.push_back(position->second);
                }*/
            }
            else {
                double amount = lane->getWidth() / 2.0;
                shape.move2side(amount);
                Position bottomFirstCorner = shape[0];
                Position bottomSecondCorner = shape[1];
                shape = lane->getShape();
                shape.move2side(-amount);
                Position topFirstCorner = shape[0];
                Position topSecondCorner = shape[1];

                std::vector<Position> lanePolygon{ topFirstCorner, bottomFirstCorner, bottomSecondCorner, topSecondCorner };
                for (const Position& position : lanePolygon) {
                    lanePolygonCoordinatesFlattened.push_back(position.x());
                    lanePolygonCoordinatesFlattened.push_back(position.y());
                }
            }
            
#ifdef DEBUG
            geometryDumpFile << "Lane " <<  lane->getID() << std::endl;
            for (double coordinate: lanePolygonCoordinatesFlattened) {
                geometryDumpFile << coordinate << std::endl;
            }
#endif

            JPS_GeometryBuilder_AddAccessibleArea(myGeometryBuilder, lanePolygonCoordinatesFlattened.data(), lanePolygonCoordinatesFlattened.size() / 2);
        }
	}

#ifdef DEBUG
    geometryDumpFile.close();
#endif

    JPS_ErrorMessage message = nullptr; 

    myGeometry = JPS_GeometryBuilder_Build(myGeometryBuilder, &message);
    if (myGeometry == nullptr) {
        std::ostringstream oss;
        oss << "Error while creating the geometry: " << JPS_ErrorMessage_GetMessage(message);
        WRITE_ERROR(oss.str());
    }

    // Areas are built (although unused) because the JPS_Simulation object needs them.
	myAreas = JPS_AreasBuilder_Build(myAreasBuilder, nullptr);

    JPS_VelocityModelBuilder modelBuilder = JPS_VelocityModelBuilder_Create(8.0, 0.1, 5.0, 0.02);
    myParameterProfileId = 1;
    double initial_speed = 1.0; // stage->getMaxSpeed(person);
    double pedestrian_radius = 0.5; // 1.0 brings bad pedestrian behavior...
    JPS_VelocityModelBuilder_AddParameterProfile(modelBuilder, myParameterProfileId, 1.0, 0.5, initial_speed, pedestrian_radius);
    myModel = JPS_VelocityModelBuilder_Build(modelBuilder, &message);
    if (myModel == nullptr) {
        std::ostringstream oss;
        oss << "Error while creating the pedestrian model: " << JPS_ErrorMessage_GetMessage(message);
        WRITE_ERROR(oss.str());
    }

	mySimulation = JPS_Simulation_Create(myModel, myGeometry, myAreas, STEPS2TIME(JPS_DELTA_T), &message);
    if (mySimulation == nullptr) {
        std::ostringstream oss;
        oss << "Error while creating the simulation: " << JPS_ErrorMessage_GetMessage(message);
        WRITE_ERROR(oss.str());
    }

    JPS_ErrorMessage_Free(message);

#ifdef DEBUG
    myTrajectoryDumpFile.open("trajectory.txt");
#endif
}


MSLane* MSPModel_Remote::getNextPedestrianLane(const MSLane* const currentLane) const {
    std::vector<MSLink*> links = currentLane->getLinkCont();
    MSLane* nextLane = nullptr;
    for (MSLink* link : links) {
        MSLane* lane = link->getViaLaneOrLane();
        if (lane->getPermissions() == SVC_PEDESTRIAN) {
            nextLane = lane;
            break;
        }
    }
    return nextLane;
}


// ===========================================================================
// MSPModel_Remote::PState method definitions
// ===========================================================================
MSPModel_Remote::PState::PState(MSPerson* person, MSStageMoving* stage, JPS_Journey journey, Position destination, JPS_AgentId agentId)
    : myPerson(person), myAngle(0), myPosition(0, 0), myStage(stage), myJourney(journey), myDestination(destination), myAgentId(agentId) {
    ConstMSEdgeVector route = stage->getRoute();
    myCustomRoute.push_back(route.front());
    myCustomRoute.push_back(route.back());
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


void MSPModel_Remote::PState::addEdgeToRoute(MSEdge* edge) {
    myCustomRoute.insert(myCustomRoute.end()-1, edge);
}


const ConstMSEdgeVector& MSPModel_Remote::PState::getRoute(void) const {
    return myCustomRoute;
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


void
MSPModel_Remote::PState::moveToXY(MSPerson* p, Position pos, MSLane* lane, double lanePos, double lanePosLat, double angle, int routeOffset, const ConstMSEdgeVector& edges, SUMOTime t) {
    if (edges.empty()) {
        myStage->setRouteIndex(myPerson, routeOffset);
    }
    else {
        myStage->replaceRoute(myPerson, edges, routeOffset);
    }

    /*
    if (currentEdge->isWalkingArea()) {
        MSLane* nextLane = getNextPedestrianLane(currentLane);
        PositionVector shape = nextLane->getShape();
        Position nextLaneDirection = shape[1] - shape[0];
        Position pedestrianLookAhead = newPosition - shape[0];
        if (pedestrianLookAhead.dotProduct(nextLaneDirection) > 0.0) {
            MSEdge& nextEdge = nextLane->getEdge();
            if (nextEdge.isCrossing()) {
                stage->moveToNextEdge(person, time, 1, &nextEdge);
            }
            else {
                stage->moveToNextEdge(person, time, 1, nullptr);
            }
        }
    }
    else {
        Position relativePosition = (currentLane->getShape()).transformToVectorCoordinates(newPosition);
        if (relativePosition == Position::INVALID) {
            MSLane* nextLane = getNextPedestrianLane(currentLane);
            stage->moveToNextEdge(person, time, 1, nextLane ? &(nextLane->getEdge()) : nullptr);
        }
    }*/
}


Position MSPModel_Remote::PState::getDestination(void) const {
    return myDestination;
}


JPS_AgentId MSPModel_Remote::PState::getAgentId(void) const {
    return myAgentId;
}