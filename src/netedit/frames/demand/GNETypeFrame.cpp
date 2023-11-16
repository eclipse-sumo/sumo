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
#include <netedit/elements/demand/GNEVType.h>
#include <netedit/dialogs/GNEVehicleTypeDialog.h>
#include <netedit/dialogs/GNEVTypeDistributionsDialog.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNETypeFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNETypeFrame::TypeSelector) typeSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_TYPE,   GNETypeFrame::TypeSelector::onCmdSelectItem)
};

FXDEFMAP(GNETypeFrame::TypeEditor) typeEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATE,    GNETypeFrame::TypeEditor::onCmdCreateType),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_DELETE,    GNETypeFrame::TypeEditor::onCmdDeleteResetType),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_COPY,      GNETypeFrame::TypeEditor::onCmdCopyType)
};

// Object implementation
FXIMPLEMENT(GNETypeFrame::TypeSelector,         MFXGroupBoxModule,  typeSelectorMap,        ARRAYNUMBER(typeSelectorMap))
FXIMPLEMENT(GNETypeFrame::TypeEditor,           MFXGroupBoxModule,  typeEditorMap,          ARRAYNUMBER(typeEditorMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNETypeFrame::TypeSelector - methods
// ---------------------------------------------------------------------------

GNETypeFrame::TypeSelector::TypeSelector(GNETypeFrame* typeFrameParent) :
    MFXGroupBoxModule(typeFrameParent, TL("Current Type")),
    myTypeFrameParent(typeFrameParent),
    myCurrentType(nullptr) {
    // Create MFXComboBoxIcon
    myTypeComboBox = new MFXComboBoxIcon(getCollapsableFrame(), GUIDesignComboBoxNCol, true, GUIDesignComboBoxVisibleItemsLarge,
                                         this, MID_GNE_SET_TYPE, GUIDesignComboBox);
    // add default Types (always first)
    for (const auto& vType : myTypeFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_VTYPE)) {
        if (DEFAULT_VTYPES.count(vType.second->getID()) != 0) {
            myTypeComboBox->appendIconItem(vType.second->getID().c_str(), vType.second->getACIcon(), FXRGB(255, 255, 200));
        }
    }
    // fill myTypeMatchBox with list of VTypes IDs
    for (const auto& vType : myTypeFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_VTYPE)) {
        if (DEFAULT_VTYPES.count(vType.second->getID()) == 0) {
            myTypeComboBox->appendIconItem(vType.second->getID().c_str(), vType.second->getACIcon());
        }
    }
    // set DEFAULT_VEHTYPE as default VType
    myCurrentType = myTypeFrameParent->getViewNet()->getNet()->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, DEFAULT_VTYPE_ID);
    myTypeComboBox->setCurrentItem(myTypeComboBox->findItem(DEFAULT_VTYPE_ID.c_str()));
    // TypeSelector is always shown
    show();
}


GNETypeFrame::TypeSelector::~TypeSelector() {}


GNEDemandElement*
GNETypeFrame::TypeSelector::getCurrentType() const {
    return myCurrentType;
}


void
GNETypeFrame::TypeSelector::setCurrentType(GNEDemandElement* vType) {
    myCurrentType = vType;
    refreshTypeSelector(true);
}


