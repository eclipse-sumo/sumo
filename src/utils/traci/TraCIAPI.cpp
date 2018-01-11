/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    TraCIAPI.cpp
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    30.05.2012
/// @version $Id$
///
// C++ TraCI client API implementation
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

using namespace libsumo;


// ===========================================================================
// member definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// TraCIAPI-methods
// ---------------------------------------------------------------------------
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4355)
#endif
TraCIAPI::TraCIAPI()
    : edge(*this), gui(*this), inductionloop(*this),
      junction(*this), lane(*this), lanearea(*this), multientryexit(*this),
      person(*this), poi(*this), polygon(*this), route(*this),
      simulation(*this), trafficlights(*this),
      vehicle(*this), vehicletype(*this),
      mySocket(0) {}
#ifdef _MSC_VER
#pragma warning(pop)
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
TraCIAPI::setOrder(int order) {
    tcpip::Storage outMsg;
    // command length
    outMsg.writeUnsignedByte(1 + 1 + 4);
    // command id
    outMsg.writeUnsignedByte(CMD_SETORDER);
    outMsg.writeInt(order);
    // send request message
    mySocket->sendExact(outMsg);
    tcpip::Storage inMsg;
    check_resultState(inMsg, CMD_SETORDER);
}


void
TraCIAPI::close() {
    send_commandClose();
    tcpip::Storage inMsg;
    std::string acknowledgement;
    check_resultState(inMsg, CMD_CLOSE, false, &acknowledgement);
    closeSocket();
}


void
TraCIAPI::closeSocket() {
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
    outMsg.writeUnsignedByte(CMD_SIMSTEP);
    outMsg.writeInt((int)time);
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
TraCIAPI::send_commandSetOrder(int order) const {
    tcpip::Storage outMsg;
    // command length
    outMsg.writeUnsignedByte(1 + 1 + 4);
    // command id
    outMsg.writeUnsignedByte(CMD_SETORDER);
    // client index
    outMsg.writeInt(order);
    mySocket->sendExact(outMsg);
}


void
TraCIAPI::send_commandGetVariable(int domID, int varID, const std::string& objID, tcpip::Storage* add) const {
    if (mySocket == 0) {
        throw tcpip::SocketException("Socket is not initialised");
    }
    tcpip::Storage outMsg;
    // command length
    int length = 1 + 1 + 1 + 4 + (int) objID.length();
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
TraCIAPI::send_commandSubscribeObjectVariable(int domID, const std::string& objID, SUMOTime beginTime, SUMOTime endTime,
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
    outMsg.writeInt((int)beginTime);
    outMsg.writeInt((int)endTime);
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
TraCIAPI::send_commandSubscribeObjectContext(int domID, const std::string& objID, SUMOTime beginTime, SUMOTime endTime,
        int domain, double range, const std::vector<int>& vars) const {
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
    outMsg.writeInt((int)beginTime);
    outMsg.writeInt((int)endTime);
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
TraCIAPI::send_commandMoveToXY(const std::string& vehicleID, const std::string& edgeID, const int lane, const double x, const double y, const double angle, const int keepRoute) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_COMPOUND);
    content.writeInt(6);
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(edgeID);
    content.writeUnsignedByte(TYPE_INTEGER);
    content.writeInt(lane);
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(x);
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(y);
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(angle);
    content.writeUnsignedByte(TYPE_BYTE);
    content.writeByte(keepRoute);
    send_commandSetValue(CMD_SET_VEHICLE_VARIABLE, MOVE_TO_XY, vehicleID, content);
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


int
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
        // not called from the TraCITestClient but from within the TraCIAPI
        inMsg.readUnsignedByte(); // variableID
        inMsg.readString(); // objectID
        int valueDataType = inMsg.readUnsignedByte();
        if (valueDataType != expectedType) {
            throw tcpip::SocketException("Expected " + toString(expectedType) + " but got " + toString(valueDataType));
        }
    }
    return cmdId;
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


double
TraCIAPI::getFloat(int cmd, int var, const std::string& id, tcpip::Storage* add) {
    tcpip::Storage inMsg;
    send_commandGetVariable(cmd, var, id, add);
    processGET(inMsg, cmd, TYPE_FLOAT);
    return inMsg.readFloat();
}


double
TraCIAPI::getDouble(int cmd, int var, const std::string& id, tcpip::Storage* add) {
    tcpip::Storage inMsg;
    send_commandGetVariable(cmd, var, id, add);
    processGET(inMsg, cmd, TYPE_DOUBLE);
    return inMsg.readDouble();
}


TraCIBoundary
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


TraCIPositionVector
TraCIAPI::getPolygon(int cmd, int var, const std::string& id, tcpip::Storage* add) {
    tcpip::Storage inMsg;
    send_commandGetVariable(cmd, var, id, add);
    processGET(inMsg, cmd, TYPE_POLYGON);
    int size = inMsg.readByte();
    TraCIPositionVector ret;
    for (int i = 0; i < size; ++i) {
        TraCIPosition p;
        p.x = inMsg.readDouble();
        p.y = inMsg.readDouble();
        p.z = 0;
        ret.push_back(p);
    }
    return ret;
}


TraCIPosition
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


TraCIPosition
TraCIAPI::getPosition3D(int cmd, int var, const std::string& id, tcpip::Storage* add) {
    tcpip::Storage inMsg;
    send_commandGetVariable(cmd, var, id, add);
    processGET(inMsg, cmd, POSITION_3D);
    TraCIPosition p;
    p.x = inMsg.readDouble();
    p.y = inMsg.readDouble();
    p.z = inMsg.readDouble();
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
    int size = inMsg.readInt();
    std::vector<std::string> r;
    for (int i = 0; i < size; ++i) {
        r.push_back(inMsg.readString());
    }
    return r;
}


TraCIColor
TraCIAPI::getColor(int cmd, int var, const std::string& id, tcpip::Storage* add) {
    tcpip::Storage inMsg;
    send_commandGetVariable(cmd, var, id, add);
    processGET(inMsg, cmd, TYPE_COLOR);
    TraCIColor c;
    c.r = (unsigned char)inMsg.readUnsignedByte();
    c.g = (unsigned char)inMsg.readUnsignedByte();
    c.b = (unsigned char)inMsg.readUnsignedByte();
    c.a = (unsigned char)inMsg.readUnsignedByte();
    return c;
}

void
TraCIAPI::readVariables(tcpip::Storage& inMsg, const std::string& objectID, int variableCount, SubscribedValues& into) {
    while (variableCount > 0) {

        const int variableID = inMsg.readUnsignedByte();
        const int status = inMsg.readUnsignedByte();
        const int type = inMsg.readUnsignedByte();

        if (status == RTYPE_OK) {

            TraCIValue v;

            switch (type) {
                case TYPE_DOUBLE:
                    v.scalar = inMsg.readDouble();
                    break;
                case TYPE_STRING:
                    v.string = inMsg.readString();
                    break;
                case POSITION_2D:
                    v.position.x = inMsg.readDouble();
                    v.position.y = inMsg.readDouble();
                    v.position.z = 0;
                    break;
                case POSITION_3D:
                    v.position.x = inMsg.readDouble();
                    v.position.y = inMsg.readDouble();
                    v.position.z = inMsg.readDouble();
                    break;
                case TYPE_COLOR:
                    v.color.r = (unsigned char)inMsg.readUnsignedByte();
                    v.color.g = (unsigned char)inMsg.readUnsignedByte();
                    v.color.b = (unsigned char)inMsg.readUnsignedByte();
                    v.color.a = (unsigned char)inMsg.readUnsignedByte();
                    break;
                case TYPE_INTEGER:
                    v.scalar = inMsg.readInt();
                    break;
                case TYPE_STRINGLIST: {
                    int n = inMsg.readInt();
                    for (int i = 0; i < n; ++i) {
                        v.stringList.push_back(inMsg.readString());
                    }
                }
                break;

                // TODO Other data types

                default:
                    throw tcpip::SocketException("Unimplemented subscription type: " + toString(type));
            }

            into[objectID][variableID] = v;
        } else {
            throw tcpip::SocketException("Subscription response error: variableID=" + toString(variableID) + " status=" + toString(status));
        }

        variableCount--;
    }
}

void
TraCIAPI::readVariableSubscription(tcpip::Storage& inMsg) {
    const std::string objectID = inMsg.readString();
    const int variableCount = inMsg.readUnsignedByte();
    readVariables(inMsg, objectID, variableCount, mySubscribedValues);
}

void
TraCIAPI::readContextSubscription(tcpip::Storage& inMsg) {
    const std::string contextID = inMsg.readString();
    inMsg.readUnsignedByte(); // context domain
    const int variableCount = inMsg.readUnsignedByte();
    int numObjects = inMsg.readInt();

    while (numObjects > 0) {
        std::string objectID = inMsg.readString();
        readVariables(inMsg, objectID, variableCount, mySubscribedContextValues[contextID]);
        numObjects--;
    }
}

void
TraCIAPI::simulationStep(SUMOTime time) {
    send_commandSimulationStep(time);
    tcpip::Storage inMsg;
    check_resultState(inMsg, CMD_SIMSTEP);

    mySubscribedValues.clear();
    mySubscribedContextValues.clear();
    int numSubs = inMsg.readInt();
    while (numSubs > 0) {
        int cmdId = check_commandGetResult(inMsg, 0, -1, true);
        if (cmdId >= RESPONSE_SUBSCRIBE_INDUCTIONLOOP_VARIABLE && cmdId <= RESPONSE_SUBSCRIBE_PERSON_VARIABLE) {
            readVariableSubscription(inMsg);
        } else {
            readContextSubscription(inMsg);
        }
        numSubs--;
    }
}


void
TraCIAPI::load(const std::vector<std::string>& args) {
    int numChars = 0;
    for (int i = 0; i < (int)args.size(); ++i) {
        numChars += (int)args[i].size();
    }
    tcpip::Storage content;
    content.writeUnsignedByte(0);
    content.writeInt(1 + 4 + 1 + 1 + 4 + numChars + 4 * (int)args.size());
    content.writeUnsignedByte(CMD_LOAD);
    content.writeUnsignedByte(TYPE_STRINGLIST);
    content.writeStringList(args);
    mySocket->sendExact(content);
    tcpip::Storage inMsg;
    check_resultState(inMsg, CMD_LOAD);
}


// ---------------------------------------------------------------------------
// TraCIAPI::EdgeScope-methods
// ---------------------------------------------------------------------------
std::vector<std::string>
TraCIAPI::EdgeScope::getIDList() const {
    return myParent.getStringVector(CMD_GET_EDGE_VARIABLE, ID_LIST, "");
}

int
TraCIAPI::EdgeScope::getIDCount() const {
    return myParent.getInt(CMD_GET_EDGE_VARIABLE, ID_COUNT, "");
}

double
TraCIAPI::EdgeScope::getAdaptedTraveltime(const std::string& edgeID, double time) const {
    tcpip::Storage content;
    content.writeByte(TYPE_INTEGER);
    content.writeInt((int)time);
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, VAR_EDGE_TRAVELTIME, edgeID, &content);
}

double
TraCIAPI::EdgeScope::getEffort(const std::string& edgeID, SUMOTime time) const {
    tcpip::Storage content;
    content.writeByte(TYPE_INTEGER);
    content.writeInt((int)time);
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, VAR_EDGE_EFFORT, edgeID, &content);
}

