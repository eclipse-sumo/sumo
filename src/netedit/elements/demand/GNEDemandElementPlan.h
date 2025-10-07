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
#include <netedit/elements/moving/GNEMoveElementPlan.h>

#include "GNEPlanParents.h"

// ===========================================================================
// class declaration
// ===========================================================================

class SUMOVehicleParameter;
class GNEDemandElement;
class GNEEdge;
class GNEJunction;
class GNEAdditional;
class GNERoute;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEDemandElementPlan {

protected:
    /// @brief move element plan
    GNEMoveElementPlan* myMoveElementPlan = nullptr;

    /// @brief variable used for draw central contour
    GNEContour myPlanContour;

    /// @brief variable used for draw contour end
    GNEContour myPlanContourEnd;

    /// @brief plan boundary
    Boundary myPlanBoundary;

    /// @brief constructor
    GNEDemandElementPlan(GNEDemandElement* planElement, const double departPosition, const double arrivalPosition);

    /// @brief write plan element common attributes
    void writeLocationAttributes(OutputDevice& device) const;

    /// @brief write initial stop stage if plan starts at a stopping place
    void writeOriginStop(OutputDevice& device) const;

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

    /// @brief get plan centering boundary
    Boundary getPlanCenteringBoundary() const;

    /// @brief update plan centering boundary
    void updatePlanCenteringBoundary(const bool updateGrid);

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
    void drawPlanLanePartial(const bool drawPlan, const GUIVisualizationSettings& s, const GNESegment* segment, const double offsetFront,
                             const double planWidth, const RGBColor& planColor, const RGBColor& planSelectedColor) const;

    /// @brief draw plan partial junction
    void drawPlanJunctionPartial(const bool drawPlan, const GUIVisualizationSettings& s, const GNESegment* segment, const double offsetFront,
                                 const double planWidth, const RGBColor& planColor, const RGBColor& planSelectedColor) const;

    /// @}

    /// @brief check if plan is valid
    GNEDemandElement::Problem isPlanPersonValid() const;

    /// @brief get plan problem
    std::string getPersonPlanProblem() const;

    /// @brief depart position (used in tranships)
    double myDepartPosition;

    /// @brief arrival position (used in all plans over edges)
    double myArrivalPosition;

private:
    /// @brief get end position radius
    double getEndPosRadius(const GUIVisualizationSettings& s, const GNESegment* segment, const bool drawHalfWidth) const;

    /// @brief draw from arrow
    void drawFromArrow(const GUIVisualizationSettings& s, const GNELane* lane, const GNESegment* segment) const;

    /// @brief draw to arrow
    void drawToArrow(const GUIVisualizationSettings& s, const GNELane* lane, const GNESegment* segment) const;

    /// @brief draw to arrow
    void drawEndPosition(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d, const double endPosRadius) const;

    /// @brief replace plan parent
    bool replacePlanParent(const std::string& newParentID);

    /// @brief pointer to plan element
    GNEDemandElement* myPlanElement;

    /// @brief arrival position diameter
    static const double myArrivalPositionDiameter;

    /// @brief Invalidated copy constructor.
    GNEDemandElementPlan(const GNEDemandElementPlan&) = delete;

    /// @brief Invalidated assignment operator.
    GNEDemandElementPlan& operator=(const GNEDemandElementPlan&) = delete;
};
