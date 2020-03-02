/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/elements/data/GNEGenericData.h>

#include "GNEDataInterval.h"
#include "GNEDataSet.h"


// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEDataInterval - methods
// ---------------------------------------------------------------------------

GNEDataInterval::GNEDataInterval(GNEDataSet *dataSetParent, const double begin, const double end) :
    GNEAttributeCarrier(SUMO_TAG_DATAINTERVAL),
    myDataSetParent(dataSetParent),
    myBegin(begin),
    myEnd(end) {
}


GNEDataInterval::~GNEDataInterval() {}


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
    } else {
        throw ProcessError("GenericData was already inserted");
    }
}


void 
GNEDataInterval::removeGenericDataChild(GNEGenericData* genericData) {
    auto it = std::find(myGenericDataChildren.begin(), myGenericDataChildren.end(), genericData);
    // check that GenericData was previously inserted
    if (it != myGenericDataChildren.end()) {
        myGenericDataChildren.erase(it);
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
GNEDataInterval::getMinimumGenericDataChildAttribute(const std::string &paramStr) const {
    double result = INVALID_DOUBLE;
    // iterate over generic data children
    for (const auto &genericData : myGenericDataChildren) {
        // iterate over generic data params
        for (const auto &param : genericData->getParametersMap()) {
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
GNEDataInterval::getMaximunGenericDataChildAttribute(const std::string &paramStr) const {
    double result = INVALID_DOUBLE;
    // iterate over generic data children
    for (const auto &genericData : myGenericDataChildren) {
        // iterate over generic data params
        for (const auto &param : genericData->getParametersMap()) {
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
            break;
        case SUMO_ATTR_END:
            myEnd = parse<double>(value);
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
