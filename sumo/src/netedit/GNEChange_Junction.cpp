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

#include "GNEChange_Junction.h"
#include "GNENet.h"
#include "GNEJunction.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


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
        delete myJunction;
    }
}


void
GNEChange_Junction::undo() {
    if (myForward) {
        myNet->deleteSingleJunction(myJunction);
    } else {
        myNet->insertJunction(myJunction);
    }
}


void
GNEChange_Junction::redo() {
    if (myForward) {
        myNet->insertJunction(myJunction);
    } else {
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
