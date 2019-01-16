/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChange_DemandElement.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2019
/// @version $Id$
///
// A network change in which a busStop is created or deleted
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/demandelements/GNEDemandElement.h>
#include <netedit/frames/GNEInspectorFrame.h>
#include <netedit/GNEViewParent.h>

#include "GNEChange_DemandElement.h"

// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_DemandElement, GNEChange, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

GNEChange_DemandElement::GNEChange_DemandElement(GNEDemandElement* demandElement, bool forward) :
    GNEChange(demandElement->getViewNet()->getNet(), forward),
    myDemandElement(demandElement),
    myFirstDemandElementParent(myDemandElement->getFirstDemandElementParent()),
    mySecondDemandElementParent(myDemandElement->getSecondDemandElementParent()),
    myEdgeChilds(myDemandElement->getEdgeChilds()),
    myLaneChilds(myDemandElement->getLaneChilds()) {
    myDemandElement->incRef("GNEChange_DemandElement");
    // handle demandElements with lane parent
    if (demandElement->getTagProperty().canBePlacedOverLane()) {
        myLaneParents.push_back(myNet->retrieveLane(myDemandElement->getAttribute(SUMO_ATTR_LANE)));
    }
    if (demandElement->getTagProperty().canBePlacedOverLanes()) {
        myLaneParents = GNEAttributeCarrier::parse<std::vector<GNELane*> >(demandElement->getViewNet()->getNet(), myDemandElement->getAttribute(SUMO_ATTR_LANES));
    }
    // handle demandElements with edge parent
    if (demandElement->getTagProperty().canBePlacedOverEdge()) {
        myEdgeParents.push_back(myNet->retrieveEdge(myDemandElement->getAttribute(SUMO_ATTR_EDGE)));
    }
    if (demandElement->getTagProperty().canBePlacedOverEdges()) {
        myEdgeParents = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(demandElement->getViewNet()->getNet(), myDemandElement->getAttribute(SUMO_ATTR_EDGES));
    }
}


GNEChange_DemandElement::~GNEChange_DemandElement() {
    assert(myDemandElement);
    myDemandElement->decRef("GNEChange_DemandElement");
    if (myDemandElement->unreferenced()) {
        // show extra information for tests
        WRITE_DEBUG("Deleting unreferenced " + myDemandElement->getTagStr() + " '" + myDemandElement->getID() + "'");
        // make sure that demand element isn't in net before removing
        if (myNet->demandElementExist(myDemandElement)) {
            myNet->deleteDemandElement(myDemandElement);
        }
        delete myDemandElement;
    }
}


void
GNEChange_DemandElement::undo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Removing " + myDemandElement->getTagStr() + " '" + myDemandElement->getID() + "' in GNEChange_DemandElement");
        // 1 - If demand element own a lane parent, remove it from lane
        for (auto i : myLaneParents) {
            i->removeDemandElementChild(myDemandElement);
        }
        // 2 - If demand element own a edge parent, remove it from edge
        for (auto i : myEdgeParents) {
            i->removeDemandElementChild(myDemandElement);
        }
        // 3 - If demand element has a first parent, remove it from their demand element childs
        if (myFirstDemandElementParent) {
            myFirstDemandElementParent->removeDemandElementChild(myDemandElement);
        }
        // 4 - If additiona has a second parent, remove it from their demand element childs
        if (mySecondDemandElementParent) {
            mySecondDemandElementParent->removeDemandElementChild(myDemandElement);
        }
        // 5 - if DemandElement has edge childs, remove it of their demand element parents
        for (auto i : myEdgeChilds) {
            i->removeDemandElementParent(myDemandElement);
        }
        // 6 - if DemandElement has lane childs, remove it of their demand element parents
        for (auto i : myLaneChilds) {
            i->removeDemandElementParent(myDemandElement);
        }
        // delete demand element of net
        myNet->deleteDemandElement(myDemandElement);
    } else {
        // show extra information for tests
        WRITE_DEBUG("Adding " + myDemandElement->getTagStr() + " '" + myDemandElement->getID() + "' in GNEChange_DemandElement");
        // insert demand element of test
        myNet->insertDemandElement(myDemandElement);
        // 1 - If demand element own a Lane parent, add it to lane
        for (auto i : myLaneParents) {
            i->addDemandElementChild(myDemandElement);
        }
        // 2 - If demand element own a edge parent, add it to edge
        for (auto i : myEdgeParents) {
            i->addDemandElementChild(myDemandElement);
        }
        // 3 - If demand element has a parent, add it into demand element parent
        if (myFirstDemandElementParent) {
            myFirstDemandElementParent->addDemandElementChild(myDemandElement);
        }
        // 4 - If demand element has a parent, add it into demand element parent
        if (mySecondDemandElementParent) {
            mySecondDemandElementParent->addDemandElementChild(myDemandElement);
        }
        // 5 - if DemandElement has edge childs, add id into demand element parents
        for (auto i : myEdgeChilds) {
            i->addDemandElementParent(myDemandElement);
        }
        // 6 - if DemandElement has lane childs, add id into demand element parents
        for (auto i : myLaneChilds) {
            i->addDemandElementParent(myDemandElement);
        }
    }
    // Requiere always save demandElements
    myNet->requiereSaveDemandElements(true);
    // check if inspector frame has to be updated
    if (myNet->getViewNet()->getViewParent()->getInspectorFrame()->shown()) {
        myNet->getViewNet()->getViewParent()->getInspectorFrame()->getACHierarchy()->refreshACHierarchy();
    }
}


