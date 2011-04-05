/****************************************************************************/
/// @file    TraCIServerAPI_Vehicle.cpp
/// @author  Daniel Krajzewicz
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting vehicle values via TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <microsim/MSNet.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <utils/geom/Position2DVector.h>
#include <utils/common/DijkstraRouterTT.h>
#include <utils/common/DijkstraRouterEffort.h>
#include "TraCIConstants.h"
#include "TraCIServerAPI_Simulation.h"
#include "TraCIServerAPI_Vehicle.h"
#include <utils/common/HelpersHBEFA.h>
#include <utils/common/HelpersHarmonoise.h>
#include <microsim/MSEdgeWeightsStorage.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace traci;


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_Vehicle::processGet(TraCIServer &server, tcpip::Storage &inputStorage,
                                   tcpip::Storage &outputStorage) {
    std::string warning = ""; // additional description for response
    // variable & id
    int variable = inputStorage.readUnsignedByte();
    std::string id = inputStorage.readString();
    // check variable
    if (variable!=ID_LIST&&variable!=VAR_SPEED&&variable!=VAR_SPEED_WITHOUT_TRACI&&variable!=VAR_POSITION&&variable!=VAR_ANGLE
            &&variable!=VAR_ROAD_ID&&variable!=VAR_LANE_ID&&variable!=VAR_LANE_INDEX
            &&variable!=VAR_TYPE&&variable!=VAR_ROUTE_ID&&variable!=VAR_COLOR
            &&variable!=VAR_LANEPOSITION
            &&variable!=VAR_CO2EMISSION&&variable!=VAR_COEMISSION&&variable!=VAR_HCEMISSION&&variable!=VAR_PMXEMISSION
            &&variable!=VAR_NOXEMISSION&&variable!=VAR_FUELCONSUMPTION&&variable!=VAR_NOISEEMISSION
            &&variable!=VAR_EDGE_TRAVELTIME&&variable!=VAR_EDGE_EFFORT
            &&variable!=VAR_ROUTE_VALID&&variable!=VAR_EDGES
            &&variable!=VAR_SIGNALS
            &&variable!=VAR_LENGTH&&variable!=VAR_MAXSPEED&&variable!=VAR_VEHICLECLASS
            &&variable!=VAR_SPEED_FACTOR&&variable!=VAR_SPEED_DEVIATION&&variable!=VAR_EMISSIONCLASS
            &&variable!=VAR_WIDTH&&variable!=VAR_GUIOFFSET&&variable!=VAR_SHAPE
            &&variable!=VAR_ACCEL&&variable!=VAR_DECEL&&variable!=VAR_IMPERFECTION
            &&variable!=VAR_TAU&&variable!=VAR_BEST_LANES&&variable!=DISTANCE_REQUEST
       ) {
        server.writeStatusCmd(CMD_GET_VEHICLE_VARIABLE, RTYPE_ERR, "Get Vehicle Variable: unsupported variable specified", outputStorage);
        return false;
    }
    // begin response building
    tcpip::Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(RESPONSE_GET_VEHICLE_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);
    // process request
    if (variable==ID_LIST) {
        std::vector<std::string> ids;
        MSVehicleControl &c = MSNet::getInstance()->getVehicleControl();
        for (MSVehicleControl::constVehIt i=c.loadedVehBegin(); i!=c.loadedVehEnd(); ++i) {
            if ((*i).second->isOnRoad()) {
                ids.push_back((*i).first);
            }
        }
        tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
        tempMsg.writeStringList(ids);
    } else {
        MSVehicle *v = static_cast<MSVehicle*>(MSNet::getInstance()->getVehicleControl().getVehicle(id));
        if (v==0) {
            server.writeStatusCmd(CMD_GET_VEHICLE_VARIABLE, RTYPE_ERR, "Vehicle '" + id + "' is not known", outputStorage);
            return false;
        }
        bool onRoad = v->isOnRoad();
        switch (variable) {
        case VAR_SPEED:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            onRoad ? tempMsg.writeFloat((float)v->getSpeed()) : tempMsg.writeFloat((float)-1001);
            break;
        case VAR_SPEED_WITHOUT_TRACI:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            onRoad ? tempMsg.writeFloat((float)v->getSpeedWithoutTraciInfluence()) : tempMsg.writeFloat((float)-1001);
            break;
        case VAR_POSITION:
            tempMsg.writeUnsignedByte(POSITION_2D);
            onRoad ? tempMsg.writeFloat((float)v->getPosition().x()) : tempMsg.writeFloat((float)-1001);
            onRoad ? tempMsg.writeFloat((float)v->getPosition().y()) : tempMsg.writeFloat((float)-1001);
            break;
        case VAR_ANGLE:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            onRoad ? tempMsg.writeFloat((float)v->getAngle()) : tempMsg.writeFloat((float)-1001);
            break;
        case VAR_ROAD_ID:
            tempMsg.writeUnsignedByte(TYPE_STRING);
            onRoad ? tempMsg.writeString(v->getLane()->getEdge().getID()) : tempMsg.writeString("");
            break;
        case VAR_LANE_ID:
            tempMsg.writeUnsignedByte(TYPE_STRING);
            onRoad ? tempMsg.writeString(v->getLane()->getID()) : tempMsg.writeString("");
            break;
        case VAR_LANE_INDEX: {
            const std::vector<MSLane*> &lanes = v->getLane()->getEdge().getLanes();
            tempMsg.writeUnsignedByte(TYPE_INTEGER);
            onRoad ? tempMsg.writeInt((int)(std::distance(lanes.begin(), std::find(lanes.begin(), lanes.end(), v->getLane())))) : tempMsg.writeInt(-1);
        }
        break;
        case VAR_TYPE:
            tempMsg.writeUnsignedByte(TYPE_STRING);
            tempMsg.writeString(v->getVehicleType().getID());
            break;
        case VAR_ROUTE_ID:
            tempMsg.writeUnsignedByte(TYPE_STRING);
            tempMsg.writeString(v->getRoute().getID());
            break;
        case VAR_COLOR:
            tempMsg.writeUnsignedByte(TYPE_COLOR);
            tempMsg.writeUnsignedByte((int)(v->getParameter().color.red()*255.));
            tempMsg.writeUnsignedByte((int)(v->getParameter().color.green()*255.));
            tempMsg.writeUnsignedByte((int)(v->getParameter().color.blue()*255.));
            tempMsg.writeUnsignedByte(255);
            break;
        case VAR_LANEPOSITION:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            onRoad ? tempMsg.writeFloat((float)v->getPositionOnLane()) : tempMsg.writeFloat((float)-1001);
            break;
        case VAR_CO2EMISSION:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            onRoad ? tempMsg.writeFloat((float)(HelpersHBEFA::computeCO2(v->getVehicleType().getEmissionClass(), v->getSpeed(), v->getPreDawdleAcceleration()))) : tempMsg.writeFloat((float)-1001);
            break;
        case VAR_COEMISSION:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            onRoad ? tempMsg.writeFloat((float)(HelpersHBEFA::computeCO(v->getVehicleType().getEmissionClass(), v->getSpeed(), v->getPreDawdleAcceleration()))) : tempMsg.writeFloat((float)-1001);
            break;
        case VAR_HCEMISSION:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            onRoad ? tempMsg.writeFloat((float)(HelpersHBEFA::computeHC(v->getVehicleType().getEmissionClass(), v->getSpeed(), v->getPreDawdleAcceleration()))) : tempMsg.writeFloat((float)-1001);
            break;
        case VAR_PMXEMISSION:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            onRoad ? tempMsg.writeFloat((float)(HelpersHBEFA::computePMx(v->getVehicleType().getEmissionClass(), v->getSpeed(), v->getPreDawdleAcceleration()))) : tempMsg.writeFloat((float)-1001);
            break;
        case VAR_NOXEMISSION:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            onRoad ? tempMsg.writeFloat((float)(HelpersHBEFA::computeNOx(v->getVehicleType().getEmissionClass(), v->getSpeed(), v->getPreDawdleAcceleration()))) : tempMsg.writeFloat((float)-1001);
            break;
        case VAR_FUELCONSUMPTION:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            onRoad ? tempMsg.writeFloat((float)(HelpersHBEFA::computeFuel(v->getVehicleType().getEmissionClass(), v->getSpeed(), v->getPreDawdleAcceleration()))) : tempMsg.writeFloat((float)-1001);
            break;
        case VAR_NOISEEMISSION:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            onRoad ? tempMsg.writeFloat((float)(HelpersHarmonoise::computeNoise(v->getVehicleType().getEmissionClass(), v->getSpeed(), v->getPreDawdleAcceleration()))) : tempMsg.writeFloat((float)-1001);
            break;
        case VAR_EDGE_TRAVELTIME: {
            if (inputStorage.readUnsignedByte()!=TYPE_COMPOUND) {
                server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Retrieval of travel time requires a compound object.", outputStorage);
                return false;
            }
            if (inputStorage.readInt()!=2) {
                server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Retrieval of travel time requires time, and edge as parameter.", outputStorage);
                return false;
            }
            // time
            if (inputStorage.readUnsignedByte()!=TYPE_INTEGER) {
                server.writeStatusCmd(CMD_GET_VEHICLE_VARIABLE, RTYPE_ERR, "Retrieval of travel time requires the referenced time as first parameter.", outputStorage);
                return false;
            }
            SUMOTime time = inputStorage.readInt();
            // edge
            if (inputStorage.readUnsignedByte()!=TYPE_STRING) {
                server.writeStatusCmd(CMD_GET_VEHICLE_VARIABLE, RTYPE_ERR, "Retrieval of travel time requires the referenced edge as second parameter.", outputStorage);
                return false;
            }
            std::string edgeID = inputStorage.readString();
            MSEdge *edge = MSEdge::dictionary(edgeID);
            if (edge==0) {
                server.writeStatusCmd(CMD_GET_VEHICLE_VARIABLE, RTYPE_ERR, "Referenced edge '" + edgeID + "' is not known.", outputStorage);
                return false;
            }
            // retrieve
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            SUMOReal value;
            if (!v->getWeightsStorage().retrieveExistingTravelTime(edge, 0, time, value)) {
                tempMsg.writeFloat(-1);
            } else {
                tempMsg.writeFloat((float)value);
            }

        }
        break;
        case VAR_EDGE_EFFORT: {
            if (inputStorage.readUnsignedByte()!=TYPE_COMPOUND) {
                server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Retrieval of travel time requires a compound object.", outputStorage);
                return false;
            }
            if (inputStorage.readInt()!=2) {
                server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Retrieval of travel time requires time, and edge as parameter.", outputStorage);
                return false;
            }
            // time
            if (inputStorage.readUnsignedByte()!=TYPE_INTEGER) {
                server.writeStatusCmd(CMD_GET_VEHICLE_VARIABLE, RTYPE_ERR, "Retrieval of effort requires the referenced time as first parameter.", outputStorage);
                return false;
            }
            SUMOTime time = inputStorage.readInt();
            // edge
            if (inputStorage.readUnsignedByte()!=TYPE_STRING) {
                server.writeStatusCmd(CMD_GET_VEHICLE_VARIABLE, RTYPE_ERR, "Retrieval of effort requires the referenced edge as second parameter.", outputStorage);
                return false;
            }
            std::string edgeID = inputStorage.readString();
            MSEdge *edge = MSEdge::dictionary(edgeID);
            if (edge==0) {
                server.writeStatusCmd(CMD_GET_VEHICLE_VARIABLE, RTYPE_ERR, "Referenced edge '" + edgeID + "' is not known.", outputStorage);
                return false;
            }
            // retrieve
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            SUMOReal value;
            if (!v->getWeightsStorage().retrieveExistingEffort(edge, 0, time, value)) {
                tempMsg.writeFloat(-1);
            } else {
                tempMsg.writeFloat((float)value);
            }

        }
        break;
        case VAR_ROUTE_VALID: {
            std::string msg;
            if (!v->hasValidRoute(msg)) {
                server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, msg, outputStorage);
                return false;
            }
            tempMsg.writeUnsignedByte(TYPE_UBYTE);
            tempMsg.writeUnsignedByte(1);
        }
        break;
        case VAR_EDGES: {
            const MSRoute &r = v->getRoute();
            tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
            tempMsg.writeInt(r.size());
            for (MSRouteIterator i=r.begin(); i!=r.end(); ++i) {
                tempMsg.writeString((*i)->getID());
            }
        }
        break;
        case VAR_SIGNALS:
            tempMsg.writeUnsignedByte(TYPE_INTEGER);
            tempMsg.writeInt(v->getSignals());
            break;
        case VAR_LENGTH:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat((float)(v->getVehicleType().getLength()));
            break;
        case VAR_MAXSPEED:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat((float)(v->getVehicleType().getMaxSpeed()));
            break;
        case VAR_VEHICLECLASS:
            tempMsg.writeUnsignedByte(TYPE_STRING);
            tempMsg.writeString(getVehicleClassName(v->getVehicleType().getVehicleClass()));
            break;
        case VAR_SPEED_FACTOR:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat((float)(v->getVehicleType().getSpeedFactor()));
            break;
        case VAR_SPEED_DEVIATION:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat((float)(v->getVehicleType().getSpeedDeviation()));
            break;
        case VAR_EMISSIONCLASS:
            tempMsg.writeUnsignedByte(TYPE_STRING);
            tempMsg.writeString(getVehicleEmissionTypeName(v->getVehicleType().getEmissionClass()));
            break;
        case VAR_WIDTH:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat((float)(v->getVehicleType().getGuiWidth()));
            break;
        case VAR_GUIOFFSET:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat((float)(v->getVehicleType().getGuiOffset()));
            break;
        case VAR_SHAPE:
            tempMsg.writeUnsignedByte(TYPE_STRING);
            tempMsg.writeString(getVehicleShapeName(v->getVehicleType().getGuiShape()));
            break;
        case VAR_ACCEL:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat((float)(v->getVehicleType().getCarFollowModel().getMaxAccel()));
            break;
        case VAR_DECEL:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat((float)(v->getVehicleType().getCarFollowModel().getMaxDecel()));
            break;
        case VAR_IMPERFECTION:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat((float)(v->getVehicleType().getCarFollowModel().getImperfection()));
            break;
        case VAR_TAU:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat((float)(v->getVehicleType().getCarFollowModel().getTau()));
            break;
        case VAR_BEST_LANES: {
            const std::vector<MSVehicle::LaneQ> &bestLanes = v->getBestLanes();
            tempMsg.writeUnsignedByte(TYPE_COMPOUND);
            tcpip::Storage tempContent;
            unsigned int cnt = 0;
            tempContent.writeUnsignedByte(TYPE_INTEGER);
            tempContent.writeInt((int) bestLanes.size());
            ++cnt;
            for (std::vector<MSVehicle::LaneQ>::const_iterator i=bestLanes.begin(); i!=bestLanes.end(); ++i) {
                const MSVehicle::LaneQ &lq = *i;
                tempContent.writeUnsignedByte(TYPE_STRING);
                tempContent.writeString(lq.lane->getID());
                ++cnt;
                tempContent.writeUnsignedByte(TYPE_FLOAT);
                tempContent.writeFloat((float)(lq.length));
                ++cnt;
                tempContent.writeUnsignedByte(TYPE_FLOAT);
                tempContent.writeFloat((float)(lq.nextOccupation));
                ++cnt;
                tempContent.writeUnsignedByte(TYPE_BYTE);
                tempContent.writeByte(lq.bestLaneOffset);
                ++cnt;
                tempContent.writeUnsignedByte(TYPE_UBYTE);
                lq.allowsContinuation ? tempContent.writeUnsignedByte(1) : tempContent.writeUnsignedByte(0);
                ++cnt;
                std::vector<std::string> bestContIDs;
                for (std::vector<MSLane*>::const_iterator j=lq.bestContinuations.begin(); j!=lq.bestContinuations.end(); ++j) {
                    bestContIDs.push_back((*j)->getID());
                }
                tempContent.writeUnsignedByte(TYPE_STRINGLIST);
                tempContent.writeStringList(bestContIDs);
                ++cnt;
            }
            tempMsg.writeInt((int) cnt);
            tempMsg.writeStorage(tempContent);
        }
        break;
        case DISTANCE_REQUEST:
            if (!commandDistanceRequest(server, inputStorage, tempMsg, v)) {
                return false;
            }
            break;
        default:
            break;
        }
    }
    server.writeStatusCmd(CMD_GET_VEHICLE_VARIABLE, RTYPE_OK, warning, outputStorage);
    // send response
    outputStorage.writeUnsignedByte(0); // command length -> extended
    outputStorage.writeInt(1 + 4 + (int)tempMsg.size());
    outputStorage.writeStorage(tempMsg);
    return true;
}


