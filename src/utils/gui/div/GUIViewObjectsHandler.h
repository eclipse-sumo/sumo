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
/// @file    GUIViewObjectsHandler.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 22
///
// class used for handle objects over view
/****************************************************************************/
#pragma once
#include <config.h>

#include <vector>
#include <unordered_map>
#include <utils/geom/Triangle.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/settings/GUIVisualizationSettings.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEJunction;
class GNEEdge;
class GNELane;
class GNERoute;
class GNEPathElement;
class GNESegment;

// ===========================================================================
// class definitions
// ===========================================================================

class GUIViewObjectsHandler {

public:
    /// @brief object container
    struct ObjectContainer {

        /// @brief parameter constructor
        ObjectContainer(const GUIGlObject* object_) :
            object(object_) {}

        /// @brief object
        const GUIGlObject* object = nullptr;

        /// @brief vector with geometry points
        std::vector<int> geometryPoints;

        /// @brief position over shape
        Position posOverShape = Position::INVALID;

        /// @brief offset of position over shape
        double offset = 0;
    };

    /// @brief object container layer
    struct ObjectContainerLayer : public std::vector<ObjectContainer> {

        /// @brief parameter constructor
        ObjectContainerLayer() {}

        // @brief append object container and resize if neccesary
        void append(const ObjectContainer& objectContainer);
    };

    /// @brief typedef for pack elements sorted by layer
    typedef std::map<double, ObjectContainerLayer > GLObjectsSortedContainer;

    /// @brief constructor
    GUIViewObjectsHandler();

    /// @brief reset view objects handler
    void reset();

    /// @name position and boundary functions. used for defining the posion that will be check (usually the mouse position)
    /// @{
    /// @brief get selection position
    const Position& getSelectionPosition() const;

    /// @brief get selection triangle
    const Triangle& getSelectionTriangle() const;

    /// @brief set selection position
    void setSelectionPosition(const Position& pos);

    /// @brief set selection triangle
    void setSelectionTriangle(const Triangle& triangle);

    /// @brief return true if we're selecting using a triangle
    bool selectingUsingRectangle() const;

    /// @}

    /// @name check functions. If the result is positive, the given GLObject will be added to elementUnderCursor
    /// @{
    /// @brief check boundary parent element
    bool checkBoundaryParentObject(const GUIGlObject* GLObject, const double layer, const GUIGlObject* parent);

    /// @brief check if mouse is within elements geometry (for circles)
    bool checkCircleObject(const GUIVisualizationSettings::Detail d, const GUIGlObject* GLObject,
                           const Position& center, const double radius, const double layer);

    /// @brief check if mouse is within geometry point
    bool checkGeometryPoint(const GUIVisualizationSettings::Detail d, const GUIGlObject* GLObject,
                            const PositionVector& shape, const int index, const double layer, const double radius);

    /// @brief check if mouse is within geometry point
    bool checkPositionOverShape(const GUIVisualizationSettings::Detail d, const GUIGlObject* GLObject,
                                const PositionVector& shape, const double layer, const double distance);

    /// @brief check (closed) shape element
    bool checkShapeObject(const GUIGlObject* GLObject, const PositionVector& shape, const Boundary& shapeBoundary,
                          const double layer, const GNESegment* segment);
    /// @}

    /// @name functions used for mark (select) elements
    /// @{
    ///
    /// @brief add element into list of elements under cursor
    bool selectObject(const GUIGlObject* GLObject, const double layer, const bool checkDuplicated,
                      const GNESegment* segment);

    /// @brief add geometryPoint into list of elements under cursor
    bool selectGeometryPoint(const GUIGlObject* GLObject, const int newIndex, const double layer);

    /// @brief select position over shape (for example, the position over a lane shape)
    bool selectPositionOverShape(const GUIGlObject* GLObject, const Position& pos, const double layer, const double offset);

    /// @brief check if element was already selected
    bool isObjectSelected(const GUIGlObject* GLObject) const;

    /// @brief check rectangle selection
    bool checkRectangleSelection(const GUIVisualizationSettings& s, const GUIGlObject* GLObject,
                                 const double layer, const GUIGlObject* parent);

    /// @brief get all elements under cursor sorted by layer
    const GLObjectsSortedContainer& getSelectedObjects() const;

    /// @brief get segment associated with the given GLObject (if exist)
    const GNESegment* getSelectedSegment(const GUIGlObject* GLObject) const;

    /// @brief get geometry points for the given glObject
    const std::vector<int>& getSelectedGeometryPoints(const GUIGlObject* GLObject) const;

    /// @brief get position over shape
    const Position& getSelectedPositionOverShape(const GUIGlObject* GLObject) const;

    /// @brief get number of selected objects
    int getNumberOfSelectedObjects() const;

    /// @brief reverse selected objects
    void reverseSelectedObjects();

    /// @}

    /// @name functions related with redrawing path elements
    /// @{
    /// @brief get redrawing objects
    const std::set<const GNEPathElement*>& getRedrawPathElements() const;

    /// @brief check if the given path element has to be redraw again
    bool isPathElementMarkForRedraw(const GNEPathElement* pathElement) const;

    /// @brief add path element to redrawing set
    void addToRedrawPathElements(const GNEPathElement* pathElement);

    /// @}

    /// @name functions related with merging junctions
    /// @{
    /// @brief get merging junctions
    const std::vector<const GNEJunction*>& getMergingJunctions() const;

    /// @brief add to merging junctions (used for marking junctions to merge)
    bool addMergingJunctions(const GNEJunction* junction);

    /// @}

    /// @brief move the given object to the front (currently used only in netedit)
    void updateFrontObject(const GUIGlObject* GLObject);

    /// @brief isolate edge geometry points (used for moving)
    void isolateEdgeGeometryPoints();

    /// @brief recompute boundaries
    GUIGlObjectType recomputeBoundaries = GLO_NETWORK;

    /// @brief marked edge (used in create edge mode, for splitting)
    const GNEEdge* markedEdge = nullptr;

    /// @brief marked lane (used in create edge mode, for splitting)
    const GNELane* markedLane = nullptr;

    /// @brief marked TAZ (used in create TAZRel mode)
    const GUIGlObject* markedTAZ = nullptr;

    /// @brief marked route (used in create vehicle mode)
    const GNERoute* markedRoute = nullptr;

    /// @brief marked first geometry point (used for moving/delete geometry points)
    const GUIGlObject* markedFirstGeometryPoint = nullptr;

    /// @brief marked first geometry point (used for moving/delete geometry points)
    const GUIGlObject* markedSecondGeometryPoint = nullptr;

protected:
    /// @brief selected element sorted by layer
    GLObjectsSortedContainer mySortedSelectedObjects;

    /// @brief map with selected elements and if was selected with full boundary (used only to avoid double selections)
    std::unordered_map<const GUIGlObject*, const GNESegment*> mySelectedObjects;

    /// @brief number of selected objects
    int myNumberOfSelectedObjects = 0;

    /// @brief set with path elements marked for redrawing
    std::set<const GNEPathElement*> myRedrawPathElements;

    /// @brief selection triangle
    Triangle mySelectionTriangle;

    /// @brief position
    Position mySelectionPosition;

    /// @brief empty geometry points
    std::vector<int> myEmptyGeometryPoints;

    /// @brief merging junctions
    std::vector<const GNEJunction*> myMergingJunctions;

private:
    /// @brief set copy constructor private
    GUIViewObjectsHandler(const GUIViewObjectsHandler&) = default;

    /// @brief set assignment operator private
    GUIViewObjectsHandler& operator=(const GUIViewObjectsHandler&) = default;
};
