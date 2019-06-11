/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEPersonTypeFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2019
/// @version $Id$
///
// The Widget for edit person type (VTypes with vclass='pedestrian) elements
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIDesigns.h>
#include <netedit/changes/GNEChange_DemandElement.h>
#include <netedit/dialogs/GNEVehicleTypeDialog.h>
#include <netedit/demandelements/GNEVehicleType.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>

#include "GNEPersonTypeFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEPersonTypeFrame::PersonTypeSelector) vehicleTypeSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_TYPE,   GNEPersonTypeFrame::PersonTypeSelector::onCmdSelectItem)
};

FXDEFMAP(GNEPersonTypeFrame::PersonTypeEditor) vehicleTypeEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_VEHICLETYPEFRAME_CREATE,    GNEPersonTypeFrame::PersonTypeEditor::onCmdCreatePersonType),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_VEHICLETYPEFRAME_DELETE,    GNEPersonTypeFrame::PersonTypeEditor::onCmdDeletePersonType),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_VEHICLETYPEFRAME_RESET,     GNEPersonTypeFrame::PersonTypeEditor::onCmdResetPersonType),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_VEHICLETYPEFRAME_COPY,      GNEPersonTypeFrame::PersonTypeEditor::onCmdCopyPersonType)
};

// Object implementation
FXIMPLEMENT(GNEPersonTypeFrame::PersonTypeSelector,   FXGroupBox,     vehicleTypeSelectorMap,     ARRAYNUMBER(vehicleTypeSelectorMap))
FXIMPLEMENT(GNEPersonTypeFrame::PersonTypeEditor,     FXGroupBox,     vehicleTypeEditorMap,       ARRAYNUMBER(vehicleTypeEditorMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEPersonTypeFrame::PersonTypeSelector - methods
// ---------------------------------------------------------------------------

GNEPersonTypeFrame::PersonTypeSelector::PersonTypeSelector(GNEPersonTypeFrame* vehicleTypeFrameParent) :
    FXGroupBox(vehicleTypeFrameParent->myContentFrame, "Current Vehicle Type", GUIDesignGroupBoxFrame),
    myPersonTypeFrameParent(vehicleTypeFrameParent),
    myCurrentPersonType(nullptr) {
    // Create FXComboBox
    myTypeMatchBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SET_TYPE, GUIDesignComboBox);
    // fill myTypeMatchBox with list of VTypes IDs
    for (const auto& i : myPersonTypeFrameParent->getViewNet()->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_VTYPE)) {
        myTypeMatchBox->appendItem(i.first.c_str());
    }
    // set DEFAULT_VEHTYPE as default VType
    myCurrentPersonType = myPersonTypeFrameParent->getViewNet()->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, DEFAULT_VTYPE_ID);
    // Set visible items
    if (myTypeMatchBox->getNumItems() <= 20) {
        myTypeMatchBox->setNumVisible((int)myTypeMatchBox->getNumItems());
    } else {
        myTypeMatchBox->setNumVisible(20);
    }
    // PersonTypeSelector is always shown
    show();
}


GNEPersonTypeFrame::PersonTypeSelector::~PersonTypeSelector() {}


GNEDemandElement*
GNEPersonTypeFrame::PersonTypeSelector::getCurrentPersonType() const {
    return myCurrentPersonType;
}


void
GNEPersonTypeFrame::PersonTypeSelector::setCurrentPersonType(GNEDemandElement* vType) {
    myCurrentPersonType = vType;
    refreshPersonTypeSelector();
}


