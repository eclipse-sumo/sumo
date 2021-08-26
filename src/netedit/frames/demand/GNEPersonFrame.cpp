/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    GNEPersonFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    May 2019
///
// The Widget for add Person elements
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/elements/demand/GNERouteHandler.h>
#include <utils/vehicle/SUMOVehicleParserHelper.h>
#include <utils/xml/SUMOSAXAttributesImpl_Cached.h>

#include "GNEPersonFrame.h"

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEPersonFrame - methods
// ---------------------------------------------------------------------------

GNEPersonFrame::GNEPersonFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Persons"),
    myRouteHandler("", viewNet->getNet(), true),
    myPersonBaseObject(new CommonXMLStructure::SumoBaseObject(nullptr)) {

    // create tag Selector modul for persons
    myPersonTagSelector = new GNEFrameModuls::TagSelector(this, GNETagProperties::TagType::PERSON);

    // create person types selector modul
    myPTypeSelector = new GNEFrameModuls::DemandElementSelector(this, SUMO_TAG_PTYPE);

    // create person attributes
    myPersonAttributes = new GNEFrameAttributesModuls::AttributesCreator(this);

    // create tag Selector modul for person plans
    myPersonPlanTagSelector = new GNEFrameModuls::TagSelector(this, GNETagProperties::TagType::PERSONPLAN);

    // create person plan attributes
    myPersonPlanAttributes = new GNEFrameAttributesModuls::AttributesCreator(this);

    // Create Netedit parameter
    myNeteditAttributes = new GNEFrameAttributesModuls::NeteditAttributes(this);

    // create PathCreator Modul
    myPathCreator = new GNEFrameModuls::PathCreator(this);

    // limit path creator to pedestrians
    myPathCreator->setVClass(SVC_PEDESTRIAN);

    // set Person as default vehicle
    myPersonTagSelector->setCurrentTag(SUMO_TAG_PERSON);
}


GNEPersonFrame::~GNEPersonFrame() {
    delete myPersonBaseObject;
}


void
GNEPersonFrame::show() {
    // refresh item selector
    myPersonTagSelector->refreshTagProperties();
    myPTypeSelector->refreshDemandElementSelector();
    myPersonPlanTagSelector->refreshTagProperties();
    // update VClass of myPathCreator
    if (myPersonPlanTagSelector->getCurrentTagProperties().isRide()) {
        myPathCreator->setVClass(SVC_PASSENGER);
    } else {
        myPathCreator->setVClass(SVC_PEDESTRIAN);
    }
    // show frame
    GNEFrame::show();
}


void
GNEPersonFrame::hide() {
    // reset candidate edges
    for (const auto& edge : myViewNet->getNet()->getAttributeCarriers()->getEdges()) {
        edge.second->resetCandidateFlags();
    }
    // hide frame
    GNEFrame::hide();
}


bool
GNEPersonFrame::addPerson(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor, const GNEViewNetHelper::MouseButtonKeyPressed& mouseButtonKeyPressed) {
    // first check that we clicked over an AC
    if (objectsUnderCursor.getAttributeCarrierFront() == nullptr) {
        return false;
    }
    // obtain tags (only for improve code legibility)
    SumoXMLTag personTag = myPersonTagSelector->getCurrentTagProperties().getTag();
    SumoXMLTag clickedACTag = objectsUnderCursor.getAttributeCarrierFront()->getTagProperty().getTag();
    // first check that current selected person is valid
    if (personTag == SUMO_TAG_NOTHING) {
        myViewNet->setStatusBarText("Current selected person isn't valid.");
        return false;
    }
    // now check that pType is valid
    if (myPTypeSelector->getCurrentDemandElement() == nullptr) {
        myViewNet->setStatusBarText("Current selected person type isn't valid.");
        return false;
    }
    // finally check that person plan selected is valid
    if (myPersonPlanTagSelector->getCurrentTagProperties().getTag() == SUMO_TAG_NOTHING) {
        myViewNet->setStatusBarText("Current selected person plan isn't valid.");
        return false;
    }
    // add elements to path creator
    if (clickedACTag == SUMO_TAG_LANE) {
        return myPathCreator->addEdge(objectsUnderCursor.getEdgeFront(), mouseButtonKeyPressed.shiftKeyPressed(), mouseButtonKeyPressed.controlKeyPressed());
    } else if (clickedACTag == SUMO_TAG_BUS_STOP) {
        return myPathCreator->addStoppingPlace(objectsUnderCursor.getAdditionalFront(), mouseButtonKeyPressed.shiftKeyPressed(), mouseButtonKeyPressed.controlKeyPressed());
    } else if (clickedACTag == SUMO_TAG_ROUTE) {
        return myPathCreator->addRoute(objectsUnderCursor.getDemandElementFront(), mouseButtonKeyPressed.shiftKeyPressed(), mouseButtonKeyPressed.controlKeyPressed());
    } else {
        return false;
    }
}


