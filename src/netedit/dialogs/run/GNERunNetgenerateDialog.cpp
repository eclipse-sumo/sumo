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

#include "GNERunNetgenerateDialog.h"

// ============================================-===============================
// member method definitions
// ===========================================================================

GNERunNetgenerateDialog::GNERunNetgenerateDialog(GNEApplicationWindow* applicationWindow, const OptionsCont* netgenerateOptions) :
    GNERunDialog(applicationWindow, new GNERunNetgenerate(applicationWindow, netgenerateOptions, myEvents, myThreadEvent),
                 TL("Running NetGenerate results"), GUIIcon::NETGENERATE) {
}


GNERunNetgenerateDialog::~GNERunNetgenerateDialog() {}


void
GNERunNetgenerateDialog::runInternalTest(const InternalTestStep::DialogArgument* /*dialogArgument*/) {
    // nothing to do
}



long
GNERunNetgenerateDialog::onCmdBack(FXObject*, FXSelector, void*) {
    // close run dialog and open tool dialog
    closeDialogCanceling();
    return myApplicationWindow->handle(this, FXSEL(SEL_COMMAND, MID_GNE_NETGENERATE), nullptr);
}


long
GNERunNetgenerateDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    // close run dialog and call postprocessing
    closeDialogCanceling();
    // abort tool
    myRunner->abort();
    // reset text
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

/****************************************************************************/
