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
/// @file    GNEVehicleFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2018
///
// The Widget for add Vehicles/Flows/Trips/etc. elements
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/vehicle/SUMOVehicleParserHelper.h>
#include <utils/xml/SUMOSAXAttributesImpl_Cached.h>

#include "GNEVehicleFrame.h"

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEVehicleFrame::HelpCreation - methods
// ---------------------------------------------------------------------------

GNEVehicleFrame::HelpCreation::HelpCreation(GNEVehicleFrame* vehicleFrameParent) :
    MFXGroupBoxModule(vehicleFrameParent, TL("Help")),
    myVehicleFrameParent(vehicleFrameParent) {
    myInformationLabel = new FXLabel(getCollapsableFrame(), "", 0, GUIDesignLabelFrameInformation);
}


GNEVehicleFrame::HelpCreation::~HelpCreation() {}


void
GNEVehicleFrame::HelpCreation::showHelpCreation() {
    // first update help cration
    updateHelpCreation();
    // show modul
    show();
}


void
GNEVehicleFrame::HelpCreation::hideHelpCreation() {
    hide();
}

void
GNEVehicleFrame::HelpCreation::updateHelpCreation() {
    // create information label
    std::ostringstream information;
    // set text depending of selected vehicle type
    switch (myVehicleFrameParent->myVehicleTagSelector->getCurrentTemplateAC()->getTagProperty().getTag()) {
        // vehicles
        case SUMO_TAG_VEHICLE:
            information
                    << TL("- Click over a route to\n")
                    << TL("  create a vehicle.");
            break;
        case SUMO_TAG_TRIP:
            information
                    << TL("- Select two edges to\n")
                    << TL("  create a Trip.");
            break;
        case GNE_TAG_VEHICLE_WITHROUTE:
            information
                    << TL("- Select two edges to\n")
                    << TL("  create a vehicle with\n")
                    << TL("  embedded route.");
            break;
        case GNE_TAG_TRIP_JUNCTIONS:
            information
                    << TL("- Select two junctions\n")
                    << TL("  to create a Trip.");
            break;
        // flows
        case GNE_TAG_FLOW_ROUTE:
            information
                    << TL("- Click over a route to\n")
                    << TL("  create a routeFlow.");
            break;
        case SUMO_TAG_FLOW:
            information
                    << TL("- Select two edges to\n")
                    << TL("  create a flow.");
            break;
        case GNE_TAG_FLOW_WITHROUTE:
            information
                    << TL("- Select two edges to\n")
                    << TL("  create a flow with\n")
                    << TL("  embedded route.");
            break;
        case GNE_TAG_FLOW_JUNCTIONS:
            information
                    << TL("- Select two junctions\n")
                    << TL("  to create a flow.");
            break;
        default:
            break;
    }
    // set information label
    myInformationLabel->setText(information.str().c_str());
}

// ---------------------------------------------------------------------------
// GNEVehicleFrame - methods
// ---------------------------------------------------------------------------

GNEVehicleFrame::GNEVehicleFrame(GNEViewParent* viewParent, GNEViewNet* viewNet) :
    GNEFrame(viewParent, viewNet, "Vehicles"),
    myRouteHandler("", viewNet->getNet(), true, false),
    myVehicleBaseObject(new CommonXMLStructure::SumoBaseObject(nullptr)) {

    // Create item Selector modul for vehicles
    myVehicleTagSelector = new GNETagSelector(this, GNETagProperties::TagType::VEHICLE, SUMO_TAG_TRIP);

    // Create vehicle type selector and set DEFAULT_VTYPE_ID as default element
    myTypeSelector = new DemandElementSelector(this, SUMO_TAG_VTYPE, viewNet->getNet()->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, DEFAULT_VTYPE_ID));

    // Create vehicle parameters
    myVehicleAttributes = new GNEAttributesCreator(this);

    // create GNEPathCreator Module
    myPathCreator = new GNEPathCreator(this);

    // Create Help Creation Module
    myHelpCreation = new HelpCreation(this);

    // create legend label
    myPathLegend = new GNEPathLegendModule(this);
}


