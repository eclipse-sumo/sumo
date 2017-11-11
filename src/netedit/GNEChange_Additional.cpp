/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2017 German Aerospace Center (DLR) and others.
/****************************************************************************/
//
//   This program and the accompanying materials
//   are made available under the terms of the Eclipse Public License v2.0
//   which accompanies this distribution, and is available at
//   http://www.eclipse.org/legal/epl-v20.html
//
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
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/MsgHandler.h>

#include "GNEChange_Additional.h"
#include "GNENet.h"
#include "GNELane.h"
#include "GNEEdge.h"
#include "GNEAdditional.h"
#include "GNEViewNet.h"
#include "GNEDetectorE3.h"
#include "GNEDetectorEntry.h"
#include "GNEDetectorExit.h"
#include "GNEStoppingPlace.h"
#include "GNERerouter.h"
#include "GNEVariableSpeedSign.h"


// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_Additional, GNEChange, NULL, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

GNEChange_Additional::GNEChange_Additional(GNEAdditional* additional, bool forward) :
    GNEChange(additional->getViewNet()->getNet(), forward),
    myAdditional(additional),
    myLaneParent(NULL),
    myEdgeParent(NULL),
    myAdditionalParent(NULL) {
    assert(myNet);
    myAdditional->incRef("GNEChange_Additional");
    // handle additionals with lane parent
    if (GNEAttributeCarrier::hasAttribute(myAdditional->getTag(), SUMO_ATTR_LANE)) {
        myLaneParent = myNet->retrieveLane(myAdditional->getAttribute(SUMO_ATTR_LANE));
    }
    // handle additionals with edge parent
    if (GNEAttributeCarrier::hasAttribute(myAdditional->getTag(), SUMO_ATTR_EDGE)) {
        myEdgeParent = myNet->retrieveEdge(myAdditional->getAttribute(SUMO_ATTR_EDGE));
    }
    // handle additional with childs
    if (myAdditional->getTag() == SUMO_TAG_DET_ENTRY) {
        myAdditionalParent = myAdditional->getAdditionalParent();
    } else if (myAdditional->getTag() == SUMO_TAG_DET_EXIT) {
        myAdditionalParent = myAdditional->getAdditionalParent();
    }

    myEdgeChilds = myAdditional->getEdgeChilds();
    myLaneChilds = myAdditional->getLaneChilds();
}


GNEChange_Additional::~GNEChange_Additional() {
    assert(myAdditional);
    myAdditional->decRef("GNEChange_Additional");
    if (myAdditional->unreferenced()) {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Deleting unreferenced " + toString(myAdditional->getTag()) + " '" + myAdditional->getID() + "'");
        }
        // make sure that additional isn't in net before removing
        if(myNet->retrieveAdditional(myAdditional->getID(), false)) {
            myNet->deleteAdditional(myAdditional);
        }
        delete myAdditional;
    }
}


void
GNEChange_Additional::undo() {
    if (myForward) {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Removing " + toString(myAdditional->getTag()) + " '" + myAdditional->getID() + "' in GNEChange_Additional");
        }
        // delete additional of test
        myNet->deleteAdditional(myAdditional);
        // 1 - If additional own a lane parent, remove it from lane
        if (myLaneParent) {
            myLaneParent->removeAdditionalChild(myAdditional);
        }
        // 2 - If additional own a lane parent, remove it from edge
        if (myEdgeParent) {
            myEdgeParent->removeAdditionalChild(myAdditional);
        }
        // 4 - If additiona is an Entry detector, remove it from E3 parent
        if (myAdditionalParent) {
            myAdditionalParent->removeAdditionalChild(myAdditional);
            myAdditionalParent->updateGeometry();
        }
        // 6 - if Additional if a rerouter, remove it of all of their edge childs
        if (myAdditional->getTag() == SUMO_TAG_REROUTER) {
            GNERerouter* rerouter = dynamic_cast<GNERerouter*>(myAdditional);
            for (auto i : myEdgeChilds) {
                i->removeAdditionalParent(rerouter);
            }
        }
        // 7 - if Additional if a VSS, remove it of all of their lane childs
        if (myAdditional->getTag() == SUMO_TAG_VSS) {
            GNEVariableSpeedSign* vss = dynamic_cast<GNEVariableSpeedSign*>(myAdditional);
            for (auto i : myLaneChilds) {
                i->removeAdditionalParent(vss);
            }
        }
    } else {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Adding " + toString(myAdditional->getTag()) + " '" + myAdditional->getID() + "' in GNEChange_Additional");
        }
        // insert additional of test
        myNet->insertAdditional(myAdditional);
        // 1 - If additional own a Lane parent, add it to lane
        if (myLaneParent) {
            myLaneParent->addAdditionalChild(myAdditional);
        }
        // 2 - If additional own a lane parent, add it to edge
        if (myEdgeParent != NULL) {
            myEdgeParent->addAdditionalChild(myAdditional);
        }
        // 4 - If additional is an Exit detector, add id to E3 parent
        if (myAdditionalParent) {
            myAdditionalParent->addAdditionalChild(myAdditional);
            myAdditionalParent->updateGeometry();
        }
        // 6 - if Additional if a rerouter, add it of all of their edge childs
        if (myAdditional->getTag() == SUMO_TAG_REROUTER) {
            GNERerouter* rerouter = dynamic_cast<GNERerouter*>(myAdditional);
            for (auto i : myEdgeChilds) {
                i->addAdditionalParent(rerouter);
            }
        }
        // 7 - if Additional if a VSS, add it into all of their lane childs
        if (myAdditional->getTag() == SUMO_TAG_VSS) {
            GNEVariableSpeedSign* vss = dynamic_cast<GNEVariableSpeedSign*>(myAdditional);
            for (auto i : myLaneChilds) {
                i->addAdditionalParent(vss);
            }
        }
    }
    // Requiere always save additionals
    myNet->requiereSaveAdditionals();
}


