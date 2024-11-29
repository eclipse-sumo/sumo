/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    GNEAttributesEditorRow.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2024
///
// Row used for edit attributes in GNEAttributesEditor
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/elements/GNEAttributeCarrier.h>

#include "GNEAttributesEditor.h"

// ===========================================================================
// class declaration
// ===========================================================================

class GNEAttributesEditor;
class GNEAttributeCarrier;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEAttributesEditorRow : protected FXHorizontalFrame {
    /// @brief FOX-declaration
    FXDECLARE(GNEAttributesEditorRow)

public:
    /// @brief constructor
    GNEAttributesEditorRow(GNEAttributesEditor* attributeTable);

    /// @brief show attribute row
    void showAttributeRow(const GNEAttributeProperties& attrProperty);

    /// @brief hide attribute row
    void hideAttributeRow();

    /// @brief check if current attribute row is shown
    bool isAttributeRowShown() const;

    /// @brief check if current attribute of TextField/ComboBox is valid
    bool isCurrentValueValid() const;

    /// @name FOX-callbacks
    /// @{

    /// @brief try to set new attribute value
    long onCmdSetAttribute(FXObject* obj, FXSelector, void*);

    /// @brief called when user press the enable/disable checkbox
    long onCmdToogleEnableAttribute(FXObject*, FXSelector, void*);

    /// @brief open model dialog for edit color
    long onCmdOpenColorDialog(FXObject*, FXSelector, void*);

    /// @brief open model dialog for edit allow
    long onCmdOpenAllowDialog(FXObject*, FXSelector, void*);

    /// @brief inspect parent
    long onCmdInspectParent(FXObject*, FXSelector, void*);

    /// @brief inspect vType/VTypeDistribution parent
    long onCmdMoveLaneUp(FXObject*, FXSelector, void*);

    /// @brief inspect vType/VTypeDistribution parent
    long onCmdMoveLaneDown(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief default constructor (needed for FOX)
    GNEAttributesEditorRow();

    /// @brief get value from edited ACs
    const std::string getAttributeValue(const GNEAttributeProperties& attrProperty) const;

    /// @brief show attribute checkButton
    void showAttributeCheckButton(const GNEAttributeProperties& attrProperty, const bool value, const bool enabled);

    /// @brief show attribute parent
    void showAttributeParent(const GNEAttributeProperties& attrProperty, const bool enabled);

    /// @brief show attribute vClass
    void showAttributeVClass(const GNEAttributeProperties& attrProperty, const bool enabled);

    /// @brief show attribute color
    void showAttributeColor(const GNEAttributeProperties& attrProperty, const bool enabled);

    /// @brief show attribute label
    void showAttributeLabel(const GNEAttributeProperties& attrProperty);

    /// @brief show value for check button
    void showValueCheckButton(const GNEAttributeProperties& attrProperty, const std::string& value, const bool enabled, const bool computed);

    /// @brief show value for combo Box
    void showValueComboBox(const GNEAttributeProperties& attrProperty, const std::string& value, const bool enabled, const bool computed);

    /// @brief show value for strings
    void showValueString(const GNEAttributeProperties& attrProperty, const std::string& value, const bool enabled, const bool computed);

    /// @brief show move lane buttons
    void showMoveLaneButtons(const std::string& laneID);

    /// @brief check if enable or disable depending of current supermode
    void enableDependingOfSupermode(const GNEAttributeProperties& attrProperty);

    /// @brief check junction merging
    /// bool mergeJunction(SumoXMLAttr attr, const std::string& newVal) const;

private:
    /// @brief pointer to attribute table parent
    GNEAttributesEditor* myAttributeTable;

    /// @brief edited attribute
    SumoXMLAttr myAttribute = SUMO_ATTR_NOTHING;

    /// @brief pointer to attribute label
    MFXLabelTooltip* myAttributeLabel = nullptr;

    /// @brief pointer to attribute menu check
    FXCheckButton* myAttributeCheckButton = nullptr;

    /// @brief pointer to attribute parent button
    MFXButtonTooltip* myAttributeParentButton = nullptr;

    /// @brief pointer to attribute vClass button
    MFXButtonTooltip* myAttributeVClassButton = nullptr;

    /// @brief pointer to attribute color button
    MFXButtonTooltip* myAttributeColorButton = nullptr;

    /// @brief pointer to text field for modify values
    MFXTextFieldTooltip* myValueTextField = nullptr;

    /// @brief pointer to combo box for select choices
    MFXComboBoxIcon* myValueComboBox = nullptr;

    /// @brief pointer to menu check
    FXCheckButton* myValueCheckButton = nullptr;

    /// @brief Button for move lane up
    MFXButtonTooltip* myValueLaneUpButton = nullptr;

    /// @brief Button for move lane down
    MFXButtonTooltip* myValueLaneDownButton = nullptr;

    /// @brief Invalidated copy constructor.
    GNEAttributesEditorRow(GNEAttributesEditorRow*) = delete;

    /// @brief Invalidated assignment operator.
    GNEAttributesEditorRow& operator=(GNEAttributesEditorRow*) = delete;
};
