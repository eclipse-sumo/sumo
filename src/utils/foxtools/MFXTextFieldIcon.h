/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2006-2021 German Aerospace Center (DLR) and others.
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
/// @file    MFXTextFieldIcon.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2021
///
//
/****************************************************************************/

#pragma once
#include <config.h>

#include "fxheader.h"


/// @brief FXTextFieldIcon (based on FXTextFieldIcon)
class MFXTextFieldIcon : public FXTextField {

public:
    /// @brief constructor
    MFXTextFieldIcon(FXComposite* p, FXint ncols, FXObject* tgt = NULL, FXSelector sel = 0, FXuint opts = TEXTFIELD_NORMAL, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0, FXint pl = DEFAULT_PAD, FXint pr = DEFAULT_PAD, FXint pt = DEFAULT_PAD, FXint pb = DEFAULT_PAD);

    /// @brief set icon
    void setIcon(FXIcon* icon);

    /// @brief reset textField
    void resetTextField();

protected:
    /// @brief draw icon text range
    void drawIconTextRange(FXDCWindow& dc, const FXint iconWidth, FXint fm, FXint to);

    /// @brief icon
    FXIcon* myIcon;
};