void
GNETypeFrame::TypeSelector::refreshTypeSelector(const bool updateModuls) {
    bool valid = false;
    // clear items
    myTypeComboBox->clearItems();
    // add default Vehicle an Bike types in the first and second positions
    for (const auto& vType : myTypeFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_VTYPE)) {
        if (DEFAULT_VTYPES.count(vType.second->getID()) != 0) {
            myTypeComboBox->appendIconItem(vType.second->getID().c_str(), vType.second->getACIcon(), FXRGB(255, 255, 200));
        }
    }
    // fill myTypeMatchBox with list of VTypes IDs sorted by ID
    std::map<std::string, GNEDemandElement*> types;
    for (const auto& vType : myTypeFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_VTYPE)) {
        if (DEFAULT_VTYPES.count(vType.second->getID()) == 0) {
            types[vType.second->getID()] = vType.second;
        }
    }
    for (const auto& vType : types) {
        myTypeComboBox->appendIconItem(vType.first.c_str(), vType.second->getACIcon());
    }
    // make sure that tag is in myTypeMatchBox
    if (myCurrentType) {
        for (int i = 0; i < (int)myTypeComboBox->getNumItems(); i++) {
            if (myTypeComboBox->getItemText(i) == myCurrentType->getID()) {
                myTypeComboBox->setCurrentItem(i);
                valid = true;
            }
        }
    }
    // Check that give vType type is valid
    if (!valid) {
        // set DEFAULT_VEHTYPE as default VType
        myCurrentType = myTypeFrameParent->getViewNet()->getNet()->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, DEFAULT_VTYPE_ID);
        // refresh myTypeMatchBox again
        for (int i = 0; i < (int)myTypeComboBox->getNumItems(); i++) {
            if (myTypeComboBox->getItemText(i) == myCurrentType->getID()) {
                myTypeComboBox->setCurrentItem(i);
            }
        }
    }
    // check if update other moduls
    if (updateModuls) {
        // refresh vehicle type editor module
        myTypeFrameParent->myTypeEditor->refreshTypeEditorModule();
        // set myCurrentType as inspected element
        myTypeFrameParent->getViewNet()->setInspectedAttributeCarriers({myCurrentType});
        // show modules
        myTypeFrameParent->myTypeAttributesEditor->showAttributeEditorModule(false);
        myTypeFrameParent->myAttributesEditorExtended->showAttributesEditorExtendedModule();
        myTypeFrameParent->myParametersEditor->refreshParametersEditor();
    }
}


long
GNETypeFrame::TypeSelector::onCmdSelectItem(FXObject*, FXSelector, void*) {
    // Check if value of myTypeMatchBox correspond of an allowed additional tags
    for (const auto& vType : myTypeFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_VTYPE)) {
        if (vType.second->getID() == myTypeComboBox->getText().text()) {
            // set pointer
            myCurrentType = vType.second;
            // set color of myTypeMatchBox to black (valid)
            myTypeComboBox->setTextColor(FXRGB(0, 0, 0));
            // refresh vehicle type editor module
            myTypeFrameParent->myTypeEditor->refreshTypeEditorModule();
            // set myCurrentType as inspected element
            myTypeFrameParent->getViewNet()->setInspectedAttributeCarriers({myCurrentType});
            // show modules if selected item is valid
            myTypeFrameParent->myTypeAttributesEditor->showAttributeEditorModule(false);
            myTypeFrameParent->myAttributesEditorExtended->showAttributesEditorExtendedModule();
            myTypeFrameParent->myParametersEditor->refreshParametersEditor();
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected item '" + myTypeComboBox->getText() + "' in TypeSelector").text());
            // update viewNet
            myTypeFrameParent->getViewNet()->updateViewNet();
            return 1;
        }
    }
    myCurrentType = nullptr;
    // refresh vehicle type editor module
    myTypeFrameParent->myTypeEditor->refreshTypeEditorModule();
    // hide all modules if selected item isn't valid
    myTypeFrameParent->myTypeAttributesEditor->hideAttributesEditorModule();
    myTypeFrameParent->myAttributesEditorExtended->hideAttributesEditorExtendedModule();
    // set color of myTypeMatchBox to red (invalid)
    myTypeComboBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Selected invalid item in TypeSelector");
    // update viewNet
    myTypeFrameParent->getViewNet()->updateViewNet();
    return 1;
}

// ---------------------------------------------------------------------------
// GNETypeFrame::TypeEditor - methods
// ---------------------------------------------------------------------------

GNETypeFrame::TypeEditor::TypeEditor(GNETypeFrame* typeFrameParent) :
    MFXGroupBoxModule(typeFrameParent, TL("Type Editor")),
    myTypeFrameParent(typeFrameParent) {
    // Create new vehicle type
    myCreateTypeButton = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Create Type"), "", "", GUIIconSubSys::getIcon(GUIIcon::VTYPE), this, MID_GNE_CREATE, GUIDesignButton);
    // Create delete/reset vehicle type
    myDeleteResetTypeButton = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Delete Type"), "", "", GUIIconSubSys::getIcon(GUIIcon::MODEDELETE), this, MID_GNE_DELETE, GUIDesignButton);
    // Create copy vehicle type
    myCopyTypeButton = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Copy Type"), "", "", GUIIconSubSys::getIcon(GUIIcon::COPY), this, MID_GNE_COPY, GUIDesignButton);
}


GNETypeFrame::TypeEditor::~TypeEditor() {}


