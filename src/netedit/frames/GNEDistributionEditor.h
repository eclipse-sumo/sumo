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

class GNEDistributionEditor {

public:
    // ===========================================================================
    // class declaration
    // ===========================================================================

    class AttributesEditor;

    // ===========================================================================
    // class AttributesEditorRow
    // ===========================================================================
    class AttributesEditorRow : protected FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE(GNEDistributionEditor::AttributesEditorRow)

    public:
        /// @brief constructor for ID attribute
        AttributesEditorRow(AttributesEditor* attributeEditorParent,
            const GNEAttributeProperties& ACAttr, const std::string& id);

        /// @brief constructor for type-probability attributes
        AttributesEditorRow(AttributesEditor* attributeEditorParent,
            const std::string& type, const std::string& probability);

        /// @brief destroy GNEAttributesCreatorRow (but don't delete)
        void destroy();

        /// @brief refresh current row
        void refreshAttributesEditorRow(const std::string& value);

        /// @brief check if current attribute of TextField/ComboBox is valid
        bool isAttributesEditorRowValid() const;

        /// @name FOX-callbacks
        /// @{

        /// @brief try to set new attribute value
        long onCmdSetAttribute(FXObject*, FXSelector, void*);

        /// @brief remove row
        long onCmdRemoveRow(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief default constructor
        AttributesEditorRow();

    private:
        /// @brief pointer to AttributesEditor parent
        AttributesEditor* myAttributesEditorParent;

        /// @brief pointer to attribute label
        MFXLabelTooltip* myIDLabel = nullptr;

        /// @brief delete row button
        MFXButtonTooltip* myDeleteRowButton = nullptr;

        /// @brief textField to modify the value of string attributes
        MFXTextFieldTooltip* myValueTextField = nullptr;

        /// @brief textField to modify the probability attribute
        MFXTextFieldTooltip* myProbabilityTextField = nullptr;
    };

    // ===========================================================================
    // class AttributesEditor
    // ===========================================================================

    class AttributesEditor : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNEDistributionEditor::AttributesEditor)

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

        /// @brief list of Attribute editor rows
        std::vector<AttributesEditorRow*> myAttributesEditorRows;

        /// @brief button for add new row
        FXButton* myAddButton = nullptr;
    };
};
