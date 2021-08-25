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
/// @file    GNEAdditional.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
///
// A abstract class for representation of additional elements
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/options/OptionsCont.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEAdditional.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEAdditional::GNEAdditional(const std::string& id, GNENet* net, GUIGlObjectType type, SumoXMLTag tag, std::string additionalName,
                             const std::vector<GNEJunction*>& junctionParents,
                             const std::vector<GNEEdge*>& edgeParents,
                             const std::vector<GNELane*>& laneParents,
                             const std::vector<GNEAdditional*>& additionalParents,
                             const std::vector<GNEShape*>& shapeParents,
                             const std::vector<GNETAZElement*>& TAZElementParents,
                             const std::vector<GNEDemandElement*>& demandElementParents,
                             const std::vector<GNEGenericData*>& genericDataParents,
                             const std::map<std::string, std::string>& parameters) :
    GUIGlObject(type, id),
    GNEHierarchicalElement(net, tag, junctionParents, edgeParents, laneParents, additionalParents, shapeParents, TAZElementParents, demandElementParents, genericDataParents),
    GNEPathManager::PathElement(GNEPathManager::PathElement::Options::ADDITIONAL_ELEMENT),
    Parameterised(parameters),
    myAdditionalName(additionalName) {
}


GNEAdditional::GNEAdditional(GNENet* net, GUIGlObjectType type, SumoXMLTag tag, std::string additionalName,
                             const std::vector<GNEJunction*>& junctionParents,
                             const std::vector<GNEEdge*>& edgeParents,
                             const std::vector<GNELane*>& laneParents,
                             const std::vector<GNEAdditional*>& additionalParents,
                             const std::vector<GNEShape*>& shapeParents,
                             const std::vector<GNETAZElement*>& TAZElementParents,
                             const std::vector<GNEDemandElement*>& demandElementParents,
                             const std::vector<GNEGenericData*>& genericDataParents,
                             const std::map<std::string, std::string>& parameters) :
    GUIGlObject(type, additionalParents.front()->getID()),
    GNEHierarchicalElement(net, tag, junctionParents, edgeParents, laneParents, additionalParents, shapeParents, TAZElementParents, demandElementParents, genericDataParents),
    GNEPathManager::PathElement(GNEPathManager::PathElement::Options::ADDITIONAL_ELEMENT),
    Parameterised(parameters),
    myAdditionalName(additionalName) {
}


GNEAdditional::~GNEAdditional() {}


void
GNEAdditional::removeGeometryPoint(const Position /*clickedPosition*/, GNEUndoList* /*undoList*/) {
    // currently there isn't additionals with removable geometry points
}


const std::string&
GNEAdditional::getID() const {
    return getMicrosimID();
}


GUIGlObject*
GNEAdditional::getGUIGlObject() {
    return this;
}


const GNEGeometry::Geometry&
GNEAdditional::getAdditionalGeometry() const {
    return myAdditionalGeometry;
}


void
GNEAdditional::setSpecialColor(const RGBColor* color) {
    mySpecialColor = color;
}


