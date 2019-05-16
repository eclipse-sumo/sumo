/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEStopFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    March 2019
/// @version $Id$
///
// The Widget for add Stops elements
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netbuild/NBNetBuilder.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/changes/GNEChange_DemandElement.h>
#include <netedit/demandelements/GNEDemandElement.h>
#include <netedit/demandelements/GNERouteHandler.h>
#include <netedit/netelements/GNEConnection.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/vehicle/SUMOVehicleParameter.h>
#include <utils/xml/SUMOSAXAttributesImpl_Cached.h>

#include "GNEStopFrame.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEStopFrame::StopParentSelector) StopParentSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_TYPE,    GNEStopFrame::StopParentSelector::onCmdSelectStopParent),
};

// Object implementation
FXIMPLEMENT(GNEStopFrame::StopParentSelector,     FXGroupBox, StopParentSelectorMap,       ARRAYNUMBER(StopParentSelectorMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEStopFrame::StopParentSelector - methods
// ---------------------------------------------------------------------------

GNEStopFrame::StopParentSelector::StopParentSelector(GNEStopFrame* StopFrameParent) :
    FXGroupBox(StopFrameParent->myContentFrame, "Stop parent", GUIDesignGroupBoxFrame),
    myStopFrameParent(StopFrameParent),
    myCurrentStopParent(nullptr) {
    // Create FXComboBox
    myStopParentMatchBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SET_TYPE, GUIDesignComboBox);
    // create information label
    new FXLabel(this, "- Shift + Click to select\nanother stop parent", 0, GUIDesignLabelFrameInformation);
    // StopParentSelector is always shown
    show();
}


GNEStopFrame::StopParentSelector::~StopParentSelector() {}


GNEDemandElement*
GNEStopFrame::StopParentSelector::getCurrentStopParent() const {
    return myCurrentStopParent;
}


void
GNEStopFrame::StopParentSelector::setStopParent(GNEDemandElement* stopParent) {
    // update stopParentMatchBox
    myStopParentMatchBox->setText(stopParent->getID().c_str());
    onCmdSelectStopParent(nullptr, 0, nullptr);
}


void
GNEStopFrame::StopParentSelector::showStopParentSelector() {
    // refresh stop parent selector
    refreshStopParentSelector();
    // show VType selector
    show();
}


void
GNEStopFrame::StopParentSelector::hideStopParentSelector() {
    hide();
}


void
GNEStopFrame::StopParentSelector::refreshStopParentSelector() {
    // clear comboBox and show
    myStopParentMatchBox->clearItems();
    myStopParentMatchBox->show();
    myStopParentCandidates.clear();
    // reserve stop parent cantidadtes
    myStopParentCandidates.reserve(
        myStopFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_ROUTE).size() +
        myStopFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_VEHICLE).size() +
        myStopFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_TRIP).size() +
        myStopFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_ROUTEFLOW).size() +
        myStopFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_FLOW).size());
    // fill myStopParentMatchBox with list of routes
    for (const auto& i : myStopFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_ROUTE)) {
        myStopParentMatchBox->appendItem(i.first.c_str());
        myStopParentCandidates.push_back(i.second);
    }
    // fill myStopParentMatchBox with list of vehicles
    for (const auto& i : myStopFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_VEHICLE)) {
        myStopParentMatchBox->appendItem(i.first.c_str());
        myStopParentCandidates.push_back(i.second);
    }
    // fill myStopParentMatchBox with list of trips
    for (const auto& i : myStopFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_TRIP)) {
        myStopParentMatchBox->appendItem(i.first.c_str());
        myStopParentCandidates.push_back(i.second);
    }
    // fill myStopParentMatchBox with list of routeFlows
    for (const auto& i : myStopFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_ROUTEFLOW)) {
        myStopParentMatchBox->appendItem(i.first.c_str());
        myStopParentCandidates.push_back(i.second);
    }
    // fill myStopParentMatchBox with list of routeFlows
    for (const auto& i : myStopFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_FLOW)) {
        myStopParentMatchBox->appendItem(i.first.c_str());
        myStopParentCandidates.push_back(i.second);
    }
    // Set visible items
    if ((int)myStopParentMatchBox->getNumItems() < 20) {
        myStopParentMatchBox->setNumVisible((int)myStopParentMatchBox->getNumItems());
    } else {
        myStopParentMatchBox->setNumVisible(20);
    }
    // if myCurrentStopParent is nullptr but there ist myStopParentCandidates, set the first candidate as myCurrentStopParent
    if ((myCurrentStopParent == nullptr) && (myStopParentCandidates.size() > 0)) {
        myCurrentStopParent = myStopParentCandidates.front();
    }
    // check if myCurrentStopParent exists
    bool found = false;
    for (int i = 0; i < (int)myStopParentCandidates.size(); i++) {
        if (myStopParentCandidates.at(i) == myCurrentStopParent) {
            myStopParentMatchBox->setCurrentItem(i);
            found = true;
        }
    }
    // show moduls depending of found
    if (found) {
        myStopFrameParent->myStopTypeSelector->showItemSelector(true);
    } else {
        // disable combo box and moduls if there isn't candidate stop parents in net
        if (myStopParentCandidates.size() > 0) {
            myStopParentMatchBox->setCurrentItem(0, TRUE);
        } else {
            myStopParentMatchBox->hide();
            myStopFrameParent->myStopTypeSelector->hideItemSelector();
        }
    }
}


