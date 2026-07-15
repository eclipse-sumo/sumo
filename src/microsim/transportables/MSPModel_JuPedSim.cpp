/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2014-2026 German Aerospace Center (DLR) and others.
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
#include <foreign/tcpip/socket.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSLink.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSJunctionControl.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSStoppingPlace.h>
#include <microsim/MSTrainHelper.h>
#include <microsim/MSRouteHandler.h>
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
const double MSPModel_JuPedSim::GEOS_MIN_AREA = 1;
const double MSPModel_JuPedSim::GEOS_BUFFERED_SEGMENT_WIDTH = 0.5 * SUMO_const_laneWidth;
const RGBColor MSPModel_JuPedSim::PEDESTRIAN_NETWORK_COLOR = RGBColor(179, 217, 255, 255);
const RGBColor MSPModel_JuPedSim::PEDESTRIAN_NETWORK_CARRIAGES_AND_RAMPS_COLOR = RGBColor(255, 217, 179, 255);
const std::string MSPModel_JuPedSim::PEDESTRIAN_NETWORK_ID = "jupedsim.pedestrian_network";
const std::string MSPModel_JuPedSim::PEDESTRIAN_NETWORK_CARRIAGES_AND_RAMPS_ID = "jupedsim.pedestrian_network.carriages_and_ramps";

const std::vector<MSPModel_JuPedSim::PState*> MSPModel_JuPedSim::noPedestrians;

// ===========================================================================
// method definitions
// ===========================================================================
MSPModel_JuPedSim::MSPModel_JuPedSim(const OptionsCont& oc, MSNet* net) :
    myRNG("JuPedSim"), myNetwork(net), myShapeContainer(net->getShapeContainer()), myJPSDeltaT(string2time(oc.getString("pedestrian.jupedsim.step-length"))),
    myExitTolerance(oc.getFloat("pedestrian.jupedsim.exit-tolerance")), myGEOSPedestrianNetworkLargestComponent(nullptr),
    myHaveAdditionalWalkableAreas(false) {
    RandHelper::initRandGlobal(&myRNG);
    myJPSModel = oc.getString("pedestrian.jupedsim.model");
    if (myJPSModel.find("Model") == std::string::npos) {
        // TODO warn about outdated model name
        myJPSModel += "Model";
        if (myJPSModel == "CollisionFreeSpeedV2Model") {
            myJPSModel = "CollisionFreeSpeedModelV2";
        }
    }
    std::string address = oc.getString("pedestrian.jupedsim.address");
    // address = "localhost:50051"; // for debugging
    if (address == "") {
#ifdef HAVE_BOOST
        const char* pythonEnv = getenv("PYTHON");
        const std::string python = (pythonEnv == nullptr) ? "python" : pythonEnv;
        const char* sumoHomeEnv = getenv("SUMO_HOME");
        const std::string sumoHome = (sumoHomeEnv == nullptr) ? "." : sumoHomeEnv;
        const std::string port = toString(tcpip::Socket::getFreeSocketPort());
        std::string command = python + " " + sumoHome + "/tools/jupedsim_grpc/servicer.py --port " + port;
        if (oc.isSet("pedestrian.jupedsim.py")) {
            command += " --debug";
        }
        // std::string command = "python -c \"import jupedsim; print(jupedsim.__file__)\" 2>&1";
        myJuPedSimServer = new bp::child(command);
        address = "localhost:" + port;
#endif
    }

    myGrpcChannel = grpc::CreateChannel(address, grpc::InsecureChannelCredentials());
    myGrpcStub = sumo_jupedsim_api::JuPedSimService::NewStub(myGrpcChannel);
    if (myGrpcChannel->WaitForConnected(std::chrono::system_clock::now() + std::chrono::seconds(2))) {
        WRITE_MESSAGEF(TL("Connected to JuPedSim gRPC server on '%'."), address);
    } else {
        WRITE_WARNINGF(TL("Could not connect to JuPedSim gRPC server on '%' (will retry on demand)."), address);
    }
    initialize(oc);
    net->getBeginOfTimestepEvents()->addEvent(new Event(this), net->getCurrentTimeStep() + DELTA_T);
}


MSPModel_JuPedSim::~MSPModel_JuPedSim() {
    clearState();
#ifdef HAVE_BOOST
    if (myJuPedSimServer != nullptr) {
        myJuPedSimServer->terminate();
    }
#endif

    // TODO: clear simulation, operational model and geometries?

    GEOSGeom_destroy(myGEOSPedestrianNetwork);
    finishGEOS();
}


void
MSPModel_JuPedSim::tryPedestrianInsertion(PState* state, const Position& p) {
    const MSVehicleType& type = state->getPerson()->getVehicleType();
    sumo_jupedsim_api::CreateAgentRequest agentRequest;
    agentRequest.set_simulation_id(myJPSSimulation);
    agentRequest.set_agent_model_class(myJPSModel);
    auto& agentParams = *agentRequest.mutable_agent_model_parameters()->mutable_fields();
    agentParams["journey_id"].set_number_value(static_cast<double>(state->getJourneyId()));
    agentParams["stage_id"].set_number_value(static_cast<double>(state->getStageId()));
    agentParams["desired_speed"].set_number_value(state->getPerson()->getMaxSpeed());
    if (myJPSModel != "GeneralizedCentrifugalForceModel") {
        agentParams["radius"].set_number_value(getRadius(type));
    }
    google::protobuf::ListValue* position = agentParams["position"].mutable_list_value();
    position->add_values()->set_number_value(p.x());
    position->add_values()->set_number_value(p.y());
    if (myJPSModel == "GeneralizedCentrifugalForceModel" || myJPSModel == "SocialForceModel") {
        const double angle = state->getAngle(*state->getStage(), 0);
        double orientation_x = 0.;
        double orientation_y = 0.;
        if (fabs(angle - M_PI / 2) < NUMERICAL_EPS) {
            orientation_y = 1.;
        } else if (fabs(angle + M_PI / 2) < NUMERICAL_EPS) {
            orientation_y = -1.;
        } else {
            orientation_x = 1.;
            orientation_y = tan(angle);
        }
        google::protobuf::ListValue* orientation = agentParams["orientation"].mutable_list_value();
        orientation->add_values()->set_number_value(orientation_x);
        orientation->add_values()->set_number_value(orientation_y);
    }
    grpc::ClientContext agentContext;
    sumo_jupedsim_api::CreateAgentResponse agentResponse;
    const grpc::Status agentStatus = myGrpcStub->CreateAgent(&agentContext, agentRequest, &agentResponse);
    if (!agentStatus.ok()) {
        WRITE_WARNINGF(TL("Error while adding person '%' as JuPedSim agent: %"), state->getPerson()->getID(), agentStatus.error_message());
        return;
    }
    state->setAgentId(agentResponse.agent_id());
}


