/****************************************************************************/
/// @file    TraCIServerAPI_GUI.h
/// @author  Daniel Krajzewicz
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting GUI values via TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef TraCIServerAPI_GUI_h
#define TraCIServerAPI_GUI_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifndef NO_TRACI

#include <traci-server/TraCIException.h>
#include <traci-server/TraCIServer.h>
#include <foreign/tcpip/storage.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TraCIServerAPI_GUI
 * @brief APIs for getting/setting GUI values via TraCI
 */
class TraCIServerAPI_GUI {
public:
    /** @brief Processes a get value command (Command 0xac: Get GUI Variable)
     *
     * @param[in] server The TraCI-server-instance which schedules this request
     * @param[in] inputStorage The storage to read the command from
     * @param[out] outputStorage The storage to write the result to
     */
    static bool processGet(traci::TraCIServer &server, tcpip::Storage &inputStorage, 
		tcpip::Storage &outputStorage) throw(traci::TraCIException, std::invalid_argument);


    /** @brief Processes a set value command (Command 0xcc: Change GUI State)
     *
     * @param[in] server The TraCI-server-instance which schedules this request
     * @param[in] inputStorage The storage to read the command from
     * @param[out] outputStorage The storage to write the result to
     */
    static bool processSet(traci::TraCIServer &server, tcpip::Storage &inputStorage, 
		tcpip::Storage &outputStorage) throw(traci::TraCIException, std::invalid_argument);


private:
    /// @brief invalidated copy constructor
    TraCIServerAPI_GUI(const TraCIServerAPI_GUI &s);

    /// @brief invalidated assignment operator
    TraCIServerAPI_GUI &operator=(const TraCIServerAPI_GUI &s);


};


#endif

#endif

/****************************************************************************/

