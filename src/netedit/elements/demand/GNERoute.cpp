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
/// @file    GNERoute.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2019
///
// A class for visualizing routes in Netedit
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/network/GNEJunction.h>
#include <utils/gui/windows/GUIAppEnum.h>

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
// GNERoute::GNERoutePopupMenu - methods
// ===========================================================================

GNERoute::GNERoutePopupMenu::GNERoutePopupMenu(GUIMainWindow& app, GUISUMOAbstractView& parent, GUIGlObject& o) :
    GUIGLObjectPopupMenu(app, parent, o) {
}


GNERoute::GNERoutePopupMenu::~GNERoutePopupMenu() {}


long
GNERoute::GNERoutePopupMenu::onCmdApplyDistance(FXObject*, FXSelector, void*) {
    GNERoute* route = static_cast<GNERoute*>(myObject);
    GNEUndoList* undoList = route->myNet->getViewNet()->getUndoList();
    undoList->p_begin("apply distance along route");
    double dist = (route->getParentEdges().size() > 0) ? route->getParentEdges().front()->getNBEdge()->getDistance() : 0;
    for (GNEEdge* edge : route->getParentEdges()) {
        undoList->p_add(new GNEChange_Attribute(edge, SUMO_ATTR_DISTANCE, toString(dist), edge->getAttribute(SUMO_ATTR_DISTANCE)));
        dist += edge->getNBEdge()->getFinalLength();
    }
    undoList->p_end();
    return 1;
}

// ===========================================================================
// GNERoute - methods
// ===========================================================================

GNERoute::GNERoute(GNENet* net) :
    GNEDemandElement(net->generateDemandElementID("", SUMO_TAG_ROUTE), net, GLO_ROUTE, SUMO_TAG_ROUTE,
        {}, {}, {}, {}, {}, {}, {}, {},     // Parents
        {}, {}, {}, {}, {}, {}, {}, {}),    // Childrens
    Parameterised(),
    myColor(RGBColor::YELLOW),
    myVClass(SVC_PASSENGER) {
}


GNERoute::GNERoute(GNENet* net, const GNERouteHandler::RouteParameter& routeParameters) :
    GNEDemandElement(routeParameters.routeID, net, GLO_ROUTE, SUMO_TAG_ROUTE,
    {}, routeParameters.edges, {}, {}, {}, {}, {}, {},  // Parents
    {}, {}, {}, {}, {}, {}, {}, {}),                    // Childrens
    Parameterised(routeParameters.parameters),
    myColor(routeParameters.color),
    myVClass(routeParameters.vClass) {
}


GNERoute::GNERoute(GNENet* net, GNEDemandElement* vehicleParent, const GNERouteHandler::RouteParameter& routeParameters) :
    GNEDemandElement(net->generateDemandElementID("", SUMO_TAG_EMBEDDEDROUTE), net, GLO_EMBEDDEDROUTE, SUMO_TAG_EMBEDDEDROUTE,
        {}, routeParameters.edges, {}, {}, {}, {}, {vehicleParent}, {}, // Parents
        {}, {}, {}, {}, {}, {}, {}, {}),                                // Childrens
    Parameterised(routeParameters.parameters),
    myColor(routeParameters.color),
    myVClass(routeParameters.vClass) {
}


