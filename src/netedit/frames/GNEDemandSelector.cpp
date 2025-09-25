/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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

#include <netedit/frames/common/GNEInspectorFrame.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagPropertiesDatabase.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
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

GNEDemandElementSelector::GNEDemandElementSelector(GNEFrame* frameParent, SumoXMLTag demandElementTag, const GNETagProperties::Type tagType) :
    MFXGroupBoxModule(frameParent, TLF("Parent %", toString(demandElementTag)).c_str()),
    myFrameParent(frameParent),
    myCurrentDemandElement(nullptr),
    myDemandElementTags({demandElementTag}),
                    myTagType(tagType),
mySelectingMultipleElements(false) {
    // Create MFXComboBoxIcon
    myDemandElementsComboBox = new MFXComboBoxIcon(getCollapsableFrame(), frameParent->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu(),
            true, GUIDesignComboBoxVisibleItems, this, MID_GNE_SET_TYPE, GUIDesignComboBox);
    // refresh demand element MatchBox
    refreshDemandElementSelector();
    // shown after creation
    show();
}


GNEDemandElementSelector::GNEDemandElementSelector(GNEFrame* frameParent, const std::vector<GNETagProperties::Type> tagTypes,
        const std::vector<SumoXMLTag> exceptions) :
    MFXGroupBoxModule(frameParent, TL("Parent element")),
    myFrameParent(frameParent),
    myCurrentDemandElement(nullptr),
    myTagType(GNETagProperties::Type::OTHER),
    mySelectingMultipleElements(false) {
    // fill myDemandElementTags
    for (const auto& tagType : tagTypes) {
        const auto tagPropertiesByType = frameParent->getViewNet()->getNet()->getTagPropertiesDatabase()->getTagPropertiesByType(tagType);
        for (const auto tagProperty : tagPropertiesByType) {
            if (std::find(exceptions.begin(), exceptions.end(), tagProperty->getTag()) == exceptions.end()) {
                myDemandElementTags.push_back(tagProperty->getTag());
            }
        }
    }
    // Create MFXComboBoxIcon
    myDemandElementsComboBox = new MFXComboBoxIcon(getCollapsableFrame(), frameParent->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu(),
            true, GUIDesignComboBoxVisibleItems, this, MID_GNE_SET_TYPE, GUIDesignComboBox);
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
        if (std::find(myDemandElementTags.begin(), myDemandElementTags.end(), demandElement->getTagProperty()->getTag()) != myDemandElementTags.end()) {
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
            // add default types in the first positions depending of frame parent
            if (myTagType & GNETagProperties::Type::PERSON) {
                // first pedestrian
                myDemandElementsComboBox->appendIconItem(DEFAULT_PEDTYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE_PEDESTRIAN), FXRGBA(253, 255, 206, 255));
                myDemandElementsComboBox->appendIconItem(DEFAULT_VTYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE_DEFAULT), FXRGBA(253, 255, 206, 255));
                myDemandElementsComboBox->appendIconItem(DEFAULT_BIKETYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE_BIKE), FXRGBA(253, 255, 206, 255));
                myDemandElementsComboBox->appendIconItem(DEFAULT_TAXITYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE_TAXI), FXRGBA(253, 255, 206, 255));
                myDemandElementsComboBox->appendIconItem(DEFAULT_RAILTYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE_RAIL), FXRGBA(253, 255, 206, 255));
                myDemandElementsComboBox->appendIconItem(DEFAULT_CONTAINERTYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE_CONTAINER), FXRGBA(253, 255, 206, 255));
            } else if (myTagType & GNETagProperties::Type::CONTAINER) {
                // first container
                myDemandElementsComboBox->appendIconItem(DEFAULT_CONTAINERTYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE_CONTAINER), FXRGBA(253, 255, 206, 255));
                myDemandElementsComboBox->appendIconItem(DEFAULT_VTYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE_DEFAULT), FXRGBA(253, 255, 206, 255));
                myDemandElementsComboBox->appendIconItem(DEFAULT_BIKETYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE_BIKE), FXRGBA(253, 255, 206, 255));
                myDemandElementsComboBox->appendIconItem(DEFAULT_TAXITYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE_RAIL), FXRGBA(253, 255, 206, 255));
                myDemandElementsComboBox->appendIconItem(DEFAULT_RAILTYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE_RAIL), FXRGBA(253, 255, 206, 255));
                myDemandElementsComboBox->appendIconItem(DEFAULT_PEDTYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE_PEDESTRIAN), FXRGBA(253, 255, 206, 255));
            } else {
                // first default vType
                myDemandElementsComboBox->appendIconItem(DEFAULT_VTYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE_DEFAULT), FXRGBA(253, 255, 206, 255));
                myDemandElementsComboBox->appendIconItem(DEFAULT_BIKETYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE_BIKE), FXRGBA(253, 255, 206, 255));
                myDemandElementsComboBox->appendIconItem(DEFAULT_TAXITYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE_TAXI), FXRGBA(253, 255, 206, 255));
                myDemandElementsComboBox->appendIconItem(DEFAULT_RAILTYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE_RAIL), FXRGBA(253, 255, 206, 255));
                myDemandElementsComboBox->appendIconItem(DEFAULT_PEDTYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE_PEDESTRIAN), FXRGBA(253, 255, 206, 255));
                myDemandElementsComboBox->appendIconItem(DEFAULT_CONTAINERTYPE_ID.c_str(), GUIIconSubSys::getIcon(GUIIcon::VTYPE_CONTAINER), FXRGBA(253, 255, 206, 255));
            }
            // add rest of vTypes
            for (const auto& vType : ACs->getDemandElements().at(demandElementTag)) {
                // avoid insert duplicated default vType
                if (DEFAULT_VTYPES.count(vType.second->getID()) == 0) {
                    myDemandElementsComboBox->appendIconItem(vType.second->getID().c_str(), vType.second->getACIcon());
                }
            }
        } else {
            // insert all elements sorted by ID
            std::map<std::string, GNEDemandElement*> sortedElements;
            for (const auto& demandElement : ACs->getDemandElements().at(demandElementTag)) {
                sortedElements[demandElement.second->getID()] = demandElement.second;
            }
            for (const auto& demandElement : sortedElements) {
                myDemandElementsComboBox->appendIconItem(demandElement.first.c_str(), demandElement.second->getACIcon(),
                        demandElement.second->getTagProperty()->getBackGroundColor());
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
                    myCurrentDemandElement = ACs->getDemandElements().at(*i).begin()->second;
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
    if (!myCurrentDemandElement->getTagProperty()->isPerson() &&
            !myCurrentDemandElement->getTagProperty()->isContainer()) {
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
            if (demandElement.second->getID() == myDemandElementsComboBox->getText().text()) {
                // set color of myTypeMatchBox to black (valid)
                myDemandElementsComboBox->setTextColor(GUIDesignTextColorBlack);
                myDemandElementsComboBox->killFocus();
                // Set new current demand element
                myCurrentDemandElement = demandElement.second;
                // call demandElementSelected function
                myFrameParent->demandElementSelected();
                return 1;
            }
        }
    }
    // if demand element selected is invalid, set demand element as null
    myCurrentDemandElement = nullptr;
    // call demandElementSelected function
    myFrameParent->demandElementSelected();
    // change color of myDemandElementsComboBox to red (invalid)
    myDemandElementsComboBox->setTextColor(GUIDesignTextColorRed);
    return 1;
}

/****************************************************************************/
