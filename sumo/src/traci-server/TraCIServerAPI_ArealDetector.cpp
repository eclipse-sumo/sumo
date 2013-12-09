/****************************************************************************/
/// @file    TraCIServerAPI_AreaDetector.cpp
/// @author  Mario Krumnow
/// @date    15.09.2013
/// @version $Id$
///
// APIs for getting/setting areal detector values via TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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

#include "TraCIConstants.h"
#include <microsim/output/MSDetectorControl.h>
#include <microsim/output/MSE3Collector.h>
#include "TraCIServerAPI_ArealDetector.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_ArealDetector::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
        tcpip::Storage& outputStorage) {
    // variable & id
    int variable = inputStorage.readUnsignedByte();
    std::string id = inputStorage.readString();
    // check variable
    if (variable != ID_LIST && variable != ID_COUNT && variable != JAM_LENGTH_VEHICLE && variable != JAM_LENGTH_METERS
		&& variable != LAST_STEP_MEAN_SPEED && variable != LAST_STEP_OCCUPANCY) {
        return server.writeErrorStatusCmd(CMD_GET_AREAL_DETECTOR_VARIABLE, "Get Areal Detector Variable: unsupported variable specified", outputStorage);
    }

    // begin response building
    tcpip::Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(RESPONSE_GET_AREAL_DETECTOR_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);
    if (variable == ID_LIST) {
        std::vector<std::string> ids;
        MSNet::getInstance()->getDetectorControl().getTypedDetectors(SUMO_TAG_LANE_AREA_DETECTOR).insertIDs(ids);
        tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
        tempMsg.writeStringList(ids);
    } else if (variable == ID_COUNT) {
        std::vector<std::string> ids;
        MSNet::getInstance()->getDetectorControl().getTypedDetectors(SUMO_TAG_LANE_AREA_DETECTOR).insertIDs(ids);
        tempMsg.writeUnsignedByte(TYPE_INTEGER);
        tempMsg.writeInt((int) ids.size());
    } else {
        MSE2Collector* e2 = static_cast<MSE2Collector*>(MSNet::getInstance()->getDetectorControl().getTypedDetectors(SUMO_TAG_LANE_AREA_DETECTOR).get(id));
        if (e2 == 0) {
            return server.writeErrorStatusCmd(CMD_GET_AREAL_DETECTOR_VARIABLE, "Areal detector '" + id + "' is not known", outputStorage);
        }
        switch (variable) {
            case ID_LIST:
                break;
            case JAM_LENGTH_VEHICLE:
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt((int) e2->getCurrentJamLengthInVehicles());
                break;
            case JAM_LENGTH_METERS:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(e2->getCurrentJamLengthInMeters());
                break;
			case LAST_STEP_MEAN_SPEED:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
				tempMsg.writeDouble(e2->getCurrentMeanSpeed());
                break;
			case LAST_STEP_OCCUPANCY:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
				tempMsg.writeDouble(e2->getCurrentOccupancy());
                break;
            default:
                break;
        }
    }
    server.writeStatusCmd(CMD_GET_AREAL_DETECTOR_VARIABLE, RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, tempMsg);
    return true;
}

#endif


/****************************************************************************/

