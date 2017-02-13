/****************************************************************************/
/// @file    GNEChange_Additional.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
/// A network change in which a busStop is created or deleted
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
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

#include "GNEChange_Additional.h"
#include "GNENet.h"
#include "GNELane.h"
#include "GNEAdditional.h"
#include "GNEViewNet.h"
#include "GNEDetectorE3.h"
#include "GNEDetectorEntry.h"
#include "GNEDetectorExit.h"
#include "GNEStoppingPlace.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


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
    myE3Parent(NULL) {
    assert(myNet);
    myAdditional->incRef("GNEChange_Additional");
    // handle additionals with lane parent
    if (myAdditional->getLane() != NULL) {
        myLaneParent = myAdditional->getLane();
    }
    // handle additional with childs
    if (myAdditional->getTag() == SUMO_TAG_E3DETECTOR) {
        GNEDetectorE3* E3 = dynamic_cast<GNEDetectorE3*>(myAdditional);
        myEntryChilds = E3->myGNEDetectorEntrys;
        myExitChilds = E3->myGNEDetectorExits;
    } else if (myAdditional->getTag() == SUMO_TAG_DET_ENTRY) {
        myE3Parent = dynamic_cast<GNEDetectorEntry*>(myAdditional)->getE3Parent();
    } else if (myAdditional->getTag() == SUMO_TAG_DET_EXIT) {
        myE3Parent = dynamic_cast<GNEDetectorExit*>(myAdditional)->getE3Parent();
    }
}


GNEChange_Additional::~GNEChange_Additional() {
    assert(myAdditional);
    myAdditional->decRef("GNEChange_Additional");
    if (myAdditional->unreferenced()) {
        delete myAdditional;
    }
}


void
GNEChange_Additional::undo() {
    if (myForward) {
        myNet->deleteAdditional(myAdditional);
        // If additional own a Lane parent, remove it from lane
        if (myAdditional->getLane() != NULL) {
            assert(myLaneParent);
            myAdditional->getLane()->removeAdditionalChild(myAdditional);
        }
        // If additional is an E3 Detector, delete Entry/Exit childs
        if (myAdditional->getTag() == SUMO_TAG_E3DETECTOR) {
            for (std::vector<GNEDetectorEntry*>::iterator i = myEntryChilds.begin(); i != myEntryChilds.end(); i++) {
                myNet->deleteAdditional(*i);
            }
            for (std::vector<GNEDetectorExit*>::iterator i = myExitChilds.begin(); i != myExitChilds.end(); i++) {
                myNet->deleteAdditional(*i);
            }
        }
        // if additiona is an Entry detector, remove it from E3 parent
        if (myAdditional->getTag() == SUMO_TAG_DET_ENTRY) {
            assert(myE3Parent);
            myE3Parent->removeEntryChild(dynamic_cast<GNEDetectorEntry*>(myAdditional));
            myE3Parent->updateGeometry();
            myNet->getViewNet()->update();
        }
        // if additiona is an Exit detector, remove it from E3 parent
        if (myAdditional->getTag() == SUMO_TAG_DET_EXIT) {
            assert(myE3Parent);
            myE3Parent->removeExitChild(dynamic_cast<GNEDetectorExit*>(myAdditional));
            myE3Parent->updateGeometry();
            myNet->getViewNet()->update();
        }
    } else {
        myNet->insertAdditional(myAdditional);
        // If additional own a Lane parent, add it to lane
        if (myAdditional->getLane() != NULL) {
            assert(myLaneParent);
            myAdditional->getLane()->addAdditionalChild(myAdditional);
        }
        // If additional is an E3 detector, add it their Entry/Exit childs
        if (myAdditional->getTag() == SUMO_TAG_E3DETECTOR) {
            for (std::vector<GNEDetectorEntry*>::iterator i = myEntryChilds.begin(); i != myEntryChilds.end(); i++) {
                myNet->insertAdditional(*i);
            }
            for (std::vector<GNEDetectorExit*>::iterator i = myExitChilds.begin(); i != myExitChilds.end(); i++) {
                myNet->insertAdditional(*i);
            }
        }
        // If additional is an Exit detector, add id to E3 parent
        if (myAdditional->getTag() == SUMO_TAG_DET_ENTRY) {
            assert(myE3Parent);
            myE3Parent->addEntryChild(dynamic_cast<GNEDetectorEntry*>(myAdditional));
            myE3Parent->updateGeometry();
            myNet->getViewNet()->update();
        }
        // If additional is an Exit detector, add id to E3 parent
        if (myAdditional->getTag() == SUMO_TAG_DET_EXIT) {
            assert(myE3Parent);
            myE3Parent->addExitChild(dynamic_cast<GNEDetectorExit*>(myAdditional));
            myE3Parent->updateGeometry();
            myNet->getViewNet()->update();
        }
    }
}


