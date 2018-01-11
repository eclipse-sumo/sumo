/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
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
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/MsgHandler.h>
#include <utils/common/RGBColor.h>
#include <utils/geom/PositionVector.h>

#include "GNEChange_Shape.h"
#include "GNEShape.h"
#include "GNEViewNet.h"
#include "GNENet.h"


// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_Shape, GNEChange, NULL, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

GNEChange_Shape::GNEChange_Shape(GNEShape* shape, bool forward) :
    GNEChange(shape->getNet(), forward),
    myShape(shape) {
    assert(myNet);
    myShape->incRef("GNEChange_Shape");
}


GNEChange_Shape::~GNEChange_Shape() {
    assert(myShape);
    myShape->decRef("GNEChange_Shape");
    if (myShape->unreferenced()) {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Removing " + toString(myShape->getTag()) + " '" + myShape->getID() + "' from net");
        }
        // make sure that shape are removed of ShapeContainer
        if (myShape->getTag() == SUMO_TAG_POLY) {
            myNet->myPolygons.remove(myShape->getID());
        } else {
            myNet->myPOIs.remove(myShape->getID());
        }
    }
}


void
GNEChange_Shape::undo() {
    if (myForward) {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Removing " + toString(myShape->getTag()) + " '" + myShape->getID() + "' from viewNet");
        }
        // remove shape from net
        myNet->removeShape(myShape);
    } else {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Adding " + toString(myShape->getTag()) + " '" + myShape->getID() + "' into viewNet");
        }
        // Add shape in net
        myNet->insertShape(myShape);
    }
}


void
GNEChange_Shape::redo() {
    if (myForward) {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Adding " + toString(myShape->getTag()) + " '" + myShape->getID() + "' into viewNet");
        }
        // Add shape in net
        myNet->insertShape(myShape);
    } else {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Removing " + toString(myShape->getTag()) + " '" + myShape->getID() + "' from viewNet");
        }
        // remove shape from net
        myNet->removeShape(myShape);
    }
}


FXString
GNEChange_Shape::undoName() const {
    if (myForward) {
        return ("Undo create " + toString(myShape->getTag())).c_str();
    } else {
        return ("Undo delete " + toString(myShape->getTag())).c_str();
    }
}


FXString
GNEChange_Shape::redoName() const {
    if (myForward) {
        return ("Redo create " + toString(myShape->getTag())).c_str();
    } else {
        return ("Redo delete " + toString(myShape->getTag())).c_str();
    }
}
