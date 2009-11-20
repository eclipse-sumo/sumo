/****************************************************************************/
/// @file    TraCIServerAPI_Edge.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: TraCIServerAPI_Edge.cpp 6907 2009-03-13 12:13:38Z dkrajzew $
///
// APIs for getting/setting edge values via TraCI
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

#include <utils/common/StdDefs.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include "TraCIConstants.h"
#include "TraCIServerAPIHelper.h"
#include "TraCIServerAPI_Edge.h"
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
TraCIServerAPI_Edge::processGet(tcpip::Storage &inputStorage,
                                tcpip::Storage &outputStorage) throw(TraCIException) {
    string warning = ""; // additional description for response
    // variable & id
    int variable = inputStorage.readUnsignedByte();
    string id = inputStorage.readString();
    // check variable
    if (variable!=ID_LIST&&variable!=VAR_EDGE_TRAVELTIME&&variable!=VAR_EDGE_EFFORT&&variable!=VAR_CURRENT_TRAVELTIME
            &&variable!=LANE_ALLOWED&&variable!=LANE_DISALLOWED
            &&variable!=LAST_STEP_VEHICLE_ID_LIST) {
        TraCIServerAPIHelper::writeStatusCmd(CMD_GET_EDGE_VARIABLE, RTYPE_ERR, "Unsupported variable specified", outputStorage);
        return false;
    }
    // begin response building
    Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(RESPONSE_GET_EDGE_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);
    // process request
    if (variable==ID_LIST) {
        std::vector<std::string> ids;
        MSEdge::insertIDs(ids);
        tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
        tempMsg.writeStringList(ids);
    } else {
        MSEdge *e = MSEdge::dictionary(id);
        if (e==0) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_GET_EDGE_VARIABLE, RTYPE_ERR, "Edge '" + id + "' is not known", outputStorage);
            return false;
        }
        switch (variable) {
        case VAR_EDGE_TRAVELTIME: {
            // time
            if (inputStorage.readUnsignedByte()!=TYPE_INTEGER) {
                TraCIServerAPIHelper::writeStatusCmd(CMD_GET_EDGE_VARIABLE, RTYPE_ERR, "The message must contain the time definition.", outputStorage);
                return false;
            }
            SUMOTime time = inputStorage.readInt();
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            SUMOReal value;
            if (!MSNet::getInstance()->getWeightsStorage().retrieveExistingTravelTime(e, 0, time, value)) {
                tempMsg.writeFloat(-1);
            } else {
                tempMsg.writeFloat(value);
            }
        }
        break;
        case VAR_EDGE_EFFORT: {
            // time
            if (inputStorage.readUnsignedByte()!=TYPE_INTEGER) {
                TraCIServerAPIHelper::writeStatusCmd(CMD_GET_EDGE_VARIABLE, RTYPE_ERR, "The message must contain the time definition.", outputStorage);
                return false;
            }
            SUMOTime time = inputStorage.readInt();
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            SUMOReal value;
            if (!MSNet::getInstance()->getWeightsStorage().retrieveExistingEffort(e, 0, time, value)) {
                tempMsg.writeFloat(-1);
            } else {
                tempMsg.writeFloat(value);
            }
        }
        break;
        case VAR_CURRENT_TRAVELTIME: {
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(e->getCurrentTravelTime());
        }
        break;
        case LAST_STEP_VEHICLE_ID_LIST: {
            std::vector<std::string> vehIDs;
            const std::vector<MSLane*> &lanes = e->getLanes();
            for (std::vector<MSLane*>::const_iterator i=lanes.begin(); i!=lanes.end(); ++i) {
                const std::deque<MSVehicle*> &vehs = (*i)->getVehiclesSecure();
                for (std::deque<MSVehicle*>::const_iterator j=vehs.begin(); j!=vehs.end(); ++j) {
                    vehIDs.push_back((*j)->getID());
                }
                (*i)->releaseVehicles();
            }
            tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
            tempMsg.writeStringList(vehIDs);
        }
        break;
        default:
            break;
        }
    }
    TraCIServerAPIHelper::writeStatusCmd(CMD_GET_EDGE_VARIABLE, RTYPE_OK, warning, outputStorage);
    // send response
    outputStorage.writeUnsignedByte(0); // command length -> extended
    outputStorage.writeInt(1 + 4 + tempMsg.size());
    outputStorage.writeStorage(tempMsg);
    return true;
}


