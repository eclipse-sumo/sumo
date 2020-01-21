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
// A abstract class for data sets
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

GNEDataSet::GNEDataSet(GNEViewNet* viewNet) :
    GNEAttributeCarrier(SUMO_TAG_NOTHING),
    myViewNet(viewNet) {
}


GNEDataSet::~GNEDataSet() {}


GNEViewNet*
GNEDataSet::getViewNet() const {
    return myViewNet;
}


void 
GNEDataSet::selectAttributeCarrier(bool /*changeFlag*/) {
    //
}


void 
GNEDataSet::unselectAttributeCarrier(bool /*changeFlag*/) {
    //
}


bool
GNEDataSet::isAttributeCarrierSelected() const {
    return false;
}


bool
GNEDataSet::drawUsingSelectColor() const {
    return false;
}


std::string 
GNEDataSet::getAttribute(SumoXMLAttr /*key*/) const {
    return "";
}


double 
GNEDataSet::getAttributeDouble(SumoXMLAttr /*key*/) const {
    return 0;
}


void 
GNEDataSet::setAttribute(SumoXMLAttr /*key*/, const std::string& /*value*/, GNEUndoList* /*undoList*/) {
    //
}


bool 
GNEDataSet::isValid(SumoXMLAttr /*key*/, const std::string& /*value*/) {
    return false;
}


void 
GNEDataSet::enableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    //
}


void 
GNEDataSet::disableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    //
}


bool 
GNEDataSet::isAttributeEnabled(SumoXMLAttr /*key*/) const {
    return false;
}


std::string 
GNEDataSet::getPopUpID() const {
    return "";
}


std::string 
GNEDataSet::getHierarchyName() const {
    return "";
}


/****************************************************************************/
