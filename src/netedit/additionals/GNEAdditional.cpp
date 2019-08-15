/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEAdditional.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
/// @version $Id$
///
// A abstract class for representation of additional elements
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/frames/GNESelectorFrame.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/demandelements/GNEDemandElement.h>
#include <utils/common/StringTokenizer.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/options/OptionsCont.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEAdditional.h"

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEAdditional::AdditionalGeometry - methods
// ---------------------------------------------------------------------------

GNEAdditional::AdditionalGeometry::AdditionalGeometry() {}


void
GNEAdditional::AdditionalGeometry::clearGeometry() {
    shape.clear();
    multiShape.clear();
    shapeRotations.clear();
    shapeLengths.clear();
    multiShapeRotations.clear();
    multiShapeLengths.clear();
    multiShapeUnified.clear();
}


void
GNEAdditional::AdditionalGeometry::calculateMultiShapeUnified() {
    // merge all multishape parts in a single shape
    for (auto i : multiShape) {
        multiShapeUnified.append(i);
    }
}


void
GNEAdditional::AdditionalGeometry::calculateShapeRotationsAndLengths() {
    // Get number of parts of the shape
    int numberOfSegments = (int)shape.size() - 1;
    // If number of segments is more than 0
    if (numberOfSegments >= 0) {
        // Reserve memory (To improve efficiency)
        shapeRotations.reserve(numberOfSegments);
        shapeLengths.reserve(numberOfSegments);
        // For every part of the shape
        for (int i = 0; i < numberOfSegments; ++i) {
            // Obtain first position
            const Position& f = shape[i];
            // Obtain next position
            const Position& s = shape[i + 1];
            // Save distance between position into myShapeLengths
            shapeLengths.push_back(f.distanceTo(s));
            // Save rotation (angle) of the vector constructed by points f and s
            shapeRotations.push_back((double)atan2((s.x() - f.x()), (f.y() - s.y())) * (double) 180.0 / (double)M_PI);
        }
    }
}


void
GNEAdditional::AdditionalGeometry::calculateMultiShapeRotationsAndLengths() {
    // Get number of parts of the shape for every part shape
    std::vector<int> numberOfSegments;
    for (auto i : multiShape) {
        // numseg cannot be 0
        int numSeg = (int)i.size() - 1;
        numberOfSegments.push_back((numSeg >= 0) ? numSeg : 0);
        multiShapeRotations.push_back(std::vector<double>());
        multiShapeLengths.push_back(std::vector<double>());
    }
    // If number of segments is more than 0
    for (int i = 0; i < (int)multiShape.size(); i++) {
        // Reserve size for every part
        multiShapeRotations.back().reserve(numberOfSegments.at(i));
        multiShapeLengths.back().reserve(numberOfSegments.at(i));
        // iterate over each segment
        for (int j = 0; j < numberOfSegments.at(i); j++) {
            // Obtain first position
            const Position& f = multiShape[i][j];
            // Obtain next position
            const Position& s = multiShape[i][j + 1];
            // Save distance between position into myShapeLengths
            multiShapeLengths.at(i).push_back(f.distanceTo(s));
            // Save rotation (angle) of the vector constructed by points f and s
            multiShapeRotations.at(i).push_back((double)atan2((s.x() - f.x()), (f.y() - s.y())) * (double) 180.0 / (double)M_PI);
        }
    }
}

// ---------------------------------------------------------------------------
// GNEAdditional - methods
// ---------------------------------------------------------------------------

GNEAdditional::GNEAdditional(const std::string& id, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag, std::string additionalName, bool blockMovement,
                             const std::vector<GNEEdge*>& edgeParents,
                             const std::vector<GNELane*>& laneParents,
                             const std::vector<GNEShape*>& shapeParents,
                             const std::vector<GNEAdditional*>& additionalParents,
                             const std::vector<GNEDemandElement*>& demandElementParents,
                             const std::vector<GNEEdge*>& edgeChildren,
                             const std::vector<GNELane*>& laneChildren,
                             const std::vector<GNEShape*>& shapeChildren,
                             const std::vector<GNEAdditional*>& additionalChildren,
                             const std::vector<GNEDemandElement*>& demandElementChildren) :
    GUIGlObject(type, id),
    GNEAttributeCarrier(tag),
    Parameterised(),
    GNEHierarchicalElementParents(this, edgeParents, laneParents, shapeParents, additionalParents, demandElementParents),
    GNEHierarchicalElementChildren(this, edgeChildren, laneChildren, shapeChildren, additionalChildren, demandElementChildren),
    myViewNet(viewNet),
    myAdditionalName(additionalName),
    myBlockMovement(blockMovement),
    myBlockIcon(this),
    mySpecialColor(nullptr) {
}


