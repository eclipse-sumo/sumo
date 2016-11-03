/****************************************************************************/
/// @file    GNEChange_Additional.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
/// A network change in which a busStop is created or deleted
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2016 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
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

#include "GNEChange_Additional.h"
#include "GNENet.h"
#include "GNEAdditional.h"
#include "GNEViewNet.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_Additional, GNEChange, NULL, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

GNEChange_Additional::GNEChange_Additional(GNENet* net, GNEAdditional* additional, bool forward) :
    GNEChange(net, forward),
    myAdditional(additional) {
    assert(myNet);
    myAdditional->incRef("GNEChange_Additional");
}


GNEChange_Additional::~GNEChange_Additional() {
    assert(myAdditional);
    myAdditional->decRef("GNEChange_Additional");
    if (myAdditional->unreferenced()) {
        delete myAdditional;
    }
}


void GNEChange_Additional::undo() {
    if (myForward) {
        myNet->deleteAdditional(myAdditional);
    } else {
        myNet->insertAdditional(myAdditional);
    }
}


void GNEChange_Additional::redo() {
    if (myForward) {
        myNet->insertAdditional(myAdditional);
    } else {
        myNet->deleteAdditional(myAdditional);
    }
}


FXString GNEChange_Additional::undoName() const {
    if (myForward) {
        return ("Undo create additional");
    } else {
        return ("Undo delete additional");
    }
}


FXString GNEChange_Additional::redoName() const {
    if (myForward) {
        return ("Redo create additional");
    } else {
        return ("Redo delete additional");
    }
}
