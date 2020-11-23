/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2020 German Aerospace Center (DLR) and others.
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
/// @file    Connection.h
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Michael Behrisch
/// @date    30.05.2012
///
// C++ TraCI client API implementation
/****************************************************************************/
#pragma once
#include <vector>
#include <map>
#include <limits>
#include <string>
#include <sstream>
#include <iomanip>
#include <foreign/tcpip/socket.h>
#include <libsumo/Subscription.h>


// ===========================================================================
// global definitions
// ===========================================================================
#define PRECISION 2


// ===========================================================================
// class definitions
// ===========================================================================
namespace libtraci {
/**
 * @class Connection
 * @brief C++ TraCI client API implementation
 */
class Connection {
public:
    static void connect(const std::string& host, int port, int numRetries, const std::string& label, FILE* const pipe) {
        myConnections[label] = new Connection(host, port, numRetries, label, pipe);
    }

    static Connection& getActive() {
        return *myActive;
    }

    static bool isActive() {
        return myActive != nullptr;
    }

    static void switchCon(const std::string& label) {
        myActive = myConnections.find(label)->second;
    }

    const std::string& getLabel() {
        return myLabel;
    }

    /// @brief ends the simulation and closes the connection
    void close();

    libsumo::SubscriptionResults getAllSubscriptionResults(const int domain) {
        return mySubscriptionResults[domain];
    }

    libsumo::ContextSubscriptionResults& getAllContextSubscriptionResults(const int domain) {
        return myContextSubscriptionResults[domain];
    }

    /// @name Command sending methods
    /// @{

    /** @brief Sends a SimulationStep command
     */
    void simulationStep(double time);


    /** @brief Sends a SetOrder command
     */
    void send_commandSetOrder(int order);

    /** @brief Sends a GetVariable / SetVariable request if mySocket is connected.
     * Otherwise writes to myOutput only.
     * @param[in] cmdID The command and domain of the variable
     * @param[in] varID The variable to retrieve
     * @param[in] objID The object to retrieve the variable from
     * @param[in] add Optional additional parameter
     */
    void createCommand(int cmdID, int varID, const std::string& objID, tcpip::Storage* add = nullptr) const;
    void createFilterCommand(int cmdID, int varID, tcpip::Storage* add = nullptr) const;


    /** @brief Sends a SubscribeVariable request
     * @param[in] domID The domain of the variable
     * @param[in] objID The object to subscribe the variables from
     * @param[in] beginTime The begin time step of subscriptions
     * @param[in] endTime The end time step of subscriptions
     * @param[in] vars The variables to subscribe
     */
    void subscribeObjectVariable(int domID, const std::string& objID, double beginTime, double endTime, const std::vector<int>& vars, const libsumo::TraCIResults& params);


    /** @brief Sends a SubscribeContext request
     * @param[in] domID The domain of the variable
     * @param[in] objID The object to subscribe the variables from
     * @param[in] beginTime The begin time step of subscriptions
     * @param[in] endTime The end time step of subscriptions
     * @param[in] domain The domain of the objects which values shall be returned
     * @param[in] range The range around the obj to investigate
     * @param[in] vars The variables to subscribe
     */
    void subscribeObjectContext(int domID, const std::string& objID, double beginTime, double endTime,
                                int domain, double range, const std::vector<int>& vars, const libsumo::TraCIResults& params);
    /// @}


    /// @name Command sending methods
    /// @{

    /** @brief Validates the result state of a command
     * @param[in] inMsg The buffer to read the message from
     * @param[in] command The original command id
     * @param[in] ignoreCommandId Whether the returning command id shall be validated
     * @param[in] acknowledgement Pointer to an existing string into which the acknowledgement message shall be inserted
     */
    void check_resultState(tcpip::Storage& inMsg, int command, bool ignoreCommandId = false, std::string* acknowledgement = 0);

    /** @brief Validates the result state of a command
     * @return The command Id
     */
    int check_commandGetResult(tcpip::Storage& inMsg, int command, int expectedType = -1, bool ignoreCommandId = false) const;

    bool processGet(int command, int expectedType, bool ignoreCommandId = false);
    /// @}