void
GNEChange_DemandElement::redo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Adding " + myDemandElement->getTagStr() + " '" + myDemandElement->getID() + "' in GNEChange_DemandElement");
        // insert demand element into net
        myNet->insertDemandElement(myDemandElement);
        // 1 - If demand element own a Lane parent, add it to lane
        for (auto i : myLaneParents) {
            i->addDemandElementChild(myDemandElement);
        }
        // 2 - If demand element own a edge parent, add it to edge
        for (auto i : myEdgeParents) {
            i->addDemandElementChild(myDemandElement);
        }
        // 3 - If demand element has a parent, add it into demand element parent
        if (myFirstDemandElementParent) {
            myFirstDemandElementParent->addDemandElementChild(myDemandElement);
        }
        // 4 - If demand element has a parent, add it into demand element parent
        if (mySecondDemandElementParent) {
            mySecondDemandElementParent->addDemandElementChild(myDemandElement);
        }
        // 5 - if DemandElement has edge childs, add id into demand element parents
        for (auto i : myEdgeChilds) {
            i->addDemandElementParent(myDemandElement);
        }
        // 6 - if DemandElement has lane childs, add id into demand element parents
        for (auto i : myLaneChilds) {
            i->addDemandElementParent(myDemandElement);
        }
    } else {
        // show extra information for tests
        WRITE_DEBUG("Removing " + myDemandElement->getTagStr() + " '" + myDemandElement->getID() + "' in GNEChange_DemandElement");
        // 1 - If demand element own a lane parent, remove it from lane
        for (auto i : myLaneParents) {
            i->removeDemandElementChild(myDemandElement);
        }
        // 2 - If demand element own a edge parent, remove it from edge
        for (auto i : myEdgeParents) {
            i->removeDemandElementChild(myDemandElement);
        }
        // 3 - If additiona has a first parent, remove it from their demand element childs
        if (myFirstDemandElementParent) {
            myFirstDemandElementParent->removeDemandElementChild(myDemandElement);
        }
        // 4 - If additiona has a second parent, remove it from their demand element childs
        if (mySecondDemandElementParent) {
            mySecondDemandElementParent->removeDemandElementChild(myDemandElement);
        }
        // 5 - if DemandElement has edge childs, remove it of their demand element parents
        for (auto i : myEdgeChilds) {
            i->removeDemandElementParent(myDemandElement);
        }
        // 6 - if DemandElement has lane childs, remove it of their demand element parents
        for (auto i : myLaneChilds) {
            i->removeDemandElementParent(myDemandElement);
        }
        // delete demand element of net
        myNet->deleteDemandElement(myDemandElement);
    }
    // Requiere always save demandElements
    myNet->requiereSaveDemandElements(true);
    // check if inspector frame has to be updated
    if (myNet->getViewNet()->getViewParent()->getInspectorFrame()->shown()) {
        myNet->getViewNet()->getViewParent()->getInspectorFrame()->getACHierarchy()->refreshACHierarchy();
    }
}


FXString
GNEChange_DemandElement::undoName() const {
    if (myForward) {
        return ("Undo create " + myDemandElement->getTagStr()).c_str();
    } else {
        return ("Undo delete " + myDemandElement->getTagStr()).c_str();
    }
}


FXString
GNEChange_DemandElement::redoName() const {
    if (myForward) {
        return ("Redo create " + myDemandElement->getTagStr()).c_str();
    } else {
        return ("Redo delete " + myDemandElement->getTagStr()).c_str();
    }
}
