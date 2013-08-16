/****************************************************************************/
/// @file    TraCIAPI.h
/// @author  Daniel Krajzewicz
/// @date    30.05.2012
/// @version $Id$
///
// C++ TraCI client API implementation
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
#ifdef _MSC_VER
#pragma warning(disable: 4355)
#endif
TraCIAPI::TraCIAPI()
    : edge(*this), gui(*this), inductionloop(*this),
      junction(*this), lane(*this), multientryexit(*this), poi(*this),
      polygon(*this), route(*this), simulation(*this), trafficlights(*this),
      vehicletype(*this),
      mySocket(0) {}
#ifdef _MSC_VER
#pragma warning(default: 4355)
#endif


TraCIAPI::~TraCIAPI() {
    delete mySocket;
}


void
TraCIAPI::connect(const std::string& host, int port) {
    mySocket = new tcpip::Socket(host, port);
    try {
        mySocket->connect();
    } catch (tcpip::SocketException&) {
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
TraCIAPI::send_commandSimulationStep(SUMOTime time) const {
    tcpip::Storage outMsg;
    // command length
    outMsg.writeUnsignedByte(1 + 1 + 4);
    // command id
    outMsg.writeUnsignedByte(CMD_SIMSTEP2);
    outMsg.writeInt(time);
    // send request message
    mySocket->sendExact(outMsg);
}


void
TraCIAPI::send_commandClose() const {
    tcpip::Storage outMsg;
    // command length
    outMsg.writeUnsignedByte(1 + 1);
    // command id
    outMsg.writeUnsignedByte(CMD_CLOSE);
    mySocket->sendExact(outMsg);
}


void
TraCIAPI::send_commandGetVariable(int domID, int varID, const std::string& objID, tcpip::Storage* add) const {
    if (mySocket == 0) {
        throw tcpip::SocketException("Socket is not initialised");
    }
    tcpip::Storage outMsg;
    // command length
    unsigned int length = 1 + 1 + 1 + 4 + (int) objID.length();
    if (add != 0) {
        length += (int)add->size();
    }
    outMsg.writeUnsignedByte(length);
    // command id
    outMsg.writeUnsignedByte(domID);
    // variable id
    outMsg.writeUnsignedByte(varID);
    // object id
    outMsg.writeString(objID);
    // additional values
    if (add != 0) {
        outMsg.writeStorage(*add);
    }
    // send request message
    mySocket->sendExact(outMsg);
}


void
TraCIAPI::send_commandSetValue(int domID, int varID, const std::string& objID, tcpip::Storage& content) const {
    if (mySocket == 0) {
        throw tcpip::SocketException("Socket is not initialised");
    }
    tcpip::Storage outMsg;
    // command length (domID, varID, objID, dataType, data)
    outMsg.writeUnsignedByte(1 + 1 + 1 + 4 + (int) objID.length() + (int)content.size());
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
}


void
TraCIAPI::send_commandSubscribeObjectVariable(int domID, const std::string& objID, int beginTime, int endTime,
        const std::vector<int>& vars) const {
    if (mySocket == 0) {
        throw tcpip::SocketException("Socket is not initialised");
    }
    tcpip::Storage outMsg;
    // command length (domID, objID, beginTime, endTime, length, vars)
    int varNo = (int) vars.size();
    outMsg.writeUnsignedByte(0);
    outMsg.writeInt(5 + 1 + 4 + 4 + 4 + (int) objID.length() + 1 + varNo);
    // command id
    outMsg.writeUnsignedByte(domID);
    // time
    outMsg.writeInt(beginTime);
    outMsg.writeInt(endTime);
    // object id
    outMsg.writeString(objID);
    // command id
    outMsg.writeUnsignedByte((int)vars.size());
    for (int i = 0; i < varNo; ++i) {
        outMsg.writeUnsignedByte(vars[i]);
    }
    // send message
    mySocket->sendExact(outMsg);
}


void
TraCIAPI::send_commandSubscribeObjectContext(int domID, const std::string& objID, int beginTime, int endTime,
        int domain, SUMOReal range, const std::vector<int>& vars) const {
    if (mySocket == 0) {
        throw tcpip::SocketException("Socket is not initialised");
    }
    tcpip::Storage outMsg;
    // command length (domID, objID, beginTime, endTime, length, vars)
    int varNo = (int) vars.size();
    outMsg.writeUnsignedByte(0);
    outMsg.writeInt(5 + 1 + 4 + 4 + 4 + (int) objID.length() + 1 + 8 + 1 + varNo);
    // command id
    outMsg.writeUnsignedByte(domID);
    // time
    outMsg.writeInt(beginTime);
    outMsg.writeInt(endTime);
    // object id
    outMsg.writeString(objID);
    // domain and range
    outMsg.writeUnsignedByte(domain);
    outMsg.writeDouble(range);
    // command id
    outMsg.writeUnsignedByte((int)vars.size());
    for (int i = 0; i < varNo; ++i) {
        outMsg.writeUnsignedByte(vars[i]);
    }
    // send message
    mySocket->sendExact(outMsg);
}




void
TraCIAPI::check_resultState(tcpip::Storage& inMsg, int command, bool ignoreCommandId, std::string* acknowledgement) const {
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
        if (command != cmdId && !ignoreCommandId) {
            throw tcpip::SocketException("#Error: received status response to command: " + toString(cmdId) + " but expected: " + toString(command));
        }
        resultType = inMsg.readUnsignedByte();
        msg = inMsg.readString();
    } catch (std::invalid_argument&) {
        throw tcpip::SocketException("#Error: an exception was thrown while reading result state message");
    }
    switch (resultType) {
        case RTYPE_ERR:
            throw tcpip::SocketException(".. Answered with error to command (" + toString(command) + "), [description: " + msg + "]");
        case RTYPE_NOTIMPLEMENTED:
            throw tcpip::SocketException(".. Sent command is not implemented (" + toString(command) + "), [description: " + msg + "]");
        case RTYPE_OK:
            if (acknowledgement != 0) {
                (*acknowledgement) = ".. Command acknowledged (" + toString(command) + "), [description: " + msg + "]";
            }
            break;
        default:
            throw tcpip::SocketException(".. Answered with unknown result code(" + toString(resultType) + ") to command(" + toString(command) + "), [description: " + msg + "]");
    }
    if ((cmdStart + cmdLength) != (int) inMsg.position()) {
        throw tcpip::SocketException("#Error: command at position " + toString(cmdStart) + " has wrong length");
    }
}


void
TraCIAPI::check_commandGetResult(tcpip::Storage& inMsg, int command, int expectedType, bool ignoreCommandId) const {
    inMsg.position(); // respStart
    int length = inMsg.readUnsignedByte();
    if (length == 0) {
        length = inMsg.readInt();
    }
    int cmdId = inMsg.readUnsignedByte();
    if (!ignoreCommandId && cmdId != (command + 0x10)) {
        throw tcpip::SocketException("#Error: received response with command id: " + toString(cmdId) + "but expected: " + toString(command + 0x10));
    }
    if (expectedType >= 0) {
        int valueDataType = inMsg.readUnsignedByte();
        if (valueDataType != expectedType) {
            throw tcpip::SocketException("Expected " + toString(expectedType) + " but got " + toString(valueDataType));
        }
    }
}


void
TraCIAPI::processGET(tcpip::Storage& inMsg, int command, int expectedType, bool ignoreCommandId) const {
    check_resultState(inMsg, command, ignoreCommandId);
    check_commandGetResult(inMsg, command, expectedType, ignoreCommandId);
}




SUMOTime
TraCIAPI::getSUMOTime(int cmd, int var, const std::string& id, tcpip::Storage* add) {
    tcpip::Storage inMsg;
    send_commandGetVariable(cmd, var, id, add);
    processGET(inMsg, cmd, TYPE_INTEGER);
    return inMsg.readInt();
}


int
TraCIAPI::getUnsignedByte(int cmd, int var, const std::string& id, tcpip::Storage* add) {
    tcpip::Storage inMsg;
    send_commandGetVariable(cmd, var, id, add);
    processGET(inMsg, cmd, TYPE_UBYTE);
    return inMsg.readUnsignedByte();
}


int
TraCIAPI::getByte(int cmd, int var, const std::string& id, tcpip::Storage* add) {
    tcpip::Storage inMsg;
    send_commandGetVariable(cmd, var, id, add);
    processGET(inMsg, cmd, TYPE_BYTE);
    return inMsg.readByte();
}


int
TraCIAPI::getInt(int cmd, int var, const std::string& id, tcpip::Storage* add) {
    tcpip::Storage inMsg;
    send_commandGetVariable(cmd, var, id, add);
    processGET(inMsg, cmd, TYPE_INTEGER);
    return inMsg.readInt();
}


SUMOReal
TraCIAPI::getFloat(int cmd, int var, const std::string& id, tcpip::Storage* add) {
    tcpip::Storage inMsg;
    send_commandGetVariable(cmd, var, id, add);
    processGET(inMsg, cmd, TYPE_FLOAT);
    return inMsg.readFloat();
}


SUMOReal
TraCIAPI::getDouble(int cmd, int var, const std::string& id, tcpip::Storage* add) {
    tcpip::Storage inMsg;
    send_commandGetVariable(cmd, var, id, add);
    processGET(inMsg, cmd, TYPE_DOUBLE);
    return inMsg.readDouble();
}


TraCIAPI::TraCIBoundary
TraCIAPI::getBoundingBox(int cmd, int var, const std::string& id, tcpip::Storage* add) {
    tcpip::Storage inMsg;
    send_commandGetVariable(cmd, var, id, add);
    processGET(inMsg, cmd, TYPE_BOUNDINGBOX);
    TraCIBoundary b;
    b.xMin = inMsg.readDouble();
    b.yMin = inMsg.readDouble();
    b.zMin = 0;
    b.xMax = inMsg.readDouble();
    b.yMax = inMsg.readDouble();
    b.zMax = 0;
    return b;
}


TraCIAPI::TraCIPositionVector
TraCIAPI::getPolygon(int cmd, int var, const std::string& id, tcpip::Storage* add) {
    tcpip::Storage inMsg;
    send_commandGetVariable(cmd, var, id, add);
    processGET(inMsg, cmd, TYPE_POLYGON);
    unsigned int size = inMsg.readInt();
    TraCIPositionVector ret;
    for (unsigned int i = 0; i < size; ++i) {
        TraCIPosition p;
        p.x = inMsg.readDouble();
        p.y = inMsg.readDouble();
        p.z = 0;
        ret.push_back(p);
    }
    return ret;
}


TraCIAPI::TraCIPosition
TraCIAPI::getPosition(int cmd, int var, const std::string& id, tcpip::Storage* add) {
    tcpip::Storage inMsg;
    send_commandGetVariable(cmd, var, id, add);
    processGET(inMsg, cmd, POSITION_2D);
    TraCIPosition p;
    p.x = inMsg.readDouble();
    p.y = inMsg.readDouble();
    p.z = 0;
    return p;
}


std::string
TraCIAPI::getString(int cmd, int var, const std::string& id, tcpip::Storage* add) {
    tcpip::Storage inMsg;
    send_commandGetVariable(cmd, var, id, add);
    processGET(inMsg, cmd, TYPE_STRING);
    return inMsg.readString();
}


std::vector<std::string>
TraCIAPI::getStringVector(int cmd, int var, const std::string& id, tcpip::Storage* add) {
    tcpip::Storage inMsg;
    send_commandGetVariable(cmd, var, id, add);
    processGET(inMsg, cmd, TYPE_STRINGLIST);
    unsigned int size = inMsg.readInt();
    std::vector<std::string> r;
    for (unsigned int i = 0; i < size; ++i) {
        r.push_back(inMsg.readString());
    }
    return r;
}


TraCIAPI::TraCIColor
TraCIAPI::getColor(int cmd, int var, const std::string& id, tcpip::Storage* add) {
    tcpip::Storage inMsg;
    send_commandGetVariable(cmd, var, id, add);
    processGET(inMsg, cmd, TYPE_COLOR);
    TraCIColor c;
    c.r = inMsg.readUnsignedByte();
    c.g = inMsg.readUnsignedByte();
    c.b = inMsg.readUnsignedByte();
    c.a = inMsg.readUnsignedByte();
    return c;
}



// ---------------------------------------------------------------------------
// TraCIAPI::EdgeScope-methods
// ---------------------------------------------------------------------------
std::vector<std::string>
TraCIAPI::EdgeScope::getIDList() const {
    return myParent.getStringVector(CMD_GET_EDGE_VARIABLE, ID_LIST, "");
}

unsigned int
TraCIAPI::EdgeScope::getIDCount() const {
    return myParent.getInt(CMD_GET_EDGE_VARIABLE, ID_COUNT, "");
}

SUMOReal
TraCIAPI::EdgeScope::getAdaptedTraveltime(const std::string& edgeID, SUMOTime time) const {
    tcpip::Storage content;
    content.writeInt(time);
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, VAR_CO2EMISSION, edgeID, &content);
}

