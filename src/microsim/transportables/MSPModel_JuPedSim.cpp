/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2014-2024 German Aerospace Center (DLR) and others.
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
/// @file    MSPModel_JuPedSim.cpp
/// @author  Gregor Laemmel
/// @author  Benjamin Coueraud
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Mon, 13 Jan 2014
///
// The pedestrian following model that can instantiate different pedestrian models
// that come with the JuPedSim third-party simulation framework.
/****************************************************************************/

#include <config.h>

#include <algorithm>
#include <fstream>
#include <geos_c.h>
#include <jupedsim/jupedsim.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSLink.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSJunctionControl.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSStoppingPlace.h>
#include <microsim/MSTrainHelper.h>
#include <libsumo/Helper.h>
#include <utils/geom/Position.h>
#include <utils/geom/PositionVector.h>
#include <utils/options/OptionsCont.h>
#include <utils/shapes/ShapeContainer.h>
#include "MSPerson.h"
#include "MSStageWalking.h"
#include "MSPModel_JuPedSim.h"


// #define DEBUG_GEOMETRY_GENERATION


const int MSPModel_JuPedSim::GEOS_QUADRANT_SEGMENTS = 16;
const double MSPModel_JuPedSim::GEOS_MITRE_LIMIT = 5.0;
const double MSPModel_JuPedSim::GEOS_MIN_AREA = 0.01;
const double MSPModel_JuPedSim::GEOS_BUFFERED_SEGMENT_WIDTH = 0.5 * SUMO_const_laneWidth;
const double MSPModel_JuPedSim::CARRIAGE_RAMP_LENGTH = 2.0;
const RGBColor MSPModel_JuPedSim::PEDESTRIAN_NETWORK_COLOR = RGBColor(179, 217, 255, 255);
const RGBColor MSPModel_JuPedSim::PEDESTRIAN_NETWORK_CARRIAGES_AND_RAMPS_COLOR = RGBColor(255, 217, 179, 255);
const std::string MSPModel_JuPedSim::PEDESTRIAN_NETWORK_ID = "jupedsim.pedestrian_network";
const std::string MSPModel_JuPedSim::PEDESTRIAN_NETWORK_CARRIAGES_AND_RAMPS_ID = "jupedsim.pedestrian_network.carriages_and_ramps";

// ===========================================================================
// method definitions
// ===========================================================================
MSPModel_JuPedSim::MSPModel_JuPedSim(const OptionsCont& oc, MSNet* net) :
    myNetwork(net), myShapeContainer(net->getShapeContainer()), myJPSDeltaT(string2time(oc.getString("pedestrian.jupedsim.step-length"))),
    myExitTolerance(oc.getFloat("pedestrian.jupedsim.exit-tolerance")), myGEOSPedestrianNetworkLargestComponent(nullptr),
    myHaveAdditionalWalkableAreas(false) {
    initialize(oc);
    net->getBeginOfTimestepEvents()->addEvent(new Event(this), net->getCurrentTimeStep() + DELTA_T);
}


MSPModel_JuPedSim::~MSPModel_JuPedSim() {
    clearState();

    JPS_Simulation_Free(myJPSSimulation);
    JPS_OperationalModel_Free(myJPSModel);
    JPS_Geometry_Free(myJPSGeometry);
    if (myJPSGeometryWithTrainsAndRamps != nullptr) {
        JPS_Geometry_Free(myJPSGeometryWithTrainsAndRamps);
    }

    GEOSGeom_destroy(myGEOSPedestrianNetwork);
    finishGEOS();
}


void
MSPModel_JuPedSim::tryPedestrianInsertion(PState* state, const Position& p) {
    JPS_CollisionFreeSpeedModelAgentParameters agent_parameters{};
    agent_parameters.journeyId = state->getJourneyId();
    agent_parameters.stageId = state->getStageId();
    agent_parameters.position = {p.x(), p.y()};
    /*
    const double angle = state->getAngle(*state->getStage(), 0);
    JPS_Point orientation;
    if (fabs(angle - M_PI / 2) < NUMERICAL_EPS) {
        orientation = JPS_Point{0., 1.};
    }
    else if (fabs(angle + M_PI / 2) < NUMERICAL_EPS) {
        orientation = JPS_Point{0., -1.};
    }
    else {
        orientation = JPS_Point{1., tan(angle)};
    }
    agent_parameters.orientation = orientation;
    */
    const MSVehicleType& type = state->getPerson()->getVehicleType();
    agent_parameters.radius = MAX2(type.getLength(), type.getWidth()) / 2.;
    agent_parameters.v0 = state->getPerson()->getMaxSpeed();
    JPS_ErrorMessage message = nullptr;
    JPS_AgentId agentId = JPS_Simulation_AddCollisionFreeSpeedModelAgent(myJPSSimulation, agent_parameters, &message);
    if (message != nullptr) {
        WRITE_WARNINGF(TL("Error while adding person '%' as JuPedSim agent: %"), state->getPerson()->getID(), JPS_ErrorMessage_GetMessage(message));
        JPS_ErrorMessage_Free(message);
    } else {
        state->setAgentId(agentId);
    }
}


bool
MSPModel_JuPedSim::addWaypoint(JPS_JourneyDescription journey, JPS_StageId& predecessor, const Position& point, const std::string& agentID, const double radius) {
    JPS_ErrorMessage message = nullptr;
    const JPS_StageId waypointId = JPS_Simulation_AddStageWaypoint(myJPSSimulation, {point.x(), point.y()}, radius, &message);
    if (message != nullptr) {
        WRITE_WARNINGF(TL("Error while adding waypoint for person '%': %"), agentID, JPS_ErrorMessage_GetMessage(message));
        JPS_ErrorMessage_Free(message);
        return false;
    }
    if (predecessor != 0) {
        const JPS_Transition transition = JPS_Transition_CreateFixedTransition(waypointId, &message);
        if (message != nullptr) {
            WRITE_WARNINGF(TL("Error while creating fixed transition for person '%': %"), agentID, JPS_ErrorMessage_GetMessage(message));
            JPS_ErrorMessage_Free(message);
            return false;
        }
        JPS_JourneyDescription_SetTransitionForStage(journey, predecessor, transition, &message);
        if (message != nullptr) {
            WRITE_WARNINGF(TL("Error while setting transition for person '%': %"), agentID, JPS_ErrorMessage_GetMessage(message));
            JPS_ErrorMessage_Free(message);
            return false;
        }
        JPS_Transition_Free(transition);
    }
    JPS_JourneyDescription_AddStage(journey, waypointId);
    predecessor = waypointId;
    return true;
}


