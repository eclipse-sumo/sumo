/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
#include <netedit/frames/common/GNEInspectorFrame.h>

#include "GNEDataInterval.h"


// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEDataInterval - methods
// ---------------------------------------------------------------------------

GNEDataInterval::GNEDataInterval(GNEDataSet* dataSetParent, const double begin, const double end) :
    GNEHierarchicalElement(dataSetParent->getNet(), SUMO_TAG_DATAINTERVAL, {}, {}, {}, {}, {}, {}, {}, {}),
                       myDataSetParent(dataSetParent),
                       myBegin(begin),
myEnd(end) {
}


GNEDataInterval::~GNEDataInterval() {}


void
GNEDataInterval::updateGenericDataIDs() {
    if (myNet->isUpdateDataEnabled()) {
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
}


void
GNEDataInterval::updateAttributeColors() {
    if (myNet->isUpdateDataEnabled()) {
        // first clear both container
        myAllAttributeColors.clear();
        mySpecificAttributeColors.clear();
        // iterate over generic data children
        for (const auto& genericData : myGenericDataChildren) {
            for (const auto& param : genericData->getParametersMap()) {
                // check if value can be parsed
                if (canParse<double>(param.second)) {
                    // parse param value
                    const double value = parse<double>(param.second);
                    // update values in both containers
                    myAllAttributeColors.updateValues(param.first, value);
                    mySpecificAttributeColors[genericData->getTagProperty().getTag()].updateValues(param.first, value);
                }
            }
        }
    }
}


const GNEDataSet::AttributeColors&
GNEDataInterval::getAllAttributeColors() const {
    return myAllAttributeColors;
}


const std::map<SumoXMLTag, GNEDataSet::AttributeColors>&
GNEDataInterval::getSpecificAttributeColors() const {
    return mySpecificAttributeColors;
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
        // check if add to boundary
        if (genericData->getTagProperty().isPlacedInRTree()) {
            myNet->getGrid().addAdditionalGLObject(genericData->getGUIGlObject());
        }
        // update geometry after insertion if myUpdateGeometryEnabled is enabled
        if (myNet->isUpdateGeometryEnabled()) {
            // update generic data RTREE
            genericData->updateGeometry();
        }
        // add reference in attributeCarriers
        myNet->getAttributeCarriers()->insertGenericData(genericData);
        // update colors
        genericData->getDataIntervalParent()->getDataSetParent()->updateAttributeColors();
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
        // remove it from inspected ACs and HierarchicalElementTree
        myDataSetParent->getNet()->getViewNet()->removeFromAttributeCarrierInspected(genericData);
        myDataSetParent->getNet()->getViewNet()->getViewParent()->getInspectorFrame()->getHierarchicalElementTree()->removeCurrentEditedAttributeCarrier(genericData);
        // update colors
        genericData->getDataIntervalParent()->getDataSetParent()->updateAttributeColors();
        // delete path element
        myNet->getPathManager()->removePath(genericData);
        // check if remove from RTREE
        if (genericData->getTagProperty().isPlacedInRTree()) {
            myNet->getGrid().removeAdditionalGLObject(genericData->getGUIGlObject());
        }
        // remove reference from attributeCarriers
        myNet->getAttributeCarriers()->deleteGenericData(genericData);
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


bool
GNEDataInterval::TAZRelExists(const GNETAZElement* TAZ) const {
    // interate over all generic datas and check TAZ parents
    for (const auto& genericData : myGenericDataChildren) {
        if ((genericData->getTagProperty().getTag() == SUMO_TAG_TAZREL) &&
                (genericData->getParentTAZElements().size() == 1) &&
                (genericData->getParentTAZElements().front() == TAZ)) {
            return true;
        }
    }
    return false;
}


bool
GNEDataInterval::TAZRelExists(const GNETAZElement* fromTAZ, const GNETAZElement* toTAZ) const {
    // interate over all generic datas and check TAZ parents
    for (const auto& genericData : myGenericDataChildren) {
        if ((genericData->getTagProperty().getTag() == SUMO_TAG_TAZREL) &&
                (genericData->getParentTAZElements().size() == 2) &&
                (genericData->getParentTAZElements().front() == fromTAZ) &&
                (genericData->getParentTAZElements().back() == toTAZ)) {
            return true;
        }
    }
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
            undoList->changeAttribute(new GNEChange_Attribute(this, key, value));
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


bool
GNEDataInterval::isAttributeComputed(SumoXMLAttr /*key*/) const {
    return false;
}


std::string
GNEDataInterval::getPopUpID() const {
    return getTagStr();
}


std::string
GNEDataInterval::getHierarchyName() const {
    return "interval: " + getAttribute(SUMO_ATTR_BEGIN) + " -> " + getAttribute(SUMO_ATTR_END);
}


const std::map<std::string, std::string>&
GNEDataInterval::getACParametersMap() const {
    return getParametersMap();
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
GNEDataInterval::toogleAttribute(SumoXMLAttr /*key*/, const bool /*value*/) {
    throw InvalidArgument("Nothing to enable");
}

/****************************************************************************/
