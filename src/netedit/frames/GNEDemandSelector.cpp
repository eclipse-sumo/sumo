/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
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

FXDEFMAP(GNEDemandElementSelector) DemandElementSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_TYPE,    GNEDemandElementSelector::onCmdSelectDemandElement),
};

// Object implementation
FXIMPLEMENT(GNEDemandElementSelector,      MFXGroupBoxModule,     DemandElementSelectorMap,       ARRAYNUMBER(DemandElementSelectorMap))


// ===========================================================================
// method definitions
// ===========================================================================

GNEDemandElementSelector::GNEDemandElementSelector(GNEFrame* frameParent, SumoXMLTag demandElementTag, int tagType) :
    MFXGroupBoxModule(frameParent, (TL("Parent ") + toString(demandElementTag)).c_str()),
    myFrameParent(frameParent),
    myCurrentDemandElement(nullptr),
    myDemandElementTags({demandElementTag}),
    myTagType(tagType),
    mySelectingMultipleElements(false) {
    // Create MFXComboBoxIcon
    myDemandElementsComboBox = new MFXComboBoxIcon(getCollapsableFrame(), GUIDesignComboBoxNCol, true, GUIDesignComboBoxVisibleItemsMedium,
                                                   this, MID_GNE_SET_TYPE, GUIDesignComboBox);
    // refresh demand element MatchBox
    refreshDemandElementSelector();
    // shown after creation
    show();
}


GNEDemandElementSelector::GNEDemandElementSelector(GNEFrame* frameParent, const std::vector<GNETagProperties::TagType>& tagTypes) :
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
    myDemandElementsComboBox = new MFXComboBoxIcon(getCollapsableFrame(), GUIDesignComboBoxNCol, true, GUIDesignComboBoxVisibleItemsMedium,
                                                   this, MID_GNE_SET_TYPE, GUIDesignComboBox);
    // refresh demand element MatchBox
    refreshDemandElementSelector();
    // shown after creation
    show();
}


GNEDemandElementSelector::~GNEDemandElementSelector() {}


GNEDemandElement*
GNEDemandElementSelector::getCurrentDemandElement() const {
    return myCurrentDemandElement;
}


const std::vector<SumoXMLTag>&
GNEDemandElementSelector::getAllowedTags() const {
    return myDemandElementTags;
}


void
GNEDemandElementSelector::setDemandElement(GNEDemandElement* demandElement) {
    mySelectingMultipleElements = false;
    // Set new current demand element
    myCurrentDemandElement = demandElement;
    if (demandElement != nullptr) {
        // check that demandElement tag correspond to a tag of myDemandElementTags
        if (std::find(myDemandElementTags.begin(), myDemandElementTags.end(), demandElement->getTagProperty().getTag()) != myDemandElementTags.end()) {
            // update text of myDemandElementsComboBox
            myDemandElementsComboBox->setCurrentItem(demandElement->getID().c_str());
        }
    }
    // call demandElementSelected function
    myFrameParent->demandElementSelected();
}


void
GNEDemandElementSelector::setDemandElements(const std::vector<GNEDemandElement*>& demandElements) {
    mySelectingMultipleElements = true;
    myCurrentDemandElement = nullptr;
    myDemandElementsComboBox->clearItems();
    for (const auto& demandElement : demandElements) {
        myDemandElementsComboBox->appendIconItem(demandElement->getID().c_str(), demandElement->getACIcon());
    }
}


void
GNEDemandElementSelector::showDemandElementSelector() {
    // first refresh modul
    refreshDemandElementSelector();
    // if current selected item isn't valid, set DEFAULT_VTYPE_ID or DEFAULT_PEDTYPE_ID
    if (myCurrentDemandElement) {
        myDemandElementsComboBox->setCurrentItem(myCurrentDemandElement->getID().c_str());
    } else if (myDemandElementTags.size() == 1) {
        if (myDemandElementTags.at(0) == SUMO_TAG_VTYPE) {
            const auto defaultVType = myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, DEFAULT_VTYPE_ID);
            myDemandElementsComboBox->setCurrentItem(defaultVType->getID().c_str());
        }
    }
    onCmdSelectDemandElement(nullptr, 0, nullptr);
    show();
}


void
GNEDemandElementSelector::hideDemandElementSelector() {
    hide();
}


bool
GNEDemandElementSelector::isDemandElementSelectorShown() const {
    return shown();
}