SUMOReal
TraCIAPI::EdgeScope::getEffort(const std::string& edgeID, SUMOTime time) const {
    tcpip::Storage content;
    content.writeInt(time);
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, VAR_CO2EMISSION, edgeID, &content);
}

SUMOReal
TraCIAPI::EdgeScope::getCO2Emission(const std::string& edgeID) const {
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, VAR_CO2EMISSION, edgeID);
}

SUMOReal
TraCIAPI::EdgeScope::getCOEmission(const std::string& edgeID) const {
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, VAR_COEMISSION, edgeID);
}

SUMOReal
TraCIAPI::EdgeScope::getHCEmission(const std::string& edgeID) const {
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, VAR_HCEMISSION, edgeID);
}

SUMOReal
TraCIAPI::EdgeScope::getPMxEmission(const std::string& edgeID) const {
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, VAR_PMXEMISSION, edgeID);
}

SUMOReal
TraCIAPI::EdgeScope::getNOxEmission(const std::string& edgeID) const {
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, VAR_NOXEMISSION, edgeID);
}

SUMOReal
TraCIAPI::EdgeScope::getFuelConsumption(const std::string& edgeID) const {
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, VAR_FUELCONSUMPTION, edgeID);
}

SUMOReal
TraCIAPI::EdgeScope::getNoiseEmission(const std::string& edgeID) const {
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, VAR_NOISEEMISSION, edgeID);
}

