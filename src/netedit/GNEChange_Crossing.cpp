/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
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
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/MsgHandler.h>
#include <utils/gui/div/GUIGlobalSelection.h>

#include "GNEChange_Crossing.h"
#include "GNENet.h"
#include "GNEViewNet.h"
#include "GNECrossing.h"
#include "GNEJunction.h"


// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_Crossing, GNEChange, NULL, 0)

// ===========================================================================
// member method definitions
// ===========================================================================


/// @brief constructor for creating an crossing
GNEChange_Crossing::GNEChange_Crossing(GNEJunction* junctionParent, const std::vector<NBEdge*>& edges,
                                       double width, bool priority, int customTLIndex, const PositionVector& customShape, bool selected, bool forward):
    GNEChange(junctionParent->getNet(), forward),
    myJunctionParent(junctionParent),
    myEdges(edges),
    myWidth(width),
    myPriority(priority),
    myCustomTLIndex(customTLIndex),
    myCustomShape(customShape),
    mySelected(selected) {
    assert(myNet);
}


GNEChange_Crossing::~GNEChange_Crossing() {
    assert(myNet);
}


void GNEChange_Crossing::undo() {
    if (myForward) {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("removing " + toString(SUMO_TAG_CROSSING) + " from " + toString(myJunctionParent->getTag()) + " '" + myJunctionParent->getID() + "'");
        }
        // remove crossing of NBNode
        myJunctionParent->getNBNode()->removeCrossing(myEdges);
        // rebuild GNECrossings
        myJunctionParent->rebuildGNECrossings();
        // Check if Flag "haveNetworkCrossings" has to be disabled
        if ((myNet->netHasGNECrossings() == false) && (myNet->getNetBuilder()->haveNetworkCrossings())) {
            // change flag of NetBuilder (For build GNECrossing)
            myNet->getNetBuilder()->setHaveNetworkCrossings(false);
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Changed flag netBuilder::haveNetworkCrossings from 'true' to 'false'");
            }
        }
        // Update view
        myNet->getViewNet()->update();
    } else {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            std::string selected = mySelected ? ("a previously selected ") : ("");
            WRITE_WARNING("Adding " + selected + toString(SUMO_TAG_CROSSING) + " into " + toString(myJunctionParent->getTag()) + " '" + myJunctionParent->getID() + "'");
        }
        // add crossing of NBNode
        myJunctionParent->getNBNode()->addCrossing(myEdges, myWidth, myPriority, myCustomTLIndex,  myCustomShape);
        // Check if Flag "haveNetworkCrossings" has to be enabled
        if (myNet->getNetBuilder()->haveNetworkCrossings() == false) {
            myNet->getNetBuilder()->setHaveNetworkCrossings(true);
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Changed flag netBuilder::haveNetworkCrossings from 'false' to 'true'");
            }
        }
        // rebuild GNECrossings
        myJunctionParent->rebuildGNECrossings();
        // check if created GNECrossing must be selected
        if (mySelected) {
            // iterate over GNECrossing of junction to find GNECrossing and select it
            for (std::vector<GNECrossing*>::const_iterator i = myJunctionParent->getGNECrossings().begin(); i != myJunctionParent->getGNECrossings().end(); i++) {
                NBNode::Crossing crossingFromJunction = *(*i)->getNBCrossing();
                if (crossingFromJunction.edges == myEdges) {
                    gSelected.select((*i)->getGlID());
                }
            }
        }
        // Update view
        myNet->getViewNet()->update();
    }
}


void GNEChange_Crossing::redo() {
    if (myForward) {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            std::string selected = mySelected ? ("a previously selected ") : ("");
            WRITE_WARNING("Adding " + selected + toString(SUMO_TAG_CROSSING) + " into " + toString(myJunctionParent->getTag()) + " '" + myJunctionParent->getID() + "'");
        }
        // add crossing of NBNode and update geometry
        myJunctionParent->getNBNode()->addCrossing(myEdges, myWidth, myPriority);
        // Check if Flag "haveNetworkCrossings" has to be enabled
        if (myNet->getNetBuilder()->haveNetworkCrossings() == false) {
            myNet->getNetBuilder()->setHaveNetworkCrossings(true);
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Changed flag netBuilder::haveNetworkCrossings from 'false' to 'true'");
            }
        }
        // rebuild GNECrossings
        myJunctionParent->rebuildGNECrossings();
        // check if created GNECrossing must be selected
        if (mySelected) {
            // iterate over GNECrossing of junction to find GNECrossing and select it
            for (std::vector<GNECrossing*>::const_iterator i = myJunctionParent->getGNECrossings().begin(); i != myJunctionParent->getGNECrossings().end(); i++) {
                NBNode::Crossing crossingFromJunction = *(*i)->getNBCrossing();
                if (crossingFromJunction.edges == myEdges) {
                    gSelected.select((*i)->getGlID());
                }
            }
        }
        // Update view
        myNet->getViewNet()->update();
    } else {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Removing " + toString(SUMO_TAG_CROSSING) + " from " + toString(myJunctionParent->getTag()) + " '" + myJunctionParent->getID() + "'");
        }
        // remove crossing of NBNode and update geometry
        myJunctionParent->getNBNode()->removeCrossing(myEdges);
        // rebuild GNECrossings
        myJunctionParent->rebuildGNECrossings();
        // Check if Flag "haveNetworkCrossings" has to be disabled
        if ((myNet->netHasGNECrossings() == false) && (myNet->getNetBuilder()->haveNetworkCrossings())) {
            // change flag of NetBuilder (For build GNECrossing)
            myNet->getNetBuilder()->setHaveNetworkCrossings(false);
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Changed flag netBuilder::haveNetworkCrossings from 'true' to 'false'");
            }
        }
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
