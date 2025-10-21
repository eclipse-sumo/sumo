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
/// @file    GNEMultiEntryExitDetector.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
// multi entry-exit (E3) detector
/****************************************************************************/

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/elements/moving/GNEMoveElementView.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>

#include "GNEMultiEntryExitDetector.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEMultiEntryExitDetector::GNEMultiEntryExitDetector(GNENet* net) :
    GNEAdditional("", net, "", SUMO_TAG_ENTRY_EXIT_DETECTOR, ""),
    GNEAdditionalSquared(this) {
}


GNEMultiEntryExitDetector::GNEMultiEntryExitDetector(const std::string& id, GNENet* net, const std::string& filename, const Position pos, const SUMOTime freq,
        const std::string& outputFilename, const std::vector<std::string>& vehicleTypes, const std::vector<std::string>& nextEdges, const std::string& detectPersons,
        const std::string& name, const SUMOTime timeThreshold, const double speedThreshold, const bool openEntry, const bool expectedArrival, const Parameterised::Map& parameters) :
    GNEAdditional(id, net, filename, SUMO_TAG_ENTRY_EXIT_DETECTOR, name),
    GNEAdditionalSquared(this, pos),
    Parameterised(parameters),
    myPeriod(freq),
    myOutputFilename(outputFilename),
    myVehicleTypes(vehicleTypes),
    myNextEdges(nextEdges),
    myDetectPersons(detectPersons),
    myTimeThreshold(timeThreshold),
    mySpeedThreshold(speedThreshold),
    myOpenEntry(openEntry),
    myExpectedArrival(expectedArrival) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
    // set default output filename
    if (outputFilename.empty()) {
        myOutputFilename = id + ".xml";
    }
}


GNEMultiEntryExitDetector::~GNEMultiEntryExitDetector() {
}


GNEMoveElement*
GNEMultiEntryExitDetector::getMoveElement() const {
    return myMoveElementView;
}


Parameterised*
GNEMultiEntryExitDetector::getParameters() {
    return this;
}


const Parameterised*
GNEMultiEntryExitDetector::getParameters() const {
    return this;
}


void
GNEMultiEntryExitDetector::writeAdditional(OutputDevice& device) const {
    bool entry = false;
    bool exit = false;
    // first check if E3 has at least one entry and one exit
    for (const auto& additionalChild : getChildAdditionals()) {
        if (additionalChild->getTagProperty()->getTag() == SUMO_TAG_DET_ENTRY) {
            entry = true;
        } else if (additionalChild->getTagProperty()->getTag() == SUMO_TAG_DET_EXIT) {
            exit = true;
        }
    }
    // check entry/exits
    if (entry && exit) {
        device.openTag(getTagProperty()->getTag());
        // write common additional attributes
        writeAdditionalAttributes(device);
        // write move atributes
        myMoveElementView->writeMoveAttributes(device);
        // write specific attributes
        if (getAttribute(SUMO_ATTR_PERIOD).size() > 0) {
            device.writeAttr(SUMO_ATTR_PERIOD, time2string(myPeriod));
        }
        if (myOutputFilename.size() > 0) {
            device.writeAttr(SUMO_ATTR_FILE, myOutputFilename);
        }
        if (myVehicleTypes.size() > 0) {
            device.writeAttr(SUMO_ATTR_VTYPES, myVehicleTypes);
        }
        if (myTimeThreshold != myTagProperty->getDefaultTimeValue(SUMO_ATTR_HALTING_TIME_THRESHOLD)) {
            device.writeAttr(SUMO_ATTR_HALTING_TIME_THRESHOLD, time2string(myTimeThreshold));
        }
        if (mySpeedThreshold != myTagProperty->getDefaultDoubleValue(SUMO_ATTR_HALTING_SPEED_THRESHOLD)) {
            device.writeAttr(SUMO_ATTR_HALTING_SPEED_THRESHOLD, mySpeedThreshold);
        }
        if (myExpectedArrival != myTagProperty->getDefaultBoolValue(SUMO_ATTR_EXPECT_ARRIVAL)) {
            device.writeAttr(SUMO_ATTR_EXPECT_ARRIVAL, myExpectedArrival);
        }
        if (myOpenEntry != myTagProperty->getDefaultBoolValue(SUMO_ATTR_OPEN_ENTRY)) {
            device.writeAttr(SUMO_ATTR_OPEN_ENTRY, myOpenEntry);
        }
        // write all entry/exits
        for (const auto& access : getChildAdditionals()) {
            access->writeAdditional(device);
        }
        // write parameters (Always after children to avoid problems with additionals.xsd)
        writeParams(device);
        device.closeTag();
    } else {
        WRITE_WARNING("E3 '" + getID() + TL("' needs at least one entry and one exit"));
    }
}


