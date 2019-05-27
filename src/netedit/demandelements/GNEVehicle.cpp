/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEVehicle.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2019
/// @version $Id$
///
// Representation of vehicles in NETEDIT
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================

#include <cmath>
#include <microsim/MSVehicle.h>
#include <microsim/devices/MSDevice_BTreceiver.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/frames/GNESelectorFrame.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIBaseVehicleHelper.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEVehicle.h"
#include "GNERouteHandler.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GNEVehicle::GNEVehiclePopupMenu) GNEVehiclePopupMenuMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_VEHICLE_TRANSFORM,   GNEVehicle::GNEVehiclePopupMenu::onCmdTransform),
};

// Object implementation
FXIMPLEMENT(GNEVehicle::GNEVehiclePopupMenu, GUIGLObjectPopupMenu, GNEVehiclePopupMenuMap, ARRAYNUMBER(GNEVehiclePopupMenuMap))

// ===========================================================================
// GNEVehicle::GNEVehiclePopupMenu
// ===========================================================================

GNEVehicle::GNEVehiclePopupMenu::GNEVehiclePopupMenu(GNEVehicle *vehicle, GUIMainWindow& app, GUISUMOAbstractView& parent) :
    GUIGLObjectPopupMenu(app, parent, *vehicle),
    myVehicle(vehicle),
    myTransformToVehicle(nullptr),
    myTransformToVehicleWithEmbeddedRoute(nullptr),
    myTransformToRouteFlow(nullptr),
    myTransformToRouteFlowWithEmbeddedRoute(nullptr),
    myTransformToTrip(nullptr),
    myTransformToFlow(nullptr) { 
    // build header
    myVehicle->buildPopupHeader(this, app);
    // build menu command for center button and copy cursor position to clipboard
    myVehicle->buildCenterPopupEntry(this);
    myVehicle->buildPositionCopyEntry(this, false);
    // buld menu commands for names
    new FXMenuCommand(this, ("Copy " + myVehicle->getTagStr() + " name to clipboard").c_str(), nullptr, this, MID_COPY_NAME);
    new FXMenuCommand(this, ("Copy " + myVehicle->getTagStr() + " typed name to clipboard").c_str(), nullptr, this, MID_COPY_TYPED_NAME);
    new FXMenuSeparator(this);
    // build selection and show parameters menu
    myVehicle->getViewNet()->buildSelectionACPopupEntry(this, myVehicle);
    myVehicle->buildShowParamsPopupEntry(this);
    // add transform functions only in demand mode
    if (myVehicle->getViewNet()->getEditModes().currentSupermode == GNE_SUPERMODE_DEMAND) {
        // Get icons
        FXIcon* vehicleIcon = GUIIconSubSys::getIcon(ICON_VEHICLE);
        FXIcon* tripIcon = GUIIconSubSys::getIcon(ICON_TRIP);
        FXIcon* routeFlowIcon= GUIIconSubSys::getIcon(ICON_ROUTEFLOW);
        FXIcon* flowIcon= GUIIconSubSys::getIcon(ICON_FLOW);
        // create menu pane for transform operations
        FXMenuPane* transformOperation = new FXMenuPane(this);
        this->insertMenuPaneChild(transformOperation);
        new FXMenuCascade(this, "transform to", nullptr, transformOperation);
        // Create menu comands for all vehicles
        myTransformToVehicle = new FXMenuCommand(transformOperation, "Vehicle", vehicleIcon, this, MID_GNE_VEHICLE_TRANSFORM);
        myTransformToVehicleWithEmbeddedRoute = new FXMenuCommand(transformOperation, "Vehicle (embedded route)", vehicleIcon, this, MID_GNE_VEHICLE_TRANSFORM);
        myTransformToRouteFlow = new FXMenuCommand(transformOperation, "RouteFlow", routeFlowIcon, this, MID_GNE_VEHICLE_TRANSFORM);
        myTransformToRouteFlowWithEmbeddedRoute = new FXMenuCommand(transformOperation, "RouteFlow (embedded route)", routeFlowIcon, this, MID_GNE_VEHICLE_TRANSFORM);
        myTransformToTrip = new FXMenuCommand(transformOperation, "Trip", tripIcon, this, MID_GNE_VEHICLE_TRANSFORM);
        myTransformToFlow = new FXMenuCommand(transformOperation, "Flow", flowIcon, this, MID_GNE_VEHICLE_TRANSFORM);
        // check what menu command has to be disabled
        if (myVehicle->getTagProperty().getTag() == SUMO_TAG_VEHICLE) {
            if (myVehicle->getDemandElementParents().size() > 1) {
                myTransformToVehicle->disable();
            } else {
                myTransformToVehicleWithEmbeddedRoute->disable();
            }
        } else if (myVehicle->getTagProperty().getTag() == SUMO_TAG_ROUTEFLOW) {
            if (myVehicle->getDemandElementParents().size() > 1) {
                myTransformToRouteFlow->disable();
            } else {
                myTransformToRouteFlowWithEmbeddedRoute->disable();
            }
        } else if (myVehicle->getTagProperty().getTag() == SUMO_TAG_TRIP) {
            myTransformToTrip->disable();
        } else if (myVehicle->getTagProperty().getTag() == SUMO_TAG_FLOW) {
            myTransformToFlow->disable();
        }
    }
}


GNEVehicle::GNEVehiclePopupMenu::~GNEVehiclePopupMenu() {}


long
GNEVehicle::GNEVehiclePopupMenu::onCmdTransform(FXObject* obj, FXSelector, void*) {
    if (obj == myTransformToVehicle) {
        GNERouteHandler::transformToVehicle(myVehicle, false);
    } else if (obj == myTransformToVehicleWithEmbeddedRoute) {
        GNERouteHandler::transformToVehicle(myVehicle, true);
    } else if (obj == myTransformToRouteFlow) { 
        GNERouteHandler::transformToRouteFlow(myVehicle, false);
    } else if (obj == myTransformToRouteFlowWithEmbeddedRoute) { 
        GNERouteHandler::transformToRouteFlow(myVehicle, true);
    } else if (obj == myTransformToTrip) { 
        GNERouteHandler::transformToTrip(myVehicle);
    } else if (obj == myTransformToFlow) { 
        GNERouteHandler::transformToFlow(myVehicle);
    }
    return 1;
}

// ===========================================================================
// member method definitions
// ===========================================================================

GNEVehicle::GNEVehicle(SumoXMLTag tag, GNEViewNet* viewNet, const std::string& vehicleID, GNEDemandElement* vehicleType, GNEDemandElement* route) :
    GNEDemandElement(vehicleID, viewNet, (tag == SUMO_TAG_ROUTEFLOW) ? GLO_ROUTEFLOW : GLO_VEHICLE, tag,
    {}, {}, {}, {}, {vehicleType, route}, {}, {}, {}, {}, {}),
    SUMOVehicleParameter() {
    // SUMOVehicleParameter ID has to be set manually
    id = vehicleID;
    // set manually vtypeID (needed for saving)
    vtypeid = vehicleType->getID();
}


