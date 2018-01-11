/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2009-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    TraCIServerAPI_Lane.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting lane values via TraCI
/****************************************************************************/
#ifndef TraCIServerAPI_Lane_h
#define TraCIServerAPI_Lane_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <foreign/tcpip/storage.h>


// ===========================================================================
// class declarations
// ===========================================================================
class TraCIServer;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TraCIServerAPI_Lane
 * @brief APIs for getting/setting lane values via TraCI
 */
class TraCIServerAPI_Lane {
public:
    /** @brief Processes a get value command (Command 0xa3: Get Lane Variable)
     *
     * @param[in] server The TraCI-server-instance which schedules this request
     * @param[in] inputStorage The storage to read the command from
     * @param[out] outputStorage The storage to write the result to
     */
    static bool processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                           tcpip::Storage& outputStorage);


    /** @brief Processes a set value command (Command 0xc3: Change Lane State)
     *
     * @param[in] server The TraCI-server-instance which schedules this request
     * @param[in] inputStorage The storage to read the command from
     * @param[out] outputStorage The storage to write the result to
     */
    static bool processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                           tcpip::Storage& outputStorage);


    /** @brief Returns the named lane's shape
     *
     * @param[in] id The id of the searched lane
     * @param[out] shape The shape, if the lane is known
     * @return Whether the lane is known
     */
    static bool getShape(const std::string& id, PositionVector& shape);


private:
    /// @brief invalidated copy constructor
    TraCIServerAPI_Lane(const TraCIServerAPI_Lane& s);

    /// @brief invalidated assignment operator
    TraCIServerAPI_Lane& operator=(const TraCIServerAPI_Lane& s);


};


#endif

/****************************************************************************/