MSPModel_JuPedSim::JPS_StageId
MSPModel_JuPedSim::addWaypoint(const std::string& agentID, const WaypointDesc& waypoint) {
    // Unlike the previous implementation we do not need to set the transitions explicitly anymore,
    // the intermediate JuPedSim GRPC service handles this for us.
    const Position& coords = std::get<1>(waypoint);
    sumo_jupedsim_api::AddWaypointStageRequest waypointRequest;
    waypointRequest.set_simulation_id(myJPSSimulation);
    waypointRequest.mutable_point()->set_x(coords.x());
    waypointRequest.mutable_point()->set_y(coords.y());
    waypointRequest.set_distance(std::get<2>(waypoint));
    grpc::ClientContext waypointContext;
    sumo_jupedsim_api::AddWaypointStageResponse waypointResponse;
    const grpc::Status waypointStatus = myGrpcStub->AddWaypointStage(&waypointContext, waypointRequest, &waypointResponse);
    if (!waypointStatus.ok()) {
        WRITE_WARNINGF(TL("Error while adding waypoint for person '%': %"), agentID, waypointStatus.error_message());
        return -1;
    }
    return waypointResponse.waypoint_id();
}


MSTransportableStateAdapter*
MSPModel_JuPedSim::add(MSTransportable* person, MSStageMoving* stage, SUMOTime now) {
    assert(person->getCurrentStageType() == MSStageType::WALKING);
    const double radius = getRadius(person->getVehicleType());
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
                    departurePosition.setx(RandHelper::rand(bbox.xmin(), bbox.xmax(), &myRNG));
                    departurePosition.sety(RandHelper::rand(bbox.ymin(), bbox.ymax(), &myRNG));
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
            departureRelativePositionY = RandHelper::rand(-halfDepartureLaneWidth, halfDepartureLaneWidth, &myRNG);
        }
        departurePosition = departureLane->getShape().positionAtOffset(stage->getDepartPos() + radius + POSITION_EPS, -departureRelativePositionY); // Minus sign is here for legacy reasons.
    }

    std::vector<WaypointDesc> waypoints;
    const MSEdge* prev = nullptr;
    for (const MSEdge* const e : stage->getEdges()) {
        const MSLane* const lane = getSidewalk<MSEdge, MSLane>(e);
        JPS_StageId waitingStage = 0;
        if (prev != nullptr) {
            int dir = UNDEFINED_DIRECTION;
            if (prev->getToJunction() == e->getFromJunction() || prev->getToJunction() == e->getToJunction()) {
                dir = FORWARD;
            } else if (prev->getFromJunction() == e->getFromJunction() || prev->getFromJunction() == e->getToJunction()) {
                dir = BACKWARD;
            }
            if (dir != UNDEFINED_DIRECTION) {
                ConstMSEdgeVector crossingRoute;
                MSNet::getInstance()->getPedestrianRouter(0).compute(prev, e, 0, 0, stage->getMaxSpeed(person), 0,
                        dir == FORWARD ? prev->getToJunction() : prev->getFromJunction(),
                        person->getVTypeParameter(), crossingRoute, true);
                const MSEdge* wa = nullptr;
                for (const MSEdge* const ce : crossingRoute) {
                    if (ce->isCrossing()) {
                        const MSLane* const crossing = getSidewalk<MSEdge, MSLane>(ce);
                        if (myCrossingWaits.count(crossing) > 0) {
                            if (waitingStage != 0 && wa != nullptr) {
                                // we already have a waiting stage we need an intermediate waypoint
                                waypoints.push_back({waitingStage, wa->getLanes()[0]->getShape().getCentroid(), wa->getWidth() / 2.});
                            }
                            const MSLane* const prevLane = getSidewalk<MSEdge, MSLane>(prev);
                            const Position& startPos = dir == FORWARD ? prevLane->getShape().back() : prevLane->getShape().front();
                            // choose the waiting set closer to the lane "end"
                            if (crossing->getShape().front().distanceSquaredTo(startPos) < crossing->getShape().back().distanceSquaredTo(startPos)) {
                                waitingStage = myCrossingWaits[crossing].first;
                            } else {
                                waitingStage = myCrossingWaits[crossing].second;
                            }
                        } else {
                            throw ProcessError(TLF("No waiting set for crossing at %.", ce->getID()));
                        }
                    }
                    if (ce->isWalkingArea()) {
                        wa = ce;
                    }
                }
            }
        }
        waypoints.push_back({waitingStage, lane->getShape().positionAtOffset(e->getLength() / 2.), lane->getWidth() / 2.});
        prev = e;
    }
    const JPS_StageId finalWait = std::get<0>(waypoints.back());
    waypoints.pop_back();  // arrival waypoint comes later
    if (!waypoints.empty()) {
        waypoints.erase(waypoints.begin());  // departure edge also does not need a waypoint
    }
    const MSLane* const arrivalLane = getSidewalk<MSEdge, MSLane>(stage->getDestination());
    const Position arrivalPosition = arrivalLane->getShape().positionAtOffset(stage->getArrivalPos());
    waypoints.push_back({finalWait, arrivalPosition, stage->getDouble("jupedsim.waypoint.radius", myExitTolerance)});

    sumo_jupedsim_api::AddJourneyRequest journeyRequest;
    journeyRequest.set_simulation_id(myJPSSimulation);
    JPS_StageId startingStage = 0;
    for (const auto& p : waypoints) {
        const JPS_StageId waiting = std::get<0>(p);
        if (waiting != 0) {
            journeyRequest.add_stage_ids(waiting);
        }
        JPS_StageId waypoint = addWaypoint(person->getID(), p);
        if (waypoint == -1) {
            return nullptr;
        }
        journeyRequest.add_stage_ids(waypoint);
        if (startingStage == 0) {
            startingStage = waiting != 0 ? waiting : waypoint;
        }
    }
    grpc::ClientContext journeyContext;
    sumo_jupedsim_api::AddJourneyResponse journeyResponse;
    const grpc::Status journeyStatus = myGrpcStub->AddJourney(&journeyContext, journeyRequest, &journeyResponse);
    if (!journeyStatus.ok()) {
        WRITE_WARNINGF(TL("JuPedSim gRPC AddJourney failed: %"), journeyStatus.error_message());
        return nullptr;
    }
    const JPS_JourneyId journeyId = journeyResponse.journey_id();

    PState* state = nullptr;
    for (PState* const pstate : myPedestrianStates) {  // TODO transform myPedestrianStates into a map for faster lookup
        if (pstate->getPerson() == person) {
            state = pstate;
            break;
        }
    }
    if (state == nullptr) {
        state = new PState(static_cast<MSPerson*>(person), stage, journeyId, startingStage, waypoints);
        state->setLanePosition(stage->getDepartPos() + radius + POSITION_EPS);
        state->setPosition(departurePosition.x(), departurePosition.y());
        state->setAngle(departureLane->getShape().rotationAtOffset(stage->getDepartPos()));
        myPedestrianStates.push_back(state);
        myNumActivePedestrians++;
    } else {
        state->reinit(stage, journeyId, startingStage, waypoints);
    }
    if (state->isWaitingToEnter()) {
        const Position p = state->getPosition(*state->getStage(), now);
        tryPedestrianInsertion(state, p);
    } else {
        sumo_jupedsim_api::SwitchAgentJourneyRequest switchRequest;
        switchRequest.set_agent_id(state->getAgentId());
        switchRequest.set_journey_id(journeyId);
        switchRequest.set_stage_id(startingStage);
        callGrpc(&sumo_jupedsim_api::JuPedSimService::Stub::SwitchAgentJourney, switchRequest,
                 TLF("Error while switching journey for agent '%': ", person->getID()));
    }
    return state;
}


