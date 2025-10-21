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
/// @file    GNECalibrator.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
// Calibrator over edge or lane
/****************************************************************************/

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/dialogs/elements/GNECalibratorDialog.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/xml/NamespaceIDs.h>

#include "GNECalibrator.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNECalibrator::GNECalibrator(SumoXMLTag tag, GNENet* net) :
    GNEAdditional("", net, "", tag, ""),
    myEdgeCalibratorContours(new std::vector<GNEContour*>()) {
}


GNECalibrator::GNECalibrator(const std::string& id, GNENet* net, const std::string& filename, GNEEdge* edge, double pos, SUMOTime frequency, const std::string& name,
                             const std::string& output, const double jamThreshold, const std::vector<std::string>& vTypes, const Parameterised::Map& parameters) :
    GNEAdditional(id, net, filename, SUMO_TAG_CALIBRATOR, name),
    Parameterised(parameters),
    myPositionOverLane(pos),
    myFrequency(frequency),
    myOutput(output),
    myJamThreshold(jamThreshold),
    myVTypes(vTypes),
    myEdgeCalibratorContours(new std::vector<GNEContour*>()) {
    // set parents
    setParent<GNEEdge*>(edge);
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNECalibrator::GNECalibrator(const std::string& id, GNENet* net, const std::string& filename, GNEEdge* edge, double pos, SUMOTime frequency, const std::string& name,
                             const std::string& output, GNEAdditional* routeProbe, const double jamThreshold, const std::vector<std::string>& vTypes,
                             const Parameterised::Map& parameters) :
    GNEAdditional(id, net, filename, SUMO_TAG_CALIBRATOR, name),
    Parameterised(parameters),
    myPositionOverLane(pos),
    myFrequency(frequency),
    myOutput(output),
    myJamThreshold(jamThreshold),
    myVTypes(vTypes),
    myEdgeCalibratorContours(new std::vector<GNEContour*>()) {
    // set parents
    setParent<GNEEdge*>(edge);
    setParent<GNEAdditional*>(routeProbe);
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNECalibrator::GNECalibrator(const std::string& id, GNENet* net, const std::string& filename, GNELane* lane, double pos, SUMOTime frequency, const std::string& name,
                             const std::string& output, const double jamThreshold, const std::vector<std::string>& vTypes, const Parameterised::Map& parameters) :
    GNEAdditional(id, net, filename, GNE_TAG_CALIBRATOR_LANE, name),
    Parameterised(parameters),
    myPositionOverLane(pos),
    myFrequency(frequency),
    myOutput(output),
    myJamThreshold(jamThreshold),
    myVTypes(vTypes),
    myEdgeCalibratorContours(new std::vector<GNEContour*>()) {
    // set parents
    setParent<GNELane*>(lane);
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNECalibrator::GNECalibrator(const std::string& id, GNENet* net, const std::string& filename, GNELane* lane, double pos, SUMOTime frequency, const std::string& name,
                             const std::string& output, GNEAdditional* routeProbe, const double jamThreshold, const std::vector<std::string>& vTypes,
                             const Parameterised::Map& parameters) :
    GNEAdditional(id, net, filename, GNE_TAG_CALIBRATOR_LANE, name),
    Parameterised(parameters),
    myPositionOverLane(pos),
    myFrequency(frequency),
    myOutput(output),
    myJamThreshold(jamThreshold),
    myVTypes(vTypes),
    myEdgeCalibratorContours(new std::vector<GNEContour*>()) {
    // set parents
    setParent<GNELane*>(lane);
    setParent<GNEAdditional*>(routeProbe);
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNECalibrator::~GNECalibrator() {
    for (auto it = myEdgeCalibratorContours->begin(); it != myEdgeCalibratorContours->end(); it++) {
        delete *it;
    }
    delete myEdgeCalibratorContours;
}


GNEMoveElement*
GNECalibrator::getMoveElement() const {
    return nullptr;
}


Parameterised*
GNECalibrator::getParameters() {
    return this;
}


const Parameterised*
GNECalibrator::getParameters() const {
    return this;
}


void
GNECalibrator::writeAdditional(OutputDevice& device) const {
    // open tag
    device.openTag(SUMO_TAG_CALIBRATOR);
    // write common additional attributes
    writeAdditionalAttributes(device);
    // write specific attributes
    if (getParentEdges().size() > 0) {
        device.writeAttr(SUMO_ATTR_EDGE, getParentEdges().front()->getID());
    }
    if (getParentLanes().size() > 0) {
        device.writeAttr(SUMO_ATTR_LANE, getParentLanes().front()->getID());
    }
    device.writeAttr(SUMO_ATTR_POSITION, myPositionOverLane);
    if (time2string(myFrequency) != "1.00") {
        device.writeAttr(SUMO_ATTR_PERIOD, time2string(myFrequency));
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
        if (calibratorFlow->getTagProperty()->getTag() == GNE_TAG_CALIBRATOR_FLOW) {
            calibratorFlow->writeAdditional(device);
        }
    }
    // write parameters (Always after children to avoid problems with additionals.xsd)
    writeParams(device);
    device.closeTag();
}


bool
GNECalibrator::isAdditionalValid() const {
    return true;
}


std::string
GNECalibrator::getAdditionalProblem() const {
    return "";
}


void
GNECalibrator::fixAdditionalProblem() {
    // nothing to fix
}


void
GNECalibrator::updateGeometry() {
    // get shape depending of we have a edge or a lane
    if (getParentLanes().size() > 0) {
        // simply update geometry
        myAdditionalGeometry.updateGeometry(getParentLanes().front()->getLaneShape(), myPositionOverLane, 0);
    } else if (getParentEdges().size() > 0) {
        // clear all contours
        for (auto it = myEdgeCalibratorContours->begin(); it != myEdgeCalibratorContours->end(); it++) {
            delete *it;
        }
        // clear all edge geometries
        myEdgeCalibratorGeometries.clear();
        myEdgeCalibratorContours->clear();
        // iterate over every lane and upadte geometries
        for (const auto& lane : getParentEdges().front()->getChildLanes()) {
            // this is needed for centering calibratorFlows as additional listed
            if (lane == getParentEdges().front()->getChildLanes().front()) {
                myAdditionalGeometry.updateGeometry(lane->getLaneShape(), myPositionOverLane, 0);
            }
            // add new calibrator geometry
            GUIGeometry calibratorGeometry;
            calibratorGeometry.updateGeometry(lane->getLaneShape(), myPositionOverLane, 0);
            myEdgeCalibratorGeometries.push_back(calibratorGeometry);
            // also add a new contour
            myEdgeCalibratorContours->push_back(new GNEContour());
        }
    }
    // update geometries of all children
    for (const auto& rerouterElement : getChildAdditionals()) {
        rerouterElement->updateGeometry();
    }
}


Position
GNECalibrator::getPositionInView() const {
    return myAdditionalGeometry.getShape().getPolygonCenter();
}


void
GNECalibrator::updateCenteringBoundary(const bool /*updateGrid*/) {
    // nothing to update
}


void
GNECalibrator::splitEdgeGeometry(const double splitPosition, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* newElement, GNEUndoList* undoList) {
    if (splitPosition < myPositionOverLane) {
        // change lane or edge
        if (newElement->getTagProperty()->getTag() == SUMO_TAG_LANE) {
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
        return getParentEdges().front()->getChildLanes().at(0)->getID();
    } else {
        throw ProcessError(TL("Both myEdge and myLane aren't defined"));
    }
}


void
GNECalibrator::drawGL(const GUIVisualizationSettings& s) const {
    const auto& inspectedElements = myNet->getViewNet()->getInspectedElements();
    // first check if additional has to be drawn
    if (myNet->getViewNet()->getDataViewOptions().showAdditionals()) {
        // get values
        const double exaggeration = getExaggeration(s);
        // get detail level
        const auto d = s.getDetailLevel(exaggeration);
        if (myEdgeCalibratorGeometries.size() > 0) {
            // draw all calibrator symbols
            for (int i = 0; i < (int)myEdgeCalibratorGeometries.size(); i++) {
                drawCalibratorSymbol(s, d, exaggeration, myEdgeCalibratorGeometries.at(i).getShape().front(),
                                     myEdgeCalibratorGeometries.at(i).getShapeRotations().front(), i);
            }
        } else {
            // draw single calibrator symbol
            drawCalibratorSymbol(s, d, exaggeration, myAdditionalGeometry.getShape().front(),
                                 myAdditionalGeometry.getShapeRotations().front(), -1);
        }
        // draw additional ID
        drawAdditionalID(s);
        // iterate over additionals and check if drawn
        for (const auto& calibratorFlow : getChildAdditionals()) {
            // if calibrator is being inspected or selected, then draw
            if (myNet->getViewNet()->getNetworkViewOptions().showSubAdditionals() ||
                    isAttributeCarrierSelected() || inspectedElements.isACInspected(this) ||
                    calibratorFlow->isAttributeCarrierSelected() || inspectedElements.isACInspected(calibratorFlow) ||
                    calibratorFlow->isMarkedForDrawingFront()) {
                calibratorFlow->drawGL(s);
            }
        }
    }
}


bool
GNECalibrator::checkDrawMoveContour() const {
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
GNECalibrator::openAdditionalDialog() {
    // Open calibrator dialog
    GNECalibratorDialog calibratorDialog(this);
}


std::string
GNECalibrator::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_EDGE:
            return getParentEdges().front()->getID();
        case SUMO_ATTR_LANE:
            return getParentLanes().front()->getID();
        case SUMO_ATTR_POSITION:
            return toString(myPositionOverLane);
        case SUMO_ATTR_PERIOD:
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
        case GNE_ATTR_SHIFTLANEINDEX:
            return "";
        default:
            return getCommonAttribute(key);
    }
}


double
GNECalibrator::getAttributeDouble(SumoXMLAttr key) const {
    return getCommonAttributeDouble(key);
}


Position
GNECalibrator::getAttributePosition(SumoXMLAttr key) const {
    return getCommonAttributePosition(key);
}


PositionVector
GNECalibrator::getAttributePositionVector(SumoXMLAttr key) const {
    return getCommonAttributePositionVector(key);
}


void
GNECalibrator::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_EDGE:
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_PERIOD:
        case SUMO_ATTR_FREQUENCY:
        case SUMO_ATTR_NAME:
        case SUMO_ATTR_OUTPUT:
        case SUMO_ATTR_ROUTEPROBE:
        case SUMO_ATTR_JAM_DIST_THRESHOLD:
        case SUMO_ATTR_VTYPES:
        case GNE_ATTR_SHIFTLANEINDEX:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            setCommonAttribute(key, value, undoList);
            break;
    }

}


bool
GNECalibrator::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(NamespaceIDs::calibrators, value);
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
                const double newPosition = parse<double>(value);
                if (isTemplate()) {
                    return (newPosition >= 0);
                }
                // get shape
                PositionVector shape = (getParentLanes().size() > 0) ? getParentLanes().front()->getLaneShape() : getParentEdges().front()->getChildLanes().at(0)->getLaneShape();
                if ((newPosition < 0) || (newPosition > shape.length())) {
                    return false;
                } else {
                    return true;
                }
            } else {
                return false;
            }
        case SUMO_ATTR_PERIOD:
        case SUMO_ATTR_FREQUENCY:
            return canParse<SUMOTime>(value) ? (parse<SUMOTime>(value) >= 0) : false;
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
        default:
            return isCommonAttributeValid(key, value);
    }
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

