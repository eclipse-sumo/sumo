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
FXIMPLEMENT(GNEElementSet, FXGroupBoxModul, GNEElementSetMap, ARRAYNUMBER(GNEElementSetMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNEElementSet::GNEElementSet(GNESelectorFrame* selectorFrameParent, Supermode supermode,
                             SumoXMLTag defaultTag, SumoXMLAttr defaultAttr, const std::string& defaultValue) :
    FXGroupBoxModul(selectorFrameParent->getContentFrame(), "Element Set"),
    mySelectorFrameParent(selectorFrameParent),
    myMatchAttribute(nullptr),
    myMatchGenericDataAttribute(nullptr),
    myCurrentSet(Type::INVALID) {
    // Create MatchTagBox for tags and fill it
    mySetComboBox = new MFXIconComboBox(this, GUIDesignComboBoxNCol, this, MID_CHOOSEN_ELEMENTS, GUIDesignComboBox);
    // continue depending of supermode
    if (supermode == Supermode::NETWORK) {
        // append elements
        mySetComboBox->appendIconItem("Network", GUIIconSubSys::getIcon(GUIIcon::MODECREATEEDGE));
        mySetComboBox->appendIconItem("Additional", GUIIconSubSys::getIcon(GUIIcon::MODEADDITIONAL));
        mySetComboBox->appendIconItem("Shape", GUIIconSubSys::getIcon(GUIIcon::MODEPOLYGON));
        mySetComboBox->appendIconItem("TAZ", GUIIconSubSys::getIcon(GUIIcon::MODETAZ));
        // set default set
        myCurrentSet = Type::NETWORK;
        // build MatchAttribute
        myMatchAttribute = new GNEMatchAttribute(this, defaultTag, defaultAttr, defaultValue);
    } else if (supermode == Supermode::DEMAND) {
        // append elements
        mySetComboBox->appendIconItem("Demand", GUIIconSubSys::getIcon(GUIIcon::SUPERMODEDEMAND));
        // set default set
        myCurrentSet = Type::DEMAND;
        // build MatchAttribute
        myMatchAttribute = new GNEMatchAttribute(this, defaultTag, defaultAttr, defaultValue);
    } else if (supermode == Supermode::DATA) {
        // append elements
        mySetComboBox->appendIconItem("Data", GUIIconSubSys::getIcon(GUIIcon::SUPERMODEDATA));
        // set default set
        myCurrentSet = Type::DATA;
        /// build MatchGenericAttribute
        myMatchGenericDataAttribute = new GNEMatchGenericDataAttribute(this, defaultTag, defaultAttr, defaultValue);
    } else {
        throw ProcessError("Invalid supermode");
    }
    // set visible items
    mySetComboBox->setNumVisible(mySetComboBox->getNumItems());
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
    // show myMatchAttribute (if exist)
    if (myMatchAttribute) {
        myMatchAttribute->showMatchAttribute(myCurrentSet);
        // first check if myCurrentSet is invalid
        if (myCurrentSet == Type::INVALID) {
            // disable macht attribute
            myMatchAttribute->disableMatchAttribute();
        } else {
            // enable match attribute
            myMatchAttribute->enableMatchAttribute();
        }
    }
    // show myMatchGenericDataAttribute (if exist)
    if (myMatchGenericDataAttribute) {
        myMatchGenericDataAttribute->showMatchGenericDataAttribute();
        // first check if myCurrentSet is invalid
        if (myCurrentSet == Type::INVALID) {
            // disable macht attribute
            myMatchGenericDataAttribute->disableMatchGenericDataAttribute();
        } else {
            // enable match attribute
            myMatchGenericDataAttribute->enableMatchGenericDataAttribute();
        }
    }
}


void
GNEElementSet::hideElementSet() {
    // hide match attribute (if exist)
    if (myMatchAttribute) {
        myMatchAttribute->hideMatchAttribute();
    }
    // hide match generic data attribute (if exist)
    if (myMatchGenericDataAttribute) {
        myMatchGenericDataAttribute->hideMatchGenericDataAttribute();
    }
    // hide group box
    hide();
}


long
GNEElementSet::onCmdSelectElementSet(FXObject*, FXSelector, void*) {
    // check depending of current supermode
    if (mySelectorFrameParent->getViewNet()->getEditModes().isCurrentSupermodeNetwork()) {
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
    } else if (mySelectorFrameParent->getViewNet()->getEditModes().isCurrentSupermodeDemand()) {
        if (mySetComboBox->getText() == "Demand") {
            myCurrentSet = Type::DEMAND;
        } else {
            myCurrentSet = Type::INVALID;
        }
    } else if (mySelectorFrameParent->getViewNet()->getEditModes().isCurrentSupermodeData()) {
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
        // hide match attribute
        if (myMatchAttribute) {
            myMatchAttribute->hideMatchAttribute();
        }
        if (myMatchGenericDataAttribute) {
            myMatchGenericDataAttribute->hideMatchGenericDataAttribute();
        }
    } else {
        // set default color
        mySetComboBox->setTextColor(FXRGB(0, 0, 0));
        // set current set
        if (myMatchAttribute) {
            myMatchAttribute->showMatchAttribute(myCurrentSet);
        }
        if (myMatchGenericDataAttribute) {
            myMatchGenericDataAttribute->showMatchGenericDataAttribute();
        }
    }
    return 1;
}

/****************************************************************************/
