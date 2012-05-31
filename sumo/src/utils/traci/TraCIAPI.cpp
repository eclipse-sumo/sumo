/****************************************************************************/
/// @file    TraCIAPI.h
/// @author  Daniel Krajzewicz
/// @date    30.05.2012
/// @version $Id:$
///
// C++ TraCI client API implementation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
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

#include "TraCIAPI.h"
#include <traci-server/TraCIConstants.h>
#include <utils/common/ToString.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// TraCIAPI-methods
// ---------------------------------------------------------------------------
TraCIAPI::TraCIAPI() 
    : edge(*this), vehicle(*this), simulation(*this), mySocket(0) {}


TraCIAPI::~TraCIAPI() 
{}


void
TraCIAPI::connect(const std::string &host, int port) throw(tcpip::SocketException) {
    mySocket = new tcpip::Socket(host, port);
    //socket->set_blocking(true);
    try {
        mySocket->connect();
    } catch (tcpip::SocketException& ) {
        delete mySocket;
        mySocket = 0;
        throw;
    }
}


void
TraCIAPI::close() {
    if (mySocket == 0) {
        return;
    }
    mySocket->close();
    delete mySocket;
    mySocket = 0;
}


void 
TraCIAPI::send_commandGetVariable(int domID, int varID, const std::string& objID, tcpip::Storage* add) const throw(tcpip::SocketException) {
    if (mySocket == 0) {
        throw tcpip::SocketException("Socket is not initialised");
    }
    tcpip::Storage outMsg;
    // command length
    unsigned int length = 1 + 1 + 1 + 4 + (int) objID.length();
    if(add!=0) {
        length += add->size();
    }
    outMsg.writeUnsignedByte(length);
    // command id
    outMsg.writeUnsignedByte(domID);
    // variable id
    outMsg.writeUnsignedByte(varID);
    // object id
    outMsg.writeString(objID);
    // additional values
    if(add!=0) {
        outMsg.writeStorage(*add);
    }
    // send request message
    mySocket->sendExact(outMsg);
}


void
TraCIAPI::check_resultState(tcpip::Storage& inMsg, int command, bool ignoreCommandId) const throw(tcpip::SocketException) {
    mySocket->receiveExact(inMsg);
    int cmdLength;
    int cmdId;
    int resultType;
    int cmdStart;
    std::string msg;
    try {
        cmdStart = inMsg.position();
        cmdLength = inMsg.readUnsignedByte();
        cmdId = inMsg.readUnsignedByte();
        if (cmdId != command && !ignoreCommandId) {
            throw tcpip::SocketException("#Error: received status response to command: " + toString(cmdId) + " but expected: " + toString(command));
        }
        resultType = inMsg.readUnsignedByte();
        msg = inMsg.readString();
    } catch (std::invalid_argument&) {
        throw tcpip::SocketException("#Error: an exception was thrown while reading result state message");
    }
    switch (resultType) {
        case RTYPE_ERR:
            throw tcpip::SocketException(".. Answered with error to command (" + toString(cmdId) + "), [description: " + msg + "]");
        case RTYPE_NOTIMPLEMENTED:
            throw tcpip::SocketException(".. Sent command is not implemented (" + toString(cmdId) + "), [description: " + msg + "]");
        case RTYPE_OK:
            //throw tcpip::SocketException(".. Command acknowledged (" << cmdId << "), [description: " << msg << "]");
            break;
        default:
            throw tcpip::SocketException(".. Answered with unknown result code(" + toString(resultType) + ") to command(" + toString(cmdId) + "), [description: " + msg + "]");
    }
    if ((cmdStart + cmdLength) != inMsg.position()) {
        throw tcpip::SocketException("#Error: command at position " + toString(cmdStart) + " has wrong length");
    }
}


void
TraCIAPI::check_commandGetResult(tcpip::Storage& inMsg, int command, int expectedType, bool ignoreCommandId) const throw(tcpip::SocketException) {
    int respStart = inMsg.position();
    int length = inMsg.readUnsignedByte();
    if (length == 0) {
        length = inMsg.readInt();
    }
    int cmdId = inMsg.readUnsignedByte();
    if (cmdId != (command + 0x10)) {
        throw tcpip::SocketException("#Error: received response with command id: " + toString(cmdId) + "but expected: " + toString(command + 0x10));
    }
    int valueDataType = inMsg.readUnsignedByte();
    if(valueDataType!=expectedType) {
        throw tcpip::SocketException("Expected " + toString(expectedType) + " but got " + toString(valueDataType));
    }
}


void
TraCIAPI::processGET(tcpip::Storage& inMsg, int command, int expectedType, bool ignoreCommandId) const throw(tcpip::SocketException) {
    check_resultState(inMsg, command, ignoreCommandId);
    check_commandGetResult(inMsg, command, expectedType, ignoreCommandId);
}


