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
/// @file    GNEPlanSelector.h
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2023
///
// Frame for select person/container plans
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNETagSelector.h"

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
    GNEPlanSelector(GNEFrame* frameParent, SumoXMLTag planType);

    /// @brief destructor
    ~GNEPlanSelector();

    /// @brief show plan selector
    void showPlanSelector();

    /// @brief plan item selector
    void hidePlanSelector();

    /// @brief get current plan tag properties
    const GNETagProperties& getCurrentPlanTagProperties() const;

    /// @brief get current plan template
    GNEDemandElement* getCurrentPlanTemplate() const;

    /// @brief refresh plan selector (used when frameParent is show)
    void refreshPlanSelector();

    /// @brief check if mark routes with dotted contours
    bool markRoutes() const;

    /// @brief check if mark edges with dotted contours
    bool markEdges() const;

    /// @brief check if mark junctions with dotted contours
    bool markJunctions() const;

    /// @brief check if mark busStops with dotted contours
    bool markBusStops() const;

    /// @brief check if mark trainStops with dotted contours
    bool markTrainStops() const;

    /// @brief check if mark containerStops with dotted contours
    bool markContainerStops() const;

    /// @brief check if mark TAZs with dotted contours
    bool markTAZs() const;

    /// @name FOX-callbacks
    /// @{

    /// @brief Called when the user select an element in ComboBox
    long onCmdSelectPlan(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX need this
    FOX_CONSTRUCTOR(GNEPlanSelector)

    /// @brief check if selected plan is valid
    bool isPlanValid() const;

    /// @brief fill person templates
    void fillPersonPlanTemplates(GNENet* net);

    /// @brief fill container templates
    void fillContainerPlanTemplates(GNENet* net);

private:
    /// @brief pointer to Frame Parent
    GNEFrame* myFrameParent;

    /// @brief comboBox with the tags
    MFXComboBoxIcon* myPlansComboBox;

    /// @brief current plan template;
    std::pair<GNETagProperties, GNEDemandElement*> myCurrentPlanTemplate;

    /// @brief list with demand templates
    std::vector<std::pair<GNETagProperties, GNEDemandElement*> > myPlanTemplates;
};