SUMOReal
TraCIAPI::EdgeScope::getLastStepMeanSpeed(const std::string& edgeID) const {
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, LAST_STEP_MEAN_SPEED, edgeID);
}

SUMOReal
TraCIAPI::EdgeScope::getLastStepOccupancy(const std::string& edgeID) const {
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, LAST_STEP_OCCUPANCY, edgeID);
}

SUMOReal
TraCIAPI::EdgeScope::getLastStepLength(const std::string& edgeID) const {
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, LAST_STEP_LENGTH, edgeID);
}

SUMOReal
TraCIAPI::EdgeScope::getTraveltime(const std::string& edgeID) const {
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, VAR_CURRENT_TRAVELTIME, edgeID);
}

unsigned int
TraCIAPI::EdgeScope::getLastStepVehicleNumber(const std::string& edgeID) const {
    return myParent.getInt(CMD_GET_EDGE_VARIABLE, LAST_STEP_VEHICLE_NUMBER, edgeID);
}

SUMOReal
TraCIAPI::EdgeScope::getLastStepHaltingNumber(const std::string& edgeID) const {
    return myParent.getInt(CMD_GET_EDGE_VARIABLE, LAST_STEP_VEHICLE_HALTING_NUMBER, edgeID);
}

std::vector<std::string>
TraCIAPI::EdgeScope::getLastStepVehicleIDs(const std::string& edgeID) const {
    return myParent.getStringVector(CMD_GET_EDGE_VARIABLE, LAST_STEP_VEHICLE_ID_LIST, edgeID);
}



void
TraCIAPI::EdgeScope::adaptTraveltime(const std::string& edgeID, SUMOReal time) const {
    tcpip::Storage content;
    content.writeDouble(time);
    myParent.send_commandSetValue(CMD_SET_EDGE_VARIABLE, VAR_EDGE_TRAVELTIME, edgeID, content);
}

void
TraCIAPI::EdgeScope::setEffort(const std::string& edgeID, SUMOReal effort) const {
    tcpip::Storage content;
    content.writeDouble(effort);
    myParent.send_commandSetValue(CMD_SET_EDGE_VARIABLE, VAR_EDGE_EFFORT, edgeID, content);
}

void
TraCIAPI::EdgeScope::setMaxSpeed(const std::string& edgeID, SUMOReal speed) const {
    tcpip::Storage content;
    content.writeDouble(speed);
    myParent.send_commandSetValue(CMD_SET_EDGE_VARIABLE, VAR_MAXSPEED, edgeID, content);
}




// ---------------------------------------------------------------------------
// TraCIAPI::GUIScope-methods
// ---------------------------------------------------------------------------
std::vector<std::string>
TraCIAPI::GUIScope::getIDList() const {
    return myParent.getStringVector(CMD_GET_GUI_VARIABLE, ID_LIST, "");
}

SUMOReal
TraCIAPI::GUIScope::getZoom(const std::string& viewID) const {
    return myParent.getDouble(CMD_GET_GUI_VARIABLE, VAR_VIEW_ZOOM, viewID);
}

TraCIAPI::TraCIPosition
TraCIAPI::GUIScope::getOffset(const std::string& viewID) const {
    return myParent.getPosition(CMD_GET_GUI_VARIABLE, VAR_VIEW_OFFSET, viewID);
}

std::string
TraCIAPI::GUIScope::getSchema(const std::string& viewID) const {
    return myParent.getString(CMD_GET_GUI_VARIABLE, VAR_VIEW_SCHEMA, viewID);
}

TraCIAPI::TraCIBoundary
TraCIAPI::GUIScope::getBoundary(const std::string& viewID) const {
    return myParent.getBoundingBox(CMD_GET_GUI_VARIABLE, VAR_VIEW_BOUNDARY, viewID);
}


void
TraCIAPI::GUIScope::setZoom(const std::string& viewID, SUMOReal zoom) const {
    tcpip::Storage content;
    content.writeDouble(zoom);
    myParent.send_commandSetValue(CMD_SET_GUI_VARIABLE, VAR_VIEW_ZOOM, viewID, content);
}

void
TraCIAPI::GUIScope::setOffset(const std::string& viewID, SUMOReal x, SUMOReal y) const {
    tcpip::Storage content;
    content.writeUnsignedByte(POSITION_2D);
    content.writeDouble(x);
    content.writeDouble(y);
    myParent.send_commandSetValue(CMD_SET_GUI_VARIABLE, VAR_VIEW_OFFSET, viewID, content);
}

void
TraCIAPI::GUIScope::setSchema(const std::string& viewID, const std::string& schemeName) const {
    tcpip::Storage content;
    content.writeString(schemeName);
    myParent.send_commandSetValue(CMD_SET_GUI_VARIABLE, VAR_VIEW_SCHEMA, viewID, content);
}

void
TraCIAPI::GUIScope::setBoundary(const std::string& viewID, SUMOReal xmin, SUMOReal ymin, SUMOReal xmax, SUMOReal ymax) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_BOUNDINGBOX);
    content.writeDouble(xmin);
    content.writeDouble(ymin);
    content.writeDouble(xmax);
    content.writeDouble(ymax);
    myParent.send_commandSetValue(CMD_SET_GUI_VARIABLE, VAR_VIEW_BOUNDARY, viewID, content);
}

void
TraCIAPI::GUIScope::screenshot(const std::string& viewID, const std::string& filename) const {
    tcpip::Storage content;
    content.writeString(filename);
    myParent.send_commandSetValue(CMD_SET_GUI_VARIABLE, VAR_SCREENSHOT, viewID, content);
}

