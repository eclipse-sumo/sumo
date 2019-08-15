/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEHierarchicalElementChildren.cpp
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

#include "GNEHierarchicalElementChildren.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEHierarchicalElementChildren::GNEHierarchicalElementChildren(GNEAttributeCarrier* AC,
        const std::vector<GNEEdge*>& edgeChildren,
        const std::vector<GNELane*>& laneChildren,
        const std::vector<GNEShape*>& shapeChildren,
        const std::vector<GNEAdditional*>& additionalChildren,
        const std::vector<GNEDemandElement*>& demandElementChildren) :
    myChildConnections(this),
    myEdgeChildren(edgeChildren),
    myLaneChildren(laneChildren),
    myShapeChildren(shapeChildren),
    myAdditionalChildren(additionalChildren),
    myDemandElementChildren(demandElementChildren),
    myAC(AC) {
    // fill SortedDemandElementChildrenByType with all demand element tags (it's needed because getSortedDemandElementChildrenByType(...) function is constant
    auto listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNEAttributeCarrier::TagType::TAGTYPE_DEMANDELEMENT, false);
    for (const auto& i : listOfTags) {
        mySortedDemandElementChildrenByType[i];
    }
}


GNEHierarchicalElementChildren::~GNEHierarchicalElementChildren() {}


const Position&
GNEHierarchicalElementChildren::getChildPosition(const GNELane* lane) {
    for (const auto& i : myChildConnections.symbolsPositionAndRotation) {
        if (i.lane == lane) {
            return i.pos;
        }
    }
    throw ProcessError("Lane doesn't exist");
}


double
GNEHierarchicalElementChildren::getChildRotation(const GNELane* lane) {
    for (const auto& i : myChildConnections.symbolsPositionAndRotation) {
        if (i.lane == lane) {
            return i.rot;
        }
    }
    throw ProcessError("Lane doesn't exist");
}


void
GNEHierarchicalElementChildren::updateChildConnections() {
    myChildConnections.update();
}


void
GNEHierarchicalElementChildren::drawChildConnections(const GUIVisualizationSettings& s, const GUIGlObjectType GLTypeParent) const {
    myChildConnections.draw(s, GLTypeParent);
}


void
GNEHierarchicalElementChildren::addAdditionalChild(GNEAdditional* additional) {
    // Check if additional is valid
    if (additional == nullptr) {
        throw InvalidArgument("Trying to add an empty additional child in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        // add it in additional children container
        myAdditionalChildren.push_back(additional);
        // Check if children has to be sorted automatically
        if (myAC->getTagProperty().canAutomaticSortChildren()) {
            sortAdditionalChildren();
        }
        // update additional parent after add additional (note: by default non-implemented)
        updateAdditionalParent();
    }
}


void
GNEHierarchicalElementChildren::removeAdditionalChild(GNEAdditional* additional) {
    // First check that additional was already inserted
    auto it = std::find(myAdditionalChildren.begin(), myAdditionalChildren.end(), additional);
    if (it == myAdditionalChildren.end()) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' doesn't exist in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myAdditionalChildren.erase(it);
        // Check if children has to be sorted automatically
        if (myAC->getTagProperty().canAutomaticSortChildren()) {
            sortAdditionalChildren();
        }
        // update additional parent after add additional (note: by default non-implemented)
        updateAdditionalParent();
    }
}


const std::vector<GNEAdditional*>&
GNEHierarchicalElementChildren::getAdditionalChildren() const {
    return myAdditionalChildren;
}


