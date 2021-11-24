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
/// @file    GNECalibrator.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
//
/****************************************************************************/
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/dialogs/GNECalibratorDialog.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNECalibrator.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNECalibrator::GNECalibrator(SumoXMLTag tag, GNENet* net) :
    GNEAdditional("", net, GLO_CALIBRATOR, tag, "",
        {}, {}, {}, {}, {}, {}, {}, {},
    std::map<std::string, std::string>()),
    myPositionOverLane(0),
    myFrequency(0),
    myJamThreshold(0) {
    // reset default values
    resetDefaultValues();
}


GNECalibrator::GNECalibrator(const std::string& id, GNENet* net, GNEEdge* edge, double pos, SUMOTime frequency, const std::string& name,
                             const std::string& output, const double jamThreshold, const std::vector<std::string>& vTypes, const std::map<std::string, std::string>& parameters) :
    GNEAdditional(id, net, GLO_CALIBRATOR, SUMO_TAG_CALIBRATOR, name,
        {}, {edge}, {}, {}, {}, {}, {}, {},
    parameters),
    myPositionOverLane(pos),
    myFrequency(frequency),
    myOutput(output),
    myJamThreshold(jamThreshold),
    myVTypes(vTypes) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNECalibrator::GNECalibrator(const std::string& id, GNENet* net, GNEEdge* edge, double pos, SUMOTime frequency, const std::string& name,
                             const std::string& output, GNEAdditional* routeProbe, const double jamThreshold, const std::vector<std::string>& vTypes,
                             const std::map<std::string, std::string>& parameters) :
    GNEAdditional(id, net, GLO_CALIBRATOR, SUMO_TAG_CALIBRATOR, name,
        {}, {edge}, {}, {routeProbe}, {}, {}, {}, {},
    parameters),
    myPositionOverLane(pos),
    myFrequency(frequency),
    myOutput(output),
    myJamThreshold(jamThreshold),
    myVTypes(vTypes) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNECalibrator::GNECalibrator(const std::string& id, GNENet* net, GNELane* lane, double pos, SUMOTime frequency, const std::string& name,
                             const std::string& output, const double jamThreshold, const std::vector<std::string>& vTypes, const std::map<std::string, std::string>& parameters) :
    GNEAdditional(id, net, GLO_CALIBRATOR, SUMO_TAG_LANECALIBRATOR, name,
        {}, {}, {lane}, {}, {}, {}, {}, {},
    parameters),
    myPositionOverLane(pos),
    myFrequency(frequency),
    myOutput(output),
    myJamThreshold(jamThreshold),
    myVTypes(vTypes) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNECalibrator::GNECalibrator(const std::string& id, GNENet* net, GNELane* lane, double pos, SUMOTime frequency, const std::string& name,
                             const std::string& output, GNEAdditional* routeProbe, const double jamThreshold, const std::vector<std::string>& vTypes,
                             const std::map<std::string, std::string>& parameters) :
    GNEAdditional(id, net, GLO_CALIBRATOR, SUMO_TAG_LANECALIBRATOR, name,
        {}, {}, {lane}, {routeProbe}, {}, {}, {}, {},
    parameters),
    myPositionOverLane(pos),
    myFrequency(frequency),
    myOutput(output),
    myJamThreshold(jamThreshold),
    myVTypes(vTypes) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNECalibrator::~GNECalibrator() {}


void
GNECalibrator::writeAdditional(OutputDevice& device) const {
    // open tag
    device.openTag(SUMO_TAG_CALIBRATOR);
    // write parameters
    device.writeAttr(SUMO_ATTR_ID, getID());
    if (getParentEdges().size() > 0) {
        device.writeAttr(SUMO_ATTR_EDGE, getParentEdges().front()->getID());
    }
    if (getParentLanes().size() > 0) {
        device.writeAttr(SUMO_ATTR_LANE, getParentLanes().front()->getID());
    }
    device.writeAttr(SUMO_ATTR_POSITION, myPositionOverLane);
    if (time2string(myFrequency) != "1.00") {
        device.writeAttr(SUMO_ATTR_FREQUENCY, time2string(myFrequency));
    }
    if (!myAdditionalName.empty()) {
        device.writeAttr(SUMO_ATTR_NAME, myAdditionalName);
    }
    if (!myOutput.empty()) {
        device.writeAttr(SUMO_ATTR_OUTPUT, myOutput);
    }
    if (getParentAdditionals().size() > 0) {
        device.writeAttr(SUMO_ATTR_ROUTEPROBE, getParentAdditionals().front()->getID());
    }
    if (myJamThreshold != 0.5) {
        device.writeAttr(SUMO_ATTR_JAM_DIST_THRESHOLD, myJamThreshold);
    }
    if (myVTypes.size() > 0) {
        device.writeAttr(SUMO_ATTR_VTYPES, myVTypes);
    }
    // write calibrator flows
    for (const auto& calibratorFlow : getChildAdditionals()) {
        if (calibratorFlow->getTagProperty().getTag() == GNE_TAG_FLOW_CALIBRATOR) {
            calibratorFlow->writeAdditional(device);
        }
    }
    // write parameters (Always after children to avoid problems with additionals.xsd)
    writeParams(device);
    device.closeTag();
}


