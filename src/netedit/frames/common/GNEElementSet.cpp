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
/// @file    GNEElementSet.cpp
/// @author  Jakob Erdmann
/// @date    Mar 2011
///
// The Widget for modifying selections of network-elements
/****************************************************************************/
#include <config.h>

#include <netedit/GNEViewNet.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEElementSet.h"
#include "GNEMatchAttribute.h"
#include "GNEMatchGenericDataAttribute.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEElementSet) GNEElementSetMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_ELEMENTS,   GNEElementSet::onCmdSelectElementSet)
};

// Object implementation
FXIMPLEMENT(GNEElementSet, FXGroupBox, GNEElementSetMap, ARRAYNUMBER(GNEElementSetMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNEElementSet::GNEElementSet(GNESelectorFrame* selectorFrameParent, Supermode supermode, 
    SumoXMLTag defaultTag, SumoXMLAttr defaultAttr, const std::string &defaultValue) :
    FXGroupBox(selectorFrameParent->myContentFrame, "Element Set", GUIDesignGroupBoxFrame),
    mySelectorFrameParent(selectorFrameParent),
    myMatchAttribute(nullptr),
    myCurrentSet(Type::INVALID) {
    // Create MatchTagBox for tags and fill it
    mySetComboBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_CHOOSEN_ELEMENTS, GUIDesignComboBox);
    // continue depending of supermode
    if (supermode == Supermode::NETWORK) {
        // append elements
        mySetComboBox->appendItem("Network");
        mySetComboBox->appendItem("Additional");
        mySetComboBox->appendItem("Shape");
        mySetComboBox->appendItem("TAZ");
        // set default set
        myCurrentSet = Type::NETWORK;
    } else if (supermode == Supermode::DEMAND) {
        // append elements
        mySetComboBox->appendItem("Demand");
        // set default set
        myCurrentSet = Type::DEMAND;
    } else if (supermode == Supermode::DATA) {
        // append elements
        mySetComboBox->appendItem("Data");
        // set default set
        myCurrentSet = Type::DATA;
    } else {
        throw ProcessError("Invalid supermode");
    }
    // set visible items
    mySetComboBox->setNumVisible(mySetComboBox->getNumItems());
    // build MatchAttribute
    myMatchAttribute = new GNEMatchAttribute(this, defaultTag, defaultAttr, defaultValue);
}


GNEElementSet::~GNEElementSet() {}


GNESelectorFrame* 
GNEElementSet::getSelectorFrameParent() const {
    return mySelectorFrameParent;
}


GNEElementSet::Type
GNEElementSet::getElementSet() const {
    return myCurrentSet;
}


void
GNEElementSet::showElementSet() {
    // first show group box
    show();
    // show myMatchAttribute
    myMatchAttribute->showMatchAttribute(myCurrentSet);
    // first check if myCurrentSet is invalid
    if (myCurrentSet == Type::INVALID) {
        // disable macht attribute
        myMatchAttribute->disableMatchAttribute();
    } else {
        // enable match attribute
        myMatchAttribute->enableMatchAttribute();
        mySelectorFrameParent->myMatchGenericDataAttribute->hideMatchGenericDataAttribute();
    }
}


void
GNEElementSet::hideElementSet() {
    // hide match attribute
    myMatchAttribute->hideMatchAttribute();
    // hide group box
    hide();
}


long
GNEElementSet::onCmdSelectElementSet(FXObject*, FXSelector, void*) {
    // check depending of current supermode
    if (mySelectorFrameParent->myViewNet->getEditModes().isCurrentSupermodeNetwork()) {
        if (mySetComboBox->getText() == "Network") {
            myCurrentSet = Type::NETWORK;
        } else if (mySetComboBox->getText() == "Additional") {
            myCurrentSet = Type::ADDITIONAL;
        } else if (mySetComboBox->getText() == "TAZ") {
            myCurrentSet = Type::TAZ;
            mySetComboBox->setTextColor(FXRGB(0, 0, 0));
        } else if (mySetComboBox->getText() == "Shape") {
            myCurrentSet = Type::SHAPE;
        } else {
            myCurrentSet = Type::INVALID;
        }
    } else if (mySelectorFrameParent->myViewNet->getEditModes().isCurrentSupermodeDemand()) {
        if (mySetComboBox->getText() == "Demand") {
            myCurrentSet = Type::DEMAND;
        } else {
            myCurrentSet = Type::INVALID;
        }
    } else if (mySelectorFrameParent->myViewNet->getEditModes().isCurrentSupermodeData()) {
        if (mySetComboBox->getText() == "Data") {
            myCurrentSet = Type::DATA;
        } else {
            myCurrentSet = Type::INVALID;
        }
    }
    // set color
    if (myCurrentSet == Type::INVALID) {
        // set invalid color
        mySetComboBox->setTextColor(FXRGB(255, 0, 0));
    } else {
        // set default color
        mySetComboBox->setTextColor(FXRGB(0, 0, 0));
    }
    return 1;
}

/****************************************************************************/
