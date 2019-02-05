/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIShortcutsSubSys.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2019
/// @version $Id$
///
// Helper for Shortcuts
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIApplicationWindow.h>
#include <netedit/GNEApplicationWindow.h>

#include "GUIShortcutsSubSys.h"

// ===========================================================================
// member definitions
// ===========================================================================


void 
GUIShortcutsSubSys::buildSUMOAccelerators(GUIApplicationWindow *GUIApp) {
    // initialize Ctrl hotkeys with Caps Lock enabled using decimal code (to avoid problems in Linux)
    GUIApp->getAccelTable()->addAccel(262222, GUIApp, FXSEL(SEL_COMMAND, MID_OPEN_NETWORK));  // Ctrl + N
    GUIApp->getAccelTable()->addAccel(262223, GUIApp, FXSEL(SEL_COMMAND, MID_OPEN_CONFIG));   // Ctrl + O
    GUIApp->getAccelTable()->addAccel(262226, GUIApp, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_R_RELOAD)); // Ctrl + R
    GUIApp->getAccelTable()->addAccel(262224, GUIApp, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_P)); // Ctrl + P
    GUIApp->getAccelTable()->addAccel(262230, GUIApp, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_W_CLOSESIMULATION)); // Ctrl + W
    GUIApp->getAccelTable()->addAccel(262225, GUIApp, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_Q_CLOSE)); // Ctrl + Q
    GUIApp->getAccelTable()->addAccel(262214, GUIApp, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_F_FULSCREENMODE)); // Ctrl + F
    GUIApp->getAccelTable()->addAccel(262215, GUIApp, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_G_GAMINGMODE_TOOGLEGRID)); // Ctrl + G
    GUIApp->getAccelTable()->addAccel(262209, GUIApp, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_A_STARTSIMULATION)); // Ctrl + A
    GUIApp->getAccelTable()->addAccel(262227, GUIApp, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_S_SAVENETWORK_STOPSIMULATION)); // Ctrl + S
    GUIApp->getAccelTable()->addAccel(262212, GUIApp, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_D_SINGLESIMULATIONSTEP)); // Ctrl + D

    /** Disabled shorcuts for Locate dialog due #4261
    // initialize Shift hotkeys with Caps Lock enabled using decimal code (to avoid problems in Linux)
    GUIApp->getAccelTable()->addAccel(65642, GUIApp, FXSEL(SEL_COMMAND, MID_LOCATEJUNCTION)); // Shift + J
    GUIApp->getAccelTable()->addAccel(65637, GUIApp, FXSEL(SEL_COMMAND, MID_LOCATEEDGE));     // Shift + E
    GUIApp->getAccelTable()->addAccel(65652, GUIApp, FXSEL(SEL_COMMAND, MID_LOCATETLS));      // Shift + T
    GUIApp->getAccelTable()->addAccel(65633, GUIApp, FXSEL(SEL_COMMAND, MID_LOCATEADD));      // Shift + A
    GUIApp->getAccelTable()->addAccel(65647, GUIApp, FXSEL(SEL_COMMAND, MID_LOCATEPOI));      // Shift + O
    GUIApp->getAccelTable()->addAccel(65644, GUIApp, FXSEL(SEL_COMMAND, MID_LOCATEPOLY));     // Shift + L
    **/
}