bool
TraCIServerAPI_Edge::processSet(tcpip::Storage &inputStorage,
                                tcpip::Storage &outputStorage) throw(TraCIException) {
    string warning = ""; // additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    if (variable!=VAR_EDGE_TRAVELTIME&&variable!=VAR_EDGE_EFFORT&&variable!=VAR_MAXSPEED) {
        TraCIServerAPIHelper::writeStatusCmd(CMD_SET_EDGE_VARIABLE, RTYPE_ERR, "Unsupported variable specified", outputStorage);
        return false;
    }
    // id
    string id = inputStorage.readString();
    MSEdge *e = MSEdge::dictionary(id);
    if (e==0) {
        TraCIServerAPIHelper::writeStatusCmd(CMD_SET_EDGE_VARIABLE, RTYPE_ERR, "Edge '" + id + "' is not known", outputStorage);
        return false;
    }
    // process
    int valueDataType = inputStorage.readUnsignedByte();
    switch (variable) {
    case LANE_ALLOWED: {
        if (inputStorage.readUnsignedByte()!=TYPE_STRINGLIST) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_GET_EDGE_VARIABLE, RTYPE_ERR, "Allowed vehicle classes must be given as a list of strings.", outputStorage);
            return false;
        }
        std::vector<SUMOVehicleClass> allowed;
        parseVehicleClasses(inputStorage.readStringList(), allowed);
        const std::vector<MSLane*> &lanes = e->getLanes();
        for (std::vector<MSLane*>::const_iterator i=lanes.begin(); i!=lanes.end(); ++i) {
            (*i)->setAllowedClasses(allowed);
        }
        e->rebuildAllowedLanes();
    }
    break;
    case LANE_DISALLOWED: {
        // time
        if (inputStorage.readUnsignedByte()!=TYPE_STRINGLIST) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_GET_EDGE_VARIABLE, RTYPE_ERR, "Not allowed vehicle classes must be given as a list of strings.", outputStorage);
            return false;
        }
        std::vector<SUMOVehicleClass> disallowed;
        parseVehicleClasses(inputStorage.readStringList(), disallowed);
        const std::vector<MSLane*> &lanes = e->getLanes();
        for (std::vector<MSLane*>::const_iterator i=lanes.begin(); i!=lanes.end(); ++i) {
            (*i)->setNotAllowedClasses(disallowed);
        }
        e->rebuildAllowedLanes();
    }
    break;
    case VAR_EDGE_TRAVELTIME: {
        if (valueDataType!=TYPE_COMPOUND) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting travel time requires a compund object.", outputStorage);
            return false;
        }
        if (inputStorage.readInt()!=3) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting travel time requires begin time, end time, and value as parameter.", outputStorage);
            return false;
        }
        // begin
        if (inputStorage.readUnsignedByte()!=TYPE_INTEGER) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_EDGE_VARIABLE, RTYPE_ERR, "The first variable must be the begin time given as int.", outputStorage);
            return false;
        }
        SUMOTime begTime = inputStorage.readInt();
        // end
        if (inputStorage.readUnsignedByte()!=TYPE_INTEGER) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_EDGE_VARIABLE, RTYPE_ERR, "The second variable must be the end time given as int.", outputStorage);
            return false;
        }
        SUMOTime endTime = inputStorage.readInt();
        // value
        if (inputStorage.readUnsignedByte()!=TYPE_FLOAT) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_EDGE_VARIABLE, RTYPE_ERR, "The second variable must be the value given as float", outputStorage);
            return false;
        }
        SUMOReal value = inputStorage.readFloat();
        // set
        MSNet::getInstance()->getWeightsStorage().addTravelTime(e, begTime, endTime, value);
    }
    break;
    case VAR_EDGE_EFFORT: {
        if (valueDataType!=TYPE_COMPOUND) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting effort requires a compund object.", outputStorage);
            return false;
        }
        if (inputStorage.readInt()!=3) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting effort requires begin time, end time, and value as parameter.", outputStorage);
            return false;
        }
        // begin
        if (inputStorage.readUnsignedByte()!=TYPE_INTEGER) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_EDGE_VARIABLE, RTYPE_ERR, "The first variable must be the begin time given as int.", outputStorage);
            return false;
        }
        SUMOTime begTime = inputStorage.readInt();
        // end
        if (inputStorage.readUnsignedByte()!=TYPE_INTEGER) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_EDGE_VARIABLE, RTYPE_ERR, "The second variable must be the end time given as int.", outputStorage);
            return false;
        }
        SUMOTime endTime = inputStorage.readInt();
        // value
        if (inputStorage.readUnsignedByte()!=TYPE_FLOAT) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_EDGE_VARIABLE, RTYPE_ERR, "The second variable must be the value given as float", outputStorage);
            return false;
        }
        SUMOReal value = inputStorage.readFloat();
        // set
        MSNet::getInstance()->getWeightsStorage().addEffort(e, begTime, endTime, value);
    }
    break;
    case VAR_MAXSPEED: {
        // speed
        if (valueDataType!=TYPE_FLOAT) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_SET_EDGE_VARIABLE, RTYPE_ERR, "The speed must be given as a float.", outputStorage);
            return false;
        }
        SUMOReal val = inputStorage.readFloat();
        const std::vector<MSLane*> &lanes = e->getLanes();
        for (std::vector<MSLane*>::const_iterator i=lanes.begin(); i!=lanes.end(); ++i) {
            (*i)->setMaxSpeed(val);
        }
    }
    break;
    default:
        break;
    }
    TraCIServerAPIHelper::writeStatusCmd(CMD_SET_EDGE_VARIABLE, RTYPE_OK, warning, outputStorage);
    return true;
}



/****************************************************************************/

