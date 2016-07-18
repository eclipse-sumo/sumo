/****************************************************************************/
/// @file    GNEChange_Connection.cpp
/// @author  Jakob Erdmann
/// @date    May 2011
/// @version $Id$
///
// A network change in which a single connection is created or deleted
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

#include <cassert>
#include "GNEChange_Connection.h"
#include "GNEEdge.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_Connection, GNEChange, NULL, 0)

// ===========================================================================
// member method definitions
// ===========================================================================


// Constructor for creating an edge
GNEChange_Connection::GNEChange_Connection(GNEEdge* edge, int fromLane,
        const std::string& toEdgeID, int toLane,
        bool mayDefinitelyPass, bool forward):
    GNEChange(0, forward),
    myEdge(edge),
    myFromLane(fromLane),
    myToEdgeID(toEdgeID),
    myToLane(toLane),
    myPass(mayDefinitelyPass) {
    myEdge->incRef("GNEChange_Connection");
}


GNEChange_Connection::~GNEChange_Connection() {
    assert(myEdge);
    myEdge->decRef("GNEChange_Connection");
    if (myEdge->unreferenced()) {
        delete myEdge;
    }
}


void GNEChange_Connection::undo() {
    if (myForward) {
        myEdge->removeConnection(myFromLane, myToEdgeID, myToLane);
    } else {
        myEdge->addConnection(myFromLane, myToEdgeID, myToLane, myPass);
    }
}


void GNEChange_Connection::redo() {
    if (myForward) {
        myEdge->addConnection(myFromLane, myToEdgeID, myToLane, myPass);
    } else {
        myEdge->removeConnection(myFromLane, myToEdgeID, myToLane);
    }
}


FXString GNEChange_Connection::undoName() const {
    if (myForward) {
        return ("Undo create connection");
    } else {
        return ("Undo delete connection");
    }
}


FXString GNEChange_Connection::redoName() const {
    if (myForward) {
        return ("Redo create connection");
    } else {
        return ("Redo delete connection");
    }
}
