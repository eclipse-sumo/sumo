/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
/// @file    GNEMeanDataFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2022
///
// The Widget for edit meanData elements
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/changes/GNEChange_MeanData.h>
#include <netedit/elements/data/GNEMeanData.h>
#include <netedit/elements/data/GNEDataHandler.h>
#include <netedit/elements/data/GNEDataInterval.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEMeanDataFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEMeanDataFrame::MeanDataTypeSelector) typeSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_TYPE,   GNEMeanDataFrame::MeanDataTypeSelector::onCmdSelectItem)
};

FXDEFMAP(GNEMeanDataFrame::MeanDataEditor) typeEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATE,    GNEMeanDataFrame::MeanDataEditor::onCmdCreateType),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_DELETE,    GNEMeanDataFrame::MeanDataEditor::onCmdDeleteResetType),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_COPY,      GNEMeanDataFrame::MeanDataEditor::onCmdCopyType)
};

// Object implementation
FXIMPLEMENT(GNEMeanDataFrame::MeanDataTypeSelector,     MFXGroupBoxModule,  typeSelectorMap,    ARRAYNUMBER(typeSelectorMap))
FXIMPLEMENT(GNEMeanDataFrame::MeanDataEditor,           MFXGroupBoxModule,  typeEditorMap,      ARRAYNUMBER(typeEditorMap))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEMeanDataFrame::MeanDataTypeSelector - methods
// ---------------------------------------------------------------------------

GNEMeanDataFrame::MeanDataTypeSelector::MeanDataTypeSelector(GNEMeanDataFrame* meanDataFrameParent) :
    MFXGroupBoxModule(meanDataFrameParent, TL("Current MeanData")),
    myMeanDataFrameParent(meanDataFrameParent) {
    // Create FXComboBox
    myTypeComboBox = new MFXIconComboBox(getCollapsableFrame(), GUIDesignComboBoxNCol, true, this, MID_GNE_SET_TYPE, GUIDesignComboBox);
    // add mean data types
    const auto meanDataTypes = GNEAttributeCarrier::getTagPropertiesByType(GNETagProperties::MEANDATA);
    for (const auto& meanDataType : meanDataTypes) {
        myTypeComboBox->appendIconItem(meanDataType.getTagStr().c_str(), GUIIconSubSys::getIcon(meanDataType.getGUIIcon()));
    }
    // set DEFAULT_VEHTYPE as default VType
    myCurrentMeanData = meanDataTypes.front();
    // Set visible items
    myTypeComboBox->setNumVisible((int)myTypeComboBox->getNumItems());
    // MeanDataTypeSelector is always shown
    show();
}


GNEMeanDataFrame::MeanDataTypeSelector::~MeanDataTypeSelector() {}


const GNETagProperties&
GNEMeanDataFrame::MeanDataTypeSelector::getCurrentMeanData() const {
    return myCurrentMeanData;
}


void
GNEMeanDataFrame::MeanDataTypeSelector::refreshMeanDataTypeSelector() {
    // clear items
    myTypeComboBox->clearItems();
    // add mean data types
    const auto meanDataTypes = GNEAttributeCarrier::getTagPropertiesByType(GNETagProperties::MEANDATA);
    for (const auto& meanDataType : meanDataTypes) {
        myTypeComboBox->appendIconItem(meanDataType.getTagStr().c_str(), GUIIconSubSys::getIcon(meanDataType.getGUIIcon()));
    }
    // make sure that tag is in myTypeMatchBox
    if (myCurrentMeanData.getTagStr() != myInvalidMeanData.getTagStr()) {
        for (int i = 0; i < (int)myTypeComboBox->getNumItems(); i++) {
            if (myTypeComboBox->getItem(i).text() == myCurrentMeanData.getTagStr()) {
                myTypeComboBox->setCurrentItem(i);            
            }
        }
    } else {
        myCurrentMeanData = meanDataTypes.front();
    }
    // refresh meanData editor module
    myMeanDataFrameParent->myMeanDataEditor->refreshMeanDataEditorModule();
    // show modules
/*
    myMeanDataFrameParent->myTypeAttributesEditor->showAttributeEditorModule(false, true);
    myMeanDataFrameParent->myAttributesEditorExtended->showAttributesEditorExtendedModule();
    myMeanDataFrameParent->myVTypeDistributions->showVTypeDistributionsModule();
*/
}


