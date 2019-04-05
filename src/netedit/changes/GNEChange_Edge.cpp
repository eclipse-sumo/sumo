/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
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
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/additionals/GNEShape.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/demandelements/GNEDemandElement.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEViewNet.h>
#include <netedit/frames/GNEFrame.h>


#include "GNEChange_Edge.h"

// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_Edge, GNEChange, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================


/// @brief constructor for creating an edge
GNEChange_Edge::GNEChange_Edge(GNEEdge* edge, bool forward):
    GNEChange(edge->getNet(), forward),
    myEdge(edge),
    myShapeParents(edge->getShapeParents()),
    myAdditionalParents(edge->getAdditionalParents()),
    myDemandElementParents(edge->getDemandElementParents()),
    myShapeChilds(edge->getShapeChilds()),
    myAdditionalChilds(edge->getAdditionalChilds()),
    myDemandElementChilds(edge->getDemandElementChilds()) {
    edge->incRef("GNEChange_Edge");
}


GNEChange_Edge::~GNEChange_Edge() {
    assert(myEdge);
    myEdge->decRef("GNEChange_Edge");
    if (myEdge->unreferenced()) {
        // show extra information for tests
        WRITE_DEBUG("Deleting unreferenced " + myEdge->getTagStr() + " '" + myEdge->getID() + "' GNEChange_Edge");
        delete myEdge;
    }
}


void
GNEChange_Edge::undo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Removing " + myEdge->getTagStr() + " '" + myEdge->getID() + "' from " + toString(SUMO_TAG_NET));
        // delete edge from net
        myNet->deleteSingleEdge(myEdge, false);
        // Remove edge from parent elements
        for (const auto &i : myShapeParents) {
            i->removeEdgeChild(myEdge);
        }
        for (const auto &i : myAdditionalParents) {
            i->removeEdgeChild(myEdge);
        }
        for (const auto &i : myDemandElementParents) {
            i->removeEdgeChild(myEdge);
        }
        // Remove edge from child elements
        for (const auto &i : myShapeChilds) {
            i->removeEdgeParent(myEdge);
        }
        for (const auto &i : myAdditionalChilds) {
            i->removeEdgeParent(myEdge);
        }
        for (const auto &i : myDemandElementChilds) {
            i->removeEdgeParent(myEdge);
        }
    } else {
        // show extra information for tests
        WRITE_DEBUG("Adding " + myEdge->getTagStr() + " '" + myEdge->getID() + "' from " + toString(SUMO_TAG_NET));
        // insert edge into net
        myNet->insertEdge(myEdge);
        // add edge in parent elements
        for (const auto &i : myShapeParents) {
            i->addEdgeChild(myEdge);
        }
        for (const auto &i : myAdditionalParents) {
            i->addEdgeChild(myEdge);
        }
        for (const auto &i : myDemandElementParents) {
            i->addEdgeChild(myEdge);
        }
        // add edge in child elements
        for (const auto &i : myShapeChilds) {
            i->addEdgeParent(myEdge);
        }
        for (const auto &i : myAdditionalChilds) {
            i->addEdgeParent(myEdge);
        }
        for (const auto &i : myDemandElementChilds) {
            i->addEdgeParent(myEdge);
        }
    }
    // enable save netElements
    myNet->requiereSaveNet(true);
}


void
GNEChange_Edge::redo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Adding " + myEdge->getTagStr() + " '" + myEdge->getID() + "' from " + toString(SUMO_TAG_NET));
        // insert edge into net
        myNet->insertEdge(myEdge);
        // add edge in parent elements
        for (const auto &i : myShapeParents) {
            i->addEdgeChild(myEdge);
        }
        for (const auto &i : myAdditionalParents) {
            i->addEdgeChild(myEdge);
        }
        for (const auto &i : myDemandElementParents) {
            i->addEdgeChild(myEdge);
        }
        // add edge in child elements
        for (const auto &i : myShapeChilds) {
            i->addEdgeParent(myEdge);
        }
        for (const auto &i : myAdditionalChilds) {
            i->addEdgeParent(myEdge);
        }
        for (const auto &i : myDemandElementChilds) {
            i->addEdgeParent(myEdge);
        }
    } else {
        // show extra information for tests
        WRITE_DEBUG("Removing " + myEdge->getTagStr() + " '" + myEdge->getID() + "' from " + toString(SUMO_TAG_NET));
        // delete edge from net
        myNet->deleteSingleEdge(myEdge, false);
        // Remove edge from parent elements
        for (const auto &i : myShapeParents) {
            i->removeEdgeChild(myEdge);
        }
        for (const auto &i : myAdditionalParents) {
            i->removeEdgeChild(myEdge);
        }
        for (const auto &i : myDemandElementParents) {
            i->removeEdgeChild(myEdge);
        }
        // Remove edge from child elements
        for (const auto &i : myShapeChilds) {
            i->removeEdgeParent(myEdge);
        }
        for (const auto &i : myAdditionalChilds) {
            i->removeEdgeParent(myEdge);
        }
        for (const auto &i : myDemandElementChilds) {
            i->removeEdgeParent(myEdge);
        }
    }
    // enable save netElements
    myNet->requiereSaveNet(true);
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
