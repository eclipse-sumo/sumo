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
/// @file    GNEColorDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2025
///
// Custom GNEColorDialog used in Netedit that supports internal tests
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEDialog.h"

// ===========================================================================
// class definitions
// ===========================================================================

class GNEColorDialog : public GNEDialog {
    FXDECLARE(GNEColorDialog)

public:
    /// @brief Construct color dialog
    GNEColorDialog(GNEApplicationWindow* applicationWindow, const RGBColor color);

    /// @brief Destructor
    virtual ~GNEColorDialog();

    /// @brief run internal test
    void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument);

    /// @brief Get the color
    RGBColor getColor() const;

    /// @name FOX-callbacks
    /// @{

    /// @brief Called when color is changed
    long onChgColor(FXObject*, FXSelector, void*);

    /// @brief Called when color is selected
    long onCmdColor(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief Default constructor
    FOX_CONSTRUCTOR(GNEColorDialog)

    /// @brief Color selector box
    FXColorSelector* colorbox;

private:
    /// @brief Invalidated copy constructor.
    GNEColorDialog(const GNEColorDialog&) = delete;

    /// @brief Invalidated assignment operator
    GNEColorDialog& operator=(const GNEColorDialog& src) = delete;
};