GNEVehicleFrame::~GNEVehicleFrame() {
    delete myVehicleBaseObject;
}


void
GNEVehicleFrame::show() {
    // refresh tag selector
    myVehicleTagSelector->refreshTagSelector();
    // show frame
    GNEFrame::show();
}


void
GNEVehicleFrame::hide() {
    // reset edge candidates
    for (const auto& edge : myViewNet->getNet()->getAttributeCarriers()->getEdges()) {
        edge.second->resetCandidateFlags();
    }
    // reset junctioncandidates
    for (const auto& junction : myViewNet->getNet()->getAttributeCarriers()->getJunctions()) {
        junction.second->resetCandidateFlags();
    }
    // hide frame
    GNEFrame::hide();
}


bool
GNEVehicleFrame::addVehicle(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor, const GNEViewNetHelper::MouseButtonKeyPressed& mouseButtonKeyPressed) {
    // check template AC
    if (myVehicleTagSelector->getCurrentTemplateAC() == nullptr) {
        return false;
    }
    // begin cleaning vehicle base object
    myVehicleBaseObject->clear();
    // obtain tag (only for improve code legibility)
    SumoXMLTag vehicleTag = myVehicleTagSelector->getCurrentTemplateAC()->getTagProperty().getTag();
    const bool addEdge = ((vehicleTag == SUMO_TAG_TRIP) || (vehicleTag == GNE_TAG_VEHICLE_WITHROUTE) || (vehicleTag == SUMO_TAG_FLOW) || (vehicleTag == GNE_TAG_FLOW_WITHROUTE));
    const bool addJunction = ((vehicleTag == GNE_TAG_TRIP_JUNCTIONS) || (vehicleTag == GNE_TAG_FLOW_JUNCTIONS));
    // first check that current selected vehicle is valid
    if (vehicleTag == SUMO_TAG_NOTHING) {
        myViewNet->setStatusBarText(TL("Current selected vehicle isn't valid."));
        return false;
    }
    // now check if VType is valid
    if (myTypeSelector->getCurrentDemandElement() == nullptr) {
        myViewNet->setStatusBarText(TL("Current selected vehicle type isn't valid."));
        return false;
    }
    // now check if parameters are valid
    if (!myVehicleAttributes->areValuesValid()) {
        myVehicleAttributes->showWarningMessage();
        return false;
    }
    // get vehicle attributes
    myVehicleAttributes->getAttributesAndValues(myVehicleBaseObject, false);
    // Check if ID has to be generated
    if (!myVehicleBaseObject->hasStringAttribute(SUMO_ATTR_ID)) {
        myVehicleBaseObject->addStringAttribute(SUMO_ATTR_ID, myViewNet->getNet()->getAttributeCarriers()->generateDemandElementID(vehicleTag));
    }
    // add VType
    myVehicleBaseObject->addStringAttribute(SUMO_ATTR_TYPE, myTypeSelector->getCurrentDemandElement()->getID());
    // set route or edges depending of vehicle type
    if ((vehicleTag == SUMO_TAG_VEHICLE) || (vehicleTag == GNE_TAG_FLOW_ROUTE)) {
        return buildVehicleOverRoute(vehicleTag, objectsUnderCursor.getDemandElementFront());
    } else if (addEdge && objectsUnderCursor.getEdgeFront()) {
        // add clicked edge in GNEPathCreator
        return myPathCreator->addEdge(objectsUnderCursor.getEdgeFront(), mouseButtonKeyPressed.shiftKeyPressed(), mouseButtonKeyPressed.controlKeyPressed());
    } else if (addJunction && objectsUnderCursor.getJunctionFront()) {
        // add clicked junction in GNEPathCreator
        return myPathCreator->addJunction(objectsUnderCursor.getJunctionFront(), mouseButtonKeyPressed.shiftKeyPressed(), mouseButtonKeyPressed.controlKeyPressed());
    } else {
        return false;
    }
}


