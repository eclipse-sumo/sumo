/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2014-2023 German Aerospace Center (DLR) and others.
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
/// @file    MSPModel_JuPedSim.h
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
#include <unordered_set>
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
#include <libsumo/Helper.h>
#include <utils/geom/Position.h>
#include <utils/geom/PositionVector.h>
#include <utils/options/OptionsCont.h>
#include <utils/shapes/ShapeContainer.h>
#include "MSPModel_Striping.h"
#include "MSPModel_JuPedSim.h"
#include "MSPerson.h"


const int MSPModel_JuPedSim::GEOS_QUADRANT_SEGMENTS = 16;
const double MSPModel_JuPedSim::GEOS_MITRE_LIMIT= 5.0;
const double MSPModel_JuPedSim::GEOS_MIN_AREA = 10.0;


unsigned int toUINT(size_t bigInteger)
{
    if (bigInteger > UINT_MAX) {
       throw std::bad_cast();
    }
    return static_cast<unsigned int>(bigInteger);
}


MSPModel_JuPedSim::MSPModel_JuPedSim(const OptionsCont& oc, MSNet* net) :
    myNetwork(net), myJPSDeltaT(string2time(oc.getString("pedestrian.jupedsim.step-length"))),
    myExitTolerance(oc.getFloat("pedestrian.jupedsim.exit-tolerance")) {
    initialize();
    net->getBeginOfTimestepEvents()->addEvent(new Event(this), net->getCurrentTimeStep() + DELTA_T);
}


MSPModel_JuPedSim::~MSPModel_JuPedSim() {
    clearState();

    JPS_Simulation_Free(myJPSSimulation);
    JPS_OperationalModel_Free(myJPSModel);
    JPS_VelocityModelBuilder_Free(myJPSModelBuilder);
    JPS_Geometry_Free(myJPSGeometry);
    JPS_GeometryBuilder_Free(myJPSGeometryBuilder);

    GEOSGeom_destroy(myGEOSPedestrianNetwork);
    finishGEOS();
}


void
MSPModel_JuPedSim::tryPedestrianInsertion(PState* state) {
	JPS_VelocityModelAgentParameters agent_parameters{};
	agent_parameters.journeyId = state->getJourneyId();
	agent_parameters.position = {state->getPosition(*state->getStage(), 0).x(), state->getPosition(*state->getStage(), 0).y()};
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
    agent_parameters.orientation =  orientation;
    std::string pedestrianTypeID = state->getPerson()->getVehicleType().getID();
    std::map<std::string, JPS_ModelParameterProfileId>::iterator it = myJPSParameterProfileIds.find(pedestrianTypeID);
    if (it != myJPSParameterProfileIds.end()) {
        agent_parameters.profileId = it->second;
    } else {
        WRITE_WARNINGF(TL("Error while adding an agent: vType '%' hasn't been registered as a JuPedSim parameter profile, using the default type."), pedestrianTypeID);
        agent_parameters.profileId = myJPSParameterProfileIds[DEFAULT_PEDTYPE_ID];
    }

    JPS_ErrorMessage message = nullptr;
    JPS_AgentId agentId = JPS_Simulation_AddVelocityModelAgent(myJPSSimulation, agent_parameters, &message);
    if (message != nullptr) {
        WRITE_WARNINGF(TL("Error while adding an agent: %"), JPS_ErrorMessage_GetMessage(message));
        JPS_ErrorMessage_Free(message);
    } else {
        state->setAgentId(agentId);
    }
}


