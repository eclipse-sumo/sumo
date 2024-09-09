/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    GNENetworkElement.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
///
// A abstract class for networkElements
/****************************************************************************/

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/frames/common/GNESelectorFrame.h>
#include <utils/foxtools/MFXMenuHeader.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/options/OptionsCont.h>

#include "GNENetworkElement.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNENetworkElement::GNENetworkElement(GNENet* net, const std::string& id, GUIGlObjectType type, SumoXMLTag tag, FXIcon* icon,
                                     const std::vector<GNEJunction*>& junctionParents,
                                     const std::vector<GNEEdge*>& edgeParents,
                                     const std::vector<GNELane*>& laneParents,
                                     const std::vector<GNEAdditional*>& additionalParents,
                                     const std::vector<GNEDemandElement*>& demandElementParents,
                                     const std::vector<GNEGenericData*>& genericDataParents) :
    GUIGlObject(type, id, icon),
    GNEHierarchicalElement(net, tag, junctionParents, edgeParents, laneParents, additionalParents, demandElementParents, genericDataParents),
    myShapeEdited(false) {
}


GNENetworkElement::~GNENetworkElement() {}


GUIGlObject*
GNENetworkElement::getGUIGlObject() {
    return this;
}


const GUIGlObject*
GNENetworkElement::getGUIGlObject() const {
    return this;
}


bool
GNENetworkElement::GNENetworkElement::isNetworkElementValid() const {
    // implement in children
    return true;
}


std::string
GNENetworkElement::GNENetworkElement::getNetworkElementProblem() const {
    // implement in children
    return "";
}


