/****************************************************************************/
/// @file    GNEChange_Junction.cpp
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id$
///
// A network change in which a single junction is created or deleted
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
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
        if (myJunction->getNet()->getViewNet()->isTestingModeEnabled()) {
            WRITE_WARNING("Deleting unreferenced " + toString(myJunction->getTag()) + " '" + myJunction->getID() + "'");
        }
        delete myJunction;
    }
}


void
GNEChange_Junction::undo() {
    if (myForward) {
        // show extra information for tests
        if (myJunction->getNet()->getViewNet()->isTestingModeEnabled()) {
            WRITE_WARNING("Deleting " + toString(myJunction->getTag()) + " '" + myJunction->getID() + "'");
        }
        // add junction to net
        myNet->deleteSingleJunction(myJunction);
    } else {
        // show extra information for tests
        if (myJunction->getNet()->getViewNet()->isTestingModeEnabled()) {
            WRITE_WARNING("Adding " + toString(myJunction->getTag()) + " '" + myJunction->getID() + "'");
        }
        // delete junction from net
        myNet->insertJunction(myJunction);
    }
}


void
GNEChange_Junction::redo() {
    if (myForward) {
        // show extra information for tests
        if (myJunction->getNet()->getViewNet()->isTestingModeEnabled()) {
            WRITE_WARNING("Adding " + toString(myJunction->getTag()) + " '" + myJunction->getID() + "'");
        }
        // add junction into net
        myNet->insertJunction(myJunction);
    } else {
        // show extra information for tests
        if (myJunction->getNet()->getViewNet()->isTestingModeEnabled()) {
            WRITE_WARNING("Deleting " + toString(myJunction->getTag()) + " '" + myJunction->getID() + "'");
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
