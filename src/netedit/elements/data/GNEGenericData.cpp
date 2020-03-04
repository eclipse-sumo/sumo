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
/// @file    GNEGenericData.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// A abstract class for data sets
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/frames/data/GNEEdgeDataFrame.h>

#include "GNEGenericData.h"
#include "GNEDataInterval.h"


// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEGenericData - methods
// ---------------------------------------------------------------------------

GNEGenericData::GNEGenericData(const SumoXMLTag tag, const GUIGlObjectType GLType, GNEDataInterval* dataIntervalParent,
        const std::map<std::string, std::string>& parameters,
        const std::vector<GNEEdge*>& edgeParents,
        const std::vector<GNELane*>& laneParents,
        const std::vector<GNEShape*>& shapeParents,
        const std::vector<GNEAdditional*>& additionalParents,
        const std::vector<GNEDemandElement*>& demandElementParents,
        const std::vector<GNEGenericData*>& genericDataParents,
        const std::vector<GNEEdge*>& edgeChildren,
        const std::vector<GNELane*>& laneChildren,
        const std::vector<GNEShape*>& shapeChildren,
        const std::vector<GNEAdditional*>& additionalChildren,
        const std::vector<GNEDemandElement*>& demandElementChildren,
        const std::vector<GNEGenericData*>& genericDataChildren) :
    GNEAttributeCarrier(tag),
    Parameterised(Parameterised::ATTRTYPE_DOUBLE, parameters),
    GNEHierarchicalParentElements(this, edgeParents, laneParents, shapeParents, additionalParents, demandElementParents, genericDataParents),
    GNEHierarchicalChildElements(this, edgeChildren, laneChildren, shapeChildren, additionalChildren, demandElementChildren, genericDataChildren),
    myDataIntervalParent(dataIntervalParent),
    myGLType(GLType) {
}


GNEGenericData::~GNEGenericData() {}


GNEDataInterval*
GNEGenericData::getDataIntervalParent() const {
    return myDataIntervalParent;
}


const GUIGlObjectType 
GNEGenericData::getGLType() const {
    return myGLType;
}


bool
GNEGenericData::isVisible() const {
    // first check if we're in supermode demand
    if (myDataIntervalParent->getViewNet()->getEditModes().currentSupermode == GNE_SUPERMODE_DATA) {
        // special case for edgeDatas
        if (myTagProperty.getTag() == SUMO_TAG_MEANDATA_EDGE) {
            // obtain pointer to edge data frame (only for code legibly)
            const GNEEdgeDataFrame* edgeDataFrame = myDataIntervalParent->getViewNet()->getViewParent()->getEdgeDataFrame();
            // check if we have to filter generic data
            if (edgeDataFrame->shown()) {
                // check interval
                if (edgeDataFrame->getIntervalSelector()->getDataInterval() &&
                        (edgeDataFrame->getIntervalSelector()->getDataInterval() != myDataIntervalParent)) {
                    return false;
                } else {
                    return true;
                }
            } else {
                return true;
            }
        } else {
            return true;
        }
    } else {
        return false;
    }
}


bool
GNEGenericData::isGenericDataValid() const {
    return true;
}


std::string
GNEGenericData::getGenericDataProblem() const {
    return "";
}


void
GNEGenericData::fixGenericDataProblem() {
    throw InvalidArgument(getTagStr() + " cannot fix any problem");
}


GNEViewNet*
GNEGenericData::getViewNet() const {
    return myDataIntervalParent->getViewNet();
}


bool
GNEGenericData::isAttributeCarrierSelected() const {
    return mySelected;
}


bool
GNEGenericData::drawUsingSelectColor() const {
    if (mySelected && (myDataIntervalParent->getViewNet()->getEditModes().currentSupermode == GNE_SUPERMODE_DEMAND)) {
        return true;
    } else {
        return false;
    }
}


std::string
GNEGenericData::generateChildID(SumoXMLTag /*childTag*/) {
    return "";
}

/****************************************************************************/
