/****************************************************************************/
/// @file    GNEVariableSpeedSignalDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    April 2016
/// @version $Id: GNEVariableSpeedSignalDialog.h 20472 2016-04-15 15:36:45Z palcraft $
///
/// A class for edit phases of Variable Speed Signals
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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
#include <utils/common/SUMOTime.h>
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
    GNEVariableSpeedSignalDialog(GNEVariableSpeedSignal *variableSpeedSignalParent);

    // destructor
    ~GNEVariableSpeedSignalDialog();

    /// @name FOX-callbacks
    /// @{
    /// @brief event after press insert button
    long onCMDInsertRow(FXObject*, FXSelector, void*);

    /// @brief event after press remove row
    long onCmdRemoveRow(FXObject*, FXSelector, void*);

    /// @brief event after press accept button
    long onCmdAccept(FXObject* sender, FXSelector sel, void* ptr);

    /// @brief event after press cancel button
    long onCmdCancel(FXObject* sender, FXSelector sel, void* ptr);

    /// @brief event after press cancel button
    long onCmdReset(FXObject*, FXSelector, void*);

    /// @brief event after press add row button
    long onCmdAddRow(FXObject* sender, FXSelector sel, void* data);
    /// @}

protected:
    /// @brief FOX needs this
    GNEVariableSpeedSignalDialog() {}

    /// @brief Pointer to Variable Speed Signal
    GNEVariableSpeedSignal *myVariableSpeedSignalParent;

    /// @brief Map with the temporal VSSData
    std::map<SUMOTime, SUMOReal> myVSSData;

    /// @brief Table with the data
    FXTable *myDataList;

    /// @brief Text field with data
    FXLabel *myRowLabel;

    /// @bbriefie Text field with step
    FXTextField *myRowStep;

    /// @brief Text field with speed
    FXTextField *myRowSpeed;

    /// @brief Button for insert row
    FXButton *myAddRow;

    /// @brief Horizontal frame for row elements
    FXHorizontalFrame *myRowFrame;

private:
    /// @brif reffill table
    void fillTable();

    /// @brief Invalidated copy constructor.
    GNEVariableSpeedSignalDialog(const GNEVariableSpeedSignalDialog&);

    /// @brief Invalidated assignment operator.
    GNEVariableSpeedSignalDialog& operator=(const GNEVariableSpeedSignalDialog&);
};

#endif