GNEAdditional::GNEAdditional(GNEAdditional* additionalParent, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag, std::string additionalName, bool blockMovement,
                             const std::vector<GNEEdge*>& edgeParents,
                             const std::vector<GNELane*>& laneParents,
                             const std::vector<GNEShape*>& shapeParents,
                             const std::vector<GNEAdditional*>& additionalParents,
                             const std::vector<GNEDemandElement*>& demandElementParents,
                             const std::vector<GNEEdge*>& edgeChildren,
                             const std::vector<GNELane*>& laneChildren,
                             const std::vector<GNEShape*>& shapeChildren,
                             const std::vector<GNEAdditional*>& additionalChildren,
                             const std::vector<GNEDemandElement*>& demandElementChildren) :
    GUIGlObject(type, additionalParent->generateChildID(tag)),
    GNEAttributeCarrier(tag),
    Parameterised(),
    GNEHierarchicalElementParents(this, edgeParents, laneParents, shapeParents, additionalParents, demandElementParents),
    GNEHierarchicalElementChildren(this, edgeChildren, laneChildren, shapeChildren, additionalChildren, demandElementChildren),
    myViewNet(viewNet),
    myAdditionalName(additionalName),
    myBlockMovement(blockMovement),
    myBlockIcon(this),
    mySpecialColor(nullptr) {
}


GNEAdditional::~GNEAdditional() {}


std::string
GNEAdditional::generateChildID(SumoXMLTag childTag) {
    int counter = (int)getAdditionalChildren().size();
    while (myViewNet->getNet()->retrieveAdditional(childTag, getID() + toString(childTag) + toString(counter), false) != nullptr) {
        counter++;
    }
    return (getID() + toString(childTag) + toString(counter));
}


const GNEAdditional::AdditionalGeometry&
GNEAdditional::getAdditionalGeometry() const {
    return myGeometry;
}


void
GNEAdditional::setSpecialColor(const RGBColor* color) {
    mySpecialColor = color;
}


