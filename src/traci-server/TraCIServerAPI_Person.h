/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    TraCIServerAPI_Person.h
/// @author  Daniel Krajzewicz
/// @date    26.05.2014
///
// APIs for getting/setting person values via TraCI
/****************************************************************************/
#pragma once
#include <config.h>

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


private:
    /// @brief invalidated copy constructor
    TraCIServerAPI_Person(const TraCIServerAPI_Person& s);

    /// @brief invalidated assignment operator
    TraCIServerAPI_Person& operator=(const TraCIServerAPI_Person& s);


};
