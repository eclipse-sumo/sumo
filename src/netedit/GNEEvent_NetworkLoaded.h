/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEEvent_NetworkLoaded.h
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// Event to send when the network has been loaded by GNELoadThread
/****************************************************************************/
#ifndef GNEEvent_NetworkLoaded_h
#define GNEEvent_NetworkLoaded_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/gui/events/GUIEvent.h>


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

