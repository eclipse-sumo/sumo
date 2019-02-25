/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEVehicleTypeDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2019
/// @version $Id$
///
// Dialog for edit vehicleTypes
/****************************************************************************/
#ifndef GNEVehicleTypeDialog_h
#define GNEVehicleTypeDialog_h

// ===========================================================================
// included modules
// ===========================================================================

#include <config.h>

#include "GNEDemandElementDialog.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNEVehicleType;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEVehicleTypeDialog
 * @brief Dialog for edit rerouter intervals
 */

class GNEVehicleTypeDialog : public GNEDemandElementDialog {
    /// @brief FOX-declaration
    FXDECLARE(GNEVehicleTypeDialog)

public:
    /// @brief constructor
    GNEVehicleTypeDialog(GNEDemandElement* editedVehicleType, bool updatingElement);

    /// @brief destructor
    ~GNEVehicleTypeDialog();

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
    /// @brief class for VClasses
    class VClassRow : public FXHorizontalFrame {

    public:
        /// @brief constructor
        VClassRow(GNEVehicleTypeDialog* vehicleTypeDialog, FXVerticalFrame* column);

        /// @brief set VClass texture
        void setVClassLabelImage();

        /// @brief FXComboBox for VClass
        FXComboBox* comboBoxVClass;
        
    private:
        /// @brief label with image of VClass
        FXLabel* myComboBoxVClassLabelImage;

        /// @brief pointer to Vehicle Type dialog parent
        GNEVehicleTypeDialog* myVehicleTypeDialog;
    };

    /// @brief class for VShapeRow
    class VShapeRow : public FXHorizontalFrame {
        
    public:
        /// @brief constructor
        VShapeRow(GNEVehicleTypeDialog* vehicleTypeDialog, FXVerticalFrame* column);

        /// @brief set VShape texture
        void setVShapeLabelImage();

        /// @brief FXComboBox for Shape
        FXComboBox* comboBoxShape;
        
    private:
        /// @brief label with image of Shape
        FXLabel* myComboBoxShapeLabelImage;

        /// @brief pointer to Vehicle Type dialog parent
        GNEVehicleTypeDialog* myVehicleTypeDialog;
    };

    /// @brief FOX needs this
    GNEVehicleTypeDialog() {}

    /// @brief flag to check if current vehicleType is valid
    bool myVehicleTypeValid;

    /// @brief current sumo attribute invalid
    SumoXMLAttr myInvalidAttr;

    /// @brief FXTextfield for vehicleTypeID
    FXTextField* myTextFieldVehicleTypeID;

    /// @brief vehicle class row
    VClassRow* myVClassRow;

    /// @brief FXTextfield for Color
    FXTextField* myTextFieldColor;

    /// @brief FXTextfield for Accel
    FXTextField* myTextFieldAccel;

    /// @brief FXTextfield for Decel
    FXTextField* myTextFieldDecel;

    /// @brief FXTextfield for aparent Decel
    FXTextField* myTextFieldAparentDecel;
    
    /// @brief FXTextfield for emergency Decel
    FXTextField* myTextFieldEmergencyAccel;

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

    /// @brief FXTextfield for EmissionClass
    FXTextField* myTextFieldEmissionClass;

    /// @brief vehicle shape row
    VShapeRow* myVShapeRow;

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

    /// @brief FXTextfield for MaxSpeedLat
    FXTextField* myTextFieldActionStepLenght;

    /// @brief update data fields
    void updateVehicleTypeValues();

private:
    /// @brief build row int
    FXTextField* buildRowInt(FXVerticalFrame* column, SumoXMLAttr tag);

    /// @brief build row float
    FXTextField* buildRowFloat(FXVerticalFrame* column, SumoXMLAttr tag);

    /// @brief build row
    FXTextField* buildRowString(FXVerticalFrame* column, SumoXMLAttr tag);

    /// @brief Invalidated copy constructor.
    GNEVehicleTypeDialog(const GNEVehicleTypeDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNEVehicleTypeDialog& operator=(const GNEVehicleTypeDialog&) = delete;
};

#endif
