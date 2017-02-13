/****************************************************************************/
/// @file    GNEChange_Crossing.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2016
/// @version $Id$
///
// A network change in which a single junction is created or deleted
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2016 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GNEChange_Crossing.h"
#include "GNENet.h"
#include "GNEViewNet.h"
#include "GNECrossing.h"
#include "GNEJunction.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_Crossing, GNEChange, NULL, 0)

// ===========================================================================
// member method definitions
// ===========================================================================


/// @brief constructor for creating an crossing
GNEChange_Crossing::GNEChange_Crossing(GNEJunction* junctionParent, const std::vector<NBEdge*>& edges, SUMOReal width, bool priority, bool forward):
    GNEChange(junctionParent->getNet(), forward),
    myJunctionParent(junctionParent),
    myEdges(edges),
    myWidth(width),
    myPriority(priority) {
    assert(myNet);
}


GNEChange_Crossing::~GNEChange_Crossing() {}


void GNEChange_Crossing::undo() {
    if (myForward) {
        // remove crossing of NBNode and update geometry
        myJunctionParent->getNBNode()->removeCrossing(myEdges);
        myJunctionParent->updateGeometry();
        // Update view
        myNet->getViewNet()->update();
    } else {
        // add crossing of NBNode and update geometry
        myJunctionParent->getNBNode()->addCrossing(myEdges, myWidth, myPriority);
        myJunctionParent->updateGeometry();
        // Update view
        myNet->getViewNet()->update();
    }
}


void GNEChange_Crossing::redo() {
    if (myForward) {
        // add crossing of NBNode and update geometry
        myJunctionParent->getNBNode()->addCrossing(myEdges, myWidth, myPriority);
        myJunctionParent->updateGeometry();
        // Update view
        myNet->getViewNet()->update();
    } else {
        // remove crossing of NBNode and update geometry
        myJunctionParent->getNBNode()->removeCrossing(myEdges);
        myJunctionParent->updateGeometry();
        // Update view
        myNet->getViewNet()->update();
    }
}


FXString
GNEChange_Crossing::undoName() const {
    if (myForward) {
        return ("Undo create " + toString(SUMO_TAG_CROSSING)).c_str();
    } else {
        return ("Undo delete " + toString(SUMO_TAG_CROSSING)).c_str();
    }
}


FXString
GNEChange_Crossing::redoName() const {
    if (myForward) {
        return ("Redo create " + toString(SUMO_TAG_CROSSING)).c_str();
    } else {
        return ("Redo delete " + toString(SUMO_TAG_CROSSING)).c_str();
    }
}
