/****************************************************************************/
/// @file    GNEChange_Edge.cpp
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id$
///
// A network change in which a single junction is created or deleted
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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

#include <utils/common/MsgHandler.h>

#include "GNEChange_Edge.h"
#include "GNENet.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNERerouter.h"
#include "GNEViewNet.h"


// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_Edge, GNEChange, NULL, 0)

// ===========================================================================
// member method definitions
// ===========================================================================


/// @brief constructor for creating an edge
GNEChange_Edge::GNEChange_Edge(GNEEdge* edge, bool forward):
    GNEChange(edge->getNet(), forward),
    myEdge(edge) {
    assert(myNet);
    edge->incRef("GNEChange_Edge");
    // Save additionals of edge
    myAdditionalChilds = myEdge->getAdditionalChilds();
    // save rerouters of edge
    myGNERerouters = myEdge->getGNERerouters();
}


GNEChange_Edge::~GNEChange_Edge() {
    assert(myEdge);
    myEdge->decRef("GNEChange_Edge");
    if (myEdge->unreferenced()) {
        // show extra information for tests
        if (myEdge->getNet()->getViewNet()->isTestingModeEnabled()) {
            WRITE_WARNING("Deleting unreferenced " + toString(myEdge->getTag()) + " '" + myEdge->getID() + "'");
        }
        delete myEdge;
    }
}


void
GNEChange_Edge::undo() {
    if (myForward) {
        // show extra information for tests
        if (myEdge->getNet()->getViewNet()->isTestingModeEnabled()) {
            WRITE_WARNING("Deleting " + toString(myEdge->getTag()) + " '" + myEdge->getID() + "'");
        }
        // delete edge from net
        myNet->deleteSingleEdge(myEdge);
        // 1 - Remove additionals childs of this edge
        for (std::vector<GNEAdditional*>::iterator i = myAdditionalChilds.begin(); i != myAdditionalChilds.end(); i++) {
            myNet->deleteAdditional(*i);
        }
        // 2 - Remove references to this edge in GNERerouters
        for (std::vector<GNERerouter*>::iterator i = myGNERerouters.begin(); i != myGNERerouters.end(); i++) {
            (*i)->removeEdgeChild(myEdge);
        }
    } else {
        // show extra information for tests
        if (myEdge->getNet()->getViewNet()->isTestingModeEnabled()) {
            WRITE_WARNING("Adding " + toString(myEdge->getTag()) + " '" + myEdge->getID() + "'");
        }
        // insert edge into net
        myNet->insertEdge(myEdge);
        // 1 - add additionals childs of this edge
        for (std::vector<GNEAdditional*>::iterator i = myAdditionalChilds.begin(); i != myAdditionalChilds.end(); i++) {
            myNet->insertAdditional(*i);
        }
        // 2 - Add references to this edge in GNERerouters
        for (std::vector<GNERerouter*>::iterator i = myGNERerouters.begin(); i != myGNERerouters.end(); i++) {
            (*i)->addEdgeChild(myEdge);
        }
    }
}


void
GNEChange_Edge::redo() {
    if (myForward) {
        // show extra information for tests
        if (myEdge->getNet()->getViewNet()->isTestingModeEnabled()) {
            WRITE_WARNING("Adding " + toString(myEdge->getTag()) + " '" + myEdge->getID() + "'");
        }
        // insert edge into net
        myNet->insertEdge(myEdge);
        // 1 - Add additionals childs of this edge
        for (std::vector<GNEAdditional*>::iterator i = myAdditionalChilds.begin(); i != myAdditionalChilds.end(); i++) {
            myNet->insertAdditional(*i);
        }
        // 2 - Add references to this edge in GNERerouters
        for (std::vector<GNERerouter*>::iterator i = myGNERerouters.begin(); i != myGNERerouters.end(); i++) {
            (*i)->addEdgeChild(myEdge);
        }
    } else {
        // show extra information for tests
        if (myEdge->getNet()->getViewNet()->isTestingModeEnabled()) {
            WRITE_WARNING("Deleting " + toString(myEdge->getTag()) + " '" + myEdge->getID() + "'");
        }
        // delte edge from net
        myNet->deleteSingleEdge(myEdge);
        // 1 - Remove additionals childs of this edge
        for (std::vector<GNEAdditional*>::iterator i = myAdditionalChilds.begin(); i != myAdditionalChilds.end(); i++) {
            myNet->deleteAdditional(*i);
        }
        // 2 - Remove references to this edge in GNERerouters
        for (std::vector<GNERerouter*>::iterator i = myGNERerouters.begin(); i != myGNERerouters.end(); i++) {
            (*i)->removeEdgeChild(myEdge);
        }
    }
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
