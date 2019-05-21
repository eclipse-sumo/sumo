/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
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
#include <netedit/additionals/GNEShape.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/demandelements/GNEDemandElement.h>
#include <netedit/frames/GNEVehicleTypeFrame.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEViewNet.h>

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
    myEdgeParents(demandElement->getEdgeParents()),
    myLaneParents(demandElement->getLaneParents()),
    myShapeParents(demandElement->getShapeParents()),
    myAdditionalParents(demandElement->getAdditionalParents()),
    myDemandElementParents(demandElement->getDemandElementParents()),
    myEdgeChilds(demandElement->getEdgeChilds()),
    myLaneChilds(demandElement->getLaneChilds()),
    myShapeChilds(demandElement->getShapeChilds()),
    myAdditionalChilds(demandElement->getAdditionalChilds()),
    myDemandElementChilds(demandElement->getDemandElementChilds()),
    myXMLChild(demandElement->getXMLChild()) {
    myDemandElement->incRef("GNEChange_DemandElement");
}


GNEChange_DemandElement::~GNEChange_DemandElement() {
    assert(myDemandElement);
    myDemandElement->decRef("GNEChange_DemandElement");
    if (myDemandElement->unreferenced()) {
        // show extra information for tests
        WRITE_DEBUG("Deleting unreferenced " + myDemandElement->getTagStr() + " '" + myDemandElement->getID() + "'");
        // make sure that element isn't in net before removing
        if (myNet->demandElementExist(myDemandElement)) {
            myNet->deleteDemandElement(myDemandElement, false);
            // Remove element from parent elements
            for (const auto& i : myEdgeParents) {
                i->removeDemandElementChild(myDemandElement);
            }
            for (const auto& i : myLaneParents) {
                i->removeDemandElementChild(myDemandElement);
            }
            for (const auto& i : myShapeParents) {
                i->removeDemandElementChild(myDemandElement);
            }
            for (const auto& i : myAdditionalParents) {
                i->removeDemandElementChild(myDemandElement);
            }
            for (const auto& i : myDemandElementParents) {
                i->removeDemandElementChild(myDemandElement);
            }
            // Remove element from child elements
            for (const auto& i : myEdgeChilds) {
                i->removeDemandElementParent(myDemandElement);
            }
            for (const auto& i : myLaneChilds) {
                i->removeDemandElementParent(myDemandElement);
            }
            for (const auto& i : myShapeChilds) {
                i->removeDemandElementParent(myDemandElement);
            }
            for (const auto& i : myAdditionalChilds) {
                i->removeDemandElementParent(myDemandElement);
            }
            for (const auto& i : myDemandElementChilds) {
                i->removeDemandElementParent(myDemandElement);
            }

        }
        delete myDemandElement;
    }
}


void
GNEChange_DemandElement::undo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Removing " + myDemandElement->getTagStr() + " '" + myDemandElement->getID() + "' in GNEChange_DemandElement");
        // delete demand element from net
        myNet->deleteDemandElement(myDemandElement, false);
        // Remove element from parent elements
        for (const auto& i : myEdgeParents) {
            i->removeDemandElementChild(myDemandElement);
        }
        for (const auto& i : myLaneParents) {
            i->removeDemandElementChild(myDemandElement);
        }
        for (const auto& i : myShapeParents) {
            i->removeDemandElementChild(myDemandElement);
        }
        for (const auto& i : myAdditionalParents) {
            i->removeDemandElementChild(myDemandElement);
        }
        for (const auto& i : myDemandElementParents) {
            i->removeDemandElementChild(myDemandElement);
        }
        // Remove element from child elements
        for (const auto& i : myEdgeChilds) {
            i->removeDemandElementParent(myDemandElement);
        }
        for (const auto& i : myLaneChilds) {
            i->removeDemandElementParent(myDemandElement);
        }
        for (const auto& i : myShapeChilds) {
            i->removeDemandElementParent(myDemandElement);
        }
        for (const auto& i : myAdditionalChilds) {
            i->removeDemandElementParent(myDemandElement);
        }
        for (const auto& i : myDemandElementChilds) {
            i->removeDemandElementParent(myDemandElement);
        }
        // update vehicle type selector if demand element is a VType and vehicle type Frame is shown
        if ((myDemandElement->getTagProperty().getTag() == SUMO_TAG_VTYPE) && myNet->getViewNet()->getViewParent()->getVehicleTypeFrame()->shown()) {
            myNet->getViewNet()->getViewParent()->getVehicleTypeFrame()->getVehicleTypeSelector()->refreshVehicleTypeSelector();
        }
    } else {
        // show extra information for tests
        WRITE_DEBUG("Adding " + myDemandElement->getTagStr() + " '" + myDemandElement->getID() + "' in GNEChange_DemandElement");
        // insert demand element into net
        myNet->insertDemandElement(myDemandElement);
        // add element in parent elements
        for (const auto& i : myEdgeParents) {
            i->addDemandElementChild(myDemandElement);
        }
        for (const auto& i : myLaneParents) {
            i->addDemandElementChild(myDemandElement);
        }
        for (const auto& i : myShapeParents) {
            i->addDemandElementChild(myDemandElement);
        }
        for (const auto& i : myAdditionalParents) {
            i->addDemandElementChild(myDemandElement);
        }
        for (const auto& i : myDemandElementParents) {
            i->addDemandElementChild(myDemandElement);
        }
        // add element in child elements
        for (const auto& i : myEdgeChilds) {
            i->addDemandElementParent(myDemandElement);
        }
        for (const auto& i : myLaneChilds) {
            i->addDemandElementParent(myDemandElement);
        }
        for (const auto& i : myShapeChilds) {
            i->addDemandElementParent(myDemandElement);
        }
        for (const auto& i : myAdditionalChilds) {
            i->addDemandElementParent(myDemandElement);
        }
        for (const auto& i : myDemandElementChilds) {
            i->addDemandElementParent(myDemandElement);
        }
    }
    // update vehicle type selector if demand element is a VType and vehicle type Frame is shown
    if ((myDemandElement->getTagProperty().getTag() == SUMO_TAG_VTYPE) && myNet->getViewNet()->getViewParent()->getVehicleTypeFrame()->shown()) {
        myNet->getViewNet()->getViewParent()->getVehicleTypeFrame()->getVehicleTypeSelector()->refreshVehicleTypeSelector();
    }
    // Requiere always save elements
    myNet->requiereSaveDemandElements(true);
}


