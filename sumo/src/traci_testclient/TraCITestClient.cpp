/****************************************************************************/
/// @file    TraCITestClient.cpp
/// @author  Friedemann Wesner
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Axel Wegener
/// @author  Michael Behrisch
/// @date    2008/04/07
/// @version $Id$
///
/// A dummy client to simulate communication to a TraCI server
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
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <ctime>
#include <cstdlib>

#define BUILD_TCPIP
#include <foreign/tcpip/storage.h>
#include <foreign/tcpip/socket.h>

#include <traci-server/TraCIConstants.h>
#include <utils/common/SUMOTime.h>
#include "TraCITestClient.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace testclient;


// ===========================================================================
// method definitions
// ===========================================================================
TraCITestClient::TraCITestClient(std::string outputFileName)
    : socket(NULL), outputFileName(outputFileName), answerLog("") {
    answerLog.setf(std::ios::fixed , std::ios::floatfield); // use decimal format
    answerLog.setf(std::ios::showpoint); // print decimal point
    answerLog << std::setprecision(2);
}


TraCITestClient::~TraCITestClient() {
    writeResult();
    delete socket;
}


bool
TraCITestClient::connect(int port, std::string host) {
    std::stringstream msg;
    socket = new tcpip::Socket(host, port);
    try {
        socket->connect();
    } catch (tcpip::SocketException& e) {
        msg << "#Error while connecting: " << e.what();
        errorMsg(msg);
        return false;
    }
    return true;
}


bool
TraCITestClient::close() {
    if (socket != NULL) {
        socket->close();
    }
    return true;
}


bool
TraCITestClient::run(std::string fileName, int port, std::string host) {
    std::ifstream defFile;
    std::string fileContentStr;
    std::stringstream fileContent;
    std::string lineCommand;
    std::stringstream msg;
    int repNo = 1;
    bool commentRead = false;

    if (!connect(port, host)) {
        return false;
    }

    // read definition file and trigger commands according to it
    defFile.open(fileName.c_str());
    if (!defFile) {
        msg << "Can not open definition file " << fileName << std::endl;
        errorMsg(msg);
        return false;
    }
    defFile.unsetf(std::ios::dec);

    while (defFile >> lineCommand) {
        repNo = 1;
        if (lineCommand.compare("%") == 0) {
            // a comment was read
            commentRead = !commentRead;
            continue;
        }
        if (commentRead) {
            // wait until end of comment is reached
            continue;
        }
        if (lineCommand.compare("repeat") == 0) {
            defFile >> repNo;
            defFile >> lineCommand;
        }
        if (lineCommand.compare("simstep2") == 0) {
            // read parameter for command simulation step and trigger command
            std::string time;
            defFile >> time;
            for (int i = 0; i < repNo; i++) {
                commandSimulationStep2(string2time(time));
            }
        } else if (lineCommand.compare("getvariable") == 0) {
            // trigger command GetXXXVariable
            int domID, varID;
            std::string objID;
            defFile >> domID >> varID >> objID;
            commandGetVariable(domID, varID, objID);
        } else if (lineCommand.compare("getvariable_plus") == 0) {
            // trigger command GetXXXVariable with one parameter
            int domID, varID;
            std::string objID;
            defFile >> domID >> varID >> objID;
            commandGetVariablePlus(domID, varID, objID, defFile);
        } else if (lineCommand.compare("subscribevariable") == 0) {
            // trigger command SubscribeXXXVariable
            int domID, varNo;
            std::string beginTime, endTime;
            std::string objID;
            defFile >> domID >> objID >> beginTime >> endTime >> varNo;
            commandSubscribeVariable(domID, objID, string2time(beginTime), string2time(endTime), varNo, defFile);
        }  else if (lineCommand.compare("setvalue") == 0) {
            // trigger command SetXXXValue
            int domID, varID;
            std::string objID;
            defFile >> domID >> varID >> objID;
            commandSetValue(domID, varID, objID, defFile);
        } else {
            msg << "Error in definition file: " << lineCommand << " is not a valid command";
            errorMsg(msg);
            commandClose();
            close();
            return false;
        }
    }
    defFile.close();
    commandClose();
    close();
    return true;
}


