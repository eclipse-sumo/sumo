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
/// @file    GNEHierarchicalChildElements.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
///
// A abstract class for representation of additional elements
/****************************************************************************/
#include <config.h>

#include <netedit/GNEViewNet.h>
#include <netedit/elements/additional/GNEAdditional.h>
#include <netedit/elements/demand/GNEDemandElement.h>
#include <netedit/elements/data/GNEGenericData.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/network/GNELane.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEHierarchicalChildElements.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEHierarchicalChildElements::GNEHierarchicalChildElements(GNEAttributeCarrier* AC,
        const std::vector<GNEEdge*>& childEdges,
        const std::vector<GNELane*>& childLanes,
        const std::vector<GNEShape*>& childShapes,
        const std::vector<GNEAdditional*>& childAdditionals,
        const std::vector<GNEDemandElement*>& childDemandElements,
        const std::vector<GNEGenericData*>& childGenericDataElements) :
    myChildConnections(this),
    myChildEdges(childEdges),
    myChildLanes(childLanes),
    myChildShapes(childShapes),
    myChildAdditionals(childAdditionals),
    myChildDemandElements(childDemandElements),
    myChildGenericDataElements(childGenericDataElements),
    myAC(AC) {
    // fill SortedChildDemandElementsByType with all demand element tags (it's needed because getChildDemandElementsSortedByType(...) function is constant
    auto listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNETagProperties::TagType::DEMANDELEMENT, false);
    for (const auto& tag : listOfTags) {
        myDemandElementsByType[tag] = {};
    }
}


GNEHierarchicalChildElements::~GNEHierarchicalChildElements() {}


const Position&
GNEHierarchicalChildElements::getChildPosition(const GNELane* lane) {
    for (const auto& childConnection : myChildConnections.symbolsPositionAndRotation) {
        if (childConnection.lane == lane) {
            return childConnection.pos;
        }
    }
    throw ProcessError("Lane doesn't exist");
}


double
GNEHierarchicalChildElements::getChildRotation(const GNELane* lane) {
    for (const auto& childConnection : myChildConnections.symbolsPositionAndRotation) {
        if (childConnection.lane == lane) {
            return childConnection.rot;
        }
    }
    throw ProcessError("Lane doesn't exist");
}


void
GNEHierarchicalChildElements::updateChildConnections() {
    myChildConnections.update();
}


void
GNEHierarchicalChildElements::drawChildConnections(const GUIVisualizationSettings& s, const GUIGlObjectType GLTypeParent) const {
    myChildConnections.draw(s, GLTypeParent);
}


void
GNEHierarchicalChildElements::addChildAdditional(GNEAdditional* additional) {
    // Check if additional is valid
    if (additional == nullptr) {
        throw InvalidArgument("Trying to add an empty child additional in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        // add it in child additional container
        myChildAdditionals.push_back(additional);
        // Check if children has to be sorted automatically
        if (myAC->getTagProperty().canAutomaticSortChildren()) {
            sortChildAdditionals();
        }
        // update parent additional after add additional (note: by default non-implemented)
        updateParentAdditional();
    }
}


void
GNEHierarchicalChildElements::removeChildAdditional(GNEAdditional* additional) {
    // First check that additional was already inserted
    auto it = std::find(myChildAdditionals.begin(), myChildAdditionals.end(), additional);
    if (it == myChildAdditionals.end()) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' doesn't exist in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myChildAdditionals.erase(it);
        // Check if children has to be sorted automatically
        if (myAC->getTagProperty().canAutomaticSortChildren()) {
            sortChildAdditionals();
        }
        // update parent additional after add additional (note: by default non-implemented)
        updateParentAdditional();
    }
}


const std::vector<GNEAdditional*>&
GNEHierarchicalChildElements::getChildAdditionals() const {
    return myChildAdditionals;
}


size_t 
GNEHierarchicalChildElements::getNumberOfChildAdditionals(GNETagProperties::TagType additionalType) const {
    size_t number = 0;
    // check additional type
    if (additionalType == GNETagProperties::TagType::ADDITIONALELEMENT) {
        for (const auto &additional : myChildAdditionals) {
            if (additional->getTagProperty().isAdditionalElement()){
                number++;
            }
        }
    } else if (additionalType == GNETagProperties::TagType::TAZ) {
        for (const auto& additional : myChildAdditionals) {
            if (additional->getTagProperty().isTAZ()) {
                number++;
            }
        }
    } else {
        throw ProcessError("invalid additionalType");
    }
    return number;
}


