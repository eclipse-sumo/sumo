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

#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNEConnection.h>
#include <netedit/frames/GNESelectorFrame.h>
#include <utils/gui/div/GUIGlobalSelection.h>

#include "GNERoute.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GNERoute::GNERoutePopupMenu) GNERoutePopupMenuMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_ROUTE_APPLY_DISTANCE,     GNERoute::GNERoutePopupMenu::onCmdApplyDistance),
};

// Object implementation
FXIMPLEMENT(GNERoute::GNERoutePopupMenu, GUIGLObjectPopupMenu, GNERoutePopupMenuMap, ARRAYNUMBER(GNERoutePopupMenuMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNERoute::GNERoute(GNEViewNet* viewNet) :
    GNEDemandElement(viewNet->getNet()->generateDemandElementID(SUMO_TAG_ROUTE), viewNet, GLO_ROUTE, SUMO_TAG_ROUTE,
    {}, {}, {}, {}, {}, {}, {}, {}, {}, {}),
    myColor(RGBColor::YELLOW),
    myVClass(SVC_PASSENGER) {
}


GNERoute::GNERoute(GNEViewNet* viewNet, const std::string& routeID, const std::vector<GNEEdge*>& edges, const RGBColor& color, const SUMOVehicleClass VClass) :
    GNEDemandElement(routeID, viewNet, GLO_ROUTE, SUMO_TAG_ROUTE,
    edges, {}, {}, {}, {}, {}, {}, {}, {}, {}),
    myColor(color),
    myVClass(VClass) {
}


GNERoute::~GNERoute() {}


GUIGLObjectPopupMenu*
GNERoute::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GNERoutePopupMenu(app, parent, *this);
    // build header
    buildPopupHeader(ret, app);
    // build menu command for center button and copy cursor position to clipboard
    buildCenterPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
    // buld menu commands for names
    new FXMenuCommand(ret, ("Copy " + getTagStr() + " name to clipboard").c_str(), nullptr, ret, MID_COPY_NAME);
    new FXMenuCommand(ret, ("Copy " + getTagStr() + " typed name to clipboard").c_str(), nullptr, ret, MID_COPY_TYPED_NAME);
    new FXMenuSeparator(ret);
    // build selection and show parameters menu
    myViewNet->buildSelectionACPopupEntry(ret, this);
    buildShowParamsPopupEntry(ret);
    // show option to open demand element dialog
    if (myTagProperty.hasDialog()) {
        new FXMenuCommand(ret, ("Open " + getTagStr() + " Dialog").c_str(), getIcon(), &parent, MID_OPEN_ADDITIONAL_DIALOG);
        new FXMenuSeparator(ret);
    }
    new FXMenuCommand(ret, ("Cursor position in view: " + toString(getPositionInView().x()) + "," + toString(getPositionInView().y())).c_str(), nullptr, nullptr, 0);
    new FXMenuSeparator(ret);
    new FXMenuCommand(ret, "Apply distance along route", nullptr, ret, MID_GNE_ROUTE_APPLY_DISTANCE);
    return ret;
}

const RGBColor&
GNERoute::getColor() const {
    return myColor;
}


void
GNERoute::writeDemandElement(OutputDevice& device) const {
    device.openTag(SUMO_TAG_ROUTE);
    device.writeAttr(SUMO_ATTR_ID, getDemandElementID());
    device.writeAttr(SUMO_ATTR_EDGES, parseIDs(getEdgeParents()));
    device.writeAttr(SUMO_ATTR_COLOR, toString(myColor));
    // write stops associated to this route
    for (const auto& i : getDemandElementChilds()) {
        if (i->getTagProperty().isStop()) {
            i->writeDemandElement(device);
        }
    }
    device.closeTag();
}


bool
GNERoute::isDemandElementValid() const {
    if (getEdgeParents().size() == 0) {
        return false;
    } else if (getEdgeParents().size() == 1) {
        return true;
    } else {
        // check if exist at least a connection between every edge
        for (int i = 1; i < (int)getEdgeParents().size(); i++) {
            if (getRouteCalculatorInstance()->areEdgesConsecutives(myVClass, getEdgeParents().at(i - 1), getEdgeParents().at(i)) == false) {
                return false;
            }
        }
        // there is connections bewteen all edges, then return true
        return true;
    }
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
GNERoute::updateGeometry() {
    // Clear all containers
    myGeometry.clearGeometry();

    // calculate start and end positions depending of number of lanes
    if (getEdgeParents().size() == 1) {
        // append shape
        myGeometry.shape = getEdgeParents().back()->getLanes().front()->getGeometry().shape;

        // calculate multi shape rotation and lengths
        myGeometry.calculateShapeRotationsAndLengths();

    } else if (getEdgeParents().size() > 1) {
        // declare a vector of shapes
        std::vector<PositionVector> multiShape;

        // start with the first lane shape
        multiShape.push_back(getEdgeParents().front()->getLanes().front()->getGeometry().shape);

        // add first shape connection (if exist, in other case leave it empty)
        multiShape.push_back(PositionVector{getEdgeParents().at(0)->getLanes().front()->getGeometry().shape.back(), getEdgeParents().at(1)->getLanes().front()->getGeometry().shape.front()});
        for (auto j : getEdgeParents().at(0)->getGNEConnections()) {
            if (j->getLaneTo() == getEdgeParents().at(1)->getLanes().front()) {
                multiShape.back() = j->getGeometry().shape;
            }
        }

        // append shapes of intermediate lanes AND connections (if exist)
        for (int i = 1; i < ((int)getEdgeParents().size() - 1); i++) {
            // add lane shape
            multiShape.push_back(getEdgeParents().at(i)->getLanes().front()->getGeometry().shape);
            // add empty shape for connection
            multiShape.push_back(PositionVector{getEdgeParents().at(i)->getLanes().front()->getGeometry().shape.back(), getEdgeParents().at(i + 1)->getLanes().front()->getGeometry().shape.front()});
            // set connection shape (if exist). In other case, insert an empty shape
            for (auto j : getEdgeParents().at(i)->getGNEConnections()) {
                if (j->getLaneTo() == getEdgeParents().at(i + 1)->getLanes().front()) {
                    multiShape.back() = j->getGeometry().shape;
                }
            }
        }

        // append last shape
        multiShape.push_back(getEdgeParents().back()->getLanes().front()->getGeometry().shape);

        // calculate unified shape
        for (auto i : multiShape) {
            myGeometry.shape.append(i);
        }
        myGeometry.shape.removeDoublePoints();

        // calculate multi shape rotation and lengths
        myGeometry.calculateShapeRotationsAndLengths();
    }

    // update demand element childs
    for (const auto& i : getDemandElementChilds()) {
        i->updateGeometry();
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
GNERoute::drawGL(const GUIVisualizationSettings& /*s*/) const {
    // Routes are drawn in GNEEdges
}


void
GNERoute::selectAttributeCarrier(bool changeFlag) {
    if (!myViewNet) {
        throw ProcessError("ViewNet cannot be nullptr");
    } else {
        gSelected.select(getGlID());
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
        gSelected.deselect(getGlID());
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
            return parseIDs(getEdgeParents());
        case SUMO_ATTR_COLOR:
            return toString(myColor);
        case SUMO_ATTR_VCLASS:
            return toString(myVClass);
        case GNE_ATTR_EMBEDDED_ROUTE:
            return toString(getDemandElementParents().size() > 0);
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
        case GNE_ATTR_EMBEDDED_ROUTE:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_GENERIC:
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), key, value));
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
        case GNE_ATTR_EMBEDDED_ROUTE:
            if (value.empty()) {
                return true;
            } else if(isValidDemandElementID(value)) {
                return (myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_VEHICLE, false) || 
                        myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_FLOW, false) || 
                        myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_TRIP, false));
            } else {
                return false;
            }
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_GENERIC:
            return isGenericParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool 