long
GNEMeanDataFrame::MeanDataTypeSelector::onCmdSelectItem(FXObject*, FXSelector, void*) {
    // add mean data types
    const auto meanDataTypes = GNEAttributeCarrier::getTagPropertiesByType(GNETagProperties::MEANDATA);
    // Check if value of myTypeMatchBox correspond of an allowed additional tags
    for (const auto& meanDataType : meanDataTypes) {
        if (meanDataType.getTagStr() == myTypeComboBox->getText().text()) {
            // set pointer
            myCurrentMeanData = meanDataType;
            // set color of myTypeMatchBox to black (valid)
            myTypeComboBox->setTextColor(FXRGB(0, 0, 0));
            // refresh meanData editor module
            myMeanDataFrameParent->myMeanDataEditor->refreshMeanDataEditorModule();
            // show modules if selected item is valid
/*
            myMeanDataFrameParent->myTypeAttributesEditor->showAttributeEditorModule(false, true);
            myMeanDataFrameParent->myAttributesEditorExtended->showAttributesEditorExtendedModule();
            myMeanDataFrameParent->myVTypeDistributions->showVTypeDistributionsModule();
*/
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected item '" + myTypeComboBox->getText() + "' in MeanDataTypeSelector").text());
            return 1;
        }
    }
    myCurrentMeanData = myInvalidMeanData;
    // refresh meanData editor module
    myMeanDataFrameParent->myMeanDataEditor->refreshMeanDataEditorModule();
    // hide all modules if selected item isn't valid
/*
    myMeanDataFrameParent->myTypeAttributesEditor->hideAttributesEditorModule();
    myMeanDataFrameParent->myAttributesEditorExtended->hideAttributesEditorExtendedModule();
    myMeanDataFrameParent->myVTypeDistributions->hideVTypeDistributionsModule();
*/
    // set color of myTypeMatchBox to red (invalid)
    myTypeComboBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Selected invalid item in MeanDataTypeSelector");
    return 1;
}

// ---------------------------------------------------------------------------
// GNEMeanDataFrame::MeanDataEditor - methods
// ---------------------------------------------------------------------------

GNEMeanDataFrame::MeanDataEditor::MeanDataEditor(GNEMeanDataFrame* meanDataFrameParent) :
    MFXGroupBoxModule(meanDataFrameParent, TL("MeanData Editor")),
    myMeanDataFrameParent(meanDataFrameParent) {
    // Create new meanData
    myCreateTypeButton = new FXButton(getCollapsableFrame(), TL("Create MeanData"), GUIIconSubSys::getIcon(GUIIcon::MEANDATAEDGE), this, MID_GNE_CREATE, GUIDesignButton);
    // Create delete/reset meanData
    myDeleteResetTypeButton = new FXButton(getCollapsableFrame(), TL("Delete MeanData"), GUIIconSubSys::getIcon(GUIIcon::MODEDELETE), this, MID_GNE_DELETE, GUIDesignButton);
    // Create copy meanData
    myCopyTypeButton = new FXButton(getCollapsableFrame(), TL("Copy MeanData"), GUIIconSubSys::getIcon(GUIIcon::COPY), this, MID_GNE_COPY, GUIDesignButton);
}


GNEMeanDataFrame::MeanDataEditor::~MeanDataEditor() {}


void
GNEMeanDataFrame::MeanDataEditor::showMeanDataEditorModule() {
    refreshMeanDataEditorModule();
    show();
}


void
GNEMeanDataFrame::MeanDataEditor::hideMeanDataEditorModule() {
    hide();
}


void
GNEMeanDataFrame::MeanDataEditor::refreshMeanDataEditorModule() {
/*
    // first check if selected VType is valid
    if (myMeanDataFrameParent->myMeanDataTypeSelector->getCurrentMeanData() == nullptr) {
        // disable buttons
        myDeleteResetTypeButton->disable();
        myCopyTypeButton->disable();
    } else if (GNEAttributeCarrier::parse<bool>(myMeanDataFrameParent->myMeanDataTypeSelector->getCurrentMeanData()->getAttribute(GNE_ATTR_DEFAULT_VTYPE))) {
        // enable copy button
        myCopyTypeButton->enable();
        // enable and set myDeleteTypeButton as "reset")
        myDeleteResetTypeButton->setText(TL("Reset Type"));
        myDeleteResetTypeButton->setIcon(GUIIconSubSys::getIcon(GUIIcon::RESET));
        // check if reset default meanData button has to be enabled or disabled
        if (GNEAttributeCarrier::parse<bool>(myMeanDataFrameParent->myMeanDataTypeSelector->getCurrentMeanData()->getAttribute(GNE_ATTR_DEFAULT_VTYPE_MODIFIED))) {
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
*/
}


long
GNEMeanDataFrame::MeanDataEditor::onCmdCreateType(FXObject*, FXSelector, void*) {
/*
    // obtain a new valid Type ID
    const std::string typeID = myMeanDataFrameParent->myViewNet->getNet()->getAttributeCarriers()->generateMeanDataID(SUMO_TAG_VTYPE);
    // create new meanData
    GNEMeanData* meanData = new GNEMeanData(myMeanDataFrameParent->myViewNet->getNet(), typeID);
    // add it using undoList (to allow undo-redo)
    myMeanDataFrameParent->myViewNet->getUndoList()->begin(GUIIcon::VTYPE, "create meanData");
    myMeanDataFrameParent->myViewNet->getUndoList()->add(new GNEChange_MeanData(meanData, true), true);
    myMeanDataFrameParent->myViewNet->getUndoList()->end();
    // set created meanData in selector
    myMeanDataFrameParent->myMeanDataTypeSelector->setCurrentType(meanData);
    // refresh Type Editor Module
    myMeanDataFrameParent->myMeanDataEditor->refreshMeanDataEditorModule();
*/
    return 1;
}


