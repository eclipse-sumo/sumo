/****************************************************************************/
/// @file    GNEAdditional.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
/// @version $Id$
///
/// A abstract class for representation of additional elements
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream>
#include <utility>
#include <foreign/polyfonts/polyfonts.h>
#include <utils/geom/PositionVector.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/xml/SUMOSAXHandler.h>

#include "GNEAdditional.h"
#include "GNELane.h"
#include "GNEEdge.h"
#include "GNENet.h"
#include "GNEUndoList.h"
#include "GNEViewNet.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// member method definitions
// ===========================================================================

GNEAdditional::GNEAdditional(const std::string& id, GNEViewNet* viewNet, Position pos, SumoXMLTag tag, GUIIcon icon) :
    GUIGlObject(GLO_ADDITIONAL, id),
    GNEAttributeCarrier(tag, icon),
    myViewNet(viewNet),
    myEdge(NULL),
    myLane(NULL),
    myPosition(pos),
    myBlockIconRotation(0),
    myBlocked(false),
    myInspectionable(true),
    mySelectable(true),
    myMovable(true),
    myBaseColor(RGBColor::GREEN),
    myBaseColorSelected(RGBColor::BLUE),
    myAdditionalDialog(NULL) {
    // Set rotation left hand
    myRotationLefthand = OptionsCont::getOptions().getBool("lefthand");
}


GNEAdditional::~GNEAdditional() {}


void
GNEAdditional::moveAdditionalGeometry(const Position& offset) {
    moveAdditionalGeometry(offset.x(), offset.y());
}


void
GNEAdditional::commmitAdditionalGeometryMoved(const Position& oldPos, GNEUndoList* undoList) {
    commmitAdditionalGeometryMoved(oldPos.x(), oldPos.y(), undoList);
    // Refresh element
    myViewNet->getNet()->refreshAdditional(this);
}


void
GNEAdditional::openAdditionalDialog() {}


const std::string&
GNEAdditional::getAdditionalID() const {
    return getMicrosimID();
}


GNEViewNet*
GNEAdditional::getViewNet() const {
    return myViewNet;
}


PositionVector
GNEAdditional::getShape() const {
    return myShape;
}


bool
GNEAdditional::isAdditionalBlocked() const {
    return myBlocked;
}


bool
GNEAdditional::isAdditionalInspectionable() const {
    return myInspectionable;
}


bool
GNEAdditional::isAdditionalSelectable() const {
    return mySelectable;
}


bool
GNEAdditional::isAdditionalMovable() const {
    return myMovable;
}


bool
GNEAdditional::isAdditionalSelected() const {
    return gSelected.isSelected(getType(), getGlID());
}


void
GNEAdditional::setAdditionalID(const std::string& id) {
    // Save old ID
    std::string oldID = getMicrosimID();
    // set New ID
    setMicrosimID(id);
    // update additional ID in the container of net
    myViewNet->getNet()->updateAdditionalID(oldID, this);
}

void
GNEAdditional::setPositionInView(const Position& pos) {
    myPosition = pos;
}


GNEEdge*
GNEAdditional::getEdge() const {
    return myEdge;
}


GNELane*
GNEAdditional::getLane() const {
    return myLane;
}

const std::string&
GNEAdditional::getParentName() const {
    return myViewNet->getNet()->getMicrosimID();
}


GUIGLObjectPopupMenu*
GNEAdditional::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    // build header
    buildPopupHeader(ret, app);
    // build menu command for center button
    buildCenterPopupEntry(ret);
    // buld menu commands for names
    new FXMenuCommand(ret, ("Copy " + toString(getTag()) + " name to clipboard").c_str(), 0, ret, MID_COPY_NAME);
    new FXMenuCommand(ret, ("Copy " + toString(getTag()) + " typed name to clipboard").c_str(), 0, ret, MID_COPY_TYPED_NAME);
    // build menu command selection
    buildSelectionPopupEntry(ret);
    // build menu command copy cursor position to clipboard
    buildPositionCopyEntry(ret, false);
    buildShowParamsPopupEntry(ret, false);
    // get attributes
    std::vector<SumoXMLAttr> attributes = getAttrs();
    // Show position parameters
    if (std::find(attributes.begin(), attributes.end(), SUMO_ATTR_LANE) != attributes.end()) {
        // If additional own an lane as attribute, get lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(getParentName(), false);
        if (lane) {
            // Show menu command inner position
            const SUMOReal innerPos = myShape.nearest_offset_to_point2D(parent.getPositionInformation());
            new FXMenuCommand(ret, ("inner position: " + toString(innerPos)).c_str(), 0, 0, 0);
            // If shape isn't empty, show menu command lane position
            if (myShape.size() > 0) {
                const SUMOReal lanePos = lane->getShape().nearest_offset_to_point2D(myShape[0]);
                new FXMenuCommand(ret, ("position over " + toString(SUMO_TAG_LANE) + ": " + toString(innerPos + lanePos)).c_str(), 0, 0, 0);
            }
        } else {
            throw InvalidArgument(toString(getTag()) + " with ID '" + getMicrosimID() + "' doesn't have their lane as a ParentName()");
        }
    } else if (std::find(attributes.begin(), attributes.end(), SUMO_ATTR_EDGE) != attributes.end()) {
        // If additional own an edge as attribute, get lane
        GNEEdge* edge = myViewNet->getNet()->retrieveEdge(getParentName(), false);
        if (edge) {
            // Show menu command inner position
            const SUMOReal innerPos = myShape.nearest_offset_to_point2D(parent.getPositionInformation());
            new FXMenuCommand(ret, ("inner position: " + toString(innerPos)).c_str(), 0, 0, 0);
            // If shape isn't empty, show menu command edge position
            if (myShape.size() > 0) {
                const SUMOReal edgePos = edge->getLanes().at(0)->getShape().nearest_offset_to_point2D(myShape[0]);
                new FXMenuCommand(ret, ("position over " + toString(SUMO_TAG_LANE) + ": " + toString(innerPos + edgePos)).c_str(), 0, 0, 0);
            }
        } else {
            throw InvalidArgument(toString(getTag()) + " with ID '" + getMicrosimID() + "' don't have their edge as a ParentName()");
        }
    } else {
        new FXMenuCommand(ret, ("position in view: " + toString(myPosition.x()) + "," + toString(myPosition.y())).c_str(), 0, 0, 0);
    }
    new FXMenuSeparator(ret);
    // let the GNEViewNet store the popup position
    dynamic_cast<GNEViewNet&>(parent).markPopupPosition();
    return ret;
}


