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

// Object implementation
FXIMPLEMENT(GNEMeanDataFrame::MeanDataTypeSelector,         MFXGroupBoxModule,  typeSelectorMap,        ARRAYNUMBER(typeSelectorMap))

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
    bool valid = false;
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
/*
    // refresh vehicle type editor module
    myMeanDataFrameParent->myTypeEditor->refreshTypeEditorModule();
    // set myCurrentMeanData as inspected element
    myMeanDataFrameParent->getViewNet()->setInspectedAttributeCarriers({myCurrentMeanData});
    // show modules
    myMeanDataFrameParent->myTypeAttributesEditor->showAttributeEditorModule(false, true);
    myMeanDataFrameParent->myAttributesEditorExtended->showAttributesEditorExtendedModule();
    myMeanDataFrameParent->myVTypeDistributions->showVTypeDistributionsModule();
*/
}


void
GNEMeanDataFrame::MeanDataTypeSelector::refreshMeanDataTypeSelectorIDs() {
    if (myCurrentMeanData.getTagStr() != myInvalidMeanData.getTagStr()) {
        myTypeComboBox->setIconItem(myTypeComboBox->getCurrentItem(), myCurrentMeanData.getTagStr().c_str(), 
            GUIIconSubSys::getIcon(myCurrentMeanData.getGUIIcon()));
    }
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
/*
            // refresh vehicle type editor module
            myMeanDataFrameParent->myTypeEditor->refreshTypeEditorModule();
            // set myCurrentMeanData as inspected element
            myMeanDataFrameParent->getViewNet()->setInspectedAttributeCarriers({myCurrentMeanData});
            // show modules if selected item is valid
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
/*
    // refresh vehicle type editor module
    myMeanDataFrameParent->myTypeEditor->refreshTypeEditorModule();
    // hide all modules if selected item isn't valid
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
// GNEMeanDataFrame - methods
// ---------------------------------------------------------------------------

GNEMeanDataFrame::GNEMeanDataFrame(GNEViewParent* viewParent, GNEViewNet* viewNet) :
    GNEFrame(viewParent, viewNet, "Meandata") {
    // build meanDataType selector
    myMeanDataTypeSelector = new MeanDataTypeSelector(this);
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
