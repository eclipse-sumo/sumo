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
/// @file    GNEEdgeRelationData.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// class for edge relation data
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
#include <netedit/frames/data/GNEEdgeRelationDataFrame.h>

#include "GNEEdgeRelationData.h"
#include "GNEDataInterval.h"
#include "GNEDataSet.h"


// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEEdgeRelationData - methods
// ---------------------------------------------------------------------------

GNEEdgeRelationData::GNEEdgeRelationData(GNEDataInterval* dataIntervalParent, GNEEdge* edgeParent, const std::map<std::string, std::string>& parameters) :
    GNEGenericData(SUMO_TAG_EDGEREL, GLO_EDGERELATIONDATA, dataIntervalParent, parameters,
        {edgeParent}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}) {
}


GNEEdgeRelationData::~GNEEdgeRelationData() {}


void
GNEEdgeRelationData::updateGeometry() {
    // nothing to update
}


void
GNEEdgeRelationData::updateDottedContour() {
    // nothing to update
}


Position
GNEEdgeRelationData::getPositionInView() const {
    return getParentEdges().front()->getPositionInView();
}


void
GNEEdgeRelationData::writeGenericData(OutputDevice& device) const {
    // open device (don't use SUMO_TAG_EDGEREL)
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
GNEEdgeRelationData::isGenericDataValid() const {
    return true;
}


std::string
GNEEdgeRelationData::getGenericDataProblem() const {
    return "";
}


void
GNEEdgeRelationData::fixGenericDataProblem() {
    throw InvalidArgument(getTagStr() + " cannot fix any problem");
}


Boundary 
GNEEdgeRelationData::getCenteringBoundary() const {
    return getParentEdges().front()->getCenteringBoundary();
}


std::string
GNEEdgeRelationData::getAttribute(SumoXMLAttr key) const {
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
GNEEdgeRelationData::getAttributeDouble(SumoXMLAttr key) const {
    throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
}


void
GNEEdgeRelationData::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
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
GNEEdgeRelationData::isValid(SumoXMLAttr key, const std::string& value) {
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
GNEEdgeRelationData::enableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    // Nothing to enable
}


void
GNEEdgeRelationData::disableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    // Nothing to disable enable
}


bool GNEEdgeRelationData::isAttributeEnabled(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return false;
        default:
            return true;
    }
}


std::string
GNEEdgeRelationData::getPopUpID() const {
    return getTagStr();
}


std::string
GNEEdgeRelationData::getHierarchyName() const {
    return getTagStr() + ": " + getParentEdges().front()->getID();
}


void
GNEEdgeRelationData::setAttribute(SumoXMLAttr key, const std::string& value) {
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
GNEEdgeRelationData::setEnabledAttribute(const int /*enabledAttributes*/) {
    throw InvalidArgument("Nothing to enable");
}


const RGBColor&
GNEEdgeRelationData::getSpecificColor() const {
    return RGBColor::GREEN;
}

/****************************************************************************/
