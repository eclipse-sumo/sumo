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
/// @file    GNEBasicDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2025
///
// A basic dialog with simple buttons (ok, accept, cancel, etc.)
/****************************************************************************/

#include <utils/common/MsgHandler.h>
#include <utils/foxtools/MFXLinkLabel.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEBasicDialog.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNEBasicDialog::GNEBasicDialog(GNEApplicationWindow* applicationWindow, const std::string &title,
                               const std::string &text, Buttons buttons, GUIIcon icon) :
    GNEDialog(applicationWindow, title.c_str(), GUIDesignDialogBox) {
    // get icon
    auto ic = GUIIconSubSys::getIcon(icon);
    // set dialog icon
    setIcon(ic);
    // create dialog layout (obtained from FXMessageBox)
    FXVerticalFrame* content = new FXVerticalFrame(this, LAYOUT_FILL_X | LAYOUT_FILL_Y);
    FXHorizontalFrame* info = new FXHorizontalFrame(content, LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_FILL_X | LAYOUT_FILL_Y, 0, 0, 0, 0, 10, 10, 10, 10);
    new FXLabel(info, FXString::null, ic, ICON_BEFORE_TEXT | LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_FILL_X | LAYOUT_FILL_Y);
    new FXLabel(info, text.c_str(), NULL, JUSTIFY_LEFT | ICON_BEFORE_TEXT | LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_FILL_X | LAYOUT_FILL_Y);
    new FXHorizontalSeparator(content, SEPARATOR_GROOVE | LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_FILL_X);
    // Create horizontal separator
    new FXHorizontalSeparator(this, GUIDesignHorizontalSeparator);
    // Create frame for buttons
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    // add horizontal frame used to center buttons horizontally
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    // create buttons according to the type
    switch(buttons) {
        case Buttons::OK: {
            GUIDesigns::buildFXButton(buttonsFrame, TL("OK"), "", TL("OK"), GUIIconSubSys::getIcon(GUIIcon::ACCEPT), this, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
            break;
        }
        case Buttons::YES_NO: {
            GUIDesigns::buildFXButton(buttonsFrame, TL("Yes"), "", TL("Yes"), GUIIconSubSys::getIcon(GUIIcon::YES), this, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
            GUIDesigns::buildFXButton(buttonsFrame, TL("No"), "", TL("No"), GUIIconSubSys::getIcon(GUIIcon::NO), this, FXDialogBox::ID_CANCEL, GUIDesignButtonOK);
            break;
        }
        case Buttons::YES_NO_CANCEL: {
            GUIDesigns::buildFXButton(buttonsFrame, TL("Yes"), "", TL("Yes"), GUIIconSubSys::getIcon(GUIIcon::YES), this, FXDialogBox::ID_ACCEPT, GUIDesignButtonAccept);
            GUIDesigns::buildFXButton(buttonsFrame, TL("No"), "", TL("No"), GUIIconSubSys::getIcon(GUIIcon::NO), this, FXDialogBox::ID_CANCEL, GUIDesignButtonAccept);
            GUIDesigns::buildFXButton(buttonsFrame, TL("Cancel"), "", TL("Cancel"), GUIIconSubSys::getIcon(GUIIcon::CANCEL), this, FXDialogBox::ID_CANCEL, GUIDesignButtonCancel);
            // CHEC CANCEL
            break;
        }
        case Buttons::ACCEPT: {
            GUIDesigns::buildFXButton(buttonsFrame, TL("Accept"), "", TL("Accept"), GUIIconSubSys::getIcon(GUIIcon::ACCEPT), this, FXDialogBox::ID_ACCEPT, GUIDesignButtonAccept);
            break;
        }
        case Buttons::ACCEPT_CANCEL: {
            GUIDesigns::buildFXButton(buttonsFrame, TL("Accept"), "", TL("Accept"), GUIIconSubSys::getIcon(GUIIcon::ACCEPT), this, FXDialogBox::ID_ACCEPT, GUIDesignButtonAccept);
            GUIDesigns::buildFXButton(buttonsFrame, TL("Cancel"), "", TL("Cancel"), GUIIconSubSys::getIcon(GUIIcon::CANCEL), this, FXDialogBox::ID_CANCEL, GUIDesignButtonCancel);
            break;
        }
    }
    // add horizontal frame used to center buttons horizontally
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
}


GNEBasicDialog::~GNEBasicDialog() {
}


void
GNEBasicDialog::runInternalTest(const InternalTestStep::DialogArgument* /*dialogArgument*/) {
    // finish runInternalTest
}


long
GNEBasicDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    return closeDialogAccepting();
}


long
GNEBasicDialog::onCmdDecline(FXObject*, FXSelector, void*) {
    return closeDialogDeclining();
}


long
GNEBasicDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    return closeDialogCanceling();
}

/****************************************************************************/
