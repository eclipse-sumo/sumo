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
/// @file    GNEFrameModules.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2019
///
// Auxiliar class for GNEFrame Modules
/****************************************************************************/
#include <config.h>

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Children.h>
#include <netedit/elements/additional/GNEAccess.h>
#include <netedit/elements/additional/GNEBusStop.h>
#include <netedit/elements/additional/GNECalibrator.h>
#include <netedit/elements/additional/GNECalibratorFlow.h>
#include <netedit/elements/additional/GNEChargingStation.h>
#include <netedit/elements/additional/GNEClosingLaneReroute.h>
#include <netedit/elements/additional/GNEClosingReroute.h>
#include <netedit/elements/additional/GNEContainerStop.h>
#include <netedit/elements/additional/GNEDestProbReroute.h>
#include <netedit/elements/additional/GNEDetectorE1.h>
#include <netedit/elements/additional/GNEDetectorE1Instant.h>
#include <netedit/elements/additional/GNEDetectorE2.h>
#include <netedit/elements/additional/GNEDetectorE3.h>
#include <netedit/elements/additional/GNEDetectorEntryExit.h>
#include <netedit/elements/additional/GNEPOI.h>
#include <netedit/elements/additional/GNEParkingArea.h>
#include <netedit/elements/additional/GNEParkingAreaReroute.h>
#include <netedit/elements/additional/GNEParkingSpace.h>
#include <netedit/elements/additional/GNEPoly.h>
#include <netedit/elements/additional/GNERerouter.h>
#include <netedit/elements/additional/GNERerouterInterval.h>
#include <netedit/elements/additional/GNERouteProbReroute.h>
#include <netedit/elements/additional/GNERouteProbe.h>
#include <netedit/elements/additional/GNETAZ.h>
#include <netedit/elements/additional/GNETAZSourceSink.h>
#include <netedit/elements/additional/GNEVaporizer.h>
#include <netedit/elements/additional/GNEVariableSpeedSign.h>
#include <netedit/elements/additional/GNEVariableSpeedSignStep.h>
#include <netedit/elements/additional/GNETractionSubstation.h>
#include <netedit/elements/additional/GNEOverheadWire.h>
#include <netedit/elements/data/GNEDataInterval.h>
#include <netedit/elements/demand/GNEContainer.h>
#include <netedit/elements/demand/GNEPerson.h>
#include <netedit/elements/demand/GNEPersonTrip.h>
#include <netedit/elements/demand/GNERide.h>
#include <netedit/elements/demand/GNERoute.h>
#include <netedit/elements/demand/GNEStop.h>
#include <netedit/elements/demand/GNETranship.h>
#include <netedit/elements/demand/GNETransport.h>
#include <netedit/elements/demand/GNEVehicle.h>
#include <netedit/elements/demand/GNEVType.h>
#include <netedit/elements/demand/GNEVTypeDistribution.h>
#include <netedit/elements/demand/GNEWalk.h>
#include <netedit/elements/network/GNEConnection.h>
#include <netedit/elements/network/GNECrossing.h>
#include <netedit/frames/common/GNEInspectorFrame.h>
#include <utils/foxtools/MFXMenuHeader.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEM_SelectorParent.h"
#include "GNEFrame.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNEM_SelectorParent::GNEM_SelectorParent(GNEFrame* frameParent) :
    FXGroupBoxModule(frameParent->getContentFrame(), "Parent selector"),
    myFrameParent(frameParent) {
    // Create label with the type of GNEM_SelectorParent
    myParentsLabel = new FXLabel(getCollapsableFrame(), "No element selected", nullptr, GUIDesignLabelLeftThick);
    // Create list
    myParentsList = new FXList(getCollapsableFrame(), this, MID_GNE_SET_TYPE, GUIDesignListSingleElementFixedHeight);
    // Hide List
    hideSelectorParentModule();
}


GNEM_SelectorParent::~GNEM_SelectorParent() {}


std::string
GNEM_SelectorParent::getIdSelected() const {
    for (int i = 0; i < myParentsList->getNumItems(); i++) {
        if (myParentsList->isItemSelected(i)) {
            return myParentsList->getItem(i)->getText().text();
        }
    }
    return "";
}


void
GNEM_SelectorParent::setIDSelected(const std::string& id) {
    // first unselect all
    for (int i = 0; i < myParentsList->getNumItems(); i++) {
        myParentsList->getItem(i)->setSelected(false);
    }
    // select element if correspond to given ID
    for (int i = 0; i < myParentsList->getNumItems(); i++) {
        if (myParentsList->getItem(i)->getText().text() == id) {
            myParentsList->getItem(i)->setSelected(true);
        }
    }
    // recalc myFirstParentsList
    myParentsList->recalc();
}


void
GNEM_SelectorParent::showSelectorParentModule(const std::vector<SumoXMLTag>& parentTags) {
    if (parentTags.size() > 0) {
        myParentTags = parentTags;
        myParentsLabel->setText(("Parent type: " + toString(parentTags.front())).c_str());
        refreshSelectorParentModule();
        show();
    } else {
        myParentTags.clear();
        hide();
    }
}


void
GNEM_SelectorParent::hideSelectorParentModule() {
    myParentTags.clear();
    hide();
}


void
GNEM_SelectorParent::refreshSelectorParentModule() {
    // save current edited elements
    std::set<std::string> selectedItems;
    for (int i = 0; i < myParentsList->getNumItems(); i++) {
        if (myParentsList->isItemSelected(i)) {
            selectedItems.insert(myParentsList->getItem(i)->getText().text());
        }
    }
    myParentsList->clearItems();
    if (myParentTags.size() > 0) {
        // insert additionals sorted
        std::set<std::string> IDs;
        // fill list with IDs
        for (const auto& parentTag : myParentTags) {
            // check type
            const auto tagProperty = GNEAttributeCarrier::getTagProperty(parentTag);
            // additionals
            if (tagProperty.isAdditionalElement()) {
                for (const auto& additional : myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getAdditionals().at(parentTag)) {
                    IDs.insert(additional->getID().c_str());
                }
            }
        }
        // fill list with IDs
        for (const auto& ID : IDs) {
            const int item = myParentsList->appendItem(ID.c_str());
            if (selectedItems.find(ID) != selectedItems.end()) {
                myParentsList->selectItem(item);
            }
        }
    }
}

/****************************************************************************/
