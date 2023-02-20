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
/// @file    GNEAttributesCreatorRow.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2022
///
// Row used in AttributesCreator
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/MFXGroupBoxModule.h>
#include <utils/foxtools/MFXLabelTooltip.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEFrame;
class GNEAttributesCreator;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEAttributesCreatorRow : public FXHorizontalFrame {
    /// @brief FOX-declaration
    FXDECLARE(GNEAttributesCreatorRow)

public:
    /// @brief constructor
    GNEAttributesCreatorRow(GNEAttributesCreator* AttributesCreatorParent, const GNEAttributeProperties& attrProperties);

    /// @brief destroy GNEAttributesCreatorRow (but don't delete)
    void destroy();

    /// @brief return Attr
    const GNEAttributeProperties& getAttrProperties() const;

    /// @brief return value
    std::string getValue() const;

    /// @brief return status of label checkbox button
    bool getAttributeCheckButtonCheck() const;

    /// @brief enable or disable label checkbox button for Terminatel attributes
    void setAttributeCheckButtonCheck(bool value);

    /// @brief enable row
    void enableAttributesCreatorRow();

    /// @brief disable row
    void disableAttributesCreatorRow();

    /// @brief check if row is enabled
    bool isAttributesCreatorRowEnabled() const;

    /// @brief refresh row
    void refreshRow();

    /// @brief disable Rows
    void disableRow();

    /// @brief check if current attribute is valid
    bool isAttributeValid() const;

    /// @brief get GNEAttributesCreator parent
    GNEAttributesCreator* getAttributesCreatorParent() const;

    /// @name FOX-callbacks
    /// @{
    /// @brief called when user set the value of an attribute of type int/float/string/bool
    long onCmdSetAttribute(FXObject*, FXSelector, void*);

    /// @brief called when user press the open dialog button
    long onCmdOpenAttributeDialog(FXObject*, FXSelector, void*);
    /// @}

protected:
    FOX_CONSTRUCTOR(GNEAttributesCreatorRow)

    /// @brief generate ID
    std::string generateID() const;

    /// @brief check if current ID placed in myValueTextField is valid
    bool isValidID() const;

private:
    /// @brief pointer to GNEAttributesCreator
    GNEAttributesCreator* myAttributesCreatorParent = nullptr;

    /// @brief attribute properties
    const GNEAttributeProperties myAttrProperties;

    /// @brief string which indicates the reason due current value is invalid
    std::string myInvalidValue;

    /// @brief Label with the name of the attribute
    MFXLabelTooltip* myAttributeLabel = nullptr;

    /// @brief check button to enable/disable Label attribute
    FXCheckButton* myEnableAttributeCheckButton = nullptr;

    /// @brief Button for open color or allow/disallow editor
    FXButton* myAttributeButton = nullptr;

    /// @brief textField to modify the default value of string parameters
    FXTextField* myValueTextField = nullptr;

    /// @brief check button to enable/disable the value of boolean parameters
    FXCheckButton* myValueCheckButton = nullptr;

    /// @brief comboBox for discrete vaues
    FXComboBox* myValueComboBox = nullptr;
};