void
GNEAdditional::writeAdditional(OutputDevice& device) const {
    // first check if minimum number of children is correct
    if ((myTagProperty.hasMinimumNumberOfChildren() || myTagProperty.hasMinimumNumberOfChildren()) && !checkChildAdditionalRestriction()) {
        WRITE_WARNING(getTagStr() + " with ID='" + getID() + "' cannot be written");
    } else {
        // Open XML Tag or synonym Tag
        device.openTag(myTagProperty.getXMLTag());
        // iterate over attribute properties
        for (const auto& attrProperty : myTagProperty) {
            // obtain attribute value
            const std::string attributeValue = getAttribute(attrProperty.getAttr());
            //  first check if attribute is optional but not vehicle classes
            if (attrProperty.isOptional() && !attrProperty.isVClasses()) {
                // Only write attributes with default value if is different from original
                if (attrProperty.getDefaultValue() != attributeValue) {
                    // check if attribute must be written using a synonim
                    if (attrProperty.hasAttrSynonym()) {
                        device.writeAttr(attrProperty.getAttrSynonym(), attributeValue);
                    } else {
                        // SVC permissions uses their own writting function
                        if (attrProperty.isSVCPermission()) {
                            // disallow attribute musn't be written
                            if (attrProperty.getAttr() != SUMO_ATTR_DISALLOW) {
                                writePermissions(device, parseVehicleClasses(attributeValue));
                            }
                        } else if (myTagProperty.canMaskXYZPositions() && (attrProperty.getAttr() == SUMO_ATTR_POSITION)) {
                            // get position attribute and write it separate
                            Position pos = parse<Position>(getAttribute(SUMO_ATTR_POSITION));
                            device.writeAttr(SUMO_ATTR_X, toString(pos.x()));
                            device.writeAttr(SUMO_ATTR_Y, toString(pos.y()));
                            // write 0 only if is different from 0 (the default value)
                            if (pos.z() != 0) {
                                device.writeAttr(SUMO_ATTR_Z, toString(pos.z()));
                            }
                        } else {
                            device.writeAttr(attrProperty.getAttr(), attributeValue);
                        }
                    }
                }
            } else {
                // Attributes without default values are always writted
                if (attrProperty.hasAttrSynonym()) {
                    device.writeAttr(attrProperty.getAttrSynonym(), attributeValue);
                } else {
                    // SVC permissions uses their own writting function
                    if (attrProperty.isSVCPermission()) {
                        // disallow attribute musn't be written
                        if (attrProperty.getAttr() != SUMO_ATTR_DISALLOW) {
                            writePermissions(device, parseVehicleClasses(attributeValue));
                        }
                    } else if (myTagProperty.canMaskXYZPositions() && (attrProperty.getAttr() == SUMO_ATTR_POSITION)) {
                        // get position attribute and write it separate
                        Position pos = parse<Position>(getAttribute(SUMO_ATTR_POSITION));
                        device.writeAttr(SUMO_ATTR_X, toString(pos.x()));
                        device.writeAttr(SUMO_ATTR_Y, toString(pos.y()));
                        // write 0 only if is different from 0 (the default value)
                        if (pos.z() != 0) {
                            device.writeAttr(SUMO_ATTR_Z, toString(pos.z()));
                        }
                    } else {
                        device.writeAttr(attrProperty.getAttr(), attributeValue);
                    }
                }
            }
        }
        // iterate over children and write it in XML (or in a different file)
        if (myTagProperty.canWriteChildrenSeparate() && myTagProperty.hasAttribute(SUMO_ATTR_FILE) && !getAttribute(SUMO_ATTR_FILE).empty()) {
            // we assume that rerouter values files is placed in the same folder as the additional file
            OutputDevice& deviceChildren = OutputDevice::getDevice(FileHelpers::getFilePath(OptionsCont::getOptions().getString("additional-files")) + getAttribute(SUMO_ATTR_FILE));
            deviceChildren.writeXMLHeader("rerouterValue", "additional_file.xsd");
            // save children in a different filename
            for (const auto& additionalChild : getChildAdditionals()) {
                // avoid to write two times additionals that haben two parents (Only write as child of first parent)
                if (additionalChild->getParentAdditionals().size() < 1) {
                    additionalChild->writeAdditional(deviceChildren);
                } else if (myTagProperty.getTag() == additionalChild->getTagProperty().getMasterTags().front()) {
                    additionalChild->writeAdditional(deviceChildren);
                }
            }
            deviceChildren.close();
        } else {
            for (const auto& additionalChild : getChildAdditionals()) {
                // avoid write symbols
                if (!additionalChild->getTagProperty().isSymbol()) {
                    // avoid to write two times additionals that haben two parents (Only write as child of first parent)
                    if (additionalChild->getParentAdditionals().size() < 2) {
                        additionalChild->writeAdditional(device);
                    } else if (myTagProperty.getTag() == additionalChild->getTagProperty().getMasterTags().front()) {
                        additionalChild->writeAdditional(device);
                    }
                }
            }
        }
        // write parameters (Always after children to avoid problems with additionals.xsd)
        writeParams(device);
        // Close tag
        device.closeTag();
    }
}


bool
GNEAdditional::isAdditionalValid() const {
    return true;
}


std::string
GNEAdditional::getAdditionalProblem() const {
    return "";
}


void
GNEAdditional::fixAdditionalProblem() {
    throw InvalidArgument(getTagStr() + " cannot fix any problem");
}


void
GNEAdditional::openAdditionalDialog() {
    throw InvalidArgument(getTagStr() + " doesn't have an additional dialog");
}


Boundary
GNEAdditional::getCenteringBoundary() const {
    return myBoundary;
}


