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
#include <netedit/elements/network/GNEJunction.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>


// ===========================================================================
// member method definitions
// ===========================================================================

GNEHierarchicalElementHelper::Container::Container() {
    // fill SortedChildDemandElementsByType with all demand element tags (it's needed because getChildDemandElementsSortedByType(...) function is constant
    auto listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNETagProperties::TagType::DEMANDELEMENT, false);
    for (const auto& tag : listOfTags) {
        myDemandElementsByType[tag] = {};
    }
}

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
	const std::vector<GNEGenericData*>& _childGenericDatas):
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
    childGenericDatas(_childGenericDatas) {
    // fill SortedChildDemandElementsByType with all demand element tags (it's needed because getChildDemandElementsSortedByType(...) function is constant
    auto listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNETagProperties::TagType::DEMANDELEMENT, false);
    for (const auto& tag : listOfTags) {
        myDemandElementsByType[tag] = {};
    }
}


template <> void
GNEHierarchicalElementHelper::Container::addParentElement(const GNEAttributeCarrier* AC, GNEJunction* junction, const int position) {
    // check junction
    if (std::find(parentJunctions.begin(), parentJunctions.end(), junction) != parentJunctions.end()) {
        throw ProcessError(junction->getTagStr() + " with ID='" + junction->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else if (position != -1) {
        parentJunctions.insert(parentJunctions.begin() + position, junction);
    } else {
        parentJunctions.push_back(junction);
    }
}


template <> void
GNEHierarchicalElementHelper::Container::addParentElement(const GNEAttributeCarrier* AC, GNEEdge* edge, const int position) {
    // check edge
    if (std::find(parentEdges.begin(), parentEdges.end(), edge) != parentEdges.end()) {
        throw ProcessError(edge->getTagStr() + " with ID='" + edge->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else if (position != -1) {
        parentEdges.insert(parentEdges.begin() + position, edge);
    } else {
        parentEdges.push_back(edge);
    }
}


template <> void
GNEHierarchicalElementHelper::Container::addParentElement(const GNEAttributeCarrier* AC, GNELane* lane, const int position) {
    // check lane
    if (std::find(parentLanes.begin(), parentLanes.end(), lane) != parentLanes.end()) {
        throw ProcessError(lane->getTagStr() + " with ID='" + lane->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else if (position != -1) {
        parentLanes.insert(parentLanes.begin() + position, lane);
    } else {
        parentLanes.push_back(lane);
    }
}


template <> void
GNEHierarchicalElementHelper::Container::addParentElement(const GNEAttributeCarrier* AC, GNEAdditional* additional, const int position) {
    // check additional
    if (std::find(parentAdditionals.begin(), parentAdditionals.end(), additional) != parentAdditionals.end()) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else if (position != -1) {
        parentAdditionals.insert(parentAdditionals.begin() + position, additional);
    } else {
        parentAdditionals.push_back(additional);
    }
}


template <> void
GNEHierarchicalElementHelper::Container::addParentElement(const GNEAttributeCarrier* AC, GNEShape* shape, const int position) {
    // check shape
    if (std::find(parentShapes.begin(), parentShapes.end(), shape) != parentShapes.end()) {
        throw ProcessError(shape->getTagStr() + " with ID='" + shape->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else if (position != -1) {
        parentShapes.insert(parentShapes.begin() + position, shape);
    } else {
        parentShapes.push_back(shape);
    }
}


template <> void
GNEHierarchicalElementHelper::Container::addParentElement(const GNEAttributeCarrier* AC, GNETAZElement* TAZElement, const int position) {
    // check TAZElement
    if (std::find(parentTAZElements.begin(), parentTAZElements.end(), TAZElement) != parentTAZElements.end()) {
        throw ProcessError(TAZElement->getTagStr() + " with ID='" + TAZElement->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else if (position != -1) {
        parentTAZElements.insert(parentTAZElements.begin() + position, TAZElement);
    } else {
        parentTAZElements.push_back(TAZElement);
    }
}


template <> void
GNEHierarchicalElementHelper::Container::addParentElement(const GNEAttributeCarrier* AC, GNEDemandElement* demandElement, const int position) {
    // check TAZElement
    if (std::find(parentDemandElements.begin(), parentDemandElements.end(), demandElement) != parentDemandElements.end()) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else if (position != -1) {
        parentDemandElements.insert(parentDemandElements.begin() + position, demandElement);
    } else {
        parentDemandElements.push_back(demandElement);
    }
}


template <> void
GNEHierarchicalElementHelper::Container::addParentElement(const GNEAttributeCarrier* AC, GNEGenericData* genericData, const int position) {
    // check generic data
    if (std::find(parentGenericDatas.begin(), parentGenericDatas.end(), genericData) != parentGenericDatas.end()) {
        throw ProcessError(genericData->getTagStr() + " with ID='" + genericData->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else if (position != -1) {
        parentGenericDatas.insert(parentGenericDatas.begin() + position, genericData);
    } else {
        parentGenericDatas.push_back(genericData);
    }
}


template <> void
GNEHierarchicalElementHelper::Container::removeParentElement(const GNEAttributeCarrier* AC, GNEJunction* junction) {
    // check junction
    auto it = std::find(parentJunctions.begin(), parentJunctions.end(), junction);
    if (it == parentJunctions.end()) {
        throw ProcessError(junction->getTagStr() + " with ID='" + junction->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        parentJunctions.erase(it);
    }
}


template <> void
GNEHierarchicalElementHelper::Container::removeParentElement(const GNEAttributeCarrier* AC, GNEEdge* edge) {
    // check edge
    auto it = std::find(parentEdges.begin(), parentEdges.end(), edge);
    if (it == parentEdges.end()) {
        throw ProcessError(edge->getTagStr() + " with ID='" + edge->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        parentEdges.erase(it);
    }
}


template <> void
GNEHierarchicalElementHelper::Container::removeParentElement(const GNEAttributeCarrier* AC, GNELane* lane) {
    // check lane
    auto it = std::find(parentLanes.begin(), parentLanes.end(), lane);
    if (it == parentLanes.end()) {
        throw ProcessError(lane->getTagStr() + " with ID='" + lane->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        parentLanes.erase(it);
    }
}


template <> void
GNEHierarchicalElementHelper::Container::removeParentElement(const GNEAttributeCarrier* AC, GNEAdditional* additional) {
    // check additional
    auto it = std::find(parentAdditionals.begin(), parentAdditionals.end(), additional);
    if (it == parentAdditionals.end()) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        parentAdditionals.erase(it);
    }
}


template <> void
GNEHierarchicalElementHelper::Container::removeParentElement(const GNEAttributeCarrier* AC, GNEShape* shape) {
    // check shape
    auto it = std::find(parentShapes.begin(), parentShapes.end(), shape);
    if (it == parentShapes.end()) {
        throw ProcessError(shape->getTagStr() + " with ID='" + shape->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        parentShapes.erase(it);
    }
}


template <> void
GNEHierarchicalElementHelper::Container::removeParentElement(const GNEAttributeCarrier* AC, GNETAZElement* TAZElement) {
    // check TAZElement
    auto it = std::find(parentTAZElements.begin(), parentTAZElements.end(), TAZElement);
    if (it == parentTAZElements.end()) {
        throw ProcessError(TAZElement->getTagStr() + " with ID='" + TAZElement->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        parentTAZElements.erase(it);
    }
}


template <> void
GNEHierarchicalElementHelper::Container::removeParentElement(const GNEAttributeCarrier* AC, GNEDemandElement* demandElement) {
    // check TAZElement
    auto it = std::find(parentDemandElements.begin(), parentDemandElements.end(), demandElement);
    if (it == parentDemandElements.end()) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        parentDemandElements.erase(it);
    }
}


template <> void
GNEHierarchicalElementHelper::Container::removeParentElement(const GNEAttributeCarrier* AC, GNEGenericData* genericData) {
    // check generic data
    auto it = std::find(parentGenericDatas.begin(), parentGenericDatas.end(), genericData);
    if (it == parentGenericDatas.end()) {
        throw ProcessError(genericData->getTagStr() + " with ID='" + genericData->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        parentGenericDatas.erase(it);
    }
}


template <> void
GNEHierarchicalElementHelper::Container::addChildElement(const GNEAttributeCarrier* AC, GNEJunction* junction, const int position) {
    // check junction
    if (std::find(childJunctions.begin(), childJunctions.end(), junction) != childJunctions.end()) {
        throw ProcessError(junction->getTagStr() + " with ID='" + junction->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else if (position != -1) {
        childJunctions.insert(childJunctions.begin() + position, junction);
    } else {
        childJunctions.push_back(junction);
    }
}


template <> void
GNEHierarchicalElementHelper::Container::addChildElement(const GNEAttributeCarrier* AC, GNEEdge* edge, const int position) {
    // check edge
    if (std::find(childEdges.begin(), childEdges.end(), edge) != childEdges.end()) {
        throw ProcessError(edge->getTagStr() + " with ID='" + edge->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else if (position != -1) {
        childEdges.insert(childEdges.begin() + position, edge);
    } else {
        childEdges.push_back(edge);
    }
}


template <> void
GNEHierarchicalElementHelper::Container::addChildElement(const GNEAttributeCarrier* AC, GNELane* lane, const int position) {
    // check lane
    if (std::find(childLanes.begin(), childLanes.end(), lane) != childLanes.end()) {
        throw ProcessError(lane->getTagStr() + " with ID='" + lane->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else if (position != -1) {
        childLanes.insert(childLanes.begin() + position, lane);
    } else {
        childLanes.push_back(lane);
    }
}


template <> void
GNEHierarchicalElementHelper::Container::addChildElement(const GNEAttributeCarrier* AC, GNEAdditional* additional, const int position) {
    // check additional
    if (std::find(childAdditionals.begin(), childAdditionals.end(), additional) != childAdditionals.end()) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else if (position != -1) {
        childAdditionals.insert(childAdditionals.begin() + position, additional);
    } else {
        childAdditionals.push_back(additional);
    }
}


template <> void
GNEHierarchicalElementHelper::Container::addChildElement(const GNEAttributeCarrier* AC, GNEShape* shape, const int position) {
    // check shape
    if (std::find(childShapes.begin(), childShapes.end(), shape) != childShapes.end()) {
        throw ProcessError(shape->getTagStr() + " with ID='" + shape->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else if (position != -1) {
        childShapes.insert(childShapes.begin() + position, shape);
    } else {
        childShapes.push_back(shape);
    }
}


template <> void
GNEHierarchicalElementHelper::Container::addChildElement(const GNEAttributeCarrier* AC, GNETAZElement* TAZElement, const int position) {
    // check TAZElement
    if (std::find(childTAZElements.begin(), childTAZElements.end(), TAZElement) != childTAZElements.end()) {
        throw ProcessError(TAZElement->getTagStr() + " with ID='" + TAZElement->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else if (position != -1) {
        childTAZElements.insert(childTAZElements.begin() + position, TAZElement);
    } else {
        childTAZElements.push_back(TAZElement);
    }
}


template <> void
GNEHierarchicalElementHelper::Container::addChildElement(const GNEAttributeCarrier* AC, GNEDemandElement* demandElement, const int position) {
    // check TAZElement
    if (std::find(childDemandElements.begin(), childDemandElements.end(), demandElement) != childDemandElements.end()) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        if (position != -1) {
            childDemandElements.insert(childDemandElements.begin() + position, demandElement);
        } else {
            childDemandElements.push_back(demandElement);
        }
        // add it also in SortedChildDemandElementsByType container
        myDemandElementsByType.at(demandElement->getTagProperty().getTag()).push_back(demandElement);
    }
}


template <> void
GNEHierarchicalElementHelper::Container::addChildElement(const GNEAttributeCarrier* AC, GNEGenericData* genericData, const int position) {
    // check generic data
    if (std::find(childGenericDatas.begin(), childGenericDatas.end(), genericData) != childGenericDatas.end()) {
        throw ProcessError(genericData->getTagStr() + " with ID='" + genericData->getID() + "' was already inserted in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else if (position != -1) {
        childGenericDatas.insert(childGenericDatas.begin() + position, genericData);
    } else {
        childGenericDatas.push_back(genericData);
    }
}


template <> void
GNEHierarchicalElementHelper::Container::removeChildElement(const GNEAttributeCarrier* AC, GNEJunction* junction) {
    // check junction
    auto it = std::find(childJunctions.begin(), childJunctions.end(), junction);
    if (it == childJunctions.end()) {
        throw ProcessError(junction->getTagStr() + " with ID='" + junction->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        childJunctions.erase(it);
    }
}


template <> void
GNEHierarchicalElementHelper::Container::removeChildElement(const GNEAttributeCarrier* AC, GNEEdge* edge) {
    // check edge
    auto it = std::find(childEdges.begin(), childEdges.end(), edge);
    if (it == childEdges.end()) {
        throw ProcessError(edge->getTagStr() + " with ID='" + edge->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        childEdges.erase(it);
    }
}


template <> void
GNEHierarchicalElementHelper::Container::removeChildElement(const GNEAttributeCarrier* AC, GNELane* lane) {
    // check lane
    auto it = std::find(childLanes.begin(), childLanes.end(), lane);
    if (it == childLanes.end()) {
        throw ProcessError(lane->getTagStr() + " with ID='" + lane->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        childLanes.erase(it);
    }
}


template <> void
GNEHierarchicalElementHelper::Container::removeChildElement(const GNEAttributeCarrier* AC, GNEAdditional* additional) {
    // check additional
    auto it = std::find(childAdditionals.begin(), childAdditionals.end(), additional);
    if (it == childAdditionals.end()) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        childAdditionals.erase(it);
    }
}


template <> void
GNEHierarchicalElementHelper::Container::removeChildElement(const GNEAttributeCarrier* AC, GNEShape* shape) {
    // check shape
    auto it = std::find(childShapes.begin(), childShapes.end(), shape);
    if (it == childShapes.end()) {
        throw ProcessError(shape->getTagStr() + " with ID='" + shape->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        childShapes.erase(it);
    }
}


template <> void
GNEHierarchicalElementHelper::Container::removeChildElement(const GNEAttributeCarrier* AC, GNETAZElement* TAZElement) {
    // check TAZElement
    auto it = std::find(childTAZElements.begin(), childTAZElements.end(), TAZElement);
    if (it == childTAZElements.end()) {
        throw ProcessError(TAZElement->getTagStr() + " with ID='" + TAZElement->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        childTAZElements.erase(it);
    }
}


template <> void
GNEHierarchicalElementHelper::Container::removeChildElement(const GNEAttributeCarrier* AC, GNEDemandElement* demandElement) {
    // check TAZElement
    auto it = std::find(childDemandElements.begin(), childDemandElements.end(), demandElement);
    auto itByType = std::find(myDemandElementsByType.at(demandElement->getTagProperty().getTag()).begin(), myDemandElementsByType.at(demandElement->getTagProperty().getTag()).end(), demandElement);
    if (it == childDemandElements.end()) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        childDemandElements.erase(it);
        // only remove it from mySortedChildDemandElementsByType if is a single element
        if ((std::count(childDemandElements.begin(), childDemandElements.end(), demandElement) == 1) && 
            (itByType != myDemandElementsByType.at(demandElement->getTagProperty().getTag()).end())) {
            myDemandElementsByType.at(demandElement->getTagProperty().getTag()).erase(itByType);
        }
    }
}


template <> void
GNEHierarchicalElementHelper::Container::removeChildElement(const GNEAttributeCarrier* AC, GNEGenericData* genericData) {
    // check generic data
    auto it = std::find(childGenericDatas.begin(), childGenericDatas.end(), genericData);
    if (it == childGenericDatas.end()) {
        throw ProcessError(genericData->getTagStr() + " with ID='" + genericData->getID() + "' doesn't exist in " + AC->getTagStr() + " with ID='" + AC->getID() + "'");
    } else {
        childGenericDatas.erase(it);
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
