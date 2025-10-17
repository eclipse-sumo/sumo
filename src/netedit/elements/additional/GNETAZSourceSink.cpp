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
/// @file    GNETAZSourceSink.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Apr 2017
///
//
/****************************************************************************/

#include <netedit/GNETagProperties.h>
#include <netedit/changes/GNEChange_Attribute.h>

#include "GNETAZSourceSink.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNETAZSourceSink::GNETAZSourceSink(SumoXMLTag sourceSinkTag, GNENet* net) :
    GNEAttributeCarrier(sourceSinkTag, net, "", true) {
}


GNETAZSourceSink::GNETAZSourceSink(SumoXMLTag sourceSinkTag, GNEAdditional* TAZParent, GNEEdge* edge, const double departWeight) :
    GNEAttributeCarrier(sourceSinkTag, TAZParent->getNet(), TAZParent->getFilename(), false),
    myWeight(departWeight) {
    // set parents
    setParent<GNEEdge*>(edge);
    setParent<GNEAdditional*>(TAZParent);
    //check that this is a TAZ Source OR a TAZ Sink
    if ((sourceSinkTag != SUMO_TAG_TAZSOURCE) && (sourceSinkTag != SUMO_TAG_TAZSINK)) {
        throw InvalidArgument("Invalid TAZ Child Tag");
    }
}


GNETAZSourceSink::~GNETAZSourceSink() {}


GNEMoveElement*
GNETAZSourceSink::getMoveElement() const {
    return nullptr;
}


Parameterised*
GNETAZSourceSink::getParameters() {
    return nullptr;
}


const Parameterised*
GNETAZSourceSink::getParameters() const {
    return nullptr;
}


GNEHierarchicalElement*
GNETAZSourceSink::getHierarchicalElement() {
    return this;
}


void
GNETAZSourceSink::writeTAZSourceSink(OutputDevice& device) const {
    // open source/sink tag
    device.openTag(myTagProperty->getTag());
    // write source/sink attributes
    device.writeAttr(SUMO_ATTR_ID, getParentEdges().front()->getID());
    device.writeAttr(SUMO_ATTR_WEIGHT, myWeight);
    // close tag
    device.closeTag();
}


double
GNETAZSourceSink::getWeight() const {
    return myWeight;
}


GUIGlObject*
GNETAZSourceSink::getGUIGlObject() {
    return nullptr;
}


const GUIGlObject*
GNETAZSourceSink::getGUIGlObject() const {
    return nullptr;
}


void
GNETAZSourceSink::updateGeometry() {
    // nothing to update
}


bool
GNETAZSourceSink::checkDrawFromContour() const {
    return false;
}


bool
GNETAZSourceSink::checkDrawToContour() const {
    return false;
}


bool
GNETAZSourceSink::checkDrawRelatedContour() const {
    return false;
}


bool
GNETAZSourceSink::checkDrawOverContour() const {
    return false;
}


bool
GNETAZSourceSink::checkDrawDeleteContour() const {
    return false;
}


bool
GNETAZSourceSink::checkDrawDeleteContourSmall() const {
    return false;
}


bool
GNETAZSourceSink::checkDrawSelectContour() const {
    return false;
}


bool
GNETAZSourceSink::checkDrawMoveContour() const {
    return false;
}


std::string
GNETAZSourceSink::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getParentAdditionals().front()->getID();
        case SUMO_ATTR_EDGE:
            return getParentEdges().front()->getID();
        case SUMO_ATTR_WEIGHT:
            return toString(myWeight);
        case GNE_ATTR_PARENT:
            return getParentAdditionals().at(0)->getID();
        case GNE_ATTR_TAZCOLOR: {
            // obtain max and min weight source
            double maxWeightSource = getParentAdditionals().at(0)->getAttributeDouble(GNE_ATTR_MAX_SOURCE);
            double minWeightSource = getParentAdditionals().at(0)->getAttributeDouble(GNE_ATTR_MIN_SOURCE);
            // avoid division between zero
            if ((maxWeightSource - minWeightSource) == 0) {
                return "0";
            } else {
                // calculate percentage relative to the max and min weight
                double percentage = (myWeight - minWeightSource) / (maxWeightSource - minWeightSource);
                // convert percentage to a value between [0-9] (because we have only 10 colors)
                if (percentage >= 1) {
                    return "9";
                } else if (percentage < 0) {
                    return "0";
                } else {
                    return toString((int)(percentage * 10));
                }
            }
        }
        default:
            return getCommonAttribute(key);
    }
}

double
GNETAZSourceSink::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_WEIGHT:
            return myWeight;
        default:
            return getCommonAttributeDouble(key);
    }
}


Position
GNETAZSourceSink::getAttributePosition(SumoXMLAttr key) const {
    return getCommonAttributePosition(key);
}


PositionVector
GNETAZSourceSink::getAttributePositionVector(SumoXMLAttr key) const {
    return getCommonAttributePositionVector(key);
}


void
GNETAZSourceSink::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    // this TAZElement is the only that can edit a variable directly, see GNEAdditionalHandler::buildTAZEdge(...)
    if (undoList == nullptr) {
        setAttribute(key, value);
    } else {
        if (value == getAttribute(key)) {
            return; //avoid needless changes, later logic relies on the fact that attributes have changed
        }
        switch (key) {
            case SUMO_ATTR_ID:
            case SUMO_ATTR_WEIGHT:
                GNEChange_Attribute::changeAttribute(this, key, value, undoList);
                break;
            default:
                setCommonAttribute(key, value, undoList);
                break;
        }
    }
}


bool
GNETAZSourceSink::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return false;
        case SUMO_ATTR_WEIGHT:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        default:
            return isCommonAttributeValid(key, value);
    }
}



bool
GNETAZSourceSink::isAttributeEnabled(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_EDGE:
            return false;
        default:
            return true;
    }
}


std::string
GNETAZSourceSink::getPopUpID() const {
    return getTagStr();
}


std::string
GNETAZSourceSink::getHierarchyName() const {
    return getTagStr() + ": " + getAttribute(SUMO_ATTR_WEIGHT);
}

// ===========================================================================
// private
// ===========================================================================

void
GNETAZSourceSink::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument(getTagStr() + " cannot edit '" + toString(key) + "'");
        case SUMO_ATTR_WEIGHT:
            myWeight = parse<double>(value);
            break;
        default:
            setCommonAttribute(key, value);
            break;
    }
}

/****************************************************************************/
