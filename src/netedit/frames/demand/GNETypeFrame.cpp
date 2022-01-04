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
/// @file    GNETypeFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2018
///
// The Widget for edit Type elements (vehicle, person and container)
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_DemandElement.h>
#include <netedit/elements/demand/GNEVehicleType.h>
#include <netedit/dialogs/GNEVehicleTypeDialog.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNETypeFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNETypeFrame::VehicleTypeSelector) vehicleTypeSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_TYPE,   GNETypeFrame::VehicleTypeSelector::onCmdSelectItem)
};

FXDEFMAP(GNETypeFrame::VehicleTypeEditor) vehicleTypeEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATE,    GNETypeFrame::VehicleTypeEditor::onCmdCreateVehicleType),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_DELETE,    GNETypeFrame::VehicleTypeEditor::onCmdDeleteVehicleType),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_RESET,     GNETypeFrame::VehicleTypeEditor::onCmdResetVehicleType),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_COPY,      GNETypeFrame::VehicleTypeEditor::onCmdCopyVehicleType)
};

// Object implementation
FXIMPLEMENT(GNETypeFrame::VehicleTypeSelector,   FXGroupBoxModule,     vehicleTypeSelectorMap,     ARRAYNUMBER(vehicleTypeSelectorMap))
FXIMPLEMENT(GNETypeFrame::VehicleTypeEditor,     FXGroupBoxModule,     vehicleTypeEditorMap,       ARRAYNUMBER(vehicleTypeEditorMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNETypeFrame::VehicleTypeSelector - methods
// ---------------------------------------------------------------------------

GNETypeFrame::VehicleTypeSelector::VehicleTypeSelector(GNETypeFrame* vehicleTypeFrameParent) :
    FXGroupBoxModule(vehicleTypeFrameParent->myContentFrame, "Current Vehicle Type"),
    myTypeFrameParent(vehicleTypeFrameParent),
    myCurrentVehicleType(nullptr) {
    // Create FXComboBox
    myTypeMatchBox = new FXComboBox(getCollapsableFrame(), GUIDesignComboBoxNCol, this, MID_GNE_SET_TYPE, GUIDesignComboBox);
    // add default Vehicle an Bike types in the first and second positions
    myTypeMatchBox->appendItem(DEFAULT_VTYPE_ID.c_str());
    myTypeMatchBox->appendItem(DEFAULT_BIKETYPE_ID.c_str());
    // fill myTypeMatchBox with list of VTypes IDs
    for (const auto& vType : myTypeFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_VTYPE)) {
        if ((vType->getID() != DEFAULT_VTYPE_ID) && (vType->getID() != DEFAULT_BIKETYPE_ID)) {
            myTypeMatchBox->appendItem(vType->getID().c_str());
        }
    }
    // set DEFAULT_VEHTYPE as default VType
    myCurrentVehicleType = myTypeFrameParent->getViewNet()->getNet()->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, DEFAULT_VTYPE_ID);
    // Set visible items
    if (myTypeMatchBox->getNumItems() <= 20) {
        myTypeMatchBox->setNumVisible((int)myTypeMatchBox->getNumItems());
    } else {
        myTypeMatchBox->setNumVisible(20);
    }
    // VehicleTypeSelector is always shown
    show();
}


GNETypeFrame::VehicleTypeSelector::~VehicleTypeSelector() {}


GNEDemandElement*
GNETypeFrame::VehicleTypeSelector::getCurrentVehicleType() const {
    return myCurrentVehicleType;
}


void
GNETypeFrame::VehicleTypeSelector::setCurrentVehicleType(GNEDemandElement* vType) {
    myCurrentVehicleType = vType;
    refreshVehicleTypeSelector();
}


