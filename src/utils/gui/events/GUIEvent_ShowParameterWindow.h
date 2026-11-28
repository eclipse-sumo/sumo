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
/// @file    GUIEvent_ShowParameterWindow.h
/// @author  Jakob Erdmann
/// @date    2026-03-27
///
// Event sent when a parameter window should be opened on the GUI thread
/****************************************************************************/
#pragma once
#include <config.h>

#include <atomic>
#include <memory>
#include <string>
#include "GUIEvent.h"


// ===========================================================================
// class definitions
// ===========================================================================
class GUIEvent_ShowParameterWindow : public GUIEvent {
public:
    /// @brief constructor
    GUIEvent_ShowParameterWindow(const std::string& fullName, const std::shared_ptr<std::atomic<bool>>& done) :
        GUIEvent(GUIEventType::SHOW_PARAMETER_WINDOW),
        myFullName(fullName),
        myDone(done) {
    }

    /// @brief destructor
    ~GUIEvent_ShowParameterWindow() override {
        if (myDone != nullptr) {
            *myDone = true;
        }
    }

    /// @brief return target GL object full name, or empty string for network parameters
    const std::string& getFullName() const {
        return myFullName;
    }

private:
    const std::string myFullName;
    std::shared_ptr<std::atomic<bool>> myDone;
};