bool
GNEMultiEntryExitDetector::isAdditionalValid() const {
    return true;
}


std::string
GNEMultiEntryExitDetector::getAdditionalProblem() const {
    return "";
}


void
GNEMultiEntryExitDetector::fixAdditionalProblem() {
    // nothing to fix
}


bool
GNEMultiEntryExitDetector::checkDrawMoveContour() const {
    // get edit modes
    const auto& editModes = myNet->getViewNet()->getEditModes();
    // check if we're in move mode
    if (!myNet->getViewNet()->isCurrentlyMovingElements() && editModes.isCurrentSupermodeNetwork() &&
            !myNet->getViewNet()->getEditNetworkElementShapes().getEditedNetworkElement() &&
            (editModes.networkEditMode == NetworkEditMode::NETWORK_MOVE) && myNet->getViewNet()->checkOverLockedElement(this, mySelected)) {
        // only move the first element
        return myNet->getViewNet()->getViewObjectsSelector().getGUIGlObjectFront() == this;
    } else {
        return false;
    }
}


void
GNEMultiEntryExitDetector::updateGeometry() {
    updatedSquaredGeometry();
}


Position
GNEMultiEntryExitDetector::getPositionInView() const {
    return myPosOverView;
}


void
GNEMultiEntryExitDetector::updateCenteringBoundary(const bool updateGrid) {
    updatedSquaredCenteringBoundary(updateGrid);
}


void
GNEMultiEntryExitDetector::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
}


std::string
GNEMultiEntryExitDetector::getParentName() const {
    return myNet->getMicrosimID();
}


void
GNEMultiEntryExitDetector::drawGL(const GUIVisualizationSettings& s) const {
    // first check if additional has to be drawn
    if (myNet->getViewNet()->getDataViewOptions().showAdditionals() &&
            !myNet->getViewNet()->selectingDetectorsTLSMode()) {
        // draw parent and child lines
        drawParentChildLines(s, s.additionalSettings.connectionColor);
        // draw E3
        drawSquaredAdditional(s, s.detectorSettings.E3Size, GUITexture::E3, GUITexture::E3_SELECTED);
    }
}


std::string
GNEMultiEntryExitDetector::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_PERIOD:
            if (myPeriod == SUMOTime_MAX_PERIOD) {
                return "";
            } else {
                return time2string(myPeriod);
            }
        case SUMO_ATTR_NAME:
            return myAdditionalName;
        case SUMO_ATTR_FILE:
            return myOutputFilename;
        case SUMO_ATTR_VTYPES:
            return toString(myVehicleTypes);
        case SUMO_ATTR_NEXT_EDGES:
            return toString(myNextEdges);
        case SUMO_ATTR_DETECT_PERSONS:
            return toString(myDetectPersons);
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            return time2string(myTimeThreshold);
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            return toString(mySpeedThreshold);
        case SUMO_ATTR_OPEN_ENTRY:
            return toString(myOpenEntry);
        case SUMO_ATTR_EXPECT_ARRIVAL:
            return toString(myExpectedArrival);
        default:
            return myMoveElementView->getMovingAttribute(key);
    }
}


double
GNEMultiEntryExitDetector::getAttributeDouble(SumoXMLAttr key) const {
    return myMoveElementView->getMovingAttributeDouble(key);
}


Position
GNEMultiEntryExitDetector::getAttributePosition(SumoXMLAttr key) const {
    return myMoveElementView->getMovingAttributePosition(key);
}


PositionVector
GNEMultiEntryExitDetector::getAttributePositionVector(SumoXMLAttr key) const {
    return myMoveElementView->getMovingAttributePositionVector(key);
}


