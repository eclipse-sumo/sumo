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
/// @file    GNECrashDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2025
///
// Dialog used for handling crashes produced in Netedit
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEDialog.h"

// ===========================================================================
// class definitions
// ===========================================================================

class GNECrashDialog : public GNEDialog {

public:
    /// @brief Constructor
    GNECrashDialog(GNEApplicationWindow* applicationWindow, const std::exception& e);

    /// @brief Destructor
    ~GNECrashDialog();

    /// @brief run internal test
    void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument);

private:
    /// @brief Font for the widget
    FXFont* myHeadlineFont;

    /// @brief Invalidated copy constructor.
    GNECrashDialog(const GNECrashDialog&) = delete;

    /// @brief Invalidated assignment operator
    GNECrashDialog& operator=(const GNECrashDialog& src) = delete;
};