void
TraCIAPI::GUIScope::trackVehicle(const std::string& viewID, const std::string& vehID) const {
    tcpip::Storage content;
    content.writeString(vehID);
    myParent.send_commandSetValue(CMD_SET_GUI_VARIABLE, VAR_TRACK_VEHICLE, viewID, content);
}




// ---------------------------------------------------------------------------
// TraCIAPI::InductionLoopScope-methods
// ---------------------------------------------------------------------------
std::vector<std::string>
TraCIAPI::InductionLoopScope::getIDList() const {
    return myParent.getStringVector(CMD_GET_INDUCTIONLOOP_VARIABLE, ID_LIST, "");
}

SUMOReal
TraCIAPI::InductionLoopScope::getPosition(const std::string& loopID) const {
    return myParent.getDouble(CMD_GET_INDUCTIONLOOP_VARIABLE, VAR_POSITION, loopID);
}

std::string
TraCIAPI::InductionLoopScope::getLaneID(const std::string& loopID) const {
    return myParent.getString(CMD_GET_INDUCTIONLOOP_VARIABLE, VAR_LANE_ID, loopID);
}

unsigned int
TraCIAPI::InductionLoopScope::getLastStepVehicleNumber(const std::string& loopID) const {
    return myParent.getInt(CMD_GET_INDUCTIONLOOP_VARIABLE, LAST_STEP_VEHICLE_NUMBER, loopID);
}

SUMOReal
TraCIAPI::InductionLoopScope::getLastStepMeanSpeed(const std::string& loopID) const {
    return myParent.getDouble(CMD_GET_INDUCTIONLOOP_VARIABLE, LAST_STEP_MEAN_SPEED, loopID);
}

std::vector<std::string>
TraCIAPI::InductionLoopScope::getLastStepVehicleIDs(const std::string& loopID) const {
    return myParent.getStringVector(CMD_GET_INDUCTIONLOOP_VARIABLE, LAST_STEP_VEHICLE_ID_LIST, loopID);
}

SUMOReal
TraCIAPI::InductionLoopScope::getLastStepOccupancy(const std::string& loopID) const {
    return myParent.getDouble(CMD_GET_INDUCTIONLOOP_VARIABLE, LAST_STEP_OCCUPANCY, loopID);
}

SUMOReal
TraCIAPI::InductionLoopScope::getLastStepMeanLength(const std::string& loopID) const {
    return myParent.getDouble(CMD_GET_INDUCTIONLOOP_VARIABLE, LAST_STEP_LENGTH, loopID);
}

SUMOReal
TraCIAPI::InductionLoopScope::getTimeSinceDetection(const std::string& loopID) const {
    return myParent.getDouble(CMD_GET_INDUCTIONLOOP_VARIABLE, LAST_STEP_TIME_SINCE_DETECTION, loopID);
}

unsigned int
TraCIAPI::InductionLoopScope::getVehicleData(const std::string& loopID) const {
    return myParent.getInt(CMD_GET_INDUCTIONLOOP_VARIABLE, LAST_STEP_VEHICLE_DATA, loopID);
}




// ---------------------------------------------------------------------------
// TraCIAPI::JunctionScope-methods
// ---------------------------------------------------------------------------
std::vector<std::string>
TraCIAPI::JunctionScope::getIDList() const {
    return myParent.getStringVector(CMD_GET_JUNCTION_VARIABLE, ID_LIST, "");
}

TraCIAPI::TraCIPosition
TraCIAPI::JunctionScope::getPosition(const std::string& junctionID) const {
    return myParent.getPosition(CMD_GET_JUNCTION_VARIABLE, VAR_POSITION, junctionID);
}




// ---------------------------------------------------------------------------
// TraCIAPI::LaneScope-methods
// ---------------------------------------------------------------------------
std::vector<std::string>
TraCIAPI::LaneScope::getIDList() const {
    return myParent.getStringVector(CMD_GET_LANE_VARIABLE, ID_LIST, "");
}

SUMOReal
TraCIAPI::LaneScope::getLength(const std::string& laneID) const {
    return myParent.getDouble(CMD_GET_LANE_VARIABLE, VAR_LENGTH, laneID);
}

SUMOReal
TraCIAPI::LaneScope::getMaxSpeed(const std::string& laneID) const {
    return myParent.getDouble(CMD_GET_LANE_VARIABLE, VAR_MAXSPEED, laneID);
}

SUMOReal
TraCIAPI::LaneScope::getWidth(const std::string& laneID) const {
    return myParent.getDouble(CMD_GET_LANE_VARIABLE, VAR_WIDTH, laneID);
}

std::vector<std::string>
TraCIAPI::LaneScope::getAllowed(const std::string& laneID) const {
    return myParent.getStringVector(CMD_GET_LANE_VARIABLE, LANE_ALLOWED, laneID);
}

std::vector<std::string>
TraCIAPI::LaneScope::getDisallowed(const std::string& laneID) const {
    return myParent.getStringVector(CMD_GET_LANE_VARIABLE, LANE_DISALLOWED, laneID);
}

unsigned int
TraCIAPI::LaneScope::getLinkNumber(const std::string& /* laneID */) const {
    throw tcpip::SocketException("Not implemented!");
}

TraCIAPI::TraCIPositionVector
TraCIAPI::LaneScope::getShape(const std::string& laneID) const {
    throw myParent.getPolygon(CMD_GET_LANE_VARIABLE, VAR_SHAPE, laneID);
}

std::string
TraCIAPI::LaneScope::getEdgeID(const std::string& laneID) const {
    throw myParent.getString(CMD_GET_LANE_VARIABLE, LANE_EDGE_ID, laneID);
}

SUMOReal
TraCIAPI::LaneScope::getCO2Emission(const std::string& laneID) const {
    throw myParent.getDouble(CMD_GET_LANE_VARIABLE, VAR_CO2EMISSION, laneID);
}

SUMOReal
TraCIAPI::LaneScope::getCOEmission(const std::string& laneID) const {
    throw myParent.getDouble(CMD_GET_LANE_VARIABLE, VAR_COEMISSION, laneID);
}

SUMOReal
TraCIAPI::LaneScope::getHCEmission(const std::string& laneID) const {
    throw myParent.getDouble(CMD_GET_LANE_VARIABLE, VAR_HCEMISSION, laneID);
}

SUMOReal
TraCIAPI::LaneScope::getPMxEmission(const std::string& laneID) const {
    throw myParent.getDouble(CMD_GET_LANE_VARIABLE, VAR_PMXEMISSION, laneID);
}

SUMOReal
TraCIAPI::LaneScope::getNOxEmission(const std::string& laneID) const {
    throw myParent.getDouble(CMD_GET_LANE_VARIABLE, VAR_NOXEMISSION, laneID);
}

