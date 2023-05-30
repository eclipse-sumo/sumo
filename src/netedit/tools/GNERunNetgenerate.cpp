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
/// @file    GNERunNetgenerate.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2023
///
// Thread for run netgenerate tool
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/dialogs/tools/GNERunNetgenerateDialog.h>
#include <utils/gui/events/GUIEvent_Message.h>

#include "GNERunNetgenerate.h"

// ============================================-===============================
// member method definitions
// ===========================================================================

GNERunNetgenerate::GNERunNetgenerate(GNERunNetgenerateDialog* runDialog, MFXSynchQue<GUIEvent*>& eq, FXEX::MFXThreadEvent& ev) :
    MFXSingleEventThread(runDialog->getGNEApp()->getApp(), runDialog->getGNEApp()),
    myEventQueue(eq),
    myEventThrow(ev) {
}


GNERunNetgenerate::~GNERunNetgenerate() {}


void
GNERunNetgenerate::run(const OptionsCont* netgenerateOptions) {
    // set command
#ifdef WIN32
    myNetgenerateCommand = getenv("SUMO_HOME") + std::string("/bin/netgenerate.exe");
#else
    myNetgenerateCommand = getenv("SUMO_HOME") + std::string("/bin/netgenerate");
#endif
    // iterate over all topics
    for (const auto& topic : netgenerateOptions->getSubTopics()) {
        // ignore configuration
        if (topic != "Configuration") {
            const std::vector<std::string> entries = netgenerateOptions->getSubTopicsEntries(topic);
            for (const auto& entry : entries) {
                if (!netgenerateOptions->isDefault(entry)) {
                    myNetgenerateCommand += " --" + entry + " " + netgenerateOptions->getValueString(entry);
                }
            }
        }
    }
    // reset flags
    myRunning = false;
    myErrorOccurred = false;
    start();
}


void
GNERunNetgenerate::abort() {
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
GNERunNetgenerate::isRunning() const {
    return myRunning;
}


bool
GNERunNetgenerate::errorOccurred() const {
    return myErrorOccurred;
}


FXint
GNERunNetgenerate::run() {
    // declare buffer
    char buffer[128];
    for (int i = 0; i < 128; i++) {
        buffer[i] = '\0';
    }
    // open process showing std::err in console
#ifdef WIN32
    myPipe = _popen((myNetgenerateCommand + " 2>&1").c_str(), "r");
#else
    myPipe = popen((myNetgenerateCommand + " 2>&1").c_str(), "r");
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
        myEventQueue.push_back(new GUIEvent_Message(GUIEventType::OUTPUT_OCCURRED, myNetgenerateCommand + "\n"));
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
