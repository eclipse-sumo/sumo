/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
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
/// @file    GNEChange_DataInterval.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// A network change in which a data interval element is created or deleted
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
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
    myDataInterval->decRef("GNEChange_DataInterval");
    if (myDataInterval->unreferenced() &&
            myDataInterval->getNet()->getAttributeCarriers()->retrieveDataSet(myDataSetParent, false) &&
            myDataInterval->getNet()->getAttributeCarriers()->retrieveDataInterval(myDataInterval, false)) {
        // show extra information for tests
        WRITE_DEBUG("Deleting unreferenced " + myDataInterval->getTagStr() + " [" +
                    myDataInterval->getAttribute(SUMO_ATTR_BEGIN) + ", " +
                    myDataInterval->getAttribute(SUMO_ATTR_END) + "] in ~GNEChange_DataInterval()");
        // check that data interval don't exist
        myDataSetParent->removeDataIntervalChild(myDataInterval);
        // delete dataInterval
        delete myDataInterval;
    }
}


void
GNEChange_DataInterval::undo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Removing " + myDataInterval->getTagStr() + " [" +
                    myDataInterval->getAttribute(SUMO_ATTR_BEGIN) + ", " +
                    myDataInterval->getAttribute(SUMO_ATTR_END) + "] in GNEChange_DataInterval");
        // remove data interval from data set parent
        myDataSetParent->removeDataIntervalChild(myDataInterval);
    } else {
        // show extra information for tests
        WRITE_DEBUG("adding " + myDataInterval->getTagStr() + " [" +
                    myDataInterval->getAttribute(SUMO_ATTR_BEGIN) + ", " +
                    myDataInterval->getAttribute(SUMO_ATTR_END) + "] in GNEChange_DataInterval");
        // add data interval into data set parent
        myDataSetParent->addDataIntervalChild(myDataInterval);
    }
    // require always save elements
    myDataInterval->getDataSetParent()->getNet()->getSavingStatus()->requireSaveDataElements();
}


void
GNEChange_DataInterval::redo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("adding " + myDataInterval->getTagStr() + " [" +
                    myDataInterval->getAttribute(SUMO_ATTR_BEGIN) + ", " +
                    myDataInterval->getAttribute(SUMO_ATTR_END) + "] in GNEChange_DataInterval");
        // add data interval into data set parent
        myDataSetParent->addDataIntervalChild(myDataInterval);
    } else {
        // show extra information for tests
        WRITE_DEBUG("Removing " + myDataInterval->getTagStr() + " [" +
                    myDataInterval->getAttribute(SUMO_ATTR_BEGIN) + ", " +
                    myDataInterval->getAttribute(SUMO_ATTR_END) + "] in GNEChange_DataInterval");
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
