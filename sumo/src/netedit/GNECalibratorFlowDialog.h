/****************************************************************************/
/// @file    GNECalibratorFlowDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    March 2017
/// @version $Id$
///
/// Dialog for edit calibrator flows
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
#ifndef GNECalibratorFlowDialog_h
#define GNECalibratorFlowDialog_h

// ===========================================================================
// included modules
// ===========================================================================

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GNEAdditionalDialog.h"
#include "GNECalibratorFlow.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNECalibratorDialog;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNECalibratorFlowDialog
 * @brief Dialog for edit rerouter intervals
 */

class GNECalibratorFlowDialog : public GNEAdditionalDialog {
    /// @brief FOX-declaration
    FXDECLARE(GNECalibratorFlowDialog)

public:
    /// @brief constructor
    GNECalibratorFlowDialog(GNECalibratorDialog* calibratorDialog, GNECalibratorFlow& calibratorFlow, bool updatingElement);

    /// @brief destructor
    ~GNECalibratorFlowDialog();

    /// @name FOX-callbacks
    /// @{
    /// @brief event after press accept button
    long onCmdAccept(FXObject*, FXSelector, void*);

    /// @brief event after press cancel button
    long onCmdCancel(FXObject*, FXSelector, void*);

    /// @brief event after press reset button
    long onCmdReset(FXObject*, FXSelector, void*);

    /// @brief event after change value
    long onCmdSetVariable(FXObject*, FXSelector, void*);

    /// @brief event after change type of flow
    long onCmdSetTypeOfFlow(FXObject* radioButton, FXSelector, void*);
    /// @}

protected:
    /// @brief FOX needs this
    GNECalibratorFlowDialog() {}

    /// @brief update data fields
    void updateCalibratorFlowValues();

    /// @brief pointer to GNECalibratorDialog parent
    GNECalibratorDialog* myCalibratorDialogParent;

    /// @brief pointer to rerouter interval
    GNECalibratorFlow* myCalibratorFlow;

    /// @brief flag to indicate if flow are being created or modified
    bool myUpdatingElement;

    /// @brief pointer in which save modifications of CalibratorFlow
    GNECalibratorFlow* myCopyOfCalibratorFlow;

    /// @brief flag to check if current calibrator flow is valid
    bool myCalibratorFlowValid;

    /// @brief current sumo attribute invalid
    SumoXMLAttr myInvalidAttr;

    /// @brief TextField for flow ID
    FXTextField* myTextFieldFlowID;

    /// @brief ComboBox for Type
    FXComboBox* myComboBoxVehicleType;

    /// @brief TextField for Route
    FXComboBox* myComboBoxRoute;

    /// @brief TextField for Color
    FXTextField* myTextFieldColor;

    /// @brief TextField for Depart Lane
    FXTextField* myTextFieldDepartLane;

    /// @brief TextField for Depart Pos
    FXTextField* myTextFieldDepartPos;

    /// @brief TextField for Depart Speed
    FXTextField* myTextFieldDepartSpeed;

    /// @brief TextField for Arrival Lane
    FXTextField* myTextFieldArrivalLane;

    /// @brief TextField for Arrival Pos
    FXTextField* myTextFieldArrivalPos;

    /// @brief TextField for Arrival Speed
    FXTextField* myTextFieldArrivalSpeed;

    /// @brief TextField for Line
    FXTextField* myTextFieldLine;

    /// @brief TextField for PersonNumber
    FXTextField* myTextFieldPersonNumber;

    /// @brief TextField for Container Number
    FXTextField* myTextFieldContainerNumber;

    /// @brief checkButton for Reroute
    FXCheckButton* myRerouteCheckButton;

    /// @brief TextField for DepartPosLat
    FXTextField* myTextFieldDepartPosLat;

    /// @brief TextField for ArrivalPosLat
    FXTextField* myTextFieldArrivalPosLat;

    /// @brief TextField for Begin
    FXTextField* myTextFieldBegin;

    /// @brief TextField for End
    FXTextField* myTextFieldEnd;

    /// @brief TextField for Number
    FXTextField* myTextFieldNumber;

    /// @brief RadioButton for VehsPerHour
    FXRadioButton* myRadioButtonVehsPerHour;

    /// @brief TextField for VehsPerHour
    FXTextField* myTextFieldVehsPerHour;

    /// @brief RadioButton for Period
    FXRadioButton* myRadioButtonPeriod;

    /// @brief TextField for Period
    FXTextField* myTextFieldPeriod;

    /// @brief RadioButton for Probability
    FXRadioButton* myRadioButtonProbability;

    /// @brief TextField for Probability
    FXTextField* myTextFieldProbability;

private:
    /// @brief Invalidated copy constructor.
    GNECalibratorFlowDialog(const GNECalibratorFlowDialog&);

    /// @brief Invalidated assignment operator.
    GNECalibratorFlowDialog& operator=(const GNECalibratorFlowDialog&);
};

#endif