void
GNEHierarchicalChildElements::sortChildAdditionals() {
    if (myAC->getTagProperty().getTag() == SUMO_TAG_E3DETECTOR) {
        // we need to sort Entry/Exits due additional.xds model
        std::vector<GNEAdditional*> sortedEntryExits;
        // obtain all entrys
        for (const auto& additional : myChildAdditionals) {
            if (additional->getTagProperty().getTag() == SUMO_TAG_DET_ENTRY) {
                sortedEntryExits.push_back(additional);
            }
        }
        // obtain all exits
        for (const auto& additional : myChildAdditionals) {
            if (additional->getTagProperty().getTag() == SUMO_TAG_DET_EXIT) {
                sortedEntryExits.push_back(additional);
            }
        }
        // change myChildAdditionals for sortedEntryExits
        if (sortedEntryExits.size() == myChildAdditionals.size()) {
            myChildAdditionals = sortedEntryExits;
        } else {
            throw ProcessError("Some child additional were lost during sorting");
        }
    } else if (myAC->getTagProperty().getTag() == SUMO_TAG_TAZ) {
        // we need to sort Entry/Exits due additional.xds model
        std::vector<GNEAdditional*> sortedTAZSourceSink;
        // obtain all TAZSources
        for (const auto& additional : myChildAdditionals) {
            if (additional->getTagProperty().getTag() == SUMO_TAG_TAZSOURCE) {
                sortedTAZSourceSink.push_back(additional);
            }
        }
        // obtain all TAZSinks
        for (const auto& additional : myChildAdditionals) {
            if (additional->getTagProperty().getTag() == SUMO_TAG_TAZSINK) {
                sortedTAZSourceSink.push_back(additional);
            }
        }
        // change myChildAdditionals for sortedEntryExits
        if (sortedTAZSourceSink.size() == myChildAdditionals.size()) {
            myChildAdditionals = sortedTAZSourceSink;
        } else {
            throw ProcessError("Some child additional were lost during sorting");
        }
    } else {
        // declare a vector to keep sorted children
        std::vector<std::pair<std::pair<double, double>, GNEAdditional*> > sortedChildren;
        // iterate over child additional
        for (const auto& additional : myChildAdditionals) {
            sortedChildren.push_back(std::make_pair(std::make_pair(0., 0.), additional));
            // set begin/start attribute
            if (additional->getTagProperty().hasAttribute(SUMO_ATTR_TIME) && GNEAttributeCarrier::canParse<double>(additional->getAttribute(SUMO_ATTR_TIME))) {
                sortedChildren.back().first.first = additional->getAttributeDouble(SUMO_ATTR_TIME);
            } else if (additional->getTagProperty().hasAttribute(SUMO_ATTR_BEGIN) && GNEAttributeCarrier::canParse<double>(additional->getAttribute(SUMO_ATTR_BEGIN))) {
                sortedChildren.back().first.first = additional->getAttributeDouble(SUMO_ATTR_BEGIN);
            }
            // set end attribute
            if (additional->getTagProperty().hasAttribute(SUMO_ATTR_END) && GNEAttributeCarrier::canParse<double>(additional->getAttribute(SUMO_ATTR_END))) {
                sortedChildren.back().first.second = additional->getAttributeDouble(SUMO_ATTR_END);
            } else {
                sortedChildren.back().first.second = sortedChildren.back().first.first;
            }
        }
        // sort children
        std::sort(sortedChildren.begin(), sortedChildren.end());
        // make sure that number of sorted children is the same as the child additional
        if (sortedChildren.size() == myChildAdditionals.size()) {
            myChildAdditionals.clear();
            for (auto i : sortedChildren) {
                myChildAdditionals.push_back(i.second);
            }
        } else {
            throw ProcessError("Some child additional were lost during sorting");
        }
    }
}


