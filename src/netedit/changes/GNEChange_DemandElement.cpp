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
/// @file    GNEChange_DemandElement.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2019
///
// A network change in which a busStop is created or deleted
/****************************************************************************/
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
    myEdgePath(demandElement->getPathEdges()),
    myParentEdges(demandElement->getParentEdges()),
    myParentLanes(demandElement->getParentLanes()),
    myParentShapes(demandElement->getParentShapes()),
    myParentAdditionals(demandElement->getParentAdditionals()),
    myParentDemandElements(demandElement->getParentDemandElements()),
    myChildEdges(demandElement->getChildEdges()),
    myChildLanes(demandElement->getChildLanes()),
    myChildShapes(demandElement->getChildShapes()),
    myChildAdditionals(demandElement->getChildAdditionals()),
    myChildDemandElements(demandElement->getChildDemandElements()) {
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
            // remove element from path
            for (const auto& i : myEdgePath) {
                i->removePathElement(myDemandElement);
            }
            // Remove element from parent elements
            for (const auto& i : myParentEdges) {
                i->removeChildDemandElement(myDemandElement);
            }
            for (const auto& i : myParentLanes) {
                i->removeChildDemandElement(myDemandElement);
            }
            for (const auto& i : myParentShapes) {
                i->removeChildDemandElement(myDemandElement);
            }
            for (const auto& i : myParentAdditionals) {
                i->removeChildDemandElement(myDemandElement);
            }
            for (const auto& i : myParentDemandElements) {
                i->removeChildDemandElement(myDemandElement);
            }
            // Remove element from child elements
            for (const auto& i : myChildEdges) {
                i->removeParentDemandElement(myDemandElement);
            }
            for (const auto& i : myChildLanes) {
                i->removeParentDemandElement(myDemandElement);
            }
            for (const auto& i : myChildShapes) {
                i->removeParentDemandElement(myDemandElement);
            }
            for (const auto& i : myChildAdditionals) {
                i->removeParentDemandElement(myDemandElement);
            }
            for (const auto& i : myChildDemandElements) {
                i->removeParentDemandElement(myDemandElement);
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
        // remove element from path
        for (const auto& i : myEdgePath) {
            i->removePathElement(myDemandElement);
        }
        // Remove element from parent elements
        for (const auto& i : myParentEdges) {
            i->removeChildDemandElement(myDemandElement);
        }
        for (const auto& i : myParentLanes) {
            i->removeChildDemandElement(myDemandElement);
        }
        for (const auto& i : myParentShapes) {
            i->removeChildDemandElement(myDemandElement);
        }
        for (const auto& i : myParentAdditionals) {
            i->removeChildDemandElement(myDemandElement);
        }
        for (const auto& i : myParentDemandElements) {
            i->removeChildDemandElement(myDemandElement);
        }
        // Remove element from child elements
        for (const auto& i : myChildEdges) {
            i->removeParentDemandElement(myDemandElement);
        }
        for (const auto& i : myChildLanes) {
            i->removeParentDemandElement(myDemandElement);
        }
        for (const auto& i : myChildShapes) {
            i->removeParentDemandElement(myDemandElement);
        }
        for (const auto& i : myChildAdditionals) {
            i->removeParentDemandElement(myDemandElement);
        }
        for (const auto& i : myChildDemandElements) {
            i->removeParentDemandElement(myDemandElement);
        }
    } else {
        // show extra information for tests
        WRITE_DEBUG("Adding " + myDemandElement->getTagStr() + " '" + myDemandElement->getID() + "' in GNEChange_DemandElement");
        // insert demand element into net
        myNet->insertDemandElement(myDemandElement);
        // add element in parent elements
        for (const auto& i : myParentEdges) {
            i->addChildDemandElement(myDemandElement);
        }
        for (const auto& i : myParentLanes) {
            i->addChildDemandElement(myDemandElement);
        }
        for (const auto& i : myParentShapes) {
            i->addChildDemandElement(myDemandElement);
        }
        for (const auto& i : myParentAdditionals) {
            i->addChildDemandElement(myDemandElement);
        }
        for (const auto& i : myParentDemandElements) {
            i->addChildDemandElement(myDemandElement);
        }
        // add element in child elements
        for (const auto& i : myChildEdges) {
            i->addParentDemandElement(myDemandElement);
        }
        for (const auto& i : myChildLanes) {
            i->addParentDemandElement(myDemandElement);
        }
        for (const auto& i : myChildShapes) {
            i->addParentDemandElement(myDemandElement);
        }
        for (const auto& i : myChildAdditionals) {
            i->addParentDemandElement(myDemandElement);
        }
        for (const auto& i : myChildDemandElements) {
            i->addParentDemandElement(myDemandElement);
        }
    }
    // update vehicle type selector if demand element is a VType and vehicle type Frame is shown
    if ((myDemandElement->getTagProperty().getTag() == SUMO_TAG_VTYPE) && myNet->getViewNet()->getViewParent()->getVehicleTypeFrame()->shown()) {
        myNet->getViewNet()->getViewParent()->getVehicleTypeFrame()->getVehicleTypeSelector()->refreshVehicleTypeSelector();
    }
    // update stack labels
    if (myParentEdges.size() > 0) {
        myParentEdges.front()->updateVehicleStackLabels();
    }
    // Requiere always save elements
    myNet->requireSaveDemandElements(true);
}


