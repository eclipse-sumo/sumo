/****************************************************************************/
/// @file    GNEVariableSpeedSignalDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    April 2016
/// @version $Id$
///
/// A class for edit phases of Variable Speed Signals
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2016 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNEVariableSpeedSignalDialog_h
#define GNEVariableSpeedSignalDialog_h

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

class GNEVariableSpeedSignal;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEAdditionalDialog
 * @brief Dialog to edit sequences, parameters, etc.. of Additionals
 */
class GNEVariableSpeedSignalDialog : public GNEAdditionalDialog {
    /// @brief FOX-declaration
    FXDECLARE(GNEVariableSpeedSignalDialog)

public:
    // Constructor
    GNEVariableSpeedSignalDialog(GNEVariableSpeedSignal* variableSpeedSignalParent);

    // destructor
    ~GNEVariableSpeedSignalDialog();

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
    GNEVariableSpeedSignalDialog() {}

    /// @brief Pointer to Variable Speed Signal
    GNEVariableSpeedSignal* myVariableSpeedSignalParent;

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
    GNEVariableSpeedSignalDialog(const GNEVariableSpeedSignalDialog&);

    /// @brief Invalidated assignment operator.
    GNEVariableSpeedSignalDialog& operator=(const GNEVariableSpeedSignalDialog&);
};

#endif
