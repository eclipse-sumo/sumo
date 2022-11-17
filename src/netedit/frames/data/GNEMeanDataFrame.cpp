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

FXDEFMAP(GNEMeanDataFrame::MeanDataTypeSelector) meanDataSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_TYPE,   GNEMeanDataFrame::MeanDataTypeSelector::onCmdSelectItem)
};

FXDEFMAP(GNEMeanDataFrame::MeanDataEditor) meanDataEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATE,    GNEMeanDataFrame::MeanDataEditor::onCmdCreateMeanData),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_DELETE,    GNEMeanDataFrame::MeanDataEditor::onCmdDeleteResetMeanData),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_COPY,      GNEMeanDataFrame::MeanDataEditor::onCmdCopyMeanData)
};

FXDEFMAP(GNEMeanDataFrame::MeanDataSelector) meanDataTypeSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_TYPE,   GNEMeanDataFrame::MeanDataSelector::onCmdSelectItem)
};

// Object implementation
FXIMPLEMENT(GNEMeanDataFrame::MeanDataTypeSelector, MFXGroupBoxModule,  meanDataSelectorMap,        ARRAYNUMBER(meanDataSelectorMap))
FXIMPLEMENT(GNEMeanDataFrame::MeanDataEditor,       MFXGroupBoxModule,  meanDataEditorMap,          ARRAYNUMBER(meanDataEditorMap))
FXIMPLEMENT(GNEMeanDataFrame::MeanDataSelector,     MFXGroupBoxModule,  meanDataTypeSelectorMap,    ARRAYNUMBER(meanDataTypeSelectorMap))


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
    myCreateMeanDataButton = new FXButton(getCollapsableFrame(), TL("Create MeanData"), GUIIconSubSys::getIcon(GUIIcon::MEANDATAEDGE), this, MID_GNE_CREATE, GUIDesignButton);
    // Create delete/reset meanData
    myDeleteResetMeanDataButton = new FXButton(getCollapsableFrame(), TL("Delete MeanData"), GUIIconSubSys::getIcon(GUIIcon::MODEDELETE), this, MID_GNE_DELETE, GUIDesignButton);
    // Create copy meanData
    myCopyMeanDataButton = new FXButton(getCollapsableFrame(), TL("Copy MeanData"), GUIIconSubSys::getIcon(GUIIcon::COPY), this, MID_GNE_COPY, GUIDesignButton);
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
    // first check if selected VMeanData is valid
    if (myMeanDataFrameParent->myMeanDataSelector->getCurrentMeanData() == nullptr) {
        // disable buttons
        myDeleteResetMeanDataButton->disable();
        myCopyMeanDataButton->disable();
    } else if (GNEAttributeCarrier::parse<bool>(myMeanDataFrameParent->myMeanDataSelector->getCurrentMeanData()->getAttribute(GNE_ATTR_DEFAULT_VTYPE))) {
        // enable copy button
        myCopyMeanDataButton->enable();
        // enable and set myDeleteMeanDataButton as "reset")
        myDeleteResetMeanDataButton->setText(TL("Reset MeanData"));
        myDeleteResetMeanDataButton->setIcon(GUIIconSubSys::getIcon(GUIIcon::RESET));
        // check if reset default meanData button has to be enabled or disabled
        if (GNEAttributeCarrier::parse<bool>(myMeanDataFrameParent->myMeanDataSelector->getCurrentMeanData()->getAttribute(GNE_ATTR_DEFAULT_VTYPE_MODIFIED))) {
            myDeleteResetMeanDataButton->enable();
        } else {
            myDeleteResetMeanDataButton->disable();
        }
    } else {
        // enable copy button
        myCopyMeanDataButton->enable();
        // enable and set myDeleteMeanDataButton as "delete")
        myDeleteResetMeanDataButton->setText(TL("Delete MeanData"));
        myDeleteResetMeanDataButton->setIcon(GUIIconSubSys::getIcon(GUIIcon::MODEDELETE));
        myDeleteResetMeanDataButton->enable();
    }
    // update module
    recalc();
}


