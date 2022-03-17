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

// ===========================================================================
// class definitions
// ===========================================================================

class FlowEditor : public FXGroupBoxModule {
    /// @brief FOX-declaration
    FXDECLARE(FlowEditor)

public:
    /// @brief constructor
    FlowEditor(GNEViewNet* viewNet, FXVerticalFrame* contentFrame);

    /// @brief destructor
    ~FlowEditor();

    /// @brief show FlowEditor modul
    void showFlowEditor(const std::vector<GNEAttributeCarrier*> editedFlows);

    /// @brief hide group box
    void hideFlowEditor();

    /// @brief shown FlowEditor modul
    bool shownFlowEditor() const;

    /// @brief refresh FlowEditor
    void refreshFlowEditor();

    /// @brief get flow attributes
    void getFlowAttributes(CommonXMLStructure::SumoBaseObject* baseObject);

    /// @brief check if parameters of attributes are valid
    bool areFlowValuesValid() const;

    /// @name FOX-callbacks
    /// @{
    /// @brief called when user change flow attribute
    long onCmdSetFlowAttribute(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX need this
    FOX_CONSTRUCTOR(FlowEditor);

    /// @brief refresh single flow
    void refreshSingleFlow();

    /// @brief refresh multiple flows
    void refreshMultipleFlows();

    /// @brief get flow attribute (of the current edited flows)
    const std::string getFlowAttribute(SumoXMLAttr attr);

private:
    /// @brief pointer to viewNet
    GNEViewNet *myViewNet;

    /// @brief ComboBox for terminate options (end, number, end/number)
    FXComboBox* myTerminateComboBox = nullptr;

    /// @brief horizontal frame for terminate definition
    FXHorizontalFrame* myTerminateFrameTextField = nullptr;

    /// @brief Label for terminate definition
    FXLabel* myTerminateLabel = nullptr;

    /// @brief textField for terminate attribute
    FXTextField* myTerminateTextField = nullptr;

    /// @brief horizontal frame for terminate options
    FXHorizontalFrame* mySpacingFrameComboBox = nullptr;

    /// @brief ComboBox for spacing comboBox (perHour, period, probability)
    FXComboBox* mySpacingComboBox = nullptr;

    /// @brief horizontal frame for spacing textField
    FXHorizontalFrame* mySpacingFrameTextField = nullptr;

    /// @brief Label for spacing
    FXLabel* mySpacingLabel = nullptr;

    /// @brief textField for spacing attribute
    FXTextField* mySpacingTextField = nullptr;

    /// @brief edited flows
    std::vector<GNEAttributeCarrier*> myEditedFlows;

    /// @brief per hours attr (vehicles/person/container)
    SumoXMLAttr myPerHourAttr = SUMO_ATTR_NOTHING;
};