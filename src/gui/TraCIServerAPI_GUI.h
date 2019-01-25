/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    TraCIServerAPI_GUI.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting GUI values via TraCI
/****************************************************************************/
#ifndef TraCIServerAPI_GUI_h
#define TraCIServerAPI_GUI_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
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
    static bool processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                           tcpip::Storage& outputStorage);


    /** @brief Processes a set value command (Command 0xcc: Change GUI State)
     *
     * @param[in] server The TraCI-server-instance which schedules this request
     * @param[in] inputStorage The storage to read the command from
     * @param[out] outputStorage The storage to write the result to
     */
    static bool processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                           tcpip::Storage& outputStorage);


protected:
    /** @brief Returns the named view
     * @param[in] viewName The name of the view
     * @return The named view if existing, 0 otherwise
     */
    static GUISUMOAbstractView* getNamedView(const std::string& id);


private:
    /// @brief invalidated copy constructor
    TraCIServerAPI_GUI(const TraCIServerAPI_GUI& s);

    /// @brief invalidated assignment operator
    TraCIServerAPI_GUI& operator=(const TraCIServerAPI_GUI& s);


};


#endif

/****************************************************************************/

