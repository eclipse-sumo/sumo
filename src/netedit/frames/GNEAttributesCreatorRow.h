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
/// @file    GNEFrameAttributeModules.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2019
///
// Auxiliar class for GNEFrame Modules (only for attributes edition)
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/elements/GNEAttributeCarrier.h>
#include <netedit/GNEViewNetHelper.h>
#include <utils/common/Parameterised.h>
#include <utils/foxtools/FXGroupBoxModule.h>
#include <utils/xml/CommonXMLStructure.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEFrame;
class AttributesCreator;

// ===========================================================================
// class definitions
// ===========================================================================


class AttributesCreatorRow : public FXHorizontalFrame {
    /// @brief FOX-declaration
    FXDECLARE(AttributesCreatorRow)

public:
    /// @brief constructor
    AttributesCreatorRow(AttributesCreator* AttributesCreatorParent, const GNEAttributeProperties& attrProperties);

    /// @brief destroy AttributesCreatorRow (but don't delete)
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

    /// @brief get AttributesCreator parent
    AttributesCreator* getAttributesCreatorParent() const;

    /// @name FOX-callbacks
    /// @{
    /// @brief called when user set the value of an attribute of type int/float/string/bool
    long onCmdSetAttribute(FXObject*, FXSelector, void*);

    /// @brief called when user press the open dialog button
    long onCmdOpenAttributeDialog(FXObject*, FXSelector, void*);
    /// @}

protected:
    FOX_CONSTRUCTOR(AttributesCreatorRow)

    /// @brief generate ID
    std::string generateID() const;

    /// @brief check if current ID placed in myValueTextField is valid
    bool isValidID() const;

private:
    /// @brief pointer to AttributesCreator
    AttributesCreator* myAttributesCreatorParent = nullptr;

    /// @brief attribute properties
    const GNEAttributeProperties myAttrProperties;

    /// @brief string which indicates the reason due current value is invalid
    std::string myInvalidValue;

    /// @brief Label with the name of the attribute
    FXLabel* myAttributeLabel = nullptr;

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