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


// ===========================================================================
// method definitions
// ===========================================================================
MSPModel_JuPedSim::MSPModel_JuPedSim(const OptionsCont& oc, MSNet* net) :
    myNetwork(net), myJPSDeltaT(string2time(oc.getString("pedestrian.jupedsim.step-length"))),
    myExitTolerance(oc.getFloat("pedestrian.jupedsim.exit-tolerance")), myHaveAdditionalWalkableAreas(false) {
    initialize();
    net->getBeginOfTimestepEvents()->addEvent(new Event(this), net->getCurrentTimeStep() + DELTA_T);
}


MSPModel_JuPedSim::~MSPModel_JuPedSim() {
    clearState();

    JPS_Simulation_Free(myJPSSimulation);
    JPS_OperationalModel_Free(myJPSModel);
    JPS_CollisionFreeSpeedModelBuilder_Free(myJPSModelBuilder);
    JPS_Geometry_Free(myJPSGeometry);
    JPS_GeometryBuilder_Free(myJPSGeometryBuilder);

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
MSPModel_JuPedSim::addWaypoint(JPS_JourneyDescription journey, JPS_StageId& predecessor, const Position& point, const std::string& agentID) {
    JPS_ErrorMessage message = nullptr;
    const JPS_StageId waypointId = JPS_Simulation_AddStageWaypoint(myJPSSimulation, {point.x(), point.y()}, myExitTolerance, &message);
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
    for (PState* const pstate : myPedestrianStates) {  // TODO transform myPedestrianStates into a map for faster lookup
        if (pstate->getPerson() == person) {
            return pstate;
        }
    }
    Position departurePosition = Position::INVALID;
    const MSLane* const departureLane = getSidewalk<MSEdge, MSLane>(stage->getRoute().front());
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

    JPS_JourneyDescription journey = JPS_JourneyDescription_Create();
    JPS_StageId startingStage = 0;
    JPS_StageId predecessor = 0;

    int stageOffset = 1;
    PositionVector waypoints;
    while (person->getNumRemainingStages() > stageOffset) {
        const MSStage* const next = person->getNextStage(stageOffset);
        if (!next->isWalk()) {
            break;
        }
        const MSStage* const prev = person->getNextStage(stageOffset - 1);
        if (prev->getDestination() != next->getEdge()) {
            // she is probably using an access
            break;
        }
        double prevArrivalPos = prev->getArrivalPos();
        if (prev->getDestinationStop() != nullptr) {
            prevArrivalPos = prev->getDestinationStop()->getAccessPos(prev->getDestination());
        }
        waypoints.push_back(getSidewalk<MSEdge, MSLane>(prev->getDestination())->getShape().positionAtOffset(prevArrivalPos));
        if (!addWaypoint(journey, predecessor, waypoints.back(), person->getID())) {
            return nullptr;
        }
        if (startingStage == 0) {
            startingStage = predecessor;
        }
        stageOffset++;
    }

    const MSStage* const arrivalStage = person->getNextStage(stageOffset - 1);
    const MSLane* const arrivalLane = getSidewalk<MSEdge, MSLane>(arrivalStage->getDestination());
    const Position arrivalPosition = arrivalLane->getShape().positionAtOffset(arrivalStage->getArrivalPos());
    waypoints.push_back(arrivalPosition);

    if (!addWaypoint(journey, predecessor, arrivalPosition, person->getID())) {
        return nullptr;
    }
    if (startingStage == 0) {
        startingStage = predecessor;
    }
    JPS_ErrorMessage message = nullptr;
    JPS_JourneyId journeyId = JPS_Simulation_AddJourney(myJPSSimulation, journey, &message);
    if (message != nullptr) {
        WRITE_WARNINGF(TL("Error while adding a journey for person '%': %"), person->getID(), JPS_ErrorMessage_GetMessage(message));
        JPS_ErrorMessage_Free(message);
        return nullptr;
    }

    PState* state = new PState(static_cast<MSPerson*>(person), stage, journey, journeyId, startingStage, waypoints);
    state->setLanePosition(stage->getDepartPos());
    state->setPreviousPosition(departurePosition);
    state->setPosition(departurePosition.x(), departurePosition.y());
    state->setAngle(departureLane->getShape().rotationAtOffset(stage->getDepartPos()));
    myPedestrianStates.push_back(state);
    myNumActivePedestrians++;
    tryPedestrianInsertion(state, state->getPosition(*state->getStage(), now));

    return state;
}


void
MSPModel_JuPedSim::remove(MSTransportableStateAdapter* /* state */) {
    // This function is called only when using TraCI.
    // Not sure what to do here.
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
            Position p = state->getPosition(*state->getStage(), time) + Position(RandHelper::rand(-.5, .5), RandHelper::rand(-.5, .5));
            tryPedestrianInsertion(state, p);
            ++stateIt;
            continue;
        }

        MSPerson* person = state->getPerson();
        MSStageWalking* stage = dynamic_cast<MSStageWalking*>(person->getCurrentStage());
        assert(stage != nullptr);

        // Updates the agent position.
        auto agent = JPS_Simulation_GetAgent(myJPSSimulation, state->getAgentId(), nullptr);
        state->setPreviousPosition(state->getPosition(*stage, DELTA_T));
        const auto position = JPS_Agent_GetPosition(agent);
        state->setPosition(position.x, position.y);

        // Updates the agent direction.
        const auto orientation = JPS_Agent_GetOrientation(agent);
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
        }

        if (newPosition.distanceTo2D(state->getNextWaypoint()) < 2 * myExitTolerance) {
            while (!stage->moveToNextEdge(person, time, 1, nullptr));
            // If near the last waypoint, remove the agent.
            if (state->advanceNextWaypoint()) {
                registerArrived();
                JPS_Simulation_MarkAgentForRemoval(myJPSSimulation, state->getAgentId(), nullptr);
                stateIt = myPedestrianStates.erase(stateIt);
            }
        } else {
            ++stateIt;
        }
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
    const unsigned int size = (unsigned int)centerLine.size();
    GEOSCoordSequence* coordinateSequence = GEOSCoordSeq_create(size, 2);
    for (unsigned int i = 0; i < size; i++) {
        GEOSCoordSeq_setXY(coordinateSequence, i, centerLine[i].x(), centerLine[i].y());
    }
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
        if (shape[i] != shape[i-1]) {
            cleanShape.push_back(shape[i]);
        } else {
            duplicates.push_back(shape[i]);
        }
    }
    if (cleanShape.size() < shape.size()) {
        WRITE_WARNINGF(TL("Polygon '%' had some equal consecutive points removed: %"), shapeID, toString(duplicates, 9));
    }
    GEOSCoordSequence* coordinateSequence = GEOSCoordSeq_create((unsigned int)cleanShape.size(), 2);
    for (int i = 0; i < (int)cleanShape.size(); i++) {
        GEOSCoordSeq_setXY(coordinateSequence, i, cleanShape[i].x(), cleanShape[i].y());
    }
    GEOSGeometry* linearRing = GEOSGeom_createLinearRing(coordinateSequence);
    GEOSGeometry* polygon = GEOSGeom_createPolygon(linearRing, nullptr, 0);
    if (!GEOSisSimple(polygon)) {
        if (cleanShape.size() == 3) {
            if (isInternalShape) {
                WRITE_WARNINGF(TL("Polygon for junction '%' has been dilated as it is just a segment."), shapeID);
            } else {
                WRITE_WARNINGF(TL("Polygon '%' has been dilated as it is just a segment."), shapeID);
            }
            GEOSGeometry* lineString = GEOSGeom_createLineString(coordinateSequence);
            polygon = GEOSBufferWithStyle(lineString, GEOS_BUFFERED_SEGMENT_WIDTH, GEOS_QUADRANT_SEGMENTS, GEOSBUF_CAP_ROUND, GEOSBUF_JOIN_ROUND, GEOS_MITRE_LIMIT);
            GEOSGeom_destroy(lineString);
        } else {
            if (isInternalShape) {
                WRITE_WARNINGF(TL("Polygon on junction '%' has been replaced by its convex hull as it is not simple."), shapeID);
                polygon = GEOSConvexHull(polygon);
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


PositionVector
MSPModel_JuPedSim::getCoordinates(const GEOSGeometry* geometry) {
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
MSPModel_JuPedSim::convertToJPSPoints(const PositionVector& coordinates) {
    std::vector<JPS_Point> pointVector;
    for (const Position& p : coordinates) {
        pointVector.push_back({p.x(), p.y()});
    }
    // Remove the last point so that CGAL doesn't complain about the simplicity of the polygon downstream.
    pointVector.pop_back();
    return pointVector;
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
MSPModel_JuPedSim::preparePolygonForDrawing(const GEOSGeometry* polygon, const std::string& polygonId) {
    const GEOSGeometry* exterior = GEOSGetExteriorRing(polygon);
    PositionVector shape = getCoordinates(exterior);

    std::vector<PositionVector> holes;
    int nbrInteriorRings = GEOSGetNumInteriorRings(polygon);
    if (nbrInteriorRings != -1) {
        for (unsigned int k = 0; k < (unsigned int)nbrInteriorRings; k++) {
            const GEOSGeometry* linearRing = GEOSGetInteriorRingN(polygon, k);
            double area = getHoleArea(linearRing);
            if (area > GEOS_MIN_AREA) {
                PositionVector hole = getCoordinates(linearRing);
                holes.push_back(hole);
            }
        }

        ShapeContainer& shapeContainer = myNetwork->getShapeContainer();
        shapeContainer.addPolygon(polygonId, std::string("jupedsim.pedestrian_network"), RGBColor(179, 217, 255, 255), 10.0, 0.0, std::string(), false, shape, false, true, 1.0);
        shapeContainer.getPolygons().get(polygonId)->setHoles(holes);
    }
}


void
MSPModel_JuPedSim::preparePolygonForJPS(const GEOSGeometry* polygon) {
    // Handle the exterior polygon.
    const GEOSGeometry* exterior =  GEOSGetExteriorRing(polygon);
    std::vector<JPS_Point> exteriorCoordinates = convertToJPSPoints(exterior);
    JPS_GeometryBuilder_AddAccessibleArea(myJPSGeometryBuilder, exteriorCoordinates.data(), exteriorCoordinates.size());

    // Handle the interior polygons (holes).
    int nbrInteriorRings = GEOSGetNumInteriorRings(polygon);
    if (nbrInteriorRings != -1) {
        for (unsigned int k = 0; k < (unsigned int)nbrInteriorRings; k++) {
            const GEOSGeometry* linearRing = GEOSGetInteriorRingN(polygon, k);
            double area = getHoleArea(linearRing);
            if (area > GEOS_MIN_AREA) {
                std::vector<JPS_Point> holeCoordinates = convertToJPSPoints(linearRing);
                JPS_GeometryBuilder_ExcludeFromAccessibleArea(myJPSGeometryBuilder, holeCoordinates.data(), holeCoordinates.size());
            }
        }
    }
}


void
MSPModel_JuPedSim::dumpGeometry(const GEOSGeometry* polygon, const std::string& filename) {
    std::ofstream dumpFile;
    dumpFile.open(filename);
    GEOSWKTWriter* writer = GEOSWKTWriter_create();
    char* wkt = GEOSWKTWriter_write(writer, polygon);
    dumpFile << wkt << std::endl;
    dumpFile.close();
    GEOSFree(wkt);
    GEOSWKTWriter_destroy(writer);
}


void
MSPModel_JuPedSim::initialize() {
    initGEOS(nullptr, nullptr);
    myGEOSPedestrianNetwork = buildPedestrianNetwork(myNetwork);
    int nbrConnectedComponents = GEOSGetNumGeometries(myGEOSPedestrianNetwork);

    // myJPSGeometryBuilder = JPS_GeometryBuilder_Create();
    // for (size_t i = 0; i < GEOSGetNumGeometries(myGEOSPedestrianNetwork); i++) {
    //     const GEOSGeometry* connectedComponentPolygon = GEOSGetGeometryN(myGEOSPedestrianNetwork, i);
    //     std::string polygonId = std::string("pedestrian_network_connected_component_") + std::to_string(i);
    //     preparePolygonForDrawing(connectedComponentPolygon, polygonId);
    //     preparePolygonForJPS(connectedComponentPolygon);
    // }
    // prepareAdditionalPolygonsForJPS();

    // For the moment, JuPedSim only supports one connected component, select the one with max area.
    const GEOSGeometry* maxAreaConnectedComponentPolygon = nullptr;
    std::string maxAreaPolygonId;
    double maxArea = 0.0;
    double totalArea = 0.0;
    for (unsigned int i = 0; i < (unsigned int)nbrConnectedComponents; i++) {
        const GEOSGeometry* connectedComponentPolygon = GEOSGetGeometryN(myGEOSPedestrianNetwork, i);
        std::string polygonId = std::string("jupedsim.pedestrian_network.") + std::to_string(i);
        double area;
        GEOSArea(connectedComponentPolygon, &area);
        totalArea += area;
        if (area > maxArea) {
            maxArea = area;
            maxAreaConnectedComponentPolygon = connectedComponentPolygon;
            maxAreaPolygonId = polygonId;
        }
    }
    if (nbrConnectedComponents > 1) {
        WRITE_WARNINGF(TL("While generating geometry % connected components were detected, %% of total pedestrian area is covered by the first."),
            nbrConnectedComponents, maxArea/totalArea*100.0, "%");
    }
#ifdef DEBUG_GEOMETRY_GENERATION
    dumpGeometry(maxAreaConnectedComponentPolygon, "pedestrianNetwork.wkt");
#endif
    myJPSGeometryBuilder = JPS_GeometryBuilder_Create();
    preparePolygonForJPS(maxAreaConnectedComponentPolygon);
    preparePolygonForDrawing(maxAreaConnectedComponentPolygon, maxAreaPolygonId);

    JPS_ErrorMessage message = nullptr;
    myJPSGeometry = JPS_GeometryBuilder_Build(myJPSGeometryBuilder, &message);
    if (myJPSGeometry == nullptr) {
        const std::string error = TLF("Error creating the geometry: %", JPS_ErrorMessage_GetMessage(message));
        JPS_ErrorMessage_Free(message);
        throw ProcessError(error);
    } else {
        WRITE_MESSAGE("Geometry generation for JuPedSim done.");
    }
    myJPSModelBuilder = JPS_CollisionFreeSpeedModelBuilder_Create(8.0, 0.1, 5.0, 0.02);
    myJPSModel = JPS_CollisionFreeSpeedModelBuilder_Build(myJPSModelBuilder, &message);
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
                                  JPS_JourneyDescription journey, JPS_JourneyId journeyId, JPS_StageId stageId,
                                  const PositionVector& waypoints)
    : myPerson(person), myStage(stage), myJourney(journey), myJourneyId(journeyId), myStageId(stageId), myWaypoints(waypoints),
      myAgentId(0), myPosition(0, 0), myAngle(0), myWaitingToEnter(true) {
}


MSPModel_JuPedSim::PState::~PState() {
    JPS_JourneyDescription_Free(myJourney);
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


MSStageMoving* MSPModel_JuPedSim::PState::getStage() {
    return myStage;
}


MSPerson* MSPModel_JuPedSim::PState::getPerson() {
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


const Position& MSPModel_JuPedSim::PState::getNextWaypoint() const {
    return myWaypoints.front();
}


JPS_AgentId MSPModel_JuPedSim::PState::getAgentId() const {
    return myAgentId;
}
