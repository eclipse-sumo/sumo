/****************************************************************************/
/// @file    GNEAdditional.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
/// @version $Id$
///
/// A abstract class for representation of additional elements
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/xml/SUMOSAXHandler.h>

#include "GNEAdditional.h"
#include "GNELane.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEViewNet.h"
#include "GNELogo_Lock.cpp"
#include "GNELogo_Empty.cpp"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// static member definitions
// ===========================================================================
GUIGlID GNEAdditional::myAdditionalLockGlID = 0;
GUIGlID GNEAdditional::myAdditionalEmptyGlID = 0;
bool GNEAdditional::myAdditionalLockInitialized = false;
bool GNEAdditional::myAdditionalEmptyInitialized = false;

// ===========================================================================
// member method definitions
// ===========================================================================

GNEAdditional::GNEAdditional(const std::string& id, GNEViewNet* viewNet, Position pos, SumoXMLTag tag, GNEAdditionalSet *additionalSetParent, bool blocked, bool inspectionable, bool selectable) :
    GUIGlObject(GLO_ADDITIONAL, id),
    GNEAttributeCarrier(tag),
    myViewNet(viewNet),
    myPosition(pos),
    myAdditionalSetParent(additionalSetParent),
    myBlocked(blocked),
    myInspectionable(inspectionable),
    mySelectable(selectable),
    myBlockIconRotation(0),
    myBaseColor(RGBColor::GREEN),
    myBaseColorSelected(RGBColor::BLUE), 
    myAdditionalDialog(NULL) {
    // Set rotation left hand
    myRotationLefthand = OptionsCont::getOptions().getBool("lefthand");
    // If this additional belongs to a set, add it.
    if(myAdditionalSetParent)
        myAdditionalSetParent->addAdditionalChild(this);
    // load additional lock, if wasn't already initialized
    if (!myAdditionalLockInitialized) {
        FXImage* i = new FXGIFImage(getViewNet()->getNet()->getApp(), GNELogo_Lock, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP);
        myAdditionalLockGlID = GUITexturesHelper::add(i);
        myAdditionalLockInitialized = true;
        delete i;
    }
    // load additional empty, if wasn't already inicializated
    if (!myAdditionalEmptyInitialized) {
        FXImage* i = new FXGIFImage(getViewNet()->getNet()->getApp(), GNELogo_Empty, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP);
        myAdditionalEmptyGlID = GUITexturesHelper::add(i);
        myAdditionalEmptyInitialized = true;
        delete i;
    }
}


GNEAdditional::~GNEAdditional() {
    // If this additional belongs to a set, remove it.
    if(myAdditionalSetParent)
        myAdditionalSetParent->removeAdditionalChild(this); 
}


void 
GNEAdditional::openAdditionalDialog() {}


GNEViewNet*
GNEAdditional::getViewNet() const {
    return myViewNet;
}


PositionVector
GNEAdditional::getShape() const {
    return myShape;
}


bool
GNEAdditional::isBlocked() const {
    return myBlocked;
}


bool
GNEAdditional::isAdditionalSelected() const {
    return gSelected.isSelected(getType(), getGlID());
}


GNEAdditionalSet*
GNEAdditional::getAdditionalSetParent() const {
    return myAdditionalSetParent;
}


void
GNEAdditional::setBlocked(bool value) {
    myBlocked = value;
}


void
GNEAdditional::setPositionInView(const Position &pos) {
    myPosition = pos;
}


GNEEdge* 
GNEAdditional::getEdge() const {
    return NULL;
}


GNELane* 
GNEAdditional::getLane() const {
    return NULL;
}


void 
GNEAdditional::removeEdgeReference() {
    std::cout << "WARNING: Calling virtual function removeEdgeReference() of class GNEAdditional. Implement removeEdgeReference() in additional child to avoid errors" << std::endl;
}