void
GNEChange_DemandElement::redo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Adding " + myDemandElement->getTagStr() + " '" + myDemandElement->getID() + "' in GNEChange_DemandElement");
        // insert demand element into net
        myNet->insertDemandElement(myDemandElement);
        // add element in parent elements
        for (const auto& i : myParentEdges) {
            i->addChildDemandElement(myDemandElement);
        }
        for (const auto& i : myParentLanes) {
            i->addChildDemandElement(myDemandElement);
        }
        for (const auto& i : myParentShapes) {
            i->addChildDemandElement(myDemandElement);
        }
        for (const auto& i : myParentAdditionals) {
            i->addChildDemandElement(myDemandElement);
        }
        for (const auto& i : myParentDemandElements) {
            i->addChildDemandElement(myDemandElement);
        }
        // add element in child elements
        for (const auto& i : myChildEdges) {
            i->addParentDemandElement(myDemandElement);
        }
        for (const auto& i : myChildLanes) {
            i->addParentDemandElement(myDemandElement);
        }
        for (const auto& i : myChildShapes) {
            i->addParentDemandElement(myDemandElement);
        }
        for (const auto& i : myChildAdditionals) {
            i->addParentDemandElement(myDemandElement);
        }
        for (const auto& i : myChildDemandElements) {
            i->addParentDemandElement(myDemandElement);
        }
    } else {
        // show extra information for tests
        WRITE_DEBUG("Removing " + myDemandElement->getTagStr() + " '" + myDemandElement->getID() + "' in GNEChange_DemandElement");
        // delete demand element from net
        myNet->deleteDemandElement(myDemandElement, false);
        // remove element from path
        for (const auto& i : myEdgePath) {
            i->removePathElement(myDemandElement);
        }
        // Remove element from parent elements
        for (const auto& i : myParentEdges) {
            i->removeChildDemandElement(myDemandElement);
        }
        for (const auto& i : myParentLanes) {
            i->removeChildDemandElement(myDemandElement);
        }
        for (const auto& i : myParentShapes) {
            i->removeChildDemandElement(myDemandElement);
        }
        for (const auto& i : myParentAdditionals) {
            i->removeChildDemandElement(myDemandElement);
        }
        for (const auto& i : myParentDemandElements) {
            i->removeChildDemandElement(myDemandElement);
        }
        // Remove element from child elements
        for (const auto& i : myChildEdges) {
            i->removeParentDemandElement(myDemandElement);
        }
        for (const auto& i : myChildLanes) {
            i->removeParentDemandElement(myDemandElement);
        }
        for (const auto& i : myChildShapes) {
            i->removeParentDemandElement(myDemandElement);
        }
        for (const auto& i : myChildAdditionals) {
            i->removeParentDemandElement(myDemandElement);
        }
        for (const auto& i : myChildDemandElements) {
            i->removeParentDemandElement(myDemandElement);
        }
    }
    // update vehicle type selector if demand element is a VType and vehicle type Frame is shown
    if ((myDemandElement->getTagProperty().getTag() == SUMO_TAG_VTYPE) && myNet->getViewNet()->getViewParent()->getVehicleTypeFrame()->shown()) {
        myNet->getViewNet()->getViewParent()->getVehicleTypeFrame()->getVehicleTypeSelector()->refreshVehicleTypeSelector();
    }
    // update stack labels
    if (myParentEdges.size() > 0) {
        myParentEdges.front()->updateVehicleStackLabels();
    }
    // Requiere always save elements
    myNet->requireSaveDemandElements(true);
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
