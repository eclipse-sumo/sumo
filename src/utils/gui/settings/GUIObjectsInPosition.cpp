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
/// @file    GUIObjectsInPosition.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 22
///
// Operations that must be applied after drawGL()
/****************************************************************************/
#include <config.h>
#include <algorithm>

#include <utils/shapes/Shape.h>

#include "GUIObjectsInPosition.h"


GUIObjectsInPosition::GUIObjectsInPosition() {}


void
GUIObjectsInPosition::clearElements() {
    // reset recompute boundaries
    recomputeBoundaries = GLO_NETWORK;
    // clear objects under cursor
    myElementsUnderCursor.clear();
    // reset marked elements
    markedEdge = nullptr;
    markedLane = nullptr;
    markedTAZ = nullptr;
    markedRoute = nullptr;
    markedFirstGeometryPoint = nullptr;
    markedSecondGeometryPoint = nullptr;
}


void
GUIObjectsInPosition::setSelectionPosition(Position pos) {
    mySelectionPosition = pos;
    mySelectionBoundary.reset();
}


bool
GUIObjectsInPosition::isElementSelected(const GUIGlObject* GLObject) const {
    // avoid to insert duplicated elements
    for (auto &elementLayer : myElementsUnderCursor) {
        for (auto &element : elementLayer.second) {
            if (element.object == GLObject) {
                return true;
            }
        }
    }
    return false;
}


bool
GUIObjectsInPosition::isGeometryPointSelected(const GUIGlObject* GLObject, const int index) const {
    // avoid to insert duplicated elements
    for (auto &elementLayer : myElementsUnderCursor) {
        for (auto &element : elementLayer.second) {
            if (element.object == GLObject) {
                return std::find(element.geometryPoints.begin(), element.geometryPoints.end(), index) != element.geometryPoints.end();
            }
        }
    }
    return false;
}


bool
GUIObjectsInPosition::checkCircleElement(const GUIGlObject* GLObject, const Position &center, const double radius) {
    if (mySelectionPosition.distanceSquaredTo2D(center) <= (radius * radius)) {
        return addElementUnderCursor(GLObject);
    } else {
        return false;
    }
}


bool
GUIObjectsInPosition::checkGeometryPoint(const GUIGlObject* GLObject, const int index, const Position &center, const double radius) {
    if (mySelectionPosition.distanceSquaredTo2D(center) <= (radius * radius)) {
        return addGeometryPointUnderCursor(GLObject, index);
    } else {
        return false;
    }
}


bool
GUIObjectsInPosition::checkPositionOverShape(const GUIGlObject* GLObject, const PositionVector &shape, const double radius) {
    // check if mouse is over shape
    const auto nearestPos = shape.positionAtOffset2D(shape.nearest_offset_to_point2D(mySelectionPosition));
    if (mySelectionPosition.distanceSquaredTo2D(nearestPos) <= (radius * radius)) {
        return addPositionOverShape(GLObject, nearestPos);
    } else {
        return false;
    }
}


bool
GUIObjectsInPosition::checkShapeElement(const GUIGlObject* GLObject, const PositionVector &shape) {
    if (shape.around(mySelectionPosition)) {
        return addElementUnderCursor(GLObject);
    } else {
        return false;
    }
}


const GUIObjectsInPosition::GLObjectsSortedContainer&
GUIObjectsInPosition::getElementsUnderCursor() const {
    return myElementsUnderCursor;
}


const std::vector<int>&
GUIObjectsInPosition::getGeometryPoints(const GUIGlObject* GLObject) const {
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
GUIObjectsInPosition::getPositionOverShape(const GUIGlObject* GLObject) const {
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
GUIObjectsInPosition::updateFrontElement(const GUIGlObject* GLObject) {
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
GUIObjectsInPosition::addElementUnderCursor(const GUIGlObject* GLObject) {
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
        return true;
    }
}


bool
GUIObjectsInPosition::addGeometryPointUnderCursor(const GUIGlObject* GLObject, const int newIndex) {
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
    return true;
}


bool
GUIObjectsInPosition::addPositionOverShape(const GUIGlObject* GLObject, const Position &pos) {
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
    return true;
}


/****************************************************************************/