void
GNETypeFrame::VehicleTypeSelector::refreshVehicleTypeSelector() {
    bool valid = false;
    // clear items
    myTypeMatchBox->clearItems();
    // add default Vehicle an Bike types in the first and second positions
    myTypeMatchBox->appendItem(DEFAULT_VTYPE_ID.c_str());
    myTypeMatchBox->appendItem(DEFAULT_BIKETYPE_ID.c_str());
    // fill myTypeMatchBox with list of VTypes IDs
    for (const auto& vType : myTypeFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_VTYPE)) {
        if ((vType->getID() != DEFAULT_VTYPE_ID) && (vType->getID() != DEFAULT_BIKETYPE_ID)) {
            myTypeMatchBox->appendItem(vType->getID().c_str());
        }
    }
    // Set visible items
    if (myTypeMatchBox->getNumItems() <= 20) {
        myTypeMatchBox->setNumVisible((int)myTypeMatchBox->getNumItems());
    } else {
        myTypeMatchBox->setNumVisible(20);
    }
    // make sure that tag is in myTypeMatchBox
    if (myCurrentVehicleType) {
        for (int i = 0; i < (int)myTypeMatchBox->getNumItems(); i++) {
            if (myTypeMatchBox->getItem(i).text() == myCurrentVehicleType->getID()) {
                myTypeMatchBox->setCurrentItem(i);
                valid = true;
            }
        }
    }
    // Check that give vType type is valid
    if (!valid) {
        // set DEFAULT_VEHTYPE as default VType
        myCurrentVehicleType = myTypeFrameParent->getViewNet()->getNet()->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, DEFAULT_VTYPE_ID);
        // refresh myTypeMatchBox again
        for (int i = 0; i < (int)myTypeMatchBox->getNumItems(); i++) {
            if (myTypeMatchBox->getItem(i).text() == myCurrentVehicleType->getID()) {
                myTypeMatchBox->setCurrentItem(i);
            }
        }
    }
    // refresh vehicle type editor modul
    myTypeFrameParent->myVehicleTypeEditor->refreshVehicleTypeEditorModule();
    // set myCurrentVehicleType as inspected element
    myTypeFrameParent->getViewNet()->setInspectedAttributeCarriers({myCurrentVehicleType});
    // show Attribute Editor modul
    myTypeFrameParent->myVehicleTypeAttributesEditor->showAttributeEditorModule(false, true);
}


void
GNETypeFrame::VehicleTypeSelector::refreshVehicleTypeSelectorIDs() {
    if (myCurrentVehicleType) {
        myTypeMatchBox->setItem(myTypeMatchBox->getCurrentItem(), myCurrentVehicleType->getID().c_str());
    }
}


long
GNETypeFrame::VehicleTypeSelector::onCmdSelectItem(FXObject*, FXSelector, void*) {
    // Check if value of myTypeMatchBox correspond of an allowed additional tags
    for (const auto& vType : myTypeFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_VTYPE)) {
        if (vType->getID() == myTypeMatchBox->getText().text()) {
            // set pointer
            myCurrentVehicleType = vType;
            // set color of myTypeMatchBox to black (valid)
            myTypeMatchBox->setTextColor(FXRGB(0, 0, 0));
            // refresh vehicle type editor modul
            myTypeFrameParent->myVehicleTypeEditor->refreshVehicleTypeEditorModule();
            // set myCurrentVehicleType as inspected element
            myTypeFrameParent->getViewNet()->setInspectedAttributeCarriers({myCurrentVehicleType});
            // show moduls if selected item is valid
            myTypeFrameParent->myVehicleTypeAttributesEditor->showAttributeEditorModule(false, true);
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected item '" + myTypeMatchBox->getText() + "' in VehicleTypeSelector").text());
            return 1;
        }
    }
    myCurrentVehicleType = nullptr;
    // refresh vehicle type editor modul
    myTypeFrameParent->myVehicleTypeEditor->refreshVehicleTypeEditorModule();
    // hide all moduls if selected item isn't valid
    myTypeFrameParent->myVehicleTypeAttributesEditor->hideAttributesEditorModule();
    // set color of myTypeMatchBox to red (invalid)
    myTypeMatchBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Selected invalid item in VehicleTypeSelector");
    return 1;
}

// ---------------------------------------------------------------------------
// GNETypeFrame::VehicleTypeEditor - methods
// ---------------------------------------------------------------------------

GNETypeFrame::VehicleTypeEditor::VehicleTypeEditor(GNETypeFrame* vehicleTypeFrameParent) :
    FXGroupBoxModule(vehicleTypeFrameParent->myContentFrame, "Vehicle Type Editor"),
    myTypeFrameParent(vehicleTypeFrameParent) {
    // Create new vehicle type
    myCreateVehicleTypeButton = new FXButton(getCollapsableFrame(), "Create Vehicle Type", nullptr, this, MID_GNE_CREATE, GUIDesignButton);
    // Create delete vehicle type
    myDeleteVehicleTypeButton = new FXButton(getCollapsableFrame(), "Delete Vehicle Type", nullptr, this, MID_GNE_DELETE, GUIDesignButton);
    // Create reset vehicle type
    myResetDefaultVehicleTypeButton = new FXButton(getCollapsableFrame(), "Reset default Vehicle Type", nullptr, this, MID_GNE_RESET, GUIDesignButton);
    // Create copy vehicle type
    myCopyVehicleTypeButton = new FXButton(getCollapsableFrame(), "Copy Vehicle Type", nullptr, this, MID_GNE_COPY, GUIDesignButton);
}