double
TraCIAPI::EdgeScope::getCO2Emission(const std::string& edgeID) const {
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, VAR_CO2EMISSION, edgeID);
}


double
TraCIAPI::EdgeScope::getCOEmission(const std::string& edgeID) const {
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, VAR_COEMISSION, edgeID);
}

double
TraCIAPI::EdgeScope::getHCEmission(const std::string& edgeID) const {
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, VAR_HCEMISSION, edgeID);
}

double
TraCIAPI::EdgeScope::getPMxEmission(const std::string& edgeID) const {
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, VAR_PMXEMISSION, edgeID);
}

double
TraCIAPI::EdgeScope::getNOxEmission(const std::string& edgeID) const {
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, VAR_NOXEMISSION, edgeID);
}

double
TraCIAPI::EdgeScope::getFuelConsumption(const std::string& edgeID) const {
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, VAR_FUELCONSUMPTION, edgeID);
}

double
TraCIAPI::EdgeScope::getNoiseEmission(const std::string& edgeID) const {
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, VAR_NOISEEMISSION, edgeID);
}

double
TraCIAPI::EdgeScope::getElectricityConsumption(const std::string& edgeID) const {
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, VAR_ELECTRICITYCONSUMPTION, edgeID);
}

double
TraCIAPI::EdgeScope::getLastStepMeanSpeed(const std::string& edgeID) const {
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, LAST_STEP_MEAN_SPEED, edgeID);
}

double
TraCIAPI::EdgeScope::getLastStepOccupancy(const std::string& edgeID) const {
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, LAST_STEP_OCCUPANCY, edgeID);
}

double
TraCIAPI::EdgeScope::getLastStepLength(const std::string& edgeID) const {
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, LAST_STEP_LENGTH, edgeID);
}

double
TraCIAPI::EdgeScope::getTraveltime(const std::string& edgeID) const {
    return myParent.getDouble(CMD_GET_EDGE_VARIABLE, VAR_CURRENT_TRAVELTIME, edgeID);
}

int
TraCIAPI::EdgeScope::getLastStepVehicleNumber(const std::string& edgeID) const {
    return myParent.getInt(CMD_GET_EDGE_VARIABLE, LAST_STEP_VEHICLE_NUMBER, edgeID);
}

double
TraCIAPI::EdgeScope::getLastStepHaltingNumber(const std::string& edgeID) const {
    return myParent.getInt(CMD_GET_EDGE_VARIABLE, LAST_STEP_VEHICLE_HALTING_NUMBER, edgeID);
}

std::vector<std::string>
TraCIAPI::EdgeScope::getLastStepVehicleIDs(const std::string& edgeID) const {
    return myParent.getStringVector(CMD_GET_EDGE_VARIABLE, LAST_STEP_VEHICLE_ID_LIST, edgeID);
}


int
TraCIAPI::EdgeScope::getLaneNumber(const std::string& edgeID) const {
    return myParent.getInt(CMD_GET_EDGE_VARIABLE, VAR_LANE_INDEX, edgeID);
}


void
TraCIAPI::EdgeScope::adaptTraveltime(const std::string& edgeID, double time, int beginSeconds, int endSeconds) const {
    tcpip::Storage content;
    content.writeByte(TYPE_COMPOUND);
    if (endSeconds != std::numeric_limits<double>::max()) {
        content.writeInt(3);
        content.writeByte(TYPE_INTEGER);
        content.writeInt(beginSeconds);
        content.writeByte(TYPE_INTEGER);
        content.writeInt(endSeconds);
    } else {
        content.writeInt(1);
    }
    content.writeByte(TYPE_DOUBLE);
    content.writeDouble(time);
    myParent.send_commandSetValue(CMD_SET_EDGE_VARIABLE, VAR_EDGE_TRAVELTIME, edgeID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_EDGE_VARIABLE);
}

void
TraCIAPI::EdgeScope::setEffort(const std::string& edgeID, double effort, int beginSeconds, int endSeconds) const {
    tcpip::Storage content;
    content.writeByte(TYPE_COMPOUND);
    if (endSeconds != std::numeric_limits<double>::max()) {
        content.writeInt(3);
        content.writeByte(TYPE_INTEGER);
        content.writeInt(beginSeconds);
        content.writeByte(TYPE_INTEGER);
        content.writeInt(endSeconds);
    } else {
        content.writeInt(1);
    }
    content.writeByte(TYPE_DOUBLE);
    content.writeDouble(effort);
    myParent.send_commandSetValue(CMD_SET_EDGE_VARIABLE, VAR_EDGE_EFFORT, edgeID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_EDGE_VARIABLE);
}

void
TraCIAPI::EdgeScope::setMaxSpeed(const std::string& edgeID, double speed) const {
    tcpip::Storage content;
    content.writeDouble(speed);
    myParent.send_commandSetValue(CMD_SET_EDGE_VARIABLE, VAR_MAXSPEED, edgeID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_EDGE_VARIABLE);
}




// ---------------------------------------------------------------------------
// TraCIAPI::GUIScope-methods
// ---------------------------------------------------------------------------
std::vector<std::string>
TraCIAPI::GUIScope::getIDList() const {
    return myParent.getStringVector(CMD_GET_GUI_VARIABLE, ID_LIST, "");
}

double
TraCIAPI::GUIScope::getZoom(const std::string& viewID) const {
    return myParent.getDouble(CMD_GET_GUI_VARIABLE, VAR_VIEW_ZOOM, viewID);
}

TraCIPosition
TraCIAPI::GUIScope::getOffset(const std::string& viewID) const {
    return myParent.getPosition(CMD_GET_GUI_VARIABLE, VAR_VIEW_OFFSET, viewID);
}

std::string
TraCIAPI::GUIScope::getSchema(const std::string& viewID) const {
    return myParent.getString(CMD_GET_GUI_VARIABLE, VAR_VIEW_SCHEMA, viewID);
}

TraCIBoundary
TraCIAPI::GUIScope::getBoundary(const std::string& viewID) const {
    return myParent.getBoundingBox(CMD_GET_GUI_VARIABLE, VAR_VIEW_BOUNDARY, viewID);
}


void
TraCIAPI::GUIScope::setZoom(const std::string& viewID, double zoom) const {
    tcpip::Storage content;
    content.writeDouble(zoom);
    myParent.send_commandSetValue(CMD_SET_GUI_VARIABLE, VAR_VIEW_ZOOM, viewID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_GUI_VARIABLE);
}

void
TraCIAPI::GUIScope::setOffset(const std::string& viewID, double x, double y) const {
    tcpip::Storage content;
    content.writeUnsignedByte(POSITION_2D);
    content.writeDouble(x);
    content.writeDouble(y);
    myParent.send_commandSetValue(CMD_SET_GUI_VARIABLE, VAR_VIEW_OFFSET, viewID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_GUI_VARIABLE);
}

void
TraCIAPI::GUIScope::setSchema(const std::string& viewID, const std::string& schemeName) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(schemeName);
    myParent.send_commandSetValue(CMD_SET_GUI_VARIABLE, VAR_VIEW_SCHEMA, viewID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_GUI_VARIABLE);
}

void
TraCIAPI::GUIScope::setBoundary(const std::string& viewID, double xmin, double ymin, double xmax, double ymax) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_BOUNDINGBOX);
    content.writeDouble(xmin);
    content.writeDouble(ymin);
    content.writeDouble(xmax);
    content.writeDouble(ymax);
    myParent.send_commandSetValue(CMD_SET_GUI_VARIABLE, VAR_VIEW_BOUNDARY, viewID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_GUI_VARIABLE);
}

void
TraCIAPI::GUIScope::screenshot(const std::string& viewID, const std::string& filename) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(filename);
    myParent.send_commandSetValue(CMD_SET_GUI_VARIABLE, VAR_SCREENSHOT, viewID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_GUI_VARIABLE);
}

void
TraCIAPI::GUIScope::trackVehicle(const std::string& viewID, const std::string& vehID) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(vehID);
    myParent.send_commandSetValue(CMD_SET_GUI_VARIABLE, VAR_TRACK_VEHICLE, viewID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_GUI_VARIABLE);
}




// ---------------------------------------------------------------------------
// TraCIAPI::InductionLoopScope-methods
// ---------------------------------------------------------------------------
std::vector<std::string>
TraCIAPI::InductionLoopScope::getIDList() const {
    return myParent.getStringVector(CMD_GET_INDUCTIONLOOP_VARIABLE, ID_LIST, "");
}

double
TraCIAPI::InductionLoopScope::getPosition(const std::string& loopID) const {
    return myParent.getDouble(CMD_GET_INDUCTIONLOOP_VARIABLE, VAR_POSITION, loopID);
}

std::string
TraCIAPI::InductionLoopScope::getLaneID(const std::string& loopID) const {
    return myParent.getString(CMD_GET_INDUCTIONLOOP_VARIABLE, VAR_LANE_ID, loopID);
}

int
TraCIAPI::InductionLoopScope::getLastStepVehicleNumber(const std::string& loopID) const {
    return myParent.getInt(CMD_GET_INDUCTIONLOOP_VARIABLE, LAST_STEP_VEHICLE_NUMBER, loopID);
}

double
TraCIAPI::InductionLoopScope::getLastStepMeanSpeed(const std::string& loopID) const {
    return myParent.getDouble(CMD_GET_INDUCTIONLOOP_VARIABLE, LAST_STEP_MEAN_SPEED, loopID);
}

std::vector<std::string>
TraCIAPI::InductionLoopScope::getLastStepVehicleIDs(const std::string& loopID) const {
    return myParent.getStringVector(CMD_GET_INDUCTIONLOOP_VARIABLE, LAST_STEP_VEHICLE_ID_LIST, loopID);
}

double
TraCIAPI::InductionLoopScope::getLastStepOccupancy(const std::string& loopID) const {
    return myParent.getDouble(CMD_GET_INDUCTIONLOOP_VARIABLE, LAST_STEP_OCCUPANCY, loopID);
}

double
TraCIAPI::InductionLoopScope::getLastStepMeanLength(const std::string& loopID) const {
    return myParent.getDouble(CMD_GET_INDUCTIONLOOP_VARIABLE, LAST_STEP_LENGTH, loopID);
}

double
TraCIAPI::InductionLoopScope::getTimeSinceDetection(const std::string& loopID) const {
    return myParent.getDouble(CMD_GET_INDUCTIONLOOP_VARIABLE, LAST_STEP_TIME_SINCE_DETECTION, loopID);
}