GNERoute::isAttributeSet(SumoXMLAttr /*key*/) const {
    return true;
}


std::string
GNERoute::getPopUpID() const {
    return getTagStr();
}


std::string
GNERoute::getHierarchyName() const {
    return getTagStr() + ": " + getAttribute(SUMO_ATTR_ID) ;
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
            changeEdgeParents(this, value);
            break;
        case SUMO_ATTR_COLOR:
            myColor = parse<RGBColor>(value);
            break;
        case GNE_ATTR_EMBEDDED_ROUTE:
            if (value.empty()) {
                // if value is't empty, remove demand element parent if had it
                if (getDemandElementChilds().size() > 0) {
                    removeDemandElementParent(getDemandElementParents().front());
                }
            } else {
                // if value isn't empty, add or change element parent
                if (getDemandElementChilds().size() > 0) {
                    changeDemandElementParent(this, value, 0);
                } else {
                    removeDemandElementParent(getDemandElementParents().front());
                }
            }
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

/* -------------------------------------------------------------------------
 * GNERoute::GNERoutePopupMenu - methods
 * ----------------------------------------------------------------------- */
GNERoute::GNERoutePopupMenu::GNERoutePopupMenu(GUIMainWindow& app, GUISUMOAbstractView& parent, GUIGlObject& o) :
    GUIGLObjectPopupMenu(app, parent, o)
{ }


GNERoute::GNERoutePopupMenu::~GNERoutePopupMenu() {}

long
GNERoute::GNERoutePopupMenu::onCmdApplyDistance(FXObject*, FXSelector, void*) {
    GNERoute* route = static_cast<GNERoute*>(myObject);
    GNEViewNet* viewNet = static_cast<GNEViewNet*>(myParent);
    GNEUndoList* undoList =  route->myViewNet->getUndoList();
    undoList->p_begin("apply distance along route");
    double dist = route->getEdgeParents().front()->getNBEdge()->getDistance();
    for (GNEEdge* edge : route->getEdgeParents()) {
        undoList->p_add(new GNEChange_Attribute(edge, viewNet->getNet(), SUMO_ATTR_DISTANCE, toString(dist), true, edge->getAttribute(SUMO_ATTR_DISTANCE)));
        dist += edge->getNBEdge()->getFinalLength();
    }
    undoList->p_end();
    return 1;
}


/****************************************************************************/
