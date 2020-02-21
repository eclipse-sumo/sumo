/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GNEChange_Edge.cpp
/// @author  Jakob Erdmann
/// @date    Mar 2011
///
// A network change in which a single junction is created or deleted
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/additionals/GNEShape.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/demandelements/GNEDemandElement.h>


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
    myEdgeParentShapes(edge->getParentShapes()),
    myEdgeParentAdditionals(edge->getParentAdditionals()),
    myEdgeParentDemandElements(edge->getParentDemandElements()),
    myChildEdgeShapes(edge->getChildShapes()),
    myChildEdgeAdditionals(edge->getChildAdditionals()),
    myChildEdgeDemandElements(edge->getChildDemandElements()) {
    // now save all hierarchical elements of edge's lane
    for (const auto& i : edge->getLanes()) {
        myLaneParentShapes.push_back(i->getParentShapes());
        myLaneParentAdditionals.push_back(i->getParentAdditionals());
        myLaneParentDemandElements.push_back(i->getParentDemandElements());
        myChildLaneShapes.push_back(i->getChildShapes());
        myChildLaneAdditionals.push_back(i->getChildAdditionals());
        myChildLaneDemandElements.push_back(i->getChildDemandElements());
    }

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
        // Remove edge from parent elements
        for (const auto& i : myEdgeParentShapes) {
            i->removeChildEdge(myEdge);
        }
        for (const auto& i : myEdgeParentAdditionals) {
            i->removeChildEdge(myEdge);
        }
        for (const auto& i : myEdgeParentDemandElements) {
            i->removeChildEdge(myEdge);
        }
        // Remove edge from child elements
        for (const auto& i : myChildEdgeShapes) {
            i->removeParentEdge(myEdge);
        }
        for (const auto& i : myChildEdgeAdditionals) {
            i->removeParentEdge(myEdge);
        }
        for (const auto& i : myChildEdgeDemandElements) {
            i->removeParentEdge(myEdge);
        }
        // repeat operations for all lane's edge
        for (int i = 0; i < (int)myEdge->getLanes().size(); i++) {
            // Remove every lane's edge from parent elements
            for (const auto& j : myLaneParentShapes.at(i)) {
                j->removeChildLane(myEdge->getLanes().at(i));
            }
            for (const auto& j : myLaneParentAdditionals.at(i)) {
                j->removeChildLane(myEdge->getLanes().at(i));
            }
            for (const auto& j : myLaneParentDemandElements.at(i)) {
                j->removeChildLane(myEdge->getLanes().at(i));
            }
            // Remove every lane's edge from child elements
            for (const auto& j : myChildLaneShapes.at(i)) {
                j->removeParentLane(myEdge->getLanes().at(i));
            }
            for (const auto& j : myChildLaneAdditionals.at(i)) {
                j->removeParentLane(myEdge->getLanes().at(i));
            }
            for (const auto& j : myChildLaneDemandElements.at(i)) {
                j->removeParentLane(myEdge->getLanes().at(i));
            }
        }
        // delete edge from net
        myNet->deleteSingleEdge(myEdge, false);
    } else {
        // show extra information for tests
        WRITE_DEBUG("Adding " + myEdge->getTagStr() + " '" + myEdge->getID() + "' from " + toString(SUMO_TAG_NET));
        // insert edge into net
        myNet->insertEdge(myEdge);
        // add edge in parent elements
        for (const auto& i : myEdgeParentShapes) {
            i->addChildEdge(myEdge);
        }
        for (const auto& i : myEdgeParentAdditionals) {
            i->addChildEdge(myEdge);
        }
        for (const auto& i : myEdgeParentDemandElements) {
            i->addChildEdge(myEdge);
        }
        // add edge in child elements
        for (const auto& i : myChildEdgeShapes) {
            i->addParentEdge(myEdge);
        }
        for (const auto& i : myChildEdgeAdditionals) {
            i->addParentEdge(myEdge);
        }
        for (const auto& i : myChildEdgeDemandElements) {
            i->addParentEdge(myEdge);
        }
        // repeat operations for all lane's edge
        for (int i = 0; i < (int)myEdge->getLanes().size(); i++) {
            // add lane's edge in parent elements
            for (const auto& j : myLaneParentShapes.at(i)) {
                j->addChildLane(myEdge->getLanes().at(i));
            }
            for (const auto& j : myLaneParentAdditionals.at(i)) {
                j->addChildLane(myEdge->getLanes().at(i));
            }
            for (const auto& j : myLaneParentDemandElements.at(i)) {
                j->addChildLane(myEdge->getLanes().at(i));
            }
            // add lane's edge in child elements
            for (const auto& j : myChildLaneShapes.at(i)) {
                j->addParentLane(myEdge->getLanes().at(i));
            }
            for (const auto& j : myChildLaneAdditionals.at(i)) {
                j->addParentLane(myEdge->getLanes().at(i));
            }
            for (const auto& j : myChildLaneDemandElements.at(i)) {
                j->addParentLane(myEdge->getLanes().at(i));
            }
        }
    }
    // enable save netElements
    myNet->requireSaveNet(true);
}