void
GNEChange_Additional::redo() {
    if (myForward) {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Adding " + toString(myAdditional->getTag()) + " '" + myAdditional->getID() + "' in GNEChange_Additional");
        }
        // insert additional into net
        myNet->insertAdditional(myAdditional);
        // 1 - If additional own a Lane parent, add it to lane
        if (myLaneParent) {
            myLaneParent->addAdditionalChild(myAdditional);
        }
        // 2 - If additional own a lane parent, add it to edge
        if (myEdgeParent != NULL) {
            myEdgeParent->addAdditionalChild(myAdditional);
        }
        // 4 - If additional is an Entry detector, add id to E3 parent
        if (myAdditionalParent) {
            myAdditionalParent->addAdditionalChild(myAdditional);
            myAdditionalParent->updateGeometry();
        }
        // 6 - if Additional if a rerouter, add it of all of their edge childs
        if (myAdditional->getTag() == SUMO_TAG_REROUTER) {
            GNERerouter* rerouter = dynamic_cast<GNERerouter*>(myAdditional);
            for (auto i : myEdgeChilds) {
                i->addAdditionalParent(rerouter);
            }
        }
        // 7 - if Additional if a VSS, add it into all of their lane childs
        if (myAdditional->getTag() == SUMO_TAG_VSS) {
            GNEVariableSpeedSign* vss = dynamic_cast<GNEVariableSpeedSign*>(myAdditional);
            for (auto i : myLaneChilds) {
                i->addAdditionalParent(vss);
            }
        }
    } else {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Removing " + toString(myAdditional->getTag()) + " '" + myAdditional->getID() + "' in GNEChange_Additional");
        }
        myNet->deleteAdditional(myAdditional);
        // 1 - If additional own a lane parent, remove it from lane
        if (myLaneParent) {
            myLaneParent->removeAdditionalChild(myAdditional);
        }
        // 2 - If additional own a lane parent, remove it from edge
        if (myEdgeParent) {
            myEdgeParent->removeAdditionalChild(myAdditional);
        }
        // 4 - If additiona is an Entry detector, remove it from E3 parent
        if (myAdditionalParent) {
            myAdditionalParent->removeAdditionalChild(myAdditional);
            myAdditionalParent->updateGeometry();
        }
        // 6 - if Additional if a rerouter, remove it of all of their edge childs
        if (myAdditional->getTag() == SUMO_TAG_REROUTER) {
            GNERerouter* rerouter = dynamic_cast<GNERerouter*>(myAdditional);
            for (auto i : myEdgeChilds) {
                i->removeAdditionalParent(rerouter);
            }
        }
        // 7 - if Additional if a VSS, remove it of all of their lane childs
        if (myAdditional->getTag() == SUMO_TAG_VSS) {
            GNEVariableSpeedSign* vss = dynamic_cast<GNEVariableSpeedSign*>(myAdditional);
            for (auto i : myLaneChilds) {
                i->removeAdditionalParent(vss);
            }
        }
    }
    // Requiere always save additionals
    myNet->requiereSaveAdditionals();
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
