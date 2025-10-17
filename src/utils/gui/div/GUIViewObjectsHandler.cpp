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
/// @file    GUIViewObjectsHandler.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 22
///
// class used for handle objects over view
/****************************************************************************/
#include <config.h>
#include <algorithm>

#include <utils/shapes/Shape.h>

#include "GUIViewObjectsHandler.h"


// ===========================================================================
// method definitions
// ===========================================================================

GUIViewObjectsHandler::GUIViewObjectsHandler() {}


void
GUIViewObjectsHandler::reset() {
    // reset recompute boundaries
    recomputeBoundaries = GLO_NETWORK;
    // clear objects containers
    mySortedSelectedObjects.clear();
    mySelectedObjects.clear();
    myNumberOfSelectedObjects = 0;
    myRedrawPathElements.clear();
    // reset marked elements
    myMergingJunctions.clear();
    markedEdge = nullptr;
    markedLane = nullptr;
    markedTAZ = nullptr;
    markedRoute = nullptr;
    markedFirstGeometryPoint = nullptr;
    markedSecondGeometryPoint = nullptr;
}


const Position&
GUIViewObjectsHandler::getSelectionPosition() const {
    return mySelectionPosition;
}


const Triangle&
GUIViewObjectsHandler::getSelectionTriangle() const {
    return mySelectionTriangle;
}


void
GUIViewObjectsHandler::setSelectionPosition(const Position& pos) {
    // set position selection
    mySelectionPosition = pos;
    // invalidate selection triangle
    mySelectionTriangle = Triangle::INVALID;
}


void
GUIViewObjectsHandler::setSelectionTriangle(const Triangle& triangle) {
    // invalidate position selection
    mySelectionPosition = Position::INVALID;
    // set selection triangle
    mySelectionTriangle = triangle;
}


bool
GUIViewObjectsHandler::selectingUsingRectangle() const {
    return mySelectionTriangle != Triangle::INVALID;
}


bool
GUIViewObjectsHandler::checkBoundaryParentObject(const GUIGlObject* GLObject, const double layer,
        const GUIGlObject* parent) {
    // first check if we're selecting for boundary
    if (mySelectionTriangle == Triangle::INVALID) {
        return false;
    }
    // try to find parent in seleted object
    auto finder = mySelectedObjects.find(parent);
    // if parent was found and was inserted with full boundary, insert it
    if (finder != mySelectedObjects.end() && finder->second && !isObjectSelected(GLObject)) {
        // insert element with full boundary
        return selectObject(GLObject, layer, false, nullptr);
    } else {
        return false;
    }
}


bool
GUIViewObjectsHandler::checkCircleObject(const GUIVisualizationSettings::Detail d, const GUIGlObject* GLObject,
        const Position& center, const double radius, const double layer) {
    // first check that object doesn't exist
    if (isObjectSelected(GLObject)) {
        return false;
    } else {
        // declare squared radius
        const double squaredRadius = (radius * radius);
        // continue depending if we're selecting a position or a boundary
        if (selectingUsingRectangle()) {
            // continue depending of detail level
            if (d <= GUIVisualizationSettings::Detail::PreciseSelection) {
                // check if triangle intersect with circle
                if (mySelectionTriangle.intersectWithCircle(center, radius)) {
                    return selectObject(GLObject, layer, false, nullptr);
                } else {
                    return false;
                }
            } else {
                // simply check if center is within triangle
                if (mySelectionTriangle.isPositionWithin(center)) {
                    return selectObject(GLObject, layer, false, nullptr);
                } else {
                    return false;
                }
            }
        } else if (mySelectionPosition != Position::INVALID) {
            // check distance between selection position and center
            if (mySelectionPosition.distanceSquaredTo2D(center) <= squaredRadius) {
                return selectObject(GLObject, layer, false, nullptr);
            } else {
                return false;
            }
        } else {
            return false;
        }
    }
}


bool
GUIViewObjectsHandler::checkGeometryPoint(const GUIVisualizationSettings::Detail d, const GUIGlObject* GLObject,
        const PositionVector& shape, const int index, const double layer, const double radius) {
    // obtain geometry point pos
    const auto geometryPointPos = shape[index];
    // declare squared radius
    const double squaredRadius = (radius * radius);
    // continue depending if we're selecting a position or a boundary
    if (selectingUsingRectangle()) {
        // continue depending of detail level
        if (d <= GUIVisualizationSettings::Detail::PreciseSelection) {
            // calculate boundary for geometry point
            Boundary geometryPointBoundary;
            geometryPointBoundary.add(geometryPointPos);
            // check if center is within mySelectionBoundary
            if (mySelectionTriangle.intersectWithCircle(geometryPointPos, radius)) {
                return selectGeometryPoint(GLObject, index, layer);
            } else {
                return false;
            }
        } else {
            // simply check if center is within triangle
            if (mySelectionTriangle.isPositionWithin(geometryPointPos)) {
                return selectObject(GLObject, layer, false, nullptr);
            } else {
                return false;
            }
        }
    } else if (mySelectionPosition != Position::INVALID) {
        // check distance between selection position and center
        if (mySelectionPosition.distanceSquaredTo2D(geometryPointPos) <= squaredRadius) {
            return selectGeometryPoint(GLObject, index, layer);
        } else {
            return false;
        }
    } else {
        return false;
    }
}


