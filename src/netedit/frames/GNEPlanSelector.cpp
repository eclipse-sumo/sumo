/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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

#include <netedit/elements/additional/GNEAccess.h>
#include <netedit/elements/additional/GNEBusStop.h>
#include <netedit/elements/additional/GNECalibrator.h>
#include <netedit/elements/additional/GNECalibratorFlow.h>
#include <netedit/elements/additional/GNEChargingStation.h>
#include <netedit/elements/additional/GNEClosingLaneReroute.h>
#include <netedit/elements/additional/GNEClosingReroute.h>
#include <netedit/elements/additional/GNEContainerStop.h>
#include <netedit/elements/additional/GNEDestProbReroute.h>
#include <netedit/elements/additional/GNEEntryExitDetector.h>
#include <netedit/elements/additional/GNEInductionLoopDetector.h>
#include <netedit/elements/additional/GNEInstantInductionLoopDetector.h>
#include <netedit/elements/additional/GNELaneAreaDetector.h>
#include <netedit/elements/additional/GNEMultiEntryExitDetector.h>
#include <netedit/elements/additional/GNEOverheadWire.h>
#include <netedit/elements/additional/GNEParkingArea.h>
#include <netedit/elements/additional/GNEParkingAreaReroute.h>
#include <netedit/elements/additional/GNEParkingSpace.h>
#include <netedit/elements/additional/GNEPOI.h>
#include <netedit/elements/additional/GNEPoly.h>
#include <netedit/elements/additional/GNERerouter.h>
#include <netedit/elements/additional/GNERerouterInterval.h>
#include <netedit/elements/additional/GNERouteProbe.h>
#include <netedit/elements/additional/GNERouteProbReroute.h>
#include <netedit/elements/additional/GNETAZ.h>
#include <netedit/elements/additional/GNETAZSourceSink.h>
#include <netedit/elements/additional/GNETractionSubstation.h>
#include <netedit/elements/additional/GNEVaporizer.h>
#include <netedit/elements/additional/GNEVariableSpeedSign.h>
#include <netedit/elements/additional/GNEVariableSpeedSignStep.h>
#include <netedit/elements/demand/GNEContainer.h>
#include <netedit/elements/demand/GNEPerson.h>
#include <netedit/elements/demand/GNEPersonTrip.h>
#include <netedit/elements/demand/GNERide.h>
#include <netedit/elements/demand/GNERoute.h>
#include <netedit/elements/demand/GNEStop.h>
#include <netedit/elements/demand/GNEStopPlan.h>
#include <netedit/elements/demand/GNETranship.h>
#include <netedit/elements/demand/GNETransport.h>
#include <netedit/elements/demand/GNEVehicle.h>
#include <netedit/elements/demand/GNEVType.h>
#include <netedit/elements/demand/GNEVTypeDistribution.h>
#include <netedit/elements/demand/GNEWalk.h>
#include <netedit/frames/common/GNEInspectorFrame.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
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
    myPlanType(planType),
    myFrameParent(frameParent) {
    // Create MFXComboBoxIcon
    myPlansComboBox = new MFXComboBoxIcon(getCollapsableFrame(), frameParent->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu(),
                                          false, GUIDesignComboBoxVisibleItems, this, MID_GNE_TAG_SELECTED, GUIDesignComboBox);
    // add person plan elements
    for (const auto& planTemplate : myFrameParent->getViewNet()->getNet()->getACTemplates()->getPlanTemplates(myPlanType)) {
        planTemplate.second->resetDefaultValues(false);
        myPlansComboBox->appendIconItem(planTemplate.first->getTooltipText().c_str(),
                                        GUIIconSubSys::getIcon(planTemplate.second->getTagProperty()->getGUIIcon()),
                                        planTemplate.second->getTagProperty()->getBackGroundColor());
    }
    // set myCurrentPlanTemplate
    myCurrentPlanTemplate = myFrameParent->getViewNet()->getNet()->getACTemplates()->getPlanTemplates(myPlanType).front();
    // set color of myTypeMatchBox to black (valid)
    myPlansComboBox->setTextColor(GUIDesignTextColorBlack);
    myPlansComboBox->killFocus();
    // GNEPlanSelector is always shown
    show();
}


GNEPlanSelector::~GNEPlanSelector() {
}


void
GNEPlanSelector::showPlanSelector() {
    show();
    updateEdgeColors();
    updateJunctionColors();
}


void
GNEPlanSelector::hidePlanSelector() {
    hide();
    clearEdgeColors();
    clearJunctionColors();
}


const GNETagProperties*
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
        return myCurrentPlanTemplate.first->planRoute();
    } else {
        return false;
    }
}


