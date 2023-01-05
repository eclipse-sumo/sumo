/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2006-2023 German Aerospace Center (DLR) and others.
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
/// @file    MFXStaticToolTip.h
/// @author  Pablo Alvarez Lopez
/// @date    May 2022
///
//
/****************************************************************************/

#pragma once
#include <config.h>

#include "fxheader.h"

/// @brief MFXStaticToolTip (based on FXToolTip)
class MFXStaticToolTip : protected FXToolTip {
    /// @brief FOX declaration
    FXDECLARE(MFXStaticToolTip)

public:
    /// @brief constructor
    MFXStaticToolTip(FXApp* app);

    /// @brief destructor
    ~MFXStaticToolTip();

    /// @brief enable/disable static tooltip
    void enableStaticToolTip(const bool value);

    /// @brief check staticToolTip is enabled
    bool isStaticToolTipEnabled() const;

    /// @brief show static toolTip
    void showStaticToolTip(const FXString& toolTipText);

    /// @brief hide static toolTip
    void hideStaticToolTip();

    /// @name FOX callbacks
    /// @{
    /// @brief draw MFXStaticToolTip
    long onPaint(FXObject* obj, FXSelector sel, void* ptr);

    /// @brief called when tooltip is updated
    long onUpdate(FXObject* sender, FXSelector sel, void* ptr);

    /// @}

protected:
    /// @brief FOX need this
    MFXStaticToolTip();

private:
    /// @brief flag for enable/disable static tooltip
    bool myEnableStaticTooltip = true;
};