void
GNETypeFrame::TypeEditor::showTypeEditorModule() {
    refreshTypeEditorModule();
    show();
}


void
GNETypeFrame::TypeEditor::hideTypeEditorModule() {
    hide();
}


void
GNETypeFrame::TypeEditor::refreshTypeEditorModule() {
    // first check if selected VType is valid
    if (myTypeFrameParent->myTypeSelector->getCurrentType() == nullptr) {
        // disable buttons
        myDeleteResetTypeButton->disable();
        myCopyTypeButton->disable();
    } else if (GNEAttributeCarrier::parse<bool>(myTypeFrameParent->myTypeSelector->getCurrentType()->getAttribute(GNE_ATTR_DEFAULT_VTYPE))) {
        // enable copy button
        myCopyTypeButton->enable();
        // enable and set myDeleteTypeButton as "reset")
        myDeleteResetTypeButton->setText(TL("Reset Type"));
        myDeleteResetTypeButton->setIcon(GUIIconSubSys::getIcon(GUIIcon::RESET));
        // check if reset default vehicle type button has to be enabled or disabled
        if (GNEAttributeCarrier::parse<bool>(myTypeFrameParent->myTypeSelector->getCurrentType()->getAttribute(GNE_ATTR_DEFAULT_VTYPE_MODIFIED))) {
            myDeleteResetTypeButton->enable();
        } else {
            myDeleteResetTypeButton->disable();
        }
    } else {
        // enable copy button
        myCopyTypeButton->enable();
        // enable and set myDeleteTypeButton as "delete")
        myDeleteResetTypeButton->setText(TL("Delete Type"));
        myDeleteResetTypeButton->setIcon(GUIIconSubSys::getIcon(GUIIcon::MODEDELETE));
        myDeleteResetTypeButton->enable();
    }
    // update module
    recalc();
}


long
GNETypeFrame::TypeEditor::onCmdCreateType(FXObject*, FXSelector, void*) {
    // obtain a new valid Type ID
    const std::string typeID = myTypeFrameParent->myViewNet->getNet()->getAttributeCarriers()->generateDemandElementID(SUMO_TAG_VTYPE);
    // create new vehicle type
    GNEDemandElement* type = new GNEVType(myTypeFrameParent->myViewNet->getNet(), typeID);
    // add it using undoList (to allow undo-redo)
    myTypeFrameParent->myViewNet->getUndoList()->begin(type, TL("create vehicle type"));
    myTypeFrameParent->myViewNet->getUndoList()->add(new GNEChange_DemandElement(type, true), true);
    myTypeFrameParent->myViewNet->getUndoList()->end();
    // set created vehicle type in selector
    myTypeFrameParent->myTypeSelector->setCurrentType(type);
    return 1;
}


long
GNETypeFrame::TypeEditor::onCmdDeleteResetType(FXObject*, FXSelector, void*) {
    // continue depending of current mode
    if (myDeleteResetTypeButton->getIcon() == GUIIconSubSys::getIcon(GUIIcon::MODEDELETE)) {
        deleteType();
    } else {
        resetType();
    }
    return 1;
}


long
GNETypeFrame::TypeEditor::onCmdCopyType(FXObject*, FXSelector, void*) {
    // obtain a new valid Type ID
    const std::string typeID = myTypeFrameParent->myViewNet->getNet()->getAttributeCarriers()->generateDemandElementID(SUMO_TAG_VTYPE);
    // obtain vehicle type in which new Type will be based
    GNEVType* vType = dynamic_cast<GNEVType*>(myTypeFrameParent->myTypeSelector->getCurrentType());
    // check that vType exist
    if (vType) {
        // create a new Type based on the current selected vehicle type
        GNEDemandElement* typeCopy = new GNEVType(myTypeFrameParent->myViewNet->getNet(), typeID, vType);
        // begin undo list operation
        myTypeFrameParent->myViewNet->getUndoList()->begin(typeCopy, TL("copy vehicle type"));
        // add it using undoList (to allow undo-redo)
        myTypeFrameParent->myViewNet->getUndoList()->add(new GNEChange_DemandElement(typeCopy, true), true);
        // end undo list operation
        myTypeFrameParent->myViewNet->getUndoList()->end();
        // set created vehicle type in selector
        myTypeFrameParent->myTypeSelector->setCurrentType(typeCopy);
    }
    return 1;
}


