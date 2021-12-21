/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    GNEDetectorE3.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
//
/****************************************************************************/
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/changes/GNEChange_Attribute.h>

#include "GNEDetectorE3.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEDetectorE3::GNEDetectorE3(GNENet* net) :
    GNEAdditional("", net, GLO_E3DETECTOR, SUMO_TAG_E3DETECTOR, "",
        {}, {}, {}, {}, {}, {}, {}, {},
    std::map<std::string, std::string>()),
    myFreq(0),
    myFilename(""),
    myTimeThreshold(0),
    mySpeedThreshold(0) {
    // reset default values
    resetDefaultValues();
}


GNEDetectorE3::GNEDetectorE3(const std::string& id, GNENet* net, const Position pos, const SUMOTime freq, const std::string& filename,
                             const std::vector<std::string>& vehicleTypes, const std::string& name, SUMOTime timeThreshold, double speedThreshold,
                             const std::map<std::string, std::string>& parameters) :
    GNEAdditional(id, net, GLO_E3DETECTOR, SUMO_TAG_E3DETECTOR, name,
        {}, {}, {}, {}, {}, {}, {}, {},
    parameters),
    myPosition(pos),
    myFreq(freq),
    myFilename(filename),
    myVehicleTypes(vehicleTypes),
    myTimeThreshold(timeThreshold),
    mySpeedThreshold(speedThreshold) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEDetectorE3::~GNEDetectorE3() {}


GNEMoveOperation*
GNEDetectorE3::getMoveOperation() {
    // return move operation for additional placed in view
    return new GNEMoveOperation(this, myPosition);
}


void 
GNEDetectorE3::writeAdditional(OutputDevice& device) const {
    device.openTag(getTagProperty().getTag());
    device.writeAttr(SUMO_ATTR_ID, getID());
    if (!myAdditionalName.empty()) {
        device.writeAttr(SUMO_ATTR_NAME, StringUtils::escapeXML(myAdditionalName));
    }
    device.writeAttr(SUMO_ATTR_POSITION, myPosition);
    device.writeAttr(SUMO_ATTR_FREQUENCY, time2string(myFreq));
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
    // write parameters (Always after children to avoid problems with additionals.xsd)
    writeParams(device);
    device.closeTag();
}


void
GNEDetectorE3::updateGeometry() {
    // update additional geometry
    myAdditionalGeometry.updateSinglePosGeometry(myPosition, 0);
}


Position
GNEDetectorE3::getPositionInView() const {
    return myPosition;
}


void
GNEDetectorE3::updateCenteringBoundary(const bool updateGrid) {
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
GNEDetectorE3::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
}


std::string
GNEDetectorE3::getParentName() const {
    return myNet->getMicrosimID();
}


void
GNEDetectorE3::drawGL(const GUIVisualizationSettings& s) const {
    // draw parent and child lines
    drawParentChildLines(s, s.additionalSettings.connectionColor);
    // draw E3
    drawSquaredAdditional(s, myPosition, s.detectorSettings.E3Size, GUITexture::E3, GUITexture::E3_SELECTED);
}


std::string
GNEDetectorE3::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getID();
        case SUMO_ATTR_POSITION:
            return toString(myPosition);
        case SUMO_ATTR_FREQUENCY:
            return time2string(myFreq);
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
GNEDetectorE3::getAttributeDouble(SumoXMLAttr key) const {
    throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
}


void
GNEDetectorE3::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_FREQUENCY:
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
GNEDetectorE3::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidDetectorID(value);
        case SUMO_ATTR_POSITION:
            return canParse<Position>(value);
        case SUMO_ATTR_FREQUENCY:
            return canParse<SUMOTime>(value);
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
            return canParse<SUMOTime>(value);
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return Parameterised::areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEDetectorE3::checkChildAdditionalRestriction() const {
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
        WRITE_WARNING("An " + toString(SUMO_TAG_E3DETECTOR) + " need at least one " + toString(SUMO_TAG_DET_ENTRY) + " detector");
    }
    if (numExits == 0) {
        WRITE_WARNING("An " + toString(SUMO_TAG_E3DETECTOR) + " need at least one " + toString(SUMO_TAG_DET_EXIT) + " detector");
    }
    // return false depending of number of Entrys and Exits
    return ((numEntrys != 0) && (numExits != 0));
}


bool
GNEDetectorE3::isAttributeEnabled(SumoXMLAttr /* key */) const {
    return true;
}


std::string
GNEDetectorE3::getPopUpID() const {
    return getTagStr() + ":" + getID();
}


std::string
GNEDetectorE3::getHierarchyName() const {
    return getTagStr();
}

// ===========================================================================
// private
// ===========================================================================

void
GNEDetectorE3::setAttribute(SumoXMLAttr key, const std::string& value) {
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
        case SUMO_ATTR_FREQUENCY:
            myFreq = string2time(value);
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
GNEDetectorE3::setMoveShape(const GNEMoveResult& moveResult) {
    // update position
    myPosition = moveResult.shapeToUpdate.front();
    // update geometry
    updateGeometry();
}


void
GNEDetectorE3::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    undoList->begin(GUIIcon::E3, "position of " + getTagStr());
    undoList->changeAttribute(new GNEChange_Attribute(this, SUMO_ATTR_POSITION, toString(moveResult.shapeToUpdate.front())));
    undoList->end();
}


/****************************************************************************/
