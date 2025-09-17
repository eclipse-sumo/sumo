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
/// @file    GNERunNetgenerateDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2023
///
// Dialog for running tools
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/tools/GNERunNetgenerate.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/events/GUIEvent_Message.h>

#include "GNERunNetgenerateDialog.h"

// ===========================================================================
// Defines
// ===========================================================================

#define MARGIN 4

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNERunNetgenerateDialog) GNERunNetgenerateDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_SAVE,    GNERunNetgenerateDialog::onCmdSaveLog),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_ABORT,   GNERunNetgenerateDialog::onCmdAbort),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_RERUN,   GNERunNetgenerateDialog::onCmdRerun),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_BACK,    GNERunNetgenerateDialog::onCmdBack),
    // threads events
    FXMAPFUNC(FXEX::SEL_THREAD_EVENT,   ID_LOADTHREAD_EVENT,    GNERunNetgenerateDialog::onThreadEvent),
    FXMAPFUNC(FXEX::SEL_THREAD,         ID_LOADTHREAD_EVENT,    GNERunNetgenerateDialog::onThreadEvent)
};

// Object implementation
FXIMPLEMENT(GNERunNetgenerateDialog, GNEDialog, GNERunNetgenerateDialogMap, ARRAYNUMBER(GNERunNetgenerateDialogMap))

// ============================================-===============================
// member method definitions
// ===========================================================================

GNERunNetgenerateDialog::GNERunNetgenerateDialog(GNEApplicationWindow* applicationWindow, const OptionsCont* netgenerateOptions) :
    GNEDialog(applicationWindow, TL("Running netgenerate results"), GUIIcon::NETGENERATE,
              GNEDialog::Buttons::ABORT_RERUN_BACK_CLOSE, OpenType::MODAL,
              GNEDialog::ResizeMode::RESIZABLE, 640, 480) {
    // build the thread - io
    myThreadEvent.setTarget(this);
    myThreadEvent.setSelector(ID_LOADTHREAD_EVENT);
    // create run tool
    myRunNetgenerate = new GNERunNetgenerate(this, myEvents, myThreadEvent);
    // create header frame
    auto headerFrame = new FXHorizontalFrame(myContentFrame, GUIDesignHorizontalFrame);
    // adjust padding
    headerFrame->setPadLeft(0);
    headerFrame->setPadRight(0);
    GUIDesigns::buildFXButton(headerFrame, "", "", + TL("Save output"),
                              GUIIconSubSys::getIcon(GUIIcon::SAVE), this, MID_GNE_BUTTON_SAVE, GUIDesignButtonIcon);
    new FXLabel(headerFrame, TL("Console output"), nullptr, GUIDesignLabelThick(JUSTIFY_LEFT));
    // create text
    auto textFrame = new FXVerticalFrame(myContentFrame, GUIDesignFrameThick);
    myText = new FXText(textFrame, 0, 0, (TEXT_READONLY | LAYOUT_FILL_X | LAYOUT_FILL_Y));
    // set styled
    myText->setHiliteStyles(GUIMessageWindow::getStyles());
    myText->setStyled(true);
    // set netgenerate options
    myNetgenerateOptions = netgenerateOptions;
    // reset error flag
    myError = false;
    // open modal dialog before running netgenerate
    openDialog();
    // run tool
    myRunNetgenerate->run(myNetgenerateOptions);
}


GNERunNetgenerateDialog::~GNERunNetgenerateDialog() {}


void
GNERunNetgenerateDialog::runInternalTest(const InternalTestStep::DialogArgument* /*dialogArgument*/) {
    // nothing to do
}


void
GNERunNetgenerateDialog::updateDialog() {
    /*
        // update buttons
        if (myRunNetgenerate->isRunning()) {
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
    */
    // update dialog
    GNEDialog::update();
}


long
GNERunNetgenerateDialog::onCmdSaveLog(FXObject*, FXSelector, void*) {
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
GNERunNetgenerateDialog::onCmdAbort(FXObject*, FXSelector, void*) {
    // abort tool
    myRunNetgenerate->abort();
    return 1;
}


long
GNERunNetgenerateDialog::onCmdRerun(FXObject*, FXSelector, void*) {
    // add line and info
    std::string line("-------------------------------------------\n");
    myText->appendStyledText(line.c_str(), (int)line.length(), 4, TRUE);
    myText->appendStyledText("rerun tool\n", 1, TRUE);
    myText->layout();
    myText->update();
    myError = false;
    // run tool
    myRunNetgenerate->run(myNetgenerateOptions);
    return 1;
}


long
GNERunNetgenerateDialog::onCmdBack(FXObject*, FXSelector, void*) {
    // close run dialog and open tool dialog
    onCmdCancel(nullptr, 0, nullptr);
    return myApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_GNE_NETGENERATE), nullptr);
}


long
GNERunNetgenerateDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    // close run dialog and call postprocessing
    onCmdCancel(nullptr, 0, nullptr);
    myText->setText("", 0);
    // call postprocessing dialog
    if (myError) {
        return 1;
    } else {
        // don't run this again
        myError = true;
        return myApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_GNE_POSTPROCESSINGNETGENERATE), nullptr);
    }
}


long
GNERunNetgenerateDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // abort tool
    myRunNetgenerate->abort();
    // workaround race conditionat that prevents hiding
    show();
    hide();
    return 1;
}

long
GNERunNetgenerateDialog::onThreadEvent(FXObject*, FXSelector, void*) {
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
        updateDialog();
    }

    if (toolFinished) {
        // check if close dialog immediately after running
        if (myText->getText().find("Error") != -1) {
            myError = true;
        } else if ((myText->getText().find("Success") != -1) && (myText->getText().find("Warning") == -1)) {
            onCmdClose(nullptr, 0, nullptr);
        }
    }

    return 1;
}

/****************************************************************************/