GNETypeFrame::VehicleTypeEditor::~VehicleTypeEditor() {}


void
GNETypeFrame::VehicleTypeEditor::showVehicleTypeEditorModule() {
    refreshVehicleTypeEditorModule();
    show();
}


void
GNETypeFrame::VehicleTypeEditor::hideVehicleTypeEditorModule() {
    hide();
}


void
GNETypeFrame::VehicleTypeEditor::refreshVehicleTypeEditorModule() {
    // first check if selected VType is valid
    if (myTypeFrameParent->myVehicleTypeSelector->getCurrentVehicleType() == nullptr) {
        // disable all buttons except create button
        myDeleteVehicleTypeButton->disable();
        myResetDefaultVehicleTypeButton->disable();
        myCopyVehicleTypeButton->disable();
    } else if (GNEAttributeCarrier::parse<bool>(myTypeFrameParent->myVehicleTypeSelector->getCurrentVehicleType()->getAttribute(GNE_ATTR_DEFAULT_VTYPE))) {
        // enable copy button
        myCopyVehicleTypeButton->enable();
        // hide delete vehicle type buttond and show reset default vehicle type button
        myDeleteVehicleTypeButton->hide();
        myResetDefaultVehicleTypeButton->show();
        // check if reset default vehicle type button has to be enabled or disabled
        if (GNEAttributeCarrier::parse<bool>(myTypeFrameParent->myVehicleTypeSelector->getCurrentVehicleType()->getAttribute(GNE_ATTR_DEFAULT_VTYPE_MODIFIED))) {
            myResetDefaultVehicleTypeButton->enable();
        } else {
            myResetDefaultVehicleTypeButton->disable();
        }
    } else {
        // enable copy button
        myCopyVehicleTypeButton->enable();
        // show delete vehicle type button and hide reset default vehicle type button
        myDeleteVehicleTypeButton->show();
        myDeleteVehicleTypeButton->enable();
        myResetDefaultVehicleTypeButton->hide();
    }
    // update modul
    recalc();
}


long
GNETypeFrame::VehicleTypeEditor::onCmdCreateVehicleType(FXObject*, FXSelector, void*) {
    // obtain a new valid Vehicle Type ID
    const std::string vehicleTypeID = myTypeFrameParent->myViewNet->getNet()->getAttributeCarriers()->generateDemandElementID(SUMO_TAG_VTYPE);
    // create new vehicle type
    GNEDemandElement* vehicleType = new GNEVehicleType(myTypeFrameParent->myViewNet->getNet(), vehicleTypeID, SUMO_TAG_VTYPE);
    // add it using undoList (to allow undo-redo)
    myTypeFrameParent->myViewNet->getUndoList()->begin(GUIIcon::VTYPE, "create vehicle type");
    myTypeFrameParent->myViewNet->getUndoList()->add(new GNEChange_DemandElement(vehicleType, true), true);
    myTypeFrameParent->myViewNet->getUndoList()->end();
    // set created vehicle type in selector
    myTypeFrameParent->myVehicleTypeSelector->setCurrentVehicleType(vehicleType);
    // refresh VehicleType Editor Module
    myTypeFrameParent->myVehicleTypeEditor->refreshVehicleTypeEditorModule();
    return 1;
}