bool
TraCIServerAPI_Vehicle::processSet(TraCIServer &server, tcpip::Storage &inputStorage,
                                   tcpip::Storage &outputStorage) {
    std::string warning = ""; // additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    if (variable!=CMD_SETMAXSPEED&&variable!=CMD_STOP&&variable!=CMD_CHANGELANE
            &&variable!=CMD_SLOWDOWN&&/*variable!=CMD_CHANGEROUTE&&*/variable!=CMD_CHANGETARGET
            &&variable!=VAR_ROUTE_ID&&variable!=VAR_ROUTE
            &&variable!=VAR_EDGE_TRAVELTIME&&variable!=VAR_EDGE_EFFORT
            &&variable!=CMD_REROUTE_TRAVELTIME&&variable!=CMD_REROUTE_EFFORT
            &&variable!=VAR_SIGNALS&&variable!=VAR_MOVE_TO
            &&variable!=VAR_LENGTH&&variable!=VAR_MAXSPEED&&variable!=VAR_VEHICLECLASS
            &&variable!=VAR_SPEED_FACTOR&&variable!=VAR_SPEED_DEVIATION&&variable!=VAR_EMISSIONCLASS
            &&variable!=VAR_WIDTH&&variable!=VAR_GUIOFFSET&&variable!=VAR_SHAPE
            &&variable!=VAR_ACCEL&&variable!=VAR_DECEL&&variable!=VAR_IMPERFECTION
            &&variable!=VAR_TAU
            &&variable!=VAR_SPEED&&variable!=VAR_COLOR
            &&variable!=ADD
       ) {
        server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Change Vehicle State: unsupported variable specified", outputStorage);
        return false;
    }
    // id
    std::string id = inputStorage.readString();
    SUMOVehicle *v = MSNet::getInstance()->getVehicleControl().getVehicle(id);
    if (v==0&&variable!=ADD) {
        server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Vehicle '" + id + "' is not known", outputStorage);
        return false;
    }
    // process
    int valueDataType = inputStorage.readUnsignedByte();
    switch (variable) {
    case CMD_SETMAXSPEED: {
        if (valueDataType!=TYPE_FLOAT) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "The speed must be given as a float.", outputStorage);
            return false;
        }
        getSingularType(v).setMaxSpeed(inputStorage.readFloat());
    }
    break;
    case CMD_STOP: {
        if (valueDataType!=TYPE_COMPOUND) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Stop needs a compound object description.", outputStorage);
            return false;
        }
        if (inputStorage.readInt()!=4) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Stop needs a compound object description of four items.", outputStorage);
            return false;
        }
        // read road map position
        valueDataType = inputStorage.readUnsignedByte();
        if (valueDataType!=TYPE_STRING) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "The first stop parameter must be the edge id given as a string.", outputStorage);
            return false;
        }
        std::string roadId = inputStorage.readString();
        valueDataType = inputStorage.readUnsignedByte();
        if (valueDataType!=TYPE_FLOAT) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "The second stop parameter must be the position along the edge given as a float.", outputStorage);
            return false;
        }
        SUMOReal pos = inputStorage.readFloat();
        valueDataType = inputStorage.readUnsignedByte();
        if (valueDataType!=TYPE_BYTE) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "The third stop parameter must be the lane index given as a byte.", outputStorage);
            return false;
        }
        int laneIndex = inputStorage.readByte();
        // waitTime
        valueDataType = inputStorage.readUnsignedByte();
        if (valueDataType!=TYPE_INTEGER) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "The fourth stop parameter must be the waiting time given as an integer.", outputStorage);
            return false;
        }
        SUMOTime waitTime = inputStorage.readInt();
        // check
        if (pos < 0) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Position on lane must not be negative", outputStorage);
            return false;
        }
        // get the actual lane that is referenced by laneIndex
        MSEdge* road = MSEdge::dictionary(roadId);
        if (road == 0) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Unable to retrieve road with given id", outputStorage);
            return false;
        }
        const std::vector<MSLane*> &allLanes = road->getLanes();
        if ((laneIndex < 0) || laneIndex >= (int)(allLanes.size())) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "No lane existing with such id on the given road", outputStorage);
            return false;
        }
        // Forward command to vehicle
        if (!static_cast<MSVehicle*>(v)->addTraciStop(allLanes[laneIndex], pos, 0, waitTime)) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Vehicle is too close or behind the stop on " + allLanes[laneIndex]->getID(), outputStorage);
            return false;
        }
    }
    break;
    case CMD_CHANGELANE: {
        if (valueDataType!=TYPE_COMPOUND) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Lane change needs a compound object description.", outputStorage);
            return false;
        }
        if (inputStorage.readInt()!=2) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Lane change needs a compound object description of two items.", outputStorage);
            return false;
        }
        // Lane ID
        valueDataType = inputStorage.readUnsignedByte();
        if (valueDataType!=TYPE_BYTE) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "The first lane change parameter must be the lane index given as a byte.", outputStorage);
            return false;
        }
        int laneIndex = inputStorage.readByte();
        // stickyTime
        valueDataType = inputStorage.readUnsignedByte();
        if (valueDataType!=TYPE_INTEGER) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "The second lane change parameter must be the duration given as an integer.", outputStorage);
            return false;
        }
        SUMOTime stickyTime = inputStorage.readInt();
        if ((laneIndex < 0) || (laneIndex >= (int)(v->getEdge()->getLanes().size()))) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "No lane existing with given id on the current road", outputStorage);
            return false;
        }
        // Forward command to vehicle
        static_cast<MSVehicle*>(v)->startLaneChange(static_cast<unsigned>(laneIndex), static_cast<SUMOTime>(stickyTime));
    }
    break;
    case CMD_SLOWDOWN: {
        if (valueDataType!=TYPE_COMPOUND) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Slow down needs a compound object description.", outputStorage);
            return false;
        }
        if (inputStorage.readInt()!=2) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Slow down needs a compound object description of two items.", outputStorage);
            return false;
        }
        if (inputStorage.readUnsignedByte()!=TYPE_FLOAT) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "The first slow down parameter must be the speed given as a float.", outputStorage);
            return false;
        }
        SUMOReal newSpeed = MAX2(inputStorage.readFloat(), 0.0f);
        if (newSpeed < 0) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Speed must not be negative", outputStorage);
            return false;
        }
        if (inputStorage.readUnsignedByte()!=TYPE_INTEGER) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "The second slow down parameter must be the duration given as an integer.", outputStorage);
            return false;
        }
        SUMOTime duration = inputStorage.readInt();
        if (duration <= 0) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Invalid time interval", outputStorage);
            return false;
        }
        std::vector<std::pair<SUMOTime, SUMOReal> > speedTimeLine;
        speedTimeLine.push_back(std::make_pair(MSNet::getInstance()->getCurrentTimeStep(), v->getSpeed()));
        speedTimeLine.push_back(std::make_pair(MSNet::getInstance()->getCurrentTimeStep()+duration, (SUMOReal) newSpeed));
        static_cast<MSVehicle*>(v)->getInfluencer().setSpeedTimeLine(speedTimeLine);
    }
    break;
    case CMD_CHANGEROUTE: {
        if (valueDataType!=TYPE_STRINGLIST) {
            server.writeStatusCmd(CMD_SET_ROUTE_VARIABLE, RTYPE_ERR, "A string list is needed for changing to a new route.", outputStorage);
            return false;
        }
        int numEdges = inputStorage.readInt();
		MSEdgeVector edges(numEdges);
		while (numEdges--) {
			MSEdge* edge = MSEdge::dictionary(inputStorage.readString());
			if (edge==0) {
				server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Unknown edge in route.", outputStorage);
				return false;
			}
			edges.push_back(edge);
		}
	    if (!v->replaceRouteEdges(edges)) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Route replacement failed for " + v->getID(), outputStorage);
            return false;
        }
	}
    break;
    case CMD_CHANGETARGET: {
        if (valueDataType!=TYPE_STRING) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Change target requires a string containing the id of the new destination edge as parameter.", outputStorage);
            return false;
        }
        std::string edgeID = inputStorage.readString();
        const MSEdge* destEdge = MSEdge::dictionary(edgeID);
        if (destEdge == 0) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Can not retrieve road with ID " + edgeID, outputStorage);
            return false;
        }
        // build a new route between the vehicle's current edge and destination edge
        MSEdgeVector newRoute;
        const MSEdge* currentEdge = v->getEdge();
        MSEdgeWeightsStorage empty;
        MSNet::EdgeWeightsProxi proxi(empty, MSNet::getInstance()->getWeightsStorage());
        DijkstraRouterTT_ByProxi<MSEdge, SUMOVehicle, prohibited_withRestrictions<MSEdge, SUMOVehicle>, MSNet::EdgeWeightsProxi> router(MSEdge::dictSize(), true, &proxi, &MSNet::EdgeWeightsProxi::getTravelTime);
        router.compute(currentEdge, destEdge, (const MSVehicle* const) v, MSNet::getInstance()->getCurrentTimeStep(), newRoute);
        // replace the vehicle's route by the new one
        if (!v->replaceRouteEdges(newRoute)) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Route replacement failed for " + v->getID(), outputStorage);
            return false;
        }
    }
    break;
    case VAR_ROUTE_ID: {
        if (valueDataType!=TYPE_STRING) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "The route id must be given as a string.", outputStorage);
            return false;
        }
        std::string rid = inputStorage.readString();
        const MSRoute *r = MSRoute::dictionary(rid);
        if (r==0) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "The route '" + rid + "' is not known.", outputStorage);
            return false;
        }
        if (!v->replaceRoute(r)) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Route replacement failed for " + v->getID(), outputStorage);
            return false;
        }
    }
    break;
    case VAR_ROUTE: {
        if (valueDataType!=TYPE_STRINGLIST) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "A route must be defined as a list of edge ids.", outputStorage);
            return false;
        }
        std::vector<std::string> edgeIDs = inputStorage.readStringList();
        std::vector<const MSEdge*> edges;
        MSEdge::parseEdgesList(edgeIDs, edges, "<unknown>");
        if (!v->replaceRouteEdges(edges)) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Route replacement failed for " + v->getID(), outputStorage);
            return false;
        }
    }
    break;
    case VAR_EDGE_TRAVELTIME: {
        if (valueDataType!=TYPE_COMPOUND) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting travel time requires a compound object.", outputStorage);
            return false;
        }
        int parameterCount = inputStorage.readInt();
        if (parameterCount==4) {
            // begin time
            if (inputStorage.readUnsignedByte()!=TYPE_INTEGER) {
                server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting travel time using 4 parameters requires the begin time as first parameter.", outputStorage);
                return false;
            }
            SUMOTime begTime = inputStorage.readInt();
            // begin time
            if (inputStorage.readUnsignedByte()!=TYPE_INTEGER) {
                server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting travel time using 4 parameters requires the end time as second parameter.", outputStorage);
                return false;
            }
            SUMOTime endTime = inputStorage.readInt();
            // edge
            if (inputStorage.readUnsignedByte()!=TYPE_STRING) {
                server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting travel time using 4 parameters requires the referenced edge as third parameter.", outputStorage);
                return false;
            }
            std::string edgeID = inputStorage.readString();
            MSEdge *edge = MSEdge::dictionary(edgeID);
            if (edge==0) {
                server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Referenced edge '" + edgeID + "' is not known.", outputStorage);
                return false;
            }
            // value
            if (inputStorage.readUnsignedByte()!=TYPE_FLOAT) {
                server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting travel time using 4 parameters requires the travel time as fourth parameter.", outputStorage);
                return false;
            }
            SUMOReal value = inputStorage.readFloat();
            // retrieve
            static_cast<MSVehicle*>(v)->getWeightsStorage().addTravelTime(edge, begTime, endTime, value);
        } else if (parameterCount==2) {
            // edge
            if (inputStorage.readUnsignedByte()!=TYPE_STRING) {
                server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting travel time using 2 parameters requires the referenced edge as first parameter.", outputStorage);
                return false;
            }
            std::string edgeID = inputStorage.readString();
            MSEdge *edge = MSEdge::dictionary(edgeID);
            if (edge==0) {
                server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Referenced edge '" + edgeID + "' is not known.", outputStorage);
                return false;
            }
            // value
            if (inputStorage.readUnsignedByte()!=TYPE_FLOAT) {
                server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting travel time using 2 parameters requires the travel time as second parameter.", outputStorage);
                return false;
            }
            SUMOReal value = inputStorage.readFloat();
            // retrieve
            while (static_cast<MSVehicle*>(v)->getWeightsStorage().knowsTravelTime(edge)) {
                static_cast<MSVehicle*>(v)->getWeightsStorage().removeTravelTime(edge);
            }
            static_cast<MSVehicle*>(v)->getWeightsStorage().addTravelTime(edge, 0, SUMOTime_MAX, value);
        } else if (parameterCount==1) {
            // edge
            if (inputStorage.readUnsignedByte()!=TYPE_STRING) {
                server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting travel time using 1 parameter requires the referenced edge as first parameter.", outputStorage);
                return false;
            }
            std::string edgeID = inputStorage.readString();
            MSEdge *edge = MSEdge::dictionary(edgeID);
            if (edge==0) {
                server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Referenced edge '" + edgeID + "' is not known.", outputStorage);
                return false;
            }
            // retrieve
            while (static_cast<MSVehicle*>(v)->getWeightsStorage().knowsTravelTime(edge)) {
                static_cast<MSVehicle*>(v)->getWeightsStorage().removeTravelTime(edge);
            }
        } else {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting travel time requires 1, 2, or 4 parameters.", outputStorage);
            return false;
        }
    }
    break;
    case VAR_EDGE_EFFORT: {
        if (valueDataType!=TYPE_COMPOUND) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting effort requires a compound object.", outputStorage);
            return false;
        }
        int parameterCount = inputStorage.readInt();
        if (parameterCount==4) {
            // begin time
            if (inputStorage.readUnsignedByte()!=TYPE_INTEGER) {
                server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting effort using 4 parameters requires the begin time as first parameter.", outputStorage);
                return false;
            }
            SUMOTime begTime = inputStorage.readInt();
            // begin time
            if (inputStorage.readUnsignedByte()!=TYPE_INTEGER) {
                server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting effort using 4 parameters requires the end time as second parameter.", outputStorage);
                return false;
            }
            SUMOTime endTime = inputStorage.readInt();
            // edge
            if (inputStorage.readUnsignedByte()!=TYPE_STRING) {
                server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting effort using 4 parameters requires the referenced edge as third parameter.", outputStorage);
                return false;
            }
            std::string edgeID = inputStorage.readString();
            MSEdge *edge = MSEdge::dictionary(edgeID);
            if (edge==0) {
                server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Referenced edge '" + edgeID + "' is not known.", outputStorage);
                return false;
            }
            // value
            if (inputStorage.readUnsignedByte()!=TYPE_FLOAT) {
                server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting effort using 4 parameters requires the travel time as fourth parameter.", outputStorage);
                return false;
            }
            SUMOReal value = inputStorage.readFloat();
            // retrieve
            static_cast<MSVehicle*>(v)->getWeightsStorage().addEffort(edge, begTime, endTime, value);
        } else if (parameterCount==2) {
            // edge
            if (inputStorage.readUnsignedByte()!=TYPE_STRING) {
                server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting effort using 2 parameters requires the referenced edge as first parameter.", outputStorage);
                return false;
            }
            std::string edgeID = inputStorage.readString();
            MSEdge *edge = MSEdge::dictionary(edgeID);
            if (edge==0) {
                server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Referenced edge '" + edgeID + "' is not known.", outputStorage);
                return false;
            }
            // value
            if (inputStorage.readUnsignedByte()!=TYPE_FLOAT) {
                server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting effort using 2 parameters requires the travel time as second parameter.", outputStorage);
                return false;
            }
            SUMOReal value = inputStorage.readFloat();
            // retrieve
            while (static_cast<MSVehicle*>(v)->getWeightsStorage().knowsEffort(edge)) {
                static_cast<MSVehicle*>(v)->getWeightsStorage().removeEffort(edge);
            }
            static_cast<MSVehicle*>(v)->getWeightsStorage().addEffort(edge, 0, SUMOTime_MAX, value);
        } else if (parameterCount==1) {
            // edge
            if (inputStorage.readUnsignedByte()!=TYPE_STRING) {
                server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting effort using 1 parameter requires the referenced edge as first parameter.", outputStorage);
                return false;
            }
            std::string edgeID = inputStorage.readString();
            MSEdge *edge = MSEdge::dictionary(edgeID);
            if (edge==0) {
                server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Referenced edge '" + edgeID + "' is not known.", outputStorage);
                return false;
            }
            // retrieve
            while (static_cast<MSVehicle*>(v)->getWeightsStorage().knowsEffort(edge)) {
                static_cast<MSVehicle*>(v)->getWeightsStorage().removeEffort(edge);
            }
        } else {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting effort requires 1, 2, or 4 parameters.", outputStorage);
            return false;
        }
    }
    break;
    case CMD_REROUTE_TRAVELTIME: {
        if (valueDataType!=TYPE_COMPOUND) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Rerouting requires a compound object.", outputStorage);
            return false;
        }
        if (inputStorage.readInt()!=0) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Rerouting should obtain an empty compound object.", outputStorage);
            return false;
        }
        MSNet::EdgeWeightsProxi proxi(static_cast<MSVehicle*>(v)->getWeightsStorage(), MSNet::getInstance()->getWeightsStorage());
        DijkstraRouterTT_ByProxi<MSEdge, SUMOVehicle, prohibited_withRestrictions<MSEdge, SUMOVehicle>, MSNet::EdgeWeightsProxi> router(MSEdge::dictSize(), true, &proxi, &MSNet::EdgeWeightsProxi::getTravelTime);
        v->reroute(MSNet::getInstance()->getCurrentTimeStep(), router);
    }
    break;
    case CMD_REROUTE_EFFORT: {
        if (valueDataType!=TYPE_COMPOUND) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Rerouting requires a compound object.", outputStorage);
            return false;
        }
        if (inputStorage.readInt()!=0) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Rerouting should obtain an empty compound object.", outputStorage);
            return false;
        }
        MSNet::EdgeWeightsProxi proxi(static_cast<MSVehicle*>(v)->getWeightsStorage(), MSNet::getInstance()->getWeightsStorage());
        DijkstraRouterEffort_ByProxi<MSEdge, SUMOVehicle, prohibited_withRestrictions<MSEdge, SUMOVehicle>, MSNet::EdgeWeightsProxi> router(MSEdge::dictSize(), true, &proxi, &MSNet::EdgeWeightsProxi::getEffort, &MSNet::EdgeWeightsProxi::getTravelTime);
        v->reroute(MSNet::getInstance()->getCurrentTimeStep(), router);
    }
    break;
    case VAR_SIGNALS:
        if (valueDataType!=TYPE_INTEGER) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting signals requires an integer.", outputStorage);
            return false;
        }
        static_cast<MSVehicle*>(v)->switchOffSignal(0x0fffffff);
        static_cast<MSVehicle*>(v)->switchOnSignal(inputStorage.readInt());
        break;
    case VAR_MOVE_TO: {
        if (valueDataType!=TYPE_COMPOUND) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting position requires a compound object.", outputStorage);
            return false;
        }
        if (inputStorage.readInt()!=2) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting position should obtain the lane id and the position.", outputStorage);
            return false;
        }
        // lane ID
        if (inputStorage.readUnsignedByte()!=TYPE_STRING) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "The first parameter for setting a position must be the lane ID given as a string.", outputStorage);
            return false;
        }
        std::string laneID = inputStorage.readString();
        // position on lane
        if (inputStorage.readUnsignedByte()!=TYPE_DOUBLE) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "The second parameter for setting a position must be the position given as a double.", outputStorage);
            return false;
        }
        float position = (float)(inputStorage.readDouble());
        // process
        MSLane *l = MSLane::dictionary(laneID);
        if (l==0) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Unknown lane '" + laneID + "'.", outputStorage);
            return false;
        }
        MSEdge &destinationEdge = l->getEdge();
        if (!static_cast<MSVehicle*>(v)->willPass(&destinationEdge)) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Vehicle '" + laneID + "' may be set onto an edge to pass only.", outputStorage);
            return false;
        }
        static_cast<MSVehicle*>(v)->onRemovalFromNet(MSMoveReminder::NOTIFICATION_TELEPORT);
        static_cast<MSVehicle*>(v)->getLane()->removeVehicle(static_cast<MSVehicle*>(v));
        while (v->getEdge()!=&destinationEdge) {
            const MSEdge *nextEdge = v->succEdge(1);
            // let the vehicle move to the next edge
            if (static_cast<MSVehicle*>(v)->enterLaneAtMove(nextEdge->getLanes()[0], true)) {
                MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(v);
                continue;
            }
        }
        l->forceVehicleInsertion(static_cast<MSVehicle*>(v), position);
    }
    break;
    case VAR_SPEED: {
        if (valueDataType!=TYPE_DOUBLE) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting speed requires a double.", outputStorage);
            return false;
        }
        SUMOReal speed = inputStorage.readDouble();
        std::vector<std::pair<SUMOTime, SUMOReal> > speedTimeLine;
        if(speed>=0) {
            speedTimeLine.push_back(std::make_pair(MSNet::getInstance()->getCurrentTimeStep(), speed));
            speedTimeLine.push_back(std::make_pair(SUMOTime_MAX, speed));
        }
        static_cast<MSVehicle*>(v)->getInfluencer().setSpeedTimeLine(speedTimeLine);
                    }
        break;
    case VAR_COLOR: {
        if (valueDataType!=TYPE_COLOR) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "The color must be given using the according type.", outputStorage);
            return false;
        }
        SUMOReal r = (SUMOReal) inputStorage.readUnsignedByte() / 255.;
        SUMOReal g = (SUMOReal) inputStorage.readUnsignedByte() / 255.;
        SUMOReal b = (SUMOReal) inputStorage.readUnsignedByte() / 255.;
		//read SUMOReal a 
		inputStorage.readUnsignedByte();
        v->getParameter().color.set(r, g, b);
    }
    break;
    case VAR_LENGTH: {
        if (valueDataType!=TYPE_FLOAT) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting length requires a float.", outputStorage);
            return false;
        }
        getSingularType(v).setLength(inputStorage.readFloat());
    }
    break;
    case VAR_MAXSPEED: {
        if (valueDataType!=TYPE_FLOAT) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting maximum speed requires a float.", outputStorage);
            return false;
        }
        getSingularType(v).setMaxSpeed(inputStorage.readFloat());
    }
    break;
    case VAR_VEHICLECLASS: {
        if (valueDataType!=TYPE_STRING) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting vehicle class requires a string.", outputStorage);
            return false;
        }
        getSingularType(v).setVClass(getVehicleClassID(inputStorage.readString()));
    }
    break;
    case VAR_SPEED_FACTOR: {
        if (valueDataType!=TYPE_FLOAT) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting speed factor requires a float.", outputStorage);
            return false;
        }
        getSingularType(v).setSpeedFactor(inputStorage.readFloat());
    }
    break;
    case VAR_SPEED_DEVIATION: {
        if (valueDataType!=TYPE_FLOAT) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting speed deviation requires a float.", outputStorage);
            return false;
        }
        getSingularType(v).setSpeedDeviation(inputStorage.readFloat());
    }
    break;
    case VAR_EMISSIONCLASS: {
        if (valueDataType!=TYPE_STRING) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting emission class requires a string.", outputStorage);
            return false;
        }
        getSingularType(v).setEmissionClass(getVehicleEmissionTypeID(inputStorage.readString()));
    }
    break;
    case VAR_WIDTH: {
        if (valueDataType!=TYPE_FLOAT) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting width requires a float.", outputStorage);
            return false;
        }
        getSingularType(v).setWidth(inputStorage.readFloat());
    }
    break;
    case VAR_GUIOFFSET: {
        if (valueDataType!=TYPE_FLOAT) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting gui offset requires a float.", outputStorage);
            return false;
        }
        getSingularType(v).setOffset(inputStorage.readFloat());
    }
    break;
    case VAR_SHAPE: {
        if (valueDataType!=TYPE_STRING) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting vehicle shape requires a string.", outputStorage);
            return false;
        }
        getSingularType(v).setShape(getVehicleShapeID(inputStorage.readString()));
    }
    break;
    case VAR_ACCEL: {
        if (valueDataType!=TYPE_FLOAT) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting acceleration requires a float.", outputStorage);
            return false;
        }
        getSingularType(v).getCarFollowModel().setMaxAccel(inputStorage.readFloat());
    }
    break;
    case VAR_DECEL: {
        if (valueDataType!=TYPE_FLOAT) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting deceleration requires a float.", outputStorage);
            return false;
        }
        getSingularType(v).getCarFollowModel().setMaxDecel(inputStorage.readFloat());
    }
    break;
    case VAR_IMPERFECTION: {
        if (valueDataType!=TYPE_FLOAT) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting driver imperfection requires a float.", outputStorage);
            return false;
        }
        getSingularType(v).getCarFollowModel().setImperfection(inputStorage.readFloat());
    }
    break;
    case VAR_TAU: {
        if (valueDataType!=TYPE_FLOAT) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting reaction time requires a float.", outputStorage);
            return false;
        }
        getSingularType(v).getCarFollowModel().setTau(inputStorage.readFloat());
    }
    break;
    case ADD: {
        if (valueDataType!=TYPE_COMPOUND) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Adding a vehicle requires a compound object.", outputStorage);
            return false;
        }
        if (inputStorage.readInt()!=6) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Adding a vehicle needs six parameters.", outputStorage);
            return false;
        }
        SUMOVehicleParameter* vehicleParams = new SUMOVehicleParameter();
        vehicleParams->id = id;
        if (inputStorage.readUnsignedByte()!=TYPE_STRING) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "First parameter (type) requires a string.", outputStorage);
            return false;
        }
        const std::string typeID = inputStorage.readString();
        if (inputStorage.readUnsignedByte()!=TYPE_STRING) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Second parameter (route) requires a string.", outputStorage);
            return false;
        }
        const std::string routeID = inputStorage.readString();
        if (inputStorage.readUnsignedByte()!=TYPE_INTEGER) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Third parameter (depart) requires an integer.", outputStorage);
            return false;
        }
        vehicleParams->depart = inputStorage.readInt();
        if (inputStorage.readUnsignedByte()!=TYPE_FLOAT) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Fourth parameter (position) requires a float.", outputStorage);
            return false;
        }
        const SUMOReal pos = inputStorage.readFloat();
        if (inputStorage.readUnsignedByte()!=TYPE_FLOAT) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Fifth parameter (speed) requires a float.", outputStorage);
            return false;
        }
        const SUMOReal speed = inputStorage.readFloat();
        if (inputStorage.readUnsignedByte()!=TYPE_BYTE) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Sixth parameter (lane) requires a byte.", outputStorage);
            return false;
        }
        const int laneIndex = inputStorage.readByte();
    }
    break;
    default:
        break;
    }
    server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_OK, warning, outputStorage);
    return true;
}


