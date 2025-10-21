/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/changes/GNEChange_DemandElement.h>
#include <netedit/frames/demand/GNEVehicleFrame.h>
#include <netedit/frames/GNETagSelector.h>
#include <netedit/GNENet.h>
#include <netedit/GNESegment.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNERoute.h"
#include "GNEVehicle.h"

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

GNERoute::GNERoutePopupMenu::GNERoutePopupMenu(GUIMainWindow& app, GUISUMOAbstractView& parent, GUIGlObject* o) :
    GUIGLObjectPopupMenu(app, parent, o) {
}


GNERoute::GNERoutePopupMenu::~GNERoutePopupMenu() {}


long
GNERoute::GNERoutePopupMenu::onCmdApplyDistance(FXObject*, FXSelector, void*) {
    GNERoute* route = static_cast<GNERoute*>(myObject);
    GNEUndoList* undoList = route->myNet->getViewNet()->getUndoList();
    undoList->begin(route, "apply distance along route");
    double dist = (route->getParentEdges().size() > 0) ? route->getParentEdges().front()->getNBEdge()->getDistance() : 0;
    for (GNEEdge* edge : route->getParentEdges()) {
        GNEChange_Attribute::changeAttribute(edge, SUMO_ATTR_DISTANCE, toString(dist), edge->getAttribute(SUMO_ATTR_DISTANCE), undoList);
        dist += edge->getNBEdge()->getFinalLength();
    }
    undoList->end();
    return 1;
}

// ===========================================================================
// GNERoute - methods
// ===========================================================================

GNERoute::GNERoute(SumoXMLTag tag, GNENet* net) :
    GNEDemandElement("", net, "", tag, GNEPathElement::Options::DEMAND_ELEMENT | GNEPathElement::Options::ROUTE) {
}


GNERoute::GNERoute(GNEAdditional* calibrator) :
    GNEDemandElement(calibrator->getNet()->getAttributeCarriers()->generateDemandElementID(SUMO_TAG_ROUTE), calibrator->getNet(),
                     calibrator->getFilename(), SUMO_TAG_ROUTE, GNEPathElement::Options::DEMAND_ELEMENT | GNEPathElement::Options::ROUTE) {
    // set parent edge
    if (calibrator->getParentEdges().size() > 0) {
        setParents<GNEEdge*>({calibrator->getParentEdges().front()});
    } else if (calibrator->getParentLanes().size() > 0) {
        setParents<GNEEdge*>({calibrator->getParentLanes().front()->getParentEdge()});
    } else {
        throw InvalidArgument("Calibrator parent requieres at least one edge or one lane");
    }
}


GNERoute::GNERoute(const std::string& id, const GNEDemandElement* originalRoute) :
    GNEDemandElement(id, originalRoute->getNet(), originalRoute->getFilename(), originalRoute->getTagProperty()->getTag(),
                     originalRoute->getPathElementOptions()),
    Parameterised(originalRoute->getParameters()->getParametersMap()),
    myRepeat(parse<int>(originalRoute->getAttribute(SUMO_ATTR_REPEAT))),
    myCycleTime(string2time(originalRoute->getAttribute(SUMO_ATTR_REPEAT))),
    myVClass(originalRoute->getVClass()) {
    // set parents
    setParents<GNEEdge*>(originalRoute->getParentEdges());
    setAttribute(SUMO_ATTR_COLOR, originalRoute->getAttribute(SUMO_ATTR_COLOR));
}


GNERoute::GNERoute(GNEVehicle* vehicleParent, const GNEDemandElement* originalRoute) :
    GNEDemandElement(vehicleParent, originalRoute->getTagProperty()->getTag(), originalRoute->getPathElementOptions()),
    Parameterised(originalRoute->getParameters()->getParametersMap()),
    myRepeat(parse<int>(originalRoute->getAttribute(SUMO_ATTR_REPEAT))),
    myCycleTime(string2time(originalRoute->getAttribute(SUMO_ATTR_REPEAT))),
    myVClass(originalRoute->getVClass()) {
    // set parents
    setParents<GNEEdge*>(originalRoute->getParentEdges());
    setParent<GNEDemandElement*>(vehicleParent);
    setAttribute(SUMO_ATTR_COLOR, originalRoute->getAttribute(SUMO_ATTR_COLOR));
}


