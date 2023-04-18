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
/// @file    GNEDemandSelector.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2022
///
// Frame for select demand elements
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/frames/common/GNEInspectorFrame.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEDemandSelector.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(DemandElementSelector) DemandElementSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_TYPE,    DemandElementSelector::onCmdSelectDemandElement),
};

// Object implementation
FXIMPLEMENT(DemandElementSelector,      MFXGroupBoxModule,     DemandElementSelectorMap,       ARRAYNUMBER(DemandElementSelectorMap))


// ===========================================================================
// method definitions
// ===========================================================================

DemandElementSelector::DemandElementSelector(GNEFrame* frameParent, SumoXMLTag demandElementTag, GNEDemandElement* defaultElement) :
    MFXGroupBoxModule(frameParent, (TL("Parent ") + toString(demandElementTag)).c_str()),
    myFrameParent(frameParent),
    myCurrentDemandElement(defaultElement),
    myDemandElementTags({demandElementTag}),
mySelectingMultipleElements(false) {
    // Create MFXComboBoxIcon
    myDemandElementsMatchBox = new MFXComboBoxIcon(getCollapsableFrame(), GUIDesignComboBoxNCol, true, this, MID_GNE_SET_TYPE, GUIDesignComboBox);
    // create info label
    myInfoLabel = new FXLabel(getCollapsableFrame(), "", nullptr, GUIDesignLabelFrameInformation);
    // refresh demand element MatchBox
    refreshDemandElementSelector();
    // shown after creation
    show();
}


DemandElementSelector::DemandElementSelector(GNEFrame* frameParent, const std::vector<GNETagProperties::TagType>& tagTypes) :
    MFXGroupBoxModule(frameParent, TL("Parent element")),
    myFrameParent(frameParent),
    myCurrentDemandElement(nullptr),
    mySelectingMultipleElements(false) {
    // fill myDemandElementTags
    for (const auto& tagType : tagTypes) {
        const auto tagProperties = GNEAttributeCarrier::getTagPropertiesByType(tagType);
        for (const auto& tagProperty : tagProperties) {
            myDemandElementTags.push_back(tagProperty.getTag());
        }
    }
    // Create MFXComboBoxIcon
    myDemandElementsMatchBox = new MFXComboBoxIcon(getCollapsableFrame(), GUIDesignComboBoxNCol, true, this, MID_GNE_SET_TYPE, GUIDesignComboBox);
    // create info label
    myInfoLabel = new FXLabel(getCollapsableFrame(), "", nullptr, GUIDesignLabelFrameInformation);
    // refresh demand element MatchBox
    refreshDemandElementSelector();
    // shown after creation
    show();
}


DemandElementSelector::~DemandElementSelector() {}


GNEDemandElement*
DemandElementSelector::getCurrentDemandElement() const {
    return myCurrentDemandElement;
}


const std::vector<SumoXMLTag>&
DemandElementSelector::getAllowedTags() const {
    return myDemandElementTags;
}


void
DemandElementSelector::setDemandElement(GNEDemandElement* demandElement) {
    mySelectingMultipleElements = false;
    // Set new current demand element
    myCurrentDemandElement = demandElement;
    if (demandElement == nullptr) {
        myDemandElementsMatchBox->setCustomText(TL("select item..."));
        // set info label
        myInfoLabel->setText((TL("-Select an item in the list or") + std::string("\n") + TL("click over an element in view")).c_str());
        myInfoLabel->show();
    } else {
        // check that demandElement tag correspond to a tag of myDemandElementTags
        if (std::find(myDemandElementTags.begin(), myDemandElementTags.end(), demandElement->getTagProperty().getTag()) != myDemandElementTags.end()) {
            // update text of myDemandElementsMatchBox
            myDemandElementsMatchBox->setItem(demandElement->getID().c_str(), demandElement->getACIcon());
        }
        myInfoLabel->hide();
    }
    // call demandElementSelected function
    myFrameParent->demandElementSelected();
}


void
DemandElementSelector::setDemandElements(const std::vector<GNEDemandElement*>& demandElements) {
    mySelectingMultipleElements = true;
    myCurrentDemandElement = nullptr;
    myDemandElementsMatchBox->clearItems();
    for (const auto& demandElement : demandElements) {
        myDemandElementsMatchBox->appendIconItem(demandElement->getID().c_str(), demandElement->getACIcon());
    }
    myDemandElementsMatchBox->setCustomText(TL("select sub-item..."));
    // set info label
    const std::string info =
        TL("-Clicked over multiple") + std::string("\n") +
        TL("elements") + std::string("\n") +
        TL(" - Select an item in the") + std::string("\n") +
        TL(" list or click over an") + std::string("\n") +
        TL(" element in view");
    myInfoLabel->setText(info.c_str());
    myInfoLabel->show();
}