long
GNEStopFrame::StopParentSelector::onCmdSelectStopParent(FXObject*, FXSelector, void*) {
    // Check if value of myStopParentMatchBox correspond to a existent stop parent candidate
    for (int i = 0; i < (int)myStopParentCandidates.size(); i++) {
        if (myStopParentCandidates.at(i)->getID() == myStopParentMatchBox->getText().text()) {
            // set color of myStopParentMatchBox to black (valid)
            myStopParentMatchBox->setTextColor(FXRGB(0, 0, 0));
            // Set new current VType
            myCurrentStopParent = myStopParentCandidates.at(i);
            // show Stop selector, attributes and help creation moduls
            myStopFrameParent->myStopTypeSelector->showItemSelector(true);
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected item '" + myStopParentMatchBox->getText() + "' in StopParentSelector").text());
            return 1;
        }
    }
    // if VType selecte is invalid, select
    myCurrentStopParent = nullptr;
    // hide all moduls if selected item isn't valid
    myStopFrameParent->myStopTypeSelector->hideItemSelector();
    // set color of myStopParentMatchBox to red (invalid)
    myStopParentMatchBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Selected invalid item in StopParentSelector");
    return 1;
}

// ---------------------------------------------------------------------------
// GNEStopFrame::HelpCreation - methods
// ---------------------------------------------------------------------------

GNEStopFrame::HelpCreation::HelpCreation(GNEStopFrame* StopFrameParent) :
    FXGroupBox(StopFrameParent->myContentFrame, "Help", GUIDesignGroupBoxFrame),
    myStopFrameParent(StopFrameParent) {
    myInformationLabel = new FXLabel(this, "", 0, GUIDesignLabelFrameInformation);
}


GNEStopFrame::HelpCreation::~HelpCreation() {}


void
GNEStopFrame::HelpCreation::showHelpCreation() {
    // first update help cration
    updateHelpCreation();
    // show modul
    show();
}


void
GNEStopFrame::HelpCreation::hideHelpCreation() {
    hide();
}


void
GNEStopFrame::HelpCreation::updateHelpCreation() {
    // create information label
    std::ostringstream information;
    // set text depending of selected Stop type
    switch (myStopFrameParent->myStopTypeSelector->getCurrentTagProperties().getTag()) {
        case SUMO_TAG_STOP_BUSSTOP:
            information
                    << "- Click over a bus stop\n"
                    << "  to create a stop.";
            break;
        case SUMO_TAG_STOP_CONTAINERSTOP:
            information
                    << "- Click over a container stop\n"
                    << "  to create a stop.";
            break;
        case SUMO_TAG_STOP_CHARGINGSTATION:
            information
                    << "- Click over a charging \n"
                    << "  station to create a stop.";
            break;
        case SUMO_TAG_STOP_PARKINGAREA:
            information
                    << "- Click over a parking area\n"
                    << "  to create a stop.";
            break;
        case SUMO_TAG_STOP_LANE:
            information
                    << "- Click over a lane to\n"
                    << "  create a stop.";
            break;
        default:
            break;
    }
    // set information label
    myInformationLabel->setText(information.str().c_str());
}

