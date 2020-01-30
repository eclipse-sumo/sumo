/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDataHandler.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// Builds data objects for netedit
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/common/MsgHandler.h>
#include <netedit/elements/data/GNEEdgeData.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNENet.h>

#include "GNEDataHandler.h"


// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEAdditionalHandler::HierarchyInsertedDatas method definitions
// ---------------------------------------------------------------------------

void
GNEDataHandler::HierarchyInsertedDatas::insertElement(SumoXMLTag tag) {
    myInsertedElements.push_back(std::make_pair(tag, nullptr));
}


void
GNEDataHandler::HierarchyInsertedDatas::commitElementInsertion(GNEGenericData* genericData) {
    myInsertedElements.back().second = genericData;
}


void
GNEDataHandler::HierarchyInsertedDatas::popElement() {
    if (!myInsertedElements.empty()) {
        myInsertedElements.pop_back();
    }
}


GNEGenericData*
GNEDataHandler::HierarchyInsertedDatas::retrieveParentGenericData(GNEViewNet* viewNet, SumoXMLTag expectedTag) const {
    if (myInsertedElements.size() < 2) {
        // currently we're finding parent additional in the additional XML root
        WRITE_WARNING("A " + toString(myInsertedElements.back().first) + " must be declared within the definition of a " + toString(expectedTag) + ".");
        return nullptr;
    } else {
        if (myInsertedElements.size() < 2) {
            // additional was hierarchically bad loaded, then return nullptr
            return nullptr;
        } else if ((myInsertedElements.end() - 2)->second == nullptr) {
            WRITE_WARNING(toString(expectedTag) + " parent of " + toString((myInsertedElements.end() - 1)->first) + " was not loaded sucesfully.");
            // parent additional wasn't sucesfully loaded, then return nullptr
            return nullptr;
        }
        /*
        GNEGenericData* retrievedAdditional = viewNet->getNet()->retrieveAdditional((myInsertedElements.end() - 2)->first, (myInsertedElements.end() - 2)->second->getID(), false);
        if (retrievedAdditional == nullptr) {
            // additional doesn't exist
            WRITE_WARNING("A " + toString((myInsertedElements.end() - 1)->first) + " must be declared within the definition of a " + toString(expectedTag) + ".");
            return nullptr;
        } else if (retrievedAdditional->getTagProperty().getTag() != expectedTag) {
            // invalid parent additional
            WRITE_WARNING("A " + toString((myInsertedElements.end() - 1)->first) + " cannot be declared within the definition of a " + retrievedAdditional->getTagStr() + ".");
            return nullptr;
        } else {
            return retrievedAdditional;
        }
        */
        return nullptr;
    }
}


