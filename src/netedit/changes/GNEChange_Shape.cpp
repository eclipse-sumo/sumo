/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChange_Shape.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2017
/// @version $Id$
///
// A network change in which a single poly is created or deleted
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/additionals/GNEShape.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/demandelements/GNEDemandElement.h>

#include "GNEChange_Shape.h"

// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_Shape, GNEChange, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

GNEChange_Shape::GNEChange_Shape(GNEShape* shape, bool forward) :
    GNEChange(shape->getNet(), forward),
    myShape(shape),
    myEdgeParents(shape->getEdgeParents()),
    myLaneParents(shape->getLaneParents()),
    myShapeParents(shape->getShapeParents()),
    myAdditionalParents(shape->getAdditionalParents()),
    myDemandElementParents(shape->getDemandElementParents()),
    myEdgeChildren(shape->getEdgeChildren()),
    myLaneChildren(shape->getLaneChildren()),
    myShapeChildren(shape->getShapeChildren()),
    myAdditionalChildren(shape->getAdditionalChildren()),
    myDemandElementChildren(shape->getDemandElementChildren()) {
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
            for (const auto& i : myEdgeParents) {
                i->removeShapeChild(myShape);
            }
            for (const auto& i : myLaneParents) {
                i->removeShapeChild(myShape);
            }
            for (const auto& i : myShapeParents) {
                i->removeShapeChild(myShape);
            }
            for (const auto& i : myAdditionalParents) {
                i->removeShapeChild(myShape);
            }
            for (const auto& i : myDemandElementParents) {
                i->removeShapeChild(myShape);
            }
            // Remove element from child elements
            for (const auto& i : myEdgeChildren) {
                i->removeShapeParent(myShape);
            }
            for (const auto& i : myLaneChildren) {
                i->removeShapeParent(myShape);
            }
            for (const auto& i : myShapeChildren) {
                i->removeShapeParent(myShape);
            }
            for (const auto& i : myAdditionalChildren) {
                i->removeShapeParent(myShape);
            }
            for (const auto& i : myDemandElementChildren) {
                i->removeShapeParent(myShape);
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
            for (const auto& i : myEdgeParents) {
                i->removeShapeChild(myShape);
            }
            for (const auto& i : myLaneParents) {
                i->removeShapeChild(myShape);
            }
            for (const auto& i : myShapeParents) {
                i->removeShapeChild(myShape);
            }
            for (const auto& i : myAdditionalParents) {
                i->removeShapeChild(myShape);
            }
            for (const auto& i : myDemandElementParents) {
                i->removeShapeChild(myShape);
            }
            // Remove element from child elements
            for (const auto& i : myEdgeChildren) {
                i->removeShapeParent(myShape);
            }
            for (const auto& i : myLaneChildren) {
                i->removeShapeParent(myShape);
            }
            for (const auto& i : myShapeChildren) {
                i->removeShapeParent(myShape);
            }
            for (const auto& i : myAdditionalChildren) {
                i->removeShapeParent(myShape);
            }
            for (const auto& i : myDemandElementChildren) {
                i->removeShapeParent(myShape);
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
        for (const auto& i : myEdgeParents) {
            i->removeShapeChild(myShape);
        }
        for (const auto& i : myLaneParents) {
            i->removeShapeChild(myShape);
        }
        for (const auto& i : myShapeParents) {
            i->removeShapeChild(myShape);
        }
        for (const auto& i : myAdditionalParents) {
            i->removeShapeChild(myShape);
        }
        for (const auto& i : myDemandElementParents) {
            i->removeShapeChild(myShape);
        }
        // Remove element from child elements
        for (const auto& i : myEdgeChildren) {
            i->removeShapeParent(myShape);
        }
        for (const auto& i : myLaneChildren) {
            i->removeShapeParent(myShape);
        }
        for (const auto& i : myShapeChildren) {
            i->removeShapeParent(myShape);
        }
        for (const auto& i : myAdditionalChildren) {
            i->removeShapeParent(myShape);
        }
        for (const auto& i : myDemandElementChildren) {
            i->removeShapeParent(myShape);
        }
    } else {
        // show extra information for tests
        WRITE_DEBUG("Adding " + myShape->getTagStr() + " '" + myShape->getID() + "' into viewNet");
        // Add shape in net
        myNet->insertShape(myShape, false);
        // add element in parent elements
        for (const auto& i : myEdgeParents) {
            i->addShapeChild(myShape);
        }
        for (const auto& i : myLaneParents) {
            i->addShapeChild(myShape);
        }
        for (const auto& i : myShapeParents) {
            i->addShapeChild(myShape);
        }
        for (const auto& i : myAdditionalParents) {
            i->addShapeChild(myShape);
        }
        for (const auto& i : myDemandElementParents) {
            i->addShapeChild(myShape);
        }
        // add element in child elements
        for (const auto& i : myEdgeChildren) {
            i->addShapeParent(myShape);
        }
        for (const auto& i : myLaneChildren) {
            i->addShapeParent(myShape);
        }
        for (const auto& i : myShapeChildren) {
            i->addShapeParent(myShape);
        }
        for (const auto& i : myAdditionalChildren) {
            i->addShapeParent(myShape);
        }
        for (const auto& i : myDemandElementChildren) {
            i->addShapeParent(myShape);
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
        for (const auto& i : myEdgeParents) {
            i->addShapeChild(myShape);
        }
        for (const auto& i : myLaneParents) {
            i->addShapeChild(myShape);
        }
        for (const auto& i : myShapeParents) {
            i->addShapeChild(myShape);
        }
        for (const auto& i : myAdditionalParents) {
            i->addShapeChild(myShape);
        }
        for (const auto& i : myDemandElementParents) {
            i->addShapeChild(myShape);
        }
        // add element in child elements
        for (const auto& i : myEdgeChildren) {
            i->addShapeParent(myShape);
        }
        for (const auto& i : myLaneChildren) {
            i->addShapeParent(myShape);
        }
        for (const auto& i : myShapeChildren) {
            i->addShapeParent(myShape);
        }
        for (const auto& i : myAdditionalChildren) {
            i->addShapeParent(myShape);
        }
        for (const auto& i : myDemandElementChildren) {
            i->addShapeParent(myShape);
        }
    } else {
        // show extra information for tests
        WRITE_DEBUG("Removing " + myShape->getTagStr() + " '" + myShape->getID() + "' from viewNet");
        // remove shape from net
        myNet->removeShape(myShape, false);
        // Remove element from parent elements
        for (const auto& i : myEdgeParents) {
            i->removeShapeChild(myShape);
        }
        for (const auto& i : myLaneParents) {
            i->removeShapeChild(myShape);
        }
        for (const auto& i : myShapeParents) {
            i->removeShapeChild(myShape);
        }
        for (const auto& i : myAdditionalParents) {
            i->removeShapeChild(myShape);
        }
        for (const auto& i : myDemandElementParents) {
            i->removeShapeChild(myShape);
        }
        // Remove element from child elements
        for (const auto& i : myEdgeChildren) {
            i->removeShapeParent(myShape);
        }
        for (const auto& i : myLaneChildren) {
            i->removeShapeParent(myShape);
        }
        for (const auto& i : myShapeChildren) {
            i->removeShapeParent(myShape);
        }
        for (const auto& i : myAdditionalChildren) {
            i->removeShapeParent(myShape);
        }
        for (const auto& i : myDemandElementChildren) {
            i->removeShapeParent(myShape);
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
