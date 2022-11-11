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

#include <netedit/elements/data/GNEDataHandler.h>
#include <netedit/elements/data/GNEDataInterval.h>
#include <netedit/elements/additional/GNETAZ.h>
#include <netedit/GNEViewNet.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEMeanDataFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEMeanDataFrame::ConfirmTAZRelation) ConfirmTAZRelationMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_CREATE, GNEMeanDataFrame::ConfirmTAZRelation::onCmdConfirmTAZRelation),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_CREATE, GNEMeanDataFrame::ConfirmTAZRelation::onUpdConfirmTAZRelation),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ABORT, GNEMeanDataFrame::ConfirmTAZRelation::onCmdClearSelection)
};

// Object implementation
FXIMPLEMENT(GNEMeanDataFrame::ConfirmTAZRelation, MFXGroupBoxModule, ConfirmTAZRelationMap, ARRAYNUMBER(ConfirmTAZRelationMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEMeanDataFrame::ConfirmTAZRelation - methods
// ---------------------------------------------------------------------------

GNEMeanDataFrame::ConfirmTAZRelation::ConfirmTAZRelation(GNEMeanDataFrame* MeanDataFrame) :
    MFXGroupBoxModule(MeanDataFrame, TL("Confirm TAZRelation")),
    myMeanDataFrame(MeanDataFrame) {
    myConfirmTAZButton = new FXButton(getCollapsableFrame(), TL("Create TAZRelation\t\tClick fromTaz and toTaz (confirm hotkey <ENTER>)"), GUIIconSubSys::getIcon(GUIIcon::TAZRELDATA), this, MID_GNE_CREATE, GUIDesignButton);
    myConfirmTAZButton->disable();
    myClearTAZButton = new FXButton(getCollapsableFrame(), TL("Clear selection\t\tClear selected TAZs (hotkey <ESC>)"), GUIIconSubSys::getIcon(GUIIcon::CLEARMESSAGEWINDOW), this, MID_GNE_ABORT, GUIDesignButton);
    myClearTAZButton->disable();
}


GNEMeanDataFrame::ConfirmTAZRelation::~ConfirmTAZRelation() {}


long
GNEMeanDataFrame::ConfirmTAZRelation::onCmdConfirmTAZRelation(FXObject*, FXSelector, void*) {
    myMeanDataFrame->buildTAZRelationData();
    return 1;
}


long
GNEMeanDataFrame::ConfirmTAZRelation::onUpdConfirmTAZRelation(FXObject*, FXSelector, void*) {
    if (myMeanDataFrame->myFirstTAZ && myMeanDataFrame->mySecondTAZ) {
        myConfirmTAZButton->enable();
    } else {
        myConfirmTAZButton->disable();
    }
    if (myMeanDataFrame->myFirstTAZ || myMeanDataFrame->mySecondTAZ) {
        myClearTAZButton->enable();
    } else {
        myClearTAZButton->disable();
    }
    return 1;
}


long
GNEMeanDataFrame::ConfirmTAZRelation::onCmdClearSelection(FXObject*, FXSelector, void*) {
    myMeanDataFrame->clearTAZSelection();
    myMeanDataFrame->getViewNet()->update();
    return 1;
}

// ---------------------------------------------------------------------------
// GNEMeanDataFrame::TAZRelLegend - methods
// ---------------------------------------------------------------------------

GNEMeanDataFrame::Legend::Legend(GNEMeanDataFrame* MeanDataFrame) :
    MFXGroupBoxModule(MeanDataFrame, TL("Information")),
    myFromTAZLabel(nullptr),
    myToTAZLabel(nullptr) {
    // create from TAZ label
    myFromTAZLabel = new FXLabel(getCollapsableFrame(), "From TAZ", 0, GUIDesignLabelLeft);
    myFromTAZLabel->setBackColor(MFXUtils::getFXColor(RGBColor::GREEN));
    // create to TAZ Label
    myToTAZLabel = new FXLabel(getCollapsableFrame(), "To TAZ", 0, GUIDesignLabelLeft);
    myToTAZLabel->setBackColor(MFXUtils::getFXColor(RGBColor::MAGENTA));
}


GNEMeanDataFrame::Legend::~Legend() {}


void
GNEMeanDataFrame::Legend::setLabels(const GNETAZ* fromTAZ, const GNETAZ* toTAZ) {
    // from TAZ
    if (fromTAZ) {
        myFromTAZLabel->setText(("From TAZ: " + fromTAZ->getID()).c_str());
    } else {
        myFromTAZLabel->setText(TL("From TAZ"));
    }
    // to TAZ
    if (toTAZ) {
        myToTAZLabel->setText(("To TAZ: " + toTAZ->getID()).c_str());
    } else {
        myToTAZLabel->setText(TL("To TAZ"));
    }
}

// ---------------------------------------------------------------------------
// GNEMeanDataFrame - methods
// ------------------------------------------------------------------------

GNEMeanDataFrame::GNEMeanDataFrame(GNEViewParent* viewParent, GNEViewNet* viewNet) :
    GNEGenericDataFrame(viewParent, viewNet, SUMO_TAG_TAZREL, false) {
    // create confirm TAZ Relation
    myConfirmTAZRelation = new ConfirmTAZRelation(this);
    // create legend
    myLegend = new Legend(this);
}


GNEMeanDataFrame::~GNEMeanDataFrame() {}


bool
GNEMeanDataFrame::setTAZ(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor) {
    // check if myFirstTAZElement is empty
    if (myFirstTAZ) {
        if (mySecondTAZ) {
            // both already defined
            return false;
        } else if (objectsUnderCursor.getTAZFront()) {
            mySecondTAZ = objectsUnderCursor.getTAZFront();
            myLegend->setLabels(myFirstTAZ, mySecondTAZ);
            return true;
        } else {
            return false;
        }
    } else if (objectsUnderCursor.getTAZFront()) {
        myFirstTAZ = objectsUnderCursor.getTAZFront();
        myLegend->setLabels(myFirstTAZ, mySecondTAZ);
        return true;
    } else {
        return false;
    }
}


void
GNEMeanDataFrame::buildTAZRelationData() {
    // check conditions
    if (myFirstTAZ && mySecondTAZ) {
        if (!myIntervalSelector->getDataInterval()) {
            WRITE_WARNING("A " + toString(SUMO_TAG_TAZREL) + " must be defined within an interval.");
        } else if ((myFirstTAZ == mySecondTAZ) && myIntervalSelector->getDataInterval()->TAZRelExists(myFirstTAZ)) {
            WRITE_WARNING("There is already a " + toString(SUMO_TAG_TAZREL) + " defined in TAZ'" + myFirstTAZ->getID() + "'.");
        } else if ((myFirstTAZ != mySecondTAZ) && myIntervalSelector->getDataInterval()->TAZRelExists(myFirstTAZ, mySecondTAZ)) {
            WRITE_WARNING("There is already a " + toString(SUMO_TAG_TAZREL) + " defined between TAZ'" + myFirstTAZ->getID() + "' and '" + mySecondTAZ->getID() + "'.");
        } else if (myGenericDataAttributes->areAttributesValid()) {
            // declare data handler
            GNEDataHandler dataHandler(myViewNet->getNet(), "", true);
            // build data interval object and fill it
            CommonXMLStructure::SumoBaseObject* dataIntervalObject = new CommonXMLStructure::SumoBaseObject(nullptr);
            dataIntervalObject->addStringAttribute(SUMO_ATTR_ID, myIntervalSelector->getDataInterval()->getID());
            dataIntervalObject->addDoubleAttribute(SUMO_ATTR_BEGIN, myIntervalSelector->getDataInterval()->getAttributeDouble(SUMO_ATTR_BEGIN));
            dataIntervalObject->addDoubleAttribute(SUMO_ATTR_END, myIntervalSelector->getDataInterval()->getAttributeDouble(SUMO_ATTR_END));
            // create MeanData
            CommonXMLStructure::SumoBaseObject* MeanData = new CommonXMLStructure::SumoBaseObject(dataIntervalObject);
            // finally create TAZRelationData
            dataHandler.buildTAZRelationData(MeanData, myFirstTAZ->getID(), mySecondTAZ->getID(), myGenericDataAttributes->getParametersMap());
            delete dataIntervalObject;
            // reset both TAZs
            myFirstTAZ = nullptr;
            mySecondTAZ = nullptr;
            myLegend->setLabels(myFirstTAZ, mySecondTAZ);
        }
    }
}


GNEAdditional*
GNEMeanDataFrame::getFirstTAZ() const {
    return myFirstTAZ;
}


GNEAdditional*
GNEMeanDataFrame::getSecondTAZ() const {
    return mySecondTAZ;
}


void
GNEMeanDataFrame::clearTAZSelection() {
    myFirstTAZ = nullptr;
    mySecondTAZ = nullptr;
    myLegend->setLabels(myFirstTAZ, mySecondTAZ);
}

/****************************************************************************/
