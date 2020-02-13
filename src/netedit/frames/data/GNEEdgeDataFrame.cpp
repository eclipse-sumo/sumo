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
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/network/GNELane.h>
#include <netedit/elements/network/GNEConnection.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>

#include "GNEEdgeDataFrame.h"

#define NEWINTERVAL "<new interval>"

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
    myEdgeDataFrameParent(edgeDataFrameParent),
    myCreateDataInterval(false) {
    // Create FXComboBox
    myIntervalsComboBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_INTERVAL_SELECTED, GUIDesignComboBox);
    // create new id elements
    myHorizontalFrameBegin = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(myHorizontalFrameBegin, toString(SUMO_ATTR_ID).c_str(), nullptr, GUIDesignLabelAttribute);
    myNewIDTextField = new FXTextField(myHorizontalFrameBegin, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
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
    // Set visible items
    myIntervalsComboBox->setNumVisible((int)myIntervalsComboBox->getNumItems());
    // IntervalSelector is always shown
    show();
}


GNEEdgeDataFrame::IntervalSelector::~IntervalSelector() {}


void
GNEEdgeDataFrame::IntervalSelector::refreshIntervalSelector() {
    // clear items
    myIntervalsComboBox->clearItems();
    // add "<new interval>" option
    myIntervalsComboBox->appendItem(NEWINTERVAL);
    // fill myIntervalsComboBox with all DataSets
    auto dataSetCopy = myEdgeDataFrameParent->getViewNet()->getNet()->retrieveDataSets();
    for (const auto& dataSet : dataSetCopy) {
        myIntervalsComboBox->appendItem(dataSet->getID().c_str());
    }
    // Set visible items
    myIntervalsComboBox->setNumVisible((int)myIntervalsComboBox->getNumItems());
}


std::string 
GNEEdgeDataFrame::IntervalSelector::getIntervalID() const {
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
GNEEdgeDataFrame::IntervalSelector::getCreateDataInterval() const {
    return myCreateDataInterval;
}


std::string 
GNEEdgeDataFrame::IntervalSelector::getBegin() const {
    if (myBeginTextField->getTextColor() == FXRGB(255, 0, 0)) {
        return "";
    } else {
        return myBeginTextField->getText().text();
    }
}


std::string 
GNEEdgeDataFrame::IntervalSelector::getEnd() const {
    if (myEndTextField->getTextColor() == FXRGB(255, 0, 0)) {
        return "";
    } else {
        return myEndTextField->getText().text();
    }
}


long 
GNEEdgeDataFrame::IntervalSelector::onCmdSelectInterval(FXObject*, FXSelector, void*) {
    // get interval ID
    const std::string intervalID = myIntervalsComboBox->getItem(myIntervalsComboBox->getCurrentItem()).text();
    if (intervalID == NEWINTERVAL) {
        // show newID text field
        myNewIDTextField->show();
        // clear current data interval
        myCreateDataInterval = false;
    } else {
        // hide newID text field
        myNewIDTextField->hide();
        // retrieve data set
        myCreateDataInterval = true;
    }
    return 1;
}


long 
GNEEdgeDataFrame::IntervalSelector::onCmdSetAttribute(FXObject* obj, FXSelector, void*) {
    if (obj == myNewIDTextField) {
        // check new ID
        if (myNewIDTextField->getText().empty() || 
            myEdgeDataFrameParent->getViewNet()->getNet()->retrieveDataSet(myNewIDTextField->getText().text(), false) == nullptr) {
            myNewIDTextField->setTextColor(FXRGB(0, 0, 0));
        } else {
            myNewIDTextField->setTextColor(FXRGB(255, 0, 0));
        }
    } else if (obj == myBeginTextField) {
        //
    } else if (obj == myEndTextField) {
        //
    }
    /*
    // Check if value of myTypeMatchBox correspond of an allowed additional tags
    for (const auto& i : myListOfTagTypes) {
        if (i.first == myTagTypesMatchBox->getText().text()) {
            // set color of myTagTypesMatchBox to black (valid)
            myTagTypesMatchBox->setTextColor(FXRGB(0, 0, 0));
            // fill myListOfTags with personTrips (the first Tag Type)
            myListOfTags = GNEAttributeCarrier::allowedTagsByCategory(i.second, true);
            // show and clear myTagsMatchBox
            myTagsMatchBox->show();
            myTagsMatchBox->clearItems();
            // fill myTypeMatchBox with list of tags
            for (const auto& j : myListOfTags) {
                myTagsMatchBox->appendItem(toString(j).c_str());
            }
            // Set visible items
            myTagsMatchBox->setNumVisible((int)myTagsMatchBox->getNumItems());
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected item '" + myTagsMatchBox->getText() + "' in TagTypeSelector").text());
            // call onCmdSelectTag
            return onCmdSelectTag(nullptr, 0, nullptr);
        }
    }
    // if TagType isn't valid, hide myTagsMatchBox
    myTagsMatchBox->hide();
    // if additional name isn't correct, set SUMO_TAG_NOTHING as current type
    myCurrentTagProperties = myInvalidTagProperty;
    // call interval selected function
    myEdgeDataFrameParent->intervalSelected();
    // set color of myTagTypesMatchBox to red (invalid)
    myTagTypesMatchBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Selected invalid item in TagTypeSelector");
    */
    return 1;
}

// ---------------------------------------------------------------------------
// GNEEdgeDataFrame - methods
// ---------------------------------------------------------------------------

GNEEdgeDataFrame::GNEEdgeDataFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "EdgeData") {
    // create IntervalSelector
    myIntervalSelector = new IntervalSelector(this);
}


GNEEdgeDataFrame::~GNEEdgeDataFrame() {}


void
GNEEdgeDataFrame::show() {

    // show frame
    GNEFrame::show();
}


bool
GNEEdgeDataFrame::addEdgeData(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor) {
    return false;
}


void 
GNEEdgeDataFrame::intervalSelected() {
    //
}

/****************************************************************************/