void
GNETypeFrame::TypeEditor::resetType() {
    // begin reset default vehicle type values
    myTypeFrameParent->getViewNet()->getUndoList()->begin(GUIIcon::VTYPE, TL("reset default vehicle type values"));
    // reset all values of default vehicle type
    for (const auto& attrProperty : GNEAttributeCarrier::getTagProperty(SUMO_TAG_VTYPE)) {
        // change all attributes with "" to reset it (except ID and vClass)
        if ((attrProperty.getAttr() != SUMO_ATTR_ID) && (attrProperty.getAttr() != SUMO_ATTR_VCLASS)) {
            myTypeFrameParent->myTypeSelector->getCurrentType()->setAttribute(attrProperty.getAttr(), "", myTypeFrameParent->myViewNet->getUndoList());
        }
    }
    // change manually VClass (because it depends of Default VType)
    if (myTypeFrameParent->myTypeSelector->getCurrentType()->getAttribute(SUMO_ATTR_ID) == DEFAULT_VTYPE_ID) {
        myTypeFrameParent->myTypeSelector->getCurrentType()->setAttribute(SUMO_ATTR_VCLASS, toString(SVC_PASSENGER), myTypeFrameParent->myViewNet->getUndoList());
    } else if (myTypeFrameParent->myTypeSelector->getCurrentType()->getAttribute(SUMO_ATTR_ID) == DEFAULT_BIKETYPE_ID) {
        myTypeFrameParent->myTypeSelector->getCurrentType()->setAttribute(SUMO_ATTR_VCLASS, toString(SVC_BICYCLE), myTypeFrameParent->myViewNet->getUndoList());
    } else if (myTypeFrameParent->myTypeSelector->getCurrentType()->getAttribute(SUMO_ATTR_ID) == DEFAULT_TAXITYPE_ID) {
        myTypeFrameParent->myTypeSelector->getCurrentType()->setAttribute(SUMO_ATTR_VCLASS, toString(SVC_TAXI), myTypeFrameParent->myViewNet->getUndoList());
    } else if (myTypeFrameParent->myTypeSelector->getCurrentType()->getAttribute(SUMO_ATTR_ID) == DEFAULT_RAILTYPE_ID) {
        myTypeFrameParent->myTypeSelector->getCurrentType()->setAttribute(SUMO_ATTR_VCLASS, toString(SVC_RAIL), myTypeFrameParent->myViewNet->getUndoList());
    } else if (myTypeFrameParent->myTypeSelector->getCurrentType()->getAttribute(SUMO_ATTR_ID) == DEFAULT_PEDTYPE_ID) {
        myTypeFrameParent->myTypeSelector->getCurrentType()->setAttribute(SUMO_ATTR_VCLASS, toString(SVC_PEDESTRIAN), myTypeFrameParent->myViewNet->getUndoList());
    } else if (myTypeFrameParent->myTypeSelector->getCurrentType()->getAttribute(SUMO_ATTR_ID) == DEFAULT_CONTAINERTYPE_ID) {
        myTypeFrameParent->myTypeSelector->getCurrentType()->setAttribute(SUMO_ATTR_VCLASS, toString(SVC_PEDESTRIAN), myTypeFrameParent->myViewNet->getUndoList());
    }
    // change special attribute GNE_ATTR_DEFAULT_VTYPE_MODIFIED
    myTypeFrameParent->myTypeSelector->getCurrentType()->setAttribute(GNE_ATTR_DEFAULT_VTYPE_MODIFIED, "false", myTypeFrameParent->myViewNet->getUndoList());
    // finish reset default vehicle type values
    myTypeFrameParent->getViewNet()->getUndoList()->end();
    // refresh TypeSelector
    myTypeFrameParent->myTypeSelector->refreshTypeSelector(true);
}