long
GNETypeFrame::VehicleTypeEditor::onCmdDeleteVehicleType(FXObject*, FXSelector, void*) {
    // show question dialog if vtype has already assigned vehicles
    if (myTypeFrameParent->myVehicleTypeSelector->getCurrentVehicleType()->getChildDemandElements().size() > 0) {
        std::string plural = myTypeFrameParent->myVehicleTypeSelector->getCurrentVehicleType()->getChildDemandElements().size() == 1 ? ("") : ("s");
        // show warning in gui testing debug mode
        WRITE_DEBUG("Opening FXMessageBox 'remove vType'");
        // Ask confirmation to user
        FXuint answer = FXMessageBox::question(getApp(), MBOX_YES_NO,
                                               ("Remove " + toString(SUMO_TAG_VTYPE) + "s").c_str(), "%s",
                                               ("Delete " + toString(SUMO_TAG_VTYPE) + " '" + myTypeFrameParent->myVehicleTypeSelector->getCurrentVehicleType()->getID() +
                                                "' will remove " + toString(myTypeFrameParent->myVehicleTypeSelector->getCurrentVehicleType()->getChildDemandElements().size()) +
                                                " vehicle" + plural + ". Continue?").c_str());
        if (answer != 1) { // 1:yes, 2:no, 4:esc
            // write warning if netedit is running in testing mode
            if (answer == 2) {
                WRITE_DEBUG("Closed FXMessageBox 'remove vType' with 'No'");
            } else if (answer == 4) {
                WRITE_DEBUG("Closed FXMessageBox 'remove vType' with 'ESC'");
            }
        } else {
            // begin undo list operation
            myTypeFrameParent->myViewNet->getUndoList()->begin(GUIIcon::VTYPE, "delete vehicle type");
            // remove vehicle type (and all of their children)
            myTypeFrameParent->myViewNet->getNet()->deleteDemandElement(myTypeFrameParent->myVehicleTypeSelector->getCurrentVehicleType(),
                    myTypeFrameParent->myViewNet->getUndoList());
            // end undo list operation
            myTypeFrameParent->myViewNet->getUndoList()->end();
        }
    } else {
        // begin undo list operation
        myTypeFrameParent->myViewNet->getUndoList()->begin(GUIIcon::VTYPE, "delete vehicle type");
        // remove vehicle type (and all of their children)
        myTypeFrameParent->myViewNet->getNet()->deleteDemandElement(myTypeFrameParent->myVehicleTypeSelector->getCurrentVehicleType(),
                myTypeFrameParent->myViewNet->getUndoList());
        // end undo list operation
        myTypeFrameParent->myViewNet->getUndoList()->end();
    }
    return 1;
}


long
GNETypeFrame::VehicleTypeEditor::onCmdResetVehicleType(FXObject*, FXSelector, void*) {
    // begin reset default vehicle type values
    myTypeFrameParent->getViewNet()->getUndoList()->begin(GUIIcon::VTYPE, "reset default vehicle type values");
    // reset all values of default vehicle type
    for (const auto& i : GNEAttributeCarrier::getTagProperty(SUMO_TAG_VTYPE)) {
        // change all attributes with "" to reset it (except ID and vClass)
        if ((i.getAttr() != SUMO_ATTR_ID) && (i.getAttr() != SUMO_ATTR_VCLASS)) {
            myTypeFrameParent->myVehicleTypeSelector->getCurrentVehicleType()->setAttribute(i.getAttr(), "", myTypeFrameParent->myViewNet->getUndoList());
        }
    }
    // change manually VClass (because it depends of Default VType)
    if (myTypeFrameParent->myVehicleTypeSelector->getCurrentVehicleType()->getAttribute(SUMO_ATTR_ID) == DEFAULT_VTYPE_ID) {
        myTypeFrameParent->myVehicleTypeSelector->getCurrentVehicleType()->setAttribute(SUMO_ATTR_VCLASS, toString(SVC_PASSENGER), myTypeFrameParent->myViewNet->getUndoList());
    } else if (myTypeFrameParent->myVehicleTypeSelector->getCurrentVehicleType()->getAttribute(SUMO_ATTR_ID) == DEFAULT_BIKETYPE_ID) {
        myTypeFrameParent->myVehicleTypeSelector->getCurrentVehicleType()->setAttribute(SUMO_ATTR_VCLASS, toString(SVC_BICYCLE), myTypeFrameParent->myViewNet->getUndoList());
    }
    // change special attribute GNE_ATTR_DEFAULT_VTYPE_MODIFIED
    myTypeFrameParent->myVehicleTypeSelector->getCurrentVehicleType()->setAttribute(GNE_ATTR_DEFAULT_VTYPE_MODIFIED, "false", myTypeFrameParent->myViewNet->getUndoList());
    // finish reset default vehicle type values
    myTypeFrameParent->getViewNet()->getUndoList()->end();
    // refresh VehicleTypeSelector
    myTypeFrameParent->myVehicleTypeSelector->refreshVehicleTypeSelector();
    return 1;
}


