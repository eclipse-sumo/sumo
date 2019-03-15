/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEVehicleTypeFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2018
/// @version $Id$
///
// The Widget for edit Vehicle Type elements
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIDesigns.h>
#include <netedit/changes/GNEChange_DemandElement.h>
#include <netedit/dialogs/GNEVehicleTypeDialog.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/netelements/GNEConnection.h>
#include <netedit/demandelements/GNEVehicle.h>
#include <netedit/demandelements/GNEVehicleType.h>
#include <netedit/demandelements/GNERouteHandler.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>

#include "GNEVehicleTypeFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEVehicleTypeFrame::VehicleTypeSelector) vehicleTypeSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_TYPE,   GNEVehicleTypeFrame::VehicleTypeSelector::onCmdSelectItem)
};

FXDEFMAP(GNEVehicleTypeFrame::VehicleTypeEditor) vehicleTypeEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_VEHICLETYPEFRAME_CREATE,    GNEVehicleTypeFrame::VehicleTypeEditor::onCmdCreateVehicleType),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_VEHICLETYPEFRAME_DELETE,    GNEVehicleTypeFrame::VehicleTypeEditor::onCmdDeleteVehicleType),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_VEHICLETYPEFRAME_RESET,     GNEVehicleTypeFrame::VehicleTypeEditor::onCmdResetVehicleType),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_VEHICLETYPEFRAME_COPY,      GNEVehicleTypeFrame::VehicleTypeEditor::onCmdCopyVehicleType)
};

// Object implementation
FXIMPLEMENT(GNEVehicleTypeFrame::VehicleTypeSelector,   FXGroupBox,     vehicleTypeSelectorMap,     ARRAYNUMBER(vehicleTypeSelectorMap))
FXIMPLEMENT(GNEVehicleTypeFrame::VehicleTypeEditor,     FXGroupBox,     vehicleTypeEditorMap,       ARRAYNUMBER(vehicleTypeEditorMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEVehicleTypeFrame::VehicleTypeSelector - methods
// ---------------------------------------------------------------------------

GNEVehicleTypeFrame::VehicleTypeSelector::VehicleTypeSelector(GNEVehicleTypeFrame* vehicleTypeFrameParent) :
    FXGroupBox(vehicleTypeFrameParent->myContentFrame, "Current Vehicle Type", GUIDesignGroupBoxFrame),
    myVehicleTypeFrameParent(vehicleTypeFrameParent) {
    // Create FXComboBox
    myTypeMatchBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SET_TYPE, GUIDesignComboBox);
    // fill myTypeMatchBox with list of VTypes IDs
    for (const auto& i : myVehicleTypeFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_VTYPE)) {
        myTypeMatchBox->appendItem(i.first.c_str());
    }
    // Set visible items
    myTypeMatchBox->setNumVisible((int)myTypeMatchBox->getNumItems());
    // VehicleTypeSelector is always shown
    show();
}


GNEVehicleTypeFrame::VehicleTypeSelector::~VehicleTypeSelector() {}


GNEDemandElement*
GNEVehicleTypeFrame::VehicleTypeSelector::getCurrentVehicleType() const {
    // obtain current VType ID (To improve code legibly)
    std::string vTypeID = myTypeMatchBox->getItem(myTypeMatchBox->getCurrentItem()).text();
    // check if ID of myTypeMatchBox is a valid ID
    if (myVehicleTypeFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_VTYPE).count(vTypeID) == 1) {
        return myVehicleTypeFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_VTYPE).at(vTypeID);
    } else {
        return nullptr;
    }
}


void
GNEVehicleTypeFrame::VehicleTypeSelector::setCurrentVehicleType(GNEDemandElement *vType) {
    bool valid = false;
    // make sure that tag is in myTypeMatchBox
    for (int i = 0; i < (int)myTypeMatchBox->getNumItems(); i++) {
        if (myTypeMatchBox->getItem(i).text() == vType->getID()) {
            myTypeMatchBox->setCurrentItem(i);
            valid = true;
        }
    }
    // Check that give vType type is valid
    if (valid) {
        // show moduls if selected item is valid
        myVehicleTypeFrameParent->enableModuls(vType);
    } else {
        // hide all moduls if selected item isn't valid
        myVehicleTypeFrameParent->disableModuls();
    }
}


void 
GNEVehicleTypeFrame::VehicleTypeSelector::refreshVehicleTypeSelector() {
    // first clear items
    myTypeMatchBox->clearItems();
    // fill myTypeMatchBox with list of VTypes IDs
    for (const auto& i : myVehicleTypeFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_VTYPE)) {
        myTypeMatchBox->appendItem(i.first.c_str());
    }
    // Set visible items
    myTypeMatchBox->setNumVisible((int)myTypeMatchBox->getNumItems());
    // refresh vehicle type editor
    myVehicleTypeFrameParent->myVehicleTypeEditor->refreshVehicleTypeEditorModul();
}