SUMOReal
TraCIAPI::LaneScope::getFuelConsumption(const std::string& laneID) const {
    throw myParent.getDouble(CMD_GET_LANE_VARIABLE, VAR_FUELCONSUMPTION, laneID);
}

SUMOReal
TraCIAPI::LaneScope::getNoiseEmission(const std::string& laneID) const {
    throw myParent.getDouble(CMD_GET_LANE_VARIABLE, VAR_NOISEEMISSION, laneID);
}

SUMOReal
TraCIAPI::LaneScope::getLastStepMeanSpeed(const std::string& laneID) const {
    throw myParent.getDouble(CMD_GET_LANE_VARIABLE, LAST_STEP_MEAN_SPEED, laneID);
}

SUMOReal
TraCIAPI::LaneScope::getLastStepOccupancy(const std::string& laneID) const {
    throw myParent.getDouble(CMD_GET_LANE_VARIABLE, LAST_STEP_OCCUPANCY, laneID);
}

SUMOReal
TraCIAPI::LaneScope::getLastStepLength(const std::string& laneID) const {
    throw myParent.getDouble(CMD_GET_LANE_VARIABLE, LAST_STEP_LENGTH, laneID);
}

SUMOReal
TraCIAPI::LaneScope::getTraveltime(const std::string& laneID) const {
    throw myParent.getDouble(CMD_GET_LANE_VARIABLE, VAR_CURRENT_TRAVELTIME, laneID);
}

unsigned int
TraCIAPI::LaneScope::getLastStepVehicleNumber(const std::string& laneID) const {
    throw myParent.getInt(CMD_GET_LANE_VARIABLE, LAST_STEP_VEHICLE_NUMBER, laneID);
}

unsigned int
TraCIAPI::LaneScope::getLastStepHaltingNumber(const std::string& laneID) const {
    throw myParent.getInt(CMD_GET_LANE_VARIABLE, LAST_STEP_VEHICLE_HALTING_NUMBER, laneID);
}

std::vector<std::string>
TraCIAPI::LaneScope::getLastStepVehicleIDs(const std::string& laneID) const {
    throw myParent.getStringVector(CMD_GET_LANE_VARIABLE, LAST_STEP_VEHICLE_ID_LIST, laneID);
}


void
TraCIAPI::LaneScope::setAllowed(const std::string& laneID, const std::vector<std::string>& allowedClasses) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_STRINGLIST);
    content.writeInt((int)allowedClasses.size());
    for (unsigned int i = 0; i < allowedClasses.size(); ++i) {
        content.writeString(allowedClasses[i]);
    }
    myParent.send_commandSetValue(CMD_SET_LANE_VARIABLE, LANE_ALLOWED, laneID, content);
}

void
TraCIAPI::LaneScope::setDisallowed(const std::string& laneID, const std::vector<std::string>& disallowedClasses) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_STRINGLIST);
    content.writeInt((int)disallowedClasses.size());
    for (unsigned int i = 0; i < disallowedClasses.size(); ++i) {
        content.writeString(disallowedClasses[i]);
    }
    myParent.send_commandSetValue(CMD_SET_LANE_VARIABLE, LANE_DISALLOWED, laneID, content);
}

void
TraCIAPI::LaneScope::setMaxSpeed(const std::string& laneID, SUMOReal speed) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(speed);
    myParent.send_commandSetValue(CMD_SET_LANE_VARIABLE, VAR_MAXSPEED, laneID, content);
}

void
TraCIAPI::LaneScope::setLength(const std::string& laneID, SUMOReal length) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(length);
    myParent.send_commandSetValue(CMD_SET_LANE_VARIABLE, VAR_LENGTH, laneID, content);
}



// ---------------------------------------------------------------------------
// TraCIAPI::MeMeScope-methods
// ---------------------------------------------------------------------------
std::vector<std::string>
TraCIAPI::MeMeScope::getIDList() const {
    return myParent.getStringVector(CMD_GET_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE, ID_LIST, "");
}

unsigned int
TraCIAPI::MeMeScope::getLastStepVehicleNumber(const std::string& detID) const {
    return myParent.getInt(CMD_GET_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE, LAST_STEP_VEHICLE_NUMBER, detID);
}

SUMOReal
TraCIAPI::MeMeScope::getLastStepMeanSpeed(const std::string& detID) const {
    return myParent.getInt(CMD_GET_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE, LAST_STEP_MEAN_SPEED, detID);
}

std::vector<std::string>
TraCIAPI::MeMeScope::getLastStepVehicleIDs(const std::string& detID) const {
    return myParent.getStringVector(CMD_GET_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE, LAST_STEP_VEHICLE_ID_LIST, detID);
}

unsigned int
TraCIAPI::MeMeScope::getLastStepHaltingNumber(const std::string& detID) const {
    return myParent.getInt(CMD_GET_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE, LAST_STEP_VEHICLE_HALTING_NUMBER, detID);
}



// ---------------------------------------------------------------------------
// TraCIAPI::POIScope-methods
// ---------------------------------------------------------------------------
std::vector<std::string>
TraCIAPI::POIScope::getIDList() const {
    return myParent.getStringVector(CMD_GET_POI_VARIABLE, ID_LIST, "");
}

std::string
TraCIAPI::POIScope::getType(const std::string& poiID) const {
    return myParent.getString(CMD_GET_POI_VARIABLE, VAR_TYPE, poiID);
}

TraCIAPI::TraCIPosition
TraCIAPI::POIScope::getPosition(const std::string& poiID) const {
    return myParent.getPosition(CMD_GET_POI_VARIABLE, VAR_POSITION, poiID);
}

TraCIAPI::TraCIColor
TraCIAPI::POIScope::getColor(const std::string& poiID) const {
    return myParent.getColor(CMD_GET_POI_VARIABLE, VAR_COLOR, poiID);
}


void
TraCIAPI::POIScope::setType(const std::string& poiID, const std::string& setType) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(setType);
    myParent.send_commandSetValue(CMD_SET_POI_VARIABLE, VAR_TYPE, poiID, content);
}

void
TraCIAPI::POIScope::setPosition(const std::string& poiID, SUMOReal x, SUMOReal y) const {
    tcpip::Storage content;
    content.writeUnsignedByte(POSITION_2D);
    content.writeDouble(x);
    content.writeDouble(y);
    myParent.send_commandSetValue(CMD_SET_POI_VARIABLE, VAR_POSITION, poiID, content);
}

void
TraCIAPI::POIScope::setColor(const std::string& poiID, const TraCIColor& c) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_COLOR);
    content.writeUnsignedByte(c.r);
    content.writeUnsignedByte(c.g);
    content.writeUnsignedByte(c.b);
    content.writeUnsignedByte(c.a);
    myParent.send_commandSetValue(CMD_SET_POI_VARIABLE, VAR_COLOR, poiID, content);
}