long
GNEMeanDataFrame::MeanDataEditor::onCmdCreateMeanData(FXObject*, FXSelector, void*) {
    // get current meanData type
    SumoXMLTag meanDataTag = myMeanDataFrameParent->myMeanDataTypeSelector->getCurrentMeanData().getTag();
    // obtain a new valid MeanData ID
    const std::string typeID = myMeanDataFrameParent->myViewNet->getNet()->getAttributeCarriers()->generateMeanDataID(meanDataTag);
    // create new meanData
    GNEMeanData* meanData = new GNEMeanData(myMeanDataFrameParent->myViewNet->getNet(), 
        myMeanDataFrameParent->myMeanDataTypeSelector->getCurrentMeanData().getTag(), "");
    // add it using undoList (to allow undo-redo)
    myMeanDataFrameParent->myViewNet->getUndoList()->begin(GUIIcon::VTYPE, "create meanData");
    myMeanDataFrameParent->myViewNet->getUndoList()->add(new GNEChange_MeanData(meanData, true), true);
    myMeanDataFrameParent->myViewNet->getUndoList()->end();
    // set created meanData in selector
    myMeanDataFrameParent->myMeanDataSelector->setCurrentMeanData(meanData);
    // refresh MeanData Editor Module
    myMeanDataFrameParent->myMeanDataEditor->refreshMeanDataEditorModule();
    return 1;
}


long
GNEMeanDataFrame::MeanDataEditor::onCmdDeleteResetMeanData(FXObject*, FXSelector, void*) {
    // continue depending of current mode
    if (myDeleteResetMeanDataButton->getIcon() == GUIIconSubSys::getIcon(GUIIcon::MODEDELETE)) {
        deleteMeanData();
    } else {
        resetMeanData();
    }
    return 1;
}


long
GNEMeanDataFrame::MeanDataEditor::onCmdCopyMeanData(FXObject*, FXSelector, void*) {
    // get current meanData type
    SumoXMLTag meanDataTag = myMeanDataFrameParent->myMeanDataTypeSelector->getCurrentMeanData().getTag();
    // obtain a new valid MeanData ID
    const std::string typeID = myMeanDataFrameParent->myViewNet->getNet()->getAttributeCarriers()->generateMeanDataID(meanDataTag);
    // obtain meanData in which new MeanData will be based
    GNEMeanData* meanData = dynamic_cast<GNEMeanData*>(myMeanDataFrameParent->myMeanDataSelector->getCurrentMeanData());
    // check that meanData exist
    if (meanData) {
        // create a new MeanData based on the current selected meanData
        GNEMeanData* typeCopy = new GNEMeanData(myMeanDataFrameParent->myViewNet->getNet(),
            myMeanDataFrameParent->myMeanDataTypeSelector->getCurrentMeanData().getTag(), "");
        // begin undo list operation
        myMeanDataFrameParent->myViewNet->getUndoList()->begin(GUIIcon::VTYPE, "copy meanData");
        // add it using undoList (to allow undo-redo)
        myMeanDataFrameParent->myViewNet->getUndoList()->add(new GNEChange_MeanData(typeCopy, true), true);
        // end undo list operation
        myMeanDataFrameParent->myViewNet->getUndoList()->end();
        // refresh MeanData Selector (to show the new VMeanData)
        myMeanDataFrameParent->myMeanDataSelector->refreshMeanDataSelector();
        // set created meanData in selector
        myMeanDataFrameParent->myMeanDataSelector->setCurrentMeanData(typeCopy);
        // refresh MeanData Editor Module
        myMeanDataFrameParent->myMeanDataEditor->refreshMeanDataEditorModule();
    }
    return 1;
}


void
GNEMeanDataFrame::MeanDataEditor::resetMeanData() {
    // begin reset default meanData values
    myMeanDataFrameParent->getViewNet()->getUndoList()->begin(GUIIcon::VTYPE, "reset default meanData values");
    // reset all values of default meanData
    for (const auto& attrProperty : myMeanDataFrameParent->myMeanDataTypeSelector->getCurrentMeanData()) {
        // change all attributes with "" to reset it (except ID and vClass)
        if ((attrProperty.getAttr() != SUMO_ATTR_ID) && (attrProperty.getAttr() != SUMO_ATTR_VCLASS)) {
            myMeanDataFrameParent->myMeanDataSelector->getCurrentMeanData()->setAttribute(attrProperty.getAttr(), "", myMeanDataFrameParent->myViewNet->getUndoList());
        }
    }
    // change special attribute GNE_ATTR_DEFAULT_VTYPE_MODIFIED
    myMeanDataFrameParent->myMeanDataSelector->getCurrentMeanData()->setAttribute(GNE_ATTR_DEFAULT_VTYPE_MODIFIED, "false", myMeanDataFrameParent->myViewNet->getUndoList());
    // finish reset default meanData values
    myMeanDataFrameParent->getViewNet()->getUndoList()->end();
    // refresh MeanDataSelector
    myMeanDataFrameParent->myMeanDataSelector->refreshMeanDataSelector();
}


