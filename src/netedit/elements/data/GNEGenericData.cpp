/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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

GNEGenericData::GNEGenericData(const SumoXMLTag tag, GNEDataInterval* dataIntervalParent,
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
    Parameterised(parameters),
    GNEHierarchicalParentElements(this, edgeParents, laneParents, shapeParents, additionalParents, demandElementParents, genericDataParents),
    GNEHierarchicalChildElements(this, edgeChildren, laneChildren, shapeChildren, additionalChildren, demandElementChildren, genericDataChildren),
    myDataIntervalParent(dataIntervalParent) {
}


GNEGenericData::~GNEGenericData() {}


GNEDataInterval*
GNEGenericData::getDataIntervalParent() const {
    return myDataIntervalParent;
}


std::string 
GNEGenericData::generateChildID(SumoXMLTag /*childTag*/) {
    return "";
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
                // check if we can 
                if (edgeDataFrame->getDataSetSelector()->getDataSet() && 
                    (edgeDataFrame->getDataSetSelector()->getDataSet() != myDataIntervalParent->getDataSetParent())) {
                    return false;
                } else if (edgeDataFrame->getIntervalSelector()->getDataInterval() &&
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

/****************************************************************************/