GUIGLObjectPopupMenu*
GNEAdditional::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    // build header
    buildPopupHeader(ret, app);
    // build menu command for center button and copy cursor position to clipboard
    buildCenterPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
    // buld menu commands for names
    GUIDesigns::buildFXMenuCommand(ret, "Copy " + getTagStr() + " name to clipboard", nullptr, ret, MID_COPY_NAME);
    GUIDesigns::buildFXMenuCommand(ret, "Copy " + getTagStr() + " typed name to clipboard", nullptr, ret, MID_COPY_TYPED_NAME);
    new FXMenuSeparator(ret);
    // build selection and show parameters menu
    myNet->getViewNet()->buildSelectionACPopupEntry(ret, this);
    buildShowParamsPopupEntry(ret);
    // show option to open additional dialog
    if (myTagProperty.hasDialog()) {
        GUIDesigns::buildFXMenuCommand(ret, "Open " + getTagStr() + " Dialog", getIcon(), &parent, MID_OPEN_ADDITIONAL_DIALOG);
        new FXMenuSeparator(ret);
    }
    // Show position parameters
    if (myTagProperty.hasAttribute(SUMO_ATTR_LANE) && (myAdditionalGeometry.getShape().size() > 1)) {
        const GNELane* lane = myNet->retrieveLane(getAttribute(SUMO_ATTR_LANE));
        // Show menu command inner position
        const double innerPos = myAdditionalGeometry.getShape().nearest_offset_to_point2D(parent.getPositionInformation());
        GUIDesigns::buildFXMenuCommand(ret, "Cursor position over additional shape: " + toString(innerPos), nullptr, nullptr, 0);
        // If shape isn't empty, show menu command lane position
        if (myAdditionalGeometry.getShape().size() > 0) {
            const double lanePos = lane->getLaneShape().nearest_offset_to_point2D(myAdditionalGeometry.getShape().front());
            GUIDesigns::buildFXMenuCommand(ret, "Cursor position over " + toString(SUMO_TAG_LANE) + ": " + toString(innerPos + lanePos), nullptr, nullptr, 0);
        }
    } else if (myTagProperty.hasAttribute(SUMO_ATTR_EDGE) && (myAdditionalGeometry.getShape().size() > 1)) {
        const GNEEdge* edge = myNet->retrieveEdge(getAttribute(SUMO_ATTR_EDGE));
        // Show menu command inner position
        const double innerPos = myAdditionalGeometry.getShape().nearest_offset_to_point2D(parent.getPositionInformation());
        GUIDesigns::buildFXMenuCommand(ret, "Cursor position over additional shape: " + toString(innerPos), nullptr, nullptr, 0);
        // If shape isn't empty, show menu command edge position
        if (myAdditionalGeometry.getShape().size() > 0) {
            const double edgePos = edge->getLanes().at(0)->getLaneShape().nearest_offset_to_point2D(myAdditionalGeometry.getShape().front());
            GUIDesigns::buildFXMenuCommand(ret, "Mouse position over " + toString(SUMO_TAG_EDGE) + ": " + toString(innerPos + edgePos), nullptr, nullptr, 0);
        }
    } else {
        GUIDesigns::buildFXMenuCommand(ret, "Cursor position in view: " + toString(getPositionInView().x()) + "," + toString(getPositionInView().y()), nullptr, nullptr, 0);
    }
    return ret;
}


GUIParameterTableWindow*
GNEAdditional::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView&) {
    // Create table
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this);
    // Iterate over attributes
    for (const auto& i : myTagProperty) {
        // Add attribute and set it dynamic if aren't unique
        if (i.isUnique()) {
            ret->mkItem(i.getAttrStr().c_str(), false, getAttribute(i.getAttr()));
        } else {
            ret->mkItem(i.getAttrStr().c_str(), true, getAttribute(i.getAttr()));
        }
    }
    // close building
    ret->closeBuilding();
    return ret;
}


const std::string&
GNEAdditional::getOptionalAdditionalName() const {
    return myAdditionalName;
}


void
GNEAdditional::computePathElement() {
    // currently onle for E2 multilane detectors
    if (myTagProperty.getTag() == GNE_TAG_E2DETECTOR_MULTILANE) {
        // calculate path
        myNet->getPathManager()->calculateConsecutivePathLanes(this, getParentLanes());
    }
}


