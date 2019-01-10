/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDemandHandler.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2019
/// @version $Id$
///
// Builds demand objects for netedit
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>
#include <utils/xml/XMLSubSys.h>
#include <netedit/changes/GNEChange_DemandElement.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNENet.h>
#include <utils/options/OptionsCont.h>

#include "GNEDemandHandler.h"
#include "GNERoute.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEDemandHandler::GNEDemandHandler(const std::string& file, GNEViewNet* viewNet, bool undoDemandElements, GNEDemandElement* demandElementParent) :
    SUMOSAXHandler(file),
    myViewNet(viewNet),
    myUndoDemandElements(undoDemandElements),
    myDemandElementParent(demandElementParent) {
    if (demandElementParent) {
        myHierarchyInsertedDemandElements.insertElement(demandElementParent->getTagProperty().getTag());
        myHierarchyInsertedDemandElements.commitElementInsertion(demandElementParent);
    }
}


GNEDemandHandler::~GNEDemandHandler() {}


void
GNEDemandHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    // Obtain tag of element
    SumoXMLTag tag = static_cast<SumoXMLTag>(element);
    // check if we're parsing a generic parameter
    if (tag == SUMO_TAG_PARAM) {
        // push element int stack
        myHierarchyInsertedDemandElements.insertElement(tag);
        // parse generic parameter
        parseGenericParameter(attrs);
    } else if (tag != SUMO_TAG_NOTHING) {
        // push element int stack
        myHierarchyInsertedDemandElements.insertElement(tag);
        // Call parse and build depending of tag
        switch (tag) {
            case SUMO_TAG_ROUTE:
                parseAndBuildRoute(attrs, tag);
                break;
            default:
                break;
        }
    }
}


void
GNEDemandHandler::myEndElement(int /*element*/) {
    // pop last inserted element
    myHierarchyInsertedDemandElements.popElement();
}


void
GNEDemandHandler::parseAndBuildRoute(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attribute of calibrator routes
    std::string routeID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    std::string edgeIDs = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, routeID, tag, SUMO_ATTR_EDGES, abort);
    RGBColor color = GNEAttributeCarrier::parseAttributeFromXML<RGBColor>(attrs, routeID, tag, SUMO_ATTR_COLOR, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // obtain edges (And show warnings if isn't valid)
        std::vector<GNEEdge*> edges;
        if (GNEAttributeCarrier::canParse<std::vector<GNEEdge*> >(myViewNet->getNet(), edgeIDs, true)) {
            edges = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(myViewNet->getNet(), edgeIDs);
        }
        // check that all elements are valid
        if (myViewNet->getNet()->retrieveAdditional(SUMO_TAG_ROUTE, routeID, false) != nullptr) {
            WRITE_WARNING("There is another " + toString(tag) + " with the same ID='" + routeID + "'.");
        } else if (edges.size() == 0) {
            WRITE_WARNING("Routes needs at least one edge.");
        } else {
            // save ID of last created element
            myHierarchyInsertedDemandElements.commitElementInsertion(buildRoute(myViewNet, myUndoDemandElements, routeID, edges, color));
        }
    }
}


void
GNEDemandHandler::parseGenericParameter(const SUMOSAXAttributes& attrs) {
    if (myHierarchyInsertedDemandElements.getLastInsertedDemandElement()) {
        // first check if given demand element supports generic parameters
        if (myHierarchyInsertedDemandElements.getLastInsertedDemandElement()->getTagProperty().hasGenericParameters()) {
            bool ok = true;
            std::string key;
            if (attrs.hasAttribute(SUMO_ATTR_KEY)) {
                // obtain key
                key = attrs.get<std::string>(SUMO_ATTR_KEY, nullptr, ok);
                if (key.empty()) {
                    WRITE_WARNING("Error parsing key from demand element generic parameter. Key cannot be empty");
                    ok = false;
                }
                if (!SUMOXMLDefinitions::isValidTypeID(key)) {
                    WRITE_WARNING("Error parsing key from demand element generic parameter. Key contains invalid characters");
                    ok = false;
                }
            } else {
                WRITE_WARNING("Error parsing key from demand element generic parameter. Key doesn't exist");
                ok = false;
            }
            // circumventing empty string test
            const std::string val = attrs.hasAttribute(SUMO_ATTR_VALUE) ? attrs.getString(SUMO_ATTR_VALUE) : "";
            if (!SUMOXMLDefinitions::isValidAttribute(val)) {
                WRITE_WARNING("Error parsing value from demand element generic parameter. Value contains invalid characters");
                ok = false;
            }
            // set parameter in last inserted demand element
            if (ok) {
                WRITE_DEBUG("Inserting generic parameter '" + key + "|" + val + "' into demand element " + myHierarchyInsertedDemandElements.getLastInsertedDemandElement()->getTagStr() + ".");
                /*
                myHierarchyInsertedDemandElements.getLastInsertedDemandElement()->setParameter(key, val);
                */
            }
        } else {
        WRITE_WARNING("DemandElements of type '" + myHierarchyInsertedDemandElements.getLastInsertedDemandElement()->getTagStr() + "' doesn't support Generic Parameters");
        }
    } else {
        WRITE_WARNING("Generic Parameters has to be declared within the definition of an demand element element");
    }
}


