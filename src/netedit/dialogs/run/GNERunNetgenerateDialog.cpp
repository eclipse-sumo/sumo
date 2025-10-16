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
#include <netedit/GNEExternalRunner.h>

#include "GNERunNetgenerateDialog.h"

// ============================================-===============================
// member method definitions
// ===========================================================================

GNERunNetgenerateDialog::GNERunNetgenerateDialog(GNEApplicationWindow* applicationWindow, const OptionsCont* netgenerateOptions) :
    GNERunDialog(applicationWindow, TL("Running netgenerate results"), GUIIcon::NETGENERATE),
    myNetgenerateOptions(netgenerateOptions) {
    // run tool
    applicationWindow->getExternalRunner()->runTool(this);
    // open modal dialog
    openDialog();
}


GNERunNetgenerateDialog::~GNERunNetgenerateDialog() {}


void
GNERunNetgenerateDialog::runInternalTest(const InternalTestStep::DialogArgument* /*dialogArgument*/) {
    // nothing to do
}


std::string
GNERunNetgenerateDialog::getRunCommand() const {
    // set command
#ifdef WIN32
    const std::string exePath = "netgenerate.exe";
#else
    const std::string exePath = "netgenerate";
#endif
    const char* sumoHomeEnv = getenv("SUMO_HOME");
    std::string sumoHome = "";
    if (sumoHomeEnv != nullptr && sumoHomeEnv != std::string("")) {
        sumoHome = std::string(sumoHomeEnv);
        // harmonise slash
        if (sumoHome.back() == '\\') {
            sumoHome = sumoHome.substr(0, sumoHome.size() - 1);
        }
        // prevent double quotes
        if (sumoHome.front() == '"') {
            sumoHome.erase(sumoHome.begin());
        }
        if (sumoHome.size() > 0 && sumoHome.back() == '"') {
            sumoHome.pop_back();
        }
        sumoHome += "/bin/";
    }
    // quote to handle spaces. note that this differs from GNEPythonTool because the python interpreter is a bit smarter
    // when handling quoted parts within a path
    std::string runCommand = "\"" + sumoHome + exePath + "\"";

    // iterate over all topics
    for (const auto& topic : myNetgenerateOptions->getSubTopics()) {
        // ignore configuration
        if (topic != "Configuration") {
            const std::vector<std::string> entries = myNetgenerateOptions->getSubTopicsEntries(topic);
            for (const auto& entry : entries) {
                if (!myNetgenerateOptions->isDefault(entry)) {
                    runCommand += " --" + entry + " \"" + StringUtils::escapeShell(myNetgenerateOptions->getValueString(entry)) + "\" ";
                }
            }
        }
    }
    return runCommand;
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
    myApplicationWindow->getExternalRunner()->abort();
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
