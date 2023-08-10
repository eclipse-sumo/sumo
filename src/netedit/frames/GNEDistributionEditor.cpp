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
/// @file    GNEDistributionEditor.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2023
///
// The Widget for edit type distribution elements
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/changes/GNEChange_DemandElement.h>
#include <netedit/elements/demand/GNEVTypeDistribution.h>
#include <netedit/dialogs/GNEVehicleTypeDialog.h>
#include <netedit/dialogs/GNEVTypeDistributionsDialog.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEDistributionEditor.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEDistributionEditor::AttributesEditorRow) AttributesEditorRowMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEDistributionEditor::AttributesEditorRow::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_REMOVE,  GNEDistributionEditor::AttributesEditorRow::onCmdRemoveRow)
};

FXDEFMAP(GNEDistributionEditor::AttributesEditor) AttributesEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_ADD,   GNEDistributionEditor::AttributesEditor::onCmdAddRow)
};

// Object implementation
FXIMPLEMENT(GNEDistributionEditor::AttributesEditorRow,  FXHorizontalFrame,  AttributesEditorRowMap, ARRAYNUMBER(AttributesEditorRowMap))
FXIMPLEMENT(GNEDistributionEditor::AttributesEditor,     MFXGroupBoxModule,  AttributesEditorMap,    ARRAYNUMBER(AttributesEditorMap))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEDistributionEditor::AttributesEditorRow - methods
// ---------------------------------------------------------------------------