void
TraCIAPI::POIScope::add(const std::string& poiID, SUMOReal x, SUMOReal y, const TraCIColor& c, const std::string& type, int layer) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_COMPOUND);
    content.writeInt(4);
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(type);
    content.writeUnsignedByte(TYPE_COLOR);
    content.writeUnsignedByte(c.r);
    content.writeUnsignedByte(c.g);
    content.writeUnsignedByte(c.b);
    content.writeUnsignedByte(c.a);
    content.writeUnsignedByte(TYPE_INTEGER);
    content.writeInt(layer);
    content.writeUnsignedByte(POSITION_2D);
    content.writeDouble(x);
    content.writeDouble(y);
    myParent.send_commandSetValue(CMD_SET_POI_VARIABLE, ADD, poiID, content);
}

void
TraCIAPI::POIScope::remove(const std::string& poiID, int layer) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_INTEGER);
    content.writeInt(layer);
    myParent.send_commandSetValue(CMD_SET_POI_VARIABLE, REMOVE, poiID, content);
}



// ---------------------------------------------------------------------------
// TraCIAPI::PolygonScope-methods
// ---------------------------------------------------------------------------
std::vector<std::string>
TraCIAPI::PolygonScope::getIDList() const {
    return myParent.getStringVector(CMD_GET_POLYGON_VARIABLE, ID_LIST, "");
}

std::string
TraCIAPI::PolygonScope::getType(const std::string& polygonID) const {
    return myParent.getString(CMD_GET_POLYGON_VARIABLE, VAR_TYPE, polygonID);
}

TraCIAPI::TraCIPositionVector
TraCIAPI::PolygonScope::getShape(const std::string& polygonID) const {
    return myParent.getPolygon(CMD_GET_POLYGON_VARIABLE, VAR_SHAPE, polygonID);
}

TraCIAPI::TraCIColor
TraCIAPI::PolygonScope::getColor(const std::string& polygonID) const {
    return myParent.getColor(CMD_GET_POLYGON_VARIABLE, VAR_COLOR, polygonID);
}


void
TraCIAPI::PolygonScope::setType(const std::string& polygonID, const std::string& setType) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(setType);
    myParent.send_commandSetValue(CMD_SET_POLYGON_VARIABLE, VAR_TYPE, polygonID, content);
}

void
TraCIAPI::PolygonScope::setShape(const std::string& polygonID, const TraCIAPI::TraCIPositionVector& shape) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_POLYGON);
    content.writeInt((int)shape.size());
    for (unsigned int i = 0; i < shape.size(); ++i) {
        content.writeDouble(shape[i].x);
        content.writeDouble(shape[i].y);
    }
    myParent.send_commandSetValue(CMD_SET_POLYGON_VARIABLE, VAR_POSITION, polygonID, content);
}

void
TraCIAPI::PolygonScope::setColor(const std::string& polygonID, const TraCIColor& c) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_COLOR);
    content.writeUnsignedByte(c.r);
    content.writeUnsignedByte(c.g);
    content.writeUnsignedByte(c.b);
    content.writeUnsignedByte(c.a);
    myParent.send_commandSetValue(CMD_SET_POLYGON_VARIABLE, VAR_COLOR, polygonID, content);
}

void
TraCIAPI::PolygonScope::add(const std::string& polygonID, const TraCIAPI::TraCIPositionVector& shape, const TraCIColor& c, bool fill, const std::string& type, int layer) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_COMPOUND);
    content.writeInt(4);
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(type);
    content.writeUnsignedByte(TYPE_COLOR);
    content.writeUnsignedByte(c.r);
    content.writeUnsignedByte(c.g);
    content.writeUnsignedByte(c.b);
    content.writeUnsignedByte(c.a);
    content.writeUnsignedByte(TYPE_UBYTE);
    int f = fill ? 1 : 0;
    content.writeUnsignedByte(f);
    content.writeUnsignedByte(TYPE_INTEGER);
    content.writeInt(layer);
    content.writeUnsignedByte(TYPE_POLYGON);
    content.writeInt((int)shape.size());
    for (unsigned int i = 0; i < shape.size(); ++i) {
        content.writeDouble(shape[i].x);
        content.writeDouble(shape[i].y);
    }
    myParent.send_commandSetValue(CMD_SET_POLYGON_VARIABLE, ADD, polygonID, content);
}

void
TraCIAPI::PolygonScope::remove(const std::string& polygonID, int layer) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_INTEGER);
    content.writeInt(layer);
    myParent.send_commandSetValue(CMD_SET_POLYGON_VARIABLE, REMOVE, polygonID, content);
}



// ---------------------------------------------------------------------------
// TraCIAPI::RouteScope-methods
// ---------------------------------------------------------------------------
std::vector<std::string>
TraCIAPI::RouteScope::getIDList() const {
    return myParent.getStringVector(CMD_GET_ROUTE_VARIABLE, ID_LIST, "");
}

std::vector<std::string>
TraCIAPI::RouteScope::getEdges(const std::string& routeID) const {
    return myParent.getStringVector(CMD_GET_ROUTE_VARIABLE, VAR_EDGES, routeID);
}


void
TraCIAPI::RouteScope::add(const std::string& routeID, const std::vector<std::string>& edges) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_STRINGLIST);
    content.writeStringList(edges);
    myParent.send_commandSetValue(CMD_SET_ROUTE_VARIABLE, VAR_POSITION, routeID, content);
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

TraCIAPI::TraCIBoundary
TraCIAPI::SimulationScope::getNetBoundary() const {
    return myParent.getBoundingBox(CMD_GET_SIM_VARIABLE, VAR_NET_BOUNDING_BOX, "");
}

unsigned int
TraCIAPI::SimulationScope::getMinExpectedNumber() const {
    return myParent.getInt(CMD_GET_SIM_VARIABLE, VAR_MIN_EXPECTED_VEHICLES, "");
}



// ---------------------------------------------------------------------------
// TraCIAPI::TrafficLightScope-methods
// ---------------------------------------------------------------------------
std::vector<std::string>
TraCIAPI::TrafficLightScope::getIDList() const {
    return myParent.getStringVector(CMD_GET_TL_VARIABLE, ID_LIST, "");
}

std::string
TraCIAPI::TrafficLightScope::getRedYellowGreenState(const std::string& tlsID) const {
    return myParent.getString(CMD_GET_TL_VARIABLE, TL_RED_YELLOW_GREEN_STATE, tlsID);
}

