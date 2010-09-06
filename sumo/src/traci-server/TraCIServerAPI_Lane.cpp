/****************************************************************************/
/// @file    TraCIServerAPI_Lane.cpp
/// @author  Daniel Krajzewicz
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting lane values via TraCI
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

#include "TraCIConstants.h"
#include <microsim/MSLane.h>
#include "TraCIServerAPI_Lane.h"

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
TraCIServerAPI_Lane::processGet(TraCIServer &server, tcpip::Storage &inputStorage,
                                tcpip::Storage &outputStorage) {
    Storage tmpResult;
    std::string warning = "";	// additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    std::string id = inputStorage.readString();
    // check variable
    if (variable!=ID_LIST&&variable!=LANE_LINK_NUMBER&&variable!=LANE_EDGE_ID&&variable!=VAR_LENGTH
            &&variable!=VAR_MAXSPEED&&variable!=LANE_LINKS&&variable!=VAR_SHAPE
            &&variable!=VAR_CO2EMISSION&&variable!=VAR_COEMISSION&&variable!=VAR_HCEMISSION&&variable!=VAR_PMXEMISSION
            &&variable!=VAR_NOXEMISSION&&variable!=VAR_FUELCONSUMPTION&&variable!=VAR_NOISEEMISSION
            &&variable!=LAST_STEP_MEAN_SPEED&&variable!=LAST_STEP_VEHICLE_NUMBER
            &&variable!=LAST_STEP_VEHICLE_ID_LIST&&variable!=LAST_STEP_OCCUPANCY&&variable!=LAST_STEP_VEHICLE_HALTING_NUMBER
            &&variable!=LAST_STEP_LENGTH&&variable!=VAR_CURRENT_TRAVELTIME
            &&variable!=LANE_ALLOWED&&variable!=LANE_DISALLOWED) {
        server.writeStatusCmd(CMD_GET_LANE_VARIABLE, RTYPE_ERR, "Get Lane Variable: unsupported variable specified", outputStorage);
        return false;
    }
    // begin response building
    Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(RESPONSE_GET_LANE_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);
    if (variable==ID_LIST) {
        std::vector<std::string> ids;
        MSLane::insertIDs(ids);
        tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
        tempMsg.writeStringList(ids);
    } else {
        MSLane *lane = MSLane::dictionary(id);
        if (lane==0) {
            server.writeStatusCmd(CMD_GET_LANE_VARIABLE, RTYPE_ERR, "Lane '" + id + "' is not known", outputStorage);
            return false;
        }
        switch (variable) {
        case LANE_LINK_NUMBER:
            tempMsg.writeUnsignedByte(TYPE_UBYTE);
            tempMsg.writeUnsignedByte((int) lane->getLinkCont().size());
            break;
        case LANE_EDGE_ID:
            tempMsg.writeUnsignedByte(TYPE_STRING);
            tempMsg.writeString(lane->getEdge().getID());
            break;
        case VAR_LENGTH:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(lane->getLength());
            break;
        case VAR_MAXSPEED:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(lane->getMaxSpeed());
            break;
        case LANE_LINKS: {
            tempMsg.writeUnsignedByte(TYPE_COMPOUND);
            Storage tempContent;
            unsigned int cnt = 0;
            tempContent.writeUnsignedByte(TYPE_INTEGER);
            const MSLinkCont &links = lane->getLinkCont();
            tempContent.writeInt((int) links.size());
            ++cnt;
            for (MSLinkCont::const_iterator i=links.begin(); i!=links.end(); ++i) {
                MSLink *link = (*i);
                // approached non-internal lane (if any)
                tempContent.writeUnsignedByte(TYPE_STRING);
                tempContent.writeString(link->getLane()!=0 ? link->getLane()->getID() : "");
                ++cnt;
                // approached "via", internal lane (if any)
                tempContent.writeUnsignedByte(TYPE_STRING);
#ifdef HAVE_INTERNAL_LANES
                tempContent.writeString(link->getViaLane()!=0 ? link->getViaLane()->getID() : "");
#else
                tempContent.writeString("");
#endif
                ++cnt;
                // priority
                tempContent.writeUnsignedByte(TYPE_UBYTE);
                tempContent.writeUnsignedByte(link->havePriority() ? 1 : 0);
                ++cnt;
                // opened
                tempContent.writeUnsignedByte(TYPE_UBYTE);
                tempContent.writeUnsignedByte(link->opened(MSNet::getInstance()->getCurrentTimeStep(), MSNet::getInstance()->getCurrentTimeStep(), 0.) ? 1 : 0);
                ++cnt;
                // approaching foe
                tempContent.writeUnsignedByte(TYPE_UBYTE);
                tempContent.writeUnsignedByte(link->hasApproachingFoe(MSNet::getInstance()->getCurrentTimeStep(), MSNet::getInstance()->getCurrentTimeStep()) ? 1 : 0);
                ++cnt;
                // state (not implemented, yet)
                tempContent.writeUnsignedByte(TYPE_STRING);
                tempContent.writeString("");
                ++cnt;
                // direction (not implemented, yet)
                tempContent.writeUnsignedByte(TYPE_STRING);
                tempContent.writeString("");
                ++cnt;
                // length
                tempContent.writeUnsignedByte(TYPE_FLOAT);
                tempContent.writeFloat(link->getLength());
                ++cnt;
            }
            tempMsg.writeInt((int) cnt);
            tempMsg.writeStorage(tempContent);
        }
        break;
        case LANE_ALLOWED: {
            const std::vector<SUMOVehicleClass> &allowed = lane->getAllowedClasses();
            std::vector<std::string> allowedS;
            for (std::vector<SUMOVehicleClass>::const_iterator i=allowed.begin(); i!=allowed.end(); ++i) {
                allowedS.push_back(getVehicleClassName(*i));
            }
            tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
            tempMsg.writeStringList(allowedS);
        }
        case LANE_DISALLOWED: {
            const std::vector<SUMOVehicleClass> &disallowed = lane->getNotAllowedClasses();
            std::vector<std::string> disallowedS;
            for (std::vector<SUMOVehicleClass>::const_iterator i=disallowed.begin(); i!=disallowed.end(); ++i) {
                disallowedS.push_back(getVehicleClassName(*i));
            }
            tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
            tempMsg.writeStringList(disallowedS);
        }
        break;
        case VAR_SHAPE:
            tempMsg.writeUnsignedByte(TYPE_POLYGON);
            tempMsg.writeUnsignedByte(MIN2(static_cast<size_t>(255),lane->getShape().size()));
            for (int iPoint=0; iPoint < MIN2(static_cast<size_t>(255),lane->getShape().size()); ++iPoint) {
                tempMsg.writeFloat(lane->getShape()[iPoint].x());
                tempMsg.writeFloat(lane->getShape()[iPoint].y());
            }
            break;
        case VAR_CO2EMISSION:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(lane->getHBEFA_CO2Emissions());
            break;
        case VAR_COEMISSION:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(lane->getHBEFA_COEmissions());
            break;
        case VAR_HCEMISSION:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(lane->getHBEFA_HCEmissions());
            break;
        case VAR_PMXEMISSION:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(lane->getHBEFA_PMxEmissions());
            break;
        case VAR_NOXEMISSION:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(lane->getHBEFA_NOxEmissions());
            break;
        case VAR_FUELCONSUMPTION:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(lane->getHBEFA_FuelConsumption());
            break;
        case VAR_NOISEEMISSION:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(lane->getHarmonoise_NoiseEmissions());
            break;
        case LAST_STEP_VEHICLE_NUMBER:
            tempMsg.writeUnsignedByte(TYPE_INTEGER);
            tempMsg.writeInt((int) lane->getVehicleNumber());
            break;
        case LAST_STEP_MEAN_SPEED:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(lane->getMeanSpeed());
            break;
        case LAST_STEP_VEHICLE_ID_LIST: {
            std::vector<std::string> vehIDs;
            const std::deque<MSVehicle*> &vehs = lane->getVehiclesSecure();
            for (std::deque<MSVehicle*>::const_iterator j=vehs.begin(); j!=vehs.end(); ++j) {
                vehIDs.push_back((*j)->getID());
            }
            lane->releaseVehicles();
            tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
            tempMsg.writeStringList(vehIDs);
        }
        break;
        case LAST_STEP_OCCUPANCY:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat(lane->getOccupancy());
            break;
        case LAST_STEP_VEHICLE_HALTING_NUMBER: {
            int halting = 0;
            const std::deque<MSVehicle*> &vehs = lane->getVehiclesSecure();
            for (std::deque<MSVehicle*>::const_iterator j=vehs.begin(); j!=vehs.end(); ++j) {
                if ((*j)->getSpeed()<0.1) {
                    ++halting;
                }
            }
            lane->releaseVehicles();
            tempMsg.writeUnsignedByte(TYPE_INTEGER);
            tempMsg.writeInt(halting);
        }
        break;
        case LAST_STEP_LENGTH: {
            SUMOReal lengthSum = 0;
            const std::deque<MSVehicle*> &vehs = lane->getVehiclesSecure();
            for (std::deque<MSVehicle*>::const_iterator j=vehs.begin(); j!=vehs.end(); ++j) {
                lengthSum += (*j)->getVehicleType().getLength();
            }
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            if (vehs.size()==0) {
                tempMsg.writeFloat(0);
            } else {
                tempMsg.writeFloat(lengthSum / (SUMOReal) vehs.size());
            }
            lane->releaseVehicles();
        }
        break;
        case VAR_CURRENT_TRAVELTIME: {
            SUMOReal meanSpeed = lane->getMeanSpeed();
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            if (meanSpeed!=0) {
                tempMsg.writeFloat(lane->getLength() / meanSpeed);
            } else {
                tempMsg.writeFloat(1000000.);
            }
        }
        break;
        default:
            break;
        }
    }
        server.writeStatusCmd(CMD_GET_LANE_VARIABLE, RTYPE_OK, warning, outputStorage);
    // send response
    outputStorage.writeUnsignedByte(0); // command length -> extended
    outputStorage.writeInt(1 + 4 + tempMsg.size());
    outputStorage.writeStorage(tempMsg);
    return true;
}