// ---------- Commands handling
void
TraCITestClient::commandSimulationStep2(SUMOTime time) {
    tcpip::Storage outMsg;
    tcpip::Storage inMsg;
    std::stringstream msg;

    if (socket == NULL) {
        msg << "#Error while sending command: no connection to server";
        errorMsg(msg);
        return;
    }

    // command length
    outMsg.writeUnsignedByte(1 + 1 + 4);
    // command id
    outMsg.writeUnsignedByte(CMD_SIMSTEP2);
    outMsg.writeInt(time);
    // send request message
    try {
        socket->sendExact(outMsg);
    } catch (tcpip::SocketException& e) {
        msg << "Error while sending command: " << e.what();
        errorMsg(msg);
        return;
    }
    answerLog << std::endl << "-> Command sent: <SimulationStep2>:" << std::endl;
    // receive answer message
    try {
        socket->receiveExact(inMsg);
    } catch (tcpip::SocketException& e) {
        msg << "Error while receiving command: " << e.what();
        errorMsg(msg);
        return;
    }
    // validate result state
    if (!reportResultState(inMsg, CMD_SIMSTEP2)) {
        return;
    }
    // validate answer message
    validateSimulationStep2(inMsg);
}


void
TraCITestClient::commandGetVariable(int domID, int varID, const std::string& objID) {
    tcpip::Storage outMsg, inMsg;
    std::stringstream msg;
    if (socket == NULL) {
        msg << "#Error while sending command: no connection to server";
        errorMsg(msg);
        return;
    }
    // command length
    outMsg.writeUnsignedByte(1 + 1 + 1 + 4 + (int) objID.length());
    // command id
    outMsg.writeUnsignedByte(domID);
    // variable id
    outMsg.writeUnsignedByte(varID);
    // object id
    outMsg.writeString(objID);

    // send request message
    try {
        socket->sendExact(outMsg);
    } catch (tcpip::SocketException& e) {
        msg << "Error while sending command: " << e.what();
        errorMsg(msg);
        return;
    }
    answerLog << std::endl << "-> Command sent: <GetVariable>:" << std::endl
              << "  domID=" << domID << " varID=" << varID
              << " objID=" << objID << std::endl;

    // receive answer message
    try {
        socket->receiveExact(inMsg);
        if (!reportResultState(inMsg, domID)) {
            return;
        }
    } catch (tcpip::SocketException& e) {
        msg << "Error while receiving command: " << e.what();
        errorMsg(msg);
        return;
    }
    // validate result state
    try {
        int length = inMsg.readUnsignedByte();
        if (length == 0) {
            length = inMsg.readInt();
        }
        int cmdId = inMsg.readUnsignedByte();
        if (cmdId != (domID + 0x10)) {
            answerLog << "#Error: received response with command id: " << cmdId
                      << "but expected: " << (int)(domID + 0x10) << std::endl;
            return;
        }
        answerLog << "  CommandID=" << cmdId;
        answerLog << "  VariableID=" << inMsg.readUnsignedByte();
        answerLog << "  ObjectID=" << inMsg.readString();
        int valueDataType = inMsg.readUnsignedByte();
        answerLog << " valueDataType=" << valueDataType;
        readAndReportTypeDependent(inMsg, valueDataType);
    } catch (tcpip::SocketException& e) {
        msg << "Error while receiving command: " << e.what();
        errorMsg(msg);
        return;
    }
}


