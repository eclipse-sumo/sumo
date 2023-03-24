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
/// @file    MFXToggleButtonTooltip.h
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2022
///
// Button similar to FXToggleButton but with the possibility of showing tooltips
/****************************************************************************/
#pragma once
#include <config.h>

#include <iostream>

#include "fxheader.h"
#include "MFXStaticToolTip.h"

/**
 * @class MFXToggleButtonTooltip
 */
class MFXToggleButtonTooltip : public FXToggleButton {
    /// @brief fox declaration
    FXDECLARE(MFXToggleButtonTooltip)

public:
    /// @brief constructor (Exactly like the FXToggleButton constructor)
    MFXToggleButtonTooltip(FXComposite* p, MFXStaticToolTip* staticToolTip, const std::string& text1, const std::string& text2,
                           FXIcon* ic1, FXIcon* ic2, FXObject* tgt = NULL, FXSelector sel = 0,
                           FXuint opts = BUTTON_NORMAL, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0,
                           FXint pl = DEFAULT_PAD, FXint pr = DEFAULT_PAD, FXint pt = DEFAULT_PAD, FXint pb = DEFAULT_PAD);

    /// @brief destructor (Called automatically)
    ~MFXToggleButtonTooltip();

    /// @name FOX callbacks
    /// @{
    /// @brief called when mouse enter in MFXToggleButtonTooltip
    long onEnter(FXObject*, FXSelector, void*);

    /// @brief called when mouse leaves in MFXToggleButtonTooltip
    long onLeave(FXObject*, FXSelector, void*);

    /// @brief called when mouse motion in MFXToggleButtonTooltip
    long onMotion(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX needs this
    FOX_CONSTRUCTOR(MFXToggleButtonTooltip)

    /// @brief static tooltip
    MFXStaticToolTip* myStaticToolTip = nullptr;

private:
    /// @brief Invalidated copy constructor.
    MFXToggleButtonTooltip(const MFXToggleButtonTooltip&) = delete;

    /// @brief Invalidated assignment operator.
    MFXToggleButtonTooltip& operator=(const MFXToggleButtonTooltip&) = delete;
};
