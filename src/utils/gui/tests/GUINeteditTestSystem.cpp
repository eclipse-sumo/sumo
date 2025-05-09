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
/// @file    GUINeteditTestSystem.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2025
///
// Thread used for testing netedit
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/elements/GNEAttributeCarrier.h>
#include <netedit/frames/GNETagSelector.h>
#include <netedit/frames/network/GNEAdditionalFrame.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>

#include <thread>
#include <chrono>

#include "GUINeteditTestSystem.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GUINeteditTestSystem::GUINeteditTestSystem() {}


GUINeteditTestSystem::~GUINeteditTestSystem() {}


void 
GUINeteditTestSystem::setSpecificMainWindow(GUIMainWindow* mainWindow) {
    myGNEApplicationWindow = dynamic_cast<GNEApplicationWindow*>(mainWindow);
    // check that GNEApplicationWindow was casted successfully
    if (myGNEApplicationWindow == nullptr) {
        throw ProcessError("Invalid GNEApplicationWindow used in GUINeteditTestSystem");
    }
}


void
GUINeteditTestSystem::runSpecificTest(const TestStep* testStep) {
    // continue depending of step type
    switch (testStep->getStepType()) {
        // supermodes
        case TestStepType::SUPERMODE_NETWORK:
            myGNEApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_F2_SUPERMODE_NETWORK), nullptr);
            break;
        case TestStepType::SUPERMODE_DEMAND:
            myGNEApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_F3_SUPERMODE_DEMAND), nullptr);
            break;
        case TestStepType::SUPERMODE_DATA:
            myGNEApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_F4_SUPERMODE_DATA), nullptr);
            break;
        // network mode
        case TestStepType::NETWORKMODE_INSPECT:
            myGNEApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_I_MODE_INSPECT), nullptr);
            break;
        case TestStepType::NETWORKMODE_DELETE:
            myGNEApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_D_MODE_SINGLESIMULATIONSTEP_DELETE), nullptr);
            break;
        case TestStepType::NETWORKMODE_SELECT:
            myGNEApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_S_MODE_STOPSIMULATION_SELECT), nullptr);
            break;
        case TestStepType::NETWORKMODE_MOVE:
            myGNEApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_M_MODE_MOVE_MEANDATA), nullptr);
            break;
        case TestStepType::NETWORKMODE_EDGE:
            myGNEApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_E_MODE_EDGE_EDGEDATA), nullptr);
            break;
        case TestStepType::NETWORKMODE_TRAFFICLIGHT:
            myGNEApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_T_MODE_TLS_TYPE), nullptr);
            break;
        case TestStepType::NETWORKMODE_CONNECTION:
            myGNEApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_C_MODE_CONNECT_CONTAINER), nullptr);
            break;
        case TestStepType::NETWORKMODE_PROHIBITION:
            myGNEApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_H_MODE_PROHIBITION_CONTAINERPLAN), nullptr);
            break;
        case TestStepType::NETWORKMODE_CROSSING:
            myGNEApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_R_MODE_CROSSING_ROUTE_EDGERELDATA), nullptr);
            break;
        case TestStepType::NETWORKMODE_ADDITIONAL:
            myGNEApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_A_MODE_STARTSIMULATION_ADDITIONALS_STOPS), nullptr);
            break;
        case TestStepType::NETWORKMODE_WIRE:
            myGNEApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_W_MODE_WIRE_ROUTEDISTRIBUTION), nullptr);
            break;
        case TestStepType::NETWORKMODE_TAZ:
            myGNEApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_Z_MODE_TAZ_TAZREL), nullptr);
            break;
        case TestStepType::NETWORKMODE_SHAPE:
            myGNEApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_P_MODE_POLYGON_PERSON), nullptr);
            break;
        case TestStepType::NETWORKMODE_DECAL:
            myGNEApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_U_MODE_DECAL_TYPEDISTRIBUTION), nullptr);
            break;
        // set additional
        case TestStepType::SELECT_ADDITIONAL:
            myGNEApplicationWindow->getViewNet()->getViewParent()->getAdditionalFrame()->getAdditionalTagSelector()->handle(this, FXSEL(SEL_COMMAND, MID_GNE_TAG_SELECTED), (void*)testStep->getText());
            break;
        // other
        case TestStepType::PROCESSING:
            myGNEApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_F5_COMPUTE_NETWORK_DEMAND), nullptr);
            break;
        case TestStepType::SAVE_NETEDITCONFIG:
            myGNEApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_SHIFT_E_SAVENETEDITCONFIG), nullptr);
            break;
        case TestStepType::QUIT:
            myGNEApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_Q_CLOSE), nullptr);
            break;
        default:
            throw ProcessError("Test type not yet implemented for Netedit");
    }
}

/****************************************************************************/