GNETagSelector*
GNEVehicleFrame::getVehicleTagSelector() const {
    return myVehicleTagSelector;
}


GNEPathCreator*
GNEVehicleFrame::getPathCreator() const {
    return myPathCreator;
}

// ===========================================================================
// protected
// ===========================================================================

void
GNEVehicleFrame::tagSelected() {
    if (myVehicleTagSelector->getCurrentTemplateAC()) {
        // show vehicle type selector modul
        myTypeSelector->showDemandElementSelector();
        // show path creator modul
        myPathCreator->showPathCreatorModule(myVehicleTagSelector->getCurrentTemplateAC()->getTagProperty().getTag(), false, false);
        // check if show path legend
        if ((myVehicleTagSelector->getCurrentTemplateAC()->getTagProperty().getTag() != SUMO_TAG_VEHICLE) &&
                (myVehicleTagSelector->getCurrentTemplateAC()->getTagProperty().getTag() != GNE_TAG_FLOW_ROUTE) &&
                (myVehicleTagSelector->getCurrentTemplateAC()->getTagProperty().getTag() != GNE_TAG_TRIP_JUNCTIONS) &&
                (myVehicleTagSelector->getCurrentTemplateAC()->getTagProperty().getTag() != GNE_TAG_FLOW_JUNCTIONS)) {
            myPathLegend->showPathLegendModule();
        } else {
            myPathLegend->hidePathLegendModule();
        }
    } else {
        // hide all moduls if tag isn't valid
        myTypeSelector->hideDemandElementSelector();
        myVehicleAttributes->hideAttributesCreatorModule();
        myPathCreator->hidePathCreatorModule();
        myHelpCreation->hideHelpCreation();
        myPathLegend->hidePathLegendModule();
    }
}


void
GNEVehicleFrame::demandElementSelected() {
    if (myTypeSelector->getCurrentDemandElement()) {
        // show vehicle attributes modul
        myVehicleAttributes->showAttributesCreatorModule(myVehicleTagSelector->getCurrentTemplateAC(), {});
        // clear colors
        myPathCreator->clearJunctionColors();
        myPathCreator->clearEdgeColors();
        // set current VTypeClass in pathCreator
        myPathCreator->setVClass(myTypeSelector->getCurrentDemandElement()->getVClass());
        // show path creator module
        myPathCreator->showPathCreatorModule(myVehicleTagSelector->getCurrentTemplateAC()->getTagProperty().getTag(), false, false);
        // show help creation
        myHelpCreation->showHelpCreation();
        // show warning if we have selected a vType oriented to pedestrians or containers
        if (myTypeSelector->getCurrentDemandElement()->getVClass() == SVC_PEDESTRIAN) {
            WRITE_WARNING(TL("VType with vClass == 'pedestrian' is oriented to pedestrians"));
        } else if (myTypeSelector->getCurrentDemandElement()->getVClass() == SVC_IGNORING) {
            WRITE_WARNING(TL("VType with vClass == 'ignoring' is oriented to containers"));
        }
    } else {
        // hide all moduls if selected item isn't valid
        myVehicleAttributes->hideAttributesCreatorModule();
        myPathCreator->hidePathCreatorModule();
        myPathLegend->hidePathLegendModule();
        myHelpCreation->hideHelpCreation();
    }
}