long
GNEVehicleTypeFrame::VehicleTypeSelector::onCmdSelectItem(FXObject*, FXSelector, void*) {
    // Check if value of myTypeMatchBox correspond of an allowed additional tags
    for (const auto& i : myVehicleTypeFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_VTYPE)) {
        if (i.first == myTypeMatchBox->getText().text()) {
            // set color of myTypeMatchBox to black (valid)
            myTypeMatchBox->setTextColor(FXRGB(0, 0, 0));
            // show moduls if selected item is valid
            myVehicleTypeFrameParent->enableModuls(i.second);
            // refresh vehicle type editor
            myVehicleTypeFrameParent->myVehicleTypeEditor->refreshVehicleTypeEditorModul();
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected item '" + myTypeMatchBox->getText() + "' in VehicleTypeSelector").text());
            return 1;
        }
    }
    // hide all moduls if selected item isn't valid
    myVehicleTypeFrameParent->disableModuls();
    // set color of myTypeMatchBox to red (invalid)
    myTypeMatchBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Selected invalid item in VehicleTypeSelector");
    return 1;
}

// ---------------------------------------------------------------------------
// GNEVehicleTypeFrame::VehicleTypeEditor - methods
// ---------------------------------------------------------------------------

GNEVehicleTypeFrame::VehicleTypeEditor::VehicleTypeEditor(GNEVehicleTypeFrame* vehicleTypeFrameParent) :
    FXGroupBox(vehicleTypeFrameParent->myContentFrame, "Vehicle Type Editor", GUIDesignGroupBoxFrame),
    myVehicleTypeFrameParent(vehicleTypeFrameParent) {
    // Create new vehicle type
    myCreateVehicleTypeButton = new FXButton(this, "Create Vehicle Type", nullptr, this, MID_GNE_VEHICLETYPEFRAME_CREATE, GUIDesignButton);
    // Create delete vehicle type
    myDeleteVehicleTypeButton = new FXButton(this, "Delete Vehicle Type", nullptr, this, MID_GNE_VEHICLETYPEFRAME_DELETE, GUIDesignButton);
    // Create reset vehicle type
    myResetDefaultVehicleTypeButton = new FXButton(this, "Reset default Vehicle Type", nullptr, this, MID_GNE_VEHICLETYPEFRAME_RESET, GUIDesignButton);
    // Create copy vehicle type
    myCopyVehicleTypeButton = new FXButton(this, "Copy Vehicle Type", nullptr, this, MID_GNE_VEHICLETYPEFRAME_COPY, GUIDesignButton);
}


GNEVehicleTypeFrame::VehicleTypeEditor::~VehicleTypeEditor() {}


void 
GNEVehicleTypeFrame::VehicleTypeEditor::showVehicleTypeEditorModul() {
    refreshVehicleTypeEditorModul();
    show();
}


void 
GNEVehicleTypeFrame::VehicleTypeEditor::hideVehicleTypeEditorModul() {
    hide();
}


void 
GNEVehicleTypeFrame::VehicleTypeEditor::refreshVehicleTypeEditorModul() {
    // disable delete button if DEFAULT_VTYPE_ID is selected
    if ((myVehicleTypeFrameParent->myVehicleTypeSelector->getCurrentVehicleType()->getID() == DEFAULT_VTYPE_ID) ||
        (myVehicleTypeFrameParent->myVehicleTypeSelector->getCurrentVehicleType()->getID() == DEFAULT_PEDTYPE_ID) ||
        (myVehicleTypeFrameParent->myVehicleTypeSelector->getCurrentVehicleType()->getID() == DEFAULT_BIKETYPE_ID)) {
        // hide delete vehicle type buttond and show reset default vehicle type button
        myDeleteVehicleTypeButton->hide();
        myResetDefaultVehicleTypeButton->show();
        // check if reset default vehicle type button has to be enabled or disabled
        if (GNEAttributeCarrier::parse<bool>(myVehicleTypeFrameParent->myVehicleTypeSelector->getCurrentVehicleType()->getAttribute(GNE_ATTR_DEFAULT_VTYPE_MODIFIED))) {
            myResetDefaultVehicleTypeButton->enable();
        } else {
            myResetDefaultVehicleTypeButton->disable();
        }
    } else {
        // show delete vehicle type buttond and hide reset default vehicle type button
        myDeleteVehicleTypeButton->show();
        myResetDefaultVehicleTypeButton->hide();
    }
    // update modul
    recalc();
}


