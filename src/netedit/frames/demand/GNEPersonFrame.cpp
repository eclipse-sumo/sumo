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
/// @file    GNEPersonFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    May 2019
///
// The Widget for add Person elements
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>
#include <netedit/elements/additional/GNETAZ.h>
#include <netedit/elements/demand/GNERouteHandler.h>
#include <netedit/frames/GNEAttributesEditor.h>
#include <netedit/frames/GNEDemandSelector.h>
#include <netedit/frames/GNEPlanCreator.h>
#include <netedit/frames/GNEPlanCreatorLegend.h>
#include <utils/vehicle/SUMOVehicleParserHelper.h>
#include <utils/xml/SUMOSAXAttributesImpl_Cached.h>

#include "GNEPersonFrame.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNEPersonFrame::GNEPersonFrame(GNEViewParent* viewParent, GNEViewNet* viewNet) :
    GNEFrame(viewParent, viewNet, TL("Persons")),
    myPersonBaseObject(new CommonXMLStructure::SumoBaseObject(nullptr)) {

    // create tag Selector module for persons
    myPersonTagSelector = new GNETagSelector(this, GNETagProperties::Type::PERSON, SUMO_TAG_PERSON);

    // create person types selector module and set DEFAULT_PEDTYPE_ID as default element
    myTypeSelector = new GNEDemandElementSelector(this, SUMO_TAG_VTYPE, GNETagProperties::Type::PERSON);

    // create person attributes
    myPersonAttributesEditor = new GNEAttributesEditor(this, GNEAttributesEditorType::EditorType::CREATOR);

    // create plan selector module for person plans
    myPlanSelector = new GNEPlanSelector(this, SUMO_TAG_PERSON);

    // create person plan attributes
    myPersonPlanAttributesEditor = new GNEAttributesEditor(this, GNEAttributesEditorType::EditorType::CREATOR);

    // create GNEPlanCreator Module
    myPlanCreator = new GNEPlanCreator(this, viewNet->getNet()->getDemandPathManager());

    // create plan creator legend
    myPlanCreatorLegend = new GNEPlanCreatorLegend(this);
}


GNEPersonFrame::~GNEPersonFrame() {
    delete myPersonBaseObject;
}