bool
TraCIServerAPI_Lane::processSet(TraCIServer &server, tcpip::Storage &inputStorage,
                                tcpip::Storage &outputStorage) {
    std::string warning = ""; // additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    if (variable!=VAR_MAXSPEED&&variable!=VAR_LENGTH&&variable!=LANE_ALLOWED&&variable!=LANE_DISALLOWED) {
        server.writeStatusCmd(CMD_SET_LANE_VARIABLE, RTYPE_ERR, "Change Lane State: unsupported variable specified", outputStorage);
        return false;
    }
    // id
    std::string id = inputStorage.readString();
    MSLane *l = MSLane::dictionary(id);
    if (l==0) {
        server.writeStatusCmd(CMD_SET_LANE_VARIABLE, RTYPE_ERR, "Lane '" + id + "' is not known", outputStorage);
        return false;
    }
    // process
    int valueDataType = inputStorage.readUnsignedByte();
    switch (variable) {
    case VAR_MAXSPEED: {
        // speed
        if (valueDataType!=TYPE_FLOAT) {
            server.writeStatusCmd(CMD_SET_LANE_VARIABLE, RTYPE_ERR, "The speed must be given as a float.", outputStorage);
            return false;
        }
        SUMOReal val = inputStorage.readFloat();
        l->setMaxSpeed(val);
    }
    break;
    case VAR_LENGTH: {
        // speed
        if (valueDataType!=TYPE_FLOAT) {
            server.writeStatusCmd(CMD_SET_LANE_VARIABLE, RTYPE_ERR, "The length must be given as a float.", outputStorage);
            return false;
        }
        SUMOReal val = inputStorage.readFloat();
        l->setLength(val);
    }
    break;
    case LANE_ALLOWED: {
        if (valueDataType!=TYPE_STRINGLIST) {
            server.writeStatusCmd(CMD_SET_LANE_VARIABLE, RTYPE_ERR, "Allowed classes must be given as a list of strings.", outputStorage);
            return false;
        }
        std::vector<SUMOVehicleClass> allowed;
        parseVehicleClasses(inputStorage.readStringList(), allowed);
        l->setAllowedClasses(allowed);
        l->getEdge().rebuildAllowedLanes();
    }
    break;
    case LANE_DISALLOWED: {
        if (valueDataType!=TYPE_STRINGLIST) {
            server.writeStatusCmd(CMD_SET_LANE_VARIABLE, RTYPE_ERR, "Not allowed classes must be given as a list of strings.", outputStorage);
            return false;
        }
        std::vector<SUMOVehicleClass> disallowed;
        parseVehicleClasses(inputStorage.readStringList(), disallowed);
        l->setNotAllowedClasses(disallowed);
        l->getEdge().rebuildAllowedLanes();
    }
    break;
    default:
        break;
    }
    server.writeStatusCmd(CMD_SET_LANE_VARIABLE, RTYPE_OK, warning, outputStorage);
    return true;
}


/****************************************************************************/