std::vector<TraCIVehicleData>
TraCIAPI::InductionLoopScope::getVehicleData(const std::string& loopID) const {
    tcpip::Storage inMsg;
    myParent.send_commandGetVariable(CMD_GET_INDUCTIONLOOP_VARIABLE, LAST_STEP_VEHICLE_DATA, loopID);
    myParent.processGET(inMsg, CMD_GET_INDUCTIONLOOP_VARIABLE, TYPE_COMPOUND);
    std::vector<TraCIVehicleData> result;
    inMsg.readInt(); // components
    // number of items
    inMsg.readUnsignedByte();
    const int n = inMsg.readInt();
    for (int i = 0; i < n; ++i) {
        TraCIVehicleData vd;

        inMsg.readUnsignedByte();
        vd.id = inMsg.readString();

        inMsg.readUnsignedByte();
        vd.length = inMsg.readDouble();

        inMsg.readUnsignedByte();
        vd.entryTime = inMsg.readDouble();

        inMsg.readUnsignedByte();
        vd.leaveTime = inMsg.readDouble();

        inMsg.readUnsignedByte();
        vd.typeID = inMsg.readString();

        result.push_back(vd);
    }
    return result;
}




// ---------------------------------------------------------------------------
// TraCIAPI::JunctionScope-methods
// ---------------------------------------------------------------------------
std::vector<std::string>
TraCIAPI::JunctionScope::getIDList() const {
    return myParent.getStringVector(CMD_GET_JUNCTION_VARIABLE, ID_LIST, "");
}

TraCIPosition
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

int
TraCIAPI::LaneScope::getIDCount() const {
    return myParent.getInt(CMD_GET_LANE_VARIABLE, ID_COUNT, "");
}

double
TraCIAPI::LaneScope::getLength(const std::string& laneID) const {
    return myParent.getDouble(CMD_GET_LANE_VARIABLE, VAR_LENGTH, laneID);
}

double
TraCIAPI::LaneScope::getMaxSpeed(const std::string& laneID) const {
    return myParent.getDouble(CMD_GET_LANE_VARIABLE, VAR_MAXSPEED, laneID);
}

double
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

int
TraCIAPI::LaneScope::getLinkNumber(const std::string& laneID) const {
    return myParent.getUnsignedByte(CMD_GET_LANE_VARIABLE, LANE_LINK_NUMBER, laneID);
}

std::vector<TraCIConnection>
TraCIAPI::LaneScope::getLinks(const std::string& laneID) const {
    tcpip::Storage inMsg;
    myParent.send_commandGetVariable(CMD_GET_LANE_VARIABLE, LANE_LINKS, laneID);
    myParent.processGET(inMsg, CMD_GET_LANE_VARIABLE, TYPE_COMPOUND);
    std::vector<TraCIConnection> ret;

    inMsg.readUnsignedByte();
    inMsg.readInt();

    int linkNo = inMsg.readInt();
    for (int i = 0; i < linkNo; ++i) {

        inMsg.readUnsignedByte();
        std::string approachedLane = inMsg.readString();

        inMsg.readUnsignedByte();
        std::string approachedLaneInternal = inMsg.readString();

        inMsg.readUnsignedByte();
        bool hasPrio = inMsg.readUnsignedByte() != 0;

        inMsg.readUnsignedByte();
        bool isOpen = inMsg.readUnsignedByte() != 0;

        inMsg.readUnsignedByte();
        bool hasFoe = inMsg.readUnsignedByte() != 0;

        inMsg.readUnsignedByte();
        std::string state = inMsg.readString();

        inMsg.readUnsignedByte();
        std::string direction = inMsg.readString();

        inMsg.readUnsignedByte();
        double length = inMsg.readDouble();

        ret.push_back(TraCIConnection(approachedLane,
                                      hasPrio,
                                      isOpen,
                                      hasFoe,
                                      approachedLaneInternal,
                                      state,
                                      direction,
                                      length));

    }
    return ret;
}

TraCIPositionVector
TraCIAPI::LaneScope::getShape(const std::string& laneID) const {
    return myParent.getPolygon(CMD_GET_LANE_VARIABLE, VAR_SHAPE, laneID);
}

std::string
TraCIAPI::LaneScope::getEdgeID(const std::string& laneID) const {
    return myParent.getString(CMD_GET_LANE_VARIABLE, LANE_EDGE_ID, laneID);
}

double
TraCIAPI::LaneScope::getCO2Emission(const std::string& laneID) const {
    return myParent.getDouble(CMD_GET_LANE_VARIABLE, VAR_CO2EMISSION, laneID);
}

double
TraCIAPI::LaneScope::getCOEmission(const std::string& laneID) const {
    return myParent.getDouble(CMD_GET_LANE_VARIABLE, VAR_COEMISSION, laneID);
}

double
TraCIAPI::LaneScope::getHCEmission(const std::string& laneID) const {
    return myParent.getDouble(CMD_GET_LANE_VARIABLE, VAR_HCEMISSION, laneID);
}

double
TraCIAPI::LaneScope::getPMxEmission(const std::string& laneID) const {
    return myParent.getDouble(CMD_GET_LANE_VARIABLE, VAR_PMXEMISSION, laneID);
}

double
TraCIAPI::LaneScope::getNOxEmission(const std::string& laneID) const {
    return myParent.getDouble(CMD_GET_LANE_VARIABLE, VAR_NOXEMISSION, laneID);
}

double
TraCIAPI::LaneScope::getFuelConsumption(const std::string& laneID) const {
    return myParent.getDouble(CMD_GET_LANE_VARIABLE, VAR_FUELCONSUMPTION, laneID);
}

double
TraCIAPI::LaneScope::getNoiseEmission(const std::string& laneID) const {
    return myParent.getDouble(CMD_GET_LANE_VARIABLE, VAR_NOISEEMISSION, laneID);
}

double
TraCIAPI::LaneScope::getElectricityConsumption(const std::string& laneID) const {
    return myParent.getDouble(CMD_GET_LANE_VARIABLE, VAR_ELECTRICITYCONSUMPTION, laneID);
}

double
TraCIAPI::LaneScope::getLastStepMeanSpeed(const std::string& laneID) const {
    return myParent.getDouble(CMD_GET_LANE_VARIABLE, LAST_STEP_MEAN_SPEED, laneID);
}

double
TraCIAPI::LaneScope::getLastStepOccupancy(const std::string& laneID) const {
    return myParent.getDouble(CMD_GET_LANE_VARIABLE, LAST_STEP_OCCUPANCY, laneID);
}

double
TraCIAPI::LaneScope::getLastStepLength(const std::string& laneID) const {
    return myParent.getDouble(CMD_GET_LANE_VARIABLE, LAST_STEP_LENGTH, laneID);
}

double
TraCIAPI::LaneScope::getTraveltime(const std::string& laneID) const {
    return myParent.getDouble(CMD_GET_LANE_VARIABLE, VAR_CURRENT_TRAVELTIME, laneID);
}

int
TraCIAPI::LaneScope::getLastStepVehicleNumber(const std::string& laneID) const {
    return myParent.getInt(CMD_GET_LANE_VARIABLE, LAST_STEP_VEHICLE_NUMBER, laneID);
}

int
TraCIAPI::LaneScope::getLastStepHaltingNumber(const std::string& laneID) const {
    return myParent.getInt(CMD_GET_LANE_VARIABLE, LAST_STEP_VEHICLE_HALTING_NUMBER, laneID);
}

std::vector<std::string>
TraCIAPI::LaneScope::getLastStepVehicleIDs(const std::string& laneID) const {
    return myParent.getStringVector(CMD_GET_LANE_VARIABLE, LAST_STEP_VEHICLE_ID_LIST, laneID);
}


std::vector<std::string>
TraCIAPI::LaneScope::getFoes(const std::string& laneID, const std::string& toLaneID) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(toLaneID);
    myParent.send_commandGetVariable(CMD_GET_LANE_VARIABLE, VAR_FOES, laneID, &content);
    tcpip::Storage inMsg;
    myParent.processGET(inMsg, CMD_GET_LANE_VARIABLE, TYPE_STRINGLIST);
    int size = inMsg.readInt();
    std::vector<std::string> r;
    for (int i = 0; i < size; ++i) {
        r.push_back(inMsg.readString());
    }
    return r;
}

std::vector<std::string>
TraCIAPI::LaneScope::getInternalFoes(const std::string& laneID) const {
    return getFoes(laneID, "");
}


void
TraCIAPI::LaneScope::setAllowed(const std::string& laneID, const std::vector<std::string>& allowedClasses) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_STRINGLIST);
    content.writeInt((int)allowedClasses.size());
    for (int i = 0; i < (int)allowedClasses.size(); ++i) {
        content.writeString(allowedClasses[i]);
    }
    myParent.send_commandSetValue(CMD_SET_LANE_VARIABLE, LANE_ALLOWED, laneID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_LANE_VARIABLE);
}

void
TraCIAPI::LaneScope::setDisallowed(const std::string& laneID, const std::vector<std::string>& disallowedClasses) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_STRINGLIST);
    content.writeInt((int)disallowedClasses.size());
    for (int i = 0; i < (int)disallowedClasses.size(); ++i) {
        content.writeString(disallowedClasses[i]);
    }
    myParent.send_commandSetValue(CMD_SET_LANE_VARIABLE, LANE_DISALLOWED, laneID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_LANE_VARIABLE);
}

void
TraCIAPI::LaneScope::setMaxSpeed(const std::string& laneID, double speed) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(speed);
    myParent.send_commandSetValue(CMD_SET_LANE_VARIABLE, VAR_MAXSPEED, laneID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_LANE_VARIABLE);
}

void
TraCIAPI::LaneScope::setLength(const std::string& laneID, double length) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(length);
    myParent.send_commandSetValue(CMD_SET_LANE_VARIABLE, VAR_LENGTH, laneID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_LANE_VARIABLE);
}


// ---------------------------------------------------------------------------
// TraCIAPI::LaneAreaDetector-methods
// ---------------------------------------------------------------------------
std::vector<std::string>
TraCIAPI::LaneAreaScope::getIDList() const {
    return myParent.getStringVector(CMD_GET_LANEAREA_VARIABLE, ID_LIST, "");
}




// ---------------------------------------------------------------------------
// TraCIAPI::MeMeScope-methods
// ---------------------------------------------------------------------------
std::vector<std::string>
TraCIAPI::MeMeScope::getIDList() const {
    return myParent.getStringVector(CMD_GET_MULTIENTRYEXIT_VARIABLE, ID_LIST, "");
}

int
TraCIAPI::MeMeScope::getLastStepVehicleNumber(const std::string& detID) const {
    return myParent.getInt(CMD_GET_MULTIENTRYEXIT_VARIABLE, LAST_STEP_VEHICLE_NUMBER, detID);
}

double
TraCIAPI::MeMeScope::getLastStepMeanSpeed(const std::string& detID) const {
    return myParent.getInt(CMD_GET_MULTIENTRYEXIT_VARIABLE, LAST_STEP_MEAN_SPEED, detID);
}

std::vector<std::string>
TraCIAPI::MeMeScope::getLastStepVehicleIDs(const std::string& detID) const {
    return myParent.getStringVector(CMD_GET_MULTIENTRYEXIT_VARIABLE, LAST_STEP_VEHICLE_ID_LIST, detID);
}

