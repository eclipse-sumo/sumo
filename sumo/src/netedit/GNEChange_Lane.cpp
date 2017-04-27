/****************************************************************************/
/// @file    GNEChange_Lane.cpp
/// @author  Jakob Erdmann
/// @date    April 2011
/// @version $Id$
///
// A network change in which a single lane is created or deleted
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
#include <cassert>

#include "GNEChange_Lane.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNENet.h"
#include "GNEViewNet.h"

// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_Lane, GNEChange, NULL, 0)

// ===========================================================================
// member method definitions
// ===========================================================================


/// @brief constructor for creating an edge
GNEChange_Lane::GNEChange_Lane(GNEEdge* edge, GNELane* lane, const NBEdge::Lane& laneAttrs, bool forward):
    GNEChange(edge->getNet(), forward),
    myEdge(edge),
    myLane(lane),
    myLaneAttrs(laneAttrs) {
    assert(myNet);
    myEdge->incRef("GNEChange_Lane");
    if (myLane) {
        // non-zero pointer is passsed in case of removal or duplication
        myLane->incRef("GNEChange_Lane");
        // Save additionals of lane
        myAdditionalChilds = myLane->getAdditionalChilds();
    } else {
        assert(forward);
    }
}


GNEChange_Lane::~GNEChange_Lane() {
    assert(myEdge);
    myEdge->decRef("GNEChange_Lane");
    if (myEdge->unreferenced()) {
        // show extra information for tests
        if (myNet->getViewNet()->isTestingModeEnabled()) {
            WRITE_WARNING("Deleting unreferenced " + toString(myEdge->getTag()) + " '" + myEdge->getID() + "'");
        }
        delete myEdge;
    }
    if (myLane) {
        myLane->decRef("GNEChange_Lane");
        if (myLane->unreferenced()) {
            // show extra information for tests
            if (myNet->getViewNet()->isTestingModeEnabled()) {
                WRITE_WARNING("Deleting unreferenced " + toString(myLane->getTag()) + " '" + myLane->getID() + "'");
            }
            delete myLane;
        }
    }
}


void
GNEChange_Lane::undo() {
    if (myForward) {
        // show extra information for tests
        if (myEdge->getNet()->getViewNet()->isTestingModeEnabled()) {
            if (myLane != NULL) {
                WRITE_WARNING("Deleting " + toString(myLane->getTag()) + " '" + myLane->getID() + "'");
            } else {
                WRITE_WARNING("Deleting NULL " + toString(SUMO_TAG_LANE));
            }
        }
        // remove lane from edge
        myEdge->removeLane(myLane);
        // Remove additional sets vinculated with this lane of net
        for (std::vector<GNEAdditional*>::iterator i = myAdditionalChilds.begin(); i != myAdditionalChilds.end(); i++) {
            myNet->deleteAdditional(*i);
        }
    } else {
        // show extra information for tests
        if (myEdge->getNet()->getViewNet()->isTestingModeEnabled()) {
            if (myLane != NULL) {
                WRITE_WARNING("Adding " + toString(myLane->getTag()) + " '" + myLane->getID() + "'");
            } else {
                WRITE_WARNING("Adding NULL " + toString(SUMO_TAG_LANE));
            }
        }
        // add lane and their attributes to edge
        myEdge->addLane(myLane, myLaneAttrs);
        // add additional sets vinculated with this lane of net
        for (std::vector<GNEAdditional*>::iterator i = myAdditionalChilds.begin(); i != myAdditionalChilds.end(); i++) {
            myNet->insertAdditional(*i);
        }
    }
}


void
GNEChange_Lane::redo() {
    if (myForward) {
        // show extra information for tests
        if (myEdge->getNet()->getViewNet()->isTestingModeEnabled()) {
            if (myLane != NULL) {
                WRITE_WARNING("Adding " + toString(myLane->getTag()) + " '" + myLane->getID() + "'");
            } else {
                WRITE_WARNING("Adding NULL " + toString(SUMO_TAG_LANE));
            }
        }
        // add lane and their attributes to edge
        myEdge->addLane(myLane, myLaneAttrs);
        // add additional sets vinculated with this lane of net
        for (std::vector<GNEAdditional*>::iterator i = myAdditionalChilds.begin(); i != myAdditionalChilds.end(); i++) {
            myNet->insertAdditional(*i);
        }
    } else {
        // show extra information for tests
        if (myEdge->getNet()->getViewNet()->isTestingModeEnabled()) {
            if (myLane != NULL) {
                WRITE_WARNING("Deleting " + toString(myLane->getTag()) + " '" + myLane->getID() + "'");
            } else {
                WRITE_WARNING("Deleting NULL " + toString(SUMO_TAG_LANE));
            }
        }
        // remove lane from edge
        myEdge->removeLane(myLane);
        // Remove additional sets vinculated with this lane of net
        for (std::vector<GNEAdditional*>::iterator i = myAdditionalChilds.begin(); i != myAdditionalChilds.end(); i++) {
            myNet->deleteAdditional(*i);
        }
    }
}


FXString
GNEChange_Lane::undoName() const {
    if (myForward) {
        return ("Undo create " + toString(SUMO_TAG_LANE)).c_str();
    } else {
        return ("Undo delete " + toString(SUMO_TAG_LANE)).c_str();
    }
}


FXString
GNEChange_Lane::redoName() const {
    if (myForward) {
        return ("Redo create " + toString(SUMO_TAG_LANE)).c_str();
    } else {
        return ("Redo delete " + toString(SUMO_TAG_LANE)).c_str();
    }
}