void
DemandElementSelector::showDemandElementSelector() {
    // first refresh modul
    refreshDemandElementSelector();
    // if current selected item isn't valid, set DEFAULT_VTYPE_ID or DEFAULT_PEDTYPE_ID
    if (myCurrentDemandElement) {
        myDemandElementsMatchBox->setItem(myCurrentDemandElement->getID().c_str(), myCurrentDemandElement->getACIcon());
    } else if (myDemandElementTags.size() == 1) {
        if (myDemandElementTags.at(0) == SUMO_TAG_VTYPE) {
            const auto defaultVType = myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, DEFAULT_VTYPE_ID);
            myDemandElementsMatchBox->setItem(defaultVType->getID().c_str(), defaultVType->getACIcon());
        }
    }
    onCmdSelectDemandElement(nullptr, 0, nullptr);
    show();
}


void
DemandElementSelector::hideDemandElementSelector() {
    hide();
}


bool
DemandElementSelector::isDemandElementSelectorShown() const {
    return shown();
}


void
DemandElementSelector::refreshDemandElementSelector() {
    // get demand elemenst container
    const auto& demandElements = myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getDemandElements();
    // clear demand elements comboBox
    myDemandElementsMatchBox->clearItems();
    // fill myTypeMatchBox with list of demand elements
    for (const auto& demandElementTag : myDemandElementTags) {
        // special case for VTypes
        if (demandElementTag == SUMO_TAG_VTYPE) {
            // add default  types in the first positions
            myDemandElementsMatchBox->appendIconItem(DEFAULT_VTYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE));
            myDemandElementsMatchBox->appendIconItem(DEFAULT_BIKETYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE));
            myDemandElementsMatchBox->appendIconItem(DEFAULT_TAXITYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE));
            myDemandElementsMatchBox->appendIconItem(DEFAULT_RAILTYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE));
            myDemandElementsMatchBox->appendIconItem(DEFAULT_PEDTYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE));
            myDemandElementsMatchBox->appendIconItem(DEFAULT_CONTAINERTYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE));
            // add rest of vTypes
            for (const auto& vType : demandElements.at(demandElementTag)) {
                // avoid insert duplicated default vType
                if (DEFAULT_VTYPES.count(vType->getID()) == 0) {
                    myDemandElementsMatchBox->appendIconItem(vType->getID().c_str(), vType->getACIcon());
                }
            }
        } else {
            // insert all Ids
            for (const auto& demandElement : demandElements.at(demandElementTag)) {
                myDemandElementsMatchBox->appendIconItem(demandElement->getID().c_str(), demandElement->getACIcon());
            }
        }
    }
    // Set number of  items (maximum 10)
    if (myDemandElementsMatchBox->getNumItems() < 10) {
        myDemandElementsMatchBox->setNumVisible((int)myDemandElementsMatchBox->getNumItems());
    } else {
        myDemandElementsMatchBox->setNumVisible(10);
    }
    // update myCurrentDemandElement
    if (myDemandElementsMatchBox->getNumItems() == 0) {
        myCurrentDemandElement = nullptr;
    } else if (myCurrentDemandElement) {
        for (int i = 0; i < myDemandElementsMatchBox->getNumItems(); i++) {
            if (myDemandElementsMatchBox->getItem(i).text() == myCurrentDemandElement->getID()) {
                myDemandElementsMatchBox->setCurrentItem(i, FALSE);
            }
        }
    } else {
        // set first element in the list as myCurrentDemandElement (Special case for default person and vehicle type)
        if (myDemandElementsMatchBox->getItem(0).text() == DEFAULT_VTYPE_ID) {
            myCurrentDemandElement = myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getDefaultType();
        } else {
            // disable myCurrentDemandElement
            myCurrentDemandElement = nullptr;
            // update myCurrentDemandElement with the first allowed element
            for (auto i = myDemandElementTags.begin(); (i != myDemandElementTags.end()) && (myCurrentDemandElement == nullptr); i++) {
                if (demandElements.at(*i).size() > 0) {
                    myCurrentDemandElement = *demandElements.at(*i).begin();
                }
            }
        }
    }
}


