/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GNEChange_EdgeType.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2020
///
// A network change in which a single edgeType is created or deleted
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/frames/network/GNECreateEdgeFrame.h>
#include <netedit/elements/network/GNEEdgeType.h>

#include "GNEChange_EdgeType.h"

// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_EdgeType, GNEChange, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================


/// @brief constructor for creating an edgeType
GNEChange_EdgeType::GNEChange_EdgeType(GNEEdgeType* edgeType, bool forward):
    GNEChange(Supermode::NETWORK, edgeType, forward, edgeType->isAttributeCarrierSelected()),
    myEdgeType(edgeType) {
    edgeType->incRef("GNEChange_EdgeType");
}


GNEChange_EdgeType::~GNEChange_EdgeType() {
    myEdgeType->decRef("GNEChange_EdgeType");
    if (myEdgeType->unreferenced()) {
        // show extra information for tests
        WRITE_DEBUG("Deleting unreferenced " + myEdgeType->getTagStr() + " '" + myEdgeType->getID() + "' GNEChange_EdgeType");
        // make sure that edgeType isn't in net before removing
        if (myEdgeType->getNet()->getAttributeCarriers()->edgeTypeExist(myEdgeType)) {
            // delete edgeType from net
            myEdgeType->getNet()->getAttributeCarriers()->deleteEdgeType(myEdgeType);
        }
        // delete edgeType
        delete myEdgeType;
    }
}


void
GNEChange_EdgeType::undo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Removing " + myEdgeType->getTagStr() + " '" + myEdgeType->getID() + "' from " + toString(SUMO_TAG_NET));
        // delete edgeType from net
        myEdgeType->getNet()->getAttributeCarriers()->deleteEdgeType(myEdgeType);
    } else {
        // show extra information for tests
        WRITE_DEBUG("Adding " + myEdgeType->getTagStr() + " '" + myEdgeType->getID() + "' into " + toString(SUMO_TAG_NET));
        // insert edgeType into net
        myEdgeType->getNet()->getAttributeCarriers()->insertEdgeType(myEdgeType);
    }
    // refresh create edge frame
    if (myEdgeType->getNet()->getViewNet()->getViewParent()->getCreateEdgeFrame()->shown()) {
        myEdgeType->getNet()->getViewNet()->getViewParent()->getCreateEdgeFrame()->getEdgeTypeSelector()->refreshEdgeTypeSelector();
    }
    // enable save networkElements
    myEdgeType->getNet()->getSavingStatus()->requireSaveNetwork();
}


void
GNEChange_EdgeType::redo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Adding " + myEdgeType->getTagStr() + " '" + myEdgeType->getID() + "' into " + toString(SUMO_TAG_NET));
        // insert edgeType into net
        myEdgeType->getNet()->getAttributeCarriers()->insertEdgeType(myEdgeType);
    } else {
        // show extra information for tests
        WRITE_DEBUG("Removing " + myEdgeType->getTagStr() + " '" + myEdgeType->getID() + "' from " + toString(SUMO_TAG_NET));
        // delete edgeType from net
        myEdgeType->getNet()->getAttributeCarriers()->deleteEdgeType(myEdgeType);
    }
    // refresh create edge frame
    if (myEdgeType->getNet()->getViewNet()->getViewParent()->getCreateEdgeFrame()->shown()) {
        myEdgeType->getNet()->getViewNet()->getViewParent()->getCreateEdgeFrame()->getEdgeTypeSelector()->refreshEdgeTypeSelector();
    }
    // enable save networkElements
    myEdgeType->getNet()->getSavingStatus()->requireSaveNetwork();
}


std::string
GNEChange_EdgeType::undoName() const {
    if (myForward) {
        return TL("Undo create edgeType '") + myEdgeType->getID() + "'";
    } else {
        return TL("Undo delete edgeType '") + myEdgeType->getID() + "'";
    }
}


std::string
GNEChange_EdgeType::redoName() const {
    if (myForward) {
        return TL("Redo create edgeType '") + myEdgeType->getID() + "'";
    } else {
        return TL("Redo delete edgeType '") + myEdgeType->getID() + "'";
    }
}
