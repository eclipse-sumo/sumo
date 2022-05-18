/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2006-2022 German Aerospace Center (DLR) and others.
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
/// @file    FXStaticToolTip.h
/// @author  Pablo Alvarez Lopez
/// @date    May 2022
///
//
/****************************************************************************/

#pragma once
#include <config.h>

#include "fxheader.h"

/// @brief FXStaticToolTip (based on FXToolTip)
class FXStaticToolTip : public FXToolTip {
    /// @brief FOX declaration
    FXDECLARE(FXStaticToolTip)

public:
    /// @brief constructor
    FXStaticToolTip(FXApp* app);

    /// @brief destructor
    ~FXStaticToolTip();

    /// @brief draw FXStaticToolTip
    long onPaint(FXObject* obj, FXSelector sel, void* ptr);

    /// @brief show tip
    long onTipShow(FXObject*,FXSelector,void*);

    /// @brief hide tip
    long onTipHide(FXObject*,FXSelector,void*);

protected:
    /// @brief FOX need this
    FXStaticToolTip();

private:
    /// @brief object called in show()
    FXEvent* myToolTippedObject = nullptr;
};
