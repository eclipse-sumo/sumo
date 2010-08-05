/****************************************************************************/
/// @file    TraCIServerAPI_Edge.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// APIs for getting/setting edge values via TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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
#include "TraCIServerAPI_Edge.h"
#include <microsim/MSEdgeWeightsStorage.h>
#include <utils/common/HelpersHarmonoise.h>

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
TraCIServerAPI_Edge::processGet(TraCIServer &server, tcpip::Storage &inputStorage,
                                tcpip::Storage &outputStorage) {
    std::string warning = ""; // additional description for response
    // variable & id
    int variable = inputStorage.readUnsignedByte();
    std::string id = inputStorage.readString();
    // check variable
    if (variable!=ID_LIST&&variable!=VAR_EDGE_TRAVELTIME&&variable!=VAR_EDGE_EFFORT&&variable!=VAR_CURRENT_TRAVELTIME
            &&variable!=LANE_ALLOWED&&variable!=LANE_DISALLOWED
            &&variable!=VAR_CO2EMISSION&&variable!=VAR_COEMISSION&&variable!=VAR_HCEMISSION&&variable!=VAR_PMXEMISSION
            &&variable!=VAR_NOXEMISSION&&variable!=VAR_FUELCONSUMPTION&&variable!=VAR_NOISEEMISSION
            &&variable!=LAST_STEP_VEHICLE_NUMBER&&variable!=LAST_STEP_MEAN_SPEED&&variable!=LAST_STEP_OCCUPANCY
            &&variable!=LAST_STEP_VEHICLE_HALTING_NUMBER&&variable!=LAST_STEP_LENGTH
            &&variable!=LAST_STEP_VEHICLE_ID_LIST) {
        server.writeStatusCmd(CMD_GET_EDGE_VARIABLE, RTYPE_ERR, "Get Edge Variable: unsupported variable specified", outputStorage);
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
            server.writeStatusCmd(CMD_GET_EDGE_VARIABLE, RTYPE_ERR, "Edge '" + id + "' is not known", outputStorage);
            return false;
        }
        switch (variable) {
        case VAR_EDGE_TRAVELTIME: {
            // time
            if (inputStorage.readUnsignedByte()!=TYPE_INTEGER) {
                server.writeStatusCmd(CMD_GET_EDGE_VARIABLE, RTYPE_ERR, "The message must contain the time definition.", outputStorage);
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
                server.writeStatusCmd(CMD_GET_EDGE_VARIABLE, RTYPE_ERR, "The message must contain the time definition.", outputStorage);
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
        case VAR_CURRENT_TRAVELTIME:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(e->getCurrentTravelTime());
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
        case VAR_CO2EMISSION: {
            SUMOReal sum = 0;
            const std::vector<MSLane*> &lanes = e->getLanes();
            for (std::vector<MSLane*>::const_iterator i=lanes.begin(); i!=lanes.end(); ++i) {
                sum += (*i)->getHBEFA_CO2Emissions();
            }
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(sum);
        }
        break;
        case VAR_COEMISSION: {
            SUMOReal sum = 0;
            const std::vector<MSLane*> &lanes = e->getLanes();
            for (std::vector<MSLane*>::const_iterator i=lanes.begin(); i!=lanes.end(); ++i) {
                sum += (*i)->getHBEFA_COEmissions();
            }
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(sum);
        }
        break;
        case VAR_HCEMISSION: {
            SUMOReal sum = 0;
            const std::vector<MSLane*> &lanes = e->getLanes();
            for (std::vector<MSLane*>::const_iterator i=lanes.begin(); i!=lanes.end(); ++i) {
                sum += (*i)->getHBEFA_HCEmissions();
            }
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(sum);
        }
        break;
        case VAR_PMXEMISSION: {
            SUMOReal sum = 0;
            const std::vector<MSLane*> &lanes = e->getLanes();
            for (std::vector<MSLane*>::const_iterator i=lanes.begin(); i!=lanes.end(); ++i) {
                sum += (*i)->getHBEFA_PMxEmissions();
            }
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(sum);
        }
        break;
        case VAR_NOXEMISSION: {
            SUMOReal sum = 0;
            const std::vector<MSLane*> &lanes = e->getLanes();
            for (std::vector<MSLane*>::const_iterator i=lanes.begin(); i!=lanes.end(); ++i) {
                sum += (*i)->getHBEFA_NOxEmissions();
            }
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(sum);
        }
        break;
        case VAR_FUELCONSUMPTION: {
            SUMOReal sum = 0;
            const std::vector<MSLane*> &lanes = e->getLanes();
            for (std::vector<MSLane*>::const_iterator i=lanes.begin(); i!=lanes.end(); ++i) {
                sum += (*i)->getHBEFA_FuelConsumption();
            }
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(sum);
        }
        break;
        case VAR_NOISEEMISSION: {
            SUMOReal sum = 0;
            const std::vector<MSLane*> &lanes = e->getLanes();
            for (std::vector<MSLane*>::const_iterator i=lanes.begin(); i!=lanes.end(); ++i) {
                sum += (SUMOReal) pow(10., ((*i)->getHarmonoise_NoiseEmissions()/10.));
            }
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            if (sum!=0) {
                tempMsg.writeFloat(HelpersHarmonoise::sum(sum));
            } else {
                tempMsg.writeFloat(0);
            }
        }
        break;
        case LAST_STEP_VEHICLE_NUMBER: {
            int sum = 0;
            const std::vector<MSLane*> &lanes = e->getLanes();
            for (std::vector<MSLane*>::const_iterator i=lanes.begin(); i!=lanes.end(); ++i) {
                sum += (*i)->getVehicleNumber();
            }
            tempMsg.writeUnsignedByte(TYPE_INTEGER);
            tempMsg.writeInt(sum);
        }
        break;
        case LAST_STEP_MEAN_SPEED: {
            SUMOReal sum = 0;
            const std::vector<MSLane*> &lanes = e->getLanes();
            for (std::vector<MSLane*>::const_iterator i=lanes.begin(); i!=lanes.end(); ++i) {
                sum += (*i)->getMeanSpeed();
            }
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(sum / (SUMOReal) lanes.size());
        }
        break;
        case LAST_STEP_OCCUPANCY: {
            SUMOReal sum = 0;
            const std::vector<MSLane*> &lanes = e->getLanes();
            for (std::vector<MSLane*>::const_iterator i=lanes.begin(); i!=lanes.end(); ++i) {
                sum += (*i)->getOccupancy();
            }
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(sum / (SUMOReal) lanes.size());
        }
        break;
        case LAST_STEP_VEHICLE_HALTING_NUMBER: {
            int halting = 0;
            const std::vector<MSLane*> &lanes = e->getLanes();
            for (std::vector<MSLane*>::const_iterator i=lanes.begin(); i!=lanes.end(); ++i) {
                const std::deque<MSVehicle*> &vehs = (*i)->getVehiclesSecure();
                for (std::deque<MSVehicle*>::const_iterator j=vehs.begin(); j!=vehs.end(); ++j) {
                    if ((*j)->getSpeed()<0.1) {
                        ++halting;
                    }
                }
                (*i)->releaseVehicles();
            }
            tempMsg.writeUnsignedByte(TYPE_INTEGER);
            tempMsg.writeInt(halting);
        }
        break;
        case LAST_STEP_LENGTH: {
            SUMOReal lengthSum = 0;
            int noVehicles = 0;
            const std::vector<MSLane*> &lanes = e->getLanes();
            for (std::vector<MSLane*>::const_iterator i=lanes.begin(); i!=lanes.end(); ++i) {
                const std::deque<MSVehicle*> &vehs = (*i)->getVehiclesSecure();
                for (std::deque<MSVehicle*>::const_iterator j=vehs.begin(); j!=vehs.end(); ++j) {
                    lengthSum += (*j)->getVehicleType().getLength();
                }
                noVehicles += (int) vehs.size();
                (*i)->releaseVehicles();
            }
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            if (noVehicles==0) {
                tempMsg.writeFloat(0);
            } else {
                tempMsg.writeFloat(lengthSum / (SUMOReal) noVehicles);
            }
        }
        break;
        default:
            break;
        }
    }
        server.writeStatusCmd(CMD_GET_EDGE_VARIABLE, RTYPE_OK, warning, outputStorage);
    // send response
    outputStorage.writeUnsignedByte(0); // command length -> extended
    outputStorage.writeInt(1 + 4 + tempMsg.size());
    outputStorage.writeStorage(tempMsg);
    return true;
}


bool
TraCIServerAPI_Edge::processSet(TraCIServer &server, tcpip::Storage &inputStorage,
                                tcpip::Storage &outputStorage) {
    std::string warning = ""; // additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    if (variable!=VAR_EDGE_TRAVELTIME&&variable!=VAR_EDGE_EFFORT&&variable!=VAR_MAXSPEED) {
        server.writeStatusCmd(CMD_SET_EDGE_VARIABLE, RTYPE_ERR, "Change Edge State: unsupported variable specified", outputStorage);
        return false;
    }
    // id
    std::string id = inputStorage.readString();
    MSEdge *e = MSEdge::dictionary(id);
    if (e==0) {
        server.writeStatusCmd(CMD_SET_EDGE_VARIABLE, RTYPE_ERR, "Edge '" + id + "' is not known", outputStorage);
        return false;
    }
    // process
    int valueDataType = inputStorage.readUnsignedByte();
    switch (variable) {
    case LANE_ALLOWED: {
        if (inputStorage.readUnsignedByte()!=TYPE_STRINGLIST) {
            server.writeStatusCmd(CMD_GET_EDGE_VARIABLE, RTYPE_ERR, "Allowed vehicle classes must be given as a list of strings.", outputStorage);
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
            server.writeStatusCmd(CMD_GET_EDGE_VARIABLE, RTYPE_ERR, "Not allowed vehicle classes must be given as a list of strings.", outputStorage);
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
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting travel time requires a compound object.", outputStorage);
            return false;
        }
        if (inputStorage.readInt()!=3) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting travel time requires begin time, end time, and value as parameter.", outputStorage);
            return false;
        }
        // begin
        if (inputStorage.readUnsignedByte()!=TYPE_INTEGER) {
            server.writeStatusCmd(CMD_SET_EDGE_VARIABLE, RTYPE_ERR, "The first variable must be the begin time given as int.", outputStorage);
            return false;
        }
        SUMOTime begTime = inputStorage.readInt();
        // end
        if (inputStorage.readUnsignedByte()!=TYPE_INTEGER) {
            server.writeStatusCmd(CMD_SET_EDGE_VARIABLE, RTYPE_ERR, "The second variable must be the end time given as int.", outputStorage);
            return false;
        }
        SUMOTime endTime = inputStorage.readInt();
        // value
        if (inputStorage.readUnsignedByte()!=TYPE_FLOAT) {
            server.writeStatusCmd(CMD_SET_EDGE_VARIABLE, RTYPE_ERR, "The second variable must be the value given as float", outputStorage);
            return false;
        }
        SUMOReal value = inputStorage.readFloat();
        // set
        MSNet::getInstance()->getWeightsStorage().addTravelTime(e, begTime, endTime, value);
    }
    break;
    case VAR_EDGE_EFFORT: {
        if (valueDataType!=TYPE_COMPOUND) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting effort requires a compound object.", outputStorage);
            return false;
        }
        if (inputStorage.readInt()!=3) {
            server.writeStatusCmd(CMD_SET_VEHICLE_VARIABLE, RTYPE_ERR, "Setting effort requires begin time, end time, and value as parameter.", outputStorage);
            return false;
        }
        // begin
        if (inputStorage.readUnsignedByte()!=TYPE_INTEGER) {
            server.writeStatusCmd(CMD_SET_EDGE_VARIABLE, RTYPE_ERR, "The first variable must be the begin time given as int.", outputStorage);
            return false;
        }
        SUMOTime begTime = inputStorage.readInt();
        // end
        if (inputStorage.readUnsignedByte()!=TYPE_INTEGER) {
            server.writeStatusCmd(CMD_SET_EDGE_VARIABLE, RTYPE_ERR, "The second variable must be the end time given as int.", outputStorage);
            return false;
        }
        SUMOTime endTime = inputStorage.readInt();
        // value
        if (inputStorage.readUnsignedByte()!=TYPE_FLOAT) {
            server.writeStatusCmd(CMD_SET_EDGE_VARIABLE, RTYPE_ERR, "The second variable must be the value given as float", outputStorage);
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
            server.writeStatusCmd(CMD_SET_EDGE_VARIABLE, RTYPE_ERR, "The speed must be given as a float.", outputStorage);
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
    server.writeStatusCmd(CMD_SET_EDGE_VARIABLE, RTYPE_OK, warning, outputStorage);
    return true;
}



/****************************************************************************/

