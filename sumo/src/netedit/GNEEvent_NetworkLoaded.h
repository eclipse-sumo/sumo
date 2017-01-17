/****************************************************************************/
/// @file    GNEEvent_NetworkLoaded.h
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// Event to send when the network has been loaded by GNELoadThread
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
#ifndef GNEEvent_NetworkLoaded_h
#define GNEEvent_NetworkLoaded_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream>
#include <utils/gui/events/GUIEvent.h>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GNENet;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class  GNEEvent_NetworkLoaded
 *
 * Throw to GUIApplicationWindow from GUILoadThread after a simulation has
 * been loaded or the loading process failed
 */
class GNEEvent_NetworkLoaded : public GUIEvent {
public:
    /// @brief constructor
    GNEEvent_NetworkLoaded(GNENet* net,
                           const std::string& file,
                           const std::string& settingsFile,
                           const bool viewportFromRegistry)
        : GUIEvent(EVENT_SIMULATION_LOADED),
          myNet(net), myFile(file), mySettingsFile(settingsFile), myViewportFromRegistry(viewportFromRegistry) { }

    /// @brief destructor
    ~GNEEvent_NetworkLoaded() { }

public:
    /// @brief the loaded net
    GNENet* myNet;

    /// @brief the name of the loaded file
    std::string myFile;

    /// @brief the name of the settings file to load
    std::string mySettingsFile;

    /// @brief whether loading viewport from registry
    bool myViewportFromRegistry;
};


#endif

/****************************************************************************/

