/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNECalibratorFlowDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    March 2017
/// @version $Id$
///
// Dialog for edit calibrator flows
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

// ===========================================================================
// class declarations
// ===========================================================================

class GNECalibratorFlow;

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
    GNECalibratorFlowDialog(GNECalibratorFlow* editedCalibratorFlowParent, bool updatingElement);

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

    /// @brief interator to edited calibrator flow
    GNECalibratorFlow* myEditedCalibratorFlow;

    /// @brief flag to indicate if flow are being created or modified
    bool myUpdatingElement;

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

    /// @brief update data fields
    void updateCalibratorFlowValues();

private:
    /// @brief Invalidated copy constructor.
    GNECalibratorFlowDialog(const GNECalibratorFlowDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNECalibratorFlowDialog& operator=(const GNECalibratorFlowDialog&) = delete;
};

#endif