MSTransportableStateAdapter*
MSPModel_JuPedSim::add(MSTransportable* person, MSStageMoving* stage, SUMOTime now) {
    assert(person->getCurrentStageType() == MSStageType::WALKING);
    Position departurePosition = Position::INVALID;
    const MSLane* const departureLane = getSidewalk<MSEdge, MSLane>(stage->getRoute().front());
    if (departureLane == nullptr) {
        const char* error = TL("Person '%' could not find sidewalk on edge '%', time=%.");
        if (OptionsCont::getOptions().getBool("ignore-route-errors")) {
            WRITE_WARNINGF(error, person->getID(), person->getEdge()->getID(), time2string(SIMSTEP));
            return nullptr;
        } else {
            throw ProcessError(TLF(error, person->getID(), person->getEdge()->getID(), time2string(SIMSTEP)));
        }
    }
    // First real stage, stage 0 is waiting.
    if (person->getCurrentStageIndex() == 2 && person->getParameter().departPosProcedure == DepartPosDefinition::RANDOM_LOCATION) {
        const MSEdge* const tripOrigin = person->getNextStage(-1)->getEdge();
        if (tripOrigin->isTazConnector()) {
            const SUMOPolygon* tazShape = myNetwork->getShapeContainer().getPolygons().get(tripOrigin->getParameter("taz"));
            if (tazShape == nullptr) {
                WRITE_WARNINGF(TL("FromTaz '%' for person '%' has no shape information."), tripOrigin->getParameter("taz"), person->getID());
            } else {
                const Boundary& bbox = tazShape->getShape().getBoxBoundary();
                while (!tazShape->getShape().around(departurePosition)) {
                    // TODO: Optimize for speed if necessary or at least abort trying to find a point.
                    departurePosition.setx(RandHelper::rand(bbox.xmin(), bbox.xmax()));
                    departurePosition.sety(RandHelper::rand(bbox.ymin(), bbox.ymax()));
                }
            }
        }
    }
    if (departurePosition == Position::INVALID) {
        const double halfDepartureLaneWidth = departureLane->getWidth() / 2.0;
        double departureRelativePositionY = stage->getDepartPosLat();
        if (departureRelativePositionY == UNSPECIFIED_POS_LAT) {
            departureRelativePositionY = 0.0;
        }
        if (departureRelativePositionY == MSPModel::RANDOM_POS_LAT) {
            departureRelativePositionY = RandHelper::rand(-halfDepartureLaneWidth, halfDepartureLaneWidth);
        }
        departurePosition = departureLane->getShape().positionAtOffset(stage->getDepartPos(), -departureRelativePositionY); // Minus sign is here for legacy reasons.
    }

    std::vector<std::pair<Position, double> >waypoints;
    for (const MSEdge* const e : stage->getEdges()) {
        const MSLane* const lane = getSidewalk<MSEdge, MSLane>(e);
        waypoints.push_back(std::make_pair(lane->getShape().positionAtOffset(e->getLength() / 2.), lane->getWidth() / 2.));
    }
    waypoints.pop_back();  // arrival waypoint comes later
    if (!waypoints.empty()) {
        waypoints.erase(waypoints.begin());  // departure edge also does not need a waypoint
    }
    const MSLane* const arrivalLane = getSidewalk<MSEdge, MSLane>(stage->getDestination());
    const Position arrivalPosition = arrivalLane->getShape().positionAtOffset(stage->getArrivalPos());
    waypoints.push_back(std::make_pair(arrivalPosition, stage->getDouble("jupedsim.waypoint.radius", myExitTolerance)));

    JPS_JourneyDescription journeyDesc = JPS_JourneyDescription_Create();
    JPS_StageId startingStage = 0;
    JPS_StageId predecessor = 0;
    for (const std::pair<Position, double>& p : waypoints) {
        if (!addWaypoint(journeyDesc, predecessor, p.first, person->getID(), p.second)) {
            JPS_JourneyDescription_Free(journeyDesc);
            return nullptr;
        }
        if (startingStage == 0) {
            startingStage = predecessor;
        }
    }
    JPS_ErrorMessage message = nullptr;
    JPS_JourneyId journeyId = JPS_Simulation_AddJourney(myJPSSimulation, journeyDesc, &message);
    JPS_JourneyDescription_Free(journeyDesc);
    if (message != nullptr) {
        WRITE_WARNINGF(TL("Error while adding a journey for person '%': %"), person->getID(), JPS_ErrorMessage_GetMessage(message));
        JPS_ErrorMessage_Free(message);
        return nullptr;
    }

    PState* state = nullptr;
    for (PState* const pstate : myPedestrianStates) {  // TODO transform myPedestrianStates into a map for faster lookup
        if (pstate->getPerson() == person) {
            state = pstate;
            break;
        }
    }
    if (state == nullptr) {
        state = new PState(static_cast<MSPerson*>(person), stage, journeyId, startingStage, waypoints);
        state->setLanePosition(stage->getDepartPos());
        state->setPreviousPosition(departurePosition);
        state->setPosition(departurePosition.x(), departurePosition.y());
        state->setAngle(departureLane->getShape().rotationAtOffset(stage->getDepartPos()));
        myPedestrianStates.push_back(state);
        myNumActivePedestrians++;
    } else {
        state->reinit(stage, journeyId, startingStage, waypoints);
    }
    if (state->isWaitingToEnter()) {
        tryPedestrianInsertion(state, state->getPosition(*state->getStage(), now));
    } else {
        JPS_Simulation_SwitchAgentJourney(myJPSSimulation, state->getAgentId(), journeyId, startingStage, &message);
        if (message != nullptr) {
            WRITE_WARNINGF(TL("Error while switching to a new journey for person '%': %"), person->getID(), JPS_ErrorMessage_GetMessage(message));
            JPS_ErrorMessage_Free(message);
            return nullptr;
        }
    }
    return state;
}


void
MSPModel_JuPedSim::remove(MSTransportableStateAdapter* state) {
    PState* pstate = static_cast<PState*>(state);
    if (pstate->getStage() != nullptr) {
        pstate->getStage()->setPState(nullptr);  // we need to remove the old state reference to avoid double deletion
    }
    pstate->setStage(nullptr);
}


