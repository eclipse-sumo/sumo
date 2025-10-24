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
#include <config.h>

#ifdef HAVE_BOOST
#ifdef _MSC_VER
#pragma warning(push, 0)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <boost/process.hpp>
#include <boost/process/v1/child.hpp>
#include <boost/process/v1/io.hpp>
#pragma warning(pop)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#include <boost/process.hpp>
#include <boost/process/v1/child.hpp>
#include <boost/process/v1/io.hpp>
#pragma GCC diagnostic pop
#endif
#endif

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
// check if use boost version, or the "classic" version
#ifdef HAVE_BOOST
    try {
        // declare both streams for read out and err
        boost::process::v1::ipstream out;
        boost::process::v1::ipstream err;
        // declare run command
        const auto runCommand = myRunDialog->getRunCommand();
        // Show command
        myRunDialog->addEvent(new GUIEvent_Message(GUIEventType::OUTPUT_OCCURRED, runCommand + "\n"), false);
        // run command derivating the std_out to out and std_err to err
        boost::process::v1::child c(runCommand, boost::process::v1::std_out > out, boost::process::v1::std_err > err);
        // declare a stdout reader thread
        std::thread outReaderThread([&out, this]() {
            std::string buffer;
            // read until a \n appears
            while (std::getline(out, buffer)) {
                // clear '\r' character
                if (!buffer.empty() && (buffer.back() == '\r')) {
                    buffer.pop_back();
                }
                myRunDialog->addEvent(new GUIEvent_Message(GUIEventType::OUTPUT_OCCURRED, buffer.c_str()), true);
            }
        });
        // declare a stderr reader thread
        std::thread errReaderThread([&err, this]() {
            std::string buffer;
            // read until a \n appears
            while (std::getline(err, buffer)) {
                // clear '\r' character
                if (!buffer.empty() && (buffer.back() == '\r')) {
                    buffer.pop_back();
                }
                myRunDialog->addEvent(new GUIEvent_Message(GUIEventType::ERROR_OCCURRED, buffer.c_str()), true);
            }
        });
        // wait until child process is finish
        c.wait();
        // use readers for read output
        if (outReaderThread.joinable()) {
            outReaderThread.join();
        }
        if (errReaderThread.joinable()) {
            errReaderThread.join();
        }
        // add a end of line
        myRunDialog->addEvent(new GUIEvent_Message(GUIEventType::OUTPUT_OCCURRED, "\n"), true);
        // return exit code
        return c.exit_code();
    } catch (...) {
        return EXIT_FAILURE;
    }
#else
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
#endif
}

/****************************************************************************/
