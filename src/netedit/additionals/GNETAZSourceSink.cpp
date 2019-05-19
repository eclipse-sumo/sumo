/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNETAZSourceSink.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Apr 2017
/// @version $Id$
///
//
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/netelements/GNEEdge.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>

#include "GNETAZSourceSink.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNETAZSourceSink::GNETAZSourceSink(SumoXMLTag sourceSinkTag, GNEAdditional* TAZParent, GNEEdge* edge, double departWeight) :
    GNEAdditional(TAZParent, TAZParent->getViewNet(), GLO_TAZ, sourceSinkTag, "", false, {
    edge
}, {}, {}, {TAZParent}, {}, {}, {}, {}, {}, {}),
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
    // Currently this additional doesn't own a Geometry
}


Position
GNETAZSourceSink::getPositionInView() const {
    return getAdditionalParents().at(0)->getPositionInView();
}


Boundary
GNETAZSourceSink::getCenteringBoundary() const {
    return getEdgeParents().front()->getCenteringBoundary();
}


std::string
GNETAZSourceSink::getParentName() const {
    return getAdditionalParents().at(0)->getID();
}


void
GNETAZSourceSink::drawGL(const GUIVisualizationSettings&) const {
    // Currently This additional isn't drawn
}


std::string
GNETAZSourceSink::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_EDGE:
            return getEdgeParents().front()->getID();
        case SUMO_ATTR_WEIGHT:
            return toString(myDepartWeight);
        case GNE_ATTR_PARENT:
            return getAdditionalParents().at(0)->getID();
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
        case GNE_ATTR_TAZCOLOR: {
            // obtain max and min weight source
            double maxWeightSource = parse<double>(getAdditionalParents().at(0)->getAttribute(GNE_ATTR_MAX_SOURCE));
            double minWeightSource = parse<double>(getAdditionalParents().at(0)->getAttribute(GNE_ATTR_MIN_SOURCE));
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
            case GNE_ATTR_GENERIC:
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
        case GNE_ATTR_GENERIC:
            return isGenericParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
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
            changeAdditionalID(value);
            break;
        case SUMO_ATTR_WEIGHT:
            myDepartWeight = parse<double>(value);
            // update statictis of TAZ parent
            getAdditionalParents().at(0)->updateAdditionalParent();
            break;
        case GNE_ATTR_GENERIC:
            setGenericParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


/****************************************************************************/
