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
/// @file    GNEPlanSelector.h
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2023
///
// Frame for select person/container plans
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/MFXGroupBoxModule.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEFrame;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEPlanSelector : public MFXGroupBoxModule {
    /// @brief FOX-declaration
    FXDECLARE(GNEPlanSelector)

public:
    /// @brief constructor
    GNEPlanSelector(GNEFrame* frameParent, GNETagProperties::TagType type);

    /// @brief destructor
    ~GNEPlanSelector();

    /// @brief show item selector
    void showTagSelector();

    /// @brief hide item selector
    void hideTagSelector();

    /// @brief get current template plan
    GNEDemandElement* getCurrentTemplatePlan() const;

    /// @brief refresh tagSelector (used when frameParent is show)
    void refreshTagSelector();

    /// @name FOX-callbacks
    /// @{

    /// @brief Called when the user select an element in ComboBox
    long onCmdSelectPlan(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX need this
    FOX_CONSTRUCTOR(GNEPlanSelector)

private:

    /// @brief pointer to Frame Parent
    GNEFrame* myFrameParent;

    /// @brief current tagType
    GNETagProperties::TagType myTagType;

    /// @brief comboBox with the tags
    MFXComboBoxIcon* myPlansComboBox;

    /// @brief current templateAC;
    GNEDemandElement* myCurrentPlanTemplate;

    /// @brief list with plan templates
    std::map<std::string, GNEDemandElement*> myPlanTemplates;
};
