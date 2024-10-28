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
/// @file    GUIDialog_Breakpoints.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @date    Thu, 17 Jun 2004
///
// Editor for simulation breakpoints
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <vector>
#include <utils/foxtools/fxheader.h>
#include <utils/gui/div/GUIPersistentWindowPos.h>


// ===========================================================================
// class definition
// ===========================================================================
/**
 * @class GUIDialog_Breakpoints
 * @brief Editor for simulation breakpoints
 *
 * This dialog shows and lets the user edit the list of breakpoints - simulation
 *  time steps where the simulation halts.
 * @todo Use a LineReader instead of >> while reading
 */
class GUIDialog_Breakpoints : public FXMainWindow, public GUIPersistentWindowPos {
    // FOX-declarations
    FXDECLARE(GUIDialog_Breakpoints)

public:
    /** @brief Constructor
     * @param[in] parent The parent window
     */
    GUIDialog_Breakpoints(GUIApplicationWindow* parent, std::vector<SUMOTime>& breakpoints, FXMutex& breakpointLock, const SUMOTime simBegin);

    /// @brief Destructor
    ~GUIDialog_Breakpoints();

    /// @brief sets the focus after the window is created
    void show();

    /// @name FOX-callbacks
    /// @{

    /// @brief Called when the user presses the Load-button
    long onCmdLoad(FXObject*, FXSelector, void*);

    /// @brief Called when the user presses the Save-button
    long onCmdSave(FXObject*, FXSelector, void*);

    /// @brief Called when the user presses the Clear-button
    long onCmdClear(FXObject*, FXSelector, void*);

    /// @brief Called when the user clicks a time link in the message window
    long onCmdUpdateBreakpoints(FXObject*, FXSelector, void*);

    /// @brief Called when the user presses the Close-button
    long onCmdClose(FXObject*, FXSelector, void*);

    /// @brief Called when the table was changed
    long onCmdEditTable(FXObject*, FXSelector, void*);
    /// @}

    virtual void layout();

    /// @brief Rebuilds the entire list
    void rebuildList();

protected:
    /// @brief FOX need this
    FOX_CONSTRUCTOR(GUIDialog_Breakpoints)

private:

    /** @brief Builds a text representation of the items in the list
     * @return Breakpoints encoded as a string
     */
    std::string encode2TXT();

    /// @brief The list that holds the ids
    FXTable* myTable;

    /// @brief The parent window
    GUIApplicationWindow* myParent;

    /// @brief List of breakpoints
    std::vector<SUMOTime>* myBreakpoints;

    /// @brief Lock for modifying the list of breakpoints
    FXMutex* myBreakpointLock;

    /// @brief simulation begin
    SUMOTime mySimBegin;
};
