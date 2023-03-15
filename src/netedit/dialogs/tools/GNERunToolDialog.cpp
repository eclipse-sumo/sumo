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
/// @file    GNERunToolDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2023
///
// Dialog for running tools
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <utils/common/FileHelpers.h>
#include <utils/foxtools/MFXLabelTooltip.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/handlers/TemplateHandler.h>

#include "GNERunToolDialog.h"
#include "GNERunTool.h"
#include "GNETool.h"


#define MARGING 4

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNERunToolDialog) GNERunToolDialogMap[] = {
    FXMAPFUNC(SEL_CLOSE,    0,                      GNERunToolDialog::onCmdOK),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_ACCEPT,  GNERunToolDialog::onCmdOK),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_RESET,   GNERunToolDialog::onCmdRerun),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_SAVE,    GNERunToolDialog::onCmdSaveLog)
};

// Object implementation
FXIMPLEMENT(GNERunToolDialog, FXDialogBox, GNERunToolDialogMap, ARRAYNUMBER(GNERunToolDialogMap))

// ============================================-===============================
// member method definitions
// ===========================================================================

GNERunToolDialog::GNERunToolDialog(GNEApplicationWindow* GNEApp) :
    FXDialogBox(GNEApp->getApp(), "Tool", GUIDesignDialogBoxExplicit(0, 0)),
    myGNEApp(GNEApp) {
    // create run tool
    myRunTool = new GNERunTool(this);
    // set icon
    setIcon(GUIIconSubSys::getIcon(GUIIcon::TOOL_PYTHON));
    // create content frame
    auto contentFrame = new FXVerticalFrame(this, GUIDesignAuxiliarFrame);
    // create header frame
    auto headerFrame = new FXHorizontalFrame(contentFrame, GUIDesignHorizontalFrame);
    // adjust padding
    headerFrame->setPadLeft(0);
    headerFrame->setPadRight(0);
    new FXButton(headerFrame, (std::string("\t\t") + TL("Save output")).c_str(), 
        GUIIconSubSys::getIcon(GUIIcon::SAVE), this, MID_GNE_BUTTON_SAVE, GUIDesignButtonIcon);
    new FXLabel(headerFrame, TL("Console output"), nullptr, GUIDesignLabelThick(JUSTIFY_LEFT));
    // create text
    auto textFrame = new FXVerticalFrame(contentFrame, GUIDesignFrameThick);
    myText = new FXText(textFrame, 0, 0, (TEXT_READONLY | LAYOUT_FILL_X | LAYOUT_FILL_Y));
    // set styled
    myText->setHiliteStyles(GUIMessageWindow::getStyles());
    myText->setStyled(true);
    // create buttons centered
    auto buttonsFrame = new FXHorizontalFrame(contentFrame, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    new FXButton(buttonsFrame, (TL("OK") + std::string("\t\t") + TL("close dialog")).c_str(),
        GUIIconSubSys::getIcon(GUIIcon::OK), this, MID_GNE_BUTTON_ACCEPT, GUIDesignButtonAccept);
    new FXButton(buttonsFrame, (TL("Rerun") + std::string("\t\t") + TL("rerun tool")).c_str(), 
        GUIIconSubSys::getIcon(GUIIcon::RESET),  this, MID_GNE_BUTTON_RESET,  GUIDesignButtonReset);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    // resize
    resize(640, 480);
}


GNERunToolDialog::~GNERunToolDialog() {}


GNEApplicationWindow*
GNERunToolDialog::getGNEApp() const {
    return myGNEApp;
}


void
GNERunToolDialog::runTool(GNETool* tool) {
    // set title
    setTitle((tool->getToolName()  + " output").c_str());
    // refresh APP
    getApp()->refresh();
    // clear text
    myText->setText("");
    // show dialog
    FXDialogBox::show(PLACEMENT_SCREEN);
    // run tool
    myRunTool->runTool(tool);
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    myGNEApp->getApp()->runModalFor(this);
}


void
GNERunToolDialog::appendInfoMessage(const std::string text) {
    myText->appendStyledText(text.c_str(), text.length(), 2, TRUE);
    myText->layout();
    myText->update();
}


void
GNERunToolDialog::appendErrorMessage(const std::string text) {
    myText->appendStyledText(text.c_str(), text.length(), 3, TRUE);
    myText->layout();
    myText->update();
}


void
GNERunToolDialog::appendBuffer(const char *buffer) {
    FXString FXText(buffer);
    myText->appendStyledText(FXText, 1, TRUE);
    myText->layout();
    myText->update();
}


long
GNERunToolDialog::onCmdOK(FXObject*, FXSelector, void*) {
    // stop modal
    myGNEApp->getApp()->stopModal(this);
    // hide dialog
    hide();
    return 1;
}


long
GNERunToolDialog::onCmdRerun(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNERunToolDialog::onCmdSaveLog(FXObject*, FXSelector, void*) {
    return 1;
}


GNERunToolDialog::GNERunToolDialog() :
    myGNEApp(nullptr) {
}

/****************************************************************************/
