/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
#include <netedit/elements/demand/GNETransport.h>
#include <netedit/elements/demand/GNETranship.h>
#include <netedit/elements/demand/GNERide.h>
#include <netedit/elements/demand/GNERoute.h>
#include <netedit/elements/demand/GNEStop.h>
#include <netedit/elements/demand/GNETranship.h>
#include <netedit/elements/demand/GNETransport.h>
#include <netedit/elements/demand/GNEVehicle.h>
#include <netedit/elements/demand/GNEVType.h>
#include <netedit/elements/demand/GNEVTypeDistribution.h>
#include <netedit/elements/demand/GNEWalk.h>
#include <netedit/elements/demand/GNEStopPlan.h>
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

GNEPlanSelector::GNEPlanSelector(GNEFrame* frameParent, SumoXMLTag planType) :
    MFXGroupBoxModule(frameParent, TL("Plan type")),
    myFrameParent(frameParent) {
    // Create MFXComboBoxIcon
    myPlansComboBox = new MFXComboBoxIcon(getCollapsableFrame(), GUIDesignComboBoxNCol, false, GUIDesignComboBoxVisibleItemsLarge,
                                          this, MID_GNE_TAG_SELECTED, GUIDesignComboBox);
    // get net
    const auto net = myFrameParent->getViewNet()->getNet();
    // continue depending of plan type
    if (planType == SUMO_TAG_PERSON) {
        fillPersonPlanTemplates(net);
    } else if (planType == SUMO_TAG_CONTAINER) {
        fillContainerPlanTemplates(net);
    } else {
        throw ProcessError("Invalid plan");
    }
    // add person plan elements
    for (const auto& planTemplate : myPlanTemplates) {
        myPlansComboBox->appendIconItem(planTemplate.first.getTooltipText().c_str(),
                                        GUIIconSubSys::getIcon(planTemplate.second->getTagProperty().getGUIIcon()),
                                        planTemplate.second->getTagProperty().getBackGroundColor());
    }
    // set myCurrentPlanTemplate
    myCurrentPlanTemplate = myPlanTemplates.front();
    // set color of myTypeMatchBox to black (valid)
    myPlansComboBox->setTextColor(FXRGB(0, 0, 0));
    myPlansComboBox->killFocus();
    // GNEPlanSelector is always shown
    show();
}


GNEPlanSelector::~GNEPlanSelector() {
    for (auto& planTemplate : myPlanTemplates) {
        delete planTemplate.second;
    }
    myPlanTemplates.clear();
}


void
GNEPlanSelector::showPlanSelector() {
    show();
}


void
GNEPlanSelector::hidePlanSelector() {
    hide();
}


const GNETagProperties&
GNEPlanSelector::getCurrentPlanTagProperties() const {
    return myCurrentPlanTemplate.first;
}


GNEDemandElement*
GNEPlanSelector::getCurrentPlanTemplate() const {
    return myCurrentPlanTemplate.second;
}


void
GNEPlanSelector::refreshPlanSelector() {
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
    if (isPlanValid()) {
        return myCurrentPlanTemplate.first.planRoute();
    } else {
        return false;
    }
}


bool
GNEPlanSelector::markEdges() const {
    // first check if this modul is shown and selected plan is valid
    if (isPlanValid()) {
        return myCurrentPlanTemplate.first.planConsecutiveEdges() ||
               myCurrentPlanTemplate.first.planEdge() ||
               myCurrentPlanTemplate.first.planFromEdge() ||
               myCurrentPlanTemplate.first.planToEdge();
    } else {
        return false;
    }
}


bool
GNEPlanSelector::markJunctions() const {
    // first check if this modul is shown and selected plan is valid
    if (isPlanValid()) {
        return myCurrentPlanTemplate.first.planFromJunction() ||
               myCurrentPlanTemplate.first.planToJunction();
    } else {
        return false;
    }
}


bool
GNEPlanSelector::markStoppingPlaces() const {
    // first check if this modul is shown and selected plan is valid
    if (isPlanValid()) {
        return myCurrentPlanTemplate.first.planStoppingPlace() ||
               myCurrentPlanTemplate.first.planFromStoppingPlace() ||
               myCurrentPlanTemplate.first.planToStoppingPlace();
    } else {
        return false;
    }
}


bool
GNEPlanSelector::markTAZs() const {
    // first check if this modul is shown and selected plan is valid
    if (isPlanValid()) {
        return myCurrentPlanTemplate.first.planFromTAZ() ||
               myCurrentPlanTemplate.first.planToTAZ();
    } else {
        return false;
    }
}


