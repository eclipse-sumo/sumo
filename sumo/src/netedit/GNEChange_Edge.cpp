/****************************************************************************/
/// @file    GNEChange_Edge.cpp
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id$
///
// A network change in which a single junction is created or deleted
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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

#include "GNEChange_Edge.h"
#include "GNENet.h"
#include "GNEEdge.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_Edge, GNEChange, NULL, 0)

// ===========================================================================
// member method definitions
// ===========================================================================


// Constructor for creating an edge
GNEChange_Edge::GNEChange_Edge(GNENet* net, GNEEdge* edge, bool forward):
    GNEChange(net, forward),
    myEdge(edge) {
    assert(myNet);
    edge->incRef("GNEChange_Edge");
}


GNEChange_Edge::~GNEChange_Edge() {
    assert(myEdge);
    myEdge->decRef("GNEChange_Edge");
    if (myEdge->unreferenced()) {
        delete myEdge;
    }
}


void GNEChange_Edge::undo() {
    if (myForward) {
        myNet->deleteSingleEdge(myEdge);
    } else {
        myNet->insertEdge(myEdge);
    }
}


void GNEChange_Edge::redo() {
    if (myForward) {
        myNet->insertEdge(myEdge);
    } else {
        myNet->deleteSingleEdge(myEdge);
    }
}


FXString GNEChange_Edge::undoName() const {
    if (myForward) {
        return ("Undo create edge");
    } else {
        return ("Undo delete edge");
    }
}


FXString GNEChange_Edge::redoName() const {
    if (myForward) {
        return ("Redo create edge");
    } else {
        return ("Redo delete edge");
    }
}