void 
TraCIAPI::send_commandSetValue(int domID, int varID, const std::string& objID, tcpip::Storage& content) const throw(tcpip::SocketException) {
    if (mySocket == 0) {
        throw tcpip::SocketException("Socket is not initialised");
    }
    tcpip::Storage outMsg;
    // command length (domID, varID, objID, dataType, data)
    outMsg.writeUnsignedByte(1 + 1 + 1 + 4 + (int) objID.length() + content.size());
    // command id
    outMsg.writeUnsignedByte(domID);
    // variable id
    outMsg.writeUnsignedByte(varID);
    // object id
    outMsg.writeString(objID);
    // data type
    outMsg.writeStorage(content);
    // send message
    mySocket->sendExact(outMsg);
    // check answer
    tcpip::Storage inMsg;
    mySocket->receiveExact(inMsg);
    check_resultState(inMsg, domID);
}



SUMOTime 
TraCIAPI::getSUMOTime(int cmd, int var, const std::string &id, tcpip::Storage* add) throw(tcpip::SocketException) {
    tcpip::Storage inMsg;
    send_commandGetVariable(cmd, var, id, add);
    processGET(inMsg, cmd, TYPE_INTEGER);
    return inMsg.readInt();
}


int 
TraCIAPI::getUnsignedByte(int cmd, int var, const std::string &id, tcpip::Storage* add) throw(tcpip::SocketException) {
    tcpip::Storage inMsg;
    send_commandGetVariable(cmd, var, id, add);
    processGET(inMsg, cmd, TYPE_UBYTE);
    return inMsg.readUnsignedByte();
}


int 
TraCIAPI::getByte(int cmd, int var, const std::string &id, tcpip::Storage* add) throw(tcpip::SocketException) {
    tcpip::Storage inMsg;
    send_commandGetVariable(cmd, var, id, add);
    processGET(inMsg, cmd, TYPE_BYTE);
    return inMsg.readByte();
}


int 
TraCIAPI::getInt(int cmd, int var, const std::string &id, tcpip::Storage* add) throw(tcpip::SocketException) {
    tcpip::Storage inMsg;
    send_commandGetVariable(cmd, var, id, add);
    processGET(inMsg, cmd, TYPE_INTEGER);
    return inMsg.readInt();
}


SUMOReal 
TraCIAPI::getFloat(int cmd, int var, const std::string &id, tcpip::Storage* add) throw(tcpip::SocketException) {
    tcpip::Storage inMsg;
    send_commandGetVariable(cmd, var, id, add);
    processGET(inMsg, cmd, TYPE_FLOAT);
    return inMsg.readFloat();
}


SUMOReal 
TraCIAPI::getDouble(int cmd, int var, const std::string &id, tcpip::Storage* add) throw(tcpip::SocketException) {
    tcpip::Storage inMsg;
    send_commandGetVariable(cmd, var, id, add);
    processGET(inMsg, cmd, TYPE_DOUBLE);
    return inMsg.readDouble();
}


Boundary 
TraCIAPI::getBoundingBox(int cmd, int var, const std::string &id, tcpip::Storage* add) throw(tcpip::SocketException) {
    tcpip::Storage inMsg;
    send_commandGetVariable(cmd, var, id, add);
    processGET(inMsg, cmd, TYPE_BOUNDINGBOX);
    SUMOReal xmin = inMsg.readDouble();
    SUMOReal ymin = inMsg.readDouble();
    SUMOReal xmax = inMsg.readDouble();
    SUMOReal ymax = inMsg.readDouble();
    return Boundary(xmin, ymin, xmax, ymax);
}


PositionVector 
TraCIAPI::getPolygon(int cmd, int var, const std::string &id, tcpip::Storage* add) throw(tcpip::SocketException) {
    tcpip::Storage inMsg;
    send_commandGetVariable(cmd, var, id, add);
    processGET(inMsg, cmd, TYPE_POLYGON);
    unsigned int size = inMsg.readInt();
    PositionVector p;
    for(unsigned int i=0; i<size; ++i) {
        SUMOReal x = inMsg.readDouble();
        SUMOReal y = inMsg.readDouble();
        p.push_back(Position(x, y));
    }
    return p;
}


Position 
TraCIAPI::getPosition(int cmd, int var, const std::string &id, tcpip::Storage* add) throw(tcpip::SocketException) {
    tcpip::Storage inMsg;
    send_commandGetVariable(cmd, var, id, add);
    processGET(inMsg, cmd, POSITION_2D);
    int valueDataType = inMsg.readUnsignedByte();
    throw tcpip::SocketException("Expected TYPE_INTEGER but got " + toString(valueDataType));
    //return inMsg.readInt();
}


