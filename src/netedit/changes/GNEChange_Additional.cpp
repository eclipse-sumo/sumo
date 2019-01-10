/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
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

#include <utils/common/MsgHandler.h>
#include <netedit/GNENet.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/GNEViewNet.h>
#include <netedit/additionals/GNEDetectorE3.h>
#include <netedit/additionals/GNEDetectorEntry.h>
#include <netedit/additionals/GNEDetectorExit.h>
#include <netedit/additionals/GNEStoppingPlace.h>
#include <netedit/additionals/GNERerouter.h>
#include <netedit/additionals/GNEVariableSpeedSign.h>
#include <netedit/frames/GNEInspectorFrame.h>
#include <netedit/GNEViewParent.h>

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
    myLaneParent(nullptr),
    myEdgeParent(nullptr),
    myFirstAdditionalParent(myAdditional->getFirstAdditionalParent()),
    mySecondAdditionalParent(myAdditional->getSecondAdditionalParent()),
    myEdgeChilds(myAdditional->getEdgeChilds()),
    myLaneChilds(myAdditional->getLaneChilds()) {
    assert(myNet);
    myAdditional->incRef("GNEChange_Additional");
    // handle additionals with lane parent
    if (GNEAttributeCarrier::getTagProperties(myAdditional->getTag()).hasAttribute(SUMO_ATTR_LANE)) {
        myLaneParent = myNet->retrieveLane(myAdditional->getAttribute(SUMO_ATTR_LANE));
    }
    // handle additionals with edge parent
    if (GNEAttributeCarrier::getTagProperties(myAdditional->getTag()).hasAttribute(SUMO_ATTR_EDGE)) {
        myEdgeParent = myNet->retrieveEdge(myAdditional->getAttribute(SUMO_ATTR_EDGE));
    }
    // special case for Vaporizers
    if (myAdditional->getTag() == SUMO_TAG_VAPORIZER) {
        myEdgeParent = myNet->retrieveEdge(myAdditional->getAttribute(SUMO_ATTR_ID));
    }
}


GNEChange_Additional::~GNEChange_Additional() {
    assert(myAdditional);
    myAdditional->decRef("GNEChange_Additional");
    if (myAdditional->unreferenced()) {
        // show extra information for tests
        WRITE_DEBUG("Deleting unreferenced " + toString(myAdditional->getTag()) + " '" + myAdditional->getID() + "'");
        // make sure that additional isn't in net before removing
        if (myNet->retrieveAdditional(myAdditional->getTag(), myAdditional->getID(), false)) {
            myNet->deleteAdditional(myAdditional);
        }
        delete myAdditional;
    }
}


void
GNEChange_Additional::undo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Removing " + toString(myAdditional->getTag()) + " '" + myAdditional->getID() + "' in GNEChange_Additional");
        // delete additional of test
        myNet->deleteAdditional(myAdditional);
        // 1 - If additional own a lane parent, remove it from lane
        if (myLaneParent) {
            myLaneParent->removeAdditionalChild(myAdditional);
        }
        // 2 - If additional own a edge parent, remove it from edge
        if (myEdgeParent) {
            myEdgeParent->removeAdditionalChild(myAdditional);
        }
        // 3 - If additional has a first parent, remove it from their additional childs
        if (myFirstAdditionalParent) {
            myFirstAdditionalParent->removeAdditionalChild(myAdditional);
        }
        // 4 - If additiona has a second parent, remove it from their additional childs
        if (mySecondAdditionalParent) {
            mySecondAdditionalParent->removeAdditionalChild(myAdditional);
        }
        // 5 - if Additional has edge childs, remove it of their additional parents
        for (auto i : myEdgeChilds) {
            i->removeAdditionalParent(myAdditional);
        }
        // 6 - if Additional has lane childs, remove it of their additional parents
        for (auto i : myLaneChilds) {
            i->removeAdditionalParent(myAdditional);
        }
    } else {
        // show extra information for tests
        WRITE_DEBUG("Adding " + toString(myAdditional->getTag()) + " '" + myAdditional->getID() + "' in GNEChange_Additional");
        // insert additional of test
        myNet->insertAdditional(myAdditional);
        // 1 - If additional own a Lane parent, add it to lane
        if (myLaneParent) {
            myLaneParent->addAdditionalChild(myAdditional);
        }
        // 2 - If additional own a edge parent, add it to edge
        if (myEdgeParent != nullptr) {
            myEdgeParent->addAdditionalChild(myAdditional);
        }
        // 3 - If additional has a parent, add it into additional parent
        if (myFirstAdditionalParent) {
            myFirstAdditionalParent->addAdditionalChild(myAdditional);
        }
        // 4 - If additional has a parent, add it into additional parent
        if (mySecondAdditionalParent) {
            mySecondAdditionalParent->addAdditionalChild(myAdditional);
        }
        // 5 - if Additional has edge childs, add id into additional parents
        for (auto i : myEdgeChilds) {
            i->addAdditionalParent(myAdditional);
        }
        // 6 - if Additional has lane childs, add id into additional parents
        for (auto i : myLaneChilds) {
            i->addAdditionalParent(myAdditional);
        }
    }
    // Requiere always save additionals
    myNet->requiereSaveAdditionals(true);
    // check if inspector frame has to be updated
    if (myNet->getViewNet()->getViewParent()->getInspectorFrame()->shown()) {
        myNet->getViewNet()->getViewParent()->getInspectorFrame()->getACHierarchy()->refreshACHierarchy();
    }
}


