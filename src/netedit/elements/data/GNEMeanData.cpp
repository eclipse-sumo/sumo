/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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

#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/changes/GNEChange_Attribute.h>

#include "GNEMeanData.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEMeanData::GNEMeanData(SumoXMLTag tag, std::string ID, GNENet* net, const std::string& filename) :
    GNEAttributeCarrier(tag, net, filename, true),
    myID(ID) {
    // reset default values
    resetDefaultValues(false);
}


GNEMeanData::GNEMeanData(SumoXMLTag tag, std::string ID, GNENet* net, const std::string& filename, const std::string& file, const std::string& type,
                         const SUMOTime period, const SUMOTime begin, const SUMOTime end, const bool trackVehicles, const std::vector<SumoXMLAttr>& writtenAttributes,
                         const bool aggregate, const std::vector<std::string>& edges, const std::string& edgeFile, const std::string& excludeEmpty,
                         const bool withInternal, const std::vector<std::string>& detectPersons, const double minSamples, const double maxTravelTime,
                         const std::vector<std::string>& vTypes, const double speedThreshold) :
    GNEAttributeCarrier(tag, net, filename, false),
    myID(ID),
    myFile(file),
    myType(type),
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


GNEHierarchicalElement*
GNEMeanData::getHierarchicalElement() {
    return this;
}


GNEMoveElement*
GNEMeanData::getMoveElement() const {
    return nullptr;
}


Parameterised*
GNEMeanData::getParameters() {
    return nullptr;
}


const Parameterised*
GNEMeanData::getParameters() const {
    return nullptr;
}


GUIGlObject*
GNEMeanData::getGUIGlObject() {
    return nullptr;
}


const GUIGlObject*
GNEMeanData::getGUIGlObject() const {
    return nullptr;
}


void
GNEMeanData::writeMeanData(OutputDevice& device) const {
    device.openTag(getTagProperty()->getTag());
    // write needed attributes
    device.writeAttr(SUMO_ATTR_ID, getID());
    if (myFile.empty()) {
        device.writeAttr(SUMO_ATTR_FILE, myID + ".xml");
    } else {
        device.writeAttr(SUMO_ATTR_FILE, myFile);
    }
    // write optional attributes
    if (myType != myTagProperty->getDefaultStringValue(SUMO_ATTR_TYPE)) {
        device.writeAttr(SUMO_ATTR_TYPE, myType);
    }
    if (myPeriod != myTagProperty->getDefaultTimeValue(SUMO_ATTR_PERIOD)) {
        device.writeAttr(SUMO_ATTR_PERIOD, time2string(myPeriod));
    }
    if (myBegin != myTagProperty->getDefaultTimeValue(SUMO_ATTR_BEGIN)) {
        device.writeAttr(SUMO_ATTR_BEGIN, time2string(myBegin));
    }
    if (myEnd != myTagProperty->getDefaultTimeValue(SUMO_ATTR_END)) {
        device.writeAttr(SUMO_ATTR_END, time2string(myEnd));
    }
    if (myExcludeEmpty != myTagProperty->getDefaultStringValue(SUMO_ATTR_EXCLUDE_EMPTY)) {
        device.writeAttr(SUMO_ATTR_EXCLUDE_EMPTY, myExcludeEmpty);
    }
    if (myWithInternal) {
        device.writeAttr(SUMO_ATTR_WITH_INTERNAL, true);
    }
    if (myMaxTravelTime != myTagProperty->getDefaultDoubleValue(SUMO_ATTR_MAX_TRAVELTIME)) {
        device.writeAttr(SUMO_ATTR_MAX_TRAVELTIME, myMaxTravelTime);
    }
    if (myMinSamples != myTagProperty->getDefaultDoubleValue(SUMO_ATTR_MIN_SAMPLES)) {
        device.writeAttr(SUMO_ATTR_MIN_SAMPLES, myMinSamples);
    }
    if (mySpeedThreshold != myTagProperty->getDefaultDoubleValue(SUMO_ATTR_HALTING_SPEED_THRESHOLD)) {
        device.writeAttr(SUMO_ATTR_HALTING_SPEED_THRESHOLD, mySpeedThreshold);
    }
    if (myVTypes.size() > 0) {
        device.writeAttr(SUMO_ATTR_VTYPES, getAttribute(SUMO_ATTR_VTYPES));
    }
    if (myTrackVehicles) {
        device.writeAttr(SUMO_ATTR_TRACK_VEHICLES, true);
    }
    if (myDetectPersons.size() > 0 && (myDetectPersons.at(0) != SUMOXMLDefinitions::PersonModeValues.getString(PersonMode::NONE))) {
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


void
GNEMeanData::updateGeometry() {
    // nothing to update
}


Position
GNEMeanData::getPositionInView() const {
    return Position();
}


bool
GNEMeanData::checkDrawFromContour() const {
    return false;
}


bool
GNEMeanData::checkDrawToContour() const {
    return false;
}


bool
GNEMeanData::checkDrawRelatedContour() const {
    return false;
}


bool
GNEMeanData::checkDrawOverContour() const {
    return false;
}


bool
GNEMeanData::checkDrawDeleteContour() const {
    return false;
}


bool
GNEMeanData::checkDrawDeleteContourSmall() const {
    return false;
}


bool
GNEMeanData::checkDrawSelectContour() const {
    return false;
}


bool
GNEMeanData::checkDrawMoveContour() const {
    return false;
}


std::string
GNEMeanData::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return myID;
        case SUMO_ATTR_FILE:
            return myFile;
        case SUMO_ATTR_TYPE:
            return myType;
        case SUMO_ATTR_PERIOD:
            if (myPeriod == myTagProperty->getDefaultTimeValue(key)) {
                return "";
            } else {
                return time2string(myPeriod);
            }
        case SUMO_ATTR_BEGIN:
            if (myBegin == myTagProperty->getDefaultTimeValue(key)) {
                return "";
            } else {
                return time2string(myBegin);
            }
        case SUMO_ATTR_END:
            if (myEnd == myTagProperty->getDefaultTimeValue(key)) {
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
            return joinToString(myWrittenAttributes, " ");
        case SUMO_ATTR_EDGES:
            return toString(myEdges);
        case SUMO_ATTR_EDGESFILE:
            return myEdgeFile;
        case SUMO_ATTR_AGGREGATE:
            return toString(myAggregate);
        default:
            return getCommonAttribute(key);
    }
}


double
GNEMeanData::getAttributeDouble(SumoXMLAttr key) const {
    return getCommonAttributeDouble(key);
}


Position
GNEMeanData::getAttributePosition(SumoXMLAttr key) const {
    return getCommonAttributePosition(key);
}


PositionVector
GNEMeanData::getAttributePositionVector(SumoXMLAttr key) const {
    return getCommonAttributePositionVector(key);
}


void
GNEMeanData::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_FILE:
        case SUMO_ATTR_TYPE:
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
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            setCommonAttribute(key, value, undoList);
            break;
    }
}


