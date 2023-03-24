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
/// @file    GUIPersistentWindowPos.h
/// @author  Jakob Erdmann
/// @date    Thu, 09.03.2023
///
// Store persistent window position
/****************************************************************************/
#pragma once
#include <config.h>
#include <utils/foxtools/fxheader.h>



// ===========================================================================
// class definition
// ===========================================================================
/**
 * @class GUIPersistentWindowPos
 * @brief Persists window position in the registry
 */
class GUIPersistentWindowPos {

public:
    /** @brief Constructor (Notifies both the parent and the storage about being initialised)
     * @param[in] parent The parent window
     * @param[in] str The storage of object selections to use
     */
    GUIPersistentWindowPos(FXWindow* parent, const std::string& name, bool storeSize,
                           int x = 150, int y = 150,
                           int width = 700, int height = 500,
                           int minSize = 400, int minTitlebarHeight = 20);

    /// @brief Destructor (Notifies both the parent and the storage about being destroyed)
    ~GUIPersistentWindowPos();

    void saveWindowPos();
    void loadWindowPos();

protected:
    /// @brief The window for which the position is being stored
    FXWindow* myParent;

    /// @brief Name for storing in the registry
    std::string myWindowName;

    /// @brief whether window size shall be stored
    bool myStoreSize;

    int myDefaultX;
    int myDefaultY;

    int myDefaultWidth;
    int myDefaultHeight;

    int myMinSize;
    int myMinTitlebarHeight;

    GUIPersistentWindowPos();

};
