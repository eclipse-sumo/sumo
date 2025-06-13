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
/// @file    MFXCheckButtonTooltip.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2026
///
// reimplementation of FXColorDialog, which allow to test it using internal test
/****************************************************************************/
#pragma once
#include <config.h>

#include "fxheader.h"

// ===========================================================================
// class declaration
// ===========================================================================

class InternalTest;

// ===========================================================================
// class definitions
// ===========================================================================

class MFXColorDialog : public FXColorDialog {

public:
    /// @brief constructor
    MFXColorDialog(FXWindow* owner, const FXString& name, FXuint opts = 0, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0);

    /// @brief destructor
    virtual ~MFXColorDialog();

    /// @brief open dialog
    FXuint openDialog(const InternalTest* internalTest, const FXuint placement = PLACEMENT_CURSOR);

private:
    /// @brief disable copy constructor
    MFXColorDialog(const MFXColorDialog&) = delete;

    /// @brief disable assignment operator
    MFXColorDialog& operator=(const MFXColorDialog&) = delete;
};
