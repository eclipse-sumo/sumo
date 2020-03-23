/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
/// @file    GNEDataInterval.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// A abstract class for data sets
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/data/GNEGenericData.h>
#include <netedit/frames/common/GNEInspectorFrame.h>

#include "GNEDataInterval.h"
#include "GNEDataSet.h"
#include "GNEGenericData.h"


// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEDataInterval - methods
// ---------------------------------------------------------------------------

GNEDataInterval::GNEDataInterval(GNEDataSet* dataSetParent, const double begin, const double end) :
    GNEAttributeCarrier(SUMO_TAG_DATAINTERVAL),
    myDataSetParent(dataSetParent),
    myBegin(begin),
    myEnd(end) {
}


GNEDataInterval::~GNEDataInterval() {}


void 
GNEDataInterval::updateGenericDataIDs() {
    // iterate over generic data childrens
    for (const auto& genericData : myGenericDataChildren) {
        if (genericData->getTagProperty().getTag() == SUMO_TAG_MEANDATA_EDGE) {
            // {dataset}[{begin}m{end}]{edge}
            genericData->setMicrosimID(myDataSetParent->getID() + "[" + toString(myBegin) + "," + toString(myEnd) + "]" + 
            genericData->getParentEdges().front()->getID());
        } else if (genericData->getTagProperty().getTag() == SUMO_TAG_EDGEREL) {
            // {dataset}[{begin}m{end}]{from}->{to}
            genericData->setMicrosimID(myDataSetParent->getID() + "[" + toString(myBegin) + "," + toString(myEnd) + "]" + 
            genericData->getParentEdges().front()->getID() + "->" + genericData->getParentEdges().back()->getID());
        }
    }
}


const std::string& 
GNEDataInterval::getID() const {
    return myDataSetParent->getID();
}


GNEViewNet*
GNEDataInterval::getViewNet() const {
    return myDataSetParent->getViewNet();
}


void
GNEDataInterval::updateGeometry() {
    // nothing to update
}


void
GNEDataInterval::updateDottedContour() {
    // nothing to update
}


Position
GNEDataInterval::getPositionInView() const {
    return Position();
}


bool
GNEDataInterval::isDataIntervalValid() const {
    return true;
}


std::string
GNEDataInterval::getDataIntervalProblem() const {
    return "";
}


void
GNEDataInterval::fixDataIntervalProblem() {
    throw InvalidArgument(getTagStr() + " cannot fix any problem");
}


GNEDataSet*
GNEDataInterval::getDataSetParent() const {
    return myDataSetParent;
}


void
GNEDataInterval::addGenericDataChild(GNEGenericData* genericData) {
    // check that GenericData wasn't previously inserted
    if (!hasGenericDataChild(genericData)) {
        myGenericDataChildren.push_back(genericData);
        // update generic data IDs
        updateGenericDataIDs();
    } else {
        throw ProcessError("GenericData was already inserted");
    }
}


void
GNEDataInterval::removeGenericDataChild(GNEGenericData* genericData) {
    auto it = std::find(myGenericDataChildren.begin(), myGenericDataChildren.end(), genericData);
    // check that GenericData was previously inserted
    if (it != myGenericDataChildren.end()) {
        // remove generic data child
        myGenericDataChildren.erase(it);
        // remove it from Inspector Frame and AttributeCarrierHierarchy
        myDataSetParent->getViewNet()->getViewParent()->getInspectorFrame()->getAttributesEditor()->removeEditedAC(genericData);
        myDataSetParent->getViewNet()->getViewParent()->getInspectorFrame()->getAttributeCarrierHierarchy()->removeCurrentEditedAttribute(genericData);
    } else {
        throw ProcessError("GenericData wasn't previously inserted");
    }
}


bool
GNEDataInterval::hasGenericDataChild(GNEGenericData* genericData) const {
    return std::find(myGenericDataChildren.begin(), myGenericDataChildren.end(), genericData) != myGenericDataChildren.end();
}