GNERoute::GNERoute(const std::string& id, GNENet* net, const std::string& filename, SUMOVehicleClass vClass,
                   const std::vector<GNEEdge*>& edges, const RGBColor& color, const int repeat,
                   const SUMOTime cycleTime, const Parameterised::Map& parameters) :
    GNEDemandElement(id, net, filename, SUMO_TAG_ROUTE,
                     GNEPathElement::Options::DEMAND_ELEMENT | GNEPathElement::Options::ROUTE),
    Parameterised(parameters),
    myColor(color),
    myRepeat(repeat),
    myCycleTime(cycleTime),
    myVClass(vClass) {
    // set parents
    setParents<GNEEdge*>(edges);
}


GNERoute::GNERoute(GNEDemandElement* vehicleParent, const std::vector<GNEEdge*>& edges, const RGBColor& color,
                   const int repeat, const SUMOTime cycleTime, const Parameterised::Map& parameters) :
    GNEDemandElement(vehicleParent, GNE_TAG_ROUTE_EMBEDDED,
                     GNEPathElement::Options::DEMAND_ELEMENT | GNEPathElement::Options::ROUTE),
    Parameterised(parameters),
    myColor(color),
    myRepeat(repeat),
    myCycleTime(cycleTime),
    myVClass(vehicleParent->getVClass()) {
    // set parents
    setParents<GNEEdge*>(edges);
    setParent<GNEDemandElement*>(vehicleParent);
}


GNERoute::~GNERoute() {}


GNEMoveElement* GNERoute::getMoveElement() const {
    return nullptr;
}


Parameterised*
GNERoute::getParameters() {
    return this;
}


const Parameterised*
GNERoute::getParameters() const {
    return this;
}


GUIGLObjectPopupMenu*
GNERoute::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    // create popup
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, this);
    // build common options
    buildPopUpMenuCommonOptions(ret, app, myNet->getViewNet(), myTagProperty->getTag(), mySelected);
    // show option to open demand element dialog
    if (myTagProperty->hasDialog()) {
        GUIDesigns::buildFXMenuCommand(ret, "Open " + getTagStr() + " Dialog", getACIcon(), &parent, MID_OPEN_ADDITIONAL_DIALOG);
        new FXMenuSeparator(ret);
    }
    GUIDesigns::buildFXMenuCommand(ret, "Cursor position in view: " + toString(getPositionInView().x()) + "," + toString(getPositionInView().y()), nullptr, nullptr, 0);
    new FXMenuSeparator(ret);
    GUIDesigns::buildFXMenuCommand(ret, "Apply distance along route", nullptr, ret, MID_GNE_ROUTE_APPLY_DISTANCE);
    // route length
    buildMenuCommandRouteLength(ret);
    if (myNet->getViewNet()->getEditModes().isCurrentSupermodeDemand()) {
        // add reverse
        buildMenuAddReverse(ret);
    }
    return ret;
}


void
GNERoute::writeDemandElement(OutputDevice& device) const {
    device.openTag(SUMO_TAG_ROUTE);
    // write id only for non-embedded routes
    if (myTagProperty->getTag() == SUMO_TAG_ROUTE) {
        device.writeAttr(SUMO_ATTR_ID, getID());
    }
    device.writeAttr(SUMO_ATTR_EDGES, parseIDs(getParentEdges()));
    if (myColor != myTagProperty->getDefaultColorValue(SUMO_ATTR_COLOR)) {
        device.writeAttr(SUMO_ATTR_COLOR, toString(myColor));
    }
    if (myRepeat != myTagProperty->getDefaultDoubleValue(SUMO_ATTR_REPEAT)) {
        device.writeAttr(SUMO_ATTR_REPEAT, toString(myRepeat));
    }
    if (myCycleTime != myTagProperty->getDefaultTimeValue(SUMO_ATTR_CYCLETIME)) {
        device.writeAttr(SUMO_ATTR_CYCLETIME, time2string(myCycleTime));
    }
    // write probability if we have exactly one routeRef
    std::vector<GNEDemandElement*> refs;
    for (const auto& routeChild : getChildDemandElements()) {
        if (routeChild->getTagProperty()->getTag() == GNE_TAG_ROUTEREF) {
            refs.push_back(routeChild);
        }
    }
    if (refs.size() == 1) {
        device.writeAttr(SUMO_ATTR_PROB, refs.front()->getAttribute(SUMO_ATTR_PROB));
    }
    // write sorted stops
    if (myTagProperty->getTag() == SUMO_TAG_ROUTE) {
        // write stops
        for (const auto& routeChild : getChildDemandElements()) {
            if (routeChild->getTagProperty()->isVehicleStop()) {
                routeChild->writeDemandElement(device);
            }
        }
    }
    // write parameters
    writeParams(device);
    // close tag
    device.closeTag();
}


