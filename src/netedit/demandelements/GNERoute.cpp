/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNERoute.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2019
/// @version $Id$
///
// A class for visualizing routes in Netedit
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/common/StringTokenizer.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <netbuild/NBLoadedSUMOTLDef.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/changes/GNEChange_TLS.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNEConnection.h>
#include <netedit/frames/GNESelectorFrame.h>
#include <utils/options/OptionsCont.h>
#include <utils/gui/div/GUIGlobalSelection.h>

#include "GNERoute.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNERoute::GNERoute(GNEViewNet* viewNet) :
    GNEDemandElement(viewNet->getNet()->generateDemandElementID(SUMO_TAG_ROUTE), viewNet, GLO_ROUTE, SUMO_TAG_ROUTE),
    myColor(RGBColor::YELLOW) {
}


GNERoute::GNERoute(GNEViewNet* viewNet, const std::string& routeID, const std::vector<GNEEdge*>& edges, const RGBColor& color) :
    GNEDemandElement(routeID, viewNet, GLO_ROUTE, SUMO_TAG_ROUTE),
    myColor(color) {
    myEdges = edges;
}


GNERoute::~GNERoute() {}


bool 
GNERoute::wasSet(int what) const {
    //return (parametersSet & what) != 0;
    return true;
}


const RGBColor &
GNERoute::getColor() const {
    return myColor;
}


void 
GNERoute::writeDemandElement(OutputDevice& device) const {
    device.openTag(SUMO_TAG_ROUTE);
    device.writeAttr(SUMO_ATTR_ID, getDemandElementID());
    device.writeAttr(SUMO_ATTR_EDGES, parseIDs(myEdges));
    device.writeAttr(SUMO_ATTR_COLOR, toString(myColor));
    device.closeTag();
}


void
GNERoute::moveGeometry(const Position&) {
    // This additional cannot be moved
}


void
GNERoute::commitGeometryMoving(GNEUndoList*) {
    // This additional cannot be moved
}


void
GNERoute::updateGeometry(bool updateGrid) {
    // first check if object has to be removed from grid (SUMOTree)
    if (updateGrid) {
        myViewNet->getNet()->removeGLObjectFromGrid(this);
    }

    // Clear all containers
    myGeometry.clearGeometry();

    // calculate start and end positions dependin of number of lanes
    if (myEdges.size() == 1) {
        // set shape lane as detector shape
        myGeometry.shape = myEdges.front()->getLanes().front()->getShape();

        // Get calculate lenghts and rotations
        myGeometry.calculateShapeRotationsAndLengths();

    } else if (myEdges.size() > 1) {
        // start with the first lane shape
        myGeometry.multiShape.push_back(myEdges.front()->getLanes().front()->getShape());

        // add first shape connection (if exist, in other case leave it empty)
        myGeometry.multiShape.push_back(PositionVector{myEdges.at(0)->getLanes().front()->getShape().back(), myEdges.at(1)->getLanes().front()->getShape().front()});
        for (auto j : myEdges.at(0)->getGNEConnections()) {
            if (j->getLaneTo() == myEdges.at(1)->getLanes().front()) {
                myGeometry.multiShape.back() = j->getShape();
            }
        }

        // append shapes of intermediate lanes AND connections (if exist)
        for (int i = 1; i < ((int)myEdges.size() - 1); i++) {
            // add lane shape
            myGeometry.multiShape.push_back(myEdges.at(i)->getLanes().front()->getShape());
            // add empty shape for connection
            myGeometry.multiShape.push_back(PositionVector{myEdges.at(i)->getLanes().front()->getShape().back(), myEdges.at(i + 1)->getLanes().front()->getShape().front()});
            // set connection shape (if exist). In other case, insert an empty shape
            for (auto j : myEdges.at(i)->getGNEConnections()) {
                if (j->getLaneTo() == myEdges.at(i + 1)->getLanes().front()) {
                    myGeometry.multiShape.back() = j->getShape();
                }
            }
        }

        // append last shape
        myGeometry.multiShape.push_back(myEdges.back()->getLanes().front()->getShape());

        // calculate multi shape rotation and lengths
        myGeometry.calculateMultiShapeRotationsAndLengths();

        // calculate unified shape
        myGeometry.calculateMultiShapeUnified();

        // check integrity
        /*checkRouteIntegrity();*/
    }

    // last step is to check if object has to be added into grid (SUMOTree) again
    if (updateGrid) {
        myViewNet->getNet()->addGLObjectIntoGrid(this);
    }

    // update demand element childs
    for (const auto &i : myDemandElementChilds) {
        i->updateGeometry(updateGrid);
    }
}