std::string 
TraCIAPI::getString(int cmd, int var, const std::string &id, tcpip::Storage* add) throw(tcpip::SocketException) {
    tcpip::Storage inMsg;
    send_commandGetVariable(cmd, var, id, add);
    processGET(inMsg, cmd, TYPE_STRING);
    return inMsg.readString();
}


std::vector<std::string>
TraCIAPI::getStringVector(int cmd, int var, const std::string &id, tcpip::Storage* add) throw(tcpip::SocketException) {
    tcpip::Storage inMsg;
    send_commandGetVariable(cmd, var, id, add);
    processGET(inMsg, cmd, TYPE_STRINGLIST);
    throw tcpip::SocketException("Expected TYPE_INTEGER but got ");
    //return inMsg.readInt();
}



// ---------------------------------------------------------------------------
// TraCIAPI::SimulationScope-methods
// ---------------------------------------------------------------------------
std::vector<std::string>
TraCIAPI::EdgeScope::getIDList() const throw(tcpip::SocketException) {
    return myParent.getStringVector(CMD_GET_EDGE_VARIABLE, ID_LIST, "");
}

unsigned int
TraCIAPI::EdgeScope::getIDCount() const throw(tcpip::SocketException) {
    return myParent.getInt(CMD_GET_EDGE_VARIABLE, ID_COUNT, "");
}

SUMOReal 
TraCIAPI::EdgeScope::getAdaptedTraveltime(const std::string &edgeID, SUMOTime time) const throw(tcpip::SocketException) {
    tcpip::Storage content;
    content.writeInt(time);
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, VAR_CO2EMISSION, edgeID, &content);
 }

SUMOReal
TraCIAPI::EdgeScope::getEffort(const std::string &edgeID, SUMOTime time) const throw(tcpip::SocketException) {
    tcpip::Storage content;
    content.writeInt(time);
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, VAR_CO2EMISSION, edgeID, &content);
}

SUMOReal
TraCIAPI::EdgeScope::getCO2Emission(const std::string &edgeID) const throw(tcpip::SocketException) {
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, VAR_CO2EMISSION, edgeID);
}

SUMOReal 
TraCIAPI::EdgeScope::getCOEmission(const std::string &edgeID) const throw(tcpip::SocketException) {
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, VAR_COEMISSION, edgeID);
}

SUMOReal 
TraCIAPI::EdgeScope::getHCEmission(const std::string &edgeID) const throw(tcpip::SocketException) {
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, VAR_HCEMISSION, edgeID);
}

SUMOReal 
TraCIAPI::EdgeScope::getPMxEmission(const std::string &edgeID) const throw(tcpip::SocketException) {
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, VAR_PMXEMISSION, edgeID);
}

SUMOReal 
TraCIAPI::EdgeScope::getNOxEmission(const std::string &edgeID) const throw(tcpip::SocketException) {
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, VAR_NOXEMISSION, edgeID);
}

SUMOReal 
TraCIAPI::EdgeScope::getFuelConsumption(const std::string &edgeID) const throw(tcpip::SocketException) {
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, VAR_FUELCONSUMPTION, edgeID);
}

SUMOReal 
TraCIAPI::EdgeScope::getNoiseEmission(const std::string &edgeID) const throw(tcpip::SocketException) {
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, VAR_NOISEEMISSION, edgeID);
}

SUMOReal 
TraCIAPI::EdgeScope::getLastStepMeanSpeed(const std::string &edgeID) const throw(tcpip::SocketException) {
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, LAST_STEP_MEAN_SPEED, edgeID);
}

SUMOReal
TraCIAPI::EdgeScope::getLastStepOccupancy(const std::string &edgeID) const throw(tcpip::SocketException) {
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, LAST_STEP_OCCUPANCY, edgeID);
}

SUMOReal
TraCIAPI::EdgeScope::getLastStepLength(const std::string &edgeID) const throw(tcpip::SocketException) {
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, LAST_STEP_LENGTH, edgeID);
}

SUMOReal
TraCIAPI::EdgeScope::getTraveltime(const std::string &edgeID) const throw(tcpip::SocketException) {
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, VAR_CURRENT_TRAVELTIME, edgeID);
}

unsigned int
TraCIAPI::EdgeScope::getLastStepVehicleNumber(const std::string &edgeID) const throw(tcpip::SocketException) {
    return myParent.getInt(CMD_GET_EDGE_VARIABLE, LAST_STEP_VEHICLE_NUMBER, edgeID);
}

SUMOReal
TraCIAPI::EdgeScope::getLastStepHaltingNumber(const std::string &edgeID) const throw(tcpip::SocketException) {
    return myParent.getInt(CMD_GET_EDGE_VARIABLE, LAST_STEP_VEHICLE_HALTING_NUMBER, edgeID);
}

