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
#include <netedit/tools/GNERunPythonTool.h>

#include "GNERunPythonToolDialog.h"

// ============================================-===============================
// member method definitions
// ===========================================================================

GNERunPythonToolDialog::GNERunPythonToolDialog(GNEApplicationWindow* applicationWindow, GNEPythonTool* tool) :
    GNERunDialog(applicationWindow, new GNERunPythonTool(applicationWindow, tool, myEvents, myThreadEvent),
                 TL("Python Tool"), GUIIcon::TOOL_PYTHON) {
}


GNERunPythonToolDialog::~GNERunPythonToolDialog() {}


void
GNERunPythonToolDialog::runInternalTest(const InternalTestStep::DialogArgument* /*dialogTest*/) {
    // finish
}


long
GNERunPythonToolDialog::onCmdBack(FXObject*, FXSelector, void*) {
    // close runTool dialog and open tool dialog
    onCmdClose(nullptr, 0, nullptr);
    return myApplicationWindow->handle(myRunner->getSender(), FXSEL(SEL_COMMAND, MID_GNE_OPENPYTHONTOOLDIALOG), nullptr);
}


long
GNERunPythonToolDialog::onCmdAccept(FXObject* obj, FXSelector, void*) {
    // close run dialog and call postprocessing
    closeDialogCanceling();
    // reset text
    myText->setText("", 0);
    // call postprocessing dialog
    if (myError) {
        return 1;
    } else {
        // execute post processing
        return myApplicationWindow->handle(myRunner->getSender(), FXSEL(SEL_COMMAND, MID_GNE_POSTPROCESSINGPYTHONTOOL), nullptr);
    }
}

/****************************************************************************/
