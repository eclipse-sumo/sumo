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
/// @file    MFXListIcon.h
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2023
///
//
/****************************************************************************/

#pragma once
#include <config.h>

#include "fxheader.h"


/// @brief A list item which allows for custom coloring
class MFXListIcon : public FXList {
    /// @brief FOX declaration
    FXDECLARE(MFXListIcon)

public:
    /// @brief Construct new item with given text, icon, and user-data
    MFXListIcon(FXComposite* p, FXObject* tgt = NULL, FXSelector sel = 0, FXuint opts = LIST_NORMAL, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0);

    /// @brief Get default height
    FXint getDefaultHeight();

    /// @brief Draw item list
    long onPaint(FXObject*, FXSelector, void*);

protected:
    /// @brief fox need this
    MFXListIcon();
};
