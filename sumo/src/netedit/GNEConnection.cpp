/****************************************************************************/
/// @file    GNEConnection.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
/// @version $Id$
///
// A class for visualizing connections between lanes
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
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/common/ToString.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEConnection.h"
#include "GNEJunction.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNENet.h"
#include "GNEChange_Attribute.h"
#include "GNEUndoList.h"
#include "GNEViewNet.h"
#include "GNEInternalLane.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS



// ===========================================================================
// static member definitions
// ===========================================================================
int NUM_POINTS = 5;

// ===========================================================================
// method definitions
// ===========================================================================

GNEConnection::GNEConnection(GNELane* from, GNELane* to) :
    GNENetElement(from->getNet(), from->getMicrosimID() + " -> " + to->getMicrosimID(),
                  GLO_CONNECTION, SUMO_TAG_CONNECTION, ICON_CONNECTION),
    myFromLane(from),
    myToLane(to),
    myLinkState(LINKSTATE_TL_OFF_NOSIGNAL),
    myDrawConnection(true) {
    // geometry will be updated later
}


GNEConnection::~GNEConnection() {}


void
GNEConnection::updateGeometry() {
    // Clear containers
    myShapeRotations.clear();
    myShapeLengths.clear();
    // Get shape of from and to lanes
    NBEdge::Connection& nbCon = getNBEdgeConnection();
    PositionVector laneShapeFrom;
    if ((int)getEdgeFrom()->getNBEdge()->getLanes().size() > nbCon.fromLane) {
        laneShapeFrom = getEdgeFrom()->getNBEdge()->getLanes().at(nbCon.fromLane).shape;
    } else {
        return;
    }
    PositionVector laneShapeTo;
    if ((int)nbCon.toEdge->getLanes().size() > nbCon.toLane) {
        laneShapeTo = nbCon.toEdge->getLanes().at(nbCon.toLane).shape;
    } else {
        return;
    }
    // Calculate shape of connection depending of the size of Junction shape
    if (getEdgeFrom()->getNBEdge()->getToNode()->getShape().area() > 4) { // value obtanied from GNEJunction::drawgl
        // Calculate shape using a smooth shape
        myShape = getEdgeFrom()->getNBEdge()->getToNode()->computeSmoothShape(
                      laneShapeFrom,
                      laneShapeTo,
                      NUM_POINTS, getEdgeFrom()->getNBEdge()->getTurnDestination() == nbCon.toEdge,
                      (SUMOReal) 5. * (SUMOReal) getEdgeFrom()->getNBEdge()->getNumLanes(),
                      (SUMOReal) 5. * (SUMOReal) nbCon.toEdge->getNumLanes());

    } else {
        myShape.clear();
        myShape.push_back(laneShapeFrom.positionAtOffset(laneShapeFrom.length() - 1));
        myShape.push_back(laneShapeTo.positionAtOffset(1));
    }

    // Obtain lenghts and shape rotations
    int segments = (int) myShape.size() - 1;
    if (segments >= 0) {
        myShapeRotations.reserve(segments);
        myShapeLengths.reserve(segments);
        for (int i = 0; i < segments; ++i) {
            const Position& f = myShape[i];
            const Position& s = myShape[i + 1];
            myShapeLengths.push_back(f.distanceTo2D(s));
            myShapeRotations.push_back((SUMOReal) atan2((s.x() - f.x()), (f.y() - s.y())) * (SUMOReal) 180.0 / (SUMOReal) PI);
        }
    }
}


Boundary
GNEConnection::getBoundary() const {
    return myShape.getBoxBoundary();
}


GNEEdge*
GNEConnection::getEdgeFrom() const {
    return &(myFromLane->getParentEdge());
}


GNEEdge*
GNEConnection::getEdgeTo() const {
    return &(myToLane->getParentEdge());
}


GNELane*
GNEConnection::getLaneFrom() const {
    return myFromLane;
}


GNELane*
GNEConnection::getLaneTo() const {
    return myToLane;
}


int
GNEConnection::getFromLaneIndex() const {
    return myFromLane->getIndex();
}


int
GNEConnection::getToLaneIndex() const {
    return myToLane->getIndex();
}


NBEdge::Connection&
GNEConnection::getNBEdgeConnection() const {
    return getEdgeFrom()->getNBEdge()->getConnectionRef(getFromLaneIndex(), getEdgeTo()->getNBEdge(), getToLaneIndex());
}


NBConnection
GNEConnection::getNBConnection() const {
    return NBConnection(getEdgeFrom()->getNBEdge(), getFromLaneIndex(),
                        getEdgeTo()->getNBEdge(), getToLaneIndex(),
                        (int)getNBEdgeConnection().tlLinkNo);
}


LinkState
GNEConnection::getLinkState() const {
    return myLinkState;
}


void
GNEConnection::updateLinkState() {
    NBEdge::Connection& nbCon = getNBEdgeConnection();
    myLinkState = getEdgeFrom()->getNBEdge()->getToNode()->getLinkState(getEdgeFrom()->getNBEdge(),
                  nbCon.toEdge,
                  nbCon.fromLane,
                  nbCon.toLane,
                  nbCon.mayDefinitelyPass,
                  nbCon.tlID);
}


