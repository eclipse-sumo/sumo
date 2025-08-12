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
/// @file    GNEExternalRunner.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2023
///
// External runner for python and external tools
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/dialogs/run/GNERunDialog.h>
#include <utils/gui/events/GUIEvent_Message.h>

#include "GNEExternalRunner.h"

// ============================================-===============================
// member method definitions
// ===========================================================================

GNEExternalRunner::GNEExternalRunner(GNEApplicationWindow* applicationWindow) :
    MFXSingleEventThread(applicationWindow->getApp(), applicationWindow) {
    // set external runner in application window
    applicationWindow->setExternalRunner(this);
}


GNEExternalRunner::~GNEExternalRunner() {}


void
GNEExternalRunner::runTool(GNERunDialog* runDialog) {
    // first abort any running process
    abort();
    // set run dialog
    myRunDialog = runDialog;
    std::cout << myRunDialog->getRunCommand() << std::endl;
    // set flags
    myRunning = false;
    myErrorOccurred = false;
    // start thread
    start();
}


void
GNEExternalRunner::abort() {
    if (myRunning) {
        // cancel thread
        cancel();
        // reset flags
        myRunning = false;
        myErrorOccurred = false;
        // add event in runDialog
        myRunDialog->addEvent(new GUIEvent_Message(GUIEventType::ERROR_OCCURRED, std::string(TL("cancelled by user\n"))), true);
    }
}


bool
GNEExternalRunner::isRunning() const {
    return myRunning;
}


bool
GNEExternalRunner::errorOccurred() const {
    return myErrorOccurred;
}


FXint
GNEExternalRunner::run() {
    // get run command
    const std::string runCommand = myRunDialog->getRunCommand();
    // declare buffer
    char buffer[128];
    for (int i = 0; i < 128; i++) {
        buffer[i] = '\0';
    }
    // open process showing std::err in console
#ifdef WIN32
    myPipe = _popen(StringUtils::transcodeToLocal(runCommand + " 2>&1").c_str(), "r");
#else
    myPipe = popen((runCommand + " 2>&1").c_str(), "r");
#endif
    if (!myPipe) {
        // set error ocurred flag
        myErrorOccurred = true;
        myRunDialog->addEvent(new GUIEvent_Message(GUIEventType::ERROR_OCCURRED, "popen() failed!"), false);
        myRunDialog->addEvent(new GUIEvent_Message(GUIEventType::TOOL_ENDED, ""), true);
        return 1;
    } else {
        // set running flag
        myRunning = true;
        // Show command
        myRunDialog->addEvent(new GUIEvent_Message(GUIEventType::OUTPUT_OCCURRED, runCommand + "\n"), false);
        // start process
        myRunDialog->addEvent(new GUIEvent_Message(GUIEventType::MESSAGE_OCCURRED, std::string(TL("starting process...\n"))), true);
        try {
            // add buffer
            while (fgets(buffer, sizeof buffer, myPipe) != NULL) {
                myRunDialog->addEvent(new GUIEvent_Message(GUIEventType::OUTPUT_OCCURRED, buffer), true);
            }
        } catch (...) {
            // close process
#ifdef WIN32
            _pclose(myPipe);
#else
            pclose(myPipe);
#endif
            // set flags
            myRunning = false;
            myErrorOccurred = true;
            myRunDialog->addEvent(new GUIEvent_Message(GUIEventType::ERROR_OCCURRED, std::string(TL("error processing command\n"))), true);
            return 1;
        }
    }
    // close process
#ifdef WIN32
    _pclose(myPipe);
#else
    pclose(myPipe);
#endif
    myPipe = nullptr;
    // set running flag
    myRunning = false;
    // end process
    myRunDialog->addEvent(new GUIEvent_Message(GUIEventType::MESSAGE_OCCURRED, std::string(TL("process finished\n"))), false);
    myRunDialog->addEvent(new GUIEvent_Message(GUIEventType::TOOL_ENDED, ""), true);
    return 1;
}

/****************************************************************************/
