/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
/// @file    GUIDesigns.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2020
///
// File with the definitions of standard style of FXObjects in SUMO
/****************************************************************************/

#include "GUIDesigns.h"

// ===========================================================================
// Definitions
// ===========================================================================

FXMenuCommand*
GUIDesigns::buildFXMenuCommand(FXComposite* p, const std::string& text, FXIcon* icon, FXObject* tgt, FXSelector sel) {
    // build rest of menu commands
    FXMenuCommand* menuCommand = new FXMenuCommand(p, text.c_str(), icon, tgt, sel, LAYOUT_FIX_HEIGHT);
    // wet width and height (to avoid problems between Windows und Linux
    menuCommand->setHeight(23);
    // return menuCommand
    return menuCommand;
}


FXMenuCommand*
GUIDesigns::buildFXMenuCommandRecentFile(FXComposite* p, const std::string& text, const double width, FXIcon* icon, FXObject* tgt, FXSelector sel) {
    // build rest of menu commands
    FXMenuCommand* menuCommand = new FXMenuCommand(p, text.c_str(), icon, tgt, sel, LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT);
    // wet width and height (to avoid problems between Windows und Linux
    menuCommand->setWidth(width);
    menuCommand->setHeight(23);
    // return menuCommand
    return menuCommand;
}


FXMenuCheck*
GUIDesigns::buildFXMenuCheck(FXComposite* p, const std::string& text, FXIcon* icon, FXObject* tgt, FXSelector sel) {
    // crate menuCheck
    FXMenuCheck* menuCheck = new FXMenuCheck(p, text.c_str(), tgt, sel, LAYOUT_FIX_HEIGHT);
    // set height
    menuCheck->setHeight(23);
    // check if icon hast o be set
    if (icon) {
        menuCheck->setIcon(icon);
    }
    // return menu check
    return menuCheck;
}