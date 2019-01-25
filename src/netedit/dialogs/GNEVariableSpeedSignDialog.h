/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEVariableSpeedSignDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    April 2016
/// @version $Id$
///
// A class for edit phases of Variable Speed Signals
/****************************************************************************/
#ifndef GNEVariableSpeedSignDialog_h
#define GNEVariableSpeedSignDialog_h

// ===========================================================================
// included modules
// ===========================================================================

#include <config.h>

#include <map>
#include <netedit/additionals/GNEVariableSpeedSignStep.h>

#include "GNEAdditionalDialog.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNEVariableSpeedSign;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEAdditionalDialog
 * @brief Dialog to edit sequences, parameters, etc.. of Additionals
 */
class GNEVariableSpeedSignDialog : public GNEAdditionalDialog {
    /// @brief FOX-declaration
    FXDECLARE(GNEVariableSpeedSignDialog)

public:
    /// @brief constructor
    GNEVariableSpeedSignDialog(GNEVariableSpeedSign* editedVariableSpeedSign);

    /// @brief destructor
    ~GNEVariableSpeedSignDialog();

    /// @name FOX-callbacks
    /// @{
    /// @brief event called after press add row button
    long onCmdAddStep(FXObject*, FXSelector, void*);

    /// @brief event called after edit row
    long onCmdEditStep(FXObject*, FXSelector, void*);

    /// @brief event called after clicked a row
    long onCmdClickedStep(FXObject*, FXSelector, void*);

    /// @brief event called after clicked over sort step button
    long onCmdSortSteps(FXObject*, FXSelector, void*);

    /// @brief event called after press accept button
    long onCmdAccept(FXObject*, FXSelector, void*);

    /// @brief event called after press cancel button
    long onCmdCancel(FXObject*, FXSelector, void*);

    /// @brief event called after press cancel button
    long onCmdReset(FXObject*, FXSelector, void*);
    /// @}

protected:
    /// @brief FOX needs this
    GNEVariableSpeedSignDialog() {}

    /// @brief Table with the data
    FXTable* myStepsTable;

    /// @brief Button for insert row
    FXButton* myAddStepButton;

    /// @brief Button for sort row
    FXButton* mySortStepButton;

    // @brief Flag to check if current steps are valid
    bool myStepsValids;

private:
    /// @brief update data table
    void updateTableSteps();

    /// @brief Invalidated copy constructor.
    GNEVariableSpeedSignDialog(const GNEVariableSpeedSignDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNEVariableSpeedSignDialog& operator=(const GNEVariableSpeedSignDialog&) = delete;
};

#endif