GNEVehicle::GNEVehicle(GNEViewNet* viewNet, GNEDemandElement* vehicleType, GNEDemandElement* route, const SUMOVehicleParameter& vehicleParameters) :
    GNEDemandElement(vehicleParameters.id, viewNet, (vehicleParameters.tag == SUMO_TAG_ROUTEFLOW) ? GLO_ROUTEFLOW : GLO_VEHICLE, vehicleParameters.tag,
    {}, {}, {}, {}, {vehicleType, route}, {}, {}, {}, {}, {}),
    SUMOVehicleParameter(vehicleParameters) {
    // SUMOVehicleParameter ID has to be set manually
    id = vehicleParameters.id;
    // set manually vtypeID (needed for saving)
    vtypeid = vehicleType->getID();
}


GNEVehicle::GNEVehicle(GNEViewNet* viewNet, GNEDemandElement* vehicleType, const SUMOVehicleParameter& vehicleParameters) :
    GNEDemandElement(vehicleParameters.id, viewNet, (vehicleParameters.tag == SUMO_TAG_ROUTEFLOW) ? GLO_ROUTEFLOW : GLO_VEHICLE, vehicleParameters.tag,
    {}, {}, {}, {}, {vehicleType}, {}, {}, {}, {}, {}),
    SUMOVehicleParameter(vehicleParameters) {
    // SUMOVehicleParameter ID has to be set manually
    id = vehicleParameters.id;
    // reset routeid
    routeid.clear();
    // set manually vtypeID (needed for saving)
    vtypeid = vehicleType->getID();
}


GNEVehicle::GNEVehicle(SumoXMLTag tag, GNEViewNet* viewNet, const std::string& vehicleID, GNEDemandElement* vehicleType, const std::vector<GNEEdge*>& edges) :
    GNEDemandElement(vehicleID, viewNet, (tag == SUMO_TAG_FLOW) ? GLO_FLOW : GLO_TRIP, tag,
    {edges}, {}, {}, {}, {vehicleType}, {}, {}, {}, {}, {}),
    SUMOVehicleParameter() {
}


GNEVehicle::GNEVehicle(GNEViewNet* viewNet, GNEDemandElement* vehicleType, const std::vector<GNEEdge*>& edges, const SUMOVehicleParameter& vehicleParameters) :
    GNEDemandElement(vehicleParameters.id, viewNet, (vehicleParameters.tag == SUMO_TAG_FLOW) ? GLO_FLOW : GLO_TRIP, vehicleParameters.tag,
    {edges}, {}, {}, {}, {vehicleType}, {}, {}, {}, {}, {}),
    SUMOVehicleParameter(vehicleParameters) {
}


GNEVehicle::~GNEVehicle() {}


SUMOVehicleClass
GNEVehicle::getVClass() const {
    return getDemandElementParents().front()->getVClass();
}


std::string
GNEVehicle::getBegin() const {
    // obtain depart depending if is a Vehicle, trip or routeFlow
    std::string departStr;
    if ((myTagProperty.getTag() == SUMO_TAG_ROUTEFLOW) || (myTagProperty.getTag() == SUMO_TAG_FLOW)) {
        departStr = toString(depart);
    } else {
        departStr = getDepart();
    }
    // we need to handle depart as a tuple of 20 numbers (format: 000000...00<departTime>)
    departStr.reserve(20 - departStr.size());
    // add 0s at the beginning of departStr until we have 20 numbers
    for (int i = (int)departStr.size(); i < 20; i++) {
        departStr.insert(departStr.begin(), '0');
    }
    return departStr;
}


const RGBColor&
GNEVehicle::getColor() const {
    return color;
}


void
GNEVehicle::writeDemandElement(OutputDevice& device) const {
    // open tag depending of synonym
    if(myTagProperty.hasTagSynonym()) {
        write(device, OptionsCont::getOptions(), myTagProperty.getTagSynonym());
    } else {
        write(device, OptionsCont::getOptions(), myTagProperty.getTag());
    }
    // write specific attribute depeding of their tag
    if ((getDemandElementParents().size() == 2) && (myTagProperty.getTag() == SUMO_TAG_VEHICLE || myTagProperty.getTag() == SUMO_TAG_ROUTEFLOW)) {
        // write manually route
        device.writeAttr(SUMO_ATTR_ROUTE, getDemandElementParents().at(1)->getID());
    } 
    // write from, to and edge vias
    if ((myTagProperty.getTag() == SUMO_TAG_TRIP) || (myTagProperty.getTag() == SUMO_TAG_FLOW)) {
        // write manually from/to edges (it correspond to fron and back edge parents)
        device.writeAttr(SUMO_ATTR_FROM, getEdgeParents().front()->getID());
        device.writeAttr(SUMO_ATTR_TO, getEdgeParents().back()->getID());
        // only write via if there isn't empty
        if(via.size() > 0) {
            device.writeAttr(SUMO_ATTR_VIA, via);
        }
    }
    // write specific routeFlow/flow attributes
    if ((myTagProperty.getTag() == SUMO_TAG_ROUTEFLOW) || (myTagProperty.getTag() == SUMO_TAG_FLOW)) {
        // write routeFlow values depending if it was set
        if (isDisjointAttributeSet(SUMO_ATTR_END)) {
            device.writeAttr(SUMO_ATTR_END,  time2string(repetitionEnd));
        }
        if (isDisjointAttributeSet(SUMO_ATTR_NUMBER)) {
            device.writeAttr(SUMO_ATTR_NUMBER, repetitionNumber);
        }
        if (isDisjointAttributeSet(SUMO_ATTR_VEHSPERHOUR)) {
            device.writeAttr(SUMO_ATTR_VEHSPERHOUR, 3600. / STEPS2TIME(repetitionOffset));
        }
        if (isDisjointAttributeSet(SUMO_ATTR_PERIOD)) {
            device.writeAttr(SUMO_ATTR_PERIOD, time2string(repetitionOffset));
        }
        if (isDisjointAttributeSet(SUMO_ATTR_PROB)) {
            device.writeAttr(SUMO_ATTR_PROB, repetitionProbability);
        }
    }
    // write demand element childs associated to this vehicle
    for (const auto& i : getDemandElementChilds()) {
        i->writeDemandElement(device);
    }
    // close vehicle tag
    device.closeTag();
}


bool
GNEVehicle::isDemandElementValid() const {
    // only trips or flows can have problems
    if ((myTagProperty.getTag() == SUMO_TAG_TRIP) || (myTagProperty.getTag() == SUMO_TAG_FLOW)) {
        // check if exist at least a connection between every edge
        for (int i = 1; i < (int)getEdgeParents().size(); i++) {
            if (getRouteCalculatorInstance()->areEdgesConsecutives(getDemandElementParents().at(0)->getVClass(), getEdgeParents().at((int)i - 1), getEdgeParents().at(i)) == false) {
                return false;
            }
        }
        // there is connections bewteen all edges, then return true
        return true;
    } else {
        return true;
    }
}


std::string 
GNEVehicle::getDemandElementProblem() const {
    // only trips or flows can have problems
    if ((myTagProperty.getTag() == SUMO_TAG_TRIP) || (myTagProperty.getTag() == SUMO_TAG_FLOW)) {
        // check if exist at least a connection between every edge
        for (int i = 1; i < (int)getEdgeParents().size(); i++) {
            if (getRouteCalculatorInstance()->areEdgesConsecutives(getDemandElementParents().at(0)->getVClass(), getEdgeParents().at((int)i - 1), getEdgeParents().at(i)) == false) {
                return ("Edge '" + getEdgeParents().at((int)i - 1)->getID() + "' and edge '" + getEdgeParents().at(i)->getID() + "' aren't consecutives");
            }
        }
        // there is connections bewteen all edges, then all ok
        return "";
    } else {
        return "";
    }
}


