/****************************************************************************/
/// @file    GNECrossing.cpp
/// @author  Jakob Erdmann
/// @date    June 2011
/// @version $Id$
///
// A class for visualizing Inner Lanes (used when editing traffic lights)
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

#include <string>
#include <iostream>
#include <utility>
#include <time.h>
#include <foreign/polyfonts/polyfonts.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/geom/PositionVector.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/common/ToString.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNECrossing.h"
#include "GNEJunction.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEEdge.h"
#include "GNEViewNet.h"
#include "GNEChange_Attribute.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// method definitions
// ===========================================================================
GNECrossing::GNECrossing(GNEJunction* parentJunction, const std::string& id) :
    GNENetElement(parentJunction->getNet(), id, GLO_CROSSING, SUMO_TAG_CROSSING, ICON_CROSSING),
    myParentJunction(parentJunction),
    myCrossing(parentJunction->getNBNode()->getCrossingRef(id)) {
    // Update geometry
    updateGeometry();
}


GNECrossing::~GNECrossing() {}


void
GNECrossing::updateGeometry() {
    // Clear Shape rotations and segments
    myShapeRotations.clear();
    myShapeLengths.clear();
    // only rebuild shape if junction's shape isn't in Buuble mode
    if (myParentJunction->getNBNode()->getShape().size() > 0) {
        // Obtain segments of size and calculate it
        int segments = (int) myCrossing.shape.size() - 1;
        if (segments >= 0) {
            myShapeRotations.reserve(segments);
            myShapeLengths.reserve(segments);
            for (int i = 0; i < segments; ++i) {
                const Position& f = myCrossing.shape[i];
                const Position& s = myCrossing.shape[i + 1];
                myShapeLengths.push_back(f.distanceTo2D(s));
                myShapeRotations.push_back((SUMOReal) atan2((s.x() - f.x()), (f.y() - s.y())) * (SUMOReal) 180.0 / (SUMOReal) PI);
            }
        }
    }
}


GNEJunction*
GNECrossing::getParentJunction() const {
    return myParentJunction;
}


NBNode::Crossing&
GNECrossing::getNBCrossing() const {
    return myCrossing;
}


void
GNECrossing::drawGL(const GUIVisualizationSettings& s) const {
    // only draw if option drawCrossingsAndWalkingareas is enabled and size of shape is greather than 0
    if (s.drawCrossingsAndWalkingareas == true && myShapeRotations.size() > 0 && myShapeLengths.size() > 0) {
        // push first draw matrix
        glPushMatrix();
        // push name
        glPushName(getGlID());
        // must draw on top of junction
        glTranslated(0, 0, GLO_JUNCTION + 0.1);
        // set color depending of priority
        if (myCrossing.priority) {
            glColor3d(0.9, 0.9, 0.9);
        } else {
            glColor3d(0.1, 0.1, 0.1);
        }
        // traslate to front
        glTranslated(0, 0, .2);
        // set default values
        SUMOReal length = 0.5;
        SUMOReal spacing = 1.0;
        SUMOReal halfWidth = myCrossing.width * 0.5;
        // push second draw matrix
        glPushMatrix();
        // draw on top of of the white area between the rails
        glTranslated(0, 0, 0.1);
        for (int i = 0; i < (int)myCrossing.shape.size() - 1; ++i) {
            // push three draw matrix
            glPushMatrix();
            // traslete and rotate
            glTranslated(myCrossing.shape[i].x(), myCrossing.shape[i].y(), 0.0);
            glRotated(myShapeRotations[i], 0, 0, 1);
            // draw crossing
            for (SUMOReal t = 0; t < myShapeLengths[i]; t += spacing) {
                glBegin(GL_QUADS);
                glVertex2d(-halfWidth, -t);
                glVertex2d(-halfWidth, -t - length);
                glVertex2d(halfWidth, -t - length);
                glVertex2d(halfWidth, -t);
                glEnd();
            }
            // pop three draw matrix
            glPopMatrix();
        }
        // pop second draw matrix
        glPopMatrix();
        // traslate to back
        glTranslated(0, 0, -.2);
        // pop name
        glPopName();
        // pop draw matrix
        glPopMatrix();
    }
}


GUIGLObjectPopupMenu*
GNECrossing::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    myPopup = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(myPopup, app);
    return myPopup;
}


GUIParameterTableWindow*
GNECrossing::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret =
        new GUIParameterTableWindow(app, *this, 2);
    // add items
    // close building
    ret->closeBuilding();
    return ret;
}


Boundary
GNECrossing::getCenteringBoundary() const {
    Boundary b = myCrossing.shape.getBoxBoundary();
    b.grow(10);
    return b;
}


std::string
GNECrossing::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
            break;
        case SUMO_ATTR_WIDTH:
            return toString(myCrossing.width);
            break;
        case SUMO_ATTR_PRIORITY:
            return myCrossing.priority ? "true" : "false";
            break;
        case SUMO_ATTR_EDGES:
            return toString(myCrossing.edges);
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNECrossing::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument("Modifying attribute '" + toString(key) + "' of " + toString(getTag()) + " isn't allowed");
        case SUMO_ATTR_EDGES:
        case SUMO_ATTR_WIDTH:
        case SUMO_ATTR_PRIORITY:
            undoList->add(new GNEChange_Attribute(this, key, value), true);
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNECrossing::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return false;
        case SUMO_ATTR_EDGES: {
            std::vector<std::string> NBEdgeIDs;
            SUMOSAXAttributes::parseStringVector(value, NBEdgeIDs);
            // Obtain NBEdges of GNENet and check if exists
            for (std::vector<std::string>::iterator i = NBEdgeIDs.begin(); i != NBEdgeIDs.end(); i++) {
                if (myNet->retrieveEdge((*i), false) == NULL) {
                    return false;
                }
            }
            return true;
        }
        case SUMO_ATTR_WIDTH:
            return isPositive<SUMOReal>(value);
        case SUMO_ATTR_PRIORITY:
            return ((value == "true") || (value == "false"));
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNECrossing::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument("Modifying attribute '" + toString(key) + "' of " + toString(getTag()) + " isn't allowed");
        case SUMO_ATTR_EDGES: {
            // remove edges of crossing
            myCrossing.edges.clear();
            std::vector<std::string> NBEdgeIDs;
            SUMOSAXAttributes::parseStringVector(value, NBEdgeIDs);
            // Obtain NBEdges of GNENet and insert it in the crossing
            for (std::vector<std::string>::iterator i = NBEdgeIDs.begin(); i != NBEdgeIDs.end(); i++) {
                myCrossing.edges.push_back(myNet->retrieveEdge(*i)->getNBEdge());
            }
            break;
        }
        case SUMO_ATTR_WIDTH:
            // Change width an refresh element
            myCrossing.width = parse<SUMOReal>(value);
            myNet->refreshElement(this);
            break;
        case SUMO_ATTR_PRIORITY:
            myCrossing.priority = parse<bool>(value);
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

/****************************************************************************/
