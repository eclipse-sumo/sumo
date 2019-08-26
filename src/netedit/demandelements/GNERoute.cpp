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

#include <netbuild/NBNode.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/frames/GNESelectorFrame.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <utils/common/StringTokenizer.h>
#include <utils/gui/div/GUIGlobalSelection.h>
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
    GNEViewNet* viewNet = static_cast<GNEViewNet*>(myParent);
    GNEUndoList* undoList =  route->myViewNet->getUndoList();
    undoList->p_begin("apply distance along route");
    double dist = (route->getEdgeParents().size() > 0) ? route->getEdgeParents().front()->getNBEdge()->getDistance() : 0;
    for (GNEEdge* edge : route->getEdgeParents()) {
        undoList->p_add(new GNEChange_Attribute(edge, viewNet->getNet(), SUMO_ATTR_DISTANCE, toString(dist), true, edge->getAttribute(SUMO_ATTR_DISTANCE)));
        dist += edge->getNBEdge()->getFinalLength();
    }
    undoList->p_end();
    return 1;
}

// ===========================================================================
// GNERoute - methods
// ===========================================================================

GNERoute::GNERoute(GNEViewNet* viewNet) :
    GNEDemandElement(viewNet->getNet()->generateDemandElementID("", SUMO_TAG_ROUTE), viewNet, GLO_ROUTE, SUMO_TAG_ROUTE,
{}, {}, {}, {}, {}, {}, {}, {}, {}, {}),
Parameterised(),
myColor(RGBColor::YELLOW),
myVClass(SVC_PASSENGER) {
}


GNERoute::GNERoute(GNEViewNet* viewNet, const GNERouteHandler::RouteParameter& routeParameters) :
    GNEDemandElement(routeParameters.routeID, viewNet, GLO_ROUTE, SUMO_TAG_ROUTE,
                     routeParameters.edges, {}, {}, {}, {}, {}, {}, {}, {}, {}),
Parameterised(routeParameters.genericParameters),
myColor(routeParameters.color),
myVClass(routeParameters.VClass) {
}


GNERoute::GNERoute(GNEViewNet* viewNet, GNEDemandElement* vehicleParent, const GNERouteHandler::RouteParameter& routeParameters) :
    GNEDemandElement(viewNet->getNet()->generateDemandElementID("", SUMO_TAG_EMBEDDEDROUTE), viewNet, GLO_EMBEDDEDROUTE, SUMO_TAG_EMBEDDEDROUTE,
                     routeParameters.edges, {}, {}, {}, {vehicleParent}, {}, {}, {}, {}, {}),
Parameterised(routeParameters.genericParameters),
myColor(routeParameters.color),
myVClass(routeParameters.VClass) {
}


GNERoute::GNERoute(GNEDemandElement* route) :
    GNEDemandElement(route->getViewNet()->getNet()->generateDemandElementID("", SUMO_TAG_ROUTE), route->getViewNet(), GLO_ROUTE, SUMO_TAG_ROUTE,
                     route->getEdgeParents(), {}, {}, {}, {}, {}, {}, {}, {}, {}),
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


void
GNERoute::writeDemandElement(OutputDevice& device) const {
    device.openTag(SUMO_TAG_ROUTE);
    device.writeAttr(SUMO_ATTR_EDGES, parseIDs(getEdgeParents()));
    device.writeAttr(SUMO_ATTR_COLOR, toString(myColor));
    // write extra attributes depending if is an embedded route
    if (myTagProperty.getTag() == SUMO_TAG_ROUTE) {
        device.writeAttr(SUMO_ATTR_ID, getDemandElementID());
        // write stops associated to this route
        for (const auto& i : getDemandElementChildren()) {
            if (i->getTagProperty().isStop()) {
                i->writeDemandElement(device);
            }
        }
    }
    // write generic parameters
    writeParams(device);
    // close tag
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
            if (getRouteCalculatorInstance()->areEdgesConsecutives(myVClass, getEdgeParents().at((int)i - 1), getEdgeParents().at(i)) == false) {
                return false;
            }
        }
        // there is connections bewteen all edges, then return true
        return true;
    }
}


