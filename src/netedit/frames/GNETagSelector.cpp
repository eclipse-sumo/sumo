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
/// @file    GNETagSelector.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2022
///
// Frame for select tags
/****************************************************************************/

#include <netedit/GNENet.h>
#include <netedit/GNETagPropertiesDatabase.h>
#include <netedit/GNEViewNet.h>
#include <netedit/frames/common/GNEInspectorFrame.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNETagSelector.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNETagSelector) TagSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_TAG_SELECTED,        GNETagSelector::onCmdSelectTag)
};

// Object implementation
FXIMPLEMENT(GNETagSelector, MFXGroupBoxModule,  TagSelectorMap, ARRAYNUMBER(TagSelectorMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNETagSelector::GNETagSelector(GNEFrame* frameParent, const GNETagProperties::TagType type, const SumoXMLTag tag) :
    MFXGroupBoxModule(frameParent, TL("Element")),
    myFrameParent(frameParent),
    myCurrentTemplateAC(nullptr) {
    // Create MFXComboBoxIcon
    myTagsMatchBox = new MFXComboBoxIcon(getCollapsableFrame(), GUIDesignComboBoxNCol, true, GUIDesignComboBoxVisibleItems,
                                         this, MID_GNE_TAG_SELECTED, GUIDesignComboBox);
    // update tag types without informing parent (because we're in the creator
    updateTagTypes(type, tag, false);
    // GNETagSelector is always shown
    show();
}


GNETagSelector::~GNETagSelector() {}


void
GNETagSelector::showTagSelector() {
    show();
}


void
GNETagSelector::hideTagSelector() {
    hide();
}


GNEAttributeCarrier*
GNETagSelector::getCurrentTemplateAC() const {
    return myCurrentTemplateAC;
}


void
GNETagSelector::updateTagTypes(const GNETagProperties::TagType type, const SumoXMLTag tag, const bool informParent) {
    // check if net has proj
    const bool proj = (GeoConvHelper::getFinal().getProjString() != "!");
    // change GNETagSelector text
    switch (type) {
        case GNETagProperties::TagType::NETWORKELEMENT:
            setText(TL("network elements"));
            break;
        case GNETagProperties::TagType::ADDITIONALELEMENT:
            setText(TL("Additional elements"));
            break;
        case GNETagProperties::TagType::SHAPE:
            setText(TL("Shape elements"));
            break;
        case GNETagProperties::TagType::TAZELEMENT:
            setText(TL("TAZ elements"));
            break;
        case GNETagProperties::TagType::WIRE:
            setText(TL("Wire elements"));
            break;
        case GNETagProperties::TagType::VEHICLE:
            setText(TL("Vehicles"));
            break;
        case GNETagProperties::TagType::STOP_VEHICLE:
            setText(TL("Stops"));
            break;
        case GNETagProperties::TagType::PERSON:
            setText(TL("Persons"));
            break;
        case GNETagProperties::TagType::PERSONPLAN:
            setText(TL("Person plans"));
            break;
        case GNETagProperties::TagType::CONTAINER:
            setText(TL("Container"));
            break;
        case GNETagProperties::TagType::CONTAINERPLAN:
            setText(TL("Container plans"));
            break;
        case GNETagProperties::TagType::PERSONTRIP:
            setText(TL("Person trips"));
            break;
        case GNETagProperties::TagType::WALK:
            setText(TL("Walks"));
            break;
        case GNETagProperties::TagType::RIDE:
            setText(TL("Rides"));
            break;
        case GNETagProperties::TagType::STOP_PERSON:
            setText(TL("Person stops"));
            break;
        default:
            throw ProcessError("invalid tag property");
    }
    myTagsMatchBox->clearItems();
    // get tag properties
    const auto tagPropertiesByType = myFrameParent->getViewNet()->getNet()->getTagPropertiesDatabase()->getTagPropertiesByType(type);
    // fill myACTemplates and myTagsMatchBox
    for (const auto tagProperty : tagPropertiesByType) {
        if (!tagProperty->requireProj() || proj) {
            myTagsMatchBox->appendIconItem(tagProperty->getSelectorText().c_str(), GUIIconSubSys::getIcon(tagProperty->getGUIIcon()), tagProperty->getBackGroundColor());
        }
    }
    if (myTagsMatchBox->getNumItems() > 0) {
        myTagsMatchBox->enable();
        setCurrentTag(tag, informParent);
    } else {
        myTagsMatchBox->disable();
    }
}


void
GNETagSelector::setCurrentTag(SumoXMLTag newTag, const bool informParent) {
    // first reset myCurrentTemplateAC
    myCurrentTemplateAC = myFrameParent->getViewNet()->getNet()->getACTemplates()->getTemplateAC(newTag);
    // iterate over all myTagsMatchBox
    for (int i = 0; i < myTagsMatchBox->getNumItems(); i++) {
        if (myTagsMatchBox->getItemText(i) == myCurrentTemplateAC->getTagProperty()->getTagStr()) {
            myTagsMatchBox->setCurrentItem(i);
            // set color of myTypeMatchBox to black (valid)
            myTagsMatchBox->setTextColor(FXRGB(0, 0, 0));
            myTagsMatchBox->killFocus();
        }
    }
    // inform to frame parent that a tag was selected
    if (informParent) {
        myFrameParent->tagSelected();
    }
}


void
GNETagSelector::refreshTagSelector() {
    // call tag selected function
    myFrameParent->tagSelected();
}


long
GNETagSelector::onCmdSelectTag(FXObject*, FXSelector, void*) {
    myCurrentTemplateAC = myFrameParent->getViewNet()->getNet()->getACTemplates()->getTemplateAC(myTagsMatchBox->getText().text());
    if (myCurrentTemplateAC) {
        // set color of myTypeMatchBox to black (valid)
        myTagsMatchBox->setTextColor(FXRGB(0, 0, 0));
        myTagsMatchBox->killFocus();
    } else {
        // set color of myTypeMatchBox to red (invalid)
        myTagsMatchBox->setTextColor(FXRGB(255, 0, 0));
    }
    // inform to frame parent that a tag was selected
    myFrameParent->tagSelected();
    return 1;
}

/****************************************************************************/