void
GNEPersonTypeFrame::PersonTypeSelector::refreshPersonTypeSelector() {
    bool valid = false;
    myTypeMatchBox->clearItems();
    // fill myTypeMatchBox with list of VTypes IDs
    for (const auto& i : myPersonTypeFrameParent->getViewNet()->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_VTYPE)) {
        myTypeMatchBox->appendItem(i.first.c_str());
    }
    // Set visible items
    if (myTypeMatchBox->getNumItems() <= 20) {
        myTypeMatchBox->setNumVisible((int)myTypeMatchBox->getNumItems());
    } else {
        myTypeMatchBox->setNumVisible(20);
    }
    // make sure that tag is in myTypeMatchBox
    for (int i = 0; i < (int)myTypeMatchBox->getNumItems(); i++) {
        if (myTypeMatchBox->getItem(i).text() == myCurrentPersonType->getID()) {
            myTypeMatchBox->setCurrentItem(i);
            valid = true;
        }
    }
    // Check that give vType type is valid
    if (!valid) {
        // set DEFAULT_VEHTYPE as default VType
        myCurrentPersonType = myPersonTypeFrameParent->getViewNet()->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, DEFAULT_VTYPE_ID);
        // refresh myTypeMatchBox again
        for (int i = 0; i < (int)myTypeMatchBox->getNumItems(); i++) {
            if (myTypeMatchBox->getItem(i).text() == myCurrentPersonType->getID()) {
                myTypeMatchBox->setCurrentItem(i);
            }
        }
    }
    // refresh vehicle type editor modul
    myPersonTypeFrameParent->myPersonTypeEditor->refreshPersonTypeEditorModul();
    // show Attribute Editor modul if selected item is valid
    myPersonTypeFrameParent->myPersonTypeAttributesEditor->showAttributeEditorModul({myCurrentPersonType}, false);
}


long
GNEPersonTypeFrame::PersonTypeSelector::onCmdSelectItem(FXObject*, FXSelector, void*) {
    // Check if value of myTypeMatchBox correspond of an allowed additional tags
    for (const auto& i : myPersonTypeFrameParent->getViewNet()->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_VTYPE)) {
        if (i.first == myTypeMatchBox->getText().text()) {
            // set pointer
            myCurrentPersonType = i.second;
            // set color of myTypeMatchBox to black (valid)
            myTypeMatchBox->setTextColor(FXRGB(0, 0, 0));
            // refresh vehicle type editor modul
            myPersonTypeFrameParent->myPersonTypeEditor->refreshPersonTypeEditorModul();
            // show moduls if selected item is valid
            myPersonTypeFrameParent->myPersonTypeAttributesEditor->showAttributeEditorModul({myCurrentPersonType}, false);
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected item '" + myTypeMatchBox->getText() + "' in PersonTypeSelector").text());
            return 1;
        }
    }
    myCurrentPersonType = nullptr;
    // refresh vehicle type editor modul
    myPersonTypeFrameParent->myPersonTypeEditor->refreshPersonTypeEditorModul();
    // hide all moduls if selected item isn't valid
    myPersonTypeFrameParent->myPersonTypeAttributesEditor->hideAttributesEditorModul();
    // set color of myTypeMatchBox to red (invalid)
    myTypeMatchBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Selected invalid item in PersonTypeSelector");
    return 1;
}

// ---------------------------------------------------------------------------
// GNEPersonTypeFrame::PersonTypeEditor - methods
// ---------------------------------------------------------------------------

GNEPersonTypeFrame::PersonTypeEditor::PersonTypeEditor(GNEPersonTypeFrame* vehicleTypeFrameParent) :
    FXGroupBox(vehicleTypeFrameParent->myContentFrame, "Vehicle Type Editor", GUIDesignGroupBoxFrame),
    myPersonTypeFrameParent(vehicleTypeFrameParent) {
    // Create new vehicle type
    myCreatePersonTypeButton = new FXButton(this, "Create Vehicle Type", nullptr, this, MID_GNE_VEHICLETYPEFRAME_CREATE, GUIDesignButton);
    // Create delete vehicle type
    myDeletePersonTypeButton = new FXButton(this, "Delete Vehicle Type", nullptr, this, MID_GNE_VEHICLETYPEFRAME_DELETE, GUIDesignButton);
    // Create reset vehicle type
    myResetDefaultPersonTypeButton = new FXButton(this, "Reset default Vehicle Type", nullptr, this, MID_GNE_VEHICLETYPEFRAME_RESET, GUIDesignButton);
    // Create copy vehicle type
    myCopyPersonTypeButton = new FXButton(this, "Copy Vehicle Type", nullptr, this, MID_GNE_VEHICLETYPEFRAME_COPY, GUIDesignButton);
}