SUMOTime
MSPModel_JuPedSim::execute(SUMOTime time) {
    const int nbrIterations = (int)(DELTA_T / myJPSDeltaT);
    JPS_ErrorMessage message = nullptr;
    for (int i = 0; i < nbrIterations; ++i) {
        // Perform one JuPedSim iteration.
        bool ok = JPS_Simulation_Iterate(myJPSSimulation, &message);
        if (!ok) {
            WRITE_ERRORF(TL("Error during iteration %: %"), i, JPS_ErrorMessage_GetMessage(message));
        }
    }

    // Update the state of all pedestrians.
    // If necessary, this could be done more often in the loop above but the more precise positions are probably never visible.
    // If it is needed for model correctness (precise stopping / arrivals) we should rather reduce SUMO's step-length.
    for (auto stateIt = myPedestrianStates.begin(); stateIt != myPedestrianStates.end();) {
        PState* const state = *stateIt;

        if (state->isWaitingToEnter()) {
            // insertion failed at first try so we retry with some noise
            Position p = state->getPosition(*state->getStage(), time);
            p.setx(p.x() + RandHelper::rand(-.5, .5));  // we do this separately to avoid evaluation order problems
            p.sety(p.y() + RandHelper::rand(-.5, .5));
            tryPedestrianInsertion(state, p);
            ++stateIt;
            continue;
        }

        MSPerson* person = state->getPerson();
        MSStageWalking* stage = dynamic_cast<MSStageWalking*>(person->getCurrentStage());
        if (stage == nullptr) {
            // It seems we kept the state for another stage but the new stage is not a walk.
            // So let's remove the state because after the new stage we will be elsewhere and need to be reinserted for JuPedSim anyway.
            // We cannot check this earlier because when the old stage ends the next stage might not know yet whether it will be a walk.
            registerArrived();
            JPS_Simulation_MarkAgentForRemoval(myJPSSimulation, state->getAgentId(), nullptr);
            stateIt = myPedestrianStates.erase(stateIt);
            continue;
        }

        // Updates the agent position.
        const JPS_Agent agent = JPS_Simulation_GetAgent(myJPSSimulation, state->getAgentId(), nullptr);
        state->setPreviousPosition(state->getPosition(*stage, DELTA_T));
        const JPS_Point position = JPS_Agent_GetPosition(agent);
        state->setPosition(position.x, position.y);

        // Updates the agent direction.
        const JPS_Point orientation = JPS_Agent_GetOrientation(agent);
        state->setAngle(atan2(orientation.y, orientation.x));

        // Find on which edge the pedestrian is, using route's forward-looking edges because of how moveToXY is written.
        Position newPosition(position.x, position.y);
        ConstMSEdgeVector route = stage->getEdges();
        const int routeIndex = (int)(stage->getRouteStep() - stage->getRoute().begin());
        ConstMSEdgeVector forwardRoute = ConstMSEdgeVector(route.begin() + routeIndex, route.end());
        double bestDistance = std::numeric_limits<double>::max();
        MSLane* candidateLane = nullptr;
        double candidateLaneLongitudinalPosition = 0.0;
        int routeOffset = 0;
        const bool found = libsumo::Helper::moveToXYMap_matchingRoutePosition(newPosition, "",
                           forwardRoute, 0, person->getVClass(), true, bestDistance, &candidateLane, candidateLaneLongitudinalPosition, routeOffset);

        if (found) {
            state->setLanePosition(candidateLaneLongitudinalPosition);
        }

        const MSEdge* const expectedEdge = stage->getEdge();
        if (found && expectedEdge->isNormal() && candidateLane->getEdge().isNormal() && &candidateLane->getEdge() != expectedEdge) {
            const bool result = stage->moveToNextEdge(person, time, 1, nullptr);
            UNUSED_PARAMETER(result);
            assert(result == false); // The person has not arrived yet.
            stage->activateEntryReminders(person);
            // Adapt speed to lane's speed limit.
            JPS_CollisionFreeSpeedModelState modelState = JPS_Agent_GetCollisionFreeSpeedModelState(agent, nullptr);
            const double newMaxSpeed = MIN2(candidateLane->getSpeedLimit(), person->getMaxSpeed());
            if (newMaxSpeed != JPS_CollisionFreeSpeedModelState_GetV0(modelState)) {
                JPS_CollisionFreeSpeedModelState_SetV0(modelState, newMaxSpeed);
            }
        }

        if (newPosition.distanceTo2D(state->getNextWaypoint().first) < 2 * state->getNextWaypoint().second) {
            // If near the last waypoint, remove the agent.
            if (state->advanceNextWaypoint()) {
                // TODO this only works if the final stage is actually a walk
                const bool finalStage = person->getNumRemainingStages() == 1;
                const JPS_AgentId agentID = state->getAgentId();
                while (!stage->moveToNextEdge(person, time, 1, nullptr));
                if (finalStage) {
                    registerArrived();
                    JPS_Simulation_MarkAgentForRemoval(myJPSSimulation, agentID, nullptr);
                    stateIt = myPedestrianStates.erase(stateIt);
                    continue;
                }
            }
        }
        ++stateIt;
    }

    // Remove pedestrians that are in a predefined area, at a predefined rate.
    for (AreaData& area : myAreas) {
        const std::vector<JPS_Point>& areaBoundary = area.areaBoundary;
        JPS_AgentIdIterator agentsInArea = JPS_Simulation_AgentsInPolygon(myJPSSimulation, areaBoundary.data(), areaBoundary.size());
        if (area.areaType == "vanishing_area") {
            const SUMOTime period = area.params.count("period") > 0 ? string2time(area.params.at("period")) : 1000;
            if (time - area.lastRemovalTime >= period) {
                const JPS_AgentId agentID = JPS_AgentIdIterator_Next(agentsInArea);
                if (agentID != 0) {
                    auto lambda = [agentID](const PState * const p) {
                        return p->getAgentId() == agentID;
                    };
                    std::vector<PState*>::const_iterator iterator = std::find_if(myPedestrianStates.begin(), myPedestrianStates.end(), lambda);
                    if (iterator != myPedestrianStates.end()) {
                        const PState* const state = *iterator;
                        MSPerson* const person = state->getPerson();
                        // Code below only works if the removal happens at the last stage.
                        const bool finalStage = person->getNumRemainingStages() == 1;
                        if (finalStage) {
                            WRITE_MESSAGEF(TL("Person '%' in vanishing area '%' was removed from the simulation."), person->getID(), area.id);
                            while (!state->getStage()->moveToNextEdge(person, time, 1, nullptr));
                            registerArrived();
                            JPS_Simulation_MarkAgentForRemoval(myJPSSimulation, agentID, nullptr);
                            myPedestrianStates.erase(iterator);
                            area.lastRemovalTime = time;
                        }
                    }
                }
            }
        } else {  // areaType == "influencer"
            for (JPS_AgentId agentID = JPS_AgentIdIterator_Next(agentsInArea); agentID != 0; agentID = JPS_AgentIdIterator_Next(agentsInArea)) {
                if (area.params.count("speed") > 0) {
                    const JPS_Agent agent = JPS_Simulation_GetAgent(myJPSSimulation, agentID, nullptr);
                    JPS_CollisionFreeSpeedModelState modelState = JPS_Agent_GetCollisionFreeSpeedModelState(agent, nullptr);
                    const double newMaxSpeed = StringUtils::toDouble(area.params.at("speed"));
                    if (newMaxSpeed != JPS_CollisionFreeSpeedModelState_GetV0(modelState)) {
                        JPS_CollisionFreeSpeedModelState_SetV0(modelState, newMaxSpeed);
                    }
                }
            }
        }
        JPS_AgentIdIterator_Free(agentsInArea);
    }

    // Add dynamically additional geometry from train carriages that are stopped.
    const auto& stoppingPlaces = myNetwork->getStoppingPlaces(SumoXMLTag::SUMO_TAG_BUS_STOP);
    std::vector<SUMOTrafficObject::NumericalID> allStoppedTrainIDs;
    std::vector<const MSVehicle*> allStoppedTrains;
    for (const auto& stop : stoppingPlaces) {
        std::vector<const SUMOVehicle*> stoppedTrains = stop.second->getStoppedVehicles();
        for (const SUMOVehicle* train : stoppedTrains) {
            allStoppedTrainIDs.push_back(train->getNumericalID());
            allStoppedTrains.push_back(dynamic_cast<const MSVehicle*>(train));
        }
    }
    if (allStoppedTrainIDs != myAllStoppedTrainIDs) {
        removePolygonFromDrawing(PEDESTRIAN_NETWORK_CARRIAGES_AND_RAMPS_ID);
        if (!allStoppedTrainIDs.empty()) {
            std::vector<GEOSGeometry*> carriagePolygons;
            std::vector<GEOSGeometry*> rampPolygons;
            for (const MSVehicle* train : allStoppedTrains) {
                if (train->getLeavingPersonNumber() > 0) {
                    MSTrainHelper trainHelper = MSTrainHelper(train);
                    trainHelper.computeDoorPositions();
                    const std::vector<MSTrainHelper::Carriage*>& carriages = trainHelper.getCarriages();
                    for (const MSTrainHelper::Carriage* carriage : carriages) {
                        Position dir = carriage->front - carriage->back;
                        if (dir.length2D() == 0.0) {
                            continue;
                        }
                        dir.norm2D();
                        Position perp = Position(-dir.y(), dir.x());
                        // Create carriages geometry.
                        double p = trainHelper.getHalfWidth();
                        PositionVector carriageShape;
                        carriageShape.push_back(carriage->front + perp * p);
                        carriageShape.push_back(carriage->back + perp * p);
                        carriageShape.push_back(carriage->back - perp * p);
                        carriageShape.push_back(carriage->front - perp * p);
                        carriagePolygons.push_back(createGeometryFromShape(carriageShape, train->getID(), false));
                        // Create ramps geometry.
                        p += CARRIAGE_RAMP_LENGTH;
                        const double d = 0.5 * MSTrainHelper::CARRIAGE_DOOR_WIDTH;
                        for (const Position& door : carriage->doorPositions) {
                            PositionVector rampShape;
                            rampShape.push_back(door - perp * p + dir * d);
                            rampShape.push_back(door - perp * p - dir * d);
                            rampShape.push_back(door + perp * p - dir * d);
                            rampShape.push_back(door + perp * p + dir * d);
                            rampPolygons.push_back(createGeometryFromShape(rampShape, train->getID(), false));
                        }
                    }
                }
            }
            if (!carriagePolygons.empty()) {
                GEOSGeometry* carriagesCollection = GEOSGeom_createCollection(GEOS_MULTIPOLYGON, carriagePolygons.data(), (unsigned int)carriagePolygons.size());
                GEOSGeometry* rampsCollection = GEOSGeom_createCollection(GEOS_MULTIPOLYGON, rampPolygons.data(), (unsigned int)rampPolygons.size());
                GEOSGeometry* carriagesAndRampsUnion = GEOSUnion(carriagesCollection, rampsCollection);
                if (carriagesAndRampsUnion == nullptr) {
                    WRITE_WARNING(TL("Error while generating geometry for carriages."));
                } else {
                    GEOSGeometry* pedestrianNetworkWithTrainsAndRamps = GEOSUnion(carriagesAndRampsUnion, myGEOSPedestrianNetworkLargestComponent);
#ifdef DEBUG_GEOMETRY_GENERATION
                    dumpGeometry(pedestrianNetworkWithTrainsAndRamps, "pedestrianNetworkWithTrainsAndRamps.wkt");
#endif
                    int nbrComponents = 0;
                    double maxArea = 0.0;
                    double totalArea = 0.0;
                    const GEOSGeometry* pedestrianNetworkWithTrainsAndRampsLargestComponent = getLargestComponent(pedestrianNetworkWithTrainsAndRamps, nbrComponents, maxArea, totalArea);
                    if (nbrComponents > 1) {
                        WRITE_WARNINGF(TL("While generating geometry % connected components were detected, %% of total pedestrian area is covered by the largest."),
                                       nbrComponents, maxArea / totalArea * 100.0, "%");
                    }
#ifdef DEBUG_GEOMETRY_GENERATION
                    dumpGeometry(pedestrianNetworkWithTrainsAndRampsLargestComponent, "pedestrianNetworkWithTrainsAndRamps.wkt");
#endif
                    myJPSGeometryWithTrainsAndRamps = buildJPSGeometryFromGEOSGeometry(pedestrianNetworkWithTrainsAndRampsLargestComponent);
                    JPS_Simulation_SwitchGeometry(myJPSSimulation, myJPSGeometryWithTrainsAndRamps, nullptr, nullptr);
                    removePolygonFromDrawing(PEDESTRIAN_NETWORK_ID);
                    preparePolygonForDrawing(pedestrianNetworkWithTrainsAndRampsLargestComponent, PEDESTRIAN_NETWORK_CARRIAGES_AND_RAMPS_ID, PEDESTRIAN_NETWORK_CARRIAGES_AND_RAMPS_COLOR);
                    GEOSGeom_destroy(pedestrianNetworkWithTrainsAndRamps);
                }
                GEOSGeom_destroy(rampsCollection);
                GEOSGeom_destroy(carriagesCollection);
            }
        } else {
            JPS_Simulation_SwitchGeometry(myJPSSimulation, myJPSGeometry, nullptr, nullptr);
            preparePolygonForDrawing(myGEOSPedestrianNetworkLargestComponent, PEDESTRIAN_NETWORK_ID, PEDESTRIAN_NETWORK_COLOR);
        }
        myAllStoppedTrainIDs = allStoppedTrainIDs;
    }

    JPS_ErrorMessage_Free(message);

    return DELTA_T;
}