void
TraCITestClient::commandGetVariablePlus(int domID, int varID, const std::string& objID, std::ifstream& defFile) {
    std::stringstream msg;
    if (socket == NULL) {
        msg << "#Error while sending command: no connection to server";
        errorMsg(msg);
        return;
    }
    tcpip::Storage outMsg, inMsg, tmp;
    const int dataLength = setValueTypeDependant(tmp, defFile, msg);
    std::string msgS = msg.str();
    if (msgS != "") {
        errorMsg(msg);
    }
    // command length (domID, varID, objID, dataType, data)
    outMsg.writeUnsignedByte(1 + 1 + 1 + 4 + (int) objID.length() + dataLength);
    // command id
    outMsg.writeUnsignedByte(domID);
    // variable id
    outMsg.writeUnsignedByte(varID);
    // object id
    outMsg.writeString(objID);
    // data type
    outMsg.writeStorage(tmp);
    // send request message
    try {
        socket->sendExact(outMsg);
    } catch (tcpip::SocketException& e) {
        msg << "Error while sending command: " << e.what();
        errorMsg(msg);
        return;
    }
    answerLog << std::endl << "-> Command sent: <GetVariable>:" << std::endl
              << "  domID=" << domID << " varID=" << varID
              << " objID=" << objID << std::endl;

    // receive answer message
    try {
        socket->receiveExact(inMsg);
        if (!reportResultState(inMsg, domID)) {
            return;
        }
    } catch (tcpip::SocketException& e) {
        msg << "Error while receiving command: " << e.what();
        errorMsg(msg);
        return;
    }
    // validate result state
    try {
        int length = inMsg.readUnsignedByte();
        if (length == 0) {
            length = inMsg.readInt();
        }
        int cmdId = inMsg.readUnsignedByte();
        if (cmdId != (domID + 0x10)) {
            answerLog << "#Error: received response with command id: " << cmdId
                      << "but expected: " << (int)(domID + 0x10) << std::endl;
            return;
        }
        answerLog << "  CommandID=" << cmdId;
        answerLog << "  VariableID=" << inMsg.readUnsignedByte();
        answerLog << "  ObjectID=" << inMsg.readString();
        int valueDataType = inMsg.readUnsignedByte();
        answerLog << " valueDataType=" << valueDataType;
        readAndReportTypeDependent(inMsg, valueDataType);
    } catch (tcpip::SocketException& e) {
        msg << "Error while receiving command: " << e.what();
        errorMsg(msg);
        return;
    }
}


void
TraCITestClient::commandSubscribeVariable(int domID, const std::string& objID, int beginTime, int endTime, int varNo, std::ifstream& defFile) {
    std::stringstream msg;
    if (socket == NULL) {
        msg << "#Error while sending command: no connection to server";
        errorMsg(msg);
        return;
    }
    tcpip::Storage outMsg, inMsg, tmp;
    std::string msgS = msg.str();
    if (msgS != "") {
        errorMsg(msg);
    }
    // command length (domID, beginTime, endTime, objID, varNo, <vars>)
    outMsg.writeUnsignedByte(0);
    outMsg.writeInt(/*1 + 4 +*/ 5 + 1 + 4 + 4 + 4 + (int) objID.length() + 1 + varNo);
    // command id
    outMsg.writeUnsignedByte(domID);
    // time
    outMsg.writeInt(beginTime);
    outMsg.writeInt(endTime);
    // object id
    outMsg.writeString(objID);
    // command id
    outMsg.writeUnsignedByte(varNo);
    for (int i = 0; i < varNo; ++i) {
        int var;
        defFile >> var;
        // variable id
        outMsg.writeUnsignedByte(var);
    }
    // send request message
    try {
        socket->sendExact(outMsg);
    } catch (tcpip::SocketException& e) {
        msg << "Error while sending command: " << e.what();
        errorMsg(msg);
        return;
    }
    answerLog << std::endl << "-> Command sent: <SubscribeVariable>:" << std::endl
              << "  domID=" << domID << " objID=" << objID << " with " << varNo << " variables" << std::endl;

    // receive answer message
    try {
        socket->receiveExact(inMsg);
        if (!reportResultState(inMsg, domID)) {
            return;
        }
    } catch (tcpip::SocketException& e) {
        msg << "Error while receiving command: " << e.what();
        errorMsg(msg);
        return;
    }
    // validate result state
    try {
        validateSubscription(inMsg);
    } catch (tcpip::SocketException& e) {
        msg << "Error while receiving command: " << e.what();
        errorMsg(msg);
        return;
    }
}



