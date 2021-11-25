/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/div/GUIDesigns.h>

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
    undoList->begin(GUIIcon::ROUTE, "apply distance along route");
    double dist = (route->getParentEdges().size() > 0) ? route->getParentEdges().front()->getNBEdge()->getDistance() : 0;
    for (GNEEdge* edge : route->getParentEdges()) {
        undoList->changeAttribute(new GNEChange_Attribute(edge, SUMO_ATTR_DISTANCE, toString(dist), edge->getAttribute(SUMO_ATTR_DISTANCE)));
        dist += edge->getNBEdge()->getFinalLength();
    }
    undoList->end();
    return 1;
}

// ===========================================================================
// GNERoute - methods
// ===========================================================================

GNERoute::GNERoute(SumoXMLTag tag, GNENet* net) :
    GNEDemandElement("", net, GLO_ROUTE, tag,
        GNEPathManager::PathElement::Options::DEMAND_ELEMENT | GNEPathManager::PathElement::Options::ROUTE,
        {}, {}, {}, {}, {}, {}, {}, {}),
    Parameterised(),
    myColor(RGBColor::YELLOW),
    myRepeat(0),
    myCycleTime(0),
    myVClass(SVC_PASSENGER) {
    // reset default values
    resetDefaultValues();
}


GNERoute::GNERoute(GNENet* net) :
    GNEDemandElement(net->getAttributeCarriers()->generateDemandElementID(SUMO_TAG_ROUTE), net, GLO_ROUTE, SUMO_TAG_ROUTE,
        GNEPathManager::PathElement::Options::DEMAND_ELEMENT | GNEPathManager::PathElement::Options::ROUTE,
        {}, {}, {}, {}, {}, {}, {}, {}),
    Parameterised(),
    myColor(RGBColor::YELLOW),
    myRepeat(0),
    myCycleTime(0),
    myVClass(SVC_PASSENGER) {
    // reset default values
    resetDefaultValues();
}


GNERoute::GNERoute(GNENet* net, const std::string& id, SUMOVehicleClass vClass, const std::vector<GNEEdge*>& edges,
                   const RGBColor& color, const int repeat, const SUMOTime cycleTime, const std::map<std::string, std::string>& parameters) :
    GNEDemandElement(id, net, GLO_ROUTE, SUMO_TAG_ROUTE,
        GNEPathManager::PathElement::Options::DEMAND_ELEMENT | GNEPathManager::PathElement::Options::ROUTE,
        {}, edges, {}, {}, {}, {}, {}, {}),
    Parameterised(parameters),
    myColor(color),
    myRepeat(repeat),
    myCycleTime(cycleTime),
    myVClass(vClass) {
}


GNERoute::GNERoute(GNENet* net, GNEDemandElement* vehicleParent, const std::vector<GNEEdge*>& edges,
                   const RGBColor& color, const int repeat, const SUMOTime cycleTime, const std::map<std::string, std::string>& parameters) :
    GNEDemandElement(vehicleParent, net, GLO_ROUTE, GNE_TAG_ROUTE_EMBEDDED,
        GNEPathManager::PathElement::Options::DEMAND_ELEMENT | GNEPathManager::PathElement::Options::ROUTE,
        {}, edges, {}, {}, {}, {}, {vehicleParent}, {}),
    Parameterised(parameters),
    myColor(color),
    myRepeat(repeat),
    myCycleTime(cycleTime),
    myVClass(vehicleParent->getVClass()) {
}


GNERoute::GNERoute(GNEDemandElement* route) :
    GNEDemandElement(route, route->getNet(), GLO_ROUTE, SUMO_TAG_ROUTE,
                     GNEPathManager::PathElement::Options::DEMAND_ELEMENT | GNEPathManager::PathElement::Options::ROUTE,
        {}, route->getParentEdges(), {}, {}, {}, {}, {}, {}),
    Parameterised(),
    myColor(route->getColor()),
    myRepeat(parse<int>(route->getAttribute(SUMO_ATTR_REPEAT))),
    myCycleTime(parse<SUMOTime>(route->getAttribute(SUMO_ATTR_CYCLETIME))),
    myVClass(route->getVClass()) {
}


GNERoute::~GNERoute() {}


GNEMoveOperation*
GNERoute::getMoveOperation() {
    return nullptr;
}


