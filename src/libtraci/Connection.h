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
/// @file    Connection.h
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Michael Behrisch
/// @date    30.05.2012
///
// C++ TraCI client API implementation
/****************************************************************************/
#pragma once
#include <config.h>

#include <vector>
#include <map>
#include <limits>
#include <string>
#include <sstream>
#include <iomanip>
#include <thread>
#include <mutex>
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

    const std::string& getLabel() const {
        return myLabel;
    }

    std::mutex& getMutex() const {
        return myMutex;
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
    void setOrder(int order);

    /** @brief Sends a GetVariable / SetVariable request if mySocket is connected.
     * Otherwise writes to myOutput only.
     * @param[in] cmdID The command and domain of the variable
     * @param[in] varID The variable to retrieve
     * @param[in] objID The object to retrieve the variable from
     * @param[in] add Optional additional parameter
     */
    void createCommand(int cmdID, int varID, const std::string* const objID, tcpip::Storage* add = nullptr) const;


    /** @brief Sends a SubscribeContext or a SubscribeVariable request
     * @param[in] domID The domain of the variable
     * @param[in] objID The object to subscribe the variables from
     * @param[in] beginTime The begin time step of subscriptions
     * @param[in] endTime The end time step of subscriptions
     * @param[in] domain The domain of the objects which values shall be returned (-1 means variable subscription)
     * @param[in] range The range around the obj to investigate (only meaningful for context subscription)
     * @param[in] vars The variables to subscribe
     * @param[in] params map of variable ids to parameters if needed
     */
    void subscribe(int domID, const std::string& objID, double beginTime, double endTime,
                   int domain, double range, const std::vector<int>& vars, const libsumo::TraCIResults& params);
    /// @}


    tcpip::Storage& doCommand(int command, int var = -1, const std::string& id = "", tcpip::Storage* add = nullptr, int expectedType = -1);
    void addFilter(int var, tcpip::Storage* add = nullptr);

    void readVariableSubscription(int responseID, tcpip::Storage& inMsg);
    void readContextSubscription(int responseID, tcpip::Storage& inMsg);
    void readVariables(tcpip::Storage& inMsg, const std::string& objectID, int variableCount, libsumo::SubscriptionResults& into);

private:
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

    template <class T>
    static inline std::string toString(const T& t, std::streamsize accuracy = PRECISION) {
        std::ostringstream oss;
        oss.setf(std::ios::fixed, std::ios::floatfield);
        oss << std::setprecision(accuracy);
        oss << t;
        return oss.str();
    }

    template<typename T>
    inline std::string toHex(const T i, std::streamsize numDigits = 2) {
        // inspired by http://stackoverflow.com/questions/5100718/int-to-hex-string-in-c
        std::stringstream stream;
        stream << "0x" << std::setfill('0') << std::setw(numDigits == 0 ? sizeof(T) * 2 : numDigits) << std::hex << i;
        return stream.str();
    }

    void readOutput();

    /** @brief Constructor, connects to the specified SUMO server
     * @param[in] host The name of the host to connect to
     * @param[in] port The port to connect to
     * @exception tcpip::SocketException if the connection fails
     */
    Connection(const std::string& host, int port, int numRetries, const std::string& label, FILE* const pipe);

private:
    const std::string myLabel;
    FILE* const myProcessPipe;
    std::thread* myProcessReader;
    /// @brief The socket
    tcpip::Socket mySocket;
    /// @brief The reusable output storage
    mutable tcpip::Storage myOutput;
    /// @brief The reusable input storage
    mutable tcpip::Storage myInput;

    mutable std::mutex myMutex;

    std::map<int, libsumo::SubscriptionResults> mySubscriptionResults;
    std::map<int, libsumo::ContextSubscriptionResults> myContextSubscriptionResults;

    static Connection* myActive;
    static std::map<const std::string, Connection*> myConnections;

private:
    /// @brief Invalidated assignment operator.
    Connection& operator=(const Connection&);

};

}