void
GNEAdditional::writeAdditional(OutputDevice& device) const {
    // first check if minimum number of children is correct
    if ((myTagProperty.hasMinimumNumberOfChildren() || myTagProperty.hasMinimumNumberOfChildren()) && !checkAdditionalChildRestriction()) {
        WRITE_WARNING(getTagStr() + " with ID='" + getID() + "' cannot be written");
    } else {
        // Open Tag or synonym Tag
        if (myTagProperty.hasTagSynonym()) {
            device.openTag(myTagProperty.getTagSynonym());
        } else {
            device.openTag(myTagProperty.getTag());
        }
        // iterate over attributes and write it
        for (auto i : myTagProperty) {
            // obtain attribute
            std::string attribute = getAttribute(i.getAttr());
            if (i.isWriteXMLOptional() && !i.isCombinable()) {
                // Only write attributes with default value if is different from original
                if (i.getDefaultValue() != attribute) {
                    // check if attribute must be written using a synonim
                    if (i.hasAttrSynonym()) {
                        device.writeAttr(i.getAttrSynonym(), attribute);
                    } else {
                        // SVC permissions uses their own writting function
                        if (i.isSVCPermission()) {
                            // disallow attribute musn't be written
                            if (i.getAttr() != SUMO_ATTR_DISALLOW) {
                                writePermissions(device, parseVehicleClasses(attribute));
                            }
                        } else if (myTagProperty.canMaskXYZPositions() && (i.getAttr() == SUMO_ATTR_POSITION)) {
                            // get position attribute and write it separate
                            Position pos = parse<Position>(getAttribute(SUMO_ATTR_POSITION));
                            device.writeAttr(SUMO_ATTR_X, toString(pos.x()));
                            device.writeAttr(SUMO_ATTR_Y, toString(pos.y()));
                            // write 0 only if is different from 0 (the default value)
                            if (pos.z() != 0) {
                                device.writeAttr(SUMO_ATTR_Z, toString(pos.z()));
                            }
                        } else {
                            device.writeAttr(i.getAttr(), attribute);
                        }
                    }
                }
            } else {
                // Attributes without default values are always writted
                if (i.hasAttrSynonym()) {
                    device.writeAttr(i.getAttrSynonym(), attribute);
                } else {
                    // SVC permissions uses their own writting function
                    if (i.isSVCPermission()) {
                        // disallow attribute musn't be written
                        if (i.getAttr() != SUMO_ATTR_DISALLOW) {
                            writePermissions(device, parseVehicleClasses(attribute));
                        }
                    } else if (myTagProperty.canMaskXYZPositions() && (i.getAttr() == SUMO_ATTR_POSITION)) {
                        // get position attribute and write it separate
                        Position pos = parse<Position>(getAttribute(SUMO_ATTR_POSITION));
                        device.writeAttr(SUMO_ATTR_X, toString(pos.x()));
                        device.writeAttr(SUMO_ATTR_Y, toString(pos.y()));
                        // write 0 only if is different from 0 (the default value)
                        if (pos.z() != 0) {
                            device.writeAttr(SUMO_ATTR_Z, toString(pos.z()));
                        }
                    } else {
                        device.writeAttr(i.getAttr(), attribute);
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
            for (auto i : getAdditionalChildren()) {
                // avoid to write two times additionals that haben two parents (Only write as child of first parent)
                if (i->getAdditionalParents().size() < 1) {
                    i->writeAdditional(deviceChildren);
                } else if (myTagProperty.getTag() == i->getTagProperty().getParentTag()) {
                    i->writeAdditional(deviceChildren);
                }
            }
            deviceChildren.close();
        } else {
            for (auto i : getAdditionalChildren()) {
                // avoid to write two times additionals that haben two parents (Only write as child of first parent)
                if (i->getAdditionalParents().size() < 2) {
                    i->writeAdditional(device);
                } else if (myTagProperty.getTag() == i->getTagProperty().getParentTag()) {
                    i->writeAdditional(device);
                }
            }
        }
        // save generic parameters (Always after children to avoid problems with additionals.xsd)
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
        for (const auto& i : getDemandElementChildren()) {
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
            myViewNet->getNet()->removeGLObjectFromGrid(this);
            // reset myMovingGeometryBoundary
            myMove.movingGeometryBoundary.reset();
            // add object into grid again (using the new centering boundary)
            myViewNet->getNet()->addGLObjectIntoGrid(this);
        }
        // end geometry in all children
        for (const auto& i : getDemandElementChildren()) {
            i->endGeometryMoving();
        }
    }
}


GNEViewNet*
GNEAdditional::getViewNet() const {
    return myViewNet;
}


PositionVector
GNEAdditional::getShape() const {
    return myGeometry.shape;
}


bool
GNEAdditional::isAdditionalBlocked() const {
    return myBlockMovement;
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
    myViewNet->buildSelectionACPopupEntry(ret, this);
    buildShowParamsPopupEntry(ret);
    // show option to open additional dialog
    if (myTagProperty.hasDialog()) {
        new FXMenuCommand(ret, ("Open " + getTagStr() + " Dialog").c_str(), getIcon(), &parent, MID_OPEN_ADDITIONAL_DIALOG);
        new FXMenuSeparator(ret);
    }
    // Show position parameters
    if (myTagProperty.hasAttribute(SUMO_ATTR_LANE)) {
        GNELane* lane = myViewNet->getNet()->retrieveLane(getAttribute(SUMO_ATTR_LANE));
        // Show menu command inner position
        const double innerPos = myGeometry.shape.nearest_offset_to_point2D(parent.getPositionInformation());
        new FXMenuCommand(ret, ("Cursor position inner additional: " + toString(innerPos)).c_str(), nullptr, nullptr, 0);
        // If shape isn't empty, show menu command lane position
        if (myGeometry.shape.size() > 0) {
            const double lanePos = lane->getGeometry().shape.nearest_offset_to_point2D(myGeometry.shape[0]);
            new FXMenuCommand(ret, ("Cursor position over " + toString(SUMO_TAG_LANE) + ": " + toString(innerPos + lanePos)).c_str(), nullptr, nullptr, 0);
        }
    } else if (myTagProperty.hasAttribute(SUMO_ATTR_EDGE)) {
        GNEEdge* edge = myViewNet->getNet()->retrieveEdge(getAttribute(SUMO_ATTR_EDGE));
        // Show menu command inner position
        const double innerPos = myGeometry.shape.nearest_offset_to_point2D(parent.getPositionInformation());
        new FXMenuCommand(ret, ("Cursor position inner additional: " + toString(innerPos)).c_str(), nullptr, nullptr, 0);
        // If shape isn't empty, show menu command edge position
        if (myGeometry.shape.size() > 0) {
            const double edgePos = edge->getLanes().at(0)->getGeometry().shape.nearest_offset_to_point2D(myGeometry.shape[0]);
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
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this, myTagProperty.getNumberOfAttributes());
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

/*
Boundary
GNEAdditional::getCenteringBoundary() const {
    // Return Boundary depending if myMovingGeometryBoundary is initialised (important for move geometry)
    if (myMove.movingGeometryBoundary.isInitialised()) {
        return myMove.movingGeometryBoundary;
    } else if (myGeometry.shape.size() > 0) {
        Boundary b = myGeometry.shape.getBoxBoundary();
        b.grow(20);
        return b;
    } else if (myGeometry.multiShape.size() > 0) {
        // obtain boundary of multishape fixed
        Boundary b = myGeometry.multiShapeUnified.getBoxBoundary();
        b.grow(20);
        return b;
    } else if (getAdditionalParents().size() > 0) {
        return getAdditionalParents().at(0)->getCenteringBoundary();
    } else {
        return Boundary(-0.1, -0.1, 0.1, 0.1);
    }
}
*/
// ---------------------------------------------------------------------------
// GNEAdditional::BlockIcon - methods
// ---------------------------------------------------------------------------

GNEAdditional::BlockIcon::BlockIcon(GNEAdditional* additional) :
    rotation(0.),
    myAdditional(additional) {}


void
GNEAdditional::BlockIcon::setRotation(GNELane* additionalLane) {
    if (myAdditional->myGeometry.shape.size() > 0 && myAdditional->myGeometry.shape.length() != 0) {
        // If length of the shape is distint to 0, Obtain rotation of center of shape
        rotation = myAdditional->myGeometry.shape.rotationDegreeAtOffset((myAdditional->myGeometry.shape.length() / 2.)) - 90;
    } else if (additionalLane) {
        // If additional is over a lane, set rotation in the position over lane
        double posOverLane = additionalLane->getGeometry().shape.nearest_offset_to_point2D(myAdditional->getPositionInView());
        rotation = additionalLane->getGeometry().shape.rotationDegreeAtOffset(posOverLane) - 90;
    } else {
        // In other case, rotation is 0
        rotation = 0;
    }
}


void
GNEAdditional::BlockIcon::drawIcon(const GUIVisualizationSettings& s, const double exaggeration, const double size) const {
    // check if block icon can be draw
    if (!s.drawForSelecting && s.drawDetail(s.detailSettings.lockIcon, exaggeration) && myAdditional->myViewNet->showLockIcon()) {
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


const std::string&
GNEAdditional::getAdditionalID() const {
    return getMicrosimID();
}


bool
GNEAdditional::isValidAdditionalID(const std::string& newID) const {
    if (SUMOXMLDefinitions::isValidNetID(newID) && (myViewNet->getNet()->retrieveAdditional(myTagProperty.getTag(), newID, false) == nullptr)) {
        return true;
    } else {
        return false;
    }
}


bool
GNEAdditional::isValidDetectorID(const std::string& newID) const {
    if (SUMOXMLDefinitions::isValidDetectorID(newID) && (myViewNet->getNet()->retrieveAdditional(myTagProperty.getTag(), newID, false) == nullptr)) {
        return true;
    } else {
        return false;
    }
}


void
GNEAdditional::changeAdditionalID(const std::string& newID) {
    if (myViewNet->getNet()->retrieveAdditional(myTagProperty.getTag(), newID, false) != nullptr) {
        throw InvalidArgument("An Additional with tag " + getTagStr() + " and ID = " + newID + " already exists");
    } else {
        // Save old ID
        std::string oldID = getMicrosimID();
        // set New ID
        setMicrosimID(newID);
        // update additional ID in the container of net
        myViewNet->getNet()->updateAdditionalID(oldID, this);
    }
}


void
GNEAdditional::selectAttributeCarrier(bool changeFlag) {
    if (!myViewNet) {
        throw ProcessError("ViewNet cannot be nullptr");
    } else {
        gSelected.select(getGlID());
        // add object of list into selected objects
        myViewNet->getViewParent()->getSelectorFrame()->getLockGLObjectTypes()->addedLockedObject(GLO_ADDITIONAL);
        if (changeFlag) {
            mySelected = true;
        }
    }
}


void
GNEAdditional::unselectAttributeCarrier(bool changeFlag) {
    if (!myViewNet) {
        throw ProcessError("ViewNet cannot be nullptr");
    } else {
        gSelected.deselect(getGlID());
        // remove object of list of selected objects
        myViewNet->getViewParent()->getSelectorFrame()->getLockGLObjectTypes()->removeLockedObject(GLO_ADDITIONAL);
        if (changeFlag) {
            mySelected = false;

        }
    }
}


bool
GNEAdditional::isAttributeCarrierSelected() const {
    return mySelected;
}


bool
GNEAdditional::drawUsingSelectColor() const {
    if (mySelected && (myViewNet->getEditModes().currentSupermode == GNE_SUPERMODE_NETWORK)) {
        return true;
    } else {
        return false;
    }
}


void
GNEAdditional::enableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    //
}


bool
GNEAdditional::isAttributeEnabled(SumoXMLAttr /*key*/) const {
    return true;
}


std::string
GNEAdditional::getGenericParametersStr() const {
    std::string result;
    // Generate an string using the following structure: "key1=value1|key2=value2|...
    for (auto i : getParametersMap()) {
        result += i.first + "=" + i.second + "|";
    }
    // remove the last "|"
    if (!result.empty()) {
        result.pop_back();
    }
    return result;
}


std::vector<std::pair<std::string, std::string> >
GNEAdditional::getGenericParameters() const {
    std::vector<std::pair<std::string, std::string> >  result;
    // iterate over parameters map and fill result
    for (auto i : getParametersMap()) {
        result.push_back(std::make_pair(i.first, i.second));
    }
    return result;
}


void
GNEAdditional::setGenericParametersStr(const std::string& value) {
    // clear parameters
    clearParameter();
    // separate value in a vector of string using | as separator
    std::vector<std::string> parsedValues;
    StringTokenizer stValues(value, "|", true);
    while (stValues.hasNext()) {
        parsedValues.push_back(stValues.next());
    }
    // check that parsed values (A=B)can be parsed in generic parameters
    for (auto i : parsedValues) {
        std::vector<std::string> parsedParameters;
        StringTokenizer stParam(i, "=", true);
        while (stParam.hasNext()) {
            parsedParameters.push_back(stParam.next());
        }
        // Check that parsed parameters are exactly two and contains valid chracters
        if (parsedParameters.size() == 2 && SUMOXMLDefinitions::isValidGenericParameterKey(parsedParameters.front()) && SUMOXMLDefinitions::isValidGenericParameterValue(parsedParameters.back())) {
            setParameter(parsedParameters.front(), parsedParameters.back());
        }
    }
}


bool
GNEAdditional::checkAdditionalChildRestriction() const {
    // throw exception because this function mus be implemented in child (see GNEE3Detector)
    throw ProcessError("Calling non-implemented function checkAdditionalChildRestriction during saving of " + getTagStr() + ". It muss be reimplemented in child class");
}


void
GNEAdditional::setEnabledAttribute(const int /*enabledAttributes*/) {
    //
}

/****************************************************************************/
