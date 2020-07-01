/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/options/OptionsCont.h>

#include "GNEAdditional.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEAdditional::GNEAdditional(const std::string& id, GNENet* net, GUIGlObjectType type, SumoXMLTag tag, std::string additionalName, bool blockMovement,
        const std::vector<GNEJunction*>& junctionParents,
        const std::vector<GNEEdge*>& edgeParents,
        const std::vector<GNELane*>& laneParents,
        const std::vector<GNEAdditional*>& additionalParents,
        const std::vector<GNEShape*>& shapeParents,
        const std::vector<GNETAZElement*>& TAZElementParents,
        const std::vector<GNEDemandElement*>& demandElementParents,
        const std::vector<GNEGenericData*>& genericDataParents,
        const std::vector<GNEJunction*>& junctionChildren,
        const std::vector<GNEEdge*>& edgeChildren,
        const std::vector<GNELane*>& laneChildren,
        const std::vector<GNEAdditional*>& additionalChildren,
        const std::vector<GNEShape*>& shapeChildren,
        const std::vector<GNETAZElement*>& TAZElementChildren,
        const std::vector<GNEDemandElement*>& demandElementChildren,
        const std::vector<GNEGenericData*>& genericDataChildren) :
    GUIGlObject(type, id),
    GNEAttributeCarrier(tag, net),
    GNEHierarchicalParentElements(this, junctionParents, edgeParents, laneParents, additionalParents, shapeParents, TAZElementParents, demandElementParents, genericDataParents),
    GNEHierarchicalChildElements(this, junctionChildren, edgeChildren, laneChildren, additionalChildren, shapeChildren, TAZElementChildren, demandElementChildren, genericDataChildren),
    GNEPathElements(this),
    myAdditionalName(additionalName),
    myBlockMovement(blockMovement),
    myBlockIcon(this),
    mySpecialColor(nullptr) {
}


GNEAdditional::GNEAdditional(GNEAdditional* additionalParent, GNENet* net, GUIGlObjectType type, SumoXMLTag tag, std::string additionalName, bool blockMovement,
        const std::vector<GNEJunction*>& junctionParents,
        const std::vector<GNEEdge*>& edgeParents,
        const std::vector<GNELane*>& laneParents,
        const std::vector<GNEAdditional*>& additionalParents,
        const std::vector<GNEShape*>& shapeParents,
        const std::vector<GNETAZElement*>& TAZElementParents,
        const std::vector<GNEDemandElement*>& demandElementParents,
        const std::vector<GNEGenericData*>& genericDataParents,
        const std::vector<GNEJunction*>& junctionChildren,
        const std::vector<GNEEdge*>& edgeChildren,
        const std::vector<GNELane*>& laneChildren,
        const std::vector<GNEAdditional*>& additionalChildren,
        const std::vector<GNEShape*>& shapeChildren,
        const std::vector<GNETAZElement*>& TAZElementChildren,
        const std::vector<GNEDemandElement*>& demandElementChildren,
        const std::vector<GNEGenericData*>& genericDataChildren) :
    GUIGlObject(type, additionalParent->generateAdditionalChildID(tag)),
    GNEAttributeCarrier(tag, net),
    GNEHierarchicalParentElements(this, junctionParents, edgeParents, laneParents, additionalParents, shapeParents, TAZElementParents, demandElementParents, genericDataParents),
    GNEHierarchicalChildElements(this, junctionChildren, edgeChildren, laneChildren, additionalChildren, shapeChildren, TAZElementChildren, demandElementChildren, genericDataChildren),
    GNEPathElements(this),
    myAdditionalName(additionalName),
    myBlockMovement(blockMovement),
    myBlockIcon(this),
    mySpecialColor(nullptr) {
}


GNEAdditional::~GNEAdditional() {}


const std::string&
GNEAdditional::getID() const {
    return getMicrosimID();
}


GUIGlObject*
GNEAdditional::getGUIGlObject() {
    return this;
}


std::string
GNEAdditional::generateAdditionalChildID(SumoXMLTag childTag) {
    int counter = (int)getChildAdditionals().size();
    while (myNet->retrieveAdditional(childTag, getID() + toString(childTag) + toString(counter), false) != nullptr) {
        counter++;
    }
    return (getID() + toString(childTag) + toString(counter));
}