GNEDemandElement::Problem
GNERoute::isDemandElementValid() const {
    // get sorted stops and check number
    std::vector<GNEDemandElement*> stops;
    for (const auto& routeChild : getChildDemandElements()) {
        if (routeChild->getTagProperty()->isVehicleStop()) {
            stops.push_back(routeChild);
        }
    }
    // check stops
    if (getInvalidStops().size() > 0) {
        return Problem::STOP_DOWNSTREAM;
    }
    // check repeating
    if (myRepeat > 0) {
        // avoid repeat in routes with only one edge
        if (getParentEdges().size() == 1) {
            return Problem::REPEATEDROUTE_DISCONNECTED;
        }
        // check if front and last routes are connected
        if (isRouteValid({getParentEdges().back(), getParentEdges().front()}).size() > 0) {
            return Problem::REPEATEDROUTE_DISCONNECTED;
        }
    }
    // check that exist a connection between every edge
    if (isRouteValid(getParentEdges()).size() > 0) {
        return Problem::INVALID_PATH;
    } else {
        return Problem::OK;
    }
    return Problem::INVALID_ELEMENT;
}


std::string
GNERoute::getDemandElementProblem() const {
    // get sorted stops and check number
    std::vector<GNEDemandElement*> stops;
    for (const auto& routeChild : getChildDemandElements()) {
        if (routeChild->getTagProperty()->isVehicleStop()) {
            stops.push_back(routeChild);
        }
    }
    const auto invalidStops = getInvalidStops();
    if (invalidStops.size() > 0) {
        return toString(invalidStops.size()) + " stops are outside of route (downstream)";
    }
    // check repeating
    if (myRepeat > 0) {
        // avoid repeat in routes with only one edge
        if (getParentEdges().size() == 1) {
            return TL("Cannot repeat in routes with only one edge");
        }
        // check if front and last routes is connected
        if (isRouteValid({getParentEdges().back(), getParentEdges().front()}).size() > 0) {
            return TL("Cannot repeat route; front and last edge aren't connected");
        }
    }
    // return string with the problem obtained from isRouteValid
    return isRouteValid(getParentEdges());
}


void
GNERoute::fixDemandElementProblem() {
    // currently the only solution is removing Route
}


SUMOVehicleClass
GNERoute::getVClass() const {
    if (myTagProperty->getTag() == GNE_TAG_ROUTE_EMBEDDED) {
        return getParentDemandElements().at(0)->getVClass();
    } else {
        return myVClass;
    }
}


const RGBColor&
GNERoute::getColor() const {
    if (myColor != RGBColor::INVISIBLE) {
        return myColor;
    } else if ((getParentDemandElements().size() > 0) && (getParentDemandElements().front()->getColor() != RGBColor::INVISIBLE)) {
        return getParentDemandElements().front()->getColor();
    } else if ((getChildDemandElements().size() > 0) && (getChildDemandElements().front()->getColor() != RGBColor::INVISIBLE)) {
        return getChildDemandElements().front()->getColor();
    } else {
        return RGBColor::YELLOW;
    }
}