bool
MSPModel_JuPedSim::usingInternalLanes() {
    return MSGlobals::gUsingInternalLanes && MSNet::getInstance()->hasInternalLinks();
}


void MSPModel_JuPedSim::registerArrived() {
    myNumActivePedestrians--;
}


int MSPModel_JuPedSim::getActiveNumber() {
    return myNumActivePedestrians;
}


void MSPModel_JuPedSim::clearState() {
    myPedestrianStates.clear();
    myNumActivePedestrians = 0;
}


const Position&
MSPModel_JuPedSim::getAnchor(const MSLane* const lane, const MSEdge* const edge, MSEdgeVector incoming) {
    if (std::count(incoming.begin(), incoming.end(), edge)) {
        return lane->getShape().back();
    }

    return lane->getShape().front();
}


const MSEdgeVector
MSPModel_JuPedSim::getAdjacentEdgesOfEdge(const MSEdge* const edge) {
    const MSEdgeVector& outgoing = edge->getSuccessors();
    MSEdgeVector adjacent = edge->getPredecessors();
    adjacent.insert(adjacent.end(), outgoing.begin(), outgoing.end());

    return adjacent;
}


const MSEdge*
MSPModel_JuPedSim::getWalkingAreaInbetween(const MSEdge* const edge, const MSEdge* const otherEdge) {
    for (const MSEdge* nextEdge : getAdjacentEdgesOfEdge(edge)) {
        if (nextEdge->isWalkingArea()) {
            MSEdgeVector walkingAreOutgoing = getAdjacentEdgesOfEdge(nextEdge);
            if (std::count(walkingAreOutgoing.begin(), walkingAreOutgoing.end(), otherEdge)) {
                return nextEdge;
            }
        }
    }

    return nullptr;
}


GEOSGeometry*
MSPModel_JuPedSim::createGeometryFromCenterLine(PositionVector centerLine, double width, int capStyle) {
    GEOSCoordSequence* coordinateSequence = convertToGEOSPoints(centerLine);
    GEOSGeometry* lineString = GEOSGeom_createLineString(coordinateSequence);
    GEOSGeometry* dilatedLineString = GEOSBufferWithStyle(lineString, width, GEOS_QUADRANT_SEGMENTS, capStyle, GEOSBUF_JOIN_ROUND, GEOS_MITRE_LIMIT);
    GEOSGeom_destroy(lineString);
    return dilatedLineString;
}


