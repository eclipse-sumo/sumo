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
/// @file    GNEToolGenerateRerouters.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2022
///
// Abstract dialog for tools
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEToolGenerateReroutersElements.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNEUndoList;
class GNEApplicationWindow;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEToolGenerateRerouters : protected FXTopWindow {
    /// @brief FOX-declaration
    FXDECLARE(GNEToolGenerateRerouters)

public:
    /// @brief Constructor
    GNEToolGenerateRerouters(GNEApplicationWindow* GNEApp, const std::string& name, const int dialogWidth, const int dialogHeight);

    /// @brief destructor
    ~GNEToolGenerateRerouters();

    /// @brief open window
    void openToolDialog();

    /// @brief hide window
    void hideToolDialog();

    /// @brief shown
    bool shown() const;

    /// @brief add argument
    void addArgument(GNEToolGenerateReroutersElements::Argument* argument);

    /// @name FOX-callbacks
    /// @{
    /// @brief event after press run button
    long onCmdRun(FXObject*, FXSelector, void*);

    /// @brief event after press cancel button
    long onCmdCancel(FXObject*, FXSelector, void*);

    /// @brief event after press reset button
    long onCmdReset(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX needs this
    FOX_CONSTRUCTOR(GNEToolGenerateRerouters)

    /// @brief pointer to GNEApplicationWindow
    GNEApplicationWindow* myGNEApp;

    /// @brief vertical frame for contents
    FXVerticalFrame* myContentFrame = nullptr;

    /// @brief execute dialog as modal
    FXint openAsModalDialog(FXuint placement = PLACEMENT_CURSOR);

private:
    /// @brief list of arguments
    std::vector<GNEToolGenerateReroutersElements::Argument*> myArguments;

    /// @brief Invalidated copy constructor.
    GNEToolGenerateRerouters(const GNEToolGenerateRerouters&) = delete;

    /// @brief Invalidated assignment operator.
    GNEToolGenerateRerouters& operator=(const GNEToolGenerateRerouters&) = delete;
};