GNEPersonTypeFrame::PersonTypeEditor::~PersonTypeEditor() {}


void
GNEPersonTypeFrame::PersonTypeEditor::showPersonTypeEditorModul() {
    refreshPersonTypeEditorModul();
    show();
}


void
GNEPersonTypeFrame::PersonTypeEditor::hidePersonTypeEditorModul() {
    hide();
}


void
GNEPersonTypeFrame::PersonTypeEditor::refreshPersonTypeEditorModul() {
    // first check if selected VType is valid
    if (myPersonTypeFrameParent->myPersonTypeSelector->getCurrentPersonType() == nullptr) {
        // disable all buttons except create button
        myDeletePersonTypeButton->disable();
        myResetDefaultPersonTypeButton->disable();
        myCopyPersonTypeButton->disable();
    } else if (GNEAttributeCarrier::parse<bool>(myPersonTypeFrameParent->myPersonTypeSelector->getCurrentPersonType()->getAttribute(GNE_ATTR_DEFAULT_VTYPE))) {
        // enable copy button
        myCopyPersonTypeButton->enable();
        // hide delete vehicle type buttond and show reset default vehicle type button
        myDeletePersonTypeButton->hide();
        myResetDefaultPersonTypeButton->show();
        // check if reset default vehicle type button has to be enabled or disabled
        if (GNEAttributeCarrier::parse<bool>(myPersonTypeFrameParent->myPersonTypeSelector->getCurrentPersonType()->getAttribute(GNE_ATTR_DEFAULT_VTYPE_MODIFIED))) {
            myResetDefaultPersonTypeButton->enable();
        } else {
            myResetDefaultPersonTypeButton->disable();
        }
    } else {
        // enable copy button
        myCopyPersonTypeButton->enable();
        // show delete vehicle type button and hide reset default vehicle type button
        myDeletePersonTypeButton->show();
        myDeletePersonTypeButton->enable();
        myResetDefaultPersonTypeButton->hide();
    }
    // update modul
    recalc();
}


long
GNEPersonTypeFrame::PersonTypeEditor::onCmdCreatePersonType(FXObject*, FXSelector, void*) {
    // obtain a new valid Vehicle Type ID
    std::string vehicleTypeID = myPersonTypeFrameParent->myViewNet->getNet()->generateDemandElementID("", SUMO_TAG_VTYPE);
    // create new vehicle type
    GNEDemandElement* personType = new GNEVehicleType(myPersonTypeFrameParent->myViewNet, vehicleTypeID);
    // add it using undoList (to allow undo-redo)
    myPersonTypeFrameParent->myViewNet->getUndoList()->p_begin("create person type");
    myPersonTypeFrameParent->myViewNet->getUndoList()->add(new GNEChange_DemandElement(personType, true), true);
    myPersonTypeFrameParent->myViewNet->getUndoList()->p_end();
    // set created vehicle type in selector
    myPersonTypeFrameParent->myPersonTypeSelector->setCurrentPersonType(personType);
    // refresh PersonType Editor Modul
    myPersonTypeFrameParent->myPersonTypeEditor->refreshPersonTypeEditorModul();
    return 1;
}


