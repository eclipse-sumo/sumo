/****************************************************************************/
/// @file    GNEChange_Edge.cpp
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

#include "GNEChange_Edge.h"
#include "GNENet.h"
#include "GNEEdge.h"
#include "GNELane.h"

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


/// @brief constructor for creating an edge
GNEChange_Edge::GNEChange_Edge(GNEEdge* edge, bool forward):
    GNEChange(edge->getNet(), forward),
    myEdge(edge) {
    assert(myNet);
    edge->incRef("GNEChange_Edge");
    // Save additionals of edge
    myAdditionalChilds = myEdge->getAdditionalChilds();
}


GNEChange_Edge::~GNEChange_Edge() {
    assert(myEdge);
    myEdge->decRef("GNEChange_Edge");
    if (myEdge->unreferenced()) {
        delete myEdge;
    }
}


void
GNEChange_Edge::undo() {
    if (myForward) {
        myNet->deleteSingleEdge(myEdge);
        // 1 - Remove additional sets vinculated with this edge of net
        for (std::vector<GNEAdditional*>::iterator i = myAdditionalChilds.begin(); i != myAdditionalChilds.end(); i++) {
            myNet->deleteAdditional(*i);
        }
    } else {
        myNet->insertEdge(myEdge);
        // 1 - add additional sets vinculated with this edge to the net
        for (std::vector<GNEAdditional*>::iterator i = myAdditionalChilds.begin(); i != myAdditionalChilds.end(); i++) {
            myNet->insertAdditional(*i);
        }
    }
}


void
GNEChange_Edge::redo() {
    if (myForward) {
        myNet->insertEdge(myEdge);
        // 1 - Add additional sets vinculated with this edge to the net
        for (std::vector<GNEAdditional*>::iterator i = myAdditionalChilds.begin(); i != myAdditionalChilds.end(); i++) {
            myNet->insertAdditional(*i);
        }
    } else {
        myNet->deleteSingleEdge(myEdge);
        // 1 - Remove additional sets vinculated with this edge of net
        for (std::vector<GNEAdditional*>::iterator i = myAdditionalChilds.begin(); i != myAdditionalChilds.end(); i++) {
            myNet->deleteAdditional(*i);
        }
    }
}


FXString
GNEChange_Edge::undoName() const {
    if (myForward) {
        return ("Undo create " + toString(SUMO_TAG_EDGE)).c_str();
    } else {
        return ("Undo delete " + toString(SUMO_TAG_EDGE)).c_str();
    }
}


FXString
GNEChange_Edge::redoName() const {
    if (myForward) {
        return ("Redo create " + toString(SUMO_TAG_EDGE)).c_str();
    } else {
        return ("Redo delete " + toString(SUMO_TAG_EDGE)).c_str();
    }
}
