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
/// @file    GNEChange_Poly.cpp
/// @author  Jakob Erdmann
/// @date    Mar 2011
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

#include "GNEChange_Poly.h"
#include "GNEPoly.h"
#include "GNENet.h"
#include "GNEViewNet.h"


// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_Poly, GNEChange, NULL, 0)

// ===========================================================================
// member method definitions
// ===========================================================================


/// @brief constructor for creating a poly
GNEChange_Poly::GNEChange_Poly(GNENet* net, GNEPoly* poly, bool forward) :
    GNEChange(net, forward),
    myPoly(poly) {
    myPoly->incRef("GNEChange_Poly");
    assert(myNet);
}


GNEChange_Poly::~GNEChange_Poly() {
    assert(myPoly);
    myPoly->decRef("GNEChange_Poly");
    if (myPoly->unreferenced()) {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Removing " + toString(SUMO_TAG_POLY) + " '" + myPoly->getID() + "' from net");
        }
        // remove polygon of net
        if (myNet->removePolygon(myPoly->getID()) == false) {
            WRITE_ERROR("Trying to remove non-inserted ''" + myPoly->getID() + "' from net");
        }
    }
}


void
GNEChange_Poly::undo() {
    if (myForward) {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Removing " + toString(SUMO_TAG_POLY) + " '" + myPoly->getID() + "' from viewNet");
        }
        // remove polygon of net
        myNet->removePolygonOfView(myPoly);
    } else {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Adding " + toString(SUMO_TAG_POLY) + " '" + myPoly->getID() + "' into viewNet");
        }
        // Add polygon to view
        myNet->insertPolygonInView(myPoly);
    }
    // Requiere always save shapes
    myNet->requiereSaveShapes();
}


void
GNEChange_Poly::redo() {
    if (myForward) {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Adding " + toString(SUMO_TAG_POLY) + " '" + myPoly->getID() + "' into viewNet");
        }
        // Add polygon to view
        myNet->insertPolygonInView(myPoly);
    } else {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Removing " + toString(SUMO_TAG_POLY) + " '" + myPoly->getID() + "' from viewNet");
        }
        // delete poly from view
        myNet->removePolygonOfView(myPoly);
    }
    // Requiere always save shapes
    myNet->requiereSaveShapes();
}


FXString
GNEChange_Poly::undoName() const {
    if (myForward) {
        return ("Undo create " + toString(SUMO_TAG_POLY)).c_str();
    } else {
        return ("Undo delete " + toString(SUMO_TAG_POLY)).c_str();
    }
}


FXString
GNEChange_Poly::redoName() const {
    if (myForward) {
        return ("Redo create " + toString(SUMO_TAG_POLY)).c_str();
    } else {
        return ("Redo delete " + toString(SUMO_TAG_POLY)).c_str();
    }
}
