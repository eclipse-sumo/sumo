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
/// @file    GNEChange_Junction.cpp
/// @author  Jakob Erdmann
/// @date    Mar 2011
///
// A network change in which a single junction is created or deleted
/****************************************************************************/

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>

#include "GNEChange_Junction.h"

// ===========================================================================
// FOX-declarations
// ===========================================================================

FXIMPLEMENT_ABSTRACT(GNEChange_Junction, GNEChange, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================


/// @brief constructor for creating a junction
GNEChange_Junction::GNEChange_Junction(GNEJunction* junction, bool forward):
    GNEChange(Supermode::NETWORK, junction, forward, junction->isAttributeCarrierSelected()),
    myJunction(junction) {
    junction->incRef("GNEChange_Junction");
}


GNEChange_Junction::~GNEChange_Junction() {
    // only continue we have undo-redo mode enabled
    if (myJunction->getNet()->getGNEApplicationWindow()->isUndoRedoAllowed()) {
        myJunction->decRef("GNEChange_Junction");
        if (myJunction->unreferenced()) {
            delete myJunction;
        }
    }
}


void
GNEChange_Junction::undo() {
    if (myForward) {
        // unselect if mySelectedElement is enabled
        if (mySelectedElement) {
            myJunction->unselectAttributeCarrier();
        }
        // delete junction from net
        myJunction->getNet()->getAttributeCarriers()->deleteSingleJunction(myJunction);
        // remove element from parent and children
        removeElementFromParentsAndChildren(myJunction);
    } else {
        // select if mySelectedElement is enabled
        if (mySelectedElement) {
            myJunction->selectAttributeCarrier();
        }
        // add element in parent and children
        addElementInParentsAndChildren(myJunction);
        // insert junction in net
        myJunction->getNet()->getAttributeCarriers()->insertJunction(myJunction);
    }
    // enable save networkElements
    myJunction->getNet()->getSavingStatus()->requireSaveNetwork();
}


void
GNEChange_Junction::redo() {
    if (myForward) {
        // select if mySelectedElement is enabled
        if (mySelectedElement) {
            myJunction->selectAttributeCarrier();
        }
        // add element in parent and children
        addElementInParentsAndChildren(myJunction);
        // add junction into net
        myJunction->getNet()->getAttributeCarriers()->insertJunction(myJunction);
    } else {
        // unselect if mySelectedElement is enabled
        if (mySelectedElement) {
            myJunction->unselectAttributeCarrier();
        }
        // add element in parent and children
        addElementInParentsAndChildren(myJunction);
        // delete junction from net
        myJunction->getNet()->getAttributeCarriers()->deleteSingleJunction(myJunction);
        // remove element from parent and children
        removeElementFromParentsAndChildren(myJunction);
    }
    // enable save networkElements
    myJunction->getNet()->getSavingStatus()->requireSaveNetwork();
}


std::string
GNEChange_Junction::undoName() const {
    if (myForward) {
        return (TL("Undo create junction '") + myJunction->getID() + "'");
    } else {
        return (TL("Undo delete junction '") + myJunction->getID() + "'");
    }
}


std::string
GNEChange_Junction::redoName() const {
    if (myForward) {
        return (TL("Redo create junction '") + myJunction->getID() + "'");
    } else {
        return (TL("Redo delete junction '") + myJunction->getID() + "'");
    }
}
