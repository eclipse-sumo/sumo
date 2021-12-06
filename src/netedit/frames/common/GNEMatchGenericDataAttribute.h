/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    GNEMatchGenericDataAttribute.h
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2020
///
// The Widget for modifying selections of network-elements
// (some elements adapted from GUIDialog_GLChosenEditor)
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNESelectorFrame.h"

// ===========================================================================
// class definitions
// ===========================================================================

class GNEMatchGenericDataAttribute : public FXGroupBoxModul {
    /// @brief FOX-declaration
    FXDECLARE(GNEMatchGenericDataAttribute)

public:
    /// @brief constructor
    GNEMatchGenericDataAttribute(GNEElementSet* elementSet, SumoXMLTag defaultTag, SumoXMLAttr defaultAttr, const std::string& defaultValue);

    /// @brief destructor
    ~GNEMatchGenericDataAttribute();

    /// @brief enable match attributes
    void enableMatchGenericDataAttribute();

    /// @brief disable match attributes
    void disableMatchGenericDataAttribute();

    /// @brief show match attributes
    void showMatchGenericDataAttribute();

    /// @brief hide match attributes
    void hideMatchGenericDataAttribute();

    /// @name FOX-callbacks
    /// @{

    /// @brief Called when the user selects an interval
    long onCmdSetInterval(FXObject*, FXSelector, void*);

    /// @brief Called when the user change begin text field
    long onCmdSetBegin(FXObject*, FXSelector, void*);

    /// @brief Called when the user change end text field
    long onCmdSetEnd(FXObject*, FXSelector, void*);

    /// @brief Called when the user selects a tag in the match box
    long onCmdSelectTag(FXObject*, FXSelector, void*);

    /// @brief Called when the user selects an attribute in the match box
    long onCmdSelectAttribute(FXObject*, FXSelector, void*);

    /// @brief Called when the user enters a new selection expression
    long onCmdProcessString(FXObject*, FXSelector, void*);

    /// @brief Called when the user clicks the help button
    long onCmdHelp(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX need this
    FOX_CONSTRUCTOR(GNEMatchGenericDataAttribute)

private:
    /// @brief pointer to element set Parent
    GNEElementSet* myElementSet;

    /// @brief tag of the match box
    MFXIconComboBox* myIntervalSelector;

    /// @brief TextField for begin
    FXTextField* myBegin;

    /// @brief TextField for end
    FXTextField* myEnd;

    /// @brief tag of the match box
    MFXIconComboBox* myMatchGenericDataTagComboBox;

    /// @brief attributes of the match box
    FXComboBox* myMatchGenericDataAttrComboBox;

    /// @brief current SumoXMLTag tag
    SumoXMLTag myCurrentTag;

    /// @brief current string Attribute
    std::string myCurrentAttribute;

    /// @brief string of the match
    FXTextField* myMatchGenericDataString;

    // declare a set and fill it with all intervals
    std::map<std::pair<double, double>, int> myIntervals;

    /// @brief Invalidated copy constructor.
    GNEMatchGenericDataAttribute(const GNEMatchGenericDataAttribute&) = delete;

    /// @brief Invalidated assignment operator.
    GNEMatchGenericDataAttribute& operator=(const GNEMatchGenericDataAttribute&) = delete;
};
