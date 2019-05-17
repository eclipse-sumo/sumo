/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEHierarchicalElementChilds.cpp
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

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/additionals/GNEShape.h>
#include <netedit/demandelements/GNEDemandElement.h>
#include <netedit/frames/GNESelectorFrame.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/netelements/GNELane.h>
#include <utils/common/StringTokenizer.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/options/OptionsCont.h>

#include "GNEHierarchicalElementChilds.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEHierarchicalElementChilds::GNEHierarchicalElementChilds(GNEAttributeCarrier* AC,
        const std::vector<GNEEdge*>& edgeChilds,
        const std::vector<GNELane*>& laneChilds,
        const std::vector<GNEShape*>& shapeChilds,
        const std::vector<GNEAdditional*>& additionalChilds,
        const std::vector<GNEDemandElement*>& demandElementChilds) :
    myChildConnections(this),
    myEdgeChilds(edgeChilds),
    myLaneChilds(laneChilds),
    myShapeChilds(shapeChilds),
    myAdditionalChilds(additionalChilds),
    myDemandElementChilds(demandElementChilds),
    myAC(AC) {
    // fill SortedDemandElementChildsByType with all demand element tags (it's needed because getSortedDemandElementChildsByType(...) function is constant
    auto listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNEAttributeCarrier::TagType::TAGTYPE_DEMANDELEMENT, false);
    for (const auto &i : listOfTags) {
        mySortedDemandElementChildsByType[i];
    }
}


GNEHierarchicalElementChilds::~GNEHierarchicalElementChilds() {}


const Position &
GNEHierarchicalElementChilds::getChildPosition(const GNELane* lane) {
    for (const auto &i : myChildConnections.symbolsPositionAndRotation) {
        if (i.lane == lane) {
            return i.pos;
        }
    }
    throw ProcessError("Lane doesn't exist");
}


double 
GNEHierarchicalElementChilds::getChildRotation(const GNELane* lane) {
    for (const auto &i : myChildConnections.symbolsPositionAndRotation) {
        if (i.lane == lane) {
            return i.rot;
        }
    }
    throw ProcessError("Lane doesn't exist");
}


void
GNEHierarchicalElementChilds::updateChildConnections() {
    myChildConnections.update();
}


void 
GNEHierarchicalElementChilds::drawChildConnections(GUIGlObjectType GLTypeParent) const {
    myChildConnections.draw(GLTypeParent);
}


void
GNEHierarchicalElementChilds::addAdditionalChild(GNEAdditional* additional) {
    // Check if additional is valid
    if (additional == nullptr) {
        throw InvalidArgument("Trying to add an empty additional child in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        // add it in additional childs container
        myAdditionalChilds.push_back(additional);
        // Check if childs has to be sorted automatically
        if (myAC->getTagProperty().canAutomaticSortChilds()) {
            sortAdditionalChilds();
        }
        // update additional parent after add additional (note: by default non-implemented)
        updateAdditionalParent();
    }
}


void
GNEHierarchicalElementChilds::removeAdditionalChild(GNEAdditional* additional) {
    // First check that additional was already inserted
    auto it = std::find(myAdditionalChilds.begin(), myAdditionalChilds.end(), additional);
    if (it == myAdditionalChilds.end()) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' doesn't exist in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myAdditionalChilds.erase(it);
        // Check if childs has to be sorted automatically
        if (myAC->getTagProperty().canAutomaticSortChilds()) {
            sortAdditionalChilds();
        }
        // update additional parent after add additional (note: by default non-implemented)
        updateAdditionalParent();
    }
}


const std::vector<GNEAdditional*>&
GNEHierarchicalElementChilds::getAdditionalChilds() const {
    return myAdditionalChilds;
}