// ---------------------------------------------------------------------------
// GNEStopFrame - methods
// ---------------------------------------------------------------------------

GNEStopFrame::GNEStopFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Stops") {

    // Create Stop parent selector
    myStopParentSelector = new StopParentSelector(this);

    // Create item Selector modul for Stops
    myStopTypeSelector = new ItemSelector(this, GNEAttributeCarrier::TagType::TAGTYPE_STOP);

    // Create Stop parameters
    myStopAttributes = new AttributesCreator(this);

    // Create Netedit parameter
    myNeteditAttributes = new NeteditAttributes(this);

    // Create Help Creation Modul
    myHelpCreation = new HelpCreation(this);

    // refresh myStopParentMatchBox
    myStopParentSelector->refreshStopParentSelector();
}


GNEStopFrame::~GNEStopFrame() {}


void
GNEStopFrame::show() {
    // refresh vType selector
    myStopParentSelector->refreshStopParentSelector();
    // refresh item selector
    myStopTypeSelector->refreshTagProperties();
    // show frame
    GNEFrame::show();
}


bool
GNEStopFrame::addStop(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor, bool shiftPressed) {
    // check if we're selecting a new stop parent
    if (shiftPressed) {
        if (objectsUnderCursor.getDemandElementFront() &&
                (objectsUnderCursor.getDemandElementFront()->getTagProperty().isVehicle() || objectsUnderCursor.getDemandElementFront()->getTagProperty().getTag() == SUMO_TAG_ROUTE)) {
            myStopParentSelector->setStopParent(objectsUnderCursor.getDemandElementFront());
            WRITE_WARNING("Selected " + objectsUnderCursor.getDemandElementFront()->getTagStr() + " '" + objectsUnderCursor.getDemandElementFront()->getID() + "' as stop parent.");
            return true;
        } else {
            WRITE_WARNING("Selected Stop parent isn't valid.");
            return false;
        }

    } else {
        // now check if stop parent selector is valid
        if (myStopParentSelector->getCurrentStopParent() == nullptr) {
            WRITE_WARNING("Current selected Stop parent isn't valid.");
            return false;
        }

        // obtain tag (only for improve code legibility)
        SumoXMLTag stopTag = myStopTypeSelector->getCurrentTagProperties().getTag();

        // declare a Stop
        SUMOVehicleParameter::Stop stopParameter;

        // first check that current selected Stop is valid
        if (stopTag == SUMO_TAG_NOTHING) {
            WRITE_WARNING("Current selected Stop type isn't valid.");
            return false;
        } else if (stopTag == SUMO_TAG_STOP_LANE) {
            if (objectsUnderCursor.getLaneFront()) {
                stopParameter.lane = objectsUnderCursor.getLaneFront()->getID();
            } else {
                WRITE_WARNING("Click over a lane to create a stop placed in a lane");
                return false;
            }
        } else if (objectsUnderCursor.getAdditionalFront()) {
            if (objectsUnderCursor.getAdditionalFront()->getTagProperty().getTag() == SUMO_TAG_BUS_STOP) {
                if (stopTag != SUMO_TAG_STOP_BUSSTOP) {
                    WRITE_WARNING("Invalid clicked stopping place to create a stop placed in a busstop");
                    return false;
                } else {
                    stopParameter.busstop = objectsUnderCursor.getAdditionalFront()->getID();
                    stopParameter.startPos = 0;
                    stopParameter.endPos = 0;
                }
            } else if (objectsUnderCursor.getAdditionalFront()->getTagProperty().getTag() == SUMO_TAG_CONTAINER_STOP) {
                if (stopTag != SUMO_TAG_STOP_CONTAINERSTOP) {
                    WRITE_WARNING("Invalid clicked stopping place to create a stop placed in a containerStop");
                    return false;
                } else {
                    stopParameter.containerstop = objectsUnderCursor.getAdditionalFront()->getID();
                    stopParameter.startPos = 0;
                    stopParameter.endPos = 0;
                }
            } else if (objectsUnderCursor.getAdditionalFront()->getTagProperty().getTag() == SUMO_TAG_CHARGING_STATION) {
                if (stopTag != SUMO_TAG_STOP_CHARGINGSTATION) {
                    WRITE_WARNING("Invalid clicked stopping place to create a stop placed in a chargingStation");
                    return false;
                } else {
                    stopParameter.chargingStation = objectsUnderCursor.getAdditionalFront()->getID();
                    stopParameter.startPos = 0;
                    stopParameter.endPos = 0;
                }
            } else if (objectsUnderCursor.getAdditionalFront()->getTagProperty().getTag() == SUMO_TAG_PARKING_AREA) {
                if (stopTag != SUMO_TAG_STOP_PARKINGAREA) {
                    WRITE_WARNING("Invalid clicked stopping place to create a stop placed in a parkingArea");
                    return false;
                } else {
                    stopParameter.parkingarea = objectsUnderCursor.getAdditionalFront()->getID();
                    stopParameter.startPos = 0;
                    stopParameter.endPos = 0;
                }
            }
        } else {
            if (stopTag == SUMO_TAG_STOP_BUSSTOP) {
                WRITE_WARNING("Click over a busStop to create a stop placed in a busstop");
                return false;
            } else if (stopTag != SUMO_TAG_STOP_CONTAINERSTOP) {
                WRITE_WARNING("Click over a containerStop to create a stop placed in a containerStop");
                return false;
            } else if (stopTag != SUMO_TAG_CHARGING_STATION) {
                WRITE_WARNING("Click over a chargingStation to create a stop placed in a chargingStation");
                return false;
            } else if (stopTag != SUMO_TAG_STOP_PARKINGAREA) {
                WRITE_WARNING("Click over a parkingArea to create a stop placed in a parkingArea");
                return false;
            }
        }

        // check if stop attributes are valid
        if (!myStopAttributes->areValuesValid()) {
            myStopAttributes->showWarningMessage();
            return false;
        }

        // declare map to keep attributes from Frames from Frame
        std::map<SumoXMLAttr, std::string> valuesMap = myStopAttributes->getAttributesAndValues(false);

        // generate ID
        valuesMap[SUMO_ATTR_ID] = myViewNet->getNet()->generateDemandElementID(stopTag);

        // add netedit values
        if (!stopParameter.lane.empty()) {
            myNeteditAttributes->getNeteditAttributesAndValues(valuesMap, objectsUnderCursor.getLaneFront());
            // check if start position can be parsed
            if (GNEAttributeCarrier::canParse<double>(valuesMap[SUMO_ATTR_STARTPOS])) {
                stopParameter.startPos = GNEAttributeCarrier::parse<double>(valuesMap[SUMO_ATTR_STARTPOS]);
                stopParameter.parametersSet |= STOP_START_SET;
            }
            // check if end position can be parsed
            if (GNEAttributeCarrier::canParse<double>(valuesMap[SUMO_ATTR_ENDPOS])) {
                stopParameter.endPos = GNEAttributeCarrier::parse<double>(valuesMap[SUMO_ATTR_ENDPOS]);
                stopParameter.parametersSet |= STOP_END_SET;
            }
        }

        // obtain friendly position
        bool friendlyPosition = false;
        if (valuesMap.count(SUMO_ATTR_FRIENDLY_POS) > 0) {
            friendlyPosition = GNEAttributeCarrier::parse<bool>(valuesMap.at(SUMO_ATTR_FRIENDLY_POS));
        }

        // fill rest of parameters depending if it was edited
        if (valuesMap.count(SUMO_ATTR_DURATION) > 0) {
            stopParameter.duration = string2time(valuesMap.at(SUMO_ATTR_DURATION));
        } else {
            stopParameter.duration = string2time(GNEAttributeCarrier::getTagProperties(stopTag).getAttributeProperties(SUMO_ATTR_DURATION).getDefaultValue());
        }
        if (valuesMap.count(SUMO_ATTR_UNTIL) > 0) {
            stopParameter.until = string2time(valuesMap[SUMO_ATTR_UNTIL]);
        } else {
            stopParameter.until = string2time(GNEAttributeCarrier::getTagProperties(stopTag).getAttributeProperties(SUMO_ATTR_UNTIL).getDefaultValue());
        }
        if (valuesMap.count(SUMO_ATTR_TRIGGERED) > 0) {
            stopParameter.triggered = GNEAttributeCarrier::parse<bool>(valuesMap.at(SUMO_ATTR_TRIGGERED));
            stopParameter.parametersSet |= STOP_TRIGGER_SET;
        }
        if (valuesMap.count(SUMO_ATTR_CONTAINER_TRIGGERED) > 0) {
            stopParameter.containerTriggered = GNEAttributeCarrier::parse<bool>(valuesMap.at(SUMO_ATTR_CONTAINER_TRIGGERED));
            stopParameter.parametersSet |= STOP_CONTAINER_TRIGGER_SET;
        }
        if (valuesMap.count(SUMO_ATTR_PARKING) > 0) {
            stopParameter.parking = GNEAttributeCarrier::parse<bool>(valuesMap.at(SUMO_ATTR_PARKING));
            stopParameter.parametersSet |= STOP_PARKING_SET;
        }
        if (valuesMap.count(SUMO_ATTR_EXPECTED) > 0) {
            stopParameter.awaitedPersons = GNEAttributeCarrier::parse<std::set<std::string> >(valuesMap.at(SUMO_ATTR_EXPECTED));
            stopParameter.parametersSet |= STOP_EXPECTED_SET;
        }
        if (valuesMap.count(SUMO_ATTR_EXPECTED_CONTAINERS) > 0) {
            stopParameter.awaitedContainers = GNEAttributeCarrier::parse<std::set<std::string> >(valuesMap.at(SUMO_ATTR_EXPECTED_CONTAINERS));
            stopParameter.parametersSet |= STOP_EXPECTED_CONTAINERS_SET;
        }
        if (valuesMap.count(SUMO_ATTR_TRIP_ID) > 0) {
            stopParameter.tripId = valuesMap.at(SUMO_ATTR_TRIP_ID);
            stopParameter.parametersSet |= STOP_TRIP_ID_SET;
        }
        if (valuesMap.count(SUMO_ATTR_INDEX) > 0) {
            if (valuesMap[SUMO_ATTR_INDEX] == "fit") {
                stopParameter.index = STOP_INDEX_FIT;
            } else if (valuesMap[SUMO_ATTR_INDEX] == "end") {
                stopParameter.index = STOP_INDEX_END;
            } else {
                stopParameter.index = GNEAttributeCarrier::parse<int>(valuesMap[SUMO_ATTR_INDEX]);
            }
        } else {
            stopParameter.index = STOP_INDEX_END;
        }

        // create it in RouteFrame
        GNERouteHandler::buildStop(myViewNet, true, stopParameter, myStopParentSelector->getCurrentStopParent(), friendlyPosition);

        // stop sucesfully created, then return true
        return true;
    }
}


// ===========================================================================
// protected
// ===========================================================================

void
GNEStopFrame::enableModuls(const GNEAttributeCarrier::TagProperties& tagProperties) {
    // show Stop type selector modul
    myStopAttributes->showAttributesCreatorModul(tagProperties);
    myNeteditAttributes->showNeteditAttributesModul(tagProperties);
    myHelpCreation->showHelpCreation();
}


void
GNEStopFrame::disableModuls() {
    // hide all moduls if stop parent isn't valid
    myStopAttributes->hideAttributesCreatorModul();
    myNeteditAttributes->hideNeteditAttributesModul();
    myHelpCreation->hideHelpCreation();
}

/****************************************************************************/