GEOSGeometry*
MSPModel_JuPedSim::createGeometryFromShape(PositionVector shape, std::string shapeID, bool isInternalShape) {
    // Corner case.
    if (shape.size() == 1) {
        WRITE_WARNINGF(TL("Polygon '%' will be skipped as it is just a point."), shapeID);
        return nullptr;
    }
    // Make sure the shape is closed.
    if (shape.back() != shape.front()) {
        shape.push_back(shape.front());
    }
    // Replace consecutive points that are equal with just one.
    PositionVector cleanShape;
    cleanShape.push_back(shape[0]);
    PositionVector duplicates;
    for (int i = 1; i < (int)shape.size(); i++) {
        if (shape[i] != shape[i - 1]) {
            cleanShape.push_back(shape[i]);
        } else {
            duplicates.push_back(shape[i]);
        }
    }
    if (cleanShape.size() < shape.size()) {
        WRITE_WARNINGF(TL("Polygon '%' had some equal consecutive points removed: %"), shapeID, toString(duplicates, 9));
    }
    GEOSCoordSequence* coordinateSequence = convertToGEOSPoints(cleanShape);
    GEOSGeometry* linearRing = GEOSGeom_createLinearRing(coordinateSequence);
    GEOSGeometry* polygon = GEOSGeom_createPolygon(linearRing, nullptr, 0);
    if (!GEOSisSimple(polygon)) {
        if (cleanShape.size() == 3) {
            if (isInternalShape) {
                WRITE_WARNINGF(TL("Polygon for junction '%' has been dilated as it is just a segment."), shapeID);
            } else {
                WRITE_WARNINGF(TL("Polygon '%' has been dilated as it is just a segment."), shapeID);
            }
            GEOSGeometry* lineString = GEOSGeom_createLineString(GEOSCoordSeq_clone(coordinateSequence));
            GEOSGeom_destroy(polygon);
            polygon = GEOSBufferWithStyle(lineString, GEOS_BUFFERED_SEGMENT_WIDTH, GEOS_QUADRANT_SEGMENTS, GEOSBUF_CAP_ROUND, GEOSBUF_JOIN_ROUND, GEOS_MITRE_LIMIT);
            GEOSGeom_destroy(lineString);
        } else {
            if (isInternalShape) {
                WRITE_WARNINGF(TL("Polygon on junction '%' has been replaced by its convex hull as it is not simple."), shapeID);
                GEOSGeometry* hull = GEOSConvexHull(polygon);
                GEOSGeom_destroy(polygon);
                polygon = hull;
            } else {
                WRITE_WARNINGF(TL("Polygon '%' will be skipped as it is not simple."), shapeID);
                polygon = nullptr;
            }
        }
    }
    return polygon;
}


GEOSGeometry*
MSPModel_JuPedSim::createGeometryFromAnchors(const Position& anchor, const MSLane* const lane, const Position& otherAnchor, const MSLane* const otherLane) {
    GEOSGeometry* geometry;
    if (lane->getWidth() == otherLane->getWidth()) {
        PositionVector anchors = { anchor, otherAnchor };
        geometry = createGeometryFromCenterLine(anchors, lane->getWidth() / 2.0, GEOSBUF_CAP_ROUND);
    } else {
        GEOSGeometry* anchorPoint = GEOSGeom_createPointFromXY(anchor.x(), anchor.y());
        GEOSGeometry* dilatedAnchorPoint = GEOSBufferWithStyle(anchorPoint, lane->getWidth() / 2.0,
                                           GEOS_QUADRANT_SEGMENTS, GEOSBUF_CAP_ROUND, GEOSBUF_JOIN_ROUND, GEOS_MITRE_LIMIT);
        GEOSGeom_destroy(anchorPoint);
        GEOSGeometry* otherAnchorPoint = GEOSGeom_createPointFromXY(otherAnchor.x(), otherAnchor.y());
        GEOSGeometry* dilatedOtherAnchorPoint = GEOSBufferWithStyle(otherAnchorPoint, otherLane->getWidth() / 2.0,
                                                GEOS_QUADRANT_SEGMENTS, GEOSBUF_CAP_ROUND, GEOSBUF_JOIN_ROUND, GEOS_MITRE_LIMIT);
        GEOSGeom_destroy(otherAnchorPoint);
        GEOSGeometry* polygons[2] = { dilatedAnchorPoint, dilatedOtherAnchorPoint };
        GEOSGeometry* multiPolygon = GEOSGeom_createCollection(GEOS_MULTIPOLYGON, polygons, 2);
        geometry = GEOSConvexHull(multiPolygon);
        GEOSGeom_destroy(multiPolygon);
    }

    return geometry;
}


