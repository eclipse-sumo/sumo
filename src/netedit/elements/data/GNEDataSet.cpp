/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDataSet.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// A abstract class for data elements
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/GNEViewNet.h>

#include "GNEDataSet.h"


// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEDataSet - methods
// ---------------------------------------------------------------------------

GNEDataSet::GNEDataSet(const std::string& id, GNEViewNet* viewNet) :
    GNEAttributeCarrier(SUMO_TAG_NOTHING),
    myViewNet(viewNet) {
}


GNEDataSet::GNEDataSet(GNEDataSet* dataElementParent, GNEViewNet* viewNet) :
    GNEAttributeCarrier(SUMO_TAG_NOTHING),
    myViewNet(viewNet) {
}


GNEDataSet::~GNEDataSet() {}


bool
GNEDataSet::isDataSetValid() const {
    return true;
}


std::string
GNEDataSet::getDataSetProblem() const {
    return "";
}


void
GNEDataSet::fixDataSetProblem() {
    throw InvalidArgument(getTagStr() + " cannot fix any problem");
}


GNEViewNet*
GNEDataSet::getViewNet() const {
    return myViewNet;
}


bool
GNEDataSet::isAttributeCarrierSelected() const {
    return mySelected;
}


bool
GNEDataSet::drawUsingSelectColor() const {
    if (mySelected && (myViewNet->getEditModes().currentSupermode == GNE_SUPERMODE_DEMAND)) {
        return true;
    } else {
        return false;
    }
}

/****************************************************************************/
