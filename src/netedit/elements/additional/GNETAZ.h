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
/// @file    GNETAZ.h
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2018
///
//
/****************************************************************************/
#pragma once
#include <config.h>
#include <netedit/GNEMoveElement.h>
#include <utils/shapes/SUMOPolygon.h>

#include "GNETAZElement.h"

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNETAZ
 * Class for Traffic Assign Zones (TAZs)
 */
class GNETAZ : public GNETAZElement, private SUMOPolygon, public GNEMoveElement {

public:
    /// @brief needed to avoid diamond Problem between GUIPolygon and GNETAZElement
    using GNETAZElement::getID;

    /// @default GNETAZ Constructor
    GNETAZ(GNENet* net);

    /**@brief GNETAZ Constructor
     * @param[in] id The storage of gl-ids to get the one for this lane representation from
     * @param[in] net pointer to GNENet of this additional element belongs
     * @param[in] shape TAZ shape
     * @param[in] center TAZ center
     * @param[in] fill flag for fill TAZ shape
     * @param[in] color TAZ color
     * @param[in] name TAZ's name
     * @param[in] parameters generic parameters
     */
    GNETAZ(const std::string& id, GNENet* net, const PositionVector& shape, const Position& TAZ, const bool fill,
           const RGBColor& color, const std::string& name, const std::map<std::string, std::string>& parameters);

    /// @brief GNETAZ Destructor
    ~GNETAZ();

    /**@brief get move operation
    * @note returned GNEMoveOperation can be nullptr
    */
    GNEMoveOperation* getMoveOperation();

    /**@brief return index of a vertex of shape, or of a new vertex if position is over an shape's edge
     * @param pos position of new/existent vertex
     * @param snapToGrid enable or disable snapToActiveGrid
     * @return index of position vector
     */
    int getVertexIndex(Position pos, bool snapToGrid);

    /// @brief remove geometry point in the clicked position
    void removeGeometryPoint(const Position clickedPosition, GNEUndoList* undoList);

    /// @brief get TAZ Shape
    const PositionVector& getTAZElementShape() const;

    /**@brief writte TAZElement element into a xml file
     * @param[in] device device in which write parameters of additional element
     */
    void writeTAZElement(OutputDevice& device) const;

    /// @name Functions related with geometry of element
    /// @{
    /// @brief update pre-computed geometry information
    void updateGeometry();

    /// @brief Returns position of additional in view
    Position getPositionInView() const;

    /// @brief return exaggeration asociated with this GLObject
    double getExaggeration(const GUIVisualizationSettings& s) const;

    /// @brief Returns the boundary to which the view shall be centered in order to show the object
    Boundary getCenteringBoundary() const;

    /// @}

    /// @name inherited from GUIGlObject
    /// @{
    /// @brief Returns the name of the parent object
    /// @return This object's parent id
    std::string getParentName() const;

    /**@brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent);

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

    /* @brief method for getting the Attribute of an XML key in position format (to avoid unnecessary parse<position>(...) for certain attributes)
     * @param[in] key The attribute key
     * @return double with the value associated to key
     */
    Position getAttributePosition(SumoXMLAttr key) const;

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    /* @brief method for checking if the key and their conrrespond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value asociated to key key
     * @return true if the value is valid, false in other case
     */
    bool isValid(SumoXMLAttr key, const std::string& value);

    /* @brief method for check if the value for certain attribute is set
     * @param[in] key The attribute key
     */
    bool isAttributeEnabled(SumoXMLAttr key) const;

    /// @brief get PopPup ID (Used in AC Hierarchy)
    std::string getPopUpID() const;

    /// @brief get Hierarchy Name (Used in AC Hierarchy)
    std::string getHierarchyName() const;
    /// @}

    /// @brief update TAZ Stadistic
    void updateTAZStadistic();

protected:
    /// @brief boundary used during moving of elements
    Boundary myMovingGeometryBoundary;

    /// @brief geometry for lenghts/rotations
    GUIGeometry myTAZGeometry;

    /// @brief TAZ center
    Position myTAZCenter;

private:
    /// @brief hint size of vertex
    static const double myHintSize;

    /// @brief squaredhint size of vertex
    static const double myHintSizeSquared;

    /// @brief Max source weight
    double myMaxWeightSource;

    /// @brief Min source weight
    double myMinWeightSource;

    /// @brief Average source weight
    double myAverageWeightSource;

    /// @brief Max Sink weight
    double myMaxWeightSink;

    /// @brief Min Sink weight
    double myMinWeightSink;

    /// @brief Average Sink weight
    double myAverageWeightSink;

    /// @brief draw dotted contours
    void drawDottedContours(const GUIVisualizationSettings& s, const double TAZExaggeration) const;

    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief set move shape
    void setMoveShape(const GNEMoveResult& moveResult);

    /// @brief commit move shape
    void commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList);

    /// @brief Invalidated copy constructor.
    GNETAZ(const GNETAZ&) = delete;

    /// @brief Invalidated assignment operator.
    GNETAZ& operator=(const GNETAZ&) = delete;
};
