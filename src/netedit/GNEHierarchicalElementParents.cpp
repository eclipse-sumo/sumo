/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEHierarchicalElementParents.cpp
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
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/additionals/GNEShape.h>
#include <netedit/demandelements/GNEDemandElement.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEHierarchicalElementParents.h"

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEHierarchicalElementParents - methods
// ---------------------------------------------------------------------------

GNEHierarchicalElementParents::GNEHierarchicalElementParents(GNEAttributeCarrier* AC,
        const std::vector<GNEEdge*>& edgeParents,
        const std::vector<GNELane*>& laneParents,
        const std::vector<GNEShape*>& shapeParents,
        const std::vector<GNEAdditional*>& additionalParents,
        const std::vector<GNEDemandElement*>& demandElementParents) :
    myParentConnections(this),
    myEdgeParents(edgeParents),
    myLaneParents(laneParents),
    myShapeParents(shapeParents),
    myAdditionalParents(additionalParents),
    myDemandElementParents(demandElementParents),
    myAC(AC) {
}


GNEHierarchicalElementParents::~GNEHierarchicalElementParents() {}


void
GNEHierarchicalElementParents::addAdditionalParent(GNEAdditional* additional) {
    // First check that additional wasn't already inserted
    if (std::find(myAdditionalParents.begin(), myAdditionalParents.end(), additional) != myAdditionalParents.end()) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' was already inserted in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myAdditionalParents.push_back(additional);
    }
}


void
GNEHierarchicalElementParents::removeAdditionalParent(GNEAdditional* additional) {
    // First check that additional was already inserted
    auto it = std::find(myAdditionalParents.begin(), myAdditionalParents.end(), additional);
    if (it == myAdditionalParents.end()) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' doesn't exist in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myAdditionalParents.erase(it);
    }
}


const std::vector<GNEAdditional*>&
GNEHierarchicalElementParents::getAdditionalParents() const {
    return myAdditionalParents;
}


void
GNEHierarchicalElementParents::addDemandElementParent(GNEDemandElement* demandElement) {
    // First check that demandElement wasn't already inserted
    if (std::find(myDemandElementParents.begin(), myDemandElementParents.end(), demandElement) != myDemandElementParents.end()) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' was already inserted in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myDemandElementParents.push_back(demandElement);
    }
}


void
GNEHierarchicalElementParents::removeDemandElementParent(GNEDemandElement* demandElement) {
    // First check that demandElement was already inserted
    auto it = std::find(myDemandElementParents.begin(), myDemandElementParents.end(), demandElement);
    if (it == myDemandElementParents.end()) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' doesn't exist in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myDemandElementParents.erase(it);
    }
}


const std::vector<GNEDemandElement*>&
GNEHierarchicalElementParents::getDemandElementParents() const {
    return myDemandElementParents;
}


