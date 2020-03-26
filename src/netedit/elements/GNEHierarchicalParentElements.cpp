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
/// @file    GNEHierarchicalParentElements.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
///
// A abstract class for representation of additional elements
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/elements/additional/GNEAdditional.h>
#include <netedit/elements/additional/GNEShape.h>
#include <netedit/elements/data/GNEGenericData.h>
#include <netedit/elements/demand/GNEDemandElement.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/network/GNELane.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEHierarchicalParentElements.h"

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEHierarchicalParentElements - methods
// ---------------------------------------------------------------------------

GNEHierarchicalParentElements::GNEHierarchicalParentElements(GNEAttributeCarrier* AC,
        const std::vector<GNEEdge*>& parentEdges,
        const std::vector<GNELane*>& parentLanes,
        const std::vector<GNEShape*>& parentShapes,
        const std::vector<GNEAdditional*>& parentAdditionals,
        const std::vector<GNEDemandElement*>& parentDemandElements,
        const std::vector<GNEGenericData*>& parentGenericDatas) :
    myParentConnections(this),
    myParentEdges(parentEdges),
    myParentLanes(parentLanes),
    myParentShapes(parentShapes),
    myParentAdditionals(parentAdditionals),
    myParentDemandElements(parentDemandElements),
    myParentGenericDatas(parentGenericDatas),
    myAC(AC) {
}


GNEHierarchicalParentElements::~GNEHierarchicalParentElements() {}


void
GNEHierarchicalParentElements::addParentAdditional(GNEAdditional* additional) {
    // First check that additional wasn't already inserted
    if (std::find(myParentAdditionals.begin(), myParentAdditionals.end(), additional) != myParentAdditionals.end()) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' was already inserted in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentAdditionals.push_back(additional);
    }
}


void
GNEHierarchicalParentElements::removeParentAdditional(GNEAdditional* additional) {
    // First check that additional was already inserted
    auto it = std::find(myParentAdditionals.begin(), myParentAdditionals.end(), additional);
    if (it == myParentAdditionals.end()) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' doesn't exist in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentAdditionals.erase(it);
    }
}


const std::vector<GNEAdditional*>&
GNEHierarchicalParentElements::getParentAdditionals() const {
    return myParentAdditionals;
}


