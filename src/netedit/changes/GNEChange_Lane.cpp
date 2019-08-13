/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChange_Lane.cpp
/// @author  Jakob Erdmann
/// @date    April 2011
/// @version $Id$
///
// A network change in which a single lane is created or deleted
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/additionals/GNEShape.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/demandelements/GNEDemandElement.h>

#include "GNEChange_Lane.h"


// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_Lane, GNEChange, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================


/// @brief constructor for creating an edge
GNEChange_Lane::GNEChange_Lane(GNEEdge* edge, GNELane* lane, const NBEdge::Lane& laneAttrs, bool forward, bool recomputeConnections):
    GNEChange(edge->getNet(), forward),
    myEdge(edge),
    myLane(lane),
    myLaneAttrs(laneAttrs),
    myRecomputeConnections(recomputeConnections) {
    assert(myNet);
    myEdge->incRef("GNEChange_Lane");
    if (myLane) {
        // non-zero pointer is passsed in case of removal or duplication
        myLane->incRef("GNEChange_Lane");
        // Save hierarchy elements of lane
        myShapeParents = myLane->getShapeParents();
        myAdditionalParents = myLane->getAdditionalParents();
        myDemandElementParents = myLane->getDemandElementParents();
        myShapeChildren = myLane->getShapeChildren();
        myAdditionalChildren = myLane->getAdditionalChildren();
        myDemandElementChildren = myLane->getDemandElementChildren();
    } else {
        assert(forward);
    }
}


GNEChange_Lane::~GNEChange_Lane() {
    assert(myEdge);
    myEdge->decRef("GNEChange_Lane");
    if (myEdge->unreferenced()) {
        // show extra information for tests
        WRITE_DEBUG("Deleting unreferenced " + myEdge->getTagStr() + " '" + myEdge->getID() + "' in GNEChange_Lane");
        delete myEdge;
    }
    if (myLane) {
        myLane->decRef("GNEChange_Lane");
        if (myLane->unreferenced()) {
            // show extra information for tests
            WRITE_DEBUG("Deleting unreferenced " + myLane->getTagStr() + " '" + myLane->getID() + "' in GNEChange_Lane");
            delete myLane;
        }
    }
}


void
GNEChange_Lane::undo() {
    if (myForward) {
        // show extra information for tests
        if (myLane != nullptr) {
            WRITE_DEBUG("Removing " + myLane->getTagStr() + " '" + myLane->getID() + "' from " + toString(SUMO_TAG_EDGE));
        } else {
            WRITE_DEBUG("Removing nullptr " + toString(SUMO_TAG_LANE) + " from " + toString(SUMO_TAG_EDGE));
        }
        // remove lane from edge
        myEdge->removeLane(myLane, false);
        // Remove lane from parent elements
        for (const auto& i : myShapeParents) {
            i->removeLaneChild(myLane);
        }
        for (const auto& i : myAdditionalParents) {
            i->removeLaneChild(myLane);
        }
        for (const auto& i : myDemandElementParents) {
            i->removeLaneChild(myLane);
        }
        // Remove lane from child elements
        for (const auto& i : myShapeChildren) {
            i->removeLaneParent(myLane);
        }
        for (const auto& i : myAdditionalChildren) {
            i->removeLaneParent(myLane);
        }
        for (const auto& i : myDemandElementChildren) {
            i->removeLaneParent(myLane);
        }
    } else {
        // show extra information for tests
        if (myLane != nullptr) {
            WRITE_DEBUG("Adding " + myLane->getTagStr() + " '" + myLane->getID() + "' into " + toString(SUMO_TAG_EDGE));
        } else {
            WRITE_DEBUG("Adding nullptr " + toString(SUMO_TAG_LANE) + " into " + toString(SUMO_TAG_EDGE));
        }
        // add lane and their attributes to edge
        // (lane removal is reverted, no need to recompute connections)
        myEdge->addLane(myLane, myLaneAttrs, false);
        // add lane in parent elements
        for (const auto& i : myShapeParents) {
            i->addLaneChild(myLane);
        }
        for (const auto& i : myAdditionalParents) {
            i->addLaneChild(myLane);
        }
        for (const auto& i : myDemandElementParents) {
            i->addLaneChild(myLane);
        }
        // add lane in child elements
        for (const auto& i : myShapeChildren) {
            i->addLaneParent(myLane);
        }
        for (const auto& i : myAdditionalChildren) {
            i->addLaneParent(myLane);
        }
        for (const auto& i : myDemandElementChildren) {
            i->addLaneParent(myLane);
        }
    }
    // enable save netElements
    myNet->requiereSaveNet(true);
}


void
GNEChange_Lane::redo() {
    if (myForward) {
        // show extra information for tests
        if (myLane != nullptr) {
            WRITE_DEBUG("Adding " + myLane->getTagStr() + " '" + myLane->getID() + "' into " + toString(SUMO_TAG_EDGE));
        } else {
            WRITE_DEBUG("Adding nullptr " + toString(SUMO_TAG_LANE) + " into " + toString(SUMO_TAG_EDGE));
        }
        // add lane and their attributes to edge
        myEdge->addLane(myLane, myLaneAttrs, myRecomputeConnections);
        // add lane in parent elements
        for (const auto& i : myShapeParents) {
            i->addLaneChild(myLane);
        }
        for (const auto& i : myAdditionalParents) {
            i->addLaneChild(myLane);
        }
        for (const auto& i : myDemandElementParents) {
            i->addLaneChild(myLane);
        }
        // add additional in child elements
        for (const auto& i : myShapeChildren) {
            i->addLaneParent(myLane);
        }
        for (const auto& i : myAdditionalChildren) {
            i->addLaneParent(myLane);
        }
        for (const auto& i : myDemandElementChildren) {
            i->addLaneParent(myLane);
        }
    } else {
        // show extra information for tests
        if (myLane != nullptr) {
            WRITE_DEBUG("Removing " + myLane->getTagStr() + " '" + myLane->getID() + "' from " + toString(SUMO_TAG_EDGE));
        } else {
            WRITE_DEBUG("Removing nullptr " + toString(SUMO_TAG_LANE) + " from " + toString(SUMO_TAG_EDGE));
        }
        // Remove lane from parent elements
        for (const auto& i : myShapeParents) {
            i->removeLaneChild(myLane);
        }
        for (const auto& i : myAdditionalParents) {
            i->removeLaneChild(myLane);
        }
        for (const auto& i : myDemandElementParents) {
            i->removeLaneChild(myLane);
        }
        // Remove additional from child elements
        for (const auto& i : myShapeChildren) {
            i->removeLaneParent(myLane);
        }
        for (const auto& i : myAdditionalChildren) {
            i->removeLaneParent(myLane);
        }
        for (const auto& i : myDemandElementChildren) {
            i->removeLaneParent(myLane);
        }
        // remove lane from edge
        myEdge->removeLane(myLane, myRecomputeConnections);
    }
    // enable save netElements
    myNet->requiereSaveNet(true);
}


FXString
GNEChange_Lane::undoName() const {
    if (myForward) {
        return ("Undo create " + toString(SUMO_TAG_LANE)).c_str();
    } else {
        return ("Undo delete " + toString(SUMO_TAG_LANE)).c_str();
    }
}


FXString
GNEChange_Lane::redoName() const {
    if (myForward) {
        return ("Redo create " + toString(SUMO_TAG_LANE)).c_str();
    } else {
        return ("Redo delete " + toString(SUMO_TAG_LANE)).c_str();
    }
}