void 
GNEVehicle::fixDemandElementProblem() {

}


void 
GNEVehicle::startGeometryMoving() {
    // Vehicles cannot be moved
}


void 
GNEVehicle::endGeometryMoving() {
    // Vehicles cannot be moved
}


void
GNEVehicle::moveGeometry(const Position&) {
    // Vehicles cannot be moved
}


void
GNEVehicle::commitGeometryMoving(GNEUndoList*) {
    // Vehicles cannot be moved
}


void
GNEVehicle::updateGeometry() {    
    // vehicles use the geometry of lane parent
}


Position
GNEVehicle::getPositionInView() const {
    // obtain lane depending of edited vehicle type
    GNELane* lane = nullptr;
    if (getDemandElementParents().size() == 2) {
        lane = getDemandElementParents().at(1)->getEdgeParents().at(0)->getLanes().front();
    } else if (getEdgeParents().size() > 0) {
        lane = getEdgeParents().front()->getLanes().front();
    } else if (getDemandElementChilds().size() == 1) {
        lane = getDemandElementChilds().at(0)->getEdgeParents().at(0)->getLanes().front();
    } else {
        throw ProcessError("Invalid vehicle tag");
    }
    if (lane->getGeometry().shape.length() < 2.5) {
        return lane->getGeometry().shape.front();
    } else {
        Position A = lane->getGeometry().shape.positionAtOffset(2.5);
        Position B = lane->getGeometry().shape.positionAtOffset(2.5);
        // return Middle point
        return Position((A.x() + B.x()) / 2, (A.y() + B.y()) / 2);
    }
}


GUIGLObjectPopupMenu* 
GNEVehicle::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    // return a GNEVehiclePopupMenu
    return new GNEVehiclePopupMenu(this, app, parent);
}


std::string
GNEVehicle::getParentName() const {
    if ((myTagProperty.getTag() == SUMO_TAG_VEHICLE) || (myTagProperty.getTag() == SUMO_TAG_ROUTEFLOW)) {
        return getDemandElementParents().at(1)->getID();
    } else if ((myTagProperty.getTag() == SUMO_TAG_TRIP) || (myTagProperty.getTag() == SUMO_TAG_FLOW)) {
        return getEdgeParents().front()->getID();
    } else {
        throw ProcessError("Invalid vehicle tag");
    }
}


Boundary
GNEVehicle::getCenteringBoundary() const {
    Boundary vehicleBoundary;
    if (getDemandElementParents().size() == 2) {
        vehicleBoundary.add(getDemandElementParents().at(1)->getEdgeParents().at(0)->getLanes().front()->getGeometry().shape.front());
    } else if (getEdgeParents().size() > 0) {
        vehicleBoundary.add(getEdgeParents().front()->getLanes().front()->getGeometry().shape.front());
    } else {
        vehicleBoundary = Boundary(-0.1, -0.1, 0.1, 0.1);
    }
    vehicleBoundary.grow(20);
    return vehicleBoundary;
}


void
GNEVehicle::drawGL(const GUIVisualizationSettings& s) const {
    // only drawn in super mode demand
    if (myViewNet->getViewOptionsNetwork().showDemandElements() && myViewNet->getViewOptionsDemand().showNonInspectedDemandElements(this)) {
        // declare common attributes
        const double upscale = s.vehicleSize.getExaggeration(s, this);
        const double width = parse<double>(getDemandElementParents().at(0)->getAttribute(SUMO_ATTR_WIDTH));
        const double length = parse<double>(getDemandElementParents().at(0)->getAttribute(SUMO_ATTR_LENGTH));
        double vehicleSizeSquared = width * length * upscale * width * length * upscale;
        // declare a flag to check if glPushName() / glPopName() has to be added (needed due GNEEdge::drawGL(...))
        const bool pushName = (myTagProperty.getTag() != SUMO_TAG_FLOW) && (myTagProperty.getTag() != SUMO_TAG_TRIP);
        // obtain Position an rotation (it depend of their parents)
        Position vehiclePosition;
        double vehicleRotation = 0;
        if (getDemandElementParents().size() == 2) {
            // obtain position and rotation of first edge route
            vehiclePosition = getDemandElementParents().at(1)->getEdgeParents().at(0)->getLanes().front()->getGeometry().shape.front();
            vehicleRotation = getDemandElementParents().at(1)->getEdgeParents().at(0)->getLanes().front()->getGeometry().shapeRotations.front();
        } else if (getEdgeParents().size() > 0) {
            // obtain position and rotation of from route
            vehiclePosition = getEdgeParents().at(0)->getLanes().front()->getGeometry().shape.front();
            vehicleRotation = getEdgeParents().at(0)->getLanes().front()->getGeometry().shapeRotations.front();
        } else if (getDemandElementChilds().size() > 0) {
            // obtain position and rotation of embedded route
            vehiclePosition = getDemandElementChilds().at(0)->getEdgeParents().at(0)->getLanes().front()->getGeometry().shape.front();
            vehicleRotation = getDemandElementChilds().at(0)->getEdgeParents().at(0)->getLanes().front()->getGeometry().shapeRotations.front();
        }
        // first check if if mouse is enought near to this vehicle to draw it
        if (s.drawForSelecting && (myViewNet->getPositionInformation().distanceSquaredTo2D(vehiclePosition) >= (vehicleSizeSquared + 2))) {
            // first push name
            if (pushName) {
                glPushName(getGlID());
            }
            // push draw matrix
            glPushMatrix();
            // translate to drawing position
            glTranslated(vehiclePosition.x(), vehiclePosition.y(), GLO_ROUTE + getType() + 0.1);
            glRotated(vehicleRotation, 0, 0, 1);
            GLHelper::drawBoxLine(Position(0, 1), 0, 2, 1);
            // Pop last matrix
            glPopMatrix();
            // pop name
            if (pushName) {
                glPopName();
            }
        } else {
            SUMOVehicleShape shape = getVehicleShapeID(getDemandElementParents().at(0)->getAttribute(SUMO_ATTR_GUISHAPE));
            // first push name
            if (pushName) {
                glPushName(getGlID());
            }
            // push draw matrix
            glPushMatrix();
            // translate to drawing position
            glTranslated(vehiclePosition.x(), vehiclePosition.y(), GLO_ROUTE + getType() + 0.1);
            glRotated(vehicleRotation, 0, 0, 1);
            // set lane color
            setColor(s);
            double upscaleLength = upscale;
            if (upscale > 1 && length > 5) {
                // reduce the length/width ratio because this is not usefull at high zoom
                upscaleLength = MAX2(1.0, upscaleLength * (5 + sqrt(length - 5)) / length);
            }
            glScaled(upscale, upscaleLength, 1);
            // check if we're drawing in selecting mode
            if (s.drawForSelecting) {
                // draw vehicle as a box and don't draw the rest of details
                GUIBaseVehicleHelper::drawAction_drawVehicleAsBoxPlus(width, length);
            } else {
                // draw the vehicle
                switch (s.vehicleQuality) {
                    case 0:
                        // in "normal" mode draw vehicle as poly
                        //GUIBaseVehicleHelper::drawAction_drawVehicleAsTrianglePlus(width, length);
                        GUIBaseVehicleHelper::drawAction_drawVehicleAsPoly(s, shape, width, length);
                        break;
                    case 1:
                        GUIBaseVehicleHelper::drawAction_drawVehicleAsBoxPlus(width, length);
                        break;
                    default:
                        GUIBaseVehicleHelper::drawAction_drawVehicleAsPoly(s, shape, width, length);
                        break;
                }
                // check if min gap has to be drawn
                if (s.drawMinGap) {
                    const double minGap = -1 * parse<double>(getDemandElementParents().at(0)->getAttribute(SUMO_ATTR_MINGAP));
                    glColor3d(0., 1., 0.);
                    glBegin(GL_LINES);
                    glVertex2d(0., 0);
                    glVertex2d(0., minGap);
                    glVertex2d(-.5, minGap);
                    glVertex2d(.5, minGap);
                    glEnd();
                }
                // drawing name at GLO_MAX fails unless translating z
                glTranslated(0, MIN2(length / 2, double(5)), -getType());
                glScaled(1 / upscale, 1 / upscaleLength, 1);
                glRotated(-1 * vehicleRotation, 0, 0, 1);
                drawName(Position(0, 0), s.scale, getDemandElementParents().at(0)->getAttribute(SUMO_ATTR_GUISHAPE) == "pedestrian" ? s.personName : s.vehicleName, s.angle);
                // draw line
                if (s.vehicleName.show && line != "") {
                    glTranslated(0, 0.6 * s.vehicleName.scaledSize(s.scale), 0);
                    GLHelper::drawTextSettings(s.vehicleName, "line:" + line, Position(0, 0), s.scale, s.angle);
                }
            }
            // pop draw matrix
            glPopMatrix();
            // check if dotted contour has to be drawn
            if (!s.drawForSelecting && (myViewNet->getDottedAC() == this)) {
                GLHelper::drawShapeDottedContour(getType(), vehiclePosition, width, length, vehicleRotation, 0, length / 2);
            }
            // pop name
            if (pushName) {
                glPopName();
            }
        }
    }
}


