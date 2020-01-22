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
/// @file    GNEChange_Shape.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2017
///
// A network change in which a single poly is created or deleted
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/elements/network/GNELane.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/additional/GNEShape.h>
#include <netedit/elements/additional/GNEAdditional.h>
#include <netedit/elements/demand/GNEDemandElement.h>

#include "GNEChange_Shape.h"

// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_Shape, GNEChange, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

GNEChange_Shape::GNEChange_Shape(GNEShape* shape, bool forward) :
    GNEChange(shape->getNet(), shape, shape, forward),
    myShape(shape) {
    myShape->incRef("GNEChange_Shape");
}


GNEChange_Shape::~GNEChange_Shape() {
    assert(myShape);
    myShape->decRef("GNEChange_Shape");
    if (myShape->unreferenced()) {
        // make sure that shape are removed of ShapeContainer (net) AND grid
        if (myNet->retrievePolygon(myShape->getID(), false) != nullptr) {
            // show extra information for tests
            WRITE_DEBUG("Removing " + myShape->getTagStr() + " '" + myShape->getID() + "' from net in ~GNEChange_Shape()");
            // all polygons are placed in RTREE
            myNet->removeGLObjectFromGrid(dynamic_cast<GUIGlObject*>(myShape));
            // remove polygon from container
            myNet->myPolygons.remove(myShape->getID(), false);
            // Remove element from parent elements
            for (const auto& i : myParentEdges) {
                i->removeChildShape(myShape);
            }
            for (const auto& i : myParentLanes) {
                i->removeChildShape(myShape);
            }
            for (const auto& i : myParentShapes) {
                i->removeChildShape(myShape);
            }
            for (const auto& i : myParentAdditionals) {
                i->removeChildShape(myShape);
            }
            for (const auto& i : myParentDemandElements) {
                i->removeChildShape(myShape);
            }
            // Remove element from child elements
            for (const auto& i : myChildEdges) {
                i->removeParentShape(myShape);
            }
            for (const auto& i : myChildLanes) {
                i->removeParentShape(myShape);
            }
            for (const auto& i : myChildShapes) {
                i->removeParentShape(myShape);
            }
            for (const auto& i : myChildAdditionals) {
                i->removeParentShape(myShape);
            }
            for (const auto& i : myChildDemandElements) {
                i->removeParentShape(myShape);
            }
        } else if (myNet->retrievePOI(myShape->getID(), false) != nullptr) {
            // show extra information for tests
            WRITE_DEBUG("Removing " + myShape->getTagStr() + " '" + myShape->getID() + "' from net in ~GNEChange_Shape()");
            // only certain POIS are placed in RTREE
            if (myShape->getTagProperty().isPlacedInRTree()) {
                myNet->removeGLObjectFromGrid(dynamic_cast<GUIGlObject*>(myShape));
            }
            // remove POI from container
            myNet->myPOIs.remove(myShape->getID(), false);
            // Remove element from parent elements
            for (const auto& i : myParentEdges) {
                i->removeChildShape(myShape);
            }
            for (const auto& i : myParentLanes) {
                i->removeChildShape(myShape);
            }
            for (const auto& i : myParentShapes) {
                i->removeChildShape(myShape);
            }
            for (const auto& i : myParentAdditionals) {
                i->removeChildShape(myShape);
            }
            for (const auto& i : myParentDemandElements) {
                i->removeChildShape(myShape);
            }
            // Remove element from child elements
            for (const auto& i : myChildEdges) {
                i->removeParentShape(myShape);
            }
            for (const auto& i : myChildLanes) {
                i->removeParentShape(myShape);
            }
            for (const auto& i : myChildShapes) {
                i->removeParentShape(myShape);
            }
            for (const auto& i : myChildAdditionals) {
                i->removeParentShape(myShape);
            }
            for (const auto& i : myChildDemandElements) {
                i->removeParentShape(myShape);
            }
        }
        // show extra information for tests
        WRITE_DEBUG("delete " + myShape->getTagStr() + " '" + myShape->getID() + "' in ~GNEChange_Shape()");
        delete myShape;
    }
}