GUIGLObjectPopupMenu*
GNERoute::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GNERoutePopupMenu(app, parent, *this);
    // build header
    buildPopupHeader(ret, app);
    // build menu command for center button and copy cursor position to clipboard
    buildCenterPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
    // buld menu commands for names
    GUIDesigns::buildFXMenuCommand(ret, "Copy " + getTagStr() + " name to clipboard", nullptr, ret, MID_COPY_NAME);
    GUIDesigns::buildFXMenuCommand(ret, "Copy " + getTagStr() + " typed name to clipboard", nullptr, ret, MID_COPY_TYPED_NAME);
    new FXMenuSeparator(ret);
    // build selection and show parameters menu
    myNet->getViewNet()->buildSelectionACPopupEntry(ret, this);
    buildShowParamsPopupEntry(ret);
    // show option to open demand element dialog
    if (myTagProperty.hasDialog()) {
        GUIDesigns::buildFXMenuCommand(ret, "Open " + getTagStr() + " Dialog", getIcon(), &parent, MID_OPEN_ADDITIONAL_DIALOG);
        new FXMenuSeparator(ret);
    }
    GUIDesigns::buildFXMenuCommand(ret, "Cursor position in view: " + toString(getPositionInView().x()) + "," + toString(getPositionInView().y()), nullptr, nullptr, 0);
    new FXMenuSeparator(ret);
    GUIDesigns::buildFXMenuCommand(ret, "Apply distance along route", nullptr, ret, MID_GNE_ROUTE_APPLY_DISTANCE);
    return ret;
}


