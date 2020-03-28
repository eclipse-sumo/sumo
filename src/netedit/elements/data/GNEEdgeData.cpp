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
/// @file    GNEEdgeData.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// class for edge data
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/frames/data/GNEEdgeDataFrame.h>

#include "GNEEdgeData.h"
#include "GNEDataInterval.h"
#include "GNEDataSet.h"


// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEEdgeData - methods
// ---------------------------------------------------------------------------

GNEEdgeData::GNEEdgeData(GNEDataInterval* dataIntervalParent, GNEEdge* edgeParent, const std::map<std::string, std::string>& parameters) :
    GNEGenericData(SUMO_TAG_MEANDATA_EDGE, GLO_EDGEDATA, dataIntervalParent, parameters, 
        {edgeParent}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}) {
}


GNEEdgeData::~GNEEdgeData() {}


void
GNEEdgeData::updateGeometry() {
    // nothing to update
}


void
GNEEdgeData::updateDottedContour() {
    // nothing to update
}


Position
GNEEdgeData::getPositionInView() const {
    return getParentEdges().front()->getPositionInView();
}


void
GNEEdgeData::writeGenericData(OutputDevice& device) const {
    // open device (don't use SUMO_TAG_MEANDATA_EDGE)
    device.openTag(SUMO_TAG_EDGE);
    // write edge ID
    device.writeAttr(SUMO_ATTR_ID, getParentEdges().front()->getID());
    // iterate over attributes
    for (const auto& attribute : getParametersMap()) {
        // write attribute (don't use writeParams)
        device.writeAttr(attribute.first, attribute.second);
    }
    // close device
    device.closeTag();
}


bool
GNEEdgeData::isGenericDataValid() const {
    return true;
}


std::string
GNEEdgeData::getGenericDataProblem() const {
    return "";
}


void
GNEEdgeData::fixGenericDataProblem() {
    throw InvalidArgument(getTagStr() + " cannot fix any problem");
}


Boundary 
GNEEdgeData::getCenteringBoundary() const {
    return getParentEdges().front()->getCenteringBoundary();
}


std::string
GNEEdgeData::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getParentEdges().front()->getID();
        case GNE_ATTR_DATASET:
            return myDataIntervalParent->getDataSetParent()->getID();
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARAMETERS:
            return getParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNEEdgeData::getAttributeDouble(SumoXMLAttr key) const {
    throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
}


void
GNEEdgeData::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
            undoList->p_add(new GNEChange_Attribute(this, getViewNet()->getNet(), key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEEdgeData::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return Parameterised::areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEEdgeData::enableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    // Nothing to enable
}


void
GNEEdgeData::disableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    // Nothing to disable enable
}


bool GNEEdgeData::isAttributeEnabled(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return false;
        default:
            return true;
    }
}


std::string
GNEEdgeData::getPopUpID() const {
    return getTagStr();
}


std::string
GNEEdgeData::getHierarchyName() const {
    return getTagStr() + ": " + getParentEdges().front()->getID();
}


void
GNEEdgeData::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case GNE_ATTR_SELECTED:
            if (parse<bool>(value)) {
                selectAttributeCarrier();
            } else {
                unselectAttributeCarrier();
            }
            break;
        case GNE_ATTR_PARAMETERS:
            setParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEEdgeData::setEnabledAttribute(const int /*enabledAttributes*/) {
    throw InvalidArgument("Nothing to enable");
}


bool 
GNEEdgeData::isVisible() const {
    // obtain pointer to edge data frame (only for code legibly)
    const GNEEdgeDataFrame* edgeDataFrame = myDataIntervalParent->getViewNet()->getViewParent()->getEdgeDataFrame();
    // check if we have to filter generic data
    if (edgeDataFrame->shown()) {
        // check interval
        if ((edgeDataFrame->getIntervalSelector()->getDataInterval() != nullptr) &&
            (edgeDataFrame->getIntervalSelector()->getDataInterval() != myDataIntervalParent)) {
            return false;
        }
        // check attribute
        if ((edgeDataFrame->getAttributeSelector()->getFilteredAttribute().size() > 0) &&
            (getParametersMap().count(edgeDataFrame->getAttributeSelector()->getFilteredAttribute()) == 0)) {
            return false;
        }
        // all checks ok, then return true
        return true;
    }
    else {
        // GNEEdgeDataFrame hidden, then return false
        return false;
    }
}


const RGBColor&
GNEEdgeData::getSpecificColor() const {
    return RGBColor::RED;
}

/****************************************************************************/