void
GNERoute::updateGeometry() {
    // compute geometry
    computePathElement();
    // update child demand elements
    for (const auto& demandElement : getChildDemandElements()) {
        if (!demandElement->getTagProperty()->isVehicleStop()) {
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
    // Routes are drawn in drawLanePartialGL and drawJunctionPartialGL
}


void
GNERoute::computePathElement() {
    // calculate path depending if is embedded
    if (myTagProperty->getTag() == GNE_TAG_ROUTE_EMBEDDED) {
        myNet->getDemandPathManager()->calculateConsecutivePathEdges(this, getVClass(), getParentEdges(),
                (int)getParentDemandElements().at(0)->getAttributeDouble(SUMO_ATTR_DEPARTLANE),
                (int)getParentDemandElements().at(0)->getAttributeDouble(SUMO_ATTR_ARRIVALLANE));
    } else {
        myNet->getDemandPathManager()->calculateConsecutivePathEdges(this, SVC_PASSENGER, getParentEdges());
    }
    // if path is empty, then calculate path again using SVC_IGNORING
    if (!myNet->getDemandPathManager()->isPathValid(this)) {
        myNet->getDemandPathManager()->calculateConsecutivePathEdges(this, SVC_IGNORING, getParentEdges());
    }
}


void
GNERoute::drawLanePartialGL(const GUIVisualizationSettings& s, const GNESegment* segment, const double offsetFront) const {
    // check conditions
    if (segment->getLane() && myNet->getViewNet()->getNetworkViewOptions().showDemandElements() && myNet->getViewNet()->getDataViewOptions().showDemandElements() &&
            myNet->getViewNet()->getDemandViewOptions().showNonInspectedDemandElements(this) &&
            myNet->getDemandPathManager()->getPathDraw()->checkDrawPathGeometry(s, segment->getLane(), myTagProperty->getTag(), false) &&
            checkCreatingVehicleOverRoute()) {
        // get exaggeration
        const double exaggeration = getExaggeration(s);
        // get detail level
        const auto d = s.getDetailLevel(exaggeration);
        // get embedded route flag
        const bool embedded = (myTagProperty->getTag() == GNE_TAG_ROUTE_EMBEDDED);
        // get route width
        const double routeWidth = embedded ? s.widthSettings.embeddedRouteWidth : s.widthSettings.routeWidth;
        // calculate startPos
        const double geometryDepartPos = embedded ? (getParentDemandElements().at(0)->getAttributeDouble(SUMO_ATTR_DEPARTPOS) + getParentDemandElements().at(0)->getParentDemandElements().at(0)->getAttributeDouble(SUMO_ATTR_LENGTH)) : -1;
        // get endPos
        const double geometryEndPos = embedded ? getParentDemandElements().at(0)->getAttributeDouble(SUMO_ATTR_ARRIVALPOS) : -1;
        // declare path geometry
        GUIGeometry routeGeometry;
        // update pathGeometry depending of first and last segment
        if (segment->isFirstSegment() && segment->isLastSegment()) {
            routeGeometry.updateGeometry(segment->getLane()->getLaneGeometry().getShape(),
                                         geometryDepartPos,
                                         Position::INVALID,
                                         geometryEndPos,
                                         Position::INVALID);
        } else if (segment->isFirstSegment()) {
            routeGeometry.updateGeometry(segment->getLane()->getLaneGeometry().getShape(),
                                         geometryDepartPos,
                                         Position::INVALID,
                                         -1,
                                         Position::INVALID);
        } else if (segment->isLastSegment()) {
            routeGeometry.updateGeometry(segment->getLane()->getLaneGeometry().getShape(),
                                         -1,
                                         Position::INVALID,
                                         geometryEndPos,
                                         Position::INVALID);
        } else {
            routeGeometry = segment->getLane()->getLaneGeometry();
        }
        // draw geometry only if we'rent in drawForObjectUnderCursor mode
        if (s.checkDrawVehicle(d, isAttributeCarrierSelected())) {
            // draw route partial lane
            drawRoutePartialLane(s, d, segment, offsetFront, routeGeometry, exaggeration);
            // draw name
            if (myTagProperty->getTag() == SUMO_TAG_ROUTE) {
                drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
            }
            // draw dotted contour
            segment->getContour()->drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidth, true);
            // show index over every edge
            if (s.showRouteIndex && myNet->getViewNet()->getEditModes().isCurrentSupermodeDemand() &&
                    myNet->getViewNet()->getInspectedElements().isACInspected(this)) {
                const double textSize = s.vehicleName.size / s.scale;
                std::string label = toString(segment->getLaneIndex());
                Position pos = segment->getLane()->getLaneShape().front() - Position(0, textSize * 1);
                // use layer above all demand elements
                GLHelper::drawTextSettings(s.vehicleName, label, pos, s.scale, s.angle, GLO_VEHICLELABELS);
            }
        }
        // calculate contour
        segment->getContour()->calculateContourExtrudedShape(s, d, this, routeGeometry.getShape(), getType(), routeWidth, exaggeration,
                segment->isFirstSegment(), segment->isLastSegment(), 0, segment, segment->getLane()->getParentEdge());
        // check if add this path element to redraw buffer
        if (!gViewObjectsHandler.isPathElementMarkForRedraw(this) && segment->getContour()->checkDrawPathContour(s, d, this)) {
            gViewObjectsHandler.addToRedrawPathElements(this);
        }
    }
}


void
GNERoute::drawJunctionPartialGL(const GUIVisualizationSettings& s, const GNESegment* segment, const double offsetFront) const {
    // check conditions
    if (myNet->getViewNet()->getNetworkViewOptions().showDemandElements() && myNet->getViewNet()->getDataViewOptions().showDemandElements() &&
            myNet->getViewNet()->getDemandViewOptions().showNonInspectedDemandElements(this) &&
            myNet->getDemandPathManager()->getPathDraw()->checkDrawPathGeometry(s, segment, myTagProperty->getTag(), false) &&
            checkCreatingVehicleOverRoute()) {
        // Obtain exaggeration of the draw
        const double routeExaggeration = getExaggeration(s);
        // get detail level
        const auto d = s.getDetailLevel(routeExaggeration);
        // get route width
        const double routeWidth = (myTagProperty->getTag() == GNE_TAG_ROUTE_EMBEDDED) ? s.widthSettings.embeddedRouteWidth : s.widthSettings.routeWidth;
        // check if connection to next lane exist
        const bool connectionExist = segment->getPreviousLane()->getLane2laneConnections().exist(segment->getNextLane());
        // get geometry
        const GUIGeometry& routeGeometry = connectionExist ? segment->getPreviousLane()->getLane2laneConnections().getLane2laneGeometry(segment->getNextLane()) :
                                           GUIGeometry({segment->getPreviousLane()->getLaneShape().back(), segment->getNextLane()->getLaneShape().front()});
        // draw geometry only if we'rent in drawForObjectUnderCursor mode
        if (s.checkDrawVehicle(d, isAttributeCarrierSelected())) {
            // draw route partial
            drawRoutePartialJunction(s, d, offsetFront, routeGeometry, routeExaggeration);
            // draw dotted contour
            segment->getContour()->drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidth, true);
        }
        // calculate contour
        segment->getContour()->calculateContourExtrudedShape(s, d, this, routeGeometry.getShape(), getType(), routeWidth, routeExaggeration,
                false, false, 0, segment, segment->getJunction());
        // check if add this path element to redraw buffer
        if (!gViewObjectsHandler.isPathElementMarkForRedraw(this) && segment->getContour()->checkDrawPathContour(s, d, this)) {
            gViewObjectsHandler.addToRedrawPathElements(this);
        }
    }
}