void
GNEPersonFrame::show() {
    // refresh tag selector
    myPersonTagSelector->refreshTagSelector();
    myTypeSelector->refreshDemandElementSelector();
    myPlanSelector->refreshPlanSelector();
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
GNEPersonFrame::addPerson(const GNEViewNetHelper::ViewObjectsSelector& viewObjects) {
    // first check that we clicked over an AC
    if (viewObjects.getAttributeCarrierFront() == nullptr) {
        return false;
    }
    // obtain tags (only for improve code legibility)
    SumoXMLTag personTag = myPersonTagSelector->getCurrentTemplateAC()->getTagProperty()->getTag();
    // first check that current selected person is valid
    if (personTag == SUMO_TAG_NOTHING) {
        myViewNet->setStatusBarText(TL("Current selected person isn't valid."));
        return false;
    }
    // now check that pType is valid
    if (myTypeSelector->getCurrentDemandElement() == nullptr) {
        myViewNet->setStatusBarText(TL("Current selected person type isn't valid."));
        return false;
    }
    // finally check that person plan selected is valid
    if (myPlanSelector->getCurrentPlanTemplate() == nullptr) {
        myViewNet->setStatusBarText(TL("Current selected person plan isn't valid."));
        return false;
    }
    for (GNEAdditional* o : viewObjects.getAdditionals()) {
        if (o->getTagProperty()->isStoppingPlace()) {
            return myPlanCreator->addStoppingPlace(o);
        }
    }
    for (GNEDemandElement* o : viewObjects.getDemandElements()) {
        if (o->getTagProperty()->getTag() == SUMO_TAG_ROUTE) {
            return myPlanCreator->addRoute(o);
        }
    }
    if (viewObjects.getAttributeCarrierFront() == viewObjects.getJunctionFront()) {
        return myPlanCreator->addJunction(viewObjects.getJunctions().front());
    }
    if (viewObjects.getAttributeCarrierFront() == viewObjects.getLaneFront()) {
        return myPlanCreator->addEdge(viewObjects.getLanes().front());
    }
    if (viewObjects.getAttributeCarrierFront() == viewObjects.getTAZFront()) {
        return myPlanCreator->addTAZ(viewObjects.getTAZs().front());
    }
    return false;
}


GNEPlanCreator*
GNEPersonFrame::getPlanCreator() const {
    return myPlanCreator;
}


GNEDemandElementSelector*
GNEPersonFrame::getTypeSelector() const {
    return myTypeSelector;
}


GNEPlanSelector*
GNEPersonFrame::getPlanSelector() const {
    return myPlanSelector;
}


GNEAttributesEditor*
GNEPersonFrame::getPersonAttributesEditor() const {
    return myPersonAttributesEditor;
}

// ===========================================================================
// protected
// ===========================================================================

void
GNEPersonFrame::tagSelected() {
    // first check if person is valid
    if (myPersonTagSelector->getCurrentTemplateAC()) {
        // show PType selector and person plan selector
        myTypeSelector->showDemandElementSelector();
        // check if current person type selected is valid
        if (myTypeSelector->getCurrentDemandElement()) {
            // show person attributes depending of myPlanSelector
            myPersonAttributesEditor->showAttributesEditor(myPersonTagSelector->getCurrentTemplateAC(), true);
            // show person plan tag selector
            myPlanSelector->showPlanSelector();
            // check current plan template
            if (myPlanSelector->getCurrentPlanTemplate()) {
                // show person plan attributes
                myPersonPlanAttributesEditor->showAttributesEditor(myPlanSelector->getCurrentPlanTemplate(), false);
                // show edge path creator module
                myPlanCreator->showPlanCreatorModule(myPlanSelector, nullptr);
                // show path legend
                myPlanCreatorLegend->showPlanCreatorLegend();
            } else {
                // hide modules
                myPersonPlanAttributesEditor->hideAttributesEditor();
                myPlanCreator->hidePathCreatorModule();
                myPlanCreatorLegend->hidePlanCreatorLegend();
            }
        } else {
            // hide modules
            myPlanSelector->hidePlanSelector();
            myPersonAttributesEditor->hideAttributesEditor();
            myPersonPlanAttributesEditor->hideAttributesEditor();
            myPlanCreator->hidePathCreatorModule();
            myPlanCreatorLegend->hidePlanCreatorLegend();
        }
    } else {
        // hide all modules if person isn't valid
        myTypeSelector->hideDemandElementSelector();
        myPlanSelector->hidePlanSelector();
        myPersonPlanAttributesEditor->hideAttributesEditor();
        myPersonPlanAttributesEditor->hideAttributesEditor();
        myPlanCreator->hidePathCreatorModule();
        myPlanCreatorLegend->hidePlanCreatorLegend();
    }
}


void
GNEPersonFrame::demandElementSelected() {
    if (myTypeSelector->getCurrentDemandElement() && myPlanSelector->getCurrentPlanTemplate()) {
        // show person attributes
        myPersonAttributesEditor->showAttributesEditor(myPersonTagSelector->getCurrentTemplateAC(), true);
        // show person plan tag selector
        myPlanSelector->showPlanSelector();
        // now check if person plan selected is valid
        if (myPlanSelector->getCurrentPlanTagProperties()->getTag() != SUMO_TAG_NOTHING) {
            // show person plan attributes
            myPersonPlanAttributesEditor->showAttributesEditor(myPlanSelector->getCurrentPlanTemplate(), false);
            // show edge path creator module
            myPlanCreator->showPlanCreatorModule(myPlanSelector, nullptr);
            // show legend
            myPlanCreatorLegend->showPlanCreatorLegend();
        } else {
            // hide modules
            myPersonPlanAttributesEditor->hideAttributesEditor();
            myPlanCreator->hidePathCreatorModule();
        }
    } else {
        // hide modules
        myPlanSelector->hidePlanSelector();
        myPersonAttributesEditor->hideAttributesEditor();
        myPersonPlanAttributesEditor->hideAttributesEditor();
        myPlanCreator->hidePathCreatorModule();
    }
}


bool
GNEPersonFrame::createPath(const bool /*useLastRoute*/) {
    // first check that all attributes are valid
    if (!myPersonAttributesEditor->checkAttributes(true) || !myPersonPlanAttributesEditor->checkAttributes(true)) {
        return false;
    } else if (myPlanCreator->planCanBeCreated(myPlanSelector->getCurrentPlanTemplate())) {
        // begin undo-redo operation
        myViewNet->getUndoList()->begin(myPersonTagSelector->getCurrentTemplateAC(), "create " +
                                        myPersonTagSelector->getCurrentTemplateAC()->getTagProperty()->getTagStr() + " and " +
                                        myPlanSelector->getCurrentPlanTagProperties()->getTagStr());
        // create person
        GNEDemandElement* person = buildPerson();
        // declare route handler
        GNERouteHandler routeHandler(myViewNet->getNet(), person->getFileBucket(),
                                     myViewNet->getViewParent()->getGNEAppWindows()->isUndoRedoAllowed(), true);
        // check if person and person plan can be created
        if (routeHandler.buildPersonPlan(myPlanSelector->getCurrentPlanTemplate(),
                                         person, myPersonPlanAttributesEditor, myPlanCreator, true)) {
            // end undo-redo operation
            myViewNet->getUndoList()->end();
            // abort path creation
            myPlanCreator->abortPathCreation();
            // refresh person and personPlan attributes
            myPersonAttributesEditor->refreshAttributesEditor();
            myPersonPlanAttributesEditor->refreshAttributesEditor();
            // compute person
            person->computePathElement();
            // enable show all person plans
            myViewNet->getDemandViewOptions().menuCheckShowAllPersonPlans->setChecked(TRUE);
            return true;
        } else {
            // abort person creation
            myViewNet->getUndoList()->abortAllChangeGroups();
            return false;
        }
    } else {
        return false;
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
    SumoXMLTag personTag = myPersonTagSelector->getCurrentTemplateAC()->getTagProperty()->getTag();
    // set tag
    myPersonBaseObject->setTag(personTag);
    // get attribute ad values
    myPersonAttributesEditor->fillSumoBaseObject(myPersonBaseObject);
    // add pType parameter
    myPersonBaseObject->addStringAttribute(SUMO_ATTR_TYPE, myTypeSelector->getCurrentDemandElement()->getID());
    // declare route handler
    GNERouteHandler routeHandler(myViewNet->getNet(), myViewNet->getNet()->getACTemplates()->getTemplateAC(personTag)->getFileBucket(),
                                 myViewNet->getViewParent()->getGNEAppWindows()->isUndoRedoAllowed(), false);
    // check if we're creating a person or personFlow
    if (personTag == SUMO_TAG_PERSON) {
        // Add parameter departure
        if (!myPersonBaseObject->hasStringAttribute(SUMO_ATTR_DEPART) || myPersonBaseObject->getStringAttribute(SUMO_ATTR_DEPART).empty()) {
            myPersonBaseObject->addStringAttribute(SUMO_ATTR_DEPART, "0");
        }
        // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
        SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(myPersonBaseObject->getAllAttributes(), getPredefinedTagsMML(), toString(personTag));
        // obtain person parameters
        SUMOVehicleParameter* personParameters = SUMOVehicleParserHelper::parseVehicleAttributes(SUMO_TAG_PERSON, SUMOSAXAttrs, false, false, false);
        // check personParameters
        if (personParameters) {
            myPersonBaseObject->setVehicleParameter(personParameters);
            // parse vehicle
            routeHandler.parseSumoBaseObject(myPersonBaseObject);
            // delete personParameters
            delete personParameters;
        }
    } else {
        // set begin and end attributes
        if (!myPersonBaseObject->hasStringAttribute(SUMO_ATTR_BEGIN) || myPersonBaseObject->getStringAttribute(SUMO_ATTR_BEGIN).empty()) {
            myPersonBaseObject->addStringAttribute(SUMO_ATTR_BEGIN, "0");
        }
        // adjust poisson value
        if (myPersonBaseObject->hasDoubleAttribute(GNE_ATTR_POISSON)) {
            myPersonBaseObject->addStringAttribute(SUMO_ATTR_PERIOD, "exp(" + toString(myPersonBaseObject->getDoubleAttribute(GNE_ATTR_POISSON)) + ")");
        }
        // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
        SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(myPersonBaseObject->getAllAttributes(), getPredefinedTagsMML(), toString(personTag));
        // obtain personFlow parameters
        SUMOVehicleParameter* personFlowParameters = SUMOVehicleParserHelper::parseFlowAttributes(SUMO_TAG_PERSONFLOW, SUMOSAXAttrs, false, true, 0, SUMOTime_MAX);
        // check personParameters
        if (personFlowParameters) {
            myPersonBaseObject->setVehicleParameter(personFlowParameters);
            // parse vehicle
            routeHandler.parseSumoBaseObject(myPersonBaseObject);
            // delete personParameters
            delete personFlowParameters;
        }
    }
    // refresh person and personPlan attributes
    myPersonAttributesEditor->refreshAttributesEditor();
    myPersonPlanAttributesEditor->refreshAttributesEditor();
    // return created person
    return myViewNet->getNet()->getAttributeCarriers()->retrieveDemandElement(personTag, myPersonBaseObject->getStringAttribute(SUMO_ATTR_ID));
}

/****************************************************************************/
