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
/// @file    GNEChange_GenericData.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// A network change in which a generic data set is created or deleted
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/elements/data/GNEGenericData.h>
#include <netedit/elements/data/GNEDataInterval.h>
#include <netedit/GNEViewNet.h>

#include "GNEChange_GenericData.h"

// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_GenericData, GNEChange, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

GNEChange_GenericData::GNEChange_GenericData(GNEGenericData* genericData, bool forward) :
    GNEChange(genericData->getViewNet()->getNet(), genericData, genericData, forward),
    myGenericData(genericData),
    myDataIntervalParent(genericData->getDataIntervalParent()) {
    myGenericData->incRef("GNEChange_GenericData");
}


GNEChange_GenericData::~GNEChange_GenericData() {
    assert(myGenericData);
    myGenericData->decRef("GNEChange_GenericData");
    if (myGenericData->unreferenced()) {
        // show extra information for tests
        WRITE_DEBUG("Deleting unreferenced " + myGenericData->getTagStr() + " '" + myGenericData->getID() + "'");
        // delete generic data
        delete myGenericData;
    }
}


void
GNEChange_GenericData::undo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Removing " + myGenericData->getTagStr() + " '" + myGenericData->getID() + "' in GNEChange_GenericData");
        // delete generic data from interval parent
        myDataIntervalParent->removeGenericDataChild(myGenericData);
        // remove genericData from parents and children
        removeGenericData(myGenericData);
    } else {
        // show extra information for tests
        WRITE_DEBUG("Adding " + myGenericData->getTagStr() + " '" + myGenericData->getID() + "' in GNEChange_GenericData");
        // insert generic data into interval parent
        myDataIntervalParent->addGenericDataChild(myGenericData);
        // add genericData in parents and children
        addGenericData(myGenericData);
    }
    // Requiere always save elements
    myNet->requireSaveDataElements(true);
}


void
GNEChange_GenericData::redo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Adding " + myGenericData->getTagStr() + " '" + myGenericData->getID() + "' in GNEChange_GenericData");
        // insert generic data into interval parent
        myDataIntervalParent->addGenericDataChild(myGenericData);
        // add genericData in parents and children
        addGenericData(myGenericData);
    } else {
        // show extra information for tests
        WRITE_DEBUG("Removing " + myGenericData->getTagStr() + " '" + myGenericData->getID() + "' in GNEChange_GenericData");
        // delete generic data from interval parent
        myDataIntervalParent->removeGenericDataChild(myGenericData);
        // remove genericData from parents and children
        removeGenericData(myGenericData);
    }
    // Requiere always save elements
    myNet->requireSaveDataElements(true);
}


FXString
GNEChange_GenericData::undoName() const {
    if (myForward) {
        return ("Undo create " + myGenericData->getTagStr()).c_str();
    } else {
        return ("Undo delete " + myGenericData->getTagStr()).c_str();
    }
}


FXString
GNEChange_GenericData::redoName() const {
    if (myForward) {
        return ("Redo create " + myGenericData->getTagStr()).c_str();
    } else {
        return ("Redo delete " + myGenericData->getTagStr()).c_str();
    }
}