void
GNEHierarchicalElementChildren::sortAdditionalChildren() {
    if (myAC->getTagProperty().getTag() == SUMO_TAG_E3DETECTOR) {
        // we need to sort Entry/Exits due additional.xds model
        std::vector<GNEAdditional*> sortedEntryExits;
        // obtain all entrys
        for (auto i : myAdditionalChildren) {
            if (i->getTagProperty().getTag() == SUMO_TAG_DET_ENTRY) {
                sortedEntryExits.push_back(i);
            }
        }
        // obtain all exits
        for (auto i : myAdditionalChildren) {
            if (i->getTagProperty().getTag() == SUMO_TAG_DET_EXIT) {
                sortedEntryExits.push_back(i);
            }
        }
        // change myAdditionalChildren for sortedEntryExits
        if (sortedEntryExits.size() == myAdditionalChildren.size()) {
            myAdditionalChildren = sortedEntryExits;
        } else {
            throw ProcessError("Some additional children were lost during sorting");
        }
    } else if (myAC->getTagProperty().getTag() == SUMO_TAG_TAZ) {
        // we need to sort Entry/Exits due additional.xds model
        std::vector<GNEAdditional*> sortedTAZSourceSink;
        // obtain all TAZSources
        for (auto i : myAdditionalChildren) {
            if (i->getTagProperty().getTag() == SUMO_TAG_TAZSOURCE) {
                sortedTAZSourceSink.push_back(i);
            }
        }
        // obtain all TAZSinks
        for (auto i : myAdditionalChildren) {
            if (i->getTagProperty().getTag() == SUMO_TAG_TAZSINK) {
                sortedTAZSourceSink.push_back(i);
            }
        }
        // change myAdditionalChildren for sortedEntryExits
        if (sortedTAZSourceSink.size() == myAdditionalChildren.size()) {
            myAdditionalChildren = sortedTAZSourceSink;
        } else {
            throw ProcessError("Some additional children were lost during sorting");
        }
    } else {
        // declare a vector to keep sorted children
        std::vector<std::pair<std::pair<double, double>, GNEAdditional*> > sortedChildren;
        // iterate over additional children
        for (auto i : myAdditionalChildren) {
            sortedChildren.push_back(std::make_pair(std::make_pair(0., 0.), i));
            // set begin/start attribute
            if (i->getTagProperty().hasAttribute(SUMO_ATTR_TIME) && GNEAttributeCarrier::canParse<double>(i->getAttribute(SUMO_ATTR_TIME))) {
                sortedChildren.back().first.first = GNEAttributeCarrier::parse<double>(i->getAttribute(SUMO_ATTR_TIME));
            } else if (i->getTagProperty().hasAttribute(SUMO_ATTR_BEGIN) && GNEAttributeCarrier::canParse<double>(i->getAttribute(SUMO_ATTR_BEGIN))) {
                sortedChildren.back().first.first = GNEAttributeCarrier::parse<double>(i->getAttribute(SUMO_ATTR_BEGIN));
            }
            // set end attribute
            if (i->getTagProperty().hasAttribute(SUMO_ATTR_END) && GNEAttributeCarrier::canParse<double>(i->getAttribute(SUMO_ATTR_END))) {
                sortedChildren.back().first.second = GNEAttributeCarrier::parse<double>(i->getAttribute(SUMO_ATTR_END));
            } else {
                sortedChildren.back().first.second = sortedChildren.back().first.first;
            }
        }
        // sort children
        std::sort(sortedChildren.begin(), sortedChildren.end());
        // make sure that number of sorted children is the same as the additional children
        if (sortedChildren.size() == myAdditionalChildren.size()) {
            myAdditionalChildren.clear();
            for (auto i : sortedChildren) {
                myAdditionalChildren.push_back(i.second);
            }
        } else {
            throw ProcessError("Some additional children were lost during sorting");
        }
    }
}


