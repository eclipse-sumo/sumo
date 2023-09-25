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

GNEPlanSelector::GNEPlanSelector(GNEFrame* frameParent, GNETagProperties::TagType type) :
    MFXGroupBoxModule(frameParent, TL("Element")),
    myFrameParent(frameParent),
    myTagType(type),
    myCurrentPlanTemplate(nullptr) {
    // Create MFXComboBoxIcon
    myPlansComboBox = new MFXComboBoxIcon(getCollapsableFrame(), GUIDesignComboBoxNCol, false, GUIDesignComboBoxVisibleItemsLarge,
                                          this, MID_GNE_TAG_SELECTED, GUIDesignComboBox);
    // get net
    auto net = frameParent->getViewNet()->getNet();
    // continue depending of plan type
    if (myTagType == GNETagProperties::TagType::PERSON) {
        setText(TL("Person plans"));
        // add person plan elements
        myPlansComboBox->appendIconItem("PersonTrip", GUIIconSubSys::getIcon(GUIIcon::PERSONTRIP_EDGE), FXRGBA(253, 255, 206, 255));
        myPlansComboBox->appendIconItem("Walk", GUIIconSubSys::getIcon(GUIIcon::WALK_EDGE), FXRGBA(240, 255, 205, 255));
        myPlansComboBox->appendIconItem("Walk (Edges)", GUIIconSubSys::getIcon(GUIIcon::WALK_EDGES), FXRGBA(240, 255, 205, 255));
        myPlansComboBox->appendIconItem("Walk (Route)", GUIIconSubSys::getIcon(GUIIcon::WALK_ROUTE), FXRGBA(240, 255, 205, 255));
        myPlansComboBox->appendIconItem("Ride", GUIIconSubSys::getIcon(GUIIcon::RIDE_EDGE), FXRGBA(210, 233, 255, 255));
        // add person plan templates
        myPlanTemplates["PersonTrip"] = new GNEPersonTrip(GNE_TAG_PERSONTRIP_EDGE_EDGE, net);
        myPlanTemplates["Walk"] = new GNEWalk(GNE_TAG_WALK_EDGE_EDGE, net);
        myPlanTemplates["Walk (Edges)"] = new GNEWalk(GNE_TAG_WALK_EDGES, net);
        myPlanTemplates["Walk (Route)"] = new GNEWalk(GNE_TAG_WALK_ROUTE, net);
        myPlanTemplates["Ride"] = new GNERide(GNE_TAG_RIDE_EDGE_EDGE, net);
    } else if (myTagType == GNETagProperties::TagType::CONTAINER) {
        setText(TL("ContainerPlans"));
        // add person plan elements
        myPlansComboBox->appendIconItem("Transport", GUIIconSubSys::getIcon(GUIIcon::TRANSPORT_EDGE), FXRGBA(240, 255, 205, 255));
        myPlansComboBox->appendIconItem("Tranship", GUIIconSubSys::getIcon(GUIIcon::TRANSHIP_EDGE), FXRGBA(210, 233, 255, 255));
        myPlansComboBox->appendIconItem("Tranship (Edges)", GUIIconSubSys::getIcon(GUIIcon::TRANSHIP_EDGES), FXRGBA(210, 233, 255, 255));
        // add person plan templates
        myPlanTemplates["Transport"] = new GNEPersonTrip(GNE_TAG_TRANSPORT_EDGE, net);
        myPlanTemplates["Tranship"] = new GNEWalk(GNE_TAG_TRANSHIP_EDGE, net);
        myPlanTemplates["Tranship (Edges)"] = new GNEWalk(GNE_TAG_TRANSHIP_EDGES, net);
    } else {
        throw ProcessError("invalid plan parent tag");
    }
    // set color of myTypeMatchBox to black (valid)
    myPlansComboBox->setTextColor(FXRGB(0, 0, 0));
    myPlansComboBox->killFocus();
    // set current plan
    myCurrentPlanTemplate = myPlanTemplates[myPlansComboBox->getText().text()];
    // GNEPlanSelector is always shown
    show();
}


GNEPlanSelector::~GNEPlanSelector() {
    // clear myACTemplates and myTagsMatchBox
    for (const auto& planTemplate : myPlanTemplates) {
        delete planTemplate.second;
    }
    myPlanTemplates.clear();
}


void
GNEPlanSelector::showTagSelector() {
    show();
}


void
GNEPlanSelector::hideTagSelector() {
    hide();
}


GNEDemandElement*
GNEPlanSelector::getCurrentTemplatePlan() const {
    return myCurrentPlanTemplate;
}


void
GNEPlanSelector::refreshTagSelector() {
    // call tag selected function
    myFrameParent->tagSelected();
}


long
GNEPlanSelector::onCmdSelectPlan(FXObject*, FXSelector, void*) {
    // iterate over all plan templates
    for (const auto& planTemplate : myPlanTemplates) {
        if (planTemplate.first == myPlansComboBox->getText().text()) {
            // set templateAC and currentItem
            myCurrentPlanTemplate = planTemplate.second;
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
    // reset templateAC
    myCurrentPlanTemplate = nullptr;
    // set color of myTypeMatchBox to red (invalid)
    myPlansComboBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Selected invalid item in TemplatePlanSelector");
    // call tag selected function
    myFrameParent->tagSelected();
    return 1;
}

/****************************************************************************/
