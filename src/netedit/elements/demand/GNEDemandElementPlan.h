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
    GNEDemandElementPlan(const GNEDemandElement* planElement);

    /// @brief check if person plan can be drawn
    bool drawPersonPlan() const;

    /// @brief check if container plan can be drawn
    bool drawContainerPlan() const;

    /// @brief draw person plan partial lane
    void drawPersonPlanPartial(const bool drawPlan, const GUIVisualizationSettings& s, const GNELane* lane, const GNEPathManager::Segment* segment, const double offsetFront,
                               const double personPlanWidth, const RGBColor& personPlanColor) const;

    /// @brief draw person plan partial junction
    void drawPersonPlanPartial(const bool drawPlan, const GUIVisualizationSettings& s, const GNELane* fromLane, const GNELane* toLane, const GNEPathManager::Segment* segment,
                               const double offsetFront, const double personPlanWidth, const RGBColor& personPlanColor) const;

    /// @brief check if person plan is valid
    GNEDemandElement::Problem isPersonPlanValid() const;

    /// @brief get person plan problem
    std::string getPersonPlanProblem() const;

protected:
    /// @brief person plans arrival position radius
    static const double myPersonPlanArrivalPositionDiameter;

private:
    /// @brief pointer to plan element
    const GNEDemandElement* myPlanElement;

    /// @brief Invalidated copy constructor.
    GNEDemandElementPlan(const GNEDemandElementPlan&) = delete;

    /// @brief Invalidated assignment operator.
    GNEDemandElementPlan& operator=(const GNEDemandElementPlan&) = delete;
};
