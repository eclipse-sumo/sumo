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
/// @file    GUIDesigns.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2020
///
// File with the definitions of standard style of FXObjects in SUMO
/****************************************************************************/

#include "GUIDesigns.h"

#include "utils/foxtools/MFXMenuCheckIcon.h"


// ===========================================================================
// Definitions
// ===========================================================================

FXMenuTitle*
GUIDesigns::buildFXMenuTitle(FXComposite* p, const std::string& text, FXIcon* icon, FXMenuPane* menuPane) {
    // create menu title
    FXMenuTitle* menuTitle = new FXMenuTitle(p, text.c_str(), icon, menuPane, LAYOUT_FIX_HEIGHT);
    // setheight (to avoid problems between Windows und Linux)
    menuTitle->setHeight(GUIDesignHeight);
    // return menuTitle
    return menuTitle;
}


FXMenuCommand*
GUIDesigns::buildFXMenuCommand(FXComposite* p, const std::string& text, FXIcon* icon, FXObject* tgt, FXSelector sel) {
    // build menu command
    FXMenuCommand* menuCommand = new FXMenuCommand(p, text.c_str(), icon, tgt, sel, LAYOUT_FIX_HEIGHT);
    // set width and height (to avoid problems between Windows und Linux)
    menuCommand->setHeight(GUIDesignHeight);
    // return menuCommand
    return menuCommand;
}


FXMenuCommand*
GUIDesigns::buildFXMenuCommandShortcut(FXComposite* p, const std::string& text, const std::string& shortcut, const std::string& info, FXIcon* icon, FXObject* tgt, FXSelector sel) {
    // build menu command with shortcut
    FXMenuCommand* menuCommand = new FXMenuCommand(p, (text + "\t" + shortcut + "\t" + info).c_str(), icon, tgt, sel, LAYOUT_FIX_HEIGHT);
    // set width and height (to avoid problems between Windows und Linux)
    menuCommand->setHeight(GUIDesignHeight);
    // return menuCommand
    return menuCommand;
}


FXMenuCheck*
GUIDesigns::buildFXMenuCheckbox(FXComposite* p, const std::string& text, const std::string& info, FXObject* tgt, FXSelector sel) {
    // build menu checkbox
    FXMenuCheck* menuCheck = new FXMenuCheck(p, (text + std::string("\t\t") + info).c_str(), tgt, sel, LAYOUT_FIX_HEIGHT);
    // set height (to avoid problems between Windows und Linux)
    menuCheck->setHeight(GUIDesignHeight);
    // return menuCommand
    return menuCheck;
}


MFXMenuCheckIcon*
GUIDesigns::buildFXMenuCheckboxIcon(FXComposite* p, const std::string& text, const std::string& shortcut, const std::string& info, FXIcon* icon, FXObject* tgt, FXSelector sel) {
    // build menu checkbox
    MFXMenuCheckIcon* menuCheck = new MFXMenuCheckIcon(p, text, shortcut, info, icon, tgt, sel, LAYOUT_FIX_HEIGHT);
    // set height (to avoid problems between Windows und Linux)
    menuCheck->setHeight(GUIDesignHeight);
    // return menuCommand
    return menuCheck;
}


FXMenuCommand*
GUIDesigns::buildFXMenuCommandRecentFile(FXComposite* p, const std::string& text, FXObject* tgt, FXSelector sel) {
    // build rest of menu commands
    FXMenuCommand* menuCommand = new FXMenuCommand(p, text.c_str(), nullptr, tgt, sel, LAYOUT_FIX_HEIGHT);
    // set width and height (to avoid problems between Windows und Linux)
    menuCommand->setHeight(GUIDesignHeight);
    // return menuCommand
    return menuCommand;
}
