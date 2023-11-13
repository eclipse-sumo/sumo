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
/// @file    GNEContainerPlanFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2019
///
// The Widget for add ContainerPlan elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/elements/demand/GNERouteHandler.h>
#include <netedit/frames/GNEAttributesCreator.h>
#include <netedit/frames/GNEDemandSelector.h>
#include <netedit/frames/GNEElementTree.h>
#include <netedit/frames/GNEFrame.h>
#include <netedit/frames/GNEPlanCreatorLegend.h>
#include <netedit/frames/GNETagSelector.h>
#include <netedit/frames/GNEPlanSelector.h>
#include <netedit/frames/GNEPlanCreator.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEContainerPlanFrame
 */
class GNEContainerPlanFrame : public GNEFrame {

public:

    /**@brief Constructor
     * @brief viewParent GNEViewParent in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNEContainerPlanFrame(GNEViewParent* viewParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEContainerPlanFrame();

    /// @brief show Frame
    void show();

    /// @brief hide Frame
    void hide();

    /**@brief add container plan element
     * @param objectsUnderCursor collection of objects under cursor after click over view
     * @return true if element was successfully added
     */
    bool addContainerPlanElement(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor);

    /// @brief reset selected container
    void resetSelectedContainer();

    /// @brief get plan creator module
    GNEPlanCreator* getPlanCreator() const;

    /// @brief get Container Hierarchy
    GNEElementTree* getContainerHierarchy() const;

    /// @brief get container selectors
    GNEDemandElementSelector* getContainerSelector() const;

    /// @brief get containerPlan selector
    GNEPlanSelector* getPlanSelector() const;

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

    /// @brief Container selectors
    GNEDemandElementSelector* myContainerSelector;

    /// @brief containerPlan selector
    GNEPlanSelector* myPlanSelector;

    /// @brief internal vehicle attributes
    GNEAttributesCreator* myContainerPlanAttributes;

    /// @brief plan Creator
    GNEPlanCreator* myPlanCreator;

    /// @brief Container Hierarchy
    GNEElementTree* myContainerHierarchy;

    /// @brief plan creator legend
    GNEPlanCreatorLegend* myPlanCreatorLegend;
};
