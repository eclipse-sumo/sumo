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
/// @file    GNEAboutDialog.h
/// @author  Jakob Erdmann
/// @date    Feb 2011
///
// The "About" - dialog for netedit, (adapted from GUIDialog_AboutSUMO)
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEDialog.h"

// ===========================================================================
// class definitions
// ===========================================================================

class GNEAboutDialog : public GNEDialog {

public:
    /// @brief Constructor
    GNEAboutDialog(GNEApplicationWindow* applicationWindow);

    /// @brief Destructor
    ~GNEAboutDialog();

    /// @brief run internal test
    void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument);

private:
    /// @brief Font for the widget
    FXFont* myHeadlineFont;

    /// @brief Invalidated copy constructor.
    GNEAboutDialog(const GNEAboutDialog&) = delete;

    /// @brief Invalidated assignment operator
    GNEAboutDialog& operator=(const GNEAboutDialog& src) = delete;
};
