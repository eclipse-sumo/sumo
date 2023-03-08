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
/// @file    GNETAZRelDataFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    May 2020
///
// The Widget for add TAZRelationData elements
/****************************************************************************/
#include <config.h>

#include <netedit/elements/data/GNEDataHandler.h>
#include <netedit/elements/data/GNEDataInterval.h>
#include <netedit/elements/additional/GNETAZ.h>
#include <netedit/GNEViewNet.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNETAZRelDataFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNETAZRelDataFrame::ConfirmTAZRelation) ConfirmTAZRelationMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_CREATE, GNETAZRelDataFrame::ConfirmTAZRelation::onCmdConfirmTAZRelation),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_CREATE, GNETAZRelDataFrame::ConfirmTAZRelation::onUpdConfirmTAZRelation),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ABORT, GNETAZRelDataFrame::ConfirmTAZRelation::onCmdClearSelection)
};

// Object implementation
FXIMPLEMENT(GNETAZRelDataFrame::ConfirmTAZRelation, MFXGroupBoxModule, ConfirmTAZRelationMap, ARRAYNUMBER(ConfirmTAZRelationMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNETAZRelDataFrame::ConfirmTAZRelation - methods
// ---------------------------------------------------------------------------

GNETAZRelDataFrame::ConfirmTAZRelation::ConfirmTAZRelation(GNETAZRelDataFrame* TAZRelDataFrame) :
    MFXGroupBoxModule(TAZRelDataFrame, TL("Confirm TAZRelation")),
    myTAZRelDataFrame(TAZRelDataFrame) {
    myConfirmTAZButton = new FXButton(getCollapsableFrame(), (TL("Create TAZRelation") + std::string("\t\t") + TL("Click fromTaz and toTaz (confirm hotkey <ENTER>)")).c_str(), GUIIconSubSys::getIcon(GUIIcon::TAZRELDATA), this, MID_GNE_CREATE, GUIDesignButton);
    myConfirmTAZButton->disable();
    myClearTAZButton = new FXButton(getCollapsableFrame(), (TL("Clear selection") + std::string("\t\t") + TL("Clear selected TAZs (hotkey <ESC>)")).c_str(), GUIIconSubSys::getIcon(GUIIcon::CLEARMESSAGEWINDOW), this, MID_GNE_ABORT, GUIDesignButton);
    myClearTAZButton->disable();
}


GNETAZRelDataFrame::ConfirmTAZRelation::~ConfirmTAZRelation() {}


long
GNETAZRelDataFrame::ConfirmTAZRelation::onCmdConfirmTAZRelation(FXObject*, FXSelector, void*) {
    myTAZRelDataFrame->buildTAZRelationData();
    return 1;
}


long
GNETAZRelDataFrame::ConfirmTAZRelation::onUpdConfirmTAZRelation(FXObject*, FXSelector, void*) {
    if (myTAZRelDataFrame->myFirstTAZ && myTAZRelDataFrame->mySecondTAZ) {
        myConfirmTAZButton->enable();
    } else {
        myConfirmTAZButton->disable();
    }
    if (myTAZRelDataFrame->myFirstTAZ || myTAZRelDataFrame->mySecondTAZ) {
        myClearTAZButton->enable();
    } else {
        myClearTAZButton->disable();
    }
    return 1;
}


long
GNETAZRelDataFrame::ConfirmTAZRelation::onCmdClearSelection(FXObject*, FXSelector, void*) {
    myTAZRelDataFrame->clearTAZSelection();
    myTAZRelDataFrame->getViewNet()->update();
    return 1;
}

// ---------------------------------------------------------------------------
// GNETAZRelDataFrame::TAZRelLegend - methods
// ---------------------------------------------------------------------------

GNETAZRelDataFrame::Legend::Legend(GNETAZRelDataFrame* TAZRelDataFrame) :
    MFXGroupBoxModule(TAZRelDataFrame, TL("Information")),
    myFromTAZLabel(nullptr),
    myToTAZLabel(nullptr) {
    // create from TAZ label
    myFromTAZLabel = new FXLabel(getCollapsableFrame(), "From TAZ", 0, GUIDesignLabel(JUSTIFY_LEFT));
    myFromTAZLabel->setBackColor(MFXUtils::getFXColor(RGBColor::GREEN));
    // create to TAZ Label
    myToTAZLabel = new FXLabel(getCollapsableFrame(), "To TAZ", 0, GUIDesignLabel(JUSTIFY_LEFT));
    myToTAZLabel->setBackColor(MFXUtils::getFXColor(RGBColor::MAGENTA));
}


GNETAZRelDataFrame::Legend::~Legend() {}


void
GNETAZRelDataFrame::Legend::setLabels(const GNETAZ* fromTAZ, const GNETAZ* toTAZ) {
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
// GNETAZRelDataFrame - methods
// ------------------------------------------------------------------------

GNETAZRelDataFrame::GNETAZRelDataFrame(GNEViewParent* viewParent, GNEViewNet* viewNet) :
    GNEGenericDataFrame(viewParent, viewNet, SUMO_TAG_TAZREL, false) {
    // create confirm TAZ Relation
    myConfirmTAZRelation = new ConfirmTAZRelation(this);
    // create legend
    myLegend = new Legend(this);
}


GNETAZRelDataFrame::~GNETAZRelDataFrame() {}


bool
GNETAZRelDataFrame::setTAZ(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor) {
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
GNETAZRelDataFrame::buildTAZRelationData() {
    // check conditions
    if (myFirstTAZ && mySecondTAZ) {
        if (!myIntervalSelector->getDataInterval()) {
            WRITE_WARNINGF(TL("A % must be defined within an interval."), toString(SUMO_TAG_TAZREL));
        } else if ((myFirstTAZ == mySecondTAZ) && myIntervalSelector->getDataInterval()->TAZRelExists(myFirstTAZ)) {
            WRITE_WARNINGF(TL("There is already a % defined in TAZ'%'."), toString(SUMO_TAG_TAZREL), myFirstTAZ->getID());
        } else if ((myFirstTAZ != mySecondTAZ) && myIntervalSelector->getDataInterval()->TAZRelExists(myFirstTAZ, mySecondTAZ)) {
            WRITE_WARNINGF(TL("There is already a % defined between TAZ'%' and '%'."), toString(SUMO_TAG_TAZREL), myFirstTAZ->getID(), mySecondTAZ->getID());
        } else if (myGenericDataAttributes->areAttributesValid()) {
            // declare data handler
            GNEDataHandler dataHandler(myViewNet->getNet(), "", true, false);
            // build data interval object and fill it
            CommonXMLStructure::SumoBaseObject* dataIntervalObject = new CommonXMLStructure::SumoBaseObject(nullptr);
            dataIntervalObject->addStringAttribute(SUMO_ATTR_ID, myIntervalSelector->getDataInterval()->getID());
            dataIntervalObject->addDoubleAttribute(SUMO_ATTR_BEGIN, myIntervalSelector->getDataInterval()->getAttributeDouble(SUMO_ATTR_BEGIN));
            dataIntervalObject->addDoubleAttribute(SUMO_ATTR_END, myIntervalSelector->getDataInterval()->getAttributeDouble(SUMO_ATTR_END));
            // create TAZRelData
            CommonXMLStructure::SumoBaseObject* TAZRelData = new CommonXMLStructure::SumoBaseObject(dataIntervalObject);
            // finally create TAZRelationData
            dataHandler.buildTAZRelationData(TAZRelData, myFirstTAZ->getID(), mySecondTAZ->getID(), myGenericDataAttributes->getParametersMap());
            delete dataIntervalObject;
            // reset both TAZs
            myFirstTAZ = nullptr;
            mySecondTAZ = nullptr;
            myLegend->setLabels(myFirstTAZ, mySecondTAZ);
        }
    }
}


GNEAdditional*
GNETAZRelDataFrame::getFirstTAZ() const {
    return myFirstTAZ;
}


GNEAdditional*
GNETAZRelDataFrame::getSecondTAZ() const {
    return mySecondTAZ;
}


void
GNETAZRelDataFrame::clearTAZSelection() {
    myFirstTAZ = nullptr;
    mySecondTAZ = nullptr;
    myLegend->setLabels(myFirstTAZ, mySecondTAZ);
}

/****************************************************************************/