MSTransportableStateAdapter*
MSPModel_JuPedSim::add(MSTransportable* person, MSStageMoving* stage, SUMOTime /* now */) {
	assert(person->getCurrentStageType() == MSStageType::WALKING);

    const MSLane* const departureLane = getSidewalk<MSEdge, MSLane>(stage->getRoute().front());
    const double halfDepartureLaneWidth = departureLane->getWidth() / 2.0;
    double departureRelativePositionY = stage->getDepartPosLat();
    if (departureRelativePositionY == UNSPECIFIED_POS_LAT) {
        departureRelativePositionY = 0.0;
    }
    if (departureRelativePositionY == MSPModel::RANDOM_POS_LAT) {
        departureRelativePositionY = RandHelper::rand(-halfDepartureLaneWidth, halfDepartureLaneWidth);
    }
    const Position departurePosition = departureLane->getShape().positionAtOffset(stage->getDepartPos(), -departureRelativePositionY); // Minus sign is here for legacy reasons.
    
    const MSLane* const arrivalLane = getSidewalk<MSEdge, MSLane>(stage->getRoute().back());
    const Position arrivalPosition = arrivalLane->getShape().positionAtOffset(stage->getArrivalPos());

    const JPS_StageId waypointId = JPS_Simulation_AddStageWaypoint(myJPSSimulation, {arrivalPosition.x(), arrivalPosition.y()}, myExitTolerance, nullptr);
    JPS_JourneyDescription journey = JPS_JourneyDescription_Create();
    JPS_JourneyDescription_AddStage(journey, waypointId);
    JPS_JourneyId journeyId = JPS_Simulation_AddJourney(myJPSSimulation, journey, nullptr);

    PState* state = new PState(static_cast<MSPerson*>(person), stage, journey, journeyId, arrivalPosition);
    state->setLanePosition(stage->getDepartPos());
    state->setPreviousPosition(departurePosition);
    state->setPosition(departurePosition.x(), departurePosition.y());
    state->setAngle(departureLane->getShape().rotationAtOffset(stage->getDepartPos()));
    myPedestrianStates.push_back(state);
    myNumActivePedestrians++;
    tryPedestrianInsertion(state);
		
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
            tryPedestrianInsertion(state);
            ++stateIt;
            continue;
        }

        MSPerson* person = state->getPerson();
        MSPerson::MSPersonStage_Walking* stage = dynamic_cast<MSPerson::MSPersonStage_Walking*>(person->getCurrentStage());

        // Updates the agent position.
        JPS_VelocityModelAgentParameters agent{};
        JPS_Simulation_ReadVelocityModelAgent(myJPSSimulation, state->getAgentId(), &agent, nullptr);
        state->setPreviousPosition(state->getPosition(*stage, DELTA_T));
        state->setPosition(agent.position.x, agent.position.y);

        // Updates the agent direction.
        state->setAngle(atan2(agent.orientation.y, agent.orientation.x));

        // Find on which edge the pedestrian is, using route's forward-looking edges because of how moveToXY is written.
        Position newPosition(agent.position.x, agent.position.y);
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

        const MSEdge* expectedEdge = stage->getEdge();
        const MSLane* expectedLane = getSidewalk<MSEdge, MSLane>(expectedEdge);
        if (found && expectedLane->isNormal() && candidateLane->isNormal() && candidateLane != expectedLane) {
            state->setLanePosition(candidateLaneLongitudinalPosition);
            const bool result = stage->moveToNextEdge(person, time, 1, nullptr);
            UNUSED_PARAMETER(result);
            assert(result == false); // The person has not arrived yet.
        }
        
        // If near the last waypoint, remove the agent.
        if (newPosition.distanceTo2D(state->getDestination()) < myExitTolerance) {
            JPS_Simulation_RemoveAgent(myJPSSimulation, state->getAgentId(), nullptr);
            while (!stage->moveToNextEdge(person, time, 1, nullptr));
            registerArrived();
            stateIt = myPedestrianStates.erase(stateIt);
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
    unsigned int size = toUINT(centerLine.size());
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
MSPModel_JuPedSim::createGeometryFromShape(PositionVector shape) {
    if (shape.back() != shape.front()) {
        shape.push_back(shape.front());
    }
    GEOSCoordSequence* coordSeq = GEOSCoordSeq_create(toUINT(shape.size()), 2);
    for (unsigned int i = 0; i < shape.size(); i++) {
        GEOSCoordSeq_setXY(coordSeq, i, shape[i].x(), shape[i].y());
    }
    GEOSGeometry* linearRing = GEOSGeom_createLinearRing(coordSeq);
    if (GEOSisSimple(linearRing)) {
        return GEOSGeom_createPolygon(GEOSGeom_clone(linearRing), nullptr, 0);
    }
    else {
        return nullptr;
    } 
}


GEOSGeometry*
MSPModel_JuPedSim::createGeometryFromAnchors(const Position& anchor, const MSLane* const lane, const Position& otherAnchor, const MSLane* const otherLane) {
    GEOSGeometry* geometry;
    if (lane->getWidth() == otherLane->getWidth()) {
        PositionVector anchors = { anchor, otherAnchor };
        geometry = createGeometryFromCenterLine(anchors, lane->getWidth() / 2.0, GEOSBUF_CAP_ROUND);
    }
    else {
        GEOSGeometry* anchorPoint = GEOSGeom_createPointFromXY(anchor.x(), anchor.y());
        GEOSGeometry* dilatedAnchorPoint = GEOSBufferWithStyle(anchorPoint, lane->getWidth() / 2.0, 
            GEOS_QUADRANT_SEGMENTS, GEOSBUF_CAP_ROUND, GEOSBUF_JOIN_ROUND , GEOS_MITRE_LIMIT);
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
    std::vector<GEOSGeometry*> dilatedPedestrianLanes;
    for (const std::pair<std::string, MSJunction*>& junctionWithID : network->getJunctionControl()) {
        const MSJunction* const junction = junctionWithID.second;
        const ConstMSEdgeVector& incoming = junction->getIncoming();
        std::unordered_set<const MSEdge*> adjacent(incoming.begin(), incoming.end());
        const ConstMSEdgeVector& outgoing = junction->getOutgoing();
        adjacent.insert(outgoing.begin(), outgoing.end());

        for (const MSEdge* const edge : adjacent) {
            if (!edge->isWalkingArea()) {
                const MSLane* const lane = getSidewalk<MSEdge, MSLane>(edge);
                if (lane != nullptr) {
                    GEOSGeometry* dilatedLane = createGeometryFromCenterLine(lane->getShape(), lane->getWidth() / 2.0, GEOSBUF_CAP_ROUND);
                    dilatedPedestrianLanes.push_back(dilatedLane);
                    for (const MSEdge* const nextEdge : adjacent) {
                        if (nextEdge != edge) {
                            const MSEdge* walkingArea = getWalkingAreaInbetween(edge, nextEdge);
                            if (walkingArea) {
                                MSEdgeVector walkingAreaIncoming = walkingArea->getPredecessors();
                                const MSLane* const nextLane = getSidewalk<MSEdge, MSLane>(nextEdge);
                                if (nextLane != nullptr) {
                                    GEOSGeometry* walkingAreaGeom;
                                    Position anchor;
                                    Position nextAnchor;
                                    
                                    if (edge->isNormal() && nextEdge->isNormal()) {
                                        PositionVector walkingAreaShape = getSidewalk<MSEdge, MSLane>(walkingArea)->getShape();
                                        walkingAreaGeom = createGeometryFromShape(walkingAreaShape);
                                        if (walkingAreaGeom) {
                                            dilatedPedestrianLanes.push_back(walkingAreaGeom);
                                            continue;
                                        }
                                        else {
                                            anchor = getAnchor(lane, edge, walkingAreaIncoming);
                                            nextAnchor = getAnchor(nextLane, nextEdge, walkingAreaIncoming);
                                        }
                                    }
                                    else if ((edge->isNormal() && nextEdge->isCrossing()) || (edge->isCrossing() && nextEdge->isNormal())) {
                                        MSEdgeVector walkingAreaEdges = edge->isCrossing() ? walkingAreaIncoming : walkingArea->getSuccessors();
                                        if (std::none_of(walkingAreaEdges.begin(), walkingAreaEdges.end(), [](MSEdge* e){ return e->isNormal(); })) {
                                            anchor = getAnchor(lane, edge, walkingAreaIncoming);
                                            nextAnchor = getAnchor(nextLane, nextEdge, walkingAreaIncoming);
                                        }                                        
                                    }
                                    else if (edge->isCrossing() && nextEdge->isCrossing())
                                    {
                                        anchor = getAnchor(lane, edge, walkingAreaIncoming);
                                        nextAnchor = getAnchor(nextLane, nextEdge, walkingAreaIncoming);
                                    }
                                    else {
                                        continue;
                                    }

                                    walkingAreaGeom = createGeometryFromAnchors(anchor, lane, nextAnchor, nextLane);
                                    dilatedPedestrianLanes.push_back(walkingAreaGeom);
                                }
                            }
                        }
                    }
                }  
            }
        }
    }

    GEOSGeometry* disjointDilatedPedestrianLanes = GEOSGeom_createCollection(GEOS_MULTIPOLYGON, dilatedPedestrianLanes.data(), toUINT(dilatedPedestrianLanes.size()));
    GEOSGeometry* pedestrianNetwork = GEOSUnaryUnion(disjointDilatedPedestrianLanes);
    GEOSGeom_destroy(disjointDilatedPedestrianLanes);
    return pedestrianNetwork;
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
    // Remove the last point so that CGAL doesn't complain of the simplicity of the polygon downstream.
    for (unsigned int i = 0; i < toUINT(coordinates.size()) - 1; i++) {
        Position c = coordinates[i];
        pointVector.push_back({c.x(), c.y()});
    }
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
    // Remove the last point so that CGAL doesn't complain of the simplicity of the polygon downstream.
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
MSPModel_JuPedSim::renderPolygon(const GEOSGeometry* polygon, const std::string& polygonId) {
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
        shapeContainer.addPolygon(polygonId, std::string("pedestrian_network"), RGBColor(255, 0, 0, 255), 10.0, 0.0, std::string(), false, shape, false, true, 1.0);
        shapeContainer.getPolygons().get(polygonId)->setHoles(holes);
    }
}


void 
MSPModel_JuPedSim::preparePolygonForJPS(const GEOSGeometry* polygon, const std::string& polygonId) {
    std::ofstream dumpFile;
    dumpFile.open(polygonId + std::string(".txt"));
    int maxPrecision = std::numeric_limits<double>::max_digits10 + 2;

    // Handle the exterior polygon.
    const GEOSGeometry* exterior =  GEOSGetExteriorRing(polygon);
    std::vector<JPS_Point> exteriorCoordinates = convertToJPSPoints(exterior);
    JPS_GeometryBuilder_AddAccessibleArea(myJPSGeometryBuilder, exteriorCoordinates.data(), exteriorCoordinates.size());

    for (const auto& c : exteriorCoordinates) {
        dumpFile << std::setprecision(maxPrecision) << c.x << std::endl;
        dumpFile << std::setprecision(maxPrecision) << c.y << std::endl;
    }
    dumpFile << std::endl;

    // Handle the interior polygons (holes).
    int nbrInteriorRings = GEOSGetNumInteriorRings(polygon);
    if (nbrInteriorRings != -1) {
        for (unsigned int k = 0; k < (unsigned int)nbrInteriorRings; k++) {
            const GEOSGeometry* linearRing = GEOSGetInteriorRingN(polygon, k);
            double area = getHoleArea(linearRing);
            if (area > GEOS_MIN_AREA) {
                std::vector<JPS_Point> holeCoordinates = convertToJPSPoints(linearRing);
                JPS_GeometryBuilder_ExcludeFromAccessibleArea(myJPSGeometryBuilder, holeCoordinates.data(), holeCoordinates.size());

                for (const auto& c : holeCoordinates) {
                    dumpFile << std::setprecision(maxPrecision) << c.x << std::endl;
                    dumpFile << std::setprecision(maxPrecision) << c.y << std::endl;
                }
                dumpFile << std::endl;
            }
        }
    }

    dumpFile.close();
}


void MSPModel_JuPedSim::prepareAdditionalPolygonsForJPS(void) {
    for (auto shape: myNetwork->getShapeContainer().getPolygons()) {
        PositionVector translatedShape = shape.second->getShape();
        Position netOffset = GeoConvHelper::getFinal()::getOffset();
        translatedShape.add(netOffset);
        std::vector<JPS_Point> coordinates = convertToJPSPoints(translatedShape);
        if (shape.second->getShapeType() == "jupedsim.walkable_area") {
            JPS_GeometryBuilder_AddAccessibleArea(myJPSGeometryBuilder, coordinates.data(), coordinates.size());
        }
        else if (shape.second->getShapeType() == "jupedsim.obstacle") {
            JPS_GeometryBuilder_ExcludeFromAccessibleArea(myJPSGeometryBuilder, coordinates.data(), coordinates.size());
        }
        else {
            continue;
        }
    }
}


static void
geos_msg_handler(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vprintf (fmt, ap);
    va_end(ap);
}


void
MSPModel_JuPedSim::initialize() {
    initGEOS(geos_msg_handler, geos_msg_handler);
    myGEOSPedestrianNetwork = buildPedestrianNetwork(myNetwork);
    myIsPedestrianNetworkConnected = GEOSGetNumGeometries(myGEOSPedestrianNetwork) == 1 ? true : false; 

    // myJPSGeometryBuilder = JPS_GeometryBuilder_Create();
    // for (size_t i = 0; i < GEOSGetNumGeometries(myGEOSPedestrianNetwork); i++) {
    //     const GEOSGeometry* connectedComponentPolygon = GEOSGetGeometryN(myGEOSPedestrianNetwork, i);
    //     std::string polygonId = std::string("pedestrian_network_connected_component_") + std::to_string(i);
    //     renderPolygon(connectedComponentPolygon, polygonId);
    //     preparePolygonForJPS(connectedComponentPolygon, polygonId);
    // }
    // prepareAdditionalPolygonsForJPS();
    
    // For the moment, JuPedSim only supports one connected component.
    const GEOSGeometry* maxAreaConnectedComponentPolygon = nullptr;
    std::string maxAreaPolygonId;
    double maxArea = 0.0;
    for (unsigned int i = 0; i < (unsigned int)GEOSGetNumGeometries(myGEOSPedestrianNetwork); i++) {
        const GEOSGeometry* connectedComponentPolygon = GEOSGetGeometryN(myGEOSPedestrianNetwork, i);
        std::string polygonId = std::string("pedestrian_network_connected_component_") + std::to_string(i);
        double area;
        GEOSArea(connectedComponentPolygon, &area);
        if (area > maxArea) {
            maxArea = area;
            maxAreaConnectedComponentPolygon = connectedComponentPolygon;
            maxAreaPolygonId = polygonId;
        }
    }
    renderPolygon(maxAreaConnectedComponentPolygon, maxAreaPolygonId);
    myJPSGeometryBuilder = JPS_GeometryBuilder_Create();
    preparePolygonForJPS(maxAreaConnectedComponentPolygon, maxAreaPolygonId);
    prepareAdditionalPolygonsForJPS();

    std::ofstream GEOSGeometryDumpFile;
    GEOSGeometryDumpFile.open("pedestrianNetwork.wkt");
    GEOSWKTWriter* writer = GEOSWKTWriter_create();
    char* wkt = GEOSWKTWriter_write(writer, maxAreaConnectedComponentPolygon); // Change to myGEOSPedestrianNetwork when multiple components have been implemented.
    GEOSGeometryDumpFile << wkt << std::endl;
    GEOSGeometryDumpFile.close();
    GEOSFree(wkt);
    GEOSWKTWriter_destroy(writer);

    JPS_ErrorMessage message = nullptr; 
    
    myJPSGeometry = JPS_GeometryBuilder_Build(myJPSGeometryBuilder, &message);
    if (myJPSGeometry == nullptr) {
        WRITE_WARNINGF(TL("Error creating the geometry: %"), JPS_ErrorMessage_GetMessage(message));
    }

    myJPSModelBuilder = JPS_VelocityModelBuilder_Create(8.0, 0.1, 5.0, 0.02);
    size_t nbrParameterProfiles = 0;
    for (const MSVehicleType* type : myNetwork->getVehicleControl().getPedestrianTypes()) {
        ++nbrParameterProfiles;
        double radius;
        if ((!type->wasSet(VTYPEPARS_LENGTH_SET)) && (!type->wasSet(VTYPEPARS_WIDTH_SET))) {
            radius = 0.3;
        }
        else if (!type->wasSet(VTYPEPARS_WIDTH_SET)) {
            radius = 0.5 * type->getLength();
        }
        else if (!type->wasSet(VTYPEPARS_LENGTH_SET)) {
            radius = 0.5 * type->getWidth();
        }
        else {
            radius = 0.25 * (type->getLength() + type->getWidth());
        }
        JPS_VelocityModelBuilder_AddParameterProfile(myJPSModelBuilder, nbrParameterProfiles, 1.0, 0.5, MIN2(type->getMaxSpeed(), type->getDesiredMaxSpeed()), radius);
        myJPSParameterProfileIds[type->getID()] = nbrParameterProfiles;
    }
    
    myJPSModel = JPS_VelocityModelBuilder_Build(myJPSModelBuilder, &message);
    if (myJPSModel == nullptr) {
        WRITE_WARNINGF(TL("Error creating the pedestrian model: %"), JPS_ErrorMessage_GetMessage(message));
    }

    if ((myJPSModel != nullptr) && (myJPSGeometry != nullptr)) {
        myJPSSimulation = JPS_Simulation_Create(myJPSModel, myJPSGeometry, STEPS2TIME(myJPSDeltaT), &message);
        if (myJPSSimulation == nullptr) {
            WRITE_WARNINGF(TL("Error creating the simulation: %"), JPS_ErrorMessage_GetMessage(message));
        }
    }

    JPS_ErrorMessage_Free(message);
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
MSPModel_JuPedSim::PState::PState(MSPerson* person, MSStageMoving* stage, JPS_JourneyDescription journey, JPS_JourneyId journeyId, Position destination)
    : myPerson(person), myStage(stage), myJourney(journey), myJourneyId(journeyId), myDestination(destination), myAgentId(0), myPosition(0, 0), myAngle(0), myWaitingToEnter(true) {
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


Position MSPModel_JuPedSim::PState::getDestination() const {
    return myDestination;
}


JPS_AgentId MSPModel_JuPedSim::PState::getAgentId() const {
    return myAgentId;
}
