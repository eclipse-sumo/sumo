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
/// @file    GNEChange_TAZSourceSink.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
// A network change in which a busStop is created or deleted
/****************************************************************************/

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>

#include "GNEChange_TAZSourceSink.h"

// ===========================================================================
// FOX-declarations
// ===========================================================================

FXIMPLEMENT_ABSTRACT(GNEChange_TAZSourceSink, GNEChange, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

GNEChange_TAZSourceSink::GNEChange_TAZSourceSink(GNETAZSourceSink* sourceSink, bool forward) :
    GNEChange(Supermode::NETWORK, sourceSink, forward, sourceSink->isAttributeCarrierSelected()),
    mySourceSink(sourceSink) {
    mySourceSink->incRef("GNEChange_TAZSourceSink");
}


GNEChange_TAZSourceSink::~GNEChange_TAZSourceSink() {
    // only continue we have undo-redo mode enabled
    if (mySourceSink->getNet()->getGNEApplicationWindow()->isUndoRedoAllowed()) {
        mySourceSink->decRef("GNEChange_TAZSourceSink");
        if (mySourceSink->unreferenced()) {
            // make sure that sourceSink isn't in net before removing
            if (mySourceSink->getNet()->getAttributeCarriers()->retrieveTAZSourceSink(mySourceSink, false)) {
                // delete sourceSink from net
                mySourceSink->getNet()->getAttributeCarriers()->deleteTAZSourceSink(mySourceSink);
            }
            delete mySourceSink;
        }
    }
}


void
GNEChange_TAZSourceSink::undo() {
    if (myForward) {
        // delete sourceSink from net
        mySourceSink->getNet()->getAttributeCarriers()->deleteTAZSourceSink(mySourceSink);
        // remove element from parent and children
        removeElementFromParentsAndChildren(mySourceSink);
    } else {
        // add element in parent and children
        addElementInParentsAndChildren(mySourceSink);
        // insert sourceSink into net
        mySourceSink->getNet()->getAttributeCarriers()->insertTAZSourceSink(mySourceSink);
    }
    // require always save sourceSinks
    mySourceSink->getNet()->getSavingStatus()->requireSaveAdditionals();
}


void
GNEChange_TAZSourceSink::redo() {
    if (myForward) {
        // select if mySelectedElement is enabled
        if (mySelectedElement) {
            mySourceSink->selectAttributeCarrier();
        }
        // add element in parent and children
        addElementInParentsAndChildren(mySourceSink);
        // insert sourceSink into net
        mySourceSink->getNet()->getAttributeCarriers()->insertTAZSourceSink(mySourceSink);
    } else {
        // delete sourceSink from net
        mySourceSink->getNet()->getAttributeCarriers()->deleteTAZSourceSink(mySourceSink);
        // remove element from parent and children
        removeElementFromParentsAndChildren(mySourceSink);
    }
    // require always save sourceSinks
    mySourceSink->getNet()->getSavingStatus()->requireSaveAdditionals();
}


std::string
GNEChange_TAZSourceSink::undoName() const {
    if (myForward) {
        return (TL("Undo create ") + mySourceSink->getTagStr() + " '" + mySourceSink->getID() + "'");
    } else {
        return (TL("Undo delete ") + mySourceSink->getTagStr() + " '" + mySourceSink->getID() + "'");
    }
}


std::string
GNEChange_TAZSourceSink::redoName() const {
    if (myForward) {
        return (TL("Redo create ") + mySourceSink->getTagStr() + " '" + mySourceSink->getID() + "'");
    } else {
        return (TL("Redo delete ") + mySourceSink->getTagStr() + " '" + mySourceSink->getID() + "'");
    }
}
