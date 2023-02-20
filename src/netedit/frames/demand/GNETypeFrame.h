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
/// @file    GNETypeFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2018
///
// The Widget for edit Type elements (vehicle, person and container)
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/frames/GNEFrame.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNEVehicle;
class GNEVTypeDistributionsDialog;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNETypeFrame
 */
class GNETypeFrame : public GNEFrame {

public:
    // ===========================================================================
    // class TypeSelector
    // ===========================================================================

    class TypeSelector : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNETypeFrame::TypeSelector)

    public:
        /// @brief constructor
        TypeSelector(GNETypeFrame* typeFrameParent);

        /// @brief destructor
        ~TypeSelector();

        /// @brief get current Vehicle Type
        GNEDemandElement* getCurrentType() const;

        /// @brief set current Vehicle Type
        void setCurrentType(GNEDemandElement* vType);

        /// @brief refresh vehicle type selector
        void refreshTypeSelector();

        /// @brief refresh vehicle type selector (only IDs, without refreshing attributes)
        void refreshTypeSelectorIDs();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user select another element in ComboBox
        long onCmdSelectItem(FXObject*, FXSelector, void*);
        /// @}

    protected:
        FOX_CONSTRUCTOR(TypeSelector)

    private:
        /// @brief pointer to Frame Parent
        GNETypeFrame* myTypeFrameParent;

        /// @brief pointer to current vehicle type
        GNEDemandElement* myCurrentType;

        /// @brief comboBox with the list of vTypes
        MFXComboBoxIcon* myTypeComboBox;
    };

    // ===========================================================================
    // class TypeEditor
    // ===========================================================================

    class TypeEditor : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNETypeFrame::TypeEditor)

    public:
        /// @brief constructor
        TypeEditor(GNETypeFrame* typeFrameParent);

        /// @brief destructor
        ~TypeEditor();

        /// @brief show TypeEditor modul
        void showTypeEditorModule();

        /// @brief hide TypeEditor box
        void hideTypeEditorModule();

        /// @brief update TypeEditor modul
        void refreshTypeEditorModule();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when "Vreate Vehicle Type" button is clicked
        long onCmdCreateType(FXObject*, FXSelector, void*);

        /// @brief Called when "Delete/Reset Vehicle Type" button is clicked
        long onCmdDeleteResetType(FXObject*, FXSelector, void*);

        /// @brief Called when "Copy Vehicle Type" button is clicked
        long onCmdCopyType(FXObject*, FXSelector, void*);
        /// @}

    protected:
        FOX_CONSTRUCTOR(TypeEditor)

        /// @brief reset type
        void resetType();

        /// @brief delete type
        void deleteType();

    private:
        /// @brief pointer to vehicle type Frame Parent
        GNETypeFrame* myTypeFrameParent;

        /// @brief "create vehicle type" button
        FXButton* myCreateTypeButton;

        /// @brief "delete/reset vehicle type" button
        FXButton* myDeleteResetTypeButton;

        /// @brief "copy vehicle type"
        FXButton* myCopyTypeButton;
    };

    // ===========================================================================
    // class VTypeDistributions
    // ===========================================================================

    class VTypeDistributions  : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNETypeFrame::VTypeDistributions)

    public:
        /// @brief constructor
        VTypeDistributions(GNETypeFrame* typeFrameParent);

        /// @brief destructor
        ~VTypeDistributions();

        /// @brief get pointer to type frame Parent
        GNETypeFrame* getTypeFrameParent() const;

        /// @brief show VTypeDistributions  modul
        void showVTypeDistributionsModule();

        /// @brief hide group box
        void hideVTypeDistributionsModule();

        /// @brief get vType distribution dialog
        GNEVTypeDistributionsDialog* getVTypeDistributionsDialog() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when open dialog button is clicked
        long onCmdOpenDialog(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(VTypeDistributions)

    private:
        /// @brief pointer to type frame Parent
        GNETypeFrame* myTypeFrameParent = nullptr;

        /// @brief VType distribution dialog
        GNEVTypeDistributionsDialog* myVTypeDistributionsDialog = nullptr;
    };

    /**@brief Constructor
     * @brief viewParent GNEViewParent in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNETypeFrame(GNEViewParent* viewParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNETypeFrame();

    /// @brief show Frame
    void show();

    /// @brief get vehicle type selector
    TypeSelector* getTypeSelector() const;

protected:
    /// @brief function called after set a valid attribute in AttributeCreator/AttributeEditor/ParametersEditor/...
    void attributeUpdated(SumoXMLAttr attribute);

    /// @brief open GNEAttributesCreator extended dialog (used for editing advance attributes of Vehicle Types)
    void attributesEditorExtendedDialogOpened();

private:
    /// @brief vehicle type selector
    TypeSelector* myTypeSelector;

    /// @brief Vehicle Type editor (Create, copy, etc.)
    TypeEditor* myTypeEditor;

    /// @brief editorinternal vehicle type attributes
    GNEFrameAttributeModules::AttributesEditor* myTypeAttributesEditor = nullptr;

    /// @brief modul for open extended attributes dialog
    GNEFrameAttributeModules::AttributesEditorExtended* myAttributesEditorExtended = nullptr;

    /// @brief modul for open vType distributions dialog
    VTypeDistributions* myVTypeDistributions = nullptr;
};