GUIParameterTableWindow*
GNENetworkElement::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView&) {
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


bool
GNENetworkElement::isGLObjectLocked() const {
    if (myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork()) {
        return myNet->getViewNet()->getLockManager().isObjectLocked(getType(), isAttributeCarrierSelected());
    } else {
        return true;
    }
}


void
GNENetworkElement::markAsFrontElement() {
    myNet->getViewNet()->setFrontAttributeCarrier(this);
}


void
GNENetworkElement::selectGLObject() {
    if (isAttributeCarrierSelected()) {
        unselectAttributeCarrier();
    } else {
        selectAttributeCarrier();
    }
    // update information label
    myNet->getViewNet()->getViewParent()->getSelectorFrame()->getSelectionInformation()->updateInformationLabel();
}


const std::string
GNENetworkElement::getOptionalName() const {
    try {
        return getAttribute(SUMO_ATTR_NAME);
    } catch (InvalidArgument&) {
        return "";
    }
}


std::string
GNENetworkElement::getPopUpID() const {
    if (myTagProperty.getTag() == SUMO_TAG_CONNECTION) {
        return getAttribute(SUMO_ATTR_FROM) + "_" + getAttribute(SUMO_ATTR_FROM_LANE) + " -> " + getAttribute(SUMO_ATTR_TO) + "_" + getAttribute(SUMO_ATTR_TO_LANE);
    } else {
        return getTagStr() + ": " + getID();
    }
}


std::string
GNENetworkElement::getHierarchyName() const {
    if (myTagProperty.getTag() == SUMO_TAG_LANE) {
        return toString(SUMO_TAG_LANE) + " " + getAttribute(SUMO_ATTR_INDEX);
    } else if (myTagProperty.getTag() == SUMO_TAG_CONNECTION) {
        return getAttribute(SUMO_ATTR_FROM_LANE) + " -> " + getAttribute(SUMO_ATTR_TO_LANE);
    } else if ((myTagProperty.getTag() == SUMO_TAG_EDGE) || (myTagProperty.getTag() == SUMO_TAG_CROSSING)) {
        return getPopUpID();
    } else {
        return getTagStr();
    }
}


void
GNENetworkElement::setShapeEdited(const bool value) {
    myShapeEdited = value;
}


bool
GNENetworkElement::isShapeEdited() const {
    return myShapeEdited;
}


int
GNENetworkElement::getGeometryPointUnderCursorShapeEdited() const {
    const auto& s = myNet->getViewNet()->getVisualisationSettings();
    // calculate squared geometry point radius depending of edited item
    double geometryPointRadius = s.neteditSizeSettings.polygonGeometryPointRadius;
    if (myTagProperty.getTag() == SUMO_TAG_JUNCTION) {
        geometryPointRadius = s.neteditSizeSettings.junctionGeometryPointRadius;
    } else if (myTagProperty.getTag() == SUMO_TAG_EDGE) {
        geometryPointRadius = s.neteditSizeSettings.edgeGeometryPointRadius;
    } else if (myTagProperty.getTag() == SUMO_TAG_LANE) {
        geometryPointRadius = s.neteditSizeSettings.laneGeometryPointRadius;
    } else if (myTagProperty.getTag() == SUMO_TAG_CONNECTION) {
        geometryPointRadius = s.neteditSizeSettings.connectionGeometryPointRadius;
    } else if (myTagProperty.getTag() == SUMO_TAG_CROSSING) {
        geometryPointRadius = s.neteditSizeSettings.crossingGeometryPointRadius;
    }
    const auto geometryPointRadiusSquared = (geometryPointRadius * geometryPointRadius);
    const auto shape = getAttributePositionVector(SUMO_ATTR_SHAPE);
    const auto mousePos = myNet->getViewNet()->getPositionInformation();
    for (int i = 0; i < (int)shape.size(); i++) {
        if (shape[i].distanceSquaredTo2D(mousePos) < geometryPointRadiusSquared) {
            return i;
        }
    }
    return -1;
}


void
GNENetworkElement::simplifyShapeEdited(GNEUndoList* undoList) {
    auto shape = getAttributePositionVector(SUMO_ATTR_SHAPE);
    const Boundary b = shape.getBoxBoundary();
    // create a square as simplified shape
    PositionVector simplifiedShape;
    simplifiedShape.push_back(Position(b.xmin(), b.ymin()));
    simplifiedShape.push_back(Position(b.xmin(), b.ymax()));
    simplifiedShape.push_back(Position(b.xmax(), b.ymax()));
    simplifiedShape.push_back(Position(b.xmax(), b.ymin()));
    if (shape.isClosed()) {
        simplifiedShape.push_back(simplifiedShape[0]);
    }
    setAttribute(SUMO_ATTR_SHAPE, toString(simplifiedShape), undoList);
}


void
GNENetworkElement::closeShapeEdited(GNEUndoList* undoList) {
    auto shape = getAttributePositionVector(SUMO_ATTR_SHAPE);
    shape.closePolygon();
    setAttribute(SUMO_ATTR_SHAPE, toString(shape), undoList);
}


void
GNENetworkElement::openShapeEdited(GNEUndoList* undoList) {
    auto shape = getAttributePositionVector(SUMO_ATTR_SHAPE);
    shape.pop_back();
    setAttribute(SUMO_ATTR_SHAPE, toString(shape), undoList);
}


void
GNENetworkElement::setFirstGeometryPointShapeEdited(int index, GNEUndoList* undoList) {
    const auto shape = getAttributePositionVector(SUMO_ATTR_SHAPE);
    PositionVector newShape;
    for (int i = index; i < (int)shape.size(); i++) {
        newShape.push_back(shape[i]);
    }
    for (int i = 0; i < index; i++) {
        newShape.push_back(shape[i]);
    }
    setAttribute(SUMO_ATTR_SHAPE, toString(newShape), undoList);
}


void
GNENetworkElement::deleteGeometryPointShapeEdited(int index, GNEUndoList* undoList) {
    const auto shape = getAttributePositionVector(SUMO_ATTR_SHAPE);
    PositionVector newShape;
    for (int i = 0; i < (int)shape.size(); i++) {
        if (i != index) {
            newShape.push_back(shape[i]);
        }
    }
    setAttribute(SUMO_ATTR_SHAPE, toString(newShape), undoList);
}


void
GNENetworkElement::resetShapeEdited(GNEUndoList* /*undoList*/) {

}


void
GNENetworkElement::setNetworkElementID(const std::string& newID) {
    // set microsim ID
    setMicrosimID(newID);
    // enable save add elements if this network element has children
    if (getChildAdditionals().size() > 0) {
        myNet->getSavingStatus()->requireSaveAdditionals();
    }
    // enable save demand elements if this network element has children
    if (getChildDemandElements().size() > 0) {
        myNet->getSavingStatus()->requireSaveDemandElements();
    }
    // enable save data elements if this network element has children
    if (getChildGenericDatas().size() > 0) {
        myNet->getSavingStatus()->requireSaveDataElements();
    }
}


bool
GNENetworkElement::checkDrawingBoundarySelection() const {
    if (!gViewObjectsHandler.getSelectionBoundary().isInitialised()) {
        return true;
    } else if (!gViewObjectsHandler.isElementSelected(this)) {
        return true;
    } else {
        return false;
    }
}


GUIGLObjectPopupMenu*
GNENetworkElement::getShapeEditedPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent, const PositionVector& shape) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    const std::string headerName = TLF("% (Edited shape)", getFullName());
    new MFXMenuHeader(ret, app.getBoldFont(), headerName.c_str(), getGLIcon(), nullptr, 0);
    if (OptionsCont::getOptions().getBool("gui-testing")) {
        GUIDesigns::buildFXMenuCommand(ret, TL("Copy test coordinates to clipboard"), nullptr, ret, MID_COPY_TEST_COORDINATES);
    }
    new FXMenuSeparator(ret);
    FXMenuCommand* simplifyShape = GUIDesigns::buildFXMenuCommand(ret, TL("Simplify Shape"), TL("Replace current shape with a rectangle"), nullptr, &parent, MID_GNE_SHAPEEDITED_SIMPLIFY);
    // disable simplify shape if polygon was already simplified
    if (shape.size() <= 2) {
        simplifyShape->disable();
    }
    // only allow open/close for junctions
    if (myTagProperty.getTag() == SUMO_TAG_JUNCTION) {
        if (shape.isClosed()) {
            GUIDesigns::buildFXMenuCommand(ret, TL("Open shape"), TL("Open junction's shape"), nullptr, &parent, MID_GNE_SHAPEEDITED_OPEN);
        } else {
            GUIDesigns::buildFXMenuCommand(ret, TL("Close shape"), TL("Close junction's shape"), nullptr, &parent, MID_GNE_SHAPEEDITED_CLOSE);
        }
    }
    // create a extra FXMenuCommand if mouse is over a vertex
    const int index = getGeometryPointUnderCursorShapeEdited();
    if (index != -1) {
        FXMenuCommand* removeGeometryPoint = GUIDesigns::buildFXMenuCommand(ret, TL("Remove geometry point (shift+click)"), TL("Remove geometry point under mouse"), nullptr, &parent, MID_GNE_SHAPEEDITED_DELETE_GEOMETRY_POINT);
        FXMenuCommand* setFirstPoint = GUIDesigns::buildFXMenuCommand(ret, TL("Set first geometry point"), TL("Set first geometry point"), nullptr, &parent, MID_GNE_SHAPEEDITED_SET_FIRST_POINT);
        // disable setFirstPoint if shape only have three points
        if ((shape.isClosed() && (shape.size() <= 4)) || (!shape.isClosed() && (shape.size() <= 2))) {
            removeGeometryPoint->disable();
        }
        // disable setFirstPoint if mouse is over first point
        if (index == 0) {
            setFirstPoint->disable();
        }
    }
    return ret;
}


int
GNENetworkElement::getVertexIndex(const PositionVector& shape, const Position& pos) {
    // first check if vertex already exists
    for (const auto& shapePosition : shape) {
        if (shapePosition.distanceTo2D(pos) < myNet->getViewNet()->getVisualisationSettings().neteditSizeSettings.polygonGeometryPointRadius) {
            return shape.indexOfClosest(shapePosition);
        }
    }
    return -1;
}

/****************************************************************************/