GNEEdge*
DemandElementSelector::getPersonPlanPreviousEdge() const {
    if (myCurrentDemandElement == nullptr) {
        return nullptr;
    }
    if (!myCurrentDemandElement->getTagProperty().isPerson()) {
        return nullptr;
    }
    if (myCurrentDemandElement->getChildDemandElements().empty()) {
        return nullptr;
    }
    // get last person plan
    const GNEDemandElement* lastPersonPlan = myCurrentDemandElement->getChildDemandElements().back();
    // check tag
    switch (lastPersonPlan->getTagProperty().getTag()) {
        // person trips
        case GNE_TAG_PERSONTRIP_EDGE:
        // rides
        case GNE_TAG_RIDE_EDGE:
        // walks
        case GNE_TAG_WALK_EDGE:
        case GNE_TAG_WALK_EDGES:
        // stops
        case GNE_TAG_STOPPERSON_EDGE:
            return lastPersonPlan->getParentEdges().back();
        // person trips
        case GNE_TAG_PERSONTRIP_BUSSTOP:
        // person trips
        case GNE_TAG_RIDE_BUSSTOP:
        // walks
        case GNE_TAG_WALK_BUSSTOP:
        // stops
        case GNE_TAG_STOPPERSON_BUSSTOP:
            return lastPersonPlan->getParentAdditionals().back()->getParentLanes().front()->getParentEdge();
        // route walks
        case GNE_TAG_WALK_ROUTE:
            return lastPersonPlan->getParentDemandElements().back()->getParentEdges().back();
        default:
            return nullptr;
    }
}


GNEEdge*
DemandElementSelector::getContainerPlanPreviousEdge() const {
    if (myCurrentDemandElement == nullptr) {
        return nullptr;
    }
    if (!myCurrentDemandElement->getTagProperty().isContainer()) {
        return nullptr;
    }
    if (myCurrentDemandElement->getChildDemandElements().empty()) {
        return nullptr;
    }
    // get last container plan
    const GNEDemandElement* lastContainerPlan = myCurrentDemandElement->getChildDemandElements().back();
    // check tag
    switch (lastContainerPlan->getTagProperty().getTag()) {
        // transport
        case GNE_TAG_TRANSPORT_EDGE:
        // tranship
        case GNE_TAG_TRANSHIP_EDGE:
        case GNE_TAG_TRANSHIP_EDGES:
        // stop
        case GNE_TAG_STOPCONTAINER_EDGE:
            return lastContainerPlan->getParentEdges().back();
        // transport
        case GNE_TAG_TRANSPORT_CONTAINERSTOP:
        // tranship
        case GNE_TAG_TRANSHIP_CONTAINERSTOP:
        // stop
        case GNE_TAG_STOPCONTAINER_CONTAINERSTOP:
            return lastContainerPlan->getParentAdditionals().back()->getParentLanes().front()->getParentEdge();
        default:
            return nullptr;
    }
}


long
DemandElementSelector::onCmdSelectDemandElement(FXObject*, FXSelector, void*) {
    // Check if value of myTypeMatchBox correspond to a demand element
    for (const auto& demandElementTag : myDemandElementTags) {
        for (const auto& demandElement : myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getDemandElements().at(demandElementTag)) {
            if (demandElement->getID() == myDemandElementsMatchBox->getText().text()) {
                // set color of myTypeMatchBox to black (valid)
                myDemandElementsMatchBox->setTextColor(FXRGB(0, 0, 0));
                myDemandElementsMatchBox->killFocus();
                // Set new current demand element
                myCurrentDemandElement = demandElement;
                // call demandElementSelected function
                myFrameParent->demandElementSelected();
                // Write Warning in console if we're in testing mode
                WRITE_DEBUG((TL("Selected item '") + myDemandElementsMatchBox->getText() + TL("' in DemandElementSelector")).text());
                myInfoLabel->hide();
                return 1;
            }
        }
    }
    // if demand element selected is invalid, set demand element as null
    myCurrentDemandElement = nullptr;
    // call demandElementSelected function
    myFrameParent->demandElementSelected();
    // change color of myDemandElementsMatchBox to red (invalid)
    myDemandElementsMatchBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG(TL("Selected invalid item in DemandElementSelector"));
    return 1;
}

/****************************************************************************/
