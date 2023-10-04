/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
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
/// @file    GNEPlanSelector.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2023
///
// Frame for select person/container plans
/****************************************************************************/
#include <config.h>

#include <netedit/GNEViewNet.h>
#include <netedit/elements/additional/GNEAccess.h>
#include <netedit/elements/additional/GNEBusStop.h>
#include <netedit/elements/additional/GNECalibrator.h>
#include <netedit/elements/additional/GNECalibratorFlow.h>
#include <netedit/elements/additional/GNEChargingStation.h>
#include <netedit/elements/additional/GNEClosingLaneReroute.h>
#include <netedit/elements/additional/GNEClosingReroute.h>
#include <netedit/elements/additional/GNEContainerStop.h>
#include <netedit/elements/additional/GNEDestProbReroute.h>
#include <netedit/elements/additional/GNEInductionLoopDetector.h>
#include <netedit/elements/additional/GNEInstantInductionLoopDetector.h>
#include <netedit/elements/additional/GNELaneAreaDetector.h>
#include <netedit/elements/additional/GNEMultiEntryExitDetector.h>
#include <netedit/elements/additional/GNEEntryExitDetector.h>
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
#include <netedit/frames/common/GNEInspectorFrame.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEPlanSelector.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEPlanSelector) TagSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_TAG_SELECTED, GNEPlanSelector::onCmdSelectPlan)
};