const std::vector<GNEGenericData*>&
GNEDataInterval::getGenericDataChildren() const {
    return myGenericDataChildren;
}


double
GNEDataInterval::getMinimumGenericDataChildAttribute(const std::string& paramStr) const {
    double result = INVALID_DOUBLE;
    // iterate over generic data children
    for (const auto& genericData : myGenericDataChildren) {
        // iterate over generic data params
        for (const auto& param : genericData->getParametersMap()) {
            // check paramStr and if attribute can be parsed to double
            if ((param.first == paramStr) && canParse<double>(param.second)) {
                // parse param value
                const double paramDouble = parse<double>(param.second);
                // update result
                if (result == INVALID_DOUBLE) {
                    result = paramDouble;
                } else if (paramDouble < result) {
                    result = paramDouble;
                }
            }
        }
    }
    // return solution depending of result
    if (result == INVALID_DOUBLE) {
        return 0;
    } else {
        return result;
    }
}


double
GNEDataInterval::getMaximunGenericDataChildAttribute(const std::string& paramStr) const {
    double result = INVALID_DOUBLE;
    // iterate over generic data children
    for (const auto& genericData : myGenericDataChildren) {
        // iterate over generic data params
        for (const auto& param : genericData->getParametersMap()) {
            // check paramStr and if attribute can be parsed to double
            if ((param.first == paramStr) && canParse<double>(param.second)) {
                // parse param value
                const double paramDouble = parse<double>(param.second);
                // update result
                if (result == INVALID_DOUBLE) {
                    result = paramDouble;
                } else if (paramDouble > result) {
                    result = paramDouble;
                }
            }
        }
    }
    // return solution depending of result
    if (result == INVALID_DOUBLE) {
        return 0;
    } else {
        return result;
    }
}


void
GNEDataInterval::selectAttributeCarrier(bool /*changeFlag*/) {
    // GNEDataInterval cannot be selected
}


void
GNEDataInterval::unselectAttributeCarrier(bool /*changeFlag*/) {
    // GNEDataInterval cannot be selected
}


bool
GNEDataInterval::isAttributeCarrierSelected() const {
    return false;
}


bool
GNEDataInterval::drawUsingSelectColor() const {
    return false;
}


std::string
GNEDataInterval::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return myDataSetParent->getAttribute(SUMO_ATTR_ID);
        case SUMO_ATTR_BEGIN:
            return toString(myBegin);
        case SUMO_ATTR_END:
            return toString(myEnd);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNEDataInterval::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_BEGIN:
            return myBegin;
        case SUMO_ATTR_END:
            return myEnd;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
    }
}


void
GNEDataInterval::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_BEGIN:
        case SUMO_ATTR_END:
            undoList->p_add(new GNEChange_Attribute(this, myDataSetParent->getViewNet()->getNet(), key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEDataInterval::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_BEGIN:
            return canParse<double>(value);
        case SUMO_ATTR_END:
            return canParse<double>(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEDataInterval::enableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    // Nothing to enable
}


void
GNEDataInterval::disableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    // Nothing to disable
}


bool
GNEDataInterval::isAttributeEnabled(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return false;
        default:
            return true;
    }
}


std::string
GNEDataInterval::getPopUpID() const {
    return getTagStr();
}


std::string
GNEDataInterval::getHierarchyName() const {
    return "interval: " + getAttribute(SUMO_ATTR_BEGIN) + " -> " + getAttribute(SUMO_ATTR_END);
}


void
GNEDataInterval::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_BEGIN:
            myBegin = parse<double>(value);
            // update Generic Data IDs
            updateGenericDataIDs();
            break;
        case SUMO_ATTR_END:
            myEnd = parse<double>(value);
            // update Generic Data IDs
            updateGenericDataIDs();
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEDataInterval::setEnabledAttribute(const int /*enabledAttributes*/) {
    throw InvalidArgument("Nothing to enable");
}

/****************************************************************************/