GNEMoveOperation*
GNECalibrator::getMoveOperation() {
    // calibrators cannot be moved
    return nullptr;
}


void
GNECalibrator::updateGeometry() {
    // get shape depending of we have a edge or a lane
    if (getParentLanes().size() > 0) {
        // update geometry
        myAdditionalGeometry.updateGeometry(getParentLanes().front()->getLaneShape(), myPositionOverLane, 0);
    } else if (getParentEdges().size() > 0) {
        // update geometry of first edge
        myAdditionalGeometry.updateGeometry(getParentEdges().front()->getLanes().front()->getLaneShape(), myPositionOverLane, 0);
        // clear extra geometries
        myEdgeCalibratorGeometries.clear();
        // iterate over every lane and get point
        for (int i = 1; i < (int)getParentEdges().front()->getLanes().size(); i++) {
            // add new calibrator geometry
            GUIGeometry calibratorGeometry;
            calibratorGeometry.updateGeometry(getParentEdges().front()->getLanes().at(i)->getLaneShape(), myPositionOverLane, 0);
            myEdgeCalibratorGeometries.push_back(calibratorGeometry);
        }
    } else {
        throw ProcessError("Both edges and lanes aren't defined");
    }
}


Position
GNECalibrator::getPositionInView() const {
    return myAdditionalGeometry.getShape().getPolygonCenter();
}


void
GNECalibrator::updateCenteringBoundary(const bool /*updateGrid*/) {
    myAdditionalBoundary.reset();
    // add center
    myAdditionalBoundary.add(getPositionInView());
    // grow
    myAdditionalBoundary.grow(10);
}


void
GNECalibrator::splitEdgeGeometry(const double splitPosition, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* newElement, GNEUndoList* undoList) {
    if (splitPosition < myPositionOverLane) {
        // change lane or edge
        if (newElement->getTagProperty().getTag() == SUMO_TAG_LANE) {
            setAttribute(SUMO_ATTR_LANE, newElement->getID(), undoList);
        } else {
            setAttribute(SUMO_ATTR_EDGE, newElement->getID(), undoList);
        }
        // now adjust start position
        setAttribute(SUMO_ATTR_POSITION, toString(myPositionOverLane - splitPosition), undoList);
    }
}


std::string
GNECalibrator::getParentName() const {
    // get parent name depending of we have a edge or a lane
    if (getParentLanes().size() > 0) {
        return getParentLanes().front()->getID();
    } else if (getParentEdges().size() > 0) {
        return getParentEdges().front()->getLanes().at(0)->getID();
    } else {
        throw ProcessError("Both myEdge and myLane aren't defined");
    }
}


void
GNECalibrator::drawGL(const GUIVisualizationSettings& s) const {
    // get values
    const double exaggeration = getExaggeration(s);
    // first check if additional has to be drawn
    if (myNet->getViewNet()->getDataViewOptions().showAdditionals()) {
        if (s.drawAdditionals(exaggeration)) {
            // begin push name
            GLHelper::pushName(getGlID());
            // draw first symbol
            drawCalibratorSymbol(s, exaggeration, myAdditionalGeometry.getShape().front(), myAdditionalGeometry.getShapeRotations().front());
            // continue with the other symbols
            for (const auto& edgeCalibratorGeometry : myEdgeCalibratorGeometries) {
                drawCalibratorSymbol(s, exaggeration, edgeCalibratorGeometry.getShape().front(), edgeCalibratorGeometry.getShapeRotations().front());
            }
            // pop name
            GLHelper::popName();
        }
        // draw additional ID
        drawAdditionalID(s);
    }
}


