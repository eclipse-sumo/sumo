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
/// @file    GNEChange_Junction.cpp
/// @author  Jakob Erdmann
/// @date    Mar 2011
///
// A network change in which a single junction is created or deleted
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/elements/network/GNEJunction.h>

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
    GNEChange(junction->getNet(), junction, junction, forward),
    myJunction(junction) {
    junction->incRef("GNEChange_Junction");
}


GNEChange_Junction::~GNEChange_Junction() {
    assert(myJunction);
    myJunction->decRef("GNEChange_Junction");
    if (myJunction->unreferenced()) {
        // show extra information for tests
        WRITE_DEBUG("Deleting unreferenced " + myJunction->getTagStr() + " '" + myJunction->getID() + "' in GNEChange_Junction");
        delete myJunction;
    }
}


void
GNEChange_Junction::undo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Removing " + myJunction->getTagStr() + " '" + myJunction->getID() + "' from " + toString(SUMO_TAG_NET));
        // add junction to net
        myNet->getAttributeCarriers()->deleteSingleJunction(myJunction);
    } else {
        // show extra information for tests
        WRITE_DEBUG("Adding " + myJunction->getTagStr() + " '" + myJunction->getID() + "' into " + toString(SUMO_TAG_NET));
        // delete junction from net
        myNet->getAttributeCarriers()->insertJunction(myJunction);
    }
    // enable save networkElements
    myNet->requireSaveNet(true);
}


void
GNEChange_Junction::redo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Adding " + myJunction->getTagStr() + " '" + myJunction->getID() + "' into " + toString(SUMO_TAG_NET));
        // add junction into net
        myNet->getAttributeCarriers()->insertJunction(myJunction);
    } else {
        // show extra information for tests
        WRITE_DEBUG("Removing " + myJunction->getTagStr() + " '" + myJunction->getID() + "' from " + toString(SUMO_TAG_NET));
        // delete junction from net
        myNet->getAttributeCarriers()->deleteSingleJunction(myJunction);
    }
    // enable save networkElements
    myNet->requireSaveNet(true);
}


FXString
GNEChange_Junction::undoName() const {
    if (myForward) {
        return ("Undo create " + toString(SUMO_TAG_JUNCTION)).c_str();
    } else {
        return ("Undo delete " + toString(SUMO_TAG_JUNCTION)).c_str();
    }
}


FXString
GNEChange_Junction::redoName() const {
    if (myForward) {
        return ("Redo create " + toString(SUMO_TAG_JUNCTION)).c_str();
    } else {
        return ("Redo delete " + toString(SUMO_TAG_JUNCTION)).c_str();
    }
}
