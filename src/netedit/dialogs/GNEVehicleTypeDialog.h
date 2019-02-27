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

        /// @brief set variables
        void setVariable();

        /// @brief update values
        void updateValues();

    protected:
        /// @brief set VClass texture
        void setVClassLabelImage();

        /// @brief FXComboBox for VClass
        FXComboBox* myComboBoxVClass;
        
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

        /// @brief set variables
        void setVariable();

        /// @brief update values
        void updateValues();

    protected:
        /// @brief set VShape texture
        void setVShapeLabelImage();

        /// @brief FXComboBox for Shape
        FXComboBox* myComboBoxShape;
        
    private:
        /// @brief label with image of Shape
        FXLabel* myComboBoxShapeLabelImage;

        /// @brief pointer to Vehicle Type dialog parent
        GNEVehicleTypeDialog* myVehicleTypeDialog;
    };

    /// @brief class for VClasses
    class VTypeCommonAtributes : public FXGroupBox {

    public:
        /// @brief constructor
        VTypeCommonAtributes(GNEVehicleTypeDialog* vehicleTypeDialog, FXHorizontalFrame* column);

        /// @brief build commmon attributes (A)
        void buildCommonAttributesA(FXVerticalFrame* column);

        /// @brief build commmon attributes (B)
        void buildCommonAttributesB(FXVerticalFrame* column);

        /// @brief set variables
        void setVariable();

        /// @brief update values
        void updateValues();

    protected:
        /// @brief FXTextfield for vehicleTypeID
        FXTextField* myTextFieldVehicleTypeID;

        /// @brief vehicle class row
        VClassRow* myVClassRow;

        /// @brief FXTextfield for Color
        FXTextField* myTextFieldColor;

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

        /// @brief FXTextfield for ActionStepLenght
        FXTextField* myTextFieldActionStepLenght;

    private:
        /// @brief pointer to Vehicle Type dialog parent
        GNEVehicleTypeDialog* myVehicleTypeDialog;
    };

    /// @brief class for CarFollowingModel
    class CarFollowingModelParameters : public FXGroupBox {
        
    public:
        /// @brief constructor
        CarFollowingModelParameters(GNEVehicleTypeDialog* vehicleTypeDialog, FXHorizontalFrame* column);

        /// @brief refresh Car Following Model Fields
        void refreshCFMFields();

        /// @brief set variables
        void setVariable();

        /// @brief update values
        void updateValues();
        
    protected:

        class CarFollowingModelRow : public FXHorizontalFrame {
        public:
            CarFollowingModelRow(CarFollowingModelParameters *carFollowingModelParametersParent, FXVerticalFrame* verticalFrame, SumoXMLAttr attr);
            
            FXTextField* textField;
        private:

            FXLabel *myLabel;

        };

        /// @brief FXTextfield for CarFollowModel
        FXComboBox* myComboBoxCarFollowModel;

        /// @brief Vertical Frame for CarFollowingModelRow
        FXVerticalFrame *myVerticalFrameRows;

        /// @brief FXTextfield for Accel
        CarFollowingModelRow* myTextFieldAccel;

        /// @brief FXTextfield for Decel
        CarFollowingModelRow* myTextFieldDecel;

        /// @brief FXTextfield for aparent Decel
        CarFollowingModelRow* myTextFieldApparentDecel;
    
        /// @brief FXTextfield for emergency Decel
        CarFollowingModelRow* myTextFieldEmergencyDecel;

        /// @brief FXTextfield for Sigma
        CarFollowingModelRow* myTextFieldSigma;

        /// @brief FXTextfield for Tau
        CarFollowingModelRow* myTextFieldTau;

        /// @brief FXTextfield for MinGapFactor
        CarFollowingModelRow* myTextFieldMinGapFactor;

        /// @brief FXTextfield for MinGap (only for Kerner)
        CarFollowingModelRow* myTextFieldK;

        /// @brief FXTextfield for MinGap (only for Kerner)
        CarFollowingModelRow* myTextFieldPhi;

        /// @brief FXTextfield for MinGap (only for IDM)
        CarFollowingModelRow* myTextFieldDelta;

        /// @brief FXTextfield for MinGap(only for IDM)
        CarFollowingModelRow* myTextFieldStepping;

        /// @brief FXTextfield for Security (only for Wiedemann)
        CarFollowingModelRow* myTextFieldSecurity;

        /// @brief FXTextfield for Estimation (only for Wiedemann)
        CarFollowingModelRow* myTextFieldEstimation;

        /// @brief FXTextfield for TMP1
        CarFollowingModelRow* myTextFieldTmp1;

        /// @brief FXTextfield for TMP2
        CarFollowingModelRow* myTextFieldTmp2;

        /// @brief FXTextfield for TMP3
        CarFollowingModelRow* myTextFieldTmp3;

        /// @brief FXTextfield for TMP4
        CarFollowingModelRow* myTextFieldTmp4;

        /// @brief FXTextfield for TMP5
        CarFollowingModelRow* myTextFieldTmp5;

        /// @brief FXTextfield for TrainType
        CarFollowingModelRow* myTextFieldTrainType;

    private:
        /// @brief pointer to Vehicle Type dialog parent
        GNEVehicleTypeDialog* myVehicleTypeDialog;
    };

    /// @brief FOX needs this
    GNEVehicleTypeDialog() {}

    /// @brief update data fields
    void updateVehicleTypeValues();

    /// @brief build row int
    FXTextField* buildRowInt(FXPacker* column, SumoXMLAttr tag);

    /// @brief build row float
    FXTextField* buildRowFloat(FXPacker* column, SumoXMLAttr tag);

    /// @brief build row
    FXTextField* buildRowString(FXPacker* column, SumoXMLAttr tag);

private:
    /// @brief flag to check if current vehicleType is valid
    bool myVehicleTypeValid;

    /// @brief current sumo attribute invalid
    SumoXMLAttr myInvalidAttr;

    /// @brief Vehicle Type Common Attributes
    VTypeCommonAtributes* myVTypeCommonAtributes;

    /// @brief Car Following model parameters
    CarFollowingModelParameters* myCarFollowingModelParameters;

    /// @brief Invalidated copy constructor.
    GNEVehicleTypeDialog(const GNEVehicleTypeDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNEVehicleTypeDialog& operator=(const GNEVehicleTypeDialog&) = delete;
};

#endif