bool
GNEVehicleFrame::createPath(const bool useLastRoute) {
    // first check if parameters are valid
    if (myVehicleAttributes->areValuesValid() && myTypeSelector->getCurrentDemandElement()) {
        // obtain tag (only for improve code legibility)
        SumoXMLTag vehicleTag = myVehicleTagSelector->getCurrentTemplateAC()->getTagProperty().getTag();
        // begin cleaning vehicle base object
        myVehicleBaseObject->clear();
        // Updated myVehicleBaseObject
        myVehicleAttributes->getAttributesAndValues(myVehicleBaseObject, false);
        // Check if ID has to be generated
        if (!myVehicleBaseObject->hasStringAttribute(SUMO_ATTR_ID)) {
            myVehicleBaseObject->addStringAttribute(SUMO_ATTR_ID, myViewNet->getNet()->getAttributeCarriers()->generateDemandElementID(vehicleTag));
        }
        // add VType
        myVehicleBaseObject->addStringAttribute(SUMO_ATTR_TYPE, myTypeSelector->getCurrentDemandElement()->getID());
        // check if use last route
        if (useLastRoute) {
            // build vehicle using last route
            return buildVehicleOverRoute(vehicleTag, myViewNet->getLastCreatedRoute());
        } else {
            // check number of edges
            if ((myPathCreator->getSelectedEdges().size() > 0) || (myPathCreator->getSelectedJunctions().size() > 0)) {
                // extract via attribute
                std::vector<std::string> viaEdges;
                for (int i = 1; i < ((int)myPathCreator->getSelectedEdges().size() - 1); i++) {
                    viaEdges.push_back(myPathCreator->getSelectedEdges().at(i)->getID());
                }
                // continue depending of tag
                if (vehicleTag == SUMO_TAG_TRIP) {
                    // set tag
                    myVehicleBaseObject->setTag(SUMO_TAG_TRIP);
                    // Add parameter departure
                    if (!myVehicleBaseObject->hasStringAttribute(SUMO_ATTR_DEPART) || myVehicleBaseObject->getStringAttribute(SUMO_ATTR_DEPART).empty()) {
                        myVehicleBaseObject->addStringAttribute(SUMO_ATTR_DEPART, "0");
                    }
                    // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
                    SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(myVehicleBaseObject->getAllAttributes(), getPredefinedTagsMML(), toString(vehicleTag));
                    // obtain trip parameters
                    SUMOVehicleParameter* tripParameters = SUMOVehicleParserHelper::parseVehicleAttributes(vehicleTag, SUMOSAXAttrs, false);
                    // check trip parameters
                    if (tripParameters) {
                        myVehicleBaseObject->setVehicleParameter(tripParameters);
                        myVehicleBaseObject->addStringAttribute(SUMO_ATTR_FROM, myPathCreator->getSelectedEdges().front()->getID());
                        myVehicleBaseObject->addStringAttribute(SUMO_ATTR_TO, myPathCreator->getSelectedEdges().back()->getID());
                        myVehicleBaseObject->addStringListAttribute(SUMO_ATTR_VIA, viaEdges);
                        // parse vehicle
                        myRouteHandler.parseSumoBaseObject(myVehicleBaseObject);
                        // delete tripParameters and base object
                        delete tripParameters;
                    }
                } else if (vehicleTag == GNE_TAG_VEHICLE_WITHROUTE) {
                    // set tag
                    myVehicleBaseObject->setTag(SUMO_TAG_VEHICLE);
                    // Add parameter departure
                    if (!myVehicleBaseObject->hasStringAttribute(SUMO_ATTR_DEPART) || myVehicleBaseObject->getStringAttribute(SUMO_ATTR_DEPART).empty()) {
                        myVehicleBaseObject->addStringAttribute(SUMO_ATTR_DEPART, "0");
                    }
                    // get route edges
                    std::vector<std::string> routeEdges;
                    for (const auto& subPath : myPathCreator->getPath()) {
                        for (const auto& edge : subPath.getSubPath()) {
                            routeEdges.push_back(edge->getID());
                        }
                    }
                    // avoid consecutive duplicated edges
                    routeEdges.erase(std::unique(routeEdges.begin(), routeEdges.end()), routeEdges.end());
                    // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
                    SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(myVehicleBaseObject->getAllAttributes(), getPredefinedTagsMML(), toString(vehicleTag));
                    // obtain vehicle parameters
                    SUMOVehicleParameter* vehicleParameters = SUMOVehicleParserHelper::parseVehicleAttributes(vehicleTag, SUMOSAXAttrs, false);
                    // continue depending of vehicleParameters
                    if (vehicleParameters) {
                        myVehicleBaseObject->setVehicleParameter(vehicleParameters);
                        // create route base object
                        CommonXMLStructure::SumoBaseObject* embeddedRouteObject = new CommonXMLStructure::SumoBaseObject(myVehicleBaseObject);
                        embeddedRouteObject->setTag(SUMO_TAG_ROUTE);
                        embeddedRouteObject->addStringAttribute(SUMO_ATTR_ID, "");
                        embeddedRouteObject->addStringListAttribute(SUMO_ATTR_EDGES, routeEdges);
                        embeddedRouteObject->addColorAttribute(SUMO_ATTR_COLOR, RGBColor::INVISIBLE),
                                            embeddedRouteObject->addIntAttribute(SUMO_ATTR_REPEAT, 0),
                                            embeddedRouteObject->addTimeAttribute(SUMO_ATTR_CYCLETIME, 0),
                                            // parse route
                                            myRouteHandler.parseSumoBaseObject(embeddedRouteObject);
                        // delete vehicleParamters
                        delete vehicleParameters;
                    }
                } else if (vehicleTag == SUMO_TAG_FLOW) {
                    // set tag
                    myVehicleBaseObject->setTag(SUMO_TAG_FLOW);
                    // set begin and end attributes
                    if (!myVehicleBaseObject->hasStringAttribute(SUMO_ATTR_BEGIN) || myVehicleBaseObject->getStringAttribute(SUMO_ATTR_BEGIN).empty()) {
                        myVehicleBaseObject->addStringAttribute(SUMO_ATTR_BEGIN, "0");
                    }
                    // adjust poisson value
                    if (myVehicleBaseObject->hasDoubleAttribute(GNE_ATTR_POISSON)) {
                        myVehicleBaseObject->addStringAttribute(SUMO_ATTR_PERIOD, "exp(" + toString(myVehicleBaseObject->getDoubleAttribute(GNE_ATTR_POISSON)) + ")");
                    }
                    // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
                    SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(myVehicleBaseObject->getAllAttributes(), getPredefinedTagsMML(), toString(vehicleTag));
                    // obtain flow parameters
                    SUMOVehicleParameter* flowParameters = SUMOVehicleParserHelper::parseFlowAttributes(vehicleTag, SUMOSAXAttrs, false, true, 0, SUMOTime_MAX);
                    // check flowParameters
                    if (flowParameters) {
                        myVehicleBaseObject->setVehicleParameter(flowParameters);
                        myVehicleBaseObject->addStringAttribute(SUMO_ATTR_FROM, myPathCreator->getSelectedEdges().front()->getID());
                        myVehicleBaseObject->addStringAttribute(SUMO_ATTR_TO, myPathCreator->getSelectedEdges().back()->getID());
                        myVehicleBaseObject->addStringListAttribute(SUMO_ATTR_VIA, viaEdges);
                        // parse vehicle
                        myRouteHandler.parseSumoBaseObject(myVehicleBaseObject);
                        // delete flowParameters and base object
                        delete flowParameters;
                    }
                } else if (vehicleTag == GNE_TAG_FLOW_WITHROUTE) {
                    // set tag
                    myVehicleBaseObject->setTag(SUMO_TAG_FLOW);
                    // set begin and end attributes
                    if (!myVehicleBaseObject->hasStringAttribute(SUMO_ATTR_BEGIN) || myVehicleBaseObject->getStringAttribute(SUMO_ATTR_BEGIN).empty()) {
                        myVehicleBaseObject->addStringAttribute(SUMO_ATTR_BEGIN, "0");
                    }
                    // adjust poisson value
                    if (myVehicleBaseObject->hasDoubleAttribute(GNE_ATTR_POISSON)) {
                        myVehicleBaseObject->addStringAttribute(SUMO_ATTR_PERIOD, "exp(" + toString(myVehicleBaseObject->getDoubleAttribute(GNE_ATTR_POISSON)) + ")");
                    }
                    // get route edges
                    std::vector<std::string> routeEdges;
                    for (const auto& subPath : myPathCreator->getPath()) {
                        for (const auto& edge : subPath.getSubPath()) {
                            routeEdges.push_back(edge->getID());
                        }
                    }
                    // avoid consecutive duplicated edges
                    routeEdges.erase(std::unique(routeEdges.begin(), routeEdges.end()), routeEdges.end());
                    // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
                    SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(myVehicleBaseObject->getAllAttributes(), getPredefinedTagsMML(), toString(vehicleTag));
                    // obtain flow parameters
                    SUMOVehicleParameter* flowParameters = SUMOVehicleParserHelper::parseFlowAttributes(vehicleTag, SUMOSAXAttrs, false, true, 0, SUMOTime_MAX);
                    // continue depending of vehicleParameters
                    if (flowParameters) {
                        myVehicleBaseObject->setVehicleParameter(flowParameters);
                        // create under base object
                        CommonXMLStructure::SumoBaseObject* embeddedRouteObject = new CommonXMLStructure::SumoBaseObject(myVehicleBaseObject);
                        embeddedRouteObject->setTag(SUMO_TAG_ROUTE);
                        embeddedRouteObject->addStringAttribute(SUMO_ATTR_ID, "");
                        embeddedRouteObject->addStringListAttribute(SUMO_ATTR_EDGES, routeEdges);
                        embeddedRouteObject->addColorAttribute(SUMO_ATTR_COLOR, RGBColor::INVISIBLE),
                                            embeddedRouteObject->addIntAttribute(SUMO_ATTR_REPEAT, 0),
                                            embeddedRouteObject->addTimeAttribute(SUMO_ATTR_CYCLETIME, 0),
                                            // parse route
                                            myRouteHandler.parseSumoBaseObject(embeddedRouteObject);
                        // delete vehicleParamters
                        delete flowParameters;
                    }
                } else if (vehicleTag == GNE_TAG_TRIP_JUNCTIONS) {
                    // set tag
                    myVehicleBaseObject->setTag(SUMO_TAG_TRIP);
                    // Add parameter departure
                    if (!myVehicleBaseObject->hasStringAttribute(SUMO_ATTR_DEPART) || myVehicleBaseObject->getStringAttribute(SUMO_ATTR_DEPART).empty()) {
                        myVehicleBaseObject->addStringAttribute(SUMO_ATTR_DEPART, "0");
                    }
                    // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
                    SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(myVehicleBaseObject->getAllAttributes(), getPredefinedTagsMML(), toString(vehicleTag));
                    // obtain trip parameters
                    SUMOVehicleParameter* tripParameters = SUMOVehicleParserHelper::parseVehicleAttributes(vehicleTag, SUMOSAXAttrs, false);
                    // check trip parameters
                    if (tripParameters) {
                        myVehicleBaseObject->setVehicleParameter(tripParameters);
                        myVehicleBaseObject->addStringAttribute(SUMO_ATTR_FROMJUNCTION, myPathCreator->getSelectedJunctions().front()->getID());
                        myVehicleBaseObject->addStringAttribute(SUMO_ATTR_TOJUNCTION, myPathCreator->getSelectedJunctions().back()->getID());
                        // parse vehicle
                        myRouteHandler.parseSumoBaseObject(myVehicleBaseObject);
                        // delete tripParameters and base object
                        delete tripParameters;
                    }
                } else if (vehicleTag == GNE_TAG_FLOW_JUNCTIONS) {
                    // set tag
                    myVehicleBaseObject->setTag(SUMO_TAG_FLOW);
                    // set begin and end attributes
                    if (!myVehicleBaseObject->hasStringAttribute(SUMO_ATTR_BEGIN) || myVehicleBaseObject->getStringAttribute(SUMO_ATTR_BEGIN).empty()) {
                        myVehicleBaseObject->addStringAttribute(SUMO_ATTR_BEGIN, "0");
                    }
                    // adjust poisson value
                    if (myVehicleBaseObject->hasDoubleAttribute(GNE_ATTR_POISSON)) {
                        myVehicleBaseObject->addStringAttribute(SUMO_ATTR_PERIOD, "exp(" + toString(myVehicleBaseObject->getDoubleAttribute(GNE_ATTR_POISSON)) + ")");
                    }
                    // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
                    SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(myVehicleBaseObject->getAllAttributes(), getPredefinedTagsMML(), toString(vehicleTag));
                    // obtain flow parameters
                    SUMOVehicleParameter* flowParameters = SUMOVehicleParserHelper::parseFlowAttributes(vehicleTag, SUMOSAXAttrs, false, true, 0, SUMOTime_MAX);
                    // check flowParameters
                    if (flowParameters) {
                        myVehicleBaseObject->setVehicleParameter(flowParameters);
                        myVehicleBaseObject->addStringAttribute(SUMO_ATTR_FROMJUNCTION, myPathCreator->getSelectedJunctions().front()->getID());
                        myVehicleBaseObject->addStringAttribute(SUMO_ATTR_TOJUNCTION, myPathCreator->getSelectedJunctions().back()->getID());
                        // parse vehicle
                        myRouteHandler.parseSumoBaseObject(myVehicleBaseObject);
                        // delete flowParameters and base object
                        delete flowParameters;
                    }
                }
                // abort path creation
                myPathCreator->abortPathCreation();
                // refresh myVehicleAttributes
                myVehicleAttributes->refreshAttributesCreator();
                return true;
            }
        }
    }
    return false;
}