int
TraCIAPI::MeMeScope::getLastStepHaltingNumber(const std::string& detID) const {
    return myParent.getInt(CMD_GET_MULTIENTRYEXIT_VARIABLE, LAST_STEP_VEHICLE_HALTING_NUMBER, detID);
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

TraCIPosition
TraCIAPI::POIScope::getPosition(const std::string& poiID) const {
    return myParent.getPosition(CMD_GET_POI_VARIABLE, VAR_POSITION, poiID);
}

TraCIColor
TraCIAPI::POIScope::getColor(const std::string& poiID) const {
    return myParent.getColor(CMD_GET_POI_VARIABLE, VAR_COLOR, poiID);
}


void
TraCIAPI::POIScope::setType(const std::string& poiID, const std::string& setType) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(setType);
    myParent.send_commandSetValue(CMD_SET_POI_VARIABLE, VAR_TYPE, poiID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_POI_VARIABLE);
}

void
TraCIAPI::POIScope::setPosition(const std::string& poiID, double x, double y) const {
    tcpip::Storage content;
    content.writeUnsignedByte(POSITION_2D);
    content.writeDouble(x);
    content.writeDouble(y);
    myParent.send_commandSetValue(CMD_SET_POI_VARIABLE, VAR_POSITION, poiID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_POI_VARIABLE);
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
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_POI_VARIABLE);
}

void
TraCIAPI::POIScope::add(const std::string& poiID, double x, double y, const TraCIColor& c, const std::string& type, int layer) const {
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
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_POI_VARIABLE);
}

void
TraCIAPI::POIScope::remove(const std::string& poiID, int layer) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_INTEGER);
    content.writeInt(layer);
    myParent.send_commandSetValue(CMD_SET_POI_VARIABLE, REMOVE, poiID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_POI_VARIABLE);
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

TraCIPositionVector
TraCIAPI::PolygonScope::getShape(const std::string& polygonID) const {
    return myParent.getPolygon(CMD_GET_POLYGON_VARIABLE, VAR_SHAPE, polygonID);
}

TraCIColor
TraCIAPI::PolygonScope::getColor(const std::string& polygonID) const {
    return myParent.getColor(CMD_GET_POLYGON_VARIABLE, VAR_COLOR, polygonID);
}


void
TraCIAPI::PolygonScope::setType(const std::string& polygonID, const std::string& setType) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(setType);
    myParent.send_commandSetValue(CMD_SET_POLYGON_VARIABLE, VAR_TYPE, polygonID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_POLYGON_VARIABLE);
}

void
TraCIAPI::PolygonScope::setShape(const std::string& polygonID, const TraCIPositionVector& shape) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_POLYGON);
    content.writeInt((int)shape.size());
    for (int i = 0; i < (int)shape.size(); ++i) {
        content.writeDouble(shape[i].x);
        content.writeDouble(shape[i].y);
    }
    myParent.send_commandSetValue(CMD_SET_POLYGON_VARIABLE, VAR_POSITION, polygonID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_POLYGON_VARIABLE);
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
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_POLYGON_VARIABLE);
}

void
TraCIAPI::PolygonScope::add(const std::string& polygonID, const TraCIPositionVector& shape, const TraCIColor& c, bool fill, const std::string& type, int layer) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_COMPOUND);
    content.writeInt(5);
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
    content.writeUnsignedByte((int)shape.size());
    for (int i = 0; i < (int)shape.size(); ++i) {
        content.writeDouble(shape[i].x);
        content.writeDouble(shape[i].y);
    }
    myParent.send_commandSetValue(CMD_SET_POLYGON_VARIABLE, ADD, polygonID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_POLYGON_VARIABLE);
}

void
TraCIAPI::PolygonScope::remove(const std::string& polygonID, int layer) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_INTEGER);
    content.writeInt(layer);
    myParent.send_commandSetValue(CMD_SET_POLYGON_VARIABLE, REMOVE, polygonID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_POLYGON_VARIABLE);
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
    myParent.send_commandSetValue(CMD_SET_ROUTE_VARIABLE, ADD, routeID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_ROUTE_VARIABLE);
}





// ---------------------------------------------------------------------------
// TraCIAPI::SimulationScope-methods
// ---------------------------------------------------------------------------
SUMOTime
TraCIAPI::SimulationScope::getCurrentTime() const {
    return myParent.getSUMOTime(CMD_GET_SIM_VARIABLE, VAR_TIME_STEP, "");
}

int
TraCIAPI::SimulationScope::getLoadedNumber() const {
    return (int) myParent.getInt(CMD_GET_SIM_VARIABLE, VAR_LOADED_VEHICLES_NUMBER, "");
}

std::vector<std::string>
TraCIAPI::SimulationScope::getLoadedIDList() const {
    return myParent.getStringVector(CMD_GET_SIM_VARIABLE, VAR_LOADED_VEHICLES_IDS, "");
}

int
TraCIAPI::SimulationScope::getDepartedNumber() const {
    return (int) myParent.getInt(CMD_GET_SIM_VARIABLE, VAR_DEPARTED_VEHICLES_NUMBER, "");
}

std::vector<std::string>
TraCIAPI::SimulationScope::getDepartedIDList() const {
    return myParent.getStringVector(CMD_GET_SIM_VARIABLE, VAR_DEPARTED_VEHICLES_IDS, "");
}

int
TraCIAPI::SimulationScope::getArrivedNumber() const {
    return (int) myParent.getInt(CMD_GET_SIM_VARIABLE, VAR_ARRIVED_VEHICLES_NUMBER, "");
}

std::vector<std::string>
TraCIAPI::SimulationScope::getArrivedIDList() const {
    return myParent.getStringVector(CMD_GET_SIM_VARIABLE, VAR_ARRIVED_VEHICLES_IDS, "");
}

int
TraCIAPI::SimulationScope::getStartingTeleportNumber() const {
    return (int) myParent.getInt(CMD_GET_SIM_VARIABLE, VAR_TELEPORT_STARTING_VEHICLES_NUMBER, "");
}

std::vector<std::string>
TraCIAPI::SimulationScope::getStartingTeleportIDList() const {
    return myParent.getStringVector(CMD_GET_SIM_VARIABLE, VAR_TELEPORT_STARTING_VEHICLES_IDS, "");
}

int
TraCIAPI::SimulationScope::getEndingTeleportNumber() const {
    return (int) myParent.getInt(CMD_GET_SIM_VARIABLE, VAR_TELEPORT_ENDING_VEHICLES_NUMBER, "");
}

std::vector<std::string>
TraCIAPI::SimulationScope::getEndingTeleportIDList() const {
    return myParent.getStringVector(CMD_GET_SIM_VARIABLE, VAR_TELEPORT_ENDING_VEHICLES_IDS, "");
}

SUMOTime
TraCIAPI::SimulationScope::getDeltaT() const {
    return myParent.getSUMOTime(CMD_GET_SIM_VARIABLE, VAR_DELTA_T, "");
}

TraCIBoundary
TraCIAPI::SimulationScope::getNetBoundary() const {
    return myParent.getBoundingBox(CMD_GET_SIM_VARIABLE, VAR_NET_BOUNDING_BOX, "");
}

int
TraCIAPI::SimulationScope::getMinExpectedNumber() const {
    return myParent.getInt(CMD_GET_SIM_VARIABLE, VAR_MIN_EXPECTED_VEHICLES, "");
}

void
TraCIAPI::SimulationScope::subscribe(int domID, const std::string& objID, SUMOTime beginTime, SUMOTime endTime, const std::vector<int>& vars) const {
    myParent.send_commandSubscribeObjectVariable(domID, objID, beginTime, endTime, vars);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, domID);
    if (vars.size() > 0) {
        myParent.check_commandGetResult(inMsg, domID);
        myParent.readVariableSubscription(inMsg);
    }
}

void
TraCIAPI::SimulationScope::subscribeContext(int domID, const std::string& objID, SUMOTime beginTime, SUMOTime endTime, int domain, double range, const std::vector<int>& vars) const {

    myParent.send_commandSubscribeObjectContext(domID, objID, beginTime, endTime, domain, range, vars);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, domID);
    myParent.check_commandGetResult(inMsg, domID);
    myParent.readContextSubscription(inMsg);
}

const TraCIAPI::SubscribedValues
TraCIAPI::SimulationScope::getSubscriptionResults() const {
    return myParent.mySubscribedValues;
}


const TraCIAPI::TraCIValues
TraCIAPI::SimulationScope::getSubscriptionResults(const std::string& objID) const {
    if (myParent.mySubscribedValues.find(objID) != myParent.mySubscribedValues.end()) {
        return myParent.mySubscribedValues[objID];
    } else {
        return TraCIValues();
    }
}


const TraCIAPI::SubscribedContextValues
TraCIAPI::SimulationScope::getContextSubscriptionResults() const {
    return myParent.mySubscribedContextValues;
}


const TraCIAPI::SubscribedValues
TraCIAPI::SimulationScope::getContextSubscriptionResults(const std::string& objID) const {
    if (myParent.mySubscribedContextValues.find(objID) != myParent.mySubscribedContextValues.end()) {
        return myParent.mySubscribedContextValues[objID];
    } else {
        return SubscribedValues();
    }
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

std::vector<TraCILogic>
TraCIAPI::TrafficLightScope::getCompleteRedYellowGreenDefinition(const std::string& tlsID) const {
    tcpip::Storage inMsg;
    myParent.send_commandGetVariable(CMD_GET_TL_VARIABLE, TL_COMPLETE_DEFINITION_RYG, tlsID);
    myParent.processGET(inMsg, CMD_GET_TL_VARIABLE, TYPE_COMPOUND);
    std::vector<TraCILogic> ret;

    inMsg.readUnsignedByte();
    inMsg.readInt();

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
        TraCILogic logic(subID, type, phaseIndex);
        for (int j = 0; j < phaseNumber; ++j) {
            inMsg.readUnsignedByte();
            int duration = inMsg.readInt();
            inMsg.readUnsignedByte();
            int duration1 = inMsg.readInt();
            inMsg.readUnsignedByte();
            int duration2 = inMsg.readInt();
            inMsg.readUnsignedByte();
            std::string phase = inMsg.readString();
            logic.phases.emplace_back(TraCIPhase(duration, duration1, duration2, phase));
        }
        ret.emplace_back(logic);
    }
    return ret;
}

std::vector<std::string>
TraCIAPI::TrafficLightScope::getControlledLanes(const std::string& tlsID) const {
    return myParent.getStringVector(CMD_GET_TL_VARIABLE, TL_CONTROLLED_LANES, tlsID);
}

std::vector<std::vector<TraCILink> >
TraCIAPI::TrafficLightScope::getControlledLinks(const std::string& tlsID) const {
    tcpip::Storage inMsg;
    myParent.send_commandGetVariable(CMD_GET_TL_VARIABLE, TL_CONTROLLED_LINKS, tlsID);
    myParent.processGET(inMsg, CMD_GET_TL_VARIABLE, TYPE_COMPOUND);
    std::vector<std::vector<TraCILink> > result;

    inMsg.readUnsignedByte();
    inMsg.readInt();

    int linkNo = inMsg.readInt();
    for (int i = 0; i < linkNo; ++i) {
        inMsg.readUnsignedByte();
        int no = inMsg.readInt();
        std::vector<TraCILink> ret;
        for (int i1 = 0; i1 < no; ++i1) {
            inMsg.readUnsignedByte();
            inMsg.readInt();
            std::string from = inMsg.readString();
            std::string to = inMsg.readString();
            std::string via = inMsg.readString();
            ret.emplace_back(TraCILink(from, via, to));
        }
        result.emplace_back(ret);
    }
    return result;
}