GNEDistributionEditor::AttributesEditorRow::AttributesEditorRow(
        GNEDistributionEditor::AttributesEditor* attributeEditorParent,
        const GNEAttributeProperties& ACAttr, const std::string& id) :
    FXHorizontalFrame(attributeEditorParent->getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame),
    myAttributesEditorParent(attributeEditorParent) {
    // get staticTooltip menu
    auto staticTooltipMenu = attributeEditorParent->getFrameParent()->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu();
    // Create attribute label (usually used only for ID)
    myIDLabel = new MFXLabelTooltip(this, staticTooltipMenu,
        ACAttr.getAttrStr().c_str(), nullptr, GUIDesignLabelThickedFixed(GUIDesignHeight));
    // Create and hide MFXTextFieldTooltip for string attributes
    myValueTextField = new MFXTextFieldTooltip(this, staticTooltipMenu,
        GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // only create if parent was created
    if (getParent()->id()) {
        // create AttributesEditorRow
        FXHorizontalFrame::create();
        // Show attribute ACAttr.getAttrStr().c_str());
        myIDLabel->setTipText(ACAttr.getDefinition().c_str());
        // In any other case (String, list, etc.), show value as String
        myValueTextField->setText(id.c_str());
        myValueTextField->setTextColor(FXRGB(0, 0, 0));
        myValueTextField->killFocus();
        // Show AttributesEditorRow
        show();
    }
}


GNEDistributionEditor::AttributesEditorRow::AttributesEditorRow(
        AttributesEditor* attributeEditorParent, const std::string& type, const std::string& probability) :
    FXHorizontalFrame(attributeEditorParent->getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame),
    myAttributesEditorParent(attributeEditorParent) {
    // get staticTooltip menu
    auto staticTooltipMenu = attributeEditorParent->getFrameParent()->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu();
    // create and hide color editor
    myDeleteRowButton = new MFXButtonTooltip(this, staticTooltipMenu,
        "", GUIIconSubSys::getIcon(GUIIcon::REMOVE), this, MID_GNE_BUTTON_REMOVE, GUIDesignButtonIcon);
    // Create and hide MFXTextFieldTooltip for string attributes
    myValueTextField = new MFXTextFieldTooltip(this, staticTooltipMenu,
        GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // Create and hide MFXTextFieldTooltip for string attributes
    myProbabilityTextField = new MFXTextFieldTooltip(this, staticTooltipMenu,
        GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldRestricted(TEXTFIELD_REAL));
    // only create if parent was created
    if (getParent()->id()) {
        // create AttributesEditorRow
        FXHorizontalFrame::create();
        // set type
        myValueTextField->setText(type.c_str());
        myValueTextField->setTextColor(FXRGB(0, 0, 0));
        myValueTextField->killFocus();
        // set probability
        myProbabilityTextField->setText(probability.c_str());
        myProbabilityTextField->setTextColor(FXRGB(0, 0, 0));
        myProbabilityTextField->killFocus();
        // Show AttributesEditorRow
        show();
    }
}


void
GNEDistributionEditor::AttributesEditorRow::destroy() {
    // only destroy if parent was created
    if (getParent()->id()) {
        FXHorizontalFrame::destroy();
    }
}


void
GNEDistributionEditor::AttributesEditorRow::refreshAttributesEditorRow(const std::string& value) {
    // set last valid value and restore color if onlyValid is disabled
    myValueTextField->setText(value.c_str());
    // set blue color if is an computed value
    myValueTextField->setTextColor(FXRGB(0, 0, 0));
    myValueTextField->killFocus();
}


bool
GNEDistributionEditor::AttributesEditorRow::isAttributesEditorRowValid() const {
    return (myValueTextField->getTextColor() == FXRGB(0, 0, 0));
}


long
GNEDistributionEditor::AttributesEditorRow::onCmdSetAttribute(FXObject*, FXSelector, void*) {
    // Declare changed value
    std::string newVal;
    // obtain value of myValueTextField
    newVal = myValueTextField->getText().text();
    // get current distribution
    auto currentDistribution = myAttributesEditorParent->getDistribution();
    // continue if we have a distribution to edit
    if (currentDistribution) {
        // Check if attribute must be changed
        if (currentDistribution->isValid(SUMO_ATTR_ID, newVal)) {
            // set attribute
            currentDistribution->setAttribute(SUMO_ATTR_ID, newVal, myAttributesEditorParent->getFrameParent()->getViewNet()->getUndoList());
            // update text field
            myValueTextField->setTextColor(FXRGB(0, 0, 0));
            myValueTextField->setBackColor(FXRGB(255, 255, 255));
            myValueTextField->killFocus();
            // in this case, we need to refresh the other values (For example, allow/Disallow objects)
            myAttributesEditorParent->refreshAttributeEditor();
            // update frame parent after attribute successfully set
            myAttributesEditorParent->getFrameParent()->attributeUpdated(SUMO_ATTR_ID);
        } else {
            myValueTextField->setTextColor(FXRGB(255, 0, 0));
            if (newVal.empty()) {
                myValueTextField->setBackColor(FXRGBA(255, 213, 213, 255));
            }
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(TL("Value '") + newVal + TL("' for attribute ID of Distribution isn't valid"));
        }
    }
    return 1;
}


long
GNEDistributionEditor::AttributesEditorRow::onCmdRemoveRow(FXObject*, FXSelector, void*) {

    return 1;
}


GNEDistributionEditor::AttributesEditorRow::AttributesEditorRow() :
    myAttributesEditorParent(nullptr) {
}

// ---------------------------------------------------------------------------
// GNEDistributionEditor::AttributesEditor - methods
// ---------------------------------------------------------------------------

GNEDistributionEditor::AttributesEditor::AttributesEditor(GNEFrame* frameParent) :
    MFXGroupBoxModule(frameParent, TL("Internal attributes")),
    myFrameParent(frameParent) {
    // resize myAttributesEditorRows
    myAttributesEditorRows.resize(GNEAttributeCarrier::MAXNUMBEROFATTRIBUTES, nullptr);
    // Create help button
    myAddButton = new FXButton(getCollapsableFrame(), "", GUIIconSubSys::getIcon(GUIIcon::ADD), this, MID_GNE_BUTTON_ADD, GUIDesignButtonIcon);
}


void
GNEDistributionEditor::AttributesEditor::showAttributeEditorModule() {
    // first remove all rows
    for (auto& row : myAttributesEditorRows) {
        // destroy and delete all rows
        if (row != nullptr) {
            row->destroy();
            delete row;
            row = nullptr;
        }
    }
    // continue if we have a distribution to edit
    if (myDistribution) {

        // show AttributesEditor
        show();
    }
    // reparent help button (to place it at bottom)
    myAddButton->reparent(this);
}


void
GNEDistributionEditor::AttributesEditor::hideAttributesEditorModule() {
    // hide also AttributesEditor
    hide();
}


void
GNEDistributionEditor::AttributesEditor::refreshAttributeEditor() {
    // continue if we have a distribution to edit
    if (myDistribution) {
        // Iterate over inspected attribute carriers
        for (const auto& attrProperty : myDistribution->getTagProperty()) {
            // Refresh attributes
            myAttributesEditorRows[attrProperty.getPositionListed()]->refreshAttributesEditorRow(myDistribution->getAttribute(attrProperty.getAttr()));
        }
    }
}


GNEFrame*
GNEDistributionEditor::AttributesEditor::getFrameParent() const {
    return myFrameParent;
}


GNEDemandElement*
GNEDistributionEditor::AttributesEditor::getDistribution() const {
    return myDistribution;
}


void
GNEDistributionEditor::AttributesEditor::setDistribution(GNEDemandElement* distribution) {
    myDistribution = distribution;
}


long
GNEDistributionEditor::AttributesEditor::onCmdAddRow(FXObject*, FXSelector, void*) {
    return 1;
}

/****************************************************************************/
