/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
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

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <map>
#include "GNEAdditionalDialog.h"
#include "GNEVariableSpeedSignStep.h"


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

    /// @brief get edited Variable Speed Sign
    GNEVariableSpeedSign* getEditedVariableSpeedSign() const;

    /// @name FOX-callbacks
    /// @{
    /// @brief event called after press add row button
    long onCmdAddStep(FXObject*, FXSelector, void*);

    /// @brief event called after edit row
    long onCmdEditStep(FXObject*, FXSelector, void*);

    /// @brief event called after clicked a row
    long onCmdClickedStep(FXObject*, FXSelector, void*);

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

    /// @brief Pointer to Variable Speed Signal
    GNEVariableSpeedSign* myEditedVariableSpeedSign;

    /// @brief Table with the data
    FXTable* myStepsTable;

    /// @brief Horizontal frame for row elements
    FXHorizontalFrame* myAddStepFrame;

    /// @brief Button for insert row
    FXButton* myAddStepButton;

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
