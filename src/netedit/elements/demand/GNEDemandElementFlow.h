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
#include <utils/vehicle/SUMOVehicleParameter.h>

// ===========================================================================
// class declaration
// ===========================================================================

class SUMOVehicleParameter;
class GNEDemandElement;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEDemandElementFlow : public SUMOVehicleParameter {

protected:
    /// @brief constructor
    GNEDemandElementFlow(GNEDemandElement* flowElement);

    /// @brief constructor with parameters
    GNEDemandElementFlow(GNEDemandElement* flowElement, const SUMOVehicleParameter& vehicleParameters);

    /// @brief destructor
    ~GNEDemandElementFlow();

    /// @brief draw flow label
    void drawFlowLabel(const Position& position, const double rotation, const double width,
                       const double length, const double exaggeration) const;

    /// @brief inherited from GNEAttributeCarrier and adapted to GNEDemandElementFlow
    /// @{
    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    std::string getFlowAttribute(const GNEDemandElement* flowElement, SumoXMLAttr key) const;

    /* @brief method for getting the Attribute of an XML key in double format (to avoid unnecessary parse<double>(...) for certain attributes)
     * @param[in] key The attribute key
     * @return double with the value associated to key
     */
    double getFlowAttributeDouble(SumoXMLAttr key) const;

    /* @brief method for setting the attribute and letting the object perform demand element changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     * @param[in] net optionally the GNENet to inform about gui updates
     */
    void setFlowAttribute(GNEDemandElement* flowElement, SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    /* @brief method for setting the attribute and letting the object perform demand element changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    bool isValidFlowAttribute(GNEDemandElement* flowElement, SumoXMLAttr key, const std::string& value);

    /* @brief method for enable attribute
     * @param[in] key The attribute key
     * @param[in] undoList The undoList on which to register changes
     * @note certain attributes can be only enabled, and can produce the disabling of other attributes
     */
    void enableFlowAttribute(GNEDemandElement* flowElement, SumoXMLAttr key, GNEUndoList* undoList);

    /* @brief method for disable attribute
     * @param[in] key The attribute key
     * @param[in] undoList The undoList on which to register changes
     * @note certain attributes can be only enabled, and can produce the disabling of other attributes
     */
    void disableFlowAttribute(GNEDemandElement* flowElement, SumoXMLAttr key, GNEUndoList* undoList);

    /* @brief method for check if the value for certain attribute is set
     * @param[in] key The attribute key
     */
    bool isFlowAttributeEnabled(SumoXMLAttr key) const;

    /// @brief method for setting the attribute and nothing else
    void setFlowAttribute(GNEDemandElement* flowElement, SumoXMLAttr key, const std::string& value);

    /// @brief toggle flow parameters (used in toggleAttribute(...) function of vehicles, persons and containers
    void toggleFlowAttribute(const SumoXMLAttr attribute, const bool value);

private:
    /// @brief set flow default attributes
    void setDefaultFlowAttributes(GNEDemandElement* flowElement);

    /// @brief Invalidated copy constructor.
    GNEDemandElementFlow(const GNEDemandElementFlow&) = delete;

    /// @brief Invalidated assignment operator.
    GNEDemandElementFlow& operator=(const GNEDemandElementFlow&) = delete;
};