long
GNEPlanSelector::onCmdSelectPlan(FXObject*, FXSelector, void*) {
    // check if selected plan of comboBox exists in plans
    for (const auto& planTemplate : myPlanTemplates) {
        if (planTemplate.first.getTooltipText().c_str() == myPlansComboBox->getText()) {
            // update myCurrentPlanTemplate
            myCurrentPlanTemplate = planTemplate;
            // set color of myTypeMatchBox to black (valid)
            myPlansComboBox->setTextColor(FXRGB(0, 0, 0));
            myPlansComboBox->killFocus();
            // call tag selected function
            myFrameParent->tagSelected();
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected item '" + myPlansComboBox->getText() + "' in GNEPlanSelector").text());
            return 1;
        }
    }
    // reset myCurrentPlanTemplate
    myCurrentPlanTemplate = std::make_pair(GNETagProperties(), nullptr);
    // set color of myTypeMatchBox to red (invalid)
    myPlansComboBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Selected invalid item in TemplatePlanSelector");
    // call tag selected function
    myFrameParent->tagSelected();
    return 1;
}


bool
GNEPlanSelector::isPlanValid() const {
    if (myCurrentPlanTemplate.second) {
        return myPlansComboBox->getTextColor() == FXRGB(0, 0, 0);
    } else {
        return false;
    }
}