bool
GNEVehicleFrame::buildVehicleOverRoute(SumoXMLTag vehicleTag, GNEDemandElement* route) {
    if (route && (route->getTagProperty().isRoute())) {
        // check if departLane is valid
        if ((route->getTagProperty().getTag() == SUMO_TAG_ROUTE) && myVehicleBaseObject->hasStringAttribute(SUMO_ATTR_DEPARTLANE) &&
                GNEAttributeCarrier::canParse<int>(myVehicleBaseObject->getStringAttribute(SUMO_ATTR_DEPARTLANE))) {
            const int departLane = GNEAttributeCarrier::parse<int>(myVehicleBaseObject->getStringAttribute(SUMO_ATTR_DEPARTLANE));
            if (departLane >= (int)route->getParentEdges().front()->getLanes().size()) {
                myViewNet->setStatusBarText("Invalid " + toString(SUMO_ATTR_DEPARTLANE));
                return false;
            }
        }
        // check if departSpeed is valid
        if (myVehicleBaseObject->hasStringAttribute(SUMO_ATTR_DEPARTSPEED) && GNEAttributeCarrier::canParse<double>(myVehicleBaseObject->getStringAttribute(SUMO_ATTR_DEPARTSPEED))) {
            double departSpeed = GNEAttributeCarrier::parse<double>(myVehicleBaseObject->getStringAttribute(SUMO_ATTR_DEPARTSPEED));
            if (departSpeed >= myTypeSelector->getCurrentDemandElement()->getAttributeDouble(SUMO_ATTR_MAXSPEED)) {
                myViewNet->setStatusBarText("Invalid " + toString(SUMO_ATTR_DEPARTSPEED));
                return false;
            }
        }
        // check if we're creating a vehicle or a flow
        if (vehicleTag == SUMO_TAG_VEHICLE) {
            // set tag
            myVehicleBaseObject->setTag(SUMO_TAG_VEHICLE);
            // Add parameter departure
            if (!myVehicleBaseObject->hasStringAttribute(SUMO_ATTR_DEPART) || myVehicleBaseObject->getStringAttribute(SUMO_ATTR_DEPART).empty()) {
                myVehicleBaseObject->addStringAttribute(SUMO_ATTR_DEPART, "0");
            }
            // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
            SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(myVehicleBaseObject->getAllAttributes(), getPredefinedTagsMML(), toString(vehicleTag));
            // obtain vehicle parameters in vehicleParameters
            SUMOVehicleParameter* vehicleParameters = SUMOVehicleParserHelper::parseVehicleAttributes(vehicleTag, SUMOSAXAttrs, false);
            // check if vehicle was successfully created)
            if (vehicleParameters) {
                vehicleParameters->routeid = route->getID();
                myVehicleBaseObject->setVehicleParameter(vehicleParameters);
                // parse vehicle
                myRouteHandler.parseSumoBaseObject(myVehicleBaseObject);
                // delete vehicleParameters and sumoBaseObject
                delete vehicleParameters;
            }
        } else {
            // set tag
            myVehicleBaseObject->setTag(SUMO_TAG_FLOW);
            // set begin and end attributes
            if (!myVehicleBaseObject->hasStringAttribute(SUMO_ATTR_BEGIN) || myVehicleBaseObject->getStringAttribute(SUMO_ATTR_BEGIN).empty()) {
                myVehicleBaseObject->addStringAttribute(SUMO_ATTR_BEGIN, "0");
            }
            if (!myVehicleBaseObject->hasStringAttribute(SUMO_ATTR_END) || myVehicleBaseObject->getStringAttribute(SUMO_ATTR_END).empty()) {
                myVehicleBaseObject->addStringAttribute(SUMO_ATTR_END, "3600");
            }
            // adjust poisson value
            if (myVehicleBaseObject->hasDoubleAttribute(GNE_ATTR_POISSON)) {
                myVehicleBaseObject->addStringAttribute(SUMO_ATTR_PERIOD, "exp(" + toString(myVehicleBaseObject->getDoubleAttribute(GNE_ATTR_POISSON)) + ")");
            }
            // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
            SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(myVehicleBaseObject->getAllAttributes(), getPredefinedTagsMML(), toString(vehicleTag));
            // obtain routeFlow parameters in routeFlowParameters
            SUMOVehicleParameter* routeFlowParameters = SUMOVehicleParserHelper::parseFlowAttributes(vehicleTag, SUMOSAXAttrs, false, true, 0, SUMOTime_MAX);
            // check if flow was successfully created)
            if (routeFlowParameters) {
                routeFlowParameters->routeid = route->getID();
                myVehicleBaseObject->setVehicleParameter(routeFlowParameters);
                // parse flow
                myRouteHandler.parseSumoBaseObject(myVehicleBaseObject);
                // delete vehicleParameters and sumoBaseObject
                delete routeFlowParameters;
            }
        }
        // center view after creation
        const auto* vehicle = myViewNet->getNet()->getAttributeCarriers()->retrieveDemandElement(myVehicleBaseObject->getTag(), myVehicleBaseObject->getStringAttribute(SUMO_ATTR_ID), false);
        if (vehicle && !myViewNet->getVisibleBoundary().around(vehicle->getPositionInView())) {
            myViewNet->centerTo(vehicle->getPositionInView(), false);
        }
        // refresh myVehicleAttributes
        myVehicleAttributes->refreshAttributesCreator();
        // all ok, then return true;
        return true;
    } else {
        myViewNet->setStatusBarText(toString(vehicleTag) + " has to be placed within a route.");
        return false;
    }
}

/****************************************************************************/