    int getUnsignedByte(int command, int var, const std::string& id, tcpip::Storage* add = nullptr) {
        createCommand(command, var, id, add);
        if (processGet(command, libsumo::TYPE_UBYTE)) {
            return myInput.readUnsignedByte();
        }
        return libsumo::INVALID_INT_VALUE;
    }
    int getByte(int command, int var, const std::string& id, tcpip::Storage* add = nullptr) {
        createCommand(command, var, id, add);
        if (processGet(command, libsumo::TYPE_BYTE)) {
            return myInput.readByte();
        }
        return libsumo::INVALID_INT_VALUE;
    }
    int getInt(int command, int var, const std::string& id, tcpip::Storage* add = nullptr) {
        createCommand(command, var, id, add);
        if (processGet(command, libsumo::TYPE_INTEGER)) {
            return myInput.readInt();
        }
        return libsumo::INVALID_INT_VALUE;
    }
    double getDouble(int command, int var, const std::string& id, tcpip::Storage* add = nullptr) {
        createCommand(command, var, id, add);
        if (processGet(command, libsumo::TYPE_DOUBLE)) {
            return myInput.readDouble();
        }
        return libsumo::INVALID_DOUBLE_VALUE;
    }
    libsumo::TraCIPositionVector getPolygon(int command, int var, const std::string& id, tcpip::Storage* add = nullptr) {
        libsumo::TraCIPositionVector ret;
        createCommand(command, var, id, add);
        if (processGet(command, libsumo::TYPE_POLYGON)) {
            int size = myInput.readUnsignedByte();
            if (size == 0) {
                size = myInput.readInt();
            }
            for (int i = 0; i < size; ++i) {
                libsumo::TraCIPosition p;
                p.x = myInput.readDouble();
                p.y = myInput.readDouble();
                p.z = 0.;
                ret.push_back(p);
            }
        }
        return ret;
    }
    libsumo::TraCIPosition getPos(int command, int var, const std::string& id, tcpip::Storage* add = nullptr) {
        libsumo::TraCIPosition p;
        createCommand(command, var, id, add);
        if (processGet(command, libsumo::POSITION_2D)) {
            p.x = myInput.readDouble();
            p.y = myInput.readDouble();
        }
        return p;
    }

    libsumo::TraCIPosition getPos3D(int command, int var, const std::string& id, tcpip::Storage* add = nullptr) {
        libsumo::TraCIPosition p;
        createCommand(command, var, id, add);
        if (processGet(command, libsumo::POSITION_3D)) {
            p.x = myInput.readDouble();
            p.y = myInput.readDouble();
            p.z = myInput.readDouble();
        }
        return p;
    }
    std::string getString(int command, int var, const std::string& id, tcpip::Storage* add = nullptr) {
        createCommand(command, var, id, add);
        if (processGet(command, libsumo::TYPE_STRING)) {
            return myInput.readString();
        }
        return "";
    }
    std::vector<std::string> getStringVector(int command, int var, const std::string& id, tcpip::Storage* add = nullptr) {
        std::vector<std::string> r;
        createCommand(command, var, id, add);
        if (processGet(command, libsumo::TYPE_STRINGLIST)) {
            const int size = myInput.readInt();
            for (int i = 0; i < size; ++i) {
                r.push_back(myInput.readString());
            }
        }
        return r;
    }

    libsumo::TraCIColor getCol(int command, int var, const std::string& id, tcpip::Storage* add = nullptr) {
        libsumo::TraCIColor c;
        createCommand(command, var, id, add);
        if (processGet(command, libsumo::TYPE_COLOR)) {
            c.r = (unsigned char)myInput.readUnsignedByte();
            c.g = (unsigned char)myInput.readUnsignedByte();
            c.b = (unsigned char)myInput.readUnsignedByte();
            c.a = (unsigned char)myInput.readUnsignedByte();
        }
        return c;
    }

