/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
/// @file    GNEMeanData.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2022
///
// Class for representing MeanData
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/elements/data/GNEMeanData.h>
#include <netedit/frames/common/GNESelectorFrame.h>
#include <netedit/frames/data/GNEEdgeDataFrame.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEMeanData.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEMeanData::GNEMeanData(GNENet *net, GNEEdge* edge, const std::string &file) :
    GUIGlObject(GLO_MEANDATAEDGE, edge->getID(), nullptr),
    GNEHierarchicalElement(net, SUMO_TAG_MEANDATA_EDGE, {}, {edge}, {}, {}, {}, {}),
    myFile(file) {
}


GNEMeanData::GNEMeanData(GNENet *net, GNELane* lane, const std::string &file) :
    GUIGlObject(GLO_MEANDATALANE, lane->getID(), nullptr),
    GNEHierarchicalElement(net, SUMO_TAG_MEANDATA_LANE, {}, {}, {lane}, {}, {}, {}),
    myFile(file) {
}


GNEMeanData::~GNEMeanData() {}


GUIGlObject*
GNEMeanData::getGUIGlObject() {
    return this;
}


void
GNEMeanData::updateGeometry() {
    // nothing to update
}


Position 
GNEMeanData::getPositionInView() const {
    if (getParentLanes().size() > 0) {
        return getParentLanes().front()->getPositionInView();
    } else {
        return getParentEdges().front()->getPositionInView();
    }
}


GUIGLObjectPopupMenu*
GNEMeanData::getPopUpMenu(GUIMainWindow& /*app*/, GUISUMOAbstractView& /*parent*/) {
    return nullptr;
}


GUIParameterTableWindow*
GNEMeanData::getParameterWindow(GUIMainWindow& /*app*/, GUISUMOAbstractView& /*parent*/) {
    return nullptr;
}


void
GNEMeanData::drawGL(const GUIVisualizationSettings& /*s*/) const {
    // nothing to draw
}


void
GNEMeanData::deleteGLObject() {
    // nothing to delete
}


void
GNEMeanData::selectGLObject() {
    // nothing to select
}


void
GNEMeanData::updateGLObject() {
    // nothing to do
}


Boundary
GNEMeanData::getCenteringBoundary() const {
    return getParentEdges().front()->getCenteringBoundary();
}


std::string
GNEMeanData::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            if (getParentLanes().size() > 0) {
                return getParentLanes().front()->getID();
            } else {
                return getParentEdges().front()->getID();
            }
        case SUMO_ATTR_FILE:
            return myFile;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNEMeanData::getAttributeDouble(SumoXMLAttr key) const {
    throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
}


bool
GNEMeanData::isAttributeEnabled(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return false;
        default:
            return true;
    }
}


void
GNEMeanData::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_FILE:
            undoList->changeAttribute(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEMeanData::isValid(SumoXMLAttr key , const std::string& value) {
    switch (key) {
        case SUMO_ATTR_FILE:
            return SUMOXMLDefinitions::isValidFilename(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


std::string
GNEMeanData::getPopUpID() const {
    return getTagStr();
}


std::string
GNEMeanData::getHierarchyName() const {
    if (getParentLanes().size() > 0) {
        return getTagStr() + ": " + getParentLanes().front()->getID();
    } else {
        return getTagStr() + ": " + getParentEdges().front()->getID();
    }
    
}


const Parameterised::Map&
GNEMeanData::getACParametersMap() const {
    return GNEAttributeCarrier::PARAMETERS_EMPTY;
}


void
GNEMeanData::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_FILE:
            myFile = value;
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

/****************************************************************************/