void 
GNEAdditional::removeLaneReference() {
    std::cout << "WARNING: Calling virtual function removeLaneReference() of class GNEAdditional. Implement removeLaneReference() in additional child to avoid errors" << std::endl;
}


const std::string&
GNEAdditional::getParentName() const {
    return myViewNet->getNet()->getMicrosimID();
}


GUIGLObjectPopupMenu*
GNEAdditional::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    new FXMenuCommand(ret, ("Copy " + toString(getTag()) + " name to clipboard").c_str(), 0, ret, MID_COPY_NAME);
    new FXMenuCommand(ret, ("Copy " + toString(getTag()) + " typed name to clipboard").c_str(), 0, ret, MID_COPY_TYPED_NAME);
    buildSelectionPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
    // buildShowParamsPopupEntry(ret, false);
    // Show positions
/*** REIMPLEMENT IN CHILDS **
    if(getLane() != 0) {
        const SUMOReal innerPos = myShape.nearest_offset_to_point2D(parent.getPositionInformation());
        new FXMenuCommand(ret, ("inner position: " + toString(innerPos)).c_str(), 0, 0, 0);
        if(myShape.size() > 0) {
            const SUMOReal lanePos = myLane->getShape().nearest_offset_to_point2D(myShape[0]);
            new FXMenuCommand(ret, ("lane position: " + toString(innerPos + lanePos)).c_str(), 0, 0, 0);
        }
    } else
        new FXMenuCommand(ret, ("position: " + toString(myPosition.x()) + "," + toString(myPosition.y())).c_str(), 0, 0, 0);
    // Show childs (if this is is an additionalSet)
    GNEAdditionalSet* additionalSet = dynamic_cast<GNEAdditionalSet*>(this);
    if(additionalSet) {
        new FXMenuSeparator(ret);
        new FXMenuCommand(ret, ("number of childs: " + toString(additionalSet->getNumberOfChilds())).c_str(), 0, 0, 0);
    }
    // new FXMenuSeparator(ret);
    // buildPositionCopyEntry(ret, false);
    // let the GNEViewNet store the popup position
    dynamic_cast<GNEViewNet&>(parent).markPopupPosition();
**/
    return ret;
}



Boundary
GNEAdditional::getCenteringBoundary() const {
    Boundary b = myShape.getBoxBoundary();
    b.grow(20);
    return b;
}


void
GNEAdditional::setBlockIconRotation(GNELane *lane) {
    if (myShape.length() != 0)
        // If lenght of the shape is distint to 0, Obtain rotation of center of shape
        myBlockIconRotation = myShape.rotationDegreeAtOffset((myShape.length() / 2.)) - 90;
    else if(lane != NULL)
        // If additional is over a lane, set rotation in the position over lane
        myBlockIconRotation = lane->getShape().rotationDegreeAtOffset(lane->getPositionRelativeToParametricLenght(myPosition.x())) - 90;
    else
        // In other case, rotation is 0
        myBlockIconRotation = 0;
}


void
GNEAdditional::drawLockIcon(SUMOReal size) const {
    // Start pushing matrix
    glPushMatrix();
    // Traslate to middle of shape
    glTranslated(myShape.getLineCenter().x(), myShape.getLineCenter().y(), getType() + 0.1);
    // Set draw color
    glColor3d(1, 1, 1);
    // Rotate depending of myBlockIconRotation
    glRotated(myBlockIconRotation, 0, 0, -1);
    // Rotate 180º
    glRotated(180, 0, 0, 1);
    // Traslate depending of the offset
    glTranslated(myBlockIconOffset.x(), myBlockIconOffset.y(), 0);
    // If myBlocked is enable, draw lock, in other case, draw empty square
    if(myBlocked)
        GUITexturesHelper::drawTexturedBox(myAdditionalLockGlID, size);
    else
        GUITexturesHelper::drawTexturedBox(myAdditionalEmptyGlID, size);
    // Pop matrix
    glPopMatrix();
}


/****************************************************************************/
