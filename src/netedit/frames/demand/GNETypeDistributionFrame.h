/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
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

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNETypeDistributionFrame
 */
class GNETypeDistributionFrame : public GNEFrame {

public:
    // ===========================================================================
    // class TypeDistributionEditor
    // ===========================================================================

    class TypeDistributionEditor : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNETypeDistributionFrame::TypeDistributionEditor)

    public:
        /// @brief constructor
        TypeDistributionEditor(GNETypeDistributionFrame* typeDistributionFrameParent);

        /// @brief destructor
        ~TypeDistributionEditor();

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
        FOX_CONSTRUCTOR(TypeDistributionEditor)

    private:
        /// @brief pointer to type distribution frame parent
        GNETypeDistributionFrame* myTypeDistributionFrameParent;

        /// @brief "create vehicle type" button
        FXButton* myCreateTypeButton = nullptr;

        /// @brief "delete vehicle type" button
        FXButton* myDeleteTypeButton = nullptr;
    };

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

        /// @brief get current Vehicle Type distribution
        GNEDemandElement* getCurrentTypeDistribution() const;

        /// @brief set current vehicle type distribution
        void setCurrentTypeDistribution(const GNEDemandElement* vTypeDistribution);

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
    TypeDistributionEditor* myTypeDistributionEditor = nullptr;

    /// @brief type distribution selector
    TypeDistributionSelector* myTypeDistributionSelector = nullptr;

    /// @brief editor for vehicle type attributes
    GNEFrameAttributeModules::AttributesEditor* myTypeAttributesEditor = nullptr;
};