void 
GUIShortcutsSubSys::buildNETEDITAccelerators(GNEApplicationWindow *GNEApp) {
    // initialize single hotkeys using decimal code (to avoid problems in Linux)
    GNEApp->getAccelTable()->addAccel(101, GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_E_EDGEMODE));   // e
    GNEApp->getAccelTable()->addAccel(69,  GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_E_EDGEMODE));   // E
    GNEApp->getAccelTable()->addAccel(109, GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_M_MOVEMODE));   // m
    GNEApp->getAccelTable()->addAccel(77,  GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_M_MOVEMODE));   // M
    GNEApp->getAccelTable()->addAccel(100, GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_D_DELETEMODE));   // d
    GNEApp->getAccelTable()->addAccel(68,  GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_D_DELETEMODE));   // D
    GNEApp->getAccelTable()->addAccel(105, GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_I_INSPECTMODE));   // i
    GNEApp->getAccelTable()->addAccel(73,  GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_I_INSPECTMODE));   // I
    GNEApp->getAccelTable()->addAccel(115, GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_S_SELECTMODE));   // s
    GNEApp->getAccelTable()->addAccel(83,  GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_S_SELECTMODE));   // S
    GNEApp->getAccelTable()->addAccel(99,  GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_C_CONNECTMODE));   // c
    GNEApp->getAccelTable()->addAccel(67,  GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_C_CONNECTMODE));   // C
    GNEApp->getAccelTable()->addAccel(119, GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_W_PROHIBITIONMODE));   // w
    GNEApp->getAccelTable()->addAccel(87,  GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_W_PROHIBITIONMODE));   // W
    GNEApp->getAccelTable()->addAccel(116, GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_T_TLSMODE_VTYPEMODE));   // t
    GNEApp->getAccelTable()->addAccel(94,  GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_T_TLSMODE_VTYPEMODE));   // T
    GNEApp->getAccelTable()->addAccel(97,  GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_A_ADDITIONALMODE));   // a
    GNEApp->getAccelTable()->addAccel(65,  GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_A_ADDITIONALMODE));   // A
    GNEApp->getAccelTable()->addAccel(114, GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_R_CROSSINGMODE_ROUTEMODE));   // r
    GNEApp->getAccelTable()->addAccel(82,  GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_R_CROSSINGMODE_ROUTEMODE));   // R
    GNEApp->getAccelTable()->addAccel(122, GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_Z_TAZMODE));   // z
    GNEApp->getAccelTable()->addAccel(90,  GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_Z_TAZMODE));   // Z
    GNEApp->getAccelTable()->addAccel(112, GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_P_POLYGONMODE));   // p
    GNEApp->getAccelTable()->addAccel(80,  GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_P_POLYGONMODE));   // P
    GNEApp->getAccelTable()->addAccel(118, GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_V_VEHICLEMODE));    // v
    GNEApp->getAccelTable()->addAccel(86,  GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_V_VEHICLEMODE));    // V
    
    // initialize Ctrl hotkeys with Caps Lock enabled using decimal code (to avoid problems in Linux)
    GNEApp->getAccelTable()->addAccel(262222, GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_N_NEWNETWORK));       // Ctrl + N
    GNEApp->getAccelTable()->addAccel(262223, GNEApp, FXSEL(SEL_COMMAND, MID_OPEN_NETWORK));           // Ctrl + O
    GNEApp->getAccelTable()->addAccel(327691, GNEApp, FXSEL(SEL_COMMAND, MID_OPEN_CONFIG));            // Ctrl + Shift + O
    GNEApp->getAccelTable()->addAccel(262226, GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_R_RELOAD));          // Ctrl + R
    GNEApp->getAccelTable()->addAccel(262227, GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_S_SAVENETWORK_STOPSIMULATION));       // Ctrl + S
    GNEApp->getAccelTable()->addAccel(327695, GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_SHIFT_S_SAVENETWORKAS));   // Ctrl + Shift + S
    GNEApp->getAccelTable()->addAccel(262220, GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_L_SAVEASPLAINXML));       // Ctrl + L
    GNEApp->getAccelTable()->addAccel(262218, GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_J_SAVEJOINEDJUNCTIONS));       // Ctrl + J
    /*
    GNEApp->getAccelTable()->addAccel(262224, GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_P));          // Ctrl + P
    GNEApp->getAccelTable()->addAccel(327692, GNEApp, FXSEL(SEL_COMMAND, MID_GNE_HOTKEY_CTRL_SHIFT_P));   // Ctrl + Shift + P
    */
    GNEApp->getAccelTable()->addAccel(262212, GNEApp, FXSEL(SEL_COMMAND, MID_OPEN_ADDITIONALS));                 // Ctrl + D
    GNEApp->getAccelTable()->addAccel(327780, GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_SHIFT_D_SAVEADDITIONAL));    // Ctrl + Shift + D
    GNEApp->getAccelTable()->addAccel(262219, GNEApp, FXSEL(SEL_COMMAND, MID_OPEN_TLSPROGRAMS));                 // Ctrl + K
    GNEApp->getAccelTable()->addAccel(327787, GNEApp, FXSEL(SEL_COMMAND, MID_GNE_HOTKEY_CTRL_SHIFT_K));            // Ctrl + Shift + K
    GNEApp->getAccelTable()->addAccel(262230, GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_W_CLOSESIMULATION));       // Ctrl + W
    GNEApp->getAccelTable()->addAccel(262225, GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_Q_CLOSE));               // Ctrl + Q
    GNEApp->getAccelTable()->addAccel(262234, GNEApp, FXSEL(SEL_COMMAND, FXUndoList::ID_UNDO));                  // Ctrl + Z
    GNEApp->getAccelTable()->addAccel(262233, GNEApp, FXSEL(SEL_COMMAND, FXUndoList::ID_REDO));                  // Ctrl + Y
    GNEApp->getAccelTable()->addAccel(262230, GNEApp, FXSEL(SEL_COMMAND, MID_EDITVIEWSCHEME));                   // Ctrl + V
    GNEApp->getAccelTable()->addAccel(262217, GNEApp, FXSEL(SEL_COMMAND, MID_EDITVIEWPORT));                    // Ctrl + I
    GNEApp->getAccelTable()->addAccel(262215, GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_G_GAMINGMODE_TOOGLEGRID));   // Ctrl + G
    GNEApp->getAccelTable()->addAccel(262228, GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_T_OPENSUMONETEDIT));       // Ctrl + T
    GNEApp->getAccelTable()->addAccel(parseAccel("ctrl+shift+c"), GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_SHIFT_C_SHOWCONNECTIONS));   // Ctrl + Shift + C
    GNEApp->getAccelTable()->addAccel(parseAccel("ctrl+shift+i"), GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_SHIFT_I_SELECTEDGES));      // Ctrl + Shift + I
    
    // initialize Shift hotkeys with Caps Lock enabled using decimal code (to avoid problems in Linux)
    GNEApp->getAccelTable()->addAccel(65642, GNEApp, FXSEL(SEL_COMMAND, MID_LOCATEJUNCTION)); // Shift + J
    GNEApp->getAccelTable()->addAccel(65637, GNEApp, FXSEL(SEL_COMMAND, MID_LOCATEEDGE));    // Shift + E
    GNEApp->getAccelTable()->addAccel(65652, GNEApp, FXSEL(SEL_COMMAND, MID_LOCATETLS));     // Shift + T
    GNEApp->getAccelTable()->addAccel(65633, GNEApp, FXSEL(SEL_COMMAND, MID_LOCATEADD));     // Shift + A
    GNEApp->getAccelTable()->addAccel(65647, GNEApp, FXSEL(SEL_COMMAND, MID_LOCATEPOI));     // Shift + O
    GNEApp->getAccelTable()->addAccel(65644, GNEApp, FXSEL(SEL_COMMAND, MID_LOCATEPOLY));    // Shift + L
    
    // initialize rest of hotkeys
    GNEApp->getAccelTable()->addAccel(parseAccel("F3"), GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_F3_SUPERMODE_NETWORK));
    GNEApp->getAccelTable()->addAccel(parseAccel("F4"), GNEApp, FXSEL(SEL_COMMAND, MID_HOTKEY_F4_SUPERMODE_DEMAND));
    GNEApp->getAccelTable()->addAccel(parseAccel("Esc"), GNEApp, FXSEL(SEL_COMMAND, MID_GNE_HOTKEY_ESC));
    GNEApp->getAccelTable()->addAccel(parseAccel("Del"), GNEApp, FXSEL(SEL_COMMAND, MID_GNE_HOTKEY_DEL));
    GNEApp->getAccelTable()->addAccel(parseAccel("Enter"), GNEApp, FXSEL(SEL_COMMAND, MID_GNE_HOTKEY_ENTER));
    GNEApp->getAccelTable()->addAccel(parseAccel("F12"), GNEApp, FXSEL(SEL_COMMAND, MID_GNE_HOTKEY_F12));
}


int 
GUIShortcutsSubSys::parseKey() {
    return 0;
}

/****************************************************************************/