void
GNEHierarchicalElementChilds::sortAdditionalChilds() {
    if (myAC->getTagProperty().getTag() == SUMO_TAG_E3DETECTOR) {
        // we need to sort Entry/Exits due additional.xds model
        std::vector<GNEAdditional*> sortedEntryExits;
        // obtain all entrys
        for (auto i : myAdditionalChilds) {
            if (i->getTagProperty().getTag() == SUMO_TAG_DET_ENTRY) {
                sortedEntryExits.push_back(i);
            }
        }
        // obtain all exits
        for (auto i : myAdditionalChilds) {
            if (i->getTagProperty().getTag() == SUMO_TAG_DET_EXIT) {
                sortedEntryExits.push_back(i);
            }
        }
        // change myAdditionalChilds for sortedEntryExits
        if (sortedEntryExits.size() == myAdditionalChilds.size()) {
            myAdditionalChilds = sortedEntryExits;
        } else {
            throw ProcessError("Some additional childs were lost during sorting");
        }
    } else if (myAC->getTagProperty().getTag() == SUMO_TAG_TAZ) {
        // we need to sort Entry/Exits due additional.xds model
        std::vector<GNEAdditional*> sortedTAZSourceSink;
        // obtain all TAZSources
        for (auto i : myAdditionalChilds) {
            if (i->getTagProperty().getTag() == SUMO_TAG_TAZSOURCE) {
                sortedTAZSourceSink.push_back(i);
            }
        }
        // obtain all TAZSinks
        for (auto i : myAdditionalChilds) {
            if (i->getTagProperty().getTag() == SUMO_TAG_TAZSINK) {
                sortedTAZSourceSink.push_back(i);
            }
        }
        // change myAdditionalChilds for sortedEntryExits
        if (sortedTAZSourceSink.size() == myAdditionalChilds.size()) {
            myAdditionalChilds = sortedTAZSourceSink;
        } else {
            throw ProcessError("Some additional childs were lost during sorting");
        }
    } else {
        // declare a vector to keep sorted childs
        std::vector<std::pair<std::pair<double, double>, GNEAdditional*> > sortedChilds;
        // iterate over additional childs
        for (auto i : myAdditionalChilds) {
            sortedChilds.push_back(std::make_pair(std::make_pair(0., 0.), i));
            // set begin/start attribute
            if (i->getTagProperty().hasAttribute(SUMO_ATTR_TIME) && GNEAttributeCarrier::canParse<double>(i->getAttribute(SUMO_ATTR_TIME))) {
                sortedChilds.back().first.first = GNEAttributeCarrier::parse<double>(i->getAttribute(SUMO_ATTR_TIME));
            } else if (i->getTagProperty().hasAttribute(SUMO_ATTR_BEGIN) && GNEAttributeCarrier::canParse<double>(i->getAttribute(SUMO_ATTR_BEGIN))) {
                sortedChilds.back().first.first = GNEAttributeCarrier::parse<double>(i->getAttribute(SUMO_ATTR_BEGIN));
            }
            // set end attribute
            if (i->getTagProperty().hasAttribute(SUMO_ATTR_END) && GNEAttributeCarrier::canParse<double>(i->getAttribute(SUMO_ATTR_END))) {
                sortedChilds.back().first.second = GNEAttributeCarrier::parse<double>(i->getAttribute(SUMO_ATTR_END));
            } else {
                sortedChilds.back().first.second = sortedChilds.back().first.first;
            }
        }
        // sort childs
        std::sort(sortedChilds.begin(), sortedChilds.end());
        // make sure that number of sorted childs is the same as the additional childs
        if (sortedChilds.size() == myAdditionalChilds.size()) {
            myAdditionalChilds.clear();
            for (auto i : sortedChilds) {
                myAdditionalChilds.push_back(i.second);
            }
        } else {
            throw ProcessError("Some additional childs were lost during sorting");
        }
    }
}


bool
GNEHierarchicalElementChilds::checkAdditionalChildsOverlapping() const {
    // declare a vector to keep sorted childs
    std::vector<std::pair<std::pair<double, double>, GNEAdditional*> > sortedChilds;
    // iterate over additional childs
    for (auto i : myAdditionalChilds) {
        sortedChilds.push_back(std::make_pair(std::make_pair(0., 0.), i));
        // set begin/start attribute
        if (i->getTagProperty().hasAttribute(SUMO_ATTR_TIME) && GNEAttributeCarrier::canParse<double>(i->getAttribute(SUMO_ATTR_TIME))) {
            sortedChilds.back().first.first = GNEAttributeCarrier::parse<double>(i->getAttribute(SUMO_ATTR_TIME));
        } else if (i->getTagProperty().hasAttribute(SUMO_ATTR_BEGIN) && GNEAttributeCarrier::canParse<double>(i->getAttribute(SUMO_ATTR_BEGIN))) {
            sortedChilds.back().first.first = GNEAttributeCarrier::parse<double>(i->getAttribute(SUMO_ATTR_BEGIN));
        }
        // set end attribute
        if (i->getTagProperty().hasAttribute(SUMO_ATTR_END) && GNEAttributeCarrier::canParse<double>(i->getAttribute(SUMO_ATTR_END))) {
            sortedChilds.back().first.second = GNEAttributeCarrier::parse<double>(i->getAttribute(SUMO_ATTR_END));
        } else {
            sortedChilds.back().first.second = sortedChilds.back().first.first;
        }
    }
    // sort childs
    std::sort(sortedChilds.begin(), sortedChilds.end());
    // make sure that number of sorted childs is the same as the additional childs
    if (sortedChilds.size() == myAdditionalChilds.size()) {
        if (sortedChilds.size() <= 1) {
            return true;
        } else {
            // check overlapping
            for (int i = 0; i < (int)sortedChilds.size() - 1; i++) {
                if (sortedChilds.at(i).first.second > sortedChilds.at(i + 1).first.first) {
                    return false;
                }
            }
        }
        return true;
    } else {
        throw ProcessError("Some additional childs were lost during sorting");
    }
}


