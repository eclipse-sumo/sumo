/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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

#include <netedit/GNEViewNet.h>

#include "GNEEdgeData.h"


// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEEdgeData - methods
// ---------------------------------------------------------------------------

GNEEdgeData::GNEEdgeData(GNEDataInterval* dataIntervalParent) :
    GNEGenericData(SUMO_TAG_MEANDATA_EDGE, dataIntervalParent,
        {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}) {
}


GNEEdgeData::~GNEEdgeData() {}


void 
GNEEdgeData::writeEdgeData(OutputDevice& device) const {
    //
}


bool
GNEEdgeData::isEdgeDataValid() const {
    return true;
}


std::string
GNEEdgeData::getEdgeDataProblem() const {
    return "";
}


void
GNEEdgeData::fixEdgeDataProblem() {
    throw InvalidArgument(getTagStr() + " cannot fix any problem");
}


void 
GNEEdgeData::selectAttributeCarrier(bool changeFlag) {
    //
}


void
GNEEdgeData::unselectAttributeCarrier(bool changeFlag) {
    //
}


bool
GNEEdgeData::isAttributeCarrierSelected() const {
    return mySelected;
}


bool
GNEEdgeData::drawUsingSelectColor() const {
    if (mySelected && (myViewNet->getEditModes().currentSupermode == GNE_SUPERMODE_DEMAND)) {
        return true;
    } else {
        return false;
    }
}


std::string 
GNEEdgeData::getAttribute(SumoXMLAttr key) const {
    return "";
}


double 
GNEEdgeData::getAttributeDouble(SumoXMLAttr key) const {
    return 0;
}


void 
GNEEdgeData::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    //
}


bool 
GNEEdgeData::isValid(SumoXMLAttr key, const std::string& value) {
    return false;
}


void 
GNEEdgeData::enableAttribute(SumoXMLAttr key, GNEUndoList* undoList) {
    //
}


void 
GNEEdgeData::disableAttribute(SumoXMLAttr key, GNEUndoList* undoList) {
    //
}


bool GNEEdgeData::isAttributeEnabled(SumoXMLAttr key) const {
    return false;
}


std::string 
GNEEdgeData::getPopUpID() const {
    return "";
}


std::string 
GNEEdgeData::getHierarchyName() const {
    return "";
}


void 
GNEEdgeData::setAttribute(SumoXMLAttr key, const std::string& value) {
}

/****************************************************************************/
