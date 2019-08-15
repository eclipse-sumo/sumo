/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNERerouter.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
//
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/dialogs/GNERerouterDialog.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNERerouter.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNERerouter::GNERerouter(const std::string& id, GNEViewNet* viewNet, const Position& pos, const std::vector<GNEEdge*>& edges, const std::string& name, const std::string& filename, double probability, bool off, SUMOTime timeThreshold, const std::string& vTypes, bool blockMovement) :
    GNEAdditional(id, viewNet, GLO_REROUTER, SUMO_TAG_REROUTER, name, blockMovement, {}, {}, {}, {}, {}, edges, {}, {}, {}, {}),
              myPosition(pos),
              myFilename(filename),
              myProbability(probability),
              myOff(off),
              myTimeThreshold(timeThreshold),
myVTypes(vTypes) {
}


GNERerouter::~GNERerouter() {
}


void
GNERerouter::updateGeometry() {
    // Set block icon position
    myBlockIcon.position = myPosition;

    // Set block icon offset
    myBlockIcon.offset = Position(-0.5, -0.5);

    // Set block icon rotation, and using their rotation for draw logo
    myBlockIcon.setRotation();

    // update connection positions
    myChildConnections.update();
}


Position
GNERerouter::getPositionInView() const {
    return myPosition;
}


Boundary
GNERerouter::getCenteringBoundary() const {
    // Return Boundary depending if myMovingGeometryBoundary is initialised (important for move geometry)
    if (myMove.movingGeometryBoundary.isInitialised()) {
        return myMove.movingGeometryBoundary;
    } else {
        Boundary b;
        b.add(myPosition);
        b.grow(5);
        return b;
    }
}


void
GNERerouter::openAdditionalDialog() {
    // Open rerouter dialog
    GNERerouterDialog(this);
}


void
GNERerouter::moveGeometry(const Position& offset) {
    // restore old position, apply offset and update Geometry
    myPosition = myMove.originalViewPosition;
    myPosition.add(offset);
    // filtern position using snap to active grid
    myPosition = myViewNet->snapToActiveGrid(myPosition);
    updateGeometry();
}


void
GNERerouter::commitGeometryMoving(GNEUndoList* undoList) {
    // commit new position allowing undo/redo
    undoList->p_begin("position of " + getTagStr());
    undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), SUMO_ATTR_POSITION, toString(myPosition), true, toString(myMove.originalViewPosition)));
    undoList->p_end();
}


std::string
GNERerouter::getParentName() const {
    return myViewNet->getNet()->getMicrosimID();
}


void
GNERerouter::drawGL(const GUIVisualizationSettings& s) const {
    // Obtain exaggeration of the draw
    const double exaggeration = s.addSize.getExaggeration(s, this);
    // check if boundary has to be drawn
    if (s.drawBoundaries) {
        GLHelper::drawBoundary(getCenteringBoundary());
    }
    // Start drawing adding an gl identificator
    glPushName(getGlID());
    // Add a draw matrix for drawing logo
    glPushMatrix();
    glTranslated(myPosition.x(), myPosition.y(), getType());
    // Draw icon depending of detector is selected and if isn't being drawn for selecting
    if (!s.drawForSelecting && s.drawDetail(s.detailSettings.laneTextures, exaggeration)) {
        glColor3d(1, 1, 1);
        glRotated(180, 0, 0, 1);
        if (drawUsingSelectColor()) {
            GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_REROUTERSELECTED), 1);
        } else {
            GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_REROUTER), 1);
        }
    } else {
        GLHelper::setColor(RGBColor::RED);
        GLHelper::drawBoxLine(Position(0, 1), 0, 2, 1);
    }
    // Pop draw matrix
    glPopMatrix();
    // Show Lock icon
    myBlockIcon.drawIcon(s, exaggeration, 0.4);
    // Draw child connections
    drawChildConnections(s, getType());
    // check if dotted contour has to be drawn
    if (myViewNet->getDottedAC() == this) {
        GLHelper::drawShapeDottedContourRectangle(s, getType(), myPosition, 2, 2);
        // draw shape dotte contour aroud alld connections between child and parents
        for (auto i : myChildConnections.connectionPositions) {
            GLHelper::drawShapeDottedContourAroundShape(s, getType(), i, 0);
        }
    }
    // Draw name
    drawName(getPositionInView(), s.scale, s.addName);
    // Pop name
    glPopName();
}


