/****************************************************************************/
/// @file    TraCIServerAPI_AreaDetector.h
/// @author  Mario Krumnow
/// @author  Michael Behrisch
/// @date    15.09.2013
/// @version $Id$
///
// APIs for getting/setting areal detector values via TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2013-2016 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef TraCIServerAPI_ArealDetector_h
#define TraCIServerAPI_ArealDetector_h


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
 * @class TraCIServerAPI_AreaDetector
 * @brief APIs for getting/setting multi-entry/multi-exit detector values via TraCI
 */
class TraCIServerAPI_ArealDetector {
public:
    /** @brief Processes a get value command (Command 0xa1: Get AreaDetector Variable)
     *
     * @param[in] server The TraCI-server-instance which schedules this request
     * @param[in] inputStorage The storage to read the command from
     * @param[out] outputStorage The storage to write the result to
     */
    static bool processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                           tcpip::Storage& outputStorage);


private:
    /// @brief invalidated copy constructor
    TraCIServerAPI_ArealDetector(const TraCIServerAPI_ArealDetector& s);

    /// @brief invalidated assignment operator
    TraCIServerAPI_ArealDetector& operator=(const TraCIServerAPI_ArealDetector& s);


};


#endif

#endif

/****************************************************************************/