GNEFrameModuls::PathCreator*
GNEPersonFrame::getPathCreator() const {
    return myPathCreator;
}

// ===========================================================================
// protected
// ===========================================================================

void
GNEPersonFrame::tagSelected() {
    // first check if person is valid
    if (myPersonTagSelector->getCurrentTagProperties().getTag() != SUMO_TAG_NOTHING) {
        // show PType selector and person plan selector
        myPTypeSelector->showDemandElementSelector();
        // check if current person type selected is valid
        if (myPTypeSelector->getCurrentDemandElement()) {
            // show person attributes depending of myPersonPlanTagSelector
            if (myPersonPlanTagSelector->getCurrentTagProperties().isStopPerson()) {
                myPersonAttributes->showAttributesCreatorModul(myPersonTagSelector->getCurrentTagProperties(), {SUMO_ATTR_DEPARTPOS});
            } else {
                myPersonAttributes->showAttributesCreatorModul(myPersonTagSelector->getCurrentTagProperties(), {});
            }
            // show person plan tag selector
            myPersonPlanTagSelector->showTagSelector();
            // now check if person plan selected is valid
            if (myPersonPlanTagSelector->getCurrentTagProperties().getTag() != SUMO_TAG_NOTHING) {
                // update VClass of myPathCreator depending if person is a ride
                if (myPersonPlanTagSelector->getCurrentTagProperties().isRide()) {
                    myPathCreator->setVClass(SVC_PASSENGER);
                } else {
                    myPathCreator->setVClass(SVC_PEDESTRIAN);
                }
                // show person plan attributes
                myPersonPlanAttributes->showAttributesCreatorModul(myPersonPlanTagSelector->getCurrentTagProperties(), {});
                // show Netedit attributes modul
                myNeteditAttributes->showNeteditAttributesModul(myPersonPlanTagSelector->getCurrentTagProperties());
                // show edge path creator modul
                myPathCreator->showPathCreatorModul(myPersonPlanTagSelector->getCurrentTagProperties().getTag(), false, false);
            } else {
                // hide modules
                myPersonPlanAttributes->hideAttributesCreatorModul();
                myNeteditAttributes->hideNeteditAttributesModul();
                myPathCreator->hidePathCreatorModul();
            }
        } else {
            // hide modules
            myPersonPlanTagSelector->hideTagSelector();
            myPersonAttributes->hideAttributesCreatorModul();
            myPersonPlanAttributes->hideAttributesCreatorModul();
            myNeteditAttributes->hideNeteditAttributesModul();
            myPathCreator->hidePathCreatorModul();
        }
    } else {
        // hide all moduls if person isn't valid
        myPTypeSelector->hideDemandElementSelector();
        myPersonPlanTagSelector->hideTagSelector();
        myPersonAttributes->hideAttributesCreatorModul();
        myPersonPlanAttributes->hideAttributesCreatorModul();
        myNeteditAttributes->hideNeteditAttributesModul();
        myPathCreator->hidePathCreatorModul();
    }
}


void
GNEPersonFrame::demandElementSelected() {
    if (myPTypeSelector->getCurrentDemandElement()) {
        // show person attributes depending of myPersonPlanTagSelector
        if (myPersonPlanTagSelector->getCurrentTagProperties().isStopPerson()) {
            myPersonAttributes->showAttributesCreatorModul(myPersonTagSelector->getCurrentTagProperties(), {SUMO_ATTR_DEPARTPOS});
        } else {
            myPersonAttributes->showAttributesCreatorModul(myPersonTagSelector->getCurrentTagProperties(), {});
        }
        // show person plan tag selector
        myPersonPlanTagSelector->showTagSelector();
        // now check if person plan selected is valid
        if (myPersonPlanTagSelector->getCurrentTagProperties().getTag() != SUMO_TAG_NOTHING) {
            // update VClass of myPathCreator depending if person is a ride
            if (myPersonPlanTagSelector->getCurrentTagProperties().isRide()) {
                myPathCreator->setVClass(SVC_PASSENGER);
            } else {
                myPathCreator->setVClass(SVC_PEDESTRIAN);
            }
            // show person plan attributes
            myPersonPlanAttributes->showAttributesCreatorModul(myPersonPlanTagSelector->getCurrentTagProperties(), {});
            // show Netedit attributes modul
            myNeteditAttributes->showNeteditAttributesModul(myPersonPlanTagSelector->getCurrentTagProperties());
            // show edge path creator modul
            myPathCreator->showPathCreatorModul(myPersonPlanTagSelector->getCurrentTagProperties().getTag(), false, false);
        } else {
            // hide modules
            myPersonPlanAttributes->hideAttributesCreatorModul();
            myNeteditAttributes->hideNeteditAttributesModul();
            myPathCreator->hidePathCreatorModul();
        }
    } else {
        // hide modules
        myPersonPlanTagSelector->hideTagSelector();
        myPersonAttributes->hideAttributesCreatorModul();
        myPersonPlanAttributes->hideAttributesCreatorModul();
        myNeteditAttributes->hideNeteditAttributesModul();
        myPathCreator->hidePathCreatorModul();
    }
}