bool
GNEConnection::getDrawConnection() const {
    return myDrawConnection;
}


void
GNEConnection::setDrawConnection(bool drawConnection) {
    myDrawConnection = drawConnection;
}


GUIGLObjectPopupMenu*
GNEConnection::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
    // let the GNEViewNet store the popup position
    (dynamic_cast<GNEViewNet&>(parent)).markPopupPosition();
    return ret;
}


GUIParameterTableWindow*
GNEConnection::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    // Currently ignored before implementation to avoid warnings
    UNUSED_PARAMETER(app);
    UNUSED_PARAMETER(parent);
    return NULL;
}


Boundary
GNEConnection::getCenteringBoundary() const {
    return Boundary();
}


void
GNEConnection::drawGL(const GUIVisualizationSettings& s) const {
    // Check if connection must be drawed
    if (myDrawConnection && (myNet->getViewNet()->showConnections())) {
        // Push draw matrix 1
        glPushMatrix();
        // Push name
        glPushName(getGlID());
        // Traslate matrix
        glTranslated(0, 0, GLO_JUNCTION + 0.1); // must draw on top of junction
        // Set color
        if (gSelected.isSelected(getType(), getGlID()) && s.junctionColorer.getActive() != 1) {
            // override with special colors (unless the color scheme is based on selection)
            GLHelper::setColor(GNENet::selectedConnectionColor);
        } else {
            // Set color depending of the link state
            GLHelper::setColor(GNEInternalLane::colorForLinksState(getLinkState()));
        }
        // draw connection checking whether it is not too small
        if (s.scale < 1.) {
            // If it's small, dra a simple line
            GLHelper::drawLine(myShape);
        } else {
            // draw a list of lines
            GLHelper::drawBoxLines(myShape, myShapeRotations, myShapeLengths, 0.2);
        }
        // Pop name
        glPopName();
        // Pop draw matrix 1
        glPopMatrix();
    }
}


std::string
GNEConnection::getAttribute(SumoXMLAttr key) const {
    if (key == SUMO_ATTR_ID) {
        // used by GNEReferenceCounter
        // @note: may be called for connections without a valid nbCon reference
        return getMicrosimID();
    }
    NBEdge::Connection& nbCon = getNBEdgeConnection();
    switch (key) {
        case SUMO_ATTR_FROM:
            return getEdgeFrom()->getID();
        case SUMO_ATTR_TO:
            return nbCon.toEdge->getID();
        case SUMO_ATTR_FROM_LANE:
            return toString(nbCon.toLane);
        case SUMO_ATTR_TO_LANE:
            return toString(nbCon.toLane);
        case SUMO_ATTR_PASS:
            return toString(nbCon.mayDefinitelyPass);
        case SUMO_ATTR_KEEP_CLEAR:
            return toString(nbCon.keepClear);
        case SUMO_ATTR_CONTPOS:
            return toString(nbCon.contPos);
        case SUMO_ATTR_UNCONTROLLED:
            return toString(!getEdgeFrom()->getNBEdge()->mayBeTLSControlled(nbCon.fromLane, nbCon.toEdge, nbCon.toLane));
        case SUMO_ATTR_VISIBILITY_DISTANCE:
            return toString(nbCon.visibility);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEConnection::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_TO:
        case SUMO_ATTR_FROM_LANE:
        case SUMO_ATTR_TO_LANE:
        case SUMO_ATTR_PASS:
        case SUMO_ATTR_KEEP_CLEAR:
        case SUMO_ATTR_CONTPOS:
        case SUMO_ATTR_UNCONTROLLED:
        case SUMO_ATTR_VISIBILITY_DISTANCE:
            // no special handling
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEConnection::isValid(SumoXMLAttr key, const std::string& value) {
    // Currently ignored before implementation to avoid warnings
    switch (key) {
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_TO:
        case SUMO_ATTR_FROM_LANE:
        case SUMO_ATTR_TO_LANE:
            return false;
        case SUMO_ATTR_PASS:
            return canParse<bool>(value);
        case SUMO_ATTR_KEEP_CLEAR:
            return canParse<bool>(value);
        case SUMO_ATTR_CONTPOS:
            return canParse<SUMOReal>(value);
        case SUMO_ATTR_UNCONTROLLED:
            return false; // XXX see #2599
        //return canParse<bool>(value);
        case SUMO_ATTR_VISIBILITY_DISTANCE:
            return isPositive<SUMOReal>(value);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEConnection::setAttribute(SumoXMLAttr key, const std::string& value) {
    NBEdge::Connection& nbCon = getNBEdgeConnection();
    switch (key) {
        case SUMO_ATTR_PASS:
            nbCon.mayDefinitelyPass = parse<bool>(value);
            break;
        case SUMO_ATTR_KEEP_CLEAR:
            nbCon.keepClear = parse<bool>(value);
            break;
        /*
        case SUMO_ATTR_UNCONTROLLED:
        // XXX see @2599
        break;
        */
        case SUMO_ATTR_CONTPOS:
            nbCon.contPos = parse<SUMOReal>(value);
            break;
        case SUMO_ATTR_VISIBILITY_DISTANCE:
            nbCon.visibility = parse<SUMOReal>(value);
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


/****************************************************************************/
