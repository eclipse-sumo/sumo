/****************************************************************************/
/// @file    TraCITestClient.h
/// @author  Friedemann Wesner
/// @author  Daniel Krajzewicz
/// @author  Axel Wegener
/// @author  Michael Behrisch
/// @date    2008/04/07
/// @version $Id$
///
/// A test execution class
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef TRACITESTCLIENT_H
#define TRACITESTCLIENT_H

// ===========================================================================
// included modules
// ===========================================================================
#include <string>
#include <sstream>
#include <vector>

#include <foreign/tcpip/socket.h>
#include <utils/common/SUMOTime.h>
#include <utils/traci/TraCIAPI.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TraCITestClient
 * @brief A test execution class
 *
 * Reads a program file and executes the actions stored within it
 */
class TraCITestClient : public TraCIAPI {
public:
    /** @brief Constructor
     * @param[in] outputFileName The name of the file the outputs will be written into
     */
    TraCITestClient(std::string outputFileName = "testclient_result.out");


    /// @brief Destructor
    ~TraCITestClient();


    /** @brief Runs a test
     * @param[in] fileName The name of the file containing the test script
     * @param[in] port The server port to connect to
     * @param[in] host The server name to connect to
     */
    bool run(std::string fileName, int port, std::string host = "localhost");


protected:
    /// @name Commands handling
    /// @{

    /** @brief Sends and validates a simulation step command
     * @param[in] time The time step to send
     */
    void commandSimulationStep(SUMOTime time);


    /** @brief Sends and validates a Close command
     */
    void commandClose();


    /** @brief Sends and validates a GetVariable command
     * @param[in] domID The ID of the domain the addressed object belongs to
     * @param[in] varID The ID of the variable one asks for
     * @param[in] objID The ID of the object a variable shall be retrieved from
     * @param[in] addData Storage to read additional data from, if needed
     */
    void commandGetVariable(int domID, int varID, const std::string& objID, tcpip::Storage* addData = 0);


    /** @brief Sends and validates a SetVariable command
     * @param[in] domID The ID of the domain the addressed object belongs to
     * @param[in] varID The ID of the variable to set
     * @param[in] objID The ID of the object which shall be changed
     * @param[in] defFile Storage to read additional data from
     */
    void commandSetValue(int domID, int varID, const std::string& objID, std::ifstream& defFile);


    /** @brief Sends and validates a SubscribeVariable command
     * @param[in] domID The ID of the domain the addressed object belongs to
     * @param[in] objID The ID of the object a variable shall be subscribed from
     * @param[in] beginTime The time the subscription shall begin at
     * @param[in] endTime The time the subscription shall end at
     * @param[in] varNo The number of subscribed variables
     * @param[in] defFile The stream to read variable values from
     */
    void commandSubscribeObjectVariable(int domID, const std::string& objID, int beginTime, int endTime, int varNo, std::ifstream& defFile);


    /** @brief Sends and validates a SubscribeContext command
     * @param[in] domID The ID of the domain the addressed object belongs to
     * @param[in] objID The ID of the object a variable shall be subscribed from
     * @param[in] beginTime The time the subscription shall begin at
     * @param[in] endTime The time the subscription shall end at
     * @param[in] domain The domain of the objects which shall be reported
     * @param[in] range The range within which objects shall be for being reported
     * @param[in] varNo The number of subscribed variables
     * @param[in] defFile The stream to read variable values from
     */
    void commandSubscribeContextVariable(int domID, const std::string& objID, int beginTime, int endTime, int domain, SUMOReal range, int varNo, std::ifstream& defFile);
    /// @}



private:
    /// @name Report helper
    /// @{

    /** @brief Writes the results file
     */
    void writeResult();


    /** @brief Writes an error message
     * @param[in] msg The message to write
     */
    void errorMsg(std::stringstream& msg);
    /// @}



    /// @name Results validation methods
    /// @{

    /** @brief Validates whether the given message is a valid answer to CMD_SIMSTEP2
     * @param[in] inMsg The storage contain the message to validate
     * @return Whether the message is valid
     */
    bool validateSimulationStep2(tcpip::Storage& inMsg);


    /** @brief Validates whether the given message is a valid subscription return message
     * @param[in] inMsg The storage contain the message to validate
     * @return Whether the message is valid
     */
    bool validateSubscription(tcpip::Storage& inMsg);
    /// @}



    /// @name Conversion helper
    /// @{

    /** @brief Parses the next value type / value pair from the stream and inserts it into the storage
     *
     * @param[out] into The storage to add the value type and the value into
     * @param[in] defFile The file to read the values from
     * @param[out] msg If any error occurs, this should be filled
     * @return The number of written bytes
     */
    int setValueTypeDependant(tcpip::Storage& into, std::ifstream& defFile, std::stringstream& msg);


    /** @brief Reads a value of the given type from the given storage and reports it
     * @param[in] inMsg The storage to read the value from
     * @param[in] valueDataType The type of the expected value
     */
    void readAndReportTypeDependent(tcpip::Storage& inMsg, int valueDataType);
    /// @}


private:
    /// @brief The name of the file to write the results log into
    std::string outputFileName;

    /// @brief Stream containing the log
    std::stringstream answerLog;

};

#endif
