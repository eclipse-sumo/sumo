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

#define MARGING 4

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNERunNetgenerateDialog) GNERunNetgenerateDialogMap[] = {
    FXMAPFUNC(SEL_CLOSE,    0,                      GNERunNetgenerateDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_SAVE,    GNERunNetgenerateDialog::onCmdSaveLog),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_ABORT,   GNERunNetgenerateDialog::onCmdAbort),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_RERUN,   GNERunNetgenerateDialog::onCmdRerun),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_BACK,    GNERunNetgenerateDialog::onCmdBack),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_ACCEPT,  GNERunNetgenerateDialog::onCmdClose),
    // threads events
    FXMAPFUNC(FXEX::SEL_THREAD_EVENT,   ID_LOADTHREAD_EVENT,    GNERunNetgenerateDialog::onThreadEvent),
    FXMAPFUNC(FXEX::SEL_THREAD,         ID_LOADTHREAD_EVENT,    GNERunNetgenerateDialog::onThreadEvent)
};

// Object implementation
FXIMPLEMENT(GNERunNetgenerateDialog, FXDialogBox, GNERunNetgenerateDialogMap, ARRAYNUMBER(GNERunNetgenerateDialogMap))

// ============================================-===============================
// member method definitions
// ===========================================================================

GNERunNetgenerateDialog::GNERunNetgenerateDialog(GNEApplicationWindow* GNEApp) :
    FXDialogBox(GNEApp->getApp(), "", GUIDesignDialogBoxExplicit(0, 0)),
    myGNEApp(GNEApp) {
    // build the thread - io
    myThreadEvent.setTarget(this);
    myThreadEvent.setSelector(ID_LOADTHREAD_EVENT);
    // create run tool
    myRunNetgenerate = new GNERunNetgenerate(this, myEvents, myThreadEvent);
    // set icon
    setIcon(GUIIconSubSys::getIcon(GUIIcon::NETGENERATE));
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


GNERunNetgenerateDialog::~GNERunNetgenerateDialog() {}


GNEApplicationWindow*
GNERunNetgenerateDialog::getGNEApp() const {
    return myGNEApp;
}


void
GNERunNetgenerateDialog::run(const OptionsCont* netgenerateOptions) {
    // set title
    setTitle("Netgenerate output");
    // refresh APP
    getApp()->refresh();
    // clear text
    myText->setText("");
    // show dialog
    FXDialogBox::show(PLACEMENT_SCREEN);
    // set netgenerate options
    myNetgenerateOptions = netgenerateOptions;
    // run tool
    myRunNetgenerate->run(myNetgenerateOptions);
}


void
GNERunNetgenerateDialog::updateDialog() {
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
    // update dialog
    FXDialogBox::update();
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
    // run tool
    myRunNetgenerate->run(myNetgenerateOptions);
    return 1;
}


long
GNERunNetgenerateDialog::onCmdBack(FXObject*, FXSelector, void*) {
    // close run dialog and open tool dialog
    onCmdCancel(nullptr, 0, nullptr);
    return myGNEApp->handle(this, FXSEL(SEL_COMMAND, MID_GNE_NETGENERATE), nullptr);
}


long
GNERunNetgenerateDialog::onCmdClose(FXObject*, FXSelector, void*) {
    // close run dialog and call postprocessing
    onCmdCancel(nullptr, 0, nullptr);
    // call postprocessing dialog
    return myGNEApp->handle(this, FXSEL(SEL_COMMAND, MID_GNE_POSTPROCESSINGNETGENERATE), nullptr);
}


long
GNERunNetgenerateDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // abort tool
    myRunNetgenerate->abort();
    // hide dialog
    hide();
    return 1;
}

long
GNERunNetgenerateDialog::onThreadEvent(FXObject*, FXSelector, void*) {
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

GNERunNetgenerateDialog::GNERunNetgenerateDialog() :
    myGNEApp(nullptr) {
}

/****************************************************************************/