void
GNEMultiEntryExitDetector::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_PERIOD:
        case SUMO_ATTR_NAME:
        case SUMO_ATTR_FILE:
        case SUMO_ATTR_VTYPES:
        case SUMO_ATTR_NEXT_EDGES:
        case SUMO_ATTR_DETECT_PERSONS:
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
        case SUMO_ATTR_OPEN_ENTRY:
        case SUMO_ATTR_EXPECT_ARRIVAL:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            myMoveElementView->setMovingAttribute(key, value, undoList);
            break;
    }
}


bool
GNEMultiEntryExitDetector::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidDetectorID(value);
        case SUMO_ATTR_PERIOD:
            if (value.empty()) {
                return true;
            } else {
                return (canParse<double>(value) && (parse<double>(value) >= 0));
            }
        case SUMO_ATTR_NAME:
            return SUMOXMLDefinitions::isValidAttribute(value);
        case SUMO_ATTR_FILE:
            return SUMOXMLDefinitions::isValidFilename(value);
        case SUMO_ATTR_VTYPES:
            if (value.empty()) {
                return true;
            } else {
                return SUMOXMLDefinitions::isValidListOfTypeID(value);
            }
        case SUMO_ATTR_NEXT_EDGES:
            if (value.empty()) {
                return true;
            } else {
                return SUMOXMLDefinitions::isValidListOfNetIDs(value);
            }
        case SUMO_ATTR_DETECT_PERSONS:
            if (value.empty()) {
                return true;
            } else {
                return SUMOXMLDefinitions::PersonModeValues.hasString(value);
            }
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case SUMO_ATTR_OPEN_ENTRY:
        case SUMO_ATTR_EXPECT_ARRIVAL:
            return canParse<bool>(value);
        default:
            return myMoveElementView->isMovingAttributeValid(key, value);
    }
}


bool
GNEMultiEntryExitDetector::checkChildAdditionalRestriction() const {
    int numEntrys = 0;
    int numExits = 0;
    // iterate over additional chidls and obtain number of entrys and exits
    for (auto i : getChildAdditionals()) {
        if (i->getTagProperty()->getTag() == SUMO_TAG_DET_ENTRY) {
            numEntrys++;
        } else if (i->getTagProperty()->getTag() == SUMO_TAG_DET_EXIT) {
            numExits++;
        }
    }
    // write warnings
    if (numEntrys == 0) {
        WRITE_WARNING(TL("An entry-exit detector needs at least one entry detector"));
    }
    if (numExits == 0) {
        WRITE_WARNING(TL("An entry-exit detector needs at least one exit detector"));
    }
    // return false depending of number of Entrys and Exits
    return ((numEntrys != 0) && (numExits != 0));
}


std::string
GNEMultiEntryExitDetector::getPopUpID() const {
    return getTagStr() + ":" + getID();
}


std::string
GNEMultiEntryExitDetector::getHierarchyName() const {
    return getTagStr();
}

// ===========================================================================
// private
// ===========================================================================

void
GNEMultiEntryExitDetector::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            // update microsimID
            setAdditionalID(value);
            break;
        case SUMO_ATTR_PERIOD:
            if (value.empty()) {
                myPeriod = SUMOTime_MAX_PERIOD;
            } else {
                myPeriod = string2time(value);
            }
            break;
        case SUMO_ATTR_NAME:
            myAdditionalName = value;
            break;
        case SUMO_ATTR_FILE:
            myOutputFilename = value;
            break;
        case SUMO_ATTR_VTYPES:
            myVehicleTypes = parse<std::vector<std::string> >(value);
            break;
        case SUMO_ATTR_NEXT_EDGES:
            myNextEdges = parse<std::vector<std::string> >(value);
            break;
        case SUMO_ATTR_DETECT_PERSONS:
            myDetectPersons = value;
            break;
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            myTimeThreshold = parse<SUMOTime>(value);
            break;
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            mySpeedThreshold = parse<double>(value);
            break;
        case SUMO_ATTR_OPEN_ENTRY:
            myOpenEntry = parse<bool>(value);
            break;
        case SUMO_ATTR_EXPECT_ARRIVAL:
            myExpectedArrival = parse<bool>(value);
            break;
        default:
            myMoveElementView->setMovingAttribute(key, value);
            break;
    }
    // update boundary (except for template)
    if (getID().size() > 0) {
        updateCenteringBoundary(true);
    }
}

/****************************************************************************/