std::string
TraCIAPI::TrafficLightScope::getProgram(const std::string& tlsID) const {
    return myParent.getString(CMD_GET_TL_VARIABLE, TL_CURRENT_PROGRAM, tlsID);
}

int
TraCIAPI::TrafficLightScope::getPhase(const std::string& tlsID) const {
    return myParent.getInt(CMD_GET_TL_VARIABLE, TL_CURRENT_PHASE, tlsID);
}

int
TraCIAPI::TrafficLightScope::getNextSwitch(const std::string& tlsID) const {
    return myParent.getInt(CMD_GET_TL_VARIABLE, TL_NEXT_SWITCH, tlsID);
}


void
TraCIAPI::TrafficLightScope::setRedYellowGreenState(const std::string& tlsID, const std::string& state) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(state);
    myParent.send_commandSetValue(CMD_SET_TL_VARIABLE, TL_RED_YELLOW_GREEN_STATE, tlsID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_TL_VARIABLE);
}

void
TraCIAPI::TrafficLightScope::setPhase(const std::string& tlsID, int index) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_INTEGER);
    content.writeInt(index);
    myParent.send_commandSetValue(CMD_SET_TL_VARIABLE, TL_PHASE_INDEX, tlsID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_TL_VARIABLE);
}

void
TraCIAPI::TrafficLightScope::setProgram(const std::string& tlsID, const std::string& programID) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(programID);
    myParent.send_commandSetValue(CMD_SET_TL_VARIABLE, TL_PROGRAM, tlsID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_TL_VARIABLE);
}

void
TraCIAPI::TrafficLightScope::setPhaseDuration(const std::string& tlsID, int phaseDuration) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_INTEGER);
    content.writeInt(int(1000 * phaseDuration));
    myParent.send_commandSetValue(CMD_SET_TL_VARIABLE, TL_PHASE_DURATION, tlsID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_TL_VARIABLE);
}

void
TraCIAPI::TrafficLightScope::setCompleteRedYellowGreenDefinition(const std::string& tlsID, const TraCILogic& logic) const {
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
        content.writeInt((int)logic.phases[i].duration);
        content.writeUnsignedByte(TYPE_INTEGER);
        content.writeInt((int)logic.phases[i].duration1);
        content.writeUnsignedByte(TYPE_INTEGER);
        content.writeInt((int)logic.phases[i].duration2);
        content.writeUnsignedByte(TYPE_STRING);
        content.writeString(logic.phases[i].phase);
    }
    myParent.send_commandSetValue(CMD_SET_TL_VARIABLE, TL_COMPLETE_PROGRAM_RYG, tlsID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_TL_VARIABLE);
}





// ---------------------------------------------------------------------------
// TraCIAPI::VehicleTypeScope-methods
// ---------------------------------------------------------------------------
std::vector<std::string>
TraCIAPI::VehicleTypeScope::getIDList() const {
    return myParent.getStringVector(CMD_GET_VEHICLETYPE_VARIABLE, ID_LIST, "");
}

double
TraCIAPI::VehicleTypeScope::getLength(const std::string& typeID) const {
    return myParent.getDouble(CMD_GET_VEHICLETYPE_VARIABLE, VAR_LENGTH, typeID);
}

double
TraCIAPI::VehicleTypeScope::getMaxSpeed(const std::string& typeID) const {
    return myParent.getDouble(CMD_GET_VEHICLETYPE_VARIABLE, VAR_MAXSPEED, typeID);
}

double
TraCIAPI::VehicleTypeScope::getSpeedFactor(const std::string& typeID) const {
    return myParent.getDouble(CMD_GET_VEHICLETYPE_VARIABLE, VAR_SPEED_FACTOR, typeID);
}

double
TraCIAPI::VehicleTypeScope::getSpeedDeviation(const std::string& typeID) const {
    return myParent.getDouble(CMD_GET_VEHICLETYPE_VARIABLE, VAR_SPEED_DEVIATION, typeID);
}

double
TraCIAPI::VehicleTypeScope::getAccel(const std::string& typeID) const {
    return myParent.getDouble(CMD_GET_VEHICLETYPE_VARIABLE, VAR_ACCEL, typeID);
}

double
TraCIAPI::VehicleTypeScope::getDecel(const std::string& typeID) const {
    return myParent.getDouble(CMD_GET_VEHICLETYPE_VARIABLE, VAR_DECEL, typeID);
}

double
TraCIAPI::VehicleTypeScope::getEmergencyDecel(const std::string& typeID) const {
    return myParent.getDouble(CMD_GET_VEHICLETYPE_VARIABLE, VAR_EMERGENCY_DECEL, typeID);
}

double
TraCIAPI::VehicleTypeScope::getApparentDecel(const std::string& typeID) const {
    return myParent.getDouble(CMD_GET_VEHICLETYPE_VARIABLE, VAR_APPARENT_DECEL, typeID);
}

double
TraCIAPI::VehicleTypeScope::getImperfection(const std::string& typeID) const {
    return myParent.getDouble(CMD_GET_VEHICLETYPE_VARIABLE, VAR_IMPERFECTION, typeID);
}

double
TraCIAPI::VehicleTypeScope::getTau(const std::string& typeID) const {
    return myParent.getDouble(CMD_GET_VEHICLETYPE_VARIABLE, VAR_TAU, typeID);
}

std::string
TraCIAPI::VehicleTypeScope::getVehicleClass(const std::string& typeID) const {
    return myParent.getString(CMD_GET_VEHICLETYPE_VARIABLE, VAR_VEHICLECLASS, typeID);
}

std::string
TraCIAPI::VehicleTypeScope::getEmissionClass(const std::string& typeID) const {
    return myParent.getString(CMD_GET_VEHICLETYPE_VARIABLE, VAR_EMISSIONCLASS, typeID);
}

std::string
TraCIAPI::VehicleTypeScope::getShapeClass(const std::string& typeID) const {
    return myParent.getString(CMD_GET_VEHICLETYPE_VARIABLE, VAR_SHAPECLASS, typeID);
}

double
TraCIAPI::VehicleTypeScope::getMinGap(const std::string& typeID) const {
    return myParent.getDouble(CMD_GET_VEHICLETYPE_VARIABLE, VAR_MINGAP, typeID);
}

double
TraCIAPI::VehicleTypeScope::getMinGapLat(const std::string& typeID) const {
    return myParent.getDouble(CMD_GET_VEHICLETYPE_VARIABLE, VAR_MINGAP_LAT, typeID);
}

double
TraCIAPI::VehicleTypeScope::getMaxSpeedLat(const std::string& typeID) const {
    return myParent.getDouble(CMD_GET_VEHICLETYPE_VARIABLE, VAR_MAXSPEED_LAT, typeID);
}

std::string
TraCIAPI::VehicleTypeScope::getLateralAlignment(const std::string& typeID) const {
    return myParent.getString(CMD_GET_VEHICLETYPE_VARIABLE, VAR_LATALIGNMENT, typeID);
}

double
TraCIAPI::VehicleTypeScope::getWidth(const std::string& typeID) const {
    return myParent.getDouble(CMD_GET_VEHICLETYPE_VARIABLE, VAR_WIDTH, typeID);
}

double
TraCIAPI::VehicleTypeScope::getHeight(const std::string& typeID) const {
    return myParent.getDouble(CMD_GET_VEHICLETYPE_VARIABLE, VAR_HEIGHT, typeID);
}

TraCIColor
TraCIAPI::VehicleTypeScope::getColor(const std::string& typeID) const {
    return myParent.getColor(CMD_GET_VEHICLETYPE_VARIABLE, VAR_COLOR, typeID);
}



void
TraCIAPI::VehicleTypeScope::setLength(const std::string& typeID, double length) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(length);
    myParent.send_commandSetValue(CMD_SET_VEHICLETYPE_VARIABLE, VAR_LENGTH, typeID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLETYPE_VARIABLE);
}

void
TraCIAPI::VehicleTypeScope::setMaxSpeed(const std::string& typeID, double speed) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(speed);
    myParent.send_commandSetValue(CMD_SET_VEHICLETYPE_VARIABLE, VAR_MAXSPEED, typeID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLETYPE_VARIABLE);
}

void
TraCIAPI::VehicleTypeScope::setVehicleClass(const std::string& typeID, const std::string& clazz) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(clazz);
    myParent.send_commandSetValue(CMD_SET_VEHICLETYPE_VARIABLE, VAR_VEHICLECLASS, typeID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLETYPE_VARIABLE);
}

void
TraCIAPI::VehicleTypeScope::setSpeedFactor(const std::string& typeID, double factor) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(factor);
    myParent.send_commandSetValue(CMD_SET_VEHICLETYPE_VARIABLE, VAR_SPEED_FACTOR, typeID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLETYPE_VARIABLE);
}

void
TraCIAPI::VehicleTypeScope::setSpeedDeviation(const std::string& typeID, double deviation) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(deviation);
    myParent.send_commandSetValue(CMD_SET_VEHICLETYPE_VARIABLE, VAR_SPEED_DEVIATION, typeID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLETYPE_VARIABLE);
}


void
TraCIAPI::VehicleTypeScope::setEmissionClass(const std::string& typeID, const std::string& clazz) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(clazz);
    myParent.send_commandSetValue(CMD_SET_VEHICLETYPE_VARIABLE, VAR_EMISSIONCLASS, typeID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLETYPE_VARIABLE);
}

void
TraCIAPI::VehicleTypeScope::setWidth(const std::string& typeID, double width) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(width);
    myParent.send_commandSetValue(CMD_SET_VEHICLETYPE_VARIABLE, VAR_WIDTH, typeID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLETYPE_VARIABLE);
}

void
TraCIAPI::VehicleTypeScope::setHeight(const std::string& typeID, double height) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(height);
    myParent.send_commandSetValue(CMD_SET_VEHICLETYPE_VARIABLE, VAR_HEIGHT, typeID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLETYPE_VARIABLE);
}

void
TraCIAPI::VehicleTypeScope::setMinGap(const std::string& typeID, double minGap) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(minGap);
    myParent.send_commandSetValue(CMD_SET_VEHICLETYPE_VARIABLE, VAR_MINGAP, typeID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLETYPE_VARIABLE);
}


void
TraCIAPI::VehicleTypeScope::setMinGapLat(const std::string& typeID, double minGapLat) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(minGapLat);
    myParent.send_commandSetValue(CMD_SET_VEHICLETYPE_VARIABLE, VAR_MINGAP_LAT, typeID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLETYPE_VARIABLE);
}

void
TraCIAPI::VehicleTypeScope::setMaxSpeedLat(const std::string& typeID, double speed) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(speed);
    myParent.send_commandSetValue(CMD_SET_VEHICLETYPE_VARIABLE, VAR_MAXSPEED_LAT, typeID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLETYPE_VARIABLE);
}

void
TraCIAPI::VehicleTypeScope::setLateralAlignment(const std::string& typeID, const std::string& latAlignment) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(latAlignment);
    myParent.send_commandSetValue(CMD_SET_VEHICLETYPE_VARIABLE, VAR_LATALIGNMENT, typeID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLETYPE_VARIABLE);
}

