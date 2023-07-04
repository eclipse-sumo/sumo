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
/// @file    GNEDemandElementFlow.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2023
///
// An auxiliar, asbtract class for flow elements (vehicles, person and containers)
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/geom/Position.h>
#include <utils/xml/SUMOXMLDefinitions.h>

// ===========================================================================
// class declaration
// ===========================================================================

class SUMOVehicleParameter;
class GNEDemandElement;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEDemandElementFlow {

protected:
    /// @brief constructor
    GNEDemandElementFlow();

    /// @brief destructor
    ~GNEDemandElementFlow();

    /// @brief draw flow label
    void drawFlowLabel(const Position& position, const double rotation, const double width, const double length, const double exaggeration) const;

    /// @brief set flow parameters (used in toggleAttribute(...) function of vehicles, persons and containers
    void setFlowParameters(SUMOVehicleParameter* vehicleParameters, const SumoXMLAttr attribute, const bool value);

    /// @brief adjust flow default attributes (called in vehicle/person/flow constructors)
    void adjustDefaultFlowAttributes(GNEDemandElement* flowElement, SUMOVehicleParameter* vehicleParameters);

private:
    /// @brief Invalidated copy constructor.
    GNEDemandElementFlow(const GNEDemandElementFlow&) = delete;

    /// @brief Invalidated assignment operator.
    GNEDemandElementFlow& operator=(const GNEDemandElementFlow&) = delete;
};
