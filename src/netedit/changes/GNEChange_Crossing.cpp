/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChange_Crossing.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2016
/// @version $Id$
///
// A network change in which a single junction is created or deleted
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/netelements/GNECrossing.h>
#include <netedit/netelements/GNEJunction.h>
#include <netbuild/NBNetBuilder.h>
#include <netedit/GNEViewNet.h>

#include "GNEChange_Crossing.h"


// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_Crossing, GNEChange, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================


GNEChange_Crossing::GNEChange_Crossing(GNEJunction* junctionParent, const std::vector<NBEdge*>& edges,
                                       double width, bool priority, int customTLIndex, int customTLIndex2, const PositionVector& customShape, bool selected, bool forward):
    GNEChange(junctionParent->getNet(), forward),
    myJunctionParent(junctionParent),
    myEdges(edges),
    myWidth(width),
    myPriority(priority),
    myCustomTLIndex(customTLIndex),
    myCustomTLIndex2(customTLIndex2),
    myCustomShape(customShape),
    mySelected(selected) {
    assert(myNet);
}


GNEChange_Crossing::GNEChange_Crossing(GNEJunction* junctionParent, const NBNode::Crossing& crossing, bool forward):
    GNEChange(junctionParent->getNet(), forward),
    myJunctionParent(junctionParent),
    myEdges(crossing.edges),
    myWidth(crossing.width),
    myPriority(crossing.priority),
    myCustomTLIndex(crossing.customTLIndex),
    myCustomTLIndex2(crossing.customTLIndex2),
    myCustomShape(crossing.customShape),
    mySelected(false) {
    assert(myNet);
}


GNEChange_Crossing::~GNEChange_Crossing() {
    assert(myNet);
}


void GNEChange_Crossing::undo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("removing " + toString(SUMO_TAG_CROSSING) + " from " + myJunctionParent->getTagStr() + " '" + myJunctionParent->getID() + "'");
        // remove crossing of NBNode
        myJunctionParent->getNBNode()->removeCrossing(myEdges);
        // rebuild GNECrossings
        myJunctionParent->rebuildGNECrossings();
        // Check if Flag "haveNetworkCrossings" has to be disabled
        if ((myNet->netHasGNECrossings() == false) && (myNet->getNetBuilder()->haveNetworkCrossings())) {
            // change flag of NetBuilder (For build GNECrossing)
            myNet->getNetBuilder()->setHaveNetworkCrossings(false);
            // show extra information for tests
            WRITE_DEBUG("Changed flag netBuilder::haveNetworkCrossings from 'true' to 'false'");
        }
        // Update view
        myNet->getViewNet()->update();
    } else {
        // show extra information for tests
        WRITE_DEBUG("Adding " + toString(SUMO_TAG_CROSSING) + " into " + myJunctionParent->getTagStr() + " '" + myJunctionParent->getID() + "'");
        // add crossing of NBNode
        NBNode::Crossing* c = myJunctionParent->getNBNode()->addCrossing(myEdges, myWidth, myPriority, myCustomTLIndex, myCustomTLIndex2, myCustomShape);
        // Check if Flag "haveNetworkCrossings" has to be enabled
        if (myNet->getNetBuilder()->haveNetworkCrossings() == false) {
            myNet->getNetBuilder()->setHaveNetworkCrossings(true);
            // show extra information for tests
            WRITE_DEBUG("Changed flag netBuilder::haveNetworkCrossings from 'false' to 'true'");
        }
        // rebuild GNECrossings
        myJunctionParent->rebuildGNECrossings();
        // select if mySelected is enabled
        if (mySelected) {
            myJunctionParent->retrieveGNECrossing(c, false)->selectAttributeCarrier();
        }
        // Update view
        myNet->getViewNet()->update();
    }
    // enable save netElements
    myNet->requiereSaveNet(true);
}


void GNEChange_Crossing::redo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Adding " + toString(SUMO_TAG_CROSSING) + " into " + myJunctionParent->getTagStr() + " '" + myJunctionParent->getID() + "'");
        // add crossing of NBNode and update geometry
        NBNode::Crossing* c = myJunctionParent->getNBNode()->addCrossing(myEdges, myWidth, myPriority, myCustomTLIndex, myCustomTLIndex2, myCustomShape);
        // Check if Flag "haveNetworkCrossings" has to be enabled
        if (myNet->getNetBuilder()->haveNetworkCrossings() == false) {
            myNet->getNetBuilder()->setHaveNetworkCrossings(true);
            // show extra information for tests
            WRITE_DEBUG("Changed flag netBuilder::haveNetworkCrossings from 'false' to 'true'");
        }
        // rebuild GNECrossings
        myJunctionParent->rebuildGNECrossings();
        // select if mySelected is enabled
        if (mySelected) {
            myJunctionParent->retrieveGNECrossing(c, false)->selectAttributeCarrier();
        }
        // Update view
        myNet->getViewNet()->update();
    } else {
        // show extra information for tests
        WRITE_DEBUG("Removing " + toString(SUMO_TAG_CROSSING) + " from " + myJunctionParent->getTagStr() + " '" + myJunctionParent->getID() + "'");
        // remove crossing of NBNode and update geometry
        myJunctionParent->getNBNode()->removeCrossing(myEdges);
        // rebuild GNECrossings
        myJunctionParent->rebuildGNECrossings();
        // Check if Flag "haveNetworkCrossings" has to be disabled
        if ((myNet->netHasGNECrossings() == false) && (myNet->getNetBuilder()->haveNetworkCrossings())) {
            // change flag of NetBuilder (For build GNECrossing)
            myNet->getNetBuilder()->setHaveNetworkCrossings(false);
            // show extra information for tests
            WRITE_DEBUG("Changed flag netBuilder::haveNetworkCrossings from 'true' to 'false'");
        }
        // Update view
        myNet->getViewNet()->update();
    }
    // enable save netElements
    myNet->requiereSaveNet(true);
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
