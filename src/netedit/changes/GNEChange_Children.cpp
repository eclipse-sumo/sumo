/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    GNEChange_Children.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2019
///
// A network change used to modify sorting of hierarchical element children
/****************************************************************************/

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>

#include "GNEChange_Children.h"

// ===========================================================================
// FOX-declarations
// ===========================================================================

FXIMPLEMENT_ABSTRACT(GNEChange_Children, GNEChange, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

GNEChange_Children::GNEChange_Children(GNEDemandElement* demandElementParent, GNEDemandElement* demandElementChild, const Operation operation) :
    GNEChange(Supermode::DEMAND, true, demandElementChild->isAttributeCarrierSelected()),
    myParentDemandElement(demandElementParent),
    myChildDemandElement(demandElementChild),
    myOperation(operation),
    myOriginalChildElements(demandElementParent->getChildDemandElements()),
    myEditedChildElements(demandElementParent->getChildDemandElements()) {
    myParentDemandElement->incRef("GNEChange_Children");
    // obtain iterator to demandElementChild
    auto it = std::find(myEditedChildElements.begin(), myEditedChildElements.end(), myChildDemandElement);
    // edit myEditedChildElements vector
    if (it != myEditedChildElements.end()) {
        if ((operation == Operation::MOVE_FRONT) && (it != (myEditedChildElements.end() - 1))) {
            // remove element
            it = myEditedChildElements.erase(it);
            // insert again in a different position
            myEditedChildElements.insert(it + 1, myChildDemandElement);
        } else if ((operation == Operation::MOVE_BACK) && (it != myEditedChildElements.begin())) {
            // remove element
            it = myEditedChildElements.erase(it);
            // insert again in a different position
            myEditedChildElements.insert(it - 1, myChildDemandElement);
        }
    }
}


GNEChange_Children::~GNEChange_Children() {
    // only continue we have undo-redo mode enabled
    if (myParentDemandElement->getNet()->getGNEApplicationWindow()->isUndoRedoAllowed()) {
        myParentDemandElement->decRef("GNEChange_Children");
        // remove if is unreferenced
        if (myParentDemandElement->unreferenced()) {
            // delete AC
            delete myParentDemandElement;
        }
    }
}


void
GNEChange_Children::undo() {
    if (myForward) {
        // continue depending of myOperation
        if (myOperation == GNEChange_Children::Operation::MOVE_FRONT) {
            // restore child demand element original vector in myChildDemandElement
//          myParentDemandElement->myChildDemandElements = myOriginalChildElements;
        } else if (myOperation == GNEChange_Children::Operation::MOVE_BACK) {
            // restore child demand element original vector in myChildDemandElement
//          myParentDemandElement->myChildDemandElements = myOriginalChildElements;
        }
    } else {
        // continue depending of myOperation
        if (myOperation == GNEChange_Children::Operation::MOVE_FRONT) {
            // set child demand element edited vector in myChildDemandElement
//          myParentDemandElement- myChildDemandElements = myEditedChildElements;
        } else if (myOperation == GNEChange_Children::Operation::MOVE_BACK) {
            // set child demand element edited vector in myChildDemandElement
//          myParentDemandElement->myChildDemandElements = myEditedChildElements;
        }
    }
    // require always save children
    myParentDemandElement->getNet()->getSavingStatus()->requireSaveDemandElements();
}


void
GNEChange_Children::redo() {
    if (myForward) {
        // continue depending of myOperation
        if (myOperation == GNEChange_Children::Operation::MOVE_FRONT) {
            // set child demand element edited vector in myChildDemandElement
//          myParentDemandElement->myChildDemandElements = myEditedChildElements;
        } else if (myOperation == GNEChange_Children::Operation::MOVE_BACK) {
            // set child demand element edited vector in myChildDemandElement
//          myParentDemandElement->myChildDemandElements = myEditedChildElements;
        }
    } else {
        // continue depending of myOperation
        if (myOperation == GNEChange_Children::Operation::MOVE_FRONT) {
            // restore child demand element original vector in myChildDemandElement
//          myParentDemandElement->myChildDemandElements = myOriginalChildElements;
        } else if (myOperation == GNEChange_Children::Operation::MOVE_BACK) {
            // restore child demand element original vector in myChildDemandElement
//          myParentDemandElement->myChildDemandElements = myOriginalChildElements;
        }
    }
    // require always save children
    myParentDemandElement->getNet()->getSavingStatus()->requireSaveDemandElements();
}


std::string
GNEChange_Children::undoName() const {
    if (myForward) {
        // check myOperation
        if (myOperation == GNEChange_Children::Operation::MOVE_FRONT) {
            return (TL("Undo moving up ") + myChildDemandElement->getTagStr());
        } else if (myOperation == GNEChange_Children::Operation::MOVE_BACK) {
            return (TL("Undo moving down ") + myChildDemandElement->getTagStr());
        } else {
            return (TL("Invalid operation"));
        }
    } else {
        // check myOperation
        if (myOperation == GNEChange_Children::Operation::MOVE_FRONT) {
            return (TL("Undo moving down ") + myChildDemandElement->getTagStr());
        } else if (myOperation == GNEChange_Children::Operation::MOVE_BACK) {
            return (TL("Undo moving up ") + myChildDemandElement->getTagStr());
        } else {
            return (TL("Invalid operation"));
        }
    }
}


std::string
GNEChange_Children::redoName() const {
    if (myForward) {
        // check myOperation
        if (myOperation == GNEChange_Children::Operation::MOVE_FRONT) {
            return (TL("Redo moving front ") + myParentDemandElement->getTagStr());
        } else if (myOperation == GNEChange_Children::Operation::MOVE_BACK) {
            return (TL("Redo moving back ") + myParentDemandElement->getTagStr());
        } else {
            return (TL("Invalid operation"));
        }
    } else {
        // check myOperation
        if (myOperation == GNEChange_Children::Operation::MOVE_FRONT) {
            return (TL("Redo moving front ") + myParentDemandElement->getTagStr());
        } else if (myOperation == GNEChange_Children::Operation::MOVE_BACK) {
            return (TL("Redo moving back ") + myParentDemandElement->getTagStr());
        } else {
            return (TL("Invalid operation"));
        }
    }
}
