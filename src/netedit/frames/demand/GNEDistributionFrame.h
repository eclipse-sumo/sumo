/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GNEFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2023
///
// The Widget for edit type distribution elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/frames/GNEFrame.h>

// ===========================================================================
// class definitions
// ===========================================================================

class GNEDistributionFrame {

public:
    // ===========================================================================
    // class declaration
    // ===========================================================================

    class AttributesEditor;

    // ===========================================================================
    // class DistributionEditor
    // ===========================================================================

    class DistributionEditor : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNEDistributionFrame::DistributionEditor)

    public:
        /// @brief constructor
        DistributionEditor(GNEFrame* frameParent);

        /// @brief destructor
        ~DistributionEditor();

        /// @name FOX-callbacks
        /// @{

        /// @brief Called when "create type distribution" button is clicked
        long onCmdCreateType(FXObject*, FXSelector, void*);

        /// @brief Called when "Delete type distribution" button is clicked
        long onCmdDeleteType(FXObject*, FXSelector, void*);

        /// @brief Called when "Delete type distribution" button is updated
        long onUpdDeleteType(FXObject* sender, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX needs this
        FOX_CONSTRUCTOR(DistributionEditor)

    private:
        /// @brief pointer to type distribution frame parent
        GNEFrame* myFrameParent;

        /// @brief "create vehicle type" button
        FXButton* myCreateTypeButton = nullptr;

        /// @brief "delete vehicle type" button
        FXButton* myDeleteTypeButton = nullptr;
    };

    // ===========================================================================
    // class AttributeRow
    // ===========================================================================

    class AttributeRow : protected FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE(GNEDistributionFrame::AttributeRow)

    public:
        /// @brief constructor for ID attribute
        AttributeRow(AttributesEditor* attributeEditorParent,
            const GNEAttributeProperties& ACAttr, const std::string& attribute);

        /// @brief destroy GNEAttributesCreatorRow (but don't delete)
        void destroy();

        /// @brief refresh current row
        void refreshAttributeRow(const std::string& value);

        /// @brief check if current attribute of TextField/ComboBox is valid
        bool isAttributeRowValid() const;

        /// @name FOX-callbacks
        /// @{

        /// @brief try to set new attribute value
        long onCmdSetAttribute(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief default constructor
        AttributeRow();

    private:
        /// @brief pointer to AttributesEditor parent
        AttributesEditor* myAttributesEditorParent;

        /// @brief pointer to attribute label
        MFXLabelTooltip* myAttributeLabel = nullptr;

        /// @brief textField to modify the value of string attributes
        MFXTextFieldTooltip* myValueTextField = nullptr;

        /// @brief ACAttribute
        GNEAttributeProperties myACAttr;
    };

    // ===========================================================================
    // class DistributionRow
    // ===========================================================================

    class DistributionRow : protected FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE(GNEDistributionFrame::DistributionRow)

    public:
        /// @brief constructor for type-probability attributes
        DistributionRow(AttributesEditor* attributeEditorParent,
            const GNEDemandElement* key, const double probability);

        /// @brief destroy GNEAttributesCreatorRow (but don't delete)
        void destroy();

        /// @brief refresh current row
        void refreshDistributionRow(const GNEDemandElement* key, const double value);

        /// @brief check if current attribute of TextField/ComboBox is valid
        bool isDistributionRowValid() const;

        /// @name FOX-callbacks
        /// @{

        /// @brief try to set new attribute value
        long onCmdSetAttribute(FXObject* obj, FXSelector, void*);

        /// @brief remove row
        long onCmdRemoveRow(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief default constructor
        DistributionRow();

        /// @brief check if the given ID is valid
        bool isValidKey() const;

    private:
        /// @brief pointer to AttributesEditor parent
        AttributesEditor* myAttributesEditorParent;

        /// @brief delete row button
        MFXButtonTooltip* myDeleteRowButton = nullptr;

        /// @brief textField to modify the value of string attributes
        MFXComboBoxIcon* myComboBoxKeys = nullptr;

        /// @brief textField to modify the probability attribute
        MFXTextFieldTooltip* myProbabilityTextField = nullptr;

        /// @brief current Key
        const GNEDemandElement* myKey = nullptr;

        /// @brief probability
        double myProbability = 0;
    };

    // ===========================================================================
    // class AttributesEditor
    // ===========================================================================

    class AttributesEditor : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNEDistributionFrame::AttributesEditor)

    public:
        /// @brief constructor
        AttributesEditor(GNEFrame* frameParent);

        /// @brief show attributes of multiple ACs
        void showAttributeEditorModule();

        /// @brief hide attribute editor
        void hideAttributesEditorModule();

        /// @brief refresh attribute editor (only the valid values will be refresh)
        void refreshAttributeEditor();

        /// @brief pointer to GNEFrame parent
        GNEFrame* getFrameParent() const;

        /// @brief getdistribution
        GNEDemandElement* getDistribution() const;

        /// @brief distribution
        void setDistribution(GNEDemandElement* distribution);

        /// @name FOX-callbacks
        /// @{

        /// @brief Called when user press the add button
        long onCmdAddRow(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief fox need this
        FOX_CONSTRUCTOR(AttributesEditor)

    private:
        /// @brief pointer to type distribution frame parent
        GNEFrame* myFrameParent;

        /// @brief distribution
        GNEDemandElement* myDistribution = nullptr;

        /// @brief attribute 
        AttributeRow* myIDAttributeRow = nullptr;

        /// @brief list of Attribute editor rows
        std::vector<DistributionRow*> myDistributionRows;

        /// @brief button for add new row
        FXButton* myAddButton = nullptr;
    };
};
