/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    GNEViewObjectSelector.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2022
///
// NetworkElement selector module
/****************************************************************************/

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNETagProperties.h>
#include <utils/common/MsgHandler.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEViewObjectSelector.h"
#include "GNEFrame.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEViewObjectSelector) SelectorParentNetworkElementsMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_USESELECTED,        GNEViewObjectSelector::onCmdUseSelectedElements),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CLEARSELECTION,     GNEViewObjectSelector::onCmdClearSelection),
};

// Object implementation
FXIMPLEMENT(GNEViewObjectSelector, GNEGroupBoxModule, SelectorParentNetworkElementsMap, ARRAYNUMBER(SelectorParentNetworkElementsMap))

// ---------------------------------------------------------------------------
// GNEViewObjectSelector - methods
// ---------------------------------------------------------------------------

GNEViewObjectSelector::GNEViewObjectSelector(GNEFrame* frameParent) :
    GNEGroupBoxModule(frameParent, TL("NetworkElements")),
    myFrameParent(frameParent) {
    // Create buttons
    myClearSelection = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Clear selection"), "", "", nullptr, this, MID_GNE_CLEARSELECTION, GUIDesignButton);
    myUseSelected = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Use selected"), "", "", nullptr, this, MID_GNE_USESELECTED, GUIDesignButton);
    // list label
    new FXLabel(getCollapsableFrame(), TL("Selected elements"), 0, GUIDesignLabelThick(JUSTIFY_NORMAL));
    // Create list
    myList = new FXList(getCollapsableFrame(), this, MID_GNE_SELECT, GUIDesignListFixedHeight);
    // create information label
    myLabel = new FXLabel(getCollapsableFrame(), "", 0, GUIDesignLabelFrameInformation);
    // Hide List
    hide();
}


GNEViewObjectSelector::~GNEViewObjectSelector() {}


SumoXMLTag
GNEViewObjectSelector::getTag() const {
    return myTag;
}


bool
GNEViewObjectSelector::isNetworkElementSelected(const GNEAttributeCarrier* AC) const {
    return std::find(mySelectedACs.begin(), mySelectedACs.end(), AC) != mySelectedACs.end();
}


void
GNEViewObjectSelector::showNetworkElementsSelector(const SumoXMLTag newTag, const SumoXMLAttr attribute) {
    myTag = newTag;
    myAttribute = attribute;
    // update info
    myLabel->setText((TL("-This additional requires to\n select at least\n one element") + std::string("\n") +
                      TLF("-Click over % to select", toString(newTag)) + std::string("\n") +
                      TL("-ESC to clear elements")).c_str());
    myUseSelected->setText(TLF("Use selected %s", toString(newTag)).c_str());
    // update groupBox elements
    setText(TLF("% selector", toString(newTag)));
    // clear items
    myList->clearItems();
    mySelectedACs.clear();
    show();
}


void
GNEViewObjectSelector::hideNetworkElementsSelector() {
    hide();
}


bool
GNEViewObjectSelector::toggleSelectedElement(const GNEAttributeCarrier* AC) {
    if (shown() && AC) {
        // Obtain Id's of list
        for (int i = 0; i < myList->getNumItems(); i++) {
            if (myList->getItem(i)->getText().text() == AC->getID()) {
                // unselect element
                myList->removeItem(i);
                // remove it in list
                mySelectedACs.erase(mySelectedACs.begin() + i);
                // update viewNet
                myFrameParent->getViewNet()->update();
                return true;
            }
        }
        // select element
        myList->appendItem(AC->getID().c_str(), AC->getACIcon());
        mySelectedACs.push_back(AC);
        // update viewNet
        myFrameParent->getViewNet()->update();
        return true;
    } else {
        // nothing to toogle
        return false;
    }
}


bool
GNEViewObjectSelector::toggleSelectedLane(const GNELane* lane) {
    if (shown() && lane) {
        if (myTag == SUMO_TAG_EDGE) {
            return toggleSelectedElement(lane->getParentEdge());
        } else {
            return toggleSelectedElement(lane);
        }
    } else {
        // nothing to toogle
        return false;
    }
}


bool
GNEViewObjectSelector::fillSumoBaseObject(CommonXMLStructure::SumoBaseObject* baseObject) const {
    if (shown()) {
        if (myList->getNumItems() == 0) {
            WRITE_WARNING(TLF("List of % cannot be empty", toString(myTag)));
            return false;
        } else {
            std::vector<std::string> selectedIDs;
            selectedIDs.reserve(mySelectedACs.size());
            // Obtain Id's of list
            for (const auto AC : mySelectedACs) {
                selectedIDs.push_back(AC->getID());
            }
            baseObject->addStringListAttribute(myAttribute, selectedIDs);
            return true;
        }
    } else {
        // nothing to fill
        return true;
    }
}


void
GNEViewObjectSelector::clearSelection() {
    // clear list of egdge ids
    myList->clearItems();
    mySelectedACs.clear();
    // update viewNet
    myFrameParent->getViewNet()->update();
}


long
GNEViewObjectSelector::onCmdUseSelectedElements(FXObject*, FXSelector, void*) {
    // clear list of egdge ids
    myList->clearItems();
    mySelectedACs.clear();
    // get all selected ACs
    const auto selectedACs = myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getSelectedAttributeCarriers(false);
    for (const auto AC : selectedACs) {
        if (AC->getTagProperty()->getTag() == myTag) {
            if (AC->isAttributeCarrierSelected()) {
                myList->appendItem(AC->getID().c_str(), AC->getACIcon());
                mySelectedACs.push_back(AC);
            }
        }
    }
    // Update Frame
    update();
    return 1;
}


long
GNEViewObjectSelector::onCmdClearSelection(FXObject*, FXSelector, void*) {
    clearSelection();
    return 1;
}


GNEViewObjectSelector::GNEViewObjectSelector() :
    myFrameParent(nullptr) {
}

/****************************************************************************/
