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
#include <fstream>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/operation/buffer/BufferOp.h>
#include <geos/io/WKTWriter.h>
#include <jupedsim/jupedsim.h>
#include "microsim/MSEdge.h"
#include "microsim/MSLane.h"
#include "microsim/MSLink.h"
#include "microsim/MSEdgeControl.h"
#include "microsim/MSJunctionControl.h"
#include "microsim/MSEventControl.h"
#include "libsumo/Person.h"
#include "utils/geom/Position.h"
#include "utils/geom/PositionVector.h"
#include "utils/shapes/ShapeContainer.h"
#include "MSPModel_JuPedSim.h"
#include "MSPerson.h"


const int MSPModel_JuPedSim::GEOS_QUADRANT_SEGMENTS = 16;
const double MSPModel_JuPedSim::GEOS_MIN_AREA = 10;
const SUMOTime MSPModel_JuPedSim::JPS_DELTA_T = 10;
const double MSPModel_JuPedSim::JPS_EXIT_TOLERANCE = 1;


MSPModel_JuPedSim::MSPModel_JuPedSim(const OptionsCont& oc, MSNet* net) : myNetwork(net) {
    initialize();
    Event* e = new Event(this);
    net->getBeginOfTimestepEvents()->addEvent(e, net->getCurrentTimeStep() + DELTA_T);
}


MSPModel_JuPedSim::~MSPModel_JuPedSim() {
    clearState();

    JPS_Simulation_Free(myJPSSimulation);
    JPS_OperationalModel_Free(myJPSModel);
    JPS_Geometry_Free(myJPSGeometry);
    JPS_GeometryBuilder_Free(myJPSGeometryBuilder);

    myGEOSGeometryFactory->destroyGeometry(myGEOSPedestrianNetwork);
    for (geos::geom::Geometry* geometry : myGEOSConvertedLinearRingsDump) {
        myGEOSGeometryFactory->destroyGeometry(geometry);
    }
    for (geos::geom::Geometry* geometry : myGEOSConvexHullsDump) {
        myGEOSGeometryFactory->destroyGeometry(geometry);
    }
    for (geos::geom::Geometry* geometry : myGEOSGeometryCollectionsDump) {
        myGEOSGeometryFactory->destroyGeometry(geometry);
    }
    for (geos::geom::Geometry* geometry : myGEOSBufferedGeometriesDump) {
        myGEOSGeometryFactory->destroyGeometry(geometry);
    }
    for (geos::geom::Geometry* geometry : myGEOSPointsDump) {
        myGEOSGeometryFactory->destroyGeometry(geometry);
    }
    for (geos::geom::Geometry* geometry : myGEOSLineStringsDump) {
        myGEOSGeometryFactory->destroyGeometry(geometry);
    }
}


MSTransportableStateAdapter*
MSPModel_JuPedSim::add(MSTransportable* person, MSStageMoving* stage, SUMOTime now) {
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

	JPS_Journey journey = JPS_Journey_Create();
    JPS_Journey_AddWaypoint(journey, {arrivalPosition.x(), arrivalPosition.y()}, JPS_EXIT_TOLERANCE);
    JPS_JourneyId journeyId = JPS_Simulation_AddJourney(myJPSSimulation, journey, nullptr);
    JPS_Journey_Free(journey);

	JPS_VelocityModelAgentParameters agent_parameters{};
	agent_parameters.journeyId = journeyId;
	agent_parameters.orientation = {1.0, 0.0};
	agent_parameters.position = {departurePosition.x(), departurePosition.y()};
    agent_parameters.profileId = myJPSParameterProfileId;

    JPS_AgentId agentId = JPS_Simulation_AddVelocityModelAgent(myJPSSimulation, agent_parameters, nullptr);
    PState* state = new PState(static_cast<MSPerson*>(person), stage, journey, arrivalPosition, agentId);
	myPedestrianStates.push_back(state);
    myNumActivePedestrians++;
		
    return state;
}


void
MSPModel_JuPedSim::remove(MSTransportableStateAdapter* state) {
    // This function is called only when using TraCI.
    // Not sure what to do here.
}


