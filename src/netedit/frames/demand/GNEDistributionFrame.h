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
#include <netedit/frames/GNEFrameAttributeModules.h>
#include <utils/xml/SUMOXMLDefinitions.h>

// ===========================================================================
// class definitions
// ===========================================================================

class GNEDistributionFrame {

public:
    // ===========================================================================
    // class declaration
    // ===========================================================================

    class DistributionValuesEditor;
    class DistributionSelector;

    // ===========================================================================
    // class DistributionEditor
    // ===========================================================================

    class DistributionEditor : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNEDistributionFrame::DistributionEditor)

        /// @brief declare friend class
        friend class DistributionValuesEditor;

    public:
        /// @brief constructor
        DistributionEditor(GNEFrame* frameParent, GUIIcon icon);

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

        /// @brief distribution selector
        DistributionSelector* myDistributionSelector = nullptr;

        /// @brief "create vehicle type" button
        FXButton* myCreateTypeButton = nullptr;

        /// @brief "delete vehicle type" button
        FXButton* myDeleteTypeButton = nullptr;
    };

    // ===========================================================================
    // class DistributionSelector
    // ===========================================================================

    class DistributionSelector : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNEDistributionFrame::DistributionSelector)

        /// @brief declare friend class
        friend class DistributionValuesEditor;

    public:
        /// @brief constructor
        DistributionSelector(GNEFrame* frameParent, SumoXMLTag distributionTag);

        /// @brief destructor
        ~DistributionSelector();

        /// @brief get distribution tag
        SumoXMLTag getDistributionTag() const;

        /// @brief current type distribution
        GNEDemandElement *getCurrentDistribution() const;

        /// @brief refresh modul
        void refreshDistributionSelector();

        /// @name FOX-callbacks
        /// @{

        /// @brief Called when the user select type distribution in ComboBox
        long onCmdSelectTypeDistribution(FXObject*, FXSelector, void*);

        /// @brief update type distribution comboBox
        long onCmdUpdateTypeDistribution(FXObject* sender, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(DistributionSelector)

    private:
        /// @brief pointer to Frame Parent
        GNEFrame* myFrameParent;

        /// @brief distribution tag
        SumoXMLTag myDistributionTag;

        /// @brief pointer to distribution editor
        DistributionEditor* myDistributionEditor = nullptr;

        /// @brief attributes editor
        GNEFrameAttributeModules::AttributesEditor* myAttributesEditor = nullptr;

        /// @brief distribution values editor
        DistributionValuesEditor* myDistributionValuesEditor = nullptr;

        /// @brief comboBox with the list of type distributions
        FXComboBox* myTypeComboBox = nullptr;

        /// @brief current type distribution
        std::string myCurrentTypeDistribution;

        /// @brief map with the original vTypeDistribution attributes
        std::pair<std::string, std::string> myOriginalValues;
    };

    // ===========================================================================
    // class DistributionRow
    // ===========================================================================

    class DistributionRow : protected FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE(GNEDistributionFrame::DistributionRow)

    public:
        /// @brief constructor for type-probability attributes
        DistributionRow(DistributionValuesEditor* attributeEditorParent,
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
        /// @brief FOX need this
        FOX_CONSTRUCTOR(DistributionRow)

        /// @brief check if the given ID is valid
        bool isValidKey() const;

    private:
        /// @brief pointer to DistributionValuesEditor parent
        DistributionValuesEditor* myDistributionValuesEditorParent;

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
    // class DistributionValuesEditor
    // ===========================================================================

    class DistributionValuesEditor : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNEDistributionFrame::DistributionValuesEditor)

        /// @brief declare friend class
        friend class DistributionRow;

    public:
        /// @brief constructor
        DistributionValuesEditor(GNEFrame* frameParent, DistributionEditor* distributionEditor,
                                 DistributionSelector* distributionSelector,
                                 GNEFrameAttributeModules::AttributesEditor* attributesEditor,
                                 SumoXMLTag distributionValueTag);

        /// @brief show attributes of multiple ACs
        void showAttributeEditorModule();

        /// @brief hide attribute editor
        void hideDistributionValuesEditorModule();

        /// @brief refresh attribute editor (only the valid values will be refresh)
        void refreshAttributeEditor();

        /// @brief pointer to GNEFrame parent
        GNEFrame* getFrameParent() const;

        /// @name FOX-callbacks
        /// @{

        /// @brief Called when user press the add button
        long onCmdAddRow(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(DistributionValuesEditor)

    private:
        /// @brief pointer to type distribution frame parent
        GNEFrame* myFrameParent;

        /// @brief distribution editor
        DistributionEditor* myDistributionEditor;
        
        /// @brief distribution selector
        DistributionSelector* myDistributionSelector;
        
        /// @brief attributes editor
        GNEFrameAttributeModules::AttributesEditor* myAttributesEditor;

        /// @brief distribution value tag
        SumoXMLTag myDistributionValueTag;

        /// @brief list of Attribute editor rows
        std::vector<DistributionRow*> myDistributionRows;

        /// @brief button for add new row
        FXButton* myAddButton = nullptr;
    };
};
