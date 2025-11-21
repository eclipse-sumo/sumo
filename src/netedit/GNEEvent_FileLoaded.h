/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEEvent_FileLoaded.h
/// @author  Jakob Erdmann
/// @date    Feb 2011
///
// Event to send when the network has been loaded by GNELoadThread
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>

#include <utils/gui/events/GUIEvent.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNENet;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEEvent_FileLoaded : public GUIEvent {

public:
    /// @brief type of loaded element
    enum class Type {
        NETECFG,            // netedit config
        SUMOCFG,            // sumo config
        NETCCFG,            // netconvert config
        NETWORK,            // sumo network
        OSM,                // OSM network
        CONSOLE,            // console options
        NEW,                // new network
        INVALID_TYPE,       // invalid loading type
        INVALID_OPTIONS,    // invalid options
        INVALID_PROJECTION  // invalid options
    };

    /// @brief constructor
    GNEEvent_FileLoaded(GNEEvent_FileLoaded::Type type, GNENet* net, const std::string& file,
                        const std::string& settingsFile, const bool viewportFromRegistry);

    /// @brief destructor
    ~GNEEvent_FileLoaded();

    /// @brief get event type
    GNEEvent_FileLoaded::Type getType() const;

    /// @brief get the loaded net
    GNENet* getNet() const;

    /// @brief get the name of the loaded file
    const std::string& getFile() const;

    /// @brief get the name of the settings file to load
    const std::string& getSettingsFile() const;

    /// @brief get whether loading viewport from registry
    bool getViewportFromRegistry() const;

protected:
    /// @brief event type
    GNEEvent_FileLoaded::Type myType = GNEEvent_FileLoaded::Type::INVALID_TYPE;

    /// @brief the loaded net
    GNENet* myNet;

    /// @brief the name of the loaded file
    std::string myFile;

    /// @brief the name of the settings file to load
    std::string mySettingsFile;

    /// @brief whether loading viewport from registry
    bool myViewportFromRegistry;

private:
    /// @brief Invalidated copy constructor.
    GNEEvent_FileLoaded(const GNEEvent_FileLoaded&) = delete;

    /// @brief Invalidated assignment operator
    GNEEvent_FileLoaded& operator=(const GNEEvent_FileLoaded& src) = delete;
};
