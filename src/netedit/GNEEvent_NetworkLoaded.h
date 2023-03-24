/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GNEEvent_NetworkLoaded.h
/// @author  Jakob Erdmann
/// @date    Feb 2011
///
// Event to send when the network has been loaded by GNELoadThread
/****************************************************************************/
#pragma once
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
    GNEEvent_NetworkLoaded(GNENet* _net, const std::string& _file,
                           const std::string& _settingsFile, const bool _viewportFromRegistry) :
        GUIEvent(GUIEventType::SIMULATION_LOADED),
        net(_net),
        file(_file),
        settingsFile(_settingsFile),
        viewportFromRegistry(_viewportFromRegistry) { }

    /// @brief destructor
    ~GNEEvent_NetworkLoaded() { }

    /// @brief the loaded net
    GNENet* net;

    /// @brief the name of the loaded file
    const std::string file;

    /// @brief the name of the settings file to load
    const std::string settingsFile;

    /// @brief whether loading viewport from registry
    const bool viewportFromRegistry;
};
