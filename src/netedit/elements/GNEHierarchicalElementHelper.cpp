/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
/// @file    GNEHierarchicalElementHelper.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2020
///
// Helper class for GNEHierarchicalElements
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/elements/additional/GNEAdditional.h>
#include <netedit/elements/additional/GNEShape.h>
#include <netedit/elements/additional/GNETAZElement.h>
#include <netedit/elements/data/GNEGenericData.h>
#include <netedit/elements/demand/GNEDemandElement.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/network/GNEJunction.h>
#include <netedit/elements/network/GNELane.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEHierarchicalElement.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEHierarchicalElementHelper::Container::Container(
	const std::vector<GNEJunction*>& _parentJunctions,
	const std::vector<GNEEdge*>& _parentEdges,
	const std::vector<GNELane*>& _parentLanes,
	const std::vector<GNEAdditional*>& _parentAdditionals,
	const std::vector<GNEShape*>& _parentShapes,
	const std::vector<GNETAZElement*>& _parentTAZElements,
	const std::vector<GNEDemandElement*>& _ParentDemandElements,
	const std::vector<GNEGenericData*>& _parentGenericDatas,
	const std::vector<GNEJunction*>& _childJunctions,
	const std::vector<GNEEdge*>& _childEdges,
	const std::vector<GNELane*>& _childLanes,
	const std::vector<GNEAdditional*>& _childAdditionals,
	const std::vector<GNEShape*>& _childShapes,
	const std::vector<GNETAZElement*>& _childTAZElements,
	const std::vector<GNEDemandElement*>& _childDemandElements,
	const std::vector<GNEGenericData*>& _childGenericDataElements):
	parentJunctions(_parentJunctions),
	parentEdges(_parentEdges),
	parentLanes(_parentLanes),
	parentAdditionals(_parentAdditionals),
	parentShapes(_parentShapes),
	parentTAZElements(_parentTAZElements),
	parentDemandElements(_ParentDemandElements),
	parentGenericDatas(_parentGenericDatas),
	childJunctions(_childJunctions),
	childEdges(_childEdges),
	childLanes(_childLanes),
	childAdditionals(_childAdditionals),
	childShapes(_childShapes),
	childTAZElements(_childTAZElements),
	childDemandElements(_childDemandElements),
	childGenericDataElements(_childGenericDataElements) {
}


