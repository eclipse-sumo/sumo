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
/// @file    GNEContainerPlanFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2021
///
// The Widget for add ContainerPlan elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/frames/GNEFrame.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEContainerPlanFrame
 */
class GNEContainerPlanFrame : public GNEFrame {

public:
    /**@brief Constructor
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNEContainerPlanFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEContainerPlanFrame();

    /// @brief show Frame
    void show();

    /// @brief hide Frame
    void hide();

    /**@brief add container plan element
     * @param objectsUnderCursor collection of objects under cursor after click over view
     * @param mouseButtonKeyPressed key pressed during click
     * @return true if element was sucesfully added
     */
    bool addContainerPlanElement(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor, const GNEViewNetHelper::MouseButtonKeyPressed& mouseButtonKeyPressed);

    /// @brief get path creator modul
    GNEFrameModuls::PathCreator* getPathCreator() const;

protected:
    /// @brief Tag selected in TagSelector
    void tagSelected();

    /// @brief selected demand element in DemandElementSelector
    void demandElementSelected();

    /// @brief create path
    void createPath();

private:
    /// @brief route handler
    GNERouteHandler myRouteHandler;

    /// @brief Container selectors
    GNEFrameModuls::DemandElementSelector* myContainerSelector;

    /// @brief containerPlan selector
    GNEFrameModuls::TagSelector* myContainerPlanTagSelector;

    /// @brief internal vehicle attributes
    GNEFrameAttributesModuls::AttributesCreator* myContainerPlanAttributes;

    /// @brief Path Creator
    GNEFrameModuls::PathCreator* myPathCreator;

    /// @brief Container Hierarchy
    GNEFrameModuls::HierarchicalElementTree* myContainerHierarchy;
};
