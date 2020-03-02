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
#include <netedit/elements/data/GNEEdgeData.h>
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
    FXMAPFUNC(SEL_COMMAND, MID_GNE_CREATE,              GNEEdgeDataFrame::DataSetSelector::onCmdCreateDataSet),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DATASET_NEW,         GNEEdgeDataFrame::DataSetSelector::onCmdSetNewDataSetID),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DATASET_SELECTED,    GNEEdgeDataFrame::DataSetSelector::onCmdSelectDataSet),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SELECT,              GNEEdgeDataFrame::DataSetSelector::onCmdSelectCheckButton)

};

FXDEFMAP(GNEEdgeDataFrame::IntervalSelector) IntervalSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_CREATE,                  GNEEdgeDataFrame::IntervalSelector::onCmdCreateInterval),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DATAINTERVAL_SELECTED,   GNEEdgeDataFrame::IntervalSelector::onCmdSelectInterval),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_ATTRIBUTE,           GNEEdgeDataFrame::IntervalSelector::onCmdSetIntervalAttribute),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SELECT,                  GNEEdgeDataFrame::IntervalSelector::onCmdSelectCheckButton)
};

FXDEFMAP(GNEEdgeDataFrame::AttributeSelector) AttributeSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SELECT,  GNEEdgeDataFrame::AttributeSelector::onCmdSelectAttribute)
};