void
GNEChange_Additional::redo() {
    if (myForward) {
        myNet->insertAdditional(myAdditional);
        // If additional own a Lane parent, add it to lane
        if (myAdditional->getLane() != NULL) {
            assert(myLaneParent);
            myAdditional->getLane()->addAdditionalChild(myAdditional);
        }
        // If additional is an E3 detector, add it their Entry/Exit childs
        if (myAdditional->getTag() == SUMO_TAG_E3DETECTOR) {
            for (std::vector<GNEDetectorEntry*>::iterator i = myEntryChilds.begin(); i != myEntryChilds.end(); i++) {
                myNet->insertAdditional(*i);
            }
            for (std::vector<GNEDetectorExit*>::iterator i = myExitChilds.begin(); i != myExitChilds.end(); i++) {
                myNet->insertAdditional(*i);
            }
        }
        // If additional is an Entry detector, add id to E3 parent
        if (myAdditional->getTag() == SUMO_TAG_DET_ENTRY) {
            assert(myE3Parent);
            myE3Parent->addEntryChild(dynamic_cast<GNEDetectorEntry*>(myAdditional));
            myE3Parent->updateGeometry();
            myNet->getViewNet()->update();
        }
        // If additional is an Exit detector, add id to E3 parent
        if (myAdditional->getTag() == SUMO_TAG_DET_EXIT) {
            assert(myE3Parent);
            myE3Parent->addExitChild(dynamic_cast<GNEDetectorExit*>(myAdditional));
            myE3Parent->updateGeometry();
            myNet->getViewNet()->update();
        }
    } else {
        myNet->deleteAdditional(myAdditional);
        // if additionl own a Lane Parent, remove it from lane
        if (myAdditional->getLane() != NULL) {
            assert(myLaneParent);
            myAdditional->getLane()->removeAdditionalChild(myAdditional);
        }
        // If additional is an E3 Detector, delete Entry/Exit childs
        if (myAdditional->getTag() == SUMO_TAG_E3DETECTOR) {
            for (std::vector<GNEDetectorEntry*>::iterator i = myEntryChilds.begin(); i != myEntryChilds.end(); i++) {
                myNet->deleteAdditional(*i);
            }
            for (std::vector<GNEDetectorExit*>::iterator i = myExitChilds.begin(); i != myExitChilds.end(); i++) {
                myNet->deleteAdditional(*i);
            }
        }
        // if additiona is an Entry detector, remove it from E3 parent
        if (myAdditional->getTag() == SUMO_TAG_DET_ENTRY) {
            assert(myE3Parent);
            myE3Parent->removeEntryChild(dynamic_cast<GNEDetectorEntry*>(myAdditional));
            myE3Parent->updateGeometry();
            myNet->getViewNet()->update();
        }
        // if additiona is an Exit detector, remove it from E3 parent
        if (myAdditional->getTag() == SUMO_TAG_DET_EXIT) {
            assert(myE3Parent);
            myE3Parent->removeExitChild(dynamic_cast<GNEDetectorExit*>(myAdditional));
            myE3Parent->updateGeometry();
            myNet->getViewNet()->update();
        }
    }
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
