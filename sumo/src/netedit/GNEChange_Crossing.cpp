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
GNEChange_Crossing::GNEChange_Crossing(GNEJunction* junctionParent, const std::vector<NBEdge*>& edges, double width, bool priority, bool selected, bool forward):
    GNEChange(junctionParent->getNet(), forward),
    myJunctionParent(junctionParent),
    myEdges(edges),
    myWidth(width),
    myPriority(priority),
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
        if ((myNet->netHasGNECrossings() == false) && (myNet->getNetBuilder()->haveNetworkCrossings() == true)) {
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
            std::string selected = mySelected? ("a previously selected ") : ("");
            WRITE_WARNING("Adding " + selected + toString(SUMO_TAG_CROSSING) + " into " + toString(myJunctionParent->getTag()) + " '" + myJunctionParent->getID() + "'");
        }
        // add crossing of NBNode
        myJunctionParent->getNBNode()->addCrossing(myEdges, myWidth, myPriority, mySelected);
        // Check if Flag "haveNetworkCrossings" has to be enabled
        if(myNet->getNetBuilder()->haveNetworkCrossings() == false) {
            myNet->getNetBuilder()->setHaveNetworkCrossings(true);
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Changed flag netBuilder::haveNetworkCrossings from 'false' to 'true'");
            }
        }
        // rebuild GNECrossings
        myJunctionParent->rebuildGNECrossings();
        // check if created GNECrossing must be selected
        if(mySelected) {
            // iterate over GNECrossing of junction to find GNECrossing and select it
            for(std::vector<GNECrossing*>::const_iterator i = myJunctionParent->getGNECrossings().begin(); i != myJunctionParent->getGNECrossings().end(); i++) {
                NBNode::Crossing crossingFromJunction = *(*i)->getNBCrossing();
                if(crossingFromJunction.edges == myEdges) {
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
            std::string selected = mySelected? ("a previously selected ") : ("");
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
        if(mySelected) {
            // iterate over GNECrossing of junction to find GNECrossing and select it
            for(std::vector<GNECrossing*>::const_iterator i = myJunctionParent->getGNECrossings().begin(); i != myJunctionParent->getGNECrossings().end(); i++) {
                NBNode::Crossing crossingFromJunction = *(*i)->getNBCrossing();
                if(crossingFromJunction.edges == myEdges) {
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
        if((myNet->netHasGNECrossings() == false) && (myNet->getNetBuilder()->haveNetworkCrossings() == true)) {
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