// Object implementation
FXIMPLEMENT(GNEEdgeDataFrame::DataSetSelector,   FXGroupBox, DataSetSelectorMap,   ARRAYNUMBER(DataSetSelectorMap))
FXIMPLEMENT(GNEEdgeDataFrame::IntervalSelector,  FXGroupBox, IntervalSelectorMap,  ARRAYNUMBER(IntervalSelectorMap))
FXIMPLEMENT(GNEEdgeDataFrame::AttributeSelector, FXGroupBox, AttributeSelectorMap, ARRAYNUMBER(AttributeSelectorMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEEdgeDataFrame::DataSetSelector - methods
// ---------------------------------------------------------------------------

GNEEdgeDataFrame::DataSetSelector::DataSetSelector(GNEEdgeDataFrame* edgeDataFrameParent) :
    FXGroupBox(edgeDataFrameParent->myContentFrame, "DataSet", GUIDesignGroupBoxFrame),
    myEdgeDataFrameParent(edgeDataFrameParent) {
    // create check button for new data set
    myNewDataSetCheckButton = new FXCheckButton(this, "Create new dataSet", this, MID_GNE_SELECT, GUIDesignCheckButton);
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
    refreshDataSetSelector(nullptr);
    // DataSetSelector is always shown
    show();
}


GNEEdgeDataFrame::DataSetSelector::~DataSetSelector() {}


void
GNEEdgeDataFrame::DataSetSelector::refreshDataSetSelector(const GNEDataSet *currentDataSet) {
    // clear items
    myDataSetsComboBox->clearItems();
    // declare item index
    int currentItemIndex = -1;
    // fill myDataSetsComboBox with all DataSets
    auto dataSetCopy = myEdgeDataFrameParent->getViewNet()->getNet()->retrieveDataSets();
    for (const auto& dataSet : dataSetCopy) {
        // check if we have to set currentItemIndex
        if ((currentItemIndex == -1) && (dataSet == currentDataSet)) {
            currentItemIndex = myDataSetsComboBox->getNumItems();
        }
        myDataSetsComboBox->appendItem(dataSet->getID().c_str());
    }
    // Set visible items
    myDataSetsComboBox->setNumVisible((int)myDataSetsComboBox->getNumItems());
    // check if we have to set current element
    if(currentItemIndex != -1) {
        myDataSetsComboBox->setCurrentItem(currentItemIndex, FALSE);
    }
    // recalc frame
    recalc();
    // refresh interval selector
    if (myEdgeDataFrameParent->myIntervalSelector) {
        myEdgeDataFrameParent->myIntervalSelector->refreshIntervalSelector();
    }
}


GNEDataSet*
GNEEdgeDataFrame::DataSetSelector::getDataSet() const {
    if ((myNewDataSetCheckButton->getCheck() == TRUE) || (myDataSetsComboBox->getNumItems() == 0)) {
        return nullptr;
    } else {
        return myEdgeDataFrameParent->getViewNet()->getNet()->retrieveDataSet(myDataSetsComboBox->getItem(myDataSetsComboBox->getCurrentItem()).text(), false);
    }
}


long 
GNEEdgeDataFrame::DataSetSelector::onCmdCreateDataSet(FXObject*, FXSelector, void*) {
    // get string
    const std::string dataSetID = myNewDataSetIDTextField->getText().text();
    // check conditions 
    if (myNewDataSetIDTextField->getTextColor() == FXRGB(255, 0, 0)) {
        WRITE_WARNING("Invalid dataSet ID");
    } else if (dataSetID.empty()) {
        WRITE_WARNING("Invalid empty dataSet ID");
    } else if (myEdgeDataFrameParent->getViewNet()->getNet()->retrieveDataSet(dataSetID, false) != nullptr) {
        WRITE_WARNING("Invalid duplicated dataSet ID");
    } else {
        // build data set
        const GNEDataSet *dataSet = GNEDataHandler::buildDataSet(myEdgeDataFrameParent->getViewNet(), true, dataSetID);
        // refresh tag selector
        refreshDataSetSelector(dataSet);
        // change check button
        myNewDataSetCheckButton->setCheck(FALSE, TRUE);
    }
    return 1;
}


long 
GNEEdgeDataFrame::DataSetSelector::onCmdSetNewDataSetID(FXObject*, FXSelector, void*) {
    //
    return 1;
}


long 
GNEEdgeDataFrame::DataSetSelector::onCmdSelectDataSet(FXObject*, FXSelector, void*) {
    // update interval modul
    myEdgeDataFrameParent->myIntervalSelector->refreshIntervalSelector();
    return 1;
}


long 
GNEEdgeDataFrame::DataSetSelector::onCmdSelectCheckButton(FXObject*, FXSelector, void*) {
    if (myNewDataSetCheckButton->getCheck() == TRUE) {
        // enable textfield and label
        myHorizontalFrameNewID->show();
        myCreateDataSetButton->show();
        // disable comboBox
        myDataSetsComboBox->hide();
    } else {
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
    // create check button for new interval
    myNewIntervalCheckButton = new FXCheckButton(this, "Create new interval", this, MID_GNE_SELECT, GUIDesignCheckButton);
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
    myIntervalsTreelist = new FXTreeList(this, this, MID_GNE_DATAINTERVAL_SELECTED, GUIDesignTreeListFrame);
    // refresh interval selector
    refreshIntervalSelector();
    // IntervalSelector is always shown
    show();
}


GNEEdgeDataFrame::IntervalSelector::~IntervalSelector() {}


void
GNEEdgeDataFrame::IntervalSelector::refreshIntervalSelector() {
    // first clear items from tree and intervalMap
    myIntervalsTreelist->clearItems();
    myTreeItemIntervalMap.clear();
    // obtain data set
    const GNEDataSet *dataSet = myEdgeDataFrameParent->myDataSetSelector->getDataSet();
    // add intervals 
    if (dataSet) {
        // insert dataSetItem in Tree list
        FXTreeItem* dataSetItem = myIntervalsTreelist->insertItem(nullptr, nullptr, 
            dataSet->getHierarchyName().c_str(), 
            dataSet->getIcon(), 
            dataSet->getIcon());
        // by default item is expanded
        dataSetItem->setExpanded(true);
        // iterate over intevals
        for (const auto &interval : dataSet->getDataIntervalChildren()) {
            addListItem(interval.second, dataSetItem);
        }
    }
    // refresh attribute selector
    if (myEdgeDataFrameParent->myAttributeSelector) {
        myEdgeDataFrameParent->myAttributeSelector->refreshAttributeSelector();
    }
    // recalc frame
    recalc();
}


GNEDataInterval*
GNEEdgeDataFrame::IntervalSelector::getDataInterval() const {
    // first check if there is elements in interval tree
    if (myIntervalsTreelist->getNumItems() > 0) {
        for (const auto &treeItem : myTreeItemIntervalMap) {
            if (treeItem.first->isSelected()) {
                return treeItem.second;
            }
        }
    }
    // no GNEDataInterval found, then return nullptr 
    return nullptr;
}


long 
GNEEdgeDataFrame::IntervalSelector::onCmdCreateInterval(FXObject*, FXSelector, void*) {
    // first check that begin and end are valid
    if (GNEAttributeCarrier::canParse<double>(myBeginTextField->getText().text()) &&
        GNEAttributeCarrier::canParse<double>(myEndTextField->getText().text())) {
        // obtain begin and end
        const double begin = GNEAttributeCarrier::parse<double>(myBeginTextField->getText().text());
        const double end = GNEAttributeCarrier::parse<double>(myEndTextField->getText().text());
        // get data set parent
        GNEDataSet *dataSet = myEdgeDataFrameParent->myDataSetSelector->getDataSet();
        if (dataSet && dataSet->checkNewInterval(begin, end)) {
            GNEDataHandler::buildDataInterval(myEdgeDataFrameParent->getViewNet(), true, dataSet, begin, end);
        }
        // disable select interval check button
        myNewIntervalCheckButton->setCheck(FALSE, TRUE);
    }
    return 1;
}


long 
GNEEdgeDataFrame::IntervalSelector::onCmdSelectInterval(FXObject*, FXSelector, void*) {
    // refresh attribute selector
    myEdgeDataFrameParent->myAttributeSelector->refreshAttributeSelector();
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
GNEEdgeDataFrame::IntervalSelector::onCmdSelectCheckButton(FXObject* obj, FXSelector, void*) {
    if (myNewIntervalCheckButton->getCheck() == TRUE) {
        // enable begin and end elements
        myHorizontalFrameBegin->show();
        myHorizontalFrameEnd->show();
        myCreateIntervalButton->show();
        // refresh begin and end text fields
        const GNEDataSet *dataSet = myEdgeDataFrameParent->myDataSetSelector->getDataSet();
        if (dataSet) {
            if (dataSet->getDataIntervalChildren().empty()) {
                // set default interval (1 hour)
                myBeginTextField->setText("0");
                myEndTextField->setText("3600");
            } else {
                // obtain last data interval
                const GNEDataInterval *lastDataInterval = dataSet->getDataIntervalChildren().rbegin()->second;
                const double intervalDuration = lastDataInterval->getAttributeDouble(SUMO_ATTR_END) - lastDataInterval->getAttributeDouble(SUMO_ATTR_BEGIN);
                // set new begin end
                myBeginTextField->setText(toString(lastDataInterval->getAttributeDouble(SUMO_ATTR_END)).c_str());
                myEndTextField->setText(toString(lastDataInterval->getAttributeDouble(SUMO_ATTR_END) + intervalDuration).c_str());
            }
        }
    } else {
        // disable begin and end elements
        myHorizontalFrameBegin->hide();
        myHorizontalFrameEnd->hide();
        myCreateIntervalButton->hide();
    }
    // refresh interval seletor
    refreshIntervalSelector();
    return 1;
}


FXTreeItem*
GNEEdgeDataFrame::IntervalSelector::addListItem(GNEDataInterval* dataInterval, FXTreeItem* itemParent) {
    // insert item in Tree list
    FXTreeItem* item = myIntervalsTreelist->insertItem(nullptr, itemParent, 
        dataInterval->getHierarchyName().c_str(), 
        dataInterval->getIcon(), 
        dataInterval->getIcon());
    // insert item in map
    myTreeItemIntervalMap[item] = dataInterval;
    // by default item is expanded
    item->setExpanded(true);
    // return created FXTreeItem
    return item;
}

// ---------------------------------------------------------------------------
// GNEEdgeDataFrame::AttributeSelector - methods
// ---------------------------------------------------------------------------

GNEEdgeDataFrame::AttributeSelector::AttributeSelector(GNEEdgeDataFrame* edgeDataFrameParent) :
    FXGroupBox(edgeDataFrameParent->myContentFrame, "Data attributes", GUIDesignGroupBoxFrame),
    myEdgeDataFrameParent(edgeDataFrameParent) {
    // Create FXComboBox
    myAttributesComboBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SELECT, GUIDesignComboBox);
    // refresh interval selector
    refreshAttributeSelector();
    // AttributeSelector is always shown
    show();
}


GNEEdgeDataFrame::AttributeSelector::~AttributeSelector() {}


void
GNEEdgeDataFrame::AttributeSelector::refreshAttributeSelector() {
    // first clear items
    myAttributesComboBox->clearItems();
    // fill myAttributesComboBox depending of data sets
    if (myEdgeDataFrameParent->myDataSetSelector->getDataSet() == nullptr) {
        myAttributesComboBox->appendItem("<no dataSet selected>");
        myAttributesComboBox->disable();
    } else {
        // add all item
        myAttributesComboBox->appendItem("<all>");
        // add attributes depending of interval
        if (myEdgeDataFrameParent->myIntervalSelector->getDataInterval() == nullptr) {
            const auto parameters = myEdgeDataFrameParent->getViewNet()->getNet()->retrieveGenericDataParameters(
                myEdgeDataFrameParent->myDataSetSelector->getDataSet()->getID(), "", "");
            // add all parameters
            for (const auto &attribute : parameters) {
                myAttributesComboBox->appendItem(attribute.c_str());
            }
        } else {
            // retrieve all parameters within begin and end
            const auto parameters = myEdgeDataFrameParent->getViewNet()->getNet()->retrieveGenericDataParameters(
                myEdgeDataFrameParent->myDataSetSelector->getDataSet()->getID(), 
                myEdgeDataFrameParent->myIntervalSelector->getDataInterval()->getAttribute(SUMO_ATTR_BEGIN), 
                myEdgeDataFrameParent->myIntervalSelector->getDataInterval()->getAttribute(SUMO_ATTR_END));
            // add all parameters
            for (const auto &attribute : parameters) {
                myAttributesComboBox->appendItem(attribute.c_str());
            }
        }
        // enable combo Box
        myAttributesComboBox->enable();
        // adjust visible items
        if (myAttributesComboBox->getNumItems() < 10) {
            myAttributesComboBox->setNumVisible(myAttributesComboBox->getNumItems());
        } else {
            myAttributesComboBox->setNumVisible(10);
        }
    }
    // recalc frame
    recalc();
    // update view net
    myEdgeDataFrameParent->getViewNet()->update();
}


long 
GNEEdgeDataFrame::AttributeSelector::onCmdSelectAttribute(FXObject*, FXSelector, void*) {
    return 1;
}

// ---------------------------------------------------------------------------
// GNEEdgeDataFrame - methods
// ---------------------------------------------------------------------------

GNEEdgeDataFrame::GNEEdgeDataFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "EdgeData"),
    myDataSetSelector(nullptr),
    myIntervalSelector(nullptr),
    myAttributeSelector(nullptr),
    myParametersEditor(nullptr) {
    // create DataSetSelector
    myDataSetSelector = new DataSetSelector(this);
    // create IntervalSelector
    myIntervalSelector = new IntervalSelector(this);
    // create AttributeSelector
    myAttributeSelector = new AttributeSelector(this);
    // create parameter editor
    myParametersEditor = new GNEFrameAttributesModuls::ParametersEditor(this, "Attributes");
}


GNEEdgeDataFrame::~GNEEdgeDataFrame() {}


void
GNEEdgeDataFrame::show() {
    // first refresh data set selector
    myDataSetSelector->refreshDataSetSelector(nullptr);
    // show frame
    GNEFrame::show();
}


bool
GNEEdgeDataFrame::addEdgeData(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor) {
    // first check if we clicked over an edge
    if (objectsUnderCursor.getEdgeFront() && myDataSetSelector->getDataSet() && myIntervalSelector->getDataInterval()) {
        // first check if the given interval there is already a EdgeData for the given ID
        for (const auto &genericData : myIntervalSelector->getDataInterval()->getGenericDataChildren()) {
            if ((genericData->getTagProperty().getTag() == SUMO_TAG_MEANDATA_EDGE) && (genericData->getParentEdges().front() == objectsUnderCursor.getEdgeFront())) {
                // write warning
                WRITE_WARNING("There is already a " + genericData->getTagStr() + " in edge '" + objectsUnderCursor.getEdgeFront()->getID() + "'");
                // abort edge data creation
                return false;
            }
        }
        // finally create edgeData
        GNEDataHandler::buildEdgeData(myViewNet, true, myIntervalSelector->getDataInterval(), objectsUnderCursor.getEdgeFront(), myParametersEditor->getParametersMap());
        // edgeData created, then return true
        return true;
    } else {
        // invalid parent parameters
        return false;
    }
}


const GNEEdgeDataFrame::DataSetSelector*
GNEEdgeDataFrame::getDataSetSelector() const {
    return myDataSetSelector;
}


const GNEEdgeDataFrame::IntervalSelector*
GNEEdgeDataFrame::getIntervalSelector() const {
    return myIntervalSelector;
}


const GNEEdgeDataFrame::AttributeSelector*
GNEEdgeDataFrame::getAttributeSelector() const {
    return myAttributeSelector;
}


void 
GNEEdgeDataFrame::intervalSelected() {
    //
}

/****************************************************************************/
