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


std::string 
GNEHierarchicalElementParents::getNewListOfParents(const GNENetElement *currentElement, const GNENetElement *newNextElement) const {
    std::vector<std::string> solution;
    if ((currentElement->getTagProperty().getTag() == SUMO_TAG_EDGE) && (newNextElement->getTagProperty().getTag() == SUMO_TAG_EDGE)) {
        // reserve solution
        solution.reserve(myEdgeParents.size());
        // iterate over edges
        for (const auto &edge: myEdgeParents) {
            // add edge ID
            solution.push_back(edge->getID());
            // if current edge is the current element, then insert newNextElement ID
            if (edge == currentElement) {
                solution.push_back(newNextElement->getID());
            }
        }
    } else if ((currentElement->getTagProperty().getTag() == SUMO_TAG_LANE) && (newNextElement->getTagProperty().getTag() == SUMO_TAG_LANE)) {
        // reserve solution
        solution.reserve(myLaneParents.size());
        // iterate over lanes
        for (const auto &lane: myLaneParents) {
            // add lane ID
            solution.push_back(lane->getID());
            // if current lane is the current element, then insert newNextElement ID
            if (lane == currentElement) {
                solution.push_back(newNextElement->getID());
            }
        }
    }
    return toString(solution);
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
// GNEHierarchicalElementParents - protected methods
// ---------------------------------------------------------------------------

const std::vector<GNEEdge*>&
GNEHierarchicalElementParents::getPathEdges() const {
    return myRouteEdges;
}


std::vector<GNEEdge*> 
GNEHierarchicalElementParents::getMiddleEdgeParents() const {
    std::vector<GNEEdge*> middleEdges;
    // there are only middle edges if there is more than two edges
    if (middleEdges.size() > 2) {
        // resize middleEdges
        middleEdges.resize(myEdgeParents.size()-2);
        // iterate over second and previous last edge parent
        for (auto i = (myEdgeParents.begin() + 1); i !=(myEdgeParents.end() - 1); i++) {
            middleEdges.push_back(*i);
        }
    }
    return middleEdges;
}


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
GNEHierarchicalElementParents::changeFirstEdgeParent(GNEDemandElement* elementChild, GNEEdge* newFirstEdge) {
    // first check that at least there is two edges
    if (myEdgeParents.size() < 2) {
        throw InvalidArgument("Invalid minimum number of edges");
    } else {
        // remove demandElement of edge parents
        myEdgeParents.front()->removeDemandElementChild(elementChild);
        // replace first edge 
        myEdgeParents[0] = newFirstEdge;
        // add demandElement into edge parents
        myEdgeParents.front()->addDemandElementChild(elementChild);
    }
}


void 
GNEHierarchicalElementParents::changeMiddleEdgeParents(GNEDemandElement* elementChild, const std::vector<GNEEdge*>& newMiddleEdges) {
    // first check that at least there is two edges
    if (myEdgeParents.size() < 2) {
        throw InvalidArgument("Invalid minimum number of edges");
    } else {
        // declare a vector for new parent edges
        std::vector<GNEEdge*> newEdges;
        // resize newEdges
        newEdges.resize(newMiddleEdges.size() + 2);
        // add first edge
        newEdges.push_back(myEdgeParents.front());
        // add newMiddleEdges
        for (const auto &edge : newMiddleEdges) {
            newEdges.push_back(edge);
        }
        // add last edge
        newEdges.push_back(myEdgeParents.back());
        // change all edge parents
        changeEdgeParents(elementChild, newEdges);
    }
}


void 
GNEHierarchicalElementParents::changeLastEdgeParent(GNEDemandElement* elementChild, GNEEdge* newLastEdge) {
    // first check that at least there is two edges
    if (myEdgeParents.size() < 2) {
        throw InvalidArgument("Invalid minimum number of edges");
    } else {
        // remove demandElement of edge parents
        myEdgeParents.back()->removeDemandElementChild(elementChild);
        // replace last edge 
        myEdgeParents.pop_back();
        myEdgeParents.push_back(newLastEdge);
        // add demandElement into edge parents
        myEdgeParents.back()->addDemandElementChild(elementChild);
    }
}


void 
GNEHierarchicalElementParents::changePathEdges(GNEDemandElement* elementChild, const std::vector<GNEEdge*> &routeEdges) {
    // remove demandElement of edge parents
    for (const auto& i : myRouteEdges) {
        i->removePathElement(elementChild);
    }
    // set new route edges
    myRouteEdges = routeEdges;
    // add demandElement into edge parents
    for (const auto& i : myEdgeParents) {
        i->addPathElement(elementChild);
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