GNELane*
GNERoute::getFirstPathLane() const {
    if (myTagProperty->getTag() == SUMO_TAG_ROUTE) {
        return getParentEdges().front()->getLaneByAllowedVClass(SVC_PASSENGER);
    } else {
        return getParentDemandElements().at(0)->getFirstPathLane();
    }
}


GNELane*
GNERoute::getLastPathLane() const {
    if (myTagProperty->getTag() == SUMO_TAG_ROUTE) {
        return getParentEdges().back()->getLaneByAllowedVClass(SVC_PASSENGER);
    } else {
        return getParentDemandElements().at(0)->getLastPathLane();
    }
}


std::string
GNERoute::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_EDGES:
            return parseIDs(getParentEdges());
        case SUMO_ATTR_COLOR:
            if (myColor != RGBColor::INVISIBLE) {
                return toString(myColor);
            } else {
                return "";
            }
        case SUMO_ATTR_REPEAT:
            return toString(myRepeat);
        case SUMO_ATTR_CYCLETIME:
            return time2string(myCycleTime);
        default:
            return getCommonAttribute(key);
    }
}


double
GNERoute::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_DEPARTPOS:
            return 0;
        case SUMO_ATTR_ARRIVALPOS:
            return getParentEdges().back()->getChildLanes().front()->getLaneShape().length2D();
        default:
            return getCommonAttributeDouble(key);
    }
}