void
GNECalibrator::openAdditionalDialog() {
    // Open calibrator dialog
    GNECalibratorDialog calibratorDialog(this);
}


std::string
GNECalibrator::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getID();
        case SUMO_ATTR_EDGE:
            return getParentEdges().front()->getID();
        case SUMO_ATTR_LANE:
            return getParentLanes().front()->getID();
        case SUMO_ATTR_POSITION:
            return toString(myPositionOverLane);
        case SUMO_ATTR_FREQUENCY:
            return time2string(myFrequency);
        case SUMO_ATTR_NAME:
            return myAdditionalName;
        case SUMO_ATTR_OUTPUT:
            return myOutput;
        case SUMO_ATTR_ROUTEPROBE:
            if (getParentAdditionals().size() > 0) {
                return getParentAdditionals().front()->getID();
            } else {
                return "";
            }
        case SUMO_ATTR_JAM_DIST_THRESHOLD:
            return toString(myJamThreshold);
        case SUMO_ATTR_VTYPES:
            return toString(myVTypes);
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARAMETERS:
            return getParametersStr();
        case GNE_ATTR_SHIFTLANEINDEX:
            return "";
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNECalibrator::getAttributeDouble(SumoXMLAttr key) const {
    throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
}


void
GNECalibrator::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_EDGE:
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_FREQUENCY:
        case SUMO_ATTR_NAME:
        case SUMO_ATTR_OUTPUT:
        case SUMO_ATTR_ROUTEPROBE:
        case SUMO_ATTR_JAM_DIST_THRESHOLD:
        case SUMO_ATTR_VTYPES:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
        case GNE_ATTR_SHIFTLANEINDEX:
            undoList->changeAttribute(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }

}


bool
GNECalibrator::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_EDGE:
            if (myNet->getAttributeCarriers()->retrieveEdge(value, false) != nullptr) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_LANE:
            if (myNet->getAttributeCarriers()->retrieveLane(value, false) != nullptr) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_POSITION:
            if (canParse<double>(value)) {
                // obtain position and check if is valid
                double newPosition = parse<double>(value);
                PositionVector shape = (getParentLanes().size() > 0) ? getParentLanes().front()->getLaneShape() : getParentEdges().front()->getLanes().at(0)->getLaneShape();
                if ((newPosition < 0) || (newPosition > shape.length())) {
                    return false;
                } else {
                    return true;
                }
            } else {
                return false;
            }
        case SUMO_ATTR_FREQUENCY:
            return canParse<SUMOTime>(value);
        case SUMO_ATTR_NAME:
            return SUMOXMLDefinitions::isValidAttribute(value);
        case SUMO_ATTR_OUTPUT:
            return SUMOXMLDefinitions::isValidFilename(value);
        case SUMO_ATTR_ROUTEPROBE:
            if (value.empty()) {
                return true;
            } else {
                return (myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_ROUTEPROBE, value, false) != nullptr);
            }
        case SUMO_ATTR_JAM_DIST_THRESHOLD:
            return canParse<double>(value) ? (parse<double>(value) >= 0) : false;
        case SUMO_ATTR_VTYPES:
            if (value.empty()) {
                return true;
            } else {
                return SUMOXMLDefinitions::isValidListOfTypeID(value);
            }
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return Parameterised::areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNECalibrator::isAttributeEnabled(SumoXMLAttr /* key */) const {
    return true;
}


std::string
GNECalibrator::getPopUpID() const {
    return getTagStr() + ": " + getID();
}


std::string
GNECalibrator::getHierarchyName() const {
    return getTagStr();
}

// ===========================================================================
// private
// ===========================================================================

