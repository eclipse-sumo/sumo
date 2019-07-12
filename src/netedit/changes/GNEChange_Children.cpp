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

GNEChange_Children::GNEChange_Children(GNEDemandElement* demandElementParent, bool forward) :
    GNEChange(demandElementParent->getViewNet()->getNet(), forward),
    myDemandElementParent(demandElementParent),
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
        // show extra information for tests
        WRITE_DEBUG("Removing " + myDemandElementParent->getTagStr() + " '" + myDemandElementParent->getID() + "' in GNEChange_Children");
        /*
        // delete children from net
        myNet->deleteChildren(myChildren, false);
        // Remove children from parent elements
        for (const auto& i : myEdgeParents) {
            i->removeChildrenChild(myChildren);
        }
        */
    } else {
        // show extra information for tests
        WRITE_DEBUG("Adding " + myDemandElementParent->getTagStr() + " '" + myDemandElementParent->getID() + "' in GNEChange_Children");
        /*
        // insert children into net
        myNet->insertChildren(myChildren);
        // add children in parent elements
        for (const auto& i : myEdgeParents) {
            i->addChildrenChild(myChildren);
        }
        */
    }
    // Requiere always save childrens
    myNet->requiereSaveDemandElements(true);
}


void
GNEChange_Children::redo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Adding " + myDemandElementParent->getTagStr() + " '" + myDemandElementParent->getID() + "' in GNEChange_Children");
        /*
        // insert children into net
        myNet->insertChildren(myChildren);
        // add children in parent elements
        for (const auto& i : myEdgeParents) {
            i->addChildrenChild(myChildren);
        }
        */
    } else {
        // show extra information for tests
        WRITE_DEBUG("Removing " + myDemandElementParent->getTagStr() + " '" + myDemandElementParent->getID() + "' in GNEChange_Children");
        /*
        // delete children from net
        myNet->deleteChildren(myChildren, false);
        // Remove children from parent elements
        for (const auto& i : myEdgeParents) {
            i->removeChildrenChild(myChildren);
        }
        */
    }
    // Requiere always save childrens
    myNet->requiereSaveDemandElements(true);
}


FXString
GNEChange_Children::undoName() const {
    if (myForward) {
        return ("Undo create " + myDemandElementParent->getTagStr()).c_str();
    } else {
        return ("Undo delete " + myDemandElementParent->getTagStr()).c_str();
    }
}


FXString
GNEChange_Children::redoName() const {
    if (myForward) {
        return ("Redo create " + myDemandElementParent->getTagStr()).c_str();
    } else {
        return ("Redo delete " + myDemandElementParent->getTagStr()).c_str();
    }
}