void
GNEHierarchicalElementChilds::addDemandElementChild(GNEDemandElement* demandElement) {
    // Check if demand element is valid
    if (demandElement == nullptr) {
        throw InvalidArgument("Trying to add an empty demand element child in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        // add it in demandElement child container
        myDemandElementChilds.push_back(demandElement);
        // add it also in SortedDemandElementChildsByType container
        mySortedDemandElementChildsByType.at(demandElement->getTagProperty().getTag()).insert(demandElement);
        // Check if childs has to be sorted automatically
        if (myAC->getTagProperty().canAutomaticSortChilds()) {
            sortDemandElementChilds();
        }
    }
}


void
GNEHierarchicalElementChilds::removeDemandElementChild(GNEDemandElement* demandElement) {
    // First check that demandElement was already inserted
    auto it = std::find(myDemandElementChilds.begin(), myDemandElementChilds.end(), demandElement);
    if (it == myDemandElementChilds.end()) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' doesn't exist in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        // first check if element is duplicated in vector
        bool singleElement = std::count(myDemandElementChilds.begin(), myDemandElementChilds.end(), demandElement) == 1;
        myDemandElementChilds.erase(it);
        // only remove it from mySortedDemandElementChildsByType if is a single element
        if (singleElement) {
            mySortedDemandElementChildsByType.at(demandElement->getTagProperty().getTag()).erase(demandElement);
        }
        // Check if childs has to be sorted automatically
        if (myAC->getTagProperty().canAutomaticSortChilds()) {
            sortDemandElementChilds();
        }
    }
}


const std::vector<GNEDemandElement*>&
GNEHierarchicalElementChilds::getDemandElementChilds() const {
    return myDemandElementChilds;
}


const std::set<GNEDemandElement*>& 
GNEHierarchicalElementChilds::getSortedDemandElementChildsByType(SumoXMLTag tag) const {
    return mySortedDemandElementChildsByType.at(tag);
}


void
GNEHierarchicalElementChilds::sortDemandElementChilds() {
    // by default empty
}


bool
GNEHierarchicalElementChilds::checkDemandElementChildsOverlapping() const {
    return true;
}


