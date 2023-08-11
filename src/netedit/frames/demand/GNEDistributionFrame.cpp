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

#include "GNEDistributionFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEDistributionEditor::DistributionEditor) DistributionEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATE,    GNEDistributionEditor::DistributionEditor::onCmdCreateType),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_DELETE,    GNEDistributionEditor::DistributionEditor::onCmdDeleteType),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_DELETE,    GNEDistributionEditor::DistributionEditor::onUpdDeleteType),
};

FXDEFMAP(GNEDistributionEditor::AttributeRow) AttributeRowMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEDistributionEditor::AttributeRow::onCmdSetAttribute),
};

FXDEFMAP(GNEDistributionEditor::DistributionRow) DistributionRowMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEDistributionEditor::DistributionRow::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_REMOVE,  GNEDistributionEditor::DistributionRow::onCmdRemoveRow)
};

FXDEFMAP(GNEDistributionEditor::AttributesEditor) AttributesEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_ADD,   GNEDistributionEditor::AttributesEditor::onCmdAddRow)
};

// Object implementation
FXIMPLEMENT(GNEDistributionEditor::DistributionEditor,  MFXGroupBoxModule,  DistributionEditorMap,  ARRAYNUMBER(DistributionEditorMap))
FXIMPLEMENT(GNEDistributionEditor::AttributeRow,        FXHorizontalFrame,  AttributeRowMap,        ARRAYNUMBER(AttributeRowMap))
FXIMPLEMENT(GNEDistributionEditor::DistributionRow,     FXHorizontalFrame,  DistributionRowMap,     ARRAYNUMBER(DistributionRowMap))
FXIMPLEMENT(GNEDistributionEditor::AttributesEditor,    MFXGroupBoxModule,  AttributesEditorMap,    ARRAYNUMBER(AttributesEditorMap))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEDistributionEditor::DistributionEditor - methods
// ---------------------------------------------------------------------------

GNEDistributionEditor::DistributionEditor::DistributionEditor(GNEFrame* frameParent) :
    MFXGroupBoxModule(frameParent, TL("Distribution Editor")),
    myFrameParent(frameParent) {
    // Create new vehicle type
    myCreateTypeButton = new FXButton(getCollapsableFrame(), TL("New"), GUIIconSubSys::getIcon(GUIIcon::VTYPEDISTRIBUTION), this, MID_GNE_CREATE, GUIDesignButton);
    
    // Añadir TOOLTIP

    // Create delete/reset vehicle type
    myDeleteTypeButton = new FXButton(getCollapsableFrame(), TL("Delete"), GUIIconSubSys::getIcon(GUIIcon::MODEDELETE), this, MID_GNE_DELETE, GUIDesignButton);
    // show type editor
    show();
}


GNEDistributionEditor::DistributionEditor::~DistributionEditor() {}


long
GNEDistributionEditor::DistributionEditor::onCmdCreateType(FXObject*, FXSelector, void*) {
    auto undoList = myFrameParent->getViewNet()->getUndoList();
    // obtain a new valid Type ID
    const std::string typeDistributionID = myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->generateDemandElementID(SUMO_TAG_VTYPE_DISTRIBUTION);
    // create new vehicle type
    GNEDemandElement* type = new GNEVTypeDistribution(myFrameParent->getViewNet()->getNet(), typeDistributionID);
    // add it using undoList (to allow undo-redo)
    undoList->begin(GUIIcon::VTYPEDISTRIBUTION, "create vehicle type distribution");
    undoList->add(new GNEChange_DemandElement(type, true), true);
    undoList->end();
    // refresh type distribution
    myFrameParent->myTypeDistributionSelector->refreshTypeDistributionSelector();
    return 1;
}


long
GNEDistributionEditor::DistributionEditor::onCmdDeleteType(FXObject*, FXSelector, void*) {
    auto undoList = myFrameParent->getViewNet()->getUndoList();
    // begin undo list operation
    undoList->begin(GUIIcon::VTYPE, "delete vehicle type distribution");
    // remove vehicle type (and all of their children)
    myFrameParent->getViewNet()->getNet()->deleteDemandElement(myFrameParent->myDistributionEditor->getDistribution(), undoList);
    // end undo list operation
    undoList->end();
    // refresh type distribution
    myFrameParent->myTypeDistributionSelector->refreshTypeDistributionSelector();
    return 1;
}


