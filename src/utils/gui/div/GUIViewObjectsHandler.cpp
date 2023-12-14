/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
GUIViewObjectsHandler::clearElements() {
    // reset recompute boundaries
    recomputeBoundaries = GLO_NETWORK;
    // clear objects under cursor
    myElementsUnderCursor.clear();
    mySelectedObjets.clear();
    // reset marked elements
    markedEdge = nullptr;
    markedLane = nullptr;
    markedTAZ = nullptr;
    markedRoute = nullptr;
    markedFirstGeometryPoint = nullptr;
    markedSecondGeometryPoint = nullptr;
}


void
GUIViewObjectsHandler::setSelectionPosition(const Position &pos) {
    mySelectionPosition = pos;
    mySelectionBoundary.reset();
}


void
GUIViewObjectsHandler::setSelectionBoundary(const Boundary &boundary) {
    mySelectionPosition = Position::INVALID;
    mySelectionBoundary = boundary;
}


bool
GUIViewObjectsHandler::isElementSelected(const GUIGlObject* GLObject) const {
    return mySelectedObjets.find(GLObject) != mySelectedObjets.end();
}


bool
GUIViewObjectsHandler::checkCircleElement(const GUIVisualizationSettings::Detail d, const GUIGlObject* GLObject,
                                         const Position &center, const double radius) {
    // declare squared radius
    const double squaredRadius = (radius * radius);
    // continue depending if we're selecting a position or a boundary
    if (mySelectionBoundary.isInitialised()) {
        // continue depending of detail level
        if (d <= GUIVisualizationSettings::Detail::PreciseSelection) {
            // make a boundary using center and radius
            Boundary b;
            b.add(center);
            b.grow(radius);
            // check if boundary overlaps
            if (mySelectionBoundary.overlapsWith(b)) {
                return addElementUnderCursor(GLObject, false);
            } else {
                // get shape associated with SelectionBoundary
                const auto boundaryVertices = mySelectionBoundary.getShape(false);
                // check the four vertex
                for (const auto &vertex : boundaryVertices) {
                    if (vertex.distanceSquaredTo2D(center) <= squaredRadius) {
                        return addElementUnderCursor(GLObject, false);
                    }
                }
                // no intersection, then return false
                return false;
            }
        } else {
            // check if center is within mySelectionBoundary
            if (mySelectionBoundary.around(center)) {
                return addElementUnderCursor(GLObject, false);
            } else {
                return false;
            }
        }
    } else if (mySelectionPosition != Position::INVALID) {
        // check distance between selection position and center
        if (mySelectionPosition.distanceSquaredTo2D(center) <= squaredRadius) {
            return addElementUnderCursor(GLObject, false);
        } else {
            return false;
        }
    } else {
        return false;
    }
}


bool
GUIViewObjectsHandler::checkGeometryPoint(const GUIVisualizationSettings::Detail d, const GUIGlObject* GLObject,
                                         const int index, const Position &center, const double radius) {
    // declare squared radius
    const double squaredRadius = (radius * radius);
    // continue depending if we're selecting a position or a boundary
    if (mySelectionBoundary.isInitialised()) {
        // continue depending of detail level
        if (d <= GUIVisualizationSettings::Detail::PreciseSelection) {
            // make a boundary using center and radius
            Boundary b;
            b.add(center);
            b.grow(radius);
            // check if boundary overlaps
            if (mySelectionBoundary.overlapsWith(b)) {
                return addGeometryPointUnderCursor(GLObject, index);
            } else {
                // get shape associated with SelectionBoundary
                const auto boundaryVertices = mySelectionBoundary.getShape(false);
                // check the four vertex
                for (const auto &vertex : boundaryVertices) {
                    if (vertex.distanceSquaredTo2D(center) <= squaredRadius) {
                        return addGeometryPointUnderCursor(GLObject, index);
                    }
                }
                // no intersection, then return false
                return false;
            }
        } else {
            // check if center is within mySelectionBoundary
            if (mySelectionBoundary.around(center)) {
                return addElementUnderCursor(GLObject, false);
            } else {
                return false;
            }
        }
    } else if (mySelectionPosition != Position::INVALID) {
        // check distance between selection position and center
        if (mySelectionPosition.distanceSquaredTo2D(center) <= squaredRadius) {
            return addGeometryPointUnderCursor(GLObject, index);
        } else {
            return false;
        }
    } else {
        return false;
    }
}


bool
GUIViewObjectsHandler::checkPositionOverShape(const GUIVisualizationSettings::Detail d, const GUIGlObject* GLObject,
                                             const PositionVector &shape, const double distance) {
    if ((mySelectionPosition != Position::INVALID) && (d <= GUIVisualizationSettings::Detail::PreciseSelection)) {
        // obtain nearest position over shape
        const auto nearestPos = shape.positionAtOffset2D(shape.nearest_offset_to_point2D(mySelectionPosition));
        // check distance nearest position and pos
        if (mySelectionPosition.distanceSquaredTo2D(nearestPos) <= (distance * distance)) {
            return addPositionOverShape(GLObject, nearestPos);
        } else {
            return false;
        }
    } else {
        return false;
    }
}


