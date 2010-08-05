/****************************************************************************/
/// @file    TraCIServerAPI_Junction.h
/// @author  Daniel Krajzewicz
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting junction values via TraCI
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
#ifndef TraCIServerAPI_Junction_h
#define TraCIServerAPI_Junction_h


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
 * @class TraCIServerAPI_Junction
 * @brief APIs for getting/setting junction values via TraCI
 */
class TraCIServerAPI_Junction {
public:
    /** @brief Processes a get value command (Command 0xa9: Get Junction Variable)
     *
     * @param[in] server The TraCI-server-instance which schedules this request
     * @param[in] inputStorage The storage to read the command from
     * @param[out] outputStorage The storage to write the result to
     */
    static bool processGet(traci::TraCIServer &server, tcpip::Storage &inputStorage, 
		tcpip::Storage &outputStorage);


private:
    /// @brief invalidated copy constructor
    TraCIServerAPI_Junction(const TraCIServerAPI_Junction &s);

    /// @brief invalidated assignment operator
    TraCIServerAPI_Junction &operator=(const TraCIServerAPI_Junction &s);


};


#endif

/****************************************************************************/