void
GNEChange_Shape::undo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Removing " + myShape->getTagStr() + " '" + myShape->getID() + "' from viewNet");
        // remove shape from net
        myNet->removeShape(myShape, false);
        // Remove element from parent elements
        for (const auto& i : myParentEdges) {
            i->removeChildShape(myShape);
        }
        for (const auto& i : myParentLanes) {
            i->removeChildShape(myShape);
        }
        for (const auto& i : myParentShapes) {
            i->removeChildShape(myShape);
        }
        for (const auto& i : myParentAdditionals) {
            i->removeChildShape(myShape);
        }
        for (const auto& i : myParentDemandElements) {
            i->removeChildShape(myShape);
        }
        // Remove element from child elements
        for (const auto& i : myChildEdges) {
            i->removeParentShape(myShape);
        }
        for (const auto& i : myChildLanes) {
            i->removeParentShape(myShape);
        }
        for (const auto& i : myChildShapes) {
            i->removeParentShape(myShape);
        }
        for (const auto& i : myChildAdditionals) {
            i->removeParentShape(myShape);
        }
        for (const auto& i : myChildDemandElements) {
            i->removeParentShape(myShape);
        }
    } else {
        // show extra information for tests
        WRITE_DEBUG("Adding " + myShape->getTagStr() + " '" + myShape->getID() + "' into viewNet");
        // Add shape in net
        myNet->insertShape(myShape, false);
        // add element in parent elements
        for (const auto& i : myParentEdges) {
            i->addChildShape(myShape);
        }
        for (const auto& i : myParentLanes) {
            i->addChildShape(myShape);
        }
        for (const auto& i : myParentShapes) {
            i->addChildShape(myShape);
        }
        for (const auto& i : myParentAdditionals) {
            i->addChildShape(myShape);
        }
        for (const auto& i : myParentDemandElements) {
            i->addChildShape(myShape);
        }
        // add element in child elements
        for (const auto& i : myChildEdges) {
            i->addParentShape(myShape);
        }
        for (const auto& i : myChildLanes) {
            i->addParentShape(myShape);
        }
        for (const auto& i : myChildShapes) {
            i->addParentShape(myShape);
        }
        for (const auto& i : myChildAdditionals) {
            i->addParentShape(myShape);
        }
        for (const auto& i : myChildDemandElements) {
            i->addParentShape(myShape);
        }
    }
}


void
GNEChange_Shape::redo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Adding " + myShape->getTagStr() + " '" + myShape->getID() + "' into viewNet");
        // Add shape in net
        myNet->insertShape(myShape, false);
        // add element in parent elements
        for (const auto& i : myParentEdges) {
            i->addChildShape(myShape);
        }
        for (const auto& i : myParentLanes) {
            i->addChildShape(myShape);
        }
        for (const auto& i : myParentShapes) {
            i->addChildShape(myShape);
        }
        for (const auto& i : myParentAdditionals) {
            i->addChildShape(myShape);
        }
        for (const auto& i : myParentDemandElements) {
            i->addChildShape(myShape);
        }
        // add element in child elements
        for (const auto& i : myChildEdges) {
            i->addParentShape(myShape);
        }
        for (const auto& i : myChildLanes) {
            i->addParentShape(myShape);
        }
        for (const auto& i : myChildShapes) {
            i->addParentShape(myShape);
        }
        for (const auto& i : myChildAdditionals) {
            i->addParentShape(myShape);
        }
        for (const auto& i : myChildDemandElements) {
            i->addParentShape(myShape);
        }
    } else {
        // show extra information for tests
        WRITE_DEBUG("Removing " + myShape->getTagStr() + " '" + myShape->getID() + "' from viewNet");
        // remove shape from net
        myNet->removeShape(myShape, false);
        // Remove element from parent elements
        for (const auto& i : myParentEdges) {
            i->removeChildShape(myShape);
        }
        for (const auto& i : myParentLanes) {
            i->removeChildShape(myShape);
        }
        for (const auto& i : myParentShapes) {
            i->removeChildShape(myShape);
        }
        for (const auto& i : myParentAdditionals) {
            i->removeChildShape(myShape);
        }
        for (const auto& i : myParentDemandElements) {
            i->removeChildShape(myShape);
        }
        // Remove element from child elements
        for (const auto& i : myChildEdges) {
            i->removeParentShape(myShape);
        }
        for (const auto& i : myChildLanes) {
            i->removeParentShape(myShape);
        }
        for (const auto& i : myChildShapes) {
            i->removeParentShape(myShape);
        }
        for (const auto& i : myChildAdditionals) {
            i->removeParentShape(myShape);
        }
        for (const auto& i : myChildDemandElements) {
            i->removeParentShape(myShape);
        }
    }
}


FXString
GNEChange_Shape::undoName() const {
    if (myForward) {
        return ("Undo create " + myShape->getTagStr()).c_str();
    } else {
        return ("Undo delete " + myShape->getTagStr()).c_str();
    }
}


FXString
GNEChange_Shape::redoName() const {
    if (myForward) {
        return ("Redo create " + myShape->getTagStr()).c_str();
    } else {
        return ("Redo delete " + myShape->getTagStr()).c_str();
    }
}
