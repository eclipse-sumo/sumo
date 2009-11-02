/****************************************************************************/
/// @file    TraCIServerAPI_Vehicle.cpp
/// @author  Daniel Krajzewicz
/// @date    07.05.2009
/// @version $Id: TraCIServerAPI_Vehicle.cpp 6907 2009-03-13 12:13:38Z dkrajzew $
///
// APIs for getting/setting vehicle values via TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
#include "TraCIConstants.h"
#include "TraCIServerAPIHelper.h"
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
using namespace std;
using namespace traci;
using namespace tcpip;


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_Vehicle::processGet(tcpip::Storage &inputStorage,
                                   tcpip::Storage &outputStorage) throw(TraCIException) {
    string warning = ""; // additional description for response
    // variable & id
    int variable = inputStorage.readUnsignedByte();
    string id = inputStorage.readString();
    // check variable
    if (variable!=ID_LIST&&variable!=VAR_SPEED&&variable!=VAR_POSITION&&variable!=VAR_ANGLE
            &&variable!=VAR_ROAD_ID&&variable!=VAR_LANE_ID&&variable!=VAR_LANE_INDEX
            &&variable!=VAR_TYPE&&variable!=VAR_ROUTE_ID&&variable!=VAR_COLOR
            &&variable!=VAR_LANEPOSITION
            &&variable!=VAR_CO2EMISSION&&variable!=VAR_COEMISSION&&variable!=VAR_HCEMISSION&&variable!=VAR_PMXEMISSION
            &&variable!=VAR_NOXEMISSION&&variable!=VAR_FUELCONSUMPTION&&variable!=VAR_NOISEEMISSION
            &&variable!=VAR_EDGE_TRAVELTIME&&variable!=VAR_EDGE_EFFORT
       ) {
        TraCIServerAPIHelper::writeStatusCmd(CMD_GET_VEHICLE_VARIABLE, RTYPE_ERR, "Unsupported variable specified", outputStorage);
        return false;
    }
    // begin response building
    Storage tempMsg;
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
        MSVehicle *v = MSNet::getInstance()->getVehicleControl().getVehicle(id);
        if (v==0) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_GET_VEHICLE_VARIABLE, RTYPE_ERR, "Vehicle '" + id + "' is not known", outputStorage);
            return false;
        }
        switch (variable) {
        case VAR_SPEED:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(v->getSpeed());
            break;
        case VAR_POSITION:
            tempMsg.writeUnsignedByte(TYPE_POSITION2D);
            tempMsg.writeFloat(v->getPosition().x());
            tempMsg.writeFloat(v->getPosition().y());
            break;
        case VAR_ANGLE:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(v->getLane().getShape().rotationDegreeAtLengthPosition(v->getPositionOnLane()));
            break;
        case VAR_ROAD_ID:
            tempMsg.writeUnsignedByte(TYPE_STRING);
            tempMsg.writeString(v->getLane().getEdge().getID());
            break;
        case VAR_LANE_ID:
            tempMsg.writeUnsignedByte(TYPE_STRING);
            tempMsg.writeString(v->getLane().getID());
            break;
        case VAR_LANE_INDEX: {
            const std::vector<MSLane*> &lanes = v->getLane().getEdge().getLanes();
            tempMsg.writeUnsignedByte(TYPE_INTEGER);
            tempMsg.writeInt((int)(std::distance(lanes.begin(), std::find(lanes.begin(), lanes.end(), &v->getLane()))));
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
            tempMsg.writeFloat(v->getPositionOnLane());
            break;
        case VAR_CO2EMISSION:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(HelpersHBEFA::computeCO2(v->getVehicleType().getEmissionClass(), v->getSpeed(), v->getPreDawdleAcceleration()));
            break;
        case VAR_COEMISSION:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(HelpersHBEFA::computeCO(v->getVehicleType().getEmissionClass(), v->getSpeed(), v->getPreDawdleAcceleration()));
            break;
        case VAR_HCEMISSION:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(HelpersHBEFA::computeHC(v->getVehicleType().getEmissionClass(), v->getSpeed(), v->getPreDawdleAcceleration()));
            break;
        case VAR_PMXEMISSION:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(HelpersHBEFA::computePMx(v->getVehicleType().getEmissionClass(), v->getSpeed(), v->getPreDawdleAcceleration()));
            break;
        case VAR_NOXEMISSION:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(HelpersHBEFA::computeNOx(v->getVehicleType().getEmissionClass(), v->getSpeed(), v->getPreDawdleAcceleration()));
            break;
        case VAR_FUELCONSUMPTION:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(HelpersHBEFA::computeFuel(v->getVehicleType().getEmissionClass(), v->getSpeed(), v->getPreDawdleAcceleration()));
            break;
        case VAR_NOISEEMISSION:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(HelpersHarmonoise::computeNoise(v->getVehicleType().getEmissionClass(), v->getSpeed(), v->getPreDawdleAcceleration()));
            break;
        case VAR_EDGE_TRAVELTIME: {
            if (inputStorage.readUnsignedByte()!=TYPE_COMPOUND) {
                TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Retrieval of travel time requires a compund object.", outputStorage);
                return false;
            }
            if (inputStorage.readInt()!=2) {
                TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Retrieval of travel time requires time, and edge as parameter.", outputStorage);
                return false;
            }
            // time
            if (inputStorage.readUnsignedByte()!=TYPE_INTEGER) {
                TraCIServerAPIHelper::writeStatusCmd(CMD_GET_VEHICLE_VARIABLE, RTYPE_ERR, "Retrieval of travel time requires the referenced time as first parameter.", outputStorage);
                return false;
            }
            SUMOTime time = inputStorage.readInt();
            // edge
            if (inputStorage.readUnsignedByte()!=TYPE_STRING) {
                TraCIServerAPIHelper::writeStatusCmd(CMD_GET_VEHICLE_VARIABLE, RTYPE_ERR, "Retrieval of travel time requires the referenced edge as second parameter.", outputStorage);
                return false;
            }
            std::string edgeID = inputStorage.readString();
            MSEdge *edge = MSEdge::dictionary(edgeID);
            if(edge==0) {
                TraCIServerAPIHelper::writeStatusCmd(CMD_GET_VEHICLE_VARIABLE, RTYPE_ERR, "Referenced edge '" + edgeID + "' is not known.", outputStorage);
                return false;
            }
            // retrieve
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            SUMOReal value;
            if(!v->getWeightsStorage()->retrieveExistingTravelTime(edge, 0, time, value)) {
                tempMsg.writeFloat(-1);
            } else {
                tempMsg.writeFloat(value);
            }
            
                                  }
            break;
        case VAR_EDGE_EFFORT: {
            if (inputStorage.readUnsignedByte()!=TYPE_COMPOUND) {
                TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Retrieval of travel time requires a compund object.", outputStorage);
                return false;
            }
            if (inputStorage.readInt()!=2) {
                TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Retrieval of travel time requires time, and edge as parameter.", outputStorage);
                return false;
            }
            // time
            if (inputStorage.readUnsignedByte()!=TYPE_INTEGER) {
                TraCIServerAPIHelper::writeStatusCmd(CMD_GET_VEHICLE_VARIABLE, RTYPE_ERR, "Retrieval of effort requires the referenced time as first parameter.", outputStorage);
                return false;
            }
            SUMOTime time = inputStorage.readInt();
            // edge
            if (inputStorage.readUnsignedByte()!=TYPE_STRING) {
                TraCIServerAPIHelper::writeStatusCmd(CMD_GET_VEHICLE_VARIABLE, RTYPE_ERR, "Retrieval of effort requires the referenced edge as second parameter.", outputStorage);
                return false;
            }
            std::string edgeID = inputStorage.readString();
            MSEdge *edge = MSEdge::dictionary(edgeID);
            if(edge==0) {
                TraCIServerAPIHelper::writeStatusCmd(CMD_GET_VEHICLE_VARIABLE, RTYPE_ERR, "Referenced edge '" + edgeID + "' is not known.", outputStorage);
                return false;
            }
            // retrieve
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            SUMOReal value;
            if(!v->getWeightsStorage()->retrieveExistingEffort(edge, 0, time, value)) {
                tempMsg.writeFloat(-1);
            } else {
                tempMsg.writeFloat(value);
            }
            
                                  }
            break;
        default:
            break;
        }
    }
    TraCIServerAPIHelper::writeStatusCmd(CMD_GET_VEHICLE_VARIABLE, RTYPE_OK, warning, outputStorage);
    // send response
    outputStorage.writeUnsignedByte(0); // command length -> extended
    outputStorage.writeInt(1 + 4 + tempMsg.size());
    outputStorage.writeStorage(tempMsg);
    return true;
}


