/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    Connection.cpp
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    30.05.2012
///
// C++ TraCI client API implementation
/****************************************************************************/
#include <config.h>

#include <thread>
#include <chrono>
#include <array>
#include <libsumo/StorageHelper.h>
#include <libsumo/TraCIDefs.h>
#include "Connection.h"


namespace libtraci {
// ===========================================================================
// static member initializations
// ===========================================================================
Connection* Connection::myActive = nullptr;
std::map<const std::string, Connection*> Connection::myConnections;


// ===========================================================================
// member method definitions
// ===========================================================================
Connection::Connection(const std::string& host, int port, int numRetries, const std::string& label, FILE* const pipe) :
    myLabel(label), myProcessPipe(pipe), myProcessReader(nullptr), mySocket(host, port) {
    if (pipe != nullptr) {
        myProcessReader = new std::thread(&Connection::readOutput, this);
    }
    for (int i = 0; i <= numRetries; i++) {
        try {
            mySocket.connect();
            break;
        } catch (tcpip::SocketException& e) {
            if (i == numRetries) {
                close();
                throw;
            }
            std::cout << "Could not connect to TraCI server at " << host << ":" << port << " " << e.what() << std::endl;
            std::cout << " Retrying in 1 second" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}


void
Connection::readOutput() {
    std::array<char, 256> buffer;
    bool errout = false;
    while (fgets(buffer.data(), (int)buffer.size(), myProcessPipe) != nullptr) {
        std::stringstream result;
        result << buffer.data();
        std::string line;
        while (std::getline(result, line)) {
            if ((errout && (line.empty() || line[0] == ' ')) || line.compare(0, 6, "Error:") == 0 || line.compare(0, 8, "Warning:") == 0) {
                std::cerr << line << std::endl;
                errout = true;
            } else {
                std::cout << line << std::endl;
                errout = false;
            }
        }
    }
}


void
Connection::close() {
    if (mySocket.has_client_connection()) {
        std::unique_lock<std::mutex> lock{ myMutex };
        tcpip::Storage outMsg;
        // command length
        outMsg.writeUnsignedByte(1 + 1);
        // command id
        outMsg.writeUnsignedByte(libsumo::CMD_CLOSE);
        mySocket.sendExact(outMsg);

        tcpip::Storage inMsg;
        std::string acknowledgement;
        check_resultState(inMsg, libsumo::CMD_CLOSE, false, &acknowledgement);
        mySocket.close();
    }
    if (myProcessReader != nullptr) {
        myProcessReader->join();
        delete myProcessReader;
        myProcessReader = nullptr;
#ifdef WIN32
        _pclose(myProcessPipe);
#else
        pclose(myProcessPipe);
#endif
    }
    myConnections.erase(myLabel);
    delete myActive;
    myActive = nullptr;
}


void
Connection::simulationStep(double time) {
    std::unique_lock<std::mutex> lock{myMutex};
    tcpip::Storage outMsg;
    // command length
    outMsg.writeUnsignedByte(1 + 1 + 8);
    // command id
    outMsg.writeUnsignedByte(libsumo::CMD_SIMSTEP);
    outMsg.writeDouble(time);
    // send request message
    mySocket.sendExact(outMsg);

    tcpip::Storage inMsg;
    check_resultState(inMsg, libsumo::CMD_SIMSTEP);
    mySubscriptionResults.clear();
    myContextSubscriptionResults.clear();
    int numSubs = inMsg.readInt();
    while (numSubs-- > 0) {
        const int responseID = check_commandGetResult(inMsg, 0, -1, true);
        if ((responseID >= libsumo::RESPONSE_SUBSCRIBE_INDUCTIONLOOP_VARIABLE && responseID <= libsumo::RESPONSE_SUBSCRIBE_BUSSTOP_VARIABLE) ||
                (responseID >= libsumo::RESPONSE_SUBSCRIBE_PARKINGAREA_VARIABLE && responseID <= libsumo::RESPONSE_SUBSCRIBE_OVERHEADWIRE_VARIABLE)) {
            readVariableSubscription(responseID, inMsg);
        } else {
            readContextSubscription(responseID, inMsg);
        }
    }
}


void
Connection::setOrder(int order) {
    std::unique_lock<std::mutex> lock{ myMutex };
    tcpip::Storage outMsg;
    // command length
    outMsg.writeUnsignedByte(1 + 1 + 4);
    // command id
    outMsg.writeUnsignedByte(libsumo::CMD_SETORDER);
    // client index
    outMsg.writeInt(order);
    mySocket.sendExact(outMsg);

    tcpip::Storage inMsg;
    check_resultState(inMsg, libsumo::CMD_SETORDER);
}


void
Connection::createCommand(int cmdID, int varID, const std::string* const objID, tcpip::Storage* add) const {
    if (!mySocket.has_client_connection()) {
        throw libsumo::FatalTraCIError("Not connected.");
    }
    myOutput.reset();
    // command length
    int length = 1 + 1;
    if (varID >= 0) {
        length += 1;
        if (objID != nullptr) {
            length += 4 + (int)objID->length();
        }
    }
    if (add != nullptr) {
        length += (int)add->size();
    }
    if (length <= 255) {
        myOutput.writeUnsignedByte(length);
    } else {
        myOutput.writeUnsignedByte(0);
        myOutput.writeInt(length + 4);
    }
    myOutput.writeUnsignedByte(cmdID);
    if (varID >= 0) {
        myOutput.writeUnsignedByte(varID);
        if (objID != nullptr) {
            myOutput.writeString(*objID);
        }
    }
    // additional values
    if (add != nullptr) {
        myOutput.writeStorage(*add);
    }
}


void
Connection::subscribe(int domID, const std::string& objID, double beginTime, double endTime,
                      int domain, double range, const std::vector<int>& vars, const libsumo::TraCIResults& params) {
    if (!mySocket.has_client_connection()) {
        throw tcpip::SocketException("Socket is not initialised");
    }
    const bool isContext = domain != -1;
    tcpip::Storage outMsg;
    outMsg.writeUnsignedByte(domID); // command id
    outMsg.writeDouble(beginTime);
    outMsg.writeDouble(endTime);
    outMsg.writeString(objID);
    if (isContext) {
        outMsg.writeUnsignedByte(domain);
        outMsg.writeDouble(range);
    }
    if (vars.size() == 1 && vars.front() == -1) {
        if (domID == libsumo::CMD_SUBSCRIBE_VEHICLE_VARIABLE && !isContext) {
            // default for vehicles is edge id and lane position
            outMsg.writeUnsignedByte(2);
            outMsg.writeUnsignedByte(libsumo::VAR_ROAD_ID);
            outMsg.writeUnsignedByte(libsumo::VAR_LANEPOSITION);
        } else {
            // default for detectors is vehicle number, for all others (and contexts) id list
            outMsg.writeUnsignedByte(1);
            const bool isDetector = domID == libsumo::CMD_SUBSCRIBE_INDUCTIONLOOP_VARIABLE
                                    || domID == libsumo::CMD_SUBSCRIBE_LANEAREA_VARIABLE
                                    || domID == libsumo::CMD_SUBSCRIBE_MULTIENTRYEXIT_VARIABLE
                                    || domID == libsumo::CMD_SUBSCRIBE_LANE_VARIABLE
                                    || domID == libsumo::CMD_SUBSCRIBE_EDGE_VARIABLE;
            outMsg.writeUnsignedByte(isDetector ? libsumo::LAST_STEP_VEHICLE_NUMBER : libsumo::TRACI_ID_LIST);
        }
    } else {
        outMsg.writeUnsignedByte((int)vars.size());
        for (const int v : vars) {
            outMsg.writeUnsignedByte(v);
            const auto& paramEntry = params.find(v);
            if (paramEntry != params.end()) {
                outMsg.writeStorage(*libsumo::StorageHelper::toStorage(*paramEntry->second));
            }
        }
    }
    tcpip::Storage complete;
    complete.writeUnsignedByte(0);
    complete.writeInt(5 + (int)outMsg.size());
    complete.writeStorage(outMsg);
    std::unique_lock<std::mutex> lock{ myMutex };
    // send message
    mySocket.sendExact(complete);

    tcpip::Storage inMsg;
    check_resultState(inMsg, domID);
    if (!vars.empty()) {
        const int responseID = check_commandGetResult(inMsg, domID);
        if (isContext) {
            readContextSubscription(responseID, inMsg);
        } else {
            readVariableSubscription(responseID, inMsg);
        }
    }
}


void
Connection::check_resultState(tcpip::Storage& inMsg, int command, bool ignoreCommandId, std::string* acknowledgement) {
    mySocket.receiveExact(inMsg);
    int cmdLength;
    int cmdId;
    int resultType;
    int cmdStart;
    std::string msg;
    try {
        cmdStart = inMsg.position();
        cmdLength = inMsg.readUnsignedByte();
        cmdId = inMsg.readUnsignedByte();
        resultType = inMsg.readUnsignedByte();
        msg = inMsg.readString();
    } catch (std::invalid_argument&) {
        throw libsumo::TraCIException("#Error: an exception was thrown while reading result state message");
    }
    switch (resultType) {
        case libsumo::RTYPE_ERR:
            throw libsumo::TraCIException(msg);
        case libsumo::RTYPE_NOTIMPLEMENTED:
            throw libsumo::TraCIException(".. Sent command is not implemented (" + toHex(command) + "), [description: " + msg + "]");
        case libsumo::RTYPE_OK:
            if (acknowledgement != nullptr) {
                (*acknowledgement) = ".. Command acknowledged (" + toHex(command) + "), [description: " + msg + "]";
            }
            break;
        default:
            throw libsumo::TraCIException(".. Answered with unknown result code(" + toHex(resultType) + ") to command(" + toHex(command) + "), [description: " + msg + "]");
    }
    if (command != cmdId && !ignoreCommandId) {
        throw libsumo::TraCIException("#Error: received status response to command: " + toHex(cmdId) + " but expected: " + toHex(command));
    }
    if ((cmdStart + cmdLength) != (int) inMsg.position()) {
        throw libsumo::TraCIException("#Error: command at position " + toHex(cmdStart) + " has wrong length");
    }
}


int
Connection::check_commandGetResult(tcpip::Storage& inMsg, int command, int expectedType, bool ignoreCommandId) const {
    int length = inMsg.readUnsignedByte();
    if (length == 0) {
        length = inMsg.readInt();
    }
    int cmdId = inMsg.readUnsignedByte();
    if (!ignoreCommandId && cmdId != (command + 0x10)) {
        throw libsumo::TraCIException("#Error: received response with command id: " + toString(cmdId) + "but expected: " + toString(command + 0x10));
    }
    if (expectedType >= 0) {
        // not called from the TraCITestClient but from within the Connection
        inMsg.readUnsignedByte(); // variableID
        inMsg.readString(); // objectID
        int valueDataType = inMsg.readUnsignedByte();
        if (valueDataType != expectedType) {
            throw libsumo::TraCIException("Expected " + toString(expectedType) + " but got " + toString(valueDataType));
        }
    }
    return cmdId;
}


tcpip::Storage&
Connection::doCommand(int command, int var, const std::string& id, tcpip::Storage* add, int expectedType) {
    createCommand(command, var, &id, add);
    mySocket.sendExact(myOutput);
    myInput.reset();
    check_resultState(myInput, command);
    if (expectedType >= 0) {
        check_commandGetResult(myInput, command, expectedType);
    }
    return myInput;
}


void
Connection::addFilter(int var, tcpip::Storage* add) {
    std::unique_lock<std::mutex> lock{ myMutex };
    createCommand(libsumo::CMD_ADD_SUBSCRIPTION_FILTER, var, nullptr, add);
    mySocket.sendExact(myOutput);
    myInput.reset();
    check_resultState(myInput, libsumo::CMD_ADD_SUBSCRIPTION_FILTER);
}


void
Connection::readVariables(tcpip::Storage& inMsg, const std::string& objectID, int variableCount, libsumo::SubscriptionResults& into) {
    while (variableCount > 0) {

        const int variableID = inMsg.readUnsignedByte();
        const int status = inMsg.readUnsignedByte();
        const int type = inMsg.readUnsignedByte();

        if (status == libsumo::RTYPE_OK) {
            switch (type) {
                case libsumo::TYPE_DOUBLE:
                    into[objectID][variableID] = std::make_shared<libsumo::TraCIDouble>(inMsg.readDouble());
                    break;
                case libsumo::TYPE_STRING:
                    into[objectID][variableID] = std::make_shared<libsumo::TraCIString>(inMsg.readString());
                    break;
                case libsumo::POSITION_2D: {
                    auto p = std::make_shared<libsumo::TraCIPosition>();
                    p->x = inMsg.readDouble();
                    p->y = inMsg.readDouble();
                    into[objectID][variableID] = p;
                    break;
                }
                case libsumo::POSITION_3D: {
                    auto p = std::make_shared<libsumo::TraCIPosition>();
                    p->x = inMsg.readDouble();
                    p->y = inMsg.readDouble();
                    p->z = inMsg.readDouble();
                    into[objectID][variableID] = p;
                    break;
                }
                case libsumo::TYPE_COLOR: {
                    auto c = std::make_shared<libsumo::TraCIColor>();
                    c->r = (unsigned char)inMsg.readUnsignedByte();
                    c->g = (unsigned char)inMsg.readUnsignedByte();
                    c->b = (unsigned char)inMsg.readUnsignedByte();
                    c->a = (unsigned char)inMsg.readUnsignedByte();
                    into[objectID][variableID] = c;
                    break;
                }
                case libsumo::TYPE_INTEGER:
                    into[objectID][variableID] = std::make_shared<libsumo::TraCIInt>(inMsg.readInt());
                    break;
                case libsumo::TYPE_STRINGLIST: {
                    auto sl = std::make_shared<libsumo::TraCIStringList>();
                    int n = inMsg.readInt();
                    for (int i = 0; i < n; ++i) {
                        sl->value.push_back(inMsg.readString());
                    }
                    into[objectID][variableID] = sl;
                }
                break;
                case libsumo::TYPE_COMPOUND: {
                    int n = inMsg.readInt();
                    if (n == 2) {
                        inMsg.readUnsignedByte();
                        const std::string s = inMsg.readString();
                        const int secondType = inMsg.readUnsignedByte();
                        if (secondType == libsumo::TYPE_DOUBLE) {
                            auto r = std::make_shared<libsumo::TraCIRoadPosition>();
                            r->edgeID = s;
                            r->pos = inMsg.readDouble();
                            into[objectID][variableID] = r;
                        } else if (secondType == libsumo::TYPE_STRING) {
                            auto sl = std::make_shared<libsumo::TraCIStringList>();
                            sl->value.push_back(s);
                            sl->value.push_back(inMsg.readString());
                            into[objectID][variableID] = sl;
                        }
                    }
                }
                break;

                // TODO Other data types

                default:
                    throw libsumo::TraCIException("Unimplemented subscription type: " + toString(type));
            }
        } else {
            throw libsumo::TraCIException("Subscription response error: variableID=" + toString(variableID) + " status=" + toString(status));
        }

        variableCount--;
    }
}


void
Connection::readVariableSubscription(int responseID, tcpip::Storage& inMsg) {
    const std::string objectID = inMsg.readString();
    const int variableCount = inMsg.readUnsignedByte();
    readVariables(inMsg, objectID, variableCount, mySubscriptionResults[responseID]);
}


void
Connection::readContextSubscription(int responseID, tcpip::Storage& inMsg) {
    const std::string contextID = inMsg.readString();
    inMsg.readUnsignedByte(); // context domain
    const int variableCount = inMsg.readUnsignedByte();
    int numObjects = inMsg.readInt();
    // the following also instantiates the empty map to get comparable results with libsumo
    // see also https://github.com/eclipse/sumo/issues/7288
    libsumo::SubscriptionResults& results = myContextSubscriptionResults[responseID][contextID];
    while (numObjects-- > 0) {
        std::string objectID = inMsg.readString();
        readVariables(inMsg, objectID, variableCount, results);
    }
}


}


/****************************************************************************/
