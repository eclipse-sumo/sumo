/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GNEChange_Distribution.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2019
///
// A change in which the distribution attribute of some object is modified
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>

#include "GNEChange_Distribution.h"

// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_Distribution, GNEChange, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

void
GNEChange_Distribution::addKey(GNEDemandElement* distribution, const GNEDemandElement* key, const double value, GNEUndoList* undoList) {
    // create change
    auto change = new GNEChange_Distribution(distribution, nullptr, key, 0, value);
    // add into undoList
    undoList->begin(distribution, TLF("add '%' key in % '%'", key, distribution->getTagStr(), distribution->getID()));
    undoList->add(change, true);
    undoList->end();
}


void
GNEChange_Distribution::removeKey(GNEDemandElement* distribution, const GNEDemandElement* key, GNEUndoList* undoList) {
    // create change
    auto change = new GNEChange_Distribution(distribution, key, nullptr, 0, 0);
    // add into undoList
    undoList->begin(distribution, TLF("remove '%' key from % '%'", key, distribution->getTagStr(), distribution->getID()));
    undoList->add(change, true);
    undoList->end();
}


void
GNEChange_Distribution::editValue(GNEDemandElement* distribution, const GNEDemandElement* key, const double newValue, GNEUndoList* undoList) {
    // create change
    auto change = new GNEChange_Distribution(distribution, key, key, distribution->getAttributeDistributionValue(key), newValue);
    // add into undoList
    undoList->begin(distribution, TLF("change '%' key value from % to %", key, newValue, newValue));
    undoList->add(change, true);
    undoList->end();
}


GNEChange_Distribution::~GNEChange_Distribution() {
    // decrease reference
    myDistribution->decRef("GNEChange_Distribution " + myDistribution->getTagProperty().getTagStr());
    // remove if is unreferenced
    if (myDistribution->unreferenced()) {
        // show extra information for tests
        WRITE_DEBUG("Deleting unreferenced " + myDistribution->getTagStr() + " '" + myDistribution->getID() + "' in GNEChange_Distribution");
        // delete distribution
        delete myDistribution;
    }
}


void
GNEChange_Distribution::undo() {
    // show extra information for tests
    WRITE_DEBUG("Setting previous distribution into " + myDistribution->getTagStr() + " '" + myDistribution->getID() + "'");
    // continue depending of key
    if (myOrigKey == nullptr) {
        myDistribution->removeDistributionKey(myNewKey);
    } else if (myNewKey == nullptr) {
        myDistribution->addDistributionKey(myOrigKey, myOrigValue);

    } else {
        myDistribution->editDistributionValue(myOrigKey, myNewValue);
    }
    // mark demand elements as unsaved
    myDistribution->getNet()->getSavingStatus()->requireSaveDemandElements();
}


void
GNEChange_Distribution::redo() {
    // show extra information for tests
    WRITE_DEBUG("Setting new distribution into " + myDistribution->getTagStr() + " '" + myDistribution->getID() + "'");
    // continue depending of key
    if (myOrigKey == nullptr) {
        myDistribution->addDistributionKey(myNewKey, myNewValue);
    } else if (myNewKey == nullptr) {
        myDistribution->removeDistributionKey(myNewKey);
    } else {
        myDistribution->editDistributionValue(myOrigKey, myNewValue);
    }
    // mark demand elements as unsaved
    myDistribution->getNet()->getSavingStatus()->requireSaveDemandElements();
}


std::string
GNEChange_Distribution::undoName() const {
    return TLF("Undo edit distribution in '%'", myDistribution->getID());
}


std::string
GNEChange_Distribution::redoName() const {
    return TLF("Redo edit distribution in '%'", myDistribution->getID());
}


GNEChange_Distribution::GNEChange_Distribution(GNEDemandElement* distribution, const GNEDemandElement* originalKey, const GNEDemandElement* newKey,
        const double originalValue, const double newValue) :
    GNEChange(Supermode::DEMAND, true, false),
    myDistribution(distribution),
    myOrigKey(originalKey),
    myNewKey(newKey),
    myOrigValue(originalValue),
    myNewValue(newValue) {
    myDistribution->incRef("GNEChange_Distribution " + myDistribution->getTagProperty().getTagStr());
}

/****************************************************************************/
