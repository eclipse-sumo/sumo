/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
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
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_DELETE,    GNEMeanDataFrame::MeanDataEditor::onCmdDeletetMeanData),
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
    MFXGroupBoxModule(meanDataFrameParent, TL("MeanData Type")),
    myMeanDataFrameParent(meanDataFrameParent) {
    // Create FXComboBox
    myTypeComboBox = new MFXComboBoxIcon(getCollapsableFrame(), GUIDesignComboBoxNCol, true, this, MID_GNE_SET_TYPE, GUIDesignComboBox);
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
    myMeanDataFrameParent->myMeanDataEditor->showMeanDataEditorModule();
    myMeanDataFrameParent->myMeanDataSelector->showMeanDataSelector();
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
            myMeanDataFrameParent->myMeanDataEditor->showMeanDataEditorModule();
            myMeanDataFrameParent->myMeanDataSelector->showMeanDataSelector();
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected item '" + myTypeComboBox->getText() + "' in MeanDataTypeSelector").text());
            return 1;
        }
    }
    myCurrentMeanData = myInvalidMeanData;
    // refresh meanData editor module
    myMeanDataFrameParent->myMeanDataEditor->refreshMeanDataEditorModule();
    // hide all modules if selected item isn't valid
    myMeanDataFrameParent->myMeanDataEditor->hideMeanDataEditorModule();
    myMeanDataFrameParent->myMeanDataSelector->hideMeanDataSelector();
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
    myCreateMeanDataButton = new FXButton(getCollapsableFrame(), TL("Create MeanData"),
                                          GUIIconSubSys::getIcon(GUIIcon::MODEMEANDATA), this, MID_GNE_CREATE, GUIDesignButton);
    // Create delete/reset meanData
    myDeleteMeanDataButton = new FXButton(getCollapsableFrame(), TL("Delete MeanData"),
                                          GUIIconSubSys::getIcon(GUIIcon::MODEDELETE), this, MID_GNE_DELETE, GUIDesignButton);
    // Create copy meanData
    myCopyMeanDataButton = new FXButton(getCollapsableFrame(), TL("Copy MeanData"),
                                        GUIIconSubSys::getIcon(GUIIcon::COPY), this, MID_GNE_COPY, GUIDesignButton);
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
        myDeleteMeanDataButton->disable();
        myCopyMeanDataButton->disable();
    } else {
        // enable buttons
        myDeleteMeanDataButton->enable();
        myCopyMeanDataButton->enable();
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
                                            myMeanDataFrameParent->myMeanDataTypeSelector->getCurrentMeanData().getTag(), typeID);
    // add it using undoList (to allow undo-redo)
    myMeanDataFrameParent->myViewNet->getUndoList()->begin(GUIIcon::VTYPE, "create meanData");
    myMeanDataFrameParent->myViewNet->getUndoList()->add(new GNEChange_MeanData(meanData, true), true);
    myMeanDataFrameParent->myViewNet->getUndoList()->end();
    // set created meanData in selector
    myMeanDataFrameParent->myMeanDataSelector->setCurrentMeanData(meanData);
    return 1;
}


long
GNEMeanDataFrame::MeanDataEditor::onCmdDeletetMeanData(FXObject*, FXSelector, void*) {
    // begin undo list operation
    myMeanDataFrameParent->myViewNet->getUndoList()->begin(GUIIcon::VTYPE, "delete meanData");
    // remove meanData (and all of their children)
    myMeanDataFrameParent->myViewNet->getNet()->deleteMeanData(myMeanDataFrameParent->myMeanDataSelector->getCurrentMeanData(),
            myMeanDataFrameParent->myViewNet->getUndoList());
    // end undo list operation
    myMeanDataFrameParent->myViewNet->getUndoList()->end();
    // set created meanData in selector
    myMeanDataFrameParent->myMeanDataSelector->refreshMeanDataSelector(false);
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
                                                myMeanDataFrameParent->myMeanDataTypeSelector->getCurrentMeanData().getTag(), typeID);
        // begin undo list operation
        myMeanDataFrameParent->myViewNet->getUndoList()->begin(GUIIcon::VTYPE, "copy meanData");
        // add it using undoList (to allow undo-redo)
        myMeanDataFrameParent->myViewNet->getUndoList()->add(new GNEChange_MeanData(typeCopy, true), true);
        // end undo list operation
        myMeanDataFrameParent->myViewNet->getUndoList()->end();
        // refresh MeanData Selector (to show the new VMeanData)
        myMeanDataFrameParent->myMeanDataSelector->refreshMeanDataSelector(false);
        // set created meanData in selector
        myMeanDataFrameParent->myMeanDataSelector->setCurrentMeanData(typeCopy);
        // refresh MeanData Editor Module
        myMeanDataFrameParent->myMeanDataEditor->refreshMeanDataEditorModule();
    }
    return 1;
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
    myMeanDataComboBox = new MFXComboBoxIcon(getCollapsableFrame(), GUIDesignComboBoxNCol, true, this, MID_GNE_SET_TYPE, GUIDesignComboBox);
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


