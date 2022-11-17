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

GNEMeanData::GNEMeanData(GNENet *net, SumoXMLTag tag, const std::string& id) :
    GNEHierarchicalElement(net, tag, {}, {}, {}, {}, {}, {}),
    myID(id) {
}


GNEMeanData::GNEMeanData(GNENet *net, SumoXMLTag tag, std::string ID, std::string file, SUMOTime period,
        SUMOTime begin, SUMOTime end, const bool trackVehicles, const std::vector<std::string> &writtenAttributes,
        const bool aggregate, const std::vector<std::string> &edges, const std::vector<std::string> &edgeFile, 
        std::string excludeEmpty, const bool withInternal, const std::vector<std::string> &detectPersons, 
        const float minSamples, const float maxTravelTime, const std::vector<std::string> &vTypes, const float speedThreshold) :
    GNEHierarchicalElement(net, tag, {}, {}, {}, {}, {}, {}),
    myID(ID), 
    myFile(file),
    myPeriod(period),
    myBegin(begin),  
    myEnd(end), 
    myTrackVehicles(trackVehicles),
    myWrittenAttributes(writtenAttributes),
    myAggregate(aggregate),
    myEdges(edges),
    myEdgeFile(edgeFile),
    myExcludeEmpty(excludeEmpty),
    myWithInternal(withInternal),
    myDetectPersons(detectPersons), 
    myMinSamples(minSamples),
    myMaxTravelTime(maxTravelTime),
    myVTypes(vTypes), 
    mySpeedThreshold(speedThreshold) {
}


GNEMeanData::~GNEMeanData() {}


GUIGlObject*
GNEMeanData::getGUIGlObject() {
    return nullptr;
}


void
GNEMeanData::updateGeometry() {
    // nothing to update
}


Position
GNEMeanData::getPositionInView() const {
    return Position();
}


std::string
GNEMeanData::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return "";
        case SUMO_ATTR_FILE:
            return myFile;

        case SUMO_ATTR_PERIOD:
        case SUMO_ATTR_BEGIN:
        case SUMO_ATTR_END:
        case SUMO_ATTR_EXCLUDE_EMPTY:
        case SUMO_ATTR_WITH_INTERNAL:
        case SUMO_ATTR_MAX_TRAVELTIME:
        case SUMO_ATTR_MIN_SAMPLES:
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
        case SUMO_ATTR_VTYPES:
        case SUMO_ATTR_TRACK_VEHICLES:
        case SUMO_ATTR_DETECT_PERSONS:
        case SUMO_ATTR_WRITE_ATTRIBUTES:
        case SUMO_ATTR_EDGES:
        case SUMO_ATTR_EDGESFILE:
        case SUMO_ATTR_AGGREGATE:
            return "";

        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNEMeanData::getAttributeDouble(SumoXMLAttr key) const {
    throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
}


void
GNEMeanData::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_FILE:
        case SUMO_ATTR_PERIOD:
        case SUMO_ATTR_BEGIN:
        case SUMO_ATTR_END:
        case SUMO_ATTR_EXCLUDE_EMPTY:
        case SUMO_ATTR_WITH_INTERNAL:
        case SUMO_ATTR_MAX_TRAVELTIME:
        case SUMO_ATTR_MIN_SAMPLES:
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
        case SUMO_ATTR_VTYPES:
        case SUMO_ATTR_TRACK_VEHICLES:
        case SUMO_ATTR_DETECT_PERSONS:
        case SUMO_ATTR_WRITE_ATTRIBUTES:
        case SUMO_ATTR_EDGES:
        case SUMO_ATTR_EDGESFILE:
        case SUMO_ATTR_AGGREGATE:
            undoList->changeAttribute(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEMeanData::isValid(SumoXMLAttr key , const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_FILE:
        case SUMO_ATTR_PERIOD:
        case SUMO_ATTR_BEGIN:
        case SUMO_ATTR_END:
        case SUMO_ATTR_EXCLUDE_EMPTY:
        case SUMO_ATTR_WITH_INTERNAL:
        case SUMO_ATTR_MAX_TRAVELTIME:
        case SUMO_ATTR_MIN_SAMPLES:
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
        case SUMO_ATTR_VTYPES:
        case SUMO_ATTR_TRACK_VEHICLES:
        case SUMO_ATTR_DETECT_PERSONS:
        case SUMO_ATTR_WRITE_ATTRIBUTES:
        case SUMO_ATTR_EDGES:
        case SUMO_ATTR_EDGESFILE:
        case SUMO_ATTR_AGGREGATE:
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
    return getTagStr();
}


const Parameterised::Map&
GNEMeanData::getACParametersMap() const {
    return GNEAttributeCarrier::PARAMETERS_EMPTY;
}


void
GNEMeanData::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_FILE:
        case SUMO_ATTR_PERIOD:
        case SUMO_ATTR_BEGIN:
        case SUMO_ATTR_END:
        case SUMO_ATTR_EXCLUDE_EMPTY:
        case SUMO_ATTR_WITH_INTERNAL:
        case SUMO_ATTR_MAX_TRAVELTIME:
        case SUMO_ATTR_MIN_SAMPLES:
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
        case SUMO_ATTR_VTYPES:
        case SUMO_ATTR_TRACK_VEHICLES:
        case SUMO_ATTR_DETECT_PERSONS:
        case SUMO_ATTR_WRITE_ATTRIBUTES:
        case SUMO_ATTR_EDGES:
        case SUMO_ATTR_EDGESFILE:
        case SUMO_ATTR_AGGREGATE:
            myFile = value;
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

/****************************************************************************/
