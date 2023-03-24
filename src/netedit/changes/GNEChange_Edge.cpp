/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GNEChange_Edge.cpp
/// @author  Jakob Erdmann
/// @date    Mar 2011
///
// A network change in which a single edge is created or deleted
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>


#include "GNEChange_Edge.h"

// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_Edge, GNEChange, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================


/// @brief constructor for creating an edge
GNEChange_Edge::GNEChange_Edge(GNEEdge* edge, bool forward):
    GNEChange(Supermode::NETWORK, edge, forward, edge->isAttributeCarrierSelected()),
    myEdge(edge) {
    edge->incRef("GNEChange_Edge");
    // save all hierarchical elements of edge's lane
    for (const auto& lane : edge->getLanes()) {
        myLaneParentAdditionals.push_back(lane->getParentAdditionals());
        myLaneParentDemandElements.push_back(lane->getParentDemandElements());
        myLaneParentGenericData.push_back(lane->getParentGenericDatas());
        myChildLaneAdditionals.push_back(lane->getChildAdditionals());
        myChildLaneDemandElements.push_back(lane->getChildDemandElements());
        myChildLaneGenericData.push_back(lane->getChildGenericDatas());
    }
}


GNEChange_Edge::~GNEChange_Edge() {
    myEdge->decRef("GNEChange_Edge");
    if (myEdge->unreferenced()) {
        // show extra information for tests
        WRITE_DEBUG("Deleting unreferenced " + myEdge->getTagStr() + " '" + myEdge->getID() + "' GNEChange_Edge");
        // delete edge
        delete myEdge;
    }
}


void
GNEChange_Edge::undo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Removing " + myEdge->getTagStr() + " '" + myEdge->getID() + "' from " + toString(SUMO_TAG_NET));
        // unselect if mySelectedElement is enabled
        if (mySelectedElement) {
            myEdge->unselectAttributeCarrier();
        }
        // restore container
        restoreHierarchicalContainers();
        // remove edge lanes from parents and children
        removeEdgeLanes();
        // delete edge from net
        myEdge->getNet()->getAttributeCarriers()->deleteSingleEdge(myEdge);
    } else {
        // show extra information for tests
        WRITE_DEBUG("Adding " + myEdge->getTagStr() + " '" + myEdge->getID() + "' from " + toString(SUMO_TAG_NET));
        // select if mySelectedElement is enabled
        if (mySelectedElement) {
            myEdge->selectAttributeCarrier();
        }
        // insert edge into net
        myEdge->getNet()->getAttributeCarriers()->insertEdge(myEdge);
        // restore container
        restoreHierarchicalContainers();
        // add edge lanes into parents and children
        addEdgeLanes();
    }
    // enable save networkElements
    myEdge->getNet()->getSavingStatus()->requireSaveNetwork();
}


void
GNEChange_Edge::redo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Adding " + myEdge->getTagStr() + " '" + myEdge->getID() + "' from " + toString(SUMO_TAG_NET));
        // select if mySelectedElement is enabled
        if (mySelectedElement) {
            myEdge->selectAttributeCarrier();
        }
        // insert edge into net
        myEdge->getNet()->getAttributeCarriers()->insertEdge(myEdge);
        // add edge into parents and children
        addElementInParentsAndChildren(myEdge);
        // add edge lanes into parents and children
        addEdgeLanes();
    } else {
        // show extra information for tests
        WRITE_DEBUG("Removing " + myEdge->getTagStr() + " '" + myEdge->getID() + "' from " + toString(SUMO_TAG_NET));
        // unselect if mySelectedElement is enabled
        if (mySelectedElement) {
            myEdge->unselectAttributeCarrier();
        }
        // remove edge from parents and children
        removeElementFromParentsAndChildren(myEdge);
        // remove edge lanes from parents and children
        removeEdgeLanes();
        // delete edge from net
        myEdge->getNet()->getAttributeCarriers()->deleteSingleEdge(myEdge);
    }
    // enable save networkElements
    myEdge->getNet()->getSavingStatus()->requireSaveNetwork();
}


std::string
GNEChange_Edge::undoName() const {
    if (myForward) {
        return (TL("Undo create edge '") + myEdge->getID() + "'");
    } else {
        return (TL("Undo delete edge '") + myEdge->getID() + "'");
    }
}


std::string
GNEChange_Edge::redoName() const {
    if (myForward) {
        return (TL("Redo create edge '") + myEdge->getID() + "'");
    } else {
        return (TL("Redo delete edge '") + myEdge->getID() + "'");
    }
}



void
GNEChange_Edge::addEdgeLanes() {
    // iterate over edge lanes
    for (int i = 0; i < (int)myEdge->getLanes().size(); i++) {
        // add lane's edge in parent elements
        for (const auto& additionalParent : myLaneParentAdditionals.at(i)) {
            additionalParent->addChildElement(myEdge->getLanes().at(i));
        }
        for (const auto& demandParent : myLaneParentDemandElements.at(i)) {
            demandParent->addChildElement(myEdge->getLanes().at(i));
        }
        for (const auto& genericParent : myLaneParentGenericData.at(i)) {
            genericParent->addChildElement(myEdge->getLanes().at(i));
        }
        // add lane's edge in child elements
        for (const auto& additionalChild : myChildLaneAdditionals.at(i)) {
            additionalChild->addParentElement(myEdge->getLanes().at(i));
        }
        for (const auto& demandChild : myChildLaneDemandElements.at(i)) {
            demandChild->addParentElement(myEdge->getLanes().at(i));
        }
        for (const auto& genericChild : myChildLaneGenericData.at(i)) {
            genericChild->addParentElement(myEdge->getLanes().at(i));
        }
    }
}


void
GNEChange_Edge::removeEdgeLanes() {
    // iterate over edge lanes
    for (int i = 0; i < (int)myEdge->getLanes().size(); i++) {
        // Remove every lane's edge from parent elements
        for (const auto& additionalParent : myLaneParentAdditionals.at(i)) {
            additionalParent->removeChildElement(myEdge->getLanes().at(i));
        }
        for (const auto& demandParent : myLaneParentDemandElements.at(i)) {
            demandParent->removeChildElement(myEdge->getLanes().at(i));
        }
        for (const auto& genericParent : myLaneParentGenericData.at(i)) {
            genericParent->removeChildElement(myEdge->getLanes().at(i));
        }
        // Remove every lane's edge from child elements
        for (const auto& additionalChild : myChildLaneAdditionals.at(i)) {
            additionalChild->removeParentElement(myEdge->getLanes().at(i));
        }
        for (const auto& demandChild : myChildLaneDemandElements.at(i)) {
            demandChild->removeParentElement(myEdge->getLanes().at(i));
        }
        for (const auto& genericChild : myChildLaneGenericData.at(i)) {
            genericChild->removeParentElement(myEdge->getLanes().at(i));
        }
    }
}
