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
/// @file    GNEStoppingPlace.h
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
///
// A abstract class to define common parameters of lane area in which vehicles can halt (GNE version)
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEAdditional.h"

// ===========================================================================
// class declaration
// ===========================================================================

class GNEMoveElementLaneDouble;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEStoppingPlace : public GNEAdditional, public Parameterised {

public:
    /**@brief Default constructor
     * @param[in] net pointer to GNENet of this additional element belongs
     * @param[in] tag Type of xml tag that define the StoppingPlace (SUMO_TAG_BUS_STOP, SUMO_TAG_CHARGING_STATION, etc...)
     */
    GNEStoppingPlace(GNENet* net, SumoXMLTag tag);

    /**@brief Constructor
     * @param[in] id The name of the stopping place
     * @param[in] net net in which this polygon is placed
     * @param[in] filename file in which this element is stored
     * @param[in] tag Type of xml tag that define the StoppingPlace (SUMO_TAG_BUS_STOP, SUMO_TAG_CHARGING_STATION, etc...)
     * @param[in] lane Lane of this StoppingPlace belongs
     * @param[in] startPos Start position of the StoppingPlace
     * @param[in] endPos End position of the StoppingPlace
     * @param[in] name Name of stoppingPlace
     * @param[in] friendlyPos enable or disable friendly position
     * @param[in] color stoppingPlace color
     * @param[in] angle stoppingPlace angle
     * @param[in] parameters generic parameters
     */
    GNEStoppingPlace(const std::string& id, GNENet* net, const std::string& filename, SumoXMLTag tag, GNELane* lane,
                     const double startPos, const double endPos, const std::string& name, bool friendlyPosition,
                     const RGBColor& color, const double angle, const Parameterised::Map& parameters);

    /// @brief Destructor
    ~GNEStoppingPlace();

    /// @brief methods to retrieve the elements linked to this stoppingPlace
    /// @{

    /// @brief get GNEMoveElement associated with this stoppingPlace
    GNEMoveElement* getMoveElement() const override;

    /// @brief get parameters associated with this stoppingPlace
    Parameterised* getParameters() override;

    /// @brief get parameters associated with this stoppingPlace (constant)
    const Parameterised* getParameters() const override;

    /// @}

    /// @name members and functions relative to write additionals into XML
    /// @{

    /// @brief check if current additional is valid to be written into XML (must be reimplemented in all detector children)
    bool isAdditionalValid() const;

    /// @brief return a string with the current additional problem (must be reimplemented in all detector children)
    std::string getAdditionalProblem() const;

    /// @brief fix additional problem (must be reimplemented in all detector children)
    void fixAdditionalProblem();

    /// @}

    /// @name Function related with contour drawing
    /// @{

    /// @brief check if draw move contour (red)
    bool checkDrawMoveContour() const override;

    /// @}

    /// @name Functions related with geometry of element
    /// @{

    /// @brief Returns position of additional in view
    Position getPositionInView() const;

    /// @brief update centering boundary (implies change in RTREE)
    void updateCenteringBoundary(const bool updateGrid);

    /// @brief split geometry
    void splitEdgeGeometry(const double splitPosition, const GNENetworkElement* originalElement, const GNENetworkElement* newElement, GNEUndoList* undoList);

    /// @}

    /// @name inherited from GNEAdditional
    /// @{
    /// @brief Returns the name of the parent object
    /// @return This object's parent id
    std::string getParentName() const;

    /// @}

    /// @name inherited from GNEAttributeCarrier
    /// @{

    /* @brief method for getting the Attribute of an XML key in position format
     * @param[in] key The attribute key
     * @return position with the value associated to key
     */
    Position getAttributePosition(SumoXMLAttr key) const override;

    /* @brief method for getting the Attribute of an XML key in positionVector format
     * @param[in] key The attribute key
     * @return positionVector with the value associated to key
     */
    PositionVector getAttributePositionVector(SumoXMLAttr key) const override;

    /* @brief method for check if the value for certain attribute is set
     * @param[in] key The attribute key
     */
    bool isAttributeEnabled(SumoXMLAttr key) const;

    /// @brief get PopPup ID (Used in AC Hierarchy)
    std::string getPopUpID() const override;

    /// @brief get Hierarchy Name (Used in AC Hierarchy)
    std::string getHierarchyName() const override;

    /// @}

protected:
    /// @brief The start position over lane
    double myStartPosOverLane = 0;

    /// @brief The end position over lane
    double myEndPosPosOverLane = 0;

    /// @brief Flag for friendly position
    bool myFriendlyPosition = false;

    /// @brief RGB color
    RGBColor myColor = RGBColor::INVISIBLE;

    /// @brief angle
    double myAngle = 0;

    /// @brief move element lane double
    GNEMoveElementLaneDouble* myMoveElementLaneDouble = nullptr;

    /// @brief The position of the sign
    Position mySymbolPosition;

    /// @brief circle contour
    GNEContour mySymbolContour;

    /// @name Functions related with stoppingPlace attributes
    /// @{

    /// @brief write common stoppingPlace attributes
    void writeStoppingPlaceAttributes(OutputDevice& device) const;

    /* @brief method for getting the stoppingPlace attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    std::string getStoppingPlaceAttribute(SumoXMLAttr key) const;

    /* @brief method for getting the Attribute of an XML key in double format
     * @param[in] key The attribute key
     * @return double with the value associated to key
     */
    double getStoppingPlaceAttributeDouble(SumoXMLAttr key) const;

    /* @brief method for getting the Attribute of an XML key in position format
     * @param[in] key The attribute key
     * @return position with the value associated to key
     */
    Position getStoppingPlaceAttributePosition(SumoXMLAttr key) const;

    /* @brief method for setting the stoppingPlace attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    void setStoppingPlaceAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    /* @brief method for check if new value for certain stoppingPlace attribute is valid
     * @param[in] key The attribute key
     * @param[in] value The new value
     */
    bool isStoppingPlaceValid(SumoXMLAttr key, const std::string& value) const;

    /// @brief method for setting the stoppingPlace attribute and nothing else (used in GNEChange_Attribute)
    void setStoppingPlaceAttribute(SumoXMLAttr key, const std::string& value);

    /// @}

    /// @brief set geometry common to all stopping places
    void setStoppingPlaceGeometry(double movingToSide);

    /// @brief draw lines
    void drawLines(const GUIVisualizationSettings::Detail d, const std::vector<std::string>& lines,
                   const RGBColor& color) const;

    /// @brief draw sign
    void drawSign(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d, const double exaggeration,
                  const RGBColor& baseColor, const RGBColor& signColor, const std::string& word) const;

    /// @brief check object in view
    void calculateStoppingPlaceContour(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                       const double width, const double exaggeration, const bool movingGeometryPoints) const;

private:
    /// @brief Invalidate set new position in the view
    void setPosition(const Position& pos) = delete;

    /// @brief Invalidated copy constructor.
    GNEStoppingPlace(const GNEStoppingPlace&) = delete;

    /// @brief Invalidated assignment operator
    GNEStoppingPlace& operator=(const GNEStoppingPlace& src) = delete;
};