bool
GNEHierarchicalElementChildren::checkAdditionalChildrenOverlapping() const {
    // declare a vector to keep sorted children
    std::vector<std::pair<std::pair<double, double>, GNEAdditional*> > sortedChildren;
    // iterate over additional children
    for (auto i : myAdditionalChildren) {
        sortedChildren.push_back(std::make_pair(std::make_pair(0., 0.), i));
        // set begin/start attribute
        if (i->getTagProperty().hasAttribute(SUMO_ATTR_TIME) && GNEAttributeCarrier::canParse<double>(i->getAttribute(SUMO_ATTR_TIME))) {
            sortedChildren.back().first.first = GNEAttributeCarrier::parse<double>(i->getAttribute(SUMO_ATTR_TIME));
        } else if (i->getTagProperty().hasAttribute(SUMO_ATTR_BEGIN) && GNEAttributeCarrier::canParse<double>(i->getAttribute(SUMO_ATTR_BEGIN))) {
            sortedChildren.back().first.first = GNEAttributeCarrier::parse<double>(i->getAttribute(SUMO_ATTR_BEGIN));
        }
        // set end attribute
        if (i->getTagProperty().hasAttribute(SUMO_ATTR_END) && GNEAttributeCarrier::canParse<double>(i->getAttribute(SUMO_ATTR_END))) {
            sortedChildren.back().first.second = GNEAttributeCarrier::parse<double>(i->getAttribute(SUMO_ATTR_END));
        } else {
            sortedChildren.back().first.second = sortedChildren.back().first.first;
        }
    }
    // sort children
    std::sort(sortedChildren.begin(), sortedChildren.end());
    // make sure that number of sorted children is the same as the additional children
    if (sortedChildren.size() == myAdditionalChildren.size()) {
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
        throw ProcessError("Some additional children were lost during sorting");
    }
}


void
GNEHierarchicalElementChildren::addDemandElementChild(GNEDemandElement* demandElement) {
    // Check if demand element is valid
    if (demandElement == nullptr) {
        throw InvalidArgument("Trying to add an empty demand element child in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        // add it in demandElement child container
        myDemandElementChildren.push_back(demandElement);
        // add it also in SortedDemandElementChildrenByType container
        mySortedDemandElementChildrenByType.at(demandElement->getTagProperty().getTag()).insert(demandElement);
        // Check if children has to be sorted automatically
        if (myAC->getTagProperty().canAutomaticSortChildren()) {
            sortDemandElementChildren();
        }
    }
}


void
GNEHierarchicalElementChildren::removeDemandElementChild(GNEDemandElement* demandElement) {
    // First check that demandElement was already inserted
    auto it = std::find(myDemandElementChildren.begin(), myDemandElementChildren.end(), demandElement);
    if (it == myDemandElementChildren.end()) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' doesn't exist in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        // first check if element is duplicated in vector
        bool singleElement = std::count(myDemandElementChildren.begin(), myDemandElementChildren.end(), demandElement) == 1;
        myDemandElementChildren.erase(it);
        // only remove it from mySortedDemandElementChildrenByType if is a single element
        if (singleElement) {
            mySortedDemandElementChildrenByType.at(demandElement->getTagProperty().getTag()).erase(demandElement);
        }
        // Check if children has to be sorted automatically
        if (myAC->getTagProperty().canAutomaticSortChildren()) {
            sortDemandElementChildren();
        }
    }
}


const std::vector<GNEDemandElement*>&
GNEHierarchicalElementChildren::getDemandElementChildren() const {
    return myDemandElementChildren;
}


const std::set<GNEDemandElement*>&
GNEHierarchicalElementChildren::getSortedDemandElementChildrenByType(SumoXMLTag tag) const {
    return mySortedDemandElementChildrenByType.at(tag);
}


void
GNEHierarchicalElementChildren::sortDemandElementChildren() {
    // by default empty
}


bool
GNEHierarchicalElementChildren::checkDemandElementChildrenOverlapping() const {
    return true;
}


GNEDemandElement*
GNEHierarchicalElementChildren::getPreviousemandElement(const GNEDemandElement* demandElement) const {
    // find demand element child
    auto it = std::find(myDemandElementChildren.begin(), myDemandElementChildren.end(), demandElement);
    // return element or null depending of iterator
    if (it == myDemandElementChildren.end()) {
        return nullptr;
    } else if (it == myDemandElementChildren.begin()) {
        return nullptr;
    } else {
        return *(it - 1);
    }
}


