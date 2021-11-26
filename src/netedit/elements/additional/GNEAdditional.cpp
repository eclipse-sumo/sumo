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

#include <foreign/fontstash/fontstash.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/options/OptionsCont.h>

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


const GUIGeometry&
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
                } else if (myTagProperty.getTag() == additionalChild->getTagProperty().getParentTags().front()) {
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
                    } else if (myTagProperty.getTag() == additionalChild->getTagProperty().getParentTags().front()) {
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


double
GNEAdditional::getExaggeration(const GUIVisualizationSettings& s) const {
    return s.addSize.getExaggeration(s, this);
}


Boundary
GNEAdditional::getCenteringBoundary() const {
    return myAdditionalBoundary;
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
        const GNELane* lane = myNet->getAttributeCarriers()->retrieveLane(getAttribute(SUMO_ATTR_LANE));
        // Show menu command inner position
        const double innerPos = myAdditionalGeometry.getShape().nearest_offset_to_point2D(parent.getPositionInformation());
        GUIDesigns::buildFXMenuCommand(ret, "Cursor position over additional shape: " + toString(innerPos), nullptr, nullptr, 0);
        // If shape isn't empty, show menu command lane position
        if (myAdditionalGeometry.getShape().size() > 0) {
            const double lanePos = lane->getLaneShape().nearest_offset_to_point2D(myAdditionalGeometry.getShape().front());
            GUIDesigns::buildFXMenuCommand(ret, "Cursor position over " + toString(SUMO_TAG_LANE) + ": " + toString(innerPos + lanePos), nullptr, nullptr, 0);
        }
    } else if (myTagProperty.hasAttribute(SUMO_ATTR_EDGE) && (myAdditionalGeometry.getShape().size() > 1)) {
        const GNEEdge* edge = myNet->getAttributeCarriers()->retrieveEdge(getAttribute(SUMO_ATTR_EDGE));
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
        const double geometryDepartPos = getAttributeDouble(SUMO_ATTR_POSITION);
        // get endPos
        const double geometryEndPos = getAttributeDouble(SUMO_ATTR_ENDPOS);
        // declare path geometry
        GUIGeometry E2Geometry;
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
        // push layer matrix
        GLHelper::pushMatrix();
        // Start with the drawing of the area traslating matrix to origin
        glTranslated(0, 0, getType() + offsetFront);
        // Set color
        GLHelper::setColor(E2Color);
        // draw geometry
        GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), E2Geometry, E2DetectorWidth);
        // draw geometry points
        if (segment->isFirstSegment() && segment->isLastSegment()) {
            drawLeftGeometryPoint(myNet->getViewNet(), E2Geometry.getShape().front(),  E2Geometry.getShapeRotations().front(), E2Color);
            drawRightGeometryPoint(myNet->getViewNet(), E2Geometry.getShape().back(), E2Geometry.getShapeRotations().back(), E2Color);
        } else if (segment->isFirstSegment()) {
            drawLeftGeometryPoint(myNet->getViewNet(), E2Geometry.getShape().front(), E2Geometry.getShapeRotations().front(), E2Color);
        } else if (segment->isLastSegment()) {
            drawRightGeometryPoint(myNet->getViewNet(), E2Geometry.getShape().back(), E2Geometry.getShapeRotations().back(), E2Color);
        }
        // Pop layer matrix
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
                GUIGeometry::rotateOverLane(rot);
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
        if (myNet->getViewNet()->isAttributeCarrierInspected(this)) {
            // declare trim geometry to draw
            const auto shape = (segment->isFirstSegment() || segment->isLastSegment()) ? E2Geometry.getShape() : lane->getLaneShape();
            // draw inspected dotted contour
            if (myNet->getViewNet()->isAttributeCarrierInspected(this)) {
                GUIDottedGeometry::drawDottedContourShape(GUIDottedGeometry::DottedContourType::INSPECT, s, shape, E2DetectorWidth, 1, segment->isFirstSegment(), segment->isLastSegment());
            }
            // draw front dotted contour
            if ((myNet->getViewNet()->getFrontAttributeCarrier() == this)) {
                GUIDottedGeometry::drawDottedContourShape(GUIDottedGeometry::DottedContourType::FRONT, s, shape, E2DetectorWidth, 1, segment->isFirstSegment(), segment->isLastSegment());
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
                GUIGeometry::drawContourGeometry(fromLane->getLane2laneConnections().getLane2laneGeometry(toLane), E2DetectorWidth);
            } else {
                GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), fromLane->getLane2laneConnections().getLane2laneGeometry(toLane), E2DetectorWidth);
            }
        } else {
            // Set invalid person plan color
            GLHelper::setColor(RGBColor::RED);
            // calculate invalid geometry
            const GUIGeometry invalidGeometry({fromLane->getLaneShape().back(), toLane->getLaneShape().front()});
            // check if draw only contour
            if (onlyContour) {
                GUIGeometry::drawContourGeometry(invalidGeometry, (0.5 * E2DetectorWidth));
            } else {
                // draw invalid geometry
                GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), invalidGeometry, (0.5 * E2DetectorWidth));
            }
        }
        // Pop last matrix
        GLHelper::popMatrix();
        // Pop name
        GLHelper::popName();
        // check if shape dotted contour has to be drawn
        if (myNet->getViewNet()->isAttributeCarrierInspected(this)) {
            // draw lane2lane dotted geometry
            if (fromLane->getLane2laneConnections().exist(toLane)) {
                GUIDottedGeometry::drawDottedContourShape(GUIDottedGeometry::DottedContourType::INSPECT, s, fromLane->getLane2laneConnections().getLane2laneGeometry(toLane).getShape(),
                        E2DetectorWidth, 1, false, false);
            }
        }
        if ((myNet->getViewNet()->getFrontAttributeCarrier() == this)) {
            // draw lane2lane dotted geometry
            if (fromLane->getLane2laneConnections().exist(toLane)) {
                GUIDottedGeometry::drawDottedContourShape(GUIDottedGeometry::DottedContourType::FRONT, s, fromLane->getLane2laneConnections().getLane2laneGeometry(toLane).getShape(),
                        E2DetectorWidth, 1, false, false);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// GNEAdditional - protected methods
// ---------------------------------------------------------------------------

bool
GNEAdditional::isValidAdditionalID(const std::string& newID) const {
    if (SUMOXMLDefinitions::isValidAdditionalID(newID) && (myNet->getAttributeCarriers()->retrieveAdditional(myTagProperty.getTag(), newID, false) == nullptr)) {
        return true;
    } else {
        return false;
    }
}


bool
GNEAdditional::isValidDetectorID(const std::string& newID) const {
    if (SUMOXMLDefinitions::isValidDetectorID(newID) && (myNet->getAttributeCarriers()->retrieveAdditional(myTagProperty.getTag(), newID, false) == nullptr)) {
        return true;
    } else {
        return false;
    }
}


void
GNEAdditional::drawAdditionalID(const GUIVisualizationSettings& s) const {
    if (s.addName.show(this) && (myAdditionalGeometry.getShape().size() > 0) && !s.drawForRectangleSelection && !s.drawForPositionSelection) {
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
    if (s.addFullName.show(this) && (myAdditionalGeometry.getShape().size() > 0) && (myAdditionalName != "") && !s.drawForRectangleSelection && !s.drawForPositionSelection) {
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
    std::vector<GNEAdditional*> parentAdditionals;
    // special case for calibrators and routeprobes
    if (value.size() > 0) {
        parentAdditionals = getParentAdditionals();
        if ((parentAdditionals.size() == 0) && (parentIndex == 0)) {
            parentAdditionals.push_back(myNet->getAttributeCarriers()->retrieveAdditional(tag, value));
        } else {
            parentAdditionals[parentIndex] = myNet->getAttributeCarriers()->retrieveAdditional(tag, value);
        }
    }
    // replace parent additionals
    replaceParentElements(this, parentAdditionals);
}


void
GNEAdditional::replaceDemandElementParent(SumoXMLTag tag, const std::string& value, const int parentIndex) {
    std::vector<GNEDemandElement*> parentDemandElements = getParentDemandElements();
    parentDemandElements[parentIndex] = myNet->getAttributeCarriers()->retrieveDemandElement(tag, value);
    // replace parent demand elements
    replaceParentElements(this, parentDemandElements);
}


void
GNEAdditional::shiftLaneIndex() {
    // get new lane parent vector
    std::vector<GNELane*> newLane = {getParentLanes().front()->getParentEdge()->getLanes().at(getParentLanes().front()->getIndex() + 1)};
    // replace parent elements
    replaceParentElements(this, newLane);
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
    const double exaggeration = getExaggeration(s);
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
        // draw lock icon
        GNEViewNetHelper::LockIcon::drawLockIcon(this, getType(), pos, exaggeration, 0.4, 0.5, 0.5);
        // check if dotted contour has to be drawn
        if (myNet->getViewNet()->isAttributeCarrierInspected(this)) {
            GUIDottedGeometry::drawDottedSquaredShape(GUIDottedGeometry::DottedContourType::INSPECT, s, pos, size, size, 0, 0, 0, exaggeration);
        }
        if ((myNet->getViewNet()->getFrontAttributeCarrier() == this)) {
            GUIDottedGeometry::drawDottedSquaredShape(GUIDottedGeometry::DottedContourType::FRONT, s, pos, size, size, 0, 0, 0, exaggeration);
        }
        // Draw additional ID
        drawAdditionalID(s);
        // draw additional name
        drawAdditionalName(s);
    }
}


void
GNEAdditional::drawListedAddtional(const GUIVisualizationSettings& s, const Position& parentPosition, const int offsetX, const int extraOffsetY,
                                   const RGBColor baseCol, const RGBColor textCol, GUITexture texture, const std::string text) const {
    // first check if additional has to be drawn
    if (s.drawAdditionals(getExaggeration(s)) && myNet->getViewNet()->getDataViewOptions().showAdditionals()) {
        // declare offsets
        const double lineOffset = 0.1875;
        const double baseOffsetX = 6.25;
        const double baseOffsetY = 0.6;
        // get draw position index
        const int drawPositionIndex = getDrawPositionIndex();
        // calculate lineA position (from parent to middle)
        Position positionLineA = parentPosition;
        const double positionLineA_Y = (0 - extraOffsetY + baseOffsetY);
        // set position depending of indexes
        positionLineA.add(1 + lineOffset + (baseOffsetX * offsetX), positionLineA_Y, 0);
        // calculate lineC position (From middle until current listenAdditional
        Position positionLineB = parentPosition;
        const double positionLineB_Y = ((drawPositionIndex * -1) - extraOffsetY + baseOffsetY);
        // set position depending of indexes
        positionLineB.add(1 + lineOffset + (baseOffsetX * offsetX) + (2 * lineOffset), positionLineB_Y, 0);
        // calculate signPosition position
        Position signPosition = parentPosition;
        // set position depending of indexes
        signPosition.add(4.5 + (baseOffsetX * offsetX), (drawPositionIndex * -1) - extraOffsetY + 1, 0);
        // check if boundary has to be drawn
        if (s.drawBoundaries) {
            GLHelper::drawBoundary(getCenteringBoundary());
        }
        // Start drawing adding an gl identificator
        GLHelper::pushName(getGlID());
        // calculate colors
        const RGBColor baseColor = isAttributeCarrierSelected() ? s.colorSettings.selectedAdditionalColor : baseCol;
        const RGBColor secondColor = baseColor.changedBrightness(-30);
        const RGBColor textColor = isAttributeCarrierSelected() ? s.colorSettings.selectedAdditionalColor.changedBrightness(30) : textCol;
        // Add layer matrix
        GLHelper::pushMatrix();
        // translate to front
        myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, getType());
        // set line color
        GLHelper::setColor(s.additionalSettings.connectionColor);
        // draw both lines
        GLHelper::drawBoxLine(positionLineA, 0, 0.1, lineOffset);
        GLHelper::drawBoxLine(positionLineB, 0, 0.1, lineOffset);
        // check if draw middle lane
        if (drawPositionIndex != 0) {
            // calculate length
            const double lenght = std::abs(positionLineA_Y - positionLineB_Y);
            // push middle lane matrix
            GLHelper::pushMatrix();
            //move and rotate
            glTranslated(positionLineA.x() + lineOffset, positionLineA.y(), 0);
            glRotated(90, 0, 0, 1);
            glTranslated((lenght * -0.5), 0, 0);
            // draw line
            GLHelper::drawBoxLine(Position(0, 0), 0, 0.1, lenght * 0.5);
            // pop middle lane matrix
            GLHelper::popMatrix();
        }
        // draw extern rectangle
        GLHelper::setColor(secondColor);
        GLHelper::drawBoxLine(signPosition, 0, 0.96, 2.75);
        // move to front
        glTranslated(0, -0.06, 0.1);
        // draw intern rectangle
        GLHelper::setColor(baseColor);
        GLHelper::drawBoxLine(signPosition, 0, 0.84, 2.69);
        // move position down
        signPosition.add(-2, -0.43, 0);
        // draw interval
        GLHelper::drawText(adjustListedAdditionalText(text), signPosition, .1, 0.5, textColor, 0, (FONS_ALIGN_LEFT | FONS_ALIGN_MIDDLE));
        // move to icon position
        signPosition.add(-0.3, 0);
        // check if draw lock icon or rerouter interval icon
        if (GNEViewNetHelper::LockIcon::checkDrawing(this, getType(), 1)) {
            // pop layer matrix
            GLHelper::popMatrix();
            // Pop name
            GLHelper::popName();
            // draw lock icon
            GNEViewNetHelper::LockIcon::drawLockIcon(this, getType(), signPosition, 1, 0.4, 0.0, -0.05);
        } else {
            // translate to front
            glTranslated(signPosition.x(), signPosition.y(), 0.1);
            // set White color
            glColor3d(1, 1, 1);
            // rotate
            glRotated(180, 0, 0, 1);
            // draw texture
            GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(texture), 0.25);
            // pop layer matrix
            GLHelper::popMatrix();
            // Pop name
            GLHelper::popName();
        }
        // check if dotted contour has to be drawn
        if (myNet->getViewNet()->isAttributeCarrierInspected(this)) {
            GUIDottedGeometry::drawDottedSquaredShape(GUIDottedGeometry::DottedContourType::INSPECT, s, signPosition, 0.56, 2.75, 0, -2.3, 0, 1);
        }
        if ((myNet->getViewNet()->getFrontAttributeCarrier() == this)) {
            GUIDottedGeometry::drawDottedSquaredShape(GUIDottedGeometry::DottedContourType::FRONT, s, signPosition, 0.56, 2.75, 0, -2.3, 0, 1);
        }
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


bool
GNEAdditional::isAttributeComputed(SumoXMLAttr /*key*/) const {
    return false;
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


void
GNEAdditional::drawParentChildLines(const GUIVisualizationSettings& s, const RGBColor& color, const bool onlySymbols) const {
    // check if current additional is inspected, front or selected
    const bool currentDrawEntire = myNet->getViewNet()->isAttributeCarrierInspected(this) ||
                                   (myNet->getViewNet()->getFrontAttributeCarrier() == this) || isAttributeCarrierSelected();
    // push layer matrix
    GLHelper::pushMatrix();
    // translate to parentChildLine layer
    glTranslated(0, 0, GLO_PARENTCHILDLINE);
    // iterate over parent additionals
    for (const auto& parent : getParentAdditionals()) {
        // get inspected flag
        const bool inspected = myNet->getViewNet()->isAttributeCarrierInspected(parent);
        // draw parent lines
        GUIGeometry::drawParentLine(s, getPositionInView(), parent->getPositionInView(),
                                    (isAttributeCarrierSelected() || parent->isAttributeCarrierSelected()) ? s.additionalSettings.connectionColorSelected : color,
                                    currentDrawEntire || inspected || parent->isAttributeCarrierSelected());
    }
    // special case for Parking area reroutes
    if (getTagProperty().getTag() == SUMO_TAG_REROUTER) {
        // iterate over rerouter elements
        for (const auto& rerouterInterval : getChildAdditionals()) {
            for (const auto& rerouterElement : rerouterInterval->getChildAdditionals()) {
                if (rerouterElement->getTagProperty().getTag() == SUMO_TAG_PARKING_AREA_REROUTE) {
                    // get parking area
                    const auto parkingArea = rerouterElement->getParentAdditionals().at(1);
                    // get inspected flag
                    const bool inspected = myNet->getViewNet()->isAttributeCarrierInspected(parkingArea);
                    // draw parent lines
                    GUIGeometry::drawParentLine(s, getPositionInView(), parkingArea->getPositionInView(),
                                                (isAttributeCarrierSelected() || parkingArea->isAttributeCarrierSelected()) ? s.additionalSettings.connectionColorSelected : color,
                                                currentDrawEntire || inspected || parkingArea->isAttributeCarrierSelected());
                }
            }
        }
    }
    // iterate over child additionals
    for (const auto& child : getChildAdditionals()) {
        // get inspected flag
        const bool inspected = myNet->getViewNet()->isAttributeCarrierInspected(child);
        // special case for parking zone reroute
        if (child->getTagProperty().getTag() == SUMO_TAG_PARKING_AREA_REROUTE) {
            // draw child line between parking area and rerouter
            GUIGeometry::drawChildLine(s, getPositionInView(), child->getParentAdditionals().front()->getParentAdditionals().front()->getPositionInView(),
                                       (isAttributeCarrierSelected() || child->isAttributeCarrierSelected()) ? s.additionalSettings.connectionColorSelected : color,
                                       currentDrawEntire || inspected || child->isAttributeCarrierSelected());
        } else if (!onlySymbols || child->getTagProperty().isSymbol()) {
            // draw child line
            GUIGeometry::drawChildLine(s, getPositionInView(), child->getPositionInView(),
                                       (isAttributeCarrierSelected() || child->isAttributeCarrierSelected()) ? s.additionalSettings.connectionColorSelected : color,
                                       currentDrawEntire || inspected || child->isAttributeCarrierSelected());
        }
    }
    // pop layer matrix
    GLHelper::popMatrix();
}


void
GNEAdditional::drawUpGeometryPoint(const GNEViewNet* viewNet, const Position& pos, const double rot, const RGBColor& baseColor) {
    drawSemiCircleGeometryPoint(viewNet, pos, rot, baseColor, -90, 90);
}


void
GNEAdditional::drawDownGeometryPoint(const GNEViewNet* viewNet, const Position& pos, const double rot, const RGBColor& baseColor) {
    drawSemiCircleGeometryPoint(viewNet, pos, rot, baseColor, 90, 270);
}


void
GNEAdditional::drawLeftGeometryPoint(const GNEViewNet* viewNet, const Position& pos, const double rot, const RGBColor& baseColor) {
    drawSemiCircleGeometryPoint(viewNet, pos, rot, baseColor, -90, 90);
}


void
GNEAdditional::drawRightGeometryPoint(const GNEViewNet* viewNet, const Position& pos, const double rot, const RGBColor& baseColor) {
    drawSemiCircleGeometryPoint(viewNet, pos, rot, baseColor, 270, 90);
}


int
GNEAdditional::getDrawPositionIndex() const {
    // filter symbols
    std::vector<GNEAdditional*> children;
    for (const auto& child : getParentAdditionals().front()->getChildAdditionals()) {
        if (!child->getTagProperty().isSymbol()) {
            children.push_back(child);
        }
    }
    // now get index
    for (int i = 0; i < (int)children.size(); i++) {
        if (children.at(i) == this) {
            return i;
        }
    }
    return 0;
}


bool
GNEAdditional::checkChildAdditionalRestriction() const {
    // throw exception because this function mus be implemented in child (see GNEE3Detector)
    throw ProcessError("Calling non-implemented function checkChildAdditionalRestriction during saving of " + getTagStr() + ". It muss be reimplemented in child class");
}


void
GNEAdditional::toogleAttribute(SumoXMLAttr /*key*/, const bool /*value*/, const int /*previousParameters*/) {
    //throw InvalidArgument("Nothing to enable");
}


void
GNEAdditional::drawSemiCircleGeometryPoint(const GNEViewNet* viewNet, const Position& pos, const double rot, const RGBColor& baseColor,
        const double fromAngle, const double toAngle) {
    // first check that we're in move mode and shift key is pressed
    if (viewNet->getEditModes().isCurrentSupermodeNetwork() && (viewNet->getEditModes().networkEditMode == NetworkEditMode::NETWORK_MOVE) &&
            viewNet->getMouseButtonKeyPressed().shiftKeyPressed()) {
        // calculate new color
        const RGBColor color = baseColor.changedBrightness(-50);
        // push matrix
        GLHelper::pushMatrix();
        // translated to front
        glTranslated(0, 0, 0.1);
        // set color
        GLHelper::setColor(color);
        // push geometry point matrix
        GLHelper::pushMatrix();
        // translate and rotate
        glTranslated(pos.x(), pos.y(), 0.1);
        glRotated(rot, 0, 0, 1);
        // draw geometry point
        GLHelper::drawFilledCircle(viewNet->getVisualisationSettings().neteditSizeSettings.additionalGeometryPointRadius,
                                   viewNet->getVisualisationSettings().getCircleResolution(), fromAngle, toAngle);
        // pop geometry point matrix
        GLHelper::popMatrix();
        // pop draw matrix
        GLHelper::popMatrix();
    }
}


std::string
GNEAdditional::adjustListedAdditionalText(const std::string& text) const {
    // 10 + 3 + 10
    if (text.size() <= 23) {
        return text;
    } else {
        // get text size
        const int textPosition = (int)text.size() - 10;
        // declare strings
        std::string partA, partB;
        // resize
        partA.reserve(10);
        partB.reserve(10);
        // fill both
        for (int i = 0; i < 10; i++) {
            partA.push_back(text.at(i));
            partB.push_back(text.at(textPosition + i));
        }
        // return composition
        return (partA + "..." + partB);
    }
}

/****************************************************************************/
