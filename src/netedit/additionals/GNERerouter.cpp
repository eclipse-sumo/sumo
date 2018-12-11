/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
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
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/dialogs/GNERerouterDialog.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/images/GUITextureSubSys.h>

#include "GNERerouter.h"
#include "GNERerouterInterval.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNERerouter::GNERerouter(const std::string& id, GNEViewNet* viewNet, const Position& pos, const std::vector<GNEEdge*>& edges, const std::string& name, const std::string& filename, double probability, bool off, double timeThreshold, const std::string& vTypes, bool blockMovement) :
    GNEAdditional(id, viewNet, GLO_REROUTER, SUMO_TAG_REROUTER, name, blockMovement, edges),
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
GNERerouter::updateGeometry(bool updateGrid) {
    // first check if object has to be removed from grid (SUMOTree)
    if (updateGrid) {
        myViewNet->getNet()->removeGLObjectFromGrid(this);
    }

    // Clear shape
    myGeometry.shape.clear();

    // Set block icon position
    myBlockIcon.position = myPosition;

    // Set block icon offset
    myBlockIcon.offset = Position(-0.5, -0.5);

    // Set block icon rotation, and using their rotation for draw logo
    myBlockIcon.setRotation();

    // Set position
    myGeometry.shape.push_back(myPosition);

    // update connection positions
    myChildConnections.update();

    // last step is to check if object has to be added into grid (SUMOTree) again
    if (updateGrid) {
        myViewNet->getNet()->addGLObjectIntoGrid(this);
    }
}


Position
GNERerouter::getPositionInView() const {
    return myPosition;
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
    updateGeometry(false);
}


void
GNERerouter::commitGeometryMoving(GNEUndoList* undoList) {
    // commit new position allowing undo/redo
    undoList->p_begin("position of " + getTagStr());
    undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_POSITION, toString(myPosition), true, toString(myMove.originalViewPosition)));
    undoList->p_end();
}


std::string
GNERerouter::getParentName() const {
    return myViewNet->getNet()->getMicrosimID();
}


void
GNERerouter::drawGL(const GUIVisualizationSettings& s) const {
    // Start drawing adding an gl identificator
    glPushName(getGlID());

    // Add a draw matrix for drawing logo
    glPushMatrix();
    glTranslated(myGeometry.shape[0].x(), myGeometry.shape[0].y(), getType());


    // Draw icon depending of detector is selected and if isn't being drawn for selecting
    if (s.drawForSelecting) {
        GLHelper::setColor(RGBColor::RED);
        GLHelper::drawBoxLine(Position(0, 1), 0, 2, 1);
    } else {
        glColor3d(1, 1, 1);
        glRotated(180, 0, 0, 1);
        if (isAttributeCarrierSelected()) {
            GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_REROUTERSELECTED), 1);
        } else {
            GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_REROUTER), 1);
        }
    }

    // Pop draw matrix
    glPopMatrix();

    // Only lock and childs if isn't being drawn for selecting
    if (!s.drawForSelecting) {

        // Show Lock icon depending of the Edit mode
        myBlockIcon.draw(0.4);

        // Draw symbols in every lane
        const double exaggeration = s.addSize.getExaggeration(s, this);

        if (s.scale * exaggeration >= 3) {
            // draw rerouter symbol over all lanes
            for (auto i : myChildConnections.symbolsPositionAndRotation) {
                glPushMatrix();
                glTranslated(i.first.x(), i.first.y(), getType());
                glRotated(-1 * i.second, 0, 0, 1);
                glScaled(exaggeration, exaggeration, 1);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

                glBegin(GL_TRIANGLES);
                glColor3d(1, .8f, 0);
                // base
                glVertex2d(0 - 1.4, 0);
                glVertex2d(0 - 1.4, 6);
                glVertex2d(0 + 1.4, 6);
                glVertex2d(0 + 1.4, 0);
                glVertex2d(0 - 1.4, 0);
                glVertex2d(0 + 1.4, 6);
                glEnd();

                // draw "U"
                GLHelper::drawText("U", Position(0, 2), .1, 3, RGBColor::BLACK, 180);

                // draw Probability
                GLHelper::drawText((toString((int)(myProbability * 100)) + "%").c_str(), Position(0, 4), .1, 0.7, RGBColor::BLACK, 180);

                glPopMatrix();
            }
        }

        // Draw connections
        myChildConnections.draw();
    }
    // check if dotted contour has to be drawn
    if (!s.drawForSelecting && (myViewNet->getDottedAC() == this)) {
        GLHelper::drawShapeDottedContour(getType(), myPosition, 2, 2);
        // draw shape dotte contour aroud alld connections between child and parents
        for (auto i : myChildConnections.connectionPositions) {
            GLHelper::drawShapeDottedContour(getType(), i, 0);
        }
        // draw rerouter symbol over all lanes
        for (auto i : myChildConnections.symbolsPositionAndRotation) {
            GLHelper::drawShapeDottedContour(getType(), i.first, 2.8, 6, -1 * i.second, 0, 3);
        }
    }

    // Draw name
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);

    // Pop name
    glPopName();
}


std::string
GNERerouter::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_EDGES:
            return parseIDs(myEdgeChilds);
        case SUMO_ATTR_POSITION:
            return toString(myPosition);
        case SUMO_ATTR_NAME:
            return myAdditionalName;
        case SUMO_ATTR_FILE:
            return myFilename;
        case SUMO_ATTR_PROB:
            return toString(myProbability);
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            return toString(myTimeThreshold);
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
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            // Change Ids of all Rerouter interval childs
            for (auto i : myAdditionalChilds) {
                i->setAttribute(SUMO_ATTR_ID, generateAdditionalChildID(SUMO_TAG_INTERVAL), undoList);
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
            undoList->p_add(new GNEChange_Attribute(this, key, value));
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
            return canParse<double>(value) && (parse<double>(value) >= 0);
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
        case SUMO_ATTR_EDGES: {
            // remove references of this rerouter in all edge childs
            for (auto i : myEdgeChilds) {
                i->removeAdditionalParent(this);
            }
            // set new edges
            myEdgeChilds = parse<std::vector<GNEEdge*> >(myViewNet->getNet(), value);
            // add references to this rerouter in all newedge childs
            for (auto i : myEdgeChilds) {
                i->addAdditionalParent(this);
            }
            break;
        }
        case SUMO_ATTR_POSITION:
            myPosition = parse<Position>(value);
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
            myTimeThreshold = parse<double>(value);
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
    // Update Geometry after setting a new attribute (but avoided for certain attributes)
    if((key != SUMO_ATTR_ID) && (key != GNE_ATTR_GENERIC) && (key != GNE_ATTR_SELECTED)) {
        updateGeometry(true);
    }
}

/****************************************************************************/
