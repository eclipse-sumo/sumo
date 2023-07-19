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
/// @file    GNERouteDistributionFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2023
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
 * @class GNERouteDistributionFrame
 */
class GNERouteDistributionFrame : public GNEFrame {

public:
    // ===========================================================================
    // class declaration
    // ===========================================================================

    class RouteDistributionAttributesEditor;

    // ===========================================================================
    // class RouteDistributionEditor
    // ===========================================================================

    class RouteDistributionEditor : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNERouteDistributionFrame::RouteDistributionEditor)

    public:
        /// @brief constructor
        RouteDistributionEditor(GNERouteDistributionFrame* typeDistributionFrameParent);

        /// @brief destructor
        ~RouteDistributionEditor();

        /// @name FOX-callbacks
        /// @{

        /// @brief Called when "create type distribution" button is clicked
        long onCmdCreateRoute(FXObject*, FXSelector, void*);

        /// @brief Called when "Delete type distribution" button is clicked
        long onCmdDeleteRoute(FXObject*, FXSelector, void*);

        /// @brief Called when "Delete type distribution" button is updated
        long onUpdDeleteRoute(FXObject* sender, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX needs this
        FOX_CONSTRUCTOR(RouteDistributionEditor)

    private:
        /// @brief pointer to type distribution frame parent
        GNERouteDistributionFrame* myRouteDistributionFrameParent;

        /// @brief "create route" button
        FXButton* myCreateRouteButton = nullptr;

        /// @brief "delete route" button
        FXButton* myDeleteRouteButton = nullptr;
    };

    // ===========================================================================
    // class RouteDistributionSelector
    // ===========================================================================

    class RouteDistributionSelector : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNERouteDistributionFrame::RouteDistributionSelector)

    public:
        /// @brief constructor
        RouteDistributionSelector(GNERouteDistributionFrame* typeFrameParent);

        /// @brief destructor
        ~RouteDistributionSelector();

        /// @brief get current Vehicle Route distribution
        GNEDemandElement* getCurrentRouteDistribution() const;

        /// @brief set current route distribution
        void setCurrentRouteDistribution(const GNEDemandElement* routeDistribution);

        /// @brief refresh modul
        void refreshRouteDistributionSelector();

        /// @name FOX-callbacks
        /// @{

        /// @brief Called when the user select type distribution in ComboBox
        long onCmdSelectRouteDistribution(FXObject*, FXSelector, void*);

        /// @brief update type distribution comboBox
        long onCmdUpdateRouteDistribution(FXObject* sender, FXSelector, void*);

        /// @}

    protected:
        FOX_CONSTRUCTOR(RouteDistributionSelector)

    private:
        /// @brief pointer to Frame Parent
        GNERouteDistributionFrame* myRouteDistributionFrameParent;

        /// @brief comboBox with the list of type distributions
        FXComboBox* myRouteComboBox = nullptr;

        /// @brief current type distribution
        std::string myCurrentRouteDistribution;
    };

    // ===========================================================================
    // class RouteDistributionAttributesEditorRow
    // ===========================================================================

    class RouteDistributionAttributesEditorRow : protected FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE(GNERouteDistributionFrame::RouteDistributionAttributesEditorRow)

    public:
        /// @brief constructor for ID attribute
        RouteDistributionAttributesEditorRow(RouteDistributionAttributesEditor* attributeEditorParent,
            const GNEAttributeProperties& ACAttr, const std::string& id);

        /// @brief constructor for type-probability attributes
        RouteDistributionAttributesEditorRow(RouteDistributionAttributesEditor* attributeEditorParent,
            const std::string& type, const std::string& probability);

        /// @brief destroy GNEAttributesCreatorRow (but don't delete)
        void destroy();

        /// @brief refresh current row
        void refreshRouteDistributionAttributesEditorRow(const std::string& value);

        /// @brief check if current attribute of TextField/ComboBox is valid
        bool isRouteDistributionAttributesEditorRowValid() const;

        /// @name FOX-callbacks
        /// @{

        /// @brief try to set new attribute value
        long onCmdSetAttribute(FXObject*, FXSelector, void*);

        /// @brief remove row
        long onCmdRemoveRow(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief default constructor
        RouteDistributionAttributesEditorRow();

    private:
        /// @brief pointer to RouteDistributionAttributesEditor parent
        RouteDistributionAttributesEditor* myRouteDistributionAttributesEditorParent;

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
    // class RouteDistributionAttributesEditor
    // ===========================================================================

    class RouteDistributionAttributesEditor : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNERouteDistributionFrame::RouteDistributionAttributesEditor)

    public:
        /// @brief constructor
        RouteDistributionAttributesEditor(GNERouteDistributionFrame* typeDistributionFrameParent);

        /// @brief show attributes of multiple ACs
        void showAttributeEditorModule();

        /// @brief hide attribute editor
        void hideRouteDistributionAttributesEditorModule();

        /// @brief refresh attribute editor (only the valid values will be refresh)
        void refreshAttributeEditor();

        /// @brief pointer to GNEFrame parent
        GNERouteDistributionFrame* getRouteDistributionFrameParent() const;

        /// @name FOX-callbacks
        /// @{

        /// @brief Called when user press the add button
        long onCmdAddRow(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief fox need this
        FOX_CONSTRUCTOR(RouteDistributionAttributesEditor)

    private:
        /// @brief pointer to type distribution frame parent
        GNERouteDistributionFrame* myRouteDistributionFrameParent;

        /// @brief list of Attribute editor rows
        std::vector<RouteDistributionAttributesEditorRow*> myRouteDistributionAttributesEditorRows;

        /// @brief button for add new row
        FXButton* myAddButton = nullptr;
    };

    /**@brief Constructor
     * @brief viewParent GNEViewParent in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNERouteDistributionFrame(GNEViewParent* viewParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNERouteDistributionFrame();

    /// @brief show Frame
    void show();

    /// @brief get type distribution selector
    RouteDistributionSelector* getRouteDistributionSelector() const;

protected:
    /// @brief function called after set a valid attribute in AttributeCreator/AttributeEditor/ParametersEditor/...
    void attributeUpdated(SumoXMLAttr attribute);

private:
    /// @brief type editor
    RouteDistributionEditor* myRouteDistributionEditor = nullptr;

    /// @brief type distribution selector
    RouteDistributionSelector* myRouteDistributionSelector = nullptr;

    /// @brief editor for route attributes
    RouteDistributionAttributesEditor* myRouteDistributionAttributesEditor = nullptr;
};