void
GNEPlanSelector::fillPersonPlanTemplates(GNENet* net) {
    GNETagProperties tagProperty;
    // person trip
    tagProperty = GNETagProperties(SUMO_TAG_PERSONTRIP,
                                   GNETagProperties::TagType::PERSONPLAN,
                                   GNETagProperties::TagProperty::NO_PROPERTY,
                                   GNETagProperties::TagParents::PLAN_FROM_EDGE | GNETagProperties::TagParents::PLAN_TO_EDGE |
                                   GNETagProperties::TagParents::PLAN_FROM_TAZ | GNETagProperties::TagParents::PLAN_TO_TAZ |
                                   GNETagProperties::TagParents::PLAN_FROM_JUNCTION | GNETagProperties::TagParents::PLAN_TO_JUNCTION |
                                   GNETagProperties::TagParents::PLAN_FROM_BUSSTOP | GNETagProperties::TagParents::PLAN_TO_BUSSTOP |
                                   GNETagProperties::TagParents::PLAN_FROM_TRAINSTOP | GNETagProperties::TagParents::PLAN_TO_TRAINSTOP |
                                   GNETagProperties::TagParents::PLAN_FROM_CONTAINERSTOP | GNETagProperties::TagParents::PLAN_TO_CONTAINERSTOP |
                                   GNETagProperties::TagParents::PLAN_FROM_CHARGINGSTATION | GNETagProperties::TagParents::PLAN_TO_CHARGINGSTATION |
                                   GNETagProperties::TagParents::PLAN_FROM_PARKINGAREA | GNETagProperties::TagParents::PLAN_TO_PARKINGAREA,
                                   GNETagProperties::Conflicts::NO_CONFLICTS,
                                   GUIIcon::EMPTY, SUMO_TAG_PERSONTRIP, "PersonTrip");
    myPlanTemplates.push_back(std::make_pair(tagProperty, new GNEPersonTrip(GNE_TAG_PERSONTRIP_EDGE_EDGE, net)));
    // ride
    tagProperty = GNETagProperties(SUMO_TAG_RIDE,
                                   GNETagProperties::TagType::PERSONPLAN,
                                   GNETagProperties::TagProperty::NO_PROPERTY,
                                   GNETagProperties::TagParents::PLAN_FROM_EDGE | GNETagProperties::TagParents::PLAN_TO_EDGE |
                                   GNETagProperties::TagParents::PLAN_FROM_TAZ | GNETagProperties::TagParents::PLAN_TO_TAZ |
                                   GNETagProperties::TagParents::PLAN_FROM_JUNCTION | GNETagProperties::TagParents::PLAN_TO_JUNCTION |
                                   GNETagProperties::TagParents::PLAN_FROM_BUSSTOP | GNETagProperties::TagParents::PLAN_TO_BUSSTOP |
                                   GNETagProperties::TagParents::PLAN_FROM_TRAINSTOP | GNETagProperties::TagParents::PLAN_TO_TRAINSTOP |
                                   GNETagProperties::TagParents::PLAN_FROM_CONTAINERSTOP | GNETagProperties::TagParents::PLAN_TO_CONTAINERSTOP |
                                   GNETagProperties::TagParents::PLAN_FROM_CHARGINGSTATION | GNETagProperties::TagParents::PLAN_TO_CHARGINGSTATION |
                                   GNETagProperties::TagParents::PLAN_FROM_PARKINGAREA | GNETagProperties::TagParents::PLAN_TO_PARKINGAREA,
                                   GNETagProperties::Conflicts::NO_CONFLICTS,
                                   GUIIcon::EMPTY, SUMO_TAG_RIDE, "Ride");
    myPlanTemplates.push_back(std::make_pair(tagProperty, new GNERide(GNE_TAG_RIDE_EDGE_EDGE, net)));
    // walk
    tagProperty = GNETagProperties(SUMO_TAG_WALK,
                                   GNETagProperties::TagType::PERSONPLAN,
                                   GNETagProperties::TagProperty::NO_PROPERTY,
                                   GNETagProperties::TagParents::PLAN_FROM_EDGE | GNETagProperties::TagParents::PLAN_TO_EDGE |
                                   GNETagProperties::TagParents::PLAN_FROM_TAZ | GNETagProperties::TagParents::PLAN_TO_TAZ |
                                   GNETagProperties::TagParents::PLAN_FROM_JUNCTION | GNETagProperties::TagParents::PLAN_TO_JUNCTION |
                                   GNETagProperties::TagParents::PLAN_FROM_BUSSTOP | GNETagProperties::TagParents::PLAN_TO_BUSSTOP |
                                   GNETagProperties::TagParents::PLAN_FROM_TRAINSTOP | GNETagProperties::TagParents::PLAN_TO_TRAINSTOP |
                                   GNETagProperties::TagParents::PLAN_FROM_CONTAINERSTOP | GNETagProperties::TagParents::PLAN_TO_CONTAINERSTOP |
                                   GNETagProperties::TagParents::PLAN_FROM_CHARGINGSTATION | GNETagProperties::TagParents::PLAN_TO_CHARGINGSTATION |
                                   GNETagProperties::TagParents::PLAN_FROM_PARKINGAREA | GNETagProperties::TagParents::PLAN_TO_PARKINGAREA,
                                   GNETagProperties::Conflicts::NO_CONFLICTS,
                                   GUIIcon::EMPTY, SUMO_TAG_WALK, "Walk");
    myPlanTemplates.push_back(std::make_pair(tagProperty, new GNEWalk(GNE_TAG_WALK_EDGE_EDGE, net)));
    // walk (edges)
    tagProperty = GNETagProperties(SUMO_TAG_WALK,
                                   GNETagProperties::TagType::PERSONPLAN,
                                   GNETagProperties::TagProperty::NO_PROPERTY,
                                   GNETagProperties::TagParents::PLAN_CONSECUTIVE_EDGES,
                                   GNETagProperties::Conflicts::NO_CONFLICTS,
                                   GUIIcon::EMPTY, SUMO_TAG_WALK, "Walk (edges)");
    myPlanTemplates.push_back(std::make_pair(tagProperty, new GNEWalk(GNE_TAG_WALK_EDGES, net)));
    // walk (route)
    tagProperty = GNETagProperties(SUMO_TAG_WALK,
                                   GNETagProperties::TagType::PERSONPLAN,
                                   GNETagProperties::TagProperty::NO_PROPERTY,
                                   GNETagProperties::TagParents::PLAN_ROUTE,
                                   GNETagProperties::Conflicts::NO_CONFLICTS,
                                   GUIIcon::EMPTY, SUMO_TAG_WALK, "Walk (route)");
    myPlanTemplates.push_back(std::make_pair(tagProperty, new GNEWalk(GNE_TAG_WALK_ROUTE, net)));
    // stop
    tagProperty = GNETagProperties(SUMO_TAG_STOP,
                                   GNETagProperties::TagType::PERSONPLAN,
                                   GNETagProperties::TagProperty::NO_PROPERTY,
                                   GNETagProperties::TagParents::PLAN_EDGE | GNETagProperties::TagParents::PLAN_BUSSTOP |
                                   GNETagProperties::TagParents::PLAN_TRAINSTOP | GNETagProperties::TagParents::PLAN_CONTAINERSTOP |
                                   GNETagProperties::TagParents::PLAN_CHARGINGSTATION | GNETagProperties::TagParents::PLAN_PARKINGAREA,
                                   GNETagProperties::Conflicts::NO_CONFLICTS,
                                   GUIIcon::EMPTY, SUMO_TAG_STOP, "Person Stop");
    myPlanTemplates.push_back(std::make_pair(tagProperty, new GNEStopPlan(GNE_TAG_STOPPERSON_EDGE, net)));
}


