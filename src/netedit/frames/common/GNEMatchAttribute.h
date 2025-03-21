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
/// @file    GNEMatchAttribute.h
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2020
///
// The Widget for modifying selections of network-elements
// (some elements adapted from GUIDialog_GLChosenEditor)
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/MFXGroupBoxModule.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEAttributeProperties;
class GNESelectorFrame;
class GNETagProperties;
class MFXComboBoxTagProperty;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEMatchAttribute : public MFXGroupBoxModule {
    /// @brief FOX-declaration
    FXDECLARE(GNEMatchAttribute)

public:
    /// @brief constructor
    GNEMatchAttribute(GNESelectorFrame* selectorFrameParent, SumoXMLTag defaultTag, SumoXMLAttr defaultAttr, const std::string& defaultValue);

    /// @brief destructor
    ~GNEMatchAttribute();

    /// @brief enable match attributes
    void enableMatchAttribute();

    /// @brief disable match attributes
    void disableMatchAttribute();

    /// @brief show match attributes
    void showMatchAttribute();

    /// @brief hide match attributes
    void hideMatchAttribute();

    /// @brief refresh match attribute
    void refreshMatchAttribute();

    /// @name FOX-callbacks
    /// @{

    /**@brief Called when the user selectes a tag in the match box
     * @note updates the attr listbox and repaints itself
     */
    long onCmdSelMBTag(FXObject* obj, FXSelector, void*);

    /**@brief Called when the user selectes a tag in the match box
     * @note updates the attr listbox and repaints itself
     */
    long onCmdSelMBAttribute(FXObject*, FXSelector, void*);

    /**@brief Called when the user enters a new selection expression
     * @note validates expression and modifies current selection
     */
    long onCmdSelMBString(FXObject*, FXSelector, void*);

    /**@brief Called when the user clicks the help button
     * @note pop up help window
     */
    long onCmdHelp(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX need this
    FOX_CONSTRUCTOR(GNEMatchAttribute)

    /// @brief update tag
    void updateTag();

    /// @brief update attribute
    void updateAttribute();

private:
    /// @brief pointer to selector frame parent
    GNESelectorFrame* mySelectorFrameParent = nullptr;

    /// @brief vector with ComboBoxes
    std::vector <MFXComboBoxTagProperty*> myTagComboBoxVector;

    /// @brief attributes of the match box
    MFXComboBoxIcon* myMatchAttrComboBox = nullptr;

    /// @brief current tag properties
    const GNETagProperties* myTagProperties = nullptr;

    /// @brief current attribute properties
    const GNEAttributeProperties* myAttributeProperties = nullptr;

    /// @brief string of the match
    FXTextField* myMatchString = nullptr;

    /// @brief match string button
    FXButton* myMatchStringButton = nullptr;

    /// @brief Invalidated copy constructor.
    GNEMatchAttribute(const GNEMatchAttribute&) = delete;

    /// @brief Invalidated assignment operator.
    GNEMatchAttribute& operator=(const GNEMatchAttribute&) = delete;
};