int
TraCITestClient::setValueTypeDependant(tcpip::Storage& into, std::ifstream& defFile, std::stringstream& msg) {
    std::string dataTypeS, valueS;
    defFile >> dataTypeS;
    if (dataTypeS == "<airDist>") {
        into.writeUnsignedByte(REQUEST_AIRDIST);
        return 1;
    } else if (dataTypeS == "<drivingDist>") {
        into.writeUnsignedByte(REQUEST_DRIVINGDIST);
        return 1;
    }
    defFile >> valueS;
    if (dataTypeS == "<int>") {
        into.writeUnsignedByte(TYPE_INTEGER);
        into.writeInt(atoi(valueS.c_str()));
        return 4 + 1;
    } else if (dataTypeS == "<byte>") {
        into.writeUnsignedByte(TYPE_BYTE);
        into.writeByte(atoi(valueS.c_str()));
        return 1 + 1;
    }  else if (dataTypeS == "<ubyte>") {
        into.writeUnsignedByte(TYPE_UBYTE);
        into.writeUnsignedByte(atoi(valueS.c_str()));
        return 1 + 1;
    } else if (dataTypeS == "<float>") {
        into.writeUnsignedByte(TYPE_FLOAT);
        into.writeFloat(float(atof(valueS.c_str())));
        return 4 + 1;
    } else if (dataTypeS == "<double>") {
        into.writeUnsignedByte(TYPE_DOUBLE);
        into.writeDouble(atof(valueS.c_str()));
        return 8 + 1;
    } else if (dataTypeS == "<string>") {
        into.writeUnsignedByte(TYPE_STRING);
        into.writeString(valueS);
        return 4 + 1 + (int) valueS.length();
    } else if (dataTypeS == "<string*>") {
        std::vector<std::string> slValue;
        int number = atoi(valueS.c_str());
        int length = 1 + 4;
        for (int i = 0; i < number; ++i) {
            std::string tmp;
            defFile >> tmp;
            slValue.push_back(tmp);
            length += 4 + int(tmp.length());
        }
        into.writeUnsignedByte(TYPE_STRINGLIST);
        into.writeStringList(slValue);
        return length;
    } else if (dataTypeS == "<compound>") {
        into.writeUnsignedByte(TYPE_COMPOUND);
        int number = atoi(valueS.c_str());
        into.writeInt(number);
        int length = 1 + 4;
        for (int i = 0; i < number; ++i) {
            length += setValueTypeDependant(into, defFile, msg);
        }
        return length;
    } else if (dataTypeS == "<color>") {
        into.writeUnsignedByte(TYPE_COLOR);
        into.writeUnsignedByte(atoi(valueS.c_str()));
        for (int i = 0; i < 3; ++i) {
            defFile >> valueS;
            into.writeUnsignedByte(atoi(valueS.c_str()));
        }
        return 1 + 4;
    } else if (dataTypeS == "<position2D>") {
        into.writeUnsignedByte(POSITION_2D);
        into.writeDouble(atof(valueS.c_str()));
        defFile >> valueS;
        into.writeDouble(atof(valueS.c_str()));
        return 1 + 8 + 8;
    } else if (dataTypeS == "<position3D>") {
        into.writeUnsignedByte(POSITION_3D);
        into.writeDouble(atof(valueS.c_str()));
        defFile >> valueS;
        into.writeDouble(atof(valueS.c_str()));
        defFile >> valueS;
        into.writeDouble(atof(valueS.c_str()));
        return 1 + 8 + 8 + 8;
    } else if (dataTypeS == "<positionRoadmap>") {
        into.writeUnsignedByte(POSITION_ROADMAP);
        into.writeString(valueS);
        int length = 1 + 8 + (int) valueS.length();
        defFile >> valueS;
        into.writeDouble(atof(valueS.c_str()));
        defFile >> valueS;
        into.writeUnsignedByte(atoi(valueS.c_str()));
        return length + 4 + 1;
    } else if (dataTypeS == "<shape>") {
        into.writeUnsignedByte(TYPE_POLYGON);
        int number = atoi(valueS.c_str());
        into.writeUnsignedByte(number);
        int length = 1 + 1;
        for (int i = 0; i < number; ++i) {
            std::string x, y;
            defFile >> x >> y;
            into.writeDouble(atof(x.c_str()));
            into.writeDouble(atof(y.c_str()));
            length += 8 + 8;
        }
        return length;
    }
    msg << "## Unknown data type: " << dataTypeS;
    return 0;
}