void 
GNEHierarchicalElementHelper::Container::addParentElement(const GNEAttributeCarrier* AC,
    GNEJunction* junction, GNEEdge* edge, GNELane* lane, GNEAdditional* additional, 
    GNEShape* shape, GNETAZElement* TAZElement, GNEDemandElement* demandElement, 
    GNEGenericData* genericData) {
    // check junction
    if (junction) {
        if (std::find(parentJunctions.begin(), parentJunctions.end(), junction) != parentJunctions.end()) {
            throw ProcessError(junction->getTagStr() + " with ID='" + junction->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
        } else {
            parentJunctions.push_back(junction);
        }
    }
    // check edge
    if (edge) {
        if (std::find(parentEdges.begin(), parentEdges.end(), edge) != parentEdges.end()) {
            throw ProcessError(edge->getTagStr() + " with ID='" + edge->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
        } else {
            parentEdges.push_back(edge);
        }
    }
    // check lane
    if (lane) {
        if (std::find(parentLanes.begin(), parentLanes.end(), lane) != parentLanes.end()) {
            throw ProcessError(lane->getTagStr() + " with ID='" + lane->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
        } else {
            parentLanes.push_back(lane);
        }
    }
    // check additional
    if (additional) {
        if (std::find(parentAdditionals.begin(), parentAdditionals.end(), additional) != parentAdditionals.end()) {
            throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
        } else {
            parentAdditionals.push_back(additional);
        }
    }
    // check shape
    if (shape) {
        if (std::find(parentShapes.begin(), parentShapes.end(), shape) != parentShapes.end()) {
            throw ProcessError(shape->getTagStr() + " with ID='" + shape->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
        } else {
            parentShapes.push_back(shape);
        }
    }
    // check TAZElement
    if (TAZElement) {
        if (std::find(parentTAZElements.begin(), parentTAZElements.end(), TAZElement) != parentTAZElements.end()) {
            throw ProcessError(TAZElement->getTagStr() + " with ID='" + TAZElement->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
        } else {
            parentTAZElements.push_back(TAZElement);
        }
    }
    // check TAZElement
    if (demandElement) {
        if (std::find(parentDemandElements.begin(), parentDemandElements.end(), demandElement) != parentDemandElements.end()) {
            throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
        } else {
            parentDemandElements.push_back(demandElement);
        }
    }
    // check generic data
    if (genericData) {
        if (std::find(parentGenericDatas.begin(), parentGenericDatas.end(), genericData) != parentGenericDatas.end()) {
            throw ProcessError(genericData->getTagStr() + " with ID='" + genericData->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
        } else {
            parentGenericDatas.push_back(genericData);
        }
    }
}

// ---------------------------------------------------------------------------
// GNEHierarchicalElementHelper::ChildConnections - methods
// ---------------------------------------------------------------------------

GNEHierarchicalElementHelper::ChildConnections::ConnectionGeometry::ConnectionGeometry(GNELane* lane) :
    myLane(lane),
    myRotation(0) {
    // set position and length depending of shape's lengt
    if (lane->getLaneShape().length() - 6 > 0) {
        myPosition = lane->getLaneShape().positionAtOffset(lane->getLaneShape().length() - 6);
        myRotation = lane->getLaneShape().rotationDegreeAtOffset(lane->getLaneShape().length() - 6);
    } else {
        myPosition = lane->getLaneShape().positionAtOffset(lane->getLaneShape().length());
        myRotation = lane->getLaneShape().rotationDegreeAtOffset(lane->getLaneShape().length());
    }
}


const GNELane*
GNEHierarchicalElementHelper::ChildConnections::ConnectionGeometry::getLane() const {
    return myLane;
}


const Position&
GNEHierarchicalElementHelper::ChildConnections::ConnectionGeometry::getPosition() const {
    return myPosition;
}


double
GNEHierarchicalElementHelper::ChildConnections::ConnectionGeometry::getRotation() const {
    return myRotation;
}


GNEHierarchicalElementHelper::ChildConnections::ConnectionGeometry::ConnectionGeometry() :
    myLane(nullptr),
    myRotation(0) {
}


GNEHierarchicalElementHelper::ChildConnections::ChildConnections(GNEHierarchicalElement* hierarchicalElement) :
    myHierarchicalElement(hierarchicalElement) {}


void
GNEHierarchicalElementHelper::ChildConnections::update() {
    // first clear containers
    connectionsGeometries.clear();
    symbolsPositionAndRotation.clear();
    // calculate position and rotation of every simbol for every edge
    for (const auto& edge : myHierarchicalElement->getChildEdges()) {
        for (const auto& lane : edge->getLanes()) {
            symbolsPositionAndRotation.push_back(ConnectionGeometry(lane));
        }
    }
    // calculate position and rotation of every symbol for every lane
    for (const auto& lane : myHierarchicalElement->getChildLanes()) {
        symbolsPositionAndRotation.push_back(ConnectionGeometry(lane));
    }
    // calculate position for every child additional
    for (const auto& additional : myHierarchicalElement->getChildAdditionals()) {
        // check that additional position is different of parent position
        if (additional->getPositionInView() != myHierarchicalElement->getPositionInView()) {
            // create connection shape
            std::vector<Position> connectionShape;
            const double A = std::abs(additional->getPositionInView().x() - myHierarchicalElement->getPositionInView().x());
            const double B = std::abs(additional->getPositionInView().y() - myHierarchicalElement->getPositionInView().y());
            // Set positions of connection's vertex. Connection is build from Entry to E3
            connectionShape.push_back(additional->getPositionInView());
            if (myHierarchicalElement->getPositionInView().x() > additional->getPositionInView().x()) {
                if (myHierarchicalElement->getPositionInView().y() > additional->getPositionInView().y()) {
                    connectionShape.push_back(Position(additional->getPositionInView().x() + A, additional->getPositionInView().y()));
                } else {
                    connectionShape.push_back(Position(additional->getPositionInView().x(), additional->getPositionInView().y() - B));
                }
            } else {
                if (myHierarchicalElement->getPositionInView().y() > additional->getPositionInView().y()) {
                    connectionShape.push_back(Position(additional->getPositionInView().x(), additional->getPositionInView().y() + B));
                } else {
                    connectionShape.push_back(Position(additional->getPositionInView().x() - A, additional->getPositionInView().y()));
                }
            }
            connectionShape.push_back(myHierarchicalElement->getPositionInView());
            // declare Geometry
            GNEGeometry::Geometry geometry;
            // update geometry with connectino shape
            geometry.updateGeometry(connectionShape);
            // add geometry in connectionsGeometry
            connectionsGeometries.push_back(geometry);
        }
    }
    // calculate geometry for connections between parent and children
    for (const auto& symbol : symbolsPositionAndRotation) {
        // create connection shape
        std::vector<Position> connectionShape;
        const double A = std::abs(symbol.getPosition().x() - myHierarchicalElement->getPositionInView().x());
        const double B = std::abs(symbol.getPosition().y() - myHierarchicalElement->getPositionInView().y());
        // Set positions of connection's vertex. Connection is build from Entry to E3
        connectionShape.push_back(symbol.getPosition());
        if (myHierarchicalElement->getPositionInView().x() > symbol.getPosition().x()) {
            if (myHierarchicalElement->getPositionInView().y() > symbol.getPosition().y()) {
                connectionShape.push_back(Position(symbol.getPosition().x() + A, symbol.getPosition().y()));
            } else {
                connectionShape.push_back(Position(symbol.getPosition().x(), symbol.getPosition().y() - B));
            }
        } else {
            if (myHierarchicalElement->getPositionInView().y() > symbol.getPosition().y()) {
                connectionShape.push_back(Position(symbol.getPosition().x(), symbol.getPosition().y() + B));
            } else {
                connectionShape.push_back(Position(symbol.getPosition().x() - A, symbol.getPosition().y()));
            }
        }
        connectionShape.push_back(myHierarchicalElement->getPositionInView());
        // declare Geometry
        GNEGeometry::Geometry geometry;
        // update geometry with connectino shape
        geometry.updateGeometry(connectionShape);
        // add geometry in connectionsGeometry
        connectionsGeometries.push_back(geometry);
    }
}


void
GNEHierarchicalElementHelper::ChildConnections::drawConnection(const GUIVisualizationSettings& s, const GUIGlObjectType parentType, const double exaggeration) const {
    // Iterate over myConnectionPositions
    for (const auto& connectionGeometry : connectionsGeometries) {
        // Add a draw matrix
        glPushMatrix();
        // traslate in the Z axis
        glTranslated(0, 0, parentType - 0.01);
        // Set color of the base
        GLHelper::setColor(s.colorSettings.childConnections);
        // Draw box lines
        GLHelper::drawBoxLines(connectionGeometry.getShape(), connectionGeometry.getShapeRotations(), connectionGeometry.getShapeLengths(), exaggeration * 0.1);
        // Pop draw matrix
        glPopMatrix();
    }
}


void
GNEHierarchicalElementHelper::ChildConnections::drawDottedConnection(const GUIVisualizationSettings& s, const double exaggeration) const {
    // Iterate over myConnectionPositions
    for (const auto& connectionGeometry : connectionsGeometries) {
        // calculate dotted geometry
        GNEGeometry::DottedGeometry dottedGeometry(s, connectionGeometry.getShape(), false);
        // change default width
        dottedGeometry.setWidth(0.1);
        // use drawDottedContourLane to draw it
        GNEGeometry::drawDottedContourLane(true, s, dottedGeometry, exaggeration * 0.1, false, false);
    }
}

/****************************************************************************/