GEOSGeometry*
MSPModel_JuPedSim::buildPedestrianNetwork(MSNet* network) {
    std::vector<GEOSGeometry*> walkableAreas;
    for (const auto& junctionWithID : network->getJunctionControl()) {
        const MSJunction* const junction = junctionWithID.second;
        const ConstMSEdgeVector& incoming = junction->getIncoming();
        std::set<const MSEdge*> adjacent(incoming.begin(), incoming.end());
        const ConstMSEdgeVector& outgoing = junction->getOutgoing();
        adjacent.insert(outgoing.begin(), outgoing.end());

        bool hasWalkingArea = false;
        int pedEdgeCount = 0;
        for (const MSEdge* const edge : adjacent) {
            if (!edge->isWalkingArea()) {
                const MSLane* const lane = getSidewalk<MSEdge, MSLane>(edge);
                if (lane != nullptr) {
                    pedEdgeCount++;
                    GEOSGeometry* dilatedLane = createGeometryFromCenterLine(lane->getShape(), lane->getWidth() / 2.0, GEOSBUF_CAP_ROUND);
                    walkableAreas.push_back(dilatedLane);
                    for (const MSEdge* const nextEdge : adjacent) {
                        if (nextEdge != edge) {
                            const MSEdge* walkingArea = getWalkingAreaInbetween(edge, nextEdge);
                            if (walkingArea != nullptr) {
                                hasWalkingArea = true;
                                MSEdgeVector walkingAreaIncoming = walkingArea->getPredecessors();
                                const MSLane* const nextLane = getSidewalk<MSEdge, MSLane>(nextEdge);
                                if (nextLane != nullptr) {
                                    GEOSGeometry* walkingAreaGeom;
                                    Position anchor;
                                    Position nextAnchor;

                                    if (edge->isNormal() && nextEdge->isNormal()) {
                                        const PositionVector& walkingAreaShape = getSidewalk<MSEdge, MSLane>(walkingArea)->getShape();
                                        walkingAreaGeom = createGeometryFromShape(walkingAreaShape, junction->getID(), true);
                                        if (walkingAreaGeom) {
                                            walkableAreas.push_back(walkingAreaGeom);
                                            continue;
                                        } else {
                                            anchor = getAnchor(lane, edge, walkingAreaIncoming);
                                            nextAnchor = getAnchor(nextLane, nextEdge, walkingAreaIncoming);
                                        }
                                    } else if ((edge->isNormal() && nextEdge->isCrossing()) || (edge->isCrossing() && nextEdge->isNormal())) {
                                        MSEdgeVector walkingAreaEdges = edge->isCrossing() ? walkingAreaIncoming : walkingArea->getSuccessors();
                                        if (std::none_of(walkingAreaEdges.begin(), walkingAreaEdges.end(), [](MSEdge * e) {
                                        return e->isNormal();
                                        })) {
                                            anchor = getAnchor(lane, edge, walkingAreaIncoming);
                                            nextAnchor = getAnchor(nextLane, nextEdge, walkingAreaIncoming);
                                        }
                                    } else if (edge->isCrossing() && nextEdge->isCrossing()) {
                                        anchor = getAnchor(lane, edge, walkingAreaIncoming);
                                        nextAnchor = getAnchor(nextLane, nextEdge, walkingAreaIncoming);
                                    } else {
                                        continue;
                                    }

                                    walkingAreaGeom = createGeometryFromAnchors(anchor, lane, nextAnchor, nextLane);
                                    walkableAreas.push_back(walkingAreaGeom);
                                }
                            }
                        }
                    }
                }
            }
        }
        if (pedEdgeCount > 1 && !hasWalkingArea) {
            // there is something to connect but no walking area, let's assume peds are allowed everywhere
            GEOSGeometry* walkingAreaGeom = createGeometryFromShape(junction->getShape(), junction->getID(), true);
            if (walkingAreaGeom != nullptr) {
                walkableAreas.push_back(walkingAreaGeom);
            }
        }
    }

    // Retrieve additional walkable areas and obstacles (walkable areas and obstacles in the sense of JuPedSim).
    std::vector<GEOSGeometry*> additionalObstacles;
    for (const auto& polygonWithID : myNetwork->getShapeContainer().getPolygons()) {
        if (polygonWithID.second->getShapeType() == "jupedsim.walkable_area" || polygonWithID.second->getShapeType() == "taz") {
            GEOSGeometry* walkableArea = createGeometryFromShape(polygonWithID.second->getShape(), polygonWithID.first);
            if (walkableArea != nullptr) {
                walkableAreas.push_back(walkableArea);
                myHaveAdditionalWalkableAreas = true;
            }
        } else if (polygonWithID.second->getShapeType() == "jupedsim.obstacle") {
            GEOSGeometry* additionalObstacle = createGeometryFromShape(polygonWithID.second->getShape(), polygonWithID.first);
            if (additionalObstacle != nullptr) {
                additionalObstacles.push_back(additionalObstacle);
            }
        }
    }

    // Take the union of all walkable areas.
    GEOSGeometry* disjointWalkableAreas = GEOSGeom_createCollection(GEOS_MULTIPOLYGON, walkableAreas.data(), (unsigned int)walkableAreas.size());
#ifdef DEBUG_GEOMETRY_GENERATION
    dumpGeometry(disjointWalkableAreas, "disjointWalkableAreas.wkt");
#endif
    GEOSGeometry* initialWalkableAreas = GEOSUnaryUnion(disjointWalkableAreas);
#ifdef DEBUG_GEOMETRY_GENERATION
    dumpGeometry(initialWalkableAreas, "initialWalkableAreas.wkt");
#endif
    GEOSGeom_destroy(disjointWalkableAreas);

    // At last, remove additional obstacles from the merged walkable areas.
    GEOSGeometry* disjointAdditionalObstacles = GEOSGeom_createCollection(GEOS_MULTIPOLYGON, additionalObstacles.data(), (unsigned int)additionalObstacles.size());
#ifdef DEBUG_GEOMETRY_GENERATION
    dumpGeometry(disjointAdditionalObstacles, "disjointAdditionalObstacles.wkt");
#endif
    GEOSGeometry* additionalObstaclesUnion = GEOSUnaryUnion(disjointAdditionalObstacles); // Obstacles may overlap, e.g. if they were loaded from separate files.
#ifdef DEBUG_GEOMETRY_GENERATION
    dumpGeometry(additionalObstaclesUnion, "additionalObstaclesUnion.wkt");
#endif
    GEOSGeometry* finalWalkableAreas = GEOSDifference(initialWalkableAreas, additionalObstaclesUnion);
#ifdef DEBUG_GEOMETRY_GENERATION
    dumpGeometry(finalWalkableAreas, "finalWalkableAreas.wkt");
#endif
    GEOSGeom_destroy(initialWalkableAreas);
    GEOSGeom_destroy(additionalObstaclesUnion);
    GEOSGeom_destroy(disjointAdditionalObstacles);

    if (!GEOSisSimple(finalWalkableAreas)) {
        throw ProcessError(TL("Union of walkable areas minus union of obstacles is not a simple polygon."));
    }

    return finalWalkableAreas;
}


GEOSCoordSequence*
MSPModel_JuPedSim::convertToGEOSPoints(PositionVector shape) {
    GEOSCoordSequence* coordinateSequence = GEOSCoordSeq_create((unsigned int)shape.size(), 2);
    for (int i = 0; i < (int)shape.size(); i++) {
        GEOSCoordSeq_setXY(coordinateSequence, i, shape[i].x(), shape[i].y());
    }
    return coordinateSequence;
}


PositionVector
MSPModel_JuPedSim::convertToSUMOPoints(const GEOSGeometry* geometry) {
    PositionVector coordinateVector;
    const GEOSCoordSequence* coordinateSequence = GEOSGeom_getCoordSeq(geometry);
    unsigned int coordinateSequenceSize;
    GEOSCoordSeq_getSize(coordinateSequence, &coordinateSequenceSize);
    double x;
    double y;
    for (unsigned int i = 0; i < coordinateSequenceSize; i++) {
        GEOSCoordSeq_getX(coordinateSequence, i, &x);
        GEOSCoordSeq_getY(coordinateSequence, i, &y);
        coordinateVector.push_back(Position(x, y));
    }
    return coordinateVector;
}


std::vector<JPS_Point>
MSPModel_JuPedSim::convertToJPSPoints(const GEOSGeometry* geometry) {
    std::vector<JPS_Point> pointVector;
    const GEOSCoordSequence* coordinateSequence = GEOSGeom_getCoordSeq(geometry);
    unsigned int coordinateSequenceSize;
    GEOSCoordSeq_getSize(coordinateSequence, &coordinateSequenceSize);
    double x;
    double y;
    // Remove the last point so that CGAL doesn't complain about the simplicity of the polygon downstream.
    for (unsigned int i = 0; i < coordinateSequenceSize - 1; i++) {
        GEOSCoordSeq_getX(coordinateSequence, i, &x);
        GEOSCoordSeq_getY(coordinateSequence, i, &y);
        pointVector.push_back({x, y});
    }
    return pointVector;
}


double
MSPModel_JuPedSim::getHoleArea(const GEOSGeometry* hole) {
    double area;
    GEOSGeometry* linearRingAsPolygon = GEOSGeom_createPolygon(GEOSGeom_clone(hole), nullptr, 0);
    GEOSArea(linearRingAsPolygon, &area);
    GEOSGeom_destroy(linearRingAsPolygon);
    return area;
}


