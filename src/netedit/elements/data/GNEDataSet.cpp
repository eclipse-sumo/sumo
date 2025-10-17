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
/// @file    GNEDataSet.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// A abstract class for data sets
/****************************************************************************/

#include <netedit/GNENet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/frames/common/GNEInspectorFrame.h>
#include <netedit/frames/GNEElementTree.h>

#include "GNEDataSet.h"
#include "GNEDataInterval.h"

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEDataSet::AttributeColors - methods
// ---------------------------------------------------------------------------

GNEDataSet::AttributeColors::AttributeColors() {
}


void
GNEDataSet::AttributeColors::updateValues(const std::string& attribute, const double value) {
    // check if exist
    if (myMinMaxValue.count(attribute) == 0) {
        myMinMaxValue[attribute] = std::make_pair(value, value);
    } else {
        // update min value
        if (value < myMinMaxValue.at(attribute).first) {
            myMinMaxValue.at(attribute).first = value;
        }
        // update max value
        if (value > myMinMaxValue.at(attribute).second) {
            myMinMaxValue.at(attribute).second = value;
        }
    }
}


void
GNEDataSet::AttributeColors::updateAllValues(const AttributeColors& attributeColors) {
    // iterate over map
    for (const auto& attributeColor : attributeColors.myMinMaxValue) {
        if (myMinMaxValue.count(attributeColor.first) == 0) {
            myMinMaxValue[attributeColor.first] = attributeColor.second;
        } else {
            // update min value
            if (attributeColor.second.first < myMinMaxValue.at(attributeColor.first).first) {
                myMinMaxValue.at(attributeColor.first).first = attributeColor.second.first;
            }
            // update max value
            if (attributeColor.second.second > myMinMaxValue.at(attributeColor.first).second) {
                myMinMaxValue.at(attributeColor.first).second = attributeColor.second.second;
            }
        }
    }
}


bool
GNEDataSet::AttributeColors::exist(const std::string& attribute) const {
    return (myMinMaxValue.count(attribute) > 0);
}


double
GNEDataSet::AttributeColors::getMinValue(const std::string& attribute) const {
    return myMinMaxValue.at(attribute).first;
}


double
GNEDataSet::AttributeColors::getMaxValue(const std::string& attribute) const {
    return myMinMaxValue.at(attribute).second;
}


void
GNEDataSet::AttributeColors::clear() {
    myMinMaxValue.clear();
}

// ---------------------------------------------------------------------------
// GNEDataSet - methods
// ---------------------------------------------------------------------------

GNEDataSet::GNEDataSet(const std::string& dataSetID, GNENet* net, const std::string& filename) :
    GNEAttributeCarrier(SUMO_TAG_DATASET, net, filename, false),
    myDataSetID(dataSetID) {
}


GNEDataSet::~GNEDataSet() {}


GNEHierarchicalElement*
GNEDataSet::getHierarchicalElement() {
    return this;
}


GNEMoveElement*
GNEDataSet::getMoveElement() const {
    return nullptr;
}


Parameterised*
GNEDataSet::getParameters() {
    return nullptr;
}


const Parameterised*
GNEDataSet::getParameters() const {
    return nullptr;
}


GUIGlObject*
GNEDataSet::getGUIGlObject() {
    return nullptr;
}


const GUIGlObject*
GNEDataSet::getGUIGlObject() const {
    return nullptr;
}


void
GNEDataSet::updateAttributeColors() {
    // first update attribute colors in data interval childrens
    for (const auto& interval : myDataIntervalChildren) {
        interval.second->updateAttributeColors();
    }
    // continue with data sets containers
    myAllAttributeColors.clear();
    mySpecificAttributeColors.clear();
    // iterate over all data interval children
    for (const auto& interval : myDataIntervalChildren) {
        myAllAttributeColors.updateAllValues(interval.second->getAllAttributeColors());
    }
    // iterate over specificdata interval children
    for (const auto& interval : myDataIntervalChildren) {
        for (const auto& specificAttributeColor : interval.second->getSpecificAttributeColors()) {
            mySpecificAttributeColors[specificAttributeColor.first].updateAllValues(specificAttributeColor.second);
        }
    }
}


const GNEDataSet::AttributeColors&
GNEDataSet::getAllAttributeColors() const {
    return myAllAttributeColors;
}


const std::map<SumoXMLTag, GNEDataSet::AttributeColors>&
GNEDataSet::getSpecificAttributeColors() const {
    return mySpecificAttributeColors;
}


void
GNEDataSet::updateGeometry() {
    // nothing to update
}


Position
GNEDataSet::getPositionInView() const {
    return Position(0, 0);
}


void
GNEDataSet::writeDataSet(OutputDevice& device) const {
    // iterate over intervals
    for (const auto& interval : myDataIntervalChildren) {
        // open device
        device.openTag(SUMO_TAG_INTERVAL);
        // write ID
        device.writeAttr(SUMO_ATTR_ID, getID());
        // write begin
        device.writeAttr(SUMO_ATTR_BEGIN, interval.second->getAttribute(SUMO_ATTR_BEGIN));
        // write end
        device.writeAttr(SUMO_ATTR_END, interval.second->getAttribute(SUMO_ATTR_END));
        // iterate over interval generic datas
        for (const auto& genericData : interval.second->getGenericDataChildren()) {
            // write generic data
            genericData->writeGenericData(device);
        }
        // close device
        device.closeTag();
    }
}


