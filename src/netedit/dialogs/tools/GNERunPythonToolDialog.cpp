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
/// @file    GNERunPythonToolDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2023
///
// Dialog for running tools
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/tools/GNEPythonTool.h>
#include <netedit/tools/GNERunPythonTool.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/events/GUIEvent_Message.h>

#include "GNERunPythonToolDialog.h"

#define MARGING 4

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNERunPythonToolDialog) GNERunPythonToolDialogMap[] = {
    FXMAPFUNC(SEL_CLOSE,    0,                      GNERunPythonToolDialog::onCmdClose),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_SAVE,    GNERunPythonToolDialog::onCmdSaveLog),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_ABORT,   GNERunPythonToolDialog::onCmdAbort),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_RERUN,   GNERunPythonToolDialog::onCmdRerun),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_BACK,    GNERunPythonToolDialog::onCmdBack),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_ACCEPT,  GNERunPythonToolDialog::onCmdClose),
    // threads events
    FXMAPFUNC(FXEX::SEL_THREAD_EVENT,   ID_LOADTHREAD_EVENT,    GNERunPythonToolDialog::onThreadEvent),
    FXMAPFUNC(FXEX::SEL_THREAD,         ID_LOADTHREAD_EVENT,    GNERunPythonToolDialog::onThreadEvent)
};

// Object implementation
FXIMPLEMENT(GNERunPythonToolDialog, FXDialogBox, GNERunPythonToolDialogMap, ARRAYNUMBER(GNERunPythonToolDialogMap))

// ============================================-===============================
// member method definitions
// ===========================================================================

GNERunPythonToolDialog::GNERunPythonToolDialog(GNEApplicationWindow* GNEApp) :
    FXDialogBox(GNEApp->getApp(), "Tool", GUIDesignAuxiliarDialogBoxResizable),
    myGNEApp(GNEApp) {
    // build the thread - io
    myThreadEvent.setTarget(this);
    myThreadEvent.setSelector(ID_LOADTHREAD_EVENT);
    // create run tool
    myRunTool = new GNERunPythonTool(this, myEvents, myThreadEvent);
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
    // create buttons Abort, rerun and back
    auto buttonsFrame = new FXHorizontalFrame(contentFrame, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    myAbortButton = new FXButton(buttonsFrame, (TL("Abort") + std::string("\t\t") + TL("abort running")).c_str(),
                                 GUIIconSubSys::getIcon(GUIIcon::STOP), this, MID_GNE_BUTTON_ABORT, GUIDesignButtonAccept);
    myRerunButton = new FXButton(buttonsFrame, (TL("Rerun") + std::string("\t\t") + TL("rerun tool")).c_str(),
                                 GUIIconSubSys::getIcon(GUIIcon::RESET),  this, MID_GNE_BUTTON_RERUN,  GUIDesignButtonReset);
    myBackButton = new FXButton(buttonsFrame, (TL("Back") + std::string("\t\t") + TL("back to tool dialog")).c_str(),
                                GUIIconSubSys::getIcon(GUIIcon::BACK), this, MID_GNE_BUTTON_BACK, GUIDesignButtonAccept);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    // add separator
    new FXSeparator(contentFrame);
    // create button ok
    buttonsFrame = new FXHorizontalFrame(contentFrame, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    myCloseButton = new FXButton(buttonsFrame, (TL("Close") + std::string("\t\t") + TL("close dialog")).c_str(),
                                 GUIIconSubSys::getIcon(GUIIcon::OK), this, MID_GNE_BUTTON_ACCEPT, GUIDesignButtonAccept);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    // resize
    resize(640, 480);
}


GNERunPythonToolDialog::~GNERunPythonToolDialog() {}


GNEApplicationWindow*
GNERunPythonToolDialog::getGNEApp() const {
    return myGNEApp;
}


void
GNERunPythonToolDialog::runTool(GNEPythonTool* tool) {
    // set title
    setTitle((tool->getToolName()  + " output").c_str());
    // refresh APP
    getApp()->refresh();
    // clear text
    myText->setText("");
    // show dialog
    FXDialogBox::show(PLACEMENT_SCREEN);
    // set tool
    myPythonTool = tool;
    // run tool
    myRunTool->runTool(tool);
}


void
GNERunPythonToolDialog::updateDialog() {
    // update buttons
    if (myRunTool->isRunning()) {
        myAbortButton->enable();
        myRerunButton->disable();
        myBackButton->disable();
        myCloseButton->disable();
    } else {
        myAbortButton->disable();
        myRerunButton->enable();
        myBackButton->enable();
        myCloseButton->enable();
    }
    // update dialog
    FXDialogBox::update();
}


long
GNERunPythonToolDialog::onCmdSaveLog(FXObject*, FXSelector, void*) {
    // get log file
    const auto logFile = GNEApplicationWindowHelper::saveToolLog(this);
    // check that file is valid
    if (logFile.size() > 0) {
        OutputDevice& dev = OutputDevice::getDevice(logFile);
        dev << myText->getText().text();
        dev.close();
    }
    return 1;
}


long
GNERunPythonToolDialog::onCmdAbort(FXObject*, FXSelector, void*) {
    // abort tool
    myRunTool->abortTool();
    return 1;
}


long
GNERunPythonToolDialog::onCmdRerun(FXObject*, FXSelector, void*) {
    // add line and info
    std::string line("-------------------------------------------\n");
    myText->appendStyledText(line.c_str(), (int)line.length(), 4, TRUE);
    myText->appendStyledText("rerun tool\n", 1, TRUE);
    myText->layout();
    myText->update();
    // run tool
    myRunTool->runTool(myPythonTool);
    return 1;
}


long
GNERunPythonToolDialog::onCmdBack(FXObject*, FXSelector, void*) {
    // close runTool dialog and open tool dialog
    onCmdClose(nullptr, 0, nullptr);
    return myGNEApp->handle(myPythonTool->getMenuCommand(), FXSEL(SEL_COMMAND, MID_GNE_OPENPYTHONTOOLDIALOG), nullptr);
}


long
GNERunPythonToolDialog::onCmdClose(FXObject* obj, FXSelector, void*) {
    // abort tool
    myRunTool->abortTool();
    // check if we're closing using "close" button
    if (obj == myCloseButton) {
        // execute post processing
        myGNEApp->handle(myPythonTool->getMenuCommand(), FXSEL(SEL_COMMAND, MID_GNE_POSTPROCESSINGPYTHONTOOL), nullptr);
    }
    // hide dialog
    hide();
    return 1;
}

long
GNERunPythonToolDialog::onThreadEvent(FXObject*, FXSelector, void*) {
    while (!myEvents.empty()) {
        // get the next event
        GUIEvent* e = myEvents.top();
        myEvents.pop();
        // process
        FXint style = -1;
        switch (e->getOwnType()) {
            case GUIEventType::TOOL_ENDED:
                break;
            case GUIEventType::MESSAGE_OCCURRED:
                style = 1;
                break;
            case GUIEventType::OUTPUT_OCCURRED:
                style = 2;
                break;
            case GUIEventType::ERROR_OCCURRED:
                style = 3;
                break;
            default:
                break;
        }
        if (style >= 0) {
            GUIEvent_Message* ec = static_cast<GUIEvent_Message*>(e);
            myText->appendStyledText(ec->getMsg().c_str(), (int)ec->getMsg().length(), style, TRUE);
            myText->layout();
            myText->update();
        }
        delete e;
        updateDialog();
    }
    return 1;
}


GNERunPythonToolDialog::GNERunPythonToolDialog() :
    myGNEApp(nullptr) {
}

/****************************************************************************/
