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

#include "GNEPythonTool.h"
#include "GNERunPythonTool.h"
#include "GNERunPythonToolDialog.h"

// ============================================-===============================
// member method definitions
// ===========================================================================

GNERunPythonTool::GNERunPythonTool(GNERunPythonToolDialog* runToolDialog) :
    MFXSingleEventThread(runToolDialog->getGNEApp()->getApp(), runToolDialog->getGNEApp()),
    myRunPythonToolDialog(runToolDialog) {
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
    // show info
    myRunPythonToolDialog->appendErrorMessage(TL("cancelled by user\n"));
    // reset flags
    myRunning = false;
    myErrorOccurred = false;
    myRunPythonToolDialog->updateDialog();
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
        myRunPythonToolDialog->appendErrorMessage(TL("popen() failed!"));
        // set error ocurred flag
        myErrorOccurred = true;
        myRunPythonToolDialog->updateDialog();
        return 1;
    } else {
        // set running flag
        myRunning = true;
        myRunPythonToolDialog->updateDialog();
        // Show command
        myRunPythonToolDialog->appendBuffer((myPythonTool->getCommand() + "\n").c_str());
        // start process
        myRunPythonToolDialog->appendInfoMessage(TL("starting process...\n"));
        try {
            // add buffer
            while (fgets(buffer, sizeof buffer, myPipe) != NULL) {
                myRunPythonToolDialog->appendBuffer(buffer);
            }
            myRunPythonToolDialog->appendBuffer(buffer);
        } catch (...) {
            // close process
        #ifdef WIN32
            _pclose(myPipe);
        #else
            pclose(myPipe);
        #endif
            myRunPythonToolDialog->appendErrorMessage(TL("error processing command\n"));
            // set flags
            myRunning = false;
            myErrorOccurred = true;
            myRunPythonToolDialog->updateDialog();
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
    // end process
    myRunPythonToolDialog->appendInfoMessage(TL("process finished\n"));
    // set running flag
    myRunning = false;
    myRunPythonToolDialog->updateDialog();
    return 1;
}

/****************************************************************************/
