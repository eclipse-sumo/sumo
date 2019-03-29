/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChange_Additional.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
// A network change in which a busStop is created or deleted
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/frames/GNEFrame.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEViewNet.h>

#include "GNEChange_Additional.h"

// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_Additional, GNEChange, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

GNEChange_Additional::GNEChange_Additional(GNEAdditional* additional, bool forward) :
    GNEChange(additional->getViewNet()->getNet(), forward),
    myAdditional(additional),
    myEdgeParents(myAdditional->getEdgeParents()),
    myLaneParents(myAdditional->getLaneParents()),
    myAdditionalParents(myAdditional->getAdditionalParents()),
    myEdgeChilds(myAdditional->getEdgeChilds()),
    myLaneChilds(myAdditional->getLaneChilds()) {
    myAdditional->incRef("GNEChange_Additional");
}


GNEChange_Additional::~GNEChange_Additional() {
    assert(myAdditional);
    myAdditional->decRef("GNEChange_Additional");
    if (myAdditional->unreferenced()) {
        // show extra information for tests
        WRITE_DEBUG("Deleting unreferenced " + myAdditional->getTagStr() + " '" + myAdditional->getID() + "'");
        // make sure that additional isn't in net before removing
        if (myNet->additionalExist(myAdditional)) {
            myNet->deleteAdditional(myAdditional, false);
        }
        delete myAdditional;
    }
}


void
GNEChange_Additional::undo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Removing " + myAdditional->getTagStr() + " '" + myAdditional->getID() + "' in GNEChange_Additional");
        // 1 - If additional own lane parents, remove it from lane
        for (auto i : myLaneParents) {
            i->removeAdditionalChild(myAdditional);
        }
        // 2 - If additional own edge parents, remove it from edge
        for (auto i : myEdgeParents) {
            i->removeAdditionalChild(myAdditional);
        }
        // 3 - If additional has parents, remove it from their additional childs
        for (auto i : myAdditionalParents) {
            i->removeAdditionalChild(myAdditional);
        }
        // 4 - if Additional has edge childs, remove it of their additional parents
        for (auto i : myEdgeChilds) {
            i->removeAdditionalParent(myAdditional);
        }
        // 5 - if Additional has lane childs, remove it of their additional parents
        for (auto i : myLaneChilds) {
            i->removeAdditionalParent(myAdditional);
        }
        // delete additional from net
        myNet->deleteAdditional(myAdditional, false);
    } else {
        // show extra information for tests
        WRITE_DEBUG("Adding " + myAdditional->getTagStr() + " '" + myAdditional->getID() + "' in GNEChange_Additional");
        // insert additional into net
        myNet->insertAdditional(myAdditional);
        // 1 - If additional own lane parents, add it into lanes
        for (auto i : myLaneParents) {
            i->addAdditionalChild(myAdditional);
        }
        // 2 - If additional own edge parents, add it into edges
        for (auto i : myEdgeParents) {
            i->addAdditionalChild(myAdditional);
        }
        // 3 - If additional has a parent, add it into additional parents
        for (auto i : myAdditionalParents) {
            i->addAdditionalChild(myAdditional);
        }
        // 4 - if Additional has edge childs, add id into additional parents
        for (auto i : myEdgeChilds) {
            i->addAdditionalParent(myAdditional);
        }
        // 5 - if Additional has lane childs, add id into additional parents
        for (auto i : myLaneChilds) {
            i->addAdditionalParent(myAdditional);
        }
    }
    // Requiere always save additionals
    myNet->requiereSaveAdditionals(true);
}


void
GNEChange_Additional::redo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Adding " + myAdditional->getTagStr() + " '" + myAdditional->getID() + "' in GNEChange_Additional");
        // insert additional into net
        myNet->insertAdditional(myAdditional);
        // 1 - If additional own lane parents, add it into lanes
        for (auto i : myLaneParents) {
            i->addAdditionalChild(myAdditional);
        }
        // 2 - If additional own edge parents, add it into edges
        for (auto i : myEdgeParents) {
            i->addAdditionalChild(myAdditional);
        }
        // 3 - If additional has a parent, add it into additional parents
        for (auto i : myAdditionalParents) {
            i->addAdditionalChild(myAdditional);
        }
        // 4 - if Additional has edge childs, add id into additional parents
        for (auto i : myEdgeChilds) {
            i->addAdditionalParent(myAdditional);
        }
        // 5 - if Additional has lane childs, add id into additional parents
        for (auto i : myLaneChilds) {
            i->addAdditionalParent(myAdditional);
        }
    } else {
        // show extra information for tests
        WRITE_DEBUG("Removing " + myAdditional->getTagStr() + " '" + myAdditional->getID() + "' in GNEChange_Additional");
        // 1 - If additional own lane parents, remove it from lane
        for (auto i : myLaneParents) {
            i->removeAdditionalChild(myAdditional);
        }
        // 2 - If additional own edge parents, remove it from edge
        for (auto i : myEdgeParents) {
            i->removeAdditionalChild(myAdditional);
        }
        // 3 - If additional has parents, remove it from their additional childs
        for (auto i : myAdditionalParents) {
            i->removeAdditionalChild(myAdditional);
        }
        // 4 - if Additional has edge childs, remove it of their additional parents
        for (auto i : myEdgeChilds) {
            i->removeAdditionalParent(myAdditional);
        }
        // 5 - if Additional has lane childs, remove it of their additional parents
        for (auto i : myLaneChilds) {
            i->removeAdditionalParent(myAdditional);
        }
        // remove additional of test
        myNet->deleteAdditional(myAdditional, false);
    }
    // Requiere always save additionals
    myNet->requiereSaveAdditionals(true);
}


FXString
GNEChange_Additional::undoName() const {
    if (myForward) {
        return ("Undo create " + myAdditional->getTagStr()).c_str();
    } else {
        return ("Undo delete " + myAdditional->getTagStr()).c_str();
    }
}


FXString
GNEChange_Additional::redoName() const {
    if (myForward) {
        return ("Redo create " + myAdditional->getTagStr()).c_str();
    } else {
        return ("Redo delete " + myAdditional->getTagStr()).c_str();
    }
}