void
MSPModel_JuPedSim::filterHoles(const GEOSGeometry* geometry, double minAreaThreshold) {
    GEOSGeometry* exterior = GEOSGeom_clone(GEOSGetExteriorRing(geometry));
    int nbrInteriorRings = GEOSGetNumInteriorRings(geometry);
    if (nbrInteriorRings != -1) {
        std::vector<GEOSGeometry*> holes;
        for (int k = 0; k < nbrInteriorRings; k++) {
            GEOSGeometry* linearRing = GEOSGeom_clone(GEOSGetInteriorRingN(geometry, k));
            const double area = getHoleArea(linearRing);
            if (area > minAreaThreshold) {
                holes.push_back(linearRing);
            }
        }
        geometry = GEOSGeom_createPolygon(exterior, holes.data(), nbrInteriorRings);
    }
}


void
MSPModel_JuPedSim::removePolygonFromDrawing(const std::string& polygonId) {
    myShapeContainer.removePolygon(polygonId);
}


void
MSPModel_JuPedSim::preparePolygonForDrawing(const GEOSGeometry* polygon, const std::string& polygonId, const RGBColor& color) {
    const GEOSGeometry* exterior = GEOSGetExteriorRing(polygon);
    bool added = myShapeContainer.addPolygon(polygonId, std::string("jupedsim.pedestrian_network"), color, 10.0, 0.0,
                 std::string(), false, convertToSUMOPoints(exterior), false, true, 1.0);
    if (added) {
        std::vector<PositionVector> holes;
        int nbrInteriorRings = GEOSGetNumInteriorRings(polygon);
        if (nbrInteriorRings != -1) {
            for (int k = 0; k < nbrInteriorRings; k++) {
                const GEOSGeometry* linearRing = GEOSGetInteriorRingN(polygon, k);
                holes.push_back(convertToSUMOPoints(linearRing));
            }
            myShapeContainer.getPolygons().get(polygonId)->setHoles(holes);
        }
    }
}


const GEOSGeometry*
MSPModel_JuPedSim::getLargestComponent(const GEOSGeometry* polygon, int& nbrComponents, double& maxArea, double& totalArea) {
    nbrComponents = GEOSGetNumGeometries(polygon);
    const GEOSGeometry* largestComponent = nullptr;
    maxArea = 0.0;
    totalArea = 0.0;
    for (int i = 0; i < nbrComponents; i++) {
        const GEOSGeometry* componentPolygon = GEOSGetGeometryN(polygon, i);
        double area;
        GEOSArea(componentPolygon, &area);
        totalArea += area;
        if (area > maxArea) {
            maxArea = area;
            largestComponent = componentPolygon;
        }
    }
    return largestComponent;
}


JPS_Geometry
MSPModel_JuPedSim::buildJPSGeometryFromGEOSGeometry(const GEOSGeometry* polygon) {
    JPS_GeometryBuilder geometryBuilder = JPS_GeometryBuilder_Create();

    // Handle the exterior polygon.
    const GEOSGeometry* exterior =  GEOSGetExteriorRing(polygon);
    std::vector<JPS_Point> exteriorCoordinates = convertToJPSPoints(exterior);
    JPS_GeometryBuilder_AddAccessibleArea(geometryBuilder, exteriorCoordinates.data(), exteriorCoordinates.size());

    // Handle the interior polygons (holes).
    int nbrInteriorRings = GEOSGetNumInteriorRings(polygon);
    if (nbrInteriorRings != -1) {
        for (int k = 0; k < nbrInteriorRings; k++) {
            const GEOSGeometry* linearRing = GEOSGetInteriorRingN(polygon, k);
            std::vector<JPS_Point> holeCoordinates = convertToJPSPoints(linearRing);
            JPS_GeometryBuilder_ExcludeFromAccessibleArea(geometryBuilder, holeCoordinates.data(), holeCoordinates.size());
        }
    }

    JPS_ErrorMessage message = nullptr;
    JPS_Geometry geometry = JPS_GeometryBuilder_Build(geometryBuilder, &message);
    if (geometry == nullptr) {
        const std::string error = TLF("Error while generating geometry: %", JPS_ErrorMessage_GetMessage(message));
        JPS_ErrorMessage_Free(message);
        throw ProcessError(error);
    }
    JPS_GeometryBuilder_Free(geometryBuilder);
    return geometry;
}


void
MSPModel_JuPedSim::dumpGeometry(const GEOSGeometry* polygon, const std::string& filename, bool useGeoCoordinates) {
    GEOSGeometry* polygonGeoCoordinates = nullptr;
    if (useGeoCoordinates) {
        const GEOSGeometry* exterior =  GEOSGetExteriorRing(polygon);
        PositionVector exteriorPoints = convertToSUMOPoints(exterior);
        for (Position& position : exteriorPoints) {
            GeoConvHelper::getFinal().cartesian2geo(position);
        }
        const int nbrInteriorRings = GEOSGetNumInteriorRings(polygon);
        std::vector<GEOSGeometry*> holeRings;
        if (nbrInteriorRings != -1) {
            for (int k = 0; k < nbrInteriorRings; k++) {
                const GEOSGeometry* linearRing = GEOSGetInteriorRingN(polygon, k);
                PositionVector holePoints = convertToSUMOPoints(linearRing);
                for (Position& position : holePoints) {
                    GeoConvHelper::getFinal().cartesian2geo(position);
                }
                GEOSCoordSequence* holeRingCoords = convertToGEOSPoints(holePoints);
                GEOSGeometry* holeRing = GEOSGeom_createLinearRing(holeRingCoords);
                holeRings.push_back(holeRing);
            }
        }
        GEOSCoordSequence* exteriorRingCoords = convertToGEOSPoints(exteriorPoints);
        GEOSGeometry* exteriorRing = GEOSGeom_createLinearRing(exteriorRingCoords);
        polygonGeoCoordinates = GEOSGeom_createPolygon(exteriorRing, holeRings.data(), nbrInteriorRings);
    }
    std::ofstream dumpFile;
    dumpFile.open(filename);
    GEOSWKTWriter* writer = GEOSWKTWriter_create();
    char* wkt = GEOSWKTWriter_write(writer, polygonGeoCoordinates == nullptr ? polygon : polygonGeoCoordinates);
    dumpFile << wkt << std::endl;
    dumpFile.close();
    GEOSFree(wkt);
    GEOSWKTWriter_destroy(writer);
    GEOSGeom_destroy(polygonGeoCoordinates);
}