bool
TraCIServerAPI_Vehicle::processSet(tcpip::Storage &inputStorage,
                                   tcpip::Storage &outputStorage) throw(TraCIException) {
    string warning = ""; // additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    if (variable!=CMD_SETMAXSPEED&&variable!=CMD_STOP&&variable!=CMD_CHANGELANE
            &&variable!=CMD_SLOWDOWN&&/*variable!=CMD_CHANGEROUTE&&*/variable!=CMD_CHANGETARGET
            &&variable!=VAR_ROUTE_ID&&variable!=VAR_ROUTE
            &&variable!=VAR_EDGE_TRAVELTIME&&variable!=VAR_EDGE_EFFORT
            ) {
        TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Unsupported variable specified", outputStorage);
        return false;
    }
    // id
    string id = inputStorage.readString();
    MSVehicle *v = MSNet::getInstance()->getVehicleControl().getVehicle(id);
    if (v==0) {
        TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Vehicle '" + id + "' is not known", outputStorage);
        return false;
    }
    // process
    int valueDataType = inputStorage.readUnsignedByte();
    switch (variable) {
    case CMD_SETMAXSPEED: {
        if (valueDataType!=TYPE_FLOAT) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "The speed must be given as a float.", outputStorage);
            return false;
        }
        SUMOReal maxspeed = inputStorage.readFloat();
        if (maxspeed>=0.0) {
            v->setIndividualMaxSpeed(maxspeed);
        } else {
            v->unsetIndividualMaxSpeed();
        }
    }
    break;
    case CMD_STOP: {
        if (valueDataType!=TYPE_COMPOUND) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Stop needs a compound object description.", outputStorage);
            return false;
        }
        if (inputStorage.readInt()!=4) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Stop needs a compound object description of four items.", outputStorage);
            return false;
        }
        // read road map position
        valueDataType = inputStorage.readUnsignedByte();
        if (valueDataType!=TYPE_STRING) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "The first stop parameter must be the edge id given as a string.", outputStorage);
            return false;
        }
        std::string roadId = inputStorage.readString();
        valueDataType = inputStorage.readUnsignedByte();
        if (valueDataType!=TYPE_FLOAT) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "The second stop parameter must be the position along the edge given as a float.", outputStorage);
            return false;
        }
        SUMOReal pos = inputStorage.readFloat();
        valueDataType = inputStorage.readUnsignedByte();
        if (valueDataType!=TYPE_BYTE) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "The third stop parameter must be the lane index given as a byte.", outputStorage);
            return false;
        }
        char laneIndex = inputStorage.readByte();
        // waitTime
        valueDataType = inputStorage.readUnsignedByte();
        if (valueDataType!=TYPE_FLOAT) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "The fourth stop parameter must be the waiting time given as a float.", outputStorage);
            return false;
        }
        SUMOReal waitTime = inputStorage.readFloat();
        // check
        if (pos < 0) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Position on lane must not be negative", outputStorage);
            return false;
        }
        // get the actual lane that is referenced by laneIndex
        MSEdge* road = MSEdge::dictionary(roadId);
        if (road == 0) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Unable to retrieve road with given id", outputStorage);
            return false;
        }
        const std::vector<MSLane*> &allLanes = road->getLanes();
        if (laneIndex >= allLanes.size()) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "No lane existing with such id on the given road", outputStorage);
            return false;
        }
        // Forward command to vehicle
        if (!v->addTraciStop(allLanes[laneIndex], pos, 0, waitTime)) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Vehicle is too close or behind the stop on " + allLanes[laneIndex]->getID(), outputStorage);
            return false;
        }
    }
    break;
    case CMD_CHANGELANE: {
        if (valueDataType!=TYPE_COMPOUND) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Lane change needs a compound object description.", outputStorage);
            return false;
        }
        if (inputStorage.readInt()!=2) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Lane change needs a compound object description of two items.", outputStorage);
            return false;
        }
        // Lane ID
        valueDataType = inputStorage.readUnsignedByte();
        if (valueDataType!=TYPE_BYTE) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "The first lane change parameter must be the lane index given as a byte.", outputStorage);
            return false;
        }
        char laneIndex = inputStorage.readByte();
        // stickyTime
        valueDataType = inputStorage.readUnsignedByte();
        if (valueDataType!=TYPE_FLOAT) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "The second lane change parameter must be the duration given as a float.", outputStorage);
            return false;
        }
        SUMOReal stickyTime = inputStorage.readFloat();
        if ((laneIndex < 0) || (laneIndex >= v->getEdge()->getLanes().size())) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "No lane existing with given id on the current road", outputStorage);
            return false;
        }
        // Forward command to vehicle
        v->startLaneChange(static_cast<unsigned>(laneIndex), static_cast<SUMOTime>(stickyTime));
    }
    break;
    case CMD_SLOWDOWN: {
        if (valueDataType!=TYPE_COMPOUND) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Slow down needs a compound object description.", outputStorage);
            return false;
        }
        if (inputStorage.readInt()!=2) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Slow down needs a compound object description of two items.", outputStorage);
            return false;
        }
        valueDataType = inputStorage.readUnsignedByte();
        if (valueDataType!=TYPE_FLOAT) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "The first slow down parameter must be the speed given as a float.", outputStorage);
            return false;
        }
        SUMOReal newSpeed = MAX2(inputStorage.readFloat(), 0.0f);
        valueDataType = inputStorage.readUnsignedByte();
        if (valueDataType!=TYPE_FLOAT) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "The second slow down parameter must be the duration given as a float.", outputStorage);
            return false;
        }
        // time interval
        SUMOReal duration = inputStorage.readFloat();
        if (duration <= 0) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Invalid time interval", outputStorage);
            return false;
        }
        if (!v->startSpeedAdaption(newSpeed, static_cast<SUMOTime>(duration), MSNet::getInstance()->getCurrentTimeStep())) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Could not slow down", outputStorage);
            return false;
        }
    }
    break;
    case CMD_CHANGEROUTE:
        break;
    case CMD_CHANGETARGET: {
        if (valueDataType!=TYPE_STRING) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Change target requires a string containing the id of the new destination edge as parameter.", outputStorage);
            return false;
        }
        std::string edgeID = inputStorage.readString();
        const MSEdge* destEdge = MSEdge::dictionary(edgeID);
        if (destEdge == 0) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_CHANGETARGET, RTYPE_ERR, "Can not retrieve road with ID " + edgeID, outputStorage);
            return false;
        }
        // build a new route between the vehicle's current edge and destination edge
        MSEdgeVector newRoute;
        const MSEdge* currentEdge = v->getEdge();
        DijkstraRouterTT_Direct<MSEdge, SUMOVehicle, prohibited_withRestrictions<MSEdge, SUMOVehicle> > router(MSEdge::dictSize(), true, &MSEdge::getVehicleEffort);
        router.compute(currentEdge, destEdge, (const MSVehicle* const) v, MSNet::getInstance()->getCurrentTimeStep(), newRoute);
        // replace the vehicle's route by the new one
        if (!v->replaceRoute(newRoute, MSNet::getInstance()->getCurrentTimeStep())) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Route replacement failed for " + v->getID(), outputStorage);
            return false;
        }
    }
    break;
    case VAR_ROUTE_ID: {
        if (valueDataType!=TYPE_STRING) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "The route id must be given as a string.", outputStorage);
            return false;
        }
        std::string rid = inputStorage.readString();
        const MSRoute *r = MSRoute::dictionary(rid);
        if (r==0) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "The route '" + rid + "' is not known.", outputStorage);
            return false;
        }
        if (!v->replaceRoute(r->getEdges(), MSNet::getInstance()->getCurrentTimeStep())) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Route replacement failed for " + v->getID(), outputStorage);
            return false;
        }
    }
    break;
    case VAR_ROUTE: {
        if (valueDataType!=TYPE_STRINGLIST) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "A route must be defined as a list of edge ids.", outputStorage);
            return false;
        }
        std::vector<std::string> edgeIDs = inputStorage.readStringList();
        std::vector<const MSEdge*> edges;
        MSEdge::parseEdgesList(edgeIDs, edges, "<unknown>");
        if (!v->replaceRoute(edges, MSNet::getInstance()->getCurrentTimeStep())) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Route replacement failed for " + v->getID(), outputStorage);
            return false;
        }
    }
    break;
    case VAR_EDGE_TRAVELTIME: {
        if (valueDataType!=TYPE_COMPOUND) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting travel time requires a compund object.", outputStorage);
            return false;
        }
        if (inputStorage.readInt()!=4) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting travel time requires begin time, end time, edge, and value as parameter.", outputStorage);
            return false;
        }
        // begin time
        if (inputStorage.readUnsignedByte()!=TYPE_INTEGER) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting travel time requires the begin time as first parameter.", outputStorage);
            return false;
        }
        SUMOTime begTime = inputStorage.readInt();
        // begin time
        if (inputStorage.readUnsignedByte()!=TYPE_INTEGER) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting travel time requires the end time as second parameter.", outputStorage);
            return false;
        }
        SUMOTime endTime = inputStorage.readInt();
        // edge
        if (inputStorage.readUnsignedByte()!=TYPE_STRING) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting travel time requires the referenced edge as third parameter.", outputStorage);
            return false;
        }
        std::string edgeID = inputStorage.readString();
        MSEdge *edge = MSEdge::dictionary(edgeID);
        if(edge==0) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Referenced edge '" + edgeID + "' is not known.", outputStorage);
            return false;
        }
        // value
        if (inputStorage.readUnsignedByte()!=TYPE_FLOAT) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting travel time requires the travel time as fourth parameter.", outputStorage);
            return false;
        }
        SUMOReal value = inputStorage.readFloat();
        // retrieve
        v->getWeightsStorage()->addTravelTime(edge, begTime, endTime, value);
                                  }
        break;
    case VAR_EDGE_EFFORT: {
        if (valueDataType!=TYPE_COMPOUND) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting effort requires a compund object.", outputStorage);
            return false;
        }
        if (inputStorage.readInt()!=4) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting effort requires begin time, end time, edge, and value as parameter.", outputStorage);
            return false;
        }
        // begin time
        if (inputStorage.readUnsignedByte()!=TYPE_INTEGER) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting effort requires the begin time as first parameter.", outputStorage);
            return false;
        }
        SUMOTime begTime = inputStorage.readInt();
        // begin time
        if (inputStorage.readUnsignedByte()!=TYPE_INTEGER) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting effort requires the end time as second parameter.", outputStorage);
            return false;
        }
        SUMOTime endTime = inputStorage.readInt();
        // edge
        if (inputStorage.readUnsignedByte()!=TYPE_STRING) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting effort requires the referenced edge as third parameter.", outputStorage);
            return false;
        }
        std::string edgeID = inputStorage.readString();
        MSEdge *edge = MSEdge::dictionary(edgeID);
        if(edge==0) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Referenced edge '" + edgeID + "' is not known.", outputStorage);
            return false;
        }
        // value
        if (inputStorage.readUnsignedByte()!=TYPE_FLOAT) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting effort requires the travel time as fourth parameter.", outputStorage);
            return false;
        }
        SUMOReal value = inputStorage.readFloat();
        // retrieve
        v->getWeightsStorage()->addEffort(edge, begTime, endTime, value);
                                  }
        break;
    default:
        break;
    }
    TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_OK, warning, outputStorage);
    return true;
}



/****************************************************************************/

