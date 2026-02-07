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
/// @file    GNEChange_Edge.cpp
/// @author  Jakob Erdmann
/// @date    Mar 2011
///
// A network change in which a single edge is created or deleted
/****************************************************************************/

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>

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
}


GNEChange_Edge::~GNEChange_Edge() {
    // only continue we have undo-redo mode enabled
    if (myEdge->getNet()->getGNEApplicationWindow()->isUndoRedoAllowed()) {
        myEdge->decRef("GNEChange_Edge");
        if (myEdge->unreferenced()) {
            // delete edge
            delete myEdge;
        }
    }
}


void
GNEChange_Edge::undo() {
    if (myForward) {
        // unselect if mySelectedElement is enabled
        if (mySelectedElement) {
            myEdge->unselectAttributeCarrier();
        }
        // delete edge from net
        myEdge->getNet()->getAttributeCarriers()->deleteSingleEdge(myEdge);
        // remove element from parent and children
        removeElementFromParentsAndChildren(myEdge);
    } else {
        // select if mySelectedElement is enabled
        if (mySelectedElement) {
            myEdge->selectAttributeCarrier();
        }
        // add element in parent and children
        addElementInParentsAndChildren(myEdge);
        // insert edge into net
        myEdge->getNet()->getAttributeCarriers()->insertEdge(myEdge);
    }
    // enable save networkElements
    myEdge->getNet()->getSavingStatus()->requireSaveNetwork();
}


void
GNEChange_Edge::redo() {
    if (myForward) {
        // select if mySelectedElement is enabled
        if (mySelectedElement) {
            myEdge->selectAttributeCarrier();
        }
        // add element in parent and children
        addElementInParentsAndChildren(myEdge);
        // insert edge into net
        myEdge->getNet()->getAttributeCarriers()->insertEdge(myEdge);
    } else {
        // unselect if mySelectedElement is enabled
        if (mySelectedElement) {
            myEdge->unselectAttributeCarrier();
        }
        // delete edge from net
        myEdge->getNet()->getAttributeCarriers()->deleteSingleEdge(myEdge);
        // remove element from parent and children
        removeElementFromParentsAndChildren(myEdge);
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