size_t 
GNEHierarchicalParentElements::getNumberOfParentAdditionals(GNETagProperties::TagType additionalType) const {
    size_t number = 0;
    // check additional type
    if (additionalType == GNETagProperties::TagType::ADDITIONALELEMENT) {
        for (const auto& additional : myParentAdditionals) {
            if (additional->getTagProperty().isAdditionalElement()) {
                number++;
            }
        }
    } else if (additionalType == GNETagProperties::TagType::TAZ) {
        for (const auto& additional : myParentAdditionals) {
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
GNEHierarchicalParentElements::addParentDemandElement(GNEDemandElement* demandElement) {
    // First check that demandElement wasn't already inserted
    if (std::find(myParentDemandElements.begin(), myParentDemandElements.end(), demandElement) != myParentDemandElements.end()) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' was already inserted in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentDemandElements.push_back(demandElement);
    }
}


void
GNEHierarchicalParentElements::removeParentDemandElement(GNEDemandElement* demandElement) {
    // First check that demandElement was already inserted
    auto it = std::find(myParentDemandElements.begin(), myParentDemandElements.end(), demandElement);
    if (it == myParentDemandElements.end()) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' doesn't exist in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentDemandElements.erase(it);
    }
}


const std::vector<GNEDemandElement*>&
GNEHierarchicalParentElements::getParentDemandElements() const {
    return myParentDemandElements;
}


void
GNEHierarchicalParentElements::addParentGenericData(GNEGenericData* genericData) {
    // First check that GenericData wasn't already inserted
    if (std::find(myParentGenericDatas.begin(), myParentGenericDatas.end(), genericData) != myParentGenericDatas.end()) {
        throw ProcessError(genericData->getTagStr() + " with ID='" + genericData->getID() + "' was already inserted in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentGenericDatas.push_back(genericData);
    }
}


void
GNEHierarchicalParentElements::removeParentGenericData(GNEGenericData* genericData) {
    // First check that GenericData was already inserted
    auto it = std::find(myParentGenericDatas.begin(), myParentGenericDatas.end(), genericData);
    if (it == myParentGenericDatas.end()) {
        throw ProcessError(genericData->getTagStr() + " with ID='" + genericData->getID() + "' doesn't exist in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentGenericDatas.erase(it);
    }
}


const std::vector<GNEGenericData*>&
GNEHierarchicalParentElements::getParentGenericDatas() const {
    return myParentGenericDatas;
}


std::string
GNEHierarchicalParentElements::getNewListOfParents(const GNENetworkElement* currentElement, const GNENetworkElement* newNextElement) const {
    std::vector<std::string> solution;
    if ((currentElement->getTagProperty().getTag() == SUMO_TAG_EDGE) && (newNextElement->getTagProperty().getTag() == SUMO_TAG_EDGE)) {
        // reserve solution
        solution.reserve(myParentEdges.size());
        // iterate over edges
        for (const auto& edge : myParentEdges) {
            // add edge ID
            solution.push_back(edge->getID());
            // if current edge is the current element, then insert newNextElement ID
            if (edge == currentElement) {
                solution.push_back(newNextElement->getID());
            }
        }
    } else if ((currentElement->getTagProperty().getTag() == SUMO_TAG_LANE) && (newNextElement->getTagProperty().getTag() == SUMO_TAG_LANE)) {
        // reserve solution
        solution.reserve(myParentLanes.size());
        // iterate over lanes
        for (const auto& lane : myParentLanes) {
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
GNEHierarchicalParentElements::addParentEdge(GNEEdge* edge) {
    // Check that edge is valid and doesn't exist previously
    if (edge == nullptr) {
        throw InvalidArgument("Trying to add an empty " + toString(SUMO_TAG_EDGE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myParentEdges.begin(), myParentEdges.end(), edge) != myParentEdges.end()) {
        throw InvalidArgument("Trying to add a duplicate " + toString(SUMO_TAG_EDGE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentEdges.push_back(edge);
    }
}


void
GNEHierarchicalParentElements::removeParentEdge(GNEEdge* edge) {
    // Check that edge is valid and exist previously
    if (edge == nullptr) {
        throw InvalidArgument("Trying to remove an empty " + toString(SUMO_TAG_EDGE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        auto it = std::find(myParentEdges.begin(), myParentEdges.end(), edge);
        if (it == myParentEdges.end()) {
            throw InvalidArgument("Trying to remove a non previously inserted " + toString(SUMO_TAG_EDGE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
        } else {
            myParentEdges.erase(it);
        }
    }
}


const std::vector<GNEEdge*>&
GNEHierarchicalParentElements::getParentEdges() const {
    return myParentEdges;
}


std::vector<GNEEdge*>
GNEHierarchicalParentElements::getMiddleParentEdges() const {
    std::vector<GNEEdge*> middleEdges;
    // there are only middle edges if there is more than two edges
    if (middleEdges.size() > 2) {
        // resize middleEdges
        middleEdges.resize(myParentEdges.size() - 2);
        // iterate over second and previous last parent edge
        for (auto i = (myParentEdges.begin() + 1); i != (myParentEdges.end() - 1); i++) {
            middleEdges.push_back(*i);
        }
    }
    return middleEdges;
}


const std::vector<GNEEdge*>&
GNEHierarchicalParentElements::getPathEdges() const {
    return myRouteEdges;
}


void
GNEHierarchicalParentElements::addParentLane(GNELane* lane) {
    // Check that lane is valid and doesn't exist previously
    if (lane == nullptr) {
        throw InvalidArgument("Trying to add an empty " + toString(SUMO_TAG_EDGE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myParentLanes.begin(), myParentLanes.end(), lane) != myParentLanes.end()) {
        throw InvalidArgument("Trying to add a duplicate " + toString(SUMO_TAG_EDGE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentLanes.push_back(lane);
    }
}


void
GNEHierarchicalParentElements::removeParentLane(GNELane* lane) {
    // Check that lane is valid and exist previously
    if (lane == nullptr) {
        throw InvalidArgument("Trying to remove an empty " + toString(SUMO_TAG_EDGE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myParentLanes.begin(), myParentLanes.end(), lane) == myParentLanes.end()) {
        throw InvalidArgument("Trying to remove a non previously inserted " + toString(SUMO_TAG_EDGE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentLanes.erase(std::find(myParentLanes.begin(), myParentLanes.end(), lane));
    }
}


const std::vector<GNELane*>&
GNEHierarchicalParentElements::getParentLanes() const {
    return myParentLanes;
}


void
GNEHierarchicalParentElements::addParentShape(GNEShape* shape) {
    // Check that shape is valid and doesn't exist previously
    if (shape == nullptr) {
        throw InvalidArgument("Trying to add an empty " + toString(SUMO_TAG_EDGE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myParentShapes.begin(), myParentShapes.end(), shape) != myParentShapes.end()) {
        throw InvalidArgument("Trying to add a duplicate " + toString(SUMO_TAG_EDGE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentShapes.push_back(shape);
    }
}


void
GNEHierarchicalParentElements::removeParentShape(GNEShape* shape) {
    // Check that shape is valid and exist previously
    if (shape == nullptr) {
        throw InvalidArgument("Trying to remove an empty " + toString(SUMO_TAG_EDGE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myParentShapes.begin(), myParentShapes.end(), shape) == myParentShapes.end()) {
        throw InvalidArgument("Trying to remove a non previously inserted " + toString(SUMO_TAG_EDGE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentShapes.erase(std::find(myParentShapes.begin(), myParentShapes.end(), shape));
    }
}


const std::vector<GNEShape*>&
GNEHierarchicalParentElements::getParentShapes() const {
    return myParentShapes;
}

// ---------------------------------------------------------------------------
// GNEHierarchicalParentElements - protected methods
// ---------------------------------------------------------------------------

void
GNEHierarchicalParentElements::replaceParentEdges(GNEShape* elementChild, const std::string& newEdgeIDs) {
    // remove additional of parent edges
    for (const auto& edge : myParentEdges) {
        edge->removeChildShape(elementChild);
    }
    // obtain new parent edges
    myParentEdges = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(elementChild->getNet(), newEdgeIDs);
    // check that lane parets aren't empty
    if (myParentEdges.empty()) {
        throw InvalidArgument("New list of parent edges cannot be empty");
    } else {
        // add additional into parent edges
        for (const auto& edge : myParentEdges) {
            edge->addChildShape(elementChild);
        }
    }
}


void
GNEHierarchicalParentElements::replaceParentEdges(GNEAdditional* elementChild, const std::string& newEdgeIDs) {
    // remove additional of parent edges
    for (const auto& edge : myParentEdges) {
        edge->removeChildAdditional(elementChild);
    }
    // obtain new parent edges
    myParentEdges = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(elementChild->getViewNet()->getNet(), newEdgeIDs);
    // check that lane parets aren't empty
    if (myParentEdges.empty()) {
        throw InvalidArgument("New list of parent edges cannot be empty");
    } else {
        // add additional into parent edges
        for (const auto& edge : myParentEdges) {
            edge->addChildAdditional(elementChild);
        }
    }
}


void
GNEHierarchicalParentElements::replaceParentEdges(GNEDemandElement* elementChild, const std::string& newEdgeIDs) {
    // remove demandElement of parent edges
    for (const auto& edge : myParentEdges) {
        edge->removeChildDemandElement(elementChild);
    }
    // obtain new parent edges
    myParentEdges = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(elementChild->getViewNet()->getNet(), newEdgeIDs);
    // check that lane parets aren't empty
    if (myParentEdges.empty()) {
        throw InvalidArgument("New list of parent edges cannot be empty");
    } else {
        // add demandElement into parent edges
        for (const auto& edge : myParentEdges) {
            edge->addChildDemandElement(elementChild);
        }
    }
}


void
GNEHierarchicalParentElements::replaceParentEdges(GNEDemandElement* elementChild, const std::vector<GNEEdge*>& newEdges) {
    // remove demandElement of parent edges
    for (const auto& edge : myParentEdges) {
        edge->removeChildDemandElement(elementChild);
    }
    // set new edges
    myParentEdges = newEdges;
    // check that lane parets aren't empty
    if (myParentEdges.empty()) {
        throw InvalidArgument("New list of parent edges cannot be empty");
    } else {
        // add demandElement into parent edges
        for (const auto& edge : myParentEdges) {
            edge->addChildDemandElement(elementChild);
        }
    }
}


void
GNEHierarchicalParentElements::replaceParentEdges(GNEGenericData* elementChild, const std::vector<GNEEdge*>& newEdges) {
    // remove genericData of parent edges
    for (const auto& edge : myParentEdges) {
        edge->removeChildGenericDataElement(elementChild);
    }
    // set new edges
    myParentEdges = newEdges;
    // check that lane parets aren't empty
    if (myParentEdges.empty()) {
        throw InvalidArgument("New list of parent edges cannot be empty");
    } else {
        // add genericData into parent edges
        for (const auto& edge : myParentEdges) {
            edge->addChildGenericDataElement(elementChild);
        }
    }
}


void
GNEHierarchicalParentElements::replaceFirstParentEdge(GNEDemandElement* elementChild, GNEEdge* newFirstEdge) {
    // first check that at least there is two edges
    if (myParentEdges.size() < 2) {
        throw InvalidArgument("Invalid minimum number of edges");
    } else {
        // remove demandElement of parent edges
        myParentEdges.front()->removeChildDemandElement(elementChild);
        // replace first edge
        myParentEdges[0] = newFirstEdge;
        // add demandElement into parent edges
        myParentEdges.front()->addChildDemandElement(elementChild);
    }
}


void
GNEHierarchicalParentElements::replaceFirstParentEdge(GNEGenericData* elementChild, GNEEdge* newFirstEdge) {
    // first check that at least there is two edges
    if (myParentEdges.size() < 2) {
        throw InvalidArgument("Invalid minimum number of edges");
    } else {
        // remove generic data of parent edges
        myParentEdges.front()->removeChildGenericDataElement(elementChild);
        // replace first edge
        myParentEdges[0] = newFirstEdge;
        // add generic data into parent edges
        myParentEdges.front()->addChildGenericDataElement(elementChild);
    }
}


void
GNEHierarchicalParentElements::replaceMiddleParentEdges(GNEDemandElement* elementChild, const std::vector<GNEEdge*>& newMiddleEdges, const bool updateChildReferences) {
    // declare a vector for new parent edges
    std::vector<GNEEdge*> newEdges;
    // check if add first edge
    if (myParentEdges.size() > 0) {
        newEdges.push_back(myParentEdges.front());
    }
    // add newMiddleEdges
    for (const auto& edge : newMiddleEdges) {
        newEdges.push_back(edge);
    }
    // check if add last edge
    if (myParentEdges.size() > 1) {
        newEdges.push_back(myParentEdges.back());
    }
    // check if we have to update references in all childs, or simply update parent edges vector
    if (updateChildReferences) {
        replaceParentEdges(elementChild, newEdges);
    } else {
        myParentEdges = newEdges;
    }
}


void
GNEHierarchicalParentElements::replaceLastParentEdge(GNEDemandElement* elementChild, GNEEdge* newLastEdge) {
    // first check that at least there is two edges
    if (myParentEdges.size() < 2) {
        throw InvalidArgument("Invalid minimum number of edges");
    } else {
        // remove demandElement of parent edges
        myParentEdges.back()->removeChildDemandElement(elementChild);
        // replace last edge
        myParentEdges.pop_back();
        myParentEdges.push_back(newLastEdge);
        // add demandElement into parent edges
        myParentEdges.back()->addChildDemandElement(elementChild);
    }
}


void
GNEHierarchicalParentElements::replaceLastParentEdge(GNEGenericData* elementChild, GNEEdge* newLastEdge) {
    // first check that at least there is two edges
    if (myParentEdges.size() < 2) {
        throw InvalidArgument("Invalid minimum number of edges");
    } else {
        // remove generic data of parent edges
        myParentEdges.back()->removeChildGenericDataElement(elementChild);
        // replace last edge
        myParentEdges.pop_back();
        myParentEdges.push_back(newLastEdge);
        // add generic data into parent edges
        myParentEdges.back()->addChildGenericDataElement(elementChild);
    }
}


void
GNEHierarchicalParentElements::replacePathEdges(GNEDemandElement* elementChild, const std::vector<GNEEdge*>& routeEdges) {
    // remove demandElement of parent edges
    for (const auto& edge : myRouteEdges) {
        edge->removePathElement(elementChild);
    }
    // set new route edges
    myRouteEdges = routeEdges;
    // add demandElement into parent edges
    for (const auto& edge : myRouteEdges) {
        edge->addPathElement(elementChild);
    }
}


void
GNEHierarchicalParentElements::replaceParentLanes(GNEAdditional* elementChild, const std::string& newLaneIDs) {
    // remove additional of parent edges
    for (const auto& lane : myParentLanes) {
        lane->removeChildAdditional(elementChild);
    }
    // obtain new parent edges
    myParentLanes = GNEAttributeCarrier::parse<std::vector<GNELane*> >(elementChild->getViewNet()->getNet(), newLaneIDs);
    // check that lane parets aren't empty
    if (myParentLanes.empty()) {
        throw InvalidArgument("New list of parent lanes cannot be empty");
    } else {
        // add additional into parent edges
        for (const auto& lane : myParentLanes) {
            lane->addChildAdditional(elementChild);
        }
    }
}


void
GNEHierarchicalParentElements::replaceParentLanes(GNEDemandElement* elementChild, const std::string& newLaneIDs) {
    // remove demandElement of parent edges
    for (const auto& lane : myParentLanes) {
        lane->removeChildDemandElement(elementChild);
    }
    // obtain new parent edges
    myParentLanes = GNEAttributeCarrier::parse<std::vector<GNELane*> >(elementChild->getViewNet()->getNet(), newLaneIDs);
    // check that lane parets aren't empty
    if (myParentLanes.empty()) {
        throw InvalidArgument("New list of parent lanes cannot be empty");
    } else {
        // add demandElement into parent edges
        for (const auto& lane : myParentLanes) {
            lane->addChildDemandElement(elementChild);
        }
    }
}


void
GNEHierarchicalParentElements::replaceParentLanes(GNEShape* elementChild, const std::string& newLaneIDs) {
    // remove demandElement of parent edges
    for (const auto& lane : myParentLanes) {
        lane->removeChildShape(elementChild);
    }
    // obtain new parent edges
    myParentLanes = GNEAttributeCarrier::parse<std::vector<GNELane*> >(elementChild->getNet(), newLaneIDs);
    // check that lane parets aren't empty
    if (myParentLanes.empty()) {
        throw InvalidArgument("New list of parent lanes cannot be empty");
    } else {
        // add demandElement into parent edges
        for (const auto& lane : myParentLanes) {
            lane->addChildShape(elementChild);
        }
    }
}


void
GNEHierarchicalParentElements::replaceParentAdditional(GNEShape* shapeTobeChanged, const std::string& newParentAdditionalID, int additionalParentIndex) {
    if ((int)myParentAdditionals.size() < additionalParentIndex) {
        throw InvalidArgument(myAC->getTagStr() + " with ID '" + myAC->getID() + "' doesn't have " + toString(additionalParentIndex) + " parent additionals");
    } else {
        // remove additional of the children of parent additional
        myParentAdditionals.at(additionalParentIndex)->removeChildShape(shapeTobeChanged);
        // set new parent additional
        myParentAdditionals.at(additionalParentIndex) = shapeTobeChanged->getNet()->retrieveAdditional(myParentAdditionals.at(additionalParentIndex)->getTagProperty().getTag(), newParentAdditionalID);
        // add additional int the children of parent additional
        myParentAdditionals.at(additionalParentIndex)->addChildShape(shapeTobeChanged);
        // update geometry after inserting
        shapeTobeChanged->updateGeometry();
    }
}


void
GNEHierarchicalParentElements::replaceParentAdditional(GNEAdditional* additionalTobeChanged, const std::string& newParentAdditionalID, int additionalParentIndex) {
    if ((int)myParentAdditionals.size() < additionalParentIndex) {
        throw InvalidArgument(myAC->getTagStr() + " with ID '" + myAC->getID() + "' doesn't have " + toString(additionalParentIndex) + " parent additionals");
    } else {
        // remove additional of the children of parent additional
        myParentAdditionals.at(additionalParentIndex)->removeChildAdditional(additionalTobeChanged);
        // set new parent additional
        myParentAdditionals.at(additionalParentIndex) = additionalTobeChanged->getViewNet()->getNet()->retrieveAdditional(myParentAdditionals.at(additionalParentIndex)->getTagProperty().getTag(), newParentAdditionalID);
        // add additional int the children of parent additional
        myParentAdditionals.at(additionalParentIndex)->addChildAdditional(additionalTobeChanged);
        // update geometry after inserting
        additionalTobeChanged->updateGeometry();
    }
}


void
GNEHierarchicalParentElements::replaceParentAdditional(GNEDemandElement* demandElementTobeChanged, const std::string& newParentAdditionalID, int additionalParentIndex) {
    if ((int)myParentAdditionals.size() < additionalParentIndex) {
        throw InvalidArgument(myAC->getTagStr() + " with ID '" + myAC->getID() + "' doesn't have " + toString(additionalParentIndex) + " parent additionals");
    } else {
        // remove demand element of the children of parent additional
        myParentAdditionals.at(additionalParentIndex)->removeChildDemandElement(demandElementTobeChanged);
        // set new parent demand element
        myParentAdditionals.at(additionalParentIndex) = demandElementTobeChanged->getViewNet()->getNet()->retrieveAdditional(myParentAdditionals.at(additionalParentIndex)->getTagProperty().getTag(), newParentAdditionalID);
        // add demand element int the children of parent additional
        myParentAdditionals.at(additionalParentIndex)->removeChildDemandElement(demandElementTobeChanged);
        // update geometry after inserting
        demandElementTobeChanged->updateGeometry();
    }
}


void
GNEHierarchicalParentElements::replaceParentDemandElement(GNEShape* shapeTobeChanged, const std::string& newParentDemandElementID, int demandElementParentIndex) {
    if ((int)myParentDemandElements.size() < demandElementParentIndex) {
        throw InvalidArgument(myAC->getTagStr() + " with ID '" + myAC->getID() + "' doesn't have " + toString(demandElementParentIndex) + " parent demand elements");
    } else {
        // remove demand element of the children of parent additional
        myParentDemandElements.at(demandElementParentIndex)->removeChildShape(shapeTobeChanged);
        // set new parent demand element
        myParentDemandElements.at(demandElementParentIndex) = shapeTobeChanged->getNet()->retrieveDemandElement(myParentDemandElements.at(demandElementParentIndex)->getTagProperty().getTag(), newParentDemandElementID);
        // add demand element int the children of parent additional
        myParentDemandElements.at(demandElementParentIndex)->addChildShape(shapeTobeChanged);
        // update geometry after inserting
        shapeTobeChanged->updateGeometry();
    }
}


void
GNEHierarchicalParentElements::replaceParentDemandElement(GNEAdditional* additionalTobeChanged, const std::string& newParentDemandElementID, int demandElementParentIndex) {
    if ((int)myParentDemandElements.size() < demandElementParentIndex) {
        throw InvalidArgument(myAC->getTagStr() + " with ID '" + myAC->getID() + "' doesn't have " + toString(demandElementParentIndex) + " parent demand elements");
    } else {
        // remove demand element of the children of parent additional
        myParentDemandElements.at(demandElementParentIndex)->removeChildAdditional(additionalTobeChanged);
        // set new parent demand element
        myParentDemandElements.at(demandElementParentIndex) = additionalTobeChanged->getViewNet()->getNet()->retrieveDemandElement(myParentDemandElements.at(demandElementParentIndex)->getTagProperty().getTag(), newParentDemandElementID);
        // add demand element int the children of parent additional
        myParentDemandElements.at(demandElementParentIndex)->addChildAdditional(additionalTobeChanged);
        // update geometry after inserting
        additionalTobeChanged->updateGeometry();
    }
}


void
GNEHierarchicalParentElements::replaceParentDemandElement(GNEDemandElement* demandElementTobeChanged, const std::string& newParentDemandElementID, int demandElementParentIndex) {
    if ((int)myParentDemandElements.size() < demandElementParentIndex) {
        throw InvalidArgument(myAC->getTagStr() + " with ID '" + myAC->getID() + "' doesn't have " + toString(demandElementParentIndex) + " parent demand elements");
    } else {
        // remove additional of the children of parent additional
        myParentDemandElements.at(demandElementParentIndex)->removeChildDemandElement(demandElementTobeChanged);
        // set new parent additional
        myParentDemandElements.at(demandElementParentIndex) = demandElementTobeChanged->getViewNet()->getNet()->retrieveDemandElement(myParentDemandElements.at(demandElementParentIndex)->getTagProperty().getTag(), newParentDemandElementID);
        // add additional int the children of parent additional
        myParentDemandElements.at(demandElementParentIndex)->addChildDemandElement(demandElementTobeChanged);
        // update geometry after inserting
        demandElementTobeChanged->updateGeometry();
    }
}


/****************************************************************************/
