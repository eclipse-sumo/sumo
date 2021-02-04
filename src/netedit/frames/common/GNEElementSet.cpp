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
/// @file    GNESelectorFrame.cpp
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

GNEElementSet::GNEElementSet(GNESelectorFrame* selectorFrameParent) :
    FXGroupBox(selectorFrameParent->myContentFrame, "Element Set", GUIDesignGroupBoxFrame),
    mySelectorFrameParent(selectorFrameParent),
    myCurrentGNEElementSet(Type::NETWORKELEMENT) {
    // Create MatchTagBox for tags and fill it
    mySetComboBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_CHOOSEN_ELEMENTS, GUIDesignComboBox);
}


GNEElementSet::~GNEElementSet() {}


GNEElementSet::Type
GNEElementSet::getElementSet() const {
    return myCurrentGNEElementSet;
}


void
GNEElementSet::refreshElementSet() {
    // first clear item
    mySetComboBox->clearItems();
    // now fill elements depending of supermode
    if (mySelectorFrameParent->myViewNet->getEditModes().isCurrentSupermodeNetwork()) {
        mySetComboBox->appendItem("network element");
        mySetComboBox->appendItem("Additional");
        mySetComboBox->appendItem("Shape");
        // show Modul
        show();
        // set num items
        mySetComboBox->setNumVisible(mySetComboBox->getNumItems());
    } else if (mySelectorFrameParent->myViewNet->getEditModes().isCurrentSupermodeDemand()) {
        mySetComboBox->appendItem("Demand Element");
        // hide Modul (because there is only an element)
        hide();
    } else if (mySelectorFrameParent->myViewNet->getEditModes().isCurrentSupermodeData()) {
        mySetComboBox->appendItem("Data Element");
        // hide Modul (because there is only an element)
        hide();
    }
    // update rest of elements
    onCmdSelectElementSet(0, 0, 0);
}


long
GNEElementSet::onCmdSelectElementSet(FXObject*, FXSelector, void*) {
    // check depending of current supermode
    if (mySelectorFrameParent->myViewNet->getEditModes().isCurrentSupermodeNetwork()) {
        // enable moduls
        mySelectorFrameParent->myMatchAttribute->showMatchAttribute();
        mySelectorFrameParent->myMatchGenericDataAttribute->hideMatchGenericDataAttribute();
        if (mySetComboBox->getText() == "network element") {
            myCurrentGNEElementSet = Type::NETWORKELEMENT;
            mySetComboBox->setTextColor(FXRGB(0, 0, 0));
            // enable match attribute
            mySelectorFrameParent->myMatchAttribute->enableMatchAttribute();
        } else if (mySetComboBox->getText() == "Additional") {
            myCurrentGNEElementSet = Type::ADDITIONALELEMENT;
            mySetComboBox->setTextColor(FXRGB(0, 0, 0));
            // enable match attribute
            mySelectorFrameParent->myMatchAttribute->enableMatchAttribute();
        } else if (mySetComboBox->getText() == "TAZ") {
            myCurrentGNEElementSet = Type::TAZELEMENT;
            mySetComboBox->setTextColor(FXRGB(0, 0, 0));
            // enable match attribute
            mySelectorFrameParent->myMatchAttribute->enableMatchAttribute();
        } else if (mySetComboBox->getText() == "Shape") {
            myCurrentGNEElementSet = Type::SHAPE;
            mySetComboBox->setTextColor(FXRGB(0, 0, 0));
            // enable match attribute
            mySelectorFrameParent->myMatchAttribute->enableMatchAttribute();
        } else {
            myCurrentGNEElementSet = Type::INVALID;
            mySetComboBox->setTextColor(FXRGB(255, 0, 0));
            // disable match attribute
            mySelectorFrameParent->myMatchAttribute->disableMatchAttribute();
        }
    } else if (mySelectorFrameParent->myViewNet->getEditModes().isCurrentSupermodeDemand()) {
        // enable moduls
        mySelectorFrameParent->myMatchAttribute->showMatchAttribute();
        mySelectorFrameParent->myMatchGenericDataAttribute->hideMatchGenericDataAttribute();
        if (mySetComboBox->getText() == "Demand Element") {
            myCurrentGNEElementSet = Type::DEMANDELEMENT;
            mySetComboBox->setTextColor(FXRGB(0, 0, 0));
            // enable match attribute
            mySelectorFrameParent->myMatchAttribute->enableMatchAttribute();
        } else {
            myCurrentGNEElementSet = Type::INVALID;
            mySetComboBox->setTextColor(FXRGB(255, 0, 0));
            // disable match attribute
            mySelectorFrameParent->myMatchAttribute->disableMatchAttribute();
        }
    } else if (mySelectorFrameParent->myViewNet->getEditModes().isCurrentSupermodeData()) {
        // enable moduls
        mySelectorFrameParent->myMatchAttribute->hideMatchAttribute();
        mySelectorFrameParent->myMatchGenericDataAttribute->showMatchGenericDataAttribute();
        if (mySetComboBox->getText() == "Data Element") {
            myCurrentGNEElementSet = Type::DATA;
            mySetComboBox->setTextColor(FXRGB(0, 0, 0));
            // enable match attribute
            mySelectorFrameParent->myMatchGenericDataAttribute->enableMatchGenericDataAttribute();
        } else {
            myCurrentGNEElementSet = Type::INVALID;
            mySetComboBox->setTextColor(FXRGB(255, 0, 0));
            // disable match attribute
            mySelectorFrameParent->myMatchGenericDataAttribute->enableMatchGenericDataAttribute();
        }
    }
    return 1;
}

/****************************************************************************/
