/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
#include <utils/common/MsgHandler.h>
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
FXIMPLEMENT(GNEElementSet, MFXGroupBoxModule, GNEElementSetMap, ARRAYNUMBER(GNEElementSetMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNEElementSet::GNEElementSet(GNESelectorFrame* selectorFrameParent, Supermode supermode,
                             SumoXMLTag defaultTag, SumoXMLAttr defaultAttr, const std::string& defaultValue) :
    MFXGroupBoxModule(selectorFrameParent, TL("Element Set")),
    mySelectorFrameParent(selectorFrameParent),
    myMatchAttribute(nullptr),
    myMatchGenericDataAttribute(nullptr),
    myCurrentSet(Type::INVALID) {
    // Create MatchTagBox for tags and fill it
    mySetComboBox = new MFXComboBoxIcon(getCollapsableFrame(), GUIDesignComboBoxNCol, false, GUIDesignComboBoxVisibleItemsMedium,
                                        this, MID_CHOOSEN_ELEMENTS, GUIDesignComboBox);
    // continue depending of supermode
    if (supermode == Supermode::NETWORK) {
        // append elements
        mySetComboBox->appendIconItem(TL("Network"), GUIIconSubSys::getIcon(GUIIcon::MODECREATEEDGE));
        mySetComboBox->appendIconItem(TL("Additional"), GUIIconSubSys::getIcon(GUIIcon::MODEADDITIONAL));
        mySetComboBox->appendIconItem(TL("Shape"), GUIIconSubSys::getIcon(GUIIcon::MODESHAPE));
        mySetComboBox->appendIconItem(tl("TAZ"), GUIIconSubSys::getIcon(GUIIcon::MODETAZ));
        // set default set
        myCurrentSet = Type::NETWORK;
    } else if (supermode == Supermode::DEMAND) {
        // append elements
        mySetComboBox->appendIconItem(TL("Demand"), GUIIconSubSys::getIcon(GUIIcon::SUPERMODEDEMAND));
        // set default set
        myCurrentSet = Type::DEMAND;
    } else if (supermode == Supermode::DATA) {
        // append elements
        mySetComboBox->appendIconItem(TL("GenericDatas"), GUIIconSubSys::getIcon(GUIIcon::SUPERMODEDATA));
        // set default set
        myCurrentSet = Type::GENERICDATA;
    } else {
        throw ProcessError(TL("Invalid supermode"));
    }
    // build MatchAttribute
    myMatchAttribute = new GNEMatchAttribute(this, defaultTag, defaultAttr, defaultValue);
    /// build MatchGenericAttribute
    myMatchGenericDataAttribute = new GNEMatchGenericDataAttribute(this, defaultTag, defaultAttr, defaultValue);
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
    // show myMatchAttribute (if exist)
    if (myCurrentSet == Type::GENERICDATA) {
        myMatchAttribute->hideMatchAttribute();
        myMatchGenericDataAttribute->showMatchGenericDataAttribute();
        // first check if myCurrentSet is invalid
        if (myCurrentSet == Type::INVALID) {
            // disable macht attribute
            myMatchGenericDataAttribute->disableMatchGenericDataAttribute();
        } else {
            // enable match attribute
            myMatchGenericDataAttribute->enableMatchGenericDataAttribute();
        }
    } else {
        myMatchAttribute->showMatchAttribute(myCurrentSet);
        myMatchGenericDataAttribute->hideMatchGenericDataAttribute();
        // first check if myCurrentSet is invalid
        if (myCurrentSet == Type::INVALID) {
            // disable macht attribute
            myMatchAttribute->disableMatchAttribute();
        } else {
            // enable match attribute
            myMatchAttribute->enableMatchAttribute();
        }
    }
    // first show group box
    show();
}


void
GNEElementSet::hideElementSet() {
    // hide both match attribute
    myMatchAttribute->hideMatchAttribute();
    myMatchGenericDataAttribute->hideMatchGenericDataAttribute();
    // hide group box
    hide();
}


long
GNEElementSet::onCmdSelectElementSet(FXObject*, FXSelector, void*) {
    // check depending of current supermode
    if (mySelectorFrameParent->getViewNet()->getEditModes().isCurrentSupermodeNetwork()) {
        if (mySetComboBox->getText() == TL("Network")) {
            myCurrentSet = Type::NETWORK;
        } else if (mySetComboBox->getText() == TL("Additional")) {
            myCurrentSet = Type::ADDITIONAL;
        } else if (mySetComboBox->getText() == TL("TAZ")) {
            myCurrentSet = Type::TAZ;
            mySetComboBox->setTextColor(FXRGB(0, 0, 0));
        } else if (mySetComboBox->getText() == TL("Shape")) {
            myCurrentSet = Type::SHAPE;
        } else {
            myCurrentSet = Type::INVALID;
        }
    } else if (mySelectorFrameParent->getViewNet()->getEditModes().isCurrentSupermodeDemand()) {
        if (mySetComboBox->getText() == TL("Demand")) {
            myCurrentSet = Type::DEMAND;
        } else {
            myCurrentSet = Type::INVALID;
        }
    } else if (mySelectorFrameParent->getViewNet()->getEditModes().isCurrentSupermodeData()) {
        if (mySetComboBox->getText() == TL("GenericDatas")) {
            myCurrentSet = Type::GENERICDATA;
        } else {
            myCurrentSet = Type::INVALID;
        }
    }
    // set color
    if (myCurrentSet == Type::INVALID) {
        // set invalid color
        mySetComboBox->setTextColor(FXRGB(255, 0, 0));
        // hide match attribute
        myMatchAttribute->hideMatchAttribute();
        myMatchGenericDataAttribute->hideMatchGenericDataAttribute();
    } else {
        // set default color
        mySetComboBox->setTextColor(FXRGB(0, 0, 0));
        // show match attribute
        if (myCurrentSet == Type::GENERICDATA) {
            myMatchAttribute->hideMatchAttribute();
            myMatchGenericDataAttribute->showMatchGenericDataAttribute();
        } else {
            myMatchAttribute->showMatchAttribute(myCurrentSet);
            myMatchGenericDataAttribute->hideMatchGenericDataAttribute();
        }
    }
    return 1;
}

/****************************************************************************/
