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
/// @file    GNERunNetgenerate.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2023
///
// Thread for run tool
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/MFXSingleEventThread.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNERunNetgenerateDialog;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNERunNetgenerate
 * @brief Abstract dialog for tools
 */
class GNERunNetgenerate : protected MFXSingleEventThread  {

public:
    /// @brief Constructor
    GNERunNetgenerate(GNERunNetgenerateDialog* runDialog);

    /// @brief destructor
    ~GNERunNetgenerate();
    
    /// @brief run tool
    void run(const GNENetgenerate* tool);

    /// @brief abort tool running
    void abort();

    /// @brief check if tool is running
    bool isRunning() const;

    /// @brief check if during execution an error was Occurred
    bool errorOccurred() const;

private:
    /// @brief pointer to run tool dialog
    GNERunNetgenerateDialog* myRunNetgenerateDialog;

    /// @brief tool
    const GNENetgenerate* myNetgenerate = nullptr;

    /// @brief flag for check if tool is running
    bool myRunning = false;

    /// @brief flag for check if during execution an error was Occurred
    bool myErrorOccurred = false;

    /// @brief pipe file
    FILE* myPipe = nullptr;

    /// @brief starts the thread. The thread ends after the tool is finished
    FXint run();

    /// @brief Invalidated copy constructor.
    GNERunNetgenerate(const GNERunNetgenerate&) = delete;

    /// @brief Invalidated assignment operator.
    GNERunNetgenerate& operator=(const GNERunNetgenerate&) = delete;
};