bool
TraCIServerAPI_Vehicle::commandDistanceRequest(traci::TraCIServer &server, tcpip::Storage &inputStorage,
                                               tcpip::Storage &outputStorage, const MSVehicle* v) {
    Position2D pos;
    std::pair<const MSLane*, SUMOReal> roadPos;

    // read position
    int posType = inputStorage.readUnsignedByte();
    switch (posType) {
    case POSITION_ROADMAP:
        try {
            std::string roadID = inputStorage.readString();
            roadPos.second = inputStorage.readFloat();
            roadPos.first = TraCIServerAPI_Simulation::getLaneChecking(roadID, inputStorage.readUnsignedByte(), roadPos.second);
            pos = roadPos.first->getShape().positionAtLengthPosition(roadPos.second);
        } catch (TraCIException &e) {
            server.writeStatusCmd(CMD_GET_VEHICLE_VARIABLE, RTYPE_ERR, e.what());
            return false;
        }
        break;
    case POSITION_2D:
    case POSITION_2_5D:
    case POSITION_3D: {
        float p1x = inputStorage.readFloat();
        float p1y = inputStorage.readFloat();
        pos.set(p1x, p1y);
    }
    if ((posType == POSITION_2_5D) || (posType == POSITION_3D)) {
        inputStorage.readFloat();		// z value is ignored
    }
    roadPos = TraCIServerAPI_Simulation::convertCartesianToRoadMap(pos);
    break;
    default:
        server.writeStatusCmd(CMD_GET_VEHICLE_VARIABLE, RTYPE_ERR, "Unknown position format used for distance request");
        return false;
    }

    // read distance type
    int distType = inputStorage.readUnsignedByte();

    SUMOReal distance = 0.0;
    if (distType == REQUEST_DRIVINGDIST) {
        distance = v->getRoute().getDistanceBetween(v->getPositionOnLane(), roadPos.second,
                                                    v->getEdge(), &roadPos.first->getEdge());
    } else {
        // compute air distance (default)
        distance = v->getPosition().distanceTo(pos);
    }
    // write response command
    outputStorage.writeUnsignedByte(TYPE_FLOAT);
    outputStorage.writeFloat(static_cast<float>(distance));
    return true;
}

// ------ helper functions ------
MSVehicleType &
TraCIServerAPI_Vehicle::getSingularType(SUMOVehicle * const veh) throw() {
    const MSVehicleType &oType = veh->getVehicleType();
    std::string newID = oType.getID().find('@')==std::string::npos ? oType.getID() + "@" + veh->getID() : oType.getID();
    MSVehicleType *type = MSVehicleType::build(newID, &oType);
    static_cast<MSVehicle*>(veh)->replaceVehicleType(type);
    return *type;
}


/****************************************************************************/