void
GNEPlanSelector::fillContainerPlanTemplates(GNENet* net) {
    GNETagProperties tagProperty;
    // transport
    tagProperty = GNETagProperties(SUMO_TAG_TRANSPORT, 0, 0,
                                   GNETagProperties::TagParents::PLAN_FROM_EDGE | GNETagProperties::TagParents::PLAN_TO_EDGE |
                                   //GNETagProperties::TagParents::PLAN_FROM_TAZ | GNETagProperties::TagParents::PLAN_TO_TAZ |
                                   //GNETagProperties::TagParents::PLAN_FROM_JUNCTION | GNETagProperties::TagParents::PLAN_TO_JUNCTION |
                                   //GNETagProperties::TagParents::PLAN_FROM_BUSSTOP | GNETagProperties::TagParents::PLAN_TO_BUSSTOP |
                                   //GNETagProperties::TagParents::PLAN_FROM_TRAINSTOP | GNETagProperties::TagParents::PLAN_TO_TRAINSTOP |
                                   GNETagProperties::TagParents::PLAN_FROM_CONTAINERSTOP | GNETagProperties::TagParents::PLAN_TO_CONTAINERSTOP,
                                   //GNETagProperties::TagParents::PLAN_FROM_CHARGINGSTATION | GNETagProperties::TagParents::PLAN_TO_CHARGINGSTATION |
                                   //GNETagProperties::TagParents::PLAN_FROM_PARKINGAREA | GNETagProperties::TagParents::PLAN_TO_PARKINGAREA,
                                   GNETagProperties::Conflicts::NO_CONFLICTS,
                                   GUIIcon::EMPTY, SUMO_TAG_PERSONTRIP, "Transport");
    myPlanTemplates.push_back(std::make_pair(tagProperty, new GNETransport(GNE_TAG_TRANSPORT_EDGE_EDGE, net)));
    // tranship
    tagProperty = GNETagProperties(SUMO_TAG_TRANSHIP, 0, 0,
                                   GNETagProperties::TagParents::PLAN_FROM_EDGE | GNETagProperties::TagParents::PLAN_TO_EDGE |
                                   //GNETagProperties::TagParents::PLAN_FROM_TAZ | GNETagProperties::TagParents::PLAN_TO_TAZ |
                                   //GNETagProperties::TagParents::PLAN_FROM_JUNCTION | GNETagProperties::TagParents::PLAN_TO_JUNCTION |
                                   //GNETagProperties::TagParents::PLAN_FROM_BUSSTOP | GNETagProperties::TagParents::PLAN_TO_BUSSTOP |
                                   //GNETagProperties::TagParents::PLAN_FROM_TRAINSTOP | GNETagProperties::TagParents::PLAN_TO_TRAINSTOP |
                                   GNETagProperties::TagParents::PLAN_FROM_CONTAINERSTOP | GNETagProperties::TagParents::PLAN_TO_CONTAINERSTOP,
                                   //GNETagProperties::TagParents::PLAN_FROM_CHARGINGSTATION | GNETagProperties::TagParents::PLAN_TO_CHARGINGSTATION |
                                   //GNETagProperties::TagParents::PLAN_FROM_PARKINGAREA | GNETagProperties::TagParents::PLAN_TO_PARKINGAREA,
                                   GNETagProperties::Conflicts::NO_CONFLICTS,
                                   GUIIcon::EMPTY, SUMO_TAG_PERSONTRIP, "Tranship");
    myPlanTemplates.push_back(std::make_pair(tagProperty, new GNETranship(GNE_TAG_TRANSHIP_EDGE_EDGE, net)));
    // tranship (edges)
    tagProperty = GNETagProperties(SUMO_TAG_TRANSHIP, 0, 0,
                                   GNETagProperties::TagParents::PLAN_CONSECUTIVE_EDGES,
                                   GNETagProperties::Conflicts::NO_CONFLICTS,
                                   GUIIcon::EMPTY, SUMO_TAG_PERSONTRIP, "Tranship (edges)");
    myPlanTemplates.push_back(std::make_pair(tagProperty, new GNETranship(GNE_TAG_TRANSHIP_EDGES, net)));
    // stop
    tagProperty = GNETagProperties(SUMO_TAG_STOP, 0, 0,
                                   GNETagProperties::TagParents::PLAN_EDGE | GNETagProperties::TagParents::PLAN_BUSSTOP |
                                   GNETagProperties::TagParents::PLAN_TRAINSTOP | GNETagProperties::TagParents::PLAN_CONTAINERSTOP |
                                   GNETagProperties::TagParents::PLAN_CHARGINGSTATION | GNETagProperties::TagParents::PLAN_PARKINGAREA,
                                   GNETagProperties::Conflicts::NO_CONFLICTS,
                                   GUIIcon::EMPTY, SUMO_TAG_STOP, "Container Stop");
    myPlanTemplates.push_back(std::make_pair(tagProperty, new GNEStopPlan(GNE_TAG_STOPCONTAINER_EDGE, net)));
}

/****************************************************************************/
