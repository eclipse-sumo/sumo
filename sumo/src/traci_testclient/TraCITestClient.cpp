/****************************************************************************/
/// @file    TraCITestClient.cpp
/// @author  Friedemann Wesner
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Axel Wegener
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    2008/04/07
/// @version $Id$
///
/// A test execution class
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors
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
#include <utils/common/ToString.h>
#include "TraCITestClient.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
TraCITestClient::TraCITestClient(std::string outputFileName)
    : outputFileName(outputFileName), answerLog("") {
    answerLog.setf(std::ios::fixed , std::ios::floatfield); // use decimal format
    answerLog.setf(std::ios::showpoint); // print decimal point
    answerLog << std::setprecision(2);
}


TraCITestClient::~TraCITestClient() {
    writeResult();
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

    // try to connect
    try {
        TraCIAPI::connect(host, port);
    } catch (tcpip::SocketException& e) {
        std::stringstream msg;
        msg << "#Error while connecting: " << e.what();
        errorMsg(msg);
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
                commandSimulationStep(string2time(time));
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
            std::stringstream msg;
            tcpip::Storage tmp;
            setValueTypeDependant(tmp, defFile, msg);
            std::string msgS = msg.str();
            if (msgS != "") {
                errorMsg(msg);
            }
            commandGetVariable(domID, varID, objID, &tmp);
        } else if (lineCommand.compare("subscribevariable") == 0) {
            // trigger command SubscribeXXXVariable
            int domID, varNo;
            std::string beginTime, endTime;
            std::string objID;
            defFile >> domID >> objID >> beginTime >> endTime >> varNo;
            commandSubscribeObjectVariable(domID, objID, string2time(beginTime), string2time(endTime), varNo, defFile);
        }  else if (lineCommand.compare("subscribecontext") == 0) {
            // trigger command SubscribeXXXVariable
            int domID, varNo, domain;
            SUMOReal range;
            std::string beginTime, endTime;
            std::string objID;
            defFile >> domID >> objID >> beginTime >> endTime >> domain >> range >> varNo;
            commandSubscribeContextVariable(domID, objID, string2time(beginTime), string2time(endTime), domain, range, varNo, defFile);
        }  else if (lineCommand.compare("setvalue") == 0) {
            // trigger command SetXXXValue
            int domID, varID;
            std::string objID;
            defFile >> domID >> varID >> objID;
            commandSetValue(domID, varID, objID, defFile);
        }  else if (lineCommand.compare("testAPI") == 0) {
            // call all native API methods
            testAPI();
        } else {
            msg << "Error in definition file: " << lineCommand << " is not a valid command";
            errorMsg(msg);
            commandClose();
            closeSocket();
            return false;
        }
    }
    defFile.close();
    commandClose();
    closeSocket();
    return true;
}


// ---------- Commands handling
void
TraCITestClient::commandSimulationStep(SUMOTime time) {
    send_commandSimulationStep(time);
    answerLog << std::endl << "-> Command sent: <SimulationStep2>:" << std::endl;
    tcpip::Storage inMsg;
    try {
        std::string acknowledgement;
        check_resultState(inMsg, CMD_SIMSTEP2, false, &acknowledgement);
        answerLog << acknowledgement << std::endl;
        validateSimulationStep2(inMsg);
    } catch (tcpip::SocketException& e) {
        answerLog << e.what() << std::endl;
    }
}


void
TraCITestClient::commandClose() {
    send_commandClose();
    answerLog << std::endl << "-> Command sent: <Close>:" << std::endl;
    try {
        tcpip::Storage inMsg;
        std::string acknowledgement;
        check_resultState(inMsg, CMD_CLOSE, false, &acknowledgement);
        answerLog << acknowledgement << std::endl;
    } catch (tcpip::SocketException& e) {
        answerLog << e.what() << std::endl;
    }
}