GNERoute::GNERoute(GNEDemandElement* route) :
    GNEDemandElement(route->getNet()->generateDemandElementID("", SUMO_TAG_ROUTE), route->getNet(), GLO_ROUTE, SUMO_TAG_ROUTE,
        {}, route->getParentEdges(), {}, {}, {}, {}, {}, {},    // Parents
        {}, {}, {}, {}, {}, {}, {}, {}),                        // Childrens
    Parameterised(),
    myColor(route->getColor()),
    myVClass(route->getVClass()) {
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
    myNet->getViewNet()->buildSelectionACPopupEntry(ret, this);
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


void
GNERoute::writeDemandElement(OutputDevice& device) const {
    device.openTag(SUMO_TAG_ROUTE);
    device.writeAttr(SUMO_ATTR_EDGES, parseIDs(getParentEdges()));
    device.writeAttr(SUMO_ATTR_COLOR, toString(myColor));
    // write extra attributes depending if is an embedded route
    if (myTagProperty.getTag() == SUMO_TAG_ROUTE) {
        device.writeAttr(SUMO_ATTR_ID, getID());
        // write stops associated to this route
        for (const auto& stop : getChildDemandElements()) {
            if (stop->getTagProperty().isStop()) {
                stop->writeDemandElement(device);
            }
        }
    }
    // write parameters
    writeParams(device);
    // close tag
    device.closeTag();
}


bool
GNERoute::isDemandElementValid() const {
    if ((getParentEdges().size() == 2) && (getParentEdges().at(0) == getParentEdges().at(1))) {
        // from and to are the same edges, then return true
        return true;
    } else if (getParentEdges().size() > 0) {
        // check that exist a connection between every edge
        return isRouteValid(getParentEdges()).empty();
    } else {
        return false;
    }
}


std::string
GNERoute::getDemandElementProblem() const {
    // return string with the problem obtained from isRouteValid
    return isRouteValid(getParentEdges());
}


void
GNERoute::fixDemandElementProblem() {
    // currently the only solution is removing Route
}


GNEEdge*
GNERoute::getFromEdge() const {
    return getParentEdges().front();
}


GNEEdge*
GNERoute::getToEdge() const {
    return getParentEdges().back();
}


SUMOVehicleClass
GNERoute::getVClass() const {
    return myVClass;
}


const RGBColor&
GNERoute::getColor() const {
    return myColor;
}


void
GNERoute::startGeometryMoving() {
    // Routes cannot be moved
}


void
GNERoute::endGeometryMoving() {
    // Routes cannot be moved
}


void
GNERoute::moveGeometry(const Position&) {
    // Routes cannot be moved
}


void
GNERoute::commitGeometryMoving(GNEUndoList*) {
    // Routes cannot be moved
}


void
GNERoute::updateGeometry() {
    // calculate geometry path
    GNEGeometry::calculateEdgeGeometricPath(this, myDemandElementSegmentGeometry, getParentEdges(),
                                            getVClass(), getFirstAllowedVehicleLane(), getLastAllowedVehicleLane());
    // update child demand elementss
    for (const auto& i : getChildDemandElements()) {
        if (!i->getTagProperty().isPersonStop() && !i->getTagProperty().isStop()) {
            i->updateGeometry();
        }
    }
}


void
GNERoute::updateDottedContour() {
    //
}


void
GNERoute::updatePartialGeometry(const GNEEdge* edge) {
    // calculate geometry path
    GNEGeometry::updateGeometricPath(myDemandElementSegmentGeometry, edge);
    // update child demand elementss
    for (const auto& i : getChildDemandElements()) {
        if (!i->getTagProperty().isPersonStop() && !i->getTagProperty().isStop()) {
            i->updatePartialGeometry(edge);
        }
    }
}


void
GNERoute::computePath() {
    // nothing to compute
}


void
GNERoute::invalidatePath() {
    // nothing to invalidate
}


Position
GNERoute::getPositionInView() const {
    return Position();
}


std::string
GNERoute::getParentName() const {
    return getParentEdges().front()->getID();
}


Boundary
GNERoute::getCenteringBoundary() const {
    Boundary routeBoundary;
    // return the combination of all parent edges's boundaries
    for (const auto& i : getParentEdges()) {
        routeBoundary.add(i->getCenteringBoundary());
    }
    // check if is valid
    if (routeBoundary.isInitialised()) {
        return routeBoundary;
    } else {
        return Boundary(-0.1, -0.1, 0.1, 0.1);
    }
}


void
GNERoute::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* originalElement, const GNENetworkElement* newElement, GNEUndoList* undoList) {
    // check that both network elementes are edges
    if ((originalElement->getTagProperty().getTag() == SUMO_TAG_EDGE) &&
            (originalElement->getTagProperty().getTag() == SUMO_TAG_EDGE)) {
        // obtain new list of route edges
        std::string newRouteEdges = getNewListOfParents(originalElement, newElement);
        // update route edges
        if (newRouteEdges.size() > 0) {
            setAttribute(SUMO_ATTR_EDGES, newRouteEdges, undoList);
        }
    }
}


void
GNERoute::drawGL(const GUIVisualizationSettings& /*s*/) const {
    // Routes are drawn in GNEEdges
}


std::string
GNERoute::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getID();
        case SUMO_ATTR_EDGES:
            return parseIDs(getParentEdges());
        case SUMO_ATTR_COLOR:
            return toString(myColor);
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARAMETERS:
            return getParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNERoute::getAttributeDouble(SumoXMLAttr /*key*/) const {
    return 0;
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
        case GNE_ATTR_PARAMETERS:
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
            if (canParse<std::vector<GNEEdge*> >(myNet, value, false)) {
                // all edges exist, then check if compounds a valid route
                return isRouteValid(parse<std::vector<GNEEdge*> >(myNet, value)).empty();
            } else {
                return false;
            }
        case SUMO_ATTR_COLOR:
            return canParse<RGBColor>(value);
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return Parameterised::areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNERoute::enableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    //
}


void
GNERoute::disableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    //
}


bool
GNERoute::isAttributeEnabled(SumoXMLAttr /*key*/) const {
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


std::string
GNERoute::isRouteValid(const std::vector<GNEEdge*>& edges) {
    if (edges.size() == 0) {
        // routes cannot be empty
        return ("list of route edges cannot be empty");
    } else if (edges.size() == 1) {
        // routes with a single edge are valid, then return an empty string
        return ("");
    } else {
        // iterate over edges to check that compounds a chain
        auto it = edges.begin();
        while (it != edges.end() - 1) {
            const GNEEdge* currentEdge = *it;
            const GNEEdge* nextEdge = *(it + 1);
            // same consecutive edges aren't allowed
            if (currentEdge->getID() == nextEdge->getID()) {
                return ("consecutive duplicated edges (" + currentEdge->getID() + ") aren't allowed in a route");
            }
            // obtain outgoing edges of currentEdge
            const std::vector<GNEEdge*>& outgoingEdges = currentEdge->getSecondParentJunction()->getGNEOutgoingEdges();
            // check if nextEdge is in outgoingEdges
            if (std::find(outgoingEdges.begin(), outgoingEdges.end(), nextEdge) == outgoingEdges.end()) {
                return ("Edges '" + currentEdge->getID() + "' and '" + nextEdge->getID() + "' aren't consecutives");
            }
            it++;
        }
        // all edges consecutives, then return an empty string
        return ("");
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNERoute::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            myNet->getAttributeCarriers()->updateID(this, value);
            break;
        case SUMO_ATTR_EDGES:
            replaceParentEdges(this, value);
            // compute route
            updateGeometry();
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
        case GNE_ATTR_PARAMETERS:
            setParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNERoute::setEnabledAttribute(const int /*enabledAttributes*/) {
    //
}


/****************************************************************************/
