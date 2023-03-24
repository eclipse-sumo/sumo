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
/// @file    MFXListItemIcon.h
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2023
///
//
/****************************************************************************/

#pragma once
#include <config.h>

#include "fxheader.h"


/// @brief A list item which allows for custom coloring
class MFXListItemIcon : public FXListItem {
    /// @brief FOX declaration
    FXDECLARE(MFXListItemIcon)

public:
    /// @brief Construct new item with given text, icon, and user-data
    MFXListItemIcon(const FXString& text, FXIcon* ic, FXColor backGroundColor, void* ptr = NULL);

    /// @brief draw MFXListItemIcon
    void draw(const FXList* list, FXDC& dc, FXint x, FXint y, FXint w, FXint h);

    /// @brief get background color
    const FXColor& getBackGroundColor() const;

protected:
    /// @brief fox need this
    MFXListItemIcon();

    /// @brief backGround color
    FXColor myBackGroundColor;
};
