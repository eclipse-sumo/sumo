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
/// @file    GNEContainerFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    May 2019
///
// The Widget for add container elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/elements/demand/GNERouteHandler.h>
#include <netedit/frames/GNEAttributesCreator.h>
#include <netedit/frames/GNEDemandSelector.h>
#include <netedit/frames/GNEFrame.h>
#include <netedit/frames/GNENeteditAttributes.h>
#include <netedit/frames/GNEPlanCreator.h>
#include <netedit/frames/GNEPlanCreatorLegend.h>
#include <netedit/frames/GNETagSelector.h>
#include <netedit/frames/GNEPlanSelector.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEContainerFrame
 */
class GNEContainerFrame : public GNEFrame {

public:
    /**@brief Constructor
     * @brief viewParent GNEViewParent in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNEContainerFrame(GNEViewParent* viewParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEContainerFrame();

    /// @brief show Frame
    void show();

    /// @brief hide Frame
    void hide();

    /**@brief add vehicle element
     * @param viewObjects collection of objects under cursor after click over view
     * @return true if vehicle was successfully added
     */
    bool addContainer(const GNEViewNetHelper::ViewObjectsSelector& viewObjects);

    /// @brief get plan creator module
    GNEPlanCreator* getPlanCreator() const;

    /// @brief get Type selectors
    GNEDemandElementSelector* getTypeSelector() const;

    /// @brief get containerPlan selector
    GNEPlanSelector* getPlanSelector() const;

    /// @brief get attributes creator
    GNEAttributesCreator* getContainerAttributes() const;

protected:
    /// @brief Tag selected in GNETagSelector
    void tagSelected();

    /// @brief selected demand element in DemandElementSelector
    void demandElementSelected();

    /// @brief create path
    bool createPath(const bool useLastRoute);

private:
    /// @brief route handler
    GNERouteHandler myRouteHandler;

    /// @brief container base object
    CommonXMLStructure::SumoBaseObject* myContainerBaseObject;

    /// @brief container tag selector (used to select diffent kind of containers)
    GNETagSelector* myContainerTagSelector;

    /// @brief Container Type selectors
    GNEDemandElementSelector* myTypeSelector;

    /// @brief containerPlan selector
    GNEPlanSelector* myPlanSelector;

    /// @brief internal vehicle attributes
    GNEAttributesCreator* myContainerAttributes;

    /// @brief internal container plan attributes
    GNEAttributesCreator* myContainerPlanAttributes;

    /// @brief Netedit parameter
    GNENeteditAttributes* myNeteditAttributes;

    /// @brief plan creator
    GNEPlanCreator* myPlanCreator;

    /// @brief plan creator legend
    GNEPlanCreatorLegend* myPlanCreatorLegend;

    /// @brief build container and return it (note: function includes a call to begin(...), but NOT a call to end(...))
    GNEDemandElement* buildContainer();
};
