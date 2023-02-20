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
/// @file    Command_Hotkey_TrafficLight.cpp
/// @author  Jakob Erdmann
/// @date    31 Jan 2022
///
// Registers custom hotkey for aborting current traffic light phase
/****************************************************************************/
#include <config.h>

#include <fxkeys.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <guisim/GUINet.h>
#include "Command_Hotkey_TrafficLight.h"


// ===========================================================================
// method definitions
// ===========================================================================
Command_Hotkey_TrafficLight::Command_Hotkey_TrafficLight(MSTrafficLightLogic& tll) :
    myLogic(tll)
{ }


Command_Hotkey_TrafficLight::~Command_Hotkey_TrafficLight() { }


SUMOTime
Command_Hotkey_TrafficLight::execute(SUMOTime currentTime) {
    int next = (myLogic.getCurrentPhaseIndex() + 1) % myLogic.getPhaseNumber();
    myLogic.changeStepAndDuration(MSNet::getInstance()->getTLSControl(), currentTime, next, -1);
    return 1;
}


bool
Command_Hotkey_TrafficLight::registerHotkey(const std::string& key, MSTrafficLightLogic& tll) {
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
        gn->addHotkey(hotkey, new Command_Hotkey_TrafficLight(tll));
    }
    return true;
}


/****************************************************************************/
