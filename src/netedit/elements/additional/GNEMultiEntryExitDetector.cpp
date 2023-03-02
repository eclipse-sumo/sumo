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
/// @file    GNEMultiEntryExitDetector.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
//
/****************************************************************************/

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/frames/network/GNETLSEditorFrame.h>

#include "GNEMultiEntryExitDetector.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEMultiEntryExitDetector::GNEMultiEntryExitDetector(GNENet* net) :
    GNEAdditional("", net, GLO_E3DETECTOR, SUMO_TAG_ENTRY_EXIT_DETECTOR, GUIIconSubSys::getIcon(GUIIcon::E3ENTRY), "", {}, {}, {}, {}, {}, {}),
              myPeriod(0),
              myFilename(""),
              myTimeThreshold(0),
mySpeedThreshold(0) {
    // reset default values
    resetDefaultValues();
}


GNEMultiEntryExitDetector::GNEMultiEntryExitDetector(const std::string& id, GNENet* net, const Position pos, const SUMOTime freq, const std::string& filename,
        const std::vector<std::string>& vehicleTypes, const std::string& name, SUMOTime timeThreshold, double speedThreshold,
        const Parameterised::Map& parameters) :
    GNEAdditional(id, net, GLO_E3DETECTOR, SUMO_TAG_ENTRY_EXIT_DETECTOR, GUIIconSubSys::getIcon(GUIIcon::E3EXIT), name, {}, {}, {}, {}, {}, {}),
Parameterised(parameters),
myPosition(pos),
myPeriod(freq),
myFilename(filename),
myVehicleTypes(vehicleTypes),
myTimeThreshold(timeThreshold),
mySpeedThreshold(speedThreshold) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEMultiEntryExitDetector::~GNEMultiEntryExitDetector() {}


GNEMoveOperation*
GNEMultiEntryExitDetector::getMoveOperation() {
    // return move operation for additional placed in view
    return new GNEMoveOperation(this, myPosition);
}


