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

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/data/GNEGenericData.h>
#include <netedit/frames/common/GNEInspectorFrame.h>

#include "GNEDataInterval.h"
#include "GNEDataSet.h"


// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEDataInterval - methods
// ---------------------------------------------------------------------------

GNEDataInterval::GNEDataInterval(GNEDataSet* dataSetParent, const double begin, const double end) :
    GNEAttributeCarrier(SUMO_TAG_DATAINTERVAL, dataSetParent->getNet()),
    myDataSetParent(dataSetParent),
    myBegin(begin),
    myEnd(end),
    myAttributeColorsDeprecated(true) {
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


void 
GNEDataInterval::markAttributeColorsDeprecated() {
    myAttributeColorsDeprecated = true;
    // also mark it in data set parent
    myDataSetParent->markAttributeColorsDeprecated();
}


void 
GNEDataInterval::updateAttributeColors() {
    if (myAttributeColorsDeprecated) {
        // first clear container
        myAttributeColors.clear();
        // iterate over generic data children
        for (const auto &genericData : myGenericDataChildren) {
            for (const auto &param : genericData->getParametersMap()) {
                // parse param value
                const double value = parse<double>(param.second);
                // if param doesn't exist, simply add it
                if (myAttributeColors.count(param.first) == 0) {
                    myAttributeColors[param.first] = AttributeColors(value);
                } else {
                    // update min value
                    if (value < myAttributeColors.at(param.first).minValue) {
                        myAttributeColors.at(param.first).minValue = value;
                    }
                    // update max value
                    if (value > myAttributeColors.at(param.first).maxValue) {
                        myAttributeColors.at(param.first).minValue = value;
                    }
                }
            }
        }
        myAttributeColorsDeprecated = false;
    }
}


double 
GNEDataInterval::getMinimumParameterValue(const std::string& parameter) const {
    if (myAttributeColors.count(parameter) > 0) {
        return myAttributeColors.at(parameter).minValue;
    } else {
        return 0;
    }
}


double
GNEDataInterval::getMaximumParameterValue(const std::string& parameter) const {
    if (myAttributeColors.count(parameter) > 0) {
        return myAttributeColors.at(parameter).maxValue;
    } else {
        return 0;
    }
}


const std::string&
GNEDataInterval::getID() const {
    return myDataSetParent->getID();
}


GUIGlObject*
GNEDataInterval::getGUIGlObject() {
    return nullptr;
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
        // mark attributeColors deprecated
        myAttributeColorsDeprecated = true;
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
        // mark attributeColors deprecated
        myAttributeColorsDeprecated = true;
        // remove it from Inspector Frame and AttributeCarrierHierarchy
        myDataSetParent->getNet()->getViewNet()->getViewParent()->getInspectorFrame()->getAttributesEditor()->removeEditedAC(genericData);
        myDataSetParent->getNet()->getViewNet()->getViewParent()->getInspectorFrame()->getAttributeCarrierHierarchy()->removeCurrentEditedAttribute(genericData);
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
            undoList->p_add(new GNEChange_Attribute(this, key, value));
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


GNEDataInterval::AttributeColors::AttributeColors() :
    minValue(0),
    maxValue(0) {
}


GNEDataInterval::AttributeColors::AttributeColors(const double defaultValue) :
    minValue(defaultValue),
    maxValue(defaultValue) {
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
