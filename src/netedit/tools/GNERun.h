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
/// @file    GNERun.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2025
///
// abstract class for running tools
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/dialogs/run/GNERunDialog.h>
#include <utils/foxtools/MFXSingleEventThread.h>

// ===========================================================================
// class definitions
// ===========================================================================

template <typename T>
class GNERun : protected MFXSingleEventThread {

public:
    /// @brief Constructor
    GNERun(GNERunDialog* runDialog, MFXSynchQue<GUIEvent*>& eq, FXEX::MFXThreadEvent& ev) :
        MFXSingleEventThread(runDialog->getApplicationWindow()->getApp(), runDialog->getApplicationWindow()),
        myEventQueue(eq),
        myEventThrow(ev) {
    }

    /// @brief destructor
    ~GNERun() {}

    /// @brief run
    virtual void run(const T* arguments) = 0;

    /// @brief abort running
    void abort() {
        if (myRunning) {
            // cancel thread
            cancel();
            // reset flags
            myRunning = false;
            myErrorOccurred = false;
            // show info
            myEventQueue.push_back(new GUIEvent_Message(GUIEventType::ERROR_OCCURRED, std::string(TL("cancelled by user\n"))));
            myEventThrow.signal();
        }
    }

    /// @brief check if is running
    bool isRunning() const {
        return myRunning;
    }

    /// @brief check if during execution an error was Occurred
    bool errorOccurred() const {
        return myErrorOccurred;
    }

protected:
    /// @brief running command
    std::string myRunCommand;

    /// @brief flag for check if is running
    bool myRunning = false;

    /// @brief flag for check if during execution an error was Occurred
    bool myErrorOccurred = false;

private:
    /// @brief pipe file
    FILE* myPipe = nullptr;

    /// @brief event Queue
    MFXSynchQue<GUIEvent*>& myEventQueue;

    /// @brief event throw
    FXEX::MFXThreadEvent& myEventThrow;

    /// @brief starts the thread. The thread ends after the tool is finished
    FXint run() {
        // declare buffer
        char buffer[128];
        for (int i = 0; i < 128; i++) {
            buffer[i] = '\0';
        }
        // open process showing std::err in console
#ifdef WIN32
        myPipe = _popen(StringUtils::transcodeToLocal(myRunCommand + " 2>&1").c_str(), "r");
#else
        myPipe = popen((myRunCommand + " 2>&1").c_str(), "r");
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
            myEventQueue.push_back(new GUIEvent_Message(GUIEventType::OUTPUT_OCCURRED, myRunCommand + "\n"));
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
        myEventQueue.push_back(new GUIEvent_Message(GUIEventType::TOOL_ENDED, ""));
        myEventThrow.signal();
        return 1;
    }

    /// @brief Invalidated copy constructor.
    GNERun(const GNERun&) = delete;

    /// @brief Invalidated assignment operator.
    GNERun& operator=(const GNERun&) = delete;
};
