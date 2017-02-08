/****************************************************************************/
/// @file    GNEChargingStation.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
/// A class for visualizing chargingStation geometry (adapted from GUILaneWrapper)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNEChargingStation_h
#define GNEChargingStation_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GNEStoppingPlace.h"


// ===========================================================================
// class declarations
// ===========================================================================

class GUIGLObjectPopupMenu;
class PositionVector;
class GNELane;
class GNENet;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEChargingStation : public GNEStoppingPlace {
public:
    /**@brief Constructor of charging station
     * @param[in] id The storage of gl-ids to get the one for this lane representation from
     * @param[in] lane Lane of this StoppingPlace belongs
     * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
     * @param[in] startPos Start position of the StoppingPlace
     * @param[in] endPos End position of the StoppingPlace
     * @param[in] chargingPower charging power of the charging station
     * @param[in] efficiency efficiency of the charge [0,1]
     * @param[in] chargeInTransit enable or disable charge in transit
     * @param[in] chargeDelay delay in timeSteps in the charge
     */
    GNEChargingStation(const std::string& id, GNELane* lane, GNEViewNet* viewNet, SUMOReal startPos, SUMOReal endPos, SUMOReal chargingPower, SUMOReal efficiency, bool chargeInTransit, const SUMOReal chargeDelay);

    /// @brief Destructor
    ~GNEChargingStation();

    /// @brief update pre-computed geometry information
    void updateGeometry();

    /**@brief writte additional element into a xml file
     * @param[in] device device in which write parameters of additional element
     */
    void writeAdditional(OutputDevice& device) const;

    /**@brief Returns the charging power of the chargingStation
     * @return The charging power of the chargingStation
     */
    SUMOReal getChargingPower();

    /**@brief Returns the charging efficiency of the chargingStation
     * @return The charging efficiency of the chargingStation
     */
    SUMOReal getEfficiency();
    /**@brief Returns the value of charge in transit of the chargingStation
     * @return True if charge in transit is enabled, false in other case
     */
    bool getChargeInTransit();
    /**@brief Returns the charge delay of the chargingStation
     * @return The charge delay of the chargingStation
     */
    SUMOReal getChargeDelay();

    /**@brief Set a new charging power in the charging station
     * @param[in] chargingPower new charging power
     * @throws InvalidArgument if value of chargingPower isn't valid
     */
    void setChargingPower(SUMOReal chargingPower);

    /**@brief Set a new efficiency in the charging station
     * @param[in] efficiency new efficiency
     * @throws InvalidArgument if value of efficiency isn't valid
     */
    void setEfficiency(SUMOReal efficiency);
    /**@brief Enable or disable charge in transit in the charging station
     * @param[in] chargeInTransit new charge in Transit value (0 or 1)
     */
    void setChargeInTransit(bool chargeInTransit);
    /**@brief Set a new charge delay in the charging station
     * @param[in] chargeDelay new charge delay value (true or false)
     * @throws InvalidArgument if value of chargeDelay isn't valid
     */
    void setChargeDelay(SUMOReal chargeDelay);

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
     * @param[in] value The value asociated to key key
     * @return true if the value is valid, false in other case
     */
    bool isValid(SumoXMLAttr key, const std::string& value);
    /// @}

protected:
    /// @brief Charging power pro timestep
    SUMOReal myChargingPower;

    /// @brief efficiency of the charge
    SUMOReal myEfficiency;

    /// @brief enable or disable charge in transit
    bool myChargeInTransit;

    /// @brief delay in the starting of charge
    SUMOReal myChargeDelay;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNEChargingStation(const GNEChargingStation&);

    /// @brief Invalidated assignment operator.
    GNEChargingStation& operator=(const GNEChargingStation&);
};


#endif
