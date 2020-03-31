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
#include <netedit/elements/additional/GNEShape.h>

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
        if (myNet->retrieveShape(myShape->getTagProperty().getTag(), myShape->getID(), false) != nullptr) {
            // show extra information for tests
            WRITE_DEBUG("Removing " + myShape->getTagStr() + " '" + myShape->getID() + "' from net in ~GNEChange_Shape()");
            // remove polygon from AttributeCarreirs
            myNet->getAttributeCarriers()->deleteShape(myShape);
            // Remove element from parents and children
            removeShape(myShape);
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
        myNet->getAttributeCarriers()->deleteShape(myShape);
        // Remove element from parents and children
        removeShape(myShape);
    } else {
        // show extra information for tests
        WRITE_DEBUG("Adding " + myShape->getTagStr() + " '" + myShape->getID() + "' into viewNet");
        // Add shape in net
        myNet->getAttributeCarriers()->insertShape(myShape);
        // Add element in parents and children
        addShape(myShape);
    }
}


void
GNEChange_Shape::redo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Adding " + myShape->getTagStr() + " '" + myShape->getID() + "' into viewNet");
        // Add shape in net
        myNet->getAttributeCarriers()->insertShape(myShape);
        // Add element in parents and children
        addShape(myShape);
    } else {
        // show extra information for tests
        WRITE_DEBUG("Removing " + myShape->getTagStr() + " '" + myShape->getID() + "' from viewNet");
        // remove shape from net
        myNet->getAttributeCarriers()->deleteShape(myShape);
        // Remove element from parents and children
        removeShape(myShape);
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