void
GNEChange_Edge::redo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Adding " + myEdge->getTagStr() + " '" + myEdge->getID() + "' from " + toString(SUMO_TAG_NET));
        // insert edge into net
        myNet->insertEdge(myEdge);
        // add edge in parent elements
        for (const auto& i : myEdgeParentShapes) {
            i->addChildEdge(myEdge);
        }
        for (const auto& i : myEdgeParentAdditionals) {
            i->addChildEdge(myEdge);
        }
        for (const auto& i : myEdgeParentDemandElements) {
            i->addChildEdge(myEdge);
        }
        // add edge in child elements
        for (const auto& i : myChildEdgeShapes) {
            i->addParentEdge(myEdge);
        }
        for (const auto& i : myChildEdgeAdditionals) {
            i->addParentEdge(myEdge);
        }
        for (const auto& i : myChildEdgeDemandElements) {
            i->addParentEdge(myEdge);
        }
        // repeat operations for all lane's edge
        for (int i = 0; i < (int)myEdge->getLanes().size(); i++) {
            // add lane's edge in parent elements
            for (const auto& j : myLaneParentShapes.at(i)) {
                j->addChildLane(myEdge->getLanes().at(i));
            }
            for (const auto& j : myLaneParentAdditionals.at(i)) {
                j->addChildLane(myEdge->getLanes().at(i));
            }
            for (const auto& j : myLaneParentDemandElements.at(i)) {
                j->addChildLane(myEdge->getLanes().at(i));
            }
            // add lane's edge in child elements
            for (const auto& j : myChildLaneShapes.at(i)) {
                j->addParentLane(myEdge->getLanes().at(i));
            }
            for (const auto& j : myChildLaneAdditionals.at(i)) {
                j->addParentLane(myEdge->getLanes().at(i));
            }
            for (const auto& j : myChildLaneDemandElements.at(i)) {
                j->addParentLane(myEdge->getLanes().at(i));
            }
        }
    } else {
        // show extra information for tests
        WRITE_DEBUG("Removing " + myEdge->getTagStr() + " '" + myEdge->getID() + "' from " + toString(SUMO_TAG_NET));
        // Remove edge from parent elements
        for (const auto& i : myEdgeParentShapes) {
            i->removeChildEdge(myEdge);
        }
        for (const auto& i : myEdgeParentAdditionals) {
            i->removeChildEdge(myEdge);
        }
        for (const auto& i : myEdgeParentDemandElements) {
            i->removeChildEdge(myEdge);
        }
        // Remove edge from child elements
        for (const auto& i : myChildEdgeShapes) {
            i->removeParentEdge(myEdge);
        }
        for (const auto& i : myChildEdgeAdditionals) {
            i->removeParentEdge(myEdge);
        }
        for (const auto& i : myChildEdgeDemandElements) {
            i->removeParentEdge(myEdge);
        }
        // repeat operations for all lane's edge
        for (int i = 0; i < (int)myEdge->getLanes().size(); i++) {
            // Remove every lane's edge from parent elements
            for (const auto& j : myLaneParentShapes.at(i)) {
                j->removeChildLane(myEdge->getLanes().at(i));
            }
            for (const auto& j : myLaneParentAdditionals.at(i)) {
                j->removeChildLane(myEdge->getLanes().at(i));
            }
            for (const auto& j : myLaneParentDemandElements.at(i)) {
                j->removeChildLane(myEdge->getLanes().at(i));
            }
            // Remove every lane's edge from child elements
            for (const auto& j : myChildLaneShapes.at(i)) {
                j->removeParentLane(myEdge->getLanes().at(i));
            }
            for (const auto& j : myChildLaneAdditionals.at(i)) {
                j->removeParentLane(myEdge->getLanes().at(i));
            }
            for (const auto& j : myChildLaneDemandElements.at(i)) {
                j->removeParentLane(myEdge->getLanes().at(i));
            }
        }
        // delete edge from net
        myNet->deleteSingleEdge(myEdge, false);
    }
    // enable save netElements
    myNet->requireSaveNet(true);
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
