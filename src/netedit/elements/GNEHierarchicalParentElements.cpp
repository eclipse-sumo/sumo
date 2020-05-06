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
#include <netedit/elements/additional/GNETAZElement.h>
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

GNEHierarchicalParentElements::GNEHierarchicalParentElements(const GNEAttributeCarrier* AC,
        const std::vector<GNEJunction*>& parentJunctions,
        const std::vector<GNEEdge*>& parentEdges,
        const std::vector<GNELane*>& parentLanes,
        const std::vector<GNEAdditional*>& parentAdditionals,
        const std::vector<GNEShape*>& parentShapes,
        const std::vector<GNETAZElement*>& parentTAZElements,
        const std::vector<GNEDemandElement*>& ParentDemandElements,
        const std::vector<GNEGenericData*>& parentGenericDatas) :
    myParentConnections(this),
    myParentJunctions(parentJunctions),
    myParentEdges(parentEdges),
    myParentLanes(parentLanes),
    myParentAdditionals(parentAdditionals),
    myParentShapes(parentShapes),
    myParentTAZElements(parentTAZElements),
    myParentDemandElements(ParentDemandElements),
    myParentGenericDatas(parentGenericDatas),
    myAC(AC) {
}


GNEHierarchicalParentElements::~GNEHierarchicalParentElements() {}


