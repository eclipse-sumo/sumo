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
/// @file    MFXCheckButtonTooltip.h
/// @author  Pablo Alvarez Lopez
/// @date    May 23
///
// CheckButton similar to FXCheckButton but with the possibility of showing tooltips
/****************************************************************************/
#pragma once
#include <config.h>

#include "fxheader.h"
#include "MFXStaticToolTip.h"

/**
 * @class MFXCheckButtonTooltip
 */
class MFXCheckButtonTooltip : public FXCheckButton {
    /// @brief fox declaration
    FXDECLARE(MFXCheckButtonTooltip)

public:
    /// @brief constructor (Exactly like the FXCheckButton constructor)
    MFXCheckButtonTooltip(FXComposite* p, MFXStaticToolTip* staticToolTip, const FXString& text, FXObject* tgt, FXSelector sel,
                          FXuint opts = TEXTFIELD_NORMAL, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0,
                          FXint pl = DEFAULT_PAD, FXint pr = DEFAULT_PAD, FXint pt = DEFAULT_PAD, FXint pb = DEFAULT_PAD);

    /// @brief destructor (Called automatically)
    ~MFXCheckButtonTooltip();

    /// @brief set toolTip
    void setToolTipText(const FXString& toolTip);

    /// @name FOX callbacks
    /// @{
    /// @brief called when mouse enter in MFXCheckButtonTooltip
    long onEnter(FXObject*, FXSelector, void*);

    /// @brief called when mouse leaves in MFXCheckButtonTooltip
    long onLeave(FXObject*, FXSelector, void*);

    /// @brief called when mouse motion in MFXCheckButtonTooltip
    long onMotion(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX needs this
    FOX_CONSTRUCTOR(MFXCheckButtonTooltip)

    /// @brief static tooltip
    MFXStaticToolTip* myStaticToolTip = nullptr;

    /// @brief toolTip text (if set, tooltip will be always show)
    FXString myToolTipText;

private:
    /// @brief Invalidated copy constructor.
    MFXCheckButtonTooltip(const MFXCheckButtonTooltip&) = delete;

    /// @brief Invalidated assignment operator.
    MFXCheckButtonTooltip& operator=(const MFXCheckButtonTooltip&) = delete;
};
