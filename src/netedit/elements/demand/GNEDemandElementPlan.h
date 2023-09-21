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
/// @file    GNEDemandElementPlan.h
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2023
///
// An auxiliar, asbtract class for plan elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/geom/Position.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/vehicle/SUMOVehicleParameter.h>

// ===========================================================================
// class declaration
// ===========================================================================

class SUMOVehicleParameter;
class GNEDemandElement;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEDemandElementPlan {

protected:
    /// @brief constructor
    GNEDemandElementPlan(GNEDemandElement* planElement, double arrivalPosition);

    /// @brief write plan element common attributes
    void writePlanAttributes(OutputDevice& device) const;

    /// @brief Returns an own popup-menu
    GUIGLObjectPopupMenu* getPlanPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent);

    /// @name path functions
    /// @{

    /// @brief get first plan path lane
    GNELane* getFirstPlanPathLane() const;

    /// @brief get last plan path lane
    GNELane* getLastPlanPathLane() const;

    /// @brief compute plan pathElement
    void computePlanPathElement();

    /// @}

    /// @name geometry functions
    /// @{

    /// @brief update pre-computed geometry information
    void updatePlanGeometry();

    /// @brief get centering boundaryt
    Boundary getPlanCenteringBoundary() const;

    /// @brief Returns position of additional in view
    Position getPlanPositionInView() const;

    /// @}

    /// @name attribute functions
    /// @{

    /// @brief get plan attribute string
    std::string getPlanAttribute(SumoXMLAttr key) const;

    /// @brief get plan attribute double
    double getPlanAttributeDouble(SumoXMLAttr key) const;

    /// @brief get plan attribute position
    Position getPlanAttributePosition(SumoXMLAttr key) const;

    /// @brief set plan attribute
    void setPlanAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    /// @brief check if plan attribute is valid
    bool isPlanValid(SumoXMLAttr key, const std::string& value);

    /// @brief check if plan attribute is enabled
    bool isPlanAttributeEnabled(SumoXMLAttr key) const;

    /// @brief set plan attribute (intern)
    void setPlanAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief get plan Hierarchy Name (Used in AC Hierarchy)
    std::string getPlanHierarchyName() const;
    
    /// @}

    /// @name drawing functions
    /// @{

    /// @brief check if person plan can be drawn
    bool checkDrawPersonPlan() const;

    /// @brief check if container plan can be drawn
    bool checkDrawContainerPlan() const;

    /// @brief draw plan
    void drawPlanGL(const bool drawPlan, const GUIVisualizationSettings& s, const RGBColor& planColor, const RGBColor& planSelectedColor) const;

    /// @brief draw plan partial lane
    void drawPlanPartial(const bool drawPlan, const GUIVisualizationSettings& s, const GNELane* lane, const GNEPathManager::Segment* segment, const double offsetFront,
                         const double planWidth, const RGBColor& planColor, const RGBColor& planSelectedColor) const;

    /// @brief draw plan partial junction
    void drawPlanPartial(const bool drawPlan, const GUIVisualizationSettings& s, const GNELane* fromLane, const GNELane* toLane, const GNEPathManager::Segment* segment,
                         const double offsetFront, const double planWidth, const RGBColor& planColor, const RGBColor& planSelectedColor) const;

    /// @}

    /// @brief check if plan is valid
    GNEDemandElement::Problem isPersonPlanValid() const;

    /// @brief get plan problem
    std::string getPersonPlanProblem() const;

    /// @brief arrival position
    double myArrivalPosition;

    /// @brief arrival position diameter
    static const double myArrivalPositionDiameter;

private:
    /// @brief pointer to plan element
    GNEDemandElement* myPlanElement;

    /// @brief update geometry of plan that starts or ends in TAZ
    void updateTAZGeometry();

    /// @brief Invalidated copy constructor.
    GNEDemandElementPlan(const GNEDemandElementPlan&) = delete;

    /// @brief Invalidated assignment operator.
    GNEDemandElementPlan& operator=(const GNEDemandElementPlan&) = delete;
};