void
TraCITestClient::commandSetValue(int domID, int varID, const std::string& objID, std::ifstream& defFile) {
    std::stringstream msg;
    if (socket == NULL) {
        msg << "#Error while sending command: no connection to server";
        errorMsg(msg);
        return;
    }
    tcpip::Storage outMsg, inMsg, tmp;
    int dataLength = setValueTypeDependant(tmp, defFile, msg);
    std::string msgS = msg.str();
    if (msgS != "") {
        errorMsg(msg);
    }
    // command length (domID, varID, objID, dataType, data)
    outMsg.writeUnsignedByte(1 + 1 + 1 + 4 + (int) objID.length() + dataLength);
    // command id
    outMsg.writeUnsignedByte(domID);
    // variable id
    outMsg.writeUnsignedByte(varID);
    // object id
    outMsg.writeString(objID);
    // data type
    outMsg.writeStorage(tmp);
    // send request message
    try {
        socket->sendExact(outMsg);
    } catch (tcpip::SocketException& e) {
        msg << "Error while sending command: " << e.what();
        errorMsg(msg);
        return;
    }
    answerLog << std::endl << "-> Command sent: <SetValue>:" << std::endl
              << "  domID=" << domID << " varID=" << varID
              << " objID=" << objID << std::endl;

    // receive answer message
    try {
        socket->receiveExact(inMsg);
        if (!reportResultState(inMsg, domID)) {
            return;
        }
    } catch (tcpip::SocketException& e) {
        msg << "Error while receiving command: " << e.what();
        errorMsg(msg);
        return;
    }
}




void
TraCITestClient::commandClose() {
    tcpip::Storage outMsg;
    tcpip::Storage inMsg;
    std::stringstream msg;

    if (socket == NULL) {
        msg << "#Error while sending command: no connection to server";
        errorMsg(msg);
        return;
    }

    // command length
    outMsg.writeUnsignedByte(1 + 1);
    // command id
    outMsg.writeUnsignedByte(CMD_CLOSE);

    // send request message
    try {
        socket->sendExact(outMsg);
    } catch (tcpip::SocketException& e) {
        msg << "Error while sending command: " << e.what();
        errorMsg(msg);
        return;
    }

    answerLog << std::endl << "-> Command sent: <Close>:" << std::endl;

    // receive answer message
    try {
        socket->receiveExact(inMsg);
    } catch (tcpip::SocketException& e) {
        msg << "Error while receiving command: " << e.what();
        errorMsg(msg);
        return;
    }

    // validate result state
    if (!reportResultState(inMsg, CMD_CLOSE)) {
        return;
    }
}


bool
TraCITestClient::validateSimulationStep2(tcpip::Storage& inMsg) {
    try {
        int noSubscriptions = inMsg.readInt();
        for (int s = 0; s < noSubscriptions; ++s) {
            /*
            if (!reportResultState(inMsg, CMD_SIMSTEP2, true)) {
            	return false;
            }
            */
            if (!validateSubscription(inMsg)) {
                return false;
            }
        }
    } catch (std::invalid_argument& e) {
        answerLog << "#Error while reading message:" << e.what() << std::endl;
        return false;
    }
    return true;
}


bool
TraCITestClient::validateSubscription(tcpip::Storage& inMsg) {
    try {
        int length = inMsg.readUnsignedByte();
        if (length == 0) {
            length = inMsg.readInt();
        }
        int cmdId = inMsg.readUnsignedByte();
        if (cmdId < 0xe0 || cmdId > 0xef) {
            answerLog << "#Error: received response with command id: " << cmdId << " but expected a subscription response (0xe0-0xef)" << std::endl;
            return false;
        }
        answerLog << "  CommandID=" << cmdId;
        answerLog << "  ObjectID=" << inMsg.readString();
        unsigned int varNo = inMsg.readUnsignedByte();
        answerLog << "  #variables=" << varNo << std::endl;
        for (unsigned int i = 0; i < varNo; ++i) {
            answerLog << "      VariableID=" << inMsg.readUnsignedByte();
            bool ok = inMsg.readUnsignedByte() == RTYPE_OK;
            answerLog << "      ok=" << ok;
            int valueDataType = inMsg.readUnsignedByte();
            answerLog << " valueDataType=" << valueDataType;
            readAndReportTypeDependent(inMsg, valueDataType);
        }
    } catch (std::invalid_argument& e) {
        answerLog << "#Error while reading message:" << e.what() << std::endl;
        return false;
    }
    return true;
}