SUMOTime
MSPModel_JuPedSim::execute(SUMOTime time) {
    int nbrIterations = (int)(DELTA_T / JPS_DELTA_T);
    JPS_ErrorMessage message = nullptr;
	for (int i = 0; i < nbrIterations; ++i)
	{
        // Perform one JuPedSim iteration.
		bool ok = JPS_Simulation_Iterate(myJPSSimulation, &message);
        if (!ok) {
            std::ostringstream oss;
            oss << "Error during iteration " << i << ": " << JPS_ErrorMessage_GetMessage(message);
            WRITE_ERROR(oss.str());
        }

        // Update the state of all pedestrians.
        for (PState* state : myPedestrianStates)
        {
            // Updates the agent position.
            JPS_VelocityModelAgentParameters agent{}; 
            JPS_Simulation_ReadVelocityModelAgent(myJPSSimulation, state->getAgentId(), &agent, nullptr);
            state->setPosition(agent.position.x, agent.position.y);

            // Updates the agent direction.
            state->setAngle(atan2(agent.orientation.x, agent.orientation.y));

            Position newPosition(agent.position.x, agent.position.y);
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
                libsumo::Person::moveToXY(person->getID(), currentEdge->getID(), agent.position.x, agent.position.y, libsumo::INVALID_DOUBLE_VALUE, 2);
            }
            
            // If near the last waypoint, remove the agent.
            if (newPosition.distanceTo2D(state->getDestination()) < JPS_EXIT_TOLERANCE) {
                JPS_Simulation_RemoveAgent(myJPSSimulation, state->getAgentId(), nullptr);
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


MSLane* 
MSPModel_JuPedSim::getPedestrianLane(MSEdge* edge) {
    for (MSLane* lane : edge->getLanes()) {
        SVCPermissions permissions = lane->getPermissions();
        if (permissions == SVC_PEDESTRIAN) {
            return lane;
        }
    }

    return nullptr;
}


Position 
MSPModel_JuPedSim::getAnchor(MSLane* lane, MSEdge* edge, ConstMSEdgeVector incoming) {
    if (std::count(incoming.begin(), incoming.end(), edge)) {
        return lane->getShape().back();
    }

    return lane->getShape().front();
}


Position
MSPModel_JuPedSim::getAnchor(MSLane* lane, MSEdge* edge, MSEdgeVector incoming) {
    if (std::count(incoming.begin(), incoming.end(), edge)) {
        return lane->getShape().back();
    }

    return lane->getShape().front();
}


std::tuple<ConstMSEdgeVector, ConstMSEdgeVector, std::unordered_set<MSEdge*>> 
MSPModel_JuPedSim::getAdjacentEdgesOfJunction(MSJunction* junction) {
    ConstMSEdgeVector incoming = junction->getIncoming();
    ConstMSEdgeVector outgoing = junction->getOutgoing();
    ConstMSEdgeVector adjacentVector = incoming;
    adjacentVector.insert(adjacentVector.end(), outgoing.begin(), outgoing.end());
    
    std::unordered_set<MSEdge*> adjacentSet;
    for (const MSEdge* edge : adjacentVector) {
        adjacentSet.insert(const_cast<MSEdge*>(edge));
    }

    return std::make_tuple(incoming, outgoing, adjacentSet);
}


const MSEdgeVector
MSPModel_JuPedSim::getAdjacentEdgesOfEdge(MSEdge* edge) {
    const MSEdgeVector& outgoing = edge->getSuccessors();
    MSEdgeVector adjacent = edge->getPredecessors();
    adjacent.insert(adjacent.end(), outgoing.begin(), outgoing.end());

    return adjacent;
}


bool 
MSPModel_JuPedSim::hasWalkingAreasInbetween(MSEdge* edge, MSEdge* otherEdge, ConstMSEdgeVector adjacentEdgesOfJunction) {
    for (const MSEdge* nextEdge : getAdjacentEdgesOfEdge(edge)) {
        if ((nextEdge->getFunction() == SumoXMLEdgeFunc::WALKINGAREA) && 
            (std::count(adjacentEdgesOfJunction.begin(), adjacentEdgesOfJunction.end(), edge))) {
            
            MSEdgeVector walkingAreOutgoing = nextEdge->getSuccessors();
            if (std::count(walkingAreOutgoing.begin(), walkingAreOutgoing.end(), otherEdge)) {
                return true;
            }
        }
    }

    return false;
}


geos::geom::Geometry*
MSPModel_JuPedSim::createShapeFromCenterLine(PositionVector centerLine, double width, int capStyle) {
    geos::geom::CoordinateArraySequence* coordinateArraySequence = new geos::geom::CoordinateArraySequence();
    for (Position p : centerLine) {
        coordinateArraySequence->add(geos::geom::Coordinate(p.x(), p.y()));
    }

    // In the new C++ API, the parent class CoordinateSequence has an add method and this conversion won't
    // be necessary anymore. The underlying sequence will be destroyed at the end of scope.
    std::unique_ptr<geos::geom::CoordinateSequence> coordinateSequence(coordinateArraySequence);

    // Create a line string. The line string will hold the coordinates because of the move semantics.
    // Need to release the above unique pointer because it would be destroyed at the end of the scope otherwise,
    // but the dilated (buffered) line string holds a pointer to it unfortunately, and a clone of the buffer 
    // doesn't seem to perform a deep copy as advertised.
    geos::geom::Geometry* lineString = myGEOSGeometryFactory->createLineString(std::move(coordinateSequence)).release();
    
    // Keep the pointer in store for ulterior destruction. Keeping the unique pointer isn't possible because it 
    // would require the use of move semantics, which would then invalidate the code inside buildPedestrianNetwork.
    myGEOSLineStringsDump.push_back(lineString);

    geos::geom::Geometry* dilatedLineString = lineString->buffer(width, GEOS_QUADRANT_SEGMENTS, capStyle).release();
    myGEOSBufferedGeometriesDump.push_back(dilatedLineString); 
    return dilatedLineString;
}


geos::geom::Geometry*
MSPModel_JuPedSim::createShapeFromAnchors(Position anchor, MSLane* lane, Position otherAnchor, MSLane* otherLane) {
    geos::geom::Geometry* shape;
    if (lane->getWidth() == otherLane->getWidth()) {
        PositionVector anchors = { anchor, otherAnchor };
        shape = createShapeFromCenterLine(anchors, lane->getWidth() / 2.0, geos::operation::buffer::BufferOp::CAP_ROUND);
    }
    else {
        geos::geom::Point* anchorPoint = myGEOSGeometryFactory->createPoint(geos::geom::Coordinate(anchor.x(), anchor.y()));
        myGEOSPointsDump.push_back(anchorPoint);
        geos::geom::Geometry* dilatedAnchorPoint = anchorPoint->buffer(lane->getWidth() / 2.0, GEOS_QUADRANT_SEGMENTS, geos::operation::buffer::BufferOp::CAP_ROUND).release();
        myGEOSBufferedGeometriesDump.push_back(dilatedAnchorPoint);

        geos::geom::Point* otherAnchorPoint = myGEOSGeometryFactory->createPoint(geos::geom::Coordinate(otherAnchor.x(), otherAnchor.y()));
        myGEOSPointsDump.push_back(otherAnchorPoint);
        geos::geom::Geometry* dilatedOtherAnchorPoint = otherAnchorPoint->buffer(otherLane->getWidth() / 2.0, GEOS_QUADRANT_SEGMENTS, geos::operation::buffer::BufferOp::CAP_ROUND).release();
        myGEOSBufferedGeometriesDump.push_back(dilatedOtherAnchorPoint);

        std::vector<const geos::geom::Geometry*> polygons = { dilatedAnchorPoint, dilatedOtherAnchorPoint };
        geos::geom::Geometry* multiPolygon = myGEOSGeometryFactory->createGeometryCollection(polygons);
        myGEOSGeometryCollectionsDump.push_back(multiPolygon);
        shape = multiPolygon->convexHull().release();
        myGEOSConvexHullsDump.push_back(shape);
    }

    return shape;
}


geos::geom::Geometry*
MSPModel_JuPedSim::buildPedestrianNetwork(MSNet* network) {
    std::vector<const geos::geom::Geometry*> dilatedPedestrianLanes;
    for (const auto& junctionWithID : network->getJunctionControl()) {
        MSJunction* junction = junctionWithID.second;
        ConstMSEdgeVector incoming;
        ConstMSEdgeVector outgoing;
        std::unordered_set<MSEdge*> adjacent;
        std::tie(incoming, outgoing, adjacent) = getAdjacentEdgesOfJunction(junction);

        for (MSEdge* edge : adjacent) {
            if (edge->getFunction() == SumoXMLEdgeFunc::NORMAL) {
                MSLane* lane = getPedestrianLane(edge);
                if (lane) {
                    Position anchor = getAnchor(lane, edge, incoming);
                    geos::geom::Geometry* dilatedLaneLine = createShapeFromCenterLine(lane->getShape(), lane->getWidth() / 2.0, geos::operation::buffer::BufferOp::CAP_BUTT);
                    dilatedPedestrianLanes.push_back(dilatedLaneLine);

                    for (MSEdge* nextEdge : adjacent) {
                        if ((nextEdge != edge) && (nextEdge->getFunction() == SumoXMLEdgeFunc::NORMAL)) {
                            if (hasWalkingAreasInbetween(edge, nextEdge, ConstMSEdgeVector(adjacent.begin(), adjacent.end()))) {
                                MSLane* nextLane = getPedestrianLane(nextEdge);
                                if (nextLane) {
                                    Position nextAnchor = getAnchor(nextLane, nextEdge, incoming);
                                    geos::geom::Geometry* dilatedLaneLine = createShapeFromAnchors(anchor, lane, nextAnchor, nextLane);
                                    dilatedPedestrianLanes.push_back(dilatedLaneLine);
                                }
                            }
                        }
                    }
                }
            }
        }

        for (MSEdge* edge : adjacent) {
            if (edge->getFunction() == SumoXMLEdgeFunc::CROSSING) {
                MSLane* lane = getPedestrianLane(edge);
                if (lane) {
                    geos::geom::Geometry* dilatedCrossingLane = createShapeFromCenterLine(lane->getShape(), lane->getWidth() / 2.0, geos::operation::buffer::BufferOp::CAP_BUTT);
                    dilatedPedestrianLanes.push_back(dilatedCrossingLane);

                    for (MSEdge* nextEdge : getAdjacentEdgesOfEdge(edge)) {
                        if ((nextEdge->getFunction() == SumoXMLEdgeFunc::WALKINGAREA) && (std::count(adjacent.begin(), adjacent.end(), nextEdge))) {
                            MSEdgeVector walkingAreaAdjacent = getAdjacentEdgesOfEdge(nextEdge);
                            for (MSEdge* nextNextEdge : walkingAreaAdjacent) {
                                if (nextNextEdge != edge) {
                                    MSLane* nextLane = getPedestrianLane(nextNextEdge);
                                    if (nextLane) {
                                        MSEdgeVector nextEdgeIncoming = nextEdge->getPredecessors();
                                        Position anchor = getAnchor(lane, edge, nextEdgeIncoming);
                                        Position nextAnchor = getAnchor(nextLane, nextNextEdge, nextEdgeIncoming);
                                        geos::geom::Geometry* dilatedLaneLine = createShapeFromAnchors(anchor, lane, nextAnchor, nextLane);
                                        dilatedPedestrianLanes.push_back(dilatedLaneLine);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    geos::geom::Geometry* disjointDilatedPedestrianLanes = myGEOSGeometryFactory->createGeometryCollection(dilatedPedestrianLanes);
    myGEOSGeometryCollectionsDump.push_back(disjointDilatedPedestrianLanes);
    geos::geom::Geometry* myGEOSPedestrianNetwork = disjointDilatedPedestrianLanes->Union().release();
    return myGEOSPedestrianNetwork;
}


PositionVector 
MSPModel_JuPedSim::getCoordinates(const geos::geom::Geometry* geometry) {
    PositionVector coordinates;
    std::unique_ptr<geos::geom::CoordinateSequence> coordsSeq = geometry->getCoordinates();
    for (size_t i = 0; i < coordsSeq->getSize(); i++) {
        geos::geom::Coordinate c = coordsSeq->getAt(i);
        coordinates.push_back(Position(c.x, c.y));
    }
    return coordinates;
}


std::vector<double> 
MSPModel_JuPedSim::getFlattenedCoordinates(const geos::geom::Geometry* geometry) {
    std::vector<double> flattenedCoordinates;
    std::unique_ptr<geos::geom::CoordinateSequence> coordsSeq = geometry->getCoordinates();
    // Remove the last point so that CGAL doesn't complain of the simplicity of the polygon downstream.
    for (size_t i = 0; i < coordsSeq->getSize() - 1; i++) {
        geos::geom::Coordinate c = coordsSeq->getAt(i);
        flattenedCoordinates.push_back(c.x);
        flattenedCoordinates.push_back(c.y);
    }
    return flattenedCoordinates;
}


geos::geom::Polygon* 
MSPModel_JuPedSim::toPolygon(const geos::geom::LinearRing* linearRing) {
    geos::geom::Polygon* polygon = myGEOSGeometryFactory->createPolygon(*linearRing, {});
    myGEOSConvertedLinearRingsDump.push_back(polygon);
    return polygon;
}


void 
MSPModel_JuPedSim::renderPolygon(const geos::geom::Polygon* polygon, const std::string& polygonId) {
    const geos::geom::LinearRing* exterior = polygon->getExteriorRing();
    PositionVector shape = getCoordinates(exterior);
    
    std::vector<PositionVector> holes;
    for (size_t k = 0; k < polygon->getNumInteriorRing(); k++) {
        const geos::geom::LinearRing* interior = polygon->getInteriorRingN(k);
        if (toPolygon(interior)->getArea() > GEOS_MIN_AREA) {
            PositionVector hole = getCoordinates(interior);
            holes.push_back(hole);
        }
    }

    ShapeContainer& shapeContainer = myNetwork->getShapeContainer();
    shapeContainer.addPolygon(polygonId, std::string("pedestrian_network"), RGBColor(255, 0, 0, 255), 10.0, 0.0, std::string(), false, shape, false, true, 1.0);
    shapeContainer.getPolygons().get(polygonId)->setHoles(holes);
}


void 
MSPModel_JuPedSim::preparePolygonForJPS(const geos::geom::Polygon* polygon, const std::string& polygonId) {
    std::ofstream dumpFile;
    dumpFile.open(polygonId + std::string(".txt"));
    int maxPrecision = std::numeric_limits<double>::max_digits10 + 2;

    // Handle the exterior polygon.
    const geos::geom::LinearRing* exterior = polygon->getExteriorRing();
    std::vector<double> exteriorCoordinates = getFlattenedCoordinates(exterior);
    JPS_GeometryBuilder_AddAccessibleArea(myJPSGeometryBuilder, exteriorCoordinates.data(), exteriorCoordinates.size() / 2);

    for (double c : exteriorCoordinates) {
        dumpFile << std::setprecision(maxPrecision) << c << std::endl;
    }
    dumpFile << std::endl;

    // Handle the interior polygons (holes).
    for (size_t k = 0; k < polygon->getNumInteriorRing(); k++) {
        const geos::geom::LinearRing* interior = polygon->getInteriorRingN(k);
        if (toPolygon(interior)->getArea() > GEOS_MIN_AREA) {
            std::vector<double> holeCoordinates = getFlattenedCoordinates(interior);
            JPS_GeometryBuilder_ExcludeFromAccessibleArea(myJPSGeometryBuilder, holeCoordinates.data(), holeCoordinates.size() / 2);

            for (double c : holeCoordinates) {
                dumpFile << std::setprecision(maxPrecision) << c << std::endl;
            }
            dumpFile << std::endl;
        }
    }

    dumpFile.close();
}


void
MSPModel_JuPedSim::initialize() {
    myGEOSGeometryFactory = geos::geom::GeometryFactory::create();
    myGEOSPedestrianNetwork = buildPedestrianNetwork(myNetwork);
    myIsPedestrianNetworkConnected = !myGEOSPedestrianNetwork->isCollection();

    std::ofstream GEOSGeometryDumpFile;
    GEOSGeometryDumpFile.open("pedestrianNetwork.wkt");
    geos::io::WKTWriter writer;
    std::string wkt = writer.write(myGEOSPedestrianNetwork);
    GEOSGeometryDumpFile << wkt << std::endl;
    GEOSGeometryDumpFile.close();

    myJPSGeometryBuilder = JPS_GeometryBuilder_Create();
    for (size_t i = 0; i < myGEOSPedestrianNetwork->getNumGeometries(); i++) {
        const geos::geom::Polygon* connectedComponentPolygon = dynamic_cast<const geos::geom::Polygon*>(myGEOSPedestrianNetwork->getGeometryN(i));
        std::string polygonId = std::string("pedestrian_network_connected_component_") + std::to_string(i);
        renderPolygon(connectedComponentPolygon, polygonId);
        preparePolygonForJPS(connectedComponentPolygon, polygonId);
    }

//	for (const MSEdge* const edge : (myNetwork->getEdgeControl()).getEdges()) {
//        const MSLane* lane = getSidewalk<MSEdge, MSLane>(edge);
//        if (lane) {
//            PositionVector shape = lane->getShape();
//
//            // Apparently CGAL expects polygons to be oriented CCW.
//            if (shape.isClockwiseOriented()) {
//                shape = shape.reverse();
//            }
//            assert(!shape.isClockwiseOriented());
//
//            /* The code below is in theory more robust as there would be a guarantee that
//               the shape is CCW-oriented. However at the moment the sort algorithm doesn't
//               work for non-convex polygons.
//               PositionVector shape = lane->getShape();
//               shape.sortAsPolyCWByAngle();
//               shape = shape.reverse();
//               assert(!shape.isClockwiseOriented());
//            */
//            
//            std::vector<double> lanePolygonCoordinatesFlattened;
//
//            if (edge->isWalkingArea()) {
//                if (shape.area() == 0.0) {
//                    continue;
//                }
//
//                auto last = shape.isClosed() ? shape.end()-1 : shape.end();
//                for (auto position = shape.begin(); position != last; position++) {
//                    lanePolygonCoordinatesFlattened.push_back(position->x());
//                    lanePolygonCoordinatesFlattened.push_back(position->y());
//                } 
//
//                /*std::vector<std::pair<double, double>> lanePolygonCoordinates;
//                for (const Position& position : shape)
//                    lanePolygonCoordinates.push_back(std::make_pair<double, double>(position.x(), position.y()));
//
//                auto end = lanePolygonCoordinates.end();
//                for (auto it = lanePolygonCoordinates.begin(); it != end; ++it) {
//                    end = std::remove(it + 1, end, *it);
//                }
//
//                lanePolygonCoordinates.erase(end, lanePolygonCoordinates.end());
//
//                for (auto position = lanePolygonCoordinates.begin(); position != lanePolygonCoordinates.end(); position++) {
//                    lanePolygonCoordinatesFlattened.push_back(position->first);
//                    lanePolygonCoordinatesFlattened.push_back(position->second);
//                }*/
//            }
//            else {
//                double amount = lane->getWidth() / 2.0;
//                shape.move2side(amount);
//                Position bottomFirstCorner = shape[0];
//                Position bottomSecondCorner = shape[1];
//                shape = lane->getShape();
//                shape.move2side(-amount);
//                Position topFirstCorner = shape[0];
//                Position topSecondCorner = shape[1];
//
//                std::vector<Position> lanePolygon{ topFirstCorner, bottomFirstCorner, bottomSecondCorner, topSecondCorner };
//                for (const Position& position : lanePolygon) {
//                    lanePolygonCoordinatesFlattened.push_back(position.x());
//                    lanePolygonCoordinatesFlattened.push_back(position.y());
//                }
//            }
//            
//#ifdef DEBUG
//            geometryDumpFile << "Lane " <<  lane->getID() << std::endl;
//            for (double coordinate: lanePolygonCoordinatesFlattened) {
//                geometryDumpFile << coordinate << std::endl;
//            }
//#endif

 //           JPS_GeometryBuilder_AddAccessibleArea(myGeometryBuilder, lanePolygonCoordinatesFlattened.data(), lanePolygonCoordinatesFlattened.size() / 2);
 //       }
	//}

    JPS_ErrorMessage message = nullptr; 
    
    myJPSGeometry = JPS_GeometryBuilder_Build(myJPSGeometryBuilder, &message);
    if (myJPSGeometry == nullptr) {
        std::ostringstream oss;
        oss << "Error while creating the geometry: " << JPS_ErrorMessage_GetMessage(message);
        WRITE_ERROR(oss.str());
    }


    JPS_VelocityModelBuilder modelBuilder = JPS_VelocityModelBuilder_Create(8.0, 0.1, 5.0, 0.02);
    myJPSParameterProfileId = 1;
    double initial_speed = 1.0; // stage->getMaxSpeed(person);
    double pedestrian_radius = 0.5; // 1.0 yields bad pedestrian behavior...
    JPS_VelocityModelBuilder_AddParameterProfile(modelBuilder, myJPSParameterProfileId, 1.0, 0.5, initial_speed, pedestrian_radius);
    myJPSModel = JPS_VelocityModelBuilder_Build(modelBuilder, &message);
    if (myJPSModel == nullptr) {
        std::ostringstream oss;
        oss << "Error while creating the pedestrian model: " << JPS_ErrorMessage_GetMessage(message);
        WRITE_ERROR(oss.str());
    }

	myJPSSimulation = JPS_Simulation_Create(myJPSModel, myJPSGeometry, STEPS2TIME(JPS_DELTA_T), &message);
    if (myJPSSimulation == nullptr) {
        std::ostringstream oss;
        oss << "Error while creating the simulation: " << JPS_ErrorMessage_GetMessage(message);
        WRITE_ERROR(oss.str());
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
MSPModel_JuPedSim::PState::PState(MSPerson* person, MSStageMoving* stage, JPS_Journey journey, Position destination, JPS_AgentId agentId)
    : myPerson(person), myAngle(0), myPosition(0, 0), myStage(stage), myJourney(journey), myDestination(destination), myAgentId(agentId) {
    ConstMSEdgeVector route = stage->getRoute();
}


MSPModel_JuPedSim::PState::~PState() {
    JPS_Journey_Free(myJourney);
}


Position MSPModel_JuPedSim::PState::getPosition(const MSStageMoving& stage, SUMOTime now) const {
    return myPosition;
}


void MSPModel_JuPedSim::PState::setPosition(double x, double y) {
    myPosition.set(x, y);
}


double MSPModel_JuPedSim::PState::getAngle(const MSStageMoving& stage, SUMOTime now) const {
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


double MSPModel_JuPedSim::PState::getEdgePos(const MSStageMoving& stage, SUMOTime now) const {
    return 0;
}


int MSPModel_JuPedSim::PState::getDirection(const MSStageMoving& stage, SUMOTime now) const {
    return UNDEFINED_DIRECTION;
}


SUMOTime MSPModel_JuPedSim::PState::getWaitingTime(const MSStageMoving& stage, SUMOTime now) const {
    return 0;
}


double MSPModel_JuPedSim::PState::getSpeed(const MSStageMoving& stage) const {
    return 0;
}


const MSEdge* MSPModel_JuPedSim::PState::getNextEdge(const MSStageMoving& stage) const {
    return nullptr;
}


Position MSPModel_JuPedSim::PState::getDestination(void) const {
    return myDestination;
}


JPS_AgentId MSPModel_JuPedSim::PState::getAgentId(void) const {
    return myAgentId;
}
