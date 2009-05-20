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
#include <utils/geom/Position2DVector.h>
#include "TraCIConstants.h"
#include "TraCIServerAPIHelper.h"
#include "TraCIServerAPI_Vehicle.h"

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
                                         tcpip::Storage &outputStorage) throw(TraCIException)
{
    string warning = ""; // additional description for response
    // variable & id
    int variable = inputStorage.readUnsignedByte();
    string id = inputStorage.readString();
    // check variable
    if (variable!=ID_LIST&&variable!=VAR_SPEED&&variable!=VAR_POSITION&&variable!=VAR_ANGLE
        &&variable!=VAR_ROAD_ID&&variable!=VAR_LANE_ID&&variable!=VAR_LANE_INDEX
        &&variable!=VAR_TYPE&&variable!=VAR_ROUTE&&variable!=VAR_COLOR) {
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
        for(MSVehicleControl::constVehIt i=c.loadedVehBegin(); i!=c.loadedVehEnd(); ++i) {
            if((*i).second->isOnRoad()) {
                ids.push_back((*i).first);
            }
        }
        tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
        tempMsg.writeStringList(ids);
    } else {
        MSVehicle *v = MSNet::getInstance()->getVehicleControl().getVehicle(id);
        if(v==0) {
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
            tempMsg.writeString(v->getLane().getEdge()->getID());
            break;
        case VAR_LANE_ID:
            tempMsg.writeUnsignedByte(TYPE_STRING);
            tempMsg.writeString(v->getLane().getID());
            break;
        case VAR_LANE_INDEX:
            // !!!
            throw 1;
            break;
        case VAR_TYPE:
            tempMsg.writeUnsignedByte(TYPE_STRING);
            tempMsg.writeString(v->getVehicleType().getID());
            break;
        case VAR_ROUTE:
            tempMsg.writeUnsignedByte(TYPE_STRING);
            tempMsg.writeString(v->getRoute().getID());
            break;
        case VAR_COLOR:
            tempMsg.writeUnsignedByte(TYPE_COLOR);
            tempMsg.writeUnsignedByte((int) (v->getParameter().color.red()*255.));
            tempMsg.writeUnsignedByte((int) (v->getParameter().color.green()*255.));
            tempMsg.writeUnsignedByte((int) (v->getParameter().color.blue()*255.));
            tempMsg.writeUnsignedByte(255);
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
                                         tcpip::Storage &outputStorage) throw(TraCIException)
{
    return true;
}



/****************************************************************************/