bool
TraCITestClient::readAndReportTypeDependent(tcpip::Storage& inMsg, int valueDataType) {
    if (valueDataType == TYPE_UBYTE) {
        int ubyte = inMsg.readUnsignedByte();
        answerLog << " Unsigned Byte Value: " << ubyte << std::endl;
    } else if (valueDataType == TYPE_BYTE) {
        int byte = inMsg.readByte();
        answerLog << " Byte value: " << byte << std::endl;
    } else if (valueDataType == TYPE_INTEGER) {
        int integer = inMsg.readInt();
        answerLog << " Int value: " << integer << std::endl;
    } else if (valueDataType == TYPE_FLOAT) {
        float floatv = inMsg.readFloat();
        if (floatv < 0.1 && floatv > 0) {
            answerLog.setf(std::ios::scientific, std::ios::floatfield);
        }
        answerLog << " float value: " << floatv << std::endl;
        answerLog.setf(std::ios::fixed , std::ios::floatfield); // use decimal format
        answerLog.setf(std::ios::showpoint); // print decimal point
        answerLog << std::setprecision(2);
    } else if (valueDataType == TYPE_DOUBLE) {
        double doublev = inMsg.readDouble();
        answerLog << " Double value: " << doublev << std::endl;
    } else if (valueDataType == TYPE_BOUNDINGBOX) {
        testclient::BoundingBox box;
        box.lowerLeft.x = inMsg.readDouble();
        box.lowerLeft.y = inMsg.readDouble();
        box.upperRight.x = inMsg.readDouble();
        box.upperRight.y = inMsg.readDouble();
        answerLog << " BoundaryBoxValue: lowerLeft x=" << box.lowerLeft.x
                  << " y=" << box.lowerLeft.y << " upperRight x=" << box.upperRight.x
                  << " y=" << box.upperRight.y << std::endl;
    } else if (valueDataType == TYPE_POLYGON) {
        int length = inMsg.readUnsignedByte();
        answerLog << " PolygonValue: ";
        for (int i = 0; i < length; i++) {
            SUMOReal x = inMsg.readDouble();
            SUMOReal y = inMsg.readDouble();
            answerLog << "(" << x << "," << y << ") ";
        }
        answerLog << std::endl;
    } else if (valueDataType == POSITION_3D) {
        SUMOReal x = inMsg.readDouble();
        SUMOReal y = inMsg.readDouble();
        SUMOReal z = inMsg.readDouble();
        answerLog << " Position3DValue: " << std::endl;
        answerLog << " x: " << x << " y: " << y
                  << " z: " << z << std::endl;
    } else if (valueDataType == POSITION_ROADMAP) {
        std::string roadId = inMsg.readString();
        SUMOReal pos = inMsg.readDouble();
        int laneId = inMsg.readUnsignedByte();
        answerLog << " RoadMapPositionValue: roadId=" << roadId
                  << " pos=" << pos
                  << " laneId=" << laneId << std::endl;
    } else if (valueDataType == TYPE_TLPHASELIST) {
        int length = inMsg.readUnsignedByte();
        answerLog << " TLPhaseListValue: length=" << length << std::endl;
        for (int i = 0; i < length; i++) {
            std::string pred = inMsg.readString();
            std::string succ = inMsg.readString();
            int phase = inMsg.readUnsignedByte();
            answerLog << " precRoad=" << pred << " succRoad=" << succ
                      << " phase=";
            switch (phase) {
                case TLPHASE_RED:
                    answerLog << "red" << std::endl;
                    break;
                case TLPHASE_YELLOW:
                    answerLog << "yellow" << std::endl;
                    break;
                case TLPHASE_GREEN:
                    answerLog << "green" << std::endl;
                    break;
                default:
                    answerLog << "#Error: unknown phase value" << (int)phase << std::endl;
                    return false;
            }
        }
    } else if (valueDataType == TYPE_STRING) {
        std::string s = inMsg.readString();
        answerLog << " string value: " << s << std::endl;
    } else if (valueDataType == TYPE_STRINGLIST) {
        std::vector<std::string> s = inMsg.readStringList();
        answerLog << " string list value: [ " << std::endl;
        for (std::vector<std::string>::iterator i = s.begin(); i != s.end(); ++i) {
            if (i != s.begin()) {
                answerLog << ", ";
            }
            answerLog << '"' << *i << '"';
        }
        answerLog << " ]" << std::endl;
    } else if (valueDataType == TYPE_COMPOUND) {
        int no = inMsg.readInt();
        answerLog << " compound value with " << no << " members: [ " << std::endl;
        for (int i = 0; i < no; ++i) {
            int currentValueDataType = inMsg.readUnsignedByte();
            answerLog << " valueDataType=" << currentValueDataType;
            readAndReportTypeDependent(inMsg, currentValueDataType);
        }
        answerLog << " ]" << std::endl;
    } else if (valueDataType == POSITION_2D) {
        SUMOReal xv = inMsg.readDouble();
        SUMOReal yv = inMsg.readDouble();
        answerLog << " position value: (" << xv << "," << yv << ")" << std::endl;
    } else if (valueDataType == TYPE_COLOR) {
        int r = inMsg.readUnsignedByte();
        int g = inMsg.readUnsignedByte();
        int b = inMsg.readUnsignedByte();
        int a = inMsg.readUnsignedByte();
        answerLog << " color value: (" << r << "," << g << "," << b << "," << a << ")" << std::endl;
    } else {
        answerLog << "#Error: unknown valueDataType!" << std::endl;
        return false;
    }
    return true;
}


