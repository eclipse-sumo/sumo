/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEShape.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2017
/// @version $Id$
///
// Abstract class for Shapes uses in netedit
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/images/GUITextureSubSys.h>

#include "GNEShape.h"
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>


// ===========================================================================
// method definitions
// ===========================================================================

GNEShape::GNEShape(GNENet* net, SumoXMLTag tag, GUIIcon icon, bool movementBlocked, bool shapeBlocked) :
    GNEAttributeCarrier(tag, icon),
    myNet(net),
    myBlockMovement(movementBlocked),
    myBlockShape(shapeBlocked),
    mySelected(false) {
}


GNEShape::~GNEShape() {
    if(mySelected) {
        if(getTag() == SUMO_TAG_POLY) {
            myNet->unselectAttributeCarrier(GLO_POLYGON, this, false);
        } else {
            myNet->unselectAttributeCarrier(GLO_POI, this, false);
        }
    }
}


GNENet*
GNEShape::getNet() const {
    return myNet;
}


bool
GNEShape::isMovementBlocked() const {
    return myBlockMovement;
}


bool
GNEShape::isShapeBlocked() const {
    return myBlockShape;
}


void 
GNEShape::selectShape() {
    if(!myNet) {
        throw ProcessError("Net cannot be NULL");
    } else if (!mySelected) {
        if(getTag() == SUMO_TAG_POLY) {
            myNet->selectAttributeCarrier(GLO_POLYGON, this);
        } else {
            myNet->selectAttributeCarrier(GLO_POI, this);
        }
        mySelected = true;
    } 
}


void 
GNEShape::unselectShape() {
    if(!myNet) {
        throw ProcessError("Net cannot be NULL");
    } else if (mySelected) {
        if(getTag() == SUMO_TAG_POLY) {
            myNet->unselectAttributeCarrier(GLO_POLYGON, this);
        } else {
            myNet->unselectAttributeCarrier(GLO_POI, this);
        }
        mySelected = false;
    } 
}


bool 
GNEShape::isShapeSelected() const {
    return mySelected;
}


void
GNEShape::drawLockIcon(const Position& pos, double layer, double size) const {
    if (myNet->getViewNet()->showLockIcon()) {
        // Start pushing matrix
        glPushMatrix();
        // Traslate to middle of shape
        glTranslated(pos.x(), pos.y(), layer + 0.1);
        // Rotate 180 degrees
        glRotated(180, 0, 0, 1);
        // Set draw color
        glColor3d(1, 1, 1);
        // Draw icon depending of the selection status
        if (mySelected) {
            if (myBlockMovement) {
                // Draw lock texture if shape is movable, is blocked and is selected
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_LOCKSELECTED), size);
            } else {
                // Draw empty texture if shape is movable, isn't blocked and is selected
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_EMPTYSELECTED), size);
            }
        } else {
            if (myBlockMovement) {
                // Draw lock texture if shape is movable and is blocked
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_LOCK), size);
            } else {
                // Draw empty texture if shape is movable and isn't blocked
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_EMPTY), size);
            }
        }
        // Pop matrix
        glPopMatrix();
    }
}

/****************************************************************************/