long 
GNEVehicleTypeFrame::VehicleTypeEditor::onCmdCreateVehicleType(FXObject*, FXSelector, void*) {
    // obtain a new valid Vehicle Type ID
    std::string vehicleTypeID = myVehicleTypeFrameParent->myViewNet->getNet()->generateDemandElementID(SUMO_TAG_VTYPE);
    // create new vehicle type
    GNEDemandElement *vehicleType = new GNEVehicleType(myVehicleTypeFrameParent->myViewNet, vehicleTypeID);
    // add it using undoList (to allow undo-redo)
    myVehicleTypeFrameParent->myViewNet->getUndoList()->add(new GNEChange_DemandElement(vehicleType, true), true);
    // refresh Vehicle Type Selector (to show the new VType)
    myVehicleTypeFrameParent->myVehicleTypeSelector->refreshVehicleTypeSelector();
    // set created vehicle type in selector
    myVehicleTypeFrameParent->myVehicleTypeSelector->setCurrentVehicleType(vehicleType);
    // refresh VehicleType Editor Modul
    myVehicleTypeFrameParent->myVehicleTypeEditor->refreshVehicleTypeEditorModul();
    return 1;
}


long 
GNEVehicleTypeFrame::VehicleTypeEditor::onCmdDeleteVehicleType(FXObject*, FXSelector, void*) {
    // show question dialog if vtype has already assigned vehicles
    if (myVehicleTypeFrameParent->myVehicleTypeSelector->getCurrentVehicleType()->getDemandElementChilds().size() > 0) {
        std::string plural = myVehicleTypeFrameParent->myVehicleTypeSelector->getCurrentVehicleType()->getDemandElementChilds().size() == 1 ? ("") : ("s");
        // show warning in gui testing debug mode
        WRITE_DEBUG("Opening FXMessageBox 'remove vType'");
        // Ask confirmation to user
        FXuint answer = FXMessageBox::question(getApp(), MBOX_YES_NO,
                                                ("Remove " + toString(SUMO_TAG_VTYPE) + "s").c_str(), "%s",
                                                ("Delete " + toString(SUMO_TAG_VTYPE) + " '" + myVehicleTypeFrameParent->myVehicleTypeSelector->getCurrentVehicleType()->getID() + 
                                                 "' will remove " + toString(myVehicleTypeFrameParent->myVehicleTypeSelector->getCurrentVehicleType()->getDemandElementChilds().size()) + 
                                                 " vehicle" + plural + ". Continue?").c_str());
        if (answer != 1) { // 1:yes, 2:no, 4:esc
            // write warning if netedit is running in testing mode
            if (answer == 2) {
                WRITE_DEBUG("Closed FXMessageBox 'remove vType' with 'No'");
            } else if (answer == 4) {
                WRITE_DEBUG("Closed FXMessageBox 'remove vType' with 'ESC'");
            }
        } else {
            // remove vehicle type (and all of their childs)
            myVehicleTypeFrameParent->myViewNet->getNet()->deleteDemandElement(myVehicleTypeFrameParent->myVehicleTypeSelector->getCurrentVehicleType(),
                                                                               myVehicleTypeFrameParent->myViewNet->getUndoList());
        }
    } else {
        // remove vehicle type (and all of their childs)
        myVehicleTypeFrameParent->myViewNet->getNet()->deleteDemandElement(myVehicleTypeFrameParent->myVehicleTypeSelector->getCurrentVehicleType(),
                                                                           myVehicleTypeFrameParent->myViewNet->getUndoList());
    }
    return 1;
}


long 
GNEVehicleTypeFrame::VehicleTypeEditor::onCmdResetVehicleType(FXObject*, FXSelector, void*) {
    // begin reset default vehicle type values
    myVehicleTypeFrameParent->getViewNet()->getUndoList()->p_begin("reset default vehicle type values");
    // reset all values of default vehicle type
    for (const auto &i : GNEAttributeCarrier::getTagProperties(SUMO_TAG_VTYPE)) {
        // change all attributes with "" to reset it (except ID and vClass)
        if ((i.first != SUMO_ATTR_ID) && (i.first != SUMO_ATTR_VCLASS)) {
            myVehicleTypeFrameParent->myVehicleTypeSelector->getCurrentVehicleType()->setAttribute(i.first, "", myVehicleTypeFrameParent->myViewNet->getUndoList());
        }
    }
    // change special attribute GNE_ATTR_DEFAULT_VTYPE_MODIFIED
    myVehicleTypeFrameParent->myVehicleTypeSelector->getCurrentVehicleType()->setAttribute(GNE_ATTR_DEFAULT_VTYPE_MODIFIED, "false", myVehicleTypeFrameParent->myViewNet->getUndoList());
    // finish reset default vehicle type values
    myVehicleTypeFrameParent->getViewNet()->getUndoList()->p_end();
    // call "showAttributeEditorModul" to refresh attribute list
    myVehicleTypeFrameParent->myVehicleTypeAttributesEditor->showAttributeEditorModul({myVehicleTypeFrameParent->myVehicleTypeSelector->getCurrentVehicleType()}, false);
    return 1;
}