long
GNEPersonTypeFrame::PersonTypeEditor::onCmdDeletePersonType(FXObject*, FXSelector, void*) {
    // show question dialog if vtype has already assigned vehicles
    if (myPersonTypeFrameParent->myPersonTypeSelector->getCurrentPersonType()->getDemandElementChilds().size() > 0) {
        std::string plural = myPersonTypeFrameParent->myPersonTypeSelector->getCurrentPersonType()->getDemandElementChilds().size() == 1 ? ("") : ("s");
        // show warning in gui testing debug mode
        WRITE_DEBUG("Opening FXMessageBox 'remove vType'");
        // Ask confirmation to user
        FXuint answer = FXMessageBox::question(getApp(), MBOX_YES_NO,
                                               ("Remove " + toString(SUMO_TAG_VTYPE) + "s").c_str(), "%s",
                                               ("Delete " + toString(SUMO_TAG_VTYPE) + " '" + myPersonTypeFrameParent->myPersonTypeSelector->getCurrentPersonType()->getID() +
                                                "' will remove " + toString(myPersonTypeFrameParent->myPersonTypeSelector->getCurrentPersonType()->getDemandElementChilds().size()) +
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
            myPersonTypeFrameParent->myViewNet->getUndoList()->p_begin("delete vehicle type");
            // remove vehicle type (and all of their childs)
            myPersonTypeFrameParent->myViewNet->getNet()->deleteDemandElement(myPersonTypeFrameParent->myPersonTypeSelector->getCurrentPersonType(),
                myPersonTypeFrameParent->myViewNet->getUndoList());
            // end undo list operation
            myPersonTypeFrameParent->myViewNet->getUndoList()->p_end();
        }
    } else {
        // begin undo list operation
        myPersonTypeFrameParent->myViewNet->getUndoList()->p_begin("delete vehicle type");
        // remove vehicle type (and all of their childs)
        myPersonTypeFrameParent->myViewNet->getNet()->deleteDemandElement(myPersonTypeFrameParent->myPersonTypeSelector->getCurrentPersonType(),
            myPersonTypeFrameParent->myViewNet->getUndoList());
        // end undo list operation
        myPersonTypeFrameParent->myViewNet->getUndoList()->p_end();
    }
    return 1;
}


long
GNEPersonTypeFrame::PersonTypeEditor::onCmdResetPersonType(FXObject*, FXSelector, void*) {
    // begin reset default vehicle type values
    myPersonTypeFrameParent->getViewNet()->getUndoList()->p_begin("reset default vehicle type values");
    // reset all values of default vehicle type
    for (const auto& i : GNEAttributeCarrier::getTagProperties(SUMO_TAG_VTYPE)) {
        // change all attributes with "" to reset it (except ID and vClass)
        if ((i.first != SUMO_ATTR_ID) && (i.first != SUMO_ATTR_VCLASS)) {
            myPersonTypeFrameParent->myPersonTypeSelector->getCurrentPersonType()->setAttribute(i.first, "", myPersonTypeFrameParent->myViewNet->getUndoList());
        }
    }
    // change manually VClass (because it depends of Default VType)
    if (myPersonTypeFrameParent->myPersonTypeSelector->getCurrentPersonType()->getAttribute(SUMO_ATTR_ID) == DEFAULT_VTYPE_ID) {
        myPersonTypeFrameParent->myPersonTypeSelector->getCurrentPersonType()->setAttribute(SUMO_ATTR_VCLASS, toString(SVC_PASSENGER), myPersonTypeFrameParent->myViewNet->getUndoList());
    } else if (myPersonTypeFrameParent->myPersonTypeSelector->getCurrentPersonType()->getAttribute(SUMO_ATTR_ID) == DEFAULT_PEDTYPE_ID) {
        myPersonTypeFrameParent->myPersonTypeSelector->getCurrentPersonType()->setAttribute(SUMO_ATTR_VCLASS, toString(SVC_PEDESTRIAN), myPersonTypeFrameParent->myViewNet->getUndoList());
    } else if (myPersonTypeFrameParent->myPersonTypeSelector->getCurrentPersonType()->getAttribute(SUMO_ATTR_ID) == DEFAULT_BIKETYPE_ID) {
        myPersonTypeFrameParent->myPersonTypeSelector->getCurrentPersonType()->setAttribute(SUMO_ATTR_VCLASS, toString(SVC_BICYCLE), myPersonTypeFrameParent->myViewNet->getUndoList());
    }
    // change special attribute GNE_ATTR_DEFAULT_VTYPE_MODIFIED
    myPersonTypeFrameParent->myPersonTypeSelector->getCurrentPersonType()->setAttribute(GNE_ATTR_DEFAULT_VTYPE_MODIFIED, "false", myPersonTypeFrameParent->myViewNet->getUndoList());
    // finish reset default vehicle type values
    myPersonTypeFrameParent->getViewNet()->getUndoList()->p_end();
    // refresh PersonTypeSelector
    myPersonTypeFrameParent->myPersonTypeSelector->refreshPersonTypeSelector();
    return 1;
}


long
GNEPersonTypeFrame::PersonTypeEditor::onCmdCopyPersonType(FXObject*, FXSelector, void*) {
    // obtain a new valid Vehicle Type ID
    std::string vehicleTypeID = myPersonTypeFrameParent->myViewNet->getNet()->generateDemandElementID("", SUMO_TAG_VTYPE);
    // obtain vehicle type in which new vehicle Type will be based
    GNEVehicleType* vType = dynamic_cast<GNEVehicleType*>(myPersonTypeFrameParent->myPersonTypeSelector->getCurrentPersonType());
    // check that vType exist
    if (vType) {
        // create a new Vehicle Type based on the current selected vehicle type
        GNEDemandElement* personTypeCopy = new GNEVehicleType(myPersonTypeFrameParent->myViewNet, vehicleTypeID, vType);
        // begin undo list operation
        myPersonTypeFrameParent->myViewNet->getUndoList()->p_begin("copy vehicle type");
        // add it using undoList (to allow undo-redo)
        myPersonTypeFrameParent->myViewNet->getUndoList()->add(new GNEChange_DemandElement(personTypeCopy, true), true);
        // end undo list operation
        myPersonTypeFrameParent->myViewNet->getUndoList()->p_end();
        // refresh Vehicle Type Selector (to show the new VType)
        myPersonTypeFrameParent->myPersonTypeSelector->refreshPersonTypeSelector();
        // set created vehicle type in selector
        myPersonTypeFrameParent->myPersonTypeSelector->setCurrentPersonType(personTypeCopy);
        // refresh PersonType Editor Modul
        myPersonTypeFrameParent->myPersonTypeEditor->refreshPersonTypeEditorModul();
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNEPersonTypeFrame - methods
// ---------------------------------------------------------------------------

GNEPersonTypeFrame::GNEPersonTypeFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Vehicle Types") {

    // create modul for edit vehicle types (Create, copy, etc.)
    myPersonTypeEditor = new PersonTypeEditor(this);

    // create vehicle type selector
    myPersonTypeSelector = new PersonTypeSelector(this);

    // Create vehicle type attributes editor
    myPersonTypeAttributesEditor = new AttributesEditor(this);

    // create modul for open extended attributes dialog
    myAttributesEditorExtended = new AttributesEditorExtended(this);

    // set "VTYPE_DEFAULT" as default vehicle Type
    myPersonTypeSelector->setCurrentPersonType(myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, DEFAULT_VTYPE_ID));
}


GNEPersonTypeFrame::~GNEPersonTypeFrame() {}


void
GNEPersonTypeFrame::show() {
    // refresh vehicle type and Attribute Editor
    myPersonTypeSelector->refreshPersonTypeSelector();
    // show vehicle type attributes editor (except extended attributes)
    myPersonTypeAttributesEditor->showAttributeEditorModul({myPersonTypeSelector->getCurrentPersonType()}, false);
    // show frame
    GNEFrame::show();
}


GNEPersonTypeFrame::PersonTypeSelector*
GNEPersonTypeFrame::getPersonTypeSelector() const {
    return myPersonTypeSelector;
}


void
GNEPersonTypeFrame::updateFrameAfterChangeAttribute() {
    myPersonTypeSelector->refreshPersonTypeSelector();
}


void
GNEPersonTypeFrame::openAttributesEditorExtendedDialog() {
    // open vehicle type dialog
    if (myPersonTypeSelector->getCurrentPersonType()) {
        GNEVehicleTypeDialog(myPersonTypeSelector->getCurrentPersonType(), true);
        // call "showAttributeEditorModul" to refresh attribute list
        myPersonTypeAttributesEditor->showAttributeEditorModul({myPersonTypeSelector->getCurrentPersonType()}, false);
    }
}

/****************************************************************************/