std::vector<TraCIAPI::TraCILogic>
TraCIAPI::TrafficLightScope::getCompleteRedYellowGreenDefinition(const std::string& tlsID) const {
    tcpip::Storage inMsg;
    myParent.send_commandGetVariable(CMD_GET_TL_VARIABLE, TL_COMPLETE_DEFINITION_RYG, tlsID);
    myParent.processGET(inMsg, CMD_GET_TL_VARIABLE, TYPE_COMPOUND);
    std::vector<TraCIAPI::TraCILogic> ret;
    int logicNo = inMsg.readInt();
    for (int i = 0; i < logicNo; ++i) {
        inMsg.readUnsignedByte();
        std::string subID = inMsg.readString();
        inMsg.readUnsignedByte();
        int type = inMsg.readInt();
        inMsg.readUnsignedByte();
        inMsg.readInt(); // add
        inMsg.readUnsignedByte();
        int phaseIndex = inMsg.readInt();
        inMsg.readUnsignedByte();
        int phaseNumber = inMsg.readInt();
        std::vector<TraCIAPI::TraCIPhase> phases;
        for (int j = 0; j < phaseNumber; ++j) {
            inMsg.readUnsignedByte();
            int duration = inMsg.readInt();
            inMsg.readUnsignedByte();
            int duration1 = inMsg.readInt();
            inMsg.readUnsignedByte();
            int duration2 = inMsg.readInt();
            inMsg.readUnsignedByte();
            std::string phase = inMsg.readString();
            phases.push_back(TraCIAPI::TraCIPhase(duration, duration1, duration2, phase));
        }
        ret.push_back(TraCIAPI::TraCILogic(subID, type, std::map<std::string, SUMOReal>(), phaseIndex, phases));
    }
    return ret;
}

std::vector<std::string>
TraCIAPI::TrafficLightScope::getControlledLanes(const std::string& tlsID) const {
    return myParent.getStringVector(CMD_GET_TL_VARIABLE, TL_CONTROLLED_LANES, tlsID);
}

std::vector<TraCIAPI::TraCILink>
TraCIAPI::TrafficLightScope::getControlledLinks(const std::string& tlsID) const {
    tcpip::Storage inMsg;
    myParent.send_commandGetVariable(CMD_GET_TL_VARIABLE, TL_CONTROLLED_LINKS, tlsID);
    myParent.processGET(inMsg, CMD_GET_TL_VARIABLE, TYPE_COMPOUND);
    std::vector<TraCIAPI::TraCILink> ret;
    int linkNo = inMsg.readInt();
    for (int i = 0; i < linkNo; ++i) {
        inMsg.readUnsignedByte();
        std::string from = inMsg.readString();
        inMsg.readUnsignedByte();
        std::string via = inMsg.readString();
        inMsg.readUnsignedByte();
        std::string to = inMsg.readString();
        ret.push_back(TraCIAPI::TraCILink(from, via, to));
    }
    return ret;
}

std::string
TraCIAPI::TrafficLightScope::getProgram(const std::string& tlsID) const {
    return myParent.getString(CMD_GET_TL_VARIABLE, TL_CURRENT_PROGRAM, tlsID);
}

unsigned int
TraCIAPI::TrafficLightScope::getPhase(const std::string& tlsID) const {
    return myParent.getInt(CMD_GET_TL_VARIABLE, TL_CURRENT_PHASE, tlsID);
}

unsigned int
TraCIAPI::TrafficLightScope::getNextSwitch(const std::string& tlsID) const {
    return myParent.getInt(CMD_GET_TL_VARIABLE, TL_NEXT_SWITCH, tlsID);
}


void
TraCIAPI::TrafficLightScope::setRedYellowGreenState(const std::string& tlsID, const std::string& state) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(state);
    myParent.send_commandSetValue(CMD_SET_TL_VARIABLE, TL_RED_YELLOW_GREEN_STATE, tlsID, content);
}

void
TraCIAPI::TrafficLightScope::setPhase(const std::string& tlsID, unsigned int index) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_INTEGER);
    content.writeInt(index);
    myParent.send_commandSetValue(CMD_SET_TL_VARIABLE, TL_PHASE_INDEX, tlsID, content);
}

void
TraCIAPI::TrafficLightScope::setProgram(const std::string& tlsID, const std::string& programID) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(programID);
    myParent.send_commandSetValue(CMD_SET_TL_VARIABLE, TL_PROGRAM, tlsID, content);
}

void
TraCIAPI::TrafficLightScope::setPhaseDuration(const std::string& tlsID, unsigned int phaseDuration) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_INTEGER);
    content.writeInt(int(1000 * phaseDuration));
    myParent.send_commandSetValue(CMD_SET_TL_VARIABLE, TL_PHASE_DURATION, tlsID, content);
}

void
TraCIAPI::TrafficLightScope::setCompleteRedYellowGreenDefinition(const std::string& tlsID, const TraCIAPI::TraCILogic& logic) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_COMPOUND);
    content.writeInt(5 + 4 * (int)logic.phases.size());
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(logic.subID);
    content.writeUnsignedByte(TYPE_INTEGER);
    content.writeInt(logic.type);
    content.writeUnsignedByte(TYPE_COMPOUND);
    content.writeInt(0);
    content.writeUnsignedByte(TYPE_INTEGER);
    content.writeInt(logic.currentPhaseIndex);
    content.writeUnsignedByte(TYPE_INTEGER);
    content.writeInt((int)logic.phases.size());
    for (int i = 0; i < (int) logic.phases.size(); ++i) {
        content.writeUnsignedByte(TYPE_INTEGER);
        content.writeInt(logic.phases[i].duration);
        content.writeUnsignedByte(TYPE_INTEGER);
        content.writeInt(logic.phases[i].duration1);
        content.writeUnsignedByte(TYPE_INTEGER);
        content.writeInt(logic.phases[i].duration2);
        content.writeUnsignedByte(TYPE_STRING);
        content.writeString(logic.phases[i].phase);
    }
    myParent.send_commandSetValue(CMD_SET_TL_VARIABLE, TL_COMPLETE_PROGRAM_RYG, tlsID, content);
}





// ---------------------------------------------------------------------------
// TraCIAPI::VehicleTypeScope-methods
// ---------------------------------------------------------------------------
std::vector<std::string>
TraCIAPI::VehicleTypeScope::getIDList() const {
    return myParent.getStringVector(CMD_GET_VEHICLETYPE_VARIABLE, ID_LIST, "");
}

SUMOReal
TraCIAPI::VehicleTypeScope::getLength(const std::string& typeID) const {
    return myParent.getDouble(CMD_GET_TL_VARIABLE, VAR_LENGTH, typeID);
}

SUMOReal
TraCIAPI::VehicleTypeScope::getMaxSpeed(const std::string& typeID) const {
    return myParent.getDouble(CMD_GET_TL_VARIABLE, VAR_MAXSPEED, typeID);
}