bool
GNEPlanSelector::markEdges() const {
    // first check if this modul is shown and selected plan is valid
    if (isPlanValid()) {
        return myCurrentPlanTemplate.first->planConsecutiveEdges() ||
               myCurrentPlanTemplate.first->planEdge() ||
               myCurrentPlanTemplate.first->planFromEdge() ||
               myCurrentPlanTemplate.first->planToEdge();
    } else {
        return false;
    }
}


bool
GNEPlanSelector::markJunctions() const {
    // first check if this modul is shown and selected plan is valid
    if (isPlanValid()) {
        return myCurrentPlanTemplate.first->planFromJunction() ||
               myCurrentPlanTemplate.first->planToJunction();
    } else {
        return false;
    }
}


bool
GNEPlanSelector::markStoppingPlaces() const {
    // first check if this modul is shown and selected plan is valid
    if (isPlanValid()) {
        return myCurrentPlanTemplate.first->planStoppingPlace() ||
               myCurrentPlanTemplate.first->planFromStoppingPlace() ||
               myCurrentPlanTemplate.first->planToStoppingPlace();
    } else {
        return false;
    }
}


bool
GNEPlanSelector::markTAZs() const {
    // first check if this modul is shown and selected plan is valid
    if (isPlanValid()) {
        return myCurrentPlanTemplate.first->planFromTAZ() ||
               myCurrentPlanTemplate.first->planToTAZ();
    } else {
        return false;
    }
}


void
GNEPlanSelector::updateJunctionColors() {
    // clear junction colors
    clearJunctionColors();
    // we assume that all junctions don't support pedestrians
    for (const auto& junction : myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getJunctions()) {
        junction.second->setInvalidCandidate(true);
    }
    // mark junctions that supports pedestrian as candidates
    for (const auto& edge : myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getEdges()) {
        for (int i = 0; i < (int)edge.second->getChildLanes().size(); i++) {
            if (edge.second->getNBEdge()->getLanes().at(i).permissions & SVC_PEDESTRIAN) {
                edge.second->getFromJunction()->setPossibleCandidate(true);
                edge.second->getToJunction()->setPossibleCandidate(true);
            }
        }
    }
    // update view net
    myFrameParent->getViewNet()->updateViewNet();
}


void
GNEPlanSelector::updateEdgeColors() {
    // clear edge colors
    clearEdgeColors();
    // mark edges that supports pedestrian as candidates
    for (const auto& edge : myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getEdges()) {
        bool allowPedestrian = false;
        for (int i = 0; i < (int)edge.second->getChildLanes().size(); i++) {
            if (edge.second->getNBEdge()->getLanes().at(i).permissions & SVC_PEDESTRIAN) {
                allowPedestrian = true;
            }
        }
        if (allowPedestrian) {
            edge.second->setPossibleCandidate(true);
        } else {
            edge.second->setInvalidCandidate(true);
        }
    }
    // update view net
    myFrameParent->getViewNet()->updateViewNet();
}


void
GNEPlanSelector::clearJunctionColors() {
    // reset all junction flags
    for (const auto& junction : myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getJunctions()) {
        junction.second->resetCandidateFlags();
    }
}


void
GNEPlanSelector::clearEdgeColors() {
    // reset all junction flags
    for (const auto& edge : myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getEdges()) {
        edge.second->resetCandidateFlags();
    }
}


long
GNEPlanSelector::onCmdSelectPlan(FXObject*, FXSelector, void*) {
    // check if selected plan of comboBox exists in plans
    for (const auto& planTemplate : myFrameParent->getViewNet()->getNet()->getACTemplates()->getPlanTemplates(myPlanType)) {
        if (planTemplate.first->getTooltipText().c_str() == myPlansComboBox->getText()) {
            // update myCurrentPlanTemplate
            myCurrentPlanTemplate = planTemplate;
            // set color of myTypeMatchBox to black (valid)
            myPlansComboBox->setTextColor(GUIDesignTextColorBlack);
            myPlansComboBox->killFocus();
            // call tag selected function
            myFrameParent->tagSelected();
            return 1;
        }
    }
    // reset myCurrentPlanTemplate
    myCurrentPlanTemplate = std::make_pair(nullptr, nullptr);
    // set color of myTypeMatchBox to red (invalid)
    myPlansComboBox->setTextColor(GUIDesignTextColorRed);
    // call tag selected function
    myFrameParent->tagSelected();
    return 1;
}


bool
GNEPlanSelector::isPlanValid() const {
    if (myCurrentPlanTemplate.second) {
        return myPlansComboBox->getTextColor() == GUIDesignTextColorBlack;
    } else {
        return false;
    }
}

/****************************************************************************/
