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
/// @file    Command_Hotkey_InductionLoop.cpp
/// @author  Jakob Erdmann
/// @date    31 Jan 2022
///
// Registers custom hotkey for overriding detector value
/****************************************************************************/
#include <config.h>

#include <fxkeys.h>
#include <microsim/output/MSInductLoop.h>
#include <guisim/GUINet.h>
#include "Command_Hotkey_InductionLoop.h"


// ===========================================================================
// method definitions
// ===========================================================================
Command_Hotkey_InductionLoop::Command_Hotkey_InductionLoop(MSInductLoop* det, bool set) :
    myDetector(det), mySet(set)
{ }


Command_Hotkey_InductionLoop::~Command_Hotkey_InductionLoop() { }


SUMOTime
Command_Hotkey_InductionLoop::execute(SUMOTime /*currentTime*/) {
    myDetector->overrideTimeSinceDetection(mySet ? 0 : -1);
    return 1;
}


bool
Command_Hotkey_InductionLoop::registerHotkey(const std::string& key, MSInductLoop* det) {
    int hotkey = -1;
    if (key.size() == 1) {
        char c = key[0];
        if ('a' <= c && c <= 'z') {
            // see FXAccelTable::parseAccel
            hotkey = c + FX::KEY_space - ' ';
        } else {
            WRITE_WARNINGF(TL("Hotkey '%' is not supported"), key);
            return false;
        }
    } else {
        WRITE_WARNINGF(TL("Hotkey '%' is not supported"), key);
        return false;
    }
    GUINet* gn = dynamic_cast<GUINet*>(MSNet::getInstance());
    if (gn != nullptr) {
        gn->addHotkey(hotkey,
                      new Command_Hotkey_InductionLoop(det, true),
                      new Command_Hotkey_InductionLoop(det, false));
    }
    return true;
}

/****************************************************************************/
