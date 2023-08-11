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
/// @file    GNETypeDistributionFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2023
///
// The Widget for edit type distribution elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/frames/GNEFrame.h>
#include <netedit/frames/GNEDistributionEditor.h>

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNETypeDistributionFrame
 */
class GNETypeDistributionFrame : public GNEFrame {

public:
    // ===========================================================================
    // class TypeDistributionSelector
    // ===========================================================================

    class TypeDistributionSelector : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNETypeDistributionFrame::TypeDistributionSelector)

    public:
        /// @brief constructor
        TypeDistributionSelector(GNETypeDistributionFrame* typeFrameParent);

        /// @brief destructor
        ~TypeDistributionSelector();

        /// @brief refresh modul
        void refreshTypeDistributionSelector();

        /// @name FOX-callbacks
        /// @{

        /// @brief Called when the user select type distribution in ComboBox
        long onCmdSelectTypeDistribution(FXObject*, FXSelector, void*);

        /// @brief update type distribution comboBox
        long onCmdUpdateTypeDistribution(FXObject* sender, FXSelector, void*);

        /// @}

    protected:
        FOX_CONSTRUCTOR(TypeDistributionSelector)

    private:
        /// @brief pointer to Frame Parent
        GNETypeDistributionFrame* myTypeDistributionFrameParent;

        /// @brief comboBox with the list of type distributions
        FXComboBox* myTypeComboBox = nullptr;

        /// @brief current type distribution
        std::string myCurrentTypeDistribution;

        /// @brief map with the original vTypeDistribution attributes
        std::pair<std::string, std::string> myOriginalValues;
    };

    /**@brief Constructor
     * @brief viewParent GNEViewParent in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNETypeDistributionFrame(GNEViewParent* viewParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNETypeDistributionFrame();

    /// @brief show Frame
    void show();

    /// @brief get type distribution selector
    TypeDistributionSelector* getTypeDistributionSelector() const;

protected:
    /// @brief function called after set a valid attribute in AttributeCreator/AttributeEditor/ParametersEditor/...
    void attributeUpdated(SumoXMLAttr attribute);

private:
    /// @brief type editor
    GNEDistributionEditor::TypeDistributionEditor* myTypeDistributionEditor = nullptr;

    /// @brief type distribution selector
    TypeDistributionSelector* myTypeDistributionSelector = nullptr;

    /// @brief distribution attributes editor
    GNEFrameAttributeModules::AttributesEditor* myTypeAttributesEditor = nullptr;

    /// @brief editor for distribution editor
    GNEDistributionEditor::AttributesEditor* myDistributionEditor = nullptr;
};