void
TraCIAPI::VehicleTypeScope::copy(const std::string& origTypeID, const std::string& newTypeID) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(newTypeID);
    myParent.send_commandSetValue(CMD_SET_VEHICLETYPE_VARIABLE, COPY, origTypeID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLETYPE_VARIABLE);
}

void
TraCIAPI::VehicleTypeScope::setShapeClass(const std::string& typeID, const std::string& clazz) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(clazz);
    myParent.send_commandSetValue(CMD_SET_VEHICLETYPE_VARIABLE, VAR_SHAPECLASS, typeID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLETYPE_VARIABLE);
}

void
TraCIAPI::VehicleTypeScope::setAccel(const std::string& typeID, double accel) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(accel);
    myParent.send_commandSetValue(CMD_SET_VEHICLETYPE_VARIABLE, VAR_ACCEL, typeID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLETYPE_VARIABLE);
}

void
TraCIAPI::VehicleTypeScope::setDecel(const std::string& typeID, double decel) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(decel);
    myParent.send_commandSetValue(CMD_SET_VEHICLETYPE_VARIABLE, VAR_DECEL, typeID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLETYPE_VARIABLE);
}

void
TraCIAPI::VehicleTypeScope::setEmergencyDecel(const std::string& typeID, double decel) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(decel);
    myParent.send_commandSetValue(CMD_SET_VEHICLETYPE_VARIABLE, VAR_EMERGENCY_DECEL, typeID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLETYPE_VARIABLE);
}

void
TraCIAPI::VehicleTypeScope::setApparentDecel(const std::string& typeID, double decel) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(decel);
    myParent.send_commandSetValue(CMD_SET_VEHICLETYPE_VARIABLE, VAR_APPARENT_DECEL, typeID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLETYPE_VARIABLE);
}

void
TraCIAPI::VehicleTypeScope::setImperfection(const std::string& typeID, double imperfection) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(imperfection);
    myParent.send_commandSetValue(CMD_SET_VEHICLETYPE_VARIABLE, VAR_IMPERFECTION, typeID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLETYPE_VARIABLE);
}

void
TraCIAPI::VehicleTypeScope::setTau(const std::string& typeID, double tau) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(tau);
    myParent.send_commandSetValue(CMD_SET_VEHICLETYPE_VARIABLE, VAR_TAU, typeID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLETYPE_VARIABLE);
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
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLETYPE_VARIABLE);
}





// ---------------------------------------------------------------------------
// TraCIAPI::VehicleScope-methods
// ---------------------------------------------------------------------------
std::vector<std::string>
TraCIAPI::VehicleScope::getIDList() const {
    return myParent.getStringVector(CMD_GET_VEHICLE_VARIABLE, ID_LIST, "");
}

int
TraCIAPI::VehicleScope::getIDCount() const {
    return myParent.getInt(CMD_GET_VEHICLE_VARIABLE, ID_COUNT, "");
}

double
TraCIAPI::VehicleScope::getSpeed(const std::string& vehicleID) const {
    return myParent.getDouble(CMD_GET_VEHICLE_VARIABLE, VAR_SPEED, vehicleID);
}

double
TraCIAPI::VehicleScope::getMaxSpeed(const std::string& vehicleID) const {
    return myParent.getDouble(CMD_GET_VEHICLE_VARIABLE, VAR_MAXSPEED, vehicleID);
}

TraCIPosition
TraCIAPI::VehicleScope::getPosition(const std::string& vehicleID) const {
    return myParent.getPosition(CMD_GET_VEHICLE_VARIABLE, VAR_POSITION, vehicleID);
}

TraCIPosition
TraCIAPI::VehicleScope::getPosition3D(const std::string& vehicleID) const {
    return myParent.getPosition3D(CMD_GET_VEHICLE_VARIABLE, VAR_POSITION3D, vehicleID);
}

double
TraCIAPI::VehicleScope::getAngle(const std::string& vehicleID) const {
    return myParent.getDouble(CMD_GET_VEHICLE_VARIABLE, VAR_ANGLE, vehicleID);
}

std::string
TraCIAPI::VehicleScope::getRoadID(const std::string& vehicleID) const {
    return myParent.getString(CMD_GET_VEHICLE_VARIABLE, VAR_ROAD_ID, vehicleID);
}

std::string
TraCIAPI::VehicleScope::getLaneID(const std::string& vehicleID) const {
    return myParent.getString(CMD_GET_VEHICLE_VARIABLE, VAR_LANE_ID, vehicleID);
}

int
TraCIAPI::VehicleScope::getLaneIndex(const std::string& vehicleID) const {
    return myParent.getInt(CMD_GET_VEHICLE_VARIABLE, VAR_LANE_INDEX, vehicleID);
}

std::string
TraCIAPI::VehicleScope::getTypeID(const std::string& vehicleID) const {
    return myParent.getString(CMD_GET_VEHICLE_VARIABLE, VAR_TYPE, vehicleID);
}

std::string
TraCIAPI::VehicleScope::getRouteID(const std::string& vehicleID) const {
    return myParent.getString(CMD_GET_VEHICLE_VARIABLE, VAR_ROUTE_ID, vehicleID);
}

int
TraCIAPI::VehicleScope::getRouteIndex(const std::string& vehicleID) const {
    return myParent.getInt(CMD_GET_VEHICLE_VARIABLE, VAR_ROUTE_INDEX, vehicleID);
}

std::vector<std::string>
TraCIAPI::VehicleScope::getEdges(const std::string& vehicleID) const {
    return getRoute(vehicleID);
}

std::vector<std::string>
TraCIAPI::VehicleScope::getRoute(const std::string& vehicleID) const {
    return myParent.getStringVector(CMD_GET_VEHICLE_VARIABLE, VAR_EDGES, vehicleID);
}

TraCIColor
TraCIAPI::VehicleScope::getColor(const std::string& vehicleID) const {
    return myParent.getColor(CMD_GET_VEHICLE_VARIABLE, VAR_COLOR, vehicleID);
}

double
TraCIAPI::VehicleScope::getLanePosition(const std::string& vehicleID) const {
    return myParent.getDouble(CMD_GET_VEHICLE_VARIABLE, VAR_LANEPOSITION, vehicleID);
}

double
TraCIAPI::VehicleScope::getDistance(const std::string& vehicleID) const {
    return myParent.getDouble(CMD_GET_VEHICLE_VARIABLE, VAR_DISTANCE, vehicleID);
}

double
TraCIAPI::VehicleScope::getLateralLanePosition(const std::string& vehicleID) const {
    return myParent.getDouble(CMD_GET_VEHICLE_VARIABLE, VAR_LANEPOSITION_LAT, vehicleID);
}

double
TraCIAPI::VehicleScope::getCO2Emission(const std::string& vehicleID) const {
    return myParent.getDouble(CMD_GET_VEHICLE_VARIABLE, VAR_CO2EMISSION, vehicleID);
}

double
TraCIAPI::VehicleScope::getCOEmission(const std::string& vehicleID) const {
    return myParent.getDouble(CMD_GET_VEHICLE_VARIABLE, VAR_COEMISSION, vehicleID);
}

double
TraCIAPI::VehicleScope::getHCEmission(const std::string& vehicleID) const {
    return myParent.getDouble(CMD_GET_VEHICLE_VARIABLE, VAR_HCEMISSION, vehicleID);
}

double
TraCIAPI::VehicleScope::getPMxEmission(const std::string& vehicleID) const {
    return myParent.getDouble(CMD_GET_VEHICLE_VARIABLE, VAR_PMXEMISSION, vehicleID);
}

double
TraCIAPI::VehicleScope::getNOxEmission(const std::string& vehicleID) const {
    return myParent.getDouble(CMD_GET_VEHICLE_VARIABLE, VAR_NOXEMISSION, vehicleID);
}

double
TraCIAPI::VehicleScope::getFuelConsumption(const std::string& vehicleID) const {
    return myParent.getDouble(CMD_GET_VEHICLE_VARIABLE, VAR_FUELCONSUMPTION, vehicleID);
}

double
TraCIAPI::VehicleScope::getNoiseEmission(const std::string& vehicleID) const {
    return myParent.getDouble(CMD_GET_VEHICLE_VARIABLE, VAR_NOISEEMISSION, vehicleID);
}

double
TraCIAPI::VehicleScope::getElectricityConsumption(const std::string& vehicleID) const {
    return myParent.getDouble(CMD_GET_VEHICLE_VARIABLE, VAR_ELECTRICITYCONSUMPTION, vehicleID);
}

double
TraCIAPI::VehicleScope::getWaitingTime(const std::string& vehID) const {
    return myParent.getDouble(CMD_GET_VEHICLE_VARIABLE, VAR_WAITING_TIME, vehID);
}

int
TraCIAPI::VehicleScope::getSpeedMode(const std::string& vehID) const {
    return myParent.getInt(CMD_GET_VEHICLE_VARIABLE, VAR_SPEEDSETMODE, vehID);
}


double
TraCIAPI::VehicleScope::getSlope(const std::string& vehID) const {
    return myParent.getDouble(CMD_GET_VEHICLE_VARIABLE, VAR_SLOPE, vehID);
}


std::string
TraCIAPI::VehicleScope::getLine(const std::string& typeID) const {
    return myParent.getString(CMD_GET_VEHICLE_VARIABLE, VAR_LINE, typeID);
}

std::vector<std::string>
TraCIAPI::VehicleScope::getVia(const std::string& vehicleID) const {
    return myParent.getStringVector(CMD_GET_VEHICLE_VARIABLE, VAR_VIA, vehicleID);
}

std::string
TraCIAPI::VehicleScope::getEmissionClass(const std::string& vehicleID) const {
    return myParent.getString(CMD_GET_VEHICLE_VARIABLE, VAR_EMISSIONCLASS, vehicleID);
}

std::string
TraCIAPI::VehicleScope::getShapeClass(const std::string& vehicleID) const {
    return myParent.getString(CMD_GET_VEHICLE_VARIABLE, VAR_SHAPECLASS, vehicleID);
}

std::vector<TraCINextTLSData>
TraCIAPI::VehicleScope::getNextTLS(const std::string& vehID) const {
    tcpip::Storage inMsg;
    myParent.send_commandGetVariable(CMD_GET_VEHICLE_VARIABLE, VAR_NEXT_TLS, vehID);
    myParent.processGET(inMsg, CMD_GET_VEHICLE_VARIABLE, TYPE_COMPOUND);
    std::vector<TraCINextTLSData> result;
    inMsg.readInt(); // components
    // number of items
    inMsg.readUnsignedByte();
    const int n = inMsg.readInt();
    for (int i = 0; i < n; ++i) {
        TraCINextTLSData d;
        inMsg.readUnsignedByte();
        d.id = inMsg.readString();

        inMsg.readUnsignedByte();
        d.tlIndex = inMsg.readInt();

        inMsg.readUnsignedByte();
        d.dist = inMsg.readDouble();

        inMsg.readUnsignedByte();
        d.state = (char)inMsg.readByte();

        result.push_back(d);
    }
    return result;
}

