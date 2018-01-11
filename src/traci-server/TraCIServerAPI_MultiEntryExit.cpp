/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    TraCIServerAPI_MultiEntryExit.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting multi-entry/multi-exit detector values via TraCI
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <microsim/output/MSDetectorControl.h>
#include "TraCIConstants.h"
#include <libsumo/MultiEntryExit.h>
#include "TraCIServerAPI_MultiEntryExit.h"


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_MultiEntryExit::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
        tcpip::Storage& outputStorage) {
    // variable & id
    int variable = inputStorage.readUnsignedByte();
    std::string id = inputStorage.readString();
    // check variable
    if (variable != ID_LIST && variable != LAST_STEP_VEHICLE_NUMBER && variable != LAST_STEP_MEAN_SPEED
            && variable != LAST_STEP_VEHICLE_ID_LIST && variable != LAST_STEP_VEHICLE_HALTING_NUMBER
            && variable != ID_COUNT) {
        return server.writeErrorStatusCmd(CMD_GET_MULTIENTRYEXIT_VARIABLE, "Get MeMeDetector Variable: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    // begin response building
    tcpip::Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(RESPONSE_GET_MULTIENTRYEXIT_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);
    try {
        switch (variable) {
            case ID_LIST:
                tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
                tempMsg.writeStringList(libsumo::MultiEntryExit::getIDList());
                break;
            case ID_COUNT:
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt(libsumo::MultiEntryExit::getIDCount());
                break;
            case LAST_STEP_VEHICLE_NUMBER:
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt(libsumo::MultiEntryExit::getLastStepVehicleNumber(id));
                break;
            case LAST_STEP_MEAN_SPEED:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(libsumo::MultiEntryExit::getLastStepMeanSpeed(id));
                break;
            case LAST_STEP_VEHICLE_ID_LIST:
                tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
                tempMsg.writeStringList(libsumo::MultiEntryExit::getLastStepVehicleIDs(id));
                break;
            case LAST_STEP_VEHICLE_HALTING_NUMBER:
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt(libsumo::MultiEntryExit::getLastStepHaltingNumber(id));
                break;
            default:
                break;
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(CMD_GET_MULTIENTRYEXIT_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(CMD_GET_MULTIENTRYEXIT_VARIABLE, RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, tempMsg);
    return true;
}


/****************************************************************************/
