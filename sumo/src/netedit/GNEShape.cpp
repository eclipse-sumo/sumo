/****************************************************************************/
/// @file    GNEShape.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2017
/// @version $Id$
///
// Abstract class for Shapes uses in netedit
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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

#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/images/GUITextureSubSys.h>

#include "GNEShape.h"
#include "GNENet.h"
#include "GNEViewNet.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNEShape::GNEShape(GNENet* net, SumoXMLTag tag, GUIIcon icon, bool movementBlocked, bool shapeBlocked) :
    GNEAttributeCarrier(tag, icon),
    myNet(net),
    myBlockMovement(movementBlocked),
    myBlockShape(shapeBlocked) {
}


GNEShape::~GNEShape() {}


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
GNEShape::drawLockIcon(const Position &pos, double size) const {
    if (myNet->getViewNet()->showLockIcon()) {
        // Start pushing matrix
        glPushMatrix();
        // Traslate to middle of shape
        glTranslated(pos.x(), pos.y(), parse<double>(getAttribute(SUMO_ATTR_LAYER)) + 0.1);
        // Rotate 180 degrees
        glRotated(180, 0, 0, 1);
        // Set draw color
        glColor3d(1, 1, 1);
        // Draw icon depending of the state of additional
        if (myNet->isShapeSelected(getTag(), getID())) {
            if (myBlockMovement) {
                // Draw lock texture if additional is movable, is blocked and is selected
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_LOCKSELECTED), size);
            } else {
                // Draw empty texture if additional is movable, isn't blocked and is selected
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_EMPTYSELECTED), size);
            }
        } else {
            if (myBlockMovement) {
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

/****************************************************************************/
