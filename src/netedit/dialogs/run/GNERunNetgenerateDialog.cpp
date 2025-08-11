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

// ============================================-===============================
// member method definitions
// ===========================================================================

GNERunNetgenerateDialog::GNERunNetgenerateDialog(GNEApplicationWindow* applicationWindow, const OptionsCont* netgenerateOptions) :
    GNERunDialog(applicationWindow, TL("Running NetGenerate results"), GUIIcon::NETGENERATE),
    myRunNetgenerate(new GNERunNetgenerate(this, myEvents, myThreadEvent)),
    myNetgenerateOptions(netgenerateOptions) {
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


void
GNERunNetgenerateDialog::updateDialogButtons() {
    // update buttons
    if (myRunNetgenerate->isRunning()) {
        myAbortButton->enable();
        myRunButton->disable();
        myBackButton->disable();
        myCancelButton->disable();
    } else {
        myAbortButton->disable();
        myRunButton->enable();
        myBackButton->enable();
        myCancelButton->enable();
    }
    // update dialog
    GNEDialog::update();
}

/****************************************************************************/
