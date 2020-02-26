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

FXDEFMAP(GNEEdgeDataFrame::IntervalSelector) IntervalSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SELECT,              GNEEdgeDataFrame::IntervalSelector::onCmdSelectInterval),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_ATTRIBUTE,       GNEEdgeDataFrame::IntervalSelector::onCmdSetIntervalAttribute),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DATAINTERVAL_OPTION, GNEEdgeDataFrame::IntervalSelector::onCmdSelectRadioButton)
};

// Object implementation
FXIMPLEMENT(GNEEdgeDataFrame::DataSetSelector,  FXGroupBox, DataSetSelectorMap,  ARRAYNUMBER(DataSetSelectorMap))
FXIMPLEMENT(GNEEdgeDataFrame::IntervalSelector, FXGroupBox, IntervalSelectorMap, ARRAYNUMBER(IntervalSelectorMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEEdgeDataFrame::DataSetSelector - methods
// ---------------------------------------------------------------------------

GNEEdgeDataFrame::DataSetSelector::DataSetSelector(GNEEdgeDataFrame* edgeDataFrameParent) :
    FXGroupBox(edgeDataFrameParent->myContentFrame, "DataSet", GUIDesignGroupBoxFrame),
    myEdgeDataFrameParent(edgeDataFrameParent) {
    // create radio button for new data set
    myNewDataSetRadioButton = new FXRadioButton(this, "Create new dataSet", this, MID_GNE_DATASET_OPTION, GUIDesignRadioButton);
    // create radio button for existent data set
    myExistentDataSetRadioButton = new FXRadioButton(this, "Use existent dataSet", this, MID_GNE_DATASET_OPTION, GUIDesignRadioButton);
    myExistentDataSetRadioButton->setCheck(TRUE);
    // Create FXComboBox
    myDataSetsComboBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_DATASET_SELECTED, GUIDesignComboBox);
    // create new id label
    myHorizontalFrameNewID = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(myHorizontalFrameNewID, "new dataSet ID", nullptr, GUIDesignLabelAttribute);
    // create new id textField
    myNewDataSetIDTextField = new FXTextField(myHorizontalFrameNewID, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // hide horizontal frame
    myHorizontalFrameNewID->hide();
    // create dataSet button
    myCreateDataSetButton = new FXButton(this, "Create dataSet", GUIIconSubSys::getIcon(ICON_DATASET), this, MID_GNE_CREATE, GUIDesignButton);
    myCreateDataSetButton->hide();
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
    // refresh interval selector
    if (myEdgeDataFrameParent->myIntervalSelector) {
        myEdgeDataFrameParent->myIntervalSelector->refreshIntervalSelector();
    }
}


GNEDataSet*
GNEEdgeDataFrame::DataSetSelector::getDataSet() const {
    if ((myNewDataSetRadioButton->getCheck() == TRUE) || (myDataSetsComboBox->getNumItems() == 0)) {
        return nullptr;
    } else {
        return myEdgeDataFrameParent->getViewNet()->getNet()->retrieveDataSet(myDataSetsComboBox->getItem(myDataSetsComboBox->getCurrentItem()).text(), false);
    }
}


long 
GNEEdgeDataFrame::DataSetSelector::onCmdSetNewDataSetID(FXObject*, FXSelector, void*) {
    //
    return 1;
}


long 
GNEEdgeDataFrame::DataSetSelector::onCmdSelectDataSet(FXObject* obj, FXSelector, void*) {
    // update interval modul
    myEdgeDataFrameParent->myIntervalSelector->refreshIntervalSelector();
    return 1;
}


long 
GNEEdgeDataFrame::DataSetSelector::onCmdSelectRadioButton(FXObject* obj, FXSelector, void*) {
    if (obj == myNewDataSetRadioButton) {
        // set radio buttons
        myNewDataSetRadioButton->setCheck(TRUE, FALSE);
        myExistentDataSetRadioButton->setCheck(FALSE, FALSE);
        // enable textfield and label
        myHorizontalFrameNewID->show();
        myCreateDataSetButton->show();
        // disable comboBox
        myDataSetsComboBox->hide();
    } else if (obj == myExistentDataSetRadioButton) {
        // set radio buttons
        myNewDataSetRadioButton->setCheck(FALSE, FALSE);
        myExistentDataSetRadioButton->setCheck(TRUE, FALSE);
        // disable textfield and label
        myHorizontalFrameNewID->hide();
        myCreateDataSetButton->hide();
        // enable comboBox
        myDataSetsComboBox->show();
    }
    // update interval modul
    myEdgeDataFrameParent->myIntervalSelector->refreshIntervalSelector();
    return 1;
}

// ---------------------------------------------------------------------------
// GNEEdgeDataFrame::IntervalSelector - methods
// ---------------------------------------------------------------------------

GNEEdgeDataFrame::IntervalSelector::IntervalSelector(GNEEdgeDataFrame* edgeDataFrameParent) :
    FXGroupBox(edgeDataFrameParent->myContentFrame, "Interval", GUIDesignGroupBoxFrame),
    myEdgeDataFrameParent(edgeDataFrameParent) {
    // create radio button for new interval
    myNewIntervalRadioButton = new FXRadioButton(this, "Create new interval", this, MID_GNE_DATAINTERVAL_OPTION, GUIDesignRadioButton);
    // create radio button for existent interval
    mySelectIntervalRadioButton = new FXRadioButton(this, "Use existent interval", this, MID_GNE_DATAINTERVAL_OPTION, GUIDesignRadioButton);
    mySelectIntervalRadioButton->setCheck(TRUE);
    // create begin label
    myHorizontalFrameBegin = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(myHorizontalFrameBegin, toString(SUMO_ATTR_BEGIN).c_str(), nullptr, GUIDesignLabelAttribute);
    // create begin TextField
    myBeginTextField = new FXTextField(myHorizontalFrameBegin, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myBeginTextField->setText("0");
    // hide horizontal frame begin
    myHorizontalFrameBegin->hide();
    // create end label
    myHorizontalFrameEnd = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(myHorizontalFrameEnd, toString(SUMO_ATTR_END).c_str(), nullptr, GUIDesignLabelAttribute);
    // create end textfield
    myEndTextField = new FXTextField(myHorizontalFrameEnd, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myEndTextField->setText("3600");
    // hide horizontal frame end
    myHorizontalFrameEnd->hide();
    // create interval button
    myCreateIntervalButton = new FXButton(this, "create interval", GUIIconSubSys::getIcon(ICON_DATAINTERVAL), this, MID_GNE_CREATE, GUIDesignButton);
    myCreateIntervalButton->hide();
    // Create three list
    myIntervalsTreelist = new FXTreeList(this, this, MID_GNE_SELECT, GUIDesignTreeListFrame);
    // refresh interval selector
    refreshIntervalSelector();
    // IntervalSelector is always shown
    show();
}


GNEEdgeDataFrame::IntervalSelector::~IntervalSelector() {}


void
GNEEdgeDataFrame::IntervalSelector::refreshIntervalSelector() {
    // first clear items
    myIntervalsTreelist->clearItems();
    // obtain data set
    const GNEDataSet *dataSet = myEdgeDataFrameParent->myDataSetSelector->getDataSet();
    // add intervals 
    if (dataSet) {
        // insert data item
        FXTreeItem* dataElementItem = addListItem(dataSet, nullptr);
        // iterate over intevals
        for (const auto &interval : dataSet->getDataIntervalChildren()) {
            addListItem(interval.second, dataElementItem);
        }
    }
    // recalc frame
    recalc();
}


GNEDataInterval*
GNEEdgeDataFrame::IntervalSelector::getDataInterval() const {
    return nullptr;
}


long 
GNEEdgeDataFrame::IntervalSelector::onCmdSelectInterval(FXObject*, FXSelector, void*) {
    return 1;
}


long 
GNEEdgeDataFrame::IntervalSelector::onCmdSetIntervalAttribute(FXObject* obj, FXSelector, void*) {
    if (obj == myBeginTextField) {
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



long 
GNEEdgeDataFrame::IntervalSelector::onCmdSelectRadioButton(FXObject* obj, FXSelector, void*) {
    if (obj == myNewIntervalRadioButton) {
        // set radio buttons
        myNewIntervalRadioButton->setCheck(TRUE, FALSE);
        mySelectIntervalRadioButton->setCheck(FALSE, FALSE);
        // enable begin and end elements
        myHorizontalFrameBegin->show();
        myHorizontalFrameEnd->show();
        myCreateIntervalButton->show();
        // disable list
        myIntervalsTreelist->disable();
    } else if (obj == mySelectIntervalRadioButton) {
        // set radio buttons
        myNewIntervalRadioButton->setCheck(FALSE, FALSE);
        mySelectIntervalRadioButton->setCheck(TRUE, FALSE);
        // disable begin and end elements
        myHorizontalFrameBegin->hide();
        myHorizontalFrameEnd->hide();
        myCreateIntervalButton->hide();
        // enable list
        myIntervalsTreelist->enable();
    }
    // refresh interval seletor
    refreshIntervalSelector();
    return 1;
}


FXTreeItem*
GNEEdgeDataFrame::IntervalSelector::addListItem(const GNEAttributeCarrier* AC, FXTreeItem* itemParent) {
    // insert item in Tree list
    FXTreeItem* item = myIntervalsTreelist->insertItem(nullptr, itemParent, AC->getHierarchyName().c_str(), AC->getIcon(), AC->getIcon());
    // insert item in map
    myTreeItemToACMap[item] = AC;
    // by default item is expanded
    item->setExpanded(true);
    // return created FXTreeItem
    return item;
}

// ---------------------------------------------------------------------------
// GNEEdgeDataFrame - methods
// ---------------------------------------------------------------------------

GNEEdgeDataFrame::GNEEdgeDataFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "EdgeData"),
    myDataSetSelector(nullptr),
    myIntervalSelector(nullptr),
    myParametersEditor(nullptr) {
    // create DataSetSelector
    myDataSetSelector = new DataSetSelector(this);
    // create IntervalSelector
    myIntervalSelector = new IntervalSelector(this);
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
    // first check if we clicked over an edge
    if (objectsUnderCursor.getEdgeFront() && myDataSetSelector->getDataSet() && myIntervalSelector->getDataInterval()) {
        /*
        // declare data set and interval
        GNEDataInterval *dataInterval = nullptr;
        GNEDataSet *dataSet = nullptr;
        // check if we have to create a new dataSet
        if (myDataSetSelector->createNewDataSet()) {
            // check if we can create the new dataSet
            if (myDataSetSelector->getSelectedDataSet().empty()) {
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
        */
        // finally create edgeData
        GNEDataHandler::buildEdgeData(myViewNet, true, myIntervalSelector->getDataInterval(), objectsUnderCursor.getEdgeFront(), myParametersEditor->getParametersMap());
        // edgeData created, then return true
        return true;
    } else {
        // invalid parameters
        return false;
    }
}


void 
GNEEdgeDataFrame::intervalSelected() {
    //
}

/****************************************************************************/