void
GNERoute::writeDemandElement(OutputDevice& device) const {
    device.openTag(SUMO_TAG_ROUTE);
    device.writeAttr(SUMO_ATTR_EDGES, parseIDs(getParentEdges()));
    device.writeAttr(SUMO_ATTR_COLOR, toString(myColor));
    if (myRepeat != 0) {
        device.writeAttr(SUMO_ATTR_REPEAT, toString(myRepeat));
    }
    if (myCycleTime != 0) {
        device.writeAttr(SUMO_ATTR_CYCLETIME, toString(myCycleTime));
    }
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


SUMOVehicleClass
GNERoute::getVClass() const {
    return myVClass;
}


const RGBColor&
GNERoute::getColor() const {
    return myColor;
}


void
GNERoute::updateGeometry() {
    // compute geometry
    computePathElement();
    // update child demand elementss
    for (const auto& demandElement : getChildDemandElements()) {
        if (!demandElement->getTagProperty().isStopPerson() && !demandElement->getTagProperty().isStop()) {
            demandElement->updateGeometry();
        }
    }
}


Position
GNERoute::getPositionInView() const {
    return getFirstPathLane()->getPositionInView();
}


std::string
GNERoute::getParentName() const {
    return getParentEdges().front()->getID();
}


double
GNERoute::getExaggeration(const GUIVisualizationSettings& s) const {
    return s.vehicleSize.getExaggeration(s, this);
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
    // obtain new list of route edges
    std::string newRouteEdges = getNewListOfParents(originalElement, newElement);
    // update route edges
    if (newRouteEdges.size() > 0) {
        setAttribute(SUMO_ATTR_EDGES, newRouteEdges, undoList);
    }
}


void
GNERoute::drawGL(const GUIVisualizationSettings& /*s*/) const {
    // Routes are drawn in drawPartialGL
}


void
GNERoute::computePathElement() {
    if (myTagProperty.getTag() == GNE_TAG_ROUTE_EMBEDDED) {
        // get parent vehicle
        const GNEDemandElement* parentVehicle = getParentDemandElements().at(0);
        // declare lane vector
        std::vector<GNELane*> lanes;
        // get first and last path lane
        GNELane* firstLane = parentVehicle->getFirstPathLane();
        GNELane* lastLane = parentVehicle->getLastPathLane();
        // insert first vehicle lane
        if (firstLane) {
            lanes.push_back(firstLane);
        }
        // add middle lanes
        for (int i = 1; i < ((int)getParentEdges().size() - 1); i++) {
            lanes.push_back(getParentEdges().at(i)->getLaneByAllowedVClass(getVClass()));
        }
        // insert last vehicle lane
        if (lastLane) {
            lanes.push_back(lastLane);
        }
        // calculate consecutive path using vClass of vehicle parent
        myNet->getPathManager()->calculateConsecutivePathLanes(this, lanes);
    } else {
        // calculate path using SVC_PASSENGER
        myNet->getPathManager()->calculateConsecutivePathEdges(this, SVC_PASSENGER, getParentEdges());
        // if path is empty, then calculate path again using SVC_IGNORING
        if (!myNet->getPathManager()->isPathValid(this)) {
            myNet->getPathManager()->calculateConsecutivePathEdges(this, SVC_IGNORING, getParentEdges());
        }
    }
}


void
GNERoute::drawPartialGL(const GUIVisualizationSettings& s, const GNELane* lane, const GNEPathManager::Segment* segment, const double offsetFront) const {
    // get inspected and front flags
    const bool dottedElement = myNet->getViewNet()->isAttributeCarrierInspected(this) || (myNet->getViewNet()->getFrontAttributeCarrier() == this);
    // check conditions
    if (myNet->getViewNet()->getNetworkViewOptions().showDemandElements() &&
            myNet->getViewNet()->getDataViewOptions().showDemandElements() &&
            myNet->getViewNet()->getDemandViewOptions().showNonInspectedDemandElements(this) &&
            myNet->getPathManager()->getPathDraw()->drawPathGeometry(dottedElement, lane, myTagProperty.getTag())) {
        // get embedded route flag
        const bool embedded = (myTagProperty.getTag() == GNE_TAG_ROUTE_EMBEDDED);
        // get route width
        const double routeWidth = getExaggeration(s) * (embedded ? s.widthSettings.embeddedRouteWidth : s.widthSettings.routeWidth);
        // calculate startPos
        const double geometryDepartPos = embedded ? (getParentDemandElements().at(0)->getAttributeDouble(SUMO_ATTR_DEPARTPOS) + getParentDemandElements().at(0)->getParentDemandElements().at(0)->getAttributeDouble(SUMO_ATTR_LENGTH)) : -1;
        // get endPos
        const double geometryEndPos = embedded ? getParentDemandElements().at(0)->getAttributeDouble(SUMO_ATTR_ARRIVALPOS) : -1;
        // declare path geometry
        GUIGeometry routeGeometry;
        // update pathGeometry depending of first and last segment
        if (segment->isFirstSegment() && segment->isLastSegment()) {
            routeGeometry.updateGeometry(lane->getLaneGeometry().getShape(),
                                         geometryDepartPos, geometryEndPos,      // extrem positions
                                         Position::INVALID, Position::INVALID);  // extra positions
        } else if (segment->isFirstSegment()) {
            routeGeometry.updateGeometry(lane->getLaneGeometry().getShape(),
                                         geometryDepartPos, -1,                  // extrem positions
                                         Position::INVALID, Position::INVALID);  // extra positions
        } else if (segment->isLastSegment()) {
            routeGeometry.updateGeometry(lane->getLaneGeometry().getShape(),
                                         -1, geometryEndPos,                     // extrem positions
                                         Position::INVALID, Position::INVALID);  // extra positions
        } else {
            routeGeometry = lane->getLaneGeometry();
        }
        // obtain color
        const RGBColor routeColor = drawUsingSelectColor() ? s.colorSettings.selectedRouteColor : getColor();
        // Start drawing adding an gl identificator
        GLHelper::pushName(getGlID());
        // Add a draw matrix
        GLHelper::pushMatrix();
        // Start with the drawing of the area traslating matrix to origin
        myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, getType(), offsetFront + (embedded ? 0.1 : 0));
        // Set color
        GLHelper::setColor(routeColor);
        // draw route geometry
        GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), routeGeometry, routeWidth);
        // Pop last matrix
        GLHelper::popMatrix();
        // Draw name if isn't being drawn for selecting
        if (!s.drawForRectangleSelection) {
            drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
        }
        // Pop name
        GLHelper::popName();
        // check if we have to draw a red line to the next segment
        if (segment->getNextSegment()) {
            // push draw matrix
            GLHelper::pushMatrix();
            // Start with the drawing of the area traslating matrix to origin
            myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, getType());
            // Set red color
            GLHelper::setColor(RGBColor::RED);
            // get firstPosition (last position of current lane shape)
            const Position firstPosition = lane->getLaneShape().back();
            // get lastPosition (first position of next lane shape)
            const Position arrivalPos = segment->getNextSegment()->getPathElement()->getPathElementArrivalPos();
            // draw box line
            GLHelper::drawBoxLine(arrivalPos,
                                  RAD2DEG(firstPosition.angleTo2D(arrivalPos)) - 90,
                                  firstPosition.distanceTo2D(arrivalPos), .05);
            // pop draw matrix
            GLHelper::popMatrix();
        }
        // check if shape dotted contour has to be drawn
        if (dottedElement) {
            // declare trim geometry to draw
            const auto shape = (segment->isFirstSegment() || segment->isLastSegment() ? routeGeometry.getShape() : lane->getLaneShape());
            // draw inspected dotted contour
            if (myNet->getViewNet()->isAttributeCarrierInspected(this)) {
                GUIDottedGeometry::drawDottedContourShape(GUIDottedGeometry::DottedContourType::INSPECT, s, shape, routeWidth, 1, segment->isFirstSegment(), segment->isLastSegment());
            }
            // draw front dotted contour
            if ((myNet->getViewNet()->getFrontAttributeCarrier() == this)) {
                GUIDottedGeometry::drawDottedContourShape(GUIDottedGeometry::DottedContourType::FRONT, s, shape, routeWidth, 1, segment->isFirstSegment(), segment->isLastSegment());
            }
        }
    }
}


