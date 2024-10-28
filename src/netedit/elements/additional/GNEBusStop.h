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
/// @file    GNEBusStop.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
// A class for visualizing busStop geometry (adapted from GUILaneWrapper)
/****************************************************************************/
#pragma once
#include <config.h>
#include "GNEStoppingPlace.h"


// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEBusStop
 * @brief A lane area vehicles can halt at (netedit-version)
 */
class GNEBusStop : public GNEStoppingPlace {

public:
    /// @brief default constructor
    static GNEBusStop* buildBusStop(GNENet* net);

    /// @brief default constructor
    static GNEBusStop* buildTrainStop(GNENet* net);

    /**@brief parameter constructor for bus stops
     * @param[in] id The storage of gl-ids to get the one for this lane representation from
     * @param[in] lane Lane of this StoppingPlace belongs
     * @param[in] net pointer to GNENet of this additional element belongs
     * @param[in] startPos Start position of the StoppingPlace
     * @param[in] endPos End position of the StoppingPlace
     * @param[in] name Name of busStop
     * @param[in] lines lines of the busStop
     * @param[in] personCapacity larger numbers of persons trying to enter will create an upstream jam on the sidewalk.
     * @param[in] parkingLength parking length
     * @param[in] color busStop color
     * @param[in] friendlyPos enable or disable friendly position
     * @param[in] parameters generic parameters
     */
    static GNEBusStop* buildBusStop(const std::string& id, GNELane* lane, GNENet* net,
                                    const double startPos, const double endPos, const std::string& name, const std::vector<std::string>& lines,
                                    int personCapacity, double parkingLength, const RGBColor& color, bool friendlyPosition,
                                    const Parameterised::Map& parameters);

    /**@brief parameter constructor for train stops
     * @param[in] id The storage of gl-ids to get the one for this lane representation from
     * @param[in] lane Lane of this StoppingPlace belongs
     * @param[in] net pointer to GNENet of this additional element belongs
     * @param[in] startPos Start position of the StoppingPlace
     * @param[in] endPos End position of the StoppingPlace
     * @param[in] name Name of busStop
     * @param[in] lines lines of the busStop
     * @param[in] personCapacity larger numbers of persons trying to enter will create an upstream jam on the sidewalk.
     * @param[in] parkingLength parking length
     * @param[in] color busStop color
     * @param[in] friendlyPos enable or disable friendly position
     * @param[in] parameters generic parameters
     */
    static GNEBusStop* buildTrainStop(const std::string& id, GNELane* lane, GNENet* net,
                                      const double startPos, const double endPos, const std::string& name, const std::vector<std::string>& lines,
                                      int personCapacity, double parkingLength, const RGBColor& color, bool friendlyPosition,
                                      const Parameterised::Map& parameters);

    /// @brief Destructor
    ~GNEBusStop();

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
    /// @brief The list of lines that are assigned to this stop
    std::vector<std::string> myLines;

    /// @brief maximum number of persons that can wait at this stop
    int myPersonCapacity;

    /// @brief custom space for vehicles that park at this stop
    double myParkingLength;

    /// @brief RGB color
    RGBColor myColor;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief default constructor
    GNEBusStop(SumoXMLTag tag, GUIGlObjectType type, GUIIcon icon, GNENet* net);

    /**@brief parameter Constructor
     * @param[in] tag busStop or trainStop tag
     * @param[in] type busStop or trainStop GLO type
     * @param[in] icon busStop or trainStop icon
     * @param[in] id The storage of gl-ids to get the one for this lane representation from
     * @param[in] lane Lane of this StoppingPlace belongs
     * @param[in] net pointer to GNENet of this additional element belongs
     * @param[in] startPos Start position of the StoppingPlace
     * @param[in] endPos End position of the StoppingPlace
     * @param[in] name Name of busStop
     * @param[in] lines lines of the busStop
     * @param[in] personCapacity larger numbers of persons trying to enter will create an upstream jam on the sidewalk.
     * @param[in] parkingLength parking length
     * @param[in] color busStop color
     * @param[in] friendlyPos enable or disable friendly position
     * @param[in] parameters generic parameters
     */
    GNEBusStop(SumoXMLTag tag, GUIGlObjectType type, GUIIcon icon, const std::string& id, GNELane* lane, GNENet* net,
               const double startPos, const double endPos, const std::string& name, const std::vector<std::string>& lines,
               int personCapacity, double parkingLength, const RGBColor& color, bool friendlyPosition,
               const Parameterised::Map& parameters);

    /// @brief Invalidated copy constructor.
    GNEBusStop(const GNEBusStop&) = delete;

    /// @brief Invalidated assignment operator.
    GNEBusStop& operator=(const GNEBusStop&) = delete;
};