GNEDemandElement*
GNEHierarchicalElementChildren::getNextDemandElement(const GNEDemandElement* demandElement) const {
    // find demand element child
    auto it = std::find(myDemandElementChildren.begin(), myDemandElementChildren.end(), demandElement);
    // return element or null depending of iterator
    if (it == myDemandElementChildren.end()) {
        return nullptr;
    } else if (it == (myDemandElementChildren.end() - 1)) {
        return nullptr;
    } else {
        return *(it + 1);
    }
}


void
GNEHierarchicalElementChildren::addEdgeChild(GNEEdge* edge) {
    // Check that edge is valid and doesn't exist previously
    if (edge == nullptr) {
        throw InvalidArgument("Trying to add an empty edge child in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myEdgeChildren.push_back(edge);
    }
}


void
GNEHierarchicalElementChildren::removeEdgeChild(GNEEdge* edge) {
    // Check that edge is valid and exist previously
    if (edge == nullptr) {
        throw InvalidArgument("Trying to remove an empty edge child in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myEdgeChildren.begin(), myEdgeChildren.end(), edge) == myEdgeChildren.end()) {
        throw InvalidArgument("Trying to remove a non previously inserted edge child in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myEdgeChildren.erase(std::find(myEdgeChildren.begin(), myEdgeChildren.end(), edge));
        // update connections geometry
        myChildConnections.update();
    }
}


const std::vector<GNEEdge*>&
GNEHierarchicalElementChildren::getEdgeChildren() const {
    return myEdgeChildren;
}


void
GNEHierarchicalElementChildren::addLaneChild(GNELane* lane) {
    // Check if lane is valid
    if (lane == nullptr) {
        throw InvalidArgument("Trying to add an empty lane child in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myLaneChildren.push_back(lane);
        // update connections geometry
        myChildConnections.update();
    }
}


void
GNEHierarchicalElementChildren::removeLaneChild(GNELane* lane) {
    // Check if lane is valid
    if (lane == nullptr) {
        throw InvalidArgument("Trying to remove an empty lane child in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myLaneChildren.erase(std::find(myLaneChildren.begin(), myLaneChildren.end(), lane));
        // update connections geometry
        myChildConnections.update();
    }
}


const std::vector<GNELane*>&
GNEHierarchicalElementChildren::getLaneChildren() const {
    return myLaneChildren;
}


void
GNEHierarchicalElementChildren::addShapeChild(GNEShape* shape) {
    // Check that shape is valid and doesn't exist previously
    if (shape == nullptr) {
        throw InvalidArgument("Trying to add an empty shape child in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myShapeChildren.begin(), myShapeChildren.end(), shape) != myShapeChildren.end()) {
        throw InvalidArgument("Trying to add a duplicate shape child in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myShapeChildren.push_back(shape);
        // update connections geometry
        myChildConnections.update();
    }
}


void
GNEHierarchicalElementChildren::removeShapeChild(GNEShape* shape) {
    // Check that shape is valid and exist previously
    if (shape == nullptr) {
        throw InvalidArgument("Trying to remove an empty shape child in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myShapeChildren.begin(), myShapeChildren.end(), shape) == myShapeChildren.end()) {
        throw InvalidArgument("Trying to remove a non previously inserted shape child in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myShapeChildren.erase(std::find(myShapeChildren.begin(), myShapeChildren.end(), shape));
        // update connections geometry
        myChildConnections.update();
    }
}


const std::vector<GNEShape*>&
GNEHierarchicalElementChildren::getShapeChildren() const {
    return myShapeChildren;
}


void
GNEHierarchicalElementChildren::updateAdditionalParent() {
    // by default nothing to do
}


void
GNEHierarchicalElementChildren::updateDemandElementParent() {
    // by default nothing to do
}


void
GNEHierarchicalElementChildren::changeEdgeChildren(GNEAdditional* elementChild, const std::string& newEdgeIDs) {
    // remove demandElement of edge children
    for (const auto& i : myEdgeChildren) {
        i->removeAdditionalParent(elementChild);
    }
    // obtain new child edges (note: it can be empty)
    myEdgeChildren = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(elementChild->getViewNet()->getNet(), newEdgeIDs);
    // add demandElement into edge parents
    for (const auto& i : myEdgeChildren) {
        i->addAdditionalParent(elementChild);
    }
    // update connections geometry
    myChildConnections.update();
}


void
GNEHierarchicalElementChildren::changeLaneChildren(GNEAdditional* elementChild, const std::string& newLaneIDs) {
    // remove demandElement of lane children
    for (const auto& i : myLaneChildren) {
        i->removeAdditionalParent(elementChild);
    }
    // obtain new child lanes (note: it can be empty)
    myLaneChildren = GNEAttributeCarrier::parse<std::vector<GNELane*> >(elementChild->getViewNet()->getNet(), newLaneIDs);
    // add demandElement into lane parents
    for (const auto& i : myLaneChildren) {
        i->addAdditionalParent(elementChild);
    }
    // update connections geometry
    myChildConnections.update();
}

// ---------------------------------------------------------------------------
// GNEHierarchicalElementChildren::ChildConnections - methods
// ---------------------------------------------------------------------------

GNEHierarchicalElementChildren::ChildConnections::ConnectionGeometry::ConnectionGeometry() :
    lane(nullptr),
    pos(Position::INVALID),
    rot(0) {
}


GNEHierarchicalElementChildren::ChildConnections::ConnectionGeometry::ConnectionGeometry(GNELane* _lane, Position _pos, double _rot) :
    lane(_lane),
    pos(_pos),
    rot(_rot) {
}


GNEHierarchicalElementChildren::ChildConnections::ChildConnections(GNEHierarchicalElementChildren* hierarchicalElement) :
    myHierarchicalElement(hierarchicalElement) {}


void
GNEHierarchicalElementChildren::ChildConnections::update() {
    // first clear connection positions
    connectionPositions.clear();
    symbolsPositionAndRotation.clear();
    // calculate position and rotation of every simbol for every edge
    for (const auto& i : myHierarchicalElement->myEdgeChildren) {
        for (auto j : i->getLanes()) {
            Position pos;
            double rot;
            // set position and lenght depending of shape's lengt
            if (j->getGeometry().shape.length() - 6 > 0) {
                pos = j->getGeometry().shape.positionAtOffset(j->getGeometry().shape.length() - 6);
                rot = j->getGeometry().shape.rotationDegreeAtOffset(j->getGeometry().shape.length() - 6);
            } else {
                pos = j->getGeometry().shape.positionAtOffset(j->getGeometry().shape.length());
                rot = j->getGeometry().shape.rotationDegreeAtOffset(j->getGeometry().shape.length());
            }
            symbolsPositionAndRotation.push_back(ConnectionGeometry(j, pos, rot));
        }
    }
    // calculate position and rotation of every symbol for every lane
    for (const auto& i : myHierarchicalElement->myLaneChildren) {
        Position pos;
        double rot;
        // set position and lenght depending of shape's lengt
        if (i->getGeometry().shape.length() - 6 > 0) {
            pos = i->getGeometry().shape.positionAtOffset(i->getGeometry().shape.length() - 6);
            rot = i->getGeometry().shape.rotationDegreeAtOffset(i->getGeometry().shape.length() - 6);
        } else {
            pos = i->getGeometry().shape.positionAtOffset(i->getGeometry().shape.length());
            rot = i->getGeometry().shape.rotationDegreeAtOffset(i->getGeometry().shape.length());
        }
        symbolsPositionAndRotation.push_back(ConnectionGeometry(i, pos, rot));
    }
    // calculate position for every additional child
    for (const auto& i : myHierarchicalElement->myAdditionalChildren) {
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
GNEHierarchicalElementChildren::ChildConnections::draw(const GUIVisualizationSettings& s, const GUIGlObjectType parentType) const {
    // first check if connections can be drawn
    if (!s.drawForSelecting) {
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