void
TraCITestClient::commandGetVariable(int domID, int varID, const std::string& objID, tcpip::Storage* addData) {
    send_commandGetVariable(domID, varID, objID, addData);
    answerLog << std::endl << "-> Command sent: <GetVariable>:" << std::endl
              << "  domID=" << domID << " varID=" << varID
              << " objID=" << objID << std::endl;
    tcpip::Storage inMsg;
    try {
        std::string acknowledgement;
        check_resultState(inMsg, domID, false, &acknowledgement);
        answerLog << acknowledgement << std::endl;
    } catch (tcpip::SocketException& e) {
        answerLog << e.what() << std::endl;
        return;
    }
    check_commandGetResult(inMsg, domID, -1, false);
    // report result state
    try {
        int variableID = inMsg.readUnsignedByte();
        std::string objectID = inMsg.readString();
        answerLog <<  "  CommandID=" << (domID + 0x10) << "  VariableID=" << variableID << "  ObjectID=" << objectID;
        int valueDataType = inMsg.readUnsignedByte();
        answerLog << " valueDataType=" << valueDataType;
        readAndReportTypeDependent(inMsg, valueDataType);
    } catch (tcpip::SocketException& e) {
        std::stringstream msg;
        msg << "Error while receiving command: " << e.what();
        errorMsg(msg);
        return;
    }
}


void
TraCITestClient::commandSetValue(int domID, int varID, const std::string& objID, std::ifstream& defFile) {
    std::stringstream msg;
    tcpip::Storage inMsg, tmp;
    setValueTypeDependant(tmp, defFile, msg);
    std::string msgS = msg.str();
    if (msgS != "") {
        errorMsg(msg);
    }
    send_commandSetValue(domID, varID, objID, tmp);
    answerLog << std::endl << "-> Command sent: <SetValue>:" << std::endl
              << "  domID=" << domID << " varID=" << varID
              << " objID=" << objID << std::endl;
    try {
        std::string acknowledgement;
        check_resultState(inMsg, domID, false, &acknowledgement);
        answerLog << acknowledgement << std::endl;
    } catch (tcpip::SocketException& e) {
        answerLog << e.what() << std::endl;
    }
}


void
TraCITestClient::commandSubscribeObjectVariable(int domID, const std::string& objID, SUMOTime beginTime, SUMOTime endTime, int varNo, std::ifstream& defFile) {
    std::vector<int> vars;
    for (int i = 0; i < varNo; ++i) {
        int var;
        defFile >> var;
        // variable id
        vars.push_back(var);
    }
    send_commandSubscribeObjectVariable(domID, objID, beginTime, endTime, vars);
    answerLog << std::endl << "-> Command sent: <SubscribeVariable>:" << std::endl
              << "  domID=" << domID << " objID=" << objID << " with " << varNo << " variables" << std::endl;
    tcpip::Storage inMsg;
    try {
        std::string acknowledgement;
        check_resultState(inMsg, domID, false, &acknowledgement);
        answerLog << acknowledgement << std::endl;
        validateSubscription(inMsg);
    } catch (tcpip::SocketException& e) {
        answerLog << e.what() << std::endl;
    }
}


void
TraCITestClient::commandSubscribeContextVariable(int domID, const std::string& objID, SUMOTime beginTime, SUMOTime endTime,
        int domain, SUMOReal range, int varNo, std::ifstream& defFile) {
    std::vector<int> vars;
    for (int i = 0; i < varNo; ++i) {
        int var;
        defFile >> var;
        // variable id
        vars.push_back(var);
    }
    send_commandSubscribeObjectContext(domID, objID, beginTime, endTime, domain, range, vars);
    answerLog << std::endl << "-> Command sent: <SubscribeContext>:" << std::endl
              << "  domID=" << domID << " objID=" << objID << " domain=" << domain << " range=" << range
              << " with " << varNo << " variables" << std::endl;
    tcpip::Storage inMsg;
    try {
        std::string acknowledgement;
        check_resultState(inMsg, domID, false, &acknowledgement);
        answerLog << acknowledgement << std::endl;
        validateSubscription(inMsg);
    } catch (tcpip::SocketException& e) {
        answerLog << e.what() << std::endl;
    }
}


