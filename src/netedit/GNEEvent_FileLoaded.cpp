/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    GNEEvent_FileLoaded.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2011
///
// Event to send when the network has been loaded by GNELoadThread
/****************************************************************************/

#include "GNEEvent_FileLoaded.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEEvent_FileLoaded::GNEEvent_FileLoaded(GNEEvent_FileLoaded::Type type, GNENet* net, const std::string& file,
        const std::string& settingsFile, const bool viewportFromRegistry) :
    GUIEvent(GUIEventType::SIMULATION_LOADED),
    myType(type),
    myNet(net),
    myFile(file),
    mySettingsFile(settingsFile),
    myViewportFromRegistry(viewportFromRegistry) {
}


GNEEvent_FileLoaded::~GNEEvent_FileLoaded() {}


GNEEvent_FileLoaded::Type
GNEEvent_FileLoaded::getType() const {
    return myType;
}


GNENet*
GNEEvent_FileLoaded::getNet() const {
    return myNet;
}


const std::string&
GNEEvent_FileLoaded::getFile() const {
    return myFile;
}


const std::string&
GNEEvent_FileLoaded::getSettingsFile() const {
    return mySettingsFile;
}


bool
GNEEvent_FileLoaded::getViewportFromRegistry() const {
    return myViewportFromRegistry;
}

/****************************************************************************/
