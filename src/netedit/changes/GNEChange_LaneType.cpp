/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    GNEChange_LaneType.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2020
///
// A network change in which a single laneType is created or deleted
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/elements/network/GNEEdgeType.h>
#include <netedit/elements/network/GNELaneType.h>

#include "GNEChange_LaneType.h"

// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_LaneType, GNEChange, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================


/// @brief constructor for creating an laneType
GNEChange_LaneType::GNEChange_LaneType(GNELaneType* laneType, const int position, bool forward):
    GNEChange(laneType, forward, laneType->isAttributeCarrierSelected()),
    myLaneType(laneType),
    myPosition(position) {
    laneType->incRef("GNEChange_LaneType");
}


GNEChange_LaneType::~GNEChange_LaneType() {
    myLaneType->decRef("GNEChange_LaneType");
    if (myLaneType->unreferenced()) {
        // get lane types
        const auto& laneTypes = myLaneType->getEdgeTypeParent()->getLaneTypes();
        // remove it from edgeType
        if (std::find(laneTypes.begin(), laneTypes.end(), myLaneType) != laneTypes.end()) {
            myLaneType->getEdgeTypeParent()->removeLaneType(myLaneType);
        }
        // show extra information for tests
        WRITE_DEBUG("Deleting unreferenced " + myLaneType->getTagStr() + " '" + myLaneType->getID() + "' GNEChange_LaneType");
        // delete laneType
        delete myLaneType;
    }
}


void
GNEChange_LaneType::undo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Removing " + myLaneType->getTagStr() + " from " + toString(SUMO_TAG_NET));
        // delete laneType from edgeType parent
        myLaneType->getEdgeTypeParent()->removeLaneType(myLaneType);
    } else {
        // show extra information for tests
        WRITE_DEBUG("Adding " + myLaneType->getTagStr() + " into " + toString(SUMO_TAG_NET));
        // insert laneType into edgeType parent
        myLaneType->getEdgeTypeParent()->addLaneType(myLaneType, myPosition);
    }
    // enable save networkElements
    myLaneType->getNet()->requireSaveNet(true);
}


void
GNEChange_LaneType::redo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Adding " + myLaneType->getTagStr() + " into " + toString(SUMO_TAG_NET));
        // insert laneType into edgeType parent
        myLaneType->getEdgeTypeParent()->addLaneType(myLaneType, myPosition);
    } else {
        // show extra information for tests
        WRITE_DEBUG("Removing " + myLaneType->getTagStr() + " from " + toString(SUMO_TAG_NET));
        // delete laneType from edgeType parent
        myLaneType->getEdgeTypeParent()->removeLaneType(myLaneType);
    }
    // enable save networkElements
    myLaneType->getNet()->requireSaveNet(true);
}


FXString
GNEChange_LaneType::undoName() const {
    if (myForward) {
        return "Undo create laneType";
    } else {
        return "Undo delete laneType";
    }
}


FXString
GNEChange_LaneType::redoName() const {
    if (myForward) {
        return "Redo create laneType";
    } else {
        return "Redo delete laneType";
    }
}
