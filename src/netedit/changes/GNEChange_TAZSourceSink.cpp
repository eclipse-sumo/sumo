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
#include <config.h>

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
    if (mySourceSink->getNet()->getViewNet()->getViewParent()->getGNEAppWindows()->isUndoRedoAllowed()) {
        mySourceSink->decRef("GNEChange_TAZSourceSink");
        if (mySourceSink->unreferenced()) {
            // show extra information for tests
            WRITE_DEBUG("Deleting unreferenced " + mySourceSink->getTagStr());
            // make sure that sourceSink isn't in net before removing
            if (mySourceSink->getNet()->getAttributeCarriers()->retrieveAdditional(mySourceSink, false)) {
                // delete sourceSink from net
                mySourceSink->getNet()->getAttributeCarriers()->deleteAdditional(mySourceSink);
            }
            delete mySourceSink;
        }
    }
}


void
GNEChange_TAZSourceSink::undo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Removing " + mySourceSink->getTagStr() + " '" + mySourceSink->getID() + "' in GNEChange_TAZSourceSink");
        // unselect if mySelectedElement is enabled
        if (mySelectedElement) {
            mySourceSink->unselectAttributeCarrier();
        }
        // delete sourceSink from net
        mySourceSink->getNet()->getAttributeCarriers()->deleteAdditional(mySourceSink);
        // restore container
        restoreHierarchicalContainers();
    } else {
        // show extra information for tests
        WRITE_DEBUG("Adding " + mySourceSink->getTagStr() + " '" + mySourceSink->getID() + "' in GNEChange_TAZSourceSink");
        // select if mySelectedElement is enabled
        if (mySelectedElement) {
            mySourceSink->selectAttributeCarrier();
        }
        // insert sourceSink into net
        mySourceSink->getNet()->getAttributeCarriers()->insertAdditional(mySourceSink);
        // restore container
        restoreHierarchicalContainers();
    }
    // require always save sourceSinks
    mySourceSink->getNet()->getSavingStatus()->requireSaveAdditionals();
}


void
GNEChange_TAZSourceSink::redo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Adding " + mySourceSink->getTagStr() + " '" + mySourceSink->getID() + "' in GNEChange_TAZSourceSink");
        // select if mySelectedElement is enabled
        if (mySelectedElement) {
            mySourceSink->selectAttributeCarrier();
        }
        // insert sourceSink into net
        mySourceSink->getNet()->getAttributeCarriers()->insertAdditional(mySourceSink);
        // add sourceSink in parent elements
        addElementInParentsAndChildren(mySourceSink);
    } else {
        // show extra information for tests
        WRITE_DEBUG("Removing " + mySourceSink->getTagStr() + " '" + mySourceSink->getID() + "' in GNEChange_TAZSourceSink");
        // unselect if mySelectedElement is enabled
        if (mySelectedElement) {
            mySourceSink->unselectAttributeCarrier();
        }
        // delete sourceSink from net
        mySourceSink->getNet()->getAttributeCarriers()->deleteAdditional(mySourceSink);
        // remove sourceSink from parents and children
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
