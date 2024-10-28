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
/// @file    GNEChargingStation.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
// A class for visualizing chargingStation geometry (adapted from GUILaneWrapper)
/****************************************************************************/
#pragma once
#include <config.h>
#include "GNEStoppingPlace.h"


// ===========================================================================
// class definitions
// ===========================================================================

class GNEChargingStation : public GNEStoppingPlace {

public:
    /// @brief default Constructor of charging station
    GNEChargingStation(GNENet* net);

    /**@brief Constructor of charging station
     * @param[in] id The storage of gl-ids to get the one for this lane representation from
     * @param[in] lane Lane of this StoppingPlace belongs
     * @param[in] net pointer to GNENet of this additional element belongs
     * @param[in] startPos Start position of the StoppingPlace
     * @param[in] endPos End position of the StoppingPlace
     * @param[in] name Name of busStop
     * @param[in] chargingPower charging power of the charging station
     * @param[in] efficiency efficiency of the charge [0,1]
     * @param[in] chargeInTransit enable or disable charge in transit
     * @param[in] chargeDelay delay in timeSteps in the charge
     * @param[in] chargeType charge type (fuel or electric)
     * @param[in] waitingTime waiting time until start charging
     * @param[in] friendlyPos enable or disable friendly position
     * @param[in] parameters generic parameters
     */
    GNEChargingStation(const std::string& id, GNELane* lane, GNENet* net, const double startPos, const double endPos,
                       const std::string& name, double chargingPower, double efficiency, bool chargeInTransit, SUMOTime chargeDelay,
                       const std::string& chargeType, const SUMOTime waitingTime, bool friendlyPosition, const Parameterised::Map& parameters);

    /// @brief Destructor
    ~GNEChargingStation();

    /**@brief write additional element into a xml file
     * @param[in] device device in which write parameters of additional element
     */
    void writeAdditional(OutputDevice& device) const;

    /// @name Functions related with geometry of element
    /// @{

    /// @brief update pre-computed geometry information
    void updateGeometry();

    /// @}

    /// @name inherited from GUIGlObject
    /// @{

    /**@brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const;

    /// @}

    /// @name inherited from GNEAttributeCarrier
    /// @{

    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    std::string getAttribute(SumoXMLAttr key) const;

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    /* @brief method for checking if the key and their correspond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value associated to key key
     * @return true if the value is valid, false in other case
     */
    bool isValid(SumoXMLAttr key, const std::string& value);

    /// @}

protected:
    /// @brief Charging power pro timestep
    double myChargingPower = 0;

    /// @brief efficiency of the charge
    double myEfficiency = 0;

    /// @brief enable or disable charge in transit
    bool myChargeInTransit = false;

    /// @brief delay in the starting of charge
    SUMOTime myChargeDelay = 0;

    /// @brief charging type
    std::string myChargeType = "normal";

    /// @brief waiting time before start charging
    SUMOTime myWaitingTime = 0;

    /// @brief parking area ID
    std::string myParkingAreaID;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNEChargingStation(const GNEChargingStation&) = delete;

    /// @brief Invalidated assignment operator.
    GNEChargingStation& operator=(const GNEChargingStation&) = delete;
};
