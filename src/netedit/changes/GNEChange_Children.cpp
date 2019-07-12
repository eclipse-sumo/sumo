/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChange_Children.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2019
/// @version $Id$
///
// A network change used to modify sorting of hierarchical element childrens
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/demandelements/GNEDemandElement.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNENet.h>

#include "GNEChange_Children.h"

// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_Children, GNEChange, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

GNEChange_Children::GNEChange_Children(GNEDemandElement* demandElementParent, const GNEDemandElement* demandElementChild, const Operation operation) :
    GNEChange(demandElementParent->getViewNet()->getNet(), true),
    myDemandElementParent(demandElementParent),
    myDemandElementChild(demandElementChild),
    myOperation(operation),
    myDemandElementChildren(demandElementParent->getDemandElementChildren()) {
    myDemandElementParent->incRef("GNEChange_Children");
}


GNEChange_Children::~GNEChange_Children() {
    assert(myDemandElementParent);
    myDemandElementParent->decRef("GNEChange_Children");
}


void
GNEChange_Children::undo() {
    if (myForward) {
        // continue depending of myOperation
        if (myOperation == GNEChange_Children::Operation::MOVE_UP) {
            // show extra information for tests
            WRITE_DEBUG("Moving down " + myDemandElementChild->getTagStr() + " within parent '" + myDemandElementParent->getID() + "' in GNEChange_Children");
            /*** ***/
        } else {
            // show extra information for tests
            WRITE_DEBUG("Moving up " + myDemandElementChild->getTagStr() + " within parent '" + myDemandElementParent->getID() + "' in GNEChange_Children");
            /*** ***/
        }
    } else {
        // continue depending of myOperation
        if (myOperation == GNEChange_Children::Operation::MOVE_UP) {
            // show extra information for tests
            WRITE_DEBUG("Moving up " + myDemandElementChild->getTagStr() + " within parent '" + myDemandElementParent->getID() + "' in GNEChange_Children");
            /*** ***/
        } else {
            // show extra information for tests
            WRITE_DEBUG("Moving down " + myDemandElementChild->getTagStr() + " within parent '" + myDemandElementParent->getID() + "' in GNEChange_Children");
            /*** ***/
        }
    }
    // Requiere always save childrens
    myNet->requiereSaveDemandElements(true);
}


void
GNEChange_Children::redo() {
    if (myForward) {
        // continue depending of myOperation
        if (myOperation == GNEChange_Children::Operation::MOVE_UP) {
            // show extra information for tests
            WRITE_DEBUG("Moving up " + myDemandElementChild->getTagStr() + " within parent '" + myDemandElementParent->getID() + "' in GNEChange_Children");
            /*** ***/
        } else {
            // show extra information for tests
            WRITE_DEBUG("Moving down " + myDemandElementChild->getTagStr() + " within parent '" + myDemandElementParent->getID() + "' in GNEChange_Children");
            /*** ***/
        }
    } else {
        // continue depending of myOperation
        if (myOperation == GNEChange_Children::Operation::MOVE_UP) {
            // show extra information for tests
            WRITE_DEBUG("Moving down " + myDemandElementChild->getTagStr() + " within parent '" + myDemandElementParent->getID() + "' in GNEChange_Children");
            /*** ***/
        } else {
            // show extra information for tests
            WRITE_DEBUG("Moving up " + myDemandElementChild->getTagStr() + " within parent '" + myDemandElementParent->getID() + "' in GNEChange_Children");
            /*** ***/
        }
    }
    // Requiere always save childrens
    myNet->requiereSaveDemandElements(true);
}


FXString
GNEChange_Children::undoName() const {
    if (myForward) {
        // check myOperation
        if (myOperation == GNEChange_Children::Operation::MOVE_UP) {
            return ("Undo moving up " + myDemandElementParent->getTagStr()).c_str();
        } else {
            return ("Undo moving down " + myDemandElementParent->getTagStr()).c_str();
        }
    } else {
        // check myOperation
        if (myOperation == GNEChange_Children::Operation::MOVE_UP) {
            return ("Undo moving down " + myDemandElementParent->getTagStr()).c_str();
        } else {
            return ("Undo moving up " + myDemandElementParent->getTagStr()).c_str();
        }
    }
}


FXString
GNEChange_Children::redoName() const {
    if (myForward) {
        // check myOperation
        if (myOperation == GNEChange_Children::Operation::MOVE_UP) {
            return ("Redo moving up " + myDemandElementParent->getTagStr()).c_str();
        } else {
            return ("Redo moving down " + myDemandElementParent->getTagStr()).c_str();
        }
    } else {
        // check myOperation
        if (myOperation == GNEChange_Children::Operation::MOVE_UP) {
            return ("Redo moving down " + myDemandElementParent->getTagStr()).c_str();
        } else {
            return ("Redo moving up " + myDemandElementParent->getTagStr()).c_str();
        }
    }
}
