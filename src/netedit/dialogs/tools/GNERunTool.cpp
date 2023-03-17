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
/// @file    GNERunTool.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2023
///
// Thread for run tool
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>

#include "GNETool.h"
#include "GNERunTool.h"
#include "GNERunToolDialog.h"

// ============================================-===============================
// member method definitions
// ===========================================================================

GNERunTool::GNERunTool(GNERunToolDialog* runToolDialog) :
    MFXSingleEventThread(runToolDialog->getGNEApp()->getApp(), runToolDialog->getGNEApp()),
    myRunToolDialog(runToolDialog) {
}


GNERunTool::~GNERunTool() {}


void
GNERunTool::runTool(const GNETool* tool) {
    myTool = tool;
    start();
}


FXint
GNERunTool::run() {
    // declare buffer
    char buffer[128];
    for (int i = 0; i < 128; i++) {
        buffer[i] = '\0';
    }
    // open process showing std::err in console
#ifdef WIN32
    FILE* pipe = _popen((myTool->getCommand() + " 2>&1").c_str(), "r");
#else
    FILE* pipe = popen((myTool->getCommand() + " 2>&1").c_str(), "r");
#endif 
    if (!pipe) {
        myRunToolDialog->appendErrorMessage(TL("popen() failed!"));
        return 1;
    } else {
        // Show command
        myRunToolDialog->appendInfoMessage(TL("command\n"));
        myRunToolDialog->appendBuffer((myTool->getCommand() + "\n").c_str());
        // start process
        myRunToolDialog->appendInfoMessage(TL("starting process...\n"));
        try {
            // add buffer
            while (fgets(buffer, sizeof buffer, pipe) != NULL) {
                myRunToolDialog->appendBuffer(buffer);
            }
            myRunToolDialog->appendBuffer(buffer);
        } catch (...) {
            // close process
        #ifdef WIN32
            _pclose(pipe);
        #else
            pclose(pipe);
        #endif
            myRunToolDialog->appendErrorMessage(TL("error processing command\n"));
            return 1;
        }
    }
    // close process
#ifdef WIN32
    _pclose(pipe);
#else
    pclose(pipe);
#endif
    // end process
    myRunToolDialog->appendInfoMessage(TL("process finished\n"));
    return 1;
}

/****************************************************************************/
