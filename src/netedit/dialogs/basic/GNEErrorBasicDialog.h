/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2006-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEErrorBasicDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2025
///
// Custom FXDialogBox error dialog used in Netedit that supports internal tests
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEBasicDialog.h"

// ===========================================================================
// class definitions
// ===========================================================================

class GNEErrorBasicDialog : public GNEBasicDialog {

public:
    /// @brief constructor
    GNEErrorBasicDialog(GNEApplicationWindow* applicationWindow, const std::string& name,
                        const std::string& info);

    /// @brief constructor for multiple lines
    GNEErrorBasicDialog(GNEApplicationWindow* applicationWindow, const std::string& name,
                        const std::string& infoLineA, const std::string& infoLineB);

private:
    /// @brief Invalidated copy constructor.
    GNEErrorBasicDialog(const GNEErrorBasicDialog&) = delete;

    /// @brief Invalidated assignment operator
    GNEErrorBasicDialog& operator=(const GNEErrorBasicDialog& src) = delete;
};