void
GNEChange_Additional::redo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Adding " + toString(myAdditional->getTag()) + " '" + myAdditional->getID() + "' in GNEChange_Additional");
        // insert additional into net
        myNet->insertAdditional(myAdditional);
        // 1 - If additional own a Lane parent, add it to lane
        if (myLaneParent) {
            myLaneParent->addAdditionalChild(myAdditional);
        }
        // 2 - If additional own a edge parent, add it to edge
        if (myEdgeParent != nullptr) {
            myEdgeParent->addAdditionalChild(myAdditional);
        }
        // 3 - If additional has a parent, add it into additional parent
        if (myFirstAdditionalParent) {
            myFirstAdditionalParent->addAdditionalChild(myAdditional);
        }
        // 4 - If additional has a parent, add it into additional parent
        if (mySecondAdditionalParent) {
            mySecondAdditionalParent->addAdditionalChild(myAdditional);
        }
        // 5 - if Additional has edge childs, add id into additional parents
        for (auto i : myEdgeChilds) {
            i->addAdditionalParent(myAdditional);
        }
        // 6 - if Additional has lane childs, add id into additional parents
        for (auto i : myLaneChilds) {
            i->addAdditionalParent(myAdditional);
        }
    } else {
        // show extra information for tests
        WRITE_DEBUG("Removing " + toString(myAdditional->getTag()) + " '" + myAdditional->getID() + "' in GNEChange_Additional");
        // delete additional of test
        myNet->deleteAdditional(myAdditional);
        // 1 - If additional own a lane parent, remove it from lane
        if (myLaneParent) {
            myLaneParent->removeAdditionalChild(myAdditional);
        }
        // 2 - If additional own a edge parent, remove it from edge
        if (myEdgeParent) {
            myEdgeParent->removeAdditionalChild(myAdditional);
        }
        // 3 - If additiona has a first parent, remove it from their additional childs
        if (myFirstAdditionalParent) {
            myFirstAdditionalParent->removeAdditionalChild(myAdditional);
        }
        // 4 - If additiona has a second parent, remove it from their additional childs
        if (mySecondAdditionalParent) {
            mySecondAdditionalParent->removeAdditionalChild(myAdditional);
        }
        // 5 - if Additional has edge childs, remove it of their additional parents
        for (auto i : myEdgeChilds) {
            i->removeAdditionalParent(myAdditional);
        }
        // 6 - if Additional has lane childs, remove it of their additional parents
        for (auto i : myLaneChilds) {
            i->removeAdditionalParent(myAdditional);
        }
    }
    // Requiere always save additionals
    myNet->requiereSaveAdditionals(true);
    // check if inspector frame has to be updated
    if (myNet->getViewNet()->getViewParent()->getInspectorFrame()->shown()) {
        myNet->getViewNet()->getViewParent()->getInspectorFrame()->getACHierarchy()->refreshACHierarchy();
    }
}


FXString
GNEChange_Additional::undoName() const {
    if (myForward) {
        return ("Undo create " + toString(myAdditional->getTag())).c_str();
    } else {
        return ("Undo delete " + toString(myAdditional->getTag())).c_str();
    }
}


FXString
GNEChange_Additional::redoName() const {
    if (myForward) {
        return ("Redo create " + toString(myAdditional->getTag())).c_str();
    } else {
        return ("Redo delete " + toString(myAdditional->getTag())).c_str();
    }
}