void
GNEAdditional::drawPartialGL(const GUIVisualizationSettings& s, const GNELane* lane, const GNEPathManager::Segment* segment, const double offsetFront) const {
    // calculate E2Detector width
    const double E2DetectorWidth = s.addSize.getExaggeration(s, lane);
    // check if E2 can be drawn
    if (s.drawAdditionals(E2DetectorWidth) && myNet->getViewNet()->getDataViewOptions().showAdditionals()) {
        // calculate startPos
        const double geometryDepartPos = getAttributeDouble(SUMO_ATTR_POSITION) + getAttributeDouble(SUMO_ATTR_ENDPOS);
        // get endPos
        const double geometryEndPos = getAttributeDouble(SUMO_ATTR_ENDPOS);
        // declare path geometry
        GNEGeometry::Geometry E2Geometry;
        // update pathGeometry depending of first and last segment
        if (segment->isFirstSegment() && segment->isLastSegment()) {
            E2Geometry.updateGeometry(lane->getLaneGeometry().getShape(),
                                      geometryDepartPos, geometryEndPos,      // extrem positions
                                      Position::INVALID, Position::INVALID);  // extra positions
        } else if (segment->isFirstSegment()) {
            E2Geometry.updateGeometry(lane->getLaneGeometry().getShape(),
                                      geometryDepartPos, -1,                  // extrem positions
                                      Position::INVALID, Position::INVALID);  // extra positions
        } else if (segment->isLastSegment()) {
            E2Geometry.updateGeometry(lane->getLaneGeometry().getShape(),
                                      -1, geometryEndPos,                     // extrem positions
                                      Position::INVALID, Position::INVALID);  // extra positions
        } else {
            E2Geometry = lane->getLaneGeometry();
        }
        // obtain color
        const RGBColor E2Color = drawUsingSelectColor() ? s.colorSettings.selectedAdditionalColor : s.detectorSettings.E2Color;
        // Start drawing adding an gl identificator
        GLHelper::pushName(getGlID());
        // Add a draw matrix
        GLHelper::pushMatrix();
        // Start with the drawing of the area traslating matrix to origin
        glTranslated(0, 0, getType() + offsetFront);
        // Set color
        GLHelper::setColor(E2Color);
        // draw geometry
        GNEGeometry::drawGeometry(myNet->getViewNet(), E2Geometry, E2DetectorWidth);
        // Pop last matrix
        GLHelper::popMatrix();
        // Pop name
        GLHelper::popName();
        // draw additional ID
        if (!s.drawForRectangleSelection) {
            drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
            // check if this is the label segment
            if (segment->isLabelSegment()) {
                // calculate middle point
                const double middlePoint = (E2Geometry.getShape().length2D() * 0.5);
                // calculate position
                const Position pos = E2Geometry.getShape().positionAtOffset2D(middlePoint);
                // calculate rotation
                const double rot = E2Geometry.getShape().rotationDegreeAtOffset(middlePoint);
                // Start pushing matrix
                GLHelper::pushMatrix();
                // Traslate to position
                glTranslated(pos.x(), pos.y(), getType() + offsetFront + 0.1);
                // rotate over lane
                GNEGeometry::rotateOverLane(rot);
                // move
                glTranslated(-1, 0, 0);
                // scale text
                glScaled(E2DetectorWidth, E2DetectorWidth, 1);
                // draw E1 logo
                GLHelper::drawText("E2 Multilane", Position(), .1, 1.5, RGBColor::BLACK);
                // pop matrix
                GLHelper::popMatrix();
            }
        }
        // check if shape dotted contour has to be drawn
        if (s.drawDottedContour() || myNet->getViewNet()->isAttributeCarrierInspected(this)) {
            // declare trim geometry to draw
            const GNEGeometry::DottedGeometry pathDottedGeometry((segment->isFirstSegment() || segment->isLastSegment()) ? GNEGeometry::DottedGeometry(s, E2Geometry.getShape(), false) : lane->getDottedLaneGeometry());
            // draw inspected dotted contour
            if (s.drawDottedContour() || myNet->getViewNet()->isAttributeCarrierInspected(this)) {
                GNEGeometry::drawDottedContourGeometry(GNEGeometry::DottedContourType::INSPECT, s, pathDottedGeometry, E2DetectorWidth, segment->isFirstSegment(), segment->isLastSegment());
            }
            // draw front dotted contour
            if (s.drawDottedContour() || (myNet->getViewNet()->getFrontAttributeCarrier() == this)) {
                GNEGeometry::drawDottedContourGeometry(GNEGeometry::DottedContourType::FRONT, s, pathDottedGeometry, E2DetectorWidth, segment->isFirstSegment(), segment->isLastSegment());
            }
        }
    }
}


