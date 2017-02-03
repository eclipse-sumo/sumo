/****************************************************************************/
/// @file    GNEVariableSpeedSignDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    April 2016
/// @version $Id$
///
/// A class for edit phases of Variable Speed Signals
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
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
    GNEVariableSpeedSignDialog(GNEVariableSpeedSign* variableSpeedSignParent);

    /// @brief destructor
    ~GNEVariableSpeedSignDialog();

    /// @name FOX-callbacks
    /// @{
    /// @brief event called after press add row button
    long onCmdAddRow(FXObject*, FXSelector, void*);

    /// @brief event called after press remove row
    long onCmdRemoveRow(FXObject*, FXSelector, void*);

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
    GNEVariableSpeedSign* myVariableSpeedSignParent;

    /// @brief Map with the temporal VSSValues
    std::map<SUMOReal, SUMOReal> myVSSValues;

    /// @brief Table with the data
    FXTable* myDataList;

    /// @brief Horizontal frame for row elements
    FXHorizontalFrame* myRowFrame;

    /// @bbriefie Text field with step
    FXTextField* myRowStep;

    /// @brief Text field with speed
    FXTextField* myRowSpeed;

    /// @brief Button for insert row
    FXButton* myAddRow;

private:
    /// @brief update data table
    void updateTable();

    /// @brief Invalidated copy constructor.
    GNEVariableSpeedSignDialog(const GNEVariableSpeedSignDialog&);

    /// @brief Invalidated assignment operator.
    GNEVariableSpeedSignDialog& operator=(const GNEVariableSpeedSignDialog&);
};

#endif