void GNECalibrator::drawCalibratorSymbol(const GUIVisualizationSettings& s, const double exaggeration, const Position& pos, const double rot) const {
    // draw parent and child lines
    drawParentChildLines(s, s.additionalSettings.connectionColor);
    // push layer matrix
    GLHelper::pushMatrix();
    // translate to front
    myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, GLO_CALIBRATOR);
    // translate to position
    glTranslated(pos.x(), pos.y(), 0);
    // rotate over lane
    GUIGeometry::rotateOverLane(rot - 90);
    // scale
    glScaled(exaggeration, exaggeration, 1);
    // set drawing mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // set color
    if (drawUsingSelectColor()) {
        GLHelper::setColor(s.colorSettings.selectedAdditionalColor);
    } else {
        GLHelper::setColor(s.additionalSettings.calibratorColor);
    }
    // base
    glBegin(GL_TRIANGLES);
    glVertex2d(0 - s.additionalSettings.calibratorWidth, 0);
    glVertex2d(0 - s.additionalSettings.calibratorWidth, s.additionalSettings.calibratorHeight);
    glVertex2d(0 + s.additionalSettings.calibratorWidth, s.additionalSettings.calibratorHeight);
    glVertex2d(0 + s.additionalSettings.calibratorWidth, 0);
    glVertex2d(0 - s.additionalSettings.calibratorWidth, 0);
    glVertex2d(0 + s.additionalSettings.calibratorWidth, s.additionalSettings.calibratorHeight);
    glEnd();
    // draw text if isn't being drawn for selecting
    if (!s.drawForRectangleSelection && !s.drawForPositionSelection && s.drawDetail(s.detailSettings.calibratorText, exaggeration)) {
        // set color depending of selection status
        RGBColor textColor = drawUsingSelectColor() ? s.colorSettings.selectionColor : RGBColor::BLACK;
        // draw "C"
        GLHelper::drawText("C", Position(0, 1.5), 0.1, 3, textColor, 180);
        // draw "edge" or "lane "
        if (getParentLanes().size() > 0) {
            GLHelper::drawText("lane", Position(0, 3), .1, 1, textColor, 180);
        } else if (getParentEdges().size() > 0) {
            GLHelper::drawText("edge", Position(0, 3), .1, 1, textColor, 180);
        } else {
            throw ProcessError("Both myEdge and myLane aren't defined");
        }
    }
    // pop layer matrix
    GLHelper::popMatrix();
    // check if dotted contours has to be drawn
    if (myNet->getViewNet()->isAttributeCarrierInspected(this)) {
        GUIDottedGeometry::drawDottedSquaredShape(GUIDottedGeometry::DottedContourType::INSPECT, s, pos,
                s.additionalSettings.calibratorWidth, s.additionalSettings.calibratorHeight * 0.5,
                0, s.additionalSettings.calibratorHeight * -0.5,
                rot + 90, exaggeration);
    }
    if (myNet->getViewNet()->getFrontAttributeCarrier() == this) {
        GUIDottedGeometry::drawDottedSquaredShape(GUIDottedGeometry::DottedContourType::FRONT, s, pos,
                s.additionalSettings.calibratorWidth, s.additionalSettings.calibratorHeight * 0.5,
                0, s.additionalSettings.calibratorHeight * -0.5,
                rot + 90, exaggeration);
    }
}

void
GNECalibrator::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            // update microsimID
            setMicrosimID(value);
            break;
        case SUMO_ATTR_EDGE:
            replaceAdditionalParentEdges(value);
            break;
        case SUMO_ATTR_LANE:
            replaceAdditionalParentLanes(value);
            break;
        case SUMO_ATTR_POSITION:
            myPositionOverLane = parse<double>(value);
            break;
        case SUMO_ATTR_FREQUENCY:
            myFrequency = parse<SUMOTime>(value);
            break;
        case SUMO_ATTR_NAME:
            myAdditionalName = value;
            break;
        case SUMO_ATTR_OUTPUT:
            myOutput = value;
            break;
        case SUMO_ATTR_ROUTEPROBE:
            replaceAdditionalParent(SUMO_TAG_ROUTEPROBE, value, 0);
            break;
        case SUMO_ATTR_JAM_DIST_THRESHOLD:
            myJamThreshold = parse<double>(value);
            break;
        case SUMO_ATTR_VTYPES:
            myVTypes = parse<std::vector<std::string> >(value);
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
        case GNE_ATTR_SHIFTLANEINDEX:
            shiftLaneIndex();
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNECalibrator::setMoveShape(const GNEMoveResult& /*moveResult*/) {
    // nothing to do
}


void
GNECalibrator::commitMoveShape(const GNEMoveResult& /*moveResult*/, GNEUndoList* /*undoList*/) {
    // nothing to do
}


/****************************************************************************/
