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

public:
    /// @brief class for VClasses
    class VTypeAtributes : protected FXVerticalFrame {
        /// @brief FOX-declaration
        FXDECLARE(GNEVehicleTypeDialog::VTypeAtributes)

    public:
        /// @brief class for VClasses
        class VClassRow : protected FXHorizontalFrame {

        public:
            /// @brief constructor
            VClassRow(VTypeAtributes* VTypeAtributesParent, FXVerticalFrame* column);

            /// @brief set variables
            void setVariable();

            /// @brief update values
            void updateValues();

        protected:
            /// @brief set VClass texture
            void setVClassLabelImage();

            /// @brief FXComboBox for VClass
            FXComboBox* myComboBoxVClass;

            /// @brief label with image of VClass
            FXLabel* myComboBoxVClassLabelImage;

        private:
            /// @brief pointer to VTypeAtributes parent
            VTypeAtributes* myVTypeAtributesParent;
        };

        /// @brief class for VShapeRow
        class VShapeRow : protected FXHorizontalFrame {

        public:
            /// @brief constructor
            VShapeRow(VTypeAtributes* VTypeAtributesParent, FXVerticalFrame* column);

            /// @brief set variables
            void setVariable();

            /// @brief update values
            void updateValues();

        protected:
            /// @brief set VShape texture
            void setVShapeLabelImage();

            /// @brief FXComboBox for Shape
            FXComboBox* myComboBoxShape;

            /// @brief label with image of Shape
            FXLabel* myComboBoxShapeLabelImage;

        private:
            /// @brief pointer to VTypeAtributes parent
            VTypeAtributes* myVTypeAtributesParent;
        };

        /// @brief class used for represent rows with Vehicle Type parameters
        class VTypeAttributeRow : protected FXHorizontalFrame {
        public:
            /// @brief constructor fox TextFields (type: 0 -> int, 1 -> float, other: string)
            VTypeAttributeRow(VTypeAtributes* VTypeAtributesParent, FXVerticalFrame* verticalFrame, SumoXMLAttr attr, int type);

            /// @brief constructor for comboBox
            VTypeAttributeRow(VTypeAtributes* VTypeAtributesParent, FXVerticalFrame* verticalFrame, SumoXMLAttr attr, const std::vector<std::string>& values);

            /// @brief set Variablen in VehicleType
            void setVariable();

            /// @brief update value of Vehicle Type
            void updateValue();

        private:
            /// @brief pointer to VTypeAttributeParameters parent
            VTypeAtributes* myVTypeAtributesParent;

            /// @brief edited attribute
            SumoXMLAttr myAttr;

            /// @brief text field
            FXTextField* myTextField;

            /// @brief ComboBox for attributes with limited values
            FXComboBox* myComboBox;
        };

        /// @brief constructor
        VTypeAtributes(GNEVehicleTypeDialog* vehicleTypeDialog, FXHorizontalFrame* column);

        /// @brief build commmon attributes (A)
        void buildAttributesA(FXVerticalFrame* column);

        /// @brief build commmon attributes (B)
        void buildAttributesB(FXVerticalFrame* column);

        /// @brief build JunctionModel attributes (A)
        void buildJunctionModelAttributesA(FXVerticalFrame* column);

        /// @brief build JunctionModel attributes (B)
        void buildJunctionModelAttributesB(FXVerticalFrame* column);

        /// @brief update values
        void updateValues();

        /// @name FOX-callbacks
        /// @{
        /// @event called after change a Vehicle Type parameter
        long onCmdSetVariable(FXObject*, FXSelector, void*);

        /// @event called after change a Vehicle Type color
        long onCmdSetColor(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief fox need this
        VTypeAtributes() {}

        /// @brief FXTextfield for vehicleTypeID
        FXTextField* myTextFieldVehicleTypeID;

        /// @name Common Attributes
        /// @{

        /// @brief vehicle class row
        VClassRow* myVClassRow;

        /// @brief FXButton for Color
        FXButton* myButtonColor;

        /// @brief FXTextField for Color
        FXTextField* myTextFieldColor;

        /// @brief VTypeAttributeRow for Length
        VTypeAttributeRow* myLength;

        /// @brief VTypeAttributeRow for MinGap
        VTypeAttributeRow* myMinGap;

        /// @brief VTypeAttributeRow for MaxSpeed
        VTypeAttributeRow* myMaxSpeed;

        /// @brief VTypeAttributeRow for SpeedFactor
        VTypeAttributeRow* mySpeedFactor;

        /// @brief VTypeAttributeRow for SpeedDev
        VTypeAttributeRow* mySpeedDev;

        /// @brief VTypeAttributeRow for EmissionClass
        VTypeAttributeRow* myEmissionClass;

        /// @brief vehicle shape row
        VShapeRow* myVShapeRow;

        /// @brief VTypeAttributeRow for Width
        VTypeAttributeRow* myWidth;

        /// @brief VTypeAttributeRow for Filename
        VTypeAttributeRow* myFilename;

        /// @brief VTypeAttributeRow for LaneChangeModel
        VTypeAttributeRow* myLaneChangeModel;

        /// @brief VTypeAttributeRow for PersonCapacity
        VTypeAttributeRow* myPersonCapacity;

        /// @brief VTypeAttributeRow for ContainerCapacity
        VTypeAttributeRow* myContainerCapacity;

        /// @brief VTypeAttributeRow for BoardingDuration
        VTypeAttributeRow* myBoardingDuration;

        /// @brief VTypeAttributeRow for LoadingDuration
        VTypeAttributeRow* myLoadingDuration;

        /// @brief VTypeAttributeRow for LatAlignment
        VTypeAttributeRow* myLatAlignment;

        /// @brief VTypeAttributeRow for MinGapLat
        VTypeAttributeRow* myMinGapLat;

        /// @brief VTypeAttributeRow for MaxSpeedLat
        VTypeAttributeRow* myMaxSpeedLat;

        /// @brief VTypeAttributeRow for ActionStepLenght
        VTypeAttributeRow* myActionStepLenght;

        /// @}

        /// @name JM Attributes
        /// @{

        /// @brief VTypeAttributeRow for Crossing Gap
        VTypeAttributeRow* myJMCrossingGap;

        /// @brief VTypeAttributeRow for Ignore Keep Clear Time
        VTypeAttributeRow* myJMIgnoreKeepclearTime;
        
        /// @brief VTypeAttributeRow for Drive After Yellow Time
        VTypeAttributeRow* myJMDriveAfterYellowTime;
        
        /// @brief VTypeAttributeRow for Drive After Red Time
        VTypeAttributeRow* myJMDriveAfterRedTime;

        /// @brief VTypeAttributeRow for Drive Red Speed
        VTypeAttributeRow* myJMDriveRedSpeed;

        /// @brief VTypeAttributeRow for Ignore FOE Probability
        VTypeAttributeRow* myJMIgnoreFoeProb;

        /// @brief VTypeAttributeRow for Ignore FOE Speed
        VTypeAttributeRow* myJMIgnoreFoeSpeed;

        /// @brief VTypeAttributeRow for Sigma Minor
        VTypeAttributeRow* myJMSigmaMinor;

        /// @brief VTypeAttributeRow for Time GAP Minor
        VTypeAttributeRow* myJMTimeGapMinor;

        /// @brief VTypeAttributeRow for Impatience
        VTypeAttributeRow* myJMImpatience;

        /// @}

    private:
        /// @brief pointer to Vehicle Type dialog parent
        GNEVehicleTypeDialog* myVehicleTypeDialog;
    };

    /// @brief class for CarFollowingModel
    class CarFollowingModelParameters : public FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEVehicleTypeDialog::CarFollowingModelParameters)

    public:
        /// @brief constructor
        CarFollowingModelParameters(GNEVehicleTypeDialog* vehicleTypeDialog, FXHorizontalFrame* column);

        /// @brief refresh Car Following Model Fields
        void refreshCFMFields();

        /// @brief update values
        void updateValues();

        /// @name FOX-callbacks
        /// @{
        /// @event called after change a CFM variable
        long onCmdSetVariable(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief fox need this
        CarFollowingModelParameters() {}

        /// @brief class used for represent rows with Car Following Model parameters
        class CarFollowingModelRow : public FXHorizontalFrame {
        public:
            /// @brief constructor
            CarFollowingModelRow(CarFollowingModelParameters* carFollowingModelParametersParent, FXVerticalFrame* verticalFrame, SumoXMLAttr attr);

            /// @brief set Variablen in VehicleType
            void setVariable();

            /// @brief update value of Vehicle Type
            void updateValue();

        private:
            /// @brief pointer to CarFollowingModelParameters parent
            CarFollowingModelParameters* myCarFollowingModelParametersParent;

            /// @brief edited attribute
            SumoXMLAttr myAttr;

            /// @brief text field
            FXTextField* textField;

            /// @label Label with the Row attribute
            FXLabel* myLabel;
        };

    private:
        /// @brief pointer to Vehicle Type dialog parent
        GNEVehicleTypeDialog* myVehicleTypeDialog;

        /// @brief vector with the Car Following Model Row
        std::vector<CarFollowingModelRow*> myRows;

        /// @brief Row for CarFollowModel
        FXComboBox* myComboBoxCarFollowModel;

        /// @brief Vertical Frame for CarFollowingModelRow
        FXVerticalFrame* myVerticalFrameRows;

        /// @brief Row for Accel
        CarFollowingModelRow* myAccelRow;

        /// @brief Row for Decel
        CarFollowingModelRow* myDecelRow;

        /// @brief Row for aparent Decel
        CarFollowingModelRow* myApparentDecelRow;

        /// @brief Row for emergency Decel
        CarFollowingModelRow* myEmergencyDecelRow;

        /// @brief Row for Sigma
        CarFollowingModelRow* mySigmaRow;

        /// @brief Row for Tau
        CarFollowingModelRow* myTauRow;

        /// @brief Row for MinGapFactor
        CarFollowingModelRow* myMinGapFactorRow;

        /// @brief Row for MinGap (only for Kerner)
        CarFollowingModelRow* myKRow;

        /// @brief Row for MinGap (only for Kerner)
        CarFollowingModelRow* myPhiRow;

        /// @brief Row for MinGap (only for IDM)
        CarFollowingModelRow* myDeltaRow;

        /// @brief Row for MinGap(only for IDM)
        CarFollowingModelRow* mySteppingRow;

        /// @brief Row for Security (only for Wiedemann)
        CarFollowingModelRow* mySecurityRow;

        /// @brief Row for Estimation (only for Wiedemann)
        CarFollowingModelRow* myEstimationRow;

        /// @brief Row for TMP1
        CarFollowingModelRow* myTmp1Row;

        /// @brief Row for TMP2
        CarFollowingModelRow* myTmp2Row;

        /// @brief Row for TMP3
        CarFollowingModelRow* myTmp3Row;

        /// @brief Row for TMP4
        CarFollowingModelRow* myTmp4Row;

        /// @brief Row for TMP5
        CarFollowingModelRow* myTmp5Row;

        /// @brief Row for TrainType
        CarFollowingModelRow* myTrainTypeRow;

        /// @brief Row for TauLast
        CarFollowingModelRow* myTrauLastRow;

        /// @brief Row for Aprob
        CarFollowingModelRow* myAprobRow;

        /// @brief Row for Adapt Factor
        CarFollowingModelRow* myAdaptFactorRow;

        /// @brief Row for Adapt Time
        CarFollowingModelRow* myAdaptTimeRow;

        /// @brief temporal label for incomplete attributes
        FXLabel* myLabelIncompleteAttribute;
    };

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

private:
    /// @brief flag to check if current vehicleType is valid
    bool myVehicleTypeValid;

    /// @brief current sumo attribute invalid
    SumoXMLAttr myInvalidAttr;

    /// @brief Vehicle Type Common Attributes
    VTypeAtributes* myVTypeAtributes;

    /// @brief Car Following model parameters
    CarFollowingModelParameters* myCarFollowingModelParameters;

    /// @brief Invalidated copy constructor.
    GNEVehicleTypeDialog(const GNEVehicleTypeDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNEVehicleTypeDialog& operator=(const GNEVehicleTypeDialog&) = delete;
};

#endif