std::vector<std::string>
TraCIAPI::EdgeScope::getLastStepVehicleIDs(const std::string &edgeID) const throw(tcpip::SocketException) {
    return myParent.getStringVector(CMD_GET_EDGE_VARIABLE, LAST_STEP_VEHICLE_ID_LIST, edgeID);
}



void
TraCIAPI::EdgeScope::adaptTraveltime(const std::string &edgeID, SUMOReal time) const throw(tcpip::SocketException) {
    tcpip::Storage content;
    content.writeDouble(time);
    myParent.send_commandSetValue(CMD_SET_EDGE_VARIABLE, VAR_EDGE_TRAVELTIME, edgeID, content);
}

void
TraCIAPI::EdgeScope::setEffort(const std::string &edgeID, SUMOReal effort) const throw(tcpip::SocketException) {
    tcpip::Storage content;
    content.writeDouble(effort);
    myParent.send_commandSetValue(CMD_SET_EDGE_VARIABLE, VAR_EDGE_EFFORT, edgeID, content);
}

void
TraCIAPI::EdgeScope::setMaxSpeed(const std::string &edgeID, SUMOReal speed) const throw(tcpip::SocketException) {
    tcpip::Storage content;
    content.writeDouble(speed);
    myParent.send_commandSetValue(CMD_SET_EDGE_VARIABLE, VAR_MAXSPEED, edgeID, content);
}

    
// ---------------------------------------------------------------------------
// TraCIAPI::SimulationScope-methods
// ---------------------------------------------------------------------------
SUMOTime 
TraCIAPI::SimulationScope::getCurrentTime() const {
    return myParent.getSUMOTime(CMD_GET_SIM_VARIABLE, VAR_TIME_STEP, "");
}

unsigned int 
TraCIAPI::SimulationScope::getLoadedNumber() const {
    return (unsigned int) myParent.getInt(CMD_GET_SIM_VARIABLE, VAR_LOADED_VEHICLES_NUMBER, "");
}

std::vector<std::string> 
TraCIAPI::SimulationScope::getLoadedIDList() const {
    return myParent.getStringVector(CMD_GET_SIM_VARIABLE, VAR_LOADED_VEHICLES_IDS, "");
}

unsigned int 
TraCIAPI::SimulationScope::getDepartedNumber() const {
    return (unsigned int) myParent.getInt(CMD_GET_SIM_VARIABLE, VAR_DEPARTED_VEHICLES_NUMBER, "");
}

std::vector<std::string> 
TraCIAPI::SimulationScope::getDepartedIDList() const {
    return myParent.getStringVector(CMD_GET_SIM_VARIABLE, VAR_DEPARTED_VEHICLES_IDS, "");
}

unsigned int 
TraCIAPI::SimulationScope::getArrivedNumber() const {
    return (unsigned int) myParent.getInt(CMD_GET_SIM_VARIABLE, VAR_ARRIVED_VEHICLES_NUMBER, "");
}

std::vector<std::string> 
TraCIAPI::SimulationScope::getArrivedIDList() const {
    return myParent.getStringVector(CMD_GET_SIM_VARIABLE, VAR_ARRIVED_VEHICLES_IDS, "");
}

unsigned int 
TraCIAPI::SimulationScope::getStartingTeleportNumber() const {
    return (unsigned int) myParent.getInt(CMD_GET_SIM_VARIABLE, VAR_TELEPORT_STARTING_VEHICLES_NUMBER, "");
}

std::vector<std::string> 
TraCIAPI::SimulationScope::getStartingTeleportIDList() const {
    return myParent.getStringVector(CMD_GET_SIM_VARIABLE, VAR_TELEPORT_STARTING_VEHICLES_IDS, "");
}

unsigned int 
TraCIAPI::SimulationScope::getEndingTeleportNumber() const {
    return (unsigned int) myParent.getInt(CMD_GET_SIM_VARIABLE, VAR_TELEPORT_ENDING_VEHICLES_NUMBER, "");
}

std::vector<std::string> 
TraCIAPI::SimulationScope::getEndingTeleportIDList() const {
    return myParent.getStringVector(CMD_GET_SIM_VARIABLE, VAR_TELEPORT_ENDING_VEHICLES_IDS, "");
}

SUMOTime 
TraCIAPI::SimulationScope::getDeltaT() const {
    return myParent.getSUMOTime(CMD_GET_SIM_VARIABLE, VAR_DELTA_T, "");
}

Boundary 
TraCIAPI::SimulationScope::getNetBoundary() const {
    return myParent.getBoundingBox(CMD_GET_SIM_VARIABLE, VAR_NET_BOUNDING_BOX, "");
}

unsigned int 
TraCIAPI::SimulationScope::getMinExpectedNumber() const {
    return myParent.getInt(CMD_GET_SIM_VARIABLE, VAR_MIN_EXPECTED_VEHICLES, "");
}





/****************************************************************************/

