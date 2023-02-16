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
/// @file    GNETagSelector.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2022
///
// Frame for select tags
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

#include "GNETagSelector.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNETagSelector) TagSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_TAG_SELECTED,        GNETagSelector::onCmdSelectTag)
};

// Object implementation
FXIMPLEMENT(GNETagSelector,                MFXGroupBoxModule,     TagSelectorMap,                 ARRAYNUMBER(TagSelectorMap))


// ===========================================================================
// method definitions
// ===========================================================================

GNETagSelector::GNETagSelector(GNEFrame* frameParent, GNETagProperties::TagType type, SumoXMLTag tag, bool onlyDrawables) :
    MFXGroupBoxModule(frameParent, TL("Element")),
    myFrameParent(frameParent),
    myTagType(type),
    myCurrentTemplateAC(nullptr) {
    // Create MFXComboBoxIcon
    myTagsMatchBox = new MFXComboBoxIcon(getCollapsableFrame(), GUIDesignComboBoxNCol, true, this, MID_GNE_TAG_SELECTED, GUIDesignComboBox);
    // set current tag type without notifying
    setCurrentTagType(myTagType, onlyDrawables, false);
    // set current tag without notifying
    setCurrentTag(tag, false);
    // GNETagSelector is always shown
    show();
}


GNETagSelector::~GNETagSelector() {
    // clear myACTemplates and myTagsMatchBox
    for (const auto& ACTemplate : myACTemplates) {
        delete ACTemplate;
    }
    myACTemplates.clear();
}


void
GNETagSelector::showTagSelector() {
    show();
}


void
GNETagSelector::hideTagSelector() {
    hide();
}


GNEAttributeCarrier*
GNETagSelector::getTemplateAC(SumoXMLTag ACTag) const {
    // clear myACTemplates and myTagsMatchBox
    for (const auto& ACTemplate : myACTemplates) {
        if (ACTemplate->getAC()->getTagProperty().getTag() == ACTag) {
            return ACTemplate->getAC();
        }
    }
    return nullptr;
}


GNEAttributeCarrier*
GNETagSelector::getCurrentTemplateAC() const {
    return myCurrentTemplateAC;
}


void
GNETagSelector::setCurrentTagType(GNETagProperties::TagType tagType, const bool onlyDrawables, const bool notifyFrameParent) {
    // check if net has proj
    const bool proj = (GeoConvHelper::getFinal().getProjString() != "!");
    // set new tagType
    myTagType = tagType;
    // change GNETagSelector text
    switch (myTagType) {
        case GNETagProperties::TagType::NETWORKELEMENT:
            setText(TL("network elements"));
            break;
        case GNETagProperties::TagType::ADDITIONALELEMENT:
            setText(TL("Additional elements"));
            break;
        case GNETagProperties::TagType::SHAPE:
            setText(TL("Shape elements"));
            break;
        case GNETagProperties::TagType::TAZELEMENT:
            setText(TL("TAZ elements"));
            break;
        case GNETagProperties::TagType::WIRE:
            setText(TL("Wire elements"));
            break;
        case GNETagProperties::TagType::VEHICLE:
            setText(TL("Vehicles"));
            break;
        case GNETagProperties::TagType::STOP:
            setText(TL("Stops"));
            break;
        case GNETagProperties::TagType::PERSON:
            setText(TL("Persons"));
            break;
        case GNETagProperties::TagType::PERSONPLAN:
            setText(TL("Person plans"));
            break;
        case GNETagProperties::TagType::CONTAINER:
            setText(TL("Container"));
            break;
        case GNETagProperties::TagType::CONTAINERPLAN:
            setText(TL("Container plans"));
            break;
        case GNETagProperties::TagType::PERSONTRIP:
            setText(TL("Person trips"));
            break;
        case GNETagProperties::TagType::WALK:
            setText(TL("Walks"));
            break;
        case GNETagProperties::TagType::RIDE:
            setText(TL("Rides"));
            break;
        case GNETagProperties::TagType::STOPPERSON:
            setText(TL("Person stops"));
            break;
        default:
            throw ProcessError("invalid tag property");
    }
    // clear myACTemplates and myTagsMatchBox
    for (const auto& ACTemplate : myACTemplates) {
        delete ACTemplate;
    }
    myACTemplates.clear();
    myTagsMatchBox->clearItems();
    // get tag properties
    const auto tagProperties = GNEAttributeCarrier::getTagPropertiesByType(myTagType);
    // fill myACTemplates and myTagsMatchBox
    for (const auto& tagProperty : tagProperties) {
        if ((!onlyDrawables || tagProperty.isDrawable()) && (!tagProperty.requireProj() || proj)) {
            myACTemplates.push_back(new ACTemplate(myFrameParent->getViewNet()->getNet(), tagProperty));
            myTagsMatchBox->appendIconItem(tagProperty.getFieldString().c_str(), GUIIconSubSys::getIcon(tagProperty.getGUIIcon()), tagProperty.getBackGroundColor());
        }
    }
    // set color of myTypeMatchBox to black (valid)
    myTagsMatchBox->setTextColor(FXRGB(0, 0, 0));
    myTagsMatchBox->killFocus();
    // Set visible items
    myTagsMatchBox->setNumVisible((int)myTagsMatchBox->getNumItems());
    // set first myACTemplate as edited AC
    myCurrentTemplateAC = myACTemplates.front()->getAC();
    // call tag selected function
    if (notifyFrameParent) {
        myFrameParent->tagSelected();
    }
}


