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
/// @file    GNEShape.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2017
///
// Abstract class for Shapes uses in netedit
/****************************************************************************/
#include <config.h>

#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <netedit/frames/common/GNESelectorFrame.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEShape.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNEShape::GNEShape(GNENet* net, SumoXMLTag tag, bool movementBlocked,
        const std::vector<GNEEdge*>& edgeParents,
        const std::vector<GNELane*>& laneParents,
        const std::vector<GNEShape*>& shapeParents,
        const std::vector<GNEAdditional*>& additionalParents,
        const std::vector<GNEDemandElement*>& demandElementParents,
        const std::vector<GNEGenericData*>& genericDataParents,
        const std::vector<GNEEdge*>& edgeChildren,
        const std::vector<GNELane*>& laneChildren,
        const std::vector<GNEShape*>& shapeChildren,
        const std::vector<GNEAdditional*>& additionalChildren,
        const std::vector<GNEDemandElement*>& demandElementChildren,
        const std::vector<GNEGenericData*>& genericDataChildren) :
    GNEAttributeCarrier(tag),
    GNEHierarchicalParentElements(this, edgeParents, laneParents, shapeParents, additionalParents, demandElementParents, genericDataParents),
    GNEHierarchicalChildElements(this, edgeChildren, laneChildren, shapeChildren, additionalChildren, demandElementChildren, genericDataChildren),
    myNet(net),
    myBlockMovement(movementBlocked) {
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


void
GNEShape::draw(const Position& pos, double layer, double size) const {
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


void
GNEShape::selectAttributeCarrier(bool changeFlag) {
    if (!myNet) {
        throw ProcessError("Net cannot be nullptr");
    } else {
        gSelected.select(getGUIGlObject()->getGlID());
        // add object into list of selected objects
        myNet->getViewNet()->getViewParent()->getSelectorFrame()->getLockGLObjectTypes()->addedLockedObject(getGUIGlObject()->getType());
        if (changeFlag) {
            mySelected = true;
        }
    }
}


void
GNEShape::unselectAttributeCarrier(bool changeFlag) {
    if (!myNet) {
        throw ProcessError("Net cannot be nullptr");
    } else {
        gSelected.deselect(getGUIGlObject()->getGlID());
        // remove object of list of selected objects
        myNet->getViewNet()->getViewParent()->getSelectorFrame()->getLockGLObjectTypes()->removeLockedObject(getGUIGlObject()->getType());
        if (changeFlag) {
            mySelected = false;
        }
    }
}


bool
GNEShape::isAttributeCarrierSelected() const {
    return mySelected;
}


bool
GNEShape::drawUsingSelectColor() const {
    if (mySelected && (myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork())) {
        return true;
    } else {
        return false;
    }
}


void
GNEShape::enableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    //
}


void
GNEShape::disableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    //
}


std::string
GNEShape::getPopUpID() const {
    return getTagStr() + ": " + getID();
}


std::string
GNEShape::getHierarchyName() const {
    return getTagStr();
}


void
GNEShape::setEnabledAttribute(const int /*enabledAttributes*/) {
    //
}


/****************************************************************************/
