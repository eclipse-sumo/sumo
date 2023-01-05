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
/// @file    GNEAttributesCreator.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2022
///
// Attribute creator
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/MFXGroupBoxModule.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEFrame;
class GNEAttributesCreatorRow;
class GNEFlowEditor;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEAttributesCreator : public MFXGroupBoxModule {
    /// @brief FOX-declaration
    FXDECLARE(GNEAttributesCreator)

public:
    /// @brief constructor
    GNEAttributesCreator(GNEFrame* frameParent);

    /// @brief destructor
    ~GNEAttributesCreator();

    /// @brief show GNEAttributesCreator modul
    void showAttributesCreatorModule(GNEAttributeCarrier* templateAC, const std::vector<SumoXMLAttr>& hiddenAttributes);

    /// @brief hide group box
    void hideAttributesCreatorModule();

    /// @brief return frame parent
    GNEFrame* getFrameParent() const;

    /// @brief get attributes and their values
    void getAttributesAndValues(CommonXMLStructure::SumoBaseObject* baseObject, bool includeAll) const;

    /// @brief get current template AC
    GNEAttributeCarrier* getCurrentTemplateAC() const;

    /// @brief check if parameters of attributes are valid
    bool areValuesValid() const;

    /// @brief show warning message with information about non-valid attributes
    void showWarningMessage(std::string extra = "") const;

    /// @brief refresh attribute creator
    void refreshAttributesCreator();

    /// @brief disable GNEAttributesCreator
    void disableAttributesCreator();

    /// @name FOX-callbacks
    /// @{
    /// @brief Called when reset button is pressed
    long onCmdReset(FXObject*, FXSelector, void*);

    /// @brief Called when help button is pressed
    long onCmdHelp(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX need this
    FOX_CONSTRUCTOR(GNEAttributesCreator);

    /// @brief refresh rows
    void refreshRows(const bool createRows);

private:
    /// @brief pointer to Frame Parent
    GNEFrame* myFrameParent = nullptr;

    /// @brief pointer to myFlowEditor
    GNEFlowEditor* myFlowEditor = nullptr;

    /// @brief current templateAC
    GNEAttributeCarrier* myTemplateAC;

    /// @brief hidden attributes
    std::vector<SumoXMLAttr> myHiddenAttributes;

    /// @brief vector with the GNEAttributesCreatorRow
    std::vector<GNEAttributesCreatorRow*> myAttributesCreatorRows;

    /// @brief frame buttons
    FXHorizontalFrame* myFrameButtons = nullptr;

    /// @brief reset button
    FXButton* myResetButton = nullptr;
};