void
GNEMeanDataFrame::MeanDataEditor::deleteMeanData() {
    // begin undo list operation
    myMeanDataFrameParent->myViewNet->getUndoList()->begin(GUIIcon::VTYPE, "delete meanData");
    // remove meanData (and all of their children)
    myMeanDataFrameParent->myViewNet->getNet()->deleteMeanData(myMeanDataFrameParent->myMeanDataSelector->getCurrentMeanData(),
            myMeanDataFrameParent->myViewNet->getUndoList());
    // end undo list operation
    myMeanDataFrameParent->myViewNet->getUndoList()->end();
}

// ---------------------------------------------------------------------------
// GNEMeanDataFrame::MeanDataSelector - methods
// ---------------------------------------------------------------------------

GNEMeanDataFrame::MeanDataSelector::MeanDataSelector(GNEMeanDataFrame* typeFrameParent) :
    MFXGroupBoxModule(typeFrameParent, TL("Current MeanData")),
    myMeanDataFrameParent(typeFrameParent),
    myCurrentMeanData(nullptr) {
    // get current meanData type
    SumoXMLTag meanDataTag = myMeanDataFrameParent->myMeanDataTypeSelector->getCurrentMeanData().getTag();
    // Create FXComboBox
    myMeanDataComboBox = new MFXIconComboBox(getCollapsableFrame(), GUIDesignComboBoxNCol, true, this, MID_GNE_SET_TYPE, GUIDesignComboBox);
    // add meanDatas
    for (const auto& vMeanData : myMeanDataFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getMeanDatas().at(meanDataTag)) {
        myMeanDataComboBox->appendIconItem(vMeanData->getID().c_str(), vMeanData->getACIcon());
    }
    if (myMeanDataFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getMeanDatas().at(meanDataTag).size() > 0) {
        myCurrentMeanData = *myMeanDataFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getMeanDatas().at(meanDataTag).begin();
    } else {
        myCurrentMeanData = nullptr;
    }
    // Set visible items
    if (myMeanDataComboBox->getNumItems() <= 20) {
        myMeanDataComboBox->setNumVisible((int)myMeanDataComboBox->getNumItems());
    } else {
        myMeanDataComboBox->setNumVisible(20);
    }
    // MeanDataSelector is always shown
    show();
}


GNEMeanDataFrame::MeanDataSelector::~MeanDataSelector() {}


GNEMeanData*
GNEMeanDataFrame::MeanDataSelector::getCurrentMeanData() const {
    return myCurrentMeanData;
}


void
GNEMeanDataFrame::MeanDataSelector::setCurrentMeanData(GNEMeanData* vMeanData) {
    myCurrentMeanData = vMeanData;
    refreshMeanDataSelector();
}