const GNEGeometry::Geometry&
GNEAdditional::getAdditionalGeometry() const {
    return myAdditionalGeometry;
}


const GNEGeometry::SegmentGeometry&
GNEAdditional::getAdditionalSegmentGeometry() const {
    return myAdditionalSegmentGeometry;
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
        // Open Tag or synonym Tag
        if (myTagProperty.hasTagSynonym()) {
            device.openTag(myTagProperty.getTagSynonym());
        } else {
            device.openTag(myTagProperty.getTag());
        }
        // iterate over attributes and write it
        for (const auto& tagProperty : myTagProperty) {
            // obtain attribute
            const std::string attributeValue = getAttribute(tagProperty.getAttr());
            //  first check if attribute is optional but not vehicle classes
            if (tagProperty.isOptional() && !tagProperty.isVClasses()) {
                // Only write attributes with default value if is different from original
                if (tagProperty.getDefaultValue() != attributeValue) {
                    // check if attribute must be written using a synonim
                    if (tagProperty.hasAttrSynonym()) {
                        device.writeAttr(tagProperty.getAttrSynonym(), attributeValue);
                    } else {
                        // SVC permissions uses their own writting function
                        if (tagProperty.isSVCPermission()) {
                            // disallow attribute musn't be written
                            if (tagProperty.getAttr() != SUMO_ATTR_DISALLOW) {
                                writePermissions(device, parseVehicleClasses(attributeValue));
                            }
                        } else if (myTagProperty.canMaskXYZPositions() && (tagProperty.getAttr() == SUMO_ATTR_POSITION)) {
                            // get position attribute and write it separate
                            Position pos = parse<Position>(getAttribute(SUMO_ATTR_POSITION));
                            device.writeAttr(SUMO_ATTR_X, toString(pos.x()));
                            device.writeAttr(SUMO_ATTR_Y, toString(pos.y()));
                            // write 0 only if is different from 0 (the default value)
                            if (pos.z() != 0) {
                                device.writeAttr(SUMO_ATTR_Z, toString(pos.z()));
                            }
                        } else {
                            device.writeAttr(tagProperty.getAttr(), attributeValue);
                        }
                    }
                }
            } else {
                // Attributes without default values are always writted
                if (tagProperty.hasAttrSynonym()) {
                    device.writeAttr(tagProperty.getAttrSynonym(), attributeValue);
                } else {
                    // SVC permissions uses their own writting function
                    if (tagProperty.isSVCPermission()) {
                        // disallow attribute musn't be written
                        if (tagProperty.getAttr() != SUMO_ATTR_DISALLOW) {
                            writePermissions(device, parseVehicleClasses(attributeValue));
                        }
                    } else if (myTagProperty.canMaskXYZPositions() && (tagProperty.getAttr() == SUMO_ATTR_POSITION)) {
                        // get position attribute and write it separate
                        Position pos = parse<Position>(getAttribute(SUMO_ATTR_POSITION));
                        device.writeAttr(SUMO_ATTR_X, toString(pos.x()));
                        device.writeAttr(SUMO_ATTR_Y, toString(pos.y()));
                        // write 0 only if is different from 0 (the default value)
                        if (pos.z() != 0) {
                            device.writeAttr(SUMO_ATTR_Z, toString(pos.z()));
                        }
                    } else {
                        device.writeAttr(tagProperty.getAttr(), attributeValue);
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
                } else if (myTagProperty.getTag() == additionalChild->getTagProperty().getParentTag()) {
                    additionalChild->writeAdditional(deviceChildren);
                }
            }
            deviceChildren.close();
        } else {
            for (const auto& additionalChild : getChildAdditionals()) {
                // avoid to write two times additionals that haben two parents (Only write as child of first parent)
                if (additionalChild->getParentAdditionals().size() < 2) {
                    additionalChild->writeAdditional(device);
                } else if (myTagProperty.getTag() == additionalChild->getTagProperty().getParentTag()) {
                    additionalChild->writeAdditional(device);
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


void
GNEAdditional::startGeometryMoving() {
    // only move if additional is drawable
    if (myTagProperty.isDrawable()) {
        // always save original position over view
        myMove.originalViewPosition = getPositionInView();
        // check if position over lane or lanes has to be saved
        if (myTagProperty.hasAttribute(SUMO_ATTR_LANE)) {
            if (myTagProperty.canMaskStartEndPos()) {
                // obtain start and end position
                myMove.firstOriginalLanePosition = getAttribute(SUMO_ATTR_STARTPOS);
                myMove.secondOriginalPosition = getAttribute(SUMO_ATTR_ENDPOS);
            } else {
                // obtain position attribute
                myMove.firstOriginalLanePosition = getAttribute(SUMO_ATTR_POSITION);
            }
        } else if (myTagProperty.hasAttribute(SUMO_ATTR_LANES) &&
                   myTagProperty.hasAttribute(SUMO_ATTR_POSITION) &&
                   myTagProperty.hasAttribute(SUMO_ATTR_ENDPOS)) {
            // obtain start and end position
            myMove.firstOriginalLanePosition = getAttribute(SUMO_ATTR_POSITION);
            myMove.secondOriginalPosition = getAttribute(SUMO_ATTR_ENDPOS);
        }
        // save current centering boundary if element is placed in RTree
        if (myTagProperty.isPlacedInRTree()) {
            myMove.movingGeometryBoundary = getCenteringBoundary();
        }
        // start geometry in all children
        for (const auto& i : getChildDemandElements()) {
            i->startGeometryMoving();
        }
    }
}


void
GNEAdditional::endGeometryMoving() {
    // check that endGeometryMoving was called only once
    if (myTagProperty.isDrawable()) {
        // check if object must be placed in RTREE
        if (myTagProperty.isPlacedInRTree()) {
            // Remove object from net
            myNet->removeGLObjectFromGrid(this);
            // reset myMovingGeometryBoundary
            myMove.movingGeometryBoundary.reset();
            // add object into grid again (using the new centering boundary)
            myNet->addGLObjectIntoGrid(this);
        }
        // end geometry in all children
        for (const auto& i : getChildDemandElements()) {
            i->endGeometryMoving();
        }
    }
}


bool
GNEAdditional::isAdditionalBlocked() const {
    return myBlockMovement;
}


void 
GNEAdditional::updatePartialGeometry(const GNELane* lane) {
    // currently only for E2 Multilane Detectors
    if (myTagProperty.getTag() == SUMO_TAG_E2DETECTOR_MULTILANE) {
        // declare extreme geometry
        GNEGeometry::ExtremeGeometry extremeGeometry;
        // get extremes
        extremeGeometry.laneStartPosition = GNEAttributeCarrier::parse<double>(getAttribute(SUMO_ATTR_POSITION));
        extremeGeometry.laneEndPosition = GNEAttributeCarrier::parse<double>(getAttribute(SUMO_ATTR_ENDPOS));
        // update geometry path for the given lane
        GNEGeometry::updateGeometricPath(myAdditionalSegmentGeometry, lane, extremeGeometry);
    }
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
    new FXMenuCommand(ret, ("Copy " + getTagStr() + " name to clipboard").c_str(), nullptr, ret, MID_COPY_NAME);
    new FXMenuCommand(ret, ("Copy " + getTagStr() + " typed name to clipboard").c_str(), nullptr, ret, MID_COPY_TYPED_NAME);
    new FXMenuSeparator(ret);
    // build selection and show parameters menu
    myNet->getViewNet()->buildSelectionACPopupEntry(ret, this);
    buildShowParamsPopupEntry(ret);
    // show option to open additional dialog
    if (myTagProperty.hasDialog()) {
        new FXMenuCommand(ret, ("Open " + getTagStr() + " Dialog").c_str(), getIcon(), &parent, MID_OPEN_ADDITIONAL_DIALOG);
        new FXMenuSeparator(ret);
    }
    // Show position parameters
    if (myTagProperty.hasAttribute(SUMO_ATTR_LANE)) {
        GNELane* lane = myNet->retrieveLane(getAttribute(SUMO_ATTR_LANE));
        // Show menu command inner position
        const double innerPos = myAdditionalGeometry.getShape().nearest_offset_to_point2D(parent.getPositionInformation());
        new FXMenuCommand(ret, ("Cursor position over additional shape: " + toString(innerPos)).c_str(), nullptr, nullptr, 0);
        // If shape isn't empty, show menu command lane position
        if (myAdditionalGeometry.getShape().size() > 0) {
            const double lanePos = lane->getLaneShape().nearest_offset_to_point2D(myAdditionalGeometry.getShape().front());
            new FXMenuCommand(ret, ("Cursor position over " + toString(SUMO_TAG_LANE) + ": " + toString(innerPos + lanePos)).c_str(), nullptr, nullptr, 0);
        }
    } else if (myTagProperty.hasAttribute(SUMO_ATTR_EDGE)) {
        GNEEdge* edge = myNet->retrieveEdge(getAttribute(SUMO_ATTR_EDGE));
        // Show menu command inner position
        const double innerPos = myAdditionalGeometry.getShape().nearest_offset_to_point2D(parent.getPositionInformation());
        new FXMenuCommand(ret, ("Cursor position over additional shape: " + toString(innerPos)).c_str(), nullptr, nullptr, 0);
        // If shape isn't empty, show menu command edge position
        if (myAdditionalGeometry.getShape().size() > 0) {
            const double edgePos = edge->getLanes().at(0)->getLaneShape().nearest_offset_to_point2D(myAdditionalGeometry.getShape().front());
            new FXMenuCommand(ret, ("Mouse position over " + toString(SUMO_TAG_EDGE) + ": " + toString(innerPos + edgePos)).c_str(), nullptr, nullptr, 0);
        }
    } else {
        new FXMenuCommand(ret, ("Cursor position in view: " + toString(getPositionInView().x()) + "," + toString(getPositionInView().y())).c_str(), nullptr, nullptr, 0);
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
GNEAdditional::drawPartialGL(const GUIVisualizationSettings& s, const GNELane* lane, const double offsetFront) const {
    // calculate E2Detector width
    const double E2DetectorWidth = s.addSize.getExaggeration(s, lane);
    // check if E2 can be drawn
    if (s.drawAdditionals(E2DetectorWidth) && myNet->getViewNet()->getDataViewOptions().showAdditionals()) {
        // obtain color
        const RGBColor routeColor = drawUsingSelectColor()? s.colorSettings.selectedAdditionalColor : s.detectorSettings.E2Color;
        // Start drawing adding an gl identificator
        glPushName(getGlID());
        // Add a draw matrix
        glPushMatrix();
        // Start with the drawing of the area traslating matrix to origin
        glTranslated(0, 0, getType() + offsetFront);
        // iterate over segments
        for (const auto& segment : myAdditionalSegmentGeometry) {
            // draw partial segment
            if (segment.isLaneSegment() && (segment.getLane() == lane)) {
                // Set route color (needed due drawShapeDottedContour)
                GLHelper::setColor(routeColor);
                // draw box lines
                GNEGeometry::drawSegmentGeometry(myNet->getViewNet(), segment, E2DetectorWidth);
            }
        }
        // Pop last matrix
        glPopMatrix();
        // Draw name if isn't being drawn for selecting
        if (!s.drawForRectangleSelection) {
            drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
        }
        // Pop name
        glPopName();
        // check if shape dotted contour has to be drawn
        if (s.drawDottedContour() || (myNet->getViewNet()->getInspectedAttributeCarrier() == this)) {
            // iterate over segments
            for (const auto& segment : myAdditionalSegmentGeometry) {
                if (segment.isLaneSegment() && (segment.getLane() == lane)) {
                    // draw partial segment
                    if (getParentLanes().front() == lane) {
                        // draw front dotted contour
                        GNEGeometry::drawDottedContourLane(s, GNEGeometry::DottedGeometry(s, segment.getShape(), false), E2DetectorWidth, true, false);
                    } else if (getParentLanes().back() == lane) {
                        // draw back dotted contour
                        GNEGeometry::drawDottedContourLane(s, GNEGeometry::DottedGeometry(s, segment.getShape(), false), E2DetectorWidth, false, true);
                    } else {
                        // draw dotted contour
                        GNEGeometry::drawDottedContourLane(s, lane->getDottedLaneGeometry(), E2DetectorWidth, false, false);
                    }
                }
            }
        }
    }
}


void 
GNEAdditional::drawPartialGL(const GUIVisualizationSettings& s, const GNELane* fromLane, const GNELane* toLane, const double offsetFront) const {
    // calculate E2Detector width
    const double E2DetectorWidth = s.addSize.getExaggeration(s, fromLane);
    // check if E2 can be drawn
    if (s.drawAdditionals(E2DetectorWidth) && myNet->getViewNet()->getDataViewOptions().showAdditionals()) {
        // Start drawing adding an gl identificator
        glPushName(getGlID());
        // Add a draw matrix
        glPushMatrix();
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
            GNEGeometry::drawGeometry(myNet->getViewNet(), fromLane->getLane2laneConnections().getLane2laneGeometry(toLane), E2DetectorWidth);
        } else {
            // Set invalid person plan color
            GLHelper::setColor(RGBColor::RED);
            // draw line between end of first shape and first position of second shape
            GLHelper::drawBoxLines({fromLane->getLaneShape().back(), toLane->getLaneShape().front()}, (0.5*E2DetectorWidth));
        }
        // Pop last matrix
        glPopMatrix();
        // Pop name
        glPopName();
        // check if shape dotted contour has to be drawn
        if (s.drawDottedContour() || (myNet->getViewNet()->getInspectedAttributeCarrier() == this)) {
            // draw lane2lane dotted geometry
            if (fromLane->getLane2laneConnections().exist(toLane)) {
                GNEGeometry::drawDottedContourLane(s, fromLane->getLane2laneConnections().getLane2laneDottedGeometry(toLane), E2DetectorWidth, false, false);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// GNEAdditional::BlockIcon - methods
// ---------------------------------------------------------------------------

GNEAdditional::BlockIcon::BlockIcon(GNEAdditional* additional) :
    rotation(0.),
    myAdditional(additional) {}


void
GNEAdditional::BlockIcon::setRotation(GNELane* additionalLane) {
    if (myAdditional->myAdditionalGeometry.getShape().size() > 0 && myAdditional->myAdditionalGeometry.getShape().length() != 0) {
        // If length of the shape is distint to 0, Obtain rotation of center of shape
        rotation = myAdditional->myAdditionalGeometry.getShape().rotationDegreeAtOffset((myAdditional->myAdditionalGeometry.getShape().length() / 2.)) - 90;
    } else if (additionalLane) {
        // If additional is over a lane, set rotation in the position over lane
        double posOverLane = additionalLane->getLaneShape().nearest_offset_to_point2D(myAdditional->getPositionInView());
        rotation = additionalLane->getLaneShape().rotationDegreeAtOffset(posOverLane) - 90;
    } else {
        // In other case, rotation is 0
        rotation = 0;
    }
}


void
GNEAdditional::BlockIcon::drawIcon(const GUIVisualizationSettings& s, const double exaggeration, const double size) const {
    // check if block icon can be draw
    if (!s.drawForRectangleSelection && s.drawDetail(s.detailSettings.lockIcon, exaggeration) && myAdditional->myNet->getViewNet()->showLockIcon()) {
        // Start pushing matrix
        glPushMatrix();
        // Traslate to middle of shape
        glTranslated(position.x(), position.y(), myAdditional->getType() + 0.1);
        // Set draw color
        glColor3d(1, 1, 1);
        // Rotate depending of rotation
        glRotated(rotation, 0, 0, -1);
        // Rotate 180 degrees
        glRotated(180, 0, 0, 1);
        // Traslate depending of the offset
        glTranslated(offset.x(), offset.y(), 0);
        // Draw icon depending of the state of additional
        if (myAdditional->drawUsingSelectColor()) {
            if (!myAdditional->getTagProperty().canBlockMovement()) {
                // Draw not movable texture if additional isn't movable and is selected
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_NOTMOVINGSELECTED), size);
            } else if (myAdditional->myBlockMovement) {
                // Draw lock texture if additional is movable, is blocked and is selected
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_LOCKSELECTED), size);
            } else {
                // Draw empty texture if additional is movable, isn't blocked and is selected
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_EMPTYSELECTED), size);
            }
        } else {
            if (!myAdditional->getTagProperty().canBlockMovement()) {
                // Draw not movable texture if additional isn't movable
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_NOTMOVING), size);
            } else if (myAdditional->myBlockMovement) {
                // Draw lock texture if additional is movable and is blocked
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_LOCK), size);
            } else {
                // Draw empty texture if additional is movable and isn't blocked
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_EMPTY), size);
            }
        }
        // Pop matrix
        glPopMatrix();
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
GNEAdditional::enableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    //
}


void
GNEAdditional::disableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    //
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