void
GNETypeFrame::TypeEditor::deleteType() {
    // show question dialog if vtype has already assigned vehicles
    if (myTypeFrameParent->myTypeSelector->getCurrentType()->getChildDemandElements().size() > 0) {
        std::string plural = myTypeFrameParent->myTypeSelector->getCurrentType()->getChildDemandElements().size() == 1 ? ("") : ("s");
        // show warning in gui testing debug mode
        WRITE_DEBUG("Opening FXMessageBox 'remove vType'");
        // Ask confirmation to user
        FXuint answer = FXMessageBox::question(getApp(), MBOX_YES_NO,
                                               ("Remove " + toString(SUMO_TAG_VTYPE) + "s").c_str(), "%s",
                                               ("Delete " + toString(SUMO_TAG_VTYPE) + " '" + myTypeFrameParent->myTypeSelector->getCurrentType()->getID() +
                                                "' will remove " + toString(myTypeFrameParent->myTypeSelector->getCurrentType()->getChildDemandElements().size()) +
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
            myTypeFrameParent->myViewNet->getUndoList()->begin(myTypeFrameParent->myTypeSelector->getCurrentType(), ("delete vehicle type"));
            // remove vehicle type (and all of their children)
            myTypeFrameParent->myViewNet->getNet()->deleteDemandElement(myTypeFrameParent->myTypeSelector->getCurrentType(),
                    myTypeFrameParent->myViewNet->getUndoList());
            // end undo list operation
            myTypeFrameParent->myViewNet->getUndoList()->end();
        }
    } else {
        // begin undo list operation
        myTypeFrameParent->myViewNet->getUndoList()->begin(myTypeFrameParent->myTypeSelector->getCurrentType(), ("delete vehicle type"));
        // remove vehicle type (and all of their children)
        myTypeFrameParent->myViewNet->getNet()->deleteDemandElement(myTypeFrameParent->myTypeSelector->getCurrentType(),
                myTypeFrameParent->myViewNet->getUndoList());
        // end undo list operation
        myTypeFrameParent->myViewNet->getUndoList()->end();
    }
}

// ---------------------------------------------------------------------------
// GNETypeFrame - methods
// ---------------------------------------------------------------------------

GNETypeFrame::GNETypeFrame(GNEViewParent* viewParent, GNEViewNet* viewNet) :
    GNEFrame(viewParent, viewNet, TL("Types")) {

    // create module for edit vehicle types (Create, copy, etc.)
    myTypeEditor = new TypeEditor(this);

    // create vehicle type selector
    myTypeSelector = new TypeSelector(this);

    // Create vehicle type attributes editor
    myTypeAttributesEditor = new GNEFrameAttributeModules::AttributesEditor(this);

    // create module for open extended attributes dialog
    myAttributesEditorExtended = new GNEFrameAttributeModules::AttributesEditorExtended(this);

    /// create module for edit parameters
    myParametersEditor = new GNEFrameAttributeModules::ParametersEditor(this);

    // set "VTYPE_DEFAULT" as default vehicle Type
    myTypeSelector->setCurrentType(myViewNet->getNet()->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, DEFAULT_VTYPE_ID));
}


GNETypeFrame::~GNETypeFrame() {}


void
GNETypeFrame::show() {
    // refresh vehicle type and Attribute Editor
    myTypeSelector->refreshTypeSelector(true);
    // set myCurrentType as inspected element
    myTypeAttributesEditor->getFrameParent()->getViewNet()->setInspectedAttributeCarriers({myTypeSelector->getCurrentType()});
    // show modules
    myTypeAttributesEditor->showAttributeEditorModule(false);
    myAttributesEditorExtended->showAttributesEditorExtendedModule();
    // show frame
    GNEFrame::show();
}


GNETypeFrame::TypeSelector*
GNETypeFrame::getTypeSelector() const {
    return myTypeSelector;
}


void
GNETypeFrame::attributeUpdated(SumoXMLAttr /*attribute*/) {
    // after changing an attribute myTypeSelector, we need to update the list of typeSelector, because ID could be changed
    myTypeSelector->refreshTypeSelector(false);
    //... and typeEditor (due reset)
    myTypeEditor->refreshTypeEditorModule();
}


void
GNETypeFrame::attributesEditorExtendedDialogOpened() {
    // open vehicle type dialog
    if (myTypeSelector->getCurrentType()) {
        GNEVehicleTypeDialog(myTypeSelector->getCurrentType(), true);  // NOSONAR, constructor returns after dialog has been closed
        // set myCurrentType as inspected element
        myTypeAttributesEditor->getFrameParent()->getViewNet()->setInspectedAttributeCarriers({myTypeSelector->getCurrentType()});
        // call "showAttributeEditorModule" to refresh attribute list
        myTypeAttributesEditor->showAttributeEditorModule(false);
        myParametersEditor->refreshParametersEditor();
    }
}


/****************************************************************************/
