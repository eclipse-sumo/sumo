/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChange_Edge.cpp
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

#include "GNEChange_Edge.h"
#include "GNENet.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNERerouter.h"
#include "GNEViewNet.h"


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
}


GNEChange_Edge::~GNEChange_Edge() {
    assert(myEdge);
    myEdge->decRef("GNEChange_Edge");
    if (myEdge->unreferenced()) {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Deleting unreferenced " + toString(myEdge->getTag()) + " '" + myEdge->getID() + "' GNEChange_Edge");
        }
        delete myEdge;
    }
}


void
GNEChange_Edge::undo() {
    if (myForward) {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Removing " + toString(myEdge->getTag()) + " '" + myEdge->getID() + "' from " + toString(SUMO_TAG_NET));
        }
        // delete edge from net
        myNet->deleteSingleEdge(myEdge);
    } else {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Adding " + toString(myEdge->getTag()) + " '" + myEdge->getID() + "' from " + toString(SUMO_TAG_NET));
        }
        // insert edge into net
        myNet->insertEdge(myEdge);
    }
}


void
GNEChange_Edge::redo() {
    if (myForward) {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Adding " + toString(myEdge->getTag()) + " '" + myEdge->getID() + "' from " + toString(SUMO_TAG_NET));
        }
        // insert edge into net
        myNet->insertEdge(myEdge);
    } else {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Removing " + toString(myEdge->getTag()) + " '" + myEdge->getID() + "' from " + toString(SUMO_TAG_NET));
        }
        // delte edge from net
        myNet->deleteSingleEdge(myEdge);
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