void
GNERoute::drawPartialGL(const GUIVisualizationSettings& s, const GNELane* fromLane, const GNELane* toLane, const GNEPathManager::Segment* /*segment*/, const double offsetFront) const {
    // get inspected and front flags
    const bool dottedElement = myNet->getViewNet()->isAttributeCarrierInspected(this) || (myNet->getViewNet()->getFrontAttributeCarrier() == this);
    // check conditions
    if (myNet->getViewNet()->getNetworkViewOptions().showDemandElements() &&
            myNet->getViewNet()->getDataViewOptions().showDemandElements() &&
            fromLane->getLane2laneConnections().exist(toLane) &&
            myNet->getViewNet()->getDemandViewOptions().showNonInspectedDemandElements(this) &&
            myNet->getPathManager()->getPathDraw()->drawPathGeometry(dottedElement, fromLane, toLane, myTagProperty.getTag())) {
        // get embedded route flag
        const bool embedded = (myTagProperty.getTag() == GNE_TAG_ROUTE_EMBEDDED);
        // get route width
        const double routeWidth = getExaggeration(s) * (embedded ? s.widthSettings.embeddedRouteWidth : s.widthSettings.routeWidth);
        // obtain lane2lane geometry
        const GUIGeometry& lane2laneGeometry = fromLane->getLane2laneConnections().getLane2laneGeometry(toLane);
        // obtain color
        const RGBColor routeColor = drawUsingSelectColor() ? s.colorSettings.selectedRouteColor : getColor();
        // Start drawing adding an gl identificator
        GLHelper::pushName(getGlID());
        // Add a draw matrix
        GLHelper::pushMatrix();
        // Start with the drawing of the area traslating matrix to origin
        myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, getType(), offsetFront + (embedded ? 0.1 : 0));
        // Set color
        GLHelper::setColor(routeColor);
        // draw lane2lane
        GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), lane2laneGeometry, routeWidth);
        // Pop last matrix
        GLHelper::popMatrix();
        // Pop name
        GLHelper::popName();
        // draw lock icon
        GNEViewNetHelper::LockIcon::drawLockIcon(this, getType(), getPositionInView(), getExaggeration(s));
        // check if shape dotted contour has to be drawn
        if (dottedElement) {
            // check if exist lane2lane connection
            if (fromLane->getLane2laneConnections().exist(toLane)) {
                // draw inspected dotted contour
                if (myNet->getViewNet()->isAttributeCarrierInspected(this)) {
                    GUIDottedGeometry::drawDottedContourShape(GUIDottedGeometry::DottedContourType::INSPECT, s, fromLane->getLane2laneConnections().getLane2laneGeometry(toLane).getShape(),
                            routeWidth, 1, false, false);
                }
                // draw front dotted contour
                if ((myNet->getViewNet()->getFrontAttributeCarrier() == this)) {
                    GUIDottedGeometry::drawDottedContourShape(GUIDottedGeometry::DottedContourType::FRONT, s, fromLane->getLane2laneConnections().getLane2laneGeometry(toLane).getShape(),
                            routeWidth, 1, false, false);
                }
            }
        }
    }
}


GNELane*
GNERoute::getFirstPathLane() const {
    if (myTagProperty.getTag() == SUMO_TAG_ROUTE) {
        return getParentEdges().front()->getLaneByAllowedVClass(SVC_PASSENGER);
    } else {
        return getParentDemandElements().at(0)->getFirstPathLane();
    }
}