GUIParameterTableWindow*
GNEAdditional::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    // Ignore Warning
    UNUSED_PARAMETER(parent);
    // get attributes
    std::vector<SumoXMLAttr> attributes = getAttrs();
    // Create table
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this, (int)attributes.size());
    // Iterate over attributes
    for (std::vector<SumoXMLAttr>::iterator i = attributes.begin(); i != attributes.end(); i++) {
        // Add attribute and set it dynamic if aren't unique
        if (GNEAttributeCarrier::isUnique(getTag(), *i)) {
            ret->mkItem(toString(*i).c_str(), false, getAttribute(*i));
        } else {
            ret->mkItem(toString(*i).c_str(), true, getAttribute(*i));
        }
    }
    /** @TODO complet with the rest of parameters **/
    // close building
    ret->closeBuilding();
    return ret;
}


Boundary
GNEAdditional::getCenteringBoundary() const {
    Boundary b = myShape.getBoxBoundary();
    b.grow(20);
    return b;
}


void
GNEAdditional::setBlockIconRotation(GNELane* lane) {
    if (myShape.size() > 0 && myShape.length() != 0) {
        // If lenght of the shape is distint to 0, Obtain rotation of center of shape
        myBlockIconRotation = myShape.rotationDegreeAtOffset((myShape.length() / 2.)) - 90;
    } else if (lane != NULL) {
        // If additional is over a lane, set rotation in the position over lane
        myBlockIconRotation = lane->getShape().rotationDegreeAtOffset(lane->getPositionRelativeToParametricLenght(myPosition.x())) - 90;
    } else {
        // In other case, rotation is 0
        myBlockIconRotation = 0;
    }
}


void
GNEAdditional::drawLockIcon(SUMOReal size) const {
    if (myViewNet->showLockIcon()) {
        // Start pushing matrix
        glPushMatrix();
        // Traslate to middle of shape
        glTranslated(myBlockIconPosition.x(), myBlockIconPosition.y(), getType() + 0.1);
        // Set draw color
        glColor3d(1, 1, 1);
        // Rotate depending of myBlockIconRotation
        glRotated(myBlockIconRotation, 0, 0, -1);
        // Rotate 180º
        glRotated(180, 0, 0, 1);
        // Traslate depending of the offset
        glTranslated(myBlockIconOffset.x(), myBlockIconOffset.y(), 0);
        // Draw icon depending of the state of additional
        if (isAdditionalSelected()) {
            if (myMovable == false) {
                // Draw not movable texture if additional isn't movable and is selected
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_NOTMOVINGSELECTED), size);
            } else if (myBlocked) {
                // Draw lock texture if additional is movable, is blocked and is selected
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_LOCKSELECTED), size);
            } else {
                // Draw empty texture if additional is movable, isn't blocked and is selected
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_EMPTYSELECTED), size);
            }
        } else {
            if (myMovable == false) {
                // Draw not movable texture if additional isn't movable
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_NOTMOVING), size);
            } else if (myBlocked) {
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


void
GNEAdditional::drawParentAndChildrenConnections() const {
    // Iterate over myConnectionPositions
    for (std::vector<std::vector<Position> >::const_iterator i = myConnectionPositions.begin(); i != myConnectionPositions.end(); i++) {
        // Add a draw matrix
        glPushMatrix();
        // traslate in the Z axis
        glTranslated(0, 0, getType() - 0.01);
        // Set color of the base
        GLHelper::setColor(RGBColor(255, 235, 0, 255));
        for (std::vector<Position>::const_iterator j = (*i).begin(); (j + 1) != (*i).end(); j++) {
            // Draw Lines
            GLHelper::drawLine((*j), (*(j + 1)));
        }
        // Pop draw matrix
        glPopMatrix();
    }
}


void
GNEAdditional::changeEdge(const std::string& edgeID) {
    if (myEdge == NULL) {
        throw InvalidArgument(toString(getTag()) + " with ID '" + getMicrosimID() + "' doesn't belong to an " + toString(SUMO_TAG_EDGE));
    } else {
        myEdge->removeAdditionalChild(this);
        myEdge = getViewNet()->getNet()->retrieveEdge(edgeID);
        myEdge->addAdditionalChild(this);
        updateGeometry();
        getViewNet()->update();
    }
}


void
GNEAdditional::changeLane(const std::string& laneID) {
    if (myLane == NULL) {
        throw InvalidArgument(toString(getTag()) + " with ID '" + getMicrosimID() + "' doesn't belong to a " + toString(SUMO_TAG_LANE));
    } else {
        myLane->removeAdditionalChild(this);
        myLane = getViewNet()->getNet()->retrieveLane(laneID);
        myLane->addAdditionalChild(this);
        updateGeometry();
        getViewNet()->update();
    }
}

/****************************************************************************/
