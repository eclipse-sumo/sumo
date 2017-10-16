/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2017 German Aerospace Center (DLR) and others.
/****************************************************************************/
//
//   This program and the accompanying materials
//   are made available under the terms of the Eclipse Public License v2.0
//   which accompanies this distribution, and is available at
//   http://www.eclipse.org/legal/epl-v20.html
//
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
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream>
#include <utility>
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


// ===========================================================================
// member method definitions
// ===========================================================================

GNEAdditional::GNEAdditional(const std::string& id, GNEViewNet* viewNet, SumoXMLTag tag, GUIIcon icon) :
    GUIGlObject(GLO_ADDITIONAL, id),
    GNEAttributeCarrier(tag, icon),
    myViewNet(viewNet),
    myBlockIconRotation(0),
    myBlocked(false),
    myInspectionable(true),
    mySelectable(true),
    myMovable(true),
    myAdditionalDialog(NULL) {
    // Set rotation left hand
    myRotationLefthand = OptionsCont::getOptions().getBool("lefthand");
}


GNEAdditional::~GNEAdditional() {}


void
GNEAdditional::openAdditionalDialog() {
    throw InvalidArgument(toString(getTag()) + " doesn't have an additional dialog");
}


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


const std::string&
GNEAdditional::getParentName() const {
    return myViewNet->getNet()->getMicrosimID();
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
    new FXMenuCommand(ret, ("Copy " + toString(getTag()) + " name to clipboard").c_str(), 0, ret, MID_COPY_NAME);
    new FXMenuCommand(ret, ("Copy " + toString(getTag()) + " typed name to clipboard").c_str(), 0, ret, MID_COPY_TYPED_NAME);
    new FXMenuSeparator(ret);
    // build selection and show parameters menu
    buildSelectionPopupEntry(ret);
    buildShowParamsPopupEntry(ret);
    // show option to open additional dialog
    if(canOpenDialog(getTag())) {
        new FXMenuCommand(ret, ("Open " + toString(getTag()) + " Dialog").c_str(), getIcon(), &parent, MID_OPEN_ADDITIONAL_DIALOG);
        new FXMenuSeparator(ret);
    }
    // get attributes
    std::vector<SumoXMLAttr> attributes = getAttrs();
    // Show position parameters
    if (hasAttribute(getTag(), SUMO_ATTR_LANE)) {
        GNELane *lane = myViewNet->getNet()->retrieveLane(getAttribute(SUMO_ATTR_LANE));
        // Show menu command inner position
        const double innerPos = myShape.nearest_offset_to_point2D(parent.getPositionInformation());
        new FXMenuCommand(ret, ("Cursor position inner additional: " + toString(innerPos)).c_str(), 0, 0, 0);
        // If shape isn't empty, show menu command lane position
        if (myShape.size() > 0) {
            const double lanePos = lane->getShape().nearest_offset_to_point2D(myShape[0]);
            new FXMenuCommand(ret, ("Cursor position over " + toString(SUMO_TAG_LANE) + ": " + toString(innerPos + lanePos)).c_str(), 0, 0, 0);
        }
    } else if (hasAttribute(getTag(), SUMO_ATTR_EDGE)) {
        GNEEdge *edge = myViewNet->getNet()->retrieveEdge(getAttribute(SUMO_ATTR_EDGE));
        // Show menu command inner position
        const double innerPos = myShape.nearest_offset_to_point2D(parent.getPositionInformation());
        new FXMenuCommand(ret, ("Cursor position inner additional: " + toString(innerPos)).c_str(), 0, 0, 0);
        // If shape isn't empty, show menu command edge position
        if (myShape.size() > 0) {
            const double edgePos = edge->getLanes().at(0)->getShape().nearest_offset_to_point2D(myShape[0]);
            new FXMenuCommand(ret, ("Mouse position over " + toString(SUMO_TAG_EDGE) + ": " + toString(innerPos + edgePos)).c_str(), 0, 0, 0);
        }
    } else {
        new FXMenuCommand(ret, ("Cursor position in view: " + toString(getPositionInView().x()) + "," + toString(getPositionInView().y())).c_str(), 0, 0, 0);
    }
    return ret;
}


GUIParameterTableWindow*
GNEAdditional::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView&) {
    // get attributes
    std::vector<SumoXMLAttr> attributes = getAttrs();
    // Create table
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this, (int)attributes.size());
    // Iterate over attributes
    for (auto i : attributes) {
        // Add attribute and set it dynamic if aren't unique
        if (GNEAttributeCarrier::isUnique(getTag(), i)) {
            ret->mkItem(toString(i).c_str(), false, getAttribute(i));
        } else {
            ret->mkItem(toString(i).c_str(), true, getAttribute(i));
        }
    }
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
GNEAdditional::setBlockIconRotation(GNELane *additionalLane) {
    if (myShape.size() > 0 && myShape.length() != 0) {
        // If length of the shape is distint to 0, Obtain rotation of center of shape
        myBlockIconRotation = myShape.rotationDegreeAtOffset((myShape.length() / 2.)) - 90;
    } else if (hasAttribute(getTag(), SUMO_ATTR_LANE)) {
        // If additional is over a lane, set rotation in the position over lane
        double posOverLane = additionalLane->getShape().nearest_offset_to_point2D(getPositionInView());
        myBlockIconRotation = additionalLane->getShape().rotationDegreeAtOffset(posOverLane) - 90;
    } else {
        // In other case, rotation is 0
        myBlockIconRotation = 0;
    }
}


void
GNEAdditional::drawLockIcon(double size) const {
    if (myViewNet->showLockIcon()) {
        // Start pushing matrix
        glPushMatrix();
        // Traslate to middle of shape
        glTranslated(myBlockIconPosition.x(), myBlockIconPosition.y(), getType() + 0.1);
        // Set draw color
        glColor3d(1, 1, 1);
        // Rotate depending of myBlockIconRotation
        glRotated(myBlockIconRotation, 0, 0, -1);
        // Rotate 180 degrees
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
        GLHelper::setColor(RGBColor(255, 235, 0));
        for (std::vector<Position>::const_iterator j = (*i).begin(); (j + 1) != (*i).end(); j++) {
            // Draw Lines
            GLHelper::drawLine((*j), (*(j + 1)));
        }
        // Pop draw matrix
        glPopMatrix();
    }
}


GNEEdge*
GNEAdditional::changeEdge(GNEEdge *oldEdge, const std::string& newEdgeID) {
    if (oldEdge == NULL) {
        throw InvalidArgument(toString(getTag()) + " with ID '" + getMicrosimID() + "' doesn't belong to an " + toString(SUMO_TAG_EDGE));
    } else {
        oldEdge->removeAdditionalChild(this);
        GNEEdge* newEdge = myViewNet->getNet()->retrieveEdge(newEdgeID);
        newEdge->addAdditionalChild(this);
        updateGeometry();
        return newEdge;
    }
}


GNELane*
GNEAdditional::changeLane(GNELane *oldLane, const std::string& newLaneID) {
    if (oldLane == NULL) {
        throw InvalidArgument(toString(getTag()) + " with ID '" + getMicrosimID() + "' doesn't belong to a " + toString(SUMO_TAG_LANE));
    } else {
        oldLane->removeAdditionalChild(this);
        GNELane* newLane = myViewNet->getNet()->retrieveLane(newLaneID);
        newLane->addAdditionalChild(this);
        updateGeometry();
        return newLane;
    }
}

/****************************************************************************/
