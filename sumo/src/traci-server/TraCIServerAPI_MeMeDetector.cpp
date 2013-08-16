/****************************************************************************/
/// @file    TraCIServerAPI_MeMeDetector.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting multi-entry/multi-exit detector values via TraCI
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
#include "TraCIServerAPI_MeMeDetector.h"

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
TraCIServerAPI_MeMeDetector::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                                        tcpip::Storage& outputStorage) {
    // variable & id
    int variable = inputStorage.readUnsignedByte();
    std::string id = inputStorage.readString();
    // check variable
    if (variable != ID_LIST && variable != LAST_STEP_VEHICLE_NUMBER && variable != LAST_STEP_MEAN_SPEED
            && variable != LAST_STEP_VEHICLE_ID_LIST && variable != LAST_STEP_VEHICLE_HALTING_NUMBER
            && variable != ID_COUNT) {
        return server.writeErrorStatusCmd(CMD_GET_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE, "Get MeMeDetector Variable: unsupported variable specified", outputStorage);
    }
    // begin response building
    tcpip::Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(RESPONSE_GET_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);
    if (variable == ID_LIST) {
        std::vector<std::string> ids;
        MSNet::getInstance()->getDetectorControl().getTypedDetectors(SUMO_TAG_ENTRY_EXIT_DETECTOR).insertIDs(ids);
        tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
        tempMsg.writeStringList(ids);
    } else if (variable == ID_COUNT) {
        std::vector<std::string> ids;
        MSNet::getInstance()->getDetectorControl().getTypedDetectors(SUMO_TAG_ENTRY_EXIT_DETECTOR).insertIDs(ids);
        tempMsg.writeUnsignedByte(TYPE_INTEGER);
        tempMsg.writeInt((int) ids.size());
    } else {
        MSE3Collector* e3 = static_cast<MSE3Collector*>(MSNet::getInstance()->getDetectorControl().getTypedDetectors(SUMO_TAG_ENTRY_EXIT_DETECTOR).get(id));
        if (e3 == 0) {
            return server.writeErrorStatusCmd(CMD_GET_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE, "Areal detector '" + id + "' is not known", outputStorage);
        }
        switch (variable) {
            case ID_LIST:
                break;
            case LAST_STEP_VEHICLE_NUMBER:
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt((int) e3->getVehiclesWithin());
                break;
            case LAST_STEP_MEAN_SPEED:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(e3->getCurrentMeanSpeed());
                break;
            case LAST_STEP_VEHICLE_ID_LIST: {
                tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
                std::vector<std::string> ids = e3->getCurrentVehicleIDs();
                tempMsg.writeStringList(ids);
            }
            break;
            case LAST_STEP_VEHICLE_HALTING_NUMBER:
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt((int) e3->getCurrentHaltingNumber());
                break;
            default:
                break;
        }
    }
    server.writeStatusCmd(CMD_GET_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE, RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, tempMsg);
    return true;
}

#endif


/****************************************************************************/