std::string
GNERoute::getDemandElementProblem() const {
    if (getEdgeParents().size() == 0) {
        return ("A route need at least one edge");
    } else {
        // check if exist at least a connection between every edge
        for (int i = 1; i < (int)getEdgeParents().size(); i++) {
            if (getRouteCalculatorInstance()->areEdgesConsecutives(myVClass, getEdgeParents().at((int)i - 1), getEdgeParents().at(i)) == false) {
                return ("Edge '" + getEdgeParents().at((int)i - 1)->getID() + "' and edge '" + getEdgeParents().at(i)->getID() + "' aren't consecutives");
            }
        }
        // there is connections bewteen all edges, then all ok
        return "";
    }
}


void
GNERoute::fixDemandElementProblem() {
    // currently the only solution is removing Route
}


GNEEdge*
GNERoute::getFromEdge() const {
    return getEdgeParents().front();
}


GNEEdge*
GNERoute::getToEdge() const {
    return getEdgeParents().back();
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
GNERoute::compute() {
    // Nothing to compute
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
    // first check if geometry is deprecated
    if (myDemandElementSegmentGeometry.geometryDeprecated) {
        // first clear geometry
        myDemandElementSegmentGeometry.clearDemandElementSegmentGeometry();
        // declare vector for saving a reference to lane geometry and connection shapes
        std::vector<std::pair<GNEEdge*, GNENetElement::NetElementGeometry> > laneGeometries;
        std::vector<PositionVector> connectionShapes;
        // obtain all lane shapes
        for (const auto& i : getEdgeParents()) {
            laneGeometries.push_back(std::make_pair(i, i->getLaneByVClass(myVClass)->getGeometry()));
        }
        // resize connectionShapes
        connectionShapes.resize(laneGeometries.size());
        // iterate over edge parents
        for (int i = 0; i < ((int)getEdgeParents().size() - 1); i++) {
            // obtain NBEdges from both edges
            NBEdge* nbFrom = getEdgeParents().at(i)->getNBEdge();
            NBEdge* nbTo = getEdgeParents().at(i + 1)->getNBEdge();
            // declare a flags
            bool connectionFound = false;
            // iterate over all connections of NBFrom
            for (NBEdge::Connection c : nbFrom->getConnectionsFromLane(-1, nbTo, -1)) {
                //check if given VClass is allowed for from and to lanes
                if (!connectionFound && ((nbFrom->getPermissions(c.fromLane) & nbTo->getPermissions(c.toLane) & myVClass) == myVClass)) {
                    // save shape
                    if (c.customShape.size() != 0) {
                        connectionShapes.at(i) = c.customShape;
                    } else if (nbFrom->getToNode()->getShape().area() > 4) {
                        if (c.shape.size() != 0) {
                            connectionShapes.at(i) = c.shape;
                            // only append via shape if it exists
                            if (c.haveVia) {
                                connectionShapes.at(i).append(c.viaShape);
                            }
                        } else {
                            // manually calculate smooth shape
                            PositionVector laneShapeFrom = nbFrom->getLanes().at(c.fromLane).shape;
                            PositionVector laneShapeTo = c.toEdge->getLanes().at(c.toLane).shape;
                            // Calculate shape so something can be drawn immidiately
                            connectionShapes.at(i) = nbFrom->getToNode()->computeSmoothShape(
                                                         laneShapeFrom,
                                                         laneShapeTo,
                                                         5, nbFrom->getTurnDestination() == c.toEdge,
                                                         (double) 5. * (double) nbFrom->getNumLanes(),
                                                         (double) 5. * (double) c.toEdge->getNumLanes());
                        }
                    }
                    // change flag
                    connectionFound = true;
                }
            }
        }
        // fill shapeSegments
        for (int i = 0; i < (int)laneGeometries.size(); i++) {
            // set lane shapes
            for (int j = 0; j < (int)laneGeometries.at(i).second.shape.size(); j++) {
                // save position and rotations (to avoid useless calculations)
                if (j < (int)laneGeometries.at(i).second.shape.size() - 1) {
                    myDemandElementSegmentGeometry.insertEdgeLengthRotSegment(this, laneGeometries.at(i).first,
                            laneGeometries.at(i).second.shape[j], laneGeometries.at(i).second.shapeLengths[j],
                            laneGeometries.at(i).second.shapeRotations[j], true, true);
                } else {
                    myDemandElementSegmentGeometry.insertEdgeSegment(this, laneGeometries.at(i).first,
                            laneGeometries.at(i).second.shape[j], true, true);
                }
            }
            // set connection shapes
            for (const auto& connectionShapePos : connectionShapes.at(i)) {
                myDemandElementSegmentGeometry.insertJunctionSegment(this, laneGeometries.at(i).first->getGNEJunctionDestiny(), connectionShapePos, true, true);
            }
        }
        // calculate entire shape, rotations and lengths
        myDemandElementSegmentGeometry.calculatePartialShapeRotationsAndLengths();
        // update demand element childrens
        for (const auto& i : getDemandElementChildren()) {
            i->updateGeometry();
        }
        // set geometry as non-deprecated
        myDemandElementSegmentGeometry.geometryDeprecated = false;
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


Boundary
GNERoute::getCenteringBoundary() const {
    Boundary routeBoundary;
    // return the combination of all edge parents's boundaries
    for (const auto& i : getEdgeParents()) {
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
        myViewNet->getViewParent()->getSelectorFrame()->getLockGLObjectTypes()->addedLockedObject(getType());
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
        myViewNet->getViewParent()->getSelectorFrame()->getLockGLObjectTypes()->removeLockedObject(getType());
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
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
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
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_GENERIC:
            return isGenericParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNERoute::enableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
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
GNERoute::getGenericParametersStr() const {
    std::string result;
    // Generate an string using the following structure: "key1=value1|key2=value2|...
    for (auto i : getParametersMap()) {
        result += i.first + "=" + i.second + "|";
    }
    // remove the last "|"
    if (!result.empty()) {
        result.pop_back();
    }
    return result;
}


std::vector<std::pair<std::string, std::string> >
GNERoute::getGenericParameters() const {
    std::vector<std::pair<std::string, std::string> >  result;
    // iterate over parameters map and fill result
    for (auto i : getParametersMap()) {
        result.push_back(std::make_pair(i.first, i.second));
    }
    return result;
}


void
GNERoute::setGenericParametersStr(const std::string& value) {
    // clear parameters
    clearParameter();
    // separate value in a vector of string using | as separator
    std::vector<std::string> parsedValues;
    StringTokenizer stValues(value, "|", true);
    while (stValues.hasNext()) {
        parsedValues.push_back(stValues.next());
    }
    // check that parsed values (A=B)can be parsed in generic parameters
    for (auto i : parsedValues) {
        std::vector<std::string> parsedParameters;
        StringTokenizer stParam(i, "=", true);
        while (stParam.hasNext()) {
            parsedParameters.push_back(stParam.next());
        }
        // Check that parsed parameters are exactly two and contains valid chracters
        if (parsedParameters.size() == 2 && SUMOXMLDefinitions::isValidGenericParameterKey(parsedParameters.front()) && SUMOXMLDefinitions::isValidGenericParameterValue(parsedParameters.back())) {
            setParameter(parsedParameters.front(), parsedParameters.back());
        }
    }
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
            // change flag for geometry deprecating
            myDemandElementSegmentGeometry.geometryDeprecated = true;
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
}


void
GNERoute::setEnabledAttribute(const int /*enabledAttributes*/) {
    //
}

/****************************************************************************/
