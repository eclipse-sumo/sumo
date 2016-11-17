/****************************************************************************/
/// @file    GNEChange_Crossing.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2016
/// @version $Id$
///
// A network change in which a single junction is created or deleted
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2016 DLR (http://www.dlr.de/) and contributors
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

#include "GNEChange_Crossing.h"
#include "GNENet.h"
#include "GNECrossing.h"
#include "GNEEdge.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_Crossing, GNEChange, NULL, 0)

// ===========================================================================
// member method definitions
// ===========================================================================


// Constructor for creating an crossing
GNEChange_Crossing::GNEChange_Crossing(GNENet* net, GNECrossing* crossing, bool forward):
    GNEChange(net, forward),
    myCrossing(crossing) {
    assert(myNet);
    crossing->incRef("GNEChange_Crossing");
}


GNEChange_Crossing::~GNEChange_Crossing() {
    assert(myCrossing);
    myCrossing->decRef("GNEChange_Crossing");
    if (myCrossing->unreferenced()) {
        delete myCrossing;
    }
}


void GNEChange_Crossing::undo() {
    if (myForward) {
        ;//myNet->deleteSingleCrossing(myCrossing);
    } else {
        ;//myNet->insertCrossing(myCrossing);
    }
}


void GNEChange_Crossing::redo() {
    if (myForward) {
        ;//myNet->insertCrossing(myCrossing);
    } else {
        ;//myNet->deleteSingleCrossing(myCrossing);
    }
}


FXString GNEChange_Crossing::undoName() const {
    if (myForward) {
        return ("Undo create crossing");
    } else {
        return ("Undo delete crossing");
    }
}


FXString GNEChange_Crossing::redoName() const {
    if (myForward) {
        return ("Redo create crossing");
    } else {
        return ("Redo delete crossing");
    }
}