void
GNETagSelector::setCurrentTag(SumoXMLTag newTag, const bool notifyFrameParent) {
    // first reset myCurrentTemplateAC
    myCurrentTemplateAC = nullptr;
    // iterate over all myTagsMatchBox
    for (int i = 0; i < (int)myACTemplates.size(); i++) {
        if (myACTemplates.at(i)->getAC() && (myACTemplates.at(i)->getAC()->getTagProperty().getTag() == newTag)) {
            // set current template and currentItem
            myCurrentTemplateAC = myACTemplates.at(i)->getAC();
            myTagsMatchBox->setCurrentItem(i);
            // set color of myTypeMatchBox to black (valid)
            myTagsMatchBox->setTextColor(FXRGB(0, 0, 0));
            myTagsMatchBox->killFocus();
        }
    }
    // call tag selected function
    if (notifyFrameParent) {
        myFrameParent->tagSelected();
    }
}


void
GNETagSelector::refreshTagSelector() {
    // call tag selected function
    myFrameParent->tagSelected();
}


long
GNETagSelector::onCmdSelectTag(FXObject*, FXSelector, void*) {
    // iterate over all myTagsMatchBox
    for (int i = 0; i < (int)myACTemplates.size(); i++) {
        if (myACTemplates.at(i)->getAC() && myACTemplates.at(i)->getAC()->getTagProperty().getFieldString() == myTagsMatchBox->getText().text()) {
            // set templateAC and currentItem
            myCurrentTemplateAC = myACTemplates.at(i)->getAC();
            myTagsMatchBox->setCurrentItem(i);
            // set color of myTypeMatchBox to black (valid)
            myTagsMatchBox->setTextColor(FXRGB(0, 0, 0));
            myTagsMatchBox->killFocus();
            // call tag selected function
            myFrameParent->tagSelected();
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected item '" + myTagsMatchBox->getText() + "' in GNETagSelector").text());
            return 1;
        }
    }
    // reset templateAC
    myCurrentTemplateAC = nullptr;
    // set color of myTypeMatchBox to red (invalid)
    myTagsMatchBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Selected invalid item in GNETagSelector");
    // call tag selected function
    myFrameParent->tagSelected();
    return 1;
}


GNEAttributeCarrier*
GNETagSelector::ACTemplate::getAC() const {
    return myAC;
}


