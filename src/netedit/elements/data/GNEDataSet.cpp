/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDataSet.cpp
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

#include "GNEDataSet.h"
#include "GNEDataInterval.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEDataSet::GNEDataSet(GNEViewNet* viewNet, const std::string dataSetID) :
    GNEAttributeCarrier(SUMO_TAG_DATASET),
    myViewNet(viewNet),
    myDataSetID(dataSetID) {
}


GNEDataSet::~GNEDataSet() {}


GNEViewNet*
GNEDataSet::getViewNet() const {
    return myViewNet;
}


void 
GNEDataSet::addDataIntervalChild(GNEDataInterval* dataInterval) {
    // check that dataInterval wasn't previously inserted
    if (myDataIntervalChildren.count(dataInterval->getAttributeDouble(SUMO_ATTR_BEGIN)) == 0) {
        myDataIntervalChildren[dataInterval->getAttributeDouble(SUMO_ATTR_BEGIN)] = dataInterval;
    } else {
        throw ProcessError("DataInterval was already inserted");
    }
}


void 
GNEDataSet::removeDataIntervalChild(GNEDataInterval* dataInterval) {
    // check that dataInterval was previously inserted
    if (myDataIntervalChildren.count(dataInterval->getAttributeDouble(SUMO_ATTR_BEGIN)) == 1) {
        myDataIntervalChildren.erase(dataInterval->getAttributeDouble(SUMO_ATTR_BEGIN));
    } else {
        throw ProcessError("DataInterval wasn't previously inserted");
    }
}


void 
GNEDataSet::updateDataIntervalBegin(const double oldBegin) {
    // check that dataInterval was previously inserted
    if (myDataIntervalChildren.count(oldBegin) == 1) {
        // get data interval
        GNEDataInterval* dataInterval = myDataIntervalChildren.at(oldBegin);
        // insert again using new begin
        myDataIntervalChildren[dataInterval->getAttributeDouble(SUMO_ATTR_BEGIN)] = dataInterval;
    } else {
        throw ProcessError("DataInterval wasn't previously inserted");
    }
}


bool 
GNEDataSet::checkNewInterval(const double newBegin, const double newEnd) {
    return checkNewInterval(myDataIntervalChildren, newBegin, newEnd);
}


bool 
GNEDataSet::checkNewBeginEnd(const GNEDataInterval* dataInterval, const double newBegin, const double newEnd) {
    // make a copy of myDataIntervalChildren without dataInterval, and check checkNewInterval
    std::map<const double, GNEDataInterval*> copyOfDataIntervalMap;
    for (const auto &element : myDataIntervalChildren) {
        if (element.second != dataInterval) {
            copyOfDataIntervalMap.insert(element);
        }
    }
    return checkNewInterval(copyOfDataIntervalMap, newBegin, newEnd);
}


const std::map<const double, GNEDataInterval*>& 
GNEDataSet::getDataIntervalChildren() const {
    return myDataIntervalChildren;
}


void 
GNEDataSet::selectAttributeCarrier(bool /*changeFlag*/) {
    // nothing to select
}


void 
GNEDataSet::unselectAttributeCarrier(bool /*changeFlag*/) {
    // nothing to unselect
}


bool
GNEDataSet::isAttributeCarrierSelected() const {
    return false;
}


bool
GNEDataSet::drawUsingSelectColor() const {
    return false;
}


std::string 
GNEDataSet::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return myDataSetID;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double 
GNEDataSet::getAttributeDouble(SumoXMLAttr key) const {
    throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
}


void 
GNEDataSet::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_ID:
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool 
GNEDataSet::isValid(SumoXMLAttr /*key*/, const std::string& /*value*/) {
    return true;
}


void 
GNEDataSet::enableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    // nothing to enable
}


void 
GNEDataSet::disableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    // nothing to disable
}


bool 
GNEDataSet::isAttributeEnabled(SumoXMLAttr /*key*/) const {
    return true;
}


std::string 
GNEDataSet::getPopUpID() const {
    return "";
}


std::string 
GNEDataSet::getHierarchyName() const {
    return "";
}


void 
GNEDataSet::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            /* */
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool 
GNEDataSet::checkNewInterval(const std::map<const double, GNEDataInterval*> &dataIntervalMap, const double newBegin, const double newEnd) {
    if (dataIntervalMap.empty()) {
        return true;
    } else {
        // declare first and last element
        const auto itFirstElement = dataIntervalMap.begin();
        const auto itLastElement = (dataIntervalMap.end()--);
        if (newBegin > newEnd) {
            return false;
        } else if (dataIntervalMap.count(newBegin) == 1) {
            return false;
        } else if (newBegin < itFirstElement->first) {
            return (newEnd <= itFirstElement->first);
        } else if (newBegin > itLastElement->first) {
            return (newBegin >= itLastElement->second->getAttributeDouble(SUMO_ATTR_END));
        } else {
            // iterate over myDataIntervalChildren
            for (auto it = itFirstElement; it != itLastElement; it++) {
                if (newBegin < it->first) {
                    // obtain previous edge
                    auto itPrevious = it;
                    itPrevious--;
                    // check overlapping with end 
                    if (itPrevious->second->getAttributeDouble(SUMO_ATTR_END) < newBegin) {
                        return true;
                    }
                }
            }
        }
        return false;
    }
}

/****************************************************************************/