// ---------- Report helper
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
TraCITestClient::validateSimulationStep2(tcpip::Storage& inMsg) {
    try {
        int noSubscriptions = inMsg.readInt();
        for (int s = 0; s < noSubscriptions; ++s) {
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
        if (cmdId >= RESPONSE_SUBSCRIBE_INDUCTIONLOOP_VARIABLE && cmdId <= RESPONSE_SUBSCRIBE_GUI_VARIABLE) {
            answerLog << "  CommandID=" << cmdId;
            answerLog << "  ObjectID=" << inMsg.readString();
            int varNo = inMsg.readUnsignedByte();
            answerLog << "  #variables=" << varNo << std::endl;
            for (int i = 0; i < varNo; ++i) {
                answerLog << "      VariableID=" << inMsg.readUnsignedByte();
                bool ok = inMsg.readUnsignedByte() == RTYPE_OK;
                answerLog << "      ok=" << ok;
                int valueDataType = inMsg.readUnsignedByte();
                answerLog << " valueDataType=" << valueDataType;
                readAndReportTypeDependent(inMsg, valueDataType);
            }
        } else if (cmdId >= RESPONSE_SUBSCRIBE_INDUCTIONLOOP_CONTEXT && cmdId <= RESPONSE_SUBSCRIBE_GUI_CONTEXT) {
            answerLog << "  CommandID=" << cmdId;
            answerLog << "  ObjectID=" << inMsg.readString();
            answerLog << "  Domain=" << inMsg.readUnsignedByte();
            int varNo = inMsg.readUnsignedByte();
            answerLog << "  #variables=" << varNo << std::endl;
            int objNo = inMsg.readInt();
            answerLog << "  #objects=" << objNo << std::endl;
            for (int j = 0; j < objNo; ++j) {
                answerLog << "   ObjectID=" << inMsg.readString() << std::endl;
                for (int i = 0; i < varNo; ++i) {
                    answerLog << "      VariableID=" << inMsg.readUnsignedByte();
                    bool ok = inMsg.readUnsignedByte() == RTYPE_OK;
                    answerLog << "      ok=" << ok;
                    int valueDataType = inMsg.readUnsignedByte();
                    answerLog << " valueDataType=" << valueDataType;
                    readAndReportTypeDependent(inMsg, valueDataType);
                }
            }
        } else {
            answerLog << "#Error: received response with command id: " << cmdId << " but expected a subscription response (0xe0-0xef / 0x90-0x9f)" << std::endl;
            return false;
        }
    } catch (std::invalid_argument& e) {
        answerLog << "#Error while reading message:" << e.what() << std::endl;
        return false;
    }
    return true;
}







// ---------- Conversion helper
int
TraCITestClient::setValueTypeDependant(tcpip::Storage& into, std::ifstream& defFile, std::stringstream& msg) {
    std::string dataTypeS;
    defFile >> dataTypeS;
    if (dataTypeS == "<airDist>") {
        into.writeUnsignedByte(REQUEST_AIRDIST);
        return 1;
    } else if (dataTypeS == "<drivingDist>") {
        into.writeUnsignedByte(REQUEST_DRIVINGDIST);
        return 1;
    } else if (dataTypeS == "<objSubscription>") {
        int beginTime, endTime, numVars;
        defFile >> beginTime >> endTime >> numVars;
        into.writeInt(beginTime);
        into.writeInt(endTime);
        into.writeInt(numVars);
        for (int i = 0; i < numVars; ++i) {
            int var;
            defFile >> var;
            into.writeUnsignedByte(var);
        }
        return 4 + 4 + 4 + numVars;
    }
    int valI;
    double valF;
    if (dataTypeS == "<int>") {
        defFile >> valI;
        into.writeUnsignedByte(TYPE_INTEGER);
        into.writeInt(valI);
        return 4 + 1;
    } else if (dataTypeS == "<byte>") {
        defFile >> valI;
        into.writeUnsignedByte(TYPE_BYTE);
        into.writeByte(valI);
        return 1 + 1;
    }  else if (dataTypeS == "<ubyte>") {
        defFile >> valI;
        into.writeUnsignedByte(TYPE_UBYTE);
        into.writeUnsignedByte(valI);
        return 1 + 1;
    } else if (dataTypeS == "<float>") {
        defFile >> valF;
        into.writeUnsignedByte(TYPE_FLOAT);
        into.writeFloat(float(valF));
        return 4 + 1;
    } else if (dataTypeS == "<double>") {
        defFile >> valF;
        into.writeUnsignedByte(TYPE_DOUBLE);
        into.writeDouble(valF);
        return 8 + 1;
    } else if (dataTypeS == "<string>") {
        std::string valueS;
        defFile >> valueS;
        if (valueS == "\"\"") {
            valueS = "";
        }
        into.writeUnsignedByte(TYPE_STRING);
        into.writeString(valueS);
        return 4 + 1 + (int) valueS.length();
    } else if (dataTypeS == "<string*>") {
        std::vector<std::string> slValue;
        defFile >> valI;
        int length = 1 + 4;
        for (int i = 0; i < valI; ++i) {
            std::string tmp;
            defFile >> tmp;
            slValue.push_back(tmp);
            length += 4 + int(tmp.length());
        }
        into.writeUnsignedByte(TYPE_STRINGLIST);
        into.writeStringList(slValue);
        return length;
    } else if (dataTypeS == "<compound>") {
        defFile >> valI;
        into.writeUnsignedByte(TYPE_COMPOUND);
        into.writeInt(valI);
        int length = 1 + 4;
        for (int i = 0; i < valI; ++i) {
            length += setValueTypeDependant(into, defFile, msg);
        }
        return length;
    } else if (dataTypeS == "<color>") {
        defFile >> valI;
        into.writeUnsignedByte(TYPE_COLOR);
        into.writeUnsignedByte(valI);
        for (int i = 0; i < 3; ++i) {
            defFile >> valI;
            into.writeUnsignedByte(valI);
        }
        return 1 + 4;
    } else if (dataTypeS == "<position2D>") {
        defFile >> valF;
        into.writeUnsignedByte(POSITION_2D);
        into.writeDouble(valF);
        defFile >> valF;
        into.writeDouble(valF);
        return 1 + 8 + 8;
    } else if (dataTypeS == "<position3D>") {
        defFile >> valF;
        into.writeUnsignedByte(POSITION_3D);
        into.writeDouble(valF);
        defFile >> valF;
        into.writeDouble(valF);
        defFile >> valF;
        into.writeDouble(valF);
        return 1 + 8 + 8 + 8;
    } else if (dataTypeS == "<positionRoadmap>") {
        std::string valueS;
        defFile >> valueS;
        into.writeUnsignedByte(POSITION_ROADMAP);
        into.writeString(valueS);
        int length = 1 + 8 + (int) valueS.length();
        defFile >> valF;
        into.writeDouble(valF);
        defFile >> valI;
        into.writeUnsignedByte(valI);
        return length + 4 + 1;
    } else if (dataTypeS == "<shape>") {
        defFile >> valI;
        into.writeUnsignedByte(TYPE_POLYGON);
        into.writeUnsignedByte(valI);
        int length = 1 + 1;
        for (int i = 0; i < valI; ++i) {
            double x, y;
            defFile >> x >> y;
            into.writeDouble(x);
            into.writeDouble(y);
            length += 8 + 8;
        }
        return length;
    }
    msg << "## Unknown data type: " << dataTypeS;
    return 0;
}


void
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
        SUMOReal lowerLeftX = inMsg.readDouble();
        SUMOReal lowerLeftY = inMsg.readDouble();
        SUMOReal upperRightX = inMsg.readDouble();
        SUMOReal upperRightY = inMsg.readDouble();
        answerLog << " BoundaryBoxValue: lowerLeft x=" << lowerLeftX
                  << " y=" << lowerLeftY << " upperRight x=" << upperRightX
                  << " y=" << upperRightY << std::endl;
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
                    return;
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
    }
}


