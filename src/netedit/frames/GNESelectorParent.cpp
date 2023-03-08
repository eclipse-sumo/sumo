/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GNESelectorParent.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2022
///
// Frame for select parents
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/frames/common/GNEInspectorFrame.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNESelectorParent.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNESelectorParent::GNESelectorParent(GNEFrame* frameParent) :
    MFXGroupBoxModule(frameParent, TL("Parent selector")),
    myFrameParent(frameParent) {
    // Create label with the type of GNESelectorParent
    myParentsLabel = new FXLabel(getCollapsableFrame(), TL("No element selected"), nullptr, GUIDesignLabelThick(JUSTIFY_NORMAL));
    // Create list
    myParentsList = new FXList(getCollapsableFrame(), this, MID_GNE_SET_TYPE, GUIDesignListSingleElementFixedHeight);
    // Hide List
    hideSelectorParentModule();
}


GNESelectorParent::~GNESelectorParent() {}


std::string
GNESelectorParent::getIdSelected() const {
    for (int i = 0; i < myParentsList->getNumItems(); i++) {
        if (myParentsList->isItemSelected(i)) {
            return myParentsList->getItem(i)->getText().text();
        }
    }
    return "";
}


void
GNESelectorParent::setIDSelected(const std::string& id) {
    // first unselect all
    for (int i = 0; i < myParentsList->getNumItems(); i++) {
        myParentsList->getItem(i)->setSelected(false);
    }
    // select element if correspond to given ID
    for (int i = 0; i < myParentsList->getNumItems(); i++) {
        if (myParentsList->getItem(i)->getText().text() == id) {
            myParentsList->getItem(i)->setSelected(true);
        }
    }
    // recalc myFirstParentsList
    myParentsList->recalc();
}


void
GNESelectorParent::showSelectorParentModule(const std::vector<SumoXMLTag>& parentTags) {
    if (parentTags.size() > 0) {
        myParentTags = parentTags;
        myParentsLabel->setText((TL("Parent type: ") + toString(parentTags.front())).c_str());
        refreshSelectorParentModule();
        show();
    } else {
        myParentTags.clear();
        hide();
    }
}


void
GNESelectorParent::hideSelectorParentModule() {
    myParentTags.clear();
    hide();
}


void
GNESelectorParent::refreshSelectorParentModule() {
    // save current edited elements
    std::set<std::string> selectedItems;
    for (int i = 0; i < myParentsList->getNumItems(); i++) {
        if (myParentsList->isItemSelected(i)) {
            selectedItems.insert(myParentsList->getItem(i)->getText().text());
        }
    }
    myParentsList->clearItems();
    if (myParentTags.size() > 0) {
        // insert additionals sorted
        std::set<std::string> IDs;
        // fill list with IDs
        for (const auto& parentTag : myParentTags) {
            // check type
            const auto tagProperty = GNEAttributeCarrier::getTagProperty(parentTag);
            // additionals
            if (tagProperty.isAdditionalElement()) {
                for (const auto& additional : myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getAdditionals().at(parentTag)) {
                    IDs.insert(additional->getID().c_str());
                }
            }
        }
        // fill list with IDs
        for (const auto& ID : IDs) {
            const int item = myParentsList->appendItem(ID.c_str());
            if (selectedItems.find(ID) != selectedItems.end()) {
                myParentsList->selectItem(item);
            }
        }
    }
}

/****************************************************************************/
