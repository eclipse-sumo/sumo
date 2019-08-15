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

GNEChange_Children::GNEChange_Children(GNEDemandElement* demandElementParent, GNEDemandElement* demandElementChild, const Operation operation) :
    GNEChange(demandElementParent->getViewNet()->getNet(), true),
    myDemandElementParent(demandElementParent),
    myDemandElementChild(demandElementChild),
    myOperation(operation),
    myOriginalElementChildren(demandElementParent->getDemandElementChildren()),
    myEditedElementChildren(demandElementParent->getDemandElementChildren()) {
    myDemandElementParent->incRef("GNEChange_Children");
    // obtain iterator to demandElementChild
    auto it = std::find(myEditedElementChildren.begin(), myEditedElementChildren.end(), myDemandElementChild);
    // edit myEditedElementChildren vector
    if (it != myEditedElementChildren.end()) {
        if ((operation == Operation::MOVE_FRONT) && (it != (myEditedElementChildren.end() - 1))) {
            // remove element
            it = myEditedElementChildren.erase(it);
            // insert again in a different position
            myEditedElementChildren.insert(it + 1, myDemandElementChild);
        } else if ((operation == Operation::MOVE_BACK) && (it != myEditedElementChildren.begin())) {
            // remove element
            it = myEditedElementChildren.erase(it);
            // insert again in a different position
            myEditedElementChildren.insert(it - 1, myDemandElementChild);
        }
    }
}


GNEChange_Children::~GNEChange_Children() {
    assert(myDemandElementParent);
    myDemandElementParent->decRef("GNEChange_Children");
}


void
GNEChange_Children::undo() {
    if (myForward) {
        // continue depending of myOperation
        if (myOperation == GNEChange_Children::Operation::MOVE_FRONT) {
            // show extra information for tests
            WRITE_DEBUG("Moving front " + myDemandElementChild->getTagStr() + " within demandElement parent '" + myDemandElementParent->getID() + "' in GNEChange_Children");
            // restore demand element child original vector in myDemandElementChild
            myDemandElementParent->myDemandElementChildren = myOriginalElementChildren;
        } else if (myOperation == GNEChange_Children::Operation::MOVE_BACK) {
            // show extra information for tests
            WRITE_DEBUG("Moving back " + myDemandElementChild->getTagStr() + " within demandElement parent '" + myDemandElementParent->getID() + "' in GNEChange_Children");
            // restore demand element child original vector in myDemandElementChild
            myDemandElementParent->myDemandElementChildren = myOriginalElementChildren;
        }
    } else {
        // continue depending of myOperation
        if (myOperation == GNEChange_Children::Operation::MOVE_FRONT) {
            // show extra information for tests
            WRITE_DEBUG("Moving front " + myDemandElementChild->getTagStr() + " within demandElement parent '" + myDemandElementParent->getID() + "' in GNEChange_Children");
            // set demand element child edited vector in myDemandElementChild
            myDemandElementParent->myDemandElementChildren = myEditedElementChildren;
        } else if (myOperation == GNEChange_Children::Operation::MOVE_BACK) {
            // show extra information for tests
            WRITE_DEBUG("Moving back " + myDemandElementChild->getTagStr() + " within demandElement parent '" + myDemandElementParent->getID() + "' in GNEChange_Children");
            // set demand element child edited vector in myDemandElementChild
            myDemandElementParent->myDemandElementChildren = myEditedElementChildren;
        }
    }
    // Requiere always save childrens
    myNet->requiereSaveDemandElements(true);
}


void
GNEChange_Children::redo() {
    if (myForward) {
        // continue depending of myOperation
        if (myOperation == GNEChange_Children::Operation::MOVE_FRONT) {
            // show extra information for tests
            WRITE_DEBUG("Moving front " + myDemandElementChild->getTagStr() + " within demandElement parent '" + myDemandElementParent->getID() + "' in GNEChange_Children");
            // set demand element child edited vector in myDemandElementChild
            myDemandElementParent->myDemandElementChildren = myEditedElementChildren;
        } else if (myOperation == GNEChange_Children::Operation::MOVE_BACK) {
            // show extra information for tests
            WRITE_DEBUG("Moving back " + myDemandElementChild->getTagStr() + " within demandElement parent '" + myDemandElementParent->getID() + "' in GNEChange_Children");
            // set demand element child edited vector in myDemandElementChild
            myDemandElementParent->myDemandElementChildren = myEditedElementChildren;
        }
    } else {
        // continue depending of myOperation
        if (myOperation == GNEChange_Children::Operation::MOVE_FRONT) {
            // show extra information for tests
            WRITE_DEBUG("Moving front " + myDemandElementChild->getTagStr() + " within demandElement parent '" + myDemandElementParent->getID() + "' in GNEChange_Children");
            // restore demand element child original vector in myDemandElementChild
            myDemandElementParent->myDemandElementChildren = myOriginalElementChildren;
        } else if (myOperation == GNEChange_Children::Operation::MOVE_BACK) {
            // show extra information for tests
            WRITE_DEBUG("Moving back " + myDemandElementChild->getTagStr() + " within demandElement parent '" + myDemandElementParent->getID() + "' in GNEChange_Children");
            // restore demand element child original vector in myDemandElementChild
            myDemandElementParent->myDemandElementChildren = myOriginalElementChildren;
        }
    }
    // Requiere always save childrens
    myNet->requiereSaveDemandElements(true);
}


FXString
GNEChange_Children::undoName() const {
    if (myForward) {
        // check myOperation
        if (myOperation == GNEChange_Children::Operation::MOVE_FRONT) {
            return ("Undo moving up " + myDemandElementChild->getTagStr()).c_str();
        } else if (myOperation == GNEChange_Children::Operation::MOVE_BACK) {
            return ("Undo moving down " + myDemandElementChild->getTagStr()).c_str();
        } else {
            return ("Invalid operation");
        }
    } else {
        // check myOperation
        if (myOperation == GNEChange_Children::Operation::MOVE_FRONT) {
            return ("Undo moving down " + myDemandElementChild->getTagStr()).c_str();
        } else if (myOperation == GNEChange_Children::Operation::MOVE_BACK) {
            return ("Undo moving up " + myDemandElementChild->getTagStr()).c_str();
        } else {
            return ("Invalid operation");
        }
    }
}


FXString
GNEChange_Children::redoName() const {
    if (myForward) {
        // check myOperation
        if (myOperation == GNEChange_Children::Operation::MOVE_FRONT) {
            return ("Redo moving front " + myDemandElementParent->getTagStr()).c_str();
        } else if (myOperation == GNEChange_Children::Operation::MOVE_BACK) {
            return ("Redo moving back " + myDemandElementParent->getTagStr()).c_str();
        } else {
            return ("Invalid operation");
        }
    } else {
        // check myOperation
        if (myOperation == GNEChange_Children::Operation::MOVE_FRONT) {
            return ("Redo moving front " + myDemandElementParent->getTagStr()).c_str();
        } else if (myOperation == GNEChange_Children::Operation::MOVE_BACK) {
            return ("Redo moving back " + myDemandElementParent->getTagStr()).c_str();
        } else {
            return ("Invalid operation");
        }
    }
}
