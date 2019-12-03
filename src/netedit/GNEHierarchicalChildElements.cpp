/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEHierarchicalChildElements.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
/// @version $Id$
///
// A abstract class for representation of additional elements
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/GNEViewNet.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/demandelements/GNEDemandElement.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
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
        const std::vector<GNEDemandElement*>& childDemandElements) :
    myChildConnections(this),
    myChildEdges(childEdges),
    myChildLanes(childLanes),
    myChildShapes(childShapes),
    myChildAdditionals(childAdditionals),
    myChildDemandElements(childDemandElements),
    myAC(AC) {
    // fill SortedChildDemandElementsByType with all demand element tags (it's needed because getChildDemandElementsSortedByType(...) function is constant
    auto listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNEAttributeCarrier::TagType::TAGTYPE_DEMANDELEMENT, false);
    for (const auto& tag : listOfTags) {
        mySortedChildDemandElementsByType[tag];
    }
}


GNEHierarchicalChildElements::~GNEHierarchicalChildElements() {}


const Position&
GNEHierarchicalChildElements::getChildPosition(const GNELane* lane) {
    for (const auto& i : myChildConnections.symbolsPositionAndRotation) {
        if (i.lane == lane) {
            return i.pos;
        }
    }
    throw ProcessError("Lane doesn't exist");
}


