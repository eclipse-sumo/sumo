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
/// @file    MFXTextFieldTooltip.h
/// @author  Pablo Alvarez Lopez
/// @date    2022-07-22
///
// TextField similar to FXTextField but with the possibility of showing tooltips
/****************************************************************************/
#pragma once
#include <config.h>

#include "fxheader.h"
#include "MFXStaticToolTip.h"

/**
 * @class MFXTextFieldTooltip
 */
class MFXTextFieldTooltip : public FXTextField {
    /// @brief fox declaration
    FXDECLARE(MFXTextFieldTooltip)

public:
    /// @brief constructor (Exactly like the FXTextField constructor)
    MFXTextFieldTooltip(FXComposite* p, MFXStaticToolTip* staticToolTip, FXint ncols, FXObject* tgt, FXSelector sel,
                        FXuint opts = TEXTFIELD_NORMAL, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0,
                        FXint pl = DEFAULT_PAD, FXint pr = DEFAULT_PAD, FXint pt = DEFAULT_PAD, FXint pb = DEFAULT_PAD);

    /// @brief destructor (Called automatically)
    ~MFXTextFieldTooltip();

    /// @brief set toolTip
    void setToolTipText(const FXString& toolTip);

    /// @name FOX callbacks
    /// @{
    /// @brief called when mouse enter in MFXTextFieldTooltip
    long onEnter(FXObject*, FXSelector, void*);

    /// @brief called when mouse leaves in MFXTextFieldTooltip
    long onLeave(FXObject*, FXSelector, void*);

    /// @brief called when mouse motion in MFXTextFieldTooltip
    long onMotion(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX needs this
    FOX_CONSTRUCTOR(MFXTextFieldTooltip)

    /// @brief static tooltip
    MFXStaticToolTip* myStaticToolTip = nullptr;

    /// @brief toolTip text (if set, tooltip will be always show)
    FXString myToolTipText;

private:
    /// @brief Invalidated copy constructor.
    MFXTextFieldTooltip(const MFXTextFieldTooltip&) = delete;

    /// @brief Invalidated assignment operator.
    MFXTextFieldTooltip& operator=(const MFXTextFieldTooltip&) = delete;
};
