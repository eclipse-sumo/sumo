/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    GNEChange_Crossing.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2016
///
// A network change in which a single junction is created or deleted
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/elements/network/GNECrossing.h>
#include <netbuild/NBNetBuilder.h>

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
    GNEChange(Supermode::NETWORK, junctionParent, forward, selected),
    myJunctionParent(junctionParent),
    myEdges(edges),
    myWidth(width),
    myPriority(priority),
    myCustomTLIndex(customTLIndex),
    myCustomTLIndex2(customTLIndex2),
    myCustomShape(customShape) {
}


GNEChange_Crossing::GNEChange_Crossing(GNEJunction* junctionParent, const NBNode::Crossing& crossing, bool forward):
    GNEChange(Supermode::NETWORK, forward, false),
    myJunctionParent(junctionParent),
    myEdges(crossing.edges),
    myWidth(crossing.width),
    myPriority(crossing.priority),
    myCustomTLIndex(crossing.customTLIndex),
    myCustomTLIndex2(crossing.customTLIndex2),
    myCustomShape(crossing.customShape) {
}


GNEChange_Crossing::~GNEChange_Crossing() {
}


void GNEChange_Crossing::undo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("removing " + toString(SUMO_TAG_CROSSING) + " from " + myJunctionParent->getTagStr() + " '" + myJunctionParent->getID() + "'");
        // unselect if mySelectedElement is enabled
        if (mySelectedElement) {
            GNECrossing* crossing = myJunctionParent->retrieveGNECrossing(myJunctionParent->getNBNode()->getCrossing(myEdges), false);
            if (crossing) {
                crossing->unselectAttributeCarrier();
            } else {
#ifdef _DEBUG
                WRITE_WARNINGF("Unable to deselect crossing at junction '%' after undo", myJunctionParent->getID());
#endif
            }
        }
        // remove crossing of NBNode
        myJunctionParent->getNBNode()->removeCrossing(myEdges);
        // rebuild GNECrossings
        myJunctionParent->rebuildGNECrossings();
        // clean walking areas
        myJunctionParent->clearWalkingAreas();
        // Check if Flag "haveNetworkCrossings" has to be disabled
        if (myJunctionParent->getNet()->getAttributeCarriers()->getCrossings().empty() && (myJunctionParent->getNet()->getNetBuilder()->haveNetworkCrossings())) {
            // change flag of NetBuilder (For build GNECrossing)
            myJunctionParent->getNet()->getNetBuilder()->setHaveNetworkCrossings(false);
            // show extra information for tests
            WRITE_DEBUG("Changed flag netBuilder::haveNetworkCrossings from 'true' to 'false'");
        }
    } else {
        // show extra information for tests
        WRITE_DEBUG("Adding " + toString(SUMO_TAG_CROSSING) + " into " + myJunctionParent->getTagStr() + " '" + myJunctionParent->getID() + "'");
        // add crossing of NBNode
        NBNode::Crossing* c = myJunctionParent->getNBNode()->addCrossing(myEdges, myWidth, myPriority, myCustomTLIndex, myCustomTLIndex2, myCustomShape);
        // Check if Flag "haveNetworkCrossings" has to be enabled
        if (!myJunctionParent->getNet()->getNetBuilder()->haveNetworkCrossings() == false) {
            myJunctionParent->getNet()->getNetBuilder()->setHaveNetworkCrossings(true);
            // show extra information for tests
            WRITE_DEBUG("Changed flag netBuilder::haveNetworkCrossings from 'false' to 'true'");
        }
        // rebuild GNECrossings
        myJunctionParent->rebuildGNECrossings();
        // clean walking areas
        myJunctionParent->clearWalkingAreas();
        // select if mySelectedElement is enabled
        if (mySelectedElement) {
            if (mySelectedElement) {
                GNECrossing* crossing = myJunctionParent->retrieveGNECrossing(c, false);
                if (crossing) {
                    crossing->selectAttributeCarrier();
                } else {
#ifdef _DEBUG
                    WRITE_WARNINGF("Unable to select crossing at junction '%' after undo", myJunctionParent->getID());
#endif
                }
            }
        }
    }
    // enable save networkElements
    myJunctionParent->getNet()->getSavingStatus()->requireSaveNetwork();
}


void GNEChange_Crossing::redo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Adding " + toString(SUMO_TAG_CROSSING) + " into " + myJunctionParent->getTagStr() + " '" + myJunctionParent->getID() + "'");
        // add crossing of NBNode and update geometry
        NBNode::Crossing* c = myJunctionParent->getNBNode()->addCrossing(myEdges, myWidth, myPriority, myCustomTLIndex, myCustomTLIndex2, myCustomShape);
        // Check if Flag "haveNetworkCrossings" has to be enabled
        if (myJunctionParent->getNet()->getNetBuilder()->haveNetworkCrossings() == false) {
            myJunctionParent->getNet()->getNetBuilder()->setHaveNetworkCrossings(true);
            // show extra information for tests
            WRITE_DEBUG("Changed flag netBuilder::haveNetworkCrossings from 'false' to 'true'");
        }
        // rebuild GNECrossings
        myJunctionParent->rebuildGNECrossings();
        // clean walking areas
        myJunctionParent->clearWalkingAreas();
        // select if mySelectedElement is enabled
        if (mySelectedElement) {
            if (mySelectedElement) {
                GNECrossing* crossing = myJunctionParent->retrieveGNECrossing(c, false);
                if (crossing) {
                    crossing->selectAttributeCarrier();
                } else {
#ifdef _DEBUG
                    WRITE_WARNINGF("Unable to select crossing at junction '%' after undo", myJunctionParent->getID());
#endif
                }
            }
        }
    } else {
        // show extra information for tests
        WRITE_DEBUG("Removing " + toString(SUMO_TAG_CROSSING) + " from " + myJunctionParent->getTagStr() + " '" + myJunctionParent->getID() + "'");
        // unselect if mySelectedElement is enabled
        if (mySelectedElement) {
            GNECrossing* crossing = myJunctionParent->retrieveGNECrossing(myJunctionParent->getNBNode()->getCrossing(myEdges), false);
            if (crossing) {
                crossing->unselectAttributeCarrier();
            } else {
#ifdef _DEBUG
                WRITE_WARNINGF("Unable to deselect crossing at junction '%' after undo", myJunctionParent->getID());
#endif
            }
        }
        // remove crossing of NBNode and update geometry
        myJunctionParent->getNBNode()->removeCrossing(myEdges);
        // rebuild GNECrossings
        myJunctionParent->rebuildGNECrossings();
        // clean walking areas
        myJunctionParent->clearWalkingAreas();
        // Check if Flag "haveNetworkCrossings" has to be disabled
        if (myJunctionParent->getNet()->getAttributeCarriers()->getCrossings().empty() && (myJunctionParent->getNet()->getNetBuilder()->haveNetworkCrossings())) {
            // change flag of NetBuilder (For build GNECrossing)
            myJunctionParent->getNet()->getNetBuilder()->setHaveNetworkCrossings(false);
            // show extra information for tests
            WRITE_DEBUG("Changed flag netBuilder::haveNetworkCrossings from 'true' to 'false'");
        }
    }
    // enable save networkElements
    myJunctionParent->getNet()->getSavingStatus()->requireSaveNetwork();
}


std::string
GNEChange_Crossing::undoName() const {
    if (myForward) {
        return TL("Undo create crossing");
    } else {
        return TL("Undo delete crossing");
    }
}


std::string
GNEChange_Crossing::redoName() const {
    if (myForward) {
        return TL("Redo create crossing");
    } else {
        return TL("Redo delete crossing");
    }
}