Position
GNERoute::getPositionInView() const {
    return Position();
}


std::string
GNERoute::getParentName() const {
    return myViewNet->getNet()->getMicrosimID();
}


void
GNERoute::drawGL(const GUIVisualizationSettings& s) const {
    // only drawn in super mode demand
    if (myViewNet->getViewOptions().showDemandElements()) {
        // calculate route width
        double routeWidth = s.addSize.getExaggeration(s, this) * 0.66;

        // Start drawing adding an gl identificator
        glPushName(getGlID());

        // Add a draw matrix
        glPushMatrix();

        // Start with the drawing of the area traslating matrix to origin
        glTranslated(0, 0, getType());

        // Set color of the base
        if (drawUsingSelectColor()) {
            GLHelper::setColor(s.selectedAdditionalColor);
        } else {
            GLHelper::setColor(myColor);
        }

        // draw route
        if (myGeometry.shape.size() > 0) {
            GLHelper::drawBoxLines(myGeometry.shape, myGeometry.shapeRotations, myGeometry.shapeLengths, routeWidth);
        } else {
            for (int i = 0; i < (int)myGeometry.multiShape.size(); i++) {
                GLHelper::drawBoxLines(myGeometry.multiShape.at(i), myGeometry.multiShapeRotations.at(i), myGeometry.multiShapeLengths.at(i), routeWidth);
            }
        }

        // Pop last matrix
        glPopMatrix();

        // Draw name if isn't being drawn for selecting
        if (!s.drawForSelecting) {
            drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
        }

        // check if dotted contour has to be drawn
        if (!s.drawForSelecting && (myViewNet->getDottedAC() == this)) {
            if (myGeometry.shape.size() > 0) {
                GLHelper::drawShapeDottedContour(getType(), myGeometry.shape, routeWidth);
            } else {
                GLHelper::drawShapeDottedContour(getType(), myGeometry.multiShapeUnified, routeWidth);
            }
        }

        // Pop name
        glPopName();
    }
}


void
GNERoute::selectAttributeCarrier(bool changeFlag) {
    if (!myViewNet) {
        throw ProcessError("ViewNet cannot be nullptr");
    } else {
        gSelected.select(dynamic_cast<GUIGlObject*>(this)->getGlID());
        // add object of list into selected objects
        myViewNet->getViewParent()->getSelectorFrame()->getLockGLObjectTypes()->addedLockedObject(GLO_ROUTE);
        if (changeFlag) {
            mySelected = true;
        }
    }
}


void
GNERoute::unselectAttributeCarrier(bool changeFlag) {
    if (!myViewNet) {
        throw ProcessError("ViewNet cannot be nullptr");
    } else {
        gSelected.deselect(dynamic_cast<GUIGlObject*>(this)->getGlID());
        // remove object of list of selected objects
        myViewNet->getViewParent()->getSelectorFrame()->getLockGLObjectTypes()->removeLockedObject(GLO_ROUTE);
        if (changeFlag) {
            mySelected = false;

        }
    }
}


std::string
GNERoute::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getDemandElementID();
        case SUMO_ATTR_EDGES:
            return parseIDs(myEdges);
        case SUMO_ATTR_COLOR:
            return toString(myColor);
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNERoute::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_EDGES:
        case SUMO_ATTR_COLOR:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_GENERIC:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNERoute::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidDemandElementID(value);
        case SUMO_ATTR_EDGES:
            if (canParse<std::vector<GNEEdge*> >(myViewNet->getNet(), value, false)) {
                // all edges exist, then check if compounds a valid route
                return GNEDemandElement::isRouteValid(parse<std::vector<GNEEdge*> >(myViewNet->getNet(), value), false);
            } else {
                return false;
            }
        case SUMO_ATTR_COLOR:
            return canParse<RGBColor>(value);
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_GENERIC:
            return isGenericParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


std::string
GNERoute::getPopUpID() const {
    return getTagStr() + ": " + getID();
}


std::string
GNERoute::getHierarchyName() const {
    return getTagStr();
}

// ===========================================================================
// private
// ===========================================================================

void
GNERoute::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            changeDemandElementID(value);
            break;
        case SUMO_ATTR_EDGES:
            myEdges = parse<std::vector<GNEEdge*> >(myViewNet->getNet(), value);
            break;
        case SUMO_ATTR_COLOR:
            myColor = parse<RGBColor>(value);
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
    // update geometry
    updateGeometry(true);
}


/****************************************************************************/
