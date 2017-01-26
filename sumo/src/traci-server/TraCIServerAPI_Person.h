/****************************************************************************/
/// @file    TraCIServerAPI_Person.h
/// @author  Daniel Krajzewicz
/// @date    26.05.2014
/// @version $Id$
///
// APIs for getting/setting person values via TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef TraCIServerAPI_Person_h
#define TraCIServerAPI_Person_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifndef NO_TRACI

#include <foreign/tcpip/storage.h>


// ===========================================================================
// class declarations
// ===========================================================================
class TraCIServer;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TraCIServerAPI_Person
 * @brief APIs for getting/setting person values via TraCI
 */
class TraCIServerAPI_Person {
public:
    /** @brief Processes a get value command (Command 0xae: Get Person Variable)
     *
     * @param[in] server The TraCI-server-instance which schedules this request
     * @param[in] inputStorage The storage to read the command from
     * @param[out] outputStorage The storage to write the result to
     */
    static bool processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                           tcpip::Storage& outputStorage);


    /** @brief Processes a set value command (Command 0xce: Change Person State)
     *
     * @param[in] server The TraCI-server-instance which schedules this request
     * @param[in] inputStorage The storage to read the command from
     * @param[out] outputStorage The storage to write the result to
     */
    static bool processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                           tcpip::Storage& outputStorage);


    /** @brief Returns the named persons's position
     * @param[in] id The id of the searched person
     * @param[out] p The position, if the person is on the network
     * @return Whether the person is known
     */
    static bool getPosition(const std::string& id, Position& p);

private:
    static MSVehicleType& getSingularType(MSTransportable* const t);

private:
    /// @brief invalidated copy constructor
    TraCIServerAPI_Person(const TraCIServerAPI_Person& s);

    /// @brief invalidated assignment operator
    TraCIServerAPI_Person& operator=(const TraCIServerAPI_Person& s);


};


#endif

#endif

/****************************************************************************/
