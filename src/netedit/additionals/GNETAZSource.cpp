/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNETAZSource.cpp
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

#include "GNETAZSource.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNETAZSource::GNETAZSource(GNEAdditional* TAZParent, GNEEdge* edge, double departWeight) :
    GNEAdditional(TAZParent, TAZParent->getViewNet(), GLO_TAZ, SUMO_TAG_TAZSOURCE, "", false),
    myEdge(edge),
    myDepartWeight(departWeight) {
    // set edge as child
    addEdgeChild(edge);
}


GNETAZSource::~GNETAZSource() {}


void
GNETAZSource::moveGeometry(const Position&) {
    // This additional cannot be moved
}


void
GNETAZSource::commitGeometryMoving(GNEUndoList*) {
    // This additional cannot be moved
}


void
GNETAZSource::updateGeometry(bool /*updateGrid*/) {
    // Currently this additional doesn't own a Geometry
}


Position
GNETAZSource::getPositionInView() const {
    return myFirstAdditionalParent->getPositionInView();
}


std::string
GNETAZSource::getParentName() const {
    return myFirstAdditionalParent->getID();
}


void
GNETAZSource::drawGL(const GUIVisualizationSettings&) const {
    // Currently This additional isn't drawn
}


std::string
GNETAZSource::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_EDGE:
            return myEdge->getID();
        case SUMO_ATTR_WEIGHT:
            return toString(myDepartWeight);
        case GNE_ATTR_PARENT:
            return myFirstAdditionalParent->getID();
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
        case GNE_ATTR_TAZCOLOR: {
            // obtain max and min weight source
            double maxWeightSource = parse<double>(myFirstAdditionalParent->getAttribute(GNE_ATTR_MAX_SOURCE));
            double minWeightSource = parse<double>(myFirstAdditionalParent->getAttribute(GNE_ATTR_MIN_SOURCE));
            // avoid division between zero
            if ((maxWeightSource - minWeightSource) == 0) {
                return "0";
            } else {
                // calculate percentage relative to the max and min weight
                double percentage = (myDepartWeight - minWeightSource) / (maxWeightSource - minWeightSource);
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
GNETAZSource::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
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
GNETAZSource::isValid(SumoXMLAttr key, const std::string& value) {
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
GNETAZSource::getPopUpID() const {
    return getTagStr();
}


std::string
GNETAZSource::getHierarchyName() const {
    return getTagStr() + ": " + getAttribute(SUMO_ATTR_WEIGHT);
}

// ===========================================================================
// private
// ===========================================================================

void
GNETAZSource::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            changeAdditionalID(value);
            break;
        case SUMO_ATTR_WEIGHT:
            myDepartWeight = parse<double>(value);
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
