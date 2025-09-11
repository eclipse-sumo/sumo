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
/// @file    GNERunDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2025
///
// Abstract dialog for running tools
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNEExternalRunner.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/events/GUIEvent_Message.h>

#include "GNERunDialog.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNERunDialog) GNERunDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_BUTTON_SAVE,    GNERunDialog::onCmdSaveLog),
    FXMAPFUNC(FXEX::SEL_THREAD_EVENT,   ID_LOADTHREAD_EVENT,    GNERunDialog::onThreadEvent),
    FXMAPFUNC(FXEX::SEL_THREAD,         ID_LOADTHREAD_EVENT,    GNERunDialog::onThreadEvent)
};

// Object implementation
FXIMPLEMENT_ABSTRACT(GNERunDialog, GNEDialog, GNERunDialogMap, ARRAYNUMBER(GNERunDialogMap))

// ============================================-===============================
// member method definitions
// ===========================================================================

GNERunDialog::GNERunDialog(GNEApplicationWindow* applicationWindow, const std::string& name, GUIIcon titleIcon) :
    GNEDialog(applicationWindow, name, titleIcon, DialogType::RUN, GNEDialog::Buttons::RERUN_BACK_CLOSE,
              OpenType::MODAL, GNEDialog::ResizeMode::RESIZABLE, 640, 480) {
    // build the thread - io
    myThreadEvent.setTarget(this);
    myThreadEvent.setSelector(ID_LOADTHREAD_EVENT);
    // create header frame
    auto headerFrame = new FXHorizontalFrame(myContentFrame, GUIDesignHorizontalFrame);
    // adjust padding
    headerFrame->setPadLeft(0);
    headerFrame->setPadRight(0);
    GUIDesigns::buildFXButton(headerFrame, "", "", + TL("Save output"), GUIIconSubSys::getIcon(GUIIcon::SAVE),
                              this, MID_GNE_BUTTON_SAVE, GUIDesignButtonIcon);
    new FXLabel(headerFrame, TL("Console output"), nullptr, GUIDesignLabelThick(JUSTIFY_LEFT));
    // create text
    auto textFrame = new FXVerticalFrame(myContentFrame, GUIDesignFrameThick);
    myText = new FXText(textFrame, 0, 0, (TEXT_READONLY | LAYOUT_FILL_X | LAYOUT_FILL_Y));
    // set styled
    myText->setHiliteStyles(GUIMessageWindow::getStyles());
    myText->setStyled(true);
    // update dialog button
    updateDialogButtons();
}


GNERunDialog::~GNERunDialog() {}


void
GNERunDialog::addEvent(GUIEvent* event, const bool signal) {
    // add event to queue
    myEvents.push_back(event);
    // signal thread event
    if (signal) {
        myThreadEvent.signal();
    }
}


long
GNERunDialog::onCmdAbort(FXObject*, FXSelector, void*) {
    // abort external runner
    myApplicationWindow->getExternalRunner()->abort();
    // hide dialog
    return closeDialogCanceling();
}


long
GNERunDialog::onCmdRun(FXObject*, FXSelector, void*) {
    if (myApplicationWindow->getExternalRunner()->isRunning()) {
        // abort external runner
        myApplicationWindow->getExternalRunner()->abort();
    } else {
        // add line and info
        std::string line("-------------------------------------------\n");
        myText->appendStyledText(line.c_str(), (int)line.length(), 4, TRUE);
        myText->appendStyledText("rerun tool\n", 1, TRUE);
        myText->layout();
        myText->update();
        myError = false;
        // abort external runner
        myApplicationWindow->getExternalRunner()->runTool(this);
    }
    // update dialog button
    updateDialogButtons();
    return 1;
}


long
GNERunDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    // close run dialog and call postprocessing
    closeDialogAccepting();
    // reset text
    myText->setText("", 0);
    // call postprocessing dialog depending of myError
    if (myError) {
        return 1;
    } else {
        // don't run this again
        myError = true;
        return myApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_GNE_POSTPROCESSINGNETGENERATE), nullptr);
    }
}


long
GNERunDialog::onCmdSaveLog(FXObject*, FXSelector, void*) {
    // create fileDialog
    const auto saveLogFileDialog = GNEFileDialog(myApplicationWindow,
                                   TL("tool log file"),
                                   SUMOXMLDefinitions::TXTFileExtensions.getStrings(),
                                   GNEFileDialog::OpenMode::SAVE,
                                   GNEFileDialog::ConfigType::NETEDIT);
    // check that file is valid
    if (saveLogFileDialog.getResult() == GNEDialog::Result::ACCEPT) {
        OutputDevice& dev = OutputDevice::getDevice(saveLogFileDialog.getFilename());
        dev << myText->getText().text();
        dev.close();
    }
    return 1;
}


long
GNERunDialog::onThreadEvent(FXObject*, FXSelector, void*) {
    bool toolFinished = false;
    while (!myEvents.empty()) {
        // get the next event
        GUIEvent* e = myEvents.top();
        myEvents.pop();
        // process
        FXint style = -1;
        switch (e->getOwnType()) {
            case GUIEventType::TOOL_ENDED:
                toolFinished = true;
                break;
            case GUIEventType::MESSAGE_OCCURRED:
                style = 1;
                break;
            case GUIEventType::OUTPUT_OCCURRED:
                style = 2;
                break;
            case GUIEventType::ERROR_OCCURRED:
                style = 3;
                myError = true;
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
    }
    if (toolFinished) {
        // check if close dialog immediately after running
        if (myText->getText().find("Error") != -1) {
            myError = true;
        } else if ((myText->getText().find("Success") != -1) && (myText->getText().find("Warning") == -1)) {
            //onCmdClose(nullptr, 0, nullptr);
        }
    }
    updateDialogButtons();
    return 1;
}


void
GNERunDialog::updateDialogButtons() {
    // update buttons
    if (myApplicationWindow->getExternalRunner()->isRunning()) {
        // update run button
        myRunButton->setText(TL("Abort"));
        myRunButton->setTipText(TL("Abort running"));
        myRunButton->setIcon(GUIIconSubSys::getIcon(GUIIcon::STOP));
        // disable buttons
        myBackButton->disable();
        myAcceptButton->disable();
    } else {
        // update run button
        myRunButton->setText(TL("Rerun"));
        myRunButton->setTipText(TL("Rerun tool"));
        myRunButton->setIcon(GUIIconSubSys::getIcon(GUIIcon::START));
        // enable buttons
        myBackButton->enable();
        myAcceptButton->enable();
    }
    // update dialog
    GNEDialog::update();
}

/****************************************************************************/
