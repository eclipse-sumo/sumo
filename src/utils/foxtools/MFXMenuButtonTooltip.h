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
/// @file    MFXMenuButtonTooltip.h
/// @author  Pablo Alvarez Lopez
/// @date    2022-07-27
///
// MenuButton similar to FXMenuButton but with the possibility of showing tooltips
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>

#include "fxheader.h"
#include "MFXStaticToolTip.h"

/**
 * @class MFXMenuButtonTooltip
 */
class MFXMenuButtonTooltip : public FXMenuButton {
    /// @brief fox declaration
    FXDECLARE(MFXMenuButtonTooltip)

public:
    /// @brief constructor (Exactly like the FXMenuButton constructor)
    MFXMenuButtonTooltip(FXComposite* p, MFXStaticToolTip* staticToolTip, const std::string& text, FXIcon* ic,
                         FXPopup* pup, FXObject* optionalTarget,
                         FXuint opts = JUSTIFY_NORMAL | ICON_BEFORE_TEXT | MENUBUTTON_DOWN,
                         FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0,
                         FXint pl = DEFAULT_PAD, FXint pr = DEFAULT_PAD, FXint pt = DEFAULT_PAD, FXint pb = DEFAULT_PAD);

    /// @brief destructor (Called automatically)
    ~MFXMenuButtonTooltip();

    /// @name FOX callbacks
    /// @{
    /// @brief called when mouse enter in MFXMenuButtonTooltip
    long onEnter(FXObject*, FXSelector, void*);

    /// @brief called when mouse leaves in MFXMenuButtonTooltip
    long onLeave(FXObject*, FXSelector, void*);

    /// @brief called when mouse motion in MFXMenuButtonTooltip
    long onMotion(FXObject*, FXSelector, void*);

    /// @brief called when left mouse button is press
    long onLeftBtnPress(FXObject*, FXSelector, void*);

    /// @brief called when key is press
    long onKeyPress(FXObject*, FXSelector, void*);

    /// @brief called when button is press
    long onCmdPost(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX needs this
    FOX_CONSTRUCTOR(MFXMenuButtonTooltip)

    /// @brief static tooltip
    MFXStaticToolTip* myStaticToolTip = nullptr;

    /// @brief optional target
    FXObject* myOptionalTarget = nullptr;

private:
    /// @brief Invalidated copy constructor.
    MFXMenuButtonTooltip(const MFXMenuButtonTooltip&) = delete;

    /// @brief Invalidated assignment operator.
    MFXMenuButtonTooltip& operator=(const MFXMenuButtonTooltip&) = delete;
};
