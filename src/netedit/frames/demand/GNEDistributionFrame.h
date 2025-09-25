/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEDistributionFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2023
///
// The Widget for edit distribution elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/frames/GNEFrame.h>
#include <utils/xml/SUMOXMLDefinitions.h>

// ===========================================================================
// class definitions
// ===========================================================================

class GNEAttributesEditor;
class GNEDemandElement;
class MFXComboBoxIcon;
class MFXTextFieldIcon;

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
        DistributionEditor(GNEFrame* frameParent, SumoXMLTag distributionTag, GUIIcon icon);

        /// @brief destructor
        ~DistributionEditor();

        /// @brief get distribution tag
        SumoXMLTag getDistributionTag() const;

        /// @name FOX-callbacks
        /// @{

        /// @brief Called when "create distribution" button is clicked
        long onCmdCreateDistribution(FXObject*, FXSelector, void*);

        /// @brief Called when "Delete distribution" button is clicked
        long onCmdDeleteDistribution(FXObject*, FXSelector, void*);

        /// @brief Called when "Delete distribution" button is updated
        long onUpdDeleteDistribution(FXObject* sender, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX needs this
        FOX_CONSTRUCTOR(DistributionEditor)

    private:
        /// @brief pointer to frame parent
        GNEFrame* myFrameParent;

        /// @briuef distribution tag
        SumoXMLTag myDistributionTag;

        /// @brief distribution selector
        DistributionSelector* myDistributionSelector = nullptr;

        /// @brief "create Distribution" button
        MFXButtonTooltip* myCreateDistributionButton = nullptr;

        /// @brief "delete Distribution" button
        MFXButtonTooltip* myDeleteDistributionButton = nullptr;
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
        DistributionSelector(GNEFrame* frameParent);

        /// @brief destructor
        ~DistributionSelector();

        /// @brief refresh modul
        void setDistribution(GNEDemandElement* distribution);

        /// @brief current distribution
        GNEDemandElement* getCurrentDistribution() const;

        /// @brief refresh distribution IDs (only call when user change ID in internal attributes)
        void refreshDistributionIDs();

        /// @brief refresh modul
        void refreshDistributionSelector();

        /// @name FOX-callbacks
        /// @{

        /// @brief Called when the user select distribution in ComboBox
        long onCmdSelectDistribution(FXObject*, FXSelector, void*);

        /// @brief update distribution comboBox
        long onCmdUpdateDistribution(FXObject* sender, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(DistributionSelector)

        /// @brief fill distribution comboBox
        std::map<std::string, GNEDemandElement*> fillDistributionComboBox();

    private:
        /// @brief pointer to Frame Parent
        GNEFrame* myFrameParent;

        /// @brief pointer to distribution editor
        DistributionEditor* myDistributionEditor = nullptr;

        /// @brief attributes editor
        GNEAttributesEditor* myAttributesEditor = nullptr;

        /// @brief distribution values editor
        DistributionValuesEditor* myDistributionValuesEditor = nullptr;

        /// @brief comboBox with the list of distributions
        MFXComboBoxIcon* myDistributionsComboBox = nullptr;

        /// @brief current distribution element
        GNEDemandElement* myCurrentDistribution = nullptr;
    };

    // ===========================================================================
    // class DistributionRow
    // ===========================================================================

    class DistributionRow : protected FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE(GNEDistributionFrame::DistributionRow)

    public:
        /// @brief constructor for key-probability attributes
        DistributionRow(DistributionValuesEditor* attributeEditorParent,
                        const GNEDemandElement* key, const double probability);

        /// @brief destroy DistributionRow (but don't delete)
        void destroy();

        /// @brief refresh comboBox
        void refreshRow();

        /// @brief get current probability
        double getProbability() const;

        /// @name FOX-callbacks
        /// @{

        /// @brief try to set new key
        long onCmdSetKey(FXObject*, FXSelector, void*);

        /// @brief try to set new probability
        long onCmdSetProbability(FXObject*, FXSelector, void*);

        /// @brief remove row
        long onCmdRemoveRow(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(DistributionRow)

        /// @brief check if the given ID is valid
        bool isValidNewKey() const;

    private:
        /// @brief pointer to DistributionValuesEditor parent
        DistributionValuesEditor* myDistributionValuesEditorParent;

        /// @brief delete row button
        MFXButtonTooltip* myDeleteRowButton = nullptr;

        /// @brief label
        FXLabel* myIconLabel = nullptr;

        /// @brief comboBox with keys
        MFXComboBoxIcon* myComboBoxKeys = nullptr;

        /// @brief textField to modify the probability attribute
        MFXTextFieldIcon* myProbabilityTextField = nullptr;

        /// @brief current probability
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
                                 GNEAttributesEditor* attributesEditor,
                                 SumoXMLTag distributionValueTag);

        /// @brief show attributes of multiple ACs
        void showDistributionValuesEditor();

        /// @brief hide attribute editor
        void hideDistributionValuesEditor();

        /// @brief destroy and remake rows
        void remakeRows();

        /// @brief refresh rows
        void refreshRows();

        /// @brief pointer to GNEFrame parent
        GNEFrame* getFrameParent() const;

        /// @brief update sum label
        void updateSumLabel();

        /// @name FOX-callbacks
        /// @{

        /// @brief Called when user press the add button
        long onCmdAddRow(FXObject*, FXSelector, void*);

        /// @brief Called when sum button button is updated
        long onUpdAddRow(FXObject* sender, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(DistributionValuesEditor)

    private:
        /// @brief pointer to frame parent
        GNEFrame* myFrameParent;

        /// @brief distribution editor
        DistributionEditor* myDistributionEditor;

        /// @brief distribution selector
        DistributionSelector* myDistributionSelector;

        /// @brief attributes editor
        GNEAttributesEditor* myAttributesEditor;

        /// @brief list of Attribute editor rows
        std::vector<DistributionRow*> myDistributionRows;

        /// @brief bot frame
        FXHorizontalFrame* myBotFrame = nullptr;

        /// @brief sum label
        FXLabel* mySumLabel = nullptr;
    };
};