bool
GNEMeanData::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return SUMOXMLDefinitions::isValidNetID(value) && (myNet->getAttributeCarriers()->retrieveMeanData(myTagProperty->getTag(), value, false) == nullptr);
        case SUMO_ATTR_FILE:
            return SUMOXMLDefinitions::isValidFilename(value);
        case SUMO_ATTR_TYPE:
            return SUMOXMLDefinitions::MeanDataTypes.hasString(value);
        case SUMO_ATTR_PERIOD:
        case SUMO_ATTR_BEGIN:
        case SUMO_ATTR_END:
            if (value.empty()) {
                return true;
            } else {
                return (canParse<SUMOTime>(value) && (parse<SUMOTime>(value) >= 0));
            }
        case SUMO_ATTR_EXCLUDE_EMPTY:
            if (canParse<bool>(value)) {
                return true;
            } else {
                return (value == SUMOXMLDefinitions::ExcludeEmptys.getString(ExcludeEmpty::DEFAULTS));
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
                return SUMOXMLDefinitions::PersonModeValues.hasString(value);
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
            return isCommonAttributeValid(key, value);
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


void
GNEMeanData::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            myNet->getAttributeCarriers()->updateMeanDataID(this, value);
            myID = value;
            break;
        case SUMO_ATTR_FILE:
            if (value.empty()) {
                myFile = (myID + ".xml");
            } else {
                myFile = value;
            }
            break;
        case SUMO_ATTR_TYPE:
            myType = value;
            break;
        case SUMO_ATTR_PERIOD:
            if (value.empty()) {
                myPeriod = myTagProperty->getDefaultTimeValue(key);
            } else {
                myPeriod = string2time(value);
            }
            break;
        case SUMO_ATTR_BEGIN:
            if (value.empty()) {
                myBegin = myTagProperty->getDefaultTimeValue(key);
            } else {
                myBegin = string2time(value);
            }
            break;
        case SUMO_ATTR_END:
            if (value.empty()) {
                myEnd = myTagProperty->getDefaultTimeValue(key);
            } else {
                myEnd = string2time(value);
            }
            break;
        case SUMO_ATTR_EXCLUDE_EMPTY:
            if (value == SUMOXMLDefinitions::ExcludeEmptys.getString(ExcludeEmpty::DEFAULTS)) {
                myExcludeEmpty = value;
            } else if (parse<bool>(value)) {
                myExcludeEmpty = SUMOXMLDefinitions::ExcludeEmptys.getString(ExcludeEmpty::TRUES);
            } else {
                myExcludeEmpty = SUMOXMLDefinitions::ExcludeEmptys.getString(ExcludeEmpty::FALSES);
            }
            break;
        case SUMO_ATTR_WITH_INTERNAL:
            myWithInternal = parse<bool>(value);
            break;
        case SUMO_ATTR_MAX_TRAVELTIME:
            if (value.empty()) {
                myMaxTravelTime = myTagProperty->getDefaultDoubleValue(key);
            } else {
                myMaxTravelTime = parse<double>(value);
            }
            break;
        case SUMO_ATTR_MIN_SAMPLES:
            if (value.empty()) {
                myMinSamples = myTagProperty->getDefaultDoubleValue(key);
            } else {
                myMinSamples = parse<double>(value);
            }
            break;
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            if (value.empty()) {
                mySpeedThreshold = myTagProperty->getDefaultDoubleValue(key);
            } else {
                mySpeedThreshold = parse<double>(value);
            }
            break;
        case SUMO_ATTR_VTYPES:
            myVTypes = parse<std::vector<std::string> >(value);
            break;
        case SUMO_ATTR_TRACK_VEHICLES:
            myTrackVehicles = parse<bool>(value);
            break;
        case SUMO_ATTR_DETECT_PERSONS:
            myDetectPersons.clear();
            if (value.empty()) {
                myDetectPersons.push_back(SUMOXMLDefinitions::PersonModeValues.getString(PersonMode::NONE));
            } else {
                myDetectPersons = parse<std::vector<std::string> >(value);
            }
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
            setCommonAttribute(key, value);
            break;
    }
}

/****************************************************************************/