double
GNEHierarchicalChildElements::getChildRotation(const GNELane* lane) {
    for (const auto& i : myChildConnections.symbolsPositionAndRotation) {
        if (i.lane == lane) {
            return i.rot;
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


void
GNEHierarchicalChildElements::sortChildAdditionals() {
    if (myAC->getTagProperty().getTag() == SUMO_TAG_E3DETECTOR) {
        // we need to sort Entry/Exits due additional.xds model
        std::vector<GNEAdditional*> sortedEntryExits;
        // obtain all entrys
        for (auto i : myChildAdditionals) {
            if (i->getTagProperty().getTag() == SUMO_TAG_DET_ENTRY) {
                sortedEntryExits.push_back(i);
            }
        }
        // obtain all exits
        for (auto i : myChildAdditionals) {
            if (i->getTagProperty().getTag() == SUMO_TAG_DET_EXIT) {
                sortedEntryExits.push_back(i);
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
        for (auto i : myChildAdditionals) {
            if (i->getTagProperty().getTag() == SUMO_TAG_TAZSOURCE) {
                sortedTAZSourceSink.push_back(i);
            }
        }
        // obtain all TAZSinks
        for (auto i : myChildAdditionals) {
            if (i->getTagProperty().getTag() == SUMO_TAG_TAZSINK) {
                sortedTAZSourceSink.push_back(i);
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
        for (auto i : myChildAdditionals) {
            sortedChildren.push_back(std::make_pair(std::make_pair(0., 0.), i));
            // set begin/start attribute
            if (i->getTagProperty().hasAttribute(SUMO_ATTR_TIME) && GNEAttributeCarrier::canParse<double>(i->getAttribute(SUMO_ATTR_TIME))) {
                sortedChildren.back().first.first = i->getAttributeDouble(SUMO_ATTR_TIME);
            } else if (i->getTagProperty().hasAttribute(SUMO_ATTR_BEGIN) && GNEAttributeCarrier::canParse<double>(i->getAttribute(SUMO_ATTR_BEGIN))) {
                sortedChildren.back().first.first = i->getAttributeDouble(SUMO_ATTR_BEGIN);
            }
            // set end attribute
            if (i->getTagProperty().hasAttribute(SUMO_ATTR_END) && GNEAttributeCarrier::canParse<double>(i->getAttribute(SUMO_ATTR_END))) {
                sortedChildren.back().first.second = i->getAttributeDouble(SUMO_ATTR_END);
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
    for (auto i : myChildAdditionals) {
        sortedChildren.push_back(std::make_pair(std::make_pair(0., 0.), i));
        // set begin/start attribute
        if (i->getTagProperty().hasAttribute(SUMO_ATTR_TIME) && GNEAttributeCarrier::canParse<double>(i->getAttribute(SUMO_ATTR_TIME))) {
            sortedChildren.back().first.first = i->getAttributeDouble(SUMO_ATTR_TIME);
        } else if (i->getTagProperty().hasAttribute(SUMO_ATTR_BEGIN) && GNEAttributeCarrier::canParse<double>(i->getAttribute(SUMO_ATTR_BEGIN))) {
            sortedChildren.back().first.first = i->getAttributeDouble(SUMO_ATTR_BEGIN);
        }
        // set end attribute
        if (i->getTagProperty().hasAttribute(SUMO_ATTR_END) && GNEAttributeCarrier::canParse<double>(i->getAttribute(SUMO_ATTR_END))) {
            sortedChildren.back().first.second = i->getAttributeDouble(SUMO_ATTR_END);
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
        mySortedChildDemandElementsByType.at(demandElement->getTagProperty().getTag()).insert(demandElement);
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
    if (it == myChildDemandElements.end()) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' doesn't exist in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        // first check if element is duplicated in vector
        bool singleElement = std::count(myChildDemandElements.begin(), myChildDemandElements.end(), demandElement) == 1;
        myChildDemandElements.erase(it);
        // only remove it from mySortedChildDemandElementsByType if is a single element
        if (singleElement) {
            mySortedChildDemandElementsByType.at(demandElement->getTagProperty().getTag()).erase(demandElement);
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


const std::set<GNEDemandElement*>&
GNEHierarchicalChildElements::getChildDemandElementsSortedByType(SumoXMLTag tag) const {
    return mySortedChildDemandElementsByType.at(tag);
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
    for (const auto& i : myChildEdges) {
        i->removeParentAdditional(elementChild);
    }
    // obtain new child edges (note: it can be empty)
    myChildEdges = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(elementChild->getViewNet()->getNet(), newEdgeIDs);
    // add demandElement into parent edges
    for (const auto& i : myChildEdges) {
        i->addParentAdditional(elementChild);
    }
    // update connections geometry
    myChildConnections.update();
}


void
GNEHierarchicalChildElements::changeChildLanes(GNEAdditional* elementChild, const std::string& newLaneIDs) {
    // remove demandElement of child lanes
    for (const auto& i : myChildLanes) {
        i->removeParentAdditional(elementChild);
    }
    // obtain new child lanes (note: it can be empty)
    myChildLanes = GNEAttributeCarrier::parse<std::vector<GNELane*> >(elementChild->getViewNet()->getNet(), newLaneIDs);
    // add demandElement into parent lanes
    for (const auto& i : myChildLanes) {
        i->addParentAdditional(elementChild);
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
    for (const auto& i : myHierarchicalElement->myChildEdges) {
        for (auto j : i->getLanes()) {
            Position pos;
            double rot;
            // set position and length depending of shape's lengt
            if (j->getLaneShape().length() - 6 > 0) {
                pos = j->getLaneShape().positionAtOffset(j->getLaneShape().length() - 6);
                rot = j->getLaneShape().rotationDegreeAtOffset(j->getLaneShape().length() - 6);
            } else {
                pos = j->getLaneShape().positionAtOffset(j->getLaneShape().length());
                rot = j->getLaneShape().rotationDegreeAtOffset(j->getLaneShape().length());
            }
            symbolsPositionAndRotation.push_back(ConnectionGeometry(j, pos, rot));
        }
    }
    // calculate position and rotation of every symbol for every lane
    for (const auto& i : myHierarchicalElement->myChildLanes) {
        Position pos;
        double rot;
        // set position and length depending of shape's lengt
        if (i->getLaneShape().length() - 6 > 0) {
            pos = i->getLaneShape().positionAtOffset(i->getLaneShape().length() - 6);
            rot = i->getLaneShape().rotationDegreeAtOffset(i->getLaneShape().length() - 6);
        } else {
            pos = i->getLaneShape().positionAtOffset(i->getLaneShape().length());
            rot = i->getLaneShape().rotationDegreeAtOffset(i->getLaneShape().length());
        }
        symbolsPositionAndRotation.push_back(ConnectionGeometry(i, pos, rot));
    }
    // calculate position for every child additional
    for (const auto& i : myHierarchicalElement->myChildAdditionals) {
        // check that position is different of position
        if (i->getPositionInView() != myHierarchicalElement->getPositionInView()) {
            std::vector<Position> posConnection;
            double A = std::abs(i->getPositionInView().x() - myHierarchicalElement->getPositionInView().x());
            double B = std::abs(i->getPositionInView().y() - myHierarchicalElement->getPositionInView().y());
            // Set positions of connection's vertex. Connection is build from Entry to E3
            posConnection.push_back(i->getPositionInView());
            if (myHierarchicalElement->getPositionInView().x() > i->getPositionInView().x()) {
                if (myHierarchicalElement->getPositionInView().y() > i->getPositionInView().y()) {
                    posConnection.push_back(Position(i->getPositionInView().x() + A, i->getPositionInView().y()));
                } else {
                    posConnection.push_back(Position(i->getPositionInView().x(), i->getPositionInView().y() - B));
                }
            } else {
                if (myHierarchicalElement->getPositionInView().y() > i->getPositionInView().y()) {
                    posConnection.push_back(Position(i->getPositionInView().x(), i->getPositionInView().y() + B));
                } else {
                    posConnection.push_back(Position(i->getPositionInView().x() - A, i->getPositionInView().y()));
                }
            }
            posConnection.push_back(myHierarchicalElement->getPositionInView());
            connectionPositions.push_back(posConnection);
        }
    }
    // calculate geometry for connections between parent and children
    for (const auto& i : symbolsPositionAndRotation) {
        std::vector<Position> posConnection;
        double A = std::abs(i.pos.x() - myHierarchicalElement->getPositionInView().x());
        double B = std::abs(i.pos.y() - myHierarchicalElement->getPositionInView().y());
        // Set positions of connection's vertex. Connection is build from Entry to E3
        posConnection.push_back(i.pos);
        if (myHierarchicalElement->getPositionInView().x() > i.pos.x()) {
            if (myHierarchicalElement->getPositionInView().y() > i.pos.y()) {
                posConnection.push_back(Position(i.pos.x() + A, i.pos.y()));
            } else {
                posConnection.push_back(Position(i.pos.x(), i.pos.y() - B));
            }
        } else {
            if (myHierarchicalElement->getPositionInView().y() > i.pos.y()) {
                posConnection.push_back(Position(i.pos.x(), i.pos.y() + B));
            } else {
                posConnection.push_back(Position(i.pos.x() - A, i.pos.y()));
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
        for (const auto& i : connectionPositions) {
            // Add a draw matrix
            glPushMatrix();
            // traslate in the Z axis
            glTranslated(0, 0, parentType - 0.01);
            // Set color of the base
            GLHelper::setColor(s.colorSettings.childConnections);
            // iterate over connections
            for (auto j = i.begin(); (j + 1) != i.end(); j++) {
                // Draw Lines
                GLHelper::drawLine((*j), (*(j + 1)));
            }
            // Pop draw matrix
            glPopMatrix();
        }
    }
}

/****************************************************************************/