void
GNECalibrator::drawCalibratorSymbol(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d, const double exaggeration,
                                    const Position& pos, const double rot, const int symbolIndex) const {
    // draw geometry only if we'rent in drawForObjectUnderCursor mode
    if (s.checkDrawAdditional(d, isAttributeCarrierSelected())) {
        // push layer matrix
        GLHelper::pushMatrix();
        // translate to front
        drawInLayer(GLO_CALIBRATOR);
        // translate to position
        glTranslated(pos.x(), pos.y(), 0);
        // rotate over lane
        GUIGeometry::rotateOverLane(rot + 90);
        // scale
        glScaled(exaggeration, exaggeration, 1);
        // set drawing mode
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        // set color
        GLHelper::setColor(drawUsingSelectColor() ? s.colorSettings.selectedAdditionalColor : s.additionalSettings.calibratorColor);
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
        if (d <= GUIVisualizationSettings::Detail::Text) {
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
                throw ProcessError(TL("Both myEdge and myLane aren't defined"));
            }
        }
        // pop layer matrix
        GLHelper::popMatrix();
        // draw dotted contours
        if (symbolIndex == -1) {
            myAdditionalContour.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidth, true);
        } else {
            myEdgeCalibratorContours->at(symbolIndex)->drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidth, true);
        }
    }
    GUIGlObject* parentBoundary = nullptr;
    if (getParentEdges().size() > 0) {
        parentBoundary = getParentEdges().front();
    } else if (getParentLanes().size() > 0) {
        parentBoundary = getParentLanes().front();
    }
    // calculate dotted contour
    if (symbolIndex == -1) {
        myAdditionalContour.calculateContourRectangleShape(s, d, this, pos, s.additionalSettings.calibratorWidth,
                s.additionalSettings.calibratorHeight * 0.5, getType(), 0, s.additionalSettings.calibratorHeight * 0.5, rot + 90,
                exaggeration, parentBoundary);
    } else {
        if (symbolIndex == 0) {
            myAdditionalContour.calculateContourRectangleShape(s, d, this, pos, s.additionalSettings.calibratorWidth,
                    s.additionalSettings.calibratorHeight * 0.5, getType(), 0, s.additionalSettings.calibratorHeight * 0.5, rot + 90,
                    exaggeration, parentBoundary);
        }
        myEdgeCalibratorContours->at(symbolIndex)->calculateContourRectangleShape(s, d, this, pos, s.additionalSettings.calibratorWidth,
                s.additionalSettings.calibratorHeight * 0.5, getType(), 0, s.additionalSettings.calibratorHeight * 0.5, rot + 90,
                exaggeration, parentBoundary);
    }
}

void
GNECalibrator::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            // update microsimID
            setAdditionalID(value);
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
        case SUMO_ATTR_PERIOD:
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
        case GNE_ATTR_SHIFTLANEINDEX:
            shiftLaneIndex();
            break;
        default:
            setCommonAttribute(key, value);
            break;
    }
}

/****************************************************************************/
