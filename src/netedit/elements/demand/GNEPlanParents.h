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
/// @file    GNEPlanParents.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2024
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

/// @brief GNEPlanParents (used for group all plan parents)
class GNEPlanParents {

public:
    /// @brief default constructor
    GNEPlanParents();

    /// @brief constructor for parsing the parameters from SUMOSAXAttributes
    GNEPlanParents(const CommonXMLStructure::PlanParameters& planParameters,
                   const GNENetHelper::AttributeCarriers* ACs);

    /// @brief check integrity between planParameters and GNE elements
    bool checkIntegrity(SumoXMLTag planTag, const GNEDemandElement* parent,
                        const CommonXMLStructure::PlanParameters& planParameters) const;

    /// @brief add the given element in the element as child
    void addChildElements(GNEDemandElement* element);

    /// @brief clear elements
    void clear();

    /// @name functions for checking stopping places
    /// @{
    /// @brief get from busStop
    bool getFromBusStop() const;

    /// @brief get to busStop
    bool getToBusStop() const;

    /// @brief get from trainStop
    bool getFromTrainStop() const;

    /// @brief get to trainStop
    bool getToTrainStop() const;

    /// @brief get from containerStop
    bool getFromContainerStop() const;

    /// @brief get to containerStop
    bool getToContainerStop() const;

    /// @brief get from chargingStation
    bool getFromChargingStation() const;

    /// @brief get to chargingStation
    bool getToChargingStation() const;

    /// @brief get from parkingArea
    bool getFromParkingArea() const;

    /// @brief get to parkingArea
    bool getToParkingArea() const;

    /// @}

    /// @brief get junctions (used in plan constructors)
    std::vector<GNEJunction*> getJunctions() const;

    /// @brief get edges (used in plan constructors)
    std::vector<GNEEdge*> getEdges() const;

    /// @brief get additionalElements (used in plan constructors)
    std::vector<GNEAdditional*> getAdditionalElements() const;

    /// @brief get demand elements (used in plan constructors)
    std::vector<GNEDemandElement*> getDemandElements(GNEDemandElement* parent) const;

    /// @brief from edge
    GNEEdge* fromEdge = nullptr;

    /// @brief to edge
    GNEEdge* toEdge = nullptr;

    /// @brief edges
    std::vector<GNEEdge*> consecutiveEdges;

    /// @brief from junction
    GNEJunction* fromJunction = nullptr;

    /// @brief to junction
    GNEJunction* toJunction = nullptr;

    /// @brief from TAZ
    GNEAdditional* fromTAZ = nullptr;

    /// @brief to TAZ
    GNEAdditional* toTAZ = nullptr;

    /// @brief from stoppingPlace
    GNEAdditional* fromStoppingPlace = nullptr;

    /// @brief to stoppingPlace
    GNEAdditional* toStoppingPlace = nullptr;

    /// @brief from route (currently only used by walks)
    GNEDemandElement* fromRoute = nullptr;

    /// @brief to route (currently only used by walks)
    GNEDemandElement* toRoute = nullptr;
};
