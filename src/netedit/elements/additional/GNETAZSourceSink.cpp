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
/// @file    GNETAZSourceSink.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Apr 2017
///
//
/****************************************************************************/
#include <config.h>

#include <netedit/elements/network/GNEEdge.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>

#include "GNETAZSourceSink.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNETAZSourceSink::GNETAZSourceSink(SumoXMLTag sourceSinkTag, GNEAdditional* TAZParent, GNEEdge* edge, double departWeight) :
    GNEAdditional(TAZParent, TAZParent->getViewNet(), GLO_TAZ, sourceSinkTag, "", false, 
        {edge}, {}, {}, {TAZParent}, {}, {}, {}, {}, {}, {}, {}, {}),
    myDepartWeight(departWeight) {
    //check that this is a TAZ Source OR a TAZ Sink
    if ((sourceSinkTag != SUMO_TAG_TAZSOURCE) && (sourceSinkTag != SUMO_TAG_TAZSINK)) {
        throw InvalidArgument("Invalid TAZ Child Tag");
    }
}


GNETAZSourceSink::~GNETAZSourceSink() {}


double
GNETAZSourceSink::getDepartWeight() const {
    return myDepartWeight;
}


void
GNETAZSourceSink::moveGeometry(const Position&) {
    // This additional cannot be moved
}


void
GNETAZSourceSink::commitGeometryMoving(GNEUndoList*) {
    // This additional cannot be moved
}


void
GNETAZSourceSink::updateGeometry() {
    // This additional doesn't own a geometry
}

void
GNETAZSourceSink::updateDottedContour() {
    // This additional doesn't own a dotted contour
}


Position
GNETAZSourceSink::getPositionInView() const {
    return getParentAdditionals().at(0)->getPositionInView();
}


Boundary
GNETAZSourceSink::getCenteringBoundary() const {
    return getParentEdges().front()->getCenteringBoundary();
}


void
GNETAZSourceSink::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
}


std::string
GNETAZSourceSink::getParentName() const {
    return getParentAdditionals().at(0)->getID();
}


void
GNETAZSourceSink::drawGL(const GUIVisualizationSettings&) const {
    // Currently This additional isn't drawn
}


std::string
GNETAZSourceSink::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getID();
        case SUMO_ATTR_EDGE:
            return getParentEdges().front()->getID();
        case SUMO_ATTR_WEIGHT:
            return toString(myDepartWeight);
        case GNE_ATTR_PARENT:
            return getParentAdditionals().at(0)->getID();
        case GNE_ATTR_PARAMETERS:
            return getParametersStr();
        case GNE_ATTR_TAZCOLOR: {
            // obtain max and min weight source
            double maxWeightSource = getParentAdditionals().at(0)->getAttributeDouble(GNE_ATTR_MAX_SOURCE);
            double minWeightSource = getParentAdditionals().at(0)->getAttributeDouble(GNE_ATTR_MIN_SOURCE);
            // avoid division between zero
            if ((maxWeightSource - minWeightSource) == 0) {
                return "0";
            } else {
                // calculate percentage relative to the max and min weight
                double percentage = (myDepartWeight - minWeightSource) / (maxWeightSource - minWeightSource);
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
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

double
GNETAZSourceSink::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_WEIGHT:
            return myDepartWeight;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
    }
}


void
GNETAZSourceSink::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    // this additional is the only that can edit a variable directly, see GNEAdditionalHandler::buildTAZEdge(...)
    if (undoList == nullptr) {
        setAttribute(key, value);
    } else {
        if (value == getAttribute(key)) {
            return; //avoid needless changes, later logic relies on the fact that attributes have changed
        }
        switch (key) {
            case SUMO_ATTR_ID:
            case SUMO_ATTR_WEIGHT:
            case GNE_ATTR_PARAMETERS:
                undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), key, value));
                break;
            default:
                throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
        }
    }
}


bool
GNETAZSourceSink::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_WEIGHT:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case GNE_ATTR_PARAMETERS:
            return Parameterised::areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
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
            myViewNet->getNet()->getAttributeCarriers()->updateID(this, value);
            break;
        case SUMO_ATTR_WEIGHT:
            myDepartWeight = parse<double>(value);
            // update statictis of TAZ parent
            getParentAdditionals().at(0)->updateParentAdditional();
            break;
        case GNE_ATTR_PARAMETERS:
            setParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


/****************************************************************************/
