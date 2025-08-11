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
/// @file    GNERunNetgenerate.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2023
///
// Thread for run netgenerate tool
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>

#include "GNERunNetgenerate.h"

// ============================================-===============================
// member method definitions
// ===========================================================================

GNERunNetgenerate::GNERunNetgenerate(GNEApplicationWindow* applicationWindow, const OptionsCont* netgenerateOptions,
                                     MFXSynchQue<GUIEvent*>& eq, FXEX::MFXThreadEvent& ev) :
    GNERun(applicationWindow, eq, ev),
    myNetgenerateOptions(netgenerateOptions) {
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
    myNetGenerateExecutablePath = "\"" + sumoHome + exePath + "\"";
}


GNERunNetgenerate::~GNERunNetgenerate() {}


FXObject*
GNERunNetgenerate::getSender() const {
    return nullptr;
}


void
GNERunNetgenerate::runThread() {
    // first set executable path
    myRunCommand = myNetGenerateExecutablePath;
    // iterate over all topics
    for (const auto& topic : myNetgenerateOptions->getSubTopics()) {
        // ignore configuration
        if (topic != "Configuration") {
            const std::vector<std::string> entries = myNetgenerateOptions->getSubTopicsEntries(topic);
            for (const auto& entry : entries) {
                if (!myNetgenerateOptions->isDefault(entry)) {
                    myRunCommand += " --" + entry + " " + myNetgenerateOptions->getValueString(entry);
                }
            }
        }
    }
    // reset flags
    myRunning = false;
    myErrorOccurred = false;
    // start thread
    start();
}

/****************************************************************************/
