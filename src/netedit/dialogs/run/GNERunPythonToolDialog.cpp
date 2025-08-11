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


// ============================================-===============================
// member method definitions
// ===========================================================================

GNERunPythonToolDialog::GNERunPythonToolDialog(GNEApplicationWindow* applicationWindow, GNEPythonTool* tool) :
    GNERunDialog(applicationWindow, TL("Python Tool"), GUIIcon::TOOL_PYTHON),
    myRunPythonTool(new GNERunPythonTool(this, myEvents, myThreadEvent)),
    myPythonTool(tool) {
    // open modal dialog
    openDialog();
    // run tool
    myRunPythonTool->run(tool);
}


GNERunPythonToolDialog::~GNERunPythonToolDialog() {}


void
GNERunPythonToolDialog::runInternalTest(const InternalTestStep::DialogArgument* /*dialogTest*/) {
    // finish
}


long
GNERunPythonToolDialog::onCmdAbort(FXObject*, FXSelector, void*) {
    // abort tool
    myRunPythonTool->abort();
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
    myRunPythonTool->run(myPythonTool);
    return 1;
}


long
GNERunPythonToolDialog::onCmdBack(FXObject*, FXSelector, void*) {
    // close runTool dialog and open tool dialog
    onCmdClose(nullptr, 0, nullptr);
    return myApplicationWindow->handle(myPythonTool->getMenuCommand(), FXSEL(SEL_COMMAND, MID_GNE_OPENPYTHONTOOLDIALOG), nullptr);
}


long
GNERunPythonToolDialog::onCmdCancel(FXObject* obj, FXSelector, void*) {
    // abort tool
    myRunPythonTool->abort();
    // hide dialog
    hide();
    return 1;
}


long
GNERunPythonToolDialog::onCmdAccept(FXObject* obj, FXSelector, void*) {
    // abort tool
    myRunPythonTool->abort();
    // execute post processing
    myApplicationWindow->handle(myPythonTool->getMenuCommand(), FXSEL(SEL_COMMAND, MID_GNE_POSTPROCESSINGPYTHONTOOL), nullptr);
    // hide dialog
    hide();
    return 1;
}


void
GNERunPythonToolDialog::updateDialogButtons() {
    // update buttons
    if (myRunPythonTool->isRunning()) {
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