long
GNETypeFrame::VehicleTypeEditor::onCmdCopyVehicleType(FXObject*, FXSelector, void*) {
    // obtain a new valid Vehicle Type ID
    const std::string vehicleTypeID = myTypeFrameParent->myViewNet->getNet()->getAttributeCarriers()->generateDemandElementID(SUMO_TAG_VTYPE);
    // obtain vehicle type in which new Vehicle Type will be based
    GNEVehicleType* vType = dynamic_cast<GNEVehicleType*>(myTypeFrameParent->myVehicleTypeSelector->getCurrentVehicleType());
    // check that vType exist
    if (vType) {
        // create a new Vehicle Type based on the current selected vehicle type
        GNEDemandElement* vehicleTypeCopy = new GNEVehicleType(myTypeFrameParent->myViewNet->getNet(), vehicleTypeID, vType);
        // begin undo list operation
        myTypeFrameParent->myViewNet->getUndoList()->begin(GUIIcon::VTYPE, "copy vehicle type");
        // add it using undoList (to allow undo-redo)
        myTypeFrameParent->myViewNet->getUndoList()->add(new GNEChange_DemandElement(vehicleTypeCopy, true), true);
        // end undo list operation
        myTypeFrameParent->myViewNet->getUndoList()->end();
        // refresh Vehicle Type Selector (to show the new VType)
        myTypeFrameParent->myVehicleTypeSelector->refreshVehicleTypeSelector();
        // set created vehicle type in selector
        myTypeFrameParent->myVehicleTypeSelector->setCurrentVehicleType(vehicleTypeCopy);
        // refresh VehicleType Editor Module
        myTypeFrameParent->myVehicleTypeEditor->refreshVehicleTypeEditorModule();
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNETypeFrame - methods
// ---------------------------------------------------------------------------

GNETypeFrame::GNETypeFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Vehicle Types") {

    // create modul for edit vehicle types (Create, copy, etc.)
    myVehicleTypeEditor = new VehicleTypeEditor(this);

    // create vehicle type selector
    myVehicleTypeSelector = new VehicleTypeSelector(this);

    // Create vehicle type attributes editor
    myVehicleTypeAttributesEditor = new GNEFrameAttributeModules::AttributesEditor(this);

    // create modul for open extended attributes dialog
    myAttributesEditorExtended = new GNEFrameAttributeModules::AttributesEditorExtended(this);

    // set "VTYPE_DEFAULT" as default vehicle Type
    myVehicleTypeSelector->setCurrentVehicleType(myViewNet->getNet()->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, DEFAULT_VTYPE_ID));
}


GNETypeFrame::~GNETypeFrame() {}


void
GNETypeFrame::show() {
    // refresh vehicle type and Attribute Editor
    myVehicleTypeSelector->refreshVehicleTypeSelector();
    // set myCurrentVehicleType as inspected element
    myVehicleTypeAttributesEditor->getFrameParent()->getViewNet()->setInspectedAttributeCarriers({myVehicleTypeSelector->getCurrentVehicleType()});
    // show vehicle type attributes editor (except extended attributes)
    myVehicleTypeAttributesEditor->showAttributeEditorModule(false, true);
    // show frame
    GNEFrame::show();
}


GNETypeFrame::VehicleTypeSelector*
GNETypeFrame::getVehicleTypeSelector() const {
    return myVehicleTypeSelector;
}


void
GNETypeFrame::attributeUpdated() {
    // after changing an attribute myVehicleTypeSelector, we need to update the list of vehicleTypeSelector, because ID could be changed
    myVehicleTypeSelector->refreshVehicleTypeSelectorIDs();
}


void
GNETypeFrame::attributesEditorExtendedDialogOpened() {
    // open vehicle type dialog
    if (myVehicleTypeSelector->getCurrentVehicleType()) {
        GNEVehicleTypeDialog(myVehicleTypeSelector->getCurrentVehicleType(), true);
        // set myCurrentVehicleType as inspected element
        myVehicleTypeAttributesEditor->getFrameParent()->getViewNet()->setInspectedAttributeCarriers({myVehicleTypeSelector->getCurrentVehicleType()});
        // call "showAttributeEditorModule" to refresh attribute list
        myVehicleTypeAttributesEditor->showAttributeEditorModule(false, true);
    }
}


/****************************************************************************/