long
GNEDistributionEditor::DistributionEditor::onUpdDeleteType(FXObject* sender, FXSelector, void*) {
    // first check if selected VType is valid
    if (myFrameParent->myDistributionEditor->getDistribution()) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    } else {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    }
}

// ---------------------------------------------------------------------------
// GNEDistributionEditor::AttributeRow - methods
// ---------------------------------------------------------------------------

GNEDistributionEditor::AttributeRow::AttributeRow(
        GNEDistributionEditor::AttributesEditor* attributeEditorParent,
        const GNEAttributeProperties& ACAttr, const std::string& attribute) :
    FXHorizontalFrame(attributeEditorParent->getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame),
    myAttributesEditorParent(attributeEditorParent),
    myACAttr(ACAttr) {
    // get staticTooltip menu
    auto staticTooltipMenu = attributeEditorParent->getFrameParent()->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu();
    // Create attribute label (usually used only for ID)
    myAttributeLabel = new MFXLabelTooltip(this, staticTooltipMenu,
        ACAttr.getAttrStr().c_str(), nullptr, GUIDesignLabelThickedFixed(GUIDesignHeight));
    // Create and hide MFXTextFieldTooltip for string attributes
    myValueTextField = new MFXTextFieldTooltip(this, staticTooltipMenu,
        GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // only create if parent was created
    if (getParent()->id()) {
        // create AttributeRow
        FXHorizontalFrame::create();
        // Show attribute ACAttr.getAttrStr().c_str());
        myAttributeLabel->setTipText(ACAttr.getDefinition().c_str());
        // show value
        myValueTextField->setText(attribute.c_str());
        myValueTextField->setTextColor(FXRGB(0, 0, 0));
        myValueTextField->killFocus();
        // Show AttributeRow
        show();
    }
}


void
GNEDistributionEditor::AttributeRow::destroy() {
    // only destroy if parent was created
    if (getParent()->id()) {
        FXHorizontalFrame::destroy();
    }
}


void
GNEDistributionEditor::AttributeRow::refreshAttributeRow(const std::string& value) {
    // set last valid value and restore color if onlyValid is disabled
    myValueTextField->setText(value.c_str());
    // set blue color if is an computed value
    myValueTextField->setTextColor(FXRGB(0, 0, 0));
    myValueTextField->killFocus();
}


bool
GNEDistributionEditor::AttributeRow::isAttributeRowValid() const {
    return (myValueTextField->getTextColor() == FXRGB(0, 0, 0));
}


long
GNEDistributionEditor::AttributeRow::onCmdSetAttribute(FXObject*, FXSelector, void*) {
    // Declare changed value
    std::string newVal;
    // obtain value of myValueTextField
    newVal = myValueTextField->getText().text();
    // get current distribution
    auto currentDistribution = myAttributesEditorParent->getDistribution();
    // continue if we have a distribution to edit
    if (currentDistribution) {
        // Check if attribute must be changed
        if (currentDistribution->isValid(myACAttr.getAttr(), newVal)) {
            // set attribute
            currentDistribution->setAttribute(myACAttr.getAttr(), newVal, myAttributesEditorParent->getFrameParent()->getViewNet()->getUndoList());
            // update text field
            myValueTextField->setTextColor(FXRGB(0, 0, 0));
            myValueTextField->setBackColor(FXRGB(255, 255, 255));
            myValueTextField->killFocus();
            // in this case, we need to refresh the other values (For example, allow/Disallow objects)
            myAttributesEditorParent->refreshAttributeEditor();
            // update frame parent after attribute successfully set
            myAttributesEditorParent->getFrameParent()->attributeUpdated(myACAttr.getAttr());
        } else {
            myValueTextField->setTextColor(FXRGB(255, 0, 0));
            if (newVal.empty()) {
                myValueTextField->setBackColor(FXRGBA(255, 213, 213, 255));
            }
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG("Value '" + newVal + "' for attribute " + myACAttr.getAttrStr() + " of Distribution isn't valid");
        }
    }
    return 1;
}


GNEDistributionEditor::AttributeRow::AttributeRow() :
    myAttributesEditorParent(nullptr) {
}

// ---------------------------------------------------------------------------
// GNEDistributionEditor::DistributionRow - methods
// ---------------------------------------------------------------------------

GNEDistributionEditor::DistributionRow::DistributionRow(
        AttributesEditor* attributeEditorParent, const GNEDemandElement* key, const double probability) :
    FXHorizontalFrame(attributeEditorParent->getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame),
    myAttributesEditorParent(attributeEditorParent),
    myKey(key),
    myProbability(probability) {
    // get staticTooltip menu
    auto staticTooltipMenu = attributeEditorParent->getFrameParent()->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu();
    // create and hide color editor
    myDeleteRowButton = new MFXButtonTooltip(this, staticTooltipMenu,
        "", GUIIconSubSys::getIcon(GUIIcon::REMOVE), this, MID_GNE_BUTTON_REMOVE, GUIDesignButtonIcon);
    // Create and hide MFXTextFieldTooltip for string attributes
    myComboBoxKeys = new MFXComboBoxIcon(this, GUIDesignComboBoxNCol, true, this, MID_GNE_SET_TYPE, GUIDesignComboBoxAttribute);
    // Create and hide MFXTextFieldTooltip for string attributes
    myProbabilityTextField = new MFXTextFieldTooltip(this, staticTooltipMenu,
        GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldFixedRestricted(50, TEXTFIELD_REAL));
    // only create if parent was created
    if (getParent()->id()) {
        // create DistributionRow
        FXHorizontalFrame::create();
        // fill comboBox with all possible keys
        const auto possibleKeys = attributeEditorParent->getDistribution()->getPossibleDistributionKeys(SUMO_TAG_VTYPE);
        for (int i = 0; i < (int)possibleKeys.size(); i++) {
            myComboBoxKeys->appendIconItem(possibleKeys[i]->getID().c_str(), possibleKeys[i]->getFXIcon());
            if (possibleKeys[i] == myKey) {
                myComboBoxKeys->setCurrentItem(i);
            }
        }
        // adjust combo Box
        myComboBoxKeys->setNumVisible(myComboBoxKeys->getNumItems() <= 10? myComboBoxKeys->getNumItems() : 10);
        myComboBoxKeys->setTextColor(FXRGB(0, 0, 0));
        myComboBoxKeys->killFocus();
        // set probability
        myProbabilityTextField->setText(toString(myProbability).c_str());
        myProbabilityTextField->setTextColor(FXRGB(0, 0, 0));
        myProbabilityTextField->killFocus();
        // Show DistributionRow
        show();
    }
}


void
GNEDistributionEditor::DistributionRow::destroy() {
    // only destroy if parent was created
    if (getParent()->id()) {
        FXHorizontalFrame::destroy();
    }
}


void
GNEDistributionEditor::DistributionRow::refreshDistributionRow(const GNEDemandElement* key, const double value) {
    // set key
    myComboBoxKeys->setText(key->getID().c_str());
    // set probability
    myProbabilityTextField->setText(toString(value).c_str());
    myProbabilityTextField->setTextColor(FXRGB(0, 0, 0));
    myProbabilityTextField->killFocus();
}


bool
GNEDistributionEditor::DistributionRow::isDistributionRowValid() const {
    return (myComboBoxKeys->getTextColor() == FXRGB(0, 0, 0));
}


long
GNEDistributionEditor::DistributionRow::onCmdSetAttribute(FXObject* obj, FXSelector, void*) {
    // get Undo list
    GNEUndoList* undoList = myAttributesEditorParent->getFrameParent()->getViewNet()->getUndoList();
    // get current distribution
    auto currentDistribution = myAttributesEditorParent->getDistribution();
    // continue if we have a distribution to edit
    if (currentDistribution == nullptr) {
        return 1;
    }
    // continue depending of calle dobj
    if (obj == myComboBoxKeys) {
        if (isValidKey()) {
            myComboBoxKeys->setTextColor(FXRGB(0, 0, 0));
            // get new key
            const auto newKey = myAttributesEditorParent->getFrameParent()->getViewNet()->getNet()->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, myComboBoxKeys->getText().text());
            // only change if is different of current key
            if (myKey != newKey) {
                // change distribution key removing and adding it
                undoList->begin(myKey, "edit distribution key");
                currentDistribution->removeDistributionKey(myKey, undoList);
                myKey = myAttributesEditorParent->getFrameParent()->getViewNet()->getNet()->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, myComboBoxKeys->getText().text());
                currentDistribution->addDistributionKey(myKey, myProbability, undoList);
                undoList->end();
            }
            // refresh rows
            myAttributesEditorParent->refreshAttributeEditor();
        } else {
            myComboBoxKeys->setBackColor(FXRGB(255, 255, 255));
            myComboBoxKeys->killFocus();
        }
    } else if (obj == myProbabilityTextField) {
        // get probability
        const std::string probabilityStr = myProbabilityTextField->getText().text();
        const double probability = GNEAttributeCarrier::canParse<double>(probabilityStr)? GNEAttributeCarrier::parse<double>(probabilityStr) : -1;
        // Check if set new probability
        if (probability >= 0) {
            myProbabilityTextField->setTextColor(FXRGB(0, 0, 0));
            currentDistribution->editDistributionValue(myKey, probability, undoList);
        } else {
            myProbabilityTextField->setBackColor(FXRGB(255, 255, 255));
            myProbabilityTextField->killFocus();
        }
    }
    return 1;
}


