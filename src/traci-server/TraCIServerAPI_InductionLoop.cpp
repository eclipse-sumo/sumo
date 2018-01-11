/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2009-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    TraCIServerAPI_InductionLoop.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting induction loop values via TraCI
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
#include <microsim/output/MSDetectorControl.h>
#include <libsumo/InductionLoop.h>
#include "TraCIConstants.h"
#include "TraCIServerAPI_InductionLoop.h"


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_InductionLoop::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
        tcpip::Storage& outputStorage) {
    // variable & id
    int variable = inputStorage.readUnsignedByte();
    std::string id = inputStorage.readString();
    // check variable
    if (variable != ID_LIST && variable != LAST_STEP_VEHICLE_NUMBER && variable != LAST_STEP_MEAN_SPEED
            && variable != LAST_STEP_VEHICLE_ID_LIST && variable != LAST_STEP_OCCUPANCY
            && variable != LAST_STEP_LENGTH && variable != LAST_STEP_TIME_SINCE_DETECTION
            && variable != LAST_STEP_VEHICLE_DATA && variable != ID_COUNT
            && variable != VAR_POSITION && variable != VAR_LANE_ID) {
        return server.writeErrorStatusCmd(CMD_GET_INDUCTIONLOOP_VARIABLE, "Get Induction Loop Variable: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    // begin response building
    tcpip::Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(RESPONSE_GET_INDUCTIONLOOP_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);
    // process request
    try {
        switch (variable) {
            case ID_LIST:
                tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
                tempMsg.writeStringList(libsumo::InductionLoop::getIDList());
                break;
            case ID_COUNT:
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt(libsumo::InductionLoop::getIDCount());
                break;
            case VAR_POSITION:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(libsumo::InductionLoop::getPosition(id));
                break;
            case VAR_LANE_ID:
                tempMsg.writeUnsignedByte(TYPE_STRING);
                tempMsg.writeString(libsumo::InductionLoop::getLaneID(id));
                break;
            case LAST_STEP_VEHICLE_NUMBER:
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt(libsumo::InductionLoop::getLastStepVehicleNumber(id));
                break;
            case LAST_STEP_MEAN_SPEED:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(libsumo::InductionLoop::getLastStepMeanSpeed(id));
                break;
            case LAST_STEP_VEHICLE_ID_LIST:
                tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
                tempMsg.writeStringList(libsumo::InductionLoop::getLastStepVehicleIDs(id));
                break;
            case LAST_STEP_OCCUPANCY:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(libsumo::InductionLoop::getLastStepOccupancy(id));
                break;
            case LAST_STEP_LENGTH:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(libsumo::InductionLoop::getLastStepMeanLength(id));
                break;
            case LAST_STEP_TIME_SINCE_DETECTION:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(libsumo::InductionLoop::getTimeSinceDetection(id));
                break;
            case LAST_STEP_VEHICLE_DATA: {
                std::vector<libsumo::TraCIVehicleData> vd = libsumo::InductionLoop::getVehicleData(id);
                tempMsg.writeUnsignedByte(TYPE_COMPOUND);
                tcpip::Storage tempContent;
                int cnt = 0;
                tempContent.writeUnsignedByte(TYPE_INTEGER);
                tempContent.writeInt((int) vd.size());
                ++cnt;
                for (int i = 0; i < (int)vd.size(); ++i) {
                    libsumo::TraCIVehicleData& svd = vd[i];
                    tempContent.writeUnsignedByte(TYPE_STRING);
                    tempContent.writeString(svd.id);
                    ++cnt;
                    tempContent.writeUnsignedByte(TYPE_DOUBLE);
                    tempContent.writeDouble(svd.length);
                    ++cnt;
                    tempContent.writeUnsignedByte(TYPE_DOUBLE);
                    tempContent.writeDouble(svd.entryTime);
                    ++cnt;
                    tempContent.writeUnsignedByte(TYPE_DOUBLE);
                    tempContent.writeDouble(svd.leaveTime);
                    ++cnt;
                    tempContent.writeUnsignedByte(TYPE_STRING);
                    tempContent.writeString(svd.typeID);
                    ++cnt;
                }

                tempMsg.writeInt((int) cnt);
                tempMsg.writeStorage(tempContent);
                break;
            }
            default:
                break;
        }
    } catch (libsumo::TraCIException& e) {
        return server.writeErrorStatusCmd(CMD_GET_INDUCTIONLOOP_VARIABLE, e.what(), outputStorage);
    }
    server.writeStatusCmd(CMD_GET_INDUCTIONLOOP_VARIABLE, RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, tempMsg);
    return true;
}


bool
TraCIServerAPI_InductionLoop::getPosition(const std::string& id, Position& p) {
    MSInductLoop* il = static_cast<MSInductLoop*>(MSNet::getInstance()->getDetectorControl().getTypedDetectors(SUMO_TAG_INDUCTION_LOOP).get(id));
    if (il == 0) {
        return false;
    }
    p = il->getLane()->getShape().positionAtOffset(il->getPosition());
    return true;
}


/****************************************************************************/

