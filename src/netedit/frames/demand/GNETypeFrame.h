/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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

    class TypeSelector : public FXGroupBoxModule {
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
        FXComboBox* myTypeMatchBox;
    };

    // ===========================================================================
    // class TypeEditor
    // ===========================================================================

    class TypeEditor : public FXGroupBoxModule {
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

    /**@brief Constructor
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNETypeFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNETypeFrame();

    /// @brief show Frame
    void show();

    /// @brief get vehicle type selector
    TypeSelector* getTypeSelector() const;

protected:
    /// @brief function called after set a valid attribute in AttributeCreator/AttributeEditor/ParametersEditor/...
    void attributeUpdated();

    /// @brief open AttributesCreator extended dialog (used for editing advance attributes of Vehicle Types)
    void attributesEditorExtendedDialogOpened();

private:
    /// @brief vehicle type selector
    TypeSelector* myTypeSelector;

    /// @brief editorinternal vehicle type attributes
    GNEFrameAttributeModules::AttributesEditor* myTypeAttributesEditor;

    /// @brief modul for open extended attributes dialog
    GNEFrameAttributeModules::AttributesEditorExtended* myAttributesEditorExtended;

    /// @brief Vehicle Type editor (Create, copy, etc.)
    TypeEditor* myTypeEditor;
};
