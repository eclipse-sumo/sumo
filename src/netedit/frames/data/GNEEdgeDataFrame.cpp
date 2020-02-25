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

#define NEWINTERVAL "<new ID interval>"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEEdgeDataFrame::IntervalSelector) IntervalSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_INTERVAL_SELECTED,   GNEEdgeDataFrame::IntervalSelector::onCmdSelectInterval),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_ATTRIBUTE,       GNEEdgeDataFrame::IntervalSelector::onCmdSetAttribute)
};

// Object implementation
FXIMPLEMENT(GNEEdgeDataFrame::IntervalSelector, FXGroupBox, IntervalSelectorMap, ARRAYNUMBER(IntervalSelectorMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEEdgeDataFrame::IntervalSelector - methods
// ---------------------------------------------------------------------------

GNEEdgeDataFrame::IntervalSelector::IntervalSelector(GNEEdgeDataFrame* edgeDataFrameParent) :
    FXGroupBox(edgeDataFrameParent->myContentFrame, "Interval", GUIDesignGroupBoxFrame),
    myEdgeDataFrameParent(edgeDataFrameParent) {
    // Create FXComboBox
    myIntervalsComboBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_INTERVAL_SELECTED, GUIDesignComboBox);
    // create new id elements
    myHorizontalFrameNewID = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(myHorizontalFrameNewID, toString(SUMO_ATTR_ID).c_str(), nullptr, GUIDesignLabelAttribute);
    myNewIDTextField = new FXTextField(myHorizontalFrameNewID, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
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
    // refresh interval selector
    refreshIntervalSelector();
    // IntervalSelector is always shown
    show();
}


GNEEdgeDataFrame::IntervalSelector::~IntervalSelector() {}


void
GNEEdgeDataFrame::IntervalSelector::refreshIntervalSelector() {
    // clear items
    myIntervalsComboBox->clearItems();
    // fill myIntervalsComboBox with all DataSets
    auto dataSetCopy = myEdgeDataFrameParent->getViewNet()->getNet()->retrieveDataSets();
    for (const auto& dataSet : dataSetCopy) {
        myIntervalsComboBox->appendItem(dataSet->getID().c_str());
    }
    // add "<new interval>" option
    myIntervalsComboBox->appendItem(NEWINTERVAL);
    // Set visible items
    myIntervalsComboBox->setNumVisible((int)myIntervalsComboBox->getNumItems());
    // show or hidde horizontal frame new id
    if (myIntervalsComboBox->getItem(0) == NEWINTERVAL) {
        myHorizontalFrameNewID->show();
        // clear text
        myNewIDTextField->setText("", FALSE);
    } else {
        myHorizontalFrameNewID->hide();
    }
    // recalc frame
    recalc();
}


std::string 
GNEEdgeDataFrame::IntervalSelector::getDataSetID() const {
    if (myNewIDTextField->shown()) {
        if (myNewIDTextField->getTextColor() == FXRGB(0, 0, 0)) {
            return myNewIDTextField->getText().text();
        } else {
            return "";
        }
    } else {
        return myIntervalsComboBox->getItem(myIntervalsComboBox->getCurrentItem()).text();
    }
}


bool
GNEEdgeDataFrame::IntervalSelector::createNewDataSet() const {
    return myHorizontalFrameNewID->shown();
}


double
GNEEdgeDataFrame::IntervalSelector::getBegin() const {
    if (myBeginTextField->getTextColor() == FXRGB(255, 0, 0)) {
        return 0;
    } else {
        return GNEAttributeCarrier::parse<double>(myBeginTextField->getText().text());
    }
}


double
GNEEdgeDataFrame::IntervalSelector::getEnd() const {
    if (myEndTextField->getTextColor() == FXRGB(255, 0, 0)) {
        return 0;
    } else {
        return GNEAttributeCarrier::parse<double>(myEndTextField->getText().text());
    }
}


bool 
GNEEdgeDataFrame::IntervalSelector::isIntervalValid() const {
    return (myBeginTextField->getTextColor() == FXRGB(0, 0, 0)) && (myEndTextField->getTextColor() == FXRGB(0, 0, 0));
}


long 
GNEEdgeDataFrame::IntervalSelector::onCmdSelectInterval(FXObject*, FXSelector, void*) {
    // get interval ID
    const std::string intervalID = myIntervalsComboBox->getItem(myIntervalsComboBox->getCurrentItem()).text();
    if (intervalID == NEWINTERVAL) {
        // show newID text field
        myHorizontalFrameNewID->show();
        // clear text
        myNewIDTextField->setText("", FALSE);
    } else {
        // hide newID text field
        myHorizontalFrameNewID->hide();
        // check if given interval exist
    }
    // recalc frame
    recalc();
    return 1;
}


long 
GNEEdgeDataFrame::IntervalSelector::onCmdSetAttribute(FXObject* obj, FXSelector, void*) {
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

// ---------------------------------------------------------------------------
// GNEEdgeDataFrame - methods
// ---------------------------------------------------------------------------

GNEEdgeDataFrame::GNEEdgeDataFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "EdgeData") {
    // create IntervalSelector
    myIntervalSelector = new IntervalSelector(this);
    // create parameter editor
    myParametersEditor = new GNEFrameAttributesModuls::ParametersEditor(this);
}


GNEEdgeDataFrame::~GNEEdgeDataFrame() {}


void
GNEEdgeDataFrame::show() {

    // show frame
    GNEFrame::show();
}


bool
GNEEdgeDataFrame::addEdgeData(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor) {
    // first check if we clicked over an edge
    if (objectsUnderCursor.getEdgeFront() && myIntervalSelector->isIntervalValid()) {
        GNEDataInterval *dataInterval = nullptr;
        GNEDataSet *dataSet = nullptr;
        // check if we have to create a new dataSet
        if (myIntervalSelector->createNewDataSet()) {
            // check if we can create the new dataSet
            if (myIntervalSelector->getDataSetID().empty()) {
                // if obtained dataSet ID is empty, then given ID isn't valid (duplicated/invalid characters, etc.)
                return false;
            } else {
                dataSet = GNEDataHandler::buildDataSet(myViewNet, true, myIntervalSelector->getDataSetID());
                // refresh interval selector
                myIntervalSelector->refreshIntervalSelector();
            }
        } else {
            dataSet = myViewNet->getNet()->retrieveDataSet(myIntervalSelector->getDataSetID());
        }
        // now check if there is another interval with the given begin/end
        dataInterval = dataSet->retrieveInterval(myIntervalSelector->getBegin(), myIntervalSelector->getEnd());
        // if not, create it
        if (dataInterval == nullptr) {
            // check if given interval is valid
            if (dataSet->checkNewInterval(myIntervalSelector->getBegin(), myIntervalSelector->getEnd())) {
                dataInterval = GNEDataHandler::buildDataInterval(myViewNet, true, dataSet, myIntervalSelector->getBegin(), myIntervalSelector->getEnd());
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
}


void 
GNEEdgeDataFrame::intervalSelected() {
    //
}

/****************************************************************************/