void
GNEVehicle::selectAttributeCarrier(bool changeFlag) {
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
GNEVehicle::unselectAttributeCarrier(bool changeFlag) {
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
GNEVehicle::getAttribute(SumoXMLAttr key) const {
    // declare string error
    std::string error;
    switch (key) {
        case SUMO_ATTR_ID:
            return getDemandElementID();
        case SUMO_ATTR_TYPE:
            return getDemandElementParents().at(0)->getID();
        case SUMO_ATTR_COLOR:
            if (wasSet(VEHPARS_COLOR_SET)) {
                return toString(color);
            } else {
                return myTagProperty.getDefaultValue(SUMO_ATTR_COLOR);
            }
        case SUMO_ATTR_DEPARTLANE:
            if (wasSet(VEHPARS_DEPARTLANE_SET)) {
                return getDepartLane();
            } else {
                return myTagProperty.getDefaultValue(SUMO_ATTR_DEPARTLANE);
            }
        case SUMO_ATTR_DEPARTPOS:
            if (wasSet(VEHPARS_DEPARTPOS_SET)) {
                return getDepartPos();
            } else {
                return myTagProperty.getDefaultValue(SUMO_ATTR_DEPARTPOS);
            }
        case SUMO_ATTR_DEPARTSPEED:
            if (wasSet(VEHPARS_DEPARTSPEED_SET)) {
                return getDepartSpeed();
            } else {
                return myTagProperty.getDefaultValue(SUMO_ATTR_DEPARTSPEED);
            }
        case SUMO_ATTR_ARRIVALLANE:
            if (wasSet(VEHPARS_ARRIVALLANE_SET)) {
                return getArrivalLane();
            } else {
                return myTagProperty.getDefaultValue(SUMO_ATTR_ARRIVALLANE);
            }
        case SUMO_ATTR_ARRIVALPOS:
            if (wasSet(VEHPARS_ARRIVALPOS_SET)) {
                return getArrivalPos();
            } else {
                return myTagProperty.getDefaultValue(SUMO_ATTR_ARRIVALPOS);
            }
        case SUMO_ATTR_ARRIVALSPEED:
            if (wasSet(VEHPARS_ARRIVALSPEED_SET)) {
                return getArrivalSpeed();
            } else {
                return myTagProperty.getDefaultValue(SUMO_ATTR_ARRIVALSPEED);
            }
        case SUMO_ATTR_LINE:
            if (wasSet(VEHPARS_LINE_SET)) {
                return line;
            } else {
                return myTagProperty.getDefaultValue(SUMO_ATTR_LINE);
            }
        case SUMO_ATTR_PERSON_NUMBER:
            if (wasSet(VEHPARS_PERSON_NUMBER_SET)) {
                return toString(personNumber);
            } else {
                return myTagProperty.getDefaultValue(SUMO_ATTR_PERSON_NUMBER);
            }
        case SUMO_ATTR_CONTAINER_NUMBER:
            if (wasSet(VEHPARS_CONTAINER_NUMBER_SET)) {
                return toString(containerNumber);
            } else {
                return myTagProperty.getDefaultValue(SUMO_ATTR_CONTAINER_NUMBER);
            }
        case SUMO_ATTR_REROUTE:
            if (wasSet(VEHPARS_CONTAINER_NUMBER_SET)) {
                return "true";
            } else {
                return "false";
            }
        case SUMO_ATTR_DEPARTPOS_LAT:
            if (wasSet(VEHPARS_DEPARTPOSLAT_SET)) {
                return getDepartPosLat();
            } else {
                return myTagProperty.getDefaultValue(SUMO_ATTR_DEPARTPOS_LAT);
            }
        case SUMO_ATTR_ARRIVALPOS_LAT:
            if (wasSet(VEHPARS_ARRIVALPOSLAT_SET)) {
                return getArrivalPosLat();
            } else {
                return myTagProperty.getDefaultValue(SUMO_ATTR_ARRIVALPOS_LAT);
            }
        // Specific of vehicles
        case SUMO_ATTR_DEPART:
            return toString(depart);
        case SUMO_ATTR_ROUTE:
            if (getDemandElementParents().size() == 2) {
                return getDemandElementParents().at(1)->getID();
            } else {
                return "";
            }
        // Specific of Trips
        case SUMO_ATTR_FROM:
            return getEdgeParents().front()->getID();
        case SUMO_ATTR_TO:
            return getEdgeParents().back()->getID();
        case SUMO_ATTR_VIA:
            return toString(via);
        // Specific of routeFlows
        case SUMO_ATTR_BEGIN:
            return time2string(depart);
        case SUMO_ATTR_END:
            return time2string(repetitionEnd);
        case SUMO_ATTR_VEHSPERHOUR:
            return toString(3600 / STEPS2TIME(repetitionOffset));
        case SUMO_ATTR_PERIOD:
            return time2string(repetitionOffset);
        case SUMO_ATTR_PROB:
            return toString(repetitionProbability);
        case SUMO_ATTR_NUMBER:
            return toString(repetitionNumber);
        //
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEVehicle::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_TYPE:
        case SUMO_ATTR_COLOR:
        case SUMO_ATTR_DEPARTLANE:
        case SUMO_ATTR_DEPARTPOS:
        case SUMO_ATTR_DEPARTSPEED:
        case SUMO_ATTR_ARRIVALLANE:
        case SUMO_ATTR_ARRIVALPOS:
        case SUMO_ATTR_ARRIVALSPEED:
        case SUMO_ATTR_LINE:
        case SUMO_ATTR_PERSON_NUMBER:
        case SUMO_ATTR_CONTAINER_NUMBER:
        case SUMO_ATTR_REROUTE:
        case SUMO_ATTR_DEPARTPOS_LAT:
        case SUMO_ATTR_ARRIVALPOS_LAT:
        // Specific of vehicles
        case SUMO_ATTR_DEPART:
        case SUMO_ATTR_ROUTE:
        // Specific of Trips
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_TO:
        case SUMO_ATTR_VIA:
        //
        // Specific of routeFlows
        case SUMO_ATTR_BEGIN:
        case SUMO_ATTR_END:
        case SUMO_ATTR_NUMBER:
        case SUMO_ATTR_VEHSPERHOUR:
        case SUMO_ATTR_PERIOD:
        case SUMO_ATTR_PROB:
        //
        case GNE_ATTR_GENERIC:
        case GNE_ATTR_SELECTED:
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEVehicle::isValid(SumoXMLAttr key, const std::string& value) {
    // declare string error
    std::string error;
    switch (key) {
        case SUMO_ATTR_ID:
            // Vehicles, Trips and Flows share namespace
            if (SUMOXMLDefinitions::isValidVehicleID(value) && 
                (myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_VEHICLE, value, false) == nullptr) &&
                (myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_TRIP, value, false) == nullptr) &&
                (myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_ROUTEFLOW, value, false) == nullptr) &&
                (myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_FLOW, value, false) == nullptr)) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_TYPE:
            return SUMOXMLDefinitions::isValidTypeID(value) && (myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, value, false) != nullptr);
        case SUMO_ATTR_COLOR:
            return canParse<RGBColor>(value);
        case SUMO_ATTR_DEPARTLANE: {
            int dummyDepartLane;
            DepartLaneDefinition dummyDepartLaneProcedure;
            parseDepartLane(value, toString(SUMO_TAG_VEHICLE), id, dummyDepartLane, dummyDepartLaneProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        case SUMO_ATTR_DEPARTPOS: {
            double dummyDepartPos;
            DepartPosDefinition dummyDepartPosProcedure;
            parseDepartPos(value, toString(SUMO_TAG_VEHICLE), id, dummyDepartPos, dummyDepartPosProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        case SUMO_ATTR_DEPARTSPEED: {
            double dummyDepartSpeed;
            DepartSpeedDefinition dummyDepartSpeedProcedure;
            parseDepartSpeed(value, toString(SUMO_TAG_VEHICLE), id, dummyDepartSpeed, dummyDepartSpeedProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        case SUMO_ATTR_ARRIVALLANE: {
            int dummyArrivalLane;
            ArrivalLaneDefinition dummyArrivalLaneProcedure;
            parseArrivalLane(value, toString(SUMO_TAG_VEHICLE), id, dummyArrivalLane, dummyArrivalLaneProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        case SUMO_ATTR_ARRIVALPOS: {
            double dummyArrivalPos;
            ArrivalPosDefinition dummyArrivalPosProcedure;
            parseArrivalPos(value, toString(SUMO_TAG_VEHICLE), id, dummyArrivalPos, dummyArrivalPosProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        case SUMO_ATTR_ARRIVALSPEED: {
            double dummyArrivalSpeed;
            ArrivalSpeedDefinition dummyArrivalSpeedProcedure;
            parseArrivalSpeed(value, toString(SUMO_TAG_VEHICLE), id, dummyArrivalSpeed, dummyArrivalSpeedProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        case SUMO_ATTR_LINE:
            return true;
        case SUMO_ATTR_PERSON_NUMBER:
            return canParse<int>(value) && parse<int>(value) >= 0;
        case SUMO_ATTR_CONTAINER_NUMBER:
            return canParse<int>(value) && parse<int>(value) >= 0;
        case SUMO_ATTR_REROUTE:
            return true;    // check
        case SUMO_ATTR_DEPARTPOS_LAT: {
            double dummyDepartPosLat;
            DepartPosLatDefinition dummyDepartPosLatProcedure;
            parseDepartPosLat(value, toString(SUMO_TAG_VEHICLE), id, dummyDepartPosLat, dummyDepartPosLatProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        case SUMO_ATTR_ARRIVALPOS_LAT: {
            double dummyArrivalPosLat;
            ArrivalPosLatDefinition dummyArrivalPosLatProcedure;
            parseArrivalPosLat(value, toString(SUMO_TAG_VEHICLE), id, dummyArrivalPosLat, dummyArrivalPosLatProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        // Specific of vehicles
        case SUMO_ATTR_DEPART: {
            SUMOTime dummyDepart;
            DepartDefinition dummyDepartProcedure;
            parseDepart(value, toString(SUMO_TAG_VEHICLE), id, dummyDepart, dummyDepartProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        case SUMO_ATTR_ROUTE:
            if (getDemandElementParents().size() == 2) {
                return SUMOXMLDefinitions::isValidVehicleID(value) && (myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_ROUTE, value, false) != nullptr);
            } else {
                return true;
            }
        // Specific of Trips
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_TO:
            return SUMOXMLDefinitions::isValidNetID(value) && (myViewNet->getNet()->retrieveEdge(value, false) != nullptr);
        case SUMO_ATTR_VIA:
            if (value.empty()) {
                return true;
            } else {
                return canParse<std::vector<GNEEdge*> >(myViewNet->getNet(), value, false);
            }
        // Specific of routeFlows
        case SUMO_ATTR_BEGIN:
            if (canParse<double>(value)) {
                return (parse<double>(value) >= 0);
            } else {
                return false;
            }
        case SUMO_ATTR_END:
            if (value.empty()) {
                return true;
            } else if (canParse<double>(value)) {
                return (parse<double>(value) >= 0);
            } else {
                return false;
            }
        case SUMO_ATTR_VEHSPERHOUR:
            if (value.empty()) {
                return true;
            } else if (canParse<double>(value)) {
                return (parse<double>(value) > 0);
            } else {
                return false;
            }
        case SUMO_ATTR_PERIOD:
            if (value.empty()) {
                return true;
            } else if (canParse<double>(value)) {
                return (parse<double>(value) > 0);
            } else {
                return false;
            }
        case SUMO_ATTR_PROB:
            if (value.empty()) {
                return true;
            } else if (canParse<double>(value)) {
                return (parse<double>(value) >= 0);
            } else {
                return false;
            }
        case SUMO_ATTR_NUMBER:
            if (canParse<int>(value)) {
                return (parse<int>(value) >= 0);
            } else {
                return false;
            }
        //
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_GENERIC:
            return isGenericParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool 
GNEVehicle::isAttributeSet(SumoXMLAttr /*key*/) const {
    return true;
}


bool
GNEVehicle::isDisjointAttributeSet(const SumoXMLAttr attr) const {
    switch (attr) {
        case SUMO_ATTR_END:
            return (parametersSet & VEHPARS_END_SET) != 0;
        case SUMO_ATTR_NUMBER:
            return (parametersSet & VEHPARS_NUMBER_SET) != 0;
        case SUMO_ATTR_VEHSPERHOUR:
            return (parametersSet & VEHPARS_VPH_SET) != 0;
        case SUMO_ATTR_PERIOD:
            return (parametersSet & VEHPARS_PERIOD_SET) != 0;
        case SUMO_ATTR_PROB:
            return (parametersSet & VEHPARS_PROB_SET) != 0;
        default:
            return true;
    };
}


void
GNEVehicle::setDisjointAttribute(const SumoXMLAttr attr, GNEUndoList* undoList) {
    // obtain a copy of parameter sets
    int parametersSetCopy = parametersSet;
    // modify parametersSetCopy depending of attr
    switch (attr) {
        case SUMO_ATTR_END: {
            // give more priority to end
            parametersSetCopy = VEHPARS_END_SET | VEHPARS_NUMBER_SET;
            break;
        }
        case SUMO_ATTR_NUMBER:
            parametersSetCopy ^= VEHPARS_END_SET;
            parametersSetCopy |= VEHPARS_NUMBER_SET;
            break;
        case SUMO_ATTR_VEHSPERHOUR: {
            // give more priority to end
            if ((parametersSetCopy & VEHPARS_END_SET) && (parametersSetCopy & VEHPARS_NUMBER_SET)) {
                parametersSetCopy = VEHPARS_END_SET;
            } else if (parametersSetCopy & VEHPARS_END_SET) {
                parametersSetCopy = VEHPARS_END_SET;
            } else if (parametersSetCopy & VEHPARS_NUMBER_SET) {
                parametersSetCopy = VEHPARS_NUMBER_SET;
            }
            // set VehsPerHour
            parametersSetCopy |= VEHPARS_VPH_SET;
            break;
        }
        case SUMO_ATTR_PERIOD: {
            // give more priority to end
            if ((parametersSetCopy & VEHPARS_END_SET) && (parametersSetCopy & VEHPARS_NUMBER_SET)) {
                parametersSetCopy = VEHPARS_END_SET;
            } else if (parametersSetCopy & VEHPARS_END_SET) {
                parametersSetCopy = VEHPARS_END_SET;
            } else if (parametersSetCopy & VEHPARS_NUMBER_SET) {
                parametersSetCopy = VEHPARS_NUMBER_SET;
            }
            // set period
            parametersSetCopy |= VEHPARS_PERIOD_SET;
            break;
        }
        case SUMO_ATTR_PROB: {
            // give more priority to end
            if ((parametersSetCopy & VEHPARS_END_SET) && (parametersSetCopy & VEHPARS_NUMBER_SET)) {
                parametersSetCopy = VEHPARS_END_SET;
            } else if (parametersSetCopy & VEHPARS_END_SET) {
                parametersSetCopy = VEHPARS_END_SET;
            } else if (parametersSetCopy & VEHPARS_NUMBER_SET) {
                parametersSetCopy = VEHPARS_NUMBER_SET;
            }
            // set probability
            parametersSetCopy |= VEHPARS_PROB_SET;
            break;
        }
        default:
            break;
    }
    undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), parametersSet, parametersSetCopy));
}


std::string
GNEVehicle::getPopUpID() const {
    return getTagStr();
}


std::string
GNEVehicle::getHierarchyName() const {
    // special case for Trips and flow
    if ((myTagProperty.getTag() == SUMO_TAG_TRIP) || (myTagProperty.getTag() == SUMO_TAG_FLOW)) {
        // check if we're inspecting a Edge
        if (myViewNet->getNet()->getViewNet()->getDottedAC() && 
            myViewNet->getNet()->getViewNet()->getDottedAC()->getTagProperty().getTag() == SUMO_TAG_EDGE) {
            // check if edge correspond to a "from", "to" or "via" edge
            if (getEdgeParents().front() == myViewNet->getNet()->getViewNet()->getDottedAC()) {
                return getTagStr() + ": " + getAttribute(SUMO_ATTR_ID) + " (from)";
            } else if (getEdgeParents().front() == myViewNet->getNet()->getViewNet()->getDottedAC()) {
                return getTagStr() + ": " + getAttribute(SUMO_ATTR_ID) + " (to)";
            } else {
                // iterate over via
                for (const auto & i : via) {
                    if (i == myViewNet->getNet()->getViewNet()->getDottedAC()->getID()) {
                        return getTagStr() + ": " + getAttribute(SUMO_ATTR_ID) + " (via)";
                    }
                }
            }
        }
    }
    return getTagStr() + ": " + getAttribute(SUMO_ATTR_ID);
}

// ===========================================================================
// protected
// ===========================================================================

void
GNEVehicle::setColor(const GUIVisualizationSettings& s) const {
    // change color
    if (drawUsingSelectColor()) {
        GLHelper::setColor(s.selectedAdditionalColor);
    } else {
        // obtain vehicle color
        const GUIColorer& c = s.vehicleColorer;
        // set color depending of vehicle color active
        switch (c.getActive()) {
            case 0: {
                //test for emergency vehicle
                if (getDemandElementParents().at(0)->getAttribute(SUMO_ATTR_GUISHAPE) == "emergency") {
                    GLHelper::setColor(RGBColor::WHITE);
                    break;
                }
                //test for firebrigade
                if (getDemandElementParents().at(0)->getAttribute(SUMO_ATTR_GUISHAPE) == "firebrigade") {
                    GLHelper::setColor(RGBColor::RED);
                    break;
                }
                //test for police car
                if (getDemandElementParents().at(0)->getAttribute(SUMO_ATTR_GUISHAPE) == "police") {
                    GLHelper::setColor(RGBColor::BLUE);
                    break;
                }
                if (wasSet(VEHPARS_COLOR_SET)) {
                    GLHelper::setColor(color);
                    break;
                }
                if (getDemandElementParents().at(0)->isDisjointAttributeSet(SUMO_ATTR_COLOR)) {
                    GLHelper::setColor(getDemandElementParents().at(0)->getColor());
                    break;
                }
                if (&(getDemandElementParents().at(1)->getColor()) != &RGBColor::DEFAULT_COLOR) {
                    GLHelper::setColor(getDemandElementParents().at(1)->getColor());
                } else {
                    GLHelper::setColor(c.getScheme().getColor(0));
                }
                break;
            }
            case 2: {
                if (wasSet(VEHPARS_COLOR_SET)) {
                    GLHelper::setColor(color);
                } else {
                    GLHelper::setColor(c.getScheme().getColor(0));
                }
                break;
            }
            case 3: {
                if (getDemandElementParents().at(0)->isDisjointAttributeSet(SUMO_ATTR_COLOR)) {
                    GLHelper::setColor(getDemandElementParents().at(0)->getColor());
                } else {
                    GLHelper::setColor(c.getScheme().getColor(0));
                }
                break;
            }
            case 4: {
                if (getDemandElementParents().at(1)->getColor() != RGBColor::DEFAULT_COLOR) {
                    GLHelper::setColor(getDemandElementParents().at(1)->getColor());
                } else {
                    GLHelper::setColor(c.getScheme().getColor(0));
                }
                break;
            }
            case 5: {
                Position p = getDemandElementParents().at(1)->getEdgeParents().at(0)->getLanes().at(0)->getGeometry().shape[0];
                const Boundary& b = myViewNet->getNet()->getBoundary();
                Position center = b.getCenter();
                double hue = 180. + atan2(center.x() - p.x(), center.y() - p.y()) * 180. / M_PI;
                double sat = p.distanceTo(center) / center.distanceTo(Position(b.xmin(), b.ymin()));
                GLHelper::setColor(RGBColor::fromHSV(hue, sat, 1.));
                break;
            }
            case 6: {
                Position p = getDemandElementParents().at(1)->getEdgeParents().back()->getLanes().at(0)->getGeometry().shape[-1];
                const Boundary& b = myViewNet->getNet()->getBoundary();
                Position center = b.getCenter();
                double hue = 180. + atan2(center.x() - p.x(), center.y() - p.y()) * 180. / M_PI;
                double sat = p.distanceTo(center) / center.distanceTo(Position(b.xmin(), b.ymin()));
                GLHelper::setColor(RGBColor::fromHSV(hue, sat, 1.));
                break;
            }
            case 7: {
                Position pb = getDemandElementParents().at(1)->getEdgeParents().at(0)->getLanes().at(0)->getGeometry().shape[0];
                Position pe = getDemandElementParents().at(1)->getEdgeParents().back()->getLanes().at(0)->getGeometry().shape[-1];
                const Boundary& b = myViewNet->getNet()->getBoundary();
                double hue = 180. + atan2(pb.x() - pe.x(), pb.y() - pe.y()) * 180. / M_PI;
                Position minp(b.xmin(), b.ymin());
                Position maxp(b.xmax(), b.ymax());
                double sat = pb.distanceTo(pe) / minp.distanceTo(maxp);
                GLHelper::setColor(RGBColor::fromHSV(hue, sat, 1.));
                break;
            }
            case 29: { // color randomly (by pointer hash)
                std::hash<const GNEVehicle*> ptr_hash;
                const double hue = (double)(ptr_hash(this) % 360); // [0-360]
                const double sat = ((ptr_hash(this) / 360) % 67) / 100.0 + 0.33; // [0.33-1]
                GLHelper::setColor(RGBColor::fromHSV(hue, sat, 1.));
                break;
            }
            default: {
                GLHelper::setColor(c.getScheme().getColor(0));
            }
        }
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNEVehicle::setAttribute(SumoXMLAttr key, const std::string& value) {
    // declare string error
    std::string error;
    switch (key) {
        case SUMO_ATTR_ID:
            changeDemandElementID(value);
            break;
        case SUMO_ATTR_TYPE:
            changeDemandElementParent(this, value, 0);
            // set manually vtypeID (needed for saving)
            vtypeid = value;
            break;
        case SUMO_ATTR_COLOR:
            if (!value.empty() && (value != myTagProperty.getDefaultValue(key))) {
                color = parse<RGBColor>(value);
                // mark parameter as set
                parametersSet |= VEHPARS_COLOR_SET;
            } else {
                // set default value
                color = parse<RGBColor>(myTagProperty.getDefaultValue(key));
                // unset parameter
                parametersSet &= ~VEHPARS_COLOR_SET;
            }
            break;
        case SUMO_ATTR_DEPARTLANE:
            if (!value.empty() && (value != myTagProperty.getDefaultValue(key))) {
                parseDepartLane(value, toString(SUMO_TAG_VEHICLE), id, departLane, departLaneProcedure, error);
                // mark parameter as set
                parametersSet |= VEHPARS_DEPARTLANE_SET;
            } else {
                // set default value
                parseDepartLane(myTagProperty.getDefaultValue(key), toString(SUMO_TAG_VEHICLE), id, departLane, departLaneProcedure, error);
                // unset parameter
                parametersSet &= ~VEHPARS_DEPARTLANE_SET;
            }
            break;
        case SUMO_ATTR_DEPARTPOS:
            if (!value.empty() && (value != myTagProperty.getDefaultValue(key))) {
                parseDepartPos(value, toString(SUMO_TAG_VEHICLE), id, departPos, departPosProcedure, error);
                // mark parameter as set
                parametersSet |= VEHPARS_DEPARTPOS_SET;
            } else {
                // set default value
                parseDepartPos(myTagProperty.getDefaultValue(key), toString(SUMO_TAG_VEHICLE), id, departPos, departPosProcedure, error);
                // unset parameter
                parametersSet &= ~VEHPARS_DEPARTPOS_SET;
            }
            break;
        case SUMO_ATTR_DEPARTSPEED:
            if (!value.empty() && (value != myTagProperty.getDefaultValue(key))) {
                parseDepartSpeed(value, toString(SUMO_TAG_VEHICLE), id, departSpeed, departSpeedProcedure, error);
                // mark parameter as set
                parametersSet |= VEHPARS_DEPARTSPEED_SET;
            } else {
                // set default value
                parseDepartSpeed(myTagProperty.getDefaultValue(key), toString(SUMO_TAG_VEHICLE), id, departSpeed, departSpeedProcedure, error);
                // unset parameter
                parametersSet &= ~VEHPARS_DEPARTSPEED_SET;
            }
            break;
        case SUMO_ATTR_ARRIVALLANE:
            if (!value.empty() && (value != myTagProperty.getDefaultValue(key))) {
                parseArrivalLane(value, toString(SUMO_TAG_VEHICLE), id, arrivalLane, arrivalLaneProcedure, error);
                // mark parameter as set
                parametersSet |= VEHPARS_ARRIVALLANE_SET;
            } else {
                // set default value
                parseArrivalLane(myTagProperty.getDefaultValue(key), toString(SUMO_TAG_VEHICLE), id, arrivalLane, arrivalLaneProcedure, error);
                // unset parameter
                parametersSet &= ~VEHPARS_ARRIVALLANE_SET;
            }
            break;
        case SUMO_ATTR_ARRIVALPOS:
            if (!value.empty() && (value != myTagProperty.getDefaultValue(key))) {
                parseArrivalPos(value, toString(SUMO_TAG_VEHICLE), id, arrivalPos, arrivalPosProcedure, error);
                // mark parameter as set
                parametersSet |= VEHPARS_ARRIVALPOS_SET;
            } else {
                // set default value
                parseArrivalPos(myTagProperty.getDefaultValue(key), toString(SUMO_TAG_VEHICLE), id, arrivalPos, arrivalPosProcedure, error);
                // unset parameter
                parametersSet &= ~VEHPARS_ARRIVALPOS_SET;
            }
            break;
        case SUMO_ATTR_ARRIVALSPEED:
            if (!value.empty() && (value != myTagProperty.getDefaultValue(key))) {
                parseArrivalSpeed(value, toString(SUMO_TAG_VEHICLE), id, arrivalSpeed, arrivalSpeedProcedure, error);
                // mark parameter as set
                parametersSet |= VEHPARS_ARRIVALSPEED_SET;
            } else {
                // set default value
                parseArrivalSpeed(myTagProperty.getDefaultValue(key), toString(SUMO_TAG_VEHICLE), id, arrivalSpeed, arrivalSpeedProcedure, error);
                // unset parameter
                parametersSet &= ~VEHPARS_ARRIVALSPEED_SET;
            }
            break;
        case SUMO_ATTR_LINE:
            if (!value.empty() && (value != myTagProperty.getDefaultValue(key))) {
                line = value;
                // mark parameter as set
                parametersSet |= VEHPARS_LINE_SET;
            } else {
                // set default value
                line = myTagProperty.getDefaultValue(key);
                // unset parameter
                parametersSet &= ~VEHPARS_LINE_SET;
            }
            break;
        case SUMO_ATTR_PERSON_NUMBER:
            if (!value.empty() && (value != myTagProperty.getDefaultValue(key))) {
                personNumber = parse<int>(value);
                // mark parameter as set
                parametersSet |= VEHPARS_PERSON_NUMBER_SET;
            } else {
                // set default value
                personNumber = parse<int>(myTagProperty.getDefaultValue(key));
                // unset parameter
                parametersSet &= ~VEHPARS_PERSON_NUMBER_SET;
            }
            break;
        case SUMO_ATTR_CONTAINER_NUMBER:
            if (!value.empty() && (value != myTagProperty.getDefaultValue(key))) {
                containerNumber = parse<int>(value);
                // mark parameter as set
                parametersSet |= VEHPARS_CONTAINER_NUMBER_SET;
            } else {
                // set default value
                containerNumber = parse<int>(myTagProperty.getDefaultValue(key));
                // unset parameter
                parametersSet &= ~VEHPARS_CONTAINER_NUMBER_SET;
            }
            break;
        case SUMO_ATTR_REROUTE:
            if (!value.empty() && (value != myTagProperty.getDefaultValue(key))) {
                // mark parameter as set
                parametersSet |= VEHPARS_ROUTE_SET;
            } else {
                // unset parameter
                parametersSet &= ~VEHPARS_ROUTE_SET;
            }
            break;
        case SUMO_ATTR_DEPARTPOS_LAT:
            if (!value.empty() && (value != myTagProperty.getDefaultValue(key))) {
                parseDepartPosLat(value, toString(SUMO_TAG_VEHICLE), id, departPosLat, departPosLatProcedure, error);
                // mark parameter as set
                parametersSet |= VEHPARS_DEPARTPOSLAT_SET;
            } else {
                // set default value
                parseDepartPosLat(myTagProperty.getDefaultValue(key), toString(SUMO_TAG_VEHICLE), id, departPosLat, departPosLatProcedure, error);
                // unset parameter
                parametersSet &= ~VEHPARS_DEPARTPOSLAT_SET;
            }
            break;
        case SUMO_ATTR_ARRIVALPOS_LAT:
            if (!value.empty() && (value != myTagProperty.getDefaultValue(key))) {
                parseArrivalPosLat(value, toString(SUMO_TAG_VEHICLE), id, arrivalPosLat, arrivalPosLatProcedure, error);
                // mark parameter as set
                parametersSet |= VEHPARS_ARRIVALPOSLAT_SET;
            } else {
                // set default value
                parseArrivalPosLat(myTagProperty.getDefaultValue(key), toString(SUMO_TAG_VEHICLE), id, arrivalPosLat, arrivalPosLatProcedure, error);
                // unset parameter
                parametersSet &= ~VEHPARS_ARRIVALPOSLAT_SET;
            }
            parseArrivalPosLat(value, toString(SUMO_TAG_VEHICLE), id, arrivalPosLat, arrivalPosLatProcedure, error);
            break;
        // Specific of vehicles
        case SUMO_ATTR_DEPART: {
            std::string oldDepart = getBegin();
            parseDepart(value, toString(SUMO_TAG_VEHICLE), id, depart, departProcedure, error);
            myViewNet->getNet()->updateDemandElementBegin(oldDepart, this);
            break;
        }
        case SUMO_ATTR_ROUTE:
            if (getDemandElementParents().size() == 2) {
                changeDemandElementParent(this, value, 1);
            }
            break;
        // Specific of Trips and flow
        case SUMO_ATTR_FROM: {
            // declare a from-via-to edges vector
            std::vector<std::string> FromViaToEdges;
            // add from edge
            FromViaToEdges.push_back(value);
            // add via edges
            FromViaToEdges.insert(FromViaToEdges.end(), via.begin(), via.end());
            // add to edge
            FromViaToEdges.push_back(getEdgeParents().back()->getID());
            // calculate route
            std::vector<GNEEdge*> route = getRouteCalculatorInstance()->calculateDijkstraRoute(myViewNet->getNet(), getDemandElementParents().at(0)->getVClass(), FromViaToEdges);
            // change edge parents
            changeEdgeParents(this, toString(route));
            break;
        }
        case SUMO_ATTR_TO: {
            // declare a from-via-to edges vector
            std::vector<std::string> FromViaToEdges;
            // add from edge
            FromViaToEdges.push_back(getEdgeParents().front()->getID());
            // add via edges
            FromViaToEdges.insert(FromViaToEdges.end(), via.begin(), via.end());
            // add to edge
            FromViaToEdges.push_back(value);
            // calculate route
            std::vector<GNEEdge*> route = getRouteCalculatorInstance()->calculateDijkstraRoute(myViewNet->getNet(), getDemandElementParents().at(0)->getVClass(), FromViaToEdges);
            // change edge parents
            changeEdgeParents(this, toString(route));
            break;
        }
        case SUMO_ATTR_VIA: {
            if (!value.empty()) {
                // set new via edges
                via = parse< std::vector<std::string> >(value);
                // mark parameter as set
                parametersSet |= VEHPARS_VIA_SET;
            } else {
                // clear via
                via.clear();
                // unset parameter
                parametersSet &= ~VEHPARS_VIA_SET;
            }
            // declare a from-via-to edges vector
            std::vector<std::string> FromViaToEdges;
            // add from edge
            FromViaToEdges.push_back(getEdgeParents().front()->getID());
            // add via edges
            FromViaToEdges.insert(FromViaToEdges.end(), via.begin(), via.end());
            // add to edge
            FromViaToEdges.push_back(getEdgeParents().back()->getID());
            // calculate route
            std::vector<GNEEdge*> route = getRouteCalculatorInstance()->calculateDijkstraRoute(myViewNet->getNet(), getDemandElementParents().at(0)->getVClass(), FromViaToEdges);
            // change edge parents
            changeEdgeParents(this, toString(route));
            break;
        }
        // Specific of routeFlows
        case SUMO_ATTR_BEGIN: {
            std::string oldBegin = getBegin();
            depart = string2time(value);
            myViewNet->getNet()->updateDemandElementBegin(oldBegin, this);
            break;
        }
        case SUMO_ATTR_END:
            repetitionEnd = string2time(value);
            break;
        case SUMO_ATTR_VEHSPERHOUR:
            repetitionOffset = TIME2STEPS(3600 / parse<double>(value));
            break;
        case SUMO_ATTR_PERIOD:
            repetitionOffset = string2time(value);
            break;
        case SUMO_ATTR_PROB:
            repetitionProbability = parse<double>(value);
            break;
        case SUMO_ATTR_NUMBER:
            repetitionNumber = parse<int>(value);
            break;
        //
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
GNEVehicle::setDisjointAttribute(const int newParameterSet) {
    parametersSet = newParameterSet;
}

/****************************************************************************/