std::vector<TraCIBestLanesData>
TraCIAPI::VehicleScope::getBestLanes(const std::string& vehicleID) const {
    tcpip::Storage inMsg;
    myParent.send_commandGetVariable(CMD_GET_VEHICLE_VARIABLE, VAR_BEST_LANES, vehicleID);
    myParent.processGET(inMsg, CMD_GET_VEHICLE_VARIABLE, TYPE_COMPOUND);
    inMsg.readInt();
    inMsg.readUnsignedByte();

    std::vector<TraCIBestLanesData> result;
    const int n = inMsg.readInt(); // number of following edge information
    for (int i = 0; i < n; ++i) {
        TraCIBestLanesData info;
        inMsg.readUnsignedByte();
        info.laneID = inMsg.readString();

        inMsg.readUnsignedByte();
        info.length = inMsg.readDouble();

        inMsg.readUnsignedByte();
        info.occupation = inMsg.readDouble();

        inMsg.readUnsignedByte();
        info.bestLaneOffset = inMsg.readByte();

        inMsg.readUnsignedByte();
        info.allowsContinuation = (inMsg.readUnsignedByte() == 1);

        inMsg.readUnsignedByte();
        const int m = inMsg.readInt();
        for (int i = 0; i < m; ++i) {
            info.continuationLanes.push_back(inMsg.readString());
        }

        result.push_back(info);
    }
    return result;
}


std::pair<std::string, double>
TraCIAPI::VehicleScope::getLeader(const std::string& vehicleID, double dist) const {
    tcpip::Storage content;
    content.writeByte(TYPE_DOUBLE);
    content.writeDouble(dist);
    myParent.send_commandGetVariable(CMD_GET_VEHICLE_VARIABLE, VAR_LEADER, vehicleID, &content);
    tcpip::Storage inMsg;
    myParent.processGET(inMsg, CMD_GET_VEHICLE_VARIABLE, TYPE_COMPOUND);
    inMsg.readInt(); // components
    inMsg.readUnsignedByte();
    const std::string leaderID = inMsg.readString();
    inMsg.readUnsignedByte();
    const double gap = inMsg.readDouble();
    return make_pair(leaderID, gap);
}


int
TraCIAPI::VehicleScope::getStopState(const std::string& vehicleID) const {
    return myParent.getUnsignedByte(CMD_GET_VEHICLE_VARIABLE, VAR_STOPSTATE, vehicleID);
}

double
TraCIAPI::VehicleScope::getAccel(const std::string& vehicleID) const {
    return myParent.getDouble(CMD_GET_VEHICLE_VARIABLE, VAR_ACCEL, vehicleID);
}

double
TraCIAPI::VehicleScope::getDecel(const std::string& vehicleID) const {
    return myParent.getDouble(CMD_GET_VEHICLE_VARIABLE, VAR_DECEL, vehicleID);
}

double
TraCIAPI::VehicleScope::getTau(const std::string& vehicleID) const {
    return myParent.getDouble(CMD_GET_VEHICLE_VARIABLE, VAR_TAU, vehicleID);
}

double
TraCIAPI::VehicleScope::getImperfection(const std::string& vehicleID) const {
    return myParent.getDouble(CMD_GET_VEHICLE_VARIABLE, VAR_IMPERFECTION, vehicleID);
}

double
TraCIAPI::VehicleScope::getSpeedFactor(const std::string& vehicleID) const {
    return myParent.getDouble(CMD_GET_VEHICLE_VARIABLE, VAR_SPEED_FACTOR, vehicleID);
}

double
TraCIAPI::VehicleScope::getSpeedDeviation(const std::string& vehicleID) const {
    return myParent.getDouble(CMD_GET_VEHICLE_VARIABLE, VAR_SPEED_DEVIATION, vehicleID);
}

std::string
TraCIAPI::VehicleScope::getVehicleClass(const std::string& vehicleID) const {
    return myParent.getString(CMD_GET_VEHICLE_VARIABLE, VAR_VEHICLECLASS, vehicleID);
}

double
TraCIAPI::VehicleScope::getMinGap(const std::string& vehicleID) const {
    return myParent.getDouble(CMD_GET_VEHICLE_VARIABLE, VAR_MINGAP, vehicleID);
}

double
TraCIAPI::VehicleScope::getWidth(const std::string& vehicleID) const {
    return myParent.getDouble(CMD_GET_VEHICLE_VARIABLE, VAR_WIDTH, vehicleID);
}

double
TraCIAPI::VehicleScope::getLength(const std::string& vehicleID) const {
    return myParent.getDouble(CMD_GET_VEHICLE_VARIABLE, VAR_LENGTH, vehicleID);
}

double
TraCIAPI::VehicleScope::getHeight(const std::string& vehicleID) const {
    return myParent.getDouble(CMD_GET_VEHICLE_VARIABLE, VAR_HEIGHT, vehicleID);
}

double
TraCIAPI::VehicleScope::getAccumulatedWaitingTime(const std::string& vehicleID) const {
    return myParent.getDouble(CMD_GET_VEHICLE_VARIABLE, VAR_ACCUMULATED_WAITING_TIME, vehicleID);
}

double
TraCIAPI::VehicleScope::getAllowedSpeed(const std::string& vehicleID) const {
    return myParent.getDouble(CMD_GET_VEHICLE_VARIABLE, VAR_ALLOWED_SPEED, vehicleID);
}

int
TraCIAPI::VehicleScope::getPersonNumber(const std::string& vehicleID) const {
    return myParent.getInt(CMD_GET_VEHICLE_VARIABLE, VAR_PERSON_NUMBER, vehicleID);
}

double
TraCIAPI::VehicleScope::getSpeedWithoutTraCI(const std::string& vehicleID) const {
    return myParent.getDouble(CMD_GET_VEHICLE_VARIABLE, VAR_SPEED_WITHOUT_TRACI, vehicleID);
}

bool
TraCIAPI::VehicleScope::isRouteValid(const std::string& vehicleID) const {
    return myParent.getUnsignedByte(CMD_GET_VEHICLE_VARIABLE, VAR_ROUTE_VALID, vehicleID) != 0;
}

double
TraCIAPI::VehicleScope::getMaxSpeedLat(const std::string& vehicleID) const {
    return myParent.getDouble(CMD_GET_VEHICLE_VARIABLE, VAR_MAXSPEED_LAT, vehicleID);
}

double
TraCIAPI::VehicleScope::getMinGapLat(const std::string& vehicleID) const {
    return myParent.getDouble(CMD_GET_VEHICLE_VARIABLE, VAR_MINGAP_LAT, vehicleID);
}

std::string
TraCIAPI::VehicleScope::getLateralAlignment(const std::string& vehicleID) const {
    return myParent.getString(CMD_GET_VEHICLE_VARIABLE, VAR_LATALIGNMENT, vehicleID);
}

void
TraCIAPI::VehicleScope::add(const std::string& vehicleID,
                            const std::string& routeID,
                            const std::string& typeID,
                            std::string depart,
                            const std::string& departLane,
                            const std::string& departPos,
                            const std::string& departSpeed,
                            const std::string& arrivalLane,
                            const std::string& arrivalPos,
                            const std::string& arrivalSpeed,
                            const std::string& fromTaz,
                            const std::string& toTaz,
                            const std::string& line,
                            int personCapacity,
                            int personNumber) const {

    if (depart == "-1") {
        depart = toString(myParent.simulation.getCurrentTime() / 1000.0);
    }
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_COMPOUND);
    content.writeInt(14);
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(routeID);
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(typeID);
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(depart);
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(departLane);
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(departPos);
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(departSpeed);

    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(arrivalLane);
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(arrivalPos);
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(arrivalSpeed);

    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(fromTaz);
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(toTaz);
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(line);

    content.writeUnsignedByte(TYPE_INTEGER);
    content.writeInt(personCapacity);
    content.writeUnsignedByte(TYPE_INTEGER);
    content.writeInt(personNumber);

    myParent.send_commandSetValue(CMD_SET_VEHICLE_VARIABLE, ADD_FULL, vehicleID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLE_VARIABLE);
}


void
TraCIAPI::VehicleScope::remove(const std::string& vehicleID, char reason) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_BYTE);
    content.writeUnsignedByte(reason);
    myParent.send_commandSetValue(CMD_SET_VEHICLE_VARIABLE, REMOVE, vehicleID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLE_VARIABLE);

}

void
TraCIAPI::VehicleScope::changeTarget(const std::string& vehicleID, const std::string& edgeID) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(edgeID);
    myParent.send_commandSetValue(CMD_SET_VEHICLE_VARIABLE, CMD_CHANGETARGET, vehicleID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLE_VARIABLE);
}

void
TraCIAPI::VehicleScope::setRouteID(const std::string& vehicleID, const std::string& routeID) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(routeID);
    myParent.send_commandSetValue(CMD_SET_VEHICLE_VARIABLE, VAR_ROUTE_ID, vehicleID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLE_VARIABLE);
}


void
TraCIAPI::VehicleScope::setRoute(const std::string& vehicleID, const std::vector<std::string>& edges) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_STRINGLIST);
    content.writeInt((int)edges.size());
    for (int i = 0; i < (int)edges.size(); ++i) {
        content.writeString(edges[i]);
    }
    myParent.send_commandSetValue(CMD_SET_VEHICLE_VARIABLE, VAR_ROUTE, vehicleID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLE_VARIABLE);
}


void
TraCIAPI::VehicleScope::rerouteTraveltime(const std::string& vehicleID, bool currentTravelTimes) const {
    if (currentTravelTimes) {
        // updated edge weights with current network traveltimes (at most once per simulation step)
        SUMOTime time = myParent.simulation.getCurrentTime();
        if (time != LAST_TRAVEL_TIME_UPDATE) {
            LAST_TRAVEL_TIME_UPDATE = time;
        }
        std::vector<std::string> edges = myParent.edge.getIDList();
        for (std::vector<std::string>::iterator it = edges.begin(); it != edges.end(); ++it) {
            myParent.edge.adaptTraveltime(*it, myParent.edge.getTraveltime(*it));
        }
    }

    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_COMPOUND);
    content.writeInt(0);
    myParent.send_commandSetValue(CMD_SET_VEHICLE_VARIABLE, CMD_REROUTE_TRAVELTIME, vehicleID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLE_VARIABLE);
}

void
TraCIAPI::VehicleScope::moveTo(const std::string& vehicleID, const std::string& laneID, double position) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_COMPOUND);
    content.writeInt(2);
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(laneID);
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(position);
    myParent.send_commandSetValue(CMD_SET_VEHICLE_VARIABLE, VAR_MOVE_TO, vehicleID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLE_VARIABLE);
}

void
TraCIAPI::VehicleScope::moveToXY(const std::string& vehicleID, const std::string& edgeID, const int lane, const double x, const double y, const double angle, const int keepRoute) const {
    myParent.send_commandMoveToXY(vehicleID, edgeID, lane, x, y, angle, keepRoute);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLE_VARIABLE);
}


void
TraCIAPI::VehicleScope::slowDown(const std::string& vehicleID, double speed, SUMOTime duration) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_COMPOUND);
    content.writeInt(2);
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(speed);
    content.writeUnsignedByte(TYPE_INTEGER);
    content.writeInt((int)duration);
    myParent.send_commandSetValue(CMD_SET_VEHICLE_VARIABLE, CMD_SLOWDOWN, vehicleID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLE_VARIABLE);
}

