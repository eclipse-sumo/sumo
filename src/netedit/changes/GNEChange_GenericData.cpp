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
/// @file    GNEChange_GenericData.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// A network change in which a generic data set is created or deleted
/****************************************************************************/

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/elements/data/GNEDataInterval.h>

#include "GNEChange_GenericData.h"

// ===========================================================================
// FOX-declarations
// ===========================================================================

FXIMPLEMENT_ABSTRACT(GNEChange_GenericData, GNEChange, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

GNEChange_GenericData::GNEChange_GenericData(GNEGenericData* genericData, bool forward) :
    GNEChange(Supermode::DATA, genericData, forward, genericData->isAttributeCarrierSelected()),
    myGenericData(genericData),
    myDataIntervalParent(genericData->getDataIntervalParent()) {
    myGenericData->incRef("GNEChange_GenericData");
}


GNEChange_GenericData::~GNEChange_GenericData() {
    // only continue we have undo-redo mode enabled
    if (myGenericData->getNet()->getGNEApplicationWindow()->isUndoRedoAllowed()) {
        myGenericData->decRef("GNEChange_GenericData");
        if (myGenericData->unreferenced() &&
                myGenericData->getNet()->getAttributeCarriers()->retrieveDataInterval(myDataIntervalParent, false) &&
                myGenericData->getNet()->getAttributeCarriers()->retrieveGenericData(myGenericData, false)) {
            // delete generic data from interval parent
            myDataIntervalParent->removeGenericDataChild(myGenericData);
            // delete generic data
            delete myGenericData;
        }
    }
}


void
GNEChange_GenericData::undo() {
    if (myForward) {
        // unselect if mySelectedElement is enabled
        if (mySelectedElement) {
            myGenericData->unselectAttributeCarrier();
        }
        // delete generic data from interval parent
        myDataIntervalParent->removeGenericDataChild(myGenericData);
        // remove element from parent and children
        removeElementFromParentsAndChildren(myGenericData);
    } else {
        // select if mySelectedElement is enabled
        if (mySelectedElement) {
            myGenericData->selectAttributeCarrier();
        }
        // add element in parent and children
        addElementInParentsAndChildren(myGenericData);
        // insert generic data into interval parent
        myDataIntervalParent->addGenericDataChild(myGenericData);
    }
    // require always save elements
    myGenericData->getNet()->getSavingStatus()->requireSaveDataElements();
}


void
GNEChange_GenericData::redo() {
    if (myForward) {
        // select if mySelectedElement is enabled
        if (mySelectedElement) {
            myGenericData->selectAttributeCarrier();
        }
        // add element in parent and children
        addElementInParentsAndChildren(myGenericData);
        // insert generic data into interval parent
        myDataIntervalParent->addGenericDataChild(myGenericData);
    } else {
        // unselect if mySelectedElement is enabled
        if (mySelectedElement) {
            myGenericData->unselectAttributeCarrier();
        }
        // delete generic data from interval parent
        myDataIntervalParent->removeGenericDataChild(myGenericData);
        // remove element from parent and children
        removeElementFromParentsAndChildren(myGenericData);
    }
    // require always save elements
    myGenericData->getNet()->getSavingStatus()->requireSaveDataElements();
}


std::string
GNEChange_GenericData::undoName() const {
    if (myForward) {
        return (TL("Undo create ") + myGenericData->getTagStr());
    } else {
        return (TL("Undo delete ") + myGenericData->getTagStr());
    }
}


std::string
GNEChange_GenericData::redoName() const {
    if (myForward) {
        return (TL("Redo create ") + myGenericData->getTagStr());
    } else {
        return (TL("Redo delete ") + myGenericData->getTagStr());
    }
}