// Object implementation
FXIMPLEMENT(GNEPlanSelector, MFXGroupBoxModule, TagSelectorMap, ARRAYNUMBER(TagSelectorMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNEPlanSelector::GNEPlanSelector(GNEFrame* frameParent) :
    MFXGroupBoxModule(frameParent, TL("Person plans")),
    myFrameParent(frameParent) {
    // Create MFXComboBoxIcon
    myPlansComboBox = new MFXComboBoxIcon(getCollapsableFrame(), GUIDesignComboBoxNCol, false, GUIDesignComboBoxVisibleItemsLarge,
                                          this, MID_GNE_TAG_SELECTED, GUIDesignComboBox);
    // set list of plans
    myPlans = {"PersonTrip", "Ride", "Walk", "Walk (Edges)", "Walk (Route)"};
    // add person plan elements
    myPlansComboBox->appendIconItem(myPlans[0], GUIIconSubSys::getIcon(GUIIcon::PERSONTRIP_EDGE), FXRGBA(253, 255, 206, 255));
    myPlansComboBox->appendIconItem(myPlans[1], GUIIconSubSys::getIcon(GUIIcon::RIDE_EDGE), FXRGBA(210, 233, 255, 255));
    myPlansComboBox->appendIconItem(myPlans[2], GUIIconSubSys::getIcon(GUIIcon::WALK_EDGE), FXRGBA(240, 255, 205, 255));
    myPlansComboBox->appendIconItem(myPlans[3], GUIIconSubSys::getIcon(GUIIcon::WALK_EDGES), FXRGBA(240, 255, 205, 255));
    myPlansComboBox->appendIconItem(myPlans[4], GUIIconSubSys::getIcon(GUIIcon::WALK_ROUTE), FXRGBA(240, 255, 205, 255));
    // set color of myTypeMatchBox to black (valid)
    myPlansComboBox->setTextColor(FXRGB(0, 0, 0));
    myPlansComboBox->killFocus();
    // GNEPlanSelector is always shown
    show();
}


GNEPlanSelector::~GNEPlanSelector() {
}


void
GNEPlanSelector::showPlanSelector() {
    show();
}


void
GNEPlanSelector::hidePlanSelector() {
    hide();
}


bool
GNEPlanSelector::isPlanValid() const {
    return myPlansComboBox->getTextColor() == FXRGB(0, 0, 0);
}


void
GNEPlanSelector::refreshTagSelector() {
    if (isPlanValid()) {
        // call tag selected function
        myFrameParent->tagSelected();
    } else {
        // set first item
        myPlansComboBox->setCurrentItem(0, TRUE);
    }
}


bool
GNEPlanSelector::markRoutes() const {
    // first check if this modul is shown and selected plan is valid
    if (isPlanMode() && isPlanValid()) {
        // only for plan over routes
        return (myPlansComboBox->getText() == myPlans[4]);
    }
    return false;
}


bool
GNEPlanSelector::markContinuousEdges() const {
    // first check if this modul is shown and selected plan is valid
    if (isPlanMode() && isPlanValid()) {
        // only for plan over continuousEdges
        return (myPlansComboBox->getText() == myPlans[3]);
    }
    return false;
}


bool
GNEPlanSelector::markSingleEdges() const {
    // first check if this modul is shown and selected plan is valid
    if (isPlanMode() && isPlanValid()) {
        // for all plan routes and continous edges
        return (myPlansComboBox->getText() != myPlans[3]) &&
               (myPlansComboBox->getText() != myPlans[4]);
    }
    return false;
}


bool
GNEPlanSelector::markJunctions() const {
    // first check if this modul is shown and selected plan is valid
    if (isPlanMode() && isPlanValid()) {
        // for all plan except rides, routes and continous edges
        return (myPlansComboBox->getText() != myPlans[1]) &&
               (myPlansComboBox->getText() != myPlans[3]) &&
               (myPlansComboBox->getText() != myPlans[4]);
    }
    return false;
}


bool
GNEPlanSelector::markBusStops() const {
    // first check if this modul is shown and selected plan is valid
    if (isPlanMode() && isPlanValid()) {
        // for all plan routes and continous edges
        return (myPlansComboBox->getText() != myPlans[3]) &&
               (myPlansComboBox->getText() != myPlans[4]);
    }
    return false;
}


bool
GNEPlanSelector::markTrainStops() const {
    // first check if this modul is shown and selected plan is valid
    if (isPlanMode() && isPlanValid()) {
        // for all plan routes and continous edges
        return (myPlansComboBox->getText() != myPlans[3]) &&
               (myPlansComboBox->getText() != myPlans[4]);
    }
    return false;
}


bool
GNEPlanSelector::markTAZs() const {
    // first check if this modul is shown and selected plan is valid
    if (isPlanMode() && isPlanValid()) {
        // for all plan except rides, routes and continous edges
        return (myPlansComboBox->getText() != myPlans[1]) &&
               (myPlansComboBox->getText() != myPlans[3]) &&
               (myPlansComboBox->getText() != myPlans[4]);
    }
    return false;
}


long
GNEPlanSelector::onCmdSelectPlan(FXObject*, FXSelector, void*) {
    // check if selected plan of comboBox exists in plans
    if (std::find(myPlans.begin(), myPlans.end(), myPlansComboBox->getText()) != myPlans.end()) {
        // set color of myTypeMatchBox to black (valid)
        myPlansComboBox->setTextColor(FXRGB(0, 0, 0));
        myPlansComboBox->killFocus();
        // call tag selected function
        myFrameParent->tagSelected();
        // Write Warning in console if we're in testing mode
        WRITE_DEBUG(("Selected item '" + myPlansComboBox->getText() + "' in GNEPlanSelector").text());
    } else {
        // set color of myTypeMatchBox to red (invalid)
        myPlansComboBox->setTextColor(FXRGB(255, 0, 0));
        // Write Warning in console if we're in testing mode
        WRITE_DEBUG("Selected invalid item in TemplatePlanSelector");
        // call tag selected function
        myFrameParent->tagSelected();
    }
    return 1;
}


bool
GNEPlanSelector::isPlanMode() const {
    const auto& modes = myFrameParent->getViewNet()->getEditModes();
    if (shown() && modes.isCurrentSupermodeDemand()) {
        return (modes.demandEditMode == DemandEditMode::DEMAND_PERSON) || (modes.demandEditMode == DemandEditMode::DEMAND_PERSONPLAN);
    } else {
        return false;
    }
}

/****************************************************************************/