SUMOReal
TraCIAPI::VehicleTypeScope::getSpeedFactor(const std::string& typeID) const {
    return myParent.getDouble(CMD_GET_TL_VARIABLE, VAR_SPEED_FACTOR, typeID);
}

SUMOReal
TraCIAPI::VehicleTypeScope::getSpeedDeviation(const std::string& typeID) const {
    return myParent.getDouble(CMD_GET_TL_VARIABLE, VAR_SPEED_DEVIATION, typeID);
}

SUMOReal
TraCIAPI::VehicleTypeScope::getAccel(const std::string& typeID) const {
    return myParent.getDouble(CMD_GET_TL_VARIABLE, VAR_ACCEL, typeID);
}

SUMOReal
TraCIAPI::VehicleTypeScope::getDecel(const std::string& typeID) const {
    return myParent.getDouble(CMD_GET_TL_VARIABLE, VAR_DECEL, typeID);
}

SUMOReal
TraCIAPI::VehicleTypeScope::getImperfection(const std::string& typeID) const {
    return myParent.getDouble(CMD_GET_TL_VARIABLE, VAR_IMPERFECTION, typeID);
}

SUMOReal
TraCIAPI::VehicleTypeScope::getTau(const std::string& typeID) const {
    return myParent.getDouble(CMD_GET_TL_VARIABLE, VAR_TAU, typeID);
}

std::string
TraCIAPI::VehicleTypeScope::getVehicleClass(const std::string& typeID) const {
    return myParent.getString(CMD_GET_TL_VARIABLE, VAR_VEHICLECLASS, typeID);
}

std::string
TraCIAPI::VehicleTypeScope::getEmissionClass(const std::string& typeID) const {
    return myParent.getString(CMD_GET_TL_VARIABLE, VAR_EMISSIONCLASS, typeID);
}

std::string
TraCIAPI::VehicleTypeScope::getShapeClass(const std::string& typeID) const {
    return myParent.getString(CMD_GET_TL_VARIABLE, VAR_SHAPECLASS, typeID);
}

SUMOReal
TraCIAPI::VehicleTypeScope::getMinGap(const std::string& typeID) const {
    return myParent.getDouble(CMD_GET_TL_VARIABLE, VAR_MINGAP, typeID);
}

SUMOReal
TraCIAPI::VehicleTypeScope::getWidth(const std::string& typeID) const {
    return myParent.getDouble(CMD_GET_TL_VARIABLE, VAR_WIDTH, typeID);
}

TraCIAPI::TraCIColor
TraCIAPI::VehicleTypeScope::getColor(const std::string& typeID) const {
    return myParent.getColor(CMD_GET_TL_VARIABLE, VAR_COLOR, typeID);
}



void
TraCIAPI::VehicleTypeScope::setLength(const std::string& typeID, SUMOReal length) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(length);
    myParent.send_commandSetValue(CMD_SET_VEHICLETYPE_VARIABLE, VAR_LENGTH, typeID, content);
}

void
TraCIAPI::VehicleTypeScope::setMaxSpeed(const std::string& typeID, SUMOReal speed) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(speed);
    myParent.send_commandSetValue(CMD_SET_VEHICLETYPE_VARIABLE, VAR_MAXSPEED, typeID, content);
}

void
TraCIAPI::VehicleTypeScope::setVehicleClass(const std::string& typeID, const std::string& clazz) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(clazz);
    myParent.send_commandSetValue(CMD_SET_VEHICLETYPE_VARIABLE, VAR_VEHICLECLASS, typeID, content);
}

void
TraCIAPI::VehicleTypeScope::setSpeedFactor(const std::string& typeID, SUMOReal factor) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(factor);
    myParent.send_commandSetValue(CMD_SET_VEHICLETYPE_VARIABLE, VAR_SPEED_FACTOR, typeID, content);
}

void
TraCIAPI::VehicleTypeScope::setSpeedDeviation(const std::string& typeID, SUMOReal deviation) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(deviation);
    myParent.send_commandSetValue(CMD_SET_VEHICLETYPE_VARIABLE, VAR_SPEED_DEVIATION, typeID, content);
}

void
TraCIAPI::VehicleTypeScope::setEmissionClass(const std::string& typeID, const std::string& clazz) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(clazz);
    myParent.send_commandSetValue(CMD_SET_VEHICLETYPE_VARIABLE, VAR_EMISSIONCLASS, typeID, content);
}

void
TraCIAPI::VehicleTypeScope::setWidth(const std::string& typeID, SUMOReal width) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(width);
    myParent.send_commandSetValue(CMD_SET_VEHICLETYPE_VARIABLE, VAR_WIDTH, typeID, content);
}

void
TraCIAPI::VehicleTypeScope::setMinGap(const std::string& typeID, SUMOReal minGap) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(minGap);
    myParent.send_commandSetValue(CMD_SET_VEHICLETYPE_VARIABLE, VAR_MINGAP, typeID, content);
}

void
TraCIAPI::VehicleTypeScope::setShapeClass(const std::string& typeID, const std::string& clazz) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(clazz);
    myParent.send_commandSetValue(CMD_SET_VEHICLETYPE_VARIABLE, VAR_SHAPECLASS, typeID, content);
}

void
TraCIAPI::VehicleTypeScope::setAccel(const std::string& typeID, SUMOReal accel) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(accel);
    myParent.send_commandSetValue(CMD_SET_VEHICLETYPE_VARIABLE, VAR_ACCEL, typeID, content);
}

void
TraCIAPI::VehicleTypeScope::setDecel(const std::string& typeID, SUMOReal decel) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(decel);
    myParent.send_commandSetValue(CMD_SET_VEHICLETYPE_VARIABLE, VAR_DECEL, typeID, content);
}

void
TraCIAPI::VehicleTypeScope::setImperfection(const std::string& typeID, SUMOReal imperfection) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(imperfection);
    myParent.send_commandSetValue(CMD_SET_VEHICLETYPE_VARIABLE, VAR_IMPERFECTION, typeID, content);
}

void
TraCIAPI::VehicleTypeScope::setTau(const std::string& typeID, SUMOReal tau) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(tau);
    myParent.send_commandSetValue(CMD_SET_VEHICLETYPE_VARIABLE, VAR_TAU, typeID, content);
}

void
TraCIAPI::VehicleTypeScope::setColor(const std::string& typeID, const TraCIColor& c) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_COLOR);
    content.writeUnsignedByte(c.r);
    content.writeUnsignedByte(c.g);
    content.writeUnsignedByte(c.b);
    content.writeUnsignedByte(c.a);
    myParent.send_commandSetValue(CMD_SET_VEHICLETYPE_VARIABLE, VAR_COLOR, typeID, content);
}



/****************************************************************************/