GNELane*
GNERoute::getLastPathLane() const {
    if (myTagProperty.getTag() == SUMO_TAG_ROUTE) {
        return getParentEdges().back()->getLaneByAllowedVClass(SVC_PASSENGER);
    } else {
        return getParentDemandElements().at(0)->getLastPathLane();
    }
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
        case SUMO_ATTR_REPEAT:
            return toString(myRepeat);
        case SUMO_ATTR_CYCLETIME:
            return time2string(myCycleTime);
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARAMETERS:
            return getParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNERoute::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_DEPARTPOS:
            return 0;
        case SUMO_ATTR_ARRIVALPOS:
            return getParentEdges().back()->getLanes().front()->getLaneShape().length2D();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


Position
GNERoute::getAttributePosition(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_DEPARTPOS:
            return getParentEdges().front()->getLanes().front()->getLaneShape().front();
        case SUMO_ATTR_ARRIVALPOS:
            return getParentEdges().back()->getLanes().front()->getLaneShape().back();
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
        case SUMO_ATTR_COLOR:
        case SUMO_ATTR_REPEAT:
        case SUMO_ATTR_CYCLETIME:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
            undoList->changeAttribute(new GNEChange_Attribute(this, key, value));
            break;
        // special case due depart and arrival edge vehicles
        case SUMO_ATTR_EDGES: {
            // extract all vehicle childrens
            std::vector<GNEDemandElement*> vehicles;
            for (const auto& childDemandElement : getChildDemandElements()) {
                if (childDemandElement->getTagProperty().isVehicle()) {
                    vehicles.push_back(childDemandElement);
                }
            }
            // check vehicles
            if (vehicles.size() > 0) {
                undoList->begin(GUIIcon::ROUTE, "reset start and end edges");
                for (const auto& vehicle : vehicles) {
                    undoList->changeAttribute(new GNEChange_Attribute(vehicle, SUMO_ATTR_DEPARTEDGE, ""));
                    undoList->changeAttribute(new GNEChange_Attribute(vehicle, SUMO_ATTR_ARRIVALEDGE, ""));
                }
                undoList->changeAttribute(new GNEChange_Attribute(this, key, value));
                undoList->end();
            } else if (myTagProperty.getTag() == GNE_TAG_ROUTE_EMBEDDED) {
                undoList->begin(GUIIcon::ROUTE, "reset start and end edges");
                undoList->changeAttribute(new GNEChange_Attribute(getParentDemandElements().front(), SUMO_ATTR_DEPARTEDGE, ""));
                undoList->changeAttribute(new GNEChange_Attribute(getParentDemandElements().front(), SUMO_ATTR_ARRIVALEDGE, ""));
                undoList->changeAttribute(new GNEChange_Attribute(this, key, value));
                undoList->end();
            } else {
                // just change edges
                undoList->changeAttribute(new GNEChange_Attribute(this, key, value));
            }
            break;
        }
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
        case SUMO_ATTR_REPEAT:
            return canParse<int>(value);
        case SUMO_ATTR_CYCLETIME:
            if (canParse<double>(value)) {
                return (parse<double>(value) >= 0);
            } else {
                return false;
            }
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


const std::map<std::string, std::string>&
GNERoute::getACParametersMap() const {
    return getParametersMap();
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
            const std::vector<GNEEdge*>& outgoingEdges = currentEdge->getToJunction()->getGNEOutgoingEdges();
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
            // update microsimID
            setMicrosimID(value);
            break;
        case SUMO_ATTR_EDGES:
            replaceDemandParentEdges(value);
            // compute route
            computePathElement();
            break;
        case SUMO_ATTR_COLOR:
            myColor = parse<RGBColor>(value);
            break;
        case SUMO_ATTR_REPEAT:
            myRepeat = parse<int>(value);
            break;
        case SUMO_ATTR_CYCLETIME:
            myCycleTime = string2time(value);
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
GNERoute::toogleAttribute(SumoXMLAttr /*key*/, const bool /*value*/) {
    throw InvalidArgument("Nothing to enable");
}


void
GNERoute::setMoveShape(const GNEMoveResult& /*moveResult*/) {
    // routes cannot be moved
}


void
GNERoute::commitMoveShape(const GNEMoveResult& /*moveResult*/, GNEUndoList* /*undoList*/) {
    // routes cannot be moved
}

/****************************************************************************/