void
MSPModel_JuPedSim::remove(MSTransportableStateAdapter* state) {
    PState* pstate = static_cast<PState*>(state);
    if (pstate->getLane() != nullptr) {
        auto& peds = myActiveLanes[pstate->getLane()];
        peds.erase(std::find(peds.begin(), peds.end(), pstate));
        pstate->setLane(nullptr);
    }
    if (pstate->getStage() != nullptr) {
        pstate->getStage()->setPState(nullptr);  // we need to remove the old state reference to avoid double deletion
    }
    pstate->setStage(nullptr);
}


SUMOTime
MSPModel_JuPedSim::execute(SUMOTime time) {
    const int nbrIterations = (int)(DELTA_T / myJPSDeltaT);
    sumo_jupedsim_api::IterateRequest iterateRequest;
    iterateRequest.set_simulation_id(myJPSSimulation);
    iterateRequest.set_count(nbrIterations);
    grpc::ClientContext iterateContext;
    sumo_jupedsim_api::IterateResponse iterateResponse;
    const grpc::Status iterateStatus = myGrpcStub->Iterate(&iterateContext, iterateRequest, &iterateResponse);
    if (!iterateStatus.ok()) {
        WRITE_WARNINGF(TL("JuPedSim gRPC Iterate failed: %"), iterateStatus.error_message());
        return DELTA_T;
    }

    sumo_jupedsim_api::GetCorePropertiesOfAllAgentsRequest propertiesRequest;
    propertiesRequest.set_simulation_id(myJPSSimulation);
    grpc::ClientContext propertiesContext;
    sumo_jupedsim_api::GetCorePropertiesOfAllAgentsResponse propertiesResponse;
    const grpc::Status propertiesStatus = myGrpcStub->GetCorePropertiesOfAllAgents(&propertiesContext, propertiesRequest, &propertiesResponse);
    if (!propertiesStatus.ok()) {
        WRITE_WARNINGF(TL("JuPedSim gRPC GetCorePropertiesOfAllAgents failed: %"), propertiesStatus.error_message());
        return DELTA_T;
    }
    const auto& properties = propertiesResponse.properties();

    for (auto stateIt = myPedestrianStates.begin(); stateIt != myPedestrianStates.end();) {
        PState* const state = *stateIt;

        if (state->isWaitingToEnter()) {
            // insertion failed at first try so we retry with some noise
            Position p = state->getPosition(*state->getStage(), time);
            p.setx(p.x() + RandHelper::rand(-.5, .5, &myRNG));  // we do this separately to avoid evaluation order problems
            p.sety(p.y() + RandHelper::rand(-.5, .5, &myRNG));
            tryPedestrianInsertion(state, p);
            ++stateIt;
            continue;
        }

        MSPerson* const person = state->getPerson();
        MSStageWalking* const stage = dynamic_cast<MSStageWalking*>(person->getCurrentStage());
        if (stage == nullptr) {
            // It seems we kept the state for another stage but the new stage is not a walk.
            // So let's remove the state because after the new stage we will be elsewhere and need to be reinserted for JuPedSim anyway.
            // We cannot check this earlier because when the old stage ends the next stage might not know yet whether it will be a walk.
            registerArrived(state->getAgentId());
            stateIt = myPedestrianStates.erase(stateIt);
            continue;
        }

        // Updates the agent position.
        const auto agentIt = properties.find(state->getAgentId());
        if (agentIt == properties.end()) {
            WRITE_WARNINGF(TL("JuPedSim gRPC unknown agent: %"), state->getAgentId());
            continue;
        }
        const sumo_jupedsim_api::Point& pos = agentIt->second.position();
        state->setPosition(pos.x(), pos.y());
        const sumo_jupedsim_api::Point& orientation = agentIt->second.orientation();
        state->setAngle(atan2(orientation.y(), orientation.x()));

        // Find on which edge the pedestrian is, using route's forward-looking edges because of how moveToXY is written.
        Position newPosition(pos.x(), pos.y());
        ConstMSEdgeVector route = stage->getEdges();
        const int routeIndex = (int)(stage->getRouteStep() - stage->getRoute().begin());
        const double oldLanePos = state->getEdgePos(time);
        ConstMSEdgeVector forwardRoute = ConstMSEdgeVector(route.begin() + routeIndex, route.end());
        double bestDistance = std::numeric_limits<double>::max();
        MSLane* candidateLane = nullptr;
        double candidateLaneLongitudinalPosition = 0.0;
        int routeOffset = 0;
        const bool found = libsumo::Helper::moveToXYMap_matchingRoutePosition(newPosition, "",
                           forwardRoute, 0, person->getVClass(), true, bestDistance, &candidateLane, candidateLaneLongitudinalPosition, routeOffset);

        if (found) {
            if (candidateLane != state->getLane()) {
                if (state->getLane() != nullptr) {
                    auto& peds = myActiveLanes[state->getLane()];
                    peds.erase(std::find(peds.begin(), peds.end(), state));
                }
                myActiveLanes[candidateLane].push_back(state);
                state->setLane(candidateLane);
            }
            state->setLanePosition(candidateLaneLongitudinalPosition);
        }

        const MSEdge* const expectedEdge = stage->getEdge();
        if (found && expectedEdge->isNormal() && candidateLane->getEdge().isNormal() && &candidateLane->getEdge() != expectedEdge) {
            const bool arrived = stage->moveToNextEdge(person, time, 1, nullptr);
            UNUSED_PARAMETER(arrived);
            assert(!arrived); // The person has not arrived yet.
            stage->activateEntryReminders(person);
            // Adapt speed to lane's speed limit.
            // TODO make this a bulk request
            sumo_jupedsim_api::SetDesiredSpeedRequest desiredSpeedRequest;
            auto speeds = desiredSpeedRequest.mutable_desired_speeds();
            (*speeds)[state->getAgentId()] = MIN2(candidateLane->getSpeedLimit(), person->getMaxSpeed());
            callGrpc(&sumo_jupedsim_api::JuPedSimService::Stub::SetDesiredSpeedOfAgents, desiredSpeedRequest,
                     TLF("Error while setting desired speed for %: ", person->getID()));
        }
        const double speed = person->getSpeed();
        for (int offset = 0; offset < 2; offset++) {
            const WaypointDesc* const waypoint = state->getNextWaypoint(offset);
            if (waypoint != nullptr && std::get<0>(*waypoint) != 0) {
                const JPS_StageId waitingStage = std::get<0>(*waypoint);
                const MSLane* const crossing = myCrossings[waitingStage];
                const MSLink* link = crossing->getIncomingLanes().front().viaLink;
                if (waitingStage == myCrossingWaits[crossing].second && link->getTLLogic() != nullptr) {
                    // we are walking backwards on a traffic light, there are different links to check
                    link = crossing->getLinkCont().front();
                    if (link->getTLLogic() == nullptr) {
                        link = crossing->getLogicalPredecessorLane()->getLinkTo(crossing);
                    }
                }
                // compare to and maybe adapt for MSPModel_Striping.cpp:1264
                const double passingClearanceTime = person->getFloatParam("pedestrian.timegap-crossing");
                const bool open = link->opened(time - DELTA_T, speed, speed, person->getVehicleType().getLength() + passingClearanceTime * speed,
                                               person->getImpatience(), speed, 0, 0, nullptr, false, person);
                sumo_jupedsim_api::SetWaitingSetStateRequest waitingSetStateRequest;
                waitingSetStateRequest.set_stage_id(waitingStage);
                waitingSetStateRequest.set_state(open ? sumo_jupedsim_api::INACTIVE : sumo_jupedsim_api::ACTIVE);
                callGrpc(&sumo_jupedsim_api::JuPedSimService::Stub::SetWaitingSetState, waitingSetStateRequest,
                         TLF("Error while setting waiting status for %:", crossing->getID()));
            }
        }
        stage->activateMoveReminders(person, oldLanePos, state->getEdgePos(time), state->getSpeed(*stage));
        // In the worst case during one SUMO step the person touches the waypoint radius and walks immediately into a different direction,
        // but at some simstep it should have a maximum distance of v * delta_t / 2 to the waypoint circle.
        const double slack = person->getMaxSpeed() * TS / 2. + POSITION_EPS;
        if (newPosition.distanceTo2D(std::get<1>(*state->getNextWaypoint())) < std::get<2>(*state->getNextWaypoint()) + slack) {
            // If near the last waypoint, remove the agent.
            if (state->advanceNextWaypoint()) {
                // TODO this only works if the final stage is actually a walk
                const bool finalStage = person->getNumRemainingStages() == 1;
                const JPS_AgentId agentID = state->getAgentId();
                while (!stage->moveToNextEdge(person, time, 1, nullptr));
                if (finalStage) {
                    registerArrived(agentID);
                    stateIt = myPedestrianStates.erase(stateIt);
                    continue;
                }
            }
        }
        ++stateIt;
    }

    // Remove pedestrians that are in a predefined area, at a predefined rate.
    for (const auto& area : myAreas) {
        sumo_jupedsim_api::GetAgentsInRegionRequest agentsRequest;
        *agentsRequest.mutable_region() = area->areaBoundary;
        const sumo_jupedsim_api::AgentIdsResponse agentsResponse = callGrpc(&sumo_jupedsim_api::JuPedSimService::Stub::GetAgentsInRegion,
                agentsRequest, TL("Error while retrieving agents in area: "));
        const int numAgents = agentsResponse.agent_ids_size();
        if (numAgents == 0) {
            continue;
        }
        if (area->areaType == "vanishing_area") {
            const SUMOTime period = area->params.count("period") > 0 ? string2time(area->params.at("period")) : 1000;
            const int nbrPeriodsCoveringTimestep = (int)ceil(TS / STEPS2TIME(period));
            if (time - area->lastRemovalTime >= nbrPeriodsCoveringTimestep * period) {
                for (int k = 0; k < MIN2(nbrPeriodsCoveringTimestep, numAgents); k++) {
                    const JPS_AgentId agentID = agentsResponse.agent_ids(k);
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
                            WRITE_MESSAGEF(TL("Person '%' in vanishing area '%' was removed from the simulation."), person->getID(), area->id);
                            while (!state->getStage()->moveToNextEdge(person, time, 1, nullptr));
                            registerArrived(agentID);
                            myPedestrianStates.erase(iterator);
                            area->lastRemovalTime = time;
                        }
                    }
                }
            }
        } else {  // areaType == "influencer"
            if (area->params.count("speed") == 0) {
                continue;
            }
            const double newSpeed = StringUtils::toDouble(area->params.at("speed"));
            sumo_jupedsim_api::SetDesiredSpeedRequest desiredSpeedRequest;
            auto speeds = desiredSpeedRequest.mutable_desired_speeds();
            for (int i = 0; i < numAgents; i++) {
                (*speeds)[agentsResponse.agent_ids(i)] = newSpeed;
            }
            callGrpc(&sumo_jupedsim_api::JuPedSimService::Stub::SetDesiredSpeedOfAgents, desiredSpeedRequest,
                     TL("Error while setting desired speed for area: "));
        }
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
                    const SUMOVTypeParameter& vTypeParam = train->getVehicleType().getParameter();
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
                        carriagePolygons.push_back(createGeometryFromShape(carriageShape));
                        // Create ramps geometry.
                        p += vTypeParam.maxPlatformDistance;
                        const double d = 0.5 * vTypeParam.carriageDoorWidth;
                        for (const Position& door : carriage->doorPositions) {
                            PositionVector rampShape;
                            rampShape.push_back(door - perp * p + dir * d);
                            rampShape.push_back(door - perp * p - dir * d);
                            rampShape.push_back(door + perp * p - dir * d);
                            rampShape.push_back(door + perp * p + dir * d);
                            rampPolygons.push_back(createGeometryFromShape(rampShape));
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
                    //dumpGeometry(pedestrianNetworkWithTrainsAndRamps, "pedestrianNetworkWithTrainsAndRamps.wkt");
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
                    //dumpGeometry(pedestrianNetworkWithTrainsAndRampsLargestComponent, "pedestrianNetworkWithTrainsAndRamps.wkt");
#endif
                    myJPSGeometryWithTrainsAndRamps = buildJPSGeometryFromGEOSGeometry(pedestrianNetworkWithTrainsAndRampsLargestComponent);
                    sumo_jupedsim_api::SwitchGeometryRequest switchRequest;
                    switchRequest.set_geometry_id(myJPSGeometryWithTrainsAndRamps);
                    callGrpc(&sumo_jupedsim_api::JuPedSimService::Stub::SwitchGeometry, switchRequest, TL("While switching to train geometry: "), true);
                    removePolygonFromDrawing(PEDESTRIAN_NETWORK_ID);
                    preparePolygonForDrawing(pedestrianNetworkWithTrainsAndRampsLargestComponent, PEDESTRIAN_NETWORK_CARRIAGES_AND_RAMPS_ID, PEDESTRIAN_NETWORK_CARRIAGES_AND_RAMPS_COLOR);
                    GEOSGeom_destroy(pedestrianNetworkWithTrainsAndRamps);
                }
                GEOSGeom_destroy(rampsCollection);
                GEOSGeom_destroy(carriagesCollection);
            }
        } else {
            if (myJPSGeometryWithTrainsAndRamps != -1) {
                sumo_jupedsim_api::SwitchGeometryRequest switchRequest;
                switchRequest.set_geometry_id(myJPSGeometry);
                callGrpc(&sumo_jupedsim_api::JuPedSimService::Stub::SwitchGeometry, switchRequest, TL("While switching to default geometry: "), true);
                preparePolygonForDrawing(myGEOSPedestrianNetworkLargestComponent, PEDESTRIAN_NETWORK_ID, PEDESTRIAN_NETWORK_COLOR);
                myJPSGeometryWithTrainsAndRamps = -1;
            }
        }
        myAllStoppedTrainIDs = allStoppedTrainIDs;
    }

    return DELTA_T;
}


