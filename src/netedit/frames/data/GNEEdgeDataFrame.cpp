/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEEdgeDataFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// The Widget for add edgeData elements
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/xml/SUMOSAXAttributesImpl_Cached.h>
#include <netedit/elements/data/GNEDataSet.h>
#include <netedit/elements/data/GNEDataInterval.h>
#include <netedit/elements/data/GNEDataHandler.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/network/GNELane.h>
#include <netedit/elements/network/GNEConnection.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>

#include "GNEEdgeDataFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEEdgeDataFrame::DataSetSelector) DataSetSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DATASET_NEW,         GNEEdgeDataFrame::DataSetSelector::onCmdSetNewDataSetID),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DATASET_SELECTED,    GNEEdgeDataFrame::DataSetSelector::onCmdSelectDataSet),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DATASET_OPTION,      GNEEdgeDataFrame::DataSetSelector::onCmdSelectRadioButton)
};

// Object implementation
FXIMPLEMENT(GNEEdgeDataFrame::DataSetSelector, FXGroupBox, DataSetSelectorMap, ARRAYNUMBER(DataSetSelectorMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEEdgeDataFrame::DataSetSelector - methods
// ---------------------------------------------------------------------------

GNEEdgeDataFrame::DataSetSelector::DataSetSelector(GNEEdgeDataFrame* edgeDataFrameParent) :
    FXGroupBox(edgeDataFrameParent->myContentFrame, "DataSet", GUIDesignGroupBoxFrame),
    myEdgeDataFrameParent(edgeDataFrameParent) {
    // create radio buttons
    myNewDataSetRadioButton = new FXRadioButton(this, "create new dataSet",
        this, MID_GNE_DATASET_OPTION, GUIDesignRadioButton);
    myExistentDataSetRadioButton = new FXRadioButton(this, "Use existent dataSet",
        this, MID_GNE_DATASET_OPTION, GUIDesignRadioButton);
    myExistentDataSetRadioButton->setCheck(TRUE);
    // create new id elements
    FXHorizontalFrame *horizontalFrameNewID = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myNetDataSetLabel = new FXLabel(horizontalFrameNewID, "new dataSet ID", nullptr, GUIDesignLabelAttribute);
    myNetDataSetLabel->disable();
    myNewDataSetIDTextField = new FXTextField(horizontalFrameNewID, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myNewDataSetIDTextField->disable();
    // Create FXComboBox
    myDataSetsComboBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_DATASET_SELECTED, GUIDesignComboBox);
    /*
    // create begin elements
    FXHorizontalFrame *horizontalFrameBegin = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(horizontalFrameBegin, toString(SUMO_ATTR_BEGIN).c_str(), nullptr, GUIDesignLabelAttribute);
    myBeginTextField = new FXTextField(horizontalFrameBegin, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myBeginTextField->setText("0");
    // create end elements
    FXHorizontalFrame *horizontalFrameEnd = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(horizontalFrameEnd, toString(SUMO_ATTR_END).c_str(), nullptr, GUIDesignLabelAttribute);
    myEndTextField = new FXTextField(horizontalFrameEnd, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myEndTextField->setText("3600");
    */
    // refresh interval selector
    refreshDataSetSelector();
    // DataSetSelector is always shown
    show();
}


GNEEdgeDataFrame::DataSetSelector::~DataSetSelector() {}


void
GNEEdgeDataFrame::DataSetSelector::refreshDataSetSelector() {
    // clear items
    myDataSetsComboBox->clearItems();
    // fill myDataSetsComboBox with all DataSets
    auto dataSetCopy = myEdgeDataFrameParent->getViewNet()->getNet()->retrieveDataSets();
    for (const auto& dataSet : dataSetCopy) {
        myDataSetsComboBox->appendItem(dataSet->getID().c_str());
    }
    // Set visible items
    myDataSetsComboBox->setNumVisible((int)myDataSetsComboBox->getNumItems());
    // recalc frame
    recalc();
}


std::string 
GNEEdgeDataFrame::DataSetSelector::getNewDataSetID() const {
    if (createNewDataSet() && (myNewDataSetIDTextField->getTextColor() == FXRGB(0, 0, 0))) {
        return myNewDataSetIDTextField->getText().text();
    } else {
        return "";
    }
}


GNEDataSet*
GNEEdgeDataFrame::DataSetSelector::getSelectedDataSet() const {
    return nullptr;
}


bool 
GNEEdgeDataFrame::DataSetSelector::createNewDataSet() const {
    //
    return 1;
}


long 
GNEEdgeDataFrame::DataSetSelector::onCmdSetNewDataSetID(FXObject*, FXSelector, void*) {
    //
    return 1;
}


long 
GNEEdgeDataFrame::DataSetSelector::onCmdSelectDataSet(FXObject* obj, FXSelector, void*) {
    //
    return 1;
}


long 
GNEEdgeDataFrame::DataSetSelector::onCmdSelectRadioButton(FXObject* obj, FXSelector, void*) {
    if (obj == myNewDataSetRadioButton) {
        // set radio buttons
        myNewDataSetRadioButton->setCheck(TRUE, FALSE);
        myExistentDataSetRadioButton->setCheck(FALSE, FALSE);
        // enable textfield and label
        myNewDataSetIDTextField->enable();
        myNetDataSetLabel->enable();
        // disable comboBox
        myDataSetsComboBox->disable();
    } else if (obj == myExistentDataSetRadioButton) {
        // set radio buttons
        myNewDataSetRadioButton->setCheck(FALSE, FALSE);
        myExistentDataSetRadioButton->setCheck(TRUE, FALSE);
        // disable textfield and label
        myNewDataSetIDTextField->disable();
        myNetDataSetLabel->disable();
        // enable comboBox
        myDataSetsComboBox->enable();
    }
    return 1;
}

/*
long 
GNEEdgeDataFrame::DataSetSelector::onCmdSetAttribute(FXObject* obj, FXSelector, void*) {
    if (obj == myNewIDTextField) {
        // check new ID
        if (myNewIDTextField->getText().empty() || 
            (myEdgeDataFrameParent->getViewNet()->getNet()->retrieveDataSet(myNewIDTextField->getText().text(), false) == nullptr) &&
            (SUMOXMLDefinitions::isValidNetID(myNewIDTextField->getText().text()))) {
            myNewIDTextField->setTextColor(FXRGB(0, 0, 0));
            myNewIDTextField->killFocus();
        } else {
            myNewIDTextField->setTextColor(FXRGB(255, 0, 0));
        }
    } else if (obj == myBeginTextField) {
        // check if begin value can be parsed to double
        if (GNEAttributeCarrier::canParse<double>(myBeginTextField->getText().text())) {
            myBeginTextField->setTextColor(FXRGB(0, 0, 0));
            myBeginTextField->killFocus();
        } else {
            myBeginTextField->setTextColor(FXRGB(255, 0, 0));
        }
    } else if (obj == myEndTextField) {
        // check if end value can be parsed to double
        if (GNEAttributeCarrier::canParse<double>(myEndTextField->getText().text())) {
            myEndTextField->setTextColor(FXRGB(0, 0, 0));
            myEndTextField->killFocus();
        } else {
            myEndTextField->setTextColor(FXRGB(255, 0, 0));
        }
    }
    return 1;
}
*/
// ---------------------------------------------------------------------------
// GNEEdgeDataFrame - methods
// ---------------------------------------------------------------------------

GNEEdgeDataFrame::GNEEdgeDataFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "EdgeData") {
    // create DataSetSelector
    myDataSetSelector = new DataSetSelector(this);
    // create parameter editor
    myParametersEditor = new GNEFrameAttributesModuls::ParametersEditor(this);
}


GNEEdgeDataFrame::~GNEEdgeDataFrame() {}


void
GNEEdgeDataFrame::show() {
    // first refresh data set selector
    myDataSetSelector->refreshDataSetSelector();
    // show frame
    GNEFrame::show();
}


bool
GNEEdgeDataFrame::addEdgeData(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor) {
/*
    // first check if we clicked over an edge
    if (objectsUnderCursor.getEdgeFront() && myDataSetSelector->isIntervalValid()) {

        GNEDataInterval *dataInterval = nullptr;
        GNEDataSet *dataSet = nullptr;
        // check if we have to create a new dataSet
        if (myDataSetSelector->createNewDataSet()) {
            // check if we can create the new dataSet
            if (myDataSetSelector->getDataSetID().empty()) {
                // if obtained dataSet ID is empty, then given ID isn't valid (duplicated/invalid characters, etc.)
                return false;
            } else {
                dataSet = GNEDataHandler::buildDataSet(myViewNet, true, myDataSetSelector->getDataSetID());
                // refresh interval selector
                myDataSetSelector->refreshDataSetSelector();
            }
        } else {
            dataSet = myViewNet->getNet()->retrieveDataSet(myDataSetSelector->getDataSetID());
        }
        // now check if there is another interval with the given begin/end
        dataInterval = dataSet->retrieveInterval(myDataSetSelector->getBegin(), myDataSetSelector->getEnd());
        // if not, create it
        if (dataInterval == nullptr) {
            // check if given interval is valid
            if (dataSet->checkNewInterval(myDataSetSelector->getBegin(), myDataSetSelector->getEnd())) {
                dataInterval = GNEDataHandler::buildDataInterval(myViewNet, true, dataSet, myDataSetSelector->getBegin(), myDataSetSelector->getEnd());
            } else {
                return false;
            }
        }
        // finally create edgeData
        GNEDataHandler::buildEdgeData(myViewNet, true, dataInterval, objectsUnderCursor.getEdgeFront(), myParametersEditor->getParametersMap());
        // edgeData created, then return true

        return true;
    } else {
        // invalid parameters
        return false;
    }
*/
    return false ;
}


void 
GNEEdgeDataFrame::intervalSelected() {
    //
}

/****************************************************************************/