Position
GNERoute::getAttributePosition(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_DEPARTPOS:
            return getParentEdges().front()->getChildLanes().front()->getLaneShape().front();
        case SUMO_ATTR_ARRIVALPOS:
            return getParentEdges().back()->getChildLanes().front()->getLaneShape().back();
        default:
            return getCommonAttributePosition(key);
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
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        // special case due depart and arrival edge vehicles
        case SUMO_ATTR_EDGES: {
            // extract all vehicle childrens
            std::vector<GNEDemandElement*> vehicles;
            for (const auto& childDemandElement : getChildDemandElements()) {
                if (childDemandElement->getTagProperty()->isVehicle()) {
                    vehicles.push_back(childDemandElement);
                }
            }
            // check vehicles
            if (vehicles.size() > 0) {
                undoList->begin(this, "reset start and end edges");
                for (const auto& vehicle : vehicles) {
                    GNEChange_Attribute::changeAttribute(vehicle, SUMO_ATTR_DEPARTEDGE, "", undoList);
                    GNEChange_Attribute::changeAttribute(vehicle, SUMO_ATTR_ARRIVALEDGE, "", undoList);
                }
                GNEChange_Attribute::changeAttribute(this, key, value, undoList);
                undoList->end();
            } else if (myTagProperty->getTag() == GNE_TAG_ROUTE_EMBEDDED) {
                undoList->begin(this, "reset start and end edges");
                GNEChange_Attribute::changeAttribute(getParentDemandElements().front(), SUMO_ATTR_DEPARTEDGE, "", undoList);
                GNEChange_Attribute::changeAttribute(getParentDemandElements().front(), SUMO_ATTR_ARRIVALEDGE, "", undoList);
                GNEChange_Attribute::changeAttribute(this, key, value, undoList);
                undoList->end();
            } else {
                // just change edges
                GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            }
            break;
        }
        default:
            setCommonAttribute(key, value, undoList);
            break;
    }
}


bool
GNERoute::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidDemandElementID(value);
        case SUMO_ATTR_EDGES:
            if (value.empty()) {
                return false;
            } else {
                return canParse<std::vector<GNEEdge*> >(myNet, value, true);
            }
        case SUMO_ATTR_COLOR:
            if (value.empty()) {
                return true;
            } else {
                return canParse<RGBColor>(value);
            }
        case SUMO_ATTR_REPEAT:
            return canParse<int>(value) && (parse<int>(value) >= 0);
        case SUMO_ATTR_CYCLETIME:
            if (canParse<SUMOTime>(value)) {
                return (parse<SUMOTime>(value) >= 0);
            } else {
                return false;
            }
        default:
            return isCommonAttributeValid(key, value);
    }
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

GNEDemandElement*
GNERoute::copyRoute(const GNERoute* originalRoute) {
    // get net and undoList
    const auto net = originalRoute->getNet();
    auto undoList = net->getViewNet()->getUndoList();
    // generate new route ID
    const std::string newRouteID = net->getAttributeCarriers()->generateDemandElementID(SUMO_TAG_ROUTE);
    // create new route
    GNERoute* newRoute = new GNERoute(newRouteID, originalRoute);
    // add new route using undo-list
    undoList->begin(originalRoute, TLF("copy % '%'", originalRoute->getTagStr(), newRouteID));
    net->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(newRoute, true), true);
    undoList->end();
    // return new route
    return newRoute;
}

// ===========================================================================
// private
// ===========================================================================

