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
/// @file    GNEParkingArea.h
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2018
///
// A class for visualizing ParkingArea geometry (adapted from GUILaneWrapper)
/****************************************************************************/
#pragma once
#include <config.h>
#include "GNEStoppingPlace.h"


// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEParkingArea
 * @brief A lane area vehicles can park at (netedit-version)
 */
class GNEParkingArea : public GNEStoppingPlace {

public:
    /**@brief Constructor
     * @param[in] id The storage of gl-ids to get the one for this lane representation from
     * @param[in] lane Lane of this StoppingPlace belongs
     * @param[in] net pointer to GNENet of this additional element belongs
     * @param[in] startPos Start position of the StoppingPlace
     * @param[in] endPos End position of the StoppingPlace
     * @param[in] departPos lane position in that vehicle must depart when leaves parkingArea
     * @param[in] name Name of ParkingArea
     * @param[in] friendlyPos enable or disable friendly position
     * @param[in] roadSideCapacity road side capacity of ParkingArea
     * @param[in] width ParkingArea's length
     * @param[in] length ParkingArea's length
     * @param[in] angle ParkingArea's angle
     * @param[in] parameters generic parameters
     */
    GNEParkingArea(const std::string& id, GNELane* lane, GNENet* net, const double startPos, const double endPos, const std::string& departPos,
                   const std::string& name, bool friendlyPosition, int roadSideCapacity, bool onRoad, double width, const double length,
                   double angle, const std::map<std::string, std::string>& parameters);

    /// @brief Destructor
    ~GNEParkingArea();

    /**@brief writte additional element into a xml file
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

    /* @brief method for getting the Attribute of an XML key in double format (to avoid unnecessary parse<double>(...) for certain attributes)
     * @param[in] key The attribute key
     * @return double with the value associated to key
     */
    double getAttributeDouble(SumoXMLAttr key) const;

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
    /** @struct GNELotSpaceDefinition
     * @brief Representation of a single lot space in Netedit
     */
    struct GNELotSpaceDefinition {
        /// @brief default constructor
        GNELotSpaceDefinition();

        /// @brief parameter constructor
        GNELotSpaceDefinition(double x, double y, double z, double rotation, double width, double length);

        /// @brief The position of the vehicle when parking in this space
        const Position position;

        /// @brief The rotation
        const double rotation;

        /// @brief The width
        const double width;

        /// @brief The length
        const double length;

        /// @brief Invalidated assignment operator.
        GNELotSpaceDefinition& operator=(const GNELotSpaceDefinition&) = delete;
    };

    /// @brief departPos
    std::string myDepartPos;

    /// @brief roadside capacity of Parking Area
    int myRoadSideCapacity;

    /// @brief Whether vehicles stay on the road
    bool myOnRoad;

    /// @brief width of Parking Area
    double myWidth;

    /// @brief Length of Parking Area (by default (endPos - startPos) / roadsideCapacity
    double myLength;

    /// @brief Angle of Parking Area
    double myAngle;

    /// @brief vector with GNELotSpaceDefinition
    std::vector<GNELotSpaceDefinition> myLotSpaceDefinitions;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNEParkingArea(const GNEParkingArea&) = delete;

    /// @brief Invalidated assignment operator.
    GNEParkingArea& operator=(const GNEParkingArea&) = delete;
};