bool
GUIViewObjectsHandler::checkPositionOverShape(const GUIVisualizationSettings::Detail d, const GUIGlObject* GLObject,
        const PositionVector& shape, const double layer, const double distance) {
    // only process if we're selecting a precise position
    if ((mySelectionPosition != Position::INVALID) && (d <= GUIVisualizationSettings::Detail::PreciseSelection)) {
        // obtain nearest position over shape
        const auto nearestOffset = shape.nearest_offset_to_point2D(mySelectionPosition);
        const auto nearestPos = shape.positionAtOffset2D(nearestOffset);
        // check distance nearest position and pos
        if (mySelectionPosition.distanceSquaredTo2D(nearestPos) <= (distance * distance)) {
            return selectPositionOverShape(GLObject, nearestPos, layer, nearestOffset);
        } else {
            return false;
        }
    } else {
        return false;
    }
}


bool
GUIViewObjectsHandler::checkShapeObject(const GUIGlObject* GLObject, const PositionVector& shape, const Boundary& shapeBoundary,
                                        const double layer, const GNESegment* segment) {
    // first check that object doesn't exist
    if (isObjectSelected(GLObject)) {
        return false;
    } else if (selectingUsingRectangle()) {
        // avoid invalid boundaries
        if (!shapeBoundary.isInitialised()) {
            return false;
        }
        // check if triangle contains the given shape
        if (mySelectionTriangle.intersectWithShape(shape, shapeBoundary)) {
            return selectObject(GLObject, layer, false, segment);
        }
        // no intersection, then return false
        return false;
    } else if (mySelectionPosition != Position::INVALID) {
        // check if selection position is around shape
        if (shape.around(mySelectionPosition)) {
            return selectObject(GLObject, layer, false, segment);
        } else {
            return false;
        }
    } else {
        return false;
    }
}


bool
GUIViewObjectsHandler::selectObject(const GUIGlObject* GLObject, const double layer, const bool checkDuplicated,
                                    const GNESegment* segment) {
    // first check that object doesn't exist
    if (checkDuplicated && isObjectSelected(GLObject)) {
        return false;
    } else {
        auto& layerContainer = mySortedSelectedObjects[layer * -1];
        layerContainer.append(ObjectContainer(GLObject));
        mySelectedObjects[GLObject] = segment;
        myNumberOfSelectedObjects++;
        return true;
    }
}


bool
GUIViewObjectsHandler::selectGeometryPoint(const GUIGlObject* GLObject, const int newIndex,
        const double layer) {
    // avoid to insert duplicated elements
    for (auto& elementLayer : mySortedSelectedObjects) {
        for (auto& element : elementLayer.second) {
            if (element.object == GLObject) {
                // avoid double points
                for (auto& index : element.geometryPoints) {
                    if (index == newIndex) {
                        return false;
                    }
                }
                // add new index
                element.geometryPoints.push_back(newIndex);
                return true;
            }
        }
    }
    // no element found then add it
    auto& layerContainer = mySortedSelectedObjects[layer * -1];
    layerContainer.append(ObjectContainer(GLObject));
    layerContainer.back().geometryPoints.push_back(newIndex);
    mySelectedObjects[GLObject] = nullptr;
    myNumberOfSelectedObjects++;
    return true;
}


bool
GUIViewObjectsHandler::selectPositionOverShape(const GUIGlObject* GLObject, const Position& pos, const double layer,
        const double offset) {
    // avoid to insert duplicated elements
    for (auto& elementLayer : mySortedSelectedObjects) {
        for (auto& element : elementLayer.second) {
            if (element.object == GLObject) {
                if (element.posOverShape != Position::INVALID) {
                    return false;
                } else {
                    // set position and offset over shape
                    element.posOverShape = pos;
                    element.offset = offset;
                    return true;
                }
            }
        }
    }
    // no element found then add it
    auto& layerContainer = mySortedSelectedObjects[layer * -1];
    layerContainer.append(ObjectContainer(GLObject));
    layerContainer.back().posOverShape = pos;
    mySelectedObjects[GLObject] = nullptr;
    myNumberOfSelectedObjects++;
    return true;
}


bool
GUIViewObjectsHandler::isObjectSelected(const GUIGlObject* GLObject) const {
    return mySelectedObjects.find(GLObject) != mySelectedObjects.end();
}


