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
/// @file    GUIDialog_HallOfFame.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Angelo Banse
/// @date    Thu, 15 Oct 2020
///
// The SUMO User Conference "Hall of Fame" - dialog / easter egg
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/fxheader.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIDialog_HallOfFame
 * @brief The SUMO User Conference "Hall of Fame" - dialog / easter egg
 */
class GUIDialog_HallOfFame : public FXDialogBox {
public:
    /** @brief Constructor
     * @param[in] parent The parent window
     */
    GUIDialog_HallOfFame(FXWindow* parent);

    /// @brief Destructor
    ~GUIDialog_HallOfFame();

    /// @brief Creates the widget
    void create();

private:
    /// @brief Font for the widget
    FXFont* myHeadlineFont;
};
