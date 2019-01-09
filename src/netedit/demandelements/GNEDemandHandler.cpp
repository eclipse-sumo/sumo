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
            case SUMO_TAG_VAPORIZER:
                parseAndBuildVaporizer(attrs, tag);
                break;
            default:
                break;
        }
    }
}


void
GNEDemandHandler::myEndElement(int element) {
    // pop last inserted element
    myHierarchyInsertedDemandElements.popElement();
}


void
GNEDemandHandler::parseAndBuildVaporizer(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    /*
    bool abort = false;
    // parse attributes of Vaporizer
    const std::string edgeID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    double begin = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", tag, SUMO_ATTR_BEGIN, abort);
    double end = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", tag, SUMO_ATTR_END, abort);
    std::string name = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_NAME, abort);
    // Continue if all parameters were successfully loaded
    if (!abort) {
        // get GNEEdge
        GNEEdge* edge = myViewNet->getNet()->retrieveEdge(edgeID, false);
        // check that all parameters are valid
        if (edge == nullptr) {
            WRITE_WARNING("The edge '" + edgeID + "' to use within the " + toString(tag) + " is not known.");
        } else if (myViewNet->getNet()->retrieveDemandElement(tag, edgeID, false) != nullptr) {
            WRITE_WARNING("There is already a " + toString(tag) + " in the edge '" + edgeID + "'.");
        } else if (begin > end) {
            WRITE_WARNING("Time interval of " + toString(tag) + " isn't valid. Attribute '" + toString(SUMO_ATTR_BEGIN) + "' is greater than attribute '" + toString(SUMO_ATTR_END) + "'.");
        } else {
            // build vaporizer
            myHierarchyInsertedDemandElements.commitElementInsertion(buildVaporizer(myViewNet, myUndoDemandElements, edge, begin, end, name));
        }
    }
    */
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
GNEDemandHandler::buildDemandElement(GNEViewNet* viewNet, bool allowUndoRedo, SumoXMLTag tag, std::map<SumoXMLAttr, std::string> values) {
    // create demand element depending of the tag
    switch (tag) {
        case SUMO_TAG_VAPORIZER: {
            /*
            // obtain specify attributes of vaporizer
            GNEEdge* edge = viewNet->getNet()->retrieveEdge(values[SUMO_ATTR_EDGE], false);
            double begin = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_BEGIN]);
            double end = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_END]);
            std::string name = values[SUMO_ATTR_NAME];
            // Build Vaporizer
            if (edge) {
                if (begin > end) {
                    WRITE_WARNING("Time interval of " + toString(tag) + " isn't valid. Attribute '" + toString(SUMO_ATTR_BEGIN) + "' is greater than attribute '" + toString(SUMO_ATTR_END) + "'.");
                } else if (viewNet->getNet()->retrieveDemandElement(tag, edge->getID(), false) == nullptr) {
                    return buildVaporizer(viewNet, allowUndoRedo, edge, begin, end, name);
                } else {
                    WRITE_WARNING("There is already a " + toString(tag) + " in the edge '" + edge->getID() + "'.");
                }
            }
            */
            return nullptr;
        }
        default:
            return nullptr;
    }
}


GNEDemandElement*
GNEDemandHandler::buildBusStop(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNELane* lane, const std::string& startPos, const std::string& endPos, const std::string& name, const std::vector<std::string>& lines, bool friendlyPosition, bool blockMovement) {
    /*
    if (viewNet->getNet()->retrieveDemandElement(SUMO_TAG_BUS_STOP, id, false) == nullptr) {
        GNEBusStop* busStop = new GNEBusStop(id, lane, viewNet, startPos, endPos, name, lines, friendlyPosition, blockMovement);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_BUS_STOP));
            viewNet->getUndoList()->add(new GNEChange_DemandElement(busStop, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertDemandElement(busStop);
            lane->addDemandElementChild(busStop);
            busStop->incRef("buildBusStop");
        }
        return busStop;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_BUS_STOP) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
    */
    return nullptr;
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
