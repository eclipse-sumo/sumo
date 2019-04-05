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
#include <netedit/netelements/GNELane.h>
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
    myNumLanes((int)edge->getLanes().size()) {
    // first save all hierarchical elements of edge
    myShapeParents.push_back(edge->getShapeParents());
    myAdditionalParents.push_back(edge->getAdditionalParents());
    myDemandElementParents.push_back(edge->getDemandElementParents());
    myShapeChilds.push_back(edge->getShapeChilds());
    myAdditionalChilds.push_back(edge->getAdditionalChilds());
    myDemandElementChilds.push_back(edge->getDemandElementChilds());
    // now save all hierarchical elements of edge's lane
    for (const auto &i : edge->getLanes()) {
        myShapeParents.push_back(i->getShapeParents());
        myAdditionalParents.push_back(i->getAdditionalParents());
        myDemandElementParents.push_back(i->getDemandElementParents());
        myShapeChilds.push_back(i->getShapeChilds());
        myAdditionalChilds.push_back(i->getAdditionalChilds());
        myDemandElementChilds.push_back(i->getDemandElementChilds());
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
        for (const auto &i : myShapeParents.at(0)) {
            i->removeEdgeChild(myEdge);
        }
        for (const auto &i : myAdditionalParents.at(0)) {
            i->removeEdgeChild(myEdge);
        }
        for (const auto &i : myDemandElementParents.at(0)) {
            i->removeEdgeChild(myEdge);
        }
        // Remove edge from child elements
        for (const auto &i : myShapeChilds.at(0)) {
            i->removeEdgeParent(myEdge);
        }
        for (const auto &i : myAdditionalChilds.at(0)) {
            i->removeEdgeParent(myEdge);
        }
        for (const auto &i : myDemandElementChilds.at(0)) {
            i->removeEdgeParent(myEdge);
        }
        // repeat operations for all lane's edge
        for (int i = 1; i <= myNumLanes; i++) {
            // Remove every lane's edge from parent elements
            for (const auto &j : myShapeParents.at(i)) {
                j->removeLaneChild(myEdge->getLanes().at(i-1));
            }
            for (const auto &j : myAdditionalParents.at(i)) {
                j->removeLaneChild(myEdge->getLanes().at(i-1));
            }
            for (const auto &j : myDemandElementParents.at(i)) {
                j->removeLaneChild(myEdge->getLanes().at(i-1));
            }
            // Remove every lane's edge from child elements
            for (const auto &j : myShapeChilds.at(i)) {
                j->removeLaneParent(myEdge->getLanes().at(i-1));
            }
            for (const auto &j : myAdditionalChilds.at(i)) {
                j->removeLaneParent(myEdge->getLanes().at(i-1));
            }
            for (const auto &j : myDemandElementChilds.at(i)) {
                j->removeLaneParent(myEdge->getLanes().at(i-1));
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
        for (const auto &i : myShapeParents.at(0)) {
            i->addEdgeChild(myEdge);
        }
        for (const auto &i : myAdditionalParents.at(0)) {
            i->addEdgeChild(myEdge);
        }
        for (const auto &i : myDemandElementParents.at(0)) {
            i->addEdgeChild(myEdge);
        }
        // add edge in child elements
        for (const auto &i : myShapeChilds.at(0)) {
            i->addEdgeParent(myEdge);
        }
        for (const auto &i : myAdditionalChilds.at(0)) {
            i->addEdgeParent(myEdge);
        }
        for (const auto &i : myDemandElementChilds.at(0)) {
            i->addEdgeParent(myEdge);
        }
        // repeat operations for all lane's edge
        for (int i = 1; i <= myNumLanes; i++) {
            // add lane's edge in parent elements
            for (const auto &j : myShapeParents.at(i)) {
                j->addLaneChild(myEdge->getLanes().at(i-1));
            }
            for (const auto &j : myAdditionalParents.at(i)) {
                j->addLaneChild(myEdge->getLanes().at(i-1));
            }
            for (const auto &j : myDemandElementParents.at(i)) {
                j->addLaneChild(myEdge->getLanes().at(i-1));
            }
            // add lane's edge in child elements
            for (const auto &j : myShapeChilds.at(i)) {
                j->addLaneParent(myEdge->getLanes().at(i-1));
            }
            for (const auto &j : myAdditionalChilds.at(i)) {
                j->addLaneParent(myEdge->getLanes().at(i-1));
            }
            for (const auto &j : myDemandElementChilds.at(i)) {
                j->addLaneParent(myEdge->getLanes().at(i-1));
            }
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
        for (const auto &i : myShapeParents.at(0)) {
            i->addEdgeChild(myEdge);
        }
        for (const auto &i : myAdditionalParents.at(0)) {
            i->addEdgeChild(myEdge);
        }
        for (const auto &i : myDemandElementParents.at(0)) {
            i->addEdgeChild(myEdge);
        }
        // add edge in child elements
        for (const auto &i : myShapeChilds.at(0)) {
            i->addEdgeParent(myEdge);
        }
        for (const auto &i : myAdditionalChilds.at(0)) {
            i->addEdgeParent(myEdge);
        }
        for (const auto &i : myDemandElementChilds.at(0)) {
            i->addEdgeParent(myEdge);
        }
        // repeat operations for all lane's edge
        for (int i = 1; i <= myNumLanes; i++) {
            // add lane's edge in parent elements
            for (const auto &j : myShapeParents.at(i)) {
                j->addLaneChild(myEdge->getLanes().at(i-1));
            }
            for (const auto &j : myAdditionalParents.at(i)) {
                j->addLaneChild(myEdge->getLanes().at(i-1));
            }
            for (const auto &j : myDemandElementParents.at(i)) {
                j->addLaneChild(myEdge->getLanes().at(i-1));
            }
            // add lane's edge in child elements
            for (const auto &j : myShapeChilds.at(i)) {
                j->addLaneParent(myEdge->getLanes().at(i-1));
            }
            for (const auto &j : myAdditionalChilds.at(i)) {
                j->addLaneParent(myEdge->getLanes().at(i-1));
            }
            for (const auto &j : myDemandElementChilds.at(i)) {
                j->addLaneParent(myEdge->getLanes().at(i-1));
            }
        }
    } else {
        // show extra information for tests
        WRITE_DEBUG("Removing " + myEdge->getTagStr() + " '" + myEdge->getID() + "' from " + toString(SUMO_TAG_NET));
        // Remove edge from parent elements
        for (const auto &i : myShapeParents.at(0)) {
            i->removeEdgeChild(myEdge);
        }
        for (const auto &i : myAdditionalParents.at(0)) {
            i->removeEdgeChild(myEdge);
        }
        for (const auto &i : myDemandElementParents.at(0)) {
            i->removeEdgeChild(myEdge);
        }
        // Remove edge from child elements
        for (const auto &i : myShapeChilds.at(0)) {
            i->removeEdgeParent(myEdge);
        }
        for (const auto &i : myAdditionalChilds.at(0)) {
            i->removeEdgeParent(myEdge);
        }
        for (const auto &i : myDemandElementChilds.at(0)) {
            i->removeEdgeParent(myEdge);
        }
        // repeat operations for all lane's edge
        for (int i = 1; i <= myNumLanes; i++) {
            // Remove every lane's edge from parent elements
            for (const auto &j : myShapeParents.at(i)) {
                j->removeLaneChild(myEdge->getLanes().at(i-1));
            }
            for (const auto &j : myAdditionalParents.at(i)) {
                j->removeLaneChild(myEdge->getLanes().at(i-1));
            }
            for (const auto &j : myDemandElementParents.at(i)) {
                j->removeLaneChild(myEdge->getLanes().at(i-1));
            }
            // Remove every lane's edge from child elements
            for (const auto &j : myShapeChilds.at(i)) {
                j->removeLaneParent(myEdge->getLanes().at(i-1));
            }
            for (const auto &j : myAdditionalChilds.at(i)) {
                j->removeLaneParent(myEdge->getLanes().at(i-1));
            }
            for (const auto &j : myDemandElementChilds.at(i)) {
                j->removeLaneParent(myEdge->getLanes().at(i-1));
            }
        }
        // delete edge from net
        myNet->deleteSingleEdge(myEdge, false);
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
