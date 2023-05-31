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
/// @file    GNERunPythonTool.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2023
///
// Thread for run tool
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/dialogs/tools/GNERunPythonToolDialog.h>
#include <utils/gui/events/GUIEvent_Message.h>

#include "GNEPythonTool.h"
#include "GNERunPythonTool.h"

// ============================================-===============================
// member method definitions
// ===========================================================================

GNERunPythonTool::GNERunPythonTool(GNERunPythonToolDialog* runToolDialog, MFXSynchQue<GUIEvent*>& eq, FXEX::MFXThreadEvent& ev) :
    MFXSingleEventThread(runToolDialog->getGNEApp()->getApp(), runToolDialog->getGNEApp()),
    myEventQueue(eq),
    myEventThrow(ev) {
}


GNERunPythonTool::~GNERunPythonTool() {}


void
GNERunPythonTool::runTool(const GNEPythonTool* tool) {
    myPythonTool = tool;
    // reset flags
    myRunning = false;
    myErrorOccurred = false;
    start();
}


void
GNERunPythonTool::abortTool() {
    // cancel thread
    cancel();
    // reset flags
    myRunning = false;
    myErrorOccurred = false;
    // show info
    myEventQueue.push_back(new GUIEvent_Message(GUIEventType::ERROR_OCCURRED, std::string(TL("cancelled by user\n"))));
    myEventThrow.signal();
}


bool
GNERunPythonTool::isRunning() const {
    return myRunning;
}


bool
GNERunPythonTool::errorOccurred() const {
    return myErrorOccurred;
}


FXint
GNERunPythonTool::run() {
    // declare buffer
    char buffer[128];
    for (int i = 0; i < 128; i++) {
        buffer[i] = '\0';
    }
    // open process showing std::err in console
#ifdef WIN32
    myPipe = _popen((myPythonTool->getCommand() + " 2>&1").c_str(), "r");
#else
    myPipe = popen((myPythonTool->getCommand() + " 2>&1").c_str(), "r");
#endif
    if (!myPipe) {
        // set error ocurred flag
        myErrorOccurred = true;
        myEventQueue.push_back(new GUIEvent_Message(GUIEventType::ERROR_OCCURRED, "popen() failed!"));
        myEventQueue.push_back(new GUIEvent_Message(GUIEventType::TOOL_ENDED, ""));
        myEventThrow.signal();
        return 1;
    } else {
        // set running flag
        myRunning = true;
        // Show command
        myEventQueue.push_back(new GUIEvent_Message(GUIEventType::OUTPUT_OCCURRED, myPythonTool->getCommand()));
        // start process
        myEventQueue.push_back(new GUIEvent_Message(GUIEventType::MESSAGE_OCCURRED, std::string(TL("starting process...\n"))));
        myEventThrow.signal();
        try {
            // add buffer
            while (fgets(buffer, sizeof buffer, myPipe) != NULL) {
                myEventQueue.push_back(new GUIEvent_Message(GUIEventType::OUTPUT_OCCURRED, buffer));
                myEventThrow.signal();
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
            myEventQueue.push_back(new GUIEvent_Message(GUIEventType::ERROR_OCCURRED, std::string(TL("error processing command\n"))));
            myEventThrow.signal();
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
    myEventQueue.push_back(new GUIEvent_Message(GUIEventType::MESSAGE_OCCURRED, std::string(TL("process finished\n"))));
    myEventThrow.signal();
    return 1;
}

/****************************************************************************/