void
GNEMultiEntryExitDetector::writeAdditional(OutputDevice& device) const {
    bool entry = false;
    bool exit = false;
    // first check if E3 has at least one entry and one exit
    for (const auto& additionalChild : getChildAdditionals()) {
        if (additionalChild->getTagProperty().getTag() == SUMO_TAG_DET_ENTRY) {
            entry = true;
        } else if (additionalChild->getTagProperty().getTag() == SUMO_TAG_DET_EXIT) {
            exit = true;
        }
    }
    // check entry/exits
    if (entry && exit) {
        device.openTag(getTagProperty().getTag());
        device.writeAttr(SUMO_ATTR_ID, getID());
        if (!myAdditionalName.empty()) {
            device.writeAttr(SUMO_ATTR_NAME, StringUtils::escapeXML(myAdditionalName));
        }
        device.writeAttr(SUMO_ATTR_POSITION, myPosition);
        if (getAttribute(SUMO_ATTR_PERIOD).size() > 0) {
            device.writeAttr(SUMO_ATTR_PERIOD, time2string(myPeriod));
        }
        if (myFilename.size() > 0) {
            device.writeAttr(SUMO_ATTR_FILE, myFilename);
        }
        if (myVehicleTypes.size() > 0) {
            device.writeAttr(SUMO_ATTR_VTYPES, myVehicleTypes);
        }
        if (getAttribute(SUMO_ATTR_HALTING_TIME_THRESHOLD) != myTagProperty.getDefaultValue(SUMO_ATTR_HALTING_TIME_THRESHOLD)) {
            device.writeAttr(SUMO_ATTR_HALTING_TIME_THRESHOLD, myTimeThreshold);
        }
        if (getAttribute(SUMO_ATTR_HALTING_SPEED_THRESHOLD) != myTagProperty.getDefaultValue(SUMO_ATTR_HALTING_SPEED_THRESHOLD)) {
            device.writeAttr(SUMO_ATTR_HALTING_SPEED_THRESHOLD, mySpeedThreshold);
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


void
GNEMultiEntryExitDetector::updateGeometry() {
    // update additional geometry
    myAdditionalGeometry.updateSinglePosGeometry(myPosition, 0);
}


Position
GNEMultiEntryExitDetector::getPositionInView() const {
    return myPosition;
}


void
GNEMultiEntryExitDetector::updateCenteringBoundary(const bool updateGrid) {
    // remove additional from grid
    if (updateGrid) {
        myNet->removeGLObjectFromGrid(this);
    }
    // now update geometry
    updateGeometry();
    // add shape boundary
    myAdditionalBoundary = myAdditionalGeometry.getShape().getBoxBoundary();
    // add positions of all childrens
    for (const auto& additionalChildren : getChildAdditionals()) {
        myAdditionalBoundary.add(additionalChildren->getPositionInView());
    }
    // grow
    myAdditionalBoundary.grow(10);
    // add additional into RTREE again
    if (updateGrid) {
        myNet->addGLObjectIntoGrid(this);
    }
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
    // check if drawn
    if (!myNet->getViewNet()->selectingDetectorsTLSMode()) {
        // draw parent and child lines
        drawParentChildLines(s, s.additionalSettings.connectionColor);
        // draw E3
        drawSquaredAdditional(s, myPosition, s.detectorSettings.E3Size, GUITexture::E3, GUITexture::E3_SELECTED);
    }
}


std::string
GNEMultiEntryExitDetector::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_POSITION:
            return toString(myPosition);
        case SUMO_ATTR_PERIOD:
            if (myPeriod == SUMOTime_MAX_PERIOD) {
                return "";
            } else {
                return time2string(myPeriod);
            }
        case SUMO_ATTR_NAME:
            return myAdditionalName;
        case SUMO_ATTR_FILE:
            return myFilename;
        case SUMO_ATTR_VTYPES:
            return toString(myVehicleTypes);
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            return time2string(myTimeThreshold);
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            return toString(mySpeedThreshold);
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARAMETERS:
            return getParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNEMultiEntryExitDetector::getAttributeDouble(SumoXMLAttr key) const {
    throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
}


const Parameterised::Map&
GNEMultiEntryExitDetector::getACParametersMap() const {
    return getParametersMap();
}


void
GNEMultiEntryExitDetector::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_PERIOD:
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_NAME:
        case SUMO_ATTR_FILE:
        case SUMO_ATTR_VTYPES:
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
            undoList->changeAttribute(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEMultiEntryExitDetector::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidDetectorID(value);
        case SUMO_ATTR_POSITION:
            return canParse<Position>(value);
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
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEMultiEntryExitDetector::checkChildAdditionalRestriction() const {
    int numEntrys = 0;
    int numExits = 0;
    // iterate over additional chidls and obtain number of entrys and exits
    for (auto i : getChildAdditionals()) {
        if (i->getTagProperty().getTag() == SUMO_TAG_DET_ENTRY) {
            numEntrys++;
        } else if (i->getTagProperty().getTag() == SUMO_TAG_DET_EXIT) {
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
            setMicrosimID(value);
            // Change IDs of all Entry/Exits children
            for (const auto& entryExit : getChildAdditionals()) {
                entryExit->setMicrosimID(getID());
            }
            break;
        case SUMO_ATTR_POSITION:
            myPosition = parse<Position>(value);
            // update boundary (except for template)
            if (getID().size() > 0) {
                updateCenteringBoundary(true);
            }
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
            myFilename = value;
            break;
        case SUMO_ATTR_VTYPES:
            myVehicleTypes = parse<std::vector<std::string> >(value);
            break;
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            myTimeThreshold = parse<SUMOTime>(value);
            break;
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            mySpeedThreshold = parse<double>(value);
            break;
        case GNE_ATTR_SELECTED:
            if (parse<bool>(value)) {
                selectAttributeCarrier();
            } else {
                unselectAttributeCarrier();
            }
            break;
        case GNE_ATTR_PARAMETERS:
            setParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEMultiEntryExitDetector::setMoveShape(const GNEMoveResult& moveResult) {
    // update position
    myPosition = moveResult.shapeToUpdate.front();
    // update geometry
    updateGeometry();
}


void
GNEMultiEntryExitDetector::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    undoList->begin(GUIIcon::E3, "position of " + getTagStr());
    undoList->changeAttribute(new GNEChange_Attribute(this, SUMO_ATTR_POSITION, toString(moveResult.shapeToUpdate.front())));
    undoList->end();
}


/****************************************************************************/
