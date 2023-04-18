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
/// @file    GNENetworkSelector.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2022
///
// NetworkElement selector module
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNENetworkSelector.h"
#include "GNEFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNENetworkSelector) SelectorParentNetworkElementsMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_USESELECTED,        GNENetworkSelector::onCmdUseSelectedElements),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CLEARSELECTION,     GNENetworkSelector::onCmdClearSelection),
};

// Object implementation
FXIMPLEMENT(GNENetworkSelector, MFXGroupBoxModule, SelectorParentNetworkElementsMap, ARRAYNUMBER(SelectorParentNetworkElementsMap))


// ---------------------------------------------------------------------------
// GNENetworkSelector - methods
// ---------------------------------------------------------------------------

GNENetworkSelector::GNENetworkSelector(GNEFrame* frameParent, const Type networkElementType) :
    MFXGroupBoxModule(frameParent, TL("NetworkElements")),
    myFrameParent(frameParent),
    myNetworkElementType(networkElementType) {
    // Create horizontal frame
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame);
    // Create buttons
    myClearSelection = new FXButton(buttonsFrame, TL("Clear"), nullptr, this, MID_GNE_CLEARSELECTION, GUIDesignButtonRectangular100);
    myUseSelected = new FXButton(buttonsFrame, TL("Use selected"), nullptr, this, MID_GNE_USESELECTED, GUIDesignButtonRectangular100);
    // Create list
    myList = new FXList(getCollapsableFrame(), this, MID_GNE_SELECT, GUIDesignListFixedHeight, 0, 0, 0, 100);
    // create information label and update modul name
    switch (myNetworkElementType) {
        case Type::EDGE:
            new FXLabel(this,
                        (TL("-Click over an edge to select") + std::string("\n") + std::string("-ESC to clear selection")).c_str(),
                        0, GUIDesignLabelFrameInformation);
            setText(TL("Edges"));
            break;
        case Type::LANE:
            new FXLabel(this,
                        (TL("-Click over an lane to select") + std::string("\n") + std::string("-ESC to clear selection")).c_str(),
                        0, GUIDesignLabelFrameInformation);
            setText(TL("Lanes"));
            break;
        default:
            throw ProcessError(TL("Invalid NetworkElementType"));
    }
    // Hide List
    hide();
}


GNENetworkSelector::~GNENetworkSelector() {}


std::vector<std::string>
GNENetworkSelector::getSelectedIDs() const {
    // declare solution
    std::vector<std::string> solution;
    // reserve
    solution.reserve(myList->getNumItems());
    // fill IDs
    for (int i = 0; i < myList->getNumItems(); i++) {
        solution.push_back(myList->getItem(i)->getText().text());
    }
    return solution;
}


bool
GNENetworkSelector::isNetworkElementSelected(const GNENetworkElement* networkElement) const {
    if (myFrameParent->shown() && shown()) {
        // check if id is selected
        for (int i = 0; i < myList->getNumItems(); i++) {
            if (myList->getItem(i)->getText().text() == networkElement->getID()) {
                return true;
            }
        }
    }
    return false;
}


void
GNENetworkSelector::showNetworkElementsSelector() {
    // clear list of egdge ids
    myList->clearItems();
    // Show dialog
    show();
}


void
GNENetworkSelector::hideNetworkElementsSelector() {
    hide();
}


bool
GNENetworkSelector::isShown() const {
    return shown();
}


bool
GNENetworkSelector::toggleSelectedElement(const GNENetworkElement* networkElement) {
    // Obtain Id's of list
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->getItem(i)->getText().text() == networkElement->getID()) {
            // unselect element
            myList->removeItem(i);
            // update viewNet
            myFrameParent->getViewNet()->update();
            return true;
        }
    }
    // select element
    myList->appendItem(networkElement->getID().c_str(), networkElement->getACIcon());
    // update viewNet
    myFrameParent->getViewNet()->update();
    return true;
}


void
GNENetworkSelector::clearSelection() {
    // clear list of egdge ids
    myList->clearItems();
    // update viewNet
    myFrameParent->getViewNet()->update();
}


long
GNENetworkSelector::onCmdUseSelectedElements(FXObject*, FXSelector, void*) {
    // clear list of egdge ids
    myList->clearItems();
    // set modul name
    switch (myNetworkElementType) {
        case Type::EDGE:
            for (const auto& edge : myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getEdges()) {
                if (edge.second->isAttributeCarrierSelected()) {
                    myList->appendItem(edge.first.c_str(), edge.second->getACIcon());
                }
            }
            break;
        case Type::LANE:
            for (const auto& lane : myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getLanes()) {
                if (lane->isAttributeCarrierSelected()) {
                    myList->appendItem(lane->getID().c_str(), lane->getACIcon());
                }
            }
            break;
        default:
            throw ProcessError(TL("Invalid NetworkElementType"));
    }
    // Update Frame
    update();
    return 1;
}


long
GNENetworkSelector::onCmdClearSelection(FXObject*, FXSelector, void*) {
    clearSelection();
    return 1;
}


GNENetworkSelector::GNENetworkSelector() :
    myFrameParent(nullptr),
    myNetworkElementType(Type::EDGE) {
}

/****************************************************************************/
