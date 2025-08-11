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

#include <utils/foxtools/MFXSingleEventThread.h>

// ===========================================================================
// class definitions
// ===========================================================================

class GNERun : protected MFXSingleEventThread {

public:
    /// @brief Constructor
    GNERun(GNEApplicationWindow* applicationWindow, MFXSynchQue<GUIEvent*>& eq, FXEX::MFXThreadEvent& ev);

    /// @brief destructor
    ~GNERun();

    /// @brief get sender (used to inform about events)
    virtual FXObject* getSender() const = 0;

    /// @brief run
    virtual void runThread() = 0;

    /// @brief abort running
    void abort();

    /// @brief check if is running
    bool isRunning() const;

    /// @brief check if during execution an error was Occurred
    bool errorOccurred() const;

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
    FXint run();

    /// @brief Invalidated copy constructor.
    GNERun(const GNERun&) = delete;

    /// @brief Invalidated assignment operator.
    GNERun& operator=(const GNERun&) = delete;
};