void
MSPModel_JuPedSim::initialize(const OptionsCont& oc) {
    initGEOS(nullptr, nullptr);
    PROGRESS_BEGIN_MESSAGE("Generating initial JuPedSim geometry for pedestrian network");
    myGEOSPedestrianNetwork = buildPedestrianNetwork(myNetwork);
    int nbrComponents = 0;
    double maxArea = 0.0;
    double totalArea = 0.0;
    myGEOSPedestrianNetworkLargestComponent = getLargestComponent(myGEOSPedestrianNetwork, nbrComponents, maxArea, totalArea);
    if (nbrComponents > 1) {
        WRITE_WARNINGF(TL("While generating geometry % connected components were detected, %% of total pedestrian area is covered by the largest."),
                       nbrComponents, maxArea / totalArea * 100.0, "%");
    }
    filterHoles(myGEOSPedestrianNetworkLargestComponent, GEOS_MIN_AREA);
#ifdef DEBUG_GEOMETRY_GENERATION
    dumpGeometry(myGEOSPedestrianNetworkLargestComponent, "pedestrianNetwork.wkt");
#endif
    std::string filename = oc.getString("pedestrian.jupedsim.wkt");
    if (!filename.empty()) {
        dumpGeometry(myGEOSPedestrianNetworkLargestComponent, filename, oc.getBool("pedestrian.jupedsim.wkt.geo"));
    }
    // For the moment, only one connected component is supported.
    myJPSGeometry = buildJPSGeometryFromGEOSGeometry(myGEOSPedestrianNetworkLargestComponent);
    myJPSGeometryWithTrainsAndRamps = nullptr;
    preparePolygonForDrawing(myGEOSPedestrianNetworkLargestComponent, PEDESTRIAN_NETWORK_ID, PEDESTRIAN_NETWORK_COLOR);
    PROGRESS_DONE_MESSAGE();
    JPS_ErrorMessage message = nullptr;

    double strengthGeometryRepulsion = oc.getFloat("pedestrian.jupedsim.strength-geometry-repulsion");
    double rangeGeometryRepulsion = oc.getFloat("pedestrian.jupedsim.range-geometry-repulsion");
    if (myJPSDeltaT == 20) {
        if (oc.isDefault("pedestrian.jupedsim.strength-geometry-repulsion") && oc.isDefault("pedestrian.jupedsim.range-geometry-repulsion")) {
            WRITE_MESSAGE(TL("Adapting geometry repulsion default values for jupedsim timestep of 0.02."));
            strengthGeometryRepulsion = 35.;
            rangeGeometryRepulsion = 0.019;
        }
    }
    if (oc.getString("pedestrian.jupedsim.model") == "CollisionFreeSpeed") {
        JPS_CollisionFreeSpeedModelBuilder modelBuilder = JPS_CollisionFreeSpeedModelBuilder_Create(oc.getFloat("pedestrian.jupedsim.strength-neighbor-repulsion"),
                oc.getFloat("pedestrian.jupedsim.range-neighbor-repulsion"),
                strengthGeometryRepulsion, rangeGeometryRepulsion);
        myJPSModel = JPS_CollisionFreeSpeedModelBuilder_Build(modelBuilder, &message);
        JPS_CollisionFreeSpeedModelBuilder_Free(modelBuilder);
    } else {
        throw ProcessError(TLF("Unknown JuPedSim model: %", oc.getString("pedestrian.jupedsim.model")));
    }

    if (myJPSModel == nullptr) {
        const std::string error = TLF("Error creating the pedestrian model: %", JPS_ErrorMessage_GetMessage(message));
        JPS_ErrorMessage_Free(message);
        throw ProcessError(error);
    }
    myJPSSimulation = JPS_Simulation_Create(myJPSModel, myJPSGeometry, STEPS2TIME(myJPSDeltaT), &message);
    if (myJPSSimulation == nullptr) {
        const std::string error = TLF("Error creating the simulation: %", JPS_ErrorMessage_GetMessage(message));
        JPS_ErrorMessage_Free(message);
        throw ProcessError(error);
    }
    // Polygons that define vanishing areas aren't part of the regular JuPedSim geometry.
    for (const auto& polygonWithID : myNetwork->getShapeContainer().getPolygons()) {
        const SUMOPolygon* const poly = polygonWithID.second;
        if (poly->getShapeType() == "jupedsim.vanishing_area" || poly->getShapeType() == "jupedsim.influencer") {
            std::vector<JPS_Point> areaBoundary;
            for (const Position& p : poly->getShape()) {
                areaBoundary.push_back({p.x(), p.y()});
            }
            // Make sure the shape is not repeating the first point.
            if (areaBoundary.back().x == areaBoundary.front().x && areaBoundary.back().y == areaBoundary.front().y) {
                areaBoundary.pop_back();
            }
            const std::string type = StringTokenizer(poly->getShapeType(), ".").getVector()[1];
            myAreas.push_back({poly->getID(), type, areaBoundary, poly->getParametersMap(), 0});
        }
    }
}


MSLane* MSPModel_JuPedSim::getNextPedestrianLane(const MSLane* const currentLane) {
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
MSPModel_JuPedSim::PState::PState(MSPerson* person, MSStageMoving* stage,
                                  JPS_JourneyId journeyId, JPS_StageId stageId,
                                  const std::vector<std::pair<Position, double> >& waypoints)
    : myPerson(person), myStage(stage), myJourneyId(journeyId), myStageId(stageId), myWaypoints(waypoints),
      myAgentId(0), myPosition(0, 0), myAngle(0), myWaitingToEnter(true) {
}


void
MSPModel_JuPedSim::PState::reinit(MSStageMoving* stage, JPS_JourneyId journeyId, JPS_StageId stageId,
                                  const std::vector<std::pair<Position, double> >& waypoints) {
    if (myStage != nullptr) {
        myStage->setPState(nullptr);  // we need to remove the old state reference to avoid double deletion
    }
    myStage = stage;
    myJourneyId = journeyId;
    myStageId = stageId;
    myWaypoints = waypoints;
}


MSPModel_JuPedSim::PState::~PState() {
}


Position MSPModel_JuPedSim::PState::getPosition(const MSStageMoving& /* stage */, SUMOTime /* now */) const {
    return myPosition;
}


void MSPModel_JuPedSim::PState::setPosition(double x, double y) {
    myPosition.set(x, y);
}


void MSPModel_JuPedSim::PState::setPreviousPosition(Position previousPosition) {
    myPreviousPosition = previousPosition;
}


double MSPModel_JuPedSim::PState::getAngle(const MSStageMoving& /* stage */, SUMOTime /* now */) const {
    return myAngle;
}


void MSPModel_JuPedSim::PState::setAngle(double angle) {
    myAngle = angle;
}


MSStageMoving* MSPModel_JuPedSim::PState::getStage() const {
    return myStage;
}


void
MSPModel_JuPedSim::PState::setStage(MSStageMoving* const stage) {
    myStage = stage;
}


MSPerson* MSPModel_JuPedSim::PState::getPerson() const {
    return myPerson;
}


void MSPModel_JuPedSim::PState::setLanePosition(double lanePosition) {
    myLanePosition = lanePosition;
}


double MSPModel_JuPedSim::PState::getEdgePos(const MSStageMoving& /* stage */, SUMOTime /* now */) const {
    return myLanePosition;
}


int MSPModel_JuPedSim::PState::getDirection(const MSStageMoving& /* stage */, SUMOTime /* now */) const {
    return UNDEFINED_DIRECTION;
}


SUMOTime MSPModel_JuPedSim::PState::getWaitingTime(const MSStageMoving& /* stage */, SUMOTime /* now */) const {
    return 0;
}


double MSPModel_JuPedSim::PState::getSpeed(const MSStageMoving& /* stage */) const {
    return myPosition.distanceTo2D(myPreviousPosition) / STEPS2TIME(DELTA_T);
}


const MSEdge* MSPModel_JuPedSim::PState::getNextEdge(const MSStageMoving& stage) const {
    return stage.getNextRouteEdge();
}


const std::pair<Position, double>& MSPModel_JuPedSim::PState::getNextWaypoint() const {
    return myWaypoints.front();
}


JPS_AgentId MSPModel_JuPedSim::PState::getAgentId() const {
    return myAgentId;
}
