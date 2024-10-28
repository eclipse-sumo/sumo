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
// class definitions
// ===========================================================================
/**
 * @class GNEStoppingPlace
 * @briefA abstract class to define common parameters and functions of stopping places
 */
class GNEStoppingPlace : public GNEAdditional, public Parameterised {

public:
    /**@brief Constructor.
     * @param[in] id Gl-id of the stopping place (Must be unique)
     * @param[in] net pointer to GNENet of this additional element belongs
     * @param[in] type GUIGlObjectType of stoppingPlace
     * @param[in] tag Type of xml tag that define the StoppingPlace (SUMO_TAG_BUS_STOP, SUMO_TAG_CHARGING_STATION, etc...)
     * @param[in] lane Lane of this StoppingPlace belongs
     * @param[in] startPos Start position of the StoppingPlace
     * @param[in] endPos End position of the StoppingPlace
     * @param[in] name Name of stoppingPlace
     * @param[in] friendlyPos enable or disable friendly position
     * @param[in] parameters generic parameters
     */
    GNEStoppingPlace(const std::string& id, GNENet* net, GUIGlObjectType type, SumoXMLTag tag, FXIcon* icon, GNELane* lane,
                     const double startPos, const double endPos, const std::string& name, bool friendlyPosition,
                     const Parameterised::Map& parameters);

    /// @brief Destructor
    ~GNEStoppingPlace();

    /**@brief get move operation
    * @note returned GNEMoveOperation can be nullptr
    */
    GNEMoveOperation* getMoveOperation();

    /// @name members and functions relative to write additionals into XML
    /// @{
    /**@brief write additional element into a xml file
    * @param[in] device device in which write parameters of additional element
    */
    virtual void writeAdditional(OutputDevice& device) const = 0;

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
    bool checkDrawMoveContour() const;

    /// @}

    /// @name Functions related with geometry of element
    /// @{
    /// @brief update pre-computed geometry information
    virtual void updateGeometry() = 0;

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

    /**@brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    virtual void drawGL(const GUIVisualizationSettings& s) const = 0;
    /// @}

    /// @name inherited from GNEAttributeCarrier
    /// @{
    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    virtual std::string getAttribute(SumoXMLAttr key) const = 0;

    /* @brief method for getting the Attribute of an XML key in double format (to avoid unnecessary parse<double>(...) for certain attributes)
     * @param[in] key The attribute key
     * @return double with the value associated to key
     */
    virtual double getAttributeDouble(SumoXMLAttr key) const;

    /// @brief get parameters map
    const Parameterised::Map& getACParametersMap() const;

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    virtual void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) = 0;

    /* @brief method for checking if the key and their conrrespond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value associated to key key
     * @return true if the value is valid, false in other case
     */
    virtual bool isValid(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief get PopPup ID (Used in AC Hierarchy)
    std::string getPopUpID() const;

    /// @brief get Hierarchy Name (Used in AC Hierarchy)
    std::string getHierarchyName() const;
    /// @}

protected:
    /// @brief The relative start position this stopping place is located at (-1 means empty)
    double myStartPosition;

    /// @brief The  position this stopping place is located at (-1 means empty)
    double myEndPosition;

    /// @brief Flag for friendly position
    bool myFriendlyPosition;

    /// @brief The position of the sign
    Position mySymbolPosition;

    /// @brief circle contour
    GNEContour mySymbolContour;

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
    /// @brief set attribute after validation
    virtual void setAttribute(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief get start position over lane that is applicable to the shape
    double getStartGeometryPositionOverLane() const;

    /// @brief get end position over lane that is applicable to the shape
    double getEndGeometryPositionOverLane() const;

    /// @brief set move shape
    void setMoveShape(const GNEMoveResult& moveResult);

    /// @brief commit move shape
    void commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList);

    /// @brief Invalidate set new position in the view
    void setPosition(const Position& pos) = delete;
};