bool
GUIViewObjectsHandler::checkRectangleSelection(const GUIVisualizationSettings& s, const GUIGlObject* GLObject,
        const double layer, const GUIGlObject* parent) {
    if (!s.drawForRectangleSelection) {
        return false;
    } else {
        return checkBoundaryParentObject(GLObject, layer, parent);
    }
}


const GUIViewObjectsHandler::GLObjectsSortedContainer&
GUIViewObjectsHandler::getSelectedObjects() const {
    return mySortedSelectedObjects;
}


const GNESegment*
GUIViewObjectsHandler::getSelectedSegment(const GUIGlObject* GLObject) const {
    auto finder = mySelectedObjects.find(GLObject);
    if (finder != mySelectedObjects.end()) {
        return finder->second;
    } else {
        return nullptr;
    }
}


const std::vector<int>&
GUIViewObjectsHandler::getSelectedGeometryPoints(const GUIGlObject* GLObject) const {
    // avoid to insert duplicated elements
    for (auto& elementLayer : mySortedSelectedObjects) {
        for (auto& element : elementLayer.second) {
            if (element.object == GLObject) {
                return element.geometryPoints;
            }
        }
    }
    return myEmptyGeometryPoints;
}


const Position&
GUIViewObjectsHandler::getSelectedPositionOverShape(const GUIGlObject* GLObject) const {
    // avoid to insert duplicated elements
    for (auto& elementLayer : mySortedSelectedObjects) {
        for (auto& element : elementLayer.second) {
            if (element.object == GLObject) {
                return element.posOverShape;
            }
        }
    }
    return Position::INVALID;
}


int
GUIViewObjectsHandler::getNumberOfSelectedObjects() const {
    return myNumberOfSelectedObjects;
}


void
GUIViewObjectsHandler::reverseSelectedObjects() {
    for (auto& layerContainer : mySortedSelectedObjects) {
        std::reverse(layerContainer.second.begin(), layerContainer.second.end());
    }
}


const std::set<const GNEPathElement*>&
GUIViewObjectsHandler::getRedrawPathElements() const {
    return myRedrawPathElements;
}


bool
GUIViewObjectsHandler::isPathElementMarkForRedraw(const GNEPathElement* pathElement) const {
    if (myRedrawPathElements.empty()) {
        return false;
    } else {
        return myRedrawPathElements.find(pathElement) != myRedrawPathElements.end();
    }
}


void
GUIViewObjectsHandler::addToRedrawPathElements(const GNEPathElement* pathElement) {
    myRedrawPathElements.insert(pathElement);
}


const std::vector<const GNEJunction*>&
GUIViewObjectsHandler::getMergingJunctions() const {
    return myMergingJunctions;
}


bool
GUIViewObjectsHandler::addMergingJunctions(const GNEJunction* junction) {
    // avoid insert duplicated junctions
    for (const auto mergingJunctions : myMergingJunctions) {
        if (mergingJunctions == junction) {
            return false;
        }
    }
    myMergingJunctions.push_back(junction);
    return true;
}


void
GUIViewObjectsHandler::updateFrontObject(const GUIGlObject* GLObject) {
    ObjectContainer frontElement(nullptr);
    // extract element
    for (auto& elementLayer : mySortedSelectedObjects) {
        auto it = elementLayer.second.begin();
        while (it != elementLayer.second.end()) {
            if (it->object == GLObject) {
                // copy element to front element
                frontElement.object = it->object;
                frontElement.geometryPoints = it->geometryPoints;
                // remove element from myElementsUnderCursor
                it = elementLayer.second.erase(it);
            } else {
                it++;
            }
        }
    }
    // add element again wit a new layer
    if (frontElement.object) {
        mySortedSelectedObjects[(double)GLO_FRONTELEMENT].append(frontElement);
    }
}


void
GUIViewObjectsHandler::isolateEdgeGeometryPoints() {
    // declare object container for edge
    ObjectContainer edgeWithGeometryPoints(nullptr);
    // check if there are edges with geometry points in mySortedSelectedObjects
    for (auto& elementLayer : mySortedSelectedObjects) {
        for (auto element : elementLayer.second) {
            if ((element.object->getType() == GLO_EDGE) && (element.geometryPoints.size() > 0)) {
                edgeWithGeometryPoints = element;
            }
        }
    }
    // continue if something was found
    if (edgeWithGeometryPoints.object != nullptr) {
        // clear all selected objects
        mySortedSelectedObjects.clear();
        // add edge with geometry points as front element
        mySortedSelectedObjects[(double)GLO_FRONTELEMENT].append(edgeWithGeometryPoints);
    }
}


void
GUIViewObjectsHandler::ObjectContainerLayer::append(const ObjectContainer& objectContainer) {
    if (capacity() == size()) {
        if (size() < 10) {
            reserve(size() + 10);
        } else {
            reserve(size() + 1000);
        }
    }
    push_back(objectContainer);
}

/****************************************************************************/