void
TraCIAPI::VehicleScope::setSpeed(const std::string& vehicleID, double speed) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(speed);
    myParent.send_commandSetValue(CMD_SET_VEHICLE_VARIABLE, VAR_SPEED, vehicleID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLE_VARIABLE);
}

void
TraCIAPI::VehicleScope::setType(const std::string& vehicleID, const std::string& typeID) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(typeID);
    myParent.send_commandSetValue(CMD_SET_VEHICLE_VARIABLE, VAR_TYPE, vehicleID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLE_VARIABLE);
}

void
TraCIAPI::VehicleScope::setMaxSpeed(const std::string& vehicleID, double speed) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(speed);
    myParent.send_commandSetValue(CMD_SET_VEHICLE_VARIABLE, VAR_MAXSPEED, vehicleID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLE_VARIABLE);
}

void
TraCIAPI::VehicleScope::setColor(const std::string& vehicleID, const TraCIColor& c) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_COLOR);
    content.writeUnsignedByte(c.r);
    content.writeUnsignedByte(c.g);
    content.writeUnsignedByte(c.b);
    content.writeUnsignedByte(c.a);
    myParent.send_commandSetValue(CMD_SET_VEHICLE_VARIABLE, VAR_COLOR, vehicleID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLE_VARIABLE);
}

void
TraCIAPI::VehicleScope::setLine(const std::string& vehicleID, const std::string& line) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(line);
    myParent.send_commandSetValue(CMD_SET_VEHICLE_VARIABLE, VAR_LINE, vehicleID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLE_VARIABLE);
}

void
TraCIAPI::VehicleScope::setVia(const std::string& vehicleID, const std::vector<std::string>& via) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_STRINGLIST);
    content.writeInt((int)via.size());
    for (int i = 0; i < (int)via.size(); ++i) {
        content.writeString(via[i]);
    }
    myParent.send_commandSetValue(CMD_SET_VEHICLE_VARIABLE, VAR_VIA, vehicleID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLE_VARIABLE);
}


void
TraCIAPI::VehicleScope::setShapeClass(const std::string& vehicleID, const std::string& clazz) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(clazz);
    myParent.send_commandSetValue(CMD_SET_VEHICLE_VARIABLE, VAR_SHAPECLASS, vehicleID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLE_VARIABLE);
}


void
TraCIAPI::VehicleScope::setEmissionClass(const std::string& vehicleID, const std::string& clazz) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(clazz);
    myParent.send_commandSetValue(CMD_SET_VEHICLE_VARIABLE, VAR_EMISSIONCLASS, vehicleID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_VEHICLE_VARIABLE);
}


// ---------------------------------------------------------------------------
// // TraCIAPI::PersonScope-methods
//  ---------------------------------------------------------------------------

std::vector<std::string>
TraCIAPI::PersonScope::getIDList() const {
    return myParent.getStringVector(CMD_GET_PERSON_VARIABLE, ID_LIST, "");
}

int
TraCIAPI::PersonScope::getIDCount() const {
    return myParent.getInt(CMD_GET_PERSON_VARIABLE, ID_COUNT, "");
}

double
TraCIAPI::PersonScope::getSpeed(const std::string& personID) const {
    return myParent.getDouble(CMD_GET_PERSON_VARIABLE, VAR_SPEED, personID);
}

TraCIPosition
TraCIAPI::PersonScope::getPosition(const std::string& personID) const {
    return myParent.getPosition(CMD_GET_PERSON_VARIABLE, VAR_POSITION, personID);
}

std::string
TraCIAPI::PersonScope::getRoadID(const std::string& personID) const {
    return myParent.getString(CMD_GET_PERSON_VARIABLE, VAR_ROAD_ID, personID);
}

std::string
TraCIAPI::PersonScope::getTypeID(const std::string& personID) const {
    return myParent.getString(CMD_GET_PERSON_VARIABLE, VAR_TYPE, personID);
}

double
TraCIAPI::PersonScope::getWaitingTime(const std::string& personID) const {
    return myParent.getDouble(CMD_GET_PERSON_VARIABLE, VAR_WAITING_TIME, personID);
}

std::string
TraCIAPI::PersonScope::getNextEdge(const std::string& personID) const {
    return myParent.getString(CMD_GET_PERSON_VARIABLE, VAR_NEXT_EDGE, personID);
}


std::string
TraCIAPI::PersonScope::getVehicle(const std::string& personID) const {
    return myParent.getString(CMD_GET_PERSON_VARIABLE, VAR_VEHICLE, personID);
}

int
TraCIAPI::PersonScope::getRemainingStages(const std::string& personID) const {
    return myParent.getInt(CMD_GET_PERSON_VARIABLE, VAR_STAGES_REMAINING, personID);
}

int
TraCIAPI::PersonScope::getStage(const std::string& personID, int nextStageIndex) const {
    tcpip::Storage content;
    content.writeByte(TYPE_INTEGER);
    content.writeInt(nextStageIndex);
    return myParent.getInt(CMD_GET_PERSON_VARIABLE, VAR_STAGE, personID, &content);
}

std::vector<std::string>
TraCIAPI::PersonScope::getEdges(const std::string& personID, int nextStageIndex) const {
    tcpip::Storage content;
    content.writeByte(TYPE_INTEGER);
    content.writeInt(nextStageIndex);
    return myParent.getStringVector(CMD_GET_PERSON_VARIABLE, VAR_EDGES, personID, &content);
}

void
TraCIAPI::PersonScope::removeStages(const std::string& personID) const {
    // remove all stages after the current and then abort the current stage
    while (getRemainingStages(personID) > 1) {
        removeStage(personID, 1);
    }
    removeStage(personID, 0);
}


void
TraCIAPI::PersonScope::rerouteTraveltime(const std::string& personID) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_COMPOUND);
    content.writeInt(0);
    myParent.send_commandSetValue(CMD_SET_PERSON_VARIABLE, CMD_REROUTE_TRAVELTIME, personID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_PERSON_VARIABLE);
}

void
TraCIAPI::PersonScope::add(const std::string& personID, const std::string& edgeID, double pos, double depart, const std::string typeID) {
    if (depart > 0) {
        depart *= 1000;
    }
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_COMPOUND);
    content.writeInt(4);
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(typeID);
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(edgeID);
    content.writeUnsignedByte(TYPE_INTEGER);
    content.writeInt((int)depart);
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(pos);
    myParent.send_commandSetValue(CMD_SET_PERSON_VARIABLE, ADD, personID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_PERSON_VARIABLE);
}

void
TraCIAPI::PersonScope::appendWaitingStage(const std::string& personID, double duration, const std::string& description, const std::string& stopID) {
    duration *= 1000;
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_COMPOUND);
    content.writeInt(4);
    content.writeUnsignedByte(TYPE_INTEGER);
    content.writeInt(STAGE_WAITING);
    content.writeUnsignedByte(TYPE_INTEGER);
    content.writeInt((int)duration);
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(description);
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(stopID);
    myParent.send_commandSetValue(CMD_SET_PERSON_VARIABLE, APPEND_STAGE, personID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_PERSON_VARIABLE);
}

void
TraCIAPI::PersonScope::appendWalkingStage(const std::string& personID, const std::vector<std::string>& edges, double arrivalPos, double duration, double speed, const std::string& stopID) {
    if (duration > 0) {
        duration *= 1000;
    }
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_COMPOUND);
    content.writeInt(6);
    content.writeUnsignedByte(TYPE_INTEGER);
    content.writeInt(STAGE_WALKING);
    content.writeUnsignedByte(TYPE_STRINGLIST);
    content.writeStringList(edges);
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(arrivalPos);
    content.writeUnsignedByte(TYPE_INTEGER);
    content.writeInt((int)duration);
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(speed);
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(stopID);
    myParent.send_commandSetValue(CMD_SET_PERSON_VARIABLE, APPEND_STAGE, personID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_PERSON_VARIABLE);
}

void
TraCIAPI::PersonScope::appendDrivingStage(const std::string& personID, const std::string& toEdge, const std::string& lines, const std::string& stopID) {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_COMPOUND);
    content.writeInt(4);
    content.writeUnsignedByte(TYPE_INTEGER);
    content.writeInt(STAGE_DRIVING);
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(toEdge);
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(lines);
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(stopID);
    myParent.send_commandSetValue(CMD_SET_PERSON_VARIABLE, APPEND_STAGE, personID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_PERSON_VARIABLE);
}

void
TraCIAPI::PersonScope::removeStage(const std::string& personID, int nextStageIndex) const {
    tcpip::Storage content;
    content.writeByte(TYPE_INTEGER);
    content.writeInt(nextStageIndex);
    myParent.send_commandSetValue(CMD_SET_PERSON_VARIABLE, REMOVE_STAGE, personID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_PERSON_VARIABLE);
}


void
TraCIAPI::PersonScope::setSpeed(const std::string& personID, double speed) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(speed);
    myParent.send_commandSetValue(CMD_SET_PERSON_VARIABLE, VAR_SPEED, personID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_PERSON_VARIABLE);
}


void
TraCIAPI::PersonScope::setType(const std::string& personID, const std::string& typeID) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_STRING);
    content.writeString(typeID);
    myParent.send_commandSetValue(CMD_SET_PERSON_VARIABLE, VAR_TYPE, personID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_PERSON_VARIABLE);
}

void
TraCIAPI::PersonScope::setLength(const std::string& personID, double length) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(length);
    myParent.send_commandSetValue(CMD_SET_PERSON_VARIABLE, VAR_LENGTH, personID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_PERSON_VARIABLE);
}


void
TraCIAPI::PersonScope::setWidth(const std::string& personID, double width) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(width);
    myParent.send_commandSetValue(CMD_SET_PERSON_VARIABLE, VAR_WIDTH, personID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_PERSON_VARIABLE);
}

void
TraCIAPI::PersonScope::setHeight(const std::string& personID, double height) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(height);
    myParent.send_commandSetValue(CMD_SET_PERSON_VARIABLE, VAR_HEIGHT, personID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_PERSON_VARIABLE);
}

void
TraCIAPI::PersonScope::setMinGap(const std::string& personID, double minGap) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_DOUBLE);
    content.writeDouble(minGap);
    myParent.send_commandSetValue(CMD_SET_PERSON_VARIABLE, VAR_MINGAP, personID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_PERSON_VARIABLE);
}


void
TraCIAPI::PersonScope::setColor(const std::string& personID, const TraCIColor& c) const {
    tcpip::Storage content;
    content.writeUnsignedByte(TYPE_COLOR);
    content.writeUnsignedByte(c.r);
    content.writeUnsignedByte(c.g);
    content.writeUnsignedByte(c.b);
    content.writeUnsignedByte(c.a);
    myParent.send_commandSetValue(CMD_SET_PERSON_VARIABLE, VAR_COLOR, personID, content);
    tcpip::Storage inMsg;
    myParent.check_resultState(inMsg, CMD_SET_PERSON_VARIABLE);
}


/****************************************************************************/