std::string
GNERerouter::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_EDGES:
            return parseIDs(getEdgeChildren());
        case SUMO_ATTR_POSITION:
            return toString(myPosition);
        case SUMO_ATTR_NAME:
            return myAdditionalName;
        case SUMO_ATTR_FILE:
            return myFilename;
        case SUMO_ATTR_PROB:
            return toString(myProbability);
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            return time2string(myTimeThreshold);
        case SUMO_ATTR_VTYPES:
            return myVTypes;
        case SUMO_ATTR_OFF:
            return toString(myOff);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return toString(myBlockMovement);
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNERerouter::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID: {
            // change ID of Rerouter Interval
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), key, value));
            // Change Ids of all Rerouter interval children
            for (auto i : getAdditionalChildren()) {
                i->setAttribute(SUMO_ATTR_ID, generateChildID(SUMO_TAG_INTERVAL), undoList);
            }
            break;
        }
        case SUMO_ATTR_EDGES:
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_NAME:
        case SUMO_ATTR_FILE:
        case SUMO_ATTR_PROB:
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
        case SUMO_ATTR_VTYPES:
        case SUMO_ATTR_OFF:
        case GNE_ATTR_BLOCK_MOVEMENT:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_GENERIC:
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNERerouter::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_EDGES:
            if (value.empty()) {
                return false;
            } else {
                return canParse<std::vector<GNEEdge*> >(myViewNet->getNet(), value, false);
            }
        case SUMO_ATTR_POSITION:
            return canParse<Position>(value);
        case SUMO_ATTR_NAME:
            return SUMOXMLDefinitions::isValidAttribute(value);
        case SUMO_ATTR_FILE:
            return SUMOXMLDefinitions::isValidFilename(value);
        case SUMO_ATTR_PROB:
            return canParse<double>(value) && (parse<double>(value) >= 0) && (parse<double>(value) <= 1);
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            return canParse<SUMOTime>(value);
        case SUMO_ATTR_VTYPES:
            if (value.empty()) {
                return true;
            } else {
                return SUMOXMLDefinitions::isValidListOfTypeID(value);
            }
        case SUMO_ATTR_OFF:
            return canParse<bool>(value);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return canParse<bool>(value);
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_GENERIC:
            return isGenericParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


std::string
GNERerouter::getPopUpID() const {
    return getTagStr() + ": " + getID();
}


std::string
GNERerouter::getHierarchyName() const {
    return getTagStr();
}

// ===========================================================================
// private
// ===========================================================================

void
GNERerouter::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            changeAdditionalID(value);
            break;
        case SUMO_ATTR_EDGES:
            changeEdgeChildren(this, value);
            break;
        case SUMO_ATTR_POSITION:
            myViewNet->getNet()->removeGLObjectFromGrid(this);
            myPosition = parse<Position>(value);
            myViewNet->getNet()->addGLObjectIntoGrid(this);
            break;
        case SUMO_ATTR_NAME:
            myAdditionalName = value;
            break;
        case SUMO_ATTR_FILE:
            myFilename = value;
            break;
        case SUMO_ATTR_PROB:
            myProbability = parse<double>(value);
            break;
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            myTimeThreshold = parse<SUMOTime>(value);
            break;
        case SUMO_ATTR_VTYPES:
            myVTypes = value;
            break;
        case SUMO_ATTR_OFF:
            myOff = parse<bool>(value);
            break;
        case GNE_ATTR_BLOCK_MOVEMENT:
            myBlockMovement = parse<bool>(value);
            break;
        case GNE_ATTR_SELECTED:
            if (parse<bool>(value)) {
                selectAttributeCarrier();
            } else {
                unselectAttributeCarrier();
            }
            break;
        case GNE_ATTR_GENERIC:
            setGenericParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

/****************************************************************************/