long
GNEMeanDataFrame::MeanDataEditor::onCmdDeleteResetType(FXObject*, FXSelector, void*) {
    // continue depending of current mode
    if (myDeleteResetTypeButton->getIcon() == GUIIconSubSys::getIcon(GUIIcon::MODEDELETE)) {
        deleteType();
    } else {
        resetType();
    }
    return 1;
}


long
GNEMeanDataFrame::MeanDataEditor::onCmdCopyType(FXObject*, FXSelector, void*) {
/*
    // obtain a new valid Type ID
    const std::string typeID = myMeanDataFrameParent->myViewNet->getNet()->getAttributeCarriers()->generateMeanDataID(SUMO_TAG_VTYPE);
    // obtain meanData in which new Type will be based
    GNEMeanData* meanData = dynamic_cast<GNEMeanData*>(myMeanDataFrameParent->myMeanDataTypeSelector->getCurrentMeanData());
    // check that meanData exist
    if (meanData) {
        // create a new Type based on the current selected meanData
        GNEMeanData* typeCopy = new GNEMeanData(myMeanDataFrameParent->myViewNet->getNet(), typeID, meanData);
        // begin undo list operation
        myMeanDataFrameParent->myViewNet->getUndoList()->begin(GUIIcon::VTYPE, "copy meanData");
        // add it using undoList (to allow undo-redo)
        myMeanDataFrameParent->myViewNet->getUndoList()->add(new GNEChange_MeanData(typeCopy, true), true);
        // end undo list operation
        myMeanDataFrameParent->myViewNet->getUndoList()->end();
        // refresh Type Selector (to show the new VType)
        myMeanDataFrameParent->myMeanDataTypeSelector->refreshTypeSelector();
        // set created meanData in selector
        myMeanDataFrameParent->myMeanDataTypeSelector->setCurrentType(typeCopy);
        // refresh Type Editor Module
        myMeanDataFrameParent->myMeanDataEditor->refreshMeanDataEditorModule();
    }
*/
    return 1;
}


void
GNEMeanDataFrame::MeanDataEditor::resetType() {
/*
    // begin reset default meanData values
    myMeanDataFrameParent->getViewNet()->getUndoList()->begin(GUIIcon::VTYPE, "reset default meanData values");
    // reset all values of default meanData
    for (const auto& attrProperty : GNEAttributeCarrier::getTagProperty(SUMO_TAG_VTYPE)) {
        // change all attributes with "" to reset it (except ID and vClass)
        if ((attrProperty.getAttr() != SUMO_ATTR_ID) && (attrProperty.getAttr() != SUMO_ATTR_VCLASS)) {
            myMeanDataFrameParent->myMeanDataTypeSelector->getCurrentMeanData()->setAttribute(attrProperty.getAttr(), "", myMeanDataFrameParent->myViewNet->getUndoList());
        }
    }
    // change special attribute GNE_ATTR_DEFAULT_VTYPE_MODIFIED
    myMeanDataFrameParent->myMeanDataTypeSelector->getCurrentMeanData()->setAttribute(GNE_ATTR_DEFAULT_VTYPE_MODIFIED, "false", myMeanDataFrameParent->myViewNet->getUndoList());
    // finish reset default meanData values
    myMeanDataFrameParent->getViewNet()->getUndoList()->end();
    // refresh TypeSelector
    myMeanDataFrameParent->myMeanDataTypeSelector->refreshTypeSelector();
*/
}


void
GNEMeanDataFrame::MeanDataEditor::deleteType() {
/*
    // begin undo list operation
    myMeanDataFrameParent->myViewNet->getUndoList()->begin(GUIIcon::VTYPE, "delete meanData");
    // remove meanData (and all of their children)
    myMeanDataFrameParent->myViewNet->getNet()->deleteMeanData(myMeanDataFrameParent->myMeanDataTypeSelector->getCurrentMeanData(),
            myMeanDataFrameParent->myViewNet->getUndoList());
    // end undo list operation
    myMeanDataFrameParent->myViewNet->getUndoList()->end();
*/
}

// ---------------------------------------------------------------------------
// GNEMeanDataFrame - methods
// ---------------------------------------------------------------------------

GNEMeanDataFrame::GNEMeanDataFrame(GNEViewParent* viewParent, GNEViewNet* viewNet) :
    GNEFrame(viewParent, viewNet, "Meandata") {
    // build meanDataType selector
    myMeanDataTypeSelector = new MeanDataTypeSelector(this);
    // build meanData editor
    myMeanDataEditor = new MeanDataEditor(this);
}


GNEMeanDataFrame::~GNEMeanDataFrame() {
}


void
GNEMeanDataFrame::show() {

    // show frame
    GNEFrame::show();
}


void
GNEMeanDataFrame::hide() {

    // hide frame
    GNEFrame::hide();
}


void
GNEMeanDataFrame::updateFrameAfterUndoRedo() {

}

/****************************************************************************/