void
GNEHierarchicalElementChilds::addEdgeChild(GNEEdge* edge) {
    // Check that edge is valid and doesn't exist previously
    if (edge == nullptr) {
        throw InvalidArgument("Trying to add an empty edge child in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myEdgeChilds.push_back(edge);
    }
}


void
GNEHierarchicalElementChilds::removeEdgeChild(GNEEdge* edge) {
    // Check that edge is valid and exist previously
    if (edge == nullptr) {
        throw InvalidArgument("Trying to remove an empty edge child in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myEdgeChilds.begin(), myEdgeChilds.end(), edge) == myEdgeChilds.end()) {
        throw InvalidArgument("Trying to remove a non previously inserted edge child in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myEdgeChilds.erase(std::find(myEdgeChilds.begin(), myEdgeChilds.end(), edge));
        // update connections geometry
        myChildConnections.update();
    }
}


const std::vector<GNEEdge*>&
GNEHierarchicalElementChilds::getEdgeChilds() const {
    return myEdgeChilds;
}


void
GNEHierarchicalElementChilds::addLaneChild(GNELane* lane) {
    // Check if lane is valid
    if (lane == nullptr) {
        throw InvalidArgument("Trying to add an empty lane child in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myLaneChilds.push_back(lane);
        // update connections geometry
        myChildConnections.update();
    }
}


void
GNEHierarchicalElementChilds::removeLaneChild(GNELane* lane) {
    // Check if lane is valid
    if (lane == nullptr) {
        throw InvalidArgument("Trying to remove an empty lane child in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myLaneChilds.erase(std::find(myLaneChilds.begin(), myLaneChilds.end(), lane));
        // update connections geometry
        myChildConnections.update();
    }
}


const std::vector<GNELane*>&
GNEHierarchicalElementChilds::getLaneChilds() const {
    return myLaneChilds;
}


void
GNEHierarchicalElementChilds::addShapeChild(GNEShape* shape) {
    // Check that shape is valid and doesn't exist previously
    if (shape == nullptr) {
        throw InvalidArgument("Trying to add an empty shape child in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myShapeChilds.begin(), myShapeChilds.end(), shape) != myShapeChilds.end()) {
        throw InvalidArgument("Trying to add a duplicate shape child in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myShapeChilds.push_back(shape);
        // update connections geometry
        myChildConnections.update();
    }
}


void
GNEHierarchicalElementChilds::removeShapeChild(GNEShape* shape) {
    // Check that shape is valid and exist previously
    if (shape == nullptr) {
        throw InvalidArgument("Trying to remove an empty shape child in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myShapeChilds.begin(), myShapeChilds.end(), shape) == myShapeChilds.end()) {
        throw InvalidArgument("Trying to remove a non previously inserted shape child in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myShapeChilds.erase(std::find(myShapeChilds.begin(), myShapeChilds.end(), shape));
        // update connections geometry
        myChildConnections.update();
    }
}


const std::vector<GNEShape*>&
GNEHierarchicalElementChilds::getShapeChilds() const {
    return myShapeChilds;
}


void
GNEHierarchicalElementChilds::updateAdditionalParent() {
    // by default nothing to do
}


void
GNEHierarchicalElementChilds::updateDemandElementParent() {
    // by default nothing to do
}


void
GNEHierarchicalElementChilds::changeEdgeChilds(GNEAdditional* elementChild, const std::string& newEdgeIDs) {
    // remove demandElement of edge childs
    for (const auto& i : myEdgeChilds) {
        i->removeAdditionalParent(elementChild);
    }
    // obtain new child edges (note: it can be empty)
    myEdgeChilds = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(elementChild->getViewNet()->getNet(), newEdgeIDs);
    // add demandElement into edge parents
    for (const auto& i : myEdgeChilds) {
        i->addAdditionalParent(elementChild);
    }
    // update connections geometry
    myChildConnections.update();
}


void
GNEHierarchicalElementChilds::changeLaneChilds(GNEAdditional* elementChild, const std::string& newLaneIDs) {
    // remove demandElement of lane childs
    for (const auto& i : myLaneChilds) {
        i->removeAdditionalParent(elementChild);
    }
    // obtain new child lanes (note: it can be empty)
    myLaneChilds = GNEAttributeCarrier::parse<std::vector<GNELane*> >(elementChild->getViewNet()->getNet(), newLaneIDs);
    // add demandElement into lane parents
    for (const auto& i : myLaneChilds) {
        i->addAdditionalParent(elementChild);
    }
    // update connections geometry
    myChildConnections.update();
}

// ---------------------------------------------------------------------------
// GNEHierarchicalElementChilds::ChildConnections - methods
// ---------------------------------------------------------------------------

GNEHierarchicalElementChilds::ChildConnections::ConnectionGeometry::ConnectionGeometry() :
    lane(nullptr), 
    pos(Position::INVALID), 
    rot(0) {
}


GNEHierarchicalElementChilds::ChildConnections::ConnectionGeometry::ConnectionGeometry(GNELane* _lane, Position _pos, double _rot) :
    lane(_lane), 
    pos(_pos), 
    rot(_rot) {
}


GNEHierarchicalElementChilds::ChildConnections::ChildConnections(GNEHierarchicalElementChilds* hierarchicalElement) :
    myHierarchicalElement(hierarchicalElement) {}


void
GNEHierarchicalElementChilds::ChildConnections::update() {
    // first clear connection positions
    connectionPositions.clear();
    symbolsPositionAndRotation.clear();
    // calculate position and rotation of every simbol for every edge
    for (const auto& i : myHierarchicalElement->myEdgeChilds) {
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
    for (const auto& i : myHierarchicalElement->myLaneChilds) {
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
    for (const auto& i : myHierarchicalElement->myAdditionalChilds) {
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
    // calculate geometry for connections between parent and childs
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
GNEHierarchicalElementChilds::ChildConnections::draw(GUIGlObjectType parentType) const {
    // Iterate over myConnectionPositions
    for (const auto& i : connectionPositions) {
        // Add a draw matrix
        glPushMatrix();
        // traslate in the Z axis
        glTranslated(0, 0, parentType - 0.01);
        // Set color of the base
        GLHelper::setColor(RGBColor(255, 235, 0));
        for (auto j = i.begin(); (j + 1) != i.end(); j++) {
            // Draw Lines
            GLHelper::drawLine((*j), (*(j + 1)));
        }
        // Pop draw matrix
        glPopMatrix();
    }
}

/****************************************************************************/
