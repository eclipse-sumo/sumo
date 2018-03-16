/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNECalibratorVehicleTypeDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    March 2017
/// @version $Id$
///
// Dialog for edit calibrator vehicleTypes
/****************************************************************************/
#ifndef GNECalibratorVehicleTypeDialog_h
#define GNECalibratorVehicleTypeDialog_h

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

class GNECalibratorVehicleType;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNECalibratorVehicleTypeDialog
 * @brief Dialog for edit rerouter intervals
 */

class GNECalibratorVehicleTypeDialog : public GNEAdditionalDialog {
    /// @brief FOX-declaration
    FXDECLARE(GNECalibratorVehicleTypeDialog)

public:
    /// @brief constructor
    GNECalibratorVehicleTypeDialog(GNECalibratorVehicleType* editedCalibratorVehicleType, bool updatingElement);

    /// @brief destructor
    ~GNECalibratorVehicleTypeDialog();

    /// @name FOX-callbacks
    /// @{
    /// @brief event after press accept button
    long onCmdAccept(FXObject*, FXSelector, void*);

    /// @brief event after press cancel button
    long onCmdCancel(FXObject*, FXSelector, void*);

    /// @brief event after press reset button
    long onCmdReset(FXObject*, FXSelector, void*);

    /// @event after change a variable of vehicle type
    long onCmdSetVariable(FXObject*, FXSelector, void*);
    /// @}

protected:
    /// @brief FOX needs this
    GNECalibratorVehicleTypeDialog() {}

    /// @brief pointer to original calibrator vehicle type
    GNECalibratorVehicleType* myEditedCalibratorVehicleType;

    /// @brief flag to indicate if flow are being created or modified
    bool myUpdatingElement;

    /// @brief flag to check if current calibrator vehicleType is valid
    bool myCalibratorVehicleTypeValid;

    /// @brief current sumo attribute invalid
    SumoXMLAttr myInvalidAttr;

    /// @brief FXTextfield for vehicleTypeID
    FXTextField* myTextFieldVehicleTypeID;

    /// @brief FXComboBox for VClass
    FXComboBox* myComboBoxVClass;

    /// @brief label with image of VClass
    FXLabel* myComboBoxVClassLabelImage;

    /// @brief FXComboBox for Shape
    FXComboBox* myComboBoxShape;

    /// @brief label with image of Shape
    FXLabel* myComboBoxShapeLabelImage;

    /// @brief FXTextfield for Accel
    FXTextField* myTextFieldAccel;

    /// @brief FXTextfield for Decel
    FXTextField* myTextFieldDecel;

    /// @brief FXTextfield for Sigma
    FXTextField* myTextFieldSigma;

    /// @brief FXTextfield for Tau
    FXTextField* myTextFieldTau;

    /// @brief FXTextfield for Length
    FXTextField* myTextFieldLength;

    /// @brief FXTextfield for MinGap
    FXTextField* myTextFieldMinGap;

    /// @brief FXTextfield for MaxSpeed
    FXTextField* myTextFieldMaxSpeed;

    /// @brief FXTextfield for SpeedFactor
    FXTextField* myTextFieldSpeedFactor;

    /// @brief FXTextfield for SpeedDev
    FXTextField* myTextFieldSpeedDev;

    /// @brief FXTextfield for Color
    FXTextField* myTextFieldColor;

    /// @brief FXTextfield for EmissionClass
    FXTextField* myTextFieldEmissionClass;

    /// @brief FXTextfield for Width
    FXTextField* myTextFieldWidth;

    /// @brief FXTextfield for Filename
    FXTextField* myTextFieldFilename;

    /// @brief FXTextfield for Impatience
    FXTextField* myTextFieldImpatience;

    /// @brief FXTextfield for LaneChangeModel
    FXTextField* myTextFieldLaneChangeModel;

    /// @brief FXTextfield for CarFollowModel
    FXTextField* myTextFieldCarFollowModel;

    /// @brief FXTextfield for PersonCapacity
    FXTextField* myTextFieldPersonCapacity;

    /// @brief FXTextfield for ContainerCapacity
    FXTextField* myTextFieldContainerCapacity;

    /// @brief FXTextfield for BoardingDuration
    FXTextField* myTextFieldBoardingDuration;

    /// @brief FXTextfield for LoadingDuration
    FXTextField* myTextFieldLoadingDuration;

    /// @brief FXTextfield for LatAlignment
    FXTextField* myTextFieldLatAlignment;

    /// @brief FXTextfield for MinGapLat
    FXTextField* myTextFieldMinGapLat;

    /// @brief FXTextfield for MaxSpeedLat
    FXTextField* myTextFieldMaxSpeedLat;

    /// @brief update data fields
    void updateCalibratorVehicleTypeValues();

    /// @brief set VClass texture
    void setVClassLabelImage();

private:
    /// @brief Invalidated copy constructor.
    GNECalibratorVehicleTypeDialog(const GNECalibratorVehicleTypeDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNECalibratorVehicleTypeDialog& operator=(const GNECalibratorVehicleTypeDialog&) = delete;
};

#endif