void
GNEHierarchicalElementParents::addEdgeParent(GNEEdge* edge) {
    // Check that edge is valid and doesn't exist previously
    if (edge == nullptr) {
        throw InvalidArgument("Trying to add an empty " + toString(SUMO_TAG_EDGE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myEdgeParents.begin(), myEdgeParents.end(), edge) != myEdgeParents.end()) {
        throw InvalidArgument("Trying to add a duplicate " + toString(SUMO_TAG_EDGE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myEdgeParents.push_back(edge);
    }
}


void
GNEHierarchicalElementParents::removeEdgeParent(GNEEdge* edge) {
    // Check that edge is valid and exist previously
    if (edge == nullptr) {
        throw InvalidArgument("Trying to remove an empty " + toString(SUMO_TAG_EDGE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        auto it = std::find(myEdgeParents.begin(), myEdgeParents.end(), edge);
        if (it == myEdgeParents.end()) {
            throw InvalidArgument("Trying to remove a non previously inserted " + toString(SUMO_TAG_EDGE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
        } else {
            myEdgeParents.erase(it);
        }
    }
}


const std::vector<GNEEdge*>&
GNEHierarchicalElementParents::getEdgeParents() const {
    return myEdgeParents;
}


void
GNEHierarchicalElementParents::addLaneParent(GNELane* lane) {
    // Check that lane is valid and doesn't exist previously
    if (lane == nullptr) {
        throw InvalidArgument("Trying to add an empty " + toString(SUMO_TAG_EDGE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myLaneParents.begin(), myLaneParents.end(), lane) != myLaneParents.end()) {
        throw InvalidArgument("Trying to add a duplicate " + toString(SUMO_TAG_EDGE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myLaneParents.push_back(lane);
    }
}


void
GNEHierarchicalElementParents::removeLaneParent(GNELane* lane) {
    // Check that lane is valid and exist previously
    if (lane == nullptr) {
        throw InvalidArgument("Trying to remove an empty " + toString(SUMO_TAG_EDGE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myLaneParents.begin(), myLaneParents.end(), lane) == myLaneParents.end()) {
        throw InvalidArgument("Trying to remove a non previously inserted " + toString(SUMO_TAG_EDGE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myLaneParents.erase(std::find(myLaneParents.begin(), myLaneParents.end(), lane));
    }
}


const std::vector<GNELane*>&
GNEHierarchicalElementParents::getLaneParents() const {
    return myLaneParents;
}


void
GNEHierarchicalElementParents::addShapeParent(GNEShape* shape) {
    // Check that shape is valid and doesn't exist previously
    if (shape == nullptr) {
        throw InvalidArgument("Trying to add an empty " + toString(SUMO_TAG_EDGE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myShapeParents.begin(), myShapeParents.end(), shape) != myShapeParents.end()) {
        throw InvalidArgument("Trying to add a duplicate " + toString(SUMO_TAG_EDGE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myShapeParents.push_back(shape);
    }
}


void
GNEHierarchicalElementParents::removeShapeParent(GNEShape* shape) {
    // Check that shape is valid and exist previously
    if (shape == nullptr) {
        throw InvalidArgument("Trying to remove an empty " + toString(SUMO_TAG_EDGE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myShapeParents.begin(), myShapeParents.end(), shape) == myShapeParents.end()) {
        throw InvalidArgument("Trying to remove a non previously inserted " + toString(SUMO_TAG_EDGE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myShapeParents.erase(std::find(myShapeParents.begin(), myShapeParents.end(), shape));
    }
}


const std::vector<GNEShape*>&
GNEHierarchicalElementParents::getShapeParents() const {
    return myShapeParents;
}

// ---------------------------------------------------------------------------
// GNEHierarchicalElementParents::ParentConnections - methods
// ---------------------------------------------------------------------------

GNEHierarchicalElementParents::ParentConnections::ParentConnections(GNEHierarchicalElementParents* hierarchicalElement) :
    myHierarchicalElement(hierarchicalElement) {}


void
GNEHierarchicalElementParents::ParentConnections::update() {
    // first clear connection positions
    connectionPositions.clear();
    symbolsPositionAndRotation.clear();

    // calculate position and rotation of every simbol for every edge
    for (auto i : myHierarchicalElement->myEdgeParents) {
        for (auto j : i->getLanes()) {
            std::pair<Position, double> posRot;
            // set position and lenght depending of shape's lengt
            if (j->getGeometry().shape.length() - 6 > 0) {
                posRot.first = j->getGeometry().shape.positionAtOffset(j->getGeometry().shape.length() - 6);
                posRot.second = j->getGeometry().shape.rotationDegreeAtOffset(j->getGeometry().shape.length() - 6);
            } else {
                posRot.first = j->getGeometry().shape.positionAtOffset(j->getGeometry().shape.length());
                posRot.second = j->getGeometry().shape.rotationDegreeAtOffset(j->getGeometry().shape.length());
            }
            symbolsPositionAndRotation.push_back(posRot);
        }
    }

    // calculate position and rotation of every symbol for every lane
    for (auto i : myHierarchicalElement->myLaneParents) {
        std::pair<Position, double> posRot;
        // set position and lenght depending of shape's lengt
        if (i->getGeometry().shape.length() - 6 > 0) {
            posRot.first = i->getGeometry().shape.positionAtOffset(i->getGeometry().shape.length() - 6);
            posRot.second = i->getGeometry().shape.rotationDegreeAtOffset(i->getGeometry().shape.length() - 6);
        } else {
            posRot.first = i->getGeometry().shape.positionAtOffset(i->getGeometry().shape.length());
            posRot.second = i->getGeometry().shape.rotationDegreeAtOffset(i->getGeometry().shape.length());
        }
        symbolsPositionAndRotation.push_back(posRot);
    }

    // calculate position for every additional parent
    for (auto i : myHierarchicalElement->myAdditionalParents) {
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

    // calculate geometry for connections between parent and parents
    for (auto i : symbolsPositionAndRotation) {
        std::vector<Position> posConnection;
        double A = std::abs(i.first.x() - myHierarchicalElement->getPositionInView().x());
        double B = std::abs(i.first.y() - myHierarchicalElement->getPositionInView().y());
        // Set positions of connection's vertex. Connection is build from Entry to E3
        posConnection.push_back(i.first);
        if (myHierarchicalElement->getPositionInView().x() > i.first.x()) {
            if (myHierarchicalElement->getPositionInView().y() > i.first.y()) {
                posConnection.push_back(Position(i.first.x() + A, i.first.y()));
            } else {
                posConnection.push_back(Position(i.first.x(), i.first.y() - B));
            }
        } else {
            if (myHierarchicalElement->getPositionInView().y() > i.first.y()) {
                posConnection.push_back(Position(i.first.x(), i.first.y() + B));
            } else {
                posConnection.push_back(Position(i.first.x() - A, i.first.y()));
            }
        }
        posConnection.push_back(myHierarchicalElement->getPositionInView());
        connectionPositions.push_back(posConnection);
    }
}


void
GNEHierarchicalElementParents::ParentConnections::draw(const GUIVisualizationSettings& /* s */, const GUIGlObjectType parentType) const {
    // Iterate over myConnectionPositions
    for (const PositionVector& i : connectionPositions) {
        // Add a draw matrix
        glPushMatrix();
        // translate in the Z axis
        glTranslated(0, 0, parentType - 0.01);
        // Set color of the base
        GLHelper::setColor(GUIVisualizationColorSettings::childConnections);
        // iterate over connections
        for (PositionVector::const_iterator j = i.begin(); (j + 1) != i.end(); ++j) {
            // Draw Lines
            GLHelper::drawLine((*j), (*(j + 1)));
        }
        // Pop draw matrix
        glPopMatrix();
    }
}


// ---------------------------------------------------------------------------
// GNEHierarchicalElementParents - protected methods
// ---------------------------------------------------------------------------
void
GNEHierarchicalElementParents::changeEdgeParents(GNEShape* elementChild, const std::string& newEdgeIDs) {
    // remove additional of edge parents
    for (const auto& i : myEdgeParents) {
        i->removeShapeChild(elementChild);
    }
    // obtain new parent edges
    myEdgeParents = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(elementChild->getNet(), newEdgeIDs);
    // check that lane parets aren't empty
    if (myEdgeParents.empty()) {
        throw InvalidArgument("New list of edge parents cannot be empty");
    } else {
        // add additional into edge parents
        for (const auto& i : myEdgeParents) {
            i->addShapeChild(elementChild);
        }
    }
}


void
GNEHierarchicalElementParents::changeEdgeParents(GNEAdditional* elementChild, const std::string& newEdgeIDs) {
    // remove additional of edge parents
    for (const auto& i : myEdgeParents) {
        i->removeAdditionalChild(elementChild);
    }
    // obtain new parent edges
    myEdgeParents = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(elementChild->getViewNet()->getNet(), newEdgeIDs);
    // check that lane parets aren't empty
    if (myEdgeParents.empty()) {
        throw InvalidArgument("New list of edge parents cannot be empty");
    } else {
        // add additional into edge parents
        for (const auto& i : myEdgeParents) {
            i->addAdditionalChild(elementChild);
        }
    }
}


void
GNEHierarchicalElementParents::changeEdgeParents(GNEDemandElement* elementChild, const std::string& newEdgeIDs) {
    // remove demandElement of edge parents
    for (const auto& i : myEdgeParents) {
        i->removeDemandElementChild(elementChild);
    }
    // obtain new parent edges
    myEdgeParents = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(elementChild->getViewNet()->getNet(), newEdgeIDs);
    // check that lane parets aren't empty
    if (myEdgeParents.empty()) {
        throw InvalidArgument("New list of edge parents cannot be empty");
    } else {
        // add demandElement into edge parents
        for (const auto& i : myEdgeParents) {
            i->addDemandElementChild(elementChild);
        }
    }
}


void
GNEHierarchicalElementParents::changeEdgeParents(GNEDemandElement* elementChild, const std::vector<GNEEdge*>& newEdges) {
    // remove demandElement of edge parents
    for (const auto& i : myEdgeParents) {
        i->removeDemandElementChild(elementChild);
    }
    // set new edges
    myEdgeParents = newEdges;
    // check that lane parets aren't empty
    if (myEdgeParents.empty()) {
        throw InvalidArgument("New list of edge parents cannot be empty");
    } else {
        // add demandElement into edge parents
        for (const auto& i : myEdgeParents) {
            i->addDemandElementChild(elementChild);
        }
    }
}


void
GNEHierarchicalElementParents::changeLaneParents(GNEAdditional* elementChild, const std::string& newLaneIDs) {
    // remove additional of edge parents
    for (const auto& i : myLaneParents) {
        i->removeAdditionalChild(elementChild);
    }
    // obtain new parent edges
    myLaneParents = GNEAttributeCarrier::parse<std::vector<GNELane*> >(elementChild->getViewNet()->getNet(), newLaneIDs);
    // check that lane parets aren't empty
    if (myLaneParents.empty()) {
        throw InvalidArgument("New list of lane parents cannot be empty");
    } else {
        // add additional into edge parents
        for (const auto& i : myLaneParents) {
            i->addAdditionalChild(elementChild);
        }
    }
}


void
GNEHierarchicalElementParents::changeLaneParents(GNEDemandElement* elementChild, const std::string& newLaneIDs) {
    // remove demandElement of edge parents
    for (const auto& i : myLaneParents) {
        i->removeDemandElementChild(elementChild);
    }
    // obtain new parent edges
    myLaneParents = GNEAttributeCarrier::parse<std::vector<GNELane*> >(elementChild->getViewNet()->getNet(), newLaneIDs);
    // check that lane parets aren't empty
    if (myLaneParents.empty()) {
        throw InvalidArgument("New list of lane parents cannot be empty");
    } else {
        // add demandElement into edge parents
        for (const auto& i : myLaneParents) {
            i->addDemandElementChild(elementChild);
        }
    }
}


void
GNEHierarchicalElementParents::changeLaneParents(GNEShape* elementChild, const std::string& newLaneIDs) {
    // remove demandElement of edge parents
    for (const auto& i : myLaneParents) {
        i->removeShapeChild(elementChild);
    }
    // obtain new parent edges
    myLaneParents = GNEAttributeCarrier::parse<std::vector<GNELane*> >(elementChild->getNet(), newLaneIDs);
    // check that lane parets aren't empty
    if (myLaneParents.empty()) {
        throw InvalidArgument("New list of lane parents cannot be empty");
    } else {
        // add demandElement into edge parents
        for (const auto& i : myLaneParents) {
            i->addShapeChild(elementChild);
        }
    }
}


void
GNEHierarchicalElementParents::changeAdditionalParent(GNEShape* shapeTobeChanged, const std::string& newAdditionalParentID, int additionalParentIndex) {
    if ((int)myAdditionalParents.size() < additionalParentIndex) {
        throw InvalidArgument(myAC->getTagStr() + " with ID '" + myAC->getID() + "' doesn't have " + toString(additionalParentIndex) + " additional parents");
    } else {
        // remove additional of the children of parent additional
        myAdditionalParents.at(additionalParentIndex)->removeShapeChild(shapeTobeChanged);
        // set new additional parent
        myAdditionalParents.at(additionalParentIndex) = shapeTobeChanged->getNet()->retrieveAdditional(myAdditionalParents.at(additionalParentIndex)->getTagProperty().getTag(), newAdditionalParentID);
        // add additional int the children of parent additional
        myAdditionalParents.at(additionalParentIndex)->addShapeChild(shapeTobeChanged);
        // update geometry after inserting
        shapeTobeChanged->updateGeometry();
    }
}


void
GNEHierarchicalElementParents::changeAdditionalParent(GNEAdditional* additionalTobeChanged, const std::string& newAdditionalParentID, int additionalParentIndex) {
    if ((int)myAdditionalParents.size() < additionalParentIndex) {
        throw InvalidArgument(myAC->getTagStr() + " with ID '" + myAC->getID() + "' doesn't have " + toString(additionalParentIndex) + " additional parents");
    } else {
        // remove additional of the children of parent additional
        myAdditionalParents.at(additionalParentIndex)->removeAdditionalChild(additionalTobeChanged);
        // set new additional parent
        myAdditionalParents.at(additionalParentIndex) = additionalTobeChanged->getViewNet()->getNet()->retrieveAdditional(myAdditionalParents.at(additionalParentIndex)->getTagProperty().getTag(), newAdditionalParentID);
        // add additional int the children of parent additional
        myAdditionalParents.at(additionalParentIndex)->addAdditionalChild(additionalTobeChanged);
        // update geometry after inserting
        additionalTobeChanged->updateGeometry();
    }
}


void
GNEHierarchicalElementParents::changeAdditionalParent(GNEDemandElement* demandElementTobeChanged, const std::string& newAdditionalParentID, int additionalParentIndex) {
    if ((int)myAdditionalParents.size() < additionalParentIndex) {
        throw InvalidArgument(myAC->getTagStr() + " with ID '" + myAC->getID() + "' doesn't have " + toString(additionalParentIndex) + " additional parents");
    } else {
        // remove demand element of the children of parent additional
        myAdditionalParents.at(additionalParentIndex)->removeDemandElementChild(demandElementTobeChanged);
        // set new demand element parent
        myAdditionalParents.at(additionalParentIndex) = demandElementTobeChanged->getViewNet()->getNet()->retrieveAdditional(myAdditionalParents.at(additionalParentIndex)->getTagProperty().getTag(), newAdditionalParentID);
        // add demand element int the children of parent additional
        myAdditionalParents.at(additionalParentIndex)->removeDemandElementChild(demandElementTobeChanged);
        // update geometry after inserting
        demandElementTobeChanged->updateGeometry();
    }
}


void
GNEHierarchicalElementParents::changeDemandElementParent(GNEShape* shapeTobeChanged, const std::string& newDemandElementParentID, int demandElementParentIndex) {
    if ((int)myDemandElementParents.size() < demandElementParentIndex) {
        throw InvalidArgument(myAC->getTagStr() + " with ID '" + myAC->getID() + "' doesn't have " + toString(demandElementParentIndex) + " demand element parents");
    } else {
        // remove demand element of the children of parent additional
        myDemandElementParents.at(demandElementParentIndex)->removeShapeChild(shapeTobeChanged);
        // set new demand element parent
        myDemandElementParents.at(demandElementParentIndex) = shapeTobeChanged->getNet()->retrieveDemandElement(myDemandElementParents.at(demandElementParentIndex)->getTagProperty().getTag(), newDemandElementParentID);
        // add demand element int the children of parent additional
        myDemandElementParents.at(demandElementParentIndex)->addShapeChild(shapeTobeChanged);
        // update geometry after inserting
        shapeTobeChanged->updateGeometry();
    }
}


void
GNEHierarchicalElementParents::changeDemandElementParent(GNEAdditional* additionalTobeChanged, const std::string& newDemandElementParentID, int demandElementParentIndex) {
    if ((int)myDemandElementParents.size() < demandElementParentIndex) {
        throw InvalidArgument(myAC->getTagStr() + " with ID '" + myAC->getID() + "' doesn't have " + toString(demandElementParentIndex) + " demand element parents");
    } else {
        // remove demand element of the children of parent additional
        myDemandElementParents.at(demandElementParentIndex)->removeAdditionalChild(additionalTobeChanged);
        // set new demand element parent
        myDemandElementParents.at(demandElementParentIndex) = additionalTobeChanged->getViewNet()->getNet()->retrieveDemandElement(myDemandElementParents.at(demandElementParentIndex)->getTagProperty().getTag(), newDemandElementParentID);
        // add demand element int the children of parent additional
        myDemandElementParents.at(demandElementParentIndex)->addAdditionalChild(additionalTobeChanged);
        // update geometry after inserting
        additionalTobeChanged->updateGeometry();
    }
}


void
GNEHierarchicalElementParents::changeDemandElementParent(GNEDemandElement* demandElementTobeChanged, const std::string& newDemandElementParentID, int demandElementParentIndex) {
    if ((int)myDemandElementParents.size() < demandElementParentIndex) {
        throw InvalidArgument(myAC->getTagStr() + " with ID '" + myAC->getID() + "' doesn't have " + toString(demandElementParentIndex) + " demand element parents");
    } else {
        // remove additional of the children of parent additional
        myDemandElementParents.at(demandElementParentIndex)->removeDemandElementChild(demandElementTobeChanged);
        // set new additional parent
        myDemandElementParents.at(demandElementParentIndex) = demandElementTobeChanged->getViewNet()->getNet()->retrieveDemandElement(myDemandElementParents.at(demandElementParentIndex)->getTagProperty().getTag(), newDemandElementParentID);
        // add additional int the children of parent additional
        myDemandElementParents.at(demandElementParentIndex)->addDemandElementChild(demandElementTobeChanged);
        // update geometry after inserting
        demandElementTobeChanged->updateGeometry();
    }
}

/****************************************************************************/