GNETagSelector::ACTemplate::ACTemplate(GNENet* net, const GNETagProperties tagProperty) :
    myAC(nullptr) {
    // create attribute carrier depending of
    switch (tagProperty.getTag()) {
        // additional elements
        case SUMO_TAG_BUS_STOP:
        case SUMO_TAG_TRAIN_STOP:
            myAC = new GNEBusStop(tagProperty.getTag(), net);
            break;
        case SUMO_TAG_ACCESS:
            myAC = new GNEAccess(net);
            break;
        case SUMO_TAG_CONTAINER_STOP:
            myAC = new GNEContainerStop(net);
            break;
        case SUMO_TAG_CHARGING_STATION:
            myAC = new GNEChargingStation(net);
            break;
        case SUMO_TAG_PARKING_AREA:
            myAC = new GNEParkingArea(net);
            break;
        case SUMO_TAG_PARKING_SPACE:
            myAC = new GNEParkingSpace(net);
            break;
        case SUMO_TAG_INDUCTION_LOOP:
            myAC = new GNEInductionLoopDetector(net);
            break;
        case SUMO_TAG_LANE_AREA_DETECTOR:
        case GNE_TAG_MULTI_LANE_AREA_DETECTOR:
            myAC = new GNELaneAreaDetector(tagProperty.getTag(), net);
            break;
        case SUMO_TAG_ENTRY_EXIT_DETECTOR:
            myAC = new GNEMultiEntryExitDetector(net);
            break;
        case SUMO_TAG_DET_ENTRY:
        case SUMO_TAG_DET_EXIT:
            myAC = new GNEEntryExitDetector(tagProperty.getTag(), net);
            break;
        case SUMO_TAG_INSTANT_INDUCTION_LOOP:
            myAC = new GNEInstantInductionLoopDetector(net);
            break;
        case SUMO_TAG_VSS:
            myAC = new GNEVariableSpeedSign(net);
            break;
        case SUMO_TAG_STEP:
            myAC = new GNEVariableSpeedSignStep(net);
            break;
        case SUMO_TAG_CALIBRATOR:
        case GNE_TAG_CALIBRATOR_LANE:
            myAC = new GNECalibrator(tagProperty.getTag(), net);
            break;
        case GNE_TAG_CALIBRATOR_FLOW:
            myAC = new GNECalibratorFlow(net);
            break;
        case SUMO_TAG_REROUTER:
            myAC = new GNERerouter(net);
            break;
        case SUMO_TAG_INTERVAL:
            myAC = new GNERerouterInterval(net);
            break;
        case SUMO_TAG_CLOSING_REROUTE:
            myAC = new GNEClosingReroute(net);
            break;
        case SUMO_TAG_CLOSING_LANE_REROUTE:
            myAC = new GNEClosingLaneReroute(net);
            break;
        case SUMO_TAG_DEST_PROB_REROUTE:
            myAC = new GNEDestProbReroute(net);
            break;
        case SUMO_TAG_PARKING_AREA_REROUTE:
            myAC = new GNEParkingAreaReroute(net);
            break;
        case SUMO_TAG_ROUTE_PROB_REROUTE:
            myAC = new GNERouteProbReroute(net);
            break;
        case SUMO_TAG_ROUTEPROBE:
            myAC = new GNERouteProbe(net);
            break;
        case SUMO_TAG_VAPORIZER:
            myAC = new GNEVaporizer(net);
            break;
        // shapes
        case SUMO_TAG_POLY:
            myAC = new GNEPoly(net);
            break;
        case SUMO_TAG_POI:
        case GNE_TAG_POILANE:
        case GNE_TAG_POIGEO:
            myAC = new GNEPOI(tagProperty.getTag(), net);
            break;
        // TAZs
        case SUMO_TAG_TAZ:
            myAC = new GNETAZ(net);
            break;
        case SUMO_TAG_TAZSOURCE:
        case SUMO_TAG_TAZSINK:
            myAC = new GNETAZSourceSink(tagProperty.getTag(), net);
            break;
        // wires
        case SUMO_TAG_TRACTION_SUBSTATION:
            myAC = new GNETractionSubstation(net);
            break;
        case SUMO_TAG_OVERHEAD_WIRE_SECTION:
            myAC = new GNEOverheadWire(net);
            break;
        case SUMO_TAG_OVERHEAD_WIRE_CLAMP:
            myAC = nullptr; // TMP
            break;
        // Demand elements
        case SUMO_TAG_ROUTE:
        case GNE_TAG_ROUTE_EMBEDDED:
            myAC = new GNERoute(tagProperty.getTag(), net);
            break;
        case SUMO_TAG_VTYPE:
            myAC = new GNEVType(net);
            break;
        case SUMO_TAG_VTYPE_DISTRIBUTION:
            myAC = new GNEVTypeDistribution(net);
            break;
        case SUMO_TAG_VEHICLE:
        case GNE_TAG_VEHICLE_WITHROUTE:
        case GNE_TAG_FLOW_ROUTE:
        case GNE_TAG_FLOW_WITHROUTE:
        case SUMO_TAG_TRIP:
        case GNE_TAG_TRIP_JUNCTIONS:
        case SUMO_TAG_FLOW:
        case GNE_TAG_FLOW_JUNCTIONS:
            myAC = new GNEVehicle(tagProperty.getTag(), net);
            break;
        // stops
        case SUMO_TAG_STOP_LANE:
        case SUMO_TAG_STOP_BUSSTOP:
        case SUMO_TAG_STOP_CONTAINERSTOP:
        case SUMO_TAG_STOP_CHARGINGSTATION:
        case SUMO_TAG_STOP_PARKINGAREA:
        case GNE_TAG_STOPPERSON_EDGE:
        case GNE_TAG_STOPPERSON_BUSSTOP:
        case GNE_TAG_STOPCONTAINER_EDGE:
        case GNE_TAG_STOPCONTAINER_CONTAINERSTOP:
        // waypoints
        case GNE_TAG_WAYPOINT_LANE:
        case GNE_TAG_WAYPOINT_BUSSTOP:
        case GNE_TAG_WAYPOINT_CONTAINERSTOP:
        case GNE_TAG_WAYPOINT_CHARGINGSTATION:
        case GNE_TAG_WAYPOINT_PARKINGAREA:
            myAC = new GNEStop(tagProperty.getTag(), net);
            break;
        case SUMO_TAG_PERSON:
        case SUMO_TAG_PERSONFLOW:
            myAC = new GNEPerson(tagProperty.getTag(), net);
            break;
        case SUMO_TAG_CONTAINER:
        case SUMO_TAG_CONTAINERFLOW:
            myAC = new GNEContainer(tagProperty.getTag(), net);
            break;
        case GNE_TAG_TRANSPORT_EDGE:
        case GNE_TAG_TRANSPORT_CONTAINERSTOP:
            myAC = new GNETransport(tagProperty.getTag(), net);
            break;
        case GNE_TAG_TRANSHIP_EDGE:
        case GNE_TAG_TRANSHIP_CONTAINERSTOP:
        case GNE_TAG_TRANSHIP_EDGES:
            myAC = new GNETranship(tagProperty.getTag(), net);
            break;
        case GNE_TAG_PERSONTRIP_EDGE:
        case GNE_TAG_PERSONTRIP_BUSSTOP:
        case GNE_TAG_PERSONTRIP_JUNCTIONS:
            myAC = new GNEPersonTrip(tagProperty.getTag(), net);
            break;
        case GNE_TAG_WALK_EDGE:
        case GNE_TAG_WALK_BUSSTOP:
        case GNE_TAG_WALK_EDGES:
        case GNE_TAG_WALK_ROUTE:
        case GNE_TAG_WALK_JUNCTIONS:
            myAC = new GNEWalk(tagProperty.getTag(), net);
            break;
        case GNE_TAG_RIDE_EDGE:
        case GNE_TAG_RIDE_BUSSTOP:
            myAC = new GNERide(tagProperty.getTag(), net);
            break;
        default:
            throw ProcessError("Non-supported tagProperty in ACTemplate");
            break;
    }
}


GNETagSelector::ACTemplate::~ACTemplate() {
    delete myAC;
}

/****************************************************************************/