bool
GNEDataSet::checkDrawFromContour() const {
    return false;
}


bool
GNEDataSet::checkDrawToContour() const {
    return false;
}


bool
GNEDataSet::checkDrawRelatedContour() const {
    return false;
}


bool
GNEDataSet::checkDrawOverContour() const {
    return false;
}


bool
GNEDataSet::checkDrawDeleteContour() const {
    return false;
}


bool
GNEDataSet::checkDrawDeleteContourSmall() const {
    return false;
}


bool
GNEDataSet::checkDrawSelectContour() const {
    return false;
}


bool
GNEDataSet::checkDrawMoveContour() const {
    return false;
}


void
GNEDataSet::addDataIntervalChild(GNEDataInterval* dataInterval) {
    // check that dataInterval wasn't previously inserted
    if (myDataIntervalChildren.count(dataInterval->getAttributeDouble(SUMO_ATTR_BEGIN)) == 0) {
        // add data interval child
        myDataIntervalChildren[dataInterval->getAttributeDouble(SUMO_ATTR_BEGIN)] = dataInterval;
        // add reference in attributeCarriers
        myNet->getAttributeCarriers()->insertDataInterval(dataInterval, dataInterval);
    } else {
        throw ProcessError(TL("DataInterval was already inserted"));
    }
}


void
GNEDataSet::removeDataIntervalChild(GNEDataInterval* dataInterval) {
    // check that dataInterval was previously inserted
    if (myDataIntervalChildren.count(dataInterval->getAttributeDouble(SUMO_ATTR_BEGIN)) == 1) {
        // remove data interval child
        myDataIntervalChildren.erase(dataInterval->getAttributeDouble(SUMO_ATTR_BEGIN));
        // remove it from inspected elements and GNEElementTree
        myNet->getViewNet()->getInspectedElements().uninspectAC(dataInterval);
        myNet->getViewNet()->getViewParent()->getInspectorFrame()->getHierarchicalElementTree()->removeCurrentEditedAttributeCarrier(dataInterval);
        // remove reference from attributeCarriers
        myNet->getAttributeCarriers()->deleteDataInterval(dataInterval);
    } else {
        throw ProcessError(TL("DataInterval wasn't previously inserted"));
    }
}


bool
GNEDataSet::dataIntervalChildrenExist(GNEDataInterval* dataInterval) const {
    for (const auto& interval : myDataIntervalChildren) {
        if (interval.second == dataInterval) {
            return true;
        }
    }
    return false;
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
        throw ProcessError(TL("DataInterval wasn't previously inserted"));
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
    for (const auto& element : myDataIntervalChildren) {
        if (element.second != dataInterval) {
            copyOfDataIntervalMap.insert(element);
        }
    }
    return checkNewInterval(copyOfDataIntervalMap, newBegin, newEnd);
}


GNEDataInterval*
GNEDataSet::retrieveInterval(const double begin, const double end) const {
    if (myDataIntervalChildren.count(begin) == 0) {
        return nullptr;
    } else if (myDataIntervalChildren.at(begin)->getAttributeDouble(SUMO_ATTR_END) != end) {
        return nullptr;
    } else {
        return myDataIntervalChildren.at(begin);
    }
}


const std::map<const double, GNEDataInterval*>&
GNEDataSet::getDataIntervalChildren() const {
    return myDataIntervalChildren;
}


std::string
GNEDataSet::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return myDataSetID;
        default:
            return getCommonAttribute(key);
    }
}


double
GNEDataSet::getAttributeDouble(SumoXMLAttr key) const {
    return getCommonAttributeDouble(key);
}


Position
GNEDataSet::getAttributePosition(SumoXMLAttr key) const {
    return getCommonAttributePosition(key);
}


PositionVector GNEDataSet::getAttributePositionVector(SumoXMLAttr key) const {
    return getCommonAttributePositionVector(key);
}


void
GNEDataSet::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_ID:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            setCommonAttribute(key, value, undoList);
            break;
    }
}


bool
GNEDataSet::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            if (SUMOXMLDefinitions::isValidNetID(value) && (myNet->getAttributeCarriers()->retrieveDataSet(value, false) == nullptr)) {
                return true;
            } else {
                return false;
            }
        default:
            return isCommonAttributeValid(key, value);
    }
}


std::string
GNEDataSet::getPopUpID() const {
    return getTagStr();
}


std::string
GNEDataSet::getHierarchyName() const {
    return getTagStr() + ": " + myDataSetID;
}


void
GNEDataSet::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            myDataSetID = value;
            // update all intervals
            for (const auto& interval : myDataIntervalChildren) {
                interval.second->updateGenericDataIDs();
            }
            break;
        default:
            setCommonAttribute(key, value);
            break;
    }
    // mark interval toolbar for update
    myNet->getViewNet()->getIntervalBar().markForUpdate();
}


bool
GNEDataSet::checkNewInterval(const std::map<const double, GNEDataInterval*>& dataIntervalMap, const double newBegin, const double newEnd) {
    if (dataIntervalMap.empty()) {
        return true;
    } else {
        // declare first and last element
        const auto itFirstElement = dataIntervalMap.begin();
        const auto itLastElement = dataIntervalMap.rbegin();
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
            for (auto it = itFirstElement; it != dataIntervalMap.end(); it++) {
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
