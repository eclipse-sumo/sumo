/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
#include <netedit/frames/data/GNEMeanDataFrame.h>
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

GNEMeanData::GNEMeanData(GNENet* net, SumoXMLTag tag, const std::string& id) :
    GNEHierarchicalElement(net, tag, {}, {}, {}, {}, {}, {}),
myID(id) {
    // reset default values
    resetDefaultValues();
    // set file
    if (myFile.empty()) {
        myFile = (myID + ".xml");
    }
}


GNEMeanData::GNEMeanData(GNENet* net, SumoXMLTag tag, std::string ID, std::string file, SUMOTime period,
                         SUMOTime begin, SUMOTime end, const bool trackVehicles, const std::vector<SumoXMLAttr>& writtenAttributes,
                         const bool aggregate, const std::vector<std::string>& edges, const std::string& edgeFile,
                         std::string excludeEmpty, const bool withInternal, const std::vector<std::string>& detectPersons,
                         const double minSamples, const double maxTravelTime, const std::vector<std::string>& vTypes, const double speedThreshold) :
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
    // set file
    if (myFile.empty()) {
        myFile = (myID + ".xml");
    }
}


GNEMeanData::~GNEMeanData() {}


void
GNEMeanData::writeMeanData(OutputDevice& device) const {
    device.openTag(getTagProperty().getTag());
    // write needed attributes
    device.writeAttr(SUMO_ATTR_ID, getID());
    device.writeAttr(SUMO_ATTR_FILE, myFile);
    // write optional attributes
    if (myPeriod != -1) {
        device.writeAttr(SUMO_ATTR_PERIOD, STEPS2TIME(myPeriod));
    }
    if (myBegin != -1) {
        device.writeAttr(SUMO_ATTR_BEGIN, myBegin);
    }
    if (myEnd != -1) {
        device.writeAttr(SUMO_ATTR_END, myEnd);
    }
    if (myExcludeEmpty != "default") {
        device.writeAttr(SUMO_ATTR_EXCLUDE_EMPTY, myExcludeEmpty);
    }
    if (myWithInternal) {
        device.writeAttr(SUMO_ATTR_WITH_INTERNAL, true);
    }
    if (myMaxTravelTime != 100000) {
        device.writeAttr(SUMO_ATTR_MAX_TRAVELTIME, myMaxTravelTime);
    }
    if (myMinSamples != 0) {
        device.writeAttr(SUMO_ATTR_MIN_SAMPLES, myMinSamples);
    }
    if (mySpeedThreshold != 0.1) {
        device.writeAttr(SUMO_ATTR_HALTING_SPEED_THRESHOLD, mySpeedThreshold);
    }
    if (myVTypes.size() > 0) {
        device.writeAttr(SUMO_ATTR_VTYPES, getAttribute(SUMO_ATTR_VTYPES));
    }
    if (myTrackVehicles) {
        device.writeAttr(SUMO_ATTR_TRACK_VEHICLES, true);
    }
    if (myDetectPersons.size() > 0) {
        device.writeAttr(SUMO_ATTR_DETECT_PERSONS, myDetectPersons);
    }
    if (myWrittenAttributes.size() > 0) {
        device.writeAttr(SUMO_ATTR_WRITE_ATTRIBUTES, getAttribute(SUMO_ATTR_WRITE_ATTRIBUTES));
    }
    if (myEdges.size() > 0) {
        device.writeAttr(SUMO_ATTR_EDGES, myEdges);
    }
    if (myEdgeFile.size() > 0) {
        device.writeAttr(SUMO_ATTR_EDGESFILE, myEdgeFile);
    }
    if (myAggregate) {
        device.writeAttr(SUMO_ATTR_AGGREGATE, true);
    }
    device.closeTag();
}


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
            return myID;
        case SUMO_ATTR_FILE:
            return myFile;
        case SUMO_ATTR_PERIOD:
            if (myPeriod == -1) {
                return "";
            } else {
                return time2string(myPeriod);
            }
        case SUMO_ATTR_BEGIN:
            if (myBegin == -1) {
                return "";
            } else {
                return time2string(myBegin);
            }
        case SUMO_ATTR_END:
            if (myEnd == -1) {
                return "";
            } else {
                return time2string(myEnd);
            }
        case SUMO_ATTR_EXCLUDE_EMPTY:
            return myExcludeEmpty;
        case SUMO_ATTR_WITH_INTERNAL:
            return toString(myWithInternal);
        case SUMO_ATTR_MAX_TRAVELTIME:
            return toString(myMaxTravelTime);
        case SUMO_ATTR_MIN_SAMPLES:
            return toString(myMinSamples);
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            return toString(mySpeedThreshold);
        case SUMO_ATTR_VTYPES:
            return toString(myVTypes);
        case SUMO_ATTR_TRACK_VEHICLES:
            return toString(myTrackVehicles);
        case SUMO_ATTR_DETECT_PERSONS:
            return toString(myDetectPersons);
        case SUMO_ATTR_WRITE_ATTRIBUTES:
            return toString(myWrittenAttributes);
        case SUMO_ATTR_EDGES:
            return toString(myEdges);
        case SUMO_ATTR_EDGESFILE:
            return myEdgeFile;
        case SUMO_ATTR_AGGREGATE:
            return toString(myAggregate);
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
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
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
GNEMeanData::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return (myNet->getAttributeCarriers()->retrieveMeanData(myTagProperty.getTag(), value, false) == nullptr);
        case SUMO_ATTR_FILE:
            return SUMOXMLDefinitions::isValidFilename(value);
        case SUMO_ATTR_PERIOD:
        case SUMO_ATTR_BEGIN:
        case SUMO_ATTR_END:
            if (value.empty()) {
                return true;
            } else {
                return (canParse<double>(value) && (parse<double>(value) >= 0));
            }
        case SUMO_ATTR_EXCLUDE_EMPTY:
            if (canParse<bool>(value)) {
                return true;
            } else {
                return (value == "default");
            }
        case SUMO_ATTR_WITH_INTERNAL:
            return (canParse<bool>(value));
        case SUMO_ATTR_MAX_TRAVELTIME:
            if (value.empty()) {
                return true;
            } else {
                return (canParse<double>(value) && (parse<double>(value) >= 0));
            }
        case SUMO_ATTR_MIN_SAMPLES:
            return (canParse<double>(value) && (parse<double>(value) >= 0));
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            return (canParse<double>(value) && (parse<double>(value) >= 0));
        case SUMO_ATTR_VTYPES:
            return true;
        case SUMO_ATTR_TRACK_VEHICLES:
            return (canParse<bool>(value));
        case SUMO_ATTR_DETECT_PERSONS:
            if (value.empty()) {
                return true;
            } else {
                return (value == "walk");
            }
        case SUMO_ATTR_WRITE_ATTRIBUTES:
            return canParse<std::vector<SumoXMLAttr> >(value);
        case SUMO_ATTR_EDGES:
            return canParse<std::vector<GNEEdge*> >(myNet, value, false);
        case SUMO_ATTR_EDGESFILE:
            return SUMOXMLDefinitions::isValidFilename(value);
        case SUMO_ATTR_AGGREGATE:
            return (canParse<bool>(value));
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
            myID = value;
            break;
        case SUMO_ATTR_FILE:
            myFile = value;
            break;
        case SUMO_ATTR_PERIOD:
            if (value.empty()) {
                myPeriod = -1;
            } else {
                myPeriod = string2time(value);
            }
            break;
        case SUMO_ATTR_BEGIN:
            if (value.empty()) {
                myBegin = -1;
            } else {
                myBegin = string2time(value);
            }
            break;
        case SUMO_ATTR_END:
            if (value.empty()) {
                myEnd = -1;
            } else {
                myEnd = string2time(value);
            }
            break;
        case SUMO_ATTR_EXCLUDE_EMPTY:
            myExcludeEmpty = value;
            break;
        case SUMO_ATTR_WITH_INTERNAL:
            myWithInternal = parse<bool>(value);
            break;
        case SUMO_ATTR_MAX_TRAVELTIME:
            if (value.empty()) {
                myMaxTravelTime = 100000;
            } else {
                myMaxTravelTime = parse<double>(value);
            }
            break;
        case SUMO_ATTR_MIN_SAMPLES:
            myMinSamples = parse<double>(value);
            break;
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            mySpeedThreshold = parse<double>(value);
            break;
        case SUMO_ATTR_VTYPES:
            myVTypes = parse<std::vector<std::string> >(value);
            break;
        case SUMO_ATTR_TRACK_VEHICLES:
            myTrackVehicles = parse<bool>(value);
            break;
        case SUMO_ATTR_DETECT_PERSONS:
            myDetectPersons = parse<std::vector<std::string> >(value);
            break;
        case SUMO_ATTR_WRITE_ATTRIBUTES:
            myWrittenAttributes = parse<std::vector<SumoXMLAttr> >(value);
            break;
        case SUMO_ATTR_EDGES:
            myEdges = parse<std::vector<std::string> >(value);
            break;
        case SUMO_ATTR_EDGESFILE:
            myEdgeFile = value;
            break;
        case SUMO_ATTR_AGGREGATE:
            myAggregate = parse<bool>(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

/****************************************************************************/