bool
GUIViewObjectsHandler::checkShapeElement(const GUIVisualizationSettings::Detail d, const GUIGlObject* GLObject,
                                        const PositionVector &shape) {
    // continue depending if we're selecting a position or a boundary
    if (mySelectionBoundary.isInitialised()) {
        // first check if selection boundary contains the centering boundary of object
        if (mySelectionBoundary.contains(GLObject->getCenteringBoundary())) {
            return addElementUnderCursor(GLObject, true);
        }
        // check if shape crosses to selection boundary
        for (int i = 1; i < shape.size(); i++) {
            if (mySelectionBoundary.crosses(shape[i-1], shape[i])) {
                return addElementUnderCursor(GLObject, false);
            }
        }
        // no intersection, then return false
        return false;
    } else if (mySelectionPosition != Position::INVALID) {
        // check if selection position is around shape
        if (shape.around(mySelectionPosition)) {
            return addElementUnderCursor(GLObject, false);
        } else {
            return false;
        }
    } else {
        return false;
    }
}


const GUIViewObjectsHandler::GLObjectsSortedContainer&
GUIViewObjectsHandler::getSelectedObjects() const {
    return myElementsUnderCursor;
}


const std::vector<int>&
GUIViewObjectsHandler::getGeometryPoints(const GUIGlObject* GLObject) const {
    // avoid to insert duplicated elements
    for (auto &elementLayer : myElementsUnderCursor) {
        for (auto &element : elementLayer.second) {
            if (element.object == GLObject) {
                return element.geometryPoints;
            }
        }
    }
    return myEmptyGeometryPoints;
}


const Position&
GUIViewObjectsHandler::getPositionOverShape(const GUIGlObject* GLObject) const {
    // avoid to insert duplicated elements
    for (auto &elementLayer : myElementsUnderCursor) {
        for (auto &element : elementLayer.second) {
            if (element.object == GLObject) {
                return element.posOverShape;
            }
        }
    }
    return Position::INVALID;
}


void
GUIViewObjectsHandler::updateFrontElement(const GUIGlObject* GLObject) {
    ObjectContainer frontElement(nullptr);
    // extract element
    for (auto &elementLayer : myElementsUnderCursor) {
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
        myElementsUnderCursor[(double)GLO_FRONTELEMENT].push_back(frontElement);
    }
}


bool
GUIViewObjectsHandler::addElementUnderCursor(const GUIGlObject* GLObject, const bool fullBoundary) {
    // avoid to insert duplicated elements
    if (isElementSelected(GLObject)) {
        return false;
    } else {
        // check if this is an element with an associated layer
        const auto layer = dynamic_cast<const Shape*>(GLObject);
        if (layer) {
            auto &layerContainer = myElementsUnderCursor[layer->getShapeLayer() * -1];
            layerContainer.insert(layerContainer.begin(), ObjectContainer(GLObject));
        } else {
            auto &layerContainer = myElementsUnderCursor[GLObject->getType() * -1];
            layerContainer.insert(layerContainer.begin(), ObjectContainer(GLObject));
        }
        mySelectedObjets[GLObject] = fullBoundary;
        return true;
    }
}


bool
GUIViewObjectsHandler::addGeometryPointUnderCursor(const GUIGlObject* GLObject, const int newIndex) {
    // avoid to insert duplicated elements
    for (auto &elementLayer : myElementsUnderCursor) {
        for (auto &element : elementLayer.second) {
            if (element.object == GLObject) {
                // avoid double points
                for (auto &index : element.geometryPoints) {
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
    const auto layer = dynamic_cast<const Shape*>(GLObject);
    if (layer) {
        auto &layerContainer = myElementsUnderCursor[layer->getShapeLayer() * -1];
        auto it = layerContainer.insert(layerContainer.begin(), ObjectContainer(GLObject));
        it->geometryPoints.push_back(newIndex);
    } else {
        auto &layerContainer = myElementsUnderCursor[GLObject->getType() * -1];
        auto it = layerContainer.insert(layerContainer.begin(), ObjectContainer(GLObject));
        it->geometryPoints.push_back(newIndex);
    }
    mySelectedObjets[GLObject] = false;
    return true;
}


bool
GUIViewObjectsHandler::addPositionOverShape(const GUIGlObject* GLObject, const Position &pos) {
    // avoid to insert duplicated elements
    for (auto &elementLayer : myElementsUnderCursor) {
        for (auto &element : elementLayer.second) {
            if (element.object == GLObject) {
                if (element.posOverShape != Position::INVALID) {
                    return false;
                } else {
                    element.posOverShape = pos;
                    return true;
                }
            }
        }
    }
    // no element found then add it
    const auto layer = dynamic_cast<const Shape*>(GLObject);
    if (layer) {
        auto &layerContainer = myElementsUnderCursor[layer->getShapeLayer() * -1];
        auto it = layerContainer.insert(layerContainer.begin(), ObjectContainer(GLObject));
        it->posOverShape = pos;
    } else {
        auto &layerContainer = myElementsUnderCursor[GLObject->getType() * -1];
        auto it = layerContainer.insert(layerContainer.begin(), ObjectContainer(GLObject));
        it->posOverShape = pos;
    }
    mySelectedObjets[GLObject] = false;
    return true;
}


/****************************************************************************/
