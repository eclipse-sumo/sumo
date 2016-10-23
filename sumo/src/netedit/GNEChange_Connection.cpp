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
#include "GNEConnection.h"
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


GNEChange_Connection::GNEChange_Connection(GNEConnection *connection, bool forward) :
    GNEChange(connection->getNet(), forward),
    myConnection(connection) {
    assert(connection);
    myConnection->incRef("GNEChange_Connection");
}


GNEChange_Connection::~GNEChange_Connection() {
    assert(myConnection);
    myConnection->decRef("GNEChange_Connection");
}


void GNEChange_Connection::undo() {
    if (myForward) {
        myConnection->getEdgeFrom()->removeConnection(myConnection);
    } else {
        myConnection->getEdgeFrom()->addConnection(myConnection);
    }
}


void GNEChange_Connection::redo() {
    if (myForward) {
        myConnection->getEdgeFrom()->addConnection(myConnection);
    } else {
        myConnection->getEdgeFrom()->removeConnection(myConnection);
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