long 
GNEVehicleTypeFrame::VehicleTypeEditor::onCmdCopyVehicleType(FXObject*, FXSelector, void*) {
    // obtain a new valid Vehicle Type ID
    std::string vehicleTypeID = myVehicleTypeFrameParent->myViewNet->getNet()->generateDemandElementID(SUMO_TAG_VTYPE);
    // obtain vehicle type in which new Vehicle Type will be based
    GNEVehicleType *vType = dynamic_cast<GNEVehicleType*>(myVehicleTypeFrameParent->myVehicleTypeSelector->getCurrentVehicleType());
    // check that vType exist
    if (vType) {
        // create a new Vehicle Type based on the current selected vehicle type
        GNEDemandElement *vehicleTypeCopy = new GNEVehicleType(myVehicleTypeFrameParent->myViewNet, vehicleTypeID, vType);
        // add it using undoList (to allow undo-redo)
        myVehicleTypeFrameParent->myViewNet->getUndoList()->add(new GNEChange_DemandElement(vehicleTypeCopy, true), true);
        // refresh Vehicle Type Selector (to show the new VType)
        myVehicleTypeFrameParent->myVehicleTypeSelector->refreshVehicleTypeSelector();
        // set created vehicle type in selector
        myVehicleTypeFrameParent->myVehicleTypeSelector->setCurrentVehicleType(vehicleTypeCopy);
        // refresh VehicleType Editor Modul
        myVehicleTypeFrameParent->myVehicleTypeEditor->refreshVehicleTypeEditorModul();
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNEVehicleTypeFrame - methods
// ---------------------------------------------------------------------------

GNEVehicleTypeFrame::GNEVehicleTypeFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Vehicle Types") {

    // create modul for edit vehicle types (Create, copy, etc.)
    myVehicleTypeEditor = new VehicleTypeEditor(this);

    // create vehicle type selector
    myVehicleTypeSelector = new VehicleTypeSelector(this);

    // Create vehicle type attributes editor
    myVehicleTypeAttributesEditor = new AttributesEditor(this);

    // create modul for open extended attributes dialog
    myAttributesEditorExtended = new AttributesEditorExtended(this);

    // set "VTYPE_DEFAULT" as default vehicle Type
    myVehicleTypeSelector->setCurrentVehicleType(myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, DEFAULT_VTYPE_ID));
}


GNEVehicleTypeFrame::~GNEVehicleTypeFrame() {}


void
GNEVehicleTypeFrame::show() {
    // refresh vehicle type and Attribute Editor
    myVehicleTypeSelector->refreshVehicleTypeSelector();
    // show vehicle type attributes editor (except extended attributes)
    myVehicleTypeAttributesEditor->showAttributeEditorModul({myVehicleTypeSelector->getCurrentVehicleType()}, false);
    // show frame
    GNEFrame::show();
}


GNEVehicleTypeFrame::VehicleTypeSelector* 
GNEVehicleTypeFrame::getVehicleTypeSelector() const {
    return myVehicleTypeSelector;
}


void
GNEVehicleTypeFrame::enableModuls(GNEDemandElement *vType) {
    // show vehicle type attributes editor (except extended attributes)
    myVehicleTypeAttributesEditor->showAttributeEditorModul({vType}, false);
}


void
GNEVehicleTypeFrame::disableModuls() {
    // hide attributes editor
    myVehicleTypeAttributesEditor->hideAttributesEditorModul();
}


void
GNEVehicleTypeFrame::updateFrameAfterChangeAttribute() {
    myVehicleTypeSelector->refreshVehicleTypeSelector();
}


void 
GNEVehicleTypeFrame::openAttributesEditorExtendedDialog() {
    // open vehicle type dialog
    if (myVehicleTypeSelector->getCurrentVehicleType()) {
        GNEVehicleTypeDialog(myVehicleTypeSelector->getCurrentVehicleType(), true);
        // call "showAttributeEditorModul" to refresh attribute list
        myVehicleTypeAttributesEditor->showAttributeEditorModul({myVehicleTypeSelector->getCurrentVehicleType()}, false);
    }
}

/****************************************************************************/