long
GNEDistributionEditor::DistributionRow::onCmdRemoveRow(FXObject*, FXSelector, void*) {

    return 1;
}


GNEDistributionEditor::DistributionRow::DistributionRow() :
    myAttributesEditorParent(nullptr) {
}


bool
GNEDistributionEditor::DistributionRow::isValidKey() const {
    // get element associated with key
    const auto element = myAttributesEditorParent->getFrameParent()->getViewNet()->getNet()->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, myComboBoxKeys->getText().text(), false);
    // first check if element exists
    if (element) {
        // avoid duplicated keys
        return !myKey->keyExists(element);
    } else {
        return false;
    }
}

// ---------------------------------------------------------------------------
// GNEDistributionEditor::AttributesEditor - methods
// ---------------------------------------------------------------------------

GNEDistributionEditor::AttributesEditor::AttributesEditor(GNEFrame* frameParent) :
    MFXGroupBoxModule(frameParent, TL("Internal attributes")),
    myFrameParent(frameParent) {
    // resize myDistributionRows
    myDistributionRows.resize(GNEAttributeCarrier::MAXNUMBEROFATTRIBUTES, nullptr);
    // Create help button
    myAddButton = new FXButton(getCollapsableFrame(), "", GUIIconSubSys::getIcon(GUIIcon::ADD), this, MID_GNE_BUTTON_ADD, GUIDesignButtonIcon);
}