void
GNEMeanDataFrame::MeanDataSelector::showMeanDataSelector() {
    // refresh mean data selector
    refreshMeanDataSelector(false);
    // show
    show();
}


void
GNEMeanDataFrame::MeanDataSelector::hideMeanDataSelector() {
    // hide attributes editor
    myMeanDataFrameParent->myMeanDataAttributesEditor->hideAttributesEditorModule();
    // hide
    hide();
}


GNEMeanData*
GNEMeanDataFrame::MeanDataSelector::getCurrentMeanData() const {
    return myCurrentMeanData;
}


void
GNEMeanDataFrame::MeanDataSelector::setCurrentMeanData(GNEMeanData* vMeanData) {
    myCurrentMeanData = vMeanData;
    refreshMeanDataSelector(false);
}


void
GNEMeanDataFrame::MeanDataSelector::refreshMeanDataSelector(bool afterChangingID) {
    // get current meanData type
    SumoXMLTag meanDataTag = myMeanDataFrameParent->myMeanDataTypeSelector->getCurrentMeanData().getTag();
    // get mean datas sorted by ID
    std::map<std::string, GNEMeanData*> sortedMeanDatas;
    for (const auto& meanData : myMeanDataFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getMeanDatas().at(meanDataTag)) {
        sortedMeanDatas[meanData->getID()] = meanData;
    }
    // clear items
    myMeanDataComboBox->clearItems();
    // fill myMeanDataMatchBox with meanDatas
    for (const auto& sortedMeanData : sortedMeanDatas) {
        myMeanDataComboBox->appendIconItem(sortedMeanData.first.c_str(), sortedMeanData.second->getACIcon());
    }
    // Set visible items
    if (myMeanDataComboBox->getNumItems() <= 20) {
        myMeanDataComboBox->setNumVisible((int)myMeanDataComboBox->getNumItems());
    } else {
        myMeanDataComboBox->setNumVisible(20);
    }
    // make sure that mean data exists
    if (myMeanDataFrameParent->getViewNet()->getNet()->getAttributeCarriers()->retrieveMeanData(myCurrentMeanData, false)) {
        bool validMeanData = false;
        for (int i = 0; i < (int)myMeanDataComboBox->getNumItems(); i++) {
            if (myMeanDataComboBox->getItem(i).text() == myCurrentMeanData->getID()) {
                myMeanDataComboBox->setCurrentItem(i);
                validMeanData = true;
            }
        }
        if (!validMeanData) {
            myCurrentMeanData = nullptr;
        }
    } else {
        myCurrentMeanData = nullptr;
    }
    // check if enable or disable comboBox
    if (sortedMeanDatas.size() > 0) {
        myMeanDataComboBox->enable();
        // check ifupdate myCurrentMeanData
        if (myCurrentMeanData == nullptr) {
            myCurrentMeanData = sortedMeanDatas.begin()->second;
        }
    } else {
        myMeanDataComboBox->disable();
    }
    // refresh meanData editor module
    myMeanDataFrameParent->myMeanDataEditor->refreshMeanDataEditorModule();
    // check if show attribute editor
    if (!afterChangingID) {
        if (myCurrentMeanData) {
            // set myCurrentMeanData as inspected element (needed for attribute editor)
            myMeanDataFrameParent->getViewNet()->setInspectedAttributeCarriers({myCurrentMeanData});
            myMeanDataFrameParent->myMeanDataAttributesEditor->showAttributeEditorModule(false, true);
        } else {
            // set myCurrentMeanData as inspected element (needed for attribute editor)
            myMeanDataFrameParent->getViewNet()->setInspectedAttributeCarriers({});
            myMeanDataFrameParent->myMeanDataAttributesEditor->hideAttributesEditorModule();
        }
    }
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
            myMeanDataFrameParent->myMeanDataAttributesEditor->showAttributeEditorModule(false, true);
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
    myMeanDataFrameParent->myMeanDataAttributesEditor->hideAttributesEditorModule();
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
    GNEFrame(viewParent, viewNet, "MeanData") {
    // build meanData type selector
    myMeanDataTypeSelector = new MeanDataTypeSelector(this);
    // build meanData editor
    myMeanDataEditor = new MeanDataEditor(this);
    // build meanData selector
    myMeanDataSelector = new MeanDataSelector(this);
    // build meanData attributes editor
    myMeanDataAttributesEditor = new GNEFrameAttributeModules::AttributesEditor(this);
}


GNEMeanDataFrame::~GNEMeanDataFrame() {
}


void
GNEMeanDataFrame::show() {
    // refresh meanData type selector
    myMeanDataTypeSelector->refreshMeanDataTypeSelector();
    // show frame
    GNEFrame::show();
}


void
GNEMeanDataFrame::hide() {
    // hide frame
    GNEFrame::hide();
}


void
GNEMeanDataFrame::attributeUpdated(SumoXMLAttr attribute) {
    if (attribute == SUMO_ATTR_ID) {
        myMeanDataSelector->refreshMeanDataSelector(true);
    }
}


void
GNEMeanDataFrame::updateFrameAfterUndoRedo() {
}

/****************************************************************************/