GNEDemandElement*
GNEDemandHandler::buildRoute(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& routeID, const std::vector<GNEEdge*>& edges, const RGBColor& color) {
    if (viewNet->getNet()->retrieveDemandElement(SUMO_TAG_ROUTE, routeID, false) == nullptr) {
        // create route
        GNERoute* route = new GNERoute(viewNet, routeID, edges, color);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + route->getTagStr());
            viewNet->getUndoList()->add(new GNEChange_DemandElement(route, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertDemandElement(route);
            route->incRef("buildRoute");
        }
        return route;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_ROUTE) + " with ID '" + routeID + "' in netedit; probably declared twice.");
    }
}


void
GNEDemandHandler::HierarchyInsertedDemandElements::insertElement(SumoXMLTag tag) {
    myInsertedElements.push_back(std::make_pair(tag, nullptr));
}


void
GNEDemandHandler::HierarchyInsertedDemandElements::commitElementInsertion(GNEDemandElement* demandElement) {
    myInsertedElements.back().second = demandElement;
}


void
GNEDemandHandler::HierarchyInsertedDemandElements::popElement() {
    if (!myInsertedElements.empty()) {
        myInsertedElements.pop_back();
    }
}


GNEDemandElement*
GNEDemandHandler::HierarchyInsertedDemandElements::retrieveDemandElementParent(GNEViewNet* viewNet, SumoXMLTag expectedTag) const {
    if (myInsertedElements.size() < 2) {
        // currently we're finding demand element parent in the demand element XML root
        WRITE_WARNING("A " + toString(myInsertedElements.back().first) + " must be declared within the definition of a " + toString(expectedTag) + ".");
        return nullptr;
    } else {
        if(myInsertedElements.size() < 2) {
            // demand element was hierarchically bad loaded, then return nullptr
            return nullptr;
        } else if ((myInsertedElements.end() - 2)->second == nullptr) {
            WRITE_WARNING(toString(expectedTag) + " parent of " + toString((myInsertedElements.end() - 1)->first) + " was not loaded sucesfully.");
            // demand element parent wasn't sucesfully loaded, then return nullptr
            return nullptr;
        }
        GNEDemandElement* retrievedDemandElement = viewNet->getNet()->retrieveDemandElement((myInsertedElements.end() - 2)->first, (myInsertedElements.end() - 2)->second->getID(), false);
        if (retrievedDemandElement == nullptr) {
            // demand element doesn't exist
            WRITE_WARNING("A " + toString((myInsertedElements.end() - 1)->first) + " must be declared within the definition of a " + toString(expectedTag) + ".");
            return nullptr;
        } else if (retrievedDemandElement->getTagProperty().getTag() != expectedTag) {
            // invalid demand element parent
            WRITE_WARNING("A " + toString((myInsertedElements.end() - 1)->first) + " cannot be declared within the definition of a " + retrievedDemandElement->getTagStr() + ".");
            return nullptr;
        } else {
            return retrievedDemandElement;
        }
    }
}


GNEDemandElement* 
GNEDemandHandler::HierarchyInsertedDemandElements::getLastInsertedDemandElement() const {
    // ierate in reverse mode over myInsertedElements to obtain last inserted demand element
    for (std::vector<std::pair<SumoXMLTag, GNEDemandElement*> >::const_reverse_iterator i = myInsertedElements.rbegin(); i != myInsertedElements.rend(); i++) {
        // we need to avoid Tag Param because isn't an demand element
        if (i->first != SUMO_TAG_PARAM) {
            return i->second;
        }
    }
    return nullptr;
}

/****************************************************************************/
