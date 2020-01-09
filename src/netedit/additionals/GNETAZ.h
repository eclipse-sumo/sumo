/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNETAZ.h
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2018
///
//
/****************************************************************************/
#ifndef GNETAZ_h
#define GNETAZ_h


// ===========================================================================
// included modules
// ===========================================================================

#include "GNEAdditional.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNETAZ
 * Class for multy Entry/multy Exits detectors
 */
class GNETAZ : public GNEAdditional {

public:
    /**@brief GNETAZ Constructor
     * @param[in] id The storage of gl-ids to get the one for this lane representation from
     * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
     * @param[in] block movement enable or disable additional movement
     */
    GNETAZ(const std::string& id, GNEViewNet* viewNet, PositionVector shape, RGBColor color, bool blockMovement);

    /// @brief GNETAZ Destructor
    ~GNETAZ();

    /// @brief get TAZ shape
    const PositionVector& getTAZShape() const;

    /// @name Functions related with geometry of element
    /// @{
    /**@brief change the position of the element geometry without saving in undoList
     * @param[in] offset Position used for calculate new position of geometry without updating RTree
     */
    void moveGeometry(const Position& offset);

    /**@brief commit geometry changes in the attributes of an element after use of moveGeometry(...)
     * @param[in] undoList The undoList on which to register changes
     */
    void commitGeometryMoving(GNEUndoList* undoList);

    /// @brief update pre-computed geometry information
    void updateGeometry();

    /// @brief update dotted contour
    void updateDottedContour();

    /// @brief Returns position of additional in view
    Position getPositionInView() const;

    /// @brief Returns the boundary to which the view shall be centered in order to show the object
    Boundary getCenteringBoundary() const;

    /// @brief split geometry
    void splitEdgeGeometry(const double splitPosition, const GNENetElement* originalElement, const GNENetElement* newElement, GNEUndoList* undoList);
    /// @}

    /// @name Functions related with shape of element
    /// @{
    /**@brief change position of a vertex of shape without commiting change
    * @param[in] index index of Vertex shape
    * @param[in] newPos The new position of vertex
    * @return index of vertex (in some cases index can change
    */
    int moveVertexShape(const int index, const Position& oldPos, const Position& offset);

    /**@brief move entire shape without commiting change
    * @param[in] oldShape the old shape of polygon before moving
    * @param[in] offset the offset of movement
    */
    void moveEntireShape(const PositionVector& oldShape, const Position& offset);

    /**@brief commit geometry changes in the attributes of an element after use of changeShapeGeometry(...)
    * @param[in] oldShape the old shape of polygon
    * @param[in] undoList The undoList on which to register changes
    */
    void commitShapeChange(const PositionVector& oldShape, GNEUndoList* undoList);

    /**@brief return index of a vertex of shape, or of a new vertex if position is over an shape's edge
     * @param pos position of new/existent vertex
     * @param createIfNoExist enable or disable creation of new verte if there isn't another vertex in position
     * @param snapToGrid enable or disable snapToActiveGrid
     * @return index of position vector
     */
    int getVertexIndex(Position pos, bool createIfNoExist, bool snapToGrid);

    /// @brief delete the geometry point closest to the given pos
    void deleteGeometryPoint(const Position& pos, bool allowUndo = true);

    /// @brief return true if Shape TAZ is blocked
    bool isShapeBlocked() const;
    /// @}

    /// @name inherited from GUIGlObject
    /// @{
    /// @brief Returns the name of the parent object
    /// @return This object's parent id
    std::string getParentName() const;

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

    /// @brief update TAZ after add or remove a Source/sink, or change their weight
    void updateParentAdditional();

protected:
    /// @brief TAZ Color
    RGBColor myColor;

    /// @brief TAZ shape
    PositionVector myTAZShape;

    /// @brief flag for block shape
    bool myBlockShape;

    /// @brief index of vertex that is been moved (-1 means that none vertex is been moved)
    int myCurrentMovingVertexIndex;

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

    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNETAZ(const GNETAZ&) = delete;

    /// @brief Invalidated assignment operator.
    GNETAZ& operator=(const GNETAZ&) = delete;
};

#endif
/****************************************************************************/