void
GNEAdditional::drawPartialGL(const GUIVisualizationSettings& s, const GNELane* fromLane, const GNELane* toLane, const GNEPathManager::Segment* /*segment*/, const double offsetFront) const {
    // calculate E2Detector width
    const double E2DetectorWidth = s.addSize.getExaggeration(s, fromLane);
    // check if E2 can be drawn
    if (s.drawAdditionals(E2DetectorWidth) && myNet->getViewNet()->getDataViewOptions().showAdditionals()) {
        // get flag for show only contour
        const bool onlyContour = myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork() ? myNet->getViewNet()->getNetworkViewOptions().showConnections() : false;
        // Start drawing adding an gl identificator
        GLHelper::pushName(getGlID());
        // Add a draw matrix
        GLHelper::pushMatrix();
        // Start with the drawing of the area traslating matrix to origin
        glTranslated(0, 0, getType() + offsetFront);
        // Set color of the base
        if (drawUsingSelectColor()) {
            GLHelper::setColor(s.colorSettings.selectedAdditionalColor);
        } else {
            GLHelper::setColor(s.detectorSettings.E2Color);
        }
        // draw lane2lane
        if (fromLane->getLane2laneConnections().exist(toLane)) {
            // check if draw only contour
            if (onlyContour) {
                GNEGeometry::drawContourGeometry(fromLane->getLane2laneConnections().getLane2laneGeometry(toLane), E2DetectorWidth);
            } else {
                GNEGeometry::drawGeometry(myNet->getViewNet(), fromLane->getLane2laneConnections().getLane2laneGeometry(toLane), E2DetectorWidth);
            }
        } else {
            // Set invalid person plan color
            GLHelper::setColor(RGBColor::RED);
            // calculate invalid geometry
            const GNEGeometry::Geometry invalidGeometry({fromLane->getLaneShape().back(), toLane->getLaneShape().front()});
            // check if draw only contour
            if (onlyContour) {
                GNEGeometry::drawContourGeometry(invalidGeometry, (0.5 * E2DetectorWidth));
            } else {
                // draw invalid geometry
                GNEGeometry::drawGeometry(myNet->getViewNet(), invalidGeometry, (0.5 * E2DetectorWidth));
            }
        }
        // Pop last matrix
        GLHelper::popMatrix();
        // Pop name
        GLHelper::popName();
        // check if shape dotted contour has to be drawn
        if (s.drawDottedContour() || myNet->getViewNet()->isAttributeCarrierInspected(this)) {
            // draw lane2lane dotted geometry
            if (fromLane->getLane2laneConnections().exist(toLane)) {
                GNEGeometry::drawDottedContourGeometry(GNEGeometry::DottedContourType::INSPECT, s, fromLane->getLane2laneConnections().getLane2laneDottedGeometry(toLane), E2DetectorWidth, false, false);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// GNEAdditional - protected methods
// ---------------------------------------------------------------------------

void
GNEAdditional::setDefaultValues() {
    // iterate over attributes and set default value
    for (const auto& i : myTagProperty) {
        if (i.hasStaticDefaultValue()) {
            setAttribute(i.getAttr(), i.getDefaultValue());
        }
    }
}


bool
GNEAdditional::isValidAdditionalID(const std::string& newID) const {
    if (SUMOXMLDefinitions::isValidAdditionalID(newID) && (myNet->retrieveAdditional(myTagProperty.getTag(), newID, false) == nullptr)) {
        return true;
    } else {
        return false;
    }
}


bool
GNEAdditional::isValidDetectorID(const std::string& newID) const {
    if (SUMOXMLDefinitions::isValidDetectorID(newID) && (myNet->retrieveAdditional(myTagProperty.getTag(), newID, false) == nullptr)) {
        return true;
    } else {
        return false;
    }
}


void
GNEAdditional::drawAdditionalID(const GUIVisualizationSettings& s) const {
    if (s.addName.show && (myAdditionalGeometry.getShape().size() > 0) && !s.drawForRectangleSelection && !s.drawForPositionSelection) {
        // calculate middle point
        const double middlePoint = (myAdditionalGeometry.getShape().length2D() * 0.5);
        // calculate position
        const Position pos = (myAdditionalGeometry.getShape().size() == 1) ? myAdditionalGeometry.getShape().front() : myAdditionalGeometry.getShape().positionAtOffset2D(middlePoint);
        // calculate rotation
        const double rot = (myAdditionalGeometry.getShape().size() == 1) ? myAdditionalGeometry.getShapeRotations().front() : myAdditionalGeometry.getShape().rotationDegreeAtOffset(middlePoint);
        // draw additional ID
        if (myTagProperty.hasAttribute(SUMO_ATTR_LANE)) {
            GLHelper::drawText(getMicrosimID(), pos, GLO_MAX - getType(), s.addName.scaledSize(s.scale), s.addName.color, s.getTextAngle(rot - 90));
        } else {
            GLHelper::drawText(getMicrosimID(), pos, GLO_MAX - getType(), s.addName.scaledSize(s.scale), s.addName.color, 0);
        }
    }
}


void
GNEAdditional::drawAdditionalName(const GUIVisualizationSettings& s) const {
    if (s.addFullName.show && (myAdditionalGeometry.getShape().size() > 0) && (myAdditionalName != "") && !s.drawForRectangleSelection && !s.drawForPositionSelection) {
        // calculate middle point
        const double middlePoint = (myAdditionalGeometry.getShape().length2D() * 0.5);
        // calculate position
        const Position pos = (myAdditionalGeometry.getShape().size() == 1) ? myAdditionalGeometry.getShape().front() : myAdditionalGeometry.getShape().positionAtOffset2D(middlePoint);
        // calculate rotation
        const double rot = (myAdditionalGeometry.getShape().size() == 1) ? myAdditionalGeometry.getShapeRotations().front() : myAdditionalGeometry.getShape().rotationDegreeAtOffset(middlePoint);
        // draw additional name
        if (myTagProperty.hasAttribute(SUMO_ATTR_LANE)) {
            GLHelper::drawText(myAdditionalName, pos, GLO_MAX - getType(), s.addFullName.scaledSize(s.scale), s.addFullName.color, s.getTextAngle(rot - 90));
        } else {
            GLHelper::drawText(myAdditionalName, pos, GLO_MAX - getType(), s.addFullName.scaledSize(s.scale), s.addFullName.color, 0);
        }
    }
}


void
GNEAdditional::replaceAdditionalParentEdges(const std::string& value) {
    replaceParentElements(this, parse<std::vector<GNEEdge*> >(getNet(), value));
}


void
GNEAdditional::replaceAdditionalParentLanes(const std::string& value) {
    replaceParentElements(this, parse<std::vector<GNELane*> >(getNet(), value));
}


void
GNEAdditional::replaceAdditionalChildEdges(const std::string& value) {
    replaceChildElements(this, parse<std::vector<GNEEdge*> >(getNet(), value));
}


void
GNEAdditional::replaceAdditionalChildLanes(const std::string& value) {
    replaceChildElements(this, parse<std::vector<GNELane*> >(getNet(), value));
}


void
GNEAdditional::replaceAdditionalParent(SumoXMLTag tag, const std::string& value, const int parentIndex) {
    std::vector<GNEAdditional*> parentAdditionals = getParentAdditionals();
    parentAdditionals[parentIndex] = myNet->retrieveAdditional(tag, value);
    // replace parent additionals
    replaceParentElements(this, parentAdditionals);
}


void
GNEAdditional::replaceDemandElementParent(SumoXMLTag tag, const std::string& value, const int parentIndex) {
    std::vector<GNEDemandElement*> parentDemandElements = getParentDemandElements();
    parentDemandElements[parentIndex] = myNet->retrieveDemandElement(tag, value);
    // replace parent demand elements
    replaceParentElements(this, parentDemandElements);
}


void
GNEAdditional::calculatePerpendicularLine(const double endLaneposition) {
    if (getParentEdges().empty()) {
        throw ProcessError("Invalid number of edges");
    } else {
        // get lanes
        const GNELane* firstLane = getParentEdges().front()->getLanes().front();
        const GNELane* lastLane = getParentEdges().front()->getLanes().back();
        // get first and back lane shapes
        PositionVector firstLaneShape = firstLane->getLaneShape();
        PositionVector lastLaneShape = lastLane->getLaneShape();
        // move shapes
        firstLaneShape.move2side((firstLane->getParentEdge()->getNBEdge()->getLaneWidth(firstLane->getIndex()) * 0.5) + 1);
        lastLaneShape.move2side(lastLane->getParentEdge()->getNBEdge()->getLaneWidth(lastLane->getIndex()) * -0.5);
        // calculate lane postion
        const double lanePosition = firstLaneShape.length2D() >= endLaneposition ? endLaneposition : firstLaneShape.length2D();
        // update geometry
        myAdditionalGeometry.updateGeometry({firstLaneShape.positionAtOffset2D(lanePosition), lastLaneShape.positionAtOffset2D(lanePosition)});
    }
}


void
GNEAdditional::drawSquaredAdditional(const GUIVisualizationSettings& s, const Position& pos, const double size, GUITexture texture, GUITexture selectedTexture) const {
    // Obtain drawing exaggeration
    const double exaggeration = s.addSize.getExaggeration(s, this);
    // first check if additional has to be drawn
    if (s.drawAdditionals(exaggeration) && myNet->getViewNet()->getDataViewOptions().showAdditionals()) {
        // check if boundary has to be drawn
        if (s.drawBoundaries) {
            GLHelper::drawBoundary(getCenteringBoundary());
        }
        // Start drawing adding an gl identificator
        GLHelper::pushName(getGlID());
        // Add layer matrix
        GLHelper::pushMatrix();
        // translate to front
        myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, GLO_E3DETECTOR);
        // translate to position
        glTranslated(pos.x(), pos.y(), 0);
        // scale
        glScaled(exaggeration, exaggeration, 1);
        // set White color
        glColor3d(1, 1, 1);
        // rotate
        glRotated(180, 0, 0, 1);
        // draw texture
        if (drawUsingSelectColor()) {
            GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(selectedTexture), size);
        } else {
            GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(texture), size);
        }
        // Pop layer matrix
        GLHelper::popMatrix();
        // Pop name
        GLHelper::popName();
        // push connection matrix
        GLHelper::pushMatrix();
        // translate to front
        myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, GLO_E3DETECTOR, -0.1);
        // draw lock icon
        GNEViewNetHelper::LockIcon::drawLockIcon(getType(), this, pos, exaggeration, 0.4, -0.5, -0.5);
        // Draw child connections
        drawHierarchicalConnections(s, this, exaggeration);
        // Pop connection matrix
        GLHelper::popMatrix();
        // check if dotted contour has to be drawn
        if (s.drawDottedContour() || myNet->getViewNet()->isAttributeCarrierInspected(this)) {
            GNEGeometry::drawDottedSquaredShape(GNEGeometry::DottedContourType::INSPECT, s, pos, size, size, 0, 0, 0, exaggeration);
        }
        if (s.drawDottedContour() || (myNet->getViewNet()->getFrontAttributeCarrier() == this)) {
            GNEGeometry::drawDottedSquaredShape(GNEGeometry::DottedContourType::FRONT, s, pos, size, size, 0, 0, 0, exaggeration);
        }
        // Draw additional ID
        drawAdditionalID(s);
        // draw additional name
        drawAdditionalName(s);
    }
}


void
GNEAdditional::enableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    //
}


void
GNEAdditional::disableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    //
}


GNELane*
GNEAdditional::getFirstPathLane() const {
    return getParentLanes().front();
}


GNELane*
GNEAdditional::getLastPathLane() const {
    return getParentLanes().back();
}


double
GNEAdditional::getPathElementDepartValue() const {
    return 0;   // CHECK
}


Position
GNEAdditional::getPathElementDepartPos() const {
    return Position();  // CHECK
}


double
GNEAdditional::getPathElementArrivalValue() const {
    return 0;   // CHECK
}


Position
GNEAdditional::getPathElementArrivalPos() const {
    return Position();  /// CHECK
}


const std::map<std::string, std::string>&
GNEAdditional::getACParametersMap() const {
    return getParametersMap();
}


bool
GNEAdditional::checkChildAdditionalRestriction() const {
    // throw exception because this function mus be implemented in child (see GNEE3Detector)
    throw ProcessError("Calling non-implemented function checkChildAdditionalRestriction during saving of " + getTagStr() + ". It muss be reimplemented in child class");
}


void
GNEAdditional::setEnabledAttribute(const int /*enabledAttributes*/) {
    //
}


/****************************************************************************/
