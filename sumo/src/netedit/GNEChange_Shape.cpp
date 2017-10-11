/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2017 German Aerospace Center (DLR) and others.
/****************************************************************************/
//
//   This program and the accompanying materials
//   are made available under the terms of the Eclipse Public License v2.0
//   which accompanies this distribution, and is available at
//   http://www.eclipse.org/legal/epl-v20.html
//
/****************************************************************************/
/// @file    GNEChange_Shape.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2017
/// @version $Id: GNEChange_Shape.cpp 26421 2017-10-11 11:21:09Z palcraft $
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

GNEChange_Shape::GNEChange_Shape(GNENet* net, GNEShape* shape, bool forward) :
    GNEChange(net, forward),
    myShape(shape) {
    myShape->incRef("GNEChange_Shape");
    assert(myNet);
}


GNEChange_Shape::~GNEChange_Shape() {
    assert(myShape);
    myShape->decRef("GNEChange_Shape");
    if (myShape->unreferenced()) {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Removing " + toString(myShape->getTag()) + " '" + myShape->getID() + "' from net");
        }
        // remove shape of net
        if(myShape->getTag() == SUMO_TAG_POLY) {
            if (myNet->removePolygon(myShape->getID()) == false) {
                WRITE_ERROR("Trying to remove non-inserted ''" + myShape->getID() + "' from net");
            }
        } else {
            if (myNet->removePOI(myShape->getID()) == false) {
                WRITE_ERROR("Trying to remove non-inserted ''" + myShape->getID() + "' from net");
            }
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
        // remove polygon of view
        myNet->removeShapeOfView(myShape);
    } else {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Adding " + toString(myShape->getTag()) + " '" + myShape->getID() + "' into viewNet");
        }
        // Add polygon in view
        myNet->insertShapeInView(myShape);
    }
    // Requiere always save shapes
    myNet->requiereSaveShapes();
}


void
GNEChange_Shape::redo() {
    if (myForward) {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Adding " + toString(myShape->getTag()) + " '" + myShape->getID() + "' into viewNet");
        }
        // Add polygon to view
        myNet->insertShapeInView(myShape);
    } else {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Removing " + toString(myShape->getTag()) + " '" + myShape->getID() + "' from viewNet");
        }
        // delete poly of view
        myNet->removeShapeOfView(myShape);
    }
    // Requiere always save shapes
    myNet->requiereSaveShapes();
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
