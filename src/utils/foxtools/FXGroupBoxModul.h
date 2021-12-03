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
/// @file    FXGroupBoxModul.h
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2021
///
//
/****************************************************************************/

#pragma once
#include <config.h>

#include "fxheader.h"

/// @brief FXGroupBoxModul (based on FXGroupBox)
class FXGroupBoxModul : public FXGroupBox {
    FXDECLARE(FXGroupBoxModul)

public:
    /// @brief constructor
    FXGroupBoxModul(FXComposite* p, const FXString& text, FXuint opts = GROUPBOX_NORMAL, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0, 
               FXint pl = DEFAULT_SPACING, FXint pr = DEFAULT_SPACING, FXint pt = DEFAULT_SPACING, FXint pb = DEFAULT_SPACING, FXint hs = DEFAULT_SPACING, FXint vs = DEFAULT_SPACING);

    /// @brief destructor
    ~FXGroupBoxModul() {}

protected:
    /// @brief FOX need this
    FXGroupBoxModul();
};
