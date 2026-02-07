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
/// @file    GNEChange_DataInterval.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// A network change in which a data interval element is created or deleted
/****************************************************************************/

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/elements/data/GNEDataInterval.h>

#include "GNEChange_DataInterval.h"

// ===========================================================================
// FOX-declarations
// ===========================================================================

FXIMPLEMENT_ABSTRACT(GNEChange_DataInterval, GNEChange, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

GNEChange_DataInterval::GNEChange_DataInterval(GNEDataInterval* dataInterval, bool forward) :
    GNEChange(Supermode::DATA, forward, dataInterval->isAttributeCarrierSelected()),
    myDataInterval(dataInterval),
    myDataSetParent(dataInterval->getDataSetParent()) {
    myDataInterval->incRef("GNEChange_DataInterval");
}


GNEChange_DataInterval::~GNEChange_DataInterval() {
    // only continue we have undo-redo mode enabled
    if (myDataInterval->getNet()->getGNEApplicationWindow()->isUndoRedoAllowed()) {
        myDataInterval->decRef("GNEChange_DataInterval");
        if (myDataInterval->unreferenced() &&
                myDataInterval->getNet()->getAttributeCarriers()->retrieveDataSet(myDataSetParent->getID(), false) &&
                myDataInterval->getNet()->getAttributeCarriers()->retrieveDataInterval(myDataInterval, false)) {
            // check that data interval don't exist
            myDataSetParent->removeDataIntervalChild(myDataInterval);
            // delete dataInterval
            delete myDataInterval;
        }
    }
}


void
GNEChange_DataInterval::undo() {
    if (myForward) {
        // remove data interval from data set parent
        myDataSetParent->removeDataIntervalChild(myDataInterval);
    } else {
        // add data interval into data set parent
        myDataSetParent->addDataIntervalChild(myDataInterval);
    }
    // require always save elements
    myDataInterval->getDataSetParent()->getNet()->getSavingStatus()->requireSaveDataElements();
}


void
GNEChange_DataInterval::redo() {
    if (myForward) {
        // add data interval into data set parent
        myDataSetParent->addDataIntervalChild(myDataInterval);
    } else {
        // remove data interval from data set parent
        myDataSetParent->removeDataIntervalChild(myDataInterval);
    }
    // require always save elements
    myDataInterval->getDataSetParent()->getNet()->getSavingStatus()->requireSaveDataElements();
}


std::string
GNEChange_DataInterval::undoName() const {
    if (myForward) {
        return (TL("Undo create ") + myDataInterval->getTagStr() + " '" + myDataInterval->getID() + "'");
    } else {
        return (TL("Undo delete ") + myDataInterval->getTagStr() + " '" + myDataInterval->getID() + "'");
    }
}


std::string
GNEChange_DataInterval::redoName() const {
    if (myForward) {
        return (TL("Redo create ") + myDataInterval->getTagStr() + " '" + myDataInterval->getID() + "'");
    } else {
        return (TL("Redo delete ") + myDataInterval->getTagStr() + " '" + myDataInterval->getID() + "'");
    }
}


/****************************************************************************/