void
TraCITestClient::testAPI() {
    answerLog << "testAPI:\n";

    answerLog << "  edge:\n";
    answerLog << "    getIDList: " << joinToString(edge.getIDList(), " ") << "\n";
    answerLog << "    getIDCount: " << edge.getIDCount() << "\n";
    const std::string edgeID = "e_m0";
    edge.adaptTraveltime(edgeID, 42, 0, 10);
    edge.setEffort(edgeID, 420, 0, 10);
    answerLog << "    currentTraveltime: " << edge.getTraveltime(edgeID) << "\n";
    answerLog << "    adaptedTravelTime: " << edge.getAdaptedTraveltime(edgeID, 0) << "\n";
    answerLog << "    effort: " << edge.getEffort(edgeID, 0) << "\n";
    answerLog << "  route:\n";
    answerLog << "    add:\n";
    std::vector<std::string> edges;
    edges.push_back("e_u1");
    route.add("e_u1", edges);
    answerLog << "    getIDList: " << joinToString(route.getIDList(), " ") << "\n";
    answerLog << "  vehicleType:\n";
    answerLog << "    getIDList: " << joinToString(vehicletype.getIDList(), " ") << "\n";
    vehicletype.setWidth("t1", 1.9);
    answerLog << "    getWidth: " << vehicletype.getWidth("t1") << "\n";
    vehicletype.setHeight("t1", 1.8);
    answerLog << "    getHeight: " << vehicletype.getHeight("t1") << "\n";
    answerLog << "  vehicle:\n";
    vehicle.setLine("0", "S42");
    std::vector<std::string> via;
    via.push_back("e_m1");
    vehicle.setVia("0", via);
    answerLog << "    getRoadID: " << vehicle.getRoadID("0") << "\n";
    answerLog << "    getLaneID: " << vehicle.getLaneID("0") << "\n";
    answerLog << "    getSpeedMode: " << vehicle.getSpeedMode("0") << "\n";
    answerLog << "    getSlope: " << vehicle.getSlope("0") << "\n";
    answerLog << "    getLine: " << vehicle.getLine("0") << "\n";
    answerLog << "    getVia: " << joinToString(vehicle.getVia("0"), ",") << "\n";
    TraCIColor col1;
    col1.r = 255;
    col1.g = 255;
    col1.b = 0;
    col1.a = 128;
    vehicle.setColor("0", col1);
    TraCIColor col2 = vehicle.getColor("0");
    answerLog << "    getColor: " << col2.r << "r=" << col2.r << " g=" << col2.g << " b=" << col2.b << " a=" << col2.a << "\n";
    answerLog << "    getNextTLS:\n";
    std::vector<VehicleScope::NextTLSData> result = vehicle.getNextTLS("0");
    for (int i = 0; i < (int)result.size(); ++i) {
        const VehicleScope::NextTLSData& d = result[i];
        answerLog << "      tls=" << d.id << " tlIndex=" << d.tlIndex << " dist=" << d.dist << " state=" << d.state << "\n";
    }
    answerLog << "    moveToXY, simStep:\n";
    vehicle.moveToXY("0", "dummy", 0, 2231.61, 498.29, 90, 1);
    simulationStep();
    answerLog << "    getRoadID: " << vehicle.getRoadID("0") << "\n";
    answerLog << "    getLaneID: " << vehicle.getLaneID("0") << "\n";
    answerLog << "    add:\n";
    vehicle.add("1", "e_u1");
    simulationStep();
    answerLog << "    getIDList: " << joinToString(vehicle.getIDList(), " ") << "\n";
    answerLog << "    getWaitingTime: " << vehicle.getWaitingTime("0") << "\n";
    vehicle.setShapeClass("0", "bicycle");
    answerLog << "    getShapeClass: " << vehicle.getShapeClass("0") << "\n";
    answerLog << "    remove:\n";
    vehicle.remove("0");
    answerLog << "    getIDCount: " << vehicle.getIDCount() << "\n";

    answerLog << "  inductionloop:\n";
    answerLog << "    getIDList: " << joinToString(inductionloop.getIDList(), " ") << "\n";
    answerLog << "    getVehicleData:\n";
    std::vector<InductionLoopScope::VehicleData> result2 = inductionloop.getVehicleData("det1");
    for (int i = 0; i < (int)result2.size(); ++i) {
        const InductionLoopScope::VehicleData& vd = result2[i];
        answerLog << "      veh=" << vd.id << " length=" << vd.length << " entered=" << vd.entryTime << " left=" << vd.leaveTime << " type=" << vd.typeID << "\n";
    }

    answerLog << "  simulation:\n";
    answerLog << "    getCurrentTime: " << simulation.getCurrentTime() << "\n";
    answerLog << "    subscribe to road and pos of vehicle '1':\n";
    std::vector<int> vars;
    vars.push_back(VAR_ROAD_ID);
    vars.push_back(VAR_LANEPOSITION);
    simulation.subscribe(CMD_SUBSCRIBE_VEHICLE_VARIABLE, "1", 0, TIME2STEPS(100), vars);
    simulationStep();
    answerLog << "    subscription results:\n";
    TraCIValues result3 = simulation.getSubscriptionResults("1");
    answerLog << "      roadID=" << result3[VAR_ROAD_ID].string << " pos=" << result3[VAR_LANEPOSITION].scalar << "\n";

    answerLog << "    subscribe to vehicles around edge 'e_u1':\n";
    std::vector<int> vars2;
    vars2.push_back(VAR_LANEPOSITION);
    simulation.subscribeContext(CMD_SUBSCRIBE_EDGE_CONTEXT, "e_u1", 0, TIME2STEPS(100), CMD_GET_VEHICLE_VARIABLE, 100, vars2);
    simulationStep();
    answerLog << "    context subscription results:\n";
    SubscribedValues result4 = simulation.getContextSubscriptionResults("e_u1");
    for (SubscribedValues::iterator it = result4.begin(); it != result4.end(); ++it) {
        answerLog << "      vehicle=" << it->first << " pos=" << it->second[VAR_LANEPOSITION].scalar << "\n";
    }

    answerLog << "  person:\n";
    person.setWidth("p0", 1);
    person.setMinGap("p0", 2);
    person.setLength("p0", 3);
    person.setHeight("p0", 4);
    person.setColor("p0", col1);
    person.setType("p0", "stilts");
    answerLog << "    getIDList: " << joinToString(person.getIDList(), " ") << "\n";
    answerLog << "    getRoadID: " << person.getRoadID("p0") << "\n";
    answerLog << "    getTypeID: " << person.getTypeID("p0") << "\n";
    answerLog << "    getWaitingTime: " << person.getWaitingTime("p0") << "\n";
    answerLog << "    getNextEdge: " << person.getNextEdge("p0") << "\n";
    answerLog << "    getStage: " << person.getStage("p0") << "\n";
    answerLog << "    getRemainingStages: " << person.getRemainingStages("p0") << "\n";
    answerLog << "    getVehicle: " << person.getVehicle("p0") << "\n";
    answerLog << "    getEdges: " << joinToString(person.getEdges("p0"), " ") << "\n";
    person.setSpeed("p0", 3);
    simulationStep();
    answerLog << "    getSpeed: " << person.getSpeed("p0") << "\n";
    person.add("p1", "e_u1", 10);
    std::vector<std::string> walkEdges;
    walkEdges.push_back("e_u1");
    walkEdges.push_back("e_shape1");
    person.appendWalkingStage("p1", walkEdges, -20);
    person.appendWaitingStage("p1", 5);
    person.appendDrivingStage("p1", "e_vu2", "BusLine42");
    // expect 4 stages due to the initial waiting-for-departure stage
    answerLog << "    getRemainingStages: " << person.getRemainingStages("p1") << "\n";
    person.removeStage("p1", 3);
    answerLog << "    getRemainingStages: " << person.getRemainingStages("p1") << "\n";
    person.removeStages("p1");
    answerLog << "    getRemainingStages: " << person.getRemainingStages("p1") << "\n";
    answerLog << "    getStage: " << person.getStage("p1") << "\n";

    answerLog << "  gui:\n";
    try {
        answerLog << "    setScheme: \n";
        gui.setSchema("View #0", "real world");
        answerLog << "    getScheme: " << gui.getSchema("View #0") << "\n";
        answerLog << "    take screenshot: \n";
        gui.screenshot("View #0", "image.png");
    } catch (tcpip::SocketException&) {
        answerLog << "    no support for gui commands\n";
    }
}
