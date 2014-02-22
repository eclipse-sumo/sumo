/****************************************************************************/
/// @file    TraCIServerAPI_Junction.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting junction values via TraCI
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

#ifndef NO_TRACI

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
    static bool processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                           tcpip::Storage& outputStorage);


    /** @brief Returns the named junction's position
     * @param[in] id The id of the searched junction
     * @param[out] p The position, if the junction is known
     * @return Whether the junction is known (and on road)
     */
    static bool getPosition(const std::string& id, Position& p);


    /** @brief Returns a tree filled with junction instances
     * @return The rtree of junctions
     */
    static NamedRTree* getTree();


private:
    /// @brief invalidated copy constructor
    TraCIServerAPI_Junction(const TraCIServerAPI_Junction& s);

    /// @brief invalidated assignment operator
    TraCIServerAPI_Junction& operator=(const TraCIServerAPI_Junction& s);


};


#endif

#endif

/****************************************************************************/