void
GNEDistributionEditor::AttributesEditor::showAttributeEditorModule() {
    // first remove all rows
    for (auto& row : myDistributionRows) {
        // destroy and delete all rows
        if (row != nullptr) {
            row->destroy();
            delete row;
            row = nullptr;
        }
    }
    // also destroy ID
    if (myIDAttributeRow) {
        myIDAttributeRow->destroy();
        delete myIDAttributeRow;
    }
    // continue if we have a distribution to edit
    if (myDistribution) {
        // create ID row
        myIDAttributeRow = new AttributeRow(this, myDistribution->getTagProperty().getAttributeProperties(SUMO_ATTR_ID), myDistribution->getAttribute(SUMO_ATTR_ID));
        // refresh attribute editor
        refreshAttributeEditor();
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
        // first remove all rows
        for (auto& row : myDistributionRows) {
            // destroy and delete all rows
            if (row != nullptr) {
                row->destroy();
                delete row;
                row = nullptr;
            }
        }
        // Iterate over distribution key-values
        for (const auto& keyValue : myDistribution->getDistributionKeyValues()) {
            // create distribution row
            auto distributionRow = new DistributionRow(this, keyValue.first, keyValue.second);
            // add into distribution rows
            myDistributionRows.push_back(distributionRow);
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
