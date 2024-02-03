/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
GUIDesigns::buildFXMenuCommand(FXComposite* p, const std::string& text, FXIcon* icon, FXObject* tgt, FXSelector sel, const bool disable) {
    // build menu command
    FXMenuCommand* menuCommand = new FXMenuCommand(p, text.c_str(), icon, tgt, sel, LAYOUT_FIX_HEIGHT);
    // set width and height (to avoid problems between Windows und Linux)
    menuCommand->setHeight(GUIDesignHeight);
    // check if disable after creation (used in certain parts of netedit)
    if (disable) {
        menuCommand->disable();
    }
    // return menuCommand
    return menuCommand;
}


FXMenuCommand*
GUIDesigns::buildFXMenuCommand(FXComposite* p, const std::string& text, const std::string& help, FXIcon* icon, FXObject* tgt, FXSelector sel, const bool disable) {
    // build menu command
    FXMenuCommand* menuCommand = new FXMenuCommand(p, text.c_str(), icon, tgt, sel, LAYOUT_FIX_HEIGHT);
    // set help
    menuCommand->setHelpText(help.c_str());
    // set width and height (to avoid problems between Windows und Linux)
    menuCommand->setHeight(GUIDesignHeight);
    // check if disable after creation (used in certain parts of netedit)
    if (disable) {
        menuCommand->disable();
    }
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


FXLabel*
GUIDesigns::buildFXLabel(FXComposite* p, const std::string& text, const std::string& tip, const std::string& help, FXIcon* ic,
                         FXuint opts, FXint x, FXint y, FXint w, FXint h, FXint pl, FXint pr, FXint pt, FXint pb) {
    FXLabel* label = new FXLabel(p, text.c_str(), ic, opts, x, y, w, h, pl, pr, pt, pb);
    label->setTipText(tip.c_str());
    label->setHelpText(help.c_str());
    return label;
}


FXButton*
GUIDesigns::buildFXButton(FXComposite* p, const std::string& text, const std::string& tip, const std::string& help, FXIcon* ic, FXObject* tgt,
                          FXSelector sel, FXuint opts, FXint x, FXint y, FXint w, FXint h, FXint pl, FXint pr, FXint pt, FXint pb) {
    FXButton* button = new FXButton(p, text.c_str(), ic, tgt, sel, opts, x, y, w, h, pl, pr, pt, pb);
    button->setTipText(tip.c_str());
    button->setHelpText(help.c_str());
    return button;
}


FXRadioButton*
GUIDesigns::buildFXRadioButton(FXComposite* p, const std::string& text, const std::string& tip, const std::string& help, FXObject* tgt,
                               FXSelector sel, FXuint opts, FXint x, FXint y, FXint w, FXint h, FXint pl, FXint pr, FXint pt, FXint pb) {
    FXRadioButton* radioButton = new FXRadioButton(p, text.c_str(), tgt, sel, opts, x, y, w, h, pl, pr, pt, pb);
    radioButton->setTipText(tip.c_str());
    radioButton->setHelpText(help.c_str());
    return radioButton;
}