void MSPModel_JuPedSim::registerArrived(const JPS_AgentId agentID) {
    myNumActivePedestrians--;
    sumo_jupedsim_api::RemoveAgentsRequest removeRequest;
    removeRequest.add_agent_ids(agentID);
    callGrpc(&sumo_jupedsim_api::JuPedSimService::Stub::RemoveAgents, removeRequest, TL("Error while removing agent: "));
}


void MSPModel_JuPedSim::clearState() {
    myPedestrianStates.clear();
    myNumActivePedestrians = 0;
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
MSPModel_JuPedSim::createGeometryFromShape(PositionVector shape, std::string junctionID, std::string shapeID, bool isInternalShape) {
    // Corner case.
    if (shape.size() == 1) {
        WRITE_WARNINGF(TL("Polygon '%' at junction '%' will be skipped as it is just a point."), shapeID, junctionID);
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
        WRITE_WARNINGF(TL("Polygon '%' at junction '%' had some equal consecutive points removed: %"), shapeID, junctionID, toString(duplicates, 9));
    }
    GEOSCoordSequence* coordinateSequence = convertToGEOSPoints(cleanShape);
    GEOSGeometry* linearRing = GEOSGeom_createLinearRing(coordinateSequence);
    GEOSGeometry* polygon = GEOSGeom_createPolygon(linearRing, nullptr, 0);

    if (!GEOSisValid(polygon)) {
        if (cleanShape.size() == 3) {
            if (isInternalShape) {
                WRITE_WARNINGF(TL("Polygon '%' at junction '%' has been dilated as it is just a segment."), shapeID, junctionID);
            } else {
                WRITE_WARNINGF(TL("Polygon '%' has been dilated as it is just a segment."), shapeID);
            }
            GEOSGeometry* lineString = GEOSGeom_createLineString(GEOSCoordSeq_clone(coordinateSequence));
            GEOSGeom_destroy(polygon);
            polygon = GEOSBufferWithStyle(lineString, GEOS_BUFFERED_SEGMENT_WIDTH, GEOS_QUADRANT_SEGMENTS, GEOSBUF_CAP_FLAT, GEOSBUF_JOIN_BEVEL, GEOS_MITRE_LIMIT);
            GEOSGeom_destroy(lineString);
        } else {
            if (isInternalShape) {
                WRITE_WARNINGF(TL("Polygon '%' at junction '%' has been replaced by its convex hull as it is not simple."), shapeID, junctionID);
                GEOSGeometry* hull = GEOSConvexHull(polygon);
                GEOSGeom_destroy(polygon);
                polygon = GEOSBufferWithStyle(hull, POSITION_EPS, GEOS_QUADRANT_SEGMENTS, GEOSBUF_CAP_FLAT, GEOSBUF_JOIN_ROUND, GEOS_MITRE_LIMIT);;
            } else {
                WRITE_WARNINGF(TL("Polygon '%' at junction '%' will be skipped as it is not simple."), shapeID, junctionID);
                polygon = nullptr;
            }
        }

        return polygon;
    }

    // Some junctions need to be buffered a little so that edges are properly connected.
    if (isInternalShape) {
        polygon = GEOSBufferWithStyle(polygon, POSITION_EPS, GEOS_QUADRANT_SEGMENTS, GEOSBUF_CAP_FLAT, GEOSBUF_JOIN_ROUND, GEOS_MITRE_LIMIT);
    }

    return polygon;
}


GEOSGeometry*
MSPModel_JuPedSim::buildPedestrianNetwork(MSNet* network) {
    std::vector<GEOSGeometry*> walkableAreas;
    for (const MSEdge* const edge : network->getEdgeControl().getEdges()) {
        const MSLane* const lane = getSidewalk<MSEdge, MSLane>(edge);
        if (lane != nullptr) {
            GEOSGeometry* dilatedLane = nullptr;
            if (edge->isWalkingArea()) {
                dilatedLane = createGeometryFromShape(lane->getShape(), edge->getFromJunction()->getID(), edge->getID(), true);
            } else if (edge->isCrossing()) {
                const PositionVector* outlineShape = lane->getOutlineShape();
                if (outlineShape != nullptr) {
                    // this is mainly to ensure that we have at least the "normal" crossing, see #15037
                    dilatedLane = createGeometryFromCenterLine(lane->getShape(), lane->getWidth() / 2.0, GEOSBUF_CAP_FLAT);
                    if (dilatedLane != nullptr) {
                        walkableAreas.push_back(dilatedLane);
                    }
                    dilatedLane = createGeometryFromShape(*outlineShape, edge->getFromJunction()->getID(), edge->getID(), true);
                } else {
                    dilatedLane = createGeometryFromCenterLine(lane->getShape(), lane->getWidth() / 2.0, GEOSBUF_CAP_ROUND);
                }
                myCrossingWaits[lane] = {0, 0};
            } else {  // regular sidewalk
                dilatedLane = createGeometryFromCenterLine(lane->getShape(), lane->getWidth() / 2.0 + POSITION_EPS, GEOSBUF_CAP_FLAT);
            }
            if (dilatedLane != nullptr) {
                walkableAreas.push_back(dilatedLane);
            }
        }
    }
    for (const auto& junctionWithID : network->getJunctionControl()) {
        const MSJunction* const junction = junctionWithID.second;
        int pedEdgeCount = 0;
        bool hasWalkingArea = false;
        for (const ConstMSEdgeVector& edges : {
                    junction->getIncoming(), junction->getOutgoing()
                }) {
            for (const MSEdge* const edge : edges) {
                if (edge->isWalkingArea()) {
                    hasWalkingArea = true;
                    break;
                }
                if (getSidewalk<MSEdge, MSLane>(edge) != nullptr) {
                    pedEdgeCount++;
                }
            }
            if (hasWalkingArea) {
                break;
            }
        }
        if (!hasWalkingArea && pedEdgeCount > 1) {
            // there is something to connect but no walking area, let's assume peds are allowed everywhere
            GEOSGeometry* walkingAreaGeom = createGeometryFromShape(junction->getShape(), junction->getID(), junction->getID(), true);
            if (walkingAreaGeom != nullptr) {
                walkableAreas.push_back(walkingAreaGeom);
            }
        }
    }

    // Retrieve additional walkable areas and obstacles (walkable areas and obstacles in the sense of JuPedSim).
    std::vector<GEOSGeometry*> additionalObstacles;
    for (const auto& polygonWithID : myNetwork->getShapeContainer().getPolygons()) {
        if (polygonWithID.second->getShapeType() == "jupedsim.walkable_area" || polygonWithID.second->getShapeType() == "taz") {
            GEOSGeometry* walkableArea = createGeometryFromShape(polygonWithID.second->getShape(), std::string(""), polygonWithID.first);
            if (walkableArea != nullptr) {
                walkableAreas.push_back(walkableArea);
                myHaveAdditionalWalkableAreas = true;
            }
        } else if (polygonWithID.second->getShapeType() == "jupedsim.obstacle") {
            GEOSGeometry* additionalObstacle = createGeometryFromShape(polygonWithID.second->getShape(), std::string(""), polygonWithID.first);
            if (additionalObstacle != nullptr) {
                additionalObstacles.push_back(additionalObstacle);
            }
        }
    }

    // Take the union of all walkable areas.
    GEOSGeometry* disjointWalkableAreas = GEOSGeom_createCollection(GEOS_MULTIPOLYGON, walkableAreas.data(), (unsigned int)walkableAreas.size());
#ifdef DEBUG_GEOMETRY_GENERATION
    // dumpGeometry(disjointWalkableAreas, "disjointWalkableAreas.wkt");
#endif
    GEOSGeometry* initialWalkableAreas = GEOSUnaryUnion(disjointWalkableAreas);
#ifdef DEBUG_GEOMETRY_GENERATION
    dumpGeometry(initialWalkableAreas, "initialWalkableAreas.wkt");
#endif
    GEOSGeom_destroy(disjointWalkableAreas);

    // At last, remove additional obstacles from the merged walkable areas.
    GEOSGeometry* disjointAdditionalObstacles = GEOSGeom_createCollection(GEOS_MULTIPOLYGON, additionalObstacles.data(), (unsigned int)additionalObstacles.size());
#ifdef DEBUG_GEOMETRY_GENERATION
    // dumpGeometry(disjointAdditionalObstacles, "disjointAdditionalObstacles.wkt");
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
        GEOSCoordSeq_setX(coordinateSequence, i, shape[i].x());
        GEOSCoordSeq_setY(coordinateSequence, i, shape[i].y());
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


void
MSPModel_JuPedSim::convertToJPSPoints(const GEOSGeometry* geometry, sumo_jupedsim_api::Polygon* into) {
    const GEOSCoordSequence* coordinateSequence = GEOSGeom_getCoordSeq(geometry);
    unsigned int coordinateSequenceSize;
    GEOSCoordSeq_getSize(coordinateSequence, &coordinateSequenceSize);
    double x;
    double y;
    // Remove the last point so that CGAL doesn't complain about the simplicity of the polygon downstream.
    for (unsigned int i = 0; i < coordinateSequenceSize - 1; i++) {
        GEOSCoordSeq_getX(coordinateSequence, i, &x);
        GEOSCoordSeq_getY(coordinateSequence, i, &y);
        sumo_jupedsim_api::Point* point = into->add_points();
        point->set_x(x);
        point->set_y(y);
    }
}


double
MSPModel_JuPedSim::getLinearRingArea(const GEOSGeometry* linearRing) {
    double area;
    GEOSGeometry* linearRingAsPolygon = GEOSGeom_createPolygon(GEOSGeom_clone(linearRing), nullptr, 0);
    GEOSArea(linearRingAsPolygon, &area);
    GEOSGeom_destroy(linearRingAsPolygon);
    return area;
}


void
MSPModel_JuPedSim::removePolygonFromDrawing(const std::string& polygonId) {
    myShapeContainer.removePolygon(polygonId);
}


void
MSPModel_JuPedSim::preparePolygonForDrawing(const GEOSGeometry* polygon, const std::string& polygonId, const RGBColor& color) {
    const GEOSGeometry* exterior = GEOSGetExteriorRing(polygon);
    bool added = myShapeContainer.addPolygon(polygonId, std::string("jupedsim.pedestrian_network"), color, 10.0, 0.0,
                 std::string(), convertToSUMOPoints(exterior), false, true, 1.0);
    if (added) {
        std::vector<PositionVector> holes;
        int nbrInteriorRings = GEOSGetNumInteriorRings(polygon);
        if (nbrInteriorRings != -1) {
            for (int k = 0; k < nbrInteriorRings; k++) {
                const GEOSGeometry* linearRing = GEOSGetInteriorRingN(polygon, k);
                double area = getLinearRingArea(linearRing);
                if (area > GEOS_MIN_AREA) {
                    holes.push_back(convertToSUMOPoints(linearRing));
                }
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


int64_t
MSPModel_JuPedSim::buildJPSGeometryFromGEOSGeometry(const GEOSGeometry* polygon) {
    sumo_jupedsim_api::CreateGeometryRequest geometryRequest;
    sumo_jupedsim_api::Polygon* boundary = geometryRequest.mutable_boundary();

    // Handle the exterior polygon.
    const GEOSGeometry* exterior =  GEOSGetExteriorRing(polygon);
    convertToJPSPoints(exterior, boundary);

    // Handle the interior polygons (holes).
    int nbrInteriorRings = GEOSGetNumInteriorRings(polygon);
    if (nbrInteriorRings != -1) {
        for (int k = 0; k < nbrInteriorRings; k++) {
            const GEOSGeometry* linearRing = GEOSGetInteriorRingN(polygon, k);
            double area = getLinearRingArea(linearRing);
            if (area > GEOS_MIN_AREA) {
                sumo_jupedsim_api::Polygon* hole = geometryRequest.add_obstacles();
                convertToJPSPoints(linearRing, hole);
            }
        }
    }
    grpc::ClientContext geometryContext;
    sumo_jupedsim_api::CreateGeometryResponse geometryResponse;
    const grpc::Status geometryStatus = myGrpcStub->CreateGeometry(&geometryContext, geometryRequest, &geometryResponse);
    if (!geometryStatus.ok()) {
        WRITE_WARNINGF(TL("JuPedSim gRPC CreateGeometry failed: %"), geometryStatus.error_message());
        return -1;
    }

    return geometryResponse.geometry_id();
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
    GEOSWKTWriter_setRoundingPrecision(writer, gPrecisionGeo);
    char* wkt = GEOSWKTWriter_write(writer, polygonGeoCoordinates == nullptr ? polygon : polygonGeoCoordinates);
    dumpFile << wkt << std::endl;
    dumpFile.close();
    GEOSFree(wkt);
    GEOSWKTWriter_destroy(writer);
    GEOSGeom_destroy(polygonGeoCoordinates);
}


double
MSPModel_JuPedSim::getRadius(const MSVehicleType& vehType) {
    return 0.5 * MAX2(vehType.getLength(), vehType.getWidth());
}


MSPModel_JuPedSim::JPS_StageId
MSPModel_JuPedSim::addWaitingSet(const MSLane* const crossing, const bool entry) {
    PositionVector shape = crossing->getShape();
    const double radius = getRadius(*MSNet::getInstance()->getVehicleControl().getVType(DEFAULT_PEDTYPE_ID, nullptr, true));
    shape.extrapolate2D((shape.length() + radius) / shape.length());
    const double offset = 2 * radius + POSITION_EPS;
    const double lonOffset = entry ? NUMERICAL_EPS : shape.length() - NUMERICAL_EPS;
    PositionVector pv;
    pv.push_back(shape.positionAtOffset(lonOffset));
    for (double latOff = offset; latOff < crossing->getWidth() / 2. - offset; latOff += offset) {
        PositionVector moved(shape);
        moved.move2side(latOff);
        pv.push_back(moved.positionAtOffset(lonOffset));
        moved.move2side(-2. * latOff);
        pv.push_back(moved.positionAtOffset(lonOffset));
    }
    if (entry && crossing->getIncomingLanes().size() == 1 && crossing->getIncomingLanes().front().lane->isWalkingArea()) {
        pv.push_back(crossing->getIncomingLanes().front().lane->getShape().getCentroid());
    }
    if (!entry && crossing->getLinkCont().size() == 1 && crossing->getLinkCont().front()->getLane()->isWalkingArea()) {
        pv.push_back(crossing->getLinkCont().front()->getLane()->getShape().getCentroid());
    }

    sumo_jupedsim_api::AddWaitingSetStageRequest waitingSetRequest;
    for (const Position& p : pv) {
        GEOSCoordSequence* seq = GEOSCoordSeq_create(1, 2); // 1 point, 2 dimensions
        GEOSCoordSeq_setX(seq, 0, p.x());
        GEOSCoordSeq_setY(seq, 0, p.y());
        GEOSGeometry* geosPoint = GEOSGeom_createPoint(seq);
        if (GEOSContains(myGEOSPedestrianNetworkLargestComponent, geosPoint)) {
            sumo_jupedsim_api::Point* point = waitingSetRequest.add_points();
            point->set_x(p.x());
            point->set_y(p.y());
        } else {
            WRITE_WARNINGF("Waiting point %,% is not in the geometry for % on '%'.", p.x(), p.y(), entry ? "entry" : "exit", crossing->getID())
        }
        GEOSGeom_destroy(geosPoint);
    }
    auto waitingSetResponse = callGrpc(&sumo_jupedsim_api::JuPedSimService::Stub::AddWaitingSetStage, waitingSetRequest,
                                       TLF("Error while adding waiting set for % on '%':", entry ? "entry" : "exit", crossing->getID()));
    JPS_StageId waitingStage = waitingSetResponse.stage_id();

    sumo_jupedsim_api::SetWaitingSetStateRequest waitingSetStateRequest;
    waitingSetStateRequest.set_stage_id(waitingStage);
    waitingSetStateRequest.set_state(sumo_jupedsim_api::INACTIVE);
    callGrpc(&sumo_jupedsim_api::JuPedSimService::Stub::SetWaitingSetState, waitingSetStateRequest,
             TLF("Error while setting waiting status for % on '%':", entry ? "entry" : "exit", crossing->getID()));

    myCrossings[waitingStage] = crossing;
    return waitingStage;
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
#ifdef DEBUG_GEOMETRY_GENERATION
    dumpGeometry(myGEOSPedestrianNetworkLargestComponent, "pedestrianNetwork.wkt");
#endif
    const std::string filename = oc.getString("pedestrian.jupedsim.wkt");
    if (!filename.empty()) {
        dumpGeometry(myGEOSPedestrianNetworkLargestComponent, filename, oc.getBool("pedestrian.jupedsim.wkt.geo"));
    }
    // For the moment, only one connected component is supported.
    myJPSGeometry = buildJPSGeometryFromGEOSGeometry(myGEOSPedestrianNetworkLargestComponent);
    myJPSGeometryWithTrainsAndRamps = -1;
    preparePolygonForDrawing(myGEOSPedestrianNetworkLargestComponent, PEDESTRIAN_NETWORK_ID, PEDESTRIAN_NETWORK_COLOR);
    PROGRESS_DONE_MESSAGE();

    double strengthGeometryRepulsion = oc.getFloat("pedestrian.jupedsim.strength-geometry-repulsion");
    double rangeGeometryRepulsion = oc.getFloat("pedestrian.jupedsim.range-geometry-repulsion");
    if (myJPSDeltaT == 20) {
        if (oc.isDefault("pedestrian.jupedsim.strength-geometry-repulsion") && oc.isDefault("pedestrian.jupedsim.range-geometry-repulsion")) {
            WRITE_MESSAGE(TL("Adapting geometry repulsion default values for jupedsim timestep of 0.02."));
            strengthGeometryRepulsion = 35.;
            rangeGeometryRepulsion = 0.019;
        }
    }
    sumo_jupedsim_api::CreateSimulationRequest simulationRequest;
    simulationRequest.set_geometry_id(myJPSGeometry);
    simulationRequest.set_model_class(myJPSModel);
    simulationRequest.set_delta_t(STEPS2TIME(myJPSDeltaT));
    // Default server side arguments for the models.
    // TODO: Better handling of this as this is still hard-coded for each model class.
    auto& modelParams = *simulationRequest.mutable_model_parameters()->mutable_fields();
    if (myJPSModel == "CollisionFreeSpeedModel") {
        modelParams["strength_neighbor_repulsion"].set_number_value(oc.getFloat("pedestrian.jupedsim.strength-neighbor-repulsion"));
        modelParams["range_neighbor_repulsion"].set_number_value(oc.getFloat("pedestrian.jupedsim.range-neighbor-repulsion"));
        modelParams["strength_geometry_repulsion"].set_number_value(strengthGeometryRepulsion);
        modelParams["range_geometry_repulsion"].set_number_value(rangeGeometryRepulsion);
    } else if (myJPSModel == "CollisionFreeSpeedModelV2") {
        // Empty - did not set any parameters in old implementation
    } else if (myJPSModel == "GeneralizedCentrifugalForceModel") {
        modelParams["strength_neighbor_repulsion"].set_number_value(oc.getFloat("pedestrian.jupedsim.strength-neighbor-repulsion"));
        modelParams["strength_geometry_repulsion"].set_number_value(strengthGeometryRepulsion);
    } else if (myJPSModel == "SocialForceModel") {
        modelParams["body_force"].set_number_value(120000.0);
        modelParams["friction"].set_number_value(240000.0);
    }
    grpc::ClientContext simulationContext;
    sumo_jupedsim_api::CreateSimulationResponse simulationResponse;
    const grpc::Status simulationStatus = myGrpcStub->CreateSimulation(&simulationContext, simulationRequest, &simulationResponse);
    if (simulationStatus.ok()) {
        myJPSSimulation = simulationResponse.simulation_id();
    } else {
        throw ProcessError(TLF("JuPedSim gRPC CreateSimulation failed: %", simulationStatus.error_message()));
    }
    // Polygons that define vanishing areas aren't part of the regular JuPedSim geometry.
    for (const auto& polygonWithID : myNetwork->getShapeContainer().getPolygons()) {
        polygonChanged(polygonWithID.second, true, false);
    }
    // add waiting sets at crossings
    for (auto& crossing : myCrossingWaits) {
        crossing.second.first = addWaitingSet(crossing.first, true);
        crossing.second.second = addWaitingSet(crossing.first, false);
    }
    myNetwork->getShapeContainer().addShapeListener(this);
}


void
MSPModel_JuPedSim::polygonChanged(const SUMOPolygon* const poly, const bool added, const bool removed) {
    if (poly->getShapeType() == "jupedsim.vanishing_area" || poly->getShapeType() == "jupedsim.influencer") {
        SUMOTime lastRemovalTime = 0;
        if (!added) {
            for (auto areaIt = myAreas.begin(); areaIt != myAreas.end(); ++areaIt) {
                if (poly->getID() == (*areaIt)->id) {
                    lastRemovalTime = (*areaIt)->lastRemovalTime;
                    myAreas.erase(areaIt);
                    break;
                }
            }
        }
        if (!removed) {
            sumo_jupedsim_api::Polygon areaBoundary;
            for (const Position& p : poly->getShape()) {
                sumo_jupedsim_api::Point* point = areaBoundary.add_points();
                point->set_x(p.x());
                point->set_y(p.y());
            }
            // // Make sure the shape is not repeating the first point.
            // if (areaBoundary.back().x == areaBoundary.front().x && areaBoundary.back().y == areaBoundary.front().y) {
            //     areaBoundary.pop_back();
            // }
            const std::string type = StringTokenizer(poly->getShapeType(), ".").getVector()[1];
            myAreas.emplace_back(std::unique_ptr<AreaData>(new AreaData{poly->getID(), type, areaBoundary, poly->getParametersMap(), lastRemovalTime}));
        }
    }
}


// ===========================================================================
// MSPModel_Remote::PState method definitions
// ===========================================================================
MSPModel_JuPedSim::PState::PState(MSPerson* person, MSStageMoving* stage,
                                  JPS_JourneyId journeyId, JPS_StageId stageId,
                                  const std::vector<WaypointDesc>& waypoints) :
    MSPModel_InteractingState(person, stage, nullptr),
    myJourneyId(journeyId), myStageId(stageId), myWaypoints(waypoints), myAgentId(0) {
    myDir = FORWARD;
}


void
MSPModel_JuPedSim::PState::reinit(MSStageMoving* stage, JPS_JourneyId journeyId, JPS_StageId stageId,
                                  const std::vector<WaypointDesc>& waypoints) {
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


void MSPModel_JuPedSim::PState::setPosition(const double x, const double y, const double z) {
    if (myRemoteXYPos != Position::INVALID) {
        mySpeed = myRemoteXYPos.distanceTo2D(Position(x, y, z)) / STEPS2TIME(DELTA_T);
    } else {
        mySpeed = 0.;
    }
    myRemoteXYPos.set(x, y, z);
}


const MSEdge* MSPModel_JuPedSim::PState::getNextEdge(const MSStageMoving& stage) const {
    return stage.getNextRouteEdge();
}


const MSPModel_JuPedSim::WaypointDesc*
MSPModel_JuPedSim::PState::getNextWaypoint(const int offset) const {
    return offset < (int)myWaypoints.size() ? &myWaypoints[offset] : nullptr;
}


/****************************************************************************/