void
TraCITestClient::writeResult() {
    time_t seconds;
    tm* locTime;

    std::ofstream outFile(outputFileName.c_str());
    if (!outFile) {
        std::cerr << "Unable to write result file" << std::endl;
    }
    time(&seconds);
    locTime = localtime(&seconds);
    outFile << "TraCITestClient output file. Date: " << asctime(locTime) << std::endl;
    outFile << answerLog.str();
    outFile.close();
}


void
TraCITestClient::errorMsg(std::stringstream& msg) {
    std::cerr << msg.str() << std::endl;
    answerLog << "----" << std::endl << msg.str() << std::endl;
}



bool
TraCITestClient::reportResultState(tcpip::Storage& inMsg, int command, bool ignoreCommandId) {
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
            answerLog << "#Error: received status response to command: " << cmdId
                      << " but expected: " << command << std::endl;
            return false;
        }
        resultType = inMsg.readUnsignedByte();
        msg = inMsg.readString();
    } catch (std::invalid_argument&) {
        answerLog << "#Error: an exception was thrown while reading result state message" << std::endl;
        return false;
    }
    switch (resultType) {
        case RTYPE_ERR:
            answerLog << ".. Answered with error to command (" << cmdId << "), [description: " << msg << "]" << std::endl;
            return false;
        case RTYPE_NOTIMPLEMENTED:
            answerLog << ".. Sent command is not implemented (" << cmdId << "), [description: " << msg << "]" << std::endl;
            return false;
        case RTYPE_OK:
            answerLog << ".. Command acknowledged (" << cmdId << "), [description: " << msg << "]" << std::endl;
            break;
        default:
            answerLog << ".. Answered with unknown result code(" << resultType << ") to command(" << cmdId
                      << "), [description: " << msg << "]" << std::endl;
            return false;
    }
    if ((cmdStart + cmdLength) != (int) inMsg.position()) {
        answerLog << "#Error: command at position " << cmdStart << " has wrong length" << std::endl;
        return false;
    }

    return true;
}