GNEGenericData*
GNEDataHandler::HierarchyInsertedDatas::getLastInsertedGenericData() const {
    // ierate in reverse mode over myInsertedElements to obtain last inserted additional
    for (std::vector<std::pair<SumoXMLTag, GNEGenericData*> >::const_reverse_iterator i = myInsertedElements.rbegin(); i != myInsertedElements.rend(); i++) {
        // we need to avoid Tag Param because isn't an additional
        if (i->first != SUMO_TAG_PARAM) {
            return i->second;
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------
// GNEAdditionalHandler::HierarchyInsertedDatas method definitions
// ---------------------------------------------------------------------------

GNEDataHandler::GNEDataHandler(const std::string& file, GNEViewNet* viewNet) :
    SUMOSAXHandler(file),
    myViewNet(viewNet) {
}


GNEDataHandler::~GNEDataHandler() {}


void
GNEDataHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    // Obtain tag of element
    SumoXMLTag tag = static_cast<SumoXMLTag>(element);
    // check if we're parsing a parameter
    if (tag == SUMO_TAG_PARAM) {
        // push element int stack
        myHierarchyInsertedGenericDatas.insertElement(tag);
        // parse parameter
        parseParameter(attrs);
    } else if (tag != SUMO_TAG_NOTHING) {
        // push element int stack
        myHierarchyInsertedGenericDatas.insertElement(tag);
        // build data
        buildData(myViewNet, true, tag, attrs, &myHierarchyInsertedGenericDatas);
    }
}


void
GNEDataHandler::myEndElement(int /*element*/) {
    // pop last inserted element
    myHierarchyInsertedGenericDatas.popElement();
}


bool
GNEDataHandler::buildData(GNEViewNet* viewNet, bool allowUndoRedo, SumoXMLTag tag, const SUMOSAXAttributes& attrs, HierarchyInsertedDatas* insertedDatas) {
    // Call parse and build depending of tag
    switch (tag) {
        case SUMO_TAG_MEANDATA_EDGE:
            return parseAndBuildEdgeData(viewNet, allowUndoRedo, attrs, insertedDatas);
        default:
            return false;
    }
}


GNEEdgeData*
GNEDataHandler::buildEdgeData(GNEViewNet* viewNet, bool allowUndoRedo, GNEDataInterval *dataInterval, GNEEdge* edge) {
    /*
    if (viewNet->getNet()->retrieveData(SUMO_TAG_BUS_STOP, id, false) == nullptr) {
        GNEBusStop* busStop = new GNEBusStop(id, lane, viewNet, startPos, endPos, parametersSet, name, lines, personCapacity, friendlyPosition, blockMovement);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_BUS_STOP));
            viewNet->getUndoList()->add(new GNEChange_Data(busStop, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertData(busStop);
            lane->addChildData(busStop);
            busStop->incRef("buildBusStop");
        }
        return busStop;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_BUS_STOP) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
    */
    return nullptr;
}

bool
GNEDataHandler::parseAndBuildEdgeData(GNEViewNet* viewNet, bool allowUndoRedo, const SUMOSAXAttributes& attrs, HierarchyInsertedDatas* insertedDatas) {
    bool abort = false;
    // parse edgeData attributes
    std::string edgeID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", SUMO_TAG_MEANDATA_EDGE, SUMO_ATTR_ID, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get pointer to edge
        GNEEdge* edge = viewNet->getNet()->retrieveEdge(edgeID, false);
        // check that edge is valid
        if (edge == nullptr) {
            // Write error if lane isn't valid
            WRITE_WARNING("The edge '" + edgeID + "' to use within the " + toString(SUMO_TAG_MEANDATA_EDGE) + " '" + edgeID + "' is not known.");
        } else {
            // save ID of last created element
            GNEGenericData* dataCreated = buildEdgeData(viewNet, allowUndoRedo, nullptr, edge);
            // check if insertion has to be commited
            if (insertedDatas) {
                insertedDatas->commitElementInsertion(dataCreated);
            }
            return true;
        }
    }
    return false;
}



// ===========================================================================
// private method definitions
// ===========================================================================

void
GNEDataHandler::parseParameter(const SUMOSAXAttributes& attrs) {
    if (myHierarchyInsertedGenericDatas.getLastInsertedGenericData()) {
        // first check if given data supports parameters
        if (myHierarchyInsertedGenericDatas.getLastInsertedGenericData()->getTagProperty().hasParameters()) {
            bool ok = true;
            std::string key;
            if (attrs.hasAttribute(SUMO_ATTR_KEY)) {
                // obtain key
                key = attrs.get<std::string>(SUMO_ATTR_KEY, nullptr, ok);
                if (key.empty()) {
                    WRITE_WARNING("Error parsing key from data parameter. Key cannot be empty");
                    ok = false;
                }
                if (!SUMOXMLDefinitions::isValidTypeID(key)) {
                    WRITE_WARNING("Error parsing key from data parameter. Key contains invalid characters");
                    ok = false;
                }
            } else {
                WRITE_WARNING("Error parsing key from data parameter. Key doesn't exist");
                ok = false;
            }
            // circumventing empty string test
            const std::string val = attrs.hasAttribute(SUMO_ATTR_VALUE) ? attrs.getString(SUMO_ATTR_VALUE) : "";
            if (!SUMOXMLDefinitions::isValidAttribute(val)) {
                WRITE_WARNING("Error parsing value from data parameter. Value contains invalid characters");
                ok = false;
            }
            // set parameter in last inserted data
            if (ok) {
                WRITE_DEBUG("Inserting parameter '" + key + "|" + val + "' into data " + myHierarchyInsertedGenericDatas.getLastInsertedGenericData()->getTagStr() + ".");
                myHierarchyInsertedGenericDatas.getLastInsertedGenericData()->setParameter(key, val);
            }
        } else {
            WRITE_WARNING("Datas of type '" + myHierarchyInsertedGenericDatas.getLastInsertedGenericData()->getTagStr() + "' doesn't support parameters");
        }
    } else {
        WRITE_WARNING("Parameters has to be declared within the definition of an data");
    }
}

/****************************************************************************/
