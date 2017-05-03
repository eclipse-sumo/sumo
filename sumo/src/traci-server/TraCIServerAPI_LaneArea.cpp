/****************************************************************************/
/// @file    TraCIServerAPI_AreaDetector.cpp
/// @author  Mario Krumnow
/// @author  Robbin Blokpoel
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    03.02.2014
/// @version $Id$
///
// APIs for getting/setting areal detector values via TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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

#ifndef NO_TRACI

#include <microsim/output/MSDetectorControl.h>
#include "TraCIConstants.h"
#include "TraCIDefs.h"
#include "TraCIServer.h"
#include "lib/TraCI_LaneArea.h"
#include "TraCIServerAPI_LaneArea.h"


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_LaneArea::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                                    tcpip::Storage& outputStorage) {
    // variable & id
    int variable = inputStorage.readUnsignedByte();
    std::string id = inputStorage.readString();
    // check variable
    if (variable != ID_LIST
            && variable != ID_COUNT
            && variable != JAM_LENGTH_VEHICLE
            && variable != JAM_LENGTH_METERS
            && variable != LAST_STEP_VEHICLE_NUMBER
            && variable != LAST_STEP_MEAN_SPEED
            && variable != LAST_STEP_VEHICLE_ID_LIST
            && variable != LAST_STEP_VEHICLE_HALTING_NUMBER
            && variable != ID_COUNT
            && variable != LAST_STEP_OCCUPANCY
            && variable != VAR_POSITION
            && variable != VAR_LANE_ID
            && variable != VAR_LENGTH) {
        return server.writeErrorStatusCmd(CMD_GET_LANEAREA_VARIABLE, "Get Areal Detector Variable: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }

    // begin response building
    tcpip::Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(RESPONSE_GET_LANEAREA_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);
    try {
        switch (variable) {
            case ID_LIST:
                tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
                tempMsg.writeStringList(TraCI_LaneArea::getIDList());
                break;
            case ID_COUNT:
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt(TraCI_LaneArea::getIDCount());
                break;
            case LAST_STEP_VEHICLE_NUMBER:
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt(TraCI_LaneArea::getLastStepVehicleNumber(id));
                break;
            case LAST_STEP_MEAN_SPEED:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(TraCI_LaneArea::getLastStepMeanSpeed(id));
                break;
            case LAST_STEP_VEHICLE_ID_LIST:
                tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
                tempMsg.writeStringList(TraCI_LaneArea::getLastStepVehicleIDs(id));
                break;
            case LAST_STEP_VEHICLE_HALTING_NUMBER:
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt(TraCI_LaneArea::getLastStepHaltingNumber(id));
                break;
            case JAM_LENGTH_VEHICLE:
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt(TraCI_LaneArea::getJamLengthVehicle(id));
                break;
            case JAM_LENGTH_METERS:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(TraCI_LaneArea::getJamLengthMeters(id));
                break;
            case LAST_STEP_OCCUPANCY:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(TraCI_LaneArea::getLastStepOccupancy(id));
                break;
            case VAR_POSITION:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(TraCI_LaneArea::getPosition(id));
                break;
            case VAR_LANE_ID:
                tempMsg.writeUnsignedByte(TYPE_STRING);
                tempMsg.writeString(TraCI_LaneArea::getLaneID(id));
                break;
            case VAR_LENGTH:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(TraCI_LaneArea::getLength(id));
                break;
            default:
                break;
        }
    } catch (TraCIException& e) {
        return server.writeErrorStatusCmd(CMD_GET_LANEAREA_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(CMD_GET_LANEAREA_VARIABLE, RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, tempMsg);
    return true;
}

#endif


/****************************************************************************/

