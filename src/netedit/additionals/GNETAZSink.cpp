/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNETAZSink.cpp
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

#include "GNETAZSink.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNETAZSink::GNETAZSink(GNEAdditional* TAZParent, GNEEdge* edge, double arrivalWeight) :
    GNEAdditional(TAZParent, TAZParent->getViewNet(), GLO_TAZ, SUMO_TAG_TAZSINK, "", false),
    myEdge(edge),
    myArrivalWeight(arrivalWeight) {
    // set edge as child
    addEdgeChild(edge);
}


GNETAZSink::~GNETAZSink() {}


void
GNETAZSink::moveGeometry(const Position&) {
    // This additional cannot be moved
}


void
GNETAZSink::commitGeometryMoving(GNEUndoList*) {
    // This additional cannot be moved
}


void
GNETAZSink::updateGeometry(bool /*updateGrid*/) {
    // Currently this additional doesn't own a Geometry
}


Position
GNETAZSink::getPositionInView() const {
    return myFirstAdditionalParent->getPositionInView();
}


std::string
GNETAZSink::getParentName() const {
    return myFirstAdditionalParent->getID();
}


void
GNETAZSink::drawGL(const GUIVisualizationSettings&) const {
    // Currently This additional isn't drawn
}


std::string
GNETAZSink::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_EDGE:
            return myEdge->getID();
        case SUMO_ATTR_WEIGHT:
            return toString(myArrivalWeight);
        case GNE_ATTR_PARENT:
            return myFirstAdditionalParent->getID();
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
        case GNE_ATTR_TAZCOLOR: {
            // obtain max and min weight source
            double maxWeightSink = parse<double>(myFirstAdditionalParent->getAttribute(GNE_ATTR_MAX_SINK));
            double minWeightSink = parse<double>(myFirstAdditionalParent->getAttribute(GNE_ATTR_MIN_SINK));
            // avoid division between zero
            if ((maxWeightSink - minWeightSink) == 0) {
                return "0";
            } else {
                // calculate percentage relative to the max and min weight
                double percentage = (myArrivalWeight - minWeightSink) / (maxWeightSink - minWeightSink);
                // convert percentage to a value between [0-9] (because we have only 10 colors)
                if(percentage >= 1) {
                    return "9";
                } else if(percentage < 0) {
                    return "0";
                } else {
                    return toString((int)(percentage*10));
                }
            }
        }
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNETAZSink::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    // this additional is the only that can edit a variable directly, see GNEAdditionalHandler::buildTAZEdge(...)
    if(undoList == nullptr) {
        setAttribute(key, value);
    } else {
        if (value == getAttribute(key)) {
            return; //avoid needless changes, later logic relies on the fact that attributes have changed
        }
        switch (key) {
            case SUMO_ATTR_ID:
            case SUMO_ATTR_WEIGHT:
            case GNE_ATTR_GENERIC:
                undoList->p_add(new GNEChange_Attribute(this, key, value));
                break;
            default:
                throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
        }
    }
}


bool
GNETAZSink::isValid(SumoXMLAttr key, const std::string& value) {
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
GNETAZSink::getPopUpID() const {
    return getTagStr();
}


std::string
GNETAZSink::getHierarchyName() const {
    return getTagStr() + ": " + getAttribute(SUMO_ATTR_WEIGHT);
}

// ===========================================================================
// private
// ===========================================================================

void
GNETAZSink::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            changeAdditionalID(value);
            break;
        case SUMO_ATTR_WEIGHT:
            myArrivalWeight = parse<double>(value);
            // update statictis of TAZ parent
            myFirstAdditionalParent->updateAdditionalParent();
            break;
        case GNE_ATTR_GENERIC:
            setGenericParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


/****************************************************************************/