template <> void
GNEHierarchicalParentElements::addParentElement(GNEEdge* edge) {
    // Check that edge is valid and doesn't exist previously
    if (edge == nullptr) {
        throw InvalidArgument("Trying to add an empty " + toString(SUMO_TAG_EDGE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myParentEdges.begin(), myParentEdges.end(), edge) != myParentEdges.end()) {
        throw InvalidArgument("Trying to add a duplicate " + toString(SUMO_TAG_EDGE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentEdges.push_back(edge);
    }
}


template <> void
GNEHierarchicalParentElements::addParentElement(GNELane* lane) {
    // Check that lane is valid and doesn't exist previously
    if (lane == nullptr) {
        throw InvalidArgument("Trying to add an empty " + toString(SUMO_TAG_LANE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myParentLanes.begin(), myParentLanes.end(), lane) != myParentLanes.end()) {
        throw InvalidArgument("Trying to add a duplicate " + toString(SUMO_TAG_LANE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentLanes.push_back(lane);
    }
}


template <> void
GNEHierarchicalParentElements::addParentElement(GNEAdditional* additional) {
    // First check that additional wasn't already inserted
    if (std::find(myParentAdditionals.begin(), myParentAdditionals.end(), additional) != myParentAdditionals.end()) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' was already inserted in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentAdditionals.push_back(additional);
    }
}


template <> void
GNEHierarchicalParentElements::addParentElement(GNEShape* shape) {
    // Check that shape is valid and doesn't exist previously
    if (shape == nullptr) {
        throw InvalidArgument("Trying to add an empty shape parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myParentShapes.begin(), myParentShapes.end(), shape) != myParentShapes.end()) {
        throw InvalidArgument("Trying to add a duplicate shape parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentShapes.push_back(shape);
    }
}


template <> void
GNEHierarchicalParentElements::addParentElement(GNETAZElement* TAZElement) {
    // Check that TAZElement is valid and doesn't exist previously
    if (TAZElement == nullptr) {
        throw InvalidArgument("Trying to add an empty " + toString(SUMO_TAG_TAZ) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myParentTAZElements.begin(), myParentTAZElements.end(), TAZElement) != myParentTAZElements.end()) {
        throw InvalidArgument("Trying to add a duplicate " + toString(SUMO_TAG_TAZ) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentTAZElements.push_back(TAZElement);
    }
}


template <> void
GNEHierarchicalParentElements::addParentElement(GNEDemandElement* demandElement) {
    // First check that demandElement wasn't already inserted
    if (std::find(myParentDemandElements.begin(), myParentDemandElements.end(), demandElement) != myParentDemandElements.end()) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' was already inserted in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentDemandElements.push_back(demandElement);
    }
}


template <> void
GNEHierarchicalParentElements::addParentElement(GNEGenericData* genericData) {
    // First check that GenericData wasn't already inserted
    if (std::find(myParentGenericDatas.begin(), myParentGenericDatas.end(), genericData) != myParentGenericDatas.end()) {
        throw ProcessError(genericData->getTagStr() + " with ID='" + genericData->getID() + "' was already inserted in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentGenericDatas.push_back(genericData);
    }
}


template <> void
GNEHierarchicalParentElements::removeParentElement(GNEEdge* edge) {
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



template <> void
GNEHierarchicalParentElements::removeParentElement(GNELane* lane) {
    // Check that lane is valid and exist previously
    if (lane == nullptr) {
        throw InvalidArgument("Trying to remove an empty " + toString(SUMO_TAG_LANE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myParentLanes.begin(), myParentLanes.end(), lane) == myParentLanes.end()) {
        throw InvalidArgument("Trying to remove a non previously inserted " + toString(SUMO_TAG_LANE) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentLanes.erase(std::find(myParentLanes.begin(), myParentLanes.end(), lane));
    }
}


template <> void
GNEHierarchicalParentElements::removeParentElement(GNEAdditional* additional) {
    // First check that additional was already inserted
    auto it = std::find(myParentAdditionals.begin(), myParentAdditionals.end(), additional);
    if (it == myParentAdditionals.end()) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' doesn't exist in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentAdditionals.erase(it);
    }
}


template <> void
GNEHierarchicalParentElements::removeParentElement(GNEShape* shape) {
    // Check that shape is valid and exist previously
    if (shape == nullptr) {
        throw InvalidArgument("Trying to remove an empty shape parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myParentShapes.begin(), myParentShapes.end(), shape) == myParentShapes.end()) {
        throw InvalidArgument("Trying to remove a non previously inserted shape parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentShapes.erase(std::find(myParentShapes.begin(), myParentShapes.end(), shape));
    }
}


template <> void
GNEHierarchicalParentElements::removeParentElement(GNETAZElement* TAZElement) {
    // Check that TAZElement is valid and exist previously
    if (TAZElement == nullptr) {
        throw InvalidArgument("Trying to remove an empty " + toString(SUMO_TAG_TAZ) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myParentTAZElements.begin(), myParentTAZElements.end(), TAZElement) == myParentTAZElements.end()) {
        throw InvalidArgument("Trying to remove a non previously inserted " + toString(SUMO_TAG_TAZ) + " parent in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentTAZElements.erase(std::find(myParentTAZElements.begin(), myParentTAZElements.end(), TAZElement));
    }
}


template <> void
GNEHierarchicalParentElements::removeParentElement(GNEDemandElement* demandElement) {
    // First check that demandElement was already inserted
    auto it = std::find(myParentDemandElements.begin(), myParentDemandElements.end(), demandElement);
    if (it == myParentDemandElements.end()) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' doesn't exist in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentDemandElements.erase(it);
    }
}


template <> void
GNEHierarchicalParentElements::removeParentElement(GNEGenericData* genericData) {
    // First check that GenericData was already inserted
    auto it = std::find(myParentGenericDatas.begin(), myParentGenericDatas.end(), genericData);
    if (it == myParentGenericDatas.end()) {
        throw ProcessError(genericData->getTagStr() + " with ID='" + genericData->getID() + "' doesn't exist in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myParentGenericDatas.erase(it);
    }
}


const std::vector<GNEEdge*>&
GNEHierarchicalParentElements::getParentEdges() const {
    return myParentEdges;
}


const std::vector<GNELane*>&
GNEHierarchicalParentElements::getParentLanes() const {
    return myParentLanes;
}


const std::vector<GNEAdditional*>&
GNEHierarchicalParentElements::getParentAdditionals() const {
    return myParentAdditionals;
}


const std::vector<GNEShape*>&
GNEHierarchicalParentElements::getParentShapes() const {
    return myParentShapes;
}


const std::vector<GNETAZElement*>&
GNEHierarchicalParentElements::getParentTAZElements() const {
    return myParentTAZElements;
}


const std::vector<GNEDemandElement*>&
GNEHierarchicalParentElements::getParentDemandElements() const {
    return myParentDemandElements;
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


GNEJunction*
GNEHierarchicalParentElements::getFirstParentJunction() const {
    if (myParentJunctions.size() > 0) {
        return myParentJunctions.at(0);
    } else {
        throw InvalidArgument("Invalid number of parent junctions (0)");
    }
}


GNEJunction*
GNEHierarchicalParentElements::getSecondParentJunction()const {
    if (myParentJunctions.size() > 1) {
        return myParentJunctions.at(1);
    } else {
        throw InvalidArgument("Invalid number of parent junctions (<1)");
    }
}


void 
GNEHierarchicalParentElements::updateFirstParentJunction(GNEJunction* junction) {
    if (myParentJunctions.size() > 0) {
        myParentJunctions.at(0) = junction;
    } else {
        throw InvalidArgument("Invalid number of parent junctions (0)");
    }
}


void 
GNEHierarchicalParentElements::updateSecondParentJunction(GNEJunction* junction) {
    if (myParentJunctions.size() > 1) {
        myParentJunctions.at(1) = junction;
    } else {
        throw InvalidArgument("Invalid number of parent junctions (<1)");
    }
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

// ---------------------------------------------------------------------------
// GNEHierarchicalParentElements - protected methods
// ---------------------------------------------------------------------------

void
GNEHierarchicalParentElements::replaceParentEdges(GNEShape* elementChild, const std::string& newEdgeIDs) {
    // remove additional of parent edges
    for (const auto& edge : myParentEdges) {
        edge->removeChildElement(elementChild);
    }
    // obtain new parent edges
    myParentEdges = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(elementChild->getNet(), newEdgeIDs);
    // check that lane parets aren't empty
    if (myParentEdges.empty()) {
        throw InvalidArgument("New list of parent edges cannot be empty");
    } else {
        // add additional into parent edges
        for (const auto& edge : myParentEdges) {
            edge->addChildElement(elementChild);
        }
    }
}


void
GNEHierarchicalParentElements::replaceParentEdges(GNEAdditional* elementChild, const std::string& newEdgeIDs) {
    // remove additional of parent edges
    for (const auto& edge : myParentEdges) {
        edge->removeChildElement(elementChild);
    }
    // obtain new parent edges
    myParentEdges = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(elementChild->getNet(), newEdgeIDs);
    // check that lane parets aren't empty
    if (myParentEdges.empty()) {
        throw InvalidArgument("New list of parent edges cannot be empty");
    } else {
        // add additional into parent edges
        for (const auto& edge : myParentEdges) {
            edge->addChildElement(elementChild);
        }
    }
}


void
GNEHierarchicalParentElements::replaceParentEdges(GNEDemandElement* elementChild, const std::string& newEdgeIDs) {
    // remove demandElement of parent edges
    for (const auto& edge : myParentEdges) {
        edge->removeChildElement(elementChild);
    }
    // obtain new parent edges
    myParentEdges = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(elementChild->getNet(), newEdgeIDs);
    // check that lane parets aren't empty
    if (myParentEdges.empty()) {
        throw InvalidArgument("New list of parent edges cannot be empty");
    } else {
        // add demandElement into parent edges
        for (const auto& edge : myParentEdges) {
            edge->addChildElement(elementChild);
        }
    }
}


void
GNEHierarchicalParentElements::replaceParentEdges(GNEDemandElement* elementChild, const std::vector<GNEEdge*>& newEdges) {
    // remove demandElement of parent edges
    for (const auto& edge : myParentEdges) {
        edge->removeChildElement(elementChild);
    }
    // set new edges
    myParentEdges = newEdges;
    // check that lane parets aren't empty
    if (myParentEdges.empty()) {
        throw InvalidArgument("New list of parent edges cannot be empty");
    } else {
        // add demandElement into parent edges
        for (const auto& edge : myParentEdges) {
            edge->addChildElement(elementChild);
        }
    }
}


void
GNEHierarchicalParentElements::replaceParentEdges(GNEGenericData* elementChild, const std::vector<GNEEdge*>& newEdges) {
    // remove genericData of parent edges
    for (const auto& edge : myParentEdges) {
        edge->removeChildElement(elementChild);
    }
    // set new edges
    myParentEdges = newEdges;
    // check that lane parets aren't empty
    if (myParentEdges.empty()) {
        throw InvalidArgument("New list of parent edges cannot be empty");
    } else {
        // add genericData into parent edges
        for (const auto& edge : myParentEdges) {
            edge->addChildElement(elementChild);
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
        myParentEdges.front()->removeChildElement(elementChild);
        // replace first edge
        myParentEdges[0] = newFirstEdge;
        // add demandElement into parent edges
        myParentEdges.front()->addChildElement(elementChild);
    }
}


void
GNEHierarchicalParentElements::replaceFirstParentEdge(GNEGenericData* elementChild, GNEEdge* newFirstEdge) {
    // first check that at least there is two edges
    if (myParentEdges.size() < 2) {
        throw InvalidArgument("Invalid minimum number of edges");
    } else {
        // remove generic data of parent edges
        myParentEdges.front()->removeChildElement(elementChild);
        // replace first edge
        myParentEdges[0] = newFirstEdge;
        // add generic data into parent edges
        myParentEdges.front()->addChildElement(elementChild);
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
        myParentEdges.back()->removeChildElement(elementChild);
        // replace last edge
        myParentEdges.pop_back();
        myParentEdges.push_back(newLastEdge);
        // add demandElement into parent edges
        myParentEdges.back()->addChildElement(elementChild);
    }
}


void
GNEHierarchicalParentElements::replaceLastParentEdge(GNEGenericData* elementChild, GNEEdge* newLastEdge) {
    // first check that at least there is two edges
    if (myParentEdges.size() < 2) {
        throw InvalidArgument("Invalid minimum number of edges");
    } else {
        // remove generic data of parent edges
        myParentEdges.back()->removeChildElement(elementChild);
        // replace last edge
        myParentEdges.pop_back();
        myParentEdges.push_back(newLastEdge);
        // add generic data into parent edges
        myParentEdges.back()->addChildElement(elementChild);
    }
}


void
GNEHierarchicalParentElements::replaceParentLanes(GNEAdditional* elementChild, const std::string& newLaneIDs) {
    // remove additional of parent edges
    for (const auto& lane : myParentLanes) {
        lane->removeChildElement(elementChild);
    }
    // obtain new parent edges
    myParentLanes = GNEAttributeCarrier::parse<std::vector<GNELane*> >(elementChild->getNet(), newLaneIDs);
    // check that lane parets aren't empty
    if (myParentLanes.empty()) {
        throw InvalidArgument("New list of parent lanes cannot be empty");
    } else {
        // add additional into parent edges
        for (const auto& lane : myParentLanes) {
            lane->addChildElement(elementChild);
        }
    }
}


void
GNEHierarchicalParentElements::replaceParentLanes(GNEDemandElement* elementChild, const std::string& newLaneIDs) {
    // remove demandElement of parent edges
    for (const auto& lane : myParentLanes) {
        lane->removeChildElement(elementChild);
    }
    // obtain new parent edges
    myParentLanes = GNEAttributeCarrier::parse<std::vector<GNELane*> >(elementChild->getNet(), newLaneIDs);
    // check that lane parets aren't empty
    if (myParentLanes.empty()) {
        throw InvalidArgument("New list of parent lanes cannot be empty");
    } else {
        // add demandElement into parent edges
        for (const auto& lane : myParentLanes) {
            lane->addChildElement(elementChild);
        }
    }
}


void
GNEHierarchicalParentElements::replaceParentLanes(GNEShape* elementChild, const std::string& newLaneIDs) {
    // remove demandElement of parent edges
    for (const auto& lane : myParentLanes) {
        lane->removeChildElement(elementChild);
    }
    // obtain new parent edges
    myParentLanes = GNEAttributeCarrier::parse<std::vector<GNELane*> >(elementChild->getNet(), newLaneIDs);
    // check that lane parets aren't empty
    if (myParentLanes.empty()) {
        throw InvalidArgument("New list of parent lanes cannot be empty");
    } else {
        // add demandElement into parent edges
        for (const auto& lane : myParentLanes) {
            lane->addChildElement(elementChild);
        }
    }
}


void
GNEHierarchicalParentElements::replaceParentAdditional(GNEShape* shapeTobeChanged, const std::string& newParentAdditionalID, int additionalParentIndex) {
    if ((int)myParentAdditionals.size() < additionalParentIndex) {
        throw InvalidArgument(myAC->getTagStr() + " with ID '" + myAC->getID() + "' doesn't have " + toString(additionalParentIndex) + " parent additionals");
    } else {
        // remove additional of the children of parent additional
        myParentAdditionals.at(additionalParentIndex)->removeChildElement(shapeTobeChanged);
        // set new parent additional
        myParentAdditionals.at(additionalParentIndex) = shapeTobeChanged->getNet()->retrieveAdditional(myParentAdditionals.at(additionalParentIndex)->getTagProperty().getTag(), newParentAdditionalID);
        // add additional int the children of parent additional
        myParentAdditionals.at(additionalParentIndex)->addChildElement(shapeTobeChanged);
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
        myParentAdditionals.at(additionalParentIndex)->removeChildElement(additionalTobeChanged);
        // set new parent additional
        myParentAdditionals.at(additionalParentIndex) = additionalTobeChanged->getNet()->retrieveAdditional(myParentAdditionals.at(additionalParentIndex)->getTagProperty().getTag(), newParentAdditionalID);
        // add additional int the children of parent additional
        myParentAdditionals.at(additionalParentIndex)->addChildElement(additionalTobeChanged);
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
        myParentAdditionals.at(additionalParentIndex)->removeChildElement(demandElementTobeChanged);
        // set new parent demand element
        myParentAdditionals.at(additionalParentIndex) = demandElementTobeChanged->getNet()->retrieveAdditional(myParentAdditionals.at(additionalParentIndex)->getTagProperty().getTag(), newParentAdditionalID);
        // add demand element int the children of parent additional
        myParentAdditionals.at(additionalParentIndex)->removeChildElement(demandElementTobeChanged);
        // update geometry after inserting
        demandElementTobeChanged->updateGeometry();
    }
}


void
GNEHierarchicalParentElements::replaceFirstParentTAZElement(GNEGenericData* elementChild, GNETAZElement* newFirstTAZElement) {
    // first check that at least there is two TAZElements
    if (myParentTAZElements.size() < 2) {
        throw InvalidArgument("Invalid minimum number of TAZElements");
    } else {
        // remove generic data of parent TAZElements
        myParentTAZElements.front()->removeChildElement(elementChild);
        // replace first TAZElement
        myParentTAZElements[0] = newFirstTAZElement;
        // add generic data into parent TAZElements
        myParentTAZElements.front()->addChildElement(elementChild);
    }
}


void
GNEHierarchicalParentElements::replaceLastParentTAZElement(GNEGenericData* elementChild, GNETAZElement* newLastTAZElement) {
    // first check that at least there is two TAZElements
    if (myParentTAZElements.size() < 2) {
        throw InvalidArgument("Invalid minimum number of TAZElements");
    } else {
        // remove demandElement of parent TAZElements
        myParentTAZElements.back()->removeChildElement(elementChild);
        // replace last TAZElement
        myParentTAZElements.pop_back();
        myParentTAZElements.push_back(newLastTAZElement);
        // add demandElement into parent TAZElements
        myParentTAZElements.back()->addChildElement(elementChild);
    }
}


void
GNEHierarchicalParentElements::replaceParentDemandElement(GNEShape* shapeTobeChanged, const std::string& newParentDemandElementID, int demandElementParentIndex) {
    if ((int)myParentDemandElements.size() < demandElementParentIndex) {
        throw InvalidArgument(myAC->getTagStr() + " with ID '" + myAC->getID() + "' doesn't have " + toString(demandElementParentIndex) + " parent demand elements");
    } else {
        // remove demand element of the children of parent additional
        myParentDemandElements.at(demandElementParentIndex)->removeChildElement(shapeTobeChanged);
        // set new parent demand element
        myParentDemandElements.at(demandElementParentIndex) = shapeTobeChanged->getNet()->retrieveDemandElement(myParentDemandElements.at(demandElementParentIndex)->getTagProperty().getTag(), newParentDemandElementID);
        // add demand element int the children of parent additional
        myParentDemandElements.at(demandElementParentIndex)->addChildElement(shapeTobeChanged);
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
        myParentDemandElements.at(demandElementParentIndex)->removeChildElement(additionalTobeChanged);
        // set new parent demand element
        myParentDemandElements.at(demandElementParentIndex) = additionalTobeChanged->getNet()->retrieveDemandElement(myParentDemandElements.at(demandElementParentIndex)->getTagProperty().getTag(), newParentDemandElementID);
        // add demand element int the children of parent additional
        myParentDemandElements.at(demandElementParentIndex)->addChildElement(additionalTobeChanged);
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
        myParentDemandElements.at(demandElementParentIndex)->removeChildElement(demandElementTobeChanged);
        // set new parent additional
        myParentDemandElements.at(demandElementParentIndex) = demandElementTobeChanged->getNet()->retrieveDemandElement(myParentDemandElements.at(demandElementParentIndex)->getTagProperty().getTag(), newParentDemandElementID);
        // add additional int the children of parent additional
        myParentDemandElements.at(demandElementParentIndex)->addChildElement(demandElementTobeChanged);
        // update geometry after inserting
        demandElementTobeChanged->updateGeometry();
    }
}


/****************************************************************************/
