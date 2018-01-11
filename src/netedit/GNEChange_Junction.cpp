/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChange_Junction.cpp
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id$
///
// A network change in which a single junction is created or deleted
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/MsgHandler.h>

#include "GNEChange_Junction.h"
#include "GNENet.h"
#include "GNEJunction.h"
#include "GNEViewNet.h"


// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_Junction, GNEChange, NULL, 0)

// ===========================================================================
// member method definitions
// ===========================================================================


/// @brief constructor for creating a junction
GNEChange_Junction::GNEChange_Junction(GNEJunction* junction, bool forward):
    GNEChange(junction->getNet(), forward),
    myJunction(junction) {
    assert(myNet);
    junction->incRef("GNEChange_Junction");
}


GNEChange_Junction::~GNEChange_Junction() {
    assert(myJunction);
    myJunction->decRef("GNEChange_Junction");
    if (myJunction->unreferenced()) {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Deleting unreferenced " + toString(myJunction->getTag()) + " '" + myJunction->getID() + "' in GNEChange_Junction");
        }
        delete myJunction;
    }
}


void
GNEChange_Junction::undo() {
    if (myForward) {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Removing " + toString(myJunction->getTag()) + " '" + myJunction->getID() + "' from " + toString(SUMO_TAG_NET));
        }
        // add junction to net
        myNet->deleteSingleJunction(myJunction);
    } else {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Adding " + toString(myJunction->getTag()) + " '" + myJunction->getID() + "' into " + toString(SUMO_TAG_NET));
        }
        // delete junction from net
        myNet->insertJunction(myJunction);
    }
}


void
GNEChange_Junction::redo() {
    if (myForward) {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Adding " + toString(myJunction->getTag()) + " '" + myJunction->getID() + "' into " + toString(SUMO_TAG_NET));
        }
        // add junction into net
        myNet->insertJunction(myJunction);
    } else {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Removing " + toString(myJunction->getTag()) + " '" + myJunction->getID() + "' from " + toString(SUMO_TAG_NET));
        }
        // delete junction from net
        myNet->deleteSingleJunction(myJunction);
    }
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