void
GNERoute::drawRoutePartialLane(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                               const GNESegment* segment, const double offsetFront,
                               const GUIGeometry& geometry, const double exaggeration) const {
    // get route width
    const double routeWidth = (myTagProperty->getTag() == GNE_TAG_ROUTE_EMBEDDED) ? s.widthSettings.embeddedRouteWidth : s.widthSettings.routeWidth;
    // push layer matrix
    GLHelper::pushMatrix();
    // Start with the drawing of the area traslating matrix to origin
    glTranslated(0, 0, getType() + offsetFront);
    // Set color
    if (drawUsingSelectColor()) {
        GLHelper::setColor(s.colorSettings.selectedRouteColor);
    } else {
        GLHelper::setColor(getColor());
    }
    // draw geometry
    GUIGeometry::drawGeometry(d, geometry, routeWidth * exaggeration);
    // check if we have to draw a red line to the next segment
    if (segment->getNextLane()) {
        // push draw matrix
        GLHelper::pushMatrix();
        // Set red color
        GLHelper::setColor(RGBColor::RED);
        // get firstPosition (last position of current lane shape)
        const Position firstPosition = segment->getLane()->getLaneShape().back();
        // get lastPosition (first position of next lane shape)
        const Position arrivalPos = segment->getNextLane()->getLaneShape().front();
        // draw box line
        GLHelper::drawBoxLine(arrivalPos,
                              RAD2DEG(firstPosition.angleTo2D(arrivalPos)) - 90,
                              firstPosition.distanceTo2D(arrivalPos), .05);
        // pop draw matrix
        GLHelper::popMatrix();
    }
    // Pop layer matrix
    GLHelper::popMatrix();
}


void
GNERoute::drawRoutePartialJunction(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                   const double offsetFront, const GUIGeometry& geometry, const double exaggeration) const {
    const bool invalid = geometry.getShape().length() == 2;
    // get route width
    const double routeWidth = (myTagProperty->getTag() == GNE_TAG_ROUTE_EMBEDDED) ? s.widthSettings.embeddedRouteWidth : s.widthSettings.routeWidth;
    // Add a draw matrix
    GLHelper::pushMatrix();
    // Start with the drawing of the area traslating matrix to origin
    glTranslated(0, 0, getType() + offsetFront);
    // Set color of the base
    if (drawUsingSelectColor()) {
        GLHelper::setColor(s.colorSettings.selectedAdditionalColor);
    } else if (invalid) {
        GLHelper::setColor(RGBColor::RED);
    } else {
        GLHelper::setColor(getColor());
    }
    // draw geometry
    GUIGeometry::drawGeometry(d, geometry, routeWidth * exaggeration);
    // Pop last matrix
    GLHelper::popMatrix();
}


void
GNERoute::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            // update microsimID
            setDemandElementID(value);
            break;
        case SUMO_ATTR_EDGES:
            // replace parents
            replaceParentEdges(value);
            // compute route
            computePathElement();
            // update all parent and child demand elements
            for (const auto& element : getParentDemandElements()) {
                element->updateGeometry();
            }
            for (const auto& element : getChildDemandElements()) {
                element->updateGeometry();
            }
            break;
        case SUMO_ATTR_COLOR:
            if (value.empty()) {
                myColor = RGBColor::INVISIBLE;
            } else {
                myColor = parse<RGBColor>(value);
            }
            break;
        case SUMO_ATTR_REPEAT:
            if (value.empty()) {
                myRepeat = myTagProperty->getDefaultIntValue(key);
            } else {
                myRepeat = parse<int>(value);
            }
            break;
        case SUMO_ATTR_CYCLETIME:
            if (value.empty()) {
                myCycleTime = myTagProperty->getDefaultTimeValue(key);
            } else {
                myCycleTime = string2time(value);
            }
            break;
        default:
            setCommonAttribute(key, value);
            break;
    }
}


bool
GNERoute::checkCreatingVehicleOverRoute() const {
    if (myTagProperty->getTag() != GNE_TAG_ROUTE_EMBEDDED) {
        // this affect only to embedded routes
        return true;
    } else if (!myNet->getViewNet()->getEditModes().isCurrentSupermodeDemand()) {
        // only in demand mode
        return true;
    } else if (myNet->getViewNet()->getEditModes().demandEditMode != DemandEditMode::DEMAND_VEHICLE) {
        // only creating vehicles
        return true;
    } else {
        // get current template AC
        const auto templateAC = myNet->getViewNet()->getViewParent()->getVehicleFrame()->getVehicleTagSelector()->getCurrentTemplateAC();
        if (templateAC && templateAC->getTagProperty()->vehicleRoute()) {
            // we're creating a vehicle over a route, then hidde all embedded routes
            return false;
        } else {
            return true;
        }
    }
}


/****************************************************************************/