void
GNEChange_DemandElement::redo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Adding " + myDemandElement->getTagStr() + " '" + myDemandElement->getID() + "' in GNEChange_DemandElement");
        // insert demand element into net
        myNet->insertDemandElement(myDemandElement);
        // add element in parent elements
        for (const auto& i : myEdgeParents) {
            i->addDemandElementChild(myDemandElement);
        }
        for (const auto& i : myLaneParents) {
            i->addDemandElementChild(myDemandElement);
        }
        for (const auto& i : myShapeParents) {
            i->addDemandElementChild(myDemandElement);
        }
        for (const auto& i : myAdditionalParents) {
            i->addDemandElementChild(myDemandElement);
        }
        for (const auto& i : myDemandElementParents) {
            i->addDemandElementChild(myDemandElement);
        }
        // add element in child elements
        for (const auto& i : myEdgeChilds) {
            i->addDemandElementParent(myDemandElement);
        }
        for (const auto& i : myLaneChilds) {
            i->addDemandElementParent(myDemandElement);
        }
        for (const auto& i : myShapeChilds) {
            i->addDemandElementParent(myDemandElement);
        }
        for (const auto& i : myAdditionalChilds) {
            i->addDemandElementParent(myDemandElement);
        }
        for (const auto& i : myDemandElementChilds) {
            i->addDemandElementParent(myDemandElement);
        }
        // update vehicle type selector if demand element is a VType and vehicle type Frame is shown
        if ((myDemandElement->getTagProperty().getTag() == SUMO_TAG_VTYPE) && myNet->getViewNet()->getViewParent()->getVehicleTypeFrame()->shown()) {
            myNet->getViewNet()->getViewParent()->getVehicleTypeFrame()->getVehicleTypeSelector()->refreshVehicleTypeSelector();
        }
    } else {
        // show extra information for tests
        WRITE_DEBUG("Removing " + myDemandElement->getTagStr() + " '" + myDemandElement->getID() + "' in GNEChange_DemandElement");
        // delete demand element from net
        myNet->deleteDemandElement(myDemandElement, false);
        // Remove element from parent elements
        for (const auto& i : myEdgeParents) {
            i->removeDemandElementChild(myDemandElement);
        }
        for (const auto& i : myLaneParents) {
            i->removeDemandElementChild(myDemandElement);
        }
        for (const auto& i : myShapeParents) {
            i->removeDemandElementChild(myDemandElement);
        }
        for (const auto& i : myAdditionalParents) {
            i->removeDemandElementChild(myDemandElement);
        }
        for (const auto& i : myDemandElementParents) {
            i->removeDemandElementChild(myDemandElement);
        }
        // Remove element from child elements
        for (const auto& i : myEdgeChilds) {
            i->removeDemandElementParent(myDemandElement);
        }
        for (const auto& i : myLaneChilds) {
            i->removeDemandElementParent(myDemandElement);
        }
        for (const auto& i : myShapeChilds) {
            i->removeDemandElementParent(myDemandElement);
        }
        for (const auto& i : myAdditionalChilds) {
            i->removeDemandElementParent(myDemandElement);
        }
        for (const auto& i : myDemandElementChilds) {
            i->removeDemandElementParent(myDemandElement);
        }
    }
    // update vehicle type selector if demand element is a VType and vehicle type Frame is shown
    if ((myDemandElement->getTagProperty().getTag() == SUMO_TAG_VTYPE) && myNet->getViewNet()->getViewParent()->getVehicleTypeFrame()->shown()) {
        myNet->getViewNet()->getViewParent()->getVehicleTypeFrame()->getVehicleTypeSelector()->refreshVehicleTypeSelector();
    }
    // Requiere always save elements
    myNet->requiereSaveDemandElements(true);
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