void
GNEMeanDataFrame::MeanDataSelector::refreshMeanDataSelector() {
    // get current meanData type
    SumoXMLTag meanDataTag = myMeanDataFrameParent->myMeanDataTypeSelector->getCurrentMeanData().getTag();
    bool valid = false;
    // clear items
    myMeanDataComboBox->clearItems();
    // add default Vehicle an Bike types in the first and second positions
    for (const auto& vMeanData : myMeanDataFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getMeanDatas().at(meanDataTag)) {
        if (DEFAULT_VTYPES.count(vMeanData->getID()) != 0) {
            myMeanDataComboBox->appendIconItem(vMeanData->getID().c_str(), vMeanData->getACIcon(), FXRGB(255, 255, 200));
        }
    }
    // fill myMeanDataMatchBox with list of VMeanDatas IDs
    for (const auto& vMeanData : myMeanDataFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getMeanDatas().at(meanDataTag)) {
        if (DEFAULT_VTYPES.count(vMeanData->getID()) == 0) {
            myMeanDataComboBox->appendIconItem(vMeanData->getID().c_str(), vMeanData->getACIcon());
        }
    }
    // Set visible items
    if (myMeanDataComboBox->getNumItems() <= 20) {
        myMeanDataComboBox->setNumVisible((int)myMeanDataComboBox->getNumItems());
    } else {
        myMeanDataComboBox->setNumVisible(20);
    }
    // make sure that tag is in myMeanDataMatchBox
    if (myCurrentMeanData) {
        for (int i = 0; i < (int)myMeanDataComboBox->getNumItems(); i++) {
            if (myMeanDataComboBox->getItem(i).text() == myCurrentMeanData->getID()) {
                myMeanDataComboBox->setCurrentItem(i);
                valid = true;
            }
        }
    }
    // Check that give vMeanData type is valid
    if (!valid) {
        // refresh myMeanDataMatchBox again
        for (int i = 0; i < (int)myMeanDataComboBox->getNumItems(); i++) {
            if (myMeanDataComboBox->getItem(i).text() == myCurrentMeanData->getID()) {
                myMeanDataComboBox->setCurrentItem(i);
            }
        }
    }
    // refresh meanData editor module
    myMeanDataFrameParent->myMeanDataEditor->refreshMeanDataEditorModule();
    // set myCurrentMeanData as inspected element
    myMeanDataFrameParent->getViewNet()->setInspectedAttributeCarriers({myCurrentMeanData});
    // show modules
/*
    myMeanDataFrameParent->myMeanDataAttributesEditor->showAttributeEditorModule(false, true);
*/
}


void
GNEMeanDataFrame::MeanDataSelector::refreshMeanDataSelectorIDs() {
    if (myCurrentMeanData) {
        myMeanDataComboBox->setIconItem(myMeanDataComboBox->getCurrentItem(), myCurrentMeanData->getID().c_str(), myCurrentMeanData->getACIcon());
    }
}


long
GNEMeanDataFrame::MeanDataSelector::onCmdSelectItem(FXObject*, FXSelector, void*) {
    // get current meanData type
    SumoXMLTag meanDataTag = myMeanDataFrameParent->myMeanDataTypeSelector->getCurrentMeanData().getTag();
    // Check if value of myMeanDataMatchBox correspond of an allowed additional tags
    for (const auto& vMeanData : myMeanDataFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getMeanDatas().at(meanDataTag)) {
        if (vMeanData->getID() == myMeanDataComboBox->getText().text()) {
            // set pointer
            myCurrentMeanData = vMeanData;
            // set color of myMeanDataMatchBox to black (valid)
            myMeanDataComboBox->setTextColor(FXRGB(0, 0, 0));
            // refresh meanData editor module
            myMeanDataFrameParent->myMeanDataEditor->refreshMeanDataEditorModule();
            // set myCurrentMeanData as inspected element
            myMeanDataFrameParent->getViewNet()->setInspectedAttributeCarriers({myCurrentMeanData});
            // show modules if selected item is valid
/*
            myMeanDataFrameParent->myMeanDataAttributesEditor->showAttributeEditorModule(false, true);
*/
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected item '" + myMeanDataComboBox->getText() + "' in MeanDataSelector").text());
            // update viewNet
            myMeanDataFrameParent->getViewNet()->updateViewNet();
            return 1;
        }
    }
    myCurrentMeanData = nullptr;
    // refresh meanData editor module
    myMeanDataFrameParent->myMeanDataEditor->refreshMeanDataEditorModule();
    // hide all modules if selected item isn't valid
/*
    myMeanDataFrameParent->myMeanDataAttributesEditor->hideAttributesEditorModule();
*/
    // set color of myMeanDataMatchBox to red (invalid)
    myMeanDataComboBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Selected invalid item in MeanDataSelector");
    // update viewNet
    myMeanDataFrameParent->getViewNet()->updateViewNet();
    return 1;
}

// ---------------------------------------------------------------------------
// GNEMeanDataFrame - methods
// ---------------------------------------------------------------------------

GNEMeanDataFrame::GNEMeanDataFrame(GNEViewParent* viewParent, GNEViewNet* viewNet) :
    GNEFrame(viewParent, viewNet, "Meandata") {
    // build meanData type selector
    myMeanDataTypeSelector = new MeanDataTypeSelector(this);
    // build meanData editor
    myMeanDataEditor = new MeanDataEditor(this);
    // build meanData selector
    myMeanDataSelector = new MeanDataSelector(this);
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