    libsumo::TraCIStage getTraCIStage(int command, int var, const std::string& id, tcpip::Storage* add = nullptr) {
        libsumo::TraCIStage s;
        createCommand(command, var, id, add);
        if (processGet(command, libsumo::TYPE_COMPOUND)) {
            myInput.readInt(); // components
            myInput.readUnsignedByte();
            s.type = myInput.readInt();

            myInput.readUnsignedByte();
            s.vType = myInput.readString();

            myInput.readUnsignedByte();
            s.line = myInput.readString();

            myInput.readUnsignedByte();
            s.destStop = myInput.readString();

            myInput.readUnsignedByte();
            s.edges = myInput.readStringList();

            myInput.readUnsignedByte();
            s.travelTime = myInput.readDouble();

            myInput.readUnsignedByte();
            s.cost = myInput.readDouble();

            myInput.readUnsignedByte();
            s.length = myInput.readDouble();

            myInput.readUnsignedByte();
            s.intended = myInput.readString();

            myInput.readUnsignedByte();
            s.depart = myInput.readDouble();

            myInput.readUnsignedByte();
            s.departPos = myInput.readDouble();

            myInput.readUnsignedByte();
            s.arrivalPos = myInput.readDouble();

            myInput.readUnsignedByte();
            s.description = myInput.readString();
        }
        return s;
    }

    tcpip::Storage& doCommand(int command, int var, const std::string& id, tcpip::Storage* add = nullptr);

    void setInt(int command, int var, const std::string& id, int value) {
        tcpip::Storage content;
        content.writeUnsignedByte(libsumo::TYPE_INTEGER);
        content.writeInt(value);
        doCommand(command, var, id, &content);
    }
    void setDouble(int command, int var, const std::string& id, double value) {
        tcpip::Storage content;
        content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
        content.writeDouble(value);
        doCommand(command, var, id, &content);
    }

    void setString(int command, int var, const std::string& id, const std::string& value) {
        tcpip::Storage content;
        content.writeUnsignedByte(libsumo::TYPE_STRING);
        content.writeString(value);
        doCommand(command, var, id, &content);
    }

    void setStringVector(int command, int var, const std::string& id, const std::vector<std::string>& value) {
        tcpip::Storage content;
        content.writeUnsignedByte(libsumo::TYPE_STRINGLIST);
        content.writeStringList(value);
        doCommand(command, var, id, &content);
    }

    void setCol(int command, int var, const std::string& id, const libsumo::TraCIColor c) {
        tcpip::Storage content;
        content.writeUnsignedByte(libsumo::TYPE_COLOR);
        content.writeUnsignedByte(c.r);
        content.writeUnsignedByte(c.g);
        content.writeUnsignedByte(c.b);
        content.writeUnsignedByte(c.a);
        doCommand(command, var, id, &content);
    }

    void readVariableSubscription(int responseID, tcpip::Storage& inMsg);
    void readContextSubscription(int responseID, tcpip::Storage& inMsg);
    void readVariables(tcpip::Storage& inMsg, const std::string& objectID, int variableCount, libsumo::SubscriptionResults& into);

private:
    template <class T>
    static inline std::string toString(const T& t, std::streamsize accuracy = PRECISION) {
        std::ostringstream oss;
        oss.setf(std::ios::fixed, std::ios::floatfield);
        oss << std::setprecision(accuracy);
        oss << t;
        return oss.str();
    }

    /** @brief Constructor, connects to the specified SUMO server
     * @param[in] host The name of the host to connect to
     * @param[in] port The port to connect to
     * @exception tcpip::SocketException if the connection fails
     */
    Connection(const std::string& host, int port, int numRetries, const std::string& label, FILE* const pipe);

private:
    const std::string myLabel;
    FILE* const myProcessPipe;
    /// @brief The socket
    tcpip::Socket mySocket;
    /// @brief The reusable output storage
    mutable tcpip::Storage myOutput;
    /// @brief The reusable input storage
    mutable tcpip::Storage myInput;

    std::map<int, libsumo::SubscriptionResults> mySubscriptionResults;
    std::map<int, libsumo::ContextSubscriptionResults> myContextSubscriptionResults;

    static Connection* myActive;
    static std::map<const std::string, Connection*> myConnections;

private:
    /// @brief Invalidated assignment operator.
    Connection& operator=(const Connection&);

};

}
