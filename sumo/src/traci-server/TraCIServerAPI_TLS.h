/****************************************************************************/
/// @file    TraCIServerAPI_TLS.h
/// @author  Daniel Krajzewicz
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting traffic light values via TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef TraCIServerAPI_TLS_h
#define TraCIServerAPI_TLS_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "TraCIException.h"
#include "TraCIServer.h"
#include <foreign/tcpip/storage.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TraCIServerAPI_TLS
 * @brief APIs for getting/setting traffic light values via TraCI
 */
class TraCIServerAPI_TLS {
public:
    /** @brief Processes a get value command (Command 0xa2: Get Traffic Lights Variable)
     *
     * @param[in] server The TraCI-server-instance which schedules this request
     * @param[in] inputStorage The storage to read the command from
     * @param[out] outputStorage The storage to write the result to
     */
    static bool processGet(traci::TraCIServer &server, tcpip::Storage &inputStorage,
                           tcpip::Storage &outputStorage);


    /** @brief Processes a set value command (Command 0xc2: Change Traffic Lights State)
     *
     * @param[in] server The TraCI-server-instance which schedules this request
     * @param[in] inputStorage The storage to read the command from
     * @param[out] outputStorage The storage to write the result to
     */
    static bool processSet(traci::TraCIServer &server, tcpip::Storage &inputStorage,
                           tcpip::Storage &outputStorage);



    /// @name "old" API functions
    /// @{

    /** @brief Processes command getTLStatus
     *
     * The traffic light with the given id is asked for all state transitions, that will occur within
     *  a given time interval. Each status change is returned by a TLSwitch command.
     */
    static bool commandGetTLStatus(traci::TraCIServer &server, tcpip::Storage &inputStorage, tcpip::Storage &outputStorage);

    /** @brief Processes command getAllTLIds
     *
     * Returns a list of strings representing the ids of all traffic lights in the simulation
     */
    static bool commandGetAllTLIds(traci::TraCIServer &server, tcpip::Storage &inputStorage, tcpip::Storage &outputStorage);
    /// @}

protected:
    /// @brief Whether deprecated definition of phases was already reported
    static bool myHaveWarnedAboutDeprecatedPhases;

private:
    /// @brief invalidated copy constructor
    TraCIServerAPI_TLS(const TraCIServerAPI_TLS &s);

    /// @brief invalidated assignment operator
    TraCIServerAPI_TLS &operator=(const TraCIServerAPI_TLS &s);


};


#endif

/****************************************************************************/