bool
GNEHierarchicalChildElements::checkChildAdditionalsOverlapping() const {
    // declare a vector to keep sorted children
    std::vector<std::pair<std::pair<double, double>, GNEAdditional*> > sortedChildren;
    // iterate over child additional
    for (const auto& additional : myChildAdditionals) {
        sortedChildren.push_back(std::make_pair(std::make_pair(0., 0.), additional));
        // set begin/start attribute
        if (additional->getTagProperty().hasAttribute(SUMO_ATTR_TIME) && GNEAttributeCarrier::canParse<double>(additional->getAttribute(SUMO_ATTR_TIME))) {
            sortedChildren.back().first.first = additional->getAttributeDouble(SUMO_ATTR_TIME);
        } else if (additional->getTagProperty().hasAttribute(SUMO_ATTR_BEGIN) && GNEAttributeCarrier::canParse<double>(additional->getAttribute(SUMO_ATTR_BEGIN))) {
            sortedChildren.back().first.first = additional->getAttributeDouble(SUMO_ATTR_BEGIN);
        }
        // set end attribute
        if (additional->getTagProperty().hasAttribute(SUMO_ATTR_END) && GNEAttributeCarrier::canParse<double>(additional->getAttribute(SUMO_ATTR_END))) {
            sortedChildren.back().first.second = additional->getAttributeDouble(SUMO_ATTR_END);
        } else {
            sortedChildren.back().first.second = sortedChildren.back().first.first;
        }
    }
    // sort children
    std::sort(sortedChildren.begin(), sortedChildren.end());
    // make sure that number of sorted children is the same as the child additional
    if (sortedChildren.size() == myChildAdditionals.size()) {
        if (sortedChildren.size() <= 1) {
            return true;
        } else {
            // check overlapping
            for (int i = 0; i < (int)sortedChildren.size() - 1; i++) {
                if (sortedChildren.at(i).first.second > sortedChildren.at(i + 1).first.first) {
                    return false;
                }
            }
        }
        return true;
    } else {
        throw ProcessError("Some child additional were lost during sorting");
    }
}


void
GNEHierarchicalChildElements::addChildDemandElement(GNEDemandElement* demandElement) {
    // Check if demand element is valid
    if (demandElement == nullptr) {
        throw InvalidArgument("Trying to add an empty child demand element in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        // add it in demandElement child container
        myChildDemandElements.push_back(demandElement);
        // add it also in SortedChildDemandElementsByType container
        myDemandElementsByType.at(demandElement->getTagProperty().getTag()).push_back(demandElement);
        // Check if children has to be sorted automatically
        if (myAC->getTagProperty().canAutomaticSortChildren()) {
            sortChildDemandElements();
        }
    }
}


void
GNEHierarchicalChildElements::removeChildDemandElement(GNEDemandElement* demandElement) {
    // First check that demandElement was already inserted
    auto it = std::find(myChildDemandElements.begin(), myChildDemandElements.end(), demandElement);
    auto itByType = std::find(myDemandElementsByType.at(demandElement->getTagProperty().getTag()).begin(), myDemandElementsByType.at(demandElement->getTagProperty().getTag()).end(), demandElement);
    if (it == myChildDemandElements.end()) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' doesn't exist in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        // first check if element is duplicated in vector
        bool singleElement = std::count(myChildDemandElements.begin(), myChildDemandElements.end(), demandElement) == 1;
        myChildDemandElements.erase(it);
        // only remove it from mySortedChildDemandElementsByType if is a single element
        if (singleElement && (itByType != myDemandElementsByType.at(demandElement->getTagProperty().getTag()).end())) {
            myDemandElementsByType.at(demandElement->getTagProperty().getTag()).erase(itByType);
        }
        // Check if children has to be sorted automatically
        if (myAC->getTagProperty().canAutomaticSortChildren()) {
            sortChildDemandElements();
        }
    }
}


const std::vector<GNEDemandElement*>&
GNEHierarchicalChildElements::getChildDemandElements() const {
    return myChildDemandElements;
}


const std::vector<GNEDemandElement*>&
GNEHierarchicalChildElements::getChildDemandElementsByType(SumoXMLTag tag) const {
    return myDemandElementsByType.at(tag);
}


void
GNEHierarchicalChildElements::sortChildDemandElements() {
    // by default empty
}