void
GNEPersonFrame::createPath() {
    // first check that all attributes are valid
    if (!myPersonAttributes->areValuesValid()) {
        myViewNet->setStatusBarText("Invalid person parameters.");
    } else if (!myPersonPlanAttributes->areValuesValid()) {
        myViewNet->setStatusBarText("Invalid " + myPersonPlanTagSelector->getCurrentTagProperties().getTagStr() + " parameters.");
    } else {
        // begin undo-redo operation
        myViewNet->getUndoList()->p_begin("create " + myPersonTagSelector->getCurrentTagProperties().getTagStr() + " and " + myPersonPlanTagSelector->getCurrentTagProperties().getTagStr());
        // create person
        GNEDemandElement* person = buildPerson();
        // check if person and person plan can be created
        if (GNERouteHandler::buildPersonPlan(
                    myPersonPlanTagSelector->getCurrentTagProperties().getTag(),
                    person, myPersonPlanAttributes, myPathCreator)) {
            // end undo-redo operation
            myViewNet->getUndoList()->p_end();
            // abort path creation
            myPathCreator->abortPathCreation();
            // refresh person and personPlan attributes
            myPersonAttributes->refreshRows();
            myPersonPlanAttributes->refreshRows();
            // compute person
            person->computePathElement();
        } else {
            // abort person creation
            myViewNet->getUndoList()->p_abort();
        }
    }
}

// ---------------------------------------------------------------------------
// GNEPersonFrame - private methods
// ---------------------------------------------------------------------------

GNEDemandElement*
GNEPersonFrame::buildPerson() {
    // first person base object
    myPersonBaseObject->clear();
    // obtain person tag (only for improve code legibility)
    SumoXMLTag personTag = myPersonTagSelector->getCurrentTagProperties().getTag();
    // set tag
    myPersonBaseObject->setTag(personTag);
    // Declare map to keep attributes from myPersonAttributes
    std::map<SumoXMLAttr, std::string> valuesMap = myPersonAttributes->getAttributesAndValuesTemporal(false);
    // Check if ID has to be generated
    if (valuesMap.count(SUMO_ATTR_ID) == 0) {
        valuesMap[SUMO_ATTR_ID] = myViewNet->getNet()->generateDemandElementID(personTag);
    }
    // add pType parameter
    valuesMap[SUMO_ATTR_TYPE] = myPTypeSelector->getCurrentDemandElement()->getID();

    // check if we're creating a person or personFlow
    if (personTag == SUMO_TAG_PERSON) {
        // Add parameter departure
        if (valuesMap[SUMO_ATTR_DEPART].empty()) {
            valuesMap[SUMO_ATTR_DEPART] = "0";
        }
        // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
        SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(valuesMap, getPredefinedTagsMML(), toString(personTag));
        // obtain person parameters
        SUMOVehicleParameter* personParameters = SUMOVehicleParserHelper::parseVehicleAttributes(SUMO_TAG_PERSON, SUMOSAXAttrs, false, false, false);
        // check personParameters
        if (personParameters) {
            myPersonBaseObject->setVehicleParameter(personParameters);
            // parse vehicle
            myRouteHandler.parseSumoBaseObject(myPersonBaseObject);
            // delete personParameters
            delete personParameters;
        }
    } else {
        // set begin and end attributes
        if (valuesMap[SUMO_ATTR_BEGIN].empty()) {
            valuesMap[SUMO_ATTR_BEGIN] = "0";
        }
        if (valuesMap[SUMO_ATTR_END].empty()) {
            valuesMap[SUMO_ATTR_END] = "3600";
        }
        // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
        SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(valuesMap, getPredefinedTagsMML(), toString(personTag));
        // obtain personFlow parameters
        SUMOVehicleParameter* personFlowParameters = SUMOVehicleParserHelper::parseFlowAttributes(SUMO_TAG_PERSONFLOW, SUMOSAXAttrs, false, 0, SUMOTime_MAX);
        // check personParameters
        if (personFlowParameters) {
            myPersonBaseObject->setVehicleParameter(personFlowParameters);
            // parse vehicle
            myRouteHandler.parseSumoBaseObject(myPersonBaseObject);
            // delete personParameters
            delete personFlowParameters;
        }
    }
    // refresh person and personPlan attributes
    myPersonAttributes->refreshRows();
    myPersonPlanAttributes->refreshRows();
    // return created person
    return myViewNet->getNet()->retrieveDemandElement(personTag, valuesMap[SUMO_ATTR_ID]);
}


/****************************************************************************/
