/****************************************************************************/
/// @file    TraCIServerAPI_InductionLoop.cpp
/// @author  Daniel Krajzewicz
/// @date    07.05.2009
/// @version $Id: TraCIServerAPI_InductionLoop.cpp 6907 2009-03-13 12:13:38Z dkrajzew $
///
// APIs for getting/setting induction loop values via TraCI
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

#include "TraCIConstants.h"
#include <microsim/output/MSDetectorControl.h>
#include <microsim/output/MSInductLoop.h>
#include "TraCIServerAPIHelper.h"
#include "TraCIServerAPI_InductionLoop.h"

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
TraCIServerAPI_InductionLoop::processGet(tcpip::Storage &inputStorage, 
                                         tcpip::Storage &outputStorage) throw(TraCIException)
{
    string warning = ""; // additional description for response
    // variable & id
    int variable = inputStorage.readUnsignedByte();
    string id = inputStorage.readString();
    // check variable
    if (variable!=ID_LIST&&variable!=LAST_STEP_VEHICLE_NUMBER&&variable!=LAST_STEP_MEAN_SPEED&&variable!=LAST_STEP_VEHICLE_ID_LIST) {
        TraCIServerAPIHelper::writeStatusCmd(CMD_GET_INDUCTIONLOOP_VARIABLE, RTYPE_ERR, "Unsupported variable specified", outputStorage);
        return false;
    }
    // begin response building
    Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(RESPONSE_GET_INDUCTIONLOOP_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);
    // process request
    if (variable==ID_LIST) {
        std::vector<std::string> ids;
        MSNet::getInstance()->getDetectorControl().getInductLoops().insertIDs(ids);
        tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
        tempMsg.writeStringList(ids);
    } else {
        MSInductLoop *il = MSNet::getInstance()->getDetectorControl().getInductLoops().get(id);
        if (il==0) {
            TraCIServerAPIHelper::writeStatusCmd(CMD_GET_INDUCTIONLOOP_VARIABLE, RTYPE_ERR, "Induction loop '" + id + "' is not known", outputStorage);
            return false;
        }
        switch (variable) {
        case ID_LIST:
            break;
        case LAST_STEP_VEHICLE_NUMBER:
            tempMsg.writeUnsignedByte(TYPE_INTEGER);
            tempMsg.writeInt(il->getCurrentPassedNumber());
            break;
        case LAST_STEP_MEAN_SPEED:
            tempMsg.writeUnsignedByte(TYPE_FLOAT);
            tempMsg.writeFloat((float) il->getCurrentSpeed());
            break;
        case LAST_STEP_VEHICLE_ID_LIST: {
            tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
            vector<string> ids = il->getCurrentVehicleIDs();
            tempMsg.writeStringList(ids);
        }
        break;
        default:
            break;
        }
    }
    TraCIServerAPIHelper::writeStatusCmd(CMD_GET_INDUCTIONLOOP_VARIABLE, RTYPE_OK, warning, outputStorage);
    // send response
    outputStorage.writeUnsignedByte(0); // command length -> extended
    outputStorage.writeInt(1 + 4 + tempMsg.size());
    outputStorage.writeStorage(tempMsg);
    return true;
}


/****************************************************************************/

