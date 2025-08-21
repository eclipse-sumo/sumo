/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEVariableSpeedSignDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    April 2016
///
// Dialog for edit variableSpeedSigns
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/changes/GNEChange_Additional.h>
#include <netedit/dialogs/elements/lists/GNETemplateElementList.h>
#include <netedit/elements/additional/GNEAdditional.h>

#include "GNEElementDialog.h"

// ===========================================================================
// class definitions
// ===========================================================================

class GNEVariableSpeedSignDialog : public GNEElementDialog<GNEAdditional> {

public:
    /// @brief Constructor
    GNEVariableSpeedSignDialog(GNEAdditional* variableSpeedSign);

    /// @brief destructor
    ~GNEVariableSpeedSignDialog();

    /// @brief run internal test
    void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument);

    /// @name FOX-callbacks
    /// @{

    /// @brief event after press accept button
    long onCmdAccept(FXObject*, FXSelector, void*);

    /// @brief event after press reset button
    long onCmdReset(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief variableSpeedSign step list
    class VariableSpeedSignStepsList : public GNETemplateElementList<GNEAdditional, GNEAdditional, GNEChange_Additional> {

    public:
        /// @brief constructor
        VariableSpeedSignStepsList(GNEVariableSpeedSignDialog* variableSpeedSignDialog);

        /// @brief add new element
        long addNewElement();

        /// @brief open dialog
        long openDialog(const size_t rowIndex);

        /// @briec check if steps are sorted
        bool isSorted() const;

    private:
        /// @brief Invalidated copy constructor
        VariableSpeedSignStepsList(const VariableSpeedSignStepsList&) = delete;

        /// @brief Invalidated assignment operator
        VariableSpeedSignStepsList& operator=(const VariableSpeedSignStepsList&) = delete;
    };

    /// @brief Element list for variableSpeedSign steps
    VariableSpeedSignStepsList* myVariableSpeedSignSteps;

private:
    /// @brief Invalidated copy constructor.
    GNEVariableSpeedSignDialog(const GNEVariableSpeedSignDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNEVariableSpeedSignDialog& operator=(const GNEVariableSpeedSignDialog&) = delete;
};
