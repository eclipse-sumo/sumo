/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    MFXButtonTooltip.h
/// @author  Angelo Banse
/// @date    2022-06-21
///
// Button similar to FXButton but with the possibility of showing tooltips
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>

#include "fxheader.h"
#include "MFXStaticToolTip.h"

/**
 * @class MFXButtonTooltip
 */
class MFXButtonTooltip : public FXButton {
    /// @brief fox declaration
    FXDECLARE(MFXButtonTooltip)

public:
    /// @brief constructor (Exactly like the FXButton constructor)
    MFXButtonTooltip(FXComposite* p, MFXStaticToolTip* staticToolTip, const std::string& text, FXIcon* ic = NULL,
                     FXObject* tgt = NULL, FXSelector sel = 0, FXuint opts = BUTTON_NORMAL,
                     FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0,
                     FXint pl = DEFAULT_PAD, FXint pr = DEFAULT_PAD, FXint pt = DEFAULT_PAD, FXint pb = DEFAULT_PAD);

    /// @brief destructor (Called automatically)
    ~MFXButtonTooltip();

    /// @name FOX callbacks
    /// @{
    /// @brief called when mouse enter in MFXButtonTooltip
    long onEnter(FXObject*, FXSelector, void*);

    /// @brief called when mouse leaves in MFXButtonTooltip
    long onLeave(FXObject*, FXSelector, void*);

    /// @brief called when mouse motion in MFXButtonTooltip
    long onMotion(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX needs this
    FOX_CONSTRUCTOR(MFXButtonTooltip)

    /// @brief static tooltip
    MFXStaticToolTip* myStaticToolTip = nullptr;

private:
    /// @brief Invalidated copy constructor.
    MFXButtonTooltip(const MFXButtonTooltip&) = delete;

    /// @brief Invalidated assignment operator.
    MFXButtonTooltip& operator=(const MFXButtonTooltip&) = delete;
};
