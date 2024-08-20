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
/// @file    GNERouteHandler.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2019
///
// Builds demand objects for netedit
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/frames/GNEFrameAttributeModules.h>
#include <netedit/frames/GNEPathCreator.h>
#include <netedit/frames/GNEPlanCreator.h>
#include <netedit/frames/GNEAttributesCreator.h>
#include <netedit/GNENetHelper.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/vehicle/SUMORouteHandler.h>
#include <utils/xml/SUMOSAXAttributes.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/handlers/RouteHandler.h>


// ===========================================================================
// class declarations
// ===========================================================================

class GNEViewNet;
class GNEEdge;
class GNETAZ;
class GNEDemandElement;
class GNEVehicle;
class GNEPerson;
class GNEContainer;
class GNEUndoList;

// ===========================================================================
// class definitions
// ===========================================================================

/// @brief GNE plan parameters (used for group all from-to parameters related with plans)
class GNEPlanParameters {

public:
    /// @brief default constructor
    GNEPlanParameters();

    /// @brief constructor for parsing the parameters from SUMOSAXAttributes
    GNEPlanParameters(const CommonXMLStructure::SumoBaseObject* sumoBaseObject,
                      const CommonXMLStructure::PlanParameters& planParameters,
                      const GNENetHelper::AttributeCarriers* ACs);

    /// @brief add the given element in the element as child
    void addChildElements(GNEDemandElement* element);

    /// @brief clear elements
    void clear();

    /// @brief get junctions (used in plan constructors)
    std::vector<GNEJunction*> getJunctions() const;

    /// @brief get edges (used in plan constructors)
    std::vector<GNEEdge*> getEdges() const;

    /// @brief get additionalElements (used in plan constructors)
    std::vector<GNEAdditional*> getAdditionalElements() const;

    /// @brief get demand elements (used in plan constructors)
    std::vector<GNEDemandElement*> getDemandElements(GNEDemandElement* parent) const;

    /// @brief from junction
    GNEJunction* fromJunction = nullptr;

    /// @brief to junction
    GNEJunction* toJunction = nullptr;

    /// @brief from edge
    GNEEdge* fromEdge = nullptr;

    /// @brief to edge
    GNEEdge* toEdge = nullptr;

    /// @brief from TAZ
    GNEAdditional* fromTAZ = nullptr;

    /// @brief to TAZ
    GNEAdditional* toTAZ = nullptr;

    /// @brief from stoppingPlace
    GNEAdditional* fromStoppingPlace = nullptr;

    /// @brief to stoppingPlace
    GNEAdditional* toStoppingPlace = nullptr;

    /// @brief edges
    std::vector<GNEEdge*> consecutiveEdges;

    /// @brief route (currently only used by walks)
    GNEDemandElement* route = nullptr;

    /// @name values used only by stops
    /// @{

    /// @brief edge
    GNEEdge* edge = nullptr;

    /// @brief stoppingPlace
    GNEAdditional* stoppingPlace = nullptr;

    /// @}

private:
    /// @brief get previous plan obj
    const CommonXMLStructure::SumoBaseObject* getPreviousPlanObj(const CommonXMLStructure::SumoBaseObject* sumoBaseObject) const;

    /// @brief get previous plan element if was not defined previoulsy (used for loaded elements)
    void updateFromAttributes(const CommonXMLStructure::SumoBaseObject* sumoBaseObject,
                              const GNENetHelper::AttributeCarriers* ACs);

    /// @brief invalidate copy constructor
    GNEPlanParameters(const GNEPlanParameters& s) = delete;

    /// @brief invalidate assignment operator
    GNEPlanParameters& operator=(const GNEPlanParameters& s) = delete;
};
