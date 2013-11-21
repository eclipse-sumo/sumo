/****************************************************************************/
/// @file    TraCIServerAPI_GUI.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting GUI values via TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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

#include <string>
#include <traci-server/TraCIException.h>
#include <traci-server/TraCIServer.h>
#include <foreign/tcpip/storage.h>


// ===========================================================================
// class definitions
// ===========================================================================
class GUIGlChildWindow;


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
    /** @brief Returns the main window
     * @return The main window (should exist)
     */
    static GUIMainWindow* getMainWindow();


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

#endif

/****************************************************************************/