void
GNEDemandElementSelector::refreshDemandElementSelector() {
    // get demand elemenst container
    const auto& ACs = myFrameParent->getViewNet()->getNet()->getAttributeCarriers();
    // clear demand elements comboBox
    myDemandElementsComboBox->clearItems();
    // fill myTypeMatchBox with list of demand elements
    for (const auto& demandElementTag : myDemandElementTags) {
        // special case for VTypes
        if (demandElementTag == SUMO_TAG_VTYPE) {
            // get view parent
            auto viewParent = myFrameParent->getViewNet()->getViewParent();
            // add default types in the first positions depending of frame parent
            if (myTagType & GNETagProperties::TagType::PERSON) {
                // first pedestrian
                myDemandElementsComboBox->appendIconItem(DEFAULT_PEDTYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE_PEDESTRIAN));
                myDemandElementsComboBox->appendIconItem(DEFAULT_VTYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE));
                myDemandElementsComboBox->appendIconItem(DEFAULT_BIKETYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE));
                myDemandElementsComboBox->appendIconItem(DEFAULT_TAXITYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE));
                myDemandElementsComboBox->appendIconItem(DEFAULT_RAILTYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE));
                myDemandElementsComboBox->appendIconItem(DEFAULT_CONTAINERTYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE_CONTAINER));
            } else if (myTagType & GNETagProperties::TagType::CONTAINER) {
                // first container
                myDemandElementsComboBox->appendIconItem(DEFAULT_CONTAINERTYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE_CONTAINER));
                myDemandElementsComboBox->appendIconItem(DEFAULT_VTYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE));
                myDemandElementsComboBox->appendIconItem(DEFAULT_BIKETYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE));
                myDemandElementsComboBox->appendIconItem(DEFAULT_TAXITYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE));
                myDemandElementsComboBox->appendIconItem(DEFAULT_RAILTYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE));
                myDemandElementsComboBox->appendIconItem(DEFAULT_PEDTYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE_PEDESTRIAN));
            } else {
                // first default vType
                myDemandElementsComboBox->appendIconItem(DEFAULT_VTYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE));
                myDemandElementsComboBox->appendIconItem(DEFAULT_BIKETYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE));
                myDemandElementsComboBox->appendIconItem(DEFAULT_TAXITYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE));
                myDemandElementsComboBox->appendIconItem(DEFAULT_RAILTYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE));
                myDemandElementsComboBox->appendIconItem(DEFAULT_PEDTYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE_PEDESTRIAN));
                myDemandElementsComboBox->appendIconItem(DEFAULT_CONTAINERTYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE_CONTAINER));
            }
            // add rest of vTypes
            for (const auto& vType : ACs->getDemandElements().at(demandElementTag)) {
                // avoid insert duplicated default vType
                if (DEFAULT_VTYPES.count(vType->getID()) == 0) {
                    myDemandElementsComboBox->appendIconItem(vType->getID().c_str(), vType->getACIcon());
                }
            }
        } else {
            // insert all Ids
            for (const auto& demandElement : ACs->getDemandElements().at(demandElementTag)) {
                myDemandElementsComboBox->appendIconItem(demandElement->getID().c_str(), demandElement->getACIcon());
            }
        }
    }
    // update myCurrentDemandElement
    if (myDemandElementsComboBox->getNumItems() == 0) {
        myCurrentDemandElement = nullptr;
    } else if (myCurrentDemandElement) {
        for (int i = 0; i < myDemandElementsComboBox->getNumItems(); i++) {
            if (myDemandElementsComboBox->getItemText(i) == myCurrentDemandElement->getID()) {
                myDemandElementsComboBox->setCurrentItem(i, FALSE);
            }
        }
    } else {
        for (auto i = myDemandElementTags.begin(); (i != myDemandElementTags.end()) && (myCurrentDemandElement == nullptr); i++) {
            myCurrentDemandElement = ACs->retrieveDemandElement(*i, myDemandElementsComboBox->getItemText(0), false);
        }
        if (myCurrentDemandElement == nullptr) {
            // update myCurrentDemandElement with the first allowed element
            for (auto i = myDemandElementTags.begin(); (i != myDemandElementTags.end()) && (myCurrentDemandElement == nullptr); i++) {
                if (ACs->getDemandElements().at(*i).size() > 0) {
                    myCurrentDemandElement = *ACs->getDemandElements().at(*i).begin();
                }
            }
        }
    }
}


GNEDemandElement*
GNEDemandElementSelector::getPreviousPlanElement() const {
    if (myCurrentDemandElement == nullptr) {
        return nullptr;
    }
    if (!myCurrentDemandElement->getTagProperty().isPerson() &&
        !myCurrentDemandElement->getTagProperty().isContainer()) {
        return nullptr;
    }
    if (myCurrentDemandElement->getChildDemandElements().empty()) {
        return nullptr;
    }
    return myCurrentDemandElement->getChildDemandElements().back();
}


long
GNEDemandElementSelector::onCmdSelectDemandElement(FXObject*, FXSelector, void*) {
    // Check if value of myTypeMatchBox correspond to a demand element
    for (const auto& demandElementTag : myDemandElementTags) {
        for (const auto& demandElement : myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getDemandElements().at(demandElementTag)) {
            if (demandElement->getID() == myDemandElementsComboBox->getText().text()) {
                // set color of myTypeMatchBox to black (valid)
                myDemandElementsComboBox->setTextColor(FXRGB(0, 0, 0));
                myDemandElementsComboBox->killFocus();
                // Set new current demand element
                myCurrentDemandElement = demandElement;
                // call demandElementSelected function
                myFrameParent->demandElementSelected();
                // Write Warning in console if we're in testing mode
                WRITE_DEBUG((TL("Selected item '") + myDemandElementsComboBox->getText() + TL("' in DemandElementSelector")).text());
                return 1;
            }
        }
    }
    // if demand element selected is invalid, set demand element as null
    myCurrentDemandElement = nullptr;
    // call demandElementSelected function
    myFrameParent->demandElementSelected();
    // change color of myDemandElementsComboBox to red (invalid)
    myDemandElementsComboBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG(TL("Selected invalid item in DemandElementSelector"));
    return 1;
}

/****************************************************************************/
