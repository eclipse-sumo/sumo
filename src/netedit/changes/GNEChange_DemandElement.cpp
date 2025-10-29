/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEChange_DemandElement.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2019
///
// A network change in which a busStop is created or deleted
/****************************************************************************/

#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/frames/demand/GNETypeFrame.h>

#include "GNEChange_DemandElement.h"

// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_DemandElement, GNEChange, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

GNEChange_DemandElement::GNEChange_DemandElement(GNEDemandElement* demandElement, bool forward) :
    GNEChange(Supermode::DEMAND, demandElement, forward, demandElement->isAttributeCarrierSelected()),
    myDemandElement(demandElement) {
    myDemandElement->incRef("GNEChange_DemandElement");
}


GNEChange_DemandElement::~GNEChange_DemandElement() {
    // only continue we have undo-redo mode enabled
    if (myDemandElement->getNet()->getViewNet()->getViewParent()->getGNEAppWindows()->isUndoRedoAllowed()) {
        myDemandElement->decRef("GNEChange_DemandElement");
        if (myDemandElement->unreferenced()) {
            // make sure that element isn't in net before removing
            if (myDemandElement->getNet()->getAttributeCarriers()->retrieveDemandElement(myDemandElement, false)) {
                // remove demand element of network
                myDemandElement->getNet()->getAttributeCarriers()->deleteDemandElement(myDemandElement, false);
            }
            delete myDemandElement;
        }
    }
}


void
GNEChange_DemandElement::undo() {
    if (myForward) {
        // unselect if mySelectedElement is enabled
        if (mySelectedElement) {
            myDemandElement->unselectAttributeCarrier();
        }
        // remove element from parent and children
        removeElementFromParentsAndChildren(myDemandElement);
        // delete demand element from net
        myDemandElement->getNet()->getAttributeCarriers()->deleteDemandElement(myDemandElement, true);
    } else {
        // select if mySelectedElement is enabled
        if (mySelectedElement) {
            myDemandElement->selectAttributeCarrier();
        }
        // add element in parent and children
        addElementInParentsAndChildren(myDemandElement);
        // insert demand element into net
        myDemandElement->getNet()->getAttributeCarriers()->insertDemandElement(myDemandElement);
    }
    // update vehicle type selector if demand element is a VType and vehicle type Frame is shown
    if ((myDemandElement->getTagProperty()->getTag() == SUMO_TAG_VTYPE) && myDemandElement->getNet()->getViewNet()->getViewParent()->getTypeFrame()->shown()) {
        myDemandElement->getNet()->getViewNet()->getViewParent()->getTypeFrame()->getTypeSelector()->refreshTypeSelector(true);
    }
    // update stack labels
    const auto parentEdges = myParents.get<GNEEdge*>();
    if (parentEdges.size() > 0) {
        parentEdges.front()->updateVehicleStackLabels();
        parentEdges.front()->updatePersonStackLabels();
        parentEdges.front()->updateContainerStackLabels();
    }
    // require always save elements
    myDemandElement->getNet()->getSavingStatus()->requireSaveDemandElements();
}


void
GNEChange_DemandElement::redo() {
    if (myForward) {
        // select if mySelectedElement is enabled
        if (mySelectedElement) {
            myDemandElement->selectAttributeCarrier();
        }
        // add element in parent and children
        addElementInParentsAndChildren(myDemandElement);
        // insert demand element into net
        myDemandElement->getNet()->getAttributeCarriers()->insertDemandElement(myDemandElement);
    } else {
        // unselect if mySelectedElement is enabled
        if (mySelectedElement) {
            myDemandElement->unselectAttributeCarrier();
        }
        // remove element from parent and children
        removeElementFromParentsAndChildren(myDemandElement);
        // delete demand element from net
        myDemandElement->getNet()->getAttributeCarriers()->deleteDemandElement(myDemandElement, true);
    }
    // update vehicle type selector if demand element is a VType and vehicle type Frame is shown
    if ((myDemandElement->getTagProperty()->getTag() == SUMO_TAG_VTYPE) && myDemandElement->getNet()->getViewNet()->getViewParent()->getTypeFrame()->shown()) {
        myDemandElement->getNet()->getViewNet()->getViewParent()->getTypeFrame()->getTypeSelector()->refreshTypeSelector(true);
    }
    // update stack labels
    const auto parentEdges = myParents.get<GNEEdge*>();
    if (parentEdges.size() > 0) {
        parentEdges.front()->updateVehicleStackLabels();
        parentEdges.front()->updatePersonStackLabels();
        parentEdges.front()->updateContainerStackLabels();
    }
    // require always save elements
    myDemandElement->getNet()->getSavingStatus()->requireSaveDemandElements();
}


std::string
GNEChange_DemandElement::undoName() const {
    if (myForward) {
        return (TL("Undo create ") + myDemandElement->getTagStr() + " '" + myDemandElement->getID() + "'");
    } else {
        return (TL("Undo delete ") + myDemandElement->getTagStr() + " '" + myDemandElement->getID() + "'");
    }
}


std::string
GNEChange_DemandElement::redoName() const {
    if (myForward) {
        return (TL("Redo create ") + myDemandElement->getTagStr() + " '" + myDemandElement->getID() + "'");
    } else {
        return (TL("Redo delete ") + myDemandElement->getTagStr() + " '" + myDemandElement->getID() + "'");
    }
}