bool
GNEHierarchicalChildElements::checkChildDemandElementsOverlapping() const {
    return true;
}


GNEDemandElement*
GNEHierarchicalChildElements::getPreviousChildDemandElement(const GNEDemandElement* demandElement) const {
    // find child demand element
    auto it = std::find(myChildDemandElements.begin(), myChildDemandElements.end(), demandElement);
    // return element or null depending of iterator
    if (it == myChildDemandElements.end()) {
        return nullptr;
    } else if (it == myChildDemandElements.begin()) {
        return nullptr;
    } else {
        return *(it - 1);
    }
}


GNEDemandElement*
GNEHierarchicalChildElements::getNextChildDemandElement(const GNEDemandElement* demandElement) const {
    // find child demand element
    auto it = std::find(myChildDemandElements.begin(), myChildDemandElements.end(), demandElement);
    // return element or null depending of iterator
    if (it == myChildDemandElements.end()) {
        return nullptr;
    } else if (it == (myChildDemandElements.end() - 1)) {
        return nullptr;
    } else {
        return *(it + 1);
    }
}


void
GNEHierarchicalChildElements::addChildGenericDataElement(GNEGenericData* genericDataElement) {
    // Check if demand element is valid
    if (genericDataElement == nullptr) {
        throw InvalidArgument("Trying to add an empty child generic data element in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        // add it in generic data element child container
        myChildGenericDataElements.push_back(genericDataElement);
    }
}


void
GNEHierarchicalChildElements::removeChildGenericDataElement(GNEGenericData* genericDataElement) {
    // First check that genericDataElement was already inserted
    auto it = std::find(myChildGenericDataElements.begin(), myChildGenericDataElements.end(), genericDataElement);
    if (it == myChildGenericDataElements.end()) {
        throw ProcessError(genericDataElement->getTagStr() + " with ID='" + genericDataElement->getID() + "' doesn't exist in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        // remove it from child demand elements
        myChildGenericDataElements.erase(it);
    }
}


const std::vector<GNEGenericData*>&
GNEHierarchicalChildElements:: getChildGenericDataElements() const {
    return myChildGenericDataElements;
}


void
GNEHierarchicalChildElements::addChildEdge(GNEEdge* edge) {
    // Check that edge is valid and doesn't exist previously
    if (edge == nullptr) {
        throw InvalidArgument("Trying to add an empty child edge in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myChildEdges.push_back(edge);
    }
}


void
GNEHierarchicalChildElements::removeChildEdge(GNEEdge* edge) {
    // Check that edge is valid and exist previously
    if (edge == nullptr) {
        throw InvalidArgument("Trying to remove an empty child edge in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myChildEdges.begin(), myChildEdges.end(), edge) == myChildEdges.end()) {
        throw InvalidArgument("Trying to remove a non previously inserted child edge in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myChildEdges.erase(std::find(myChildEdges.begin(), myChildEdges.end(), edge));
        // update connections geometry
        myChildConnections.update();
    }
}


const std::vector<GNEEdge*>&
GNEHierarchicalChildElements::getChildEdges() const {
    return myChildEdges;
}


void
GNEHierarchicalChildElements::addChildLane(GNELane* lane) {
    // Check if lane is valid
    if (lane == nullptr) {
        throw InvalidArgument("Trying to add an empty child lane in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myChildLanes.push_back(lane);
        // update connections geometry
        myChildConnections.update();
    }
}


void
GNEHierarchicalChildElements::removeChildLane(GNELane* lane) {
    // Check if lane is valid
    if (lane == nullptr) {
        throw InvalidArgument("Trying to remove an empty child lane in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myChildLanes.erase(std::find(myChildLanes.begin(), myChildLanes.end(), lane));
        // update connections geometry
        myChildConnections.update();
    }
}


const std::vector<GNELane*>&
GNEHierarchicalChildElements::getChildLanes() const {
    return myChildLanes;
}


void
GNEHierarchicalChildElements::addChildShape(GNEShape* shape) {
    // Check that shape is valid and doesn't exist previously
    if (shape == nullptr) {
        throw InvalidArgument("Trying to add an empty child shape in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myChildShapes.begin(), myChildShapes.end(), shape) != myChildShapes.end()) {
        throw InvalidArgument("Trying to add a duplicate child shape in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myChildShapes.push_back(shape);
        // update connections geometry
        myChildConnections.update();
    }
}


void
GNEHierarchicalChildElements::removeChildShape(GNEShape* shape) {
    // Check that shape is valid and exist previously
    if (shape == nullptr) {
        throw InvalidArgument("Trying to remove an empty child shape in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myChildShapes.begin(), myChildShapes.end(), shape) == myChildShapes.end()) {
        throw InvalidArgument("Trying to remove a non previously inserted child shape in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myChildShapes.erase(std::find(myChildShapes.begin(), myChildShapes.end(), shape));
        // update connections geometry
        myChildConnections.update();
    }
}


const std::vector<GNEShape*>&
GNEHierarchicalChildElements::getChildShapes() const {
    return myChildShapes;
}


void
GNEHierarchicalChildElements::updateParentAdditional() {
    // by default nothing to do
}


void
GNEHierarchicalChildElements::updateParentDemandElement() {
    // by default nothing to do
}


void
GNEHierarchicalChildElements::changeChildEdges(GNEAdditional* elementChild, const std::string& newEdgeIDs) {
    // remove demandElement of child edges
    for (const auto& edge : myChildEdges) {
        edge->removeParentAdditional(elementChild);
    }
    // obtain new child edges (note: it can be empty)
    myChildEdges = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(elementChild->getViewNet()->getNet(), newEdgeIDs);
    // add demandElement into parent edges
    for (const auto& edge : myChildEdges) {
        edge->addParentAdditional(elementChild);
    }
    // update connections geometry
    myChildConnections.update();
}


void
GNEHierarchicalChildElements::changeChildLanes(GNEAdditional* elementChild, const std::string& newLaneIDs) {
    // remove demandElement of child lanes
    for (const auto& lane : myChildLanes) {
        lane->removeParentAdditional(elementChild);
    }
    // obtain new child lanes (note: it can be empty)
    myChildLanes = GNEAttributeCarrier::parse<std::vector<GNELane*> >(elementChild->getViewNet()->getNet(), newLaneIDs);
    // add demandElement into parent lanes
    for (const auto& lane : myChildLanes) {
        lane->addParentAdditional(elementChild);
    }
    // update connections geometry
    myChildConnections.update();
}

// ---------------------------------------------------------------------------
// GNEHierarchicalChildElements::ChildConnections - methods
// ---------------------------------------------------------------------------

GNEHierarchicalChildElements::ChildConnections::ConnectionGeometry::ConnectionGeometry() :
    lane(nullptr),
    pos(Position::INVALID),
    rot(0) {
}


GNEHierarchicalChildElements::ChildConnections::ConnectionGeometry::ConnectionGeometry(GNELane* _lane, Position _pos, double _rot) :
    lane(_lane),
    pos(_pos),
    rot(_rot) {
}


GNEHierarchicalChildElements::ChildConnections::ChildConnections(GNEHierarchicalChildElements* hierarchicalElement) :
    myHierarchicalElement(hierarchicalElement) {}


void
GNEHierarchicalChildElements::ChildConnections::update() {
    // first clear connection positions
    connectionPositions.clear();
    symbolsPositionAndRotation.clear();
    // calculate position and rotation of every simbol for every edge
    for (const auto& edge : myHierarchicalElement->myChildEdges) {
        for (const auto& lane : edge->getLanes()) {
            Position pos;
            double rot;
            // set position and length depending of shape's lengt
            if (lane->getLaneShape().length() - 6 > 0) {
                pos = lane->getLaneShape().positionAtOffset(lane->getLaneShape().length() - 6);
                rot = lane->getLaneShape().rotationDegreeAtOffset(lane->getLaneShape().length() - 6);
            } else {
                pos = lane->getLaneShape().positionAtOffset(lane->getLaneShape().length());
                rot = lane->getLaneShape().rotationDegreeAtOffset(lane->getLaneShape().length());
            }
            symbolsPositionAndRotation.push_back(ConnectionGeometry(lane, pos, rot));
        }
    }
    // calculate position and rotation of every symbol for every lane
    for (const auto& lane : myHierarchicalElement->myChildLanes) {
        Position pos;
        double rot;
        // set position and length depending of shape's lengt
        if (lane->getLaneShape().length() - 6 > 0) {
            pos = lane->getLaneShape().positionAtOffset(lane->getLaneShape().length() - 6);
            rot = lane->getLaneShape().rotationDegreeAtOffset(lane->getLaneShape().length() - 6);
        } else {
            pos = lane->getLaneShape().positionAtOffset(lane->getLaneShape().length());
            rot = lane->getLaneShape().rotationDegreeAtOffset(lane->getLaneShape().length());
        }
        symbolsPositionAndRotation.push_back(ConnectionGeometry(lane, pos, rot));
    }
    // calculate position for every child additional
    for (const auto& additional : myHierarchicalElement->myChildAdditionals) {
        // check that position is different of position
        if (additional->getPositionInView() != myHierarchicalElement->getPositionInView()) {
            std::vector<Position> posConnection;
            double A = std::abs(additional->getPositionInView().x() - myHierarchicalElement->getPositionInView().x());
            double B = std::abs(additional->getPositionInView().y() - myHierarchicalElement->getPositionInView().y());
            // Set positions of connection's vertex. Connection is build from Entry to E3
            posConnection.push_back(additional->getPositionInView());
            if (myHierarchicalElement->getPositionInView().x() > additional->getPositionInView().x()) {
                if (myHierarchicalElement->getPositionInView().y() > additional->getPositionInView().y()) {
                    posConnection.push_back(Position(additional->getPositionInView().x() + A, additional->getPositionInView().y()));
                } else {
                    posConnection.push_back(Position(additional->getPositionInView().x(), additional->getPositionInView().y() - B));
                }
            } else {
                if (myHierarchicalElement->getPositionInView().y() > additional->getPositionInView().y()) {
                    posConnection.push_back(Position(additional->getPositionInView().x(), additional->getPositionInView().y() + B));
                } else {
                    posConnection.push_back(Position(additional->getPositionInView().x() - A, additional->getPositionInView().y()));
                }
            }
            posConnection.push_back(myHierarchicalElement->getPositionInView());
            connectionPositions.push_back(posConnection);
        }
    }
    // calculate geometry for connections between parent and children
    for (const auto& symbol : symbolsPositionAndRotation) {
        std::vector<Position> posConnection;
        double A = std::abs(symbol.pos.x() - myHierarchicalElement->getPositionInView().x());
        double B = std::abs(symbol.pos.y() - myHierarchicalElement->getPositionInView().y());
        // Set positions of connection's vertex. Connection is build from Entry to E3
        posConnection.push_back(symbol.pos);
        if (myHierarchicalElement->getPositionInView().x() > symbol.pos.x()) {
            if (myHierarchicalElement->getPositionInView().y() > symbol.pos.y()) {
                posConnection.push_back(Position(symbol.pos.x() + A, symbol.pos.y()));
            } else {
                posConnection.push_back(Position(symbol.pos.x(), symbol.pos.y() - B));
            }
        } else {
            if (myHierarchicalElement->getPositionInView().y() > symbol.pos.y()) {
                posConnection.push_back(Position(symbol.pos.x(), symbol.pos.y() + B));
            } else {
                posConnection.push_back(Position(symbol.pos.x() - A, symbol.pos.y()));
            }
        }
        posConnection.push_back(myHierarchicalElement->getPositionInView());
        connectionPositions.push_back(posConnection);
    }
}


void
GNEHierarchicalChildElements::ChildConnections::draw(const GUIVisualizationSettings& s, const GUIGlObjectType parentType) const {
    // first check if connections can be drawn
    if (!s.drawForRectangleSelection) {
        // Iterate over myConnectionPositions
        for (const auto& connection : connectionPositions) {
            // Add a draw matrix
            glPushMatrix();
            // traslate in the Z axis
            glTranslated(0, 0, parentType - 0.01);
            // Set color of the base
            GLHelper::setColor(s.colorSettings.childConnections);
            // iterate over connections
            for (auto position = connection.begin(); (position + 1) != connection.end(); position++) {
                // Draw Lines
                GLHelper::drawLine((*position), (*(position + 1)));
            }
            // Pop draw matrix
            glPopMatrix();
        }
    }
}


/****************************************************************************/
